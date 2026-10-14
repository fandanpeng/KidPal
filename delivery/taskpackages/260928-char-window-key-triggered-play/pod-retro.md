# Pod Retro · 260928-char-window-key-triggered-play（planner 第一人称热记忆，2026-09-28）

> 只记第一人称过程体感：卡点、绕轮原因的主观判断、别扭处、帮倒忙的钩子/规则、人工纠偏体感、下次改进。不复述、不载 tokens / gates / rounds / verdict / test results（权威 = machine artifacts / pod-events / verifier-evidence）。

## 过程体感

- 起包最顺的一段是 plan.md 直接可用——architect /triage-change 已把逐文件改法、行号快照、取舍和红线自查写死，我起 taskpackage 几乎是转写。acceptance-author 和 builder 都没回改方案，一次冻结、一次实现、零 fixer 轮。这再次印证：上游 reviewed plan 质量直接决定 loop 长度。
- 我自己的两次红都是**起包侧簿记**而非实现侧：第一次 taskpackage-required 红（origin.upstreamType 闭集外自造值、contractRefs 空数组、decisionCoverage 用了自创字段形态）；第二次 mutation-check 红（mutationRequired=false 没带 mutationSkipReason）。自造字段形态是本棒的根因——闸的 schema 在脚本里都有，但我凭上一包样子"像就行"地写，没有先跑一遍 `taskpackage-required-check.mjs` 预检。**下次改进：finalize taskpackage 后、首次派发 verify 前就直跑一遍 taskpackage-required-check + mutation-check 预检**，省一整轮。
- decisionCoverage 的 landsIn 格式我第一次写成散文（"plan 方案 A4——取…"），闸要求受控格式（acceptance:<id> / exitCriteria:<id> / dropped+reason）。还有 acceptance 条目 id 提取按 ASCII 冒号切割，我用了全角冒号接括号导致 id 提不出。这两处都是"闸比 prose 严"的典型——闸判据文档化在脚本注释里，起包侧靠踩才知道。**建议（非门）：DEV-GOVERNANCE 起包格式清单里 decisionCoverage 受控格式值得一句点名。**
- verify final 相位先踩了一次空轮：我按 scoped r3 顺延猜 round=5，闸报「round 4 is not next completed ordinal」——序数必须连续，r4 那次缺 scope-file 在写 evidence 前就 fail 了、不占序。机械闸 fail-loud 干净利落，没有制造假绿，这个体验是好的。
- write-serialization 风险这次没踩（taskpackage 全程用 Edit/python patch 局部改，没整文件重打大段中文）。
- over-scan 申报面两个 subagent 都自觉报了（acceptance-author 6 行、builder 11 行），均属实现必需定位，无发散——input-brief 的申报纪律已在两代 agent 身上形成习惯。

## escalation themes

- 无 agent-escalation 事件（cross-reference pod-events：仅 dispatch-completed ×3 + friction over-scan ×2 + human-decision 无）。本次零升级是 Pod 形状简单（单包、Lite、零契约门、零 human 门）的自然结果，不视为常态基线。

## 下次改进（自留）

1. finalize 后先机检预检再进 verify（上面已述）。
2. final 相位 scope-file 依赖 integrator Step 4 产物——本次我先派了 verifier final 才意识到缺 scope-file，白走一次 preflight。**派 integrator 的 dispatch prompt 里就把 final-scope.txt 路径约死**（本次已做到），且 planner 记住顺序：integrator 先行、final 后跑。
3. pod-retro 趁热写完了——这条本身在按新纪律执行。
