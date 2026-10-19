---
id: ARQ-261019-verbatim-quote-line-level-false-negative
title: landing-fidelity 结构门 verbatim-quote 检查按行级匹配——目标句嵌在长行中部时报 not-found 假阴性（byte 比对证实逐字在场），advisory 语义局限误导核对
domain: gov
status: deferred
severity: NIT
owner: architect
trigger: 下一次动 `.claude/scripts/landing-fidelity-check.mjs` 的治理批
opened: 2026-10-19
closed: ""
closedBy: ""
paths: [.claude/scripts/landing-fidelity-check.mjs]
source: [delivery/review/261019-gov-review-local-memory-extraction/landing-verdict.md（ADVISORY-2，landing-verifier 2026-10-19 实测）]
---

**证据**：261019-gov-review-local-memory-extraction 批 landing 期，结构门 verbatim-quote 检查报 T2 落点 A 替换句「not-found-verbatim-in-any-target」；landing-verifier 以 node `indexOf` byte 比对证实该句**逐字在** `docs/templates/PROPOSAL.template.md`（offset 4839）——目标句嵌在整行长行（模板自检块单行多句形态）中部，行级匹配边界对不上故漏报。verdict ADVISORY-2 逐字在案。

**影响面**：落点为长行中部子串的替换句（模板类文件常见——自检块、读法注都是单行多句）必出假阴性 advisory；核验者每次须手工 byte 比对复核一次，门报错可信度被稀释（狼来了效应）。

**建议改动**：verbatim-quote 检查由行级匹配改子串（indexOf）匹配，或对长行先做句子级切分再匹配（候选，交 review、不自批）。

**风险**：riskTier: low——匹配精度提高属报错减少方向，不放宽任何 RED 判定。review profile: full-adversarial + human（触 G3 门脚本，如实标注）。
