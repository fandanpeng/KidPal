# architect-landing-summary · 260929-gov-review-agent-def-alignment

packet-id：FROZEN PROPOSAL §2 改法集（T1–T9）+ §5 验证集 · 评审轮次 R1（full-adversarial，六关 GREEN，结论「可批准」）· change-id 260929-gov-review-agent-def-alignment

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 docs/systems 旧路径清扫 | `.claude/agents/architect.md` ×3、`review-moderator.md` ×4、`architect-reviewer.md` ×3 | 平铺表述替换完毕；F4 地板两句（「写入不等于 spec 已冻结」「冻结判定归 review-moderator spec profile」）原样保留 |
| T2 scope-map 写域对齐（G3） | `.claude/scope-map.json` | integrator.allow 去 `docs/systems/**` 零增（零写权裁定）；architect / review-moderator.allow 换授两 spec 文件；adjudicator.deny 换两 spec 文件 |
| T3 builder 包边界表述 | `.claude/agents/builder.md` | description 与 When-to-Use 改 TaskPackage subsystem 面 + changeManifest；zod 句删除 |
| T4 challenger 影子路径 | `.claude/agents/challenger.md` | 两处统一 `docs/contracts/**`，:27 斜杠列表写法改顿号 |
| T5 S2/K8 全链删除（OQ2=(a)） | builder / fixer / integrator / planner / task-intake / build-feature / dev-loop | 死条文全删；integrator Step 5/6 删除后 7–12 步重排为 5–10（外部仅引用 integrator Step 4，不受影响）；planner:193-195 示例字段三行删 |
| T6 uiux-author 补编制（OQ1=(b)） | `.claude/agents/uiux-author.md`（新增）+ scope-map 增条目 | def 按 task-intake L2 职责面起草（翻译者定位、sonnet 档）；planner.md:61 uiux-designer 加「尚未接线」限定 |
| T7 过程修改记录清理 | planner / acceptance-author / portfolio-planner / integrator / builder / fixer / architect / review-moderator / architect-reviewer / adjudicator / defender / landing-verifier | 溯源注记删（census 62 → 白名单外零命中）；defender.md:47 trim-not-delete（must-never 句保留）；[MODEL-TIER]/[DISPATCH-STAMP]/[JOURNAL-APPEND]/FS-EVIDENCE 运行时合同段按 PROPOSAL 白名单整段不动 |
| T8 能力透镜锚（OQ5=(a)） | acceptance-author.md 6b 行、portfolio-planner.md 2a 行 | 按定稿措辞落字，行尾语义权威引用去重（F15） |
| T9 域外同构残留清扫（E4=(a)） | arch-change / review-arch / build-feature（commands）、arch-review-protocol / architect-operating-model / security-hardening / dev-loop / task-intake / arch-review-rubric（references）、brainstorming / gov-arq（skills）、surface-alignment.json | 全量重扫清单逐项改写；task-intake:338 地板件改指 `docs/ui/UIUX-DESIGN-BRIEF.md`；surface-alignment.json 与 scope-map 镜像同步（lastReviewed 2026-09-29） |
| F8 routing.json 补牙 | `.claude/route-fixtures/routing.json` | builder / planner / landing-verifier 三行 signal 断言 + planner Agent() 白名单行；route-regression-check 36→54 断言全过 |

## 2. 验证证据

- **族谱复扫（§5.1）**：`grep -rnE 'docs.{0,2}/.{0,2}systems|systems.{0,3}/.{0,3}(spec|kidpal)' .claude/ --include=*.md --include=*.json`（排除 route-fixtures 镜像豁免面外为零；镜像本身已同步）→ 零命中。
- **§5.2**：`grep -rn "docs/systems" .claude/` → 零命中。
- **§5.3**：`grep -rn "apps/sX-\|zod" .claude/{agents,commands,references,skills}/` → 零命中。
- **§5.4**：`grep -n "contracts/" .claude/agents/challenger.md` 反查（排除 docs/contracts 前缀）→ 零命中。
- **§5.5**：`grep -rn "run_inline_check|get_run_results|write_knowledge|knowledgeSuggestions|s2Exploratory" .claude/{agents,commands,references,skills}/` → 零命中。
- **§5.6**：`python -m json.tool .claude/scope-map.json` → 解析 OK。
- **§5.7 行为矩阵（enforce-scope stdin 实跑）**：architect→docs/architecture.md **exit 0**；review-moderator→docs/architecture.md **exit 0**；integrator→docs/requirements.md **exit 2**（零写权，报错列出 allow 面）；integrator→docs/architecture.md **exit 2**；integrator→docs/contracts/interface-contract.md **exit 2**（deny 命中）；adjudicator→docs/ADR.md **exit 2**（allow-miss 惰性）。6/6 按预期。
- **§5.8**：`node .claude/scripts/route-regression-check.mjs` → **54 ok / 0 fail**（F8 补牙后由 36 断言扩至 54）。
- **§5.9**：`node .claude/scripts/lint-descriptions.mjs` → **flagged 0**（基线 1 条 landing-verifier 超长告警消除；新增 uiux-author def 一并入扫，clean）。
- **§5.10 census 复跑**：planner 5 / landing-verifier 1 / defender 1 / 其余 0（残留全部位于 [MODEL-TIER]/[DISPATCH-STAMP]/[JOURNAL-APPEND]/FS-EVIDENCE 运行时合同段白名单内，PROPOSAL T7 明示不动）。
- **§5.11 T8 锚**：`grep -c "断言设计透镜"` acceptance-author = 1；`grep -c "影响面机械辅证"` portfolio-planner = 1。

## 3. 诚实注记

1. **「齿」字面残留**：human 260929 明示散文禁用「齿」（记 moderator 长期记忆 + 写作纪律）。本批**新写文本**已全部遵守；存量残留两处属引用或受保护面——① planner.md 运行时合同段（FS-EVIDENCE / DISPATCH-STAMP 区）内 3 处，PROPOSAL T7 白名单整段不动；② acceptance-author.md 引 acceptance-evidence.md 的**节名原文**（「契约条目 ↔ 齿 的强度申报」等），节名属外部文件锚、本批该文件不在 manifest。全仓「齿」清扫未立项（human 裁「触及顺手改、不单独立项」）。
2. **integrator Step 5/6 删除引发步号重排**（7–12 → 5–10）：外部引用核查仅 `integrator Step 4`（planner.md 两处）不受影响；integrator 内部交叉引用（Step 4 ↔ 9b、Merge-Chain 步骤 12→10）已同批更新。
3. **planner.md 运行时合同段**内仍存历史括注（〔T3/T6〕、ARQ-260819 类）——PROPOSAL T7 白名单明示「承重运行时合同整段不动」，属有意保留。
4. **uiux-author.md 为新增件**（manifest 28 锚之一）；其 scope-map 条目（allow docs/ui/** + delivery/taskpackages/**）为本批新授写域，enforce-scope 矩阵未单测该角色（新增面、无既有断言），人读 diff 已核对与 def 写面一致。
5. **crq-doctor advisory 148 条**（no-paths 等存量类）与本批无关；doctor RED 零。

```landing-manifest
.claude/agents/architect.md
.claude/agents/review-moderator.md
.claude/agents/architect-reviewer.md
.claude/agents/builder.md
.claude/agents/challenger.md
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/portfolio-planner.md
.claude/agents/adjudicator.md
.claude/agents/defender.md
.claude/agents/landing-verifier.md
.claude/agents/uiux-author.md
.claude/scope-map.json
.claude/commands/arch-change.md
.claude/commands/review-arch.md
.claude/commands/build-feature.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/references/security-hardening.md
.claude/references/dev-loop.md
.claude/references/task-intake-and-slicing.md
.claude/references/arch-review-rubric.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/route-fixtures/surface-alignment.json
.claude/route-fixtures/routing.json
```
