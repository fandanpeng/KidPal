# pod-retro · 261022-m5-k4-history-corpus-backfill

标签:第一人称热记忆(planner 主观过程体感;tokens / gates / rounds / verdict / test results 不载,以 machine artifacts / pod-events / verifier-evidence 为权威;post-cutoff 不产 session-digest)。

## 卡点与体感

- **最别扭的一次返工是我自己造成的**:acceptance 冻结派发时我随手指定了「acceptance[] 本地 id 用 K4-A1.. 起」,而这个包家族(K3)的既有约定是 acceptance[] id 即 exitCriteria id(coverage 标记写 E-id)。冻结件 docstring 全部按 E-id 写,首轮 scoped 验收的 acceptance-coverage 闸就把六个 id 全判缺覆盖。修在我 finalize 权内(改 taskpackage 一行、冻结件零动),但如果不修派发口径、直接去改冻结件就变成无谓的验收面翻动。下次派 acceptance-author 前先看同目录姊妹包的 acceptance[] 形状再定 id 约定。
- **security 闸的 evidence 回填时机**:securityReviewRequired=true 的包,securityEvidence 字段要在派 builder 前就想好骨架——它是闸的硬前置,而它的内容一半来自 builder 的实现事实(零内联凭据、隔离命名空间、回滚恰删面)。本轮我在 r2 才补,等于多烧一轮。下次在 Draft TaskPackage 阶段就把 securityEvidence 的提纲写进 notes,让 builder 交付报告按提纲回填素材。
- **冻结件自身也有缺陷**:K4-A4 的回滚测试漏了一行桩注入,builder 旁路证明后由 acceptance-author 一行补齐。这条路走得很干净(唯一写点纪律 + 修复注记留痕),但说明冻结件也需要被「审」——acceptance-author 自检设计态红时,红的理由应该逐条归因(实现缺 vs 测试缺),不默认红都是实现缺。
- **快照保鲜值得跑**:K4-E2 的「设备级 peer」措辞是拆包期(10-22)写的,面 C(10-23)的 ADR-016 已经把它改成「设备级 session 键族 + 家庭单 owner」。若不是起包时逐字段对磁盘核验,builder 会照旧口径实现,验收冻结也跟着错,到 supervisor 或 live 轮才炸。这是 1f 步骤的高价值实例。
- **launch-card 缺失没有造成实际损失**:本包输入已经足够齐(brief + plan.md + roadmap 锚),但按契约仍留痕。pod-events 闭集装不下「pod 启动元事实」这类事件,只能挤进 taskpackage notes——记账面有点拧,但可接受。
- **builder 的「标记即键」裁定**:回滚批量值取 session 自身键,是冻结契约自由度内的合理选择,acceptance-author 相容性确认后一次通过。这类「builder 在契约自由度内做的结构裁定」值得在交付报告里显式带出(本轮 builder 做了,好习惯)。

## verifier↔fixer 绕轮原因主观判断

- 三轮 scoped 中两轮红均为**簿记面**(acceptance id 命名错位、securityEvidence 缺字段),实现面零绕轮——builder 一次交付 13/14,剩余一红还是冻结件缺陷。实现质量高于流程摩擦,摩擦源全在我这侧的 finalize 时机与派发口径。

## 人工纠偏体感

- 无。本轮零人工纠偏、零 AskUserQuestion,全程自动推进到停等位。

## 下次改进

1. 派 acceptance-author 前核对同家族 acceptance[] id 约定(E-id vs A-id),写进派发单。
2. securityReviewRequired=true 的包,Draft 阶段预置 securityEvidence 提纲进 notes。
3. 冻结件验收自检要求「红因逐条归因」——实现缺与测试缺分列,防把冻结件缺陷当实现红烧 fixer 预算。

## escalation themes(post-cutoff,交叉引用 pod-events)

- 本包 pod-events 无 agent-escalation / human-decision / human-correction 事件;两次 friction 均为 over-scan 纯记账(acceptance-author 3 件、integrator 5 件),无升级主题。
