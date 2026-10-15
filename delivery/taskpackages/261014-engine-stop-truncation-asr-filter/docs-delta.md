# docs-delta —— 261014-engine-stop-truncation-asr-filter

> blocking closeout 制品。integrator 落笔（2026-10-14 收口制品轮）。本包 = 单包、rigor=lite、sourceType=regression、riskTier=medium：引擎 listen stop 收尾截断 + ASR 幻听串过滤——查字/播报收尾后引擎对尾音幻听出「不客气」寒暄被 LLM 回复推送（defect of record = `docs/crq/tech/TECH-261014-engine-asr-hallucination-after-stop.md`，真机两轮逐字命中）。修法三改动点（权威 = sibling `plan.md`，architect `/triage-change` 分流 review 2026-10-14，human 触点①「做吧」已裁）：① `connection.py` `_route_message` 内新增 `_kidpal_on_listen_message` listen 帧旁路截断（先清后转，manual 模式豁免）；② `chat` 入口 depth==0 守卫（截断态丢弃 + 幻听串过滤，不置 client_abort）；③ 模块级幻听串表 `_KIDPAL_ASR_HALLUCINATIONS` + `_kidpal_is_asr_hallucination`（归一化后整串相等，绝不子串）。改动面收在 `server/deploy/engine-patch/connection.py`（+93/−0，bind-mount 单文件）+ 新增 `server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py`（804 行，ET-a..g 七齿）。

## 文档面改动清单

**零文档面改动**（taskpackage `docsSyncRequired=false`）——docs-sync 闸 r3 scoped **PASS** 承载；docsSyncRequired=false 的理由字段（`docsSyncReason`）即本包零触碰申报，integrator 逐项独立核验属实（`git status --porcelain -- docs/` 空、`git diff --stat -- docs/requirements.md docs/architecture.md docs/contracts/` 空）。plan.md「不动（显式零触碰面）」节逐条对账：

- `docs/requirements.md`（需求面）：**零 diff**。本包为引擎侧内部处置修复，无用户可见 spec 语义变化——截断只作用于「listen stop（非 manual）后引擎对缓冲尾音的 stop 触发识别」，该路径本就是 defect 行为而非 spec 口径；幻听串过滤是引擎入口噪声治理。无新验收条目、无口径改写。spec 面漂移检出 = 无，故无 architect spec 更新交接（`ARQ-260929-docs-sync-spec-handoff` 条款未触发；integrator 对两份 spec 文件无写权、本轮亦无写需求）。
- `docs/architecture.md`（架构面）：**零 diff**。改动收敛在既有 engine-patch bind-mount 单文件内（listen 帧旁路消费 + chat 入口守卫 + 模块级常量表），零结构性架构变更、零新增模块、零新增依赖、零 wire 协议变更（listen start/stop 为既有消息，front 旁路消费转发不变）。
- `docs/contracts/**`（契约面）：**零 diff**（contract-lint 闸 r3 PASS）。wire 协议零变更的直接推论 = 无契约条目触面；`docs/contracts/interface-contract.md` 仅作 `contractRefs` canary 引用（在册），零触碰。契约变更门不触发。
- `docs/crq/tech/TECH-261014-engine-asr-hallucination-after-stop.md`（源 CRQ）：**本轮零触碰**（`git status --porcelain -- docs/` 空）。CRQ 闭账条件 = 包 merge + 真机复验轮（live-manual，human 手工部署 ECS 后执行）——两者均不在本 Pod 范围（plan 执行边界：只 commit 本分支、不部署 ECS），CRQ 状态翻转与闭账留痕留待主线程手工部署 + 真机复验轮处理，**本 integrator 按 scope 写权不翻转 CRQ 状态**。

### 代码面 doc 钉行（随包在案、非本件义务）

- `server/deploy/engine-patch/connection.py`：三处改动点新增注释块随实现同 PR 原子落地——幻听串表逐条来源注记（真机逐字命中 + Whisper 系已知静音幻听 + 中文视频结尾语同族）、`_kidpal_on_listen_message` docstring（病灶机制 + 先清后转顺序 + manual 豁免毁稿保护 + 静默返回语义）、`chat` 守卫注释（不置 client_abort 的原因）、`_initialize_components` 启动 marker 行「kidpal stop-truncation/asr-filter patch active」（部署生效锚）。零 doc 漂移。

## 零改申报（其余面）

- `firmware/**`：零 diff（CRQ 已核实设备侧无责：判据② 两轮正确触发；crossEnd exempt 留痕于 taskpackage）。
- `manifest.yaml` / `docker-compose.yml` / `deploy.sh`：零 diff——全部改动收在既有 bind-mount 单文件 `connection.py` 内，不扩部署面。
- 镜像内上游文件（`core/handle/**`、`core/providers/**`）：零触碰。
- manual 模式（故事录音）行为零改动：豁免是硬边界（ET-b 承重齿钉住），SAO-A2 转写归档通道零扰动。
- `.claude/**`、`tools/**`：零触碰。
- ADR：**零新增**（`adrRequired=false`；单文件内部处置修复，非契约/跨系统边界裁决，不构成 ADR 事由）。

## Post-cutoff 制品纪律确认

本包 post-EVENTS_SCHEMA_CUTOFF（changeId 前缀 261014 >= 250725）：**不产** `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner 收尾相位的 `pod-retro.md`。`delivery/taskpackages/261014-engine-stop-truncation-asr-filter/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write，含 r1/r2 scoped 红 + r3 scoped 绿）；`delivery/taskpackages/261014-engine-stop-truncation-asr-filter/pod-events.jsonl` 在场（advisory 检查归 planner/supervisor、non-blocking，本件不阻塞 closeout）。机器证据投影 = `delivery/taskpackages/current/verdict.json` + `verification.json`（advisory，non-bearing；**承重机器件 = `verifier-evidence/rounds/r3.json`（scoped 绿事实）**；final bearing 轮 r5 由 planner 持本包 `test-scope-final.txt` 圈定清单跑，fact 落盘后 closeout 判断以 final fact 为准）。验收测试文件（本件引用的 `*.acceptance.py` 断言语义）由验收件承载，本件不复制其正文。
