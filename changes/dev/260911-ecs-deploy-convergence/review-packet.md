# Review Packet — 260911-ecs-deploy-convergence（批次① ECS 部署收敛 · 漂移修复 + 提醒 cron §8 部署 + live 五齿 L1-L5 交班 + #13 证据回收）

> integrator 产出 · 2026-09-11 · 分支 `dev/260911-ecs-deploy-convergence`（base = main `6364a89` = 260829-hardware-validation 合并尾差；本 phase 前功能提交已在分支：`28e4319` stage1 + `6f110c0` D5 纠偏）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（live-deploy/evidence-recovery gateProfile），照常触发 supervisor 升级人类。
> 交付本体：EOS redeploy 收敛——`apply-xiaozhi-overlay.sh` 渲染输出与 ECS live 逐行 diff 后全量对齐（D1-D7，其中 D7=prompt v1→v4 为唯一预期 delta），修复后渲染面 vs live 备份 diff = 零意外 delta；提醒 cron §8（kidpal-reminder-plan 15m cron + lastRunStatus=ok + front.service env）上线；live 五齿 L1-L5 证据交班 + WSA-A4 flash.log 归位 + WSA-A6 当日真探针；引擎补丁/gateway 代码 ECS 同步。**部署窗口（2026-09-11 10:33–12:10）由 human 全程在场**，live 侧同步不产本 repo diff（见 docs-delta §B 对应关系）。
> **合并门必读**：本包无 BLOCKING finding。四条实测发现（§Findings）全部为「留红如案 / 交班」项，无一由本包 repo diff 引入，无一阻断部署收敛包合并——ECS-A5 的 L4 渲染回执未达与 ECS-A6 的 WSA-A4 三制品散失均已按计划留红、不伪造；发布面代码（4 件 deploy 文件）经 r5 七闸 + 全量验收套复跑确认零回归。

## Change Size

- Files changed（repo 侧，`6364a89..HEAD`）：**17 文件 +765/−285**（含 13 件 dev-loop 执行 ledger / evidence 投影）。
  **实现面 = 4 件 deploy 文件 +32/−28**：
  - `server/deploy/apply-xiaozhi-overlay.sh`（42 行中 +/−28 净）：D1-D6 漂移全量对齐 + D5 auth 二次纠偏（两个 commit：`28e4319` D1-D6 + `6f110c0` D5 auth 回 false）。
  - `server/deploy/provider.config.yaml`（12 行 ±）：`providers.asr` 声明面对齐 Qwen3ASRFlash 终选。
  - `server/deploy/README.md`（2 行）：ASR 凭据行对齐终选（去 ALIYUN_ASR_APPKEY/TOKEN）。
  - `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（+4 行）：§8 头「覆盖指针」。
- Net diff：本 phase 另加 closeout 制品 2 件（`changes/dev/260911-ecs-deploy-convergence/{docs-delta,review-packet}.md`）+ 归位证据件 1（`changes/dev/260829-schedule-reminder/live-five-teeth-evidence-260911.md`，从 `server/acceptance/` 原样搬入 + L1 回填）。
- Scope: server 部署载体面（taskpackage `subsystem` 钉定）；`docs/contracts/**` 与 `docs/spec.md` **零改动**（`git diff --name-only 6364a89..HEAD -- docs/` = 空输出，contract-lint r5 PASS）。
- Split recommendation: **不拆**——漂移修复 + cron 部署 + 证据回收是单一部署窗口的原子动作（human 已批窗口语义，pod-events `04174089`），拆开任一面都要二次上 ECS。ECS-A7 回滚就绪保证可还原。

## Verification Evidence

> **承重 fact**：final completed-only round = **r5**（`phase:"scoped"`，identity = `changeId + phase + round + invocationId`）；
> `verifier-evidence/rounds/r5.json`：`ok:true`、`observedGateExit:0`、`authority:"bearing"`、`producedBy:"verify.mjs"`、
> `invocationId adfc128d-e62d-46e0-9ffa-2051ed1f5bc0`、`startedAt 2026-09-11T04:13:45.926Z / completedAt 04:14:12.941Z`。
> `current/verdict.json` 为同一 `producedBy:"verify.mjs (script fs-write)"` 的投影（non-bearing compatibility projection），closeout 判断以 r5 fact 为准。

- **verifier 逐轮收敛轨迹（r1-r5，每轮闸结果）**：
  - **r1**（inv `1e0a4a96-…`，scoped）：唯一红 = `acceptance-coverage`——acceptance 由字符串条目转对象条目后的首跑，`missingCoverage:` ECS-A1..A7 全列表。根因 = 本包 acceptance 原为**字符串数组**（非可断言对象），须转 `{id, kind:"structural"}`（260731 一等豁免通道，镜像 260829-hardware-validation 16 条 structural 同闸先例）。**非实现缺陷**，是声明形态修正。
  - **r2**（inv `d37fa773-…`）：唯一红 = `mutation-check`——`mutationRequired=false must include mutationSkipReason/mutationReason`。根因 = 缺 `mutationSkipReason` 字段（证据型包零新写测试，mutation 不适用）。**非实现缺陷**。
  - **r3**（inv `811341c8-…`）：唯一红 = `docs-sync`——`docRefs do not mention any declared acceptance ids`，`docsWithoutAcceptance:["server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md#§8"]`。根因 = 覆盖指针未落。fixer 补 `DEPLOY-RUNBOOK.md §8` 头 4 行指针后过。**非实现缺陷**。
  - **r4**（inv `e0e9c43c-…`）：七闸全绿 `ok:true`。
  - **r5**（inv `adfc128d-…`）：七闸全绿 `ok:true`、`observedGateExit:0`（final）。
  - **fixer 两轮次**均为「声明形态/字段/指针」修正，无一次是实现代码缺陷（`friction` 事件仅超读 2+3 条，纯记账）。
- **r5 逐闸（实测 ms）**：taskpackage-required 181 / **firmware-build 25979（`build-pass` 真编译，非降级——r5 stdout `"phase":"build-pass"` + ninja `[1/5]…[5/5]` 全跑、`xiaozhi.bin 0x2c2450`、分区余 20%）** / contract-lint 142（闸对象 `docs/contracts/interface-contract.md`）/ acceptance-coverage 147（`acceptance` ECS-A1..A7 全在册、`structural:["ECS-A1",…"ECS-A7"]`、`suiteGates:[]`、files = 5 件冻结件）/ mutation-check 140（`skipped:true`，mutationRequired=false + mutationSkipReason 已填）/ docs-sync 144（6 docRefs 全在场 + acceptance 全 id）/ security 134（`securityReviewRequired:false`）。
  **豁免通道清单（SSOT 逐条枚举，supervisor 核其存在）**：`structural:["ECS-A1","ECS-A2","ECS-A3","ECS-A4","ECS-A5","ECS-A6","ECS-A7"]`（七条全 structural，gateProfile=live-deploy/evidence-recovery 证据型口径，判据 = 渲染 diff 证据 / live 部署核验 / 既有冻结齿复跑的文件粒度结构断言，非逐条可断言行为条目）；`suiteGates:[]`（空，无需 human glance）；`mutation-check skipped`（mutationSkipReason 立项即裁定，证据载体 = 渲染 diff 文件 + live 人工核验 + 既有冻结齿复跑）。
- **全量 server 验收套复跑（integrator 本 phase 实跑，2026-09-11，工作树未动）**：`cd server && python -m pytest acceptance/ -q --import-mode=importlib` → **1 failed / 125 passed / 51 skipped**（51 skip = `@pytest.mark.live` conftest 机械闸，本 phase 不 auto-run live）。
  唯一红 = `test_smoke_e2e.acceptance.py::SmokeE2EStructuralAcceptance::test_a4_display_line_evidence`（SMK-A4：`server/acceptance/_artifacts/smoke-e2e/screen.md` 证据缺、无 on-screen line evidence）——`[evidence]` 族先行红，与本包 ECS 部署收敛**零相关**（本包不产 display-line 证据），**零新增红**。
  **留红如案（host runner C 齿，本 phase python-only 复跑不覆盖，据 live-findings note 逐条登记、不喂哨兵）**：P2-A1（first-match 歧义，归批次③渲染批）；WSA-A4（三制品散失 session-id 不配对，留红如案，flash.log 归位已完成——见 Findings #4）；VRT-A2/E2（回环证据齿）。三者均为固件 host ELF `[evidence]` 族红，非本包范围。
- **部署窗口实况（2026-09-11 10:33–12:10，human 全程在场；live 侧不产 repo diff）**：
  - **S2.1 apply**：修复后 `apply-xiaozhi-overlay.sh` 渲染到临时目录 vs live 备份 diff = **零意外 delta**（唯一 delta = D7 prompt v1→v4，交付物本身）。D1-D6 全对齐，D5 经 human 二次纠偏（S2.3 冒烟失败 → 引擎 auth 回 false，pod-events `f9322b9c`）。
  - **S2.3 语音 e2e**：引擎容器重载后 live config 四 provider 全对（Qwen3ASRFlash/OpenClawOpenAI/EdgeTTS/nointent/nomem）+ prompt v4 在场；10:55 一轮完整语音端到端 0 断链（engine 日志 exercise：listen→ASR→TTS→音频下发）。
  - **S2.4 cron**：kidpal-reminder-plan 在册（`862ce6ab`，venv 解释器载荷）+ `state.lastRunStatus=ok`（~12:00 planner 实测 `ok (not delivered)`，`(not delivered)` = 投递路由注记、载荷跑成功）+ kidpal-front.service env 含 KIDPAL_ALERTS_DIR/KIDPAL_SERVER_DIR + `alert_poll` 心跳（11:34-11:45）。
  - **L3 真实推送**：`alert_deliver delivered=True device_frames=1 speech=True` + 语音下行证据 `发送音频 FIRST "@/LAST"`（绿）。
- **ECS-A1~A7 逐条状态**：
  | 齿 | 状态 | 依据 |
  |---|---|---|
  | ECS-A1（漂移全量对齐） | **绿** | S2.1 渲染 diff 零意外 delta（live-findings §交付达成） |
  | ECS-A2（冻结齿不回归） | **绿** | 聚焦套 53 passed/18 skipped/0 failed（`redBaselineResult`）+ r5 acceptance-coverage |
  | ECS-A3（live 部署核验） | **绿** | 四 provider 全对 + prompt v4 + 0 断链 |
  | ECS-A4（提醒 cron §8） | **绿** | 862ce6ab 在册 + lastRunStatus=ok + env + 心跳 |
  | ECS-A5（live 五齿 L1-L5） | **绿 / 留红如案** | L1 绿（lastRunStatus=ok 回填）/ L2 绿（契约 §三 逐字段合）/ L3 绿（真实推送）/**L4=红发现（渲染回执未达，Findings #1）** / L5 pending（机械兜底态如实登记，语义产物由 agent 回合后补） |
  | ECS-A6（WSA-A4/A6 证据回收） | **绿 / 留红如案** | flash.log 归位（188 处烧录标记精确匹配）+ WSA-A6 当日真探针绿（8080 `TcpTestSucceeded:False`/8000 `True`）/ **WSA-A4 三制品留红如案**（Findings #4） |
  | ECS-A7（回滚就绪） | **绿** | live `.config.yaml` 时间戳备份多份 + `openclaw cron remove` 演练成功（`"ok":true,"removed":true`）+ 冻结面零触碰 |
- **Security gate**：r5 `ok:true`（`securityReviewRequired:false`）。本包无新公网入口、无新 secrets（D1 把 ASR 凭据改读 env `DASHSCOPE_API_KEY`、淘汰 ALIYUN_ASR_APPKEY/TOKEN，零硬编码）；安全边界 = 引擎腿 loopback `127.0.0.1:8010` + front 层静态 token 强制（260903 架构，D5/D6 裁定钉死）；ECS-A6 当日探针实证 8080 公网不可达、8000 唯一公网入口（decision 18）。
- **Docs sync**：r5 `ok:true`（6 docRefs + acceptance 全 id 在册）。判定与逐条理由 = 同目录 `docs-delta.md` §A（canonical 零改动 + `docs/contracts/**`/`docs/spec.md` 零触碰 + 部署载体 4 件 repo commit + 4 件 ECS 同步对应关系）。**不改 docs/contracts/**。
- **Closeout**：本 phase 跑 `node .claude/scripts/closeout-check.mjs --update-index` → 结果见 §Closeout。

## Simplification Debt

> 本包实现面 = 4 件 deploy 文件 +30 行级小改，**无新写测试逻辑、无新抽象、无 `minimal:` 标记**（grep 全仓 `minimal:` 命中 0 处，为 260829-schedule-reminder 既有 1 处，本包未新增）。故本表 **无条目、无 no-trigger 项**——诚实声明「无简化债务」而非伪造一条。D1-D6 对齐本质是「把 deploy 声明面收敛到 live 单一事实源」，删的是漂移冗余（ALIYUN_ASR_APPKEY/TOKEN 淘汰移除 = `delete` 语义，但由 builder 已在 stage 1 做完、不留债）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | live-acceptance / 固件提醒弹窗面（跨包） | **L4 渲染回执 live 未达**：4 次投递均只收到设备 `ack:false`（`state:queued`）受理帧，无一次 `ack:true` 渲染回执 → 档位不出队、走重推上限。human 现场实测板子只显「该出发了」等 speech 文字（普通文字上屏），**未出 state-reminder 富弹窗**——提醒弹窗消费面（`reminder_page`）live 未渲染、掉回普通显示路径，故无渲染回执。固件侧 `mcp_server.cc` 有 `kidpal_reminder_may_emit_receipt` 判定门，live 未触发；疑闪录固件提醒弹窗面未随 schedule-reminder 生效 | Basis: 冻结固件面 = `firmware/main/mcp_server.cc` `kidpal_reminder_may_emit_receipt` 判定门（契约 §三「渲染即确认即出队」语义的车载面）；live 证据 = `alert_deliver delivered=True` + 无 `ack:true` 回执 + human 现场实测无富弹窗（live-findings note finding #1）；heuristic: probe≠exercise（L3 推送绿 ≠ L4 渲染出队绿） | 归 schedule-reminder 固件面后续排查（本包越权不改冻结固件）；human 待办「L4 回执面」技术裁量。不阻断部署收敛包合并（本包交付 = 证据交班 + 部署收敛，非新车载弹窗渲染） |
| SHOULD | judgement-call | 引擎补丁 / own code（跨包） | **engine-patch `_kidpal_try_speak` await 报错**：每次提醒语音注入伴随 `ERROR-kidpal_speak 播报注入失败: object NoneType can't be used in 'await' expression`，但 edge-tts 语音仍照常合成下发（两段音频真实送出）——「报了错但主链仍通」的真 bug，在 own code `server/deploy/engine-patch/connection.py` | Basis: own code hunk = `server/deploy/engine-patch/connection.py` `_kidpal_try_speak`（260829-schedule-reminder 包挂点，本包只 sync 到 ECS 未改源码）；live 证据 = engine 日志 `ERROR-kidpal_speak … object NoneType can't be used in 'await' expression` 每次注入必现 + 音频仍送出；heuristic: 报错但主链通 = 非阻塞缺陷、须留痕 | planner 记后续修复：`_kidpal_try_speak` 的 await 目标为 None 时降级为不 await（真 bug 但主链通，非本包引入的 diff）。归后续 fix 包 |
| FYI | judgement-call | 产品决策（非实现面） | **设备休眠即断 WS → 推送只落在线时刻**：`alert_skip reason=no_online_device` 多次出现；设备闲置约 3 分钟即「再见」休眠断开。三档提醒的价值前提（前晚/早档在家时段在线）与当前休眠/连接行为不匹配 | Basis: live 证据 = `alert_skip reason=no_online_device`（kidpal-front 日志）+ human 现场指出；heuristic: 产品价值前提 vs 行为实况不匹配 = 决策项非代码缺陷 | human 待裁（候选项：常亮/充电时段保连、重连即重推重审、档位时刻对齐唤醒时段、晨档唤醒时补投）。不属本包实现面 |
| FYI | judgement-call | 证据回收（留红如案） | **WSA-A4 host 齿仍红（不伪造）**：除 flash.log 外三制品（provision.log / smoke-roundtrip.log / screen*.jpg）是 260903-device-ws-switch 包 closeout 时散失的证据遗物，部分真源在 `C:\tmp` 但往返证据 session-id 不配对（`d6f4e629` vs `40024357`），无法诚实拼装成一次匹配会话的往返。flash.log 归位（本包 #13 交付项）**已完成**（188 处烧录标记精确匹配） | Basis: live-findings note finding #4 + 本包 exitCriteria ECS-E2「WSA-A4/A6 证据落盘且含真实标记；host runner WSA 两齿复跑转绿」的 WSA-A4 半边留红；heuristic: probe≠exercise / 不拼凑过齿 | 留红如案、不拼凑（integrator 采纳该「不伪造」本身为 finding）；WSA-A4 三制品真源缺失由 260903-device-ws-switch 复盘承载，不开本包行动 |

**Kind 说明**：四条均为 `judgement-call`（无一条是本包 repo diff 引入的 hard-violation；live/跨包/证据类发现，Basis 引 changed-code 面 + live 证据 + heuristic，不伪造 rule citation）。无 BLOCKING → 不停合并门。

## Five-Axis Review

1. **Correctness（对 `ECS-Ax` / S0.3 定谳终表）**：D1-D6 逐条对齐 S0.3 定谳（D1 ASR⇒Qwen3ASRFlash `{api_key:$DASHSCOPE_API_KEY, model_name:qwen3-asr-flash, type:qwen3_asr_flash}`、D2 Intent⇒nointent、D3 base_url 补 `/v1`、D4 TTS type⇒edge、D5 auth⇒false、D6 补渲 `server.ip:127.0.0.1/port:8010`）——修复后渲染 diff 零意外 delta 是逐键对账的直接证据，不是「跑了没报错」；D7 prompt v1→v4 为唯一预期 delta（prompt 版本化 = `openclaw-system-prompt.v4.md` + `PROMPT_SRC` 指针一致，验收所测=生产所用）。D5 的二次纠偏（auth 回 false）被 HMAC 不相容理由注释钉死，防止未来再次误读「恢复 true」。
2. **Test quality**：本包零新写测试逻辑（mutationRequired=false + `mutationSkipReason` 立项裁定）；断言载体 = 渲染 diff 证据 + live 人工核验 + 既有冻结齿复跑。既有无回归（全量 python 1 failed/125 passed/51 skipped，唯一红 SMK-A4 为 `[evidence]` 族先行红，聚焦套 53P/18S/0F）。S2.3 的一轮语音端到端 0 断链 + S2.4 cron lastRunStatus=ok 是「验收所测=生产所用」的 live 实证。**诚实边界**：L4/L5 未绿（L4=红发现、L5=pending fallback），均如实登记不伪造。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/spec.md` 零触碰（`git diff --name-only 6364a89..HEAD -- docs/` 空 + contract-lint r5 PASS）。L2 载荷逐字段对照契约 §三（5 键全合）；D3 base_url 补 `/v1` 是 provider.config.yaml 已经记载的 live 教训（260906），不改契约。D5 auth=false 不构成 spec 漂移（决策22 静态 token 强制点在 front 层，不变；工程腿 loopback 拓扑 260903 已裁定）。GWA-A4 齿命中面保留（aliyun/dashscope/qwen3 任一）。
4. **Operability / security**：回滚就绪（live `.config.yaml` 多份时间戳备份 + `openclaw cron remove` 演练成功 + engine-patch/gateway ECS 侧 `.bak-260911-*` 备份 + schedule.md `.bak-260911-l3l4`）；凭据零硬化面（D1 凭据 env 化 + 淘汰 ALIYUN_ASR_APPKEY/TOKEN + 证据件全脱敏）；`openclaw cron list` 首版 88fceff0 因系统 python 3.6.8 不支持 `from __future__ import annotations` 首跑 error → 重注册 venv 解释器载荷（862ce6ab）是操作面正确纠偏、留痕。安全组 8080 规则状态 = **human 待回填**（agent 权限面外，见 Residual Risk）。
5. **Maintainability / simplicity**：部署载体 4 件改动局部、易懂、无过早抽象、零新依赖（`requirements*.txt`/`package.json` 零改动）。漂移修复的产物 = 声明面收敛到 live 单一事实源（去掉 ALIYUN_ASR_APPKEY/TOKEN 死引用）。无 `minimal:` 标记债务（§Simplification Debt 已声明）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=high` 但 planner 未开该面；本包的风险面是 live 部署（deployment window human 在场 + 回滚演练）而非探索式攻击面，planner 已用「human 全程在场 + live 冒烟 + 当日真探针」等价覆盖，本 packet 如实记录「未声明 ⇒ 未执行」，不以 builder debug run 冒充 official evidence。

## UI design-review 证据

本包无 UIUX work（TaskPackage 未声明 uiux 字段，改动为 server 部署载体面，无 viewport/a11y/design 触及）。无 UI finding。

## Residual Risk

- Known limitation:
  1. **L4 渲染回执未达**（Findings #1）：提醒弹窗消费面 live 未渲染、掉回普通显示路径，档位不出队走重推上限。本包已如实留红、如实交班；修复在 schedule-reminder 固件面（`mcp_server.cc` 判定门），非本包可改。
  2. **engine-patch `_kidpal_try_speak` await NoneType**（Findings #2）：每次注入必报错但语音仍通，真 bug 待后续 fix 包。
  3. **设备休眠断 WS**（Findings #3）：`alert_skip reason=no_online_device`，产品级决策项 human 待裁。
  4. **WSA-A4 三制品散失**（Findings #4）：session-id 不配对、无法拼装，留红如案；flash.log 归位已完成。
  5. **live 五齿未全绿**（L4 红 / L5 pending）：L5 语义产物由 OpenClaw agent 回合补（DEPLOY-RUNBOOK §8 混合分层，机械兜底态如实登记非伪装）。
  6. **firmware-build r5 真编译但本 phase 未复跑设备构建**：本包实现面纯 deploy 脚本（bash/python 前端无 C 面改动），`firmware-build` 闸 r5 真编译为冻结齿复跑范畴，integrator 本 phase 不做设备重建（planner 指令「不重建、树未动」的固件面）。
- Follow-up recommended:
  - schedule-reminder 固件面排查 L4 渲染回执（`kidpal_reminder_may_emit_receipt` live 未触发根因）；engine-patch `_kidpal_try_speak` await 修复；产品决策「休眠/在线时段」三档提醒价值前提；OpenClaw agent 回合产出 semantic.json 后补 L5 语义产物。
  - 安全组 8080 规则状态：human 待回填（先例 shim-archive-record.md:22）。
  - 测试条目「部署核验测试提醒」已无用，下次编辑课表时可删（schedule.md「一次性安排」行，human 待办）。
- Human decision needed:
  - **安全组 8080 规则状态确认并回填**（agent 权限面外；`server/acceptance/_artifacts/device-ws-switch/ecs-teardown-evidence.md §三` 已留 `[human-回填：…]` 占位，本 phase 保留占位、不代填）。
  - L4 回执面 / kidpal_speak bug / 休眠连接设计三项 finding 的产品与技术裁量。
  - D5 引擎 auth 边界不变式（EMAC 动态签名 vs 决策22 静态 token 互斥 → 引擎层 auth 恒 false + loopback + front 强制）为可复用架构裁决，是否需 ADR/ARQ 登记由 architect 裁（本 phase 已在 taskpackage notes + pod-events `f9322b9c` 留痕，不自建 ADR）。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 三条 decision（`human-20260906-ASR终选Qwen3ASRFlash`→ECS-A1、`contract-§三-alerts-pending形状`→ECS-A5、`ARQ-260901-g3-server-live-acceptance`→ECS-A3）的 `landsIn` **全部**指向 acceptance、**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项。

## Closeout

`node .claude/scripts/closeout-check.mjs --update-index`（本 phase 实跑，2026-09-11）→ **`ok:true` / EXIT=0**，`indexProjection:"inserted"`（`changes/INDEX.md` 单行 upsert 落「closed」）。`requiredFiles:["docs-delta.md","review-packet.md"]` 两件 blocking 制品在场；承重闸 `taskpackage-required-check` 程序化调用 PASS（F-04 单源）。
advisories 三条（均 advisory-only，不阻塞）：① `verdict.json`/`verification.json` 未晋升（投影·advisory，非承重；按 closeout 制品 SSOT 表二者由脚本 fs-write、非 agent 落盘 R11，缺失 advisory-only——与 260829-schedule-reminder 先例同处置，承重 = r5 fact）；② `pod-retro.md` 缺（planner 收尾相位产物，Q5=warn advisory、零追溯，非本 phase 职责）；③ loop-retro 触发阈值（9 pending + 2 waiting = 11 ≥ 3，发起责任 = 主线程 /loop-retro）。
post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 满足，15 行 human 裁决/纠偏/授权全留痕）。

## Commit 冻结（合并门复核的对象）

- 功能链 2 提交（base = main `6364a89`）：`28e4319`（stage1 漂移全量对齐 + provider + runbook §8 覆盖指针，17 文件）+ `6f110c0`（D5 二次纠偏 auth 回 false + HMAC 注释钉死，5 文件）。
- integrator 收拢提交（本次，含 closeout 制品 2 件 + 归位证据件 1 + dev-loop ledger 收口：pod-events.jsonl / verify-runs.jsonl / r5.json / live-findings note 等）；commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/*`、不 stage `current/verdict.json` 机器指针**（投影由脚本 fs-write）。
- 可回滚路径：全部交付物为 feature 分支 `dev/260911-ecs-deploy-convergence` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert + ECS 侧 `.config.yaml.bak-*` 多份时间戳备份还原 + `openclaw cron remove <id>`（演练路径已在案）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并。本 integrator 不自行判定通过、不执行 merge。

## 合并门须知（短清单）

1. 无 BLOCKING finding；四条 SHOULD/FYI 均为留红如案/交班项，不阻断合并。
2. 承重 fact = **r5**（phase scoped，7 闸全绿）；supervisor 复核读 `verifier-evidence/rounds/r5.json`。
3. 全量 python 1 failed/125 passed/51 skipped，唯一红 SMK-A4 为越界 `[evidence]` 红、零新增。
4. 冻结面（`docs/contracts/**`、`*.acceptance.*`、`managed_components/**`）git 零触碰（ECS-E3）。
5. human 待办三条（安全组 8080 回填 / L4+kidpal_speak+休眠三 finding 裁量 / 可复用裁决是否立 ADR），均不阻塞本包合并。