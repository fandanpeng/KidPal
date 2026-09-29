# requirement-change · 需求面变更流程（三档）

> 权威面：本文件是需求面变更的流程 SSOT（260928-gov-review-requirement-face FROZEN v1.0 立）。story **怎么写**归 `write-story` skill，本文件管**谁起草、谁审、怎么定稿**。triage intake 跨面接线消费本文件判据（ARQ-260928 批，留根）。

## 三档判据

| 档 | 范围 | 边界例 |
|---|---|---|
| 0 · 勘误/措辞 | 不改语义：错字、排版、表述澄清、锚行格式 | 跨 story 零语义清扫（锚行格式批改、勘误连带同步）= 档 0 |
| 1 · 语义修改 | AC 增删改 / 补充规则口径 / 单 story 内行为变更 | NFR 数值微调 = 档 1 |
| 2 · 结构性 | story 新增 / 重写 / 砍除、跨 story 连锁、NFR 新增/删除/阈值语义变更、Epic 级 | — |

**升档规则**：档 1 起草期发现牵连面（关联 story / §2 共享底座 / 线框需同步 / **触 UI**——新增页面、改页面布局或交互、需出图）即携带清单；牵连 ≥2 处或含 NFR/Epic → 升档 2（升档裁定：主线程提呈、human 确认）。触 UI 的语义修改按本规则自然落档 2——UI 生成步骤只挂档 2（见档 2 流程 4b），档 0 / 档 1 不派。

## 头部状态机

requirements.md 头部版本区设状态行（形态 `> 状态：frozen | updating · <版本号> · <日期>`），是盘上在途态标记、**非第二定稿标记**（定稿唯一标记 = §9 版本行，见「定稿载体」节）：

- **置 updating**：story-author 动笔即置，并注在途版本号与日期。
- **翻 frozen**：只归主线程——档 1 = requirement-reviewer PASS 后（档 1 无 human 终确认，human 已裁）；档 2 = human 终拍后。翻转同时 append §9 版本行。
- **档 0 勘误不走状态机**：主线程直改即 frozen，勘误无在途态。
- **边界场景**：① updating 在途期档 0 勘误不并行执行——勘误等 frozen 后补做，或并入在途变更随本批落（主线程裁其一，§9 依据列留痕）；② updating 会话中断的弃/续由主线程在下次入口裁——弃 = 在途改动段经 git 还原、状态行恢复 frozen、不 append §9 版本行；续 = 重新派 author 接着改、状态保持 updating。

## 各档流程

### 档 0 · 勘误/措辞

主线程直改（挂 write-story 完稿自检 + spec-anchor-check 机检）→ §9 追加变更行，标「勘误」。无新角色。

**跳过例外**：档 1 级语义修改，human 明确说跳过流程的一次性操作允许直改（对齐 AGENTS.md Hard Rules 既有条款）；§9 记账须写明「跳过 story-author/reviewer」。

### 档 1 · 语义修改

1. **human 裁意图**——原话即起草依据（含改什么、为什么）。
2. **story-author 盘上起草**——直接编辑 requirements.md（头部状态行置 updating）+ 完稿自检 + spec-anchor-check + 相邻 story 撞车报告；return 交主线程的内容 = 自检结果 + 撞车报告 + 已改锚行清单 + **改前/改后句级对照**（逐修订段「原句 → 新句」两行式；稿在盘上，不另携修订稿全文，对照不可省——reviewer 意图保真夹带检测的输入）。
3. **requirement-reviewer 四查**——审稿对象 = 盘上 updating 态文件（不审转交文本）；主线程 dispatch prompt 输入构成显式含「human 意图原话 + author 改前/改后对照」；不过否回循环至过（author 再改盘上文件，状态保持 updating）。
4. **过即落稿**——主线程翻状态行 frozen + §9 追加版本行。**无 human 终确认**（human 已裁：意图事前对齐 + reviewer 意图保真核查已覆盖；§9 append-only 可事后否回）。

### 档 2 · 结构性

1. **human 裁意图与取舍倾向**。
2. **story-author 盘上起草**（updating 态，含牵连面清单）。
3. **architect 可行性判断（默认必派）**：固定三件产出——可行性三态 ✅/🟡/🔴 + **逐 story/AC 技术锚**（每个受影响 story/AC 的可实现性判据与实现要点一句话）+ **影响面概貌**（牵连的实现面文件/子系统清单，供下游 triage 分档与 planner 前置技术评估消费）；只出意见不裁语义。免派例外唯一一条：human 明示可行性已有在先评估承载时免派，免派事由记 §9 该行依据列。
4. **story-author 按可行性修订**——🔴/取舍点升回 human，不带病下行。
4b. **UI 生成（仅档 2 且触 UI）**——story-author 牵连面清单标「触 UI」的批次：主线程派 `uiux-author`（**生成模式**）产出本批 UI 修订——UIUX-DESIGN-BRIEF 升版 + 设计 HTML + **出图规格单**（逐页 pageId/stateId、画布 400×300、黑白 1bpp 风格约束、内容要点、对应 brief 锚）；**出图执行 = 主线程代执行**：按规格单跑 OpenDesign MCP（od_*）回填 PNG 落 `docs/ui/`，文件名对齐 pageId/stateId。**出图定例：每个受影响页面/状态一张 400×300 精确画布 PNG——PNG 是 builder 布局坐标的定位权威，HTML 仅作母版迭代载体不进 builder 视野。**产物头部标注「随批 <changeId> 在途」；🟡 可行性或取舍点未被 human 消解前不出图；**否回循环交互**：reviewer 打回的需求修订触及 UI 面 → 主线程重派 uiux-author（生成模式）更新在途产物（含重出图）后再送复审，状态保持 updating。非触 UI 批次不派。
5. **requirement-reviewer 四查**（重点查牵连面；审盘上 updating 文件 + 改前/改后对照）——**触 UI 批次扩五查**：加查「UI↔需求一致性」，对照 updating 态需求与新落 docs/ui 在途产物逐项核（页面覆盖 / 交互口径 / 键位提示与 requirements §2.3 键位逐语境口径相符）；dispatch 输入构成显式含 UI 产物路径，UI 产物对 reviewer 恒只读。
6. **human 终拍**——architect 结论 + reviewer 结论 + **UI 产物（设计稿/图，触 UI 批次）**一并呈——human 看图拍「要不要」；拍板 → 主线程翻 frozen + §9 版本行；review 留痕 = §9 该行依据列浓缩承载 architect 结论 + reviewer 结论。

分工一句话：architect 管「能不能」、reviewer 管「准不准」、human 拍「要不要」。

## 定稿载体

- story 修订定稿的标记 = **§9 版本行本身，版本号即 changeId**（不另铸批次 ID——TaskPackage changeId 强语义见 task-intake-and-slicing.md，撞名禁用）。
- 实现包引用 story 在 write-story 合法形态（`S-Fx.y · 标题`，可选 AC 号集）之上可加「定稿版本号」；launchPreconditions 消费挂点归 ARQ-260928 批（留根）。

## 消费保护规则

updating 态的 requirements.md 不得被下游引用——triage 分流、taskpackage 撰写（story 引用）、launchPreconditions 消费一律只认 frozen 态（机械判据 = §9 对应版本行在位）。**触 UI 批次的 docs/ui 在途产物同护**：下游（triage / taskpackage / dev-loop）只认 frozen，机械判据 = §9 版本行在位**且** UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号。引用校验机检齿（taskpackage-required-check 校验引用版本号在 §9 在位）留根至实包跑 2–3 单后按摩擦立项。

## REQ- 域 CRQ 闭环纪律

`docs/crq/requirement/` 条目（前缀 `REQ-`）必填三件：

- **owner**：指向需求变更流程某档或 human——禁空。
- **trigger**：何种事件触发捡起——须可判，禁「human 想起」形态。
- **销行条件**：满足何状态可销——通常 = 对应需求变更 §9 定稿；销行跑 `node .claude/scripts/crq-render.mjs` 自动归档。

## 角色分工

| 角色 | 职责 | 禁止 |
|---|---|---|
| story-author（起草） | 盘上起草（updating 态直改 requirements.md，architecture.md 镜像句同批）+ 自检 + 撞车报告 + 改前/改后句级对照（档 2 加牵连面清单） | 不裁定、不自评；写面限 requirements.md + architecture.md（scope-map 强制）；frozen 翻转不归本 agent |
| requirement-reviewer（审稿） | 四查（审盘上 updating 态文件 + 改前/改后对照）+ 否回权 + 独立取证 | 不代笔、不落稿、不翻状态、恒无 Write、与 story-author 不得互兼 |
| architect（仅档 2，默认必派） | 可行性三件：三态 + 逐 story/AC 技术锚 + 影响面概貌 | 不裁语义、不代起草；免派例外见档 2 流程 |
| uiux-author（仅档 2 触 UI 批次，生成模式） | 产 brief 升版 + 设计 HTML + 出图规格单（逐页 400×300 规格）；在途产物随批 frozen | 不发明需求语义之外的 UI；不执行出图（od_* 归主线程）；不碰需求文字面与契约面 |
| 主线程 | 派发 + 状态翻转（frozen）+ §9 记账；dispatch 输入构成显式含「human 意图原话 + author 对照」 | 不改 reviewer 判决、不跳否回循环 |
| human | 裁意图（恒）+ 档 2 终拍 + 跳过例外授出；档 1 事后否回权（§9 append-only）不变 | — |

语义权威恒 = human。触 `docs/contracts/**` 的发现一律路由 architect-reviewer 四分类 + 人类契约门，本流程无权处置。
