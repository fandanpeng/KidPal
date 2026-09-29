# Launch Card — 260924-m4-k2-p3-star-notebook-ui

跑 260924-m4-k2-p3-star-notebook-ui（m4-star-notebook，mergeOrder 39）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-09-29 · K2 launch prep」段带 SHA 行——server 面 117 passed / 0 failed（server/acceptance；全套 offline 193P 同日佐证）+ firmware host 面 107 Tests / 0 Failures，均 @ 5575a23（本树实测）。intake 差异判停：launch 批仅触 delivery/ 治理面 → `git diff --quiet 5575a23 HEAD -- firmware/` 与 `-- server/` 预期零差异；零差异即引用锚行不重跑，有差异重跑（命令同锚行）。
- **上一包留根**：`delivery/changes/dev/260924-m4-k1-story-cloud-link/pod-retro.md` + `delivery/changes/dev/260928-story-connect-push-defer/pod-retro.md`（story 链直接前驱，K2 crossPackageBehaviors 逐条引用），intake 逐条评估（构成 blockingDecision → fail-closed 上报）；最近 merged 的 `260929-char-window-v5-trim/pod-retro.md` 为固件面旁支参考。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：契约三触点（§1.3③ story_list 形状 / §1.3④ story_text seq / §四 kidpal_story_delete）已随 K1 落地进现文，K2 纯消费零改契约；story_page.h 旧 FROZEN OFFLINE CONTRACT 头注 = 已退役的 offline 契约，重写页内输入表按 v1.47 矩阵勿误守（taskpackage notes 边角①）；拆包侧 input-brief 已按新拆包规撤回，Pod planner Step 7 自产。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
