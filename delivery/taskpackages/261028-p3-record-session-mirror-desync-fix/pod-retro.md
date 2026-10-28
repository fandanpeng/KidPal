# pod-retro · 261028-p3-record-session-mirror-desync-fix

> 第一人称热记忆（planner 收尾相位，integrator closeout 之后、supervisor gate 之前）。只记过程体感与下次改进；不复述、不载 tokens / gates / rounds / verdict / test results——那些以机器件与 pod-events 为权威。

## 这一包是怎么走过来的

起点很干脆：human 报「故事列表页卡住、长按不回首页」，我先拉设备日志，两条 `show_home suppressed` 直接把矛头指到守卫判据，再顺藤摸到镜像与 ctx 两个载体各滞后一次。归因写清楚之后 human 一句「可以，改吧」，剩下的全是流程。

真正花时间的不是代码，是起包面。单包 bug 修复本来在我直觉里是「改三行」的量级，但 `/build-feature <changeId>` 的 launch 面预检是硬路由：board 条目、启动卡、worktree、图索引缺一就退回 launch-prep。我一开始觉得这是为一条三行修复办手续，走完发现格式化的好处是收尾时能一路机械推进——基线锚怎么引用、base ref 用哪个 SHA、上包留根读哪份，全在启动卡里，后面几棒谁接都不用重新问。

## 卡点与判断

**最刺眼的一次红是我自己造的。** scoped round 1 挂在 `mutation-check`，报「Mutation evidence is missing or insensitive」。我第一反应是闸坏了——证据明明写了 `turned red`。读脚本才明白：它按**完整 acceptance 原文**做 id 匹配，而我填的是短 id。这是我的文书错误，不是代码错误，也不是闸的问题。教训很具体：这类闸的匹配口径不看「看起来合理」，看「逐字一致」。代价是白白烧掉一个 round 序数。

**这一包真正的不安来自验收的诚实边界。** 三处修法全落在 `application.cc` / `kidpal_render_node.cc` / `mcp_server.cc`，这三个 TU 不进 host 构建，所以 host 侧能写的只有源码结构锚——「符号在这个窗口里、次序对、负锚不散落」。这些锚能拦住工程回归，但拦不住「运行时到底修好了没有」。我没有让 acceptance-author 去凑假的行为断言，而是把三分（结构锚 / 可执行 canary / 只能真机目检）逐条写进件头和 TaskPackage。代价是这一包在真机轮之前，任何「已修复」的说法都必须带前缀。我认为这个诚实比多几条绿灯值钱，但它也让「闭环」这个词在这一包里推迟到了 human 持板那一步。

**扩展会话的降级是可感知的。** 这个会话跑在 VS Code 扩展里，传不了 `--settings`，于是 dev-loop.settings 的三件（Stop 闸、PostToolUse 自动 reconcile、git merge/rebase 的 ask）全程缺席。我把它们记进了 `settings-presence` 事件，收尾手工补跑。体感上最别扭的是「没有自动 reconcile 兜底」——每一步 board 状态都是我显式敲的，多一次手动就多一处可能忘。

## 帮倒忙的地方

`taskpackage-required-check` 要求 `contractRefs` 非空，即便本包**零契约改动**。为了过闸，我只能把 `docs/contracts/interface-contract.md` 作为「被引用但未改动」的契约列进去——这在语义上是诚实的（它确实是被参照的契约），但字段名会让人误以为本包碰过契约面。同文件里 `crossEnd.exempt` 已经用一句话写清「零 wire 改动」，两处口径并存反而增加读者负担。建议后续考虑「契约面零改动」的显式取值，而不是靠列一个不改的文件来满足非空。

另一处是 `mutation-check` 的 id 匹配口径缺乏前置提示：起草 TaskPackage 时没有任何地方告诉我「证据条目必须内嵌完整 acceptance 原文」。这属于闸的可用性，不是判据问题；写进 `taskpackage.json` 模板或 brief 的提示位就能免掉一次红。

## 下次改进

1. 起草 TaskPackage 的 `mutationEvidence` 时，**先贴完整 acceptance 原文再写变异描述**，不要用短 id 图省事。
2. 单包 bug 修复遇到 launch 面五件缺失时，别把 launch-prep 当负担——它产出的启动卡在后面每一棒都在省时间。
3. 设备 TU 不进 host 构建的包，验收设计一开始就把「三分申报」写死，避免中途被迫在「假行为断言」和「覆盖缺口」之间二选一。

## Escalation themes

本包无 agent 升级事件（subagent 全程无 Red Flag、无 out-of-scope、无 human ruling request），故无 escalation theme 可交叉引用。
