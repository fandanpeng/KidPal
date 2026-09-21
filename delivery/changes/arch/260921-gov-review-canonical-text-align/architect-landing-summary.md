# architect-landing-summary · 260921-gov-review-canonical-text-align

packet-id：FROZEN PROPOSAL v0.4.1 §2 改法集（**T1 / T2**）+ §5 验证集 · 评审轮次 R1 `BLOCK(F1,F2,F3,F4,F5)` → R2 `BLOCK(F7,F8,F9)` → **R3 GREEN** · change-id 260921-gov-review-canonical-text-align

## 0. 本批特殊形态（核验官先读）

1. **G1 零触面是本批的成立前提，不是副产品**。human 首裁 OQ-1 = **(b)**（`/arch-change` = 跨双轨统一入口）；R1 判官 **F1** 实测出 `docs/DEV-GOVERNANCE.md`（G1 指令根）**现文已全是 (a) 口径**且该文件不在 manifest / review profile 内 ⇒ (b) 须连带重写 G1、文件数超 human ack 的「3–4」。主线程**携该新信息回请重拍**，human **重拍 = (a)**，理由正是「G1 现文本就全是 (a)，故 (a) 零 G1 触面」。**故 `AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` 三文件本批零改动**——请核验官逐文件确认（§2 V2 的「未改动正向断言」），**任一文件出现改动即为落地越界**。
2. **F9 的处置 = 判官给的两个方向里的「方向二」**：定义句收窄 + 追加边界句，**未动** `DEV-GOVERNANCE.md:254` 的「统一入口」段标题。**理由即第 1 条**——改该标题会把 G1 重新拉进 landing-manifest（3→4 路径），与 human 重拍 (a) 的前提相悖。R3 判官独立复核后裁「方向二足矣、方向一不必」。
3. **T1 的承重指令 ② 改的是 frontmatter `description`**（路由信号面，受 `lint-descriptions.mjs` 字数预算 + `route-regression-check.mjs` 必留信号双守卫）。改动**逐字段隔离**：只动 `description`，`name` / `tools` / `model` 一字未碰（§2 V2 给出逐行 diff）。
4. **V4 的冻结断言按字面不可达（本批唯一验证项缺口）**——详见 §3 第 1 条。核验官请独立判读其性质。

## 1. 落点对照（manifest 3/3）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/references/arch-review-protocol.md` | T1 · 承重指令 ① | `:15` 的「统一入口 `/arch-change`」句**原文一字未动**，其后**追加**定义句 + 边界句：定义句写死「统一」的所指 = **Track A 内部的 spec / governance 两个 profile**（`/arch-change` 是 Track A 的入口；Track B 无命令入口、入口 = 主线程直接起草 PROPOSAL），并点明「命中升级判据 → `/arch-change`」与「默认 Track B → 主线程起草」两句各指其一、不冲突；边界句（承 R2-F9）明说「统一入口」在本仓是**通用短语**而非本命令专名，`docs/DEV-GOVERNANCE.md` §7 该词段标题指**入口总览**义、`.claude/agents/portfolio-planner.md:25` 指 **repair-intent 的收口处**义，二者**不在本句覆盖范围内**。 |
| `.claude/commands/arch-change.md` | T1 · 承重指令 ② | `:2` `description` 的 `架构/治理变更统一入口（对偶 /build-feature）。` → `架构/治理变更统一入口（Track A）（对偶 /build-feature）。`——**纯加限定词、不重写**（+9 字符）；该行**其余 130 余字一字未动**，`name`/`tools`/`model`/`argument-hint` 未触，文件正文（`:6` 起的步骤与说明）未触。 |
| `.claude/references/track-b-verifier-brief.md` | T2 | 二处改动：① 核验清单第 4 项由「跑两个脚本」改为**只留在场者**（`landing-fidelity-check.mjs`），并把缺件事实就地钉死（「本仓无 `cross-surface-check.mjs`——该件为自源仓迁移带入的悬空引用，本仓从未存在；若你到场的清单里仍见到它，即为本文修订未生效，记 ADVISORY 并按『无此门』处理」）；② 头注「harness 降级标准声明」句后**追加一行本仓实况注记**（本仓**有** typed `landing-verifier` agent，故降级头注不适用；本仓**无**该脚本）。**第 4 项未删任何其它内容**；头注原文未删（留根③未处理，见 §4）。 |

```landing-manifest
.claude/commands/arch-change.md
.claude/references/arch-review-protocol.md
.claude/references/track-b-verifier-brief.md
```

**G1 零落地核对**：`AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` **三文件均未被本批触碰**（`git diff --stat -- <三者>` 空输出，§2 V2）。

## 2. 验证集执行记录（V1–V6）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `landing-fidelity-check.mjs --preflight <PROPOSAL>` | **`ok:true, anchorCount:3, findings:[]`** ✓，`anchorUnion` = `arch-change.md` / `arch-review-protocol.md` / `track-b-verifier-brief.md`，与 manifest 围栏块**恒等 3 路径**。（R1 时为 4——OQ-1 裁 (b) 曾带 `AGENTS.md`；重拍 (a) 后回落为 3。R1/R2/R3 判官各独立复跑，末次 R3 = 同上。） |
| V2 | T1 定义句在场 + 定位表述未改动 | **四条子断言全绿**：① `grep -c "统一的\*\*不是\*\* Track A 与 Track B 两条轨" .claude/references/arch-review-protocol.md` = **1**，且边界句 `grep -c "不在本句的覆盖范围内"` = **1**；② `grep -c "统一入口（Track A）" .claude/commands/arch-change.md` = **1**（`:2` description）；③ **「未改动」正向断言**：`git diff --stat -- AGENTS.md docs/DEV-GOVERNANCE.md CLAUDE.md` = **空输出**（G1 零触面 ✓）；④ `git diff -- .claude/commands/arch-change.md` 逐行只显示 **`description` 一行变化**（`arch-change.md:2`），无其它行。**合取判据（人读，判官/核验官判）**：读 ① 的定义句与 ② 的限定词应得出同一规则——「`/arch-change` = **Track A** 的统一入口（统一的是 Track A 内 spec/governance 两 profile）；Track B 无命令入口，由主线程直接起草」。 |
| V3 | T2 悬空引用收窄 | `grep -n "cross-surface-check" .claude/references/track-b-verifier-brief.md` → **2 命中，两处均在「本仓无此件」的说明句内**（`:4` 头注实况注记、`:24` 清单第 4 项）；**裸的「跑它」指令已消失** ✓。**范围收窄声明（承 R1-F2）**：本断言**只覆盖 verifier 简报**——`landing-fidelity-check.mjs`（5 行 `existsSync` 守卫）/ `governance-gate-shared.mjs`（2 行引据）/ `ARCH-REVIEW-QUEUE.md:85`（已结 ARQ）**各有应当保留的引据，落地后仍命中，属预期**。 |
| V4 | verifier 简报引用存在性核 | ⚠ **按字面不可达——见 §3 第 1 条**（本批唯一验证项缺口）。实测：该断言要求「零 MISS」，而 T2 的**改法本身明文要求**在被修订的那一行里写出 `cross-surface-check.mjs` 这个 token ⇒ 存在性核**必然 MISS**、且该 MISS 是正确落地形态。 |
| V5 | `route-regression-check.mjs` + `lint-descriptions.mjs` | **`route-regression-check`: `28 ok, 0 fail`** ✓。**「不减」可证**：该脚本三项扫描面 = ① 命令文件正文的 `你现在是…<entryAgent>` 行（`arch-change.md` 的该行在 `:10`，本批未触）② `.claude/agents/*.md` 的 `description` ③ agent 的 `tools:Agent()` 行——**本批三处改动（command description + 两个 reference）全部落在扫描面之外**，故 28 = 落地前基线（无需构造基线即可证）。**`lint-descriptions`: `scanned 13, exempt 0, excluded 1 (architect.md), flagged 1`** —— 唯一点名 = **既有** `landing-verifier.md: over-length (363 > 320)`，**无新增**；`arch-change.md` 的 description（+9 字符）**未超阈、未新告警** ✓（承重指令 ② 的 lint 断言成立）。 |
| V6 | `landing-fidelity-check.mjs <本批>` | 结构门要求 `architect-landing-summary.md` 在场——**本文件即其应答**；主线程已亲跑一次（§5 末），**由 landing-verifier 独立亲跑复核**；逐文件人读 diff 核对 manifest（3 路径）与 §2 各 T 目标文件并集恒等（T1 {`arch-review-protocol.md`, `arch-change.md`} ∪ T2 {`track-b-verifier-brief.md`} = 3 路径 == manifest）。 |

**真空绿规约核对**：本批无「重放类 / 接线类」断言（无脚本接线、无 `--self-check` 扩展）。涉闸执行层的红基线：本批**零脚本改动**，无需基线。

## 3. 落地中的事实性偏差（如实申报）

1. **V4 的冻结断言与 T2 的冻结改法自相矛盾，按字面不可达（本批唯一验证项缺口）**。

   V4 断言「对 `track-b-verifier-brief.md` 重跑引用存在性脚本 → **预期零 MISS**」；而 **T2 的承重改法**明文要求被修订的那一行**写出**该 token——替换句为「**（本仓无 `cross-surface-check.mjs`**——该件为自源仓迁移带入的悬空引用，**本仓从未存在**；…）」。⇒ **落地越正确，V4 越必红**。实测（§2 V4 行）：引用存在性核对该文件得 `MISS .claude/scripts/cross-surface-check.mjs`（唯一 MISS），**且该 MISS 正是 frozen 明文的产物**。

   ⇒ **这是 PROPOSAL §5 V4 的内部矛盾**（T2 改法与 V4 各自明确，V4 却写死零 MISS），**R1 / R2 / R3 三轮判官均未抓到**——与**首位乘客 `260921-gov-review-launch-pipeline` V7** 是**同一族病**（「零命中」类断言未与改法集的**保留面**逐条对账），且该批已把此形态作为「③ 连带项第二实例」登记进 `ARQ-260914-landing-preflight-anchor-parse`（本日另一提交）。**本批即该 ARQ 建议改动所针对的第三例**：断言口径与改法的保留面从未对账——**即便实跑，也先要问「预期本身是否写错」**。

   **主线程处置（不自行改 PROPOSAL）**：FROZEN 后不可回改 PROPOSAL，故**不动其文本**；落地**未越 frozen 改法一步**。此处如实登记，交 **landing-verifier 判读**（请明示：判「PROPOSAL 文本缺陷、非 landing 偏离」记 ADVISORY，或判 FAIL），并登记为后续批的修正项（§4-a）。

2. **T2 落地文本里的 change-id 缺 lane 前缀（frozen 自带，非落地偏离）**：清单第 4 项替换句的末尾注记写 `260921-canonical-text-align T2 删除。`，而本批实际目录名是 **`260921-gov-review-canonical-text-align`**——缺 `gov-review-` lane token（该 token 是 `.claude/scope-map.json` review-agent 写槽的强制前缀）。⇒ 落到 canonical 里的这个 id **在仓内 grep 不到对应目录**。

   **主线程处置**：**照 frozen 逐字落地、未私改一字**（落地 == frozen 是硬纪律；私改即「私货」）。此处如实登记——**请核验官独立判读**：这属「frozen 文本自带的悬空标识符」，与 T2 正在治的病**同族**（T2 删掉一处悬空引用、却在同一句里种下一个新的不可解析 id）。若判需修，请明示走「后续小批」还是「本批修订重派」。

3. **头注注记的措辞由主线程在 landing 期拟定**（frozen 明文授权）：PROPOSAL §2 T2 的改法第 2 段写「并在该简报头注的『harness 降级标准声明』句后补一句同旨注记（**落点措辞由 architect 定**）」。主线程据该授权拟定的实际措辞见 §1 该行（两件事实同处可见：本仓有 typed agent、本仓无该脚本）。**该措辞不在 frozen 逐字清单内**，如实登记供核验官判读。

## 4. 残余风险 / 留根指针

| # | 项 | 去向 |
|---|---|---|
| a | **V4 断言过宽**（§3 第 1 条）——「零 MISS」类断言未与改法保留面对账 | FROZEN 后不可加 §6 → **建议随下次动 §5 / PROPOSAL 模板的批一并修**；本形态已作为第三例记入 `ARQ-260914-landing-preflight-anchor-parse`（③ 连带项），retro 复核 |
| b | **T2 落地的 change-id 缺 lane 前缀**（§3 第 2 条） | 交核验官判读；若须修，走后续小批（涉 canonical 文本改字） |
| c | **canonical 悬空引用全量扫描未做** | PROPOSAL §6 留根②，交接动作 = 一次全量只读 census |
| d | **两份简报的「Kimi 降级头注」在本仓不适用** | PROPOSAL §6 留根③；本批**只追加实况注记、未删头注**（删它属 harness 形态声明面，须单独判） |
| e | **「面间对账」不立机械齿** | PROPOSAL §6 留根①；触发条件 = 同类实例再现（**本批 V4 即第三例**），交 `/loop-retro` 观察 |

## 5. 落地证据 + manifest 外簿记编辑（逐项申报，供核验官复核）

- **manifest 内落地面 = 恰 3 路径**（`git diff --stat -- .claude/` 输出恰 3 文件 / 4 insertions / 3 deletions），**无夹带**。
- **manifest 外的簿记编辑（两项，逐项申报——请核验官先读本节再判「夹带」）**：

| 文件 | 性质 | 依据 |
|---|---|---|
| `delivery/changes/arch/INDEX.md` | 本批状态行（`--upsert`，幂等 append） | 仓内七步闭环常态动作（`AGENTS.md`「治理路由」节第 7 步：状态转移时跑 `changes-index.mjs --upsert <id>`）；本批行在会话早期以 `draft` 态写入 |
| `docs/ARCH-REVIEW-QUEUE.md` | `ARQ-260914-landing-preflight-anchor-parse` 行追加「③ 连带项 · 第二实例」注记 | **上一轮人指令「#1 先登记 ARQ」**的直接产物（针对**首位乘客**的落地期实测），**非本批产物**；已在 landing 前**独立提交**（`cbe7245`），故不混入本批 landing commit |

- `.codebase-memory/artifact.json` / `graph.db.zst` 亦在 `git status` 中，**属工具自刷新派生面、非本批改动**，按仓内纪律（`AGENTS.md`「CodebaseMemory 使用纪律」）**不追交、不 stage**。
- **主线程亲跑结构门**（本汇总为门要求的前置制品，写完本文件后跑）：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260921-gov-review-canonical-text-align` → 见下方「门输出」；**由 `landing-verifier` 独立亲跑复核**。
- **落地前后三文件的核心断言**：`git diff --stat -- .claude/` = `arch-change.md | 2 +-` / `arch-review-protocol.md | 2 +-` / `track-b-verifier-brief.md | 3 ++-`。

**门输出（主线程亲跑，2026-09-21，落地 commit 前；请核验官独立复跑比对）**：

```json
{ "ok": true, "gate": "landing-fidelity", "frozen": true, "packetBinding": true,
  "proposalTargets": ["arch-change.md", "arch-review-protocol.md", "track-b-verifier-brief.md"],  // 三者均带 .claude/ 前缀，此处为可读省略
  "summaryTargets": ["<同上三者>"],
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [], "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 52, "problems": [], "abbreviated": [], "ambiguousShortForm": [], "shortFormUnresolved": [] },
  "verbatimQuoteAdvisory": { "checked": [], "missing": [] },
  "advisory": { "changedCanonicalNotInProposal": ["docs/ARCH-REVIEW-QUEUE.md"] } }
```

- `proposalTargets` == `summaryTargets` == manifest **3 路径**，`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 三空 ✓。
- `citationAnchorAdvisory`：**checked 52、problems 0**——PROPOSAL 引用的行号锚全部可解析（含本批新增的 `DEV-GOVERNANCE.md:254` / `portfolio-planner.md:25`）。
- **`advisory.changedCanonicalNotInProposal = ["docs/ARCH-REVIEW-QUEUE.md"]`**：即 §5 表所列的**上一轮 ARQ 簿记**（已独立提交 `cbe7245`）；门以 `HEAD~1` 为 baseRef，故该提交进入了 diff。**非本批改动、非夹带**——本批 landing commit 不含该文件。
