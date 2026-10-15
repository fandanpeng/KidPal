# pod-retro · 260920-m2-charwindow-display-fix（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts 为权威，本文件不复述。escalation themes：本包两条 human finding（③ 播报杂质 / ④ 遮挡根因）cross-ref pod-events（`68bd12f9` / 经 `03ead7cf` 纠正闭合）；friction 两条 over-scan（`3e33ebc1` acceptance-author / `f6c4ce31` builder）。

## 卡点与体感

1. **这一棒我的最大教训是自己给的遮挡根因错了，而且是当着 human 的面**。我最初把遮挡物指认为 voice_page 右内容区（x124–388/y16–252 与右栏重叠），写进了 pod-events——architect review 复核证伪：voice_page 是默认 HIDDEN 的对象、显隐单点控制，查字窗前台时整页隐藏，且它不透明全屏，若真在屏会盖掉整窗而非一行。真凶是 `KidPalReparentChatToLayer` reparent 到 `lv_layer_top` 的**恒显顶部状态带**（top_bar_ 50% 半透 + status_bar_）。**我为什么错**：我 grep 到了几何重叠就下了结论，没有沿着「这个对象此刻在不在屏上」再问一层——几何重叠是必要条件，不是充分条件。修正动作已固化：**指认遮挡物必须同时证明「重叠 + 在屏」两个条件**。
2. **human 的两条判据把我此前的方案砍掉了一半，砍得对**。我原本按 architect 方案让云面「meaning + 组词都由 LLM 写、播报带上组词」——human 一句「现在的播报内容我觉得就够了」点醒我：**播报什么显示什么，反过来说就是没播报的不要凭空造**。组词整条从云面方案里撤掉，云面缩成一件事（把已经播报的中文解释送进 meaning 字段）。体感：**human 说「够了」的时候，删除比增加更接近意图**——我第一反应是补全数据，human 的第一反应是不要多余的东西。
3. **「板端只是展示面」这条原则值得当架构方向对待**。human 说「缺的东西将来改云端加，比板子端要方便」——这不是一句随口的话，是**内容归属权的划分**（云端拥有内容、板端只呈现）。我把它拆成落法 A（本轮，零契约）/ 落法 B（通用文本通道，契约门另立），human 同意。**B 的触发条件我采纳了 architect 的建议写成「出现第一个『云端想加 D3 既有字段外的新内容类型』的真实需求时触发」**——不留开放式欠条，也不为伪需求开契约面（architect 提醒：既有 `text` view 已是通用文本通道，B 立项时要先辨真需求）。
4. **配额中断是一次实战演练**：acceptance-author 在收尾时撞 5h 配额（429）——但它**报告已完整交付、工作全部落盘**，损失为零。我做的事：核对磁盘实况、独立复跑双通道红基线、把它的三个遗留裁决点写进 TaskPackage 后再派 builder。体感：**「agent 中途死掉」的损失上限 = 它最后一次落盘到死的窗口**；只要纪律是增量落盘，配额中断就只是节奏问题不是质量问题。
5. **跨 delta 剥离漏剥是 integrator 抓到的真问题，我接受它的 SHOULD 分级但心里记着一笔账**：流式路径逐 delta 跑 strip，`<details>` 开闭锚跨 delta 分裂时两侧都按「有开无闭透传」放行——prompt v5 禁令是第一道防线，但 LLM 遵守 prompt 不是保证。**human 的下一轮真机验证就是这道账的结算点**：如果附录再出现，直接回 triage，不讨价还价。这条已写进 review-packet 的 required action。

## 别扭的 acceptance / 帮倒忙的钩子

- **enforce-scope 把 acceptance-author 的交付拦了一次**：char_stroke_lib.py 的 docstring 指针修正（改指现役验收件）被拦——acceptance-author 的 allow 只有 `server/**/acceptance/**`，而指针要改在 `server/deploy/**`。处理：把该义务冻成一条验收齿（红）转 builder 执行（builder allow 覆盖 server/deploy）。**这是第二次见到「职责边界把正确的活挡在门外」**——上次是 CMakeLists 归 builder。这类「齿能钉的交付转义」目前工作得不错，但它依赖 planner 每次都想得起这个手法。
- **mutation-check 的 id 匹配坑我自己踩了一次**：回填 mutationEvidence 时我用 `id.slice(0,7)` 做键（"MCD-A1:" 带冒号）导致 acceptanceId 全部 undefined、字段被 JSON 序列化静默丢弃、闸报 missingSensitive。**教训：程序化回填 JSON 字段时，先打印匹配键再写入**——「静默丢字段」比「报错」更贵。

## 人工纠偏体感

- human 本包三次触点（范围四问 / 组词撤回 / 播报杂质），每次回答都直接改变了方案形态——**这棒的方向修正几乎全部来自 human 的现场反馈，而不是流程闸**。体感：真机轮的反馈回路（human 持板 → 现象 → 我查日志定位 → 对齐 → 收敛）是整条链里信息密度最高的环节，值得保持快节奏。

## 下次改进

- **指认遮挡/覆盖类根因：必须双条件（几何重叠 + 对象在屏）**，单凭几何重叠不下结论。
- **human 说「够了」= 优先做减法**：先撤方案里多余的部分，再谈补什么。
- **程序化改 JSON：改完 diff 检查字段是否真的写入**（静默丢字段陷阱）。
- **流式路径的逐 delta 处理是跨端 strip/merge 的一类系统性风险**：凡「块级操作」进流式出口都要问「锚跨 delta 怎么办」——已登记后续批（strip 累积 + 跨 delta 齿）。