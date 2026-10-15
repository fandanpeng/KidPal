# KidPal（小伴）— AI Native 治理指令根

@AGENTS.md

## Claude Code 专属

- Claude Code 读 `CLAUDE.md` 不读 `AGENTS.md`；本文件用 `@AGENTS.md` 导入既有规则，单一源、不复制。路由 / 规则变更落在 `AGENTS.md`、经 `@AGENTS.md` 传导到本文件；仅 Claude-Code-专属注记直接写本文件。
- 在 `docs/contracts/**` 下改动前先进 plan mode——契约是单一事实源，改动走 `/arch-change` loop（261015-gov-review-face-loop-unify：契约变更 = 架构变更的一种，无人签门；防线保留：Bash 重定向写恒拦、实现面 agent 与 dev-loop 会话的契约禁写不动；合并过机械凭据闸）。
- 路径作用域规则在 `.claude/rules/`；碰 `docs/contracts/**` 时自动加载对应 rule。
