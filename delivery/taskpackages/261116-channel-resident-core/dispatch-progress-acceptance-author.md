# dispatch-progress · acceptance-author · 261116-channel-resident-core

> 本文件 = 当前派发（会话内素材证据跨 TTS 往返保活 · tighten-only 补丁）的自包含进度
> 快照。覆盖写；不载 tokens / gates / rounds / verdict。

## 本轮目标（一句话）

在已冻结的 `firmware/main/test/session_channel_resident.acceptance.c` 里，为「会话内
素材证据（`asr_seen`）在 Listening→Speaking→Listening 往返中不得丢失」补一条契约
（新增 `kidpal_voice_watch_reset_soft`）与对应断言，并实测红基线。

## 已完成段落

1. 输入通读：现冻结件全文 / `firmware/main/session_voice_watch.{c,h}` / application.cc
   的 `case kDeviceStateListening:` 区（复位调用在 1519 行、诊断行与 `s_vad_speaking`
   复位紧随其后）/ host_app 两侧 CMakeLists 编译口径。
2. 变更点（仅本验收件，零实现面改动）：
   - 头注 [S8] (5) 增补第六件 `reset_soft()` 语义（六清一保 + 跨会话仍走 `reset()`），
     并把接线形态改写为「存 `old_state` + 按旧状态分派」的描述。
   - 强度申报增补一行（软复位 -> SCR-A13 行为级 + SCR-A15 结构半 + 真机端到端留在
     真机轮 B 重测，与 GAP-4 同族）。
   - 头门控/哨兵段：共用原型块加 `void kidpal_voice_watch_reset_soft(kidpal_voice_watch_t *w);`
     （同型重复声明，头在场/缺席两分支都生效），并加 `__attribute__((weak))` 哨兵实现，
     哨兵**刻意把 `asr_seen` 清 0** -> 未实现态新断言必红。
   - SCR-A13 同 case 追加：脏置 `asr_seen=1` + 六件非零 -> `reset_soft` 后 `asr_seen==1`
     且其余六件全 0；再追一条「`reset_soft` 之后 `reset()` 七字段仍全 0」。
   - SCR-A15 ③ 同 case 追加三条：`kidpal_voice_watch_reset_soft(&s_voice_watch) >= 1`、
     `kidpal_voice_watch_reset(&s_voice_watch) >= 1`、`kDeviceStateSpeaking >= 1`；
     两条计数一律带实参（裸 `kidpal_voice_watch_reset(` 是 `_reset_soft(` 的前缀子串）。
     既有 `kidpal_voice_watch_reset(` 断言原样保留（只加不减）。
   - 文件尾 REVISION LOG 2026-11-18 tighten 段 + RED baseline 判读口径补实测数字。
   - 行为等价假设增补 EQ-4（「Listening 区内出现 `kDeviceStateSpeaking` ≙ 复位按迁移
     条件分派」= 钉 planner 冻结的接线形态；把比较上移到状态迁移监听器属语义等价写法
     但会在该断言下红，属冻结形态要求，非假红）。

## 红基线（本轮实跑）

- 命令（仓根起）：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build`
  → 编译通道 **error 0**（唯一 warning = 既有 helper `scr_exists` 未使用）。
  运行：`wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`
- 结果：**320 Tests / 5 Failures / 0 Ignored**（exit=5）。5 红 = 3 条上游存量红
  （p1_auxbar_style AB-A1 / AB-A2 / AB-A4）+ **恰 2 条本补丁设计态红**：
  - SCR-A13（1349）`reset_soft 后 asr_seen = 0（期望 1）`；
  - SCR-A15（1560）`Listening case 内 kidpal_voice_watch_reset_soft(&s_voice_watch) = 0 处（冻结 >=1）`。
  其余本包 case 全绿（A5/A8/A9–A12/A14 与 A15 既有六件），证明新断言只打自己那一处缺失。

## 转绿路径与变异敏感性（scratch，仓外 `/c/tmp/vwtight2`，不落仓）

- 转绿：参考实现（`reset_soft` 六清一保）+ 假树 application.cc（Listening case 按
  `s_watch_prev_state == kDeviceStateSpeaking` 分派）-> **15 cases / 15 passed**。
- 变异（每个都被杀死，且只杀对应 case）：
  - `reset_soft` 退化成全清（清 asr_seen）-> SCR-A13 红（1349）；
  - `reset_soft` 漏清 `diag_last_s` -> SCR-A13 红（1354）；
  - 接线退化成无条件软复位（丢跨会话全清路径）-> SCR-A15 红；
  - 接线退化成无条件全清复位（= 当前真树现状）-> SCR-A15 红（1560）。

## 下一步

- builder 落 `kidpal_voice_watch_reset_soft` 强符号 + application.cc 条件分派接线后，
  按同命令重跑，期望只剩 3 条上游存量 AB 红。