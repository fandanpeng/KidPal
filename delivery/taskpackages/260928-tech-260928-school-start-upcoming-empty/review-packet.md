# Review Packet — 260928-tech-260928-school-start-upcoming-empty

- 日期：2026-09-28
- 分支：dev/260915-m1-k1a-key-layer-dispatch（Pod worktree，未 push）
- 来源 CRQ：docs/crq/tech/TECH-260928-school-start-upcoming-empty.md（已收口归位 `_archive/2026/`）
- TaskPackage：delivery/taskpackages/260928-tech-260928-school-start-upcoming-empty/taskpackage.json
- riskTier：low · gateProfile：test-quality · subsystem：server/gateway/front

## 1. 变更摘要

`server/gateway/front/front_app.py` 两处 `home_upcoming_payload` 调用补传第五参
`_schedule_school_start()`（纯接线补参，与「帮我显示课表」意图展开路径既有同参对齐）：

1. `_push_schedule_connect`（建连推，原 :685）——设备 WebSocket 建连后的首页 upcoming 推送。
2. `_push_schedule_refresh_to_legs`（零点/即时 refresh 推，原 :708）——课表刷新向各腿推送的首页 upcoming 载荷。

修复语义：周一~周五在校日，建连推与 refresh 推不再因漏传 school_start 而产出空
upcoming（`items: [], preview: null`），「上学」项恢复出现在 P1 首页「接下来」组。
跨端 wire 形状零变化（payload 结构与固件 `kidpal_home_upcoming_apply` 消费面不动，
见 taskpackage.json crossEnd 申报）。

## 2. 验收齿映射

冻结验收文件：server/acceptance/test_tech260928_school_start_wiring.acceptance.py

| 齿 | 测试类 / 函数 | 覆盖 |
|---|---|---|
| TECH-A1 | `TechA1ConnectPushWiring.test_a1_connect_push_passes_school_start` | 建连推传 `school_start='08:00'`，upcoming 含合并「上学」项 |
| TECH-A1 边界齿 | `TechA1ConnectPushWiring.test_a1_connect_push_missing_config_degrades_to_none_school_start` | 配置缺失降级 `school_start=None`（must-stay-green） |
| TECH-A2 | `TechA2RefreshPushWiring.test_a2_refresh_push_passes_school_start` | refresh 推同参对齐 |
| TECH-A3 | `TechA3GridRestoreZeroOffsetGuard.test_a3_grid_restore_stays_double_none` | grid restore 载荷保持双 None（ADR-005 恢复语义，故意不改） |

## 3. 红→绿证据

- 红基线（2026-09-28 acceptance-author 实跑，2 failed / 76 passed / 2 subtests
  passed；2 红即 TECH-A1/A2 设计态红，为漏参症状本体）与绿态命令逐字：

```
PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_tech260928_school_start_wiring.acceptance.py server/acceptance/test_m3k1_schedule_producer.acceptance.py server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py server/acceptance/test_m4k1_story_cloud_link.acceptance.py server/tests/test_schedule_producer_units.py
```

- 绿态（2026-09-28 integrator 独立复核，不盲信前序，实跑输出逐字）：
  `78 passed, 2 subtests passed in 0.82s`
- verifier fact：verify.mjs scoped round 3（phase=scoped，invocationId
  e78488d8-7b1c-4f77-bde5-960b80ae541d）`ok: true`，observedGateExit 0；
  planner 独立复验 78 passed / 2 subtests。final evidence 形态按 planner 派单口径
  为 scoped round 3 fact（本包 riskTier low、单文件接线修复），非 --phase=final
  形态，如实申报。

## 4. docs-sync 状态

- CRQ 条目 `TECH-260928-school-start-upcoming-empty.md`：frontmatter
  `status: blocker → closed`，补 `closed: 2026-09-28` + `closedBy`；正文 append-only
  追加「收口记录」段（代码半已修 / 运维半移交项 / 真机复验建议），未覆盖既有行。
- `node .claude/scripts/crq-render.mjs` 重渲染：条目由渲染器自动归位
  `docs/crq/tech/_archive/2026/TECH-260928-school-start-upcoming-empty.md`
  （人不搬文件规则），主 INDEX 与归档年 INDEX 均由脚本重生成。
- 本包无契约面改动（docs/contracts/** 零触碰）；docsSyncRequired 仅涉及 CRQ 收口，已完成。

## 5. 运维半移交项（代码修绿后的移交步骤，非本 Pod 完成）

ECS 现网配置未落位前，现网行为不变（依旧空 upcoming）；以下两步完成后即生效：

1. 将 `server/deploy/openclaw-schedule.config.yaml`（school_start: "08:00"）部署到
   ECS 上 `KIDPAL_SCHEDULE_CONFIG` 指向路径（默认 `_DEPLOY_DIR = parents[2]/deploy`，
   部署布局下 = `/opt/kidpal/deploy/`）。
2. `kidpal-front.service` 加 `Environment=KIDPAL_SCHEDULE_CONFIG=...`。

`_schedule_school_start()` 每次调用现读文件，配置落位即生效、无需重启。

## 6. 遗留 / 未覆盖面申报

- 真 WS 建连与真零点调度离线不可验：验收齿以 spy/recorder 断言设备可见载荷内容，
  未覆盖真实 WebSocket 握手与 APScheduler 零点触发链路。
- 现网（ECS）配置未落位前行为不变：代码半合并后现网依旧空 upcoming，直到运维半落地。
- 周中合并「上学」项的「进行中语义」未实现（无下一事项时 end=None 走 start-only 被
  过滤）——另条目 TECH-260928-school-merge-inprogress 承接，不在本包范围。
- 真机复验建议：运维半落位后设备重连，核对建连推 payload 含
  `{"time":"08:00","name":"上学"}`。

## 7. 五轴语义 review（author 自审）

- 正确性：两处补参与意图展开路径同参，`_schedule_school_start()` 每次现读文件，
  缺配置降级 None（既有行为，边界齿守护）——正确。
- 契约/边界：payload 形状零变化，固件消费面不动；grid restore 双 None 为 ADR-005
  故意语义，未被顺手「修复」——无契约面触碰。
- 安全：无新增攻击面（参数为服务端内部配置读取，不透传用户输入）。
- 可运维性：运维半移交步骤在 CRQ 收口记录与本 packet 双留痕。
- 简洁性：改动 = 两行补参 + 一个新建验收文件，无新增抽象。

## 8. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD / NIT） | — | — | 本批未发现需登记 finding | — | — |

## 9. Simplification debt（minimal: 标记台账）

- grep `minimal:` 于 front_app.py 与新验收文件：零命中 → `no-trigger`，无简化债务台账条目。

## 10. Residual risk（decisionCoverage risk-accepted 条目）

- taskpackage.json 无 `decisionCoverage` 字段 → 无 `landsIn: dropped` &&
  `reason: risk-accepted*` 条目可列。
- 运维半移交为范围声明（taskpackage notes 明示不在本包），非 risk-accepted 决策条目；
  行为影响已列 §6。

## 11. S2 exploratory scan

- TaskPackage `s2ExploratoryRequired=false` → 未执行，无 S2 证据。

## 12. 变更影响面核对（detect_changes）

- 本分支为 Pod worktree，工作树 diff 对共享图索引不可见 → detect_changes: N/A (worktree)。
- 替代抽查：`git diff --stat` 显示 front_app.py 仅 +2/-2 行两处补参；改动符号
  `_push_schedule_connect` / `_push_schedule_refresh_to_legs` 均为本包 taskpackage
  acceptance 明确登记的目标符号；无其他共享符号触碰。

## 13.制品清单（晋升面）

- `delivery/changes/dev/260928-tech-260928-school-start-upcoming-empty/review-packet.md`（本件）
- CRQ 条目收口归档：`docs/crq/tech/_archive/2026/TECH-260928-school-start-upcoming-empty.md`（含收口记录）
- 渲染件：`docs/crq/tech/INDEX.md`、`docs/crq/tech/_archive/2026/INDEX.md`（crq-render 产出）
- 承重机器件：`delivery/taskpackages/260928-tech-260928-school-start-upcoming-empty/verifier-evidence/rounds/r3.json`（verify.mjs 产出，随包 commit）
- verdict.json / verification.json 投影件：本包 closeout 未产（scoped round 3 fact 为最高 evidence 形态，planner/supervisor 门以 fact 为准），advisory-only 缺失如实申报。
- session-digest / loop-health / loop-metrics / pod-journal：Post-EVENTS_SCHEMA_CUTOFF 不产；
  `pod-events.jsonl` 存在于包目录（advisory 检查通过，非 blocking）。
- ADR：adrRequired=false，无架构裁决，未产。
