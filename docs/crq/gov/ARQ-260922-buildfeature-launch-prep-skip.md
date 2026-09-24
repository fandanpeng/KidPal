---
id: ARQ-260922-buildfeature-launch-prep-skip
title: `/build-feature` 直启路径不触发 launch-prep——DEV-GOVERNANCE §4 图的 ② launch 面准备阶段（`--check-launch` 门 / worktree 等价化 / 专属图索引 / 已知红基线锚 / 启动卡 / board 预填）在 changeId 直启形状下整段被绕过
domain: gov
status: deferred
severity: SHOULD
owner: architect（定预检判据）+
trigger: 下一治理批（与 ARQ-260922-pod-requirements-not-authority 同批并批——同动 dev-loop 入口/编排文面；与 ARQ-260918-pod-session-settings-identity-unobservable 残余亦同族可同批）；m3-k5 在跑不阻断
opened: 2026-09-22
closed: ""
closedBy: ""
paths: []
source: 260921-gov-review-launch-pipeline 落地后实测清点（board pods json + taskpackage 目录 launch-card 清单，2026-09-22，human 报「build feature 走老路」触发核验）
---

**证据**（文面 + 执行实证双锚，2026-09-22 实测清点）：① 文面锚——`.claude/commands/build-feature.md` 正文从 Step 1 直接进 planner intake，全文未提 `kidpal-pod-launch-prep`；`.claude/agents/planner.md:35`（Step 1①）对启动卡缺失的处置是「如实记录进 pod-events 并继续，**不设门**」——该裁定本身是对 Pod 内侧的交接断裂保护，但结果是 launch 面缺什么 Pod 都裸起步、无任何环节把主线程引回 ② 阶段手册。② 执行锚——`260921-gov-review-launch-pipeline`（landing 2026-09-21）把启动卡落点定为 `delivery/taskpackages/<changeId>/launch-card.md`；实测其后启动的 m3 全批五包 board 条目在册（git common dir `devloop/pods/260921-m3-k1…k4` + `260922-m3-k5-sntp-time-sync.json`，状态 merged×4 / running×1）而五包 launch-card.md **全部缺失**，且旧路径的 launch prompt 文件（`.git/devloop/launch/prompt-*.txt`）也止于 `260917-m2-k3`——即新旧两种 launch 面产物都没人备，② 阶段被整批跳过，human 观察「build feature 还是走的老路」与此吻合。**影响面**：凡经 `/build-feature <changeId>` 直启的 portfolio 包必现：调度判定与 `--check-launch` 机械门、固件环境等价化、专属图索引（ARQ-260905 钉死的 launch 面必办，2026-09-05 已有漏建事故先例）、上游已知红基线锚（缺位会让 Pod 首轮 verifier 误把上游已知红记成本包回归）、启动卡、board worktreePath 预填全部缺位——等于 launch-pipeline 批建立的 ② 阶段对手工 skill 调用生效、对默认实现入口 `/build-feature` 不生效。**建议改动**（候选，交 review，不自批）：`build-feature.md` 命令正文在 Step 1 之前加一段「launch 面预检」——输入为 changeId 形状（launch-argument，即 planner.md Step 1① 的 launch-argument 形状）时，先核 launch 面四件（board 条目 / launch-card.md / 运行时 worktree / 专属图索引）：齐 → 原样进 intake；缺 → 不进 planner，先按 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 把 ② 阶段补齐（该 skill 条款「执行主体 = 主线程（human 会话）」与 `/build-feature` 运行位同口径）再进 intake。raw / human-ad-hoc 无 changeId 形状不触本预检。不动 planner.md「启动卡缺失不设门」既有裁定（Pod 内侧语义不变，本条治主线程入口外侧）；若把预检实现为机械闸（verify-gate / hook 判据）即构成新增 gate → 硬排除须升 full，本条限定为 command/skill 文面路由。**riskTier: medium**（每包必现、缺的是 launch 面三项必办与基线锚；不涉行为与契约面）。**review profile**：Track B 三要件逐条 = ① 执行测出 yes（m3 五包 board 在册、launch-card 全缺，实测清点）；② 问题清晰且修复明确 yes（入口外侧补预检路由，与 planner「不设门」裁定不冲突、无真设计分叉）；③ 半径已被执行揭示 yes（launch-pipeline 批落地当批即被绕过，凡 changeId 直启均中）。G 层事实：G2 = `.claude/commands/build-feature.md` + 可选 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 文面对账一句。机检硬排除逐项声明：不触 `docs/contracts/**`、无 spec-contract drift、不改任何 agent frontmatter 的 tools/name/model/write scope、不改「启动卡缺失不设门」裁定语义、不新增机械 gate、不触 self-def、不触 review-loop agent 定义、非 mixed lane、不涉 L0 蓝图。便宜验证计划：`route-regression-check.mjs` + `lint-descriptions.mjs`（description 不动、跑一遍证未误伤）+ 落地后用下一包做 launch 预检干跑核四件齐。owner=architect（定预检判据）+ 主线程（landing）。
