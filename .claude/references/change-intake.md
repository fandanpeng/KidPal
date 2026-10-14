# change-intake · 跨面变更统一 intake 与 spine 程序（统一 intake ≠ 统一门）

> 权威面：本文件是跨面变更统一 intake 的程序 SSOT（260928-gov-review-cross-face-intake-spine 批 T1 落地，FROZEN v1.0）。
> 消费关系：`/triage-change` 与 [change-triage.md](change-triage.md) 消费本文件做四面探测与分面推进；[requirement-change.md](requirement-change.md) 的三档判据被本文件消费（不复制）；各面门照旧各归各裁——统一 intake ≠ 统一门。
> 比例化一句话：触面 ≥2 才建 spine；单面变更不建。

## 1. 触发与比例化

- **触发**：一个 idea 或请求同时触 ≥2 面（需求 / 架构 / 契约 / 实现）——human 说一次意图，本程序探测触面、落 spine、分面推进。
- **比例化**：触面 ≥2 的变更强制建 spine；单面 ① 直接修免门（地板不变：不改实现面行为 / human 明确跳过）；单面 ②③ 现状不动——plan.md / PROPOSAL 本身已载 intent，无重复陈述问题。
- **免建例外**：human 明确说跳过流程的一次性操作允许直改（对齐 AGENTS.md Hard Rules 既有条款）；记账须写明「跳过 intake」。

## 2. spine 制品形态

- **落点** = `delivery/changes/intake/<changeId>.md`，一变更一文件，lane 中立（arch / dev / retro / intake 四 sibling；结构登记见 repo-structure.md）。写者 = 主线程；**anchors 回填写者也恒 = 主线程**——下游会话以退出报告交回结果，主线程回填（写域零扩张）。
- **frontmatter 七字段**：
  1. `changeId` = 全局 changeId（YYMMDD-slug）；各面锚回填时映射为各面既有格式——req = §9 版本号、contract = packet、arch = changeId、imp = taskpackage id；单包 ② 直接复用不另铸。
  2. `sources` = human 意图原话（逐字）+ 关联引用 id 列表（ARQ / REQ- / §9 版本行 / retro / 里程碑；词汇复用 TaskPackage `sourceRefs`）。
  3. `faceMap` = 四面触面事实 + governance 探测命中行（动不动 / 动哪 / 依据行号；R2-M2）。
  4. `facePlan` = 分面计划（各面谁裁 + 执行顺序 + grilling 一次收敛的裁定）。
  5. `anchors` = per-face 回填槽（初始空；每面定稿后主线程回填）。
  6. `openQ` = 子流程疑问与裁定留痕（问答通道 + 过程账）。
  7. `status` = 状态机（`intake` → `in-flight` → `awaiting-human:<gate>` / `awaiting-upstream` / `awaiting-human:<openQ>` → `closed`）。

## 3. 四面探测程序

- **派发形态**：主线程派 architect（只读 + 同会话 return），语料 = requirements.md（含 §9 账 + story AC）+ architecture.md + `docs/contracts/**` + debt 未结账三域（gov / tech / requirement）+ governance 面（`.claude/agents/**` 定义 / gate 登记）。
- governance 命中记入 faceMap 并路由 /arch-change 治理 profile，不建实现面档（R1-F4）。
- **建档前查重**：intake 开场先查 `delivery/changes/intake/` 既有 spine（按意图与触面相近判），命中 → 消费既有件不重复建档；并存期 /arch-change 侧同此纪律（其文本改动归退役安置批，本文件单侧承载）。
- **触面事实是查出来的，不是申报出来的**（walkthrough 实证自我申报不可靠）。产出 = faceMap + facePlan 交主线程落盘。

## 4. 立项 ack

- **唯一必停人类触点**：intake 收尾，四面对齐「为什么改 / 往哪改 / 改动多大」；升级判据（触契约 / 跨系统结构性 / 存疑向严）随 ack 一并裁定路由。

## 5. 发起机制（两姿势 + 三铁律 + 写者纪律）

- **姿势一（轻面）**：主线程自己就是编排者，直接一级派发子 agent（需求档 0/1、单步小活）。
- **姿势二（重面）**：主线程经 Bash 后台 headless 拉起 `claude -p`——它是完整顶层会话、非子 agent，可 `--agent` 跑完整编排循环（对抗 review / 多 Pod 拆包）。长循环必须后台执行 + 通知回调（Bash timeout 实测上限 10 分钟）；子会话 stdout 只是进度，状态一律以 spine 落盘为准。
- **三铁律**：
  1. **禁跨门**——headless 子会话到人类门即停：status 写 `awaiting-human:<gate>` + 退出报告交回主线程；human `claude --resume <session-id>` 进同一会话拍板，上下文无损；dev loop 关键门 human 仍在场。
  2. **禁递归**——headless 子会话不得再 `claude -p` 拉孙会话（面流程的一级派发够用，递归拉起是失控面）。
  3. **身份三件套显式**——命令行显式钉 `--agent` + 按面显式传匹配 settings（`dev-loop.settings.json` / `review-loop.settings.json` 在册复用、零文件改动）+ changeId（ARQ-260918 贴卡起步丢身份先例）。
- **单写者规则**：同一 spine 同一时刻仅一个主线程会话持写权，跨会话接力经 openQ / 退出报告交接，禁并行双写。

## 6. 疑问三级处置

- **L0 子流程就地解决**：执行歧义且 canonical 有依据可引（references / 既有先例 / story 原文），不涉门语义 / 不越权限 / 不创先例 / 影响限于本变更内 → 直接办，裁定 + 依据写 `openQ` 留痕。
- **L1 主线程答疑**：子会话写 `openQ` 退出 → 主线程凭 faceMap / facePlan / 立项 ack 取舍倾向作答（跨面上下文只在主线程完整存在）；能答的写回 `openQ` → headless 续跑，不消耗 human 出场。
- **L2 呈 human**：判据存疑向严 / 枚举外新情形 / 需要取舍（对齐触点①预授权「枚举外一律回问」与 Track B「存疑向严」，从触点①推广到全部子流程疑问）。**L1 答不了的去向**：openQ 标 `escalated` + status 写 `awaiting-human:<openQ>`，主线程呈人，答复写回后 headless 续跑，不得悬置。

## 7. spine 免冻与 facePlan 修订权限

- spine 是路由账本不是设计文档，**不设 FROZEN**；各面的冻结语义由各面制品承载（§9 版本行 / 契约 packet / FROZEN PROPOSAL / origin.frozen），spine anchors 记录这些冻结事实。
- **facePlan 修订权限**：加面 / 改「谁裁」→ L2 呈人；调执行顺序 / 细节 → L0/L1 主线程可裁。

## 8. 与各面门接缝

- **faceMap 契约行 = architect-reviewer 四分类的输入**：契约门开场带 intake 取证，human 第二次出场只答「签不签」，不重听诊断。
- **升级判据 ≠ 契约门**：升级判据是路由门（答「走哪套流程」，触面疑似即可命中、判错可挽回）；契约门是审批门（答「这份 diff 签不签」，针对具体 packet、判错污染 SSOT）。两门独立可达——升级后四分类可判「无需改」、未升级也可能被逃生门踢进契约门。
- **开场即消费**：带 changeId 调用的各面流程入口（triage 实现面 / requirement-change 各档 / /arch-change / build-feature）先读 spine 的 faceMap/facePlan，不得要求 human 重述意图。
- **launch 前置**：带 spine 的变更，facePlan 中先于实现面的各面 anchors 未回填 = 上游未冻结 → 承载包不得 launch（canonical 定义见 task-intake-and-slicing.md launchPreconditions 节 spine 源条款；build-feature launch 面预检第五件）。