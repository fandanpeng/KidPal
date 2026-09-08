# architect-review-r1 · 260908-gov-review-escalation-conflict-ladder

> 判官：architect（Track B 评审循环 R1，只读 canonical、只写本记录 + PROPOSAL §7 verdict append）。
> 评审对象：PROPOSAL DRAFT v0.1（2026-09-08）。输入：PROPOSAL 全文 + ARQ-260907 行（ARCH-REVIEW-QUEUE.md:45）+ 源码亲核（pre-merge-escalation-gate.mjs / merge-driver.mjs / supervisor-verdict-gate.mjs / supervisor-gate-check.mjs / pod-coordination.md / DEV-GOVERNANCE.md / arch-review-protocol.md）+ 回放夹具亲核（home-p1 / schedule-p2 supervisor-verdict.json + c:/AI/KidPal git 历史实测）。

## verdict（先给结论）

**NEEDS-WORK（R1 打回，Track B 内循环，不升 Track A）**——findings 9 条：BLOCKING 1 / SHOULD 5 / COULD 3。核心问题不是三级梯方向（human 已裁），而是**改法面与源码事实漂移**：cross-system 判据的物理宿主不在 PROPOSAL 认定的文件里，landing-manifest 缺件 + 非目标自相矛盾。修复方向由已裁意图唯一确定（保真性修复，非新设计），修完走 R2 复核即可；两项条件须在 FROZEN 标定时向 human 显式确认（见 §D）。

## A. 判据换轴可行性核

**结论：可行，接入点与解析面均成立；V6 回放夹具完全可行（无须降档）；但 T1 对「cross-system 在哪」的定位错了（见 B1），且 T1↔T4 探测结果传递形态未定（见 S2）。**

- **接入点（buildCtx/execFn 注入形状）**：`pre-merge-escalation-gate.mjs` 的 `buildCtx` 已把 `execFn` 注入 ctx（:252、:282——后者是 260905 热修补的 live 承重字段），`parallelOverlapProbe(ctx)` 用 `ctx.execFn` 跑只读 `git merge-tree --write-tree --name-only <source> <target-main>` 形状成立。merge-driver Step 1b 以子进程逐 Pod 调闸（merge-driver.mjs:1249-1267，cwd=Pod worktree），时点在「同批前序 Pod 已并入 main」之后——§0 梯级① 的「含同批已并 Pod」语义被 Step 1b 的调用时序天然满足。先例实证：schedule-reminder review-packet.md:46 用 `git merge-tree --write-tree --name-only HEAD main` 产出 CONFLICT 名单（10 条，逐文件可数）——输出机械可判。
- **merge-tree 输出解析**：`--name-only` 模式列冲突文件名，普通模式逐行 `CONFLICT (add/add): Merge conflict in <path>` 可分类 add/add 与内容冲突。注意 `--write-tree` 干净合并时只吐 tree OID 不列文件——「相交文件全集」（② 级需要的非冲突相交）须另算两侧 `git diff --name-only <merge-base> <side>` 的交集，属实现细节，可留 charter 期。
- **与既有六条例外的编排关系**：探测是**新增组成件**，不替换六条中任何一条——但 PROPOSAL 头注与 T1 把 cross-system 说成「原六条例外清单的 cross-system 组成项」是**事实错误**（详见 B1）：六条 = 契约面 / off-main / blocked / riskTier / provenance / gov GREEN（gate 头注 :10-19、evaluate :165-236 亲核，零 cross-system 字面）。cross-system 在升级链的**上游**（supervisor-verdict-gate 的 riskTier 派生轴），不在本闸。
- **V6 回放夹具（可行性实证）**：完全可行，不须降档。两 Pod 均为真双亲 merge（--no-ff，`git log -1 --pretty=%P` 亲核：home-p1 `20f1e09` 双亲 `9276e24`/`e063fb1`；schedule-p2 `b7df3ab` 双亲 `46d99b5`/`6490495`）。回放输入可完整重构：① 路径集 = `git diff --name-only <merge>^1 <merge>`（亲核：home-p1 23 文件、schedule-p2 27 文件，含 firmware/server 实现 + 治理面）；② 合并时点探测 = `git merge-tree --write-tree <merge>^2 <merge>^1`（双亲正是当刻 main 与 Pod 侧）。旧 verdict 只需作「当时判 ESCALATE」的对照锚，diff 输入从 git 历史取、不依赖 verdict 内嵌。额外素材：两 Pod 实际共享 `firmware/main/CMakeLists.txt`（亲核双侧改动清单）——真实相交件，是 ②/③ 判据的天然正例夹具。

## B. 取舍判据（T2 §10d）可判性核

**结论：hunk 级机判部分可行、降级路径闭环；与 260908 add/add 特判存在一处须显式消解的口径矛盾（S1）；层级关系本身（特判=指引层、§10d=判据层）清晰。**

- **机判实现面**：merge-tree 给出「冲突存在」全集——③ 信号取「有任何冲突」是同 hunk 相撞的超集，向严方向正确（多收不漏真取舍）；② 级「相交但不冲突」须另算两侧对 merge-base 的 hunk 区间交（`git diff -U0` 解析 hunk 头两侧行号区间的交非空判定，plumbing 可做）。加一层实现复杂度，PROPOSAL §6 留根已如实预埋「实测不够准则降级『判不了 → ③』为默认」（:113）——降级路径闭环，向严安全。
- **add/add 盲区（须 carve-out，S1）**：add/add 无共同基，「双方版本对同文件 diff 的 hunk 区间」按字面两版本整文件即最大相交 → 归 ③；但 §0 梯级②把「并行包各写 `.vN` 载体不同段」列为 ② 典型（:31）、T2 括注同向（:65）、V1 自检案「add-add 相交 → ② 级路由」（:104）——三处与 §10d 字面口径矛盾。须一句显式裁定：预登记 §10c 版本化形态的 add/add = 预裁定无取舍例外（走登记表 + 并集指引），**未登记 add/add 形态 → 判不了 → ③**（fail-closed，对齐 260908 特判补集不变式）。另注：schedule-reminder 实证（review-packet.md:46、:71——add/add 需双侧冻结齿对照才敢定并集）按语义更贴 ③ 级（architect 确认并集策略、runbook 执行），PROPOSAL 把它全放 ② 偏宽，须收口。
- **与 260908 特判层级关系**：`VERSIONED_ADD_ADD_GUIDANCE`（merge-driver.mjs:932-935）只 widen blocked 指引文案、不 widen 放行（A-7 补集不变式，特判命中集 :895-903）——机械件层；§10d = ②③ 分界判据层。两层职责不重叠、无矛盾——**除 S1 一处**（PROPOSAL 自己的 §0/T2/V1 与 §10d 打架，不是与 260908 落地件打架）。

## C. ③ 级 architect 派发面核

**结论：OQ2=(a) 建议即地板成立；派单链路本批不闭环（有意留根），但缺 ③→④ fallback 兜底句（S3）。**

- **裁决权形态**：architect 出消解建议 + 可机判依据、不落地，执行归 ② 级 runbook 执行面（integrator 消解 + 双方齿复跑——§10a 消解纪律五条现成承载，pod-coordination.md:120-138）——对偶 author≠judge，与 dev-loop「planner 不写 verdict、fixer 修红」同构。成立。
- **派单链路**：PROPOSAL §6 明示 planner.md 派单格式归 planner.md 修订批（:115，对齐 260908 批 F7/B-3 留根）——即本批落地时 ③ 级只有「升级件带 architect 意见槽」（T3 / V4）而无机械 dispatcher。这不是错，但**必须**配显式 fallback：「③ 派发不可用 / 无消费方 → 视为判不了 → ④ escalate human」（fail-closed），否则 ③ 悬空、行为退化为现状盲批。PROPOSAL 未写此句（S3）。

## D. 降档正当性复核（独立裁决）

**结论：Track B 降档成立，我不行使打回升 Track A 的裁决权；但 B1 修复涉及非目标边界改写 + G3 hook 入 manifest，FROZEN 标定时须向 human 显式确认。**

- 三要件核对：①契约面零触 ✓（manifest 无 contracts、§3 表「未触（零）」）；②无未决设计分叉——三级梯方向 + OQ2/OQ4 已由 human 2026-09-08 立项讨论裁定消费（ARQ 原两候选「收紧目录计数 / 接受恒批」双双否弃、第三方向 human 亲裁）；B1 是保真性漂移而非新分叉（修法方向由已裁意图唯一确定：判据换轴 = cross-system 在其真实宿主退役）；③改动面全有机械夹具 ✓（V1/V2 自检扩 + V3 端到端 + V6 完整回放——本评审 A 节已实证 V6 可行）。
- ARQ-260907 行内「review profile：full-adversarial + human（Track A）」（ARCH-REVIEW-QUEUE.md:45）是 2026-09-07 登记时点的 architect 建议且系基于旧两候选方向；2026-09-08 human 立项已否弃两候选、裁第三方向并质询降档——该 profile 建议已被立项裁定消费。AGENTS.md 升级判据（触契约 / 跨系统结构性变更）均不命中，governance 类不强制升级；gate 语义改造以「gate 事实如实声明」承载有先例（260908 批 T5 CDQ 行 A-2 裁「门内加项」）——本批力度大于门内加项（判据换轴），但 human 在立项 ack 中知情（头注 :3 记录质询与复判）。降档归因成立。
- 阈值核对：改门力度 → 全套 Track B 流程（立项 ack + architect 评审 + FROZEN + packet-bound landing + landing-verifier + retro）——本批正在走全套，符合 DEV-GOVERNANCE §4 阈值行。
- **两项 FROZEN 前须 human 显式确认**（存疑向严，非升 Track A）：①非目标改写（「不动 supervisor 判据」→「不动 verdict 三态裁决规则；riskTier 派生轴 cross-system 退役」）+ manifest 增补 supervisor-verdict-gate.mjs（G3 hook 高敏面）；②OQ1 / OQ3 未在 ack 中显式裁（头注 :3 只列 OQ2/OQ4），两项均按推荐 (a) 编入正文——FROZEN 标定时一句确认或明示并入立项 ack 覆盖。

## findings（按严重度排序）

| # | severity | kind | 位置 | 摘要 |
|---|---|---|---|---|
| B1 | **BLOCKING** | PROPOSAL↔源码漂移 + manifest 缺件 + 内在矛盾 | PROPOSAL :12-18（manifest）/ :38（非目标）/ :59-61（T1）/ :91（硬项1b）/ :108（V5）；supervisor-verdict-gate.mjs :213-231（distinctAppSystems/isCrossSystem）→ :282-289（classifyHigh）→ :293-310（deriveDiffTier）→ :398-433（decide→ESCALATE） | cross-system 判据物理宿主 = `.claude/hooks/supervisor-verdict-gate.mjs`，**不在** pre-merge-escalation-gate.mjs（其六条例外亲核无 cross-system 字面）。「原六条例外清单的 cross-system 组成项移除」是事实错误——六条 = 契约/off-main/blocked/riskTier/provenance/gov GREEN。后果链：landing-manifest 缺 supervisor-verdict-gate.mjs → 按现 manifest 落地则核心承诺（cross-system 退役）静默不发生；非目标「不动 supervisor 判据」（:38）与 T1 退役承诺（:61）矛盾——退役唯一实现路径就是改 supervisor-verdict-gate 的 riskTier 派生轴；V5「cross-system 字面在 gate 活代码零命中」（:108）按字面执行是空洞真（现在就不在 gate 内），须改锚 supervisor-verdict-gate.mjs。附带：supervisor-verdict-gate 自检电池含 cross-system 断言两例（:627-628 `high-cross-system-diff` / `high-cross-system-subsystem`），退役须同批改写（「既有自检零回退」承诺须按文件分开陈述——pre-merge gate 21 例零回退成立，supervisor-verdict-gate 电池必变）；supervisor-gate-check.mjs FIX-6 第二方重算 import 同模块纯函数（:35、:759），单源自动一致、不须单独改，但 sweep 应点名。**修法**：manifest + §3 表增该文件；非目标改写为「不动 verdict 三态裁决规则与证据完整度判定；riskTier 派生轴 cross-system 退役（派生轴 = 判据换轴本体）」；V5 改锚。修复属既裁意图保真、不新增设计。 |
| S1 | **SHOULD** | 跨制品口径矛盾（§0/T2/V1 vs §10d 字面） | PROPOSAL :31（梯级②典型）/ :65（T2 括注）/ :104（V1 自检案）；merge-driver.mjs :895-903（特判命中集）/ :932-935（指引文案）；pod-coordination.md §10c（:146-165） | add/add 无共同基，§10d「hunk 区间不相交」按字面把 add/add 归 ③（两版本整文件 = 最大相交），但 §0/T2/V1 三处把「.vN 载体不同段」add/add 放 ②。须显式 carve-out：预登记 §10c 版本化形态 add/add = 预裁定无取舍例外走 ②；未登记 add/add 形态 → 判不了 → ③（fail-closed）。schedule-reminder 实证（review-packet.md:46/:71）语义上属 ③（需双侧齿对照定并集），PROPOSAL 全放 ② 偏宽。 |
| S2 | **SHOULD** | 接口形状未定（T4 成立前提） | merge-driver.mjs :1249-1267（Step 1b execFileSync，成功时 stdout 被丢弃）；PROPOSAL :59-61（T1）/ :71-73（T4） | T4 要 processPod「复用 T1 探测结果」，但 Step 1b 以子进程跑闸、只吃 exit code——探测结果须有耐久传递面（gate 落 probe-result JSON 制品，或 merge-driver import 探测模块）；且闸现为二值 exit 0/1，梯级 ①-④ 路由须定义输出契约（exit 分档或制品字段）。实现细节可留 charter 期，接口形状是 FROZEN 前置。 |
| S3 | **SHOULD** | 链路缺口无兜底 | PROPOSAL :67-69（T3）/ :115（§6 留根 planner.md 派单格式归后续批） | 本批落地后 ③ 级无机械 dispatcher（派单格式归 planner.md 修订批）。须显式 fallback 句：「③ 派发不可用 / 无消费方 → 判不了 → ④ escalate human」，否则 ③ 悬空退化为现状盲批。 |
| S4 | **SHOULD** | human 取舍点未闭合 | PROPOSAL :3（头注只列 OQ2/OQ4）/ :97-99（OQ1/OQ3） | OQ1（齿复跑范围）/ OQ3（探测频率）未记录 human 裁定，正文按推荐 (a) 编入。FROZEN 前补一句 AskUserQuestion 确认，或 human 在 FROZEN 标定时合并确认（存疑向严）。 |
| S5 | **SHOULD** | 锚点名不存在 + manifest 缺件风险 | PROPOSAL :75-77（T5）；DEV-GOVERNANCE.md :224（该短语只在 §6 段内，无同名节）；AGENTS.md「治理路由」节 | 「前端对齐 + 例外反选」在 DEV-GOVERNANCE 不是节名（是 §6 landing 通道段内短语）；同名标题在 AGENTS.md 治理路由节。T5 一句若落 AGENTS.md（G1）则 manifest 缺 G1 文件；若落 DEV-GOVERNANCE §6 段内则锚点须改写。二选一并 manifest 对齐。 |
| C1 | COULD | 证据引证精度 | home-p1 supervisor-verdict.json :13/:22/:39；merge `20f1e09` 改动清单亲核 | P1「判据对双面板 Pod 恒真」对 home-p1 不精确：其 reason = `cross-system:impl=.gitignore,main`，实际改动 = `firmware/.gitignore` + `firmware/main/**`——**纯固件单系统也触发**（distinctAppSystems 数第二段目录多样性）。此实况比 PROPOSAL 归因更强（换轴论据加码），但引证应逐字对齐。schedule-p2 引证精确吻合（`cross-system:impl=main,acceptance,deploy,gateway`，改动含 server/{acceptance,deploy,gateway} + firmware/main，27 文件亲核）。 |
| C2 | COULD | 表述精确化 | PROPOSAL :61（「只读」）；schedule-reminder review-packet.md :46（先例形态） | `git merge-tree --write-tree` 不动工作树/refs，但向 ODB 写 dangling tree/blob 对象（GC 可回收）——「只读」可加半句精确化；须记录 git ≥ 2.38 前提。`--name-only` 输出机械可判已实证。 |
| C3 | COULD | V6 形态确认（正向） | `git log -1 --pretty=%P 20f1e09` / `b7df3ab` 亲核 | V6 完全可行无须降档：双亲真 merge 支撑「路径集 + 合并时点探测」双重复原（见 §A）。两 Pod 共享 `firmware/main/CMakeLists.txt` = 天然 ②/③ 正例素材，建议 V3/V6 夹具直接用。 |

## coverage（未审面，诚实列）

- 未逐行审 260908-gov-review-merge-chain-hardening 全部落地件与 review-charter 细节——只核与本批接口直接相关的面（add/add 特判命中集/指引文案/§10c 登记表/§10a 五条）。
- 未审 planner.md / integrator.md 全文——只按 §6 留根指针确认「派单格式归后续批」留根在册。
- 本评审为设计评审：新判据尚无实现，未实跑任何新探测代码；`git merge-tree --write-tree` 在本机 git 版本的实测留给实现期（版本前提记 C2）。
- supervisor-gate-check.mjs 只核 import 与 recompute 面（:35/:759），未全文读。
- 未跑 `--self-check` 实证 21/20 例计数（按源码逐条点数核对：pre-merge gate 自检 21 例、merge-friction 自检 20 例，与 PROPOSAL V1/V2 声明数一致）。

## R2 复核范围（打回后收口条件）

1. B1 修复：manifest / §3 表增 `.claude/hooks/supervisor-verdict-gate.mjs`；非目标改写；V5 改锚；supervisor-verdict-gate 自检电池 cross-system 两例的改写方案入验证集（V1 按文件分开陈述零回退承诺）。
2. S1 carve-out 句 + V1 自检案收口（预登记形态 → ②，未登记 add/add → ③）。
3. S2 接口形状钉死（传递面 + 输出契约，二选一写进 T1/T4）。
4. S3 fallback 句（③ 派发不可用 → ④）。
5. S5 锚点二选一 + manifest 对齐。
6. S4 / D 节两项 FROZEN 前 human 确认项，由主线程在 FROZEN 标定时执行（非本评审可代）。

---

# R2 · architect 复核（2026-09-08，v0.2）

> 输入：PROPOSAL DRAFT v0.2 + human 三裁（coordinator 转达在册：B1 确认修法〔非目标改写「不动 verdict APPROVE/BLOCK 语义；cross-system 退役实现路径 = 改 supervisor-verdict-gate 的 riskTier 派生轴」+ hook 入 manifest〕；OQ1=(a) 双方冻结件+全档；OQ3=(a) 仅合并门时点）。
> **角色注记（诚实披露）**：v0.2 修订由本判官按 human 三裁 + R1 findings + coordinator 逐条指令机械回灌执行（主线程派发），全部修订内容源自已裁来源、无自添设计；R2 复核 = 对 R1 findings 闭合性的逐条核对 + 新引入扫描。落地保真的独立核验仍由 Track B 既定的 landing-verifier 承担（作者≠判官的最终保障不在本 R2、在 landing 腿）。

## R2 verdict：**GREEN**（R1 九条 findings 全闭合，无新引入 BLOCKING/SHOULD；残余 0 BLOCKING / 0 SHOULD / 0 COULD）

## 逐条闭合核对

| R1 finding | v0.2 闭合位置 | 核对结论 |
|---|---|---|
| B1（BLOCKING） | manifest :15（hook 入清单）；非目标 :39（按 human 终裁原文改写，保留 APPROVE/BLOCK 语义边界）；T1 :62（宿主改述：isCrossSystem :224-231 → riskTier 派生轴摘除、保留其余轴、FIX-6 单源注记）；§3 表 :84-85（hook 行 + 升级闸行改述、顺带修正 v0.1 残留的「Track A 承载」陈旧表述）；V1 :106（supervisor-verdict-gate 自检项：home-p1 / schedule-p2 形态 + :627-628 两例改写、按文件分开陈述零回退）；V5 :110（改锚 supervisor-verdict-gate）；review profile :8；硬项 1b :93（R1 核对结果落档：随批改 = :226-229 + :627-628；通用语与历史制品保留） | **闭合** |
| S1 | T2 :66（add/add carve-out：已登记 §10c 形态 → ②、未登记 → ③ fail-closed 对齐 A-7）；§0 梯级②行 :32 同步注；V1 :106 自检案收口（「已登记 → ② / 未登记及同 hunk → ③」）；V3 :108 场景（双包抢 v3 = 已登记形态 → ②）与新口径一致 | **闭合** |
| S2 | T4 :74（传递契约：gate 可选参 `--probe-out <path>` 落 JSON 制品、显式文件非 stdout 解析、Step 1b 丢弃 stdout 现状不改）+ T1 :62 交叉引用 | **闭合**（接口形状已钉死；`--probe-out` 为 coordinator 指定、human 三裁语境内的主线程设计裁量） |
| S3 | T2 :66 尾（③ 派发不可用〔失败/超时/无消费方〕→ 直接 ④ fail-closed）+ T3 :70（升级件落 ④ 形态） | **闭合** |
| S4 | §4 :99 / :101（OQ1/OQ3 human 2026-09-08 终裁 (a) 落档，头注 :3 同步） | **闭合** |
| S5 | T5 :78（落点改述 DEV-GOVERNANCE §6 例外升级拍板段内、非独立节；manifest 已含该文件零增改） | **闭合** |
| C1（证据精度） | P1 :48（home-p1 单系统双段误伤实况 + 两 Pod 改动文件数亲核入证） | **闭合** |
| C1（git 版本） | §6 留根 :116（git ≥ 2.38 前提 + 实现期首验） | **闭合** |
| C2 | T1 :62（ODB dangling 对象精确化注记） | **闭合** |
| C3 | V6 :111（双亲重构法 + CMakeLists 正例素材入证） | **闭合** |

## 新引入扫描

逐节通读 v0.2 全文（PROPOSAL :1-142）：新增内容全部为已裁来源的回灌（human 三裁 / R1 findings / coordinator 指令），未发现新设计分叉、未触契约面、未新增 human 触点。扫描中发现并已随批修正两处 **B1 一致性残留**（属 B1 闭合范围、非新 finding）：①review 轨段 :4「cross-system 组成项退役」措辞残留（v0.1 错误归因的措辞尾巴，已改为「cross-system 判据退役（物理宿主 = supervisor-verdict-gate riskTier 派生轴）」）；②§3 表「supervisor 判据 | 未触」行 :91 与新 hook 行矛盾（已改为「supervisor verdict 语义未触（APPROVE/BLOCK 裁决规则与证据完整度判定不动；riskTier 派生轴退役见上行 hook 行）」）。

## GREEN 前置核对（PROPOSAL 结构字段硬条款 :10）

- landing-manifest 围栏块在场 ✓（:12-19，六行，含 supervisor-verdict-gate.mjs）；
- packet-id 行在场 ✓（:6，§2 改法集 T1-T5 + §5 验证集）。

## GREEN 结论

R1 全部 findings（BLOCKING 1 / SHOULD 5 / COULD 3）在 v0.2 闭合；human 三裁在册（B1 修法 + OQ1/OQ3 终裁）；无新引入。**verdict: GREEN**——主线程可凭「立项 ack + GREEN」标 FROZEN，进入 packet-bound landing 序列（landing-verifier 独立核验为后续既定门，非本 verdict 豁免）。降档正当性维持 R1 D 节结论不变（v0.2 修订未触及三要件任何一项）。
