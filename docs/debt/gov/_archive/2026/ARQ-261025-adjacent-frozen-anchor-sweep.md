---
id: ARQ-261025-adjacent-frozen-anchor-sweep
title: intake / 1g 不扫既有冻结件的相邻字面锚——跨包冻结冲突延迟到 builder 首轮才暴露，需 human 窄口径重锚
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（动 `task-intake-and-slicing.md` / `planner.md` / `dev-loop.md` 的分面时并批）；或下一个触既有冻结件同一实现点的包
opened: 2026-10-25
closed: 2026-10-27
closedBy: 261027-gov-review-p7-devloop-doc-cluster
paths: [".claude/references/task-intake-and-slicing.md", ".claude/agents/planner.md", ".claude/references/dev-loop.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261018-sd-log-persist/pod-retro.md, delivery/taskpackages/261022-m5-k4-history-corpus-backfill/pod-retro.md, delivery/taskpackages/261016-p3-story-body-sd-cache/pod-retro.md]
---

**证据**：① `261018-sd-log-persist`：intake 只核了本包 changeManifest 与 scope-map，未扫既有冻结件里的相邻字面锚；`261016-sd-card-mount` 的 `sd_card_mount.acceptance.c` 钉 `max_files=5`，本包要 8，builder 首轮 host 全量才暴露（pod-events friction `scan-miss`）。改既有冻结断言超出 tighten-only 窗口，需 human 窄口径批准重锚（`ARQ-260908-tighten-only-unwritten` 只写 additive，未覆盖跨包重锚）。retro 原话：「这不是谁写错了，而是 intake 缺少一个『相邻冻结齿字面锚』的扫查动作」。② `261022-m5-k4-history-corpus-backfill`：起包逐字段对磁盘核验，才发现 `docs/decisions/ADR.md` ADR-016 已把 `K4-E2` 的措辞从「设备级 peer」改成「设备级 session 键族 + 家庭单 owner」；若不核，builder 会照旧口径实现。③ `261016-p3-story-body-sd-cache`：覆盖层 chrome 面不在任何冻结齿里，integrator 五轴 review 才抓住 BLOCKING。④ 正向对照 `261018-sd-log-usb-read`：消化 persist 教训、起包前扫相邻锚，零 fixer 轮。

**影响面**：凡改既有实现点、且同一点已被别的包冻结齿钉过的包。冲突暴露时点从 intake 推迟到 builder 首轮或 verifier 终审，一次至少一轮返工；跨包重锚还要 human 出场。本波 4 包命中，其中 2 包直接付出轮次。

**建议改动**（交 review、不自批）：intake / 1g 增加一条机械可判的扫查动作——用本包 changeManifest 的目标符号/常量在 `**/*.acceptance.*` 与既有冻结件里做字面 grep，命中即把「相邻冻结锚清单 + 冲突处置（沿用 / 重锚 / human 裁）」写进 plan 或 taskpackage notes；扫查结果空也显式记「零命中」。不新增门、不改任何闸判据，只补起包检查项。

**风险**：riskTier: low——纯起包检查项与文面，不触闸判定逻辑。

**结项追注（2026-10-27，261027-gov-review-p7-devloop-doc-cluster T1）**：修法已落地——task-intake-and-slicing.md 新增「相邻冻结锚扫查（起包自查 · changeManifest 定稿后）」节（扫查面 = `**/*.acceptance.*` 与既有冻结断言、命中记清单 + 沿用/重锚/human 裁三档处置、空也显式记「零命中」、与 planner 1f 分界段在场防双头），dev-loop.md changeManifest 条目 append 入口句。R1 GREEN、落地五件 sha256 对账一致。
