/*
 * Frozen acceptance tests — TaskPackage 261018-sd-log-persist
 * (subsystem: firmware/main/diagnostics（sd_log 纯逻辑 host 同源 seam + sd_log_fs 芯片面）
 *  + firmware/main（app_main 最早期装 esp_log 钩子）+ firmware/main/boards/
 *  waveshare-s3-rlcd-4.2（挂载成功分支 attach + max_files 5→8）; rigor=full;
 *  riskTier=high; 板端诊断日志落 TF 卡（有界轮转，含 STT/LLM 对话原文）)。
 *
 * Story 语义权威 = reviewed plan（delivery/taskpackages/261018-sd-log-persist/plan.md，
 * architect /triage-change 2026-10-18 比例化 review）+ 上游 FROZEN PROPOSAL
 * （delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md，决策 24 白名单增列
 * 「设备诊断日志」+ ADR-012，landing commit 5a37b001）。
 * human 拍板（2026-10-18）：① 落卡内容含对话原文（STT / LLM）；② 常开 + 有界轮转；
 * ③ 分区表与 sdkconfig 零改动。
 *
 * **隐私事实（已接受风险，不是本文件的验收义务）**：落卡内容含 STT 转写与 LLM 回复
 * 原文 = human 2026-10-18 有意识接受项（记账在 docs/debt/tech/
 * TECH-261016-sd-story-confidentiality.md）。本文件**不**断言任何加密 / 脱敏 / 访问控制
 * ——那不在本包范围，把它写成断言会与 human 拍板相抵。
 *
 * Nature: 行为级（纯逻辑决策函数 + weak 哨兵：sd_log.c 未编入 host runner 前哨兵恒返回
 * SDL_UNIMPLEMENTED / NULL = 「冻结契约未实现」设计态红，不是编译错）+ 结构级（真实工作
 * 树源锚扫描，sd_card_mount / story_cache 先例形态；正向断言只认**注释剥离后可执行源**
 * 的字面形态——260917-m2-k3 sealing pass 教训：只出现在注释里的锚值会假绿）。
 *
 * 本文件**不 include** diagnostics/sd_log.h（该头 builder 尚未落盘；落地后形状以本文件
 * 头注为冻结契约，story_cache 先例）。落地后由 host runner 的强符号覆盖本文件 weak 哨兵。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；home 路径为冻结落点）:
 *
 *   [C1] 环形缓冲（home = firmware/main/diagnostics/sd_log.{c,h}；纯逻辑、零 FATFS /
 *        零 FreeRTOS / 零 LVGL 依赖 -> host_app linux target 与设备树两侧同源；控制块
 *        归实现自有，测试只持不透明句柄，不假设其布局与大小）:
 *
 *          #define KIDPAL_SD_LOG_RECORD_MAX   512u
 *          #define KIDPAL_SD_LOG_BUFFER_BYTES (128u * 1024u)   〔chip 面 PSRAM 注入容量〕
 *
 *          typedef struct kidpal_sd_log_ring kidpal_sd_log_ring_t;
 *
 *          kidpal_sd_log_ring_t *kidpal_sd_log_ring_init(void *storage, unsigned capacity);
 *            -- storage = 调用方存储（chip 面 = PSRAM 块）；capacity = 字节数。
 *               storage == NULL / capacity < 2*(RECORD_MAX+4) -> NULL（不写 storage）;
 *               其余 -> 句柄（初始：pending 0 / dropped 0 / attached 0）。
 *
 *          void kidpal_sd_log_ring_destroy(kidpal_sd_log_ring_t *r);
 *            -- 释放实现自有控制块，**不动**调用方 storage；NULL 安全。
 *
 *          int kidpal_sd_log_ring_write(kidpal_sd_log_ring_t *r, const void *data,
 *                                       unsigned len);
 *            -- 记录 = 任意字节串（原样保存，不补 NUL）；**任意任务 / ISR 上下文可调用**，
 *               多写者并发下每条记录原子入库（无半条 / 无交错 = 不撕裂）；
 *               len > RECORD_MAX -> 截断到 RECORD_MAX；满 -> **丢最旧整条**腾位
 *               （dropped++）再接纳。
 *               返回 = 实际入库字节数（1..RECORD_MAX）; NULL r / NULL data / len == 0
 *               -> -1（零状态变更）。
 *
 *          unsigned kidpal_sd_log_ring_pending(const kidpal_sd_log_ring_t *r);
 *            -- 待消费记录数; NULL -> 0。
 *          unsigned kidpal_sd_log_ring_dropped(const kidpal_sd_log_ring_t *r);
 *            -- 因满溢丢最旧的累计记录数（单调不减）; NULL -> 0。
 *
 *          int kidpal_sd_log_ring_attach(kidpal_sd_log_ring_t *r);
 *            -- attached = 1，**不清缓冲**（早启动累积保留）; NULL -> -1。
 *          int kidpal_sd_log_ring_detach(kidpal_sd_log_ring_t *r);
 *            -- attached = 0，**不清缓冲**; NULL -> -1。
 *          int kidpal_sd_log_ring_attached(const kidpal_sd_log_ring_t *r);  〔1 / 0; NULL -> 0〕
 *
 *          unsigned kidpal_sd_log_ring_peek(kidpal_sd_log_ring_t *r, void *out,
 *                                          unsigned out_cap);
 *            -- 取**最旧未消费**记录到 out（原样字节，不补 NUL），**不推进读指针**
 *               （写盘失败可重试同一条）; **未 attach -> 0 且零消耗**（= 无卡零写盘
 *               的纯逻辑承载面）; 空 -> 0; out_cap < 记录长 -> 0 且零消耗;
 *               成功 -> 记录字节数（>= 1）。
 *          int kidpal_sd_log_ring_commit(kidpal_sd_log_ring_t *r);
 *            -- 消费掉 peek 到的最旧记录（推进读指针，不重复消费）;
 *               消费 -> 1; 无可消费 -> 0; NULL -> -1。
 *
 *   [C2] 轮转与 8.3 命名（home = sd_log.{c,h}；纯逻辑、零文件 I/O）:
 *
 *          #define KIDPAL_SD_LOG_FILE_BYTES (512u * 1024u)   〔单文件分片上限〕
 *          #define KIDPAL_SD_LOG_FILE_COUNT 32u              〔32 个封顶 = 目录 16MB〕
 *          #define KIDPAL_SD_LOG_NAME_CAP   16u              〔"KPLOG031.TXT" = 12 + NUL〕
 *
 *          int kidpal_sd_log_file_name(unsigned index, char *out, unsigned cap);
 *            -- index 0..31 -> "KPLOG%03u.TXT"（8.3：8 字符基名 + 3 字符扩展、全大写、
 *               零 LFN 依赖；恰好 12 字节 + NUL）; 成功 0;
 *               index >= FILE_COUNT / NULL out / cap < 13 -> -1。
 *
 *          typedef struct kidpal_sd_log_rot kidpal_sd_log_rot_t;
 *          kidpal_sd_log_rot_t *kidpal_sd_log_rot_init(void);       〔index 0 / bytes 0〕
 *          void kidpal_sd_log_rot_destroy(kidpal_sd_log_rot_t *rot); 〔NULL 安全〕
 *
 *          int kidpal_sd_log_rot_account(kidpal_sd_log_rot_t *rot, unsigned bytes);
 *            -- 记入本次落盘字节：bytes_in_file += bytes;
 *               若 bytes_in_file >= FILE_BYTES -> 当前文件封口，
 *               index = (index + 1) % FILE_COUNT，bytes_in_file = 0，返回 1（切档）;
 *               否则 0; NULL -> -1。
 *               （单次 account 越过边界时超出部分不计入下一文件，下一文件从 0 起算。）
 *
 *          int kidpal_sd_log_rot_index(const kidpal_sd_log_rot_t *rot);       〔0..31; NULL -> -1〕
 *          unsigned kidpal_sd_log_rot_bytes_in_file(const kidpal_sd_log_rot_t *rot); 〔NULL -> 0〕
 *
 *   [C3] 刷盘门控 / 重试 / 冻结数值（home = sd_log.{c,h}；纯逻辑决策表，显式注入、
 *        无真实 timer / 无全局态）:
 *
 *          #define KIDPAL_SD_LOG_FLUSH_MS    1000UL
 *          #define KIDPAL_SD_LOG_FLUSH_BYTES 4096u
 *          #define KIDPAL_SD_LOG_RETRY_MS    30000UL
 *          #define KIDPAL_SD_LOG_TASK_PRIO   1        〔写盘任务低优先级（<= idle+1）〕
 *
 *          int kidpal_sd_log_should_flush(int attached, unsigned pending_records,
 *                                         unsigned pending_bytes,
 *                                         unsigned long since_last_flush_ms);
 *            -- attached == 0 -> 0（**无卡 = 零 fopen 零写盘**）;
 *               pending_records == 0 -> 0;
 *               since_last_flush_ms >= FLUSH_MS || pending_bytes >= FLUSH_BYTES -> 1;
 *               其余 -> 0。
 *          int kidpal_sd_log_retry_due(int write_failed, unsigned long since_fail_ms);
 *            -- write_failed != 0 && since_fail_ms >= RETRY_MS -> 1; 其余 -> 0。
 *
 *   [C4] 芯片面（home = firmware/main/diagnostics/sd_log_fs.cc；不进 host 构建，
 *        只由结构断言扫源锚）:
 *
 *          typedef int (*kidpal_log_vprintf_t)(const char *fmt, va_list ap);  〔兼容
 *              ESP-IDF esp_log_vprintf_t / vprintf_like_t，不需要 include esp_log.h〕
 *
 *          void KidpalSdLogInit(void);
 *            -- heap_caps_malloc(KIDPAL_SD_LOG_BUFFER_BYTES, MALLOC_CAP_SPIRAM) 分配
 *               PSRAM 环形缓冲 + kidpal_sd_log_ring_init + 写盘任务
 *               xTaskCreate(SdLogWriterTask, ..., KIDPAL_SD_LOG_TASK_PRIO, ...)。
 *          void KidpalSdLogChainPreviousVprintf(kidpal_log_vprintf_t previous);
 *            -- 保存安装返回的旧 vprintf 到文件级 `static kidpal_log_vprintf_t
 *               s_prev_vprintf;`（**变量名字面冻结**，供结构断言与定向变异取证）。
 *          int KidpalSdLogVprintf(const char *fmt, va_list ap);
 *            -- esp_log_set_vprintf 回调：**先**调用 `s_prev_vprintf(` 链式转调
 *               （串口输出不中断）**再**写环形缓冲 `kidpal_sd_log_ring_write(`。
 *          void KidpalSdLogAttach(const char *mount_root);
 *            -- 板级挂载成功分支调用；内部 = kidpal_sd_log_ring_attach(...)（**不清
 *               缓冲**）+ 打开 / 轮转日志文件。无卡路径**不调用**本函数。
 *          static void SdLogWriterTask(void *arg);
 *            -- 写盘任务（自由函数形态，**函数体内零 ESP_LOG***——防 vprintf 钩子
 *               递归回灌）。文件内还须消费 kidpal_sd_log_should_flush( /
 *               kidpal_sd_log_retry_due( / kidpal_sd_log_ring_peek( /
 *               kidpal_sd_log_ring_commit( / kidpal_sd_log_file_name( /
 *               kidpal_sd_log_rot_account( 六个纯逻辑入口（驱动方 <-> 消费方闭环：
 *               决策表与轮转/命名单源不得成为死代码）。
 *
 *   [C5] 接线（home = firmware/main/main.cc + boards/waveshare-s3-rlcd-4.2/
 *        waveshare-s3-rlcd-4.2.cc）:
 *
 *          main.cc app_main 最早期（全部早于首个 nvs_flash_init()）:
 *              KidpalSdLogInit();
 *              KidpalSdLogChainPreviousVprintf(esp_log_set_vprintf(KidpalSdLogVprintf));
 *            -- 环形缓冲 init 必须早于钩子安装（钩子允许在任何时刻被调用，但缓冲须
 *               先就绪）；钩子安装必须早于 NVS 初始化（NVS/WiFi 早期日志不丢）。
 *
 *          board InitializeSdCard() 成功分支:
 *              mount_config.max_files = 8;                 〔5 -> 8〕
 *              s_sd_mounted = true; ... KidpalSdLogAttach(SD_MMC_MOUNT_ROOT);
 *              RunBootRoundtripProbe();
 *            -- attach 在 s_sd_mounted = true 之后、RunBootRoundtripProbe() 之前
 *               （只有挂载成功分支会走到该 `= true`）。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | SLP-A1 有界环形缓冲（丢最旧 / 多写者不撕裂 / drain 推进） | TEST 1（容量上界、丢最旧、FIFO、截断、参数守卫）+ TEST 2（4 写者并发不撕裂 + 记账守恒） | 行为级 |
 * | SLP-A2 轮转与 8.3 命名（000→031 wrap / 512KB 分片 / 32 个封顶 / 目录 16MB） | TEST 3（命名逐值 + 8.3 形态 + 分片边界 + wrap），并配 TEST 8 的冻结数值单源扫描 | 行为级 + 源锚代理（数值宏） |
 * | SLP-A3 早启动留存（init 早于挂载 / 未 attach 累积不清空 / attach 后从最旧 drain） | TEST 4（未 attach 零消耗、attach 不清空、FIFO 顺序）+ TEST 6（app_main 安装序）+ TEST 7（板级 attach 落在成功分支） | 行为级 + 源锚代理（接线） |
 * | SLP-A4 接线结构（钩子最早期 / 链式转调 / attach 序 / max_files=8 / PSRAM 128KB / 任务内零 ESP_LOG） | TEST 6 / TEST 7 / TEST 8 | 源锚代理（机制级锚点；**不等于运行时观察**） |
 * | SLP-A5 降级与重试（无卡零 fopen / 写失败 30s 重试 / 不 panic / 恢复后继续 drain） | TEST 5（决策表 + peek-commit 重试语义）+ TEST 8（芯片面零 panic 形态 + 决策入口被消费） | 行为级（决策半）+ 源锚代理（panic 禁令） |
 * | SLP-A6 真机轮观测（落卡内容 / 串口不中断 / 32 文件循环覆盖 / 拔卡恢复） | **live-manual，无自动断言**（见下方 coverage 行） | 真机观察（human 持板） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（ISR 级原子性）：TEST 2 用 pthread 模拟多任务并发写，证明「每条记录原子入库」；
 *         **不等于**关中断 / 双核 ISR 上下文下的无锁正确性——ISR 语义属芯片侧
 *         （sd_log_fs.cc / 临界区），host 不可达，归真机轮 SLP-A6 + 代码复核。
 *   GAP-2（真实 FATFS I/O）：fopen / 分片落盘 / 32 文件覆盖 / 目录 16MB / 串口不中断，
 *         全部在 chip 侧，host 不可达 -> 真机轮 SLP-A6（human 持板）。
 *   GAP-3（消费端与写者并发）：TEST 1/2 的 drain 发生在写者 join 之后（单消费者纪律）；
 *         写盘任务与日志写者**真正并发**时的丢最旧一致性归真机轮 + 代码复核。
 *   GAP-4（126KB 数字语义）：host 断言的是「注入容量被如实遵守」（TEST 1）与头文件数值
 *         单源（TEST 8）；真实 PSRAM 分配大小归设备编译闸 + 真机轮。
 *   GAP-5（主链路不阻断）：写盘任务低优先级以 KIDPAL_SD_LOG_TASK_PRIO 数值（TEST 8）+
 *         真机轮「语音不卡顿」承载；host 无调度器，不得把数值绿读成实时性已证。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：本文件 standalone 红跑不链接
 *        diagnostics/sd_log.c，哨兵恒红 = 设计态红；host runner 中 sd_log.c 必须在
 *        HOST_ACCEPTANCE_SRCS 在世（TEST 8 登记守校验），否则哨兵红会掩盖「未接线」。
 *   EQ-2「镜像常量 == sd_log.h 真值」：行为测试用本文件镜像的 RECORD_MAX / FILE_BYTES /
 *        FILE_COUNT / FLUSH_* / RETRY_MS；TEST 8 扫 sd_log.h 的宏字面把等值钉死。
 *   EQ-3「源锚 = 运行时行为」：TEST 6/7/8 只证明源码具备该接线形状，**不证明**设备上
 *        钩子真的被调用、串口真的不中断、attach 真的发生在挂载成功后——真机轮 SLP-A6 承载。
 *   EQ-4「KidpalSdLogVprintf 链式转调 = 串口不中断」：结构锚钉「先调 s_prev_vprintf( 再写
 *        缓冲」的顺序；运行时串口输出连续性归真机轮 SLP-A6 ②。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder 接线: 向 firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 追加 "../../sd_log.acceptance.c" 与纯逻辑源
 *      "../../../diagnostics/sd_log.c"（置于 host_infra_smoke 之前；登记行**不在
 *      acceptance-author 写权内**，TaskPackage changeManifest 预指派 builder 代落）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，sc_red_wrapper 同款通道）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -pthread -I. -o /tmp/sdl_red_runner \
 *          ../../sd_log.acceptance.c runner.c
 *      cd <repo root>; /tmp/sdl_red_runner      # exit code = 失败用例数
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告）:
 *   设计态（净工作树：无 diagnostics/sd_log.{c,h}、无 sd_log_fs.cc、main.cc / 板 .cc
 *   无钩子与 attach、max_files 仍 5、两处 CMakeLists 未登记）——预期 8 个 TEST_CASE
 *   全红，红因逐条对应当前 story 缺失的真实行为：
 *     TEST 1  -> SLP-A1 纯逻辑 seam 未实现（ring init 哨兵 NULL）
 *     TEST 2  -> SLP-A1 同上（并发段不可达）
 *     TEST 3  -> SLP-A2 命名 / 轮转 seam 未实现
 *     TEST 4  -> SLP-A3 留存语义 seam 未实现
 *     TEST 5  -> SLP-A5 降级 / 重试 seam 未实现
 *     TEST 6  -> SLP-A4 main.cc 钩子安装缺席
 *     TEST 7  -> SLP-A4 板级 attach / max_files=8 缺席
 *     TEST 8  -> SLP-A4 sd_log.h / sd_log_fs.cc / 两处 CMakeLists 登记缺席
 *
 *   冻结落盘实跑（2026-10-18，独立冒烟通道 `gcc -std=c11 -Wall -Wextra -pthread -I.`
 *   + red-baseline/runner.c，仓根执行）: COMPILE_EXIT=0（零 warning）+
 *   `8 Tests 8 Failures`（RUN_EXIT=8）。逐条首发红名（verbatim）:
 *     1) SLP-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）或
 *        拒绝合法入参（4096B 注入容量）
 *     2) SLP-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）——
 *        多写者并发验收无法进入
 *     3) SLP-A2 RED: file_name(0) 未返回 0——命名 seam 未实现（weak 哨兵 -99）或拒绝
 *        合法入参
 *     4) SLP-A3 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）——
 *        早启动留存语义无法取证
 *     5) SLP RED: 冻结 seam 未实现（weak 哨兵返回 -99）——kidpal_sd_log_retry_due
 *     6) SLP-A4 RED: app_main 未调用 KidpalSdLogInit()——PSRAM 环形缓冲 + 写盘任务未在
 *        启动最早期就绪
 *     7) SLP-A4 RED: InitializeSdCard 内 mount_config.max_files 取值非字面 8（取到 5）
 *     8) SLP-A4 RED: firmware/main/diagnostics/sd_log.h 未落地——纯逻辑 seam 与冻结数值
 *        单源缺席
 *   既有 host 全量套件（同一工作树、本文件尚未登记进 runner）: `195 Tests 3 Failures`
 *   （exit 3）——3 条全为既有 p1_auxbar_style.acceptance.c 的 AB-A1 / AB-A2 / AB-A4
 *   先存红，与本包正交；本包零新增红。
 *   双向可满足性（/tmp 一次性参考实现 + 合成树 KIDPAL_REPO_ROOT 注入，均不写入工作树）:
 *   `8 Tests 0 Failures`；五处定向变异（max_files 8->5 / 钩子移到 NVS 之后 / 写盘任务内
 *   加 ESP_LOG* / attach 清空缓冲 / 满溢改丢最新）各自只打红对应断言——断言非空过。
 *   转绿判据：builder 落地 [C1]-[C5] 后全绿；任一仍红而对应源锚已落地 = 锚漂移，
 *   复核本文件锚，**不得改判为绿**。既有 host 全量套件必须零新增红（本文件与既有
 *   套件正交：无既有源文件被本包改动，除 main.cc / 板 .cc / 两处 CMakeLists——后者
 *   不进 host 编译面）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: SLP-A1 -- 环形缓冲纯逻辑：容量注入上界 / 满后接纳新写并丢最旧 / FIFO
 *             drain 推进不重复消费 / 超长记录截断 / 非法入参零变更（TEST 1）+
 *             多写者并发写入不撕裂（每条记录完整、无半条、按写者保序）（TEST 2）
 *   coverage: SLP-A2 -- 轮转与 8.3 命名：KPLOG000.TXT..KPLOG031.TXT 逐值 / 8.3 形态 /
 *             512KB 分片边界切档 / 31 之后 wrap 回 000 / 32 个封顶 / 目录总量
 *             512KB*32 == 16MB（TEST 3）+ 头文件数值单源（TEST 8）
 *   coverage: SLP-A3 -- 早启动留存：未 attach 期间写入累积且 peek 零消耗 / attach 不清空
 *             缓冲 / attach 后从**最旧**条目开始 drain、早期条目先于晚期条目（TEST 4）+
 *             app_main 安装序（TEST 6）+ 板级 attach 在成功分支（TEST 7）
 *   coverage: SLP-A4 -- 接线结构（源锚，注释剥离后可执行源）：① main.cc app_main 内
 *             KidpalSdLogInit -> esp_log_set_vprintf(KidpalSdLogVprintf) ->
 *             KidpalSdLogChainPreviousVprintf，全部早于首个 nvs_flash_init；钩子保存旧
 *             指针 s_prev_vprintf 且**先转调再写缓冲**（TEST 6）；② 板级 InitializeSdCard
 *             成功分支 s_sd_mounted = true 之后、RunBootRoundtripProbe() 之前调用
 *             KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)（TEST 7）；③ mount_config.max_files =
 *             8 字面（TEST 7）；④ heap_caps_malloc(KIDPAL_SD_LOG_BUFFER_BYTES,
 *             MALLOC_CAP_SPIRAM) + 头文件 BUFFER_BYTES = 128*1024 + 低优先级
 *             KIDPAL_SD_LOG_TASK_PRIO（TEST 8）；⑤ 写盘任务函数体内零 ESP_LOG*
 *             （TEST 8）；另含两处 CMakeLists 登记守（anti-vacuity，非 acceptance 条目）
 *   coverage: SLP-A5 -- 降级与重试：无卡（未 attach）peek 零消耗 + should_flush 恒 0
 *             （= 零 fopen 零写盘，纯逻辑承载面）/ 写失败 30s 重试窗口决策表
 *             （29999ms 不重试、30000ms 重试）/ 写失败不 commit -> 重试后同一条仍在最旧、
 *             成功后继续 drain（TEST 5）+ 芯片面零 ESP_ERROR_CHECK(/abort( panic 形态
 *             + 决策入口被消费（TEST 8）
 *   coverage: SLP-A6 -- **live-manual 真机轮观测项，不落自动断言、不计入 runner 通过面**
 *             （TaskPackage 标 kind=structural；261012 RF-A5 / 261017 PV-A8 同款口径）。
 *             human 持板执行：① /sdcard/logs/ 出现 KPLOG*.TXT，内容含当轮 STT 转写与
 *             LLM 回复原文片段；② 串口 COM5 日志输出不因装钩子中断；③ 写满
 *             32 × 512KB 后最旧被覆盖、目录 <= 16MB；④ 拔卡 / 坏卡场景不崩、30s 重试后
 *             恢复写盘。本 Pod 到真机门前停等（status=awaiting-human:real-device），
 *             不自行烧录。**隐私事实**：含对话原文 = human 有意识接受（见文件头注）。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 镜像常量（[C1]-[C3]；sd_log.h 落盘后必须与本节逐值相同——TEST 8 扫头文件字面
 * 把等值钉死，EQ-2）。
 * ------------------------------------------------------------------------ */

#define KIDPAL_SD_LOG_RECORD_MAX    512u
#define KIDPAL_SD_LOG_BUFFER_BYTES  (128u * 1024u)
#define KIDPAL_SD_LOG_FILE_BYTES    (512u * 1024u)
#define KIDPAL_SD_LOG_FILE_COUNT    32u
#define KIDPAL_SD_LOG_NAME_CAP      16u
#define KIDPAL_SD_LOG_FLUSH_MS      1000UL
#define KIDPAL_SD_LOG_FLUSH_BYTES   4096u
#define KIDPAL_SD_LOG_RETRY_MS      30000UL
#define KIDPAL_SD_LOG_TASK_PRIO     1

/* 未实现哨兵：永不是任何整型 seam 的合法返回值（story_cache KSC_UNIMPLEMENTED 同款
 * 纪律）。指针 seam 的哨兵 = NULL。 */
#define SDL_UNIMPLEMENTED (-99)

/* -------------------------------------------------------------------------- *
 * 不透明句柄的前向声明（镜像 [C1]/[C2]；布局归 builder，本文件不假设）。
 * ------------------------------------------------------------------------ */

typedef struct kidpal_sd_log_ring kidpal_sd_log_ring_t;
typedef struct kidpal_sd_log_rot kidpal_sd_log_rot_t;

/* 纯逻辑 seam 声明（与 sd_log.h 逐字同型）。 */
kidpal_sd_log_ring_t *kidpal_sd_log_ring_init(void *storage, unsigned capacity);
void kidpal_sd_log_ring_destroy(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_write(kidpal_sd_log_ring_t *r, const void *data, unsigned len);
unsigned kidpal_sd_log_ring_pending(const kidpal_sd_log_ring_t *r);
unsigned kidpal_sd_log_ring_dropped(const kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_attach(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_detach(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_attached(const kidpal_sd_log_ring_t *r);
unsigned kidpal_sd_log_ring_peek(kidpal_sd_log_ring_t *r, void *out, unsigned out_cap);
int kidpal_sd_log_ring_commit(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_file_name(unsigned index, char *out, unsigned cap);
kidpal_sd_log_rot_t *kidpal_sd_log_rot_init(void);
void kidpal_sd_log_rot_destroy(kidpal_sd_log_rot_t *rot);
int kidpal_sd_log_rot_account(kidpal_sd_log_rot_t *rot, unsigned bytes);
int kidpal_sd_log_rot_index(const kidpal_sd_log_rot_t *rot);
unsigned kidpal_sd_log_rot_bytes_in_file(const kidpal_sd_log_rot_t *rot);
int kidpal_sd_log_should_flush(int attached, unsigned pending_records,
                               unsigned pending_bytes,
                               unsigned long since_last_flush_ms);
int kidpal_sd_log_retry_due(int write_failed, unsigned long since_fail_ms);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（home = firmware/main/diagnostics/sd_log.c；host runner 中强定义覆盖）。
 * 未落地 / 未接线态 = 设计态红，不是编译错（EQ-1）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) kidpal_sd_log_ring_t *kidpal_sd_log_ring_init(void *storage,
                                                                  unsigned capacity)
{ (void)storage; (void)capacity; return NULL; }
__attribute__((weak)) void kidpal_sd_log_ring_destroy(kidpal_sd_log_ring_t *r)
{ (void)r; }
__attribute__((weak)) int kidpal_sd_log_ring_write(kidpal_sd_log_ring_t *r,
                                                   const void *data, unsigned len)
{ (void)r; (void)data; (void)len; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) unsigned kidpal_sd_log_ring_pending(const kidpal_sd_log_ring_t *r)
{ (void)r; return 0; }
__attribute__((weak)) unsigned kidpal_sd_log_ring_dropped(const kidpal_sd_log_ring_t *r)
{ (void)r; return 0; }
__attribute__((weak)) int kidpal_sd_log_ring_attach(kidpal_sd_log_ring_t *r)
{ (void)r; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_sd_log_ring_detach(kidpal_sd_log_ring_t *r)
{ (void)r; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_sd_log_ring_attached(const kidpal_sd_log_ring_t *r)
{ (void)r; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) unsigned kidpal_sd_log_ring_peek(kidpal_sd_log_ring_t *r,
                                                       void *out, unsigned out_cap)
{ (void)r; (void)out; (void)out_cap; return 0; }
__attribute__((weak)) int kidpal_sd_log_ring_commit(kidpal_sd_log_ring_t *r)
{ (void)r; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_sd_log_file_name(unsigned index, char *out, unsigned cap)
{ (void)index; (void)out; (void)cap; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) kidpal_sd_log_rot_t *kidpal_sd_log_rot_init(void)
{ return NULL; }
__attribute__((weak)) void kidpal_sd_log_rot_destroy(kidpal_sd_log_rot_t *rot)
{ (void)rot; }
__attribute__((weak)) int kidpal_sd_log_rot_account(kidpal_sd_log_rot_t *rot, unsigned bytes)
{ (void)rot; (void)bytes; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_sd_log_rot_index(const kidpal_sd_log_rot_t *rot)
{ (void)rot; return SDL_UNIMPLEMENTED; }
__attribute__((weak)) unsigned kidpal_sd_log_rot_bytes_in_file(const kidpal_sd_log_rot_t *rot)
{ (void)rot; return 0; }
__attribute__((weak)) int kidpal_sd_log_should_flush(int attached, unsigned pending_records,
                                                     unsigned pending_bytes,
                                                     unsigned long since_last_flush_ms)
{
    (void)attached; (void)pending_records; (void)pending_bytes;
    (void)since_last_flush_ms;
    return SDL_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_sd_log_retry_due(int write_failed,
                                                  unsigned long since_fail_ms)
{ (void)write_failed; (void)since_fail_ms; return SDL_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * sd_card_mount.acceptance.c 的 sdm_* 与 story_cache.acceptance.c 的 ksc_* 助手族，
 * 命名前缀 sdl_；全 static，不与先例符号冲突）。
 * ------------------------------------------------------------------------ */

#define SDL_MAX_PATH 1024
#define SDL_MAX_READ (8u * 1024u * 1024u)

static char sdl_fm_buf[1024];
static const char *sdl_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *sdl_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sdl_fm_buf, sizeof(sdl_fm_buf), fmt, ap);
    va_end(ap);
    return sdl_fm_buf;
}

static int sdl_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char sdl_root[SDL_MAX_PATH];
static int sdl_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int sdl_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[SDL_MAX_PATH];
    const char *env_root;
    size_t i;

    if (sdl_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (sdl_is_regular(probe)) {
            snprintf(sdl_root, sizeof(sdl_root), "%s", env_root);
            sdl_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (sdl_is_regular(probe)) {
            snprintf(sdl_root, sizeof(sdl_root), "%s", cands[i]);
            sdl_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int sdl_rel(char *out, size_t cap, const char *rel)
{
    if (out == NULL || rel == NULL) {
        return 0;
    }
    if (!sdl_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", sdl_root, rel);
    return 1;
}

static char *sdl_read(const char *rel, size_t *out_len)
{
    char p[SDL_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!sdl_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SDL_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（260917-m2-k3 sealing pass / sd_card_mount 同款）：正向断言只被**可执行
 * 源**满足——注释字节抹成空格；字符串 / 字符字面量体保留（不把字面量里的 // 误判为
 * 注释）。保持缓冲长度，调用方负责 free。 */
static char *sdl_strip_c_comments(const char *src)
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

static char *sdl_strip_file(const char *rel)
{
    char *buf = sdl_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = sdl_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（函数窗内 needle 在场判定）。未命中 -> NULL。 */
static const char *sdl_find_in(const char *hay, const char *end, const char *needle)
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

/* 裸源子串出现计数（负向断言专用——panic 形态 / ESP_LOG* 计数等；调用方传入已剥
 * 注释窗时天然只计可执行源）。 */
static int sdl_count_sub(const char *hay, const char *needle)
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
static int sdl_count_lines_with_both(const char *slice, const char *needle1,
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
        char tmp[SDL_MAX_PATH];
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

/* 有界整数提取：[p, end) 内、首个 ')' 或 ';' 前的十进制字面量序。返回写入个数。 */
static int sdl_ints_between(const char *p, const char *end, long *out, int max_args)
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
static int sdl_first_int_after_on_line(const char *hay, const char *needle, long *out)
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
    n = sdl_ints_between(p, eol, a, 4);
    if (n < 1) {
        return 0;
    }
    *out = a[0];
    return 1;
}

/* 函数窗切片复制：[fn, fn_end) 拷入新分配 NUL 结尾缓冲（调用方 free）。 */
static char *sdl_window_copy(const char *fn, const char *fn_end)
{
    size_t win = (size_t)(fn_end - fn);
    char *buf = (char *)malloc(win + 1);

    assert(buf != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(buf, fn, win);
    buf[win] = '\0';
    return buf;
}

/* 自由函数体提取：从定义起点到其后首个行首 '}'（限制在 max_len 内）；找不到闭合
 * 花括号时退化为 min(max_len, strlen) 窗口（C4 写盘任务冻结为自由函数形态）。 */
static const char *sdl_body_end(const char *start, size_t max_len)
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

#define SDL_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(sdl_locate_root(), \
    "SLP-Ax RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

#define SDL_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != SDL_UNIMPLEMENTED, \
        sdl_fm("%s RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；" \
               "builder 落地 diagnostics/sd_log.c 并登记进 HOST_ACCEPTANCE_SRCS 后转绿", \
               "SLP", what))

/* -------------------------------------------------------------------------- *
 * 记录构造 / 校验助手（TEST 1/2 共用；记录为任意字节串，本文件用可自证的定长格式）
 * ------------------------------------------------------------------------ */

/* 24 字节记录："R<tag>-<4 位序号>-" + 16 个 tag 字节（内容可自证完整 / 无撕裂）。 */
static size_t sdl_make_record(char *buf, size_t cap, char tag, unsigned seq)
{
    int n = snprintf(buf, cap, "R%c-%04u-", tag, seq % 10000u);
    size_t len;
    int i;

    if (n < 0 || (size_t)n + 17 > cap) {
        return 0;
    }
    len = (size_t)n;
    for (i = 0; i < 16; i++) {
        buf[len++] = tag;
    }
    buf[len] = '\0';
    return len;
}

/* 解析并校验一条记录：形态 + 长度 + 填充字节全对才算完整（半条 / 交错 -> 0）。 */
static int sdl_parse_record(const char *buf, size_t len, char *tag, unsigned *seq)
{
    unsigned v = 0;
    int i;

    if (buf == NULL || len != 24 || buf[0] != 'R' || buf[2] != '-' || buf[7] != '-') {
        return 0;
    }
    for (i = 3; i <= 6; i++) {
        if (buf[i] < '0' || buf[i] > '9') {
            return 0;
        }
        v = v * 10u + (unsigned)(buf[i] - '0');
    }
    for (i = 8; i < 24; i++) {
        if (buf[i] != buf[1]) {
            return 0;
        }
    }
    if (tag != NULL) {
        *tag = buf[1];
    }
    if (seq != NULL) {
        *seq = v;
    }
    return 1;
}

/* 64 字节定长记录："N<9 位序号>-" + 52 个 'F'（drop-oldest 语义取证用）。 */
static void sdl_fill64(char *buf, unsigned seq)
{
    int n = snprintf(buf, 64, "N%09u-", seq);
    size_t len = n > 0 ? (size_t)n : 0u;

    while (len < 64u) {
        buf[len++] = 'F';
    }
}

static unsigned sdl_parse64(const char *buf, size_t len)
{
    unsigned v = 0;
    size_t i;

    if (buf == NULL || len != 64u || buf[0] != 'N' || buf[10] != '-') {
        return 0xFFFFFFFFu;
    }
    for (i = 1; i <= 9; i++) {
        if (buf[i] < '0' || buf[i] > '9') {
            return 0xFFFFFFFFu;
        }
        v = v * 10u + (unsigned)(buf[i] - '0');
    }
    for (i = 11; i < 64u; i++) {
        if (buf[i] != 'F') {
            return 0xFFFFFFFFu;
        }
    }
    return v;
}

/* ========================================================================== *
 * TEST 1 — SLP-A1 环形缓冲纯逻辑（容量上界 + 丢最旧 + FIFO drain 推进 + 截断 +
 * 非法入参零变更）。
 * coverage: SLP-A1 -- 容量注入上界 / 写满后新写入被接纳且丢最旧 / drain 后读指针
 *           推进且不重复消费 / 超长记录截断 / 非法入参零状态变更
 * RED today（设计态）: diagnostics/sd_log.c 未落地 -> weak 哨兵 init 返回 NULL。
 * ========================================================================== */
TEST_CASE("sd log: ring bounded capacity, drop-oldest, FIFO drain pointer (SLP-A1)",
          "[sd_log][SLP-A1][linux][behavior]")
{
    static unsigned char storage[4096];
    kidpal_sd_log_ring_t *r;
    char out[KIDPAL_SD_LOG_RECORD_MAX + 8];
    char big[600];
    unsigned len;
    unsigned i;

    /* ---- 非法入参：init 拒绝 NULL storage / 过小容量 ------------------------ */
    TEST_ASSERT_NULL_MESSAGE(kidpal_sd_log_ring_init(NULL, (unsigned)sizeof(storage)),
        "SLP-A1 RED: ring_init(NULL storage) 未返回 NULL——存储注入面必须拒绝空存储");
    TEST_ASSERT_NULL_MESSAGE(kidpal_sd_log_ring_init(storage, 16u),
        "SLP-A1 RED: ring_init 容量过小（16B < 2*(512+4)）未拒绝——容量下界义务被破坏");

    r = kidpal_sd_log_ring_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(r,
        "SLP-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）或"
        "拒绝合法入参（4096B 注入容量）——builder 落地 diagnostics/sd_log.c 并登记进"
        " HOST_ACCEPTANCE_SRCS 后转绿；契约见本文件头注 [C1]");

    /* ---- 初始态 ------------------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: 新环 pending != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_dropped(r),
        "SLP-A1 RED: 新环 dropped != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attached(r),
        "SLP-A1 RED: 新环 attached 初始值应为 0（未挂载）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attach(r),
        "SLP-A1 RED: ring_attach 合法入参未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_attached(r),
        "SLP-A1 RED: attach 后 attached != 1。");

    /* ---- 非法写入 / 空记录：拒绝且零状态变更 -------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_write(NULL, "x", 1u),
        "SLP-A1 RED: write(NULL ring) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_write(r, NULL, 3u),
        "SLP-A1 RED: write(NULL data) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_write(r, "x", 0u),
        "SLP-A1 RED: write(len == 0) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: 非法写入触碰了缓冲（pending 涨了）。");

    /* ---- FIFO：最旧先出、peek 不推进、commit 推进、不重复消费 --------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_write(r, "A", 1u),
        "SLP-A1 RED: write(\"A\") 返回非 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, kidpal_sd_log_ring_write(r, "BB", 2u),
        "SLP-A1 RED: write(\"BB\") 返回非 2。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, kidpal_sd_log_ring_write(r, "CCC", 3u),
        "SLP-A1 RED: write(\"CCC\") 返回非 3。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: 三条写入后 pending != 3。");

    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, (int)len,
        "SLP-A1 RED: peek 未取到最旧记录 \"A\"（长度 1）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE('A', (int)out[0], "SLP-A1 RED: peek 取到的不是最旧记录。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: peek 推进了读指针（pending 变了）——peek 必须无副作用（写失败可重试）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r),
        "SLP-A1 RED: commit 未消费掉 peek 到的记录。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: commit 后 pending != 2。");

    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, (int)len,
        "SLP-A1 RED: commit 后 peek 未推进到下一条（应为 \"BB\"，长度 2）——重复消费/跳条。");
    TEST_ASSERT_EQUAL_INT_MESSAGE('B', (int)out[0], "SLP-A1 RED: 第二条记录内容错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A1 RED: 第二次 commit 失败。");

    /* ---- out_cap 不足：零消耗 ------------------------------------------------- */
    len = kidpal_sd_log_ring_peek(r, out, 2u);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)len,
        "SLP-A1 RED: out_cap 不足（2 < 3）时 peek 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A1 RED: out_cap 不足的 peek 消耗了记录（必须零消耗）。");
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, (int)len, "SLP-A1 RED: 容量恢复后 peek 未取到 \"CCC\"。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A1 RED: 第三次 commit 失败。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r), "SLP-A1 RED: 清空后 pending != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_commit(r),
        "SLP-A1 RED: 空环 commit 未返回 0。");

    /* ---- 超长记录：截断到 RECORD_MAX ---------------------------------------- */
    memset(big, 'T', sizeof(big));
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)KIDPAL_SD_LOG_RECORD_MAX,
        kidpal_sd_log_ring_write(r, big, (unsigned)sizeof(big)),
        "SLP-A1 RED: 超长记录（600B）未截断到 RECORD_MAX（512B）。");
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)KIDPAL_SD_LOG_RECORD_MAX, (int)len,
        "SLP-A1 RED: 截断记录的长度 != 512。");
    TEST_ASSERT_EQUAL_INT_MESSAGE('T', (int)out[511], "SLP-A1 RED: 截断记录内容不完整。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A1 RED: 截断记录 commit 失败。");

    /* ---- 容量上界 + 丢最旧：写 200 条 64B 记录，保留的必须是**最近**一段 ---- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_dropped(r),
        "SLP-A1 RED: 尚未溢出时 dropped != 0。");
    {
        char rec[64];
        unsigned pending;
        unsigned dropped;
        unsigned first_seq;
        unsigned expect;

        for (i = 0; i < 200u; i++) {
            sdl_fill64(rec, i);
            TEST_ASSERT_EQUAL_INT_MESSAGE(64, kidpal_sd_log_ring_write(r, rec, 64u),
                sdl_fm("SLP-A1 RED: 第 %u 条 64B 记录未被接纳（满时应丢最旧腾位后接纳新写）。", i));
        }
        pending = kidpal_sd_log_ring_pending(r);
        dropped = kidpal_sd_log_ring_dropped(r);
        TEST_ASSERT_EQUAL_INT_MESSAGE(200, (int)(pending + dropped),
            sdl_fm("SLP-A1 RED: 记账不守恒 pending(%u) + dropped(%u) != 200。", pending, dropped));
        TEST_ASSERT_TRUE_MESSAGE(dropped > 0u,
            "SLP-A1 RED: 4096B 注入容量装不下 200×64B 却 zero drop——溢出策略未生效。");
        TEST_ASSERT_TRUE_MESSAGE(pending >= 8u,
            sdl_fm("SLP-A1 RED: 保留条目过少（pending=%u）——容量上界未被如实利用。", pending));
        TEST_ASSERT_TRUE_MESSAGE(pending * (64u + 4u) <= (unsigned)sizeof(storage),
            sdl_fm("SLP-A1 RED: 保留量越界（pending=%u 条 × >=68B > 注入容量 %u）——容量上界"
                   "语义被破坏（每记录至少 64B 数据 + 4B 头）。", pending, (unsigned)sizeof(storage)));

        memset(out, 0, sizeof(out));
        len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
        TEST_ASSERT_EQUAL_INT_MESSAGE(64, (int)len, "SLP-A1 RED: 溢出后 peek 未取到 64B 记录。");
        out[len] = '\0';
        first_seq = sdl_parse64(out, len);
        expect = 200u - pending; /* 丢最旧 => 保留的是最后 pending 条 */
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)expect, (int)first_seq,
            sdl_fm("SLP-A1 RED: 丢最旧语义错——溢出后最旧保留条目序号应为 %u（= 200 - pending），"
                   "实际 %u（丢最旧被实现成丢最新？）。", expect, first_seq));

        for (i = 0; i < pending; i++) {
            memset(out, 0, sizeof(out));
            len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
            TEST_ASSERT_EQUAL_INT_MESSAGE(64, (int)len,
                sdl_fm("SLP-A1 RED: 第 %u 条待消费记录长度非 64。", i));
            out[len] = '\0';
            TEST_ASSERT_EQUAL_INT_MESSAGE((int)(expect + i), (int)sdl_parse64(out, len),
                sdl_fm("SLP-A1 RED: FIFO 顺序错（第 %u 条应为序号 %u）。", i, expect + i));
            TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r),
                sdl_fm("SLP-A1 RED: 第 %u 条 commit 失败。", i));
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r),
            "SLP-A1 RED: 全量 drain 后 pending != 0。");
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_commit(NULL),
        "SLP-A1 RED: commit(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_attach(NULL),
        "SLP-A1 RED: attach(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_ring_detach(NULL),
        "SLP-A1 RED: detach(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attached(NULL),
        "SLP-A1 RED: attached(NULL) 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(NULL),
        "SLP-A1 RED: pending(NULL) 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_dropped(NULL),
        "SLP-A1 RED: dropped(NULL) 未返回 0。");

    kidpal_sd_log_ring_destroy(r);
}

/* ========================================================================== *
 * TEST 2 — SLP-A1 多写者并发写入不撕裂（任务/ISR 语义模拟：4 写者 × 200 条）。
 * coverage: SLP-A1 -- 多写者并发写入不撕裂：每条记录完整（无半条 / 无交错填充）、
 *           按写者保序、无重复消费；溢出与并发叠加时记账守恒
 * RED today（设计态）: 同上（ring init 哨兵 NULL，并发段不可达）。
 * ========================================================================== */

#define SDL_WRITERS 4
#define SDL_RECORDS_PER_WRITER 200u

typedef struct {
    kidpal_sd_log_ring_t *ring;
    char tag;
    unsigned count;
    int bad;
} sdl_writer_arg_t;

static void *sdl_writer_main(void *arg)
{
    sdl_writer_arg_t *w = (sdl_writer_arg_t *)arg;
    char rec[64];
    unsigned i;

    for (i = 0; i < w->count; i++) {
        size_t len = sdl_make_record(rec, sizeof(rec), w->tag, i);
        if (len == 0 || kidpal_sd_log_ring_write(w->ring, rec, (unsigned)len) <= 0) {
            w->bad = 1;
            break;
        }
    }
    return NULL;
}

TEST_CASE("sd log: multi-writer concurrent appends never tear records (SLP-A1)",
          "[sd_log][SLP-A1][linux][behavior][concurrency]")
{
    static unsigned char storage[4096];
    kidpal_sd_log_ring_t *r;
    sdl_writer_arg_t args[SDL_WRITERS];
    pthread_t tids[SDL_WRITERS];
    char out[KIDPAL_SD_LOG_RECORD_MAX + 8];
    unsigned per_tag[SDL_WRITERS];
    unsigned first_seq[SDL_WRITERS];
    int have_first[SDL_WRITERS];
    static const char tags[] = "ABCD"; /* NUL 结尾，供 strchr 索引写者号 */
    unsigned len;
    unsigned pending_before;
    unsigned drained = 0;
    int w;

    r = kidpal_sd_log_ring_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(r,
        "SLP-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）——"
        "多写者并发验收无法进入；builder 落地 diagnostics/sd_log.c 后转绿");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attach(r),
        "SLP-A1 RED: ring_attach 合法入参未返回 0。");

    for (w = 0; w < SDL_WRITERS; w++) {
        args[w].ring = r;
        args[w].tag = tags[w];
        args[w].count = SDL_RECORDS_PER_WRITER;
        args[w].bad = 0;
    }
    for (w = 0; w < SDL_WRITERS; w++) {
        int rc = pthread_create(&tids[w], NULL, sdl_writer_main, &args[w]);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc,
            sdl_fm("SLP-A1 RED(infra): pthread_create 失败（w=%d, rc=%d）——并发验收通道不可用。",
                   w, rc));
    }
    for (w = 0; w < SDL_WRITERS; w++) {
        (void)pthread_join(tids[w], NULL);
    }
    for (w = 0; w < SDL_WRITERS; w++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, args[w].bad,
            sdl_fm("SLP-A1 RED: 写者 %c 存在拒绝写入（write 返回值 <= 0）——满时应丢最旧后接纳新写。",
                   tags[w]));
    }

    /* 记账守恒：写入总数 = 待消费 + 因满溢丢最旧。 */
    pending_before = kidpal_sd_log_ring_pending(r);
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)(SDL_WRITERS * SDL_RECORDS_PER_WRITER),
        (int)(pending_before + kidpal_sd_log_ring_dropped(r)),
        sdl_fm("SLP-A1 RED: 并发记账不守恒 pending(%u) + dropped(%u) != %u。",
               pending_before, kidpal_sd_log_ring_dropped(r),
               (unsigned)(SDL_WRITERS * SDL_RECORDS_PER_WRITER)));
    TEST_ASSERT_TRUE_MESSAGE(pending_before > 0u,
        "SLP-A1 RED: 并发写入后缓冲为空——写入全部丢失。");

    for (w = 0; w < SDL_WRITERS; w++) {
        per_tag[w] = 0;
        first_seq[w] = 0;
        have_first[w] = 0;
    }

    /* 逐条 drain 并校验完整性 / 保序 / 无重复。 */
    while ((len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out))) > 0u) {
        char tag = 0;
        unsigned seq = 0;

        out[len] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(sdl_parse_record(out, len, &tag, &seq),
            sdl_fm("SLP-A1 RED: 多写者并发下记录撕裂（len=%u, 首字节 0x%02x）——"
                   "记录必须原子入库，不得出现半条或交错填充。", len,
                   (unsigned)(unsigned char)out[0]));
        w = (int)(strchr(tags, tag) != NULL ? (strchr(tags, tag) - tags) : -1);
        TEST_ASSERT_TRUE_MESSAGE(w >= 0,
            sdl_fm("SLP-A1 RED: 记录 tag 非本测试写者集合（tag=%c）——记录体被污染。", tag));
        if (have_first[w]) {
            TEST_ASSERT_EQUAL_INT_MESSAGE((int)(first_seq[w] + per_tag[w]), (int)seq,
                sdl_fm("SLP-A1 RED: 写者 %c 的记录失序 / 重复（期望 %u，实际 %u）。",
                       tag, first_seq[w] + per_tag[w], seq));
        } else {
            first_seq[w] = seq;
            have_first[w] = 1;
        }
        per_tag[w]++;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r),
            "SLP-A1 RED: drain commit 失败。");
        drained++;
        if (drained > SDL_WRITERS * SDL_RECORDS_PER_WRITER + 8u) {
            TEST_ASSERT_TRUE_MESSAGE(0,
                "SLP-A1 RED: drain 条目数超过写入总数——读指针未推进（无限重复消费）。");
        }
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE((int)pending_before, (int)drained,
        sdl_fm("SLP-A1 RED: drain 条目数(%u) != 进入 drain 前 pending(%u)。", drained, pending_before));
    /* 每个写者的保留段都必须是**自身序列的尾部**（丢最旧 => 每写者留下最后 k 条、
     * 序号严格递增且连续）。并发调度可能让某写者的全部记录落在保留窗口之外
     * （先跑完的写者被整体挤出），故「至少保留一条」不作为义务——只对实际保留到的
     * 写者校验尾部连续性与序。 */
    for (w = 0; w < SDL_WRITERS; w++) {
        if (!have_first[w]) {
            continue;
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)SDL_RECORDS_PER_WRITER,
            (int)(first_seq[w] + per_tag[w]),
            sdl_fm("SLP-A1 RED: 写者 %c 保留段不是自身序列的尾部（丢最旧应保留每写者最后 k 条；"
                   "first_seq=%u + count=%u != %u）。", tags[w], first_seq[w], per_tag[w],
                   SDL_RECORDS_PER_WRITER));
    }

    kidpal_sd_log_ring_destroy(r);
}

/* ========================================================================== *
 * TEST 3 — SLP-A2 轮转与 8.3 命名（KPLOG000..KPLOG031 wrap / 512KB 分片 /
 * 32 个封顶 / 目录总量 16MB）。
 * coverage: SLP-A2 -- 文件名逐值 + 8.3 形态（8 字符基名 + 3 字符扩展、全大写）/
 *           512KB 分片边界切档 / 31 之后 wrap 回 000 / 32 个封顶 / 512KB*32 == 16MB
 * RED today（设计态）: 命名 / 轮转 seam 未实现（weak 哨兵返回 -99 / NULL）。
 * ========================================================================== */
TEST_CASE("sd log: 8.3 rotation naming KPLOG000..KPLOG031 wrap, 512KB, 16MB cap (SLP-A2)",
          "[sd_log][SLP-A2][linux][behavior]")
{
    char name[KIDPAL_SD_LOG_NAME_CAP];
    char expect[KIDPAL_SD_LOG_NAME_CAP];
    kidpal_sd_log_rot_t *rot;
    unsigned i;

    /* ---- 文件名逐值（8.3：KPLOG + 3 位十进制 + .TXT） --------------------- */
    {
        static const unsigned probes[] = { 0u, 1u, 9u, 10u, 31u };
        size_t k;

        for (k = 0; k < sizeof(probes) / sizeof(probes[0]); k++) {
            memset(name, 0, sizeof(name));
            TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                kidpal_sd_log_file_name(probes[k], name, (unsigned)sizeof(name)),
                sdl_fm("SLP-A2 RED: file_name(%u) 未返回 0——命名 seam 未实现（weak 哨兵 -99）"
                       "或拒绝合法入参。", probes[k]));
            snprintf(expect, sizeof(expect), "KPLOG%03u.TXT", probes[k]);
            TEST_ASSERT_EQUAL_STRING_MESSAGE(expect, name,
                sdl_fm("SLP-A2 RED: file_name(%u) 取值错（期望 %s）。", probes[k], expect));
            TEST_ASSERT_EQUAL_INT_MESSAGE(12, (int)strlen(name),
                sdl_fm("SLP-A2 RED: %s 长度 != 12——8.3 形态（8 字符基名 + '.' + 3 字符扩展）"
                       "被破坏，存在 LFN 依赖。", name));
            TEST_ASSERT_EQUAL_INT_MESSAGE('K', (int)name[0], "SLP-A2 RED: 基名前缀非 KPLOG。");
            for (i = 5; i <= 7; i++) {
                TEST_ASSERT_TRUE_MESSAGE(name[i] >= '0' && name[i] <= '9',
                    sdl_fm("SLP-A2 RED: %s 第 %u 位不是十进制数字。", name, i));
            }
            TEST_ASSERT_EQUAL_INT_MESSAGE('.', (int)name[8], "SLP-A2 RED: 扩展名分隔符非 '.'。");
            TEST_ASSERT_EQUAL_STRING_MESSAGE("TXT", name + 9,
                sdl_fm("SLP-A2 RED: %s 扩展名非 TXT（必须全大写、零 LFN 依赖）。", name));
        }
    }

    /* ---- 越界 / 非法入参 ---------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_sd_log_file_name((unsigned)KIDPAL_SD_LOG_FILE_COUNT, name, (unsigned)sizeof(name)),
        "SLP-A2 RED: file_name(32) 未返回 -1——封顶 32 个文件，禁出 KPLOG032.TXT。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_file_name(100u, name, (unsigned)sizeof(name)),
        "SLP-A2 RED: file_name(100) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_file_name(0u, NULL, 32u),
        "SLP-A2 RED: file_name(NULL out) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_file_name(0u, name, 12u),
        "SLP-A2 RED: file_name(cap=12) 未返回 -1（\"KPLOG000.TXT\" 需 12 字节 + NUL = 13）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_file_name(0u, name, 13u),
        "SLP-A2 RED: file_name(cap=13) 未返回 0——最小可用容量被拒。");

    /* ---- 目录总量：512KB × 32 == 16MB --------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(16 * 1024 * 1024,
        (int)((unsigned long long)KIDPAL_SD_LOG_FILE_BYTES * KIDPAL_SD_LOG_FILE_COUNT),
        "SLP-A2 RED: 冻结数值不自洽——FILE_BYTES × FILE_COUNT != 16MB（目录总量封顶义务）。");

    /* ---- 512KB 分片边界：恰好到界即切档 ------------------------------------- */
    rot = kidpal_sd_log_rot_init();
    TEST_ASSERT_NOT_NULL_MESSAGE(rot,
        "SLP-A2 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_rot_init 返回 NULL）——"
        "轮转物理状态未落地；builder 落地 diagnostics/sd_log.c 后转绿");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_rot_index(rot),
        "SLP-A2 RED: 新轮转器起始文件号 != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_rot_bytes_in_file(rot),
        "SLP-A2 RED: 新轮转器起始字节数 != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_sd_log_rot_account(rot, (unsigned)(511u * 1024u)),
        "SLP-A2 RED: 511KB 未到分片边界却切档。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_rot_index(rot),
        "SLP-A2 RED: 未到界时文件号被推进。");
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)(511u * 1024u),
        (int)kidpal_sd_log_rot_bytes_in_file(rot),
        "SLP-A2 RED: 字节计量不等于写入量。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_rot_account(rot, 1024u),
        "SLP-A2 RED: 恰好写满 512KB（511KB + 1KB）未切档。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_rot_index(rot),
        "SLP-A2 RED: 512KB 封口后文件号未 +1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_rot_bytes_in_file(rot),
        "SLP-A2 RED: 切档后新文件字节数未归零。");

    /* 单次 account 越过边界：封口 + 下一文件从 0 起算（超出部分不计入下一文件）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_sd_log_rot_account(rot, (unsigned)(KIDPAL_SD_LOG_FILE_BYTES + 100u)),
        "SLP-A2 RED: 单次写入越过 512KB 边界未切档。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, kidpal_sd_log_rot_index(rot),
        "SLP-A2 RED: 越界写入后文件号推进量 != 1（一次写入只封一个文件）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_rot_bytes_in_file(rot),
        "SLP-A2 RED: 越界切档后新文件字节数未归零。");

    /* ---- 31 之后 wrap 回 000（32 个封顶后覆盖最旧） -------------------------- */
    for (i = 0; i < 30u; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_sd_log_rot_account(rot, (unsigned)KIDPAL_SD_LOG_FILE_BYTES),
            sdl_fm("SLP-A2 RED: 第 %u 次满档写入未切档。", i));
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)((2u + i + 1u) % KIDPAL_SD_LOG_FILE_COUNT),
            kidpal_sd_log_rot_index(rot),
            sdl_fm("SLP-A2 RED: 文件号推进错（第 %u 次切档后应为 %u）。", i,
                   (unsigned)((2u + i + 1u) % KIDPAL_SD_LOG_FILE_COUNT)));
    }
    /* 此刻 index == 0：从 2 连转 30 次 = 32 % 32 = 0（已越过 31 回绕）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_rot_index(rot),
        "SLP-A2 RED: 转满 32 个文件后未 wrap 回 KPLOG000（目录 32 个封顶 / 覆盖最旧义务）。");

    /* 再跑一整轮：索引始终落在 0..31，且完整一轮后回到起点。 */
    {
        int start = kidpal_sd_log_rot_index(rot);

        for (i = 0; i < KIDPAL_SD_LOG_FILE_COUNT; i++) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(1,
                kidpal_sd_log_rot_account(rot, (unsigned)KIDPAL_SD_LOG_FILE_BYTES),
                sdl_fm("SLP-A2 RED: 完整一轮第 %u 次切档失败。", i));
            TEST_ASSERT_TRUE_MESSAGE(kidpal_sd_log_rot_index(rot) >= 0 &&
                                     kidpal_sd_log_rot_index(rot) < (int)KIDPAL_SD_LOG_FILE_COUNT,
                "SLP-A2 RED: 文件号越界（必须恒在 0..31，禁 KPLOG032+）。");
            snprintf(expect, sizeof(expect), "KPLOG%03d.TXT", kidpal_sd_log_rot_index(rot));
            memset(name, 0, sizeof(name));
            TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                kidpal_sd_log_file_name((unsigned)kidpal_sd_log_rot_index(rot), name,
                                        (unsigned)sizeof(name)),
                "SLP-A2 RED: 轮转器文件号不能生成合法 8.3 名。");
            TEST_ASSERT_EQUAL_STRING_MESSAGE(expect, name,
                "SLP-A2 RED: 轮转文件号与 8.3 命名不同源（索引 -> 文件名映射不一致）。");
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(start, kidpal_sd_log_rot_index(rot),
            "SLP-A2 RED: 完整一轮（32 次切档）后文件号未回到起点——wrap 周期 != 32。");
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_rot_account(NULL, 1u),
        "SLP-A2 RED: account(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_sd_log_rot_index(NULL),
        "SLP-A2 RED: rot_index(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_rot_bytes_in_file(NULL),
        "SLP-A2 RED: bytes_in_file(NULL) 未返回 0。");
    kidpal_sd_log_rot_destroy(rot);
    kidpal_sd_log_rot_destroy(NULL); /* NULL 安全（不得崩） */
}

/* ========================================================================== *
 * TEST 4 — SLP-A3 早启动留存语义（init 早于挂载 / 未 attach 累积不清空 /
 * attach 后从最旧 drain）。
 * coverage: SLP-A3 -- 未 attach 期间写入累积、peek 零消耗；attach 不清空缓冲；
 *           attach 后从最旧条目开始 drain（NVS/WiFi 早期日志不丢）；detach/attach
 *           往返不丢内容
 * RED today（设计态）: 留存语义 seam 未实现（weak 哨兵）。
 * ========================================================================== */
TEST_CASE("sd log: early-boot records survive until attach, drained oldest-first (SLP-A3)",
          "[sd_log][SLP-A3][linux][behavior]")
{
    static unsigned char storage[4096];
    static const char *const early[] = { "early-0", "early-1", "early-2", "early-3", "early-4" };
    kidpal_sd_log_ring_t *r;
    char out[KIDPAL_SD_LOG_RECORD_MAX + 8];
    unsigned len;
    unsigned i;

    r = kidpal_sd_log_ring_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(r,
        "SLP-A3 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）——"
        "早启动留存语义无法取证；builder 落地 diagnostics/sd_log.c 后转绿");

    /* ---- 未 attach 期间累积（模拟 NVS/WiFi 早期日志） ---------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attached(r),
        "SLP-A3 RED: 新环 attached 初始值应为 0。");
    for (i = 0; i < 5u; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)strlen(early[i]),
            kidpal_sd_log_ring_write(r, early[i], (unsigned)strlen(early[i])),
            sdl_fm("SLP-A3 RED: 未 attach 期间写入 %s 被拒绝（缓冲必须从 app_main 起累积）。",
                   early[i]));
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A3 RED: 未 attach 期间 5 条记录未累积（pending != 5）。");

    /* 未 attach：peek 恒 0 且零消耗（= 无卡零写盘；内容保留待 attach 后 drain）。 */
    memset(out, 0, sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out)),
        "SLP-A3 RED: 未 attach 时 peek 取到了记录——无卡态必须静默失效（零 fopen 零写盘）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A3 RED: 未 attach 时 peek 消耗了记录——内容必须在 attach 后仍可 drain。");

    /* ---- attach：不清空缓冲 -------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attach(r),
        "SLP-A3 RED: attach 合法入参未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_attached(r),
        "SLP-A3 RED: attach 后 attached != 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A3 RED: attach 清空了缓冲（pending != 5）——早启动留存语义被破坏。");

    /* ---- 从**最旧**开始 drain（不是最新、不是任意） ------------------------ */
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE((int)strlen(early[0]), (int)len,
        "SLP-A3 RED: attach 后 peek 未取到最旧早期条目。");
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE(early[0], out,
        "SLP-A3 RED: attach 后首个 drain 条目不是最旧条目（NVS/WiFi 早期日志被跳过 / 乱序）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A3 RED: commit 失败。");

    /* attach 后新写入追加在尾部。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(6, kidpal_sd_log_ring_write(r, "late-0", 6u),
        "SLP-A3 RED: attach 后写入返回非 6。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(6, kidpal_sd_log_ring_write(r, "late-1", 6u),
        "SLP-A3 RED: attach 后写入返回非 6。");

    /* 余下顺序 = early-1..early-4, late-0, late-1（FIFO / 早期先于晚期）。 */
    {
        static const char *const order[] = {
            "early-1", "early-2", "early-3", "early-4", "late-0", "late-1"
        };

        for (i = 0; i < 6u; i++) {
            memset(out, 0, sizeof(out));
            len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
            TEST_ASSERT_EQUAL_INT_MESSAGE((int)strlen(order[i]), (int)len,
                sdl_fm("SLP-A3 RED: 第 %u 条 drain 长度错。", i));
            out[len] = '\0';
            TEST_ASSERT_EQUAL_STRING_MESSAGE(order[i], out,
                sdl_fm("SLP-A3 RED: drain 顺序错（第 %u 条应为 %s）——早期日志必须整体先于"
                       "晚期日志落地。", i, order[i]));
            TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r),
                sdl_fm("SLP-A3 RED: 第 %u 条 commit 失败。", i));
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r),
            "SLP-A3 RED: 全量 drain 后 pending != 0。");
    }

    /* ---- detach / 重新 attach 往返：内容不得丢失 --------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, kidpal_sd_log_ring_write(r, "after-detach", 12u),
        "SLP-A3 RED: 写入返回非 12。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_detach(r),
        "SLP-A3 RED: detach 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attached(r),
        "SLP-A3 RED: detach 后 attached != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A3 RED: detach 清空了缓冲（pending != 1）——必须只切标志、不动内容。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attach(r), "SLP-A3 RED: 重新 attach 失败。");
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, (int)len, "SLP-A3 RED: 重新 attach 后记录长度错。");
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("after-detach", out,
        "SLP-A3 RED: detach/attach 往返丢失了缓冲内容。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A3 RED: commit 失败。");

    kidpal_sd_log_ring_destroy(r);
}

/* ========================================================================== *
 * TEST 5 — SLP-A5 降级与重试语义（无卡零写盘 / 写失败 30s 重试 / 不丢条 /
 * 恢复后继续 drain）。
 * coverage: SLP-A5 -- should_flush 未 attach 恒 0（零 fopen 零写盘纯逻辑承载面）/
 *           retry_due 30000ms 窗口决策表（29999 不重试）/ 写失败不 commit -> 同一条
 *           仍是最旧、重试成功后继续 drain / 未 attach 期间缓冲有界（静默失效 !=
 *           无限增长）
 * RED today（设计态）: 降级 / 重试 seam 未实现（weak 哨兵 -99）。
 * ========================================================================== */
TEST_CASE("sd log: degrade to silent no-write and 30s retry window keeps records (SLP-A5)",
          "[sd_log][SLP-A5][linux][behavior]")
{
    static unsigned char storage[4096];
    kidpal_sd_log_ring_t *r;
    char out[KIDPAL_SD_LOG_RECORD_MAX + 8];
    char rec[64];
    unsigned len;
    unsigned i;

    /* ---- retry_due 决策表（30s 窗口） -------------------------------------- */
    SDL_REQUIRE_IMPL_INT(kidpal_sd_log_retry_due(1, 30000UL), "kidpal_sd_log_retry_due");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_retry_due(0, 999999UL),
        "SLP-A5 RED: 无失败态（write_failed=0）却要求重试。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_retry_due(1, 29999UL),
        "SLP-A5 RED: 失败后 29999ms 就重试——30s 重试窗口过短（会持续打坏卡）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_retry_due(1, 30000UL),
        "SLP-A5 RED: 失败后 30000ms 未重试——坏卡 / 拔卡后永不恢复写盘。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_retry_due(1, 60000UL),
        "SLP-A5 RED: 长时失败后仍未重试（重试窗口必须持续到期）。");

    /* ---- should_flush 决策表（未 attach = 零写盘；1s 或 4KB 批量） ---------- */
    SDL_REQUIRE_IMPL_INT(kidpal_sd_log_should_flush(1, 1, 4096, 0UL),
                         "kidpal_sd_log_should_flush");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_sd_log_should_flush(0, 10u, 100000u, 999999UL),
        "SLP-A5 RED: 未 attach（无卡）时 should_flush != 0——无卡必须零 fopen 零写盘。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_should_flush(1, 0u, 100000u, 999999UL),
        "SLP-A5 RED: 无待刷记录时 should_flush != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_should_flush(1, 1u, 100u, 999UL),
        "SLP-A5 RED: 未到 1s 且未到 4KB 时 should_flush != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_should_flush(1, 1u, 100u, 1000UL),
        "SLP-A5 RED: 满 1s 时 should_flush != 1——1s 批量刷盘义务被破坏。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_should_flush(1, 1u, 4096u, 0UL),
        "SLP-A5 RED: 满 4KB 时 should_flush != 1——4KB 批量刷盘义务被破坏。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_should_flush(1, 1u, 4095u, 0UL),
        "SLP-A5 RED: 4095B 就要求刷盘——批量阈值错（应为 >= 4096）。");

    /* ---- 写失败 -> 不 commit -> 同一条仍是最旧；重试成功后继续 drain -------- */
    r = kidpal_sd_log_ring_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(r,
        "SLP-A5 RED: 冻结 seam 未实现（weak 哨兵 kidpal_sd_log_ring_init 返回 NULL）——"
        "降级 / 重试语义无法取证；builder 落地 diagnostics/sd_log.c 后转绿");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attach(r), "SLP-A5 RED: attach 失败。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, kidpal_sd_log_ring_write(r, "R-one", 5u),
        "SLP-A5 RED: write(\"R-one\") 返回非 5。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, kidpal_sd_log_ring_write(r, "R-two", 5u),
        "SLP-A5 RED: write(\"R-two\") 返回非 5。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, kidpal_sd_log_ring_write(r, "R-three", 7u),
        "SLP-A5 RED: write(\"R-three\") 返回非 7。");

    /* 第一次写盘失败：peek 到 R-one 但不 commit（同一条留给 30s 后重试）。 */
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("R-one", out, "SLP-A5 RED: 首次 drain 未取到最旧记录。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A5 RED: peek 之后 pending 变了（peek 必须无副作用）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_retry_due(1, 29999UL),
        "SLP-A5 RED: 失败后未到 30s 就重试。");

    /* 重试到期：同一条仍在最旧位置（写失败不得丢条）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_retry_due(1, 30000UL),
        "SLP-A5 RED: 30s 到期未重试。");
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("R-one", out,
        "SLP-A5 RED: 重试时最旧记录变了——写失败丢条 / 乱序（缓冲必须保留到 commit）。");

    /* 重试成功：commit 后继续 drain 下一条。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r),
        "SLP-A5 RED: 重试成功后的 commit 失败。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A5 RED: 恢复后 pending != 2。");
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("R-two", out,
        "SLP-A5 RED: 恢复后未从缓冲继续 drain（下一条应为 R-two）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A5 RED: commit 失败。");
    memset(out, 0, sizeof(out));
    len = kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out));
    out[len] = '\0';
    TEST_ASSERT_EQUAL_STRING_MESSAGE("R-three", out, "SLP-A5 RED: 第三条顺序错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_sd_log_ring_commit(r), "SLP-A5 RED: commit 失败。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_pending(r),
        "SLP-A5 RED: 全量 drain 后 pending != 0。");
    kidpal_sd_log_ring_destroy(r);

    /* ---- 未 attach 期间：静默失效但缓冲有界（不得无限增长） ---------------- */
    r = kidpal_sd_log_ring_init(storage, (unsigned)sizeof(storage));
    TEST_ASSERT_NOT_NULL_MESSAGE(r, "SLP-A5 RED: 二次 init 失败（存储复用被拒）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_sd_log_ring_attached(r),
        "SLP-A5 RED: 二次 init 后 attached != 0。");
    for (i = 0; i < 200u; i++) {
        sdl_fill64(rec, i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(64, kidpal_sd_log_ring_write(r, rec, 64u),
            sdl_fm("SLP-A5 RED: 无卡态第 %u 条写入被拒——无卡只应静默不落盘，不得拒绝采集。", i));
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_sd_log_ring_dropped(r) > 0u,
        "SLP-A5 RED: 无卡态写 200×64B 却零丢弃——缓冲无界增长（掉内存）风险。");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_sd_log_ring_pending(r) > 0u &&
                             kidpal_sd_log_ring_pending(r) <= 64u,
        sdl_fm("SLP-A5 RED: 无卡态缓冲保留量异常（pending=%u，4096B 容量下应 <= 64 条）。",
               kidpal_sd_log_ring_pending(r)));
    memset(out, 0, sizeof(out));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)kidpal_sd_log_ring_peek(r, out, (unsigned)sizeof(out)),
        "SLP-A5 RED: 无卡态 peek 取到了记录——必须零 fopen 零写盘。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_sd_log_should_flush(0, kidpal_sd_log_ring_pending(r), 4096u, 999999UL),
        "SLP-A5 RED: 无卡态 should_flush != 0——写盘任务不得尝试开文件。");
    kidpal_sd_log_ring_destroy(r);
}

/* -------------------------------------------------------------------------- *
 * 结构断言扫描面（源锚；相对仓根）
 * ------------------------------------------------------------------------ */

#define SDL_MAIN_CC    "firmware/main/main.cc"
#define SDL_SDLOG_H    "firmware/main/diagnostics/sd_log.h"
#define SDL_SDLOG_FS   "firmware/main/diagnostics/sd_log_fs.cc"
#define SDL_BOARD_CC   "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define SDL_MAIN_CMAKE "firmware/main/CMakeLists.txt"
#define SDL_HOST_CMAKE "firmware/main/test/host_app/main/CMakeLists.txt"

#define SDL_FN_WINDOW   3000
#define SDL_BODY_WINDOW 4000

/* ========================================================================== *
 * TEST 6 — SLP-A4 ① 采集接线：main.cc app_main 最早期装钩子（早于 nvs_flash_init）
 * + sd_log_fs.cc 钩子保存旧 vprintf 并**先转调再写缓冲**。
 * coverage: SLP-A4 -- main.cc app_main 内 KidpalSdLogInit() ->
 *           esp_log_set_vprintf(KidpalSdLogVprintf) -> KidpalSdLogChainPreviousVprintf，
 *           全部早于首个 nvs_flash_init；钩子函数体内 s_prev_vprintf( 调用先于
 *           kidpal_sd_log_ring_write
 * RED today（设计态）: main.cc 无钩子安装、sd_log_fs.cc 未落地。
 * ========================================================================== */
TEST_CASE("sd log: app_main installs chained vprintf hook before NVS init (SLP-A4)",
          "[sd_log][SLP-A4][structural]")
{
    char *code;
    char *raw;
    const char *fn;
    const char *fn_end;
    const char *p_init;
    const char *p_set;
    const char *p_chain;
    const char *p_nvs;
    int n;

    SDL_REQUIRE_ROOT();

    /* ---- main.cc：安装序 ---------------------------------------------------- */
    code = sdl_strip_file(SDL_MAIN_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SLP-A4 RED(infra): main.cc unreadable");
    raw = sdl_read(SDL_MAIN_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "SLP-A4 RED(infra): main.cc unreadable");

    /* 扫描器自检 ①：行级双针计数器对已知在世形态能命中（计数器坏 = 全部断言失咬合力）。 */
    n = sdl_count_lines_with_both(code, "esp_err_t", "nvs_flash_init");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        sdl_fm("SLP-A4 RED(infra): 行级双针计数器失灵（main.cc 已知在世形态 "
               "esp_err_t × nvs_flash_init 只命中 %d 行）——扫描器坏，本文件结构断言失去"
               "咬合力", n));
    /* 扫描器自检 ②：注释剥离生效（只出现在注释里的锚不得假绿）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "Initialize NVS flash for WiFi") != NULL,
        "SLP-A4 RED(infra): main.cc 已知注释锚缺席——注释剥离自检无法成立（main.cc 被重写？）。");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "Initialize NVS flash for WiFi") == NULL,
        "SLP-A4 RED(infra): 注释剥离失效（已知注释文本仍留在可执行源里）——本文件全部结构"
        "断言会因注释文本假绿，须复核扫描器");
    free(raw);

    fn = strstr(code, "void app_main");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "SLP-A4 RED(infra): main.cc 无 void app_main 定义锚——入口被改名，须复核锚不得改判为绿");
    fn_end = fn + SDL_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }

    p_init = sdl_find_in(fn, fn_end, "KidpalSdLogInit();");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_init,
        "SLP-A4 RED: app_main 未调用 KidpalSdLogInit()——PSRAM 环形缓冲 + 写盘任务未在启动"
        "最早期就绪（钩子若先于缓冲安装会丢早期日志；契约见头注 [C4]/[C5]）");
    p_set = sdl_find_in(fn, fn_end, "esp_log_set_vprintf(KidpalSdLogVprintf)");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_set,
        "SLP-A4 RED: app_main 未调用 esp_log_set_vprintf(KidpalSdLogVprintf)——esp_log 采集"
        "钩子未安装（设备日志无法进入环形缓冲）");
    p_chain = sdl_find_in(fn, fn_end, "KidpalSdLogChainPreviousVprintf(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_chain,
        "SLP-A4 RED: app_main 未把 esp_log_set_vprintf 的返回值交给 "
        "KidpalSdLogChainPreviousVprintf——旧 vprintf 指针未保存，钩子无法链式转调（串口"
        "输出会中断）");
    p_nvs = sdl_find_in(fn, fn_end, "nvs_flash_init");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_nvs,
        "SLP-A4 RED(infra): app_main 内未找到 nvs_flash_init——排序锚缺席，须复核锚不得改判为绿");

    /* 顺序：init < 安装钩子 < NVS 初始化（NVS/WiFi 早期日志不丢）。 */
    TEST_ASSERT_TRUE_MESSAGE(p_init < p_set,
        "SLP-A4 RED: KidpalSdLogInit() 晚于钩子安装——钩子可能在环形缓冲就绪前被调用（早期"
        "日志丢失 / 空指针风险）");
    TEST_ASSERT_TRUE_MESSAGE(p_set < p_nvs,
        "SLP-A4 RED: esp_log_set_vprintf 晚于 nvs_flash_init——NVS 初始化阶段的早期日志抓不到"
        "（契约 [C5]：钩子必须在 app_main 最早期、NVS 之前安装）");
    TEST_ASSERT_TRUE_MESSAGE(p_chain <= p_set + 200,
        "SLP-A4 RED: KidpalSdLogChainPreviousVprintf 与安装点相隔过远——旧指针保存必须与"
        " esp_log_set_vprintf 安装同一处（同一条语句 / 紧邻语句）");
    free(code);

    /* ---- sd_log_fs.cc：旧指针保存 + 链式转调 --------------------------------- */
    code = sdl_strip_file(SDL_SDLOG_FS);
    TEST_ASSERT_NOT_NULL_MESSAGE(code,
        "SLP-A4 RED: firmware/main/diagnostics/sd_log_fs.cc 未落地——钩子保存旧函数指针 + "
        "链式转调义务无承载（串口输出会中断）");

    n = sdl_count_lines_with_both(code, "s_prev_vprintf", "static");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SLP-A4 RED: 未见 `static ... s_prev_vprintf` 声明——旧 vprintf 指针保存面缺席"
        "（变量名字面已冻结，见头注 [C4]）");
    {
        const char *chain = strstr(code, "KidpalSdLogChainPreviousVprintf");
        const char *chain_end;

        TEST_ASSERT_NOT_NULL_MESSAGE(chain,
            "SLP-A4 RED: sd_log_fs.cc 无 KidpalSdLogChainPreviousVprintf 定义——安装返回的"
            "旧指针无处保存");
        chain_end = chain + SDL_FN_WINDOW;
        if ((size_t)(chain_end - code) > strlen(code)) {
            chain_end = code + strlen(code);
        }
        TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(chain, chain_end, "s_prev_vprintf") != NULL,
            "SLP-A4 RED: KidpalSdLogChainPreviousVprintf 未把入参存入 s_prev_vprintf——链式"
            "转调链断裂");
    }
    {
        const char *hook = strstr(code, "int KidpalSdLogVprintf(");
        const char *hook_end;
        const char *prev_call;
        const char *ring_write;

        TEST_ASSERT_NOT_NULL_MESSAGE(hook,
            "SLP-A4 RED: sd_log_fs.cc 无 int KidpalSdLogVprintf(...) 定义——esp_log 钩子本体"
            "缺席");
        hook_end = hook + SDL_BODY_WINDOW;
        if ((size_t)(hook_end - code) > strlen(code)) {
            hook_end = code + strlen(code);
        }
        prev_call = sdl_find_in(hook, hook_end, "s_prev_vprintf(");
        TEST_ASSERT_NOT_NULL_MESSAGE(prev_call,
            "SLP-A4 RED: 钩子体内未调用 s_prev_vprintf(——未链式转调原 vprintf，串口（COM5）"
            "日志输出会停");
        ring_write = sdl_find_in(hook, hook_end, "kidpal_sd_log_ring_write(");
        TEST_ASSERT_NOT_NULL_MESSAGE(ring_write,
            "SLP-A4 RED: 钩子体内未调用 kidpal_sd_log_ring_write(——日志未进入环形缓冲");
        TEST_ASSERT_TRUE_MESSAGE(prev_call < ring_write,
            "SLP-A4 RED: 钩子先写缓冲再转调原 vprintf——契约 [C4] 要求**先转调再写缓冲**"
            "（串口时延 / 递归风险形态）");
    }

    free(code);
}

/* ========================================================================== *
 * TEST 7 — SLP-A4 ②③ 板级接线：挂载成功分支 attach 序 + max_files = 8。
 * coverage: SLP-A4 -- InitializeSdCard 内 s_sd_mounted = true 之后、
 *           RunBootRoundtripProbe() 之前调用 KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)；
 *           mount_config.max_files 字面 8
 * RED today（设计态）: 板 .cc 无 attach 调用、max_files 仍 5。
 * ========================================================================== */
TEST_CASE("sd log: board attaches log store on mount success, max_files=8 (SLP-A4)",
          "[sd_log][SLP-A4][structural]")
{
    char *code;
    char *winbuf;
    const char *fn;
    const char *fn_end;
    const char *p_true;
    const char *p_attach;
    const char *p_probe;
    long v = -1;

    SDL_REQUIRE_ROOT();

    code = sdl_strip_file(SDL_BOARD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SLP-A4 RED(infra): 板 .cc unreadable");

    fn = strstr(code, "void InitializeSdCard");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "SLP-A4 RED(infra): 板 .cc 无 void InitializeSdCard 定义锚——SD 挂载函数被改名，"
        "须复核锚不得改判为绿");
    fn_end = fn + SDL_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    winbuf = sdl_window_copy(fn, fn_end);

    /* ---- max_files = 8（5 -> 8） -------------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(sdl_first_int_after_on_line(winbuf, "max_files", &v) && v == 8,
        sdl_fm("SLP-A4 RED: InitializeSdCard 内 mount_config.max_files 取值非字面 8（取到 %ld）"
               "——FD 预算未给日志文件 + story_sd_io + 启动探针留头（宏间接 / 符号常量形态"
               "取值失败同红，须复核锚不得改判为绿）", v));

    /* ---- attach 序：成功置真之后、boot 探针之前 ----------------------------- */
    p_true = sdl_find_in(fn, fn_end, "s_sd_mounted = true");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_true,
        "SLP-A4 RED(infra): InitializeSdCard 内未找到 s_sd_mounted = true——成功分支锚缺席"
        "（挂载成功判定被改写？须复核锚不得改判为绿）");
    p_attach = sdl_find_in(fn, fn_end, "KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_attach,
        "SLP-A4 RED: 挂载成功分支未调用 KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)——诊断日志"
        "写盘任务拿不到挂载点，落卡链未接线（契约 [C5]）");
    p_probe = sdl_find_in(fn, fn_end, "RunBootRoundtripProbe();");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_probe,
        "SLP-A4 RED(infra): InitializeSdCard 内未找到 RunBootRoundtripProbe();——序锚缺席"
        "（既有 SD 探针被删/改名？须复核锚不得改判为绿）");

    TEST_ASSERT_TRUE_MESSAGE(p_true < p_attach,
        "SLP-A4 RED: KidpalSdLogAttach 出现在 s_sd_mounted = true 之前——可能落在失败 / "
        "降级分支（无卡时 attach 无意义，且契约要求只在成功分支）");
    TEST_ASSERT_TRUE_MESSAGE(p_attach < p_probe,
        "SLP-A4 RED: KidpalSdLogAttach 晚于 RunBootRoundtripProbe()——契约 [C5] 要求 attach "
        "在挂载成功之后、开机 SD 探针之前（探针与日志任务共享 /sdcard FD 与写序）");

    free(winbuf);
    free(code);
}

/* ========================================================================== *
 * TEST 8 — SLP-A4 ④⑤ + SLP-A5 结构半 + 两处登记守：PSRAM 128KB 分配 / 写盘任务
 * 低优先级与零 ESP_LOG / 降级不 panic / 纯逻辑决策入口被消费 / CMakeLists 登记。
 * coverage: SLP-A4 -- heap_caps_malloc(KIDPAL_SD_LOG_BUFFER_BYTES, MALLOC_CAP_SPIRAM) +
 *           BUFFER_BYTES = 128*1024 + xTaskCreate(..., KIDPAL_SD_LOG_TASK_PRIO) +
 *           写盘任务函数体内零 ESP_LOG*；SLP-A5 -- 芯片面零 ESP_ERROR_CHECK( / abort(
 *           panic 形态 + should_flush / retry_due / ring_peek / ring_commit 被消费；
 *           SLP-A2 -- 头文件冻结数值单源（FILE_BYTES 512*1024 / FILE_COUNT 32）。
 *           另含两处 CMakeLists 登记守（anti-vacuity / 设备构建面，非 acceptance 条目）。
 * RED today（设计态）: sd_log.h / sd_log_fs.cc / 两处 CMakeLists 登记均缺席。
 * ========================================================================== */
TEST_CASE("sd log: PSRAM 128KB ring, low-prio writer task without ESP_LOG, no panic (SLP-A4/A5)",
          "[sd_log][SLP-A4][SLP-A5][structural]")
{
    char *hdr;
    char *code;
    char *cmake;
    const char *fn;
    const char *fn_end;
    const char *p;
    char *body;
    long v = -1;
    int n;

    SDL_REQUIRE_ROOT();

    /* ---- sd_log.h：冻结数值单源（EQ-2） ------------------------------------- */
    hdr = sdl_strip_file(SDL_SDLOG_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr,
        "SLP-A4 RED: firmware/main/diagnostics/sd_log.h 未落地——纯逻辑 seam 与冻结数值"
        "单源缺席（契约 [C1]-[C4]）");

    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_RECORD_MAX",
                                                       "512") >= 1,
        "SLP-A4 RED: sd_log.h 未取到 #define KIDPAL_SD_LOG_RECORD_MAX 与 512 同行——单条记录"
        "上限冻结值漂移（行为断言用的镜像常量失去锚）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_BUFFER_BYTES",
                                                       "128") >= 1 &&
                             sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_BUFFER_BYTES",
                                                       "1024") >= 1,
        "SLP-A4 RED: sd_log.h 的 KIDPAL_SD_LOG_BUFFER_BYTES 非同行的 128 × 1024——PSRAM 环形"
        "缓冲容量冻结值（128KB）不成立");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_FILE_BYTES",
                                                       "512") >= 1 &&
                             sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_FILE_BYTES",
                                                       "1024") >= 1,
        "SLP-A4 RED: sd_log.h 的 KIDPAL_SD_LOG_FILE_BYTES 非同行的 512 × 1024——512KB 分片"
        "冻结值不成立（SLP-A2）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_FILE_COUNT",
                                                       "32") >= 1,
        "SLP-A4 RED: sd_log.h 的 KIDPAL_SD_LOG_FILE_COUNT 非 32——32 个封顶 / 16MB 目录总量"
        "冻结值不成立（SLP-A2）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_FLUSH_MS",
                                                       "1000") >= 1,
        "SLP-A5 RED: sd_log.h 的 KIDPAL_SD_LOG_FLUSH_MS 非 1000——1s 批量刷盘冻结值不成立");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_FLUSH_BYTES",
                                                       "4096") >= 1,
        "SLP-A5 RED: sd_log.h 的 KIDPAL_SD_LOG_FLUSH_BYTES 非 4096——4KB 批量刷盘冻结值不成立");
    TEST_ASSERT_TRUE_MESSAGE(sdl_count_lines_with_both(hdr, "#define KIDPAL_SD_LOG_RETRY_MS",
                                                       "30000") >= 1,
        "SLP-A5 RED: sd_log.h 的 KIDPAL_SD_LOG_RETRY_MS 非 30000——30s 写失败重试窗口冻结值"
        "不成立");
    TEST_ASSERT_TRUE_MESSAGE(
        sdl_first_int_after_on_line(hdr, "#define KIDPAL_SD_LOG_TASK_PRIO", &v) && v == 1,
        sdl_fm("SLP-A5 RED: sd_log.h 的 KIDPAL_SD_LOG_TASK_PRIO 非字面 1（取到 %ld）——写盘任务"
               "必须低优先级，不得与语音 / AEC 争实时（契约 [C3]）", v));
    free(hdr);

    /* ---- sd_log_fs.cc：芯片面本体 ------------------------------------------- */
    code = sdl_strip_file(SDL_SDLOG_FS);
    TEST_ASSERT_NOT_NULL_MESSAGE(code,
        "SLP-A4 RED: firmware/main/diagnostics/sd_log_fs.cc 未落地——PSRAM 分配 / 写盘任务 / "
        "attach 均无承载");

    /* ④ PSRAM 128KB 分配 + 写盘任务低优先级。 */
    fn = strstr(code, "void KidpalSdLogInit");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "SLP-A4 RED: sd_log_fs.cc 无 KidpalSdLogInit 定义——启动期 PSRAM 缓冲初始化缺席");
    fn_end = fn + SDL_BODY_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "heap_caps_malloc") != NULL,
        "SLP-A4 RED: KidpalSdLogInit 内未调用 heap_caps_malloc——环形缓冲未从 PSRAM 分配");
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "MALLOC_CAP_SPIRAM") != NULL,
        "SLP-A4 RED: heap_caps_malloc 未带 MALLOC_CAP_SPIRAM——缓冲会落内部 RAM，挤占语音 / "
        "LVGL 堆（契约 [C4]）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "KIDPAL_SD_LOG_BUFFER_BYTES") != NULL,
        "SLP-A4 RED: 分配容量未取 KIDPAL_SD_LOG_BUFFER_BYTES（128KB 单源）——容量字面漂移"
        "风险（EQ-2）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "xTaskCreate") != NULL,
        "SLP-A4 RED: KidpalSdLogInit 内未创建写盘任务（xTaskCreate）——缓冲无人 drain");
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "KIDPAL_SD_LOG_TASK_PRIO") != NULL,
        "SLP-A5 RED: 写盘任务未用 KIDPAL_SD_LOG_TASK_PRIO 创建——低优先级义务无承载（可能与"
        "语音主链路争实时）");
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "SdLogWriterTask") != NULL,
        "SLP-A4 RED: 写盘任务入口未指向 SdLogWriterTask——冻结的自由函数形态缺席");

    /* ⑤ 写盘任务函数体内零 ESP_LOG*（防 vprintf 钩子递归回灌）。 */
    p = strstr(code, "static void SdLogWriterTask(");
    if (p == NULL) {
        p = strstr(code, "void SdLogWriterTask(");
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(p,
        "SLP-A4 RED: sd_log_fs.cc 无 SdLogWriterTask 自由函数定义——写盘任务本体缺席");
    {
        const char *body_end = sdl_body_end(p, SDL_BODY_WINDOW);

        body = sdl_window_copy(p, body_end);
    }
    n = sdl_count_sub(body, "ESP_LOG");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        sdl_fm("SLP-A4 RED: 写盘任务函数体内存在 %d 处 ESP_LOG* 调用——会经 vprintf 钩子"
               "递归回灌环形缓冲（掉日志 / 栈溢出风险）；写失败诊断须走非 esp_log 通道或"
               "在任务外记录", n));
    free(body);

    /* attach：驱动面必须把环置为 attached（不清缓冲由 [C1] 行为断言承载）。 */
    fn = strstr(code, "void KidpalSdLogAttach");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "SLP-A4 RED: sd_log_fs.cc 无 KidpalSdLogAttach 定义——板级挂载成功后无处 attach");
    fn_end = fn + SDL_BODY_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    TEST_ASSERT_TRUE_MESSAGE(sdl_find_in(fn, fn_end, "kidpal_sd_log_ring_attach(") != NULL,
        "SLP-A3 RED: KidpalSdLogAttach 内未调用 kidpal_sd_log_ring_attach(——环未进入可 drain "
        "状态（早启动累积的日志永远不落盘）");

    /* 纯逻辑决策入口必须在芯片面被消费（否则决策表是死代码，A1/A2/A5 行为绿不等于接线）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_should_flush(") != NULL,
        "SLP-A5 RED: sd_log_fs.cc 未消费 kidpal_sd_log_should_flush(——无卡零写盘 / 1s·4KB "
        "批量刷盘决策未接线");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_retry_due(") != NULL,
        "SLP-A5 RED: sd_log_fs.cc 未消费 kidpal_sd_log_retry_due(——30s 重试窗口决策未接线");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_ring_peek(") != NULL &&
                             strstr(code, "kidpal_sd_log_ring_commit(") != NULL,
        "SLP-A1 RED: sd_log_fs.cc 未消费 ring_peek/ring_commit——写盘失败可重试的 drain 语义"
        "未接线（peek 后 commit 才前进）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_file_name(") != NULL,
        "SLP-A2 RED: sd_log_fs.cc 未消费 kidpal_sd_log_file_name(——8.3 命名单源未接线"
        "（落盘文件名可能绕开 KPLOGnnn.TXT 模板 = LFN 依赖 / 封顶失效）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_sd_log_rot_account(") != NULL,
        "SLP-A2 RED: sd_log_fs.cc 未消费 kidpal_sd_log_rot_account(——512KB 分片 / 32 个"
        "封顶的轮转未接线（纯逻辑绿 != 真机会切档）");

    /* 降级不崩：芯片面零 panic / abort 形态（负向断言，剥注释后可执行源上计数）。 */
    n = sdl_count_sub(code, "ESP_ERROR_CHECK(");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        sdl_fm("SLP-A5 RED: sd_log_fs.cc 存在 %d 处裸 ESP_ERROR_CHECK(——写盘失败 / 无卡会 "
               "abort 重启，违背「降级不崩、不阻断语音主链路」（WITHOUT_ABORT 前缀形态不误伤）",
               n));
    n = sdl_count_sub(code, "abort(");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        sdl_fm("SLP-A5 RED: sd_log_fs.cc 存在 %d 处 abort(——诊断日志整层不得 abort 进程", n));
    free(code);

    /* ---- 两处 CMakeLists 登记守（anti-vacuity + 设备构建面） ---------------- */
    cmake = sdl_read(SDL_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "SLP-A4 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "sd_log.acceptance.c") != NULL,
        "SLP-A4 RED: host_app/main/CMakeLists.txt 未登记 sd_log.acceptance.c——本冻结验收件不进 "
        "host runner（全部断言不会被执行 = 空过绿；builder FIRST ACTION，见头注 Run protocol 0）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/sd_log.c") != NULL,
        "SLP-A4 RED: host_app/main/CMakeLists.txt 未登记纯逻辑源 diagnostics/sd_log.c——"
        "行为断言将只跑 weak 哨兵（EQ-1：哨兵红会掩盖「未接线」，反之接线后仍红说明登记错）");
    free(cmake);

    cmake = sdl_read(SDL_MAIN_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "SLP-A4 RED(infra): firmware/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/sd_log.c") != NULL,
        "SLP-A4 RED: firmware/main/CMakeLists.txt 显式 SOURCES 未登记 diagnostics/sd_log.c——"
        "main 目录非 GLOB，设备构建拿不到纯逻辑源（SLP-X2 设备编译闸必红）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/sd_log_fs.cc") != NULL,
        "SLP-A4 RED: firmware/main/CMakeLists.txt 显式 SOURCES 未登记 diagnostics/sd_log_fs.cc——"
        "芯片面钩子 / 写盘任务不进设备构建（SLP-X2 设备编译闸必红）");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-18 初版冻结（TaskPackage 261018-sd-log-persist; acceptance[] SLP-A1~A5 +
 *               SLP-A6 structural/live-manual 为冻结边界; reviewed plan + FROZEN
 *               PROPOSAL 决策 24 白名单增列 / ADR-012 为语义权威）。
 *               八个 TEST_CASE：SLP-A1 行为两件（环形缓冲有界 / 丢最旧 / FIFO drain
 *               / 多写者并发不撕裂）、SLP-A2 行为一件（8.3 命名 / 512KB 分片 / 32 个
 *               封顶 / wrap / 16MB）、SLP-A3 行为一件（早启动累积 -> attach 不清空 ->
 *               最旧优先 drain）、SLP-A5 行为一件（无卡零写盘 / 30s 重试 / 恢复后继续
 *               drain）、SLP-A4 结构三件（main.cc 安装序 + 链式转调 / 板级 attach 序
 *               + max_files=8 / PSRAM 128KB + 任务内零 ESP_LOG + 零 panic + 两处
 *               CMakeLists 登记守）。SLP-A6 为 live-manual 真机轮观测项，不落自动断言
 *               （261012 RF-A5 / 261017 PV-A8 同款口径），本 Pod 到真机门前停等 human。
 *               冻结落盘冒烟 = 独立冒烟通道实编 + 实跑（设计态红逐条核对，红名见交付
 *               报告）+ 既有 host 全量套件零新增红判读；双向可满足性以 /tmp 一次性参考
 *               实现 + 合成树（KIDPAL_REPO_ROOT 注入）取证，不写入工作树。
 * --------------------------------------------------------------------------- */
