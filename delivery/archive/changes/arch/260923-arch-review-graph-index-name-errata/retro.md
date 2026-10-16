# retro · 260923-arch-review-graph-index-name-errata

> 治理闭环第 6 步（趁热写、先于合并）。变更 = 指令根勘误（CodebaseMemory 主索引正名 `KidPal` → `C-AI-KidPal`，4 文件 8 行 + 注册表删旧条目）。

## 1. 这轮做对了什么

- **正名的判据链是硬的，不是「格式好看」**：architect 独立补了三条承重证据——迁移指南 §P7 已成文钉过同一结论（源仓正名就是派生形态）、漂移是机制性的（工具默认派生名 + 文档教人人工名 = 忘传参即复发）、repo-structure 同行自相矛盾。最后一条尤其有用：**它把「文档不一致」从一个观点变成了可 grep 的事实**。
- **副作用处置有顺序**：先 full 刷新派生条目 + 符号级复验 3/3 → 后删旧条目。没有出现「删完发现没有健康条目」的窗口。这条顺序是 architect 主动点的，不是我想到的。
- **landing-verifier 的二分诊断非常值**：它把「内容忠实（语义 9/9 PASS）」与「制品形态（结构门 RED）」干净切开，还做了**诊断强证**——把制品临时重建为契约形态复跑，证明 RED 全部来自 authoring 形态、`diff` 本身零缺口。没有这一步，我可能误判为「要重落」。

## 2. 踩了什么（按严重度）

1. **事后收口时随手补写制品 = 踩形态契约**（本批最大教训）：human 指示「改 Agent md」，我照办、落 commit、再补四件——补件时**没先看一个既有范例**，于是文件名（写了 `landing-manifest.md` 而非 `architect-landing-summary.md`）、Status 行（用 `**freezeState**` 而判据要 `**状态**: FROZEN`）、围栏块（` ```landing-manifest `）、§2 T 锚、packet-id 五处全不合形。机器门当然读不动。**根因不是「忘了格式」，是「没有在动手前把制品契约当输入读一遍」**——这和 K7 那次「起包 schema 凭直觉造」是同一个坑的第二次现形（K7 retro 已记：起包形状照抄既有包 + 对闸源码点检）。
2. **流程时序倒置**：landing（commit 22:25）先于评审与补件。虽然 commit 不可变、diff 可离线逐字复算，保真判定不受影响，但**这违背了「FROZEN 才落地」的顺序**。根因 = human 指示直接改文件时，我没先把治理骨架铺上（reviewDir + PROPOSAL 骨架 + 落地前 FROZEN 标记）。human 后来自己点破了（「你直接改了 agent.md 按照 repo 要求，你估计合并不上去」）——**这一问比我的自检更早发现流程缺口**。
3. **清点引用面时漏了四处**：我只 grep 到 AGENTS.md 与 cli SKILL，漏了 exploring SKILL 三处 qualified_name 前缀示例与 repo-structure 同行消歧句——architect 补上了。教训：**「示例里的名字也是引用」**（qualified_name 前缀 = 项目名，示例即范式，迁移指南 §6 明说过）。

## 3. 人工纠偏体感

- human 两次轻微纠偏都在点子上：① 「`C-AI-KidPal` 反而是对的」——把我按文档转述的判断拉回工具实况；② 「你估计合并不上去」——提前点破治理流程缺口。两次都不是「机械形状」问题，而是**方向性判断**，说明这类变更的判断权确实该在人手上。
- 我唯一该更早做的是：**在 human 说「改 Agent md」时就问一句「走不走治理四件」**，而不是先落 commit 再补。

## 4. 下次改进

- **勘误类也先铺制品骨架**：落地前建 reviewDir + PROPOSAL 骨架（含 Status 行 + ```landing-manifest 围栏 + §2 T 骨架），再动 canonical。写进我的勘误类操作习惯。
- **制品形态当输入读**：任何 reviewDir 制品开写前，先 `sed -n '1,30p'` 一份最近范例（`delivery/changes/arch/*/architect-landing-summary.md`）——本仓形态契约没有单文件规范，范例就是规范。
- **示例即引用**：改文档名/路径类勘误时，grep 口径必须含「示例代码块 / qualified_name 前缀 / 注释里的名字」，不只散文正文。
- **建议升 ARQ（交 architect 判）**：本次五处形态 FAIL 全由「无单文件制品形态规范、靠范例口传」造成——可考虑在 `.claude/references/` 落一份 arch/gov-review 制品形态清单（文件名 + 必需行 + 围栏块 + §2 T 形），让机器门判据有单一可见出处。

## 5. 残项

- `delivery/changes/arch/INDEX.md` 本 change 行 upsert（合并前落）。
- 未纳入本批的两处同族失实（AGENTS.md L39 moderate / `docs/notes/ui-to-lvgl-pitfalls.md` xiaozhi-rlcd）——待 human 裁。
- cli SKILL `delete_project` 示例现指向正名主索引本身（误照抄会删主条目）——建议下批改无害占位。