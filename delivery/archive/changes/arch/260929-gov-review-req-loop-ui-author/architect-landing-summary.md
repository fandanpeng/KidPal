# landing-summary · 260929-gov-review-req-loop-ui-author

> 主线程 packet-bound 落地记录（FROZEN PROPOSAL v1.0 → landing）。landing-manifest 九文件全部落地；验证集 V1–V6 全绿（见 §3）。

## 1. 逐文件落地核对（≡ frozen PROPOSAL §2 各 T）

| T | 文件 | 落地内容 |
|---|---|---|
| T1 | `.claude/references/requirement-change.md` | ①升档规则句加「触 UI」类目 + 「UI 生成步骤只挂档 2」句；②档 2 流程插 4b（UI 生成：uiux-author 生成模式产 brief 升版 + HTML + 出图规格单，主线程代执行出图，出图定例 400×300 PNG = 布局定位权威，否回循环交互句）；③第 5 步 reviewer 四查扩五查句（UI↔需求一致性 + dispatch 含 UI 产物路径 + 恒只读）；④第 6 步终拍呈报加 UI 产物（human 看图拍板）；⑤分工表加 uiux-author 行；⑥消费保护规则扩 docs/ui 在途产物同护（机械判据 = §9 版本行 + brief 版本行对齐） |
| T2 | `.claude/agents/uiux-author.md` | def 全文重写双模式：模式 A 翻译（触发/输入/步骤保留 + 逐页 PNG ref 挂图句 + designRefs 含 PNG）；模式 B 生成（When 可行性消解后 / Inputs 需求修订句唯一设计依据 / 出图规格单步骤 / 需求未覆盖不脑补 / 写面限 docs/ui + od_* 归主线程 + 失联出图 pending）；悬空指称「uiux-designer / governance-lane 尚未接线」删除；frontmatter tools 不扩（Read/Write/Grep/Glob 不变） |
| T3 | `.claude/scope-map.json` | uiux-author allow 列增 `docs/ui/**`；deny 列不动 |
| T4 | `.claude/commands/requirement-change.md` | description 增路由信号（触 UI 类目 / uiux-author 生成模式 / 主线程代执行出图 / 终拍看图拍板）；第 4 步后插 4.5（判触 UI → 派生成模式 → 主线程代执行出图 → 在途标注 → od 失联挂 human-decision 出图 pending）；第 5 步改四查（触 UI 批次扩五查）+ dispatch 输入含 UI 产物路径 + 否回触 UI 重派句；第 6 步落稿呈报加 UI 产物 |
| T5 | `.claude/scripts/brief-inputs.mjs` | ①`collectUiuxRefs()`（designRefs/sharedUiRefs/uiux-brief.md，在位才列）；②builder brief 新增 §①c「UI 设计 refs」节（none 结论形态 + 定位权威句 + 悬空 ref advisory 不入列）；③self-check 增 6 条 uiux-refs 夹具（在位列 / builder 专列 / 缺席 none / 悬空 advisory / uiux-brief 在位 / 夹具非空反真空） |
| T5 | `.claude/references/requirement-change.md` · `.claude/agents/uiux-author.md` | 出图定例与模式 A 挂图句（已随 T1/T2 落，不重复落地） |
| T6.1 | `.claude/agents/story-author.md` | 档 1 / 档 2 两处牵连面清单各加「触 UI（新增页面 / 改页面布局或交互 / 需出图）」类目 |
| T6.2 | `.claude/agents/requirement-reviewer.md` | description 改「四查（触 UI 批次扩五查：加查 UI↔需求一致性）」——**保「四查」子串**（routing.json 必留信号）；正文四查节标题与审稿对象句扩五查 + 第五查条件条目；硬边界读面扩「触 UI 批次另只读 docs/ui 在途产物（恒只读）」 |
| T6.3 | `.claude/agents/planner.md` | Step 6b「尚未接线」悬空句改指「已接线 = requirement-change loop 档 2 uiux-author 生成模式」，职责边界语义不变 |
| T6.4 | `.claude/commands/arch-change.md` | UIUX 段「起草执行角色是 uiux-designer…第五阀 follow-up」悬空句改指模式 B 承接——**R2-F3 留痕：仅改起草执行指称句，「UIUX 不是第 3 个 profile、携带轴集在 governance profile 下审、不判审美」路由语义句原样保全** |

## 2. 评审记录

- R1 回返（4 BLOCKING + 2 SHOULD + 3 MINOR）→ v0.3 修订 → R2 **GREEN**（§7 R2 段）；R2 附带条件 R2-F1/F2 两处文字微修已随 FROZEN 前完成，R2-F3 改写注意已按上表落进 T6.4 执行并在此留痕。

## 3. 验证集结果（V1–V6，2026-09-29 实跑）

| # | 结果 |
|---|---|
| V1 preflight | ok: true · anchors 9 ≡ manifest 9 行 · findings 0 |
| V2 route-regression-check | 54 ok, 0 fail（reviewer「四查」子串与 uiux-author 路由信号均未退化） |
| V3 lint-descriptions | flagged 0 · clean（uiux-author description 首次 364>320 超长告警，已修剪至阈内后复跑 clean） |
| V4 scope-map JSON | 解析 ok |
| V5 grep uiux-author | requirement-change.md = 2 · commands/requirement-change.md = 3（均 ≥1） |
| V6 brief-inputs self-check | 40/40 PASS · SELF-CHECK GREEN · 输出含新增「uiux-refs」自检名 6 条（非 vacuous） |
