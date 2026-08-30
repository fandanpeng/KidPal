# ARCH-REVIEW-QUEUE · 架构 review 未结项队列

> 自 AInativecompany 迁移的净版队列（2026-08-27）。原仓历史条目不入本仓——原仓全量见 `c:\AI\AInativecompany\docs\ARCH-REVIEW-QUEUE.md`。
> 性质：工作队列（收件箱），只跟踪 `/review-arch`、`/review-governance`、`/arch-change` 产出的未结 spec / governance soundness 项：open findings、待拍板 deadlocks、follow-up、accepted-as-risk watchlist。已结项移入 `docs/archive/ARCH-REVIEW-QUEUE.closed.md`。
> 详细裁决、证据、debate log 归 `changes/arch/**/review-packet.md`、`decision-packet.md`；本文件只保留当前未结项。

## 0. AI 维护规则

| 规则 | 写法 |
|---|---|
| 新增 ARQ | 只有 unresolved / future-trigger / accepted-risk 仍需可见的项才进入 `1. 当前工作队列`。 |
| ARQ ID 格式 | 新条目用 `ARQ-<YYMMDD>-<english-kebab-slug>`：YYMMDD=创建日；slug=短英文 kebab（ASCII `[a-z0-9-]`、≤~5 词、不收尾 `-`）；人读中文标题留 title 列。子项引用 `ARQ-<YYMMDD>-<slug>-<子项>`。 |
| 已结 ARQ | human 批准且 landing 完成后即已结：从当前队列移除，在 `2. 已结索引` 加一行（ID / title / evidence 链接）。 |
| 计数口径 | 不手写计数：header 不带「（X 项）」后缀。 |
| 状态轨迹 | 治理落地状态不在本文件——由 `node .claude/scripts/changes-index.mjs --upsert <id>` 单行钉进 changes/INDEX.md arch 段。 |
| 写法约定 | 队列行散文写通顺完整的句子；内部标识符首次出现带一句实质说明；表格列结构与证据引用保持精确（见 AGENTS.md「输出标准」节）。 |

---

## 1. 当前工作队列

### 1.1 Blocker / 待拍板

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|

### 1.2 Deferred / forcing-function

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-tier1-boot-review | SHOULD | KidPal spec v0.1 + interface-contract 走首次对抗 review（spec 冻结前置门） | 项目现处 spec 草稿期，`docs/spec.md` 与 `docs/contracts/interface-contract.md` 均未经对抗 review；冻结前须过一轮（档 1 主用途即此）。review 时顺带裁定三个契约草案标记（E1 display 块、B1 课表 MD）是否达冻结级。 | human 说开始审 spec 时（建议入口 /arch-change 或 /review-arch） | 治理迁移（2026-08-27）首个登记项 |

### 1.3 Accepted-risk watchlist

| ID | 严重度 | title | 保留原因 | 复议触发 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-contract-mech-gate-gap | SHOULD | pass-oracle 第 4 关（spec↔contract 机械门）暂无脚本兜底 | 原仓 `arch-spec-contract-check.mjs` 深绑多文件 Prisma contracts 结构，不适用单文件 MD 契约面，未迁。pass-oracle 第 4 关在本仓降级为人工核对，须如实记录「机械门缺失」，不得虚报已机检。将来契约面复杂化（出现第二份契约文件或字段级 drift 高发）时写一个针对 markdown 契约表的一致性检查脚本补位。 | 契约面扩到 ≥2 文件 / 或出现首次 spec↔contract 漂移实例 | 治理迁移决策记录（2026-08-27，MIGRATION-NOTES §已知缺口） |

---

## 2. 已结索引

> 已结项按统一格式追加在此（ID / title / evidence），证据链接指向 `changes/arch/**`。
