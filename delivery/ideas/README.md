# delivery/ideas/ · 原始 idea 落盘

> idea 的第一落点：讨论中出现的新想法、human 原话、方向性念头，在过 `/triage-change` 之前先落在这里。**上游是聊天与会议记录，下游是 `delivery/intake/` 的 spine**（triage 产物）——两者不混放：本目录存「原始想法」，intake 存「探测完触面、排好 facePlan 的结构化批」。

## 定位（流水线中的位置）

```
讨论 / 灵感
   │  落盘（本目录，保持原话、不做结构化）
   ▼
delivery/ideas/IDEA-<YYMMDD>-<slug>.md        ← 你在这里
   │  /triage-change（四面探测、落 spine、按 facePlan 派发）
   ▼
delivery/intake/<changeId>.md（spine）
   │  按面派发
   ├── 需求面 /requirement-change
   ├── 架构+契约面 /arch-change
   ├── 治理面 /gov-change
   └── 实现面 /build-feature（五档分流）
```

## 纪律

- **原话优先**：human 的原始表述逐字保留（进 frontmatter `humanVerbatim` 或正文引文块），agent 的整理性复述只作附注、不替代原话——spine 的 `sources.humanVerbatim` 惯例同源。
- **不做结构化**：本目录不写 faceMap / facePlan / anchors——那是 `/triage-change` 之后的事，提前结构化等于替 intake 干活且会漂移。
- **只增不改语义**：追加讨论记录用 append；已落盘的原话不润色、不删除。想法被推翻了就追加一条并翻 status，不回改历史。
- **triage 之后不搬家**：idea 文件留在原地作出处凭证，frontmatter 翻 `status: triaged` 并回填 spine 的 changeId；spine 侧在其 `sources.refs` 引用本文件。

## 文件命名与 frontmatter

文件名：`IDEA-<YYMMDD>-<slug>.md`（slug 用短横线小写英文，与 intake 的 changeId 命名风格一致；`IDEA-` 前缀避免与 spine / TECH / ARQ 混淆）。

```markdown
---
ideaId: IDEA-261103-<slug>
date: 2026-11-03
status: raw          # raw → triaged（回填 spineChangeId）/ shelved（明确不做，附一句原因）
spineChangeId:       # triage 后回填，如 261103-<changeId-slug>
humanVerbatim:
  - "human 原话逐字引文，一句一条"
tags: []             # 可选：主题词，便于日后翻找
---

# <一句话标题>

## 原始想法

（human 原话 / 讨论记录 append 在此；每次讨论一个小节，带日期）

## 后续去向

（triage 之后回填：spine changeId、走了哪条面 loop；留空 = 还没分流）
```

## 状态机

| status | 含义 |
|---|---|
| `raw` | 已落盘、未分流（默认初始态） |
| `triaged` | 已过 `/triage-change` 落 spine，`spineChangeId` 已回填；idea 文件保留作出处凭证 |
| `shelved` | 明确决定不做（追加一句原因即可，不删文件） |

## 索引

本目录暂不设 INDEX（intake 的 INDEX 由 changes-index.mjs 生成，idea 无状态机机检需求，量级也小）；翻找靠文件名日期 + `tags`。量级涨了或要把 idea 纳入机检投影时，另立一批治理面变更扩 changes-index.mjs，不在本 README 预支。
