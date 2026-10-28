---
id: ARQ-261028-mutation-evidence-authoring-pointer
title: mutationEvidence 须 acceptance 原文逐字的口径文面在场（taskpackage-authoring.md §5）但实际起草者面零指针——pod-m2 填短 id 撞闸假红、白烧一轮
domain: gov
status: deferred
severity: SHOULD
owner: architect（评审裁量落点）
trigger: 下一治理批并批消费（/gov-change）；或同类假红再复发一次
opened: 2026-10-28
closed: ""
closedBy: ""
paths: [".claude/agents/planner.md", ".claude/references/taskpackage-authoring.md", ".claude/scripts/mutation-check.mjs"]
source: ["C:/AI/KidPal-pod-m2/delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/verifier-evidence/rounds/r1.json", "C:/AI/KidPal-pod-m2/delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/pod-retro.md（卡点与判断节）"]
---

**证据**：pod-m2 包 `261028-p3-record-session-mirror-desync-fix` scoped r1——r1.json `mutation-check` 门 `ok:false / exit 1`，报错逐字 `Mutation evidence is missing or insensitive for some acceptance ids`，`missingSensitive` 点名 RSL2-A1–A4 全族四条。pod-retro 第一人称复盘逐字：「读脚本才明白：它按**完整 acceptance 原文**做 id 匹配，而我填的是短 id。这是我的文书错误，不是代码错误，也不是闸的问题……代价是白白烧掉一个 round 序数」。**文面承载查明在场**：`.claude/references/taskpackage-authoring.md:30-32` §5「mutationEvidence 用 acceptance 原样完整文本——不转述、不缩写」；指路亦在场一处：`.claude/references/dev-loop.md:42`「字段级撰写约定见 taskpackage-authoring.md——撰写侧单一入口」。**断链在起草者面**：实际落 TaskPackage 的两处字段清单零指针——`.claude/agents/planner.md:175` 与 `:209`（字段清单含 mutationEvidence 语义位）、`.claude/agents/acceptance-author.md:22`（Draft TaskPackage 字段清单）均不引 taskpackage-authoring.md；`task-intake-and-slicing.md:268` 非空必填行亦无该口径。且 `.claude/scripts/mutation-check.mjs:168` 的 fail 报错文本不带口径提示（missingSensitive 只回显 acceptance 原文，不说「evidence 须与其逐字一致」），作者无法从闸红反推修法。同族形态先例 = ARQ-260913-auto-mode-classifier-legit-workflow 追记的「生产者可达性断链」：先例文面在场、消费点不指路，采纳纯靠自发翻阅。

**影响面**：凡带 `mutationEvidence` 的包（acceptance 有 functional 断言的包均要求）——口径文面在场但起草者按常规路径（planner.md / acceptance-author.md）读不到，每次起包都是一次盲抽；踩中即假红一轮（verify round 序数不可回收）。本包为首个实证（25 包 retro 时代未见同形红，说明当前命中率低但非零）。

**建议改动**（交 review、不自批，两臂不互斥）：① 文面臂（G2）：`.claude/agents/planner.md:209` 字段清单与 `acceptance-author.md:22` Draft 清单各加一行指针——「mutationEvidence 须 acceptance 原文逐字（taskpackage-authoring.md §5）」；② 闸报错臂（G3，改 `.claude/scripts/mutation-check.mjs:168` 报错文本）：`missingSensitive` 附一句口径提示「evidence 条目须内嵌对应 acceptance 条目完整原文（逐字匹配）」。倾向 ①（文面轻、起草者必到）；② 可作搭车。

**风险**：riskTier: low——文面臂纯指针；闸报错臂只改 fail 文本不动判定语义（`evidenceMatchesId` 判据不动）。review profile：文面臂 = 轻路径候选（判据①②按起草原样评）；闸报错臂触 `.claude/scripts/**`（G3 改闸）→ 恒 Track B 主路径（gov-change 判据③）。两臂取合交 architect-reviewer 判档。
