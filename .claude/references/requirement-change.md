# requirement-change · 需求面变更流程（三档）

> 权威面：本文件是需求面变更的流程 SSOT（260928-gov-review-requirement-face FROZEN v1.0 立）。story **怎么写**归 `write-story` skill，本文件管**谁起草、谁审、怎么定稿**。triage intake 跨面接线消费本文件判据（ARQ-260928 批，留根）。

## 三档判据

| 档 | 范围 | 边界例 |
|---|---|---|
| 0 · 勘误/措辞 | 不改语义：错字、排版、表述澄清、锚行格式 | 跨 story 零语义清扫（锚行格式批改、勘误连带同步）= 档 0 |
| 1 · 语义修改 | AC 增删改 / 补充规则口径 / 单 story 内行为变更 | NFR 数值微调 = 档 1 |
| 2 · 结构性 | story 新增 / 重写 / 砍除、跨 story 连锁、NFR 新增/删除/阈值语义变更、Epic 级 | — |

**升档规则**：档 1 起草期发现牵连面（关联 story / §2 共享底座 / 线框需同步）即携带清单；牵连 ≥2 处或含 NFR/Epic → 升档 2（升档裁定：主线程提呈、human 确认）。

## 各档流程

### 档 0 · 勘误/措辞

主线程直改（挂 write-story 完稿自检 + spec-anchor-check 机检）→ §9 追加变更行，标「勘误」。无新角色。

**跳过例外**：档 1 级语义修改，human 明确说跳过流程的一次性操作允许直改（对齐 AGENTS.md Hard Rules 既有条款）；§9 记账须写明「跳过 story-author/reviewer」。

### 档 1 · 语义修改

1. **human 裁意图**——原话即起草依据（含改什么、为什么）。
2. **story-author 起草**——修订稿 + 完稿自检 + spec-anchor-check + 相邻 story 撞车报告，同会话 return 交主线程。
3. **requirement-reviewer 四查**——主线程以 dispatch prompt 转「human 意图原话 + 修订稿 + 撞车报告」；不过否回循环至过。
4. **过即落稿**——主线程写 requirements.md + §9 追加版本行。**无 human 终确认**（human 已裁：意图事前对齐 + reviewer 意图保真核查已覆盖；§9 append-only 可事后否回）。

### 档 2 · 结构性

1. **human 裁意图与取舍倾向**。
2. **story-author 起草**（含牵连面清单）。
3. **architect 可行性判断**（出场由 human 指定）：可行性三态 ✅/🟡/🔴 + 实现代价 + 影响面，只出意见不裁语义。
4. **story-author 按可行性修订**——🔴/取舍点升回 human，不带病下行。
5. **requirement-reviewer 四查**（重点查牵连面）。
6. **human 终拍**——architect 结论 + reviewer 结论一并呈；拍板 → 主线程落稿 + §9 版本行；review 留痕 = §9 该行依据列浓缩承载 architect 结论 + reviewer 结论。

分工一句话：architect 管「能不能」、reviewer 管「准不准」、human 拍「要不要」。

## 定稿载体

- story 修订定稿的标记 = **§9 版本行本身，版本号即 changeId**（不另铸批次 ID——TaskPackage changeId 强语义见 task-intake-and-slicing.md，撞名禁用）。
- 实现包引用 story 在 write-story 合法形态（`S-Fx.y · 标题`，可选 AC 号集）之上可加「定稿版本号」；launchPreconditions 消费挂点归 ARQ-260928 批（留根）。

## REQ- 域 CRQ 闭环纪律

`docs/crq/requirement/` 条目（前缀 `REQ-`）必填三件：

- **owner**：指向需求变更流程某档或 human——禁空。
- **trigger**：何种事件触发捡起——须可判，禁「human 想起」形态。
- **销行条件**：满足何状态可销——通常 = 对应需求变更 §9 定稿；销行跑 `node .claude/scripts/crq-render.mjs` 自动归档。

## 角色分工

| 角色 | 职责 | 禁止 |
|---|---|---|
| story-author（起草） | 按意图转五件结构修订 + 自检 + 撞车报告（档 2 加牵连面清单） | 不裁定、不自评、无 Write |
| requirement-reviewer（审稿） | 四查 + 否回权 + 独立取证 | 不代笔、不落稿、与 story-author 不得互兼 |
| architect（仅档 2，human 指定） | 可行性意见（三态 + 代价 + 影响面） | 不裁语义、不代起草 |
| 主线程 | 派发 + 落盘 + §9 记账 | 不改 reviewer 判决、不跳否回循环 |
| human | 裁意图（恒）+ 档 2 终拍 + 跳过例外授出 | — |

语义权威恒 = human。触 `docs/contracts/**` 的发现一律路由 architect-reviewer 四分类 + 人类契约门，本流程无权处置。
