# pod-retro · 260922-m3-k5-sntp-time-sync（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：dispatch ×3——acceptance-author `a94d76f0da4ff3acb`、builder `a84061c385cfc26c7`、integrator `a1ee382880eb28a45`；friction over-scan ×2 `b1f8768f`/`16f24d4b`；零 human-decision、零 human-correction——human 只出场于 launch 指示，loop 中途零打断）。

## 卡点与体感

1. **这棒最顺的是「元数据闸判据 checklist 化」直接兑现了 K4 留根**。K4 两轮 verifier 红全是 planner 元数据（contractRefs 空、mutationEvidence 短 id），这棒我在 finalize 前先把 taskpackage-required-check / mutation-check 的源码判据当 checklist 走了一遍——预飞时就抓到三个 K4 同款坑（origin.upstreamType 不在合法闭集、mutationEvidence 记录键名要用 `acceptance` + 全文本值而非短 id、brief 副本要进 current/ 目录），两轮预算净赚，verifier 一次绿。
2. **acceptance-author 与 builder 的双向诚实是本棒质量的地基**：acceptance-author 冻齿时把真机半 4 项如实标 deferred 不伪造；builder 一次跑绿但报告里主动交了 8 条诚实申报（写回形态实现化、boot 恢复侧 year_ok 代理语义、sdkconfig 生成面陷阱、注释字面子串陷阱）——这些没有一条是验收齿逼出来的，全是自发的。我派单时把「请 verifier/integrator 复核认可」的通道留开，integrator 复核后入 Correctness 轴，链路闭合。
3. **mutation 执行证明的 provenance 差距还在**：builder 5 组变异自证是临时脚本实跑、原始日志只在 dispatch transcript 里非仓内制品——integrator 如实标注来源后我只能记「转述可信」。K4 Review Packet NIT 说的就是这个粒度差，这棒没有变好也没有变坏。下次值得让 builder 把变异实跑落成一次性仓内 evidence 文件（哪怕用完即标 temp）， provenance 就闭环了。
4. **launch card 的特记警告（taskpackage 曾截断损坏）让我 intake 时逐字段核对了 taskpackage↔plan 一致性**——虽然最终问题不在截断面（upstreamType 非法值是原始落盘就有的），但「不信任上游落盘、闸判据亲核」的姿势这棒再次划算。
5. 真机半 4 项挂设备轮窗口，K4 的五项目测复验可同窗顺带（K5 bin 已含 K4 修复）——合并协调点到真机相位再约窗，本相位不动。

## 别扭的 acceptance / 帮倒忙的钩子

- mutation-check 的记录形状（键名 `acceptance` + 整段文本值才能命中字符串型 acceptance 条目）对起包方依然是隐式的——报错只列 missing 全文本、不提示「记录里要包含整段 acceptance 原文」。K4 踩过、这棒靠读源码避掉，但这个判据值得写进报错提示或 DEV-GOVERNANCE 起包前置清单（机械闸改进建议，非阻塞）。
- required-check 对 brief 在场的判定按 taskpackage 文件路径所在目录找——`current/taskpackage.json` 就要在 current/ 再放一份 brief 副本，双份维护是个小别扭（改判据按 changeId 目录解析更干净，非阻塞建议）。
- builder 报的 sdkconfig 生成面陷阱（defaults 改了不生效、tracked 生成面 sdkconfig 显式 =1 胜出）是 IDF 侧老坑，齿钉在生成面才算真前提——这类「生成面 vs 源面」双写现状值得在板上留个先例记录（builder 已双改并复验，本包已闭环）。

## 人工纠偏体感

- 本棒 human 只出场一次（launch 指示），loop 中途零打断、零升级——分层出场纪律连续第四棒成立。loop-retro 阈值 19 继续挂账（closeout advisory 已提），发起责任在主线程/human，第四棒提请。
- 真机相位的人工协调窗口还在前面（约窗 + 在场），这是本包剩余的唯一 human 义务。

## 下次改进

- finalize 预飞 checklist 固化为本棒习惯：taskpackage-required + mutation-check + acceptance-coverage 三闸源码判据逐条对——连续两棒证明净赚。
- mutation 自证落仓内 evidence 文件，消掉 provenance 转述差。
- 同 defect 第二次修复不踩中时，把缺失实证面搬到可观测侧（K4 第 3 追加轮教训）——本棒没用上，但仍是 seam 类实现的第一常识。
