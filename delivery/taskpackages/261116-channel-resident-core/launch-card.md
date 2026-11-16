跑 261116-channel-resident-core（261115-session-channel-resident，mergeOrder 77）。
基线锚：`delivery/workstreams/261115-session-channel-resident/progress.md`（量测 HEAD `214e7a2e`）——server 面可引用（`457 passed / 0 failed`）；firmware 面可引用（`301 Tests / 3 Failures`，3 红全为 `p1_auxbar_style.acceptance.c` 存量红 `ic_wifi_14` / `ic_thermo_12` / `ic_batt_15`）；两面对 `214e7a2e` 之后的 diff 判停按 progress.md 指令（零差异引锚、有差异重跑该面）。
本包 base ref：点火时 `git rev-parse base/261115-session-channel-resident`（= 包首提交父；常驻分支 `dev/pool-channel`）——包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。
上一包留根：无（本 workstream 首包）。
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开）。
验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["docs/requirements.md#S-F1.4","docs/requirements.md#§2.1"],"hostUncompilablePaths":["firmware/main/application.cc"],"reason":"真机轮 A 由 human 持板验证会话收尾、通道保活、提醒出音与温/冷通道时延"}}
验证形态申报修正（2026-11-16，收尾相位 planner，留痕）：原申报含 `firmware/main/application.h`，合并门判据报 `device-outcome-host-path-unregistered`（该路径不在 `.claude/verify-gates.json` deviceOutcomeGate.hostUncompilablePaths 登记集）。**修正 = 只列已登记的 `firmware/main/application.cc`**：行为本体承载在 .cc，头文件单独不构成独立设备结果面；用户可见结果与真机轮判据**零弱化**（轮 A A1–A13 仍为必需）。未改动 verify-gates 登记集（保护面，归主线程）。
会话标识：Git Bash pid 1807（`claude -p … --agent planner`，启动 2026-11-16 22:03:14）——叫停时核「进程确实已死」
环境事实：本树已完成固件环境等价化（sdkconfig / dependencies.lock 已复制，managed_components 为指向 pod-m2 树的 junction）；专属图索引项目 = `C-AI-KidPal-pod-channel`（full）。
命令行（两形态并列，标注适用条件）：
① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：该形态不能传 `--settings` / `--agent` ⇒ 不加载 dev-loop.settings.json；Stop 的 supervisor-gate-check 门、PostToolUse 的 portfolio-sync 自动 reconcile、permissions.ask 的 git merge/rebase 三件按缺席处理，per-agent allow/deny 对主线程恒不生效。扩展形态起步须在 pod-events 记 settings-presence（declaredForm=extension-session），并在收尾相位手工跑 supervisor-gate-check.mjs、board 手跑 reconcile。