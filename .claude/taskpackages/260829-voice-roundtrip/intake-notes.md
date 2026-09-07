# Intake Notes · 260829-voice-roundtrip

> planner 记录 · 2026-09-07 · Pod 置 running（portfolio-reconcile 已落 15 pods、mergeToken.holder=none）

## Step 1 handoff 探测（完成）

- 权威输入 `taskpackage.json` 已读：story / VRT-A1~A7 / exitCriteria VRT-E1~E4 / 红基线命令 / testHost=hybrid、riskTier=high、mergeOrder 5、dependsOn WS+0 均已 merged。
- workstream 设计权威 `.planning/kidpal-mvp/PROPOSAL.md`（FROZEN v0.4）已读：§1 拆分依据、§2 包 2 行、§4「2 = 设备 display 路由共享底座（4/5/6/8 消费）」、§5 OQ-R3（F4 归本包）为本包关键口径。
- Pod 状态文件在主 checkout 共享区 `C:/AI/KidPal/.git/devloop/pods/260829-voice-roundtrip.json`（worktree 内 `devloop/` 不存在属正常）；intake 已 `portfolio-reconcile.mjs --set-pod --status running` 落位。

## Live 已知偏差 intake 裁定（launch 输入要求评估 blockingDecision）

**裁定：不构成 blockingDecision，按自主裁量推进；live 断言真被卡时再 fail-closed 升级。**

依据（代码 + 契约双源核实，非推断）：

1. **display 投递是 fence 驱动，不依赖 LLM tool-calling**。契约 §1.5-3/§四：OpenClaw 按 system prompt 在回复尾发 ` ```display ` fence → 网关正则切分 + 机械 schema 校验（display_schema.py）→ mcp `kidpal/display` tools/call 下发（mcp_relay.py）。网关自己的 LLM 请求 `openclaw_stream_request`（provider.py:97-108）只带 `model/messages/stream`，从不带 `tools`。
2. 「OpenClaw 拒 tools 数组 → Intent:nointent」退化的是**引擎意图分类路径**（xiaozhi 引擎 intent 检测用 function calling），影响 = 意图驱动的 control（语音触发 end_session、云端确认 stop_speaking、start_recording）与意图路由；nointent 回退 = 全部走 chat，多轮 chat 先例 2026-09-06 已串通。
3. 对验收的影响面：
   - VRT-A3（display mcp 消费）：设备侧消费 host 可测；live 端到端走 fence 路径，**不需要 tools 修复**。
   - VRT-A4（遵循度复验）：本质 = OpenClaw 对 system prompt 的格式遵循，fence 驱动，**不需要 tools**；遵循度缺陷本就「失败包内 resolve 不 gate」（契约待确认点 1 / PROPOSAL F7）。
   - 受影响而无法 live 验证的：意图驱动的 end_session（语音触发）。若 live 断言被此卡住，按 launch 纪律 fail-closed 升级 human，不降级为「记录即过」。
4. 反证留痕：launch 输入声称「A3/A4 依赖 OpenClaw 侧 tools 行为」；与契约机制 + provider.py 代码矛盾，本裁定以代码/契约为准并如实留痕。WS 包 verifier 证据中无 display live 流转记录，display live 链路正是本包要首次实证的面。

## 环境既成事实（launch 已核实，不重复排障）

- IDF 构建通路 `python c:\tmp\idf_run.py <idf.py 参数>`；firmware/sdkconfig、dependencies.lock 已镜像；managed_components 软链共享主 checkout 缓存。
- 本 worktree CodebaseMemory 项目名 `C-AI-KidPal-pod-voice-roundtrip`（本包新增文件）；跨包历史面用主索引 `KidPal`。
- ECS 访问 `c:\tmp\ssh_helper.py`；live 拓扑/配置陷阱见 ops skill `kidpal-ecs-gateway`；live 标 `@pytest.mark.live` 不 auto-run（ARQ-260901-g3-server-live-acceptance）。
- ASR 终选已裁定 Qwen3ASRFlash（human 2026-09-06），勿再报待切换项。
