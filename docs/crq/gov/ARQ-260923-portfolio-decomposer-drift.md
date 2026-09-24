---
id: ARQ-260923-portfolio-decomposer-drift
title: portfolio-planner 拆包产物是「有保质期的快照」——元数据与磁盘现实腐坏（非闸值域枚举 / sourceRefs 漏真实改动面 / 引用指向已不存在物）无任何机械闸可拦，intake 逐字段读代码面是唯一提前手段但纯 planner 习惯未成文
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（① 枚举修正半成本极低可先行；②③ 随批，与 ARQ-260922-buildfeature-launch-prep-skip / ARQ-260923-dispatch-prompt-hard-terms 同动 planner 编排文面可并批）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-23（pod-retro 实证：m2-k1 卡点 2、m2-k2 卡点 1、m2-k3 卡点 1、m3-k1 卡点 4、m3-k4 卡点 2；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（loop-retro-2026-09-23，18 包复盘，5 包实证）：① origin 枚举非闸值域——portfolio-planner 给 milestone 来源包产 `origin.type/upstreamType:"milestone"`，taskpackage-required 闸 `resolveOriginType`（upstreamType 优先）值域闭集不认：m2-k1 r1 红、m2-k3 intake 裸跑报红、m3-k1 r1 红，planner 各自在 finalize 权内改 `delivery-story` + milestone 溯源改走 upstreamRef——同一颗雷三包各修一遍，m3-k1 还留了「K2/K3 起包前预检」交接记忆；② sourceRefs 漏真实改动面——m2-k3 漏 4 处（最重 = `kidpal_render_node.h/.cc` 两处上包标注「保留不动、由本包改窗时统一收」的死态 API 承接面，恰是本包存在的理由）；m2-k2 四处引用腐坏（验收文件路径磁盘不存在 / 「冻结钉点」文件已进归档区不再跑 / 红基线命令 elf 路径 cwd 基准错 / refs 不含真实改动面单源）；③ m3-k4 contractRefs 空数组被判缺。共同点：机械闸只查字段形态，查不出「引用指向的东西还在不在」——m2-k2 retro 明言「intake 期的读代码面不是可省的成本项，是唯一的提前发现手段」；m3 批起「intake 裸跑形态闸 + 逐字段亲核」已成 planner 自发惯例（连续多棒净赚）但未成文、新棒靠翻 retro 自觉继承。**影响面**：凡接棒拆包快照的包（拆包到起包间隔越长越重）。**建议改动**（候选，交 review，不自批）：① portfolio-planner 拆包产出 origin 两键用闸值域内枚举值（`delivery-story`），milestone 溯源走 upstreamRef——上游修一次消全批雷（成本低可先行）；② intake「拆包产物逐字段对磁盘核验」成文进 planner.md intake 步（m2-k2 retro 已总结四问：验收文件存在性 / 冻结件是否仍在跑 / 命令 cwd 基准 / refs 是否覆盖真实改动面）；③（可选）taskpackage-required / brief-inputs 对引用字段加磁盘存在性 warn（advisory 不拦门）。**riskTier: medium**（拆包→接棒链每批必现；不涉契约）。owner=architect。
