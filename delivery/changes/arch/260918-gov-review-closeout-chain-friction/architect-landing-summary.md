# architect-landing-summary · 260918-gov-review-closeout-chain-friction

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK(F1..F6) → R2 BLOCK(F7+N1–N3) → R3 GREEN →（四批族内）本批为**最后落地**的一批 · change-id 260918-gov-review-closeout-chain-friction

## 1. 落点对照（manifest 3/3）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/acceptance-run-check.mjs` | T1 ① | `:34` `SKIP_DIRS` 集合补 `'archive'`（镜像 unit 闸 260916 同款修法，使两闸退役口径同步）；自检电池补**实盘断言** `real-repo-no-archive-collected`（沿 unit 闸同名先例：任一面枚举命中 `*/archive/` 即红，防 `SKIP_DIRS` 回退静默复发）。 |
| `docs/DEV-GOVERNANCE.md` | T1 ② + T3 | 「## 验收测试纪律」节尾部**按钉死顺序**新增两节：**§5「退役保留目录名（`archive`）」**（语义 / 三处代码承载点 / 为何成文 + 两实证）与 **§6「tighten-only 修订窗口（冻结件的纯加严修订）」**（防滥用五件 + 超窗即改验收 + 与 `mutation-check` 的职责分工）。 |
| `.claude/scripts/dispatch-stamp.mjs` | T2 | ① 抽出 `resolveChangeIdWithSource()` 返回 `{changeId, source}`（旧签名 `resolveChangeId()` 保留为薄封装）；调用点**始终**回显 `dispatch-stamp: resolved changeId = <id> (from <source>)`，**来源如实反映两条路径**（`--task-package-id` / `current/taskpackage.json`）。② 新增可选 `--expect-change-id <id>`：不符 → exit 2；**在场而无上下文 → 亦 exit 2**（R1-F6 定义态）；**不传则行为与改前完全一致**（含 no-context → exit 0）。 |

```landing-manifest
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/dispatch-stamp.mjs
docs/DEV-GOVERNANCE.md
```

## 2. 验证集执行记录（V1–V7）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | 闸口径：真跑 `acceptance-run-check.mjs` 取 `surfaces.firmwareAcceptanceFiles` + 按同款 `collectFiles` 拆分 | 补 `archive` 前 **22**（>0 非空基线）→ 补后 **9**；构成 = 归档 **13** + 现役 **9**。**见 §3 偏差 1（预测值为 20→7，时点已变）** |
| V2 | `node .claude/scripts/acceptance-run-check.mjs --self-check` | **全绿**（`ok:true`、10 条）；**新增断言 `real-repo-no-archive-collected` 具名在场且 ok**（detail = `no-archive-in-enumeration`）✓ |
| V3 | `dispatch-stamp.mjs` **四路探针**（为避写副作用，②③ 用**不匹配的 expect 值**触发提前退出） | ① `grep -c "resolved changeId"` = **1**；② 读指针路径回声 = `(from delivery/taskpackages/current/taskpackage.json)` ✓；③ 显式 flag 路径回声 = `(from --task-package-id)` ✓（**两来源如实区分**，R1-F6 满足）；④ 无上下文：带 flag → **exit 2**、不带 → **exit 0**（既有行为不变）✓ |
| V4 | `grep -c` 成文在场 | `archive` = **5**、`tighten` = **2**（原均 0）；`^### 5. 退役保留目录名` = **1**、`^### 6. tighten-only` = **1**（两 T 互不撞）✓ |
| V5 | `route-regression-check.mjs` + `lint-descriptions.mjs` | `28 ok, 0 fail`；lint exit 0（仅既有 `landing-verifier.md` 超长告警）✓ |
| V6 | `git diff .claude/scripts/changes-index.mjs` | **零改动**（证明原 T1〔索引终态锚〕确已移出、未夹带）✓ |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §5（由 landing-verifier 亲跑） |

**真空绿规约核对**：V1 两侧实测（补前必 > 0）；V2 断言须**具名**且 ok（防「加了断言没接线」）；V3 的 ④ 覆盖 F6 新定义态**两边**、② 与 ③ 分别测两条来源（防回声写死）。

## 3. 落地中的事实性偏差（如实申报）

1. **V1 的件数预测已过时（非本批失误，时点变化）**：FROZEN §5 V1 写「补 `archive` 前 = **20**、补后 = **7**；构成 = 归档 13 + 现役 7」——那是**起草期（2026-09-18）的时点值**。本批落地前执行了 **main → gov 回同步 merge**（`2492f44`，四批共同前置），把 main 侧 m2 系列的 **2 件新现役验收件**（`char_window_overlay.acceptance.c` / `retract_p4_three_page.acceptance.c`）带了进来 ⇒ 现役 **7 → 9**、闸口径 **20 → 22**、补后 **7 → 9**。**归档面 13 件未变。**
   **改法不受影响**（T1 是「集合补一个成员」，与件数无关）；**危害定性同样不受影响**（仍为「枚举里混着 13 件退役件」）。**PROPOSAL §1 与 §3 里引的 20/13/7 三数同为此理**——彼时为真、现时已变，**不改冻结稿**，以本行为准。
2. **`--expect-change-id` 的退出码取 2**：FROZEN §2 T2② 只写「fail-loud 退出」，未指定码。实现取 **2**（与脚本既有「参数错误」约定一致——`:161-162` 的 duration/tool-uses 校验即用 2）。属具体化，非偏离。
3. **`--preflight` 无关本批**（属 P4）；本批未触 `landing-fidelity-check.mjs`。
4. **§6 内保留「无 `sha256` 核销先例」的如实声明**：FROZEN §2 T3 第 2 条要求该声明，故 §6 正文含「本仓现无已执行的 `sha256` 核销先例——`7ab2d47` 仅留行数计数、未记 hash；本节落地后为首例」。该句是**规则内的事实注记**（非「规约未生效」），保留在成文内以便首个执行者知道自己在开路。
5. **结构门 citation advisory 3 条（如实披露）**：门报 `hooks/verify.mjs:123 -> file-not-exist` **×3**。该引据在 **PROPOSAL §6 留根③**（冻结稿、落地者不可改），形态为**相对 `.claude/` 的短形态**（真身 `.claude/hooks/verify.mjs`，候选域含 `.claude/hooks/` 但**不含裸 `hooks/`**）。门照设计「只观察不产 fail」（`ok:true` 不受影响）。

   **跨批观察（本族累计）**：这批短形态引据**在四批里都出现过**——本批 `hooks/verify.mjs`、P1 的 5 条（`<批次>/PROPOSAL.md` 类）、P4 残留 1 条（`<批次>/retro.md`）。三者的共性是**省略了仓内已知前缀的相对名**（`hooks/` ↔ `.claude/hooks/`、`<batch>/` ↔ `delivery/changes/arch/<batch>/`）。**P4-T1 已把候选域扩到「原四前缀 ∪ 递归派生」，但派生是从仓根起算的，覆盖不到这类「相对某个已知根」的短形态。** 如实登记；**建议随「citation 候选域」的下一个治理批一并评估**（届时需先定「哪些根可省」的白名单语义）。

## 4. 残余风险 / 留根指针

- **留根①（原 T1〔索引终态锚〕移出）**：完整交接见 PROPOSAL §6 留根①——含 **11 条「已合 main 却非 landed」census 表（六种成因）**、**病灶定性**（脚本自身设计意图是对的：`:176` 注释的「新批走机器锚 + 旧批归 `LANDED_ADJUDICATION` 人工 pin」；坏的是**新批没照做**、六条近批一条都没进 pin 表 ⇒ 「补一条锚」是治标、会变成打地鼠）、以及**建议方向**（按脚本自身设计意图收口：6 条按实况 pin + 点分编号补进正则 + verdict 行规范成文）。**该 ARQ 仍在册未结**，队列行已补注。
- **留根②（T3 的引用行一侧未落）**：本批只落 DEV-GOVERNANCE §6；`acceptance-author` agent def 的 References 引用行需动 agent def、触 self-def 判断点，留后续。
- **留根③（`current/` 同族解析点未改）**：本批只改 `dispatch-stamp.mjs`；实测 **17 个文件**带该解析面、其中 `hooks/verify.mjs:123` 恰是 ARQ 病例①「九闸跑错包上下文全绿」的承载者。待观察 `--expect-change-id` 采用率再决定推广。
- **留根⑧（`sha256` 核销的工具面）**：本批只成文落点、不提供脚本辅助；若首例执行时发现手工易错，随任一治理批补工具。
- **交日落复盘项**：T1① 后观察 final 相位是否如预期不再枚举退役件；T1 census（11 条）建议在下一批落地后复跑一次确认清零。
- **本批为四批拆包族之一，且是最后落地的一批**（前三批 = batch-drafting `95bc44b` / dispatch-chain `ae85306` / merge-chain `69b9928`，均已 landed）。四批零文件重叠。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V7 + 逐文件人读 diff 对照 FROZEN §2）。

## 6. 核验后注记（append-only，2026-09-19）

landing-verifier 判 **PASS + 3 ADVISORY**（`landing-verdict.md`）。三处观察及处置——**本节只追加，上文不改**：

- **A1（节位标注错误，已在此更正）**：上文 §3-5 称 3 条短形态引据 `hooks/verify.mjs:123` 落在「**§6 留根③**」——**错**。核验官实测：该 3 条在 **§7 判官记录**（R1 记录内），而 **§6 留根③ 用的是可解析全形 `.claude/hooks/verify.mjs:123`**（故未进 advisory）。**实质结论不变**（仍是同族短形态引据、仍建议随下一批评估候选域），仅节位自述有误。**以本行为准。**
- **A2（结构门假阳性，非本批问题）**：门报 `changedCanonicalNotInProposal: .claude/scripts/pre-merge-escalation-gate.mjs` —— 经核属 **`baseRef=HEAD~1` 的假阳性**（该文件由 **HEAD 那一批** merge-chain `69b9928` 所改，本批工作树未触）。**无处置**；供后续核验官参考。
- **A3（超字面但已如实，非阻断）**：`dispatch-stamp` 回声在 **null 无上下文态**追加了消歧后缀（`（不存在）` / `（不可解析）`），**超出 FROZEN §2 字面给的二形**；核验官判：如实反映来源、**无写域/权限扩张**，两**非空态逐字用被批形**。**处置：本批不动**（已通过核验，事后改动会使 verdict 失效）。