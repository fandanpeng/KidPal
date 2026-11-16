# dispatch-progress · acceptance-author · 261116-channel-resident-core

## 已完成（全部 4 段）

### 1) 新增 host 件
- `firmware/main/test/session_channel_resident.acceptance.c`（SCR-A1..A7 七个 TEST_CASE；
  头注 seam contract + 强度申报 + GAP/EQ 显式化）。
- `-fsyntax-only`（compile_commands.json flags）**EXIT=0**，验收文件本身可编译。
- 未登记进 runner（登记权在 builder）；设计态红映射见交付报告。

### 2) 新增 server 件
- `server/acceptance/test_session_channel_resident.acceptance.py`（SCRF-B1/B2/B2b/B3/B4/
  B4b/B5/B6 共 11 个 test method；offline fastapi stub + importlib 按路径装载；
  心跳短周期以 env `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S` 注入）。
- 单跑：5 failed（设计态红）/ 10 passed（must-stay-green + 回归守卫）。

### 3) 既有冻结件重锚（四件）
- `firmware/main/test/boot_auto_connect.acceptance.c`：RC-B2（TEST 11）/ RC-A3（TEST 9）
  整条退役（墓碑 + #if 0）；RC-A2 tooth6 由「恰 5」重钉为「恰 0」（回潮守卫）；
  头注 coverage map / coverage 行同步更新。语法实编 EXIT=0。
- `firmware/main/test/voice_round_ui.acceptance.c`：OCE-A1 tooth2 两条正锚
  （CloseAudioChannel/ScheduleResidentRebuild）改 `SendStopListening()` + 三条负锚；
  tooth3 三位次序末位改 `SendStopListening()`；coverage 行同步。语法实编 EXIT=0。
- `firmware/main/test/record_session_live.acceptance.c`：仅 REVISION LOG 加一条
  frozen-rebaseline 留痕（文本仍绿、语义前提失真、零齿改动）。
- `server/acceptance/test_m4k3_record_session_live.acceptance.py`：`:508` docstring
  前提精确化 + 新增释放边沿姊妹齿 `test_a7_ctx_release_edge_after_record_context_seen`
  （+0 删弱既有断言）。单跑 1 failed（新姊妹齿设计态红）/ 10 passed（既有全绿）。

### 4) 红基线实测
- host（执行目录 `firmware/main/test/host_app`）：**299 Tests / 5 Failures / 0 Ignored**。
  5 红 = 3 上游已知红（p1_auxbar_style AB-A1/A2/A4）+ 2 本包设计态红
  （boot_auto_connect RC-A2 tooth6: ScheduleResidentRebuild 5->0；voice_round_ui
  OCE-A1: SendStopListening 缺失）。build EXIT=0。
- server（仓根）：**6 failed / 467 passed / 9 deselected**。
  6 红 = 新件 5 设计态红（B1 gate / B1 emit / B2b sweep release / B4 accumulate /
  B4 chat-marked）+ m4k3 姊妹齿 1 设计态红。既有 457 全绿零回归。

## 落盘路径
- `firmware/main/test/session_channel_resident.acceptance.c`（新件）
- `server/acceptance/test_session_channel_resident.acceptance.py`（新件）
- `firmware/main/test/boot_auto_connect.acceptance.c`（重锚）
- `firmware/main/test/voice_round_ui.acceptance.c`（重锚）
- `firmware/main/test/record_session_live.acceptance.c`（留痕，零齿改动）
- `server/acceptance/test_m4k3_record_session_live.acceptance.py`（docstring + 姊妹齿）

## 待 builder / planner
- builder 在 `firmware/main/test/host_app/main/CMakeLists.txt` 追加
  `"../../session_channel_resident.acceptance.c"`（置于 `"../../host_infra_smoke.acceptance.c"`
  之前），首次登记后补跑 runner 级红 + mutation。