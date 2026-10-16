# Findings — T4 安全与儿童隐私信任边界（轴 10 ↔ 11）

> challenger 轮产出（2026-09-02）。主线程代落盘（challenger 以文本交付，内容保真）。判据 = `review-charter.md`（冻结）。
> severity 分布：SHOULD ×6，NIT ×1，BLOCKING ×0（本支无独立 BLOCKING；canary 5/6 的 BLOCKING 归 T2/T5 主责）。

**TOP 3 最锋利**：
1. **T4-F5 隐私记账失真**——spec 决策 19 声称儿童语音「仅 ASR 链路出内网」，但转写文本经 DeepSeek（deepseek-v4-flash 云）+ 回复经 edge-tts（微软）均出内网，记账不完整、隐私声明与其自身架构路径不符。
2. **T4-F3 不可信 display JSON 零 schema 校验直入设备渲染**——契约 §1.5 只写「正则切分」，shim.py 只做 json.loads 语法通过/失败回退 text，无 view 白名单、无拒绝脏 JSON；方向 B 后校验点迁移到小智服务端 push helper，spec 未命名校验环节。
3. **T4-F4 18789 full-operator 收敛未 operationalized**——ARQ-260901 的裸暴露面在方向 B 后仍只以 §9 开放项存在，无 owner/forcing-function，spec 冻结不阻断「18789 仍公网可达」。

**是否疑似 BLOCKING**：本支无独立 BLOCKING。但 T4-F5 若 human 实际未被告知「转写文本出网到 DeepSeek 云、回复经微软 edge-tts」，则记账缺陷从 SHOULD 升为需拍板的实质分歧（elicitation 待确）。

---

## T4-F1 · 自签证书生命周期 = S5 死局，无 owner / 有效期 / OTA 相位对账（轴 10 + 轴 7）

- **严重度**：SHOULD
- **引用**：spec §6.3:554「wss + 自签长期证书（…CA 嵌入固件，换证需动固件/重新烧录，轮换管理责任在自己）」；§6.3:553「自建 OTA 应答端点留二期 OTA 铺路」；§7 决策 22:584 同义；§9:621「自签证书轮换管理」列为开放项。
- **问题**：「长期」证书的有效期（年限）、到期/换证的触发条件、owner、相位，spec 全部未声明。方向 B 下设备常驻 wss（§6.3:554 握手依赖该证书），证书一旦到期 → 全部设备 wss 握手失败（charter S5 失效链），而 OTA 留二期、换证需逐台重烧。charter §7 P3 只锁死「选自签证书」这个取舍本身（不攻击），但**完整性缺口**（轮换流程无 owner/forcing-function、OTA 未建前到期即死局未与 OTA 相位对账）正是 P3 允许攻击的「实现含义/完整性」面。§9 把它当开放项 = 无触发条件的「永不」幽灵（轴 7 DEFER 违规）。
- **elicitation**：否（可从仓库证伪 spec 缺失）。
- **轴 11 框定**：证书轮换属机械运维动作，不引人工门。

## T4-F2 · 设备 token 轮换 / 配网分发流程未定义，token 强度与「每设备 vs 共享」未定（轴 10 + 轴 3）

- **严重度**：SHOULD
- **引用**：spec §9:621「设备 token 轮换与配网分发流程」开放项；§6.3:552「强制鉴权…token 未设 = 拒绝服务」；§6.3:553「配网直写 NVS（namespace websocket 键 url/token/version）」；`server/shim.py:43-45`（`SHIM_DEVICE_TOKEN` 为空 = 鉴权整个关闭、单一共享 env）。
- **问题**：① token 从何处生成、经何通道配网写入设备 NVS（配网通道是否加密、是否可被截获）未定义；② 一旦泄露如何轮换（「弱 token + 不可轮换」= 永久暴露面）未定义；③ spec 通篇「设备弱 token」单数（§3.0.0:75、§6.1:534），未声明是**每设备唯一 token** 还是**全设备共享 token**——若共享，则无每设备身份，提醒回执/故事归档/会话无法归属到具体设备，且单点泄露危及所有设备；若每设备，则「配网分发流程」开放项更加致命（每设备 token 发放无流程）。安全史佐证：ARQ-260902-shim-401-gate-bypass（require_device 靠 `return JSONResponse` 不短路曾真穿透 401 门）证明这条信任边界历史脆弱性。
- **elicitation**：部分——token 是每设备还是共享属架构定夺，但可从 spec 证伪其未声明。
- **轴 11 框定**：token 轮换/配网是机械化 background 流程，不引「家长逐条审批」人工门（自动轮换 + 配网校验即可）。

## T4-F3 · display JSON 无 schema 校验直入设备渲染（不可信内容入站 + prompt injection，DNA#7 缺口）（轴 10 + 轴 8）

- **严重度**：SHOULD
- **引用**：契约 §1.5:139「shim 用正则切出 display 块」；`server/shim.py:121-132`（`split_display` 只 `json.loads`，JSONDecodeError → `display=None` → 回退 `{"view":"text","data":{"text":speech}}`，无 view 白名单、无 data shape/size 校验）；spec §6.2:546-547 / §3.0.2:168-171（E1 只定义 view 枚举，未定义校验与拒绝语义）。
- **问题**：OpenClaw/DeepSeek 产出的 ` ```display ` JSON 块途经网关下发、板子**盲渲染**（哑终端 §3.0:57「只负责渲染」）。契约仅「正则切分」，无 schema 校验、无 view 白名单 (char/schedule/…/control 之一)、无大小上限。风险：① DeepSeek 被 prompt-inject（如家长/孩子在 schedule.md 或故事正文嵌入伪造 ` ```display ` + `control/end_session` 或畸形 view）→ 直接驱动设备进入任意状态；② 格式错误的 JSON 在 shim 路径有 `text` 回退（旧 shim），但方向 B 后校验点迁移至小智服务端 push helper（spec §3.0.0:91「结构化下发 push helper，各约几十行」），spec **未命名任何校验环节**。这与 charter §6 必载 DNA#7（zod 结构化输出、经 schema 校验才下发）直接冲突。
- **elicitation**：否（可从契约/shim 证伪无校验）。
- **轴 11 框定**：此缺口必须用**机械 schema 校验（policy-as-code，拒绝脏 JSON/白名单 view）**补，不是家长逐条人工审——无轴 10↔11 冲突。

## T4-F4 · OpenClaw chat/completions full-operator 公网收敛路径未写为强制前置（ARQ-260901）（轴 10）

- **严重度**：SHOULD
- **引用**：spec §3.0.0:77「（xinnan-tech…）部署于 ECS，唯一公网入口」+ §3.0.0:81「loopback + 真 token（真 token 不出内网）」；§7 决策 5:567「公网 ws://8.137.91.16:18789 已验证通」；§9:621「余开放项：安全组收敛（18789 仅回环）」；ARQ-260901-openclaw-chatcompletions-public（`docs/ARCH-REVIEW-QUEUE.md:46`）。
- **问题**：方向 B 使小智服务端成为「唯一公网入口」（§3.0.0:77），架构意图上 18789 转 loopback——**但真正的收敛动作（安全组/防火墙把 18789 锁 127.0.0.1）在 spec 里仍是 §9 开放项，无 owner、无 forcing-function、无 phase**。ARQ-260901 记录该端点为 full operator access、`bind=lan` 公网可达、仅 Bearer 保护，且其 accepted-risk 复议触发是「生产上线前须锁 127.0.0.1」。spec 冻结不把「18789 收敛」写成方向 B 落地的前置硬条件，意味着 spec 冻结时该裸 full-operator 暴露面仍可存续。此为轴 7 DEFER（缺触发）+ 轴 10（收敛路径未 operationalized）双违反。
- **elicitation**：否（可从 spec + ARQ 证伪）。
- **轴 11 框定**：收敛是机械防火墙动作，policy-as-code 化即可，不引人工门。

## T4-F5 · 儿童数据出网「取舍记账」不完整：「仅 ASR 链路出内网」与自身架构路径冲突（轴 10）

- **严重度**：SHOULD（若 human 未被告知出网全貌则升为需拍板实质分歧）
- **引用**：spec §7 决策 19:581「儿童语音经云端的取舍已由 human 接受（境内服务商、仅 ASR 链路）」；§3.0.0:79「TTS（edge-tts）」（edge-tts=微软外部接口，§3.0.1 A2:102「微软非官方接口」）；§7 决策 11:573「DeepSeek」+ 验证材料 §1「deepseek-v4-flash」。
- **问题**：儿童语音的账是记了（voice → 阿里云 ASR，境内，human 接受），但**派生的转写文本**随后经 OpenClaw → DeepSeek（deepseek-v4-flash，云端 LLM，外部）出内网，**回复文本**经 edge-tts（微软）出内网——两者均不在「仅 ASR 链路出内网」的记账内。spec 没有任何一处声明「转写文本出网到 DeepSeek 云」与「回复文本出网到微软」这两个事实，而「仅 ASR 链路」的措辞会给读者「儿童数据只有语音经阿里云出网」的误导。对一个 8-12 岁儿童产品，隐私记账的完整性是轴 10 的一等关注点；此处的账与架构实际出网路径不符。
- **elicitation**：是——需 human 确认：① 是否已明知转写文本会送 DeepSeek 云、回复会送微软 edge-tts；② DeepSeek（deepseek-v4-flash）托管地境内/境外 + 数据留存政策是否在「境内、可接受」的同一条取舍内。这是开放式知识抽取，不做 gotcha。

## T4-F6 · 儿童内容安全无姿态：DeepSeek 输出可无过滤直播给孩子（轴 10 ↔ 11 拍板点）

- **严重度**：SHOULD（+ elicitation）
- **引用**：spec §4.4 S-F4.1:433「答案纪律由 system prompt 钉死：口语化、3 句以内、适合 8-12 岁」；§3.0.1 A3:103（意图理解全包，无内容过滤）；§9:631「儿童隐私与鉴权：设备绑定、传输加密、家长管控」开放项。
- **问题**：spec 对生成端内容唯一的约束是「3 句以内/口语化」的长度与语气约束，**无任何内容安全/年龄适宜性姿态**——「适合 8-12 岁」被压在 system prompt 一句话里，无 schema 校验、无内容过滤层、无明确失败响应（若 DeepSeek 输出不适龄/违规内容会直接 TTS 播给孩子 + 上屏）。对于一个全自动、无家长运行时的儿童产品，这是轴 10 信任边界的真实缺口。
- **elicitation**：是——「儿童产品的内容安全是否需要一层 policy-as-code（机械内容过滤/供应商内容安全 API），还是 human 接受 system prompt 级约束即足够」是产品愿景层面需 human 注入的判断，非仓库可证伪。
- **轴 11 对撞（拍板点）**：这是本支最尖锐的轴 10↔11 对撞——轴 10 会主张「儿童需内容管控」，轴 11（蓝图 §5 关卡纪律，六门以外默认 policy-as-code）禁止把「家长逐条审批孩子收到的话」做成人第七门。challenger 不主张任何「家长逐条审批」式缓解；若拍板认为需缓解，只能走**机械内容过滤层（policy-as-code）**，否则保持「system prompt 约束 = 已接受」并 ACCEPT-AS-RISK 记账。此取舍提请拍板，不在本支私裁。

## T4-F7 · 「弱 token」/「强制鉴权」词义与 token 强度未定义（轴 3 + 轴 10，挂 T4-F2）

- **严重度**：NIT
- **引用**：spec §3.0.0:75「设备 token（弱权限…）」；§6.1:534「设备弱 token」；§6.3:552「强制鉴权…token 未设 = 拒绝服务，不留 validation-mode 后门」；`server/shim.py:43-45`（env 字符串比对）。
- **问题**：「弱 token」= 弱**权限**（仅设备面，非 operator），但 spec 未声明 token 的**随机强度/熵**（256-bit 随机会话密钥 vs 6 位 PIN）。「强制鉴权」只强制「存在性」——若 token 可被猜/弱随机，强制 == 无意义。且 `shim.py:95` 用 `!=` 明文比对（非 constant-time），虽方向 B 退役 shim，但安全史基线须记账。此点与 T4-F2（token 未定强度/共享）同源，为 NIT 级表述缺口。

---

## regression-canaries 重派生结果（6/6 全部独立重新找到）

| # | canary | 独立重派生结果 | 归属支 | 严重度 |
|---|---|---|---|---|
| 1 | 流式请求禁带 `max_tokens` | spec 三处散落 prose：§3.0.0:83、§6.1:542、§7 决策 18:580，均为「禁带」硬词但**未落为可被 builder 遵守的结构约束、无契约落点**（契约门重写小智协议剖面节时此配置约束需有落点，否则散落注记易被 builder 漏接）。 | T2/T5 | SHOULD |
| 2 | 渲染即确认 vs 排队弹窗 vs 易失 id 重弹语义 | spec §7 决策 20:582「渲染即确认出队…多提醒排队逐个弹…易失 id 集合幂等去重（重启后重弹一次可接受）」+ S-F1.4:326-327——「孩子没看但渲染了」= 算送达（渲染即确认），与「队列中未渲染条目待渲染才回执」叠加后，「确认」的语义边界（送达=渲染而非观看）读者需从 P4 重试模型自行还原，不变量未一处显式声明。 | T3 | SHOULD |
| 3 | 按区块判提醒范围边界 | §7 决策 20:582 + S-F1.4:319「按区块判」——边界案例（周中晚培训班落「每周固定课表」→仍提醒；「学校课程表」与「每周固定课表」同日重叠的去重/分组；节假日补课归入何区块）spec 无歧义裁决。 | T3/T5 | SHOULD |
| 4 | alert/custom 提醒通道命名漂移 | §3.0.1:134 E3「提醒下发…经 WS 推送 alert 消息（小智协议原生类型，固件已有渲染处理）」vs §6.1:538「mcp 消息…提醒（kidpal/alert）…小智原生 alert/custom 通道弃用」——同一提醒在同 spec 内以两种通道形态出现，是内部矛盾。（T4 角度：本漂移连带「哪个通道承载设备鉴权/出队 ack」的边界不清。） | T2 | SHOULD |
| 5 | v0.1 遗留面（HTTP / faster-whisper 残留） | §3.2:202「ASR…faster-whisper，经 shim /v1/asr」、§3.3:232、§3.4:240、§7 决策 4:566「shim…提醒轮询」+ 决策 5:568 + M2:604「whisper」vs §3.0.1 A1:101 + 决策 19「faster-whisper 弃选、云 ASR」——spec 前半段仍是 v0.1 同步 HTTP 叙事，与更新段互斥。 | T5 | BLOCKING（跨支归属） |
| 6 | spec↔契约漂移（轮询 vs 推送、remind_before_min vs 三档阶梯） | 契约 §2.2:164「remind_before_min: 15」+ §2.3:189/194 + §三:218「设备轮询 /v1/alerts → shim 读到并清除」vs spec §7 决策 20:582「frontmatter remind_before_min 废止」+ 决策 23:585「JSON-RPC 回执=送达确认（渲染即确认）」+ S-F1.4:335「契约三待重写」——硬冲突。按 protocol「契约未批前 spec 不可冻结」铁律命中。（T4 角度：契约 §三「读到并清除」的轮询模型无鉴权确认语义，与 spec「渲染即确认 ack」的送达信任边界冲突，是出队/送达确认的信任边界漂移。） | T2/T5 | BLOCKING（跨支归属） |

---

## 交付说明

- 编号 T4-F1…F7，falsifiable-first：全部引 `docs/spec.md` / `docs/contracts/interface-contract.md` / `server/shim.py` / `docs/ARCH-REVIEW-QUEUE.md` 的具体行，无凭感觉项。
- canary 6/6 重派生；canary 5/6 为 BLOCKING 但属 T2/T5 主责，此处按轴归属标注、不重复主张 T4 ownership。
- 级联标注：本批无「某条裁定的下游派生」（首轮对抗、无前序裁定）；但 T4-F7 是 T4-F2 的同源 NIT 展开，T4-F4 与 canary 6 的 ack 信任边界同源，已交叉标注。
- 需人类知识（elicitation）项：T4-F5（出网全貌是否已知 + DeepSeek 托管地/留存）、T4-F6（儿童内容安全是否需要机械过滤层 vs system prompt 即足），两处均非 gotcha、提请拍板。
