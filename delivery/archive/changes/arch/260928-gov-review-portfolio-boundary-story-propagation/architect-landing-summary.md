# architect-landing-summary · 260928-gov-review-portfolio-boundary-story-propagation

> landing 时点：2026-09-28 · 主线程 packet-bound 落地（Track B 第 4 步）· packet = FROZEN PROPOSAL v1.0 + landing-manifest + 本 summary
> **packet-id = 本 PROPOSAL §2 改法集（T1–T9）+ §5 验证集**
> 评审凭据：human 立项 ack + architect R1 `BLOCK(F1-F4)` → v0.3 闭环 → R2 `GREEN`（R2-F1/R2-F2 一行修已落）+ human OQ-1=(b)+三承载位 / OQ-2=(a) / OQ-3=号集相等+降级

```landing-manifest
.claude/agents/portfolio-planner.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/integrator.md
.claude/agents/builder.md
.claude/commands/build-feature.md
.claude/references/task-intake-and-slicing.md
.claude/references/minimal-implementation.md
.claude/references/acceptance-evidence.md
.claude/references/code-review-gate.md
.claude/references/dev-loop.md
.claude/scripts/brief-inputs.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/lib/acceptance-id-shared.mjs
.claude/scripts/mutation-check.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
delivery/workstreams/README.md
```

## 逐文件落地清单（18 == manifest，逐文件人读 diff 核对改法 == FROZEN §2）

| # | 文件 | T | 落法摘要 |
|---|---|---|---|
| 1 | `.claude/agents/portfolio-planner.md` | T1/T3/T5/T9 | Step 1 增「拆包顺序链停等三件」fail-closed 段；Step 2 并集句改 decisionCoverage 承载映射核（acceptance 级覆盖核查指向 Pod 内两级反查）；Step 4 标题去「双 input-brief」、① 增 acceptance 拆包期空骨架、② plan.md 骨架五件收窄 + OQ-1 三承载位句、⑤ 双 brief 生成句删除；Verification 节 :95 并集句同步改 |
| 2 | `.claude/agents/planner.md` | T3/T4/T9 | Step 1① 补「acceptance 为 Pod 内产物、拆包期为空骨架」澄清句；Step 1e 蒸馏写点下沉改型（通读义务保留 + 明钉 Step 8b 核对）；Step 7 改回「前置一次产双 brief」生成形态（翻转 260921 T5 裁定）；Step 8b 扩为全 story 命中包 + 核对三件（AC 号集对账 / 补充规则落点 / 断言方向）+ 退回重钉不代写 |
| 3 | `.claude/agents/acceptance-author.md` | T3/T8 | Role 节补唯一写点定位（planner Step 8b 核对、不通过退回重钉）；Inputs 节 brief 内容清单补 story 照录段；description 例句换新（Sx-Ay → S-Fx.y、S-Fx.y-AC<n>） |
| 4 | `.claude/agents/integrator.md` | T8 | Basis 引据例句 `Sx-Ay` → `S-Fx.y` |
| 5 | `.claude/agents/builder.md` | T8 | 交付报告例句 `Sx-Ay` → `S-Fx.y` |
| 6 | `.claude/commands/build-feature.md` | T8 | argument-hint 例句 S1-A1 → S-F1.4（真实 story 例） |
| 7 | `.claude/references/task-intake-and-slicing.md` | T1/T3/T6 | 新增「尺寸维与视觉对齐判例」节（四判据 + 视觉三判例，引据 ui-v5-align 终裁）；plan.md 骨架收窄五件 + 判据一句钉死 + OQ-1 三承载位；story 转写节增「acceptance 唯一写点」条 |
| 8 | `.claude/references/minimal-implementation.md` | T8 | 例句 `Sx-Ay` → `S-Fx.y` |
| 9 | `.claude/references/acceptance-evidence.md` | T8 | 例句换新 + 补 AC 复合句法口径 |
| 10 | `.claude/references/code-review-gate.md` | T8 | 两处 Basis/Correctness 例句换新 |
| 11 | `.claude/references/dev-loop.md` | T8/T9 | brief 生成时点文面归还 Pod 内 Step 7（T9 承重位）；:37/:109/:274 例句换新；**:166 未动**（见排除清单） |
| 12 | `.claude/scripts/brief-inputs.mjs` | T2 | 新增 story 照录路：`isStoryRef` + `storySectionText`（标题 `·` 前缀精确匹配防 S-F1.4/S-F1.40 误命中）+ `renderStoryRoad`（整节 fenced 照录 + AC 复合句法加亮 + not-found advisory 不阻断）；story ref 从①路路由出（missing 表不再误报）；selfCheck 增 5 夹具 |
| 13 | `.claude/scripts/taskpackage-required-check.mjs` | T4 | 新增 `collectAcBackrefErrors` 纯函数（号集相等 / bare 降级 / 存量 carve-out = gitFirstCommitDate < 2026-09-28）+ 主流程 coverageErrors 挂接 + selfCheck 增 7 夹具 |
| 14 | `.claude/scripts/lib/acceptance-id-shared.mjs` | T8 | 夹具 token 连动换新（S1-A1→S-F1.1 等，断言预期同步）；新增夹具⑨ = `S-F1.4-AC3` 复合句法不被误拆（extractShortId 整段返回 + mentionsId 子串命中）——兼容性结论：解析面零代码改动 |
| 15 | `.claude/scripts/mutation-check.mjs` | T8 | selfCheck 两夹具 token + 预期连动换新 |
| 16 | `.claude/scripts/acceptance-coverage-check.mjs` | T8 | 注释例句两处换新 |
| 17 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | T9 | launch checklist 步骤 8「双 brief 在场」删除（launch 侧不再核 Pod 内输入面）；余下 1 处 input-brief 命中 = §2 上下文说明句、非 checklist 项（§5-12 允许形态） |
| 18 | `delivery/workstreams/README.md` | T7 | 新增「拆包顺序链」节（两条纪律 + 与 portfolio-planner Step 1 停等互为表里注记 + ui-v5-align 实证引用） |

## 验证集执行记录（§5 逐条）

| # | 结果 | 关键输出 |
|---|---|---|
| 1 preflight | ✅ | `ok:true, anchorCount:18, findings:[]`（FROZEN 标注前 + landing 后各跑一次） |
| 2 brief-inputs self-check | ✅ | 34/34（含 ①b 新夹具 5 条） |
| 3 探测夹具 | ✅ | 260928-brief-probe 临时包 dry-run：①b 段落定位 `节首行 :285` == requirements.md §S-F1.4 节首行（机械切片 = byte 一致）；探测包用后即删 |
| 4 taskpackage-required self-check | ✅ | 12/12（含 AC 回引齿 7 条：Draft 放行 / finalize 缺回引红 / bare 一回引绿 / 号集相等绿 / 多引红 / 存量放行 / 新包红） |
| 5 acceptance-id-shared self-check | ✅ | 23/23（含夹具⑨复合句法兼容性——`--self-check` flag 形态实测在册） |
| 6 route-regression-check | ✅ | 36 ok, 0 fail（acceptance-author description 换新后路由信号零退化） |
| 7 lint-descriptions | ✅ | 告警仅 1：landing-verifier.md 超长（363>320）——**本批未触碰该文件，既有基线**（warn 级 exit 0） |
| 8 残留 census | ✅ | 命中恰 2 = 声明排除项（见下）；例句位零残留 |
| 9 存量抽查 | ✅ | m4-k3 实包过闸：输出含 0 条 `AC backref` 错误——新齿对存量包零误红 |
| 10 portfolio-planner 残留 | ✅ | `grep brief-inputs` = 0 |
| 11 planner Step 7 形态 | ✅ | `^7\. .*brief-inputs` 命中 :61——生成形态在位 |
| 12 SKILL 残留 | ✅ | 1 命中 = §2 上下文句（允许形态）；checklist 项已删 |
| 13 route/lint 之外回归 | ✅ | brief-inputs / taskpackage-required / mutation-check / acceptance-id-shared 四闸 self-check 全绿 |

## 排除清单（historical 不动，T8 ④）

- `contract-change-evidence.md:29` 的 `S5-A2`：指向 `s1-nsa-integration-e2e` 历史真实验收测试的引用，改了即伪造引用。
- `dev-loop.md:166` 的 `S1-A1..A14` / `P1-A14`：260731 批力度变更的闭包叙事句，句内 token 是该批「存量样本全部闭包」叙述的组成（同句含 s1-nsa / k1-* / k1-session 真实存量包 id）；整句是「旧形态不豁免」的反例展示、非格式例句。**本文件在 manifest 内且已改（:37/:109/:274 三处换新），仅此一行保留**——如实记录此逐点裁断。

## 留根移交

- §6 留根各项随 PROPOSAL 走（dev-loop landing-manifest 已单开 `ARQ-260928-devloop-landing-manifest`；OQ-2b 机械齿挂量化触发器；brief 冒号形态解析兼容 landing 夹具未覆盖——本批只保裸 id 与复合句法两形态）。
- 三条 ARQ（260927 主 + 260924 / 260914 并批）待 landing-verifier 绿 + 合并后结项（crq-render 重跑）。
