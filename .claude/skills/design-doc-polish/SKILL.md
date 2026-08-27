---
name: design-doc-polish
description: "Use when: polishing Chinese design docs, README, specs, architecture notes, governance/navigation docs, or review packets for smoother wording without changing decisions. Trigger phrases: 文档优化, 中文不通顺, 润色 README, polish docs, design doc wording, governance docs, navigation docs."
---

# Design Doc Polish

Use this skill to improve readability, navigation tone, and Chinese fluency in repository design documents without changing architecture decisions, contracts, or implementation facts.

## Core Rule

Polish wording, not meaning. Preserve authority boundaries, links, identifiers, tables, event names, model names, schema terms, and decisions unless the user explicitly asks for content changes.

Chinese-English mixing is acceptable. Do not translate stable domain terms just to make the prose look more Chinese. Keep terms like `AI agent`, `dev-loop`, `review loop`, `SSOT`, `owner`, `payload`, `consumer`, `humanGate`, `deep-link`, model names, event names, paths, and command names when they are the clearest labels.

## Workflow

1. Read the whole target section, and enough surrounding context to understand the document's role.
2. Classify the document's job before editing:
   - Navigation docs point to authority and should not duplicate decisions.
   - Specs define behavior and may carry normative language.
   - Review/change artifacts record evidence and should keep exact verdicts and references.
3. Identify awkward prose before patching:口语化、句子过长、主语缺失、中英混排不自然、导航文档里夹带裁决、重复解释、过度细节。
4. Make small semantic-preserving edits. Prefer clearer verbs and stable nouns over stylistic rewrites.
5. Validate with `git diff --check` and, when available, editor diagnostics for the edited Markdown.

## Style Guidelines

- Prefer direct Chinese sentences with clear subjects: `本仓是...`, `该机制用于...`, `权威内容见...`.
- For navigation docs, use pointer language: `入口`, `权威内容见`, `归档到`, `README 不复述条目`.
- Replace casual wording when it weakens authority:
  - `装` -> `承载`
  - `别乱建` -> `不要临时新建`
  - `长出消费者` -> `出现跨系统消费者`
  - `看链接` -> `权威内容见链接`
- Keep deliberate architecture vocabulary when it is the project language: `owner`, `act`, `awareness`, `SSOT`, `Tier-1`, `Tier-2`.
- Avoid turning tables into prose unless the table itself is unreadable.
- Do not remove useful emphasis markers like bold text around invariants.

## Navigation / Governance Docs

For governance, routing, README, or other navigation-first documents, optimize for the reader's next action before showing the internal model:

- Start with the reader task: use a quick-reference table such as `我想改... / 走什么流程 / 谁能写 / 谁批准 / 详见` before internal classifications.
- Put natural names before internal codes. If stable codes are needed, use meaningful prefixes and place them in parentheses, for example `强制执行层（G3）`.
- Use display names that explain the mechanism to humans; keep command names, paths, sourceType values, and other stable identifiers as links or parenthetical labels.
- Prefer this order for governance navigation: quick reference -> boundary/model -> classification system -> process details -> exceptions/debt.
- Put frequent or high-risk examples in the quick-reference table first. Give them their own section only when they have a separate process or substantial exceptions.
- Explain cross-domain boundaries by relationship and ownership only; do not copy another domain's fact list. Link to the authority instead.
- Make table columns serve the reader's decision: target / route / writer / approver / details usually beats author-centric categories.
- Keep stable engineering terms in English when they are identifiers, but use Chinese functional names for headings, table first columns, and navigation entry points.

## Guardrails

- Do not “fix” mixed Chinese-English terminology by translating every English term.
- Do not add new decisions, new debt lists, or new contract claims while polishing.
- Do not duplicate schema, event payloads, or model fields into navigation docs.
- Do not move files or change repository structure as part of wording polish.
- Do not widen the edit to unrelated docs unless the user asks for a broader pass.

## Output Expectations

In the final summary, state which document was polished, name the main wording classes changed, and report validation. If the edit intentionally leaves mixed Chinese-English terms, mention that this follows the project style rather than being an omission.