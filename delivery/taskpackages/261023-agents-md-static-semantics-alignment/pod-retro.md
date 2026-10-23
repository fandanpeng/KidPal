# pod-retro · 261023-agents-md-static-semantics-alignment

标签:第一人称热记忆(2026-10-23,loop 收尾相位,planner 全程在场的体感记录;不含 tokens/gates/rounds/verdict/test results 的权威复述——那些以 machine artifacts / pod-events / verifier-evidence 为准)。

## 过程体感

- 这是一包「把 prompt 拉回已冻结 spec」的 lite 快路径包,整体手感顺:plan.md 是 architect 逐字终稿 + 六件冻结件内存替换预验证,acceptance-author 与 builder 都几乎没有语义自由度,转录即绿。**前置投资(plan 质量决定 loop 速度)再次得到印证。**
- 卡点只有一处,且是我自己造的:**final verify 轮序号误派 round=3**(沿用了上一包 M5-K3 的 final r3 先例,没核本包 own 序列——本包只跑过 scoped r1,机器 nextRound=2)。integrator 正确 fail-closed 拒跑交回,零损失,但我把 Pod-local 序列当成了跨包惯例来抄。教训:**round 序列是 per-TaskPackage 的,派发前先看 `verifier-evidence/rounds/` 盘面,别背上一包的号。**
- friction 簿记的闭集不够用:launch 期没有通用 launch/dispatch 事件类型,launch-card 缺失只能塞进 `friction/degraded`,语义勉强。低优先级:pod-events 闭集可考虑加 launch 类事件或放宽 friction summary 语义。不阻塞,未升格。
- over-scan 申报纪律运转良好:三个 agent 的超读申报都有实义(acceptance-author 读冻结件学锚组惯例、builder/读码对账是 plan §5 明文要求的复核目标),没有为申报而申报的噪声。这类「清单外阅读有正当事由」的 over-scan 是良性的,不值得任何收紧行动。

## verifier↔fixer 绕轮

零绕轮。verifier scoped r1 与 final r2 一次绿。归因还是上面那条:plan 逐字终稿 + architect 预验证把歧义在起包前烧掉了。

## 别扭的点

- brief-inputs 对 8 个 refs 全部报「无锚点、整文件引用」advisory——本包 refs 天然是多锚的文档面(plan §2 逐条行号),锚点粒度没法也不该退化到文档内 anchor;advisory 合理但对这类包是常驻噪声。
- AGENTS.md 行数口径小分叉(plan §8 写 102→94,实际 HEAD blob 101→93,差 1 = 尾空行计数)——已让 integrator 在 review-packet 如实记录,无实际影响,但「行号基准」在逐字终稿类包里值得写明计数口径(wc -l 还是含尾空行)。

## 下次改进

1. 派 final verify 前先列 `verifier-evidence/rounds/` 盘面定 round 号(本条已可入 checklist 习惯)。
2. lite 快路径包的 redBaselineResult 回填字段,acceptance-author 实跑后我回填——这次流程顺畅,保持。
