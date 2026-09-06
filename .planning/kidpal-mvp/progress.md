# kidpal-mvp · progress

> 执行日志（追加式）：改了什么、测了什么、失败/修复。

- 2026-08-29 portfolio-planner 完成拆包设计，PROPOSAL FROZEN v0.2（human 盖冻结），落 11 个 TaskPackage。
- 2026-09-01 功能支撑验证批（260831 三包）收口并入 main，主线程对照 validation-batch-conclusion.md review 本 workstream，human 拍板三项调整（待落 PROPOSAL v0.3）：
  1. **C1 char 查询 109s 修法 = 服务端静态字库＋目标字表预热**（human 2026-09-01 拍板）。瓶颈定位：延迟随输出体量放大（text 4.9s / story_list 17.9s / char 109.1s），大头是 DeepSeek 把 strokes/medians 坐标 JSON 逐 token 复述；makemeahanzi 是静态字库，应确定性查表、不该走生成路径。修法：「汉字→D3 块」查表拼装挂 OpenClaw 侧（不入 shim，守契约 §1.5 薄桥红线），LLM 只出意图＋字；落地前先做 usage/stream profiling 定死延迟三段比例。纳入包 4（260829-char-stroke）云侧范围。契约 §1.6 system prompt 措辞疑似随之改 → 包 4 launch 前走 architect-reviewer 四分类预审。
  2. **C2 美术落地归属 = 新增「美术资产集成包」**（human 选 (a)），依赖包 0，B 段合入；V3 设计稿是其前置输入，OQ-3(a) 三项目测 human 已看过、观感通过。
  3. **轻量事实更新一并入 v0.3**：契约待确认点 1（display 遵循度 12/12）/ 2（课表 MD 解析 OC-11）已实证 → 包 2、6 的 contractBlocked 解除；待确认点 3（提醒 cron）未验 → 包 7 维持；包 S 不再建 shim、直接复用验证批 `server/shim.py`，只做设备侧闭环；包 1 起点改为现有 shim，launch 前先处置 ARQ-260901-shim-api-contract-gap；9b 补验 16px 以下 CJK 真机可读性＋前光。
- 2026-09-01 portfolio-planner 出 PROPOSAL-v0.3-draft.md（12 包：新增 U `260901-ui-art-integration`，mergeOrder 11，关键路径 7 级），待 human 冻结。
- 2026-09-01 **char 数据「设备端热字包」候选方案缓议**（human 拍：「先不动设备了，真机测试后看看效果再说」）：评估结论为技术可行（低年级字集打包 ~0.5–1MB，assets 分区余量 ~4MB 装得下；渲染路径本就在板端；可换毫秒级首帧＋离线回看），代价是触 spec §7 决策 1「数据全在 OpenClaw」原则（需按「预取缓存」定性）、与美术/字体共享分区余量、字集增长无轻量通道。**复议触发 = 真机测试后看 char 查询实际体验**；届时两级形态（设备热包＋服务端全库）方案全文在本条上文讨论中。v0.3 维持纯服务端字库。
- 2026-09-01 **PROPOSAL FROZEN v0.3**（human 盖冻结）：architect 单轮 dev-side review GREEN（10 条建议级修正 F1–F10 全回灌，无回问项），三项拍板 C1/C2/C3 落地。**关键发现并纠错**：v0.2 文头「11 包」是 off-by-one（盘上实有 12 个 260829-* taskpackage），v0.3 加 U 后实为 **13 包**——PROPOSAL.md/review.md 计数引用已全量校正为 13。冻结 pin = `sha1:500df090a227865e`（raw-content sha1，LF 归一化），钉入 13 包 `origin.upstreamFrozen` + decision-coverage.json。
- 2026-09-01 **冻结后机械落地完成**：① PROPOSAL.md 正文替换为 v0.3（状态 FROZEN v0.3）；② 13 包 pin 全量钉入；③ 同步 7 个 taskpackage 内容——S 缩域（复用 server/shim.py）、包 1 起点前移（+闸 B）、包 2/6 解除 contractBlocked、包 4 云侧扩 C1（+CHR-A8/A9 +闸 A + testHost→hybrid）、9b 补 HWV-A7/A8 + dependsOn 扩 U + mergeOrder 12、9a mergeOrder 13；④ 新建 U 包 `260901-ui-art-integration`（mergeOrder 11）；⑤ portfolio-reconcile 初始化 13 Pod（9b/9a/U 已更新，board 13 pods）；⑥ decision-coverage.json 12→13（新增 S-UI美术层承载 U、contractBlockedPackages 只剩包 7）。

## 2026-09-02 Pod S（260829-smoke-e2e）首板冒烟进展（planner 记）

- 冻结验收落盘（锚点修复路线实证有效）→ 结构面红基线 7 RED/3 GREEN 实测吻合 → builder 设备侧实现（shim 协议/mp3 解码/配网承载，编译通过）→ mutation 战役 4 条全敏感（A2/A3 证据回填）。
- 过程两缺陷均已修复并真机复验：① acceptance-coverage 门 Python 断言识别缺口（human 批准一次性热修）；② server/shim.py require_device 门用 return JSONResponse 不短路、实测穿透（fixer 改 raise HTTPException，ECS 复验 401/400/无转发）。
- 真板冒烟（无麦/喇叭，human 拍板先验其余面）：烧录✓、NVS 注入（wifi+shim_base+token 整片 0x9000）✓、AP 配网✓、联网✓；首板发现启动序激活门（api.tenclass.net 重试）阻塞协议初始化 → builder 编译期旁路（镜像级死代码消除，tenclass/xiaozhi.me 字符串归零）→ 重烧后 `Shim protocol selected, base=http://8.137.91.16:8080`。
- BOOT 会话实证：3s 静音 WAV → 公网 POST /v1/asr（token 鉴权通过，服务端日志源 IP 111.18.42.121）→ 200 → 空文本 → 屏显 `shim: empty asr text`（显示管线实证）。**设备↔shim↔(asr) 公网链路串通**。
- 验收现态：8 GREEN / 2 RED。余两红同因：roundtrip.json 需真实声学往返（麦克风+喇叭），待硬件到位。
- 暴露面未回收（冒烟仍需）：shim 0.0.0.0:8080 + 安全组放行，待末次冒烟后回收。

### 补记（同日稍后）
- 冻结验收 live 类首跑暴露 3 个线束缺陷（2 处 `_ssh` 三元解包写成二元、`_remote_write` 内联大 payload 撞 Windows 命令行上限）——均为「从未执行过」的冻结件缺陷，acceptance-author 修复（断言语义零变更）。
- 修复后复跑：**结构面 8 GREEN / 2 RED（两红 = roundtrip.json，硬件段）+ live 5/5 全绿**（含 401 硬闸与真实 edge-tts→asr→chat→mp3 往返）。至此，无声学硬件可测的面已全部测毕。

## 2026-09-02 包 0/1 launch 批（主线程记）

- Pod S merged 后，按 PROPOSAL §3 调度起跑包 0 + 包 1（WIP=3 内）。
- **包 0（260829-host-test-infra）launch 就绪**：worktree `C:/AI/KidPal-pod-host-test-infra`（branch `dev/260829-host-test-infra`，base = main 53731c6）；launch 前置门 `portfolio-reconcile.mjs --check-launch` 对 main 绿；环境等价化完成（firmware/sdkconfig + dependencies.lock 镜像、managed_components junction 建好、worktree-bootstrap.mjs 跑过）；board `--set-pod queued --worktree-path` 预填完毕。
- **包 1（260829-shim-skeleton）闸 B 处置**：architect-reviewer 四分类预审 = **② 独立契约变更（CONTRACT_CHANGE_REQUIRED）**——shim `/v1/*` 字段信封 schema 在契约中零定义、spec §9 明示开放问题、impl 已自成事实形状。制品落盘 `changes/arch/260902-arch-review-shim-api-field-envelope/`（CONTRACT-CHANGE-PACKET.md + architecture-gate-evidence.md，architect-reviewer 无写槽、主线程代为落盘逐字保真），CDQ-260902-shim-api-field-envelope 登记（PRECHECKED）。**预审附带措辞纠正**：ARQ/PROPOSAL 所写「契约 §九」系笔误（契约章节止于 §三，开放问题在 spec §9），落盘口径统一为「拟新增契约节（建议 §四）」。待 human 契约门三题拍板：批准新增节 / 音频编码（base64 mp3 vs Opus）/ 设备鉴权 + alerts 出队语义。**包 1 launch 暂停等待本门**。
- **launch 形态受阻记录（诚实记录）**：无人值守 headless 配方（`claude -p` + `--dangerously-skip-permissions` + `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`，portfolio-planner def ③ 钦定）被本会话的 harness 安全 classifier 硬拦（创建含该 flag 的 launcher = hard block）。human 裁「你自行执行 headless」——包 0 启动命令与 prompt 文件（`.git/devloop/launch/prompt-260829-host-test-infra.txt`）已备好交 human 自跑。

### 闸 B 拍板现场补记（同日，契约门三题未收口）

- **契约门三题与 human 对话结果（如实）**：①批准面 = 批准新增节（但目标形状悬置）；②音频编码/传输形态、③鉴权+出队 = **human 明示未给答案**。分析过程留痕：音频侧查得 xiaozhi 生态 = WebSocket+Opus 流式（服务实时流式架构），本仓 spec 已定决策 8/9 = MVP 同步 HTTP + 喊停靠本地 AEC+VAD；human 有「流式优先（推翻重来）」方向偏好但未裁；鉴权侧讲清了 token = 设备与服务端共享密钥（设备 NVS / 服务端环境变量 / Bearer 比对）、现状漏洞 = 未设即关鉴权、alerts「清」清的是提醒队列非 token。
- **human 决定：重新 review spec，之后再裁②③**。已登记 **ARQ-260902-shim-boundary-open-questions**（§1.1 Blocker/待拍板，含两题全部分析背景）；CDQ-260902 状态更新为 PRECHECKED（暂停）；闸 B 制品（packet + evidence 台账）全保留，作 spec 重审输入。
- **对本批调度的影响**：包 1（260829-shim-skeleton）launch 维持暂停（契约形状未定稿、且 spec 重审可能改其范围）；包 0（260829-host-test-infra）纯 host 基建、传输形态无关，launch 就绪不受影响，命令已交 human。其余 11 包维持 queued，待 spec 重审结论再议是否重拆。

## 2026-09-02 契约门②③方向深入验证与落盘（主线程记）

- human 就契约门三题之②③给出方向：问题①（音频编码/传输形态）更倾向 B（WebSocket + Opus 流式，借鉴小智），要求深入验证；问题②（提醒/待办清除）重新对齐需求。
- 三线验证完成：① OpenClaw `stream:true` ECS 真机实测通过（完整 OpenAI 兼容 SSE 流，端到端真流式；坑 = 流式请求不得带 `max_tokens`，实测触发 agent incomplete-turn 报错）；② 固件审计（子 agent）：小智原生 WebSocket+Opus 流式管线在树、编译通过、与 Pod S 新增面零冲突、零硬阻塞，改动点仅服务器地址入口（自建 OTA 应答端点 / 配网 NVS seed 二选一），转 B 后死代码仅 ShimProtocol/mp3_decoder/PlayMp3（无害）；③ 开源调研（子 agent）：官方 xinnan-tech/xiaozhi-esp32-server（MIT，10.5k star，活跃）配置级接入 OpenClaw（LLM `type:openai`）、edge-tts 原生 provider、ASR 16 个内置 provider，需自写部分仅结构化下发与定时提醒 push helper（各约几十行）；Go/Java 社区版为备选（均 MIT）。
- human 补充裁定：ASR 引擎不限 faster-whisper；项目非商用，不考虑许可问题；ECS 资源不足可扩容（资源约束移出风险清单）。
- 问题②提醒需求拍板：每周例行课不提醒（仅课表视图可查）；所有一次性事项三段阶梯提醒（前一天晚上带物提醒、当天早上日程提醒、事前 30 分钟）；连带 = `remind_before_min` 机制废止、提醒由轮询改服务端推送（小智 `alert` 消息）、确认即清 + 过期静默作废不补发。
- 验证记录落盘 `changes/arch/260902-arch-review-shim-api-field-envelope/spec-rereview-input-streaming-validation.md`（含三线证据与风险净表）；architecture-gate-evidence.md §4 与 ARQ-260902 补记同步。方向性结论（转 B + 三段阶梯）待 spec 重审正式裁决，CDQ-260902 状态维持 PRECHECKED 暂停，包 1 launch 维持暂停。
- **ASR 引擎选型建议（human 要求给推荐，已写入方案材料 §3.1）**：首选 SenseVoice-Small 本地部署（官方小智服务端 `fun_local` 默认路径）——零开发配置即用、中文质量与儿童口音稳健性优于 whisper-tiny、ModelScope 境内可达（对照 faster-whisper 权重在 ECS 不可达的 huggingface.co）、本地推理保儿童语音不出服务器；备选梯度 = Sherpa-ONNX（更省资源）、现有 faster-whisper-tiny（仅兜底复用）。外部云 ASR（Aliyun NLS/豆包/讯飞等）因付费+音频出境排除。候选决策，待 spec 重审钉死。
- **ASR 选型定向更新（human，同日）**：优先云部署、避免前期订阅服务器扩容，不够再扩。方案材料 §3.1 改为分段方案：阶段一 = 阿里云云 ASR（Qwen3-ASR-Flash / Aliyun 流式识别，官方服务端自带 provider、配置级接入、与 ECS 同区低延迟、按量计费+免费额度、MVP 期成本趋近零）；阶段二 = 本地 SenseVoice（`fun_local`）兜底（额度用尽或隐私需求变化时扩容切换，配置级无返工）。儿童语音经阿里云端的取舍已记入方案材料、human 定向接受。
- **对抗审收口（adjudicator，同日）**：46 findings 跨支去重归并 38 条 → **35 CONFIRMED / 3 DISMISSED / 0 deadlock**；BLOCKING 5 条全 CONFIRMED（V01 v0.1 残留 live 互斥 / V03 板端存储三处矛盾 / V05 alert 双形态+「固件已有渲染」为假 / V08 spec↔契约漂移命中「契约未批不可冻结」铁律 / V14 幂等去重边界矛盾）；T5-F1「冻结切片悬空」被 defender SSOT 方向论驳回降 SHOULD。pass-oracle：轴覆盖/场景覆盖/canaries（6/6）GREEN，finding 全结 + spec↔contract 机械门 RED（待落地/契约门批后清）。结论 = **条件冻结**（四条件：契约门四题 human 批 / CONFIRMED spec-text 改法落地 / 11 组拍板回灌 / 实现侧 3 组（V10 CA 证书加载面、V11 强制鉴权服务端承载、V27 display JSON 校验）进 dev-loop handoff）。制品 = `changes/arch/260902-arch-review-kidpal-spec-v0.2/`（adjudications.md、review-packet.md、dev-loop-handoff.json、debate-log 追加）；ARQ 新增两条（ARQ-260902-cert-lifecycle-deadlock、ARQ-260902-multiturn-continuity）、ARQ-260827-tier1-boot-review 补记首审收口。待 human 拍板：11 组 + 契约门四题（review-packet §4，建议即地板）。
- **拍板 + 落地（2026-09-03）**：契约门四题全批（①方向 B 重写②WS+Opus③强制鉴权+渲染回执+speech④remind_before_min 标废止）；11 组拍板按推荐定稿、唯一偏离 = V26 token 选全设备共享（取舍已记账）。19 条 CONFIRMED spec-text + 全部拍板回灌已落 `docs/spec.md`（v0.1 残留清尽、提醒状态机改写为解耦四态、RAM-only 介质禁令、证书十年期、18789 硬前置、出网三条记账、内容安全姿态、录音显式回退、家长端 QQ 主/MD 副、§8 重切 pin 收紧、§9 补 owner/相位）。CDQ-260902 → HUMAN_APPROVED（待原子落地）；dev-loop handoff 机检绿（V10/V11/V27）。**剩最后一步：契约重写原子落地（human 一次性放行）+ spec 标 FROZEN。**
- **原子落地完成（2026-09-03，human 放行）**：`docs/contracts/interface-contract.md` 全文重写（方向 B 修订：§1.4 control 扩 start_recording、§1.5 网关职责收敛 + display 机械校验、§2.2/2.3 remind_before_min 标废止、§三重写推送语义、§四端云协议剖面新增、待确认点 3 标已实证）；`docs/spec.md` 标 **v0.2 FROZEN**；CDQ-260902 → CLOSED；ARQ 三条移已结索引（ARQ-260902-shim-boundary-open-questions / ARQ-260827-tier1-boot-review / ARQ-260901-shim-api-contract-gap）。契约面重回冻结纪律。**下一步 = portfolio 重切**（冻结即触发，owner = portfolio-planner，相位 = 紧随）：13 包按方向 B 重新拆包，实现侧 V10（证书加载面）/V11（强制鉴权承载）/V27（display JSON 校验）经 dev-loop handoff 进包。
- **spec v0.2 直写 + grilling 十项定稿（主线程 × human，同日）**：调研结论直写 `docs/spec.md`（架构/功能清单/S-F1.4/§6 接口面/决策 18–20/里程碑注记/开放问题，契约面未动）；随后 grilling 逐条拍板十项——服务端选官方 Python 版（弃 Go 版）、常驻连接（省电实测后再议）、wss 自签证书（无域名）、强制鉴权（不留 validation mode）、脑算网关推（pending.json 交接）、不重推（下一档补）、离线不同步（断网无提醒，兜底验收撤回）、display/提醒统一走 mcp（回执兼送达确认，原生 alert/custom 弃用）、携带内容复用备注列（B1 零变更）、配网直写 NVS（OTA 端点留二期）。全部落 spec 决策 18–24 与对应章节，输入材料补 §7 定稿记录；下一步按 human 指示交对抗 review（/arch-change）。
- **第二轮 grilling：提醒投递语义定稿（同日，推翻首轮「不重推」）**：human 质疑「推两遍有啥风险」→ 重试到确认（静默丢提醒是功能级失败、重复弹窗可幂等化解）；渲染即确认出队（KEY/BOOT 只关弹窗）；弹窗无超时驻留、多提醒排队逐个弹（KEY 关闭回原页/BOOT 关闭翻页）；重推参数 = 60 秒超时、单档上限 3 次、不做重连即重推、过期静默作废、设备易失 id 集合去重。已回写 spec S-F1.4 与决策 20、输入材料 §7。
- **第三轮 grilling：全树扫描四问定稿（同日）**：① 定时器 = OpenClaw 原生 cron——human 要求当场验证，ECS 实测调度器在跑（4 任务，含 `class-reminder-0529` 课程提醒先例），契约「待确认点 3」销账；② 故事录入 = manual 聆听模式 + 网关累积（E1 control 扩 `start_recording`，随契约门过）；③ P1 刷新三触发（连接/跨天/新建日程即时推，第三条 human 补）；④ 云端故障兜底安抚句（human 修正措辞：「网络出小差了」，不说「不舒服」）。另 human 修订提醒范围 = **按区块判**：学校课程表区块不提醒、每周固定课表区块（培训班每周重复生成）+ 一次性安排走三档阶梯。已回写 spec（S-F1.4 范围、B3、S-F2.2、S-F1.1、§4.0 错误兜底行、决策 20 修订 + 新增 25/26）与输入材料 §7。
- **第四轮：human 手改 + 四问定稿（同日）**：① 提醒时刻 human 手改为 20:30/07:45/事前 15 分钟，漏改核查（决策 20 同步）+ 钉死「纯服务端配置、永不烧板子」；② 提醒**主动播报**（提示音+TTS 读文案+弹窗三件套，alert 带 speech）；③ 补通用问答设计缺口：新增 §4.4 F4 / S-F4.1（答案口语化 3 句内儿童化，**全文上屏滚动** = human 裁定否摘要方案）；④ P1「接下来」**分组呈现**（周中学校课合并「上学」取首节、培训班/一次性逐项，OpenClaw 算）。另确认服务端落地 = 部署包第一步 clone 官方小智服务端到 ECS。已回写 spec（S-F1.4/S-F1.1/§4.4/决策 20/26 修订 + 新增 27/28）与输入材料 §7。

## 2026-09-03 Portfolio 重切启动（spec v0.2 FROZEN V33 pin，主线程记）

- human 令「启动重切」。portfolio-planner 起草 `.planning/kidpal-mvp/PROPOSAL-v0.4-draft.md`：**15 包 = 13 live + S merged + 1 abandoned**——新建 **G `260903-gateway-directionB`**（方向 B 网关 bring-up：xiaozhi-esp32-server 部署 + provider 适配〔LLM type:openai 禁 max_tokens / edge-tts / 阿里云 ASR〕+ 强制鉴权 V11 + display 切分与机械校验 V27 + mcp 中转 + 网关兜底生成〔4s 安抚句/错误兜底/V32 截断〕+ 部署期首验 V21/V23 + 自签证书签发 V25）与 **WS `260903-device-ws-switch`**（设备切 WS：配网直写 NVS + 证书加载 pinning V10 + 方向 B 首链冒烟 + HTTP 台阶退役〔固件死代码消除 + shim 服务停 + 8080 安全组关闭 + Pod S/TP-A 冻结验收 supersede〕）；**包 1（shim-skeleton）作废**（能力承接映射：鉴权/loopback→G、四接口→协议剖面、alerts 出队→7）；包 2/7 重写、5/6/8/9a/9b 小调、0/3/4/U 不动。关键路径 **8 级**（G→WS→2→6→7→U→9b→9a，较 v0.3 +1 = tracer-bullet 拆对），WIP=3 不变。
- architect 单轮 dev-side review = **GREEN-with-fixes**（8 条建议级，无阻断/无回问；写域被 enforce-scope 拦、按既有约定主线程代落 review.md v0.4 段）：F1 计数 off-by-one 冻前钉死（14→15 包、落 15 pod、11 更新）；F2 网关兜底生成归 G；F3 决策 26 分组 producer 归包 6；F4 承接映射补 loopback；F5 行为碰撞型补 TP-A 冻结验收（与 Pod S 共享 shim.py，TP-B/TP-C 核实不撞）；F6 ASR 开通移出前置门簇；F7 复验分工钉死（G=机械校验、包 2=统计性遵循度复验，待确认点 1 放包 2 验收不 gate）；F8 留根补「G/WS merge 前确认 ESCALATE 批准路径就绪」（ARQ-260902-escalate-merge-no-human-path 未结）。8 条全部回灌。
- 新取舍点 4 项待 human 裁（建议即地板）：OQ-R1 G+WS 两包串行（建议）；OQ-R2 包 1 abandoned（建议）；OQ-R3 F4 归包 2（建议）；OQ-R4 shim 随 WS 同批退役（建议）。
- **交路留根（F-03 前置）**：原子落地制品（spec v0.2/契约/队列/证据）尚在工作区未 commit——时序 = human 冻结本件 → 授权 commit 原子落地制品入 main → 建 `planning/kidpal-mvp-base-v0.4` commit 重切产物 → 包 0 旧 worktree 自新 base 重建 → 交 launch。

### 重切冻结与机械落地（同日，主线程记）

- **PROPOSAL v0.4 human 盖冻结**（凭据 = 立项 ack「启动重切」+ architect GREEN-with-fixes + OQ-R1~R4 四题全按建议裁：G+WS 两包串行 / 包 1 abandoned / F4 归包 2 / shim 随 WS 同批退役；commit 授权同批给出）。frozen pin = `sha1:88a5bdb83abdd867`（raw-content sha1、LF 归一化），钉入 15 包 `origin.upstreamFrozen` + decision-coverage.json。
- **机械落地完成**：① PROPOSAL.md 正文替换为 v0.4（状态 FROZEN v0.4、OQ 裁定留痕）；② 15 包 pin 全量钉入 + mergeOrder 重排（G=3、WS=4、2=5、3=6、4=7、5=8、6=9、8=10、7=11、U=12、9b=13、9a=14）+ dependsOn 更新（2/6/8 改指 WS/2）；③ taskpackage：包 2/7 全文重写、G/WS 新建（260903- 前缀）、包 1 作废记录（承接映射四项）、6 包范围补注（3/5/6/8/9a/9b）；④ decision-coverage.json 升 v0.4（S-F4.1→包 2、§6.1 协议剖面+安全面→G+WS、兜底生成→G/消费→2、决策 26 分组 producer→包 6、contractBlocked→空、新增三条 crossPackageIntegration）；⑤ board 15 pod（S merged / 1 abandoned / 13 queued，WIP=3）。
- **交路执行（human 授权）**：原子落地制品（spec v0.2/契约重写/ARQ/CDQ/门证据）commit 入 main → 建 `planning/kidpal-mvp-base-v0.4` commit 重切产物 → base 并入 main（PROPOSAL v0.4 在 main 生效）。
- **launch 就绪**：G（260903-gateway-directionB）+ 0（host-test-infra，worktree 自新 base 重建）并行起跑；WS 等 G 绿。**G launch 前置门簇**：① 18789 锁 127.0.0.1（human/ECS 操作 + 部署期断言）② ARQ-260902-cert-lifecycle-deadlock 处置定夺（human）。包 4 闸 A（契约 §1.6 预审）与 U 的 V3 设计稿约束维持。
- **G 前置门簇②销账（2026-09-03，human 拍板）**：ARQ-260902-cert-lifecycle-deadlock = **accept-as-risk**（≥10 年有效期 V25 已把到期推出产品生命周期；轮换脚本 + 到期告警由 GWA-A9 承载实现）；ARQ 移已结索引（commit `89b648f`）。G 前置门簇余 ① 18789 锁（部署前执行 + GWA-E3 断言）。

## 2026-09-05 G 包（260903-gateway-directionB）dev-loop closeout（integrator 收口记）

- **收口**：方向 B 重切后首包、服务端共享底座（mergeOrder 3）dev loop 完成、verifier 终审 final round 4 九闸全绿（evidence `r4.json`，`verifier-evidence-check` factOk:true）。closeout 制品（docs-delta / review-packet）+ 4 件证据（v21 / v23 / deploy / cert-lifecycle-accept-risk）晋升 `changes/dev/260903-gateway-directionB/`；`server/deploy/README.md` 补 deployment-facing 指引（GATEWAY_DEPLOY_DIR / secrets 管理）。
- **治理追认**：ARQ-260902-cert-lifecycle-deadlock 迁 §1.3 watchlist（human 2026-09-04 accept-as-risk）；新增 ARQ-260904-gate-regex-uppercase-changeid（闸正则大写热修后审）与 ARQ-260904-v23-asr-swap-quota-recheck（token-plan 配额 2026-09-07 14:19 UTC 后补验 + 引擎 ASR provider 换型拍板）。
- **live 待办（human 外因，不阻塞结构面）**：公网 0.0.0.0:8000 绑定授权、阿里云 ASR 凭据/token-plan 配额、引擎 ASR provider 换型拍板。
- 下一步：WS（260903-device-ws-switch）等 G 绿后 launch（关键路径 G→WS）。
- **合并补记（planner，2026-09-05）**：G 包 supervisor 机械闸判 ESCALATE（high：跨系统 + 安全轴 auth.py），human 批准合并（pod-events `escalate-merge-approved`，事件 56750137），按 Pod S 先例一次性手工 `--no-ff` 合并入 main——ARQ-260902-escalate-merge-no-human-path 第二次实例，其治理修复 PROPOSAL（260904-gov-review-escalate-merge-approval）已在主 checkout 立项待审。

## 2026-09-02 Pod S（260829-smoke-e2e）收口补记（smoke-e2e worktree 未落盘遗留，2026-09-05 归位）

### 收口（同日）
- 特殊放行收口完成：豁免字段落盘（mutationSkipReason/docsNoChangeReason/securityEvidence）→ scoped r5 绿、final r6 绿 → integrator 收口（changes/dev 制品 + ADR + 3 条 ARQ，commit a53da2f）→ pod-retro → supervisor-verdict=ESCALATE（high：跨系统+安全轴，机械判据正确）→ **human 批准合并**（事件 86c73079）→ 本地 --no-ff 合并（e33278d，人批一次性操作：机械闸无 human-批准入口，已登记 ARQ-260902-escalate-merge-no-human-path 并跟进合并 53731c6）。
- 面板：260829-smoke-e2e = merged（mergeOrder 1 完成，下游 12 包底座就位）。
- ECS 暴露面已回收（0.0.0.0 → 127.0.0.1:8080）；**待 human：阿里云安全组关闭 8080 放行**。
- 遗留：真实声学往返（roundtrip.json）由下游 260829-voice-roundtrip 硬件到位后实证；届时先轮换设备 token（冒烟窗口值已部分留痕）。

## 2026-09-05 下一批 launch 面准备（主线程记，0/G merged 后）

- 调度判定：G（mergeOrder 3）+ 0（mergeOrder 2）已并入 main（047b89d / d99af50）→ 按 PROPOSAL §3 规则下一批 = **WS（260903-device-ws-switch，等 G✓）+ 包 3（260829-session-logic，等 0✓）**，WIP=3 内两 Pod 并行。
- launch 前置门：`portfolio-reconcile.mjs --check-launch --base-ref main` 两包全绿（taskpackage + PROPOSAL + decision-coverage 均在基准）。
- **环境等价化完成**：worktree `C:/AI/KidPal-pod-device-ws-switch`（branch `dev/260903-device-ws-switch`）+ `C:/AI/KidPal-pod-session-logic`（branch `dev/260829-session-logic`），base 均 = main 85cc0c6；firmware/sdkconfig + dependencies.lock 镜像、managed_components junction 指主 checkout（共享缓存，主 checkout 当前为空、首次 build 由 component manager 填充）、worktree-bootstrap.mjs 各跑过（.env 面源不存在、按设计跳过）。
- **专属图索引已建**：`C-AI-KidPal-pod-device-ws-switch` / `C-AI-KidPal-pod-session-logic`（moderate 模式，8983 节点 / 3.0 万边级，与主索引同口径排除）——纠正本轮中途口径：worktree 应建专属图索引（先例 = pod-gateway-directionB / pod-host-test-infra），非降级读文件。
- board `--set-pod` 预填完毕（queued + worktreePath）；launch prompt 文件两份落 `.git/devloop/launch/`（WS 含 intake 必办：Pod S + TP-A 冻结验收 supersede 由 acceptance-author 处置；包 3 纯 host 口径）。**待 human 自跑 launch 命令**（headless 形态沿用 260902 记录的交付方式）。
- **图索引漏建追因与热修（同日，human 指示「记录 ARQ、直接热修」）**：追因 = 主线程照抄 Pod 0 旧 launch prompt 的过期降级注记（建索引前旧文）而未验 `list_projects` 地面真值，叠加约定只存脚本注释（commit ed63d0c 纯文档层）+ rules 行「本仓尚未 commit 快照，索引用主索引」措辞误导。**热修（一次性跳过治理批、如实记录）**：`repo-structure.md` `.codebase-memory/` 行消歧——worktree 专属图索引定为 launch 面必办（同名建、不得降级读文件、引旧 prompt 先例前须验地面真值）。**ARQ 登记**：`ARQ-260905-worktree-graph-index-rules`（SHOULD，§1.2，追认手续 + 后审载体：是否加轻机械提醒，先例 ARQ-260904-gate-regex-uppercase-changeid「一次性热修后审」）。
- **快照 artifact 入仓（同日，human 指示「彻底解决」）**：根因坐实 = 本仓从未生成/commit `.codebase-memory/` 快照，每个 worktree 只能从头全量建图。`index_repository(persistence=true, moderate)` 生成快照（16168 节点 / 39534 边，压缩 6.2MB，钉在 main 85cc0c6）+ 工具自写 `.gitattributes`（`binary merge=ours` 防二进制冲突），commit **b7aa3df** 入 main。自此 worktree 准备 = bootstrap 后跑一次索引即从快照导入 + 增量补差（近零成本）；launch 面必办性质不变（仍须跑那一次）。rules 行同步更新（删「尚未 commit」过时句）。注：persistence 运行按路径派生刷新的是既有重名项目 `C-AI-KidPal`（在册早于本次运行，非新建），主索引 `KidPal` 项目名不变。

## 2026-09-05 包 3（260829-session-logic）dev-loop closeout（integrator 收口记）

- **收口**：会话状态机 + KEY/BOOT 全局按键手势解析（§4.0/§5.3 纯逻辑，mergeOrder 6，riskTier high）dev loop 完成：verifier r3 scoped 绿 + r4 final 九闸全绿（evidence `r4.json`，invocation `f991bafb-c80f-4536-a5f9-70ca00355ece`；`verifier-evidence-check` factOk:true EXIT=0；r1 acceptance-coverage 红 → r2 mutation-check 红 → r3/r4 绿收敛）。host runner 13 Tests 0 Failures EXIT=0（9 个 SES TEST_CASE + 包 0 既有 4 测试）；mutation M1~M4 四点破坏→变红→回滚全实证。closeout 制品（docs-delta「无需改」判定 + review-packet）晋升 `changes/dev/260829-session-logic/`。
- **留根**：SES-E2 跨页端到端整合归包 9b 兜底（PROPOSAL §7 计划内留根）；on-target 编译/真机验收同归 9b（本包闸 firmware-build degraded=host 单测包，非假绿）；espidf_wsl.sh CRLF 环境摩擦记 pod-events（NIT）。
- 下一步：supervisor 机械闸裁断 → planner 前置升级闸 → 本地合并；关键路径后续 WS→2→6→7→U→9b。

## 2026-09-06 WS 包（260903-device-ws-switch）dev-loop closeout（integrator 收口记）

- **收口**：方向 B 设备侧切换包（mergeOrder 4，dependsOn=G）dev loop 完成、verifier 终审 final round 4 九闸全绿（evidence `verifier-evidence/rounds/r4.json`，ok:true / observedGateExit 0）。设备从 Pod S 同步 HTTP shim 形态切到「配网直写 NVS（websocket 命名空间 url/token/version）+ 短路激活 + 自签 CA pinning（V10）+ WS 握手 Bearer + 原生 WS+Opus 流式」，并退役 HTTP 台阶（固件 shim_protocol/mp3_decoder/PlayMp3 死代码消除 + tenclass 激活门字符串归零、`server/shim.py` ARCHIVED 归档、ECS shim 停服 + 8080 安全组关闭、Pod S/TP-A 冻结验收标 superseded）。
- **验收实证**：host_app linux-target ELF（builder 接线 device_ws_switch.acceptance.c + ws_provision.test.cpp）**15 Tests 0 Failures / exit 0**（红基线演进 8T6F → 15T2F → 15/0，纪律留痕 red-baseline-summary.md）；live 方向 B 冒烟 2026-09-06 真机串通（设备⇄wss:8000 front⇄引擎⇄OpenClaw，ASR→LLM→流式 TTS + stt 文字上屏，多轮），证据本地-only 落 `server/acceptance/_artifacts/device-ws-switch/`（by design gitignore）。
- **human 授权验收修订**：WSA-A4 齿 3 证据形态改认（permessage-deflate 压缩致 wire 字面量 stt/tts/sentence_start 不在日志；改认本栈真实标记 识别文本/语音生成成功/SentenceType，session_id 字面保留，四协议事实仍全强制）——acceptance-author 执行、证据形态改革非降严格度。
- **live 偏差（留 retro，均必要）**：引擎 Intent:nointent（OpenClaw 拒 tools 数组）、ASR 用 qwen3_asr_flash + dashscope key（非 V23 终选 aliyun_stream，凭据未就位）、TTS type:edge、引擎 network_mode:host、front websockets v17 兼容 + 设备头透传。
- **closeout 制品**：docs-delta + review-packet 晋升 `changes/dev/260903-device-ws-switch/`；`server/deploy/README.md` docs-sync 同步 network_mode host 拓扑（本包 live 重切引入的漂移）。
- **了账**：Pod S 遗留「待 human：阿里云安全组关闭 8080 放行」由本包 WSA-A6 承接关闭（证据 ecs-teardown-evidence.md）。
- **待 human / 下游**：ASR 终选凭据/型号裁定（aliyun_stream vs qwen3_asr_flash）；OpenClaw tools 修复后回开 Intent/tool-calling；`feature-inventory.md` OC-7 行（已退役 shim）+ 设备侧能力清单更新（docs/notes 超出 integrator 写面，留主线程）。**下一步**：supervisor 机械闸 + 前置升级闸 → 人批后本地合并 main；包 2（语音回环 + display 消费，mergeOrder 5）依赖本包就绪（WSA-E3）。

## 2026-09-06 下一批 launch 面准备（主线程记，WS merged 后）

- 调度判定：WS（mergeOrder 4）今已并入 main → 按 PROPOSAL §3「0 绿且 WS 绿 → 2」下一批 = **包 2（260829-voice-roundtrip，mergeOrder 5）单包**——其余 queued 包（4/5/6/8=char-stroke/home-p1/schedule-p2/story-p3）dependsOn 全含 voice-roundtrip，依赖未就绪；WIP=3 无包可填（上一批 3 个 worktree 均已 merged，mergeToken.holder=none）。
- launch 前置门：`portfolio-reconcile.mjs --check-launch 260829-voice-roundtrip --base-ref main` 绿（taskpackage + PROPOSAL + decision-coverage 均在基准）。
- **环境等价化完成**：worktree `C:/AI/KidPal-pod-voice-roundtrip`（branch `dev/260829-voice-roundtrip`，base = main ad1611c）；firmware/sdkconfig + dependencies.lock 镜像、managed_components 软链指主 checkout（共享缓存）、worktree-bootstrap.mjs 跑过（.env 面源不存在、按设计跳过）。
- **专属图索引已建**：`C-AI-KidPal-pod-voice-roundtrip`（moderate + persistence，自 main 快照导入，9193 节点 / 3.1 万边）——launch 面必办性质不变（ARQ-260905 规则钉死）。
- board `--set-pod` 预填完毕（queued + worktreePath）；launch prompt 落 `.git/devloop/launch/prompt-260829-voice-roundtrip.txt`（含 WS 收口 live 偏差两条移入 intake 评估：Intent:nointent 待 OpenClaw tools 修复 / ASR 终选凭据未裁定——构成 blockingDecision 则 fail-closed，不静默推进）。**待 human 自跑 launch 命令**（headless 形态沿用 260905 记录的交付方式）。
- 本批准备过程同步沉淀为 ops skill（`.claude/skills/kidpal-pod-launch-prep/`，human 指示）——跳过治理批的一次性 authoring，如实记录于此。
- **ASR 终选销账（同日，human 裁定）**：WS 收口遗留「ASR 终选凭据/型号裁定」闭环——终选 = **aliyun_stream + ali SK**，凭据 human 已给、WS 包 live 实测可用。launch prompt 已同步修订（该条移出待裁定偏差、不再构成包 2 blockingDecision）；token-plan 配额 2026-09-07 14:19 UTC 后补验仍归 ARQ-260904-v23-asr-swap-quota-recheck 既有跟踪。余留 live 偏差仅 Intent:nointent 一条（OpenClaw tools 修复后回开）。
- **更正（同日，紧随上条）**：human 澄清——ASR 终选 = **Qwen3ASRFlash**（qwen3_asr_flash + dashscope SK，即 human 所给 ali SK，Qwen 属阿里系）；live 引擎配置本就是终选形态，无需切换。上一条所写的「aliyun_stream」系误记，以此条为准；launch prompt 已同步改正。V23 旧表述「aliyun_stream 终选」与本次拍板存在记录漂移，是否走治理批修订 PROPOSAL/spec 措辞归 human 裁。
- **launch 形态修订（同日，human 指示）**：包 2 launch 自 headless 无人值守改为 **human 在 VS Code 交互会话自跑**（prompt 全文作启动输入）——AskUserQuestion 可用，升级走正常 ruling（pod-events 留痕）替代非交互 fail-closed；skill `kidpal-pod-launch-prep` 模板同步改为「VS Code 交互默认、headless 备选」。
