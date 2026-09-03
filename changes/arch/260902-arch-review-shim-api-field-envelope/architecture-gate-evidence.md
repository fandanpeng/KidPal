# Architecture Gate Evidence · 260902-arch-review-shim-api-field-envelope

> Gate: ARQ-260901-shim-api-contract-gap（kidpal-mvp PROPOSAL §4/§7 闸 B，260829-shim-skeleton 包 1 launch 前置）
> Date: 2026-09-02
> Status: PRECHECKED

## 1. Gate Scope

变更面：`docs/contracts/interface-contract.md` 拟新增单一节（编号 human 定，建议 §四）——shim `/v1/asr /v1/chat /v1/tts /v1/alerts` 请求/响应字段信封 schema（含音频编码、设备鉴权、错误信封、X-Correlation-Id、alerts 出队语义）。owner = 契约门（architect-reviewer 预审 + human 签字）。显式 out-of-scope：E1 display 块 / B1 课表 MD / D3 笔顺 JSON / 提醒文件 content shape / §1.5 薄桥职责边界 / OpenClaw 自有 API 形状。

## 2. Inputs Reviewed

- CONTRACT-CHANGE-PACKET.md（同目录）
- `docs/contracts/interface-contract.md`（全文；章节止于 §三 + 待确认点）
- `docs/spec.md` §3.0.0 / §6.1 / §6.3 / §9（音频格式与字段信封的「待定」原文出处）
- `server/shim.py`（de-facto 信封形状、鉴权形态、alerts 出队现状）
- `docs/ARCH-REVIEW-QUEUE.md` §1.2 ARQ-260901-shim-api-contract-gap 行
- `.planning/kidpal-mvp/PROPOSAL.md` §4（闸 B）/ §7（前置闸 B）
- `.claude/taskpackages/260829-shim-skeleton/taskpackage.json`（launch 前置闸声明）
- 先例 packet：`changes/arch/260829-arch-review-kidpal-spec-stories/CONTRACT-CHANGE-PACKET.md`（CDQ-260829-storylist-total）

## 3. Architect-Reviewer Classification

| 变更面 | 分类 | 理由与权威引用 |
|---|---|---|
| `interface-contract.md` 新增 shim /v1/* 信封节 | `CONTRACT_CHANGE_REQUIRED`（② 独立契约变更） | 契约对 /v1/* 信封零定义、spec §9 line 575 显式承认为开放问题、impl 已自成事实形状；不随 spec 编辑携带（非①）、非已批准面互相矛盾（非③）、无现有字段可承载（非④）。反证全文见 packet §3。 |

预审执行：architect-reviewer（主线程派发，2026-09-02，后台两轮，产出经 SendMessage 回收；该角色无写槽，制品由主线程代为落盘并逐字保真）。

## 4. Human / Architecture Decision

2026-09-02 契约门三题与 human 对话记录（**诚实修正**：本节早稿曾把 Q② 记为「human 裁流式优先」，human 明示「这两个问题，我都没有给答案」，现修正如实）：

- **Q①（批准面）**：human 批准新增 shim /v1/* 信封节；但 ②③ 未裁，该批准所指的目标形状（一问一答信封）因 Q② 悬而未决而**悬置**。
- **Q②（音频编码 / 传输形态）**：**未裁**。分析背景：现状下行 base64 mp3 / 上行 WAV PCM16（Pod S live 验证全链路通）；xiaozhi 生态查证 = WebSocket + Opus 16kHz/60ms 流式，服务于实时流式架构；本仓 spec 已定决策 8/9 = MVP 同步 HTTP、喊停靠本地 AEC+VAD（不依赖传输形态）。human 表达过倾向「流式优先（推翻重来）」的方向偏好，但明确表示未给答案。
- **Q③（鉴权 + 出队）**：**未裁**。分析背景：token = 设备与服务端共享密钥（设备侧 NVS / 服务端环境变量 / 请求头 Bearer 比对），现状漏洞 = 未设时鉴权整个关闭（validation mode），shim 后置 operator 级 OpenClaw、设备是儿童用品、401 门曾出真穿透事故；alerts 出队 = 已下发提醒何时从 pending.json 删除（契约 §三原文「读到并清除」impl 未实现），读即清 / ack 后清 / 幂等未定。human 裁「重新 review 业务流程」，未裁决。

**human 决定**：重新 review spec，之后再裁这两题（登记为 ARQ-260902-shim-boundary-open-questions）。本门随之暂停；Q① 批准悬置；本 packet 与全部分析材料（鉴权安全面、出队语义、音频格式调查、xiaozhi 生态事实）保留为 spec re-review 的输入。

**补记（2026-09-02 同日稍后，方向验证）**：human 就 Q② 明确「更倾向于 B（流式）」并要求深入验证。主线程完成三线验证：OpenClaw `stream:true` ECS 真机实测通过（完整 OpenAI 兼容 SSE 流；坑 = 流式请求不得带 `max_tokens`，实测触发 agent incomplete-turn 报错）；固件审计确认复活小智原生 WebSocket+Opus 流式管线零硬阻塞（改动点仅服务器地址入口：自建 OTA 应答端点 / 配网 NVS seed 二选一）；开源调研确认官方小智服务端实现（xinnan-tech/xiaozhi-esp32-server，MIT，活跃维护）可配置级接入 OpenClaw（LLM `type:openai`，base_url/api_key/model 可配）且 edge-tts 为原生 provider、ASR 有 16 个内置 provider。human 补充裁定三点：ASR 引擎不限 faster-whisper（可选内置 provider）、项目非商用不考虑许可问题、ECS 资源不足可扩容。Q③ 的出队语义随提醒需求重对齐：每周例行不提醒（仅课表视图可查），所有一次性事项走三段阶梯提醒（前一天晚上带物提醒 / 当天早上日程提醒 / 事前 30 分钟），提醒由轮询改为服务端推送（小智协议 `alert` 消息）。全部验证记录落盘本目录 `spec-rereview-input-streaming-validation.md`。**注意：Q②③ 本节状态维持「未裁」——方向性结论（转 B）是验证支撑，正式裁决走 spec 重审。**

## 5. Applied Diff Inventory

（未落地——等 human 裁决。预期：`docs/contracts/interface-contract.md` 单节新增，additive、零删除；若音频编码裁 Opus 则附带 `server/shim.py` 对齐任务，按「Landed 后 impl 对齐」处理。）

## 6. Closeout Conditions

- C1：human 三题裁决落盘本文件 §4（owner 主线程，status PENDING）。
- C2：裁决批准的契约节落地（含机械路径确认——本仓 docs/contracts/** 全面禁写，落地通道需 human 授权一次性放行或另行指定，先例 = Pod S 合并门「human 批准特殊放行 + 诚实记录」模式）（owner 待定，status PENDING）。
- C3：落地后验证（契约 diff 逐字核对 + shim.py 形状逐字段复核 + 401 门回归）（owner 待定，status PENDING）。
- C4：ARQ-260901-shim-api-contract-gap 移已结 + 措辞纠正（「§九」→「拟新增契约节（建议 §四）」；ARQ 与 PROPOSAL 留根行的笔误按纠正口径读，不回改历史行）（owner 主线程，status PENDING）。

## 7. Validation Evidence

（未落地，暂无。落地时按 packet §6 验证计划执行，原始日志入本目录 evidence/。）

## 8. Docs / Queue Sync

- `docs/CONTRACT-DECISION-QUEUE.md`：新增 CDQ-260902-shim-api-field-envelope 行（2026-09-02，PRECHECKED）。
- `docs/ARCH-REVIEW-QUEUE.md`：ARQ-260901-shim-api-contract-gap 保持未结，待 C2/C3 完成后移已结。
- `.planning/kidpal-mvp/progress.md`：append 闸 B 处置记录。
- 批级契约 gatekeeper 同步：本仓暂无批级结构 gatekeeper 跟踪 docs/contracts/** diff（无 SANCTIONED_CONTRACT_CHANGES 类机制），不适用，如实记录。

## 9. Rollback / Follow-up

- 回滚：契约节未落地前无可回滚物；落地后回滚 = 删除该新增节（纯新增、零耦合）+ 回退 CDQ 行。
- follow-up：spec §6.3/§9 的「待定」项随本裁决转「已定」的 spec 同步，归后续 spec 修订批携带（不属本契约门范围）。

## 10. Verdict

未结（PRECHECKED，暂停等待 human 重新 review spec）。Q②③ 未裁 → 契约形状无法定稿；包 1（260829-shim-skeleton）launch 维持暂停；dev 不得按本变更动工。问题与分析背景已登记 ARQ-260902-shim-boundary-open-questions，spec re-review 时为输入。

## 11. 方向 B 补记（2026-09-02，契约预审轮）

- **重分类**：architect-reviewer 复审四分类 = **① 随 spec 改动携带（同 PR）**（§三重写推送语义 + 新增「端云协议剖面」节 + §1.5 收敛 + E1 control 扩 start_recording）；原「② 独立」前提（HTTP 信封正名）被方向 B 作废。E1 ①–⑥ view 表 / D3 / B1 表格主体 = ④ 无需改；**B1 frontmatter `remind_before_min` 字段 = 局部 ③ 漂移残留**（spec 原自称「B1 零变更」与实况矛盾，human 需裁删除或标废止——契约门第 4 题）。
- **Spec 侧残留清理**（主线程，预审抓出即修）：§3.0.2 B1 描述的轮询残留句改为推送语义；frontmatter 提醒提前量描述标废止；决策 20 与 S-F1.4 的「B1 零变更」措辞精确化为「表格主体零变更、frontmatter 字段废止待裁」。
- **制品**：`CONTRACT-CHANGE-PACKET-v2-directionB.md` 落盘（architect-reviewer 原文、主线程逐字保真代落）；旧 HTTP 版更名 `CONTRACT-CHANGE-PACKET.v1-http-field-envelope.superseded.md` 保留历史。
- **契约门状态**：四题待 human 终裁——①批准范围（协议剖面节 + §三重写 + §1.5 收敛 + control 扩 action）②Opus+WS ③强制鉴权 + 渲染回执出队 + speech ④remind_before_min 删/标废止；②③ 已有基于 spec 定稿决策（决策 22/20/23/27）的建议答案。门裁决预期随 spec 重审（260902-arch-review-kidpal-spec-v0.2）收口后发生。
- **状态维持**：CDQ-260902 PRECHECKED 暂停（待 human 终裁）；包 1 launch 维持暂停。

## 12. 契约门终裁与原子落地（2026-09-03）

- **human 终裁（四题）**：① 批准契约按方向 B 重写（协议剖面节 + §三推送重写 + §1.5 收敛 + control 扩 start_recording；D3/E1 ①-⑥/B1 表体零改动）；② 确认 WS+Opus（上行 16kHz/60ms、下行 24kHz 协商），display/提醒经 mcp；③ 确认强制鉴权（未设=拒）+ 渲染回执出队 + alert 带 speech；④ `remind_before_min` **标废止**（保留字段历史可读）。
- **原子落地完成（human 一次性放行，如实记录）**：`docs/contracts/interface-contract.md` 全文重写落地（方向 B 修订注记 + §1.4 control 扩 start_recording + §1.5 网关职责收敛 + §2.2/§2.3 废止标注 + §三重写推送语义 + §四端云协议剖面新增 + 待确认点 3 标已实证）；`docs/spec.md` 标 **v0.2 FROZEN**（条件冻结四条件全满足）。
- **闭环**：CDQ-260902 转 CLOSED；ARQ-260902-shim-boundary-open-questions / ARQ-260827-tier1-boot-review 随冻结移已结索引；契约面此后重回冻结纪律（任何改动走四分类 + 人类契约门）。
- **放行模式**：「human 批准特殊放行 + 诚实记录」（同 Pod S 合并门先例）——契约面冻结红线对会话生效，本次写入经 human 于拍板现场明确放行。
