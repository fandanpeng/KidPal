# T2 · 传输方向 B 三面对齐 · Challenger 红队 findings（轴 1/3/4）

> challenger 轮产出（2026-09-02）。主线程代落盘，内容保真。判据 = `review-charter.md`（冻结）。

## 摘要

- 严重度分布：**BLOCKING ×3 / SHOULD ×7 / NIT ×2**，共 12 条。
- canary 重派生命中：**4/4**（canary 1、4、5、6 全部独立重新找到）。
- 疑似 BLOCKING：**是**（3 条，均属可冻结性头号内部矛盾 / 铁律命中）。
- Top 3 最锋利：
  1. **E3 `alert` 通道自相矛盾**（spec:134「小智协议原生类型，固件已有渲染处理」× spec:538/585「小智原生 alert 弃用」），且固件原生 alert 处理器（`firmware/main/application.cc:590-598`）吃的是 `status/message/emotion`+震动音，根本不是 S-F1.4 的 `state-reminder` 弹窗——「固件已有渲染处理」为假。
  2. **§3.2–§3.4 整段仍是 v0.1 HTTP + faster-whisper 叙事**，与方向 B 互斥（spec:202/204/217/232/240），且 VAD 切句位置漂移（本地 spec:239 vs 服务端 spec:78/535）。
  3. **spec↔契约硬漂移命中铁律**：契约仍是轮询 `/v1/alerts`（contract:218）+ `remind_before_min`（contract:164）+ mp3 下行（contract:26），契约门未重写完成前 spec 不可冻结（canary 6 / DEV-GOVERNANCE §4）。

## Findings（falsifiable-first，引 文件:行）

### BLOCKING

**T2-F1 · 轴4（内部矛盾，canary 4）：提醒载体「alert」在 spec 内以两种互斥通道形态定义**
- 冲突行：spec:134（E3）「经 WS 推送 `alert` 消息（**小智协议原生类型，固件已有渲染处理**）」；spec:111（B3）「推送 `alert` 消息（不再轮询）」；spec:80（§3.0.0，方向 B 权威概述）「提醒走 alert 消息」；spec:221（§3.3 图）「stt/tts/custom/**alert** 消息」——这些都指**原生 alert 通道**。
- 对立行：spec:538（§6.1 mcp 行）「小智原生 **alert/custom 通道弃用**」；spec:585（决策 23）「小智原生 alert/custom 通道弃用」；spec:325（S-F1.4）「经 WS 推送 **`mcp` kidpal/alert 工具**」。
- 一句话：同一「提醒推送」在 spec 内既是「原生 alert 通道」又是「mcp kidpal/alert 工具、原生 alert 弃用」，方向 B 的核心载体定义自相矛盾。
- 代码佐证（加重 severity）：固件原生 alert 处理器 `application.cc:590-598` 只认 `status/message/emotion` 三字段 + `OGG_VIBRATION`，与契约包 v2 的 kidpal/alert 载荷 `{id, fire_at, title, speech, view}`（CONTRACT-CHANGE-PACKET-v2:31-34）形状不匹配——即便不弃用原生 alert，「固件已有渲染处理」也**不成立**，S-F1.4 的 `state-reminder` 弹窗（排队 + 渲染即确认）是全新固件面。
- elicitation：否（实体核验已定位）。

**T2-F3 · 轴4（内部矛盾，canary 5）：§3.2/§3.3/§3.4 整段仍是 v0.1 同步 HTTP + faster-whisper，与方向 B 互斥；含 VAD 切句位置漂移**
- 冲突行：spec:202「ASR…云端（**faster-whisper**，经 shim `/v1/asr`）」；spec:204「TTS…（edge-tts，经 shim `/v1/tts`）」；spec:217「公网 **HTTP/WebSocket**（Opus 音频 + JSON）」；spec:232「ASR: **faster-whisper**」；spec:240「音频经 shim `/v1/asr`（faster-whisper）→ `/v1/chat` → `/v1/tts`（edge-tts）」；spec:566（决策 4，未标记废弃）「薄桥 shim（FastAPI…ASR/TTS HTTP 口子…提醒轮询）」。
- 对立行：spec:79「ASR（阿里云云 ASR，云优先）」；spec:101（A1）「**不用 faster-whisper**」；spec:581（决策 19）ASR 云优先；spec:90 Pod S HTTP 台阶「方向 B 落地后退役」。
- 附带语义漂移：VAD 切句位置——§3.4 spec:239「本地 VAD 检测说话结束」对 §3.0.0 spec:78 / §6.1 spec:535「**服务端** VAD 切句后送云 ASR」，同一「切句」本地/服务端各说一次。
- 一句话：spec 前半段（§3.2–§3.4）用 v0.1 同步 HTTP 叙事，与更新段（§3.0.0/§6.1/决策 9/19）互斥，是可冻结性头号内部矛盾。
- elicitation：否。

**T2-F4 · 轴4（spec↔契约漂移，canary 6）：契约现有文本与 spec 新模型硬冲突，命中「契约未批 → spec 不可冻结」铁律**
- 漂移点（各引 contract 行）：
  - contract:218「设备轮询 `/v1/alerts` → shim 读到并清除」vs spec:325/582 推送 + 渲染即确认；
  - contract:164/189/194 `remind_before_min` vs spec:320/582 三档阶梯废止（B1 零变更与实况不符）；
  - contract:26「正文 → shim 送 `/v1/tts` → **mp3** → 设备播放」vs spec:537 下行 Opus 帧；且 contract:140「（小智协议 tts + **Opus**）」与 contract:26「mp3」是契约自相矛盾；
  - contract:141「（小智协议 **mcp 或自定义消息**）」vs spec:585 custom 弃用；
  - contract:131-134 §1.4 control 动作表缺 `start_recording` vs spec:363/587；
  - 契约**无** hello/Opus 帧/stt/tts/abort/鉴权/回执的「协议剖面节」，而 spec:531 §6.1 将小智协议剖面引用为事实源（契约包里「§四」尚是拟新增，未落盘）。
- 一句话：契约门（CDQ-260902）重写完成前，spec 冻结即命中「契约未批 → spec 不可冻结」铁律（charter §5 canary 6 + DEV-GOVERNANCE §4）。
- elicitation：否（charter §1 已将「契约改法本身」排除在本 charter 之外，此为漂移严重度定位）。

### SHOULD

**T2-F2 · 轴3（术语漂移，canary 4 附支）：`custom` 通道「弃用」与「custom/mcp」并写残留并存**
- spec:585「custom 弃用」对 spec:542「经 `custom`/`mcp` 消息下发」、spec:623「display 经 custom/mcp」、spec:80「display JSON 走 custom/mcp 消息」。
- 一句话：方向 B 权威概述（§3.0.0）+ 契约门待办（§9）仍写「custom/mcp」，而决策 23 已裁 custom 弃用，builder 读到三种通道结论。

**T2-F5 · 轴4（三面对齐，决策 22 实现含义）：「CA 嵌入固件」与固件 SSL 层实况不符**
- spec:554/584「wss + 自签长期证书…**CA 嵌入固件**，换证需动固件/重新烧录」。
- 固件实况：`firmware/managed_components/78__esp-ml307/src/esp/esp_ssl.cc:34` `esp_tls_cfg_t cfg = {}`——空配置，无 `cacert_buf`/`cert_pem`/`clientcert` 任何证书加载路径；`web_socket.cc:126-127` 「wss → CreateSsl」仅确认 TLS 可用，证书校验/自签 pinning 无落点。
- 一句话：「嵌入 CA」在固件 SSL 层没有对应代码面，验证材料 §2 的「无硬阻塞复活清单」全程未触及 TLS/证书；「CA 嵌入固件」的换证重烧运维影响（P3 已接受取舍）在固件侧实际还要新增证书加载实现，属完整性/实现含义缺口（非攻击自签证书取舍本身）。

**T2-F6 · 轴4（三面对齐，决策 22 实现含义）：强制鉴权「token 未设 = 拒绝服务」在官方服务端承载未验证**
- spec:552/584「强制鉴权：token 未设 = 拒绝服务，不留 validation-mode 后门」;spec:91「两个小补丁（结构化下发 push helper、提醒定时推送）」——补丁清单**不含鉴权强制补丁**。
- 验证材料 §3 只证官方 Python 版协议覆盖（hello/BinaryProtocol/provider/ASR 16 选/custom/mcp/alert/abort），未证官方服务端 WS 握手原生强制 Bearer 鉴权、更未证「未设即拒」比上游默认（常带 dev/validation 放行）更严的语义是否需自写 patch。
- 一句话：三面中「服务端强制鉴权承载」这一面未核实，「两个小补丁」未将鉴权强制列入，决策 22 的机械闸在服务端可能无 owner。

**T2-F7 · 轴1（canary 1，重新派生）：max_tokens 禁带约束散落正文、无 builder 可遵守的单一落点**
- 三处注记：spec:83、spec:542、spec:580（决策 18）「流式请求禁带 `max_tokens`（实测会触发 agent incomplete-turn 报错）」。
- 落点缺失：这是 `type:openai` 配置级接入 OpenClaw 的适配约束，但 interface-contract.md 与契约包 v2 的「§四 协议剖面」（hello/Opus/stt/tts/abort/mcp/鉴权/回执）**均无此约束**；契约包 §1 又把「OpenClaw 调用侧形状」判为 out-of-scope（CONTRACT-CHANGE-PACKET-v2:23）。
- 一句话：约束是祈使句但无单一事实源——网关 provider 配置（`type:openai`）的 builder 没有一条契约节命令他「禁带 max_tokens」，只能靠爬正文注记，违反可被 builder 遵守的硬约束要求。

**T2-F8 · 轴1 + S3：提醒 `speech` TTS 合成/下发载体未定义，构成对 edge-tts 的隐性同步依赖**
- spec:328/589「网关合成 TTS 随推送一并下发音频」+「alert 载荷带 `speech` 字段」。
- 缺口：mcp kidpal/alert 是 JSON-RPC（JSON 载荷），TTS 音频走下行 Opus/tts 消息——「随推送一并下发」的**时序/载体（合成先于推送？还是推送后紧跟 tts 流？）未裁**；若推送时实时合成，则提醒链路对 edge-tts 可用性/延迟有同步依赖（TTS 慢/被封 → 提醒三件套延迟或只剩弹窗）。
- S3 期望答案缺失：spec A2（spec:102）只说「可替换 provider、不作唯一依赖」，**无换 provider 触发条件、无 TTS 静默期降级响应**；「可替换」是声明不是可操作的降级。
- elicitation：是（TTS 被封时提醒是否仍弹窗+提示音、speech 音频合成失败是跳过还是阻塞提醒，需人类定）。

**T2-F9 · 轴1 + S2：多轮连续性「OpenClaw 自带」断言 vs 「部署期待验证」DEFER 自相矛盾，协议剖面无会话连续性承载**
- 「自带」侧：spec:70「跑完整 agent（多轮会话…全包）」、spec:86「砍掉…会话/意图层（DeepSeek+多轮对话自带）」、spec:104（A4）「多轮上下文 | OpenClaw 自带」，三者无 DEFER 标注。
- 「待验证」侧：spec:556「多轮会话连续性：🔵 待部署验证…无状态调用间保持连续…**部署期第一件事**」。
- 三面对齐缺口：契约包 §四 协议剖面只定义 hello 带 `session_id`，**未定义 `session_id` 如何维持 conversation/历史、历史由谁（网关状态 or 每轮透传）承载**；`type:openai` 配置级接入默认无状态，spec 依赖的正是在无状态调用间「传会话标识/历史」，但协议剖面没给这条。
- 一句话：A4 把多轮连续性写成「自带」（已具备）而 §6.3 写成「部署期第一件事」（未验证），同一能力两处相位相反；按 S2 期望答案，若走 DEFER 无 forcing-function、若走前置未冻结。
- elicitation：否（阶段相矛盾可实证），但「用 session_id 还是每轮传历史」方案本身需人类知识。

**T2-F12 · 轴1（charter D-5/E-2 断链点）：「新建日程后即时推」producer 未名**
- spec:291/588「P1 刷新三触发 = WS 连接建立 + 每日 0:00 跨天 + **新建日程后即时推**」。
- 缺口：cron（spec:325）是**定时扫** schedule.md，不产生「变更事件」；「新建日程 → 重算三档/刷新 P1」的**变更检测器（谁监听 schedule.md 写入并触发即时推）在 spec 内无名**，producer 断链（与 charter §4.1 D-5 / §4.3 E-2 同源留空）。

### NIT

**T2-F10 · 轴3：「shim」残留措辞与「网关」新旧混写**
- spec:545「### 6.2 设备↔**shim** 显示指令（E1）」；spec:202/204/240「经 shim /v1/asr /v1/tts」——方向 B 权威名应为「流式网关（开源小智服务端）」（spec:77），「shim」是薄桥四端点时代的措辞残留，非阻断但污染事实源命名。

**T2-F11 · 轴4（完整性）：§6.1 协议剖面消息表遗漏固件已处理的 `llm`/`system`（/`iot`）类型**
- spec:536-540 消息表与契约包 §四 均未列 `llm`（emotion 表情）、`system`（reboot 等命令）——固件 `application.cc:565-589` 明确处理这两类；spec:540「`listen`/`ping` **等**」以「等」带过。
- 一句话：协议剖面作为事实源若漏列，builder 无法判定这两类是保留（沿用原生）还是随 UI 重做移除，属表拳不全的 non-exhaustive。

## canary 重派生（独立，每则标注命中/漏）

1. **max_tokens（canary 1）→ 已重新找到**：三处祈使注记（spec:83/542/580）、契约与契约包 §四 均无落点、契约包把 OpenClaw 调用侧判 out-of-scope → T2-F7（SHOULD）。
4. **通道命名漂移（canary 4）→ 已重新找到**：alert 双形态（原生 vs mcp kidpal/alert）→ T2-F1（BLOCKING）；custom「弃用」vs「custom/mcp」并写 → T2-F2（SHOULD）。全部冲突行已逐条定位（spec:80/111/134/221/325/538/542/585/623）。
5. **v0.1 遗留（canary 5）→ 已重新找到**：faster-whisper / HTTP shim / 决策 4 未标记废弃 / VAD 切句位置漂移 → T2-F3（BLOCKING）＋ T2-F10（NIT）。
6. **spec↔契约漂移（canary 6）→ 已重新找到**：轮询/remind_before_min/mp3/custom「或」/control 缺 start_recording/无协议剖面节 → T2-F4（BLOCKING）。

（canary 2/3 属 T3 提醒投递状态机支，不在本支清单。）

## 场景走查（归属本支：S2 / S3）

- **S2 多轮上下文**：走完 → T2-F9。spec 把同一能力在两处写成相反相位（「自带」vs「部署期第一件事」），协议剖面无会话连续性承载定义；未达 charter 对 S2 的「冻结前前置 or DEFER+forcing-function」任一收口，A4 现为幽灵需求倾向。
- **S3 edge-tts 被封**：走完 → T2-F8。降级响应（换 provider 触发 + 静默期）缺失；提醒 speech 的 TTS 合成随推送实时发生，构成对 edge-tts 的隐性同步依赖，链路无该依赖的显式声明。

## Elicitation（需人类知识，非 gotcha）

1. **TTS 失效与提醒解耦**（S3/T2-F8）：edge-tts 被封或慢时，提醒三件套是否退化为「提示音 + 弹窗（无语音）」继续投递？speech 合成失败是跳过音频还是阻塞整个提醒推送？「随推送实时合成」的合成时刻在调度链上定位在哪一步（排程时预合成 vs 到点现合成）？
2. **多轮连续性承载方案**（S2/T2-F9）：`session_id` 维持 conversation 由网关侧保持状态，还是每轮把历史透传给 OpenClaw？流式探针只实证单轮，若要继续走 DEFER，forcing-function 是什么（否则 A4 悬空）？
3. **官方服务端强制鉴权现状**（T2-F6）：官方 Python 版小智服务端 WS 握手是否原生「token 未设 = 拒绝」，还是需自写补丁实现更严语义？「两个小补丁」是否要扩为三个（+ 鉴权强制）？
4. **新建日程即时推的 producer**（T2-F12）：谁监听 schedule.md 变更？（家长改 MD → 何时/如何触发 P1 刷新 + 三档重算，OpenClaw 文件 watcher / cron 短轮询 / 家长端显式回调？）
