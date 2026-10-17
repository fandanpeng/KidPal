---
id: ARQ-261017-merge-gate-scope-unstated
title: 合并闸只认 dev/gov 两条 lane——intake/debt 制品合并恒 kind=unknown BLOCK 且无豁免口（--human-approval-ref 对 unknown 直接 FATAL），「哪些合并需过闸」未成文，实际靠主线程当场解释 BLOCK
domain: gov
status: deferred
severity: SHOULD
owner: architect（定闸的适用范围）+ 主线程（文面落点）
trigger: 下次治理批；或下个动 pre-merge-escalation-gate.mjs / merge-to-main skill 文面 / AGENTS.md 治理路由节任一时并批（与 ARQ-261014 同脚本，可同批）
opened: 2026-10-17
closed: ""
closedBy: ""
paths: [.claude/scripts/pre-merge-escalation-gate.mjs, AGENTS.md]
gateLayers: [G1, G3]
hardExclusions: true
source: [主线程会话 2026-10-17（跨面变更 261016-p3-story-body-sd-cache 的 intake spine + debt 条目并入本地 main 时实测）, docs/debt/gov/_archive/2026/ARQ-260902-escalate-merge-no-human-path.md（同形前身，仅 dev-loop 档承载）, docs/debt/gov/ARQ-261014-merge-gate-tooling-hotfix-batch.md（同脚本的另根工具面缺口）]
---

**证据**（2026-10-17 实测，逐字）：

1. **闸的 kind 探测只有两条 lane**。[pre-merge-escalation-gate.mjs:419-423](../../.claude/scripts/pre-merge-escalation-gate.mjs)：`devDir = delivery/taskpackages/<changeId>`、`archDir = delivery/review/<changeId>`，`kind = hasArch ? 'gov' : hasDev ? 'devloop' : 'unknown'`。第三条 lane（`delivery/intake/`、`docs/debt/`）不在枚举内。

2. **`unknown` 恒 BLOCK，且无豁免入口**。实跑 `node .claude/scripts/pre-merge-escalation-gate.mjs 261016-p3-story-body-sd-cache` 输出逐字：

   ```
   [pre-merge-escalation-gate] BLOCK (changeId=261016-p3-story-body-sd-cache, kind=unknown):
     - unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/review）
     → 升级 human，不得自动 merge。
   ```

   而 [:736-737](../../.claude/scripts/pre-merge-escalation-gate.mjs) 逐字：`--human-approval-ref 仅适用于 dev-loop 档（changeId=… kind=…，无人批豁免概念）` → **exit FATAL**。即：`unknown` 档连"human 批准后放行"的机械口都没有——人的批准**落在真空里**。

3. **本次实际路径 = 主线程当场解释 BLOCK 后手工合并**。human 明示「merge to main」→ 主线程判定"闸说的是『不得**自动** merge、升级 human』，而升级的落点就是已下达指令的 human"，遂在 main worktree 以 `git -C <main-worktree> merge --ff-only dev/pod-m2` 完成（ff，65e32e8 → be2beb6）。**该解释与手工路径未成文于任何 canonical**。

4. **闸自己的措辞制造了错误信号**。`:398` push 的是「unknown change kind（changeId 既无 … 也无 …）」，`:753` 输出 BLOCK——读起来像"权限/凭据不足"，实际是"**本闸不适用于这类制品**"。这两件事对读者的行动指引完全相反：前者应停止并求助，后者应改用另一条通道。

5. **前身条目只覆盖了同一形状的一半**。`ARQ-260902-escalate-merge-no-human-path`（closed 2026-09-05）治「human 批准的 ESCALATE 无机械合并路径」，其结项文本逐字：「升级闸新增 `--human-approval-ref <eventId>` 五要素核验口（ESCALATE + provenance 完整 + 核验 ok 放行；BLOCK/provenance/off-main/契约永不豁免）」——ESCALATE 是 **dev-loop 概念**，故补齐的豁免口按设计只挂 dev-loop 档（本次 :736-737 复证）。**非 lane 制品的同形缺口未被覆盖，也未在任何条目中登记。**

**影响面**：

- **复现面 = 每次跨面变更的 intake spine 上 main**。跨面变更（触面 ≥2）建 spine 是常态机制（change-intake.md §1「触面 ≥2 的强制建 spine」），每个 spine 都要上 main；debt 条目同理。即这是一条**常态发生**的路径，不是边角。
- **代价形状 = 口径靠解释**。AGENTS.md 治理路由 step 7 逐字要求「主线程跑前置升级闸（含 off-main 校验），放行 → 自动合并本地 main（可回滚）」——一个照章办事的主线程会**每次**跑到闸前、拿到一个**无法清除**的 BLOCK，然后必须自行判断"这是不适用还是真拦"。两次不同的会话可能给出不同判断：一次直接合并（本次）、一次无谓升级到人（徒增一次人类出场）。**同一份 canonical 下两种行为，是治理一致性的缺口。**
- **最坏情形**：把 BLOCK 解释成"不适用"这一判断被推广到**真正该拦**的情形（例如某次制品里夹带了契约面改动），则闸被静默绕开——闸的权威被自身的覆盖面噪声稀释。

**建议改动**（候选，交 review、不自批）：

1. **闸增第三 kind 或改判**：把 `delivery/intake/**`、`docs/debt/**` 制品识别为一类（如 `ledger` / `artifact`），明确其判定（放行 / 需 human 授权 / 不适用）。**注意**：若取"以 `--human-approval-ref` 覆盖 unknown"，会撞 `:736-737` 现行「gov/unknown 无人批豁免概念」的设计——**这是真设计分叉，须 architect 裁**，本处不预断。
2. **或把 BLOCK 降为 NOT_APPLICABLE**（改动更小、不动豁免设计）：`unknown` 档的输出改为「本闸不适用（非 dev/gov lane 制品）——按 merge-to-main skill 由 human 授权合并」，从 BLOCK 改为显式的不适用态。
3. **补写适用范围**（无论取 1 或 2 都该做）：在 AGENTS.md 治理路由 step 7 或 merge-to-main skill 成文一句「哪些合并需过前置升级闸、哪些走 human 授权通道」。现行 `merge-to-main` skill 前置节已写「本 skill 只执行**已被批准的**本地合并（…或 human 批准后的本地合并动作面）」——但**没人把这条与闸的适用面接上**，读者无从知道二者是"并列的两条通道"而非"前闸后闸"。
4. **（可选）check 齿留根**：本项若只取候选 2+3（不改判定语义），无天然机检齿；即便取候选 1，闸自身的覆盖面也难自证——建议如实记为 convention-only，不强行造齿。

**riskTier: medium** —— 触闸机械面（改判定或改输出语义），影响所有合并的闸前行为；但不动契约面、不动 dev/gov 两 lane 的既有判据。

**review profile = full-adversarial + human**（gov-arq skill 判据：`paths` 含 `.claude/scripts/pre-merge-escalation-gate.mjs` ⇒ 机检硬排除 `newGate` 命中——正则 `.claude/scripts/.*gate.*.mjs`——不得美化快路径）。机推 `gateLayersOf(['.claude/scripts/pre-merge-escalation-gate.mjs','AGENTS.md'])` = `['G1','G3']`。Track B 三要件：① 执行测出 **yes**（本次实测，BLOCK 输出逐字在案）/ ② 问题清晰、修复明确 **no**（候选 1 与 2 是**真设计分叉**：闸该"放行"还是"宣布不适用"，且候选 1 撞现行豁免设计）/ ③ 爆炸半径 **部分**（本次一例实测；"照章办事的主线程每次都会撞"为 Canonical 文本推断）→ **② 不满足 + newGate 命中，profile 恒 full**。

**查重与分根说明（供评审复核，防双头记账）**：本条与两条已存在条目的关系已逐条比对——

- `docs/debt/gov/_archive/2026/ARQ-260902-escalate-merge-no-human-path.md`（已结）：**同形状、不同根**。该条治「**已进入 kind 枚举**的制品，人批之后无机械路径」（dev-loop ESCALATE），修法 = 加豁免旗标并按其设计限于 dev-loop；本条治「**根本不在 kind 枚举内**的制品」（intake/debt），是闸的**覆盖面**缺口而非豁免口缺口。故新立、不重开该已结条目。
- `docs/debt/gov/ARQ-261014-merge-gate-tooling-hotfix-batch.md`（在册）：**同脚本、不同根**。该条四项为 merge-driver `ReferenceError` / detect-changes CLI 失配 / INDEX 生成件 overlap 判定 / 教育面，均为**工具行为缺陷**；本条为**适用范围未成文**（闸行为本身可说正确，缺的是"哪些合并归它管"这句话）。两条 `paths` 同含本闸脚本，**建议同批评审**（该条 trigger 亦写「下个动 pre-merge-escalation-gate.mjs 的批」）。

**关联**：`261016-p3-story-body-sd-cache`（本条派生自该变更的合并实测；其 spine = `delivery/intake/261016-p3-story-body-sd-cache.md`）；`ARQ-261011-contract-gate-literal-selfquote-false-positive`（ARQ-261014 自述与其同脚本可同批）；AGENTS.md 治理路由 step 7（"跑前置升级闸…自动合并本地 main" 的适用范围出处）。
