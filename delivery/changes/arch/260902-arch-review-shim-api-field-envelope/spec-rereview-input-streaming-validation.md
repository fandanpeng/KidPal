# Spec 重审输入 · 流式方向深入验证记录（260902）

> 性质：ARQ-260902-shim-boundary-open-questions 待拍板两题（②音频编码/传输形态、③鉴权+提醒出队）的验证输入材料，供 spec 重审（预期载体 /arch-change 或 /review-arch）正式裁决。本文件记录验证过程与结论，**不是裁决本身**。
> 日期：2026-09-02。参与：主线程（OpenClaw 流式 live 探针 + 统筹）、固件审计子 agent（codebase-memory-auditor）、开源调研子 agent（general-purpose）。
> 上下文：契约门闸 B 三题中②③经 human 裁定「重新 review spec 后再裁」（见本目录 architecture-gate-evidence.md §4）；本轮 human 明确「问题 1 更倾向于 B（流式）」并要求深入验证。

---

## 1. 验证线一：OpenClaw `/v1/chat/completions` 流式能力（主线程 live 实测）

**方法**：经 `c:\tmp\ssh_helper.py`（paramiko，见用户 memory）SSH 至阿里云 ECS（8.137.91.16），在机内 loopback 对 OpenClaw 网关（127.0.0.1:18789，token 取自 `/opt/kidpal/shim/secrets.env`）发真实请求。调用形状照抄 `server/shim.py` 的 `openclaw_completion`（model=`openclaw/default`）。

**探针 1（流式 + `max_tokens:16`）**：SSE 流正常开始（`chat.completion.chunk` role delta），随后 `{"error":{"message":"internal error","type":"api_error"}}`。网关日志（journalctl -u openclaw-gateway）显示根因：`max_tokens` 截断 DeepSeek（deepseek-v4-flash）推理，agent 层连续报「reasoning-only assistant turn detected … retrying with visible-answer continuation」，两次重试耗尽后「surfacing incomplete-turn error」。**与流式能力无关，是 max_tokens 与推理模型不兼容。**

**探针 2（流式、不带 max_tokens）**：完整 OpenAI 兼容 SSE 流——
```
data: {"choices":[{"delta":{"role":"assistant"},...}]}
data: {"choices":[{"delta":{"content":"好"},...}]}
data: {"choices":[{"delta":{},"finish_reason":"stop"}]}
data: [DONE]
```
网关日志证实对 DeepSeek 上游本身即以 `text/event-stream` 消费（`contentType=text/event-stream`），run 以 `stopReason=stop` 正常结束。

**结论**：OpenClaw 的 OpenAI 兼容端点支持**真 token 级流式输出**，端到端（OpenClaw→DeepSeek）皆流式。**坑：流式请求不得携带 `max_tokens`。**

**ECS 资源快照（同时刻）**：内存总 1870MB、可用约 1067MB、swap 已用 341/3071MB；openclaw-gateway 约 450MB、shim（venv python）约 45MB（whisper 模型加载后另增）；磁盘 40G 用 52%、余约 19G；whisper 模型在 `/opt/kidpal/models/faster-whisper-tiny`。
**human 裁定（2026-09-02）**：服务器资源不够可以扩容——资源约束从风险清单移除，部署期内存压力不作决策障碍。

## 2. 验证线二：设备端复活小智原生流式管线（固件审计子 agent）

审计范围：`firmware/main/` 协议与音频面（图索引 `KidPal` + 逐文件直接读源回退，22 路径 coverage 核查）。要点：

1. **原生传输层在树、进 SOURCES、有编译产物**：`websocket_protocol.cc/.h`、`mqtt_protocol.cc/.h`、`protocol.cc/.h` 均在 `firmware/main/protocols/`；`CMakeLists.txt` 29–32 行无条件列入；build 树存在 `websocket_protocol.cc.obj`/`mqtt_protocol.cc.obj`。协议选择已由 Kconfig choice 改为运行时按 OTA 结果选择（`application.cc:474-494`）；`CONNECTION_TYPE_*` 项已移除，现仅 `USE_SHIM_PROTOCOL`（默认 n）等 SHIM 项（`Kconfig.projbuild:9-31`）。当前 `firmware/sdkconfig` 为原生构建（无 SHIM 项）。
2. **Opus 双向编解码完整**：走托管组件 `78/esp-opus-encoder` + `78/esp-opus`（均编译通过）。上行 16kHz 单声道 60ms 帧（`audio_service.cc:40`、`OPUS_FRAME_DURATION_MS=60` 于 `audio_service.h:38`）；下行解码率随服务端 hello 协商动态重建（`audio_service.cc:376-389`、`websocket_protocol.cc:240-250`），默认 24kHz/60ms（`protocol.h:86-87`）。
3. **流式播放链路完整**：`WebsocketProtocol::OnData` 二进制分支（`websocket_protocol.cc:111-146`）→ `OnIncomingAudio` 回调（`application.cc:505-509`，Speaking 态）→ `PushPacketToDecodeQueue` → `OpusCodecTask` 解码+重采样（`audio_service.cc:309-338`）→ 播放队列 → `AudioOutputTask`（`audio_service.cc:261-295`）→ `codec_->OutputData` 出声。上行对称（采集→处理→`opus_encoder_->Encode`→`SendAudio`）。
4. **Pod S 新增面与原生路径零冲突**：`ShimProtocol` 挂 Protocol 抽象、`#if CONFIG_USE_SHIM_PROTOCOL` 宏保护实例化（`application.cc:481-484`）；`PlayMp3`（`audio_service.cc:624-665`）仅 `shim_protocol.cc:418` 调用；`esp-libhelix-mp3` 依赖（`idf_component.yml:108`）为 Pod S 新增。三协议类独立共存，无同名符号/互斥宏。
5. **激活门勾连（唯一需注意的耦合）**：tenclass 激活旁路只作用于 shim 路径（`#if !CONFIG_USE_SHIM_PROTOCOL CheckNewVersion()`，`application.cc:327-332`）；原生 WebSocket 的服务器地址/token 来自 NVS 命名空间 `websocket` 键 `url`/`token`/`version`（`websocket_protocol.cc:83-86`），原由 OTA 激活写入（`ota.cc:164-183`）。
6. **结论：无硬阻塞。** 复活清单 = ①确认 `USE_SHIM_PROTOCOL=n`（当前 sdkconfig 已满足）；②服务器地址入口二选一：(a) `CONFIG_OTA_URL` 指向自建最小 OTA 应答端点（按上游格式回 `websocket:{url,token,version}`，协议最相容）、(b) 配网直接向 NVS 写入并短路激活（改动最小，需在 `application.cc` 加分支）；③其余全是现成能力。转 B 后死代码：`ShimProtocol`、`mp3_decoder`、`PlayMp3`、SHIM_* Kconfig 项（均无害，运行时不实例化）。

## 3. 验证线三：开源小智服务端实现调研（调研子 agent，GitHub API + 源码，2026-09-02 现抓）

| 实现 | 语言 | License | 规模/活跃度 | 备注 |
|---|---|---|---|---|
| xinnan-tech/xiaozhi-esp32-server（官方） | Python + Vue 管理台 | MIT | 10.5k star / 3.5k fork，pushed 2026-08-21，活跃 | 功能最全，docker/裸机 |
| hackers365/xiaozhi-esp32-server-golang | Go（Eino，全流式） | MIT | 403 star，活跃 | 含 `doc/openclaw_integration.md` 原生 OpenClaw 集成 |
| joey-zhou/xiaozhi-esp32-server-java | Java | MIT | 1.3k star，活跃 | 含管理平台 |

关键适配结论（以官方 Python 版为基准）：

- **协议**：完整实现小智 WebSocket 协议（hello 协商、BinaryProtocol v1/v2/v3、MQTT+UDP 双轨），Opus 上行 16kHz/60ms、下行 24kHz（`opuslib_next`）——与验证线二的设备端规格逐字吻合。
- **LLM 接 OpenClaw = 纯配置**：`type: openai`，`base_url`/`api_key`/`model_name` 全可配，直指 OpenClaw 网关。
- **TTS = edge-tts 为原生 provider**，配置即用；句内非 token 级（整句合成后发），但句级流水线（LLM token 流→句切分→TTS→Opus 帧下发）已远优于现状「等全量回复」。
- **ASR = 16 个内置 provider 可选**（FunASR/SenseVoice 本地与远端、Sherpa-ONNX、Aliyun、Tencent、Baidu、OpenAI-whisper 等），faster-whisper 非内置（若坚持用需写约 100–200 行插件）。**human 裁定（2026-09-02）：ASR 引擎不限 faster-whisper——部署期可直接选内置 provider，此项不再是成本驱动。**
- **结构化下发**（display JSON/笔顺/课表）：固件原生支持 `custom`（`CONFIG_RECEIVE_CUSTOM_MESSAGE`）/`mcp`（JSON-RPC tools/call）/`alert` 消息；服务端无一等 push helper，需自写约几十行（复用已有连接发送）。
- **定时提醒推送**：服务端无现成主动推送机制，需自建 cron + 复用连接 push `alert`（约几十行）——与问题 2 的三段阶梯提醒模型配套（见 §4）。
- **打断**：协议原生 `{"type":"abort"}` + server 侧 handler，实时打断现成。

**human 裁定（2026-09-02）**：项目非商用，不考虑许可/权重条款问题（官方实现均 MIT；edge-tts 为 LGPLv3 且走微软非官方接口，许可面不作顾虑；edge-tts 接口被封属运维风险而非许可风险，TTS provider 可换）。

### 3.1 ASR 引擎选型建议（候选决策，待 spec 重审钉死）

**human 定向（2026-09-02）**：优先用云部署 ASR，避免前期订阅服务器扩容的成本；等服务器真不够了再考虑扩容。据此选型改为分段策略。

**候选决策：云优先、本地兜底，两段走。**

**阶段一（MVP/前期）：阿里云云端 ASR**

- 首选候选：**Qwen3-ASR-Flash（DashScope）** 与 **Aliyun 流式识别（provider 名 aliyun/stream 系）** 二选一（部署期配置决定）——两者均为官方小智服务端自带 provider，配置级接入、零开发。
- 选择理由：
  1. **与 ECS 同云同区**（阿里云成都），链路延迟最低、账单统一、无跨云出口成本；
  2. **按量计费 + 免费额度**（DashScope 对新模型、NLS 对新服务均有免费额度，具体额度以部署期官方控制台为准）——KidPal 现用量极低（单设备、短句为主），云 ASR 在 MVP 期成本趋近于零，这正是「前期不扩容服务器」的落点；
  3. 支持流式识别，与方向 B 的实时链路对齐；
  4. 中文商用 ASR 第一梯队质量，对儿童口音稳健性优于 whisper-tiny。
- **取舍记录（诚实入账）**：儿童语音音频将经过阿里云端（境内服务商、仅 ASR 链路传输）。human 已定向接受此取舍，换取前期不订阅服务器扩容；契约层面应把「ASR 云服务 = 可替换 provider」写成架构前提，不形成硬依赖。

**阶段二（兜底/迁移路径）：本地 SenseVoice-Small（官方服务端 `fun_local` 路径）**

- 触发条件（任一）：云 ASR 免费额度用尽且费用不成比例；或产品形态变化要求音频不出服务器（隐私敏感场景）。
- 届时升级 ECS + 模型从 ModelScope 境内直拉（对照 faster-whisper 权重在 ECS 不可达的 huggingface.co）+ ASR provider 配置级切换为 `fun_local`——两段之间无返工，官方服务端换 provider 就是改配置。
- 备选梯度不变：Sherpa-ONNX（更省资源）、现有 faster-whisper-tiny 仅兜底复用。

**长音频注记**：故事口述（S-F2.2）与短句问话共用同一条引擎路径（云端流式识别 / 本地 VAD 切片），无需第二套；云端阶段成本随语音时长计，现用量级可忽略。

## 4. 问题 2 需求对齐记录：提醒体系重构（human 2026-09-02 拍板）

- **范围**：每周例行课**不提醒**（routine 无提醒意义，只在课表视图可查——P2 课表页 + P1「接下来」）；**所有一次性事项都提醒**。
- **触发时间 = 三段阶梯**（human 原话要点：「前一天晚上提醒带东西、第二天早上再提醒干什么、再加事情发生前半小时，就够了」）：
  1. **前一天晚上**（如 20:30）——提醒次日要带的东西（如画具），提前准备；
  2. **当天早上**（如 08:30）——今天什么时间干什么；
  3. **事项开始前 30 分钟**——临期提醒。
- **连带语义**（分析结论，待重审正式钉死）：现行 `remind_before_min`（例行课提前 N 分钟）机制废止；提醒由轮询（契约 §三「读到并清除」）改为**服务端经 WebSocket 推送 `alert` 消息**（固件端已有 `alert` 渲染处理），`/v1/alerts` 轮询端点退役；清除语义 = 每道提醒实例确认（KEY）后清、过期未送达档位静默作废不补发、已过去的档位自动跳过。

## 5. 验证后的方向性结论（待 spec 重审正式裁决）

1. **转 B（WebSocket + Opus 流式，借鉴官方开源小智服务端实现）**：三线证据齐备——设备端零硬阻塞（改动点仅服务器地址入口）、服务端配置级接入 + 小幅插件、OpenClaw 流式实测通过。原评估中「服务端需自建实时媒体层」的担忧被开源实现现状大幅削弱。
2. **契约门三题的答案指向**：Q②（音频编码/传输形态）= Opus + WebSocket 流式；CDQ-260902-shim-api-field-envelope 的拟新增节目标形状由「HTTP 字段信封」改为「小智协议剖面 + 我方消息子集（display/custom、alert、鉴权、出队/推送语义）」。Q③ 的出队语义随 §4 提醒模型重写。
3. **治理路由**：触已定决策 8/9（MVP 同步 HTTP）+ FROZEN v0.3 拆包 + Pod S 已验证链路 = 跨系统结构性变更 → Track A（/arch-change），由 spec 重审承载（同时触发 ARQ-260827-tier1-boot-review 首次对抗审）。

## 6. 风险与开放项（验证后净表）

| 项 | 性质 | 状态 |
|---|---|---|
| 流式请求带 `max_tokens` 触发 agent incomplete-turn 报错 | 实测坑 | 适配时禁用该参数（已实测定位） |
| edge-tts 微软非官方接口被封可能 | 运维风险（非许可） | TTS provider 可替换，不作阻塞 |
| 部署期内存压力 | 资源 | human 裁可扩容，不作决策障碍 |
| 设备服务器地址入口 (a) OTA 应答端点 / (b) NVS seed | 实现决策 | 留待实现包级裁定 |
| ASR 引擎选型 | 部署决策 | 候选决策 = 云优先分段：阶段一阿里云云 ASR（Qwen3-ASR-Flash / Aliyun 流式，官方服务端自带 provider、配置级接入、按量计费+免费额度），阶段二本地 SenseVoice 兜底（额度用尽或隐私需求变化时扩容切换，详见 §3.1）；human 定向云优先，待 spec 重审钉死 |
| 13 包重拆与 Pod S 制品处置 | 治理 | spec 重审 + Track A 承载 |

## 7. Spec grilling 定稿记录（2026-09-02，human × 主线程逐条拍板）

spec v0.2 直写后经 grilling 逐条拷问定稿十项（均已落 `docs/spec.md` 对应章节）：

1. 服务端实现 = 官方 Python 版小智服务端（Go 社区版弃选：社区小、provider 覆盖未验证）；
2. 连接策略 = 常驻连接，省电不预设优化、功耗实测后再看；
3. TLS = wss + 自签长期证书（无域名，CA 嵌入固件，轮换管理责任在自己）；
4. 鉴权 = 强制（token 未设 = 拒绝服务，不留 validation-mode 后门）；
5. 提醒排程 = 脑算网关推（OpenClaw cron 算三档时刻写 pending.json，网关只到点推送，业务逻辑 100% 在脑）；
6. 提醒重推 = 不重推（BOOT 翻页不出队不重推，下一档阶梯自然补上）；
7. 离线 = 不同步事项、断网无提醒，离线兜底验收撤回（哑终端不存数据的纯洁性保持）；
8. 载体 = display 与提醒统一走 mcp 工具调用（kidpal/display、kidpal/alert），设备 JSON-RPC 回执 = 送达确认；小智原生 alert/custom 弃用；
9. 携带内容 = 复用课表备注列（契约 B1 零变更，解析规则由 system prompt 约定）；三档时刻为全局常量配在 **OpenClaw 侧提醒配置**（补议裁定：不进契约 frontmatter，B1 真零变更）；
10. 设备服务器地址 = 配网直写 NVS（websocket 命名空间）+ 短路激活，OTA 应答端点留二期。

补议五点（同轮收尾自查）：① 多轮会话连续性列入部署期第一项验证（探针仅实证单轮）；② 「超 4 秒安抚」归属网关定时器；③ S-F2.2 故事录入措辞对齐流式（无「上传」动作）；④ 提醒推送成功即标送达、回执丢失不补弹窗；⑤ 三档时刻入 OpenClaw 配置（见上第 9 条）。

第二轮 grilling（提醒投递语义，human 质疑④「推两遍有啥风险」引发，**推翻④**）：
1. 投递基线 = **重试到确认**（推翻第一轮「推送成功即送达」——静默丢提醒是功能级失败，重复弹窗只是噪音且可幂等化解）；
2. 确认语义 = **渲染即确认**（弹窗渲染完成设备自动回付出队，KEY/BOOT 只是关闭动作；「孩子看了没按键」场景不再触发重推循环）；
3. 弹窗 = **无超时驻留**（human 裁）：KEY=关闭回原页、BOOT=关闭并照常翻页；多提醒**排队逐个弹**（不合并、不替换、不设上限）；
4. 重试参数 = 只做超时重推（60 秒、单档上限 3 次、不做重连即重推——human 裁简单版）、过期静默作废；设备易失已知 id 集合幂等去重（重启后重弹一次可接受）。

第三轮 grilling 验证记录（2026-09-02，human 要求当场验证）：
- **契约「待确认点 3」（OpenClaw 定时能力）验证通过**：ECS 实测 `openclaw cron status` = enabled/triggersEnabled、sqlite 持久化、4 任务在跑；其中 `class-reminder-0529`（cron 表达式、Asia/Shanghai、状态 ok、96 天前有运行记录）证明同类提醒任务已有实践。`cron add --message` 支持定时触发完整 agent 回合（LLM 组三档文案有落点）、`--command` 支持 shell 载荷。提醒排程定 = **OpenClaw 原生 cron**，脚本兜底降级为应急备选。

第三轮 grilling 定稿（全树扫描四问 + human 两处修订）：
1. 定时器 = OpenClaw 原生 cron（实测，见上）；
2. 故事录入 = manual 聆听模式 + 网关累积，开始信号 = E1 control 扩展 `start_recording`（契约 E1 增 action，随契约门过）；闲聊/录故事两条聆听语义正式分流；
3. P1「接下来」刷新三触发 = WS 连接建立 + 每日 0:00 跨天 + **新建日程后即时推**（human 补）；
4. 云端故障兜底 = 固定安抚句「网络出小差了，等一会儿再试试吧」+ text 视图（human 修正措辞：不说「不舒服」，避免孩子误解），不静默重试、不曝技术错误；
5. **提醒范围修订（human）**：从「每周例行不提醒 / 一次性都提醒」改为**按区块判**——「学校课程表」区块不提醒；「每周固定课表」区块（培训班，不论周中周末，每周重复生成）+ 一次性安排走三档阶梯。

第四轮（2026-09-02，human 手改 + 四问）：
- **提醒时刻 human 手改**：前晚 20:30 不变，当天早上 08:30 → **07:45**，事前 30 分钟 → **事前 15 分钟**；全仓漏改已核（决策 20 同步），并钉死「纯服务端配置、改时刻即时生效、永不烧板子」；
- **提醒主动播报（human 定）**：提示音 + TTS 读出文案 + 弹窗驻留三件套，alert 载荷带 speech、网关合成 TTS 随推送下发；
- **通用问答补设计（human 指出缺口）**：新增 §4.4 F4 / S-F4.1——答案纪律口语化 3 句内儿童化（system prompt 钉），显示 = **全文上屏滚动（human 裁定，否摘要方案）**，字号下限挂 1bpp 可读性实测 ARQ；
- **P1「接下来」分组（human 定）**：周中白天学校课合并为「XX:XX 上学」（取首节），培训班/一次性逐项列，分组由 OpenClaw 算；
- **服务端代码落地方式**：部署包第一步 = clone xinnan-tech/xiaozhi-esp32-server 到 ECS + provider 配置 + 两 helper；提前熟悉性克隆可随时做。

十项均标「待对抗 review 冻结」，正式冻结走 spec 重审。

## 附：本记录的产生方式与证据位置

- OpenClaw 探针与资源快照：主线程经 `c:\tmp\ssh_helper.py` SSH 实测（命令与原始输出摘要见 §1；探针时刻约 2026-09-02 10:03 CST，journalctl 日志在 ECS 本机）。
- 固件审计：codebase-memory-auditor 子 agent，逐文件读源回退，引用均带文件:行号（§2）。
- 开源调研：general-purpose 子 agent，GitHub API 元数据 + 仓库 config.yaml/源码/协议文档现抓（§3）。
- 本文件为验证记录，不改变 CDQ-260902 状态（仍 PRECHECKED 暂停）；正式裁决走 spec 重审。
