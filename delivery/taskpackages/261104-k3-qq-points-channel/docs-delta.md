# docs-delta — 261104-k3-qq-points-channel

## 结论：零 canonical docs 变更（docsSyncRequired=false，判断经复核成立）

本包实际 diff（`git diff --name-status 45518ac2..HEAD` 的非交付件部分）只触及：

- `server/gateway/points.py`（CLI 扩展：add/redeem/delete/show/recent/targets、稳定定位、回执补 revision/command_id）
- `server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md`（新建家长通道 skill）
- `server/tests/test_points_cli_units.py`（新增 unit 覆盖）
- `server/acceptance/test_qq_points_channel.acceptance.py`（本包冻结验收）
- `server/deploy/manifest.yaml`（登记新 skill 的 src/dst 对）

`taskpackage.changeManifest` 另列 `workspace-main/AGENTS.md`、`server/gateway/front/front_app.py`、`deploy.sh`、`verify-deploy.sh` 为预期路径；这些在最终 diff 中零改动（changeManifest 是预期集，实际 diff ⊆ 预期集，属允许）。front_app.py 零改动说明 K2 已接好的 `points_observe` / `run_points_observe_once` / `_push_display_payload` 观察腿本包直接复用，未新造推送通路——与 plan.md「观察缝已接线、无需新造 marker」一致。

## 逐面核实

### docs/requirements.md — 无需改

`acceptanceRefs` / `sourceRefs` 引用的 `S-F6.4`（AC1–AC5）、`S-F6.3`（AC3/AC4/AC5）、`S-F6.2`（AC4）、`NFR-PERF-4` 均已冻结在 requirements。本包是这些已批准需求在家长 QQ 通道上的落地实现，未改变需求语义，也未新增需求面（目标自定义/切换/删除明确留给 K4）。实现与冻结需求一致，不构成 spec 漂移。
（spec 面若将来出现漂移，按 `ARQ-260929-docs-sync-spec-handoff` 交 architect，本包零自改。）

### docs/architecture.md — 无需改

本包不引入新组件、新依赖或新数据流：QQ skill 只是把家长一句话整理成一条 CLI 调用，账本唯一 writer 仍是 K1 已落地的 points.py；下行推送仍是 K2 已接线的 front restore 观察缝。没有跨层调用、没有绕过 contracts 的本地类型、没有第二套账本。架构面无变化。

### docs/contracts/interface-contract.md — 无需改

`§1.3⑦⑧`（points 分录 / revision 语义）、`§1.5`（display 统一响应块 restore 语义）已冻结定义本包依赖的全部行为。本包新增的 CLI 六子命令与单行 `key=value` 回执是 skill↔CLI 的内部机读协议，不在 `docs/contracts/**` 声明的对外契约面上，扩字段（补 revision/command_id）不改变格式、不改动契约。故契约面零变更——也因此本包不触发 `/arch-change` / CDQ 凭据闸。

### docs/decisions/ADR.md — 无需改

`adrRequired=false`。本包无新的架构裁决，只是把已冻结决策（D-05/D-07/D-10、S-F6.3 同规则半）在 QQ 通道上实现。

### 里程碑 / workstream

`deliveryImpact=milestone-affecting`。workstream 进度已随实现更新（`delivery/workstreams/261103-star-points-story-cover/progress.md` 在 diff 内）。里程碑阶段跳变与 INDEX 投影由 `closeout-check.mjs --update-index` 统一处理，本文件不重复记账。

## 附：debt 边界（不结项）

`TECH-261030`（QQ `ask_user` 卡死）只作硬约束：新 skill 显式禁 `ask_user`、一次性普通文本报错；本包不修 QQ 插件本体，不宣称结项。`TECH-261012` 只复用课表 skill 工程先例，不结项。`TECH-261023` 的只读防线不扩为直接文件写。