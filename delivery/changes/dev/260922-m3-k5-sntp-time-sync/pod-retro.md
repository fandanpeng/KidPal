# pod-retro · 260922-m3-k5-sntp-time-sync（第一人称热记忆，planner；追加轮 R3 后重写，最后一轮为准）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：dispatch ×6——acceptance-author 主轮 `a94d76f0da4ff3acb` + 追加轮 `a4119cd8b31bc25cf`、builder 主轮 `a84061c385cfc26c7` + 追加轮 `a204cd4fc46609197`、integrator 主轮 `a1ee382880eb28a45` + 追加轮 `a8743aa43cda65e10`；friction over-scan ×3 `b1f8768f`/`16f24d4b`/`215edf4d`；human-decision `fc43063d` 追加拍板；零 human-correction）。

## 追加轮体感（2026-09-22 · 真机第 1 轮 PASS + human 两拍板）

1. **真机第 1 轮一次全过是主轮质量的真实回报**：A1 正锚（rtc_writeback/reinject 节拍/tm_year:70 清零/状态栏首渲染）、K4 五项全过——「齿测不到的路径」这次没有再藏雷，host 齿 + 结构 needle 的覆盖面对时间链路这类纯逻辑+挂点型改动够用。对比 K4 live 轮两次 FAIL 的字形路径（格式语义复杂面），本包的 seam 钉法（纯函数行为表 + 结构在世齿）在真机上兑现率高，这条经验值得记：**能落纯函数的语义尽量落纯函数齿，真机风险面就小**。
2. **human 两拍板的授权链走法顺畅**：真机目测发现 → 口头拍板 → 主线程转发消息即凭据 → pod-events human-decision 落账 → 冻齿修订 → 实现 → 复验。拍板①（隐藏状态栏时钟）本质是 UI 偏好迭代，但 human 主动给了边界（温度/电量/wifi 零改）——边界即验收语义，acceptance-author 把边界直接钉成 teeth 3-5 在世守卫，派单零损耗转写。
3. **acceptance-author 的守门裁定（保留 tm_year 守门不退役）是我没有预见的正确裁法**：我原以为隐藏时钟后守门会成死代码要处置，裁定时指出 P1 消费面不经该守门、守门仍辖节流 W 日志分支——冻齿语义裁定的质量直接省了一轮反复。A1 锚重指（状态栏→P1 辅助条）也处理得干净，头注三处 + 承载齿一次钉齐。
4. **追加轮 mutation 执行面粒度差依旧**：R3a/R3b 未实跑变异，替代证据 = 设计态红≙变异态在案（红恰 = 两新齿、既有 90 零偏移）。integrator 如实申报登记面与执行面差——这个欠账跨了主轮、追加轮两棒，若判官在合并门要执行证明，补跑手法已在 expectedRed 逐字登记。下棒值得把「mutation 实跑落仓内 evidence 文件」变成 builder 收尾标准动作。
5. 机械 supervisor verdict（ESCALATE）跨追加轮押后不变——机械闸 write-once 语义在多轮场景下的处理（不重跑、不覆盖、随包在案）是首次实操，K4 的 stale-verdict refresh 先例是「真变更后」，本包是「合并门押后」，两种形态都成立。

## 主轮体感（保留要点）

- 元数据闸判据 checklist 化直接兑现 K4 留根：finalize 预飞抓到三个 K4 同款坑（upstreamType 闭集、mutationEvidence 记录形状、brief 进 current/），verifier 主轮一次绿。
- acceptance-author 与 builder 双向诚实是质量地基：真机半 4 项如实 deferred、builder 8 条主动申报（写回形态实现化、恢复侧 year_ok 代理、sdkconfig 生成面陷阱、注释字面子串陷阱）。
- mutation 执行证明 provenance 差距跨棒存在（临时脚本实跑、原始日志只在 transcript）——见追加轮第 4 条。
- launch card 特记（taskpackage 曾截断）促成 intake 逐字段亲核——「不信任上游落盘、闸判据亲核」连续两棒划算。

## 别扭的 acceptance / 帮倒忙的钩子

- mutation-check 记录形状（`acceptance` 键 + 整段文本值命中字符串型条目）对起包方依然隐式——值得写进报错提示或 DEV-GOVERNANCE 起包前置（非阻塞建议）。
- required-check 按 taskpackage 文件路径所在目录找 brief——current/ 要副本，双份维护别扭（非阻塞建议）。
- sdkconfig 生成面陷阱（defaults 改了生成面显式 =1 胜出）——「生成面 vs 源面」双写先例值得留记录（builder 已双改并复验，本包闭环）。

## 人工纠偏体感

- human 分层出场：主轮零打断；追加轮一次出场（真机第 1 轮后两拍板）且自带边界与流程要求——分层纪律连续第四棒成立。
- 设备轮窗口协调（隐藏目测/读点秒≈0/A3 断电续航/A3b 长开机）是本包剩余唯一 human 义务，约窗后真机全过回合并门（ESCALATE verdict 押后不变）。
- loop-retro 阈值 19 advisory 连续第四棒挂账，发起责任在主线程/human。

## 下次改进

- mutation 实跑落仓内 evidence 文件，消掉 provenance 转述差（跨两棒的同一欠账）。
- finalize 预飞三闸判据 checklist 固化为习惯（连续两棒净赚）。
- 能落纯函数的语义尽量落纯函数齿——真机一次过的直接经验。
