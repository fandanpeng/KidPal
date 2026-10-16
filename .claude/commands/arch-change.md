---
description: 架构+契约面变更 loop 入口（对偶 /requirement-change、/gov-change）。驱动一个架构或契约变更走面内 review loop：立项（human 裁意图 + 收敛度判断）→ 方向不清走 Track A 段（对抗收敛）→ architect 起草 → architect-reviewer 四查（恒定，改一个字也审）→ 轻/主判档 → 瘦制品过闸或 Track B 后端落地；触契约携带契约包随批原子落，无人签门。
argument-hint: <human 意图原话，例如 "把首页「接下来」区改成三行清单显示">
---

# /arch-change

你现在**主线程编排者**（四面纯编排：派发 / 落地 / 状态翻转 / 记账，**不起草变更内容**）。按 `.claude/references/arch-change.md`（流程 SSOT）推进。

> $ARGUMENTS

逐步执行，并在每一步显式说明：

1. **立项**：human 裁意图 + **收敛度判断**（方向清晰 → 直接起草审稿；方向未收敛〔该不该变、收益与影响看不清〕→ Track A 段）。带 spine 调用时开场先读 faceMap/facePlan，不得要求 human 重述意图。缺意图 fail-closed，AskUserQuestion 问。
2. **Track A 段（仅命中判据时）**：派 review-moderator 跑对抗机器（architect 起草候选 → scenario-author 冻 charter → challenger↔defender → adjudicator 裁定 → human 拍方向）；收敛后流回步骤 3，内容不再重复四查。
3. **起草**：派 `architect` 起草 PROPOSAL 改法集（输入显式含 human 原话 / spine sources / 讨论记录；触契约则含契约包三节）。
4. **审稿**：派 `architect-reviewer` 四查 + 判档（轻/主路径，判据见 SSOT；**只升不降**）。不过否回循环至过（architect 再改）。
5. **落地**：轻路径 = 瘦制品五件过闸（瘦 PROPOSAL + §7 GREEN + landing-manifest + 瘦 landing-summary + landing-verdict；retro 豁免）；主路径 = Track B 后端全件（FROZEN → packet-bound landing → landing-verifier → retro → 升级闸 → 合并本地 main）。
6. **触契约分支**：契约包三节并入 PROPOSAL 随批原子落；合并过机械凭据闸（`pre-merge-escalation-gate.mjs`），**无人类签字门**。
7. **记账收尾**：`node .claude/scripts/changes-index.mjs --upsert <changeId>`；跨面 → `change-intake.md` 建 spine 回填 `anchors.arch` + `status`（回写 spine anchors + status；写者恒 = 主线程，261015 批 T5 齿）。

你不起草变更内容、不改 reviewer 判决、不跳否回循环；实现面变更转 `/triage-change`；治理面变更转 `/gov-change`。
