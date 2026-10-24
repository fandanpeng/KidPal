# 261024-qa-session-timeout-producer · plan.md（full · riskTier high · 技术语义权威）

TaskPackage: `delivery/taskpackages/261024-qa-session-timeout-producer/`
来源: 真机观察（2026-10-24 22:00 播报中 KEY 打断回聆听后设备永不收尾、麦克风常开）+ debt 账 `docs/debt/tech/TECH-261014-session-end-channels-unwired.md` 方向①
档位: ②单包 Full（architect `/triage-change` 裁决，human 触点① 确认）；`sourceType: regression`
需求锚点: `docs/requirements.md:42`（结束通道① 60 秒无语音）、`:47`（⚠️ 对账项）、`:57`（记录会话不适用 60 秒）
本 plan 为 architect reviewed plan 原样落盘，planner 只读消费、改需留痕。

## ① 改动意图（一句）

问答会话在「设备态 Listening 连续 60 秒无话音活动」时由设备侧自动结束会话（关 WS → Idle → LOW_POWER → 常驻重建），补齐 requirements §2.1 结束通道① 的设备侧 producer——修掉「KEY 打断播报回到聆听后设备永不收尾、麦克风常开直到手动 BOOT」的真机现象。

## ② 目标符号 / 文件锚（changeManifest 种子）

实现面（builder）：

- `firmware/main/qa_session_timeout.h`（新）：`KIDPAL_QA_SESSION_TIMEOUT_MS`（60000LL 单源）、`kidpal_qa_timeout_step_t`、`kidpal_qa_timeout_step()`、`kidpal_qa_timeout_event()`。
- `firmware/main/qa_session_timeout.c`（新）：纯逻辑实现（零芯片 / FreeRTOS / LVGL 依赖；include `session_state_machine.h` 取事件枚举）。
- `firmware/main/application.h`：`void EndVoiceSession(const char* cause = "overlay_close");`（原签名加默认形参）+ 私有成员 `const char* end_voice_session_cause_ = "overlay_close";`。
- `firmware/main/application.cc`：
  - `#include "qa_session_timeout.h"`；文件级 `static long long s_qa_last_activity_ms = 0;`
  - `MAIN_EVENT_CLOCK_TICK` 块（约 `:424-460`）尾部加超时判定：调 `kidpal_qa_timeout_step(now_ms, s_qa_last_activity_ms, GetDeviceState()==kDeviceStateListening, s_vad_speaking, kidpal_story_session_ctx_active())`，写回基线；`due` 时打 `session_timeout_fire corr_id=... elapsed_ms=... event=%d src=session_timeout` 并调 `EndVoiceSession("session_timeout")`。
  - `stt` 分支（约 `:845-871` 的 Schedule lambda 内）加一行基线刷新（VAD 盲区兜底）。
  - `EndVoiceSession(const char* cause)` 写 `end_voice_session_cause_` 后置位；`HandleEndVoiceSessionEvent()`（约 `:1128-1157`）settle 日志改 `src=%s` + `end_voice_session_cause_`。
- `firmware/main/CMakeLists.txt`：`SOURCES` 登记 `"qa_session_timeout.c"`（近 `:126` session_state_machine.cc）。

验收面（acceptance-author，builder 只读）：

- `firmware/main/test/qa_session_timeout.acceptance.c`（新）：判定真值表行为断言 + `application.cc` 接线结构断言 / 负锚 + 注册序守卫。
- `firmware/main/test/voice_round_ui.acceptance.c`：VQ-E1（TEST 5 tooth 2）`n==2 → n==3` + 第三文件正向钉；OCE-A1（TEST 12 tooth 2）`src=overlay_close` → `src=%s` + 默认 cause 字面正向钉。
- `firmware/main/test/host_app/main/CMakeLists.txt`：登记 `../../../qa_session_timeout.c` + `../../qa_session_timeout.acceptance.c`，置于 `host_infra_smoke.acceptance.c` 之前。

## ③ 预期行为差（before / after）

- **KEY 打断播报 → 回聆听 → 无人说话**：before = 永远停在 Listening，麦克风常开（直到底层通道超时）；after = 60–61s 后 `session_timeout_fire` → `local_close_settle src=session_timeout` → Idle + LOW_POWER + 5s 常驻重建。
- **多轮问答后孩子走开**：before = 常驻开麦；after = 60s 关闭。
- **记录会话静默**：before / after 均不触发（ctx 抑制）；自动收尾仍归网关侧卡壳 2 轮。
- **播报中**：before / after 均不触发（Speaking 不计时，长播报不被切断）。
- **连续说话跨 60s**：before / after 均不触发（VAD 每拍刷新基线）。
- **覆盖层关层（BOOT 短按 / KEY 长按）**：行为零变化；唯一可观测差 = settle 日志 `src` 由字面改为 `%s` 格式化，输出值不变。
- **其他板**：零变化（本包不触板级；非目标板的 ManualStop 聆听路径同规则计时）。

## ④ 目标区域既有覆盖态申报

- **有（must-stay-green）**：`voice_round_ui.acceptance.c` KVR-A4 tooth 2（板级 `app.EndVoiceSession()` 默认实参下源码零改动）；`boot_auto_connect.acceptance.c` RC-A2 tooth 6（`ScheduleResidentRebuild()` 恰 5）+ RC-B2 sites 四项——本包**不新增收尾调用点、不拆分 handler**，故不动。
- **必红须重钉（acceptance-author）**：
  - `firmware/main/test/voice_round_ui.acceptance.c:1263-1271` VQ-E1：文件计数 2→3 + 第三文件 = `qa_session_timeout.c`；
  - `firmware/main/test/voice_round_ui.acceptance.c` OCE-A1 tooth 2：`src=overlay_close` 字面 → `src=%s` + 默认 cause 字面。
- **无行为覆盖**：`Application` 主循环 / clock tick 不进 host 构建——`due` 判定归纯逻辑缝 host 断言，收尾时序与麦克风行为归**真机目检兜底**（申报）。

## ⑤ 爆炸半径预估

- 符号（Grep 近似，生产面）：新符号 inbound 0；`Application::EndVoiceSession` inbound 1→2；`HandleEndVoiceSessionEvent` inbound 1；`kidpal_story_session_ctx_active` 生产面 11 处既有（+1 读）；`SetDeviceState`（42 处）零改动。
- 文件：实现 5（含新 2 + CMake）+ 验收 3；预估 diff 约 130–190 行。
- 不触：`session_state_machine.*`、`device_state_machine.*`、协议层 / 契约面（零 wire）、板级、`docs/requirements.md`。
- 风险点：误切话语（VAD 失效）、记录语境误判、验收面连锁重钉、引擎慢答复 >60s、设备编译闸 degraded。

## ⑥ 实现约束（负锚）

- **不新增 timer**：不得 `esp_timer_create`；复用 clock tick（既有 `kidpal_log_sysinfo_due` 先例）。
- **不改主循环等待**：`xEventGroupWaitBits(..., portMAX_DELAY)` 保持不变。
- **不改纯模型** `session_state_machine.h/.cc`（含 MANUAL_RECORDING 超时边）。
- **不新增收尾调用点 / 不拆分 `HandleEndVoiceSessionEvent`**（否则连带 RC-A2/RC-B2 重钉）。
- 判定逻辑**只住纯逻辑缝**；`application.cc` 只做取参 + 写回 + 触发，不得就地复制判定。
- `60000` 字面只许出现在 `qa_session_timeout.h` 宏；调用点零裸写。
- 记录语境抑制**只做 producer 侧**；不引入记录语境行为变化。
- `EndVoiceSession` 默认实参必须保持 `"overlay_close"`（板级源码零改动）。

## ⑦ 判定规则（architect 裁决的单源规则）

**timer 形态**：复用既有 1 秒 `MAIN_EVENT_CLOCK_TICK`（`application.cc:249` `esp_timer_start_periodic(..., 1000000)` 常驻，`:424` 消费）+ 纯逻辑期限判定；同文件 `:452-461` 的 `kidpal_log_sysinfo_due` 是完全同构先例。**否掉**专用 one-shot timer（三线程活动沿重置、竞态面大、host 不可测）和主循环改周期等待（动整个 `Run()` 节拍，风险收益比最差）。

**只对 `kDeviceStateListening` 计时**：活侧 `DeviceState` 没有 THINKING 态（"思考中"只是 UI 状态），等待回包仍在 Listening；`stt` 到达会刷新基线，正常秒级答复不会误杀。

每 1s tick 用 `(now_ms, s_qa_last_activity_ms, listening, s_vad_speaking, record_ctx_active)` 调纯函数 `kidpal_qa_timeout_step(...)`，返回 `{due, new_last_activity_ms}`：

| 设备态 | VAD 此刻在说 | 记录语境 | 动作 |
|---|---|---|---|
| 非 Listening | 任意 | 任意 | 基线 = now（臂置；进入 Listening 后第一拍开始计时，误差 ≤1s） |
| Listening | 是 | 任意 | 基线 = now（持续说话永不误杀——每拍读现状，不依赖沿） |
| Listening | 否 | 是 | 基线 = now（记录语境抑制） |
| Listening | 否 | 否 | `now - 基线 >= 60000` → `due=1`（同时基线 = now 防同拍重复触发）；否则保持基线 |
| 任意 | — | — | `last > now`（理论时钟回退）→ fail-closed，不触发、基线 = now |

**附加写点（仅一处）**：`stt` 消息到达（`application.cc:845-871` 的 `Schedule` lambda 内）把基线刷成 now——VAD 盲区兜底（`stt` 由服务端 VAD 切句产生，等价"孩子刚说过话"）。两处写点都在主循环上下文，无跨任务写；读 `s_vad_speaking` 沿用既有宽松跨任务 bool 读（`application.cc:28` 注释已申报同级先例）。

**边界**：`>= 60000`（含边界）→ due；粒度 1s，实际关会话落在 60–61s 之间，符合"60 秒无语音"。触发后不重复：置 due 时基线已刷 now，且 `EndVoiceSession()` 事件位在下一轮主循环先于 `MAIN_EVENT_SCHEDULE` 处理，状态离开 Listening。

## ⑧ 记录语境抑制

`kidpal_story_session_ctx_active()`（`firmware/main/display/story_page.h:312`）命中时完全不注入 TIMEOUT——**只做生产侧抑制，不改纯模型**。`session_state_machine.cc` 的 MANUAL_RECORDING 超时边是"许可性"表（该状态下收到该事件合法），不是"生产侧必须产出"；requirements `:57` 的策略归 producer。抑制是纯逻辑缝的入参，不在 `application.cc` 里写第二个 if，保证"何时到点"只有一个判定源。

记录会话的自动收尾 = 卡壳引导 2 轮无回应（网关侧计数，requirements `:57` + architecture §5.2 场景④），设备侧只看不判。

## ⑨ 收尾路径

复用 `Application::EndVoiceSession()`（已在 main，姊妹包交付），把入口扩为带 cause 形参：

```cpp
// application.h
void EndVoiceSession(const char* cause = "overlay_close");   // 原：void EndVoiceSession();
// 私有成员（与 s_vad_speaking 同级跨任务宽松度，见 application.cc:28 申报先例）
const char* end_voice_session_cause_ = "overlay_close";
```

- `EndVoiceSession(cause)` 写成员后置 `MAIN_EVENT_END_VOICE_SESSION`；`HandleEndVoiceSessionEvent()` 的 settle 日志由 `src=overlay_close` 改为 `src=%s`。
- 超时路径（clock tick 内）调 `EndVoiceSession("session_timeout")`；板级 `GLOBAL_ACT_OVERLAY_CLOSE` 的 `app.EndVoiceSession()` 因默认实参**源码零改动**（KVR-A4 tooth 2 保持绿）。
- **为什么必须带 cause**：超时是高频常态关会话（每天每次弃置问答都会触发）。若复用原样，日志会天天写 `src=overlay_close`——把最常见的关会话原因整批误标成覆盖层关层，正是这条 debt 当初难以诊断的病根形态。`local_close_settle` 是收尾落定的唯一权威行。
- **代价（如实）**：OCE-A1 tooth 2 必红须重钉（`src=overlay_close` 字面 → `src=%s` + 默认 cause 字面）。RC-A2 tooth 6（`ScheduleResidentRebuild()` 恰 5）与 RC-B2 sites 四项**不动**——本包不新增收尾调用点、不新增函数。
- **否掉的替代**：专用 `HandleSessionTimeoutEvent()`（重复收尾序列、新增第 6 个 `ScheduleResidentRebuild()` 调用点，连带一天内两次重钉）；走 `ToggleChatState()`（日志 `src=toggle_chat` 同样误标，语义错位）。
- **并发**：cause 成员写者是板级回调任务或主循环，读在主循环 handler；人按 BOOT 与超时同帧时 last-writer-wins，两条都是"关会话"，token 取其一——可接受，注释申报。

## ⑩ 验收与回归

### 纯逻辑缝（host 可测，必做）

新模块 `firmware/main/qa_session_timeout.{h,c}`（家 = `firmware/main/`，与 `session_state_machine.*` 同级；纯 C、零芯片 / FreeRTOS / LVGL 依赖，设备树与 host_app 双侧同源）：

```c
#define KIDPAL_QA_SESSION_TIMEOUT_MS 60000LL   /* 冻结数值单源，调用点禁裸写字面 */
typedef struct { int due; long long last_activity_ms; } kidpal_qa_timeout_step_t;
kidpal_qa_timeout_step_t kidpal_qa_timeout_step(long long now_ms, long long last_activity_ms,
                                                int listening, int vad_speaking,
                                                int record_ctx_active);
int kidpal_qa_timeout_event(void);  /* 返回 (int)SES_EVT_SESSION_TIMEOUT_60S —— 生产侧事件语义锚 */
```

`qa_session_timeout.c` include `session_state_machine.h` 并使用枚举 → **这是 `SES_EVT_SESSION_TIMEOUT_60S` 的第三文件**，VQ-E1 重钉的实际对象。活侧日志用它打 `event=%d`，把"生产 → 模型事件"的可追溯链钉死（不建影子状态机实例）。

### VQ-E1 重钉最小形态

`voice_round_ui.acceptance.c` TEST 5 tooth 2（`:1263-1271`）：`n == 2` → `n == 3`；追加正向 `kvr_file_contains("firmware/main/qa_session_timeout.c", "SES_EVT_SESSION_TIMEOUT_60S")`；注释 / coverage 行改写成「已接线（261024-qa-session-timeout-producer）；防扩张语义不变（仍恰 3 文件）」+ REVISION LOG append。**重钉而不退役**：防扩张功能（不许第 4 个引用）仍有价值。

### 新验收文件 `firmware/main/test/qa_session_timeout.acceptance.c`

1. **行为断言（host，真值表）**：非 Listening 不触发且刷新；VAD speaking 不触发且刷新；记录语境不触发且刷新；`elapsed = 59999` 不触发；`elapsed = 60000` 触发；`elapsed > 60000` 触发；时钟回退 fail-closed；`due` 同时基线刷 now。
2. **结构断言（源锚代理档，声级同姊妹包 OCE-A1）**：`application.cc` 的 `MAIN_EVENT_CLOCK_TICK` 区含 `kidpal_qa_timeout_step(`、`kidpal_story_session_ctx_active()`、`s_vad_speaking`、`EndVoiceSession("session_timeout")`、`session_timeout_fire`；**负锚**：`application.cc` 全文 `esp_timer_create` 计数不变、clock tick 区无裸写 `60000` 字面、不新增 `ScheduleResidentRebuild()` 调用点。
3. **注册序守卫**：本文件 + `qa_session_timeout.c` 进 `HOST_ACCEPTANCE_SRCS` 且在 `host_infra_smoke.acceptance.c` 之前。

### 回归

- host 全量：零新增红，基线锚 = planner intake 时实跑取真值（不引用二手数字）。
- 设备编译闸：本包新增一个 `.c` 进 `firmware/main/CMakeLists.txt`，漏登记 = 链接错误——必须真跑一次或如实申报 degraded，不得静默。

### 真机目检最小项（human 持板，live-manual structural 豁免，同姊妹包 OCE-A4 范式）

1. **复现原报告路径**：KEY 打断播报 → 回聆听 → 不再说话 → 60s 后关会话（日志序 `session_timeout_fire` → `local_close_settle src=session_timeout` → 常驻重建），麦克风关、回常显。
2. 多轮问答后停口 60s → 同样关（日常主场景）。
3. **记录会话静默 >90s → 不得关**（ctx 抑制），且不干扰网关侧 2 轮卡壳收尾。
4. **连续说话跨 60s 边界 → 不得被切**（VAD 刷新）。
5. 唤醒词入口起的会话同样 60s 关。
6. 关会话后 `kResidentRebuildDelayS`（5s）内常驻重建恢复（既有路径零回归）。

## ⑪ spec 面后续账（不在本包改）

落地后 `docs/requirements.md:47`「真机 60 秒自动关闭从未发生」成为事实错误，需改写为「设备侧问答会话 60s 已接线；引擎侧 24h 兜底保留为后手」；`docs/architecture.md:138` / §6.4 需一句「设备侧问答 60s 已接线」。这是 spec 面，`docs-delta` 无权改：本包 docs-delta 显式申报过时面，落地后开跨面 intake（触面 = 需求 + 架构）或两个轻 loop。证据锚 = 本包 commit + 真机日志。

`docs/debt/tech/TECH-261014-session-end-channels-unwired.md`：planner 收尾相位 append「方向①已交付、剩方向②」，**status 维持 deferred，不关账**。

## ⑫ out-of-scope 观察

- 方向②「结束意图（拜拜/不用了）云端判」保持 deferred：引擎侧新 wire 语义命中跨系统结构性升级判据，设备侧本地判与 requirements `:43`「云端判」口径冲突，均非实现面单包能裁。
- `TECH-261014-post-speak-listening-lingering` 不因本包变化。
