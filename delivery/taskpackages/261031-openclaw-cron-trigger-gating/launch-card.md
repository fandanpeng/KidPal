# 启动卡 · 261031-openclaw-cron-trigger-gating

跑 261031-openclaw-cron-trigger-gating（单包，非拆包产物，mergeOrder null）。

- **基线锚**（带锚引用三件齐；producer 五要素）：
  - server 面 = **370 passed / 7 deselected / 33 subtests passed / 0 failed** @ SHA `93b92a6b`（2026-10-31 本树实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`；面域 = server）。本包 base ref 即该 SHA，**消费差异判停不需要重跑**。
  - firmware 面 = **本包零 firmware diff**，不产 firmware 基线；若实现越界触 `firmware/**` 即停、回 planner 重判。
  - 本包验收红基线（acceptance-author 新冻结断言）恒现跑，无缓存。
- **本包 base ref**：prep 提交（含 plan / taskpackage / 本启动卡）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：同树最近三包 `261025-openclaw-dreaming-disable`、`261028-m6-k1-font-assets-runtime-loading`、`261028-p3-record-session-mirror-desync-fix` 均已 merged；intake 逐条评估其 pod-retro，构成 blockingDecision 即 fail-closed 上报。与本包直接相关一条：`261025-openclaw-dreaming-disable` 的部署面教训——**改 OpenClaw 运行时配置必须留可执行回滚件与 verify 比对**（本包已按 trigger 内联特性在 plan 里写了 `--clear-trigger` 回滚和 verify 内联文本比对）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，点火时与 main 同点 `93b92a6b`，历史 Pod 全 merged、无其它活跃 Pod 占用）；本包是 server/deploy 面，固件环境不参与验收；专属图索引 `C-AI-KidPal-pod-m2` 在册（nodes 20961 / edges 60602）。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理：Stop 的 `supervisor-gate-check.mjs`、PostToolUse `portfolio-sync` 自动 reconcile、permissions.ask 的 git merge/rebase；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  - 非 CLI 形态（VS Code 扩展 / IDE 内会话）降级面：已按 extension-session 记账，起步在 pod-events 记 `settings-presence`。
- **ECS live 相位归属**：本包 live 验收（`openclaw cron edit --trigger-script`、L1–L7、回滚演练）**归主线程/human 执行**；planner 到点即停，把命令清单 + 判据 + 预期输出写 `live-handoff.md` 交回，不改 live gateway。
- **判档记录**：`rigor: lite` / `riskTier: high`；human 2026-10-31 明确「不做 spike，直接按 Lite 实现」，显式覆盖 change-triage §2 单向守卫，裁决见 plan.md frontmatter `humanDecision`。

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{}}
