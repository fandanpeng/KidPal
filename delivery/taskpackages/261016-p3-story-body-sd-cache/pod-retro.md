# pod-retro · 261016-p3-story-body-sd-cache

> 标签：**第一人称热记忆**（planner 收尾相位，integrator 复收口之后、机械 supervisor 闸之前）。
> 负向边界：本文件**不复述**不冠权威、不载 tokens / gates / rounds / verdict / test results——那些以
> 机器件（verifier-evidence / pod-events / supervisor-verdict）为权威。本文件只记我全程在场的主观过程体感。

## 这一轮是什么

一条跨面变更的落地腿：需求面已经把这套行为写到能验收（AC2 兑现 + AC7 降级 + 正文缓存边界），架构面把决策 24 的窄例外和契约帧冻结完，triage 又把包型从「三包拆开」被 human 改判成「单包一次做完」。我的活儿是把这个已经想清楚的东西，走完 dev-loop 的整条自治链——从 launch 面准备一直到合并门前。

整轮骨架：launch prep（含一次意外的上游已知红发现）→ intake + 1g 技术预评 → 验收冻结 → builder → verifier（一轮红、一轮绿）→ integrator **抓住一条 BLOCKING** → tighten 补齿 → fixer → 复验 → 复收口。真正有意思的全在中段那两个「差点糊过去」的地方。

## 我最满意的一处：1g 预评把「plan 里写错的落点」提前掰回来了

reviewed plan 里白纸黑字写着「`kidpal_story_open_reader` 签名加 id 校验」。我照单派给 architect 做前置预评，他实核后给了一个很硬的反对：这函数是冻结 seam，被既有冻结验收的 weak 桩和多个调用点钉死，改签名会直接破例。

我没有把这条当作「architect 想太多了」——去读了冻结件，确认属实，然后把它写进 plan 的预评节，改走「新增纯逻辑 seam + 消费侧校验」。这个决定的收益在两天后显形：fixer 修覆盖层时，同一份冻结件里的 KB-E2 齿还在守着 `keybar.c` 里「知道了」字面零容忍，谁动字面谁红。**冻结件的牙是真的**，plan 里的落点建议则可能是没实核过的惯性写法。

教训很直白：reviewed plan 是**意图**权威，不是**接线**权威；1g 那一档存在的意义就是让 architect 用代码面把接线的错提前撞出来。

## integrator 抓 BLOCKING 那一刀，是本轮质量的分水岭

所有机械闸全绿的时候，integrator 没有收工。它去核了覆盖层的**可见 chrome 面**——底栏键帽槽还是列表基层的「说话·双击看 / 下移」，页点还常显，而 `story_page.c` 自己在覆盖层里画的键帽被不透明底条整段盖住（死绘制）。

这条不在任何一条冻结齿里。它是「设计稿/brief 写了、验收没钉、实现就漏了」的经典夹缝。我当时的体感很复杂：一方面庆幸 integrator 的五轴 review 是真的在做语义 review 而不是复述闸结果；另一方面意识到这是**我的验收面设计漏洞**——SC-A5 只钉了「可见提示存在 + 10 秒消失 + 任一键关闭」，没钉「提示态的 chrome 长什么样」。

后续处置我选择了慢一点但干净的路：先让 acceptance-author 以 tighten-only 补齿（只增不改、红基线实跑），再让 fixer 修。这样这条 BLOCKING 修完之后，是**有牙的**——integrator 后来抽验把 `dots_overlay` 回退成 `overlay_top`，齿立刻转红。如果当时直接让 fixer 改完收工，下一次重构就会静默复发。

## 几次「事实与派单不符」的当场纠正

- **acceptance-author 的通道语义**：我给 fixer 的完成标准里写「独立冒烟通道 13/0」，fixer 如实拒绝——那个通道按冻结件自述就不链接实现源，永远只会停在 weak 哨兵。它没有去改通道让数字好看，而是把「host runner 才是判绿位」这件事写成 Red Flag 交回来。这个处理我很认：**不接受用改工具来让失败消失**。
- **KB-E2 的字面红线**：我派单时写「已核 `keybar_three_slot.acceptance.c` 对新 case 零命中」，fixer 实核后指出该件扫的是「知道了」字面（含注释），不是 case 标签——于是文案改落 `story_page.h` 宏单源。这是我派单里的事实错误，它当场纠了，没有闷头照做然后撞红。
- **TECH-261013 归档路径**：integrator 做完 debt 结项后，冻结的 docRefs 还指着归位前的路径，下一轮 verify 立刻以 docs-sync 红把这件事掀出来。档案归位与引用更新是一对必须同批的动作，机器闸替我记住了这一条。

## 我自己的两个别扭处

1. **误发了一条 human-decision 事件**。我在串联 verifier 轮结果时手滑 append 了一条 `decision=no-op` 的 human-decision——它不含任何人类授权，却占了一个「人类决策」的位。append-only 不能删，我只能补一条 friction 把它的 eventId 点名作废。**教训**：pod-events 的 eventType 是有语义的，占位/调试别借 human-decision 这一类；宁可什么都不写。
2. **UI brief 的在途警示行**。需求冻结并 human 终拍之后，`docs/ui/UIUX-DESIGN-BRIEF.md` 里「updating 在途、下游禁引」那一行本该由冻结动作撤掉，没人撤。我在起包期顺手清了并留了 friction。这不是本包的责任面，但它说明**冻结是一个多文件的原子动作**，只在 requirements / 架构 / 契约三处翻转，设计面的从属件容易漏。

## 帮了忙的机制

- **1g 预评 + 冻结件 seam contract**：acceptance-author 在冻结件头注里把 seam contract（函数名/宏名/数值）写成契约，builder 照着落、fixer 照着修，三方零口头对齐。`[C10]` 那类编号让「签名不许动」这种约束变成可引用的东西。
- **tighten-only 补齿 + 变异可杀**：BLOCKING 不靠人眼复验，靠「新增齿先红、修完转绿、再回退实现能再红」三步闭环。慢，但这条债不会再回来。
- **ESCALATION-BRIDGE 的兜底**：integrator 报 BLOCKING 后我先落 agent-escalation 再动，整个「谁发现了什么、处置到哪了」在账上可查。

## 留给下一包 / 治理面的一条体感

**「可见面」是一类独立的验收轴**。行为对了（提示出现、10 秒消失、按键关闭）不等于界面对了（底栏文案、页点、标题字号）——后者最容易被「没有自动断言」当作不存在。本包最后靠 integrator 的人工语义 review 补住这一刀，但这不是可复制的机制。下一批触 UI 的包，验收冻结时值得显式问一句：这个新态的**chrome 三件**（键帽槽 / 页点 / 主标题画法）各自有齿吗？没有的话，是 `dropped` 还是挂真机目检——必须二选一写明，不能默认「设计稿写了就等于落地了」。
