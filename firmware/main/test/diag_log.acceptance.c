/*
 * Frozen acceptance tests — TaskPackage 261022-diag-log-content
 * (subsystem: firmware/main/diagnostics（log_semantics 新纯逻辑单源：时间前缀格式化
 *  决策 + 预格式化缓存 + 语义名映射 + SystemInfo 降频决策）+ sd_log_fs.cc 钩子时间
 *  前缀拼接 + device_state_machine.cc 被拒转移补打 + display/kidpal_render_node.cc
 *  PAGE_SWITCH 行 + boards/waveshare-s3-rlcd-4.2 按键语义名 + application.cc
 *  SystemInfo 降频; rigor=lite; riskTier=medium; 板端诊断日志内容增强)。
 *
 * Story 语义权威 = reviewed plan（delivery/taskpackages/261022-diag-log-content/
 * plan.md，architect /triage-change 2026-10-22 比例化 review + human 触点①确认）。
 * human 立项原话（2026-10-22）：①「至少把每个页面和每个状态的跳转都记下来」；
 * ②「连时间都没记……至少得把当时的时间记下来」。human 拍板：档位 ②单包 Lite；
 * 墙上时间注入点 = 钩子层每行都带（ISR 风险知情选择，缓解 = 低频任务预格式化 +
 * 钩子只拼接，plan R1）。
 *
 * Nature: 行为级（纯逻辑决策函数 + weak 哨兵：log_semantics.c 未编入 host runner
 * 前哨兵恒返回 NULL / -99 = 「冻结契约未实现」设计态红，不是编译错——sd_log
 * 先例同款）+ 结构级（真实工作树源锚扫描，sd_card_mount / sd_log 先例形态；正向
 * 断言只认**注释剥离后可执行源**的字面形态——260917-m2-k3 sealing pass 教训：
 * 只出现在注释里的锚值会假绿）。
 *
 * 本文件**不 include** diagnostics/log_semantics.h（该头 builder 尚未落盘；落地后
 * 形状以本文件头注为冻结契约，story_cache / sd_log 先例）。落地后由 host runner
 * 的强符号覆盖本文件 weak 哨兵。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；home 路径为冻结落点
 * firmware/main/diagnostics/log_semantics.{h,c}——纯逻辑、零芯片依赖、零 ESP-IDF
 * include、零 FreeRTOS / 零 LVGL / 零 FATFS -> host_app linux target 与设备树两侧
 * 同源；设备侧经 firmware/main/CMakeLists.txt、host 侧经 host_app/main/
 * CMakeLists.txt 各自登记）:
 *
 *   [DLG-C1] 墙上时间前缀（纯逻辑；墙上时间字段**显式注入**——真实时钟读取归芯片
 *        面（application.cc 低频 tick 产出 / sd_log_fs.cc 钩子消费），本缝不依赖
 *        任何真实时钟）:
 *
 *          #define KIDPAL_LOG_TIME_PREFIX_CAP 32u
 *            -- "[2026-10-22 12:34:56] " = 22 字符 + NUL；BOOT 形态更短；32 足容。
 *
 *          int kidpal_log_time_format(int synced, int year, int mon, int mday,
 *                                     int hour, int min, int sec,
 *                                     unsigned long long boot_ms,
 *                                     char *out, unsigned cap);
 *            -- 格式化决策单源（无全局态、无真实时钟）:
 *               synced != 0 -> "[YYYY-MM-DD HH:MM:SS] "（year = 完整年份如 2026,
 *                   mon 1..12, mday 1..31, hour 0..23, min 0..59, sec 0..59;
 *                   任一字段越界 -> -1 且**零写入**（防御：不产出假时间）);
 *               synced == 0 -> "[BOOT+<boot_ms>ms] "（SNTP/RTC 未同步降级标注,
 *                   boot_ms = 开机毫秒数, 无上限; 不校验字段）;
 *               成功 -> 写入字符数（不含 NUL）; out == NULL 或 cap 不足 -> -1
 *               （零写入）。
 *
 *          void kidpal_log_time_cache_update(int synced, int year, int mon,
 *                                            int mday, int hour, int min,
 *                                            int sec, unsigned long long boot_ms);
 *            -- 预格式化缓存产出（R1 ISR 缓解：低频任务〔application.cc 1s clock
 *               tick〕调用；钩子**禁止**在 vprintf 回调内做时间格式化）。内部复用
 *               kidpal_log_time_format 的决策（单源一致）。
 *          const char *kidpal_log_time_cache(void);
 *            -- 取当前缓存前缀（钩子只 memcpy 拼接用）；**恒非 NULL**；未 update
 *               过 -> ""（空串：降级为无前缀，日志仍可读，绝不返回 NULL 崩钩子）。
 *
 *   [DLG-C2] 语义名映射（纯逻辑闭集 + fallback；int 入参解耦枚举头——host 侧不
 *        include key_gesture_parser.h / kidpal_render_node.h，映射表覆盖全部已知
 *        枚举值，越界回退串形态 `"<族>_<n>"`）:
 *
 *          const char *kidpal_log_action_name(int act);
 *            -- global_action_t（key_gesture_parser.h）0..5 ->
 *               "none" / "session_toggle" / "home" / "page_next" / "overlay_close" /
 *               "char_replay"; 其余（含负值）-> "act_<n>"。恒非 NULL。
 *          const char *kidpal_log_key_name(int key);
 *            -- key_id_t 0..1 -> "key" / "boot"; 其余 -> "key_<n>"。
 *          const char *kidpal_log_gesture_name(int g);
 *            -- key_gesture_t 0..3 -> "none" / "click" / "double_click" /
 *               "long_press"; 其余 -> "ges_<n>"。
 *          const char *kidpal_log_page_name(int page);
 *            -- kidpal_page_id_t（kidpal_render_node.h）0..2 -> "home" /
 *               "schedule" / "story"; 其余 -> "page_<n>"。
 *          const char *kidpal_log_trigger_name(int trg);
 *            -- PAGE_SWITCH 触发源 0..2 -> "key" / "timer" / "net"（plan 改动点 5）;
 *               其余 -> "trg_<n>"。
 *
 *   [DLG-C3] SystemInfo 降频决策（纯逻辑幂等函数；间隔数值单源，禁裸字面）:
 *
 *          #define KIDPAL_LOG_SYSINFO_INTERVAL_MS 60000LL   〔10s -> 60s〕
 *
 *          int kidpal_log_sysinfo_due(long long now, long long last,
 *                                     long long interval);
 *            -- interval <= 0 -> -1（防御）; now < last -> 0（时钟回退 fail-closed：
 *               噪声治理不因回退反弹成风暴）; now - last >= interval -> 1; 其余 -> 0。
 *               幂等：同入参恒同出（无隐藏状态）。
 *
 *   [DLG-C4] 接线（chip 面，host 不可达，只由结构断言扫源锚——sd_card_mount /
 *        sd_log TEST 6-8 先例形态。sd_log.acceptance.c [C4] 已钉的既有锚（链式转调
 *        顺序 / ring_write 消费 / 写盘任务零 ESP_LOG / 零 panic）**由该冻结件继续
 *        承载，本文件不重复不改动**；本文件只钉时间前缀与语义行**新增**锚）:
 *
 *     ① sd_log_fs.cc KidpalSdLogVprintf 体内：`kidpal_log_time_cache(` 调用在世，
 *        且位于 `s_prev_vprintf(`（链式转调，sd_log 先例锚）之后、
 *        `kidpal_sd_log_ring_write(`（环写，sd_log 先例锚）之前——预格式化前缀
 *        拼接进环写记录、钩子自身不做时间格式化（R1 纪律的可执行形态）。
 *     ② device_state_machine.cc TransitionTo：拒绝分支（IsValidTransition 失败、
 *        return false 路径）新增 `state_reject %s -> %s` 行（字面冻结），位于既有
 *        `State: %s -> %s` 行（:125 既有行为，保留）之前——被拒转移不再静默。
 *     ③ waveshare-s3-rlcd-4.2.cc：keya_dispatch_global_action 体内消费
 *        `kidpal_log_action_name(`，key_route_dispatch 行格式含 `action=%d(`
 *        （语义名括号后挂）且同行保留 `corr_id=%s`；keya_log_gesture 体内消费
 *        `kidpal_log_key_name(` / `kidpal_log_gesture_name(`，key_gesture_parser
 *        行格式含 `key=%d(` 与 `gesture=%d(` 且同行保留 `corr_id=%s`（corr_id
 *        纪律不动，plan 改动点 4）。
 *     ④ kidpal_render_node.cc：PAGE_SWITCH 一行在世（`PAGE_SWITCH` 行含
 *        `trigger=`，from→to 形态），体内消费 `kidpal_log_page_name(` /
 *        `kidpal_log_trigger_name(`（PAGE_RENDER in/out 行不动，plan 改动点 5）。
 *     ⑤ application.cc：时钟 tick 处 SystemInfo 打印经 `kidpal_log_sysinfo_due(`
 *        决策（裸 `clock_ticks_ % 10` 字面**退场**——负向断言），预格式化缓存
 *        产出方 `kidpal_log_time_cache_update(` 在世（低频任务 = main event loop
 *        1s clock tick，R1 产出侧；驱动方 <-> 消费方闭环：①消费产出）。
 *     ⑥ 头文件数值单源（EQ-2）：log_semantics.h `#define KIDPAL_LOG_SYSINFO_INTERVAL_MS`
 *        同行 60000、`#define KIDPAL_LOG_TIME_PREFIX_CAP` 同行 32。
 *
 *   [DLG-C5] 登记（anti-vacuity / 设备构建面）：host_app/main/CMakeLists.txt
 *        HOST_ACCEPTANCE_SRCS 登记 `diag_log.acceptance.c`（置于
 *        `host_infra_smoke.acceptance.c` 之前——smoke 末位 = 最先执行，包 0
 *        boundary 齿注册序守卫先例）与 `diagnostics/log_semantics.c`（强符号源，
 *        EQ-1）；firmware/main/CMakeLists.txt 设备构建登记 `diagnostics/log_semantics.c`。
 *        回归网成员（sd_log / key_layer_dispatch / time_sync 三冻结件）继续在册、
 *        不被本包登记改动静默摘除。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | DLG-A1 墙上时间前缀（同步 -> YYYY-MM-DD HH:MM:SS / 未同步 -> BOOT+ms / 预格式化缓存写两次取同串、更新取新串） | TEST 1（格式化决策表 + 字段越界防御 + 容量/NULL 守卫 + 缓存语义与 format 单源一致） | 行为级 |
 * | DLG-A2 语义名映射（action/key/gesture/page + trigger 全枚举 -> 名、未知 -> fallback 不空串不崩） | TEST 2（五族闭集逐值精确串 + 闭集边界外 fallback 逐族） | 行为级 |
 * | DLG-A3 噪声降频决策（60s 间隔单源、边界两侧、幂等纯函数、interval 非法防御） | TEST 3（59999/60000 边界 + 负 interval -1 + 回退 fail-closed + 宏与决策同源） | 行为级 |
 * | DLG-A4 接线结构（五缝源锚 + 头文件数值单源） | TEST 4（①-⑥ 逐锚，注释剥离后可执行源字面形态） | 源锚代理（机制级锚点；**不等于运行时观察**） |
 * | DLG-A5 回归网（既有冻结件全数仍编入 + 零新增红 vs 222T3F） | TEST 5（三冻结件登记在守 + 本件登记序 + log_semantics.c 双侧登记）+ 红基线实跑对照（运行面承载，非本文件断言） | 源锚代理（登记守）+ 实跑记录 |
 * | DLG-A6 真机轮观测（五类记录面逐行 / 串口不中断 / 无钩子递归 / 轮转与 dropped 计数） | **live-manual，无自动断言**（见 coverage 行） | 真机观察（human 持板） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（缓存并发语义）：TEST 1 只证缓存「写-读-更新」串行语义；update 在低频
 *         任务、fetch 在任意任务/ISR 上下文的**真并发**一致性（撕裂 / 半更新）归
 *         芯片面（builder 的 volatile/原子实现）+ 真机轮 DLG-A6 ③（无递归迹象）
 *         + 代码复核。host 不可达。
 *   GAP-2（真实墙上时间来源）：本缝字段全部显式注入；SNTP/RTC 已同步判定与
 *         gettimeofday/PCF85063 读取归芯片面接线，host 不可达 -> 真机轮 DLG-A6 ①
 *         （每行时间前缀含未同步降级形态）承载。synced 的真实判定来源本缝不裁。
 *   GAP-3（源锚 = 运行时行为）：TEST 4/5 只证明源码具备该接线形状与登记形态，
 *         **不证明**设备上每行真的带前缀、PAGE_SWITCH 真的打全 from→to+trigger、
 *         SystemInfo 真的 60s 一条——真机轮 DLG-A6 承载。
 *   GAP-4（日志量与 dropped）：内容增强抬日志量，128KB 环形 + 16MB 封顶下的
 *         dropped 计数归真机轮 DLG-A6 ④（plan R2）；本文件不断言容量行为
 *         （sd_log.acceptance.c [C1] 已冻结、不重复）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：本文件 standalone 红跑不链接
 *        diagnostics/log_semantics.c，哨兵恒红 = 设计态红；host runner 中
 *        log_semantics.c 必须在 HOST_ACCEPTANCE_SRCS 在世（TEST 5 登记守校验），
 *        否则哨兵红会掩盖「未接线」。
 *   EQ-2「镜像常量 == log_semantics.h 真值」：行为测试用本文件镜像的
 *        TIME_PREFIX_CAP=32 / SYSINFO_INTERVAL_MS=60000；TEST 4 扫头文件宏字面把
 *        等值钉死。
 *   EQ-3「枚举镜像 == 头文件真值」：TEST 2 的闭集端点（action 5 / key 1 / gesture 3
 *        / page 2 / trigger 2）按 key_gesture_parser.h 与 kidpal_render_node.h 现值
 *        镜像；若枚举集后续扩张，fallback 断言（边界外首值）会转红提醒重钉——这是
 *        有意的脆性（闭集漂移必须过 acceptance 修订窗口），不是误报。
 *   EQ-4「源锚 = 运行时行为」：TEST 4 的结构锚不证明设备运行时表现（同 GAP-3）。
 *   EQ-5「cache() == format() 同入参产物」：TEST 1 断言两者串行一致；并发面归 GAP-1。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder FIRST ACTION：向 firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 追加 "../../diag_log.acceptance.c" 与纯逻辑源
 *      "../../../diagnostics/log_semantics.c"（置于 host_infra_smoke 之前；登记行
 *      **不在 acceptance-author 写权内**——scope-map 拦截实证 2026-10-22，
 *      TaskPackage changeManifest 预指派 builder 代落，sd_log 261018 先例口径；
 *      TEST 5 登记守校验两行）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，sc_red_wrapper 同款通道）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -I. -o /tmp/dlg_red_runner \
 *          ../../diag_log.acceptance.c runner.c
 *      cd <repo root>; /tmp/dlg_red_runner      # exit code = 失败用例数
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告与下方实跑记录）:
 *   设计态（净工作树：无 diagnostics/log_semantics.{h,c}、五处接线锚未落、两处
 *   CMakeLists 未登记任何本包行——本验收件登记同样归 builder FIRST ACTION）——
 *   预期 5 个 TEST_CASE 全红，红因逐条对应当前 story 缺失的真实行为：
 *     TEST 1  -> DLG-A1 纯逻辑 seam 未实现（weak 哨兵 format 返回 -99 / cache NULL）
 *     TEST 2  -> DLG-A2 映射 seam 未实现（weak 哨兵返回 NULL）
 *     TEST 3  -> DLG-A3 降频 seam 未实现（weak 哨兵返回 -99）
 *     TEST 4  -> DLG-A4 接线锚缺席（①时间前缀拼接 / ②state_reject / ③④语义名
 *                挂接 / ⑤降频接线 / ⑥log_semantics.h 数值单源）
 *     TEST 5  -> DLG-A5 登记守（diag_log.acceptance.c 与 log_semantics.c 双侧登记
 *                全部缺席——含本验收件自身登记，builder FIRST ACTION）
 *
 *   冻结落盘实跑（2026-10-22，双通道）:
 *   〔A〕host 全量 runner（工作树、本件尚未登记进 runner——261018 先例同口径）:
 *        `222 Tests 3 Failures`（exit 3）——与已知基线（锚 5412af06）完全一致：
 *        3 条全为既有 p1_auxbar_style.acceptance.c 的 AB-A1 / AB-A2 / AB-A4 先存红，
 *        与本包正交；本包零新增红（本件尚未编入 runner，运行面零扰动）。
 *   〔B〕独立冒烟通道（red-baseline runner.c，COMPILE_EXIT=0 零 warning）:
 *        `5 Tests 5 Failures`（RUN_EXIT=5），红名 verbatim 见交付报告。
 *   双向可满足性（/tmp 合成树 KIDPAL_REPO_ROOT 注入 + 一次性参考实现，均不写入
 *   工作树）: `5 Tests 0 Failures`；九处定向变异各自只打红对应断言（每处重建
 *   净树 + 单变异 + 重编重跑）——结构五处：时间前缀拼接移到 ring_write 之后 /
 *   state_reject 移到 State: 行之后 / application.cc 回退裸 clock_ticks_ % 10 /
 *   INTERVAL_MS 60000->10000 / action 行摘语义名挂接；行为四处：日期字段摘零填充
 *   （TEST 1 红）/ interval<=0 不再拒绝（TEST 3 红）/ page fallback 改空串
 *   （TEST 2 红）/ 钩子整体摘除时间前缀（TEST 4 红）——断言非空过。逐项取证
 *   见交付报告。
 *   转绿判据：builder 落地 [DLG-C1]-[DLG-C5] 后全绿；任一仍红而对应源锚已落地 =
 *   锚漂移，复核本文件锚，**不得改判为绿**。既有 host 全量套件必须零新增红。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: DLG-A1 -- 墙上时间前缀纯逻辑：已同步 -> "[YYYY-MM-DD HH:MM:SS] "
 *             逐字段格式化 / 未同步 -> "[BOOT+<ms>ms] " 降级（含 boot_ms=0 边界）/
 *             同步态字段越界 -1 零写入 / NULL out 与 cap 不足 -1 / 预格式化缓存：
 *             未 update -> "" 恒非 NULL、写两次取同串、更新后取新串、缓存与
 *             format 同入参产物一致（TEST 1）
 *   coverage: DLG-A2 -- 语义名映射纯逻辑：action 0..5 / key 0..1 / gesture 0..3 /
 *             page 0..2 / trigger 0..2 全枚举逐值精确名 + 闭集边界外（6/2/4/3/3
 *             与负值）fallback "<族>_<n>" 形态、非空、不崩（TEST 2）
 *   coverage: DLG-A3 -- 降频决策纯逻辑：60s 边界两侧（59999/60000 与 119999/
 *             120000）/ interval <= 0 -> -1 / now < last -> 0（回退 fail-closed）/
 *             幂等（同入参两次调用同出）/ 冻结宏 KIDPAL_LOG_SYSINFO_INTERVAL_MS
 *             驱动决策为真（TEST 3）
 *   coverage: DLG-A4 -- 接线结构（源锚，注释剥离后可执行源）：① sd_log_fs.cc
 *             KidpalSdLogVprintf 体内 kidpal_log_time_cache( 落在 s_prev_vprintf(
 *             之后、kidpal_sd_log_ring_write( 之前（TEST 4）；② device_state_
 *             machine.cc TransitionTo 拒绝分支 state_reject %s -> %s 在 return
 *             false 之前、既有 State: %s -> %s 行保留且在其后（TEST 4）；③
 *             waveshare .cc key_route_dispatch 行 action=%d( + corr_id=%s 同行、
 *             key_gesture_parser 行 key=%d( / gesture=%d( + corr_id=%s 同行、
 *             kidpal_log_action_name( / kidpal_log_key_name( /
 *             kidpal_log_gesture_name( 消费锚（TEST 4）；④ kidpal_render_node.cc
 *             PAGE_SWITCH 行含 trigger=、kidpal_log_page_name( /
 *             kidpal_log_trigger_name( 消费锚（TEST 4）；⑤ application.cc
 *             kidpal_log_sysinfo_due( 在世 + 裸 clock_ticks_ % 10 退场 +
 *             kidpal_log_time_cache_update( 产出方在世（TEST 4）；⑥ log_semantics.h
 *             冻结数值单源两宏（TEST 4）
 *   coverage: DLG-A5 -- 回归网：sd_log / key_layer_dispatch / time_sync 三冻结件
 *             登记在守（anti-vacuity）+ 本件登记于 host smoke 之前（注册序守卫）
 *             + diagnostics/log_semantics.c host/设备双侧登记守 + 扫描器自检
 *             （TEST 5 + TEST 4 自检段）；运行面 = 红基线实跑对照 222T3F
 *             （先存红 = p1_auxbar AB-A1/A2/A4，锚 5412af06），零新增意外红
 *   coverage: DLG-A6 -- **live-manual 真机轮观测项，不落自动断言、不计入 runner
 *             通过面**（TaskPackage 标 kind=structural；SLP-A6 先例同款口径）。
 *             human 持板执行：① 导卡逐行核五类记录面（每行时间前缀〔含未同步
 *             BOOT+ms 降级形态〕/ PAGE_SWITCH from→to+trigger / 状态机跳转含
 *             state_reject / 按键语义名 / SystemInfo 60s 降频）；② 串口 COM5 不
 *             中断；③ 无钩子递归迹象（时间前缀拼接路径无递归回灌）；④ 轮转与
 *             dropped 计数正常（plan R2）。本 Pod 到真机门前停等
 *             （status=awaiting-human:real-device），不自行烧录。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 镜像常量（[DLG-C1]/[DLG-C3]；log_semantics.h 落盘后必须与本节逐值相同——TEST 4
 * 扫头文件字面把等值钉死，EQ-2）。
 * ------------------------------------------------------------------------ */

#define KIDPAL_LOG_TIME_PREFIX_CAP    32u
#define KIDPAL_LOG_SYSINFO_INTERVAL_MS 60000LL

/* 未实现哨兵：永不是任何整型 seam 的合法返回值（sd_log SDL_UNIMPLEMENTED 同款
 * 纪律）。指针 seam 的哨兵 = NULL。 */
#define DLG_UNIMPLEMENTED (-99)

/* -------------------------------------------------------------------------- *
 * 纯逻辑 seam 声明（与 log_semantics.h 逐字同型）。
 * ------------------------------------------------------------------------ */

int kidpal_log_time_format(int synced, int year, int mon, int mday,
                           int hour, int min, int sec,
                           unsigned long long boot_ms, char *out, unsigned cap);
void kidpal_log_time_cache_update(int synced, int year, int mon, int mday,
                                  int hour, int min, int sec,
                                  unsigned long long boot_ms);
const char *kidpal_log_time_cache(void);
const char *kidpal_log_action_name(int act);
const char *kidpal_log_key_name(int key);
const char *kidpal_log_gesture_name(int g);
const char *kidpal_log_page_name(int page);
const char *kidpal_log_trigger_name(int trg);
int kidpal_log_sysinfo_due(long long now, long long last, long long interval);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（home = firmware/main/diagnostics/log_semantics.c；host runner 中强
 * 定义覆盖）。未落地 / 未接线态 = 设计态红，不是编译错（EQ-1）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) int kidpal_log_time_format(int synced, int year, int mon,
                                                 int mday, int hour, int min,
                                                 int sec,
                                                 unsigned long long boot_ms,
                                                 char *out, unsigned cap)
{
    (void)synced; (void)year; (void)mon; (void)mday; (void)hour; (void)min;
    (void)sec; (void)boot_ms; (void)out; (void)cap;
    return DLG_UNIMPLEMENTED;
}
__attribute__((weak)) void kidpal_log_time_cache_update(int synced, int year,
                                                        int mon, int mday,
                                                        int hour, int min,
                                                        int sec,
                                                        unsigned long long boot_ms)
{
    (void)synced; (void)year; (void)mon; (void)mday; (void)hour; (void)min;
    (void)sec; (void)boot_ms;
}
__attribute__((weak)) const char *kidpal_log_time_cache(void)
{ return NULL; }
__attribute__((weak)) const char *kidpal_log_action_name(int act)
{ (void)act; return NULL; }
__attribute__((weak)) const char *kidpal_log_key_name(int key)
{ (void)key; return NULL; }
__attribute__((weak)) const char *kidpal_log_gesture_name(int g)
{ (void)g; return NULL; }
__attribute__((weak)) const char *kidpal_log_page_name(int page)
{ (void)page; return NULL; }
__attribute__((weak)) const char *kidpal_log_trigger_name(int trg)
{ (void)trg; return NULL; }
__attribute__((weak)) int kidpal_log_sysinfo_due(long long now, long long last,
                                                 long long interval)
{ (void)now; (void)last; (void)interval; return DLG_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * sd_log.acceptance.c 的 sdl_* 助手族，命名前缀 dlg_；全 static，不与先例符号
 * 冲突）。
 * ------------------------------------------------------------------------ */

#define DLG_MAX_PATH 1024
#define DLG_MAX_READ (8u * 1024u * 1024u)

static char dlg_fm_buf[1024];
static const char *dlg_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *dlg_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(dlg_fm_buf, sizeof(dlg_fm_buf), fmt, ap);
    va_end(ap);
    return dlg_fm_buf;
}

static int dlg_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char dlg_root[DLG_MAX_PATH];
static int dlg_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int dlg_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[DLG_MAX_PATH];
    const char *env_root;
    size_t i;

    if (dlg_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (dlg_is_regular(probe)) {
            snprintf(dlg_root, sizeof(dlg_root), "%s", env_root);
            dlg_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (dlg_is_regular(probe)) {
            snprintf(dlg_root, sizeof(dlg_root), "%s", cands[i]);
            dlg_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int dlg_rel(char *out, size_t cap, const char *rel)
{
    if (out == NULL || rel == NULL) {
        return 0;
    }
    if (!dlg_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", dlg_root, rel);
    return 1;
}

static char *dlg_read(const char *rel, size_t *out_len)
{
    char p[DLG_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!dlg_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > DLG_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（260917-m2-k3 sealing pass / sd_log 同款）：正向断言只被**可执行
 * 源**满足——注释字节抹成空格；字符串 / 字符字面量体保留（不把字面量里的 // 误判为
 * 注释）。保持缓冲长度，调用方负责 free。 */
static char *dlg_strip_c_comments(const char *src)
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

static char *dlg_strip_file(const char *rel)
{
    char *buf = dlg_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = dlg_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（函数窗内 needle 在场判定）。未命中 -> NULL。 */
static const char *dlg_find_in(const char *hay, const char *end, const char *needle)
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
static int dlg_count_sub(const char *hay, const char *needle)
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
static int dlg_count_lines_with_both(const char *slice, const char *needle1,
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
        char tmp[DLG_MAX_PATH];
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

/* 行提取：pos 所在行拷入 out（cap 截断）。返回 out（pos 越界返回 NULL）。 */
static char *dlg_line_at(const char *src, const char *pos, char *out, size_t cap)
{
    const char *bol;
    const char *eol;
    size_t len, cp;

    if (src == NULL || pos == NULL || pos < src || out == NULL || cap == 0) {
        return NULL;
    }
    bol = pos;
    while (bol > src && bol[-1] != '\n') {
        bol--;
    }
    eol = strchr(pos, '\n');
    len = eol ? (size_t)(eol - bol) : strlen(bol);
    cp = len < cap - 1 ? len : cap - 1;
    memcpy(out, bol, cp);
    out[cp] = '\0';
    return out;
}

/* 有界整数提取：[p, end) 内、首个 ')' 或 ';' 前的十进制字面量序。返回写入个数。 */
static int dlg_ints_between(const char *p, const char *end, long *out, int max_args)
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

/* 行内首个整数提取（"#define X N" 形态）。失败返回 0。 */
static int dlg_first_int_after_on_line(const char *hay, const char *needle, long *out)
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
    n = dlg_ints_between(p, eol, a, 4);
    if (n < 1) {
        return 0;
    }
    *out = a[0];
    return 1;
}

/* 函数窗终点：起点 + win 字节，钳制到缓冲末尾。 */
static const char *dlg_win_end(const char *start, size_t win)
{
    const char *end = start + win;
    size_t total = strlen(start);

    if ((size_t)(end - start) > total) {
        end = start + total;
    }
    return end;
}

#define DLG_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(dlg_locate_root(), \
    "DLG-Ax RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

#define DLG_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != DLG_UNIMPLEMENTED, \
        dlg_fm("%s RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；" \
               "builder 落地 diagnostics/log_semantics.c 并登记进 HOST_ACCEPTANCE_SRCS " \
               "后转绿", "DLG", what))

/* ========================================================================== *
 * TEST 1 — DLG-A1 墙上时间前缀纯逻辑（格式化决策 + 字段越界防御 + 容量守卫 +
 * 预格式化缓存语义）。
 * coverage: DLG-A1 -- 已同步 -> "[YYYY-MM-DD HH:MM:SS] " / 未同步 ->
 *           "[BOOT+<ms>ms] "（含 boot_ms=0）/ 同步态字段越界 -1 零写入 /
 *           NULL out / cap 不足 -1 / 缓存未 update -> "" 恒非 NULL / 写两次取
 *           同串 / 更新后取新串 / 缓存与 format 同入参产物一致（EQ-5）
 * RED today（设计态）: diagnostics/log_semantics.c 未落地 -> weak 哨兵
 *           format 返回 -99 / cache 返回 NULL。
 * ========================================================================== */
TEST_CASE("diag log: wall-clock time prefix format decision + preformatted cache (DLG-A1)",
          "[diag_log][DLG-A1][linux][behavior]")
{
    char buf[KIDPAL_LOG_TIME_PREFIX_CAP + 8];
    const char *c1;
    const char *c2;

    /* ---- 已同步：YYYY-MM-DD HH:MM:SS 逐字段格式化 --------------------------- */
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT_MESSAGE(22,
        kidpal_log_time_format(1, 2026, 10, 22, 12, 34, 56, 0ULL, buf,
                               (unsigned)sizeof(buf)),
        "DLG-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_log_time_format 返回 -99）或"
        "已同步前缀长度非 22——builder 落地 diagnostics/log_semantics.c 并登记进 "
        "HOST_ACCEPTANCE_SRCS 后转绿；契约见本文件头注 [DLG-C1]");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[2026-10-22 12:34:56] ", buf,
        "DLG-A1 RED: 已同步前缀串错——必须为 \"[YYYY-MM-DD HH:MM:SS] \"（零填充两位"
        "字段 + 括号 + 尾随空格，钩子直接 memcpy 拼接）。");

    /* 跨月/跨日补零与年内首日（字段宽度不变量）。 */
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT_MESSAGE(22,
        kidpal_log_time_format(1, 2026, 1, 9, 3, 7, 5, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: 单位数月份/日期格式化长度非 22。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[2026-01-09 03:07:05] ", buf,
        "DLG-A1 RED: 单位数字段未零填充（%02d 义务）——日志时间列宽不齐，导出后无法"
        "按列 grep。");

    /* ---- 未同步降级：BOOT+<ms>ms（含 0 边界） ------------------------------- */
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT_MESSAGE(15,
        kidpal_log_time_format(0, 0, 0, 0, 0, 0, 0, 12345ULL, buf,
                               (unsigned)sizeof(buf)),
        "DLG-A1 RED: 未同步降级路径长度非 15。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[BOOT+12345ms] ", buf,
        "DLG-A1 RED: 未同步前缀串错——必须为 \"[BOOT+<ms>ms] \"（human 拍板：连时间"
        "都没记的原始痛点降级标注面）。");
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT_MESSAGE(11,
        kidpal_log_time_format(0, 0, 0, 0, 0, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: boot_ms=0 边界长度非 11（早于首个 tick 的日志也要带降级标注）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[BOOT+0ms] ", buf,
        "DLG-A1 RED: boot_ms=0 前缀串错。");

    /* ---- 同步态字段越界：-1 且零写入（不产出假时间） ------------------------ */
    memset(buf, 'X', sizeof(buf));
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 13, 1, 0, 0, 0, 0ULL, buf,
                               (unsigned)sizeof(buf)),
        "DLG-A1 RED: mon=13 未拒绝——同步态字段越界必须 -1（防御：不产出假时间）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 0, 1, 0, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: mon=0 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 32, 0, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: mday=32 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 0, 0, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: mday=0 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 22, 24, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: hour=24 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 22, 0, 60, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: min=60 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 22, 0, 0, 60, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: sec=60 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 1999, 10, 22, 0, 0, 0, 0ULL, buf, (unsigned)sizeof(buf)),
        "DLG-A1 RED: year=1999 未拒绝——2000 前年份是坏 RTC/坏 SNTP 信号，按假时间"
        "落卡会污染全部下游对账。");
    {
        int i;
        int all_x = 1;
        for (i = 0; i < (int)sizeof(buf); i++) {
            if (buf[i] != 'X') { all_x = 0; break; }
        }
        TEST_ASSERT_TRUE_MESSAGE(all_x,
            "DLG-A1 RED: 拒绝路径写了 out——越界拒绝必须零写入（调用方缓冲不得被"
            "半串污染）。");
    }

    /* ---- 非法 out / cap 不足：-1 且零写入 ----------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 22, 12, 34, 56, 0ULL, NULL, 64u),
        "DLG-A1 RED: out=NULL 未拒绝。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_log_time_format(1, 2026, 10, 22, 12, 34, 56, 0ULL, buf, 22u),
        "DLG-A1 RED: cap=22 未拒绝（22 字符 + NUL = 23 最小容量）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(22,
        kidpal_log_time_format(1, 2026, 10, 22, 12, 34, 56, 0ULL, buf, 23u),
        "DLG-A1 RED: cap=23（恰好最小容量）被误拒。");

    /* ---- 预格式化缓存：未 update -> "" 恒非 NULL ---------------------------- */
    c1 = kidpal_log_time_cache();
    TEST_ASSERT_NOT_NULL_MESSAGE(c1,
        "DLG-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_log_time_cache 返回 NULL）——"
        "builder 落地 diagnostics/log_semantics.c 后转绿");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, (int)strlen(c1),
        "DLG-A1 RED: 未 update 过的缓存非空串——契约要求初始 \"\"（降级为无前缀，"
        "绝不 NULL 崩钩子）。");

    /* ---- 缓存：写两次取同串 / 更新后取新串 / 与 format 单源一致（EQ-5） ----- */
    kidpal_log_time_cache_update(1, 2026, 10, 22, 8, 0, 0, 0ULL);
    c1 = kidpal_log_time_cache();
    TEST_ASSERT_NOT_NULL_MESSAGE(c1, "DLG-A1 RED: update 后缓存为 NULL。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[2026-10-22 08:00:00] ", c1,
        "DLG-A1 RED: 缓存内容与 format 决策不一致——预格式化必须复用同一决策单源"
        "（EQ-5，否则钩子拼接的前缀与验收过的格式漂移）。");
    kidpal_log_time_cache_update(1, 2026, 10, 22, 8, 0, 0, 0ULL);
    c2 = kidpal_log_time_cache();
    TEST_ASSERT_TRUE_MESSAGE(c1 == c2 || strcmp(c1, c2) == 0,
        "DLG-A1 RED: 同入参写两次缓存，取到的串不同——缓存产出不稳定（幂等义务）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[2026-10-22 08:00:00] ", c2,
        "DLG-A1 RED: 第二次同入参 update 后缓存内容漂移。");

    kidpal_log_time_cache_update(0, 0, 0, 0, 0, 0, 0, 777ULL);
    c2 = kidpal_log_time_cache();
    TEST_ASSERT_NOT_NULL_MESSAGE(c2, "DLG-A1 RED: 未同步 update 后缓存为 NULL。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[BOOT+777ms] ", c2,
        "DLG-A1 RED: 更新后缓存未取到新串——低频任务产出（时间同步/未同步切换）"
        "必须如实反映到钩子拼接面。");

    kidpal_log_time_cache_update(1, 2026, 10, 22, 8, 0, 1, 0ULL);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("[2026-10-22 08:00:01] ",
        kidpal_log_time_cache(),
        "DLG-A1 RED: 未同步 -> 同步切换后缓存未更新（SNTP 校时生效瞬间钩子仍拼"
        "BOOT+ 前缀）。");
}

/* ========================================================================== *
 * TEST 2 — DLG-A2 语义名映射纯逻辑（五族闭集逐值精确名 + 越界 fallback）。
 * coverage: DLG-A2 -- action 0..5 / key 0..1 / gesture 0..3 / page 0..2 /
 *           trigger 0..2 全枚举精确名；闭集边界外与负值 -> "<族>_<n>" fallback、
 *           非空、不崩
 * RED today（设计态）: 映射 seam 未实现（weak 哨兵返回 NULL）。
 * ========================================================================== */
TEST_CASE("diag log: semantic name mapping closed sets + fallback for unknown (DLG-A2)",
          "[diag_log][DLG-A2][linux][behavior]")
{
    /* ---- global_action_t（key_gesture_parser.h，EQ-3 镜像）0..5 ------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("none",
        kidpal_log_action_name(0), "DLG-A2 RED: action 0 (GLOBAL_ACT_NONE) 名错或"
        "冻结 seam 未实现（weak 哨兵返回 NULL）——builder 落地 diagnostics/log_semantics.c "
        "后转绿");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("session_toggle", kidpal_log_action_name(1),
        "DLG-A2 RED: action 1 (GLOBAL_ACT_SESSION_TOGGLE) 名错——plan 形态 "
        "action=<n>(<语义名>) 的语义名必须可 grep。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("home", kidpal_log_action_name(2),
        "DLG-A2 RED: action 2 (GLOBAL_ACT_HOME) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("page_next", kidpal_log_action_name(3),
        "DLG-A2 RED: action 3 (GLOBAL_ACT_PAGE_NEXT) 名错——真机日志样本里"
        "「action=3 密码」正是本条缺语义名的直接痛点。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("overlay_close", kidpal_log_action_name(4),
        "DLG-A2 RED: action 4 (GLOBAL_ACT_OVERLAY_CLOSE) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("char_replay", kidpal_log_action_name(5),
        "DLG-A2 RED: action 5 (GLOBAL_ACT_CHAR_REPLAY) 名错——闭集端点漂移即红"
        "（EQ-3）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("act_6", kidpal_log_action_name(6),
        "DLG-A2 RED: action 6（闭集外首个）fallback 串错——必须 \"act_<n>\" 形态，"
        "不空串不崩（闭集扩张时本断言转红提醒重钉，EQ-3）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("act_-1", kidpal_log_action_name(-1),
        "DLG-A2 RED: action 负值 fallback 错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("act_99", kidpal_log_action_name(99),
        "DLG-A2 RED: action 99 fallback 错。");

    /* ---- key_id_t 0..1 ------------------------------------------------------ */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("key", kidpal_log_key_name(0),
        "DLG-A2 RED: key 0 (KEY_ID_KEY) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("boot", kidpal_log_key_name(1),
        "DLG-A2 RED: key 1 (KEY_ID_BOOT) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("key_2", kidpal_log_key_name(2),
        "DLG-A2 RED: key 2（闭集外，KEY_ID_COUNT 位置）fallback 错——新增键位必须"
        "过本验收修订窗口，防映射表漏项静默。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("key_-3", kidpal_log_key_name(-3),
        "DLG-A2 RED: key 负值 fallback 错。");

    /* ---- key_gesture_t 0..3 ------------------------------------------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("none", kidpal_log_gesture_name(0),
        "DLG-A2 RED: gesture 0 (KEY_GESTURE_NONE) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("click", kidpal_log_gesture_name(1),
        "DLG-A2 RED: gesture 1 (KEY_GESTURE_CLICK) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("double_click", kidpal_log_gesture_name(2),
        "DLG-A2 RED: gesture 2 (KEY_GESTURE_DOUBLE_CLICK) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("long_press", kidpal_log_gesture_name(3),
        "DLG-A2 RED: gesture 3 (KEY_GESTURE_LONG_PRESS) 名错——闭集端点。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ges_4", kidpal_log_gesture_name(4),
        "DLG-A2 RED: gesture 4 fallback 错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ges_-1", kidpal_log_gesture_name(-1),
        "DLG-A2 RED: gesture 负值 fallback 错。");

    /* ---- kidpal_page_id_t（kidpal_render_node.h）0..2 ----------------------- */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("home", kidpal_log_page_name(0),
        "DLG-A2 RED: page 0 (KP_PAGE_HOME) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("schedule", kidpal_log_page_name(1),
        "DLG-A2 RED: page 1 (KP_PAGE_SCHEDULE) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("story", kidpal_log_page_name(2),
        "DLG-A2 RED: page 2 (KP_PAGE_STORY) 名错——闭集端点（P4 已撤，3 页闭集）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("page_3", kidpal_log_page_name(3),
        "DLG-A2 RED: page 3 fallback 错——撤 P4 后 page 3 是非法页，绝不能映射出"
        "伪语义名。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("page_-1", kidpal_log_page_name(-1),
        "DLG-A2 RED: page 负值 fallback 错（kidpal_render_node_story_substate 用 "
        "-1 表「非 P3」，page 面同样可能收到负值）。");

    /* ---- PAGE_SWITCH 触发源 0..2（plan 改动点 5：key/timer/net） ------------ */
    TEST_ASSERT_EQUAL_STRING_MESSAGE("key", kidpal_log_trigger_name(0),
        "DLG-A2 RED: trigger 0 (key) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("timer", kidpal_log_trigger_name(1),
        "DLG-A2 RED: trigger 1 (timer) 名错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("net", kidpal_log_trigger_name(2),
        "DLG-A2 RED: trigger 2 (net) 名错——闭集端点。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("trg_3", kidpal_log_trigger_name(3),
        "DLG-A2 RED: trigger 3 fallback 错。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("trg_-1", kidpal_log_trigger_name(-1),
        "DLG-A2 RED: trigger 负值 fallback 错。");
}

/* ========================================================================== *
 * TEST 3 — DLG-A3 SystemInfo 降频决策纯逻辑（60s 边界两侧 + 防御 + 幂等）。
 * coverage: DLG-A3 -- 59999/60000 与 119999/120000 边界两侧 / interval <= 0 -> -1 /
 *           now < last -> 0（回退 fail-closed）/ 幂等（同入参两次调用同出）/
 *           冻结宏 KIDPAL_LOG_SYSINFO_INTERVAL_MS 驱动决策为真
 * RED today（设计态）: 降频 seam 未实现（weak 哨兵返回 -99）。
 * ========================================================================== */
TEST_CASE("diag log: SystemInfo print throttling decision, 60s boundary, idempotent (DLG-A3)",
          "[diag_log][DLG-A3][linux][behavior]")
{
    int r1;
    int r2;

    /* ---- 60s 边界两侧（首次打印窗口：last=0） ------------------------------- */
    r1 = kidpal_log_sysinfo_due(59999, 0, 60000);
    TEST_ASSERT_TRUE_MESSAGE(r1 != DLG_UNIMPLEMENTED,
        "DLG-A3 RED: 冻结 seam 未实现（weak 哨兵 kidpal_log_sysinfo_due 返回 -99）——"
        "builder 落地 diagnostics/log_semantics.c 后转绿；契约见本文件头注 [DLG-C3]");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r1,
        "DLG-A3 RED: 59999ms 就要求打印——降频窗口过短（SystemInfo 噪声占比 70% 的"
        "原始痛点治不掉）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_log_sysinfo_due(60000, 0, 60000),
        "DLG-A3 RED: 60000ms（恰好到界）未要求打印——降频间隔边界语义错。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_log_sysinfo_due(60001, 0, 60000),
        "DLG-A3 RED: 60001ms 未要求打印。");

    /* ---- 后续窗口（last 推进） ---------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_log_sysinfo_due(119999, 60000, 60000),
        "DLG-A3 RED: 距上次打印 59999ms 就再次打印——间隔决策不是「距上次」口径。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_log_sysinfo_due(120000, 60000, 60000),
        "DLG-A3 RED: 距上次打印 60000ms 未再次打印。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_log_sysinfo_due(180000, 60000, 60000),
        "DLG-A3 RED: 长时间未打印后未放行——降频不得退化成「只打一次」。");

    /* ---- 防御与回退 --------------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_log_sysinfo_due(100, 0, 0),
        "DLG-A3 RED: interval=0 未返回 -1——零间隔等于每 tick 一条，等于没降频。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_log_sysinfo_due(100, 0, -60000),
        "DLG-A3 RED: 负 interval 未返回 -1（防御）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_log_sysinfo_due(50, 100, 60000),
        "DLG-A3 RED: now < last（时钟回退/tick 重置）却要求打印——fail-closed：噪声"
        "治理不得因回退反弹成风暴。");

    /* ---- 幂等：同入参恒同出（纯函数，无隐藏状态） --------------------------- */
    r1 = kidpal_log_sysinfo_due(60000, 0, 60000);
    r2 = kidpal_log_sysinfo_due(60000, 0, 60000);
    TEST_ASSERT_EQUAL_INT_MESSAGE(r1, r2,
        "DLG-A3 RED: 同入参两次调用结果不同——决策函数带隐藏状态（调用次数改变"
        "决策），违反纯函数义务，间隔决策将不可单源推理。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r1, "DLG-A3 RED: 幂等段首调结果错。");

    /* ---- 冻结宏驱动决策为真（10s -> 60s 单源，宏与决策同源） ---------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_log_sysinfo_due(KIDPAL_LOG_SYSINFO_INTERVAL_MS, 0,
                               KIDPAL_LOG_SYSINFO_INTERVAL_MS),
        "DLG-A3 RED: 冻结宏 KIDPAL_LOG_SYSINFO_INTERVAL_MS 驱动决策为假——镜像常量"
        "与决策不同源（EQ-2）。");
}

/* -------------------------------------------------------------------------- *
 * 结构断言扫描面（源锚；相对仓根）
 * ------------------------------------------------------------------------ */

#define DLG_SDLOG_FS   "firmware/main/diagnostics/sd_log_fs.cc"
#define DLG_STATE_CC   "firmware/main/device_state_machine.cc"
#define DLG_BOARD_CC   "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define DLG_RENDER_CC  "firmware/main/display/kidpal_render_node.cc"
#define DLG_APP_CC     "firmware/main/application.cc"
#define DLG_SEM_H      "firmware/main/diagnostics/log_semantics.h"
#define DLG_MAIN_CMAKE "firmware/main/CMakeLists.txt"
#define DLG_HOST_CMAKE "firmware/main/test/host_app/main/CMakeLists.txt"

#define DLG_FN_WINDOW   3000
#define DLG_BODY_WINDOW 4000

/* ========================================================================== *
 * TEST 4 — DLG-A4 接线结构（五缝源锚 + 头文件数值单源 + 扫描器自检）。
 * coverage: DLG-A4 -- ① sd_log_fs.cc 钩子内 kidpal_log_time_cache( 落在链式转调
 *           之后、ring_write 之前；② device_state_machine.cc 拒绝分支
 *           state_reject 行 + 既有 State: 行保留且有序；③ waveshare .cc 两日志行
 *           语义名挂接 + corr_id 保留 + 映射消费锚；④ kidpal_render_node.cc
 *           PAGE_SWITCH + 页名/触发名消费锚；⑤ application.cc 降频决策接线 +
 *           裸 % 10 退场 + 缓存产出方在世；⑥ log_semantics.h 冻结数值单源
 * RED today（设计态）: 五处接线锚未落 + log_semantics.h 未落地。
 * ========================================================================== */
TEST_CASE("diag log: wiring source anchors — time prefix, state_reject, semantic names, throttle (DLG-A4)",
          "[diag_log][DLG-A4][structural]")
{
    char *code;
    const char *fn;
    const char *fn_end;
    const char *p_prev;
    const char *p_cache;
    const char *p_write;
    const char *p_rej;
    const char *p_state;
    char line[DLG_MAX_PATH];
    long v = -1;
    int n;

    DLG_REQUIRE_ROOT();

    /* ---- 扫描器自检（sd_log TEST 6 同款：自检不成立 = 全部结构断言失去咬合力） */
    code = dlg_strip_file(DLG_SDLOG_FS);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): sd_log_fs.cc unreadable");
    n = dlg_count_lines_with_both(code, "fflush(s_file)", "fsync(fileno(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "DLG-A4 RED(infra): 行级双针计数器失灵（sd_log_fs.cc 已知在世形态 "
        "fflush(s_file) × fsync(fileno( 只命中 0 行）——扫描器坏，本文件结构断言失去"
        "咬合力");
    {
        /* 注释剥离生效：已知仅存在于注释的中文文本不得残留在可执行源里。 */
        char *raw = dlg_read(DLG_SDLOG_FS, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(raw, "DLG-A4 RED(infra): sd_log_fs.cc unreadable");
        TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "链式转调原 vprintf") != NULL,
            "DLG-A4 RED(infra): sd_log_fs.cc 已知注释锚缺席——注释剥离自检无法成立"
            "（文件被重写？）。");
        TEST_ASSERT_TRUE_MESSAGE(strstr(code, "链式转调原 vprintf") == NULL,
            "DLG-A4 RED(infra): 注释剥离失效（已知注释文本仍留在可执行源里）——本文件"
            "全部结构断言会因注释文本假绿，须复核扫描器");
        free(raw);
    }
    free(code);

    /* ---- ① sd_log_fs.cc：钩子内时间前缀拼接（只补新锚，[C4] 旧锚不重复） ---- */
    code = dlg_strip_file(DLG_SDLOG_FS);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): sd_log_fs.cc unreadable");
    fn = strstr(code, "int KidpalSdLogVprintf(");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "DLG-A4 RED(infra): sd_log_fs.cc 无 int KidpalSdLogVprintf( 定义锚——钩子本体"
        "被改名，须复核锚不得改判为绿（sd_log.acceptance.c TEST 6 同锚，先复核该件）");
    fn_end = dlg_win_end(fn, DLG_BODY_WINDOW);
    p_prev = dlg_find_in(fn, fn_end, "s_prev_vprintf(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_prev,
        "DLG-A4 RED(infra): 钩子体内未找到 s_prev_vprintf(——链式转调锚缺席"
        "（sd_log.acceptance.c [C4] 冻结锚被破坏？先复核该件，不得在本文件改判）");
    p_write = dlg_find_in(fn, fn_end, "kidpal_sd_log_ring_write(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_write,
        "DLG-A4 RED(infra): 钩子体内未找到 kidpal_sd_log_ring_write(——环写锚缺席"
        "（sd_log.acceptance.c [C4] 冻结锚被破坏？先复核该件，不得在本文件改判）");
    p_cache = dlg_find_in(fn, fn_end, "kidpal_log_time_cache(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_cache,
        "DLG-A4 RED: KidpalSdLogVprintf 体内未消费 kidpal_log_time_cache(——墙上时间"
        "前缀未拼接进落卡日志（human 立项痛点②「连时间都没记」未治）；钩子内自行"
        " localtime_r/snprintf 格式化同样红——R1 纪律：低频任务预格式化 + 钩子只拼接");
    TEST_ASSERT_TRUE_MESSAGE(p_prev < p_cache,
        "DLG-A4 RED: 时间前缀拼接先于链式转调——前缀产出路径若在转调之前失败会拖累"
        "串口输出（[C4] 先转调纪律的时间前缀延长面，顺序不得倒置）");
    TEST_ASSERT_TRUE_MESSAGE(p_cache < p_write,
        "DLG-A4 RED: 时间前缀未在 kidpal_sd_log_ring_write 之前拼接——环写记录不带"
        "前缀（TF 卡导出日志仍无墙上时间，等于没治）。");
    free(code);

    /* ---- ② device_state_machine.cc：被拒转移补打 + 既有 State: 行保留 ------- */
    code = dlg_strip_file(DLG_STATE_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): device_state_machine.cc unreadable");
    fn = strstr(code, "DeviceStateMachine::TransitionTo");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "DLG-A4 RED(infra): device_state_machine.cc 无 DeviceStateMachine::TransitionTo "
        "定义锚——被改名，须复核锚不得改判为绿");
    fn_end = dlg_win_end(fn, DLG_FN_WINDOW);
    p_rej = dlg_find_in(fn, fn_end, "state_reject %s -> %s");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_rej,
        "DLG-A4 RED: TransitionTo 拒绝分支未补打 state_reject 行——被拒转移静默"
        "（human 立项痛点①「每个状态的跳转都记下来」的被拒半面未治；字面冻结 "
        "\"state_reject %s -> %s\"）");
    p_state = dlg_find_in(fn, fn_end, "State: %s -> %s");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_state,
        "DLG-A4 RED(infra): TransitionTo 内既有 State: %s -> %s 行缺席——:125 既有"
        "成功跳转日志被删（enrichment 不得删既有行，须复核不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(p_rej < p_state,
        "DLG-A4 RED: state_reject 出现在既有 State: 行之后——补打行必须落在拒绝"
        "分支（IsValidTransition 失败路径）内，不得挂在成功路径");
    TEST_ASSERT_NOT_NULL_MESSAGE(dlg_find_in(p_rej, p_state, "return false"),
        "DLG-A4 RED: state_reject 行与 State: 行之间无 return false——state_reject "
        "未落在拒绝分支（return false 路径）内，被拒转移的补打挂错了位置");
    free(code);

    /* ---- ③ waveshare .cc：按键语义名挂接 + corr_id 纪律保留 ----------------- */
    code = dlg_strip_file(DLG_BOARD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): 板 .cc unreadable");

    fn = strstr(code, "static void keya_dispatch_global_action");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "DLG-A4 RED(infra): 板 .cc 无 keya_dispatch_global_action 定义锚——被改名，"
        "须复核锚不得改判为绿");
    fn_end = dlg_win_end(fn, DLG_FN_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(dlg_find_in(fn, fn_end, "kidpal_log_action_name(") != NULL,
        "DLG-A4 RED: keya_dispatch_global_action 未消费 kidpal_log_action_name(——"
        "action 数字后未挂语义名（真机日志「action=3 密码」痛点未治）");
    {
        const char *p_act = dlg_find_in(fn, fn_end, "action=%d(");
        TEST_ASSERT_NOT_NULL_MESSAGE(p_act,
            "DLG-A4 RED: key_route_dispatch 行格式缺 action=%d(——语义名必须以括号"
            "形态紧跟 action 数字（plan 改动点 4：action=<n>(<语义名>)）");
        TEST_ASSERT_TRUE_MESSAGE(dlg_line_at(code, p_act, line, sizeof(line)) != NULL,
            "DLG-A4 RED(infra): action=%d( 所在行提取失败——扫描器坏");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "key_route_dispatch") != NULL,
            "DLG-A4 RED: action=%d( 不在 key_route_dispatch 日志行上——挂接点错位");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "corr_id=%s") != NULL,
            "DLG-A4 RED: key_route_dispatch 行 corr_id=%s 缺席——corr_id 纪律必须"
            "保留（plan 改动点 4：corr_id 纪律不动）");
    }

    fn = strstr(code, "static void keya_log_gesture");
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "DLG-A4 RED(infra): 板 .cc 无 keya_log_gesture 定义锚——被改名，须复核锚"
        "不得改判为绿");
    fn_end = dlg_win_end(fn, DLG_FN_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(dlg_find_in(fn, fn_end, "kidpal_log_key_name(") != NULL &&
                             dlg_find_in(fn, fn_end, "kidpal_log_gesture_name(") != NULL,
        "DLG-A4 RED: keya_log_gesture 未消费 kidpal_log_key_name( / "
        "kidpal_log_gesture_name(——key/gesture 数字后未挂语义名（DLG-A2 映射若无"
        "接线就是死代码，映射行为绿 != 日志真带名）");
    {
        const char *p_ges = dlg_find_in(fn, fn_end, "gesture=%d(");
        const char *p_key;
        TEST_ASSERT_NOT_NULL_MESSAGE(p_ges,
            "DLG-A4 RED: key_gesture_parser 行格式缺 gesture=%d(——语义名括号后挂"
            "形态缺席");
        TEST_ASSERT_TRUE_MESSAGE(dlg_line_at(code, p_ges, line, sizeof(line)) != NULL,
            "DLG-A4 RED(infra): gesture=%d( 所在行提取失败——扫描器坏");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "key_gesture_parser") != NULL,
            "DLG-A4 RED: gesture=%d( 不在 key_gesture_parser 日志行上——挂接点错位");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "corr_id=%s") != NULL,
            "DLG-A4 RED: key_gesture_parser 行 corr_id=%s 缺席——corr_id 纪律必须保留");
        p_key = dlg_find_in(fn, fn_end, "key=%d(");
        TEST_ASSERT_NOT_NULL_MESSAGE(p_key,
            "DLG-A4 RED: key_gesture_parser 行格式缺 key=%d(——键位语义名括号后挂"
            "形态缺席（键位数字同样密码化，human 样本痛点同源）");
    }
    free(code);

    /* ---- ④ kidpal_render_node.cc：PAGE_SWITCH 行 + 页名/触发名消费锚 -------- */
    code = dlg_strip_file(DLG_RENDER_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): kidpal_render_node.cc unreadable");
    {
        const char *p_switch = strstr(code, "PAGE_SWITCH");
        TEST_ASSERT_NOT_NULL_MESSAGE(p_switch,
            "DLG-A4 RED: kidpal_render_node.cc 无 PAGE_SWITCH 行——页面跳转无一行"
            "语义记录（human 立项痛点①「每个页面……的跳转都记下来」未治）");
        TEST_ASSERT_TRUE_MESSAGE(dlg_line_at(code, p_switch, line, sizeof(line)) != NULL,
            "DLG-A4 RED(infra): PAGE_SWITCH 所在行提取失败——扫描器坏");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "from=") != NULL && strstr(line, "to=") != NULL,
            "DLG-A4 RED: PAGE_SWITCH 行缺 from=/to=——from→to 形态缺席（只有单页号"
            "回答不了「从哪来」）");
        TEST_ASSERT_TRUE_MESSAGE(strstr(line, "trigger=") != NULL,
            "DLG-A4 RED: PAGE_SWITCH 行缺 trigger=——触发源（key/timer/net）缺席，"
            "「被什么触发」不可归因（plan 改动点 5）");
    }
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_log_page_name(") != NULL &&
                             strstr(code, "kidpal_log_trigger_name(") != NULL,
        "DLG-A4 RED: kidpal_render_node.cc 未消费 kidpal_log_page_name( / "
        "kidpal_log_trigger_name(——页号/触发号语义名未挂接（DLG-A2 映射死代码）");
    free(code);

    /* ---- ⑤ application.cc：降频决策接线 + 裸 % 10 退场 + 缓存产出方 -------- */
    code = dlg_strip_file(DLG_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "DLG-A4 RED(infra): application.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_log_sysinfo_due(") != NULL,
        "DLG-A4 RED: application.cc 未消费 kidpal_log_sysinfo_due(——SystemInfo 打印"
        "未走降频决策单源（plan 改动点 6：间隔数值不裸改字面）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_log_time_cache_update(") != NULL,
        "DLG-A4 RED: application.cc 未消费 kidpal_log_time_cache_update(——预格式化"
        "缓存无产出方（低频任务 = main event loop 1s clock tick，R1 缓解的产出半；"
        "驱动方 <-> 消费方闭环缺失 = 钩子永远拼空缓存）");
    n = dlg_count_sub(code, "clock_ticks_ % 10");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        dlg_fm("DLG-A4 RED: application.cc 仍存在 %d 处裸 clock_ticks_ %% 10——SystemInfo "
               "10s 打印字面未退场，降频决策单源被旁路（同窗双写 = 谁生效说不清）", n));
    free(code);

    /* ---- ⑥ log_semantics.h：冻结数值单源（EQ-2） ---------------------------- */
    code = dlg_strip_file(DLG_SEM_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(code,
        "DLG-A4 RED: firmware/main/diagnostics/log_semantics.h 未落地——纯逻辑 seam "
        "与冻结数值单源缺席（契约 [DLG-C1]-[DLG-C3]）");
    TEST_ASSERT_TRUE_MESSAGE(dlg_first_int_after_on_line(code,
                              "#define KIDPAL_LOG_SYSINFO_INTERVAL_MS", &v) &&
                             v == 60000,
        dlg_fm("DLG-A4 RED: log_semantics.h 的 KIDPAL_LOG_SYSINFO_INTERVAL_MS 非字面 "
               "60000（取到 %ld）——10s -> 60s 降频冻结值漂移（TEST 3 镜像常量失去锚）",
               v));
    TEST_ASSERT_TRUE_MESSAGE(dlg_first_int_after_on_line(code,
                              "#define KIDPAL_LOG_TIME_PREFIX_CAP", &v) && v == 32,
        dlg_fm("DLG-A4 RED: log_semantics.h 的 KIDPAL_LOG_TIME_PREFIX_CAP 非字面 32"
               "（取到 %ld）——前缀容量冻结值漂移（TEST 1 镜像常量失去锚）", v));
    free(code);
}

/* ========================================================================== *
 * TEST 5 — DLG-A5 回归网登记守（anti-vacuity / 注册序 / 双侧登记）。
 * coverage: DLG-A5 -- sd_log / key_layer_dispatch / time_sync 三冻结件登记在守 +
 *           本件登记于 host smoke 之前（注册序守卫先例）+ diagnostics/
 *           log_semantics.c host/设备双侧登记守；运行面 = 红基线实跑对照
 *           222T3F（先存红 = p1_auxbar AB-A1/A2/A4，锚 5412af06）
 * RED today（设计态）: log_semantics.c 双侧登记缺席（本验收件登记已由
 *           acceptance-author 落）。
 * ========================================================================== */
TEST_CASE("diag log: regression net registration guards — trio present, order, dual-side impl (DLG-A5)",
          "[diag_log][DLG-A5][structural]")
{
    char *cmake;
    const char *p_self;
    const char *p_smoke;

    DLG_REQUIRE_ROOT();

    /* ---- host runner 登记守 ------------------------------------------------- */
    cmake = dlg_read(DLG_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "DLG-A5 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "host_infra_smoke.acceptance.c") != NULL,
        "DLG-A5 RED(infra): host CMakeLists 无 host_infra_smoke.acceptance.c——smoke "
        "末位锚缺席（注册序守卫失去基准，须复核不得改判为绿）");

    p_self = strstr(cmake, "diag_log.acceptance.c");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_self,
        "DLG-A5 RED: host_app/main/CMakeLists.txt 未登记 diag_log.acceptance.c——本"
        "冻结验收件不进 host runner（全部断言不会被执行 = 空过绿）");
    p_smoke = strstr(cmake, "host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(p_self < p_smoke,
        "DLG-A5 RED: diag_log.acceptance.c 登记在 host_infra_smoke.acceptance.c 之后"
        "——必须置于 smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫"
        "先例）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/log_semantics.c") != NULL,
        "DLG-A5 RED: host_app/main/CMakeLists.txt 未登记纯逻辑源 diagnostics/"
        "log_semantics.c——行为断言将只跑 weak 哨兵（EQ-1：哨兵红会掩盖「未接线」，"
        "反之接线后仍红说明登记错）；builder FIRST ACTION");

    /* ---- 回归网成员在守（不被本包登记改动静默摘除） ------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "sd_log.acceptance.c") != NULL,
        "DLG-A5 RED: 回归网成员 sd_log.acceptance.c 从 host 登记消失——落卡管线"
        "冻结件（[C1]-[C5]）必须继续全量在跑，[C4] 三锚的回归面不得静默缩网");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "key_layer_dispatch.acceptance.c") != NULL,
        "DLG-A5 RED: 回归网成员 key_layer_dispatch.acceptance.c 从 host 登记消失"
        "——按键语义名改动（板 .cc 挂接）的回归面不得静默缩网");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "time_sync.acceptance.c") != NULL,
        "DLG-A5 RED: 回归网成员 time_sync.acceptance.c 从 host 登记消失——时间前缀"
        "（已同步/未同步判定）的回归面不得静默缩网");
    free(cmake);

    /* ---- 设备构建登记守 ----------------------------------------------------- */
    cmake = dlg_read(DLG_MAIN_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "DLG-A5 RED(infra): firmware/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/log_semantics.c") != NULL,
        "DLG-A5 RED: firmware/main/CMakeLists.txt 显式 SOURCES 未登记 diagnostics/"
        "log_semantics.c——main 目录非 GLOB，设备构建拿不到纯逻辑源（DLG-X2 设备"
        "编译闸必红）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "diagnostics/sd_log_fs.cc") != NULL,
        "DLG-A5 RED(infra): firmware/main/CMakeLists.txt 既有 diagnostics/sd_log_fs.cc "
        "登记缺席——芯片面被移出设备构建（回归面受损，须复核不得改判为绿）");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-22 初版冻结（TaskPackage 261022-diag-log-content; acceptance[] DLG-A1~
 *               A5 + DLG-A6 structural/live-manual 为冻结边界; reviewed plan（architect
 *               /triage-change 2026-10-22 比例化 review + human 触点①确认）为语义
 *               权威）。五个 TEST_CASE：DLG-A1 行为一件（时间前缀格式化决策 + 越界
 *               防御 + 预格式化缓存语义）、DLG-A2 行为一件（五族语义名闭集 +
 *               fallback）、DLG-A3 行为一件（SystemInfo 60s 降频决策 + 幂等），
 *               DLG-A4 结构一件（五缝源锚 + 头文件数值单源 + 扫描器自检），DLG-A5
 *               结构一件（回归网登记守 + 注册序 + log_semantics.c 双侧登记）。
 *               DLG-A6 为 live-manual 真机轮观测项，不落自动断言（SLP-A6 同款口径），
 *               本 Pod 到真机门前停等 human。冻结落盘冒烟 = host 全量 runner 实跑
 *               （222 Tests 3 Failures 与已知基线一致——本件尚未登记进 runner，
 *               261018 先例口径，登记归 builder FIRST ACTION）+ 独立冒烟通道实编
 *               实跑（5 Tests 5 Failures, 零 warning，红名逐条 = 设计态缺失面）
 *               + 双向可满足性以 /tmp 合成树（KIDPAL_REPO_ROOT 注入）一次性参考
 *               实现取证，不写入工作树。
 * --------------------------------------------------------------------------- */
