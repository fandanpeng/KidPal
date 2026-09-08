# PROPOSAL · 260908-gov-review-escalation-conflict-ladder

> **状态：DRAFT v0.1 · 2026-09-08 · 主线程起草完毕；human 立项 ack 已获（2026-09-08 AskUserQuestion：ack 进评审 + OQ2=③ 级 architect 建议即地板 + OQ4=接受相交探测承担真跨系统保护 + 质询 review 轨「有那么复杂吗」→ 主线程复判改 Track B，见下行）。ack 后走 architect 评审循环至 GREEN。**
> **review 轨**：**Track B 常驻轻路径（v0.1 原自评 Track A 作废，human 质询后主线程复判降档）**。降档归因如实记录：①「真设计分叉」已被 human 立项裁定消费（三级梯 = human 2026-09-08 讨论裁定 + OQ2/OQ4 同场裁）；②gate 面实为**门内判据改造 + cross-system 组成项退役**，非新增/删除工程门——先例 = 260908 批 T5 CDQ 核验行（A-2 裁「门内加项」，Track B 以 gate 事实声明承载）+ merge-friction 批 Step 0。Track B 三要件核对：契约面零触 / 无未决设计分叉 / 改动面全有机械夹具（V1/V2/V6 回放）。gate 事实如实声明：T1 判据换轴 + T4 分流 = gate 语义改造，交判官复核；判官若认定仍有未决分叉可打回升 Track A。
> **承载 ARQ**：`ARQ-260907-escalation-cross-system-overfire`（主承载，判据换轴）。来源链 = ARQ 原文两条候选（①收紧目录计数 ②接受恒批）经 human 2026-09-08 立项讨论**双双否弃**，裁出第三方向：「冲突核对面做实了，要不要人批不重要」——判据从「目录面启发式」换轴为「并行相交面可核性」，三级梯见 §0。姊妹面：本批 T5 CDQ 核验行同文件（`pre-merge-escalation-gate.mjs`）已随 260908-gov-review-merge-chain-hardening 落地合并（main `5eaf610`），无并发编辑冲突。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：升级门是合并链的守门判据，换轴错向会把真跨系统变更放进来自动合并——但新判据的每一级都以「机械可核证据」为放行前置、fail-closed 默认与既有链一致，且①级零相交自动放行等于 merge-driver rebase+quick-check 本来就在做的事的提前显式化，无新增放行集）。
> **review profile**：G2+G3（触面 = pod-coordination.md / DEV-GOVERNANCE 治理注 G2 + pre-merge-escalation-gate.mjs / merge-driver.mjs G3；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
.claude/references/pod-coordination.md
docs/DEV-GOVERNANCE.md
docs/ARCH-REVIEW-QUEUE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：升级门从「目录面启发式 → 恒 ESCALATE → human 盲批」换轴为「并行相交面探测 → 按裁决深度分级：机械可核的机械核、有取舍的 architect 核、architect 判不了的才升 human——且升级件自带与对面 Pod 的逐文件相交清单」，使 human 只出现在真正需要人的地方、且出现时有实据可看。

**human 立项讨论裁定的三级梯（2026-09-08，本批立项输入）**：

| 梯级 | 情形 | 裁决者 | 升级件内容 |
|---|---|---|---|
| ① 零相交 | Pod diff 与 main（含同批已并 Pod）无文件相交 | 机械：放行（merge-driver 既有 rebase+quick-check 照走） | — |
| ② 相交·无取舍 | 相交但并集即正解（不同 hunk/段落；典型 = 并行包各写 `.vN` 载体不同段、append 类） | 机械：按 §10c/§10a runbook 自动并集消解 + **复跑双方冻结件**，双方齿绿 → 过，不升人 | — |
| ③ 相交·有取舍 | 同一 hunk 语义单元相撞（同 key 异值 / 同锚互斥断言 / 版本字面互斥） | **architect review**（评审消费：给消解建议，不落地） | 相交文件清单 + 双方正文 diff |
| ④ architect 亦判不了 | 消解需动语义 / 复跑齿红 / 取舍判据本身判不了（存疑向严） | **escalate human** | 逐文件相交清单 + 双方版本正文 + architect 意见 + 消解选项 |

**现状差距**（ARQ-260907-escalation-cross-system-overfire 实证，2026-09-07）：① 260829-home-p1 supervisor 判 ESCALATE 后被裁定**误报**（main `82cb8fb` commit message 明载）；② schedule-p2 判 ESCALATE 的理由 `cross-system:impl=main,acceptance,deploy,gateway` 把 acceptance/deploy 常规目录面计入跨系统信号；③ 同批两 Pod 全 ESCALATE 全人批零驳回——门退化为仪式；④ 更本质：human 被要求批「放行」时，拿到的是一行判据摘要，**没有相交文件清单、没有冲突预演、无从核起**（human 2026-09-08 原话意旨：「我怎么能确定他们改的有没有冲突，这才是本质问题」）。

**非目标（本批不做）**：
- 不动 supervisor 判据（APPROVE/BLOCK/ESCALATE 的 verdict 语义归 supervisor；本批改的是前置升级闸的放行判据与升级件内容面）；
- 不做 ② 级「自动并集」的全自动消解执行——② 级仍走既有 runbook 通道（integrator merge-chain / §10a 消解纪律五条，260908 批已落地），本批只是把「无取舍不升人」的**判据**钉进闸，消解动作本身不新增执行者；
- 不动本批（merge-chain-hardening）刚落的 T5 CDQ 核验行 / 版本化制品 add/add 特判——它们是本批梯子的 ②③ 级下层机械件，本批复用不重写；
- 不触契约面。

## 1. 问题与根因（地面真值）

### P1 · 判据测错了东西

cross-system 判据数的是 **diff 目录前缀的多样性**，不是「两个并行 Pod 的改动是否真相交」。acceptance/deploy/tests 是双面板 Pod 的常规面（每个 firmware+server 包天生带），impl=main 是目录名不是系统边界——判据对双面板 Pod 恒真，把「要不要人看」错答成「diff 长什么样」。证据：home-p1 误报裁定（`82cb8fb`）+ schedule-p2 ESCALATE 理由行（supervisor-verdict.json adjudication.reasons）+ 双 Pod 全人批零驳回。

### P2 · 升级件零信息，human 无从核起

ESCALATE 弹给人的一行 `cross-system:...` 摘要不含：相交文件清单、对面 Pod 是谁、双方正文 diff、冲突预演结果。human 2026-09-08 指出这是本质问题——**批准动作无法核验时，批准本身就是仪式**。而真正的冲突裁决证据（merge-tree 预探测、双方冻结件复跑）在机械链里已经存在（schedule-reminder Integration Probe 实测过只读 merge-tree 形态；260908 批 §10a 补条④把双方复跑钉成纪律），只是**没有被组装进升级件**。

### P3 · 「取舍」没有判据，二级裁决者缺位

真相交时「并集是否即正解」目前全靠现场会话判断（schedule-reminder 靠仓外副本三消解对照才敢定并集）。梯级 ②③ 的分界（无取舍 / 有取舍）无成文判据、无裁决者——这是三级梯能立起来的前置件。

## 2. 改法（T1…T5）

### T1 · 闸内新增并行相交面探测（G3）

`pre-merge-escalation-gate.mjs` 增 `parallelOverlapProbe(ctx)`：ready 判定时对 Pod 源分支跑**只读** `git merge-tree --write-tree <source> <target-main>`（先例 = schedule-reminder review-packet §Integration Probe 同款只读形态），产出 `{intersectingFiles: [], addAddFiles: [], conflictedHunks: []}`。探测失败 fail-closed（视为有相交，走 ③ 级）——不放行、不误并。**cross-system 目录计数判据同批退役**（原六条例外清单的 cross-system 组成项移除，其余例外——契约面 / off-main / blocked / riskTier / provenance / gov GREEN 锚——逐字不动，canary 承诺）。

### T2 · 取舍判据成文 + 分级路由（G3+G2）

「无取舍 / 有取舍」机判判据钉进 pod-coordination.md §10d（新节，v0.1 候选交 charter 期冻结）：**无取舍** = 相交文件双方改动落在不同 hunk / 不同语义段（机判：双方版本对同文件 diff 的 hunk 区间不相交；`.vN` 载体不同段 / append 类按此判）；**有取舍** = 同 hunk 语义单元相撞（同 key 异值、同锚互斥断言、版本字面互斥——版本字面互斥复用 260908 批 guard-class wiring 判据）；**判不了 = 存疑向严升 ③ 级 architect**。分级路由：零相交 → ① 放行；无取舍 → ②（不升人，走 runbook 自动并集 + 双方齿复跑，齿红即升 ③）；有取舍 → ③ architect review（升级件带相交清单 + 双方 diff，architect 出消解建议，**不落地**——落地恒归 runbook 执行面）；③ 判不了 / 齿红 / 需动语义 → ④ escalate human（升级件带逐文件清单 + 双方版本正文 + architect 意见 + 消解选项）。

### T3 · 升级件内容面（G2）

supervisor-verdict ESCALATE 与闸 BLOCK 的输出统一增「并行相交面」段：对面 Pod 标识、逐文件相交清单、双方版本正文摘录锚、architect 意见（③ 级产出）、消解选项。human 批的是**消解方案**而非放行仪式；批后凭据走既有 `--human-approval-ref` 通道（零改动）。

### T4 · merge-driver 消费探测结果（G3）

processPod 的 rebase 冲突路径复用 T1 探测结果分流：② 级走既有投影特判 / 版本化制品并集指引（260908 批已落）+ 双方齿复跑；③④ 级按升级件走。rebase 实撞冲突与预探测结论不一致时（探测说无取舍、rebase 实撞）→ 存疑向严升 ③，fail-closed。

### T5 · DEV-GOVERNANCE 治理注 + 队列销账（G2/G4）

DEV-GOVERNANCE「前端对齐 + 例外反选」节补一句：例外升级拍板的升级件必含并行相交面内容（分级路由见 pod-coordination §10d）——human 触点语义从「盲批放行」改述为「裁消解方案」，不新增 human 触点。队列 `ARQ-260907-escalation-cross-system-overfire` 行随 landing 结移 §2。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 判据换轴（cross-system 退役 + 相交探测新增）= gate 语义变更，Track A 承载 | 合并门前置；human 升级件 |
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 冲突路径分流（复用 260908 批既有特判，不重写） | planner 主线程 merge 段 |
| 协调 runbook | `.claude/references/pod-coordination.md` | G2 新节 §10d（取舍判据 + 分级路由） | ②③④ 级裁决者 |
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G2 升级件内容面一句 | human 触点 |
| 队列 | `docs/ARCH-REVIEW-QUEUE.md` | G4 销账（ARQ-260907 行） | — |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| supervisor 判据 | — | **未触** | verdict 语义不动 |

**【硬项 1b · sweep 透镜义务】** cross-system 判据退役的全仓消费面预跑（起草时点）：`grep -rn "cross-system" .claude/ docs/` 命中面待评审期逐点核对（gate / supervisor-gate / pod-coordination / 既有 verdict 样本），逐点标注「随批改 / 保留历史语义」。探测只读性预核：`git merge-tree --write-tree` 为只读（schedule-reminder 实测先例），不落工作树。

**【硬项 1c · 复扫模式完备性先验】** cross-system 判据退役后其字面在历史 verdict / 评审制品中留存属历史证据，非残留清扫对象。

## 4. human 取舍点（OQ）

- **OQ1 · ② 级自动并集的齿复跑范围**：(a) **双方冻结件 + 全档**（推荐——对端红纪律的机械承载，260908 批 §10a 补条④同款）；(b) 仅双方冻结件（省时但漏跨文件消解副作用）。
- **OQ2 · ③ 级 architect 的裁决权形态**：(a) **建议即地板**（architect 出消解建议 + 可机判依据，② 级执行面照办；architect 不落地——对偶 author≠judge）；(b) architect 直接裁决（越权风险，不推荐）。
- **OQ3 · 探测频率**：(a) **仅合并门时点**（推荐——一处探测一处消费，最小面）；(b) launch 时预探测 + 合并时复测（提前暴露但双倍探测面 + 预探测结果易腐）。
- **OQ4 · cross-system 退役后「真跨系统变更」的保护**：判据退役后，固件↔server 实现面真相交的 Pod 落 ③ 级 architect（因为大概率同 hunk 或大面相交）——保护不靠目录启发式、靠相交探测本身。(a) 接受（推荐，证据在 OQ4 论证行）；(b) cross-system 保留为 ④ 级附加信号。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · gate 自检扩**：既有 21 例零回退；新增——零相交放行 / add-add 相交 → ② 级路由 / 同 hunk 相撞 → ③ 级 / 探测失败 fail-closed → ③ 级 / cross-system 目录面不再触发升级（home-p1 形态夹具：impl+acceptance+deploy 双面板常规面 → 不 ESCALATE）。
- **V2 · merge-driver 自检扩**：既有 20 例零回退；新增——预探测「无取舍」但 rebase 实撞 → 升 ③（fail-closed）；② 级并集 + 双方齿复跑绿 → 过。
- **V3 · 端到端演练**：临时仓复刻 schedule-reminder/story-p3 双包抢 v3 场景全链——断言：探测命中 add/add → ② 级路由（不 ESCALATE）→ 并集消解 → 双方齿绿 → 自动过，全程零 human 触点。
- **V4 · 升级件内容面**：③/④ 级夹具断言升级件含逐文件相交清单 + 双方版本锚；④ 级含 architect 意见槽。
- **V5 · 投影 grep**：pod-coordination §10d 在场；DEV-GOVERNANCE 升级件句在场；cross-system 判据字面在 gate 活代码零命中（历史制品除外）；queue ARQ-260907 行结移 §2。
- **V6 · 回归对照**：以 home-p1 / schedule-p2 的 supervisor-verdict + diff 为回放夹具——同 diff 输入下新判据不再 ESCALATE（误报归零实证），而构造的真跨系统同 hunk 相撞夹具仍落 ③/④ 级（不漏报实证）。

## 6. 留根清单

- **取舍判据的机判实现精度**（同 hunk 判定的 diff 粒度）：v0.1 只钉语义判据，实现粒度（行级 / hunk 级 / 语义单元级）归 charter 期与评审循环定，实测不够准则降级「判不了 → ③ 级」为默认。
- **supervisor-verdict payload schema**：T3 升级件内容面若需 schema 字段（对齐 friction-event-payload-schema ARQ 病族），最小 schema 留观测后批。
- **③ 级 architect 消费的派发面**：planner.md 派单格式（对齐 260908 批 F7/B-3 留根——planner 派单格式义务），归 planner.md 修订批。
- **B-8 重指排队行 / 姊妹批两条 draft**：与本批无关，既有排队在册。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
