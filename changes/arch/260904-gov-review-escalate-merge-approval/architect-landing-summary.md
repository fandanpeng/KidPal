# Landing Summary · 260904-gov-review-escalate-merge-approval

> 落地人：主线程（Track B 步骤 4，packet-bound）
> 日期：2026-09-05
> packet-id = FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集；packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 被批 review id = 260904-gov-review-escalate-merge-approval（本目录；R1 需修订 → R2 GREEN，见 PROPOSAL §7）

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
.claude/agents/planner.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/escalation-playbook.md
.claude/commands/build-feature.md
.claude/references/code-review-gate.md
docs/ARCH-REVIEW-QUEUE.md
changes/arch/260904-gov-review-escalate-merge-approval/PROPOSAL.md
```

> manifest 注：`docs/ARCH-REVIEW-QUEUE.md` 的 T4 第 6 项（ARQ 移已结 + 索引钉行）按 PROPOSAL 约定留待 landing-verifier 绿后执行；落地时序见 §4。

## 1. landing 清单执行对照

| manifest 路径 | 动作 | 落地内容 | 状态 |
|---|---|---|---|
| `.claude/scripts/pre-merge-escalation-gate.mjs` | 改 | T1：用法行补 `--human-approval-ref <eventId>`；新增纯函数 `resolveHumanApproval(dir, changeId, eventId, deps)`（读 pod-events.jsonl + pods 记录 worktreePath 回退，五要素核验 fail-closed，readText 可注入）；`evaluate` 增 `escalateExempt` 豁免语义（ESCALATE + provenance 完整 + 核验 ok → 豁免 riskTier/非 APPROVE/blocked 三条理由；BLOCK/provenance 缺失/off-main/契约永不豁免；flag 核验失败 → BLOCK 点名失败项）；`buildCtx` 增第 4 参 + `findPodsRecordPath` 抽取（原 blocked 内联逻辑等价重排）；入口 flag 解析 + gov/unknown 带 flag exit 2 防误用 + PASS 行附 `human-approval verified (eventId=...)`；self-check 增 8 例 | ✅ 已落 |
| `.claude/scripts/merge-driver.mjs` | 改 | T2：用法注释补一行；`HUMAN_APPROVAL_REF` 解析（与 `HUMAN_EXEMPTION` 同形态）；Step 1b `execFileSync` 参数数组按 flag 追加 `--human-approval-ref` 透传 + PASS log 带凭据；merge commit message 按 OQ3=(a) 强制追加 `human-approval-ref=<eventId>` | ✅ 已落 |
| `.claude/agents/planner.md` | 改 | T3：四处 ESCALATE 分支（单 Pod :87 / 多 Pod :91 / 合并门段 :109 / 最终交付段 :137）句尾统一补「human 批准合并后」机械路径（语义锚 = PROPOSAL T3 引文，按各处文体裁剪；多 Pod 处补注 merge token 串行化仍适用） | ✅ 已落 |
| `.claude/references/dev-loop.md` | 改 | T4.1：:103 supervisor 合并门段「ESCALATE 停人类」句尾补指针（人批后机械路径见 planner.md 合并门段 `--human-approval-ref`） | ✅ 已落 |
| `.claude/references/pod-coordination.md` | 改 | T4.2：:99 ESCALATE token 表行补指针（人批后重占 token 走 merge-driver 人批路径） | ✅ 已落 |
| `.claude/references/escalation-playbook.md` | 改 | T4.3：:15 ESCALATE 行补同义指针 | ✅ 已落 |
| `.claude/commands/build-feature.md` | 改 | T4.4：:31 步骤 10「ESCALATE 才停人类 review/合并」句尾补同义指针 | ✅ 已落 |
| `.claude/references/code-review-gate.md` | 改 | T4.5（R1 F1 增补项）：:89「ESCALATE 才进入人类合并门」句尾补同义指针 | ✅ 已落 |
| `docs/ARCH-REVIEW-QUEUE.md` | 改 | T4.6：ARQ-260902-escalate-merge-no-human-path 移已结 + 索引钉行——**landing-verifier 绿后执行**（PROPOSAL 约定时序） | ⏳ 待 verifier 绿 |
| `changes/arch/260904-.../PROPOSAL.md` | 改 | R1 修订（F1/F2：T4 增补 + census 补行 + manifest 增行）+ 状态行 DRAFT→FROZEN | ✅ 已落 |

manifest 外零落地：`supervisor-verdict-gate.mjs`（判级逻辑）、`docs/contracts/**`、`docs/DEV-GOVERNANCE.md`、pod-events 写入侧均未触碰（PROPOSAL §0 非目标）。

## 2. 验证集重放（PROPOSAL §5，landing 后自检，2026-09-05）

- **V1 升级闸 self-check**：`{"selfCheck":"pre-merge-escalation-gate","pass":18,"fail":0,"total":18,"ok":true}` ✅（10 既有 + 8 新增全绿，total ≥ 18 达标；8 例名单与 PROPOSAL V1 一致：escalate-no-approval-still-block / escalate-with-approval-pass / approval-wrong-eventid-block / approval-actor-not-human-block / approval-wrong-decision-block / approval-wrong-changeid-block / block-verdict-not-waived / escalate-provenance-missing-not-waived）
- **V2 语法闸**：`node --check` 两件脚本零输出通过 ✅
- **V3 merge-driver 既有自检**：`--self-check-detect-changes` 16/16 ✅、`--self-check-rebase-target` 7/7 ✅；`--self-check-quick-checks` **FATAL（既有）**——`verify-gates.json missing quick-check gate id="types" (ARQ-097 single source)`。已用 `git stash` 在 HEAD 上复跑确认为**存量失败、与本批无关**（本批未触 quick-check/verify-gates 路径，且该路径属非目标）；见 §3 插曲 2。
- **V4 真实重放（Pod S 数据）**：
  - 带 flag：`node .claude/scripts/pre-merge-escalation-gate.mjs 260829-smoke-e2e --source-ref dev/260829-smoke-e2e --human-approval-ref 86c73079-89f7-41c2-8c88-38e5a679b09d` → BLOCK 理由中**不再含** `riskTier=` 与 `非 APPROVE`（真实事件五要素核验通过）✅
  - 对照组（同命令去 flag）：两条理由回来 ✅
  - 两个方向均同时出现 `off-main：无 main ref 无法判（fail-closed）`——见 §3 插曲 1（既有缺陷），「off-main 理由仍在（向严保持）」的 PROPOSAL 预期以 fail-closed 变体成立。

## 3. landing 过程插曲（如实记录）

1. **存量缺陷（非本批引入，未修，建议补 ARQ）**：`pre-merge-escalation-gate.mjs` live 路径 `buildCtx` 返回的 ctx 恒不含 `execFn` 字段，而 `evaluate` 的 off-main 判定 `sourceReachableFromMain(ctx.execFn, ...)` 依赖它——live 运行时 `ctx.execFn === undefined`，`revParse` 抛错被吞，off-main 判定**恒** fail-closed 成「无 main ref 无法判」。self-check 能过是因为 mkCtx 手工注入了 execFn。V4 重放实证了该行为。该缺陷方向是 fail-closed（不产生错误放行），且修复不在 FROZEN manifest 半径内（改之破 packet fidelity），本批按规不动手；建议以 gov ARQ 登记（修复面小：buildCtx 补一个字段）。
2. **V3 `--self-check-quick-checks` 存量失败**：verify-gates.json 缺 `types` 门定义（ARQ-097 单源检查红），HEAD 复跑确认与本批无关。本批不改该面（非目标）；该失败使 V3 的「全绿」只能以 2/3 达成，如实记录。

## 4. 状态转移记录

- 2026-09-04 human 立项 ack + OQ1–OQ3 全裁 (a)。
- 2026-09-05 architect R1「需修订」（F1/F2 census 打回）→ 主线程修订 → R2 **GREEN**（PROPOSAL §7）→ 主线程凭「立项 ack + GREEN」标 **FROZEN**，packet-binding 生效；`changes-index.mjs --upsert` 钉 frozen。
- 2026-09-05 主线程 packet-bound 落地 T1–T4（本 summary §1 清单）→ 验证集重放（§2）→ 本 summary 写就，交 landing-verifier 独立核验（Track B 步骤 5）→ **landing-verifier GREEN**。
- 2026-09-05 T4.6 已执行（ARQ 移已结 + 两条新 ARQ 登记）+ retro 写就 + 包 commit 至 `gov/260904-escalate-merge-approval`（84ef6eb）。
- 2026-09-05 合并门前闸校准（如实记录，非 landing 内容）：① 状态行按机器锚正则格式补版本号（FROZEN → FROZEN v1.0，满足 `readGovAnchors` `/^\*\*状态：FROZEN v\d+\.\d+/`）；② 合并门被存量缺陷 `ARQ-260905-escalation-gate-live-execfn-gap` 拦（off-main 恒 fail-closed），human 拍板**同一批热修**（2026-09-05，先例 ARQ-260904-gate-regex-uppercase-changeid）——buildCtx 补 `execFn` 字段一行修，复验 self-check 18/18 + V4 重放 off-main 恢复真判（Pod S 分支正确出 `source-on-main`、本批分支正确无 off-main 理由）→ ARQ 标已结；③ 发现第二处机器锚格式问题：`readGovAnchors` frozen 正则要求行首 `**状态：FROZEN vX.Y`，而本仓 PROPOSAL 惯用 `> ` 引用前缀——即该机锚在本仓从未命中过任何实际 PROPOSAL（含已合并的 260830 批），本批状态行去掉前缀适配之；此为闸脚本侧格式与仓内文档形态的既有错位，登记观察、不扩修。
- 待办（verifier 绿后）：T4.6 ARQ 移已结 + 索引钉行 → retro（步骤 6）→ 前置升级闸（gov 档，FROZEN+GREEN+landing-verdict 机锚）放行 → 自动合并本地 main（步骤 7，可回滚；remote push 留 human）。
