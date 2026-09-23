# Docs Delta · 260923-m3-k7-schedule-grid-restore

> integrator 收拢制品（2026-09-23）。本件回答「本包文档面义务是否已全清」；TaskPackage `docsSyncRequired=false`，逐面核对与事实锚如下。

## 结论

- **TaskPackage 声明 `docsSyncRequired=false`（docsSyncReason 逐字）：「spec/契约面由上游 arch-change 同 PR 落地（commit `565cb2d`，landing-verifier GREEN）；本包为实现面，canonical 零改。」——本包责任范围内成立，无文档同步义务，义务已全清。零 docs 增量。** 本包全生命周期对 `docs/**` 零写入（`git status --porcelain docs/` 实跑零输出，2026-09-23 integrator），docs-sync 闸 r1 final 绿（stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径回显）。
- **零 docs 面改动的理由**：
  - 本包改的是**实现面**——server 侧新增 grid restore 变体 producer（`schedule_push.py`）+ 建连/piggyback 接线（`front_app.py`）+ ECS 推送开关；firmware 侧新增 restore 消费分支（`mcp_server.cc`）+ 在屏谓词（`kidpal_render_node.{h,cc}`）。契约 §1.3② 的 `data.restore` additive 布尔（L85 板子行为条款 modifying + 生产纪律句）**已由上游 arch-change 同 PR 落地**（commit `565cb2d`），本包只按该已冻结条款消费、不改一字。
  - spec 面（`docs/requirements.md` S-F1.2 冷启动恢复句 / `docs/architecture.md` §5.2 场景① / `docs/ADR.md` ADR-005）同一上游 commit 落地；本包实现向已冻结规格与 ADR-005 收敛，**零漂移**。
  - docRefs 三锚在世且零修订：`docs/requirements.md#S-F1.2`、`docs/architecture.md §5.2 场景①`、`docs/ADR.md ADR-005`。
  - 实现细节由代码注释（`schedule_push.py` 开关/变体语义头注、`front_app.py _push_schedule_connect` 契约锚注、`mcp_server.cc` restore 分支契约 §1.3② 注、`kidpal_render_node.h` on-screen 谓词 ADR-005 R10 注）与 closeout 制品（本目录 `review-packet.md` + 本件 + `verdict.json`）承载，不另立 docs 文档。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` §1.3②（contractRefs 唯一条目） | **零触碰**（上游 commit `565cb2d` 已落，铁律 K7-E2；只读消费） | 只读 |
| spec 面·需求 | `docs/requirements.md`（docRefs `#S-F1.2`） | 零触碰（上游已落，既有冻结需求） | 只读 |
| spec 面·架构 | `docs/architecture.md` §5.2 场景①（docRefs） | 零触碰（上游已落） | 只读 |
| ADR | `docs/ADR.md` ADR-005（docRefs） | 零触碰（上游已落，decision 面无需新 ADR——实现按已裁 Shape A 落地，无新架构裁决） | 只读 |
| workstream 过程账 | `delivery/workstreams/m3-schedule/progress.md` | integrator 追加 K7 段落账（遗留③/症状 B host 半闭环销账） | delivery 过程账 |
| closeout 制品 | `delivery/changes/dev/260923-m3-k7-schedule-grid-restore/`（本件 + `review-packet.md` + `verdict.json`） | integrator 收拢制品落盘 | closeout |

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/` 零输出（2026-09-23 integrator 实跑）；contract-lint 闸绿（r1 final，exit 0） |
| spec 面零写入 | 同上零输出；docs-sync 闸绿（r1 final stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径） |
| 协议/冰冻面零改 | `git diff --name-only` 变更面 7 文件全在 `firmware/**` + `server/gateway/**`（见 `review-packet.md` Change Size）；`schedule.py`（承载 `expand_schedule_intent_payload` 输出语义）、`device_state_machine.cc`、`protocol.h` 零 diff（K7-E2 铁律逐项复核见 review-packet） |
| docRefs 锚在世 | 三锚文件在世（requirement/architecture/ADR）且本包零修订；ADR-005 §1.3② 语义在两侧实现落地一致（server producer 置位 ⇄ firmware 消费分支，cross-end 抽查见 review-packet） |

## Mutation evidence 落点

- `delivery/taskpackages/260923-m3-k7-schedule-grid-restore/mutation-evidence/`（7 件，planner 代落——builder scope 限制）。逐条 content 与实测红 tooth 对照见 `review-packet.md`「Mutation replay 逐条对照」节。
- 覆盖差异如实记：plan §4.1.2 要求「逐 acceptance 原始输出落 mutation-evidence/（逐 acceptance 一文件）」；实落 7 件 = K7-A1 / A3 / A5 / A6 / A7 / A8 / E2，**K7-A2 与 K7-A4 无对应原始输出件**（二者 mutation + expectedRed 仅存 taskpackage `mutationEvidence[]`）。此项作为 SHOULD finding 记入 review-packet，交 planner 补跑或明示降级。

## 授权留痕（实现面单点偏离）

- **`firmware/main/display/schedule_page.c` 的 `ksp_copy_str` 增 `memset(dst, 0, cap)` 零填充**：**planner 扩展授权**（代码内注释逐字载「planner 扩展授权（260923-m3-k7 裁决，memcmp 确定化）」）。该行超出 plan.md §3 变更面清单（清单只列 `mcp_server.cc` + `kidpal_render_node.cc`）。
- 必要性：frozen K7-A7 保真齿以 `memcmp(got, ref, sizeof(ref[0]) * n)` 逐字段比对解析产物（`schedule_grid_restore.acceptance.c` TEST 4 tooth 2）；`kidpal_schedule_course_t` 的 `name` 尾部字节此前取自未初始化栈垃圾，令整 struct 的 memcmp 不确定。零填充使尾部确定化，字符串消费面（strlen/strcmp/渲染取 NUL 截断）行为中性——**属实现必要性、非顺手修**。
- 轻量 scope 修正通道四条件满足：① 同包边界内；② 零新依赖；③ neverTouch（`docs/contracts/**`/spec 冻结面）零触；④ 冻结验收件零改。程序要件「planner 带时间戳补登记 taskpackage」**当前缺**（taskpackage.json/plan.md 均无该条目），作为 NIT finding 记入 review-packet。