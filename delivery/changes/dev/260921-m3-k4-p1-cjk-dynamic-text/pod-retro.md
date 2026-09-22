# pod-retro · 260921-m3-k4-p1-cjk-dynamic-text（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：human-decision `b396f1ff` 主线程代跑 launch、friction over-scan ×2、dispatch ×4）。

## 卡点与体感

1. **本棒最顺的一步是「判红依据先亲核再路由」**。builder 报唯一红并声明是冻结验收自身算术缺陷（D==3D 互斥）——我没有直接转发，先自己读了 `pcn_diff_stats` 的 out_total 语义和 TEST 2 求和段，确认互斥成立才派 acceptance-author 修冻齿。builder「不私改冻齿、升级 planner」的纪律和我的亲核形成了双保险，这条链路一次往返就闭合了，体感上比 K 期「verifier↔fixer 空转」的棒次干净得多。
2. **scoped r1/r2 两红全是 planner 侧元数据格式错，不是代码红**——contractRefs 空数组被判缺、mutationEvidence 用短 id 不匹配全 id 判据。两轮本可省：起包时 K3 的 taskpackage 里 contractRefs 就有非空先例可抄，mutation-check 的「全 id + 敏感信号」锐边我在 K2/K3 期也没踩过所以没有预案。**教训：finalize 时对着闸的判据（而非自己的直觉格式）回填，两轮预算就是净赚的。**
3. **acceptance-author 两棒都发挥稳**：第一棒红基线两通道实跑 + A3 单项零墨齿诚实补缺（对我派单里「不需要新齿」的判语做了有据修正——这种向下修正比照单全收更有价值）；第二棒最小补丁修冻齿 + REVISION LOG + 判别力两层保全自查，零越界。
4. **over-scan 申报纪律运转顺滑**：acceptance-author 7 行、builder 5 行，全部任务必需或纪律强制，无一行是漫读。派发时那句「超读申报表必须诚实列」继续是低成本的防线。
5. integrator 这棒的 final bearing 轮自主跑掉了（K3 先例同款），我 FS-EVIDENCE-3 逐项复核制品在场 + closeout-check + git log 后直接进机械闸——收尾相位的动作清单已经磨合到无卡点。
6. Review Packet 一条 NIT（mutation 执行面留痕粒度：登记 ≠ 执行证明）——我裁不阻塞：红基线 stub 跑 ≙ A1 变异态在案、A2 去 clip 变异复验在 REVISION LOG，其余条目的执行留痕若判官要，iterationBudget 容得下补跑轮。这是诚实登记面与执行面的已知粒度差，不是本包缺陷。

## 别扭的 acceptance / 帮倒忙的钩子

- mutation-check 的全 id 匹配判据对起包方是隐式的（报错只说 missingSensitive，不提示「要全文 + sensitive」）——对新人或新包形态是绊点，建议报错信息里带一句格式提示（机械闸改进建议，非本包阻塞）。
- schedule_grid 的 E2 canary 扫注释子串（`!contains(home_p1.c,"upcoming")`）让 builder 被迫改注释措辞——canary 识字串而非识语义的老问题，这次代价只是一句措辞，但值得记着。

## 人工纠偏体感

- 本棒 human 只出场一次（launch 指示主线程代跑，K3 先例复用），loop 中途零打断——分层出场纪律连续第三棒成立。
- loop-retro 阈值 18（15 pending + 3 waiting）继续挂着——发起责任在主线程/human，K2、K3、K4 三棒 retro 都提了，这笔账越滚越大，收尾报告继续提请。

## 下次改进

- finalize taskpackage 前把三个元数据闸（taskpackage-required / mutation-check / acceptance-coverage）的判据当 checklist 走一遍，别凭 K 期记忆填字段。
- 「子 agent 报告断言的缺陷根因，先花一次读代码的功夫亲核再路由」——本棒再次验证划算，升为习惯。
