# docs-delta —— 261014-voice-round-entry-exit-fixes

> blocking closeout 制品。integrator 落笔（2026-10-14 制品轮）。本包 = Full 单包（rigor=full、sourceType=regression、riskTier=medium）：三条 TECH-261014 技术债合流一次性修复——① 唤醒词 connecting 态事件被合并跳过致 VOICE 层不推入（语音页整轮不显示）；② 查字/提醒播报后设备滞留聆听态（tts stop 出口缺口）；③ engine-patch `_kidpal_try_speak` 对同步 `speak_txt` 误 await（每轮提醒必报 ERROR「播报注入失败」）。技术方案权威 = sibling `plan.md`（architect `/triage-change` 分流 review 2026-10-14，fork② 真分叉已裁设备侧）。

## 文档面改动清单

**本批零文档面改动**（taskpackage `docsSyncRequired=false`，docs-sync 闸 r1 scoped + r3 final 两轮 PASS 承载）——plan.md「不动（显式零触碰面）」逐条对账：

- `docs/requirements.md`（需求面）：**零 diff**。修法全部兑现**既有**验收口径，不新增、不修订任何验收条目：唤醒词入口直推回引 S-F1.4 现文与 `requirements.md:39` 连续追问保护、`:295/:300` 入口直推语义；tts 出口查字收尾回引 `:652`（v1.37「播完会话即结束、不回聆听态」）；提醒弹窗驻留与提示音不受扰回引 S-F1.4-AC1 / S-F1.4-AC6。spec 面漂移检出 = 无，故无 architect spec 更新交接（`ARQ-260929-docs-sync-spec-handoff` 条款未触发）。
- `docs/architecture.md`（架构面）：**零 diff**。本批为单函数会话状态分发行为修复（application.cc 入口/出口接线 + 采样），零结构性架构变更、零新增模块、零 wire 协议变更。
- `docs/contracts/**`（契约面）：**零 diff**（`git status` 实证 docs/ 下无任何改动文件；contract-lint 闸 r1/r3 两轮 PASS）。fork② 裁设备侧的直接推论 = wire 协议零变更（不新增、不修改任何引擎↔设备消息）；`crossEnd` 的提醒语音链两端（engine `kidpal_speak` 下行 / 设备 tts stop 收尾）各自闭环、不触共享 wire 形状——契约变更门不触发。
- `docs/contracts/interface-contract.md` 仅作 `contractRefs` canary 引用（在册），本批零触碰。

### 代码面 doc 钉行（随包在案、非本件义务）

- `firmware/main/application.cc`：新增注释块随实现同 PR 原子落地（A″ 直推块 + tts stop 三守卫块 + sentence_start 采样块 + 文件级静态说明头注），零 doc 漂移。
- `server/deploy/engine-patch/connection.py`：`_kidpal_try_speak` 内同步调用注释（注明 `speak_txt` 为引擎原生同步函数、不可 await）随实现同 PR 落地。

## TECH CRQ 登记状态（如实申报、不翻转）

三条源 CRQ——`docs/crq/tech/TECH-261014-wakeword-connecting-voice-layer` / `TECH-261014-post-speak-listening-lingering` / `TECH-261014-kidpal-speak-await-nonetype`——**本批零触碰**（`git status` docs/crq/ 无改动）。taskpackage `docsSyncReason` 明载：三条目**闭账条件 = 包 merge + 真机复验轮**，两者均不在本轮范围（human 裁执行边界③④）——CRQ 状态翻转与闭账留痕留待主线程联测轮处理，非本 Pod 义务。**本 integrator 按 scope 写权不翻转 CRQ 状态**。

## 零改申报（其余面）

- `docs/crq/**`：零触碰（见上）。
- `.claude/**`、`tools/**`：零触碰。
- `docs/requirements.md` / `docs/architecture.md`：零 diff（本包断言，exitCriteria「wire 协议零变更」承载）。
- ADR：**零新增**（`adrRequired=false`——A″ 入口直推 + tts stop 三守卫 + 轮来源采样 + 同步调用修正为单文件会话状态分发行为修复，非契约/跨系统边界裁决，不构成 ADR 事由；`speaking_round_entered_from_` 文件级静态载体裁决属 planner 边界内裁定、随 taskpackage `notes` 留痕，非 ADR）。

## Post-cutoff 制品纪律确认

本批 post-EVENTS_SCHEMA_CUTOFF（changeId 前缀 261014 >= 250725）：**不产** `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner 收尾相位的 `pod-retro.md`（未到该相位、不在本批）。`delivery/taskpackages/261014-voice-round-entry-exit-fixes/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write，含 r1 scoped + r2 final 红 + r3 final 绿）；`delivery/taskpackages/261014-voice-round-entry-exit-fixes/pod-events.jsonl` 在场（advisory 检查归 planner/supervisor、non-blocking，本件不阻塞 closeout）。机器证据投影 = `verdict.json` + `verification.json`（advisory，non-bearing；承重机器件 = `verifier-evidence/rounds/r3.json`）。
