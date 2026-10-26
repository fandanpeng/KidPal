---
id: ARQ-261026-release-pr-gov-lane-zero-pods
title: release-pr.mjs 的 PR 标题/正文按 dev-lane Pod 合并行提取——gov 治理批发布时落成「0 个 Pod 合并」无信息量门面（261026 PR #40 实测 + human 点名）
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下个动 `.claude/scripts/release-pr.mjs` 的治理批；或下次 gov 批发布被人再次点名时立即升格
opened: 2026-10-26
closed: ""
closedBy: ""
paths: [".claude/scripts/release-pr.mjs"]
source: [PR https://github.com/fandanpeng/KidPal/pull/40 自动生成 title「release(261026-6): 0 个 Pod 合并」+ body「本批已合并 Pod：（未从 merge-driver 合并行提取到 changeId）」（gh pr view 实测存档于本条目登记当日会话）, human 2026-10-26 反馈原话「commit 写的不对，得写这次改了啥吧」]
---

**证据**：`release-pr.mjs --publish` 生成的 PR 标题/正文按 merge-driver 合并行提取 changeId（dev-lane Pod 合并形态）；gov 治理批（制品在 `delivery/review/`，无 Pod 合并行）提取为空 → PR #40 标题落成「0 个 Pod 合并」、正文落成「未从 merge-driver 合并行提取到 changeId」——发布门面对评审者零信息量，批次说明只能靠主线程事后 `gh pr edit` 人工重写（本次已重写为 T1–T8 明细 + 制品指针）。批次本身有完整承载（feat commit 正文 + `delivery/review/<changeId>/` 四件制品），缺的是发布脚本的 lane 感知。

**影响面**：每个 gov-lane 批发布一次；门面误导评审（「0 个 Pod」读起来像空发布），且每次都要主线程手工补写 PR 描述才能给评审者可判的入口。

**建议改动**（候选，交 review、不自批）：① 提取逻辑增 gov-lane 分支——`delivery/review/<changeId>/` 在场时标题取 changeId + PROPOSAL 状态行摘要、正文链四件制品并摘 PROPOSAL §0 目标效果；② 或最小版：gov lane 下标题/正文不再写「N 个 Pod」，改写「gov 治理批 <changeId>（制品见 delivery/review/<changeId>/）」。

**风险**：riskTier: low——单脚本文面/提取分支，无行为面；G3 脚本，按 gov-arq 判据如实标 review profile = Track B（G3 触机器面，不走轻路径美化）。
