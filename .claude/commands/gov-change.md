---
description: 治理面变更 loop 入口（对偶 /requirement-change、/arch-change）。驱动一个治理变更走面内 review loop：立项（human 裁意图或确认 ARQ 修法）→ architect 起草 → architect-reviewer 四查 + 判档（文档面轻路径 / 机器面恒 Track B）→ 瘦制品过闸或 Track B 后端落地；Track A 段罕见（只看收敛度）。
argument-hint: <human 意图原话或 ARQ-id 清单，例如 "ARQ-260928-arch-change-retirement">
---

# /gov-change

你现在**主线程编排者**（四面纯编排：派发 / 落地 / 状态翻转 / 记账，**不起草变更内容**）。按 `.claude/references/gov-change.md`（流程 SSOT）推进。

> $ARGUMENTS

逐步执行，并在每一步显式说明：

1. **立项**：human 裁意图，或确认 ARQ 候选修法（argument 为 ARQ-id 时开场先读 `docs/debt/gov/` 条目——证据 + 建议改动 + riskTier 已预消化，不让人重述）。带 spine 调用时开场消费 faceMap/facePlan。
2. **Track A 段（仅命中判据时，罕见）**：判据 = 只看收敛度（方向开口 / reviewer 发现方向级缺口 / 声明红队），动承重机器但方向清晰的恒走 Track B。命中则派 review-moderator 跑对抗机器，human 拍方向后流回步骤 3。
3. **起草**：派 `architect` 起草 PROPOSAL（主路径五段式）或瘦 PROPOSAL（轻路径：Status 行 + 改法清单表 + 判档依据节）。
4. **审稿**：派 `architect-reviewer` 四查 + 判档（**路径即判据**：diff 全落 references/skills/notes/templates 且纯措辞 → 轻；触 commands/agents/hooks/scripts/scope-map/settings/G1 → 恒 Track B；只升不降）。不过否回循环至过。
5. **落地**：轻路径 = 瘦制品五件过闸（retro 豁免）；主路径 = Track B 后端全件（FROZEN → packet-bound landing → landing-verifier → retro → 升级闸 → 合并本地 main）。
6. **记账收尾**：`node .claude/scripts/changes-index.mjs --upsert <changeId>`；跨面 → spine 回填 anchors + status（写者恒 = 主线程，261015 批 T5 齿）。

你不起草变更内容、不改 reviewer 判决、不跳否回循环；架构+契约面变更转 `/arch-change`；实现面变更转 `/triage-change`。
