# pod-retro · 261010-device-reconnect-on-server-close（第一人称热记忆）

> planner 全程在场的**过程体感**。非第二事实源：不复述 tokens / gates / rounds / verdict / test 结果——那些以 machine artifacts / pod-events / verifier-evidence 为权威。标签：第一人称热记忆。

## 本包最深的体感：host 结构齿全绿 ≠ 行为全对，运行时断链藏在态跃迁里

builder 交付时 host 153 全绿、变异自证三中三，看起来可以直进 verifier。是 builder 自己把第 7 节观察写出来交回（[S7] 抑制本地关回调 → Idle 收尾失去承载 → 常驻重建守卫永不放行），而不是等真机轮 RC-B4 才炸——这个「实现者主动上交疑点、不越界自补」的姿态是本包最值钱的一次交付质量。我随即走 10b advisory 档派 architect，一轮裁定（收尾归调用点 + :429 同批必办）、fixer 一轮命中。**如果 builder 没有主动上报，这条链会静默烂到真机轮才暴露**——「验收面从零钉」的包，钉得住结构钉不住运行时，派单时该把「行为面观察单列一节」写进 builder 模板。

## 我自己的簿记缺陷被机械闸逮住一次，另有一处派单内容错误

1. mutation-check 红：我代落 mutationEvidence 时把 acceptanceId 截短（自己概括了后缀），而匹配是全串包含——六条全 miss。教训很具体：**代落 acceptanceId 必须从 acceptance[] 逐字复制，禁止手抄概括**；这条值得进派发备忘（复制粘贴，不复述）。
2. 给 integrator 的速览里我写了 `firmware/main/test/session_logic.acceptance.c`——该文件不存在（我凭记忆写的）。integrator 没有照单全收，ls 实证后用真实承载件替代并在报告里纠偏。**派单里的具体文件路径必须先验证存在再写**，这次是运气好（遇到靠谱执行者），不该赌。

## 顺序踩坑一处：final 轮依赖 integrator 的 scope-file

我按 Step 10 惯性先跑 final，被 fail-closed 红闸举了手（final 相位缺 --scope-file）。复盘看流程条文本来就把「影响面圈定」归 integrator Step 4——**Full 包的 verifier 序列天然是 scoped → integrator 圈定 → final**，我应该在 scoped 绿后直接派 integrator。这个顺序这次是红闸教的，下次应该自己想到。另外轮序计数（completed-only）不吃「失败尝试」，第一次 final 尝试没落 fact 不占序——机械设计是对的，我省了一次困惑。

## 写权 handoff 通道第二次顺畅运转

上一包 retro 的教训（派单写权核对只看 agents 段枚举）本包预落实：integrator 派单时即写明「docs/architecture.md 不在你的 allow 面、arch 文本进 docs-delta」，architect 落笔派单即带 docs-delta 逐字约束——零撞钩子、零返工轮。integrator 起草 → architect 落笔 → integrator 抽查 commit 的三棒接力一次过，**上游把精确文本钉死、下游逐字落地、中间零漂移**，这个形态比「谁有权谁自己想措辞」质量高一个档次。

## acceptance-author 的 vacuity 自证值得保持

冻结轮就把 M7-M12 做完（假实现 → 红 → 还原 → sha 核对），builder 阶段的实现面变异直接复刻编号。负锚（RC-B3）和现状守卫（RC-B1）「设计使然绿」的空过风险在冻结相位就消解了，红基线判读（4 红 2 绿的 2 绿为什么绿）头注写清楚，我判读零停顿。**「红基线不齐 = 全红」不是目标，「每条红的归属讲得清」才是**——这包的红基线报告是范本形态。

## 人工纠偏体感

human 在本包出场两次（都在预期位）：立项裁定（「永远不休眠」触点①，拆包前已落 plan）与 upcoming 的合并门 + 真机轮（riskTier high 必升 + RC-A4/B4/B5/B6 持板）。loop 内零请示——10b advisory、写权 handoff、T 值冻结都是既定通道内自主完成，符合「human 只花时间在升级门/契约/真机/预算」口径。

## escalation themes

- 无 `agent-escalation` 事件（10b advisory 常规不落 escalation，按条文防语义滥用）。
- friction 四条：over-scan 三条（acceptance-author 4 行 / builder 6 行 / fixer 零越界申报一并记 1 条）+ scan-miss 一条（我的 acceptanceId 截短缺陷）。
- builder 的 HIGH 行为观察走的是交付报告第 7 节而非 escalation——形态正确（未到 blocker 级），通道值得保持。

## 下次改进（三条）

1. **builder 派单模板加一句「行为面观察（运行时态跃迁 / 异步时序 / 守卫前置满足性）单列一节交回，不越界自补」**——本次是 builder 自觉，该成文。
2. **代落 taskpackage 字段一律复制粘贴源串**（acceptanceId / sourceRefs 全路径形态），不复述不概括；scan-miss 这次是转写截短，下次可能是别的字段。
3. **Full 包 verifier 序列模板化：scoped 绿 → 立即派 integrator（圈定 + 制品）→ final（带 scope-file）→ integrator 二轮 commit**，把「final 依赖 scope-file」写进我自己的心智顺序，不靠红闸再教一次。
