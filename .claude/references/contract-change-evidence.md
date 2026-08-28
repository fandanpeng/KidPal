# Contract Change Evidence Reference

> Use for every Tier-1 contract change, every retroactive contract gate, and every arch-review finding that routes to the contract gate.

`CONTRACT-CHANGE-PACKET.md` describes the proposed contract decision. It is not enough by itself. A contract gate is not reviewable until the package also has an evidence ledger that proves what was reviewed, what was approved, what changed, and what validation ran.

## Required Artifacts

Every contract-gated change must produce these artifacts in the owning change folder:

| Artifact | Required | Purpose |
|---|---:|---|
| `CONTRACT-CHANGE-PACKET.md` | Yes | Decision packet: scope, proposed diff, alternatives, impact, human question. |
| `architecture-gate-evidence.md` | Yes | Gate ledger: reviewer classification, human ruling, applied diff inventory, validation, closeout state. |
| `evidence/` | Yes when commands or audits ran | Raw logs and machine output. Summaries must point here instead of living only in chat. |
| `ADR-*.md` | When the decision changes durable policy or semantics | Records why, not the schema itself. |
| Queue update | Yes for nontrivial gates | `docs/ARCH-REVIEW-QUEUE.md` or `docs/CONTRACT-DECISION-QUEUE.md` records status and remaining conditions. |

Do not call the gate closed from chat alone. The evidence ledger must be in the repo.

## Evidence Ledger Shape

`architecture-gate-evidence.md` must include these sections:

1. **Gate Scope**: exact surfaces (`docs/contracts/interface-contract.md` 的具体小节), owners, and what is explicitly out of scope.
2. **Inputs Reviewed**: packet, specs, review findings, implementation evidence, prior queue rows, raw logs.
3. **Architect-Reviewer Classification**: one row per change surface using `NO_CONTRACT_CHANGE`, `IMPLEMENTATION_ADAPTER`, `SPEC_CONTRACT_DRIFT`, or `CONTRACT_CHANGE_REQUIRED`, with rationale and authoritative refs.
4. **Human / Architecture Decision**: who ruled, date, status, conditions, and exact question answered. `APPROVED` means human gate approved; otherwise use `CONDITIONAL-APPROVE`, `PENDING`, or `REJECTED`.
5. **Applied Diff Inventory**: each touched file, symbol/model/tool/event/type, additive vs breaking, migration/data risk, docs sync needed — plus batch-level contract gatekeeper sync: whether the approved diff touches any path tracked by an active batch-level structural gatekeeper (an acceptance test that scans `docs/contracts/**` diffs against a workflow baseline and whitelists human-approved exceptions, e.g. `SANCTIONED_CONTRACT_CHANGES` in s1-nsa-integration-e2e S5-A2), and which whitelist entries were added (see Evidence Integrity Rules).
6. **Closeout Conditions**: C1/C2 style conditions with owner, status, and required evidence. No condition means none.
7. **Validation Evidence**: commands, exit status, expected result, actual result, raw log path. Include contract-diff re-read, JSON sample regression (e.g. makemeahanzi stroke JSON), or end-to-end smoke as applicable.
8. **Docs / Queue Sync**: which specs, ADRs, CDQ/ARQ rows, and handoff docs were updated or explicitly unchanged.
9. **Rollback / Follow-up**: what to revert if the gate fails, and which follow-up is deferred.
10. **Verdict**: closed or not closed; whether dev may implement against it; whether the queue row may move to `已结`.

## Status Vocabulary

Use these words consistently:

| Status | Meaning |
|---|---|
| `DRAFT` | Packet exists, no architect-reviewer classification yet. |
| `PRECHECKED` | Architect-reviewer classification exists, human has not ruled. |
| `HUMAN_APPROVED` | Human approved the contract decision, implementation may land after validation. |
| `CONDITIONAL-APPROVE` | Direction approved, but named closeout conditions must finish before `CLOSED`. |
| `LANDED` | Contract diff applied, validation evidence present, but not necessarily closed. |
| `CLOSED` / `已结` | Human gate, validation, docs/queue sync, and all closeout conditions are complete. |

Never keep `PENDING` beside a contract diff that already shipped. Either retroactively gate it with evidence or roll it back.

## Evidence Integrity Rules

- Every factual claim about contract files must be grep-able or validated by a command: model counts, enum counts, line references, `DROP TABLE`, indexes, generated delegates, event/tool names.
- Raw evidence belongs under `evidence/`; summaries link to it.
- If a migration claim concerns an existing database, include data-safety evidence (`migrate status`, duplicate checks, teardown decision, or `new provisioning only`).
- If a Tier-2 change is allowed without a human gate, the ledger still records why it is Tier-2 and which clone/owner may write it.
- `APPROVED` without human decision text/date is invalid. Use `PRECHECKED` or `CONDITIONAL-APPROVE` instead.
- **Batch gatekeeper whitelist sync**: when a human-approved contract diff lands while a batch-level structural gatekeeper tracks `docs/contracts/**` (acceptance tests whitelisting sanctioned contract changes), add the whitelist entry in the same landing batch — human gate approval is the entry condition, and a missed sync blocks the next merge or full acceptance run with a pre-existing red (precedent: CDQ-017 gate closed 2026-08-18 without the sync; TP0 gate-4 red 2026-08-20 forced a one-off retroactive entry, commit c70e6cd). Record the gatekeeper census and the sync result in ledger §5/§8.

## Minimal Template

```md
# Architecture Gate Evidence · <ID>

> Gate: <ARQ/CDQ/change id>
> Date: <YYYY-MM-DD>
> Status: <DRAFT|PRECHECKED|HUMAN_APPROVED|CONDITIONAL-APPROVE|LANDED|CLOSED>

## 1. Gate Scope

## 2. Inputs Reviewed

## 3. Architect-Reviewer Classification

## 4. Human / Architecture Decision

## 5. Applied Diff Inventory

## 6. Closeout Conditions

## 7. Validation Evidence

## 8. Docs / Queue Sync

## 9. Rollback / Follow-up

## 10. Verdict
```