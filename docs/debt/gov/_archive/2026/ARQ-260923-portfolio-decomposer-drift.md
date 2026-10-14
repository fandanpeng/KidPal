---
id: ARQ-260923-portfolio-decomposer-drift
title: portfolio-planner 拆包产物是「有保质期的快照」——元数据与磁盘现实腐坏（非闸值域枚举 / sourceRefs 漏真实改动面 / 引用指向已不存在物）无任何机械闸可拦，intake 逐字段读代码面是唯一提前手段但纯 planner 习惯未成文
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 已结（2026-09-25，260924-gov-review-gate-parse-teeth T5 承载残余③；①② 批 A T5 已落）
opened: 2026-09-23
closed: "2026-09-25"
closedBy: 260924-gov-review-gate-parse-teeth
paths: []
source: loop-retro-2026-09-23（pod-retro 实证：m2-k1 卡点 2、m2-k2 卡点 1、m2-k3 卡点 1、m3-k1 卡点 4、m3-k4 卡点 2；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（loop-retro-2026-09-23，18 包复盘，5 包实证）：① origin 枚举非闸值域——portfolio-planner 给 milestone 来源包产 `origin.type/upstreamType:"milestone"`，taskpackage-required 闸 `resolveOriginType`（upstreamType 优先）值域闭集不认：m2-k1 r1 红、m2-k3 intake 裸跑报红、m3-k1 r1 红，planner 各自在 finalize 权内改 `delivery-story` + milestone 溯源改走 upstreamRef——同一颗雷三包各修一遍，m3-k1 还留了「K2/K3 起包前预检」交接记忆；② sourceRefs 漏真实改动面——m2-k3 漏 4 处（最重 = `kidpal_render_node.h/.cc` 两处上包标注「保留不动、由本包改窗时统一收」的死态 API 承接面，恰是本包存在的理由）；m2-k2 四处引用腐坏（验收文件路径磁盘不存在 / 「冻结钉点」文件已进归档区不再跑 / 红基线命令 elf 路径 cwd 基准错 / refs 不含真实改动面单源）；③ m3-k4 contractRefs 空数组被判缺。共同点：机械闸只查字段形态，查不出「引用指向的东西还在不在」——m2-k2 retro 明言「intake 期的读代码面不是可省的成本项，是唯一的提前发现手段」；m3 批起「intake 裸跑形态闸 + 逐字段亲核」已成 planner 自发惯例（连续多棒净赚）但未成文、新棒靠翻 retro 自觉继承。**影响面**：凡接棒拆包快照的包（拆包到起包间隔越长越重）。**建议改动**（候选，交 review，不自批）：① portfolio-planner 拆包产出 origin 两键用闸值域内枚举值（`delivery-story`），milestone 溯源走 upstreamRef——上游修一次消全批雷（成本低可先行）；② intake「拆包产物逐字段对磁盘核验」成文进 planner.md intake 步（m2-k2 retro 已总结四问：验收文件存在性 / 冻结件是否仍在跑 / 命令 cwd 基准 / refs 是否覆盖真实改动面）；③（可选）taskpackage-required / brief-inputs 对引用字段加磁盘存在性 warn（advisory 不拦门）。**riskTier: medium**（拆包→接棒链每批必现；不涉契约）。owner=architect。

**部分承载（2026-09-24，260924-gov-review-planner-orchestration-align T5，本条不结）**：① 已落——task-intake-and-slicing.md origin 节新增「milestone 来源包 origin 填法」段（`origin.type`/`upstreamType` 必须取闸值域内枚举值、milestone 溯源走 `upstreamRef = delivery/milestones/milestones.jsonl#<milestone-id>`，附 m3-k1 收敛先例 JSON；`milestone` 不在 `ORIGIN_TYPES` 闭集写出必红），portfolio-planner.md Step 1 末追加指针一句——上游修一次消全批雷；② 已落——planner.md intake 新增 1f「拆包快照保鲜核验」步：Step 1① 命中 portfolio taskpackage 后、进 ② 归一前逐字段对磁盘核验四问（验收文件存在性 / 冻结件仍在跑 / 命令 cwd 基准 / refs 覆盖真实改动面——m2-k2 棒总结原方），漂移在 finalize 权内修正记 notes（origin 不可改，F-08 白名单），修不了 needs-triage；m3 批起自发惯例自此成文。③（taskpackage-required / brief-inputs 对引用字段加磁盘存在性 warn 齿，advisory 不拦门）**未落**——G3 机械齿按该批 OQ-3(a) human 裁定留批 C。证据：该批 PROPOSAL §2 T5 + §5 V4-T5 + §7 R2 GREEN。

**批 C 起草期实测复证（2026-09-24，260924-gov-review-gate-parse-teeth 起草前）**：③ warn 齿处方落点复核仍在——taskpackage-required-check.mjs 对全部引用字段（docRefs / sourceRefs / contractRefs / planningRefs / acceptanceTestsFile / origin.ref(upstreamRef) / crossEnd.counterparts[].path）**只查形态、零磁盘存在性消费**；现触磁盘的仅 input-brief 双份在场检查（:386-394 existsSync）与 canonical↔current sha256 比对（:140-155）。brief-inputs.mjs 已有**半齿**：renderAnchorSections 对不存在文件产 `status:'missing'` 行（:125-127，brief 内可见）但 advisories 通道只覆盖 bare-anchor ref（:131），missing 态无 advisory。**起草期实测新发现同族件**：mutation-check.mjs:111 对不存在的 acceptanceTestsFile 条目**静默 continue 跳过**——验收文件路径全错的包可绿过 mutation 闸，与本条「机械闸只查字段形态、查不出引用指向的东西还在不在」同根，归入 ③ warn 齿处方落点面一并设计（不另立条目）。①② 已由批 A T5 落地复核确认，勿重复。

**结项（2026-09-25，260924-gov-review-gate-parse-teeth T5）**：③ 落地——taskpackage-required-check 增纯函数 `collectRefWarnings`（七类字段 fragment-stripped + ANCHORABLE 形状过滤 + existsFn 注入）与 `warnings` 键（ok/fail 两路输出，exit 判定零变更）+ `--self-check` 5/5 判别 case；brief-inputs missing 态补 advisory（与 bare 态同通道）+ self-check 29/29；mutation-check 缺失验收文件从静默跳过改 `warnings` 可见（纯函数 + 判别 case，self-check 12/12）。**闸行为不变实证**：char-stroke 历史包显式跑，改前版（git show HEAD 临时对照件）与改后版 ok / missing 逐条 / coverageErrors 全部相同，warnings 3 == 起草期 census 该包缺失数 3（纯增量键）。**新发现同族件一并承载**（mutation-check 静默跳过，见上段）。census 口径（R1-F1/F2 勘正后）：43 包目录 694 条引用（478 在场 / 87 路径形缺失分布 23 包 / 129 非路径 token 跳过；「排除 current/ 指针副本」口径经判官 R2 裁定接受）。证据：本批 PROPOSAL §2 T5 + §5 V5 + landing-summary。
