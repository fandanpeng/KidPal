# Intake Notes · 260829-char-stroke

> planner 记录 · 2026-09-07 · Pod 置 running（portfolio-reconcile 已落，mergeOrder 7，依赖 260829-host-test-infra / 260829-voice-roundtrip 均 merged）

## Step 1 handoff 探测（完成）

- 权威输入 `taskpackage.json` 已读：story / CHR-A1~A9 / exitCriteria CHR-E1~E3 / 红基线命令 / testHost=hybrid、riskTier=high、mergeOrder 7、dependsOn 0+2 均 merged。
- workstream 设计权威 `.planning/kidpal-mvp/PROPOSAL.md`（FROZEN v0.4）已读：§2 包 4 行、§4「2 = 设备 display 路由共享底座（4/5/6/8 消费）」+「0 = 固件测试基建」+ 闸 A 条款、§5 C1 行（human 2026-09-01 拍板）为本包关键口径。
- Pod 状态文件在主 checkout 共享区 `C:/AI/KidPal/.git/devloop/pods/260829-char-stroke.json`（worktree 内 `devloop/` 不存在属正常）；intake 已 `portfolio-reconcile.mjs --set-pod --status running` 落位。

## 实现面定向（planner 实读）

- **设备侧集成点**：`firmware/main/mcp_server.cc` kidpal/display 消费 switch 目前只处理 case 0（text）/1（home）/2（control）；char（`VRT_ROUTE_CHAR=3`，view_router.h 已留占位标「包 4」）落 default UNKNOWN 降级路径。本包设备侧 = 补 case 3 + P4 查字页（米字格 + 逐笔动画 + 右栏速查卡）。
- **host 测试基建（包 0）**：`firmware/main/test/host_app/` 独立 linux-target 工程，运行走 WSL（HOST_TEST_CONVENTIONS.md [C4.4]）；冻结验收文件挂 `HOST_ACCEPTANCE_SRCS`；weak-symbol sentinel 模式先例见 `voice_roundtrip_b.acceptance.c`（红基线 = 「frozen contract not implemented」缺行为红，非链接红）。渲染缝 `host_infra_render_frame` / `host_infra_read_pixel` 纯 LVGL 软渲染 headless 可跑。
- **网关校验现状**：`server/gateway/display_schema.py` 对 char 无 per-view data shape 校验（只 view 白名单 + data 必须是 object + 64KB 上限）——最小块当前会穿过网关。
- **spec 面**：§3.0.2 D3 已定稿（makemeahanzi，9574 字，单字约 2KB；板子端要点 = 1024×1024→400×300 含 Y 翻转偏移、M/Q/L/Z 二次贝塞尔离散、1bpp scanline 光栅）；§4.3 S-F3.1~F3.3（笔顺三态、950ms 步进、歇 2s 重来、右栏速查卡、KEY 短按重播）；§5.6 教学字体红线（米字格示范字标准规范字形、伴随文字可用可爱体）。
- **部署面**：`server/deploy/` = ECS overlay（apply-xiaozhi-overlay.sh / provider.config.yaml / engine-patch/ / openclaw-system-prompt.v1.md 版本化落盘）；C1 静态字库 + 拼装机制的部署载体按此 overlay 形态落。

## 闸 A 契约预审（完成，2026-09-07）

**四分类裁定：④ 无需改契约（NO_CONTRACT_CHANGE）——契约门不出场、无 blockingDecision，闸 A 就此销账。**

architect-reviewer 只读预审结论文本已存 `.claude/taskpackages/260829-char-stroke/gate-a-prereview.md`。要点：

1. C1 不改任何跨边界 wire format：拼装后到达设备的仍是 §1.3① 全量 D3 JSON；§1.6 约束的是回复两部分格式 + view 白名单，从未约束「display 块内数据由 LLM 自己生成」。
2. **「最小块」（LLM 意图＋字）不进契约、且绝不以 wire 形态越过 OpenClaw→网关边界**——它只是 LLM 与 OpenClaw 侧查字工具间的内部约定。最小块终止于 OpenClaw 回合内。
3. D3 格式本身零改动；spec §3.0.2 / §4.3 与 C1 口径一致无漂移。
4. 预审边界提示：若实测发现 OpenClaw 机制无法在回复正文内注入全量块、拼装点被迫下沉网关，才构成契约门事项（§1.5 薄桥红线被破），届时重新预审。

### 闸 A 衍生的包内待办（非契约事项，进 TaskPackage 执行记录）

1. system prompt v2 落盘（版本化 `.v2`，`server/deploy/`）：预审已给措辞建议（LLM 只判意图＋提取目标字、禁自生成/复述 strokes/medians、data 原样用查字工具完整结果），机制定稿后与所选机制对齐；保持「验收所测 = 生产所用」纪律。**同时一并收紧三句纪律**（包 2 收口留根：A4 sample 1 口语 4 句超三句纪律，非 gate，V32 机械兜底已 landed）——本包动 system prompt 面，顺带收紧，不单开变更。
2. **网关 char 必需字段校验二选一**（包内裁定）：加则触 `display_schema.py` 的 GWA-A5 冻结验收、走 acceptance-author supersede 处置；不加则板子侧对缺字段 char 块做防御性降级。预审倾向 MVP 先不动网关（板子侧防御降级），实现期定。
3. CHR-A8「逐字对照数据源正确」断言打在**拼装产物**上（工具输出 = makemeahanzi 原文），非 LLM 输出。

## Live 已知偏差 intake 裁定（Intent:nointent / ARQ-260907-openclaw-tools-rejection-rootcause）

**裁定：不构成 blockingDecision，不依赖被阻的 tools/intent 面，按冻结 C1 口径推进；机制 = OpenClaw 回复出口补丁（下述 B）。**

依据（openclaw-main 克隆 + 上游源码调查，2026-09-07，报告存 `openclaw-extension-survey.md`）：

1. **C1 的「LLM 只出意图＋字」不依赖 LLM tool calling**：意图＋字以文本形态（最小 display 块）出现在回复正文，网关请求本就不带 tools 数组（包 2 已核实 provider.py:97-108 只带 model/messages/stream）。被拒收的 tools 面与此无关。
2. **OpenClaw 侧非 LLM 代码拼装的机制排查结论**（证据边界：本地快照 2026.7.2 缺 `src/gateway/`，以 GitHub 上游 `src/gateway/openai-http.ts` 补证）：
   - skill = 纯 prompt 注入（`docs/tools/skills.md`），无直通输出路径——LLM 复述坐标 JSON 的 109s 问题无解，排除；
   - `/v1/chat/completions` HTTP 路径**无任何可改写回复文本的 hook**：channel 投递类（`message_sending` 等）在该路径不触发；`before_agent_finalize` 只能 `{action:"revise"}` 再跑一轮模型（更慢）；`before_agent_reply` 是整轮替代 LLM；`tool_result_persist`/`before_message_write` 只作用于 session 持久化非返回 payload——纯插件方案不存在；
   - **机制 B（选定）**：在 OpenClaw `src/gateway/openai-http.ts` 回复文本出口（非流式 :1091 / 流式 :1353 `resolveAssistantResultText`）打小补丁：正则提取回复中 `view:"char"` 最小块 → 查 ECS OpenClaw 侧 makemeahanzi 静态字库（+目标字表预热 + LRU）→ 程序化拼全量 D3 JSON 替换。补丁 + 字库挂 OpenClaw 侧（ECS），**不入网关**，守 §1.5 薄桥红线与冻结 C1 位置裁定（human 2026-09-01）。
   - 机制 A（升级 OpenClaw ≥2026.5.12 后走 client tools、网关侧拼装）**否决**：字库/拼装入 gateway 违反冻结位置裁定；机制 C（纯插件 hook）**否决**：该路径无对应 hook。
3. **机制 B 是冻结裁定的机制化落实、非新决策**：PROPOSAL §1.6「OpenClaw 侧查表拼装、守薄桥红线」已冻结拼装位置；调查证明唯一能同时满足「LLM 不复述坐标」+「拼装挂 OpenClaw 侧」的机制就是回复出口补丁（engine-patch 同款 overlay 形态先例）。若 human 对「给部署态 OpenClaw 打补丁」这一运维面有异议，随时叫停（interactive 会话）。
4. **实现注意（进 TaskPackage 计划）**：
   - 本地 openclaw-main 快照不完整（缺 `src/gateway/`），补丁开发以 ECS 部署源为准（ssh_helper 读取；先核对线上 `openclaw --version`）；
   - **流式路径必须保证网关永不见最小块成品**：补丁在流式写出口对最小块做 hold-and-replace（块起→缓存→拼装完成替换写全量块），配合板子侧对缺字段 char 块防御性降级（闸 A 待办 2 的裁定：MVP 不动网关 display_schema、不触 GWA-A5 冻结验收）；
   - chr-A8 断言打在拼装产物（工具输出 = makemeahanzi 原文）。

## 环境既成事实（launch 已核实，不重复排障）

- IDF 构建通路 `python c:\tmp\idf_run.py <idf.py 参数>`；firmware/sdkconfig、dependencies.lock 已镜像；managed_components 软链共享主 checkout 缓存（勿删勿重建）。
- 本 worktree CodebaseMemory 项目名 `C-AI-KidPal-pod-char-stroke`（本包新增文件）；跨包历史面用主索引 `KidPal`。
- ECS 访问 `c:\tmp\ssh_helper.py`；live 拓扑/配置陷阱见 ops skill `kidpal-ecs-gateway`；live 标 `@pytest.mark.live` 不 auto-run（ARQ-260901-g3-server-live-acceptance）。
- ASR 终选已裁定 Qwen3ASRFlash（human 2026-09-06），勿再报待切换项。
- 三 Pod 并行（char-stroke 7 / home-p1 8 / schedule-p2 9，WIP=3）；共享面改动（display 路由、gateway 代码）保持 additive，勿重排他人文件结构。
