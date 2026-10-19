# retro · 261019-arch-review-p3-record-dialogue-guide（Track B 全环：需求面 v1.65 上游 → 架构+契约面 Track B 主路径三跑续跑 → landing → verifier PASS）

> 主线程第一人称热记忆 · 2026-10-19 · 乘客 #续跑（起草在前两跑完成，本跑从「派判官四查」恢复点续）· harness 形态：主线程纯编排 + architect / architect-reviewer / landing-verifier 各为零上下文子 agent（判官跨 R1/R2 复用同实例、architect 跨修稿复用同实例）· 跨面 intake spine 消费（facePlan 顺序 2）。

**【硬项 R1】verdict 绿后即写，第一人称热记忆；起草相位非本跑经历，涉及处如实转述制品留痕。**

**【硬项 R2】计量诚实**：子 agent tokens / 时长来自 harness usage 实数；主线程（编排 + landing 逐字搬运）无逐相计量，§2 以声明口径估计（±30%）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL DRAFT v1.0（前两跑完成，17:02 落盘） | 前跑 | 前跑 | —（不在本跑） | — |
| 评审 R1 | 判官四查：意图保真 PASS / 纪律合规 4 BLOCK / 撞车 PASS / 牵连 1 BLOCK，verdict BLOCK(F1-F4) | 本跑 | 本跑 | ~15 min | 135,740 |
| 修稿 v1.1 | architect 逐 finding 修（F1-F3 断言收窄 + F4 取 (a) 实证闭环 + 四 NIT） | 本跑 | 本跑 | ~21 min | 105,295 |
| 评审 R2 | 聚焦复核：四 BLOCK 闭合实证 + 波及面扫描，verdict GREEN（附 R2-N1/N2 两 NIT 不挡） | 本跑 | 本跑 | ~6 min | 180,112 |
| 修稿 v1.2 | 两处一行 NIT 收口（判官预批修法） | 本跑 | 本跑 | ~2 min | 108,642 |
| FROZEN | 主线程凭「立项 ack + R2 GREEN + 主路径判档」标 FROZEN v1.2 | 本跑 | 本跑 | <1 min | 主线程 |
| landing | 三 canonical 逐字搬运（T1-T7 + ADR-013 + 契约三处）+ CDQ 登记 + 机械断言实跑 | 本跑 | 本跑 | ~8 min | 主线程 |
| 核验 | landing-verifier：PASS（27 项对账全 OK、canonical 超范围改动 0） | 本跑 | 本跑 | ~8.5 min | 93,251 |
| **合计** | 判官两轮 + architect 两轮 + verifier 一轮 | | | **~61 min（子 agent 实数）+ 主线程落地约 10 min** | 子 agent 合计 ~623,040 |

对照：261017 同面批（R1 BLOCK(F1,F2) → R2 GREEN）全环约当量；本批 R1 否回 heavier（4 BLOCK）但修稿一次到位、无 R2 二次否回。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（编排 + landing 搬运 + retro） | ~80,000（估计，±30%） | ~11% | 无逐相计量；含三份 canonical 逐字 Edit 与断言实跑 |
| 判官（R1 + R2） | 315,852 | ~46% | R2 含全量新文本反证复核，token 密度合理 |
| architect（v1.1 + v1.2 修稿） | 213,937 | ~31% | v1.1 修稿 54 tool uses（含 F4 三件实证现核） |
| 核验官（landing-verifier） | 93,251 | ~14% | |

花费最大处：判官两轮 46%。可压缩分析：R1 的 F1-F3（断言自相矛盾）属起草侧 checklist 缺口，若起草即自跑反证，R1 可减至一条实质 finding（F4），判官成本约省 1/3。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 验证集断言被自身改法文本击穿（R1-F1/F2/F3 三连）**：现象 = §5 三条 post-commit 硬断言（「零命中」类）与 §2 改法集新文本里的「取代 X」引述句直接矛盾——判官逐条点破，post-commit 必然 FAIL → 根因 = 起草时断言按直觉写「旧措辞应消失」，未把**引述旧句的注记/对账文本**当作新文本内容跑一遍反证 → 证据锚 = PROPOSAL §7 R1-F1/F2/F3 三条 + R2 第【一】节（判官独立复跑三 pattern 基线 grep + 对全部新文本反证）。教训一句话：**断言必须对将要落盘的全部新文本做一次 grep 反证**。
- **F2 · impact「零影响」声明漏了记账面反向核对（R1-F4，唯一实质否回）**：现象 = 契约包 impact 写「OpenClaw 本体零影响」+ 改法写「不走主 agent 共享 system prompt」，合读指向 front 直连 provider = 第五条出网路径，§1.2 与 NFR-SEC-2 记账去向未钉 → 根因 = 起草只从「机制归属」（哪个组件跑 LLM 回合）想，没从**出网枚举记账面**反向核对「这次新增的 LLM 调用走哪条已记账路径」→ 证据锚 = PROPOSAL §7 R1-F4 + T3 第 5 条（修后 provider 出口钉三件实证）。教训一句话：**凡新增 LLM 回合，§1.2 出网路径归属是必答题不是推论**。
- **F3 · 多 T 改法的派生制品同步漏项（R2-N1）**：现象 = N3 给 T4 加第七处后，T7 的 §12 行改动面枚举未随之更新 → 根因 = §12 行是「T 集的投影」，T 变化时投影要手动同步，无机检 → 证据锚 = PROPOSAL §7 R2-N1。轻但典型。
- **near-miss（最险一刻）**：F1-F3 若判官未接住，FROZEN 后 landing-verifier 跑硬断言必然 FAIL——返工点从「FROZEN 前改 PROPOSAL」变成「落地后回改验证集并重核」，且三 canonical 已落、回滚面扩大。没出的原因 = 判官四查②「纪律合规」把验证集一致性当必查项逐条对改法集核。教训：评审否回发生在这条边界上是流程按设计工作，不是流程浪费。

## 4. 优化建议（按 ROI 排序）

1. **起草 checklist 增一条「断言反证」**：§5 验证集凡「零命中」类断言，起草收尾时对 §2 全部新文本（含注记引述句、对账文本）跑一次 grep 反证，反证不满足即改断言——治 F1 · 载体 = **ARQ 收件箱**（登记为 arch-change 起草期机检候选，可考虑落 landing-fidelity-check 的 advisory）· 预期收益 = 消灭本类 R1 否回（本批三条）。
2. **契约包 impact 模板加「记账面必答」提示行**：凡 impact 含「零影响」字样，须显式核对 §1.2 出网枚举 / NFR-SEC-2 记账 / 既有 action 语义三面并留核对结论——治 F2 · 载体 = **ARQ 收件箱**（与上条同批评估是否并入 PROPOSAL 模板注释）· 预期收益 = 出网记账不再依赖判官兜底。
3. **多 T 批的 §12 行生成时机后置**：T 集全部定稿后再写 §12 行（本批因 N3 后补导致同步漏项）——治 F3 · 载体 = **登记不硬治**（一行经验，无普适机检面）。

## 5. 一句总结

子 agent 实数 ~623k tokens / 全环 ~71 min / human 触点 0 次（立项 ack 在前跑已裁，本跑无新取舍点）/ 制品 4+2 件（PROPOSAL + landing-summary + verdict + retro + CDQ 行 + spine anchors 回填）· 最险一刻 = F1-F3 三条断言若漏过则在落地后爆雷，判官 FROZEN 前接住 · **保险丝判定：两方独立结构接住了作者偏差**（判官 R1 独立产出 4 BLOCK 且修法建议被 v1.1 全数采纳；landing-verifier 独立复核 27 项对账 + 机械断言复跑全绿，与判官结论正交）· 最值得带走的一条：**断言与改法是同一份制品的两半，改一半必须对另一半跑反证**。

## 6. 本环遗留

- **TECH-261014-session-end-channels-unwired**：范围收窄注记已 append（spine q4 义务完成），结项归独立包（单阈值接线 + 结束意图二选一）。
- **验收种子转移**：五条验收断言（注入不尾随 auto-listen / 静音段 fixture / 提问句用例 / 卡壳计时五边界 / 真机停顿手感）交实现面承载包 acceptance-author（PROPOSAL §6 留根清单原文）。
- **261001 spine 结项回填**：其 anchors/结项随本批闭环由主线程回填（spine q1 ruling）；q3「说完了是否并入收尾词表」随实现面裁。
- **261016-p3-story-body-sd-cache 合并序**：实现面承载包 launch 前置硬约束。
- **卡壳阈值拍数 / 素材保全取舍**：NFR-PERF-3 待定项 + R1-N4 留根（归设计/需求面另行补裁），PROPOSAL §6 原文。
