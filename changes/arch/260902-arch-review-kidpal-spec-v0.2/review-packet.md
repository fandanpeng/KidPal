# Review Packet — 260902-arch-review-kidpal-spec-v0.2（收口）

> adjudicator 收口。审 = `docs/spec.md` v0.2（KidPal ESP32-S3 儿童学习伙伴，方向 B WebSocket+Opus 流式）；判据 = `review-charter.md`（冻结）。收口时点 2026-09-02。

## 0. 摘要

- 源 finding 46 条（T1=8 / T2=12 / T3=8 / T4=7 / T5=11），challenger 5 支全部落盘，canary 6/6 独立重派生命中。
- defender 轮 **5 支齐**（defense-T1..T5 均在盘；T2/T5 于判官首轮裁定中途补齐，已复核）。T2 defender 全 CONCEDE；T5 defender **T5-F1 有实据 REBUT**（SSOT 方向论），判官据此把 T1-F5 的 BLOCKING 定性驳回、降 SHOULD（见 §1）。
- 归并 38 条独立条目：**35 CONFIRMED / 3 DISMISSED / 0 DEADLOCK**。
- BLOCKING 源 7 归并 5（T2-F3×T5-F2 合一 + T5-F1 降级），全部 CONFIRMED；SHOULD 源 26 归并 26；NIT 4；FYI 1（DISMISSED）。
- 处置归属（CONFIRMED 主归属）：spec-text 19 · contract-gate 1 · human 11 · dev-handoff 3。

## 1. 合并终版 findings（findings.md 内容并入此处）

> 注：`findings.md` 独立落盘被 harness 写盘护栏拦截（report-file guard，命中「findings」文件名）。合并终版并入本 packet；逐条裁定全量在 `adjudications.md`。拦截不作为制品缺口——终版内容仍以本表 + adjudications.md 双载体交付。

### BLOCKING（5）

1. **V01** v0.1 遗留面（§3.2–§3.4 + 决策 4/6 HTTP·faster-whisper·轮询·FastAPI）live 互斥 → FIX-SPEC · spec-text
2. **V03** 板端存储三处互斥（SD「本地缓存录音/字库/课表」vs 不存数据红线）→ FIX-SPEC · spec-text
3. **V05** alert 双形态（原生 vs mcp kidpal/alert）+「固件已有渲染处理」为假 → FIX-SPEC · spec-text
4. **V08** spec↔契约硬漂移命中「契约未批→spec 不可冻结」铁律 → CONTRACT-GATE · contract-gate
5. **V14** 幂等去重/回执「已渲染 vs 排队未渲染」矛盾（S4 必然叠加态）→ HUMAN-RULED · human 拍板修法口径

（T5-F1 原标 BLOCKING「冻结切片悬空」，被 defender SSOT 方向论驳回 → 降 SHOULD，见下 V33。）

### SHOULD（26）：V02 VAD 句边界 / V04 RAM-only 不变量 / V06 custom 并写 / V07 B3 单行残留 / V09 max_tokens 落点 / V10 CA 嵌入面(dev) / V11 鉴权承载(dev) / V15 幂等 id 稳定性 / V16 排程 handoff / V17 带物回读 / V18 滞留弹窗 / V20 提醒 TTS 解耦 / V21 多轮连续性 / V22 家长端+producer / V23 ASR 终选 forcing-function / V24 功耗 DEFER / V25 证书生命周期 / V26 token 强度 / V27 display JSON 校验(dev) / V28 18789 收敛 / V29 出网记账 / V30 内容安全 / V31 故事录音 UX / V32 字号实测 / V33 冻结切片重切 pin 收紧 / V34 过期清理 owner。全部 CONFIRMED，逐条纯度见 `adjudications.md`。

### NIT（4）：V12 llm/system / V13 shim 措辞 / V19 标记送达 / V35 §9 三列 — non-blocking，FIX-SPEC。

### DISMISSED（3）：T1-F2 缓存两义、T1-F7 遥测 owner、T1-F8-AFE（功耗子项转并 V24）。

### 实现侧 finding 移交表（供 arch-review-handoff-check.mjs 机检 + dev-loop planner intake）

| ID | 一句话 | outcome | 归属 |
|---|---|---|---|
| V10 | CA 嵌入固件无固件 SSL 层证书加载实现面 | FIX-IMPLEMENTATION (CONFIRMED) | dev-handoff |
| V11 | 强制鉴权「token 未设=拒」服务端承载未验证 | FIX-IMPLEMENTATION (CONFIRMED) | dev-handoff |
| V27 | display JSON 无 schema 校验/拒绝行为未定义 | FIX-IMPLEMENTATION (CONFIRMED) | dev-handoff |

## 2. pass-oracle 五关状态

| 关 | 状态 | 依据 |
|---|---|---|
| ① 轴覆盖 | **GREEN** | 12 轴全被 challenger 走过（轴0/5=T1，1/3/4=T2，6/9/11=T3，10/11/7/8=T4，2/7/8=T5）。轴 10↔11 对撞点（V30）已交拍板。 |
| ② 场景覆盖 | **GREEN** | S1→V24、S2→V21、S3→V20、S4→V14/V15/V17/V18、S5→V25、S6→V34、S7→V31 全走完。 |
| ③ finding 全结 | **RED（待落地）** | 46 条全部落到 outcome（35 CONFIRMED / 3 DISMISSED / 0 未结），但 5 BLOCKING + 26 SHOULD **尚未落地**（改法/回灌/契约门未完成）。按 rubric「未结 BLOCKING/SHOULD ⇒ 不可冻结」，canary 全中 + 逐步落地后才转绿。 |
| ④ spec↔contract 机械门 | **RED（人工核对，如实记录）** | 本仓无脚本兜底（ARQ-260827-contract-mech-gate-gap），降级人工核对。核对结论：无 schema 复制 / 无 core.* 引用（KidPal 无内核），但 spec↔契约**硬漂移存在**（轮询/remind_before_min/mp3/custom/无协议剖面节）= V08，契约门（CDQ-260902）未批前不绿。 |
| ⑤ canaries 全中 | **GREEN** | 6/6 独立重派生：canary1→V09、canary2→V14/V15/V17/V18、canary3→T3 走完（仅 NIT 边界）、canary4→V05/V06、canary5→V01/V03/V02/V35、canary6→V08。 |
| ⑥ dev-loop handoff 完整 | **GREEN（本次产出）** | 3 组实现侧 CONFIRMED（V10/V11/V27）已写入 `dev-loop-handoff.json`；其余为 spec-text / human / contract-gate，不进 handoff。 |

## 3. 冻结结论 —— 条件冻结（非 hard-freeze）

spec **不可标「无条件冻结」**（5 BLOCKING 未清）。**可标「条件冻结」**，满足以下条件方可转正式冻结：

1. **契约门四题 human 批准**（V08，铁律前置）：批准契约重写为协议剖面 + §三推送语义 + Opus/WS + 强制鉴权/渲染回执/speech + B1 `remind_before_min` 删/标废止——与 spec 同 PR 原子落地。
2. **CONFIRMED spec-text 改法全部落地**（moderator/主线程）：V01/V02/V03/V04/V05/V06/V07/V09/V12/V13/V15/V16/V18/V19/V23/V24/V32/V34/V35 + V08 判定的 spec 侧措辞 + V33（§8 里程碑重切 pin 收紧为 committed，冻结裁决一并钉入「冻结即触发 portfolio 重切，owner=portfolio-planner，相位=紧随」）+ V14 拍板后的 spec 文本回灌。
3. **human 拍板项全部回灌**（11 组拍板，见 §4）：拍板结果落地到 spec / PROPOSAL / ARQ。
4. **实现侧 3 组进 dev-loop handoff 并被 planner intake**（V10/V11/V27），不伪装成本 loop 落地。

## 4. 拍板清单（给 human，11 组 + 契约门四题，建议即地板）

> 每组附选项 + 判官推荐。裁断权在 human，判官不替裁。

1. **契约门四题（CDQ-260902，V08 铁律前置）**：
   ① 批准契约从 HTTP 形态重写为方向 B 小智协议剖面（新增协议剖面节 + §三重写 + §1.5 收敛 + §1.4 control 扩 start_recording）？D3/E1①-⑥/B1 表体零改动；
   ② 确认 WS+Opus（上行 16kHz/60ms、下行 24kHz），display/提醒经 mcp（非 base64 mp3）？
   → 判官推荐：批准（spec §6.3:551/§6.1:534 已定稿）。
   ③ 确认强制鉴权（Bearer、未设=拒、无 validation-mode 后门）+ 出队=渲染回执 + alert 带 speech？→ 推荐：批准（决策 22/20/23/27 已定稿）。
   ④ B1 `remind_before_min` 删 vs 标「legacy/废止」？同步清 spec:178 轮询残留 + 修正「B1 零变更」措辞。→ 推荐：标废止 + 修 spec 措辞（保留历史语义可读）。

2. **V14 幂等修法口径**：幂等集是否含「已接收未渲染」队列成员？→ 判官推荐：采纳 defender 解耦（幂等去重=接收即入集 屏蔽重复入队；回执出队=渲染即发，守住 P4「渲染即确认」）；可选规范化为 received/queued/rendered/acked 四态。**建议即地板**：human 可砍更大（如重审「渲染即确认」是否该是正确确认语义）。

3. **V22 家长端形态三选一**：QQ（OpenClaw 既有通道）/ 小程序（整建家长端）/ 直接改 MD（零实现）？连带定「新建日程即时推」producer（OpenClaw 变更事件 / 网关 FS watcher / 显式承认直改 MD 不即时推）。→ 判官推荐：MVP 用「QQ 录入（OpenClaw 改写自知可即时推）+ 直改 MD 显式降级靠 WS连接/0:00 兜底」；小程序后置。

4. **V29 出网三条路径记账**：① human 是否已明知转写文本送 DeepSeek 云、回复送 edge-tts；② DeepSeek 托管地境内/境外 + 数据留存是否落在「境内可接受」取舍内。→ 推荐：改决策 19 措辞，三条出网路径（阿里云 ASR / DeepSeek / edge-tts）逐条记账，消除「仅 ASR 链路」误导。

5. **V30 内容安全姿态（轴10↔11）**：(a) system-prompt 约束即足 + ACCEPT-AS-RISK 记账 或 (b) 机械内容过滤层（policy-as-code）+ 明确失败响应？→ 判官推荐：MVP 选 (a) 记账（与 DNA#7 的 schema 校验并行），若日后竞品/合规要求再上 (b)。**明确排除任何「家长逐条审批」形态**（第七人工门红线）。

6. **V31 故事录音意图分流 UX**：儿童口语「我要讲故事」误判率如何兜底？是否专用键/显式确认规避误判；补取消/回滚/误收尾路径。→ 判官推荐：MVP 加显式回退（KEY 长按取消/「讲错啦」重置）+ 触发句归闲聊缓冲，不投入语音意图精判；专用录音键可选。

7. **V25 证书到期死局**：OTA 未建前证书到期 = 全部设备 wss 瘫痪 + 逐台重烧，accept-as-risk 还是 OTA 提前？+ 有效期年限定夺（「长期」量化）。→ 判官推荐：证书有效期拉到足够长（如 ≥10 年）+ 落轮换脚本 owner/触发 + phase 对账；若对账显示 OTA 必晚于首期续证，则 OTA 提前或显式记账死局。

8. **V26 设备 token 每设备-vs-共享 + 强度**：每设备唯一（回执/归档/会话可归属，但需配网分发流程）vs 全设备共享（零流程但单点泄露危及全部）？+ token 熵 ≥128-bit + constant-time 比对。→ 判官推荐：每设备唯一 + 随 wss 握手发放/一次性绑定码配网；强度 ≥128-bit 随机。

9. **V28 18789 收敛相位**：把小智服务端「唯一公网入口」成立前的「18789 锁 127.0.0.1」写为方向 B 落地前硬前置？→ 判官推荐：是，绑定方向 B landing 相位 + 部署期断言，与 ARQ-260901 复议触发合并。

10. **V20 提醒 TTS 解耦**：edge-tts 被封/慢时提醒三件套是否退化「提示音+弹窗（无语音）」继续投递？speech 合成失败跳过还是阻塞？合成时刻=排程预合成 vs 到点现合成？→ 判官推荐：弹窗+提示音必达，speech 为可选增强、失败/超时降级不阻塞弹窗。

11. **V21 多轮连续性承载**：`type:openai` 接入时网关无状态调用是否由 agent 内部维持会话历史，还是网关自行传 session_id/历史？若继续 DEFER 则 forcing-function 是什么？→ 判官推荐：方向 B 落地首验（部署期第一件事）定案；验证失败则 A4/S-F3.3「那『星』字呢」指代降级为单轮问答并记账。

## 5. 残余风险（冻结后仍存在，已在 ARQ 队列）

- 证书生命周期死局（S5，V25）→ 新 ARQ 登记（见队列更新）。
- 多轮连续性验证（S2，V21）→ 新 ARQ 登记（部署期第一件事）。
- 16px 以下 CJK 1bpp 可读性（V32 关联）+ I1 索引真机可用性 + 模拟器↔固件失真要角 → 既有 ARQ-260831 系列 watchlist 持续。
- 契约门机械门脚本缺失（ARQ-260827-contract-mech-gate-gap）→ 本次第 4 关仍人工核对，如实记录。

## 6. 过程性注记项（交 moderator，不阻塞裁定）

1. **findings.md 落盘被 harness 写盘护栏拦截**（report-file guard 命中「findings」文件名）：合并终版已并入本 packet §1 + `adjudications.md` 双载体，制品内容无缺失；仅独立文件未落，且 `arch-review-handoff-check.mjs` 已按 review-packet §1 实现侧移交表正常检出 V10/V11/V27（exit ok，未因 findings.md 缺失假绿或假红）。
2. **defense-T2/T5 中途补齐**：判官首轮 T2/T5 裁定先按 challenger 依据 + 独立核验出具，defense-T2/T5 落盘后已复核——T2 全 CONCEDE（与首轮一致），T5-F1 有 REBUT 降级（§1/V33）。最终裁决以复核后为准，无残余 RED。

---

*review-packet 结：2026-09-02 · adjudicator。冻结结论 = 条件冻结；hard-freeze 待 §3 四条件满足。*