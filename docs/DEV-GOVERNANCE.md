# 开发域治理（DEV-GOVERNANCE）：设计思想与实现

> **迁移注记（2026-08-27 档 1；2026-08-29 档 2/3）**：本文件自 `c:\AI\AInativecompany` 迁移，按 KidPal 实况适配——契约面 = `docs/contracts/**`（单文件 markdown 协议）。档 1（spec 治理：arch-review 对抗链 + 契约门 + Track B）2026-08-27 迁入；档 2（dev-loop 全链 + 测试闸）与档 3（portfolio 编排）2026-08-29 迁入（迁移批 `delivery/changes/arch/260829-gov-review-migration-tier23`，栈绑定闸按本仓双实现面重写）。结构参考 = `.claude/rules/repo-structure.net-template.md`（原仓模板，含本仓未建面）。流程正文在各 operational 文档；契约 SSOT 永远是 [docs/contracts/**](contracts/)；术语表暂缺，建立时补。
> 本文件讲清开发域治理**按什么思想设计、如何在本仓实现**。
> **怎么读本文**：想了解 → 上篇 §1–§4；想改东西 → §7 速查；想改治理本身 → §6 → §4 治理变更 → §5。
> **贯穿铁律**：审批在前、改动在后（前置门）；证据入库，聊天结论不结门；冲突按 authority ladder **停下路由**、不静默选择——`docs/contracts/**` > canonical spec > 治理规则 > gate evidence > queues > `delivery/changes/` > transcripts。

> 2026-08-02：治理路由永久化为「**Track B 常驻默认 + Track A 判据升级**」（总声明见 `AGENTS.md`「治理路由」节；展开 SSOT = [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」节；立法来源 `delivery/changes/arch/260802-gov-review-track-b-permanentize/PROPOSAL.md`；2026-08-01 临时路由安排同日退役）。

# 上篇 · 思想与实现

## 1. 定位与设计思想

### 1.1 全景定位

本代码库开发 KidPal（小伴）——ESP32-S3 儿童学习伙伴。原仓的两族治理中，KidPal 当前只有 **开发活动的治理**（Part 2：本文件，工程门）；「运行系统的治理」（蓝图 / kernel spec / 6 业务门）未建，将来若产品复杂到需要再立。迁移注记见文首。

**开发域治理（Part 2）= 把业务运行时（Part 1）开发出来的 bootstrap。** 它不是与业务并列的第二套产品。它用当前的 agent / files / hooks / review loop，以同构原则（事件驱动 / 明确 owner / 少数人工门 / 机器验证 / 证据闭环）先行自治目标系统，开发 Part 1。

> 原仓（AInativecompany）的 `.delivery`/`.planning` dogfood 实例不适用本仓、未迁——那是原仓目录名，与本仓 `delivery/**`（2026-09-15 起，260914-gov-review-delivery-restructure）无关。

### 1.2 五条设计思想

**① agent = 敏捷团队里的人。** 每个 agent 不是一次工具调用，而是一个有 JD 的团队成员：职责、边界、write scope 写进它的定义文件（`.claude/agents/**`），协作方式照抄敏捷团队——RTE 分包、SM 带 Pod、开发测试成对、评审会红队对抗、retro 引导改进（§2）。

**② 检查分三族，各归其位。** 机械确定的检查**写进制度**（rule / 脚本 / hook），不占人——人盯确定性事务必然疲劳漏检，制度不会；需要判断的检查交给 **agent 角色**（评审、对抗、裁断）；真正需要负责的拍板留给 **human**（少数人工门，§5）。

**③ 作者 ≠ 判官。** 任何产出物的判定者必须独立于作者，全程无例外：spec 由不加载作者身份的 review 会话判，代码由独立 verifier 判，governance landing 由 landing-verifier 判，review 深度由 review-moderator 复判，路由 lane 归属与路由权整体亦然（作者不得自路由——别于本句已列的 review *深度*复判；见 [arch-review-protocol「路由权归属」](../.claude/references/arch-review-protocol.md)）。

**④ 审批在前、改动在后；证据闭环。** 所有前置门先批后改；结论不落库等于没发生——评审证据进 `delivery/changes/arch/**`，closeout 证据进 `delivery/changes/dev/**`，未结项进队列（ARQ / CDQ）。

**⑤ 治理递归：治理系统自身也被同一套规则治理。** 规则、角色、制度文件的变更同样走「起草 → 对抗 review → human 批准 → 独立核验」全流程，且 human 恒在顶端（§6）。

> ③④ 是横切原则，不单独设节：③ 落在 §4 各流程的每个判定点（adjudicator 独立裁、verifier 独立验、landing-verifier 核验 landing、review-moderator 复判 review 深度）；④ 落在文首铁律、§4 各流程的证据栏与 §5 门判据。

## 2. 团队构图：敏捷角色 → agent

思想①的落地：我们把一个敏捷团队（SAFe 两层）整编制搬进仓里。

| 敏捷团队角色 | 本仓实现 | 干什么 |
|---|---|---|
| RTE（Release Train Engineer） | `portfolio-planner` | 把里程碑 / 倡议拆成排好序、带依赖的 TaskPackage；跨 Pod 调度（WIP limit / merge order / merge token） |
| Scrum Master | `planner` | 单 Pod 循环编排：验收 → 开发 → 测试↔修复 → 集成 → retro → 合并门 |
| AC 撰写（PO 侧） | `acceptance-author` | 把需求写成红基线验收测试，覆盖每条 sourceRef |
| 开发 | `builder` / `fixer` | 实现 / 修红 |
| 测试 / QA | `verifier` | 确定性核验，退出码说话 |
| 集成 / 发布 | `integrator` / merge executor（planner 主线程）| docs-sync + 全量 e2e + closeout；合并走本地可回滚路径（`--no-ff`）|
| 合并把关（release gate） | `supervisor` | 证据可信 + 风险分级放行；高危升级 human |
| 设计评审会 | `scenario-author` / `challenger` / `defender` / `adjudicator` / `review-moderator` | 冻判据 → 红队挑战 → 答辩 → 裁决；主持编排 |
| 首席架构师 | `architect` | 起草 spec / 治理候选；开发治理 owner |
| 架构评审 | `architect-reviewer` | 契约四分类 + review-depth 裁断 |
| landing 独立核验 | `landing-verifier` | 治理 canonical landing 保真 == frozen PROPOSAL |
| retro 引导（跨团队） | `/loop-retro` | 周期性跨 Pod 提共性，治理类进 ARQ |
| 设计师 | `uiux-designer` | 系统级 UI brief 候选 |
| UI 翻译（TaskPackage 级） | `uiux-author` | 把冻结的系统 UI brief 翻译成 TaskPackage `uiux` block |
| 最终负责人 | human | 契约门、高影响拍板、治理批准 |

一个 Pod = 一个 git worktree 里的一支敏捷小队：多 Pod 并行开发、**串行合并**（merge token + 进门前 rebase 到当前 main），谁都不并行直落 main（机制见 [pod-coordination.md](../.claude/references/pod-coordination.md)）。

现行编制注：`supervisor` / `verifier` 不设独立 agent 定义（定义已于 2026-07-25 退役），以 **stage 身份**存续，由 planner Bash 直跑机械闸承载（`verify.mjs` round 证据 + `verifier-evidence-check.mjs` + `supervisor-verdict-gate.mjs`，**不派 agent**）。

## 3. 制度构图：机器强制层

思想②的制度族：凡是机械可判的检查，我们**不写成 agent，写成制度**——脚本、hook、配置。制度不疲劳、不看情面、每次都执行。

- **承重 = 机器硬拦，违反即拦**：契约面冻结（review-loop 整毯冻结 `docs/contracts/**`）、docs 顶层白名单、影子目录禁令、直推主干拦截、逐 agent 写边界、收口门、verify-gates 脚本链。
- **灵活 = 判断 + 就近引导**：apps 内部实现、Tier-2 私有模型、文档内容本身。

强制点清单的 SSOT 是 [scope-map.json](../.claude/scope-map.json) 与 [repo-structure.md](../.claude/rules/repo-structure.md)；agent / 制度文件的撰写纪律见 [authoring-agents-skills.md](../.claude/rules/authoring-agents-skills.md)、[prompt-authoring.md](../.claude/rules/prompt-authoring.md)。

## 4. 工作方式：四条流程

团队的日常组织成四条流程，各治理一类对象；真实变更常串起多条（串联示例见 §7）。

### 契约门 ≈ 架构契约评审

```mermaid
flowchart TD
    T["触发：疑似触 docs/contracts/**"] --> Q{"architect-reviewer 四分类"}
    Q -->|"NO_CONTRACT_CHANGE<br/>现有契约足够 / Tier-2 系统自治"| NOCHG["不改契约<br/>Tier-2 → 免门随 Dev Loop OS"]
    Q -->|"IMPLEMENTATION_ADAPTER<br/>实现适配器,不动契约"| ADAPT["走实现侧,不进契约"]
    Q -->|"SPEC_CONTRACT_DRIFT<br/>spec 与契约漂移"| DRIFT["停在人类契约门<br/>spec 改动暂存 packet"]
    Q -->|"CONTRACT_CHANGE_REQUIRED<br/>确需改契约"| PKT["产 change packet + gate evidence"]
    DRIFT --> HUMAN
    PKT --> HUMAN{"人类契约门签字"}
    HUMAN -->|"拒"| ROLLBACK["追认或回滚<br/>(禁 PENDING 与已上线并存)"]
    HUMAN -->|"签"| ATOMIC["与触契约的 spec 改动<br/>同一 PR 原子落地"]
    ATOMIC --> DONE["契约已批 → spec 方可冻结"]
    HUMAN -.->|"铁律:契约未批"| FREEZE["spec 不可冻结"]
```

- **对象 / 触发**：`docs/contracts/**`（core schema / event / tool / shared type / invariant）。
- **怎么做**：`architect-reviewer` 四分类 → 需改则产 change packet + gate evidence → **人类契约门签字** → 与触契约的 spec 改动**同一 PR 原子落地**。Tier-2（系统私有、无跨系统消费者，三条件全中）免门、随 Dev Loop OS。
- **铁律**：契约未批 → spec 不可冻结；禁「PENDING 与已上线并存」（追认或回滚）。
- **证据**：owning change folder + [CONTRACT-DECISION-QUEUE.md](CONTRACT-DECISION-QUEUE.md)。详见 [contracts.md](../.claude/rules/contracts.md) · [contract-change-evidence.md](../.claude/references/contract-change-evidence.md)。

### 对抗 review ≈ 设计评审 · 红队

```mermaid
flowchart TD
    T["对象：docs/kernel · docs/systems<br/>(意图/行为/拓扑/边界/阶段)"] --> IN["/arch-change 统一入口分类"]
    IN --> DRAFT["architect 起草候选"]
    DRAFT --> SA["scenario-author 冻判据"]
    SA --> CD["challenger ↔ defender 对抗"]
    CD --> ADJ["adjudicator 裁决"]
    ADJ --> ORACLE{"过 pass-oracle?"}
    ORACLE -->|"未过"| CD
    ORACLE -->|"过"| ROUTE{"落地分流"}
    ROUTE -->|"低影响"| MOD["moderator 落"]
    ROUTE -->|"触契约"| CGATE["转 ① 契约门"]
    ROUTE -->|"需实现"| DLOOP["进 ③ Dev Loop OS"]
    ROUTE -->|"高影响 / deadlock"| HUMAN["human 拍板"]
    ROUTE -.->|"L0 蓝图特例"| L0["备改法交 human<br/>不自动落地"]
```

- **对象 / 触发**：`docs/kernel/**`、`docs/systems/**` 的意图 / 行为 / 拓扑 / 边界 / 阶段。
- **怎么做**：`/arch-change` 统一入口分类 → architect 起草候选 → 评审会（scenario-author 冻判据 → challenger ↔ defender 对抗 → adjudicator 裁决）→ 过 pass-oracle → 落地分流：低影响 moderator 落 / 触契约转契约门 / 需实现进 Dev Loop OS / 高影响 · deadlock 由 human 拍板。L0 蓝图特例：备改法交 human，不自动落地。
- **证据**：`delivery/changes/arch/arch-review-<spec>/` + [ARCH-REVIEW-QUEUE.md](ARCH-REVIEW-QUEUE.md)。详见 [arch-review-protocol.md](../.claude/references/arch-review-protocol.md) · [arch-change.md](../.claude/commands/arch-change.md)。

### Dev Loop OS ≈ sprint 循环

```mermaid
flowchart LR
    PP["① portfolio-planner · 拆包+调度 (RTE)"]
    PL["② planner Pod ×N · acceptance→builder→verifier↔fixer→integrator→planner 收尾(pod-retro)→supervisor"]
    LR["③ loop-retro · 跨 Pod 提共性"]
    ARQ["architect-owned ARQ → 治理变更"]
    PP -->|launch| PL
    PL -->|retro evidence| LR
    LR -->|治理发现| ARQ
    ARQ -.->|改进定义| PP
```

- **对象 / 触发**：自有实现面（固件业务 + 云端 shim，动工前由 human 登记进 scope-map）实现（feature / bugfix / 重构 / 测试修复 / 实现侧 docs-sync）。
- **怎么做**：⓪ 开工前 `/triage-change` 比例化分档（拆包 / Full / Lite / 直接修）→ ① RTE 拆包调度 → ② planner 带单 Pod：acceptance-author（红基线）→ builder → verifier↔fixer → integrator（docs-sync + 全量 e2e + closeout）→ planner 收尾（pod-retro）→ supervisor 合并门 → planner 前置升级闸放行后本地合并 → ③ `/loop-retro` 周期性跨 Pod 复盘提共性 → 治理类进 ARQ、回流 ① intake，构成**自改进回路**。疑似触契约 → architect-reviewer 兜底。
- **证据**：`delivery/taskpackages/<id>/`（execution ledger）+ `delivery/changes/dev/<id>/`（closeout audit）。详见 [dev-loop-operating-system.md](../.claude/references/dev-loop-operating-system.md) · [dev-loop.md](../.claude/references/dev-loop.md) · [change-triage.md](../.claude/references/change-triage.md)。

### 治理变更 ≈ 流程改进 retrospective

```mermaid
flowchart TD
    SRC{"来源"} -->|"human 直接提出"| D1
    SRC -->|"retro 制度产出<br/>(pod-retro + /loop-retro → ARQ)"| D1
    D1["① 主线程起草 PROPOSAL<br/>→ human 立项 ack"] --> PRE["② architect-reviewer 预审<br/>确认 NO_CONTRACT_CHANGE"]
    PRE -->|"触 docs/contracts/**"| CGATE["转 ① 契约门"]
    PRE -->|"无契约"| REV["③ architect 评审循环至 GREEN<br/>(R1–R3 同判官;3轮未绿→human 三选一)"]
    REV --> SEQ["④ 落地序列:<br/>标 FROZEN → packet-bound landing<br/>→ landing-verifier 核验 → retro(趁热)<br/>→ 主线程自动 merge → upsert"]
    SEQ --> SYNC["⑤ 同步联动全部投影处"]
    D1 -.->|"命中升级判据:<br/>触contracts/跨系统结构性/声明红队"| TA["Track A → /arch-change<br/>(对抗 review)"]
```

- **对象 / 触发**：G1 / G2 / G3 治理面（§6 表）。owner / writer = architect，approver = human。
- **来源**：两类——human 直接提出；或由 retro 制度产出：pod-retro（Pod 内，planner 收尾相位）+ `/loop-retro`（跨 Pod 周期性）提流程债共性 → architect-owned ARQ → 本流程（自改进回路，见 Dev Loop OS ③）。
- **怎么做**（步骤编号为外部引用锚点，勿重排；默认 = Track B 常驻轻路径七步闭环，展开 SSOT = [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」节）：
  1. 主线程在 `delivery/changes/arch/<change>/` 起草 `PROPOSAL.md`（业务语言 + 初步技术方案 + human 取舍点标注；标注轨道自评 + 理由；大改不在 proposal 里直接改 canonical；提案目录命名 = governance-lane `<YYMMDD>-gov-review-<slug>`（spec-lane `<YYMMDD>-arch-review-<slug>`），目录名必须命中 `.claude/scope-map.json` review-agent 写槽 token，否则评审 / 核验期制品写入被 `enforce-scope.mjs` 机械拦截——见 [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」节命名条款）→ **human 立项 ack**。（Track A 触发时：入口 `/arch-change` 自动分类进 governance profile 并派 architect 起草；proposal 已备时 `/review-governance` 直达。）
  2. architect-reviewer 预审 → 确认 `NO_CONTRACT_CHANGE`（触 `docs/contracts/**` → 转契约门）。
  3. **architect 评审循环**（只读 canonical、只写评审记录于 PROPOSAL §7；R1–R3 同判官 resume）至 GREEN；影响目标效果的打回项升级 human；**3 轮未 GREEN → human 三选一**（转 Track A / 撤回·缩圈 / 换新判官实例再战 1 轮——仍不绿自动转 Track A）。（Track A 触发时：**高影响**〔改循环行为 / 门力度 / 角色边界 / agent 定义 / 强制执行层〕→ governance profile 对抗 review，产证据 + decision packet；review **不落 canonical**。）
  4. **落地序列**（状态不可合并：立项 ack ≠ FROZEN ≠ landing 授权）：主线程凭「立项 ack + GREEN」标 **FROZEN**（授权记 Status 行）→ 主线程 packet-bound canonical landing（**G1/G2/G3 全落**，landing-manifest 绑定；**landing 恒 = 主线程**——architect 恒纯判官 / 起草、永不 landing）→ `landing-verifier` 独立核验 → verdict 绿 → 主线程补写 **retro**（闭环件，模板 `docs/templates/retro.template.md`；landing-verifier 绿后趁热写、先于合并，不含合并锚——落点由 `delivery/changes/INDEX.md` `mergedInMain` 机械现推）→ **主线程前置升级闸放行后自动合并本地 main（可回滚）** → 主线程在 draft / FROZEN / landed 三转移点各跑一次 `node .claude/scripts/changes-index.mjs --upsert <id>`，把本批单行状态即时钉进 `delivery/changes/INDEX.md` arch 段（只动一行、幂等；landed 转移点在合并后跑，`mergedInMain` 由 `git branch --contains` 现推）。改 durable policy 记 **ADR**。（Track A 触发时走定稿序列：human 批 decision packet → architect 机械回灌 PROPOSAL〔不夹带新设计，作者≠判官〕→ human 盖 FROZEN → review-moderator 一致性核验 + 结构门 → 主线程 packet-bound landing，下同。）
  5. **同步联动**：`dev-loop.md`、治理文档、`AGENTS.md` / `CLAUDE.md`、agent 定义、rules/references、scope-map/hooks/scripts——**改源时同批同步本文件内的全部投影处**。
- **阈值**：纯措辞 / typo → architect 改 + human ack；改行为 / 门力度 / 角色边界 / agent 定义 / 强制执行层 → 全套。
- **详见**：[architect-operating-model.md](../.claude/references/architect-operating-model.md) · [review-governance.md](../.claude/commands/review-governance.md)。

### 四条流程怎么串：一个真实变更的路由全景

```mermaid
flowchart TD
    START(["一个变更进来"]) --> TRIAGE{"/triage-change 分档"}
    TRIAGE -->|"疑似触契约/事实源"| AR["architect-reviewer"]
    AR --> GATE1["① 契约门"]
    TRIAGE -->|"spec/治理类"| GOV["④ 治理变更 (Track B)"]
    GOV -->|"升 Track A"| ADV["② 对抗 review"]
    TRIAGE -->|"实现面"| DEV["③ Dev Loop OS（档 2）"]
    GATE1 -->|"配套 spec 同 PR"| DEV
    ADV -->|"confirmed 需实现<br/>(dev-loop-handoff.json)"| DEV
    DEV -->|"实现中发现契约不够"| GATE1
    DEV -->|"retro 发现流程债"| GOV
```

# 下篇 · 规则条款

## 5. 工程门体系

> **不变式：工程门 = 本表 6 行。** 任何「新门」裁决必须折进既有行，并在表后「折进注」留一行带出处注记；不得另立枚举。

| 工程门 | 谁把关 | 判据 | 属流程 |
|---|---|---|---|
| 契约门 | 人类架构师 | Tier-1 契约变更 | 契约门 |
| supervisor 合并门 | 机械闸 `supervisor-verdict-gate.mjs`（planner invoke，不派 supervisor agent）：低/中风险 + 证据完整机器 APPROVE 自动放行（merge 执行由 planner/主线程经前置升级闸放行后本地合并）；高/critical 升级 human；连续 2 轮 BLOCK 未转绿 → 熔断 ESCALATE（计数主体 = 机械闸脚本，FIX-12） | 证据可信 + 风险等级 | Dev Loop OS |
| 对抗 review 人类拍板 | human | deadlock / 高影响 / 跨系统 / 触契约 | 对抗 review |
| governance approval | human | G1 / G2 / G3 变更 | 治理变更 |
| verifier 闸 | verifier（确定性，靠退出码） | 结构闸 + 功能闸 | Dev Loop OS |
| acceptance 冻结门 | acceptance-author | 红基线覆盖每条 sourceRef，且对真实行为敏感 | Dev Loop OS |

**折进注**（均不新增门，与 6 业务门无关——两族门见 §1.1）：

- **lightweight-architect-review**（**Track A 触发时适用**）= 「对抗 review 人类拍板」的具名 depth 子道（**Track A · G 层路由**的一档）：仅小/低风险 G2；eligibility 判定与边界见 §6 退役后常态规则段 + [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」。
- **Track B 轻路径** = **常驻默认路由（通用化）**：一切治理 / spec 变更默认走 Track B 七步闭环（见 §4 治理变更 + [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」）；Track A = 升级判据触发的休眠红队（判据：触 `docs/contracts/**` / 跨系统结构性变更 / 评审 3 轮熔断 / 自愿声明红队）。非第 7 工程门（`ARQ-260731-fastpath-protocol-codify` FROZEN；2026-08-02 永久化见 `delivery/changes/arch/260802-gov-review-track-b-permanentize/PROPOSAL.md`）。
- **UI design-review BLOCKING** = 经 integrator 写入 Review Packet、由既有 supervisor 合并门消费；severity 阶梯是既有 BLOCKING/SHOULD/NIT 阶梯的解释，不是新轴（见 [code-review-gate.md](../.claude/references/code-review-gate.md)）。
- **`/triage-change`** = bootstrap 期临时信任门（相位化信任模型自原仓移植），非第 7 个工程门、非业务门，不进本清单；其路由裁决的人类确认触点是门内触点（见 [change-triage.md](../.claude/references/change-triage.md)）。
- **closeout 硬化** = 既有 verifier 闸族 `closeout-check` 门内加项：Q5 已裁 warn；教育期四轮观测 pod-retro 自律率 0/3 → 3/3 → 4/4 → 6/6、干净样本 13/13，human 裁 2026-08-25 **维持 warn、零文本改动**（ARQ-111 已结；升 blocking 须修订已冻结 C-07、同 profile full-adversarial + human，触发条件 = 出现非窗口期 pod-retro 缺写实例时重新登记评估）。
- **commit-before-launch launch 前置门** = launch 编排的**结构性前置条件**（制度族机械检查 `git cat-file -e <base-ref>:<path>`，对偶 `taskpackage-required-check.mjs` 起包结构闸），**非第 7 工程门**、非业务门，不进本清单；其 launch 拒止是机械结构校验（判者退出码），非 human judgment 门（governance `ARQ-260729-portfolio-handoff-failclosed` FROZEN v1.0 / F-01·F-10；机制见 [pod-coordination.md §2b](../.claude/references/pod-coordination.md)）。
- **机械 supervisor-verdict 闸** = 「supervisor 合并门」（行 2）的**承载形态变更**，非第 7 工程门（P-5 六门不变式据守）：supervisor agent 2026-07-25 退役后，合并门放行裁决由机械闸 `supervisor-verdict-gate.mjs` 产出（riskTier 从 git diff 重派生 + 证据完整度消费 bearing facts）、Stop 钩子 `supervisor-gate-check.mjs` 第二方验 provenance（`mechanicalSupervisorGate`）+ 从 immutable 输入重算核对（FIX-6）。判据列字面「低/中自动放行」是恢复已成文门力度（J10），非新增。**显式反转前序 human-ruled 裁决**：本闸反转 `gov-review-supervisor-provenance-strength`（FROZEN HUMAN-APPROVED 2026-07-25 冻 `supervisorAgentDispatched===true` required）——supervisor 退役后该钉法不可满足（260729:19-20 planner 诚实拒伪造实证），H7 human 批准反转 2026-07-30（见 [decision-packet](../delivery/changes/arch/260730-gov-review-verdict-bearing-dual/decision-packet.md) §6）。

- **前置升级闸（`pre-merge-escalation-gate.mjs`）** = 「supervisor 合并门」（行 2）的 merge-期机械前置（对偶既有「机械 supervisor-verdict 闸」折进注范式）：例外清单命中即 BLOCK 升级人，非第 7 工程门。出处 = 源仓 `delivery/changes/arch/260831-gov-review-auto-merge-local-main/PROPOSAL.md`（同步自 commit 6154a6c）。

## 6. 治理的治理（meta）

思想⑤的落地：规则、角色、制度文件本身也是被治理对象。描述任何治理面必须区分四概念——**Authority**（哪个文件是规则事实源）· **Owner**（谁负责提出 / 维护）· **Writer**（哪个 agent 可实际改）· **Approver**（谁批准）。architect 是开发治理 owner / writer，但不自批、不替代契约门、不替代合并门。

| 治理面 | 文件 / 目录（= 该面 authority） | Owner / Writer | Approver |
|---|---|---|---|
| **入口与指令根（G1）** | `AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md` | architect | **human** |
| **角色与流程定义（G2）** | `.claude/{agents,commands,references,rules,skills}/**` | architect | **human** |
| **强制执行层（G3）** | `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/verify-gates.json`、`.claude/{hooks,scripts}/**` | architect | **human** |
| **证据与队列（G4）** | `delivery/changes/arch/**`、`delivery/changes/dev/**`、`docs/ARCH-REVIEW-QUEUE.md`、`docs/CONTRACT-DECISION-QUEUE.md` | 按流程分工 | 按流程分工 |

**硬条款**：

1. **AGENTS.md ≡ CLAUDE.md**：同类常驻指令根，同 owner / writer / approver，不得一个可写一个不可写。
2. **agent 定义只归 architect 起草**：`.claude/agents/**` 的职责 / 边界 / 工具 / write scope 变更只能 architect 起草（**landing 恒 = 主线程 packet-bound**）+ human 审核批准；builder / fixer / planner / reviewer 类 agent 不得自改或代改。
3. **自改递归**：修改自身定义、权限边界、工具、write scope，或影响自身权限的强制执行层文件 → 必须升级为 human-approved governance change；不得自批，不得仅凭同一 agent 的判断落地。现行权限下主要约束 architect；未来任何被授予 `.claude/agents/**` 或强制层写权限的 agent 自动受同一约束。
4. **第五阀（review-loop 自指）**：改 review-loop agent（review-moderator / scenario-author / challenger / defender / adjudicator / architect-reviewer）自身定义时，其任何 re-review 必须在新改动 canonical landing 生效前跑、或由不加载被改 description 的独立会话 / harness 跑。此阀与 `architect.md` 的 D-C glob 硬排除不同：review-loop agent 仍在改写面内，靠本阀 + human 门兜底。

**landing 通道（常态规则，2026-08-02 起）**：canonical landing 恒 = **主线程 packet-bound**（G1/G2/G3，landing-manifest 绑定 + `landing-verifier` 独立核验）；**architect 恒为判官 / 起草，永不 landing**（approved-landing mode 已于 2026-08-02 退役——`delivery/changes/arch/260802-gov-review-track-b-permanentize/PROPOSAL.md`）；human 把关点 = **立项 ack（前端对齐：需求/方向/量级）+ 例外反选（前置升级闸）；本地合并自动，remote 同步仍 human**。例外升级拍板的升级件必含并行相交面内容（分级路由：①零相交机械放行 / ②无取舍并集+双方齿复跑 / ③architect 消解评审 / ④human——见 `pod-coordination.md` §10d；human 批的是**消解方案**而非放行仪式，260908-gov-review-escalation-conflict-ladder 判据换轴）。机械兜底 = scope-map `architect` deny carve-out（保留常驻）。Track A 触发时落地通道亦同（不因轨道而变）。详见 [arch-review-protocol.md](../.claude/references/arch-review-protocol.md)「双轨路由」。

**三类制品**（不替代 authority）：

| 类别 | 例子 | 规则 |
|---|---|---|
| 耐久 canonical | contracts、kernel/system specs、G1–G3 规则 | 定义“现在是什么”；只能按对应门修改 |
| 执行状态 | `delivery/workstreams/<workstream>`、`delivery/taskpackages/<id>/`、`delivery/milestones/`、Pod board/token | 服务正在进行的拆包与调度；不是政策或审批替身 |
| 审计证据与队列 | `delivery/changes/arch/**`、`delivery/changes/dev/**`、frozen proposal / charter / findings / decision packet / verdict；ARQ 管 spec/governance soundness 未结，CDQ 管 Tier-1 契约 shape/接线 | 记录“为何、谁、按何判据”；冻结后不得静默重写 |

## 7. 入口速查

本表是 §1–§6 的索引，不是规则本身。

| 我想改... | 入口 | 批准 |
|---|---|---|
| `docs/contracts/**` | `architect-reviewer` 四分类 → 契约门（配套 spec 同 PR 原子落地） | 人类契约门 |
| 系统 spec（KidPal：`docs/requirements.md` + `docs/architecture.md`） | 治理路由（Track B 默认）；跨系统结构性变更 / 声明红队 → `/arch-change`（Track A spec profile 对抗 review） | 低影响 moderator；高影响 / 跨系统 / deadlock / 触契约 → human |
| L0 蓝图 | Track A 语境（= 跨系统结构性变更，命中升级判据②）；备改法交人类，不自动落地 | human |
| 系统级 UI 蓝图 / UIUX·OpenDesign 治理 | `/arch-change` governance profile 携带 UIUX 轴集（非第三 profile、非新工程门；系统级 brief 由 `uiux-designer` 起草候选，moderator dispatch 尚未接线） | human（跨系统 / 主流程 / IA / 品牌）；局部低风险可 loop 落 |
| 自有实现面（固件 / shim） | `/triage-change` 比例化四档 → reviewed plan → Dev Loop OS | supervisor 合并门 |
| agent 定义 | 治理变更（Track B 默认）；被改 agent 不得自改或找人代改 | **human** |
| command / reference / rule / skill | 治理变更（Track B 默认） | human |
| scope-map / settings / verify-gates / hook / script | 治理变更（Track B 默认，高敏） | human |
| README / AGENTS / CLAUDE / 治理文档 / 蓝图 | 治理变更（Track B 默认；L0 蓝图见上行 Track A 语境） | human |
| `docs/ADR.md`（架构决策记录） | integrator 收口落新增条目 / 主线程治理批落条目；条目格式与 append-only 约定见该文件头（T7，260907 批，human 裁 docs 根平铺） | 对应流程 |
| `delivery/changes/arch/**`、`delivery/changes/dev/**`、ARQ、CDQ | 按来源流程记账 | 对应流程 |

> **统一入口**：治理 / spec 变更**默认 = Track B**（主线程起草 PROPOSAL 走起，见 §4 治理变更）；命中升级判据或声明红队 → `/arch-change <plan>`（Track A：先分类 spec / governance / mixed〔contract-touching 是横切 flag，命中转 architect-reviewer 与契约门；纯 `docs/contracts/**` 直走契约门〕，再派 architect 起草、路由进对应 profile）。
> 一个真实变更常**串起多条**：spec 触契约 → 转契约门；confirmed 需实现 → 产 `dev-loop-handoff.json` 进 Dev Loop OS；实现中发现契约不够 → 兜底转契约门；retro 发现流程债 → 治理变更；`/triage-change` 发现疑似事实源 → 先 `architect-reviewer`，再升级 `/arch-change` 或契约门。

## 8. 已知缺口

尚未成文的开发过程流程先登记，后续按**治理变更**补；对应流程成文 landing 后从本节移除，不滞留已结项。

- **repair-intent 自动化桥（Phase 9）**：入口已定为 portfolio-planner intake；把 K6 `DefectDetected` 自动变成 repair-intent 制品并触发 launch 的 automation bridge 仍是未来占位。
- **新系统创建的 `.claude` 扩充**：新增 K/S 系统时 agent 克隆体、scope-map、hook 的扩充流程。
- **`.claude/**` 一致性机械门**：agent 定义 ↔ scope-map ↔ command 引用的一致性检查。**已分批部分落地**（批次 1 / 批次 2 / 腿 6 launch 可达性 `--check-launch`）；剩余子项与消费方链见 [ARCH-REVIEW-QUEUE.md](ARCH-REVIEW-QUEUE.md) ARQ-077。
- **uiux-designer 系统级 brief 的 moderator dispatch 接线**：起草者已定（`uiux-designer`），dispatch 尚未接线。

## 验收测试纪律

> 成文：260906-gov-review-arq-governance-batch（FROZEN v1.0，Track B）；承载 ARQ-260901-g3-server-live-acceptance / ARQ-260906-live-acceptance-real-path / ARQ-260902-frozen-acceptance-never-run。unit-run-check.mjs / acceptance-run-check.mjs 行内注释与本节同名互指。

### 1. server 验收 host/live 两态

server 面验收分两态：

- **host-fast**：可在本机/CI 快速执行的 pytest。unit/acceptance 两闸执行器在闸内真跑。
- **live on-target**：需 SSH 上 ECS / 真机 / 外网凭据的验收，以 `@pytest.mark.live` 标记。两闸执行器只走结构闸、**不 auto-run**（merge/CI 环境无 SSH/凭据，auto-run 必假红；对偶固件 on-target 只结构闸）——live 验收由 acceptance 相位人工/台架触发并落 verifier-evidence。

**pytest-not-installed 语义**：host-fast 路径下 pytest 未安装 = fail-open 跳过并 warn（对偶 firmware idf.py 缺失），**不是**绿灯依据——闸担保「不静默跳过」，不担保「实机已过」。

**两态在条目/类级判**（260909-gov-review-gate-truthfulness T1c/T4）：闸内以 `pytest -m "not live"` 执行，禁按文件级标记整份排除（实测病灶：`server/acceptance/` 9/9 混合件曾被文件级判据整份放行）；一文件内混排两态合法，全为 live 面时闸须显式报 `allLiveDeselected` 而非静默 skip。闸「没跑成」（degraded / executor-skipped / allLiveDeselected）由 verify.mjs 上收为 round evidence 的机器可判字段 `notExecuted`；**`notExecuted` 非零时 verifier 报告必须逐条点名**（该字段不当第二道门、不改 ok 判定——可见性义务，机械消费另批）。

### 2. probe ≠ exercise（live 验收必须真跑一遍真实链路）

跨进程包的 live 验收必须真 exercise 一次真实端到端路径（最小 tracer-bullet 即可），**只探测可达不算 live 验收**。验收起草（acceptance-author）与核验（verifier）两级遵守。教训实证：G 包 live 验收当年只探端口可达，六个真实链路缺陷（websockets 版本不兼容、缺 device-id 头透传、TTS type 拼写、OpenClaw base_url 缺 `/v1`、容器 loopback 不可达、Intent 拒绝）全递延到 WS 包真机冒烟才连环爆（ARQ-260906-live-acceptance-real-path）。

### 3. 冻结即冒烟（线束通道 error 为零）

冻结验收落盘时，acceptance-author 当场执行一次线束冒烟：live 面真连一次 SSH 可达性、线束助手（连接/读写/payload）真调一次、pytest 收集零错误。判据 = **执行通道 error 为零**（error ≠ failure：收集错/夹具崩/线束异常 = 线束缺陷，当场修）；断言 failure 是冻结相位的设计态（实现未建、红基线），不阻塞冻结。为什么只冒烟线束、不要求断言转绿：历史病灶（`_ssh` 解包、payload 撞命令行上限）全部落在线束层、**不需要被测实现存在**就会炸（TP-A 与 smoke-e2e 两度实证，ARQ-260902-frozen-acceptance-never-run）。

### 4. C 类冻结验收实编（编译通道 error 为零）

C 类（固件/结构面）冻结验收文件落盘时，acceptance-author 当场实编一次该验收文件本身（C 编译器过一遍），判据 = **编译通道 error 为零**。红基线对结构/基建类包明示「验收文件本身可编译」：§3 的线束冒烟清单（SSH/线束助手/pytest 收集）不含编译通道，C 验收的语法缺陷（宏参数形态、复合字面量分组等）只有实编才暴露——260829-host-test-infra 实证：`host_infra_smoke.acceptance.c` 宏参编译错误到 builder 首实编才爆、退回 acceptance-author 返工（ARQ-260907-frozen-acceptance-compile-baseline）。§3 管执行通道、§4 管编译通道，两者并列不互替。

## 起包格式期望前置（planner / portfolio-planner / acceptance-author / builder 四方）

> 成文：260907-gov-review-arq-governance-batch-2（T6）；承载 ARQ-260907-taskpackage-format-expectations-frontload。格式期望只活在闸的判定代码里、起草侧零成文 = 靠闸红反推再回派（跨 4 包 5 子项实证）。六条前置清单：

1. **mutationEvidence 骨架**：Draft TaskPackage 必须带 mutationEvidence 字段骨架（空骨架也行，字段必须在场）——portfolio 起草不带骨架，mutation-check 闸红一轮再回派补（260903-gateway-directionB / 260829-session-logic 两度实证）。
2. **验收断言格式期望**：acceptance 面写机器可识别断言形态（acceptance-coverage-check 可认的断言风格）+ coverage 标记随测试名——markdown prose 断言会被判「无可识别断言」回派补写（260831-ui-art-assets 实证，回派补 31 条）。
3. **验收 ID 禁括号后缀**：acceptanceRef 形态为 `Sx-Ay` 等 kebab 短横形态，禁 `(B#)` 类括号后缀——括号破坏闸正则 `\b` 词边界匹配，标记被迫写尾随下划线丑形态（同包卡点 3）。
4. **降级声明前置**：资产/规划类包对 mutation / docs-sync 的降级声明（不适用 + 理由）在 Draft TaskPackage 阶段写字段，不等到 verifier 红才补（同包卡点 4）。
5. **新约定包同步闸判据自查**：建新约定（新目录、新测试通路等）的包，dispatch 时同步查相关机械闸的判定口径——闸判据滞后到全量终审才发现 = vacuous-green 风险（260829-host-test-infra 实证）。
6. **输入面期望前置（input-brief + 超读申报 + crossEnd）**：派发的输入面必须是**起包期派生的封闭清单**，不是子 agent 每次自扫。三件一体：① planner 在 Step 7 派发前用 `.claude/scripts/brief-inputs.mjs` **一次产双份** input-brief（`input-brief-acceptance-author.md` / `input-brief-builder.md`，从 TaskPackage refs + `crossEnd` 派生：契约/spec 锚点节定位 + 跨端对端面 + 同 subsystem 先例封顶 3 个），派发 prompt 只引用 brief 文件路径与纪律一句、**不转抄清单正文**（防双源漂移）；② 子 agent 清单外阅读**允许且是常态**，但交付报告必须列「清单外阅读 + 原因」段——**超读是常态动作，不申报才是问题**（把申报做成硬门槛只会逼出隐瞒、让仪表盘失去意义）；③ `crossEnd` 申报字段 Draft 必在（空骨架也行，同第 1 条）+ finalize 时 `counterparts` 非空（每条含 path+symbol，本批限自有实现面）**或** `exempt` 写明理由（≥12 字符，同本闸 `landsIn=dropped` 判据）二选一，双缺 = 闸红点名（与第 1/4 条同型）。跨端性是设计判断不是可推导属性——subsystem 单面标签的包可以业务实跨端，故走申报 + 留痕而非硬谓词。成文：260908-gov-review-input-surface-frontload；实证：`260829-story-p3/pod-retro.md:11`「把 spec/契约/先例读了个遍」stall 案 + `260907-g-relay-fix` 自创 `display_relay` 帧离线全绿、真机被固件 `Unknown message type` 拒案（其 pod-retro「下次改进 1」即本条第 ②③ 件的前身）。

## friction 事件最小 schema（dev-loop 记账）

> 成文：260907-gov-review-arq-governance-batch-2（T4）；承载 ARQ-260907-friction-event-payload-schema。聚合告警面 = `pod-events-aggregate.mjs` `friction.malformedFrictions` 列（残缺事件显式点名，不再静默归 unknown）。

pod-events 的 friction 事件 payload 最小 schema：**`frictionType` + `summary` 必填**（frictionType 用既有关键词形态，summary 一句话讲清现象）；附加细节允许扩展键（非标键会被聚合归 unknown——用扩展键时仍须带齐两个必填件）。**空 payload `{}` = 落账缺陷**：内容机器侧不可恢复即实际证据丢失（260829-voice-roundtrip 两条空 payload 实证，其一内容只能从 commit message 追回、另一条彻底丢失）。落账时点 = 摩擦发生的当次交互内（planner / 主线程随手记），不攒到收口补记。
