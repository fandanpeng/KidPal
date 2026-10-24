# pod-retro · 261024-memory-recall-tool-enable

标签:第一人称热记忆(planner 主观过程体感;tokens/gates/rounds/verdict/test results 不载,以 machine artifacts / pod-events / verifier-evidence 为权威)。

## 卡点与体感

- **本包节奏最顺**:acceptance-author 一次冻结出 7 条设计态红、builder 一次全转绿、verifier 两轮零红、integrator 一次收口——Lite 包+精确冻结件的组合效率非常高。对比 K4/K4b 的重轮,本包的差异不在工作量小,而在「规格先于实现」:每一条红都对着一个具体缺失,没有语义争论。
- **锚兼容是本包真正的难点,不是实现**:AGENTS.md 同时挂着 K1 五行锚、K3 六串锚、261023 A3 四锚(其中「不调用任何工具」字面与新放行的检索工具语义上要共存)。派发前我自己把三个冻结件的锚扫了一遍才把「不修改原句、以新行+澄清承载」的写法定下来。这类「提示词考古」应该是 Lite 包派发前的标准动作——不先扫锚,冻结件会以最难堪的方式红。
- **drift_guard 升级基准是我漏想一步的地方**:第一次读 apply 脚本时我只看到「drift 不中 → exit 3」,直到追 live 现状(已四项覆写)才意识到不扩基准则本包连自己的 live 应用都过不去。教训:凡「声明→应用」类包,派发前必核「现状态 vs 目标态 vs 声明容忍基线」三态,否则 live 轮才发现卡在自家闸上。
- **跨包陈旧基线是这类「口径换代」包的固有赝品**:261022-openclaw-honcho-plugin 的 drift 锚四项被 memory_isolation 冻结件逐字钉死,本包落地后它会变成「绿着的陈旧基线」——测试是绿的,但真实语义已过期。这类债不是本包能修的(changeManifest 边界),只能记账 + 建议 follow-up。以后同类换代包应默认在 review-packet 里带一节「陈旧基线扫描」。
- **integrator 的事实更正都很有价值**:AGENTS.md 实际 +3 行、rollback 零 diff、scope 路径未落 fact——三处都如实报了,没让我在后续对账里踩坑。integrator 的「与派单不符处」节值得作为固定结构保留。

## 人工纠偏体感

- 无。全程零 AskUserQuestion、零 HOLD——human 四裁决在起包前已给全,派发无歧义。

## 下次改进

1. 「声明→应用」类包,派发前必核三态(现状/目标/声明容忍基线),drift_guard 升级基准写进 exitCriteria。
2. 提示词锚兼容扫描(跨冻结件 grep 正锚)列进 Lite 包 acceptance 派发的固定前置。
3. 换代类包 review-packet 增「陈旧基线扫描」节(本包已由 integrator 以未覆盖面申报形式承载,可固化)。

## escalation themes(post-cutoff)

- pod-events:零 agent-escalation、零 human-decision(本包起包前已有人类裁决,无需过程中升级);三条 friction 均 over-scan 纯记账。
