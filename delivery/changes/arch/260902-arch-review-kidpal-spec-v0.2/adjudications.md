# Adjudications — 260902-arch-review-kidpal-spec-v0.2（中立判官逐条裁定）

> 裁定方：adjudicator（中立判官 + 收口，不改 spec/契约）。判据 = `review-charter.md`（冻结，只读）。
> 裁定时点：2026-09-02。输入 = challenger findings-T1..T5（46 条）+ defender defense-T1..T5（五支齐）+ `docs/spec.md` v0.2 + `docs/contracts/interface-contract.md`（只读）。
>
> 过程注：`defense-T2.md` / `defense-T5.md` 在判官首轮裁定中途补齐落盘（23:15/23:17），故 T2/T5 首轮裁定先按 challenger 依据 + 独立 spec/contract 核验出具，随后已据 defender 实际答辩复核——T2 全 CONCEDE（与首轮一致），**T5-F1 有实据 REBUT（SSOT 方向论），据此降级**（见 V33）。最终裁定以 defender 已答辩版本为准。

## 裁定结论总览

- 源 finding 46 条 → 裁定归并后 **38 条独立条目**（35 CONFIRMED + 3 DISMISSED）。
- BLOCKING：源 7 条 → 归并 **5 条**（T2-F3×T5-F2 合一；T5-F1 的 BLOCKING 定性被 defender「SSOT 方向论」驳回、残余降 SHOULD），全部 CONFIRMED。
- SHOULD：源 26 条 → 归并 **26 条**（多组横向合并；含 T5-F1 残余）。
- NIT：源 4 条 → **4 条**。FYI：源 1 条（T1-F7）→ DISMISSED。
- DISMISSED：3 条（T1-F2 / T1-F7 / T1-F8-AFE 子项），外加 T5-F1 的 BLOCKING 定性（残余转 SHOULD）。
- DEADLOCK：0 条真僵局；拍板项 = 11 组（见 review-packet 拍板清单）。
- 处置归属分布（CONFIRMED 35 条，按主归属）：spec-text 19 · contract-gate 1 · human（拍板/elicitation）11 · dev-handoff 3；部分条目带次归属（contract-gate 字段锁定 / dev-handoff 侧）。

---

## 过程注：跨支去重方法

按任务指定去重组逐一对过，重叠项裁一条**主条目** + 关联引用，不双计：

- T1-F4×T2-F3：「VAD/句边界」主条目 = T1-F4（轴 0 实时句边界 owner）；T2-F3 的「VAD 切句位置漂移」为关联引用。
- T5-F2×T2-F3：「v0.1 残留」主条目 = T2-F3（§3.2–§3.4 + 决策 4/6 漂移行枚举），T5-F2 为同一缺陷的「冻结阻塞结论」面，合一。
- T1-F5×T2-F12×T5-F7：「即时推 producer / 家长通道」主条目 = 合并 F-PARENT（家长录入通道未具名 + producer 断链），三条 sighting 合一。
- T1-F8×T5-F6：「功耗 DEFER」主条目 = T5-F6；T1-F8 的功耗子项转并，AFE 子项 DISMISSED。
- T4-F1×T5-F5：「证书生命周期」主条目 = T4-F1，T5-F5 合一。
- T3-F7×T2-F1/F2：「通道命名」T3-F7 子项(3) 转并 T2-F1；T2-F2(custom) 独立保留（与 alert 是不同通道）。
- 追加判定的两组合并（任务未点名但显然同源，防双计）：T2-F8×T3-F6（提醒 TTS 同步依赖/S3）、T2-F9×T5-F3（S2 多轮连续性）、T4-F3×T5-F8（display JSON 无 schema 校验）、T4-F2×T4-F7（token 强度，F7 折入 F2）。

---

## A. CONFIRMED（35 条，按主题节）

### A1 内部矛盾 / v0.1 残留（falsifiable，最高置信）

**V01 · v0.1 遗留面（§3.2–§3.4 + 决策 4/6）live 互斥，两个构置目标并存** — BLOCKING — spec-text
- 源：T2-F3 × T5-F2（合并）。
- 依据：`spec.md:202`「ASR…faster-whisper，经 shim /v1/asr」、`:204`「TTS…经 shim /v1/tts」、`:217`「公网 HTTP/WebSocket」、`:232`「ASR: faster-whisper」、`:240`「shim /v1/asr→/v1/chat→/v1/tts」、决策 4 `:566`「薄桥 shim（FastAPI…提醒轮询）」vs 决策 9/:19 `:571/:581`「方向 B、faster-whisper 弃选、云 ASR」、决策 18 `:580`「Python 小智服务端」。
- 判官独立核验：漂移属实。§3.2–§3.4 整段仍是 v0.1 同步 HTTP 叙事，与 §3.0.0/§6.1/决策 9/18/19 硬互斥；决策 4/6 仍在「已定 ✅」区与 9/18/19 并列且无废弃标注。builder 读 §3.4 得「建 FastAPI 四端点+faster-whisper」、读 §6.1 得「部署小智服务端+mcp push」。canary 5 头号内部矛盾坐实。
- 处置：spec-text（§3.2–§3.4 + 决策 4/6 标废弃或改写为方向 B；两处里程碑 M1/M2 同批重切，见 V33）。

**V02 · VAD 句边界 owner 漂移（本地 push-on-pause vs 服务端切句）** — SHOULD — spec-text
- 源：T1-F4（主）+ T2-F3 VAD 子项（关联）。
- 依据：`spec.md:239/:458-459`（§3.4/§5.2「本地 VAD 检测停顿→才送」）vs `:78/:535`（§3.0.0/§6.1「服务端 VAD 切句」）。
- 判官核验：互斥属实。若按 §5.2 停顿才送，网关无从切句。defender T1-F4 CONCEDE，改法方向（板端本地 VAD 只保留喊停/插话，句边界归服务端）与 spec 既有 `:198/:200/:265` 一致。
- 处置：spec-text（收敛 §3.4:239 与 §5.2:458 的 push-on-pause 叙述）。

**V03 · 板端存储三处互斥（SD 缓存 vs「不存数据」红线）** — BLOCKING — spec-text
- 源：T1-F1。defender CONCEDE（认可 BLOCKING）。
- 依据：`spec.md:32`「本地缓存录音、字库、课表」+ `:215`「RTC 提醒 + SD 缓存」vs `:57`「❌ 不存数据」+ `:586` 决策 24 + `:141` D3「已下载到服务器」+ `:205`「存储｜云端」。
- 判官核验：三方互斥属实，直接落「哑终端不存数据」红线（charter §6.4）。challenger 代码佐证（目标板仅 NVS 存 url/token/version、SPIFFS 存 1bpp 资产、无 SD 缓存消费方）成立——§2:32 是 v0.1 遗留叙述。
- 处置：spec-text（擦除 `:32`「本地缓存…」改记硬件事实、`:215`「SD 缓存」改「SPIFFS 1bpp 美术资产」）。属已裁边界（决策 24）的旧文清理，非新对社会拍板。

**V04 · 「不存数据」红线无 RAM-only 可强制不变量** — SHOULD — spec-text
- 源：T1-F3（吸收 T1-F2 的生命周期/介质缺口）。defender F3 CONCEDE。
- 依据：`spec.md:57/:586` 只写意图，`:582`「易失 id 集合」、`:201` 会话状态机、`:363` 录音缓冲靠「易失」二字隐含前提，无一句禁止将状态持久化到 `:24` 明列的 16MB Flash。
- 判官核验：红线当前不可证伪属实。defender 改法（非易失介质仅允许 凭证+1bpp 资产+固件本体；业务/身份状态只许 RAM）一句成本、可机判。
- 处置：spec-text（加 RAM-only 介质禁令一句）。

**V05 · 提醒载体 alert 双形态（原生 alert vs mcp kidpal/alert）+「固件已有渲染处理」为假** — BLOCKING — spec-text
- 源：T2-F1（主）+ T3-F7 子项(3)（关联）。
- 依据：`spec.md:134` E3「小智协议原生类型，固件已有渲染处理」、`:111`、`:80`、`:221`（原生 alert）vs `:538/:585`「原生 alert/custom 弃用、统一 mcp kidpal/alert」、`:325`。
- 判官核验：内部矛盾属实（canary 4）。固件佐证成立——`application.cc:590-598` 原生 alert 处理器吃 `status/message/emotion`+OGG_VIBRATION，非 S-F1.4 的 `state-reminder` 弹窗，故「固件已有渲染处理」不成立，S-F1.4 弹窗是全新固件面。
- 处置：spec-text（统一为 mcp kidpal/alert 按决策 23；擦除 :134/:111/:221/:80 原生 alert 措辞）。通道命名矛盾由已裁决策 23 解决，非新拍板。

**V06 · custom 通道「弃用」与「custom/mcp」并写残留** — SHOULD — spec-text
- 源：T2-F2。
- 依据：`spec.md:585`「custom 弃用」vs `:542/:623/:80`「经 custom/mcp」。
- 判官核验：三种通道结论并存属实（canary 4 附支）。与 V05 同源的通道命名统一，此处仅 custom。
- 处置：spec-text（「custom/mcp」统一为 mcp），随 V05 同批。

**V07 · §3.0.1 B3 单行三处 v0.1 残留** — SHOULD — spec-text
- 源：T3-F7（子项 1/2；子项 3 转并 V05）。defender CONCEDE。
- 依据：`spec.md:111`「事前 30 分钟」vs `:582`「事前 15 分钟（human 修订值）」；「确认即清」vs `:326`「渲染即确认、即出队」；「推送 alert 消息」vs `:585/:538`（转 V05）。
- 判官核验：单行三处核实属实。与 V05/V01 同源的 v0.1 文本残留。
- 处置：spec-text（:111 三处值分别对齐决策 20/26/23）。

**V08 · spec↔契约硬漂移（铁律命中）** — BLOCKING — contract-gate（契约门四题）
- 源：T2-F4（主）+ T4/T5 canary-6 关联。
- 依据：`interface-contract.md:218`「设备轮询 /v1/alerts → 读到并清除」vs spec 推送+渲染即确认；`:164/:189/:194` `remind_before_min` vs spec 三档阶梯废止；`:26`「mp3→设备」vs `spec:537` 下行 Opus；`:141`「mcp 或自定义消息」vs custom 弃用；`:131-134` control 缺 `start_recording`；契约**无** hello/Opus/stt/tts/abort/鉴权/回执协议剖面节。
- 判官核验：漂移全部属实（canary 6）。契约现有文本与 spec 新模型硬冲突；按 protocol「契约未批前 spec 不可冻结」铁律 + DEV-GOVERNANCE §4 契约门，此冲突锁死 hard-freeze——但可**条件冻结**（spec 冻结以契约门四题 human 批准为前置，同 PR 原子落地）。
- 处置：contract-gate。CDQ-260902 四题（批准范围 / Opus+WS / 强制鉴权+渲染回执+speech / remind_before_min 删/标废止）转由 human 裁，见 review-packet 拍板清单第 1 项。

### A2 协议 / 约束落点（falsifiable）

**V09 · max_tokens 禁带约束散落正文、无 builder 可遵守的单一落点** — SHOULD — spec-text（+contract-gate 注）
- 源：T2-F7。
- 依据：`spec.md:83/:542/:580` 三处 prose 注记；契约与契约包 §四 均无此约束；契约包 §1 把「OpenClaw 调用侧形状」判 out-of-scope（CONTRACT-CHANGE-PACKET-v2:23）。
- 判官核验：约束是祈使句但无单一事实源属实（canary 1）。实测坑（`stream:true` 禁带 `max_tokens`）确为 type:openai 配置级接入硬约束。
- 处置：spec-text（把三处散落注记收拢为 §6.1/决策 18 一条硬约束）；次归属 = contract-gate（契约门 §四 协议剖面重写时纳入此配置约束落点）。

**V10 · 「CA 嵌入固件」与固件 SSL 层实况不符（无证书加载实现面）** — SHOULD — dev-handoff
- 源：T2-F5。
- 依据：`spec.md:554/584`「CA 嵌入固件」vs 固件 `esp_ssl.cc:34` `esp_tls_cfg_t cfg = {}` 空配置、`web_socket.cc:126-127` 无证书校验/pinning 落点。
- 判官核验：属 P3 已接受取舍的**实现含义/完整性**缺口（非攻自签证书本身）。「嵌入 CA」无对应固件代码面，需新增证书加载实现。
- 处置：dev-handoff（固件 SSL 层证书加载/pinning 实现），随方向 B 落地包；未在 spec 层解决。

**V11 · 强制鉴权「token 未设 = 拒」在官方服务端承载未验证** — SHOULD — dev-handoff
- 源：T2-F6。
- 依据：`spec.md:552/584` 强制鉴权 vs `:91`「两个小补丁」不含鉴权强制；验证材料 §3 未证官方 Python 版原生强制 Bearer 鉴权或需自写 patch。
- 判官核验：决策 22 的机械闸在服务端可能无 owner 属实。方向 B 服务端承载未核实。
- 处置：dev-handoff（服务端强制鉴权验证 + 可能的第 3 个补丁），随方向 B 落地包。

**V12 · §6.1 协议消息表漏列 `llm`/`system` 类型** — NIT — spec-text
- 源：T2-F11。
- 依据：`spec.md:536-540` 消息表 + 契约包 §四 未列 `llm`（emotion）/`system`（reboot）；固件 `application.cc:565-589` 明确处理；`:540`「listen/ping 等」以「等」带过。
- 处置：spec-text（枚举补齐或显式标注「沿用原生」），non-blocking。

**V13 · 「shim」残留措辞 vs「网关」新旧混写** — NIT — spec-text
- 源：T2-F10。
- 依据：`spec.md:545`「设备↔shim 显示指令」、`:202/:204/:240`「经 shim」vs 方向 B 权威名「流式网关」（`:77`）。
- 处置：spec-text，随 V01 的 §3.2–§3.4 清洁同批收敛，non-blocking。

### A3 提醒投递状态机（轴 6/9/11）

**V14 · 幂等去重/回执在「已渲染 vs 排队未渲染」边界矛盾** — BLOCKING — human（拍板修法口径）
- 源：T3-F2。defender CONCEDE（真实矛盾）；分歧仅在修法口径。
- 依据：`spec.md:326`「重推的同 id 直接回回执不再排队（幂等，重复弹窗根除）」vs `:327`「队列中未渲染的条目待渲染后才回回执」。
- 判官核验（焦点裁定）：两规约不可调和坐实。若幂等集「渲染完成才入」→ 待渲染条目 60s 重推窗口内不在集 → 重推再入队 → 重复弹窗（违背「根除」）；若「接收即入集」→ 与 327 撞（接收即回执=把「渲染即确认」变「接收即确认」）。触发 = S4(a)+(c) 07:45 N 条目共享墙钟 + 驻留到按键 + 60s 重推，是必然叠加态非边角。
- 修法口径裁定（给 human，建议即地板）：defender 解耦方案成立且**守住 P4「渲染即确认」**——(a) 幂等去重 = **接收即入集**（入队前记入易失 known-id，作用仅「屏蔽重复入队」，与渲染无关）；(b) 回执出队 = **渲染即发**（不动 P4）。「同 id 重推」两态分别处置：已渲染→补回执（根除重弹）；未渲染（排队中）→去重不重复入队、不回执、等渲染后回。可规范化为 received/queued/rendered/acked 四态。spec:326 该句需改写为两态版。
- 判官推荐 = 采纳解耦口径；但「幂等集是否含已接收未渲染」本身是产品语义裁定点，须 human 拍板。不改「接收即确认」（守住已裁取舍）。
- 处置：human（拍板解耦口径）+ spec-text（改 :326）。

**V15 · 幂等 id 生成 owner + 跨重推稳定性未定义** — SHOULD — spec-text（+contract-gate 注）
- 源：T3-F3。defender CONCEDE。
- 依据：`spec.md:326` 预设「同 id 重推」跨重推不变，但谁生成 id、cron 重算覆盖写 pending.json 是否重新生成 id 均无字；契约包 :31 仅 `"id":"<reminder-instance-id>"` 占位。
- 判官核验：不变量所依赖的键自身无定义属实。
- 处置：spec-text（补「幂等 id = reminder instance 稳定标识，cron 首次生成后跨重推/重算不变」）；次归属 = contract-gate（契约门 §三 需锁定 id 字段级事实，CDQ-260902）。

**V16 · 排程链两处 handoff 未名（三档即时重算 + pending.json→网关传递）** — SHOULD — spec-text
- 源：T3-F1。defender CONCEDE（边界精确化：owner 有部分字，缺的是「即时重算三档」与「pending.json 跨进程传递机制」）。
- 依据：`spec.md:588` 决策 26「即时推」只覆盖 P1 刷新不覆盖三档重算；`:325` cron 写 pending.json vs 网关到点推送，两进程传递机制缺；`:567` 决策 5 只证「同机」未证「机制」。
- 判官核验：属实。S6 关联——预排 vs 惰性重算未定 → 离线一周重连的「过期静默作废」评估时点不可判。
- 处置：spec-text（补 cron 扫频 + pending.json 传递通道 + 三档重算粒度）。

**V17 · 「带物」唯一承载面无持久/回读（P1/P2 无备注回显）** — SHOULD — human（elicitation）+ spec-text
- 源：T3-F4。defender CONCEDE（边界纠正：带物持久 source 是备注列、未真蒸发；但 P1/P2 无备注回显 + S-F1.3 未明确备注参与 = 真实缺口）。
- 依据：`spec.md:320-321` 20:30 档 = 带物唯一承载档；`:294/:303` P1/P2 不回显备注；`:313` S-F1.3 未明确备注参与答复；孩子错过 20:30 档（渲染即出队、不重推）后无补偿可见面。
- 判官核验：采纳 defender 边界纠正后的缺口成立，与「不丢三落四」目标（:317）有张力且无缓解声明。
- 处置：human（elicitation：接受带物仅靠孩子当场看到，还是备注并入 P1/P2 视图/语音读回）+ spec-text（S-F1.3 明确备注参与答复，可选 P1 带物项星标呼应）。

**V18 · 驻留无超时 + 已渲染永未按键的滞留弹窗 失效响应缺失** — SHOULD — spec-text
- 源：T3-F5。defender CONCEDE（部分，严重度下缘：不硬降 NIT；队列深度天然有界=当天事项数，「无界」过强已纠正；真缺口 = 已渲染永未按键的弹窗滞留越过事件本身 + 阻塞队列后续 → 与 F2 联动放大）。
- 依据：`spec.md:327` 无超时驻留、`:326`「过期静默作废」仅覆盖未送达。
- 判官核验：滞留弹窗「驻留到按键」永不过期、且「不合并、不替换」会阻塞队列后续（饿死→重推→作废）属实。
- 处置：spec-text（补「event 过后已渲染弹窗允许顶替/降级显示，非追加送达」+ 队列积压极端行为声明）；不改「驻留到按键」取舍（不自动强迫关闭）。

**V19 · 「+ 标记送达」残留词（被推翻的「推送成功即送达」）** — NIT — spec-text
- 源：T3-F8。defender CONCEDE。
- 依据：`spec.md:325`「+ 标记送达」vs `:326/:538`「送达确认 = 渲染回执」。
- 处置：spec-text（:325 改「标记已推送（重试计数用）」/删），non-blocking。

### A4 相位 / DEFER / 完整性（轴 2/7/8）

**V20 · 提醒 speech TTS 合成/下发载体未定义，对 edge-tts 隐性同步依赖（S3）** — SHOULD — human（elicitation）
- 源：T2-F8 × T3-F6（合并）。T3 defender F6 CONCEDE。
- 依据：`spec.md:328/:589`「网关合成 TTS 随推送一并下发」vs mcp kidpal/alert 是 JSON-RPC、TTS 音频走下行 Opus/tts——时序/载体未裁；若推送时实时合成 → 提醒链路对 edge-tts 可用性/延迟同步依赖；`spec:102` A2「可替换 provider」无换 provider 触发、无静默期降级。
- 判官核验：S3 期望答案（降级响应 + 隐性依赖显式声明）缺失属实。改法方向 = 声明「弹窗+提示音必达、speech 为可选增强，合成失败/超时降级仅弹窗+提示音」，纯技术降级、不引人工门。
- 处置：human（elicitation：TTS 失败时提醒是否仍弹窗+提示音、speech 合成失败跳过还是阻塞、合成时刻=排程预合成 vs 到点现合成）+ spec-text。

**V21 · 多轮连续性「自带」vs「部署待验证」相位矛盾 + 协议剖面无会话连续性承载（S2）** — SHOULD — human（elicitation）
- 源：T2-F9 × T5-F3（合并）。T2 defender F9 CONCEDE（附窄 REBUT：「相位相反」定性偏强——`spec:556` 已显式回引 A4「spec A4『连续追问』依赖此项」，是「能力内在自带 vs 集成连续待验」两件相邻事非未觉矛盾）；T5 defender F3 CONCEDE-TO-HUMAN。
- 依据：`spec.md:70/:86/:104` A4「OpenClaw 自带」无 DEFER 标注 vs `:556`「多轮会话连续性：🔵 待部署验证…部署期第一件事」；契约包 §四 只定义 hello 带 session_id、未定义 conversation/历史承载。
- 判官核验：采纳 defender 窄化——spec:556 已自标依赖，非「未觉矛盾」，故不以「相位相反」定性；实质缺口 = ① 协议剖面无会话连续性承载（session_id 维持 conversation 由网关侧状态 vs 每轮透传历史未定义）② 缺具名 owner + 「连续」判据（几轮/上下文窗口）+ 降级路径「传会话标识/历史」的量级估算。A4/S-F3.3「那『星』字呢」指代依赖此项，验证失败则成幽灵需求。
- 处置：human（elicitation：OpenClaw type=openai 是否已自行维持会话历史，还是网关须传 session_id/历史）+ spec-text（S2 补 forcing-function「部署首验=两轮追问连续性探针」+ 协议剖面补会话连续性承载）。

**V22 · 家长录入通道未具名 + 「新建日程后即时推」producer 断链** — SHOULD — human（拍板家长端形态）+ spec-text
- 源：T1-F5 × T2-F12 × T5-F7（合并，主条目 F-PARENT）。T1 defender F5 CONCEDE。
- 依据：`spec.md:589/:291` 三触发中「新建日程后即时推」producer 无名；`:582/:178` cron 只覆盖提醒排程；`:343` 家长「直接改 MD」最缺 watcher；`S-F1.5:342`「约定通道」不存在（QQ/小程序/改 MD 三选一悬置，AC 不可证伪）。
- 判官核验：producer 断链 + 通道未具名双属实。charter §4.1 D-5 / §4.3 E-2 已预列。两者耦合：producer 确定是「经何通道录入」的函数。
- 处置：human（拍板家长端形态三选一 + 顺带定 producer：OpenClaw 变更事件 / 网关 FS watcher / 显式承认直改 MD 不即时推靠 WS连接+0:00 兜底）+ spec-text。

**V23 · ASR provider 终选「部署期核实」无 forcing-function，兜底不覆盖失败模式** — SHOULD — spec-text
- 源：T5-F4。T5 defender F4 CONCEDE。
- 依据：`spec.md:555`「Qwen3-ASR-Flash vs Aliyun 流式：部署期核实」；validation §3.1:68-74 两候选「配置级接入、零开发、支持流式」= 调研断言非实测；`spec:101` SenseVoice 兜底触发 = 额度/隐私，不覆盖「云 ASR 流式不达标/不与 VAD 切句语义兼容」失败模式。
- 判官核验：U-1（上行 Opus→VAD 切句→云 ASR）与候选 provider 能力对齐仅配置期核实属实；「可替换 provider」压在未证抽象上（DNA#6）。
- 处置：spec-text（补 forcing-function：候选流式+VAD 兼容性部署期首验点；声明失败模式兜底）。P8（云优先取舍）本体不攻。

**V24 · 功耗实测「再看」无 forcing-function / 预警机制（S1 / P10 完整性面）** — SHOULD — spec-text
- 源：T5-F6（主）+ T1-F8 功耗子项（转并）。T1 defender F8-CONCEDE。
- 依据：`spec.md:583` 决策 21「省电不预设优化、功耗实测后再看」+ `:572` 决策 10 便携电池，无触发点/续航阈值/降级动作；唯一相位锚点 M6 在已废弃待重切的 v0.1 里程碑里。
- 判官核验：S1「即便实测后再看也须写明实测触发点与预警机制」未满足。charter P10 只锁「常驻连接」取舍，攻击面 = 完整性（缺 forcing-function），非该不该常驻。
- 处置：spec-text（决策 21 补实测触发点 + 预警机制：续航<阈值 → 加大心跳间隔/ACFE 休眠/降刷新）。

**V25 · 证书生命周期/轮换无 owner/forcing-function/相位，OTA 未建前到期死局未记账（S5）** — SHOULD — human（拍板）+ spec-text
- 源：T4-F1 × T5-F5（合并）。T4 defender F1 CONCEDE。
- 依据：`spec.md:554/584`「长期」证书有效期未量化、换证触发/owner/相位缺失；`:553` OTA 应答端点「留二期」；`:621`「证书轮换管理」开放项无 forcing-function。charter S5「OTA 未建前证书到期=死局」spec 未答。
- 判官核验：轴 7 DEFER（缺 forcing-function）违规属实。P3 只锁「选自签」取舍，完整性（轮换流程无 owner、OTA 相位未对账）是 P3 允许攻击面。**高影响安全/可用性 → 即便 defender CONCEDE，按「建议即地板」标 human**。
- 处置：human（拍板：OTA 未建前到期死局 accept-as-risk 还是 OTA 提前 + 有效期年限定夺）+ spec-text（轮换触发 + owner + 提前告警）。

**V26 · 设备 token 轮换/配网分发/强度/每设备-vs-共享 未定义** — SHOULD — human（拍板）+ spec-text/dev-handoff
- 源：T4-F2（吸收 T4-F7 的 token 强度 NIT）。T4 defender F2 CONCEDE、F7 CONCEDE(NIT)。
- 依据：`spec.md:621` token 轮换/配网分发开放项；`:552` 强制鉴权只强制存在性；`:553` 配网直写 NVS 但生成通道/配网加密/截获防护未定义；通篇「设备弱 token」单数（`:75/:534`）未声明每设备唯一 vs 全设备共享。佐证：`shim.py:43-45`（SHIM_DEVICE_TOKEN 空=鉴权关闭、单一共享 env）+ ARQ-260902-shim-401-gate-bypass 历史脆弱。
- 判官核验：属实。共享 token → 提醒回执/故事归档/会话无法归属设备 + 单点泄露危及全部；每设备 → 配网分发流程更致命。
- 处置：human（拍板：每设备唯一 vs 共享 + token 熵强度 ≥128-bit）+ spec-text/dev-handoff（配网加密通道、constant-time 比对）。

**V27 · display JSON 无 schema 校验 / 拒绝行为未定义（DNA#7 + AC 缺口）** — SHOULD — dev-handoff + spec-text
- 源：T4-F3 × T5-F8（合并）。T4 defender F3 CONCEDE（精度修正：view 白名单「取值」已在契约 §1.3 / spec:170 枚举，真缺的是**入站 enforcement**）。
- 依据：契约 `§1.5:139`「shim 用正则切分」、`shim.py:121-132` 仅 json.loads、无 view 白名单/data shape/size 校验；spec `§6.2:546-547`、`:168-171` 只枚举 view 未命名校验；S-F4.1 `:434` 把 >3 句判为「遵循度缺陷」非有处理的失败模式。DNA#7（zod 结构化输出经 schema 校验才下发）直接冲突，charter:142 预埋。
- 判官核验：不可信 LLM 内容（含 prompt-inject 的伪造 `control/end_session`）可直入板子盲渲染属实；方向 B 后校验点迁移至 push helper（spec:91）但 spec 未命名任何校验环节。
- 处置：dev-handoff（push helper 落地机械 schema 校验：view 白名单 + data shape/size 上限 + 脏 JSON 拒绝）+ spec-text（定义拒绝/回退路径 + S-F4.1 AC「格式不遵循=失败模式」的验收断言）。无轴 11 冲突（不引家长逐条人工审）。

**V28 · 18789 full-operator 收敛未 operationalized** — SHOULD — human（拍板相位）+ spec-text
- 源：T4-F4。T4 defender F4 CONCEDE（spec 层 operationalization 缺口，注记 ARQ-260901 已有 partial backstop 但未相位绑定）。
- 依据：`spec.md:77` 小智服务端「唯一公网入口」+ `:81`「loopback + 真 token 不出内网」是架构意图；真正把 18789 锁 127.0.0.1 的动作仍是 §9:621 开放项、无 owner/forcing-function/相位；方向 B 落地早于「生产上线」（ARQ-260901 复议触发），中间窗口 18789 仍公网可达。
- 判官核验：属实。轴 7（缺触发）+ 轴 10（收敛未 operationalized）双违反。**安全高影响 → 建议即地板，标 human**。
- 处置：human（拍板：18789 锁定写为方向 B 落地前硬前置的相位）+ spec-text（部署期断言绑定 landing 相位）。

**V29 · 儿童数据出网「取舍记账」不完整（转写文本→DeepSeek、回复→edge-tts 未入账）** — SHOULD — human（拍板记账/取舍）
- 源：T4-F5。T4 defender F5 CONCEDE-TO-HUMAN。
- 依据：`spec.md:581` 决策 19「仅 ASR 链路出内网」vs 转写文本经 DeepSeek（`573`，deepseek-v4-flash 云）、回复经 edge-tts（`79/:102` 微软非官方接口）均出内网，spec 无一处声明。
- 判官核验：记账不完整属实，「仅 ASR 链路」措辞会误导「儿童数据只有语音经阿里云出网」。轴 10 一等关注点。
- 处置：human（拍板：① 出网全貌是否已知/接受；② DeepSeek 托管地境内/境外 + 数据留存是否在同一条取舍内）+ spec-text（决策 19 改三路径逐个记账）。

**V30 · 儿童内容安全无姿态（轴 10↔11 最尖锐拍板点）** — SHOULD — human（拍板）
- 源：T4-F6。T4 defender F6 CONCEDE-TO-HUMAN（明确反对任何家长逐条审批；只走机械过滤 or ACCEPT-AS-RISK）。
- 依据：`spec.md:433` 唯一约束「system prompt 钉死 3 句内儿童化」无内容过滤/失败响应；不适龄内容会直接 TTS+上屏。
- 判官核验（焦点 + 拍板）：轴 10（儿童需内容管控）↔ 轴 11（六门以外默认 policy-as-code、禁第七人工门）对撞；challenger/defender 共识在「不加家长逐条审批」，分歧在「system-prompt 约束即足（记账）vs 机械内容过滤层（policy-as-code）」。
- 处置：human（拍板二选一：(a) system-prompt 即足 + ACCEPT-AS-RISK 记账；(b) 机械内容过滤层 + 明确失败响应）。**明确排除「家长逐条审批」形态**。

**V31 · S7 故事录音意图分流误判兜底/取消/回滚全未定义** — SHOULD — human（elicitation）+ spec-text
- 源：T5-F9。T5 defender F9 CONCEDE-TO-HUMAN。
- 依据：`spec.md:363` start_recording 意图分流、`:365`「或约定键」、`:366` 按 KEY「讲完啦」停录——S7 三支失效链（误判成累积态无回滚 / 真讲故事被判闲聊故事丢失 / 误收尾半截归档无取消）均无答案；触发句「我要讲故事」归闲聊缓冲还是累积缓冲未定义；全局按键表无录音启动键。
- 判官核验：判定者（OpenClaw）已写，但 S7 要的「判错怎么回滚」完全缺。
- 处置：human（elicitation：儿童口语误判率、是否专用键/显式确认）+ spec-text（补取消/回滚/误收尾路径）。

**V32 · 全文上屏滚动字号下限系于末端实测** — SHOULD — spec-text（+既有 ARQ）
- 源：T5-F10。T5 defender F10 CONCEDE-TO-HUMAN。
- 依据：`spec.md:590` 决策 28 全文上屏 + `:430-436` 16px 以下 CJK 未实测、字号以实测为准；PROPOSAL 9b 可读性验收殿后 mergeOrder 12。spec 有降级注（按可读字号、滚动承接）+ 决策 28「3 句以内」故降 SHOULD；但「3 句以内」是 prompt 软约束非 schema 闸，超即击穿滚动假设。
- 判官核验：属实；已关联 ARQ-260831-small-cjk-1bpp-readability（accepted-risk watchlist，复议触发=真机字体子集化）。
- 处置：spec-text（决策 28 带浮动字号底 + 声明「3 句以内」为生成端约束、超限有失败处理）+ 引用既有 ARQ-260831。

### A5 经费 / 冻结切片 / 数据 hygiene

**V33 · 冻结切片重切 pin 收紧（T5-F1 的 BLOCKING 定性驳回：SSOT 方向论成立）** — SHOULD — spec-text
- 源：T5-F1。T5 defender **REBUT**（SSOT 方向论）。
- challenger 主张（BLOCKING）：FROZEN 13 包（WebSocket=二期）vs 决策 9（方向 B 立即）互斥 =「冻结自我否定」；重切无 owner/相位。
- defender REBUT 依据：① `PROPOSAL.md:7` 声明「单一事实源 = spec.md + interface-contract.md」——spec 是 PROPOSAL 上游；13 包 FROZEN pin 是 2026-09-01 旧快照，决策 9（2026-09-02）定下那一刻 13 包即已过期，与「spec 是否正式冻结」无关；冻结恰是**稳定上游、使下游可重拆**，非自我否定。② 重切 owner/相位已在治理层命名——`spec-rereview-input-streaming-validation.md:97/:108`「13 包重拆 + Pod S 处置 = spec 重审 + Track A 承载」，owner = portfolio-planner（PROPOSAL:6），相位 = 紧随冻结；顺序「先冻结 spec、后 portfolio 重切」由 SSOT 方向即可推导、不需再裁。
- 判官核验（焦点裁定，SSOT 方向论）：defender REBUT 成立。challenger「两个构置目标 / 冻结自我否定」把「下游规划制品滞后上游 SSOT」误读为两个同等权威互斥——PROPOSAL:7 已显式自认派生制品。13 包过期的根因是决策 9，非 spec 冻结；冻结是重切前置而非障碍。**BLOCKING 定性驳回**。
- 残余缺口（defender 自身承认、判官 CONFIRM）：spec §8:610「里程碑随 spec 重审重切（**预期**…）」措辞过弱，未像契约门侧（canary 6 靠 DEV-GOVERNANCE §4 铁律钉「契约未重写 → 不可冻结」）那样给下游重切一个 committed pin。属 SHOULD 级收紧。
- 处置：spec-text（收紧 §8:610 + 冻结裁决钉入「冻结即触发 portfolio 重切，owner=portfolio-planner，相位=紧随」，与契约门 pin 对称）。不需 human 拍板（顺序由 SSOT 推导）。

**V34 · 离线一周重连后过期一次性安排清理 owner 未定（S6 瞬态）** — SHOULD — spec-text
- 源：T1-F6。T1 defender F6 CONCEDE。
- 依据：`interface-contract.md:191`「过期自动忽略（或归档）」动词无主语；`spec.md:582`「过期静默作废」限死提醒档，未覆盖 schedule.md 过期条目数据清理；S6 问「过期是否只对提醒成立、对课表视图也成立」未答。
- 判官核验：P1/P2 视图靠「按当前时间筛选」（`:110/:311`）已忽略过期条目故非 BLOCKING，但数据 hygiene owner 空缺属实。
- 处置：spec-text（补 owner：查询过滤已选；归档 = OpenClaw cron 定期移出活动表，或声明 MVP 不归档、增长治理留后续）。

**V35 · §9 开放问题清单系统缺 owner/相位/forcing-function 三列 + v0.1 stale 项** — NIT — spec-text
- 源：T5-F11。T5 defender F11 CONCEDE（NIT，实质=轴 7 系统性缺口）。
- 依据：`spec.md:616`「ASR/TTS 放 OpenClaw 机器（whisper+edge-tts）」「薄桥 shim 服务端」与决策 19/18 矛盾；`:630`「离线策略…本地缓存量」与决策 24 矛盾；`:622`「服务器地址配置入口定位」已被 `:553` 配网直写 NVS 解决（stale）。
- 判官核验：属实。整节开放项无一行带 forcing-function 三列；3 处 v0.1 残留未收敛。challenger 自标 NIT（因多数项已在 ARQ 队列/别处跟踪）；判官认可 NIT（non-blocking），但注记为近 SHOULD 的系统性 DEFER 纪律缺口。
- 处置：spec-text（§9 清理 stale 行 + 补 forcing-function 三列 / 或删除已入 ARQ 的重复项）。

---

## B. DISMISSED（3 条）

**T1-F2 ·「缓存」两义并用 + P1 生命周期/介质未声明** — DISMISSED-WITH-GROUNDING
- 驳回依据：challenger 的「缓存两义矛盾」不成立——`spec.md:291` 同句「**离线不可用**」已钉死瞬时性（掉电即失渲染缓冲），与 `:586`「设备不缓存事项/课表」自洽，不负担相反语义。defender REBUT 依据成立。
- 残余缺口（「板端持有 P1 日程生命周期/介质未写成可强制不变量」）与 T1-F3 同源，折入 V04，不双计。

**T1-F7 · 设备遥测（温湿度/电量/时间）owner 边界未落清（FYI）** — DISMISSED-WITH-GROUNDING
- 驳回依据：owner 已落清——`spec.md:291`「SHTC3 本地直读」+ `:586` 决策 24「仅时间/温湿度/电量」已声明「只在板端、永不进脑」；`:628`「是否云端记录看待定」显式挂号待决，非未落清。`:61`「所有数据都在这里」上下文指业务数据，不含 display-only 传感器遥测。challenger 自标「弱数据、FYI」，defender REBUT 成立，无字面裂缝需闭合。

**T1-F8 · S1 功耗**（拆两项裁）— AFE 子项 DISMISSED / 功耗 DEFER 子项转并 V24
- AFE「常跑 vs 播放中跑」自相矛盾：**DISMISSED-WITH-GROUNDING**——`spec.md:457`「本地 AFE(AEC+VAD) 启动」把 AFE 执勤钉到会话生命周期（按 KEY 进对话态才启动、常显 idle 休眠），`:467`「播放中持续跑」是回声消除+插话检测连续性，`:185`「跑在核1」是位置声明非「常态常开」。三者自洽，challenger「AFE 常态跑核1」是对「跑在核1」的过度引申。defender REBUT 成立。
- 常驻连接无最小功耗预算/降级触发点：**CONFIRMED 转并 V24**（轴 7 DEFER 纪律，与 T5-F6 同源），不在此双计。

---

## C. DEADLOCK 与拍板项

0 条真死锁（无「既无法 REBUT 又不愿 CONCEDE 且满预算仍争」项）。**11 组拍板项**（正反方一致或 high-impact 边界项，按「建议即地板」呈现，裁断权在 human）见 `review-packet.md`「拍板清单」。

---

*adjudications 结：2026-09-02 · adjudicator。本文件是判定记录，spec 改法落地属 moderator / 契约门 / human，本判官不落 spec、不改契约。*