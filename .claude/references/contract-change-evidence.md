# Contract Change Evidence Reference（261015-gov-review-face-loop-unify T17 收窄版）

> Use for every contract change. **契约变更 = 架构变更的一种**：走 `/arch-change` loop，证据由 loop 标准制品承载；独立双制品（`CONTRACT-CHANGE-PACKET.md` / `architecture-gate-evidence.md`）不再作为新批要求（历史包不动、仍有效作证据）。

## 契约变更的证据义务（并入 loop 制品）

契约变更批（`/arch-change`）的 PROPOSAL 改法集必须含**契约包三节**：

1. **proposedContractDiff**：逐面列最小 diff（shimApi / displayBlock / strokeFormat / scheduleMD 四字段沿用 architect-reviewer 输出格式）。
2. **impact**：影响固件渲染 / OpenClaw 输出 / shim 的哪些消费方；是否破坏已跑通实测链路。
3. **validationPlan**：命令 + 退出码 + 预期（契约 diff 逐字人工核对 / JSON 样本回归如 makemeahanzi 笔顺 / 端云联调冒烟）。

外加标准 loop 制品：FROZEN PROPOSAL、§7 评审记录（architect-reviewer）、landing-manifest（必须覆盖全部被 diff 的契约路径）、landing-verdict PASS——这四件同时是 `pre-merge-escalation-gate.mjs` 机械凭据闸的联立凭据。

## Evidence Integrity Rules（保留，不变）

- Every factual claim about contract files must be grep-able or validated by a command: model counts, enum counts, line references, `DROP TABLE`, indexes, generated delegates, event/tool names.
- Raw evidence belongs under `evidence/` (in the change folder); summaries link to it.
- If a migration claim concerns an existing database, include data-safety evidence.
- `APPROVED` without a machine-verifiable anchor is invalid——契约批的批准锚 = reviewer verdict + landing-verdict，不收聊天结论。
- **Batch gatekeeper whitelist sync**：human-approved 时代条款——若后续仍存在扫描 `docs/contracts/**` 的批级验收 gatekeeper，白名单同步义务不变（先例：CDQ-017 / commit c70e6cd）。

## Status Vocabulary（261015 批修订；261019 批起转历史对照——新条目直接用 debt 闭集 blocker / deferred / accepted-risk / closed，家 = `docs/debt/contract/`）

| Status | Meaning |
|---|---|
| `DRAFT` | 契约包三节起草中。 |
| `PRECHECKED` | architect-reviewer 分类已出（CDQ 条目登记，`docs/debt/contract/`）。 |
| `LANDED` | 契约 diff 随批落地、验证证据在场。 |
| `CLOSED` / `已结` | 凭据 + 验证 + docs/queue sync + closeout conditions 全部完成。 |

`HUMAN_APPROVED` / `CONDITIONAL-APPROVE` 退役（人类契约签字门撤销；CDQ 历史行保留不改——已随 261019 批迁入 `docs/debt/contract/` 条目，退役词按映射表归一 debt 闭集）。Never keep a pending status beside a contract diff that already shipped—retroactively evidence it or roll it back.

## Minimal Template（PROPOSAL 内契约包节骨架）

```md
### 契约包 · <面名>

**proposedContractDiff**：...
**impact**：...
**validationPlan**：...
```

CDQ 登记：每契约变更批一条目（`docs/debt/contract/<CDQ-id>.md`；机读字段 `fourClass` 五值 + `loopEvidence` = changeId + landing-verdict 结果摘要；原单文件队列 261019 批并入并删除）；权威 = `.claude/rules/contracts.md`。
