# PROPOSAL · 260913-arch-review-spec-split

> **状态：FROZEN v1.0 · 2026-09-13 · 凭据 = human 立项 ack（2026-09-13「交 /arch-change」）+ architect R4 GREEN（§7 评审记录，附 F15-a/b 建议级修正已回灌）+ human 裁 OQ 全集（OQ-1 归档 / OQ-2 逐条过目 / OQ-3 待定迁入 / OQ-4 先手动跑，2026-09-13）；landing 时序 = 待 .planning 旧批收尾归档后执行（human 定：今天 review 完不 landing）；landing-manifest 即终稿 17 行，落地后 PROPOSAL 不可回改**
> **承载 ARQ**：无（新立项；与 260912-gov-review-portfolio-decomposition-chain 为并行耦合关系，见 §6 留根）。
> **review 轨**：Track B 常驻轻路径（预判：不触契约本体、非跨系统结构变更；最终分类由 review-moderator 判）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。
> **riskTier**：low-medium（行为零变更、docs 层重排 + 机检脚本新增；但触 FROZEN spec 的上游权威迁移与 docs 白名单，闸向严）。
> **review profile**：spec profile 为主 + governance 配套强耦合同 packet（human 2026-09-13 裁单 packet 原子跑）；触面 = docs/spec.md 拓扑 + **G1 指令根**（AGENTS.md:7、docs/DEV-GOVERNANCE.md:241，human 知情批准）+ **G3 强制层**（scope-map docsTopLevel、pre-merge-escalation-gate.mjs 字面核验集扩展——census 实证）+ **agent 定义自改递归**（architect.md、architect-reviewer.md、integrator.md，human 知情批准，F6 裁 (a)）；契约面触面为零——契约本体不动，:4 断链挂账 CDQ（F2 裁 (a)）。
>
> **结构字段硬条款**：landing-manifest 为草案，最终清单以 FROZEN 为准；GREEN 前必须在场，判官核对。

```landing-manifest
docs/requirements.md
docs/architecture.md
docs/archive/spec.md
docs/CONTRACT-DECISION-QUEUE.md
docs/DEV-GOVERNANCE.md
docs/notes/ui-implementation-strategy.md
AGENTS.md
.claude/commands/arch-change.md
.claude/scope-map.json
.claude/rules/repo-structure.md
.claude/agents/architect.md
.claude/agents/architect-reviewer.md
.claude/agents/integrator.md
.claude/scripts/spec-anchor-check.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.delivery/milestones.jsonl
.delivery/delivery-workspace.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把单文件 `docs/spec.md`（644 行、需求/架构/决策/里程碑混排九节）拆成两个互相锚定的文件——`docs/requirements.md`（产品是什么：背景目标、story、交互与 UI 产品定义、非功能需求、路线图）与 `docs/architecture.md`（怎么造：硬件、系统架构、UI 实现策略、端云接口、技术决策、开放问题）——每条 story 带「支撑架构 + 可行性三态」锚并由脚本机检，作为 phase 2 重新定基线（re-baseline）的需求锚。

**现状差距**：story 没有独立的家；现有 UI 锚无机械校验且头部引用已漂移（§1 P2）；非功能需求散落在技术决策与 story 验收里、无 ID 无单列阈值（§1 P3）。

**非目标（本批不做）**：
- 不动契约本体（`docs/contracts/**` 零接触，CDQ-260902 在办事项原样继续）。
- 不改 `firmware/`、`server/` 任何实现；本变更纯 docs + 治理登记 + 一个校验脚本。
- 不做「实现符合度」审计（页面走没走偏）——那是 phase-2 workstream PROPOSAL 的一次性产物，不进 requirements.md（human 已裁，见 §4 OQ 区外已定项）。
- 不立「TaskPackage story 字段必须内嵌业务 story 原文」的包级规则——归 260912 拆包链 session，本变更只提供它消费的锚点格式（§6 交接点）。
- phase 2 立项本身是下游独立动作，不在本变更内。

## 1. 问题与根因（地面真值）

### P1 · Pod 开发丢失业务 story（两层断链）

**证据**：`.claude/taskpackages/260829-schedule-reminder/taskpackage.json` 的 `story` 字段（2026-09-12 读取，verbatim 片段）：「脑算网关推：OpenClaw cron（每 15 分钟定期重算 + QQ 录入即时触发）算三档时刻、幂等 id（确定性散列，跨重推/重算不变）写 alerts/pending.json、speech 排程时预合成」——全为实现机制语言。对应业务 story 原文在 `docs/spec.md:318`（verbatim 全句）：「作为孩子，我希望遇到非常规的事（要带的东西、临时安排）和容易忘的课（周末培训班这类）设备提前主动提醒我，以便不丢三落四；例行去上的学校课我自己看课表就行。」——**包内零出现**。且该包 `origin.upstreamRef` = `.planning/kidpal-mvp/PROPOSAL.md`，不指 spec.md §4.1，Pod 要找回业务 story 须自行跳进 644 行混合文件。

**根因（两层）**：① story 无独立的家——混排在 spec.md §4，与技术决策、接口细节、里程碑同文件，拆包者从 PROPOSAL 二次压缩时业务框架（作为…我希望…以便…）天然丢失；② 拆包转写无规则约束——包级 story 字段写成什么无人守门（此层治本归拆包链 session，本变更提供锚点接口）。

### P2 · UI 锚无机械校验，头部引用已漂移（活样本）

**证据**：`docs/spec.md:252` 声明「story 与 [kidpal-ui-design-v2.html](ui/kidpal-ui-design-v2.html) 一一对应」，但 `docs/ui/` 现行设计稿 = `kidpal-ui-design-v3.html`（2026-09-13 实查目录；§5.6 教学字体红线标题已带「（v3）」版本标签，:527）。锚的机制本身存在且良好（设计稿元素打 `data-od-id` 稳定锚名，story 按名引用），但**没有任何脚本验证锚的有效性**，断了无报警。

**根因**：锚是单向人肉引用，无机器兜底；版本更迭（v2→v3）改多处而非一处。

### P3 · 非功能需求（NFR）无独立的家

**证据**（均指 docs/spec.md 现行行号，2026-09-12 实读、2026-09-13 判官复核）：「超 4 秒先开口安抚」埋在 §4.0 底座表（:263）；功耗续航在决策 21（:595）；传输安全在决策 22（:596）；离线边界在决策 24（:598）；内容安全姿态 V30 在决策 28（:602）；可读性两条分埋两处——「字号下限待 1bpp 真机可读性实测」在决策 28（:602），「暗光不可见/无前光」在 §5.6（:529）。

**根因**：这些是孩子/家长可感知的产品质量（快不快、断网行不行、安全不安全、续航多久），按边界原则属需求面，但现无 ID、无量化阈值单列、无锚，全混在技术决策与 story 验收行里。

## 2. 改法（T1…T5）

### T1 · 新建 docs/requirements.md（需求面 SSOT）

**内容迁移与新结构**（自 spec.md；2026-09-13 human 三点修订并入——线框随 story、Epic 层级、Infra 命名）：

- **头部**：已砍声明区（现 spec.md:256 声明区原样带走 = 两处反白高亮砍项；S-F2.5 云端润色现为 :394 标题「（可选）」标注、kidpal-mvp PROPOSAL 已记 dropped——迁入时转为已砍清单新条目，不占用 story 编号位）+ UI 锚规则块 + 互链 architecture.md——导航由两文件头部互链承担。
- **§1 背景与产品定位**：原 §1 + 原 §5.1 设备定位并入。
- **§2 共享底座**：原 §4.0 + 并入原 §5.2 语音交互模型 / §5.3 按键定义 / §5.4 页面结构——现 spec 里 §4.0 与 §5.2/5.3 的状态表、按键表本就重复，拆分正好合并去重。
- **§3 需求主体（Epic > Feature > Story 三层）**：Epic（史诗）= 产品/阶段级目标（E1 = MVP「会说话的显示器」，二期另立），带目的陈述、不直接带锚，锚由 story 聚合上报；Feature = F1–F4 编号保留，**每节 = feature 说明 + 页面线框 + story 列表**——线框随 feature 走，但**源库存如实**：原 §5.5 实存仅 P1 首页线框一件（:512–522，归 F1 节），P2–P4 线框是 :530 自证的「待补」项——迁入时在对应 feature 节挂「线框待补（归设计稿迭代）」占位标，不虚构库存；Story = S-Fx.y 不变，锚行在这一级。
- **§4 体验与视觉风格**：原 §5.6（吉祥物/趣味性/教学字体红线/暗光）——跨 story 的产品视觉定义；**字体类需求（如「标题用站酷快乐体」）住这里**，其技术支撑锚指 architecture.md「UI 实现策略」节（字体转 .bin + SPIFFS 资产管道）。
- **§5 NFR 章**（见下「新增三」）。
- **§6 路线图**：原 §8 改名。
- **§7 需求类开放问题**：原 §9 分流（家长端形态、隐私归属）。

**新增一 · story 锚行**：每条 story 的锚行 = 既有「UI 锚点」+ 新增「支撑 / 可行性」两件——**原「端云」标注（`[板]`/`[云]`/`[板+云]`）废弃**（2026-09-13 human 定）：支撑锚定位到架构具体小节，是端云一维标签的严格超集，两套并存 = 两本账漂移源。**废弃作用域 = 全量**（R1-F7 收口）：story 锚行与共享底座表（原 §4.0）的「端云」列一并废弃，底座节级改钉一行支撑锚（板侧音频/渲染小节 + 云侧网关/OpenClaw 小节），各状态行的端侧/云侧职责由支撑锚吸收。迁移时原端云信息由支撑锚吸收——涉及双侧的 story，其支撑锚须同时含板侧与云侧小节；拆包时包 `subsystem` 判定从支撑锚推导（交接点同 §6）。
- **支撑**：`arch §x.y` 列表，指向 architecture.md 真实小节；
- **可行性**：三态之一——✅已实证（有实测/红基线证据）｜🟡设计可行·待实证｜🔴当前架构撑不住。
- **🔴 挂账纪律**：标 🔴 的 story 必须在同行情引用开放问题或 ARQ 编号，不许静默存在；处置路径 = 立项讨论（改需求或改架构），Pod 无权自裁。
- **初始标定规则**（迁移期）：从现有证据推导，不新造判断——spec 已标「实测可行」的（如 OpenClaw cron 调度）→ ✅；标「待部署验证」的（ASR 流式兼容性、多轮会话连续性）→ 🟡；🔴 迁移时预计为零。**逐条标定清单（每条带证据行号）经 human 逐条过目后方可落文件**（OQ-2 裁，2026-09-13：逐条 human 过目，非主线程自标后复核）。

**新增二 · UI 锚规则块**（文件头部明文，执行者抬头可见）：
1. **粒度**：锚「舞台」——页面画布（`canvas-pX`）与会话状态（`state-*`）；不锚「道具」——图标/字体/组件级资产（`icon-library`/`font-*`/`comp-*`）由设计与实现侧引用，story 不挂。
2. **共享**：跨 story 的状态链在共享底座钉一次，story 引「共享底座」不重复罗列。
3. **例外**：组件本身是 story 的验收对象时才单锚（如页点 `pgdots`）。
4. **指向**：头部引用指「docs/ui/ 现行设计稿」语义，版本更迭只改一处；当前 = v3（同时修掉 P2 的 v2 漂移）。
5. **设计稿锚稳定规约**（2026-09-13 human 提问催生）：`data-od-id` 是**约定不是保证**——实证：v2→v3 的 `canvas-pX`/`pose-*`/`state-*`/`comp-*` id 族逐字保留（2026-09-13 双文件 Grep 比对），现行生成流程事实上稳定，但推倒重生成无人拦。故立规约：id 只增不随便改，改名/删除 = 有意识的锚维护动作，改设计稿者负责同步 requirements.md 引用；出新版设计稿时跑 T3 脚本 diff 前后两版 id 全量清单，被 story 引用而消失的 id 必须显式处置（同步改锚或恢复 id），不许静默断。
6. **机检**：每个 UI 锚必须解析到现行设计稿真实存在的 `data-od-id`（见 T3）。

**新增三 · NFR 独立成章**：
- **ID 格式**：`NFR-<域>-<序号>`；域 = PERF（延迟）/ PWR（功耗续航）/ SEC（安全隐私）/ OFF（离线）/ SAFE（内容安全）/ READ（可读性）。
- **每条带四件**：量化阈值、验证方式（怎么实测、owner 谁）、支撑锚（`arch §x.y`）、可行性三态（与 story 同套）。
- **与 story 的关系**：story 内散落的 NFR 表述（4 秒安抚、断网无提醒）保留为该 NFR 在具体场景的具体化，并回引 NFR ID；同一约束全仓只定义一次。
- **边界**：NFR 只管「产品必须做到什么」；技术实现约束（介质禁令 V4、证书形态、provider 禁带 max_tokens）归 architecture.md。
- **迁移纪律**：现 spec 未拍死数值的阈值（如续航阈值）原样带「待定」标，不在拆分里夹带新决策。

**目标文件**：`docs/requirements.md`（新增）。

### T2 · 新建 docs/architecture.md（架构面 SSOT）

**内容迁移**（自 spec.md）：原 §2 硬件盘点**扩展改名「Infra（基础设施）」**——覆盖板子（ST7305/按键/电池）+ ECS 服务器 + 网络拓扑；命名的通用化动机 = 本两文件结构将来可作其他项目的模板，其他项目不一定有硬件板子，「硬件盘点」太局限（2026-09-13 human 定）+ 原 §3 系统架构（含方向 B 服务端）+ 原 §5.7 UI 实现策略（LVGL 双层方案、字体/美术资产管线——需求面字体类声明如「站酷快乐体」的技术支撑点）+ 原 §6 端云接口**收编为「契约层（Contracts）」节**（2026-09-13 human 定）：正文 = 引用 `docs/contracts/interface-contract.md` 单一事实源 + 各契约面（笔顺 D3 / display E1 / 课表 B1 / shim API）一句话导航，接口细节不重复抄写——现 §6.1/6.2 与契约重叠的内容随契约门 CDQ-260902 重写归位契约，属协议选型决策的内容（方向 B、wss、provider 约束）留在技术决策节 + 原 §7 技术决策 + 原 §9 中技术类开放问题（ASR 选型、算力）。**原 §6.3「待定/已定跟踪」不单列节**（R1-F1 收口，2026-09-13 human 裁）：七条逐项审——**已实测闭环的标「已结」不迁入、随 spec 归档自然消亡**（配网 NVS 键位 = 配网已成功；ASR 部署期首验 = live 冒烟多轮已跑，human 原话「早就结束了」）；仍开放项 landing 时逐项核证据（包闭环记录 / ARQ 状态）定案，逐条处置清单进 landing-summary 供 human 复核，仍开放者进技术类开放问题节。文件头部声明读者（architect/planner）并回链 requirements.md。

**锚稳定规则**：小节编号发布后视为稳定标识；改编号 = 有意识地同步改锚（T3 机检抓悬空）。对契约的引用保持指 `docs/contracts/interface-contract.md` 不变。

**目标文件**：`docs/architecture.md`（新增）。

### T3 · 锚定机检脚本 .claude/scripts/spec-anchor-check.mjs

机械校验三件事：① requirements.md 里每个 `arch §x.y` 锚解析到 architecture.md 真实存在的标题（悬空 = 报错）；② 每条 story 的 UI 锚解析到 docs/ui/ 现行设计稿真实存在的 `data-od-id`；③ 🔴 可行性必须同行挂开放问题/ARQ 引用。另支持 `--reverse` 生成反向视图（架构节 → 影响 story 清单），**反向视图只由脚本生成，不手写双向维护**；支持 `--diff-ids <旧设计稿> <新设计稿>` 输出两版 `data-od-id` 清单 diff（设计稿迭代时的断锚前置检查，配合 T1 规约第 5 条）。

拦阻强度：先手动 / review 时跑，不挂 hook 硬拦（YAGNI；强度升级留 OQ-4）。

**目标文件**：`.claude/scripts/spec-anchor-check.mjs`（新增）。

### T4 · docs 白名单与结构规则登记

`.claude/scope-map.json` 的 `universal.docsTopLevel` 登记 `requirements.md`、`architecture.md`，移除 `spec.md`；`.claude/rules/repo-structure.md` 同步改写「spec 唯一在 docs/spec.md」行为两文件规则（两处必须一致，enforce-scope 钩子按 scope-map 机判——这是治理明文的「有意识动作」）。

**目标文件**：`.claude/scope-map.json`（改）、`.claude/rules/repo-structure.md`（改）。

### T5 · live 引用改指 + spec.md 归档（不删）

**spec.md 去向 = 归档**（2026-09-13 human 在 F3 裁定中给出的第三形态，覆盖 OQ-1 原推荐）：`docs/spec.md` 移入 `docs/archive/spec.md`，git 历史与文件本体双保留；docs/ 根白名单相应移除（T4）。**时序**（human 定）：今天 review 至 GREEN/FROZEN，不 landing；明天先把 `.planning` 旧批 workstream 收尾归档（human 定向的独立操作，不在本 packet），本 packet 再 landing——**旧批带着对老 spec 的引用语境一起归档，新结构（requirements.md + architecture.md）全新启用**；decision-coverage.json 的旧节号键随旧批归档，无需映射注记（R1-F3 悬空问题由归档消解）。

**live 引用改指集（census 已实跑定全集，见 §3 证据段；R2-F8 修订：原列 docs/templates ×2 实测零命中移除）**：
- **G1 指令根**：`AGENTS.md:7`（「spec 面 = docs/spec.md」行）、`docs/DEV-GOVERNANCE.md:241`（治理路由表「系统 spec（KidPal：docs/spec.md）」行）。
- **G3 强制层联动改法**：`.claude/scripts/pre-merge-escalation-gate.mjs:102/:127/:246-249/:437`——CDQ 核验行按 `docs/spec.md` 字面前缀比对 findings 的 Basis，spec 改址后对新路径失效（漏拦），字面集须扩展为 `docs/spec.md`（归档件，历史 findings 仍用）+ `docs/requirements.md` + `docs/architecture.md`；`.claude/agents/integrator.md:42` 的 Basis 前缀写作约定与闸的字面集**同批联动改**（只改一边 = 闸与约定脱钩），:71「消解不改 spec.md」边界清单同步改为新 spec 面路径集 + 归档 spec。
- **agent 定义（自改递归，human 知情批准）**：`.claude/agents/architect.md:23/:52`、`.claude/agents/architect-reviewer.md:42/:146`（canonical spec 指引改指两文件）。
- **command**：`.claude/commands/arch-change.md:24`（lane 分类的「KidPal canonical spec = docs/spec.md」行）。
- **docs 面**：`docs/notes/ui-implementation-strategy.md:3`（「spec 侧索引见 spec.md §5.7」改指 architecture.md UI 实现策略节）；`docs/CONTRACT-DECISION-QUEUE.md` **实测零 spec.md 引用（R2-F11），入 manifest 的理由 = 挂账登记新条目**（F2 裁 (a) 的契约断链 + 冻结验收件翻红后果，均落 CDQ），非改指。
- **`.delivery/` 面（R3-F12 补——首趟 census 执行口径漏扫本目录【非工具行为：R4 判官实测 `git check-ignore` exit=1、三件 `git ls-files` 全追踪、仓内无 .ignore/.rgignore】、判官补扫实证）**：① `.delivery/milestones.jsonl` 七条 `"acceptanceRef": "docs/spec.md"` 是**机器消费的 SSOT 字段**（`delivery-status.mjs:4` 明注 SSOT、STATUS.md 为派生视图勿手改，`:66` 渲染消费链）——改法 = 直改 jsonl 源，acceptanceRef 改指 `docs/requirements.md`（§6 路线图承载 M0–M6）；② `.delivery/delivery-workspace.json:6`「里程碑定义权威 = docs/spec.md §8」改指「docs/requirements.md §6 路线图」；③ `.delivery/STATUS.md` ×7 条 ref 链为**派生视图——不手改**，改 jsonl 后重跑 delivery-status.mjs 重生成（重跑入口形态 landing 时按脚本实况核实）。

**豁免（census 逐件归类）**：① 冻结验收件/实现面注释（`firmware/main/test/key_route_a1.acceptance.c:636-637`、`firmware/main/test/screen_unification.acceptance.c:1010` 的 spec 存在性 canary 断言，`server/acceptance/test_schedule_reminder.acceptance.py:1561` 直读文件，`firmware/main/key_gesture_parser.h:13`、`firmware/main/session_state_machine.h:13` 权威指针注释，`server/acceptance/test_smoke_e2e.acceptance.py:5/:60/:124/:128/:142`、`test_cloud_link.acceptance.py:5` 的 provenance 注释）——冻结件改不动（红线），处置归 phase 2 验收基线重建，见下「已知机械后果」；② 通用示例占位（`.claude/agents/planner.md:155/:178`、`.claude/scripts/enforce-scope-canary.mjs:25`、`.claude/commands/review-arch.md:3`、`.claude/references/task-intake-and-slicing.md:108`、`.claude/references/architect-operating-model.md:79`、`AGENTS.template.md ×4`、`system-spec.template.md`——均为 `docs/systems/Sx-name/spec.md` 模块级示例或通用机制描述，不绑本仓 docs/spec.md）；③ append-only 历史行（docs/ARCH-REVIEW-QUEUE.md:49、.claude/rules/contracts.md:14）；④ 资产母版（docs/ui/kidpal-ui-design-v2/v3.html 各 1 处，设计稿 v4 迭代时顺带改，§6 留根）；⑤ 第三方供应商/构建产物（vendor lv_font_fmt_txt.h、managed_components、__pycache__）；⑥ `changes/**` 归档、`.claude/taskpackages/**` 历史、归档后 `.planning/**` 旧批、`docs/contracts/**`（F2 挂账）；⑦ 派生视图（`.delivery/STATUS.md` ×7——改源重生成，不手改）。

**已知机械后果（诚实登记，不在本 packet 处置）**：spec.md 归档后，①冻结验收件的 spec 存在性 canary 断言（KR-E2/SUNU 系）将**真实翻红**、`test_schedule_reminder.acceptance.py` 直读将 FileNotFoundError——这是本变更治理授权下的已知环境红，landing 时登记进 CDQ、处置归 phase 2 验收基线重建；②escalation-gate 字面集若漏改 = CDQ 核验对新路径漏拦——故 (b) 项改法与归档**同一 commit 原子落地**。

**目标文件**：`docs/archive/spec.md`（迁入）、`docs/CONTRACT-DECISION-QUEUE.md`（挂账登记）、`docs/DEV-GOVERNANCE.md`（G1）、`docs/notes/ui-implementation-strategy.md`、`AGENTS.md`（G1）、`.claude/commands/arch-change.md`、`.claude/agents/architect.md`（自改递归）、`.claude/agents/architect-reviewer.md`（自改递归）、`.claude/agents/integrator.md`（自改递归，闸联动）、`.claude/scripts/pre-merge-escalation-gate.mjs`（G3 联动）、`.delivery/milestones.jsonl`（SSOT 源改指，R3-F12）、`.delivery/delivery-workspace.json`（权威指针改指，R3-F12）；`.delivery/STATUS.md` 为派生重生成副产物（不手改、不入 manifest 独立项）；census 已跑，改指集即全集、无「待定扩列」。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| docs | requirements.md / architecture.md | 新增 | T1/T2 |
| docs | spec.md → docs/archive/spec.md | G3 | 归档移动不删（F3 裁），git 历史 + 本体双保留 |
| 治理机判面 | .claude/scope-map.json | G3 强制层 | docsTopLevel 白名单登记两件、移除 spec.md，与 repo-structure 两处一致 |
| 治理文档 | .claude/rules/repo-structure.md | G2 | 「spec 唯一在 docs/spec.md」行改写为两文件规则 |
| 治理脚本 | .claude/scripts/spec-anchor-check.mjs | 新增（G3 面、无拦阻 hook） | 锚检 + 反向视图 + 设计稿 id diff |
| 指令根 | AGENTS.md:7 | **G1** | 「spec 面 = docs/spec.md」行改指（human 知情批准，F6 裁 (a)） |
| 指令根 | docs/DEV-GOVERNANCE.md:241 | **G1** | 治理路由表「系统 spec」行改指（census 实证新增） |
| G3 强制层 | .claude/scripts/pre-merge-escalation-gate.mjs | **G3** | CDQ 核验字面集扩展（spec.md→+requirements.md+architecture.md），与 integrator.md:42 写作约定同批联动（census 实证新增） |
| agent 定义 | .claude/agents/architect.md、architect-reviewer.md、integrator.md | G2·**自改递归** | canonical spec 指引 / Basis 前缀约定 / 消解边界清单改指，human 知情批准（F6 裁 (a)，census 扩到三件） |
| command | .claude/commands/arch-change.md:24 | G2 | lane 分类 canonical spec 行改指（census 实证新增） |
| docs | docs/notes/ui-implementation-strategy.md:3 | G2 | 「spec 侧索引」改指 architecture.md UI 实现策略节 |
| docs | CONTRACT-DECISION-QUEUE.md | G3 | **挂账登记新条目**（契约断链 + 冻结验收件翻红后果），非改指（R2-F11：实测零 spec.md 引用） |
| 冻结验收件 | key_route_a1 / screen_unification / schedule_reminder 等 | 未触（豁免） | spec 存在性 canary 翻红 = 已知机械后果，挂 CDQ、处置归 phase 2（T5 详） |
| 资产母版 | docs/ui/kidpal-ui-design-v2/v3.html | 未触（豁免） | 设计稿 v4 迭代时顺带改（§6 留根） |
| delivery 工作区 | .delivery/milestones.jsonl、delivery-workspace.json | G3 | SSOT acceptanceRef ×7 + 权威指针改指 requirements.md §6；STATUS.md 派生重生成不手改（R3-F12） |
| 通用示例占位 | planner.md / enforce-scope-canary / review-arch.md / architect-operating-model.md:79 等 | 未触（豁免） | docs/systems/Sx-name/spec.md 模块级示例或通用机制描述，不绑本仓 spec.md |
| 供应商/构建产物 | vendor lv_font_fmt_txt.h、managed_components、__pycache__ | 未触（豁免） | 第三方代码与构建副产物 |
| planning | .planning/** 旧批 | 未触（豁免） | 收尾归档后带老 spec 引用语境归档（F3 裁） |
| 历史制品 | changes/**、.claude/taskpackages/** | 未触（豁免） | append-only 纪律 |
| 契约面 | docs/contracts/** | 未触 | 零接触；:4 断链挂账 CDQ、验证豁免（F2 裁 (a)） |

**【sweep 义务 · census 已实跑（2026-09-13，R2-F9 兑现 + R3 判官补扫 .delivery）】** 命令：`rg -n "spec\.md"` 分趟（① 全仓排除豁免面与参考树；② `.claude/`、`docs/ui/*.html` 定向；③ `.delivery/` 定向——①趟**执行口径漏扫**该目录（非工具行为：`git check-ignore` exit=1、三件全追踪、无 .ignore/.rgignore，R4 判官实测），判官 R3 补扫）。**按文件分组的命中清单（完整）**：

- **改指（12 文件）**：`AGENTS.md:7`；`docs/DEV-GOVERNANCE.md:241`；`docs/notes/ui-implementation-strategy.md:3`；`.claude/commands/arch-change.md:24`；`.claude/agents/architect.md:23/:52`；`.claude/agents/architect-reviewer.md:42/:146`；`.claude/agents/integrator.md:42/:71`；`.claude/scripts/pre-merge-escalation-gate.mjs:102/:127/:246-249/:437`；`.claude/rules/repo-structure.md:17/:32`（T4）；`.claude/scope-map.json:33`（T4）；`.delivery/milestones.jsonl:1-7`（acceptanceRef 字段 ×7）；`.delivery/delivery-workspace.json:6`。
- **豁免·冻结验收件/实现面注释**：`firmware/main/test/key_route_a1.acceptance.c:603/:636-637/:672`；`firmware/main/test/screen_unification.acceptance.c:1010`；`server/acceptance/test_schedule_reminder.acceptance.py:1561`；`server/acceptance/test_smoke_e2e.acceptance.py:5/:60/:124/:128/:142`；`server/acceptance/test_cloud_link.acceptance.py:5`；`firmware/main/key_gesture_parser.h:13`；`firmware/main/session_state_machine.h:13`。
- **豁免·通用示例/通用机制描述**：`.claude/agents/planner.md:155/:178`；`.claude/scripts/enforce-scope-canary.mjs:25`；`.claude/commands/review-arch.md:3`；`.claude/references/task-intake-and-slicing.md:108`；`.claude/references/architect-operating-model.md:79`（R3-F14 归此）；`docs/templates/AGENTS.template.md:2/:9/:17/:94`；`docs/templates/system-spec.template.md:2`（旧 docs/systems 结构卫生、§6 留根）。
- **豁免·append-only 历史行**：`docs/ARCH-REVIEW-QUEUE.md:49`；`.claude/rules/contracts.md:14`。
- **豁免·资产母版**：`docs/ui/kidpal-ui-design-v2.html` ×1、`docs/ui/kidpal-ui-design-v3.html` ×1。
- **豁免·第三方供应商/构建产物**：vendor `lv_font_fmt_txt.h`、`managed_components/**`、`__pycache__`。
- **豁免·归档/历史/待归档/契约**：`changes/**`、`.claude/taskpackages/**`、`.planning/**`、`docs/contracts/interface-contract.md:4`（F2 挂账）。
- **派生视图（重生成、不手改）**：`.delivery/STATUS.md` ×7。

要点：① 改指集 12 文件（含 R2 未预见的 escalation-gate 字面集消费与 integrator 写作约定联动，及 R3 补扫的 .delivery 两件）；② 冻结验收件存在性断言 + 直读 = spec 归档后真实翻红的已知机械后果（T5 登记、挂 CDQ、phase 2 处置）；③ templates 目录 PROPOSAL/retro.template 零命中（R2-F8）；④ CDQ 零命中（R2-F11）。语义引用模式（`spec §` / 旧节号）live 面命中集中于上述文件同段行，无独立新目标文件。

## 4. human 取舍点（OQ）

| OQ | 问题 | 选项 | 推荐 |
|---|---|---|---|
| OQ-1 | spec.md 去向 | (a) 删除 / (b) 留导航 stub | **已定（2026-09-13 human，F3 裁定中给出的第三形态）= 归档 `docs/archive/spec.md`**——旧批对应老 spec 一起归档，新结构全新启用；删除与 stub 均不取 |
| OQ-2 | 初始可行性标定授权 | (a) 主线程按「现有证据推导」规则初标 + 逐条清单进 landing-summary 供 human 复核 / (b) 逐条 human 过目后再落 | **已定（2026-09-13 human）= (b) 逐条 human 过目**——主线程备逐条标定清单（每条带证据行号），human 过目后方落文件（T1 初始标定规则已同步） |
| OQ-3 | NFR 未拍数值的阈值（如续航） | (a) 原样带「待定」标迁入 / (b) 本次顺带拍数值 | **已定（2026-09-13 human）= (a)**——拆分不夹带新决策，数值拍板走后续立项 |
| OQ-4 | 机检脚本拦阻强度 | (a) 先手动/review 时跑 / (b) 挂 hook 硬拦 | **已定（2026-09-13 human）= (a)**——YAGNI，跑出误报率后再议升级 |

（区外已定项，不再裁：边界划分、story 级锚 + 机检、UI 锚规则、NFR 独立成章、符合度归 phase-2——均经 2026-09-12/13 brainstorming 逐点裁定。）

## 5. 验证集（landing 前机械前置 + landing 后核验）

DRAFT 期列骨架，GREEN 前补命令与预期全文：

1. **时序前置断言**：landing 开始前确认 `.planning` 旧批已收尾归档（human 定向独立操作，不在本 packet），未归档则 landing 暂停（human 2026-09-13 时序裁定）。
2. `node .claude/scripts/spec-anchor-check.mjs` 对拆分后两文件跑全绿（双锚 + 🔴 挂账）——含真空绿规约（先断言两文件非空、story 计数 > 0）。
3. `node .claude/scripts/spec-anchor-check.mjs --reverse` 输出非空且抽样人读核对正确。
4. 引用 census 复扫（模式 = 文件名 + 语义引用，见 §3）：live 面零残留——豁免面 = `docs/contracts/**`（F2 挂账）、`changes/**` 归档、`.claude/taskpackages/**` 历史、归档后 `.planning/**` 旧批。
5. `node .claude/scripts/changes-index.mjs --upsert 260913-arch-review-spec-split` 单行钉进 INDEX（DRAFT 期已执行一次，状态转移时再 upsert）。
6. **escalation-gate 改后回归**：`.claude/scripts/pre-merge-escalation-gate.mjs` 字面集扩展后，其内嵌 contract-findings 用例（:437 区域）须含新路径形态且全绿——自检入口形态（`--self-check` 或等价）landing 时按脚本 `--help`/源码实况核实后执行，未实测不虚构入口名；integrator.md:42 写作约定与闸字面集的一致性人工比对。
7. **`.delivery/` 重生成核验**：milestones.jsonl / delivery-workspace.json 改指后重跑 delivery-status.mjs（入口形态 landing 时按脚本实况核实），STATUS.md 重生成且 ref 链指向 requirements.md，无 spec.md 残留。
8. landing-verifier 独立核验：`landing-fidelity-check.mjs` 结构门 + 逐文件人读 diff 核对落地 == 本 PROPOSAL（重点：迁移内容零丢失——两新文件节集合 ∪ 已砍声明区 ∪ 「已结不迁」逐条清单 ≡ spec.md 九节全集）。

## 6. 留根清单

- **包级 story 内嵌规则**（「TaskPackage story 字段必须内嵌业务 story 原文」）：归 260912-gov-review-portfolio-decomposition-chain session。**交接点（本变更提供）**：story 锚点格式 = requirements.md 里稳定的 `S-Fx.y` 编号 + 节标题锚；那边规则消费此锚，两边并行不阻塞。
- **实现符合度审计**：归 phase-2 workstream PROPOSAL（一次性「story × 现状 × 处置：保留/重修/重切」审计表），本变更不落。
- **NFR 阈值数值拍板**（续航等「待定」项）：后续立项。
- **机检脚本 hook 硬拦升级**：跑出误报率后经 OQ 再议。
- **契约 CDQ-260902 在办事项**（§三推送重写、§四协议剖面）：原样继续，不受本变更影响（契约地址不变）。
- **契约 :4 断链挂账**（F2 裁 (a)）：`docs/contracts/interface-contract.md:4` 对 `../spec.md` 的引用在 spec 归档后成断链，landing 时登记进 CDQ、随 CDQ-260902 契约门重写一并改指；本批契约面零接触。
- **冻结验收件与实现面注释的 spec 引用**（census 实证）：key_route_a1 / screen_unification 的 spec 存在性 canary、schedule_reminder 直读、key_gesture_parser.h / session_state_machine.h 权威指针注释——归档后翻红/过期为已知后果（挂 CDQ），处置归 phase 2 验收基线重建与首批 dev 批，本批不改冻结件。
- **设计稿 HTML 的 spec 引用**（v2/v3 各 1 处）：设计稿为资产管线母版，v4 迭代时顺带改，不为此单独动母版。
- **system-spec.template.md 旧结构卫生**（R2-F8 附带发现）：引 docs/systems/ 旧结构，属模板卫生问题另办，不搭本批。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-13 · architect

**结论：打回**。模板形制（硬项 2a 锚清单、`### T<n>` 标题形态、landing-manifest 围栏块一行一裸路径、manifest ≡ §2 并集且对 census 扩列有诚实标注）合规；§1 多数证据断言经判官亲核属实（见末尾亲核清单）。但有 3 项影响目标效果的实质缺口（F1/F2/F3，按纪律标注升级 human）与 3 项起草方可自修的精度项（F4/F5/F7）。

**F1 · §6.3「待定/已定跟踪」无迁移去向（升级 human）**。T2 只点名 §6.1/6.2 的处置（重叠内容随 CDQ-260902 归契约、协议选型留技术决策节），未覆盖 §6.3（spec.md:561–568）七条跟踪项。其中含独有内容：「设备服务器地址入口 = 配网直写 NVS（namespace `websocket` 键 url/token/version）」（:565）、「ASR 云额度与 provider 终选 = 部署期首验点」（:567）、「多轮会话连续性首验 + ARQ-260902-multiturn-continuity 挂账」（:568）——这些不是契约重叠、也不是 §7 编号决策的原文重复，按现 T2 枚举无落点，直接威胁 §5 #5 自立的「迁移零丢失」核验点。建议修法：T2 显式给 §6.3 一个去向（如 architecture.md 契约层节内保留跟踪表，或拆入技术决策节），并在 §5 #5 补一张逐节映射表（spec §x.y → 目标文件小节）作 landing 制品。

**F2 · 契约面内含 live 引用，与「契约零接触」非目标直接冲突（升级 human）**。`docs/contracts/interface-contract.md:4` 逐字为「> 配合 [docs/spec.md](../spec.md) §3.0.2（D3 笔顺格式已定稿）使用。」——契约本体就是 spec.md 的引用方。改它 = 触 `docs/contracts/**` = 契约门（且可能命中 Track A 升级判据①）；不改它 = §5 验证 #3「live 面零残留」必红。本 PROPOSAL 的非目标（契约零接触）与验证集在此自相矛盾，且触及路由判定本身，须 human 裁处置路径（候选：走 architect-reviewer 四分类「随 spec 改动携带」同 packet 带这一行链接改址；或契约内该行显式留旧、记账 CDQ）。

**F3 · census 复扫模式集不完备，且历史/证据面豁免口径未定义（升级 human）**。§3 sweep 义务与 §5 #3 只以文件名 `spec.md` 为枚举 pattern。判官实测：`grep -rln 'spec\.md'`（排除 `.git`/node_modules）命中 100+ 文件，其中 `.planning/kidpal-mvp/decision-coverage.json` **不含** "spec.md" 字样、文件名 census 会漏，但它以「spec §4 全部 story」「spec v0.2」「§4.0 共享底座 + §4.4 F4」「§6.1 协议剖面」「§7 决策 18–28」「§5.6/§5.7」等**旧节号语义引用**深度绑定 spec 结构（该文件 :9–12、:60）——拆分后这些节号全部悬空。另有两类豁免口径 PROPOSAL 未定义：① `.claude/taskpackages/**/verifier-evidence/rounds/*.json`（历史证据轮次，含 spec.md 行号引用，append-only 纪律下应豁免但未写明——T5 只豁免了 `changes/` 归档）；② `changes/dev/**`（同为归档，§3 的「排除 changes/」写了但 T5 正文只说「changes/ 归档」，需对齐）。按硬项 1c，census 模式集须显式枚举（至少：`spec\.md` × `spec §` × `spec v0\.` × `docs/spec`），并逐类给处置口径（live 改指 / 历史冻结豁免）。`.planning/` 制品（decision-coverage.json、review.md、task_plan.md、progress.md、PROPOSAL-v0.3/v0.4-draft.md）算 live 还是历史，需 human 裁。

**F4 · §5.5 源清点不实（不升级，起草方可修）**。T1 §3 称「该 feature 的页面线框（原 §5.5 拆散、随 feature 走，P1/P2 归 F1、P3 归 F2、P4 归 F3）」。实证：spec.md §5.5（:512–522）**只有 P1 首页线框一件**，P2/P3/P4 线框不存在——§5.6 末行（:530）自证「待补：各页面完整线框图（v2 趣味版生成中）」。「拆散 + 四页分配」的措辞虚构了源库存。意图（线框随 feature 落）可保留，建议改为「原 §5.5 仅 P1 一件、归 F1；P2–P4 线框为待补项，补齐时按本规则随 feature 落」。

**F5 · 证据精度四项（不升级，起草方可修）**：
- a) P3 引「字号下限待 1bpp 真机可读性实测」标（:529、:602）——该句逐字只在 :602；:529 是「暗光问题：反射屏暗处不可见」条（同属可读性域但非所引句）。建议拆开引用。
- b) P1 所引业务 story 原文截断在子句中间（:318 实文尾部尚有「；例行去上的学校课我自己看课表就行。」）——已标「verbatim 片段」可接受，建议补省略号或截在句号。
- c) T1 头部「已砍声明区（S-F2.5 云端润色、两处反白高亮，现 spec.md §4 头部声明区原样带走）」——实证 §4 头部声明区（:256）只含两处反白高亮裁定，**不含 S-F2.5**；且 S-F2.5 在 spec 中是「（可选）」标注（:394）、decision-coverage 记为 MVP 后置 dropped，不是「已砍」。措辞需修。
- d) P2「spec.md §5.6 教学字体红线节自身已引 v3」——:527 只有「（v3）」版本标签，文件链接指向 brief §3.3 而非 v3 设计稿本身，表述偏松。另 §4 节标题（:250）「与 UI 设计稿 v2 对齐」是第二处 v2 漂移实例，可补作证据。

**F6 · census 扩列的可预见大头未在 §3 预告（随 F3 一并处置，不单独升级）**。判官 spot-census 已见：`AGENTS.md:7`（**G1 根**，「系统 spec 面 = `docs/spec.md`」）、`.claude/rules/contracts.md`（实况注记）、`docs/templates/AGENTS.template.md` 与 `system-spec.template.md`、`.claude/agents/architect.md / architect-reviewer.md / integrator.md / planner.md`、`.claude/commands/arch-change.md / review-arch.md`、`.claude/references/architect-operating-model.md / task-intake-and-slicing.md`、`.claude/scripts/enforce-scope-canary.mjs / pre-merge-escalation-gate.mjs`、`docs/DEV-GOVERNANCE.md`、`docs/ARCH-REVIEW-QUEUE.md`、`docs/notes/ui-implementation-strategy.md`、`docs/ui/kidpal-ui-design-v2/v3.html`、`.delivery/STATUS.md` 等。其中 AGENTS.md 是 G1（§3 影响表需显式分类、profile 行需反映 G1 触面）；`.claude/agents/architect.md` 属判官自身定义（改动走显式 human-approved 自改递归纪律）；两个 template 不改则未来再生成时 drift 回潮。census 正式跑完后 §3 表与 landing-manifest 须相应扩列。

**F7 · 「端云」标注废弃的作用域歧义（不升级，起草方可修）**。T1 新增一宣布 story 级「端云」标注废弃、由支撑锚吸收，但 §4.0 共享底座表（:260–269）也有「端云」列（`[板]`/`[云]`/`[板+云]`），迁入 requirements §2 共享底座后该列存废未说。建议明示（同废并在底座表以支撑锚承接，或底座表保留）。

**设计自洽与可行性确认项（无问题，记录在案）**：T1–T5 编号/职责无相互矛盾（端云废弃后 T3 三检不再依赖端云标注 ✓；§6 交接点的 `S-Fx.y` 编号 + 节标题锚与 T1 §3 编号保留一致 ✓）；scope-map.json `universal.docsTopLevel.allowFiles` 机制实证存在且被 `enforce-scope.mjs:56–66` 消费，T4 改法机械可行 ✓；§5 骨架遵守真空绿规约（#1 先断言两文件非空 + story 计数 > 0）✓；`changes-index.mjs`、`landing-fidelity-check.mjs` 入口实证存在 ✓；S-Fx.y story 编号保留使 decision-coverage.json 的 story 键兼容 ✓（其旧节号引用问题见 F3）；NFR 章「同一约束全仓只定义一次 + story 回引 NFR ID」规则闭环 ✓；v2→v3 `data-od-id` 实证比 PROPOSAL 声称更强：两文件 id 集合各 61 件、**全量逐字相等**（comm 双向差集为空），T1 规约第 5 条的实证基座成立 ✓。

**判官亲核清单（2026-09-13 实测/实读）**：
1. `wc -l docs/spec.md` = 644 ✓（§0「644 行」属实）；§1–§9 九节结构 ✓（标题枚举实测）。
2. spec.md:252 引文「story 与 [kidpal-ui-design-v2.html](ui/kidpal-ui-design-v2.html) 一一对应」逐字属实 ✓。
3. spec.md:263「超 4 秒先开口安抚」在 §4.0 底座表 ✓；:595=决策 21（功耗/续航 DEFER）✓；:596=决策 22（传输安全）✓；:598=决策 24（离线边界）✓；:602=决策 28（内容安全姿态 V30 + 字号下限）✓；:529 为暗光问题条（**P3 对此行的引文错位，见 F5-a**）。
4. spec.md:318 story 原文引文为 verbatim 前缀片段（截断点见 F5-b）。
5. `.claude/taskpackages/260829-schedule-reminder/taskpackage.json`：`story` 字段所引片段逐字属实 ✓、全字段无「作为…我希望…以便…」业务框架 ✓；`origin.upstreamRef` = `.planning/kidpal-mvp/PROPOSAL.md` ✓。
6. `ls docs/ui/`：`kidpal-ui-design-v3.html` 存在 ✓（v2 文件亦仍在目录中，PROPOSAL 未声称其删除，无误）。
7. v2/v3 `data-od-id` 双文件比对：各 61 个、集合完全相等 ✓（比 T1 规约第 5 条声称的「id 族逐字保留」更强）。
8. `.claude/scope-map.json` `universal.docsTopLevel.allowFiles` 白名单机制存在、含 `docs/spec.md` ✓；`enforce-scope.mjs:56–66` 实读确认钩子按它机判 ✓。
9. `.planning/kidpal-mvp/PROPOSAL.md:6`「上游权威：[spec.md](../../docs/spec.md) v0.2 FROZEN」逐字属实 ✓（同文件 :3、:5 另有两处 spec.md 引用，census 可捕获）。
10. `docs/contracts/interface-contract.md:4` 引 `../spec.md` §3.0.2 ✓（F2 的实证基座）。
11. `.planning/kidpal-mvp/decision-coverage.json` 不含 "spec.md" 字样但含旧节号语义引用 ✓（F3 的实证基座）。
12. `.claude/scripts/changes-index.mjs`、`landing-fidelity-check.mjs` 存在 ✓。
13. spec.md:256 §4 头部已砍声明区仅含两处反白高亮、不含 S-F2.5 ✓（F5-c 基座）；S-F2.5 在 :394 标「（可选）」。
14. spec.md §5.5（:512–522）仅 P1 线框一件；:530「待补：各页面完整线框图」✓（F4 基座）。

### R2 · 2026-09-13 · architect

**结论：打回（窄幅，全部机械可修；无新增升级 human 项）**。R1 三项升级项（F1/F2/F3）已按 human 裁定回灌且四处一致，R1 自修项（F4/F5/F7）全部核实闭环。剩余四项：两项实质（F8 模板身份错位、F9 census 未跑）+ 两项措辞（F10/F11），修完可直接 R3 直判。

**R1 处置对账（逐条核验结果）**：

- **F1 ✓ 闭环**：T2 给出 §6.3 完整处置（不单列节；已实测闭环项标「已结」不迁入、随归档消亡；仍开放项 landing 时逐项核证据、进技术开放问题节；逐条清单进 landing-summary）；§5 #6 核验方程补入「已结不迁」逐条清单，零丢失核验点重新闭合。闭环判定（配网 NVS = 配网已成功、ASR 首验 = live 冒烟已跑）有 human 口径在案。
- **F2 ✓ 闭环**：四处一致——T5 豁免 `docs/contracts/**`、§3 表「未触；:4 断链挂账 CDQ」、§5 #4 豁免面含 contracts、§6 留根挂账条目。契约零接触与「live 零残留」不再自相矛盾（挂账路径明确：随 CDQ-260902 契约门重写一并改指）。
- **F3 ✓ 闭环**：归档形态（移 `docs/archive/spec.md` 不删）+ 时序前置断言（§5 #1，landing 前确认 `.planning` 旧批已归档）+ census 模式集（文件名 + `spec ?§` + 旧节号枚举 §4.0–4.4/§5.1–5.7/§6.1–6.3/§7 决策/§8/§9）+ 豁免四面（`changes/**`、`.claude/taskpackages/**`、归档后 `.planning/**`、`docs/contracts/**`）。decision-coverage.json 悬空由归档消解，与 manifest 移除 `.planning/kidpal-mvp/PROPOSAL.md` 一致。
- **F4 ✓ 闭环**：T1 §3 改为源库存如实——实存仅 P1 一件（:512–522）归 F1，P2–P4 挂「线框待补」占位标，不虚构库存。
- **F5 ✓ 四项全闭环**：a) 可读性两注拆开（字号下限 :602 / 暗光 :529）；b) :318 引文改全句 verbatim——判官复核逐字属实（含尾句「；例行去上的学校课我自己看课表就行。」）；c) 头部改为「:256 声明区（= 两处反白高亮）原样带走 + S-F2.5 由 :394『（可选）』标注转为已砍清单新条目」，与实况一致；d) v3 措辞收紧为「标题已带『（v3）』版本标签，:527」。
- **F6 △ 部分闭环**：AGENTS.md（:7 逐字核实「系统 spec 面 = `docs/spec.md`」）与 `.claude/agents/architect.md`（:23/:52 逐字核实含 `docs/spec.md` 引用；G2·自改递归、human 知情批准 F6 裁 (a)）处置正确、manifest 在列；**模板部分错位 → 新 F8**。
- **F7 ✓ 闭环**：T1 新增一明确全量废弃——story 锚行 + 共享底座表「端云」列一并废弃，底座节级改钉支撑锚，作用域歧义消除。

**新打回项**：

**F8 · manifest/T5/§3 模板两件身份错位（起草方可修，不升级）**。manifest 与 T5 目标文件列 `docs/templates/PROPOSAL.template.md`、`retro.template.md`，理由「引用改指，防 drift 回潮（F6 裁 (a)）」。判官实测：`grep -n 'spec\.md|docs/spec' docs/templates/**`——**PROPOSAL.template.md 与 retro.template.md 零命中**，两件没有任何 spec.md 引用可改指，「防 drift 回潮」的入列理由不成立，应从 manifest/T5/§3 移除（或改陈真实改动理由）。同时判官自我修正 R1-F6 的一处误导：该点名所据的 `AGENTS.template.md`（:2/:9/:17/:94）与 `system-spec.template.md`（:2）命中属实，但语义复核显示其 "spec.md" 是**通用模块级占位**（verbatim：「产品需求见同目录 `spec.md`」「复制到 docs/systems/Sx-name/spec.md 后填写」），指模块级 spec 概念、不绑 KidPal 的 `docs/spec.md`，本批归档不给它们制造断链——**这两件同样不需要进本 packet**。system-spec.template.md 引 `docs/systems/` 旧结构属陈旧模板卫生问题，建议 §6 留根、另行处理，不夹带本批。

**F9 · census 仍未跑，与 §3 自立「GREEN 前补齐」承诺冲突（机械义务，不升级）**。§3 sweep 段 v0.2 明文「本 DRAFT 未跑，GREEN 前补齐」，本轮仍未兑现——而 F8 恰是 census 缺位的直接产物（模式集真跑一遍就会暴露模板两件零命中、manifest 成员失真）。GREEN 前须：按 §3 已定义模式集（文件名 + `spec ?§` + 旧节号枚举）全仓实跑，命令与原始输出进 §3，manifest/§3 表按结果定稿（可预见扩列头：`docs/DEV-GOVERNANCE.md`、`docs/ARCH-REVIEW-QUEUE.md`、`.claude/agents/planner.md / integrator.md / architect-reviewer.md`、`.claude/commands/arch-change.md / review-arch.md`、`.claude/references/` 两件、`.claude/rules/contracts.md`、`.claude/scripts/enforce-scope-canary.mjs / pre-merge-escalation-gate.mjs`、`docs/notes/ui-implementation-strategy.md`、`.delivery/STATUS.md`——其中属豁免面者逐件给豁免归类，不静默漏）。

**F10 · profile 行未反映 G1/自改递归触面（措辞小项）**。头部 profile 仍写「spec profile…触面 = docs/spec.md 拓扑，契约面触面为零」，而 packet 实触 `AGENTS.md`（G1）与 `.claude/agents/architect.md`（自改递归）。实质升级已完成（human F6 裁 (a) 知情批准、§3 显式分类），但 FROZEN 制品头部与 §3 表述会不一致，建议 profile 行补注「+ G1（AGENTS.md:7 一行）+ 判官自改递归（architect.md），human F6 裁 (a) 知情批准」。

**F11 · CDQ「改指」用词不实（措辞小项）**。实测 `docs/CONTRACT-DECISION-QUEUE.md` 零 spec.md 引用（grep 零命中）。其入 manifest 的真实理由 = 契约 :4 断链**挂账登记新条目**（F2 裁 (a)）+ §5 #5 提及的 upsert，非引用改指。T5 目标文件行与 §3 表的「改指 + 挂账」应改为「挂账登记」。文件本身留在 manifest 正确。

**新矛盾检查（v0.2 新增内容）**：

- **manifest ≡ §2 并集**：形式成立——T1–T5 目标文件清单与 manifest 11 行逐一对得上，迁移锚只记迁入路径（`docs/archive/spec.md`，不记 `docs/spec.md`）✓、一行一裸路径无括注 ✓；但成员集因 F8 失真，census 定稿后复核。
- **T4 与归档形态咬合 ✓**：`docs/archive/` 是 repo-structure 已登记子目录（「docs/ 根白名单」子目录行含 `archive/`），spec.md 迁入机械可行；scope-map `allowFiles` 移除 `docs/spec.md` 与 repo-structure 白名单/「spec 唯一」行两处一致改写无矛盾；归档形态反而消解了 OQ-1 原先「stub 例外条款」的顾虑。
- **时序断言 ✓**：§5 #1（landing 前确认 `.planning` 旧批已归档，未归档暂停）与 Status 行时序注记、T5 时序段三处一致；`.planning` 豁免依赖该前置成立，逻辑自洽。
- **OQ ✓**：OQ-1 已定（归档，第三形态，覆盖原推荐）、与 T5 一致；OQ-2/3/4 待裁属 human 流程内，不阻塞 GREEN 判定。
- **§5 #6 核验方程**：补入「已结不迁」清单后，「两新文件节集合 ∪ 已砍声明区 ∪ 已结不迁清单 ≡ spec.md 九节全集」重新机械可判 ✓。

**R2 亲核清单（2026-09-13 实测/实读）**：① `grep -n 'spec\.md|docs/spec' docs/templates/**`——仅 AGENTS.template.md（:2/:9/:17/:94）与 system-spec.template.md（:2）命中，PROPOSAL.template.md / retro.template.md / 其余模板零命中（F8 基座）；② `grep -n 'spec\.md' docs/CONTRACT-DECISION-QUEUE.md` 零命中（F11 基座）；③ `AGENTS.md:7` 逐字 =「系统 spec 面 = `docs/spec.md`（项目 = KidPal，docs 平铺、不再套 `systems/<sys>/` 层）。」（T5 引述属实）；④ `.claude/agents/architect.md:23/:52` 含 `docs/spec.md` 引用属实（自改递归入列有据）；⑤ `ls docs/templates/` 确认 retro.template.md、PROPOSAL.template.md 实存（manifest 成员非虚构路径）；⑥ spec.md:318 全句与 P1 引文逐字比对一致；⑦ repo-structure.md「docs/ 根白名单」子目录行含 `archive/`（T4/归档咬合基座）。

### R3 · 2026-09-13 · architect

**结论：打回（一次窄幅收尾，全部机械可修；无升级 human 项）**。R2 四项（F8–F11）中三项（F8/F10/F11）核实闭环，F9 形式已兑现（census 已跑）但**结果有漏**——`.delivery/` 面（判官 R1-F6、R2-F9 可预见头两轮点名）漏网，且 census 原始输出未落 §3（硬项 1b 形式缺口），使「改指集即全集、无待定项」的声明不成立。census 回灌的其余新内容（escalation-gate 字面集联动、冻结验收件已知后果登记、新增改指面）逐项亲核属实、方向正确。

**R2 处置对账**：

- **F8 ✓ 闭环**：manifest 移除 PROPOSAL.template.md / retro.template.md（判官复核零命中）；AGENTS.template ×4 / system-spec.template ×1 归豁免②通用占位，system-spec.template 旧结构卫生 §6 留根——按 R2 判官自正口径处理。
- **F9 △ 形式闭环、结果有漏**：census 确已实跑且抓出的四类实况（escalation-gate 字面集消费、冻结验收件 canary、新增改指面、豁免归类）经判官逐项亲核属实（见末尾清单）；但 `.delivery/` 三件漏网（→ 新 F12）+ 原始输出只存会话记录未落 §3（→ 新 F13），「改指集 10 文件全集、无待定项」声明不成立。
- **F10 ✓ 闭环**：头部 review profile 行已补 G1（AGENTS.md:7 + DEV-GOVERNANCE.md:241）/ G3（scope-map + escalation-gate 字面集）/ 自改递归 ×3（architect / architect-reviewer / integrator）触面注记，human 知情批准在注——与 §3 表一致。
- **F11 ✓ 闭环**：T5/§3 明写 CDQ 实测零引用、入 manifest 理由 = 挂账登记新条目（契约断链 + 冻结验收件翻红），非改指。

**新打回项**：

**F12 · `.delivery/` 面 census 漏网（实质项，处置方向机械明确，不升级）**。判官实测三件，均在改指集与豁免五类之外：
1. `.delivery/milestones.jsonl` ×7 条 `"acceptanceRef": "docs/spec.md"`——这是**机器消费的单一事实源字段**：`delivery-status.mjs:4` 明注「单一事实源 = milestones.jsonl；STATUS.md 是派生只读视图（勿手改）」、`:66` 将 acceptanceRef 渲染成 STATUS.md 链接。spec 归档后 7 条指空，重跑脚本即生成死链视图。
2. `.delivery/delivery-workspace.json:6`「里程碑定义权威 = docs/spec.md §8（M0-M6）」——语义权威指针，spec §8 → requirements.md §6 路线图，须改指。
3. `.delivery/STATUS.md` ×7 处 `[ref](../docs/spec.md)`——派生物，**不应手改**，正确改法 = 改 milestones.jsonl 的 acceptanceRef + 重跑 `node .claude/scripts/delivery-status.mjs` 再生成（此点须写进 T5，防执行者直接编辑派生视图）。
后果链：§5 #4 复扫按「live 面零残留」口径会红——packet 现态过不了自己的验证集。处置建议：M0–M6 acceptanceRef 改指 `docs/requirements.md`（路线图权威）；manifest 增列 milestones.jsonl + delivery-workspace.json（STATUS.md 为派生物，随脚本再生成，入列与否由起草方定并在 T5 注明）。流程观察（登记不阻断）：该面在 R1-F6 与 R2-F9 可预见头两轮点名后仍漏。

**F13 · census 原始输出未进 §3 证据段（硬项 1b 形式项）**。§3 现写「原始输出存证于本会话记录」——模板硬项 1b 要求原始输出（或摘要 + 命令 + 日期）**进本节证据段**；判官无法复核会话内记录的完备性，F12 正是从这个通道溜过的（输出不落盘，漏网不可见）。修法：§3 落按文件分组的命中清单摘要 + 命令 + 日期；F12 三件随之显形。

**F14 · 归类收尾三小项（不改变结论，顺手修）**：① `architect-operating-model.md:79`（「绕过 review 直写既有 `docs/systems` spec.md 内容」）——通用治理模型语、不绑本仓 docs/spec.md，应归豁免②（现遗漏未归类）；② vendor / managed_components / tools/simulator 的 `lv_font_fmt_txt.h` 含 "spec.md" 字样（LVGL 供应商文件）及 `server/acceptance/__pycache__/*.pyc` 构建产物——建议豁免类补「第三方供应商 / managed_components / 构建产物」一条（现遗漏）；③ T5 豁免①的 `key_gesture_parser.h:13` / `session_state_machine.h:13` 实际路径在 `firmware/main/`（判官实测命中 `firmware/main/key_gesture_parser.h`），补全路径防歧义。

**census 回灌新内容自洽核验（复审重点②，全部通过）**：

- **escalation-gate 字面集扩展方向正确**：判官亲读 `.claude/scripts/pre-merge-escalation-gate.mjs:102/:127/:246-249/:437`——CDQ 核验确实按 `docs/spec.md` 字面比对 findings Basis（:127 filter 逐字核实在案），spec 改址后 Basis 引新路径的 hard-violation 将漏拦，扩展字面集 = +requirements.md +architecture.md、保留归档件字面兼容历史 findings，设计正确；`:437` 自检用例区含 `docs/spec.md` 形态，§5 #6 要求扩展后含新路径形态 + 全绿 = 对症的回归设计。与 `integrator.md:42`（Basis 前缀写作约定「须以 `docs/contracts/**` 或 `docs/spec.md` 字面前缀起引」逐字亲核）+ `:71`（消解不改清单含 `docs/spec.md` 逐字亲核）同批联动改——「只改一边 = 闸与约定脱钩」的联动逻辑成立，两处均已入 manifest。
- **冻结验收件「不改 + 挂账」处置符合红线**：`integrator.md:71` 自证冻结验收件红线存在（消解不改 `*.acceptance.*`）；key_route_a1.acceptance.c:636-637（「KR-E2 RED(infra): docs/spec.md 丢失」逐字亲核，另 :603/:672 同族）、screen_unification.acceptance.c:1010、server/acceptance/test_schedule_reminder.acceptance.py:1561（`_read_text(REPO_ROOT / "docs" / "spec.md")` 逐字亲核）三处行号全部属实——归档后真实翻红 / FileNotFoundError 是准确预判，「已知机械后果 + 挂 CDQ + 处置归 phase 2 验收基线重建 + 本批不改冻结件」与红线、§6 留根一致。escalation-gate 改法与归档同 commit 原子落地的要求正确（防中间态漏拦窗口）。
- **新增改指面行号全部属实**：DEV-GOVERNANCE.md:241（治理路由表「系统 spec（KidPal：`docs/spec.md`）」行，G1 分类正确）、arch-change.md:24、architect-reviewer.md:42/:146、ui-implementation-strategy.md:3（「spec 侧索引见 [spec.md §5.7]」）逐行亲核。
- **豁免归类抽样属实**：planner.md:155/:178、enforce-scope-canary.mjs:25、review-arch.md:3、task-intake-and-slicing.md:108 = `docs/systems/Sx-name/spec.md` 模块级通用占位（逐行亲核）；ARCH-REVIEW-QUEUE.md:49（ARQ 巨行历史记录）、rules/contracts.md:14（2026-08-27 归位注记）= append-only 历史行，豁免正当。
- **manifest ≡ §2 并集复核成立**：15 行逐一对账——T1（requirements.md）+ T2（architecture.md）+ T3（spec-anchor-check.mjs）+ T4（scope-map.json、repo-structure.md）+ T5（archive/spec.md、CDQ、DEV-GOVERNANCE.md、ui-implementation-strategy.md、AGENTS.md、arch-change.md、architect.md、architect-reviewer.md、integrator.md、escalation-gate.mjs）= 15，一行一裸路径、迁移锚只记迁入路径 ✓（F12 修复后增列 .delivery 两件，须同步保持并集成立）。

**R3 亲核清单（2026-09-13 实测/实读）**：① escalation-gate.mjs 五处行号 + 字面串逐行亲读；② integrator.md:42 写作约定 + :71 消解边界逐字亲读；③ DEV-GOVERNANCE.md:241 / arch-change.md:24 / architect-reviewer.md:42/:146 / ui-implementation-strategy.md:3 逐行亲读；④ 豁免②③八处引用逐行亲读（planner ×2 / canary / review-arch / task-intake / ARQ:49 / contracts.md:14 / templates ×5 由 R2 复核承袭）；⑤ 冻结验收件三件 + key_gesture_parser.h / session_state_machine.h 命中实测（`grep -rln` firmware/ server/ tools/）；⑥ `.delivery/` 三件漏网实测（milestones.jsonl ×7 acceptanceRef、delivery-workspace.json:6、STATUS.md ×7 链接）；⑦ delivery-status.mjs:4「勿手改」+ :66 acceptanceRef 渲染消费链亲读；⑧ architect-operating-model.md:79 亲读（docs/systems 通用语）；⑨ manifest 15 行 ↔ §2 各 T 锚清单逐一对账；⑩ profile 行注记齐全性复核。

修完 F12/F13/F14（预计 = .delivery 三件归类改法 + §3 落 census 输出摘要 + 三处归类补注，纯机械）可直接 R4 直判 GREEN。

### R4 · 2026-09-13 · architect

**结论：GREEN（附 2 项建议级修正，建议 FROZEN 前回灌；均为文字级，不动 manifest / 锚清单 / 验证集，不构成新评审轮）**。R3 三项（F12/F13/F14）全部核实闭环；manifest 17 行 ≡ §2 并集逐一对账成立；§3 命中清单与判官两轮独立 census（R1 全仓 grep + R3 补扫，grep 不受任何 ignore 影响）逐类对账闭合、无缺漏。R1–R3 已确认的设计自洽项（T1–T5 无矛盾、锚机制、验证集真空绿、§6.3/§6 交接、归档咬合、时序断言）在 v0.4 均维持。

**R3 处置对账（逐条核验）**：

- **F12 ✓ 闭环**：T5 新增 `.delivery/` 块（:137）——milestones.jsonl 七条 acceptanceRef 直改源（SSOT 字段改指 requirements.md）、delivery-workspace.json:6 权威指针改指 requirements.md §6、STATUS.md 明写「派生视图不手改、改源重跑 delivery-status.mjs 重生成」，派生纪律正确入文；manifest 增列两件（:28/:29）；§3 表新增 delivery 工作区行（:163）；§5 新增 #7 重生成核验（:204，含「入口形态 landing 时按脚本实况核实」的未实测不虚构纪律）。
- **F13 ✓ 闭环**：§3 sweep 段（:170–181）现为按文件分组的完整命中清单——改指 12 文件逐行号 + 七类豁免逐行号 + 派生视图单列，命令三趟 + 日期齐全。判官对账：与我 R1 全仓 grep（100+ 命中文件，排除豁免面后）+ R3 补扫两次独立枚举**逐类闭合**——改指 12、豁免①冻结验收件/实现面（key_route_a1:603/:636-637/:672、screen_unification:1010、schedule_reminder:1561、smoke_e2e:5/:60/:124/:128/:142、cloud_link:5、key_gesture_parser.h:13、session_state_machine.h:13——本次逐行号亲核属实）、豁免②通用占位（含 R3-F14 补的 architect-operating-model.md:79）、豁免③ append-only（ARQ:49、contracts.md:14）、豁免④资产母版、豁免⑤供应商/构建产物（新增类）、豁免⑥归档/历史/待归档/契约、派生视图（STATUS.md ×7）——无未分类命中。
- **F14 ✓ 三小项全修**：architect-operating-model.md:79 归豁免②（:174）；豁免⑤第三方供应商/构建产物新增（:139/:177）；key_gesture_parser.h / session_state_machine.h 路径补全 `firmware/main/` 前缀（:139）。

**manifest ≡ §2 并集复核（17 行成立）**：T5 目标文件 12 件（archive/spec.md、CDQ、DEV-GOVERNANCE.md、ui-implementation-strategy.md、AGENTS.md、arch-change.md、architect.md、architect-reviewer.md、integrator.md、escalation-gate.mjs、milestones.jsonl、delivery-workspace.json）+ T1/T2/T3 各 1 + T4 两件 = 17，逐一对账成立；`.delivery/STATUS.md` 为派生重生成副产物、不入 manifest 独立项的注记（T5 :143）正确——landing 时它随 §5 #7 重生成出现，属核验对象而非 manifest 成员，口径自洽。一行一裸路径、迁移锚只记迁入路径 ✓。

**建议级修正（不阻断 GREEN；建议标 FROZEN 前回灌）**：

- **F15-a · 「rg 默认尊重 gitignore 致 ① 漏网」因果句被实测证伪**（T5 :137 与 §3 :170 两处）。判官直接实测：`rg -n 'spec\.md' .delivery/` **正常命中**（exit=0，STATUS.md ×7 全部列出）；`git check-ignore` exit=1（未被 ignore）；`git ls-files .delivery/` 三件**全部在册追踪**；仓内无 `.ignore`/`.rgignore`，`.gitignore` 无 delivery 条目。即：① 趟漏扫的真实原因是**执行口径**（那次 run 没扫到 .delivery/），不是工具行为；「判官 R3 补扫实证」也不构成对该因果句的支持——判官 R3 用的是 grep，从不受 gitignore 影响。建议 FROZEN 前改为诚实表述（如「① 趟执行漏扫 `.delivery/`（原因属执行口径、非工具行为——判官实测 rg 对该目录正常命中且未被 gitignore）；③ 趟定向补扫兜底」），免得错误教训流进 retro。**不阻断理由**：该句不支撑任何 manifest 条目、目标文件或验证步——命中清单本身已经判官两轮独立 census 对账闭合，结论不受因果表述影响；属硬项 1a 精度类文字修正，与 R2-F11（manifest 入列理由失实，涉 packet 结构字段理由）不同级。
- **F15-b · T5 :137 末「git 追踪状态 landing 时核对」可落定**：判官已实测三件全部 git 追踪（见上）。landing 时保留为冗余断言或直接落定均可，随起草方。

**GREEN 范围声明**：本 GREEN 覆盖 packet-id = 本 PROPOSAL §2 改法集（T1–T5）+ §5 验证集（v0.4 现态）；后续流程 = human 裁 OQ-2/3/4 → 主线程标 FROZEN（凭据 = 立项 ack + 本 R4 GREEN + human 裁 OQ 结果；F15-a/b 回灌建议随 FROZEN 一并完成）→ 明日 `.planning` 旧批归档后按 §5 #1 时序断言 landing。FROZEN 后 manifest 即终稿（17 行），landing  fidelity 按 §5 #7 逐文件核验。

**R4 亲核清单（2026-09-13 实测/实读）**：① manifest 17 行 ↔ §2 各 T 锚清单逐一对账；② §3 命中清单 ↔ 判官 R1/R3 两次独立 grep census 逐类对账（含本次新抽查 smoke_e2e:5/:60/:124/:128/:142、cloud_link:5 逐行属实）；③ `repo-structure.md:17/:32`、`scope-map.json:33` 行号亲核属实（:17 = docs/notes 行「spec 唯一」、:32 = 白名单文件清单、:33 = allowFiles 的 "docs/spec.md"）；④ T5 .delivery 块（:137）与 §5 #7（:204）亲读，SSOT 改源 / 派生不手改 / 入口不虚构三纪律在文；⑤ rg 行为实测三连（命中 + check-ignore exit=1 + ls-files 全追踪，附 .ignore/.rgignore/.gitignore 排查）——F15-a 证伪基座。
