# landing-verdict · 260906-gov-review-index-status-truth-source

> 核验人：landing-verifier（Track B 步骤 5，独立判定器）· 日期：2026-09-06
> 凭据：landing-fidelity 结构门输出 + 逐文件人读 git diff + §5 验证集独立重放 + 私货检查 + 破坏性自检（scratch 均在仓外 `c:\tmp\`，仓内零写入）
> 本 verdict 是唯一写入件；manifest 三件 / PROPOSAL / summary / hooks / 契约面全程只读。

## 1. 结构门（landing-fidelity-check.mjs）

`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260906-gov-review-index-status-truth-source` → **`ok: true`**：

- `frozen: true` / `packetBinding: true` / `gitDegraded: false`
- `proposalTargets` = `summaryTargets` = manifest 三件（lib / changes-index / closeout-check）
- `unauthorized: []` / `claimedNotLanded: []` / `plannedNotLanded: []`

三类 advisory（citationAnchorAdvisory ×3 file-not-exist、verbatimQuoteAdvisory ×2 not-found、changedCanonicalNotInProposal ×3）逐条复核见 §5——均判为闸的已知环境性/解析性缺口或本批外来在册态，**非真实失配**。

## 2. 逐文件语义核对（人读 diff ≡ FROZEN §2）

| 文件 | structuralTouched | 核对项 | 结论 |
|---|---|---|---|
| `.claude/scripts/lib/governance-gate-shared.mjs` | 是（+23 行：1 import + 注释块 + 3 导出） | T1 | **ok** |
| `.claude/scripts/changes-index.mjs` | 是（import + deriveDevClosed + collectDev 换谓词 + HEADING_RE 容差 + JSON 锚 + 6 夹具） | T2 / T4 | **ok** |
| `.claude/scripts/closeout-check.mjs` | 是（import 加三具名 + 头注更新 + 删本地常量 + 两处谓词调用替换） | T3 | **ok** |

**T1（lib）——ok**：diff 恰 = 1 具名 import（`import { CHANGE_ID_PREFIX_RE } from './changeid-shared.mjs';`，:22）+ 注释块（:36-39）+ `export const CHANGE_ID_CUTOFF = '2026-07-23'`（:40）+ `isPostCutoffChangeId`（:42-45）+ `closeoutRequiredFiles`（:47-50）。逐元素 byte-compare（CRLF 归一后）与 PROPOSAL §2 T1 围栏块逐字一致（comment/import/常量/两函数五元素全 true，见 §4）。`./changeid-shared.mjs` 实存于 lib/ 同目录（`lib/changeid-shared.mjs:7` 导出 `CHANGE_ID_PREFIX_RE = /^(\d{6})-[a-zA-Z0-9-]+$/`），import 路径合法无环。其余导出（`TARGET_PATTERNS` / `CANONICAL_TEST` / `isFrozen` 等）零触碰 ✓。

**T2（changes-index）——ok**：
- import 行加 `closeoutRequiredFiles`（:23），并入既有同 lib 导入行 ✓。
- `deriveDevClosed(dir, changeId)` 函数声明处导出（:119-121），实现 = `closeoutRequiredFiles(changeId).every((f) => existsSync(join(dir, f)))`，与 PROPOSAL 围栏块逐字一致（CRLF 归一后 includes = true）✓。
- collectDev 内 R5 注释块 + `hasVerdict` 判据删除，换 `const status = deriveDevClosed(dir, name.name) ? 'closed' : 'open';`（:144）——判据谓词与承重闸同 lib 单源 ✓。
- `VERDICT_HEADING_RE` 加 `(?:\d{1,3}[.、]\s*)?` 编号容差（:157），位置与形态 ≡ PROPOSAL T2② ✓。
- 行循环末加 JSON 行锚 `/"verdict"\s*:\s*"(PASS|GREEN|RED|FAIL|BLOCK)"/`（:181-182），置于 STATUS/HEADING/pendingHeading 三支之后 → last-match-wins 语义成立 ✓。

**T3（closeout-check）——ok，零行为变更逐项证**：
- import 行加 `{ CHANGE_ID_CUTOFF, isPostCutoffChangeId, closeoutRequiredFiles }` 三具名（:38）✓。
- 本地 `const CHANGE_ID_CUTOFF = '2026-07-23';` 删除（原 :89）✓。
- 头注 :85-88 更新为指向 lib 单源 + 登记余 3 处历史副本（注释行、零行为）✓。
- `:260` → `const isPostCutoff = isPostCutoffChangeId(changeId);`；`:263-265` 三元 → `const requiredFiles = closeoutRequiredFiles(changeId);` ✓。
- 等价性：`:242 const hasPrefix = CHANGE_ID_PREFIX_RE.test(String(changeId))` 与 `isPostCutoffChangeId` 首合取同一谓词；切分比较式逐字同形；两分支清单值逐字相同；`:250 firstCommit >= CHANGE_ID_CUTOFF` 与 `:251` 文案插值两行未动、改由具名导入解析（值同为 `'2026-07-23'`，无 ReferenceError）✓。

**T4（selfCheck 夹具）——ok**：+6 恰为 landed f5/f6/f7（:414-419）+ dev dv1/dv2/dv3（:425-435，mkDevFix 多文件具具），走与 collectDev 同一 `deriveDevClosed` 真实现（非假实现）；total 23→29 独立重放实测 29/29 ✓。文件尾 export 列表（:487）**不含** `deriveDevClosed`——summary 注记 1 所述「重复导出修正」属实（声明处单点导出，node --check 过）✓。

## 3. 验证集独立重放（不信 summary，亲跑）

| 项 | 命令 | 实测 | 结论 |
|---|---|---|---|
| V1 | `changes-index.mjs --self-check` | `changes-index pass:29 fail:0 total:29 ok:true`（前置 `changeid-prefix 4/4` 行 = 已登记 NIT-1 双 JSON 已知态） | PASS |
| V4 | `node --check` ×3 | 三件全过 | PASS |
| V6 | `changes-index.mjs` 全量重建 | `ok:true devCount:8 archCount:11`；INDEX :13-20 dev 8 行全 `closed`（含无 sv 的 cloud-link-bringup :15 / simulator-lvgl-align :16）；arch 三批翻 `landed` = `260906-gov-review-architect-loop-advisory` :26 / `260906-gov-review-arq-governance-batch` :27 / `260905-gov-review-verify-gates-align` :29；`260904-…` :30 与 `260830-…` :33 维持 `frozen`；本包自身 :28 `frozen`（verdict 未落、fail-closed 正确） | PASS |

## 4. 私货 / scope creep 检查

`git status --short` 全量：改动面 = manifest 三件 + `changes/INDEX.md` + `changes/.changes-index-state.json`（V6 重建预期产出）+ `.planning/kidpal-mvp/progress.md`（会话前 gitStatus 快照即在册的未提交态，非本批）；未跟踪 = 会话前在册的 skill 目录 + 本批证据目录。`git diff HEAD --stat` 对 **hooks / docs/contracts / docs/spec.md / firmware / server / .claude/agents / CLAUDE.md / AGENTS.md / .claude/rules** 全空——零触碰 ✓。`docs/ARCH-REVIEW-QUEUE.md` 的 `M` 态经 `git update-index --refresh` 消除 = 纯时间戳 racily-clean，**内容与 HEAD 逐字一致**（注记 6 表述与实况相符，且无内容级 scope creep）。

## 5. summary §3 诚实注记逐条复核

1. **重复导出修正**——属实。bottom export 列表（:487）无 `deriveDevClosed`，声明处单点导出，node --check 过，对外导出面一致。 ✓
2. **import 测试副作用**——不可独立于本批复跑，属披露性注记，不影响 landing 保真度判定；注记自身体例诚实。 （ no-op ）
3. **last-match-wins**——属实。JSON 锚在行循环最末支，f1「RED→GREEN 翻判取最终 GREEN」夹具 29/29 内通过。 ✓
4. **verbatimQuoteAdvisory = CRLF 环境缺口**——**属实，逐字证实**。`git ls-files --eol`：三件 manifest 全 `i/lf w/crlf`，PROPOSAL 经 Write 落盘为 LF → 闸的字节级 includes 必不命中。verifier 独立做 CRLF 归一化后 byte-compare：T2 `deriveDevClosed` 整块 includes = **true**；T1 整块因围栏块呈现序（注释先于 import）与文件实排（import 在 :22、注释在 :36）不同，拆五元素逐一 includes 全 **true**——即两条 advisory miss 纯系 EOL + 呈现序，实质内容与 FROZEN 逐字一致。 ✓
5. **citationAnchorAdvisory ×3 = 裸文件名解析缺口**——属实。三文件实存，且三处残余副本恰在引用行号（taskpackage-required-check.mjs:95 / devloop-artifact-check.mjs:70 / supervisor-gate-check.mjs:58，grep 实证），本批未动（§6 留根吻合）。 ✓
6. **changedCanonicalNotInProposal ×3**——属实。progress.md / skill 目录 = 会话前在册态；ARCH-REVIEW-QUEUE.md = 纯时间戳假阳性（§4 证）。 ✓

## 6. 破坏性自检记录（反向验证，scratch 全在 `c:\tmp\`，仓内零写入，已清理）

- **Mutation A（非真空绿证）**：copy 中把 dv1 夹具期望值 `true` 改 `false` → self-check `pass:28 fail:1 ok:false`，恰 dv1 一例红。self-check 非真空 ✓。
- **Mutation B（JSON 锚必要性，干净隔离）**：copy 中仅禁用 `if (j) {…}` 效果（保留 `const j`，排除 ReferenceError 污染）→ 纯 JSON 夹具 f6 翻 `false`，self-check 恰「JSON 行锚」一例红（28/29）；同探针 f5（编号标题+加粗散文）仍 `true`——证 f6 例对 JSON 锚承重、HEADING 容差独立可用 ✓。
  - 旁证（如实登记一次废操作）：首次 Mutation B 用 perl 只删 `const j` 未删 `if (j)`，致 try/catch 吞 ReferenceError、deriveLanded 恒 false——该次「3 例红」系突变过粗的全函数瘫痪，已弃用该次结论并重做干净版；全部操作限于 `c:\tmp\` copy，canonical 未动，事后 diff 对账 clean。
- **Mutation C（HEADING 容差必要性）**：两次尝试（perl 误伤双正则致 SyntaxError；node -e 转义未命中 no-op）均未干净落地，已放弃该路径并清理 scratch——以逻辑推演替代：旧 `VERDICT_HEADING_RE` 在 `#{1,4}\s*` 后即刻要求 verdict 词，`## 7. Verdict` 的 `7` 结构性不命中；且有 PROPOSAL §1 双证（主线程亲跑 + R1 判官 census：三批旧代码全 false）与本 verdict §3 V6 亲跑（新代码三批翻 landed）的 before/after 实测对——两锚合计为翻转的必要条件成立。
- **恢复确认**：`c:\tmp\verifier-destructive`、`c:\tmp\vd2` 已整体删除；仓内 `git status` 与核验前一致（仅 §4 列出的在册文件），无残留改动。

## 7. Verdict

**GREEN** — T1–T4 改法与 FROZEN PROPOSAL §2 逐字对应（CRLF 归一 byte-compare 全中），结构门 ok:true，验证集独立重放全过（29/29、三批翻 landed、260830/260904 维持 frozen），敏感面零触碰，私货零检出，summary 六条诚实注记全部属实，破坏性自检证 self-check 非真空且双锚各自承重；放行建议 merge（merge 仍归 human manual-merge 门）。

```json
{"changeId": "260906-gov-review-index-status-truth-source", "verdict": "GREEN", "checkedAt": "2026-09-06", "basis": "landing-fidelity 结构门 + 逐文件人读 diff + 验证集独立重放 + 私货检查"}
```
