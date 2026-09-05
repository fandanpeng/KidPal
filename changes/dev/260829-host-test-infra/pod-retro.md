# Pod Retro · 260829-host-test-infra（第一人称热记忆，planner）

> 本文件只记过程体感；tokens/gates/rounds/verdict/test 结果以机器件（verifier-evidence / pod-events / supervisor-verdict / verify-runs）为权威，不复述。post-cutoff 不产 session-digest。

## 过程走完的主观观察

**最大的摩擦 = 环境，不是任务本身。** 本包技术上是标准的「红→绿→mutation→收口」，真正吃掉时间的是三类环境性卡顿：

1. **上游安全分类器抖动了大半天**（claude-opus-4-8 分类器 temporarily unavailable），Bash/Edit/Agent 全被拦，只剩只读工具。我把 Step1-4 的成果全部落盘到 taskpackage notes 才不至于丢状态。最后靠 human 换模型供应商解决。**教训**：分类器/配额这类上游故障挡「非只读工具」时，把进度落盘再停，别原地空转重试——走错路打转更费时间。

2. **opals 子 agent 撞 5h 用量配额**（integrator 半程 429 截断），靠 SendMessage 复活续跑才收完。**教训**：收尾型子 agent（integrator 这种大盘子）要在配额低谷派，或至少预期它可能被截断、留可续跑的上下文。

3. **F-03 基座的一个连锁**：这是 infra class 包，它要服务 6 个下游 host 包，所以它暴露的「真空绿闸判据滞后」不是本包小问题——不修，下游每个 host 包全量终审都会假红。走 human 授权直改两个闸判据。

## dev-loop 流程层面想留给下次的

- **「做了 mutation 但没落 mutationEvidence 字段」**：builder 实际做了 0x336699→红 的 mutation，但没按 gate 形态归档成 `mutationEvidence`，导致 scoped 轮 mutation-check 红、又补了两个真 mutation 才过关。**改进**：dispatch builder 时明确要求「产出 mutationEvidence 字段，每个 functional acceptance 至少一条可查证红→绿」，省一轮往返。
- **红基线验证的是「基建不存在」而非「验收文件本身可编译」**：这份冻结验收从没被实编过，line99 宏参缺陷 build 时才爆。对「结构/基建类」验收，红基线应含「冻结验收文件本身可编译」这一项。已记 friction（event f0720d11）。
- **闸判据跟着约定走**：本包用 INF-A4 建立了新的 host 测试约定，但 G3 闸还停在旧约定。**改进**：建新约定/新目录结构的包，dispatch 时就提示 builder/planner 同步查一下相关机械闸是否需要认新约定，别等全量终审才发现。

## 设备现场备忘（转后续包）

human 报：设备已连接、mic 正常、喇叭未接。本包 host 基建不受影响；后续 WS / 9a / 9b 包 intake 时带上。

## 人工纠偏体感

- human 三次打断（中断派单、换模型供应商、二次「重试」）——配合度很高，都推进了而不是推翻方向。门级决策点（闸补丁处置）给了三选一、选了推荐项，授权清晰。