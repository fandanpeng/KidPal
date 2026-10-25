---
id: TECH-261025-dreaming-capture-pollutes-honcho
title: memory-core dreaming 子回合被 honcho 插件当普通对话采集，污染孩子画像（占最近 100 条窗口 13%）——第三方插件采集策略与宿主记忆政策不同源
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（配置包）+ 主线程（清理运维轮）+ architect（面 C spec 修订）
trigger: 261025-kidpal-tools-profile-fix 落库并部署完成后起配置包；数据清理排在其部署轮之后的 human 运维轮
opened: 2026-10-25
closed: ""
closedBy: ""
paths:
  - server/deploy/261025-openclaw-dreaming-disable/
  - server/deploy/manifest.yaml
  - server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
  - server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py
  - docs/architecture.md
  - docs/decisions/ADR.md
source: "delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md（诊断全文 + architect 裁决 + human 四裁决）；ECS 2026-10-25 实测（Honcho documents/messages/sessions 三表计数、journal 03:01 event_loop_delay 告警、openclaw plugins inspect 与 config get）；上游文档 docs/concepts/dreaming.md:103 与 docs/reference/memory-config.md:487；插件源码 dist/helpers.js isSystemRun/classifySession"
handoff: "配置包 → dev-loop（trigger=261025 工具面批部署完成后起批）；数据清理 → 主线程运维轮（trigger=配置包部署轮之后）；spec 修订 → 面 C arch 批；升级清单一行 → 随配置包同批落"
---

**证据**（2026-10-25 ECS 实测，逐条可复核，详见 `delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md`）：

1. **机制**：`memory-core` dreaming 每相位起一个「写梦境日记」的后台子回合（`docs/concepts/dreaming.md:103`），session key = `agent:kidpal:dreaming-narrative-memory-core-v2-<phase>-<hash>`；其 user 消息正文逐字以 `Write a dream diary entry from these memory fragments:` 开头，后接真实孩子对话片段。
2. **为什么被采集**：honcho 插件 `dist/helpers.js` 的 `isSystemRun()` 只认「sessionClass=cron / trigger∈{cron,heartbeat} / provenance=internal_system」三条；`classifySession()` 只把 `subagent:` 判 subagent、`cron:` 判 cron，`dreaming-narrative-...` 落入 chat 分支（Honcho 侧会话名实为 `chat-dreaming-narrative-...`）。故 `captureSystemRuns: false` 对该类会话无效。
3. **上游政策对照**：OpenClaw 自身 memory 索引明确排除这类会话——`docs/reference/memory-config.md:487`「Internal dreaming-narrative, cron, and heartbeat session transcripts are not indexed」；honcho 插件（`@honcho-ai/openclaw-honcho@1.7.0`，npm 最新发布版）无此排除，配置面亦无会话级排除项。
4. **量化**：做梦会话 5 个（kidpal 3 / main 2）、消息 16 条；派生结论 owner 侧 **17 条**（kidpal 10 / main 7）、agent-kidpal 57 条、agent-main 31 条；**owner 侧最近 100 条结论窗口里 13 条来自做梦**（该窗口即插件 `session.context` 注入画像的取数窗口）。
5. **零产出旁证**：两 workspace 均无 `MEMORY.md`（做梦唯一永久产物从未落地，deep 相位恒 "Promoted 0"，TECH-261022 证据 6 在案）；DREAMS.md 仅叙事、非检索源。
6. **附带成本**：每日一次做梦 LLM 生成；2026-10-25 03:01 journal `event_loop_delay p99=4185ms` 存活告警（ECS 2C/4G 与小智引擎共置）。

**影响面**：直接挤压「孩子问旧事」的召回质量——最近 100 条窗口是插件注入画像的唯一取数面，13% 被机器自言自语占位，真实旧事被挤出（与 backfill 观察者缺位、窗口截断两层病因叠加）。同时污染 agent 侧自表示（agent-kidpal 57 条 / agent-main 31 条）。发生频率 = 每个做梦调度周期（默认 `0 3 * * *`，实测 light/rem/deep 三相位各产一条日记）。不修则每次做梦都继续生产新污染，且人工清理后仍会再生。

**建议改动**（human 2026-10-25 四裁决已定，交各面 loop 执行为准，本条目不自批落地形态）：

- **配置包**（②单包 Lite / riskTier high / gateProfile security）：新建 `server/deploy/261025-openclaw-dreaming-disable/`——声明恰一个叶键 `plugins.entries.memory-core.config.dreaming.enabled=false` + apply/rollback（复用 honcho apply 的安全编排：幂等预检 → drift-guard → 时间戳备份 → `--gen-patch` → `config patch --dry-run --json` 核 `checks.schema === true` → 正式 patch → `config validate` → restart → health）；manifest 登记三件；验收锚含「声明不得引入 memory-core 其它键」负锚。**禁脚本直写 `openclaw.json`**（K3 实测直写 → exit 78 崩循环）。
- **数据清理**（human 运维轮）：先按 provenance 导出 105 条派生结论 → 等 deriver 队列排空 → 删结论 → 删 5 个会话 → 计数归零 + 窗口构成核验 + 真机复测「王一诺是谁」仍答 9-30 事实。顺序纪律 = 先导出 ID 再删会话（派生结论不以会话为外键，先删会话会丢定位线索）。
- **升级清单防线**（human 裁：先不做探针）：`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` 升级检查清单钉一行「OpenClaw 或记忆类插件升级后，跑一次 Honcho 会话家族盘点（`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 家族必须为空）」。
- **spec 修订**（并入在途「面 C」）：`docs/architecture.md` §6.7 把记忆写入范围从逐行枚举升级为**类级规则**（平台内部系统回合不入画像），`docs/decisions/ADR.md` ADR-016 补带日期注记。**不允许「先合实现、spec 永远不补」**。
- **相邻同类缺口**：`isSystemRun` 对 `subagent:` 前缀同样只参与分类、不参与排除；未来若起探针包，须把 `subagent:*` 一并列入「必须为空」集合。

**风险**：riskTier **medium**——改动为「关一个插件功能开关 + 一次数据清理 + 文档/清单」，无代码逻辑变更、无契约面；主要风险 = ① 关 dreaming 后若上游存在「dreaming 相位顺带刷新索引」的隐藏耦合（预期不存在），用部署轮前后 `memory_search` 对比实证、一键回滚；② 删数不可逆（Honcho 不备份，`docs/architecture.md` §8.1 风险 13），以「先导出 + 精确 provenance + human 过目」三步兜底。review profile = **Track B**（三要件全满足：执行测出有实证 / 问题与修复已收敛无人裁分叉 / 爆炸半径已量化实测；paths 不落 G1/G3/契约面/gate）。owner = dev-loop（配置包）+ 主线程（运维轮）+ architect（面 C）。**形态注记**：若 review 裁定须以机械 gate 或探针脚本落地，按约定升 full-adversarial。

**数据清理完成 append（2026-10-26，主线程 human 运维轮）**：human 2026-10-26 裁「删全部 105 条」。执行记录——① 删前导出全量 105 条到 `c:/tmp/dreaming_docs_export_20261026.json`（owner 17 / agent-kidpal 57 / agent-main 31，逐条人读确认全部为「做梦过程本身」的元描述，无独占真实事实；片段涉及的真实对话在正常会话中均有原句）；② `DELETE 105`（按导出 ID 精确删除）→ 残留 0；③ 删 5 个 `dreaming-narrative-*` 会话（16 条消息，REST DELETE 全 202）→ 消息残留 0、结论残留 0；④ 核验 = 最近 100 条 owner 窗口做梦来源 **0/100**（清理前 13/100）、owner 表示 3399→2417 字符、真事实完好（王一诺 4 条 / 向宏光 1 条 / 含「出卖」原话 7 条）；⑤ 配置侧同轮复核：`dreaming.enabled=false` 持久、昨晚 20:02:52 日志 `removed 1 managed dreaming cron job(s)`、今晨 03:00 窗口 dreaming 日志 0 条、DREAMS.md mtime 停在 2026-10-25 03:01、main 侧 `memory search` 检索正常。**本条目剩余 = 面 C spec 修订**（spine `delivery/intake/261025-spec-reconciliation-memory-writing-scope.md`，human 已明示明日启动）。
