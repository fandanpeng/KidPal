# PROPOSAL · 260918-gov-review-dispatch-chain-friction

> **状态：DRAFT v0.5 · 2026-09-19 · 已立项 ack · 待 architect R2 复审**（凭据：human 2026-09-18 立项 ack + OQ 全裁；**R1 BLOCK(F1..F4) 后按 human 2026-09-19 裁定把 T1〔高风险合法动作预防声明〕整条移出本批**、退回 ARQ 队列单开一批，见 §6 留根①；R1 针对幸存 T 的 F3/F4 已在 v0.5 收口，见下）。
> **承载 ARQ**：`ARQ-260913-device-round-protocol`（本批唯一 T）。**原承载的 `ARQ-260913-auto-mode-classifier-legit-workflow` 已移出本批、退回队列**（该 ARQ 的改法经 R1 实测**打错机制**，见 §6 留根①）。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `-merge-chain-friction`（已 FROZEN v1.0）/ `-closeout-chain-friction`（已 FROZEN v1.0）/ `-batch-drafting-friction`。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：本批为 skill 面 + reference 指针行的**纯成文与固化**，零闸判定改动、零脚本行为改动、零 agent frontmatter 改动；不新增节、不改既有三节内容）。
> **review profile**：G2（`.claude/references/pod-coordination.md` + `.claude/skills/kidpal-serial-monitor/SKILL.md`）。契约面 / G1 / G3 均为零。**本批是本拆包族里零闸语义触点的一批**。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/pod-coordination.md
.claude/skills/kidpal-serial-monitor/SKILL.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把真机轮协调协议三件套**住进耐久的家**——它已经让 4 个包重复交过学费，却一直散在各包 retro 里。

**现状差距**：这不是「不知道怎么做」，而是「知道了但没地方写」——三条关键姿势（何时开捕获窗口、怎么抓串口、动作点怎么下）全部只活在各自的 pod-retro 里，下一包的 agent 只能重新踩一遍。

**非目标（本批不做什么）**：不改任何 agent 的 frontmatter（tools / name / model / description 全不动）；不改 `kidpal-serial-monitor` skill 的**既有三节内容**（只新增一节 + 在既有节补一行指向）；不动闸、脚本、任何 `*.acceptance.*`；**不并入 T1〔预防声明〕**（已移出本批，见 §6 留根①）。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节断言均为 2026-09-18/19 在 `gov/kidpal` 工作树亲核所得；skill 行数与结构为现跑 `wc -l` / `grep -n` 实测。**本节经 architect R1 独立亲核逐条通过**（留痕见 §7 R1）。

### 问题 1：真机轮协调协议三件套未固化，每次真机轮重复交学费

证据（`delivery/changes/retro/2026-09-13-loop-retro-efficiency.html`，4 包实证）：

1. **key-route-a1**——第一次 300s 捕获窗口**开早了**（checklist 还在读），human 操作落在窗口外、口述与日志互证不上，按「completion 不凭解释宣布」重捕一轮（该包卡点 3）。
2. **screen-unification**——合并夜**三次**设备重启与串口捕获 **DTR/RTS 关端口复位伪影**秒级吻合（其中 **21:15 / 22:50 两处时刻有实测记录**；R1-F4 计数更正：v0.2 原写「两度实证」把「两处可考时刻」说成了「两次重启」）；同夜提前 15 分钟分发的提醒时间表落空（fire 时刻设备离线、human 看不到 agent 发的表格），改实时口令「现在按 BOOT！」一次成功（该包追记）。
3. **hardware-validation / render-pages**——同族（烧录-复烧-目视等待是主循环周期；render-pages 的 boot 死锁烧录后才现形，开机冒烟已成必经步）。

**承载面实测（亲核）**：`.claude/skills/kidpal-serial-monitor/SKILL.md` 全文 **31 行**（`wc -l` 实测），三节 = `## Tools (already on disk, in c:/tmp)`（`:8`）/ `## Rules that cost time when forgotten`（`:19`）/ `## Reading a smoke round (what "good" looks like)`（`:24`）。**DTR/RTS 只在 `:10` 出现一次**，且是工具说明形态（`**`c:/tmp/serial_capture.py <seconds> <out>`** — **resets the board via DTR/RTS** then reads.`）——**未记录伪影模式**；「单一长捕获」「human 就绪信号开窗」「实时口令钉动作点」三条 **grep 零命中**。

**根因**：该 skill 成文于「被动读日志」年代，真机轮**协调**知识（谁在什么时候开窗、动作点怎么下）散在 4 包 retro 里未回收；而真机轮是含真实硬件、真实人、不可重放的一环，**协调失败的代价最高、却最没有说明书**。

## 2. 改法（T1）

### T1 · `kidpal-serial-monitor` skill 固化真机轮协调协议

承载 `ARQ-260913-device-round-protocol`。

改 `.claude/skills/kidpal-serial-monitor/SKILL.md`：

① **新增一节** `## Device round protocol / 真机轮协调协议`，三条：

- **单一长捕获**——一次开窗覆盖全窗口、中途**绝不关端口**；附 DTR/RTS 关端口复位伪影的**模式识别**（设备重启时刻与捕获启停时刻秒级吻合即为伪影，**非真重启**——screen-unification 合并夜三次重启、21:15 / 22:50 两处有实测记录）；
- **human 就绪信号开窗**——捕获窗口**不由 agent 单方面开播**，由 human 就绪信号开启（key-route-a1 窗口开早、操作落在窗外、按「completion 不凭解释宣布」重捕一轮的实证）；
- **实时口令钉动作点**——动作点**不提前分发时间表**，改实时口令（screen-unification 提前 15 分钟分发、fire 时刻设备离线落空的实证）。

② 在既有 `## Rules that cost time when forgotten` 节（`:19`）补一行指向新节。

③ 在 `.claude/references/pod-coordination.md` 的真机轮相关处补**一行引用指针**（指向该 skill 新节，**不转抄正文**——防双源漂移，沿本仓「引用不复制」惯例）。**目的表述如实化（R1-F3 采纳）**：该处是**收口 / 对账语境**、**不是**真机轮派发的主阅读面；本 T 的**主通道是 skill 自身的 description 触发**（真机轮必含串口读，该 skill 的 description 已覆盖「reading device boot logs / watching a smoke round」场景），指针的价值是**让做收口 / 对账的人知道该 skill 已含真机轮协议**——**v0.2 原写「防真机轮派发侧想不到翻该 skill」属可达性归因失准**，已据实改写。

为什么这样改：直击问题 1 的根因——三条零承载的知识散在 4 包 retro 里，skill 是它们的耐久家；同吃 260915 批「环境事实归 skills、prompt 只留指针」的口径（该批已把小智环境事实全部归入 skills 并收缩 launch prompt）。

目标文件：`.claude/skills/kidpal-serial-monitor/SKILL.md`（改）、`.claude/references/pod-coordination.md`（改：引用指针一行）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 串口 skill | `.claude/skills/kidpal-serial-monitor/SKILL.md` | G2 | T1① 新增一节 + ② 既有节一行指向。消费方 = 真机轮派发；主通道 = skill 的 description 触发（无机械消费方） |
| 派发契约 | `.claude/references/pod-coordination.md` | G2 | T1③ 引用指针一行（收口/对账语境的次要可达性辅助，非主通道）。消费方 = 收口 / 对账读者 |
| 契约面 / G1 / G3 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 本批纯成文 |

**【硬项 1b · sweep 透镜】** 本批**不引入新门、不引入新判定 pattern、不引入新枚举面**，故无预跑与 census 义务；也不涉判定面变更，无可预跑的 pattern。

**【硬项 1c · 复扫模式完备性先验】** 本批不涉退役语面 / 残留清扫，无清扫模式集。三条姿势的存在性已由「grep 零命中」现跑核验（§1），无模式完备性问题。

## 4. human 取舍点（OQ）

### OQ1 · T1 的两处落点

- **主体落点**：(a) `.claude/skills/kidpal-serial-monitor/SKILL.md`（**human 2026-09-18 已裁**——同吃 260915 批「环境事实归 skills」口径）vs (b) 其他 skill / reference 面。
- **指针落点**：(a) `.claude/references/pod-coordination.md` 补一行（**human 2026-09-18 已裁**）vs (b) 不加指针（更克制，但收口/对账读者失去一条发现路径；主通道不受影响，见 §2 T1③）。

### OQ 裁决（human 2026-09-18，v0.5 状态复核）

| OQ | 裁决 | v0.5 状态 |
|---|---|---|
| 主体落点 | **(a) 落 skill** | 维持 |
| 指针落点 | **(a) 补一行指针** | 维持；其**目的表述**已按 R1-F3 如实化（主通道 = skill description，指针为次要发现路径） |

**v0.5 的 OQ 清零说明**：v0.2 时的 OQ1（T1/T2 落点）中**关于 T1〔预防声明〕的部分随该 T 移出而作废**；关于 T2 的部分已裁并保留（上表）。human 裁决「落参考文档面、不落 agent def」对族内其余批次仍有效。

## 5. 验证集（landing 前机械前置 + landing 后核验）

本批为纯成文批，验证集以「在场性 + 零回退」为主。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `grep -n "Device round protocol" .claude/skills/kidpal-serial-monitor/SKILL.md` | 命中新节标题；且三小条各 `grep -c` ≥1：`单一长捕获` / `就绪信号`（或等价词）/ `实时口令` |
| V2 | `wc -l .claude/skills/kidpal-serial-monitor/SKILL.md` + 三个既有节标题各 `grep -c` | 行数由 31 增至新值；**既有三节标题仍在**（各 =1，防重写覆盖） |
| V3 | `grep -c "kidpal-serial-monitor" .claude/references/pod-coordination.md` | ≥1（指针行在场）；且该行**不含**转抄的正文内容（防双源漂移——目视核） |
| V4 | `grep -c "DTR/RTS" .claude/skills/kidpal-serial-monitor/SKILL.md` | ≥2（`:10` 原有 1 处 + 新节伪影模式 ≥1 处） |
| V5 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件）。**注**：本批不动 frontmatter，两脚本预期**零差异**通过——这是「未破坏路由」的正面证据 |
| V6 | `git diff .claude/scripts/ .claude/agents/` | **零改动**（证明 T1〔预防声明〕确已移出、未夹带） |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 2 路径） |

## 6. 留根清单

**① T1〔高风险合法动作预防声明〕——移出本批、退回 ARQ 队列，需重新设计**（human 2026-09-19 裁定；R1-F1 实测触发）

这是本批 R1 的实质产出，须完整交接，否则下一批会重复本批的错误设计：

- **原改法打错机制（R1-F1，BLOCKING、判官标「升级 human」）**：v0.2 的 T1 设计为「派发时在 dispatch prompt 里固定带两句声明」——**该机制假设误伤发生在「派发给子 agent」的场景**。R1 核了三例误伤的**运行者身份**：

  | 案例 | 实际运行者 | 派发声明机制够得到吗 |
  |---|---|---|
  | `260829-char-stroke` 变异探针 | **planner 主会话直跑**（pod-events `human-decision` 事件 verbatim：`"decision": "approve-mutation-probe-workflow"` / summary「human 批准 **planner 直跑**变异探针循环」） | ❌ **该拦截点无 dispatch prompt 存在** |
  | `260829-story-p3` mutation 取证 | **主线程直跑**（pod-retro 人工纠偏节「主线程能不能直接改实现跑变异（你放行）」） | ❌ 同上 |
  | `260907-g-relay-fix` ECS 部署 | 子 agent（派发） | ✅ 有实证（第二轮带上声明省了一轮硬碰） |

  **三例里两例够不到，且恰是最疼的两例。** 另：§1 自引的 char-stroke retro 模式「**派发/执行前**先落 `human-decision` 事件背书」**本就含「执行前」臂**，而 v0.2 的 T1 只承接了「派发」那一臂。
- **F1 的修法方向（交 review，不自批）**：条款补**第三句**覆盖**自执行姿势**——主会话直跑前先落一条**既有的**事件型 `human-decision` 背书（先例 = `260829-char-stroke` 的 `approve-mutation-probe-workflow`），**零机制扩张、零闭集扩张**。即由「派发臂」扩为「派发臂 + 自执行臂」双臂。
- **F2（生产者可达性断链）与 human 的裁决——携带臂**：v0.2 引 `pod-coordination.md:196` 作「同一层」先例，但该先例实为**双臂结构**——**口径臂**写在 `pod-coordination.md`（说明「这行必须带」）、**携带臂**写在 `.claude/agents/planner.md` 的派发步（告诉 planner 派发时要带上）；v0.2 的 T1 **只落了口径臂**，`planner.md` 派发步无任何指针指向新条款，采纳纯靠自发翻阅。**human 2026-09-19 裁决：补上携带臂**（即下一批在 `planner.md` 派发步加一行指针，触 agent def 正文一行——按本族 §0 口径该项单列申报）。**下一批须把两臂一起落，不得只落口径臂。**
- **另两处 R1 计数更正（随 T1 移出，供下一批起草时直接采用）**：① 「四类已实证误伤」实为 **3 例实证 + 1 类 ARQ trigger 行泛化**（「部署动作」类无独立实证）；② §1 引 `pod-coordination.md:196` 的「同层」表述失真，应如实写成「该先例为口径臂，其携带臂在 `planner.md`」。
- **队列状态**：`ARQ-260913-auto-mode-classifier-legit-workflow` **仍在册未结**，已在 `docs/ARCH-REVIEW-QUEUE.md` 补注本批 R1 实测（机制打错 + 三例运行者身份 + 双臂结构 + human 的携带臂裁决），供下一批直接引用。

**② skill 新增节与既有三节的关系**——V2 已设「既有三节标题仍在」的防覆盖断言；若落地时发现新节与该 skill 的既有 `## Rules that cost time when forgotten` 节语义重叠，就地合并而非并列（同义内容不双写），并如实记入 landing summary。

**③ 真机轮 checklist 载体的缺位**——ARQ 原文建议「真机轮 checklist 增两条」。本批实测全仓**不存在独立的真机轮 checklist 文件**（`grep -rln "真机轮" .claude/` 命中 `planner.md` / `pod-coordination.md` / `task-intake-and-slicing.md` / `loop-retro/ledger.jsonl`，无 checklist 载体），故按 human 不可达载体处理、统一并入 skill 新节。**若将来另立 checklist 载体，需从 skill 回链**。

**④ 拆包族另三批**（`-merge-chain-friction` 已 FROZEN v1.0 / `-closeout-chain-friction` 已 FROZEN v1.0 / `-batch-drafting-friction`）——本批不承接。

**⑤ 移出本族的两个 launch 面 ARQ**（输入清单先例取旧、launch 前置清单不核）——留 ARQ 队列后续单独处理。

**交日落复盘项**：本批 T1③ 的指针为「次要发现路径」——建议 `/loop-retro` 观察：真机轮场景下是否真由 skill description 触发（即主通道成立）、以及收口/对账读者是否真用到该指针。若指针从未被用到，下一批可评估撤除。

**自律件**：起草期已按硬项 1a 亲核；v0.5 的 T1 移出为 human 裁定 + R1 实测双重驱动，非主线程自裁；R1 记录（§7）保持 append-only 原样；F3/F4 两处针对幸存 T 的 finding 已在 v0.5 收口。


## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-19 - architect

**verdict：BLOCK(F1, F2, F3, F4)**。F1 为 BLOCKING 且标「升级 human」（影响目标效果级：T1 对其三例自证证据中的一例拦截点不可达）；F2 SHOULD；F3/F4 NIT。T2 覆盖面判定成立（F3/F4 均为措辞级，不动改法方向）。

#### F1（BLOCKING · 升级 human）· T1 的 dispatch prompt 机制到不了「主会话自执行」这一实证拦截点，且未承接 §1 自引的「执行前」臂

**证据（亲核）**：

- `delivery/changes/dev/260829-char-stroke/pod-events.jsonl:7` human-decision 事件 verbatim：`"summary":"human 批准 planner 直跑变异探针循环（mutation-check 闸 8 功能 id 证据）：变异→WSL 重建→host ELF 齿红确认→还原 sha256 校验；auto-mode 分类器曾误拦，human 逐条放行/放行工作流"`——char-stroke 的变异探针是 **planner 主会话直跑**被拦（「human 逐条放行」），该拦截点无 dispatch prompt 存在。
- `delivery/changes/dev/260829-story-p3/pod-retro.md:23`（人工纠偏节）verbatim：「只有两处真停下来问了你：**① 主线程能不能直接改实现跑变异（你放行）**」——story-p3 的最终执行姿势同样是主线程直跑；其卡点节 `:13` verbatim「mutation 那步被 auto-mode 分类器拦了，判我「不可逆破坏 builder 产物」」。
- 三例中只有 g-relay-fix 是子会话（builder）被拦，且其第二轮的 dispatch prompt 预防声明实证有效（`delivery/changes/dev/260907-g-relay-fix/pod-retro.md:11` verbatim「第二轮直接在派发 prompt 里预防性声明『被拦就停、主线程代执行』，省了硬碰」）——T1 的机制对**子会话子类**成立、有实证。
- 本 PROPOSAL §1 自己引用的 char-stroke retro 模式（`delivery/changes/dev/260829-char-stroke/pod-retro.md:11` verbatim）是「凡『看起来像攻击』的验证动作，**派发/执行前**先落 human-decision 事件背书」——含**执行前**臂；T1 两句只承接了派发臂，未承接执行前落 human-decision 事件背书这一 retro 已总结、且 char-stroke 已实操（`approve-mutation-probe-workflow` 事件在册）的形态。

**判读**：本批同族 P1/P3 各有一个 T 因「改法覆盖不了目标面」被证伪移出；T1 对「派发侧携带」子类覆盖成立，但对「主会话自执行」子类（char-stroke 实证形态；story-p3 终态形态；且变异类动作的既有处置惯例本就因子会话写权限被拦而把执行推向主会话——T1 第二句「主线程代执行」恰是该推移的产物）不可达。这与 ARQ 建议①原文一致（T1 忠实承载 ARQ 处方，处方本身漏了执行侧臂），但判官按透镜「该 T 的改法是否真能覆盖它的目标面」须指出：三例自证证据中至少一例不被本改法覆盖。

**建议修法方向（不换落点、零门改动、不触 OQ1 裁决面）**：条款补第三句（或并入判据句）覆盖自执行姿势——主会话/无 dispatch prompt 的执行者跑清单内高风险合法动作前，**预落既有事件型 `human-decision` 背书**（先例：`approve-mutation-probe-workflow`，eventType 零闭集扩张），声明内容同两句（合法性与还原路径 + 兜底句）。是否采纳此扩臂或显式缩圈 T1 目标面为「派发侧子类」= 方向取舍，**升级 human 裁**。

#### F2（SHOULD）· T1 生产者可达性断链：所引 `:196` 先例实为双臂结构，T1 只落了口径臂

**证据（亲核）**：`.claude/references/pod-coordination.md:196` verbatim「义务的生产者可达性由 **planner Step 11 派 integrator 的 dispatch prompt 显式携带**（与锚 2 brief 携带效果段同构，派发侧传递；integrator.md 不因此改）」；`.claude/agents/planner.md:78`（Step 11）verbatim「workstream 包的 dispatch prompt 显式携带『review-packet 必含 demo 对账行』……口径见 `pod-coordination.md` §11」。即先例的**携带臂活在 planner.md（agent def）**，pod-coordination.md 只承载口径。T1 按 OQ1(a) 只落口径臂；而 planner.md 全文对 pod-coordination.md 的引用仅 `:78`（§11 口径）与 `:119`（合并串行化），其派发步 Step 7 明文「派发 prompt 只引用两份 brief 的文件路径……不转抄清单正文」、Step 9「只引用 Step 7 已生成的 input-brief-builder.md 路径」——**派发期没有任何指针把新条款带给派发作者**。条款被采纳全靠派发作者自发翻阅 pod-coordination.md，无承载通道。

**判读**：§6 留根 2 已诚实登记「实际采用率未观测 + 观察计划」，风险本身是知情的；但 §1 的先例引用「派发契约的既有承载点就在这里（`:196` 同族先例）」掩盖了先例的携带臂在 agent def 面——先例之所以生效，正因 planner.md Step 11 带了那一臂。若主线程沿用此引用为 T1 的可达性论据，属引用失真。

**建议修法方向**：① 落地措辞如实改述先例结构（「同族先例」→「同族口径承载点；其携带臂在 planner.md Step 11，本批按 OQ1(a) 不建携带臂，可达性以留根 2 观察计划承载」）；② 若 human 欲保证携带臂，须显式批一条 planner.md 派发步指针行（触 OQ1(b) 邻面）——建议随 F1 一并裁，不单独升级。

#### F3（NIT）· T2 指针行的读者面错配

**证据（亲核）**：pod-coordination.md 全文「真机轮」仅出现在 `:196` demo 对账行段（closeout/对账语境，读者 = planner Step 11 / 收口侧）；真机轮串口捕获的**执行**发生在 verifier/integrator 执行期或主会话，其触达通道是 skill 的 description（`.claude/skills/kidpal-serial-monitor/SKILL.md:3`「Use whenever reading device boot logs, watching a smoke round, or debugging on-device behavior」——真机轮必含串口读，触发成立，T2 主通道覆盖成立）。指针行声称目的「防真机轮派发侧想不到翻该 skill」依赖派发侧读 pod-coordination.md，无证据支撑。

**建议修法方向**：措辞如实化为「辅助指针（收口/对账侧可达；主通道 = skill description 触发）」，或按 OQ1 裁决维持位置、仅修正目的表述。

#### F4（NIT）· 计数与清单小失真

- T2①「21:15 / 22:50 **两度**实证」 vs §1 / ARQ / `delivery/changes/dev/260911-screen-unification/pod-retro.md:34` verbatim「当晚**三次**设备重启全部与捕获结束时刻吻合（21:15 / 22:50 秒级）」——三次重启、两个时刻，PROPOSAL 内部不一致。
- §1/§2 称「四类已实证误伤」；实测误伤记录为 **3 例**（char-stroke 探针 / story-p3 取证 / g-relay-fix ECS），「部署动作」第四类来自 ARQ trigger 行（「下个含变异探针 / ECS 操作 / 部署动作的包 launch 前」）对 g-relay-fix 形态的泛化，非独立实证；§3 硬项 1b 括号「（变异探针 / ECS 部署 / mutation 取证 + 部署动作）」中「部署」出现两次。OQ2 已裁开放举例，影响小，但应如实写「三类实证 + 一类泛化示例」。

#### 亲核留痕（§1 事实断言逐项）

- `wc -l .claude/skills/kidpal-serial-monitor/SKILL.md` = 31 ✓；三节标题 `:8`（Tools）/`:19`（Rules that cost time when forgotten）/`:24`（Reading a smoke round）逐行核对 ✓；DTR/RTS 全文仅 `:10` 一次且为工具说明形态（未记录伪影模式）✓。
- `grep -rn "预防声明" .claude/` 零命中 ✓；「单一长捕获 / 就绪信号开窗 / 实时口令」零命中 ✓；`grep -rln "真机轮" .claude/` = planner.md / pod-coordination.md / task-intake-and-slicing.md / loop-retro/ledger.jsonl（与 §6 留根 3 一致，确无 checklist 载体）✓；`grep -c "kidpal-serial-monitor" .claude/references/pod-coordination.md` = 0 ✓（V4 基线成立）。
- 三例误伤证据逐条 verbatim 吻合：char-stroke pod-retro 卡点 1（`:11`）✓、story-p3 卡点（`:13`）+ 人工纠偏节（`:23`）✓、g-relay-fix 卡点 1（`:11`）✓；ARQ 两行原文（`docs/ARCH-REVIEW-QUEUE.md:57-58`）与 §1/§2 引述吻合 ✓。
- 4 包真机轮实证逐条吻合：key-route-a1 卡点 3（pod-retro `:30-34` verbatim「第一次 300s 窗口开早了……第二次窗口 human 主动喊开始，一次过」）✓、screen-unification 追记（`:33-34`）✓、hardware-validation「烧录 2min + 复烧验证 + human 目视……成为主循环周期」（`:31`）✓、render-pages「boot 死锁……烧录后才现形」「开机冒烟……已立为必经步」（`:34`）✓。三条姿势确属「各包现撞现总结」（key-route-a1 事前未知、screen-unification 当夜事后钉死），「知道了但没地方写」判定成立，T2 覆盖面成立；skill 新节与既有三节互补不冲突（「COM5 独占」↔「绝不关端口」）。
- 260915 批口径引用属实（`260915-gov-review-worktree-pool-reuse/PROPOSAL.md` T2⑤「环境事实归 skills、基线归 progress.md、留根归 pod-retro」在册）✓。
- **结构字段核对**：landing-manifest 围栏块在场（2 路径）✓；packet-id 行在场（头注「packet-binding（FROZEN 生效）」行）✓；manifest ≡ §2 各 T 目标文件并集 {pod-coordination.md, SKILL.md} ✓。
- **边界核对**：riskTier=low / profile=G2 成立（两落点均 `.claude/references` + `.claude/skills`，零 G1/G3/契约/frontmatter，零闸零脚本）✓；Track B 默认路由成立（两条升级判据均未命中）✓；OQ1/OQ2 裁决未重开（本评审仅在裁决框架内指出可达性与覆盖面后果，落点维持参考文档面）✓。
- 验证集 V1–V6 逐条机械可判、锚点非空转 ✓（V1 依赖落地文本含「预防声明」字面——§2 草案条款正文现未含该词，落地时须按 V1 锚点写入）。

**结论**：BLOCK(F1, F2, F3, F4)。F1 升级 human（T1 是否补「执行前 human-decision 背书」第三臂，或显式缩圈 T1 目标面为派发侧子类）；F2 建议 F1 裁决时一并定携带臂取舍；F3/F4 措辞级，主线程修订时顺手收。T2 除 NIT 外无实质问题。