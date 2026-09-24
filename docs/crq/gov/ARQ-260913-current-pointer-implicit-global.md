---
id: ARQ-260913-current-pointer-implicit-global
title: `delivery/taskpackages/current/` 指针是隐性全局状态：指错包即静默假绿或簿记污染，三包复现；另有指针副本与 canonical 的行尾字节差同族面
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下次动起包 checklist / dispatch-stamp 解析面的治理批
opened: 2026-09-13
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-13（13 包复盘；pod-retro 实证：home-p1 教训 2、display-first-m1 卡点 2、schedule-reminder §四、screen-unification 改进 4）
---

**证据**（loop-retro-2026-09-13，3 包 retro 实证）：① home-p1 worktree bootstrap 把上包（g-relay-fix）的 current 残留带入，verify.mjs 九闸跑在错误包上下文全绿（docRefs 全是 GRF-A*），靠顺手瞄一眼才识破；② display-first-m1 dispatch-stamp/journal-append 经 current/ 解析 changeId，指针一天没切到本包，簿记写进上一批 PKG-2 的 pod-events、还得 git checkout 还原再重灌；③ schedule-reminder intake 时 current/taskpackage.json 投影指着上一个包，不刷就静默假绿。**同族面**（screen-unification）：current/ 指针副本与 canonical 行尾字节差（Write 产 LF vs 拆包侧 CRLF）致 sha256 比较不一致——指针副本刷新须字节级复制、禁 Write 重打全文。**影响面**：凡经 current/ 解析的脚本（dispatch-stamp / journal-append / verify.mjs / closeout-check）+ 每包 intake/verifier 轮。**建议改动**（候选，交 review，不自批）：① 起包 checklist 固定第一条「核 current/ 指向本包」（planner 既有 private 习惯升级条文）；② 经 current/ 解析的脚本启动时回显所解析 changeId、与本包不符即 fail-loud；③ 指针副本刷新走字节级复制（screen-unification retro 改进 4 已自带此条，成文推广）。**riskTier: low**。owner=architect。  **部分已结（2026-09-19）**：建议 ② 已由 `260918-gov-review-closeout-chain-friction` T2 落地——`dispatch-stamp.mjs` 抽 `resolveChangeIdWithSource()`、**始终回显**所解析 changeId 与其**实际来源**（两条路径如实区分），并新增可选 `--expect-change-id <id>` 硬校验（不符／在场而无上下文均 fail-loud；不传则行为与改前一致）；landing-verdict PASS、commit `ae1dce0`。**残余**：①（起包 checklist 固定第一条「核 `current/` 指向本包」，触 `planner.md` agent def 面）与 ③（指针副本刷新走字节级复制）**未落**，见该批 §6 留根③/④；同族解析点实测 **17 个文件**（其中 `.claude/hooks/verify.mjs:123` 是病例① 的承载者）本批未改。
