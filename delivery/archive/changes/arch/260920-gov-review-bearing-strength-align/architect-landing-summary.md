# architect-landing-summary · 260920-gov-review-bearing-strength-align

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 **R1 `BLOCK(F1)`（1 SHOULD + 2 NIT，无 BLOCKING、无升级 human）→ v0.3 → R2 `GREEN`（1 NIT 不拦）** · change-id 260920-gov-review-bearing-strength-align · OQ1=(a) / OQ2=(a)（human 2026-09-20）

## 1. 落点对照（manifest 7/7）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/references/task-intake-and-slicing.md` | T1① | 「rigor 字段与 Lite TaskPackage 字段级定义」节内、`planRef` 条之后**新增两条 bullet**：`crossPackageBehaviors` 字段级定义（canonical `{topic, owner, note, carriedBy}` + 四项语义 + 两写面收编规则〔写侧归一 / 读侧兼容 `behavior` ∨ `topic` / 承接包判定 `carriedBy ?? owner`〕）+ 两写面与必填判据（含 `art_assets.c:10` 反例形态与两处先例锚）。 |
| `.claude/scripts/brief-inputs.mjs` | T1②③ | ① 文件头「四路展开」段**补第五路说明**（含承接包判定与两写面语义）；② 新增 `readJsonOrNull()` / `cellText()` / `collectCrossPackageCarry(tp, repoRoot)` 三函数；③ `renderBrief()` 增 `repoRoot = opts.repoRoot \|\| projectDir` 并渲染 **`## ⑤ 跨包承接面`** 段（表头含「承接包（`carriedBy ?? owner`）」列；零命中明写 `none`；解析失败出 `> ⚠ …` 可见行）；④ `--self-check` **+5 夹具**（23 → **28**）。 |
| `.claude/references/acceptance-evidence.md` | T2①+搭车件6 | 「破坏性自检」与「反例」之间**新增两节**：**「契约条目 ↔ 齿 的强度申报」**（映射 / 弱化显式声明 / 行为等价假设显式化 / 落点 = 冻结件头注；机制断言优先；明示不设门）+ **「evidence 类扫描齿的 root 注入」**（承 `HOST_TEST_CONVENTIONS.md:66` [C4.4] 的文档命令位实证：同一 ELF 3 条 vs 2 条）。 |
| `.claude/agents/acceptance-author.md` | T2②+搭车件5 | References 节**追加两条引用行**（结构零改动）：① `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6「tighten-only 修订窗口」（结 `ARQ-260908-tighten-only-unwritten`）；② `acceptance-evidence.md` 的两新节（强度申报 + root 注入）。 |
| `.claude/references/dev-loop.md` | T3① | ① 「### 合并门 Review Packet」必列项**追加一条**「未覆盖面申报」（含齿覆盖轴 / 证据形态轴正交说明、零缺口明写 `none`、生产者可达性由 planner Step 11 携带、既存实况 23/33 与 MCD-A6 例）；② `:134` 的 review-packet 制品清单行**同步**加该段名。 |
| `.claude/references/pod-coordination.md` | T3② | §11 内、demo 对账行段之后**新增一段**「未覆盖面申报（包收口必含）」——与 demo 对账行同址正交，写明「为什么单列一轴」（coverage 只看已声明条目、锚 3 只看证据形态，二者对「根本没进 `acceptance[]` 的设计性质」全沉默）。 |
| `.claude/agents/planner.md` | T3③ | Step 11 **改写携带句**（原「必含 demo 对账行」→「必含 demo 对账行 + 未覆盖面申报」），并把两项义务各展开一句；`integrator.md` 不动（承 260912 锚 3 先例）。 |

```landing-manifest
.claude/agents/acceptance-author.md
.claude/agents/planner.md
.claude/references/acceptance-evidence.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/brief-inputs.mjs
```

## 2. 验证集执行记录（V1–V8）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本件>` | **`ok:true` / `anchorCount: 7` / `findings: []`**（起草期 + 标冻前各一次）✓ |
| V2 | `node .claude/scripts/brief-inputs.mjs --self-check` | **28/28 passed、SELF-CHECK GREEN**（基线 23 → +5）；新夹具四类：⑤ 命中·包面（`behavior` 存量键 + `owner` 判承接）/ ⑤ 命中·workstream 面（按 `carriedBy` 判承接、`owner` 是别包）/ ⑤ 排除（承接包是别包 ∧ `owner` 指本包但 `carriedBy` 是别包 ∧ 自包登记 ∧ 归档面）/ ⑤ 零命中 `none` ✓ |
| V3 | `node .claude/scripts/route-regression-check.mjs` + `--self-check` | **`28 ok, 0 fail`**（与起草期基线同值）；`--self-check` **`56 ok, 0 fail`**（`✓ all required signals/whitelists falsifiable`）✓ |
| V4 | `node .claude/scripts/lint-descriptions.mjs` | **exit 0**；`scanned 13 … flagged 1`，唯一告警 = `landing-verifier.md: over-length (363 > 320)`——**与起草期基线逐字相同**（既有、本批未新增）✓ |
| V5 | 真包干跑：`brief-inputs.mjs --change-id 260915-m1-k4-reminder-overlay-unify --role builder --dry-run`（有承接）与 `--change-id 260920-m2-charwindow-display-fix --role acceptance-author --dry-run`（无承接） | **双向都通**——k4 的 ⑤ 段命中 `workstream:m1-home-qa` 那条「四态统一版式骨架」（`owner=260915-m1-k3-voice-round-ui` / `carriedBy=260915-m1-k4-reminder-overlay-unify`）**＝按 `owner` 单键判会漏掉的那条**；display-fix 的 ⑤ 段回落 `none` ✓ |
| V6 | `git status --porcelain`（剔 `.codebase-memory/`）+ `git diff --numstat` | **恰 7 路径 = manifest 7/7**，零多零少；numstat：`2/0`（acceptance-author）、`1/1`（planner）、`21/0`（acceptance-evidence）、`2/1`（dev-loop）、`2/0`（pod-coordination）、`2/0`（task-intake）、`138/0`（brief-inputs——纯新增函数与夹具）✓ |
| V7 | 语义面复核 grep | task-intake `crossPackageBehaviors` **2** 命中；`未覆盖面申报` **3 文件全命中**（dev-loop / pod-coordination / planner）；`契约条目 ↔ 齿 的强度申报` **1**；`evidence 类扫描齿的 root 注入` **1**；acceptance-author 两引用行 **2** ✓ |
| V8 | **反空过证伪（真制品副本）**：把 `m1-home-qa/decision-coverage.json` 复制进临时 projectDir，先原名跑→命中；再把该条 `carriedBy` 改成 `260915-nonexistent-pkg` | **未改 → 命中该条；单字段变异 → 该条消失、⑤ 段回落 `none`、命中行数 0**（临时面用后即清）⇒ 该路是**真过滤**，不是「一律列出」✓ |

**真空绿规约核对**：V2 的四类夹具均**先断夹具输入非空**（`⑤ 夹具非空（两写面 + 自包 + 归档四面均在盘）` 断言先行）再断输出；V5/V8 的临时 projectDir 亦先 `ls -l` 断副本非空（12086 B / 157 B）再跑。**V8 的变异有前置断言**（`if(!t) throw` 证夹具条目确实存在、防「变异没落上」的假绿）。

## 3. 落地中的事实性偏差（如实申报）

1. **FROZEN PROPOSAL §2 T1 证据段的两处行号范围各偏一行**（起草期笔误，冻结稿不动）：§2 写 workstream 面第 1 条 `:116-121`、第 2 条 `:122-127`；**核实真值 = 第 1 条 `:117-122`、第 2 条 `:123-128`**（`:116` 是键名行、`:122` 是第 1 条闭括号）。**落地文本用的是正确范围**（task-intake 新条写 `:116-141` 记整个数组、`:123-128` 记第 2 条）。**处置**：不改冻结稿（改 §2 会破「落地 == 冻结 packet」的核验基准），以本行如实登记；R2 判官自己的复核范围（`:118-121` / `:124-127`，逐键）与真值一致。
2. **第五路落为段号 ⑤、置于 ④ 超读申报之后**（未重排 ①②③④）：与 FROZEN §2 T1 改法 2 的「第五路」表述一致；重排会破仓内多处「brief 第④节」引用（如 `acceptance-author.md:25`，**不在本批 manifest 内**，不可同批改）。**代价如实记**：⑤ 段位于申报表之后，读者须读到末尾——若判官/核验官认为该位置降低可见性，可另立小批改「先 ⑤ 后 ④」并同步改引用。
3. **包面扫描排除本包自身**（`dir.name === selfId` 跳过）：⑤ 段的语义是「**上游交给我**的承接项」，自登记（`owner` / `carriedBy` 均指本包）进自己的 brief 属噪音。FROZEN 稿未规定此边界，落地判定并在此申报。
4. **解析失败不静默、也不 fail-loud**：扫描到坏 JSON 的第三方制品时，出 `> ⚠ 无法解析 <路径>（该写面本次未纳入）` 可见行，**不**阻断本包 brief 生成——理由：本包自己 refs 的解析失败（`counterparts`）才该 fail-loud，无关包的一件坏 JSON 不该拦下本包整个输入面；可见性由该行保留。
5. **`acceptance-evidence.md` 两新节插在既有节之间**（「破坏性自检」与「反例」之间），**未改任何既有节文字**；「反例」节（含 260908 批的接线守卫例）原样保留。
6. **核验期抓出的落点偏差（核验官 F1，已修）**：FROZEN §2 T3 改法 1（`PROPOSAL.md:124`）明写「未覆盖面申报」必列项**置 `residual risk` 之前**，落地首版**append 到了之后**（彼时 `dev-loop.md:248`）。核验官判 `verdict: FAIL(F1)` 并指出「同量级落点偏差〔本 §3-#2〕已申报、此条未申报，属该记未记」。**处置**：已把该 bullet 移回 `residual risk` 之前（现顺序 `:247` 未覆盖面申报 → `:248` residual risk），**内容一字未动**；本行即该条的补充申报。**教训如实记**：显式落点指令（「置 X 之前」）与「增一条」的语义在我起草期是两句话、落地时被我读成一句——**改法文本里的位置副词是承重指令，不是修辞**。

## 4. 未落 / 留根（与 FROZEN §6 一致）

- **留根①** `taskpackage-required-check.mjs` 的 `crossPackageBehaviors` 形状校验**未落**（本批已动一个 G3 脚本，不再动承重校验；触发条件见 §6）。
- **留根②** 簿记结移 `ARQ-260911-split-package-implementation-prose-gap` 至 §2 + 订正其队列行「19 个 MVP 拆包全无 `plan.md`」时点值——**随本批落地执行**（见批次簿记提交）。
- **留根③** 存量制品不追溯（34 份 review-packet / 8 存量包字段 / 2 份 workstream 件）。
- **留根④** 未并入的搭车项（known-red ③、current-pointer ①③、legacy-sxay）。
- **留根⑤** 判断型申报不机器化（`rounds/*.json` 14 字段闭集不动）。
- **留根⑥** 交日落复盘项（新包 review-packet 的该段是否真写 / 是否退化为套话；强度申报在冻结件头注的出现率）。
