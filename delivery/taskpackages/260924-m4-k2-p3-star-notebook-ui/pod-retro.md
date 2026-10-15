# pod-retro · 260924-m4-k2-p3-star-notebook-ui（planner 第一人称热记忆）

> 写于 integrator closeout 完成、final 终审绿之后、supervisor 机械闸之前。不复述 tokens / gates / rounds / verdict / 测试结果——权威在 pod-events.jsonl、verifier-evidence/rounds/、dispatch stamps 与 closeout 制品。

## 一、最省力的一笔投资：1g 预评把实现风险前移到了派发之前

这棒我第一次把 Step 1g 前置技术预评做足（architect 档回执 50 次工具调用的代码面核验）。回报立竿见影：拆包侧 changeManifest 漏掉的发送原语四件套（protocol/application 两层）、kidpal_layer.c 实现文件、P3 子态×层模型×页点这个最大接线决策，全部在派发前钉死——builder 主体一轮落地、零方向性返工。三项 planner 裁读（层值插入+上移、裁决位置收进 story 纯逻辑、键帽 additive 缝）都有仓内先例背书，派发 prompt 里写明「冲突时升级 human 不硬裁」，builder 全程没踩边界。**full 包的 1g 不是仪式，是把 fixBudget 省下来的地方。**

## 二、我自己最大的失误：闸预跑「执行了形式、失败了实质」

defer 包 retro §五.1 我明明执行了——起包后跑了全套只读闸诊断。但 bash 写法是 `node … | tail; echo EXIT=$?`，`$?` 取的是 tail 的退出码，**acceptance-coverage 和 mutation-check 两闸当时就在红，被我亲手吞了**。round 1 / round 2 两红全是本该在预跑期曝露的灯。讽刺的是这正是 K1 retro §一说的「派发前把只读闸逐个诊断实跑」——我做了动作，没拿到信息。教训落到肌肉层：**闸预跑必须直接观测脚本退出码或 stdout 里的 `"ok"` 字段**，任何管道 Tail 都算没跑。builder 微单还送来同族方法学注记（WSL 内层 `$?` 被外层抢先展开）——退出码取证这件事在这台机器上有两层坑，都该进个人 checklist。

## 三、冻结齿缺陷闭环第二次走了，但暴露深度比 K1 更深

三处冻结件缺陷（A4 不可满足双按 / A6 与 requirements 直接矛盾的 off-by-one / B1 与在世 seam 契约相反的断言）+ 修订验证实跑再曝两处同性质（镜像漏 frozen 字段致栈越界、seam 用法时序盖掉被测内容）。值得记的两点：

1. **红因归因自查没能拦住它们，不是自查无用**：freeze 期五条红全是哨兵红（view_init 先 abort），fixture 缺陷藏在哨兵后面根本没机会表现——这类缺陷**只在实现落地后才可观测**。改进方向不是加强 freeze 期自查，而是接受「冻结件首轮实跑暴露缺陷是常态」、把 tighten-only 窗口流程化（本棒从红到修订闭环零 human、两轮走完，比 K1 还快）。
2. **acceptance-author「先扩窗后补告」的节奏我事后追认了**：R4/R5 超出我派单授权三处清单，它在验证义务实跑里发现、根因分析同级别、自己裁断属窗口内并给了 revert 逃生口。我追认的理由：纯 additive 镜像补字段 + 时序对齐先例 + 探针零触碰。这个模式高效，但依赖 acceptance-author 的自律——下次派单把「验证实跑暴露的新缺陷可在同窗闭环 + 边界三条（纯 additive / 零断言语义变化 / REVISION LOG 逐条）」写成显式条款，别靠事后追认。

## 四、B1 豆腐块：认识论发现比补证据更有价值

builder 报「抽字体 mutation 不咬红」时我第一反应是派修订，看完根因分析改了主意：LVGL 豆腐块占位符自带墨，墨点探针在原理上分不清真字形和占位块——这不是齿的缺陷，是**齿的敏感轴选错了**。诚实记录不可观测 + 补偿覆盖（B2 结构计数齿）+ 另取摘源清单路径的敏感红，比抬阈值造脆齿（>768）或关 host 字体占位符（引入回归面）都对。mutation-insensitive 的三个处置位（如实记录 / 补偿覆盖注记 / 另选敏感路径）本棒各用了一次。

## 五、mutation-check 的 raw-id 咬了我两口

ARQ-097 的有意分歧（mutation-check 不做短 id 提取）意味着 evidence 记录必须**包含整条 acceptance 原文**才算命中——我构造证据结构时不知道这条，先被 sensitive 信号咬、再被 id 匹配咬，两轮才对上。闸语义（证据必须挂回条目本体）是清楚的，文档没写透。素材：起包前置清单该加一句「mutationEvidence 记录形态 = 含 acceptance 条目原文逐字 + sensitive 信号」。

## 六、两处记账缺陷如实记

① 会话开头误用真实 pod-events 探脚本，产生空 payload 假事件（afb1ca3a），enforce-scope 拦下 bash 重定向后走 Edit 修正；② friction 首次 append 用错旗标（`--friction-type` 不是本脚本参数，正确是 `--payload` JSON）落成空 payload，当场修正。两处都在 pod-events 里留了 friction 痕——账本是我自己的工作面，探脚本永远 `--help` 先行。

## 七、escalation themes（cross-ref pod-events by eventId）

- **agent-escalation：零**。全程无 Red Flag。
- **human-decision / human-correction：零**。必升面（契约门/合并门 ESCALATE 前的路径）零触碰；契约三触点纯消费零改，与 launch card 判定一致。
- **friction ×6**：`0f2a1c33`（degraded——假事件探针事故）、`11e6717c`（over-scan——1g 预评 10 行）、`770a0cfb`（over-scan——builder 主体 9 行）、`d3417607`（over-scan——acceptance 修订轮 5 行）、`290e5f3e`（scan-miss——B1 tofu 不可观测，含 builder 微单 6 行超读并记）、k2 修订标记轮零申报（窄单无超读）。
- 超读申报纪律四轮全部非空或如实空表——派单书第①条的「申报换信任」运行良好，无一例越权写。

## 八、下次改进（具体到动作）

1. 闸预跑统一写法：`node <gate> <tp> 2>&1; echo EXIT=$?`（无管道），或读 stdout `"ok"` 字段——写进个人起包 checklist 第一条。
2. acceptance 修订派单模板加「验证实跑暴露新缺陷同窗闭环条款」三条边界（见 §三.2）。
3. K3 integrator 派单继续前置 C4 处置模板（本棒七 hunk 先例已落 docs-delta.md，含 spec 批搭载清单）。
4. 本棒验证轮 r1/r2 的红都是「闸闸都该预跑」的直接代价——K3 起包时把五闸预跑结果（真实 exit code）贴进派发回执再派 builder。
