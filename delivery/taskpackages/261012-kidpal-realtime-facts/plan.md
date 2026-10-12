---
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect（/triage-change 快审，2026-10-12，同会话 return）
date: 2026-10-12
changeId: 261012-kidpal-realtime-facts
origin:
  type: regression
  upstreamType: regression
  upstreamRef: docs/crq/tech/TECH-261012-weather-fabrication.md
  upstreamFrozen: ""
  workstream: ""
---

# plan · 261012-kidpal-realtime-facts（kidpal 实时事实查询开放 + 通用反编造）

> reviewed plan（architect 2026-10-12 快审产出，主线程只搬运不改内容；触点① human 三答确认：②单包 Lite / 需求面档 1 先行捎带 / 延迟口径认可）。
> human UX 定调（原话）：「对于实时信息 应该查询，可以提示，我去查一下，然后开始查，这样体验也不会差」。

## 事实基线（architect 取证）

- 工具面在平台层已启用：ECS openclaw.json 顶层 `tools = {"profile":"coding","web":{"search":{"provider":"tavily","enabled":true}}}`；agents.entries.kidpal 无工具覆写、agentDir 仅会话库——禁令在 prompt（v9 :11「孩子对话回合」一回合作答 + 禁工具）不在工具配置。
- **E4 冻结件已退役**（2026-09-16 随 test_voice_roundtrip_b.acceptance.py 整体归档，archive/README.md 行 10）——不进合并门，无修订授权链。真在咬的是四个**版本无关** must-stay-green 守卫：m3k3 A1 三面、MCD 两面、M4K3-A8 三面+故事段锚、PN-A4 三面+课表行纪律+世系锚——「bump 不假红、漏翻必红」，四面同批翻即绿。
- prompt 版本演进纪律：OPENCLAW_SYSTEM_PROMPT_CARRIER（演进史唯一全量副本）+ additive v1..v9 链 + apply-xiaozhi-overlay.sh PROMPT_SRC 渲染 ECS data/ 与 .config.yaml `prompt:` 顶层键。
- 契约 §1.6（interface-contract.md :182-185）逐字引文钉 system prompt 核心约束——architect 建议 NO_CONTRACT_CHANGE（引文未枚举回合路由/工具政策，v7/v9 同位未触）；**判定归 architect-reviewer 四分类**（逃生门），CONTRACT_CHANGE_REQUIRED 则停 + 转契约门 + 重新分档。

## 范围清单（逐文件 changeManifest）

1. `server/deploy/openclaw-system-prompt.v10.md`（新建：v9 全量继承 + 实时事实回合新行 + 对话行通用反编造 + 底线节第三不变量；首行 `## 角色` 无前言）
2. `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`（现值 v9→v10 + 演进史 append，历史条目零删）
3. `server/deploy/provider.config.yaml`（system_prompt 三键 → v10）
4. `server/deploy/apply-xiaozhi-overlay.sh`（PROMPT_SRC/PROMPT_DST → v10 + 注记行）
5. `server/acceptance/test_prompt_v7_round_routing.acceptance.py`（additive RF-A1..A5 齿，tighten-only 窗口、既有齿零触碰）
6. `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` + 同目录 `README.md`（新建仓内镜像；live 位置 /root/.openclaw/workspace-kidpal/AGENTS.md + 同步纪律「先改仓内镜像进 review，再同步 live，顺序别反」）
7. `docs/architecture.md` §5.5.5 + §5.1（:196「版本化 v9」）指针刷 v10（docs-delta，v9 先例同款；ECS live ops 面归 integrator/主线程协同）
8. （核）`server/deploy/README.md` prompt 指针节

**ECS live ops**（部署纪律）：openclaw.json 复核留痕 →（条件分支）kidpal tools 覆写 → AGENTS.md 同步 → v10 + overlay 重跑（ecs_put.py exec_command+stdin + MSYS_NO_PATHCONV=1，上传前核远端==基线）；**overlay 重跑后必核 model_name=openclaw/kidpal 与决策 21 两键未被冲**（261001 冲刷陷阱 + D21 齿）。

## 关键设计要点

1. **回合路由 = 新增「实时事实回合」独立行**（不做对话行内白名单——对话行禁令是绝对句式，行内开白名单必然句内自相矛盾；v6 课表编造根因之一正是 prompt 自相矛盾，v7 整改即回合路由表消矛盾）。新行：识别特征 = 天气/新闻/今日热点/比分等当下实时信息；纪律 = 先播短提示（human 定调「我去查一下，然后开始查」）→ 联网查询 → **只依据查询结果作答**；查不到/超时/失败 → 诚实说没查到 + 引导（TECH 条目模型自纠后的正确形态固化为纪律）；严禁编造数字、严禁虚称查到了。日期/星期类不进本回合（RTC 可答）；兜底句保持「判不清按对话回合处理」→ 不查 + 诚实兜底，误判双向安全收敛。
2. **对话回合行同批补通用反编造条款**（v9 逐域枚举漏的通用实时事实域补死）；**底线节（v9 :18）扩第三不变量**「实时信息无查询支撑永不编造、永不虚称已查询」。提示语短、儿童化、不念系统词；display 规则零变。
3. **Tavily 验证步**（主环境先例 ≠ kidpal 会话必得）：① ECS 复核 openclaw.json tools 面 + kidpal 无覆写（留痕）；② 关键实证 = 经 loopback 18789 /v1/chat/completions（model openclaw/kidpal，VRT 探针同款）发实时事实问题，journalctl + agent 会话日志双侧对账真出 web search 调用；③ 分支：实证无工具 → agents.entries.kidpal 增 tools 覆写（ECS ops 引擎 config 类先例）后复测；平台级不可用（key/配额失效）→ 停、升级 human，可能转方向 B /requirement-change。④ prompt 防御性设计：实时回合写「工具不可用 → 诚实说不知道」降级口径，对工具缺席鲁棒。
4. **kidpal AGENTS.md 镜像内容边界**（次要承载，主阵地在注入 prompt v10）：约 15–25 行——角色一句、反编造底线一句（含实时事实指针）、数据只读边界、保留「冲突以注入 prompt 为准」自声明；不复述回合路由表全文（免疫两源漂移）。
5. **延迟预算**（human 已批）：提示语 ~4–5s 内作首个语音内容到达（front 4s 安抚定时器独立兜底不冲突）；完整真实答复目标 ≤15s、上限 20s——先作观测项记录实测、钉硬阈值留后续（诚实申报 live 非自动断言，PN-A5 同款）；真机轮加测一次降级路径（断 Tavily 或问查不到的事实 → 诚实+引导、零编造）。观测 = journalctl + docker logs 双侧对账 + human 目击；串口捕获器关闭会硬复位设备的坑注意归因。

## 验收齿草案（RF 批，additive tighten-only）

- RF-A1：v10 落盘非空 + 首行 `## 角色`
- RF-A2：回合路由面含实时事实回合行（内容驱动不钉措辞：新行 + 提示-查询-依据结果口径 + 降级诚实锚）+ 对话行通用反编造锚
- RF-A3：v9 sha256 零回改齿（PN-A3 形态）
- RF-A4：接线版承载实时回合锚 + 反编造锚（红→绿半同构：现接线 v9 无该行 = 设计态红）
- RF-A5：live 观测项（真机问天气全链路 + 降级一次，双侧对账 + human 目击，非自动断言）

## impactedTests 种子

test_prompt_v7_round_routing.acceptance.py（additive 宿主）· test_m3k3…::test_a1_prompt_wired_three_faces_version_agnostic · test_char_meaning_merge…::test_mcd_prompt_wired_two_faces_version_agnostic · test_m4k3_record_session_live（M4K3-A8）· test_261010_engine_decision21_overlay_persist（must-stay-green）· test_261001_openclaw_model_name_overlay_drift（must-stay-green）· test_perf_k1_roundtrip_latency（仅 contingency 触 openclaw.json 时核 drift_guard）· 机械闸全量不降。

## 影响面

固件零改动；docs/contracts/** 零触碰（以四分类判定为准）；docs/requirements.md 记账走需求面档 1 批（先行捎带，v1.62）。**已知地雷前置申报**：全文件内容锚 + live 观测项、无 DB/事件断言 → supervisor 机械闸可能判 all-mock 触 riskTier 重算 high → 需 human 放行（v9 同款先例申报）。

## 风险

① LLM 遵循度（行为等价假设，真机轮目击）；② Tavily 外部内容儿童化（新暴露面：prompt 纪律 + NFR-SAFE-1 ACCEPT-AS-RISK 形态覆盖 + 真机轮重点观测）；③ 新出网路径记账（NFR-SEC-2 第 ④ 条，human 意图原话即接受证据，记账文本须补）；④ openclaw.json contingency（小、留痕）；⑤ 回合误判双向安全收敛；⑥ Tavily 平台依赖（降级口径兜底 + 升级 human）。

## 非目标

引擎侧 strip 前导文本兜底（维持 v9 留账）；自研天气数据源/API 集成（方向 B，仅 Tavily 平台级不可用时重启评估走 /requirement-change）；对话回合一回合作答禁令本体（只开实时事实一个新行）。

## launchPreconditions

① architect-reviewer 四分类先行（预期 NO_CONTRACT_CHANGE；否则停+契约门+重新分档）；② 槽位核验 server/deploy/ 无并行 .v10 占用（ARQ-260908 槽位纪律）；③ Tavily kidpal 会话实证先于 ECS 部署 v10；④ 需求面档 1 批（NFR-SEC-2 ④ + NFR-SAFE-1 注记）先行捎带（human 已批）；⑤ intake spine = delivery/changes/intake/261012-kidpal-realtime-facts.md（已建档）；⑥ 真机轮观测安排（human 在场）。

## 下一步（facePlan 摘要，全文见 spine）

1. 契约面：architect-reviewer 四分类（轻，先行）
2. 需求面：requirement-change 档 1 先行捎带（NFR-SEC-2 ④ / NFR-SAFE-1 注记 → v1.62 落稿）
3. 实现面：planner `/build-feature 261012-kidpal-realtime-facts`（Lite 闭环 → 真机轮 human 在场）
