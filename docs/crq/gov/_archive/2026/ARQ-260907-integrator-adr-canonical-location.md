---
id: ARQ-260907-integrator-adr-canonical-location
title: enforce-scope 拦 integrator 写 docs/notes/，ADR 的 canonical 落点未给 integrator 明确答案（现状只能落 changes/dev/）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

human OQ3 自裁「放 docs 根目录、和 spec 同级」→ 新增 docs/ADR.md 单文件决策日志（append-only 条目格式在文件头、零条目预填）；scope-map docsTopLevel + integrator allow 写槽 + repo-structure docs 根白名单 + DEV-GOVERNANCE §7 入口速查 ADR 行，双向锚定一致（scope-map JSON parse 有效）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。
