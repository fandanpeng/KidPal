# Landing Verdict · 260905-gov-review-verify-gates-align

> 核验人：landing-verifier（Track B 步骤 5，独立于落地者）
> 日期：2026-09-05
> 核验对象：FROZEN PROPOSAL §2（T1–T5）+ §5 验证集 + 头部 landing-manifest（6 路径）vs 当前工作树未提交改动
> 判定来源：机械脚本退出码 + 逐文件人读 diff 实证；不以 landing-summary 自述为通过依据

## 1. 核验方法

1. 结构门 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260905-gov-review-verify-gates-align`（机械）；
2. 逐文件人读 diff：`git diff HEAD -- <manifest 路径>` 逐一比对 FROZEN PROPOSAL §2 改法；
3. 验证集 V1–V5 独立复跑（不复述 summary 自述）；
4. 破坏性自检：HEAD 旧版行为静态证明 + 正则误判面评估 + manifest 外全量 git status 对账。

## 2. 结构门输出（机械）

```
ok:false, errors: ["PROPOSAL §2 锚清单 ≢ manifest 围栏块（对账失配）：仅 §2 有：（无）；仅 manifest 有：changes/arch/260905-gov-review-verify-gates-align/PROPOSAL.md"]
```

**定性：脚本已知局限，非真实失配。** 实读脚本逻辑（`landing-fidelity-check.mjs:262-275`）：取 §2 各 T 目标文件并集（T1–T5 = 5 文件）对账 manifest（6 路径）；唯一差异项恰为 manifest 中的 PROPOSAL 本体——脚本不识 manifest 含 PROPOSAL 自身条目（R2 评审记录 §7 已确认 6 路径 = T1–T5 目标文件 + 本批制品，恰为并集无缺无余）。**不可机判部分转人工覆盖**：PROPOSAL 本体的「落地」= R1 修订（F1/F2+NIT×2）+ FROZEN 状态行，人工 diff 核验见 §3 第 6 行。其余结构判据（frozen 在场 / 双侧 manifest 围栏块在场 / packet-binding 行在场）均过，`gitDegraded:false`。

## 3. 逐文件 diff 核对表

| file | structuralTouched | semanticMatch | expected（PROPOSAL §2） | actual（diff 实证） |
|---|---|---|---|---|
| `.claude/scripts/merge-driver.mjs` | 是（diff 3 hunk，均在 T1/T1.5 指定区域） | ok | T1.1 id 表 → `['contract-lint','acceptance']` + 头注释改写（firmware-build 不入冒烟三理由）；T1.2 oracle 2 条不动 + 补对齐注；T1.3 三函数逻辑不动；T1.4 `:807` 位置不动；T1.5 `:687-688` 注释与 `:704` warn 去 types/schema 字样 | 逐字核对全部命中：`QUICK_CHECK_IDS = ['contract-lint', 'acceptance']` ✓；EXPECTED_QUICK_CHECK_CMDS 2 条本体未动、仅加注 ✓；`:807` `buildQuickChecks` 仍先于 DRY_RUN 判断、无 hunk 触及 ✓；豁免注释改「其余 contract-lint 已过、照拦；260905 对齐两门案」✓；warn 串改「contract-lint already passed and remain enforced」✓。diff 全文无 T1 区域外改动（rebase/重试/exemption/token 语义零触碰）✓ |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | 是（单行） | ok | T4：`/^\*\*状态：FROZEN v\d+\.\d+/m` → `/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m`，行内注记出处 | diff 恰一行，正则与 PROPOSAL 逐字一致，行内注「260905 T4：容忍 `> ` 引用前缀，与行首形态双命中（既有错位归位，260905-gov-review-verify-gates-align）」在场；同函数 green 锚正则未动 ✓ |
| `.claude/verify-gates.json` | 是（仅 `_doc`） | ok | T2：`_doc` 追加注（两门案决策 + firmware-build 不入冒烟 + types 缺口照旧）；gates 数组零改动 | `_doc` 恰追加 3 行，内容覆盖 PROPOSAL T2 全部要点（含 ARQ-260827 邻域指认）；gates 九门零 diff ✓ |
| `.claude/references/pod-coordination.md` | 是（单行） | ok | T5：`:72` 三门集名 → 两门案表述 | diff 恰一行，改后文案与 PROPOSAL T5 目标句**逐字一致**（含「260905 对齐后两门案」及 firmware-build 括注）✓ |
| `docs/ARCH-REVIEW-QUEUE.md` | 否（未动） | ok（预期时序） | T3：ARQ 移已结 + 索引钉行，**PROPOSAL 约定 landing-verifier 绿后执行** | git status 无此文件改动——按 PROPOSAL T3 时序属预期状态，不算缺口 ✓ |
| `changes/arch/260905-.../PROPOSAL.md` | 是（新文件，untracked） | ok | R1 修订四项到位 + 状态行 FROZEN v1.0 + R2 GREEN | 头部状态行 FROZEN v1.0 在场（行首无引用前缀，双形态兼容）✓；§7 R1「需修订」+ R2「GREEN（可冻结）」在场，R2 逐条确认 4/4 打回项到位、manifest 6 路径一致性表 ✓；§2 含 T5、T1.5，packet-id 行 T1–T5 ✓ |

## 4. 验证集独立复跑（V1–V5，2026-09-05 实跑）

- **V1**：`--self-check-quick-checks` → `{"ok":true, "gotIds":["contract-lint","acceptance"], "gotCmds":[2 条]==expectedCmds}`，exit 0 ✓
- **V2**：`node --check` 两脚本零输出通过；`JSON.parse(verify-gates.json)` 合法 ✓
- **V3**：`--self-check-detect-changes` 16/16 ok ✓；`--self-check-rebase-target` 7/7 ok ✓
- **V4**：`--dry-run` 与 `--dry-run --pod 260829-smoke-e2e` 全输出 grep `verify-gates.json missing` 均 **0 命中**，走到 `board: 15 pods → no ready-to-merge pods; nothing to do`，exit 0 ✓（`:807` 取门通过、运行时 FATAL 消除实证）
- **V5**：`node -e` 三输入对同正则：`> ` 前缀 true / 行首 true / `**状态：DRAFT` false ✓；升级闸 `--self-check` 18/18 ok ✓

## 5. 破坏性自检

1. **冒烟子集 4→2 门是否弱化防护：否，成立「从全死恢复到两门真跑」。** 静态实证：`git show HEAD` 版 merge-driver id 表 = `['types','schema','contract-lint','acceptance']`，HEAD 版 verify-gates.json 九门无 `types`/`schema`，`buildQuickChecks` 对首个取不到的 id 即 `errorExit`，且 `:807` 先于 DRY_RUN——即迁移以来**任何**运行时调用（含 dry-run 与真实合并）无条件 FATAL，名义 4 门从未执行过一扇。本批后 V1/V4 实证 contract-lint + acceptance 两门真跑。无既有有效防护被削弱，无新增放行路径。
2. **T4 正则 `[>\s]*` 误判面：有界，与 R1-F5 判定一致。** 新增误判面 = 正文/围栏内引用形态的 FROZEN 状态行（fail-open 方向 frozen:true）；gov 档另有 green 锚（GREEN+verdict 同段）+ landing-verdict.md 存在性双锚兜底（`readGovAnchors` 实读确认 landingVerdictPath existsSync 检查未动）。`**状态：DRAFT` 不误匹配（V5 第三断言实证）。
3. **260904 批状态行双形态冗余：成立。** 实测该批 PROPOSAL 状态行（行首无前缀形态）在新正则下命中 true。
4. **核验完整性自检：** manifest 6 路径全部逐一核（含 PROPOSAL 本体与待办 T3），未跳过敏感面——T4 触 G3 强制层、T1/T2 触 G3 合并执行链与门单源，均逐字核；无只挑好看文件。

## 6. manifest 外改动对账（git status 全量）

- 5 已落路径 + 1 新增 review 目录 + `changes/INDEX.md` + `changes/.changes-index-state.json`。
- `INDEX.md`：新增 260905 frozen 单行 = AGENTS.md 治理路由第 7 步「状态转移时 changes-index.mjs --upsert 钉行」的协议规定动作，summary §4 如实记录。**观察项（非越权）**：此协议动作发生在 manifest 之外，本批 manifest 未含 INDEX.md；属既有七步闭环的设计使然，如实记录即可。
- `.changes-index-state.json`：新增 260904 mergedInMain 条目 = **上一批（260904）合并的状态残留**，非本批 packet 内容；不影响本批语义面。观察项。
- verify.mjs / DEV-GOVERNANCE / 契约面 / merge-driver 其余合并链语义：零触碰，与 summary「manifest 外零落地」声明一致 ✓。

## 7. Verdict

**GREEN** — 实际落地 == FROZEN PROPOSAL 的 landing 清单（T1–T5 + PROPOSAL 修订），两处机械门偏差均为脚本已知局限且已人工覆盖，验证集 V1–V5 独立复跑全绿，破坏性自检三项均成立。

```json
{"ok": true, "changeId": "260905-gov-review-verify-gates-align", "verdict": "GREEN", "structuralGate": "pass-with-known-limitation(PROPOSAL-self-entry-not-recognized, human-diff-covered)", "designGate": "N/A(TrackB)", "perFile": "6/6 ok(1 expected-pending-T3)", "verificationSet": "V1-V5 all green", "destructiveSelfCheck": "3/3 hold", "observations": ["INDEX.md pinned frozen outside manifest (protocol step-7 action, disclosed)", "changes-index-state 260904 entry = prior-batch leftover"], "blockers": []}
```

放行建议：可进 T3（ARQ 移已结 + 索引钉行）→ retro（含 §6 留根的迁移意图表述分歧澄清义务）→ 前置升级闸 → 合并门。合并拍板仍归 human manual-merge 门。
