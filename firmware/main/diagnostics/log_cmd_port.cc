/*
 * log_cmd_port.cc — 诊断日志按需 USB 串口取回的芯片面（TaskPackage
 * 261018-sd-log-usb-read）。
 *
 * 契约权威 = firmware/main/test/log_cmd.acceptance.c 文件头 [S6]（冻结，逐字）。
 * 本文件承载 host 不可达的四件事（不进 host 构建，只由结构断言扫源锚）：
 *   1) secondary 串口 VFS 直开（O_RDONLY | O_NONBLOCK，不装 usb_serial_jtag_driver、
 *      不改 sdkconfig）+ 50..100ms 轮询读 64B 块（D2）；
 *   2) 消费 kidpal_logcmd_* 纯逻辑入口（解析 / 行装配 / 帧格式化 / 尾部窗口）——
 *      决策表不得成死代码；
 *   3) FATFS 只读（rb、一次一 FD、读完即关、不 rename/remove，D4）；
 *   4) 响应只经 stdout（fwrite）——本文件全文件零 ESP_LOG：读任务内记日志会经 sd_log
 *      的 vprintf 钩子递归回灌环形缓冲（自污染 / 掉日志 / 栈溢出，D3）。
 *
 * 启动函数 KidpalLogCmdStart 函数体内零打印（无命令零输出，不干扰串口监控与烧录）。
 * 名字面冻结：LogCmdReadTask 为自由函数（结构断言依此名定位函数体），定义在 Start 之前。
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "log_cmd.h"
#include "sd_log.h"

/* 轮询周期（plan §7：50..100ms；过密挤占主链路、过疏命令响应迟钝）。 */
#define KIDPAL_LOGCMD_POLL_MS        80
/* 单次 VFS 读块大小（plan §7：64B 块）。 */
#define KIDPAL_LOGCMD_READ_BLOCK     64
/* cat 流式输出块（plan §5：1KB 块 fwrite + fflush）。 */
#define KIDPAL_LOGCMD_CAT_CHUNK      1024
/* tail 读入上限（单文件分片上限 512KB；读任务按此上限取当前文件全文）。 */
#define KIDPAL_LOGCMD_TAIL_MAX_BYTES (512u * 1024u)

/* 日志目录：板级挂载根 SD_MMC_MOUNT_ROOT("/sdcard") + "/logs"（与 sd_log_fs.cc 的
 * KidpalSdLogAttach 同形态）。板级 config.h 的挂载根不在本组件 include 面，此处以设备
 * 路径字面固化（真机轮 LUR-A7 观测承载实际可达性）。 */
#define KIDPAL_LOGCMD_LOG_DIR        "/sdcard/logs"

/* 单次响应行缓冲（list_line / list_total 装配用；契约 cap 下界 40）。 */
static char s_line[64];
/* cat 流式块（静态，避免占用 4KB 任务栈）。 */
static char s_cat_chunk[KIDPAL_LOGCMD_CAT_CHUNK];

/* 当前轮转文件路径 = "<log_dir>/KPLOGnnn.TXT"（8.3 命名单源 = kidpal_sd_log_file_name）。 */
static int BuildLogPath(unsigned idx, char *out, size_t cap)
{
    char name[KIDPAL_SD_LOG_NAME_CAP];

    if (kidpal_sd_log_file_name(idx, name, (unsigned)sizeof(name)) != 0) {
        return -1;
    }
    if (snprintf(out, cap, "%s/%s", KIDPAL_LOGCMD_LOG_DIR, name) < 0) {
        return -1;
    }
    return 0;
}

/* 常规文件字节数；不存在 / 非常规文件 -> -1。 */
static long FileSizeOf(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return -1;
    }
    return (long)st.st_size;
}

/* 错误行（帧内）："KPLOG ERROR <reason>" + '\n'。 */
static void EmitErrorLine(int err)
{
    const char *text = kidpal_logcmd_error_text(err);

    if (text == nullptr) {
        return;
    }
    fwrite(text, 1, strlen(text), stdout);
    fputc('\n', stdout);
}

/* list：逐行 KPLOGnnn.TXT <bytes>（存在者）+ KPLOG TOTAL 汇总；当前写入文件标 *。
 * 返回 0 = 成功；KIDPAL_LOGCMD_ERR_NO_CARD = 卡不在位。 */
static int EmitList(void)
{
    char path[96];
    unsigned long total = 0ul;
    unsigned count = 0u;
    unsigned idx;
    int cur = KidpalSdLogCurrentIndex();

    if (cur < 0) {
        return KIDPAL_LOGCMD_ERR_NO_CARD;
    }
    for (idx = 0u; idx < KIDPAL_LOGCMD_CAT_COUNT; idx++) {
        long size;

        if (BuildLogPath(idx, path, sizeof(path)) != 0) {
            continue;
        }
        size = FileSizeOf(path);
        if (size < 0) {
            continue;  /* 不存在的文件不列 */
        }
        if (kidpal_logcmd_list_line(s_line, (unsigned)sizeof(s_line), idx,
                                    (unsigned long)size, (int)idx == cur) == 0) {
            fwrite(s_line, 1, strlen(s_line), stdout);
            fputc('\n', stdout);
        }
        total += (unsigned long)size;
        count++;
    }
    if (kidpal_logcmd_list_total(s_line, (unsigned)sizeof(s_line), total, count) == 0) {
        fwrite(s_line, 1, strlen(s_line), stdout);
        fputc('\n', stdout);
    }
    fflush(stdout);
    return 0;
}

/* cat <idx>：指定文件全文，1KB 块流式输出。返回 0 / NO_CARD / NO_FILE。 */
static int EmitCat(unsigned idx)
{
    char path[96];
    FILE *f;
    size_t got;

    if (KidpalSdLogCurrentIndex() < 0) {
        return KIDPAL_LOGCMD_ERR_NO_CARD;
    }
    if (BuildLogPath(idx, path, sizeof(path)) != 0) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    f = fopen(path, "rb");
    if (f == nullptr) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    while ((got = fread(s_cat_chunk, 1, sizeof(s_cat_chunk), f)) > 0u) {
        fwrite(s_cat_chunk, 1, got, stdout);
    }
    fclose(f);
    fflush(stdout);
    return 0;
}

/* tail <n>：当前写入文件末 n 行（尾部窗口单源 = kidpal_logcmd_tail_window）。
 * 返回 0 / NO_CARD / NO_FILE。
 * minimal: 整文件读入一块堆缓冲（≤512KB 分片上限）再取尾部窗口 —— 实现最简；升级路径
 * = 分块流式 + 回溯定位（免大块分配），当前按 plan「诊断命令低频、文件封顶 512KB」判为
 * 不必要。malloc 失败 / 超上限退化为 NO_FILE / 零输出（帧仍完整）。 */
static int EmitTail(unsigned n)
{
    char path[96];
    FILE *f;
    char *buf;
    long size;
    int off;
    int cur = KidpalSdLogCurrentIndex();

    if (cur < 0) {
        return KIDPAL_LOGCMD_ERR_NO_CARD;
    }
    if (BuildLogPath((unsigned)cur, path, sizeof(path)) != 0) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    size = FileSizeOf(path);
    if (size <= 0 || (unsigned long)size > (unsigned long)KIDPAL_LOGCMD_TAIL_MAX_BYTES) {
        if (size < 0) {
            return KIDPAL_LOGCMD_ERR_NO_FILE;
        }
        return 0;  /* 空文件 / 超上限：零输出（帧仍完整） */
    }
    f = fopen(path, "rb");
    if (f == nullptr) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    buf = (char *)malloc((size_t)size);
    if (buf == nullptr) {
        fclose(f);
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    if (fread(buf, 1, (size_t)size, f) != (size_t)size) {
        free(buf);
        fclose(f);
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    fclose(f);
    off = kidpal_logcmd_tail_window(buf, (unsigned)size, n);
    if (off < 0) {
        off = 0;
    }
    if ((unsigned)off < (unsigned)size) {
        fwrite(buf + off, 1, (size_t)((unsigned)size - (unsigned)off), stdout);
    }
    free(buf);
    fflush(stdout);
    return 0;
}

/* 读任务（自由函数形态，名字 LogCmdReadTask 字面冻结）：secondary 串口 VFS 非阻塞轮询读
 * -> 行装配 -> 解析 -> 帧响应（stdout）。函数体内零 ESP_LOG（D3：防 vprintf 钩子递归回灌）。 */
static void LogCmdReadTask(void *arg)
{
    static char asm_storage[KIDPAL_LOGCMD_LINE_MAX + 128];
    kidpal_logcmd_asm_t *asmr;
    char line[KIDPAL_LOGCMD_LINE_MAX + 1];
    unsigned char block[KIDPAL_LOGCMD_READ_BLOCK];
    int fd = -1;

    (void)arg;
    asmr = kidpal_logcmd_asm_init(asm_storage, (unsigned)sizeof(asm_storage));
    if (asmr == nullptr) {
        vTaskDelete(nullptr);
        return;
    }
    for (;;) {
        ssize_t got;

        if (fd < 0) {
            fd = open("/dev/secondary", O_RDONLY | O_NONBLOCK);
            if (fd < 0) {
                vTaskDelay(pdMS_TO_TICKS(KIDPAL_LOGCMD_POLL_MS));
                continue;
            }
        }
        got = read(fd, block, sizeof(block));
        if (got > 0) {
            int ln;

            (void)kidpal_logcmd_asm_feed(asmr, block, (unsigned)got);
            while ((ln = kidpal_logcmd_asm_pop(asmr, line,
                                               (unsigned)sizeof(line))) > 0) {
                int cmd = 0;
                unsigned arg = 0u;
                const char *begin;
                const char *end;
                int err = 0;

                if (kidpal_logcmd_parse(line, (unsigned)ln, &cmd, &arg) != 0) {
                    continue;
                }
                begin = kidpal_logcmd_frame_begin(cmd);
                if (begin != nullptr) {
                    fwrite(begin, 1, strlen(begin), stdout);
                    fputc('\n', stdout);
                }
                switch (cmd) {
                case KIDPAL_LOGCMD_LIST:
                    err = EmitList();
                    break;
                case KIDPAL_LOGCMD_TAIL:
                    err = EmitTail(arg);
                    break;
                case KIDPAL_LOGCMD_CAT:
                    err = EmitCat(arg);
                    break;
                case KIDPAL_LOGCMD_BAD_CMD:
                    err = KIDPAL_LOGCMD_ERR_BAD_CMD;
                    break;
                case KIDPAL_LOGCMD_BAD_INDEX:
                    err = KIDPAL_LOGCMD_ERR_BAD_INDEX;
                    break;
                default:
                    break;  /* SILENT：无命令零输出 */
                }
                if (err != 0) {
                    EmitErrorLine(err);
                }
                end = kidpal_logcmd_frame_end();
                if (end != nullptr) {
                    fwrite(end, 1, strlen(end), stdout);
                    fputc('\n', stdout);
                }
                fflush(stdout);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(KIDPAL_LOGCMD_POLL_MS));
        }
    }
}

/* main.cc 在 KidpalSdLogChainPreviousVprintf(...) 之后调用（[S8]/D7）。低优先级（prio 2，
 * 写盘任务为 1）、栈 4KB；启动函数体内零打印（无命令零输出）。 */
extern "C" void KidpalLogCmdStart(void)
{
    (void)xTaskCreate(LogCmdReadTask, "logcmd", 4096, nullptr, 2, nullptr);
}
