# pod-retro · 261016-sd-card-mount

> 标签：**第一人称热记忆**（planner 收尾相位，integrator closeout 之后、机械 supervisor 闸之前）。
> 负向边界：本文件**不复述**不冠权威、不载 tokens / gates / rounds / verdict / test results——那些以
> 机器件（verifier-evidence / pod-events / supervisor-verdict）为权威。本文件只记我全程在场的主观过程体感。

## 这一轮是什么

真机门停等点的**第 2 次接续**。上一轮我停在「SD-A5 冻结齿在设备上无触发面（实现零消费方）」这个发现处，
human 裁定补最小实现、不改验收；这一轮 human 带着真机轮的实测结果回来，并给了无卡轮的口头裁定。
整轮编排的骨架是：两条 human 裁定 → 冻结齿措辞修订 → 复跑闸 → 收口 → 终验 → 机械合并门。

## 我最满意的一处：豁免被落成「诚实形态」而不是「和稀泥」

SD-A6 无卡轮不可执行，human 裁「豁免 + 结构性证据替代」。我主观上最担心的是这类裁定最容易被悄悄
做成「用替代证据冒充原始观察」——措辞上把「代码面静态断言」说成像是「真机验证过」。这次守住了：
acceptance-author 在冻结件里明写「**代码面静态结构断言，非运行时真机观察**（明确不冒充真机观察）」，
在 review-packet 的未覆盖面申报里把「KEY 发起会话冒烟本机不可得、无结构齿承载」**如实列成差集**，
没有用降级路径的 panic 负向齿去覆盖它、假装三观察项都齐了。这条差集被明着交出去，比被糊住有价值得多。

同理 SD-A5 的第三个子项（原理图三脚目视核对）——human 声明没单独做。我没有把它当「反正 mount 成功
就默认销账」，而是让 acceptance-author 把该子项改写成「目视核对未单独执行、由 config.h 宏取值 + 真机
挂载成功实证反证」并留痕。真机 mount 成功确实强于目视核对（接错脚不可能挂上），但「没做」这件事
本身要写在纸面上，不能用结论强度替代过程事实。

## 卡点（真实、值得记的一条）

**plan 的【doc 行落点】通道假设在本 Pod 的会话形状下不成立。** plan 写「`docs/notes/**` 无 agent 有写权
→ 主线程通道代落（enforce-scope 主线程不受 per-agent 约束）」。但本 Pod 是以 `--agent planner` 起的
（launch-card 的命令行形态如此），enforce-scope 钩子读到 `agent_type=planner`——**于是「主线程」就是
planner 主体、受 planner 的 per-agent 边界约束**（planner 的 `deny` 含 `docs/**`）。我实际下手写
`docs/notes/board-hardware-esp32-s3-rlcd-4.2.md` 时被钩子拦回。回头查 scope-map：**仓内没有任何 agent
的 allow 覆盖 `docs/notes/**`**（integrator / architect / story-author 都不含）。

教训：写「主线程通道代落」这类通道假设时，必须显式区分**「无 agent_type 的裸主会话」**与
**「`--agent <role>` 起的 Pod 主会话」**——在 enforce-scope 眼里这两者不是一回事。前者不受 per-agent
约束，后者受。起包期核 scope-map 时，我只核了实现面的写权（config.h / 板 .cc / CMakeLists / 验收件），
**没把「本包 doc-sync 落点的角色写权」按 agent 粒度逐条核**——这是可以更早发现的。

结果：一个一行的文档改动变成必须交回主线程的公开未闭项。不算事故（可交回、已留 exact text），
但它把一个本该在起包期解决的信息差拖到了收尾期。零脚本改动建议——这是「起包 checklist 该多一格」
的问题，不是门的问题。

## 帮了忙的机制

- **scoped / final 分离 + immutable round fact**：这次验收件是**注释面措辞修订**，「改措辞会不会影响闸」
  这种问题如果靠推理容易心虚。我没推理，直接复跑一轮拿到逐闸结果与 immutable fact——机器给确定答案
  比人给判断便宜太多。批评改冻结件措辞时，这套机制让我敢改。
- **pod-events 的 human-decision 事件**：human 那句原话『不好取，不取了，以后也不会拿出来』本身
  就是最干净的豁免事由——直接可引、不需要我再包装成体面的措辞。裁定原文入账这一条，省掉了
  「把口语翻译成治理语言」这层最容易失真的环节。

## 别扭处 / 下次改进

1. 起包期把「doc-sync 落点的角色写权」核到 agent 粒度（不只核实现面）——本包的实际卡点就出在这。
2. 通道假设的措辞：以后写「主线程」时一律写明是「无 agent_type 主会话」还是「Pod 主会话」。
3. 真机门停等的 board 状态仍是老问题：闭集里没有 `awaiting-human:real-device`，只能退到 `blocked` +
   事件承载精确语义（这是上一轮就记过的 friction，本轮沿用同一处置）。**建议留给治理面**：闭集
   是否该为「等人操作」单列一档——本节只记体感，不主张开单。

## escalation themes（cross-reference pod-events by eventId）

- **真机门停等（本包两次）**：`86b65165`（第 1 次，代码面齿全绿后停等）→ `4ca9b808`（第 2 次，发现
  SD-A5 无触发面后停等）→ `bd8c8b56`（human 真机轮裁定后消解、resume）。
- **真机轮裁定（human-decision）**：`0263a7a3`（SD-A5 有卡轮 PASS + 原理图子项声明）+
  `416e5c42`（SD-A6 无卡轮：本机不可执行豁免 + 结构性证据替代）。
- **friction（本轮新增）**：`26636485`（plan 的 `docs/notes` 通道假设 vs enforce-scope 实况脱节）。
  上一轮遗留：`0d015481`（launch-card 的 `awaiting-human:real-device` 与脚本闭集脱节）。
