# docs-delta — 260829-voice-roundtrip（方向 B 语音回环 + display 消费链路）

> integrator 收口 · 2026-09-07。subsystem=firmware+server，riskTier=high，mergeOrder 5，
> dependsOn=260903-device-ws-switch / 260829-host-test-infra（均已 merged）。本包把设备侧
> display mcp 消费端（view 路由共享底座）+ OpenClaw 基础 system prompt（版本化落盘与部署
> 接线）+ 语音回环/abort/兜底消费验收做到绿。本文件记录 docs-sync 的 **dist（哪些已改）**
> 与 **判定（哪些无需改、为什么）**。

## A. 已改（本包落地）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| display view 路由共享底座（新增） | `firmware/main/display/view_router.{c,h}` | 三个 FROZEN OFFLINE CONTRACT 纯逻辑函数强定义（`kidpal_display_route` / `kidpal_display_control_action` / `kidpal_display_text_page_count`），7 view 白名单 + UNKNOWN 显式降级（V27）；C linkage、无芯片依赖，设备树与 host_app 同源 | VRT-A1/A3、VRT-E3（包 4/5/6/8 additive 扩展面） |
| mcp kidpal/display 消费端（新增） | `firmware/main/mcp_server.cc` | `AddTool("kidpal/display", …)` 注册（view+data string 参数）+ string 型属性收对象/数组时序列化分支（E1 嵌套 data 载荷）+ 送达确认 cJSON 回执；control 白名单动作解析在机械层（view_router） | VRT-A3、契约 §一 E1 / §四 |
| host 测试接线 | `firmware/main/CMakeLists.txt` + `firmware/main/test/host_app/main/CMakeLists.txt` | `view_router.c` 编入设备树与 `HOST_ACCEPTANCE_SRCS`（强符号覆盖 weak 哨兵）+ `voice_roundtrip_b.acceptance.c` 编入 host runner | VRT-E2（host 单测闸） |
| 冻结验收 C 侧（新增） | `firmware/main/test/voice_roundtrip_b.acceptance.c` | VRT-A1~A7 + boundary/security 齿（weak-symbol frozen contract 设计，红基线红在缺失行为） | VRT-A1~A7 |
| 冻结验收 python 侧（新增） | `server/acceptance/test_voice_roundtrip_b.acceptance.py` | a2/a3/a5 守卫 + a4/a6/a7 证据齿 + e4 两齿 + live 三齿（@live 不 auto-run） | VRT-A4/A5/A6/A7、VRT-E4 |
| live 机械 skip 闸（新增） | `server/acceptance/conftest.py` | `@pytest.mark.live` 默认收集即 skip，`RUN_LIVE=1`/`-m live` 显式放行——ARQ-260901-g3 纪律从运行命令自觉变机械闸 | ARQ-260901-g3 |
| OpenClaw system prompt 版本化落盘（新增） | `server/deploy/openclaw-system-prompt.v1.md` | E1 两部分结构 + F4 三句纪律 + 儿童化 + 7 view 白名单 + 增量扩展区（包 4 char / 包 7 提醒备注） | VRT-E4 |
| prompt 接线声明件（新增） | `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | deploy overlay 对版本化 artifact 的显式接线声明（落盘 ≠ 孤儿文件的机械断言载体，e4 第二齿 corpus 只读无扩展名文件） | VRT-E4 |
| prompt 声明块 | `server/deploy/provider.config.yaml` | `system_prompt:` 声明节（artifact/source/deploy_target/结构注释） | VRT-E4 |
| 真 mount 路径修复 + prompt 渲染 | `server/deploy/apply-xiaozhi-overlay.sh` | DATA_DIR 从陈旧诱饵路径 `main/xiaozhi-server/data/` 修正为真 bind-mount 路径 `deploy/xiaozhi-docker-data/`；prompt 拷贝到 ECS data/ + 顶层 `prompt: \|` literal block 注入 `.config.yaml`（幂等：已有 prompt 键跳过） | live「所测 = 生产所用」成立（fixer followup 的 repo 侧对齐，见 roundtrip-live-evidence.md） |
| 红基线判定摘要归位件 | `.claude/taskpackages/260829-voice-roundtrip/red-baseline-summary.md` + `firmware/main/test/acceptance/red-baseline/red-baseline-summary.md` | acceptance-author 红基线实测归位（两处同文，原位留档 = WS 包先例的归位注记形态） | 红基线纪律留痕 |

## B. docs-sync 判定

- `docs/contracts/interface-contract.md` 本包**只读消费**（`contractRefs`），contract-lint PASS（零触碰）。display 统一响应块 E1 / §四 mcp 工具调用 / §1.3 view 白名单 / §1.4 control 动作均为既有冻结定义，本包是设备侧消费实现，不改任何契约定义。
- `docs/spec.md#§4.0 / §4.4 / §6.1` 只读消费：错误兜底行（消费侧归本包、生成侧归 G）、S-F4.1 通用问答（三句纪律、全文上屏否摘要）、F4 承载均已被冻结 spec 覆盖，本包无 spec 漂移、不触发 spec 改动。
- `.planning/kidpal-mvp/PROPOSAL.md` 受 `upstreamFrozen sha1:88a5bdb83abdd867` pin，禁改防 UPSTREAM_DRIFT。决策映射由 `decisionCoverage.landsIn` 承载（五条全落 `acceptance:VRT-*`，无 dropped）。
- **Progress 补记**：按 workstream 惯例在 `.planning/kidpal-mvp/progress.md` 追加本包 integrator 收口记一段（append 不覆盖，见本文件 C 段）。

## C. `.planning/kidpal-mvp/progress.md`（已 append）

- 新增「2026-09-07 包 2（260829-voice-roundtrip）dev-loop closeout（integrator 收口记）」一节，内容与 review-packet 同口径：final r1 九闸全绿、验收实证、诚实边界、留根与下游。历史段落零改动。

## D. ADR 落点说明

- TaskPackage **无 `adrRequired`**；本包架构裁决全部已在上游冻结：PROPOSAL v0.4 §2/§4/§5（包 2 范围 + display 路由共享底座 + F4 归属）、契约 §一/§四（E1 mcp 承载 + view 白名单/control 动作）、spec §4.0/§4.4（兜底消费归属 + S-F4.1）、intake 裁定（fence 驱动不依赖 tools）。`decisionCoverage` 五条全落 `acceptance:VRT-*`，本包无新增架构裁权。
- 故**不新增 ADR**。唯一的部署面裁决（prompt 注入为 `.config.yaml` 顶层 `prompt:` 键、版本化 `.v1/.v2` 切换）已在 `OPENCLAW_SYSTEM_PROMPT_CARRIER` 与 `openclaw-system-prompt.v1.md` 头注就地记录 why——属实现注记而非跨系统架构裁决。

## E. 里程碑 / delivery 面

- `deliveryImpact=milestone-affecting`，但**无里程碑阶段跳变**（M2 全链路仍在 delivery 推进中，无 accepted 转移）→ 不改 `milestones.jsonl`。本包 live 三轮实证（流式回环多轮 + abort 云端取消 + display round）是 M2「语音→ASR→OpenClaw→回复→TTS→设备播放」的实质推进，已记入 progress.md 收口记。
- `delivery-status.mjs` 试跑仅重生成 `.delivery/STATUS.md`（内容无差异，已还原），未留脏。

## F. 无漂移确认

- 契约面零触碰、无影子目录（根级 `contracts/**` 零引用）；无新增 `docs/` 根文件。
- 证据与报告落 `changes/dev/260829-voice-roundtrip/` 与 `.claude/taskpackages/260829-voice-roundtrip/`，不散写。
- `server/acceptance/_artifacts/**`（WS live 证据工件复制件）与 `firmware/sdkconfig`（机器本地板型文件，已从 WS worktree 修正）均 gitignore，by design 不入库，未 force-add。
- G 包已 landed 面（`server/gateway/fallback.py` / `display_schema.py` / `mcp_relay.py`）零改动；G 包 pytest 基线 30 collected 已验零破坏（本包 conftest 的 live skip 闸与 G 包 live 验收 docstring「NOT auto-run by any merge gate」一致，断言零变更）。
