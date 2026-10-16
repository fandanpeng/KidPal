# retro · 260929-gov-review-agent-def-alignment（Track A full-adversarial 全环：立项 → R1 对抗 → 拍板 → 定稿回灌 → landing → 核验 → merge）

> 主线程第一人称热记忆 · 2026-09-29 · harness 形态 = 主线程编排（moderator 职能内联）+ 四个子 agent（scenario-author / challenger / defender / adjudicator / landing-verifier 各独立派发）· 无跨乘客对照（本仓 Track A 治理批首用本形态全环）。

**【R1 趁热】** verdict 绿后即写；隔场补记声明：无——同会话热写。

**【R2 计量诚实】** token 计量来源 = harness 子 agent 回执 usage 字段（scenario-author 71k / challenger 103k / defender 88k / adjudicator 80k / landing-verifier 118k，共 ~461k）；主线程 token 无逐段计量，估 250k±30%（起草 + 拍板呈报 + landing 编辑 + 收口）。合计估计 ~710k ±30%。

**【R3 摩擦带根因 + 证据锚】**

- **F1 · 锚行形态错**：起草期 PROPOSAL 锚行未用 backtick 包裹路径 → preflight `missing-anchor-list` 七连报 → 根因 = 模板读法注第 1-3 条读过但起草时未逐条对照（260920「模板示范 = 可照抄正形」教训重现）→ 证据锚 = preflight 输出（七条歧义锚 finding）。
- **F2 · manifest 漏项**：preflight anchorUnion 15 项 vs manifest 14 行——T7 触及 landing-verifier.md 但 manifest 漏列 → 根因 = manifest 按 T 清单手工维护、无并集机检依赖 → 证据锚 = preflight anchorUnion 列表比对。
- **F3 · 回灌期 enforce-scope 拦截 architect 写 ARQ**：architect 回灌定稿时向 docs/crq append 更正注记被钩子拦 → 根因 = architect 无 crq 写槽是**正确边界**（该面写权 = 主线程登记），但派单未写「ARQ 注记由主线程落盘」的代存条款 → 证据锚 = architect 回执第 4 节。
- **F4 · OQ 拍板轮次超预期**：OQ4/OQ5 各需「具体解释一下」重裁一轮 → 根因 = AskUserQuestion 选项描述对 G3 权限面变更的执法层语义承载不足（「对齐非扩张」这类定性句 human 看不出执法层含义）→ 证据锚 = debate-log Step 5。
- **F5 · T7 清理的齿黑话残留**：human 拍板期明示散文禁「齿」，但 protected 运行时合同段与外部文件节名引用中仍留字面 → 根因 = 批范围判据「触及顺手改」只覆盖新写文本，未覆盖引用性字面 → 证据锚 = landing-summary 诚实注记 1。

**near-miss（最险一刻）**：T2 的 integrator 扩权腿。原方案拟授 integrator 两 spec 文件写权，preflight 全绿、判官裁定已含、human 已在 AskUserQuestion 中放行——human 审阅期一句质询（「integrator 为什么要写 requirements.md」）把一个**已过三重门的过度授权**拦在 FROZEN 前。为什么没出：human merge 门前置审阅真的读了 PROPOSAL；教训 = G3 权限面变更的 OQ 描述必须含执法层行为差异，靠「推荐 + 一句理由」不够。

**【R4 建议必标载体】**

1. **G3 权限面 OQ 必须写执法层行为差**：治 F4 · 载体 = **ARQ 收件箱**（owner=architect；把「OQ 选项须写 hook 行为差（exit code 级）」钉进 PROPOSAL 模板 §4 或 arch-review-protocol）· 预期收益 = 拍板轮次从 2 降到 1。
2. **manifest 生成依赖 anchorUnion**：治 F2 · 转为**登记不硬治**（preflight 已能当场暴露失配，fail-loud 够用；生成依赖留给批次 2 评估）。
3. **派单模板补「跨写域代存」条款**：治 F3 · 被本批即席修复（派单已写「写盘受阻→全文回执+代存」条款并在 ARQ 注记场景实测走通）；固化 = **ARQ 收件箱**（与派发书三条硬约定同节）。
4. **齿黑话全仓清扫**：治 F5 · 载体 = **登记不硬治**（human 裁「触及顺手改」；glossary 记规范叫法「验收测试 case」）· 留根去向 = 触及验收纪律面的下一批顺手带。
5. **T7 census 窄 pattern 漏网类**（O1 同构）：治 F5 近邻 · **登记不硬治**（verifier 人读腿已实证能接住；若复发升机检，参照 ARQ-260923 同型）。

**【R5 保险丝判定】** **接住**：判官独立（challenger 21 条 finding 中 5 BLOCKING 全部被辩出并裁实——含 F1 域外残留 UNCLASSIFIED=0 被证伪、F5 扩权定性失真两处「作者看不见自己盲区」）+ verifier 独立（O1 漏网括注、O3 写域超 def 定形均为**落地者自检后仍漏**的项，独立核验抓出）——两方独立结构各接住一处作者偏差，结构按设计生效。

**【R5 附 · 保险丝未接住面（诚实披露）】** near-miss 一例：integrator 扩权腿在判官/charter 阶段未被挑战（challenger F5 只抓了「定性失真」、没抓「该不该扩」本身），直至 human 审阅才拦——**human 审阅作为第三道独立结构接住了**；判官侧对「权限该不该给」的对抗盲区在册。

**【R5 附 · 成本对照**】 本批 ~710k ±30% / human 触点 6 次（立项、OQ 三轮、FROZEN、merge）/ 制品 4 件 + 4 ARQ · 对照 260928 批（Track B）human 触点 2 次——Track A 的对抗深度买走了正确性（5 BLOCKING 全实裁）但 human 时间贵 3 倍，方向面变更建议优先 Track B。

**一句总结**：~710k ±30% / 6 触点 / 4 制品 + 4 ARQ；最险一刻 = 已过三重门的 integrator 过度授权被 human 审阅一句拦下；**保险丝判定：接住（判官接住定性盲区、verifier 接住落地漏网、human 接住权限面盲区——三道结构各接一次）**；最值得带走的一条 = **G3 权限面 OQ 必须写执法层行为差**。
