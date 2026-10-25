# memory-core dreaming 污染 Honcho 画像 · 诊断与裁决（2026-10-25）

> 性质：诊断记录 + architect 评审意见转写。**不是** PROPOSAL、不是冻结件、不豁免任何门。
> 触发：2026-10-25 在治「孩子问旧事答不出」时发现 owner 画像被做梦内容污染；human 指示「联网查一下、把诊断做准、确定方案后交架构师 review」。
> 关联条目：[TECH-261025-dreaming-capture-pollutes-honcho](../../../docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md)

## 1. 现象（实测）

owner（孩子）侧画像注入内容里混着大量非孩子发言：

```
[2026-10-24 19:00:51] The memory fragments owner supplied state that the kidpal schedule.md holds four weekly entries…
[2026-10-24 19:01:37] Owner supplied a memory fragment containing the question "明天有什么课？"
[2026-10-24 19:01:12] owner prefers a fresh angle for the requested dream diary entry…
```

## 2. 机制（代码级定位）

1. `memory-core` 插件的 dreaming 在 light / rem / deep 每相位有素材时，"runs a best-effort background subagent turn" 写梦境日记（`docs/concepts/dreaming.md:103`，ECS 本地装包内文档，版本 2026.8.1）。
2. 该子回合 session key = `agent:kidpal:dreaming-narrative-memory-core-v2-<phase>-<hash>`。
3. honcho 插件 `dist/helpers.js` 的 `isSystemRun(ctx, provenanceKind)` 判据只有三条：
   `classifySession(sessionKey) === "cron"` / `ctx.trigger ∈ {cron, heartbeat}` / `provenanceKind === "internal_system"`。
4. 同文件 `classifySession` 只认 `subagent:` 前缀 → subagent、`cron:` 前缀 → cron、其余 agent-scoped 形态 → chat。
   `dreaming-narrative-...` 落在 **chat** 分支（Honcho 侧会话名实为 `chat-dreaming-narrative-...`）。
5. 故 `isSystemRun === false`，尽管 `captureSystemRuns: false`——该键对 dreaming 无效。
6. 采集到的 user 消息正文逐字为：

```
Write a dream diary entry from these memory fragments:

- User: 今天妈妈说准备养一只小猫。
- Assistant: 「王小喵」！这个名字太可爱啦…
- User: 我们还约了这周六下午一块下楼去玩。
```

deriver 把这些片段当「owner 说的话」抽成结论，进入 owner 表示。

**上游政策对照**：OpenClaw 自身 memory 索引明确排除这类会话——`docs/reference/memory-config.md:487`「Internal dreaming-narrative, cron, and heartbeat session transcripts are not indexed」。honcho 插件（`@honcho-ai/openclaw-honcho@1.7.0`，npm 已是最新发布版）未跟上这条政策；其配置面（README 配置表）只有 `noisePatterns` / `disableDefaultNoisePatterns` / `captureSystemRuns` / `ownerObserveOthers` / `crossSessionSearch`，**无会话级排除**。

## 3. 量化（ECS，2026-10-25）

| 指标 | 值 | 取法 |
|---|---|---|
| 做梦会话 | 5（kidpal 3 + main 2） | `sessions` 表 `id like '%dreaming-narrative%'` |
| 做梦消息 | 16 条 | `messages` 表按上述会话聚合 |
| 派生结论 · owner（孩子） | **17 条**（kidpal 侧 10 / main 侧 7） | `documents` 表 `session_name like '%dreaming-narrative%'` group by observer |
| 派生结论 · agent-kidpal | 57 条 | 同上 |
| 派生结论 · agent-main | 31 条 | 同上 |
| **最近 100 条 owner 结论里做梦来源占比** | **13 / 100** | 按 `created_at desc limit 100` 取窗后计数 |
| 两个 workspace 的 `MEMORY.md` | **均不存在** | `ls` 实测；做梦唯一永久产物从未落地 |
| DREAMS.md | 主 69 KB / kidpal 44 KB | 存在，但非检索源 |
| 附带成本 | 每日 03:00 LLM 生成；2026-10-25 03:01 journal `event_loop_delay p99=4185ms` 存活告警 | journalctl |

## 4. 架构师裁决（2026-10-25，全文要点）

**方案：A —— 全局关闭 `plugins.entries.memory-core.config.dreaming.enabled: false` + 一次性清理。** B（noisePatterns）/ C（打插件补丁）/ D（定期删会话）均不采纳。

判据四条：
1. **零产出**：两个 workspace 无 `MEMORY.md`，deep 相位恒 "Promoted 0"（TECH-261022 证据 6 在案）——从未交付过产物的巩固器，关掉不损失既有能力。
2. **成本实在**：占召回窗口 13%、每日 LLM 开销、event-loop 抖动；ECS 为 2C/4G 且与小智引擎共置。
3. **职责分离**：kidpal 工具面五项不含 `memory_search`（记忆体系是 Honcho）；main 不读 Honcho（走 memory-core）。做梦对两侧都不是产品载体。
4. **可逆且最小**：一个布尔键 + 声明工件 + 一条回滚命令；不触契约、不触端云链路、不触第三方 dist。

**main 侧损失面独立核（六层）**：memory 槽归属不动（memory-core 持槽、honcho 不抢槽）；检索索引由记忆文件与会话摄取维护，dreaming 是叙事生成不是索引器，且做梦会话本不入索引；巩固产物缺失、手动路径保留；DREAMS.md 非检索源；kidpal 零影响；Honcho 捕获与 dreaming 无关。**残余不确定性**：memory-core 的 sessions 索引是否在 dreaming 相位顺带刷新——上游文档未述，用部署轮前后查询实证闭环。

**两条硬约束**：
1. 配置改动必须走 `openclaw config patch`（写前 `--dry-run --json` 核 `checks.schema === true`、写前时间戳备份、可一键回滚）；**禁止脚本直写 `openclaw.json`**（K3 实测直写绕过 zod 校验 → 网关 exit 78 崩循环）。
2. 数据清理必须**先导出后删**，按 provenance 精确定位，先 dry-run 出清单交 human 过目再 `--apply`；不得按内容全文匹配或时间窗粗筛。

**治理落点**：跨面变更（架构面 + 实现/运维面）→ 入口 `/triage-change` 建新 intake spine；架构面走 `/arch-change`（并入在途「面 C」需求+架构对账批，避免同段落两轮改写）；实现面五档分流（配置包 ②单包 Lite / riskTier high / gateProfile security；数据清理 = human 运维轮）。**不是** `/gov-change`。

**回归防护**：只读「记忆采集来源盘点」探针 + 每日 timer + 告警件（按 session key 家族做白名单，不读消息内容），成本约 0.5–1.5 人日。**human 2026-10-25 裁：先记 debt，并在部署 runbook 升级清单钉一行。**

**风险预标**：两面均 **Track B**，不升 Track A（方向已收敛、无契约面、无跨系统结构变更；删数属执行风险而非方向不确定，按 NFR-PRIV-1 归 human 运维轮 + 删除前导出）。

**相邻风险**：`isSystemRun` 对 `subagent:` 前缀同样只参与分类、不参与排除——同一类结构缺口，未来探针须把 `subagent:*` 一并列入「必须为空」集合。

## 5. human 裁决（2026-10-25）

| 问题 | 裁决 |
|---|---|
| 方案 | 采纳 A（全局关做梦），接受「main 失去自动巩固（当前零产出）+ 开关无 per-agent 作用域」两项申报 |
| 脏数据清理 | human 运维轮执行（先导出、dry-run 清单过目、再删） |
| spec 修订 | 并入在途「面 C」，不单独开 micro arch-change |
| 回归防线 | 先记 debt + 在部署 runbook 升级清单钉一行；探针暂不做 |

## 6. 执行次序（含跨包约束）

1. 先让 `261025-kidpal-tools-profile-fix` 落库并部署完（其 drift-guard 锚升级后态，本单的声明同构依赖它）。
2. 配置包：声明 + apply/rollback + manifest + 验收锚 → 部署轮（备份 → gen patch → dry-run schema → patch → validate → restart → health）。
3. 观察一个调度周期（`0 3 * * *`）：无新增 `dreaming-narrative-*` 会话、DREAMS.md mtime 不动、main 侧 `memory_search` 前后不劣化。
4. 清理（human 运维轮）：先按 provenance 找出 105 条派生结论并导出 → 等 deriver 队列排空 → 按导出 ID 删结论 → 删 5 个会话 → 计数归零核验 → 最近 100 条窗口构成 dream 派生应为 0 → 真机复测「王一诺是谁」仍答 9-30 事实。
5. 清理后 24 小时幂等复核（捕捉迟到派生或孤儿文档）。
6. 面 C 落 spec/ADR 修订 + 变更记录（把 dreaming 类并入 §6.7 的类级规则）。

**清理的三个专业细节**：① 先导出结论 ID 再删会话（会话删除可能级联消息，派生结论挂在 pair collection 上、不以会话为外键）；② 精度优先于完整——17 条 owner 侧结论引用的片段来自真实对话，删前抽样核对源消息是否存在于非做梦会话；③ 回滚诚实申报——Honcho 按 human 2026-10-22 裁决不备份（`docs/architecture.md` §8.1 风险 13），导出文件是唯一恢复源。
