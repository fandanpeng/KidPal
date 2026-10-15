# pod-retro · 260921-m3-k1-cloud-schedule-producer（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts 为权威，本文件不复述。escalation themes：human-decision ×2（`057e9204` 派发过目确认 / `95cb31e9` 自主执行授权）、friction ×4（over-scan `5535c8de`/`67570836`、degraded `707c3c7d`/`c24713a6`）。

## 卡点与体感

1. **这一棒最特殊的体感是「契约落地发生在我的 Pod 外」**。human 与另一 planner 会话走乙通道把 CCP diff 落了（24bb1b8），我收到「contract 已经落地」时面对的不是一个待办而是一个**已发生的、动了我基线面与制品账的事实**。我的处理是先验再采信——逐字核对 4 hunk == packet §1 + §5 裁定、查 pod-events 新增的两条不是我写的事件、确认无并行会话还在动包。事后看这个顺序是对的：**并行会话的事实进场，核对义务在接棒的这一棒**，包括「对方声称跑过的测试」也要在自己的相位里复见。
2. **dispatch-stamp 我连踩两次**：①漏传 `--task-package-id`，脚本按 `current/` 指针回退解析、telemetry 误投上一包；②builder 收工时传 `--duration-ms 0` 充数，违反「绝不传 0 充数」纪律、立刻真值重记。根因①是 launch-prep 没刷新 `current/taskpackage.json` 指针（它指着上一包）——**这个指针刷新应该进 launch-prep / 起包 checklist**，否则每个新 Pod 的第一次 stamp 都埋着同一颗雷。
3. **mutation-check 的 raw-id 坑换了一副面孔又来**。M2 retro 记的是「id.slice(0,7) 键导致字段静默丢」，这次是另一形态：ARQ-097 deliberate divergence 让 checker 按 acceptance **全文**提取 id，perAcceptance 记录必须提及全文 + 带 sensitive 信号才算命中。体感教训：**retro 记「坑的形状」不如记「读 checker 的提取器再回填」**——坑会变形，机制不会。
4. **taskpackage-required 闸抓的 origin 枚举问题是拆包侧系统性隐患**：portfolio-planner 给 milestone 来源包写 `origin.type/upstreamType: "milestone"`，但枚举里没有 milestone、且 `resolveOriginType` 取 upstreamType 优先。同批拆的 K2/K3 大概率同病——**交接记忆：K2 起包前先按 k1a/m2-k3 先例把 origin 两键改 `delivery-story`（milestone 溯源由 upstreamRef 承载）**，别再烧一轮闸红。
5. **human 的自主执行授权改变了这棒的节奏**：「按 dev loop 执行 不需要我授权」之后，builder→verifier→integrator→收尾零打断，一路走完。体感：过目纪律的价值在前半程（acceptance/builder 任务书 human 各看一眼），后半程的请示是纯摩擦——这个分层是对的。
6. **enforce-scope 拦 acceptance-author 回填 taskpackage 是第三次见到「职责边界挡正确的活」**，planner 代落 + diff 验证的手法已经稳定成惯例，无摩擦。

## 别扭的 acceptance / 帮倒忙的钩子

- 无别扭验收。帮上忙的：`taskpackage-required` 与 `mutation-check` 两次闸红都是真缺陷（元数据 + 证据形状），不是闸误报；final r5 九闸含 unit/acceptance 真跑，绿得有分量。

## 下次改进

- 起包 checklist 补一条：**刷新 `delivery/taskpackages/current/taskpackage.json` 指针**（或 dispatch-stamp 恒传 `--task-package-id`）。
- 回填机检 JSON 前先读 checker 的 id 提取器/匹配语义，不靠上次的坑记忆。
- workstream 拆包的 TaskPackage origin 两键用枚举内值（`delivery-story`），milestone 溯源进 upstreamRef——K2/K3 起包前预检。
