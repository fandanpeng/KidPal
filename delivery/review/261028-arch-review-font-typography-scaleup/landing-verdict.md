# landing-verdict · 261028-font-typography-scaleup

verdict: PASS

- **verifier**：landing-verifier（独立核验会话；只读 codebase，唯一写入 = 本文件）
- **判定时间**：2026-10-28
- **判据基线**：HEAD = `429643b3`（落地前基线 commit）；landing 未 commit，diff = 工作树 vs HEAD
- **packet-binding**：landing-summary packet-id 行 = 「FROZEN PROPOSAL §2 改法集（T1–T5）+ §5 验证集 · R1 GREEN · change-id arch-review-261028-font-typography-scaleup」，与被批 review id 一致；落地范围 {docs/architecture.md, docs/decisions/ADR.md} ⊆ landing-manifest 围栏（结构门 unauthorized = []）

---

## 1. 结构门（机器，verifier 独立实跑）

`node .claude/scripts/landing-fidelity-check.mjs delivery/review/261028-arch-review-font-typography-scaleup` → **ok: true，exit 0**。

- frozen: true；packetBinding: true
- proposalTargets = summaryTargets = [`docs/architecture.md`, `docs/decisions/ADR.md`]
- unauthorized: []；claimedNotLanded: []；plannedNotLanded: []；gitDegraded: false

交叉复跑 `--preflight`（对 PROPOSAL.md）→ ok: true、findings: []、anchorUnion 恰为两文件 = manifest（与主线程 summary §2.3 自述一致，独立复跑确认）。

**advisory 判读**：`citationAnchorAdvisory.problems = []`；`verbatimQuoteAdvisory.missing` 5 处，独立定性见 §3；`advisory.changedCanonicalNotInProposal` 14 个文件**全部来自 HEAD~1..HEAD commit（429643b3 需求面收口批）**——verifier 实证：`git status --porcelain` 工作树仅 `M docs/architecture.md`、`M docs/decisions/ADR.md`、`?? delivery/review/261028-arch-review-font-typography-scaleup/`，无其它未提交改动，非本批私货。

## 2. 逐文件语义核对（人读 diff，HEAD vs 工作树）

`git diff` numstat：`10 5 docs/architecture.md`、`8 0 docs/decisions/ADR.md`，两文件外零改动。

### docs/architecture.md（T1 / T2 / T3×3 / T5，7 编辑点，semanticMatch: 全 ok）

| 编辑点 | 预期（FROZEN §2） | 实际（diff 实证） | 状态 |
|---|---|---|---|
| T1 §5.5.3 一段 replace → 三段 | FROZEN T1 新文（含折入语式「不把旧三枚追述为已完成运行时接线」） | hunk @294：旧段整段删除，新三段落位；与 FROZEN 文本**逐字对账通过**（程序化 includes 全文匹配 OK） | ok |
| T2 §5.5.2 段尾注记追加 | FROZEN T2 新文（原段全文 + 字体预算注记尾缀） | hunk @294 同区：原段逐字保留，尾缀注记追加；逐字对账 OK | ok |
| T3-1 §3.1 `:121` 行替换 | FROZEN T3 替换 1 新文 | hunk @118：整行替换，逐字对账 OK | ok |
| T3-2 §4 `:158` 映射行替换 | FROZEN T3 替换 2 新文 | hunk @155：整行替换，逐字对账 OK | ok |
| T3-3 §8.1 风险 10 行替换 | FROZEN T3 替换 3 新文（「已闭账」行） | hunk @433：整行替换，逐字对账 OK | ok |
| T5 §12 新行置顶 insert | FROZEN T5 新文（分隔行 + 新行 + 旧首行） | hunk @508：恰 1 行插入于表头分隔行后、`261025-spec-reconciliation-…` 行前；逐字对账 OK | ok |

diff hunk 全集 = 上表 6 个 hunk 头（T1/T2 同 hunk 区），**零夹带**——无 FROZEN §2 未载的改动混入；标题、节编号、锚行全部未动。

### docs/decisions/ADR.md（T4 append，semanticMatch: ok）

- diff 为纯 append：`@@ -140,3 +140,11 @@` 唯一 hunk，8 增 0 删；ADR-001..016 原文**逐字未动**（numstat 删 0 即全量证明，非抽查）。
- T4 落地锚（ADR-016 尾 2026-10-26 注记行）作为上下文行原样保留、无改写。
- 追加的 ADR-017 文本与 FROZEN §6「ADR-017 落地附件」做**逐行程序化对账**（CRLF 归一后）：7 行 vs 7 行，**VERBATIM MATCH**。五小节（日期/决策/理由/备选与放弃原因/关联）齐备，数字区分实测余量与上游 324KiB 估算（估算溯源 = `delivery/workstreams/261028-font-typography-scaleup/review.md:14`「ota_1 余量 302KB < 16px 全量约 324KB」，verifier 抽查确认在源文件在场）。

## 3. cite-check「现文 missing」独立定性

结构门报 5 处 verbatim missing。verifier 逐处对照 `git show HEAD:docs/architecture.md` 独立取证：

| missing 块 | HEAD 版本逐字在场 | 定性 |
|---|---|---|
| T1 旧段（「模拟器 FreeType 快速迭代 / …SPIFFS 加载…」） | yes | 恰为 T1 被替换旧文 |
| T3-1 旧行（「16px 以下真机可读性下限未实测…」） | yes | 恰为 T3-1 被替换旧文 |
| T3-2 旧行（「字体子集管线…待实测…」） | yes | 恰为 T3-2 被替换旧文 |
| T3-3 旧行（风险 10 旧表行） | yes | 恰为 T3-3 被替换旧文 |
| T5 锚（分隔行 + 261025 旧首行） | yes | 恰为 T5 插入位锚文（旧首行仍在场、未删） |

**结论：5 处 missing 全部 = 被替换旧文的预期 missing 形态，非引文造假、非误删**；与 summary §3 诚实注记的定性一致（verifier 独立取证，非采信自述）。6 处「新文」引文全部逐字命中 canonical。

## 4. 破坏性自检

- `git diff --exit-code -- docs/requirements.md docs/contracts/interface-contract.md` → exit 0，零改动（FROZEN §3「未触」申报属实）。
- `git status`：firmware / server / .claude / docs/ui / docs/debt / docs/decisions 之外零改动；唯一 untracked = 本 review 目录。
- `node .claude/scripts/spec-anchor-check.mjs` → ok: true、errors: []、`archSectionCount` = 48（编号零变动，与 FROZEN §5.2 预期一致）。
- ADR append-only：numstat 删 0 增 8；`git grep -c "^## ADR-017 ·"` = 1（exit 0）；`git grep -c "^## ADR-018 ·"` exit 1（零命中）。

## 5. FROZEN §5 验证集独立复跑（verifier 亲跑，非转述）

| 项 | FROZEN 预期 | verifier 实跑 | 判 |
|---|---|---|---|
| 基线 A 容量 | ota_1, app, ota_1, , 0x370000 / assets, data, spiffs, 0x800000, 8M；xiaozhi.bin = 3,294,896 B；ota_1 余量 309,584 B = 302.328125 KiB；generated_assets.bin = 2,790,863 B；assets 余量 5,597,745 B = 5.338426 MiB | 重跑 node 脚本：**逐字一致** | 一致 |
| 基线 C | fonts_root 生产 grep exit 1（零消费者） | `git grep -n "fonts_root" -- firmware/main ':!firmware/main/test'` → exit 1 复现 | 一致 |
| §5.7 ADR 计数 | `^## ADR-017 ·` = 1；`^## ADR-018` = 0；numstat 删 0 增 >0 | 1 / 0 / `8 0` | 一致 |
| §5.8 残留 | 五模式（含「字号下限悬置」）在 architecture 计数 0 | 5/5 计数 = 0 | 一致 |
| §5.8 正向 | §5.5.3 各 ≥1：Assets::GetAssetData / cbin_font_create / font_puhui_14_1 / 16px 正文全量 CJK / 20px 阅读标题全量 CJK | 5/5 各恰 1 次命中于 §5.5.3 节内 | 一致 |
| §5.8 §12 行 | 恰 1 行以 `| 261028-font-typography-scaleup |` 行首 | 行首计数 = 1（全文件子串 3 处 = 行 + 行内引用 + 历史说明，与 summary 口径一致） | 一致 |
| §5.2 spec 锚 | ok: true、errors: []、archSectionCount = 48 | 见 §4 | 一致 |

## 6. FROZEN 程序凭据核对

- PROPOSAL 状态行 = **FROZEN v1.0 · 2026-10-28**，凭据 = human 立项 ack + R1 GREEN（判档主路径、契约 NO_CONTRACT_CHANGE）——在场。
- §7 R1 评审记录：`verdict: GREEN` + 四查全 PASS + 判档 = 主路径（Track B，轻路径判据①②④不满足逐条给出）+ 契约四分类 `NO_CONTRACT_CHANGE`（独立裁定、反证检查在案）——在场。
- FROZEN 前折入记录在案：R1 两条非阻断注记（T1 句式、T2 空格）折入改法文本，落地文本与折入后 FROZEN 文本逐字一致（§2 对账即证）。

## 7. ADVISORY 清单（非阻断）

1. **cite-check 工具形态限制**：landing-fidelity-check v1 对「替换前现文」不区分 replaced 状态，5 处 missing 均为预期形态（§3 已独立定性）；建议后续工具版本带 replaced 语义（不影响本批放行）。
2. **结构门 changedCanonicalNotInProposal advisory**：14 个文件来自 HEAD~1..HEAD 需求面收口 commit（429643b3），非本批私货（§1 已实证）；工具以 HEAD~1 为 base 的形态在同工作树连续两批 landing 时会产生此类噪声，判读时须区分。

## 8. 结论

**verdict: PASS** —— 实际落地 diff 与 FROZEN PROPOSAL v1.0 的 §2 改法集（T1–T5，7 编辑点）+ §6 ADR-017 落地附件**逐字一致**，零夹带、零漏改、零与批定矛盾；两个机械门绿；§5 验证集独立复跑全部与 FROZEN 预期逐字吻合。

**放行建议**：进入下一步（retro 闭环 + 前置升级闸 + human manual-merge 门）。merge 决定权归 human；P0 spike（真机字形、缺资产降级、新字体尺寸回填）不属本批验收，归实现包。
