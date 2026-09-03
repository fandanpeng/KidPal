# Review Charter — KidPal spec v0.2 首次对抗审

> 性质：冻结的 review charter（judging criteria）。本文件对 challenger / defender **只读冻结**；改变判据须经 moderator 重新派 scenario-author。
> 待审对象：`docs/spec.md` v0.2-草稿（KidPal ESP32-S3 儿童学习伙伴）。
> 核心消费契约：`docs/contracts/interface-contract.md`（E1 统一响应 + B1 课表 MD + §三 alerts）。
> provenance：`changes/arch/260902-arch-review-shim-api-field-envelope/spec-rereview-input-streaming-validation.md`（三线验证 + §7 四轮 grilling 定稿 28 条决策）。
> 首次对抗审 = 兑现 `ARQ-260827-tier1-boot-review`；同时承载跨系统方向变更「WebSocket + Opus 流式（方向 B）」的最终裁决。
> spec 类型判定：技术重 + 领域重混合（设备/网关/脑三面流式协议 + 儿童课表/提醒/故事/查字领域）。判据见 §2/§3。

---

## 1. 审查对象与范围声明

**审什么**：`docs/spec.md` v0.2 的架构自洽性、可冻结性、可实现性切片——不是替 spec 改法，不是替契约门裁契约改法。

**明确排除（不审）**：
- 契约改法本身。`interface-contract.md` 现有 HTTP 四端点 / 轮询 / `remind_before_min` 形状待契约门按方向 B 重写为「小智协议剖面节」（CDQ-260902 在办）。本 charter 只把 **「spec↔contract 一致性」列为审查轴**（轴 4），要求 challenger 指出 spec 与契约现有文本之间的漂移**在哪、算什么严重度**，不要求任何一方在 charter 内解决契约改法。
- OpenClaw 流式能力本身的真伪（已验证：`stream:true` token 级流式实测通过），只审 spec 引用该能力时的适配约束是否写对、写全。
- human 已显式裁定的取舍本身（见 §6 已接受前提），只可攻击其一致性 / 完整性 / 实现含义。

---

## 2. 适用 rubric 轴（challenger 逐条质询）

> 轴库 `.claude/references/arch-review-rubric.md`。本仓是单系统消费产品、无 `core.*` 内核、无 Tier-1/Tier-2 跨系统契约面；以下每条轴给出**本仓端云语境下的映射**，challenger 照映射打，不许照搬 AInativecompany 多系统语义。

| 轴 | 在本 spec 的映射 | 为什么适用 |
|---|---|---|
| **0 范围 / 端云边界 / 哑终端纯度** | 板子是否把本该归脑侧（OpenClaw）的能力整条揽进端侧，或反向：脑侧是否把实时性要求揽进自己（有「gap」） | 最高产轴。spec 自称「板端哑终端」，但多处端侧持有状态（提醒易失 id 集合、P1「接下来」缓存、本地 AFE、会话状态机、SD 缓存录音/字库）——challenger 须逐一对「板子持有哪些状态/数据」打边界，判「哑终端不存数据」红线（决策 24）与这些端侧状态是否自洽 |
| **1 双向声明** | 每个 expose / 喂给 / 只读投影 / 事件 producer 都有匹配的消费方声明；反之亦然 | 方向 B 下 spec 新增多条端↔云通道（display 指令、提醒 push、回执 ack、上行 Opus、abort、start_recording），任一 producer 无 consumer 或 consumer 无 producer = 断链。§5 双向消费方清单的**留空点**即打点 |
| **2 相位 / 依赖自洽** | 依赖一个「更晚」才具备的能力却不写降级 / defer 说明 | spec 把 OpenClaw 多轮连续性、ASR provider 终选、edge-tts 可用性标为「部署期验证/核实」——challenger 要打这些是否为「先冻 spec、后验证就不成立」的隐性依赖 |
| **3 命名 / 词汇冲突** | 同名异义 / 术语漂移 | spec 内「alert 消息」在多处含义漂移（§3.0.1 E3「小智协议原生类型」vs §6.1/决策 23「mcp kidpal/alert 工具、原生 alert 弃用」）；「每周固定课表」区块的「培训班」与「学校课程表」区块的分组语义；「custom / mcp」通道命名两处写法不一致 |
| **4 单一事实源 / 无漂移** | spec ↔ 契约名真实存在且不偏离；spec 内部图/表不互斥 | 契约 §二（`remind_before_min` 提醒）与 §三（轮询 /v1/alerts、读到即清）仍是 v0.1 形状，与 spec 决策 20/24/23（三档阶梯 + 推送 + 渲染即确认）硬冲突；spec 内部 §3.2/§3.3/§3.4 与 §3.0.0/§6.1/§7 也存在新旧漂移 |
| **5 所有权 & Tier** | 数据 / 配置的 owner 落点 | 数据（课表 MD / 故事 MD / 笔顺）归 OpenClaw（脑侧）；提醒三档时刻 = 全局常量配在 OpenClaw 侧（不进契约）；板子无状态。challenger 打「哪些东西的 owner 落点没写清或落错」（如「新建日程后即时推」谁来检测 schedule.md 变更） |
| **6 门 / 编排正确性** | 「脑算网关推」的排程→待发→推送→出队职责链 owner 显式、无错置 | 提醒编排 = OpenClaw cron 算三档 → pending.json → 网关到点 push → 设备回执出队。这条链的每一步 owner、触发、失败归属必须显式；challenger 打链上任何「无 owner / 双重 owner / 漏一环」 |
| **7 DEFER 纪律** | 每个开放项 / 遗留面有 forcing-function + owner + 相位 | spec §9 开放项（家长端形态、配网绑定、功耗实测、自签证书轮换流程、设备 token 轮换）是否 each 有触发条件，还是「永不」幽灵 |
| **8 完整性 / 非空过** | 每个 story / AC 可实现、可证伪；集成故事具名外部系统 + dedup 边界 | display 指令 JSON 若 DeepSeek 输出不遵循格式、或未经 schema 校验直入板子渲染 = 不可信内容直入设备；家长录入「外部系统」未具名（QQ/小程序/改 MD 三选一悬置）；故事录音「误触/误判意图」无定义 |
| **9 不变量 & 失败模式** | 声明的不变量可强制；失败场景有设计好的响应（不是「不会发生」） | 提醒投递的幂等/出队/排队、常驻连接功耗、断网离线、证书过期、TTS 被封——§4 pre-mortem 逐条走，spec 必须给出「预期响应」，缺失即 finding |
| **10 安全 / 权限 / 数据隔离**（架构级） | 公网入口的信任边界、儿童语音经云端、设备弱 token、不可信内容入站处理姿态 | 儿童产品 + 公网入口（自签证书 + 设备弱 token）+ 儿童语音经阿里云 ASR + OpenClaw chat/completions 现为 full operator 公网可达（ARQ-260901）——challenger 打信任边界与收敛 |
| **11 AI-native / 自动化保全** | 儿童产品运行时不「断在人这里」；不新增逐条人工门 | 提醒/故事/查询/ASR 全自动推进（事件驱动推送，非轮询 = DNA#3）。**与轴 10 对立**：challenger 不得以「儿童产品必须家长逐条审视」无脑加管控；defender 替自动化方辩；取舍交拍板。锚点 = 蓝图 §5 关卡纪律「六门以外一切默认 policy-as-code，不得实现成第七个人工门」 |

**不单独设轴的**（并入上表）：轴 0 已含薄桥红线（下见 §6 必载原则）；「传输方向 B 自洽」分布在轴 1/3/4；「可冻结性切片」分布在轴 2/4/7/8。

---

## 3. 冻结场景（ATAM 质量属性 + pre-mortem）

> 每个场景格式：**触发 → 失效链 → 期望 spec 给出的答案**。challenger 逐场景走；若 spec 对某场景的「答案」缺失或与其余决策冲突，标 finding 并把严重度落 BLOCKING/SHOULD 阶梯。

**S1 · 常驻连接功耗崩塌（可用性/成本，pre-mortem）**
- 触发：设备决策 21「开机即连、常驻连接」+ 18650 便携电池 + 反射屏常显 + 本地 AFE 常跑。
- 失效链：常驻 wss 保活 + 心跳 + 下行音频解码 + AFE 常跑 → 续航熔断 → 设备一天数充 → 家长弃用。
- 期望答案：spec 必须对「常驻连接的功耗预算与降级」有最小可判的描述（哪怕「功耗实测后再看」，也须写明实测的触发点与预警机制），否则与决策 10「便携电池为主」+ 二期唤醒功耗实测构成悬空依赖。

**S2 · OpenClaw 多轮上下文不连续（可用性，pre-mortem）**
- 触发：spec A4「连续追问」依赖 OpenClaw 无状态调用间多轮上下文连续；流式探针只实证单轮（spec §6.3 已标「部署期第一件事」）。
- 失效链：孩子问「那『星』字呢」/「明天有什么课」时网关 LLM 模块每次无状态新建调用 → 上下文断裂 → 回答牛头不对马嘴 → 产品不可用。
- 期望答案：spec 是否把「多轮连续性」写成**冻结前必须满足的前置**还是「冻结后再验证的 DEFER」——若后者，轴 7 要求给 forcing-function + owner + 相位；否则 A4 是幽灵需求。

**S3 · edge-tts 被封（可用性/成本，pre-mortem）**
- 触发：TTS 依赖微软非官方 edge-tts 接口（spec 决策 2/18 已标「可替换 provider、不作唯一依赖」）。
- 失效链：接口被封 → 全部下行语音静默 → 提醒主动播报（决策 27 三件套之一）与故事回听同时失声。
- 期望答案：spec 须给出 TTS 失效时的**降级响应**（换 provider 的触发 + 静默期响应），且要检查「提醒 TTS 合成随推送实时发生」是否使提醒链路对 TTS 可用性有隐性同步依赖（TTS 慢 → 提醒延迟）。

**S4 · 提醒风暴 / 丢失（一致性/并发 + 不变量，pre-mortem）**
- 触发：多档提醒密集到达 + 决策 20 重试到确认（60s 超时重推、单档 ≤3 次）+ 多提醒排队逐个弹。
- 失效链：(a) 设备弹窗驻留期间新提醒按到达顺序排队无上限 → 风暴堆积； (b) 设备关机/断线时回执未回 → 网关重推 → 设备易失 id 集合重启后丢失 → 重弹一次（决策 20 已接受「重启后重弹一次可接受」）； (c) 渲染即确认（KEY/BOOT 只关弹窗、非确认）与「孩子没看」的语义边界。
- 期望答案：spec 须对「风暴上限」「幂等去重」「回执丢失与重弹」三者的不变量有一致声明；challenger 打这些语义之间的互斥（如渲染即确认 vs 排队未渲染不回执）。

**S5 · 自签证书到期（安全/可用性，pre-mortem）**
- 触发：决策 22 wss + 自签长期证书、CA 嵌入固件、换证需改固件重新烧录。
- 失效链：证书到期 → 全部设备 wss 握手失败 → 公网入口瘫痪 → 无 OTA 下的「哑终端」无法远程续命 → 逐台重烧。
- 期望答案：spec 须声明证书生命周期管理与换证触发（「换证需动固件」的运维影响是否被接受并记账），且与「自建 OTA 留二期」的相位（决策内 §6.3）自洽——OTA 未建前证书到期 = 死局。

**S6 · 孩子离线一周再开机（可用性，pre-mortem）**
- 触发：决策 24 不同步事项、设备不缓存任何事项。
- 失效链：离线一周 → 期间三档提醒全部静默作废 → 开机重连 → P1「接下来」/P2 课表按「WS 连接建立」触发器刷新 → 但期间已过期的一次性安排是否clean、本周培训班是否还在，全看 OpenClaw 侧 schedule.md 现状。
- 期望答案：spec 须对「重连后瞬态」有答案——刷新三触发（WS 连接建立 + 每日 0:00 + 新建日程即时推）在「离线一周」场景下的行为，与「过期事项静默作废」是否只对提醒成立、对课表视图也成立。

**S7 · 故事录音误触 / 误判意图（完整性/可用性，pre-mortem）**
- 触发：决策 25 分两条聆听语义——闲聊（VAD 逐句进对话）与录故事（累积到停），开始信号 = OpenClaw 判「我要讲故事」意图后下发 `start_recording`。
- 失效链：(a) 孩子闲聊时提到「我要讲故事」被误判为录音启动 → 整个会话切换为累积态、逐句转写停止； (b) 孩子真想说「讲故事」但被判成闲聊 → 故事丢失； (c) manual 聆听模式无停止确认时误收尾 → 半截故事归档。
- 期望答案：spec 须对「两条聆听语义的意图分流边界 / 误判兜底 / 取消路径」有可判描述（意图分流的判定由谁、判错怎么回滚），否则「误触/误判」无设计好的响应。

---

## 4. 消费方与生产方双向清单（bidirectional consumer list）

> 轴 1 的落实。每个 expose / 事件 producer 必须能对上消费方声明；**留空处 = challenger 的断链打点**。

### 4.1 脑侧 → 设备（下行 expose）

| # | expose / 喂给对象 | 生产者 | 载体 | 消费方声明位置 | 断链/留空点 |
|---|---|---|---|---|---|
| D-1 | E1 display 指令（view: char/schedule/story_list/story_text/text/home/control） | OpenClaw 复文末 ` ```display ` 块 | 网关 mcp `kidpal/display` | spec §3.0.2 E1 / §6.2；契约 §一 | `char` view 的固件端渲染器（LVGL 米字格 + 逐笔动画）尚未建，仅在 M3/M4 计划内——冻 spec 时无消费方实现 |
| D-2 | D3 笔顺 JSON（strokes + medians） | OpenClaw 组（makemeahanzi） | 同上（`char` view data） | spec §3.0.2 D3 / 决策 12；契约 §一① | 固件端 1bpp 贝塞尔离散 + scanline 光栅化（§3.0.2「确定性无技术风险」）无消费方实现面佐证 |
| D-3 | 提醒 alert（含 `speech` 字段） | 网关 push helper（OpenClaw cron 算好 pending.json） | mcp `kidpal/alert`（+随推送 TTS） | spec §4.1 S-F1.4 / §6.1 / 决策 20/23/26/27 | `speech` 的 TTS **何时合成**未定（推送时实时合成 → 对 edge-tts 可用性有隐性同步依赖，见 S3） |
| D-4 | TTS 下行 Opus（24kHz 句级流） | 网关（edge-tts → Opus） | 小智协议 tts/句级流水线 | spec §6.1 / 决策 18 | Pod S MP3 链路退役后的新下行路径，固件侧 decode 消费方 = 小智原生已存在（validation §2） |
| D-5 | P1「接下来」分组日程下发 | OpenClaw 算 + 网关 push | display（schedule/text view） | S-F1.1 / 决策 26 | 刷新三触发中「**新建日程后即时推**」的 producer 断链——谁检测 schedule.md 变更并触发推送，spec 未名之 |

### 4.2 设备 → 脑侧（上行 producer）

| # | producer / 事件 | 生产者 | 载体 | 消费方声明位置 | 断链/留空点 |
|---|---|---|---|---|---|
| U-1 | 上行 Opus 16kHz/60ms 帧 | 固件 ESP-SR 采集 | WS 二进制帧（BinaryProtocol v2/v3） | spec §6.1 | 消费方 = 网关 VAD 切句 → 云 ASR；与 ASR candidate provider（Qwen3-ASR-Flash / Aliyun 流式）的能力对齐仅「部署期核实」 |
| U-2 | 语音喊停 / abort | 固件本地 AEC+VAD | 本地停播 + WS abort 消息 | spec §4.0 / §6.1 / 决策 8 | 本地停播（主）与云端 abort（补充）的语义叠加是否唯一赢家，spec 未裁 |
| U-3 | JSON-RPC 回执（提醒 ack） | 固件弹窗渲染完成 | mcp 回执 | S-F1.4 / 决策 20/23 | 回执丢失 / 设备重启后易失 id 集合丢失 → 重弹的幂等语义已裁但边界需 challenger 复证 |
| U-4 | 故事累积转写（manual 聆听） | 固件 manual 模式上行 | WS Opus + `start_recording` 触发态 | S-F2.2 / 决策 25 | `start_recording` 的意图分流误判兜回路径未定义（S7） |

### 4.3 事件 / 定时 producer（无固定方向）

| # | producer | 触发 | 消费方 | 断链/留空点 |
|---|---|---|---|---|
| E-1 | OpenClaw cron（三档阶梯排程） | 定时扫 schedule.md | 网关（到点 push alert） | cron 已实测可行（validation §7 第三轮）；但「schedule.md 变更 → 重算三档」的变更检测器未名之 |
| E-2 | 「新建日程」事件 | 家长录入 schedule.md | P1 刷新「即时推」 | producer 未名（D-5 同源） |

---

## 5. regression-canaries（challenger 必须独立重新派生的已知弱点线索）

> 每一条是「已知弱点」的**领域线索**，不直接给答案；challenger 须自己从 spec/契约/验证材料重新派生具体 finding，否则辩论判浅。来源逐条标注。

1. **流式请求禁带 `max_tokens`**（来源：`spec-rereview-input-streaming-validation.md` §1/§6 探针 1 实测坑 + spec 决策 18「流式请求禁带 max_tokens」）。challenger 须派生出：这个实测坑是对 type:openai 配置级接入 OpenClaw 的适配约束，spec 是否把它写成了**可被 builder 遵守的硬约束**（而非散落正文的注记），以及契约门重写小智协议剖面节时此约束的落点。
2. **渲染即确认与排队弹窗的交互语义**（来源：validation §7 第二轮 grilling + spec 决策 20）。challenger 须派生出：渲染即确认出队（KEY/BOOT 只关弹窗）vs 多提醒排队逐个弹 vs 设备重启后易失 id 集合丢失 → 重弹，三者叠加下的投递不变量是否真的自洽（如「孩子没看但渲染了」是否算送达）。
3. **按区块判提醒范围的边界**（来源：validation §7 第三轮 human 修订「每周例行不提醒 → 按区块判」+ spec 决策 20）。challenger 须派生出：提醒范围以「区块」而非「时段/频次」划分后，边界案例（周中晚培训班落「每周固定课表」区块 → 仍提醒；节假日补课、学校课程表与每周固定课表的同日重叠去重/分组）是否完整无歧义。
4. **提醒载体通道在 spec 内的命名漂移**（来源：横向对照 spec §3.0.1 E3「小智协议原生 alert 类型」vs §6.1/决策 23「mcp kidpal/alert 工具、小智原生 alert/custom 弃用」+ §6.1「经 custom/mcp 消息下发」）。challenger 须派生出：同一「提醒推送」在 spec 内以两种通道形态出现，属内部矛盾，须定位所有冲突行并判严重度。
5. **v0.1 遗留面（HTTP / faster-whisper 残留）未随方向 B 收敛**（来源：横向对照 spec §3.2 端云职责表 / §3.3 架构图「ASR: faster-whisper」/ §3.4 链路图「经 shim /v1/asr(whisper)→ /v1/chat」 vs §3.0.0/§6.1/决策 9/19 方向 B + ASR 云优先 + faster-whisper 弃选）。challenger 须派生出：spec 前半段（§3.2–§3.4）仍是 v0.1 同步 HTTP 叙事，与更新段互斥，属可冻结性的头号内部矛盾。
6. **spec↔契约 漂移（提醒轮询 vs 推送、remind_before_min vs 三档阶梯）**（来源：`interface-contract.md` §二 frontmatter `remind_before_min` + §2.3「到点前 remind_before_min 分钟生成提醒」+ §三「设备轮询 /v1/alerts → 读到并清除」vs spec 决策 20/23/24「三档时刻配 OpenClaw 侧、B1 零变更、轮询改推送、渲染即确认」）。challenger 须派生出：契约现有文本与 spec 新模型硬冲突，且此冲突虽在契约门在办（CDQ-260902），但**spec 冻结前**契约门未重写完成则命中「契约未批 → spec 不可冻结」铁律（DEV-GOVERNANCE §4 契约门）。

---

## 6. 必载蓝图原则（challenger 判轴 11 与轴 0 的根据）

> 蓝图源 = `c:\AI\AInativecompany\docs\00-company-blueprint.md`（§1 北极星 / §3 七条 DNA / §5 关卡纪律）。KidPal 只建了「开发活动治理」（Part 2），「运行系统的治理」（Part 1 蓝图 / kernel / 六业务门）未立（DEV-GOVERNANCE §1.1），故本 charter 载入的是蓝图**原则层面的 AI-native DNA**，映射到 KidPal 设备/网关/脑三面运行架构，非照搬公司业务门。

1. **北极星（蓝图 §1）——「人只在高风险关卡确认」**：KidPal 运行时（唤醒→聆听→ASR→理解→TTS→显示→提醒推送→故事归档）必须全自动推进，无逐条人工中断；人的确认点收敛为**内容层面**的异步输入（家长经 QQ/改 MD 录课表），而非运行时门。challenger 用此判「是否把某个运行时环节断在人工确认处」。

2. **七条 DNA（蓝图 §3）——本 spec 重点四条**：
   - **DNA#3 事件驱动 pipeline**：提醒 = 定时事件自动推（WebSocket push），非设备轮询；高风险承诺点由人确认、其余 policy-as-code。challenger 判「轮询残留」是否真被清干净。
   - **DNA#5 完整可观测性**：AI 调用与写操作记录 actor/before-after/token/成本，`correlationId` 串因果。**spec 现状缺**——提醒投递、故事归档、ASR/TTS 调用全无可观测性声明；challenger 可打「投递/归档的审计与代价不可观测」为轴 9/11 缺口。
   - **DNA#6 AI provider 抽象**：ASR / TTS / LLM 均可替换、业务不感知供应商。spec 决策 19/18/2 已把三者写成「可替换 provider」前提——challenger 打「可替换」是否真落实（如 edge-tts 的句级流水线是否锁死某 provider 的流式语义）。
   - **DNA#7 zod 结构化输出**：AI 产出经 schema 校验后才落库/下发。**spec 缺口**——display 指令 JSON 块从 DeepSeek 产出后、经网关下发板子渲染，**途中无 schema 校验环节**（契约 §一仅「shim 用正则切分」，未提校验/拒绝脏 JSON）；challenger 据此打「不可信 LLM 内容直入设备渲染器」的信任边界缺口。

3. **§5 关卡纪律（蓝图 §5）——「canonical 关卡且仅六个，六门以外一切默认 policy-as-code，不得实现成第七个人工门」**：映射到 KidPal——提醒的 KEY/BOOT 关弹窗、渲染即确认出队，是**儿童交互**而非人工审批门，不得演变成「家长逐条审批提醒/故事」；设备弱 token 强制鉴权（决策 22）是**机械门**（token 未设 = 拒），不留 validation-mode 后门。challenger 与 defender 在此对撞：轴 10 会主张「儿童隐私需家长管控」，轴 11 会主张「不加逐条人工门」，取舍交拍板。

4. **KidPal 自有架构红线（本轮被重定义后的新形态，spec 内部原则）**：
   - **板端哑终端 / 脑侧集权**（spec §3.0）：所有数据/逻辑/记忆在 OpenClaw；板子只收音/放音/显示/按键 + 本地实时音频处理（AEC/VAD/喊停）。
   - **薄桥红线**（spec §3.0.0）：「零自研业务逻辑」不变；变的是承载形态——HTTP 四端点 → 小智协议剖面 + provider 配置 + 小段 push helper（各约几十行）。
   - **儿童隐私与安全**：儿童语音经境内阿里云 ASR（决策 19，human 接受仅 ASR 链路出内网）；设备弱 token 强制鉴权；故事/课表存 OpenClaw MD。
   - **离线降级红线新形态**（决策 24）：从 v0.1「离线兜底缓存」重定义为「断网无提醒、仅时间/温湿度/电量、设备不缓存事项」。

---

## 7. 已接受前提清单（human 显式裁定，不重新攻击取舍本身）

> 以下取舍已由 human 在四轮 grilling 显式拍板（出处 = spec §7 决策 + `spec-rereview-input-streaming-validation.md` §7），challenger **不得再攻击「该不该这么选」**，但**可攻击其一致性 / 完整性 / 实现含义**（例如「选了自签证书，则证书轮换流程是否在 spec 有 owner」——攻击的是完整性，不是选自签这件事）。

| # | 前提 | 出处 |
|---|---|---|
| P1 | **方向 B 的选择本身**（转 WebSocket + Opus 流式，弃 MVP 同步 HTTP） | 决策 9 + validation §5；「要不要转 B」不重审，只审 B 的三面自洽/完整/实现含义 |
| P2 | **不同步离线**：断网无提醒、设备不缓存事项、P1/P2 不可用仅时间温湿度电量 | 决策 24 |
| P3 | **自签长期证书**：wss + 无域名自签、CA 嵌入固件、换证需重烧、轮换责任自己 | 决策 22 |
| P4 | **提醒重试简单版**：重试到确认——60s 超时重推、单档 ≤3 次、**不做重连即重推**、过期静默作废、渲染即确认出队、易失 id 幂等去重 | 决策 20（第二轮 grilling human 裁） |
| P5 | **全文上屏**：通用问答答案全文滚动上屏（否摘要方案） | 决策 28 |
| P6 | **主动播报三件套**：提醒 = 提示音 + TTS 读出 + 弹窗驻留；alert 载荷带 `speech` | 决策 27 |
| P7 | **提醒时刻值本身**：前晚 20:30 / 当天 07:45 / 事前 15 分钟（全局常量，human 手改） | 决策 20/第四轮 |
| P8 | **ASR 云优先**：一期阿里云云 ASR（Qwen3-ASR-Flash / Aliyun 流式二选一），儿童语音经境内云，本地 SenseVoice 兜底 | 决策 19 |
| P9 | **官方 Python 版小智服务端**（Go 社区版弃选）+ provider 配置级接入 OpenClaw / edge-tts | 决策 18 |
| P10 | **常驻连接 + 省电不预设优化**（功耗实测后再看） | 决策 21 |

> 说明：P4 的「不重推」取最终裁定语义 = 决策 20 的「不做重连即重推 / 无无限重推」，非第一轮 grilling 中被第二轮推翻的旧「推送成功即送达」表述；challenger 引用时以 spec 决策 20 最终文本为准。

---

## 8. 分支 / 主题划分（供 moderator 逐支驱动辩论）

> 按轴聚成 5 个主题支；moderator 逐支派 challenger ↔ defender 对抗，adjudicator 逐支裁决，pass-oracle 逐支过。

| 支 | 主题 | 覆盖轴 | 核心打点 |
|---|---|---|---|
| **T1** | 端云边界与哑终端纯度 | 0 / 5 | 板子持有状态 vs「不存数据」红线；实时音频本地 vs 业务不落地；提醒易失 id、P1 缓存、SD 缓存的边界自洽 |
| **T2** | 传输方向 B 的三面对齐 | 1 / 3 / 4 | 固件（小智原生 WS/Opus）↔ 服务端（开源服务端 provider 配置）↔ 契约（小智协议剖面节）三面一致性；mcp vs alert 通道漂移；hello 协商/鉴权/回执是否进契约；v0.1 遗留面（§3.2–§3.4）收敛 |
| **T3** | 提醒投递状态机 | 6 / 9 / 11 | cron 排程→pending.json→push→回执出队→重试/幂等/排队 的 owner 与不变量；「新建日程即时推」事件断链；渲染即确认 vs 排队语义；S4/S6 场景 |
| **T4** | 安全与儿童隐私信任边界 | 10 ↔ 11 | 设备弱 token 强制鉴权 + 自签证书轮换；chat/completions full operator 公网收敛（ARQ-260901）；儿童语音经云 ASR；display JSON 无 schema 校验直入板子；家长管控是否变第七人工门 |
| **T5** | spec 基线健全性 + 可冻结/可实现切片 | 2 / 7 / 8 | v0.1 遗留（HTTP/faster-whisper/里程碑 M1-M2）；开放项 forcing-function（家长端形态、多轮连续性、ASR provider 终选、功耗实测）；幂等/格式遵循度可证伪验收；BLOCKING 内部矛盾是否清到可冻结 |

---

## 9. 冻结声明

本 charter 一经写盘即对 challenger / defender **只读冻结**。任何一方要改判据 / 轴 / 场景 / canary / 前提，必须经 moderator 重新派 scenario-author；不得在辩论中自行解读本 charter 未载的轴。

---

*charter 冻结：2026-09-02 · scenario-author*