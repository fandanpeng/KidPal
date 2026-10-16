# landing-verdict · 260924-gov-review-gate-parse-teeth（复验 · F1 修复后）

verdict: PASS + 2 ADVISORY

核验官：landing-verifier（Track B，同 agent resume 复验；改前基线 = `/tmp/kidpal-head` = `git archive HEAD` 独立树，HEAD 仍 `2fce4b6`）。比对基准 = FROZEN v0.4 §2 改法集 + §5 验证集 + landing-manifest（31 行）；只写本文件。

**本轮结论**：首轮 F1（T6 的 5 件接线破坏「current/ 缺席 → 无上下文绿跳」短路判据，违 FROZEN「判定语义零变更」不变量）**已修复并经独立复验确认**；ADVISORY-2 一并处置到位；结构门 / preflight / 自检全家 / §5 关键项复跑全绿。**F1 闭，放行建议给（merge 仍归 human manual-merge 门）。余 2 项 ADVISORY 为接受项，非阻断。**

---

## 1. F1 复验（RESOLVED）

**修复形态核对**（`git diff HEAD -- .claude/scripts/{acceptance-coverage-check,closeout-check,docs-sync-check,security-check,mutation-check}.mjs`）：同根 5 件**一律**由无条件 `.path` 恢复为

```js
const resolved = resolveCurrentTaskPackage('<名>', projectDir);
return existsSync(resolved.path) ? resolved.path : null;
```

即恢复原 `existsSync(current) ? current : null` 的「无上下文 → null → caller 绿跳」语义；显式分支 `resolveExplicitTaskPackage(...).path` 维持原样（原实现即无条件 resolve，未变）。**修复 == 申报，无夹带**。

**基线复跑（无 current/ 夹具 `CLAUDE_PROJECT_DIR=/tmp/kidpal-empty`，逐件改前[HEAD 独立树] ↔ 改后对照 exit）**：

| 脚本 | 改后(NEW) | 改前(HEAD) | 判定 |
|---|---|---|---|
| acceptance-coverage-check | 0 | 0 | **SAME** |
| closeout-check | 0 | 0 | **SAME** |
| docs-sync-check | 0 | 0 | **SAME** |
| security-check | 0 | 0 | **SAME** |
| mutation-check | 0 | 0 | **SAME** |
| taskpackage-required-check（对照） | 1 | 1 | SAME |
| devloop-artifact-check（对照） | 0 | 0 | SAME |
| journal-append（对照） | 2 | 2 | SAME |
| loop-metrics（对照） | 0 | 0 | SAME |
| pod-events-aggregate（对照） | 0 | 0 | SAME |
| pod-events-append（对照） | 0 | 0 | SAME |

11 件全 **SAME**，与修复申报逐件吻合（5 修复件 0/0 + 对照 6 件对上）。空夹具下 acceptance-coverage-check 实测输出恢复为 `{"ok":true,"skipped":true,"reason":"no TaskPackage context"}` **exit 0**（首轮此处为 `ok:false … exit 1`）⇒ 短路语义确已恢复。

**有 current/ 回归（`CLAUDE_PROJECT_DIR=/c/AI/KidPal-gov`，current/ 指 perf-k1）**：5 件均 `{ok:true,…}` **exit 0** + 回显行照常 `[<名>] TaskPackage 上下文解析：changeId=260924-perf-k1-roundtrip-latency（来源：delivery/taskpackages/current/taskpackage.json）`；与 HEAD 版同判（差异仅新增回显 stderr 行）。**判定语义零变更恢复成立**。

---

## 2. ADVISORY-2 处置（RESOLVED）

`mutation-check --self-check` 由 12/12 → **14/14**，补入两 case（`mutation-check.mjs:216-230`）：
- `sc-unknown-flag-exit-2`：`spawnSync` 跑 `--bogus-selfcheck-probe` 断言 exit 2（入口面，反转实现必红）。
- `sc-positional-arg-compat`：mkdtemp 夹具目录（**零仓内写入**，`finally rmSync`）放 `taskpackage.json`，spawn 位置参数路径 + `CLAUDE_PROJECT_DIR=夹具`，断言 `exit 0 && stdout+stderr 含 "skipped": true && 含 位置参数`（判别性成立）。

复跑确认 `{"selfCheck":"mutation-check","pass":14,"fail":0,"ok":true}`。

---

## 3. 门与回归复跑（全绿）

- **结构门**：`landing-fidelity-check.mjs <dir>` → **ok:true / exit 0**；frozen:true、packetBinding:true、unauthorized/claimedNotLanded/plannedNotLanded 全 []、proposalTargets 31 ≡ summaryTargets 31。
- **preflight**：`--preflight <本 PROPOSAL>` → `ok:true / anchorCount 31 / advisories:[] / findings:[]`。
- **自检全家**：commit-safety 15/15 · verify.mjs 8/8 · lib current-pointer-shared 7/7 · taskpackage-required 5/5 · brief-inputs 29/29（`SELF-CHECK GREEN`）· changes-index 31 例 fail 0 · mutation-check 14/14 · landing-fidelity 套件全绿（preflight 19/19、verbatim-quotes 6/6、isFrozen 19/19）。
- **V2 live**：`verify.mjs --phase=full` → **exit 2** + 「未知 --phase 值」。
- **接线类**：`verify.mjs` / `taskpackage-required-check` 自检**末行均为宿主自检名**（未被 lib 顶替）；mutation-check 同理。isMain 守卫有效。
- **未动面核对**：本轮 git 变更面仅 5 件 F1 修复 + mutation-check 两 self-check case（+ 其 needed imports `mkdtempSync/tmpdir/rmSync/writeFileSync`）；T1–T5/T7–T10 文本与文件零改动；summary 变更 = §2 V6 补句 + §3 注10/注11。

---

## 4. summary 披露核对

- §3 注10 如实登记首轮 FAIL(F1) + 修复形态 + 基线对照 + **自我认领「首轮该记未记」**；与我方独立复跑逐项吻合。
- §3 注11 列其余 2 项 ADVISORY 为接受项（T2 拆函数 / expectCheck 两值不可产）——与我首轮判断一致，接受。
- §2 V6 补 F1 修复后补测句，与实测一致。

---

## 5. ADVISORY（余 2 项，接受项，非阻断）

- **ADVISORY-1**（接受）：T2 承重改法代码块非逐字（拆 `phaseArgError` + `assertPhaseArg`）——语义/message/判据/exit 全保真，拆分为可测性实现细节；结构门 `verbatimQuoteAdvisory.missing` 列出属预期。
- **ADVISORY-3**（接受）：T7 `expectCheck` 的 `mismatch`/`no-context` 两值因 `exit(2)` 先置而事件面不可产（只产 `absent`/`match`），与「exit 逐字不动」自洽，为 (b) 批预留值域。
- **（新，非阻断琐记）**：`acceptance-coverage-check` / `closeout-check` / `docs-sync-check` / `security-check` 四件 import 了 `echoResolution` 但**未使用**（`grep -c echoResolution` 各 = 1，仅 import 行）——回显实际由 lib 内 `resolveCurrent/Package` 承担。纯死导入、无行为影响、非私货；建议后续批次顺手清理，不阻断本轮。

---

## 6. 破坏性自检

- 逐件读了 5 修复件的实际 diff（非采信申报）；基线复跑以「改前原命令」为对照；空夹具 / 有 current 双态均实测。
- 未只核 exit 码：核了输出 payload（`skipped:true`/回显行/`位置参数`）。
- 独立复跑结构门、preflight、8 项自检、V2 live——全绿且与申报一致。
- 未发现新 F；无未授权改动；敏感面（G1/契约/agent defs/settings/scope-map/verify-gates）零触碰维持。

---

## 7. 机器锚行

verdict: PASS + 2 ADVISORY

---

## 8. summary V1 勘正单点复核（追加）

核验官（同官 resume）对 `architect-landing-summary.md` §2 V1 行的勘正做单点复核：

- **勘正接受**：原句「live 佐证 = 本批 landing commit 的 `git add` 触发相钩子输出新文案」不实——landing 期真实 `git add` 的快照**非空**（canary 删除先前已 staged，`git status` 现况 `D  .claude/scripts/enforce-scope-canary.mjs` 佐证），`statLine('add', <非空>)` 返回原 stat、回落行未触发。勘正后改为如实描述（空态分支实证由 self-check case 承担，真实空态需「add 前零 staged」场景）。属与 F1 同类的**披露纪律自我执行**，方向正确。
- **判别性不受影响**：`statLine` 三态逐字断言仍在 self-check——`statline-add-empty`（add 相空 → 新文案逐字）/ `statline-commit-empty`（commit 相空 → 原文案逐字）/ `statline-nonempty-passthrough`（非空原样）；复跑 `commit-safety --self-check` = `pass:15 fail:0 total:15 ok:true`，三 case 名在册，反转实现必红。
- **门回归**：结构门复跑仍 `ok:true / exit 0 / 31≡31`。
- **无新 F、无未授权改动**；本轮 summary 仅 V1 行措辞勘正，verdict 不受影响。

**复核结论：V1 勘正接受、verdict 维持 PASS + 2 ADVISORY。**
