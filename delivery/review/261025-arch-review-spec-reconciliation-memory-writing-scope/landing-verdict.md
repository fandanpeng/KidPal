# landing-verdict · 261025-spec-reconciliation-memory-writing-scope

- 核验者：landing-verifier（独立会话，只读 codebase + 只写本 verdict）
- 核验对象：主线程 packet-bound landing（未 commit，工作树三文件改动）
- packet-binding：landing-summary 引 packet-id = FROZEN PROPOSAL §2（T1–T9）+ §5 验证集，评审轮次 R1 GREEN——与被批 review id `261025-spec-reconciliation-memory-writing-scope` 一致；landing 实际范围（git status 三文件）⊆ manifest 三行。

## 结论

verdict: PASS

Track B 常驻轻路径主路径批；实际落地 == FROZEN PROPOSAL §2 T1–T9 逐字保真，无第四文件、无私货、无漏改。

## 两道机械门（亲跑）

1. **结构门** `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope` → `ok: true`、`frozen: true`、`packetBinding: true`、proposalTargets == summaryTargets == manifest 三行、`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []`、`gitDegraded: false`。
   - advisory `changedCanonicalNotInProposal: ["docs/requirements.md"]` **已排查、非本批**：该改动来自 HEAD~1（commit e9f5d8dd，需求面 v1.68 冻结，属需求面 lane 自己的 landing）；工作树内 `git diff HEAD -- docs/requirements.md` 为空、`git status --porcelain` 不含该文件。结构门 baseRef 含末次 commit 所致，误报源澄清。
2. **设计侧门**：Track B——证据集无 decision-packet，`proposal-consolidation-check.mjs` 不跑（N/A）；「frozen PROPOSAL ↔ 批定一致性」由 §7 R1 GREEN + 状态行 FROZEN v1.0 承载（亲读在场，verdict 行 `verdict: GREEN`）。
3. 附加机检 `spec-anchor-check.mjs` 亲跑 → `ok: true`、errors []、storyCount 14、archSectionCount 48（与 PROPOSAL §5 第 1 条预期一致）。

## 逐文件核对表

| 文件 | T 项 | diff 实测 | 语义保真 |
|---|---|---|---|
| `docs/architecture.md`（+33/−10） | T1 / T2 / T3 / T4 / T5 / T6 / T9 | 见下分项 | ok |
| `docs/decisions/ADR.md`（+1/−0） | T7 | ADR-016 条目末（关联行后、文件末行 :142）追加一条 2026-10-26 注记；numstat +1/−0 实证决策正文零改；ADR-016 为文件末条目（:135 header），未插入其他条目之前 | ok |
| `docs/glossary.md`（+1/−0） | T8 | 「记忆子系统」词条之后新增「平台内部系统回合」词条，逐字 == PROPOSAL T8；既有词条零改 | ok |

### architecture.md 分项（全部逐字比对 PROPOSAL §2 目标文本）

- **T1**：`记忆写入范围（以已验证行为为准）` 整段替换为四段——①总段（含 2026-10-22 误写沿革更正）②类级规则三类（孩子的原话 / 小伴或系统生成的回复产物 / 平台内部系统回合，含「类别优先于内容」优先序句）③「记忆巩固」两义区分（OpenClaw dreaming vs Honcho dream）④两把杠杆实况与界限（isSystemRun 三条件 + captureSystemRuns=false；noisePatterns 已退役；生成器全局关闭、关的是生成器不是采集分类、重开即复发）——逐字一致。
- **T2**：表前引导句、第 1/2/3/5 行、新增第 7 行（置于第 6 行后）、表后注记（补 Honcho 自身 dream 非回合句）——逐字一致；第 4/6 行未动（PROPOSAL 未授权改，实况也未动）。
- **T3**：新增「**残余与未闭合申报**」块，置于表后注记与「记忆身份」段之间；（一）–（八）八条齐全、逐字一致，无遗漏、无自行增补。
- **T4**：记忆身份段尾追加句（一次性回合携 device 身份 + 降级指向残余（五））逐字一致；来源门 ① 整句替换逐字一致。
- **T5**：§5.1.2 front 行「不携 `user`」句整句替换为「携 `user=<device_id>` 落同一设备 session 键族」版本，逐字一致，整行其余文字未动；`:203` 记忆子系统行实读（sed :203）仍为指针式原句，判零改动兑现。
- **T6**：§4 NFR-PRIV-1 映射行第二列替换逐字一致。
- **T9**：§12 变更记录行插入于表头分隔行之后、261023 行之前（新行置顶），行文本逐字一致——N2 所指「:488 实为 :487」的笔误在落地时按语义正确执行。

## 破坏性自检

- `docs/requirements.md`：工作树零改动（git status / git diff HEAD 均空）——需求面 v1.68 frozen 未被本批回改。✅
- `docs/contracts/**`：零触碰（git status 无该路径）。✅
- 第四文件被动：无——`git status --porcelain` 仅三目标文件 + 本评审目录（untracked，verdict 自身）。✅
- 私货排查：architecture.md 全部 diff hunk 一一对应 T1/T2/T3/T4/T5/T6/T9，无超 packet 改动；ADR/glossary 各恰一行对应 T7/T8。✅

## §5 验证集 grep（独立复跑，非转述采信）

| # | 命令要点 | 预期 | 实测 |
|---|---|---|---|
| 5 | `grep -c "noisePatterns" docs/architecture.md` | =1 | 1 ✅ |
| 6 | `grep -n "平台内部系统回合"` | architecture ≥3 且 glossary =1 | architecture 7、glossary 1 ✅ |
| 7 | `grep -c "dreaming-narrative-memory-core-v2" docs/architecture.md` | ≥1 | 2 ✅ |
| 8 | `grep -c "oneshot_user=absent" docs/architecture.md` | ≥1 | 1 ✅ |
| 9 | `grep -n "2026-10-26 注记" docs/decisions/ADR.md` | =1 且落 ADR-016 内 | :142，ADR-016（:135 起）条目末行 ✅ |
| 10 | `grep -n "261025-spec-reconciliation-memory-writing-scope" docs/architecture.md` | ≥1 | :511（§12 行）✅ |
| 附 | `grep -c "captureSystemRuns" docs/architecture.md` | T1/T2 记载 | 2（两把杠杆段 + 表第 3 行）✅ |

## 非阻塞备注核对（N1/N2/N3）

- **N1**（census「一次性回合」行位笔误）：落地按语义正确执行——T5 改的是 `:192` front 行（diff 在场）、T6 改的是 `:155` 映射行（diff 在场），两处均落地。与备注一致。
- **N2**（T9 表头行 :487/:488）：新行插入位置 = 分隔行之后置顶，意图兑现。与备注一致。
- **N3**（T7 注记「残余逐行申报于 arch §6.7」系引用式复述）：注记按 PROPOSAL 原文落地，未自行改写。与备注一致。
- 三条均为 PROPOSAL 引用行号/措辞层面的笔误注记，落地动作与评审裁定一致，不构成偏离。

## ADVISORY 清单（非阻断观察）

- **ADV-1（summary 记账小误）**：landing-summary §2.4 自述 architecture.md 「+43/−10」，`git diff --numstat` 实测为 **+33/−10**（43 是 `--stat` 图宽列，被误读为插入行数）。仅 summary 自述数字不准，实际落地内容不受影响；建议主线程在合并前顺手更正 summary 该数字（可选）。
- **ADV-2（结构门 advisory 澄清）**：`changedCanonicalNotInProposal` 含 docs/requirements.md，已核实为 HEAD~1 需求面 lane commit 所致、非本批（见「两道机械门」节）。

## 放行建议

绿。建议进入 commit + retro + 前置升级闸流程；merge 决定权归 human manual-merge 门。
