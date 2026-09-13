# Landing Summary · 260912-gov-review-portfolio-decomposition-chain

> FROZEN v1.0（commit 12d23a2）packet-bound 落地。主线程单通道落地（Track B 第 4 步）；landing 清单 = PROPOSAL ```landing-manifest``` 9 文件，零越界（见 landing-verifier verdict）。

## 落地内容（按 manifest 逐件）

| # | 文件 | 动作 | 内容摘要 |
|---|---|---|---|
| 1 | `docs/templates/WORKSTREAM-PROPOSAL.template.md` | 新增（G2） | 三问 + 分型骨架：frontmatter `workType` 字符串数组；§1 业务目的清单硬项 + 起草侧两查自检行（已砍编号 / 🔴 处置）；§2 三子形节名固定字面（「现状与差距」/「问题与根因」/「结构性痛点」）；§3.1 spec 关系标注 token（`within-spec`/`需修订`/`spec-silent`）+ `需修订` 的 launchPreconditions 执行语义；权威序声明（spec = 行为语义权威，本件 = 路径选择记录） |
| 2 | `docs/templates/WORKSTREAM-ROADMAP.template.md` | 新增（G2） | 四固定节：① 效果目标投影（不复制）；② 列车图（render-roadmap 标记区块、人工禁改、渲染源唯一声明）；③ 每包条目（效果段人工散文位 + 回指 story ID/AC 号集 + demo 证据形态 + 达成状态机械渲染）；④ 非目标与批次归属；头注含与 task_plan.md/progress.md 的职责划界 |
| 3 | `.claude/scripts/render-roadmap.mjs` | 新增（G3，advisory 非门） | 圈定按 `origin.workstream`；board.json（git common dir）+ pod-events deviation（既有 `human-decision` 型 + `payload.decision="anchor3-deviation-ruling"` 区分值）→ mermaid 图 + 达成状态表；跨 workstream dependsOn → 带标注外部节点（虚线）；全量重算幂等；`--write-into` 标记区块替换；`--self-check` hermetic 15 断言 |
| 4 | `.claude/agents/portfolio-planner.md` | 改（正文，frontmatter 零改动） | Step 1 增模板符合性核验（fail-closed 打回主线程）+ 两道 story 引用机查；Step 4 改四件套落盘（含每包 plan.md 全量义务 + roadmap 产方 + decision-coverage launchPreconditions）；新 Step 5b 门 1 停等（AskUserQuestion 请 human 发起 → GREEN + 切法过目 → 方进 Step 6）；Step 6 commit 集补 roadmap.md；References 补两模板 + 渲染脚本 |
| 5 | `.claude/agents/planner.md` | 改（正文，frontmatter 零改动） | Step 7 增锚 2 输入（workstream 包 dispatch prompt 携带 roadmap 效果段路径+节锚引用）；新 Step 8b 锚 2 验收方向核（AC 回引对账按 workType 分流）；Step 11 增锚 3 生产者可达性（dispatch prompt 携带 demo 对账行义务）；TaskPackage 格式例整体换新（`S1-A1`/`Sx-Ay`/systems 老仓遗产 → `S-F1.4` + `S-F1.4-AC3` + acceptance 本地 id `SR-Ax` + story 字段锚点引用口径注记） |
| 6 | `.claude/references/task-intake-and-slicing.md` | 改 | 新章「拆包链：workstream 双轨制品、门 1、story 转写」（双轨注记 + plan.md 全量义务 + story 锚点引用与转写【AC 复合句法 `S-Fx.y-AC<n>` 全仓首钉 + write-story 分工注记 + 换新边界】+ 门 1 checklist 五项 + 派发形态 + 裁决耐久记录 + launchPreconditions canonical 定义）；sourceRefs spec 例句换新（`S1-A1` 系 → `S-F1.4` / `S-F1.4-AC3` / `docs/requirements.md:S-F1.4`）；planRef 节补③拆包路径注记 |
| 7 | `.claude/references/pod-coordination.md` | 改 | §2b 补 launchPreconditions convention 核验条款（`--check-launch` 不读该键、人读核验、门判定语义不变）+ `:Sx-Ay` 例句换新；新 §11 锚 3 demo 对账（对账行义务 + 经 planner dispatch 传递的生产者可达性 + deviation 事件载体形态 + 绿路径 portfolio-planner 沉默 + 达成状态刷新时机 + 再出场条件收窄） |
| 8 | `.claude/references/change-triage.md` | 改 | §4 ③大 feature 落盘行补「按 WORKSTREAM-PROPOSAL.template.md 起草 + 三硬项 fail-closed」 |
| 9 | `.claude/commands/triage-change.md` | 改 | Step 4 ③大 feature 行同步补模板起草义务 |

## 验证结果（PROPOSAL §5 验证集）

1. **route-regression-check**：28 ok / 0 fail GREEN。**lint-descriptions**：exit 0（warn 级 1 条 = `landing-verifier.md` over-length——既有、与本批无关，本批未触任何 description）。
2. **render-roadmap.mjs --self-check**：15 断言全绿（圈定 / 包序 / deviation 区分值 / merged→achieved / deviation 优先 / 降级 / 外部节点标注 / 实虚线边 / 标记对 / 幂等 / write-into 替换与重跑幂等）。**260911 实跑**：圈定 3 包（key-route-a1 / screen-unification / display-first-m1，与 launchPreconditions commit 清单一致——ecs-deploy-convergence / render-pages 属 kidpal-mvp 不入集）、ecs-deploy-convergence 渲染为带标注外部前置（虚线）、三包 achieved（board=merged）。
3. **回溯演练（真阳性）**：`ls .claude/taskpackages/260911-key-route-a1/` 确认无 plan.md——T2 全量义务（豁免全取消）能拦住该真实缺陷；PKG-2 单包承载 4/5 收口标准系 §1 R1 地面真值（判官 R1 亲核），门 1 checklist ① 切片逻辑项命中面。
4. **制品在场**：两模板存在且含业务目的清单/效果目标投影硬项节与三子形节名（「问题与根因」/「现状与差距」/「结构性痛点」与 plan.md 骨架锚名、roadmap ① 节投影引用逐字一致——模板↔task-intake↔planner 格式例三处核对一致）；task-intake 含 spec 关系标注约定 + story 锚点引用转写小节（句法 + 分工注记 + 例句换新）+ launchPreconditions canonical 定义；正文引用行在位为 landing-verifier 人读核对项（route-regression 的 agentRequiredSignals 只覆盖 review-loop 六 agent，不声称机械覆盖——m-7 如实口径）。

## 诚实记录

- `docs/requirements.md` 同期在 human 主导 phase 1/2 重 review（v1.10+，已砍清单已移至文末 §8）——本批机查口径按「节名定位、位置无关」落（m-5），不受其扰。
- `Sx-Ay` 老仓遗产字样的 manifest 外命中面（integrator.md / build-feature.md / minimal-implementation.md / acceptance-evidence.md / code-review-gate.md / builder.md / dev-loop.md / acceptance-author description / acceptance-id-shared.mjs / mutation-check.mjs 等）**未在本批处理**——按 PROPOSAL §6 留根另登 ARQ 批量对齐。
- M-1 机械齿（portfolio-reconcile.mjs 读 launchPreconditions）按 260913 human 裁定**不做**，convention + 人读，ARQ 留根待登。
