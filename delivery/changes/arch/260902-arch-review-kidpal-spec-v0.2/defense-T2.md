# Defense — T2 传输方向 B 三面对齐（defender 答辩）

> defender 轮产出（2026-09-02）。主线程代落盘，内容保真。对 `findings-T2.md` 逐条答辩。

【T2-F1 · 轴4 · canary 4 · BLOCKING】**CONCEDE**

依据：spec:134（E3）「推送 `alert` 消息（**小智协议原生类型，固件已有渲染处理**）」+ spec:111（B3）「推送 `alert` 消息」+ spec:80「提醒走 alert 消息」+ spec:221「stt/tts/custom/**alert** 消息」为一方；spec:538（§6.1 mcp 行）「小智原生 **alert/custom 通道弃用**（2026-09-02 grilling 裁定）」+ spec:585（决策 23）「小智原生 alert/custom 通道弃用」+ spec:325（S-F1.4）「经 WS 推送（`mcp` kidpal/alert 工具）」为另一方。两组**字面不可调和**——「原生类型 + 已有渲染处理」与「原生弃用、走 mcp」是同一「提醒推送」的两种互斥通道定义，无统一解读空间。代码佐证成立：`application.cc:590-598` 的 `Alert(status, message, emotion, OGG_VIBRATION)` 只吃三字段 + 震动音，与 kidpal/alert 载荷 `{id, fire_at, title, speech, view}`（CONTRACT-CHANGE-PACKET-v2:31-34）形状不符——即便不退弃原生 alert，「固件已有渲染处理」对 S-F1.4 的 state-reminder 弹窗（排队 + 渲染即确认 + speech TTS，spec:326-328）仍为假，是新固件渲染面。BLOCKING 成立：核心载体双形态 + 假断言会误导 builder 严重低估固件量。

改法方向：spec:134/111/80/221 统一对齐决策 23——删「原生类型、已有渲染处理」，写「经 mcp kidpal/alert 工具推送」并把 state-reminder 弹窗明标为新固件面。**属文本清理，方向已由决策 23 钉死，不需重新 grilling。**

---

【T2-F3 · 轴4 · canary 5 · BLOCKING】**CONCEDE**

依据：spec:202「faster-whisper，经 shim `/v1/asr`」+ spec:204「edge-tts，经 shim `/v1/tts`」+ spec:217「公网 HTTP/WebSocket」+ spec:232「ASR: faster-whisper」+ spec:240「经 shim /v1/asr（faster-whisper）→ /v1/chat → /v1/tts」+ spec:566（决策 4，未标废弃）「薄桥 shim……ASR/TTS HTTP 口子……提醒轮询」为一方 v0.1 叙事；spec:79「ASR（阿里云云 ASR，云优先）」+ spec:101（A1）「不用 faster-whisper」+ spec:581（决策 19）+ spec:90「Pod S HTTP 台阶方向 B 落地后退役」为另一方。VAD 漂移成立：spec:239「本地 VAD 检测说话结束」vs spec:78/535「服务端 VAD 切句后送云 ASR」。§3.2–§3.4 整段与 §3.0.0/§6.1/决策 9/19 互斥，是头号内部矛盾。

改法方向（文本清理口径）：§3.2/§3.3/§3.4 按方向 B 重写（ASR=云优先、删 shim 四端点、传输=WS+Opus），决策 4/6/8 标「随方向 B 废止」;VAD 切句统一为「**服务端** VAD 切句送云 ASR」，本地 VAD 专职收窄为「喊停/插话检测」（spec:198/265/467 已述），纠正 spec:239 的「本地 VAD 检测说话结束触发送 ASR」——该句是 v0.1 遗留，方向 B 下切句在服务端。**纯文本清理，非方向分歧。**

---

【T2-F4 · 轴4 · canary 6 · BLOCKING】**CONCEDE（漂移 + 铁律成立）｜附处置澄清（非死锁、已路由）**

六条漂移点全部核验成立：contract:218「轮询 /v1/alerts → 读到清除」vs spec:325/582 推送 + 渲染即确认；contract:164/189/194 `remind_before_min` vs spec:320/582 三档阶梯废止（spec:319 自称「B1 零变更」与实况不符）；contract:26「→ mp3」vs spec:537 下行 Opus 帧，且契约自相矛盾（contract:140「tts + Opus」vs :26「mp3」）；contract:141「mcp 或自定义消息」vs spec:585 custom 弃用；contract:131-134 control 缺 `start_recording` vs spec:363/587；契约无协议剖面节而 spec:531 §6.1 引其为事实源。铁律出处：DEV-GOVERNANCE §4 mermaid「契约已批 → spec 方可冻结」+ 行 94「铁律：契约未批 → spec 不可冻结」+ charter §5 canary 6（行 128）。**漂移与铁律均真，spec 现确不可冻结。**

处置澄清（REBUT 面，非漂移本身）：此漂移是「已路由的前置条件」，非本 review 需自行解除的死锁——
- charter §1 行 17 已把「契约改法本身」排除出本 charter，只要求定位漂移 + 严重度;
- CDQ-260902 已在办（spec:623；契约包头行 5「Status: PRECHECKED」）;
- DEV-GOVERNANCE §4 原子落地（同一 PR）+「契约已批 → spec 方可冻结」保证 spec 不可能先于契约门签字冻结。

→ 本 review 收口即转 ① 契约门，时序上不构成死锁。正确裁决应为**条件冻结**：「spec 冻结以 CDQ-260902 契约门人签字为前置」，而非要求本 review 自行改契约。

改法方向（落契约门，非本 review）：按 CONTRACT-CHANGE-PACKET-v2 §2 拟 diff（§三重写推送 + 新增 §四协议剖面 + §1.5 收敛 + control 扩 start_recording + B1 `remind_before_min` 漂移清理）human 签字后收敛，并在 spec 决策 20 同步修正「B1 零变更」措辞。

---

【T2-F2 · 轴3 · SHOULD】**CONCEDE**

依据：spec:585（决策 23）「custom 弃用」vs spec:80「display JSON 走 custom/mcp」、spec:542「经 `custom`/`mcp`」、spec:623「display 经 custom/mcp」。三处并写残留，builder 读到三种通道结论。

改法：三处「custom/mcp」→「mcp」，与 T2-F1 同批清理。

---

【T2-F5 · 轴4 · SHOULD】**CONCEDE**

依据：spec:554/584（决策 22）「CA 嵌入固件」vs 固件 `esp_ssl.cc:34` `esp_tls_cfg_t cfg = {}` + :35 仅 `cfg.crt_bundle_attach = esp_crt_bundle_attach`（公共 CA bundle，无法校验自签长期证书），无 `cacert_buf`/`cert_pem`/`clientcert` 任何自签 CA 加载路径;`web_socket.cc:126-127`「wss → CreateSsl」仅确认 TLS 可用。验证材料 §2「无硬阻塞」范围=协议栈，未触及 TLS。属决策 22 的完整性/实现含义缺口（合 charter §7 P3 边界，不攻击自签取舍）。

改法：把「自签 CA 加载路径（esp_tls `cacert_buf`/`cert_pem` pinning）」列为固件 work item，与 spec:91「两补丁」并列；并修止 validation §2「无硬阻塞」措辞为「协议栈无硬阻塞，TLS 证书加载为新增面」。

---

【T2-F6 · 轴4 · SHOULD】**CONCEDE**

依据：spec:552/584（决策 22）「token 未设 = 拒绝服务」;spec:91 补丁清单只列「结构化下发 push helper、提醒定时推送」两件、不含鉴权强制。validation §3 只证协议覆盖，未证官方 Python 服务端 WS 握手原生 Bearer 强制、更未证「未设即拒」严于上游 dev/validation 缺省是否需自写 patch。三面中「服务端强制鉴权承载」未核实，机械闸在服务端可能无 owner。

改法：核实官方服务端握手鉴权现状，据此把「鉴权强制补丁」补入 spec:91 清单（候选「两补丁→三补丁」）。注意：契约包 §4 行 57 的 shim.py `require_device`「沿用」是 v0.1 HTTP shim 纪律，方向 B 下 shim 退役（spec:90）、新服务端是官方小智服务端——原生承载未证正是本 finding 落点，不因此 REBUT。

---

【T2-F7 · 轴1 · SHOULD】**CONCEDE**

依据：spec:83/542/580（决策 18）三处「流式请求禁带 max_tokens」散落正文;interface-contract.md 与契约包 §四均无落点;契约包 §1 line 23 判「OpenClaw /v1/chat/completions 调用侧形状」out-of-scope。该约束是 provider 配置级接入（type:openai）的适配硬约束（validation §1 探针 1 实测触发 agent incomplete-turn 报错），却无 builder 可遵守的单一事实源。

改法：落进（a）契约 §四协议剖面备注 或（b）服务端 provider 配置纪律文档，一处钉死。

---

【T2-F8 · 轴1 + S3 · SHOULD】**CONCEDE（含 elicitation，需 human）**

依据：spec:328/589「网关合成 TTS 随推送一并下发音频」+「alert 载荷带 speech」;但 mcp kidpal/alert 是 JSON-RPC，TTS 音频走下行 Opus/tts——合成时序/载体未裁。charter S3（行 60-63）要求「TTS 失效降级 + 检查隐性同步依赖」，而 spec:102（A2）只写「可替换 provider、不作唯一依赖」，无换 provider 触发、无静默期降级。「可替换」是声明不是可操作降级。

改法（部分人裁）：声明 speech 合成时刻（建议 cron 组三档时预合成存缓存、到点随 push 下发，消到点实时合成的同步依赖）;声明降级（speech 合成失败 → 提示音 + 弹窗仍投递、speech 跳过不阻塞）。→ 人知识 elicitation 1。

---

【T2-F9 · 轴1 + S2 · SHOULD】**CONCEDE（核心承载缺口成立）｜部分 REBUT（「相位相反」定性偏强）**

REBUT 面：spec:104（A4）「OpenClaw 自带」指大脑固有多轮能力（spec:70 全包）;spec:556「待部署验证」指「无状态网关 relay 间连续性」这一集成风险，且 spec:556 **显式回引 A4 并标依赖**（「spec A4『连续追问』依赖此项，部署期第一件事」）——二者是「能力内在 vs 集成连续」两件相邻事，spec 已自我标注依赖，非「未觉矛盾的同名相位相反」。

CONCEDE 面：charter S2（行 55-58）要求「冻结前前置 OR DEFER + forcing-function + owner + 相位」，§6.3 给相位（「部署期第一件事」）但缺 owner、forcing-function 弱;实质性缺口 = 协议剖面（契约包 §四）只定义 hello 带 `session_id`，未定义 session_id 如何维持 conversation/历史（网关状态 or 每轮透传）——type:openai 配置级接入默认无状态，spec 依赖的「传会话标识/历史」在剖面无承载定义。

改法：协议剖面补「会话连续性承载」;给 S2 一个 forcing-function（如「部署首验 = 两轮追问连续性探针，不连续则网关适配传历史」）。→ 人知识 elicitation 2。

---

【T2-F10 · 轴3 · NIT】**CONCEDE**

依据：spec:545「设备↔shim」、spec:202/204/240「经 shim /v1/asr /v1/tts」vs 方向 B 权威名「流式网关（开源小智服务端）」（spec:77）。「shim」是薄桥四端点时代残留。改法：spec:545「设备↔网关」，其余随 T2-F3 重写收敛。

---

【T2-F11 · 轴4 · NIT】**CONCEDE**

依据：`application.cc:565-571` 处理 `llm`（emotion→SetEmotion）、:577-589 处理 `system`（reboot）;spec:536-540 消息表与契约包 §四均未列 llm/system，spec:540「listen/ping 等」以「等」带过。剖面作为「格式事实源」（packet §2 line 39）漏列，builder 无法判定保留 or 移除。

改法：剖面补列 llm/system（标既有/弃用结论），去 spec:540「等」字。

---

【T2-F12 · 轴1 · SHOULD】**CONCEDE**

依据：spec:291/588「刷新三触发……新建日程后即时推」;spec:325 cron 是定时扫 schedule.md（validation §7 第三轮「cron --message 定时触发」），不产生变更事件;「新建日程 → 重算三档/刷新 P1」的变更检测器无名。charter §4.1 D-5/§4.3 E-2 已同源留空。

改法：具名 producer，三选一（人知识）：(a) OpenClaw 文件 watcher 监听 schedule.md;(b) cron 短轮询（「即时推」降级 ≤N 分钟）;(c) 家长录入通道本身触发重算回调。→ 人知识 elicitation 4。

---

## 摘要

**逐条计数**：CONCEDE ×12（含 2 条带窄 REBUT 限定），无纯 REBUT。

- 纯 CONCEDE：T2-F1/F2/F3/F5/F6/F7/F8/F10/F11/F12
- CONCEDE 附 REBUT 限定：**T2-F4**（漂移+铁律真，但非本 review 死锁——已路由 CDQ-260902、charter §1 排除、DEV-GOVERNANCE §4 原子落地，应为「条件冻结」）;**T2-F9**（「相位相反」定性偏强，spec:556 已显式回引 A4 依赖;实质缺口在协议剖面无会话连续性承载）。

**三条 BLOCKING 均 CONCEDE**：方向 B 的 spec 现不宜冻结——但 F1/F3 属「文本清理、方向已裁（决策 19/23 钉死）、不需重新 grilling」;F4 属「已路由前置条件、非独立死锁」。可冻结性的头号矛盾真实存在，challenger 定位精准（canary 4/4 独立重派生、引文逐条核验无误）。

**deadlock 候选（正反有据，交拍板）**：
- T2-F4 严重度/处置——challenger「spec 冻结即命中铁律（BLOCKING）」vs defender「已路由到在办契约门，应裁决『条件冻结』而非本 review 独立阻塞」。
- T2-F9 定性——challenger「相位相反内部矛盾」vs defender「spec:556 显式回引、非未觉矛盾;真正缺口是承载定义」。

**top 论点**：
1. 轴 11/自动化面无争点——challenger 本轮未要求加任何逐条人工门，蓝图 §5 关卡纪律未被挑战，defender 无需替自动化辩（与轴 10 的对立未在本支出现）。
2. 四条 SHOULD 的改法需人类知识，建议 moderator 汇总上交、不与辩论混淆：F8（speech TTS 时序/降级）、F9（连续性承载方案）、F6（鉴权承载现状）、F12（即时推 producer）——对应 findings elicitation 1/2/3/4。
3. 无过度删除诉求：challenger 未要求删任何契约 enum/字段（除 B1 `remind_before_min` 本就被 spec:174 自行标注「已废止待契约门裁」），护契约面/护既有决策轴未被触发。
