---
id: ARQ-261027-summary-verify-obligation
title: 主线程写 landing-summary 的核验断言无亲测义务——P6.5 批「junction 已不存在」失实句采信 architect 自报入档，被 landing-verifier 实测抓出
domain: gov
status: deferred
severity: NIT
owner: architect
trigger: 下个触 landing-summary 模板或主线程落地纪律文面的治理批
opened: 2026-10-27
closed: ""
closedBy: ""
paths: [docs/templates/architect-landing-summary.template.md]
source: [delivery/review/261027-gov-review-draft-cost-relief/landing-verdict.md（ADVISORY-1）, delivery/review/261027-gov-review-draft-cost-relief/architect-landing-summary.md（更正段）, delivery/review/261027-gov-review-draft-cost-relief/retro.md（F1）]
---

**证据**：261027-gov-review-draft-cost-relief 批 landing 期实录——主线程写 landing-summary §3 时，把「junction 由 architect 测试后即时清理（`C:/tmp/dcr-jrepo` 已不存在，主线程落地期复核确认）」写入 N1 处置注记，实际未亲测（采信 architect 自报）；landing-verifier 六项核验第 6 项（诚实披露核对）实测 junction 残留，判 ADVISORY-1「披露失实」。判官 / 核验官简报均有「地面真值亲核义务」硬约束，主线程写 landing-summary 时**没有对应义务句承载**。

**影响面**：凡主线程写 landing-summary 的批每批一次暴露面——「诚实注记」段的核验型断言（X 已清理 / Y 已复核 / Z 实测 = N）若采信转述，失实句随 commit 进档案；landing-verifier 第 6 项能抓，但抓到即 ADVISORY + 更正留痕，每批多付一轮往返。

**建议改动**（候选，交 review、不自批）：`docs/templates/architect-landing-summary.template.md` 诚实注记段加一句义务——「本段中的核验型断言（已清理 / 已复核 / 实测 = N）须主线程亲测后才可写入；采信他方自报的必须显式标注来源，不标注即失实」；配套在 §5 验证集模板面加一条可判形式（如「summary 核验断言逐条带命令留痕」）。

**风险**：riskTier: low——纯文面义务句，不改任何门判定；review profile：Track B（三要件全 yes：①执行测出 ADVISORY-1 实录；②修法明确无真分叉；③半径 = 每批 landing-summary 写作暴露面已被执行揭示）。paths G2（docs/templates），零硬排除。owner=architect。
