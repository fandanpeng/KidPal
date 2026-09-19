# landing-verdict · 260918-gov-review-closeout-chain-friction

> **verdict：PASS + 3 ADVISORY**（无 FAIL 项；放行建议——merge 仍归 human / Track B 自动合并闸）
> 核验官：Track B landing-verifier（零上下文独立核验，2026-09-19，`gov/kidpal` worktree，HEAD = `69b9928`）
> 核对对象：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集；manifest 3 路径
> 一切判断重新推导，不采信 `architect-landing-summary.md` 自述。

---

## 1. 逐文件 diff 对照（manifest 3/3）

| manifest 路径 | structuralTouched | semanticMatch | expected（FROZEN §2） | actual（diff） |
|---|---|---|---|---|
| `.claude/scripts/acceptance-run-check.mjs` | ✓ | **ok** | T1①：`:34` `SKIP_DIRS` 补 `'archive'`；自检补实盘断言（沿 unit 闸 `real-repo-no-archive-collected` 先例） | `SKIP_DIRS` 末位补 `'archive'`；新增断言 `real-repo-no-archive-collected`（`[...fwAccFiles,...pyAccFiles].filter(/archive\//)` → `hits.length===0`）；`6+/1-` |
| `.claude/scripts/dispatch-stamp.mjs` | ✓ | **ok** | T2①：解析值**始终**回显 + 来源如实反映两路径；T2②：可选 `--expect-change-id`（不符 fail-loud；在场而无上下文 fail-loud；不传行为不变） | 抽 `resolveChangeIdWithSource()` 返回 `{changeId, source}`，旧 `resolveChangeId()` 保留为薄封装；调用点无条件 stderr 回显；`--expect-change-id` 两 fail-loud 分支 exit 2；`33+/6-` |
| `docs/DEV-GOVERNANCE.md` | ✓ | **ok** | T1② 新增 §5「退役保留目录名」；T3 新增 §6「tighten-only 修订窗口」（在 §5 之后） | 尾部新增 §5（语义 / 三处承载点 / 为何成文 + 两实证）、§6（防滥用五件 + 核销落点 + 无先例如实声明 + 超窗即改验收 + mutation-check 职责分工）；`20+/0-`（纯 additive） |

**逐项结论**：

- **`acceptance-run-check.mjs` — ok**。① `SKIP_DIRS` 补 `'archive'` 属实（`git diff` 逐字见 `-…'ESP32-S3-RLCD-4.2-Demo']);` → `+…'ESP32-S3-RLCD-4.2-Demo', 'archive']);`）。② 新断言与 unit 闸 `unit-run-check.mjs:188-191` 的**同名先例同形**（同 `filter((f)=>/…/)` 形态、同断言名 `real-repo-no-archive-collected`、同 detail 约定 `…join(',')||'no-archive-in-enumeration'`）。③ **真能证伪（非恒真）**：断言取自 `collectReal(projectDir)` 实盘枚举，非静态常量；仓内 `firmware/main/test/acceptance/archive/` 实有 13 件归档 `.acceptance.c`，若 `SKIP_DIRS` 回退（不含 `archive`），这些件必入 `fwAccFiles`、filter 命中即红——输入非空、可判。断言包在与 `real-repo-smoke` 同一 `try` 内，枚举抛错时仅 `real-repo-smoke` 记红（与 unit 闸同构）。
- **`dispatch-stamp.mjs` — ok**。① **回声始终执行**：回显行置于 `resolveChangeIdWithSource()` 之后、`--expect-change-id` 分支之前，无分支遮挡；仅当参数校验（`required()` / `isNaN`）先退出时不回显（那是解析前，符合“解析值回显”语义）。② **来源如实两路径**（我亲跑两条路径，见 §3 V3）：读指针路径 = `(from delivery/taskpackages/current/taskpackage.json)`、显式 flag 路径 = `(from --task-package-id)`——**两来源确实区分、未写死**（R1-F6 满足）。③ **四态全测**（见 §3）。④ **旧签名 `resolveChangeId()`** 保留为薄封装（`return resolveChangeIdWithSource().changeId;`）✓。
- **`docs/DEV-GOVERNANCE.md` — ok**。**节次顺序正确**：§5 在前、§6 在后（diff 逐字序）；§5 内容 = 「退役保留目录名」（非 tighten-only）、§6 内容 = 「tighten-only 修订窗口」（非退役名）——**未颠倒**。**T1② 三要素齐**：语义（`archive` = 两面退役保留名 + 「移位即退役」对偶语义）、三处承载点（两 `SKIP_DIRS` + `pytest.ini` `norecursedirs`）、为何成文（约定原只活代码注释、包作者不可见、误落静默不跑）。**T3 五件齐 + 核销落点 + 无先例声明 + 超窗即改验收**（逐条见下 §6 摘录）。**纯 additive 无删除行**（`20+/0-`），无越出 packet 的私货；两节均**收窄/向严**，未夹带任何权限或写域扩张。

## 2. packet-binding 一致

- **summary packet-id 行**（`:3`）＝「FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集」↔ **FROZEN §2 头部 packet-binding 行**（PROPOSAL `:7`）＝「packet-id = 本 PROPOSAL.md §2 改法集（T1–T3）+ §5 验证集」——**逐字一致** ✓。
- **manifest ↔ §2 目标文件锚并集**：PROPOSAL flblock（`:13-17`）= summary flblock（`:13-17`）= `{acceptance-run-check.mjs, dispatch-stamp.mjs, docs/DEV-GOVERNANCE.md}`（3 路径）；§2 锚并集（T1① `acceptance-run-check.mjs` + T1②/T3 `docs/DEV-GOVERNANCE.md` + T2 `dispatch-stamp.mjs`）= 同 3 路径。结构门机判 `proposalTargets == summaryTargets`（3/3）✓。
- **scope creep**：无。工作树 canonical 改动恰 3 文件，`unauthorized:[]` / `claimedNotLanded:[]` / `plannedNotLanded:[]`。同族 P4/P2/P1（`95bc44b`/`ae85306`/`69b9928`）为 HEAD 及其前置 commit，非本批工作树改动、不计本批 scope。

## 3. 验证集复跑（V1–V7，亲跑）

| 项 | 我亲跑的命令 | 实测输出 | 与 summary 比对 |
|---|---|---|---|
| V1 | ① `node .claude/scripts/acceptance-run-check.mjs`；② 独立复现闸同款 `collectFiles`（standalone `node -e`，不落文件）双口径枚举 `firmware/` | ① `firmwareAcceptanceFiles: 9`；② **补 `archive` 前 = 22**（现役 9 + 归档 13）、**补后 = 9**（现役 9 + 归档 0）；现役 9 件含 `char_window_overlay` / `retract_p4_three_page` | ✓ 与 summary §3-1 的 **22→9、归档 13 + 现役 9** 完全一致 |
| V2 | `node .claude/scripts/acceptance-run-check.mjs --self-check` | `ok:true`，10 条全绿；`real-repo-no-archive-collected` **具名在场且 ok**（detail=`no-archive-in-enumeration`）；`real-repo-smoke` detail=`fwAcc=9 pyAcc=0` | ✓ 与 summary V2 一致；`fwAcc=9` 与 V1 自洽 |
| V3① | `grep -c "resolved changeId" .claude/scripts/dispatch-stamp.mjs` | `1` | ✓ =1 |
| V3② | 读指针路径真跑（不匹配 expect 触发早退，无写） | `resolved changeId = 260917-m2-k3-char-window-overlay (from delivery/taskpackages/current/taskpackage.json)` + 不符 exit 2 | ✓ 来源标注 = 读指针路径 |
| V3③ | 显式 `--task-package-id 260918-...` 真跑（不匹配 expect） | `resolved changeId = 260918-gov-review-closeout-chain-friction (from --task-package-id)` + 不符 exit 2 | ✓ 来源标注 = flag 路径（两来源如实区分） |
| V3④ | 无上下文（`CLAUDE_PROJECT_DIR=<不存在>`）：带 flag / 不带 flag 各一次 | 带 `--expect-change-id anything` → **exit 2**（“在场但无 TaskPackage 上下文——fail-loud”）；**不带 → exit 0**（“no TaskPackage context; skipping”） | ✓ 覆盖 F6 新定义态**两边** |
| V3⑤（附） | 相符放行态（隔离 temp projectDir + 匹配 expect + pre-cutoff id） | exit 0，越过硬校验进入正常流程（无“不符”/“无上下文”消息） | ✓ 相符 → 放行 |
| V4 | `grep -c` | `archive`=5、`tighten`=2、`^### 5. 退役保留目录名`=1、`^### 6. tighten-only`=1 | ✓ 与 summary V4 一致 |
| V5 | `route-regression-check.mjs` + `lint-descriptions.mjs` | route `28 ok, 0 fail`；lint exit 0，仅既有 `landing-verifier.md` 超长告警（363>320） | ✓ 与预期一致 |
| V6 | `git diff .claude/scripts/changes-index.mjs` | `diff-lines=0`（零改动） | ✓ 证明 T1〔索引锚〕未夹带 |
| V7 | `node .claude/scripts/landing-fidelity-check.mjs <本批>` | `ok:true`，exit 0 | ✓ 见 §4 |

**V1 件数申报独立复测**：FROZEN §5 V1 预测「20→7（归档 13 + 现役 7）」为**起草期（2026-09-18）时点值**。我独立复测现工作树 = **22→9（归档 13 + 现役 9）**；新增 2 件现役件 = `char_window_overlay.acceptance.c`（`759709e` 落账）/ `retract_p4_three_page.acceptance.c`（`7f2e0f3`），二者经 **main→gov 回同步 merge `2492f44`** 带入。**归档面 13 件未变**。**判该申报属实**：属时点变化、非本批失误；改法（集合补一成员）与危害定性（枚举混 13 件退役件）均不受影响。

## 4. 结构门复跑

`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260918-gov-review-closeout-chain-friction` → **`ok:true`、EXIT=0**。
`frozen:true`、`packetBinding:true`、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`、`gitDegraded:false`、`proposalTargets==summaryTargets`（3 路径）。
`citationAnchorAdvisory.problems` = **3 条**（全部 `hooks/verify.mjs:123 -> file-not-exist`）——与 summary §3-5 披露数一致；门「只观察不产 fail」，`ok:true` 不受影响。
`advisory.changedCanonicalNotInProposal = [".claude/scripts/pre-merge-escalation-gate.mjs"]`——**baseRef 假阳性**（见 ADVISORY-2）。

## 5. 真空绿规约

- **重放类先断言输入非空**：本批无重放脚本；V1 双探针我显式取「补前 22 > 0」为基线，非空已证。
- **V2 断言具名且 ok**：`real-repo-no-archive-collected` 在输出中**具名出现且 `ok:true`**（非“加了断言没接线”）✓。
- **V3④「不传 flag → exit 0」真跑**：亲跑 exit 0，防“改坏既有行为被漏”✓。
- **接线类新增自检**：新断言位于 `runSelfCheck()` 电池内、`results` 汇总驱动 `allOk`；未挡在终端 exit 型自检之前，全电池可跑完 ✓。

## 6. 诚实披露核对（summary §3 五条）

| # | summary 声称 | 独立核对 | 判定 |
|---|---|---|---|
| 1 | V1 件数过时：预测 20→7，实际 22→9（+2 现役来自回同步 merge，归档 13 不变） | 独立复测 = 22→9、归档 13 + 现役 9；2 新件 = `char_window_overlay`/`retract_p4_three_page`，经 `2492f44` 带入 | **属实** |
| 2 | `--expect-change-id` 退出码取 2（与既有 arg-error 约定一致） | 脚本既有 arg 校验（`required()` `:41`、`isNaN` 旧 `:162`/新 `:169`）均 `exit 2`；新两分支亦 `exit 2` | **属实**（属具体化，非偏离） |
| 3 | `--preflight` 属 P4、本批未触 `landing-fidelity-check.mjs` | `git diff --stat HEAD -- .claude/scripts/landing-fidelity-check.mjs` = 空（未触） | **属实** |
| 4 | §6 内保留「无 `sha256` 核销先例」如实声明 | §6 第 2 条含「本仓现无已执行的 `sha256` 核销先例——`7ab2d47` 仅留行数计数、未记 hash；本节落地后为首例」 | **属实** |
| 5 | 结构门 citation advisory 3 条（`hooks/verify.mjs:123` ×3，冻结稿、落地者不可改） | 门报恰 3 条 `hooks/verify.mjs:123`；`hooks/verify.mjs:123` 短形态出现于 FROZEN PROPOSAL（冻结稿）；真身 `.claude/hooks/verify.mjs` 存在 | **属实**（节位标注微瑕见 ADVISORY-1） |

**破坏性自检**：summary 声称的每处改动均能在真实 diff 找到对应（§1 三行表逐条落到 diff）；未挑好看文件核——**三个 manifest 文件全读全 diff**、G1（治理正文）与 G3（两脚本）两敏感面逐条核（§1），无 agent def / self-def 改动（工作树无 `.claude/agents/**` 改动）。

**§6 五件逐条落地核对**（对 FROZEN §2 T3）：① `git diff -U0` 删除行数=0 ✓；② 重跑原变异证转红 + 不许另造易过变异 + `sha256` 记入 `REVISION LOG` 同一条 `<前→后>` + 无先例声明 ✓；③ 三件记录（Reason + `mutationEvidence` `sensitive:false→true` + 红基线摘要补一段）✓；④ 范围帽（同 Pod / 同既有冻结件 / 不新建文件）✓；⑤ 意图锚定（钉既有 `acceptanceRef`/`decisionCoverage`，需新 id 或改行为即 re-spec 升 human）✓；+ 超窗即改验收 ✓。

---

## ADVISORY（非阻断观察）

- **ADVISORY-1（summary 自述节位微瑕）**：summary §3-5 称 3 条短形态引据在「PROPOSAL **§6 留根③**」，但亲核该 3 条 `hooks/verify.mjs:123`（无 `.claude/` 前缀）实际落在 **§7 判官记录**（R2 行 `:257`/`:295`）；§6 留根③（`:210`）用的是可解析全形 `.claude/hooks/verify.mjs:123`。**实质结论不变**（引据同在冻结 PROPOSAL、落地者不可改、门不产 fail）。不影响 landing 保真判定。
- **ADVISORY-2（结构门 baseRef 假阳性）**：门报 `changedCanonicalNotInProposal: .claude/scripts/pre-merge-escalation-gate.mjs`，实为该文件属 **HEAD~1（`ae85306`）→ HEAD（`69b9928`，merge-chain 批）** 的改动；本批工作树**未触**该文件（`git status` 无该文件）。门的 `baseRef=HEAD~1` 默认把上一 commit 的改动计入，属已知假阳性，非本批私货。
- **ADVISORY-3（回声 null 态扩展，非冲突）**：FROZEN §2 T2① 钉的回声形态为严格二选一 `(from --task-package-id)` / `(from delivery/taskpackages/current/taskpackage.json)`；实现于**解析值 = null 的无上下文态**追加消歧后缀（`…taskpackage.json（不存在）` / `（不可解析）`）。两**非空**态逐字用被批二形（亲跑已证）；null 态后缀是超 FROZEN 字面、但**仍如实反映来源（current/ 路径）+ 附因由**，属增强不属偏离，未扩张任何写域/权限。

---

## 结论

**PASS + 3 ADVISORY**。manifest 3 路径的语义保真（改法落到、无漏改、无私货、与 FROZEN §2 无矛盾）、packet-binding 一致、验证集 V1–V7 全复跑吻合、结构门 `ok:true`、真空绿规约满足、summary §3 五条披露属实——**放行建议**（merge 归 human manual-merge 门 / Track B 自动合并闸；remote push 仍 human）。三 ADVISORY 均非阻断，无需退回主线程返工。