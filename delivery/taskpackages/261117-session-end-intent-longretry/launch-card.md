跑 261117-session-end-intent-longretry（261115-session-channel-resident，mergeOrder 78）。
基线锚：`delivery/workstreams/261115-session-channel-resident/progress.md` 2026-11-19 节（量测 HEAD `05417c1f`）——server 面可引用（`495 passed / 0 failed / 9 deselected`）；firmware host 面可引用（`322 Tests / 3 Failures`，3 红全为 `p1_auxbar_style.acceptance.c` 存量红 `ic_wifi_14` / `ic_thermo_12` / `ic_batt_15`）；两面对 `05417c1f` 之后的 diff 判停按 progress.md 指令（零差异引锚、有差异重跑该面）。
本包 base ref：`05417c1f`（= 点火时 `git rev-parse HEAD`、包首提交父；常驻分支 `dev/pod-m2`）——包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。
上一包留根：`delivery/taskpackages/261116-channel-resident-core/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省；final 相位恒跑）。
验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["docs/requirements.md#S-F1.4","docs/requirements.md#§2.1"],"hostUncompilablePaths":["firmware/main/application.cc","firmware/main/mcp_server.cc"],"reason":"真机轮 B 由 human 持板：说拜拜全链结束、告别不误伤、伪造防护、拔网线/重启 front 自愈、离线期提醒不丢、长跑期人工入口不僵死"}}
会话标识：VS Code 扩展内交互会话（planner 主线程，2026-11-19 启动；human 明确要求就地开发、不 headless）——叫停时核「进程确实已死」。
命令行（两形态并列，标注适用条件）：
① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：该形态不能传 `--settings` / `--agent` ⇒ 不加载 dev-loop.settings.json；Stop 的 supervisor-gate-check 门、PostToolUse 的 portfolio-sync 自动 reconcile、permissions.ask 的 git merge/rebase 三件按缺席处理，per-agent allow/deny 对主线程恒不生效。扩展形态起步须在 pod-events 记 settings-presence（declaredForm=extension-session；本包已记），并在收尾相位手工跑 supervisor-gate-check.mjs、board 手跑 reconcile。