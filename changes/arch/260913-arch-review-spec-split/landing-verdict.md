# landing-verdict · 260913-arch-review-spec-split

> 核验人：landing-verifier（独立核验，只读 codebase）。landing commit = f56e765（main = HEAD）。
> 核验时间：2026-09-13。verdict 版本：v1。

## 结论

**RED（ok:false）——结构门 fail-loud 拦截，不放行 merge，退主线程修 landing 证据件后重跑结构门。**

- **语义核验（逐文件人读 diff + 全机械验证）：全绿**——18 件 manifest 文件的实际落地内容与 FROZEN PROPOSAL §2（T1–T6）改法逐一对上，无漏改、无私货、无夹带、无矛盾；§5 验证集 10 条全部实测兑现。
- **唯一阻断项 = 结构门**：`landing-fidelity-check.mjs` B2 fail-loud——`architect-landing-summary.md` 缺 ```landing-manifest 围栏块（且 packet-binding 缺 "packet-id" 字面）。属证据件格式不合格，非 canonical 落地内容缺陷；修复不触 canonical，修后重跑结构门即足，语义部分无需重核。

### ErrorEnvelope

```json
{
  "ok": false,
  "failingFile": "changes/arch/260913-arch-review-spec-split/architect-landing-summary.md",
  "category": "structural-format（B2 fail-loud：双侧 manifest 围栏块契约 + A9 packet-binding）",
  "expected": "landing-summary 含 ```landing-manifest 围栏块、恰为 manifest 18 行裸路径（与 PROPOSAL 围栏块同集），且正文含 \"packet-id\" 字面引用（A9）",
  "actual": "summary 无围栏块（manifest 以「§1 逐条对账表格」承载，extractTargets 抛 MISSING_LANDING_MANIFEST）；正文仅有 \"packet = FROZEN v1.1 PROPOSAL…\"、无 \"packet-id\" 字面",
  "根因假设": "落地者把 manifest 做成了表格对账形态，未按 authoring 格式契约（architect-operating-model §5：PROPOSAL 与 landing-summary 双侧各含恰目标路径集的 manifest 围栏块）落围栏块；packet-binding 行文用 \"packet=\" 而非 \"packet-id\"。表格内容本身与 PROPOSAL 18 行逐条一致，仅格式不合规。",
  "修复路径": "主线程在该 summary 内补 ```landing-manifest 围栏块（18 行裸路径，照抄 PROPOSAL :12–31）+ 补 \"packet-id\" 字面 → 重跑结构门。canonical 与已 commit 内容零改动，无需重新 landing。",
  "语义核验": "全绿（见下），修复格式后可直放行（merge 仍归 human）"
}
```

## 门输出（实测命令与结果）

1. **结构门**：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260913-arch-review-spec-split`（--help 非选项，按位置参数跑）
   → **exit 1**，`{"ok":false,"gate":"landing-fidelity","message":"缺 landing-manifest 围栏块（architect-landing-summary.md）：authoring 格式契约要求 frozen PROPOSAL §5 与 architect-landing-summary 各含恰 canonical 目标路径集的 manifest 块…"}`。fail-loud 于 manifest 提取阶段，后续 claimed/planned-landed 机检被短路（HEAD=f56e765，工作树改动集已另行人工核：18/18 manifest 文件均在 commit 内）。
2. **设计侧门（proposal-consolidation-check）**：**Track B，N/A**（证据集无 decision-packet.md；「frozen ↔ 批定一致」由 §7 R4/R6 GREEN + Status 行 FROZEN v1.1 承载）。
3. **packet-binding 核对**：summary 引 packet = 本 PROPOSAL §2（T1–T6）+ §5 验证集、review id 260913-arch-review-spec-split 与目录/id 一致；landing 实际范围 ⊆ 被批 packet（见破坏性自检）。机器口径的 "packet-id" 字面缺失已计入 ErrorEnvelope。

## §5 验证集复跑（landing-verifier 亲跑）

| # | 验证 | 实测 | 结果 |
|---|---|---|---|
| 2 | `node .claude/scripts/spec-anchor-check.mjs` | storyCount 13、anchorLines 22、errors []、ok:true（exit 0） | ✅ |
| 3 | `--reverse` | 非空，节→引用方映射；抽查 §3.1 referencedBy=9 件（底座+S-F1.1/1.2/1.4/2.1/2.3/2.4/4.1+NFR-READ-1），与 architecture.md 实际相符 | ✅ |
| 3 | `--diff-ids docs/ui/kidpal-ui-design-v2.html docs/ui/kidpal-ui-design-v3.html` | oldCount 61 / newCount 61、removed/added 均空、verdict OK 零断锚 | ✅ |
| 4 | census 复扫（rg spec\.md，排除 §3 豁免面） | live 面零待改残留；仅存命中均属申报豁免类：新文件溯源注记（requirements.md:3/:406、architecture.md:3）+ ARCH-REVIEW-QUEUE.md:49（append-only）+ templates ×6（通用占位）+ 冻结验收件/头注释（firmware/main ×4、server ×3 文件）+ integrator.md:42/:71（本批授权的闸联动改指本身）+ planner.md:155/:178、review-arch.md:3、rules/contracts.md:14、architect-operating-model.md:79（豁免②③）+ taskpackages 历史 | ✅ |
| 5 | changes-index upsert | INDEX.md:41 行在场；upsert 返 unchanged（FROZEN 时已钉） | ✅（注：状态列仍 "frozen"；INDEX 头部状态机写 draft→frozen→landed 终态，但全仓 7 个已 landing 的 arch 批均停在 "frozen"，系既有惯例，非本批缺陷） |
| 6 | escalation-gate `--self-check` | changeid-prefix 4/4 + pre-merge-escalation-gate **30/30 ok:true**（exit 0）；CONTRACT_SPEC_LITERALS = ['docs/contracts/**','docs/spec.md','docs/requirements.md','docs/architecture.md'] 四件在场；integrator.md:42 Basis 前缀约定同集（`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/spec.md`（已归档件，仅历史 findings））——**与闸字面集一致，同 commit 联动成立** | ✅ |
| 7 | .delivery 重生成 | milestones.jsonl ×7 acceptanceRef→docs/requirements.md（M0/M6 两条 prose 语义引用同改 architecture.md §1.1 / requirements.md §7+architecture.md §6）；delivery-workspace.json:6→requirements.md §6；STATUS.md ×7 ref 链全指 requirements.md、零 spec.md 残留（diff 亲核 + census .delivery 零命中） | ✅ |
| 8 | 零丢失核验方程 | 逐节映射（landing-summary §5.8）亲核成立：两新文件节集合 ∪ 已砍 5 件 ∪ 已结不迁清单（§6.3 七条：六条已结不迁有据、多轮会话连续性迁入 arch §6 开放问题 1）∪ 变更注记 8 条 ≡ spec 九节 | ✅ |
| 9 | story 五件核验 | 13 条全读：原文/AC/线框/补充规则/锚行五件齐、AC 编号在场、🔴 零条；机制词（cron/散列/幂等/JSON/预合成）story 正文区零命中（唯一 "JSON" 在 §6 路线图 M3，系 spec §8 verbatim 携带）；story 原文 verbatim 抽查 S-F1.4/S-F2.1/S-F3.1 与 archive/spec.md:318/:356/:405 **逐字一致**；S-F3.3 收窄（archive:423 含「、还想再查下一个字」半条，落地版删去）+ 变更注记在案（req:318）；可行性标定 8 项与 feasibility-initial-marking.md 终标逐条对符（S-F1.3/1.5 ✅、S-F3.1/3.2 🟡、S-F3.3 ✅ 收窄、NFR-PERF-2 🟡、NFR-SEC-1 ✅、NFR-READ-1 🟡、PWR-1/READ-2 砍） | ✅ |
| 10 | skill 卫生 | lint-descriptions exit 0（唯一点名 = landing-verifier.md 既存超长，非本批新增）；route-regression-check 28 ok / 0 fail | ✅ |

## 逐文件语义核对（git show f56e765 -- <file> 亲读）

| # | 文件 | 结构 touched | 语义 | 要点 |
|---|---|---|---|---|
| 1 | docs/requirements.md | ✅ | ok | T1 全项在场：已砍清单 5 件（2 反白 + S-F2.5 + NFR-PWR-1 + NFR-READ-2，砍因日期齐）／UI 锚规则块 6 条（v3 指向修掉 P2 漂移）／§1+§2 底座（端云列废弃、节级支撑锚 req:120）／§3 Epic E1>F1–F4>13 story 五件／§4／§5 NFR 5 域 7 条各带四件／§6 M0–M6／§7 开放问题 4 条（含 S-F3.3 移出的指代解析） |
| 2 | docs/architecture.md | ✅ | ok | T2 全项：§1 Infra（1.1 板子 + 1.2 ECS/网络）／§2.1–2.10（2.9/2.10 机制迁入与 spec S-F1.4/S-F2.2 机制段逐段对上，含 V14/V15/V16/V18/V20/V34/V31 全套）／§3 UI 实现策略／§4 契约层引用式导航（细节不抄）／§5 决策 28 条（抽查 20/22/26 verbatim：22/26 逐字一致，20 仅文末多「机制细节见 §2.9。」指针）+ §5.2 待 brainstorm 四条处置／§6 开放问题 3 条（多轮连续性 = §6.3 迁入） |
| 3 | docs/archive/spec.md | ✅ | ok | `R100 docs/spec.md → docs/archive/spec.md`，100% rename、零内容改动（-M diff 正文为空） |
| 4 | docs/CONTRACT-DECISION-QUEUE.md | ✅ | ok | CDQ-260913-spec-split-fallout 挂账行在场，三后果（契约 :4 断链 / key_route_a1:636+screen_unification:1010 canary / schedule_reminder:1561 直读）+ phase 2 处置口径，append-only |
| 5 | docs/DEV-GOVERNANCE.md | ✅ | ok | :241 治理路由表「系统 spec」行 → 两文件（G1） |
| 6 | docs/notes/ui-implementation-strategy.md | ✅ | ok | :3 「spec 侧索引见 spec.md §5.7」→ architecture.md §3 |
| 7 | AGENTS.md | ✅ | ok | :7 「系统 spec 面」行 → 两文件 + 归档注记（G1，行内含 2026-09-13 前单文件说明） |
| 8 | .claude/commands/arch-change.md | ✅ | ok | :24 lane 分类 canonical spec 行 → 两文件 |
| 9 | .claude/scope-map.json | ✅ | ok | docsTopLevel：−spec.md +requirements.md +architecture.md |
| 10 | .claude/rules/repo-structure.md | ✅ | ok | docs/notes 行「spec 唯一」→ 两文件规则；白名单文件行两文件 + archive 注记——与 scope-map 两处一致 ✓ |
| 11 | .claude/agents/architect.md | ✅ | ok | :23/:52 自改递归改指（human F6 裁 (a) 知情批准），逐字核无权限/写域扩张 |
| 12 | .claude/agents/architect-reviewer.md | ✅ | ok | :42/:146 canonical spec 指引改指两文件 |
| 13 | .claude/agents/integrator.md | ✅ | ok | :42 Basis 前缀约定与闸字面集同批联动（四件同集）；:71 消解边界 = 冻结件+contracts+两新文件+归档件 |
| 14 | .claude/scripts/spec-anchor-check.mjs | ✅ | ok | 新增；三检 + --reverse + --diff-ids 亲跑全绿；未挂 hook（settings/hooks 零引用，T3「先手动」兑现） |
| 15 | .claude/scripts/pre-merge-escalation-gate.mjs | ✅ | ok | CONTRACT_SPEC_LITERALS 四件字面集扩展 + 注释联动 + 自检 4 新例（requirements 检出 / architecture+CDQ 核销 / 归档件兼容 / 非字面集放行），30/30 绿；改法与归档同 commit 原子（无中间态漏拦窗口） |
| 16 | .claude/skills/write-story/SKILL.md | ✅ | ok | 与 human 审定草案逐行 diff：去草案头、标题级适配 requirements、锚 intent 占位填实编号、示例按注记 8 修正（AC3「断网本地提醒照常弹出」笔误 → AC5「断网无提醒（回引 NFR-OFF-1）」，锚 state-reminder（v3 实存）+ arch §2.9/§3.1、时刻 20:30）——**landing-summary 自报注记 8 属实** |
| 17 | .delivery/milestones.jsonl | ✅ | ok | ×7 acceptanceRef SSOT 直改源 + 两条 prose 语义引用同改 |
| 18 | .delivery/delivery-workspace.json | ✅ | ok | :6 权威指针 → requirements.md §6 |
| 派生 | .delivery/STATUS.md | ✅（commit 内，manifest 外） | ok | 派生重生成（T5/R4 口径：改源重跑 delivery-status.mjs 产物，不手改、不入 manifest），diff 亲核 ×7 ref 全改 |

story 底稿对账：story-rewrite-draft.md（human 过目稿）vs 落地 requirements.md，差异 = 草案自立「支撑锚 = 意图占位、landing 时填实编号」的兑现 + 标题级适配 + S-F3.3 注记改写（实质同），无未申报实质漂移。

## 敏感面逐字核对（G1/G3/self-def/review-loop）

- G1 指令根：AGENTS.md:7、DEV-GOVERNANCE.md:241 均只改 spec 指引行，无它处改动。
- G3 强制层：scope-map（−1+2 恰为白名单登记）、escalation-gate（字面集扩展 + 自检，无新增拦阻语义回退）、repo-structure（两处一致）。
- 自改递归 ×3（architect/architect-reviewer/integrator）：逐字核仅 canonical spec 指引与 Basis 约定/消解边界改指，无权限面、写域、工具、model 变更。
- review-loop 相关（integrator.md Basis 约定）：与闸字面集逐字同集，无脱钩。
- 私货检查：全部 20 个 commit 文件的 diff 已逐一亲读，未见 PROPOSAL 未授权的语义改动。

## 破坏性自检

- **豁免面零触碰**：`git show f56e765 --name-only` 20 件逐一归类——.claude ×10、AGENTS.md、docs ×5（requirements/architecture/rename/CDQ/DEV-GOVERNANCE/notes 实为 6）、.delivery ×3、changes/arch 本目录 summary 1 件。**firmware/、server/、docs/contracts/、.planning/、docs/ui/ 零命中** ✓。
- **声称 → diff 对查**：summary §1 表 18 行声称逐项在真实 diff 中找到对应；§2 七条处置、§3 八条注记、§4 两条机械改指（决策 4「spec §3.0.0」→「本文件 §2.2」、决策 17「（§5.7）」→「（本文件 §3.1）」）均核实。
- **核验覆盖声明**：无跳过——大文件（requirements 415 行 / architecture 329 行）全文读毕，敏感面逐字核，未抽样代替。

## 发现的偏差（均不构成本次语义打回理由，如实登记）

1. 【阻断·结构格式】summary 缺 manifest 围栏块 + packet-id 字面（ErrorEnvelope  above；修复后重跑结构门即可）。
2. 【微偏差·未申报的逐字性】决策 20、25 文末各增「机制细节见 §2.9/§2.10。」导航指针（landing-summary §4 只申报了决策 4/17 两处机械改指）。语义中性、与 T2 机制迁入设计同向，且 §2.9/§2.10 头部有反向「决策原文」注记；判 benign，建议 retro 登记。
3. 【覆盖说明】spec S-F1.4「带物回读（V17 定）」未迁入 arch §2.9——该条为用户可感知口径，已由 story 层承接（S-F1.3 AC3 + 补充规则「备注列参与语音答复」），符合 T1「机制迁 arch、需求口径留 req」的切分，非丢失。
4. 【流程观察】INDEX.md 状态列全仓惯例停在 "frozen"（状态机文档写 landed 终态），非本批引入。

**放行建议：RED 退回**——主线程补 summary 围栏块 + packet-id 字面 → 重跑 `landing-fidelity-check.mjs` 应转绿 → 语义侧本 verdict 已全绿存档可作复核对账依据；merge 归 human manual-merge 门。

---

# 最终 verdict（v2 · 2026-09-13 复验）

## 结论

**GREEN（ok:true，条件放行）**——首轮两个格式拦阻已修并复验属实；结构门唯一剩余红经独立裁定 = **已登记闸解析盲点假红**（非实质对账失败）；语义保真维持首轮全绿结论。两个条件项归 human 在 merge 门追认（Track B 第七步本就有 human 出场点，不额外加门）：

- **条件 1（human 追认）**：PROPOSAL「落地后不可回改」条款的核验期格式修复例外（6 行去强调符，零内容变更）。
- **条件 2（human 例外拍板）**：结构门 .delivery 失配 = ARQ-260913-lfc-anchorable-delivery-gap 解析盲点，本批按例外闭环。

## 复验逐项记录

### 1. 结构门复跑（实测）

`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260913-arch-review-spec-split` → exit 1，errors **恰一条**：「§2 锚清单 ≢ manifest：仅 §2 有：（无）；仅 manifest 有：.delivery/milestones.jsonl, .delivery/delivery-workspace.json」。首轮 B2/A9 拦阻（manifest 围栏块、packet-id 字面）已不再出现。

### 2. PROPOSAL 格式修复核验（git diff 亲核）

工作树 diff = **恰 6 处 hunk、每处仅 `**目标文件**：` → `目标文件：`**（去加粗强调符），零字词/零路径/零语义变更。六处对应 T1–T6 各一条目标文件行。修复性质 = 把锚行归一为解析器可辨的裸形态（260904/260905/260906 各批先例形态）；评审链 R2/R4/R5/R6 对「manifest ≡ §2 并集」的历次人工对账（17 行→18 行）均以这些行的语义内容为据、从未以加粗形态为据——形态修复不触及任何被批内容。

**独立裁定（条件 1）**：可接受，须 human 追认。理由：「落地后不可回改」条款的守护对象 = 已批改法集/清单/验证集不被事后改写（防追溯性重定义已批 packet）；本次 6 处改动不含任何字词与路径变更，不触碰 manifest 块与改法正文，且方向是向既有批次的既定形态归一。替代方案（为重打 12 个星号走整包 re-FREEZE）成本与噪声远大于收益。例外已在 summary §4 补记 2 如实自报——申报诚实性核验属实。**这是格式例外先例，同意范围以「零语义变更 + 机检可证 + 如实申报 + human 追认」四要件同时成立为界，不含糊外推。**

### 3. 闸解析盲点的独立根因确认（不依赖主线程口径）

亲读 `.claude/scripts/lib/governance-gate-shared.mjs:40`：`ANCHORABLE_TEST = /^(?:\.claude\/|docs\/|\.planning\/|README\.md|AGENTS\.md|CLAUDE\.md|\.[A-Za-z0-9_-]+$|changes\/)/`——白名单确无 `.delivery/` 形态；:393 以它过滤 §2 锚行 → T5 目标文件行里的两件 `.delivery/` 路径被滤出并集 → 并集 16 ≡ manifest 18 − 2 精确失配。三方独立佐证「纯解析盲点」：
- **§2 实质授权在场**：PROPOSAL T5 目标文件行明列 `.delivery/milestones.jsonl`（SSOT 源改指，R3-F12）、`.delivery/delivery-workspace.json`（权威指针改指，R3-F12）——parser 丢弃的是真实授权；
- **实质对账已双重成立**：判官链 R4/R6「manifest 18 行 ≡ §2 并集」逐行人工对账 + 本 verdict 首轮逐文件 diff 核（#17/#18 两件均按 T5 正确落地）；
- **处置路径合规**：ARQ-260913-lfc-anchorable-delivery-gap 已登记于 docs/ARCH-REVIEW-QUEUE.md §1.1（diff 亲核，含根因、批次首发性说明、双路径处置），修闸归下一治理批——本批 packet 无授权改闸（G3 强制层），夹带修闸反而是 scope creep。

**独立裁定（条件 2）**：本批按例外闭环可接受。机检门的权威来自其对「实质保真」的担保力；残余 error 已被证明为白名单缺口假红且非本批引入，要求本批消除它无可执行路径（修闸越权 / 从 manifest 删两件 = 篡改对账）。例外拍板归 human，与 ARQ 登记的双路径处置一致。

### 4. summary / retro / ARQ 新增内容核验

- summary 头部 `packet-id =` 字面在场 ✓；```landing-manifest 围栏块 **18 行与 PROPOSAL 块逐字一致**（程序化比对 identical:True）✓；
- summary §4 补记（决策 20/25 导航指针漏申报，与首轮偏差 #2 同条）+ 补记 2（格式修复例外自报 + 闸盲点自报）均属实 ✓；§6 新增遗留行与 ARQ 呼应 ✓；
- docs/ARCH-REVIEW-QUEUE.md 仅 +1 行（ARQ-260913-lfc-anchorable-delivery-gap，§1.1 未结案，处置含「本批 human 例外拍板」路径）✓；
- retro.md 第 50 行核验期补发条在场（两轮拦阻 + 两盲点 + 例外待追认，均如实）✓；
- 全仓工作树核对：除上述三件 + 本 verdict + retro（+ 豁免的 codebase-memory 工具自刷）外零改动，canonical 未动 ✓。

### 5. 语义结论继承

首轮 v1 verdict 的逐文件语义核对、§5 验证集 10 条实测、敏感面逐字核、破坏性自检（豁免面零触碰）、verbatim 抽查全部维持有效——canonical 自 f56e765 起无任何变动（本轮 diff 面仅证据目录三件），无需重跑。

## 最终判定汇总

| 项 | 结论 |
|---|---|
| 语义保真（18 文件 + 派生件） | **绿**（v1 全量核验维持） |
| 结构门（机器） | 首轮两格式项已修 ✅；残余 1 error = **已证解析盲点假红**（ARQ 登记、修闸归下批） |
| PROPOSAL 格式例外 | 6 行纯格式 ✅，**待 human 追认（条件 1）** |
| .delivery 闸例外 | **待 human 例外拍板（条件 2）**，ARQ 双路径处置在册 |
| merge | 归 human manual-merge 门；本 verdict 建议：**条件 1、2 经 human 点头后即全绿放行** |

合并时建议把 review 证据目录三件工作树改动（PROPOSAL 格式归一、summary 补块、ARQ +1 行）与 retro、本 verdict 一并提交，commit message 如实注记「核验期格式修复 + 例外追认」。
