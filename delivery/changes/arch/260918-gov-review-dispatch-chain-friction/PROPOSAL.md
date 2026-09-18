# PROPOSAL · 260918-gov-review-dispatch-chain-friction

> **状态：DRAFT v0.1 · 2026-09-18 · 待 human 立项 ack**（FROZEN 行待补凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果）。
> **承载 ARQ**：`ARQ-260913-auto-mode-classifier-legit-workflow`（T1）、`ARQ-260913-device-round-protocol`（T2）。不关的根写 §6 留根指针。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `-merge-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：两 T 均为派发面 / skill 面的**成文与固化**，零闸判定改动、零脚本行为改动、零 agent frontmatter 改动；T2 只往 skill 加一节，无既有消费方被破坏）。
> **review profile**：G2（`.claude/references/pod-coordination.md` + `.claude/skills/kidpal-serial-monitor/SKILL.md`）。契约面 / G1 / G3 均为零。**本批是本拆包族里唯一零闸语义触点的两批之一**（另一批为 `-batch-drafting-friction` 的 T1/T3）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/pod-coordination.md
.claude/skills/kidpal-serial-monitor/SKILL.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把两类**已经反复付过学费、却还没住进耐久的家**的 dev-loop 知识固化下来——高风险合法动作的派发预防声明，与真机轮协调协议三件套。

**现状差距**：这两条都不是「不知道怎么做」，而是「知道了但没地方写」——安全分类器误伤有 3 包实证、真机轮协调有 4 包实证，**全部只活在各自的 pod-retro 里**；下一包的 agent 只能重新踩一遍。本批把这两摊知识回收进它们的自然承载面。

**非目标（本批不做什么）**：不改任何 agent 的 frontmatter（tools / name / model / description 全不动）；不给 auto-mode 分类器加白名单（那动 harness 面、须独立治理批，见 §6）；不改 `kidpal-serial-monitor` skill 的既有三节内容（只新增一节 + 补一行指针）；不动闸、脚本、任何 `*.acceptance.*`。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节断言均为 2026-09-18 在 `gov/kidpal` 工作树亲核所得；skill 行数与结构为现跑 `wc -l` / `grep -n` 实测。

### 问题 1：auto-mode 分类器误伤合法工作流已成成本模式

证据（`delivery/changes/retro/2026-09-13-loop-retro-efficiency.html` 的 3 包 retro 实证）：

1. **char-stroke 变异探针**——`mutation-check` 闸要求「真实变异 → 齿红 → 还原」三步，分类器判成「造假绿」，**方向完全颠倒**；AskUserQuestion 拍板后才继续（该包 pod-retro 卡点 1）。
2. **story-p3 mutation 取证**——被拦「不可逆破坏 builder 产物」。分类器看不到「下一步就 checkout 还原」的意图；拦得其实合理，但成本同付（该包人工纠偏节）。
3. **g-relay-fix builder 的 ECS 部署**——被 Credential Leakage / Data Exfiltration 两形态终身盯防（transcript 出现过内联凭据痕迹之后），处置 = 主线程代执行（该包卡点 1）。

**影响面**：凡「看起来像攻击」的合法验证动作（变异探针 / ECS 操作 / mutation 取证）都会复发；char-stroke retro 已总结成模式——「派发/执行前先落 human-decision 事件背书」。**根因**：分类器判的是**动作形态**，而预防成本最低的补充信息（该动作的合法性与还原路径）**现无处固定承载**——每次靠现场解释、每次都要重新说服。

**承载面实测（亲核）**：`grep -rn "预防声明" .claude/references/pod-coordination.md .claude/agents/planner.md` **零命中**——派发面现无此类常设条款。而派发契约的既有承载点就在这里（同文件 `:196` 的「demo 对账行」条款即「义务的生产者可达性由 planner Step 11 派 integrator 的 dispatch prompt 显式携带」的同族形态）。

### 问题 2：真机轮协调协议三件套未固化，每次真机轮重复交学费

证据（同 retro，4 包实证）：

1. **key-route-a1**——第一次 300s 捕获窗口**开早了**（checklist 还在读），human 操作落在窗口外、口述与日志互证不上，按「completion 不凭解释宣布」重捕一轮（该包卡点 3）。
2. **screen-unification**——合并夜三次设备重启与串口捕获 **DTR/RTS 关端口复位伪影**秒级吻合（21:15 / 22:50 实测）；同夜提前 15 分钟分发的提醒时间表落空（fire 时刻设备离线、human 看不到 agent 发的表格），改实时口令「现在按 BOOT！」一次成功（该包追记）。
3. **hardware-validation / render-pages**——同族（烧录-复烧-目视等待是主循环周期；render-pages 的 boot 死锁烧录后才现形，开机冒烟已成必经步）。

**承载面实测（亲核）**：`.claude/skills/kidpal-serial-monitor/SKILL.md` 全文 **31 行**（`wc -l` 实测），三节 = `## Tools (already on disk, in c:/tmp)`（`:8`）/ `## Rules that cost time when forgotten`（`:19`）/ `## Reading a smoke round (what "good" looks like)`（`:24`）。**DTR/RTS 只在 `:10` 出现一次**，且是工具说明形态（`**`c:/tmp/serial_capture.py <seconds> <out>`** — **resets the board via DTR/RTS** then reads.`）——**未记录伪影模式**；「单一长捕获」「human 就绪信号开窗」「实时口令钉动作点」三条 **grep 零命中**。

**根因**：该 skill 成文于「被动读日志」年代，真机轮**协调**知识（谁在什么时候开窗、动作点怎么下）散在 4 包 retro 里未回收；而真机轮是含真实硬件、真实人、不可重放的一环，**协调失败的代价最高、却最没有说明书**。

## 2. 改法（T1–T2）

### T1 · 派发契约补「高风险合法动作预防声明」

承载 `ARQ-260913-auto-mode-classifier-legit-workflow`。

改 `.claude/references/pod-coordination.md`：在派发契约面（与 `:196`「demo 对账行」同层，即「义务的生产者可达性由派发侧传递」的那一族条款）新增一条**常设条款**——凡派发/执行清单内的高风险合法动作（**变异探针 / ECS 操作 / 部署动作 / mutation 取证**，即分类器已实证误伤的四类），dispatch prompt **固定携带两句**：

1. **合法性与还原路径声明**——做什么、为什么合法、如何还原（如变异探针的「真实变异 → 齿红 → `checkout` 还原」三步原样写进 prompt）；
2. **被拦就停、主线程代执行**——兜底句，承 g-relay-fix 第二轮已实证的省一轮硬碰形态（该包第二轮据此省掉一轮）。

为什么这样改：直击问题 1 的根因——分类器判**动作形态**，而预防成本最低的补充信息无处固定承载，每次靠现场解释；把它钉成派发契约的常设一句，**成本一次付、收益每包复得**。

**选 `pod-coordination.md` 而非 agent def 的理由**（见 OQ1）：该文件的派发契约既有承载点就在这里（`:196` 同族先例），且**避开 agent def 的 self-def 判断点**——本批 §0 已声明不动任何 frontmatter，正文条款亦不动，从面上去掉该争议。

**不做** ARQ 候选②（给分类器加白名单）：那动 harness 面、须独立治理批，本登记不预批（§6 留根）。

目标文件：`.claude/references/pod-coordination.md`（改）

### T2 · `kidpal-serial-monitor` skill 固化真机轮协调协议

承载 `ARQ-260913-device-round-protocol`。

改 `.claude/skills/kidpal-serial-monitor/SKILL.md`：

① **新增一节** `## Device round protocol / 真机轮协调协议`，三条：

- **单一长捕获**——一次开窗覆盖全窗口、中途**绝不关端口**；附 DTR/RTS 关端口复位伪影的**模式识别**（设备重启时刻与捕获启停时刻秒级吻合即为伪影，**非真重启**——screen-unification 合并夜 21:15 / 22:50 两度实证）；
- **human 就绪信号开窗**——捕获窗口**不由 agent 单方面开播**，由 human 就绪信号开启（key-route-a1 窗口开早、操作落在窗外、按「completion 不凭解释宣布」重捕一轮的实证）；
- **实时口令钉动作点**——动作点**不提前分发时间表**，改实时口令（screen-unification 提前 15 分钟分发、fire 时刻设备离线落空的实证）。

② 在既有 `## Rules that cost time when forgotten` 节（`:19`）补一行指向新节。

同批在 `.claude/references/pod-coordination.md` 的真机轮相关处补**一行引用指针**（指向该 skill 新节，**不转抄正文**——防双源漂移，沿本仓「引用不复制」惯例，与 `:196` 的 brief 引用形态同构）。

为什么这样改：直击问题 2 的根因——三条零承载的知识散在 4 包 retro 里，skill 是它们的耐久家；同吃 260915 批「环境事实归 skills、prompt 只留指针」的口径（该批已把小智环境事实全部归入 skills 并收缩 launch prompt）。

目标文件：`.claude/skills/kidpal-serial-monitor/SKILL.md`（改）、`.claude/references/pod-coordination.md`（改：引用指针一行）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 派发契约 | `.claude/references/pod-coordination.md` | G2 | T1 新常设条款；T2② 引用指针一行。消费方 = planner 各 Step 的 dispatch prompt 撰写 |
| 串口 skill | `.claude/skills/kidpal-serial-monitor/SKILL.md` | G2 | T2① 新增一节。消费方 = 真机轮派发；无机械消费方（skill 由模型按 description 触发） |
| 契约面 / G1 / G3 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 本批纯成文 |

**【硬项 1b · sweep 透镜】** 本批**不引入新门、不引入新判定 pattern、不引入新枚举面**，故无预跑与 census 义务。**但有一项等价义务已履行**：T1 的「四类高风险动作」清单不是凭空列举，而是从 3 包 retro 的**实测误伤记录**中枚举得出（变异探针 / ECS 部署 / mutation 取证 + 部署动作），已逐类注明出处。

**【硬项 1c · 复扫模式完备性先验】** 本批不涉退役语面 / 残留清扫，无清扫模式集。

## 4. human 取舍点（OQ）

### OQ1 · T1 与 T2 的落点

两 T 各有候选承载面，影响面不同：

- **T1**：(a) `.claude/references/pod-coordination.md`（**推荐**——派发契约既有承载点 `:196` 同族先例在此；避开 agent def self-def 判断点）vs (b) `.claude/agents/planner.md` 正文（离派发动作更近，但触 self-def 判断点，会把本批拖进该争议）。
- **T2**：(a) `.claude/skills/kidpal-serial-monitor/SKILL.md` + `pod-coordination.md` 一行引用指针（**推荐**——同吃 260915 批「环境事实归 skills」口径；指针防真机轮派发侧想不到翻该 skill）vs (b) 只落 skill、不加引用指针（更克制，但可达性下降）。

**这两项为什么列 OQ**：落点选择改变的是**挂载面**（reference 面 vs agent def 面），而 agent def 面的改动在本仓有独立的 self-def 约束；虽然本批推荐位全在 reference 面，判官或 human 若认为应落 agent def，属改法方向变更，该由 human 定。

### OQ2 · T1 的「四类动作」清单是否封闭

推荐**开放列举 + 判据句**（写「凡形态上可能被安全分类器误判为攻击的合法验证动作」，后附四类**示例**而非穷举），而非封闭四类。理由：分类器的误伤面会随动作类型增长（本批四类来自 3 包实证，非穷举），封闭清单会让第 5 类无据可依。备选：封闭四类（边界更清晰、更好判官核，但需人工维护）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

本批为纯成文批，验证集以「在场性 + 零回退」为主。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `grep -c "预防声明" .claude/references/pod-coordination.md` | ≥1（原为 0）；且条款内**逐字含**四类动作名与控制句 |
| V2 | `grep -n "Device round protocol" .claude/skills/kidpal-serial-monitor/SKILL.md` | 命中新节标题；且三小条（单一长捕获 / 就绪信号开窗 / 实时口令）各 `grep -c` ≥1 |
| V3 | `wc -l .claude/skills/kidpal-serial-monitor/SKILL.md` | 由 31 行增至新值；**既有三节标题仍在**（`grep -c` 各 1，防重写覆盖） |
| V4 | `grep -c "kidpal-serial-monitor" .claude/references/pod-coordination.md` | ≥1（T2② 指针在场） |
| V5 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件）。**注**：本批不动 frontmatter，两个脚本预期**零差异**通过——这是「未破坏路由」的正面证据 |
| V6 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 2 路径） |

## 6. 留根清单

**不关的根**：

1. **ARQ 候选②（给 auto-mode 分类器加白名单）**——那动 harness 面（分类器判据本身），本登记不预批，须独立治理批。本批 T1 只做「预防声明」这一半（低成本、零权限面变更）。
2. **T1 条款的实际采用率未观测**——条款落地后是否真被派发方逐次采用，本批无法验证（无机械消费方）。**建议下个含变异探针 / ECS 动作的包 launch 时观察**；若仍出现「派发未带声明即被拦」，则说明成文不足、需考虑 OQ1(b) 的 agent def 落点或机械注入。
3. **串口 skill 的「真机轮 checklist」形态**——ARQ 原文建议「真机轮 checklist 增两条」。本批实测全仓**不存在独立的真机轮 checklist 文件**（`grep -rln "真机轮" .claude/` 命中 `planner.md` / `pod-coordination.md` / `task-intake-and-slicing.md` / `loop-retro/ledger.jsonl`，无 checklist 载体），故按 human 不可达载体处理，统一并入 skill 新节。**若将来另立 checklist 载体，需从 skill 回链**。
4. **拆包族另三批**（`-merge-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`）——本批不承接。
5. **移出本族的两个 launch 面 ARQ**（输入清单先例取旧、launch 前置清单不核）——留 ARQ 队列后续单独处理。

**交日落复盘项**：T1/T2 均为「回收 retro 知识」形态，建议 `/loop-retro` 观察是否还有**同类未回收的跨包共性**（判据：同一类事故在 ≥3 包 retro 里独立出现过、且无 durable 家）。

**自律件**：起草期已亲核（skill 行数与节点行为 `wc -l` / `grep -n` 实测；「四类动作」来自 retro 实测记录而非枚举想象）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）