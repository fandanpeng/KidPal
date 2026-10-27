---
id: ARQ-261027-publish-diverged-shell-reconcile
title: release-pr --publish 每逢上批 PR 合并壳必报 diverged 拦交人裁——壳 SHA 与本地追平提交恒不同、内容差恒零，人裁答案恒同，应自动化或文面免问
domain: gov
status: deferred
severity: SHOULD
owner: 主线程（脚本面改动归 dev/gov 批）
trigger: 下次 --publish 报 diverged 时并批消费；或下次动 `release-pr.mjs` 预检逻辑时顺手
opened: 2026-10-27
closed: ""
closedBy: ""
paths: [".claude/scripts/release-pr.mjs", ".claude/skills/merge-to-main/SKILL.md"]
source: ["memory: publish-needs-sync-of-previous-pr-merge（PR #44 实例，2026-10-26）", "PR #45 实例（2026-10-27，P7 发布首跑）", "delivery/workstreams/gov-debt-reduction/task_plan.md P7 全环闭环行"]
---

**证据**：两次实例同型——① P6.5 发布（PR #44 壳 `dc1d8826`，2026-10-26）：`--publish` 报 diverged，人裁后本地 no-ff 追平 + 复跑过；② P7 发布（PR #45 壳 `eca8ac14`，2026-10-27）：同款 diverged 拦，再次 AskUserQuestion 人裁，again no-ff 追平 + INDEX 全量重建 + 复跑过。根因：human 在 GitHub 网页合并 PR 产生的 merge 壳 SHA 与本地任何追平提交恒不同（父 SHA 不同），而壳内容恒为本地已在场的批内容 → 预检判 diverged 恒真，人裁答案恒同（「核零内容差 → no-ff 追平 → 重建生成件 → 复跑」）。**human 2026-10-27 明示：没必要每次都问。**

**影响面**：每次远程发布（凡上批开过 PR 且已合并）都要弹一次人裁问询——高频、答案恒定、纯机械。

**建议改动**（交 review、不自批，二选一或并做）：① **脚本自动化**——`release-pr.mjs` 预检命中 diverged 时自动核「origin/main 相对本地 main 零新增内容」（diff 仅含本地改动反向 + 生成件行差）→ 自动 no-ff merge origin/main + `changes-index.mjs` 全量重建 + `debt-render.mjs` + 复跑；核不干净（origin 侧有真实新增内容）才升级 human。② **文面免问**——merge-to-main SKILL.md 发布节加一段「PR 壳 diverged = 常态，按既定链机械处置 + 事后报告，不再 AskUserQuestion」（沉淀 memory 与两次实例为授权依据）。① 治本但触脚本面（恒 Track B），② 零成本但依赖 memory/文面被读到。human 倾向治本（原话「没必要每次都问」），批内可先落 ② 兜底、① 另走脚本批。

**风险**：riskTier: medium——①动发布预检判定逻辑（承重自动化面），须防「零内容差」误判放行（核差逻辑要严：任何 origin 侧非生成件新增行即升级）；②纯文面 low。
