# WORKSTREAM-PROPOSAL.template · workstream 设计权威模板（canonical · 260912-gov-review-portfolio-decomposition-chain T1 落地，FROZEN v1.0）

> 用法：复制本件为 `.planning/<workstream-slug>/PROPOSAL.md`，替换 `<尖括号>` 占位。**起草方 = 主线程**（`/triage-change` ③档路径；输入常为 human 与主线程讨论的产物，起草权不转移至 portfolio-planner——下游写上游输入是倒置）。
> 本件是 workstream 的**裁决记录**：只管「三问 + 裁定」，冻结后不改。「列车怎么跑、走到哪了」归活文档 `roadmap.md`（模板 `WORKSTREAM-ROADMAP.template.md`）。
> **带【硬项】的小节是义务不是建议**——缺失即 portfolio-planner intake fail-closed 打回主线程补齐（不猜、不代写）。
> 权威序恒为：**spec（`docs/requirements.md` + `docs/architecture.md`）= 行为语义权威，本件 = 路径选择记录**。本件不得自冻成第二事实源。
> 文体：散文节按 AGENTS.md「输出标准」写通顺完整的句子；frontmatter 与标注 token（`within-spec` / `需修订` / `spec-silent`）保持精确字面。

---

```yaml
workstream: <slug，与目录名一致>
workType: [<new-capability | remediation | refactor>, ...]   # 字符串数组；可混合，逐条业务目的的类型归属在 §1 清单内标注
status: <draft | frozen>
opened: <YYYY-MM-DD>
upstreamRefs: [<触发来源：retro / ARQ / human 指示记录路径>]
```

# <workstream 标题>

> **状态：<DRAFT vX.Y | FROZEN vX.Y> · <date>**（FROZEN 凭据：human grilling 裁 OQ + architect 分流 review GREEN + human 盖冻结。作者≠判官。）

## 1. 要达成什么（业务目的清单）

**【硬项 · 枚举式】** 禁一句概括了事。每条目的含：可观察判据（人能看到/听到/按到什么）+ workType 标 + 来源引用。

| # | 业务目的 | 可观察判据 | workType | 来源 |
|---|---|---|---|---|
| 1 | <目的> | <人可观察的判据> | <new-capability / remediation / refactor> | 整改型引缺陷证据（file:line / 实测）；新建型引 story 锚点引用 |

- **新建型来源 = story 锚点引用**：`S-Fx.y · <标题>`（可选带 AC 号集，如 `S-F1.4 · 日程提醒（AC1、AC3）`）。**只加判据、不改语义**——story 语义以 `docs/requirements.md` 现文为唯一权威，本件不复制、不改写、不复述（引用形态权威 = skill `write-story`）。
- 本清单是 `roadmap.md` 每包效果段的**唯一上游源**。

**【硬项 · 起草侧自检两查（FROZEN 前自查，降低冻结后打回）】**

- [ ] 清单引用的 story 编号**无一命中** `docs/requirements.md`「已砍清单」节（按节名定位，位置随 spec 演进；grep 即判）。
- [ ] 引用 story 的可行性三态**无 🔴**；若有 🔴，已先有立项处置记录（改需求 / 改架构 + ARQ 或开放问题挂账）——🔴 不许静默入清单（Pod 无权自裁）。

## 2. 现在在哪（形态随 workType 分三子形）

**【硬项 · 节名固定字面】** 按本 workstream 的 workType 选用下列节名（混合型可并列多节）；节名与 `plan.md` 骨架锚名、`roadmap.md` ① 节投影引用**逐字一致**，不得改写。

### 现状与差距

（**新建型**用本节。）现状盘点：已就绪基础、可复用资产、前置批次状态。差距 = 目标能力 − 现状（「还没建」，非「坏了」）。现状盘点即 RTE 拆包的原料输入。

### 问题与根因

（**整改型**用本节；「问题与根因」标题仅此形态合法。）问题行为 + 证据锚（file:line / 实测输出 verbatim + 日期）+ 根因。

### 结构性痛点

（**重构型**用本节。）结构性痛点证据 + 为什么现在动。

## 3. 怎么走（方案空间 + 裁定）

### 3.1 方案空间与裁定

**【硬项 · 每条裁定带 spec 关系标注】** 标注 token 三选一，字面精确：

| 裁定 | 内容 | spec 关系 | 说明 |
|---|---|---|---|
| <OQ/裁定 id> | <裁定内容> | `within-spec` | spec 已定此语义，裁定只选实现路径——引 spec 条款号：<requirements/architecture §x.y> |
| <裁定 id> | <裁定内容> | `需修订` | 裁定要改 spec 已钉死的语义 → 必须先走 spec 修订流程（Track B `/arch-change` → 对抗 review → human 冻结）；修订落点随语义面二分（story 语义 → `requirements.md`、支撑语义 → `architecture.md`）。**执行语义 = 受影响包的硬 launch 依赖**：登记到该包 `launchPreconditions`（粒度 = 包，经 decision-coverage 映射，非整 workstream 陪等）；spec 修订未冻结，承载该裁定的包不得 launch（convention + 人读核验，见 `pod-coordination.md` §2b）。spec 修订在本件冻结后尽早并行发起。 |
| <裁定 id> | <裁定内容> | `spec-silent` | spec 未载此事——留根登记，由门 1 或收口定是否补进 spec。 |

### 3.2 human 取舍点（OQ）与裁定

每个 OQ：选项 (a)/(b)/(c) + 推荐位 + 理由一句话 + **human 裁定结果与日期**（grilling 触点产出）。

### 3.3 非目标（防 scope creep）

本 workstream 不做什么 + 各归哪条线/批次。

### 3.4 逃生门申报

实现期发现与 spec / 契约冲突时的出口（走 `/arch-change` 判级 + human 冻结；契约面走 architect-reviewer 四分类 + 人类契约门），不在 Pod 内自裁。

## 4. 拆包预判（意图提示，非权威）

初步切法与批次设想。**本节是意图提示**——拆包权威 = portfolio-planner 产的四件套（`roadmap.md` + `taskpackage.json` × N + `plan.md` × N + `decision-coverage.json`）经**门 1**（architect 轻量 checklist review）冻结 + human 过目切法。

## 5. workstream 收口标准（可选，跨包并集验收时用）

整列车「做完」的可观察判据（真机轮 / 端到端场景），并指明收口轮 owner 包。

## 6. 留根清单

不关的根 + 交日落复盘项 + 登记不硬治项。

## 7. 评审记录

（architect 分流 review findings + sign-off 落 `review.md`；本节记 human 触点与冻结凭据，append-only。）
