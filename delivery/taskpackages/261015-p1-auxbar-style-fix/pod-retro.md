# pod-retro · 261015-p1-auxbar-style-fix

> 第一人称热记忆（planner 全程在店主观体感）。不复述、不冠权威、不载 tokens / gates / rounds / verdict / test results——这些以 machine artifacts / pod-events / verifier-evidence 为权威（负向边界）。

## 卡点与绕轮体感

- 本包唯一一次 verifier↔形态修正绕轮（r1 mutation-check 红）**是我自己造成的**：回填 mutationEvidence 时把 `perAcceptance.acceptance` 写成缩写占位（「acceptance[] 对应条目全文」字样），而机械闸以 acceptance[] 条目全文为 raw id 做 includes 匹配——261010 包先例⑤条文「逐字复制」我明明引了却没照做。教训：**条文里的「逐字」就是逐字**，骨架回填时占位符不能过夜，acceptance-author 冻结回执一到就该全文落。同类风险：AB-A4/A6 守卫 case 我 draft 时没标 `kind:'structural'`，acceptance-author 回执里其实已写明「取证豁免 / 守卫 case」语义，我在回填时只顾四条 mutation 行、漏了两条豁免行的机械通道（structural 标注）——下次冻结回执落地时按「每条 acceptance 问一句 mutation 语义」过一遍，别只处理显眼的。
- 一处笔误自摆乌龙：builder 派单里写「176 Tests（172+4）」——新冻结件是六个 case 不是四个。计数这种小事也该从冻结件 TEST_CASE 数直接引用，不要心算。

## 编制 / 环境体感

- **uiux-author 不可派是真卡点但没伤到流程**：planner Pod 会话可派集没有 uiux-author（定义文件在册、scope-map 在册，但 harness 会话不含），而本包恰好是同款先例（261011）里由主线程派 uiux-author 的形状。降级处置（母版同步并入真机轮 human 门同点）对 Lite 包成立，但对未来触 docs/ui 面更大的包，这个洞会更大——建议治理面确认 planner Pod 的 cast 是否该补 uiux-author，或把「planner Pod 触 docs/ui」列为 launch-prep 检查项。
- 预存管线失配二件（p1_chip_mingzao.bin↔assets.c、verify_art_assets 预算对账、gen_art_spiffs 头注 68 实为 75）都是 builder 在确定性自检里挖出来的——「交付面收敛 + 还原 + 申报、不静默重烘」这条纪律运转得很好。这类陈旧账建议尽快立 tech-debt，否则每个触资产链的包都要重付一次发现成本。
- brief-inputs 的 ADV（ref 无锚点整文件引用）对 Lite 包无害，但 plan.md 这类核心文档天然该整读，advisory 噪音可接受。

## 升级主题（cross-reference pod-events）

- d11026fc agent-escalation（environment：uiux-author 不可派）→ 1912f2df resolved（并入真机轮 human 门）。全程唯一 escalation，无其他升级。
- friction 二条：预存管线失配（degraded）+ acceptance-author 超读申报（over-scan，3 文件均为必要读）。

## 下次改进

1. 骨架回填纪律：acceptance-author 冻结回执到达后，perAcceptance 与 kind 标注一次到位（逐字全文 + 每条问 mutation 语义），不占位。
2. 派发前置自查：任务书里的计数、行号一律从冻结件/源文件引用，不心算。
3. 触 docs/ui 的包在 intake 时先探可派集（Agent 类型表），提前定母版落盘路径，别到派发时才发现 cast 缺口。
