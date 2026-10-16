# PROPOSAL · 260921-gov-review-launch-pipeline

> **状态：FROZEN v1.0 · 2026-09-21 · 可进 landing**（凭据：**human 2026-09-21 立项 ack**（前端对齐：需求 = 把这条链缝成一条 / 方向 = T1–T3+T5+T6 落地 + T4 登记 / 量级 = 10 文件、low-medium、零契约零新增门）；**architect R3 = GREEN**（§7 `### R3 - 2026-09-21 - architect`，行首 `verdict: GREEN`；R1 = `BLOCK(F1,F2,F3,F5)`、R2 = `BLOCK(F8)`，其 F1–F9 全部闭环）；**human 三条裁定** = OQ-1 **(a)** 归拆包侧 / OQ-2 **(c)** 不做只登记 / R1-F5 **(a)** 接受 `ARQ-260918` 部分承载；**FROZEN 前结构门** = `node .claude/scripts/landing-fidelity-check.mjs --preflight <本件>` → `ok:true, anchorCount:10, findings:[]`（2026-09-21 主线程复跑）。**FROZEN = 内容冻结**——本节以下正文与 §2 改法集自此不可回改，landing 须逐字照此执行）。
> **v0.4 → FROZEN v1.0**：内容未变；仅状态行由 DRAFT 转 FROZEN 并记入三类凭据。R1/R2/R3 全文见 §7。
> **v0.3 → v0.4 修订留痕（R2 打回项）**：**F8**（T3 改法因 F1 扩面触及 `planner.md`，但该 T 的目标文件锚清单与 §3 sweep 的 planner.md 行**未同步**——「承重指令引未锚文件」，违 §2 硬项）→ 两处均已补 `planner.md`（含 `:3` / `:12` 的触面说明）。**F9**（NIT：T3 证据段/改法对 `planner.md:3` 的原文引用多一「只」字，实为「…workstream **归** portfolio-planner、不归你。」）→ 已逐字订正。
> **⚠ 主线程对 §7 的一次误伤与还原（诚实记录，请 R3 判官复核）**：主线程在订正 F9 时用了一次**全局字符串替换**，该替换同时命中了 §7 内 R2-F9 那条 finding 的**引文片段**（判官 append-only 区）。主线程随即发现并**逐字还原**该行引文（`…归 portfolio-planner…` → `…只归 portfolio-planner…`），使 R2-F9 记录恢复其原始的、自洽的陈述形态。**教训**：判官记录区内含被订正文本的引文时，机械全局替换会穿透 append-only 边界；后续修订一律以**定点 Edit（唯一锚）**而非全局替换执行。此处如实披露，不掩。**除该一处引文片段外，§7 的 R1/R2 全文未作任何改动。**
> **v0.2 → v0.3 修订留痕（R1 打回项）**：**F1**（T3 降级形枚举不完整——`planner.md:3` / `:12` 漏承接，V6 零命中不可达）→ T3 改法第 1 条扩及全部 4 处降级形 + 证据段「三处文面」改「4 降级 vs 5 正确」全量枚举 + V6 口径订正 + V10 同步（v0.1「不改任何 frontmatter」句作废）。**F2**（启动卡计数 24→**25**、存量文件 27→**28**，并补第三种物种 `headless-*.log`）→ §0/T2 问题/T2 证据/§3/§6 五处订正。**F3**（`landsIn: "package:<id>"` 误记 15 处，实为 **7**）→ T6 证据段订正并给出全分布。**F4/F6/F7**（NIT）→ V6 括注、V11「11 路径」、§3 census `devloop/launch`=3→**2** 均已订正。**F5**（升级 human）→ 见 §4「R1 升级项」。
> **承载 ARQ**：`ARQ-260914-portfolio-reconcile-launchpreconditions`（T6 承载；human 2026-09-20 裁定自 `260920-gov-review-reader-scripts-align` 移出、并入本批）、`ARQ-260918-pod-session-settings-identity-unobservable` 的**候选①**（T3 承载；候选②③ 不在本批，见 §6 留根②）。
> **本批定位**：这条链（RTE 拆包 → launch 面准备 → Pod 起步）**从来没有被当成一条链定义过**。它由四段半成品拼成——portfolio-planner 的 Step 1–8（只管前半）、`kidpal-pod-launch-prep` skill（只管中段、**无执行者**）、planner 的 Step 1–9（只管后半）、加若干 ARQ 补丁。本批做的是**把它缝成一条**：给中段一个 owner、把三个「产物的家」安正、把两处靠散文承载的机械语义收回到机械面。
> **review 轨**：**Track B 常驻轻路径**（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。
> **轨道自评（「存疑向严」义务）**：直接 Track A 的两条判据——① 触 `docs/contracts/**`：**未命中**；② 跨系统结构性变更：**未命中**（本批全部落在 `docs/DEV-GOVERNANCE.md` + `.claude/**` 的治理面内）。规则原文「**governance 类变更（含 self-def / review-loop agent defs）不再强制升级**」⇒ 默认 Track B。human 2026-09-21 在分类环节被显式询问「Track A（声明红队）vs Track B」并选定 **Track B**；本批不声明红队。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（**T1 / T2 / T3 / T5 / T6** 落地型 + **T4** 登记型·零落地）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**low-medium**（一句话证成：T1/T2/T3/T5 是**文本对齐与选址修正**——不改任何判定语义；T6 是**advisory 检查器扩读 + 数据模型迁移**，不改任何门的判定语义。T4 经 OQ-2 裁 (c) 后**零落地**，其"闸向严"面随之消失，故整批**无向严项**，取值由 low-medium 的本位理由 = T6 触数据模型 + 本批触 G1 指令根。契约面零改动。）
> **review profile**：**G1 + G2 + G3**（枚举：G1 = `docs/DEV-GOVERNANCE.md`；G2 = `.claude/{agents,references,rules,skills}/**`；G3 = `.claude/scripts/**`）。**契约面（`docs/contracts/**`）= 零改动；工程门 = 零新增**。另有一处 **spec 面指针行**（`docs/architecture.md:301`）——human 2026-09-21 裁定「**不算本批的 spec lane 触面，随批一起改**」，故走同一 packet、不另开 spec review。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/rules/repo-structure.md
.claude/scripts/portfolio-reconcile.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
docs/DEV-GOVERNANCE.md
docs/architecture.md
```

（**v0.2 裁剪留痕**：v0.1 曾含 `.claude/scripts/firmware-build-check.mjs` 作条件锚；human OQ-2 裁 **(c) 不做、只登记** ⇒ 该锚已自本清单裁剪，T4 转为零落地（见 §2 T4 与 §6 留根⑦）。）

---

## 0. 想实现的效果与差距

**目标效果一句话**：让「拆包 → launch 面准备 → Pod 起步」这条链**在 canonical 文本里完整存在一次**——中段有 owner、三个产物的家有单一归属、两处靠散文承载的机械语义回到机械面。

**现状差距**：这条链今天没有一个地方被完整叙述。human 2026-09-21 的原话是「这段有点不清晰，流程，感觉不顺」——**不顺不是某一步写错了，是这条链从来没有被当成一条链来定义**。证据：`.claude/skills/kidpal-pod-launch-prep/SKILL.md` 定义了中段的全部八项产物与九个步骤，但**全仓没有任何 agent 或 command 引用它**；`portfolio-planner.md` Step 6 标题写「交 launch」而正文不写谁来接；`DEV-GOVERNANCE.md` §4 的 Dev Loop 图把这一段画成一条**无名边** `PP -->|launch| PL`。三段各由不同文本片段承载，靠人的记忆缝起来——「我记得没错的话，大概是这个流程」正是这个状态的准确自陈。

**六处接缝**（**T1 / T2 / T3 / T5 / T6 在 §2 逐条展开；T4 经 human 裁定为登记不硬治，整条见 §6 留根⑦**）：

| # | 接缝 | 一句话 | 处置 |
|---|---|---|---|
| T1 | 链的中段没有 owner | skill 是孤儿；图上的中段是一条无名边 | §2 落地 |
| T2 | 启动卡的家在版本控制外 | 无历史、无分发、无机械消费者；且现场已被跳过（260920 两包无卡） | §2 落地 |
| T3 | 启动卡没有 canonical 命令行 | **4 处降级形 vs 5 处正确形**互不一致，且承诺的落点（board Launch 节）**根本不存在** | §2 落地 |
| T4 | 严格编译模式的开关靠一句话 | 卡上声明 `VERIFY_FW_BUILD_REQUIRED=1`，全仓零注入点 | **裁 (c)：登记不硬治，零落地** → §6 留根⑦ |
| T5 | input-brief 生成时点三方打架 | 文本说 Pod 内 / 闸在 finalize 查 / 实况在拆包侧 / skill 一字未提 | §2 落地（裁 (a) 归拆包侧） |
| T6 | `launchPreconditions` 缺两个位 | 无「核过没」位、无「属哪个包」位（= `ARQ-260914`） | §2 落地 |

**非目标（本批不做什么，防 scope creep）**：

- **不改任何门的判定语义**。六 T 无一新增工程门；T6 只给一个 advisory 检查器**增读**一个键（扩读、非门扩齿）。
- **不动 `docs/contracts/**`**（零改动）。
- **不重做中段的操作内容**——worktree 池协议、固件环境等价三件套、专属图索引、基线差异判停这些**已实测有效的步骤原样保留**，本批只给它们一个归属与坐标。
- **不追改历史制品**：`.git/devloop/launch/` 下的 28 份存量文件（25 份启动卡 + 3 份 `ruling-*`；另有 1 份 `headless-*.log`）、各 workstream `progress.md` 里以「launch prompt」为名的历史记录，**一律不回改**（append-only 纪律）。
- **`ARQ-260918` 只部分承载、该 ARQ 不结**：T3 落其候选① 的**前半**（canonical 命令行 + 全仓文面统一），**后半**（非 CLI 形态的降级面标注）deferred 至 §6 留根⑥；**候选②③ 不在本批**（planner intake 设置在场性自检 / `supervisor-gate-check.mjs` 手跑钉成步骤）。该 ARQ 行自评 `full-adversarial + human`，本批以 Track B 承载其一臂的落差**已作为升级项交 human 确认**（见 §4「R1 升级项」）。
- **不迁移存量 `launchPreconditions` 数据**——见 T6 改法第 5 条与 §6 留根①。
- **不并入 workstream INDEX 漏报问题**（`m1-home-qa` / `m2-char-window` 在顶层但 INDEX 报「Active (0)」）——边界外，见 §6 留根③。

## 2. 问题、根因与改法（T1…T6）

**【硬项 · 起草期实证义务】** 本节的断言纪律：凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；凡「全 / 均 / 无」量化声明 → 现跑取证；凡数字引用 → 起草期现跑实测或核对权威原处。**本批全部实测均为 2026-09-21 于本 worktree（`C:/AI/KidPal-gov`，branch `gov/kidpal`）现跑**；引用既有制品处逐条注明出处与其自身时点。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」（该 T 落地时要改/新增的仓内文件；条件触发的标触发条件）。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单；**缺锚清单 = 门 RED 点名缺锚**（fail-loud，不假绿）。改法标题一律 `### T<n> · <标题>` 标题形态；landing-manifest 围栏块一行一个裸路径、不带括注。

**条目形态（五段固定式）**：见 `docs/templates/PROPOSAL.template.md` §2「条目形态」——**问题 / 证据 / 根因 / 改法 / 〔第五段·锚清单〕**。

**【起草期自检两句】** ① FROZEN 标注前跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 并通过 §5 验证集命令实跑；② 本批涉**术语统一与路径迁移**（T2：`launch prompt` → 启动卡、`.git/devloop/launch/prompt-<changeId>.txt` → `delivery/taskpackages/<changeId>/launch-card.md`），故 §3 的**【硬项 · 复扫模式完备性先验】**已履行（全形态枚举见该节）。

### T1 · 链的中段没有 owner——`kidpal-pod-launch-prep` skill 是孤儿

**问题**：从 portfolio-planner 交 launch 到 planner 起步之间，存在一整段工作——调度判定、`--check-launch` 机械门、worktree 复用/新建、固件环境等价化、专属图索引、board 预填、启动卡。它有完整的操作手册（`kidpal-pod-launch-prep` skill：八项产物、九个步骤），但**这个手册没有任何执行者**：全仓 grep 在 `.claude/agents/` 与 `.claude/commands/` 下**零命中**，唯一入边是 `pod-coordination.md` 一句「操作手册 = 该 skill 步骤 3」（只指换包协议那一条）。同时 `portfolio-planner.md` Step 6 标题写「**交 launch**」，正文里"谁来接"没有下文；`DEV-GOVERNANCE.md` §4 的 Dev Loop 图把这一段画成一条**无名边**。**影响面**：整条链没有一处完整叙述——三段各由不同文本片段承载，中间那段的归属只活在约定与人的记忆里，每批 launch 都要重新拼一次。

**证据**（2026-09-21 现跑）：

- 孤儿实测：`grep -rn "pod-launch-prep\|launch-prep" .claude/agents/ .claude/commands/` → **零命中**。
- 唯一入边，verbatim（`.claude/references/pod-coordination.md:51`）：「换包协议四条（操作手册 = [`kidpal-pod-launch-prep` skill](../skills/kidpal-pod-launch-prep/SKILL.md) 步骤 3）：」——只指向换包协议（§2c），不指向整条链。
- 无名边，verbatim（`docs/DEV-GOVERNANCE.md:128`）：`    PP -->|launch| PL`。
- RTE 侧止步，verbatim（`.claude/agents/portfolio-planner.md:50` 起首）：「**③ 交 launch**：把 ready 的 TaskPackage 交 human/script/CI 做 `git worktree add`（自上述 base ref）+ 起 `planner` 会话」——"交"的动作写了，**接管方没写**。
- skill 的八项产物（`kidpal-pod-launch-prep/SKILL.md`「产物（完成标准）」节，verbatim 第 3–7 项）：「3. 每个包：运行时 worktree（复用空闲树 ∨ 按需新建…）／4. 每个包：固件环境等价化…／5. 每个包：专属图索引 `C-AI-KidPal-pod-<suffix>`…／6. board（`devloop/pods/<changeId>.json`）预填 worktreePath／7. launch prompt 落 `.git/devloop/launch/prompt-<changeId>.txt`」——**没有一项被任何 agent 认领**。
- skill 与 RTE 在「调度判定」上重叠，verbatim：skill 步骤 1「**调度判定**…规则 = dependsOn 全部 merged（或 PROPOSAL §3 明示的并行放宽条款）∧ mergeOrder 升序 ∧ WIP 余量」vs `portfolio-planner.md` Step 5「**Scheduling**：每个 TaskPackage 用显式命令初始化到 board…设 WIP limit」——两侧都在定"哪几包进本批"，且**没有一处说明两者的关系**（见 §6 留根④）。

**根因**：skill 是随 `260915-gov-review-worktree-pool-reuse` 批与更早的 launch 可达性批**逐步长出来**的操作沉淀，产方是"实际做这件事的人"（主线程）；而 RTE 的 Step 6 在它成形之前就写定了。两边都在，但**没有任何一次改动负责把两者缝上**——因为每次改动都只动了其中一边。这是典型的分头演进：文本的每个局部都自洽，缺的是把它们放在一起看的那一眼。

**改法**：

1. `docs/DEV-GOVERNANCE.md` §4 Dev Loop 图让中段在 canonical 图上显形。**承重 landing 指令（原文引用钉死 + 替换后完整句）**：
   - **替换目标文本 A**（现值 `docs/DEV-GOVERNANCE.md:128`）：`    PP -->|launch| PL`
   - **替换后**（三行，插在 `PP` 节点行与 `PP -->|launch| PL` 原位置）：
     ```
         LP["② launch 面准备 · 主线程（操作手册 = kidpal-pod-launch-prep skill）"]
         PP -->|拆包制品 commit 到 base ref| LP
         LP -->|worktree + 环境 + 图索引 + 启动卡| PL
     ```
   - **联动指称（同图内两处节点标签，必须同批改，否则编号自相矛盾）**：
     - `    PL["② planner Pod ×N · acceptance→builder→verifier↔fixer→integrator→planner 收尾(pod-retro)→supervisor"]` → 标签首字 `②` 改 `③`
     - `    LR["③ loop-retro · 跨 Pod 提共性"]` → 标签首字 `③` 改 `④`
   - **联动指称（§4 正文散文编号，同批改）**：该节「**怎么做**」句现值起首为「⓪ 开工前 `/triage-change` 比例化分档（拆包 / Full / Lite / 直接修）→ ① RTE 拆包调度 → ② planner 带单 Pod：…」——在「① RTE 拆包调度 →」与「② planner 带单 Pod」之间插入「② launch 面准备（主线程，操作手册 = `kidpal-pod-launch-prep` skill）→」，原「② planner 带单 Pod」改「③」、原「③ `/loop-retro` 周期性跨 Pod 复盘提共性」改「④」。
   - **同节末句联动**：现值「治理类进 ARQ、回流 ① intake，构成**自改进回路**」中的「回流 ① intake」**不改**（① 仍是 RTE 拆包调度，编号未动）。
2. `.claude/agents/portfolio-planner.md` Step 6 补显式交接句，钉死边界。**承重 landing 指令**：在现末句「你标记 ready 即完成本轮——**不轮询、不常驻**。」**之前**插入一句：
   > 中段（launch 面准备：调度批次判定、`--check-launch`、worktree、固件环境、专属图索引、board 预填、启动卡）的**执行主体 = 主线程（human 会话）**，操作手册 = `kidpal-pod-launch-prep` skill；你标记 ready 即交出，不执行中段、不替它决定批次。
3. `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 开篇（现首段「# KidPal Pod Launch Prep（task package 开发环境准备）」之下）补一行坐标：执行主体 = 主线程（human 会话）；本手册 = `docs/DEV-GOVERNANCE.md` §4 Dev Loop 图的 **② launch 面准备** 阶段操作手册。
4. `docs/architecture.md:301` 的指针行随批更新（human 2026-09-21 裁定不算独立 spec 触面）。**承重 landing 指令**：现值句尾「（操作协议见 kidpal-pod-launch-prep skill）」→ 改为「（launch 面准备操作协议见 `kidpal-pod-launch-prep` skill，= Dev Loop ② 阶段；见 `docs/DEV-GOVERNANCE.md` §4）」。

目标文件：`docs/DEV-GOVERNANCE.md`（改）· `.claude/agents/portfolio-planner.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）· `docs/architecture.md`（改）

### T2 · 启动卡的家在版本控制之外，且现场已被跳过

**问题**：启动卡落 `$(git rev-parse --git-common-dir)/devloop/launch/prompt-<changeId>.txt`，skill 自述「在 git common dir，**不入版本控制**」。三个后果：① **无历史、无 diff、无可评审面**（写错了没人能看见）；② **不能跨机分发**（common dir 是本机单一 `.git`，换台机器就没有）；③ **无机械消费者**——同目录的 board 有 `portfolio-reconcile.mjs` 维护，启动卡只有人读，腐坏无人知道。更关键的是它**在现场已经退化成可选项**：现存 25 份启动卡中最新一份是 `prompt-260917-m2-k3-char-window-overlay.txt`，而随后**两个包（260920 两包）根本没有卡**——树池复用后是「同树续跑」，人在既有会话里说一句话就起了。而 skill 的产物清单第 7 项仍把它写成必办。

**证据**（2026-09-21 现跑）：

- 落点与自述，verbatim（`kidpal-pod-launch-prep/SKILL.md:107`）：「写 `.git/devloop/launch/prompt-<changeId>.txt`（在 git common dir，不入版本控制）——**数行启动卡，四要素**：」；产物清单第 7 项，verbatim（同文件 `:18`）：「7. launch prompt 落 `.git/devloop/launch/prompt-<changeId>.txt`」。
- 断层实测：`ls "$(git rev-parse --git-common-dir)/devloop/launch/"` → `prompt-*.txt` **25 份** + `ruling-*.txt` **3 份** + `headless-260911-screen-unification.log` **1 份**（共 29 件），**最新为 `prompt-260917-m2-k3-char-window-overlay.txt`**；**无任何 `prompt-260920-*`**。
- 被跳过的自述，verbatim（`delivery/workstreams/m2-char-window/progress.md`「2026-09-19 · K2 merged → 本批 = 单包 K3」节）：「**launch 形态**：human 在本树会话内指示「启动 dev loop 按流程定义跑、无 block 不请示」→ 本树既有会话即 Pod 本体，按 `claude --agent planner` 主线程语义就地起 loop」。
- 同目录物种混放：`ruling-260911-screen-unification.txt` / `-02` / `-03`（**裁决件**）与 25 份启动卡、1 份 `headless-*.log` 平铺一处，**三种物种混放**，无归档规则、无命名区分。
- 术语两名一物：产物清单叫「launch prompt」（`:18`、`:122`），步骤标题叫「launch 启动卡」（`:105`）——同一物两个名字，见 §3 复扫 census。

**根因**：启动卡被设计成「launch 时刻的临时交付物」，但它的四要素里**三要素是耐久事实**——基线锚（progress.md 带 SHA 行）、上一包留根（`changes/dev/<上包>/pod-retro.md`）、本包特有钉点——这三者与包**同生命周期**，不是一次性的。放 git 外是**选址错误**，不是内容错误。而选址错误的代价在出现"更省事的路径"时立刻兑现：树池同树续跑让"人说一句话"比"写卡再贴卡"更省事，于是卡被自然绕过，且**没有任何机制提醒**。

**改法**：

1. **落点迁移**：启动卡改落 `delivery/taskpackages/<changeId>/launch-card.md`（**Git-tracked**），随 commit-before-launch 进 base ref。收益：Pod 建树即天然拿到（与 `plan.md` / `taskpackage.json` 同处，人读面收敛）；有历史、有 diff、可跨机；与拆包产物同一 commit 面，不会单独丢失。
2. **术语统一**：全仓 canonical 文本内一律称「**启动卡**」，废除「launch prompt」这一别名。**承重 landing 指令（逐处）**：
   - `kidpal-pod-launch-prep/SKILL.md:18`（产物清单第 7 项）：`7. launch prompt 落 \`.git/devloop/launch/prompt-<changeId>.txt\`` → `7. 启动卡落 \`delivery/taskpackages/<changeId>/launch-card.md\``
   - 同文件 `:107`：`写 \`.git/devloop/launch/prompt-<changeId>.txt\`（在 git common dir，不入版本控制）——**数行启动卡，四要素**：` → `写 \`delivery/taskpackages/<changeId>/launch-card.md\`（Git-tracked，随 commit-before-launch 进 base ref）——**数行启动卡，五要素**：`（第五要素 = 命令行行，见 T3）
   - 同文件 `:122`：`- 向 human 交付：每包一份 launch prompt（human 在对应 worktree 的 VS Code 会话自跑；headless 命令形态为备选）。` → `- 向 human 交付：每包一份启动卡（路径见上；human 在对应 worktree 的会话自跑，命令行见卡内第五要素）。`
   - `.claude/rules/repo-structure.md:26`：该行句中的「历史文档 / launch prompt 中的降级注记不作为依据」→「历史文档 / 启动卡中的降级注记不作为依据」（术语统一；该行其余内容不动）。
   - `.claude/skills/kidpal-pod-launch-prep/SKILL.md:126`（「坑」节）：「（260915 起 prompt 已收缩为启动卡，此坑移位到 skills / progress.md 锚行）」——**不改**（该句描述的是历史沿革，改了反而失真）；仅在 T2 落地后由本批 §6 登记为「已收缩为启动卡」的历史陈述。
3. **归属显式化**：`pod-coordination.md` §2 的协调态清单（board / pods / lock）**保持不动**——启动卡**不在**协调态清单内（它是耐久制品，不是运行时态）。在同节补一句显式归类，防后续再有人把它当协调态看待。**承重 landing 指令**：在 §2「**耐久证据**仍入版本控制…」段末补一句：
   > 启动卡（`delivery/taskpackages/<changeId>/launch-card.md`）按**耐久证据**归类（随拆包 commit 进基准 ref），**不在**上述协调态清单内——它是包的一部分，不是运行时状态。
4. **存量不迁**：`.git/devloop/launch/` 下 25 份启动卡 + 3 份裁决件**留在原处不动**（历史制品不回改）。在 skill 步骤 7 末尾留一句注记指其历史位置，防后来者按新路径找不到旧卡时误判"拆包产物缺失"。
5. **intake 复验（非门）**：`.claude/agents/planner.md` Step 1 的 intake 自探测清单（现为「包事实 / 环境事实 / 上游引用」三处）**补第四处 = 本包启动卡**（`delivery/taskpackages/<changeId>/launch-card.md`）；缺失则**如实记录并继续**（`pod-events` 留痕），**不设门**。本条的立法意图是治「静默绕过」——让"没卡"成为一条看得见的事实，而不是一条"没卡不许跑"的新规。

目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）· `.claude/references/pod-coordination.md`（改）· `.claude/rules/repo-structure.md`（改）· `.claude/agents/planner.md`（改）

### T3 · 启动卡没有 canonical 命令行，而承诺的机械落点不存在

**问题**：Pod 起步的命令行有**两套互不一致的文本**，且**都不完整**；而 canonical 文本承诺的落点（pod-board 的 Launch 节）**在被生成的 board 里根本不存在**。这不是纯文面瑕疵——`--settings` 的有无决定 `dev-loop.settings.json` 是否加载，而它相对默认 settings 独有三件：Stop `supervisor-gate-check.mjs`（防 closeout 后未跑合并门即收杆）、PostToolUse `portfolio-sync.mjs`（board 自动 reconcile）、`permissions.ask` 的 `git merge` / `git rebase`。**canonical 文本自己给的那一式（形态①）恰好就是降级的那一式。**

**证据**（全部 2026-09-21 现跑 verbatim）：

- **降级形（不带 `--settings`）共 4 处** —— 实测 `grep -rno -- "claude --agent planner" .claude/ docs/ --include=*.md` → **恰 4 处命中**：
  - `.claude/agents/planner.md:3`（**`description` frontmatter 内**，verbatim 片段）：「…归 portfolio-planner、不归你。务必主线程运行（claude --agent planner）——只有主线程能派生子 agent」
  - `.claude/agents/planner.md:12`（Role 段，verbatim 片段）：「你必须作为主线程运行（`claude --agent planner`），因为 Claude Code 的子 agent …」
  - `.claude/agents/portfolio-planner.md:50`（形态①括注内，verbatim 片段）：「harness 权限层在场生效：`claude --agent planner` 直跑，不加 `-p` / `--dangerously-skip-permissions` / 600s 时延环境变量」
  - `.claude/references/dev-loop.md:53`：「主线程 = planner（`claude --agent planner`）」
- **正确形（带 `--settings`）共 5 处** —— 实测 `grep -rn -- "claude --settings .claude/dev-loop.settings.json --agent planner" .claude/ docs/`：`.claude/dev-loop.settings.json:6`（settings 文件内自述串）、`.claude/references/dev-loop.md:19` / `:272` / `:285` / `:286`；另 `docs/ARCH-REVIEW-QUEUE.md:33`（ARQ 队列行，**属不回改面**，不计入）。
- （**v0.2 更正**：v0.1 作「三处文面」，**枚举不完整**——漏了 `planner.md:3` / `:12` 两处降级形，且未给正确形的全量计数。经 R1-F1 指出后现跑更正。）
- **承诺的落点不存在**：`.claude/agents/portfolio-planner.md:50` 末段（verbatim）「pod-board 的 Launch 节须并列两形态并标注适用条件（launch 执行者照 board 选）」；实测 board 键集：`node -e "console.log(Object.keys(require('<common>/devloop/board.json')))"` → `[ 'generatedAt', 'podCount', 'wip', 'mergeToken', 'pods', 'alerts' ]`；单 pod 文件键集 → `[ 'changeId', 'status', 'mergeOrder', 'worktreePath', 'updatedAt' ]`。**两处均无 Launch 节。**
- **skill 完全无命令行**：`kidpal-pod-launch-prep/SKILL.md` 步骤 7 的四要素模板（verbatim 括注）「**数行启动卡，四要素**」内**不含任何命令行**；步骤 8 交付句（verbatim，`:122`）：「human 在对应 worktree 的 VS Code 会话自跑；headless 命令形态为备选」。
- **全线降级的既有实测**（本批**转引**，出处 `docs/ARCH-REVIEW-QUEUE.md` 行 `ARQ-260918-pod-session-settings-identity-unobservable`，其自身时点 = 2026-09-18）：「在册三包 pod-events 分别是 15 / 4 / 15 条 `dispatch-completed`，**无一包以 `--agent planner` 载设置启动**」。本批**不重复取证**，仅引用其结论；该行的逐件证据见该 ARQ 行自身。
- **两式的真实差异**：`portfolio-planner.md:50` 形态②（verbatim）「② **无人值守 headless**（自治 loop、无人点权限提示）：`claude -p` + `--dangerously-skip-permissions`，且必须加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`」——差异**只在 headless 附加项**；`--settings` 与 `--agent planner` 是两式**共同项**。故统一可行、且不与 headless 配方冲突。

**根因**：canonical 命令行被写在 **prose 里**，而 prose 承诺的落点（board Launch 节）**从未被生成器实现**——于是"两形态"只活在两处互不引用的散句里，谁也没发现它们说的不是同一条命令。这与 T2 同根：**用散文载体承担了本该由机械面或实体制品承担的东西**。

**改法**：

1. **统一共同项**：两形态都必须带 `--settings .claude/dev-loop.settings.json` 与 `--agent planner`；差异仅在 headless 附加项（`-p` / `--dangerously-skip-permissions` / `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）。**降级形 4 处逐一改净**（**v0.2 扩面**：v0.1 只覆盖后两处，经 R1-F1 指出后补 `planner.md` 两处——**否则 V6 的"零命中"断言机械上不可达**）。**承重 landing 指令（逐处原文引用 + 替换后完整句）**：
   - `.claude/agents/planner.md:3`（`description` frontmatter 内）：`…归 portfolio-planner、不归你。务必主线程运行（claude --agent planner）——只有主线程能派生子 agent` → `…归 portfolio-planner、不归你。务必主线程运行——只有主线程能派生子 agent`。**说明**：此处在 frontmatter，**采取"删"而非"改"**——① `description` 是路由信号且有字数预算（`lint-descriptions.mjs` 守卫），塞入完整命令行会撑预算；② 完整命令行在 `:12` 的 Role 段承载，此处只留"务必主线程运行"这一路由语义即可。
   - `.claude/agents/planner.md:12`（Role 段）：`你必须作为主线程运行（\`claude --agent planner\`），因为 Claude Code 的子 agent` → `你必须作为主线程运行（\`claude --settings .claude/dev-loop.settings.json --agent planner\`），因为 Claude Code 的子 agent`。
   - `.claude/agents/portfolio-planner.md:50` 形态①括注内：`\`claude --agent planner\` 直跑` → `\`claude --settings .claude/dev-loop.settings.json --agent planner\` 直跑`。
   - `.claude/references/dev-loop.md:53`：`主线程 = planner（claude --agent planner）` → `主线程 = planner（claude --settings .claude/dev-loop.settings.json --agent planner）`。
   - `.claude/references/dev-loop.md:19` / `:272` / `:285` / `:286` 与 `.claude/dev-loop.settings.json:6` **已是正确形，不改**（不改即成为统一后的正确形态）。`docs/ARCH-REVIEW-QUEUE.md:33`（ARQ 行）属不回改面，**v0.2 显式登记为 V6 断言的排除面**。
2. **承重落点从"不存在的 board Launch 节"改到实体存在的启动卡**。**承重 landing 指令**：`.claude/agents/portfolio-planner.md:50` 末段现句（原文引用）「pod-board 的 Launch 节须并列两形态并标注适用条件（launch 执行者照 board 选）。」→ 替换为「两形态的完整命令行与适用条件由**启动卡**载明（`delivery/taskpackages/<changeId>/launch-card.md`，见 Dev Loop ② launch 面准备）；launch 执行者照卡选。」
3. **启动卡模板补第五要素 = 命令行行**。**承重 landing 指令**：`kidpal-pod-launch-prep/SKILL.md` 步骤 7 的四要素块改五要素，新增一条（紧随现存四要素之后）：
   ```
   命令行（两形态并列，标注适用条件）：
   ① 人在场交互式：claude --settings .claude/dev-loop.settings.json --agent planner
   ② 无人值守 headless：claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner（另加 CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0）
   ```
   并在该步旧四要素的首句「**数行启动卡，四要素**」改「**数行启动卡，五要素**」。

**待实测前置（诚实标注）**：VS Code 扩展会话能否传 `--settings` / `--agent`，**本批未实测**（`ARQ-260918` 已列此为该条的前置、亦未实测）。**本 T 的设计不依赖该实测结果**：无论扩展会话能否传参，`--settings` 都是 CLI 形态（人在场交互式 / headless）的**共同项**，第 1 条恒成立；该实测只决定"扩展会话形态"能否归入 canonical、抑或须显式登记为降级形态（见 §6 留根⑥）。

**⚠ 对 `ARQ-260918` 的承载度声明（R1-F5 指出，诚实标注）**：本 T **只承载该 ARQ 候选① 的一半**——「补 canonical 命令行 + 统一三/多处文面」这半本批落地；**「显式标注非 CLI 形态（VS Code 扩展会话）的降级面」这半 deferred 至 §6 留根⑥**（因其依赖上条未做的实测）。故：**该 ARQ 的核心问题（每包必现的静默降级）本批不闭合、该 ARQ 不结**，只在队列行留「候选① 部分承载」注记。该 ARQ 行自评 profile 为 `full-adversarial + human`，本批以 **Track B** 承载其一支臂——**这一落差已作为升级项交 human 确认**（见 §4 的「R1 升级项」段）；若 human 判「必须整条闭合」，则本 T 须同批补做扩展会话实测 + 降级面标注，量级与 profile 随之重估。

目标文件：`.claude/agents/planner.md`（改 · **v0.3 补锚**：T3 改法第 1 条现触及该文件的 `:3` description 与 `:12` Role 段，经 R2-F8 指出后补入——原 v0.3 稿的目标文件锚清单未同步，违 §2「每 T 显式列目标文件」硬项）· `.claude/agents/portfolio-planner.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

> **T4 已移出 §2**（不留 `### T4` 标题，以过结构门的「每 T 必备锚清单」判定）。原 T4 =「严格编译模式的开关靠卡上的一句话，而没有任何程序读它」，经 human 2026-09-21 裁 OQ-2 = (c)，整条转为 **§6 留根⑦** 的「登记不硬治项」——其问题 / 证据 / 根因 / 三条候选改法的完整记录**全部在 §6 留根⑦**，此处不重复（防两处漂移）。本条**零落地、无目标文件、不进 landing-manifest**。
>
> **§2 的 T 编号保持 T1 / T2 / T3 / T5 / T6（不复位为 T1–T5）**——保留这个编号缺口，是为了留住「该接缝被发现、并经 human 裁定不做」这一事实；后续该留根若被重新立项，可直接指回原编号，不必重述。

### T5 · input-brief 的生成时点，文本、闸、实况三方各说各话

**问题**：`input-brief-{acceptance-author,builder}.md` 是派发输入面的封闭清单（五路展开：契约/spec 锚点节 / 跨端对端面 / 先例封顶 3 / 超读申报模板 / 跨包承接面）。它的**生成时点有三个互不一致的答案**，且**没有任何一方被要求与另一方对账**：

| 来源 | 说的时点 |
|---|---|
| `.claude/references/dev-loop.md:135`、`.claude/agents/planner.md:55` | Pod 内 **planner Step 7**，「派 acceptance-author 之前」一次产双份 |
| `.claude/scripts/taskpackage-required-check.mjs:391` | 只在 `isFinalizedState` 查（= Pod 内） |
| **实况**（260920 两包） | **随拆包 commit 一起进 base ref** |
| `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 步骤 0–8 | **一字未提** |

**影响面**：同一文件在拆包侧与 Pod 侧**两次生成、内容可不同**（已有实例：`c84560d` 重生成），无单一事实源；而 human 在点火前想看一眼"这包要读什么"，文本上没有这一步。

**证据**（2026-09-21 现跑）：

- 文本，verbatim（`.claude/references/dev-loop.md:135`）：「**生成时点 = planner Step 7 派发前一次产双份**，Step 8 核在场、Step 9 只引用。」
- 闸，verbatim（`.claude/scripts/taskpackage-required-check.mjs:388-393`，摘）：`if (isFinalizedState) { const tpDir = dirname(pathToTaskPackage); for (const role of ['acceptance-author', 'builder']) { const briefPath = join(tpDir, \`input-brief-${role}.md\`); if (!existsSync(briefPath)) { missing.push(\`input-brief-${role}.md(not in taskpackage dir — planner Step 7 前置一次产双份: node .claude/scripts/brief-inputs.mjs --change-id ${taskPackage.changeId || '<id>'} --role ${role})\`); } } }`
- 实况：`git show --stat d8704bb` → `chore(devloop): 260920-m2-charwindow-card-fix 拆包制品落盘——triage ② Lite reviewed plan + taskpackage`，该 commit 内**同时落 4 件**：`input-brief-acceptance-author.md`(+50) / `input-brief-builder.md`(+50) / `plan.md`(+51) / `taskpackage.json`(+90) —— 即**拆包侧就生成了**。260920 两包目录下两份 brief 的 mtime 与 `plan.md` 同档（15:06 / 19:04）。
- 重生成痕迹：`c84560d chore(devloop): briefs 重生成（sourceType 移除后）+ intake/loop 纪律落账` —— 同一包、同一文件被生成过两次。
- skill 零提及：`grep -n "brief-inputs\|input-brief" .claude/skills/kidpal-pod-launch-prep/SKILL.md` → **零命中**。
- **本批未主张**：起草期**未**普查"拆包侧生成"是不是所有包的普遍形态（仅核实 260920 两包 + 260917 系列目录内均有 brief 且与 taskpackage 同 commit 面）。若判官需要全量，本 PROPOSAL 承认此处为**样本 + brief 存在性**级证据。

**根因**：brief 的**消费方**在 Pod 内（acceptance-author / builder），但它的**前置输入**（taskpackage 的 refs + `crossEnd`）在拆包时就已定型——于是两侧都能自称"生成时点"，而两侧都没被要求与对方对账。闸之所以能容忍，是因为它只在 finalize 态查"文件在不在"，**不查"谁生成的、什么时候生成的"**。

**改法**（推荐面 = OQ-1(**a**)，见 §4）：

- **(a) 归拆包侧**（**推荐**）：portfolio-planner Step 4 落盘四件套时一并跑双份 `brief-inputs.mjs`；brief 随 commit-before-launch 进 base ref。planner Step 7 改为「**核在场 + refs 有改动才重跑**」（沿用现成的 `--regenerated-because` 留痕机制，不新造面）。**理由三条**：① 闸的现文已在**同一目录**查（改闸更贵）；② commit-before-launch（F-03）已保证 Pod 天然拿到；③ human 在点火前就能看到输入面——这正是"launch 面"的一部分，与 human 的原始记忆一致（「pre launch 准备环境，写 launch prompt，还有 input ref」）。
- **(b) 归 Pod 侧**：拆包侧不生成，闸改为「finalize 后查」并在拆包侧显式声明不产。**代价**：260920 等已落包的 brief 成为"非法残留"须回改闸与文献；human 点火前无输入面可看。
- **(c) 两侧都产**（= 现状）：**否决**——同一文件两个产方、两次生成内容可不同（`c84560d` 已现），无单一事实源；本批要治的正是这个。

**三方文面随裁断对齐（取 (a) 时的承重 landing 指令，逐处）**：
- `.claude/references/dev-loop.md:135` 的「**生成时点 = planner Step 7 派发前一次产双份**，Step 8 核在场、Step 9 只引用。」→「**生成时点 = 拆包侧**（portfolio-planner Step 4 落盘时一次产双份，随 commit-before-launch 进基准 ref）；Pod 内 planner Step 7 **核在场**、refs 有改动才重跑（`--regenerated-because` 留痕）。」
- `.claude/agents/planner.md:55` 起首「7. **前置一次产双 brief（T4…）**：派 acceptance-author 之前跑两遍 `node .claude/scripts/brief-inputs.mjs --change-id <changeId> --role acceptance-author` 与 `--role builder`——**两份都在本步生成**」→ 改为「7. **双 brief 核在场（+ 按需重跑）**：两份 brief 由拆包侧生成、随基准 ref 到场；本步**核在场**，若 refs 在本步之后有改动才重跑两遍 `brief-inputs.mjs`…」。
- `.claude/agents/planner.md:58`（Step 9）「（不再生成；若 Step 8 之后 refs 有改动，重跑 `brief-inputs.mjs --role builder` 覆盖，重跑事由由脚本写进 brief 头部 `regenerated-because` 行）」——**不改**（与 (a) 相容，保留）。
- `.claude/agents/portfolio-planner.md` Step 4「落盘（四件套…）」节：四件套表述改为「四件套 + 双 input-brief」（或在同节补一句「同批跑 `brief-inputs.mjs` 双份」）；**不改**该节 Step 5b 门 1 / Step 6 commit-before-launch 的既有语义。
- `.claude/skills/kidpal-pod-launch-prep/SKILL.md`：步骤表的产物清单补一项「每包：双 `input-brief-*.md` 在场（拆包侧产、launch 侧核）」。

目标文件：`.claude/references/dev-loop.md`（改）· `.claude/agents/planner.md`（改）· `.claude/agents/portfolio-planner.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

### T6 · `launchPreconditions` 缺两个位：核过没、属哪个包（承载 `ARQ-260914`）

**问题**：workstream `decision-coverage.json` 顶层 `launchPreconditions` 是**纯字符串数组**——既没有 `decisionCoverage` 条目那样的「**属哪个包**」位（`landsIn: "package:<id>"`），也没有任何「**核过没**」位。而 `--check-launch <changeId>` 是 **per-package** 的：每个包点火都会看到 **workstream 级全量**条目，其中若干条根本是**别的包的事**；同时已核过的条目会被**永久重复报**——单做"报 N 条待核"只会产永久噪音。

**证据**（全部 2026-09-21 现跑）：

- **形态实测**：遍历 `delivery/workstreams/*/decision-coverage.json` —— `m1-home-qa` 与 `m2-char-window` **各 6 条、元素类型全为 `string`**（非对象）。`m1-home-qa` 顶层键 = `workstream,source,upstreamFrozen,generatedBy,generatedAt,updatedAt,_doc,decisionCoverage,crossPackageBehaviors,contractBlockedPackages,launchPreconditions`。
- **「属哪个包」位缺失的实况**（`m1-home-qa` 逐条前 90 字）：`[3] 页点数量 deviation 需 human 过目：**keybar-three-slot** 阶段一按现行 4 页循环渲染页点…`、`[4] **260915-m1-k4-reminder-overlay-unify** 额外前置：260915-m1-k1a-key-layer-dispatch 与 260915-m1-k3-v…` —— 第 3 条是关于 `260915-m1-k2-keybar-three-slot` 的事、第 4 条是关于 `260915-m1-k4-reminder-overlay-unify` 的事，**两条各自关于另一个包，却对每个包全量呈现**。
- **「核过没」位缺失，且核验结果落在别处（共三处文件）**：
  - `m1-home-qa/review.md` 门 1 节第 3 项（verbatim）：「**decision-coverage 完备：过。** …；launchPreconditions 六条齐（F-03 commit 前置、门 1 先行、deviation 拍板、RM 硬依赖、冻结面扫描、图索引）。」——记的是**清单齐备性**，不是逐条核验结论。
  - `m2-char-window/progress.md:70`（verbatim）：「**launchPreconditions 人读核（pod-coordination §2b 该门不含机械齿，260913 human 裁）**：六条逐条核——① commit-before-launch：…② …③ …④ …⑤ …⑥ …。全部满足，无未决项。」——**这才是逐条核验的耐久痕迹**，落在 progress.md。
  - `m1-home-qa/progress.md:11`（verbatim）：「**launch 前拍板（launchPreconditions 第 3 条）**：页点数量 deviation——…human 2026-09-16 AskUserQuestion 认可。」——只有第 3 条的人裁记录，**无六条逐条核段**（实测 `grep -n "launchPreconditions 人读核" delivery/workstreams/m1-home-qa/progress.md` → **零命中**）。
- **读侧不读**：`grep -c "launchPreconditions" .claude/scripts/portfolio-reconcile.mjs` → **0**（现 `--check-launch` 完全不读该键）。
- **同文件已有先例位**：`decisionCoverage` 条目带 `landsIn` —— `m1-home-qa` 的 15 条 `decisionCoverage` **全部带 `landsIn`**，取值分布现跑为：`package:<changeId>` 形 **7 条**、`exitCriteria:<ID>` 形 **6 条**、`deferred` **2 条**。（**v0.2 更正**：v0.1 作「带 `landsIn: "package:<id>"`（`m1-home-qa` 15 处）」——**把"带 landsIn 的总数"误当成"package 形的数"**；经 R1-F3 指出后现跑更正，命令与输出见本段。）——**结论不变**：`launchPreconditions` 缺的正是这组取值位，同文件已有先例形态可复用，**只是没跟上**。

**根因**：`launchPreconditions` 是后来加进 `decision-coverage.json` 的**第二张表**，加的时候只抄了"清单"这一半形状，没抄同文件 `decisionCoverage` 已经解决过的"归属"这一半；而"已核"位从来没有过——因为核验发生在 launch 时刻，落点选了 `review.md`（清单齐备性）与 `progress.md`（逐条核验），**都不在清单本身**。根因与 T2/T3/T4 同族：**该由制品自身承载的字段，落在了别的载体上**。

**改法**：

1. **数据模型**：条目由裸 `string` 改对象 `{ text, scope, status, evidence? }`：
   - `scope` ∈ `"workstream"` ∨ `"package:<changeId>"` —— **复用 `decisionCoverage.landsIn` 的取值形态**（同文件同构，不新造约定）；
   - `status` ∈ `"unverified"` ∨ `"verified"`；
   - `evidence?` = 凭据锚（`progress.md` 行 / human 裁决 ref），`status === "verified"` 时应在场。
2. **读侧**（`.claude/scripts/portfolio-reconcile.mjs --check-launch`）：**增读**该键。行为三件：① **按包过滤** —— 只取 `scope === "workstream"` ∨ `scope === "package:<本包>"`，其余折叠为一行计数（不逐条列）；② **只报** `status !== "verified"` 的条目；③ 输出维持既有 `{ok, blocking, warn, present}` 形状（`blocking` 语义**不变**——该键仍属 **warn** 通道，不是新的 blocking 源；本改法**不改门判定语义**）。
3. **写入义务**：`scope` 由**建包方**（portfolio-planner 落四件套时）填——条目文本本就点名承载包、机械可判；`status` 由 **launch 执行者**（主线程）核后填，凭据锚落 `evidence`。
4. **向后兼容（防静默、防噪音）**：裸 `string` 条目视为 `{scope:"workstream", status:"unverified"}`；读侧对**旧形态**条目**不逐条报**，只在输出末尾给一行汇总：「本 workstream 尚有 N 条旧形态（裸 string）前置未迁移，按 workstream 级呈现」。这样既不静默丢失、也不产"每包重复列 6 条"的长噪音。
5. **存量迁移不在本批 landing 面**：两个在册 workstream 的 12 条迁移**不进本批**。理由（实测支撑）：`m1-home-qa` **无逐条核验痕迹可引**（见证据段第三条），若本批强填 `status` 即为**自撰凭据**；`m2-char-window` 虽有 `progress.md:70` 六条逐条核段，但它是 **K3 一次 launch 的核验**，把这六条整体标 `verified` 会把"某包核过"读成"整清单已核"。登记 §6 留根①，由建包方在下次动该 workstream 时随批迁移（届时 `scope` 机械填、`status` 逐条引源行填）。

**取本条后的承重 landing 指令（逐处）**：
- `.claude/references/task-intake-and-slicing.md`「### launchPreconditions（canonical 定义）」节：条目形态句「条目 = 前置描述，须点名承载包与前置对象」→ 扩为对象形态定义（`text` / `scope` / `status` / `evidence` 四键 + 取值域 + 写入义务分派）。
- `.claude/references/pod-coordination.md` §2b 的 launchPreconditions 核验段：「`--check-launch` 的机械 check 集**不读** workstream `decision-coverage.json` 顶层 `launchPreconditions`；该面由 **launch 执行者（human 点火前）人工核**」→ 改为「`--check-launch` **读**该键，按包过滤、只报 `status !== 'verified'` 者（**warn 通道**，不阻断）；**人读核验义务不变**——核验结果由 launch 执行者回写 `status`/`evidence`，使下次不再重复报」。**注意**：该段末句「机械齿（脚本读该键）另登 ARQ 留根，人读反复漏检再机械化」中的「另登 ARQ 留根」**因其已由本条承载而作废**，须同批改写（否则与新文矛盾）。
- `.claude/scripts/portfolio-reconcile.mjs`：`checkLaunchReachability()` 增读该键 + 过滤/汇总逻辑；`--self-check` 补双夹具（新形态对象条目**须**被正确过滤、旧形态裸 string **须**走汇总行不逐条报）。

目标文件：`.claude/scripts/portfolio-reconcile.mjs`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/pod-coordination.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 入口与指令根 | `docs/DEV-GOVERNANCE.md` | **G1** | T1：§4 Dev Loop 图新增 `LP` 节点 + 两条边 + 全图编号顺延；§4 正文散文编号同步 |
| 角色与流程定义 | `.claude/agents/portfolio-planner.md` | **G2** | T1（Step 6 交接句）、T3（形态①命令行 + 落点句）、T5（Step 4 四件套表述） |
| 角色与流程定义 | `.claude/agents/planner.md` | **G2** | T2（Step 1 补第四处探测）、T5（Step 7 改为核在场+按需重跑）、**T3（`:3` description 删内联命令行 / `:12` Role 段命令行统一——v0.3 补，经 R2-F8 指出）**。**v0.2 已删 T4 条件锚**（OQ-2 裁 (c)，不再补环境变量落实步） |
| 角色与流程定义 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | **G2** | T1（开篇坐标）、T2（路径+术语+存量注记）、T3（第五要素命令行）、T5（产物清单补双 brief）。**v0.2 已删 T4 声明条改动**（OQ-2 裁 (c)，该条措辞原样保留） |
| 角色与流程定义 | `.claude/references/pod-coordination.md` | **G2** | T2（§2 启动卡归类句）、T6（§2b 核验段改写） |
| 角色与流程定义 | `.claude/references/dev-loop.md` | **G2** | T3（`:53` 命令行统一）、T5（`:135` 生成时点改拆包侧） |
| 角色与流程定义 | `.claude/references/task-intake-and-slicing.md` | **G2** | T6（launchPreconditions canonical 定义改对象形态） |
| 角色与流程定义 | `.claude/rules/repo-structure.md` | **G2** | T2（`:26` 术语统一：launch prompt → 启动卡） |
| 强制执行层 | `.claude/scripts/portfolio-reconcile.mjs` | **G3** | T6：`--check-launch` 增读 `launchPreconditions`（**advisory 检查器扩读，`blocking` 语义不变、非门判定语义变更**） |
| 强制执行层 | `.claude/scripts/firmware-build-check.mjs` | **未触**（**v0.2**） | 原 T4 条件锚；经 human OQ-2 裁 **(c) 不做、只登记** ⇒ 本条零落地、该锚自 landing-manifest 裁剪（见 §6 留根⑦） |
| spec 面 | `docs/architecture.md` | **spec**（随批携带） | T1：`:301` 指针行。human 2026-09-21 裁「不算本批 spec lane 触面，随批一起改」⇒ 走同一 packet、不另开 spec review |
| 契约面 | `docs/contracts/**` | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 六 T 无一新增工程门；T6 是 advisory 检查器**扩读**，非新增齿 |

**【硬项 · sweep 透镜义务】** 本批**不引入新门**。唯一的**新增判定 pattern** 是 T6 的「`--check-launch` 读 `launchPreconditions` 并按 `scope`/`status` 过滤」——该 pattern 的**全仓预跑须在实现落地后**发生（起草期脚本尚无此逻辑，无 pattern 可跑），故其预跑义务由 §5 验证集 V5 承担（含新旧两形态夹具）。**起草期已履行的 census（现跑留痕）**：① `launchPreconditions` 形态普查（2 个在册 workstream × 6 条，元素类型全 `string`，见 T6 证据段）；② 读侧不读实测（`grep -c` = 0）；③ 三处核验落点枚举（`review.md` / `progress.md:70` / `progress.md:11`，见 T6 证据段）。

**【硬项 · 复扫模式完备性先验】** 本批涉**术语统一**（`launch prompt` → 启动卡）与**路径迁移**（`.git/devloop/launch/prompt-<changeId>.txt` → `delivery/taskpackages/<changeId>/launch-card.md`），故枚举全形态（词形变体 × 有/无 backtick × 目标词）。**模式集与实测计数**（2026-09-21 现跑，范围 = `.claude/**` + `docs/**` 的 `*.md` / `*.mjs`；**历史制品面 `delivery/**` 与 `ARCH-REVIEW-QUEUE.md` 的 ARQ 行不回改、不计入**）：

| 模式 | 命中 | 归宿 |
|---|---|---|
| `launch-prompt`（连字符形） | **0** | — |
| `launch card`（英文字面） | **0** | — |
| `launch prompt` | **6**（其中 4 在 canonical 面：`kidpal-pod-launch-prep/SKILL.md:3`〔description〕/`:18` / `:122`、`.claude/rules/repo-structure.md:26`；2 在 `ARCH-REVIEW-QUEUE.md`〔ARQ 行，不回改〕） | T2 第 2 条逐处钉死 |
| `启动卡` | **5**（4 在同 skill：`:105` / `:107` / `:117` / `:126`；1 在 `ARCH-REVIEW-QUEUE.md` ARQ 行） | 已是目标术语，多数不改（`:107` 随路径改、`:126` 保留） |
| `prompt-<changeId>` | **2**（`kidpal-pod-launch-prep/SKILL.md:18` / `:107`） | T2 第 2 条逐处钉死 |
| `devloop/launch`（bare 路径） | **2**（`kidpal-pod-launch-prep/SKILL.md:18` / `:107`；第 3 处经 R1-F7 核为**幻影**——另 1 处在 `ARCH-REVIEW-QUEUE.md` ARQ 行、属不回改面） | T2 第 2/4 条 |

**v0.2 更正（R1-F7）**：`devloop/launch` 行 v0.1 记 **3**（含幻影 `:117`）——实测 `grep -rn -- "devloop/launch" .claude/ docs/ --include=*.md --include=*.mjs` 在 canonical 面**恰 2 处**（`:18` / `:107`），`:117` 是「写启动卡」一句、不含该路径串。其余 5 行经 R1 独立复跑**逐条对账一致**；「`delivery/**` 与 ARQ 行属不回改面故排除」这一边界经 R1 核**站得住**。
| `prompt-2609*` / `prompt-2608*` 等**具体实例名** | 见 T2 证据段（25 份现存文件 + `delivery/**` 内历史记录若干） | **不回改**（历史记录 append-only） |

**完备性论证**：本批改的是**规范面（canonical）对启动卡的称呼与落点**，不是历史记录。`launch prompt` 的 6 处命中已逐处归类，其中 **4 处为规范面**（T2 逐处给了原文引用与替换后完整句）、2 处在 ARQ 队列行内属"登记不改"；`launch-prompt` / `launch card` 两种变体**零命中**，说明不存在未枚举的词形。`delivery/**` 下的历史记录（各 workstream `progress.md`、`_archive/**`）按 append-only 纪律**显式排除**，不构成遗漏。

## 4. human 取舍点（OQ）

**OQ-1 · `input-brief` 的生成时点归哪侧（T5）** —— **human 2026-09-21 裁定 = (a) 归拆包侧。**

- **(a) 归拆包侧**（**human 裁定**）：portfolio-planner Step 4 落四件套时一并产双份；planner Step 7 改为「核在场 + refs 有改动才重跑」。**裁定理由**：闸的现文已在同一目录查（改闸更贵）、commit-before-launch 已保证 Pod 天然拿到、human 点火前能看到输入面（与 human 的原始记忆「pre launch …还有 input ref」一致）。
- **(b) 归 Pod 侧**（未采纳）：拆包侧不产、闸改为 finalize 后查。**代价**：260920 等已落包的 brief 成"非法残留"须回改闸与文献；human 点火前无输入面可看。
- **(c) 两侧都产**（现状，**已否决**）：同文件两产方、两次生成内容可不同（`c84560d` 已现），无单一事实源。

**OQ-2 · 固件严格编译模式的开关怎么落（T4）** —— **human 2026-09-21 裁定 = (c) 不做、只登记。**

- **(a) 保持默认宽松 + 另建桥**（未采纳）：由 Pod intake 机械落实环境变量。**扣分处**：多一处"靠 agent 记得做"的步骤、`export` 跨进程不传递、新增 planner.md 步骤。
- **(b) 默认翻转为严格**（未采纳）：未设即严格，降级须显式 `VERIFY_FW_BUILD_REQUIRED=0`。**扣分处**：**闸向严**——无工具链环境从"降级放行"变"红"，须先普查在册使用面。
- **(c) 不做**（**human 裁定**）：登记留根，等出现"降级被静默使用"的实例再动。**诚实记录代价**：问题原样留存——该开关仍由一份文本声明承载，仍无程序读它；本批不消除这一处「散文承载机械语义」。
- **裁定后本 T 的形态**：登记型 · **零落地**；`firmware-build-check.mjs` 自 landing-manifest 裁剪，`kidpal-pod-launch-prep/SKILL.md` 该条措辞原样保留。完整记录见 §6 留根⑦。

**其余四 T 无 OQ**：T1（给中段一个 owner，无真设计分叉）、T2（选址修正，无内容变更）、T3（文面统一到既有正确形态，两式共同项恒成立）、T6（数据模型与读侧，形态与取值域均有同文件同构先例 `decisionCoverage.landsIn`）——**均无"挂载面扩张 / 全会话新机制 / 不可逆性"三类须 human 裁的要素**。

**⚠ R1 升级项（`ARQ-260918` 承载度落差，交 human 确认——R1-F5）**

**事实**：`ARQ-260918` 的候选① 原文有两半——「启动卡模板补 canonical 命令行**并显式标注非 CLI 形态的降级面**（改 `kidpal-pod-launch-prep/SKILL.md` 步骤 7/8，与 `dev-loop.md:19` 对账统一三处文面）」。本批 **T3 只落前半**（canonical 命令行 + 全仓文面统一）；**后半（非 CLI 形态的降级面标注）deferred 至 §6 留根⑥**，因其依赖一条本批未做的实测（VS Code 扩展会话能否传 `--settings` / `--agent`）。而 `ARQ-260918` 行内的自评 profile 是 **`full-adversarial + human`（Track A）**。

**落差**：（i）该 ARQ 的**核心问题**——「每包必现的静默降级」——本批**不闭合**，只把它从「4 处降级形文本」收成「1 处已登记的降级形态（扩展会话）+ 全仓文面统一」；（ii）本批以 **Track B** 承载一个自评 Track A 的 ARQ 的一个臂。

**主线程已做的处置**：§2 T3 末已加**承载度声明**（「只承载候选① 的一半…该 ARQ 不结」）；§6 留根② 保留候选②③；`docs/ARCH-REVIEW-QUEUE.md` 该行将在落地时追加「候选① 部分承载」注记（**不结该 ARQ**）。

**须 human 裁的两问**：

- **(a) 接受本批的部分承载**（推荐）：T3 照现稿落地，`ARQ-260918` 不结、留「候选① 部分承载」注记，后半随 §6 留根⑥ 由下次动 launch 面的批承接（届时须先做扩展会话实测）。**理由**：本批已把可机判、无实测依赖的那半做净；后半的实测前置是一条独立的经验事实，硬塞进本批只会让整批卡在一个未知量上。
- **(b) 要求整条闭合**：则 T3 须同批补「VS Code 扩展会话传参实测 + 降级面标注」，量级扩容（新增实测项 + 可能的降级面文本），且按 `ARQ-260918` 自评须重估 review profile（**是否转 Track A**）——**此路须重启路由判定**，不在本批现稿范围内。

**✅ 裁定（human 2026-09-21）：(a) 接受本批的部分承载。** 处置落定：T3 照现稿落地（命令行 + 全仓文面统一）；**`ARQ-260918` 不结**，`docs/ARCH-REVIEW-QUEUE.md` 该行在 landing 时追加「候选① 部分承载（前半 = canonical 命令行 + 文面统一，已落 `260921-gov-review-launch-pipeline` T3；后半 = 非 CLI 形态降级面标注，deferred 至该批 §6 留根⑥）」注记；后半由其后的 launch 面批承接，**承接前须先做扩展会话传参实测**。本批 review profile **维持 Track B**（未因该 ARQ 的行内自评而升级——human 已在分类环节与本次两次显式知情后确认）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

每条机械可判：命令 + 预期。自检/重跑类遵守真空绿规约（重放先断言输入非空；自检验证输出含新增自检名 + 调用顺序审查；环境红先立基线）。

- **V1 · 结构门（landing 前）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260921-gov-review-launch-pipeline/PROPOSAL.md` → 预期 `{"ok":true,...}`。**起草期已跑，结果见 §7 起草期留痕**（若未跑则本行在 FROZEN 前必须补跑）。
- **V2 · `firmware-build-check` 自检 —— 本条不触发**（**v0.2**：human OQ-2 裁 (c) 不做、只登记，该脚本零改动、亦无"无工具链环境"普查义务）。原 v0.1 的 V2 全文作废；留根⑦承接其未做的普查项（触发条件 = 该留根被重新立项时）。
- **V3 · `portfolio-reconcile` 自检（landing 后）**：`node .claude/scripts/portfolio-reconcile.mjs --self-check` → 预期 `ok: true`，且输出含 T6 新增的**双夹具**名（新形态对象条目正确过滤 / 旧形态裸 string 走汇总行）。
- **V4 · T6 读侧实跑（landing 后，真空绿规约）**：对**一个新形态夹具包**与**一个旧形态在册包**各跑一次 `node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref <ref>`：① 夹具声明非空（先断言夹具文件存在且含 ≥2 条目的对象形态）；② 旧形态包输出**含**「N 条旧形态未迁移」汇总行且**不含**逐条 `[3]`/`[4]` 展开；③ 过滤断言：对非本包 `scope` 的条目**不出现在 warn 列表**。
- **V5 · 全仓 `--check-launch` 空转（landing 后）**：对 `delivery/taskpackages/*/` 全部包跑一遍 `--check-launch`（`--base-ref` 取各自 base），断言**零非零退出**（无包因本批改动由绿转红）——即证明 T6 的读侧扩展**不改门判定语义**。
- **V6 · T3 命令行统一断言（landing 后）**：`grep -rno -- "claude --agent planner" .claude/ docs/ --include=*.md` → 预期**零命中**（统一后该形必带 `--settings`）。**v0.2 口径更正（R1-F1/F4）**：（i）该断言现为**机械可达**——T3 改法第 1 条已扩及全部 4 处降级形（`planner.md:3` / `:12` / `portfolio-planner.md:50` / `dev-loop.md:53`），v0.1 漏后两处时此断言不成立；（ii）第二条 grep `grep -rn -- "claude --settings .claude/dev-loop.settings.json --agent planner" .claude/ docs/ --include=*.md` → 预期**≥ 8**（现值 **5**〔`dev-loop.md` 4 处 + `docs/ARCH-REVIEW-QUEUE.md:33` ARQ 行 1 处〕+ 本批新增 **3**〔`portfolio-planner.md:50`、`dev-loop.md:53`、`SKILL.md` 第五要素 ① 行〕；v0.1 括注「现值 4 + 新增 2」两处少计，已订正）；（iii）**排除面显式登记**：`docs/ARCH-REVIEW-QUEUE.md:33` 属 ARQ 队列行、**不回改**，上述两条 grep 若对 docs/ 全量跑须显式扣除该行（或在命令中 `| grep -v ARCH-REVIEW-QUEUE`），否则第一条断言会因该行的 `--settings` 引用形态误判——**该行不含 `claude --agent planner` 连续串，故第一条断言实际不受影响**，此注为口径钉死。
- **V7 · T2 术语/路径迁移断言（landing 后）**：`grep -rn "launch prompt" .claude/ docs/ --include=*.md --include=*.mjs | grep -v ARCH-REVIEW-QUEUE` → 预期**零命中**；`grep -rn "prompt-<changeId>" .claude/ docs/` → 预期**零命中**；`grep -rn "launch-card.md" .claude/` → 预期命中 ≥ 3（skill 产物清单 + 步骤 7 + pod-coordination §2）。
- **V8 · T1 图与编号一致性（landing 后）**：`node -e` 读 `docs/DEV-GOVERNANCE.md`，断言 §4 Dev Loop mermaid 块内含 `LP["② launch 面准备` 且**不含** `PP -->|launch| PL`；断言全图编号序列为 `①→②→③→④` 无重复（正则提取 `["\[]([①②③④])` 后去重计数 = 4）。
- **V9 · 路由回归（landing 后）**：`node .claude/scripts/route-regression-check.mjs` → 预期既有 ok 数不减、fail = 0（本批动了 `portfolio-planner.md` / `planner.md` 等 agent def 的近旁文面）。
- **V10 · 描述 lint（landing 后）**：`node .claude/scripts/lint-descriptions.mjs` → 预期**仅既有告警、无新增**（基线：`.claude/agents/landing-verifier.md` 超长告警 363>320，与本批无关）。**v0.2 更正（R1-F1）**：v0.1 曾写「**本批不改任何 agent/skill 的 frontmatter**」——**该句已随 T3 扩面而作废**：T3 改法第 1 条现**改变一处 frontmatter** = `.claude/agents/planner.md:3` 的 `description`（**删**去内联命令行，命令行走 `:12` 的 Role 段承载）。**本批 frontmatter 触面声明（精确）**：**动** `planner.md` 的 `description` 正文一句（字数**减少**）；**不动**任何 frontmatter 的 `tools` / `name` / `model` 三键（与 `ARQ-260918` 行机检硬排除逐项声明一致）；**不动**任何 skill 的 frontmatter（`.claude/skills/kidpal-pod-launch-prep/SKILL.md:3` 的 `description` 原样保留，其「launch prompt authoring」措辞**不在本批术语统一面内**——本批术语统一的锚清单见 T2 改法第 2 条，未含该行）。
- **V11 · 落地保真（landing 后，主线程派 landing-verifier 执行）**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260921-gov-review-launch-pipeline` → 预期 ok；逐文件人读 diff 核对 **landing-manifest 10 路径**与 §2 各 T 目标文件并集恒等（**v0.2 更正**：v0.1 作「11 路径（OQ-2=(c) 时 10 路径）」——OQ-2=(c) 已终裁、`firmware-build-check.mjs` 已自 manifest 裁剪，**现值恒 10 路径**，v0.1 的「11」为残留措辞，经 R1-F6 指出后订正）。

## 6. 留根清单

1. **存量 `launchPreconditions` 数据迁移（T6 移出）**——两个在册 workstream 的 12 条（`m1-home-qa` 6 + `m2-char-window` 6）由裸 `string` 迁至 `{text, scope, status, evidence}`。**不在本批 landing 面**，理由见 T6 改法第 5 条（`m1-home-qa` 无逐条核验痕迹可引，强填即自撰凭据）。**交接动作**：建包方在下次动该 workstream 时随批迁移——`scope` 按条目文本机械填、`status` 逐条引源行填（`m2-char-window` 有 `progress.md:70` 可引；`m1-home-qa` 逐条核段缺失，须由 human 或建包方补核后填）。**过渡期行为已定**：读侧对旧形态走汇总行、不产逐条噪音（T6 改法第 4 条）。
2. **`ARQ-260918` 的候选②③ 不在本批**——候选② = planner intake 期加「设置在场性自检」并落 pod-events（若升级为 verify 闸新判据即构成新增 gate）；候选③ = 在 `planner.md` 收尾相位把 `supervisor-gate-check.mjs` 手跑从散文纪律钉成显式步骤清单项。两条都在原 ARQ 行内、**本批只承载候选①**（T3）。触及时按该行自评的 profile 处置（候选②若机械化即 full）。
3. **workstream INDEX 漏报在册 workstream**——`delivery/workstreams/INDEX.md` 报「## Active (0)」，但 `m1-home-qa` 与 `m2-char-window` 两个目录**实际在顶层且仍在活动**（`m2-char-window` 刚 merged mo32、A6 真机腿未闭环，见其 `progress.md` 末节）。**根因初判**（未深查）：两目录**无 `task_plan.md`**（`delivery/workstreams/README.md` 把 front-matter 定为「状态单一源」），生成 INDEX 的 hook 读不到状态 ⇒ 落空。**边界外**（属 delivery-index 面，非 launch 链面），本批只登记、不治。
4. **skill 步骤 1「调度判定」与 `portfolio-planner.md` Step 5「Scheduling」的职责重叠未收口**——两侧都在定"哪几包进本批"，无一处说明关系。本批 T1 只给了中段 owner 与坐标，**未裁这一段的边界**（裁它需要先看清"ready 标记"与"批次判定"是不是两个独立动作，属真设计分叉，不在本批六处接缝内）。**交接动作**：留待下次动 launch 面的治理批，或以 `/gov-arq` 登记。
5. **`.git/devloop/launch/` 下 28 份存量文件不迁**（25 份启动卡 + 3 份 `ruling-*`；另有 1 份 `headless-*.log`）——历史制品不回改；T2 改法第 4 条已在 skill 步骤 7 留指路注记。**另一项观察**（不治）：该目录把裁决件与启动卡平铺一处，无归档规则；本批未处理。
6. **VS Code 扩展会话能否传 `--settings` / `--agent` 未实测**（T3 待实测前置）——该结果只决定"扩展会话形态"能否归入 canonical，抑或须显式登记为降级形态。**交接动作**：下次 launch 前实测一次；若不能传，则须在启动卡第五要素旁增一行「扩展会话形态 = 已登记的降级（缺 Stop 合并门钩子 / board 自动 reconcile / `permissions.ask`）」。
7. **【登记不硬治项】固件严格编译模式的开关无机械载体（原 §2 T4 整条移入）** —— **问题**：`firmware-build-check.mjs` 的严格模式由 `process.env.VERIFY_FW_BUILD_REQUIRED === '1'` 决定，**未设时该闸降级放行**（返 `degraded: true`，工具链缺失不算红）；而 canonical 文本把它写成启动卡上的一个声明行，措辞「此声明行随卡走**不得省**」。即**一个闸的松紧，由一份文本文件里的一句话保证，而那句话没有程序会读**。
   **证据（2026-09-21 现跑，verbatim）**：① 读侧 —— `.claude/scripts/firmware-build-check.mjs:108`：`  const required = process.env.VERIFY_FW_BUILD_REQUIRED === '1';`；② 降级放行分支 —— 同文件 `:70-77`（摘）：`  return { ok: true, degraded: true, buildAttempted: false, ..., strictModeEnv: 'VERIFY_FW_BUILD_REQUIRED unset', reason: '工具链两路探测均不可用：固件编译未在本机核验（降级放行，非假绿；degraded 字段供 verify.mjs 上收）' };`；③ 注入侧 —— `grep -rn "VERIFY_FW_BUILD_REQUIRED" .claude/ --include=*.json --include=*.mjs | grep -v firmware-build-check` → **零命中**；④ 声明侧 —— `kidpal-pod-launch-prep/SKILL.md` 步骤 7 四要素末条：「固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。」
   **根因**：与 T2/T3 同族——**用散文载体承担机械语义**。该声明被放进卡，是因为卡是"唯一会被人读到的地方"；但闸读的是**进程环境**，两者之间没有桥。措辞「不得省」越承重，越暴露这是用人的纪律去补机械面缺失的桥。
   **三条候选改法（human 2026-09-21 裁选 (c)，此处存档备用）**：**(a) 保持默认宽松 + 另建桥** —— 由 Pod intake 机械落实环境变量（读卡 → 本会话导出）。收益 = 不改任何既有环境的闸判定；代价 = 多一处"靠 agent 记得做"的步骤、`export` 跨进程不传递、新增一条 `planner.md` 步骤。**(b) 默认翻转为严格** —— 未设即严格、降级须显式 `VERIFY_FW_BUILD_REQUIRED=0`（默认值翻转 + 留痕方向反转）。收益 = 开关不再需要任何载体、启动卡那一整行可整句删除、三处「散文承载机械语义」一并消除；代价 = **闸向严**，须先普查在册使用面。**(c) 不动**（**human 裁定**）—— 触发条件 = 出现"降级被静默使用"的实例（判据：一次 firmware-build 闸输出 `degraded: true` 而无人记录）；触发后取 (a)/(b) 之一重新立项，**取 (b) 时须先补做「无工具链环境」普查**（哪些在册环境/闸/CI 依赖降级语义），该普查本批未做。
   **附带失真引用（随本条留根，不单独治）**：`.claude/scripts/firmware-build-check.mjs:15` 注释「// launch 面默认注入见 kidpal-pod-launch-prep SKILL.md「环境既成事实」段」指向一处**实际不存在的"默认注入"**（见证据③）。
   **human 裁定 (c) 的代价（诚实记录）**：问题原样留存——该开关仍由一份文本声明承载，仍无程序读它；本批**不消除**这一处「散文承载机械语义」。
8. **本批的「交日落复盘项」**：`/loop-retro` 观察两件——① 启动卡迁入 `delivery/taskpackages/<changeId>/` 后，**同树续跑场景**是否真的会被写上（本次要治的是"被静默绕过"；若续跑仍不写卡，说明病在"每包必写卡"这个要求本身，而非落点）；② T6 读侧落地后，下次 launch 的 `--check-launch` 输出是否真的按包过滤（若仍全量呈现，说明 scope 位没被建包方填）。
9. **自律件**：起草期全部 census 与形态实测均为 2026-09-21 现跑（命令与输出见各 T 证据段）；**T5 的「非全量普查」已如实标注为未做**（样本 + brief 存在性级证据，非作者判断替代）。**v0.2 变更留痕**：human 2026-09-21 裁 OQ-1=(a) / OQ-2=(c) 后，T4 转登记型零落地、其条件锚自 landing-manifest 裁剪、§5 V2 全文作废、原 v0.1 的「OQ-2 取 (b) 所需普查」义务移交 §6 留根⑦ 的触发条件；**OQ-2 裁 (c) 的代价（"散文承载机械语义"这一处未消除）已如实记入 §2 T4 与 §4**，未粉饰为"已解决"。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

**起草期留痕（主线程，非评审结论）**：本版为 **DRAFT v0.2**（v0.1 → v0.2 = 折入 human 立项 ack + OQ-1/OQ-2 两条裁定）。**V1 结构门已跑并绿**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260921-gov-review-launch-pipeline/PROPOSAL.md` → `{"ok": true, ..., "anchorCount": 10, "findings": []}`（2026-09-21 现跑；v0.1 首跑曾红一次，findings = `missing-anchor-list`〔T4 目标文件行内「或」使两路径间锚未决〕，已按 FROZEN/manifest 层裁决改为零落地后复跑转绿——**该次红为真问题、非工具误报**）。**§5 验证集的其余各条（V3–V11）尚未跑**，属 landing 后义务；V2 已随 OQ-2 裁定作废。

### R1 - 2026-09-21 - architect

verdict: BLOCK(F1,F2,F3,F5)

亲核基线：本批 §2/§3 证据段全部标 2026-09-21 现跑，我按证据基线清单逐件亲核（Read/Grep/Bash 实测），承重 landing 指令的替换目标文本与磁盘逐字比对通过者如下——T1 的 `docs/DEV-GOVERNANCE.md:128`（`    PP -->|launch| PL`）与同图两节点标签（`:125` ② / `:126` ③）、§4 散文「① RTE 拆包调度 → ② planner 带单 Pod」、`docs/architecture.md:301` 句尾「（操作协议见 kidpal-pod-launch-prep skill）」、`portfolio-planner.md:50` 形态①/形态②/Launch 节末段、`dev-loop.md:19/:53/:272/:285/:286/:135`、`planner.md:55/:58`、`taskpackage-required-check.mjs:388-393`、`SKILL.md:18/:107/:122/:126/:113`、`pod-coordination.md:51`、`repo-structure.md:26`、`firmware-build-check.mjs:15/:70-77/:108` 全部与磁盘逐字吻合（行号与引文实测一致）。board.json 键集 `['generatedAt','podCount','wip','mergeToken','pods','alerts']` 与单 pod 文件键集 `['changeId','status','mergeOrder','worktreePath','updatedAt']` 实测在（两处均无 Launch 节）；`grep -c "launchPreconditions" .claude/scripts/portfolio-reconcile.mjs` = 0 实测；`git show --stat d8704bb` / `c84560d` / `--follow input-brief-builder.md` 三处 git 制品与 §2 T5 引述一致；m1/m2 两 workstream decision-coverage.json 各 6 条 `launchPreconditions`、元素全 string，m1-home-qa 顶层键集与 §2 T6 引述一致；`m1-home-qa/review.md` 门 1 第 3 项、`m2-char-window/progress.md:70`、`m1-home-qa/progress.md:11` 三处 verbatim 亲核吻合。**V1 preflight 复核**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260921-gov-review-launch-pipeline/PROPOSAL.md` → `{"ok": true, "anchorCount": 10, "findings": [], "anchorUnion":[10 路径恒等 landing-manifest]}`，与起草期留痕一致，结论复核通过。

**Findings（逐条）**：

- **F1 · BLOCKING · V6「零命中」断言机械性不成立，T3 枚举漏 planner.md:3/:12 两处降级形态**。§5 V6 断言 `grep -rn -- "claude --agent planner" .claude/ docs/ --include=*.md` → 预期**零命中**（「统一后该形必带 --settings」）。实测现全仓（.claude + docs，*.md）该模式命中 **4 处**：`portfolio-planner.md:50`（形态①，T3 已改）、`dev-loop.md:53`（T3 已改）、`planner.md:3`（description frontmatter，verbatim「务必主线程运行（claude --agent planner）——只有主线程能派生子 agent。」）、`planner.md:12`（Role 段，verbatim「你必须作为主线程运行（`claude --agent planner`），因为 Claude Code 的子 agent 不能再派生子 agent。」）。T3 改法第 1 条只钉死 `portfolio-planner.md:50` 与 `dev-loop.md:53` 两处，`planner.md:3/:12` 未被任何 T 承接 ⇒ **landing 后 V6 仍命中 2 条，零命中无法成立**，落地核验必假红。同源：T3 证据段「三处文面」枚举不完整——实为「带 --settings 正确形」（`dev-loop.md:19/:272/:285/:286`，4 处）+「不带 --settings 降级形」（`portfolio-planner.md:50`、`dev-loop.md:53`、`planner.md:3`、`planner.md:12`，4 处）。**修法方向**（主线程裁）：① T3 扩及 `planner.md:12`（Role 段，非 frontmatter，与 V10「不改 frontmatter」不冲突）+ `planner.md:3`（description，属 frontmatter——若改则须同步撤销 V10「本批不改任何 agent/skill 的 frontmatter」句并重算 V10 预期、且按 authoring-agents-skills.md description 是路由信号须逐字段隔离）；或 ② V6 重定口径（排除 planner.md 身份/职责陈述形态，显式列出排除清单）+ T3 证据段补全「三处→五处文面」枚举。
- **F2 · SHOULD · 「24 份启动卡」计数错误（实为 25），且漏计第三种物种**。`ls "$(git rev-parse --git-common-dir)/devloop/launch/"` 实测（2026-09-21）：`prompt-*.txt` **25 份**、`ruling-*.txt` **3 份**、另 **1 份 `headless-260911-screen-unification.log`**（共 29 件）。PROPOSAL §0/T2 问题/T2 证据/§3 census/§6 留根⑤ 五处「24 份启动卡」「27 份存量文件（24+3）」均少计 1——应订正为「25 份启动卡」「28 份（25+3）」，且「同目录物种混放」的观察应补第三种物种 `headless-*.log`（「仅靠 ruling- 前缀」一句不完整）。非承重（改法为「历史不回改」，计数不影响落地），但证据精度须修。
- **F3 · SHOULD · T6 证据「`landsIn: "package:<id>"` 15 处」计数错误（实为 7 条）**。亲核 `m1-home-qa/decision-coverage.json` 共 **15 条** decisionCoverage，其中 `landsIn: "package:<id>"` 仅 **7 条**（`:20/:27/:40/:47/:68/:75/:88`），另 `exitCriteria` 6 条（`:34/:82/:95/:101/:107/:113`）、`deferred` 2 条（`:54/:61`）。「15 处」是把「15 条条目」误记成「15 处 package:<id>」——应订正为「15 条条目、其中 7 条 `landsIn: "package:<id>"`」。承重语义（decisionCoverage 已有 `package:<id>` 先例位）仍真，仅计数精度。
- **F4 · NIT · V6 第二 grep 的「现值 4 / 新增 2」两处少计**。实测现 `grep -rc "claude --settings .claude/dev-loop.settings.json --agent planner" .claude/ docs/ --include=*.md`：`dev-loop.md` 4 处 + `docs/ARCH-REVIEW-QUEUE.md` 1 处（ARQ-260918 行引该形）= 现值 **5**（非 4）；landing 后新增 3 处（`portfolio-planner.md:50`、`dev-loop.md:53`、`SKILL.md` 第五要素 ① 行——② 形 `claude -p ...` 因「claude --settings」不连续不命中该模式），共 **8**。「≥ 6」阈值仍安全，仅括注不精确，建议顺手订正。
- **F5 · SHOULD · 升级 human · T3 对 `ARQ-260918` 候选① 为部分承载，ARQ 核心问题本批未闭合**。`docs/ARCH-REVIEW-QUEUE.md` 行 `ARQ-260918-pod-session-settings-identity-unobservable` 候选① 原文含两半——「启动卡模板补 canonical 命令行」+「**显式标注非 CLI 形态的降级面**」，且该 ARQ 行自评 review profile =「**full-adversarial + human（Track A）**」（因「非 CLI 形态如何表达 canonical」是真设计分叉）。本批 T3 只落「CLI 命令行统一」半，把「非 CLI（VS Code 扩展会话）降级面标注」半 deferred 至 §6 留根⑥；故**本批落地后 ARQ-260918 的「每包必现静默降级」核心问题并未消除**（实际 launch 形态 = VS Code 会话仍不载 `--settings`，见 m2 progress.md:81 实证），「统一命令行」目标只对 CLI 形态成立。§4「其余四 T 无 OQ…T3（文面统一到既有正确形态，两式共同项恒成立）——均无…须 human 裁的要素」仅对 CLI 半成立、对整条候选①不成立。**建议**：① §2 T3 或 §4 显式声明「候选① 部分承载（CLI 半现落 / 非 CLI 降级面标注留根⑥），ARQ-260918 本批不结」；② 主线程向 human 确认该 deferral 可接受（ARQ 自身判其为「须 human 的设计分叉」，本批以 Track B 承载其一臂，存疑向严应请 human 点头留痕）。
- **F6 · NIT · V11「11 路径（OQ-2=(c) 时 10 路径）」为 v0.1 残留措辞**。OQ-2=(c) 已终裁、`firmware-build-check.mjs` 已自 manifest 裁剪、manifest 恒 10 路径——「11 路径」半句应删除，直接写「10 路径」。
- **F7 · NIT · §3 census「`devloop/launch`（bare 路径）= 3（:18/:107/:117）」含幻影行**。实测该模式在 `.claude/**` 仅 **2 处**（`SKILL.md:18`、`:107`）；`:117`（「launch 侧动作收敛为三件…写启动卡」）不含 `devloop/launch`。应订正为 2。census 其余各行（`launch prompt`=6、`启动卡`=5、`prompt-<changeId>`=2、`launch-prompt`=0、`launch card`=0）我独立复跑全部对账一致；「`delivery/**` 与 ARQ 行属不回改面故排除」的边界**站得住**（append-only 纪律 + 队列登记不改，且「命中」列已把 ARQ 行显式标为不回改、归类透明）。

**未升级 human 的项**：F1 是覆盖/枚举缺口（方向正确、改法方向性无错），F2/F3/F7 是证据计数精度，均属正常 R1 打回面，不触发升级线。仅 F5 因触及 ARQ 行自评 profile（full-adversarial+human）与本批 Track B 承载的落差，按「存疑向严」显式标注升级 human。

### R2 - 2026-09-21 - architect

verdict: BLOCK(F8)

**R2 聚焦复核**（只核前轮 findings 修订到位性 + 新矛盾，不全量重核）。亲核磁盘结论：**F1–F7 七条修订全部到位，F5 的 human 裁定记录忠实无粉饰**。本轮仅新出两条——F8（SHOULD，F1 修订引入的锚清单/§3 sweep 未同步）与 F9（NIT，一处「只」字引文误差）。F8 修掉即可 GREEN（下轮同判官 resume）。

**逐条修订到位性（亲核留痕）**：

- **F1 → 已修**。T3 证据段现为「降级形 4 处全量枚举 + 正确形 5 处计数 + v0.2 更正自述」，改法第 1 条扩及全部 4 处（`planner.md:3` 删 / `:12` 改 / `portfolio-planner.md:50` 改 / `dev-loop.md:53` 改）。实测 `grep -rno -- "claude --agent planner" .claude/ docs/` 恰 **4 处**命中（planner.md:3/:12、portfolio-planner.md:50、dev-loop.md:53），四改法逐一覆盖后 V6 零命中**机械可达**。`planner.md:3` 取「删」处置**成立**：description 字数**减少**（不触 `lint-descriptions` 超长阈值）、路由信号「务必主线程运行…只有主线程能派生子 agent」保全（"claude --agent planner" 是"如何调"非"何时用/派谁"，完整命令行走 `:12` Role 段承载）、不动 `tools`/`name`/`model` 三键；**已核 `route-regression-check.mjs` 的 `CFG.agentRequiredSignals`（`.claude/route-fixtures/routing.json`）不含 planner / portfolio-planner**（仅 6 个 review-loop agent），故删该行**不影响 V9 路由回归**。V10 已同步作废 v0.1「不改任何 frontmatter」句并给精确触面声明。
- **F2 → 已修**。§0/§2 问题/§2 证据/§3 census/§6 留根⑤ 五处改 25 份启动卡 / 28 份存量（25+3），补第三种物种 `headless-*.log`（T2 证据段「共 29 件」）。我 `ls` 复核 prompt 25 / ruling 3 / headless 1 = 29，一致。
- **F3 → 已修**。T6 证据段改「15 条全部带 landsIn：package 7 / exitCriteria 6 / deferred 2」+ v0.2 更正自述。我复核 m1 decision-coverage.json 分布 = 7/6/2，一致。
- **F4 → 已修**。V6 第二 grep 改「≥ 8（现值 5 + 新增 3）」。我复核现 md 内 settings 形 = dev-loop.md 4 + ARQ 行 1 = 5，一致。
- **F5 → 已裁、记录忠实无粉饰**。§4「R1 升级项」段如实记：事实（候选①两半）/ 落差（「每包必现静默降级本批不闭合」原文保留 + Track B 承 Track A 自评）/ 两问 (a)(b) / **human 2026-09-21 裁 (a) 接受部分承载**、ARQ 不结、Track B 维持、landing 时队列行补「候选① 部分承载」注记。未把「不闭合」改写为「已解决」，与 §0 非目标第 5 条、§2 T3 承载度声明、§6 留根② 四处一致。
- **F6 → 已修**。V11 删「11 路径」残留，直写「landing-manifest 10 路径」。
- **F7 → 已修**。§3 census `devloop/launch` 改 2（:18/:107）+ 幻影更正说明 + ARQ 行「另 1 处」注记。我复核 canonical 面恰 2 处、ARQ 行另有 1 处，一致。

**新 findings**：

- **F8 · SHOULD · T3 扩面至 `planner.md:3/:12` 后，T3 目标文件锚清单与 §3 sweep 的 `planner.md` 行未同步**。F1 修订把 T3 改法第 1 条扩及 `planner.md:3`（删）与 `planner.md:12`（改），但 ① T3 目标文件锚清单（§2，现为「portfolio-planner.md · dev-loop.md · SKILL.md」三件）**未含 `.claude/agents/planner.md`**；② §3 sweep 表 `planner.md` 行（现「T2（Step 1 补第四处探测）、T5（Step 7 改核在场）」）**未列 T3**。planner.md 虽经 T2/T5 已入 landing-manifest（manifest 恒等 10 路径、preflight 仍 `anchorCount:10 / findings:[]` 绿，我独立复跑确认），但违 §2「每 T 末尾显式列目标文件 / 缺锚清单 = 门 RED 点名缺锚」纪律——T3 承重 landing 指令引 `planner.md` 而 T3 锚清单不含，属「承重指令引未锚文件」。**修法**：T3 锚清单补 `.claude/agents/planner.md`（改）；§3 sweep planner.md 行补「T3（:3 description 删 / :12 命令行统一）」。
- **F9 · NIT · T3 证据段/改法第 1 条的 `planner.md:3` 原文引用多一「只」字**。「…只归 portfolio-planner、不归你。务必主线程运行（claude --agent planner）——…」中「只归」应为「归」——实测 `grep -c "只归 portfolio-planner"` = **0**、`grep -c "归 portfolio-planner、不归你"` = **1**。承重片段「务必主线程运行（claude --agent planner）——只有主线程能派生子 agent」本身逐字准确、定位不受影响，但违「逐字吻合」纪律，建议订正。

**结构字段**：landing-manifest 围栏块（10 路径）+ packet-id 行在场；preflight 独立复跑 = `ok:true, anchorCount:10, findings:[]`。

### R3 - 2026-09-21 - architect

verdict: GREEN

**R3 聚焦复核**（只核 R2 findings 修订到位性 + 新矛盾 + 主线程对 §7 的误伤处置）。亲核磁盘结论如下。

**F8 → 已修**。T3 目标文件锚清单（§2）已补 `.claude/agents/planner.md`（带 `:3` / `:12` 触面说明 + 「经 R2-F8 指出后补入」留痕），现 T3 锚清单 = `planner.md` + `portfolio-planner.md` + `dev-loop.md` + `SKILL.md`（4 件）；§3 sweep 的 `planner.md` 行已补「T3（`:3` description 删内联命令行 / `:12` Role 段命令行统一——v0.3 补，经 R2-F8 指出）」。T3 锚清单与改法第 1 条实际触面（4 文件）**恒等**；§2 各 T 目标文件并集仍 = 10 路径 = landing-manifest（preflight 独立复跑 `ok:true, anchorCount:10, findings:[]`，anchorUnion ≡ manifest，无幻影路径 token）。

**F9 → 已修**。T3 证据段（§2）与改法第 1 条两处的 `planner.md:3` 原文引用均已订正为「…**归** portfolio-planner、不归你」（无「只」）。`grep -n "只归 portfolio-planner"` 现仅 **2 处**命中、均为合法——line 5（Status 块误伤披露，描述还原动作）+ line 403（R2-F9 finding 引文，本应保留「只」以自洽陈述）；§2/§3 canonical 面**零残留**。

**§7 R1/R2 逐字一致复核**：我通读 §7 全文（R1 verdict `BLOCK(F1,F2,F3,F5)` + F1–F7 七条 + R2 verdict `BLOCK(F8)` + 逐条到位性 + F8/F9 两条），与我的原始产出**逐字一致**。重点核 R2-F9 行（line 403）——其引文片段「…只归 portfolio-planner、不归你」已恢复为原本自洽形态（该 finding 正是讲「只」字之误，引文必须保留「只」），还原正确；无其它行被替换穿透。

**「误伤 + 还原」裁断：可接受**。① 主线程自行发现并如实披露（Status 块 line 5「⚠ 主线程对 §7 的一次误伤与还原」），不掩；② R2-F9 行已逐字还原，我复核与原产出一致；③ 除该一行引文外 §7 R1/R2 全文零改动（我通读核对）；④ 立下的纪律（后续修订一律定点 Edit、不用全局替换）是针对该真实风险（判官记录区内含被订正文本的引文时，机械全局替换会穿透 append-only）的恰当补救。判官记录区完整性最终保全，无残留损伤。

**结构字段**：landing-manifest 围栏块（10 路径）+ packet-id 行在场；preflight 独立复跑 = `ok:true, anchorCount:10, findings:[]`。

**结论**：**GREEN**。R1/R2 全部 finding（F1–F9）已闭环，v0.4 无新矛盾。主线程可凭「立项 ack + human 三条裁定（OQ-1=(a) / OQ-2=(c) / R1-F5=(a)）+ GREEN」标 **FROZEN**，随后走 packet-bound landing（landing-manifest 绑定 + landing-verifier 独立核验）。
