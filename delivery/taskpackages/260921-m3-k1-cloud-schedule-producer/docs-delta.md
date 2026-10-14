# Docs Delta · 260921-m3-k1-cloud-schedule-producer

> integrator 收拢制品（2026-09-21）。本件回答「本包文档面义务是否已全清」；结论与逐面核对如下。

## 结论

- **对 TaskPackage 声明的 `docsSyncRequired=true`（docsSyncReason：契约 diff 随包同 PR 落地 + schedule.py 钉行更新；spec 零改动）：在本包责任范围内成立，义务已全清**——契约 diff 已经契约门通道（四分类预审 + human 五问裁定 HUMAN_APPROVED，packet §5）随落地批 commit `24bb1b8` 落进 `docs/contracts/interface-contract.md`，schedule.py 模块头契约钉行同批更新，CDQ 登记 + ADR-004 + gate evidence LANDED（C1–C4 全 done）同批收口。
- **spec 面（`docs/requirements.md` / `docs/architecture.md`）本包零改动**——requirements v1.19 口径（human 2026-09-14 裁课表大改）已冻结在前，本包是实现向 spec 收敛（契约向 spec 看齐的漂移对齐），需求面只消费不修改。架构面唯一相关留痕 = ADR-004（决策留痕，非 spec 正文改写）。
- **契约改动全经契约门通道**（M3K1-E2 铁律守住）：diff 以逐字 patch 经 human 乙通道授权（AskUserQuestion「授权我 git apply」，2026-09-21，留痕 gate evidence §5）一次性落盘，hunk 级过目、4 hunk 与 packet §1 触点一一对应（人工逐字核对降级，ARQ-260827-contract-mech-gate-gap 在册）。

## 文档面变更清单

### ① 契约落地批（commit `24bb1b8`，先于本包实现 commit 在分支基线）

| 面 | 文件 | 变更 | 通道 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` §1.3② | schedule view data 形状重定义：旧 `{title,items}` 列表形即废 → 周网格 `{week:{courses:[{day,slot,name,flags}]}}`（day 0 基 周一=0、flags bit0=※ / bit1=星标、备注不进载荷）+ 意图薄载荷 `data:{}` front 截获展开 + 「今天」列板端本地推导 + 行为句（V27）+ 变更记录行 | 契约门（HUMAN_APPROVED，packet §5 Q1） |
| 契约面 | `docs/contracts/interface-contract.md` §1.3⑥ | home view 增可选 `data.upcoming`（day_key/items/preview 对齐 `home_p1.h` 冻结 seam；additive，缺 data 行为不变；producer = front 机械算，Q4）+ 变更记录行 | 契约门（packet §5 Q3/Q4） |
| 契约面 | `docs/contracts/interface-contract.md` §1.5 第 3 条 / §1.6 / §2.3 | 连带改写：schedule 薄载荷截获展开 + 旧形状拒绝降级 + 零自研红线注记 / system prompt 课表段话术化 / DeepSeek/OpenClaw 课表职责行 | 契约门（packet §5 Q2/Q5） |
| 代码 doc 钉行 | `server/gateway/schedule.py` 模块头 | 旧钉行「schedule view data 形状 {title,items} 零增减、不进 UI 语义态」→ 新形状钉行 + flags 承载取代注记（packet Impact 预告的同 PR 义务） | 契约落地批随同 PR |
| 契约队列 | `docs/CONTRACT-DECISION-QUEUE.md` | `CDQ-260921-m3-k1-schedule-grid` 行登记（LANDED，随包收口同批） | packet 随包收口义务④ |
| ADR | `docs/ADR.md` | `ADR-004 · 课表数据链路收归 front 机械计算`（durable semantics 变化留痕） | packet 随包收口义务④ |
| gate evidence | `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/`（CONTRACT-CHANGE-PACKET.md / architecture-gate-evidence.md / evidence/validate_display-three-samples-20260921.txt） | Status → LANDED，C1–C4 全 done | packet 十节骨架 |

### ② 本包实现批（本包工作树，本次收口 commit 承载）

| 面 | 文件 | 变更 | 性质 |
|---|---|---|---|
| 代码 docstring | `server/gateway/schedule.py` 新增段 | 周网格 / home upcoming producer 函数 docstring（契约 §1.3②/§1.3⑥ 载荷形状逐字段钉住）+ 分界常量留痕注记（契约未钉分界时刻的显式缺口③） | doc（零行为） |
| 代码 docstring | `server/gateway/schedule_push.py`（新文件） | 模块头冻结契约 docstring（MARKER_FILENAME 跨组件协议常量 + 三触发决策面函数语义 + 时区换算约束） | doc（随实现） |
| 代码注释 | `server/gateway/front/front_app.py` 接线段 | 三触发接线注记（决策面/接线分工、主副通道判别、时区固定 UTC+8 依据） | doc（随实现） |
| 代码 docstring | `server/deploy/openclaw-patch/reminder_schedule_lib.py` | `drop_refresh_marker` docstring（marker 尽力而为语义 + S-F1.5 AC2 副通道无即时推语义） | doc（随实现） |
| workstream | `delivery/workstreams/m3-schedule/progress.md` | K1 Pod 开跑（acceptance 相位）节追加：基线重锚、intake、契约落地核对、17 齿冻齿、遗留裁决点留观 | 过程账 |
| closeout 制品 | `delivery/changes/dev/260921-m3-k1-cloud-schedule-producer/`（本件 + review-packet.md） | integrator 收拢制品落盘 | closeout |

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **契约门通道落地**（非 loop 内自改） | contract-lint 闸绿（final r5，exit 0）；CDQ-260921-m3-k1-schedule-grid = LANDED；human 签字在案（packet §5）；`24bb1b8` diff +35/−10 逐字过目 |
| spec 面·需求 | `docs/requirements.md` | 零触碰（只读消费，v1.19 口径冻结在前） | docs-sync 闸绿（final r5，`docsNoChangeReason` 在 stdout）；`git diff` 全生命周期零命中 |
| spec 面·架构 | `docs/architecture.md` | 零触碰（§5.2 场景① / §10 决策 26 为输入锚） | `git diff` 零命中；ADR-004 落 `docs/ADR.md`（裁决留痕件，非 spec 正文） |
| docRefs | `docs/requirements.md#F1` | 只读引用，无需更新——F1 课表需求行语义未变（v1.19 已是目标口径） | docs-sync 闸绿（final r5） |
| 契约队列 | `docs/CONTRACT-DECISION-QUEUE.md` | CDQ-260921-m3-k1-schedule-grid LANDED 行 | grep 实证（本件核对时逐字在场） |
| ADR | `docs/ADR.md` | ADR-004 追加 | grep 实证（:34 在场） |
| agent 定义 / gate | `.claude/**` | 零触碰 | `git diff` 全生命周期零命中（`.codebase-memory/` 机器再生面不入 commit，本包零 `.claude` 改动） |

## 核对方法

- `git diff 24bb1b8..worktree -- docs/ docs/contracts/ .claude/`：**零命中**——契约/spec/agent 面在本包实现批（`24bb1b8` 之后）零新增改动；契约落地批本身即 `24bb1b8`（已在分支基线，2026-09-21 integrator 收拢时实跑）。
- 契约面义务闭环核对：CDQ 行 + ADR-004 + schedule.py 钉行 + gate evidence §5–§10 四项逐一 grep/read 在场（2026-09-21 integrator 实证）。
- 九闸 docs-sync 闸（final r5）`ok:true`，`docsNoChangeReason` 与 taskpackage `docsSyncReason` 逐字一致。
