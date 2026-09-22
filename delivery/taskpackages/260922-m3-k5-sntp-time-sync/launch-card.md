# Launch Card · 260922-m3-k5-sntp-time-sync

跑 260922-m3-k5-sntp-time-sync（m3-schedule，mergeOrder 37）。

- **基线锚**（差异判停 2026-09-22，锚 SHA = `b054660`，命令与落盘锚见 progress.md K5 launch 段）：
  - server 面：`git diff --quiet dc6094a HEAD -- server` 零差异 → 引用 K4 锚「server 95」（progress.md K4 launch 段，不重跑）；
  - firmware 面：`git diff --quiet 9cae452 HEAD -- firmware` 零差异 → 引用 K4 closeout「host runner 84 Tests / 0 Failures」（progress.md 第 3 追加轮段，不重跑）。
  - Pod intake 复核：面域有新差异 → 按锚行命令重跑该面；零差异 → 引用不重跑。本包验收红基线（新冻结齿）恒现跑，无缓存。
- **上一包留根**：`delivery/changes/dev/260921-m3-k4-p1-cjk-dynamic-text/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **本包特记**：taskpackage.json 曾截断损坏、launch prep 已修复补齐（commit `b054660`）；intake 时 planner 逐字段核对 taskpackage.json 与 plan.md 一致性（origin.notes 有登记）。input-brief 双件由 intake 的 acceptance-author / builder 派发时补齐（拆包侧未产，current/ 下 K4 残留已清）。
- **命令行**（两形态并列，按在场情况二选一）：
  1. 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  2. 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
