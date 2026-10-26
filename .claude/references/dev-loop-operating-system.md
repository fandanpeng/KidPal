# Dev Loop OS — 开发期操作系统（canonical reference）

> 状态：canonical dev-loop 治理参考。落定自 `delivery/review/dev-loop-os-rte/PROPOSAL.md`（FROZEN v2.1，2026-07-07 对抗 review 通过；ARQ-064~068）。
> 归属：平台 / architect 维护（`.claude/references/**`）。本文件定义**开发过程**的操作系统；**公司运行时**编排见 [`docs/kernel/K9-orchestrator.md`](../../docs/kernel/K9-orchestrator.md)。
> 协调机制（blackboard / merge token / reconcile）细节见 [`.claude/references/pod-coordination.md`](pod-coordination.md)。

## 1. 是什么 / 不是什么（与 K9 的边界）

Dev Loop OS = 开发期 coding-agent 平台（GitHub Copilot / Claude Code / 未来 dev-loop runner）。它拥有：worktree、TaskPackage、acceptance/builder/verifier/fixer/integrator/supervisor、Pod 调度、merge 证据、复盘回路。

它**不是** K9。K9 是公司**运行时**编排者：消费业务事件、持 6 业务门状态、消费 K6 `DefectDetected` 并写 repair-intent（`core.Defect.status` 队列态）。K9 **写并独占** `Defect.status`；Dev Loop OS **消费 repair-intent 但不写** `Defect.status`。开发过程的编排属 Dev Loop OS，不属 K9。

| | K9（运行时编排） | Dev Loop OS（开发期编排） |
|---|---|---|
| 触发 | 业务事件 / `DefectDetected` | TaskPackage / repair-intent / arch-review handoff / drift / CI |
| 持有 | 6 业务门状态、`core.Defect.status`、repair-intent 队列 | Pod 状态、merge token、TaskPackage、耐久证据 |
| 不做 | worktree/Pod/verifier↔fixer/merge 门 | 写 `Defect.status`、跨 6 业务门 |

> 平台硬约束：Claude Code 子 agent 不能再派生子 agent，每个 planner/portfolio-planner 都是主线程。多 Pod fan-out 只能由 human / 外部 script / CI launch，不是 in-session agent（同 K9 §5.1 控制反转）。

> **非 Claude harness 降级 profile**：本 OS 的机械保全（hooks / verify-gates / scope-map / token telemetry）默认假设 Claude Code harness；在其它 harness（Kimi CLI 等）上这些门不自动触发。降级时的「必然失效面 / 约定替代 / 不许降级面」三清单见 [dev-loop.md「非 Claude harness 降级 profile」](dev-loop.md)（gov-review-devloop-closeout-hardening §3.4）。降级是证据事实、不是违规，但承重闸与 human 触点不可跳过。

## 2. 角色

| 角色 | 职责 | 不做 |
|---|---|---|
| `portfolio-planner`（RTE） | 跨 Pod：Decomposition（拆成 N 个排好序 TaskPackage + 依赖）+ Scheduling（WIP limit、merge order、Pod 状态、merge token）。创建/挂接 `delivery/workstreams/<workstream>`。 | 不驱动内层 loop、不写实现、不自己 launch Pod、不 merge、不常驻 |
| `planner`（收窄） | 单 Pod 编排者：消费**一个** TaskPackage，驱动 acceptance→builder→verifier↔fixer→integrator→supervisor。只往其 workstream append progress。supervisor gate 之前收尾时写 `delivery/taskpackages/<change-id>/pod-retro.md`（**第一人称热记忆**，planner 自著；非「Pod 自总结」——该标签唯属 integrator 的 loop-health）。 | 不做跨 Pod 拆分/调度、不再创建顶层 `delivery/workstreams/<workstream>` |
| `integrator` | 收拢已绿 Pod、解冲突、范围圈定终验（scope 圈定）、docs-sync、Review Packet、closeout；pre-cutoff 兼容产 session-digest + loop-health，post-cutoff 不产这两项。 | 不 push、不 merge 主干 |
| `supervisor` | per-Pod 合并门：只读复核证据，低/中风险放行，高/critical 升级人类。 | 不写实现、不 push |
| `architect` | Dev Loop OS 治理 owner：处理 retro/loop-health 产出的 architect-owned ARQ，裁定是否改 Dev Loop OS 定义/agent/gate/边界。 | 不实现、不批契约、不 merge |
| loop-retro（cadence 复盘） | 跨 Pod 按 cutoff 读 pod-events + pod-retro 或 legacy loop-health，提取共性 → architect-owned ARQ。 | 不重算单 Pod 健康；不自我批准、不直接开 TaskPackage |

> `portfolio-planner` 与 `planner` 是**主线程兄弟**，靠 repo 制品协调，不靠 agent 嵌套、不靠 agent 间消息。agent 定义见 [`.claude/agents/portfolio-planner.md`](../agents/portfolio-planner.md) 与 [`.claude/agents/planner.md`](../agents/planner.md)。

## 3. 控制流（scoped + full 分层）

> gov-review-devloop-evidence-led C1 (FROZEN 2026-07-24)：verifier 每轮 scoped（受影响子集 + 污染组），终审 full（按 scope-file 影响面圈定跑；scope-file 缺失红闸举手；全量功能套件 = 人工触发位，260928-gov-review-dev-loop-preassess-manifest-scope T10/T12 修订）。省量 conditional ~15-20% per-round（DL1 量化，pending P5 实测）。

```text
inputs: delivery/milestones milestone / delivery/workstreams workstream / arch-review handoff / drift / CI / K6 repair-intent
  -> portfolio-planner: intake + decomposition
       读 delivery/workstreams 作为上下文（避免重切已完成）
       产出 N 个排好序 TaskPackage + 依赖
       跨多切片时创建/挂接 delivery/workstreams/<workstream>，给每个子 TaskPackage 写 planningRefs
  -> portfolio-planner: scheduling
       WIP limit、merge order、Pod 状态 board 初始化（--set-pod queued）
  -> launch (script/CI/human)
       launch 前置门（pod-coordination §2b）: git cat-file -e 校验基准 ref 含 taskpackage+PROPOSAL（blocking）/ decision-coverage（warn），缺失拒绝 launch
       TaskPackage ready 即就运行时 worktree（复用空闲树 ∨ 按需新建，一树一常驻分支——pod-coordination §2c 换包协议；自含拆包产物的专用 base ref）+ 起 planner 会话
  -> planner (single Pod) × N
       各自跑 acceptance→builder→verifier↔fixer→integrator→planner 收尾 pod-retro→supervisor（Pod 内完整闭环）
       pod-retro 相位对齐 §5b：integrator closeout 之后、supervisor gate 之前（第一人称热记忆；post-cutoff closeout 完整性条件）
       verifier 每轮: scoped（结构闸 + 受影响子集功能闸 + 污染组）
       verifier 终审: full（按 scope-file 影响面圈定，merge-readiness；缺失红闸举手）
       supervisor APPROVE → 标 ready-to-merge
  -> merge-driver (planner 在 supervisor APPROVE + 前置升级闸放行后自动触发，单 Pod 用 --pod)
       按 mergeOrder 串行消费 ready-to-merge Pod：
         拿 token → rebase 到当前 main → quick-check → merge → 释放 token → 下一个
       下一个 Pod 必须 rebase 到含上一个 Pod 的新 main，再进门
     -> loop-retro：按 cutoff 读 pod-events + pod-retro 或 legacy loop-health，跨 Pod 提共性
  -> architect ARQ（若要改 Dev Loop OS 定义）-> 回流 portfolio-planner intake
```

## 4. session-digest（integrator closeout 产出）

> **Dual-format**：本节仅适用于 changeId date-prefix `< 250725` 的 legacy Pod。Post-cutoff 已退役 `session-digest.md`；human decision/correction 由 §5d pod-events 结构化承载，叙事由 pod-retro 承载。历史文件不删除、不重写。

落 `delivery/taskpackages/<change-id>/session-digest.md`。**只承载 machine artifacts 捕捉不到的对话/纠偏/决策内容**：

- `humanQuestions` / `humanCorrections` / `decisionDeltas` / `followUpWork` / `crossDeviceInputs` / `unresolvedConcerns`
- `artifactRefs`（指向 taskpackage / test-evidence / review-packet / supervisor-verdict / verify logs）

**不复制** verifier/supervisor 证据或 Review Packet 正文——那些是 machine artifacts；本 Pod 的 loop-health（§5）直接引用原件，digest 只补 machine 抓不到的对话/纠偏维度，二者共同喂本 Pod loop-health，避免造第二事实源。不保存原始 session 全文。跨设备交接时发送方必须提供 digest。

## 5. 每 Pod loop-health（integrator closeout 产出，Pod 自总结）

> **Dual-format**：本节仅适用于 legacy Pod。Post-cutoff 已退役 committed `loop-health.md`，需要时从 pod-events + immutable verifier evidence on-demand 聚合；历史 loop-health 继续可读。

层级：**loop 过程交付物 + session-digest → 每 Pod loop-health → retro 跨 Pod 提共性**。digest 是 loop-health 的**输入**，不与它平级。

每个 Pod 在 closeout 由 integrator 产出 `delivery/taskpackages/<change-id>/loop-health.md`——**该 Pod 对自己这趟 loop 的自总结**，源自本 Pod 的 loop 过程交付物（taskpackage / test-evidence / verifier↔fixer 轮次与 fingerprint / review-packet / supervisor-verdict）+ 本 Pod 的 `session-digest.md`。内容：结局（verdict）、verifier↔fixer 轮数与重复 fingerprint、各 gate 结果、逃逸/false-green 风险、human-correction 主题（自 digest）、简化债、本 Pod 踩到的流程摩擦。

`.claude/scripts/loop-health-report.mjs <change-id>` 抽该 Pod 的**确定性机器信号**（verdict、制品完整度、riskTier、digest 信号旗标）给 integrator 写 `loop-health.md`，避免手搜数字。**缺 digest 或缺标准制品 → 该 Pod loop-health 只能 partial/advisory**。`loop-health.md` 随 Pod worktree、合并进 main（耐久证据）。

## 5b. pod-retro（planner 收尾产出，第一人称热记忆）

由 **planner（Pod 主会话）** 在收尾时写 `delivery/taskpackages/<change-id>/pod-retro.md`——**第一人称热记忆**：全程在场的 planner 对这趟 loop 的主观过程体感（卡点、verifier↔fixer 绕轮原因的主观判断、别扭的 acceptance、帮倒忙的钩子/规则、人工纠偏体感、下次改进）。时机：**supervisor gate 之前、integrator closeout（§5）完成后**（上下文仍热；supervisor verdict 不新增过程体感，与原「APPROVE 后」几乎等热）。与 loop-health / session-digest / supervisor-verdict 同 Pod 证据簇；多 supervisor 轮 Pod（BLOCK→fixer→重验）像 loop-health 一样**每 closeout 相位重写、最后一轮为准**。

**与 session-digest / loop-health 的关系（守冻结 §4/§5，sibling 非输入）**：
- `pod-retro` 与 loop-health 是**同一 Pod 的两种自总结视角，互为补充、非源-投影**。理据 = **作者分离 + 冻结输入集**：loop-health 由 integrator 自 artifacts + digest 综合（冻结 §5 输入集**不含** pod-retro），pod-retro 由 planner 自著，integrator 不读 planner 的 pod-retro——故 loop-health **不引用、不依赖** pod-retro。
- `session-digest → loop-health` 冻结层级（§5）不动；**pod-retro 不进该层级**（sibling 非输入）。
- **负向边界（防第二事实源）**：pod-retro 只记第一人称体感；轮数 / gate 结果 / verdict / false-green / fingerprint 等客观字段以 loop-health + machine artifacts 为权威，pod-retro 不复述、不冠权威、不载客观机器字段；不复制 session-digest / loop-health 正文。

**Post-cutoff 收敛内容边界**：pod-retro 是唯一人读 retrospective，只含 root-cause/process judgment、recurring friction、escalation themes（按 eventId 引用 pod-events）、human-correction themes、unresolved concerns 与 improvement routing。MUST NOT restate tokens、gates、rounds、verdict 或 test results。

**retro 兼读 + 生命周期**：retro（§6）跨 Pod 提共性时**兼读** loop-health（综合视图）+ pod-retro（第一人称摩擦/纠偏主题）；pod-retro 缺失不阻 retro（loop-health 仍在），仅 advisory 告警 + 优雅降级。`pod-retro.md` 随 Pod worktree 合并进 main（耐久证据，对偶 §5 loop-health 生命周期）。

## 5c. 两车道过程记录（机器数据车道 + 叙事车道层 A）

为给每个 dev loop 收尾自动产可聚合的「时间去哪了」数据与最小过程叙事，引入**两条独立附加车道**（纯观测、fail-open、不改任何闸逻辑）：

### 5c.1 机器数据车道（verification.json 家族）

> Post-cutoff：`dispatch-log.jsonl` 与 committed `loop-metrics.json` 退役；dispatch telemetry 写入 §5d pod-events，metrics 仅 on-demand 输出。`verify-runs.jsonl` 在 `verifier-evidence/` 中保持不变。下表保留 legacy 生命周期定义。

| 制品 | 生产者 | 生命周期 | 说明 |
|---|---|---|---|
| `verify-runs.jsonl` | `verify.mjs`（每次 run 双写追加） | 过程态（`delivery/taskpackages/<id>/`），不随 main 合并 | per-run gate 结果 + phase/caller + timestamp；失败路径也写（:74 exit 前） |
| `dispatch-log.jsonl` | `dispatch-stamp.mjs`（planner 每次 Agent() dispatch 收工后调用） | 过程态（`delivery/taskpackages/<id>/`），不随 main 合并 | harness 回传值：durationMs / tokens / toolUses / terminalStatus |
| `loop-metrics.json` | `loop-metrics.mjs`（integrator closeout 聚合） | **耐久派生**（`delivery/taskpackages/<id>/`） | 四桶耗时 + 轮数 + 每闸 pass/fail+耗时/reran + diff 大小。`authority:derived` + `sourceRefs`，与权威源分歧时以权威源为准（DP-F1） |

`loop-metrics.json` 的 `gates[].reran` 能暴露 fixer 跨轮重跑的浪费源（fail-fast 截断使前缀闸重跑多、尾闸重跑少），但这是观测信号，**不进任何 pass/fail/verdict 判定链**（§2 不消费不变量）。

### 5c.2 叙事车道层 A（pod-journal.jsonl）

> Post-cutoff：`pod-journal.jsonl` 退役；friction 进入 §5d pod-events，derived round-outcome 不再记录。下文仅描述 legacy Pod。

`pod-journal.jsonl`（每 Pod 一份，append-only）承载机器数据车道装不下的过程叙事，用 `artifactRefs` **引用**机器数据车道原件、**不吸收、不复制**。事件类型闭集 = {`round-outcome`, `friction`}。

- **`round-outcome`**：`{round, role, ok, authority:"derived", sourceRef:"verify-runs.jsonl", caught}`。`ok` 是 verify-runs 的派生投影（非第二事实源），`caught` 是 bounded 自由文本（「这轮抓到啥」，锚 verify-run）。
- **`friction`**：`{frictionType: stall|scope-drift|scan-miss|degraded|over-scan, summary}`。机器触发（`stall` 由 dispatch-log 的 durationMs/toolUses 比启发式触发）+ planner 一句人注。**纯 advisory、绝不当门**（DP-D4）。**`over-scan` 触发定义（T7，260908-gov-review-input-surface-frontload；文件名 261015 批 T4 单份化）**：子 agent 交付报告的超读申报段（`input-brief.md` 第④节表）**非空** → planner 落一条 `over-scan`，summary 引申报行数与代表文件（如「builder 超读 6 件：docs/notes/x.md 等」）——申报 → 记账 → 仪表盘可见是同一条闭环；与 `scan-miss`（该扫没扫）语义相反（不该扫扫了），独立型才可分向统计。**记账不拦截**：超读本身是常态动作，本型的目的是让「输入面切漏了什么」在 cadence 复盘里可聚合，不是给子 agent 加门槛。

生命周期：过程态（`delivery/taskpackages/<id>/`，planner 通过 `journal-append.mjs` 追加）→ closeout 由 integrator **晋升**到 `delivery/taskpackages/<id>/`（write-once-at-closeout，DP-FD9）。无 hash-chain / journalHeadHash（层 A 不上重机器）。retro 游标复用现有 change-id ledger（`loop-health-report.mjs --pending`）；`pod-journal.jsonl` 存在性**不进 `computeRetroScope` gating**（游标仍只 key `loop-health.md`，DP-FD8；human-approved 销账/excluded 是正交覆盖层，移出触发计数但不改此可析出性 gating——见 §6 游标第三态）。

**层 A / 层 B 边界**：层 A 纯附加、不碰 FROZEN（session-digest / loop-health / pod-retro 全部原样保留、原样为权威）。层 B（让 Journal 取代 FROZEN prose——`planner-retro` 迁入 Journal 等）碰 FROZEN，须独立 review，本批不做。

## 5d. pod-events（post-cutoff 结构化观测 ledger）

`EVENTS_SCHEMA_CUTOFF = '2026-07-25'` 是唯一 cutoff SSOT，定义于 `pod-events-append.mjs`。changeId date-prefix `>= 250725` 的 Pod 使用 `delivery/taskpackages/<changeId>/pod-events.jsonl`，每行 envelope 为 `{schemaVersion:1,eventId,changeId,timestamp,eventType,actor,source,payload}`；eventId 为 UUID，eventType 闭集：`dispatch-completed | agent-escalation | agent-escalation-resolved | human-decision | human-correction | friction`。

- `dispatch-completed.tokenUsage` 按**来源分档**（260920-gov-review-dispatch-telemetry-source FROZEN v1.0）：
  - `source: 'transcript'` —— **仓内 token 归因最忠实记录**（模型 API 回传的逐消息 usage，四路齐全），由 `dispatch-stamp.mjs` 按 `--dispatch-id`（= harness agentId）自动派生。**它不是计费真值**——provider 计费可因缓存折扣等与之不同，计费对账不在此范围。
  - `source: 'harness'` —— harness 回传的**单值**（`--token-total`；legacy 面 `--tokens`），口径未文档化、与逐消息账对不上，**降为回退值与交叉核对**（存 `harnessReported`），不再作为唯一权威。
  - 取值优先级：transcript → 旗标 → `unavailable`。命中时 `status` 必须为 `'exact'`（`status` 是既有消费方的唯一分桶键）。
- `total = input + output`（两者都有时），cache 字段不重复计入；unknown 绝不写 0，genuine zero 必须带 `note: reported-zero`。
- `dispatch-completed` payload 必带三键（261024-gov-review-retro-efficiency-instrumentation T3，全部 fail-open）：`model` 恒在场（字符串或 `null`，缺值配 `modelStatus: 'unavailable'` + `modelReason`；transcript 生效模型与 `--model` 不一致记 `modelFlagMismatch`）；`durationMs` 恒在场（非负整数或 `null`，缺值配 `durationStatus: 'unavailable'` + `durationReason`；真实 0 带 `durationNote: 'reported-zero'`）；`tokenUsage` 恒在场（`exact` 必有 `total` 且 `total = input + output`；取不到写 `unavailable` + `reason`）。缺值一律显式 unavailable，不以缺键或 0 冒充。
- `mainThreadUsage` 检查点语义：同一会话内**单调累计**、每次 dispatch 刷新；写入侧发现本条累计值不高于上一条时记 `cumulativeNote: 'non-increasing'` 并 stderr 告警（fail-open，不阻断派发）。planner 成本按同 Pod 首末检查点做差；首末差 ≤0 视为失效，不得写 0。
- **降级纪律**：transcript 取不到时按既有路径降级为 `unavailable`，**不得因遥测缺失而阻断或延迟派发**；遥测是记账面，不是门。
- subagent Red Flag / blocker / out-of-scope / human ruling request 的顺序固定为：detect → best-effort append `agent-escalation` → 立即 stop/escalate。Resolution 另 append `agent-escalation-resolved` 引用原 eventId，不改写旧行。Append 失败仅在 response 暴露 advisory，并在可写时记 pod-retro degraded note；不得延迟实时升级。
- human decision/correction 由 planner 在收到或检测到 human input 后 append。Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威。
- pod-events observational、non-bearing、fail-open：malformed 行诊断后跳过，缺失不阻 verifier、supervisor 或 merge。聚合 token 时只求和 `status=exact`，并始终报告 `{dispatchesTotal, dispatchesWithTokenData, dispatchesUnavailable}`。
- **backfill 约定（gov-review-devloop-closeout-hardening §3.2 / Q4，convention-only）**：晚 append（非实时）的事件须标 `source=<actor>-backfill` + `payload.approx` 载估时（可选 `payload.backfillReason`）。**backfill 事件的 ledger 时间戳 ≠ 事件真实发生时间**——retro 消费时按 `payload.approx` 读时序，不信 ledger 时间戳。是否给 `pod-events-aggregate.mjs` 加 backfill 比例机器校验 = convention-only（fail-open 冲突；aggregate 全归 `unknown` 桶本身已是机器信号，不另加门）。

## 6. retro → architect-owned ARQ

cadence review（[`.claude/commands/loop-retro.md`](../commands/loop-retro.md)）按 exclusive cursor dispatch 读取：`changeId >= 250725 && pod-events exists` → new-format（pod-events + pod-retro）；否则 `loop-health exists` → legacy format；否则 waiting。一个 Pod 只进一条路径，两类文件并存时 new-format 优先，禁止 double-count。New-format 用 on-demand aggregation 提取 token coverage、escalation 与 friction themes；legacy 保留下文既有读取逻辑。New-format 复盘可另跑 `node .claude/scripts/retro-efficiency.mjs --since <周期起点>` 取四指标 Pod 表与按 Agent 角色汇总表；**口径以脚本为唯一权威**、本节不复述列定义，游标 / 触发条款 / 分流纪律不动。

**兼读 pod-retro + pod-journal（叙事车道层 A）+ 两源偏差仲裁**：retro 跨 Pod 提共性时**兼读** loop-health（综合视图）+ `pod-retro.md`（planner 第一人称摩擦/纠偏主题）+ **`pod-journal.jsonl`（叙事车道层 A，round-outcome / friction 事件；存在时读，advisory 补充读取面）**。当 pod-retro 与 loop-health 对**同一摩擦/纠偏主题矛盾**时，retro **显式列「planner 体感 vs integrator 综合偏差」条目 → 转 architect-owned ARQ candidate，标偏差不采信任一源、不重算单 Pod 健康**（冲突本身是有价值信号）。**new-format 源对（pod-retro vs pod-events）**：new-format Pod 无双源 join 仲裁——pod-events 是机器聚合（escalation / friction / human-correction 的 counts + 主题枚举），pod-retro 是散文主题，二者**结构性不可按主题对撞**。retro 对该类 Pod 改做**覆盖比对**（见 loop-retro.md step 3）：逐主题核对 pod-retro 记的摩擦/纠偏主题是否在 pod-events 有对应 event，显式列『机器聚合 vs 散文主题 覆盖偏差』条目（如 ledger friction=0 而 pod-retro 记 N 主题）→ 转 architect-owned ARQ candidate，标偏差不采信任一源。pod-events 缺失/降级不阻 retro（承 §5d fail-open）。缺 pod-retro 的 Pod 仍读其 loop-health 被分析（不跳过、不标未分析、不补写，仅 advisory）。缺 `pod-journal.jsonl` 同理——仅 advisory 降级、不阻 retro。

**findings 对账现行 canonical / 已裁决项（GC1）**：retro 在把任一发现定为 ARQ candidate / 分流之前，**SHOULD 对账（advisory 强度，弱闭合经 step-5 必载字段 + 下一周期消费核对）**该发现所指对象的**最小必要对账集（非全量）**——该 finding 所指对象之**现行 canonical 状态**（`docs/DEV-GOVERNANCE.md` 角色编制 / 门体系、`dev-loop-operating-system.md`、相关 agent def 现行条文）+ 该对象**直接相关之已裁决项**（ARQ / CDQ 主题直接指向该对象者，状态含 approved-registered / landed / accepted-risk）；**canonical 已改项必含**（如退役编制注，纵无独立 ARQ 亦须核）；不要求对无关活 ARQ blanket 扫荡。发现与现行 canonical 冲突（如『建议补建已退役 def』）→ **不新立 ARQ，记『复现：已裁决项未被行为闭合』**，证据引真实现行锚点（核现行行号、标旧引漂移），转 architect 判断是否属行为闭合缺口（landing ≠ 行为闭合）。对账是**人读判断步骤、advisory**，非工程门（§5 六门不变式，见 §3）。**弱闭合链三环**：① 具名 owner = 主线程（对偶 OS `:148`/`:150` + loop-retro.md `:37` 登记执行者）；② 必载输出 = step-5 GC1+GC3 对称字段（见改法 2C）；③ 下轮消费 = 上轮对账/比对是否闭合列为下一周期 retro 定点核对（活先例 loop-retro-2026-07-28 `:88-93`）。

**游标（已分析 vs 未分析）**：由 `.claude/loop-retro/ledger.jsonl`（append-only，每轮 retro 追加其覆盖的 change-id）判定——「已分析」= 所有行 `coveredChangeIds` 的并集，「本轮待分析」= 有 `loop-health.md` 但不在并集里（`loop-health-report.mjs --pending` 算范围，`--record-retro <retroId>` 收口推进）。不靠时间窗或人肉记忆；ledger 随 main 版本控制、跨设备一致。缺 `loop-health.md` 的 Pod 进 `waitingOnLoopHealth`（待 integrator 补），不算已分析。**第三态（human-approved 销账 / excluded，gov-review-retro-waiting-writeoff FROZEN 2026-08-01）**：所有行 `excludedChangeIds` 的并集 = 经 human-approved 销账的结构性永不析出 Pod；`computeRetroScope` 在分桶源头排除（不进 `pendingAnalysis`/`waitingOnLoopHealth`），不算已分析亦不计 waiting——独立第三态。销账经 `loop-health-report.mjs --writeoff` 独立手动记录（human-only、非例行步骤），`excludedFromTrigger` 桶单独报其绝对规模（fail-safe 不隐藏）；撤销走 git revert（append-only 不可逆并集）。

**触发条款（gov-review-devloop-closeout-hardening §3.1 / Q2 复合触发 + F-13）**：loop-retro 是 cadence 复盘，**复合触发**即起——
- ① **计数阈值**：`computeRetroScope` 的 `pendingAnalysis` **+ `waitingOnLoopHealth` 合计 ≥ N**（默认 N=3，Q2 终裁前 provisional）**或日历兜底**（如双周）。**盲区注（F-13）**：单看 pending 对降级群体结构性盲区（无 pod-events 的 Pod 进 waiting 而非 pending），故合计数**含 waiting，但排除经 human-approved 销账（excluded）的永不析出 Pod**（源桶排除，游标第三态）；盲区注（F-13）对未销账 waiting 仍成立——可恢复降级群体仍留 waiting、仍计入，两集合不相交。
- ② **事件触发**：任一 Pod 出现 `human-correction` 事件（pod-events 存在叠加）→ 下一 Pod 起包前必跑（纠偏 = 流程缺口信号，不等阈值）。
- **发起责任 = 主线程**（supervisor verdict advisory 或 closeout-check 提示位触发，**不新增工程门**，DEV-GOVERNANCE §5 保持 6 门）。`closeout-check.mjs` 的 pending 计数 advisory 提示位**程序化调用 `computeRetroScope`、不复制其求和逻辑**（F-13，防计数漂移；源桶排除使 3 消费方——closeout-check / retro-staleness-check / merge-driver——单源自动传导，零重算）。`/loop-retro` 命令步骤权威指本节、不重述条文。

**凡涉及修改 Dev Loop OS 定义、agent 角色、gate、scope-map、K9/DevLoop 边界、merge 标准的输出，先成为 architect-owned ARQ candidate，交 `architect` 处理**——不直接进普通 planning/taskpackage，retro 不自我批准。**登记动作 = 主线程**（human-approved 2026-07-28）：主线程先查同根项（`grep -l "<关键词>" docs/debt/*/*.md`），有 → 追加证据注记（不新立），无 → 新立；architect 经面 loop（`/arch-change`、`/gov-change`）处理已登记 ARQ、不做登记。只有 architect outcome 明确要求实现侧（`firmware/**` / `server/**` / 测试）改动，才转实现侧 dev-loop。ARQ 出口 = debt 账本 [`docs/debt/gov/`](../../docs/debt/gov/)（一条目一文件 + `debt-render.mjs` 生成 `INDEX.md`）。

## 7. 协调 + 合并串行化（摘要，机制见 pod-coordination.md）

- **运行时协调态**（board + merge token + 各 Pod 状态）放**所有 linked worktree 共享的 git common dir**（`$(git rev-parse --git-common-dir)/devloop/`），**不入版本控制**。
- **拆包交接可见性（commit-before-launch + launch 前置门，pod-coordination §2b）**：portfolio-planner 拆包产物（tracked 耐久制品 `delivery/workstreams/<ws>/**` + `delivery/taskpackages/<changeId>/**`，**不含 board**）须先 commit 到专用 base ref 再 launch；launch 前 `git cat-file -e` 校验基准 ref 含 check 集（taskpackage+PROPOSAL blocking / decision-coverage warn），缺失拒绝 launch（对偶 planner.md Step 1 intake 权威消费 + fail-closed）。
- **耐久证据**入版本控制：post-cutoff 为 pod-events / pod-retro / taskpackage / review 制品；legacy 兼容 session-digest / loop-health / loop-metrics / pod-journal。落 `delivery/taskpackages/<change-id>/` 与 `delivery/taskpackages/<change-id>/`，随 Pod worktree 合并进 main。
- **并行开发、串行合并**：N 个 Pod 并行开发、各自闭环（含 integrator+supervisor）。supervisor APPROVE 后标 ready-to-merge。merge-driver.mjs（planner 自动触发，单 Pod --pod）按 mergeOrder 逐个：拿 token → rebase 到当前 main → quick-check → merge → 释放 token。supervisor 在 Pod 内把关（APPROVE/BLOCK/ESCALATE），merge-driver 执行机械合并（本地 --no-ff 可回滚）。谁都不能并行直落 main。

## 相关

- [`docs/kernel/K9-orchestrator.md`](../../docs/kernel/K9-orchestrator.md) §5（运行时→开发期桥；repair-intent / `Defect.status` 边界）
- [`.claude/references/dev-loop.md`](dev-loop.md)（loop 内部机制）
- [`.claude/references/pod-coordination.md`](pod-coordination.md)（blackboard / merge token / reconcile 机制）
- [`.claude/references/task-intake-and-slicing.md`](task-intake-and-slicing.md)（intake / slicing / `delivery/workstreams` 归属）
- [`docs/debt/`](../../docs/debt/)（architect-owned ARQ 出口 = debt 账本 gov 域：`docs/debt/gov/INDEX.md`）
