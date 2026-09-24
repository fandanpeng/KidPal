---
id: ARQ-260923-gate-tooling-noise-batch
title: 治理脚本 / 钩子的「用户面」噪声与危险静默小修集合——误导提示 / 静默相位回落 / 传参形态不一 / 非闸守卫脚本漂移无主
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（一行级小修集合，可搭任意批）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-23（pod-retro 实证：m2-k3 钩子节、k7 钩子节、m2-k1 卡点 5、rma8 钩子节、m2-k2 卡点 5；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（loop-retro-2026-09-23，跨棒四处）：① commit-safety.mjs 在 `git add` 后弹「staged 为空——git add 可能未生效」（读的是 add 之前的快照）——m2-k2 登记一次、m2-k3 一棒三回、card-fix 两回，每回多跑一步 `git diff --cached --stat` 消化，m2-k3 retro 原话「同一条已登记两次的噪声提示，值得在下一批治理里修掉——它消耗的是每一棒 Pod 的注意力」；② verify.mjs 未知 phase 值静默回落 scoped——K7 传 `--phase=full` 静默退回 scoped、差点把 scoped 绿当 final 绿，其 retro 判「参数不认时静默降级为默认相位是危险形态」，建议未知值直接报错；③ 治理脚本传参形态不统一 + `--help` 缺失——changes-index.mjs 无 --help 分支（rma8 整合期想看用法反触发全量重建、顺带纠偏 4 行历史 INDEX）、mutation-check.mjs 位置参数且 `--change-id` 旗标被当路径（m2-k1 踩），m2-k1 retro 判「治理脚本传参形态不统一是系统性小税」；④ enforce-scope-canary 期望过期（m2-k2 四条，两条与治理变更后新增授权面相悖、两条与 docs 目录层级退役相悖）——非闸集脚本漂移后无人会被提醒复跑。**建议改动**（候选，交 review，不自批）：小修批四处各一行级——① commit-safety 提示口径改「以 `git diff --cached --stat` 为准」或读 add 后快照；② verify.mjs 未知 phase 值 fail-loud；③ changes-index / mutation-check 补 `--help` + 传参旗标对齐；④ enforce-scope-canary 挂进某复跑清单或声明退役归档。均零门判定语义变更。**riskTier: low**。owner=architect + 主线程 landing。
