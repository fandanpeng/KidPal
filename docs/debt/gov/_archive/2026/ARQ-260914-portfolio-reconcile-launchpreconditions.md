---
id: ARQ-260914-portfolio-reconcile-launchpreconditions
title: M-1 机械齿——`portfolio-reconcile.mjs` 读 workstream `decision-coverage.json` 顶层键 `launchPreconditions`（`需修订` spec 落点的 launch 前置清单），当前 `--check-launch` 不读该键、靠 convention + 人读核验
domain: gov
status: closed
severity: SHOULD
owner: architect（定核验判据）
trigger: 下次动 portfolio-reconcile.mjs / launch 检查面的治理批
opened: 2026-09-14
closed: 2026-10-27
closedBy: 261026-gov-review-p4-pod-board-cluster
paths: []
source: 260912-gov-review-portfolio-decomposition-chain（PROPOSAL §6 留根 + 260913 human 裁定 M-1；architect-landing-summary 诚实记录）
---

**证据**（260912 拆包链批，human 2026-09-13 AskUserQuestion M-1 三选一裁定）：本批落的是 convention 通道——portfolio-planner Step 4 decision-coverage 产出该键 + pod-coordination §2b convention 核验条款（`--check-launch` 不读、门判定语义不变、人读核验），机械齿裁定不做、ARQ 留根。**影响面**：launch 前置清单缺失或过期时 `--check-launch` 不报红，兜底只剩人读；凡 workstream 带 `需修订` 标注（spec 修订落进包批次）的场景均暴露。**建议改动**（候选，交 review，不自批）：portfolio-reconcile.mjs 增读 launchPreconditions 顶层键——键在则逐条核验对应 spec 修订已落、缺即报红；键不在视为无前置（向后兼容既有 workstream）。**riskTier: low**（advisory 检查器扩读，非门判定语义变更——若判为门扩齿须如实升 review）。owner=architect（定核验判据）。  **2026-09-20 起草期实测——规模判定更正：须先定数据模型，并入「拆包 / 输入 / launch prompt 流程化」批（human 裁定）**：本行随 `260920-gov-review-reader-scripts-align` 起草，被写成 OQ 后经人类读者两问暴露真实规模（两问均经实测成立）：① **「已经核了脚本知道吗？会再报吗？」→ 不知道、会永远重复报**——实测 `launchPreconditions` 是**纯字符串数组**（`delivery/workstreams/m1-home-qa/decision-coverage.json`，6 条、每条一整段散文），**无任何「已核」位**；而门 1 的核验结果落在**另一个文件** `review.md`（`## 门 1（拆包 review 门）—— 2026-09-15` + 五项逐项结论），脚本不读 ⇒ 单做「报 N 条待核」会产**永久噪音**。② **「人拍的事可能不是本包的，会显示出来吗？」→ 会、且是全量**——实测该清单是 **workstream 级**（一份、5 包共享），而 `--check-launch <changeId>` 是 **per-package**；看实况内容，第 3 条（页点 3/4）是 `keybar-three-slot` 那个包的事、第 4 条制约的是 `m1-k4`（**另一个包**）⇒ 每个包点火都会看到全部 6 条、其中几条与自己无关。**结论：缺的是清单的两个位（「核过没」+「属哪个包」），不是脚本读不读**——**同文件已有先例**：`decisionCoverage` 每条都带 `landsIn: "package:<id>"`（正是②所需的位），`launchPreconditions` 只是没跟上；改模型要动在册 workstream（迁移或兼容），**超出「读侧对齐、零设计分叉」的定位**。**技术前提已具备**：包→workstream 链接在（实测 `taskpackages/260915-m1-k2-keybar-three-slot/taskpackage.json` 的 `origin.workstream = "m1-home-qa"`），按包过滤可行。**交接动作**：先定数据模型（`string[]` → `{text, scope, status}[]` 或加平行已核位），再落 `--check-launch` 的读取与过滤；完整分析见 `delivery/changes/arch/260920-gov-review-reader-scripts-align/PROPOSAL.md` §6 留根①。**处置**：2026-09-20 由 human 裁定移出该批、并入「拆包 / 输入 / launch prompt 流程化」批。 | **2026-09-21 · 已部分承载（`260921-gov-review-launch-pipeline` T6 落地，本条不结）**：读侧 `--check-launch` 增读该键（按包过滤 + 只报 `status !== "verified"` + 旧形态裸 string 走汇总行不逐条报，**warn 通道、门判定语义不变**）+ canonical 定义改对象形态 `{ text, scope, status, evidence }`（`text`/`scope` 归建包方、`status`/`evidence` 归 launch 执行者）+ `portfolio-reconcile.mjs --self-check` 补双夹具。**未闭合面 = 存量 12 条数据迁移**（`m1-home-qa` / `m2-char-window` 各 6 条裸 string），理由：`m1-home-qa` 无逐条核验痕迹可引、强填 `status` 即自撰凭据 ⇒ 留根该批 §6 留根①，由建包方下次动该 workstream 时随批迁移。

**2026-10-27 · 结项（`261026-gov-review-p4-pod-board-cluster`，整条闭合）**：存量 12 条数据迁移随批落地——`m1-home-qa` / `m2-char-window` 两份 `decision-coverage.json` 的 `launchPreconditions` 全部迁对象形态 `{text, scope, status, evidence}`（text 逐字 ≡ HEAD、顺序不变；scope 机械取自条目点名承载包）。**诚实填法兑现**：verified 只给 5 条有独立耐久锚的（m1 #2/#3/#4、m2 #2/#3，锚经判官 R1 逐条亲核），其余 7 条一律 `unverified` + evidence ""——两条专用 base ref（base/m1-home-qa / base/m2-char-window）实测 `git cat-file -e` 不可达，「commit-before-launch 已履行」不追认（260921 T6「强填即自撰凭据」顾虑正面兑现）。读侧 260921 T6 已落、本批补齐数据侧 ⇒ 本条闭合，无留根。
