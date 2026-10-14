# landing-verdict · 260928-gov-review-cross-face-intake-spine

> landing-verifier 独立核验（Track B 第 5 步）· 2026-09-28 · 判定依据 = 机检退出码 + 逐文件人读 diff，不采信 landing-summary 自述。
> 基线：commit d62f1cd（PROPOSAL FROZEN v1.0）之上的工作树未提交改动。

## verdict：**PASS（ok:true）**（2026-09-28 第二轮修复后复核更新；第一轮为 FAIL——结构门红、语义层绿，见下文与文末复核段）

> **verdict 演进**：第一轮 FAIL（summary 缺 manifest 围栏块 + 门首错即停遮蔽的 packet-binding 字面 token 缺失）→ 主线程两处 summary 侧格式修复 → 第二轮独立复核结构门 ok:true / exit 0 → **最终 PASS，放行**。全文第一轮记录原样保留作证据链。

**唯一阻断项**：`architect-landing-summary.md` 缺 ```landing-manifest 围栏块——authoring 格式契约要求 frozen PROPOSAL 与 landing-summary **各含恰** canonical 目标路径集的 manifest 围栏块（SSOT：architect-operating-model §5）。landing-summary 现仅有逐文件清单表格（表格十行与 manifest 十路径一一对应，内容无缺），但机检认围栏不认表格，完整门 fail-loud exit 1。

**修复**：主线程在 landing-summary 中补一个 ```landing-manifest 围栏块（十路径照抄 PROPOSAL §manifest），重跑完整门 `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260928-gov-review-cross-face-intake-spine` 确认绿。修复是纯 summary 侧格式补齐，不触任何 canonical 文件，语义层核验结论（下节）不受影响、无需重核。

## 结构门输出摘录

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260928-gov-review-cross-face-intake-spine
→ exit 1
{ "ok": false, "gate": "landing-fidelity",
  "message": "缺 landing-manifest 围栏块（architect-landing-summary.md）：authoring 格式契约要求 frozen PROPOSAL §5 与 architect-landing-summary 各含恰 canonical 目标路径集的 manifest 块（一行一条；SSOT 见 architect-operating-model §5）。…" }
```

注：该门遇首错即停，围栏块补齐重跑前，claimed-but-not-landed / planned-but-not-landed / 锚集对账等后续机检项尚未跑过——重跑结果须进最终放行证据。本 verifier 已人工等价核过：manifest 十路径与 summary 清单表、PROPOSAL §2 各 T 目标文件锚并集三方面逐一吻合（10=10=10），且 git 工作树中十路径全部有改动（9 改 + 1 新增 untracked）——预计重跑可通过，但仍以机检退出码为准。

设计侧门（proposal-consolidation-check.mjs）：Track B 证据集无 decision-packet，按角色定义 N/A 不跑。

## 逐文件核对结论（10 行清单）

| # | 文件 | T | 结论 | 差异说明 |
|---|---|---|---|---|
| 1 | `.claude/references/change-intake.md`（新增，62 行 8 节） | T1 | PASS | 八块逐块对全部在位：触发比例化 / spine 七字段（含 R2-M2 governance 命中行、anchors 回填写者恒主线程）/ 四面探测（governance 面 + R1-F4 + R1-F5 建档查重）/ 立项 ack / 发起机制（两姿势 + 三铁律 + 单写者 R1-F6）/ 三级处置（含 L1 答不了去向）/ 免冻 + facePlan 修订权 / 八接缝（契约行四分类输入、升级判据≠契约门消歧、开场即消费）。两处非逐字但合规：新增「免建例外」小节（human 明确跳过）= 对 AGENTS.md Hard Rules 既有条款的显式重申，非权限扩张；PROPOSAL 第 5 块「review-moderator 现形态即拦」论证括注未落文（规则本体「写域零扩张」在位，论证性文字不落可接受） |
| 2 | `.claude/commands/triage-change.md` | T2 | PASS* | :13 替换句语义全中（四面探测 / governance 面 R2-M1 顺手修 / facePlan 路由 / 不再整单踢走）；:21 步骤 1 同语义改写、「第一刀」措辞退役；:32-35 整块替换、原 :35「本门拉不起那个会话」确认删除。*三处非逐字（见下「分级差异」#2/#3），均语义等价或有 PROPOSAL 条款授权，不计阻断 |
| 3 | `.claude/references/change-triage.md` | T2 | PASS | :10 同语义改写（PROPOSAL 只钉语义不钉逐字）：四面探测 / faceMap / 升级判据路由 / governance 命中 → /arch-change 治理 profile 全在位 |
| 4 | `.claude/rules/repo-structure.md` | T3 | PASS | 两新行与 PROPOSAL T3 围栏块**逐字一致**（含全角标点与反引号），插入位 = dev 行之后、追加不覆盖 |
| 5 | `.claude/references/task-intake-and-slicing.md` | T4 | PASS | spine 源条目逐字一致（仅引号『』→「」一处标点形变，语义零差），append 到存量迁移条目之后、append-only 合规 |
| 6 | `.claude/commands/build-feature.md` | T4 | PASS | 「四件」→「五件」两处 + ⑤ spine 上游面条目与 PROPOSAL 钉死文本**逐字一致**（含 `awaiting-upstream` 报法）；末句「五件齐 → 原样进 Step 1 intake」对应改到位；其余文字零改动（diff 单行内 1 换 1） |
| 7 | `AGENTS.md` | T5 | PASS* | 两段插在 :24 升级判据段与 landing 通道段之间，既有行零改动（diff 纯 +3 行）。*第二段「本批立」落文作「260928-gov-review-cross-face-intake-spine 立」——信息等价且更精确（指令根无「本批」语境），非逐字（分级差异 #4） |
| 8 | `.claude/scope-map.json` | T6 | PASS | agents 段新增 story-author / requirement-reviewer 两条目，形态 `allow: [], deny: ["**"]` 与 architect-reviewer 先例同形；universal 段与既有条目零改动；JSON 合法（V3 绿） |
| 9 | `.claude/route-fixtures/routing.json` | T6 | PASS | 两断言与 PROPOSAL 钉死文本逐字一致；8 个信号逐一 grep 核实为两 agent description 实文真子串（story-author：requirement-change.md / requirement-reviewer / 对偶 / 无 Write；requirement-reviewer：story-author / 四查 / 作者≠审者 / 不审契约）；commandEntry / agentRequiredToolsAgents 零改动；V2 route-regression 36 ok 0 fail |
| 10 | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | T7 | PASS* | append-only 合规（diff 纯 +2 行：空行 + 注记段，既有行零改动）；注记内容含建议 1/3/4 承载 + Track B 改判留痕句。*「改判原话留痕本批 PROPOSAL §4」落文丢「本批」二字（分级差异 #5）；V6 grep 计数 = 1 ✓ |

## 验证集独立复跑（本 verifier 实跑，非采信 summary）

- V1：完整门 = 上文 RED（阻断项）；--preflight 干跑形态侧本批不重跑（summary 自述三次绿与 PROPOSAL R2 判词吻合）。
- V2：`route-regression-check.mjs` → `36 ok, 0 fail`，exit 0。绿。
- V3：scope-map JSON parse → `scope-map ok`。绿。
- V4：`grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/` → 0 命中。绿（收窄 scope 后判据成立；集外 6 处描述性引用豁免与 PROPOSAL §3 census 一致）。
- V5：AGENTS.md :26/:27 两新增段各命中 ≥1。绿。
- V6：ARQ 条目 grep 计数 = 1。绿。
- V7：repo-structure :34/:35 两新行各命中。绿。

## packet-binding 核对

- landing-summary 引 packet = FROZEN PROPOSAL v1.0（§2 T1–T7 + §5 验证集），被批 review id = 260928-gov-review-cross-face-intake-spine，一致。
- landing 实际范围 = manifest 十路径，与 summary 清单表、PROPOSAL §2 各 T 目标锚并集三方吻合；**manifest 之外零 canonical 改动**（见破坏性自检）。scope ⊆ packet 成立。

## 破坏性自检

- `git status --short` 全量审视：改动面 = manifest 十件 + `.codebase-memory/artifact.json`（M）与 `graph.db.zst`（D，工具自刷新常态脏，既定豁免）+ 本批 packet 制品自身（architect-landing-summary.md untracked，属 review 证据目录）。**无越界件**。
- 契约面 `docs/contracts/**`：diff 为空，零触。hooks / settings 文件：diff 为空，零触（OQ-2 裁 (b) 复用既有 settings 的承诺兑现）。
- 自检完成度声明：十个文件 diff 全读，未抽查、未跳敏感面（本批最敏感面 = scope-map.json 强制层 + AGENTS.md 指令根 + routing.json 机检 fixtures，均逐字核）。

## 分级差异清单（均非阻断，随修复项一并处置或留 retro）

1. **[阻断]** landing-summary 缺 ```landing-manifest 围栏块 —— 结构门 RED，见上。修复后本 FAIL 即转 PASS 预期。
2. **[MINOR]** triage-change.md :13 落地句较 PROPOSAL 钉死替换句少「，R2-M1」注记 token——R2-M1 的实质内容（governance 面进枚举）在位；同批 T1 落文保留了 (R2-M2)/(R1-F4) 标签而此处未保留，属标注风格不一致，非语义差。
3. **[MINOR]** triage-change.md :32-35 替换块较 PROPOSAL 钉死替换句多三处：保留「人类触点③」字样、命令映射用原文全形（PROPOSAL 引句为缩写形，且 PROPOSAL 明文要求「原 :33-34 的命令映射语义并入上句保留」，故合规）、追加一行 headless 澄清句（「完整顶层会话非子 agent / 关键人类门 human 仍在场 / 到门即停」——恰为 PROPOSAL 对 :35 删除语义去向的条款原文，属授权范围内的显式承载，非私货）。
4. **[MINOR]** AGENTS.md 跨面 intake 段「本批立」→「260928-gov-review-cross-face-intake-spine 立」——信息等价且更精确，非逐字。
5. **[MINOR]** ARQ 注记「留痕本批 PROPOSAL §4」→「留痕 PROPOSAL §4」——丢「本批」二字，指向不变。
6. **[NOTE]** task-intake spine 源条目引号『』→「」；T1 实际 62 行（summary 自述 61 行，计数误差）。

## 根因假设

主线程落地时以 PROPOSAL 的 manifest 围栏为唯一清单源、summary 侧用表格复述而未按 authoring 格式契约复制围栏块；V1 只跑过 `--preflight` 形态（不检 summary 围栏），完整门在 landing 后从未实跑，故漏检。教训与 summary 自留的「生成即验证」教训同构：完整门应进 pre-commit 验证集而不止 preflight。

## 放行建议

**不放行 merge。** 退回主线程：补 landing-summary manifest 围栏块 → 重跑完整结构门取绿 → 回到本 verifier 复核（仅需复跑结构门 + 确认围栏块路径与 manifest 一致，语义层结论继续有效）。语义层十个文件实质落地全部达标，无越权、无私货、无漏改——修复完成后无其他已知障碍。

---

## 复核段（2026-09-28 第二轮 · 修复后重验）

**verdict（最终）：PASS（ok:true）**

主线程按第一轮放行建议落两处 summary 侧格式修复，本 verifier **独立重跑完整门 + 重读 summary 实文**复核（不采信修复自述）：

1. **manifest 围栏块**：`architect-landing-summary.md` 现含 ```landing-manifest 围栏块（十路径），与 PROPOSAL manifest 围栏块逐一、同序一致；「逐文件落地清单」表格保留未动。
2. **packet-binding 字面 token**：summary 头部现含字面 binding 行「packet-id = 本批 PROPOSAL §2 改法集（T1–T7）+ §5 验证集；…被批 review = 260928-gov-review-cross-face-intake-spine（R1 BLOCK→R2 GREEN）」——门要求字面 packet-id + 被批 review id token，此为第一轮 FAIL 未暴露的第二处缺陷（门首错即停，围栏块补齐后门往下一项才报出；第一轮 verdict 已声明该盲区并要求重跑取全量，此节即其闭环）。

**完整结构门独立复跑输出摘录**（本 verifier 实跑，2026-09-28）：

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260928-gov-review-cross-face-intake-spine
→ exit 0, ok: true
frozen: true · packetBinding: true · unauthorized: [] · claimedNotLanded: [] · plannedNotLanded: [] · gitDegraded: false
proposalTargets ≡ summaryTargets（十路径逐一吻合）
advisory.changedCanonicalNotInProposal = ["docs/showme/show-me-triage-intake-spine.html"]
```

**advisory 处置**：showme 讨论稿 `docs/showme/show-me-triage-intake-spine.html` 为 packet 外随批归档件，PROPOSAL §0 非目标 ⑥ 已显式声明「保持讨论稿定位、不进 manifest」，landing-summary「OQ 裁定与顺带项」已注明——非 scope creep，接受。

**修复纯度核**：两处修复均落在 summary（untracked packet 制品）内，十个 canonical 文件零改动（门 unauthorized/claimedNotLanded/plannedNotLanded 三空 + summaryTargets≡proposalTargets 实证）；第一轮语义层十文件核对结论与 MINOR 差异清单（差异 #2–#6）**继续有效、无需重核**。

**放行建议（更新）**：**放行**——结构门绿 + 语义层绿 + packet-binding 在位。merge 归 human manual-merge 门；Track B 第 6 步 retro 补写按流程继续。5 处 MINOR 逐字偏差（差异 #2–#6）不阻断，留 retro 一并记。
