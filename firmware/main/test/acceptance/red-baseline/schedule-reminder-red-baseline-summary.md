# 260829-schedule-reminder 红基线判定摘要（acceptance-author 2026-09-08 实测）

> **归位注记**：本文按机械 scope 边界落在 acceptance-author 可写面内
> （`firmware/**/acceptance/**`）。planner 要求的形态是
> `.claude/taskpackages/260829-schedule-reminder/red-baseline-summary.md`
> ——请原样 `git mv` 归位（内容与此路径要求完全对齐，无需改写）。
> 包 260829-schedule-p2 的同类摘要当初也落在本目录并留了同款注记。

TaskPackage `260829-schedule-reminder`（日程提醒全链路 · 包 7 · rigor=full / riskTier=high /
subsystem=firmware+server）冻结验收两文件的红基线执行记录：

- `firmware/main/test/schedule_reminder_b.acceptance.c`（设备面：三件套弹窗状态机 / 排队逐个弹 /
  V18 滞留顶替 / 四态与幂等去重 / 渲染才允许回执决策表 / 易失 known-id 只驻 RAM / KEY-BOOT 手势接缝）
- `server/acceptance/test_schedule_reminder.acceptance.py`（服务端：三档算术 / 幂等 id /
  pending.json 原子写 / 每分钟轮询到点 / 重试到确认 60s×3 / 过期静默作废 / 语音注入指令与降级 /
  JSON-RPC id 数字化 / 回执接出队）

## 0. 路径校正（请 planner 在 Finalize 回改 TaskPackage）

`taskpackage.acceptanceTestsFile[1]` 现写 `server/tests/test_reminder_push.acceptance.py`，
仓内实况目录是 `server/acceptance/`（包 6 / G 包 / g-relay-fix 全在此；两点文件名 +
`--import-mode=importlib` 已是既定通路）。本摘要按实况落盘：

```
"acceptanceTestsFile": [
  "firmware/main/test/schedule_reminder_b.acceptance.c",
  "server/acceptance/test_schedule_reminder.acceptance.py"
]
```

机械佐证：用原 TaskPackage 跑 `node .claude/scripts/acceptance-coverage-check.mjs` →
`ok:false / missingFiles: ["server/tests/test_reminder_push.acceptance.py"]`（C 侧文件已被找到）；
把上面两条路径喂给同一闸（临时 TaskPackage 副本，未改仓内件）→ `ok:true`，
`structural:["RMD-A1"]`，RMD-A2~A8 全覆盖标记齐、`hasRealAssertion` 两档均过。

## 1. 承重判读口径一：闸内 executor-skipped，真跑靠点名命令（planner 260908 更正已核实）

`acceptance-run-check.mjs` / `unit-run-check.mjs` 的过滤器判据是「文件内出现任一 live 标记即
整档排除出闸内 auto-run」。本 python 档含 live 齿，故两闸实测：

```
node .claude/scripts/acceptance-run-check.mjs → ok:true, phase:"executor-skipped",
                                                serverAcceptanceFiles: 8（7→8，本包新增）
node .claude/scripts/unit-run-check.mjs       → ok:true, phase:"executor-skipped"
```

**这两个 `ok:true` 只担保「不会静默跳过」，不担保本包验收跑过。**
本包 server 侧验收的「真跑过」证据只由下面这条点名命令承载（第 3 节即该命令实跑结果）：

```
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
```

verifier / landing-verifier 若只看两条闸的输出，会得出「验收闸绿」的假结论——请按本节口径核销。
（本包未给整档加 module 级 `pytestmark`，离线齿与 live 齿分开：离线齿可被点名真跑，
live 齿由 `server/acceptance/conftest.py` 的机械 skip 闸挡住，`RUN_LIVE=1` / `-m live` 放行；
`server/pytest.ini` 未改——按 planner 指示，addopts 属对端并行包也可能碰的共享治理面。）

## 2. C 侧红基线（命令与实跑）

冻结验收文件的闸上 runner = host_app linux-target ELF；但 `HOST_ACCEPTANCE_SRCS` 接线属
builder（`host_app/main/CMakeLists.txt` 不在 acceptance-author 的可写面内，见 5.3）。
本次按包 0/2/6 先例，用边界内脚手架（本目录 `unity.h` + `runner.c`，非冻结件）把**同一份
冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
wsl -d Ubuntu-24.04 -- bash -c "gcc -std=c11 -Wall -Wextra -I. -o /tmp/krm_red_runner \
    ../../schedule_reminder_b.acceptance.c runner.c"          # BUILD_CLEAN：零告警零错误
wsl -d Ubuntu-24.04 -- bash -c "cd /mnt/c/AI/KidPal-pod-schedule-reminder && /tmp/krm_red_runner"
```

实测：**10 Tests 8 Failures**（2 绿均为必须保持绿的守卫齿）。执行通道错误为零
（编译干净 + 10 个 TEST_CASE 全部真执行并逐条给出可归因红因）= 「冻结即冒烟 /
C 类冻结验收实编」判据达成。

注：ELF 退出码经 `wsl.exe` 的 `bash -c` 包装读 `$?` 恒 0（WSA/VRT 摘要已记录同款伪影），
判据取报告行 `N Tests M Failures` 与逐条 FAIL 明细。

### 2.1 逐条判定（C 侧，红因 = 各自真实缺失行为）

| TEST_CASE | 判定 | 红因 |
|---|---|---|
| 提醒设备消费面装配结构 (RMD-A1, structural) | **RED** | `firmware/main` 无任何文件实现 frozen contract（`kidpal_reminder_receive(` 零引用）→ 齿 1/2/4/5/6/7 全缺：无 state-reminder 模块、未编入设备 CMakeLists、`mcp_server.cc` 无 `kidpal/alert` 注册与路由、渲染函数无调用点、「渲染才回执」判定门无人查询、提醒模块无 `PlaySound/OGG_POPUP` 提示音面、无降级语义标记。包 2/包 6 底座守卫绿。 |
| 上游 mcp display case 装配完整性守卫 (RMD-A1 guard) | **RED（非本包缺失，上游既有缺陷）** | 见 5.1：`mcp_server.cc` 的 `kidpal/display` case 4 内 `app.Schedule(` lambda 闭合被 char-stroke merge 覆盖丢失。 |
| kidpal/alert 载荷解析降级与 fire_at 归一 (RMD-A2/A4) | **RED** | `kidpal_alert_parse` / `kidpal_fire_at_minutes` 未实现（weak 哨兵 -2）→ 逐字段保真、缺 speech/缺 view 容忍、畸形/缺 id/缺 title/坏 fire_at 的 -1 降级、安全截断、ISO 偏移归一全部缺失。 |
| 三件套动作集与 speech 缺省降级 (RMD-A3) | **RED** | `kidpal_reminder_render_actions` 未实现 → 「弹窗+提示音必达、语音仅在 speech 非空时、缺 speech 不阻塞」决策集缺失（V20 降级面）。 |
| 排队逐个弹 + V18 滞留顶替 (RMD-A5) | **RED** | 状态机全套未实现 → 接收不等于渲染、FIFO 逐个弹、事件时刻前不替换/过后允许顶替、顶替非追加送达、队列溢出有界等全部缺失。 |
| KEY 关闭回当前页、BOOT 关闭照常翻页 (RMD-A5 gesture) | **RED** | `kidpal_reminder_receive` 未实现 → fixture 第一步即红；KEY/BOOT/KEY 长按三类 DISMISS 与四组 PASS（不吞其它层）、P4→P1 回绕接缝全部未验到。 |
| 四态决策表——渲染才允许回执 (RMD-A6) | **RED** | `kidpal_reminder_may_emit_receipt` 未实现 → received/queued/rendered/acked 四态解耦、渲染前不得回执、回执一次后回落、同 id 重推两态分处（排队不回执 / 已渲染补回回执）全部缺失。 |
| 易失 known-id 只驻 RAM 与介质禁令 (RMD-A6/A7) | **RED** | 行为半：`kidpal_reminder_receive` 未实现（re-init 即全忘 / 已知 id 再推被接受 / 集合有界未验到）；结构半：无模块可扫（介质禁令的 NVS/文件系统/动态分配负向齿因定位不到文件而红）。 |
| 断网无提醒——设备零本地事项源 (RMD-A7 boundary) | **GREEN（守卫，必须保持绿）** | 实测 `firmware/main` 下 `schedule.md` / `pending.json` / `semantic.json` / `"20:30"` / `"07:45"` / `remind_before_min` 六个 needle 命中数均为 0 → 设备零本地事项源、三档时刻永不烧板子成立；builder 若把它们写进固件即转红。 |
| 扫描器自检 + 不发明 view 契约零变更边界 (boundary/security) | **GREEN（守卫，必须保持绿）** | 扫描器四态自检 + 计数器校验通过；`VRT_ROUTE_*` 八值数值码未偏移；`kidpal_display_route` 弱符号在独立 stub 未链接 view_router.c → 白名单外 / `reminder` / `state-reminder` 不路由的三条续接齿在接线后生效（如实记录）。 |

### 2.2 独立 stub 编译排除项（如实记录）

`提醒弹窗渲染面真帧（RMD-A5 render）` 这一个 TEST_CASE 需 `lvgl.h` include path，独立 stub
编译无该路径 → 被 `__has_include` 守卫编译排除，**其红态在 builder 把本文件接线进 host runner
时兑现**（`kidpal_reminder_page_render` weak 哨兵 → 「frozen contract NOT implemented」红）。
它经包 0 冻结渲染缝 `host_infra_render_frame()` + `host_infra_read_pixel()` 采样网格断言
真实像素：画到屏上（非纯缝背景 + 暗像素计数）、两个不同事项真帧可见差异（内容正确性）、
重渲染帧完全相同（先清活动屏 = 不叠屏）、NULL/无 id 返回 -1 不崩。网点疏密与 pose 动画
细节属设计 review，未钉成脆断言。

### 2.3 host ELF 相对基线（冻结相位 + builder 落地后各测一次）

**冻结相位**（`firmware/main/test/host_app/build/host_infra_test.elf` = planner 后台首跑产物，
本包未改 `CMakeLists.txt`）：实跑 **60 Tests / 3 Failures**，与 planner 260908 更正一致：
`voice_roundtrip_b.acceptance.c:878` VRT-A2/E2、`device_ws_switch.acceptance.c:830` WSA-A6、
`device_ws_switch.acceptance.c:663` WSA-A4——三条全是 `[evidence]` 族，本地-only gitignore
证据从未入仓 → main 本身即红。**判读 = 3 条既有红不变 + 本包新增齿按 2.1 节全红。**

**builder 落地后复测**（本包 11 齿 + reminder_page.test.c 5 齿已注册进 runner）：
实跑 **76 Tests / 3 Failures**，且逐条核对三条失败**正是上述 VRT-A2 / WSA-A6 / WSA-A4 一条未动**。
60→76 = 本包 11 个验收齿（全部 `:PASS`，含独立 stub 编译被排除的 RMD-A5 render 真帧齿、
以及 RMD-A1 guard 上游装配完整性齿）+ builder 自带 5 个单测齿。
**口径校正**：builder 收工报「76T/2F、VRT-A2 已转绿」是误读——VRT-A2 证据齿并未转绿，
实况 3 Failures（planner 重跑同结论）。后续一律以 **3 条既有红** 为准；
`firmware-build-check` 侧的 `mcp_server.cc` 语法缺陷已由本包 builder 补回（RMD-A1 guard 齿转绿佐证）。

## 3. python 侧红基线（命令与实跑）

```
cd server && python -m py_compile acceptance/test_schedule_reminder.acceptance.py   # COMPILE_CLEAN
cd server && python -m pytest -q --import-mode=importlib --collect-only acceptance/test_schedule_reminder.acceptance.py
    → 38 tests collected in 0.01s        # 收集零错误（§3 冻结即冒烟 / §4 C 类实编）
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
    → 30 failed, 3 passed, 5 skipped (3.12s)
cd server && python -m pytest -q --import-mode=importlib           # 全档
    → 38 failed, 67 passed, 40 skipped (4.34s)
```

判读（相对口径）：main 基线 8 failed / 64 passed / 35 skipped（7 条 smoke_e2e + 1 条 vrt
证据齿，全在 live-marked 文件内、属包 S 退役架构面）。落本包两档后
**38 / 67 / 40 = 8 条既有红不变 + 本包 30 条新红 + 3 条守卫绿 + 5 条 live skip**，
逐条对账闭合、无第三条来源的漂移。

**builder 落地后复测（2026-09-08，含本摘要第 6 节的冻结件自身缺陷修复）**：
点名命令 → **33 passed / 5 skipped / 0 failed**；全档 → **8 failed / 134 passed / 40 skipped**
—— 8 条既有红（smoke_e2e 7 + vrt 证据齿 1）一条未动，本包 33 个离线齿全绿、5 个 live 齿仍被
机械 skip（等 `changes/dev/260829-schedule-reminder/` 真跑证据落盘后由点名命令放行）。

### 3.1 30 条红的分组（每条对应哪个 RMD-Ax 与哪个缺失行为）

| 分组 | 条数 | 红因（真实缺失行为） |
|---|---|---|
| RMD-A1 | 1 | `server/gateway/reminders.py` 与 `server/deploy/openclaw-patch/reminder_schedule_lib.py` 两模块均不存在（frozen offline contract 未实现）；`pending_path()` env 可配交接路径不存在。 |
| RMD-A2 | 8 | 三档算术、按区块判范围（学校区块零提醒 / 每周重复 / 过期一次性不生成）、已过去档位跳过、幂等 id 三维度独立且跨重算不变、时刻从配置载体读、pending.json 真落盘原子写与写后读回、真 ECS 缺区块形状、废止 `remind_before_min` 不参与计算——全部无承载体。 |
| RMD-A3 | 4 | speech 注入指令构造（`kidpal_speak` 走引擎腿）、缺 speech 的降级仍投递、设备腿零二进制帧、引擎部署副本认领注入指令的载体——全部缺失。 |
| RMD-A4 | 10 | §三 信封载荷形状与 view 字符串化、JSON-RPC 传输 id 数字化与 int32 无碰撞、`mcp_relay.alert_tool_call` 仍把字符串业务 id 当传输 id（独立齿，实测红）、每分钟节拍对齐与到点闭区间、重试到确认 60s×3 后静默作废并留审计轨迹、回执翻译成业务 id 出队且幂等、重连不补推、同 id 重算复用不重开预算、front 接线四点位——全部缺失。 |
| RMD-A6（网关侧） | 1 | 非法载荷 reject 不留痕 + 投递记账无界（ECS Swap 535MB 约束下的有界性）无承载体。 |
| RMD-A7（网关侧） | 1 | 离线期间已过重试窗口的档位在推送时点判定作废（不补发）无实现。 |
| RMD-A8 | 4 | 备注列→携带内容（兜底文案带备注原文 / 语义 bring 优先）、语义中间产物 schema 与坏缺输入降级、prompt 约定载体 v3 落盘并接渲染、B1 表体零变更复用包 6 解析器——全部缺失。 |
| boundary（错误/权限路径） | 2 | §三 机械校验 11 种畸形输入的逐形拒因与读侧坏文件降级、配置/语义载体坏输入清洗——全部缺失。 |

3 条绿守卫：`test_e2_existing_frozen_acceptance_suites_green`（directionB / g-relay-fix /
schedule_p2 三档独立复跑 exit 0，实测本包落盘时全部 rc=0）、
`test_e3_no_http_alerts_endpoint_revival_and_no_data_duplication`（gateway 内零
`/v1/alerts` 复活 + 有 `kidpal/alert` 载体）、
`test_e4_contract_point3_evidenced_and_poll_form_retired`（契约「已实证」+ `kidpal/alert`
+ spec「重试到确认」三处记录在世）。

5 条 live skip（`@pytest.mark.live`，conftest 机械 skip；`RUN_LIVE=1` / `-m live` 放行）：
真 cron 在册与 pending.json 真身、真到点推送与真回执出队、真大模型语义产物、真语音注入出音
日志事实、真断网一轮无提醒作废。每条都要求 `changes/dev/260829-schedule-reminder/` 下
**真跑一轮的落盘证据**（probe ≠ exercise），并要求证据内不得出现失败字样
（`Invalid id` / `Unknown message type` / 推送失败 / 排程失败 等）。

### 3.2 真形状 fixture 依据（不自造、不冒充）

- `B1_CANONICAL` = 契约 §2.2 定稿表体逐字（三类区块 + `remind_before_min: 15` 废止字段仍在），
  一次性安排日期改到 horizon 内（`2026-09-11`）+ 保留一条过期项（`2026-08-30`）以同时钉
  「精确匹配当日/未来」与「过期自动忽略」。星期对照 2026-09-07=周一 起（`datetime` 实测核对）。
- `B1_REAL_ECS_SHAPE` = planner 2026-09-08 ECS 只读实测的 549B 真身等价体：**只有**
  「## 每周固定课表」一节、时间列 `HH:MM-HH:MM`、备注列 7 行全空、无「学校课程表」也无
  「一次性安排」、frontmatter 仍带 `remind_before_min: 15` → 钉「缺区块不崩 + 废止字段不消费
  + 备注全空不得编出带物文案」。
- `ALERT_CANONICAL` = 契约 §三 载荷块逐字段（`id/fire_at/title/speech/view`）。
- 设备 wire 事实取实测点：`application.cc:492`（只 `type=mcp` 帧进 McpServer）、
  `mcp_server.cc:546`（`cJSON_IsNumber`；本包实测该判据在 546 行，launch 指令写的 473 行
  是 `AddTool` 定义处，已在冻结件头按实测行号标注）、`mcp_server.cc:603` +
  `application.cc:906`（延迟 `ReplyResult` 是「渲染才回执」的唯一可用出口）、
  `mcp_server.h:52-56`（Property 仅 bool/int/str → 嵌套 `view` 必须字符串化）。

## 4. 冻结为 frozen offline contract 的符号（builder 只读消费，改不动）

设备面（`firmware/main/test/schedule_reminder_b.acceptance.c` 文件头；建议家
`firmware/main/display/reminder_page.{c,h}`，纯 C、host 可编、自带极简 JSON 解析、不引外部库）：
`kidpal_alert_t`（定长：id[40]/fire_at[32]/title[64]/speech[96]/view_json[256]）、
`struct kidpal_reminder_s`（不透明，只按指针使用 = 锁行为不锁内部）、
`kidpal_alert_parse` / `kidpal_fire_at_minutes`（含六个冻结参考点 29813070 / 29814510 /
29815185 / 29815695 / 29815710 / 29815860）/ `kidpal_reminder_init` / `kidpal_reminder_receive` /
`kidpal_reminder_known_count` / `kidpal_reminder_queue_count` / `kidpal_reminder_shown_id` /
`kidpal_reminder_raise_next` / `kidpal_reminder_may_emit_receipt` /
`kidpal_reminder_mark_rendered` / `kidpal_reminder_mark_acked` /
`kidpal_reminder_render_actions` / `kidpal_reminder_key` / `kidpal_reminder_page_render`，
常量 `KRM_QUEUE_CAP=8` / `KRM_KNOWN_CAP=32` / `KRM_STATE_*` / `KRM_KEY_*` / `KRM_ACT_*`。

服务端模块 1（`server/deploy/openclaw-patch/reminder_schedule_lib.py`，纯 stdlib、离线可导入）：
`LADDER_NIGHT/MORNING/SOON`、`DEFAULT_LADDER_CONFIG`、`load_ladder_config`、`entry_key`、
`reminder_id`、`load_semantic`、`bring_items`、`plan_reminders`、`write_pending_atomic`。

服务端模块 2（`server/gateway/reminders.py`，纯 python、导入期零网络）：
`POLL_INTERVAL_SEC/RETRY_TIMEOUT_SEC/MAX_ATTEMPTS/MAX_PENDING/MAX_TRACKED/VOID_WINDOW_SEC`、
`next_poll_delay`、`to_epoch_min`/`parse_iso`、`validate_alert_entry`、`read_pending`、
`due_entries`、`alert_jsonrpc_id`、`build_alert_envelope`、`build_speech_injection`、
`build_alert_frames`、`deliver_alert`、`AlertDispatcher`、`consume_receipts`、`pending_path`。

配套载体（builder 需新落）：`server/deploy/openclaw-reminder.config.yaml`（三档时刻的家，
缺省必须 = 20:30/07:45/15）、`server/deploy/openclaw-system-prompt.v3.md`（版本化新文件，
不覆盖 v2）、`server/deploy/engine-patch/connection.py` 内认领 `kidpal_speak` 并接 `speak_txt`、
`server/gateway/front/front_app.py` 内 `read_pending(` / `next_poll_delay(` / `deliver_alert(` /
`consume_receipts(` 四点位接线。

## 5. 必须回报 planner 的事项

### 5.1 上游既有缺陷（非本包缺失、曾阻塞设备构建）——已闭环

`firmware/main/mcp_server.cc` 的 `kidpal/display` case 4（`VRT_ROUTE_SCHEDULE`）内
`app.Schedule([...]() { kidpal_schedule_page_render(...) ...` **lambda 闭合被丢**：
`205faa8`（包 6 landing）在该处有 `});` / `break;` / `}` 三行，`f2a67dd`
（260829-char-stroke）之后直接接 `case 3:` —— `case` 标签落进 lambda 体内，C++ 语法不合法，
`idf.py build`（即 `firmware-build-check.mjs` 承重闸）必红。已实测 git 双版本对证。
影响面：设备构建 + 任何 on-target 验收；**不影响 host ELF**（host_app 独立工程不编 `mcp_server.cc`）。
本包把它钉成独立可机判守卫齿（`RMD-A1 guard`，冻结相位实测 RED）。
**闭环状态**：采②路径——本包 builder 在接 `kidpal/alert` 时补回该三行，守卫齿在
76T/3F 复测中转绿。

### 5.2 契约无需改动（无闸 A 触发项），但两条推导需 planner 背书

1. **`kidpal/alert` 的 `arguments.view` 在设备上以 JSON 字符串承载**。契约 §三 的 JSON 块描述的
   是 OpenClaw→网关的**逻辑载荷**（view 是对象）；契约 §四 + `mcp_server.h:52-56`（Property 仅
   bool/int/str）与 `mcp_server.cc:104` 区段 `Property("data", String)` 是**设备 wire 事实**。
   本冻结件按「逻辑载荷对象 ↔ wire 上字符串化」两层钉死（与 kidpal/display 的 `data` 字符串化
   同纪律），未新增/未改契约字段。**不需要动 §三**；若 planner 希望 §三 显式加一行 wire 注记，
   属可选的可读性改进，不是本包验收的前置。
2. **V20「排程时预合成存缓存」的「存缓存」半边在离线面不可钉**。裁定二把出音改为
   「front 注入内部指令 + 引擎现成 `speak_txt` 走既有 edge-tts + opus 下行链」，front 侧
   不自建合成、不建合成缓存。本档因此只钉三件可机判事实：§三 `speech` 文案逐字进注入指令、
   speech 缺省/失败时弹窗投递照常且引擎腿零注入、设备侧 `render_actions` 恒含 SHOW|TONE。
   「到点实时 vs 预合成」的时序收益属引擎链事实，由 live 齿（引擎认领 + 出音日志）与 9b
   （真机音频与弹窗并发时序）承载。**这是 spec V20 措辞与裁定二之间的口径差，不是契约差**
   （§三 只规定载荷带 `speech` + 失败降级），请 planner 确认按裁定二记账而不需回改 §三。

### 5.3 权限边界内的两处留给 builder

1. `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 注册
   不在 acceptance-author 可写面（允许面 = `firmware/**/*.acceptance.{c,cpp}` +
   `firmware/**/acceptance/**` + `server/**/acceptance/**` + `tools/**`，enforce-scope 钩子实测）。
   需加两行：`"../../schedule_reminder_b.acceptance.c"` 与提醒实现源
   `../../../display/reminder_page.c`，**必须加在 `"../../host_infra_smoke.acceptance.c"` 之前**
   （该条必须保持列末位：包 0 的 nothing-rendered 边界齿依赖进程内最先执行，实测执行序 =
   列表逆序）。冻结件头已写明同一要求。
   **已完成状态**：builder 落了 L53（本冻结件）、L54（`reminder_page.c`）、
   L57（自带单测 `reminder_page.test.c`），`host_infra_smoke.acceptance.c` 仍在**末位** L59 ——
   相对顺序红线未破（76T/3F 实测中包 0 边界齿 PASS）。
2. 交接目录：ECS 上 `alerts/` 与 `pending.json` 当前**不存在**（planner 实测），故
   `read_pending` 对缺文件必须安静返回 `([], [])`（已钉），路径本身经
   `KIDPAL_ALERTS_DIR` / `KIDPAL_PENDING_JSON` 可配（`pending_path()` 齿已钉）——
   builder 不得把 `/root/.openclaw/...` 硬编进读取路径。

### 5.4 质量注记（qualityNotes）

- 真实持久化副作用：`pending.json` 走真 tmp 目录落盘 + 读回 + `.tmp` 残留检查 + 两次写入
  alerts 数组逐字节比对；非 mock 文件系统。
- 领域事件/审计面：`AlertDispatcher.events` 被钉为精确序列
  `["push","push","push","void"]`，非法载荷必须产 `reject` 事件（不是「没报错就算过」）。
- 状态机决策表：RMD-A6 用「`may_emit_receipt` 何时才为 1」的纯逻辑决策表钉「渲染才回执、
  接收不回执」，把 C++ 线程/同步工具回复时序留在实现自由度里（planner 要求的可机判边界）。
- 介质禁令的可机判形态 = 行为面（re-init 即全忘 + 已知 id 再推被重新接受 + 集合上界）
  **加** 结构面（模块内零 `nvs_open`/`nvs_set`/`NvsHandle` / 零 `fopen`/`freopen`/`pending.json`
  / 零 `malloc(`/`calloc(`/`= new `/`std::vector`），不单靠源码 grep。结构面的模块定位
  按 `firmware/main/display/` 优先、全树兜底（建议家 = 包 2/5/6/8 共享显示底座所在处，
  不锁死位置）。
- UI 验收分桶：钉了「真画到屏上 / 内容随载荷变化 / 重渲染不叠屏 / 数据可见性」四条任务关键
  事实；整屏压网点疏密、pose-jump 动画细节、真实屏幕观感按 acceptance-evidence 口径留
  design review 与 9b，未钉成脆断言。
- 错误路径：C 侧畸形载荷 / 缺 id / 缺 title / 坏 fire_at / 越界键位 / 队列溢出 / NULL 状态全打；
  python 侧 11 种 §三 畸形形 + 半截 JSON + 缺文件 + 坏配置 + 坏语义条目连坐 + 未知 id 回执全打。
- 跨档一致性：§三 业务 id 上界 40 字节 = 设备 `kidpal_alert_t.id[40]`，网关机械校验按同一
  上界拒超额（避免设备侧静默截断把两条提醒并成一条）；`KRM_STATE_*` 四态命名与
  `AlertDispatcher` 的 `pending/pushing/acked/void` 一一对应，两档共用同一幂等 id 语义。
- 防假绿：两档都自带扫描器/装载自检（真实文件可见、假文件不可见、乱 needle 不命中、
  计数器能数已知字面量），负向齿因此具备「能失败」的能力。
- decisionCoverage 反查：五条决策全部有断言承载、无 dropped 条目——
  决策20→RMD-A2 组、决策27→RMD-A3 组、契约§三推送语义→RMD-A4 组、决策24→RMD-A7 组 +
  C 档 RMD-A6/A7 易失齿、V18→C 档 RMD-A5 顶替齿（python 档不重复钉，映射见两档文件头）。

## 6. 冻结件自身缺陷修复记录（2026-09-08 planner 回派，acceptance-author 修）

builder 实现落地后点名命令剩 4 条残红，planner 独立核实为**本冻结件（python 档）自身两处缺陷**，
非实现缺陷。按「验收件只有 acceptance-author 能改」的纪律由本角色修复，
**断言语义零变更**为硬红线：只改 fixture 参考日期与采集形状，未删齿、未放宽、未迁就实现。

### 6.1 缺陷一 = 钢琴课期望簇落在「已过的那一周」（牵动 3 齿）

根因：`now = "2026-09-08T07:00:00+08:00"` 是**周二**（日历真值 09-07=周一、09-08=周二，
`date.fromisoformat('2026-09-08').isoweekday()==2` 独立核实），而钢琴课是「周一 16:30」——
我原来期望的 `09-06T20:30 / 09-07T07:45 / 09-07T16:15` 三档**全部早于自身 now**，
与同档 `test_a2_past_ladders_skipped`（已过档位一个都不发）直接互斥：
任何单调的「fire_at ≤ now 即不产出」规则不可能同时满足两齿。builder 按 spec §4.1
「已过去的档位自动跳过」+ 决策 20「每周固定项每周重复生成」产出**下一次 occurrence**，正确。

改动（`server/acceptance/test_schedule_reminder.acceptance.py`）：

| 位置 | 改法 |
|---|---|
| `test_a2_three_ladder_arithmetic` L561-566 | 钢琴课期望簇 → `09-13T20:30 / 09-14T07:45 / 09-14T16:15`；并在齿内加 8 行注记（now=周二、09-07 周次三档全已过、按决策 20 取 next occurrence、**显式写明「不要改回 09-06/09-07，否则与本档 past-skip 齿无共同解」**）；顺手清掉头前一条语义混乱的旧注释 |
| `test_a2_ladder_times_come_from_config_not_code` L716-721 | 同簇按配置变体 bump → `09-13T19:45 / 09-14T06:30 / 09-14T16:00`（16:30 − 30min），加同款注记 |
| `test_a8_bring_from_note_and_semantic_override` L1163-1164 | 前晚档过滤日期 `2026-09-06T20:30` → `2026-09-13T20:30`，加注记 |

**零语义变更的自证（不是抄实现输出，是独立推导后与实现互验）**：
用与 builder 无关的一小段 `datetime` 脚本从「周一 16:30 + 三条阶梯关系」正向推 next occurrence：
`event day = 2026-09-14 Mon` → night `2026-09-13T20:30+08:00` / morning `2026-09-14T07:45+08:00` /
soon `2026-09-14T16:15+08:00`，且反证上一周次三档 `[09-06T20:30, 09-07T07:45, 09-07T16:15]`
全部 `<= now` → 应被跳过。推导结果与修复后的期望逐字相同，builder 实现亦与之相同（三齿转绿）。
本齿钉的仍是那三条关系：**前晚档 = 事件日前一天 20:30**、**早档 = 事件日当天 07:45**、
**临期档 = 开始时刻 − 15 分钟 = 16:15**，只是参考日落在正确的周。

修复后的齿仍具鉴别力（三条扰动探针实测，防「改成实现输出=空断言」）：

| 探针 | 结果 | 含义 |
|---|---|---|
| 期望 = `09-13T20:30 / 09-14T07:45 / 09-14T16:15` | **MATCH** | 关系钉得准 |
| 临期档扰动为 `16:16`（+1min） | **不匹配** | 15 分钟减法被真钉住 |
| 前晚档扰动为事件日当天 `09-14T20:30`（丢跨天映射） | **不匹配** | 「事件日前一天」映射被真钉住 |
| 旧期望（`09-06/09-07` 簇） | **不匹配** | 旧期望确属错误、不是等价写法 |

同齿内 英语培训(周三→09-09) 与 美术班(周六→09-12) 两组期望**未动**（本就一致）——
这也是「不是一次性把全部期望改成实现输出」的反证：改动只落在唯一参考周错的那一个条目上。
`test_a2_past_ladders_skipped`（与旧期望互斥的那条）**未改**，仍作为「不得产出已过档位」的守卫在位。

### 6.2 缺陷二 = a4 该齿 sink 采集形状与自身解包冲突

根因：`test_a4_retry_to_ack_60s_x3_then_void` 用 `sink=lambda i, e: pushed.append(i)` 只存业务 id
字符串，却在下一行 `[i for i, _ in pushed]`、尾部 `[e for _, e in pushed]` 按二元组解包
→ `ValueError: too many values to unpack`（采集形状自相矛盾，不是实现问题）。
同文件其余 a4 齿要求 `pushed == [业务 id]`，故多数形态（sink 签名 `(id, envelope)`、
列表存业务 id）才是本包既定制式。

改动（同一齿内）：

| 位置 | 改法 |
|---|---|
| L985-996 | 拆成两个采集器：`pushed`（业务 id 序列，与兄弟齿同形）+ `envs`（每次推送的 JSON-RPC 信封序列），sink 改为具名 `_sink(bid, envelope)` 同时 append 两者；首个断言由 `[i for i,_ in pushed] == [id]` 改为 `pushed == [id]`（等价、不再 unpack） |
| L1013-1021 | 删掉错误的 `envs = [e for _, e in pushed]` 重绑定，直接用 `_sink` 采集到的 `envs`；补一条 `len(envs) == 3` 的采集自洽断言（中性，不改变被钉行为） |

被钉的四条事实**一条未松**：`attempts()` 依次 1→3；`len(pushed)` 在 60s 内不增、
60/120s 各 +1、180s 后仍为 3；`state_of() == "void"`；审计轨迹精确等于
`["push","push","push","void"]`；三次推送的传输数字 id 集合大小为 1、`arguments.id`
业务 id 集合恰为 `{rmd-3f2a1b9c4d5e6f70}`。builder 自带单测
`test_retry_to_ack_sequence_is_bounded_and_audited` 从行为侧钉同一事实，可作旁证。

### 6.3 修复后复跑（实测，非推理）

```
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
    → 33 passed, 5 skipped（0 failed）
cd server && python -m pytest -q --import-mode=importlib
    → 8 failed, 134 passed, 40 skipped        # 8 条既有红一条未动
node .claude/scripts/acceptance-coverage-check.mjs <实况路径副本>
    → ok:true, acceptance RMD-A1..A8 全在册, structural ["RMD-A1"]
wsl ... ./build/host_infra_test.elf
    → 76 Tests / 3 Failures（三条既有 [evidence] 红不变；本包 11 个 C 齿全 PASS）
```

## 7. tighten-only 补齿记录（2026-09-08 第三单：两条 mutation `sensitive:false` → true）

来源 = mutation 跑出的两处覆盖缺口（builder 执行变异、planner 独立归因、architect advisory
确认本仓 tighten-only 既成纪律、human 知情）。两份冻结件**纯 additive**：
`git diff --numstat` = `101 / 0`（C 档）与 `200 / 0`（python 档），
`git diff -U0 | grep -c '^-[^-]'` = **0**（零删除行、零修改行，既有断言一字未动）。

### 7.1 补 1（RMD-A5，设备渲染面「先清屏」具名化）

新齿：`firmware/main/test/schedule_reminder_b.acceptance.c:1640`
`TEST_CASE("schedule reminder b: 渲染面先清屏具名化——B 后不残留 A 的场景对象 (RMD-A5 tighten)",
"[schedule_reminder][RMD-A5][render]")`（host-runner-only，在 `#ifdef KRM_HAVE_LVGL` 内）。

设计要点（为何不是原教「A 独占区域回到背景」的像素齿）：读实现后确认
`reminder_page.c` 的卡片几何（`KRM_CARD_*_PCT`）**与载荷无关**，且每帧先画整屏白底 +
整屏 50% 压层 + 不透明白卡 —— 漏掉 `lv_obj_clean` 时上一帧残留对象被本帧不透明层
**完全盖住**，任何载荷组合都造不出「A 独占而 B 不复画」的像素区域。故像素版仍是空齿。
本齿把 RMD-A5 已声明的同一意图（新提醒排队逐个弹、**不叠屏**）钉在**场景对象残留**层：

1. 测试自己 `lv_obj_clean` 建纯净基线（与被测实现的清屏动作无关），只渲染 B → 记子对象数 `alone_n`，并断言 `alone_n > 0`（防空实现假绿）；
2. 同一起点先渲染 A 再渲染 B → 断言子对象数 `== alone_n`（漏清屏 = 多出 A 的全部对象 → 咬）；
3. 再渲染一次同一提醒 → 数量仍 `== alone_n`（重复弹出不累积对象）；
4. 附一条最终帧像素一致性断言（本变异下按上述原因仍绿；作用是把「A 的可见内容不残留」也钉在案上，并在渲染面日后按头注记升级路径接中文字库/pose 位图、几何随载荷变化时自动获得鉴别力）。

A 与 B 故意走**不同绘制支路**（A 有 speech = 三件套全画；B 无 speech = 仅弹窗支路；标题一长一短），
残留探针更严。只用既有缝与采样手法（`host_infra_render_frame` / `krm_sample_grid`），
子对象数是 `lv_obj_clean` 之后的纯同步读数，不含任何 LVGL 渲染时序依赖。

### 7.2 补 2（RMD-A3，front→引擎腿注入的离线行为齿）

新齿：`server/acceptance/test_schedule_reminder.acceptance.py:957`
`ScheduleReminderOfflineAcceptance::test_a3_front_poll_injects_speech_into_engine_leg`
（配套新增装载器 `_load_front_app` + `_ensure_fastapi_stub`，本机 fastapi 实测缺席需桩；
异常类用真类以保证 front 内 `except (WebSocketDisconnect, RuntimeError)` 在桩下仍有效）。

真跑 front 一轮投递周期 `run_alert_poll_once(now_epoch=...)`，假设备腿 / 假引擎腿双 sink 收帧后
按**对端可观察事实**断言（`_register_alert_legs` / `_unregister_alert_legs` 只作 setup，
finally 注销避免污染兄弟齿；不断 front 内部私有符号形状）：

- 有 speech：`pushed == [业务 id]` → 设备腿恰 1 条 `kidpal/alert` mcp 帧、引擎腿注入条数
  **== 本周期推送条数** 且恰 1 条、`text` 等于载荷 speech 原文且非空、
  `device.sent_bytes == []`（裁定二红线端到端再钉一次）、设备腿内不得混入 `kidpal_speak`；
- 无 speech 对照：设备腿仍恰 1 条（必达）、引擎腿**零**注入（降级 ≠ 外发空文案）。

不断言任何音频 / 真机时序（属已冻结的 `test_a3_live_tts_and_popup_timing_evidence` + 9b 殿后面）。

### 7.3 变异自证表（用当初暴露 vacuity 的原变异，非另造）

| 变异（原样重放） | 新齿 | 既有齿 | 还原 |
|---|---|---|---|
| `front_app._deliver_alert_now`: `deliver_alert(entry, device_ws, engine_ws)` → `(…, device_ws, None)`（语音永不外发） | `test_a3_front_poll_injects_speech_into_engine_leg` → **FAIL**（`0 != 1`，两腿条数对账） | 同档其余 **33 个离线齿全绿**，含不敏感的 needle 齿 `test_a4_front_wires_delivery_and_receipt_dequeue`(L1266) → **仍 GREEN** | `git checkout` 还原，`front_app.py` sha256 `fc8c50a9…29257` **逐字同值** |
| `reminder_page.c:1088` 删 `lv_obj_clean(scr);` | `…(RMD-A5 tighten)`（L1640）→ **FAIL**（ELF 77T/**4**F，唯一新红恰是它） | 既有像素齿 `accum == 0`(L1596) 及其余 11 个本包齿 → **全 GREEN**（正是要补的 vacuity） | 还原后 `reminder_page.c` sha256 `a5b1d028…2f4c39` **同值**、`git diff` 内容零行、ELF 重编回 77T/**3**F |

`git diff --numstat` 复核变异期间均为 `1 / 1`（单行、可逆），两条变异均已彻底撤除。

**还原注记（诚实）**：本仓 `core.autocrlf=true`，`git checkout --` 会把工作树重新物化成 CRLF，
而 builder 提交前的工作树字节是 LF（`reminder_page.c`  sha256 由 `a5b1d028…` 短暂变
`0852b80b…`）。已按 `git hash-object`（`8e693c1f…` == HEAD blob）与 `git diff` 内容零行
双证后把该文件转回 LF-only，现 sha256 与变异前**逐字节同值**；`git status` 对该文件仍显
` M` 只是 CRLF/LF 的 stat 伪影（`git diff --stat` 内**不含**该文件、零内容行），
非我留下的实现改动。

### 7.4 补齿后复跑（实测）

```
cd server && python -m pytest -q --import-mode=importlib --collect-only acceptance/test_schedule_reminder.acceptance.py
    → 39 tests collected（零收集错误）
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
    → 34 passed, 5 skipped（0 failed）
cd server && python -m pytest -q --import-mode=importlib
    → 8 failed, 135 passed, 40 skipped        # 既有 8 红一条未动
node .claude/scripts/acceptance-coverage-check.mjs <实况路径副本>
    → ok:true, acceptance RMD-A1..A8 全在册, structural ["RMD-A1"]
cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash espidf_wsl.sh build   # 未用 -T main
    → rc=0, error: 计数 0
cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
    → 77 Tests / 3 Failures（既有三红 VRT-A2 / WSA-A6 / WSA-A4 一条未变 2、未变 4）
      本包 12 个验收齿全部 :PASS（原 11 + 本单新增 1）
```

### 7.5 回写 TaskPackage.mutationEvidence 用的 affectedTest 名

- 补 1（`reminder_page.c` 删 `lv_obj_clean`）→
  `firmware/main/test/schedule_reminder_b.acceptance.c::[schedule_reminder][RMD-A5][render]::渲染面先清屏具名化——B 后不残留 A 的场景对象 (RMD-A5 tighten)`
  （C 侧 unity 无 python nodeid，建议以 `文件::TEST_CASE 标题` 记账；标题含唯一子串 `RMD-A5 tighten`）
- 补 2（`_deliver_alert_now` 断引擎腿）→
  `server/acceptance/test_schedule_reminder.acceptance.py::ScheduleReminderOfflineAcceptance::test_a3_front_poll_injects_speech_into_engine_leg`

## 8. 第四单：carrier 宽词假绿收紧（2026-09-08，触发源 = integrator 跨包合并探测）

同源缺陷、不同触发点：`.v3` 版本化槽位被并行包 story-p3 与本包抢（add/add 冲突）。
旧 carrier 齿 `test_a8_prompt_carrier_v3_landed_and_wired` 的两个 needle（「备注」「学校课程表」）
是**课表面与提醒面共享的宽词** —— 实测 main 那份 160 行版本里分别命中 3 / 2 次，
而它把「包 7 — 提醒备注解析约定」只写成增量扩展区里的 2 行占位标题（正文缺席）。
于是「合并取 main 版 = 本包 181 行提醒约定静默消失」这一最可能失败模式下旧齿仍 PASS = 假绿。

### 8.1 新齿（python 档，纯 ADD）

`server/acceptance/test_schedule_reminder.acceptance.py::ScheduleReminderOfflineAcceptance
::test_a8_prompt_carrier_v3_reminder_section_anchors`

机制 = **内容驱动选节**（不按标题措辞定位，故标题改名/换级别都不误红）：把载体按
`##`/`###`/`####` 切成小节，对每节算「未满足必需项」清单，取最小者并要求其为空。必需项：

| 类 | 项 |
|---|---|
| 篇幅 | 小节正文 >= 400 字符（占位标题级过不了） |
| 硬锚（与确定性脚本有契约关系，非散文措辞） | `entry_key` / `bring` / `speech` |
| 语义规则组（每组 OR 候选措辞，抗润色） | 判定口径（要带/携带）· 区块范围（不提醒/不生成）· 反发明（凭空/严禁/不得编造/不要自己发明）· 语义产物（语义） |
| 三档取向 | `night`+前晚 / `morning`+早档 / `soon`+临期 **三条同时**在场 |
| 区块范围 | 学校课程表 / 每周固定课表 / 一次性安排 三词都在**同一节内** |

### 8.2 自证 = 重放当初暴露 vacuity 的场景（仓外临时副本，工作树零反例残留）

旧齿 = `test_a8_prompt_carrier_v3_landed_and_wired`（一字未动，保留作「载体在世 + 装载接线」半）：

| 变体 | 旧齿 | 新齿 |
|---|---|---|
| ours（本包版 181 行） | PASS | PASS |
| **main-only（单边丢弃本包约定）** | **PASS ← 假绿复现** | **FAIL ← 闭合** |
| union（双方并集） | PASS | PASS（不惩罚正确消解） |
| 本包版 + 提醒小节标题改名 | PASS | PASS（脆断言自查） |
| 单删 `entry_key` / `bring` / `speech` 任一模 | PASS | FAIL ×3（缺一即红） |
| 整组删「要带/携带」「不提醒/不生成」「凭空/严禁/不得编造/不要自己发明」 | PASS | FAIL ×3 |
| 删档位取向 `soon`+`临期` | PASS | FAIL |
| 只删一个同义词（保留 `不生成`） | PASS | PASS（润色容忍，符合设计） |

main-only 红因实写：`v3 载体内没有任何一节齐备提醒备注解析约定的必需项 —— 最接近的一节是
'### 家长 QQ 录入约定'，仍缺：实质篇幅…硬锚 'entry_key'…`（= 精确指向被丢弃的那一侧）。

### 8.3 同源审计答复（point 4：同一 needle 模式是否还在别处）

逐项审了两份冻结件的全部 needle：

- **有同类缺陷 1 处，已按同法收紧**：C 档 RMD-A1 tooth 7 的降级标记判据带
  `|| strstr(buf, "-1")` 逃逸支 —— 实测 `reminder_page.c` 里 `-1` 出现 **47 次**（每个防御
  分支都 return -1），且全树有 **136 个** `.c/.cc/.h` 文件「含 -1 但不含任何显式降级标记」，
  即该支恒真 = 与 carrier 宽词同一类无鉴别力。新齿
  `schedule_reminder_b.acceptance.c::提醒模块显式降级标记具名化 (RMD-A1 tighten)` 去掉该逃逸：
  只认 降级/degrade/fallback/友好提示 四种**显式**措辞任一（钉的仍是同一 V27 意图，
  不新增 acceptance id）。谓词实测：真模块 True（当下即绿，实现已含 12 处标记）→
  剥掉全部标记后 False（会咬），而旧 `-1` 支两种情况都 True（证明它看不见）。
  旧 tooth 未改（tighten-only 不可删），逃逸口因此只是「不再单独足够」。
- **无同类缺陷**：`test_e4_contract_point3_evidenced_and_poll_form_retired` 的 needle
  「已实证」在 `docs/contracts/interface-contract.md` 内**恰好只出现 1 次**（就是待确认点 3
  那一行），不是宽词；且 `docs/**` 不是多包抢的版本化槽位，无 add/add 单边丢弃面。
  `kidpal/alert`(4) / `重试到确认`(spec 2) 属「记录在世」守卫而非唯一性归因，风险等级不同，未动。
- 其余 needle 均为本包专有符号或包有文件名（`kidpal_reminder_receive(`、`kidpal/alert`、
  `reminder_page.c` basename、`openclaw-reminder.config.yaml`），无跨包共享面。
  负向齿（`schedule.md` / `pending.json` / `"20:30"`）方向相反（偏严不偏松），不属本缺陷类。

### 8.4 复跑实测（第四单后）

```
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
    → 35 passed, 5 skipped（0 failed）；--collect-only → 40 tests，零收集错误
cd server && python -m pytest -q --import-mode=importlib
    → 8 failed, 136 passed, 40 skipped        # 既有 8 红一条未动，且逐条同名：
                                              #   test_smoke_e2e ×7（包 S 退役架构面）
                                              #   + test_voice_roundtrip_b::test_a4_adherence_recheck_evidence_record ×1
node .claude/scripts/acceptance-coverage-check.mjs <实况路径副本>
    → ok:true | RMD-A1..A8 全在册 | structural ["RMD-A1"]
cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash espidf_wsl.sh build   # 无 -T main
    → rc=0, error: 0
    → ./build/host_infra_test.elf → 78 Tests / 3 Failures（既有三红未变；本包 13 齿全 :PASS）
git diff --numstat（两份冻结件）= 52 / 0 与 115 / 0；git diff -U0 删除行计数 = 0（invariant 1 成立）
```
