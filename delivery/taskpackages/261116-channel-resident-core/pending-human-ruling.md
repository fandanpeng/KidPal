# pending-human-ruling · 261116-channel-resident-core

> 非交互收尾的 fail-closed 载体（planner 收尾相位）。本包离线面已全部完成并 commit，**停在合并门**；以下两项需 human 裁决，未裁前不推进、不合并、不结项。

## 当前状态（已完成、可复核）

- 冻结验收 15 条（SCR-A1–A7 + SCRF-B1/B2/B2b/B3/B4/B4b/B5/B6）全绿；mutation 15 条逐条敏感且还原有 sha1 证据。
- verifier 机械证据：`verifier-evidence/rounds/r1.json`（scoped 8 闸 ok）、`r2.json`（final 10 闸 ok，含 `VERIFY_FW_BUILD_REQUIRED=1` 真编译）；`verifier-evidence-check` exit 0。
- 实跑：host `306 Tests / 3 Failures`（3 红 = 上游存量 `p1_auxbar_style` AB-A1/A2/A4，零新增红）；server `473 passed / 0 failed`。
- closeout：`docs-delta.md`、`review-packet.md`、`pod-retro.md` 齐；`closeout-check.mjs` exit 0；dev INDEX 已补行。
- 提交栈（本地 `dev/pool-channel`，**未 push、未 merge**）：`8a7508ad` → `cc6f6667` → `9a9a75f4` → `ad22dc20` → `23a2fe7d`。

## 待裁一：合并门 ESCALATE + 真机轮 A

- 机械 supervisor verdict = **ESCALATE**（declared riskTier high；`supervisor-verdict.json`）。
- 前置升级闸 BLOCK，残两项：
  1. `device-round-evidence-missing` —— 需 human 持板跑真机轮 A（A1–A13，判据见 `taskpackage.exitCriteria.KRC-E4` 与本包 `plan.md`）；跑完把设备 KPLOG / front journalctl / 引擎 docker 三源交回，由 planner 落 `device-round-evidence.md`（`sourceCommit` = 合并 HEAD、`binarySha256` 必填）。
  2. `riskTier=high（supervisor 升级）` —— 需 human 对 ESCALATE 明确批准合并；planner 落 `human-decision`（`payload.decision="approve-escalated-merge"`）后走 `merge-driver --pod <changeId> --human-approval-ref <eventId>`。**人批不豁免真机证据**，第 1 项仍须闭合。
- 备注：launch-card 的 hostUncompilablePaths 已由 planner 收窄为已登记的 `application.cc`（原含 `application.h`，报 `device-outcome-host-path-unregistered`）；用户可见结果与轮 A 判据零弱化。

## 待裁二：G4 工具缺口（阻断会话收尾）

- `supervisor-gate-check.mjs` 的 G4（`optional:false`，无豁免）要求 `.claude/verify-logs/*.log` 非空；但 `verify.mjs` 仅在 gate 失败时写日志（`verify.mjs:288-290` 在 `if (!ok)` 内）。本包两轮全绿 → 目录不存在 → **G4 结构性恒红 → 会话无法正常收尾**。
- 诊断、复现与可直接套用的 patch 见 `gate-tooling-gap-g4.md`。
- 供裁决的两个选项：
  - **A（推荐）**：主线程 / `/gov-change` 把 `writeGateLog` 移出 `if (!ok)`（每闸每轮落 log）；planner 随后重跑一个 scoped 新轮 + `supervisor-gate-check`，G4 自然清。
  - **B**：human 明示「接受本包在 G4 常红下结束本次会话」；planner 落 `human-decision` 留痕后停。**B 不放行合并**（合并仍受「待裁一」约束）。

## 非裁项（已按纪律处置，仅备案）

- 两条 debt（`TECH-261114` / `TECH-261012`）按 workstream PROPOSAL「结项凭据必须是真机」维持 `deferred`，本轮只刷 `trigger` + 追加收口注记；真机轮 A 通过后由主线程改 `closed`/`closedBy` 并 `debt-render.mjs` 归位。
- `docs/requirements.md:48` 的 60s 对账 ⚠️ 注记：事实前提已由本包 SCR-A5 兑现，但刷新归 workstream 留根 1（主线程路由），本包对 requirements 零 diff。