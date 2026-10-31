---
id: ARQ-281028-arq-entry-plain-language
title: ARQ 条目散文缺「问题白话段」——human 读条目懵、需主线程二次转译才能拍修法
domain: gov
status: deferred
severity: SHOULD
owner: architect（模板与写法义务修订）+ 主线程（存量触碰时顺手改写）
trigger: 下一个动 docs/templates/debt-item.template.md 或 gov-arq skill 写法义务的批；或下一次立项复证发现 human 对条目表述懵
opened: 2026-10-28
closed: ""
closedBy: ""
paths: ["docs/templates/debt-item.template.md", ".claude/skills/gov-arq/SKILL.md"]
source: [delivery/workstreams/gov-debt-reduction/task_plan.md P8 节立项 ack 记录]
---

**证据（2026-10-28 P8 立项当场实证）**：主线程按 P8 两条条目（`ARQ-261014-supervisor-escalate-preflight-ignored` / `ARQ-261001-host-blind-outcome-device-gate`）原文向 human 出修法取舍选项，human 两问均答「看不懂」——原话：「没太看懂，这到底是什么问题？所以给我的改法，我也没法判断」「还是详细给我解释一下这个问题，让我好理解一点」。主线程用大白话重讲两个问题（闸为何反复人批同型包 / host 测试全绿为何真机现象仍错）后，human 立即两拍成交。同一场对话里 human 明示：「我们登记的这些 arq 我看着有时候很懵，不知道在说什么，这个也登记一下，看看怎么能优化」。条目本身的信息密度不是问题根因——根因是条目散文**假定读者已带机制上下文**：术语（all-mock / launch-card ⑦ / 收口链 / 机械牙 / 折抵）与证据引用直接开路，「问题是什么、对谁造成什么代价」没有独立的一段，读者须先脑补整套 dev-loop 收尾链路才能读懂证据段。

**影响面**：所有 `docs/debt/**` 条目的立项消费场景——human 立项 ack 是 Track B 七步的第一道人闸，条目读不懂意味着 human 无法独立拍板、全靠主线程转译（转译质量无凭据、且是单点）。对下游还有二阶效应：architect-reviewer 判档也要重推上下文；ARQ 处方复证（`[[arq-prescriptions-must-be-reverified]]` 纪律）时若条目自己都说不清问题，复证者更易读偏。

**建议改动**（候选，交 review、不自批）：
1. `docs/templates/debt-item.template.md` 正文四段前增「问题白话」段义务：条目首段必须用**不依赖机制上下文**的语言说清「什么场景下发生什么问题、对谁造成什么代价」，之后证据段照旧逐字精确（散文通顺与引用精确不冲突，AGENTS.md 输出标准已立此原则，本条是把该标准显式延伸到 debt 条目）；
2. 机制术语首次出现带一句实质说明——与 AGENTS.md「内部标识符首次出现必须带一句实质说明」同条纪律，不另立新规；
3. `.claude/skills/gov-arq/SKILL.md` 步骤 6 的四段义务同步扩为「白话段 + 四段」，登记即生效、不留新条目再违规的空间；
4. **存量条目不专项清扫**：不回填、不批量改写；条目被某批消费（立项复证 / 触碰）时由主线程顺手补白话段——与 AGENTS.md「齿」字清扫条（human 2026-09-29 立）的存量处置纪律同构。OQ 备选：若 human 认为立项期临时补写仍太慢，可在立项材料里固定加一节主线程转译（本次 P8 实证可行），代价是每批多一段非条目正文的一次性散文。

**风险**：riskTier: **low**——纯模板与写法义务文面（docs/templates/ + skill 文档），零脚本、零门判据、零契约面；机检面仅 debt-render/doctor 复跑自证。review profile = **Track B**（三要件：① 执行测出 yes——本次立项对话原话在案；② 修法明确 yes——白话段义务与存量「触碰时顺手」处置均有既成先例同构，无真设计分叉；③ 爆炸半径已被执行揭示 yes——human 两问答「看不懂」当场发生）。owner = architect（模板修订）+ 主线程（存量触碰时顺手）。
