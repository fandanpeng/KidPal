# 红基线 · 260915-m1-k4-reminder-overlay-unify（intake 2026-09-17，acceptance-author 实测，planner 转写）

> 转写自 acceptance-author 回传全文（其 enforce-scope 写权仅 firmware acceptance 面，k1a 先例同构）。

## 两面重跑实测（launch 判停：两面均须重跑，已执行）

- **firmware host 面：26 Tests / 0 Failures / 0 Ignored，OK，EXIT=0**
  - 命令：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && ./build/host_infra_test.elf`
  - 缓存健康检查先行：CMakeCache = Ninja、指向本 worktree，一致（k3 pod-retro 教训已吸收）。
  - 计数成因：13/0 旧锚 @ e990eb2 之后 K2/N1b/K3 三包齿入 runner（+13）；schedule_reminder_b 经 k1a 批改后已归档退役；在册既存红 = 空（4 条随归档消红）。非致命告警：keybar_three_slot 两处 -Wunused-function 历史在世，error 为零。
- **server 面：0 collected，exit 0**（`python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive server/acceptance`；与锚 @ a398264 一致，现役仅 conftest.py）。

**有效基线 = firmware host 面 26/0/0 + server 面 0 collected。**

## C 类冻结验收实编（DEV-GOVERNANCE §4）

WSL gcc `-fsyntax-only -Wall -Wextra` error 为零（初版注释内 `KRM_STATE_*/` 提前闭合块注释已修复后通过）。

## 本包义务红（freeze 相位设计态红清单，解析性红定性）

- gate/hhmm 全仓零命中 → 新 seam 齿对现状必红。
- lcd_display.cc 零 KVST_SCENE_REMIND → 骨架消费齿必红。
- application.cc 零 RaiseReminderQueue → RM-A3 触发齿必红。
- mcp RaiseReminderQueue 区域旧卡面调用点在世 → 旧形态销账齿必红。
- 现状绿守卫齿（负向锚定前已 grep 实证为零）：RM-A7 零 lv_timer / 零 20:30·07:45 字面值；RM-A2 RAISE 列等行为齿对现状形态红。

## coverage 闸自证

`acceptance-coverage-check.mjs`（指 k4 taskpackage）= ok:true，RM-A1~A9 全认得、RM-A8 structural。

## 新 seam（builder 落点）

`reminder_page.{h,c}` + 接回 runner（CMakeLists 两行置于 smoke 之前）：
- `int kidpal_reminder_gate(int front_layer, int recording)`（KRM_GATE_RAISE=1 / DEFER=0 / INVALID=-1）
- `int kidpal_reminder_hhmm(const char*, char*, size_t)`（0=HH:MM / 1="--:--" 兜底 / -1 防御）
- 状态机九符号 weak 哨兵重钉（schedule_reminder_b 归档退役，RM-A4~A7 回归义务由本包验收文件承载）。

## fail-loud ②（设计取舍声明）

RM-A3 触发齿钉「application.cc 含 RaiseReminderQueue ≥1」且触发点路由不锁；若实现采用 mcp_server 侧查询录音态等其它形态，须返 planner/acceptance-author 复核返钉，builder/fixer 不得静默改齿。

## 清单外阅读申报（over-scan，14 项）

reminder_page.h、schedule_reminder_b.acceptance.c（archive）、kidpal_layer.h/.c、kidpal_voice_states.c/.h、mcp_server.cc（区段）、application.cc（区段）、waveshare-s3-rlcd-4.2.cc（dispatch 区段）、kidpal_render_node.cc（区段）、key_gesture_parser.h、art_assets.c/.h、HOST_TEST_CONVENTIONS.md、host_app/main/CMakeLists.txt、red-baseline/（unity stub）——逐项原因见 acceptance-author 回传表。
