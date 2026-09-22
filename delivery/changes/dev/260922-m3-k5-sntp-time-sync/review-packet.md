# Review Packet · 260922-m3-k5-sntp-time-sync

> workstream m3-schedule ｜ M3-K5 设备时间同步（SNTP 校时 + PCF85063 写回断电续航 + P1 周期 re-inject 修 F2 boot 快照冻结）｜ mergeOrder 37（planner 派单口径；taskpackage.json 字段暂为 null，本包为树池串行在办唯一包）｜ rigor lite ｜ riskTier medium ｜ subsystem firmware ｜ gateProfile [test-quality] ｜ origin.type human-confirmed-plan（triage ②单包 Lite，human 拍板 F1=(a) + F2 随包）
> integrator 收拢制品（2026-09-22）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。dependsOn []（K4 已 merged，常驻分支 dev/260915-m1-k1a-key-layer-dispatch 续棒）。

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit）**：15 文件（代码 12 + delivery 制品 3 类）。
  - 新增：`firmware/main/time_sync.h` / `time_sync.c`（seam [S1] 纯逻辑时间缝六函数：bcd_encode / bcd_decode / year_ok / write_decision / backoff_delay_ms / log_throttled，零芯片依赖 host-device 同源）；`firmware/main/test/time_sync.acceptance.c`（6 TEST_CASE 冻结验收件，1031 行）。
  - 修改：`firmware/main/CMakeLists.txt`（+4：time_sync.c 注册）；`firmware/main/test/host_app/main/CMakeLists.txt`（+11：HOST_ACCEPTANCE_SRCS 注册 time_sync.c + time_sync.acceptance.c，置于 host smoke 之前——包 0 boundary 齿注册序守卫约束）；`firmware/main/application.cc`（+79：[S2] SNTP 挂点——HandleNetworkConnectedEvent 后异步起 esp_sntp_init，TZ `CST-8`，三 server（ntp.aliyun.com 主 + ntp1.aliyun.com + pool.ntp.org 备），独立低优先级监测 task 按 [S1] 退避轮询，幂等防重连重启）；`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+187/-6：[S3] ReadRtcTime 增 os_bit 出参 + 新增 WriteRtcTime I2C 写路径（BCD 写 0x04–0x09，双可信门前置）+ SyncSystemTimeToRtc（SNTP 同步旗标→写回）+ boot settimeofday 恢复（year plausibility 代理 + write_decision 门）+ [S4] ReinjectRtcTime 60s 周期重读重注入经 Application::Schedule 派发主循环）；`firmware/main/display/lvgl_display/lvgl_display.cc`（+12/-1：W 日志 `System time is not set` 经 [S1] log_throttled 节流 60s，守门行与渲染链路零触碰）；`firmware/sdkconfig`（LWIP_SNTP_MAX_SERVERS 1→3）+ `firmware/sdkconfig.defaults`（+4 同步生成面前提）。
  - delivery：taskpackage.json（本包 finalize + current/ 指针）、input-brief 双件、pod-events.jsonl（4 事件）、verify-runs.jsonl（2 轮）、verifier-evidence/rounds/{r1,r2}.json。
- **Net diff**：391 insertions / 20 deletions（代码面；delivery 面另计）。
- **Scope**：firmware 时间内聚簇（SNTP 挂点 + PCF85063 读写 + render_node inject 消费侧接线 + 状态栏守门节流）——单子系统纯固件，crossEnd exempt（无跨包行为对偶面）。
- **Split recommendation**：单包，不拆。

## Verification Evidence

- **Structural gates**：final r2 九闸全绿（`delivery/taskpackages/260922-m3-k5-sntp-time-sync/verifier-evidence/rounds/r2.json`，phase final / round 2 / invocationId `eca0824f-5f85-4b2a-88a9-abc4052fac33`，ok:true / observedGateExit:0）：taskpackage-required、firmware-build（xiaozhi.bin 0x30e0c0 bytes，app 分区 0x370000，0x61f40 bytes = **11% free** 体积余量在案）、contract-lint、acceptance-coverage、mutation-check、docs-sync、security、unit（95 passed）、acceptance（41 structural 齿 passed）。scoped r1 六闸全绿（r1.json，invocationId `0c3b1298-79ea-4948-8b21-665c580bc328`，ok:true）。
- **机械 fact 校验**（integrator 实跑 2026-09-22）：`verifier-evidence-check.mjs --change-id 260922-m3-k5-sntp-time-sync --phase final --round 2 --invocation-id eca0824f-5f85-4b2a-88a9-abc4052fac33` → exit 0 / factOk:true；补跑 `--phase scoped --round 1 --invocation-id 0c3b1298-79ea-4948-8b21-665c580bc328` → exit 0 / factOk:true（同 K4 先例双查口径）。
- **Acceptance/unit/e2e**：host runner **90 Tests / 0 Failures / exit 0**（既有 84 齿零偏移——与上游锚 84 @ 9cae452 对齐 + 本包新齿 6 TEST_CASE）；device build 闸过（真编译，见上体积证据）；真机 e2e 半 **deferred**（见「未覆盖面申报」）。
- **Security gate**：r2 绿（securityReviewRequired:false）。新增 I2C 写路径为本机板载 PCF85063（地址 0x51），无网络输入面；SNTP server 为硬编码域名常量，无注入面；settimeofday 输入经 write_decision + 日期 plausibility 门。
- **Mutation check**：mutationRequired:true，六条 acceptance 全部 sensitive:true 登记于 taskpackage.json `mutationEvidence.perAcceptance`（冻结登记面）。**builder 实跑自证（交付报告原样转录，planner 派单 2026-09-22 逐字转交）**：5 组变异实跑——① `kidpal_ts_year_ok` 阈值 125→124（守门阈值敏感）；② write_decision 移除 OS 位分支（可信门降级敏感）；③ `kidpal_ts_bcd_encode` 改 `hi*10+lo`（BCD 编码敏感）；④ backoff 去封顶（重试风暴防护敏感）；⑤ `kidpal_ts_log_throttled` 边界 `<`→`<=`（节流边界敏感）——**每组变异后宿主验收 runner exit=1（至少一齿转红），未变异基线 exit=0**，证明齿对被测语义敏感、非恒绿。红基线通道②在案独立红（6 Tests / 6 Failures / exit 6，红全部为设计态义务红；MUST-STAY-GREEN 半 TEST 4 tooth 1-5 / TEST 6 tooth 1-4 基线即绿）。
- **Docs sync**：docsSyncRequired=false，r1/r2 闸绿；详见本目录 docs-delta.md。
- **S2 official exploratory scan**：TaskPackage 未声明 `s2ExploratoryRequired`，不执行，无 S2 证据义务。
- **UI design-review 证据**：不适用（本包无新 UI 面——状态栏 HH:MM 渲染与 P1 版式均为既有面，本包只喂对数据源；渲染链路/版式零触碰）。

## Demo 对账行（workstream 包硬要求）

本包 demo 证据形态 = **host runner 90 齿全绿 + device build 体积证据（xiaozhi.bin 0x30e0c0 / 11% free）+ 真机锚挂设备轮窗口（如实申报：真机半未跑、挂窗，非本相位义务）**。与本包效果段「设备全部常显时间面显示真实本地时间」的一致性结论：host 侧对效果段的可机判半（BCD 编解码、写回可信门决策、boot 恢复路径、周期 re-inject 结构接线、SNTP 挂点接线、守门阈值语义）已全齿覆盖并绿；device build 证明确可编入真机镜像且无资源回退；效果段的**实证半**（状态栏/P1 真实渲染出正确本地时间、tm_year:70 日志清零、长开机不冻结、断电续航、NTP 不可达降级表现）依赖真机观察，已在 exitCriteria K5-E3 显式登记挂设备轮窗口（单一长捕获 + 就绪信号开窗），**收口前 demo 对账不闭环**——这是计划内 deferred，不是证据缺失。

## 未覆盖面申报（workstream 包硬要求）

对照 acceptance[] 六条 + contractRefs + plan.md 效果段逐条：

| 项 | 设计性质 | 现状 | 去向 |
|---|---|---|---|
| K5-A1 真机半（SNTP 后状态栏/P1 时间正确 + `tm_year: 70` 日志清零负锚） | 真机观察锚，host 不可机判（host 半：挂点接线/守门阈值/TZ 语义齿已绿） | **未跑，挂设备轮窗口** | 设备轮收口（exitCriteria K5-E3，单一长捕获 + 就绪信号开窗） |
| K5-A3b 真机半（长开机 ≥N 分钟 P1 时间继续走、非 boot 快照冻结） | 真机时间流逝观察锚（host 半：周期重读重注入结构齿已绿） | **未跑，挂设备轮窗口** | 同上 |
| K5-A3 真机半（断电重启 + 断网 → RTC 恢复显示正确时间） | 真机断电操作锚（host 半：恢复路径 + write_decision 可信门齿已绿） | **未跑，挂设备轮窗口** | 同上 |
| K5-A4 真机半（NTP 不可达降级表现真机实证） | 真机网络置错锚（host 半：退避决策表 + 节流决策表齿已绿，降级语义 host 可证——本项 host 证据已在案） | **真机复证未跑，挂设备轮窗口**（exitCriteria 明载「A4 真机或 host 可证降级语义」——host 半已闭环） | 设备轮窗口内补真机复证 |
| 契约引用（contractRefs: interface-contract.md） | 时间同步不在任何契约内（architect F7 核查） | **无缺口（none）**——零触碰 + contract-lint 两轮绿 + 冻结齿 canary | 无 |
| plan.md 效果段其余面 | 状态栏 HH:MM 渲染、P1 辅助条版式、render_node inject 家族 | **无缺口（none）**——渲染链路零触碰、inject 家族零偏移（冻结齿 TEST 6 tooth 1-4 MUST-STAY-GREEN 全绿） | 无 |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （全仓 `minimal:` 标记 grep 仅 1 处命中：`waveshare-s3-rlcd-4.2.cc:413` boot 恢复 year_ok 入参 = 日期 plausibility 代理——见下方说明行） | — | — | — |

- `waveshare-s3-rlcd-4.2.cc:413`（minimal 标记）：boot 恢复侧 `year_ok` 入参用 RTC 日期字段 plausibility（mon 1-12 / mday 1-31 / 时分秒域）作代理，因 PCF85063 无年寄存器、真 year_ok 只在写回侧有系统时间可比——**代理假设**：芯片只被本机 SNTP 写回路径写过（写回侧已过真 year_ok 门），工厂裸片全 0 寄存器（mday=0）被 plausibility 拒、不画假时间。Ceiling：无（硬件无年寄存器，代理是该约束下的最小可信门）。Upgrade trigger：no-trigger（若未来出现非本机写回源写入 RTC，需换世纪位/魔数方案——当前单写源设计下不成立）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD / NIT） | — | — | — | — | — |

Findings 说明（五轴 review 执行留痕，见下节；无 finding 不等于未执行——逐轴结论与核对面如下）。

## Five-Axis Review

1. **Correctness（对每条 Sx-Ay 的边界核对）**：K5-A1——SNTP 挂 `HandleNetworkConnectedEvent` 后异步（esp_sntp_init 自身异步 + 监测独立 task 优先级 1），幂等旗标 `s_sntp_started` 防重连重启；守门阈值 125 与 lvgl_display.cc:134 同源单值（[S1] kidpal_ts_year_ok），TZ `CST-8` 符号反转正确（POSIX 西正东负）。K5-A2——写回 0x04–0x09 单事务自动递增，BCD 唯一走 [S1] 单源（旁路自写会被冻结齿 TEST 5 tooth 2 红）；秒/分寄存器 bit7（CV/OS）随写入值天然 <60 <32 清零；OS 停振不写回等 SNTP。K5-A3——boot 恢复 settimeofday 带 2025 年基线（tm_year=125 过守门），`kidpal_ts_write_decision(os_bit, plausible)` 门 + 系统时钟已 plausible（软重启）跳过恢复不回退。K5-A3b——60s 周期重读重注入经 `Application::Schedule` 派发主循环 task，与 boot 注入同任务上下文无渲染态跨任务竞争；60s 与 P1 分钟粒度协调不跳字。K5-A4——退避 1s→32s 封顶（`1000 << f`，f 封顶 5）+ 监测只观测不重启 SNTP（内部重试恒开），W 日志 60s 节流。K5-A5——90 齿零偏移 + 体积余量在案。边界核对中 builder 两点实现化申报已复核：① 写回形态 = SNTP 回调置 volatile 旗标 → 60s task 消费写回（I/O 移出 lwip 回调上下文，plan「事件驱动写回」的合理实现化——回调只置 `s_ts_sntp_synced=1`，无 I2C 调用，✓）；② boot 恢复侧 year_ok 入参 = 日期 plausibility 代理（PCF85063 无年寄存器，写回侧为真 year_ok；代理已入 Simplification Debt 台账并留 upgrade trigger，✓）。
2. **Test quality**：6 TEST_CASE 三层——TEST 1/2/3 纯逻辑决策表（BCD 编解码、守门/可信门、退避/节流，含相等/回拨/越界边界）；TEST 4 结构扫齿（挂点/server 前提/OTA 死代码守卫/缝消费接线 + 需求契约 canary + 扫描器自检防假绿 tooth 5）；TEST 5/6 板级接线扫齿（写回/恢复/周期性）+ 注册序守卫（强制置于 host smoke 之前）。负向齿自证能红（tooth 5 扫描器自检 + 红基线 6 红 + 变异 5 组每组 exit=1）。断言真实行为而非实现镜像（决策表断言语义输出值，结构齿断言源文件字面锚）。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（git status 实证 + contract-lint 两轮绿 + TEST 4 tooth 4 canary）；spec 零改；协议/server/front 零改（`git diff --name-only | grep server/` 零命中）；ota.cc server_time 死代码保持（ota.cc 零 diff + 冻结齿 TEST 4 tooth 2 字面守卫）；渲染版式/帧缓冲链路零触碰（lvgl_display.cc diff 仅节流 W 日志，守门行/渲染路径零触碰 + TEST 4 tooth 3 守卫）；inject 家族零偏移（kidpal_render_node.cc / home_p1.c 零 diff，`p1_home_battery.acceptance.c` / `schedule_grid.acceptance.c` 零 diff，TEST 6 tooth 1-4 在世守卫全绿）。
4. **Operability/security**：corr_id 观测链沿用 `kdp-%u`（esp_log_timestamp 低 24 位）——弱 uniqueness 但为既有全固件统一口径，非本包引入，不改（一致优先）；退避降噪（failures≤3 或 %10==0 续报）观测不失明；I2C 写失败 RecoverI2cBus 不污染 codec 事务（沿用既有模式）；新增 I2C 写路径无网络输入面、settimeofday 有可信门；ESP_LOG 无 secret。
5. **Maintainability/simplicity**：[S1] 六函数收进单源 C 缝（host/device 同源，零芯片依赖），消费方三处只接线不复制逻辑——deletion test：删掉 time_sync.c，BCD/门/退避/节流语义会泄漏散布到三个消费文件 = 深模组信号（留）。无死代码、无投机参数化、无重复实现；步进 task/回调结构均为最小形状。simplicity tags：无命中。

## E2 铁律逐项复核（K5-E2）

| 铁律项 | 证据 |
|---|---|
| `docs/contracts/**` 零改 | `git status --porcelain docs/contracts` 零输出；contract-lint r1/r2 绿；TEST 4 tooth 4 契约面在世 canary PASS |
| spec 零改 | `git status --porcelain docs/requirements.md docs/architecture.md` 零输出；TEST 4 tooth 4 `docs/requirements.md` 含 `S-F1.1` canary PASS |
| ota.cc server_time 死代码不复活 | `git diff --stat -- firmware/main/ota.cc` 零输出（零 diff）；application.cc 新增段零 `CheckVersion` 调用（冻结齿 TEST 4 tooth 1 源扫 `CheckVersion(` 命中数守卫 PASS）；ota.cc:186-204 server_time 解析路径保持现状死代码 |
| 电池链路零触碰（owner 边界） | 板文件 diff 中 `+` 行 grep `battery\|adc_\|GetBatteryLevel` **0 命中**（电池/ADC 相关全部为上下文行）；adc_battery_monitor 链路文件零 diff |
| 渲染版式/帧缓冲链路零触碰 | lvgl_display.cc diff 仅 +12/-1（W 日志节流消费），守门行 `tm_year >= 2025 - 1900` 原样在世；home_p1.c / kidpal_render_node.cc / custom_lcd_display.cc 零 diff；TEST 4 tooth 3 守卫 PASS |
| inject 家族零偏移 | `inject_rtc` / `s_rtc_hour` 家族函数体与声明面零 diff（本包仅新增调用点 `kidpal_render_node_inject_rtc(...)` 于新函数 ReinjectRtcTime）；TEST 6 tooth 1-4（既有齿注册 / inject_rtc 函数体 / .h 声明 / s_rtc_hour 家族计数基线 5 处）MUST-STAY-GREEN 全绿 |
| remind 链零改 | 提醒链文件零 diff（变更面全量见 Change Size，无 remind 路径文件） |

## Detect Changes / 影响面核对

- detect_changes（codebase-memory `KidPal`）：**N/A (worktree 未提交 diff)**——工具对 committed range 生效（base main 对比仅返回 `.codebase-memory/artifact.json` + `graph.db.zst` 快照自再生 2 文件、seed_symbols 0 / impacted 0），本包改动全部为未提交工作树 diff，工具不可见。替代核对（如实记录）：① `git diff --stat` 全量在案（391+/20-，15 文件，见 Change Size）；② 关键共享符号 trace_path 抽查——`Application::HandleNetworkConnectedEvent`（本包改动点）inbound 调用者恒 2（`Application::Run` / `app_main`），无预期外调用面；③ 变更面文件级核对——新增文件全部为本包新缝/新齿，修改文件全部在派单变更面清单内，无清单外文件。图索引 re-index 需求（新符号 time_sync.* 未入图）交主线程/human 定窗口，不阻塞本包。

## Residual Risk

- Known limitation：
  - 真机四锚（A1/A3b/A3 真机半 + A4 真机复证）deferred 挂设备轮窗口——host 证据不构成真机实证，设备轮收口前不宣称效果段实证闭环。
  - boot 恢复年份基线固定 2025（tm_year=125）：RTC 无年寄存器，年份显示面不消费（只消费 HH:MM / DD-Mon）；跨年后若设备长期断电断网（>1 年不联网），恢复年份停在 2025——但联网后 SNTP 即覆盖，状态栏守门同值通过。单写源设计下无更好方案，已入 Simplification Debt 台账。
  - corr_id 用 esp_log_timestamp 低 24 位，非全局唯一——既有口径，观测聚合时需结合时间戳前缀。
- Follow-up recommended：
  - 设备轮真机捕获时按 K5-E3 清单收口（A1 负锚日志清零 + A3b 长捕获 + A3 断电循环 + A4 域名置错），落 progress.md K5 段落账。
  - 合并后可在设备轮顺手复核 `CONFIG_LWIP_SNTP_MAX_SERVERS=3` 生成面与 defaults 一致性（本包 sdkconfig 与 defaults 双写，防 regen 漂移——现行双写为仓内惯例）。
- Human decision needed：无（真机窗口由 human 排期属既有设备轮协议，非新决策点）。

## 回滚方式

单 commit 收口（SHA 见交付报告）；回滚 = 分支上 `git revert <sha>`（单 commit 自含实现 + 制品，无跨 commit 依赖）；上游锚 84 齿 @ 9cae452 可独立复跑比对零偏移。
