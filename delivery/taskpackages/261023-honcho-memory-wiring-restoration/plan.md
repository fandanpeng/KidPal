---
changeId: 261023-honcho-memory-wiring-restoration
title: 批 B 记忆接线 bring-up（甲案）——声明工件换代为探针验证 4 键形状 + noisePatterns、apply 安全化（patch 生成器 + OpenClaw 写入口编排、零脚本直写）、自研 carrier 弃用删除、冻结锚换代（peer→session 口径 + live 证据三证 schema）
rigor: lite
riskTier: medium
sourceType: arch-review
reviewed-by: architect（memory-wiring-advisory-20261023.md §6甲/§7/§8/§9；human 三项裁决已落）
date: 2026-10-23
gateProfile: ["test-quality", "evidence-integrity", "security"]
dependsOn: "批 A（compose 拓扑回流）已先行回流 commit f4d5e647——本包不改 compose"
---

# 背景（权威输入）

- **设计权威** = `delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md`（architect advisory，只读检视 ECS + 仓内，2026-10-23）。K3 生产崩循环根因三层失效：① 声明路径错（顶层 `hooks.allowConversationAccess` + `agents.entries.kidpal.plugins.honcho` 均被 2026.8.1 zod `strictObject` 拒收 → 网关 exit 78 崩循环）；② `honcho-plugin.js` 不是插件（无 definePluginEntry/openclaw.plugin.json）；③ 它调用的 `/v1/openclaw/*` 端点在 Honcho 源码不存在（路由前缀 `/v3`，grep 零命中）。
- **「验证过的接线」**（advisory §1 逐条引证据）= 上游 npm 插件 `@honcho-ai/openclaw-honcho@1.7.0`（`openclaw plugins install` 安装）+ `plugins.entries.openclaw-honcho` 4 个配置键（enabled=true / hooks.allowConversationAccess=true（插件条目内）/ config.baseUrl=http://127.0.0.1:18100 / config.workspaceId）。探针与生产同版本 2026.8.1，离线复现叠加探针形状 = `Config valid`。
- **human 三项裁决（2026-10-23，最高约束）**：① **方案甲** = 原样复用官方插件零新代码；② 生产 `workspaceId=kidpal`，新库、探针数据（kidpal-probe）不迁；③ 与批 A 解耦——compose 已先行回流 commit f4d5e647，**本包不改 compose**。
- **human 原则（advisory 第一判据）**：「我们之前测试过、已经验证过能用的功能，就直接用起来，不要再去发明什么新的东西了。」凡冻结物与本原则冲突，以本原则为准（carrier 弃用、不重写）。

# 冻结语义（关键裁决，builder/acceptance-author 逐条消费）

1. **资格门口径 = 甲案**：上游插件无资格门（user-present-only 三家都不提供）。cron/心跳由 `captureSystemRuns=false`（默认）天然挡；front 一次性回合的 **user 消息**由 `config.noisePatterns`（上游已文档化、在 configSchema 内、`additionalProperties: false` 白名单内）挡——锚定正则 `^孩子(到目前为止)?讲的内容：`（恰配 front 引导/整理两条一次性 prompt 的 user 消息开头）。
2. **两处残余偏差如实申报**（acceptance 头注 B.4/C 节重写 + integrator docs-delta 逐条），**不静默**：① front 整理/引导回合的**读侧画像注入**与 assistant 回复单条入库（甲案挡不住）；② QQ 回合落消息（senderId 独立 peer、画像基本无污染，但消息入库且可能经 `honcho_search_messages` 可搜）。**spec（arch §6.7/§1.3）口径修正归面 C（`/arch-change`），不归本包**——本包零 canonical spec/contracts 改动。
3. **peer→session 措辞**：验证行为是**设备级 session**（Honcho session metadata `openclawSessionKey=agent:kidpal:openai-user:<device_id>`，DB peers 只有 `owner`/`agent-kidpal` 两个、无设备级 peer）+ 家庭单 owner 画像。本包验收一律按 **session key 家族**表述；"peer 键"用词只在引用 P2 常量语境保留，spec 用词修正归面 C。
4. **`honcho_*` 工具是否对 kidpal agent 挡**（疑似 `tools.allow` 管不住插件工具，`pluginToolAllowlist` 独立通道）→ **部署轮实测后定（OQ-3），本包只申报不实现**。
5. **插件版本 pin = 1.7.0**（`--pin` 安装记录），声明与验收锚均记 1.7.0。
6. **网关安全铁律（advisory §9，冻结进 apply 语义）**：`openclaw.json` 只经 OpenClaw 自己的写入口写（`config patch`），写前必 `--dry-run` schema 校验；**禁止脚本直写 openclaw.json**（K3 崩因）；不写顶层 `hooks.*`；正式 patch 一次只动本声明相关键。advisory §9 应用序（备份→dry-run→install→inspect/doctor→正式 patch→validate→restart→health）为编排权威序。

# 技术方案（planner+architect 预评）

> 留痕：本节内容由 planner 依 architect advisory `delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md`（2026-10-23，§6 方案甲 + §7 锚换代清单 + §8 shaping + §9 安全序）合成；advisory 本身即 architect 前置技术评估产物（1g 档 Lite 简版形态：同源劳动不重复派发），效力声明 = advisory 性质声明原文（advisory 不豁免任何门、不构成通过判据）。

1. **声明工件换代**（`honcho-plugin-declaration.json`，仍 stdlib JSON）：`changes` 恰 5 条点分键——`plugins.entries.openclaw-honcho.enabled=true`、`plugins.entries.openclaw-honcho.hooks.allowConversationAccess=true`、`plugins.entries.openclaw-honcho.config.baseUrl="http://127.0.0.1:18100"`、`plugins.entries.openclaw-honcho.config.workspaceId="kidpal"`、`plugins.entries.openclaw-honcho.config.noisePatterns=["^孩子(到目前为止)?讲的内容："]`。顶层 `hooks.*` 与 `agents.entries.*.plugins` **零出现**（schema 拒收键，负锚）。`drift_guard` P1 现状锚（`agents.entries.kidpal.tools.allow` 四项）原样保留；backup/rollback 形制不变。声明零 secret。
2. **apply 安全化**（`apply_honcho_plugin.py`）：撤销「点分写入器直写 openclaw.json」路径（K3 崩因，§9 铁律禁止），改为双形态——
   - **patch 生成器**：从 declaration 生成 `openclaw config patch` 消费的 JSON5 patch 文件（离线可跑、可测，host 面行为断言承载）；
   - **安全应用编排**：按 advisory §9 序（时间戳备份 cp → `config patch --file --dry-run --json` schema:true → `plugins install @honcho-ai/openclaw-honcho --pin` → `plugins inspect --runtime` + `doctor` → 正式 `config patch --file` → `config validate` → `systemctl restart openclaw-gateway` + health/NRestarts 核对），任一步失败即停；openclaw 二进制经注入（CLI 参数/环境变量）可离线以桩真跑**调用序断言**（dry-run 先于任何写、失败序不写）。
   - 幂等预检（现状已达标 → SKIP exit 0）与 drift-guard（P1 锚不中 → STOP exit 3）语义保留。真实安装/patch/restart 执行面 = **human 部署轮**（或主线程经 human 授权），本包只交付工具与序。
3. **rollback**（`rollback_honcho_plugin.py`）：对应改——保留备份还原一步回滚（advisory §8 回滚方式 2 认可），补注方式 1（`plugins disable` + restart）为首选低危回滚；脚本头注申报三级回滚序。
4. **carrier 撤净**：`honcho-plugin.js` 删除；`manifest.yaml` 插件工作台条目从 4 降为 3（decl + apply + rollback），carrier 条目删除；全工作台零 "kidpal honcho memory patch active" marker、零 `/v1/openclaw` 端点、零自研 carrier 残留（负锚）。
5. **验收锚换代**（`test_261022_memory_isolation.acceptance.py` 原件原地换代，advisory §7 清单逐条）：存活 4 组（front payload 零 user / e6 结构部分 / apply-rollback 行为组 / no-openclaw-json 组）+ 换代 9 处（marker→`Honcho memory plugin loaded` 日志行 + `Policy: allowConversationAccess: true`；peer 前缀→session key 家族；hooks 后缀→全路径；探针锚删；gate_carrier_anchors 重写为 4 键逐字+pin 1.7.0+零 carrier 残留；degradation guards 改上游插件源锚或降 live；manifest 条目数 3；live 证据 schema marker→三证；头注 B.4/C 重写）。EVIDENCE_DIR 改指本包 `delivery/taskpackages/261023-honcho-memory-wiring-restoration/memory-probes/`。**锚换代是显式治理动作（advisory §10.4 human 已裁：作为批 B 锚换代承载，非静默改冻结件）**。
6. **live 证据新 schema**（冻结进 acceptance 头注）：marker 字段 → 三证 = ① session metadata `openclawSessionKey`（前缀 `agent:kidpal:openai-user:`）+ ② 消息条数（DB 增量）+ ③ 网关日志行；one_shot 判定口径写死 = **甲案：front 引导/整理回合后 DB 增量中 user 原文零出现**（assistant 回复入库 = 申报残余，不判红）；seeded 负向探针集照旧（S-F5.1-AC3/NFR-PRIV-1 量化口径不变，机理层改动不改负向探针判据）。

# 冻结锚保护清单（must-stay-green，builder 红线）

- `test_k3_e2_front_oneshot_payload_no_user` 语义保（docstring 需改述）；K1/K2/261017 冻结件零触碰。
- 姊妹件 `test_261022_honcho_memory.acceptance.py`（K3-E1 栈面 + K3_P1_AGENTS_ANCHORS）零交叉触碰——本包不改 compose、不改 AGENTS.md。
- `test_261023_agents_md_static_semantics_alignment.acceptance.py` 零触碰。
- drift_guard P1 现状锚逐字保；openclaw.json 零入仓 + 零 secret 负锚保。
- 全量 server acceptance 基线 = **316 passed / 6 deselected / 33 subtests**（批 A+B 后本树基线，2026-10-23）——唯一允许的新红 = 本件换代断言自身（设计态红→绿）；既有其它件零新增红。

# 红基线与判法预言

- 红基线命令：`cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance`。
- 预言：换代后新红集中于 `test_261022_memory_isolation.acceptance.py` 内被换代断言（声明 4 键/负锚/manifest 数/pin/残留），数条设计态红；其余 315+ 条保持绿。acceptance-author 冻结相位实跑回填精确计数。

# changeManifest

```
server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json   # 5 键换代
server/deploy/261022-openclaw-honcho-plugin/apply_honcho_plugin.py           # 安全化重做
server/deploy/261022-openclaw-honcho-plugin/rollback_honcho_plugin.py        # 对应改
server/deploy/261022-openclaw-honcho-plugin/honcho-plugin.js                 # 删除
server/deploy/manifest.yaml                                                  # 工作台条目 4→3
server/acceptance/test_261022_memory_isolation.acceptance.py                 # 锚换代（§7 清单）
```

# live/部署轮边界（Pod 停等位）

- host 面转绿后 status `awaiting-human:live-deploy`。live 面 = 插件 install（--pin 1.7.0）+ 4 键 apply（按 §9 序）+ 重启 + live 证据采集（三证 schema / 甲案 one_shot 口径 / 降级实测 / OQ-3 honcho_* 实测），由 human 部署轮执行或主线程经 human 授权执行；Pod 不自行上 ECS。
- live 证据落 `delivery/taskpackages/261023-honcho-memory-wiring-restoration/memory-probes/*.json`，由换代后 live 齿判定。

# 风险与观察项（部署轮申报面）

- 整理回合画像注入是否影响「忠实」（真机抽检 3-5 例）；QQ 回合落库与 `honcho_search_messages` 可搜性（OQ-3）；ECS 2C/4G 内存复测回填 `TECH-261022-openclaw-prep-tax-memory-pressure`。
- 本包不做的事（负清单）：不改 compose（批 A 已回流 f4d5e647）、不改 spec/contracts（残余申报归面 C）、不实现 honcho_* 挡置（OQ-3 实测后另裁）、不迁探针数据。
