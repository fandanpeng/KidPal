# progress · device-memory-overhaul

> 执行日志（append-only）：改了什么、测了什么、失败/修复。行不覆盖。

## 2026-10-22 · 拆包（portfolio-planner，同 changeId 第二轮）

- **status：`awaiting-human:slicing-review`** —— 四件套已齐备并落盘，停于「门 1 + human 过目切法」之前；不拉子会话、不 launch Pod。
- **上游复核（intake）**：workstream PROPOSAL FROZEN v1.0（pin `sha1:b03fdcbb2150cbfa`）；arch PROPOSAL FROZEN v1.1（pin `sha1:68254183b7f6e6ab`，commit `0411e957` landing PASS）；review.md 契约四分类闭合（处 A `NO_CONTRACT_CHANGE` / 处 B `ACCOMPANIES_SPEC_CHANGE` 已随批原子落）；requirements v1.67 frozen；task_plan.md P1–P5 phases；milestones.jsonl `M5-device-memory`（state=planning）。
- **上轮停等两件复核**：① 里程碑登记（`M5-device-memory` 行在场，`delivery-status.mjs` 已重跑）✅；② `task_plan.md`（phases P1–P5 + merge order + launch 前置 + 已知窗口）✅；连同 PROPOSAL（③ 判据）三件齐全，拆包顺序链停等解除。
- **拆 5 包（单 Pod 串行，merge order 50–54）**：
  - `261022-m5-k1-prompt-carrier-migration`（P1，mo50，dependsOn []，medium/full/proposal）
  - `261022-m5-n2a-schedule-mechanical-injection`（P5，mo51，dependsOn K1，medium/lite/regression）
  - `261022-m5-k2b-persistent-session`（P2，mo52，dependsOn K1，high/full/proposal）
  - `261022-m5-k3-honcho-memory-production`（P3，mo53，dependsOn K2+K1，high/full/proposal）
  - `261022-m5-k4-history-corpus-backfill`（P4，mo54，dependsOn K3，high/full/proposal）
- **切片内容与 changeManifest 种子**：以 PROPOSAL §4 + task_plan.md + review.md §5 逐文件清单为准；P1 的 `openclaw-system-prompt.v12.md` 新建项已按 arch §6.1 / OQ-8 撤销，改为新增 `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`；既有 `kidpal-agent-prompt.txt` 去留/改写归 P1 定形（二选一、不得并存）。
- **story 机查**：S-F5.1（requirements v1.67 新建）不在 §8 已砍清单；可行性 AC1/AC2 ✅、AC3/AC4/AC5 🟡，无 🔴，无需立项处置记录。S-F1.3（课表问答）沿用。
- **跨包共享常量 / 冻结面交叉扫描**：常量互斥型未发现；行为碰撞型四处已登记处置——(a) manifest `.config.yaml` marker 锚「实时事实回合」P1 必须同批换成动态块锚；(b) 冻结验收 harness（test_prompt_v7_round_routing / test_261010 / test_261017）P1 经 acceptance-author 留痕 supersede；(c) `openclaw.json` 被 P1（tools）与 P3（Honcho 插件/hooks）两次声明式 patch，须顺序 apply / 幂等 / drift_guard；(d) `workspace-kidpal/AGENTS.md` 被 P1（v11 迁移）与 P3（v12 守则）串行编辑。详见 `decision-coverage.json` `_doc`。
- **落盘制品**：
  - `roadmap.md`（① 六目的投影 / ② render-roadmap.mjs 机械渲染 5 包列车图 / ③ 每包效果与回指 / ④ 非目标）
  - `delivery/taskpackages/<changeId>/taskpackage.json` ×5（acceptance 为空骨架 `[]`，acceptanceTestsFile 空——Draft 态，验收唯一写点归 Pod 内 acceptance-author）
  - `delivery/taskpackages/<changeId>/plan.md` ×5（背景摘录锚 workType 分流 / 可行性判据 / 跨包事实与共享冻结面约束 / 铁律 / demo 证据形态）
  - `decision-coverage.json`（R1–R9 + OQ-1–OQ-10 + 切片 + 非目标全覆盖；`crossPackageBehaviors` 8 条；`launchPreconditions` LP1–LP5）
- **机检**：5 包 JSON 均通过 `taskpackage-required-check.mjs` 除 Draft 态两字段（`acceptance` / `acceptanceTestsFile`）外零错误、零 warning；`render-roadmap.mjs` 渲染成功且编号与 changeId token 一致（M5-K1 / M5-N2a / M5-K2b / M5-K3 / M5-K4）。
- **board**：5 pod 预填 `queued` + merge order 50–54（`portfolio-reconcile.mjs --set-pod` 单一写者；worktree-path 留 null，由 launch 侧预填）。
- **待办（交给 human / 门 1）**：① human 发起门 1（architect 轻量 checklist 五项 + workType 分流），裁决 append `review.md` 门 1 节；② 门 1 GREEN + human 过目切法；③ commit-before-launch（四件套 commit 到专用 base ref，非 main；board 不入 commit）；④ 逐包 launch（P1 先，P5/P2 次，P3、P4 串行）。
