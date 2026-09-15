# DF-E1 · reviewer-first 契约复核 evidence（开工前，复核结论 = 维持初判）

> changeId: 260911-display-first-m1 · exitCriteria DF-E1 · gateProfile: contract-drift
> 复核者: architect-reviewer（只读 agent，scope-map deny **，不落盘——本件由 planner 主线程代录，结论正文逐字转写，未改动语义）
> 派发: planner intake 阶段 0 · 完成时间: 2026-09-12（本地深夜会话）· dispatch-id: 9f27b484-1557-457a-8414-5bfbeb07f9b0
> 初判来源: `.planning/260911-display-interaction-sovereignty/review.md` F3（architect 分流 review，2026-09-11）
> 复核对象: M1「围栏闭合即发」改法（PROPOSAL §2.2 FROZEN v1.0 已裁）× `docs/contracts/interface-contract.md`（E1 display 统一响应块）
> 本件落盘路径在 acceptance-author / builder 派发之前 = 「开工前复核」时序兑现（plan.md §2.1 阶段 0）。

---

以下为 architect-reviewer 返回结论（逐字转写）：

## 总裁决

`CONFIRM_NO_CONTRACT_CHANGE`

维持 review.md F3 初判：M1「围栏闭合即发」不改 `docs/contracts/interface-contract.md` 的任何结构或语义，本包可开工。`docs/contracts/**` 零触碰，无需 architect-reviewer 四分类 + human 契约门。

## 逐项 1–5

### 1. E1 钉的是什么面？M1 是否改变其中任何一项的结构或语义？

**结论：E1 钉的是「view schema + mcp 信封 + 工具白名单 + 校验权威（front 为机械校验权威）」四个面，M1 不改变其中任何一项的结构或语义。**

契约证据（`docs/contracts/interface-contract.md`）：

- **view schema**：§1.2 L32-40 钉「display 指令顶层结构」`{"view","data","speech_hint"}`；§1.3 L44-124 钉七种 view 类型（char/schedule/story_list/story_text/text/home）的 `data` shape；§1.4 L126-136 钉 control 类。
- **工具白名单**：§1.5 L142 逐字——「机械 schema 校验（view ∈ 白名单 char/schedule/story_list/story_text/text/home/control、data shape/大小上限、脏 JSON 拒绝并降级友好提示）」。
- **mcp 信封**：§1.5 L143「display JSON 经 `mcp` kidpal/display 工具调用下发」；§四 L231「`mcp` 工具调用（云→端）……`kidpal/display`（载 E1 display JSON……网关下发前机械 schema 校验）」。
- **校验权威**：§1.5 L142 校验由网关（front）做，代码面证据是 `server/gateway/front/front_app.py` L241-268 `build_display_tool_calls` 逐块吃 `validate_display` 做机械校验、脏块降级友好提示。

M1 只改引擎侧**发射时机**（`server/deploy/engine-patch/kidpal_display_strip.py` 的 `feed()`/`finish()` 内部），不改以上任何面的结构或语义；plan.md §2「改法」仅列 strip machine API 扩展（additive）+ front 时序复核「不预期改 front 代码」（`server/gateway/mcp_relay.py` 的 `kidpal/display` relay 形状不触碰）。

### 2. 契约是否有对 display 块到达时序的钉板条款？「时序契约未钉板」初判是否成立？

**结论：成立。契约全文无任何钉住「display 块相对 TTS/回复文本的先后」的条款。**

唯一相关的表述是 §1.1 L13-14「OpenClaw 在回复文本**末尾**，用一个特殊 fenced block 嵌入结构化显示指令，流式网关负责**切分成两路**」——这是对**块在文本中的物理位置**（末尾）与**网关的两路数据流职责**的钉板，不是对「display 送达时刻必须先于/后于 TTS 开口」的墙钟时序钉板。§1.5 L142-143 逐字描述两路数据流（正文→TTS→Opus；display JSON→mcp 下发→渲染），§四 L231 描述 mcp 载荷，均无「先后 / 顺序 / 延时 / 全流结束后才下发」类约束。契约对提醒（kidpal/alert）才有重试/超时语义（§三 L218「60 秒超时未收回执即重推……上限 3 次」），display 面从未有此类钉板。故「时序契约未钉板」成立。

### 3. `kidpal_display_blocks` 侧通道是否在契约面内？多条消息化是否触契约？

**结论：不在契约面内，是引擎→front 的内部实现面；多条消息化不触契约。**

证据：

- 在 `docs/contracts/**` 全库 grep `kidpal_display_blocks` / `display_blocks` / `display-strip` → **零命中**。该字面量仅存在于 `server/` 实现面与 acceptance 冻结齿：`server/deploy/engine-patch/kidpal_display_strip.py` L25/L37、`server/deploy/engine-patch/connection.py` L57/L1456、`server/gateway/front/front_app.py` L65/L323、`server/acceptance/test_g_relay_fix.acceptance.py` L51/L74/L218 等。
- 契约面固件侧要见的是 `kidpal/display`（`mcp` 工具名，§四 L231），不是 `kidpal_display_blocks`。后者是引擎→front 的 text-frame 类型，front 在 `front_app.py` L323-329 主动拦截「INTERCEPT（never forwarded verbatim — the device must not see the engine transport frame）」——设备永远看不到该帧，其内容转由 `kidpal/display` 信封逐块转发。
- M1 把「finish() 一次性弥发全部 blocks」（`connection.py` L1444-1459 现状）改为「feed() 内块闭合即发射」，只是这两段内部通道帧的**到达次数与时机**变化；该帧不在契约 SSOT 内，故触不到契约。

### 4. §1.1「在回复文本末尾嵌入」约束的对象是什么？M1 是否兼容？M3 禁采是否维持？

**结论：约束对象 = OpenClaw/LLM 的回复文本产出格式（块必须位于回复正文之后、文本末尾，语音在前），而非网关的两路下发时序。M1 与其兼容；M3 禁采判定维持。**

证据：

- §1.1 L13-14 与 §1.6 L149（system prompt 原文）——「每次回复分两部分：先用口语化……回答（这部分会被念出来）；然后**在末尾附一个** ```display 代码块」。约束的是 LLM 输出的**文本结构**（语音正文在前、display 块在末尾）。
- M1 不改 prompt、不改块在回复文本中的物理位置（块仍位于文本末尾，其闭合 ``` 仍是流末尾附近才出现），只改变引擎**剥离出块后何时发射**（feed 内闭合即发 vs finish 统一发）。与「末尾嵌入」无冲突，兼容。
- M3（prompt 改块前置）会直接改变约束对象——把 block 从「文本末尾」挪到「前面」，与 §1.1 L14「在回复文本**末尾**」逐字冲突。禁采判定维持（PROPOSAL §2.2 / §7、plan.md 铁律 1 均与此一致）。

### 5. 设备侧消费面：view schema / 信封 / 白名单是否不变？

**结论：不变。设备侧契约三面（view schema / mcp 信封 / 白名单）与 M1 无关。**

- 设备收到的每块仍是独立的 `kidpal/display` JSON-RPC tools/call 信封：`server/gateway/front/front_app.py` L271-290 `_relay_display_blocks_to_device` 逐块发独立 `{"type":"mcp","payload":<envelope>}` 帧（L275 注释「多块多帧, no batching」），`server/gateway/mcp_relay.py` L99-101 `display_tool_call` 构造 `kidpal/display` 信封。这已是现状，M1 只是让这些块更早地、仍按序地经过同一 relay 链。
- 「display 块中途到达时按 PKG-2 归一屏 layer 仲裁渲染」属于**设备端渲染语义**，由 spec §4.0/§5.3 + 固件实现（PKG-2）承载，不在端云接口契约的 wire 格式面内。契约只管「发什么格式、经什么信封、view 是否在白名单内」，不管「设备收到后何时/如何上屏」。
- 唯一新增的负担是 front 需承受「同一轮多次侧通道到达」（幂等/顺序/回执），这由 plan.md 阶段 2 在实现面内复核，前端逐块中继代码不改形状。

## 四分类预判

总裁决 = `CONFIRM_NO_CONTRACT_CHANGE`，**无需展开四分类**。若硬对照四分类网格，本例属「无需改契约」类（M1 不随 spec 携带、不构成独立契约变更、不造成 spec↔contract 漂移）。

## 边界注记（INFO 级，不改变总裁决）

1. **时序语义确为契约空白，但本包无需补**：契约从未钉「display 送达 vs TTS 首段的先后/时延」。M1 的验收标准「display 页上屏不晚于 TTS 首段后 0.5s」是**实现包验收齿**（plan.md §2 阶段 4），不是契约条款。若将来「显示先行 / 音屏同显」升级为必须由端云两端共同保证的硬 product 面（例如要求固件对 display 到达时序做断言），再启动 contract/spec 是否有必要钉时序条款的评审——现在不钉、也不需为 M1 钉。
2. **「末尾嵌入」的措辞易被误读为时序钉板**：建议日后如需澄清，可在 §1.1 加一句「『末尾』指块在回复文本中的物理位置，不约束网关两路下发的墙钟时序」以避免与 M1 类时序优化产生歧义——纯 INFO，不改契约。
3. **display 无回执/重试契约条款**：契约 §三的重试/出队语义只绑定 `kidpal/alert`（提醒），display 面无对应钉板。M1 的「多次侧通道到达」不会引入 display 重试/幂等契约义务；front 阶段 2 的幂等/顺序复核属实现面自检（每块已带独立数字 id：`mcp_relay.py` L82-86 `_next_numeric_id`），设备 JSON-RPC 回执语义不变。

---

（转写完毕。planner 注：本结论满足 DF-E1「开工前复核 + 结论落包 evidence」；M3 禁采全程有效；复核未推翻初判，契约门不触发，本包不带契约变更走。INFO 注记 2 的契约措辞澄清建议**不在本包落地**——契约面零触碰铁律，该建议留给未来独立契约变更走四分类 + human 契约门。）
