# KidPal 架构（architecture）——「怎么造」的单一事实源

> 自 `docs/spec.md`（v0.2 FROZEN）拆分落地（2026-09-13，`260913-arch-review-spec-split`，Track B FROZEN v1.1）；spec 原件归档于 [archive/spec.md](archive/spec.md)，仅作溯源。
> 读者：architect / planner / dev-loop 各 Pod。需求面（产品是什么）见 [requirements.md](requirements.md)；数据格式权威见 [contracts/interface-contract.md](contracts/interface-contract.md)（契约门）。
> **文档分工（2026-09-14 重构定）**：本档 = 系统**结构与机制**的单一事实源；架构决策本体 = [ADR.md](decisions/ADR.md)（本章 §10 只留索引 + MVP 时代决策历史档）；数据格式 = contracts/；需求与阈值 = requirements.md；术语 = [glossary.md](glossary.md)。细节一旦在两处复制即开始腐烂——本档对上述各面只放指针。
> **锚稳定规则**：本文件小节编号发布后视为稳定标识——requirements.md 的 story/NFR 支撑锚按号引用；改编号 = 有意识地同步改锚（`node .claude/scripts/spec-anchor-check.mjs` 机检抓悬空）。
> **定名（2026-09-13）**：**MVP** = 已开发完成、经真机验证的交付基线；**Phase 1** = 即将开发的下一版（需求 = requirements.md v1.49+）。「二期」= Phase 1 之后。
> **现状标注约定**：【已实证】= live/真机验证过的事实；【修复项】= 已登记的 Phase 1 dev 修复项；【二期】= 明确后置。

---

## 1. 背景与上下文（系统活在什么世界里）

> ← 收编自旧 §2.1/§2.5，按 C4-L1 重组；外部依赖清单与范围声明为本轮新增。

### 1.1 系统上下文图（C4-L1）

```
                    ┌──────────┐   QQ 语音/文字        ┌──────────────┐
                    │  家长     │ ───────────────────▶ │   OpenClaw    │◀──┐
                    └──────────┘  （录入课表/查日志）     │  （编排大脑）  │   │
                                                          └──────┬───────┘   │
 ┌──────────┐  按键/语音            ┌────────────┐ wss  │ OpenAI 兼容   │
 │  孩子     │ ──────────────────▶ │ ESP32 设备  │ ─────┼──▶（见 §2.2 端云四层链路）
 └──────────┘  看屏/听播报          │ （常显终端） │◀─────┘
                                    └────────────┘
 外部服务（设备/云端经其完成能力）：
   阿里云 ASR（Qwen3-ASR-Flash）· 阿里云 DashScope embedding（text-embedding-v4，记忆画像）· edge-tts（微软非官方接口）· DeepSeek（OpenClaw 内；含 Honcho deriver 抽取）
   makemeahanzi 字库（静态数据）· 小智官方服务端镜像（xinnan-tech，容器运行时）· Tavily（OpenClaw 平台联网搜索，实时事实查询）· Honcho 自托管记忆子系统（ECS 本机五容器）
```

### 1.2 外部依赖清单

| 依赖 | 协议/形态 | 可用性假设 | 风险归属 |
|---|---|---|---|
| 小智官方服务端镜像 `ghcr.nju.edu.cn/xinnan-tech/xiaozhi-esp32-server:server_latest` | Docker 容器（引擎服务运行时） | 镜像由官方演进；**镜像升级 = engine-patch 漂移风险** | 部署副本漂移治理（ARQ-260907-engine-deploy-patch-drift，§8） |
| 阿里云 ASR（Qwen3-ASR-Flash，DashScope 流式） | 容器内 provider，凭据 env `DASHSCOPE_API_KEY` | 按量计费 + 免费额度；额度尽/流式不达标 → 切本地 SenseVoice（决策 19，配置级切换） | 出网路径①（NFR-SEC-2 记账） |
| DeepSeek（deepseek-v4-flash） | OpenClaw 内部 provider | OpenClaw 侧配置，公网可达 | 出网路径②（Honcho deriver 抽取复用同域，NFR-SEC-2 ⑤） |
| 阿里云 DashScope embedding（text-embedding-v4） | 记忆子系统容器内 provider，凭据沿用 `DASHSCOPE_API_KEY` | 按量计费；额度尽 / 不达标 → 画像管线降级（不阻塞对话） | 出网路径⑤（NFR-SEC-2 ⑤ 记账；与 ASR 同供应商同信任域） |
| Honcho 自托管记忆服务栈 | Docker 容器（五容器；api `127.0.0.1:18100` loopback；postgres + pgvector 持久卷） | 与 ECS 同机共生；故障 → 画像注入缺失、对话照常（§6.7 降级） | 部署面运维（§5.3.2）；数据本机不出网（NFR-PRIV-1） |
| edge-tts（zh-CN-XiaoxiaoNeural） | 引擎容器内 provider | **微软非官方接口，有被封运维风险**，不作唯一依赖；被封 → 提醒降级「仅弹窗+提示音」仍必达（§6 降级） | 出网路径③ |
| Tavily（联网搜索） | OpenClaw 平台联网搜索 provider（平台承载，非自建搜索链路） | kidpal agent 实时事实查询回合（天气/新闻/今日热点类）消费 | 出网路径④（NFR-SEC-2 ④ 记账，v1.62 增） |
| makemeahanzi 字库 | 静态数据（9574 字 / 32MB），已在服务器 `chardata/` + 预热 `char_cache.json` | 静态、无在线依赖 | 无 |
| QQ（家长通道） | OpenClaw 对话通道 | 家长 QQ 在线 | 录入即时性（兜底 = 直改 MD 下次重算生效，S-F1.5） |
| 阿里云成都 ECS（8.137.91.16） | 全部云端面的宿主 | 单机；实测余量约 1GB 内存 / 19G 磁盘（2026-09-02；ASR 云优先后本机只扛 front+引擎+TTS） | 单点；扩容 human 裁（§5.3） |

### 1.3 范围声明（明确不在本系统内）

- 家长管控与小程序家长端——**不做**（2026-09-14 裁，家长知情权 = OpenClaw 侧完整日志）；
- 温湿度云端记录与家长侧视图——**不做**（仅本地直读上首页）；
- 语音喊停（一开口即停）——**二期**（依赖本地 AEC+VAD 阈值实测，不挡 Phase 1；KEY 单击打断为 Phase 1 唯一打断手段）；
- OTA——Phase 1 非目标（证书生命周期相位对账见 §8/决策 22）；
- 机械内容过滤层（policy-as-code）——留作日后合规选项（决策 28）；
- 多设备/多用户——MVP 单设备（全设备共享单 token，多设备时转每设备唯一并记账，决策 22）。
- 多设备 / 多孩子**记忆身份体系**——本批不做；已验证形态 = **设备级 session**（session key 家族 `agent:kidpal:openai-user:<device_id>`）+ **家庭单 owner 画像**（Honcho peer = `owner`，不按设备隔离）；换设备 = 新 session 家族（会话连续性重置），画像不变；身份体系另立（§6.7 / OQ-6 留根）。

---

## 2. 架构总览（一页读懂全貌）

> ← 收编自旧 §2.1/§2.5 + 决策 1/2/3/9/18/20/22/23/24 原则抽取。

### 2.1 顶层定位

**设备 = 哑终端（「会说话的显示器」），OpenClaw = 编排大脑。** 设备只负责收音/放音/显示/按键，不理解业务、不做智能、不承载业务真相源（story 正文仅作板端只读镜像缓存，见 §6.2）；所有数据、逻辑、记忆在云端。设备与云端之间用**约定的数据格式**（契约 E1 display 块）通信——课表存 MD、故事存 MD、笔顺由云端按格式输出，设备拿来就画。

### 2.2 容器级总图（C4-L2，端云四层链路）

```
【设备端】ESP32-S3（小智 V2.1.0 基座固件）
   │ wss://ECS:8000（TLS + 强制设备鉴权；小智协议：hello 协商 + Opus 帧 + JSON）
   ▼
【接入网关】server/gateway/front/front_app.py（唯一公网入口，TLS 终结 + 鉴权权威）
   │ ① 设备 ws 双泵代理（上行透传 / 下行截获 display 侧通道）
   │ ② plain ws ▶ loopback 127.0.0.1:8010
   ▼
【引擎服务】xiaozhi-esp32-server 官方容器（Docker host 网络 + engine-patch 部署副本补丁）
   │  ASR（阿里云）· VAD 切句 · TTS（edge-tts）· Opus 下行
   │  display fence 剥离（补丁状态机）→ 侧通道帧回 front
   │  LLM type:openai ▼ loopback 127.0.0.1:18789（每轮：user=<device_id> + 本回合 + 动态 context 块）
【编排大脑】OpenClaw（持久会话 + 记忆画像 + 工具 + cron）
   │  意图/多轮对话（服务端持久会话，按 user 标识；引擎只发新回合）/ 业务 MD 读写 / QQ 录入
   │  记忆画像：Honcho 自托管侧车（loopback 127.0.0.1:18100）deriver / representation
   │  提醒：cron 重算（openclaw-patch 确定性脚本）写 alerts/pending.json
   ▼
 front 每分钟轮询 pending.json → mcp kidpal/alert 推送设备
```

### 2.3 端云职责划分（实时本地 · 语义云端）

| 能力 | 位置 | 说明 |
|---|---|---|
| AEC 回声消除 / VAD / 唤醒词（WakeNet） | **本地**（ESP-SR AFE，跑核 1、内存放 PSRAM） | 实时性要求；唤醒 = 「你好，小智」双入口之一（Phase 1，MVP 期间已实测） |
| 喊停/打断检测 | **本地**（AEC+VAD）【二期】 | Phase 1 打断 = KEY 单击 + `abort` 消息云端取消生成 |
| 会话管理 | 本地状态机 | 基于小智 ToggleChatState 改；60s 问答会话超时（§5.1 会话状态机；记录会话不适用 60s——v1.65 起其超时语义 = 网关侧卡壳引导 2 轮无回应自动收尾，原 120s 设备侧阈值作废，见 §5.2 场景④） |
| ASR / 意图理解 / TTS / 业务与存储 | **云端** | 阿里云 ASR → OpenClaw+DeepSeek → edge-tts；课表/故事/查字全在脑 |
| display 校验权威 | **front** | 引擎侧剥离、front 侧机械校验后下发（「引擎为主剥离、front 为校验/relay 权威」分工） |

### 2.4 五条架构原则

1. **哑终端**：设备零业务、零智能、业务数据零真相源存储（story 正文只读镜像为窄例外，见原则 5）；
2. **唯一公网入口 = front**，引擎与 OpenClaw 不外露（18789 锁 loopback，决策 9/18/22）；
3. **网关恒薄**：front 只做终结/鉴权/校验/relay/投递，业务逻辑 100% 在脑（决策 20「脑算网推」）；**同型切分注记（`261019-p3-record-dialogue-guide`，2026-10-19）**：记录语境对话态的卡壳引导与收尾忠实整理 = front 承载计时/计数/触发与指令装配（机械层）+ 脑侧一次性 LLM 回合生成（语义层）——ADR-002「可机判性切分」混合分层同型，推理与生成 100% 在脑，本原则语义不变（决策本体 = ADR-013）。
4. **实时本地 · 语义云端**：音频信号处理（AEC/VAD/唤醒）必须本地，语义才上云（决策 3）；
5. **介质禁令**：板端非易失介质仅存设备凭证、预栅格化资产与字体、固件本体；业务/身份/事项状态只驻 RAM（决策 24）。**窄例外（`261016-p3-story-body-sd-cache`，2026-10-17）**：SD 卡可为 **story 正文**留**只读镜像**（云端 story 归档仍为唯一真相源、板端不承载任何写入，见决策 24 注记 / §6.2）；提醒 / 课表 / 日志等其余业务数据仍不落板——此例外**只给 story 正文**、不扩及任意业务数据。 **介质白名单增列（`261018-sd-log-persist`，2026-10-18）**：SD 卡可存**设备诊断日志**（设备自身运行日志，常开 + 有界轮转、目录总量 16MB 封顶、可删除，见 §6.6 / §6.2 / 决策 24）——属可观测性数据、非业务真相源、不提供离线功能；本句「日志」= **业务日志**（家长知情权），业务日志仍不落板、语义不变。

---

## 3. 架构目标与约束

> ← 收编自旧 §1.1 关键约束 + 决策 7/8/10/16/19/28；假设清单为本轮新增。

### 3.1 质量属性目标（可验证表述；阈值权威 = requirements.md §5）

- **不冷场**：孩子说完到设备开口，等待超 4 秒先播安抚句（NFR-PERF-1）；
- **查字延迟**：问「X 怎么写」到**查字窗弹出（完整示范字可见）**目标 5 秒档（NFR-PERF-2；【修复项】实测冷启动约 109 秒，见 §8）；
- **传输安全**：公网入口强制鉴权，不留 validation-mode 后门；wss + 自签长期证书（NFR-SEC-1）；
- **儿童数据出网**：五条路径（音频→阿里云 / 转写→DeepSeek / 回复→edge-tts / 实时事实查询→Tavily 联网搜索 / 记忆画像管线→deriver 复用 DeepSeek + embedding→DashScope）human 已记账接受（NFR-SEC-2；第④条 v1.62 增、第⑤条 v1.67 增）；
- **离线边界**：断网仅本地时间/温湿度/电量，不缓存任何事项（story 正文只读镜像为窄例外，NFR-OFF-1 注记）；
- **内容安全**：答案口语化、简洁不啰嗦、适合 8–12 岁（生成端 system prompt 约束 + 失效截断兜底，NFR-SAFE-1；不设硬性句数上限——human 2026-09-25 裁）；
- **1bpp CJK 可读性**：14px 真机可读性下限已于 2026-09-15 九档对照实测定案，并于 2026-10-28 复测；本批正文 / 列表上调 16px、阅读标题 20px，键帽条 / 辅助条 / 查字窗维持 14px（NFR-READ-1，ARQ-260831-small-cjk-1bpp-readability 已结项）。

### 3.2 硬约束

- 屏幕 400×300、1bpp 纯黑白无灰度、反射式无背光（暗处不可见）、刷新快（~45-70 FPS，可动画/滚动/笔顺演示）、ST7305 局部窗口更新可优化功耗；
- 可用功能键 = KEY（GPIO18）+ BOOT（GPIO0）两个；PWR 为硬件电源电路键、固件不可读；multi_button 支持单击/双击/长按；
- 18650 便携电池为主，唤醒待机功耗实测挂 Phase 1（决策 10）；
- 实时音频处理必须本地（ESP-SR），不等云端（§2.4 原则 4）；
- 模拟器与固件 LVGL 版本必须对齐（锁 9.3.x）。

### 3.3 假设清单（假设被打破时的后果）

| 假设 | 被打破时 |
|---|---|
| OpenClaw 输出遵循 display 契约格式 | 遵循度实测 12/12（validation-batch）；失效 → front `display_schema` 机械校验兜底：脏 JSON 丢弃/降级友好提示，不入板盲渲染（V27） |
| edge-tts 可用 | 被封 → 提醒降级「仅弹窗+提示音」必达不阻塞（V20 不对称性）；问答侧换 provider（配置级） |
| 云 ASR 配额充足且流式达标 | 额度尽/不达标 → 升级 ECS 切本地 SenseVoice（决策 19，无返工） |
| 设备常驻在线（提醒推送可达） | 引擎侧两道超时已修正为 24h 兜底（决策 21）；已裁：永不休眠（决策 29）；重连 + 常驻重建已落（261010 包） |
| 单设备单用户 | 多设备 → token 转每设备唯一并记账（决策 22 已预留） |
| 引擎镜像版本与部署副本兼容 | 镜像升级 → bind-mount 补丁语义漂移；唯一机械报警锚 = 启动标记行（§5.3.3） |

---

## 4. 质量属性 → 架构对策映射（需求↔架构接缝）

> 本轮新增。每条 NFR 指到满足它的机制；机制详情在所指小节。

| NFR（requirements §5） | 架构对策 | 机制位置 |
|---|---|---|
| NFR-PERF-1 不冷场 ≤4s | front 安抚定时器 `SOOTHING_TIMEOUT_MS=4000`（LLM 首 token 超时先推固定安抚句）+ 设备状态机 `THINKING_TIMEOUT_4S`→SPEAKING（COMFORT 标记）【状态机接线 = 修复项】 | §5.1 front 组件 / §5.1 会话状态机 |
| NFR-PERF-2 查字 5 秒档 | OpenClaw 侧 `warm_char_cache.py` 预热 9574 字全量 `char_cache.json` + char_stroke hold-and-replace 缓存；【修复项】live 冷启动实测 ~109s，流式/异步优化待做 | §5.1 OpenClaw 补丁组件 |
| NFR-PERF-3 记录会话卡壳引导时机 | front 卡壳计时器族（每句讲完起计时、接着说重置、计时到或明示卡壳词命中触发引导 LLM 回合、连续 2 轮无回应自动收尾；阈值权威 = requirements NFR-PERF-3〔待定、真机调优后立项拍数〕）+ `StoryAccumulator` 卡壳计时与引导轮次纯决策 | §5.1.2 front/story.py 组件行 / §5.2 场景④ |
| NFR-SEC-1 强制鉴权 + 传输安全 | front TLS 终结（自签 RSA-3072 20 年）+ `websocket.accept()` 之前强制 `gw_auth.authorize`（constant-time、token ≥22 字符、未设拒服务）+ OpenClaw 18789 锁 loopback + 引擎层 `auth: false`（鉴权唯一权威在 front，260911 裁） | §6.1 / §5.3.2 |
| NFR-SEC-2 出网记账 | 五条出网路径（§1.2 表；第④条 = 实时事实查询回合经 Tavily 联网搜索，v1.62 增；第⑤条 = 记忆画像管线 deriver→DeepSeek / embedding→DashScope，v1.67 增）+ 决策 19 记账 | §1.2 |
| NFR-PRIV-1 记忆边界与保留 | 记忆写入范围（类级规则：平台内部系统回合不入画像；设备流程回合与孩子真实聊天统一归属、按内容参与记忆；家长侧回合列在边界外——机制实况与残余见 §6.7 残余申报）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工 | §6.7 / §5.1.2 记忆子系统 / §5.4 记忆行 |
| NFR-OFF-1 离线边界 | 设备不缓存事项/课表 + 介质禁令（白名单制）+ 断网仅本地时间/温湿度/电量 +RAM 渲染缓冲不禁（掉电即失，§6.2 澄清；story 正文只读镜像窄例外，§6.2） | §6.2 / §5.4 |
| NFR-SAFE-1 内容安全 | system prompt 简洁不啰嗦纪律（回合路由表承载，不设硬性句数上限；现行版本以 §5.5.5 现状指针为准）+ front 兜底文案与 V32 截断（超限按有处理的失败模式） | §5.1 front 组件 / §5.5.5 prompt 版本化 |
| NFR-READ-1 1bpp CJK 下限 | `lv_font_conv` 生成 assets 分区 `.bin`，经 cbin 运行时装载；14px 下限已由九档真机对照实测，编译内 14px 字体兜底 | §5.5.3 字体管线 |

---

## 5. 架构视图

> 每视图回答一个问题：逻辑=分几块各管什么；运行=场景怎么流；部署=跑在哪；数据=长什么样谁拥有；开发=怎么造怎么验。

### 5.1 逻辑视图（组件与职责）

> ← 收编自旧 §2.4/§2.6/§2.7/§2.8/§2.9/§2.10，按组件归属重组；实现现状锚来自 dev 包与代码调研（2026-09-14）。

#### 5.1.1 设备端组件（firmware/main/）

| 组件 | 文件 | 职责与现状 |
|---|---|---|
| 应用主循环 | `application.cc` | WS 协议装载、OnIncomingJson 分发（tts/stt/llm/mcp/system）、主线程 Schedule 队列、OTA、abort 发送 |
| **会话状态机** | `session_state_machine.cc` | 6 态（IDLE/LISTENING/THINKING/SPEAKING/MANUAL_RECORDING/ENDED）×11 事件（KEY_SHORT_PRESS/BOOT_SHORT_PRESS/END_INTENT/SESSION_TIMEOUT_60S/UTT_START/REPLY_READY/SPEAK_DONE/ABORT_SPEECH/THINKING_TIMEOUT_4S/START_RECORDING/STOP_RECORDING），表驱动纯逻辑、头注释自证「绝无真实 timer」。⚠️【修复项】`ses_machine_dispatch` 生产代码零调用——真机 60 秒自动关闭从未发生，Phase 1 接 timer 生产者（requirements §2.1 对账；录音态例外 = 不适用 60s 会话超时；自动收尾 = 网关侧卡壳引导 2 轮无回应——requirements v1.65 取代 v1.46「120s 自动收尾归档」口径） |
| 设备状态机 | `device_state_machine.cc` | 小智原生态机（idle/connecting/listening/speaking…），`SetStatus` 仅映射三态——思考态不渲染【修复项，见 §5.5.4】 |
| **渲染节点层** | `display/kidpal_render_node.cc` | 归一屏收口（唯一顶层屏）、页循环枚举 `HOME→CHAR→SCHEDULE→STORY`（注意：与 spec P1→P2→P3 页序漂移，撤 P4 后改 3 页【修复项】）、`cycle_next()` |
| **层栈** | `display/kidpal_layer.c` | 前台层栈单一权威：CHAT(1 常驻非栈)/LOCAL_PAGE(2)/VOICE(3)/SERVER_PAGE(4)/REMINDER(5)/PROVISION(6)，数值大者置前；push/pop/top 纯逻辑 host 可测。覆盖层 BOOT 隔离、提醒最高层、查字窗均落在本层栈语义上【BOOT 不翻页 dispatch 分流 = 修复项】 |
| 页面组件 | `home_p1.c` / `schedule_page.c` / `story_page.c` / `char_page.cc` / `reminder_page.c` | 各页渲染与页内输入消费；story_page 含列表/阅读/录音（v1.65 增引导播报中子态）/整理过渡（v1.65 新增，契约 §1.4 `start_organizing` 触发）/盖章态；reminder_page 含设备侧四态（RECEIVED/QUEUED/RENDERED/ACKED） |
| view 路由 | `display/view_router.c` | 7 view 白名单路由（text/home/control/char/schedule/story_list/story_text），UNKNOWN 拒绝 |
| 板级层 | `boards/waveshare-s3-rlcd-4.2/` | CustomBoard 初始化序（I2C→codec→传感器→按键→LCD→art→渲染节点）；`custom_lcd_display.cc` 私有 RLCD 1bpp 驱动（非 esp_lcd 组件）；ADC 电池死成员【修复项：三处未接线，恒显满电】 |
| 按键路由 | `key_gesture_parser.cc` + 板装配层 | 每键一个 parser 实例；dispatch 落在 iot_button 回调经 `key_route_global` **硬路由**产 GLOBAL_ACT_*（KEY CLICK=SESSION_TOGGLE / KEY LONG=HOME / BOOT CLICK=PAGE_NEXT / BOOT LONG=故事翻屏特例）。⚠️【修复项】全局硬路由不看当前层——「覆盖层 BOOT 不翻页」「KEY 长按返回上一层」「单双击消歧」等 Phase 1 键位改造落点（requirements §2.3 ⚠️ 对账） |
| 音频链路 | `audio/`（audio_service / codecs/box_audio_codec / processors/afe_audio_processor / wake_words/afe_wake_word / opus_*） | ES8311(出)+ES7210(入) 24kHz、AEC 参考信号、esp-sr ~2.2（AFE/WakeNet/VAD）、Opus 16k 上行 / 24k 下行 |
| 协议层 | `protocols/websocket_protocol.cc` | NVS namespace "websocket"（url/token/version）读用；hello 协商（features: aec?/mcp:true，audio_params opus/16k/60ms）；BinaryProtocol3 二进制帧 |
| MCP 工具面 | `mcp_server.cc` | `kidpal/display`（view+data → 0-6 路由 → 渲染节点 begin/end + cache_* 供 cycle_next）；`kidpal/alert`（alert 解析 → 四态入队 → 渲染 → `EmitRenderReceipt` 回执）；通用工具（音量 `self.audio_speaker.set_volume` / 亮度等）【音量语音调节的板端执行面落点，requirements v1.44】 |
| 资产 | `assets.cc` + `display/art_assets.c` | assets 分区 mmap + art SPIFFS 分区（`/art` VFS，KAP1 1bpp 美术），失败降级占位矩形 |

#### 5.1.2 服务端组件（server/）

| 组件 | 文件 | 职责与现状 |
|---|---|---|
| **front 网关** | `server/gateway/front/front_app.py`（+ `server/gateway/` 冻结模块） | 唯一公网入口：TLS 终结（uvicorn ssl，自签 20 年证书）、HTTP upgrade 层强制鉴权（accept 之前，失败 403）、路由（GET /healthz、POST /command、POST /adaptation/dryrun、WS）、**ws 双泵**（上行透传；下行截获 `kidpal_display_blocks` 侧通道帧 → 机械校验 → 逐块 mcp 帧下发设备）、**故事面**（上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete`/`kidpal_story_discard` 整条消费不下透引擎；`source_page=story` 切记录语境对话态（v1.65 取代「累积态」）；stt 帧喂累积缓冲（只回显、不逐句进对话）+ 卡壳计时器族（每句讲完起计时〔阈值权威 = requirements NFR-PERF-3〕、接着说重置、计时到或明示卡壳词命中触发引导 LLM 回合 + `kidpal_speak` 注入播报〔ADR-003 同型〕，返回空/收声抑制 = 本轮不播报仍计轮，连续 2 轮无回应自动收尾）+ 记录语境等价信号注入引擎侧抑制 LLM（ADR-013）；收尾检出 → 下发 `start_organizing` → 一次性忠实整理 → 归档 + 下发 `story_text(seq)`；`story_list` 三时点自推〔① WS 建连恢复推送（`data.restore` 标记、只填缓存不抢前台——requirements v1.63 赎回，additive 字段已随契约门落（2026-10-12 human 批准，随 261010-p3-storylist-cold-start-restore 同批原子落），沿革与语义权威 = §5.2 场景④）② 归档后 ③ 删除后〕（沿革：建连即推曾于 260928-story-connect-push-defer 摘除）；relay 收紧引擎侧 `story_*` 块；卡壳计时 sweep（取代 120s 超时 sweep；pending-stop 挂起等转写窗口保留〔261001-m4-k3-story-archive-order〕）+ 引导/整理一次性 LLM 回合发起（机械触发归网关、语义生成在脑——ADR-002 混合分层同型，ADR-013））、提醒轮询循环；**引导 / 整理一次性 LLM 回合携 `user=<device_id>` 落同一设备 session 键族**（2026-10-24 R7/OQ-4；§6.7 按内容归属：孩子原话照常参与记忆，包装指令与模型回复的残余见该节残余申报）；**课表摘要注入**（会话建立时把 `schedule.py` 派生的课表文本经内部通道注入引擎，供动态 context 块消费——§5.2 场景②） |
| 鉴权 | `gateway/auth.py` | `authorize` 纯函数 + `secrets.compare_digest` 常时比较 + `TOKEN_MIN_CHARS=22`；token 来自 env，未设 = 拒绝服务，无 validation-mode 后门 |
| display 校验 | `gateway/display_schema.py` | 7 view 白名单 + `MAX_DATA_BYTES=65536` + control 动作分表（拒转 `end_session`；放行 `stop_speaking`/`start_recording`）——display 契约的机械执行点（V27） |
| mcp relay | `gateway/mcp_relay.py` | display/提醒统一 JSON-RPC `tools/call`（kidpal/display、kidpal/alert）；数字 id（时间戳+单调序号）；渲染回执消费 |
| 兜底 | `gateway/fallback.py` | 4s 安抚定时器、错误兜底文案、V32 截断 |
| provider 适配 | `gateway/provider.py` | OpenClaw/ASR/TTS 配置承载；`FEATURE_STREAMING_FORBIDS_MAX_TOKENS=true`（决策 18 硬约束） |
| 课表解析 | `gateway/schedule.py` | B1 三区块 MD 纯解析（`parse_schedule_md`/`items_for_date`/`group_upcoming` 分组）——**唯一解析器**，openclaw-patch 复用不自建第二套 |
| 故事归档 | `gateway/story.py` | `archive_story` 落盘 `<stories_dir>/<id>.md`（哨兵头 + kidpal-id/date/title）+ 持久流水号账本 `.seq.json`（`seq=max+1`、只增不减）；`list_stories` = 最新 ≤16 条窗口 + `total` 全量现存篇数；`story_text` = 三键 + 可选 `seq`（additive）；`delete_story` 硬删（哨兵头校验 + 越权/穿越拒绝 + 审计）；`is_finish_phrase` 收尾词检测 + 明示卡壳词表检测（同构子串扫描，词表 = 实现期可调常量）+ `StoryAccumulator` 卡壳计时与引导轮次纯决策（每句活动时刻重置计时、计时到 = 引导触发、连续 2 轮无回应 = 自动收尾、被抑制/返回空轮照常计轮——取代 `timed_out_sessions` 120s 纯静默决策；pending-stop 挂起窗口保留）+ 忠实整理输入装配（只取孩子的话；引导语与问答滤除） |
| 提醒投递 | `gateway/reminders.py` | AlertDispatcher：每分钟轮询 pending.json → 到点推送 → 60s 超时重推 ×3 → 过期作废；`MAX_PENDING=64/MAX_TRACKED=256` |
| **引擎部署副本补丁** | `server/deploy/engine-patch/`（connection.py 0.9.6 改 + kidpal_display_strip.py） | 官方镜像不动、bind-mount 原位覆盖容器内 `core/` 两文件：fence 状态机三态（normal/tag/fence，未闭合 fence 裸载荷进 blocks 绝不进 TTS「宁默不念 JSON」）；单一剥离点（TTS 流与全文共享状态机）；对话历史存剥离前原文；`kidpal_speak` 指令认领 → 复用引擎既有音频链出音（ADR-003）；**启动标记行 `kidpal display-strip patch active` = 补丁在位的唯一机械报警锚**；**动态 context 装配**（本回合时间 / 日期 + front 注入的课表摘要拼入每轮动态块，缓存边界之下、不含静态纪律） |
| **OpenClaw 侧补丁** | `server/deploy/openclaw-patch/` | 提醒生产侧混合分层（ADR-002）：`reminder_schedule_lib.py` 确定性脚本（三档算术 20:30/07:45/前 15min + 幂等 id 确定性散列 ≤40 字符 + `pending.json` 原子写 + 坏输入降级）；语义判断 = agent 回合产 `semantic.json`（缺/坏降级机械兜底文案）；`char_stroke_lib.py` + `warm_char_cache.py`（9574 字缓存）；`make_openclaw_patch.py`（OpenClaw dist hold-and-replace 补丁，drift-guard） |
| **记忆子系统（Honcho 自托管侧车）** | `server/deploy/honcho/**`（compose + 配置 + apply/rollback + 健康检查） | peer / session / message 存储、deriver 抽 observation、representation 生成、`before_prompt_build` 画像注入；五容器全部绑定 127.0.0.1（api 18100）；写入范围与残余见 §6.7；与引擎 / OpenClaw 同机共生、数据本机不出网 |
| OpenClaw 本体 | ECS 部署（2026.8.1） | 完整 agent（意图/多轮/工具/QQ 录入）；**对话历史 = 服务端持久会话**（按 `user` 标识，ADR-015）；**记忆画像经 Honcho 插件**（§6.7）；**静态语义承载 = workspace `AGENTS.md`**（§5.5.5）；原生 cron（提醒 15min 重算 + 录入即时触发） |
| shim（已退役） | `server/shim.py` | HTTP 四端点台阶，头部 ARCHIVED（2026-09-05 退役）；`kidpal-shim.service` 为历史单，ECS 侧停服 + 8080 安全组关闭待清【遗留待办】 |

#### 5.1.3 组件依赖方向

设备协议层 ⇄ front（唯一对外面）；front → 引擎（明文 ws；服务端内部注入：`kidpal_speak` / 记录语境等价信号 / 课表摘要）→ OpenClaw（OpenAI 兼容；每轮 `user` 标识 + 本回合 + 动态 context 块）；front ↔ 提醒交接目录（pending.json/semantic.json）；openclaw-patch 脚本 ← 复用 gateway/schedule.py 解析器（单源）；设备渲染层 ← MCP 工具面注入（view 路由单向）。

### 5.2 运行视图（关键场景端到端）

> ← 收编自旧 §2.6 一条扩为五条。证据状态（缺口详见 §8.1）：① 开机常显 live 实证；② 语音问答 live 全通；③ 查字 live 已复验恢复（2026-09-15 真机实证——板端两缺陷定位见 §8 风险债 5）；④ 记录归档 host 验收过、live 面部分在办；⑤ 提醒 live L3 投递绿、L4 渲染回执留红。

**① 开机与常显**：上电 → WiFi（NVS 凭据 / 配网）→ WS 建连（front TLS + 鉴权）→ hello 协商 → 常显 P1 首页。「接下来」刷新三触发 = WS 连接建立 + 每日 0:00 跨天 + 新建日程后即时推（决策 26；producer = QQ 录入路径 OpenClaw 自改写即时重算+推送，直改 MD 靠连接/跨天兜底）。P2 周网格缓存恢复 = 三触发（与「接下来」同源同机）的附带推送：WS 建连、每日 0:00 跨天与录入 marker 时，front 在推「接下来」后附带重推周网格**恢复性载荷**（契约 §1.3② `data.restore: true` 语义：设备只填 RAM 网格缓存、不接管前台，P2 在屏时仅重绘本页）。决策 26 三触发表述只覆盖 upcoming，网格恢复面由 ADR-005 补齐；语音「帮我显示课表」意图薄载荷展开仍为主动展示、无条件接管前台（M3-K3 冻结行为）。断网或推送未达期间翻到 P2 = 友好空态（决策 24 离线边界不变，网格缓存属 RAM 渲染缓冲、§5.4 板端状态行）。

**② 语音问答**：KEY 单击或喊「你好，小智」→ hello → Opus 16k 帧流式上行 → 引擎 VAD 切句 → 云 ASR → `stt` 消息回发（聆听态逐句回显）→ OpenClaw（服务端持久会话：请求携 user=<device_id>、只发本回合，见 ADR-015）流式回复 → 引擎 TTS delta 流上 **fence 剥离状态机**：闭块瞬间同步发侧通道帧；正文送 TTS → Opus 24k 下行播报；front 截获侧通道帧 → `display_schema` 校验 → 逐块 mcp `kidpal/display` 下发 → 设备渲染。播报中 KEY 单击 = 停播 + `abort` 云端取消生成（Phase 1 唯一打断手段）。

**动态 context 装配（`261022-device-memory-prompt-overhaul`）**：引擎每轮发往 OpenClaw 的系统侧内容分两层——静态语义在 workspace `AGENTS.md`（缓存边界之上，§5.5.5）、动态 context 块在缓存边界之下；动态块含本回合时间 / 日期与 front 经内部注入通道提供的**课表摘要**（front `schedule.py` 唯一解析器派生，会话建立时注入；引擎不解析 MD、不建第二解析器——契约 §1.5 单源红线）。课表回合因此无需工具调用即可作答；摘要窗口外日期仍走读文件或诚实说查不到。

**③ 查字**：同 ② 前半；OpenClaw 判查字意图 → `char_stroke_lib` 从 9574 字 `char_cache.json` hold-and-replace 组 D3 笔顺 JSON（逐笔念笔名 = OpenClaw 回复文本自由承载，requirements v1.55 恢复——2026-09-15 真机实证「勇」9 笔笔名几何对照零错误，试点期持续核错误率；若走权威笔名数据源需 D3 契约扩展，届时契约门）→ `view=char` 覆盖窗（驻留到按键或 5 分钟无操作；**弹窗即静态显示整字、笔顺演示由 KEY 短按起播（再按 = 重播）**、长按关窗；BOOT 关窗回原页不翻页；播完会话即结束不回聆听态——requirements v1.36/v1.37/v1.43/v1.59 口径）。2026-09-15 真机复验：全链恢复可跑（语音应答→窗开 ~21.6s，5 秒档优化归 Phase 1）；板端两缺陷定位 = LVGL 任务栈溢出（已验证修复方向）+ SVG 解析器缺 C 命令（见 §8 风险债 5）。

**④ 记录归档**（对话式引导记录——requirements v1.65 / `261019-p3-record-dialogue-guide` 重写，取代旧「累积转写」链路；决策本体 = ADR-013）：触发词（「我要讲故事」/「帮我记下来」）或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切**记录语境对话态**——requirements v1.65 取代 v1.32「累积态」；信号载体 = 契约 §四 `kidpal_session_ctx`——**front 上行泵整条拦截消费、无论取值、引擎不感知本消息**，TLS 之后不下透）→ 聆听产逐句 stt（D3-甲聆听形态，manual 模式作废——manual 模式 ASR 在 listen stop 才触发、录音期间产不出 stt，收尾词前置检测与卡壳引导物理不可能，261001 q8 真机复核）+ front 旁路累积（stt 帧喂入会话记录缓冲、右区回显照旧，AI 不逐句回复）+ **引擎侧抑制 LLM**（261001 Package B 定形：记录语境期间引擎不对 stt 跑 LLM 回合——human 261001 触点①裁读法乙「全程无 LLM 回合」；front 截不住引擎→OpenClaw 的 loopback 直达调用，抑制必须在引擎侧：front 经既有内部注入通道向引擎注入等价记录语境信号〔ADR-003 `kidpal_speak` 同型先例、引擎补丁认领〕使引擎侧跳过 LLM 派发；front↔引擎为服务端内部链路、信号形状不入契约）→ **卡壳轻引导**（每句讲完起卡壳计时——阈值权威 = requirements NFR-PERF-3〔待定、真机调优后立项拍数〕；孩子接着说即重置；计时到或明示卡壳词命中——检出器 = 关键词表子串扫描、`is_finish_phrase` 同构——→ front 把目前累积的孩子的话发一次性引导 LLM 回合〔引导指令：只基于已讲内容提问、不提供情节选项、不代编情节；**可返回空 = 本轮不引导但轮次照计**——孩子说「别说话听我讲」的收声抑制同此形态，收声不停摆〕→ 轻引导一句 → `kidpal_speak` 注入播报〔ADR-003 既有音频链〕→ 继续聆听；设备停留记录屏不跳页、引导播报中 KEY 短按 = 打断引导继续聆听）→ **收尾三式**（front 机械扫描 stt 文本命中收尾词「我说完了/讲完啦/说完了」（「说完了」v1.66 并入，与 requirements v1.66 同批）/ 再按 KEY（listen-stop）/ **连续 2 轮引导无回应自动收尾**〔取代 120s 无语音超时 sweep；轮次含被抑制与返回空的不播报轮——收声不停摆〕；三式皆走正常归档、不丢弃）→ 下发 `start_organizing`（契约 §1.4 新增控制动作：设备切整理过渡演出、整理中 KEY/BOOT 均不响应；三式统一下发、重复幂等）→ **忠实整理**（全程孩子的话一次性发 LLM〔忠实整理指令：只用孩子原话与事实，去口水词、断句、分段、起标题〔≤12 字口径不变、随整合同一回合产出〕，不换表达、不补情节；引导语、孩子中途提问与 AI 回答滤除不进素材〕；KEY 路径沿 pending-stop 挂起窗口等迟到转写〔261001-m4-k3-story-archive-order 既有机制保留〕）→ `story.py` 归档（**持久流水号账本 `<stories_dir>/.seq.json`** 分配 `seq=max+1`、只增不减——同 id 重写不变、删除不回退、重启存活、账本缺失/损坏重建不回退 + 告警）→ `story_text`（**携带 `data.seq`**）下发渲染（离整理过渡态）+ 盖章演出（N = 入册流水号只增不减）。引导/整理指令载体 = **front 侧版本化 prompt 制品**（独立一次性 LLM 回合、不走 OpenClaw 主 agent 共享 system prompt——**不触 §5.5.5**；接线不变式沿 ADR-001 三面相等，制品随实现包落）；整理/引导链路失败 = **当轮素材不归档**（孩子已讲内容不写成故事、不分配 seq——丢稿口径显式化；失败态素材保全取舍设计稿与 v1.65 均未覆盖，归 §6 留根、不发明新需求语义）+ §6.3 固定安抚句 + text 视图结束当轮（自然离整理过渡态）。卡壳引导与忠实整理 = front 承载计时/计数/触发与指令装配（机械层）+ 脑侧一次性 LLM 回合生成（语义层）——ADR-002 混合分层同型，§2.4 原则 3 注记。**story_list = front 机械 producer 推送（三时点）**（① WS 建连恢复推送 + ② 归档成功后 + ③ 删除执行后。时点① = **恢复性推送**——requirements v1.63 赎回 260928 挂起的重裁：WS 建连时附带重推 story_list，载荷带 `data.restore` 标记（wire 载体 = 契约 §1.3③ story_list 的 additive 字段，**已随契约门落地（2026-10-12 human 批准，随 261010-p3-storylist-cold-start-restore 同批原子落）**，复刻 §1.3② P2 周网格 ADR-005 先例），设备只填 RAM 列表缓存、不接管前台，P3 在屏时仅重绘本页。沿革：「WS 建连即推」曾于 260928-story-connect-push-defer 摘除——真机拓扑下设备每会话重开 WS，建连即推会在每次按 KEY 时给设备打 story_list 帧、旧固件无条件渲染 P3 页盖掉聆听态〔story_page 零 CJK → 半截页〕；本次回归以 restore 标记 + 只填缓存不抢前台消解该缺陷的复现路径（实现 = front 建连恢复推送 + 板端 restore 分流，归 261010-p3-storylist-cold-start-restore 包）；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`；推送未达期间翻到 P3 = 数据未达空态（requirements S-F2.1 AC6，不显计数））——推送触发属行为策略（不进契约）；**引擎侧来源的 `story_*` display 块经 front relay 收紧降级**（story 数据单源 = front 机械推送）。**丢弃** = 取消链显式化（2026-10-21，`261021-p3-record-cancel-lifecycle`）：记录会话中按 BOOT 丢弃（requirements S-F2.2 AC4「取消、已录内容丢弃退回列表——不设重录恢复」）→ 设备于关闭音频通道前同连接先行发送契约 §四 `kidpal_story_discard`（端→网关单向；网关按本连接 session_id 权威取消、不信载荷值）→ front 取消该记录会话（累积缓冲销毁、素材不入册、不触发归档与 `story_list` 重推、在途引导/整理产出丢弃，幂等 + 审计日志）；断连隐式取消（cancel-on-disconnect）维持为兜底通道（human 2026-10-21 裁维持，spine q2）。**删除** = 契约 §四 `kidpal_story_delete`（端→网关单向）→ front `story.py::delete_story` **硬删** MD（哨兵头校验 + 路径穿越/无哨兵头拒绝 + 审计日志留痕）→ 重推 `story_list` 对账（`total` −1 可见 = 隐式 ack 载体；seq 不回退）。误判兜底见 §10 决策 25 取代注记与 requirements v1.47/v1.49 修订；「刚才那段是故事」挽回口径在记录语境对话态内不再需要（闲聊语境、其他页保留——requirements v1.65 S-F2.2 补充规则）。

**⑤ 提醒推送**：OpenClaw cron 每 15 分钟跑 `reminder_schedule_lib.py` 重算三档（QQ 录入后即时再跑）→ `pending.json` 原子写 → front 每分钟轮询 → AlertDispatcher 到点推送 `mcp kidpal/alert`（载荷带 speech 字段）→ 设备四态入队 → 渲染即回执（出队依据；同 id 接收即入集去重）→ 语音出音 = front 注入 `{"type":"kidpal_speak"}` → 引擎认领转调既有 `speak_txt`（edge-tts + Opus 下行链，ADR-003；不自建合成/不注入二进制音频帧）。超时 60s 重推 ×3，过期作废；录音态期间提醒排队延后（requirements v1.46）。

### 5.3 部署视图（跑在哪、怎么运维）

> ← 收编自旧 §1.1/§1.2 + server/deploy/README 与 260903-gateway-directionB / 260911-ecs-deploy-convergence 实证。

#### 5.3.1 设备节点（ESP32-S3-RLCD-4.2 硬件）

| 模块 | 型号/配置 | 说明 |
|---|---|---|
| 主控 | ESP32-S3-WROOM-1-N16R8（双核 240MHz） | Wi-Fi + BLE5，16MB Flash + 8MB PSRAM |
| 屏幕 | 4.2" RLCD 反射式，300×400 竖版（固件按 400×300 横用），1bpp | SPI 接口；无背光靠环境光；一帧 15KB；驱动 IC 口径 ST7305（官方 Demo 文档；固件为 custom 初始化序列、未硬编码型号），支持局部窗口更新 |
| 按键 ×3 | KEY=GPIO18、BOOT=GPIO0、PWR=硬件电源键 | KEY/BOOT 已接线（multi_button 三手势）；PWR 长按关机/单击开机、固件不可读 |
| 音频 | ES8311 codec（出）+ ES7210 ADC 双麦阵列（入） | AEC 用播放信号做参考；MX1.25 扬声器接口 |
| RTC | PCF85063（I2C）+ 独立备用电池座 | 备用电池座**仅支持 PH1.0 可充电电池**（不可装纽扣锂原电池） |
| 传感器 | SHTC3 温湿度（I2C） | 仅本地直读上首页（云端记录不做） |
| 存储 | 16MB Flash 分区（assets/art 分区）+ Micro SD 槽 | **SD 槽固件已接入**（SDMMC 1-bit 挂载 `/sdcard`，能力就绪 2026-10-16，261016-sd-card-mount；**story 正文可本地缓存 = 决策 24 的窄例外**〔261016-p3-story-body-sd-cache，2026-10-17，只读镜像、见 §6.2〕；**设备诊断日志可落 SD**（常开 + 有界轮转、目录总量 16MB 封顶、可删除，261018-sd-log-persist，见 §6.6）；提醒 / 课表 / 业务日志等其余业务数据仍不落板——原枚举的「日志」即业务日志（家长知情权），消歧见决策 24 注记）；资产走内部 Flash SPIFFS art 分区；I2C 四器件（ES8311/ES7210/RTC/SHTC3）共享总线 |
| 电池 | 18650 + 充放电管理（CHG/WRN 指示灯） | ADC 测电量；⚠️【修复项】固件三处未接线（板类 ADC 死成员、无 GetBatteryLevel override、渲染无注入缝），真机恒显满电 |

#### 5.3.2 云端拓扑（阿里云成都 ECS 8.137.91.16）

| 面 | 承载 | 端口/边界 |
|---|---|---|
| 设备接入（公网入口） | `kidpal-front.service`（uvicorn `front.front_app:app`） | **0.0.0.0:8000 TLS**（唯一公网入口；证书 `/opt/kidpal/xiaozhi-esp32-server/cert/server.{pem,key}`，自签 RSA-3072 notAfter 2046-08-30） |
| 引擎服务 | `xiaozhi-container.service` → docker compose（`xiaozhi-esp32-server`，官方镜像 6.9GB，`network_mode: host`） | 自绑 `127.0.0.1:8010` 明文 ws（不外露）；`data/.config.yaml` overlay + models 卷；engine-patch 两文件 bind-mount 覆盖 `core/` |
| 编排大脑 | OpenClaw（宿主进程） | `18789` 锁 loopback（gateway.bind=loopback，部署期断言）；容器经真 loopback 访问 |
| 记忆子系统 | Honcho 自托管栈（`server/deploy/honcho/**` 声明 → ECS `/opt/honcho` compose；五容器） | api `127.0.0.1:18100` 全 loopback（端口探针纳入 §5.3.3 部署后核验）；postgres + pgvector 落本机持久卷；资源基线 = ECS 2C/4G（2026-10-22 升级后基线；prep 税 / 内存压力 P3 后复测，TECH-261022-openclaw-prep-tax-memory-pressure） |
| 交接目录 | `/opt/kidpal/alerts/`（pending.json + semantic.json）、`/opt/kidpal/stories/` | 同机文件交接，front 与 OpenClaw 共享 |
| 配置与密钥 | `provider.config.yaml`（渲染源）→ `data/.config.yaml`（overlay 渲染产物）；`secrets.env` chmod 600（仓库只有 `.example`） | 引擎 `server.auth: false`（鉴权唯一权威在 front，260911 裁）；token ≥22 字符否则拒绝渲染 |
| 历史单 | `xiaozhi-gateway.service`（legacy）、`kidpal-shim.service`（已退役） | shim 8080 已停；公网探针实证 8080 不可达、8000 唯一入口（260903 实证） |

#### 5.3.3 运维面

- **overlay 渲染**：`apply-xiaozhi-overlay.sh` 以 `provider.config.yaml` 为源渲染 `data/.config.yaml`（LLM type:openai→OpenClaw、TTS edge、ASR qwen3_asr_flash、禁 max_tokens、`server.ip=127.0.0.1/port=8010`）+ prompt 版本接线（PROMPT_SRC/DST）；
- **部署后核验三件**：① `docker logs | grep "kidpal display-strip patch active"`（每次建连打一行；**缺失 = 补丁未生效**，fence 重新混入播报 + relay 断链）；② `docker inspect` 核 bind-mount 两映射在位；③ 端口探针（8000 可达 / 8010·18789·8080 不可达）；另核记忆子系统 api 18100 仅本机可达、五容器其余端口公网不可达；
- **漂移告警语义**：镜像升级 → 部署副本（0.9.6 基线）可能与新引擎内部 API 不匹配（ARQ-260907-engine-deploy-patch-drift）；升级前先对上游 diff；
- **证书**：`cert-renew.sh`（VALID_DAYS=7300，auto/renew/force 三模式）+ `cert-warn-expiry.sh`（WARN_DAYS=30，exit 0/1/2 可机判）；
- **提醒 cron**：`openclaw cron`（kidpal-reminder-plan，venv 解释器载荷——系统 python 3.6.8 不兼容之坑已记录）；回滚 = live `.config.yaml` 时间戳备份 + `openclaw cron remove`；
- **已知怪癖**：容器 healthcheck 恒 unhealthy（镜像内无 ss/netstat）；引擎健康以标记行 + 设备可建连为准。

### 5.4 数据视图（谁拥有、存哪、活多久）

> ← 收编自决策 13/24/26 + ADR-002 + 路径常量调研；格式权威全部指向 contracts/。

| 数据 | 位置 | 写者（owner） | 生命周期 | 掉电语义 | 格式权威 |
|---|---|---|---|---|---|
| 课表 `schedule.md` | OpenClaw workspace `/root/.openclaw/workspace/kidpal/` | OpenClaw（QQ 录入改写）/ 家长直改 | 持久；过期一次性安排查询时过滤、不归档 | 云端持久 | 契约 B1 |
| 故事 `stories/<id>.md` | `/opt/kidpal/stories/` | front `story.py` 归档 | 持久；删除 = 云端同步**硬删** MD（requirements v1.47；**链路载体 = 契约 §四 `kidpal_story_delete`**，2026-09-24 已 LANDED） | 云端持久 | 契约 §1.3③ story_list / §1.3④ story_text |
| 入册流水号账本 `.seq.json` | `/opt/kidpal/stories/.seq.json` | front `story.py`（原子写、单写者） | 持久；**只增不减**（删除不回退、同 id 重写不变、重启存活；缺失/损坏则从现存文件重建 + 告警、不倒退） | 云端持久 | 契约 §1.3④（`seq` 语义）/ ADR-007 |
| 提醒 `alerts/pending.json` + `semantic.json` | `/opt/kidpal/alerts/` | 脚本（原子写）/ agent | 到点作废、过期不补发 | 云端可失（cron 重算再生） | §5.2 场景⑤ |
| 字库 `chardata/` + `char_cache.json` | OpenClaw workspace | `warm_char_cache.py` 生成 | 静态 | 云端持久 | 契约 D3 |
| 设备凭证（url/token/version） | 设备 NVS namespace "websocket" | 配网写入（token=24B CSPRNG） | 持久 | 掉电保持 | 介质禁令白名单（决策 24） |
| 板端业务/状态（提醒 id 集合、渲染缓冲、层栈、会话态） | 设备 RAM | 固件 | 会话期 | **掉电即失、禁落非易失介质** | 决策 24 |
| story 正文本地副本（**只读镜像**） | 设备 SD 卡 `/sdcard/story/` | 固件（云端 story 归档为唯一真相源，板端不承载任何写入） | 持久（随卡；卡可换） | 掉电保持 | **决策 24 carve-out**（261016-p3-story-body-sd-cache，只读镜像例外，见 §6.2）／契约 §1.3④ story_text |
| 设备诊断日志 `logs/KPLOGnnn.TXT` | 设备 SD 卡 `/sdcard/logs/` | 固件诊断日志写盘任务（`esp_log_set_vprintf` 钩子 → PSRAM 环形缓冲 → 低优先级批量刷盘；单写者） | 常开；有界轮转（512KB × 32 个，目录总量 16MB 封顶、覆盖最旧） | 掉电保持（随卡；卡可拔可换、可删除） | 决策 24 白名单增列（261018-sd-log-persist，见 §6.6）／板端自定义明文行格式（非端云数据格式，无契约权威；含 STT / LLM 对话原文，见决策 24 注记） |
| 1bpp 美术/字体/提示音 | 设备 art+assets 分区 | 资产管线（构建期） | 静态 | 随固件面 | §5.5.2 |
| 对话历史（服务端持久会话） | OpenClaw 会话存储（按 `agent:kidpal:openai-user:<device_id>` 键） | OpenClaw（每轮追加本回合；压缩 = auto-compaction，默认开） | 持久；跨 UI 会话保留（ADR-015；增长 / 压缩成本 = §8.1 风险 14 观察项） | 云端持久 | 非契约（wire 无此形状） |
| 对话历史（剥离前原文，引擎本地副本） | 引擎容器内存 | 引擎 | 连接期；**本地非权威副本、不向 OpenClaw 重放**（ADR-015） | 掉电即失 | — |
| 记忆·会话消息（peer / session / message） | Honcho postgres（ECS 本机；pgvector 向量列） | OpenClaw Honcho 插件（每轮 persist；写入范围与残余见 §6.7） | 持久；删除 = 运维手工（NFR-PRIV-1） | 云端持久 | Honcho 内部 schema（非契约） |
| 记忆·画像（observation / representation） | 同上 | Honcho deriver（LLM 抽取）→ representation | 持久、随抽取滚动更新；删除同上 | 云端持久 | 语义权威 = requirements NFR-PRIV-1；形状非契约 |

### 5.5 开发视图（怎么造、怎么验）

#### 5.5.1 仓库结构与技术栈

- `firmware/`：小智 V2.1.0 整树基座 + 板级 overlay（ESP-IDF + C++；esp-sr ~2.2、esp-opus-encoder ~2.4.1、LVGL 锁 9.3.x）。注：官方 Demo 的 Arduino + U8g2 路线**未被采用**（旧文档 §1.1 的 U8g2 版本约束属该路线遗留）；
- `server/`：gateway（front 冻结模块）/ deploy（部署面）/ acceptance（~183 验收函数，offline+live 两面）/ shim.py（已退役）；
- `tools/`：LVGL 模拟器 + 资产管线（不入图索引，查询降级 Grep）；
- `docs/`（spec 面 + contracts + notes）、`delivery/review/` + `delivery/intake/` + `delivery/retro/` + `delivery/archive/`（治理/开发证据，261015 批六目录新树；旧 changes/ 伞归档）、`.claude/`（治理机制）。

#### 5.5.2 资产管线（预栅格化 1bpp）

设计稿 HTML 内嵌 SVG 为唯一母版 → 脚本抽取 symbol → 栅格化 → **RGB565 `.bin`**（I1 索引仅留真机 bin-decoder 优化项，可回收 16 倍空间）→ SPIFFS art 分区；模拟器同批资产双端渲染。管线三坑已修：透明 SVG 先 flatten(#ffffff) 再阈值化；libvips 不解析多层 `<use href>`；`<text>` 依赖运行环境字体（真机须以实际字体子集重烘）。⚠️ 预算口径 = 策略笔记实测于 art 切分前（47 资产 599,698B + P2–P4 估算 ≈3.85MB vs assets 8MB、余量 54.1%）；现分区为 art 512KB（KAP1 美术）+ assets 8MB 双分区（§5.5.6），47 项资产的实际落位与 P4 笔顺字盘（~3.1MB）装载目标须在 Phase 1 资产装载时复核。PSRAM 预算：LVGL draw buffer 240KB + RLCD 帧缓冲 15KB + 两张 LUT 360KB；ThorVG sdkconfig 默认关——预栅格化是复杂图形唯一低成本路径。详见 [notes/ui-implementation-strategy.md](notes/ui-implementation-strategy.md)。**字体预算注记（`261028-font-typography-scaleup`，2026-10-28）**：本批新增 16px 正文与 20px 阅读标题两枚全量 CJK `.bin` 计入 assets 8MB 分区预算，不占 ota_0 / ota_1；assets 起草期实测余量 5,597,745B（约 5.34MiB，见 ADR-017），两枚新字体的实际尺寸由 P0 资产生成后复测，分区表不因本批重排。

#### 5.5.3 字体管线

模拟器 FreeType 仅用于设计期快速迭代，不作为真机 1bpp 字形等价证据；host 验收与真机共用同一枚字体 `.bin`（host 编入 `cbin_font.c` 读同一文件，同源接线归 P0）。真机字体经 `lv_font_conv` 产出 `.bin` 并打入 **assets 8MB 分区**，运行时由 `Assets::GetAssetData` 取数据、经 `LvglCBinFont` / `cbin_font_create` 装载；缺资产或装载失败时回退编译内 `font_puhui_14_1`（14px），保证不黑屏。

资产字族在既有 `font_body.bin`（14px 正文）、`font_title_zcool.bin`（30px 标题静态子集）、`font_num_baloo.bin`（44px 数字）三枚之外，新增 **16px 正文全量 CJK** 与 **20px 阅读标题全量 CJK** 两枚；本批只接通新增两枚的生产消费，不把旧三枚追述为已完成运行时接线。正文与列表采用 16px，阅读标题采用 20px，标题缺字回退 16px；键帽条 / 辅助条 / 查字窗仍用 14px，九档实验页与测试字体维持编入、不退役。大号数字与图案化文字首选 imgfont 或预渲染位图（原生标签行距 / 基线对齐是高频痛点）。

NFR-READ-1 的 14px 下限已由 2026-09-15 九档真机对照实测定案，并于 2026-10-28 复测，不再挂「未实测」；字号、行数与页面容量的需求阈值归 requirements.md，本节只承载字体生产、落位、装载与降级机制。

#### 5.5.4 UI 演出与现状

- **语音轮四态**（聆听/思考/播报/提醒）版式骨架 = 左小伴（各态 pose）· 右内容区（requirements v1.24）；场景码与内容映射已冻结（`kidpal_voice_states`）；⚠️【修复项】活侧 `SetStatus` 仅映射三态、思考态零调用者——真机思考页不弹，Phase 1 接线（进入时机 = 孩子说完这一句，与现有 `UTT_START` 开口边对齐）；
- **记录会话演出**（`p3-recording`〔v7 修订〕/`p3-recording-guide`〔v7 新增·引导播报中〕/`state-organizing`〔v7 新增·整理中、思考态复用——下行触发 = 契约 §1.4 `start_organizing`〕/`p3-stamp-frame`+`comp-stamp`）：录音/盖章两态 MVP 已实现验收，引导播报中/整理中两帧随本批实现面包接线；⚠️ 改名对账四项【修复项】：盖章文案「故事徽章→星星徽章」/ 页名「我的故事」应删 / 提示缺「帮我记下来」/ 选中反白应改边框（冻结验收 needle 随文案一并更新）；
- **查字窗**（v1.36 改窗）：宿主常驻页改覆盖窗，渲染资产全复用；【修复项】`canvas-p4`→`char-window` 锚改名、进度珠死成员清理、当前笔白衬边已实现（小手占位删）、部首行装配未上屏须补；
- **底部键帽条**：条体已常驻（基座 screen 所有），内部仅单个居中 label——三槽布局/页点/键帽图形【修复项】；
- **首页辅助条**：联网图标链路已实现（断网 WiFi-slash + 强/中/弱三档）；电量【修复项】见 §5.3.1。

#### 5.5.5 OpenClaw prompt 承载与版本化（静态 / 动态两件）

**静态语义（主承载）**：单一事实源 = `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（仓内镜像 → live `/root/.openclaw/workspace-kidpal/AGENTS.md`；经 `server/deploy/manifest.yaml` 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日 md5 核验）。OpenClaw 把 workspace `AGENTS.md` 作为 stable project context 组装进 system prompt 的**缓存边界之上**（参考实现：context file 序 `agents.md` 居首、仅 `heartbeat.md` 归 dynamic，stable 段落置于 `SYSTEM_PROMPT_CACHE_BOUNDARY` 之前）——跨回合字节稳定、命中缓存前缀。v1–v11 历史 prompt 版本文件原地冻结、可 diff；演进史全量副本 = `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`。

**动态 context（每轮，缓存边界之下）**：模板 = `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`（新增），经 `apply-xiaozhi-overlay.sh` 渲染进引擎 `.config.yaml` 的 `prompt:` 键；内容 = 有界的回合内动态信息（本回合时间 / 日期、front 注入的课表摘要等），**不含静态纪律**。它在 OpenClaw 侧落在缓存边界之下的 Conversation Context（参考实现 `extraSystemPrompt` 段），不污染稳定前缀。引擎既有 `prompt_template` 键与动态块的分工归 P1 实现，本节只钉「静态不进动态块、动态块有界且不含纪律」。

**承载不变式（ADR-001 重定义，`261022-device-memory-prompt-overhaul`）**：① **静态面**：渲染面（live workspace `AGENTS.md`）== 声明面（`OPENCLAW_SYSTEM_PROMPT_CARRIER` + `provider.config.yaml` 静态承载三键，键形归 P1 实现）== 被测面（探针直接读同一 `AGENTS.md`）；② **动态面**：渲染面（`.config.yaml` `prompt:` 值）== 声明面（`provider.config.yaml` 动态模板三键）== 被测面（探针请求同源）。守卫从结构化指向解析当前承载（版本无关），集合相等照 ADR-001 本体两原则执行；ADR-001 的「版本化部署制品」实例枚举由本批重定义收窄为上述两面（ADR-014 记载 amend，不覆盖其决策本体）。

**OQ-8 裁定（2026-10-22，本批）**：**否决「v12 双件」**——v12 与 `AGENTS.md` 同载静态语义 = 两处事实源，ADR-001 的三面相等会退化为「三面同指一版 + 一个影子副本」。**「单一事实源」原则采纳，落点 = 现有 prompt 资产家**：静态语义唯一件 = `openclaw-agents/workspace-kidpal/AGENTS.md`（原地）；动态模板与 carrier 留在 `server/deploy/` 现址（`kidpal-dynamic-context.v1.txt` / `OPENCLAW_SYSTEM_PROMPT_CARRIER`）。**不新建 `server/deploy/prompts/` 平行目录**——现成 `openclaw-agents/` 家仍在（`AGENTS.md`、README、历史版本文件），新造平行目录会产生「两个 prompt 家」的新歧义；搬家本身不产生单源语义收益，只增加 manifest / deploy / verify / README 四处路径改写与冻结验收测试 corpus 迁移的漂移面。目录归纳如确需，另立纯搬迁批，不阻塞本列车。

#### 5.5.6 构建与测试基建

- 构建：ESP-IDF（`idf_run.py` 包装，WSL CRLF 坑见 kidpal-idf-env skill）；资产构建 `scripts/build_default_assets.py` → assets.bin；分区表 `partitions/v2/16m.csv`（**art SPIFFS 512KB @0x780000** 承载 KAP1 美术，包 9b ART-E2 从 ota_1 尾部切出；**assets SPIFFS 8MB @0x800000** 承载字体/音效，行冻结保 canary）；
- 开发拓扑：Pod 在本地 git worktree 树池中开发（复用 ∨ 按需扩容，一树一常驻分支）；固件环境等价三件套 = sdkconfig/dependencies.lock 镜像 + managed_components 软链共享主 checkout + worktree-bootstrap；host build 热缓存随树（`build/` gitignored），换包增量编译（launch 面准备操作协议见 `kidpal-pod-launch-prep` skill，= Dev Loop ② 阶段；见 `docs/DEV-GOVERNANCE.md` §4）；
- 测试四面：① 固件 host 验收（`test/*.acceptance.c`，含冻结 needle）；② server 验收 ~183 函数（docstring 内 frozen offline contract；live 面 `@pytest.mark.live` 默认 skip、`RUN_LIVE=1` 显式放行——G3 纪律）；③ 模拟器（LVGL 9.3 对齐 + 1bpp 阈值化快照自查 `kidpal_snapshot`，验收图一律阈值化 PNG）；④ live 冒烟（真机轮，捕获纪律见 kidpal-live-smoke-debug skill：wire capture 因 permessage-deflate 失败，用设备/引擎日志替代）；
- 结构化日志：front `gateway/log.py`（K6）+ correlationId 纪律；三日志源关联 = 设备串口 × front systemd × 引擎 docker logs。

---

## 6. 横切机制（贯穿所有模块的那几件事）

> ← 从决策 21/22/24/26/27/28 抽机制本体；决策条目本身在 §10。

### 6.1 鉴权与传输安全
**在哪侧：front（唯一权威）。** TLS 终结（自签 RSA-3072，≥10 年门槛、实配 20 年）+ HTTP upgrade 层 accept 之前强制 token（常时比较、≥22 字符、未设拒服务、无 validation-mode 后门）+ OpenClaw 18789 锁 loopback（部署期断言）。引擎层 `auth: false`——HMAC 与小智引擎不相容，260911 裁静态 token 强制点唯一钉在 front。设备 token = 全设备共享单 token（MVP 单设备；已知取舍：无每设备身份、单点泄露危及全部）。CA 嵌入固件实现面 = dev-loop handoff V10；轮换脚本 + 到期告警在位（§5.3.3）。

### 6.2 离线边界与介质禁令
**在哪侧：设备（禁令）+ 云（真相源）。** 断网仅本地时间/温湿度/电量；无提醒、课表/查字/问答不可用；不缓存任何事项。（「不缓存」禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——P2 网格缓存、P1 seam——掉电即失，不在禁列，见 §5.4 板端状态行。）板端非易失介质白名单 = 设备凭证（url/token/version）+ 预栅格化资产与字体 + 固件本体；一切业务/身份/事项状态只驻 RAM、掉电即失（决策 24）。**story 正文缓存例外（`261016-p3-story-body-sd-cache`，2026-10-17）**：板端 SD 卡可为 **story 正文**留**只读镜像**——云端 story 归档仍为唯一真相源，板端不承载任何写入；断网时**已留存的正文照常回看**（需求侧 = S-F2.1）。此为决策 24 介质禁令的**窄例外**（白名单增列「story 正文只读镜像」），提醒 / 课表 / 日志等其余业务事项仍不落板；**离线边界语义（断网无提醒）不变**。 **设备诊断日志例外（`261018-sd-log-persist`，2026-10-18）**：板端 SD 卡可存**设备诊断日志**（设备自身运行日志，常开 + 有界轮转、目录总量 16MB 封顶、可删除，见 §6.6）——纯可观测性数据，不承载任何离线功能、不是业务真相源；白名单增列「设备诊断日志」。本段与原则 5 所列「日志」= **业务日志**（家长知情权），业务日志仍不落板、语义不变；提醒 / 课表仍不落板；**离线边界语义（断网无提醒）不变**。隐私接受注记见决策 24 注记。

### 6.3 错误处理与降级
**原则：有处理的失败模式，不静默重试、不曝技术错误。** 云端任一环节失败 → 固定安抚句「网络出小差了，等一会儿再试试吧」+ text 视图结束当轮（决策 26）；display 脏块 → front 机械校验拒绝（丢弃或降级提示，不入板盲渲染，V27）；答案超限 → 截断/分屏（V32）；speech 合成失败 → 弹窗+提示音仍必达（V20/ADR-003 不对称必达）；fence 未闭合 → 裸载荷进 display 侧通道、绝不进 TTS（宁默不念 JSON）；art 挂载失败 → 占位矩形降级。

### 6.4 断线重连与常驻连接
**在哪侧：设备（重连）+ 引擎配置（不再主动断）。** 设备开机即连、断线自动重连、屏幕显示不受影响。引擎侧两道超时已配置修正为 24h 纯兜底 + 告别语关闭（`close_connection_no_voice_time: 86400` + `end_prompt.enable: false`，决策 21——此前 120s 无语音断连制造推送黑洞）。休眠残余闭账（2026-10-11）：human 2026-10-10 触点①裁「永远不休眠」（决策 29）——设备 Idle 态恒持活 WS 常驻在线，会话收尾本地关后 T=5s 常驻重建、服务端/网络侧断自动重连（261010 包落地），`alert_skip reason=no_online_device` 离线驻留窗口消除。常驻连接 modem 省电档（LOW_POWER/BALANCED）以 WS keepalive 长稳为准取低档，结论待真机观测冻结（RC-B6）。

### 6.5 幂等与一致性
**在哪侧：提醒链路两端。** 幂等 id = 确定性散列（schedule 条目+档位+事件日期），脚本首次生成、跨重推/重算不变（ADR-002）；设备接收即入易失 known-id 集合（仅防重复入队）；回执出队 = 渲染即发（渲染即确认）；同 id 重推两态分处（已渲染→补回回执；排队未渲染→去重等渲染）；pending.json 临时文件原子写。重试 = 60s 超时重推、单档上限 3、不做重连即重推、过期静默作废。

### 6.6 可观测性
三日志源：设备串口（COM5，被动 watch / 复位捕获纪律）、front systemd 日志（K6 结构化 + correlationId）、引擎 docker logs（补丁标记行 = 部署面报警锚）。

设备侧诊断日志**常开落板端 SD 卡**（`/sdcard/logs/KPLOGnnn.TXT`，8.3 名、512KB × 32 个、目录总量 16MB 封顶、循环覆盖最旧；采集 = `esp_log_set_vprintf` 钩子链式转调原 vprintf，串口输出不中断；含 STT / LLM 对话原文，human 2026-10-18 有意识接受，见决策 24 注记）——这是「设备串口」同一 `esp_log` 流的留存副本，**不新增第四个源**；用途 = 设备脱离串口独立运行时事后回看。它与**业务日志**（家长知情权 = OpenClaw 侧完整日志）是两个概念——业务日志不落板、语义不变。日志内容增强（`261022-diag-log-content`，2026-10-22）：每行落卡日志带墙上时间前缀（SNTP/RTC 已同步 → `YYYY-MM-DD HH:MM:SS`，未同步降级 `BOOT+<ms>ms` 标注；低频任务预格式化缓存 + 钩子只拼接），并覆盖五类语义记录面——页面跳转（PAGE_SWITCH from→to + trigger）、设备状态机跳转（含被拒转移 state_reject 补打）、按键事件语义名、SystemInfo 周期噪声降频（10s→60s）——落卡管线口径（8.3 名 / 512KB × 32 / 16MB 封顶轮转）不变，验收锚 `[DLG-A1]-[DLG-A6]`。

关联法与已知坑（wire capture 因 permessage-deflate 不可用）见 kidpal-live-smoke-debug / kidpal-serial-monitor skills。派发观测缺口（dispatch-stamp 无模型/token 分项）= ARQ-260909。

### 6.7 记忆写入范围与画像

**在哪侧：记忆子系统（§5.1.2）+ OpenClaw 侧插件接线（跨 front / 引擎 / OpenClaw 的部署面行为）。**

**记忆写入范围（类级规则，以已验证行为为准）**：生产记忆接线 = OpenClaw 上游插件 `@honcho-ai/openclaw-honcho`（经 OpenClaw 自有插件管理器安装、pin 版本），采集（落消息 / 抽 observation）与注入（`before_prompt_build` 画像）按回合钩子**全局生效**——插件没有「仅携 `user` 的回合才入库」这类逐回合判据（2026-10-23 实况；2026-10-22 初版按「请求是否携 `user` 即是否入库」的写法源于探针配置缩略式的误读，已废止）。写入边界按**内容归属**定义，分三类：

- **孩子的原话**——无论承载它的是孩子真实聊天，还是设备为孩子运行的流程回合（故事整理、卡壳引导等）：照常记入、照常参与记忆。一次性回合自 2026-10-24（R7/OQ-4）起统一携 `user=<device_id>` 落同一设备 session 键族，归属差异只体现在**内容**，不体现在会话归属；
- **小伴或系统生成的回复产物**——整理成篇的故事、引导语、叙事生成文本等：可以留存于记忆存储，但不视为孩子说的事实、不据以形成关于孩子的结论；
- **平台内部系统回合**——cron / 心跳 / 记忆巩固与叙事生成等由平台自身调度、孩子没有开口的回合：内容不进画像。**类级规则**（下表为实例枚举；新回合形态入册前须实测其写 / 读行为，按实测入表）。类级边界自记忆子系统上线（2026-10-22）起对新增回合生效；历史语料照记（见下「历史 backfill」）。**类别优先于内容**：平台内部系统回合即使正文夹带孩子旧原话片段（dreaming 的叙事素材即此类），整回合仍不入画像——那些片段在原始对话回合里已各自记过，不因被后台回合二次引用而计入。

「记忆巩固」一词在两类系统里指向不同对象，**不可按词面一刀切**：OpenClaw `memory-core` 的 **dreaming** 是「被当对话捕获的后台子回合」（每相位在素材满足时跑一次写梦境日记的 subagent turn，其 user 消息 = 叙事指令 + 真实对话片段），属本类排除对象；Honcho 自身的 **dream**（omni / `card_refresh`）是**服务端巩固机制**——不采集新回合，只对已入库消息做 deduction / induction、产 conclusions 与 peer card，属合法记忆机制，不在「内容不进画像」的排除范围。

**现行机制与界限（两把杠杆）**：① **采集侧的分类排除** = 插件 `isSystemRun` 三条件（`classifySession === "cron"` / `trigger ∈ {cron, heartbeat}` / `provenance === "internal_system"`）+ `captureSystemRuns=false`（默认）——它按会话分类工作、不是内容判据，只覆盖 cron / 心跳 / 显式 internal_system 三类；② **`noisePatterns` 已退役**（2026-10-24 R7/OQ-4：一次性回合不再整条丢消息；live 实读为 `[]`），不再是边界件。dreaming 是①的**已知实例**：其 session key `dreaming-narrative-memory-core-v2-<phase>-<hash>` 不命中三条件、落 chat 分支被当普通对话采集（2026-10-25 实测占 owner 最近 100 条结论窗口 13/100）；现行防线 = **生成器被全局关闭**（`plugins.entries.memory-core.config.dreaming.enabled=false`，`261025-openclaw-dreaming-disable` 部署 + 2026-10-26 数据清理，清理后窗口 0/100）——**关的是生成器、不是采集分类**；重开 dreaming 即复发（重评触发与整体重开口径见该部署声明 `_note`）。

下表是类级规则的**实例枚举与实测归属**（完整枚举由本节承载，S-F5.1 AC5 所引「完整枚举由架构文档定义」即此表；以实测为准、不按设计意图推定）：

| # | 回合 | 触发 / 承载 | 实际记忆行为与残余 |
|---|---|---|---|
| 1 | 卡壳引导回合 | front 一次性 LLM（`_openclaw_chat_once`；user 消息 = 恒定包装「孩子到目前为止讲的内容：…\n\n孩子停顿了，请按指令给一句轻声引导。」；携 `user=<device_id>`） | **入库、照常参与记忆**（孩子原话照记；包装指令部分的派生残余见（四））；模型回复落库、只产 agent 侧结论（见（二））；读侧画像照常注入（见（一）） |
| 2 | 忠实整理回合 | 同上（恒定包装「孩子讲的内容：…\n\n请按指令把这段话忠实整理成一篇。」；携 `user=<device_id>`） | **入库**：孩子原话照记；整理产物 = 小伴生成的回复产物，留存于记忆存储但不据以形成关于孩子的结论；读侧仍注入孩子画像——整理回合可能被画像「带跑」（忠实性质量风险，真机抽检 3–5 例）；同一段内容先进引导、再进整理 = 两遍入库（见（三）） |
| 3 | 提醒到点语义回合 | OpenClaw cron → **main** agent 产 `semantic.json` | **排除**（平台内部系统回合实例）：cron 会话分类 + `captureSystemRuns=false`（默认）；其 user 内容是机器生成的任务提示、不是孩子原话 |
| 4 | 提醒三档机械脚本 | `reminder_schedule_lib.py` | 不适用（无 LLM 回合） |
| 5 | 家长 QQ 录入 / 查日志回合 | QQ 通道 → main agent | **消息照常落库**（残余）；家长回合按 `senderId` 归独立通道 peer（插件默认 `per-sender` 策略，不污染家庭 owner 画像）——它既不属于「孩子的话」也不属于「平台内部系统回合」，是类级边界之外的**第三类（家长侧）**；家长内容可被 `honcho_*` 工具（如跨会话检索工具 `honcho_search_messages`）检索到的风险待部署轮实测，未闭合前按未闭合风险记账 |
| 6 | 故障 / 兜底固定文案 | front 固定安抚句 / 引擎 `system_error_response` | 不适用（无 LLM 回合） |
| 7 | 记忆巩固 / 叙事生成（`memory-core` dreaming） | OpenClaw `memory-core` 插件后台 subagent turn（session key `dreaming-narrative-memory-core-v2-<phase>-<hash>`） | **平台内部系统回合实例，但采集侧无排除机制**（key 落 chat 分支、`isSystemRun` 不命中）——现行处置 = 生成器全局关闭（2026-10-25 部署 + 2026-10-26 数据清理，owner 最近 100 条结论窗口 13/100 → 0/100）；重开 dreaming 即复发 |

（记录语境期间引擎侧本就无 LLM 回合〔ADR-013 读法乙〕，不在枚举内。表外的 Honcho 自身 dream（omni / `card_refresh`）不是回合、是服务端巩固机制，见上文「记忆巩固」两类区分。新回合形态入册前须实测其写 / 读行为，按实测入表——**以实测为准，不按设计意图推定**；front / QQ 两行的部署轮实测随验收包落盘，与本表不符时以实测为准回填。）

**残余与未闭合申报**（human 2026-10-26 裁「按现状记录、本批不修实现」；需求面同口径申报 = S-F5.1 补充规则「已知残余」，本条不重复展开）：

- （一）**读侧画像注入不区分回合**——`before_prompt_build` 对所有回合统一注入，写侧的内容归属不改变读侧行为；凡进入记忆库的内容都可能经 representation / 检索回到后续回合的上下文。
- （二）**入库 ≠ 归属**——模型回复（assistant 消息）照常落库并产 agent 侧结论（`agent-kidpal`），但观察者扇出按 role 判定，assistant 消息不进 owner（孩子）画像；「留存于记忆存储」与「据以形成关于孩子的事实结论」是两件事，设备流程回合的回复产物同此。
- （三）**引导 + 整理双回合重复喂同一段孩子内容**——消息库两遍、observation 可能重复膨胀（检索质量面；261024 批申报，部署轮看检索）。
- （四）**包装指令被抽成过程性结论**——设备流程回合的 user 消息 = 孩子原话 + 机器写的整理 / 引导指令混合（`server/gateway/front/front_app.py:409-410` / `:848-849` 恒定包装），deriver 按 role 而非 authorship 扇出，指令部分会被抽成「孩子要求整理成文」一类过程性结论（live 实证 owner 侧 6 条，位于最近 100 条窗口第 35–69 位）。当前处理 = 运营面定期清理；机制修复另行立项（候选路径四条：改包装形态 / deriver 提示词补丁 / 运营清理 / 上游提报；依据 = `docs/debt/tech/TECH-261026-oneshot-wrapper-prompt-derived-facts.md`）。
- （五）**缺席 `device_id` 时的降级**——一次性回合取不到设备身份时不携 `user`（`front_app.py:853-857` / `:1246-1250`，日志 `oneshot_user=absent`），OpenClaw 回落「每请求随机 session + 调用方自携带历史」（ADR-015 ③）；插件采集与注入仍全局生效——**降级的是会话连续性，不是记忆排除**。
- （六）**多腿归属单设备 MVP 口径**——设备身份取第一条携 device-id 头的会话腿（`front_app.py:324-334`）；单设备 MVP 成立；多设备时按第一条腿归属会出错，多设备身份体系立项时解。
- （七）**`subagent:*` 家族未闭合**——插件 `classifySession` 把 `subagent:` 前缀单独分类，但 `isSystemRun` 不排除，当前无机制保证其不入库（无命中数据，如实申报未闭合）。运维防线 = 部署 runbook §9 升级盘点（`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 四家族必须为空）；命中即采集策略漂移，先盘点、机制修复另立实现批。
- （八）**类级边界在现有实现面无管线机械闸**——可判的只有两层：① 静态配置实读（`openclaw config get` 核 dreaming 叶与插件配置键）；② Honcho 会话家族盘点（运维轮）。Phase 1 只有运维观测、无管线机械闸（requirements v1.68 S-F5.1 可行性行 🟡）。

**记忆身份**：记忆库内 peer = 家庭单 `owner`（另有 agent 侧 `agent-kidpal`）；设备身份体现在 **session key 家族** `agent:kidpal:openai-user:<device_id>`——每设备一族会话，换设备 = 新 session 家族（会话连续性重置），**画像仍为家庭单 owner、不按设备隔离**（§1.3）。一次性回合（引导 / 整理）自 2026-10-24 起同样携该 device 身份落同一 session 键族；身份无法确定时的降级见残余（五）。

**画像边界（AC3 / AC4 抽取边界，三层纵深）**：① **来源门** = 上文的类级规则与内容归属（孩子的原话照记；机器产物留存但不据以形成关于孩子的结论；平台内部系统回合不入画像）——读侧注入不区分回合、模型回复落库为已申报残余（见（一）（二））；② **抽取策略** = deriver 只抽「孩子世界的事实」（喜好 / 宠物 / 同学 / 经历），家长隐私信息（姓名 / 联系方式 / 家庭安排）不产 observation——策略承载首选 Honcho deriver 侧的提示配置；若自托管版本不支持自定义 deriver 提示，退回「持久化前消息侧过滤」；③ **注入边界** = representation 注入前的类别过滤（低置信 / 敏感类不回注）。验收判据 = seeded 负向探针集 100% 零命中（NFR-PRIV-1）；三层任一失守即不达标。P3 实测成形后回填本小节。

**历史 backfill**：2026-09-30 前 `session-corpus` 全量照记（human 2026-10-22 裁：不分回合性质、含第三方姓名）；幂等可重跑、前后计数报告、清库回滚（实现归 P4）。

**删除语义**：NFR-PRIV-1 —— 运维面手工删除记忆画像与 observation / 消息存储；不删 OpenClaw 完整日志、历史补录源语料、设备诊断日志（各按既有接受口径保留）。

**降级**：记忆服务不可用 → 本轮无画像注入、对话与回复照常（不阻塞、不静默重试风暴）；记忆失败不进 §6.3 的用户可见兜底（用户无感知）。

---

## 7. 接口与契约（指针，不抄细节）

> ← 收编自旧 §4；CDQ-260902 已 CLOSED（2026-09-03 human 批四题，契约重写原子落地）。

数据格式与端云接口的单一事实源 = [docs/contracts/interface-contract.md](contracts/interface-contract.md)（契约变更走 /arch-change loop，合并过机械凭据闸 pre-merge-escalation-gate.mjs——261015 批起无人签门）：

- **笔顺 JSON（D3）**：已定稿——makemeahanzi，strokes SVG path + medians 骨架点；数据源与板端渲染要点见 §5.1.1 查字组件与 [notes/](notes/)；
- **display 统一响应块（E1）**：`{view, data, speech_hint?}`，7 view 白名单；引擎剥离 → front 校验（V27）→ mcp 下发；control 动作表已含 `start_recording`（CDQ-260902 落地）；
- **课表 MD（B1）**：YAML frontmatter + 三区块表格；`remind_before_min` 已随三档阶梯废止；
- **契约决策账**：契约相关未结项的状态唯一权威 = [docs/debt/contract/](debt/contract/)（一条目一文件 + 生成件 [INDEX.md](debt/contract/INDEX.md)）；本档不再内嵌逐条状态，防内嵌清单与账本再漂移。唯一例外 = 「音量调节」control 动作（requirements v1.44，§5.1.1 MCP 工具面）**仍在办**——debt 账本全域查无此条目（2026-10-19 grep 实测），本档保留此一行作可见性锚防丢账，随下次契约门处置后连本行一并删。

---

## 8. 风险·技术债·开放问题

### 8.1 架构风险与技术债记账（明知而暂欠的）

| # | 风险/债 | 影响 | 缓解/接受 | owner |
|---|---|---|---|---|
| 1 | 引擎部署副本补丁漂移（bind-mount 覆盖官方镜像 core/） | 镜像更新即丢补丁或语义不匹配 | 标记行机械报警锚 + 升级前对上游 diff；ARQ-260907-engine-deploy-patch-drift | dev-loop 运维面 |
| 2 | live 五齿留红：L4 渲染回执未达（设备仅回 ack:false 受理帧，疑 `mcp_server.cc` 判定门未生效） | 提醒出队机制 live 不完整 | 已如案登记（260911 review-packet）；Phase 1 修复 | dev-loop |
| 3 | 设备闲置 ~3 分钟休眠断连 → `alert_skip` | 提醒不可达窗口 | **已闭账**：human 2026-10-10 触点①裁「永远不休眠」（决策 29）——服务端/网络侧断自动重连 + 会话收尾常驻重建随 261010 包根治，离线驻留窗口消除；电池续航代价随裁定接受 | 已闭（原 owner human） |
| 4 | 查字冷启动 ~109s（DeepSeek 侧生成慢） | NFR-PERF-2 黄灯 | char_cache 预热已缓解一部分；流式/异步优化 = Phase 1 修复优先项 | dev-loop |
| 5 | live 查字链路缺陷两件（2026-09-15 真机串口实证定位，原「链路不可跑」结案）：① taskLVGL 栈溢出——CustomLcdDisplay 自建 lvgl_port_init 用默认 7168 栈 < char_page 动画帧（pts/sc/toks 256 点局部数组 ≈6.5KB）需求，panic 实证两轮；16K 一次性补丁真机验证有效（烧在 `custom_lcd_display.cc:104`，跳过流程如实记录）。② 板端 SVG 解析器（`char_stroke.c`，U 冻结合约）缺 C 命令支持——makemeahanzi 数据含三次贝塞尔 `C`，勇 9 笔中 6 笔被 `default: return -1` 整笔丢弃、示范字残缺 | S-F3.1/3.2/3.3 示范字不完整；崩溃已消（补丁）、残缺未修 | 正式修复走 dev-loop：char_page 预解析去栈 + 栈定容（①）+ `case 'C'` 三次贝塞尔采样 + acceptance 补 C 用例（②）；补丁行随包收编 | dev-loop |
| 6 | F2 改名对账四项 + 键路由对账六项 + 电量 ADC 三处未接 + 思考态零调用（均【修复项】） | 文档与代码的漂移已成片 | requirements §2.3/F2 ⚠️ 对账清单 + §5.5.4；随 Phase 1 各阶段接线 | dev-loop |
| 7 | `server/deploy/README.md` prompt 现状曾写 v3、arch 曾写 v4（两处散文漂移） | 部署指引误导 | 已随 260925-prompt-v7-round-routing 收敛：README prompt 节改指针式（沿革归 carrier 唯一副本），arch §5.1/§5.5.5 刷新 v7【已闭合】 | dev-loop |
| 8 | 逐笔念笔名试点期笔名错误率未成批核验（requirements v1.55 恢复；「勇」单字零错误 = 唯一实测点，笔名由 LLM 自由生成、无权威数据源兜底） | LLM 猜错笔名 = 教学事故（孩子照错的学） | 试点期扩常用字样本人工核错误率；不达标 → 回退纯引导语或接权威笔名数据源 + D3 契约扩展 `stroke_names`（走契约门） | human（试跑采样）+ dev-loop（数据源方案） |
| 8 | OpenClaw live 恒 `Intent:nointent` 拒收 tools 数组，根因未查 | 引擎工具调用面退化 | ARQ-260907-openclaw-tools-rejection-rootcause | architect+dev-loop |
| 9 | 对话历史跨 UI 会话累积（决策 21 连带） | 上下文污染 or feature，未定 | human 拍板；结构级备选（方案二：接入网关对设备常驻连接、引擎按会话起停）留 Phase 1。**已闭账（2026-10-22，`261022-device-memory-prompt-overhaul`）**：定性 = feature（跨会话连续性 + 记忆画像的底座），决策本体 = ADR-015；上下文污染的重放双源风险由「引擎只发新回合 + 持久会话单一权威」结构性消除；残余 = 历史增长 / 压缩成本（见风险 14）。决策 21 所列结构级备选（方案二：网关设备腿常驻、引擎腿按会话起停）随 ADR-015 承载定案失效。 | human |
| 10 | 1bpp CJK 字号下限实测（原「16px 以下未实测」） | **已闭账**：14px 下限已实测定案，不再悬置 | 2026-09-15 九档真机对照 + 2026-10-28 复测在案；NFR-READ-1 的 14px 下限语义不变，ARQ-260831-small-cjk-1bpp-readability 已按 `261028-font-typography-scaleup` consumed-by 结项 | 已闭（原 owner 真机字体任务） |
| 13 | 记忆子系统单点（Honcho 五容器与 ECS 同机；画像 / 消息库在本机 postgres） | 故障 = 无记忆（对话不阻塞）；数据损坏 = 画像需自消息库 / backfill 重建；删除 = 运维手工 | 降级语义 §6.7；备份 / 恢复形态 = 不备份、靠消息库 / backfill 语料重建（human 2026-10-22 裁 OQ-10；重建源 = session-corpus 源语料 + OpenClaw 会话存储；重评触发 = 重建不可接受 / 家长提出恢复诉求）；数据不出网（NFR-PRIV-1） | 运维面 |
| 14 | 持久会话历史增长与 auto-compaction 成本 / 质量（ADR-015 连带） | 长会话 token 成本；压缩后早期细节可能丢失（长期事实另存于记忆画像，兜底） | P2 上线后观测；压缩默认 safeguard（参考实现 `docs/concepts/compaction.md`）；必要时调 `keepRecentTokens` | dev-loop 运维面（观察项） |
| 11 | shim 退役遗留：ECS 停服 + 8080 安全组未清 | 攻击面残留（探针已证不可达） | teardown 证据待补 | dev-loop |
| 12 | 验收齿可合法弱于契约（同族 ≥6 包）+ 跨包行为无显式 owner | 齿绿而契约未兑现 | ARQ-260911-tooth-strength-below-contract / cross-package-behavior-owner-gap | architect |

> 治理类未结项全景见 [docs/debt/gov/INDEX.md](debt/gov/INDEX.md)（debt 账本 gov 域；原 ARCH-REVIEW-QUEUE.md 已撤销，261019 批），本章只记架构相关面。

### 8.2 开放问题

1. ~~多轮会话连续性~~ **已定案（2026-09-11 V21 探针实证）→ 2026-10-22 再次定案（取代）**：2026-09-11 定「stateless 不续、**每轮透传历史**续」；2026-10-22 `261022-device-memory-prompt-overhaul` 起取代为「引擎请求携 `user=<device_id>`、只发本回合，OpenClaw 按标识维护**服务端持久会话**」（决策本体 = ADR-015）——旧路线承载方案关闭；契约 §四「会话连续性承载」同批改写为定案记载（ARQ-260902-multiturn-continuity 已于 2026-10-12 关闭，本批只清契约字面残留）。
2. **AEC 实测调优**【二期】：喊停误触发/漏触发阈值（机制侧 §5.1.1 音频组件；需求侧 requirements §2.1）——2026-09-13 定不纳入 Phase 1 后不再挡路。
3. **证书生命周期相位对账**：OTA vs 首期续证（ARQ-260902-cert-lifecycle-deadlock）——证书形态已定（≥10 年 + 轮换脚本 + 告警），human 已裁 accept-as-risk。

---

## 9. 演进路线（as-is → to-be）

> 架构侧演进视角；开发推进顺序权威 = requirements.md §6（Phase 1 四阶段）。

**as-is（MVP 已建基线，22 包 closed + happy-flow-host-capture 在办）**：方向 B 端云四层链路 live 全通（listen→ASR→TTS→音频下发 0 断链）；shim 退役、front 唯一公网入口落地；引擎 fence 剥离补丁 + front 校验 relay live；提醒 cron→pending.json→推送链路 live（L3 绿、L4 回执留红）；四页旧形态渲染（HOME→CHAR→SCHEDULE→STORY）；prompt v4 版本化接线；证书/overlay/secrets/cron 运维面就位。

**to-be（Phase 1 结构变更）**：① 键路由改造——dispatch 按 `layer_top` 分流 + `cycle_next` 拆「清层/进页」（BOOT 隔离、KEY 长按返回上一层、单双击消歧仅列表态）；② 60s 问答会话 timer 生产者接线（会话状态机激活；原 120s 录音态阈值随 v1.65 作废——记录会话自动收尾归网关卡壳计时器族〔§5.2 场景④〕，本条接线范围相应收窄为单阈值）；③ 思考态渲染接线；④ 撤 P4 常驻页 → 3 页循环 + `char-window` 覆盖窗；⑤ display 周网格数据（契约）；⑥ 新增 control 动作（故事删除/音量调节，随契约门）+ 端→网关 `kidpal_session_ctx` 发送/拦截（契约已定义；固件侧接线点 = application.cc 会话发起入口读 `kidpal_layer_top()`，front 侧 = 上行泵帧解析处拦截）；⑦ L4 渲染回执修复 + 查字 live 链路修复。

**二期触发条件**：语音喊停（AEC 阈值实测就绪时）、品牌唤醒词「你好，小伴」（WakeNet 模型替换）、OTA（证书相位对账先行）、机械内容过滤层（合规需要时）、多设备（token 形态切换）。**重构触发**：引擎镜像升级 = 补丁漂移核对；需求重基线 = portfolio 重切（committed 条款）。

---

## 10. 决策记录

**2026-09-14 起，新架构决策一律登记 [ADR.md](decisions/ADR.md)（append-only，五件格式）；本节以下为 MVP 时代决策历史档，冻结不再新增。** 产品级休眠裁定（决策 29）经 human 触点①显式指示随 dev 包 261010-device-reconnect-on-server-close 落本表，为既定例外。决策 20 的实施级修正见 ADR-002（提醒生产侧混合分层）与 ADR-003（出音链路借引擎既有音频链）；版本化制品接线不变式见 ADR-001（历史档正文保持原文，本档不内嵌 ADR 注记）。历史档内「本文件 §x.y」「§2.9 / §2.10 / §3.1」等自引用均指**迁移前旧版编号**，非本档现行编号；历史档中的「设备腿 / 引擎腿 / 脑腿」为链路分层旧称谓，现行 = 设备端 / 接入网关 / 引擎服务 / 编排大脑（见 [glossary.md](glossary.md)）。映射表见 [notes/arch-restructure-skeleton.md](notes/arch-restructure-skeleton.md)。

1. **顶层定位**：**OpenClaw 作编排层+主 Agent（大脑/记忆/工具），板子只作"会说话的显示器"终端**。所有数据/逻辑/记忆在 OpenClaw（课表存MD、故事存MD、笔顺由OpenClaw按格式输出）；板子只收音/放音/显示/按键。
2. **技术路线**：**基于小智(XiaoZhi V2.1.0) 二次开发**（官方已移植本板，含 AFE唤醒+AEC+VAD、全双工音频、显示、Wi-Fi、Opus、LVGL）。换后端接 Aliyun+OpenClaw + 重做 UI + 加业务。
3. **端云划分**：本地做**实时音频处理**（AEC/VAD/唤醒/喊停检测/会话管理），云端做**语义理解**（ASR/意图/对话/TTS/业务）。
4. **服务端架构**：**薄桥 shim（FastAPI ~200行，唯一公网入口）+ OpenClaw 自带 OpenAI 兼容 API**。OpenClaw 网关有 `POST /v1/chat/completions`（默认关闭，开启即可），跑完整 agent（意图/会话/读课表MD/存故事/查字/记忆/工具全包）。**业务网关/CRUD/意图层零自研**。shim 只做 ASR/TTS HTTP 口子 + 设备鉴权 + 提醒轮询。⚠️ **已被决策 9/18 取代**：方向 B 转官方小智服务端网关，HTTP shim 退役为已验证台阶（本文件 §2.2「迁移与既有资产」）。
5. **OpenClaw 部署**：阿里云成都 ECS（公网 ws://8.137.91.16:18789 已验证通，内网 172.26.11.60）。shim 与 OpenClaw 同机，loopback 调用，真 token 不出内网。
6. **ASR/TTS**：放 OpenClaw 机器（faster-whisper + edge-tts，待装），经 shim `/v1/asr`、`/v1/tts` 暴露。⚠️ **已被决策 19 取代**：ASR 云优先（faster-whisper 弃选）；edge-tts 保留为网关 provider。
7. **语音交互**：**按一下 KEY 启动会话**（≈唤醒词体验），免提连续对话，60秒超时/说"拜拜"/再按键 结束。二期加 WakeNet 唤醒词。⚠️ **2026-09-13 需求修订**：唤醒词升入 Phase 1——双入口（按 KEY / 喊「你好，小智」，MVP 开发期间已实测），决策 10 的唤醒功耗实测相应提前至 Phase 1 阶段。
8. **语音喊停（MVP 必做）**：**本地** AEC+VAD 停播（不依赖云端取消生成）。KEY 单击打断兜底。方向 B 的 WS 通道下，`abort` 消息使云端取消生成成为补充能力（省算力，不改变本地即时停播的主地位）。⚠️ **2026-09-13 human 改判**：语音喊停不纳入 Phase 1（原 MVP 计划必做、未交付）——Phase 1 打断 = KEY 单击（100% 可靠）；本地 AEC+VAD 喊停与 abort 云端取消生成为二期，机制与端云划分不变（见 §2.4 表注、§6 风险点 2）。
9. **端云通信（2026-09-02 更新）**：**WebSocket + Opus 流式（方向 B）**——human 裁定。服务端借鉴官方开源小智服务端实现（xinnan-tech/xiaozhi-esp32-server，MIT，活跃），不逐行自研流式编排。Pod S 的同步 HTTP 链路为架构验证台阶（260829-smoke-e2e live 5/5），迁移后退役。
10. **使用形态**：**便携电池为主**（18650），唤醒待机功耗需实测（2026-09-13 唤醒词升入 Phase 1 后，此实测相应提前至 Phase 1 阶段）。
11. **语言大脑**：OpenClaw + DeepSeek，意图理解、对话、任务分发、纠正/结束判定。
12. **汉字笔顺**：✅ D3 格式定稿——makemeahanzi 数据源（已实测跑通），OpenClaw 组 JSON（strokes SVG path + medians 骨架点），板子只渲染（LVGL 绘制 + 逐笔动画）。
13. **课表/故事存储**：OpenClaw 存 MD 文件；课表录入主路径为家长端。
14. **页面结构**：**4 页循环**（首页/课表/故事/查字），BOOT 单击循环切换。⚠️ 2026-09-14 requirements v1.36：撤 P4 查字常驻页，页循环 4→3（首页/课表/星星记录本）——本条为历史口径，现状见 §5.1.1（渲染节点层）。
15. **首页内容**：时间+日期+温湿度+今日日程+电量（常显信息牌）。⚠️ 2026-09-13 requirements v1.4 重排：「接下来日程+提醒」为常显主体、环境信息退居辅助条——本条为历史口径，现状见 requirements.md S-F1.1。
16. **屏幕**：ST7305 / 400×300 / 1bpp / 45-70FPS / 无前光。按键 ×3（KEY/BOOT/PWR）。
17. **图形库**：**沿用小智自带 LVGL**（单色模式），笔顺用 `lv_canvas` 绘制。——美术元素（吉祥物/手绘框/涂鸦）不走运行时绘制，走预栅格化 1bpp 资产（本文件 §3.1）。
18. **服务端实现形态（2026-09-02 新增 + grilling 定）**：**官方 Python 版**小智服务端（xinnan-tech/xiaozhi-esp32-server；Go 社区版弃选——社区小、provider 覆盖未验证）+ 适配插件——LLM 经 OpenAI 兼容端点配置级接入 OpenClaw（`type:openai`，base_url/api_key/model 可配；**流式请求禁带 `max_tokens`**）；TTS=edge-tts（可替换 provider）；ASR=阿里云云端（云优先，见决策 19）；自写部分仅结构化下发 push helper 与提醒定时推送（各约几十行）。
19. **ASR 云优先（2026-09-02 新增）**：前期用阿里云云 ASR（Qwen3-ASR-Flash / Aliyun 流式，与 ECS 同区、按量计费+免费额度），避免前期订阅服务器扩容；兜底触发 = 额度用尽 / 隐私需求变化 / **云 ASR 流式不达标（与 VAD 切句语义不兼容）**（V23 补）→ 升级 ECS 切本地 SenseVoice（官方服务端 `fun_local`，配置级切换、无返工）。「ASR = 可替换 provider」写为架构前提。**儿童数据出网记账（human 2026-09-03 确认，V29 定）**：① 语音音频 → 阿里云 ASR（境内）；② 转写文本 → OpenClaw → DeepSeek 云（deepseek-v4-flash）；③ 回复文本 → edge-tts（微软非官方接口）——三条路径均出内网，human 已知晓并接受；原「仅 ASR 链路」措辞废止。**出网路径④ 联网搜索（human 2026-10-12 裁，v1.62 增补注记，原文①–③不动）**：kidpal agent 实时事实查询回合（天气/新闻/今日热点类）经 OpenClaw 平台联网搜索（Tavily）出网查询，human 已知晓并接受；行为口径与接受证据权威 = requirements NFR-SEC-2 ④（v1.62），本条为投影记账。**出网路径⑤ 记忆画像管线（human 2026-10-22 裁，v1.67 增补注记，原文①–④不动）**：Honcho deriver 抽 observation → DeepSeek 云（与 ② 同域，非新增暴露面）+ embedding 向量化 → DashScope（阿里云 text-embedding-v4，与 ① ASR 同供应商同信任域），human 已知晓并接受；权威 = requirements NFR-SEC-2 ⑤，本条为投影记账。
20. **提醒模型（2026-09-02 新增 + 三轮 grilling 定）**：**按区块判范围**——「学校课程表」区块不提醒（在校例行）；「每周固定课表」区块（培训班，不论周中周末，**每周重复生成**）+ 一次性安排走三段阶梯（前一天晚上 20:30 带物 / 当天早上 07:45 日程 / 事前 15 分钟——**时刻为 2026-09-02 human 修订值**，全局常量**配在 OpenClaw 侧**、不进契约）；携带内容复用课表备注列、解析规则由 system prompt 约定（契约 B1 表格主体零变更；frontmatter `remind_before_min` 字段废止、待契约门裁）；定时器 = **OpenClaw 原生 cron**（2026-09-02 ECS 实测：调度器在跑、已有 class-reminder 类任务运行记录、`cron --message` 可定时触发 agent 回合）；**脑算网关推**（OpenClaw 算三档写 pending.json、网关只到点推送）；载体走 mcp（见决策 23）；**重试到确认**（60 秒超时重推、单档上限 3 次、不做重连即重推、过期静默作废），**渲染即确认出队**（KEY/BOOT 关弹窗即出队；BOOT 同时照常翻页。⚠️ 2026-09-14 requirements v1.39 翻转：覆盖层内 BOOT = 关弹窗回原页、不翻页——本条为历史口径，现状见 requirements.md §2.3。弹窗无超时驻留、多提醒排队逐个弹（录音态例外 = 排队延后，见 §2.9）；设备易失 id 集合幂等去重）。机制细节见 §2.9。
21. **连接策略（2026-09-02 grilling 定）**：常驻连接（设备开机即连、断线重连），保提醒推送/打断可达；省电不预设优化，功耗实测后再看（决策 10 便携电池优先）。**2026-09-14 查证 + 配置级修正（PM review 驱动，human 裁方案一）**：引擎默认 120 秒无语音关闭与常驻连接直接冲突——上游 `receiveAudioHandle.no_voice_close_connect` 在 120 秒无语音时给 LLM 塞 `end_prompt` 煽情告别语、播完断连，engine-patch 另有二道 `120+60=180` 秒静默硬关兜底；而网关提醒推送（§2.9）推的是「当前在线设备腿」，设备腿与引擎腿同生共死（逐连接代理），引擎反复断连 = 推送黑洞 + 重连 churn + LLM 越权告别语（真机日志 `su-a8-keypress-02` 印证：告别播完即 WS disconnected）。修正 = ECS 引擎 `.config.yaml` 追加 `close_connection_no_voice_time: 86400` + `end_prompt.enable: false`（抄档 `.config.yaml.bak-20260914-session-timeout`，docker restart 生效）——两道超时退为 24h 纯兜底、告别语关闭，常驻连接自此真实成立。已知连带：引擎侧对话上下文将跨 UI 会话累积（当 feature 或当 bug 待 human 拍）；结构级备选（方案二）= 网关设备腿常驻、引擎腿按会话起停，留 Phase 1 评估。⚠️ **2026-10-22 取代注记（部分）**：「引擎侧对话上下文将跨 UI 会话累积（当 feature 或当 bug 待 human 拍）」已定案为 **feature**（跨会话连续性 + 记忆画像的底座），并由 `261022-device-memory-prompt-overhaul` 结构性落实——引擎请求携 `user=<device_id>`、只发本回合，上下文累积的家移到 OpenClaw 服务端持久会话（决策本体 = ADR-015）；引擎 RAM 副本降为非权威、不重放。决策 21 其余（常驻连接 / 24h 兜底 / 告别语关闭）不变。
22. **传输安全与鉴权（2026-09-02 grilling 定 + 2026-09-03 拍板回灌）**：wss + 自签长期证书——**有效期 ≥10 年**（把到期死局推出产品生命周期，V25 定）+ 轮换机械脚本 + 到期前告警（owner = 方向 B 落地包；部署期相位对账：证书有效期 vs OTA 建立，若 OTA 必晚于首期续证则 OTA 提前或显式记账死局，ARQ-260902-cert-lifecycle-deadlock）；CA 嵌入固件的证书加载实现面见 dev-loop handoff V10。**设备 token = 全设备共享单 token**（human 2026-09-03 裁，V26 定；MVP 单设备场景；已知取舍 = 无每设备身份、单点泄露危及全部、多设备时转每设备唯一并记账）；强度 ≥128-bit 随机、服务端 constant-time 比对、配网时生成写入。**强制设备鉴权**——token 未设 = 拒绝服务，公网入口不留 validation-mode 后门。**18789 收敛 = 方向 B 落地前硬前置**（V28 定）：OpenClaw 18789 锁 127.0.0.1（安全组/防火墙，部署期断言），与 ARQ-260901 复议触发合并。
23. **协议载体（2026-09-02 grilling 定）**：display 指令与提醒推送统一走 `mcp` 工具调用（kidpal/display、kidpal/alert），设备 JSON-RPC 回执 = 送达确认（提醒出队的 ack 通道）；小智原生 alert/custom 通道弃用。
24. **离线边界（2026-09-02 grilling 定）**：设备不缓存事项/课表——断网无提醒、P1「接下来」与 P2 课表不可用，仅时间/温湿度/电量；「哑终端不存数据」纯洁性保持，离线兜底验收撤回。**介质禁令（V4 定）**：板端非易失介质（16MB Flash / SD / NVS）仅允许设备凭证与服务器地址（url/token/version）、预栅格化 1bpp 美术资产与字体、固件本体；一切业务/身份/事项状态（提醒易失 id 集合、P1 日程渲染缓冲、会话状态机）只能驻留 RAM、掉电即失、禁落任何非易失介质。**史实更正 + 窄例外（`261016-p3-story-body-sd-cache`，2026-10-17）**：「『哑终端不存数据』纯洁性保持」句的历史更正——介质禁令的原始动因是**当时的存储成本约束**（未用 TF 卡、RAM 存不下业务数据），并非产品原则；human 2026-10-16 原话逐字「业务数据不落盘 是因为之前没用TF卡，ram根本存不下 无奈之举」。TF 卡能力 2026-10-16 就绪（`261016-sd-card-mount`）后该约束解除：介质禁令**白名单增列「story 正文只读镜像」**（云端 story 归档仍为唯一真相源，板端不承载任何写入；断网时已留存正文照常回看）。**边界不变**：提醒 / 课表 / 日志等其余业务 / 身份 / 事项状态仍只驻 RAM、禁落非易失介质；「离线边界（断网无提醒）」部分语义不变。 **白名单增列 · 设备诊断日志（`261018-sd-log-persist`，2026-10-18）**：介质禁令白名单再增列**设备诊断日志**——设备自身运行日志（固件 `esp_log` 输出流），常开 + 有界轮转、目录总量 16MB 封顶（512KB × 32 个文件）、可删除；写者 = 固件低优先级写盘任务。它是**可观测性数据**：不承载任何业务状态、不供掉电恢复、不是业务真相源。**口径消歧**：本条目与原则 5 所列「提醒 / 课表 / 日志等其余业务数据」中的「日志」指**业务日志**（家长知情权 = OpenClaw 侧完整日志，§1.3 / requirements §7 开放问题 3）——业务日志仍不落板、语义不变；设备诊断日志属**架构 / 运维面**概念，不在该业务数据枚举内。**边界仍不变**：提醒 / 课表等其余业务 / 身份 / 事项状态仍只驻 RAM、禁落非易失介质；「离线边界（断网无提醒）」语义不变——诊断日志不提供任何离线功能。**隐私接受**：落卡内容含**对话原文（STT / LLM 转录与回复）**，human 2026-10-18 有意识接受；与 `TECH-261016-sd-story-confidentiality` 同族（儿童数据明文落可拔卡）、暴露面加宽（全部对话转录 vs 孩子主动录的故事），隐私记账见该 tech 条目。
25. **故事录入流式机制（2026-09-02 第三轮 grilling 定）**：manual 聆听模式 + 网关累积（开始信号 = E1 control 扩展 `start_recording`，停录整段转写归档）；闲聊（VAD 自动断句逐句进对话）与录故事（累积到停）两条聆听语义正式分流。机制细节见 §2.10。 ⚠️ **已被 ADR-013 取代（`261019-p3-record-dialogue-guide`，2026-10-19；261001-m4-k3-record-session-live-repair Package B 同批定形）**：「闲聊与录故事两条聆听语义正式分流」的分流本体保留；被取代的是第二条形态——「manual 聆听 + 网关累积（停录整段转写归档）」→「记录语境对话态 = 累积 + 卡壳轻引导 + 收尾一次性忠实整理」（requirements v1.65）。技术证成 = manual 模式 ASR 在 listen stop 才触发、录音期间产不出逐句 stt，收尾词前置检测物理不可能（261001 q8 真机复核）；产品证成 = 孩子对沉默设备讲完完整故事实际很难、对话态天然消解「提问被记录」代价（设计稿翻案说明，human 2026-10-19 逐段批准）。「机制细节见 §2.10」为迁移前旧编号悬空指针（本节头部已声明旧编号映射），实际承载 = §5.1.2 + §5.2 场景④（映射表 = notes/arch-restructure-skeleton.md）。
26. **日程数据刷新与故障兜底（2026-09-02 第三轮 grilling 定）**：P1「接下来」刷新三触发 = WS 连接建立 + 每日 0:00 跨天 + 新建日程后即时推（**producer 已名，V22 定**：QQ 录入路径 = OpenClaw 自改写即时触发重算+推送；直改 MD 路径显式不即时推、靠连接/跨天兜底）；「接下来」= **分组呈现**（周中白天学校课合并成一项「上学」取首节时刻，培训班/一次性安排逐项列出，分组由 OpenClaw 算，2026-09-02 human 定）；云端任一环节失败 = 固定安抚句「网络出小差了，等一会儿再试试吧」+ text 视图结束当轮，不静默重试、不曝技术错误。
27. **提醒主动播报（2026-09-02 human 定）**：提醒到达 = 提示音 + TTS 读出文案 + 弹窗驻留三件套；alert 载荷带 `speech` 字段，网关合成 TTS 随推送下发——孩子不看屏也能听到。
28. **通用问答（2026-09-02 human 定）**：新增 F4/S-F4.1——意图分类含闲聊/百科；答案纪律（口语化、3 句以内、儿童化）由 system prompt 钉死；显示 = **全文上屏滚动**（human 裁定，不走摘要方案）；字号下限待 1bpp 真机可读性实测（超限按可读字号、滚动承接）。「3 句以内」为生成端约束，**超限按有处理的失败模式**（网关截断/分屏）而非仅记遵循度缺陷（V32 定）。**内容安全姿态（human 2026-09-03 裁，V30 定）**：MVP 接受 system-prompt 级约束 + ACCEPT-AS-RISK 记账；明确排除「家长逐条审批」形态（第七人工门红线）；机械内容过滤层（policy-as-code）留作日后合规选项。

29. **设备永不休眠、常驻在线（2026-10-10 human 触点①裁定，原话「永远不休眠」）**：设备常驻在线、永不休眠——提醒推送可达性优先于闲置功耗；会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建，Idle 态恒持活 WS；服务端/网络侧断自动重连复用既有 TryStartAutoConnect 守卫与退避、本地关 8 调用点分流零误重连；板载 18650 电池续航代价随裁定一并接受（使用形态转随用随充）。落地 = dev 包 261010-device-reconnect-on-server-close；§3.3 / §6.4 / §8.1 残余同步闭账（本条）。

---

## 11. 术语表

项目黑话的唯一权威定义 = [glossary.md](glossary.md)（哑终端 / 方向 A·B / 端云四层链路 / 三段阶梯 / 星星记录本 / fence 剥离 / overlay / 钉点 / Track A·B / 齿 …）。本档与 requirements.md 共用，不在本档重复定义。

## 12. 变更记录

> 纪律：本文件每次改动都在下表追加一行（新行在上，倒序）——changeId、日期、改了哪些节、依据。**changeId 即定稿标记**（不另设文档版本号）；走 `/arch-change` loop 的变更，跳过流程的须在依据列写明「跳过流程 + 授权方」。头部无状态行（评审期本文件不被触碰——PROPOSAL 模式）；「已冻结」的消费判据 = 本节对应 changeId 行在场（launchPreconditions 消费）。锚稳定规则（头部）不变：改小节编号 = 有意识地同步改锚，`spec-anchor-check.mjs` 机检抓悬空。

| changeId | 日期 | 改动面 | 依据 |
|---|---|---|---|
| 261028-font-typography-scaleup | 2026-10-28 | §3.1 / §4 NFR-READ-1 映射 / §5.5.2 字体资产预算注记 / §5.5.3 字体管线换代（16px 正文 + 20px 标题、assets `.bin` + cbin 运行时装载 + 编译内 14px 兜底）/ §8.1 风险 10 闭账 / §12 本行 | delivery/review/261028-arch-review-font-typography-scaleup/PROPOSAL.md（需求面 v1.69 frozen 为语义输入、逐字不回改；契约面零动作；ADR-017 同批落） |
| 261025-spec-reconciliation-memory-writing-scope | 2026-10-26 | §4（NFR-PRIV-1 映射行：两把配置键口径 → 类级规则 + 内容归属）/ §5.1.2（front 行一次性回合携 `user` 落设备 session 键族；记忆子系统行指针核后不动）/ §6.7 改写（逐行枚举 → 类级规则 + 现行机制与界限 + 七行实例表 + 残余与未闭合申报八条 + 记忆身份补一次性回合归属 + 画像边界来源门换代） | delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md（需求面 v1.68 为语义输入、不回改；契约面零动作；依据 = requirements v1.68 + spine + TECH-261026 + 261025 部署包与 docs-delta + architect 2026-10-26 核实） |
| 261023-arch-review-memory-wiring-verified-behavior | 2026-10-23 | §1.3（记忆身份体系实测口径：设备级 session + 家庭单 owner 画像）/ §4（NFR-PRIV-1 映射行口径）/ §5.1.2（front 行括注、记忆子系统行）/ §5.4（记忆·会话消息行）/ §6.7 改写（逐回合判据 → 记忆写入范围 + 六行实况表 + 残余申报 + 记忆身份） | delivery/review/261023-arch-review-memory-wiring-verified-behavior/PROPOSAL.md（依据：human 2026-10-23 方案甲裁决 + 261022 批 advisory memory-wiring-advisory-20261023 §1/§5/§6；K3 冻结验收与生产接线换代归批 B） |
| 261022-device-memory-prompt-overhaul | 2026-10-22 | §1.1 / §1.2（+Honcho 自托管栈、+DashScope embedding、DeepSeek deriver 同域注记）/ §1.3（记忆身份体系范围声明）/ §2.2 总图（持久会话 + 记忆侧车）/ §3.1·§4（出网四条→五条、+NFR-PRIV-1 对策行）/ §5.1.2（+记忆子系统行；OpenClaw 本体行 / front 行 / 引擎补丁行）/ §5.1.3 / §5.2 场景②（动态 context 装配 + 课表摘要）/ §5.3.2（+Honcho 行）/ §5.3.3（Honcho 端口核验）/ §5.4（+记忆两行、对话历史两行）/ §5.5.5 整节重写（静态 / 动态承载 + ADR-001 重定义）/ §6.7 新增（记忆写入资格与画像）/ §8.1（风险 9 闭账、+13/14）/ §8.2（会话定案取代注记）/ §10 决策 19 ⑤、决策 21 取代注记 | delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md（契约面同批原子落：§四「会话连续性承载」定案改写 + §1.6 承载对账注记；ADR-014/015 + glossary 词条同批落） |
| 261021-p3-record-cancel-lifecycle | 2026-10-21 | §5.2 场景④ 补取消链显式化一句（设备丢弃 → 契约 §四 `kidpal_story_discard` → 网关按连接权威 cancel；断连 cancel 为兜底） | delivery/review/261021-arch-review-p3-record-cancel-lifecycle/PROPOSAL.md（契约面同批原子落：§四新增 `kidpal_story_discard` + 变更记录条；§1.4 `start_organizing` 行包络核对零改） |
| 261019-p3-record-dialogue-guide | 2026-10-19 | §10 决策 25 取代注记（悬空指针同修）+ ADR-013 新增（记录语境对话态 + 引擎侧抑制定形）+ §5.1.2 front/story.py 行与 §5.2 场景④ 链路重写（卡壳计时器族 / 引导注入 / 忠实整理 / `start_organizing`）+ §2.3 / §5.1.1 / §9 to-be② / §5.5.4 / §4 NFR 映射表 / §2.4 原则 3 联动对账 | delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md（契约面同批原子落：§1.4 新增 `start_organizing` + §1.4/§四措辞对账） |
| 261018-sd-log-persist | 2026-10-18 | §2.4 原则 5 / §5.3.1 存储行 / §5.4 新增「设备诊断日志」数据行 / §6.2 / §6.6 / §10 决策 24——介质禁令白名单增列「设备诊断日志（常开 + 有界轮转、目录总量 16MB 封顶、可删除）」+ 「日志 = 业务日志」口径消歧（设备诊断日志属架构 / 运维面） | delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md（ADR-012 同批落） |
| 261016-p3-story-body-sd-cache | 2026-10-17 | §2.1 / §2.4 原则 1·5 / §3.1 / §4 禁令重述点收窄 + §10 决策 24 carve-out 史实更正注记（「哑终端纯洁性」降回「存储成本约束」）+ §5.3.1 / §5.4 / §6.2 需求批镜像 | delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md（契约面同批原子落） |
| 261015-gov-review-changes-taxonomy | 2026-10-16 | §5.5.1 :278 仓结构行——`delivery/changes/` 指针更新为六目录新树（review/intake/retro/archive + 既有三家；changes/ 伞解散归档）。档 0 勘误通道，零语义变更 | delivery/review/261015-gov-review-changes-taxonomy/PROPOSAL.md（FROZEN v0.4，T6 第 3 条改写类） |
