# architect-review · 260923-arch-review-graph-index-name-errata

> **评审人**：architect（advisory 意见作者转评审收口；Track B 评审循环，只读 canonical、只写本评审记录）
> **评审对象**：同目录 `PROPOSAL.md`（freezeState: pending-human → FROZEN 待本记录 GREEN）
> **评审日期**：2026-09-23
> **只读核验范围**：PROPOSAL 全文；commit `5530aee` 全量 diff（stat + 四文件逐行）；仓内旧名残留 grep 复核（活面 + 历史制品）；`docs/templates/GOVERNANCE-MIGRATION-GUIDE.md` §P7；`.claude/rules/repo-structure.md`。

## 1. 逐项裁定

### 1.1 正名判据（PROPOSAL §2）——成立

三条承重证据逐条复核属实：

1. **在册先例**：`GOVERNANCE-MIGRATION-GUIDE.md` §P7（:57）明文「项目名必须与目录名一致，否则图工具报 project not found——本仓已实证踩过」，源仓正名 `C-AI-AInativecompany` 即派生形态。human 直觉「与以往命名格式一致」与在册先例吻合——「以往的格式」本就是路径派生，`KidPal` 才是例外。
2. **机制性漂移**：注册表 32 项中唯二非派生形态 = `KidPal`（人工）与 `KidPal-claude`（特例豁免）；工具默认即派生名，保留人工主索引名 = 任何一次忘传 `name` 的刷新即复发双条目（2026-09-23 实测一次，本批根因）。改用派生名把「约定」对齐「工具默认」，整类漂移消失。
3. **仓内自相矛盾**：`repo-structure.md` 原 L26 同一行「主 checkout 用主索引 `KidPal`」与「项目名由工具按路径派生」并存——正名后自洽（diff 已实证）。

反向权衡（短名可读性 vs 4 文件迁移成本）不足以翻案。**裁定：正名 `C-AI-KidPal` 成立**，`KidPal-claude` 特例豁免必要且已在 L51 新条文显式声明。

### 1.2 Landing 清单一致性（PROPOSAL §3/§5 vs commit `5530aee`）——一致，无遗漏无夹带

commit stat 独立核验：**仅触 4 文件、8 insertions / 8 deletions、无任何其它文件混入**。逐处对照：

| PROPOSAL 清单 | diff 实证 | 判 |
|---|---|---|
| AGENTS.md L43 表格行 | ✓ 主索引名格换 `C-AI-KidPal`，排除列原样 | 符 |
| AGENTS.md L51 规则条 | ✓ 整条重写，含「不传 name / 先 list_projects 兜底 / KidPal-claude 特例」三要素 | 符 |
| cli SKILL.md :36/:42 | ✓ 两示例项目名替换 | 符 |
| exploring SKILL.md :44/:45/:64 | ✓ 三处 qualified_name 前缀替换（advisory 补的清单外三处**均已在册且已落**） | 符 |
| repo-structure.md L26 | ✓ 消歧句主索引名替换，该行「项目名由工具按路径派生」句正名后自洽（advisory 补的清单外一处**在册且已落**） | 符 |

**计数口径注记（如实记录，非缺陷）**：PROPOSAL §3 头写「4 文件 7 处」，其表枚举 8 处，diff 实际 8 行变更——「7」是把 exploring `:44`/`:45`（同一示例块相邻两行）计作一处。计数措辞源自 architect advisory 原文的同一算术滑差，PROPOSAL 继承；**逐处清单一一对应、无遗漏无夹带，计数口径差异无实质影响**。建议 FROZEN 时不改（内容无误）；INDEX/retro 若引计数以「4 文件 8 行」为准。

**注册表侧断言**（`C-AI-KidPal` full 刷新 nodes 17952 / edges 48374 + 符号级复验 3/3 + 删旧条目 `KidPal`）：非本评审 read-glass 可直证，采信主线程实测记录（landing-manifest :33 载 `delete_project → {"status":"deleted"}`）。**要求 landing-verifier 独立 spot-check**：`list_projects` 同 root 仅存 `C-AI-KidPal` 单条目 + `index_status` 健康。

### 1.3 副作用处置顺序（PROPOSAL §2 末段）——正确

顺序 = 先 full 刷新派生条目 → 符号级复验 3/3（K7 三符号 `grid_restore_enabled` / `grid_restore_payload` / `kidpal_render_node_schedule_on_screen`）→ 后删旧人工名条目。该顺序的关键性在于 `KidPal` 恰是刚被刷新验证过的一份，直接删会先丢已验证数据；先补刷派生条目再删旧，全程无「无健康条目」窗口。删条目无需备份（图数据 = 仓源码机器再生面，快照 + full 重索引可重建）。**裁定：顺序正确。**

### 1.4 路由归类（PROPOSAL §6）——仍成立

- **勘误类**：HQ-1 判定标准反用成立——全部改动不改变任何 authority/owner/writer/approver、门/边界/升级条件，改的只是「图索引实际叫什么名」这一事实陈述及工具默认行为的成文化。
- **非「新 idea」**：不触 `/arch-change` 硬排除档（G1 新治理机制/权限变更）。唯一新条文「不传 name、按派生名」= 工具默认行为的成文化，非新机制。
- **不触契约**：`docs/contracts/**` 零改（commit stat 实证）。
- **G1 approver 在场**：变更由 human 发起并定向（原话录于 PROPOSAL 头）。

### 1.5 时序如实记录

landing（commit `5530aee`，2026-09-23 22:25）**先于**本评审记录发生——本批为 human 裁定「补治理四件再合」的事后收口，commit message 已如实自记「未开 PROPOSAL」再由本批补件。本 GREEN 判定语义 = **已落 diff 与勘误方案逐处一致、无夹带、可 FROZEN 补件**，非事前 landing 门。该时序与仓内「human 直改 + 如实记录」勘误先例（commit message 记录依据 + human 指示）的纪律一致，补件后治理链完整。

## 2. Verdict

**GREEN（可 FROZEN）**

理由汇总：正名判据三条承重证据成立（§1.1）；landing 清单与实际 diff 逐处一致、探索面与规则面清单外补漏均已收（§1.2）；副作用处置顺序正确（§1.3）；路由归类三项判据均成立（§1.4）；时序偏差已如实记录且不损内容正确性（§1.5）。

无打回项、无条件项。唯一附加义务 = §1.2 尾注册表 spot-check（转 landing-verifier，非 FROZEN 前置）。

## 3. 残余风险（择要保留自 advisory §5）

1. **不入 grep 清零口径的历史散文**（重点保留）：正名后仓内仍带旧名 `KidPal`（作主索引名）的历史面实测包括 `delivery/taskpackages/260829-*/intake-notes.md`（char-stroke :57 / hardware-validation :68 / voice-roundtrip :28）、`delivery/changes/dev/260829-char-stroke/intake-notes.md:57`、`delivery/workstreams/_archive/2026/kidpal-mvp/progress.md:97` 等启动卡/归档/memory 类。历史 append-only 不追改，兜底 = L51 新句「报 project not found 先 `list_projects`，不凭记忆猜」。**canonical 活面（AGENTS.md / skills / rules）grep 零残留，已实证。**
2. **双条目复发兜底**：未来会话按旧肌肉记忆调 `index_repository(name="KidPal")` 会重建人工名条目；文档非机械保证。复发处置口径 = 删人工名条目为准。
3. **delete_project 示例指向风险（正名后新增形态）**：cli SKILL.md `:42` 示例正名后指向 `C-AI-KidPal`——即**正名主索引本身**。误照抄执行会删掉主条目（可再生、但产噪音与 watched 归属扰动）。建议下批小勘误把该示例改为无害占位形态；本批不阻塞。
4. **auto-refresh / watched 归属**：删旧条目后 watched 集不应留死条目；主线程删前刚做过带 name 刷新，删后需 `config list` + `index_status` 核一次。
5. **Pod 快照流**：分发链按文件不按项目名、`.claude/scripts` 零硬编码项目名（advisory 期独立 grep 复核）——低风险，下一包 launch 实测作证。
6. **`xiaozhi-rlcd` 名不符实疑点**：`docs/notes/ui-to-lvgl-pitfalls.md:124` 与 AGENTS.md 表格参考项目名不一致——同族隐患第二实例，留待下批核。
7. **L39「moderate 模式」失实**：AGENTS.md L39 与 full 实测（符号削薄约 44%）不符——留待下批。

## 4. Human 决策记录（两处）

1. **正名裁向**：`C-AI-KidPal`（2026-09-23 human 原话见 PROPOSAL 头引；architect advisory 独立复核同意，本记录 §1.1）。
2. **顺带发现处置**：L39 moderate 失实 + `xiaozhi-rlcd` 名不符实——**留待下批由 human 定夺**是否并入同类勘误（PROPOSAL §4 已如实登记为「未纳入本批」）。

## 5. 转交 landing-verifier 的核验点

- `git show 5530aee --stat` 四文件 8 行、无夹带（本评审已复核，verifier 独立重跑）；
- `list_projects`：同 root `C:/AI/KidPal` 仅存 `C-AI-KidPal` 单条目；
- `index_status`（`C-AI-KidPal`）健康；
- K7 三符号任取其一 `get_code_snippet` 命中（符号级，节点数不作数）。
