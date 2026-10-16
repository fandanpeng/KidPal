# landing-verdict · 260918-gov-review-merge-chain-friction

**verdict：PASS + 2 ADVISORY**（无 BLOCK；merge 归 human manual-merge 门）

- 核验官：landing-verifier（零上下文独立核验，不采信 landing summary 自述）
- 核验时点：2026-09-19，`gov/kidpal` 工作树，landing 未 commit（改动在工作树）
- packet 依据：FROZEN PROPOSAL v1.0 §2 改法集（T1）+ §5 验证集；Track B（无 decision-packet，设计侧门 N/A）
- 本批形态：**仓库索引态变更**（11 件 `git rm --cached`）+ **一行纯注释**——两条线分别核

---

## 1. 逐文件 diff 对照（manifest 12/12）

### 1.1 `.claude/scripts/pre-merge-escalation-gate.mjs`（T1②，G3 面）

- `git diff`（V5）：**5 insertions(+)，0 deletions，1 file changed**。5 个 `+` 行**全部以 `//` 开头**，均为在 `:199`（现 `:204`）`if (!conflictedFiles.length) return 'union-safe';` **上方**追加的注释。
- **非注释改动行数 = 0**（§5 V5 字面判据）——`diff` 只对该文件，`-` 侧为空；独立旁证：`diff <(grep -v '^\s*//' HEAD版) <(grep -v '^\s*//' 现行版)` → **NON-COMMENT LINES IDENTICAL**。可执行语句零变更。
- **注释所述核查结论 vs 实际分支逻辑**（逐条核，`Read` :197–207）：注释称「`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落下面本行判 `union-safe`」——`git diff` 中被注释的**原文行逐字未动**（context 行 `if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）`）✓；注释称「再下一行 `VERSIONED_ARTIFACT_RE_LOCAL` 是另一条并列路径」——其确为 `:205` 的 `if (conflictedFiles.every((f) => VERSIONED_ARTIFACT_RE_LOCAL.test(f))) return 'union-safe';`，是**并列的第二条 union-safe 出口**、非 union 情形所依赖 ✓。**语义保真，无与批定矛盾**。
- `semanticMatch: ok`。**无非授权私货**：注释内容 = FROZEN §2②要求的四要素（union 自动并 / 不在 conflictedFiles / 落该分支判 union-safe / 无需显式豁免）+ 依据（该行分支逻辑 + ARQ-260917 r15 k3 实测），外加 §1 残余③ 已有的「`VERSIONED_ARTIFACT_RE_LOCAL` 为并列路径」一句——均属同一核查结论，非夹带。
- 见 ADVISORY-1（5 行 vs 正文措辞「一行注释」）。

### 1.2 11 × `delivery/taskpackages/*/overlap-probe.json`（T1①，撤跟踪）

- `git diff --cached --name-status` = **恰 11 条，全为 `D`**（出索引；无 M/A/其它）——落 `git rm --cached` 语义。
- **工作树文件仍在盘**：逐条 `ls`/`-f` 核，**11/11 ON-DISK**（无 MISSING）。
- 附录态佐证：`git check-ignore -v` → `.gitignore:46:overlap-probe.json`（条目已对撤跟踪后的路径生效）；`git ls-files --error-unmatch <任一件>` → **exit 1「did not match any file(s) known to git」**（确已 untracked）。
- **完整性 / 穷举**：`find delivery -name overlap-probe.json` = **恰 11 件，与 manifest 逐条 1:1**（无遗漏、无多撤）；commit 树 `git ls-tree HEAD | grep -c` = 11（撤跟踪基线）。
- `semanticMatch: ok`。

**未授权改动 = 0**：`git status` 除本批脚本（M）+ 11 件 probe（D staged）外，仅 `.codebase-memory/*`（机器再生面、常态脏、非本批）与未跟踪的 `architect-landing-summary.md`（本批制品）。

---

## 2. packet-binding 一致

- summary packet-id 行（`architect-landing-summary.md:3`）=「FROZEN PROPOSAL v1.0 §2 改法集（T1）+ §5 验证集 · R1 BLOCK(F1/F2) → human 裁定移出 T1 → **R2 GREEN**」↔ FROZEN §7 `packet-binding` 行（`PROPOSAL.md:7`）=「packet-id = 本 PROPOSAL.md §2 改法集（T1）+ §5 验证集」——**一致** ✓。
- manifest ↔ §2 目标文件锚清单并集：结构门机判 `proposalTargets`（12）== `summaryTargets`（12），**逐条精确闭合**（1 脚本 + 11 probe）✓。
- 本批移出 T1〔升级闸 GREEN 机锚〕后，落地范围 == 被批 packet（T1 仅②的注释 + ①的撤跟踪），**无残留「GREEN 正则收窄」类改动** ✓。

---

## 3. 验证集复跑（§5 逐条亲跑）

| 项 | 亲跑命令 / 方法 | 结果 | 与 summary 比对 |
|---|---|---|---|
| V1 | 撤前基线 = `git ls-tree -r HEAD \| grep -c overlap-probe.json`；撤后 = `git ls-files \| grep -c` | **撤前 = 11（>0 非空基线）**；撤后 = **0** | 一致 ✓ |
| V2 | `git diff --cached --name-status` + 逐条 `-f` 在盘核 + `check-ignore` | 11 条均 `D`；**在盘 = 11**；已被 `.gitignore:46` 忽略 | 一致 ✓（见 ADVISORY-3 nuance） |
| V3 | `git log --oneline -1 -- delivery/taskpackages/260829-hardware-validation/overlap-probe.json` | 命中 `bd81f57`（历史仍在） | 一致（summary 记 `bd81f57`）✓ |
| V4 | HEAD 版 self-check vs 现行 self-check（temp 镜像 `lib/` 后各跑） | **两侧逐格一致**：`changeid-prefix 4/4` + `pre-merge-escalation-gate 30/30`、exit 0 | 一致（summary 记 30/30、零差异）✓ |
| V5 | `git diff .claude/scripts/pre-merge-escalation-gate.mjs` 逐行归类 | **非注释改动行数 = 0**；5 行全 `//` 追加、0 删除 | 一致（summary 记「0 非注释行、5 追加、0 删除」）✓ |
| V6 | `node route-regression-check.mjs` + `node lint-descriptions.mjs` | route：`28 ok, 0 fail`（exit 0）；lint：1 warn = `landing-verifier.md` 超长（363>320）、exit 0 | route 一致 ✓；lint 见 ADVISORY-2 |
| V7 | `node landing-fidelity-check.mjs delivery/changes/arch/260918-gov-review-merge-chain-friction` | **exit 0，ok:true**（详见 §4） | 一致 ✓ |

---

## 4. 结构门复跑（V7）输出（亲跑退出码 = 0）

```
{ "ok": true, "gate": "landing-fidelity", "frozen": true, "packetBinding": true,
  "proposalTargets": [12 paths], "summaryTargets": [12 paths],
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [], "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 11, "problems": [5 条 file-not-exist] },
  "advisory": { "changedCanonicalNotInProposal": [pod-coordination.md, SKILL.md] } }
EXIT=0
```

- `citationAnchorAdvisory.problems` = **5 条**（summary §3-3 已披露）：`260914-.../PROPOSAL.md:504`、`260915-.../PROPOSAL.md:3`、`260830-.../PROPOSAL.md:215`、`260906-.../PROPOSAL.md:168`、`architect-review-r1.md:162`。逐条落点核：**全部位于 PROPOSAL §7（R1 判官 append-only 记录）/ §1 自证引文**，落地者不可改，门照设计「只观察不产 fail」——与 summary §3-3 逐条吻合 ✓。
- `advisory.changedCanonicalNotInProposal` = `pod-coordination.md` / `SKILL.md`：**非本批 scope creep**——核实二者由 **HEAD commit `ae85306`（dispatch-chain，前置批）** 所改（本批工作树未触）；门因 `baseRef=HEAD~1` 把前一 commit 纳入，属预期。`95bc44b`（batch-drafting）改的是 `landing-fidelity-check.mjs` 等，亦前置批。与简报「勿算本批 scope creep」一致 ✓。
- 设计侧门（`proposal-consolidation-check.mjs`）：**Track B N/A**（本批无 decision-packet，前提由 R2 GREEN + Status FROZEN 承载）——未跑，符简规。

---

## 5. 真空绿规约核对

- **重放类先断言输入非空**：V1 撤前基线**真跑至 11**（commit 树枚举）再解释「撤后 0」——非真空绿 ✓。
- **在场性双向断言**：V2 既证索引已删（`--error-unmatch` exit 1 + staged `D`）**且**证盘上仍在（11/11 在盘）——双向 ✓。
- **环境红先立基线**：V4 先跑 HEAD（改动前）版得 4/4+30/30，再跑现行版比对——零差异归因于「纯注释」，非假定 ✓。

---

## 6. 诚实披露核对

- **§3-1 V5 首版踩线自我纠正**：核实与现行 diff 一致——被注释的原文行**逐字未动（呈 context 行）**，仅上方追加 5 行注释，故「1 删 1 增」的踩线形态确已消除、V5 字面成立。如实 ✓。
- **§3-2 跨工作树过渡预期**：与 FROZEN §2 T1③ 逐要素一致（main 工作树 tracked-deleted 盘删 / Pod 过渡期一次性撞 / 文件可再生无数据丢失）。如实 ✓。
- **§3-3 五条 citation advisory**：与结构门输出**逐条吻合**，落点判定（§7/§1 append-only）正确。如实 ✓。
- **该记未记**：见 ADVISORY-2（summary §2 V6 行漏记 lint-descriptions 半支）。除此之外无隐项。

---

## 7. 破坏性自检

- 未挑好看文件核：**敏感面（G3 强制层脚本）逐字核**了整份 diff（非注释行 0 + 分支逻辑 verbatim），并**两侧 self-check 实跑**；11 件 probe 机械双向核。
- 无大 diff 被躲：本批 diff 总量 = 脚本 5 行 + 11 件索引删除，**全量覆盖、无抽查式跳跃**。
- 无越界：核验期**未写任何非 verdict 文件**；**未 `git add` / 未 `git checkout` 还原 11 件 probe**（其「出索引 + 盘上在」态原样保留）。

---

## ADVISORY（非阻断观察）

1. **注释行数 5 vs 正文措辞「一行注释」**：FROZEN §2② 散文写「补**一行**注释」，落地为 **5 行**连续 `//` 注释块。内容与 §2②四要素 + §1 残余③ 完全对应、无超纲；FROZEN 自设的 V5 判据是「全部改动落在注释行内」而非指定行数，故**不构成 mismatch**。仅登记为措辞级偏差。
2. **summary §2「V6」行漏记 lint-descriptions 半支**：FROZEN §5 V6 = `route-regression-check.mjs` + `lint-descriptions.mjs`；summary §2 该行只记 route（28 ok/0 fail），未记 lint。核验官亲跑 lint → 1 warn（`landing-verifier.md` 超长，exit 0），**恰为 FROZEN V6 允许的既有告警、本批未触该文件** → V6 实际通过，属 summary 少记一支（不影响结论）。
3. **V2 措辞 nuance**：FROZEN V2 预期「显示为 untracked」；因 `.gitignore:46` 忽略，porcelain 不显 `??` 而显 staged `D`。summary §2 已如实记「11 件均 `D`（出索引）」，未误报；仅提示「untracked」与 porcelain 显示的对应关系（确为 untracked：`--error-unmatch` exit 1）。

---

## 结论

**PASS**：manifest 12/12 逐文件语义保真，无漏改、无私货、无与批定矛盾；packet-binding 一致；§5 V1–V7 逐条亲跑与 summary 声称一致（含 V4 零差异、V5 非注释行 0、V1 非空基线）；结构门 GREEN（exit 0）。两条 ADVISORY 均为非阻断措辞/少记级观察，不改变保真度判定。**放行建议：可进 human manual-merge 门**（merge 归 human，不由核验官拍板）。