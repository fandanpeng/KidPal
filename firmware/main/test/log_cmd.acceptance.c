/*
 * Frozen acceptance tests — TaskPackage 261018-sd-log-usb-read
 * (subsystem: firmware/main/diagnostics（log_cmd 纯逻辑 host 同源 seam + log_cmd_port
 *  芯片面 /dev/secondary 读任务）+ firmware/main/main.cc 接线 + firmware/main/diagnostics/
 *  sd_log.h 与 sd_log_fs.cc 只读访问器增量 + tools/diag host 取回脚本; rigor=lite;
 *  riskTier=medium; 诊断日志按需 USB 串口取回（只读命令，不自动 dump、不上云）)。
 *
 * Story 语义权威 = reviewed plan（delivery/taskpackages/261018-sd-log-usb-read/plan.md，
 * architect /triage-change 2026-10-18 比例化 review + human 触点①确认：②单包 Lite、
 * lite 快路径不适用按 full 环境备）。human 原话动机（2026-10-18）：不必自动输出；需要时
 * 经 Claude Code 调 host 脚本按需取回；防自动输出干扰串口监控与烧录。
 *
 * Nature: 行为级（纯逻辑决策函数 + weak 哨兵：log_cmd.c 未编入 host runner 前哨兵恒返回
 * LUR_UNIMPLEMENTED / NULL = 「冻结契约未实现」设计态红，不是编译错）+ 结构级（真实工作
 * 树源锚扫描，sd_log.acceptance.c 同款形态；正向断言只认**注释剥离后可执行源**的字面
 * 形态——只出现在注释里的锚值会假绿）。log_cmd_port.cc 不进 host 构建——结构齿对 port
 * 文件做**源文本扫**（读文件内容断言），不是编入 runner 的行为断言。
 *
 * 本文件**不 include** diagnostics/log_cmd.h（该头 builder 尚未落盘；落地后形状以本文件
 * 头注为冻结契约，sd_log 先例）。落地后由 host runner 的强符号覆盖本文件 weak 哨兵。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；home 路径为冻结落点）:
 *
 *   [S1] 冻结数值单源（home = firmware/main/diagnostics/log_cmd.h；宏字面由 TEST 7 扫
 *        本头钉死；行为测试用本文件镜像常量，EQ-2）:
 *
 *          #define KIDPAL_LOGCMD_LINE_MAX     256u
 *              -- 行装配器单行封顶；超过该值的输入行（到换行为止）整行静默丢弃
 *                 （LUR-A3「超长行丢弃」，不自动 dump 的行为面）。
 *          #define KIDPAL_LOGCMD_CMD_MAX      64u
 *              -- 命令行解析上限；行装配产出的行 len > CMD_MAX -> 解析为 bad-cmd
 *                 （LUR-A2「超长行」）。与 LINE_MAX 是两道不同阈值（64 < 256）：
 *                 64 < len <= 256 的行走解析拒绝（错误帧），> 256 的行走装配丢弃（零输出）。
 *          #define KIDPAL_LOGCMD_TAIL_DEFAULT 200u   〔tail 缺省行数〕
 *          #define KIDPAL_LOGCMD_TAIL_MAX     1000u  〔tail 上限，超出钳制到该值〕
 *          #define KIDPAL_LOGCMD_CAT_COUNT    KIDPAL_SD_LOG_FILE_COUNT
 *              -- cat/list 索引闭集上界；**必须引用 sd_log.h 的
 *                 KIDPAL_SD_LOG_FILE_COUNT（32）单源**（本头 include "sd_log.h"），
 *                 禁写字面 32。
 *
 *   [S2] 命令解析（home = firmware/main/diagnostics/log_cmd.c；纯逻辑、零文件 I/O、
 *        零 stdout 输出——结果只经出参返回）:
 *
 *          解析结果码（#define，禁 enum——跨编译单元整型值语义镜像，EQ-4）:
 *          #define KIDPAL_LOGCMD_SILENT     0    〔非 "kplog " 前缀：静默丢弃〕
 *          #define KIDPAL_LOGCMD_LIST       1
 *          #define KIDPAL_LOGCMD_TAIL       2
 *          #define KIDPAL_LOGCMD_CAT        3
 *          #define KIDPAL_LOGCMD_BAD_CMD    (-1)
 *          #define KIDPAL_LOGCMD_BAD_INDEX  (-2)
 *
 *          int kidpal_logcmd_parse(const char *line, unsigned len,
 *                                  int *out_cmd, unsigned *out_arg);
 *            -- line/len = 行装配产出的完整行（不含行尾）。识别规则（全部冻结）：
 *               * len > CMD_MAX -> BAD_CMD（out_arg = 0）；
 *               * 前缀匹配严格小写 "kplog "（含尾随空格）；"kplog" 无尾空格 /
 *                 "KPLOG list" / "kplogx list" / 前导空格行 -> SILENT
 *                 （防串口噪声触发输出的行为齿）；
 *               * 其后按**单空格**切 token；出现空 token（双空格 / 尾随空格 /
 *                 "kplog " 后无子命令）-> BAD_CMD；
 *               * "list" -> LIST（out_arg = 0）；带尾 token -> BAD_CMD；
 *               * "tail" 缺省 -> TAIL 且 out_arg = TAIL_DEFAULT(200)；"tail N"：
 *                 N 必须为纯十进制数字（1..9 位）；非数字 -> BAD_CMD；N == 0 ->
 *                 BAD_CMD；N > TAIL_MAX(1000) -> 钳制为 1000（out_arg = 1000）；
 *                 多 token -> BAD_CMD；
 *               * "cat" 缺参 -> BAD_CMD；"cat N"：N 必须为纯十进制数字（1..9 位）；
 *                 非数字 / 负号 / '+' / '.' '/' 路径字符开头 -> BAD_INDEX；
 *                 N >= CAT_COUNT(32) -> BAD_INDEX；合法 -> CAT 且 out_arg = idx
 *                 （前导零合法："007" -> 7）；
 *               * 未知子命令 -> BAD_CMD；
 *               * NULL line / NULL out_cmd / NULL out_arg -> 返回 -1（零写出）；
 *               * 其余返回 0（out_cmd/out_arg 已写，含 SILENT / BAD_* 态）。
 *            -- **拒绝即短路**：BAD_* 是纯逻辑出参，本函数不得触达任何文件 I/O
 *               （TEST 2 以 log_cmd.c 可执行源零文件 I/O needle 承载「文件访问层
 *               零触达」的结构半）。
 *
 *   [S3] 行装配（home = log_cmd.c；纯逻辑增量装配器，跨读块边界 / LF / CRLF）:
 *
 *          typedef struct kidpal_logcmd_asm kidpal_logcmd_asm_t;  〔不透明句柄〕
 *
 *          kidpal_logcmd_asm_t *kidpal_logcmd_asm_init(void *storage, unsigned capacity);
 *            -- storage = 调用方存储（chip 面 = 任务栈外静态缓冲）；capacity <
 *               LINE_MAX + 1 -> NULL（不写 storage）；合法 -> 句柄（初始无行）。
 *          void kidpal_logcmd_asm_destroy(kidpal_logcmd_asm_t *a);
 *            -- 释放实现自有控制块，**不动**调用方 storage；NULL 安全。
 *
 *          int kidpal_logcmd_asm_feed(kidpal_logcmd_asm_t *a, const void *data,
 *                                     unsigned len);
 *            -- 喂入读块字节（端口任务 64B 块语义）：累积到 '\n' 为止、跨 feed 边界
 *               拼行；'\r\n' 剥 '\r'；孤 '\r' 不视作行终止（v1 只承诺 LF / CRLF）；
 *               空行（'\n' 紧跟，含仅 '\r' 的行）不入队零产出；行长（不含行尾）>
 *               LINE_MAX 的行：丢弃到换行为止并置 dropped 标记（下次 pop 返
 *               LUR_ASM_DROPPED 一次）。返回实际消费字节数（== len；NULL a /
 *               NULL data -> -1）。
 *
 *          #define LUR_ASM_DROPPED (-2)   〔上次 feed 期间有超长行被整行丢弃〕
 *          #define LUR_ASM_TOOLONG (-3)   〔出参缓冲不足，该行未消费〕
 *
 *          int kidpal_logcmd_asm_pop(kidpal_logcmd_asm_t *a, char *out, unsigned cap);
 *            -- 弹出一条完整行到 out（不含行尾、补 NUL）；返回行长；0 = 无完整行；
 *               dropped 标记在 -> 清标记、返回 LUR_ASM_DROPPED、out 不写；
 *               cap < 行长 + 1 -> LUR_ASM_TOOLONG、行**不消费**；cap < LINE_MAX + 1
 *               恒返 LUR_ASM_TOOLONG（零消费）；NULL a / NULL out -> -1。
 *
 *   [S4] 帧格式化（home = log_cmd.c；纯逻辑，BEGIN/END 与错误字面精确；list 行由
 *        调用方缓冲装配）:
 *
 *          错误码（#define）:
 *          #define KIDPAL_LOGCMD_ERR_NO_CARD   1   〔"KPLOG ERROR no-card"〕
 *          #define KIDPAL_LOGCMD_ERR_NO_FILE   2   〔"KPLOG ERROR no-file"〕
 *          #define KIDPAL_LOGCMD_ERR_BAD_CMD   3   〔"KPLOG ERROR bad-cmd"〕
 *          #define KIDPAL_LOGCMD_ERR_BAD_INDEX 4   〔"KPLOG ERROR bad-index"〕
 *
 *          const char *kidpal_logcmd_frame_begin(int cmd);
 *            -- LIST -> "=== KPLOG BEGIN list ==="；TAIL -> "=== KPLOG BEGIN tail ==="；
 *               CAT -> "=== KPLOG BEGIN cat ==="；BAD_CMD -> "=== KPLOG BEGIN bad-cmd ==="；
 *               BAD_INDEX -> "=== KPLOG BEGIN bad-index ==="（错误响应也走统一帧：
 *               BEGIN(echo 码名) + KPLOG ERROR 行 + END）；其他（含 SILENT）-> NULL。
 *          const char *kidpal_logcmd_frame_end(void);
 *            -- 恒 "=== KPLOG END ==="。
 *          const char *kidpal_logcmd_error_text(int err);
 *            -- ERR_NO_CARD..ERR_BAD_INDEX -> 对应 "KPLOG ERROR <reason>"；其他 -> NULL。
 *
 *          int kidpal_logcmd_list_line(char *out, unsigned cap, unsigned index,
 *                                      unsigned long bytes, int is_current);
 *            -- "KPLOG000.TXT 1234"；is_current != 0 -> 行尾追加 " *"（当前写入文件
 *               标记，判定来源 = KidpalSdLogCurrentIndex()，D5）。成功 0；NULL out /
 *               index >= CAT_COUNT / cap < 40 -> -1（不写 out）。
 *          int kidpal_logcmd_list_total(char *out, unsigned cap,
 *                                       unsigned long total_bytes, unsigned files);
 *            -- "KPLOG TOTAL 12345 3 files"。成功 0；NULL out / cap < 40 -> -1。
 *
 *   [S5] 尾部窗口选取（home = log_cmd.c；纯逻辑，对已读入内存的字节流取末 N 行）:
 *
 *          int kidpal_logcmd_tail_window(const void *data, unsigned len, unsigned n);
 *            -- 在 data[0..len) 中取末 n 行：返回窗口起始偏移 off（窗口 = [off, len)，
 *               字节原样——保留原行尾字节）；行以 '\n' 分隔；缓冲以 '\n' 结尾时不产生
 *               额外空行；无结尾换行的非空末段算一行；n == 0 -> 返回 (int)len（空窗口）；
 *               n >= 行数 -> 0（整缓冲）；len == 0 -> 0；data == NULL 且 len > 0 -> -1。
 *
 *   [S6] 芯片面（home = firmware/main/diagnostics/log_cmd_port.cc；不进 host 构建，
 *        只由结构断言扫源锚）:
 *
 *          void KidpalLogCmdStart(void);
 *            -- main.cc 在 KidpalSdLogChainPreviousVprintf(...) 之后调用（D7；
 *               **函数体内零打印**——无 ESP_LOG* / printf / fwrite / fputs / puts，
 *               无命令零输出，不干扰烧录与监控）。
 *          static void LogCmdReadTask(void *arg);
 *            -- 读任务（自由函数形态，**函数名 LogCmdReadTask 字面冻结**，供结构断言
 *               定位函数体）：open("/dev/secondary", O_RDONLY | O_NONBLOCK)（D2，不装
 *               usb_serial_jtag_driver、不改 sdkconfig）+ 轮询读（vTaskDelay 50..100ms
 *               窗口，64B 块）+ 消费 kidpal_logcmd_* 纯逻辑入口 + 只读 FATFS（一次一个
 *               FD、读完即关、不 rename/remove）+ **函数体内零 ESP_LOG***（防 vprintf
 *               钩子自污染与递归，D3）+ 响应只经 stdout（fwrite / fputs）。
 *
 *   [S7] 只读访问器增量（home = sd_log.h 声明 + sd_log_fs.cc 定义；persist 包文件的
 *        **只增不改**——未 init/未 attach 返回 -1，供 list 的 * 标记与 tail 当前文件）:
 *
 *          int KidpalSdLogCurrentIndex(void);
 *
 *   [S8] 接线（home = firmware/main/main.cc）:
 *
 *          app_main 内 KidpalSdLogInit -> esp_log_set_vprintf(KidpalSdLogVprintf) ->
 *          KidpalSdLogChainPreviousVprintf 之后（同一函数窗内）调用 KidpalLogCmdStart();
 *          （persist 包 sd_log.acceptance.c TEST 6 的安装序锚在本加位下保持绿：新调用
 *          只追加在链式保存之后，init/安装/链保存的相对序零改动。）
 *
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | LUR-A1 命令解析（list/tail/cat 合法形态 + 默认 200 / 上限 1000 钳制 / idx 0..31） | TEST 1 | 行为级 |
 * | LUR-A2 非法输入拒绝（拒绝即短路、文件访问层零触达） | TEST 2（拒绝面逐值）+ TEST 2 末段 log_cmd.c 可执行源零文件 I/O needle | 行为级 + 源锚代理（零 I/O 形态） |
 * | LUR-A3 行装配（跨块 / CRLF/LF / 超长丢弃 / 非 kplog 静默） | TEST 3 | 行为级 |
 * | LUR-A4 帧格式（BEGIN/END 精确字面 / list 行 / 错误帧 / TOTAL 汇总） | TEST 4 | 行为级 |
 * | LUR-A5 尾部窗口（N=1 / N>行数 / CRLF / 无结尾换行） | TEST 5 | 行为级 |
 * | LUR-A6 接线结构（文件名单源 / /dev/secondary / 启动零打印 / 读任务零 ESP_LOG + stdout / D2/D5/D7） | TEST 6 + TEST 7 | 源锚代理（机制级锚点；**不等于运行时观察**） |
 * | D1 命令面只读白名单（仅 kplog 前缀、其余静默） | TEST 1 + TEST 3 SILENT 面 | 行为级 |
 * | D2 /dev/secondary VFS 直开（O_RDONLY|O_NONBLOCK） | TEST 6 源锚 | 源锚代理 |
 * | D3 输出纪律（stdout、读任务零 ESP_LOG） | TEST 6 源锚 | 源锚代理 |
 * | D4 读盘并发（rb 只读、一次一 FD、不 rename/remove） | TEST 6 源锚（禁令半）+ 真机轮 LUR-A7⑤⑥ | 源锚代理（禁令） |
 * | D5 KidpalSdLogCurrentIndex 只读访问器 | TEST 7（声明/定义/port 消费） | 源锚代理 |
 * | D7 任务形态（钩子后启动 / prio 2 / 栈 4KB / 50–100ms 轮询 64B 块） | TEST 7（main.cc 安装序）+ TEST 6（xTaskCreate prio/栈 + vTaskDelay 窗口） | 源锚代理 |
 * | LUR-A7 真机轮观测 | **live-manual，无自动断言**（见 coverage map 末行） | 真机观察（human 持板） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（真机 /dev/secondary 直读可达性）：Kconfig 帮助文本提示 secondary 主要用于
 *         输出；host 齿只证明「只 open /dev/secondary + O_RDONLY|O_NONBLOCK」的源形状，
 *         **不证明**设备上 read() 真能取到字节——真机轮 LUR-A7① 单命令 smoke 承载
 *         （失败按 plan 降级 A/B 链）。
 *   GAP-2（真实 FATFS 并发读）：FS_LOCK=0 下 rb 打开、一次一个 FD、读不干扰写，host
 *         不可达 -> 真机轮 LUR-A7⑤⑥。
 *   GAP-3（stdout 实际可见性）：fwrite 到 stdout 在设备上经 console 双写（UART0 +
 *         USB-Serial/JTAG）——host 只能断言「响应 fwrite/fputs 形态 + 读任务零
 *         ESP_LOG」的源形状，字节真到 COM5 归真机轮 LUR-A7②③④。
 *   GAP-4（host 脚本 tools/diag/kidpal_logs.py）：LUR-X6 exitCriteria 面，无自动齿
 *         （真机轮 LUR-A7① 首验），本文件不断言。
 *   GAP-5（二进制噪声）：行装配只承诺 LF/CRLF 与 NUL-free 文本；含 NUL 字节输入的
 *         行为未定义（v1 不承诺二进制安全；真机轮 LUR-A7⑦「乱敲」用可打印字符）。
 *   GAP-6（解析/装配闭集外的协议面）：BEGIN..END 之间 payload 的行序 / 大文件 cat 的
 *         1KB 块节奏属 port 层自由度，host 齿不钉（真机轮 LUR-A7④ 逐字节核对承载）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：本文件 standalone 红跑不链接
 *        diagnostics/log_cmd.c，哨兵恒红 = 设计态红；host runner 中 log_cmd.c 必须在
 *        HOST_ACCEPTANCE_SRCS 在世（TEST 7 登记守校验），否则哨兵红会掩盖「未接线」。
 *   EQ-2「镜像常量 == log_cmd.h 真值」：行为测试用本文件镜像的 LINE_MAX / CMD_MAX /
 *        TAIL_DEFAULT / TAIL_MAX / CAT_COUNT；TEST 7 扫 log_cmd.h 宏字面把等值钉死
 *        （CAT_COUNT 经 KIDPAL_SD_LOG_FILE_COUNT 单源，而 32 已被 persist 包 TEST 8
 *        钉在 sd_log.h）。
 *   EQ-3「源锚 = 运行时行为」：TEST 6/7 只证明源码具备该形状，不证明设备上端口真的
 *        打开、stdout 真的可见、任务真的以 prio 2 运行——真机轮 LUR-A7 承载。
 *   EQ-4「parse 出参跨编译单元一致」：本文件以 #define 镜像结果码（禁 enum）+ int/
 *        unsigned 指针出参，跨 TU 只有整型值语义，无结构布局风险。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder 接线: 向 firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 追加 "../../log_cmd.acceptance.c" 与纯逻辑源
 *      "../../../diagnostics/log_cmd.c"（置于 host_infra_smoke 之前；登记行**不在
 *      acceptance-author 写权内**，TaskPackage changeManifest 预指派 builder 代落）。
 *      log_cmd_port.cc **不进** host 构建（负向登记守见 TEST 7）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，sd_log sc_red_wrapper 同款通道）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -I. -o /tmp/lur_red_runner \
 *          ../../log_cmd.acceptance.c runner.c
 *      cd <repo root>; /tmp/lur_red_runner      # exit code = 失败用例数
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告）:
 *   设计态（净工作树：无 diagnostics/log_cmd.{c,h}、无 log_cmd_port.cc、main.cc 无
 *   KidpalLogCmdStart、sd_log.h/sd_log_fs.cc 无 KidpalSdLogCurrentIndex、host CMakeLists
 *   未登记）——预期 7 个 TEST_CASE 全红，红因逐条对应当前 story 缺失的真实行为：
 *     TEST 1 -> LUR-A1 解析 seam 未实现（weak 哨兵 -99）
 *     TEST 2 -> LUR-A2 解析 seam 未实现（拒绝面无承载）+ log_cmd.c 缺席
 *     TEST 3 -> LUR-A3 行装配 seam 未实现（asm_init 哨兵 NULL）
 *     TEST 4 -> LUR-A4 帧格式化 seam 未实现（frame_begin 哨兵 NULL / list_line -99）
 *     TEST 5 -> LUR-A5 尾部窗口 seam 未实现（weak 哨兵 -99）
 *     TEST 6 -> LUR-A6 log_cmd_port.cc 缺席（/dev/secondary 读任务无承载）
 *     TEST 7 -> LUR-A6 main.cc 接线缺席 + sd_log 只读访问器缺席 + log_cmd.h 缺席
 *               + 两处登记缺席
 *
 *   冻结落盘实跑（2026-10-19，独立冒烟通道 `gcc -std=c11 -Wall -Wextra -I.` +
 *   red-baseline/runner.c，仓根执行）：FINAL_RED_COMPILE_EXIT=0（零 warning）+
 *   `7 Tests 7 Failures`（RUN_EXIT=7）。逐条首发红名（verbatim）:
 *     1) LUR RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_parse 返回 -99）——命令解析
 *        无承载（样例 "kplog list"）
 *     2) LUR RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_parse 返回 -99）——命令解析
 *        无承载（样例 "kplog cat 32"）
 *     3) LUR-A3 RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_asm_init 返回 NULL）——
 *        行装配语义无法取证
 *     4) LUR-A4 RED: frame_begin(LIST) 字面错（契约 [S4]：=== KPLOG BEGIN list ===）——
 *        host 脚本按 BEGIN/END 定界收集，字面漂移 = 消费端断流
 *     5) LUR-A5 RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_tail_window 返回 -99）——
 *        尾部窗口选取无承载
 *     6) LUR-A6 RED: firmware/main/diagnostics/log_cmd_port.cc 未落地——/dev/secondary
 *        读任务无承载
 *     7) LUR-A6 RED: app_main 未调用 KidpalLogCmdStart();——USB 串口取回命令任务未接线
 *   既有 host 全量套件（同一工作树、本文件尚未登记进 runner）: `203 Tests 3 Failures`
 *   （exit 3）——3 条全为既有 p1_auxbar_style.acceptance.c 的 AB-A1 / AB-A2 / AB-A4
 *   先存红（persist 红基线记录过的同一批），与本包正交；persist 包 sd_log.acceptance.c
 *   全齿保持绿；本包零新增红。
 *   双向可满足性（/tmp 一次性参考实现 + 合成树 KIDPAL_REPO_ROOT 注入，均不写入工作树）:
 *   `7 Tests 0 Failures`；八处定向变异（tail 钳制移除 / BEGIN list 字面漂移 / 窗口偏移
 *   off-by-one / 超长行丢标记移除 / 读任务体内加 ESP_LOGI / main.cc 启动移到链保存前 /
 *   log_cmd.h CMD_MAX 64->128 / host CMakeLists 注销本验收件）各自只打红对应断言
 *   ——断言非空过。
 *   转绿判据：builder 落地 [S1]-[S8] 后全绿；任一仍红而对应源锚已落地 = 锚漂移，
 *   复核本文件锚，**不得改判为绿**。既有 host 全量套件必须零新增红（本文件与既有
 *   套件正交：不改任何既有源文件，除 main.cc / sd_log.h / sd_log_fs.cc 的只增增量
 *   ——persist 包 sd_log.acceptance.c 全齿 must-stay-green，TEST 6 安装序在
 *   KidpalLogCmdStart 链后加位下保持绿）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: LUR-A1 -- 命令解析纯逻辑：list / tail（缺省 200）/ tail N（钳制 1000）/
 *             cat N（0..31 闭集）合法形态逐值 + 参数守卫（TEST 1）
 *   coverage: LUR-A2 -- 非法输入拒绝：idx >=32 / 负数 / 非数字 / 路径字符（../、/、.）/
 *             多余 token / 未知子命令 / 超长命令行 -> bad-index|bad-cmd；拒绝即短路 =
 *             log_cmd.c 可执行源零 fopen/fread/fwrite/fclose/fflush/remove/unlink
 *             needle（TEST 2）
 *   coverage: LUR-A3 -- 行装配纯逻辑：64B 块语义跨 feed 拼行 / CRLF 与 LF 行尾 /
 *             超长行整行静默丢弃 / 空 token 守卫 / 非 kplog 前缀行 parse -> SILENT
 *             （无命令零输出的行为面）（TEST 3）
 *   coverage: LUR-A4 -- 帧格式化纯逻辑：BEGIN 帧逐码字面（list/tail/cat/bad-cmd/
 *             bad-index）/ frame_end 恒字面 / 错误帧字面（no-card|no-file|bad-cmd|
 *             bad-index）/ list 行格式（名字 + 字节数 + 当前 * 标记）/ KPLOG TOTAL
 *             汇总行（TEST 4）
 *   coverage: LUR-A5 -- 尾部窗口选取纯逻辑：N=1 / N > 行数 / CRLF / 无结尾换行 / N=0 /
 *             空缓冲 / 连续空行边界（TEST 5）
 *   coverage: LUR-A6 -- 接线结构齿（源锚，注释剥离后可执行源）：① 文件名只经
 *             kidpal_sd_log_file_name 消费（port 无 KPLOG% 字面 = 禁本地模板 / 用户串
 *             直进路径拼接）（TEST 6）；② 端口只 open /dev/secondary（O_RDONLY|
 *             O_NONBLOCK，全文件唯一 "/dev/ 设备路径字面）（TEST 6）；③ 启动函数
 *             KidpalLogCmdStart 函数体内零打印（TEST 6）；④ 读任务 LogCmdReadTask
 *             函数体内零 ESP_LOG*、响应只经 stdout（fwrite/fputs）+ 纯逻辑入口消费
 *             闭环（TEST 6）；⑤ main.cc 钩子后接线 KidpalLogCmdStart()（D7）+
 *             KidpalSdLogCurrentIndex 只读访问器声明/定义 + log_cmd.h 冻结数值单源
 *             + 两处 CMakeLists 登记（host 面登记 + 设备面登记 + port 不进 host 构建
 *             负向守）（TEST 7）
 *   coverage: LUR-A7 -- **live-manual 真机轮观测项，不落自动断言、不计入 runner 通过面**
 *             （TaskPackage 标 kind=structural；SLP-A6 同款口径）。human 持板执行九项
 *             （① 单命令 smoke 先行，/dev/secondary 直读失败按 plan 降级 A/B 链；
 *             ② 烧录后 COM5 静默无自动输出；③ list 文件名+大小+总量+当前 *；④ tail 50
 *             末 50 行含当轮 STT/LLM 原文片段；⑤ cat 0 全文逐字节、中文 UTF-8 不乱码；
 *             ⑥ 产生新日志后 list/tail 大小增长且 tail 含新行（读不干扰写）；⑦ 非法命令
 *             错误帧不崩无多余输出；⑧ 拔卡 no-card 错误帧、设备不崩、插回重启恢复；
 *             ⑨ 关脚本后 esptool 正常烧录、串口监控正常）。本 Pod 到真机门前停等
 *             （status=awaiting-human:real-device），不自行烧录。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 镜像常量（[S1]；log_cmd.h 落盘后必须与本节逐值相同——TEST 7 扫头文件字面把等值
 * 钉死，EQ-2。CAT_COUNT 镜像 32 = sd_log.h KIDPAL_SD_LOG_FILE_COUNT（persist 包
 * TEST 8 已钉其单源值）。
 * ------------------------------------------------------------------------ */
#define KIDPAL_LOGCMD_LINE_MAX     256u
#define KIDPAL_LOGCMD_CMD_MAX      64u
#define KIDPAL_LOGCMD_TAIL_DEFAULT 200u
#define KIDPAL_LOGCMD_TAIL_MAX     1000u
#define KIDPAL_LOGCMD_CAT_COUNT    32u

/* 解析结果码镜像（[S2]；#define 非 enum，跨编译单元整型值语义，EQ-4） */
#define KIDPAL_LOGCMD_SILENT    0
#define KIDPAL_LOGCMD_LIST      1
#define KIDPAL_LOGCMD_TAIL      2
#define KIDPAL_LOGCMD_CAT       3
#define KIDPAL_LOGCMD_BAD_CMD   (-1)
#define KIDPAL_LOGCMD_BAD_INDEX (-2)

/* 错误码镜像（[S4]） */
#define KIDPAL_LOGCMD_ERR_NO_CARD   1
#define KIDPAL_LOGCMD_ERR_NO_FILE   2
#define KIDPAL_LOGCMD_ERR_BAD_CMD   3
#define KIDPAL_LOGCMD_ERR_BAD_INDEX 4

/* 行装配 pop 特殊返回镜像（[S3]） */
#define LUR_ASM_DROPPED (-2)
#define LUR_ASM_TOOLONG (-3)

/* 未实现哨兵：永不是任何整型 seam 的合法返回值（sd_log SDL_UNIMPLEMENTED 同款纪律）。
 * 指针 seam 的哨兵 = NULL。 */
#define LUR_UNIMPLEMENTED (-99)

/* -------------------------------------------------------------------------- *
 * 纯逻辑 seam 声明（与 log_cmd.h 逐字同型）。
 * ------------------------------------------------------------------------ */
typedef struct kidpal_logcmd_asm kidpal_logcmd_asm_t;

int kidpal_logcmd_parse(const char *line, unsigned len, int *out_cmd, unsigned *out_arg);
kidpal_logcmd_asm_t *kidpal_logcmd_asm_init(void *storage, unsigned capacity);
void kidpal_logcmd_asm_destroy(kidpal_logcmd_asm_t *a);
int kidpal_logcmd_asm_feed(kidpal_logcmd_asm_t *a, const void *data, unsigned len);
int kidpal_logcmd_asm_pop(kidpal_logcmd_asm_t *a, char *out, unsigned cap);
const char *kidpal_logcmd_frame_begin(int cmd);
const char *kidpal_logcmd_frame_end(void);
const char *kidpal_logcmd_error_text(int err);
int kidpal_logcmd_list_line(char *out, unsigned cap, unsigned index, unsigned long bytes,
                            int is_current);
int kidpal_logcmd_list_total(char *out, unsigned cap, unsigned long total_bytes,
                             unsigned files);
int kidpal_logcmd_tail_window(const void *data, unsigned len, unsigned n);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（home = firmware/main/diagnostics/log_cmd.c；host runner 中强定义覆盖）。
 * 未落地 / 未接线态 = 设计态红，不是编译错（EQ-1）。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_logcmd_parse(const char *line, unsigned len,
                                              int *out_cmd, unsigned *out_arg)
{
    (void)line; (void)len; (void)out_cmd; (void)out_arg;
    return LUR_UNIMPLEMENTED;
}
__attribute__((weak)) kidpal_logcmd_asm_t *kidpal_logcmd_asm_init(void *storage,
                                                                  unsigned capacity)
{
    (void)storage; (void)capacity;
    return NULL;
}
__attribute__((weak)) void kidpal_logcmd_asm_destroy(kidpal_logcmd_asm_t *a)
{
    (void)a;
}
__attribute__((weak)) int kidpal_logcmd_asm_feed(kidpal_logcmd_asm_t *a,
                                                 const void *data, unsigned len)
{
    (void)a; (void)data; (void)len;
    return LUR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_logcmd_asm_pop(kidpal_logcmd_asm_t *a, char *out,
                                                unsigned cap)
{
    (void)a; (void)out; (void)cap;
    return LUR_UNIMPLEMENTED;
}
__attribute__((weak)) const char *kidpal_logcmd_frame_begin(int cmd)
{
    (void)cmd;
    return NULL;
}
__attribute__((weak)) const char *kidpal_logcmd_frame_end(void)
{
    return NULL;
}
__attribute__((weak)) const char *kidpal_logcmd_error_text(int err)
{
    (void)err;
    return NULL;
}
__attribute__((weak)) int kidpal_logcmd_list_line(char *out, unsigned cap, unsigned index,
                                                  unsigned long bytes, int is_current)
{
    (void)out; (void)cap; (void)index; (void)bytes; (void)is_current;
    return LUR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_logcmd_list_total(char *out, unsigned cap,
                                                   unsigned long total_bytes, unsigned files)
{
    (void)out; (void)cap; (void)total_bytes; (void)files;
    return LUR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_logcmd_tail_window(const void *data, unsigned len,
                                                    unsigned n)
{
    (void)data; (void)len; (void)n;
    return LUR_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * sd_log.acceptance.c 的 sdl_* 助手族，命名前缀 lur_；全 static，不与先例符号冲突）。
 * ------------------------------------------------------------------------ */

#define LUR_MAX_PATH 1024
#define LUR_MAX_READ (8u * 1024u * 1024u)

static char lur_fm_buf[1024];
static const char *lur_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *lur_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(lur_fm_buf, sizeof(lur_fm_buf), fmt, ap);
    va_end(ap);
    return lur_fm_buf;
}

static int lur_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char lur_root[LUR_MAX_PATH];
static int lur_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int lur_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[LUR_MAX_PATH];
    const char *env_root;
    size_t i;

    if (lur_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (lur_is_regular(probe)) {
            snprintf(lur_root, sizeof(lur_root), "%s", env_root);
            lur_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (lur_is_regular(probe)) {
            snprintf(lur_root, sizeof(lur_root), "%s", cands[i]);
            lur_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int lur_rel(char *out, size_t cap, const char *rel)
{
    if (out == NULL || rel == NULL) {
        return 0;
    }
    if (!lur_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", lur_root, rel);
    return 1;
}

static char *lur_read(const char *rel, size_t *out_len)
{
    char p[LUR_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!lur_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > LUR_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

/* 注释剥离（260917-m2-k3 sealing pass / sd_log 同款）：正向断言只被**可执行源**满足
 * ——注释字节抹成空格；字符串 / 字符字面量体保留（不把字面量里的 // 误判为注释）。
 * 保持缓冲长度，调用方负责 free。 */
static char *lur_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL); /* logic guard: NULL source is a test bug, not a red */
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(out, src, n + 1);

    while (i < n) {
        char c = out[i];
        if (c == '"' || c == '\'') {
            char q = c;
            i++;
            while (i < n) {
                if (out[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (out[i] == q) { i++; break; }
                i++;
            }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '/') {
            while (i < n && out[i] != '\n') { out[i] = ' '; i++; }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '*') {
            out[i] = ' '; out[i + 1] = ' '; i += 2;
            while (i < n) {
                if (out[i] == '*' && i + 1 < n && out[i + 1] == '/') {
                    out[i] = ' '; out[i + 1] = ' '; i += 2; break;
                }
                if (out[i] != '\n') { out[i] = ' '; }
                i++;
            }
            continue;
        }
        i++;
    }
    return out;
}

static char *lur_strip_file(const char *rel)
{
    char *buf = lur_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = lur_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（窗内 needle 在场判定）。未命中 -> NULL。 */
static const char *lur_find_in(const char *hay, const char *end, const char *needle)
{
    size_t nl;

    if (hay == NULL || needle == NULL || end == NULL || hay >= end) {
        return NULL;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return hay;
    }
    for (; hay + nl <= end; hay++) {
        if (hay[0] == needle[0] && memcmp(hay, needle, nl) == 0) {
            return hay;
        }
    }
    return NULL;
}

/* 裸源子串出现计数（负向断言专用；调用方传入已剥注释窗时天然只计可执行源）。 */
static int lur_count_sub(const char *hay, const char *needle)
{
    int n = 0;
    const char *p = hay;
    size_t nl;

    if (hay == NULL || needle == NULL) {
        return 0;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return 0;
    }
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p++;
    }
    return n;
}

/* 行级判定：slice 内同时含 needle1 与 needle2 的行数（注释剥离后的源上使用）。 */
static int lur_count_lines_with_both(const char *slice, const char *needle1,
                                     const char *needle2)
{
    int count = 0;
    const char *line;

    if (slice == NULL || needle1 == NULL || needle2 == NULL) {
        return 0;
    }
    line = slice;
    while (*line != '\0') {
        const char *eol = strchr(line, '\n');
        size_t len = eol ? (size_t)(eol - line) : strlen(line);
        char tmp[LUR_MAX_PATH];
        size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;

        memcpy(tmp, line, cp);
        tmp[cp] = '\0';
        if (strstr(tmp, needle1) != NULL && strstr(tmp, needle2) != NULL) {
            count++;
        }
        if (eol == NULL) {
            break;
        }
        line = eol + 1;
    }
    return count;
}

/* 有界整数提取：[p, end) 内的十进制字面量序（遇 ')' 或 ';' 停）。返回写入个数。 */
static int lur_ints_between(const char *p, const char *end, long *out, int max_args)
{
    int n = 0;

    if (p == NULL || end == NULL || out == NULL || max_args <= 0) {
        return 0;
    }
    while (p < end && n < max_args) {
        if (*p == ')' || *p == ';') {
            break;
        }
        if (*p == '-' || (*p >= '0' && *p <= '9')) {
            char *e = NULL;
            long v = strtol(p, &e, 10);
            if (e == p) {
                break;
            }
            out[n++] = v;
            p = e;
            continue;
        }
        p++;
    }
    return n;
}

/* 行内首个整数提取（`x.y = N;` / `#define X N` 形态）。失败返回 0。 */
static int lur_first_int_after_on_line(const char *hay, const char *needle, long *out)
{
    const char *p;
    const char *eol;
    long a[4];
    int n;

    if (hay == NULL || needle == NULL || out == NULL) {
        return 0;
    }
    p = strstr(hay, needle);
    if (p == NULL) {
        return 0;
    }
    p += strlen(needle);
    eol = strchr(p, '\n');
    if (eol == NULL) {
        eol = p + strlen(p);
    }
    n = lur_ints_between(p, eol, a, 4);
    if (n < 1) {
        return 0;
    }
    *out = a[0];
    return 1;
}

/* 函数窗切片复制：[fn, fn_end) 拷入新分配 NUL 结尾缓冲（调用方 free）。 */
static char *lur_window_copy(const char *fn, const char *fn_end)
{
    size_t win = (size_t)(fn_end - fn);
    char *buf = (char *)malloc(win + 1);

    assert(buf != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(buf, fn, win);
    buf[win] = '\0';
    return buf;
}

/* 自由函数体提取：从定义起点到其后首个行首 '}'（限制在 max_len 内）；找不到闭合
 * 花括号时退化为 min(max_len, strlen) 窗口（[S6] 读任务冻结为自由函数形态）。 */
static const char *lur_body_end(const char *start, size_t max_len)
{
    const char *p = start;
    size_t total;

    assert(start != NULL); /* logic guard */
    total = strlen(start);
    if (max_len > total) {
        max_len = total;
    }
    while ((size_t)(p - start) + 1u < max_len) {
        if (p[0] == '\n' && p[1] == '}') {
            return p + 2;
        }
        p++;
    }
    return start + max_len;
}

#define LUR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(lur_locate_root(), \
    "LUR-Ax RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 设计态红判据（TaskPackage acceptance[]「设计态红」句）：log_cmd 纯逻辑 seam 未实现
 * = weak 哨兵恒 -99 / NULL；builder 落地 diagnostics/log_cmd.c 并登记进
 * HOST_ACCEPTANCE_SRCS 后转绿。 */
#define LUR_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != LUR_UNIMPLEMENTED, \
        lur_fm("LUR RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；" \
               "builder 落地 diagnostics/log_cmd.c 并登记进 HOST_ACCEPTANCE_SRCS 后转绿", \
               what))

/* 一次 parse 断言的捷径：哨兵守卫 + 返回值 + 结果码 + 出参逐项。 */
static void lur_expect_parse(const char *line, int want_cmd, unsigned want_arg)
{
    int cmd = 12345;
    unsigned arg = 4242u;
    int rc = kidpal_logcmd_parse(line, (unsigned)strlen(line), &cmd, &arg);

    TEST_ASSERT_TRUE_MESSAGE(rc != LUR_UNIMPLEMENTED,
        lur_fm("LUR RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_parse 返回 -99）——"
               "命令解析无承载（样例 \"%s\"）；builder 落地 diagnostics/log_cmd.c 并"
               "登记进 HOST_ACCEPTANCE_SRCS 后转绿", line));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc,
        lur_fm("LUR RED: kidpal_logcmd_parse(\"%s\") 返回 %d != 0（非 NULL 入参必须完成"
               "解析，结果码经 out_cmd 返回，契约 [S2]）。", line, rc));
    TEST_ASSERT_EQUAL_INT_MESSAGE(want_cmd, cmd,
        lur_fm("LUR RED: parse(\"%s\") 结果码错（期望 %d 实际 %d）——命令面语义漂移。",
               line, want_cmd, cmd));
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)want_arg, (int)arg,
        lur_fm("LUR RED: parse(\"%s\") 出参错（期望 %u 实际 %u）。", line, want_arg, arg));
}

/* 一次 tail_window 断言的捷径（样本为 NUL 结尾字面，len 取 strlen）。 */
static void lur_expect_window(const char *data, unsigned n, int want_off)
{
    int off = kidpal_logcmd_tail_window(data, (unsigned)strlen(data), n);

    TEST_ASSERT_TRUE_MESSAGE(off != LUR_UNIMPLEMENTED,
        "LUR-A5 RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_tail_window 返回 -99）——"
        "尾部窗口选取无承载；builder 落地 diagnostics/log_cmd.c 后转绿");
    TEST_ASSERT_TRUE_MESSAGE(off >= 0,
        lur_fm("LUR-A5 RED: tail_window(n=%u) 返回 %d < 0（非 -99 哨兵的负值 = 非法返回）。", n, off));
    TEST_ASSERT_TRUE_MESSAGE((size_t)off <= strlen(data),
        lur_fm("LUR-A5 RED: tail_window(n=%u) 返回偏移 %d 越界（> len %u）。", n, off,
               (unsigned)strlen(data)));
    TEST_ASSERT_EQUAL_INT_MESSAGE(want_off, off,
        lur_fm("LUR-A5 RED: tail_window(n=%u) 末 N 行窗口偏移错（期望 %d 实际 %d，样本 "
               "\"%s\"）。", n, want_off, off, data));
}

/* ========================================================================== *
 * TEST 1 — LUR-A1 命令解析纯逻辑（合法形态逐值 + 缺省 200 / 钳制 1000 / idx 0..31
 * 闭集 + 参数守卫）。
 * coverage: LUR-A1 -- kplog list / kplog tail / kplog tail N / kplog cat N 合法形态；
 *           tail 缺省 N=200、上限 1000 钳制；cat idx 闭集 0..31 边界内合法；NULL 入参
 *           零写出守卫
 * RED today（设计态）: diagnostics/log_cmd.c 未落地 -> weak 哨兵 parse 返回 -99。
 * ========================================================================== */
TEST_CASE("log cmd: parse legal list/tail/cat forms with defaults and clamps (LUR-A1)",
          "[log_cmd][LUR-A1][linux][behavior]")
{
    /* ---- 合法形态逐值 -------------------------------------------------------- */
    lur_expect_parse("kplog list", KIDPAL_LOGCMD_LIST, 0u);
    lur_expect_parse("kplog tail", KIDPAL_LOGCMD_TAIL, KIDPAL_LOGCMD_TAIL_DEFAULT);
    lur_expect_parse("kplog tail 1", KIDPAL_LOGCMD_TAIL, 1u);
    lur_expect_parse("kplog tail 50", KIDPAL_LOGCMD_TAIL, 50u);
    lur_expect_parse("kplog tail 200", KIDPAL_LOGCMD_TAIL, 200u);
    lur_expect_parse("kplog tail 1000", KIDPAL_LOGCMD_TAIL, 1000u);
    lur_expect_parse("kplog tail 1001", KIDPAL_LOGCMD_TAIL, 1000u);   /* 上限钳制 */
    lur_expect_parse("kplog tail 99999", KIDPAL_LOGCMD_TAIL, 1000u);  /* 上限钳制 */
    lur_expect_parse("kplog cat 0", KIDPAL_LOGCMD_CAT, 0u);           /* 下边界 */
    lur_expect_parse("kplog cat 7", KIDPAL_LOGCMD_CAT, 7u);
    lur_expect_parse("kplog cat 31", KIDPAL_LOGCMD_CAT, 31u);         /* 上边界 */
    lur_expect_parse("kplog cat 007", KIDPAL_LOGCMD_CAT, 7u);         /* 前导零是数字 */

    /* ---- 参数守卫：NULL 入参 -> -1 且零写出 ---------------------------------- */
    {
        int cmd = 12345;
        unsigned arg = 7u;
        int rc = kidpal_logcmd_parse(NULL, 0u, &cmd, &arg);

        TEST_ASSERT_TRUE_MESSAGE(rc == -1,
            "LUR-A1 RED: parse(NULL line) 未返回 -1——参数守卫缺席（返回 0 且改写出参 = "
            "契约 [S2] 被破坏）。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(12345, cmd,
            "LUR-A1 RED: parse(NULL line) 触碰了出参（必须零写出）。");
        rc = kidpal_logcmd_parse("kplog list", 10u, NULL, &arg);
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, rc, "LUR-A1 RED: parse(NULL out_cmd) 未返回 -1。");
        rc = kidpal_logcmd_parse("kplog list", 10u, &cmd, NULL);
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, rc, "LUR-A1 RED: parse(NULL out_arg) 未返回 -1。");
    }
}

/* ========================================================================== *
 * TEST 2 — LUR-A2 非法输入拒绝（bad-index | bad-cmd + 拒绝即短路：log_cmd.c 可执行
 * 源零文件 I/O）。
 * coverage: LUR-A2 -- idx >=32 / 负数 / 非数字 / 路径字符（../、/、.）/ 多余 token /
 *           未知子命令 / 超长命令行 -> BAD_INDEX 或 BAD_CMD；结构半 = log_cmd.c 可执行
 *           源零 fopen/fread/fwrite/fclose/fflush/remove/unlink needle（文件访问层零触达）
 * RED today（设计态）: 解析 seam 未实现（weak 哨兵 -99）+ log_cmd.c 缺席。
 * ========================================================================== */
TEST_CASE("log cmd: reject bad index, path chars, extra tokens, unknown cmd (LUR-A2)",
          "[log_cmd][LUR-A2][linux][behavior]")
{
    /* ---- cat 拒绝面 -> BAD_INDEX --------------------------------------------- */
    lur_expect_parse("kplog cat 32", KIDPAL_LOGCMD_BAD_INDEX, 0u);   /* 闭集上界外 */
    lur_expect_parse("kplog cat 99", KIDPAL_LOGCMD_BAD_INDEX, 0u);
    lur_expect_parse("kplog cat -1", KIDPAL_LOGCMD_BAD_INDEX, 0u);   /* 负数 */
    lur_expect_parse("kplog cat -5", KIDPAL_LOGCMD_BAD_INDEX, 0u);
    lur_expect_parse("kplog cat abc", KIDPAL_LOGCMD_BAD_INDEX, 0u);  /* 非数字 */
    lur_expect_parse("kplog cat ../x", KIDPAL_LOGCMD_BAD_INDEX, 0u); /* 路径字符 */
    lur_expect_parse("kplog cat /etc", KIDPAL_LOGCMD_BAD_INDEX, 0u);
    lur_expect_parse("kplog cat .", KIDPAL_LOGCMD_BAD_INDEX, 0u);
    lur_expect_parse("kplog cat +5", KIDPAL_LOGCMD_BAD_INDEX, 0u);

    /* ---- tail 拒绝面 -> BAD_CMD ---------------------------------------------- */
    lur_expect_parse("kplog tail abc", KIDPAL_LOGCMD_BAD_CMD, 0u);
    lur_expect_parse("kplog tail -3", KIDPAL_LOGCMD_BAD_CMD, 0u);
    lur_expect_parse("kplog tail 0", KIDPAL_LOGCMD_BAD_CMD, 0u);     /* 0 行无意义 */
    lur_expect_parse("kplog tail 5 6", KIDPAL_LOGCMD_BAD_CMD, 0u);   /* 多 token */
    lur_expect_parse("kplog tail ../x", KIDPAL_LOGCMD_BAD_CMD, 0u);

    /* ---- 未知子命令 / 多余 token / 空白形态 -> BAD_CMD ----------------------- */
    lur_expect_parse("kplog foo", KIDPAL_LOGCMD_BAD_CMD, 0u);
    lur_expect_parse("kplog rm 1", KIDPAL_LOGCMD_BAD_CMD, 0u);       /* 写面禁令同名拒绝 */
    lur_expect_parse("kplog list extra", KIDPAL_LOGCMD_BAD_CMD, 0u);
    lur_expect_parse("kplog cat", KIDPAL_LOGCMD_BAD_CMD, 0u);        /* 缺参 */
    lur_expect_parse("kplog  list", KIDPAL_LOGCMD_BAD_CMD, 0u);      /* 双空格 = 空 token */
    lur_expect_parse("kplog list ", KIDPAL_LOGCMD_BAD_CMD, 0u);      /* 尾随空格 */

    /* ---- 超长命令行（CMD_MAX < len <= LINE_MAX）-> BAD_CMD ------------------- */
    {
        char big[KIDPAL_LOGCMD_LINE_MAX + 8];

        memset(big, 'x', sizeof(big));
        big[0] = 'k'; big[1] = 'p'; big[2] = 'l'; big[3] = 'o'; big[4] = 'g'; big[5] = ' ';
        big[KIDPAL_LOGCMD_CMD_MAX + 2] = '\0'; /* len = CMD_MAX + 2 = 66 > 64 */
        lur_expect_parse(big, KIDPAL_LOGCMD_BAD_CMD, 0u);
    }

    /* ---- 结构半：log_cmd.c 可执行源零文件 I/O（拒绝即短路的机制承载） -------- */
    {
        char *code = lur_strip_file("firmware/main/diagnostics/log_cmd.c");

        TEST_ASSERT_NOT_NULL_MESSAGE(code,
            "LUR-A2 RED(infra): firmware/main/diagnostics/log_cmd.c 未落地——拒绝面结构半"
            "（可执行源零文件 I/O）无承载；builder 落地后转绿");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "fopen("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 fopen( ——纯逻辑层触达文件 I/O（拒绝即短路"
            "被破坏，契约 [S2]：BAD_* 出参态下文件访问层零触达）。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "fread("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 fread(。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "fwrite("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 fwrite(。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "fclose("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 fclose(。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "fflush("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 fflush(。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "remove("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 remove(——只读白名单命令面禁删改。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "unlink("),
            "LUR-A2 RED: log_cmd.c 可执行源存在 unlink(。");
        free(code);
    }
}

/* ========================================================================== *
 * TEST 3 — LUR-A3 行装配纯逻辑（跨读块边界 / CRLF 与 LF / 超长行静默丢弃 / 非 kplog
 * 前缀 SILENT）。
 * coverage: LUR-A3 -- 64B 块语义跨 feed 拼行；'\r\n' 剥 '\r'；超长行（> LINE_MAX）整行
 *           丢弃（pop 返 LUR_ASM_DROPPED 一次、零输出、后续行不受污染）；空行不入队；
 *           无行尾部分行不提前产出；cap 守卫零消费；非 kplog 行 parse -> SILENT
 * RED today（设计态）: 行装配 seam 未实现（weak 哨兵 init 返回 NULL）。
 * ========================================================================== */
TEST_CASE("log cmd: line assembly across chunks, CRLF, overlong drop, silence (LUR-A3)",
          "[log_cmd][LUR-A3][linux][behavior]")
{
    static char storage[KIDPAL_LOGCMD_LINE_MAX + 16];
    kidpal_logcmd_asm_t *a;
    char line[KIDPAL_LOGCMD_LINE_MAX + 8];
    int n;

    a = kidpal_logcmd_asm_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(a,
        "LUR-A3 RED: 冻结 seam 未实现（weak 哨兵 kidpal_logcmd_asm_init 返回 NULL）——"
        "行装配语义无法取证；builder 落地 diagnostics/log_cmd.c 并登记进"
        " HOST_ACCEPTANCE_SRCS 后转绿");

    /* ---- init 参数守卫 -------------------------------------------------------- */
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_asm_init(NULL, (unsigned)sizeof(storage)),
        "LUR-A3 RED: asm_init(NULL storage) 未返回 NULL——存储注入面必须拒绝空存储。");
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_asm_init(storage, KIDPAL_LOGCMD_LINE_MAX),
        "LUR-A3 RED: asm_init 容量 == LINE_MAX 未返回 NULL（需 LINE_MAX + 1）。");

    /* ---- 跨读块边界拼行（64B 块语义） ----------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, kidpal_logcmd_asm_feed(a, "kplog ta", 8u),
        "LUR-A3 RED: feed 第一块返回非 8（必须消费全部字节）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, kidpal_logcmd_asm_feed(a, "il 5\n", 5u),
        "LUR-A3 RED: feed 第二块返回非 5。");
    n = kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line));
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, n,
        "LUR-A3 RED: 跨块拼行失败（pop 返回非 12 = \"kplog tail 5\" 长度）。");
    line[n] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("kplog tail 5", line, "LUR-A3 RED: 跨块拼行内容错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
        "LUR-A3 RED: 行弹出后缓冲未清（第二次 pop 应无完整行）。");

    /* ---- CRLF 与 LF 行尾 ------------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, kidpal_logcmd_asm_feed(a, "kplog list\r\nkplog tail\n", 23u),
        "LUR-A3 RED: feed CRLF+LF 混合块返回错。");
    n = kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line));
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, n, "LUR-A3 RED: CRLF 行 pop 长度非 10（'\\r' 未剥除）。");
    line[n] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("kplog list", line, "LUR-A3 RED: CRLF 行内容错（'\\r' 残留）。");
    n = kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line));
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, n, "LUR-A3 RED: LF 行 pop 长度非 10。");
    line[n] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("kplog tail", line, "LUR-A3 RED: LF 行内容错。");

    /* ---- 空行不入队 ------------------------------------------------------------ */
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, kidpal_logcmd_asm_feed(a, "\r\n\n", 4u),
        "LUR-A3 RED: 空行 feed 返回错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
        "LUR-A3 RED: 空行被产出（空行必须静默跳过）。");

    /* ---- 超长行整行静默丢弃（LUR-A3） ------------------------------------------ */
    {
        char big[512];

        memset(big, 'y', sizeof(big));
        big[0] = 'k'; big[1] = 'p'; big[2] = 'l'; big[3] = 'o'; big[4] = 'g'; big[5] = ' ';
        big[sizeof(big) - 1] = '\n'; /* 512 字节 > LINE_MAX 256，以 LF 结尾 */
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)sizeof(big),
            kidpal_logcmd_asm_feed(a, big, (unsigned)sizeof(big)),
            "LUR-A3 RED: 超长行 feed 未消费全部字节（丢弃也必须吃掉到换行为止的字节）。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(LUR_ASM_DROPPED,
            kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
            "LUR-A3 RED: 超长行未按契约丢弃（pop 未返 LUR_ASM_DROPPED）——超长输入行必须"
            "整行丢弃，不得截断成假命令触发输出。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
            "LUR-A3 RED: 丢弃标记应只消耗一次（第二次 pop 应无完整行）。");
        /* 丢弃后装配器仍可用：后续合法行不受污染。 */
        TEST_ASSERT_EQUAL_INT_MESSAGE(12, kidpal_logcmd_asm_feed(a, "kplog cat 3\n", 12u),
            "LUR-A3 RED: 超长行丢弃后 feed 返回错。");
        n = kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line));
        TEST_ASSERT_EQUAL_INT_MESSAGE(11, n, "LUR-A3 RED: 丢弃后合法行 pop 长度非 11。");
        line[n] = '\0';
        TEST_ASSERT_EQUAL_STRING_MESSAGE("kplog cat 3", line, "LUR-A3 RED: 丢弃后合法行内容错。");
    }

    /* ---- 无行尾的部分行：不产出（等更多字节） ---------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, kidpal_logcmd_asm_feed(a, "kplog", 5u),
        "LUR-A3 RED: 部分行 feed 返回错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
        "LUR-A3 RED: 无行尾部分行被提前产出（必须等 '\\n'）。");

    /* ---- cap 守卫：cap < LINE_MAX + 1 恒 TOOLONG、零消费 ------------------------ */
    /* 此时缓冲内仍持部分行 "kplog"；补 " ok\n" 后拼成完整行 "kplog ok"（8 字节）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, kidpal_logcmd_asm_feed(a, " ok\n", 4u),
        "LUR-A3 RED: 补行尾 feed 返回错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(LUR_ASM_TOOLONG,
        kidpal_logcmd_asm_pop(a, line, KIDPAL_LOGCMD_LINE_MAX),
        "LUR-A3 RED: cap == LINE_MAX 时 pop 未返 LUR_ASM_TOOLONG（出参缓冲守卫缺席）。");
    n = kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line));
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, n,
        "LUR-A3 RED: TOOLONG 后行被消费或内容变（必须零消费，行保留待大缓冲重试）。");
    line[n] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("kplog ok", line,
        "LUR-A3 RED: TOOLONG 重试行内容错——零消费语义被破坏。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_logcmd_asm_pop(a, line, (unsigned)sizeof(line)),
        "LUR-A3 RED: 重试成功后缓冲未清（应无剩余行）。");

    kidpal_logcmd_asm_destroy(a);
    kidpal_logcmd_asm_destroy(NULL); /* NULL 安全（不得崩） */

    /* ---- 非 kplog 前缀 -> SILENT（不自动 dump 的行为面：无命令零输出） --------- */
    {
        static const char *const noise[] = {
            "I (1234) main: boot ok",
            "KPLOG list",       /* 大写 -> 非前缀 */
            "kplogx list",      /* 前缀非 "kplog " */
            "kplog",            /* 无尾空格 */
            "  kplog list",     /* 前导空格 */
            "",
            "W (5678) wifi: disconnected"
        };
        size_t k;

        for (k = 0; k < sizeof(noise) / sizeof(noise[0]); k++) {
            int c = 12345;
            unsigned g = 4242u;
            int rc = kidpal_logcmd_parse(noise[k], (unsigned)strlen(noise[k]), &c, &g);

            TEST_ASSERT_TRUE_MESSAGE(rc != LUR_UNIMPLEMENTED,
                "LUR-A3 RED: 冻结 seam 未实现（weak 哨兵 parse 返回 -99）——SILENT 面"
                "（不自动 dump）无承载。");
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc,
                lur_fm("LUR-A3 RED: parse(\"%s\") 返回 %d != 0。", noise[k], rc));
            TEST_ASSERT_EQUAL_INT_MESSAGE(KIDPAL_LOGCMD_SILENT, c,
                lur_fm("LUR-A3 RED: 非 kplog 前缀行 \"%s\" 被识别为命令（cmd=%d）——"
                       "不自动 dump 的行为面被破坏（串口噪声 / 普通日志会触发输出）。",
                       noise[k], c));
        }
    }
}

/* ========================================================================== *
 * TEST 4 — LUR-A4 帧格式化纯逻辑（BEGIN/END 精确字面 + 错误帧 + list 行 + TOTAL
 * 汇总行）。
 * coverage: LUR-A4 -- frame_begin 逐码字面（list/tail/cat/bad-cmd/bad-index）/
 *           frame_end 恒字面 / error_text 四错误字面 + 非法码 NULL / list_line（名字 +
 *           字节数 + 当前 * 标记 + idx 闭集与 cap 守卫）/ list_total（KPLOG TOTAL 汇总）
 * RED today（设计态）: 帧格式化 seam 未实现（weak 哨兵 NULL / -99）。
 * ========================================================================== */
TEST_CASE("log cmd: frame literals, error frames, list line and total formats (LUR-A4)",
          "[log_cmd][LUR-A4][linux][behavior]")
{
    char buf[128];

    /* ---- BEGIN 帧逐码字面（统一帧：错误响应也走帧） --------------------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG BEGIN list ===",
        kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_LIST),
        "LUR-A4 RED: frame_begin(LIST) 字面错（契约 [S4]：=== KPLOG BEGIN list ===）——"
        "host 脚本按 BEGIN/END 定界收集，字面漂移 = 消费端断流。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG BEGIN tail ===",
        kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_TAIL),
        "LUR-A4 RED: frame_begin(TAIL) 字面错（契约 [S4]）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG BEGIN cat ===",
        kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_CAT),
        "LUR-A4 RED: frame_begin(CAT) 字面错（契约 [S4]）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG BEGIN bad-cmd ===",
        kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_BAD_CMD),
        "LUR-A4 RED: frame_begin(BAD_CMD) 字面错——错误响应也走统一帧（plan §1）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG BEGIN bad-index ===",
        kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_BAD_INDEX),
        "LUR-A4 RED: frame_begin(BAD_INDEX) 字面错——错误响应也走统一帧（plan §1）。");
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_frame_begin(KIDPAL_LOGCMD_SILENT),
        "LUR-A4 RED: frame_begin(SILENT) 未返回 NULL——静默态不得产出帧。");
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_frame_begin(12345),
        "LUR-A4 RED: frame_begin(未知码) 未返回 NULL（结果码闭集外必须拒绝）。");

    /* ---- END 帧恒字面 ---------------------------------------------------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=== KPLOG END ===", kidpal_logcmd_frame_end(),
        "LUR-A4 RED: frame_end() 字面错（契约 [S4]：=== KPLOG END ===）。");

    /* ---- 错误帧字面（no-card|no-file|bad-cmd|bad-index） ----------------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG ERROR no-card",
        kidpal_logcmd_error_text(KIDPAL_LOGCMD_ERR_NO_CARD),
        "LUR-A4 RED: error_text(NO_CARD) 字面错（契约 [S4]：KPLOG ERROR no-card）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG ERROR no-file",
        kidpal_logcmd_error_text(KIDPAL_LOGCMD_ERR_NO_FILE),
        "LUR-A4 RED: error_text(NO_FILE) 字面错（契约 [S4]：KPLOG ERROR no-file）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG ERROR bad-cmd",
        kidpal_logcmd_error_text(KIDPAL_LOGCMD_ERR_BAD_CMD),
        "LUR-A4 RED: error_text(BAD_CMD) 字面错（契约 [S4]：KPLOG ERROR bad-cmd）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG ERROR bad-index",
        kidpal_logcmd_error_text(KIDPAL_LOGCMD_ERR_BAD_INDEX),
        "LUR-A4 RED: error_text(BAD_INDEX) 字面错（契约 [S4]：KPLOG ERROR bad-index）。");
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_error_text(0),
        "LUR-A4 RED: error_text(0) 未返回 NULL（0 不是合法错误码）。");
    TEST_ASSERT_NULL_MESSAGE(kidpal_logcmd_error_text(-7),
        "LUR-A4 RED: error_text(-7) 未返回 NULL（错误码闭集外必须拒绝）。");

    /* ---- list 行格式（名字 + 字节数 + 当前写入文件 * 标记） -------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_logcmd_list_line(buf, (unsigned)sizeof(buf), 0u, 1234UL, 0),
        "LUR-A4 RED: list_line(0, 1234, 非当前) 未返回 0——seam 未实现（weak 哨兵 -99）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG000.TXT 1234", buf,
        "LUR-A4 RED: list_line 非当前文件行格式错（契约 [S4]：KPLOG000.TXT 1234）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_logcmd_list_line(buf, (unsigned)sizeof(buf), 7u, 42UL, 1),
        "LUR-A4 RED: list_line(7, 42, 当前) 未返回 0。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG007.TXT 42 *", buf,
        "LUR-A4 RED: 当前写入文件行尾未标 *（plan §1：当前写入文件行尾标 *，判定来源 = "
        "KidpalSdLogCurrentIndex()，D5）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_logcmd_list_line(buf, (unsigned)sizeof(buf), 31u, 0UL, 0),
        "LUR-A4 RED: list_line(31) 未返回 0（idx 闭集上边界合法）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG031.TXT 0", buf,
        "LUR-A4 RED: list_line(31, 0 bytes) 格式错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_logcmd_list_line(buf, (unsigned)sizeof(buf), 32u, 0UL, 0),
        "LUR-A4 RED: list_line(32) 未返回 -1——索引闭集 0..31（禁 KPLOG032+）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_logcmd_list_line(NULL, 128u, 0u, 0UL, 0),
        "LUR-A4 RED: list_line(NULL out) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_logcmd_list_line(buf, 8u, 0u, 0UL, 0),
        "LUR-A4 RED: list_line(cap=8) 未返回 -1（cap 下界守卫缺席）。");

    /* ---- KPLOG TOTAL 汇总行 ----------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_logcmd_list_total(buf, (unsigned)sizeof(buf), 12345UL, 3u),
        "LUR-A4 RED: list_total 未返回 0——seam 未实现（weak 哨兵 -99）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("KPLOG TOTAL 12345 3 files", buf,
        "LUR-A4 RED: list_total 格式错（契约 [S4]：KPLOG TOTAL <bytes> <n> files）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_logcmd_list_total(NULL, 128u, 0UL, 0u),
        "LUR-A4 RED: list_total(NULL out) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_logcmd_list_total(buf, 8u, 0UL, 0u),
        "LUR-A4 RED: list_total(cap=8) 未返回 -1。");
}

/* ========================================================================== *
 * TEST 5 — LUR-A5 尾部窗口选取纯逻辑（末 N 行边界：N=1 / N > 行数 / CRLF / 无结尾
 * 换行 / N=0 / 空缓冲 / 连续空行）。
 * coverage: LUR-A5 -- 行以 '\n' 分隔；缓冲以 '\n' 结尾不产生额外空行；无结尾换行的
 *           非空末段算一行；窗口 = [off, len) 字节原样（保留原行尾字节）
 * RED today（设计态）: 尾部窗口 seam 未实现（weak 哨兵 -99）。
 * ========================================================================== */
TEST_CASE("log cmd: tail window picks last N lines with CRLF and no-trailing-NL (LUR-A5)",
          "[log_cmd][LUR-A5][linux][behavior]")
{
    /* ---- 基本偏移逐值（样本 + 期望 N + 期望窗口偏移） -------------------------- */
    lur_expect_window("a\nb\nc\n", 1, 4);   /* 末 1 行 = "c\n"，偏移 4 */
    lur_expect_window("a\nb\nc\n", 2, 2);   /* 末 2 行 = "b\nc\n" */
    lur_expect_window("a\nb\nc\n", 3, 0);   /* 全缓冲 */
    lur_expect_window("a\nb\nc\n", 7, 0);   /* N > 行数 -> 整缓冲 */
    lur_expect_window("a\nb\nc", 1, 4);     /* 无结尾换行：末段 "c" 算一行 */
    lur_expect_window("a\nb\nc", 2, 2);     /* "b\nc" */
    lur_expect_window("a\nb\nc", 3, 0);
    lur_expect_window("a\nb\nc", 9, 0);     /* N > 行数 */
    lur_expect_window("a\r\nb\r\nc\r\n", 1, 6); /* CRLF：窗口含原 \r\n 字节 */
    lur_expect_window("a\r\nb\r\nc\r\n", 2, 3); /* 末 2 行 = "b\r\nc\r\n"，偏移 3 */

    /* CRLF 窗口内容字节原样（保留 \r\n，host 脚本按原字节落盘）。 */
    {
        static const char data[] = "a\r\nb\r\nc\r\n";
        int off = kidpal_logcmd_tail_window(data, (unsigned)(sizeof(data) - 1), 2u);

        LUR_REQUIRE_IMPL_INT(off, "kidpal_logcmd_tail_window");
        TEST_ASSERT_TRUE_MESSAGE(off >= 0 && (size_t)off + 6 <= sizeof(data) - 1 &&
                                     memcmp(data + off, "b\r\nc\r\n", 6) == 0,
            "LUR-A5 RED: CRLF 窗口内容非字节原样（末 2 行应为 \"b\\r\\nc\\r\\n\"，'\\r\\n' "
            "必须保留）。");
    }

    /* ---- 退化与边界 ------------------------------------------------------------- */
    lur_expect_window("", 1, 0);            /* 空缓冲 */
    lur_expect_window("a\nb\nc\n", 0, 6);   /* n == 0 -> 空窗口（偏移 = len 6） */
    lur_expect_window("only\n", 1, 0);      /* 单行 */
    lur_expect_window("only", 1, 0);        /* 单行无结尾换行 */
    lur_expect_window("\n\n\n", 1, 2);      /* 空行也算行：3 行，末行偏移 2 */
    lur_expect_window("\n\n\n", 2, 1);
    {
        /* NULL data 且 len > 0 -> -1；NULL data 且 len == 0 -> 0。 */
        int rc = kidpal_logcmd_tail_window(NULL, 5u, 1u);

        LUR_REQUIRE_IMPL_INT(rc, "kidpal_logcmd_tail_window");
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, rc,
            "LUR-A5 RED: tail_window(NULL, 5) 未返回 -1（参数守卫缺席）。");
        rc = kidpal_logcmd_tail_window(NULL, 0u, 1u);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0 || rc == -1,
            "LUR-A5 RED: tail_window(NULL, 0) 返回非法值（0 或 -1 之外）。");
    }
}

/* -------------------------------------------------------------------------- *
 * 结构断言扫描面（源锚；相对仓根）
 * ------------------------------------------------------------------------ */

#define LUR_PORT_CC    "firmware/main/diagnostics/log_cmd_port.cc"
#define LUR_LOGCMD_H   "firmware/main/diagnostics/log_cmd.h"
#define LUR_SDLOG_H    "firmware/main/diagnostics/sd_log.h"
#define LUR_SDLOG_FS   "firmware/main/diagnostics/sd_log_fs.cc"
#define LUR_MAIN_CC    "firmware/main/main.cc"
#define LUR_MAIN_CMAKE "firmware/main/CMakeLists.txt"
#define LUR_HOST_CMAKE "firmware/main/test/host_app/main/CMakeLists.txt"

#define LUR_FN_WINDOW   3000
#define LUR_BODY_WINDOW 8000

/* ========================================================================== *
 * TEST 6 — LUR-A6 ①②③④ port 芯片面源锚（log_cmd_port.cc 源文本扫，不进 host 构建）。
 * coverage: LUR-A6 -- ① 文件名只经 kidpal_sd_log_file_name 消费、port 无 KPLOG% 模板
 *           字面（禁用户串/本地模板直进路径拼接）；② 端口只 open /dev/secondary
 *           （O_RDONLY|O_NONBLOCK，全文件唯一 "/dev/ 设备路径字面，D2）；③ 启动函数
 *           KidpalLogCmdStart 函数体内零打印；④ 读任务 LogCmdReadTask 函数体内零
 *           ESP_LOG*（D3）+ 响应只经 stdout（fwrite/fputs）+ parse 接线 + FATFS 只读
 *           禁令（不 rename/remove，D4）+ 纯逻辑入口消费闭环 + 轮询窗口 50..100ms
 * RED today（设计态）: log_cmd_port.cc 未落地（/dev/secondary 读任务无承载）。
 * ========================================================================== */
TEST_CASE("log cmd: port opens only /dev/secondary, start silent, read task no ESP_LOG (LUR-A6)",
          "[log_cmd][LUR-A6][structural]")
{
    char *code;
    char *body;
    const char *fn;
    const char *fn_end;
    int n;

    LUR_REQUIRE_ROOT();

    code = lur_strip_file(LUR_PORT_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code,
        "LUR-A6 RED: firmware/main/diagnostics/log_cmd_port.cc 未落地——/dev/secondary 读任务"
        "无承载（builder 落地后转绿；本齿对 port 做源文本扫，port 不进 host 构建）");

    /* ---- ② 端口只 open /dev/secondary + D2 直开形态 -------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "\"/dev/secondary\"") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 可执行源无 \"/dev/secondary\" 字面——VFS 直开读通道"
        "缺席（D2：O_RDONLY|O_NONBLOCK，不装 usb_serial_jtag_driver、不改 sdkconfig）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, lur_count_sub(code, "\"/dev/"),
        "LUR-A6 RED: log_cmd_port.cc 出现多个 \"/dev/ 设备路径字面——端口只 open "
        "/dev/secondary（plan §1，禁其它设备路径）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "O_NONBLOCK") != NULL,
        "LUR-A6 RED: port 未用 O_NONBLOCK——非阻塞轮询读形态缺席（D2）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "O_RDONLY") != NULL,
        "LUR-A6 RED: port 未用 O_RDONLY——只读打开纪律缺席（D2/D4：一次一个 FD、读完即关）");

    /* ---- ① 文件名只经 kidpal_sd_log_file_name（禁本地模板 / 用户串直进路径） -- */
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_file_name(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费 kidpal_sd_log_file_name(——8.3 命名单源未接线"
        "（文件名可能本地拼模板 = LFN 依赖 / 32 封顶失效，LUR-A6①）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "KPLOG%"),
        "LUR-A6 RED: log_cmd_port.cc 出现 KPLOG% 模板字面——文件名必须只经 "
        "kidpal_sd_log_file_name 构造（禁用户串 / 本地模板直进路径拼接，LUR-A6①）");

    /* ---- ③ 启动函数 KidpalLogCmdStart 函数体内零打印 + 任务创建形态 ---------- */
    fn = strstr(code, "void KidpalLogCmdStart");
    if (fn == NULL) {
        fn = strstr(code, "KidpalLogCmdStart(");
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "LUR-A6 RED: log_cmd_port.cc 无 KidpalLogCmdStart 定义——命令任务启动函数缺席"
        "（契约 [S6]）");
    fn_end = fn + LUR_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    body = lur_window_copy(fn, fn_end);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(body, "ESP_LOG"),
        "LUR-A6 RED: KidpalLogCmdStart 函数体内存在 ESP_LOG*——启动期打印违背「无命令"
        "零输出」（plan §7，防干扰串口监控与烧录）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(body, "printf("),
        "LUR-A6 RED: KidpalLogCmdStart 函数体内存在 printf 形态——启动函数零打印（含 "
        "snprintf 也不得有，契约 [S6]③）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(body, "fwrite("),
        "LUR-A6 RED: KidpalLogCmdStart 函数体内存在 fwrite(——启动函数零打印。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(body, "fputs("),
        "LUR-A6 RED: KidpalLogCmdStart 函数体内存在 fputs(——启动函数零打印。");
    TEST_ASSERT_TRUE_MESSAGE(strstr(body, "xTaskCreate") != NULL,
        "LUR-A6 RED: KidpalLogCmdStart 内未创建读任务（xTaskCreate）——命令无人消费"
        "（契约 [S6]）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(body, "LogCmdReadTask") != NULL,
        "LUR-A6 RED: 任务入口未指向 LogCmdReadTask——冻结的自由函数形态 / 名字缺席"
        "（契约 [S6]，结构断言依此名定位函数体）");
    {
        /* prio 2（写盘任务为 1）+ 栈 4KB（plan §7）：xTaskCreate 调用窗内整型字面。 */
        const char *xc = strstr(body, "xTaskCreate");
        const char *xc_end;
        size_t remain;
        long a[8];
        int ni, i, has_prio = 0, has_stack = 0, has_four = 0, has_k = 0;

        remain = strlen(xc);
        if (remain > 240) {
            remain = 240;
        }
        xc_end = xc + remain;
        ni = lur_ints_between(xc, xc_end, a, 8);
        for (i = 0; i < ni; i++) {
            if (a[i] == 2) { has_prio = 1; }
            if (a[i] == 4096) { has_stack = 1; }
            if (a[i] == 4) { has_four = 1; }
            if (a[i] == 1024) { has_k = 1; }
        }
        TEST_ASSERT_TRUE_MESSAGE(has_prio,
            "LUR-A6 RED: xTaskCreate 调用窗内未取到优先级字面 2——命令任务必须低优先级"
            "（prio 2，写盘任务为 1，plan §7；宏间接取值失败同红，须复核锚不得改判为绿）");
        TEST_ASSERT_TRUE_MESSAGE(has_stack || (has_four && has_k),
            "LUR-A6 RED: xTaskCreate 调用窗内未取到栈深字面 4096（或 4*1024）——栈 4KB "
            "冻结值不成立（plan §7）");
    }
    free(body);

    /* ---- ④ 读任务 LogCmdReadTask 函数体内零 ESP_LOG* + 响应只经 stdout -------- */
    fn = strstr(code, "void LogCmdReadTask(");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "LUR-A6 RED: log_cmd_port.cc 无 LogCmdReadTask 自由函数定义——读任务本体缺席"
        "（契约 [S6]，名字字面冻结）");
    {
        const char *body_end = lur_body_end(fn, LUR_BODY_WINDOW);

        body = lur_window_copy(fn, body_end);
    }
    n = lur_count_sub(body, "ESP_LOG");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        lur_fm("LUR-A6 RED: 读任务 LogCmdReadTask 函数体内存在 %d 处 ESP_LOG* 调用——"
               "会经 vprintf 钩子递归回灌环形缓冲（自污染 / 掉日志 / 栈溢出风险，D3/"
               "plan §3：响应只经 stdout）", n));
    TEST_ASSERT_TRUE_MESSAGE(lur_count_sub(body, "fwrite(") + lur_count_sub(body, "fputs(") >= 1,
        "LUR-A6 RED: 读任务函数体内无 fwrite( / fputs( ——响应只经 stdout（D3：ESP_LOG "
        "会被 sd_log 的 vprintf 钩子再收进环形缓冲，禁止走 esp_log 通道）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(body, "read(") != NULL,
        "LUR-A6 RED: 读任务函数体内无 read( 调用——/dev/secondary 非阻塞取字节面缺席"
        "（D2）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(body, "kidpal_logcmd_parse(") != NULL,
        "LUR-A6 RED: 读任务未消费 kidpal_logcmd_parse(——解析纯逻辑入口未接线（行为齿"
        "绿 != 接线，决策表不得成死代码）");
    free(body);

    /* ---- FATFS 只读禁令（D4）：不 rename / 不 remove / 不 unlink --------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "f_rename"),
        "LUR-A6 RED: log_cmd_port.cc 存在 f_rename——读任务禁改名（plan §风险：避免 "
        "FATFS 非法操作，D4）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "f_unlink"),
        "LUR-A6 RED: log_cmd_port.cc 存在 f_unlink——只读白名单命令面禁删（D4）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "remove("),
        "LUR-A6 RED: log_cmd_port.cc 存在 remove(——只读白名单命令面禁删（D4）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, lur_count_sub(code, "unlink("),
        "LUR-A6 RED: log_cmd_port.cc 存在 unlink(——只读白名单命令面禁删（D4）");

    /* ---- 纯逻辑入口消费闭环（驱动方 <-> 消费方闭环，sd_log TEST 8 同款纪律） -- */
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_logcmd_asm_init(") != NULL &&
                             strstr(code, "kidpal_logcmd_asm_feed(") != NULL &&
                             strstr(code, "kidpal_logcmd_asm_pop(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费行装配入口（asm_init/asm_feed/asm_pop）——"
        "LUR-A3 行装配语义未接线（命令字节没人拼行）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_logcmd_frame_begin(") != NULL &&
                             strstr(code, "kidpal_logcmd_frame_end(") != NULL &&
                             strstr(code, "kidpal_logcmd_error_text(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费帧格式化入口（frame_begin/frame_end/"
        "error_text）——LUR-A4 帧字面单源未接线（port 本地拼帧 = 双源漂移）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_logcmd_list_line(") != NULL &&
                             strstr(code, "kidpal_logcmd_list_total(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费 list 行装配入口（list_line/list_total）——"
        "list 输出格式单源未接线");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_logcmd_tail_window(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费 kidpal_logcmd_tail_window(——尾部窗口单源"
        "未接线（port 本地切行 = LUR-A5 语义旁路）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "KidpalSdLogCurrentIndex(") != NULL,
        "LUR-A6 RED: log_cmd_port.cc 未消费 KidpalSdLogCurrentIndex(——当前轮转索引"
        "访问器未接线（list 的 * 标记 / tail 的当前文件无来源，D5）");
    free(code);

    /* ---- 轮询窗口（plan §7：50–100ms）：vTaskDelay 行字面，宏间接取后备 -------- */
    {
        char *raw = lur_strip_file(LUR_PORT_CC);
        long v = 0;
        int got;

        TEST_ASSERT_NOT_NULL_MESSAGE(raw, "LUR-A6 RED(infra): log_cmd_port.cc unreadable");
        got = lur_first_int_after_on_line(raw, "vTaskDelay(", &v);
        if (!got) {
            got = lur_first_int_after_on_line(raw, "#define KIDPAL_LOGCMD_POLL_MS", &v);
        }
        TEST_ASSERT_TRUE_MESSAGE(got,
            "LUR-A6 RED: log_cmd_port.cc 无 vTaskDelay( 行内字面、也无 "
            "KIDPAL_LOGCMD_POLL_MS 宏——轮询周期无法取证（plan §7：50–100ms 轮询）");
        TEST_ASSERT_TRUE_MESSAGE(v >= 50 && v <= 100,
            lur_fm("LUR-A6 RED: 轮询周期 %ldms 越界（须 50..100ms，plan §7）——过密挤占"
                   "主链路、过疏命令响应迟钝", v));
        free(raw);
    }
}

/* ========================================================================== *
 * TEST 7 — LUR-A6 ⑤ main.cc 接线序（D7，persist TEST 6 安装序锚的加位兼容形态）+
 * KidpalSdLogCurrentIndex 只读访问器增量（D5）+ log_cmd.h 冻结数值单源（EQ-2）+
 * 两处 CMakeLists 登记守（anti-vacuity / 设备构建面，非独立 acceptance 条目）。
 * coverage: LUR-A6 -- main.cc app_main 内 KidpalLogCmdStart() 在
 *           KidpalSdLogChainPreviousVprintf 之后；sd_log.h 声明 + sd_log_fs.cc 定义
 *           KidpalSdLogCurrentIndex；log_cmd.h 宏字面逐值；host CMakeLists 登记本验收件
 *           与纯逻辑源、port 不进 host 构建（负向）；设备 CMakeLists 登记两源
 * RED today（设计态）: main.cc 无 KidpalLogCmdStart；访问器缺席；log_cmd.h 缺席；
 *           两处登记缺席。
 * ========================================================================== */
TEST_CASE("log cmd: app_main wires KidpalLogCmdStart after hook, accessor, registrations (LUR-A6)",
          "[log_cmd][LUR-A6][structural]")
{
    char *code;
    char *raw;
    char *hdr;
    char *cmake;
    const char *fn;
    const char *fn_end;
    const char *p_init;
    const char *p_set;
    const char *p_chain;
    const char *p_start;
    const char *p_nvs;
    int n;

    LUR_REQUIRE_ROOT();

    /* ---- main.cc：钩子后接线（D7） ------------------------------------------- */
    code = lur_strip_file(LUR_MAIN_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "LUR-A6 RED(infra): main.cc unreadable");
    raw = lur_read(LUR_MAIN_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "LUR-A6 RED(infra): main.cc unreadable");

    /* 扫描器自检（sd_log TEST 6 同款）：行级双针计数器 + 注释剥离生效。 */
    n = lur_count_lines_with_both(code, "esp_err_t", "nvs_flash_init");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "LUR-A6 RED(infra): 行级双针计数器失灵（main.cc 已知在世形态 esp_err_t × "
        "nvs_flash_init 只命中 0 行）——扫描器坏，本文件结构断言失去咬合力");
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "Initialize NVS flash for WiFi") != NULL,
        "LUR-A6 RED(infra): main.cc 已知注释锚缺席——注释剥离自检无法成立（main.cc 被"
        "重写？）。");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "Initialize NVS flash for WiFi") == NULL,
        "LUR-A6 RED(infra): 注释剥离失效（已知注释文本仍留在可执行源里）——本文件全部"
        "结构断言会因注释文本假绿，须复核扫描器");
    free(raw);

    fn = strstr(code, "void app_main");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "LUR-A6 RED(infra): main.cc 无 void app_main 定义锚——入口被改名，须复核锚"
        "不得改判为绿");
    fn_end = fn + LUR_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }

    p_init = lur_find_in(fn, fn_end, "KidpalSdLogInit();");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_init,
        "LUR-A6 RED(infra): app_main 内未找到 KidpalSdLogInit();——persist 接线锚缺席"
        "（上游包被回退？须复核锚不得改判为绿）");
    p_set = lur_find_in(fn, fn_end, "esp_log_set_vprintf(KidpalSdLogVprintf)");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_set,
        "LUR-A6 RED(infra): app_main 内未找到 esp_log_set_vprintf(KidpalSdLogVprintf)"
        "——persist 钩子安装锚缺席");
    p_chain = lur_find_in(fn, fn_end, "KidpalSdLogChainPreviousVprintf(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_chain,
        "LUR-A6 RED(infra): app_main 内未找到 KidpalSdLogChainPreviousVprintf(——persist "
        "链式保存锚缺席");
    p_nvs = lur_find_in(fn, fn_end, "nvs_flash_init");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_nvs,
        "LUR-A6 RED(infra): app_main 内未找到 nvs_flash_init——排序锚缺席");
    p_start = lur_find_in(fn, fn_end, "KidpalLogCmdStart();");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_start,
        "LUR-A6 RED: app_main 未调用 KidpalLogCmdStart();——USB 串口取回命令任务未接线"
        "（契约 [S8]/D7：命令无人消费，kplog 协议不可达）");

    TEST_ASSERT_TRUE_MESSAGE(p_init < p_set,
        "LUR-A6 RED(infra): KidpalSdLogInit() 晚于钩子安装——persist 安装序锚漂移"
        "（persist 包 TEST 6 must-stay-green）");
    TEST_ASSERT_TRUE_MESSAGE(p_chain <= p_set + 200,
        "LUR-A6 RED(infra): 链式保存与钩子安装相距过远——persist 安装序锚漂移"
        "（persist TEST 6 同款邻近判定；一行式链式调用形态下 chain needle 可先于 set "
        "needle，故不作先后断言）");
    TEST_ASSERT_TRUE_MESSAGE(p_chain < p_start,
        "LUR-A6 RED: KidpalLogCmdStart() 出现在 KidpalSdLogChainPreviousVprintf 之前——"
        "契约 [S8] 要求命令任务在 vprintf 钩子链保存之后启动（D7；提前启动会与钩子安装"
        "竞态）");
    free(code);

    /* ---- sd_log.h / sd_log_fs.cc：只读访问器增量（D5；只增不改） --------------- */
    hdr = lur_strip_file(LUR_SDLOG_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "LUR-A6 RED(infra): sd_log.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(hdr, "KidpalSdLogCurrentIndex") != NULL,
        "LUR-A6 RED: sd_log.h 未声明 KidpalSdLogCurrentIndex——当前轮转索引只读访问器"
        "增量缺席（D5：list 的 * 标记 / tail 的当前文件无来源；不靠 mtime）");
    free(hdr);
    code = lur_strip_file(LUR_SDLOG_FS);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "LUR-A6 RED(infra): sd_log_fs.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "int KidpalSdLogCurrentIndex(") != NULL,
        "LUR-A6 RED: sd_log_fs.cc 未定义 int KidpalSdLogCurrentIndex(void)——只读访问器"
        "增量缺席（D5；未 init/未 attach 返回 -1 的行为归真机轮 / 代码复核）");
    free(code);

    /* ---- log_cmd.h：冻结数值单源（EQ-2） --------------------------------------- */
    hdr = lur_strip_file(LUR_LOGCMD_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr,
        "LUR-A6 RED: firmware/main/diagnostics/log_cmd.h 未落地——纯逻辑 seam 与冻结数值"
        "单源缺席（契约 [S1]）");
    TEST_ASSERT_TRUE_MESSAGE(
        lur_count_lines_with_both(hdr, "#define KIDPAL_LOGCMD_LINE_MAX", "256") >= 1,
        "LUR-A6 RED: log_cmd.h 的 KIDPAL_LOGCMD_LINE_MAX 非 256——行装配封顶冻结值漂移"
        "（行为断言镜像常量失去锚，EQ-2）");
    TEST_ASSERT_TRUE_MESSAGE(
        lur_count_lines_with_both(hdr, "#define KIDPAL_LOGCMD_CMD_MAX", "64") >= 1,
        "LUR-A6 RED: log_cmd.h 的 KIDPAL_LOGCMD_CMD_MAX 非 64——命令行解析上限冻结值"
        "漂移（EQ-2）");
    TEST_ASSERT_TRUE_MESSAGE(
        lur_count_lines_with_both(hdr, "#define KIDPAL_LOGCMD_TAIL_DEFAULT", "200") >= 1,
        "LUR-A6 RED: log_cmd.h 的 KIDPAL_LOGCMD_TAIL_DEFAULT 非 200——tail 缺省冻结值"
        "漂移（EQ-2）");
    TEST_ASSERT_TRUE_MESSAGE(
        lur_count_lines_with_both(hdr, "#define KIDPAL_LOGCMD_TAIL_MAX", "1000") >= 1,
        "LUR-A6 RED: log_cmd.h 的 KIDPAL_LOGCMD_TAIL_MAX 非 1000——tail 上限冻结值漂移"
        "（EQ-2）");
    TEST_ASSERT_TRUE_MESSAGE(
        lur_count_lines_with_both(hdr, "#define KIDPAL_LOGCMD_CAT_COUNT",
                                  "KIDPAL_SD_LOG_FILE_COUNT") >= 1,
        "LUR-A6 RED: log_cmd.h 的 KIDPAL_LOGCMD_CAT_COUNT 未引用 KIDPAL_SD_LOG_FILE_COUNT"
        "——索引闭集必须经 sd_log.h 单源（写字面 32 = 双源漂移风险，契约 [S1]）");
    free(hdr);

    /* ---- 两处 CMakeLists 登记守（anti-vacuity + 设备构建面） ------------------- */
    cmake = lur_read(LUR_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake,
        "LUR-A6 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "log_cmd.acceptance.c") != NULL,
        "LUR-A6 RED: host_app/main/CMakeLists.txt 未登记 log_cmd.acceptance.c——本冻结"
        "验收件不进 host runner（全部断言不会被执行 = 空过绿；builder FIRST ACTION，见"
        "头注 Run protocol 0）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/log_cmd.c") != NULL,
        "LUR-A6 RED: host_app/main/CMakeLists.txt 未登记纯逻辑源 diagnostics/log_cmd.c"
        "——行为断言将只跑 weak 哨兵（EQ-1：哨兵红会掩盖「未接线」）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        lur_count_sub(cmake, "../../../diagnostics/log_cmd_port.cc"),
        "LUR-A6 RED: host_app/main/CMakeLists.txt 登记了 diagnostics/log_cmd_port.cc——"
        "port 是芯片面（/dev/secondary 读任务），**不进 host 构建**（契约 [S6]，登记了"
        "会在 host 链接期炸出 ESP-IDF 符号缺失）");
    free(cmake);

    cmake = lur_read(LUR_MAIN_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake,
        "LUR-A6 RED(infra): firmware/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/log_cmd.c") != NULL,
        "LUR-A6 RED: firmware/main/CMakeLists.txt 显式 SOURCES 未登记 diagnostics/"
        "log_cmd.c——main 目录非 GLOB，设备构建拿不到纯逻辑源（LUR-X2 设备编译闸必红）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/log_cmd_port.cc") != NULL,
        "LUR-A6 RED: firmware/main/CMakeLists.txt 显式 SOURCES 未登记 diagnostics/"
        "log_cmd_port.cc——/dev/secondary 读任务不进设备构建（LUR-X2 设备编译闸必红；"
        "persist 包教训：登记是缺口高发位）");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-19 初版冻结（TaskPackage 261018-sd-log-usb-read; acceptance[] LUR-A1~A6
 *               为冻结边界; reviewed plan（architect /triage-change 2026-10-18）为语义
 *               权威）。七个 TEST_CASE：LUR-A1 行为一件（list/tail/cat 合法形态 +
 *               缺省 200 / 钳制 1000 / idx 0..31 闭集 + 参数守卫）、LUR-A2 行为一件
 *               （拒绝面逐值 -> bad-index|bad-cmd + log_cmd.c 可执行源零文件 I/O
 *               needle = 拒绝即短路结构半）、LUR-A3 行为一件（跨块拼行 / CRLF/LF /
 *               超长行整行静默丢弃 / 空 token / cap 守卫零消费 / 非 kplog 前缀
 *               SILENT = 不自动 dump 行为面）、LUR-A4 行为一件（BEGIN/END 精确字面 +
 *               错误帧字面 + list 行 + TOTAL 汇总）、LUR-A5 行为一件（末 N 行窗口：
 *               N=1 / N>行数 / CRLF 字节原样 / 无结尾换行 / 空缓冲 / 空行边界）、
 *               LUR-A6 结构两件（port 源文本扫：/dev/secondary 唯一设备字面 +
 *               O_RDONLY|O_NONBLOCK + 文件名单源 + KidpalLogCmdStart 零打印 +
 *               LogCmdReadTask 零 ESP_LOG + stdout + FATFS 禁令 + 纯逻辑消费闭环 +
 *               轮询 50..100ms；main.cc 钩子后接线 + sd_log 只读访问器增量 +
 *               log_cmd.h 冻结数值单源 + 两处 CMakeLists 登记守）。LUR-A7 为
 *               live-manual 真机轮观测项，不落自动断言（SLP-A6 同款口径），本 Pod 到
 *               真机门前停等 human。冻结落盘冒烟 = 独立冒烟通道实编 + 实跑（设计态红
 *               逐条核对，红名见交付报告）+ 既有 host 全量套件零新增红判读；双向
 *               可满足性以 /tmp 一次性参考实现 + 合成树（KIDPAL_REPO_ROOT 注入）取证，
 *               不写入工作树。
 * --------------------------------------------------------------------------- */
