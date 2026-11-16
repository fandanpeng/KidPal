# pod-retro · 261116-channel-resident-core

> **第一人称热记忆**（planner 视角，收口相位写于 integrator closeout 之后、合并门前）。只记过程体感与判断，不载 tokens / gates / rounds / verdict / 测试数字——那些以机器件与 pod-events 为权威。

## 一、这一包难在哪

这不是一个「加功能」包，而是**把会话与通道这对揉在一起的概念掰开**。所以真正费力的不是写代码，而是**先说清楚「哪几处代表会话结束」**——四收尾点这个提法来自拆包期，落地时才发现它其实分成两类调用上下文（协议接收任务 vs 主循环），一旦混用就是跨任务竞态。architect 预评把这一刀提前挑明了（helper 走事件位、其余直调），这是本包最省命的一条建议；如果让它留给 builder 现场判断，很可能第一轮就对。

第二个难点是**验收断言的"结构敏感"**：本仓的冻结验收大量按源文本区切与字面计数。它们很能抓回归，但也非常容易被注释字面喂饱。builder 主动把改动区注释里的判据字面删掉（并说明理由），这一步不是洁癖——我在复核 mutation 时看到，若不删，某些「变异」会因注释仍含字面而假绿。这条以后应该写进验收撰写习惯，而不是每包靠 builder 自觉。

## 二、卡点与绕轮

没有进 verifier↔fixer 内循环，但我认为有一个**差点翻车的相位**：builder 首跑后剩一条红，根因不在实现，而在另一件既有冻结验收（`record_dialogue_guide` 的 KEY 路径幂等断言）的**粗粒度区切**把统一处理器整段算了进去。这条我预评时没扫到——1g 的影响面清单补了五件 must-stay-green，偏偏漏了这一件。builder 按边界纪律停手、只回报不擅自改验收件，这个姿势是对的；我按扩单通道把文件纳入 manifest、再派 acceptance-author 做最小收窄（区切终点前移 + REVISION LOG），断言语义零变化。**这条我记了 friction（scan-miss）**，因为它的形态会复发：凡是「用函数签名对切区」的既有断言，遇到新增同族函数就会假红，影响面扫描应该把"区切终点函数"也当作被改面。

另一处让我停顿的是心跳 task 的落位：它是无限任务，绝不能进 `FIRST_COMPLETED` 集合；否则连接收尾会被它永久拖住。architect 把 `finally` 首行 cancel、循环 sleep-first 都钉死了，我照此写进派单。事后看，这类「生命周期与已有并发结构相交」的知识只存在于读过现场的人脑子里，文档不会告诉你——architect 预评的价值主要就在这。

## 三、验收体感

acceptance-author 这一轮交付很实：15 条断言逐条给了设计态红映射、mutation 建议、未覆盖面。让我别扭的只有一处——SCR-A4 的 `RaiseReminderQueue` needle 被既有注释遮蔽（单独改名不转红），它改用同区另一符号承载敏感证据并如实申报。这是**验收断言强度**的灰区：断言没撒谎，但它对某一个符号就是钝的。我选择记录成观察项、不返工，因为强制它变敏感会要求动一段本包约定不动的注释区（Idle 八件链）。这个取舍若是下一包遇到，值得先问一句「这条断言的敏感面是不是落在注释上」。

## 四、规则与钩子

- scope-map 的分工这次很顺：acceptance-author 想写 CMakeLists 会被拦，派单里就预指派了 builder；没有出现撞钩子再返工。
- `mutation-check` 要求 mutationEvidence 里放**完整 acceptance 字符串**（不是短 id），我先按短 id 填了一遍才发现。这个口径在脚本文档里有注记但不显眼，值得下次起包时直接按完整字符串填。
- `changes-index.mjs` 只服务 review lane，对 dev 包会报命名错；dev 面的 INDEX 写口是 `closeout-check --update-index`。integrator 按我派单没跑（我写了"不得传 --update-index"），结果 dev INDEX 缺行——这是**我的派单口径与 integrator.md 的硬要求冲突**。我在收尾相位补跑了写索引。下次派 integrator 时应该把「产出后跑 `closeout-check --update-index`」「planner 复核用只读」两件事分开写清，不要用一句"不得传"把 integrator 的义务也禁掉。

## 五、人与人之间

human 只给了两条输入：跑哪一包、base ref 取哪。全程没有打断、没有中途改方向，这在本包这种"改会话语义"的包上很难得。唯一需要 human 接手的仍然是**真机轮 A**——本包的时延收口（温通道 ≤0.82s）、连续讲 >90s 不误收、心跳 30s 节奏与三方并发写同一 ws 的观感，离线怎么近似都替不掉持板目击。我在合并门前停在这里，不宣布效果达成、不结 debt，等轮 A 回来再补证据与结项。

## 六、下次改进

1. 1g 影响面扫描加一条硬检查：**既有断言的区切终点函数**若被本包改到，必须逐件核（本次 `record_dialogue_guide` 就是这么漏的）。
2. 验收撰写约定补一句：断言的 needle 不得落在注释可满足的位置；撰写时先 grep 一遍实现文件里该字面是否已在注释中出现。
3. 派 integrator 的派单把 `closeout-check --update-index` 写成 integrator 的必做项，planner 复核另行只读。
4. 心跳这类"连接级无限任务"的落位，派单模板里应固化三件：不进 FIRST_COMPLETED、sleep-first、finally cancel+gather。

## 七、escalation themes

本包全程**没有 agent-escalation**（无 Red Flag 升级、无 human 裁决请求）。记入 pod-events 的两条 friction 均为 advisory 记账：

- `b3fb9aca`（**scan-miss**）：1g 影响面扫描漏 `record_dialogue_guide.acceptance.c` 的 RCL-F1 粗区切；处置 = 扩单最小重锚，未改断言语义。
- `f157a5d6`（**over-scan**）：acceptance-author 清单外阅读 6 行（`story.py`、front 的 sweep/release/finish 区、三件既有冻结验收、CMakeLists、编译数据库）——均为冻结核验必需，advisory 记账。

human 侧的唯一未闭合项不是升级，而是**流程内出口**：真机轮 A 由 human 持板，归本包 exitCriteria；在它发生前我宁可按未完成处理。