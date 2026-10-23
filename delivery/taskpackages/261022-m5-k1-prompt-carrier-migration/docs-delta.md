# docs-delta · 261022-m5-k1-prompt-carrier-migration

- **docsSyncRequired = false**（taskpackage.json `docsSyncReason`）：架构面 §5.5.5 / 三处承载声明口径 / ADR-001 重定义已由 261022 arch 批 FROZEN v1.1 landing（commit `0411e957`）落稿；本包只落实现面与部署资产。**本包 docs/** 零 diff（builder 已核契约与 docs 零改；integrator 复核 `git status` 证实 docs/** 无改动条目）。契约 §1.6 承载对账注记零改；`docs/contracts/**` 全程只读。
- 记忆守则内容回填 §6.7 的实测成形句归 P3（`261022-m5-k3-honcho-memory-production`）收口，不在本包。

## 声明性注记（zero-diff 结论之外如实登记；不改任何 docs 文件）

### 注记 1 · §5.5.5 机制口径窄漂移（命名归属级，建议随 P5 小批措辞修正）

实现定形为「原地改写」：动态 context **内容源** = `server/deploy/kidpal-agent-prompt.txt`（含 `{{schedule_summary}}` 等回合内事实占位，有界、零静态纪律）；`server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt` 为引擎**装配壳**（`{{base_prompt}}` 包裹，无内容占位）。`docs/architecture.md` §5.5.5 把装配壳描述为「经 overlay 渲染进 prompt 键」的动态模板——与实现的「内容源 + 装配壳」两件分工存在命名归属级措辞漂移。属声明性窄漂移，不影响行为与机判面；建议随 P5（`261022-m5-n2a-schedule-mechanical-injection`，同触动态块面）做小批措辞修正。本包不动 canonical docs（integrator 对 spec 面无写权，`ARQ-260929-docs-sync-spec-handoff`）。P5 注入落点注记已由 planner 落 `261022-m5-n2a-schedule-mechanical-injection/plan.md` 跨包事实节。

### 注记 2 · v11 声明面保留原因（后续包勿当废弃清理）

`test_char_meaning_merge` 守卫仍解析 provider `system_prompt` 三键（artifact / source / deploy_target）与 overlay `PROMPT_SRC` 指向 v11。provider/overlay 中 v11 保留为「冻结终点版 / data 存档」声明面（`OPENCLAW_SYSTEM_PROMPT_CARRIER` 承载反转节同口径：不再 bump、不再注入引擎 prompt 键，overlay 仅拷入 ECS `data/` 存档）是**刻意保留**，供历史可 diff 与版本无关探针守卫解析——后续包（P2/P3/P4/P5）勿当废弃清理。
