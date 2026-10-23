# docs-delta · 261023-honcho-memory-wiring-restoration

> rigor=lite 收拢制品（integrator，2026-10-23）。本包 `docsSyncRequired=false`：**零 canonical spec/contracts 改动**（`docs/requirements.md`、`docs/architecture.md`、`docs/contracts/**` 本包零 diff、零写）。本文件承载两块：① 两处**残余偏差申报**（plan.md 冻结语义第 2 条，advisory 性质）；② acceptance 头注重写对账。

## 1. 残余偏差申报（甲案资格门挡不住的两条，如实申报、不静默）

甲案资格门口径：上游插件 `@honcho-ai/openclaw-honcho@1.7.0` 无 user-present-only 资格门（三家都不提供）；cron/心跳由 `captureSystemRuns=false`（默认）天然挡；front 一次性回合的 **user 消息**由 `config.noisePatterns=["^孩子(到目前为止)?讲的内容："]` 挡在库外——noisePatterns 只作用于 user 消息侧。由此产生两条残余偏差：

### R1. front 整理/引导回合：读侧画像注入 + assistant 回复单条入库

- **现象**：front 引导/整理两条一次性回合，其 user 消息被 noisePatterns 正则挡在库外（one_shot 判定口径 = DB 增量中 user 原文零出现），但 ① `before_prompt_build` 读侧画像注入照常发生（甲案无资格门可挡读侧）；② assistant 回复单条入库（挡板只对 user 消息）。
- **危害评估**：低害——引导句不含隐私；整理产物是孩子自己的故事（acceptance 头注 R1 申报原文）。
- **对 spec 的影响**：`docs/architecture.md` §6.7（记忆写入资格门 = user-present-only 白名单口径）与 §1.3（设备级 peer 用词）的现行文字与本实现行为存在偏差。
- **处置**：spec 口径修正**归面 C `/arch-change`**（PROPOSAL `delivery/review/261023-arch-review-memory-wiring-verified-behavior/` 在途，integrator 不代写 spec、零自改）；本包以 acceptance 头注 R1 申报 + 本节留痕，不静默。

### R2. QQ 回合落消息 + `honcho_search_messages` 可搜性待实测

- **现象**：QQ 回合以独立 senderId peer 落消息入库（owner 画像基本无污染），但消息本体入库且可能经 `honcho_search_messages` 被搜索到。
- **危害评估**：画像污染风险低（独立 peer）；可搜性是隐私暴露面，未实测。
- **处置**：**待部署轮 OQ-3 实测**（`honcho_*` 工具是否对 kidpal agent 暴露 / 可搜性），本包只申报不实现挡置（plan.md 冻结语义第 4 条）；spec 口径修正同 R1 归面 C。

### 共同注记

两条残余偏差的 spec（arch §6.7 / §1.3）口径修正统一归面 C `/arch-change`（PROPOSAL 261023-arch-review-memory-wiring-verified-behavior 在途）；integrator 对 spec 面零写权、零自改（ARQ-260929-docs-sync-spec-handoff）。peer→session 措辞（验证行为 = 设备级 session + 家庭单 owner 画像，session metadata `openclawSessionKey = agent:kidpal:openai-user:<device_id>`，非 peer 键）同样归面 C 修正。

## 2. acceptance 头注重写对账

`server/acceptance/test_261022_memory_isolation.acceptance.py` 头注由「carrier 时代（自打 marker / 可达性探针门控 / peer 键）」整体重写为「已验证接线（4 键 + noisePatterns）+ 残余偏差申报」形态：

| 头注节 | 换代内容 | 与本文件对账 |
|---|---|---|
| 文档头 + 上游面 | 261023 批 B 甲案；上游 = advisory §1/§6/§7/§8/§9 + plan.md 冻结语义六条 + human 三项裁决 | 一致（§1 已验证接线证据、§9 网关安全序逐条对应） |
| 已验证接线段 | `@honcho-ai/openclaw-honcho@1.7.0` + plugins.entries.openclaw-honcho 4 键 + 第 5 键 noisePatterns；设备级 session 口径（session key 家族，非 peer 键） | 对账 R1/R2 注记与声明 `_note` 一致 |
| 残余偏差段（R1/R2） | 头注逐条列出两条残余 + 「spec 口径修正归面 C」 | 即本文件 §1 的头注侧承载，两处内容一致 |
| FROZEN OFFLINE CONTRACT §B | 声明恰 5 键全路径 / 顶层 hooks.* 与 agents.entries.*.plugins 负锚 / apply 双形态安全化 / rollback 三级回滚序 / 运行时证据两字面（loaded 日志行 + Policy 行） | 与 A1/A2/A3 exitCriteria 逐条对应 |
| §C 降级护栏 | host 零假绿断言（仓内无插件 dist 源可源锚），降级实测归 live 齿 | 换代自 carrier 时代 catch/while(true) 文本锚 |
| §D live 证据三证 schema | marker 双证 → 三证（session metadata openclawSessionKey + DB 消息条数 + 网关日志行）；one_shot 判定口径写死 = 甲案 user 原文零出现 | 与 A4 exitCriteria「三证 schema」一致 |
| EVIDENCE_DIR | 改指 `delivery/taskpackages/261023-honcho-memory-wiring-restoration/memory-probes/` | 与 plan.md live 边界一致 |

锚换代本身是**显式治理动作**（advisory §10.4 human 已裁：作为批 B 锚换代承载，非静默改冻结件）。

## 3. 零写声明

- `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`：本包**零 diff、零写**（ integrator 对 spec 面无写权；契约面只读）。
- 本包对 spec 的唯一动作 = 残余偏差申报移交面 C（§1）。
- compose（`server/deploy/honcho/docker-compose.yml`）：本包零触碰（批 A 已先行回流 f4d5e647）。
