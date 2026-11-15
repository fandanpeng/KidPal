# Review Packet

## Change Size
- **Files changed（实现面 `server/**`）**：5 files，+2046 / −56
  - `server/gateway/points.py` +370 / −56（CLI 六子命令、稳定定位删除、回执补 revision/command_id）
  - `server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md` +128（新建家长通道 skill）
  - `server/tests/test_points_cli_units.py` +594（新增 unit）
  - `server/acceptance/test_qq_points_channel.acceptance.py` +947（本包冻结验收 K3Q-A1..A7）
  - `server/deploy/manifest.yaml` +7（登记新 skill src/dst 对）
- **交付/面簿记件（非实现 diff）**：taskpackages 目录制品、`current/` 投影、workstream `progress.md`。
- **Net diff（全量 base..HEAD）**：17 files，+2932 / −424。
- **Scope**：`server/gateway/points.py`（K1 唯一 writer 之上扩 CLI）、新建 `kidpal-points-write` skill、unit/acceptance、manifest 登记。`changeManifest` 另列 `workspace-main/AGENTS.md`、`front/front_app.py`、`deploy.sh`、`verify-deploy.sh` 为预期路径，最终 diff 零改动（预期集超集，允许）。front_app.py 零改动 = K2 已接线的 `points_observe`/`run_points_observe_once`/`_push_display_payload` 观察腿被直接复用，无新造推送通路。
- **Split recommendation**：不建议拆分。逻辑上单一目的（家长 QQ 通道正式化），但体量偏大（+2046）主要来自冻结验收（947）与 unit（594）的测试面；实现本体（points.py +370）内聚且边界清晰。

## Verification Evidence
- **Structural gates**：final 相位 10 闸全 PASS —— taskpackage-required / change-manifest / firmware-build（真编译）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance。
- **Acceptance/unit**：`unit` = 218 passed, 4 deselected；`acceptance` = 84 passed, 4 deselected（7 server 验收件；`liveDeselected=4`、`filesWithLiveMark=1/7`）。deselect 的 4 条 live-manual 即 K3Q-L1 真实 QQ 轮，见「未覆盖面申报」。
- **Security gate**：`securityReviewRequired=false`，本包无新增外部依赖、无凭证/网络面变更。
- **Mutation check**：`mutationEvidence.perAcceptance` 7 条 acceptance 各一（实放 10 处变异，sha256 前后还原，终态 20 acceptance + 35 unit 全绿）。
- **Docs sync**：`docsSyncRequired=false`，经复核成立（零 canonical docs 变更，理由见 `docs-delta.md`）。
- **test-scope 清单落盘**：`delivery/taskpackages/261104-k3-qq-points-channel/test-scope-final.txt`（10 件：本包验收 + 新 unit + 7 impactedTestsSeed + NFR-PERF-4 共享 seam `test_perf_k1_roundtrip_latency`）。final fact = `verifier-evidence/rounds/r1.json`（round 1，invocationId `e45bfeb2-a3a2-4758-80e9-2594d64337ba`，observedGateExit 0，factOk true；`verifier-evidence-check.mjs` exit 0）。

## Simplification Debt
| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | no-trigger | — | 新增代码零 `minimal:` 标记 |

## Findings
| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | simplicity | `_cmd_show` 经 `read_state` + `get_active_target` 两次加载状态；`_emit_entry_receipt` 写后另做一次 `read_state` 取 revision。CLI 单进程单次调用，成本可忽略。 | changed-code hunk（points.py `_cmd_show`/`_emit_entry_receipt`）+ 判断证据 `deletion test`（合并双读不会消除复杂度，只会让只读/写入路径耦合）/ `yagni` | accept，无动作（可选：若未来 CLI 变为高频内联调用再合并） |

无 BLOCKING / SHOULD finding。

## Five-Axis Review
1. **正确性**：CLI 退出码三分（0 成功含去重/删除 no-op；1 语义拒绝零写入；2 用法/环境错误）与 acceptance K3Q-A1/A2 一致；序号→id 换算与删除同处 `_critical` 临界区（`_delete_entry_locked`），删除重放判定在序号换算之前，堵住 `--index` 重放的真实误删面；add/redeem 与 delete 回执分命名空间（裸 command_id vs `delete:<command_id>`），`_replay_receipt` 不会读到删除回执，病态撞键降级为不持久并告警，不覆盖 add/redeem 幂等。既有 `add <points> <reason>` / `redeem <points>` 位置参数形态零放宽（K0/K1 调用向后兼容）。
2. **可读性/一致性**：回执恒单行 key=value，`_cli_value` 只编码空白，扩展字段不换格式；docstring 与 acceptance 断言同一口径；`_parse_flags` 手写解析以便 `--points -3` 进语义拒绝而非被当未知选项。SKILL.md 与 CLI 的表驱动描述一致（子命令面、回执字段、revision/command_id 语义）。
3. **架构与边界**：唯一 writer 仍是 points.py，QQ skill 只调 CLI，不直写 state.json、不写 schedule.md、不碰 workspace-kidpal 只读面；下行推送复用 K2 restore 观察缝，front_app.py 零改动；未引入新依赖、未绕过契约的本地类型。
4. **安全**：state 路径单源 = `KIDPAL_POINTS_STATE`，未设即 exit 2（不建第二账本）；拒绝路径锁外挡定位符、零文件副作用；SKILL.md 负向钉死（禁 ask_user、禁直写 state、无 schedule.md 写路径残留、workspace-kidpal 只读），并有负向断言（K3Q-A5）。`securityReviewRequired=false`。
5. **简洁性（含过度设计，第 5 轴）**：无 `minimal:` 标记；新增均为需求硬性面（六子命令 + 幂等 + 稳定定位），无投机抽象。一条 NIT（双读）见上。

## Residual Risk
- **Known limitation**：本包全部证据为离线（server pytest + 静态纪律 + 离线 restore 串链）；真实 QQ 家属通道往返、`ask_user` 插件级不卡死、NFR-PERF-4 实测时延、设备其他页不抢前台/积分页原地刷新均未在真机与真实 QQ 上验证（见「未覆盖面申报」）。
- **Follow-up recommended**：ECS 部署轮（铺 skill + points.py，验 manifest 表驱动生效与 verify-deploy md5）由主线程组织；真实 QQ + 设备轮 K3Q-E4/K3Q-L1 由 human 在场执行并回填 `device-round-evidence.md`（若触发 deviceOutcome）与 live 证据。
- **Human decision needed**：无（合并门由 supervisor 机械闸复核；无高风险残留需人类裁决）。
- **decisionCoverage 中 `landsIn:dropped` + `risk-accepted` 条目**：none（本包 decisionCoverage 全部 `landsIn` 为 exitCriteria/package，无 dropped/risk-accepted）。

## demo 对账行（对照 `delivery/workstreams/261103-star-points-story-cover/roadmap.md#261104-k3-qq-points-channel`）
| roadmap 效果 / demo 证据形态 | 结论 | 证据 / 去向 |
|---|---|---|
| `server/skill 离线验收`（CLI + skill 静态 + 部署三面） | **已有离线证据** | final verify：unit 218 / acceptance 84；K3Q-A1/A5/A7 |
| 家长 QQ 加 / 兑 / 查 / 删四类动作 | **未跑（真实 QQ）**；离线等价在 | CLI 六子命令 + 回执字段精确（K3Q-A1）；加兑查的离线等价由 unit + A6 串链覆盖 |
| 稳定 id / 最近列表序号删除误记 | **已有离线证据** | K3Q-A4 + unit DeleteLocator/DeleteReplay + 源锚锁定 |
| 歧义与重复输入的一次性处理、不卡在追问 | **未跑（真实 QQ 往返）**；离线单边在 | SKILL.md 禁 ask_user 负向断言（K3Q-A5）+ CLI 拒绝 exit1/stderr/零写入（K3Q-A2）+ command_id 透传幂等（K3Q-A3） |
| 设备其他页只更新数据 / 积分页原地刷新（看屏/串口） | **未跑（设备轮 K3Q-E4）**；离线半在 | K3Q-A6：tmp state 经 CLI 驱动 `run_points_observe_once` 恰推一帧 points_home、restore=true、revision/balance 权威 |
| front / points / QQ 三源日志按 command_id / 分录 id 对账 | **未跑（真实三源日志）** | command_id 透传幂等与回执字段在离线侧就绪；三源对账需真实 QQ + 设备轮 |
| NFR-PERF-4 QQ 消息→积分页原地刷新的实测时延（不写死阈值） | **未跑（真实 QQ + 设备）** | 离线仅有通路存在半（K3Q-A6）；实测值留真实轮记录 |

## 未覆盖面申报（对照 `acceptance[]` K3Q-A1..A7 + `contractRefs` §1.3⑦⑧/§1.5 + roadmap 效果段）
- **真实 QQ 家属通道往返（加/兑/查/删各一笔）** — 本次无断言 — 现状：离线仅等价 CLI/串链验收 — 去向：human 在场真实 QQ 轮（K3Q-E2/E3，live-manual K3Q-L1）。
- **`ask_user` 插件级不卡死、无 900 秒卡死、无后续消息排队** — 本次无断言 — 现状：仅离线断言 skill 文本禁 ask_user + CLI 一次性拒绝 — 去向：真实 QQ 轮 K3Q-E3。
- **NFR-PERF-4 QQ 消息→积分页原地刷新实测时延（不写死阈值）** — 本次无断言 — 现状：未跑，仅通路存在半 — 去向：真实 QQ + 设备轮 K3Q-E4 记录值。
- **设备其他页不抢前台 / 积分页原地刷新（真机看屏/串口）** — 本次无断言 — 现状：未跑，仅离线 restore=true 单帧推送 — 去向：设备轮 K3Q-E4。
- **command_id 平台消息 id 实际可得性（TECH-261023 工具面 prompt-only/只读）** — 本次无断言 — 现状：降级形态 `qq:<ts>:<hash>` 已写死并可对账 — 去向：真实 QQ 轮观察平台 id 是否可得。
- **ECS 部署轮（铺 skill + points.py；manifest 表驱动生效、verify-deploy md5）** — 本次无断言 — 现状：离线只验 manifest 条目与表驱动（K3Q-A7） — 去向：**归主线程** ECS 部署轮。
- **front / points / QQ 三源日志按 command_id/分录 id 对账** — 本次无断言 — 现状：未跑 — 去向：真实 QQ + 设备轮 K3Q-E4。

> **明确申报**：真实 QQ 家属通道往返、`ask_user` 不卡死、NFR-PERF-4 实测时延、设备其他页不抢前台/积分页原地刷新 = **未跑**，待 human 在场的真实 QQ + 设备轮（K3Q-E4 / K3Q-L1）；ECS 部署轮（铺 skill + points.py）归主线程。本包不以离线绿替代以上任一 live 断言。