---
id: ARQ-261017-merge-gate-contract-evidence-ref
title: pre-merge-escalation-gate 的契约凭据取件用 `git show target(main):PROPOSAL` 而非 source——结构性不可满足，arch-lane 契约批恒 BLOCK（2026-10-17 实测，261015 T15 新增路径首次真实执行）
domain: gov
status: deferred
severity: SHOULD
hardExclusions: true
owner: architect
trigger: 下一治理批（动 `pre-merge-escalation-gate.mjs` / 契约凭据判据 / `/arch-change` landing 流程任一时并批）；或在 agent-lane 契约批再次被同一 BLOCK 挡住时立即升格
opened: 2026-10-17
closed: ""
closedBy: ""
paths: [.claude/scripts/pre-merge-escalation-gate.mjs]
source: [2026-10-17 主线程在 261017-arch-review-p3-story-body-sd-cache 合并前实跑闸的 BLOCK 输出（逐字见证据 2）, delivery/review/261017-arch-review-p3-story-body-sd-cache/（PROPOSAL FROZEN + landing-verdict PASS + architect-landing-summary）, delivery/review/261017-arch-review-p3-story-body-sd-cache/architect-landing-summary.md §3 第 3 条（落地时的预判留痕）]
---

**证据**：

1. **缺陷位置（逐字，`.claude/scripts/pre-merge-escalation-gate.mjs`）**：

   ```js
   // :425
   const source = sourceRef || revParse(execFn, 'HEAD');
   // :431
   const hasContractGateEvidence = hasArchLoopContractEvidence(execFn, source, 'main', projectDir);
   // :143（hasArchLoopContractEvidence 内）
   proposalText = execFn(`git show ${target}:${proposalPath}`);
   ```

   `target` 恒为字面量 `'main'`，而取 PROPOSAL 的 `git show` 用的是 `target`。**应用 `source`（或 source→target 回退）。**

2. **实测 BLOCK 输出（2026-10-17，逐字）**：

   ```
   [pre-merge-escalation-gate] BLOCK (changeId=261017-arch-review-p3-story-body-sd-cache, kind=gov):
     - docs/contracts/** diff 且同批无契约合并凭据（contract-evidence: 无候选满足三件联立（FROZEN + PASS verdict + manifest 覆盖）；契约路径 = docs/contracts/interface-contract.md；已检查 = [{"dir":"delivery/review/261017-arch-review-p3-story-body-sd-cache","frozen":false,"verdict":false,"covered":false}]）——… → 升级 human，不得自动 merge。
   ```

3. **三件凭据实际均在（假阴性直接证明）**：

   | 凭据 | 实况 | 佐证 |
   |---|---|---|
   | FROZEN PROPOSAL | 状态行 `> **状态：FROZEN v1.0 · 2026-10-17 · 主线程凭据自冻**…` | 盘上 + 已提交 `b3f50ebd` |
   | PASS landing-verdict | `landing-verdict.md` PASS（结构门完整模式 exit 0；T1–T6 逐字命中；canonical 超范围改动 0） | landing-verifier 独立核验 |
   | manifest 覆盖 | 三文件 ≡ §2 目标并集 ≡ 实际 canonical diff | landing-fidelity-check `ok:true` |

4. **根因复现（两条命令即可）**：

   ```
   git show main:delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md   → 失败
   git show HEAD:delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md   → 成功
   ```

5. **为何是「结构性」不可满足（本条的要点，不是偶发）**：候选 PROPOSAL 之所以成为候选，正因它出现在 `git diff --name-only ${source}..main` 的输出里（:137 的 `proposalPaths` 即从该 diff 捞取）。**出现在 source..main diff 里的文件，按定义就不在 main 上。** 因此 `git show main:<该文件>` **恒失败**，`proposalText` 恒 null → `frozen/manifest/covered` 三件连锁塌为 false → 恒 BLOCK。**该判据在当前实现下不存在任何可满足的输入**。

6. **历史（为何至今未暴露）**：该判据由 `261015-gov-review-face-loop-unify` 批 T15 新增，代码注释逐字「替换原 `hasContractGateEvidence: false` 硬编码（ARQ-260923 建议①兑现）」。此前 arch-lane 契约批的凭据载体在 `delivery/taskpackages/<id>/contract-gate/`（dev-lane 形态，如 `261010-p3-storylist-cold-start-restore`），走的是另一条分支；**本批（261017-arch-review-p3-story-body-sd-cache）是该新路径的首次真实执行**——首次执行即命中。

**影响面**：

- **凡走 `/arch-change` 触契约分支的批次，合并门恒 BLOCK**——而 261015 批起的现行口径正是「契约变更 = 架构变更的一种，走 `/arch-change`」。即**新口径下的每一个契约批都会撞**。
- **代价形状 = 假阴性 + 无豁免口**：三条凭据齐备仍被判「无凭据」，且 `--human-approval-ref` 走的是另一套核验（人批事件），不解决本判据；实际处置只能「升级 human + 主线程手工 `git -C <main-worktree> merge --ff-only`」，**该路径未成文**（与 `ARQ-261017-merge-gate-scope-unstated` 记录的处置实况同形）。
- **比 lane 白名单那条更隐蔽**：lane 白名单至少报 `unknown`（一眼看得出「闸不认识你」）；本条报的是「**缺凭据**」——读起来像落地方的过失，会把人的注意力引向错误方向（本批主线程即先自查了一遍三件凭据才定位到是脚本）。
- **复现频率**：每个 arch-lane 契约批一次；随新口径铺开而线性增长。

**建议改动**（候选，交 review、不自批）：

1. **取件 ref 修正（治本、一处改动）**：`:143` 的 `git show ${target}:${proposalPath}` 改为 `git show ${source}:${proposalPath}`（或 source 优先、失败回退 target）。理由：候选 PROPOSAL 由构造必然在 source 侧。
2. **加一条自检齿**：若 `proposalPaths` 非空而 `git show` 对**全部**候选都失败 → 报「取件 ref 疑似用错」而非「缺凭据」，把归因误导消掉（对齐本仓既有的「失败要可归因」取向）。
3. **处置路径成文**：把「假阴性 + human 放行 + 主线程手工 `git -C <main-worktree> merge --ff-only`」写成明文通道（与 `ARQ-261017-merge-gate-scope-unstated` 同批处置），或提供机读的 `--gate-bug-exemption <ARQ-id>` 一类显式豁免口，避免每次靠当场解释。

**风险**：riskTier: low——单文件判据改动 + 一处自检 + 文档成文，零行为面、零契约面。但**触 G3**（`.claude/scripts/**` 属治理机器面），故按 gov-arq 判据 **review profile = full-adversarial + human**，不美化快路径。硬排除六项：不触 `docs/contracts/**`、无 agent frontmatter、非 architect.md self-def、非 review-loop agent 定义、**非新增门**（改既有闸判据，不新增 gate）、无 spec-contract drift。

**路由**：归 `/gov-change` loop；paths 含 `.claude/scripts/**` = **机器面**，恒 Track B（轻路径不适用）。**关联**：`ARQ-261017-merge-gate-scope-unstated`（**同脚本、不同根**——该条治 lane 白名单覆盖面，本条治取件 ref；两条 paths 同含该闸，**建议同批评审**，且第 3 条建议（处置路径成文）两条可合并为一处落）。本条目由 `261017-arch-review-p3-story-body-sd-cache` 的合并期实测派生；该批经 human 2026-10-17 显式授权「假阴性 + 手工本地合并」处置，处置实况见该批 landing-summary §3 与合并提交 message。
