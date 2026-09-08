# PROPOSAL · 260908-gov-review-escalation-conflict-ladder

> **状态：FROZEN v1.0 · 2026-09-08 · 主线程修订（R1 九条 findings 全修 + human 三裁在册：B1 确认修法——非目标改写为「不动 verdict APPROVE/BLOCK 语义；cross-system 退役实现路径 = 改 supervisor-verdict-gate 的 riskTier 派生轴」+ hook 入 manifest；OQ1=(a) 双方冻结件+全档；OQ3=(a) 仅合并门时点，均为 human 2026-09-08 终裁）；human 立项 ack 已获（2026-09-08 AskUserQuestion：ack 进评审 + OQ2=③ 级 architect 建议即地板 + OQ4=接受相交探测承担真跨系统保护 + 质询 review 轨「有那么复杂吗」→ 主线程复判改 Track B，见下行）。R1 verdict = NEEDS-WORK（[architect-review-r1.md](architect-review-r1.md)，BLOCKING 1 / SHOULD 5 / COULD 3），v0.2 承载逐条修复，交 R2 复核。**
> **review 轨**：**Track B 常驻轻路径（v0.1 原自评 Track A 作废，human 质询后主线程复判降档）**。降档归因如实记录：①「真设计分叉」已被 human 立项裁定消费（三级梯 = human 2026-09-08 讨论裁定 + OQ2/OQ4 同场裁）；②gate 面实为**门内判据改造 + cross-system 判据退役（物理宿主 = supervisor-verdict-gate riskTier 派生轴，见 T1——R1 B1 修正后表述）**，非新增/删除工程门——先例 = 260908 批 T5 CDQ 核验行（A-2 裁「门内加项」，Track B 以 gate 事实声明承载）+ merge-friction 批 Step 0。Track B 三要件核对：契约面零触 / 无未决设计分叉 / 改动面全有机械夹具（V1/V2/V6 回放）。gate 事实如实声明：T1 判据换轴 + T4 分流 = gate 语义改造，交判官复核；判官若认定仍有未决分叉可打回升 Track A。
> **承载 ARQ**：`ARQ-260907-escalation-cross-system-overfire`（主承载，判据换轴）。来源链 = ARQ 原文两条候选（①收紧目录计数 ②接受恒批）经 human 2026-09-08 立项讨论**双双否弃**，裁出第三方向：「冲突核对面做实了，要不要人批不重要」——判据从「目录面启发式」换轴为「并行相交面可核性」，三级梯见 §0。姊妹面：本批 T5 CDQ 核验行同文件（`pre-merge-escalation-gate.mjs`）已随 260908-gov-review-merge-chain-hardening 落地合并（main `5eaf610`），无并发编辑冲突。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：升级门是合并链的守门判据，换轴错向会把真跨系统变更放进来自动合并——但新判据的每一级都以「机械可核证据」为放行前置、fail-closed 默认与既有链一致，且①级零相交自动放行等于 merge-driver rebase+quick-check 本来就在做的事的提前显式化，无新增放行集）。
> **review profile**：G2+G3（触面 = pod-coordination.md / DEV-GOVERNANCE 治理注 G2 + pre-merge-escalation-gate.mjs / merge-driver.mjs / supervisor-verdict-gate.mjs G3——B1 修正：cross-system 物理宿主在 hook、非 pre-merge gate；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
.claude/hooks/supervisor-verdict-gate.mjs
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
| ② 相交·无取舍 | 相交但并集即正解（不同 hunk/段落；典型 = 并行包各写 `.vN` 载体不同段、append 类；已登记 §10c 形态的版本化制品 add/add 亦按此级——carve-out 见 §10d） | 机械：按 §10c/§10a runbook 自动并集消解 + **复跑双方冻结件**，双方齿绿 → 过，不升人 | — |
| ③ 相交·有取舍 | 同一 hunk 语义单元相撞（同 key 异值 / 同锚互斥断言 / 版本字面互斥） | **architect review**（评审消费：给消解建议，不落地） | 相交文件清单 + 双方正文 diff |
| ④ architect 亦判不了 | 消解需动语义 / 复跑齿红 / 取舍判据本身判不了（存疑向严） | **escalate human** | 逐文件相交清单 + 双方版本正文 + architect 意见 + 消解选项 |

**现状差距**（ARQ-260907-escalation-cross-system-overfire 实证，2026-09-07）：① 260829-home-p1 supervisor 判 ESCALATE 后被裁定**误报**（main `82cb8fb` commit message 明载）；② schedule-p2 判 ESCALATE 的理由 `cross-system:impl=main,acceptance,deploy,gateway` 把 acceptance/deploy 常规目录面计入跨系统信号；③ 同批两 Pod 全 ESCALATE 全人批零驳回——门退化为仪式；④ 更本质：human 被要求批「放行」时，拿到的是一行判据摘要，**没有相交文件清单、没有冲突预演、无从核起**（human 2026-09-08 原话意旨：「我怎么能确定他们改的有没有冲突，这才是本质问题」）。

**非目标（本批不做）**：
- 不动 supervisor verdict 的 APPROVE/BLOCK 语义与证据完整度判定（human 2026-09-08 终裁）；cross-system 判据退役的实现路径 = 改 `supervisor-verdict-gate.mjs` 的 riskTier 派生轴（`isCrossSystem` :224-231 摘除、保留 core.\*/all-mock/security-axis 其余轴）——本批改的是升级链放行判据与升级件内容面；
- 不做 ② 级「自动并集」的全自动消解执行——② 级仍走既有 runbook 通道（integrator merge-chain / §10a 消解纪律五条，260908 批已落地），本批只是把「无取舍不升人」的**判据**钉进闸，消解动作本身不新增执行者；
- 不动本批（merge-chain-hardening）刚落的 T5 CDQ 核验行 / 版本化制品 add/add 特判——它们是本批梯子的 ②③ 级下层机械件，本批复用不重写；
- 不触契约面。

## 1. 问题与根因（地面真值）

### P1 · 判据测错了东西

cross-system 判据数的是 **diff 第二段目录前缀的多样性**（`distinctAppSystems`，supervisor-verdict-gate.mjs :213-221），不是「两个并行 Pod 的改动是否真相交」。acceptance/deploy/tests 是双面板 Pod 的常规面（每个 firmware+server 包天生带），impl=main 是目录名不是系统边界——schedule-p2（firmware+server 双面板，reason = `cross-system:impl=main,acceptance,deploy,gateway`）恒真；home-p1 更甚：纯固件单系统（firmware/.gitignore + firmware/main/**，reason = `cross-system:impl=.gitignore,main`）也触发——判据连单系统内双段都误伤，把「要不要人看」错答成「diff 长什么样」。证据：home-p1 误报裁定（`82cb8fb`）+ 两 Pod supervisor-verdict adjudication.reasons 逐字（R1 亲核 git 历史：home-p1 合并 `20f1e09` 改动 23 文件、schedule-p2 `b7df3ab` 27 文件）+ 双 Pod 全人批零驳回。

### P2 · 升级件零信息，human 无从核起

ESCALATE 弹给人的一行 `cross-system:...` 摘要不含：相交文件清单、对面 Pod 是谁、双方正文 diff、冲突预演结果。human 2026-09-08 指出这是本质问题——**批准动作无法核验时，批准本身就是仪式**。而真正的冲突裁决证据（merge-tree 预探测、双方冻结件复跑）在机械链里已经存在（schedule-reminder Integration Probe 实测过只读 merge-tree 形态；260908 批 §10a 补条④把双方复跑钉成纪律），只是**没有被组装进升级件**。

### P3 · 「取舍」没有判据，二级裁决者缺位

真相交时「并集是否即正解」目前全靠现场会话判断（schedule-reminder 靠仓外副本三消解对照才敢定并集）。梯级 ②③ 的分界（无取舍 / 有取舍）无成文判据、无裁决者——这是三级梯能立起来的前置件。

## 2. 改法（T1…T5）

### T1 · 闸内新增并行相交面探测（G3）

`pre-merge-escalation-gate.mjs` 增 `parallelOverlapProbe(ctx)`：ready 判定时对 Pod 源分支跑**只读** `git merge-tree --write-tree <source> <target-main>`（先例 = schedule-reminder review-packet §Integration Probe 同款只读形态；精确化：`--write-tree` 不动工作树/refs，但向 ODB 写 dangling tree/blob 对象、GC 可回收），产出 `{intersectingFiles: [], addAddFiles: [], conflictedHunks: []}`；可选参 `--probe-out <path>` 落 JSON 探测面制品（传递契约见 T4）。探测失败 fail-closed（视为有相交，走 ③ 级）——不放行、不误并。**cross-system 目录计数判据同批退役**——物理宿主 = `supervisor-verdict-gate.mjs` 的 `isCrossSystem`（:224-231，经 classifyHigh → deriveDiffTier → riskTier=high → ESCALATE；R1 亲核：pre-merge gate 六条例外内本无 cross-system 组成项，v0.1「六条例外的 cross-system 组成项」表述有误、在此改述），退役实现 = 该 riskTier 派生轴摘除 cross-system 组成、保留其余轴（core.\*/all-mock/security-axis）；FIX-6 第二方重算（supervisor-gate-check.mjs :35/:759 import 同模块纯函数）单源自动一致、不须单独改。pre-merge-escalation-gate 六条例外（契约面 / off-main / blocked / riskTier / provenance / gov GREEN 锚）逐字不动，canary 承诺。

### T2 · 取舍判据成文 + 分级路由（G3+G2）

「无取舍 / 有取舍」机判判据钉进 pod-coordination.md §10d（新节，v0.1 候选交 charter 期冻结）：**无取舍** = 相交文件双方改动落在不同 hunk / 不同语义段（机判：双方版本对同文件 diff 的 hunk 区间不相交；`.vN` 载体不同段 / append 类按此判；**add/add carve-out**：版本化制品 add/add 已登记 §10c 登记表形态 → ② 级（复用 260908 批特判 + 并集指引，预裁定无取舍），未登记形态 → 判不了 → ③ 级，fail-closed 对齐 A-7 补集不变式）；**有取舍** = 同 hunk 语义单元相撞（同 key 异值、同锚互斥断言、版本字面互斥——版本字面互斥复用 260908 批 guard-class wiring 判据）；**判不了 = 存疑向严升 ③ 级 architect**。分级路由：零相交 → ① 放行；无取舍 → ②（不升人，走 runbook 自动并集 + 双方齿复跑，齿红即升 ③）；有取舍 → ③ architect review（升级件带相交清单 + 双方 diff，architect 出消解建议，**不落地**——落地恒归 runbook 执行面）；③ 判不了 / 齿红 / 需动语义 → ④ escalate human（升级件带逐文件清单 + 双方版本正文 + architect 意见 + 消解选项）。**③ 派发 fallback**：③ 级 architect 派发不可用（派发失败 / 超时 / 无消费方）→ 直接 ④ escalate human（fail-closed，防 ③ 悬空退化盲批）。

### T3 · 升级件内容面（G2）

supervisor-verdict ESCALATE 与闸 BLOCK 的输出统一增「并行相交面」段：对面 Pod 标识、逐文件相交清单、双方版本正文摘录锚、architect 意见（③ 级产出）、消解选项。human 批的是**消解方案**而非放行仪式；批后凭据走既有 `--human-approval-ref` 通道（零改动）。③ 级 architect 派发失败 / 超时 → 升级件直接落 ④ human 形态（§10d 同款 fail-closed fallback）。

### T4 · merge-driver 消费探测结果（G3）

**传递契约（S2）**：gate 以可选参 `--probe-out <path>` 落 JSON 探测面制品（显式文件、非 stdout 解析——merge-driver Step 1b 以子进程跑闸、成功时丢弃 stdout 的现状不改，传递面走文件）。processPod 的 rebase 冲突路径复用该探测结果分流：② 级走既有投影特判 / 版本化制品并集指引（260908 批已落）+ 双方齿复跑；③④ 级按升级件走。rebase 实撞冲突与预探测结论不一致时（探测说无取舍、rebase 实撞）→ 存疑向严升 ③，fail-closed。

### T5 · DEV-GOVERNANCE 治理注 + 队列销账（G2/G4）

DEV-GOVERNANCE §6 例外升级拍板段内（landing 通道段，「前端对齐 + 例外反选」为段内短语、非独立节）补一句：例外升级拍板的升级件必含并行相交面内容（分级路由见 pod-coordination §10d）——human 触点语义从「盲批放行」改述为「裁消解方案」，不新增 human 触点；manifest 已含 docs/DEV-GOVERNANCE.md、零增改。队列 `ARQ-260907-escalation-cross-system-overfire` 行随 landing 结移 §2。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 相交探测新增（六条例外逐字不动）= gate 语义改造，Track B 以 gate 事实声明承载 | 合并门前置；human 升级件 |
| 升级链判据宿主 | `.claude/hooks/supervisor-verdict-gate.mjs` | G3 判据换轴（riskTier 派生轴摘除 cross-system，human 2026-09-08 终裁；verdict APPROVE/BLOCK 语义不动） | supervisor verdict 派生；FIX-6 重算单源自动一致 |
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 冲突路径分流（复用 260908 批既有特判，不重写） | planner 主线程 merge 段 |
| 协调 runbook | `.claude/references/pod-coordination.md` | G2 新节 §10d（取舍判据 + 分级路由） | ②③④ 级裁决者 |
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G2 升级件内容面一句 | human 触点 |
| 队列 | `docs/ARCH-REVIEW-QUEUE.md` | G4 销账（ARQ-260907 行） | — |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| supervisor verdict 语义 | — | **未触**（APPROVE/BLOCK 裁决规则与证据完整度判定不动；riskTier 派生轴 cross-system 退役见上行 hook 行） | supervisor 合并门 |

**【硬项 1b · sweep 透镜义务】** cross-system 判据退役的全仓消费面预跑（起草时点）：`grep -rn "cross-system" .claude/ docs/` 命中面待评审期逐点核对（gate / supervisor-gate / pod-coordination / 既有 verdict 样本），逐点标注「随批改 / 保留历史语义」——**R1 已核**：活代码命中 = `supervisor-verdict-gate.mjs` :226-229（判据本体，随批改）+ :627-628（自检断言，随批改写）；`task-intake-and-slicing.md` / `architect-operating-model.md` / `arch-review-protocol.md` 命中为「跨系统」通用语、非本判据（保留）；其余 = 历史 verdict / 评审制品 / ADR（保留历史语义，硬项 1c 先验成立）。探测只读性预核：`git merge-tree --write-tree` 为只读（schedule-reminder 实测先例），不落工作树。

**【硬项 1c · 复扫模式完备性先验】** cross-system 判据退役后其字面在历史 verdict / 评审制品中留存属历史证据，非残留清扫对象。

## 4. human 取舍点（OQ）

- **OQ1 · ② 级自动并集的齿复跑范围**：(a) **双方冻结件 + 全档**（推荐——对端红纪律的机械承载，260908 批 §10a 补条④同款）；(b) 仅双方冻结件（省时但漏跨文件消解副作用）。——**human 2026-09-08 终裁 (a)**（与 B1 修法、OQ3 同场三裁）。
- **OQ2 · ③ 级 architect 的裁决权形态**：(a) **建议即地板**（architect 出消解建议 + 可机判依据，② 级执行面照办；architect 不落地——对偶 author≠judge）；(b) architect 直接裁决（越权风险，不推荐）。
- **OQ3 · 探测频率**：(a) **仅合并门时点**（推荐——一处探测一处消费，最小面）；(b) launch 时预探测 + 合并时复测（提前暴露但双倍探测面 + 预探测结果易腐）。——**human 2026-09-08 终裁 (a)**（与 B1 修法、OQ1 同场三裁）。
- **OQ4 · cross-system 退役后「真跨系统变更」的保护**：判据退役后，固件↔server 实现面真相交的 Pod 落 ③ 级 architect（因为大概率同 hunk 或大面相交）——保护不靠目录启发式、靠相交探测本身。(a) 接受（推荐，证据在 OQ4 论证行）；(b) cross-system 保留为 ④ 级附加信号。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · gate 自检扩**：pre-merge gate 既有 21 例零回退（六条例外逐字不动）；新增——零相交放行 / 已登记 §10c 形态 add/add → ② 级路由 / 未登记 add/add 及同 hunk 相撞 → ③ 级 / 探测失败 fail-closed → ③ 级 / ③ 派发不可用 → ④。**supervisor-verdict-gate 自检（B1 修正随附，与 pre-merge gate 电池分开陈述零回退）**：cross-system 摘除后 home-p1 形态 diff（firmware/.gitignore + firmware/main/** 单系统双段）→ riskTier 不再 high；schedule-p2 形态（firmware+server 双面板常规面）→ 不再因 cross-system ESCALATE；既有 battery 中 cross-system 断言两例（:627-628 `high-cross-system-diff` / `high-cross-system-subsystem`）改写为摘除断言，其余断言零回退。
- **V2 · merge-driver 自检扩**：既有 20 例零回退；新增——预探测「无取舍」但 rebase 实撞 → 升 ③（fail-closed）；② 级并集 + 双方齿复跑绿 → 过。
- **V3 · 端到端演练**：临时仓复刻 schedule-reminder/story-p3 双包抢 v3 场景全链——断言：探测命中 add/add → ② 级路由（不 ESCALATE）→ 并集消解 → 双方齿绿 → 自动过，全程零 human 触点。
- **V4 · 升级件内容面**：③/④ 级夹具断言升级件含逐文件相交清单 + 双方版本锚；④ 级含 architect 意见槽。
- **V5 · 投影 grep**：pod-coordination §10d 在场；DEV-GOVERNANCE 升级件句在场；cross-system 判据字面在 `supervisor-verdict-gate.mjs` 活代码零命中（`isCrossSystem` 摘除；pre-merge gate 本就无此字面——R1 亲核；历史制品除外）；queue ARQ-260907 行结移 §2。
- **V6 · 回归对照**：以 home-p1 / schedule-p2 的 supervisor-verdict + diff 为回放夹具——同 diff 输入下新判据不再 ESCALATE（误报归零实证），而构造的真跨系统同 hunk 相撞夹具仍落 ③/④ 级（不漏报实证）。回放输入自 git 历史完整重构（R1 实证：两 Pod 均真双亲 merge——home-p1 `20f1e09`、schedule-p2 `b7df3ab`——路径集 = `git diff --name-only <merge>^1 <merge>`、合并时点探测 = `git merge-tree --write-tree <merge>^2 <merge>^1`；两 Pod 实际共享 `firmware/main/CMakeLists.txt`，兼作 ②/③ 判据天然正例素材）。

## 6. 留根清单

- **取舍判据的机判实现精度**（同 hunk 判定的 diff 粒度）：v0.1 只钉语义判据，实现粒度（行级 / hunk 级 / 语义单元级）归 charter 期与评审循环定，实测不够准则降级「判不了 → ③ 级」为默认。
- **探测工具版本前提**：`git merge-tree --write-tree`（含 `--name-only` 冲突名单）要求 git ≥ 2.38——实现期首验本机版本（R1 C1/C2 注记；评审期未实测）。
- **supervisor-verdict payload schema**：T3 升级件内容面若需 schema 字段（对齐 friction-event-payload-schema ARQ 病族），最小 schema 留观测后批。
- **③ 级 architect 消费的派发面**：planner.md 派单格式（对齐 260908 批 F7/B-3 留根——planner 派单格式义务），归 planner.md 修订批。
- **B-8 重指排队行 / 姊妹批两条 draft**：与本批无关，既有排队在册。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect verdict：NEEDS-WORK（2026-09-08）

> 评审记录全文见 [architect-review-r1.md](architect-review-r1.md)。findings 计数：BLOCKING 1 / SHOULD 5 / COULD 3。Track B 降档正当性复核成立（D 节），不升 Track A。

**结论：NEEDS-WORK（Track B 内循环打回，R2 复核后可转 GREEN）**。核心问题 = 改法面与源码事实漂移，非三级梯方向问题（方向已由 human 立项裁定消费）。

**B1（BLOCKING · PROPOSAL↔源码漂移 + manifest 缺件 + 非目标内在矛盾）**：cross-system 判据物理宿主 = `.claude/hooks/supervisor-verdict-gate.mjs`（isCrossSystem :224-231 → classifyHigh :282-289 → deriveDiffTier :293-310 → decide :398-433 → ESCALATE），**不在** `pre-merge-escalation-gate.mjs`——其六条例外（契约 / off-main / blocked / riskTier / provenance / gov GREEN）亲核无 cross-system 组成项，T1「原六条例外清单的 cross-system 组成项移除」是事实错误。landing-manifest 缺 supervisor-verdict-gate.mjs；非目标「不动 supervisor 判据」与 T1 退役承诺矛盾——退役唯一实现路径就是改该文件的 riskTier 派生轴；V5「cross-system 字面在 gate 活代码零命中」按字面执行是空洞真（现在就不在 gate 内）。修法（属既裁意图保真，不新增设计）：manifest + §3 表增该文件；非目标改写为「不动 verdict 三态裁决规则与证据完整度判定；riskTier 派生轴 cross-system 退役」；V5 改锚；supervisor-verdict-gate 自检电池 cross-system 两例（:627-628）改写方案入验证集。

**逐条件（R2 收口，全文见评审记录 R2 复核范围节）**：
1. B1 修复三件（manifest / 非目标改写 / V5 改锚）+ 自检电池改写入 V1。
2. S1：§10d 须加 add/add carve-out——预登记 §10c 版本化形态 = 预裁定无取舍例外走 ②；未登记 add/add → 判不了 → ③（V1 自检案「add-add → ②」同步收口）。
3. S2：T1↔T4 探测结果传递面（gate 落 probe-result 制品 或 merge-driver import 模块）+ 闸输出契约（梯级 ①-④ 路由信号），接口形状 FROZEN 前钉死。
4. S3：补「③ 派发不可用 / 无消费方 → ④ escalate human」fallback 句（fail-closed，防 ③ 悬空退化盲批）。
5. S5：「前端对齐 + 例外反选」锚点二选一（DEV-GOVERNANCE §6 段内 vs AGENTS.md 治理路由节）+ manifest 对齐（落 AGENTS.md 则 G1 文件入 manifest）。
6. S4 + D 节两项 FROZEN 前 human 确认：非目标边界改写 + supervisor-verdict-gate.mjs（G3 hook 高敏）入 manifest 的确认；OQ1 / OQ3 补裁（头注 ack 只列 OQ2/OQ4，两项正文按推荐 (a) 编入未获显式裁）。

**降档复核（D 节结论）**：Track B 三要件核对成立（契约零触 / 三级梯方向 + OQ2/OQ4 已由 human 立项裁定消费、B1 属保真性漂移非新分叉 / 机械夹具齐——V6 回放已由评审实证完全可行，两 Pod 真双亲 merge 支撑合并时点探测与路径集双重复原）；ARQ 原行 Track A profile 建议系基于已被 human 否弃的旧两候选方向，已被立项裁定消费。判官不行使打回升 Track A 裁决权。

### R2 · architect verdict：GREEN（2026-09-08，对 v0.2）

> 评审记录全文见 [architect-review-r1.md](architect-review-r1.md) R2 节。**结论：GREEN**——R1 九条 findings（BLOCKING 1 / SHOULD 5 / COULD 3）在 v0.2 全闭合，human 三裁在册（B1 确认修法 + OQ1=(a) + OQ3=(a)，2026-09-08 终裁），新引入扫描无 BLOCKING/SHOULD（两处 B1 一致性残留随批修正：review 轨段措辞残留 + §3 表 supervisor 行矛盾）。残余计数：0 BLOCKING / 0 SHOULD / 0 COULD。GREEN 前置核对：landing-manifest 围栏块在场（六行，含 supervisor-verdict-gate.mjs）、packet-id 行在场。主线程可凭「立项 ack + GREEN」标 FROZEN，进入 packet-bound landing 序列（landing-verifier 独立核验为后续既定门，非本 verdict 豁免）。角色注记：v0.2 修订由本判官按 human 三裁 + R1 findings + 主线程逐条指令机械回灌执行，全部内容源自已裁来源、无自添设计；落地保真的独立核验由 landing-verifier 承担。降档正当性维持 R1 D 节结论不变。

## 8. Review 定稿记录（2026-09-08 · 判官机械回灌 v0.2）

v0.1 → v0.2 = 判官（architect）按 R1 九条 findings + human 三裁（B1 修法 / OQ1 / OQ3）机械回灌，逐条落位对照见 `architect-review-r1.md` R2 节闭合表；confirmed delta 对照源 = `decision-packet.md`。R2 = GREEN（0 BLOCKING / 0 SHOULD / 0 COULD）。作者≠判官保障链：R2 闭合性核对 + landing-verifier 独立核验（后续既定门）。落点六路径与 manifest 围栏块逐字一致。
