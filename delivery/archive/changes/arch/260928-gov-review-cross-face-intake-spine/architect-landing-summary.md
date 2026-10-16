# architect-landing-summary · 260928-gov-review-cross-face-intake-spine

> 主线程 packet-bound 落地记录（Track B 第 4 步）。
> packet-binding：packet-id = 本批 PROPOSAL §2 改法集（T1–T7）+ §5 验证集；packet = FROZEN PROPOSAL v1.0 + 本 landing-manifest + 本 summary；被批 review = 260928-gov-review-cross-face-intake-spine（R1 BLOCK→R2 GREEN，见 [review.md](review.md)）。
> landing-manifest 十路径 = §2 各 T 目标文件并集（preflight V1 实证 anchorUnion ≡ manifest）。

```landing-manifest
.claude/references/change-intake.md
.claude/commands/triage-change.md
.claude/references/change-triage.md
.claude/rules/repo-structure.md
.claude/references/task-intake-and-slicing.md
.claude/commands/build-feature.md
AGENTS.md
.claude/scope-map.json
.claude/route-fixtures/routing.json
docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md
```

## 逐文件落地清单

| # | 目标文件 | T | 落法 | 要点 |
|---|---|---|---|---|
| 1 | `.claude/references/change-intake.md` | T1 | **新增** | 跨面 intake 程序 SSOT，8 节 61 行：触发与比例化（触面 ≥2 建 spine）/ spine 七字段（含 anchors 回填写者恒主线程）/ 四面探测（语料含 CRQ 三域 + governance 面；建档前查重）/ 立项 ack / 发起机制（两姿势 + 三铁律 + 单写者）/ 疑问三级处置（L0/L1/L2 + L1 答不了去向）/ spine 免冻 + facePlan 修订权 / 与各面门接缝（契约行作四分类输入、升级判据≠契约门、开场即消费、launch 前置） |
| 2 | `.claude/commands/triage-change.md` | T2 | **改** | :13 第一刀→四面探测（含 governance 面 + 枚举以 SSOT 为准，R2-M1）；:21 步骤 1 同语义改写；:32-35 触点③整块替换为 runbook 接力（原 :35「本门拉不起那个会话」随块删除，人类在场语义由三铁律承接） |
| 3 | `.claude/references/change-triage.md` | T2 | **改** | :10 第一刀→四面探测（同语义；governance 命中 → /arch-change 治理 profile） |
| 4 | `.claude/rules/repo-structure.md` | T3 | **改** | dev 行后逐字追加两行（intake / retro），行文 = PROPOSAL T3 围栏块逐字 |
| 5 | `.claude/references/task-intake-and-slicing.md` | T4 | **改** | launchPreconditions 节 append「spine 源（② 单包路径）」一条（append-only，存量条目零改动） |
| 6 | `.claude/commands/build-feature.md` | T4 | **改** | launch 面预检四件→五件（⑤ spine 上游面：anchors 未回填 → 报 awaiting-upstream 不进 intake）；末句四件齐→五件齐 |
| 7 | `AGENTS.md` | T5 | **改** | 治理路由节 :24 与 :26 之间插入两段（需求面变更行〔前批留根收口〕+ 跨面 intake 行）；既有行零改动 |
| 8 | `.claude/scope-map.json` | T6 | **改** | agents 段新增 story-author / requirement-reviewer 两条目（`allow: [], deny: ["**"]`，与 architect-reviewer 先例同形）；其余零改动 |
| 9 | `.claude/route-fixtures/routing.json` | T6 | **改** | agentRequiredSignals 补两断言（信号逐条摘自两 agent description 实文）；commandEntry / agentRequiredToolsAgents 零改动 |
| 10 | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | T7 | **改·append-only** | 正文末 append「2026-09-28 注记（第二批）」：建议 1/3/4 已由本批承载 + Track B 改判留痕（OQ-1=(b)）；既有行零改动 |

## 验证集实测（pre-commit，2026-09-28 主线程实跑）

| # | 命令 | 实测结果 |
|---|---|---|
| V1 | `landing-fidelity-check.mjs --preflight <PROPOSAL>` | `ok: True anchors: 10 findings: []`（三次复跑一致；R2-M1/M2 修订后复跑亦绿） |
| V2 | `route-regression-check.mjs` | `36 ok, 0 fail`（T6 两断言在 fixtures 且与 description 实文一致——前批 V3 真空绿闭合） |
| V3 | `node -e "JSON.parse(scope-map.json)"` | `scope-map ok` |
| V4 | `grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/` | `0`（流程面 census 清零；集外 6 处描述性引用在收窄 scope 外、已显式豁免） |
| V5–V7 | post-commit | commit 后补跑（预期：AGENTS.md 两新增段命中 / ARQ 注记恰 1 段 / repo-structure 两新行命中） |

## 执行事故记录（诚实留痕）

**T1 Write 生成事故**：落地 T1 全文时，主线程 Write 调用的内容参数发生两次输出序列化损坏（乱码行 / 重复行 / 元评论噪音混入文件）。均为主线程自身生成事故，非任何外部输入；**均当场自查发现**（第一次经 Read 核实后确认损坏，第二次截断亦经 Read 确认）。处置 = 换小步分块策略（每次工具调用一小段、逐段追加），4 段全部干净落盘；终件 61 行 8 节，噪音字符串（`D-PWR` / `g-5d8` / 乱码头）grep 复检零命中，全文人读核对与 PROPOSAL T1 改法八块一致。**教训**：长中文单次 Write 有序列化损坏风险，长文档落地宜分块追加；本仓 byte 保真纪律（PROPOSAL 自检句③）应扩及「生成即验证」——写入后立即机检自身产物，不凭「Write 成功」自信。

## OQ 裁定与顺带项

- OQ-1~4 全裁已钉 §4（Track B / settings(b) / 并入 / 后面验证）；OQ-1 改判留痕随 T7 进 ARQ 账本。
- R2-M1/M2 冻结前顺手修毕（T2 替换句补 governance 面枚举、faceMap 字段补 governance 命中行）。
- showme 讨论稿随批归档（非 manifest 件，不进 packet）。
