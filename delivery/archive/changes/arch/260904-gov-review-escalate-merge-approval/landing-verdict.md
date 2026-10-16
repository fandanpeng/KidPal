# Landing Verdict · 260904-gov-review-escalate-merge-approval

> 核验人：landing-verifier（Track B 步骤 5，独立核验；只读 codebase，只写本 verdict）
> 日期：2026-09-05
> 判定依据：FROZEN `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集 + manifest 围栏块 + 实际 git diff / 实跑复现。**不采信 landing-summary 自述**——summary 每项自述均以独立 diff / 独立重跑实证。

## 0. 结论

**GREEN（放行建议）——零 BLOCKER；四项观察（均不阻塞）。**

manifest 10 路径中 9 个已落地路径的 diff 与 FROZEN PROPOSAL §2 逐条一致（T1 五要素 / T2 透传留痕 / T3 四处语义锚 / T4 五指针），无漏改、无私货、无权限或写域扩张；验证集 V1–V4 独立复跑全过（V3 之 quick-checks 项为存量失败，已用非 stash 方式实证与本批无关）；两处结构门机报错误均判读为预期状态 / 脚本局限，非内容缺陷。T4.6（ARCH-REVIEW-QUEUE 移已结）按 PROPOSAL 约定时序留待本 verdict 绿后执行——放行进入 T4.6 + retro + 前置升级闸；merge 仍归 human 门。

---

## 1. 结构门（机器）输出与判读

命令：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260904-gov-review-escalate-merge-approval`，EXIT=1，errors 三条：

| 机报错误 | 判读 | 依据 |
|---|---|---|
| 「§2 锚清单 ≢ manifest：仅 manifest 有 `changes/arch/.../PROPOSAL.md`」 | **脚本局限，非缺陷**。§2 锚清单 9 文件与 manifest 前 9 行逐一对应；manifest 第 10 行是 packet 制品 PROPOSAL 本体——R2 GREEN 附带核对明示「10 行：…+ ARQ 队列 + PROPOSAL 本体」，判官已批。脚本 `parseSection2AnchorLists` 只解析 §2「目标文件：」行，无「packet 本体自引用」概念，故对自引用必然报差。 | R2 记录（PROPOSAL §7）+ `governance-gate-shared.mjs:314-397` 解析域实证 |
| claimed-but-not-landed：`docs/ARCH-REVIEW-QUEUE.md` | **预期状态，非缺口**。FROZEN PROPOSAL T4.6 明文「landing 完成 + landing-verifier 绿后执行」；summary manifest 注与 §1 表「⏳ 待 verifier 绿」一致。git status 实证该文件确未改动（时序正确，未被抢跑）。 | PROPOSAL §2 T4.6 + summary §1 |
| planned-but-not-landed：`docs/ARCH-REVIEW-QUEUE.md` | 同上。 | 同上 |

其余前置硬断言（未冻结 / packet-binding / summary⊄proposal / manifest 块在场）均过——脚本只报了上述三条。**不可机判部分已全部转人工 diff 核验覆盖（§2）**。Track B 下设计侧门（proposal-consolidation-check）按规程 N/A，未跑。

建议留根：① T4.6 执行后重跑结构门（届时 claimed/planned 两条自消，§2 锚对账一条将因 PROPOSAL 自引用持续存在，属已判读脚本局限）；② 可立 ARQ 让 `landing-fidelity-check.mjs` 认识「manifest 可含 packet PROPOSAL 本体」。

## 2. 逐文件人读 diff 核对（manifest 10 路径）

| manifest 路径 | structuralTouched | semanticMatch | expected（FROZEN PROPOSAL §2） | actual（diff / 实证） |
|---|---|---|---|---|
| `.claude/scripts/pre-merge-escalation-gate.mjs` | ok（` M`） | **ok** | T1 五条 | 逐条命中：① 用法行新增，文字与 PROPOSAL 引文逐字一致；② `resolveHumanApproval(dir, changeId, eventId, deps)` 新增——主路径读 `<dir>/.claude/taskpackages/<id>/pod-events.jsonl`，`deps.podsRecordPath` 在场时回退 pods 记录 `worktreePath` 下同路径，逐行 JSON 找 eventId，五要素（eventType=human-decision / actor=human / payload.decision=approve-escalated-merge / changeId / schemaVersion=1）逐一 fail-closed、reason 点名失败要素；readText 可注入（`deps.readText`）对齐既有注入形态；③ `escalateExempt` 豁免语义 = kind devloop ∧ approval.ok ∧ verdict=ESCALATE ∧ provenance.ran ∧ producedBy——豁免 riskTier（:178）/ 非 APPROVE（:185）/ blocked（:169）三条，且 blocked 豁免仅限此情形（OQ2=(a)）；provenance 缺失理由（:183）无豁免分支、off-main（:152-161）/ 契约（:164-166）原样未动、verdict=BLOCK 时 escalateExempt 因 ≠ESCALATE 恒 false（:7 例 self-check 断言）；④ flag 核验失败 → `human-approval 核验失败：<reason>` BLOCK（:173 fail-closed）；gov/unknown 带 flag → exit 2（实测验证，见 §3）；⑤ self-check 新增 8 例名单与 PROPOSAL V1 八名逐一一致，既有 10 例断言零改动（mkCtx 仅加 `humanApproval: null` 默认，零行为变更）。`findPodsRecordPath` 抽取 = 原 blocked 内联逻辑等价重排（主仓优先 / git common dir 回退，语义不变）。PASS 行附 `human-approval verified (eventId=...)` ✓ |
| `.claude/scripts/merge-driver.mjs` | ok（` M`） | **ok** | T2 三条 | ① 用法注释补一行（:14）；② `HUMAN_APPROVAL_REF` 解析与 `HUMAN_EXEMPTION` 同形态（:46-50）；Step 1b `gateArgs` 数组按 flag 追加 `--human-approval-ref`（:627-629）透传，PASS log 附 `human-approval-ref=<eventId>`（:633）；③ merge message：`Merge ${changeId} (merge-driver)` 带 ref 时追加 ` human-approval-ref=<eventId>`（:739-741，OQ3=(a) 强制）。全 diff 仅此 4 hunk（19 变更行），无越界 |
| `.claude/agents/planner.md` | ok（` M`） | **ok** | T3 四处（:87/:91/:109/:137） | 四处「停人类」句尾各追加一段，:87 处与 PROPOSAL T3 语义锚引文**逐字一致**（含 OQ2=(a) 注）；:91 多 Pod 处同义 + 补「仍经 merge token 串行化」（PROPOSAL 授权的按文体裁剪，且与既有 token 纪律一致，无新权限）；:109/:137 同义锚齐。四处均为句尾追加，既有规程文本零删改 |
| `.claude/references/dev-loop.md` | ok（` M`） | **ok** | T4.1 | :103「ESCALATE 停人类」后补「；人批后机械路径见 planner.md 合并门段（`--human-approval-ref`）」，与 PROPOSAL 措辞一致；单行追加 |
| `.claude/references/pod-coordination.md` | ok（` M`） | **ok** | T4.2 | :99 ESCALATE token 表行尾补「人批后重占 token 走 merge-driver 人批路径（`--human-approval-ref`，见 planner.md 合并门段）」；单行追加 |
| `.claude/references/escalation-playbook.md` | ok（` M`） | **ok** | T4.3 | :15 ESCALATE 行补「人批后走 `merge-driver --human-approval-ref`（见 planner.md 合并门段）」；单行追加 |
| `.claude/commands/build-feature.md` | ok（` M`） | **ok** | T4.4 | :31「ESCALATE 才停人类 review/合并」后补同义指针（人批事件 + `--human-approval-ref`，指回 planner.md）；单行追加 |
| `.claude/references/code-review-gate.md` | ok（` M`） | **ok** | T4.5（R1 F1 增补） | :89「ESCALATE 才进入人类合并门」后补同义指针；单行追加 |
| `docs/ARCH-REVIEW-QUEUE.md` | 未动（`git status` 零改动） | N/A（时序待办） | T4.6 留待 verifier 绿后 | 与 PROPOSAL 约定时序一致，非缺口（见 §1） |
| `changes/arch/.../PROPOSAL.md` | ok（untracked 新目录整体） | **ok** | R1 修订（F1/F2）+ 状态行 FROZEN | manifest 与 summary 两份围栏块逐行一致（10 行）；F1 三处在场（T4.5 项 / §3 升类行 / manifest 增行）；F2 三行「未触」补录在场；头部状态行 FROZEN + R2 GREEN 在 §7；§7 R1/R2 记录完整 |

**私货 / 夹带核验**：9 个已落文件 diff 内未发现 PROPOSAL 未授权改动；无权限/写域扩张语句；G3 敏感面（两闸脚本 + planner 规程）逐行核对与被批 packet 一致——豁免条件窄化正确（devloop 限定 + ESCALATE 限定 + provenance 硬前置）、BLOCK/provenance/off-main/契约永不豁免、gov/unknown 防误用 exit 2 实测成立。**manifest 外零落地**：git status 全量核对，tracked 改动仅 8 个 manifest 内 canonical 文件 + `changes/INDEX.md`（见观察项 O1），untracked 仅本变更目录；`supervisor-verdict-gate.mjs`、`docs/contracts/**`、`docs/DEV-GOVERNANCE.md`、`.claude/taskpackages/**`（pod-events 写入侧）均零触碰，与 §0 非目标一致。

## 3. 验证集独立复跑（原始输出要点，2026-09-05）

- **V1**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"selfCheck":"pre-merge-escalation-gate","pass":18,"fail":0,"total":18,"ok":true}` EXIT=0。10 既有 + 8 新增，total ≥ 18 达标。✅
- **V2**：`node --check` 两件脚本零输出通过。✅
- **V3**：`--self-check-detect-changes` → `{"pass":16,"fail":0,"total":16,"ok":true}`；`--self-check-rebase-target` → `{"pass":7,"fail":0,"total":7,"ok":true}`；`--self-check-quick-checks` → `FATAL: verify-gates.json missing quick-check gate id="types" (ARQ-097 single source)`。**存量判定未用 git stash，改用构造性实证**：`.claude/verify-gates.json` 本批 diff 为 0 行且其实测 gate id 清单确无 `types`；merge-driver 触发该错的 gate 查找代码（HEAD :419）不在本批 4 个 hunk 内。即「同一数据 + 同一代码路径」在 HEAD 上必然同样 FATAL——与本批无关实证成立。✅（V3 以 2/3 + 存量豁免达成）
- **V4**（Pod S 数据双向重放，独立实跑）：
  - 带 flag：`... 260829-smoke-e2e --source-ref dev/260829-smoke-e2e --human-approval-ref 86c73079-89f7-41c2-8c88-38e5a679b09d` → BLOCK 理由仅剩 `off-main：无 main ref 无法判（fail-closed）` + `docs/contracts/** diff…`，**不含** `riskTier=` 与 `非 APPROVE` ✅（事件核验真通过：事件五要素在主仓 `.claude/taskpackages/260829-smoke-e2e/pod-events.jsonl` 逐字段实核在场，schemaVersion 由闸内五要素机判通过）
  - 对照组（去 flag）：`riskTier=high（supervisor 升级）` 与 `supervisor verdict=ESCALATE（非 APPROVE）` 两条回来 ✅
  - PROPOSAL V4 预期「off-main 理由仍在」以 fail-closed 变体成立（见 O2）。
- **补充边界实测**（超出 V 集，verifier 自加）：gov 档带 flag → `FATAL: --human-approval-ref 仅适用于 dev-loop 档…` EXIT=2 ✅；错误 eventId → BLOCK 理由新增 `human-approval 核验失败：eventId=… 不在 pod-events 中` 且 riskTier/非 APPROVE 保留 ✅（fail-closed）。

## 4. 破坏性自检

- **既有闸强度**：默认无 flag 路径行为零变更（无 flag 时 `humanApproval=null`，escalateExempt 恒 false，全部理由照推；self-check 既有 10 例零改动且 18/18 绿佐证）。既有 10 例语义未被改。
- **豁免边界**：豁免三条件缺一不可且全部机器可判；BLOCK / provenance 缺失 / off-main / 契约四面实测均不可借 flag 穿越 nonzero；flag 乱填（假 eventId）fail-closed。
- **summary §3 两插曲定性复核**：插曲 1（live `buildCtx` 返回对象确无 `execFn` 字段，`evaluate` :153 恒 fail-closed「无 main ref 无法判」）——代码实证 + V4 双向输出实证，定性诚实准确；且 HEAD 版 buildCtx 返回对象同样无该字段（本批 diff 未触及该行），确为存量。插曲 2（quick-checks 存量失败）——§3 构造性实证成立。两处「不修属 packet fidelity 纪律」的判断正确（修复面均不在 FROZEN manifest 内）。
- **verifier 自身**：全程只读 codebase（git diff/show/status、grep、node 实跑、临时提取到 c:\tmp 的 HEAD 副本因相对 import 不可运行已弃用、未触碰工作树）；唯一写入 = 本 verdict 文件。未写任何 canonical 面、未触契约面、未 push。自检通过。

## 5. 观察项（不阻塞，建议随 retro / 留根处理）

- **O1 · `changes/INDEX.md` diff 为全量重建而非单行**：本批在 manifest 外改动 INDEX.md。内容侧是 `changes-index.mjs`（auto-generated 投影，头部明示「勿手改」「不做拦阻依据」）在 FROZEN 钉行时幂等重建所致：新增 260904 frozen 行 ✓，同时把此前损坏的 dev 段（「dev (0)」标题下粘连一行孤行）修复为规范表、并补齐既有 260829-gov-review-migration-tier23 等陈旧缺行、260903-gateway-directionB 状态 closed→open。属投影修复 + 流程产物，非 landing scope creep；与 260830 批 verdict 对 INDEX 钉行的先例定性一致。
- **O2 · off-main live 恒 fail-closed（存量缺陷，summary 插曲 1 已诚实披露）**：本批 V4 的「向严保持」以 fail-closed 变体成立。summary 建议补 ARQ——同意，建议登记（修复面小：buildCtx 补 `execFn` 字段）。
- **O3 · 结构门 §2 锚对账不识 PROPOSAL 自引用**：见 §1 留根建议 ②。
- **O4 · 人批事件时序绑定残差**：R1 观察项（五要素不含时序绑定）与 §6 留根第 1 条（LLM 可写事件、H5 对称）已覆盖，维持「登记不硬治」。

## 6. packet-binding 核对

landing-summary 引 packet-id = FROZEN PROPOSAL §2（T1–T4）+ §5；被批 review id = 260904-gov-review-escalate-merge-approval（R1 需修订 → R2 GREEN 在场）✓。PROPOSAL 头部状态行 FROZEN（2026-09-05，立项 ack + R2 GREEN 前提齐）✓。landing 实际范围 ⊆ 被批 manifest（10 路径，其中 9 已落 + 1 按约时序）✓。PROPOSAL 与 summary 两份 manifest 围栏块逐行一致 ✓。

---

**最终 verdict：GREEN。** 实际落地 == FROZEN PROPOSAL landing 清单（T4.6 按 PROPOSAL 约定时序留待，非缺口）。放行建议：进入 T4.6（ARQ 移已结 + `changes-index.mjs --upsert`）→ retro → 前置升级闸；merge 仍归 human 门，本 verdict 不替 human 拍板。
