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
## 门 1 · 聚焦复检（2026-11-16，reviewer = architect，只读零写入，HEAD `03d14cb0`）

**verdict：BLOCK(N-01)** —— 首轮十条 findings 实质全部闭环（F-01…F-08 CLOSED，F-09/F-10 各余一个子项），复检新跑出一条机器 fail-loud：P1 `crossEnd` 的对端符号 `SendStopListening` 解析失败（`protocol.cc:88` 是限定名 `Protocol::SendStopListening`，而解析式左邻排除 `:`），`brief-inputs.mjs` 不产 input-brief，P1 planner Step 7 会硬停。修法 = symbol 改限定名（一个 token，已实测修正候选可解析）。

- **F-01 CLOSED**：v1.2 FROZEN（R3 F-01 CLOSED → R4 GREEN）+ canonical delta（arch §5.1.2/§5.2/§8.1 行 18/§12、ADR-019）+ 拆包侧三层防线（P1 `plan.md` 收尾分层 + SCRF-B2/B2b/B4/B4b/B6 + KRC-E4 A5(a)/(b)）一致、活制品零「当刻 cancel pending_stop」残留。
- **F-02 CLOSED**：workstream `decision-coverage.json` 四条 LP；`--check-launch` 两包 `launchPreconditions` 非 null，P1 pending 只余「门 1 GREEN + human 过目」。
- **F-03 CLOSED**：P1 补五份 server 冻结件 + 逐条处置。
- **F-04 CLOSED**：两包 `acceptance`/`acceptanceTestsFile`/`redBaselineResult` 回空骨架，20 条方向移入 plan.md「验收方向（非冻结）」节，`path-not-on-disk` warnings 归零。
- **F-05 CLOSED**：「同一批函数内改不相交行段」+ SCR-A2 重锚转手项两面登记。
- **F-06 CLOSED**：P1 A1–A13 / P2 B1–B8 与 `package-split.md §4.3` 对齐，workstream §5 的 `B4+B5` debt 凭据锚恢复可用。
- **F-07 CLOSED**：C7/C8 已入 P1 `decisionCoverage`。
- **F-08 CLOSED（机械证明）**：owner/carriedBy 归正后，`brief-inputs --dry-run` 的 P2 ⑤ 跨包承接面 7 条含「end_session 必须收口到 P1 处理器」。
- **F-09 余件**：workstream 投影件未再生（已随修正轮修：`planning-index.mjs` 后 INDEX 与 index.jsonl 均含本 workstream）。
- **F-10 余件**：SCR-A1 标题未收窄（已随修正轮修）。
- **N-01（BLOCKING）**：`261116/taskpackage.json` `crossEnd` 的 `SendStopListening` → `Protocol::SendStopListening`（已修，`brief-inputs --dry-run` 两包均产出）。
- **N-02…N-06（MINOR/NIT）**：C5 summary 误粘、C7 landsIn 与 summary、`plan.md` 验收编号区间、KRC-E4 编号来源句、LP3 evidence 未含修正轮 commit、双括号句——均随修正轮修。
- 机械面：两包 `--check-launch ok:true`、`taskpackage-required-check` Draft 态 missing 三件 + `warnings:[]` + `coverageErrors:[]`、`render-roadmap` 幂等、PROPOSAL pin 复算一致、`settings-lint` 0 RED。

> **编号消歧注记**：本文件「门 1 F-01…F-10」是**拆包冻结门**的 findings 编号；`delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md §7` 的 R3/R4 用的是**评审 lane** 自己的 F 编号（其「F-04」= delta-5 的「§8.1 四行」补正）。两套编号不同源，阅读时按「门 1 F-xx」/「R3 F-xx」区分。

### 门 1 聚焦复检 GREEN（2026-11-16，reviewer = architect，只读零写入；被审四件套以 commit `003cb44b` 为准，base ref `base/261115-session-channel-resident` 同点）

制品 = `delivery/workstreams/261115-session-channel-resident/{roadmap.md, decision-coverage.json, task_plan.md, progress.md, review.md}` + `delivery/taskpackages/261116-channel-resident-core/{taskpackage.json, plan.md}` + `delivery/taskpackages/261117-session-end-intent-longretry/{taskpackage.json, plan.md}`。逐项 CLOSED：F-01（review PROPOSAL FROZEN v1.2 + R3「F-01 CLOSED」+ R4 GREEN；canonical delta 落 `docs/architecture.md:198/227/493/566` 与 ADR-019；P1 三层防线 + SCRF-B2/B2b/B4/B4b/B6 + KRC-E4 A5(a)/(b) 一致，活制品零「当刻 cancel pending_stop」残留）、F-02（LP 顶层键四条，两包 `--check-launch` 的 `launchPreconditions` 非 null，P1 pending 只余门 1）、F-03（P1 `impactedTestsSeed` 五份 server 冻结件 + 逐条处置）、F-04（acceptance 三面回空骨架、方向移入 plan.md 非冻结节、`path-not-on-disk` 归零）、F-05（同函数内不相交行段 + SCR-A2 重锚转手）、F-06（A1–A10 沿 package-split + A11–A13 追加；B1–B8 对齐；`B4+B5` 凭据锚复位）、F-07（C7/C8 已承接）、F-08（carriedBy 归正，P2 brief ⑤ 节 7 行实测送达）、F-09（task_plan/progress/review + M8 + STATUS + INDEX 投影再生）、F-10（①②③④ 全修）、N-01（`crossEnd` symbol = `Protocol::SendStopListening`，两包 `brief-inputs --dry-run` 均产出）、N-02…N-06（簿记与措辞校正）。机械面：两包 `--check-launch ok:true / blocking:[]`、`taskpackage-required-check` Draft 态 `missing` 三件 + `coverageErrors:[]` + `warnings:[]`、`render-roadmap` 幂等、workstream PROPOSAL pin `sha1:bc53e2b9d5cf772d` 复算一致、`settings-lint` 0 RED/0 WARN、`git cat-file -e` 十件制品在 base ref 全部在场。

**sign-off：门 1 放行「human 过目切法 → launch P1（`261116-channel-resident-core`，mergeOrder 77，riskTier high）」**；LP「门 1 GREEN + human 过目切法」随本节落盘 + human 过目后由 launch 执行者回写 `verified`。本 sign-off 不授权 merge、不授权远程发布；serial WIP=1，P2 须待 P1 合 main 后按 LP4 从新 main 起分支并重取两处行号锚。零影响残留两条（LP3 evidence 自指性滞后、`_doc` 双括注）已记，不阻断。
