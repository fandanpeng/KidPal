/*
 * log_cmd.h — 诊断日志按需 USB 串口取回的纯逻辑 seam + 冻结数值单源
 * （TaskPackage 261018-sd-log-usb-read）。
 *
 * 契约权威 = firmware/main/test/log_cmd.acceptance.c 文件头 [S1]-[S8]（冻结，逐字）。
 * 本头承载 [S1]-[S5] 纯逻辑面声明 + 冻结数值宏 + [S6] 芯片面启动声明：
 *   [S1] 冻结数值单源（宏字面由 acceptance TEST 7 扫本头钉死；CAT_COUNT 经 sd_log.h 的
 *        KIDPAL_SD_LOG_FILE_COUNT 单源）；
 *   [S2] 命令解析（list/tail/cat 白名单 + 结果码）；[S3] 行装配（跨块拼行 / CRLF）；
 *   [S4] 帧格式化（BEGIN/END + 错误字面 + list 行）；[S5] 尾部窗口选取。
 *   行为实现家 diagnostics/log_cmd.c，零 FATFS / 零 FreeRTOS / 零 LVGL，host_app
 *   linux target 与设备树同源。
 *   [S6] KidpalLogCmdStart 芯片面声明——实现家 diagnostics/log_cmd_port.cc（不进 host 构建）。
 *
 * 隐私事实同 sd_log（落卡内容含 STT/LLM 原文）：本层只做只读回取，不新增数据类、不出网。
 */

#ifndef KIDPAL_DIAGNOSTICS_LOG_CMD_H
#define KIDPAL_DIAGNOSTICS_LOG_CMD_H

/* [S1] CAT_COUNT 单源引用（32 = KIDPAL_SD_LOG_FILE_COUNT，禁写字面 32）。 */
#include "sd_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- *
 * [S1] 冻结数值单源（acceptance TEST 7 扫本头宏字面，EQ-2）
 * ------------------------------------------------------------------------ */

/* 行装配器单行封顶；超过该值的输入行（到换行为止）整行静默丢弃。 */
#define KIDPAL_LOGCMD_LINE_MAX     256u
/* 命令行解析上限；行 len > CMD_MAX -> bad-cmd（与 LINE_MAX 是两道不同阈值）。 */
#define KIDPAL_LOGCMD_CMD_MAX      64u
/* tail 缺省行数。 */
#define KIDPAL_LOGCMD_TAIL_DEFAULT 200u
/* tail 上限，超出钳制到该值。 */
#define KIDPAL_LOGCMD_TAIL_MAX     1000u
/* cat/list 索引闭集上界；必须引用 sd_log.h 的 KIDPAL_SD_LOG_FILE_COUNT 单源。 */
#define KIDPAL_LOGCMD_CAT_COUNT    KIDPAL_SD_LOG_FILE_COUNT

/* -------------------------------------------------------------------------- *
 * [S2] 命令解析（纯逻辑、零文件 I/O、零 stdout 输出——结果只经出参返回）
 * ------------------------------------------------------------------------ */

/* 解析结果码（#define 非 enum——跨编译单元整型值语义镜像，EQ-4）。 */
#define KIDPAL_LOGCMD_SILENT     0
#define KIDPAL_LOGCMD_LIST       1
#define KIDPAL_LOGCMD_TAIL       2
#define KIDPAL_LOGCMD_CAT        3
#define KIDPAL_LOGCMD_BAD_CMD    (-1)
#define KIDPAL_LOGCMD_BAD_INDEX  (-2)

/* line/len = 行装配产出的完整行（不含行尾）。识别规则见冻结件 [S2]。
 * NULL line / NULL out_cmd / NULL out_arg -> -1（零写出）；其余返回 0（出参已写）。 */
int kidpal_logcmd_parse(const char *line, unsigned len, int *out_cmd, unsigned *out_arg);

/* -------------------------------------------------------------------------- *
 * [S3] 行装配（纯逻辑增量装配器，跨读块边界 / LF / CRLF）
 * ------------------------------------------------------------------------ */

typedef struct kidpal_logcmd_asm kidpal_logcmd_asm_t;

/* storage = 调用方存储（chip 面 = 任务栈外静态缓冲）；capacity < LINE_MAX + 1 -> NULL
 * （不写 storage）；合法 -> 句柄（初始无行）。 */
kidpal_logcmd_asm_t *kidpal_logcmd_asm_init(void *storage, unsigned capacity);
/* 释放实现自有控制块，不动调用方 storage；NULL 安全。 */
void kidpal_logcmd_asm_destroy(kidpal_logcmd_asm_t *a);

/* 喂入读块字节（端口任务 64B 块语义）：累积到 '\n' 为止、跨 feed 边界拼行；
 * 返回实际消费字节数（== len；NULL a / NULL data -> -1）。 */
int kidpal_logcmd_asm_feed(kidpal_logcmd_asm_t *a, const void *data, unsigned len);

/* pop 特殊返回码。 */
#define LUR_ASM_DROPPED (-2)   /* 上次 feed 期间有超长行被整行丢弃 */
#define LUR_ASM_TOOLONG (-3)   /* 出参缓冲不足，该行未消费 */

/* 弹出一条完整行到 out（不含行尾、补 NUL）；返回行长；0 = 无完整行；
 * dropped 标记在 -> 清标记、返回 LUR_ASM_DROPPED、out 不写；
 * cap < 行长 + 1 -> LUR_ASM_TOOLONG、行不消费；cap < LINE_MAX + 1 恒返
 * LUR_ASM_TOOLONG（零消费）；NULL a / NULL out -> -1。 */
int kidpal_logcmd_asm_pop(kidpal_logcmd_asm_t *a, char *out, unsigned cap);

/* -------------------------------------------------------------------------- *
 * [S4] 帧格式化（纯逻辑，BEGIN/END 与错误字面精确；list 行由调用方缓冲装配）
 * ------------------------------------------------------------------------ */

/* 错误码（#define）。 */
#define KIDPAL_LOGCMD_ERR_NO_CARD   1   /* "KPLOG ERROR no-card" */
#define KIDPAL_LOGCMD_ERR_NO_FILE   2   /* "KPLOG ERROR no-file" */
#define KIDPAL_LOGCMD_ERR_BAD_CMD   3   /* "KPLOG ERROR bad-cmd" */
#define KIDPAL_LOGCMD_ERR_BAD_INDEX 4   /* "KPLOG ERROR bad-index" */

/* LIST/TAIL/CAT/BAD_CMD/BAD_INDEX -> 对应 "=== KPLOG BEGIN <cmd> ==="（错误响应也走
 * 统一帧：BEGIN(echo 码名) + KPLOG ERROR 行 + END）；其他（含 SILENT）-> NULL。 */
const char *kidpal_logcmd_frame_begin(int cmd);
/* 恒 "=== KPLOG END ==="。 */
const char *kidpal_logcmd_frame_end(void);
/* ERR_NO_CARD..ERR_BAD_INDEX -> 对应 "KPLOG ERROR <reason>"；其他 -> NULL。 */
const char *kidpal_logcmd_error_text(int err);

/* "KPLOG000.TXT 1234"；is_current != 0 -> 行尾追加 " *"。成功 0；
 * NULL out / index >= CAT_COUNT / cap < 40 -> -1（不写 out）。 */
int kidpal_logcmd_list_line(char *out, unsigned cap, unsigned index, unsigned long bytes,
                            int is_current);
/* "KPLOG TOTAL 12345 3 files"。成功 0；NULL out / cap < 40 -> -1。 */
int kidpal_logcmd_list_total(char *out, unsigned cap, unsigned long total_bytes,
                             unsigned files);

/* -------------------------------------------------------------------------- *
 * [S5] 尾部窗口选取（纯逻辑，对已读入内存的字节流取末 N 行）
 * ------------------------------------------------------------------------ */

/* 在 data[0..len) 中取末 n 行：返回窗口起始偏移 off（窗口 = [off, len)，字节原样）。
 * n == 0 -> (int)len（空窗口）；n >= 行数 -> 0（整缓冲）；len == 0 -> 0；
 * data == NULL 且 len > 0 -> -1。 */
int kidpal_logcmd_tail_window(const void *data, unsigned len, unsigned n);

/* -------------------------------------------------------------------------- *
 * [S6] 芯片面（家 diagnostics/log_cmd_port.cc；不进 host 构建）
 * ------------------------------------------------------------------------ */

/* main.cc 在 KidpalSdLogChainPreviousVprintf(...) 之后调用（[S8]/D7）；函数体内零打印
 * （无命令零输出，不干扰烧录与监控）。 */
void KidpalLogCmdStart(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_DIAGNOSTICS_LOG_CMD_H */
