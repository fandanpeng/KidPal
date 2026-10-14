---
changeId: 261001-openclaw-model-name-overlay-drift
rigor: lite
riskTier: low
sourceType: drift-report
reviewed-by: architect
date: 2026-10-01
---

# TaskPackage plan · 261001-openclaw-model-name-overlay-drift（② 单包 Lite）

> `/triage-change` 分流快审（architect，2026-10-01）+ human 触点①确认「另立独立小包 / ② 单包 Lite」后落盘。
> 与 `261001-m4-k3-record-session-live-repair`（Package A/B）边界干净：本包不含任何固件或 front 修复。

## 1. 问题与根因

2026-10-01 真机部署后 OpenClaw 返回 400：

> Multiple agents are configured, but this operation has no explicit owner. Select an agent explicitly; …

根因：OpenClaw 上跑 `main` + `kidpal` **双 agent**、`ownership: explicit`，而引擎活配置 `.config.yaml` 的 `model_name` 是 `openclaw/default`（无法唯一定位）→ OpenAI 兼容端点 400。该值在 2026-09-25 曾被手工修为 `openclaw/kidpal`，但只落在 ECS 活配置、未回流仓内；261001 的 ECS 重部署用仓内 overlay 重写活配置，把它冲回旧值。

- **活配置已热修**（261001，`/opt/kidpal/.../.config.yaml` → `openclaw/kidpal` + `docker restart`，探针 `model: openclaw/kidpal` 返回 200；备份 `.config.yaml.bak-260930-modelname-clobber`）。
- **本包修的是仓内**：模板与冻结验收仍钉旧值，下次 overlay 重部署会再次冲掉。

性质为 **drift**（外部演进 —— OpenClaw 由单 agent 变双 agent —— 造成的仓内配置漂移），非某次改动引入的回归。

## 2. changeManifest

**live 渲染链（核心修复点，必改）**
- `server/deploy/provider.config.yaml:62`（声明面 `model: "openclaw/default"`）
- `server/deploy/apply-xiaozhi-overlay.sh:72`（渲染面 `model_name: openclaw/default`）→ 引擎 `.config.yaml`

**一致性同步（非 live wire，仅改默认值）**
- `server/gateway/provider.py:32`（`OPENCLAW_MODEL` 默认值）—— **消费方口径钉死**：该模块的 LLM 面**无 live 调用方**（`front_app.py` 仅用 `gw_provider.corr_id()` 与 `FEATURE_STREAMING_FORBIDS_MAX_TOKENS`）；live model_name 由引擎读自己的 `.config.yaml` 发起。本处改动只保声明一致，**不动 `openclaw_stream_request` 结构**。
- `server/deploy/perf-k1/probe_batch.py:42`（`DEFAULT_MODEL`）

**冻结验收重钉 + 文案订正（归 acceptance-author）**
- `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py:916/:938` —— 期望值 `openclaw/default` → `openclaw/kidpal`
- 同文件 `:917/:939` —— **文案失准订正**：两处写「契约 §四钉 openclaw/default」，但 `docs/contracts/interface-contract.md` 对 `openclaw|model|model_name` **零命中**（契约从不钉 model_name）。订正为「model_name 基线 = openclaw/kidpal（双 agent 唯一定位；非契约面——契约 §四不钉 model_name）」。
- 同文件 `:929`（`test_e1_overlay_llm_segment_unchanged` docstring「E2 铁律：…model_name openclaw/default…」）—— **planner 起包期补入**（原列举漏）：重钉 `:938` 期望值后此 docstring 与断言自相矛盾，须一并对齐为 `openclaw/kidpal`。

**ADR（append-only）**
- `docs/ADR.md` 新增 **ADR-008**，注明 **amend ADR-006**（ADR.md:5 明令「不覆盖、不改写既有条目；推翻旧决策 = 新增一条并注明 supersedes/amend」）。**注**：plan/启动卡原文写「ADR-007」，但 ADR-007 已被 `260924-m4-k1-story-cloud-link` 占用（ADR.md「星星记录本数据面收口」）→ 起包 1f 核验改判 **ADR-008**（详见 §8）。

**重钉依据（供 acceptance-author 引用）**：PERF-E1/E2 齿原钉 `openclaw/default` 的成立前提 = 「OpenClaw 单 agent、`default` 可唯一定位」；261001 真机实测 OpenClaw 已为 main + kidpal 双 agent（`ownership: explicit`），该前提不成立，正确基线为 `openclaw/kidpal`。

## 3. ADR-008 草案要点（正文按 ADR.md 五节格式补齐）

- **决策**：引擎 `.config.yaml` LLM 段 `model_name` 由 `openclaw/default` 修为 `openclaw/kidpal`；同步改声明面 / 渲染面 / 探针默认值 / provider 默认值四处。
- **amend 关系**：修订 ADR-006 的支撑句「model_name `openclaw/default` 零改、模型选择对引擎与契约 §四完全不可见」——双 agent 下**对引擎可见**（必须点名 agent）；「对契约 §四不可见」仍成立。ADR-006 的**核心决策**（降 thinking 档、模型 primary `deepseek/deepseek-v4-flash` 不变、出网记账型号名零变动）不受影响。
- 理由 / 备选与放弃原因 / 关联：由包内 builder/integrator 按格式补齐。

## 4. 不在本包

- 引擎 overlay 的**活配置热修**（已执行，ECS 侧，仓内无此面）；
- `261001-m4-k3-record-session-live-repair` 的任何固件 / front 修复。

## 5. 验证方案

1. 静态核对：仓内 `openclaw/default` 仅应剩 carve-out 两类——**退役面**（`server/shim.py`〔ARCHIVED 头注〕、`server/kidpal-shim.service`、`server/acceptance/archive/*`）与**历史记录**（`server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md`、`docs/ADR.md` ADR-006 正文、`docs/crq/**`、`.claude/skills/kidpal-ecs-gateway/SKILL.md`〔已为 kidpal，`default` 仅历史解释身份〕）；模板/渲染/探针/provider/验收五处应为 `openclaw/kidpal`。
2. 冻结验收回归：PERF-K1 验收件重钉后全绿（含文案订正）。
3. 部署演练（可选，需 human 授权）：overlay 在 ECS 上重跑一次渲染，核验引擎 `.config.yaml` 落 `openclaw/kidpal` 且不覆盖其它段。

## 6. 风险与人门

- **改已 merged 包的冻结验收 oracle**：重钉 + 文案订正的人签字成本经 dev-loop 常规人门（supervisor 升级 + closeout）承载，不单设门。
- 未来新增 agent 时 `openclaw/kidpal` 仍唯一定位（点名 agent），不随 agent 数量变化。

## 7. 技术方案（planner+architect 预评）

> 本节由 planner 于 **2026-10-01** 补写，内容源 = **architect 前置技术预评档（planner.md Step 1g，只读 advisory）**。效力声明：architect 意见为 advisory 输入，不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据。本节是「planner 只读消费 plan.md」规则的唯一例外（见 task-intake-and-slicing.md plan.md 条例外条款）。

- **取向成立，live/声明面识别完整、无遗漏 live wire**。关键接线逐点复核：
  - `apply-xiaozhi-overlay.sh:72` = **唯一 live 渲染真源**（heredoc 直接写出引擎 `.config.yaml` LLM 段）→ 必改，改它才是「防下次 overlay 重部署冲掉」的实质修复。
  - `provider.config.yaml:62` = **声明面**，与渲染面成对（ADR-001「渲染面 == 声明面 == 被测面」不变式）→ 须同步，否则两面再漂移。
  - `provider.py:32` 的「无 live 消费方」判决**经 grep 实证**：live `front_app.py` 只 import `gw_provider.corr_id()`（:332/:797）与 `FEATURE_STREAMING_FORBIDS_MAX_TOKENS`（:306）；`OPENCLAW_MODEL` 仅被 `provider_config()`（:75）/`openclaw_stream_request()`（:105）引用，而这两函数**无任何 live 调用方** → 改 :32 = 纯声明一致、零 live 行为变化；:75/:105 引用同名常量自动跟随，plan「不动 `openclaw_stream_request` 结构」正确。
  - `probe_batch.py:42`（PERF-A1 探针默认值）= live 测试工具面，不改则探针继续发 `openclaw/default` → 双 agent 下同样 400，须改。
- **changeManifest 六件覆盖全部 live/声明面，成立**；`impactedTestsSeed` 单条（PERF-K1 验收件）正确（`archive/` 两件 `openclaw/default` 是 `norecursedirs` 排除的退役面，不进回归集）。
- **无阻塞性风险**；唯一需 builder 留意的前提 = 「`provider.py` 无 live 消费方」已由 architect grep 坐实，不需额外动作。

## 8. planner 起包修正留痕（2026-10-01，intake 1f 快照保鲜核验 + 1g architect 预评）

> plan.md 原为 `/triage-change` reviewed 稿；以下修正在起包期由 planner 依 1f/1g 结果就地订正并留痕（未改技术取向，只订正腐坏引用与补漏）。

1. **ADR 号 ADR-007 → ADR-008**：原稿写「新增 ADR-007」，但 ADR-007 已被 `260924-m4-k1-story-cloud-link` 占用 → 改判 ADR-008（amend ADR-006）。同步已改 taskpackage 的 story/exitCriteria/docsSyncReason。
2. **补 `:929` docstring**（§2 已补）：冻结验收件内原列举漏此一处。
3. **补 carve-out `server/kidpal-shim.service`**（§5 已补）：architect 1g 捞出——退役 shim 的 systemd 单（`OPENCLAW_MODEL=openclaw/default`），与 `shim.py` 同源退役面，正确在 manifest 之外、`零残留` 静态核对须显式排除。
4. **carve-out 措辞两轨**（§5 已改）：原「历史变更记录类文本」易与「退役面」混淆，改分「退役面」（shim.py / kidpal-shim.service / archive/*）与「历史记录」（PERF-K1-ACCEPTANCE-NOTES.md / ADR-006 正文 / crq / skill 注）两类，供 acceptance-author/verifier 判据清晰。
5. **`PERF-K1-ACCEPTANCE-NOTES.md` 处理**：自证为承载包 260924 的冻结设计/红基线/mutation 记录，**不应改写**（改写会伪造红基线与 mutation 证据）；其 :63-64 逐齿映射表陈旧为可接受的历史快照，closeout 时可 append 一行「260924 冻结快照，261001 已重钉」注记或于证据中显式记录「故意留作历史快照」（**可选、非阻塞**，由 integrator 定）。
