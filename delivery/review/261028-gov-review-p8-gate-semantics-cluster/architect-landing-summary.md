# architect-landing-summary · 261028-gov-review-p8-gate-semantics-cluster

packet-id：FROZEN PROPOSAL（v0.3）§2 改法集（T1–T5，共 42 处旧串锚定编辑 + 1 件新增先例种子文件，manifest 15 件）+ §5 验证集 · 评审轮次 R1 BLOCK(F1) → R2 BLOCK(F1′) → R3 GREEN（三重实证闭合）· change-id gov-review-261028-gov-review-p8-gate-semantics-cluster

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 启动卡申报与先例生产口径 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（T1a/T1b）、`.claude/references/dev-loop-operating-system.md`（T1c） | launch-card 增机器可读「验证形态申报（JSON）」单行；human-decision 的 approvedVia + gatePrecedents 生产口径成文 |
| T2 supervisor 与 G6 同参降噪、逐轴先例折抵、空节噪音判别 | `.claude/hooks/supervisor-verdict-gate.mjs`（T2a–T2i）、`.claude/hooks/supervisor-gate-check.mjs`（T2j/T2k）、`.claude/references/dev-loop.md`（T2l） | 闸消费申报与已人批同型先例（仅 all-mock/generated-noise 白名单轴折抵）；ESCALATE 带先例快批提示；sdkconfig 纯空节噪音判别；G6 同参重算；dev-loop.md riskTier SSOT 段同步 |
| T3 设备轮硬门（pre-merge，含 KPLOG 证据最小要件） | `.claude/scripts/pre-merge-escalation-gate.mjs`（T3a–T3i）、`.claude/verify-gates.json`（T3j）、`.claude/supervisor-precedents.json`（copy 新增） | 设备轮证据为 supervisor 合并门 merge 期子判据，approve-escalated-merge 不豁免；三路触发（launch-card 申报 + acceptance 标记 + 实际 diff 触登记路径）；证据三形态 human-visual / serial-capture / KPLOG（段号+时间窗+包内脱敏摘录+SHA-256）；hostUncompilablePaths 首批登记 6 路径；先例种子 3 条反查在册 |
| T4 配套文面 | `.claude/agents/acceptance-author.md`（T4a）、`.claude/agents/builder.md`（T4b）、`.claude/agents/integrator.md`（T4c/T4d）、`.claude/references/dev-loop-operating-system.md`（T4e）、`.claude/references/dev-loop.md`（T4f）、`.claude/skills/kidpal-device-logs/SKILL.md`（T4g）、`.claude/agents/planner.md`（T4h/T4i） | chrome 三件自查句、设备结果标记成对文面、builder 行为面观察申报节、integrator 设备证据收口、defer-merge-pending-live-round 收口链承认成文、诊断日志保留债成文、KPLOG 证据引用契约、planner 两条人批路径对齐（先例须人明确授权、人批不豁免 deviceOutcome） |
| T5 两条 ARQ 结项记账 | `docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md`（T5a–T5e）、`docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md`（T5f–T5h） | 261014 路径漂移修正（scripts→hooks）；两条翻 closed + closedBy 本批 + 结项追注；debt-render 自动归位 `docs/debt/gov/_archive/2026/` |

## 2. 验证证据

- **V1 锚定装配（落地前全新输出根）**：`node .claude/scripts/assemble-edits.mjs …/_scratch/edits.json C:/tmp/p8-assembly-final` → `ok:true`、42/42 oldStr 恰命中一次、15 件目标文件、1 件新增 copy；pre-merge 装配件 sha256 = `fa09b79d1cc43a5f335210e11c7a35e15d78fbcda0bcc46d9363f2226d493046`，与 FROZEN §5 基线逐字一致。
- **V2 语法与 JSON（副本）**：三闸脚本 `node --check` 通过；verify-gates 与种子配置 `JSON.parse` 通过。
- **V3 三闸自检（副本）**：supervisor 96/96、pre-merge 88/88、supervisor-gate-check 行为电池 OK（10/10）。副本补齐 import 闭包（verifier-evidence-check.mjs + lib/ 三件，自仓原样复制，非 manifest 件）后运行。
- **V4 反转实现变红（判别脚本指向 p8-assembly-final）**：6/6 exit 1 且逐项命中指定 case（remove-precedent-requirement / expand-axis-whitelist / allow-substantive-sdkconfig-as-noise / allow-placeholder-device-exemption〔闸层 case〕/ skip-device-merge-gate / skip-kplog-segment-validation）。
- **V4b evaluate 五案直调**：5/5——A default-none 与 B 占位符均 `blocked=true`（命中 nonvisible-reason-missing）、C required-invalid 拦、D 干净非可见理由放行语义保持、E probe-failed 拦且 probe 前缀恰一次。
- **V5 历史种子反查（真仓根，apply 后）**：`readApprovedPrecedents(cwd)` → count=3、advisories=[]；三条种子逐一反查到真实 human-decision 事件（261010-prompt-v9 / 261012-kidpal-realtime-facts / 261023-agents-md-static-semantics-alignment）。
- **V7 仓内 apply 对账**：15 件自装配副本逐件拷回仓内后，sha256 与装配基线 afterSha256 **15/15 全对上**（byte 级一致）；V2/V3 在真仓复跑同值全绿。V4/V4b 判别脚本按设计以装配副本为被测对象——该副本与仓内 15 件 sha256 逐件相同，判别结论等价适用于仓内落地面。
- **V8 landing cite-check**：`ok:true`、`phase:landing`、`frozen:true`、targetCount 15、**checkedCount 43 / missing [] / unverified []**（42 个改后块 + 1 个新增文件块全部命中落地文件）；42 个原文块 notChecked 属 apply 后预期（原文已不在现盘）。
- **V9 角色与账本**：lint-descriptions 0 flagged；route-regression-check 104 ok / 0 fail；debt-render 两条 ARQ 自动归位 `_archive/2026/` + 两份 gov INDEX 再生；debt-doctor RED 零命中。

## 3. 诚实注记

1. **V4/V4b 被测对象是装配副本而非仓内原位**：判别脚本以 `C:/tmp/p8-assembly-final` 为被测根（判官 R2/R3 同口径）；与仓内落地面的一致性由 V7 的 15/15 sha256 对账承载，等价性成立，如实注明而非扩大声明。
2. **判别脚本路径指向已就地修改**：`_scratch/mutation-check.mjs` 与 `evaluate-five-cases.mjs` 原写死判官基线根 `C:/tmp/p8-assembly9`，本次落地改指 `C:/tmp/p8-assembly-final`（两脚本随 `_scratch/` 在落地 commit 前删除，不影响任何 manifest 或仓内面）。
3. **V3 副本运行需 import 闭包补齐**：装配副本缺闸脚本的共享 lib（verifier-evidence-check.mjs、lib/ 三件），自仓原样补复制后自检才可运行——这些文件不在 manifest、未落地任何改动，属装配器 copy 通道既有覆盖面之外的一次性补齐（若下批复发，可考虑纳入装配器 copy 清单惯例）。
4. **F5 NIT 已冻结前顺手修**（OQ-3 注记轮次措辞一行，判官 R3 明示许可），不涉改法集与 manifest。
5. **留根项照 FROZEN §6 七条全部保留**：结果决策抽缝（dev-loop 个案）、host 不可编登记集首批非完备、generated-noise 暂无历史种子、在途包迁移不追溯、原始日志保留期限、先例扫描规模、OQ-3 拆批备选。
6. **主线程 prose 校正**：无 landing-verifier 更正事项（本条按 P6.5 ADVISORY-1 先例预留位）。

```landing-manifest
.claude/hooks/supervisor-verdict-gate.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/supervisor-precedents.json
.claude/verify-gates.json
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/skills/kidpal-device-logs/SKILL.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/builder.md
.claude/agents/integrator.md
docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md
docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md
```
