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

## 真机窗口收口体感（2026-09-22 · R3 bin 目测，追加轮后）

1. **追加轮两项真机一次过**：R3a 隐藏目测「消失了，正常」、R3b 滞后「1s 以内」——分钟对齐纯函数缝的端到端兑现，与主轮同款经验（纯函数齿 → 真机风险面小）第二次验证。A3b 长开机 34 分钟日志无断 + 冒号闪烁佐证 1s tick 活性，F2 冻结缺陷的修复在长时域上站住了。
2. **A3 断电续航 human 主动延期**（「断网现在做不了，跳过，后面观察」）——账面如实记 deferred 不记 PASS，E3 口径 human 承担。这是「human 决定的 deferred」与「证据缺失」的诚实分界，后续口头反馈时补账。
3. **reinject 双触发缺陷（advisory 挂账，human 裁不另开轮）**：pdMS_TO_TICKS 向下取整 + vTaskDelay(0) 自旋致边界前 ~10ms 双发（corr_id 2069695/2069705），旧值注入窗口被边界注入立即覆盖、收敛 <1s——显示语义无损但日志面可辨。修法留账供后续批：next_ms 提前量（读点前 ~50ms 唤醒）/ceil 取整，或唤醒后回校验到边界再注入。这类「tick 粒度 vs 语义粒度」的边界相位问题值得进 seam 类实现的边角常识（同 K4 双分支等价性盲区一档：host 纯函数齿测不出 vTaskDelay 的取整行为）。
4. 合并门裁决上下文已备齐：ESCALATE（sdkconfig critical-path 重算）+ A3 deferred human 知情——human 合并裁决自带上下文，planner 不再另跑闸（verdict write-once 押后不变）。

## R4 追加轮体感（2026-09-22 · 双稳态缺陷修复，最后一轮为准）

1. **R3b 双稳态是「host 纯函数齿测不出 tick 量化行为」的第二次实证**（第一例 = K4 双分支等价性盲区）：[S6] 决策表 8 点全绿、真机幸运相位一次过，倒霉相位藏在 pdMS_TO_TICKS 取整 + vTaskDelay(0) + I2C 耗时的乘积里——同一个齿面盲区 motif 两棒复现，这条该进 seam 类实现的验收常识第 3 条：**睡眠/延时类板级行为，host 齿只能钉形态不能钉时序，时序语义必须设备窗实证且要跨多次边界采样（单边界观察会采到幸运相位）**。
2. **human 的根因直觉一次命中**（「为了在1秒内对齐，导致他错过了，就得差一分钟」）——放宽拍板（边界后 +2s）是典型的「用可接受的滞后换结构性消灭竞态」，比继续追亚 tick 精度便宜得多。acceptance-author 把定值钉成 tooth 10（strtol 解析 == 2000、偏离须回 planner 重冻结），「human 拍板的数值进冻齿、实现侧不许自行取值」这个授权链落点干净。
3. **R4 单点补丁轮的预算纪律**：冻结 → builder 一次绿 → r5/r6 → 收口，全程零返工；R3 形态退役负锚（tooth 9 精确串）与「禁止新旧形态并存」的负锚设计让回归面收窄到一个表达式。
4. mutation 实跑粒度差跨三棒未解（R4 仍以设计态红≙变异态在案）——合并门判官若要执行证明，补跑手法在 expectedRed 逐字在册；这笔账下棒必须处理。
