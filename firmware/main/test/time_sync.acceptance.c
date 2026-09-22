/*
 * Frozen acceptance tests — TaskPackage 260922-m3-k5-sntp-time-sync (subsystem:
 * firmware; M3-K5; SNTP 校时 + PCF85063 写回 + 断电续航 + 周期 re-inject · rigor=lite,
 * riskTier=medium).
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1 时间/日期 + AC4 断网本地显示）+ NFR-OFF-1
 * 离线口径。设备全部常显时间面（chrome 状态栏 HH:MM + P1 辅助条时间/日期）显示真实本地
 * 时间：联网自动 NTP 对时（SNTP）→ 写回 PCF85063 硬件 RTC（断电由纽扣电池续航）→ boot
 * 从 RTC settimeofday 恢复系统时间（断网重启也正确）→ P1 时间消费面周期 re-inject 修掉
 * boot 快照冻结（F2）。R3 追加轮重指（human 拍板, 见 REVISION LOG）: chrome 状态栏
 * 时钟隐藏退役, 唯一常显时间面 = P1 辅助条（时间/日期）; re-inject 读点对齐分钟边界。
 * 语义权威 = plan.md（triage ②单包 Lite，human 拍板 F1=(a)+F2 随包，
 * 2026-09-22）；验收方向 1-6 与关键接缝（SNTP 挂点 / 写回 / re-inject / TZ / sdkconfig）
 * 以 plan.md 为准。
 *
 * 现状差距（plan.md 根因修正 + 实测锚）：方向 B 短路激活后 ActivationTask()
 * （application.cc:330-348）不再调 Ota::CheckVersion()——它是全固件唯一解析 server_time
 * 并 settimeofday 的地方（ota.cc:186-204，现死代码，铁律 K5-E2 不复活）；固件无任何
 * SNTP。真机串口刷 `System time is not set, tm_year: 70`，显示层守门
 * （lvgl_display.cc:129 `tm_year >= 2025 - 1900`）拒绝渲染时钟。PCF85063 现只读
 * （waveshare-s3-rlcd-4.2.cc ReadRtcTime，OS 位被掩掉丢弃）；boot 读 RTC 仅注入 P1
 * （ReadRtcTime 全固件恰 1 个调用点 → kidpal_render_node_inject_rtc 一次），无
 * settimeofday 恢复、无周期重读——P1 时间是 boot 快照（F2 冻结缺陷）。
 *
 * ===========================================================================
 * Frozen seam contract pinned by this file（builder implements，本文件只读扫工作树 +
 * weak 哨兵行为级验证；「home =」为冻结落点）:
 *
 *   [S1] 纯逻辑时间缝单源（home = firmware/main/time_sync.c + firmware/main/time_sync.h，
 *        新建；C linkage、零芯片依赖、零 ESP-IDF include——host_app linux target 与设备树
 *        同源，接线见两侧 CMakeLists）。冻结签名（本文件顶部原型为准，六函数）:
 *         int  kidpal_ts_bcd_encode(int dec);
 *              -- 0..99 -> 0x00..0x99（十位<<4|个位）；越界（<0 或 >99）-> -1。
 *         int  kidpal_ts_bcd_decode(int bcd);
 *              -- 两个 nibble 均 <= 9 -> hi*10+lo（0x59 -> 59）；任一 nibble > 9（坏 BCD）
 *                 -> -1（防御：寄存器脏位不得被当合法时间）。
 *         int  kidpal_ts_year_ok(int tm_year_raw);
 *              -- tm_year-1900 口径 raw >= 125（即 >= 2025 年）-> 1，否则 0。语义锚 =
 *                 lvgl_display.cc:129 守门（`tm_year >= 2025 - 1900`）同阈值；raw 70
 *                 （1970，`tm_year: 70` 负锚点）必须被拒。
 *         int  kidpal_ts_write_decision(int os_bit_set, int year_ok);
 *              -- 时钟写回/恢复统一可信门（写回 PCF85063 与 boot settimeofday 恢复共用，
 *                 不把坏时间当权威）：os_bit_set != 0（PCF85063 oscillator stop = 晶振曾
 *                 停振，如纽扣电池耗尽）-> 0（不写、等 SNTP 降级）；year_ok == 0 -> 0；
 *                 两者皆可信 -> 1。入参按非零=真布尔解释。
 *         int  kidpal_ts_backoff_delay_ms(int consecutive_failures);
 *              -- NTP 失败退避（不重试风暴）：failures < 0 -> -1（防御）；否则恒 > 0，
 *                 且随 failures 非递减、有封顶（足够大的 failures 处 delay 不再增长）；
 *                 delay(f=0) >= 1000（失败后至少 1s 才允许重试）。
 *         int  kidpal_ts_log_throttled(long long now_ms, long long last_ms,
 *                                      long long min_interval_ms);
 *              -- W 日志节流谓词（`System time is not set` 现每 10s 一条，降噪）：返回
 *                 1 = 抑制本条。now - last < min_interval -> 1（含相等 = 间隔未走完）；
 *                 now - last >= min_interval -> 0；now < last（时钟被回拨/校正）-> 0
 *                 （放行，fail-open）；min_interval <= 0 -> 0（fail-open 恒放行）。
 *        设备侧消费（真机接线，见 [S2]/[S3]/[S4]）：板级写回/恢复走 bcd_encode +
 *        write_decision；应用层 SNTP 退避走 backoff_delay_ms；lvgl_display.cc 的
 *        `System time is not set` W 日志改经 log_throttled 节流。
 *
 *   [S2] SNTP 挂点（home = firmware/main/application.cc；plan 关键接缝）：WiFi got IP 后
 *        （HandleNetworkConnectedEvent 路径）或 activation done 后（HandleActivationDoneEvent
 *        路径）启动 SNTP，异步、不阻塞激活/主流程；server 列表含 ntp.aliyun.com
 *        （主）+ 备选（plan 关键接缝）；TZ 走 POSIX `CST-8`（UTC+8；**勿照抄**
 *        emote_display.cc:488 的 `GMT+0` 错误先例，F4）。结构齿钉：application.cc 内
 *        esp_netif_sntp / esp_sntp_init 起始符号在世 + 两个事件函数区之一引用 sntp/
 *        time_sync 家族命名（挂点在网络路径上的调用证据）+ ntp.aliyun.com + CST-8。
 *        sdkconfig 前提（plan 关键接缝）：firmware/sdkconfig.defaults 的
 *        CONFIG_LWIP_SNTP_MAX_SERVERS >= 3（现无该行 / 生成面 =1，多 server 前提缺失）。
 *
 *   [S3] PCF85063 写回 + 断电恢复（home = firmware/main/boards/waveshare-s3-rlcd-4.2/
 *        waveshare-s3-rlcd-4.2.cc）：新增 I2C 写路径 WriteRtcTime（UTC+8 本地 BCD 写
 *        0x04–0x09，BCD 编码经 [S1] kidpal_ts_bcd_encode——不得旁路自写一份）；写回与
 *        boot 恢复前过 [S1] write_decision（OS 位先读不掩掉）；boot 读 RTC 后
 *        settimeofday 恢复系统时间（现状缺口 = 只注入 P1 不恢复）。
 *
 *   [S4] 周期 re-inject（F2 修，home = waveshare-s3-rlcd-4.2.cc）：P1 时间消费面从
 *        「boot 一次 ReadRtcTime → kidpal_render_node_inject_rtc」改周期重读重注入；
 *        re-inject 频率与 P1 重绘节拍协调（plan 边角 F2：避免跳字/撕裂——分钟级时间
 *        语义下重读周期不高于分钟粒度，真机面挂设备轮）。结构齿钉：板级 ReadRtcTime
 *        调用点 >= 2（boot + 周期）且 kidpal_render_node_inject_rtc 调用点 >= 2。
 *        inject 家族（kidpal_render_node_inject_rtc / s_rtc_hour）保持在世零偏移
 *        （p1_home_battery.acceptance.c guard 齿只读消费，本文件 TEST 6 复钉）。
 *
 *   [S5] 铁律守卫（K5-E2）：ota.cc server_time 路径保持现状死代码（`server_time` 解析 +
 *        settimeofday 留在原地，不删不复活）；application.cc 零 `CheckVersion(` 调用
 *        （OTA 校时通道不复活）；渲染版式/帧缓冲链路与电池链路 owner 边界零触碰
 *        （lvgl 守门行 `2025 - 1900` 在世为本文件 MUST-STAY-GREEN 钉点）。
 *
 *   [S6]（R3 追加轮新增, 2026-09-22）分钟对齐纯逻辑缝（home = time_sync.c/.h 第七
 *        函数; C linkage、零芯片依赖、零 ESP-IDF include, 同 [S1] 纪律）。冻结签名:
 *         long long kidpal_ts_ms_to_next_minute(long long now_ms);
 *              -- now_ms < 0 -> -1（防御）; 否则 = 60000 - (now_ms % 60000), 恒在
 *                 [1,60000]（恰在分钟边界 -> 60000 = 整周期后仍落边界, 禁 0 延迟
 *                 忙旋）。入参口径 = 毫秒时钟且其分钟边界与墙上分钟边界对齐
 *                 （epoch ms 满足; CST-8 整分偏移, 本地分界 = UTC 分界）。读点
 *                 语义 = 按返回值 sleep 后立即读 RTC 并注入 -> P1 显示滞后 = 唤醒
 *                 抖动 < 1s（human 追加拍板②: 读到秒 ≈0 再读再注入）。
 *        设备侧消费（真机接线, [S4] 节拍相位修正）: TimeSyncPeriodicLoop 的
 *        vTaskDelay 由该函数按墙上分钟边界计算（替代挂 boot 相位的固定 60000ms）;
 *        inject 家族语义零触碰。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K5-A1 SNTP 校时正锚 -> test 2（host 半：tm_year:70 负锚拒绝 + 守门同阈值
 *             year_ok 行为表）+ test 4（结构半：SNTP 起始符号 + 网络路径挂点 + server
 *             列表 + MAX_SERVERS 前提 + CST-8 TZ）；真机半（串口 SNTP 同步成功日志 +
 *             `tm_year: 70` 清零 + P1 辅助条时间/日期正确渲染——R3 重指, chrome 状态
 *             栏时钟已隐藏退役, 负锚 = test 7）= deferred 设备轮窗口（K5-E3，
 *             语义锚 plan 验收方向 1，不伪造 host 证据）。
 *   coverage: K5-A2 PCF85063 写回 -> test 1（BCD 编解码行为表 + 0..99 round-trip）+
 *             test 2（OS 位降级决策：OS 置位不写回）+ test 5（结构半：WriteRtcTime 写
 *             路径 + bcd_encode/write_decision 真消费接线）。
 *   coverage: K5-A3 断电续航恢复 -> test 2（恢复同走可信门：year 不 plausible 不恢复）+
 *             test 5（结构半：boot 读 RTC 后 settimeofday 恢复路径在世）；真机半（断电
 *             重启 + 保持断网显示正确时间）= deferred 设备轮窗口（语义锚 plan 验收方向
 *             3 / S-F1.1 AC4 / NFR-OFF-1）。RTC 读出值直接喂 inject 不经 TZ（寄存器存
 *             本地 BCD）= [S1]/[S3] 契约纪律，本包不动 inject 语义（现状已直读直注）。
 *   coverage: K5-A3b 周期 re-inject -> test 5（结构半：ReadRtcTime 调用点 >= 2 +
 *             inject_rtc 调用点 >= 2 = boot 一次快照退役）+ test 6（inject 家族在世
 *             零偏移守卫）；真机半（长开机 P1 时间继续走不冻结）= deferred 设备轮窗口
 *             （语义锚 plan 验收方向 2）。
 *   coverage: K5-A4 失败降级负锚 -> test 3（退避决策表：非递减 + 封顶 + 最小间隔，不重试
 *             风暴；W 日志节流谓词边界表）+ test 4（退避/节流经 [S1] 缝的真实消费接线）；
 *             真机半（域名置错后主流程不卡/无 crash/守门维持）= deferred 设备轮或 host
 *             降级语义可证（K5-E3；语义锚 plan 验收方向 4）。
 *   coverage: K5-A5 回归 + device build 闸 -> test 6（HOST_ACCEPTANCE_SRCS 注册守卫（本
 *             验收件 + time_sync.c，且既有 p1_home_battery / schedule_grid 在世）+
 *             inject 家族零偏移 + 需求契约 canary + 扫描器自检）+ test 4 tooth 1/2
 *             （time_sync.h 在世 + 设备 CMakeLists 接线）；device build 闸（xiaozhi.bin
 *             构建过 + 体积余量记录）= builder/verifier 相位义务，非 host 齿（头注
 *             DEFERRED 清单）；既有齿零偏移由红基线通道①全量实跑承载。
 *   coverage: K5-E2 铁律 -> test 4 tooth 8/9/10（CheckVersion 零调用 + ota.cc server_time
 *             死代码留置 + lvgl 守门行在世）。
 *   coverage: K5-R3a 状态栏时钟隐藏（R3 追加轮 human 拍板①, 授权链 pod-events
 *             human-decision fc43063d）-> test 7（结构半: UpdateStatusBar 区 SetStatus(
 *             零调用 = chrome 状态栏时钟渲染退役负锚 + mute/网络/低电 popup 渲染路径
 *             在世齿 + P1 辅助条时间面在世 = A1 重指承载）; 真机半（隐藏后目测: 状态栏
 *             无时钟、其余状态栏元素与 P1 辅助条正常）= deferred 设备轮窗口。
 *   coverage: K5-R3b re-inject 分钟对齐（R3 追加轮 human 拍板②, 同授权链）-> test 8
 *             （行为半: [S6] kidpal_ts_ms_to_next_minute 决策表 + 值域不变量 + 结构半:
 *             板级 TimeSyncPeriodicLoop 消费接线 + boot 相位固定节拍退役）——滞后 <1s
 *             的 host 可证部分落纯函数行为表; 真机半（读点秒 ≈0、P1 滞后 <1s 目测）=
 *             deferred 设备轮窗口。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载齿 -> 性质档；acceptance-evidence T2 纪律）:
 *   S-F1.1 AC1 时间/日期显示正确 -> K5-A1 齿组 -> 行为级（year_ok 决策表，host 可判）+
 *     源锚代理（lvgl 守门行在世钉点）；真机渲染正确性 deferred 设备轮（证据缺口 = host
 *     无法渲染 chrome 状态栏真帧，已显式挂设备轮，不以 host 伪证顶替）。
 *   S-F1.1 AC4 / NFR-OFF-1 离线口径 -> K5-A3 齿组 -> 机制级（settimeofday 恢复路径结构
 *     在世）+ 行为级（可信门决策表）；断电续航端到端 deferred 设备轮。
 *   行为等价假设（显式化）：本文件假设「BcdToDec/DecToBcd 纯函数与寄存器读写分离等价」
 *     （板级 BCD 走 [S1] 单源；若 builder 在板文件旁路自写 BCD，test 5 tooth 2 红）、
 *     「写回与恢复共用同一可信门」（若写回绕过 write_decision，test 5 tooth 3 红）。
 *   K5-R3a 状态栏时钟隐藏（R3 追加轮）-> test 7 -> 源锚代理（UpdateStatusBar 区渲染
 *     调用缺失 + 其余元素在世齿; host 无法渲染 lvgl 真帧, 隐藏目测 = deferred 设备轮,
 *     不以 host 伪证顶替）; 弱于契约处缺口 = 真机「时钟确实不再出现在屏上」仅设备轮
 *     可证, 显式挂账。
 *   K5-R3b re-inject 分钟对齐（R3 追加轮）-> test 8 -> 行为级（[S6] 纯函数决策表 +
 *     值域不变量, host 可判）+ 机制级（板级消费接线 + boot 相位退役）; 行为等价假设
 *     （显式化）:「按 ms_to_next_minute 返回值 sleep 后立即读注入 = 读点秒 ≈0」——
 *     唤醒抖动（vTaskDelay tick 粒度）是唯一残差, 真机滞后 <1s 的端到端证实挂设备轮。
 *
 * ---------------------------------------------------------------------------
 * DEFERRED（真机半，挂设备轮窗口收口，human 在场；K5-E3 / plan 验收方向 1-3）:
 *   - K5-A1 真机：SNTP 同步成功事件日志；`System time is not set, tm_year: 70` 清零；
 *     P1 辅助条时间/日期 HH:MM 正确（R3 重指）+ chrome 状态栏时钟不再渲染（隐藏后
 *     目测, test 7 的真机半）。
 *   - K5-R3b 真机：长开机观察 re-inject 读点落分钟边界（秒 ≈0）、P1 时间滞后 <1s
 *     （test 8 的真机半）。
 *   - K5-A3b 真机：长开机 >=N 分钟 P1 时间继续走（F2 收口证据）。
 *   - K5-A3 真机：SNTP 写 RTC 成功后断电重启 + 保持断网 → 显示正确时间。
 *   - K5-A4 真机：NTP 不可达 → 激活/对话照常、无 crash、守门拒绝渲染维持。
 *   - K5-A5 device build 闸：xiaozhi.bin 构建过 + 体积余量记录（builder/verifier 相位）。
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) 新建 firmware/main/time_sync.h + time_sync.c：按 [S1] 冻结签名实现六函数
 *      （纯逻辑、零芯片依赖、零 ESP-IDF include；C linkage）。
 *   2) firmware/main/CMakeLists.txt：SOURCES 加 "time_sync.c"（设备构建接线）。
 *   3) firmware/main/application.cc：WiFi got IP / activation done 后异步启动 SNTP
 *      （[S2]；esp_netif_sntp 或 esp_sntp 起始符号落在 application.cc；server 列表含
 *      ntp.aliyun.com；TZ `CST-8`；失败退避经 kidpal_ts_backoff_delay_ms；**不得**调用
 *      CheckVersion——OTA 校时通道保持死代码）。
 *   4) firmware/main/sdkconfig.defaults：CONFIG_LWIP_SNTP_MAX_SERVERS >= 3（多 server
 *      前提；生成面 sdkconfig 重配跟进由 builder 一并处理）。
 *   5) firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc：新增
 *      WriteRtcTime I2C 写路径（0x04–0x09 本地 BCD，经 [S1] bcd_encode）；OS 位先读，
 *      写回与 boot settimeofday 恢复均过 [S1] write_decision；boot 读 RTC 后
 *      settimeofday 恢复系统时间；周期重读 RTC 并重注入（[S4]，与 P1 重绘节拍协调）。
 *   6) firmware/main/display/lvgl_display/lvgl_display.cc：`System time is not set`
 *      W 日志改经 kidpal_ts_log_throttled 节流（守门行 `2025 - 1900` 与渲染链路零触碰）。
 *   7) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *      加 "../../../time_sync.c"（强符号覆盖 weak 哨兵的关键）与 "../../time_sync.
 *      acceptance.c"，置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位）。
 *   8) mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为子串**
 *      （KR-A3 _TOGGLE_X 教训；用 _STUB 类整词替换），mutationEvidence.perAcceptance
 *      逐条回填。
 *
 *   R3 追加轮 BUILDER DUTY（2026-09-22, human 拍板①②, 授权链 pod-events
 *   human-decision fc43063d; host 接线零改——本文件已注册 HOST_ACCEPTANCE_SRCS、
 *   time_sync.c 已接强符号源, 修订齿随本文件自动携带进 host runner）:
 *   R3-1) firmware/main/display/lvgl_display/lvgl_display.cc：摘除 UpdateStatusBar
 *         区的 chrome 状态栏时钟渲染（SetStatus(time_str) 调用退役, test 7 tooth 2;
 *         守门行 `2025 - 1900` 与 kidpal_ts_log_throttled 节流分支保留零触碰——
 *         test 4 teeth 3/14 既有 MUST-STAY-GREEN 齿把住）; mute/网络/低电 popup
 *         渲染路径零偏移（test 7 teeth 3-5）; 禁整函数退役。
 *   R3-2) firmware/main/time_sync.{h,c}：按 [S6] 冻结签名实现第七函数
 *         kidpal_ts_ms_to_next_minute（纯逻辑、零芯片依赖; test 8 teeth 0-3）; 板级
 *         TimeSyncPeriodicLoop 节拍 vTaskDelay 改由该函数按墙上分钟边界计算（退役
 *         固定 KIDPAL_TS_PERIODIC_PERIOD_MS 挂 boot 相位调用, test 8 teeth 4/5）;
 *         inject 家族零触碰。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *      layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* -------------------------------------------------------------------------- *
 * [S1] 冻结签名原型（time_sync.h 落地后语义一致；本文件先以原型 + weak 哨兵冻结，
 * builder 的 time_sync.c 强定义覆盖哨兵。接线前哨兵返回专用哨兵值，红基线读作
 * 「frozen seam not implemented」——真实缺失行为红，绝非链接错误）。
 * ------------------------------------------------------------------------ */
int kidpal_ts_bcd_encode(int dec);
int kidpal_ts_bcd_decode(int bcd);
int kidpal_ts_year_ok(int tm_year_raw);
int kidpal_ts_write_decision(int os_bit_set, int year_ok);
int kidpal_ts_backoff_delay_ms(int consecutive_failures);
int kidpal_ts_log_throttled(long long now_ms, long long last_ms,
                            long long min_interval_ms);
/* [S6] R3 追加轮第七函数（分钟对齐缝; time_sync.h 落地后语义一致）。 */
long long kidpal_ts_ms_to_next_minute(long long now_ms);

/* weak 哨兵（专用哨兵值：-2 / -1，均非任何合法返回值——见 TEST 1/2/3 首齿 require）。 */
__attribute__((weak)) int kidpal_ts_bcd_encode(int dec)
{
    (void)dec;
    return -2;
}
__attribute__((weak)) int kidpal_ts_bcd_decode(int bcd)
{
    (void)bcd;
    return -2;
}
__attribute__((weak)) int kidpal_ts_year_ok(int tm_year_raw)
{
    (void)tm_year_raw;
    return -1;
}
__attribute__((weak)) int kidpal_ts_write_decision(int os_bit_set, int year_ok)
{
    (void)os_bit_set; (void)year_ok;
    return -1;
}
__attribute__((weak)) int kidpal_ts_backoff_delay_ms(int consecutive_failures)
{
    (void)consecutive_failures;
    return -2;
}
__attribute__((weak)) int kidpal_ts_log_throttled(long long now_ms, long long last_ms,
                                                  long long min_interval_ms)
{
    (void)now_ms; (void)last_ms; (void)min_interval_ms;
    return -1;
}
/* [S6] 哨兵 -2（合法返回域 = [1,60000] 或防御 -1, -2 非任何合法返回值）。 */
__attribute__((weak)) long long kidpal_ts_ms_to_next_minute(long long now_ms)
{
    (void)now_ms;
    return -2;
}

/* -------------------------------------------------------------------------- *
 * 被扫文件（真实工作树，root 自手持 cwd 定位；test 目录排除——本冻结文件自身住
 * test 目录，其注释合法提及 needle，不进扫描）。
 * ------------------------------------------------------------------------ */
#define TS_APP_CC      "firmware/main/application.cc"
#define TS_OTA_CC      "firmware/main/ota.cc"
#define TS_BOARD_CC    "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define TS_LVGL_CC     "firmware/main/display/lvgl_display/lvgl_display.cc"
#define TS_RN_H        "firmware/main/display/kidpal_render_node.h"
#define TS_RN_CC       "firmware/main/display/kidpal_render_node.cc"
#define TS_MAIN_CML    "firmware/main/CMakeLists.txt"
#define TS_HOST_CML    "firmware/main/test/host_app/main/CMakeLists.txt"
#define TS_SDKDEF      "firmware/sdkconfig.defaults"
#define TS_TIME_SYNC_H "firmware/main/time_sync.h"
#define TS_HOME_P1_C   "firmware/main/display/home_p1.c"

#define TS_MAX_PATH 1024
#define TS_MAX_READ (8u * 1024u * 1024u)

static char ts_root[TS_MAX_PATH];
static int ts_root_ready;

static int ts_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int ts_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int ts_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int ts_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[TS_MAX_PATH];

    if (ts_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (ts_is_regular(probe)) {
            snprintf(ts_root, sizeof(ts_root), "%s", cands[i]);
            ts_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int ts_rel(char *out, size_t cap, const char *rel)
{
    if (!ts_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", ts_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", ts_root, rel);
    return 1;
}

static int ts_exists(const char *rel)
{
    char p[TS_MAX_PATH];
    return ts_rel(p, sizeof(p), rel) && ts_is_regular(p);
}

static char *ts_read(const char *rel, size_t *out_len)
{
    char p[TS_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ts_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > TS_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int ts_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

/* 对 malloc'd slice in-place 小写后查找（slice 由调用方持有、随后释放）。 */
static int ts_slice_contains_ci(char *slice, const char *needle_lower)
{
    char *p;
    char low_nd[64];

    if (slice == NULL || needle_lower == NULL) {
        return 0;
    }
    for (p = slice; *p != '\0'; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
    {
        size_t i;
        for (i = 0; i + 1 < sizeof(low_nd) && needle_lower[i] != '\0'; i++) {
            low_nd[i] = needle_lower[i];
        }
        low_nd[i] = '\0';
    }
    return strstr(slice, low_nd) != NULL;
}

static int ts_file_contains(const char *rel, const char *needle)
{
    char *buf = ts_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = ts_contains(buf, needle);
    free(buf);
    return r;
}

static int ts_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = ts_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

static int ts_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                      const char *needle, char *first_hit, size_t cap,
                                      int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[TS_MAX_PATH];
        char rel_next[TS_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            ts_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += ts_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > TS_MAX_READ) {
                continue;
            }
            {
                char *buf = ts_read(rel_next, NULL);
                if (buf != NULL) {
                    if (ts_contains(buf, needle)) {
                        if (count == 0 && first_hit != NULL) {
                            snprintf(first_hit, cap, "%s", rel_next);
                        }
                        count++;
                    }
                    free(buf);
                }
            }
        }
    }
    closedir(d);
    return count;
}

static int ts_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                         size_t cap)
{
    char abs[TS_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!ts_rel(abs, sizeof(abs), rel_dir) || !ts_is_dir(abs)) {
        return 0;
    }
    return ts_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL.
 * Caller frees. */
static char *ts_region_between(const char *rel, const char *start_needle,
                               const char *end_needle)
{
    char *buf = ts_read(rel, NULL);
    char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

#define TS_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(ts_locate_root(), \
    "TS RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper（K1a 先例：动态诊断先格式化进静态缓冲再进单串断言宏）。 */
static char ts_fm_buf[1024];
static const char *ts_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *ts_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ts_fm_buf, sizeof(ts_fm_buf), fmt, ap);
    va_end(ap);
    return ts_fm_buf;
}

/* weak 哨兵红统一判读：哨兵值 = frozen seam NOT implemented（builder 未落 [S1]），
 * 区别于行为表真红（实现落地但语义漂移）。 */
static int ts_is_sentinel(int v)
{
    return v == -1 || v == -2;
}

static void ts_require_implemented(int v, const char *what)
{
    if (ts_is_sentinel(v)) {
        TEST_ASSERT_TRUE_MESSAGE(0, ts_fm(
            "TS RED(frozen seam): %s 未实现（weak 哨兵 %d）——builder 须落 "
            "firmware/main/time_sync.c [S1] 六函数并接线 HOST_ACCEPTANCE_SRCS "
            "（BUILDER DUTY 1)/7))", what, v));
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — K5-A2 行为半：BCD 编解码决策表（PCF85063 寄存器 0x04–0x09 编码域）
 * RED today: time_sync.c 未落（weak 哨兵 -2）。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A2 -- BCD 编解码行为表：encode 0..99 双 nibble + 越界 -1；decode 合法域 + 坏 nibble 防御 -1；0..99 全域 round-trip */
TEST_CASE("时间同步: BCD编解码决策表 (K5-A2)", "[time_sync][K5-A2][behavior]")
{
    static const int enc_in[]  = {  0,  1,  9, 10, 25, 59, 99 };
    static const int enc_out[] = { 0x00, 0x01, 0x09, 0x10, 0x25, 0x59, 0x99 };
    size_t i;
    int v;

    /* tooth 0: 哨兵判读（frozen seam 在世义务）。 */
    ts_require_implemented(kidpal_ts_bcd_encode(25), "kidpal_ts_bcd_encode");

    /* tooth 1: encode 决策表（十位<<4|个位）。 */
    for (i = 0; i < sizeof(enc_in) / sizeof(enc_in[0]); i++) {
        int got = kidpal_ts_bcd_encode(enc_in[i]);
        TEST_ASSERT_TRUE_MESSAGE(got == enc_out[i],
            ts_fm("TS-A2 RED: bcd_encode(%d)=%d 期望 0x%02X——BCD 编码语义漂移"
                  "（寄存器 0x04-0x09 写入域）", enc_in[i], got, enc_out[i]));
    }

    /* tooth 2: encode 越界防御（非法十进制不得编出假 BCD）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_bcd_encode(100) == -1 &&
                                 kidpal_ts_bcd_encode(-1) == -1 &&
                                 kidpal_ts_bcd_encode(256) == -1,
        "TS-A2 RED: bcd_encode 越界入参未返回 -1——非法值被静默编入寄存器域");

    /* tooth 3: decode 决策表（含 0x07 日寄存器/0x09 月寄存器小值域）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_bcd_decode(0x00) == 0 &&
                                 kidpal_ts_bcd_decode(0x07) == 7 &&
                                 kidpal_ts_bcd_decode(0x10) == 10 &&
                                 kidpal_ts_bcd_decode(0x59) == 59 &&
                                 kidpal_ts_bcd_decode(0x99) == 99,
        "TS-A2 RED: bcd_decode 合法域语义漂移——0x59 必须读作 59 分钟"
        "（raw 0x48=48 分教训, 板文件头注现状锚）");

    /* tooth 4: decode 坏 nibble 防御（脏位不得被当合法时间）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_bcd_decode(0x0A) == -1 &&
                                 kidpal_ts_bcd_decode(0xA0) == -1 &&
                                 kidpal_ts_bcd_decode(0x1F) == -1,
        "TS-A2 RED: bcd_decode 坏 nibble 未拒绝（-1）——寄存器脏位被当合法时间");

    /* tooth 5: 0..99 全域 round-trip（encode/decode 互逆）。 */
    for (v = 0; v <= 99; v++) {
        int enc = kidpal_ts_bcd_encode(v);
        if (enc != -1) {
            TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_bcd_decode(enc) == v,
                ts_fm("TS-A2 RED: round-trip 失败 decode(encode(%d)) != %d——编解码"
                      "不互逆", v, v));
        }
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — K5-A1/K5-A2/K5-A3 行为半：守门阈值 year_ok + 可信门 write_decision
 * （tm_year:70 负锚 host 半；OS 位降级；坏时间不恢复）。
 * RED today: weak 哨兵 -1。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A1 -- 守门同阈值 year_ok 行为表：raw 70（tm_year:70 负锚）拒绝 + 125（2025）通过 */
/* coverage: K5-A2 -- OS 位降级：os_bit_set=1 恒不写回（时间不可信等 SNTP） */
/* coverage: K5-A3 -- 恢复可信门：year 不 plausible 不 settimeofday（坏时间不当权威） */
TEST_CASE("时间同步: 守门阈值+写回可信门决策表 (K5-A1/K5-A2/K5-A3)",
          "[time_sync][K5-A1][K5-A2][K5-A3][behavior]")
{
    int y, w;

    /* tooth 0: 哨兵判读。 */
    ts_require_implemented(kidpal_ts_year_ok(125), "kidpal_ts_year_ok");
    ts_require_implemented(kidpal_ts_write_decision(0, 1), "kidpal_ts_write_decision");

    /* tooth 1 (K5-A1): year_ok 决策表——raw 70（1970, `tm_year: 70` 真机负锚）必须拒绝；
     * 阈值 125（= 2025-1900, lvgl_display.cc:129 守门同值）边界钉死。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_year_ok(70) == 0,
        "TS-A1 RED: year_ok(70)!=0——1970 未被拒（tm_year:70 负锚点失守, 守门前置"
        "时间可信门不设防）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_year_ok(124) == 0,
        "TS-A1 RED: year_ok(124)!=0——2024 被放行（阈值下沿漂移）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_year_ok(125) == 1,
        "TS-A1 RED: year_ok(125)!=1——2025 被拒（阈值与 lvgl 守门 tm_year>=2025-1900 "
        "漂移）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_year_ok(126) == 1 && kidpal_ts_year_ok(200) == 1,
        "TS-A1 RED: year_ok 合法年份被拒——守门阈值上沿漂移");

    /* tooth 2 (K5-A2): OS 位降级决策——OS 置位（晶振曾停振）恒不写回，无论 year。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_write_decision(1, 1) == 0 &&
                                 kidpal_ts_write_decision(1, 0) == 0,
        "TS-A2 RED: write_decision(OS=1) 未恒拒——坏时间被当权威写回 PCF85063"
        "（OS 位停振语义失守, plan 边角）");

    /* tooth 3 (K5-A3): year 不 plausible 恒不写/不恢复；两者可信才放行。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_write_decision(0, 0) == 0,
        "TS-A3 RED: write_decision(year 不 plausibile) 放行——1970/坏系统时间被写回"
        "或 settimeofday（坏时间当权威）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_write_decision(0, 1) == 1,
        "TS-A3 RED: write_decision(可信) 未放行——SNTP 对好的时间无法写回 RTC/恢复"
        "系统时间（断电续航路径断）");
    /* tooth 4: 布尔宽容（非 0 即真入参语义, [S1] 契约）。 */
    y = kidpal_ts_year_ok(125);
    w = kidpal_ts_write_decision(2, 7); /* 非 0/1 入参按真解释 */
    TEST_ASSERT_TRUE_MESSAGE(w == 0,
        "TS-A2 RED: write_decision 非 0 os_bit 未按真解释——布尔宽容语义漂移");
    (void)y;
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — K5-A4 行为半：失败退避决策表（不重试风暴）+ W 日志节流谓词边界表。
 * RED today: weak 哨兵 -2 / -1。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A4 -- 退避决策表：恒 >0 + 非递减 + 封顶 + 最小 1s + 负入参防御；日志节流边界：未满间隔抑制/满格放行/时钟回拨与 interval<=0 fail-open */
TEST_CASE("时间同步: 失败退避+W日志节流决策表 (K5-A4)", "[time_sync][K5-A4][behavior]")
{
    int f;
    int d0, d_prev, d30, d31;

    /* tooth 0: 哨兵判读。 */
    ts_require_implemented(kidpal_ts_backoff_delay_ms(0), "kidpal_ts_backoff_delay_ms");
    ts_require_implemented(kidpal_ts_log_throttled(0, 0, 1000),
                           "kidpal_ts_log_throttled");

    /* tooth 1: 恒 > 0（失败重试必须有正延迟）。 */
    for (f = 0; f <= 8; f++) {
        int d = kidpal_ts_backoff_delay_ms(f);
        TEST_ASSERT_TRUE_MESSAGE(d > 0,
            ts_fm("TS-A4 RED: backoff_delay_ms(%d)=%d 非正——失败即重试风暴入口",
                  f, d));
    }

    /* tooth 2: 最小退避 >= 1s（f=0 起步即秒级, 不打点风暴）。 */
    d0 = kidpal_ts_backoff_delay_ms(0);
    TEST_ASSERT_TRUE_MESSAGE(d0 >= 1000,
        ts_fm("TS-A4 RED: backoff_delay_ms(0)=%d < 1000——退避起步 < 1s（重试风暴"
              "风险）", d0));

    /* tooth 3: 非递减（退避不回退）。 */
    d_prev = d0;
    for (f = 1; f <= 8; f++) {
        int d = kidpal_ts_backoff_delay_ms(f);
        TEST_ASSERT_TRUE_MESSAGE(d >= d_prev,
            ts_fm("TS-A4 RED: backoff_delay_ms(%d)=%d < 上一次 %d——退避回退（风暴"
                  "语义）", f, d, d_prev));
        d_prev = d;
    }

    /* tooth 4: 封顶（大 f 处不再增长——有限上限, 非无界指数）。 */
    d30 = kidpal_ts_backoff_delay_ms(30);
    d31 = kidpal_ts_backoff_delay_ms(31);
    TEST_ASSERT_TRUE_MESSAGE(d30 == d31 && d30 > 0,
        ts_fm("TS-A4 RED: backoff 未封顶 delay(30)=%d delay(31)=%d——指数退避无界"
              "增长（长断网下重试无上限）", d30, d31));

    /* tooth 5: 负入参防御。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_backoff_delay_ms(-1) == -1,
        "TS-A4 RED: backoff_delay_ms(-1) 未防御（-1）——非法失败计数被接受");

    /* tooth 6: 节流边界——间隔未走完（含相等）抑制。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_log_throttled(9999, 0, 10000) == 1 &&
                                 kidpal_ts_log_throttled(1000, 1000, 10000) == 1,
        "TS-A4 RED: 节流未抑制未满间隔日志——`System time is not set` 每 10s 刷屏"
        "现状未降噪（K5-A4 负锚）");

    /* tooth 7: 间隔走满放行。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_log_throttled(10000, 0, 10000) == 0,
        "TS-A4 RED: 间隔走满仍抑制——节流变成永久静音（故障不可观测）");

    /* tooth 8: 时钟回拨 / interval<=0 fail-open 恒放行。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_log_throttled(5000, 10000, 10000) == 0 &&
                                 kidpal_ts_log_throttled(1000, 0, 0) == 0,
        "TS-A4 RED: fail-open 语义漂移——时钟回拨或 interval<=0 时日志被吞（观测"
        "通道失明）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — K5-A1/K5-A4/K5-E2 结构半：SNTP 挂点 + server 前提 + TZ + OTA 死代码守卫
 * + [S1] 缝真实消费接线 + 需求契约 canary。
 * RED today: application.cc 零 sntp、零 ntp.aliyun.com、零 CST-8、零 backoff 消费；
 * lvgl_display.cc 零 log_throttled 消费；time_sync.h 不存在；sdkconfig.defaults 无
 * CONFIG_LWIP_SNTP_MAX_SERVERS；ota.cc server_time / lvgl 守门行为 MUST-STAY-GREEN。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A1 -- SNTP 挂点结构半：application.cc SNTP 起始符号 + 网络路径事件区引用 + ntp.aliyun.com server 列表 + sdkconfig MAX_SERVERS>=3 前提 + CST-8 TZ（GMT+0 错误先例不复抄） */
/* coverage: K5-A4 -- 退避/节流经 [S1] 缝的真实消费接线（backoff 落应用层 SNTP 侧, log_throttled 落 lvgl 日志点） */
/* coverage: K5-E2 -- 铁律守卫：application.cc 零 CheckVersion( + ota.cc server_time 死代码留置 + lvgl 守门行 2025-1900 在世 */
TEST_CASE("时间同步: SNTP挂点+server前提+OTA死代码守卫+缝消费接线 (K5-A1/K5-A4/K5-E2)",
          "[time_sync][K5-A1][K5-A4][K5-E2][structural]")
{
    char hit[TS_MAX_PATH];
    char *region, *region2;
    int n;

    TS_REQUIRE_ROOT();

    /* tooth 1 (K5-E2 守卫, MUST-STAY-GREEN): application.cc 零 CheckVersion( 调用
     * ——OTA 校时通道不复活（现状即绿, 本齿钉不复活）。 */
    n = ts_count_in_file(TS_APP_CC, "CheckVersion(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        ts_fm("TS-E2 RED: application.cc 出现 %d 处 CheckVersion( 调用——OTA 校时"
              "通道被复活（K5-E2 铁律: ota.cc server_time 路径保持死代码）", n));

    /* tooth 2 (K5-E2 守卫, MUST-STAY-GREEN): ota.cc server_time 死代码留置原地。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_OTA_CC, "server_time"),
        "TS-E2 RED: ota.cc server_time 解析路径被移除——死代码留置守卫失守"
        "（铁律 = 不删不复活; 删除亦属越界改动）");

    /* tooth 3 (K5-E2 守卫, MUST-STAY-GREEN): lvgl 守门行在世（渲染链路零触碰锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_LVGL_CC, "2025 - 1900"),
        "TS-E2 RED: lvgl_display.cc 守门行 `tm_year >= 2025 - 1900` 丢失——显示层"
        "守门被触碰（渲染版式/帧缓冲链路零触碰红线）");

    /* tooth 4 (canary, MUST-STAY-GREEN): 需求面锚在世（S-F1.1 / NFR-OFF-1）+ 契约面在世。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains("docs/requirements.md", "S-F1.1") &&
                                 ts_file_contains("docs/requirements.md", "NFR-OFF-1"),
        "TS RED: requirements S-F1.1 / NFR-OFF-1 锚丢失——需求面被修订（发现冲突走 "
        "spec-lane 逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(ts_exists("docs/contracts/interface-contract.md"),
        "TS RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 5 (MUST-STAY-GREEN): 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_exists(TS_OTA_CC) && ts_exists(TS_APP_CC) &&
                                 ts_exists(TS_BOARD_CC),
        "TS RED(infra): scanner cannot see known-good files — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(ts_exists("firmware/main/__ts_no_such__.cc"),
        "TS RED(infra): scanner reports a nonexistent file as present");
    n = ts_scan_count("firmware/main", "__ts_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "TS RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");

    /* tooth 6: [S1] 缝头文件在世。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_exists(TS_TIME_SYNC_H),
        "TS RED: firmware/main/time_sync.h 不存在——[S1] 纯逻辑时间缝未落"
        "（BUILDER DUTY 1)）");

    /* tooth 7: 设备构建接线（time_sync.c 进 firmware/main/CMakeLists.txt SOURCES）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_MAIN_CML, "time_sync.c"),
        "TS RED: firmware/main/CMakeLists.txt 未接 time_sync.c——设备构建缺时间缝"
        "单源（BUILDER DUTY 2)）");

    /* tooth 8: application.cc 有 SNTP 起始符号（esp_netif_sntp 家族或 esp_sntp_init）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_APP_CC, "esp_netif_sntp") ||
                                 ts_file_contains(TS_APP_CC, "esp_sntp_init"),
        "TS-A1 RED: application.cc 无 SNTP 起始符号——固件仍无任何校时入口"
        "（tm_year:70 现状在世, K5-A1 核心缺口）");

    /* tooth 9: 挂点在网絡路径上——HandleNetworkConnectedEvent 或 HandleActivationDoneEvent
     * 函数区引用 sntp/time_sync 家族命名（plan 关键接缝两许可位；[S2]）。 */
    region = ts_region_between(TS_APP_CC,
                               "void Application::HandleNetworkConnectedEvent()",
                               "void Application::HandleNetworkDisconnectedEvent()");
    region2 = ts_region_between(TS_APP_CC,
                                "void Application::HandleActivationDoneEvent()",
                                "void Application::ActivationTask()");
    TEST_ASSERT_TRUE_MESSAGE(
        (region != NULL && (ts_slice_contains_ci(region, "sntp") ||
                            ts_slice_contains_ci(region, "time_sync") ||
                            ts_slice_contains_ci(region, "timesync"))) ||
        (region2 != NULL && (ts_slice_contains_ci(region2, "sntp") ||
                             ts_slice_contains_ci(region2, "time_sync") ||
                             ts_slice_contains_ci(region2, "timesync"))),
        "TS-A1 RED: SNTP 启动未挂在 WiFi got IP / activation done 事件路径上"
        "（HandleNetworkConnectedEvent / HandleActivationDoneEvent 函数区均无 "
        "sntp/time_sync 家族引用）——挂点漂离网络连接生命周期, 校时时机失锚");
    free(region);
    free(region2);

    /* tooth 10: server 列表含 ntp.aliyun.com（plan 关键接缝主 server）。 */
    n = ts_scan_count("firmware/main", "ntp.aliyun.com", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "TS-A1 RED: firmware/main 无 ntp.aliyun.com——SNTP server 列表缺失"
        "（plan 关键接缝: ntp.aliyun.com 主 + 备）");

    /* tooth 11: sdkconfig 前提——CONFIG_LWIP_SNTP_MAX_SERVERS >= 3（多 server 前提；
     * 现状 sdkconfig.defaults 无此行 / 生成面 =1）。 */
    {
        char *sd = ts_read(TS_SDKDEF, NULL);
        int found = 0;
        TEST_ASSERT_NOT_NULL_MESSAGE(sd, "TS RED(infra): sdkconfig.defaults unreadable");
        {
            const char *key = "CONFIG_LWIP_SNTP_MAX_SERVERS=";
            char *p = strstr(sd, key);
            if (p != NULL) {
                int val = atoi(p + strlen(key));
                found = 1;
                TEST_ASSERT_TRUE_MESSAGE(val >= 3,
                    ts_fm("TS-A1 RED: CONFIG_LWIP_SNTP_MAX_SERVERS=%d < 3——多 server"
                          "（主+2 备）前提缺失, 备用 NTP 服务器不生效", val));
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(found,
            "TS-A1 RED: sdkconfig.defaults 无 CONFIG_LWIP_SNTP_MAX_SERVERS——SNTP "
            "多 server 前提未落（BUILDER DUTY 4)）");
        free(sd);
    }

    /* tooth 12: TZ = POSIX CST-8（UTC+8; F4 符号反转易错; 不复抄 GMT+0 错误先例）。 */
    n = ts_scan_count("firmware/main", "CST-8", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "TS-A1 RED: firmware/main 无 POSIX TZ `CST-8`——UTC+8 本地时区未设"
        "（localtime 口径错或 UTC 直显, F4）");

    /* tooth 13 (K5-A4): 退避经 [S1] 缝真实消费——backoff 家族 >= 3 文件（h 声明 + c 定义
     * + 应用层 SNTP 侧真实调用点）。 */
    n = ts_scan_count("firmware/main", "kidpal_ts_backoff_delay_ms(", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        ts_fm("TS-A4 RED: kidpal_ts_backoff_delay_ms( 仅 %d 文件引用（期望 >= 3: "
              "声明 + 定义 + SNTP 侧真实调用）——退避决策表是孤岛, NTP 失败仍重试风暴",
              n));

    /* tooth 14 (K5-A4): W 日志节流真实落点——lvgl_display.cc（`System time is not set`
     * 现日志点）消费 log_throttled（h + c + lvgl = 3 文件家族下, 落点直钉）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_LVGL_CC, "kidpal_ts_log_throttled"),
        "TS-A4 RED: lvgl_display.cc 未消费 kidpal_ts_log_throttled——`System time "
        "is not set` W 日志仍每 10s 刷屏（节流未落到真实日志点）");
    n = ts_scan_count("firmware/main", "kidpal_ts_log_throttled(", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        ts_fm("TS-A4 RED: kidpal_ts_log_throttled( 仅 %d 文件引用（期望 >= 3: 声明 "
              "+ 定义 + lvgl 消费）——节流谓词是孤岛", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — K5-A2/K5-A3/K5-A3b 结构半：板级写回路径 + 断电恢复 + 周期重读重注入（F2）
 * RED today: 板文件无 WriteRtcTime / settimeofday / [S1] 缝消费; ReadRtcTime(& 恰 1
 * 调用点; inject_rtc( 恰 1 调用点（boot 快照在世 = F2 缺陷现状证据）。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A2 -- 板级 I2C 写路径 WriteRtcTime 在世 + BCD/可信门经 [S1] 缝真消费（旁路自写 BCD 红） */
/* coverage: K5-A3 -- boot 读 RTC 后 settimeofday 恢复路径在世（现状缺口销账） */
/* coverage: K5-A3b -- 周期重读重注入：ReadRtcTime 调用点 >= 2 + inject_rtc 调用点 >= 2（boot 一次快照退役） */
TEST_CASE("时间同步: 板级写回+断电恢复+周期重读重注入 (K5-A2/K5-A3/K5-A3b)",
          "[time_sync][K5-A2][K5-A3][K5-A3b][structural]")
{
    int n;

    TS_REQUIRE_ROOT();

    /* tooth 1 (K5-A2): I2C 写路径在世——WriteRtcTime（新写, 现状只读）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_BOARD_CC, "WriteRtcTime"),
        "TS-A2 RED: 板类无 WriteRtcTime——PCF85063 仍只读, SNTP 对好的时间无法写回"
        "硬件 RTC（断电续航无源）");

    /* tooth 2 (K5-A2): BCD 走 [S1] 缝单源（不得旁路自写一份）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_BOARD_CC, "kidpal_ts_bcd_encode"),
        "TS-A2 RED: 板级写回未消费 kidpal_ts_bcd_encode——BCD 编码旁路 [S1] 单源"
        "（行为等价假设破坏, host 行为表对真写路径失去约束力）");

    /* tooth 3 (K5-A2/A3): 写回与恢复过统一可信门（OS 位 / year plausibility）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_BOARD_CC, "kidpal_ts_write_decision"),
        "TS-A2/A3 RED: 板级未消费 kidpal_ts_write_decision——OS 位停振不写回/坏时间"
        "不恢复的可信门未接线（坏时间被当权威）");

    /* tooth 4 (K5-A3): boot 读 RTC 后 settimeofday 恢复系统时间（现状缺口 = 只注入 P1）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_BOARD_CC, "settimeofday"),
        "TS-A3 RED: 板级无 settimeofday——boot 读 RTC 仅注入 P1、系统时间不恢复"
        "（现状缺口在世: 断网重启后状态栏时钟仍 1970, S-F1.1 AC4 / NFR-OFF-1 失守）");

    /* tooth 5 (K5-A3b): 周期重读——ReadRtcTime 调用点 >= 2（boot 一次 + 周期; 现状恰 1）。 */
    n = ts_count_in_file(TS_BOARD_CC, "ReadRtcTime(&");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        ts_fm("TS-A3b RED: 板级 ReadRtcTime 调用点仅 %d 处（期望 >= 2: boot + 周期"
              "重读）——boot 快照冻结（F2）现状在世", n));

    /* tooth 6 (K5-A3b): 周期重注入——inject_rtc 调用点 >= 2（现状恰 1 = boot 一次）。 */
    n = ts_count_in_file(TS_BOARD_CC, "kidpal_render_node_inject_rtc(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        ts_fm("TS-A3b RED: 板级 kidpal_render_node_inject_rtc 调用点仅 %d 处"
              "（期望 >= 2: boot + 周期重注入）——P1 时间消费面仍吃 boot 快照（F2 "
              "冻结缺陷未修: 只 settimeofday 不 re-inject 修不了 P1, plan 边角）", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — K5-A5 守卫：host 注册接线 + inject 家族零偏移 + 回归注册面 + 扫描器自检。
 * MUST-STAY-GREEN 半：inject_rtc 函数体 / .h 声明 / 既有齿注册（现状即绿）;
 * RED until builder wires：time_sync.c / 本文件注册行。
 * ------------------------------------------------------------------------ */
/* coverage: K5-A5 -- host runner 注册守卫（本验收件+time_sync.c 进 HOST_ACCEPTANCE_SRCS 且 smoke 末位; p1_home_battery/schedule_grid 回归注册在世）+ inject 家族零偏移（inject_rtc/s_rtc_hour guard 齿只读消费在世） */
TEST_CASE("时间同步: 注册接线+inject家族零偏移+回归守卫 (K5-A5)",
          "[time_sync][K5-A5][boundary]")
{
    char *mk, *region;
    int n;

    TS_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): 既有回归齿注册在世——p1_home_battery / schedule_grid。 */
    mk = ts_read(TS_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "TS RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(mk, "p1_home_battery.acceptance.c") != NULL &&
                                 strstr(mk, "schedule_grid.acceptance.c") != NULL,
        "K5-A5 RED: p1_home_battery / schedule_grid 既有冻结齿注册行丢失——回归"
        "面被本包拆（K5-A5 既有齿零偏移义务）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(mk, "host_infra_smoke.acceptance.c") != NULL,
        "TS RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
        "丢失 — 包 0 守卫被移除");
    free(mk);

    /* tooth 2 (MUST-STAY-GREEN): inject_rtc 函数体在世——切区（inject_rtc →
     * inject_env）内含 s_rtc_hour / s_rtc_minute 写点（p1_home_battery guard 齿同锚）。 */
    region = ts_region_between(TS_RN_CC,
                               "void kidpal_render_node_inject_rtc(",
                               "void kidpal_render_node_inject_env(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "TS RED(infra): kidpal_render_node.cc 的 inject_rtc 函数区切取失败——"
        "inject 家族被改动（零偏移红线, 须复核锚）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "s_rtc_hour") != NULL &&
                                 strstr(region, "s_rtc_minute") != NULL,
        "K5-A5 RED: inject_rtc 函数体不再写 s_rtc_hour/s_rtc_minute——inject 家族"
        "语义被拆（(a) 路线 inject 家族保持在世义务失守, p1_home_battery guard 齿"
        "随之失锚）");
    free(region);

    /* tooth 3 (MUST-STAY-GREEN): .h 声明面在世。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_RN_H,
                                              "kidpal_render_node_inject_rtc"),
        "K5-A5 RED: kidpal_render_node.h inject_rtc 声明丢失——inject 家族契约面"
        "被拆");

    /* tooth 4 (MUST-STAY-GREEN): s_rtc_hour 家族计数零偏移（现状基线 5 处: 声明 +
     * inject 写点 + 3 组装点读——下界守卫, 缩水 = 组装点读或注入写点被拆）。 */
    n = ts_count_in_file(TS_RN_CC, "s_rtc_hour");
    TEST_ASSERT_TRUE_MESSAGE(n >= 5,
        ts_fm("K5-A5 RED: kidpal_render_node.cc s_rtc_hour 仅 %d 处（现状基线 5）——"
              "inject 家族消费链被拆（零偏移义务）", n));

    /* tooth 5: 注册接线——time_sync.c（强符号源）+ 本验收件进 HOST_ACCEPTANCE_SRCS,
     * 排在 smoke 之前。RED until builder wires（BUILDER DUTY 7)）。 */
    mk = ts_read(TS_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "TS RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *acc  = strstr(mk, "time_sync.acceptance.c");
        const char *impl = strstr(mk, "../../../time_sync.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(acc != NULL,
            "K5-A5 RED: time_sync.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — "
            "host 验收构建未接本包（BUILDER DUTY 7)）");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "K5-A5 RED: firmware/main/time_sync.c 未接线进 HOST_ACCEPTANCE_SRCS — "
            "行为齿强符号无处落地（weak 哨兵将持续红）");
        TEST_ASSERT_TRUE_MESSAGE(acc < smoke && impl < smoke,
            "K5-A5 RED: time_sync 注册行必须排在 host_infra_smoke.acceptance.c 之前"
            "（smoke 末位 = 最先执行, boundary 齿注册序守卫）");
    }
    free(mk);

    /* tooth 6: 扫描器自检（防假绿）。 */
    n = ts_count_in_file(TS_RN_CC, "kidpal_render_node_inject_weekday");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "TS RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — K5-R3a（R3 追加轮, human 拍板①, 授权链 pod-events human-decision
 * fc43063d）结构半：chrome 状态栏时钟隐藏退役 + 状态栏其余元素在世齿 + P1 辅助条
 * 唯一常显时间面在世（A1 锚重指承载）。
 * RED today: lvgl_display.cc UpdateStatusBar 区仍 SetStatus(time_str) 渲染时钟
 * （2026-09-22 实测区内恰 1 处调用）。
 * ------------------------------------------------------------------------ */
/* coverage: K5-R3a -- 状态栏时钟隐藏（human 追加拍板①）: UpdateStatusBar 区 SetStatus( 零调用（时钟渲染退役负锚）+ mute/网络/低电 popup 渲染路径在世（其余状态栏元素零偏移）+ P1 辅助条时间面在世（A1 重指锚承载） */
TEST_CASE("时间同步: 状态栏时钟隐藏+其余元素在世 (K5-R3a)",
          "[time_sync][K5-R3a][structural][r3]")
{
    char *region;
    int n;

    TS_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): UpdateStatusBar 函数区可切（锚在世）。 */
    region = ts_region_between(TS_LVGL_CC,
                               "void LvglDisplay::UpdateStatusBar(",
                               "void LvglDisplay::SetPreviewImage(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "TS-R3a RED(infra): UpdateStatusBar 函数区切取失败——chrome 状态栏渲染链被"
        "改动（锚漂移须复核; 本齿负锚范围 = 该函数区, 禁整函数退役绕过）");
    if (region == NULL) {
        return;
    }

    /* tooth 2 (K5-R3a 负锚, RED today): 区内零 SetStatus( 调用——chrome 状态栏时钟
     * HH:MM 渲染退役（区内现状恰 1 处 SetStatus(time_str); application.cc 的状态
     * 文案 SetStatus 不在本区、不受影响; 温度/电量不在 chrome 状态栏——P1 辅助条
     * 承载, 勿混, human 边界注记）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "SetStatus(") == NULL,
        "TS-R3a RED: UpdateStatusBar 区仍调 SetStatus(——chrome 状态栏时钟仍在渲染"
        "（human 拍板隐藏未落; 只许摘时钟渲染调用, 不许整函数退役——teeth 3-5 把住"
        "其余元素零偏移）");

    /* tooth 3 (MUST-STAY-GREEN): mute 图标更新路径在世。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "mute_label_") != NULL &&
                                 strstr(region, "FONT_AWESOME_VOLUME_XMARK") != NULL,
        "TS-R3a RED: UpdateStatusBar 区 mute 图标路径丢失——「隐藏时钟」被实现成"
        "状态栏元素连带拆除（其余状态栏元素零偏移义务）");

    /* tooth 4 (MUST-STAY-GREEN): 网络图标更新路径在世（wifi）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "GetNetworkStateIcon") != NULL &&
                                 strstr(region, "network_label_") != NULL,
        "TS-R3a RED: UpdateStatusBar 区网络图标路径丢失——状态栏 wifi 元素被连带"
        "拆除（其余状态栏元素零偏移义务）");

    /* tooth 5 (MUST-STAY-GREEN): 低电 popup 判定链路在世（电池链路零触碰锚; chrome
     * 电池段本身已由 N1b 摘除, 本齿钉现状形态零偏移）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "low_battery_popup_") != NULL,
        "TS-R3a RED: UpdateStatusBar 区低电 popup 判定链路丢失——电池链路被触碰"
        "（本包电量链路/帧缓冲零触碰红线）");
    free(region);

    /* tooth 6 (MUST-STAY-GREEN, A1 锚重指承载): P1 辅助条时间面在世——p1_draw_aux_time
     * 定义 + 装配调用 >= 2（重指后的唯一常显时间面承载; 既有在世标识符 needle, 对
     * home_p1.c 零新增注释要求, canary `upcoming` 子串零引入）。 */
    n = ts_count_in_file(TS_HOME_P1_C, "p1_draw_aux_time");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        ts_fm("TS-R3a RED: home_p1.c p1_draw_aux_time 仅 %d 处（期望 >= 2: 定义 + "
              "装配调用）——P1 辅助条时间面（唯一常显时间面）被拆, A1 锚重指落空", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — K5-R3b（R3 追加轮, human 拍板②, 同授权链）：re-inject 读点分钟对齐。
 * 行为半 = [S6] 第七函数决策表（host 可判）; 结构半 = 板级节拍消费接线 + boot 相位
 * 固定节拍退役。RED today: kidpal_ts_ms_to_next_minute 全仓零命中（weak 哨兵）;
 * 板文件仍 vTaskDelay(pdMS_TO_TICKS(KIDPAL_TS_PERIODIC_PERIOD_MS)) 挂 boot 相位
 * （真机实测读点 :44 秒, P1 滞后最大 59s）。
 * ------------------------------------------------------------------------ */
/* coverage: K5-R3b -- re-inject 分钟对齐（human 追加拍板②）: kidpal_ts_ms_to_next_minute 决策表（恒 [1,60000] + 分钟边界回卷 + 负入参防御）+ 板级 TimeSyncPeriodicLoop 消费接线 + boot 相位固定节拍退役（读点秒≈0、P1 滞后 <1s 的 host 可证部分） */
TEST_CASE("时间同步: re-inject分钟对齐决策表+板级相位接线 (K5-R3b)",
          "[time_sync][K5-R3b][behavior][structural][r3]")
{
    static const struct { long long in; long long out; } tbl[] = {
        {      0LL, 60000LL}, {      1LL, 59999LL}, {  44000LL, 16000LL},
        {  59999LL,     1LL}, {  60000LL, 60000LL}, {  60001LL, 59999LL},
        { 119999LL,     1LL}, { 120000LL, 60000LL},
    };
    size_t i;
    long long ms, r;
    int n;

    /* tooth 0: 哨兵判读（[S6] 第七函数 frozen seam 在世义务）。 */
    ts_require_implemented((int)kidpal_ts_ms_to_next_minute(44000LL),
                           "kidpal_ts_ms_to_next_minute");

    /* tooth 1: 决策表——分钟边界对齐（读点 = 按返回值 sleep 后立即读再注入, 滞后 =
     * 唤醒抖动 < 1s; 恰在边界 -> 整周期 60000, 禁 0 延迟忙旋）。 */
    for (i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++) {
        r = kidpal_ts_ms_to_next_minute(tbl[i].in);
        TEST_ASSERT_TRUE_MESSAGE(r == tbl[i].out,
            ts_fm("TS-R3b RED: ms_to_next_minute(%lld)=%lld 期望 %lld——分钟边界"
                  "对齐语义漂移（读点仍挂 boot 相位, P1 显示滞后最大 59s）",
                  tbl[i].in, r, tbl[i].out));
    }

    /* tooth 2: 负入参防御。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_ts_ms_to_next_minute(-1LL) == -1LL,
        "TS-R3b RED: ms_to_next_minute(-1) 未防御（-1）——非法时钟被接受");

    /* tooth 3: 值域不变量（扫描域内恒 [1,60000] 且补满当前分钟相位）。 */
    for (ms = 0LL; ms <= 180000LL; ms += 997LL) {
        r = kidpal_ts_ms_to_next_minute(ms);
        TEST_ASSERT_TRUE_MESSAGE(r >= 1LL && r <= 60000LL &&
                                     r + (ms % 60000LL) == 60000LL,
            ts_fm("TS-R3b RED: ms_to_next_minute(%lld)=%lld 越域/相位失配——返回值"
                  "须恒在 [1,60000] 且与入参相位补满 60000（分钟边界对齐不变量）",
                  ms, r));
    }

    TS_REQUIRE_ROOT();

    /* tooth 4 (structural): 板级节拍消费——TimeSyncPeriodicLoop 相位由 [S6] 缝驱动。 */
    n = ts_count_in_file(TS_BOARD_CC, "kidpal_ts_ms_to_next_minute(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        ts_fm("TS-R3b RED: 板级 kidpal_ts_ms_to_next_minute( 仅 %d 处——"
              "TimeSyncPeriodicLoop 节拍相位未接 [S6] 分钟对齐缝（boot 相位在世, "
              "真机读点 :44 秒）", n));

    /* tooth 5 (structural, boundary): boot 相位固定节拍退役——精确串零命中（现状恰
     * 1 处; 相位须按分钟边界计算, 不许保留固定 60000ms 挂 boot 相位形态）。 */
    n = ts_count_in_file(TS_BOARD_CC,
                         "vTaskDelay(pdMS_TO_TICKS(KIDPAL_TS_PERIODIC_PERIOD_MS))");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        ts_fm("TS-R3b RED: 板级固定节拍 vTaskDelay(pdMS_TO_TICKS(KIDPAL_TS_"
              "PERIODIC_PERIOD_MS)) 仍 %d 处——读点相位仍挂 boot（分钟对齐未落, "
              "P1 滞后最大 59s）", n));

    /* tooth 6: [S1]/[S6] 头文件声明面在世（第七函数契约）。 */
    TEST_ASSERT_TRUE_MESSAGE(ts_file_contains(TS_TIME_SYNC_H,
                                              "kidpal_ts_ms_to_next_minute"),
        "TS-R3b RED: time_sync.h 无 kidpal_ts_ms_to_next_minute 声明——[S6] 第七"
        "函数契约未落（行为表与设备消费无处锚定）");
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-22 实跑）:
 *   通道① 既有 host runner 全量实跑（本文件未接线前, 既有齿零偏移通道）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *       key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *       layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *     判据 = 上游锚 84 Tests / 0 Failures（@ 9cae452, K5 launch 段登记）零偏移。
 *   通道② 本文件新齿红通道（C 类冻结验收实编 + 实跑, DEV-GOVERNANCE 验收测试纪律 §4/§3）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -I. -o /tmp/ts_red_runner ../../time_sync.acceptance.c runner.c
 *     /tmp/ts_red_runner  （exit code = 失败 TEST_CASE 数, 期望 = 4 红 / 2 绿）
 *     编译通道 error 为零（验收文件本身可编译）; 实跑红定性:
 *       - TEST 1/2/3 红 = [S1] 六函数 weak 哨兵（frozen seam not implemented, 真实缺失）;
 *       - TEST 4 绿半 = tooth 1-5（CheckVersion 零调用 / ota.cc server_time 死代码留置 /
 *         lvgl 守门行 / 需求契约 canary / 扫描器自检, MUST-STAY-GREEN 现状即绿, 基线
 *         实跑已证）; TEST 4 红半 = tooth 6 起（time_sync.h 不存在 + application.cc
 *         零 sntp/ntp.aliyun.com/CST-8/backoff 消费 + lvgl 零 log_throttled 消费 +
 *         sdkconfig.defaults 无 CONFIG_LWIP_SNTP_MAX_SERVERS, 2026-09-22 grep 逐
 *         needle 实测零命中）;
 *       - TEST 5 红 = 板文件无 WriteRtcTime/settimeofday/[S1] 消费, ReadRtcTime(& 恰 1
 *         调用点, inject_rtc( 恰 1 调用点（F2 boot 快照在世证据）;
 *       - TEST 6 绿半 = tooth 1-4（既有齿注册在世 + inject 家族零偏移, MUST-STAY-GREEN,
 *         基线实跑已证）; 红半 = tooth 5（time_sync.acceptance.c / time_sync.c 注册行
 *         缺失, builder 接线后转绿）。
 *   MUST-STAY-GREEN at baseline: TEST 4 tooth 9/10（ota.cc server_time 死代码留置 +
 *     lvgl 守门行在世）、TEST 6 tooth 2/3/4/5（inject 家族零偏移）与 TEST 6 的既有齿
 *     注册半、扫描器自检全齿。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）。
 *
 *   R3 追记（2026-09-22 追加轮冻齿修订红基线, acceptance-author 实跑; 授权链 =
 *   pod-events human-decision fc43063d）:
 *   通道① 独立 gcc 线束（修订齿设计态红 / 未动面绿, 分离证明）:
 *     a) 哨兵形态（不链实现, 上棒同款）:
 *        cd firmware/main/test/acceptance/red-baseline
 *        gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r3 ../../time_sync.acceptance.c runner.c
 *        /tmp/ts_red_r3
 *        实跑 = 8 CASE / 5 红: TEST 1/2/3 红（[S1] 六函数 weak 哨兵——线束未链
 *        time_sync.c 的已知判读态, 非本包修订面）; TEST 4/5/6 绿（主轮实现已落地 +
 *        未动面零偏移, 结构齿扫真实工作树）; TEST 7 红（tooth 2: UpdateStatusBar
 *        区 SetStatus( 仍 1 处 = 时钟渲染未摘）; TEST 8 红（tooth 0: [S6] 第七函数
 *        weak 哨兵; teeth 4/5 结构性必红被 Unity abort 屏蔽, 解析性必红）。
 *     b) 强符号形态（链真实 time_sync.c, 既有行为齿零偏移证明）:
 *        gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r3b \
 *            ../../time_sync.acceptance.c ../../time_sync.c runner.c
 *        /tmp/ts_red_r3b
 *        实跑 = TEST 1-6 全绿（既有行为表/结构齿零偏移）; TEST 7 红（同 a）;
 *        TEST 8 红（tooth 0 哨兵——[S6] 第七函数尚未实现, 设计态）。
 *   通道② host runner 全量（修订齿已随本文件注册自动携带, CMakeLists 零改）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *       key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *     cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *       ./build/host_infra_test.elf
 *     期望（builder 未落 R3-1/R3-2 时）: 92 Tests / 2 Failures, 红全部 = 本文件
 *     TEST 7/8 设计态红（在册于通道①判读）; 既有面（主轮 90 齿, 尤其 TEST 1/2/3
 *     行为齿与 TEST 6 既有守卫）零偏移全绿。
 *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-22  initial freeze (acceptance-author, TaskPackage
 *               260922-m3-k5-sntp-time-sync)。六 TEST 钉 K5-A1/A2/A3/A3b/A4/A5 host
 *               可判半; 真机半（A1 tm_year:70 清零 + A3b 长开机 + A3 断电续航 + A4
 *               真机降级）deferred 设备轮窗口（头注 DEFERRED 清单, 不伪造 host 证据）。
 *   2026-09-22  R3 追加轮冻齿修订（human 两项追加拍板; 授权链 = pod-events
 *               human-decision fc43063d, 原始拍板 = human 2026-09-22 真机第 1 轮
 *               目测后口头裁定; 主轮 e5b248a 已全绿: A1 正锚实证过 / K4 五项目测过 /
 *               wday 链路无缺陷）——纯 additive 补齿 + 锚重指, 零放宽零退役既有齿:
 *               ① A1 锚重指: 「HH:MM 渲染正确」真机锚从 chrome 状态栏重指 P1 辅助条
 *               （时间/日期; home_p1.c p1_draw_aux_time 承载, test 7 tooth 6 在世齿）。
 *               ② 新增 TEST 7（K5-R3a 负锚）: UpdateStatusBar 区 SetStatus( 零调用 =
 *               chrome 状态栏时钟渲染退役（区内现状恰 1 处 SetStatus(time_str), 红基线
 *               实红）; 在世齿 teeth 3-5 钉 mute/网络/低电 popup 渲染路径零偏移——防
 *               「隐藏时钟」实现成「整个状态栏消失」; tooth 6 钉 P1 辅助条时间面在世。
 *               温度（P1 辅助条「28」）/电量/帧缓冲零触碰边界由既有齿与在世齿把住。
 *               ③ 守门裁定（保留不退役）: lvgl_display.cc:130 `tm_year >= 2025 - 1900`
 *               守门行保留——P1 辅助条时间消费面不经该守门（P1 时间 = 板级 RTC 读 →
 *               inject 家族 → hp1_state, 无年份门; 该守门只辖 chrome SetStatus 渲染）,
 *               时钟渲染退役后该门仍辖「时间未同步」节流 W 日志分支（非死代码——test 4
 *               teeth 3/14 既有 MUST-STAY-GREEN 齿零偏移即其负锚语义落点）; 故裁定
 *               保留, 只摘 then 分支的 SetStatus(time_str) 渲染调用（R3-1 duty）。
 *               ④ 新增 TEST 8（K5-R3b）: [S6] 第七函数 kidpal_ts_ms_to_next_minute
 *               决策表 + 值域不变量（行为半, host 可判——滞后 <1s 的可证部分落到纯
 *               函数行为表）+ 板级 TimeSyncPeriodicLoop 消费接线 + boot 相位固定
 *               60000ms 节拍退役（结构半）; inject 家族语义零触碰（test 5/6 既有齿
 *               不变, 只调节拍相位不动 inject）。
 *               ⑤ 零偏移面: TEST 1-6 全部既有 tooth 零改动（尤其 test 4 tooth 3/14
 *               守门与节流消费、test 5/6 inject 家族守卫、test 6 注册守卫、OTA 死代码
 *               守卫）; CMakeLists 零改（本文件已注册 HOST_ACCEPTANCE_SRCS, 修订齿
 *               随文件自动携带进 host runner）。
 *               ⑥ canary 纪律: 新增齿 needle 均为在世标识符（SetStatus( / mute_label_ /
 *               GetNetworkStateIcon / low_battery_popup_ / p1_draw_aux_time /
 *               kidpal_ts_ms_to_next_minute / KIDPAL_TS_PERIODIC_PERIOD_MS 精确串）,
 *               对 home_p1.c 零新增注释要求, `upcoming` 字面子串零引入。红基线见
 *               头注「R3 追记」。
 * --------------------------------------------------------------------------- */
