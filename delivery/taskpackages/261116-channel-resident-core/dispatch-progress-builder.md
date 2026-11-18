# dispatch-progress · builder · 261116-channel-resident-core（真机轮 B 修复轮）

> 本文件 = 当前派发（真机轮 B 修复轮：纯逻辑缝 + application.cc 接线 + 变异证据）的
> 自包含进度快照。覆盖写；不载 tokens / gates / rounds。权威判据归 verifier。
>
> 2026-11-18 第二轮（planner 白盒复核收紧补丁）：会话内软复位 `reset_soft` + Listening
> case 按迁移条件分派。本文档已并入该轮结果。

## 本轮目标

在包边界内实现真机轮 B 三处修复（`plan.md`「### 真机轮 B 修复轮（2026-11-18）技术方案」
planner 裁定 6 = 承重修订）：判据本体迁入 host 可编纯逻辑缝 `session_voice_watch.{c,h}`、
`application.cc` 只留接线、D3 素材门换源到缝内 `asr_seen`，让冻结验收 SCR-A5/A8/A9–A15
转绿，既有回归网保绿。

## 已完成段落

### 1. 新缝 `firmware/main/session_voice_watch.{c,h}`（已落盘）

- 头文件：六个 `KIDPAL_VW_*` 宏（3/25/100/10/3/999）+ `KVW_ACT_NONE/IDLE_FIRE` +
  两个 typedef（cfg 四字段 / watch 七字段，与冻结件镜像逐字同形）+ 六函数原型；
  `extern "C"` 包裹、零 ESP-IDF / 零 FreeRTOS / 零 LVGL include（只 `<stddef.h>`）。
- 实现：`tick()` 的 `sustained`（连续段落在 [sustain, vad_trust] 闭区间）与
  `asr_fresh`（`asr_seen != 0 && asr_age_s < asr_fresh_s`，`asr_seen` 门承重）互补；
  `quiet_s` 无证据才 +1；`suspect = level_run_s > vad_trust`（纯审计位）；返回
  `quiet_s >= idle_timeout_s` 且到阈值后持续 FIRE（sticky）。`reset()` 七字段全零；
  `note_asr()` 置 `asr_age_s = 0` 且 `asr_seen = 1`；`cfg_default()` 只填三阈值，
  `idle_timeout_s` 保留调用方注入；`diag_due()` 决策表五支（非法 -1 / 回退 0 /
  周期 1 / 跳变且过 min_gap 1 / 否则 0）。cfg 数值 ≤0 = 该路证据不采信（更早收尾）。
- **第二轮新增 `kidpal_voice_watch_reset_soft()`**（additive，既有五函数语义零改）：
  清 `quiet_s / level_run_s / asr_age_s / tick_s / diag_last_s / suspect` 六件，
  **保留 `asr_seen`**；NULL -> no-op。头注写明存在理由（冻结件 `[S8]` 第六条与 EQ-4）：
  引导播报 TTS 造成 `Listening -> Speaking -> Listening`，回到聆听若走全清 `reset()`，
  本会话素材证据被清 0，D3 会把有素材的故事会话判成空素材、静置 60 秒不归档地释放。
- 缝内零 `SES_EVT_SESSION_TIMEOUT_60S` 字面（VQ-E1 口径保持 2 文件）。

### 2. 两侧 CMakeLists 登记（已落盘）

- `firmware/main/CMakeLists.txt`：`SOURCES` 末尾（`assets.cc` 前）additive 追加
  `"session_voice_watch.c"` + 包注释；既有行零重排。
- `firmware/main/test/host_app/main/CMakeLists.txt`：`HOST_ACCEPTANCE_SRCS` 在
  `host_infra_smoke.acceptance.c` **之前**追加 `"../../../session_voice_watch.c"` 与
  `"../../session_voice_watch.test.c"` + 包注释。

### 3. `application.cc` 接线（已落盘，`application.h` 零 diff）

1. include 区加 `#include "session_voice_watch.h"`；紧随其后落 seam 实例
   `s_voice_watch` / `s_voice_watch_cfg` 与诊断跳变快照四个 file-static。
2. `Initialize()` 内 `esp_timer_start_periodic` 之后：`kidpal_voice_watch_cfg_default`
   + `idle_timeout_s = kSessionIdleTimeoutS`（单源注入点）。
3. CLOCK_TICK 区、两 producer 之前：`kidpal_voice_watch_tick(&s_voice_watch,
   s_vad_speaking ? 1 : 0, &s_voice_watch_cfg)` + 诊断行块（`kidpal_voice_watch_diag_due`
   触发、周期 10s / 跳变受 3s 封顶、`ESP_LOGI` 全字段 `listening_diag` 行、emit 后写回
   `diag_last_s`；非 Listening 态零输出）。
4. A6 producer：判据改 `Listening && !kidpal_story_session_ctx_active() &&
   vw_act == KVW_ACT_IDLE_FIRE`；删 `!s_vad_speaking` 与 `clock_ticks_ >=` 两分量；
   审计行保留 `seconds=` 并追加 `quiet= asr= suspect= src=idle`。
5. D3 producer：判据改 `Listening && kidpal_story_session_ctx_active() &&
   !s_voice_watch.asr_seen && s_voice_watch.quiet_s >= kStoryEmptySessionTimeoutS`；
   `kStoryEmptySessionTimeoutS = 60` 定义位次零动；`!s_story_rec_entered` 退出判据。
6. stt 分支 `Schedule` lambda 首行：`kidpal_voice_watch_note_asr(&s_voice_watch)`。
7. `HandleStateChangedEvent` 的 `case kDeviceStateListening:` **尾部**：按迁移条件分派
   复位（第二轮收紧）——状态迁移监听器 `AddStateChangeListener` 内新增
   `s_watch_prev_state = old_state;`（file-static `DeviceState`，初值
   `kDeviceStateIdle`；写者 = 监听器，读者 = Listening case 尾部；与 `s_vad_speaking`
   同级宽松度）；case 内：
   `if (s_watch_prev_state == kDeviceStateSpeaking) reset_soft(&s_voice_watch);
   else reset(&s_voice_watch);`。之后 `s_vad_speaking = false;` 与 `src=enter`
   诊断行不变。落点选择理由：尾部不打断 RSL-A2 的「SendKidpalSessionCtx 先于
   IsAudioProcessorRunning 门」时序。**比较刻意写在 Listening case 内、两条调用均带
   实参字面 `(&s_voice_watch)`**（冻结件 EQ-4 / SCR-A15 形态要求；上移成监听器布尔会
   让 SCR-A15 红，不绕）。
- 零 diff 区确认：`MAIN_EVENT_VAD_CHANGE` 区、`case kDeviceStateIdle:` 区、
  `application.h`、`mcp_server.cc` 未触碰。

### 4. builder 单元测试 `firmware/main/test/session_voice_watch.test.c`（已落盘）

七条用例补冻结断言未穷尽的防御面：NULL 入参（含 tick 零写字段、含 `reset_soft(NULL)`）、
cfg ≤0 四路降级方向、`asr_age_s` 饱和 999 且非闩锁、`diag_due` 的 `now==last` /
周期恰等 / min_gap 恰等 / 回退 / 非法配置、可信窗上下界闭区间两侧边界、`asr_seen`
门的直接断言（第 1 tick 即红，比 SCR-A9 在第 85 tick 才红定位更快）、第二轮新增
**TEST 7 `reset_soft` 六清一保 + NULL 防御 + 硬复位仍全清**（冻结件 SCR-A13 已钉行为，
本条补 NULL 路径与逐字段定位粒度）。

## 实跑结果（第二轮终态）

| 面 | 命令（仓根起） | 结果 |
|---|---|---|
| host 全量 | `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` 后 `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf` | 编译通过；**321 Tests / 3 Failures / 0 Ignored**；3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4；SCR-A5/A8/A9/A10/A11/A12/A13/A14/A15 全 PASS；builder 七条全 PASS。**与派发书期望 320 的差 = 新增 1 条 builder 单测**（派发书允许）；去掉该条即 320/3 |
| 设备编译 | `node .claude/scripts/firmware-build-check.mjs` | `ok: true` / `phase: build-pass`；`session_voice_watch.c.obj` + `application.cc.obj` 构建并链接，`xiaozhi.bin` 0x329580（app 分区 8% free） |
| server 全量 | `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance` | **486 passed / 0 failed / 9 deselected / 33 subtests passed**（server 零 diff） |

## 变异证据（13 条全部实跑，新基线）

新基线 sha256（第二轮终态；13/13 `restore_sha256_ok = true`，终态重建后三件逐一吻合）：

| 文件 | 新基线 sha256 |
|---|---|
| firmware/main/session_voice_watch.c | 5ce7d18fec4d570682a99a93820b6ad25e3fd85773deb62afb12732fa7209ded |
| firmware/main/session_voice_watch.h | 80a94f4b718589368f207b0dbc6a2bbd02fb70917340df12b71dea65d8f5d503 |
| firmware/main/application.cc | 3800ab24eeb9c7163d2078dc513c189634f79e653fd9f0d16ec0f17911be13bf |

逐条：精确变异 -> 重建（seam/header 面；header 变异先删 `session_voice_watch.c.o` +
`session_channel_resident.acceptance.c.o` 强制重编）/ `application.cc` 源文本扫描直跑 ->
`host_infra_test.elf` -> 整文件二进制写回 + sha256 比对。期望红全部命中：

| # | 变异 | 转红（本包相关，含 co-red） |
|---|---|---|
| 1 | tick 收尾比较 `>=` -> `>` | SCR-A9 + SCR-A10/A11（builder asr_seen 门、非正 cfg） |
| 2 | sustained 删可信窗上界 | SCR-A10（kill-shot） |
| 3 | sustain 下界 -> `>= 1` | SCR-A11 + builder 窗口边界 |
| 4 | 头宏 SUSTAIN 3 -> 5（强制重编） | SCR-A11 + SCR-A13（cfg_default 同源 co-red） |
| 5 | `sustained \|\| (0 && asr_fresh)` | SCR-A12 |
| 6 | 头宏 VAD_TRUST 100 -> 30（强制重编） | SCR-A12 + SCR-A10/A13（co-red） |
| 7 | 头宏 DIAG_MIN_GAP 3 -> 0（强制重编） | SCR-A14 + builder diag_due |
| 8 | `reset()` 删 `asr_seen = 0` | SCR-A13 + SCR-A9/A11 + builder 四条 |
| 9 | 删 stt 分支 `note_asr` | SCR-A15② |
| 10 | A6 判据加回 `&& !s_vad_speaking` | SCR-A15④ + SCR-A5（双保险 co-red） |
| 11 | 诊断触发改 `clock_ticks_ % 10` | SCR-A14 + SCR-A8 + DLG-A4 三处同红 |
| **12** | **`reset_soft` 退化全清（`asr_seen` 也清 0）** | **SCR-A13 + builder TEST 7** |
| **13** | **Listening case 退化无条件 `reset(&s_voice_watch)`（删条件分派）** | **SCR-A15③（`reset_soft` 与 `kDeviceStateSpeaking` 双缺）** |

与上一轮（旧基线）对比无结果变化：#8 的 co-red 集合仍为 SCR-A13/A9/A11 + builder
（本轮 builder 多一条 TEST 7 同红 = 预期同向）；#2/#6 的 co-red 形态（#2 只打 SCR-A10、
#6 打 SCR-A12/A10/A13）与旧表一致。#12/#13 是新增覆盖，分别钉死「软复位退化」与
「条件分派被删」两种回归。

## 偏差与如实申报

1. host 用例数 321（派发书期望 320）：差 1 = 本轮新增 builder TEST 7（派发书允许补一条
   `reset_soft` 单测）。冻结核对面 320 条全绿，仅上游 AB-A1/A2/A4 存量红。
2. 变异驱动首版顺序写反（先重建后落变异），seam 行为断言一度全部漏红；已按
   「先落变异 -> 重建 -> 实跑」重跑全部 seam/header 变异，上表为准。首版还因文本模式
   读写把 `application.cc` 行尾从 CRLF 写成 LF；已按二进制写回，sha256 与基线逐字吻合。
3. 头宏变异（#4/#6/#7）沿用强制重编绕开首引入 depfile 时序缺口；该缺口是既有构建
   卫生问题，仍建议记 review-packet residual。本构建树上验收 TU 已能感知头文件变化
   （本次改头触发了 `session_channel_resident.acceptance.c.o` 重编）。
4. 真机端到端（引导播报后静置 60 秒不误释放、孩子已讲内容仍归档）仍归轮 B 重测；
   host 只钉判据与接线形态（与冻结件 GAP-4 同族）。

## 下一步

- 无实现段遗留；回执汇总交 planner（含 13 条变异全文、偏差申报）。