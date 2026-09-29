---
id: ARQ-260929-agent-def-reality-drift
title: agent def 与 scope-map 文本漂移——docs/systems 旧 spec 路径、builder apps/sX-*/zod 上仓遗留、challenger 影子路径写法、S2/K8 死条文
domain: gov
status: blocker
severity: SHOULD
owner: architect
trigger: 下一治理批（/arch-change 入口；与 ARQ-260929-agent-def-provenance-noise、ARQ-260923-planner-roster-reference-drift 同车成批）
opened: 2026-09-29
closed: ""
closedBy: ""
hardExclusions: true
paths: [.claude/agents/architect.md, .claude/agents/review-moderator.md, .claude/agents/architect-reviewer.md, .claude/agents/builder.md, .claude/agents/challenger.md, .claude/agents/fixer.md, .claude/agents/integrator.md, .claude/agents/planner.md, .claude/scope-map.json]
source: "主线程 2026-09-29 agent def 全量核查（16 个 def 逐文件人读 + 机检对账；route-regression-check.mjs 36 ok / lint-descriptions.mjs 1 warn 基线；census 命令与输出见正文证据段）"
---

**证据**（2026-09-29 实测，grep 命令 verbatim）：

- `ls docs/` 无 `systems/` 目录；`git log -- docs/systems` 顶端 commit `ed2b353`「refactor(docs): ui-to-lvgl-pitfalls 移出 systems/kidpal 层」——docs/systems 已在去层重构中删除，现行 spec 面 = `docs/requirements.md` + `docs/architecture.md`（AGENTS.md「自有代码与参考代码」节 2026-09-13 注记）。
- `grep -rn "docs/systems" .claude/agents/ .claude/scope-map.json` 共 16 行命中：architect.md:29/:53/:65（:53 同句自相矛盾——「`docs/systems/**`（canonical spec = docs/requirements.md + docs/architecture.md）」）；review-moderator.md:20/:66/:73/:90；architect-reviewer.md:42（将来多系统展开注记，无害）/:128/:146（输出示例引 `docs/systems/...`，与同文件 :42 已改单系统的新旧并存）；scope-map.json:83（integrator.allow）/ :88（architect.deny）/ :102（review-moderator.allow）/ :119（adjudicator.deny）。
- **机检实害**：scope-map integrator.allow 含 `docs/systems/**` 且不含 `docs/requirements.md` / `docs/architecture.md`——integrator 做 docs-sync 写 canonical spec 会被 enforce-scope 拒（`universal.docsTopLevel` 只管 docs/ 根新建白名单，不授 agent 写权，`.claude/hooks/enforce-scope.mjs:56-66` 实读确认）；spec 面路径类断言（比对 spec↔contract、architect 起草落点）会被引到不存在路径。
- `grep -rn "apps/sX-\|zod" .claude/agents/` 命中 builder.md:3（description——模型级路由信号）/:17/:56：`apps/sX-*` 包形状本仓从未存在（scope-map 实现面 = firmware/server），`zod` 非本仓校验栈。
- `grep -n "contracts/" .claude/agents/challenger.md` :30/:77 写裸 `contracts/`（无 docs/ 前缀）——恰是 `docs/contracts.md` rule 禁的根级影子目录路径写法。
- S2/K8 死条文：builder.md:38/:55、fixer.md:36/:53、integrator.md:39/:40/:41/:102/:128、planner.md:53 引 `run_inline_check` / `get_run_results` / `write_knowledge` / `knowledgeSuggestions`——本仓无对应 MCP 工具（K6/correlationId 经 observability-k6.md 本地化过，不在本条范围）。

**影响面**：① integrator docs-sync 写 canonical spec 被钩子拦——下次触 spec 文档同步的 dev 包必现一次返工；② spec 面断言与起草落点引到已删路径，误导对抗 review 与契约预审；③ builder description 是 orchestrator 模型级路由信号，包形状写错误导派发判断；④ S2/K8 死条文诱导 agent 找不存在的工具或自适应跳过。

**建议改动**（候选，交 review 不自批）：① 全部 `docs/systems` 引用改指 `docs/requirements.md` + `docs/architecture.md`；scope-map 同步——integrator.allow 增两个 spec 文件并去 `docs/systems/**`，architect.allow / review-moderator.allow 换路径，adjudicator.deny 换路径；② builder.md description 与 When-to-Use 包边界改「TaskPackage subsystem 面 + changeManifest」表述、删 zod 句；③ challenger.md :30/:77 统一 `docs/contracts/**`；④ S2/K8 死条文删除或按本仓现实改写（删 vs 改写交 review/OQ 定夺）。

**风险**：riskTier: medium——scope-map 属 G3 强制层，写域变更 = 权限面变更，必须 human 过目；def 文本部分机械可验。review profile：**full-adversarial + human（Track A）**——触 G3（scope-map 写域）+ mixed 面（G2 def × 7 + G3 scope-map），不美化成快路径。便宜验证：复跑 `route-regression-check.mjs` / `lint-descriptions.mjs` + `grep -rn "docs/systems\|apps/sX-" .claude/agents/ .claude/scope-map.json` 断言零命中。

**更正注记（2026-09-29，260929-gov-review-agent-def-alignment 回灌定稿期 F16 修复集 append；不改上方既有段）**：① 证据段「共 16 行命中」实为 **14 行**（agents 10 + scope-map 4；challenger F16 复核、adjudicator CONFIRMED，PROPOSAL T1 证据行同步改 10）。② 证据段 scope-map 命中标注「:88（architect.deny）」有误——:88 实在 **architect.allow** 块内（scope-map:87 `"allow": [` 起；PROPOSAL T2 的标注 architect.allow 正确）。按账本纪律 append 更正、既有行不覆盖。
