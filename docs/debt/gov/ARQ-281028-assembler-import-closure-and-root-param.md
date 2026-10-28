---
id: ARQ-281028-assembler-import-closure-and-root-param
title: 装配副本缺被改脚本的 import 闭包致自检跑不起来；判别脚本硬编码副本根需落地期 sed——落地期两处机械摩擦
domain: gov
status: deferred
severity: COULD
owner: architect（装配器与 scratch 模板）
trigger: 下一个使用 assemble-edits 的治理批起草期或落地期
opened: 2026-10-28
closed: ""
closedBy: ""
paths: [".claude/scripts/assemble-edits.mjs"]
source: [delivery/review/261028-gov-review-p8-gate-semantics-cluster/retro.md §3 F4/F5]
---

**证据（P8 落地期实测）**：① 装配副本上跑三闸自检报 ERR_MODULE_NOT_FOUND（`verifier-evidence-check.mjs` + `lib/` 三件不在副本）——装配器 copy 清单只含本批新文件，未含被改脚本的共享依赖；主线程手工自仓补复制后才跑得起来。② `_scratch/mutation-check.mjs` 与 `evaluate-five-cases.mjs` 首行写死判官基线根 `C:/tmp/p8-assembly9`，落地复跑须 sed 改指新副本根。

**影响面**：每个用 assemble-edits 的治理批的落地期——两处合计约 5–10 分钟手工 + 一次易错环节（sed 改路径漏改即判别结果指向旧基线，结论失真）。

**建议改动**（候选，交 review、不自批）：
1. 装配器对被改 `.mjs` 扫 import 闭包，依赖不在 copy 清单且不在输出根时打 advisory（或提供 `--with-imports` 开关自动补复制）；
2. scratch 判别脚本模板首行改 `const root = process.argv[2] || '<默认副本根>'`，根路径参数化，落地复跑免 sed；
3. 落地 runbook（gov-change.md 落地节）加一句「副本自检前先补 import 闭包 / 判别脚本传根参数」。

**风险**：riskTier: **low**——纯起草工艺工具改进（装配器 advisory / scratch 模板 / runbook 一句），零门判据、零 canonical 判据面；便宜验证 = 下批装配复跑观察 advisory 与 argv 根在位。review profile = **Track B**（三要件：① 执行测出 yes——P8 retro §3 F4/F5 实测；② 修法明确 yes——advisory + argv 两小改，无设计分叉；③ 爆炸半径已被执行揭示 yes——落地期实测两处摩擦）。owner = architect（装配器）。
