/*
 * log_cmd_port.cc — 诊断日志按需 USB 串口取回的芯片面（TaskPackage
 * 261018-sd-log-usb-read）。
 *
 * 契约权威 = firmware/main/test/log_cmd.acceptance.c 文件头 [S6]（冻结，逐字）。
 * 本文件承载 host 不可达的四件事（不进 host 构建，只由结构断言扫源锚）：
 *   1) secondary 串口 HAL 直读（usb_serial_jtag_ll_read_rxfifo，同 USB-Serial/JTAG
 *      外设、非阻塞轮询、不装 usb_serial_jtag_driver、不改 sdkconfig、不开任何设备
 *      路径）+ 50..100ms 轮询读 64B 块（D2；降级 A，2026-10-19 human 显式指令：
 *      真机 smoke /dev/secondary VFS 直读未达 exit 4 超时零字节）；
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

#include <sys/stat.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "hal/usb_serial_jtag_ll.h"

#include "log_cmd.h"
#include "sd_log.h"

/* 轮询周期（plan §7：50..100ms；过密挤占主链路、过疏命令响应迟钝）。 */
#define KIDPAL_LOGCMD_POLL_MS        80
/* 单次 VFS 读块大小（plan §7：64B 块）。 */
#define KIDPAL_LOGCMD_READ_BLOCK     64
/* cat 流式输出块（plan §5：1KB 块 fwrite + fflush）。 */
#define KIDPAL_LOGCMD_CAT_CHUNK      1024
/* tail 扫描窗上限（261020 D4-4 重定义）：不再假设「文件封顶 512KB 整文件读入」
 * ——真机 KPLOG001 达 740KB 时旧实现对超限文件直接零输出帧（诊断主通道失效）。
 * 新语义 = 只取文件**尾部** min(size, 窗) 字节进缓冲做尾部窗口判定；窗值 = tail
 * 行数上限的覆盖上界（LUR 行钳制 1000 行 × 单记录上限 512B ≈ 512KB）。窗内切行
 * 判定仍归 kidpal_logcmd_tail_window 单源（LUR-A6 消费锚不变）；分配失败按半
 * 递减降级（内部堆 / PSRAM 紧张时保出帧，窗变小只影响能回看的行数）。 */
#define KIDPAL_LOGCMD_TAIL_SCAN_BYTES (512u * 1024u)

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
 * 261020 D4-4: 尾部扫描窗形态 —— 只读文件末 min(size, TAIL_SCAN_BYTES) 字节进
 * 堆缓冲（真机 KPLOG001 740KB 实证「文件封顶 512KB」假设破洞：旧实现超限直接
 * 零输出帧）。分配失败按半递减降级（≥1KB 步进，全败退 NO_FILE，帧仍完整）；
 * 窗口小于文件时首字节可能落在行中间（自造截断缝）——窗口偏移先推到下一个
 * '\n' 之后再输出，窗口内的末 n 行判定仍归 kidpal_logcmd_tail_window 单源。 */
static int EmitTail(unsigned n)
{
    char path[96];
    FILE *f;
    char *buf;
    long size;
    unsigned long want;
    unsigned len;
    int off;
    int cur = KidpalSdLogCurrentIndex();

    if (cur < 0) {
        return KIDPAL_LOGCMD_ERR_NO_CARD;
    }
    if (BuildLogPath((unsigned)cur, path, sizeof(path)) != 0) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    size = FileSizeOf(path);
    if (size < 0) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    if (size == 0) {
        return 0;  /* 空文件：零输出（帧仍完整） */
    }
    f = fopen(path, "rb");
    if (f == nullptr) {
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    want = ((unsigned long)size > (unsigned long)KIDPAL_LOGCMD_TAIL_SCAN_BYTES)
               ? (unsigned long)KIDPAL_LOGCMD_TAIL_SCAN_BYTES
               : (unsigned long)size;
    for (;;) {
        buf = (char *)malloc((size_t)want);
        if (buf != nullptr || want <= 1024ul) {
            break;
        }
        want /= 2ul;  /* 分配失败半递减降级（诊断命令低频，不挤占语音/LVGL 堆） */
    }
    if (buf == nullptr) {
        fclose(f);
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    if (fseek(f, -(long)want, SEEK_END) != 0 ||
        fread(buf, 1, (size_t)want, f) != (size_t)want) {
        free(buf);
        fclose(f);
        return KIDPAL_LOGCMD_ERR_NO_FILE;
    }
    fclose(f);
    len = (unsigned)want;
    off = kidpal_logcmd_tail_window(buf, len, n);
    if (off < 0) {
        off = 0;
    }
    if ((unsigned long)want < (unsigned long)size && off < (int)len) {
        /* 自造截断缝：窗口首行可能是被切断的半行 —— 推到下一个 '\n' 之后。 */
        const char *nl = (const char *)memchr(buf + off, '\n',
                                              (size_t)(len - (unsigned)off));
        if (nl != nullptr) {
            off = (int)(nl - buf) + 1;
        }
    }
    if (off < (int)len) {
        fwrite(buf + off, 1, (size_t)((unsigned)len - (unsigned)off), stdout);
    }
    free(buf);
    fflush(stdout);
    return 0;
}

/* 读任务（自由函数形态，名字 LogCmdReadTask 字面冻结）：secondary 串口 HAL 直读
 * （usb_serial_jtag_ll_read_rxfifo 非阻塞轮询，不经 VFS）-> 行装配 -> 解析 -> 帧响应
 * （stdout）。函数体内零 ESP_LOG（D3：防 vprintf 钩子递归回灌）。 */
static void LogCmdReadTask(void *arg)
{
    static char asm_storage[KIDPAL_LOGCMD_LINE_MAX + 128];
    kidpal_logcmd_asm_t *asmr;
    char line[KIDPAL_LOGCMD_LINE_MAX + 1];
    unsigned char block[KIDPAL_LOGCMD_READ_BLOCK];

    (void)arg;
    asmr = kidpal_logcmd_asm_init(asm_storage, (unsigned)sizeof(asm_storage));
    if (asmr == nullptr) {
        vTaskDelete(nullptr);
        return;
    }
    for (;;) {
        uint32_t got;

        /* HAL 直读：rxfifo 有数据才取，read 本身非阻塞（读到空即返回已取字节数）。
         * 不装 usb_serial_jtag_driver、不开设备路径、不改 sdkconfig（降级 A）。 */
        if (usb_serial_jtag_ll_rxfifo_data_available() != 0) {
            got = usb_serial_jtag_ll_read_rxfifo(block, (uint32_t)sizeof(block));
        } else {
            got = 0u;
        }
        if (got > 0u) {
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
