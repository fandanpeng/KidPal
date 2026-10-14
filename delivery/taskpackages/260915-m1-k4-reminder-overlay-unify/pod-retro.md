# pod-retro · 260915-m1-k4-reminder-overlay-unify（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage、verify-runs）为权威。escalation themes：本包零 agent-escalation 事件（pod-events 实读），friction 条目 cross-ref 同文件。

## 卡点与体感

1. **本包唯一烧掉的 verifier 轮是我自己漏了一个前置**：r1 红 = mutation-check 台账空——不是代码病、不是环境病，是我派单时漏了「mutation 证据回传」这条起包格式期望前置（清单第 1 条），且 Step 8 finalize 时没核 mutationEvidence 非空就放了行。修法干净：补证轮派 builder 实跑 8 变异（应用→转红→还原），一轮补齐、final 一次过。教训：**finalize checklist 要把 mutationEvidence 逐条数进「缺落盘记录不得派 builder」的硬前置里**，不能等闸来抓。

2. **dispatch-stamp 落错包暴露了 current 指针刷新的连锁性**：我漏刷 `delivery/taskpackages/current/taskpackage.json`，脚本按 current 解析、两条 dispatch-completed 误落 k3 的 pod-events。好在 FS-EVIDENCE-1 的 git diff 例行核对撞见了（脏列表里出现 k3 文件才追查）。current 指针不是「顺手一步」，是**所有按 current 解析的机检脚本的根**——起包 checklist 值得把它升为显式牙（closeout-check 或 dispatch-stamp 前置校验 current.changeId === 本包 changeId）。

3. **零 fixer 轮的舒服是前面垫出来的**：acceptance-author 把义务红清单、渲染模型核验（对象模型 vs 软帧 blit）、RM-A3 fail-loud ② 的返钉声明全钉在 freeze 相位，builder 照单实现一次全绿；变异实验里 RM-A3 插座「recording 恒 0 不会转红」的预判也被 builder 证实并就地改用决策表列翻转——**验收作者对齿敏感面的预判在补证轮兑现了**，这是「齿先行」纪律的红利。

4. **真机轮 pending 的三件套惯例继续顺**：RM-A8 从拆包起就是 structural + human 持板，本包无需 loop 中人类拍板事件（与 k3 不同：k3 是 loop 中追加 defer 需要 human-decision 留痕；本包 deferred 是拆包既定，事件应在 human 持板确认时自然产生）。review-packet demo 对账行如实标了 host 一致 / 真机 pending / 模拟器截图缺口，没有半句虚构。

5. **scope-map 的 agent 写权与我的派单口径差一次撞墙**：我让 acceptance-author 写 taskpackage 目录下的 red-baseline.md，它被 enforce-scope 拦（写权实为 acceptance 面），全文回传我转写——转写惯例已顺（k2/k3 同结论），但**下次派单前先对一下 per-agent allow 表**能省一次来回。

## 别扭的 acceptance / 帮倒忙的钩子

- mutation-check 报错把整串 acceptance 原文甩在 missingSensitive 里——形态纪律的代价，先例对照即解，不算 bug。
- universal.bashDenyWrite 连 pod-events 的 fs 纠错也拦（重定向写），但 pod-events-append / dispatch-stamp / 临时脚本 fs.appendFile 三条通道口径不一——拦截是对的，通道分化是历史层积，值得治理批收敛成单一受门 append 通道。

## 下次改进

- finalize 硬前置加两条自查：mutationEvidence 逐条在账（structural 除外）、current/taskpackage.json 已刷指本包。
- 派 acceptance-author / builder 前先 cat scope-map per-agent allow，派单写权口径对齐，不让 agent 撞墙一次才对齐。
- 复用树第 6 棒起：intake 即验 CMakeCache 生成器 + current 指针双健康检查。

## NIT 移交（非本 Pod 义务，登记备查）

- NIT-3 旧卡面渲染函数 `kidpal_reminder_page_render` 清理批候选（review-packet 已登记）。
- NIT-1 图索引滞后（新符号未入图），trace 抽查已降级 grep 兜底。
- loop-retro 阈值 advisory（closeout-check 报 8≥3）——发起责任在主线程，本 Pod 不代发起。
