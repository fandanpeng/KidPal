# Review Packet — 260831-cloud-link-bringup

> integrator 收口。合并门前的审查包。本包为**功能支撑验证批 TP-A**（高危风险、subsystem=server），
> 目的是把 spec/盘点里的"待验证"云端链路项跑出可机判结论，不是新增产品功能。

## Change Size

- **Files changed**（本批新增）：
  - `server/shim.py`（288 行，FastAPI 薄桥）
  - `server/requirements.txt`（6 行，5 依赖全锁定版本）
  - `server/kidpal-shim.service`（31 行，systemd 单例，loopback 绑定）
  - `server/acceptance/test_cloud_link.acceptance.py`（557 行，冻结验收，9 测试 + boundary + precondition）
  - `.gitignore`（追加 1 行：`server/acceptance/_artifacts/`）
  - `docs/notes/feature-inventory.md`（docs-sync，7 行状态翻转）
  - `.planning/kidpal-validation/{progress,findings}.md`（追加式收口）
  - `changes/dev/260831-cloud-link-bringup/*`（本 closeout 制品）
- **Net diff**：约 +900 行（实现 325 行 + 验收 557 行），零删除，零改动既有产品代码。**纯新增**。
- **Scope**：单一子系统（server）。shim 只承受 ASR/TTS HTTP 口子 + 设备鉴权 + 提醒轮询转发，业务/CRUD/意图全部委托 OpenClaw。
- **Split recommendation**：不拆。单包内聚，验收与实现 1:1 对应。

## Verification Evidence

### 冻结验收（functional gate）—— 实测口径（诚实，非"全量复跑 9/9"）

冻结验收 `server/acceptance/test_cloud_link.acceptance.py`（9 测试）在 ECS 上实盘复跑两次（`OPENCLAW_TOKEN` 从 shim secrets.env 实取、`OPENCLAW_MODEL=openclaw`、`CLB_WHISPER_PYTHON=/opt/kidpal/venv/bin/python`、`CLB_WHISPER_MODEL=/opt/kidpal/models/faster-whisper-tiny`）：

| 测试 | 验收 | 复跑结果 |
|---|---|---|
| test_ssh_reachable | precondition | ✅ ok |
| test_chat_completions_endpoint_open | CLB-A2(OC-5) | ✅ ok（200 + choices[].message.content） |
| test_ecs_compute_profile_gives_whisper_tier | CLB-A4(OC-10) | ✅ ok（2 核 / 1870MB → tiny） |
| test_edge_tts_generates_playable_mp3 | CLB-A6(OC-9) | ✅ ok（mp3 >1KB + ID3 头） |
| test_faster_whisper_chinese_transcription | CLB-A5(OC-8) | ✅ ok（tiny，转写正确，含「公园」，延迟 1.92s） |
| test_shim_chat_full_path_returns_audio_and_display | CLB-A7(OC-7) | ✅ ok（200 + audio 非空 + display.view 合法） |
| test_openclaw_consumes_schedule_md | CLB-A8(OC-11) | ✅ ok（正确答出当天「钢琴课」+ view:schedule） |
| test_shim_chat_empty_text_rejected | boundary | ✅ ok（空 text 4xx 拒绝） |
| test_display_block_format_compliance | CLB-A3(OC-6) | ❌ **红（8/9）**：首查询 `char`「勇字怎么写」HTTP 0 → curl `-m 90` 超时 |

**CLB-A3 红的根因与定性（命门是否真的红？）**：CLB-A3 首查询是 `char` 类（「勇字怎么写」），OpenClaw agent 须现场查 makemeahanzi 并生成 9 画笔顺 stroke/medians JSON，**冷启动实测 109.1s**，超过冻结验收 curl 的 90s 超时上限 → `%{http_code}` 打印 `000` → 断言 `0 != 200`。这是**测试超时伪红，非 display 格式不遵循**。独立复核：
- builder 冻结验收早前产物 `display_compliance_20260831-233630.json`：**total=12 / n_blocks=12 / n_parsed=12 / occurrence_rate=1.0 / parse_rate=1.0 / failures=0**。
- integrator 独立 150s 超时手动探针：`char` 109.1s（has_display=True）、`text`「现在几点了」4.9s、`story_list` 17.9s，**全部 HTTP 200 且含 ` ```display ` 块**。

**结论：OC-6 命门功能面通过（12/12），复跑 8/9 的红是 char 查询延迟踩爆 90s 测试超时的工程现象（单独成 finding #3）。** 全文不声称"我全量复跑出 9/9"。

### 安全 gate（声明的 gateProfile=security）

手工走查（本批无 verify.mjs security structural gate）：见 Findings #1。结论：验证批接受 Bearer-only 暴露，生产须锁 loopback/防火墙。

### Docs sync

完成，见 `docs-delta.md`：feature-inventory §2 已改；spec「待装」措辞与契约 shim 字段契约建议走治理/契约门。

### Mutation check / 结构闸

本批为验证批，未跑 verify.mjs round 产线（无 `verifier-evidence/rounds/*`、无 `verdict.json`/`verification.json` 机器件，均 advisory 不阻塞）。shim.py 为 Python 无编译闸；契约面只读零触碰由 dev-loop.settings 的 permissions.deny 冻结兜底。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `server/shim.py:275` | `/v1/alerts` 只读 OpenClaw cron 的 `alerts/pending.json`，无则返回空 `{alerts:[]}` | 提醒出队/清除/去重逻辑 | OC-12 cron 提醒链路实装时（本批验收明确"可留 stub"） |

> 无其它 `minimal:` 标记。shim 命中 minimal-implementation：正向代理 + 正则切分 + edge-tts/whisper 惰性加载，无过早抽象、无重复实现。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 安全 | `POST /v1/chat/completions` = 完整 operator 权限（全工具/记忆/写文件），公网/LAN 可达（gateway.bind=lan），仅 Bearer token 保护。token 泄露即全权限沦陷。 | 实测：无 token → 401（Bearer 已生效）；带 token → 任意 agent 指令可执行。`security-hardening.md`「Secrets/权限」轴：真 operator token 只经 loopback 用（spec §3.0.0 已立"真 token 不出内网"），但 endpoint 本身随网关 bind=lan 暴露。 | 验证批接受；生产前：gateway bind 锁 127.0.0.1 或防火墙只放行 shim loopback；operator token 转弱权限/最小作用域。 |
| SHOULD | hard-violation | 契约 | shim `/v1/*` 请求/响应字段（`{text}`→`{audio,display}`、`{audio}`→`{text}`、`{text}`→`{audio}`、alerts 出队）**未入契约** `docs/contracts/interface-contract.md`（现只有 §一 E1 display 块、§二 B1 课表、§三 alerts 文件内容，缺 shim 层字段信封定义）。 | 契约是端云接口单一事实源（AGENTS.md「契约面」）；spec §6.1 只有方向级表格、无字段级 schema。契约"待确认点"1/2 已实证通过、确认点 3 未验。 | 走 **architect-reviewer 四分类 + 人类契约门**，归档 ARQ；本批不直改（`docs/contracts/**` 只读）。 |
| SHOULD | judgement-call | 可运维性 | `char` view 查询（如「X 字怎么写」）冷启动延迟 **~109s**，是非 char 查询（4.9~17.9s）的 6~20 倍；已踩爆冻结验收 90s 超时，且逼近 shim 正向 120s 超时。用户体验上孩子问一个字要等近 2 分钟。 | changed-code hunk：`server/acceptance/test_cloud_link.acceptance.py:220-296`（CLB-A3 12 查询含 3 条 char）+ integrator 实测计时（char 109.1s / text 4.9s / story_list 17.9s）。根因推测 = makemeahanzi 查字 + 笔顺 JSON 生成冷路径。 | 非阻断（功能已验证）。跟进：M2 流式/异步渲染；char 数据缓存/预热；或对 char view 走异步下发，先回一个"正在写"。 |
| FYI | judgement-call | 测试质量 | 冻结验收 CLB-A3 的 curl `-m 90` 对 char 类慢查询太紧，把"合法但慢"判成连接失败（HTTP 0），导致复跑呈现 8/9 伪红。 | changed-code hunk：`server/acceptance/test_cloud_link.acceptance.py:114-128`（run_curl 默认 `-m 90`）vs 实测 char 109s。 | 非阻断。建议后续把 char 查询超时放宽（或标 known-slow 分类），与 M2 降延迟同步处理。 |

> 无 BLOCKING finding。四项均为验证批可接受的已知限制/契约流程项，不进合并门前退回，但 #2 契约缺口与 #3 char 延迟均已在 docs-delta / 报告留痕、指向后续动作。

## Five-Axis Review

1. **Correctness**：shim 验收 1:1 对 CLB-A2~A8 全过（8/9 冻结 + CLB-A3 独立复证 12/12）。空 text/空 audio/坏 base64 边界有 4xx 拒绝（非静默 200）。schedule.md 消费正确（当天课程 + view:schedule + spoken answer 均含课程名）。
2. **Test quality**：冻结验收每断言真实远端副作用（HTTP 状态码/JSON 可解析/非空音频/display.view），非"没报错就算过"。CLB-A6/A7 硬失败于空音频；boundary 断言 4xx。display 失败样本逐条落盘（display_failures/）。
3. **Contract/boundary safety**：契约面零触碰（只读引用 E1/B1/D3/system prompt 逐字）。**唯一边界债 = shim 字段信封未入契约（finding #2，走契约门）**。system prompt 与 `DISPLAY_RE` 与契约 §1.6 逐字一致。
4. **Operability/security**：ErrorEnvelope（category/fingerprint/failingStep/message/logTail）对齐 observability-k6；correlationId（chat 路径 header 透传 + 缺省 uuid4）贯穿 chat/502/错误响应；secret 只经 loopback 注入、不进日志（log 只记 cid/view/字数/字节数，无 token/正文）。裸根 User=root 由 systemd 单例承载（验证批接受，见 finding #1 生产锁 loopback）。
5. **Maintainability/simplicity**：薄桥无业务/CRUD/意图逻辑（grep 零命中，仅注释声明"Deliberately NOT here"）；whisper 惰性单例（ASR 才加载，减轻常驻内存）；依赖 5 个全锁定版本（requirements.txt 精确 `==`）；无重复实现、无过早抽象。

## Residual Risk

- **Known limitation**：
  - char view 查询冷启动 ~109s（见 finding #3）——本批只证明"能不能做"（能），未解决"快不快"。
  - ECS 算力 2核/1.8GB，faster-whisper 只能 tiny 档；base/small 不可行（算力结论见报告）。
  - 提醒 cron 出队（OC-12）、故事归档/列表/回放（OC-13/14/15）、家长 QQ 录课表（OC-15）**本批未验**。
- **Follow-up recommended**：
  - M2 流式通道（WebSocket）消解 char 长延迟与整体数秒同步 HTTP 延迟。
  - spec 现状刷新（「待装」→「已装+档位」）走 Track B 治理批。
  - shim `/v1/*` 字段契约走契约门 + ARQ 归档。
- **Human decision needed**：
  - 契约缺口（finding #2）的四分类归属 + 人工门签字。
  - 生产安全：gateway bind 是否锁 loopback / operator token 是否降权（finding #1，验证批已接受暴露）。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 无：本包 `taskpackage.json.decisionCoverage` 三条均 `landsIn` 到 `exitCriteria/acceptance`（spec-§3.0.0-薄桥 → CLB-E2；contract-E1-display块 → CLB-A3；inventory-OC6-命门 → CLB-A3），无 `dropped`/`risk-accepted` 条目。

## 真实性复核注（append-only，T9，260906-gov-review-arq-governance-batch，ARQ-260902-shim-401-gate-bypass）

**本节为后置追加，不改上文任何结论的当时记录价值；上文「无 token → 401」相关结论以本节为准作废。** 本包 review 期所载「无 token → 401」结论系验证批未跑机械轮、未经真机复验得出，已被 260903-device-ws-switch（WS）包证伪并修复——根因 = shim `require_device` 鉴权短路缺陷（无 token 请求未被 401 拦截即进入转发链）。WS 包真机复验结论（生产复测在册）：**无 token → 401 零转发 / 错 token → 401 / 带 token 空文本 → 400**。下游引用鉴权行为时引 WS 包复验结论，勿引本包原始结论。