# CONTRACT-CHANGE-PACKET · 260902-arch-review-shim-api-field-envelope（方向 B 版）

> Gate: ARQ-260901-shim-api-contract-gap（260829-shim-skeleton 包 1 launch 前置闸 B）
> Date: 2026-09-02（方向 B 重拟版；取代同目录 HTTP 四端点字段信封版）
> Status: PRECHECKED（architect-reviewer 四分类 = ① 随 spec 改动携带，human 未签字）
> 变更面：docs/contracts/interface-contract.md —— §三 重写为推送语义 + 拟新增「端云协议剖面（小智协议消息子集）」节 + §1.5 shim 职责收敛 + §1.4 E1 control 扩 start_recording + B1 frontmatter remind_before_min 漂移清理
> 落盘说明：本 packet 由 architect-reviewer 产出，该角色无写槽（scope-map deny 契约面），由主线程按 .claude/references/contract-change-evidence.md 代为落盘，逐字保真。

## 1. 变更范围（Gate Scope）

In-scope（契约面四类触点，全部由 spec v0.2 方向 B 携带）：
- §三（docs/contracts/interface-contract.md:202-218）由「提醒文件结构 + 设备轮询 /v1/alerts + 读到并清除」重写为「提醒推送与出队语义」——载体 mcp kidpal/alert、设备渲染回执出队、重试到确认、speech 字段。
- 拟新增一节（编号 human 定，建议 §四）「端云协议剖面」——定义小智协议消息子集及载荷 shape：hello 握手（Opus 16kHz/60ms 单声道自报 + 服务端回 session_id + audio_params 24kHz，握手头带设备弱 token Bearer）；上行/下行 Opus 二进制帧（BinaryProtocol v2/v3）；stt/tts 状态消息（stt 识别文字上屏；tts start/sentence_start 带文字/stop）；abort 消息（端→云取消生成）；mcp 工具调用（kidpal/display 载 E1 display JSON、kidpal/alert 载提醒条目 + speech）；设备 JSON-RPC 回执 = 送达确认（出队 ack）；强制鉴权（token 未设 = 拒绝服务，不留 validation-mode 后门）。
- §1.5 shim 职责（:136-142）收敛：(a)「小智协议 tts + Opus」对齐为 stt/tts 状态消息 + 下行 Opus；(b)「小智协议 mcp 或自定义消息」收敛为「mcp 工具调用 kidpal/display」，并注明小智原生 alert/custom 通道弃用；(c)「薄桥 shim」措辞改「网关 provider + push helper」形态（薄桥红线：零自研业务逻辑，不变，spec:87）。
- §1.4 E1 control 动作表（:131-134）新增 `start_recording`（故事录入流式，spec:363、决策 25 spec:587）。
- B1 frontmatter `remind_before_min` 字段（:164）+ §2.3 提醒描述（:189、:194）漂移清理：该字段语义（单一提前量 + cron 提前 N 分钟）已被三段阶梯（前晚 20:30 / 当天 07:45 / 事前 15 分钟，全局常量配在 OpenClaw 侧、不进契约）废止，需 human 拍板删除或标注「废止（legacy，不再消费）」。

Explicitly out-of-scope（零改动）：
- D3 笔顺 JSON（makemeahanzi strokes SVG path + medians，spec:140-165 已定稿，实测「勇」字 2KB 样本回归）。
- E1 ①–⑥ view 类型表（char/schedule/story_list/story_text/text/home 七 view 不变，S-F4 通用问答复用 text view，spec:435/545-547）。
- E1 display 块产出格式（```display 块 + {view,data,speech_hint?} 顶层结构）不变——变的是传输载体（HTTP shim 下发改 mcp kidpal/display 下发），非 block 本身。
- B1 表格主体（每周固定课表/一次性安排/学校课程表 三区块 + 各列）零变更；携带内容复用「备注」列、解析规则由 OpenClaw system prompt 约定（决策 20 spec:582）。
- OpenClaw /v1/chat/completions 调用侧形状（OpenClaw 自有 API，端云边界外）。
- 三档阶梯时刻值本身（20:30/07:45/15 分钟）：纯 OpenClaw 侧提醒配置，永不进契约 frontmatter（决策 20 spec:582）。

## 2. 提议 diff（proposedContractDiff —— 草案，human 拍板后定稿）

(一) §三 重写（推送语义，取代「提醒文件 + 轮询读到即清」）：
- 载体：mcp 工具调用 `kidpal/alert`（JSON-RPC tools/call），载荷形状（建议）：
  ```json
  { "id": "<reminder-instance-id>", "fire_at": "ISO8601+08:00",
    "title": "明日美术课", "speech": "明天美术课，别忘了带画具！",
    "view": { "view": "text", "data": { "text": "…" } } }
  ```
  `speech` = 网关合成 TTS 的文案，音频随推送一并下发（决策 27 spec:589，孩子不看屏也能听到）。
- 送达/出队语义（渲染即确认，非读即清）：设备在弹窗渲染完成时回 JSON-RPC 回执（tools/call result）→ 网关收到 = 出队；重试到确认 = 60 秒超时未收回执即重推、单档上限 3 次、不做重连即重推、过期仍未送达 = 静默作废不补发；设备持易失已知 id 集合幂等去重（决策 20 spec:582、决策 23 spec:585）。
- 删除：原「设备轮询 /v1/alerts → shim 读到并清除已下发项」整段（:218）废止（HTTP 轮询端点退役，Pod S 台阶性质，spec:90）。

(二) 新增「端云协议剖面」节（建议 §四；spec §6.1 spec:531-543 是通道概览，本契约节是格式事实源）：
- hello：设备自报 `opus 16kHz/60ms 单声道`；服务端回 `session_id` + `audio_params`（采样率/帧长，默认 24kHz）；握手头带 `Authorization: Bearer <device-token>`。
- Opus 二进制帧：上行 16kHz/60ms（BinaryProtocol v2/v3）；下行 24kHz 协商。
- stt/tts 状态消息：stt 识别文字上屏；tts `start`/`sentence_start`（带同步文字）/`stop` 播报状态机。
- abort 消息：端→云，喊停/打断补云端取消生成（本地 AEC+VAD 即时停播仍是主地位，spec:570/575）。
- mcp 工具调用载荷收编两种：`kidpal/display`（载 E1 display JSON，view=char/schedule/story_list/story_text/text/home/control）、`kidpal/alert`（载 §三提醒条目）；设备 JSON-RPC 回执 = 送达确认。小智原生 alert/custom 通道弃用（决策 23 spec:585）。
- 鉴权：强制。WS 握手 Bearer 设备弱 token；token 未设 = 拒绝服务，不留 validation-mode 后门；真 token 仅内网 loopback（决策 22 spec:584、spec:552）。

(三) §1.5 收敛 + §1.4 control 扩 start_recording + B1 漂移清理：见 §1 in-scope 逐条。

## 3. 四分类判定

① 随 spec 改动携带（同 PR）。反证全文见 precheck 判定（本 packet 附前）：非 ② 独立（驱动源 = human 已裁 spec 方向 B，原 ② 前提 HTTP 形态已作废）；非 ③ 整体漂移（spec v0.2 尚草稿、契约门本次即落实方向，非已冻结僵持——唯 B1 remind_before_min 为真实局部 ③ 漂移残留，单列）；非 ④ 整体无需改（轮询→推送为端云交互方向反转，非 adapter/字段映射/显示端细节可化解）。E1 ①–⑥ view 表 + D3 + B1 表格主体为 ④ 无需改面。

## 4. 影响（Impact）

- 消费方按其角色：固件（小智 WS 协议栈 + LVGL 渲染 + 提醒弹窗渲染回执）= 主消费方，须按新协议剖面节实现承载 + 出队；服务端（官方小智服务端 provider 配置 + 两个小 helper：结构化下发 push helper、提醒定时推送）；OpenClaw（经 OpenAI 兼容端点间接，display 块的产出者，格式不变）。
- 已跑通链路：D3 笔顺 makemeahanzi 管道零影响（display 经 mcp 下发的「载体」变了，但 display JSON 本身不变）；edge-tts TTS、ASR 换码发生在网关侧，契约不锁 provider 实现。
- server/shim.py 处置口径：HTTP 四端点（/v1/asr /v1/chat /v1/tts /v1/alerts）+ require_device + ErrorEnvelope（server/shim.py:196-291）是已并入 main 的「架构验证台阶」，不删不撤，spec:90 定义其为方向 B 落地后退役。其 LLM 调用形状（openclaw_completion + SYSTEM_PROMPT + DISPLAY_RE 切分，server/shim.py:102-132）是方向 B 下网关 push helper / OpenClaw provider 配置的迁移蓝本。处置 = 契约门本轮只裁决契约形状；shim.py 退役与迁移归 dev-loop 后续「方向 B 落地包」，按「Landed 后 impl 对齐」处理，非契约破坏。require_device 强制鉴权纪律（server/shim.py:84-99，对应 ARQ-260902-shim-401-gate-bypass 已修的 HTTPException 短路）沿用到方向 B 握手 Bearer。
- Pod S 冻结验收处置口径：260829-smoke-e2e 的 HTTP 链路 live 5/5 为历史实证（架构台阶，spec:90、:543），不作废、不回滚、不撤销；方向 B 落地后其 HTTP 形态不再作为功能验收依据，方向 B 需新的 WS 链路验收（网关 provider 接入 + 设备切 WS + display/提醒走新通道）。
- 安全面强化：方向 B 强制鉴权（token 未设 = 拒绝服务）从「漏洞带病」转为「契约级强制」，比现 impl 的 validation-mode 缺省更硬，儿童隐私/设备弱 token 边界提升。

## 5. 给 human 的契约门提问（承接原三题，附本轮建议答案供人裁、非自批）

1. （批准面）是否批准把契约从 HTTP 同步形态重写为方向 B 小智协议剖面——即新增「端云协议剖面」节 + §三重写推送语义 + §1.5 shim 职责收敛 + §1.4 E1 control 扩 start_recording？批准范围 = 上述面；D3 笔顺 JSON / E1 ①-⑥ view 表 / B1 表格主体零改动。批准 / 拒绝？
2. （音频编码/传输形态）确认设备↔云走 WebSocket + Opus（上行 16kHz/60ms 单声道、下行 24kHz 协商），display/提醒经 mcp 工具调用（非 JSON 内嵌 base64 mp3）？本轮建议答案：已由 spec §6.3 spec:551 + §6.1 spec:534 定稿，建议批准。
3. （鉴权 + 出队）确认鉴权强制（WS 握手 Bearer 设备弱 token，token 未设 = 拒绝服务，不留 validation-mode 后门）+ 提醒出队 = 渲染回执（非读即清；重试到确认：60 秒超时重推、单档上限 3 次、过期静默作废）+ alert 载荷带 speech 字段、网关合成 TTS 随推下？本轮建议答案：鉴权已由决策 22 spec:584 定稿；出队已由决策 20/23 spec:582/585 定稿；speech 已由决策 27 spec:589 定稿。建议批准。
4. （附带漂移裁定点）B1 frontmatter `remind_before_min` 字段（docs/contracts/interface-contract.md:164 + :189/:194）与新三段阶梯模型冲突、spec 自称「B1 零变更」与实况不符——请裁：删除该字段，还是保留但标注「legacy/废止、不再消费」？（提醒架构师注意：无论哪种，B1 都非 spec 所称「零变更」，建议同步修正 spec 决策 20 措辞或归档本点。）同时请确认 §3.0.2 spec:178 的轮询残留句在 spec 冻结前同步清理。

## 6. 验证计划（落地时）

- 契约 diff 逐字核对：D3 笔顺 JSON / E1 ①-⑥ view 表 / B1 表格主体零 diff；新增协议剖面节 + §三推送重写 + §1.5 收敛 + control 扩 action + B1 漂移清理逐字核对。
- 固件端承载核对：本协议剖面节的 hello/Opus 帧/stt/tts/abort/mcp + JSON-RPC 回执，是否逐字吻合小智原生 WS 协议栈能力（对齐固件审计：firmware/main/protocols/websocket_protocol.cc、audio_service.cc Opus 16kHz/60ms、CONFIG_RECEIVE_CUSTOM_MESSAGE/mcp 支持，spec-rereview-input-streaming-validation.md §2）。
- 服务端承载核对：官方小智服务端 provider 配置（LLM type:openai、TTS edge-tts、ASR 阿里云）+ 自写 push helper 对本节 mcp kidpal/display、kidpal/alert 与提醒定时推送的覆盖（对齐验证线三，spec-rereview-input-streaming-validation.md §3）。
- 端云联调冒烟：方向 B 落地后对「勇」字 D3 笔顺 JSON 实测样本回归（display 经 mcp kidpal/display 下发的载体变化实测）；提醒推送/回执出队/幂等去重的 mcp kidpal/alert 冒烟。
- spec↔contract 机械门本仓无脚本兜底（ARQ-260827-contract-mech-gate-gap），降级人工核对并如实记录。

## 7. 消费方一览

固件（小智 WS 协议栈 + LVGL 渲染 + 提醒弹窗渲染回执，主消费方）· 服务端（官方小智服务端 provider 配置 + push helper + 提醒推送）· OpenClaw（经 OpenAI 兼容端点间接，display 块产出者）· 设备鉴权（儿童隐私/设备弱 token，安全边界消费者）。
