# pod-retro · 261001-m4-k3-story-archive-order（planner 第一人称热记忆）

> 写于 closeout 完成之后、supervisor 机械合并门之前。机器事实以 verifier-evidence/rounds、supervisor-verdict.json、closeout 制品为权威，本件不复述数字，只记过程体感与判断。

## 一、本棒最值钱的一笔：两个「形态」缺陷拦在机械闸前，都不是实现缺陷

本包实现本身**一次过、零 fixer 轮**——但内循环并未因此一步到绿。两次红都不是代码错，而是**产物形态**错：

1. **launch-prep 的 `acceptance[]` 全角冒号**。`acceptance-coverage` 闸从 `acceptance[]` 抽短 id 时只认 ASCII `:` 前那段；launch-prep 写的是全角 `：` + id 后跟括号说明，于是抽出来的「id」成了整段散文，标记永远找不到。这条**不是本包独有**——同批的兄弟包 C 用了一模一样的形态（B1/C 同一脚本产出）。我按仓库规范形（对照已 merged 包 `"K3R-A1: …"`）归一后才过闸。已记 friction：C 包 intake 若不修会再撞一次。
2. **planner（我）finalize 漏 `mutationEvidence`**。本包 `mutationRequired=true`，我照 Step 4/6 逐项填了 acceptance/exitCriteria/红基线/docsOwner，却把「每个 functional acceptance id 须挂敏感变异证据」这一步整个漏掉——因为它不在 /build-feature 的字段清单里，只在 `mutation-check` 闸里。收口时补上 builder 的四条真实变异重放（去 pending 分支 / routing 回 chat / 去 sweep 释放环 / 不剔词，各自钉红对应齿）才转绿。

**教训**：这两条都是「清单里没写、闸里才有」的隐形必填项。它们不阻断正确实现，但会把一条本该一轮绿的干净包拖成两轮。`acceptance[]` 用 ASCII 冒号、`mutationRequired=true` 时必填 `mutationEvidence`——建议进 launch-prep 产物模板 + planner Step 6 预检清单（与首版 K3 retro 提过的「TaskPackage 元数据四条预检」同族，可合并成文）。

## 二、顺畅的一笔：冻结齿的「绑定约束」传得到位，换来了零 fixer 轮

验收作者把两组张力显式钉进文件头（ⓢ pending 超时须并入 `_story_timeout_sweep` 才有 10s 可机判；ⓣ 负锚只钉 `routing != "chat"`、不钉内部枚举名）。我在派 builder 时**把这两条当硬约束原样转达**，并钉死「SAO-A2 的 routing 不得回 chat」「SAO-A3 超时须经 sweep 释放」。builder 据此选了新枚举值 `pending_stop`（既满足 ≠chat，又与既有 120s `timed_out_sessions` 天然隔离），一次成型。

**体感**：验收作者标注「弱于契约处/线束假设」不是软弱，是把「我假设了什么」摊给下游——planner 把它转成 builder 的绑定约束，就等于把一条可能绕轮的暗雷提前拆了。这个「验收作者显式申报假设 → planner 转成硬约束 → builder 按约束选形态」的三棒，比事后 fixer 辩论便宜得多。

## 三、别扭处：一条「以齿代现象」的老账，本包刻意不再犯

上游 repair 包 retro 的教训（host 齿全绿 ≠ 真机现象对）在本包有个天然对照：本包的缺陷本源是**真机实录**（归档早于转写的 1.7s 顺序），而验收齿全在 host 侧跑。我让 integrator 在 Review Packet 的 demo 对账行里**如实标 SAO-E3 真机复验为 human 持板 pending**，不因为「host 齿全绿」就把真机项写成已过。这正是 repair retro §二想根治的方向——本包至少在**留痕**上没重蹈。

## 四、与 B2/B3 的边界：记账干净，但有一条值得盯

plan §6 划得很清楚：本包只交付「KEY 兜底收尾在 manual 模式可用」；120s 静默 sweep 复活、逐句回显、说「我说完了」自动收尾、引擎侧抑制都归 B2/B3。builder 与 integrator 都如实把 **120s sweep 路径未加剔词**（`_story_timeout_sweep` 内 `stop()` 仍是默认 `strip_finish=False`）登记为未覆盖面。

**我盯一句**：这条「120s 未剔词」在 plan 里归 B2/B3 的理由是「manual 模式下该分支 inability-to-fire」。等 B2/B3 落地 auto 模式后，该分支会真的能触发——届时若没人记着这行申报，就会变成「auto 模式下 120s 归档正文带收尾词」的新缺陷。已在未覆盖面申报留根，供 B2/B3 立项时消费。

## 五、下次改进（具体到动作）

1. **launch-prep / planner 形态预检成文**（本件 §一两条 + K3 首版四条合并）：`acceptance[]` ASCII 冒号规范形；`mutationRequired=true` ⇒ `mutationEvidence` 逐 functional id 必填（含 `acceptanceEntry` 全串 + 敏感信号）。
2. **兄弟包 C 联动**：`261001-openclaw-model-name-overlay-drift` 的 `acceptance[]` 是同形全角冒号，其 intake 须同修，否则 `acceptance-coverage` 必再红一次（已记 friction）。
3. **`mutationEvidence` 的 `acceptanceEntry` 字段**是个易漏的形态（须逐字等于 `acceptance[]` 的 raw 全串，供 `mentionsId` 子串匹配）——写入预检清单时一并点明。

## 六、escalation themes

本棒**零 escalation**（无 Red Flag / blocker / out-of-scope / human ruling request）。全链 human 接触点仅剩 SAO-E3 真机复验（持板项，pending）。pod-events 中无 `agent-escalation` 事件。
