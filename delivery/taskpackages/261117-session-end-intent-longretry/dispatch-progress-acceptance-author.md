# dispatch-progress · acceptance-author · 261117-session-end-intent-longretry

> 自包含快照（Write 覆盖语义；不载 tokens / gates / rounds / verdict）。

## 快照 2026-11-19 · 段 1：新冻结验收件落盘

已落盘：
- `firmware/main/test/session_end_intent_longretry.acceptance.c`（SEI-A1..A4 + SEI-C1 注册序守卫）
- `server/acceptance/test_session_end_intent.acceptance.py`（SEI-B1..B4）

状态：
- 新 firmware 件 standalone 实编零 error / 零 warning（`gcc -std=c11 -Wall -Wextra -I. ... runner.c`，COMPILE_EXIT=0）。
- 新 server 件 pytest：6 红（B1/B2×2/B3×2/B4 farewell）+ 1 绿（B4 三面相等 must-stay-green）。
- **阻塞项（待 planner 派 builder「注册-only」步）**：新 firmware 冻结件尚未登记进
  `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS`（不在
  acceptance-author 写权面）。注册行原文（须置于 `../../host_infra_smoke.acceptance.c` 之前）：

  ```
      "../../session_end_intent_longretry.acceptance.c"
  ```

  注册后 host runner 才会跑本件 SEI-A1..A4 + SEI-C1（当前仅通道② standalone 红基线可跑）。

## 快照 2026-11-19 · 段 2：既有冻结件改动落盘

- `server/acceptance/test_record_dialogue_guide.acceptance.py`：RDG-A5 self_push=True +
  end_session 由 assertFalse 改 assertTrue（frozen-inversion），relay 缺省 assertFalse 保绿。
- `firmware/main/test/boot_auto_connect.acceptance.c`：K6-A2 tooth 6 收紧 5<10<20<40<60；
  tooth 7 由钉 `[3]` 长度改钉 `kAutoConnectSteadyRetryS` 首定义含 60（frozen-rebaseline）。
- `firmware/main/test/session_channel_resident.acceptance.c`：SCR-A2 加固（守卫在场 +
  pending_manual_intent_ 置位 + 审计行 manual_intent_queued）。

## 快照 2026-11-19 · 段 3：红基线实跑

- server: `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`
  → **495 passed / 7 failed / 9 deselected**（7 = RDG-A5 反转目标 1 + SEI-B 6；零连带红）。
- host: `espidf_wsl.sh build` 过 + `./build/host_infra_test.elf` →
  **322 Tests / 5 Failures**（3 上游 p1_auxbar_style AB-A1/A2/A4 + K6-A2 + SCR-A2 两条本包设计态红）。
- 新 firmware 件 standalone 通道②（未注册前）→ **5 Tests / 5 Failures**（SEI-C1/A1/A2/A3/A4 各首红）。
- `server/tests/test_points_spike_units.py` + `server/tests/test_archive_guard_units.py` → 61 passed（must-stay-green 佐证）。