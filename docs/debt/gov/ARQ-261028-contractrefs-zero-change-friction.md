---
id: ARQ-261028-contractrefs-zero-change-friction
title: 零契约改动包被迫列一个不改的契约过 contractRefs 非空闸——「零写注记串」惯例文面在场但起草者面断链，字段名误导读者以为碰过契约面
domain: gov
status: deferred
severity: COULD
owner: architect（评审裁量落点）
trigger: 下一治理批并批消费（/gov-change，可与 ARQ-261028-mutation-evidence-authoring-pointer 同批——同族断链同修法形态）；或复发一次
opened: 2026-10-28
closed: ""
closedBy: ""
paths: [".claude/agents/planner.md", ".claude/agents/acceptance-author.md", ".claude/references/taskpackage-authoring.md", ".claude/scripts/taskpackage-required-check.mjs"]
source: ["C:/AI/KidPal-pod-m2/delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/pod-retro.md（帮倒忙的地方节）"]
---

**证据**：pod-m2 包 `261028-p3-record-session-mirror-desync-fix` pod-retro「帮倒忙的地方」节逐字：「`taskpackage-required-check` 要求 `contractRefs` 非空，即便本包**零契约改动**。为了过闸，我只能把 `docs/contracts/interface-contract.md` 作为『被引用但未改动』的契约列进去……字段名会让人误以为本包碰过契约面。同文件里 `crossEnd.exempt` 已经用一句话写清『零 wire 改动』，两处口径并存反而增加读者负担」。**文面承载查明在场**：`.claude/references/taskpackage-authoring.md:26` §4 已有「零写注记串」惯例——「不改动的 contractRefs / docRefs 清单显式写空注记，不省略说明」；`.claude/references/task-intake-and-slicing.md:268` 亦有「无契约命中时按既有口径填契约面引用或显式说明」。**断链同族**：两处文面都在撰写入口 / intake 清单内，但零契约包的起草者按 planner.md（:175/:209）与 acceptance-author.md（:22）常规路径起草时均不携带该口径；`.claude/scripts/taskpackage-required-check.mjs` 的非空红也不给修法提示。retro 自评「这在语义上是诚实的……但增加读者负担」，属可用性摩擦而非判据缺陷。

**影响面**：凡零契约改动的包（多数实现包）——每次起包都在「列一个不改的契约」与「找口径文面」之间撞一次；不修的代价是持续的读者负担 + 契约触碰面误读风险（review 侧需额外甄别「列了 ≠ 改了」）。

**建议改动**（交 review、不自批，两臂不互斥）：① 文面臂（G2）：planner.md / acceptance-author.md 起草点加指针，并把 taskpackage-authoring.md §4 的零写注记串惯例对 contractRefs 显式化一句——「零契约改动 = 列被引用契约 + 逐条零写注记串（如 `ref-only: 未改动，仅交叉引用`）」；② 闸报错臂（G3，改 taskpackage-required-check.mjs 非空红文本）：红时附口径提示「零契约改动照填被引用契约并带零写注记串」。retro 另提的「契约面零改动显式取值」（改闸判据加新取值形态）成本更高、涉及判据语义，**不预批**，若 review 认为注记串不够再议。

**风险**：riskTier: low——文面臂纯指针与惯例显式化；闸报错臂只改红文本不动非空判据。review profile：文面臂 = 轻路径候选；闸报错臂触 `.claude/scripts/**`（G3 改闸）→ 恒 Track B 主路径（gov-change 判据③）。两臂取合交 architect-reviewer 判档。
