# pod-retro · 261023-honcho-memory-wiring-restoration（第一人称热记忆）

> 本文件是我（planner）全程在场的第一人称过程体感；不载 tokens/gates/rounds/verdict/test results——这些以 machine artifacts / pod-events / verifier-evidence 为权威。

## 卡点与体感

1. **coverage marker 形态是我起包期就该预判到的**：acceptance-author 把本地 ID 放在 `coverage:` 第二位，coverage 闸只认首位，为此多花了一轮派发。上一批（261023-agents-md）先例就是「本地 ID 置首」，我在派发 prompt 里没写这条机械约定——下次派 acceptance-author 做新 ID 体系时应直接把「coverage: 本地 ID 置首」写进派单。
2. **红基线对账预设我算错了一处**：我在 plan 里预设「排除本件 = 316/6/33」，但 316 是含旧件的全量数，排除本件的等价形态实为 306/4/33。acceptance-author 如实指出并给出转绿预言 318/7/33，我照实回填——没有硬凑预设数字，这是对的。教训：写判法预言时先想清楚「基线数含不含本件」。
3. **brief-inputs 的锚点 ADV 值得下次起包顺手清**：5 条 ref 无锚点告警（整文件引用退化），本包 advisory/plan 都是定向章节消费，下次起包把 #锚点 带上能封住阅读面。
4. **builder 对「五顶层键」字面读法的补充申报处理得当**：声明为满足 `_json_get` 根级嵌套断言加了顶层 `plugins` 嵌套镜像键，builder 没有静默处理而是如实申报为「加键未删键」——冻结断言与声明形状的这种咬合缝，以后 acceptance-author 冻结 `_json_get` 类断言时应同步说明声明文件的目标态形态。
5. **并行会话在同 worktree 的在途面是个新体感**：面 C（/arch-change）PROPOSAL 草稿和三件 spec 面改动在本 loop 进行期间陆续出现在工作区。我和子代理都做到了零触碰、零混提（integrator 的 stat 核对与如实申报是关键防线）。多会话同 worktree 并行会常态化，派单时显式写「不 commit 的路径清单」这条应保留成惯例。

## 下次改进

- 派 acceptance-author 的派单模板补两条机械约定：coverage marker 本地 ID 置首；冻结声明类断言时同步冻结声明文件形状语义。
- 判法预言自检问句：「这个全量数含不含被改件？」
- 起 run 前 `git status` 快照留档（本包有起始快照，识别并行会话产物全靠它，好用）。

## escalation themes（cross-ref pod-events）

本包零 agent-escalation 事件；两次 coverage/red-baseline 形态问题均在 planner↔acceptance-author 派发轮内闭环，未触发升级链。human 触点仅 launch 指示（human-decision launch-ack 已记 pod-events）。
