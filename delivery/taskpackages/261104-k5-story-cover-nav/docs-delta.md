# docs-delta · 261104-k5-story-cover-nav

- **docsSyncRequired**：`false`（TaskPackage 声明；docs-sync 闸 PASS，2026-11-15 final 终验轮 r5〔真机轮修复后重跑〕）
- **docsOwner**：integrator
- **ADR**：`adrRequired=false`，本包未产出新 ADR，`docs/decisions/ADR.md` 零变更（引用既有 ADR-018，不改文本）。

## 结论：零变更（理由）

本包没有需要同步到 canonical docs 的行为面变化；requirements / architecture / contracts / docs/ui 四类文件**全部零 diff**（`git diff --stat 03630b65..HEAD -- docs/` 为空）。逐面核实如下：

1. **需求面 `docs/requirements.md`（v1.71 frozen，2026-11-04）**：§2.4 已定义 P4 故事封皮（书封美术 + 「星星记录本」标题 + 已收集星星数 + 键帽「打开 / 下一页」）、列表与阅读下行是模块内部子页、逐层返回「阅读 → 列表 → 封皮 → 首页」；S-F2.6（AC1–AC4）已冻结封皮四件、逐层返回、语音「打开故事」只到封皮、restore 语义与 story_cover 普通语境。§2.3 按键矩阵与「双击消歧扩为故事列表态 + 积分主页」已冻结。本包实现与冻结条文逐条对齐（KCN-C1..C10 / C6..C8 承载），未新增或改写需求语义，不需要回写。
2. **架构面 `docs/architecture.md`**：§5.1.1 页面组件表已登记 `story_cover.c` 承载 P4 封皮（本包按 1g 预评改落 `story_page.c` 子态、无独立 `story_cover.c`，见 §A 落点决策——属实现落点选择，不改架构组件语义：封皮仍是 P4 模块入口、状态机单一）；§5.1.1 按键路由已冻结「P4 封皮 KEY 进列表，列表/阅读逐层返回，子页保持第 4 实心」；§5.2 已冻结 story_list = P4 模块共享数据帧、后台推送恒 `restore:true`、显式打开走空 data 薄载荷且不带 restore、封皮与列表都以 `total` 渲染篇数。会话语境映射已冻结 `STORY_COVER → story_cover`（普通问答）与 `P4 列表 → story`（记录语境）。本包实现与之一致，零漂移。
3. **契约面 `docs/contracts/interface-contract.md`**：§1.3③（2026-11-04 改写行）已落「所有后台恢复/对账推送恒带 `restore:true`；只有 front 展开『打开故事』的 `{view:"story_list",data:{}}` 薄载荷时不带 restore」；§1.5（行 254）已落「2026-11-04 增认 `data:{}` 导航薄载荷」与 relay 收窄；§四 `kidpal_session_ctx` 语境映射（含 story_cover）已落。本包 **display_schema 薄载荷口 / front `_expand_story_nav_intent` 展开 / 三时点 restore:true** 均是这些已 LANDED 契约的实现落地，无需改契约文本（1g 预评已明确）。
4. **设计面 `docs/ui/**`**：设计权威 = `docs/ui/V9/story-cover.png` + `docs/ui/kidpal-ui-design-v9.html`（母版）。本包按 V9 surface 产出书封 + `ic_coin_52_52` KAP1 资产，`docs/ui/**` 不进 changeManifest、零 diff。封皮未含「小伴」一项归 uiuxLevel 4 design review 偏差项（见 review-packet 风险 C），属观测登记，不改设计稿。

## 与 human 裁定 / ARQ 建议的关系（不构成本包 doc 变更）

- **human 2026-11-15 裁定①「归档失败离页帧 restore=False 例外」**：不改契约，按离页导航帧例外处理，在 `front_app.py:1771` 一带注释留痕，并在 review-packet 记风险 A + ARQ 建议（契约 §1.3③ 措辞未来显式排除离页帧）。**本包不动契约文本**——该 ARQ 建议交主线程/后续架构批处理，不在本包 `/arch-change` 范围外自行改写。
- **human 2026-11-15 裁定②「板级 BOOT 钩子入口 reset」**：实现层载体决策（`waveshare-s3-rlcd-4.2.cc → McpServer::StoryEnterCover`），不改架构/契约语义（首翻 P4 见封皮 = S-F2.6-AC1 已冻结行为）。
- 若将来要把「离页导航帧例外」或「薄载荷展开帧不带 restore」提升为契约显式条款，属 `CONTRACT_CHANGE_REQUIRED`，走 `/arch-change` loop，不在本包内自行处理。

## 影响面记（供 reviewer 对账）

- 本次 docs-sync 判定与 `docs-sync` 治理闸 PASS 一致（闸读 `docsSyncRequired=false`，本文件给出人工复核理由）。final r5 逐闸确认 docs-sync PASS、reason =「封皮、P4 分层、story_list 共享帧、restore 与会话语境均已由 v1.71 和 landed 架构/契约冻结」。真机轮三 commit（视觉对齐 + 导航帧接管 + 回归断言）均在已冻结设计/契约内落地，`docs/` 仍零 diff。
- pod-events 中一条 friction（`acceptance-coverage-check.mjs` 不认 `--change-id`，与 mutation-check 同族旗标不一致）候选 ARQ，属治理机检工具面建议，与本包 canonical docs 无关，交主线程评估。
