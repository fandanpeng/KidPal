# docs-delta · 261116-channel-resident-core

- **docsSyncRequired**：`false`（TaskPackage 声明；`docs-sync` 闸在 final r2 复跑 PASS、修复轮 final r5 复跑 PASS）
- **docsSyncReason**（逐字）：会话/通道解耦与保活机制已由 architecture §6.4 + ADR-019 + 契约 §四冻结；本包只兑现实现，不改 spec 语义。requirements:48 ⚠️ 对账注记按 workstream 留根 1 由主线程路由。
- **docsOwner**：integrator
- **ADR**：`adrRequired=false`，本包未产出新 ADR；`docs/decisions/ADR.md` 零变更（引用既有 ADR-019 / ADR-015，不改文本）。

## 结论：canonical docs 零变更（理由）

本包是纯实现兑现——会话/通道解耦与保活机制在拆包前已由 FROZEN workstream PROPOSAL + architecture §6.4 + ADR-019 + 契约 §四冻结。核实（`git diff --stat HEAD -- docs/requirements.md docs/architecture.md docs/contracts/ docs/decisions/` 为空）：

1. **需求面 `docs/requirements.md`**：§2.1（结束三通道：60s 静默 / 结束意图 / 手动关闭）与 §2.3（录音态按键口径）已冻结。本包 T2 的 60s 无语音超时 producer（SCR-A5，命名常量 `kSessionIdleTimeoutS = 60` + 记录语境豁免）与四条本地关收口（SCR-A1）逐条兑现冻结条文，未新增或改写需求语义。
2. **架构面 `docs/architecture.md`**：§6.4（通道常驻，五真拆通道点闭集，本地关不再拆通道）、§5.1.1（60s producer 落主循环 tick）、§5.1.2（front `listen stop` 分层收尾 + pending/sweep 释放）已冻结。本包 T1/T4/T5–T9 逐条对齐，零漂移。
3. **契约面 `docs/contracts/interface-contract.md`**：§四 `kidpal_keepalive`（front 周期下行裸心跳，非 display 帧）与 `kidpal_session_ctx` / `listen` / `kidpal_story_discard` 已冻结。本包 T3（设备 no-op 消化）与 T5–T8（front 心跳 / 分层收尾 / ctx 分态）均为这些已 LANDED 契约的实现落地，**契约文本零改动**（integrator 对契约面只读；本包无 `CONTRACT_CHANGE_REQUIRED` / `SPEC_CONTRACT_DRIFT` 触发）。
4. **ADR 面**：`docs/decisions/ADR.md` 零变更，引用既有 ADR-019（会话/通道生命周期解耦）与 ADR-015（持久会话）。

## 账本条目处置（`docs/debt/tech/**`，在本包写槽内）

按 workstream PROPOSAL §6「debt 结项凭据必须是真机、不是合入」（原句见 `delivery/workstreams/261115-session-channel-resident/PROPOSAL.md:101` 与 `:112`），本轮**不结项**两条候选（真机轮 A 尚未发生，本包只有离线证据）：

| 条目 | 与本包关系 | 本轮处置 | 结项判据（待真机轮 A） |
|---|---|---|---|
| `TECH-261114-overlay-close-channel-rebuild-churn` | 本包 T1（候选 A「close 不断通道」） | 追加「收口注记」+ 刷新 `trigger` 指向真机验证；`status` 维持 `deferred`、`closed`/`closedBy` 留空 | A1 温通道按键进聆听 ≤0.82s 对照 KPLOG029；A2 连续 5 次「结束→再说话」零冷通道零重建 burst |
| `TECH-261012-first-keypress-no-listening` | 本包 T5/T6（front 心跳保活）+ T4（设备自愈巡检） | 追加「收口注记」+ 刷新 `trigger` 指向真机验证；`status` 维持 `deferred`、`closed`/`closedBy` 留空 | A3 闲置 >5 分钟首按键一次进聆听、无重连无「按了没反应」 |

**为什么选「待验证」分支而非直接结项**：两条目的成本证据与收益都是**真机现象**（KPLOG029 实测 / 2026-10-12 真机复现），完成凭据必须是真机目击，不能凭合入或离线绿宣布。**为何不写 `status: verifying`**：债务账 status 是机检闭集 `{blocker, deferred, accepted-risk, closed}`（`.claude/scripts/debt-ledger-shared.mjs:28`）；写闭集外的 `verifying` 会在 `debt-doctor` 报 `bad-status` RED、且该条目从 `INDEX.md` 渲染中消失。故取「等价」= 维持 `deferred`（仍属未结，留在「待触发」节），仅把 `trigger` 刷新为「待 P1 真机轮 A 凭据」，并在正文追加收口注记记录实现已 land + 离线证据。收口归位（真机后改 `closed`/`closedBy` + `debt-render.mjs` 归位）归主线程。

**渲染件同步**：`docs/debt/tech/INDEX.md` 由 `node .claude/scripts/debt-render.mjs` 重新生成（`trigger` 列随条目变化）；`debt-render.mjs --check` 退出 0（无待重渲染漂移），`debt-doctor` 的存量 5 条 RED 与本次两条无关（均为其它文件 `open`/`resolved` 出闭集，改前基线即存在）。

## requirements:48 ⚠️ 对账注记（不计入本包 doc 变更）

`docs/requirements.md:48` 的「60 秒超时实现侧对账」⚠️ 注记（2026-09-14 登记，记「设备侧无 timer 生产者、真机 60s 自动关闭从未发生」）在 P1 T2 落地后其事实前提已被兑现——本包新增了唯一 60s producer（SCR-A5）。但该注记的刷新属 workstream 留根 1（`PROPOSAL.md:105`：实现包 integrator docs-delta 或需求面对账小批，**主线程路由**），**不在本包自行改写 requirements**：本包对 `docs/requirements.md` 保持零 diff，刷新时点与载体由主线程在列车收口时决定。本行仅作对账留痕。

## 影响面记（供 reviewer 对账）

- 本次 docs-sync 判定与 `docs-sync` 治理闸 PASS 一致（闸读 `docsSyncRequired=false`，本文件给人工复核理由）。
- 本包对 `docs/` 的实际改动仅 `docs/debt/tech/**` 三件（两条目 + 其生成 INDEX），属账本记录面、非 spec/契约/架构/需求语义变更。
- 无 UIUX 工作面（`uiux` 未触发），无设计稿同步需求。

## 修复轮增记（真机轮 A 修复轮，2026-11-17/18）

- **canonical docs 本轮零改动**：`git diff --stat HEAD -- docs/ docs/contracts/` 为空（spec 两件、契约面、ADR 面、debt 账本面均零 diff）。修复轮（D1/D2/D3）全部落在实现与验收面：`server/gateway/front/front_app.py`、`firmware/main/application.cc`、`server/acceptance/test_session_channel_resident.acceptance.py`、`firmware/main/test/session_channel_resident.acceptance.c`、`server/acceptance/test_record_dialogue_guide.acceptance.py`（RDG-A4 显式重锚）。
- **新增包内证据件**（非 canonical docs、不触发 docs-sync）：
  - `device-round-a-evidence.md`（真机轮 A 三源证据与 D1/D2/D3 诊断；轮 B 复测清单种子）
  - `device-evidence/round-a/kplog-excerpt.txt` / `front-journal-excerpt.txt` / `engine-docker-excerpt.txt`（最小脱敏摘录 + SHA-256，见 review-packet 设备轮证据节）
  - `mutation-evidence/round-a-fix.md`（19 条变异表 + 4 条如实观察项）
  - `verifier-evidence/rounds/r3.json` / `r4.json` / `r5.json`（承重轮 fact；r3 = ok:false 事实占序）
- **两条 debt 维持 deferred**：`TECH-261114-overlay-close-channel-rebuild-churn`、`TECH-261012-first-keypress-no-listening` 本轮零改账本文件；结项凭据仍是真机轮 B 目击（轮 A 为诊断轮且 FAILED，不构成结项凭据）。
- **spec 漂移申报（不由本 Pod 改 spec）**：D3 对 requirements §2.1「记录会话不适用 60s 会话超时」的逐字冻结构成**收窄**——新增语义 = 空素材记录会话由设备侧 60s 收尾（释放、不归档、不推 story_list、不盖章），而 AC5 只覆盖「有素材 + 2 轮引导无回应」。`docsSyncRequired=false` 的现值与该实际影响不一致；本包对 `docs/requirements.md` 保持零 diff，**交主线程按需求面轻量注记处理**（§2.1 记录会话段补对账句，或 §7 开放问题追加），不写成「已同步」。