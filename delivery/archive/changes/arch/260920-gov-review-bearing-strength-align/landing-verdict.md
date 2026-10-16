# landing-verdict · 260920-gov-review-bearing-strength-align

verdict: PASS + 3 ADVISORY

> 核验官 = landing-verifier（零上下文独立推导；不采信主线程转述，一切从 FROZEN PROPOSAL + 工作树 diff 重推）。
> 核验日期 2026-09-20 · 基线 = **FROZEN PROPOSAL v1.0**（`PROPOSAL.md` §2 改法集 T1–T3 + §5 验证集 + landing-manifest 7 路径），**不是任何 commit**。
> 落地形态：7 个 `.claude/**` 文件改动在**工作树、未 commit**；`.codebase-memory/` 两件为机器再生面脏、不计入。本判定**只读** codebase，未改任何被验文件。
> **本轮 = F1 复验轮**：首轮判 `FAIL(F1)`（`dev-loop.md` 必列项落点与 FROZEN 相反）；主线程已修（移回 `residual risk` 之前）+ summary §3 补第 6 条申报。复验确认修复到位 → 转 PASS。同轮复核并**撤回**首轮 A3（经复核实为核验官误读）。

---

## 复验结论（本轮）

| 复验项 | 结论 |
|---|---|
| F1 位次修复 | **已修**——`dev-loop.md:247` = 未覆盖面申报 → `:248` = residual risk → `:249` = S2 exploratory evidence；与 FROZEN §2 T3 改法 1「置 `residual risk` 之前」一致；**内容一字未动**（逐字仍为首版 bullet）。 |
| summary §3 第 6 条申报 | **已补**——`architect-landing-summary.md:49` 如实记录该落点偏差 + F1 判定依据 + 处置（移回 `residual risk` 之前）+ 教训（「位置副词是承重指令，不是修辞」）。落实「该记未记」。 |
| 结构门复跑 | **仍 GREEN**——exit 0、`ok:true`、`frozen:true`、`packetBinding:true`、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`。 |
| 改动面核对 | `git status`：仍恰 7 个 `.claude/**`；`dev-loop.md` numstat 仍 `2/1`（纯位移、内容不变）；**PROPOSAL.md 未被改**（`git status` 空）。 |
| A3 复核 | **撤回**（首轮误读，见下 A3 更正）。 |

---

## 首轮 F1 详情（已修复，留痕）

| 字段 | 内容 |
|---|---|
| failingFile | `.claude/references/dev-loop.md` |
| expected | FROZEN §2 T3 改法 1（`PROPOSAL.md:124`）显式定位：「未覆盖面申报」必列项**置于 `residual risk` 之前**（原文 `（置 \`residual risk\` 之前，逐字形态见下）`） |
| actual（首轮） | 该 bullet 落在 **`residual risk` 之后**（彼时 `dev-loop.md:248`，`:247` = residual risk） |
| actual（复验） | 已移回之前：`:247` 未覆盖面申报 / `:248` residual risk —— **F1 消解** |
| category | 与批定矛盾（落点相反）+ 未申报偏差（首轮 summary §3 五条未含） |
| 处置 | 主线程移回 bullet（内容不动）+ summary §3 补第 6 条申报 → 复验通过 |

---

## 六项逐项结论

| # | 项 | 结论 |
|---|---|---|
| 1 | 逐文件 diff 对照（语义保真 / 私货） | **GREEN**（F1 修复后 7/7 语义保真、无超出 packet 的私货；见下逐文件表） |
| 2 | packet-binding 一致 | **GREEN** |
| 3 | 验证集复跑（V1–V8） | **GREEN**（8/8 与 summary 声称一致，含 V2 断言数 28、V8 证伪实验） |
| 4 | 结构门复跑 | **GREEN**（exit 0，7/7，零 unauthorized / claimed-not-landed / planned-not-landed；`cross-surface-check.mjs` 本仓不存在，GAR-001 已裁剪为「脚本在场才跑」） |
| 5 | 真空绿规约 | **GREEN**（V5/V8 临时面先断言非空；V2 核新增自检名 + 调用顺序；V4 环境红基线与改动前逐字相同） |
| 6 | 诚实披露核对 | **GREEN**（首轮五条申报逐条与实况一致；首轮「该记未记」即 F1，第 6 条已补） |

---

## packet-binding 核对（GREEN）

- summary packet-id 行引 `FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集` + 评审轮次 `R1 BLOCK(F1) → R2 GREEN`——与 `PROPOSAL.md` §7 评审记录一致。✓
- manifest ≡ §2 各 T 目标文件并集：T1 = {task-intake-and-slicing.md, brief-inputs.mjs}；T2 = {acceptance-evidence.md, acceptance-author.md}；T3 = {dev-loop.md, pod-coordination.md, planner.md} → 并集 7 路径，与 §2 围栏块、summary 围栏块**三方逐字相同**（结构门 `proposalTargets == summaryTargets` 已机核）✓
- **OQ2 裁 (a) 的「不扩 manifest」声明属实**：两条搭车件——搭车件 5（`acceptance-author.md` tighten-only 引用行）、搭车件 6（`acceptance-evidence.md` root 注入节）——落点文件均已在既有 7 路径内，故 manifest 未增路径。亲核 diff 确认二者只改这两个文件内内容。✓
- 实际落地 ⊆ 被批 packet：结构门 `unauthorized: []`；人读 diff 未发现未授权 canonical 改动。✓

---

## 逐文件 diff 核对

| 文件 | manifest T 项 | 语义保真 | 说明 |
|---|---|---|---|
| `.claude/references/task-intake-and-slicing.md` | T1① | ok | `planRef` 条后新增 **2 bullet**：① `crossPackageBehaviors` 字段级定义（canonical `{topic, owner, note, carriedBy}` + 四项语义 + 「写侧归一 canonical / 读侧兼容 `behavior` ∨ `topic` / 承接包 = `carriedBy ?? owner`」+ 否决 `owner` 单键判承接）；② 两写面 + 必填判据①②（含 `art_assets.c:10` 殿后反例）+ 两处先例锚（`:100-105` / `:116-141`，第 2 条 `:123-128`）+ 声明消费方 = 第五路。对 FROZEN §2 T1 改法 1 全落。✓ |
| `.claude/scripts/brief-inputs.mjs` | T1②③ | ok | ① 文件头「四路展开」补第五路说明；② 新增 `readJsonOrNull` / `cellText` / `collectCrossPackageCarry`（扫两写面、`(carriedBy ?? owner) === selfId`、`_archive`/自包排除、坏 JSON 出 `> ⚠` 可见行不阻断）；③ `renderBrief` 增 `repoRoot = opts.repoRoot \|\| projectDir` 并渲染 `## ⑤ 跨包承接面`（含 `none` 落地 + 表头 `carriedBy ?? owner` 列）；④ `--self-check` +5 断言（含夹具非空前置）。对 FROZEN §2 T1 改法 2/3 全落。**唯一未申报实现细节见 A2**。✓ |
| `.claude/references/acceptance-evidence.md` | T2① + 搭车件6 | ok | 「破坏性自检」与「反例」之间新增 **2 节**（21/0、无删行）：①「契约条目 ↔ 齿 的强度申报」（映射三档 / 弱于契约显式声明 / 行为等价假设显式化「不写即默认不成立」/ 落点 = 冻结件头注 + 机制断言优先 + **明示不设门**）；②「evidence 类扫描齿的 root 注入」（承 `HOST_TEST_CONVENTIONS.md:66` [C4.4]，同一 ELF 3 vs 2 反例）。逐点对 FROZEN §2 T2 改法 1–4 + 搭车件 6 全落。✓ |
| `.claude/agents/acceptance-author.md` | T2② + 搭车件5 | ok | References 节**追加 2 引用行**（结构零改动、2/0）：① `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6「tighten-only 修订窗口」（结 `ARQ-260908-tighten-only-unwritten`）；② `acceptance-evidence.md` 两新节。**未触 frontmatter**（无 `name/description/tools/model` 行）。✓ |
| `.claude/references/dev-loop.md` | T3① | ok（F1 修复后） | ① 必列项新增「未覆盖面申报」，**位置已修**（`:247`，在 `residual risk` :248 之前，符 FROZEN）；② `:131` review-packet 制品清单行同步加该段名（summary §1② 已申报；见 A1）。✓ |
| `.claude/references/pod-coordination.md` | T3② | ok | §11（:194）demo 对账行段（:196）**之后同址**新增一段「未覆盖面申报（包收口必含）」——含「为什么单列一轴」/ 零缺口 `none` / 生产者可达性由 planner Step 11 携带 / `integrator.md` 不动。对 FROZEN §2 T3 改法 2 全落、与 demo 对账行同址。✓ |
| `.claude/agents/planner.md` | T3③ | ok | Step 11 携带句由「必含 demo 对账行」扩为「**必含 demo 对账行 + 未覆盖面申报**」并把两项义务各展开一句（1/1 改写）；`integrator.md` 未动；无 frontmatter 行。对 FROZEN §2 T3 改法 3 全落。✓ |

**敏感面必核（G1/G3/self-def/review-loop def）**：
- G3 = `brief-inputs.mjs`（唯一 G3 触点）：改动为**只读派生**，不参与任何门判定、不改既有闸语义、不改退出码路径（解析失败走 `notes` advisory 行，不 push `errors`）——与 FROZEN riskTier/profile 声明的「advisory 生成器、非门」一致。✓
- agent def（`acceptance-author.md` / `planner.md`，**非** self-def〔architect.md〕、**非** review-loop 集）：改动 = References 引用行 + 正文携带句，**零 frontmatter、零 write scope、零 authority 语义**——与 OQ1=(a) 裁决、FROZEN §3 profile 一致。✓
- G1 指令根（CLAUDE.md / AGENTS.md）、`docs/contracts/**`、hooks / scope-map / settings：**工作树零改动**。✓

---

## 验证集复跑（V1–V8，亲跑留痕）

| 项 | 我亲跑命令 | 关键输出 | 对 summary |
|---|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight …/PROPOSAL.md` | `{"ok":true,"anchorCount":7,"findings":[]}` exit 0 | 一致 ✓ |
| V2 | `node .claude/scripts/brief-inputs.mjs --self-check` | `28/28 passed / SELF-CHECK GREEN` exit 0 | 一致（28）✓ |
| V2 基线 | `git show HEAD:.claude/scripts/brief-inputs.mjs` → 临时文件跑 `--self-check` | `23/23 passed` | **基线 23 独立复现** → +5 属实 ✓ |
| V2 接线 | grep 新增自检名 | `⑤ 夹具非空` / `⑤ 命中·包面` / `⑤ 命中·workstream 面` / `⑤ 排除` / `⑤ 零命中明写 none` 5 条全在场且 PASS；无提前 exit 挡后续自检 ✓ | 一致 ✓ |
| V3 | `node .claude/scripts/route-regression-check.mjs` + `--self-check` | `28 ok, 0 fail` / `56 ok, 0 fail` + `✓ all required signals/whitelists falsifiable` | 与 claim 同值 ✓ |
| V4 | `node .claude/scripts/lint-descriptions.mjs` | exit 0；`scanned 13 … flagged 1`，唯一告警 `landing-verifier.md: over-length (363 > 320)` | 逐字同基线（既有、本批未新增）✓ |
| V5 | `brief-inputs.mjs --change-id 260915-m1-k4-reminder-overlay-unify --role builder --dry-run` | ⑤ 段命中 `workstream:m1-home-qa`「四态统一版式骨架」条（`owner=k3` / `carriedBy=k4`）——**正是按 `owner` 单键判会漏掉的那条** | 一致 ✓ |
| V5 | `… --change-id 260920-m2-charwindow-display-fix --role acceptance-author --dry-run` | ⑤ 段回落 `` `none` —— 两写面 … `` | 一致 ✓ |
| V6 | `git status --porcelain`（剔 `.codebase-memory/`）+ `git diff --numstat <7 file>` | 恰 7 路径；numstat `2/0` `1/1` `21/0` `2/1` `2/0` `2/0` `138/0` | **逐项与 summary §2 V6 数值全同** ✓ |
| V7 | grep 语义面 | task-intake `crossPackageBehaviors`=2；`未覆盖面申报` 3 文件命中（共 4）；强度申报节=1；root 注入节=1；acceptance-author 两引用行各 1 | 一致 ✓ |
| V8 | **反空过证伪（真制品副本，独立重做）**：拷 `m1-home-qa/decision-coverage.json`（12086 B，先断言含 `crossPackageBehaviors`）进临时 `CLAUDE_PROJECT_DIR` + 自包 `taskpackage.json` → 原名跑命中 k4 条；单字段变异 `carriedBy` → `260915-nonexistent-pkg` | 未变异：⑤ 段列出该条；变异后：**该条消失、回落 `none`**（临时面用后即删） | 一致（真过滤）✓ |

**真空绿规约核对**：V5/V8 临时 projectDir 先 `test -s` / `grep -q` **断言副本非空且含字段**再解释结果（V8 变异前断言夹具条目在盘 + `python` 单字段替换确认）；接线类（V2）验证标准 = 输出**含新增自检名** + 审查调用顺序 ✓；环境红基线：V4 唯一告警与起草期基线逐字相同（V2 基线 23 亦现跑复现）✓。

---

## 诚实披露核对（summary §3 六条）

| # | summary 申报 | 实况核对 | 判定 |
|---|---|---|---|
| 1 | FROZEN §2 T1 证据段两处行号各偏一行（冻结稿不动）；落地用真值 | FROZEN 确写 `:116-121`/`:122-127`；真值实读 = `:117-122`/`:123-128`；落地写 `:116-141` + 第 2 条 `:123-128` | 准确 ✓ |
| 2 | 第五路落为段号 ⑤、置于 ④ 之后 | 代码：④ 段后 `L.push('## ⑤ 跨包承接面…')` | 准确 ✓ |
| 3 | 包面扫描排除本包自身（`dir.name === selfId` 跳过） | 代码确含 `dir.name === selfId` 跳过 + 自包夹具 decoy 断言 | 准确 ✓ |
| 4 | 解析失败不静默、也不 fail-loud（出 `> ⚠ …` 可见行） | `readJsonOrNull` 失败 → `notes.push('> ⚠ …')`，不 push `errors`、不 `process.exit` | 准确 ✓ |
| 5 | acceptance-evidence 两新节插在既有节间、未改既有节文字 | diff 21/0（零删行）；插于「破坏性自检」与「反例」之间 | 准确 ✓ |
| 6（本轮补） | 核验期 F1 落点偏差 + 处置（移回 `residual risk` 之前）+ 教训 | `architect-landing-summary.md:49` 如实记录；与实况一致 | 准确 ✓ |

**该记未记**：首轮 1 条（F1）→ 本轮第 6 条已补，核验通过。

---

## ADVISORY 清单（非阻断观察）

- **A1（低）· `dev-loop.md:131` 制品清单行同步改动超出 §2 T3 改法 1 字面范围**：FROZEN §2 T3 改法 1 只授权「必列项增一条」，实改亦动了 `:131`「review-packet 制品清单」描述行（加该段名）。该行是 review-packet 内容的**描述性**清单，同步以免两份描述漂移，属改法意图内的**一致性维护**，且 summary §1② 已申报；无权限/写域语义。无需返工，登记即可。
- **A2（低）· `brief-inputs.mjs` 包面扫描另跳过目录 `current`**：代码 `if (!dir.isDirectory() || dir.name === 'current' || dir.name === selfId) continue;`——除已申报的「自包排除」（§3-#3）外，另跳过 `current`。此为**承 `findPrecedents`（:243-246）既有惯例**（③路先例扫描同样排除 `current` 指针副本），benign、无外部语义。无需返工。
- **A4（告知）· 结构门 citationAnchorAdvisory 4 条 `file-not-exist`**：`progress.md:18` / `m1-home-qa/decision-coverage.json:116` / `_archive/…decision-coverage.json:113` / `…/PROPOSAL.md:126` 因短路径形态未被解析器定位；属 advisory、`ok:true` 不受影响；这 4 处出处经人工抽查存在或为相对引用形态。

### A3 更正（撤回首轮 A3）

**首轮 A3 撤回**。首轮记「FROZEN §3:149 旧语义（N1）未随 landed」——**经复核实为核验官误读**：
- 实测 `PROPOSAL.md:149`（§3 硬项句）**已是 v0.3 修订后语义**——正文逐字 = 「**四者共用「承接包 = `carriedBy ?? owner`」这一条语义**（R2-N1 折入：v0.3 原写「owner 指向承接包 changeId」，未随 F1 的 `carriedBy` 语义同步；`owner` = 语义 owner、`carriedBy` = 承接包、缺省回落 `owner`）」。旧短语「owner 指向承接包」在该行**仅出现于括号内的历史引述**（「v0.3 原写」），**非 §3 本体措辞**；首轮 grep 命中该行即误当残留。
- 全稿仍含该旧短语的位置 = **`:270` 与 `:286`**，二者**均在 §7（append-only 判官记录区）**：`:270` = R2 verdict 括注、（`:286` = R2 N1 finding 的证据引述，引的是**改前** §3 文本）。§7 是历史陈述、**不应改**。
- **结论**：§3 本体无残留，**A3 不成立，撤回**。main-thread 判读正确。

---

## 放行建议

**PASS，可放行**：F1 已修（`dev-loop.md` 必列项移回 `residual risk` 之前、内容一字未动）、summary §3 第 6 条申报已补、结构门与验证集 V1–V8 全 GREEN、packet-binding 与敏感面全 GREEN、A3 已撤回。剩余 3 条 ADVISORY（A1/A2/A4）均非阻断、无需返工。**merge 仍归 human manual-merge 门**（本核验只给 verdict）。
