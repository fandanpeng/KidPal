# pod-retro · 260915-m1-n1b-p1-home-battery（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage）为权威，此处不复述、不冠权威。

## 卡点与体感

1. **opus 池熔断连吃两枪**：acceptance-author 派发先吃 400（model 参数无效）再吃 429 weekly 熔断，正卡在收尾安全检查段。当时我最担心的是冻结文件写了一半——实际落盘是完整的，我补跑了 gcc 实编 + 基线独立复核才敢收口。「agent 被截断后先验制品完成度再决定重派还是续跑」这个判断顺序是对的，重派一遍是浪费。后来 human 换账号恢复 opus，但那天我意识到 MODEL-TIER 的升档意图在没有备用通道时应该有更明确的降格记录动作——我做了 friction 留痕，但如果当时 agent 没把活干完，降格重派的开销会大得多。

2. **红基线落账转写是给 planner 自己挖的坑**：K1a retro 的教训前置（acceptance-author 回传、planner 转写）执行得很顺，但我第一版 Edit 漏了个逗号把 taskpackage JSON 弄残——靠逐字节校验 JSON 才抓到。小事，但「转写字段后立即机器校验」应该是我肌肉记忆的一部分，这次是踩了才想起。

3. **host 齿绿 ≠ 设备对**，这个包给了最生动的一课。两轮真机发现（48px 字形溢出进小钉区、图标资产裁半）全是「host fixture 不挂 art 资产 → 恒走兜底描边路径」的结构盲区——验收齿在兜底路径上全绿，真字形路径 host 从未跑过。我在验收冻结时审过 seam 契约设计，但没追问「host 上真路径到底跑不跑」——这是我这轮 intake 的最大疏漏。fixer 两轮补丁本身顺滑（根因我钉得准，fixer 只需执行），但根因本可以在冻结前被问出来。follow-up（host harness 挂真资产 + 空隙零墨收紧齿）已进 review-packet。

4. **verifier 抓不到的第三红**：integrator 逐轮复核 rounds JSON 时发现 final 实为 3 failed 而非我口头移交的 2——VRT-A2 是 544a804 归档（human 一次性操作）在 K1a 锚取点之后制造的环境漂移，不是我移交时看的 stdout 头部里那条。我读 r3 失败详情时只看了 head，这个偷懒差点把一条未在册红带进合并门判读。「判读要逐条点名」这条纪律我自己执行得不彻底，被 integrator 补住了——跨角色互查这次真的起作用了。

5. **真机轮协调比预想顺**：human 持板 + 实时口令钉动作点 + 我开捕获窗的配合节奏很好，两轮烧录-看屏-修复循环都在一个晚上跑完。意外的是 human 与本会话共用网络（断网项不可关路由）和充电数据同线（拔线即断串口）这两个环境事实——下次 intake 环境清单应该把「human 网络依赖」和「串口供电复用」列进去。

## verifier↔fixer 绕轮原因的主观判断

r1 红是 mutation-evidence 落账缺口（流程账不是代码账），r2 即绿——这轮绕行是合规成本不是缺陷。真正该绕没绕的是真机轮发现的两轮 fixer（r4/r5 前）：它们不在 verifier 相位里，因为触发源是真机看屏而非齿红——「真机轮发现直通 fixer、修复后回补 scoped 轮」这条路径我走出来了，效果不错，值得在多包推进时保持。

## 别扭的 acceptance / 帮倒忙的钩子

- enforce-scope 拦我写 device-round-record.md 是对的（planner 在 changes/dev 只能写 pod-retro），但我当时确实想偷懒直接写——最终 integrator 收口时写掉了，职责分派反而更干净。
- mutation-check 闸的「缺失即红」把我逼去做真变异执行而非纸面推演——这个闸帮了忙，10 变异全敏感的证据比 builder 首轮报告里的「可变异」描述值钱得多。

## escalation themes（cross-ref pod-events）

- dfda0852（agent-escalation / environment / VRT-A2 归档漂移）：pending，合并门停等项。
- 4c10c620 / 372601a6（human-decision：PH-E2 降级、真机轮挂起到明早）：PH-A6 闭环待 human PASS。
- a40d4f99（friction / scope-drift / 联网小标偏大）：共享底座面归属待 human 裁。

## 下次改进

- intake 追问「host fixture 与设备真实路径的差异面」（资产挂载、字体、分辨率）——冻结前问一次。
- 失败判读逐条点名，禁止只看 stdout 头部摘要。
- agent 被截断后：先验制品完整度（编译/实测），再决定续跑/重派/降档。
- JSON 转写字段后立即机器校验。
