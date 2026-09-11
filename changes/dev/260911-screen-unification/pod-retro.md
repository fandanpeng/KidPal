# Pod Retro · 260911-screen-unification（PKG-2 归一屏）

> 第一人称热记忆（planner 全程在场的主观体感，2026-09-11）。只记过程体感；tokens / 闸 / 轮次 / verdict / test results 以机器制品为权威，本文不承载。

## 这一轮最深的体感

**两次核心设计语义偏差，都是靠我（planner）逐字读冻结验收 + 读屏幕模型代码才抓出来的，机械闸抓不住。** 一次是 acceptance-author 把前台优先级钉反（语音盖服务端页），把「同步看到相关页面」的核心目标做成「图表只在回答结束后才可见」；另一次是 builder 首轮只交付了 layer 机制、没做聊天控件物理 reparent，屏幕模型仍是两个物理 LVGL 屏并存（聊天控件屏仅 VOICE 前台置前），「聊天内容任何页可见」名存实亡。两者都在结构齿下能「假绿」——前者是验收语义本身钉反、后者是「符号零残留 + 单处创建点」两类结构断言被「改名 + 捕获屏」两条路径绕过。这让我对「机制齿绿 == 设计意图兑现」这个假设产生了实打实的怀疑：**display 层的「物理归属/可见性」类意图，host 结构齿很难钉住，得读屏幕模型代码 + 真机轮兜底。**

## 绕轮的恼火点

- 治理/元数据面反复卡包：current 指针副本与 canonical 的**行尾字节差**（我 Write 出来的 LF vs 拆包侧 CRLF，sha256 比较判不一致）、mutation 证据空、docs 同步理由缺、验收条目没有 kind 标记。代码从首轮就是绿的，能量全花在把 taskpackage 元数据对齐到闸的格式契约上。这种「代码没问题、纸面反复补」的绕法很磨人。
- 结构齿选 needle 的教训：钉「符号零残留」时，选具体变量名会被人改名绕过；改钉「载体加载归零」（所有 lv_screen_load 只指向一个屏）这种对不变式更敏感的口径才真正钉住。**钉结构齿要先想「绕过路径是什么」，不能只钉一个名字或一个计数点。**

## Escalation themes（cross-ref pod-events by eventId）

- **SU-RULING-01（deputy 代裁，方案A）**：冻结验收前台优先级「语音 > 服务端页」与 PROPOSAL §5「display 页上屏 ≤0.5s」冲突。escalation `d56d2565` → resolved `578a12eb`。
- **SU-RULING-02（deputy 代裁，方案1）**：builder 首轮只做机制未做物理 reparent（双物理屏并存）。escalation `b8e3891d` → resolved `5573d9b2`。

两次 deputy 裁断给我的共同印象：**都往「与已冻结设计权威对齐」收紧、而不是「缩包降级」**——把 acceptance-author/builder 的偏离拉回 PROPOSAL 逐字，而不是接受一次「够用了」的降级。这让后面再遇偏离时，我敢坚持走 fail-closed 升级而不是内部消化。

## 人工纠偏体感

本轮 human 通过 deputy 影子预裁通道代裁两次，未直接下场纠偏；两次都是「确认我推荐位 + 补一个执行收紧要求」的形态，没有推翻重来。

## 下次改进（留给自己）

1. **起包即把 acceptance 条目对象化**：kind:structural / suiteGate 标记、mutationEvidence 骨架、docsSyncReason 这些提前备好，别让 verifier 靠三轮元数据收敛。
2. **派 acceptance-author/builder 前，把「设计意图的可观察不变式」写进派发 prompt**（如「回答期间图表前台可见」「聊天内容任何页可见」），逼它们不只交付机制面。
3. display/渲染类包，冻结验收里就该带一条**不依赖变量名的「单物理屏」结构齿**（如「显示面所有载体加载只指向一个屏」），而不是事后收紧密集补。
4. Windows 环境写 JSON 进 `.claude/taskpackages/**` 后，**用字节级复制刷新 current/ 指针副本**，别用 Write 重打一遍全文（行尾必漂）。