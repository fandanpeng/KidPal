# PROPOSAL · 261015-gov-review-face-loop-unify

> **状态：FROZEN v1.0 · 2026-10-15 · 凭据 = human 立项 ack（2026-10-15 主线程会话，OQ-1~11 全裁见 §4）+ architect R1 BLOCK(F1..F7) → R2 BLOCK(F1..F3) → R3 GREEN（三轮内收口未触熔断，评审记录见 §7；无升级 human 项）+ landing-fidelity-check --preflight 干净（findings/advisories 均空）**。主线程凭据自冻，packet-bound landing 授权记此行；内容冻结（landing 期不可回改 PROPOSAL）。
> **承载 ARQ**：`ARQ-260928-arch-change-retirement`（本批以其两候选之外的第三路径——repurposing——结项）；并承接 `260928-gov-review-requirement-loop` PROPOSAL §6 留根第 3 条「架构档挂入同一 loop 骨架」（触发条件「需求面 loop 实跑 2–3 单」已满足：v1.60/v1.61/v1.62/v1.63 共 4 单）；顺带兑现 `pod-coordination.md §10e` 留根的契约合并机械放行口（`ARQ-260923-contract-merge-deadpath` 建议①——起草期核实：该条目已于 2026-09-24 结项归档，建议①留根由本批 T15 兑现，无需回写）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。向严标注：本批删一个人工门（组 D）、改写一处合并闸判定逻辑（T15），现行 `architect-reviewer.md` 机检硬排除第 6 条「改 gate → ESCALATE_FULL_REVIEW」字面命中；主张按 Track B 执行的证成见 §3 末节，若判官评审认为须升 Track A，主线程接受升级（存疑向严）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T24）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（按模板枚举上限取值；触点约 40 文件但其中约 30 件为文面改指/引用普查，机械层判定逻辑的变化**唯一**在 T15 合并闸契约规则一处，且变化方向 = 从「恒 BLOCK」改为「凭据放行」、配 self-check 用例钉死；两处高敏子面——T15 与组 D——在 §3 单列证成，请判官按高敏件标准审）。
> **review profile**：G1 + G2 + G3（G1 = `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` 三件；G2 = agents ×3 改 / commands ×4（两删两改/增） / references ×8 / rules ×2 / skills ×2 / templates ×2；G3 = `scope-map.json` / `review-loop.settings.json` / `pre-merge-escalation-gate.mjs` / `routing.json`；契约面 `docs/contracts/**` 内容 = 零——本批只改契约面的**治理流程**，不碰契约内容一字）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/architect-reviewer.md
.claude/agents/architect.md
.claude/agents/review-moderator.md
.claude/agents/scenario-author.md
.claude/agents/planner.md
.claude/agents/integrator.md
.claude/agents/portfolio-planner.md
.claude/agents/uiux-author.md
.claude/scope-map.json
.claude/review-loop.settings.json
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/route-fixtures/routing.json
.claude/commands/arch-change.md
.claude/commands/review-arch.md
.claude/commands/gov-change.md
.claude/commands/review-governance.md
.claude/commands/triage-change.md
.claude/commands/build-feature.md
.claude/commands/loop-retro.md
.claude/references/arch-change.md
.claude/references/gov-change.md
.claude/references/architect-operating-model.md
.claude/references/arch-review-protocol.md
.claude/references/contract-change-evidence.md
.claude/references/pod-coordination.md
.claude/references/change-intake.md
.claude/references/change-triage.md
.claude/references/task-intake-and-slicing.md
.claude/references/dev-loop.md
.claude/references/dev-loop-operating-system.md
.claude/rules/contracts.md
.claude/rules/repo-structure.md
.claude/rules/authoring-agents-skills.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
AGENTS.md
CLAUDE.md
docs/DEV-GOVERNANCE.md
docs/architecture.md
docs/CONTRACT-DECISION-QUEUE.md
docs/templates/WORKSTREAM-PROPOSAL.template.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
docs/glossary.md
docs/debt/gov/ARQ-260928-arch-change-retirement.md
docs/debt/gov/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把四个面统一成同一副 loop 元结构——**每面 = 起草/审稿对偶 + 纯编排主线程**，命令面六件（`/triage-change` 总编排 / `/requirement-change` / `/arch-change` / `/gov-change` / `/build-feature` / `/loop-retro`）每面恰好一个入口；架构+契约面从此有真正的 review loop（审恒定、改一个字也审），契约面去掉人签字门、改为可机验的落地凭据，治理变更获得入口与轻路径——triage 总编排可以按 spine 把不同面的 loop 当节点拉起（graph engineering）。

**现状差距**：

1. **架构面无 loop**：架构变更只有几个散命令，git 实证（`69124f1` 语言润色、`e52d534` arc42 重构、`b041ddf` 全文对照修订、`0a303d9` 批改）全走「human 指示 + 跳过 Track B + 如实记录」直改——审不审全凭当场记忆。
2. **契约门人签是低价值人工触点**：机械上 `pre-merge-escalation-gate.mjs` 的 `hasContractGateEvidence` 写死 `false`（契约面 diff 恒 BLOCK、人合唯一通道），人批时手里只有 diff 和直觉；而 agent 侧（architect-reviewer 四分类 + 反证检查 + 最小变更原则）结构更仔细。human 2026-10-15 裁定：「人看反而不如 Architect Agent 看得仔细」。
3. **治理面无入口**：Track B 没有命令入口，且「主线程起草 PROPOSAL」是交互时代的遗留——graph 时代主线程是总编排，起草治理变更需要的是 architect 的职责（流程定义、治理设计）；主线程还**既起草又落地**，作者与落地者不分。
4. **契约「四分类」三套枚举打架**：`rules/contracts.md`（散文四分类含「随 spec 携带」）、`architect-reviewer.md`（四值枚举无「随 spec 携带」、多 `IMPLEMENTATION_ADAPTER`）、CDQ 实际在用第五值 `ACCOMPANIES_SPEC_CHANGE`——一个成形流程不应有三种互相打架的分类词表。
5. **`/arch-change` 名不副实**：字面是「架构变更」，装的却是「Track A 统一入口」（spec/governance 分类器）；其 spec lane 的服务对象将被面 loop 接管（`ARQ-260928-arch-change-retirement` 的双入口并存问题到期）。
6. **`launchPreconditions` 的「支撑语义 → `docs/architecture.md` 已冻结」无判据可依**（`task-intake-and-slicing.md` / `pod-coordination.md` 引用了冻结语义，但 architecture.md 无版本区、无状态行、无变更记录节）。

**非目标（本批不做）**：

- 不动需求面 loop（`/requirement-change` 已跑 4 单，形态差异——档 0/1 无 reviewer 审、无 Track B 后端——作统一性评估留根 §6，不在本批收口）；
- 不动 dev loop 本体（builder/verifier 对偶已是元结构形态）；
- 不动对抗机器本体——`arch-review-protocol.md` 的 charter/辩论/裁定/pass-oracle 机制原样保留，作为两个面 loop 共享的 Track A 段引擎 SSOT；
- 不动 `docs/contracts/**` 内容一字（本批只改契约面的治理流程）；
- 不改 `change-intake.md` 的 spine/headless 机制本体（graph 机制已存在，本批只做接缝改指）；
- 不合并 requirement-reviewer 与 architect-reviewer（四查骨架同源，合并评估留根）。

## 2. 问题、根因与改法（T1…T24）

**【硬项 · 起草期实证义务】**已执行部分：本稿所有 file:line 引据为 2026-10-14/15 主线程会话实读核对（含 `scope-map.json` 全量 agent 条目、`review-loop.settings.json` 全文、`pre-merge-escalation-gate.mjs:105-135/255-280/355-375`、`docs/CONTRACT-DECISION-QUEUE.md` 全文、`pod-coordination.md:190-210`、`routing.json` 全文、`AGENTS.md` 治理路由节、`architect-reviewer.md` 全文、`contract-change-evidence.md` 全文、`requirement-change.md` 全文、`change-intake.md` 全文、两个 260928 批次 PROPOSAL、git log docs/architecture.md、`/review-arch`+`/review-governance` 引用普查实测 `grep -rn` 19 文件、`/arch-change` 引用普查实测 34 文件（判官 R1 复核口径；初查 21 漏 showme / glossary / dev-loop-OS 等，R1-F2））。**遗留亲核项**（FROZEN 前完成，已列入 §5）：`changes-index.mjs` 与两个 hook 中 review 命令引用是功能还是文面（定点亲读）；`route-regression-check.mjs` 基线实跑；`ARQ-260923-contract-merge-deadpath` 条目状态核实。

**【硬项 · 目标文件锚清单】**随每 T 第五段列出；条件锚（亲读后可能不改的）在注记中写明条件，不引路径 token。

### T1 · architect-reviewer 扩编（零新增 agent 的支点）

**问题**：统一元结构需要「架构+治理面内容审稿」角色，但新建 `architecture-reviewer` 会与既有 `architect-reviewer` 一字之差、两个 reviewer 并存是长期误用源；同时既有 `architect-reviewer` 的职责②（`/arch-change` lightweight 快道）随 T7 改造自然消亡，其职责①（契约分类过滤器）在去人签门后反而更重要。
**证据**：`.claude/agents/architect-reviewer.md:3`（description 含两职责）；`:63-88`（lightweight 节）；scope-map 实测 `architect-reviewer {"allow": [], "deny": ["**"]}`；`routing.json` 实测其信号 `["ContractChangePacket", "契约", "预审"]`。
**根因**：该 agent 的字面名「审 architect 的稿」本就贴合统一后的主职责；扩编比新建更省且消解命名冲突。
**改法**：重写 `.claude/agents/architect-reviewer.md`——主职责 = 架构面与治理面的内容审稿（四查骨架，与 requirement-reviewer 同构：① 意图保真〔human 原话 → 文本表述〕② 纪律合规〔架构面 = arc42 骨架/锚稳定规则/三份分工句；治理面 = 路由表/写权边界/门语义不被文本漂移改动〕③ 撞车复核〔与既有节/既有 ADR/既有决策历史档冲突〕④ 牵连面完整〔反向锚牵连、联动文件、契约指针〕）；保留契约分类过滤器（dev-loop/triage 调用，枚举改引 T14 的 canonical 五值）；删除 lightweight `/arch-change` review-depth 节（整机退役）；description 重写（路由信号随之重钉，见 T22）；tools 不变（Read, Grep, Glob, Bash，写权经 T2 窄槽授予）；硬边界保留「不批契约、不自证、作者≠审者」。判档权：轻/主路径与 Track A 段升级由本 agent 判（判据清单见 T8/T12），**只升不降**。
目标文件：`.claude/agents/architect-reviewer.md`（改）

### T2 · architect-reviewer 授评审记录窄写槽

**问题**：T1 后 architect-reviewer 承担 Track B 第 2 步评审，评审记录须落 PROPOSAL §7；其 scope-map 现为 `allow: [] / deny: ["**"]` 纯只读，若主线程转写裁决有漂移风险。
**证据**：scope-map 实测（上引）；先例 = landing-verifier 条目 `allow: ["delivery/changes/arch/*arch-review-*/**", "delivery/changes/arch/*gov-review-*/**"]`（同样只写评审制品、碰不到 canonical）。
**根因**：写评审记录是审稿职责的一部分，窄槽先例成熟。
**改法**：scope-map `architect-reviewer` 条目改为 `allow: ["delivery/changes/arch/*arch-review-*/**", "delivery/changes/arch/*gov-review-*/**"]`、`deny: ["**"]`（与 landing-verifier 同形）；`_doc` 注一句授槽事由。
目标文件：`.claude/scope-map.json`（改）

### T3 · AGENTS.md 治理路由节重写（G1）

**问题**：治理路由节是现行 Track B 的定义地，其 cast（主线程起草、architect 评审）、升级判据①（触契约面直走契约门）、「不动」节的契约门条目，均与本批新设计冲突。
**证据**：`AGENTS.md:10-30`（治理路由节全文，2026-10-14 实读）；`:24` 升级判据两条；`:26` 需求面变更行；`:29-30`「landing 通道」与「不动」节。
**根因**：G1 指令根必须与新元结构同步，否则 agent 读到的路由表是旧的。
**改法**（承重替换，替换后完整形态如下）：Track B 七步 cast 更新——第 1 步「主线程起草 PROPOSAL」→「**主线程派 architect 起草 PROPOSAL**（主线程四面纯编排：派发/落地/状态翻转/记账，不起草内容）」；第 2 步「architect 评审循环」→「**architect-reviewer 评审循环**（只读 canonical、只写评审记录于 PROPOSAL §7）」；第 3–7 步骨架不动。升级判据——删第①条（契约门消解，契约变更 = 架构变更的一种，走 `/arch-change`）；第②条（跨系统结构性变更）保留、落点改为「所属面 loop 的 Track A 段」；「声明红队」与「熔断」保留。面入口行——「需求面变更」行保留，其下新增两行：「**架构+契约面变更**：走 `/arch-change` loop（流程 SSOT `.claude/references/arch-change.md`；architect 起草 + architect-reviewer 审稿 + 轻路径/Track B 两档落地 + 方向不清走 Track A 段）」「**治理面变更**：走 `/gov-change` loop（流程 SSOT `.claude/references/gov-change.md`；同 cast；轻路径 = 文档面，机器面恒 Track B）」。「不动」节——「契约门（architect-reviewer 四分类 + human 签字）」改为「**契约面防线 = universal.bashDenyWrite（Bash 重定向写恒拦，全会话）+ dev-loop.settings.json 契约 deny（实现面恒禁写，保留）+ 实现面/审稿面各 agent 的 scope-map 契约 deny（不变）+ 契约合并机械凭据闸（`pre-merge-escalation-gate.mjs`）**」（R1-F1：原稿「settings/scope-map deny 不动」与 T16 冲突且失实——architect 与 review-moderator 的 scope-map allow 本就含契约路径，属起草/编排角色写权（落地恒归主线程 packet-bound），维持现状）；landing-verifier 独立核验与机械强制层两条不动。
目标文件：`AGENTS.md`（改）

### T4 · CLAUDE.md 契约行改写（G1）

**问题**：现行行「改契约走人工门（review-loop settings 已 deny 冻结）」与新设计冲突。
**证据**：`CLAUDE.md` Claude Code 专属节第 2 条（2026-10-14 实读）。
**根因**：同 T3。
**改法**（承重替换，替换后完整句）：「在 `docs/contracts/**` 下改动前先进 plan mode——契约是单一事实源，改动走 `/arch-change` loop（防线保留：Bash 重定向写恒拦、实现面 agent 与 dev-loop 会话的契约禁写不动；合并过机械凭据闸）。」
目标文件：`CLAUDE.md`（改）

### T5 · architect.md 起草职责归位（自改递归保护对象，须 human 授权）

**问题**：统一 cast 后 architect 是架构面+治理面变更的起草者，其 agent 定义须写明；该文件是自改递归保护对象（`rules/authoring-agents-skills.md` D-C 硬排除、`rules/prompt-authoring.md` 硬排除），scope-map 对 architect 自身 deny 此文件。
**证据**：`.claude/agents/architect.md:28-29,65,77`（起草分两类/起草 subagent 表述，2026-10-14 普查实读）。
**根因**：职责归位须落在定义上，否则路由信号与实况漂移。
**改法**：description 与 When to Use 增补「架构面/治理面变更的起草者（`/arch-change`、`/gov-change` 内被主线程派发；含 Track B PROPOSAL 起草）」；正文「起草分两类」节更新（讨论稿仍归主线程记录、正式起草归本 agent）；自改递归边界句原样保留。**授权**：本文件改动经 human 立项 ack 一并授权（OQ-10 单列）。
目标文件：`.claude/agents/architect.md`（改）

### T6 · architect-operating-model.md 同步

**问题**：12 处 `/arch-change` 引用 + 「主线程起草 PROPOSAL」/「convention-only 主线程写权」等表述与新 cast 冲突。
**证据**：2026-10-14 普查实测 12 处命中（含 `:75` canonical 起草归 architect、`:79` convention-only 分层兜底）。
**根因**：同 T3（文面同步）。
**改法**：`/arch-change` 引用按新语义改指（loop 入口/Track A 段）；「主线程起草 PROPOSAL」类表述改为「主线程派 architect 起草」；convention-only 段保留但更新（主线程落地写权不变、起草职责移除后该段收敛为「主线程不起草内容」的纪律声明）。
目标文件：`.claude/references/architect-operating-model.md`（改）

### T7 · /arch-change 覆盖复用（repurposing 为架构+契约面 loop 入口）

**问题**：`/arch-change` 现装「Track A 统一入口」（spec/governance 分类器），名不副实；架构+契约面无 loop 入口。
**证据**：`.claude/commands/arch-change.md` 全文（2026-10-14 实读：phase-0 分类 + review-depth + 契约预审步）；`ARQ-260928-arch-change-retirement`（双入口并存问题）。
**根因**：「arch-change」字面即「架构变更」，覆盖复用比退役+新铸省事——60 处既有引用中凡「spec/架构变更走 /arch-change」的语义自然成立，且 ARQ-260928 以第三路径（repurposing）结项。
**改法**：全文重写——新语义 = 架构+契约面 review loop 入口。流程（逐步流水，仿 `requirement-change.md` 形态）：① 立项（human 裁意图 + **收敛度判断**；spine 发起则开场消费 faceMap/facePlan）→ ② 方向不清 → **Track A 段**（主线程派 review-moderator 跑对抗机器：architect 起草候选 → scenario-author 冻 charter → challenger↔defender → adjudicator 裁定 → human 拍方向；收敛后流回③，内容不再重复四查——对抗机器已审得更深）→ ③ architect 起草（PROPOSAL 改法集）→ ④ architect-reviewer 四查（**恒定，无例外**）+ 判档（轻/主路径，只升不降）→ ⑤ 轻路径 = **瘦制品五件过闸**（R1-F3 / R2-F1：升级闸 gov 机锚要 FROZEN + GREEN + verdict，无锚 = 轻批永不过闸、每次合并升级人，违背例外反选——故轻路径**不减核验、只减文书**：瘦 PROPOSAL〔Status 行 + 改法清单表 + 判档依据节，替代五段式改法集〕+ §7 评审 GREEN + landing-manifest〔瘦批数行〕+ 瘦 architect-landing-summary〔packet-id + manifest 复述 + claimed 清单，数行——landing-fidelity-check 结构门硬要求件，R2-F1〕+ landing-verdict PASS；主线程落 + architecture.md 变更记录节 append → 升级闸 kind=gov 锚全在场、**闸零改动** → 合并；**retro 豁免**，瘦 PROPOSAL 尾节一行记由、复盘并入 cadence loop-retro）；主路径 = Track B 后端全件（FROZEN → packet-bound landing → landing-verifier → retro → 升级闸 → 合并）→ ⑥ 触契约分支（任何路径：契约包三节并入 PROPOSAL 改法集，随批原子落，无人签门）。原 phase-0 分类职责移交 triage/intake（T20 改指）；governance 流量改指 `/gov-change`。
目标文件：`.claude/commands/arch-change.md`（改）

### T8 · 新建架构+契约面流程 SSOT

**问题**：面 loop 需要流程 SSOT（谁起草、谁审、怎么定稿、判档判据），对偶 `requirement-change.md`。
**证据**：`.claude/references/requirement-change.md`（对偶形态，2026-10-14 实读全文）。
**根因**：命令是入口，SSOT 是定义；两者分置是需求面验证过的形态。
**改法**：新建 `.claude/references/arch-change.md`，节 = 立项与收敛度判断 / Track A 段判据与流程 / 起草与审稿（四查）/ **轻路径判据清单** / 触契约分支 / 定稿载体 / 消费保护规则 / 角色分工表。三份关键判据清单（本批的承重语义，human 立项 ack 时逐字看）：

**轻路径判据（架构面）——全部满足才可走轻**：① 零语义变化（不改任何机制/行为/拓扑/边界/阶段/阈值的含义）；② 改动性质限于：错字、排版、表述澄清、语言润色（可挂 `design-doc-polish` skill 执行）、锚行格式、指针刷新（版本号/日期/链接）、旧→新节号映射维护；③ 不触 `docs/contracts/**`；④ 不触 §1.2 外部依赖清单、§2.2 拓扑图、§2.3 端云职责划分、§5.x 视图结构、§9 演进路线。**命中任一否定 → 主路径；评审中发现语义成分 → 只升不降。**

**Track A 段判据（命中任一 → 走 Track A 段）**：① human 立项时判断方向未收敛——该不该变、变更的收益与影响还看不清楚，需要先把方向讨论定下来；② architect-reviewer 评审中发现方向级缺口——方案存在未裁的根本分叉、收益与影响无法从方案判定（cover 不全），升级（只升不降）；③ 声明红队（human 或 architect 任一方）。**判据只看收敛度，不看触及面**：触及 §2.2 端云四层链路拓扑 / §2.3 端云职责划分 / 阶段划分 / §1.2 外部依赖增删等承重节，若方向清晰（该变、收益与成本明确），走主路径（Track B 后端 + 四查审遗漏）即可——这些节由轻路径判据④保证必落主路径，不因此进 Track A 段。

**轻路径落地通道（R1-F3 / R2-F1）**：轻路径批同样建 change 目录（token 同 `gov-review` / `arch-review` 命名槽），含**瘦制品五件**——瘦 PROPOSAL（Status 行 + 改法清单表 + 判档依据节）、§7 评审 GREEN、landing-manifest（数行）、瘦 architect-landing-summary（packet-id + manifest 复述 + claimed 清单，数行——landing-fidelity-check.mjs 结构门硬要求件）、landing-verdict PASS；升级闸 kind=gov 机锚（FROZEN + GREEN + verdict）全数在场、**闸零改动**；retro 豁免（瘦 PROPOSAL 尾节一行记由，复盘并入 cadence loop-retro）。**轻的是文书重量，不是核验重量**——独立核验与过闸纪律对轻批一分不减。

**定稿载体**：`docs/architecture.md` 文末「变更记录」节（见 T10），**changeId 即定稿标记**（不引入文档版本号）；不设状态行——PROPOSAL 模式下架构文件评审期不被触碰，updating 态不会出现；**消费保护**：`launchPreconditions` 的「支撑语义 → architecture.md 已冻结」判据 = 变更记录节对应 changeId 行在场（文句约定，机械齿留根 §6）。

**触契约分支**：架构变更改变端云数据格式 / shim API / 课表 MD 结构 / display 响应块 → PROPOSAL 改法集必须含**契约包三节**（proposedContractDiff / impact / validationPlan，沿用 `architect-reviewer.md` 输出格式节的字段名），契约 diff 与架构改动随批原子落地；纯契约变更（无架构配套）同样以本 loop 为入口（消化「独立契约变更无入口」的实况缺口）。
目标文件：`.claude/references/arch-change.md`（新增）

### T9 · 删除 /review-arch（并入 Track A 段）

**问题**：Track A 段住进 `/arch-change` 后，`/review-arch` 成为同一台机器的第二个门。
**证据**：`.claude/commands/review-arch.md:12-13`（自述「不确定走 /arch-change」「回 /arch-change」）；普查 19 文件引用（§3）。
**根因**：一面一入口；对抗机器本体（protocol + scenario-author/challenger/defender/adjudicator）不动，仅入口归并。
**改法**：删除 `.claude/commands/review-arch.md`；引用改指（T20）；`routing.json` 对应行删除（T22）。
目标文件：`.claude/commands/review-arch.md`（删）

### T10 · docs/architecture.md 新增「变更记录」节

**问题**：架构面无定稿载体；`launchPreconditions` 的「已冻结」判据悬空（差距 6）。
**证据**：`docs/architecture.md:1-8`（头部无版本区/状态行，2026-10-14 实读）；`pod-coordination.md:41`、`task-intake-and-slicing.md:294`（消费方）。
**根因**：需求面有 §9，架构面缺对偶物。
**改法**：文末追加新节（排在现有 §11 术语表之后，编号 §12——追加不重排既有节号，不破锚稳定规则）：`## 12. 变更记录`，纪律行（每次改动 append 一行、新行在上、changeId 即定稿标记、走 `/arch-change` loop、跳过流程须在依据列写明）+ 空表首（`| changeId | 日期 | 改动面 | 依据 |`）。**spec-anchor-check 影响**：新节会被 `parseArchHeadings` 计入 `archSectionCount`（`:46-50` 正则 `^#{2,4}\s+(\d+...)`——「12. 变更记录」命中），无既有锚指向 §12，零悬空；§5 验证集含实跑。
目标文件：`docs/architecture.md`（改）

### T11 · 新建 /gov-change 命令

**问题**：治理面无入口（Track B 无命令）；graph 拉起需要可命名单元。
**证据**：`arch-review-protocol.md:15`（「Track B 常驻轻路径没有命令入口」）；human 2026-10-15 裁定（「到时候应该也有一个入口才比较好」）。
**根因**：一面一入口的元结构。
**改法**：新建 `.claude/commands/gov-change.md`（仿 `requirement-change.md` 逐步流水 + `argument-hint`；argument 可为 human 意图原话或 ARQ-id 清单——治理项多自带 ARQ 预消化材料，命令开场先读条目）：① 立项（human 裁意图/确认 ARQ 候选修法）→ ② architect 起草 PROPOSAL → ③ architect-reviewer 四查 + 判档（只升不降）→ ④ 轻路径 = 瘦制品五件过闸（形态同 T8「轻路径落地通道」节：瘦 PROPOSAL + §7 GREEN + landing-manifest + 瘦 landing-summary + landing-verdict；升级闸零改动；retro 豁免）；主路径 = Track B 后端全件 → ⑤ Track A 段（罕见：判据见 T12）。
目标文件：`.claude/commands/gov-change.md`（新增）

### T12 · 新建治理面流程 SSOT

**问题**：同 T8（治理面版）。
**证据**：human 2026-10-15 裁定（治理项「大部分发现了问题、方向已限定」；轻/重分界 = 文档 vs 机器）。
**根因**：治理项天生收敛（问题即方向边界 + ARQ 预消化 + 开口方向在成为 item 前已被讨论收敛），故 Track A 罕见；轻重分界可机判。
**改法**：新建 `.claude/references/gov-change.md`，节 = 立项（ARQ 消费）/ 起草与审稿 / **轻路径判据（路径即判据）** / Track A 段判据 / Track B 后端 / 角色分工表。**轻路径判据——diff 全部同时满足**：① 仅落在 `.claude/references/**`、`.claude/skills/**`、`docs/notes/**`、`docs/templates/**`；② 性质为措辞/格式/链接/勘误（零行为语义变化）；③ 不触 `.claude/commands/**`（行为入口）、`.claude/agents/**`、`.claude/hooks/**`、`.claude/scripts/**`、`scope-map.json`、`*.settings.json`、`verify-gates.json`、`AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md`——**命中任一 → 恒 Track B，哪怕只改一个字符**。**Track A 段判据**：① human 立项时判断方向开口（该不该改、怎么改还未收敛）；② architect-reviewer 评审中发现方向级缺口（方案存在未裁的根本分叉，cover 不全），升级（只升不降）；③ 声明红队。**与架构面同一原则：判据只看收敛度，不看触及面**——动承重机器（hooks / scripts / scope-map / 闸判定逻辑）但方向清晰的变更走 Track B（四查审遗漏 + landing-verifier + 升级闸兜底），不因此进 Track A 段；要额外攻击某一高风险项走「声明红队」。

**轻路径落地通道**：形态同 `arch-change.md`「轻路径落地通道」节（瘦制品五件过闸、升级闸零改动、retro 豁免），本文件不重复定义、只引用。
目标文件：`.claude/references/gov-change.md`（新增）

### T13 · 删除 /review-governance（并入 /gov-change Track A 段）

**问题**：同 T9（治理面版）。
**证据**：`.claude/commands/review-governance.md:13-14`（自述回指 /arch-change）。
**根因**：一面一入口。
**改法**：删除该命令；引用改指（T20）；`routing.json` 对应行删除（T22）。
目标文件：`.claude/commands/review-governance.md`（删）

### T14 · rules/contracts.md 重写（五值枚举 canonical + 去人签门）

**问题**：契约「四分类」三套枚举打架（差距 4）；「改契约 = 四分类 + 人类契约门」的纪律根文本随去门失效。
**证据**：`.claude/rules/contracts.md` 全文（散文四分类）；`architect-reviewer.md:47-50`（四值枚举）；CDQ 实测 `ACCOMPANIES_SPEC_CHANGE` 在用（如 `CDQ-260902-shim-api-field-envelope` 行）。
**根因**：契约变更 = 架构变更的一种（human 2026-10-15 裁），其纪律应表述为「写冻结保留 + 走 /arch-change + 合并机械凭据闸」。
**改法**：重写 `.claude/rules/contracts.md`——**canonical 五值枚举**（其余文件一律引用本表，不再各自定义）：`NO_CONTRACT_CHANGE`（现有契约足够，实现走偏或未读对）/ `IMPLEMENTATION_ADAPTER`（契约合理，实现侧需 adapter/mapper/本地格式转换）/ `ACCOMPANIES_SPEC_CHANGE`（随 spec/架构变更携带，同批原子落地）/ `SPEC_CONTRACT_DRIFT`（已批准 spec 与契约冲突，须裁改哪边）/ `CONTRACT_CHANGE_REQUIRED`（现有契约无法表达已批准设计，独立契约变更）。纪律正文（R1-F1 精确化）：「`docs/contracts/**` 写防线保留——Bash 重定向写恒拦（universal.bashDenyWrite）、实现面各 agent 与 dev-loop 会话契约禁写不变；architect（起草）与 review-moderator（Track A 段编排）的 scope-map 契约写权维持现状——落地恒归主线程 packet-bound（R2-F3 措辞中性化）；契约变更走 `/arch-change` loop（契约包三节并入 PROPOSAL）；合并由 `pre-merge-escalation-gate.mjs` 机械凭据闸放行（T15）；影子目录禁令保留」。
目标文件：`.claude/rules/contracts.md`（改）

### T15 · 合并闸契约规则改写（本批唯一机械判定逻辑变化，高敏）

**问题**：`pre-merge-escalation-gate.mjs` 对契约面 diff 恒 BLOCK（`hasContractGateEvidence` 写死 `false`），人合是唯一通道；去人签门后须换成可机验凭据，否则契约面要么永远 BLOCK、要么开洞。
**证据**：`.claude/scripts/pre-merge-escalation-gate.mjs:105-113`（`hasContractsDiff` 实现）、`:263-265`（通用 1 BLOCK 分支原文，2026-10-15 实读）、`:366`（`hasContractGateEvidence: false // 契约门证据（contract-change-packet）后续接入；现 false → 契约面 diff 恒 BLOCK（向严）`）。
**根因**：向严的占位实现等的就是本批——凭据形态（loop 落地四件）现在有了定义。
**改法**：把 `hasContractGateEvidence: false` 替换为实查函数 `hasArchLoopContractEvidence`，行为规格——① `diff = git diff --name-only <source>..<target>`；② `contractPaths` = diff 中 `docs/contracts/` 前缀行（空 → 本规则不触发）；③ 候选 = diff 中匹配 `delivery/changes/arch/` 下带 `arch-review` token 目录的 `PROPOSAL.md`；④ 对每个候选：读其头部状态行须含 `FROZEN`，提取 landing-manifest 围栏块与 FROZEN / verdict 判定**一律复用 `.claude/scripts/lib/governance-gate-shared.mjs` 单源**（manifest 提取 + `isFrozen` + `VERDICT_STATUS_RE`，lib :629 在场——R1-F5：不另写解析、模块引用带 lib/ 路径，防 260918 / 260920 verdict-anchor 病灶复发），读同目录 `landing-verdict.md` 须在场且 verdict 为 PASS；⑤ **放行判据**：`contractPaths` 非空 且 存在候选满足 ④ 且 `contractPaths ⊆ 其 manifest 路径集`；⑥ 不满足 → BLOCK，报错点名未覆盖的契约路径与已检查的候选清单。通用 1 的报错文案同步改（删「人合 runbook」指针，改指 `rules/contracts.md`）。通用 4（`contractFindingsAccounted`，CDQ/pod-events 核销）不动。**self-check 用例**（真空绿规约，六例——R1-F4 补 FROZEN 肢否定用例，三件联立肢各有否定覆盖）：`contracts-diff-no-evidence-block` / `contracts-diff-evidence-pass` / `contracts-diff-proposal-not-frozen-block` / `contracts-diff-manifest-not-covering-block` / `contracts-diff-verdict-not-pass-block` / `no-contracts-diff-unaffected`；既有 `contracts-block` 用例同步改写。
目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T16 · review-loop.settings.json 删契约 deny（headless loop 落地前提）

**问题**：`/arch-change` headless 会话以 review-loop.settings.json 加载（身份三件套在册复用），其 `permissions.deny: ["Edit(./docs/contracts/**)"]` 会让该会话的 landing 写不了契约——去门后契约落地无路。
**证据**：`.claude/review-loop.settings.json:6-7,12-16`（2026-10-15 实读：deny 只此一条，`_doc` 注明形态教训）。
**根因**：该 deny 是「契约只读冻结」在 review 会话的 enforcement；新世界里契约的防线改为 = universal.bashDenyWrite（Bash 重定向全禁）+ 实现面/审稿面 agent 的 scope-map deny（builder / fixer / acceptance-author / story-author 等，不变）+ dev-loop.settings.json deny（保留）+ 主线程 packet-bound 纪律 + T15 合并凭据闸。**写权归属如实陈述（R1-F1）**：architect 与 review-moderator 的 scope-map allow 本就含 `docs/contracts/**`（起草/编排角色；落地恒归主线程 packet-bound），删本条后会话主线程获得与 `requirements.md` / `architecture.md` 同级的落地写权（packet-bound 约束），正对齐「契约 = 架构变更的一种」。
**改法**：删除 `permissions.deny` 整条（含 `_doc` 对应行改写，保留「Write(path) 无效形态勿加回」教训句）；**`dev-loop.settings.json` 的契约 deny 原样保留**（实现面不许写契约，触契约转 `/arch-change`）。
目标文件：`.claude/review-loop.settings.json`（改）

### T17 · contract-change-evidence.md 收窄

**问题**：packet + ledger 双独立制品（`CONTRACT-CHANGE-PACKET.md` + `architecture-gate-evidence.md`）是人签门的证据形态；去门后契约变更的证据由 loop 标准制品承载（PROPOSAL 契约包三节 + 评审记录 + landing-verdict），双制品冗余。
**证据**：`.claude/references/contract-change-evidence.md:7-19`（Required Artifacts 表）、`:36-49`（状态词表 HUMAN_APPROVED 等）。
**根因**：证据义务不消失但应并入 loop 制品，避免双写两面。
**改法**：收窄重写——保留「契约变更的证据义务」与 Evidence Integrity Rules（grep-able 铁律），改为指向：契约包三节在 PROPOSAL 内的写法 + CDQ 登记义务；状态词表修订（`HUMAN_APPROVED`/`CONDITIONAL-APPROVE` 退役，新行用 `LANDED`/`CLOSED`）；明确 `CONTRACT-CHANGE-PACKET.md` 与 `architecture-gate-evidence.md` 不再作为独立制品要求（历史包不动）。
目标文件：`.claude/references/contract-change-evidence.md`（改）

### T18 · CDQ 列义调整

**问题**：CDQ「human 门」列与头部状态词随去门失效。
**证据**：`docs/CONTRACT-DECISION-QUEUE.md:4-6`（状态词行 + 列表头，2026-10-15 实读全文）。
**根因**：CDQ 作为跨时间契约决策账本仍有价值（通用 4 闸消费它），只调列义。
**改法**：头部状态词行改（去 `HUMAN_APPROVED`/`CONDITIONAL-APPROVE`）；列「human 门」→「**loop 凭据**」（值 = changeId + landing-verdict 结果）；历史行一字不动（append-only 纪律）。
目标文件：`docs/CONTRACT-DECISION-QUEUE.md`（改）

### T19 · pod-coordination §10e 人合 runbook 退役

**问题**：人合四步 runbook 的存在前提（闸恒 BLOCK）被 T15 消除；其留根句明写「机械放行口为留根项（`ARQ-260923-contract-merge-deadpath` 建议①）」——本批 T15 即该留根的兑现。
**证据**：`.claude/references/pod-coordination.md:196-205`（§10e 全文，2026-10-15 实读）。
**根因**：runbook 是缺机械口时的手动等价；机械口落地，手动等价退役。
**改法**：§10e 整节替换为短指针节——「契约包合并 = 普通 loop 落地 + T15 机械凭据闸；历史人合实例（K7 等）见本节 git 历史」；`ARQ-260923-contract-merge-deadpath` 已核实结项（`_archive/2026/`，2026-09-24 结；建议①留根由 T15 兑现）——不回写。
目标文件：`.claude/references/pod-coordination.md`（改）

### T20 · 引用普查改指（`/review-arch` / `/review-governance` / 旧 `/arch-change` 语义）

**问题**：两命令删除 + `/arch-change` 语义改写后，全仓引用须逐处分类处置；其中脚本/hook 内的引用可能是**功能性**（目录 token 解析、模型路由），删错即破机器。
**证据**：普查实测两轮——主线程 2026-10-15 初查 + 判官 R1 复核（`grep -rn "review-arch\|review-governance"` = 19 文件，两轮一致；`grep -rn "arch-change"` = **34 文件**〔R1-F2：初查 21 漏 docs/showme 9 件、glossary、dev-loop-operating-system 等〕）；复扫模式完备性：词形 = `/review-arch`、`/review-governance`、`review-arch`、`review-governance`（backtick 有无两态 × 命令/目录 token 两语境）+ `/arch-change` 旧语义语境词（「统一入口」「Track A」「分类」）；**排除清单（记录类封存，不改）**：`docs/showme/**`（一次性视觉解释稿归档，9 件）、`delivery/changes/**`（历史批制品）、`delivery/changes/intake/**` spine、`delivery/workstreams/**` progress、`docs/debt/**`（含 _archive）、git 历史。
**根因**：文面引用改指、功能引用保留并随新语义校准、历史制品封存。
**改法**：逐件处置（§3 表逐行列类）：commands 三件（triage-change / build-feature / loop-retro 的逃生门与改指——触契约改「转 `/arch-change`」，不再「停在人类契约门」）；agents 六件（planner / integrator / portfolio-planner / uiux-author / review-moderator / scenario-author 的引用更新；review-moderator 保留为 Track A 段编排者、description 更新）；references 六件（change-intake §8 接缝——契约行路由 `/arch-change`、治理命中路由 `/gov-change`；change-triage / task-intake-and-slicing / arch-review-protocol / dev-loop / dev-loop-operating-system 改指——protocol 的「统一入口 /arch-change」节改写为「Track A 段由两面 loop 消费」）；**docs 两件**（glossary 术语/入口行随新名册更新；GOVERNANCE-MIGRATION-GUIDE:70 活模板命令清单更新为六命令名册）；skills 两件（brainstorming / gov-arq 改指）；rules 一件（authoring-agents-skills 示例行）；templates 一件（WORKSTREAM-PROPOSAL 引用更新）。**排除清单（记录类封存，不改）**：`docs/showme/**` 9 件、`delivery/changes/**` 历史批、intake spine、workstreams progress、`docs/debt/**`。条件锚三件已亲读结案（V11）：`changes-index.mjs`、`enforce-scope.mjs`、`agent-model-guard.mjs` 全为历史 changeId 字面量，不改。
目标文件：`.claude/commands/triage-change.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/commands/loop-retro.md`（改）· `.claude/agents/planner.md`（改）· `.claude/agents/integrator.md`（改）· `.claude/agents/portfolio-planner.md`（改）· `.claude/agents/uiux-author.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/agents/scenario-author.md`（改）· `.claude/references/change-intake.md`（改）· `.claude/references/change-triage.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/references/arch-review-protocol.md`（改）· `.claude/skills/brainstorming/SKILL.md`（改）· `.claude/skills/gov-arq/SKILL.md`（改）· `.claude/rules/authoring-agents-skills.md`（改）· `docs/templates/WORKSTREAM-PROPOSAL.template.md`（改）· `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md`（改）· `docs/glossary.md`（改）

### T21 · DEV-GOVERNANCE.md 重写相关节（G1）

**问题**：§4 契约门节（人签门定义）、§4 对抗 review 节（/arch-change 统一入口）、§5 工程门表（契约门行）、§7 入口速查（六命令映射）均与新设计冲突。
**证据**：`docs/DEV-GOVERNANCE.md:75-95,97-118,151-160,183-208,236-256`（2026-10-14 普查实读要点）。
**根因**：G1 同步（同 T3）。
**改法**：§4「契约门 ≈ 架构契约评审」节改写为「契约变更 = 架构变更的一种，走 `/arch-change`；合并机械凭据闸」；§4「对抗 review」节改指（Track A 段在两面 loop 内）；§5 工程门表「契约门」行改写为「契约机械凭据闸（`pre-merge-escalation-gate.mjs`）」；§7 入口速查表按六命令新映射重写（含「架构+契约 → `/arch-change`」「治理 → `/gov-change`」）。
目标文件：`docs/DEV-GOVERNANCE.md`（改）

### T22 · routing.json 更新 + 基线预跑

**问题**：commandEntry 三行（arch-change/review-arch/review-governance → review-moderator）与新命令面不符；`architect-reviewer` 的必留信号随 T1 description 重写失效。
**证据**：`.claude/route-fixtures/routing.json` 全文（2026-10-15 实读）；`260928-gov-review-requirement-loop` retro F2 教训（改信号集必先取基线 + §5 写明被证伪断言）。
**根因**：fixture 是路由回归的断言源，须与新实况对齐。
**改法**：commandEntry **三行全删**（R1-F6 钉死：判官亲核 `route-regression-check.mjs:41-45`——entryAgent 插入「你现在是…」正则、语义 = 入口路由断言；新 `/arch-change` / `/gov-change` 入口 = 主线程，保留 review-moderator 行即断言失实；对齐 `/requirement-change` 未登记先例）；`agentRequiredSignals["architect-reviewer"]` 重钉（新 description 的承重信号：契约 + 四查 + 架构 + 治理）；review-moderator 信号中 `/review-arch`、`/review-governance` 两条删除、`/arch-change` 保留（含 `/gov-change` 增补）；scenario-author 等其余条目不动。**起草期先跑 `route-regression-check.mjs` 取基线**，本批被证伪的既有断言在 §5 逐条写明。
目标文件：`.claude/route-fixtures/routing.json`（改）

### T23 · ARQ-260928-arch-change-retirement 结项

**问题**：该 ARQ 的两候选（归并退役 / 并存定界）被第三路径（repurposing）替代，须如实回写而非照候选结项。
**证据**：`docs/debt/gov/ARQ-260928-arch-change-retirement.md` 全文（2026-10-14 实读）。
**根因**：debt 账本纪律（结项注记 append-only）。
**改法**：append 结项注记（第三路径 = 覆盖复用 `/arch-change` 为架构+契约面 loop 入口 + `/review-arch`、`/review-governance` 并入各面 Track A 段；边界 = 每面一个 loop 入口、跨面编排归 triage/intake）；status 翻 closed、closedBy 填本批 changeId；跑 `node .claude/scripts/debt-render.mjs` 归位。`ARQ-260923-contract-merge-deadpath` 同法处置（状态起草期核实）。
目标文件：`docs/debt/gov/ARQ-260928-arch-change-retirement.md`（改）· `docs/debt/gov/INDEX.md`（渲染器生成件）

### T24 · repo-structure.md 同步

**问题**：`docs/contracts/**` 行的「改动走 architect-reviewer 四分类 + human 契约门」与「工作产物去哪」节的流程语境随去门失效。
**证据**：`.claude/rules/repo-structure.md` 顶层地图行（2026-10-15 实读全文）。
**根因**：结构 SSOT 同步。
**改法**：contracts 行的拥有者与流程表述改（契约门 → 「写冻结 + `/arch-change` loop + 机械凭据闸」）；「工作产物去哪」节按六命令新语境更新。
目标文件：`.claude/rules/repo-structure.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| agents | `.claude/agents/architect-reviewer.md` | G2 改 | T1 扩编 |
| agents | `.claude/agents/architect.md` | G2 改 | T5，自改递归保护对象，human 授权（OQ-10） |
| agents | review-moderator / scenario-author / planner / integrator / portfolio-planner / uiux-author | G2 改 | T20 改指 |
| scope-map | `.claude/scope-map.json` | G3 改 | T2 窄槽 |
| settings | `.claude/review-loop.settings.json` | G3 改 | T16 删契约 deny |
| scripts | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 改 | **T15 高敏：唯一机械判定逻辑变化** |
| scripts | `.claude/scripts/changes-index.mjs` | 未触 | 亲读结案（V11）：4 处均为历史 changeId 字面量（known-pattern 键/注释），功能引用为零，不改 |
| fixtures | `.claude/route-fixtures/routing.json` | G3 改 | T22 |
| commands | `arch-change.md` | G2 改 | T7 repurposing |
| commands | `gov-change.md` | 新增 | T11 |
| commands | `review-arch.md` / `review-governance.md` | 删除 | T9 / T13 |
| commands | triage-change / build-feature / loop-retro | G2 改 | T20 改指 |
| references | `arch-change.md` / `gov-change.md` | 新增 | T8 / T12（两面流程 SSOT） |
| references | architect-operating-model / arch-review-protocol / contract-change-evidence / pod-coordination / change-intake / change-triage / task-intake-and-slicing / dev-loop / dev-loop-operating-system | G2 改 | T6 / T17 / T19 / T20（dev-loop-operating-system 为 R1-F2 补入） |
| rules | contracts.md / repo-structure.md / authoring-agents-skills.md | G2 改 | T14 / T24 / T20 |
| skills | brainstorming / gov-arq | G2 改 | T20 改指 |
| hooks | enforce-scope.mjs / agent-model-guard.mjs | 未触 | 亲读结案（V11）：各 1 处均为历史事件注释，功能引用为零，不改 |
| 指令根 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | **G1 改** | T3 / T4 / T21 |
| spec 面 | `docs/architecture.md` | 改 | T10 新增 §12（内容零语义改动） |
| 契约面 | `docs/contracts/**` | **零触碰** | 本批只改契约面的治理流程 |
| 账本 | CDQ / ARQ-260928 / gov INDEX | 改+生成件 | T18 / T23 |
| templates | WORKSTREAM-PROPOSAL / GOVERNANCE-MIGRATION-GUIDE | 改 | T20 |
| docs | glossary.md | G2 改 | T20（R1-F2 补入：术语/入口行随新名册） |

**高敏子面证成（判官请按高敏标准审这两处）**：

- **T15（合并闸）**：变化方向 = 从「恒 BLOCK」到「凭据放行」——是**放宽**而非收紧，这是本批最危险的一处，故：① 凭据判据为三件联立（FROZEN 状态行 + PASS verdict + manifest 覆盖），任一缺失即 BLOCK（向严保持）；② 六个 self-check 用例钉死（含三个反向用例，三件联立肢各有否定覆盖）；③ 解析一律复用 `.claude/scripts/lib/governance-gate-shared.mjs` 单源（manifest 提取 + `isFrozen` + `VERDICT_STATUS_RE`，不新写解析，降低新代码面）。
- **组 D（去人签门）**：安全网从「人签字」换为「写防线保留（Bash 重定向恒拦 + 实现面 agent 与 dev-loop 会话契约禁写不变；architect / review-moderator scope-map 契约写权维持现状，落地恒归主线程）+ 四分类过滤保留 + 契约包三节强制 + T15 三件套凭据 + CDQ 跨时间账本 + 可回滚」（R1-F1 精确化）。人未退出 loop——立项 ack 与例外升级照常；被去掉的只是「无信息优势的例行签字」（机械实况：旧闸 `hasContractGateEvidence` 写死 false，人批时无任何机械辅助）。

**Track B 轨道证成**：① 升级判据两条均未命中——`docs/contracts/**` 内容零触碰（非「触契约面」）、非跨系统结构性变更；② 治理类变更不强制升级（AGENTS.md 现行条款）；③ 方向收敛已由 2026-10-14/15 多轮讨论承担（Track A 的方向收敛功能已发生，本会话实录即证据，含 human 对全部 11 个 OQ 的逐点裁定）；④ 现行 `architect-reviewer.md` 硬排除第 6 条「改 gate」字面命中 T14/T15——若判官认为本条应机械适用，主线程接受升 Track A（存疑向严，判权在判官）。

## 4. human 取舍点（OQ）

全部已裁（2026-10-14/15 主线程会话），逐条记录凭据；立项 ack = 对本表与 §2 判据清单措辞的最终确认。

| OQ | 问题 | 裁定（日期） |
|---|---|---|
| OQ-1 | 契约门去留 | **去人签字门**（2026-10-15）：「人看反而不如 Architect Agent 看得仔细」；写冻结/分类过滤保留，合并闸改机械凭据；契约变更 = 架构变更的一种 |
| OQ-2 | 架构面是否分档（档0/1/2） | **不分档、审恒定**（2026-10-14）：「就算只改一个字，你也得去 review」；分档 = 作者自路由 |
| OQ-3 | 是否设轻路径 | **设**（2026-10-15）：审恒定、落地两档；判官判档、只升不降；判据清单见 T8/T12 |
| OQ-4 | Track A 形态 | **loop 内 Track A 段**（2026-10-15）：方向不清 → 起草候选 → 对抗收敛 → human 拍方向 → 流回 Track B 落地；`/review-arch`、`/review-governance` 并入 |
| OQ-5 | 命名 | **覆盖复用 `/arch-change`**（2026-10-15）：不新铸命令名；ARQ-260928 以第三路径结项 |
| OQ-6 | 治理面起草者 | **architect**（2026-10-15）：主线程四面纯编排（「主 agent 可能没办法帮你起草好这份 governance 的变更」） |
| OQ-7 | 审稿角色 | **零新增 agent，architect-reviewer 扩编**（2026-10-15 主线程建议、human 认同）：命名冲突随之消解 |
| OQ-8 | architecture.md 定稿载体 | **变更记录节（changeId 即定稿标记）**，不设状态行/文档版本号 |
| OQ-9 | 元结构 | **四面统一「起草/审稿对偶 + 纯编排主线程」**（2026-10-15） |
| OQ-10 | architect.md 改动授权（自改递归保护对象） | **随立项 ack 一并授权**（T5） |
| OQ-11 | 本批轨道 | **Track B**（证成见 §3 末节；判官认为须升则升） |

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 验证 | 命令/方法 | 预期 | 时点 |
|---|---|---|---|---|
| V1 | 路由回归基线 + 落地后绿 | `node .claude/scripts/route-regression-check.mjs`（**基线已取：2026-10-15 实跑 = 53 ok, 0 fail**；被证伪断言预列：review-moderator 信号 `/review-arch`、`/review-governance` 两条；architect-reviewer 信号三条全换；commandEntry 三行）→ landing 后复跑 + `--self-check` | 落地后绿；被证伪清单与实际一致 | pre-commit 基线 / post-commit 复跑 |
| V2 | spec 锚检 | `node .claude/scripts/spec-anchor-check.mjs` | 0 错误（§12 计入 archSectionCount，零悬空） | post-commit |
| V3 | description lint | `node .claude/scripts/lint-descriptions.mjs` | 新增/改写 description 无流程叙述气味告警 | post-commit |
| V4 | JSON 形态 | `node -e "JSON.parse(...)"` 于 scope-map.json / routing.json / review-loop.settings.json | 全部合法 | post-commit |
| V5 | 合并闸自检 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | 绿；输出含六个契约凭据新用例名（T15） | post-commit |
| V6 | 引用普查归零 | `grep -rn "/review-arch\|/review-governance" .claude/ AGENTS.md CLAUDE.md docs/ --exclude-dir=archive --exclude-dir=showme`（showme = 记录类归档排除，R1-F2；排除全清单见 T20） | 排除清单（记录类封存 + 历史制品）之外归零；`/arch-change` 旧语义语境词（统一入口/Track A 入口）同步归零 | post-commit |
| V7 | debt 归位 | `node .claude/scripts/debt-render.mjs` | ARQ-260928（及 ARQ-260923 若结）归 `_archive/2026/` | post-commit |
| V8 | 落地锚检 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本PROPOSAL>` | 锚并集 ≡ manifest，无目录 token advisory | pre-commit（FROZEN 前） |
| V9 | 三场景纸上走查 | 文审走查：① 架构措辞修订（轻路径：审 → 瘦制品五件过闸 → 合并 → 变更记录节）② 架构语义+契约变更（主路径 + 契约包三节 + T15 凭据放行）③ 开口架构方向（Track A 段 → 收敛 → Track B 后端） | 每场景的角色序列、判档点、落地凭据在 SSOT 文本中逐步可追 | pre-commit |
| V10 | 破坏性自检 | `git status --porcelain` 全清单逐项核 + `git diff HEAD -- docs/contracts/` | 契约内容零改动；dev-loop.settings.json 契约 deny 在；scope-map 其余条目零改动；hooks 两条件锚件的功能行零改动 | post-commit |
| V11 | 遗留亲核项 | **已完成（2026-10-15）**：① 三条件锚亲读 = 全部历史 changeId 字面量，不改（§3 已结案）；② `ARQ-260923-contract-merge-deadpath` 已核实结项（`_archive/2026/`，2026-09-24 结，建议①留根由本批 T15 兑现）；③ V1 基线 53 ok 0 fail 在案 | 见 §3 条件锚行与 §5 V1 | pre-commit（FROZEN 前）**已完成** |

## 6. 留根清单

1. **需求面与新城堡的形态差统一评估**（需求面档 0/1 无 reviewer 审、无 Track B 后端，与「审恒定 + 落地两档」的元结构不齐；需求面跑得好不动，统一性评估留根）→ 下需求面治理批。
2. **reviewer 大一统评估**（requirement-reviewer 与 architect-reviewer 四查骨架同源，是否并为单 reviewer + 面参数）→ 新 reviewer 实跑 2–3 单后立项。
3. **architecture.md 变更记录节的机械消费齿**（launchPreconditions 读行在场）→ 首个消费实例出现后立项。
4. **契约机械凭据闸补强评估**（T15 跑 2–3 批后：凭据形态是否需细化〔如 verdict 解析形态、arch-review token 以外的面〕）→ 摩擦立项。
5. **CDQ 存废评估**（loop 凭据列跑数批后若退化为纯指针，可并入变更记录节）→ 摩擦立项。
6. **requirement-change 命令未登记 routing.json commandEntry 的形态对齐**（本批 T22 定 arch-change/gov-change 登记形态时一并对齐）→ 随 T22 亲核结论。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-15 - architect

verdict: BLOCK(F1,F2,F3,F4,F5,F6,F7)

轨道裁决先行：PROPOSAL §3 末节 Track B 证成**成立**，本判官不启动升级 human（理由见末节「轨道裁决」）。以下 findings 均为稿内可修项，不涉改法方向性错误。

**F1（BLOCKING）· 四处「settings/scope-map deny 不动」与 T16 删 settings deny 正面矛盾；T16 证成句「会话内 agent 仍全禁」失实**

- 证据（亲核）：
  - T3 承重替换句：「契约面写冻结（**settings/scope-map deny 不动**）+ 契约合并机械凭据闸」（PROPOSAL :109）；T4 替换句：「写冻结保留：**settings/scope-map 的契约 deny 不动**」（:117）；T14：「对所有 agent 只读（**settings/scope-map deny 不动**）」（:205）；§3 组 D 证成：「写冻结保留（**settings/scope-map deny 不动**）」（:318）。
  - T16 改法：「**删除 `permissions.deny` 整条**」（:221）——被删的正是 settings 级契约 deny（实测 `review-loop.settings.json:12-16` deny 仅 `Edit(./docs/contracts/**)` 一条）。
  - T16 证成句「契约的防线改为 = scope-map per-agent deny（**会话内 agent 仍全禁**）」（:220）失实：`scope-map.json:88` architect allow 含 `"docs/contracts/**"`、`:102` review-moderator allow 同——会话内两角色有 allow（新世界里落契约的合法写者恰是主线程 + 这些角色，这本身可以是设计，但表述不能写成「仍全禁」）。
- 修法方向：统一改写「写冻结」的实际构成 = scope-map per-agent deny + universal bashDenyWrite（实测 `scope-map.json:25` 含 `docs/contracts/**`）+ dev-loop.settings 契约 deny 保留（实测 `dev-loop.settings.json:25` 在场）+ 主线程 packet-bound + T15 闸；review-loop.settings 删 deny 如实表述为本批有意动作；G1 两句（T3/T4）是承重替换句，必须与 T16 一字不矛盾。

**F2（BLOCKING）· 引用普查漏面：V6 按现文不可绿；arch-change 普查计数不可复现**

- 证据（亲核实测 2026-10-15）：
  - `grep -rln "review-arch\|review-governance"`（`.claude/` + AGENTS.md + CLAUDE.md + `docs/`，排 docs/debt、docs/archive）= **19** ✓ 与 PROPOSAL 一致；同口径 `grep -rln "arch-change"` = **34**（PROPOSAL 称 21，不可复现；即使排除 showme 9 件 + glossary 仍余 24）。
  - 未覆盖活引用三类：① `docs/showme/**` 9 件——`show-me-gov-track-a.html:120` 实测含带斜杠 `<code>/review-arch &lt;spec&gt;</code>` 与 `<code>/review-governance` → V6 现文命令（`grep -rn "/review-arch\|/review-governance" .claude/ AGENTS.md CLAUDE.md docs/ --exclude-dir=archive`，PROPOSAL :349）必中此件，「除功能保留件与历史制品外归零」不成立，而 T20 排除清单（:251）未含 `docs/showme/**`；② `docs/glossary.md:33`「Track A …（入口 `/arch-change` 升级走）」——glossary 是 `architecture.md` §11（:428）指定的术语权威，新世界 Track A 段住两面 loop，定义半失效；③ `.claude/references/dev-loop-operating-system.md:158`「architect 经 `/arch-change` 处理已登记 ARQ」——新世界治理流量归 `/gov-change`。
  - `delivery/workstreams/**`（`m2-char-window/progress.md:126,128`、`m3-schedule/progress.md:267`、`m3-schedule/decision-coverage.json:117`）与 `delivery/changes/intake/**` 两件 spine 亦有命中（性质 = 历史决策记录，可归豁免，但排除清单未点名）。
- 修法方向：`docs/showme/**`、`delivery/changes/intake/**`、delivery/workstreams 记录类显式列入 T20「历史制品不改」；`docs/glossary.md` 与 `dev-loop-operating-system.md` 纳入 T20 改指 + manifest 增补 2 件；V6 命令与排除清单对齐；普查计数改写实测值。

**F3（BLOCKING）· 轻路径的合并通道未定义：按现文轻路径批永远无法过前置升级闸**

- 证据（亲核）：T7⑤「轻路径 = 主线程落 + architecture.md 变更记录节 append」（:141）、T11④「轻路径 = 主线程落 + commit」（:181）、T12 同（:189）——文本均止于「落」；而闸实况 = `pre-merge-escalation-gate.mjs:302-308` gov kind 机锚要求 PROPOSAL FROZEN + §7 GREEN + landing-verdict 在产，`:310` unknown kind（无批目录）BLOCK，`:576-578` gov 档带 `--human-approval-ref` 直接 FATAL（「gov 档无人批豁免概念」）。轻路径批（无 FROZEN / 无 verdict / 甚至无目录）三种情形全 BLOCK → 每次合并升级人，与「前端对齐 + 例外反选、其余自动」的目标效果（:62）直接冲突；V9 走查场景①（:352）亦止于「落」、未走到合并。
- 修法方向（三选一钉入 T8/T12 SSOT）：① 声明轻改动随下一主路径批合并搭载（闸消费主路径批 changeId，轻改动在变更记录节如实登记）；② 轻路径也产最小锚（reviewer GREEN + 主线程 FROZEN 翻牌 + 轻量 verdict）= 名义瘦 Track B；③ 闸开注册式轻路径豁免。

**F4（BLOCKING）· T15 self-check 用例漏一联立肢（三件联立钉不住 FROZEN 肢）**

- 证据（亲核）：T15 凭据 = 三件联立（FROZEN 状态行 + verdict PASS + manifest 覆盖，:213 ④⑤）；枚举用例五件（:213）覆盖的否定肢 = verdict-not-pass + manifest-not-covering + 无凭据，**缺 `contracts-diff-proposal-not-frozen-block`**——若实现回归丢掉 FROZEN 检查（只验 verdict + manifest），五用例全绿（真空绿规约：每个联言肢须配否定用例；本仓「夹具必须能判别」教训同款）。
- 修法方向：补该用例（候选 PROPOSAL 无 FROZEN 状态行、verdict PASS、manifest 覆盖 → 仍 BLOCK）。

**F5（SHOULD）· T15 的 FROZEN/verdict 解析未指单源，有重蹈 verdict-anchor 病灶风险**

- 证据（亲核）：T15 规格仅写「读其头部状态行须含 FROZEN」「verdict 为 PASS」（:213 ④），未指解析单源；lib 已有 `governance-gate-shared.mjs` 的 `isFrozen` 与 `VERDICT_STATUS_RE`（实测 `.claude/scripts/lib/governance-gate-shared.mjs:629`，行首标签形态、token 集 PASS|GREEN|RED|FAIL|BLOCK）；本仓 260918/260920 两批 verdict-anchor 病灶史在案（gate :95-99 注释：全文 token 恒真、PASS spoof 面已被论证否决一次）。另 T15 引该模块未带 `lib/` 路径（`dev-loop.md:339` 称其为「字节级单一源」）。
- 修法方向：规格写明复用 lib 单源（isFrozen + VERDICT_STATUS_RE），token 取舍（PASS only vs 兼容 GREEN）写一句 deliberation；模块路径补 `lib/`。

**F6（SHOULD）· T22 arch-change commandEntry 处置仍为未决二分支，FROZEN 前须钉死**

- 证据（亲核）：T22 改法「arch-change 行处置 = 起草期实读 route-regression-check.mjs 的校验形态后定（若…则…；若…则…）」（:269）——判官亲核 `route-regression-check.mjs:41-45`：entryAgent 仅作字符串插入正则 `` 你现在是[^\n]{0,40}${entryAgent} ``，机械上任意字符串可过；但语义断言 = 「命令入口路由到该 agent」。新 `/arch-change` 入口编排 = 主线程（T7 :141），review-moderator 仅 Track A 段内被派——保留 review-moderator 行 = 断言失实。且 V1 预列被证伪断言含「commandEntry 三行」（:344），仅在三行全删时自洽。
- 修法方向：钉死为「三行全删、对齐 requirement-change 未登记先例」（requirement-change 同为主线程编排、未登记 commandEntry，实测 `routing.json:3-16` 无其行），留根 6 随之一并裁；或显式选另一分支并同步修 V1 预列。

**F7（NIT）· T20 改法内计数自相矛盾**

- 证据：T20 改法「agents **五件**（planner/integrator/portfolio-planner/uiux-author/review-moderator/scenario-author…）」（:253）实列 6 件；「references **六件**（change-intake…change-triage/task-intake-and-slicing/arch-review-protocol/dev-loop…）」实列 5 件。
- 修法方向：计数对齐实列。

**轨道裁决（简报焦点 3）**：PROPOSAL §3 末节 Track B 证成**成立，不升级 human**。① `architect-reviewer.md:75`「改 gate → ESCALATE_FULL_REVIEW 或 HUMAN_REQUIRED」属 lightweight eligibility 机检硬排除，所在节（`arch-review-protocol.md:88-105`）明标「Track A 触发时适用」（:90）——它是 Track A 内部 depth 分类器，非 Track A/B 路由判据；路由判据 = 双轨路由节两条（:75-78：触 `docs/contracts/**` 文件面 / 跨系统结构性），本批均未命中（契约**内容**零触碰、非跨系统）。② 双轨路由节明言 governance 类变更不强制升级（:80）。③ 最高敏两项决策（去人签门 OQ-1、本批轨道 OQ-11）已由 human 于 2026-10-15 立项 ack 逐点亲裁——Track A 的方向收敛功能已由 human 直接承担。④ 残留风险有 containment：T15 self-check、landing-verifier 独立核验、回滚 = git revert、remote push 仍 human。

**焦点 1 高敏子面结论**：组 D 安全网六件——过滤器保留 ✓（T1+T14 五值 canonical；CDQ 实测 `ACCOMPANIES_SPEC_CHANGE` 在用于 :9-13 行）、契约包三节 ✓（T8 字段名沿用 `architect-reviewer.md:147-161` 实测在场）、凭据闸 ✓ 但见 F4/F5、CDQ ✓（T18 + 通用 4 不动，gate :271-276 实测）、回滚 ✓、dev-loop 防线 ✓（`dev-loop.settings.json:25` 实测在场）；「写冻结」表述破洞见 F1。T15 方向 = 放宽，三件联立 + 用例骨架方向正确，缺口 = F4（一联立肢无否定用例）+ F5（解析未指单源）。

**焦点 2 判据机械可判性**：T8 轻路径四条可判（②改动性质闭枚举 + ③④路径/节号机判，①由②兜底 + 只升不降；节号实测在场——`architecture.md:32` §1.2、`:64` §2.2、`:85` §2.3、`:377` §9）；Track A 段三条中 ① = human 裁定点、③ = 机械触发、② = 只升不降的升级触发（向严方向、非放行判据）——成立；T12「路径即判据」①③ 纯路径机判 + 「命中任一恒 Track B 哪怕一字符」——成立。

**焦点 4 G1 承重替换句**：除 F1 的矛盾外，T3 其余替换语义（cast 更新 / 删判据① / 面入口两行 / 「不动」节另两条）与 §2 各 T 新语义一致，无内部矛盾。

**焦点 5 manifest ≡ §2 锚并集**：43 件逐 token 对账**全中**（T9/T13 两删除件在册 :166/:198、三条件锚已正确移出并入 §3「未触」行 :298/:308）；结构字段硬条款满足（landing-manifest 围栏块 :12-56、packet-id 行 :6 在场）。注意：F2 若采纳（glossary + dev-loop-operating-system 入 T20），manifest 须同步 +2。

**亲核留痕（本轮实测清单）**：`scope-map.json` 全量（architect-reviewer :121-124 `allow:[]/deny:**` ✓、landing-verifier :137-139 双 token 写槽 ✓、architect/review-moderator 契约 allow 实况）；`review-loop.settings.json` 全文（deny 单条 ✓、`_doc` 形态教训 :7 ✓）；`dev-loop.settings.json:25`；`pre-merge-escalation-gate.mjs` :95-113 / :262-265 / :295-313 / :329-341 / :355-375 / :413 / :576-578（通用 1/3/4、gov 机锚、kind 推断、self-check `contracts-block` :413 在场 ✓、`hasContractGateEvidence: false` :366 逐字 ✓）；`route-regression-check.mjs` :41-50 + 实跑基线 **53 ok 0 fail**（V1 声称复现 ✓）；`routing.json` 全文（commandEntry 三行 ✓、architect-reviewer 信号三条 ✓、review-moderator 信号 ✓）；CDQ 全文（:4 状态词、:6 列表头、「human 门」列 ✓）；`pod-coordination.md` :41 / :185-205（§10e 全文 ✓、留根句 :205 ✓）；`architect-reviewer.md` 全文（:3 两职责 ✓、:63-88 lightweight 节 ✓、:75 gate 硬排除 ✓、:147-161 契约包字段 ✓）；`architect.md` :22-39（:28-29 ✓）；`architect-operating-model.md` `/arch-change` 命中 12 处 ✓（:75、:79 ✓）；`review-arch.md` :12-13 ✓、`review-governance.md` :13-14 ✓；`arch-change.md` :1-40（phase-0 / review-depth / 契约预审步 ✓）；`contract-change-evidence.md` :7-19 / :36-49 ✓；`architecture.md` :1-8 / 节标题全列 / tail（§11 术语表收尾 ✓、无版本区 ✓）；`spec-anchor-check.mjs` :46-50 正则 ✓；`task-intake-and-slicing.md:294` ✓；`requirement-change.md` 全文 ✓；`change-intake.md` §8 ✓；ARQ-260928 全文（两候选 ✓、status deferred ✓）；ARQ-260923 在 `_archive/2026/`、closed 2026-09-24 ✓；260928-requirement-loop PROPOSAL §6 留根「架构档挂入同一 loop 骨架」✓；`requirements.md` :7-8 v1.63 frozen ✓；landing-verdict 实证形态（261014 批 :94 `verdict: PASS` ✓）；`lib/governance-gate-shared.mjs:629` VERDICT_STATUS_RE ✓；条件锚三件（`changes-index.mjs` :50/:55/:225/:510 四处、`enforce-scope.mjs:179`、`agent-model-guard.mjs:2`）抽核 = 全部历史 changeId 字面量（「review-arch」命中均为 `review-architect` / `gov-review-arch-*` 子串碰撞、非命令引用），V11① 结论亲核成立 ✓；引用普查两套 grep 实跑（计数出入见 F2）。

### R2 - 2026-10-15 - architect

verdict: BLOCK(F1,F2,F3)

轮次纪律：本轮聚焦 R1 七项 findings 的修订到位性 + 修订是否引入新矛盾，不做全量重核。结论：F1/F2/F4/F5/F6/F7 六项修订**到位**（逐项核对见末节）；F3 修订方向正确（瘦制品过闸 = R1 所给选项②）但**仍缺一环**（F1）；修订另引入三处数字漂移 + 一处走查未延伸（F2）。无升级 human 项。

**F1（BLOCKING）· F3 修订仍缺一环：瘦制品四件不含 architect-landing-summary.md，而 landing-fidelity-check 硬要求该文件——轻批仍产不出 PASS verdict**

- 证据（亲核）：T7⑤（:143）/T8「轻路径落地通道」（:157）/T11④（:185）定义的轻批制品 = 瘦 PROPOSAL + §7 GREEN + landing-manifest + landing-verdict 四件；但 `landing-fidelity-check.mjs:575` 实测 `if (!existsSync(summaryPath)) fail('architect-landing-summary.md is required（landing 声称 + packet-binding）')`——该文件是 landing-verifier 结构门的**硬必备件**，且 :584 另要求其文本含 `packet-id` + 被批 review id（A9 packet-binding）。无 summary → landing-verifier 结构门必红 → 写不出 verdict PASS → 升级闸 `:308`「landing-verdict 不在产」仍 BLOCK。即：F3 修订把「机锚全在场、闸零改动」只核算到升级闸三锚（FROZEN/GREEN/verdict-exists），没核算**产生 verdict 之前**的结构门。
- 修法方向：瘦制品清单补一件「瘦 `architect-landing-summary.md`（packet-id + 被批 review id + landing-manifest 围栏块 + claimed 清单，数行）」——T7⑤ / T8 通道节 / T11④ / T12 引用节同步；landing-fidelity-check 保持零改动（与「闸零改动」承诺一致）。

**F2（SHOULD）· 修订传导不全：三处二手引用仍挂旧数字，V9 走查未延伸到新通道**

- 证据（亲核）：① §2 头部实证义务段（:86）仍写「`/arch-change` 引用普查实测 **21** 文件」——T20 证据行（:257）已改为「**34** 文件（两轮）」；② §3 高敏子面证成（:324）仍写「② **五个** self-check 用例钉死（含**两个**反向用例）」——T15（:219）已改为**六例**（反向肢三个：not-frozen / not-covering / not-pass），同句 ③「复用 … 的 manifest 提取」亦低于 T15 ④ 的「FROZEN/verdict 判定一律复用 lib 单源」口径；③ V5（:355）预期仍写「输出含**五个**契约凭据新用例名（T15）」——应为六（该行是机械可判预期，落地后自检输出六名，预期与现实错位）；④ V9 场景①（:359）仍止于「审 → 落 → 变更记录节」——F3 修订后轻路径的关键新增段正是「瘦制品过闸 → 合并」，走查未覆盖。
- 修法方向：四处对齐（21→34 两轮、五→六例三反向、V5 预期六名、V9 场景① 补「瘦制品过闸 → 合并」段）。

**F3（NIT）· T14 括注角色标签「review-moderator（落地）」易误导**

- 证据：T14 纪律正文（:211）「architect（起草）与 review-moderator（落地）的契约写权维持 scope-map 现状」——落地恒归主线程 packet-bound（路由 SSOT 常态规则），review-moderator 的契约 allow 是 spec profile 低影响落地边界的形态，并非「契约落地者」。
- 修法方向：括注改中性表述（如「architect 与 review-moderator 的契约写权维持 scope-map 现状不动」），不带角色归因。

**R1 七项修订到位性逐项核对**：

- **R1-F1 ✓ 到位**：T3「不动」节替换句（:111）、T4（:119）、T14（:211）、T16 根因段（:226）、§3 组 D 证成（:325）五处已统一为「bashDenyWrite 恒拦 + dev-loop.settings deny 保留 + 实现面/审稿面 scope-map deny 不变 + architect/review-moderator allow 维持现状」，「settings deny 不动」「会话内 agent 仍全禁」字样已清除；新表述与实况逐点相符（`scope-map.json:25` bashDenyWrite 含契约 ✓、`dev-loop.settings.json:25` deny 在场 ✓、两 allow 条目 :88/:102 ✓）。残留 = 本条 F3（NIT）。
- **R1-F2 ✓ 到位**：T20 证据行改两轮普查 34 文件（:257）✓；排除清单成文（showme 9 件 / delivery/changes/** / intake spine / workstreams progress / docs/debt / git 历史，:257/:259）✓；glossary 与 dev-loop-operating-system 入 T20 目标文件（:260）与 manifest（:42/:55，43→45 件 ✓）与 §3 表（:311/:320）✓；V6 加 `--exclude-dir=showme`（:356）✓。判官复跑 V6 命令现状命中 11 文件全部有 T 归属（T6/T7/T9/T13/T20/T21/T22 覆盖），docs/debt 零 slash 命中——落地后归零可期 ✓。
- **R1-F3 方向 ✓、完整度 ✗（=本条 F1）**：瘦制品过闸 = R1 所给选项②，升级闸三锚核算正确（FROZEN 机锚形态由 `isFrozen`/闸 RE 承载、GREEN 由 §7 verdict 行承载、verdict 在场由瘦批 landing-verdict 承载），retro 豁免合理；缺口 = 结构门硬件 architect-landing-summary（本条 F1）。
- **R1-F4 ✓ 到位**：T15 self-check 六例（:219），`contracts-diff-proposal-not-frozen-block` 在列，三联立肢各有否定覆盖 ✓。
- **R1-F5 ✓ 到位**：T15 ④ 明写 FROZEN/verdict/manifest 判定一律复用 `.claude/scripts/lib/governance-gate-shared.mjs` 单源（isFrozen + VERDICT_STATUS_RE），模块带 lib/ 路径 ✓。
- **R1-F6 ✓ 到位**：T22 钉死 commandEntry 三行全删（:275），对齐 requirement-change 未登记先例，与 V1 预列「commandEntry 三行」自洽 ✓；留根 6 由该决定自然收口。
- **R1-F7 ✓ 到位**：T20 计数修正（agents 六件 / references 六件，:259），与目标文件行 21 件逐一点名一致 ✓。

**亲核留痕（R2 实测）**：v0.3 全文通读；`landing-fidelity-check.mjs:571-575,584-585,621,634`（summary 硬必备 + packet-binding 机判，逐字在上）；V6 命令复跑（命中 11 文件分类在上）；manifest 45 件逐行点数 ✓；`scope-map.json:25` / `dev-loop.settings.json:25` 复核 ✓；§2 头部 :86 / §3 :324 / V5 :355 / V9 :359 四处旧数字定位逐字在上。

### R3 - 2026-10-15 - architect

verdict: GREEN

轮次纪律：聚焦 R2 三项 findings 的修订到位性 + 有无新矛盾。三项全部到位、未见新矛盾；结构字段硬条款复核在场（landing-manifest 围栏块 45 件 :12-58、packet-id 行 :6）。GREEN 判据 = 无未结 BLOCKING/SHOULD。

**R2 三项修订到位性逐项核对**：

- **R2-F1 ✓ 到位**：瘦制品四件→**五件**四处同步——T7⑤（:143）、T8「轻路径落地通道」（:157）、T11④（:185）、T12 引用节（:195）均含第五件「瘦 architect-landing-summary（packet-id + manifest 复述 + claimed 清单，数行）」并标注 landing-fidelity-check 结构门硬要求件由来；与 `landing-fidelity-check.mjs:575`（summary 硬必备）/`:584`（packet-id + review id 机判）的实况对齐，「闸零改动」承诺不受影响（改动的是轻批制品清单，非任何门脚本）。
- **R2-F2 ✓ 到位（四处）**：① §2 头部实证义务段（:86）普查口径已改「34 文件」；② §3 T15 高敏证成（:324）已改「六个 self-check 用例（含三个反向用例，三件联立肢各有否定覆盖）+ 解析一律复用 lib/governance-gate-shared.mjs 单源（manifest 提取 + isFrozen + VERDICT_STATUS_RE）」；③ V5（:355）预期已改「六个契约凭据新用例名」；④ V9 场景①（:359）已改「审 → 瘦制品五件过闸 → 合并 → 变更记录节」，新通道全程进走查。
- **R2-F3 ✓ 到位（四处同口径）**：T14 纪律正文（:211）改「architect（起草）与 review-moderator（Track A 段编排）的 scope-map 契约写权维持现状——落地恒归主线程 packet-bound」；T3「不动」节括注（:111）、T16 根因段（:226）、§3 组 D 证成（:325）三处主动传导同口径，均无「review-moderator（落地）」误导标签残留。

**新矛盾扫描（R3 负结果）**：① 全稿 grep「四件/五件」——「四件」残留仅两处且均合法：T15 根因（:218）「loop 落地四件」指主路径 Track B 制品 4 件（protocol 既有术语，与轻路径瘦制品五件是两个集合，不冲突）、R1/R2 评审记录内的历史引述（append-only 不改）；② 旧数字（21 文件 / 五个用例 / 两个反向）全稿零残留（评审记录引述除外）；③ T15 凭据候选恒来自主路径批（轻路径判据③ 不触契约），主路径全件制品天然满足 summary 硬要求，轻/主两档与 T15 无交叉矛盾；④ V1 被证伪预列与 T22 钉死形态自洽（commandEntry 三行全删）。

**三轮收口小结**：R1 七项（轨道证成获确认 + 写防线矛盾 / 普查漏面 / 轻路径过闸 / self-check 联立肢 / 解析单源 / commandEntry 未决 / 计数）→ R2 三项（瘦制品缺 summary / 传导四处 / 措辞标签）→ R3 全修齐。判官对 Track B 轨道的裁决维持 R1 结论（成立、不升级）。本 GREEN 的效力范围 = PROPOSAL 文本自洽性与可落地性；落地保真由后续 landing-verifier 独立核验承接。

**亲核留痕（R3 实测）**：v0.4 修订十处逐处 sed/grep 定位核对（行号与逐字在上）；「四件/五件/21 文件/五个/两个反向」全稿残留扫描（结果在上）；结构字段（manifest 45 件、packet-id 行）复核在场。
