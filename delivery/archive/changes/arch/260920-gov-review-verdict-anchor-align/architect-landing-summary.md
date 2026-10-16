# architect-landing-summary · 260920-gov-review-verdict-anchor-align

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 **R1 GREEN**（0 BLOCK / 3 SHOULD / 4 NIT；判官明示「F1–F3 为 FROZEN 前应折入项，折入后无需再轮」）· change-id 260920-gov-review-verdict-anchor-align

## 1. 落点对照（manifest 5/5）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/references/track-b-judge-brief.md` | T1① | `:21` verdict 格式段**追加行形态义务**——「append 到 PROPOSAL §7 的 verdict 行必须写为**行首** `verdict: <TOKEN>`（可带 `>` 引用前缀；TOKEN ∈ `GREEN` / `BLOCK(F1..Fn)`）——该行是前置升级闸 `readGovAnchors` 的机器锚消费面」。 |
| `.claude/references/track-b-verifier-brief.md` | T1② | `## verdict 格式` 段**追加行形态义务**——「`landing-verdict.md` 必须含一行**行首** `verdict: <TOKEN>`（TOKEN ∈ `PASS` / `FAIL(F1..Fn)`）——该行是 `deriveLanded` 的机器锚消费面」。 |
| `.claude/scripts/lib/governance-gate-shared.mjs` | T2① | **新增 export** `VERDICT_STATUS_RE` / `VERDICT_HEADING_RE`（自 `changes-index.mjs:177-178` 迁入，**RE 增 token 前后反引号容忍** `(?:[\x60*]{0,4})`——R1-F3）。 |
| `.claude/scripts/changes-index.mjs` | T2②+T3+T4 | ① 删本地 RE 定义、改 lib 具名 import；② `:371` 守卫 `if (merged)` → **`=== 'yes'`**（T3，另加注释）；③ `backfill()` 的 `cache[name]` 写入改 **`merged === '' ? 保旧值 : merged`**（T3/ R1-F7）；④ `LANDED_ADJUDICATION` **+7 条 pin**（T4，见下）。 |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | T2③ | ① 增 lib 具名 import `VERDICT_STATUS_RE`；② 新增纯函数 **`isGreenVerdict(text)`**（逐行 RE、**取最后命中 token**——R1-F1）；③ `readGovAnchors` 的 green 改用该函数（**裸 GREEN 分支删除**——原病灶）；④ `--self-check` **+3 夹具**（样板行不命中 / `verdict: GREEN` 命中 / `BLOCK(F1)` 不命中）。 |

```landing-manifest
.claude/references/track-b-judge-brief.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/changes-index.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
```

## 2. 验证集执行记录（V1–V7）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `node .claude/scripts/changes-index.mjs --self-check` | **`changes-index` 29/29 pass**（`ok:true`、`fail:0`）——VERDICT_* 迁 lib 后**零回退** ✓ |
| V2 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | **33/33 pass**（原 30 → +3 新夹具）；三夹具在源码中具名（`green-anchor-boilerplate-not-green` / `-verdict-line-green` / `-block-not-green`）。**注**：该闸的 self-check 输出为**单 JSON 汇总**（不逐 case 打印），故「具名」以**源码在场 + 计数 +3** 双证，非输出逐行 ✓ |
| V3 | `node .claude/scripts/pre-merge-escalation-gate.mjs 260918-gov-review-merge-chain-friction`（**已合并批**） | **PASS**（`无例外命中`、exit 0）——**过渡分析（无在途受影响批）的实证**：该批已合并，锚收窄未使其结论翻转 ✓ |
| V4 | 重建 INDEX 后逐批核 | **7 批**由 `frozen` 翻 `landed`（6 条原 pin + **1 条落地期补登**，见 §3-4）；重建后 **`frozen \| yes` = 0**（仅剩本批自身 `frozen \| no`，legit）✓ |
| V5 | `git diff --stat` 逐文件归类 | 5 路径 = manifest 5/5；T3 的守卫改动为一行 + backfill 一行（均注释伴随）；无越界 ✓ |
| V6 | `route-regression-check.mjs` + `lint-descriptions.mjs` | `28 ok, 0 fail`；lint exit 0 ✓ |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §5（由 landing-verifier 亲跑） |

**真空绿规约核对**：V2 的「样板行不命中」夹具**先断样板行确实含 GREEN 字样**（`/GREEN/.test(boilerplate)` 已写入 `got`）——防夹具没造对；V4 逐批实测（非只写 id）。

## 3. 落地中的事实性偏差（如实申报）

1. **T3 的 `=== 'yes'` 之外，`backfill` 也有同病**（起草期漏、R1-F7 抓出、本批一并修）：`backfill()` 的 `cache[name] = { …mergedInMain: merged }` 是**无条件写**——git 不可用（`''`）时会抹掉已钉的 `'yes'`。已改为 `merged === ''` 时**保旧值**。
2. **T2 的 RE 增了反引号容忍**（R1-F3）：原 `(?:\*{0,2})` 只容忍加粗；批① §7 的 `**Verdict：` + 反引号 GREEN + 反引号 + `**` 形态为**真实判官输出**而不被认 ⇒ 改为 `(?:[\x60*]{0,4})`。
3. **T2 的 green 取「最后命中 token」而非「任一行命中」**（R1-F1）：与 `deriveLanded` 的 `:189` 覆写语义一致，防「GREEN 后重开」场景 fail-open。
4. **T4 的 census 由 6 条更正为 7 条**（**落地期现跑发现，起草期 census 遗漏**）：第 7 条 = `260914-arch-review-kidpal-session-ctx-contract`。**成因**：其「已合main」在 T4 起草 census 时**尚为空**（初判非候选），后由 backfill 重算为 `yes` ⇒ 原 6 条清单漏它。其终态 verdict = `**LANDING VERDICT: PASS**`（复核轮，`landing-verdict.md:92`）——**label 为 `LANDING VERDICT` 非 `verdict`，行首判据不认**，故同样归入 pin。**已补登并逐条注明依据**；**该遗漏如实登记**（census 的时点依赖问题：`mergedInMain` 本身是会被 backfill 改写的字段）。
5. **T2 的 token 集收 {GREEN}**（R1-F2）：删 PASS——`260918-gov-review-merge-chain-friction` R1 `:176` 已论证否决该 token（spoof 面、判官语法无 PASS）。**PROPOSAL §2 T2 的 §5 V2 预期**写的是「`verdict：GREEN` 行 → 命中」，落地实现为 `verdict: GREEN`（半角冒号亦可，RE 的 `[:：=]` 两种都认）——**无偏差**，仅记明。

6. **结构门 citation advisory 4 条（如实披露）**：门报 `260907-architect-dispatch-model-override/landing-verdict.md:3`、`260918-gov-review-merge-chain-friction/PROPOSAL.md:211` 与 `:176`、`260920-gov-review-reader-scripts-align/PROPOSAL.md:151`——**全为同族的短形态引据**（省略了仓内已知前缀 `delivery/changes/arch/`；本批 §2 T4 证据段与 T1 正例句均直接引了兄弟批的文件）。门照设计「只观察不产 fail」（`ok:true` 不受影响）。**跨批累计已达 5 例**（本批 4 + 批① 的 `review.md` + P1 的 5 + P4 的 1，去重后族内共 4 批出现），**建议随「citation 候选域」下一批一并评估**（该族缺的是「相对某个已知根的短形态」语义，非前缀清单长度）。

## 4. 残余风险 / 留根指针

- **留根②（`ARQ-260920-changes-index-mergedinmain-stale` 的「重建不重算」侧）**：T3 的守卫收窄使「未合并不再钉缓存」，但**「合并后不跑 upsert/backfill 则仍显示旧值」的时序问题仍在**——7 条 pin 缓解了存量显示，新批仍依赖合并流程记得跑刷新。**随该 ARQ 收口一并评估是否需「合并流程强制 backfill」**。
- **留根④（`260830` 的 FROZEN_RE 版本号瑕疵）**：其 Status 行 `**状态：FROZEN · 2026-08-31` 无 `vN.M`，`readGovAnchors` 的 frozen 锚不命中。**不修**（该批已合并、闸不再跑它）；如实登记。
- **留根⑤（判官/核验官对新形态的遵从率）**：T1 的简报义务落地后，判官是否**每次**都写规范行需 `/loop-retro` 观察。若遵从率低，T2 的锚会对它们判红——**那是有意的**（逼规范）。
- **留根⑥（census 的时点依赖）**：本次 T4 的「6 → 7」暴露了一个可迁移教训——**`mergedInMain` 本身是会被 backfill 改写的字段，以它为筛选条件的 census 有时点依赖**。后续做同类清点时应**在 pin 前重跑 census**（本批即在落地期重跑才发现第 7 条）。

**交日落复盘项**：T2 落地后 `/loop-retro` 观察——下一批判官的 verdict 行是否规范（遵从率）、升级闸是否有新假阴性。

**自律件**：起草期四处 census（样板行 16/26、VERDICT_STATUS_RE 剖面 17/24+7、pin 6 条 verdict verbatim）均现跑；**落地期重跑 census 发现遗漏第 7 条并补登**。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V7 + 逐文件人读 diff 对照 FROZEN §2）。

## 6. 核验后注记（append-only，2026-09-20）

landing-verifier 判 **`verdict: PASS`**（1 项非阻断 ADVISORY）——**其 verdict 件是本批 T1 新义务的首个适用者**，实测行首 `verdict: PASS` 命中 `VERDICT_STATUS_RE`（token=PASS）⇒ **设计端到端跑通**（义务 → 锚 → 消费面）。四处观察及处置：

- **A1（回归夹具缺口，本批不动、登记留根）**：FROZEN §2 T1① 末句要求反引号方向「**配 V2 夹具**」，而 T2 明列的自检是**三夹具**、未含反引号包 token 形态；落地集**对齐 T2**（RE 容忍已落地且实测有效：批① 的 `**Verdict：` + 反引号 GREEN + 反引号 + `**` → 匹配 ✓）。**判定 = 回归夹具缺口、非行为缺陷**（FROZEN 内部有张力：T1 要夹具 vs T2 明列三条）。**处置：本批不动**（已通过核验，事后改动会使 verdict 失效）；**留根——下一批补夹具 `green-anchor-backtick-wrapped-green`**。**上文 §3 未披露此缺口，如实补记于本行。**
- **A2（文档 nit，已修正）**：上文 §3 编号重复（两条「5」、缺「6」）——已把 citation advisory 那条改为「6.」。
- **A3（非 manifest 工作树改动，benign）**：`.codebase-memory/*`（机器再生面、不追交）+ `INDEX.md`（T4 生成物）——结构门 `changedCanonicalNotInProposal:[]` 佐证非 scope creep。
- **A4（状态缓存内部字段，benign）**：7 pin 批的缓存项 `landed:false`（display 不受影响——pin 短路使 INDEX 正确显示 landed）。