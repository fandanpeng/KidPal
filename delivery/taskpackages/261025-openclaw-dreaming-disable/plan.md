---
changeId: 261025-openclaw-dreaming-disable
title: 全局关闭 memory-core dreaming——一个布尔叶键 + 声明/apply/rollback 工作台 + 升级清单防线
rigor: lite
riskTier: high
sourceType: human-ad-hoc
planningImpact: attach-existing
gateProfile: ["security"]
date: 2026-10-25
mergeOrder: 58
dependsOn: ["261025-kidpal-tools-profile-fix"]
---

# 261025-openclaw-dreaming-disable · plan（Lite，自建单切片）

## 背景（provenance = 诊断 + architect 裁决 + human 四裁决，照引不重推）

权威输入：

- 诊断 + architect 裁决 + human 四裁决全文 = `delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md`
- debt 条目（落地形态 / 清理细节 / 风险）= `docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md`

机制（代码级，已定位）：`memory-core` 插件 dreaming 在 light/rem/deep 每相位有素材时起一个后台子回合写梦境日记，session key = `agent:kidpal:dreaming-narrative-memory-core-v2-<phase>-<hash>`；honcho 插件 `dist/helpers.js` 的 `isSystemRun()` 只认 cron / trigger∈{cron,heartbeat} / provenance=internal_system 三条，`classifySession()` 只把 `subagent:` 判 subagent、`cron:` 判 cron，`dreaming-narrative-...` 落入 **chat** 分支。故 `captureSystemRuns: false` 对该类会话无效，做梦片段被当「owner 说的话」抽成结论进入 owner 表示。

量化（ECS 2026-10-25 实测）：做梦会话 5 个、消息 16 条；派生结论 owner 侧 17 条（kidpal 10 / main 7）；**owner 侧最近 100 条结论窗口里 13 条来自做梦**（该窗口即插件注入画像的取数窗口）。两 workspace 均无 `MEMORY.md`（做梦唯一永久产物从未落地）；DREAMS.md 非检索源。

architect 裁决方案 **A**（全局关闭 `plugins.entries.memory-core.config.dreaming.enabled: false` + 一次性清理）。B（noisePatterns）/ C（打插件补丁）/ D（定期删会话）均不采纳。判据四条：零产出 / 成本实在 / 职责分离 / 可逆且最小。

两条硬约束：

1. 配置改动必须走 `openclaw config patch`（写前 `--dry-run --json` 核 `checks.schema === true`、写前时间戳备份、可一键回滚）；**禁止脚本直写 `openclaw.json`**（K3 实测直写绕过 zod 校验 → 网关 exit 78 崩循环）。
2. 数据清理必须先导出后删、按 provenance 精确定位、先 dry-run 出清单交 human 过目再 `--apply`——**不属本包**（human 运维轮）。

human 四裁决（2026-10-25）：① 采纳 A，接受「main 失去自动巩固（当前零产出）+ 开关无 per-agent 作用域」两项申报；② 脏数据清理 = human 运维轮；③ spec 修订并入在途「面 C」；④ 回归防线先记 debt + 部署 runbook 升级清单钉一行，探针暂不做。

前账依赖：`261025-kidpal-tools-profile-fix` 已落库并部署完（其 drift-guard 锚升级后态，本包声明同构依赖它）。当前 live 实测 `agents.entries.kidpal.tools = {profile:"full", allow:[read,write,exec,web_search,honcho_search_conclusions], deny:[]}`。

## 目标态（把「关做梦」的配置能力落进仓；live 应用归主线程部署轮）

新建 `server/deploy/261025-openclaw-dreaming-disable/`：

1. `dreaming-disable-declaration.json`
   - `changes` 恰一个叶键：`plugins.entries.memory-core.config.dreaming.enabled = false`
   - `drift_guard` = 现状锚 AND：dreaming 叶接受 `unset / false`（fresh 机 / 目标态），显式 `true` → drift；外加 `agents.entries.kidpal.tools` 三叶（deny / profile / allow）锚 261025 工具面批的 live 升级态；工具面回退（allow 四项 / profile 缺席或非 full / deny 非空）→ drift。
   - `backup.pattern` = `openclaw.json.bak-kidpal-dreaming-*`
   - `_note` 写清：全局关闭、memory-core 无 per-agent 作用域、重评触发 = main 出现「手动 MEMORY.md 维护不可接受」的实际诉求时必须整体重开并重评 kidpal 侧污染。
2. `apply_dreaming_disable.py`：复用 honcho/kidpal-tools 的安全编排——幂等预检（目标态已达即 SKIP，零外部命令）→ drift-guard（不中 exit 3 零写）→ 时间戳备份 → `--gen-patch`（展平叶键恰一个，零 config 读写）→ `config patch --dry-run --json` 核 `checks.schema === true` → 正式 `config patch` → `config validate` → `systemctl restart openclaw-gateway` → `openclaw health`。**禁脚本直写 `openclaw.json`**。memory-core 为 bundled 插件，无 plugins install / inspect / doctor 步骤。SKIP/drift 先后顺序照 `apply_honcho_plugin.py`（SKIP 先于 drift），头注写清该选择理由与本包与其他姊妹脚本的差异。
3. `rollback_dreaming_disable.py`：备份整档还原 + 重启面（`systemctl restart openclaw-gateway`，可经注入桩），形态照 `rollback_honcho_plugin.py`。
4. `server/deploy/manifest.yaml`：按同族先例登记声明 / apply / rollback 三件（`owner: deploy.sh, verify: md5, restart: none`）。
5. `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`：升级检查清单钉一行——「OpenClaw 或记忆类插件升级后，跑一次 Honcho 会话家族盘点：`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 家族必须为空（违反 = 采集策略漂移）」。
6. `server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py`（新建冻结件）：①声明恰一个叶键且值 false；②`--gen-patch` 产物展平恰一个叶键；③drift-guard 两态接受（fresh unset / 目标 false）、异常态 fail-closed（true 显式 → drift；261025 工具面回退 → drift）；④幂等 SKIP；⑤负锚：声明不得引入 memory-core 其它键。

## 技术方案（planner+architect 预评）

> 2026-10-25 · architect 1g 预评（只读 advisory）：不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据。下列为回填结论，planner 代笔落盘（task-intake-and-slicing.md:266 唯一例外）。

1. **drift_guard 接受集**：`dreaming.enabled` 用 `expect_any ["unset", false]`；工具面三叶 = `deny` `["unset", []]` / `profile` **精确 `"full"`** / `allow` 五项精确（顺序敏感）。**`profile` 不接受 `"unset"`**——本包是 261025 工具面的消费方而非生产者，接受 unset 即把「工具面回退」放过；与 honcho 声明「P1 未跑即 drift、逼先跑 P1」同族，fresh 机由 exit 3 报文指引先跑 261025。实现须复制 `apply_agent_tools.py` 的 `drift_problems`（含 `_MALFORMED` 哨兵与 `expect_any`）；honcho 版无 `expect_any`，且把「父键在场非对象」误判为键缺席。布尔叶另加 `cur is False` 严格判（Python 下 `0 == False`，否则畸形值被放过）。
2. **SKIP 先于 drift（照 honcho）安全**：写目标仅 dreaming 一叶，达标即零外部命令零写；工具面锚是前置基线而非写目标，且 261025 自身验收与 honcho drift_guard 是第二道防线。SKIP 分支把 `drift_problems` 打印为 **WARN**（exit 0 不变），避免「dreaming 已关 + 工具面回退」完全静默；该行为在 DD-A4 内钉一个子 case。
3. **无 plugins install / inspect / doctor**：memory-core 为 bundled，三步只服务 npm 第三方插件安装与加载核对。保留 honcho 序链末的 **NRestarts 崩循环核对**（K3 exit 78 教训，挂在 `openclaw health` 之后的附加断言，不破序链）。
4. **rollback「重启面」**：备份整档还原 + `--systemctl-bin` 注入桩 + 非 `--dry-run` 时 `systemctl restart openclaw-gateway`（`--dry-run` 只打印不写不重启）；还原后先 `openclaw config validate`，失败则不重启并报错（网关仍跑内存态）。与「禁脚本直写 openclaw.json」不冲突：禁令针对拼装新目标态绕过 zod（K3 崩因），还原本包自己 apply 前的时间戳快照 = 姊妹 rollback 同族的第②级回滚，须在头注显式区分。
5. **声明形状不加顶层 `plugins` 镜像**：唯一消费者是 apply（读 changes）与验收（读 changes/drift_guard）；镜像造第二事实源并增加 DD-A5 负锚歧义。字段照 kidpal-tools：`case / changes / drift_guard / backup / rollback / _note`。

**部署轮残余风险（申报）**：`config patch` 对 `plugins.entries.memory-core` 的合并语义仓内无取证——若为整对象替换，可能挤掉 memory-core 其它 config 键；部署轮须 patch 前后 `openclaw config get plugins.entries.memory-core` 比对，差异只允许 `dreaming.enabled`。另：若 live 实测 `dreaming.enabled` 为显式 `true`（非缺省），apply 会 fail-closed exit 3，须人工确认该显式态来源后经显式换代纳入接受集，不得静默放宽。

**changeManifest（architect 确认 draft 完整、无需增删）**：

```
server/deploy/261025-openclaw-dreaming-disable/**
server/deploy/manifest.yaml
server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py
```

**impactedTests 种子**：必纳入 `server/acceptance/test_prompt_v7_round_routing.acceptance.py`（PV-A7 扫描全表 entries：src 存在 / dst 在场）；建议纳入（同 manifest 工作台面 + 整档 `yaml.safe_load` 解析者，按目录 token 收窄、理论无感，作回归圈定）`test_261022_memory_isolation` / `test_261022_openclaw_kidpal_tools` / `test_261022_honcho_memory` / `test_261022_m5_k1_prompt_carrier_migration` / `test_261022_persistent_session` / `test_261022_honcho_backfill` / `test_261010_engine_decision21_overlay_persist` / `test_261013_qq_schedule_write_closure`（均在 `server/acceptance/`）。


## 验收与预算

- 验收 = `server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py`（冻结件，DD-A1..DD-A5）。
- 红基线 = 冻结件在未实现树上跑出的设计态红（builder 动工前实测）。
- 预算：iterationBudget 2 / fixBudget 3 / supervisorBlockBudget 2。
- mutationRequired = false（声明 / 配置脚本 / 验收锚的 Lite 包，证据以冻结断言 + live 既有实证承载）。

## demo 证据形态

- host 侧：冻结件真跑（声明形状 / gen-patch 单叶 / drift 两态接受与两态拒绝 / 幂等 SKIP / 注入桩编排调用序 / 回滚 dry-run 与还原 / manifest 三件登记 / 零 secret）。
- live 侧：**零 host 断言**——`openclaw config patch --dry-run --json` 接受产物、`openclaw.json` 实落态、重启后 `openclaw health`、一个调度周期观察无新增 `dreaming-narrative-*` 会话，均归 human 部署轮。

## 红线

不碰 `docs/contracts/**`；不改 `AGENTS.md`；不动 Honcho 栈与 backfill 数据；不做数据迁移（清理是 human 运维轮，不在本包）；不碰 `server/gateway/**`；不改 `docs/architecture.md` 与 ADR（spec 修订归面 C）；不 push、不合并 main、不碰 ECS。
