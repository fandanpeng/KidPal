---
id: TECH-261019-impl-prose-ledger-path-drift
title: 实现面与历史快照 10 处散文仍引 docs/ADR.md、ARCH-REVIEW-QUEUE.md 等已迁/已删路径
domain: tech
status: deferred
severity: NIT
owner: dev-loop
trigger: 下个触及该文件的 dev-loop 包顺手改指（v3–v6 prompt 四件与 showme 快照永久豁免，见正文）
opened: 2026-10-19
paths: [server/deploy/openclaw-system-prompt.v3.md, server/deploy/openclaw-system-prompt.v4.md, server/deploy/openclaw-system-prompt.v5.md, server/deploy/openclaw-system-prompt.v6.md, server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER, server/deploy/README.md, server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py, server/acceptance/test_schedule_grid_restore.acceptance.py, tools/asset-pipeline/tests/test_art_assets.acceptance.md, notes/showme/show-me-gov-mechanism-growth.html]
source: [delivery/review/261019-gov-review-docs-root-ledger-rehome/]
handoff: 下个触该文件的 dev-loop 包顺手改指（散文零机械耦合，不值独立包）
---

**证据**：261019-gov-review-docs-root-ledger-rehome 批（docs 根瘦身）删除 `docs/ARCH-REVIEW-QUEUE.md` 与 `docs/CONTRACT-DECISION-QUEUE.md`、`docs/ADR.md` 迁 `docs/decisions/ADR.md`；下列 10 处散文/docstring 仍引旧路径（该批 §3 census 实现面行实测枚举）：`server/deploy/openclaw-system-prompt.v3.md` / `.v4.md` / `.v5.md` / `.v6.md` 各 1 处、`server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER:50`、`server/deploy/README.md:51/:81`、`server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py`、`server/acceptance/test_schedule_grid_restore.acceptance.py`、`tools/asset-pipeline/tests/test_art_assets.acceptance.md:517`、`notes/showme/show-me-gov-mechanism-growth.html:191`。零机械耦合实证：该批 census 以 `grep -E "existsSync|Path(|open(|read_text|readFile"` 配 ADR/QUEUE 字面扫实现面，零命中——全是散文提及，无文件读取断言。

**影响面**：读者按旧路径扑空（文件已删/已迁），兜底 = `docs/debt/README.md` 迁址记 + 按 ID grep。零行为面影响（无代码消费这些路径）。

**建议改动**：下个触及该文件的 dev-loop 包顺手把散文改指新址（`docs/ARCH-REVIEW-QUEUE.md` → 按条目 ID 指 `docs/debt/<域>/<ID>.md`；`docs/CONTRACT-DECISION-QUEUE.md` → `docs/debt/contract/`；`docs/ADR.md` → `docs/decisions/ADR.md`）。**永久豁免两件**：① `server/deploy/openclaw-system-prompt.v3–v6.md` 四件不改指——v5/v6 受 sha256 冻结验收断言钉死（改一个字节即破断言，机械冻结；`.gitattributes:32` 注记实钉 raw 集 = v6–v9），v3/v4 无任何 hash 验收断言、受 E2「历史版本零回改」铁律纪律冻结；② `notes/showme/show-me-gov-mechanism-growth.html:191`——一次性历史快照（时间线 chip 是历史事实陈述），改词 = 改写历史。

**riskTier: low**（纯散文漂移、零机械耦合；豁免面已由冻结断言与铁律机械/纪律兜底）。
