# 261115-session-channel-resident · workstream review

（append-only；本文件记录门 1（拆包冻结门）verdict 与逐项结论。）

## 门 1 · 拆包四件套 checklist review（2026-11-16，reviewer = architect，只读零写入）

- **被审对象**：`delivery/workstreams/261115-session-channel-resident/{roadmap.md, decision-coverage.json}` + `delivery/taskpackages/261116-channel-resident-core/{taskpackage.json, plan.md}` + `delivery/taskpackages/261117-session-end-intent-longretry/{taskpackage.json, plan.md}`；拆包 commit `214e7a2e`（分支 `dev/pool-channel`）。
- **verdict**：**BLOCK(F-01, F-02, F-03, F-04)** + MAJOR F-05/F-06/F-07/F-08 + MINOR F-09 + NIT F-10。
- **切法主干成立**：2 包 serial WIP=1、mergeOrder 77→78、60s 超时确在 P1、契约/spec 边界零越界、roadmap ② 经脚本渲染复核一致、`upstreamFrozen` pin 复算一致（`sha1:bc53e2b9d5cf772d`）。

### 逐项结论

| # | 结论 | 摘要 |
|---|---|---|
| 1 切法忠实 | 否回 | F-01（P1 收尾三件之②与冻结 oracle 对撞）、F-05（「不相交函数区」失真）、F-06（真机轮字表偏离） |
| 2 依赖与顺序 | 部分否回 | F-05：`HandleEndVoiceSessionEvent` / `HandleToggleChatEvent` 两函数被两包分别改不同行段，不是「不相交函数区」 |
| 3 决策覆盖 | 否回 | F-02（workstream 缺 `launchPreconditions` 顶层键 → `--check-launch` 空绿）、F-07（C7/C8 包级无承接） |
| 4 跨包行为登记 | 否回 | F-08（owner/carriedBy 倒置，承重约束送不进 P2 的 input-brief）、F-05（重锚转手项未登记） |
| 5 验收方向可判 | 否回 | F-01、F-03（P1 zero server 条目）、F-10（SEI-B3 缺回引） |
| 6 冻结齿重写申报 | 部分否回 | F-03（server 面未扫）、F-05（P1 新冻结件将被 P2 重锚未申报） |
| 7 契约/spec 边界 | GREEN | 带 NIT（CMakeLists 路径差异、三份条件件未记扩单口径） |
| 8 origin 与机器字段 | GREEN | `upstreamFrozen` 实算一致；status 枚举值随 F-02 一并修 |
| 9 roadmap 投影 | GREEN | ② 脚本渲染、人工未改；③ 回指用锚点不复制 story 正文 |

### Findings 摘要

- **F-01（BLOCKING · 需回 /arch-change）**：P1 的「`listen stop` 残余累积态取消（`acc.cancel(reason="session_end")`，routing = `pending_stop` 时）」与冻结 oracle `test_m4k1_story_cloud_link.acceptance.py:1891` `test_sao_a2_empty_stop_holds_pending_not_bail_to_chat` 断言级矛盾——manual 模式 ASR 由 `listen stop` 触发、转写约 1.7s 后到（`front_app.py:577` docstring 实录），`cancel()` = `_sessions.pop`（`story.py:886-894`），在途转写不可恢复 → 录音态 KEY 收尾归档丢失，违反 S-F2.2 AC4 v1.65。候选方向：`pending_stop` 排除出收尾三件之②，记录语境释放挂到既有 `_story_timeout_sweep` + `PENDING_STOP_SECONDS = 10.0` 释放链（`front_app.py:1208/1225/1255`、`story.py:853-872`、`_release_record_context` `front_app.py:1673`）。路由 = 回 `/arch-change`，门 1 不代裁。
- **F-02（BLOCKING）**：workstream `decision-coverage.json` 缺 `launchPreconditions` 顶层键（canonical 落点 = `task-intake-and-slicing.md:319`），`portfolio-reconcile --check-launch` 实测 `launchPreconditions: null` = 该维空绿；两包 `status` 用了 `met`/`pending`，canonical 枚举是 `verified`/`unverified`。
- **F-03（BLOCKING）**：P1 `impactedTestsSeed` 零 server 条目，而 P1 改的三处 front 缝已被 ≥5 份 server 冻结件钉住（`test_record_cancel_lifecycle` / `test_m4k1_story_cloud_link` / `test_m4k3_record_session_live` / `test_record_dialogue_guide` / `test_record_front_polish`）。
- **F-04（BLOCKING）**：拆包侧预写 `acceptance` / `acceptanceTestsFile` / `redBaselineResult`，越过 acceptance-author 唯一写点（`task-intake-and-slicing.md:300`；上一列车创建态 = `acceptance: []`）。断言方向应移进 `plan.md`「验收方向（acceptance-author 细化，非冻结）」节。
- **F-05（MAJOR）**：「application.cc 不相交函数区」失真——`HandleEndVoiceSessionEvent` 与 `HandleToggleChatEvent` 被两包分别改不同行段；P2 落地会让 P1 的 SCR-A2「in-flight 守卫逐字在世」变红，重锚转手项未登记。
- **F-06（MAJOR）**：真机轮字表偏离 `package-split.md` §4.3，打断 FROZEN PROPOSAL 的 debt 结项凭据字母锚（`TECH-261014-autoconnect` = B4+B5 = 拔网线/重启 front 自愈）；P1 丢 A8 录音态提醒排队例外与 A9 查字窗关闭路径；P2 丢 B2/B3/B6。
- **F-07（MAJOR）**：C7/C8 `landsIn` 指 P1 但 P1 包级 `decisionCoverage` 无对应条目。
- **F-08（MAJOR）**：`crossPackageBehaviors` 三条 owner/carriedBy 同值，按 `brief-inputs.mjs` 过滤实测 P2 的 input-brief 拿不到任何以它为承接方的承重约束。
- **F-09（MINOR · human 裁）**：workstream 缺 `task_plan.md` / `review.md`，未登记 `milestones.jsonl`、不在 workstream INDEX 投影；整改型 workstream 是否豁免里程碑登记规则无明文豁免 → 请 human 裁（补齐或显式记例外）。**裁定（human 2026-11-16）＝补登**：已补 `task_plan.md` 与本 `review.md`，并在 `delivery/milestones/milestones.jsonl` append `M8-session-channel-resident`（state=planning / health=green），`STATUS.md` 由 `delivery-status.mjs` 再生。
- **F-10（NIT）**：SEI-B3 缺回引；SCR-A1 标题「六触发源」但只覆盖四收尾点；manifest 与种子 CMakeLists 路径差异、三份条件件未记扩单口径。

### 处置

1. F-01 → 回 `/arch-change`（触 FROZEN PROPOSAL T2 改法 2 与契约 §四 discard/stop 顺序段）；裁定回来前不 launch P1。
2. F-02/F-03/F-04 与 F-05–F-08 在同一修正轮落盘（F-04 与 F-01 耦合，待 F-01 裁定后一次改完）。
3. F-09 交 human 裁；F-10 顺手修。
4. 修正后走聚焦复检（沿 261103 门 1 两轮先例）：逐项闭环 + `--check-launch` 的 `launchPreconditions` 非 null + 两包 `path-not-on-disk` warnings 归零。