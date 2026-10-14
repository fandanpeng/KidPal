# docs-delta —— 261014-voice-round-entry-exit-fixes

> blocking closeout 制品。integrator 落笔（2026-10-14 主轮制品轮；2026-10-14 补充轮 261014-r2 收口更新）。本包 = 单包（主轮 rigor=full → 补充轮 rigor=lite，human 裁定）、sourceType=regression、riskTier=medium：语音轮入口/出口缺口修复——① 唤醒词 connecting 态事件被合并跳过致 VOICE 层不推入（语音页整轮不显示）；② 查字/提醒播报后设备滞留聆听态（tts stop 出口缺口）；③ engine-patch `_kidpal_try_speak` 对同步 `speak_txt` 误 await（每轮提醒必报 ERROR）。**补充轮 261014-r2**（真机首轮联测后立）= 两个正交根因返修：A1（主因）= `application.cc` sentence_start 轮来源采样逐句覆写致多句对话轮误回 Idle（采样写点加 `!= kDeviceStateSpeaking` 守卫）；A2（次因）= `websocket_protocol.cc` 冷通道 `OpenAudioChannel()` 裸重赋值析构旧 WebSocket 致 stale `OnDisconnected` 迟到打回 Idle（函数顶部、`local_close_pending_ = false` 之前先按本地关分类拆除旧通道）。技术方案权威 = sibling `plan.md`（主轮节 + 「补充轮 261014-r2」节，architect `/triage-change` 二次 review 2026-10-14，fork② 真分叉已裁设备侧）。

## 文档面改动清单

**两轮累计零文档面改动**（taskpackage `docsSyncRequired=false`）——docs-sync 闸 r1 scoped / r2 final / r3 final / r4 scoped / r5 final **五轮全 PASS** 承载。plan.md「不动（显式零触碰面）」主轮节 + 补充轮「明确不做（残余登记，不扩本包）」节逐条对账：

- `docs/requirements.md`（需求面）：**零 diff**。两轮修法全部兑现**既有**验收口径，不新增、不修订任何验收条目——主轮唤醒词入口直推回引 S-F1.4 现文与 `requirements.md:39` 连续追问保护、`:295/:300` 入口直推语义；tts 出口查字收尾回引 `:652`（v1.37「播完会话即结束、不回聆听态」）；提醒弹窗驻留与提示音不受扰回引 S-F1.4-AC1 / S-F1.4-AC6。补充轮 A1/A2 亦**纯实现既有口径**：A1 修正「轮来源采样逐句覆写」使收尾判据③ 回到原注释自称语义（`requirements.md:39` 连续追问保护的正向路径），A2 修正冷通道 stale 回调使会话状态机按既有「session 结束后回 Idle」口径收敛——均无新验收条目、无口径改写。spec 面漂移检出 = 无，故无 architect spec 更新交接（`ARQ-260929-docs-sync-spec-handoff` 条款未触发；integrator 对 `docs/requirements.md` / `docs/architecture.md` 无写权、本轮亦无写需求）。
- `docs/architecture.md`（架构面）：**零 diff**。本包为单函数会话状态分发行为修复（application.cc 入口/出口接线 + 采样；websocket_protocol.cc 通道打开时序），补充轮 A1/A2 各一处守卫/拆除序微调——零结构性架构变更、零新增模块、零 wire 协议变更。
- `docs/contracts/**`（契约面）：**零 diff**（`git status --porcelain -- docs/` 实证空；contract-lint 闸五轮 PASS）。fork② 裁设备侧的直接推论 = wire 协议零变更（不新增、不修改任何引擎↔设备消息）；补充轮 A2 复用既有 `local_close_pending_` 关分类、不触 wire 形状，A1 纯本地采样逻辑——`crossEnd` 的提醒语音链两端（engine `kidpal_speak` 下行 / 设备 tts stop 收尾）仍各自闭环。契约变更门不触发。
- `docs/contracts/interface-contract.md` 仅作 `contractRefs` canary 引用（在册），两轮零触碰。

### 代码面 doc 钉行（随包在案、非本件义务）

- `firmware/main/application.cc`：两轮新增注释块随实现同 PR 原子落地（主轮 A″ 直推块 + tts stop 三守卫块 + sentence_start 采样块 + 文件级静态说明头注；补充轮 A1 守卫注释块），零 doc 漂移。
- `firmware/main/protocols/websocket_protocol.cc`：补充轮 A2 新增注释块随实现同 PR 落地，逐字说明「旧通道先拆的因果（本地关分类抑制 stale OnDisconnected）+ 真服务端/网络侧断分支（本地关分类未置位路径）零改动」。
- `server/deploy/engine-patch/connection.py`：主轮 `_kidpal_try_speak` 内同步调用注释（注明 `speak_txt` 为引擎原生同步函数、不可 await）随实现同 PR 落地；**补充轮零触碰**（本轮零增量改动、不重新部署 ECS）。

## 对 r1 既有 docs-delta 的影响（本轮 r2 收口更新说明）

r1 docs-delta 的三条结论在本轮（补充轮 261014-r2）**原样延续、无翻转**，本件仅作增量补登，不重写既有论断：

1. **r1「本批零文档面改动」结论延续**——补充轮 A1/A2 同样是既有口径兑现（见上 §文档面改动清单），累计两轮仍零文档面改动；`docsSyncRequired=false` 未变。
2. **r1「TECH CRQ 三条目状态不翻转」结论延续**——补充轮 `plan.md`「明确不做」节第 1 项把 **F2 查字轮漏一轮 ASR** 判为**引擎侧缺失（设备侧无解，主线程登记 CRQ 残余项）**；engine-patch 本轮零增量改动、不重新部署 ECS。CRQ 闭账条件仍 = 包 merge + 真机复验轮，两者均不在本轮范围（human 裁）——CRQ 状态翻转与闭账留痕仍留待主线程联测轮，integrator 按 scope 写权不翻转 CRQ。
3. **r1「零新增 ADR」结论延续，但补轮登记一处残余项**——`adrRequired=false` 未变（A1/A2 为单函数行为修复，非契约/跨系统边界裁决，不构成 ADR 事由）。补充轮新增登记的三项残余（F2 引擎侧 ASR 丢弃 / 阻塞式 OpenAudioChannel 未动 / MQTT 协议同族潜伏）均为「不扩本包」的 follow-up，去向 = 各自 CRQ 登记（主线程/后续批），**非本件 ADR 事由、非本 Pod 义务**。

## TECH CRQ 登记状态（如实申报、不翻转）

三条源 CRQ——`docs/crq/tech/TECH-261014-wakeword-connecting-voice-layer` / `TECH-261014-post-speak-listening-lingering` / `TECH-261014-kidpal-speak-await-nonetype`——**两轮累计零触碰**（`git status --porcelain -- docs/` 空）。taskpackage `docsSyncReason` 明载：三条目**闭账条件 = 包 merge + 真机复验轮**，两者均不在本轮范围（human 裁执行边界）——CRQ 状态翻转与闭账留痕留待主线程联测轮处理，非本 Pod 义务。补充轮新登记的引擎侧残余（F2 / 阻塞式 OpenAudioChannel / MQTT 同族）同理归主线程/后续批。**本 integrator 按 scope 写权不翻转 CRQ 状态、不新增 CRQ 条目**。

## 零改申报（其余面）

- `docs/crq/**`：零触碰（见上）。
- `.claude/**`、`tools/**`：零触碰（`git status --porcelain -- .claude/ tools/` 空）。
- `docs/requirements.md` / `docs/architecture.md`：零 diff（本包断言，exitCriteria「wire 协议零变更」承载）。
- ADR：**零新增**（`adrRequired=false`；见上「对 r1 既有 docs-delta 的影响」第 3 条）。
- `firmware/main/test/record_session_live.acceptance.c`（VR-A5 canary）：零 diff（在本轮 168 Tests 全量 host 实跑中保持绿）。
- `firmware/main/session_state_machine.cc`（60 秒 timer 接线面）：零 diff（本批明令不接线）。

## Post-cutoff 制品纪律确认

本包 post-EVENTS_SCHEMA_CUTOFF（changeId 前缀 261014 >= 250725）：**不产** `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner 收尾相位的 `pod-retro.md`。`delivery/taskpackages/261014-voice-round-entry-exit-fixes/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write，含 r1 scoped + r2 final 红 + r3 final 绿 + r4 scoped + r5 final 绿）；`delivery/taskpackages/261014-voice-round-entry-exit-fixes/pod-events.jsonl` 在场（advisory 检查归 planner/supervisor、non-blocking，本件不阻塞 closeout）。机器证据投影 = `verdict.json` + `verification.json`（advisory，non-bearing；承重机器件 = `verifier-evidence/rounds/r5.json`；scoped 佐证 = `rounds/r4.json`）。验收测试文件（本件引用的 `*.acceptance.c` 行号）由验收件承载，本件不复制其正文。
