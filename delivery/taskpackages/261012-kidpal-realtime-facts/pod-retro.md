# pod-retro · 261012-kidpal-realtime-facts（第一人称热记忆）

> 标签：第一人称热记忆——planner 全程在场的主观过程体感，2026-10-12 收尾相位写（integrator closeout 后、supervisor 闸前）。不复述、不冠权威 tokens/gates/rounds/verdict/test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。

## 本次跑法与体感

这是我跑过的形状最「顺」的一包：Lite、单面、上一包刚同域交付（v9 prompt 批）留下完整先例路标——红基线形态、structural 豁免对象条目、arch 指针刷 handoff、ECS live ops 纪律全部有样可抄。体感上「先例密度」决定了 Lite 包的流畅度：本包三轮 scoped 里两轮红都是**包元数据形态**问题而非实现问题，若不是 v9 先例在案，我大概率会在豁免语义上多烧一轮。

## 卡点与绕轮原因的主观判断

1. **crossEnd 两次试错是我 intake 判断不狠**：我先按「同步面=对端面」填了 counterparts，brief-inputs fail-loud 后又换 symbol 硬凑解析，二轮才认出「同端 sync 面本非对端语义」、对照 v9 exempt 先例改判。教训：crossEnd 判断第一步就该问「wire 的另一端在哪个 repo 面」，答不出就是 exempt 形状，不要拿解析器当语义裁判。
2. **两轮 scoped 红都是我的 finalize 面欠账**：RF-A5 忘了标 `kind:'structural'`（v9 PN-A5 先例就在眼前还是漏了）、`mutationSkipReason` 写错了层级（塞进 mutationEvidence.reason 而机检读顶层）。两处都是机械闸教我的——finalize 时对着 required-check 全绿就以为齐了，但 coverage/mutation 两闸的字段形状是 required-check 不看的。修法都是分钟级，但**这些形状先例应该进我起包时的自查清单**，不该靠 verifier 轮烧出来。
3. **acceptance-author 和 builder 都被 delivery/** 写权拦、回执代办链路顺畅**：本轮两次「子 agent 拟全文 → planner 代存」零摩擦，派单里预先写明代落条款（派发书三条硬约定）起了作用——没有一次撞钩子重试浪费。

## 别扭的验收 / 钩子体感

- mutation-check 要求顶层 `mutationSkipReason` 而 mutationEvidence 内的 reason 不算数——同义信息两个字段位、闸只认其一，这是给起草人挖的坑（本包就踩了）。不是要改闸，是字段名该更自明。
- RF-A5 这类 live 观测项以 structural 豁免承载语义是诚实的，但「structural（非代码可测）」这个名字对 live-manual 观测项其实不贴——先例既然统一走这条通道，文档里值得补一句「live-manual 观测项也走 kind:'structural' 通道」。

## 人工纠偏体感

本轮零人工纠偏——human 三答在 spine 里预先钉死了全部分歧点（单包 Lite / 先行捎带 / 延迟口径）， Pod 内没有一次需要停下来问。这是 intake 期触点①批得值的地方。

## escalation themes（post-cutoff，交叉引用 pod-events）

- 本轮零 agent-escalation 事件。唯一 pending-human 面 = **RF-A5 真机轮**（问天气全链路 + 降级路径，human 在场持板）+ **supervisor 潜在 all-mock 判定的人批通道**（launch-card ⑦ 前置申报、v9 同款先例）——两者都是既有门的人到面，非 Pod 内卡点。

## 下次改进

1. 起包自查清单补两条：① crossEnd 判定先答「对端在哪个 repo 面」，答不出 = exempt；② live-manual 观测项验收条目一律对象形状 + kind:'structural' + note，mutationSkipReason 记顶层字段位。
2. Lite 同域续作时，把上一包 taskpackage 的豁免/形状字段做一次机械对照（本包两轮红的根源都是「先例在场但没逐字段抄」）。
