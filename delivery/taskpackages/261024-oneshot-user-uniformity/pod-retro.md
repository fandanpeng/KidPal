# pod-retro · 261024-oneshot-user-uniformity

> 标签：第一人称热记忆。不载 tokens / gates / rounds / verdict / test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。post-cutoff escalation themes 交叉引用 pod-events eventId。

## 过程体感

- **起包顺滑**：自建形状(human launch 直给 changeId + 方向原话),范围/禁触/红基线在 launch card 里已备齐,零 triage 摩擦。current/ 指针刷新 + 字节复制纪律照跑,一次到位。
- **architect 预评(1g)是本包最大杠杆**:两条修正都改变了包的形状——① 条件传参把爆炸半径从「八处桩签名同步」缩到「零改」;② prompt 制品部署洞(manifest 不登记 prompts/*.md → live 可能根本没装)是我完全没看到的假绿通道。没有这一档,「挪指令」就是裸奔。Lite 简版预评性价比极高。
- **别扭点**:verify.mjs final 相位要 --scope-file,而我第一跑没带——报错信息清楚(指向 integrator Step 4),但这意味着 planner Step 10「终审 full bearing 跑一次」实际只能排在 integrator 之后,步骤文档读起来像 Step 10 先行。不算卡点,记一笔流程体感。
- **派发卡算术笔误**(333+3=336)被 builder 实测当场纠偏——红基线数字口径(换代后 3 failed 计入 333 总数)我转述时想当然了。以后派发卡写目标数字前先自己跑一遍 --collect-only 对齐。
- **verifier↔fixer 内循环:零绕轮**。acceptance-author 冻结的三条设计态红全部一轮转绿,冻结质量高;freeze 回执里「排除本件 330/0」的半绿基线让 builder 起跑时边界极清楚。
- **human-decision 留痕前置**:方向反转(零 user → 必携 user)是语义 180° 掉头,launch 时就把 human 原话 + 裁决范围落进 pod-events,后面 acceptance-author 写换代事由、review-packet 引用都有据可查,没有出现「回头找授权」的尴尬。

## 下次改进

- 派发卡里的预期测试数字,先实跑 collect 对齐再写。
- final verify 的 --scope-file 依赖链(final 必须在 integrator 圈定之后)可以在自己脑子里排进固定顺序,不再试错。

## escalation themes(pod-events 交叉引用)

- 本包零 agent-escalation 事件。friction 仅 over-scan 两条(a1963744 后 f6cffbb0 / d3572dc8),纯记账。
- loop-retro 阈值触发(33 ≥ 3)由 closeout-check advisory 报出——发起责任在主线程,不在本 Pod 收口内。
