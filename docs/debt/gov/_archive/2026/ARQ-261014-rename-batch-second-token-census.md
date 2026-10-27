---
id: ARQ-261014-rename-batch-second-token-census
title: 改名批 census 需覆盖两个 token 族（旧路径 + 旧文件名 stem），本批 R1 打回实证
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一次涉及重命名文件 / 脚本 / 模板的治理批起草期
opened: 2026-10-14
closed: 2026-10-27
closedBy: 261027-gov-review-p7-devloop-doc-cluster
paths: [".claude/references/path-form-family.md"]
source: ["delivery/changes/arch/261014-gov-review-crq-debt-rename/retro.md §3-F2", "delivery/changes/arch/261014-gov-review-crq-debt-rename/PROPOSAL.md §7 R1 BLOCK-R1-1"]
---

**证据**：261014-gov-review-crq-debt-rename 批 R1 评审判官实测——起草期 T1③ 只覆盖目录 token `docs/crq` 字面量，旧文件名 stem（`crq-render` / `crq-doctor` / `crq-ledger-shared` / `crq-item`）以自指串（`GENERATED_MARKER` / indexHeader 模板 / self-check 输出串）、指令面命令行（11 件 file:line）、裸文件名引用三形态散布，V1 六条路径形态模式无一可命中，落地后即断链（retro §3-F2）。

**影响面**：所有「重命名文件」类治理批（脚本改名 / 模板改名 / 命令改名）；不修则每次都复刻「V1 全绿而旧名引用全残」的判据盲区，靠判官打回兜底 = 每批多付一轮评审。

**建议改动**：在 `.claude/references/path-form-family.md`（或 PROPOSAL 模板起草自检句）补一条「改名批两 token 族」起草义务：凡重命名类变更，census 同时枚举旧路径 token 与旧文件名 stem token 的引用面，各配清扫子检（本批 V1 第七条 `crq-(render|doctor|ledger-shared|item)` 即可迁移的子检形态）。

**风险**：low——纯起草清单增补，不改任何门与闸；族谱表 SSOT 的 append 义务（表 §6-4）与「形态 vs token 族」分界需在条目落地时一并厘清（本批 R2 已裁「stem 引用属第二 token 族、非新形态类」，先例在案）。

**结项追注（2026-10-27，261027-gov-review-p7-devloop-doc-cluster T2）**：修法已落地——path-form-family.md 新增「六、第二 token 族：文件名 stem（改名批专用）」节（双 token 族 census 义务 + stem 交替组子检模式，261014 批 R2「第二 token 族、非新形态类」分界先例钉进节首），PROPOSAL.template.md 自检句 ② 内 append 触发句指回该表。R1 GREEN、落地五件 sha256 对账一致。
