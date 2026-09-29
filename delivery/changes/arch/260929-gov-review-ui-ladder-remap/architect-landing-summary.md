# landing-summary · 260929-gov-review-ui-ladder-remap

> 主线程 packet-bound 落地记录（FROZEN PROPOSAL v1.0 → landing）。landing-manifest 九文件全部落地；验证集 V1–V8 全绿（见 §3）。

## 1. 逐文件落地核对（≡ frozen PROPOSAL §2 各 T）

| T | 文件 | 落地内容 |
|---|---|---|
| T1 | `.claude/references/task-intake-and-slicing.md` | 梯子节重排——节引句改「pre-launch UI-ready check」；L2 行改「上游 frozen 页图齐 → 消费（planner 写 designRefs，不派翻译角色）」；L3 行改「设计缺口信号：fail-closed 停包 + pod-events `agent-escalation` + 路由 requirement-change（原 story 档 2 触 UI 修订批）+ frozen 后重核再续」；L4 行改「画走档 2 4b / 审走 /arch-change 轴集（只审不画）/ 蓝图先行与拆包先后不互斥」；:330 baseline 句改「UI-ready 判据权威指针（requirement-change.md 消费保护规则节）」；:332 字段说明改「Level 1+ (uiAcceptanceRefs) / Level 2+ (designRefs per-page PNGs)」；:338 JSON 示例 designRefs 改 PNG 路径形态；:349 句改「designRefs point to frozen per-page PNGs（400×300 布局坐标定位权威；brief 文本节仅口径补充）」 |
| T2 | `.claude/agents/planner.md` | Step 6b 重写——判级保留；UI-ready 前置检查（逐页 PNG 在位 + 版本行判据分形，权威指针句 + 两分支各半句摘要——R2-F2 形态）；齐 → planner 写 designRefs（acceptance-author 直读 TaskPackage / builder 经 ①c）；缺 → fail-closed 停包 + agent-escalation 留痕 + 路由句；「不由你起草」边界句并入；L4 交界句（停包不越权拆包）。T2b：frontmatter `tools: Agent(...)` 白名单删 `uiux-author` token（逐字段隔离，其余 token 未动） |
| T2b | `.claude/route-fixtures/routing.json` | planner 可派列表删 `"uiux-author"` token |
| T3 | `.claude/agents/uiux-author.md` | def 重写生成单模式——H1 改「需求 loop UI 生成」；模式 A 整节与 `uiux-brief.md` 制品义务删除；When/Inputs/Process/Boundaries 平铺（内容 = 原模式 B 现文）；description 重写（requirement-change 档 2 触 UI / 档 2 流程 4b / 主线程派路由信号保留，dev-loop 不派信号新增），tools 未动 |
| T4 | `.claude/references/dev-loop.md` | :55 流程图行改「UI-ready 前置检查（条件）」；:68 条件步骤段整段改写（判据分形 + fail-closed + agent-escalation 留痕 + 承接形态 + 供给链在 requirement-change 档 2 4b 句——措辞避开 V7 计数 token） |
| T4 | `.claude/agents/acceptance-author.md` | :23 改「UI 输入 = TaskPackage uiux block 的 uiAcceptanceRefs[] 与 designRefs[]（planner 起包期写入；acceptance-author 直读 TaskPackage，builder 侧才经 brief-inputs ①c 透传）」——分角表述（R1-F1） |
| T5 | `.claude/references/requirement-change.md` | 「消费保护规则」节加「launchPreconditions · UI-ready 消费」正式句（分形判据全句、权威承载位、防双源）；「定稿载体」节留根句改指「UI 面已落（见消费保护规则节），story 引用版本号机检齿仍留根」 |
| T6 | `.claude/scope-map.json` | uiux-author allow 删 `delivery/taskpackages/**`，只留 `docs/ui/**`；deny 列不动 |
| T7 | `.claude/scripts/brief-inputs.mjs` | ①`collectUiuxRefs()` 删 briefPath 检查块 + 函数头注释改写（不含 uiux-brief token，R3-F1）；②none 消息句删该字样；③self-check 删「uiux-brief.md 在位入列」断言与 uxDir temp 夹具块（总断言数 40 → 39），其余五条 uiux-refs 断言全保留 |

## 2. 评审记录

- R1 回返（1 BLOCKING + 4 SHOULD + 4 MINOR）→ v0.2 修订 → R2 **GREEN** → human OQ-4 追加（①c 子源删净）→ v0.3 扩 T7 → R3 **GREEN（有条件）**→ R3-F1/F2/F3 三处微修完成 → FROZEN v1.0。全程记录见 PROPOSAL §7。
- R2-F2 措辞注意（Step 6b 权威指针句 + 两分支各半句摘要形态）已按上表 T2 执行并在此留痕。

## 3. 验证集结果（V1–V8，2026-09-29 实跑）

| # | 结果 |
|---|---|
| V1 preflight | ok: true · anchors 9 ≡ manifest 9 行 · findings 0 |
| V2 route-regression-check | 53 ok, 0 fail（基线 54 → 删 planner 白名单 token 后成员 -1，与判官 R1 已核项 4 预期一致） |
| V3 lint-descriptions | flagged 0 · clean |
| V4 scope-map JSON | 解析 ok |
| V5 brief-inputs self-check | **39/39 PASS** · SELF-CHECK GREEN（40 → 39 与 T7③ 一致；designRefs 主路五条 uiux-refs 断言在场） |
| V6 planner token 计数 | **0**（基线 2 → 清零） |
| V7 三文件旧义计数 | task-intake 0 / dev-loop 0 / acceptance-author 0（基线 2/2/1 → 清零） |
| V8 脚本 uiux-brief 计数 | **0**（基线 6 → 清零；自查时曾出 1 = 主线程墓碑注释自引，已改写措辞归零） |

## 4. 落地期自查记录（主线程主动申报）

- 落地 T7③ 时曾新增一条「子源已剥离」自检断言——超出 frozen 清单（V5 预期 39），当轮撤回，未落入盘。
- dev-loop.md :68 改写初稿含「uiux-author 生成模式」指称——会打破 V7 = 0 断言，落地即改写为「生成模式，主线程派」（供给链语义保留、token 移除）。
