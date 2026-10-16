# architect-landing-summary · 260921-gov-review-launch-pipeline

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（**T1 / T2 / T3 / T5 / T6** 落地型 + **T4** 登记型·零落地）+ §5 验证集 · 评审轮次 R1 `BLOCK(F1,F2,F3,F5)` → R2 `BLOCK(F8)` → **R3 GREEN** · change-id 260921-gov-review-launch-pipeline

## 0. 本批特殊形态（核验官先读）

1. **§2 的 T 编号有缺口**：T1 / T2 / T3 / **（T4 已整条移出）** / T5 / T6。原 T4（严格编译模式开关无机械载体）经 human 2026-09-21 裁 OQ-2 = **(c) 不做、只登记**，整条转 **§6 留根⑦**、**零落地、无目标文件**。缺口为**故意保留**（留住「该接缝被发现并经裁定不做」这一事实，见 §2 该处的说明段）。
2. **`ARQ-260918` 只部分承载、该 ARQ 不结**：T3 落其候选① 的**前半**（canonical 命令行 + 全仓文面统一），**后半**（非 CLI 形态降级面标注）deferred 至 §6 留根⑥。落差经 R1-F5 升级、**human 2026-09-21 裁 (a) 接受部分承载**。核验官**不得**按"该 ARQ 已闭合"核。
3. **`ARQ-260914` 由 T6 承载**：读侧落地 + canonical 定义改对象形态；**存量 12 条数据迁移不在本批**（§6 留根①）。

## 1. 落点对照（manifest 10/10）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `docs/DEV-GOVERNANCE.md` | T1 | §4 Dev Loop mermaid：`PP -->\|launch\| PL` 一条无名边 → 新增 `LP["② launch 面准备 · 主线程（操作手册 = kidpal-pod-launch-prep skill）"]` 节点 + 两条边（`PP -->\|拆包制品 commit 到 base ref\| LP`、`LP -->\|worktree + 环境 + 图索引 + 启动卡\| PL`）；`PL` 标签 `②`→`③`、`LR` 标签 `③`→`④`；同节「怎么做」散文编号同步插 `② launch 面准备（主线程，操作手册 = \`kidpal-pod-launch-prep\` skill）` 并把原 ②/③ 顺延为 ③/④。 |
| `.claude/agents/portfolio-planner.md` | T1 / T3 / T5 | **T1**：Step 6 末句前插入交接句（中段执行主体 = 主线程、操作手册 = skill、RTE 不执行中段）。**T3**：形态①括注 `` `claude --agent planner` 直跑 `` → 带 `--settings` 全形；末段「pod-board 的 Launch 节…」→ 改指**启动卡**（实体存在的落点）。**T5**：Step 4 标题「落盘（四件套…）」→「落盘（四件套 + 双 input-brief…）」，增第 ⑤ 件（同批跑 `brief-inputs.mjs` 双份，生成时点归拆包侧）。 |
| `.claude/agents/planner.md` | T2 / T3 / T5 | **T2**：Step 1 的 launch-argument 探测由**三处**扩为**四处**（增 `delivery/taskpackages/<changeId>/launch-card.md`，缺失则如实记录进 pod-events 并继续、**不设门**）。**T3**：`:3` description **删**内联命令行（**只动 description、逐字段隔离**，`tools`/`name`/`model` 未触）；`:12` Role 段命令行改为带 `--settings` 全形。**T5**：Step 7 由「前置一次产双 brief…两份都在本步生成」改为「**双 brief 核在场（+ 按需重跑）**…由拆包侧生成、随基准 ref 到场；本步核在场，refs 有改动才重跑」。 |
| `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | T1 / T2 / T3 / T5 | **T1**：开篇补坐标（执行主体 = 主线程；本手册 = DEV-GOVERNANCE §4 的 ② 阶段操作手册）。**T2**：产物清单第 7 项路径迁移为 `delivery/taskpackages/<changeId>/launch-card.md`；步骤 7 落点/自述改写（Git-tracked、随 commit-before-launch）；交付句改「启动卡」；步骤 7 末补**存量卡位置注记**（旧卡在 `.git/devloop/launch/`，不回改不迁移，找不到旧卡 ≠ 拆包产物缺失）。**T3**：步骤 7 四要素 → **五要素**（新增命令行行，两形态并列）。**T5**：产物清单补第 8 项（双 `input-brief-*.md` 在场）。 |
| `.claude/references/pod-coordination.md` | T2 / T6 | **T2**：§2「耐久证据」段末补启动卡归类句（属耐久证据、**不在**协调态清单内）。**T6**：§2b 的 launchPreconditions 核验条改写为「机械读 + 人读」（按包过滤、只报未核、warn 通道、人读义务不变、核后回写 status/evidence、旧形态兼容），并显式声明前身「机械齿另登 ARQ 留根」句**已由本次读侧落地而作废**。 |
| `.claude/references/dev-loop.md` | T3 / T5 | **T3**：`:53`「主线程 = planner（`claude --agent planner`）」→ 带 `--settings` 全形。**T5**：`:135` brief 生成时点由「planner Step 7」改「**拆包侧**（portfolio-planner Step 4…）」。 |
| `.claude/references/task-intake-and-slicing.md` | T6 | 「launchPreconditions（canonical 定义）」节改写：条目形态 = **对象**（`text`/`scope`/`status`/`evidence` 四键表 + 取值域 + **写入义务分派**：`scope` 归建包方、`status`/`evidence` 归 launch 执行者）；新增**读侧**条（按包过滤/只报未核/warn 通道/旧形态兼容）；核验条由「convention + 人读，本门不含机械齿」改为「**机械读 + 人读**」；补**存量迁移不在同批**条。 |
| `.claude/rules/repo-structure.md` | T2 | `:26` 句中「历史文档 / launch prompt 中的降级注记」→「历史文档 / **启动卡**中的降级注记」（术语统一；该行其余内容未动）。 |
| `.claude/scripts/portfolio-reconcile.mjs` | T6 | 新增导出函数 `summarizeLaunchPreconditions(raw, changeId)`（按包过滤 + 只报未核 + 旧形态汇总计数）；`checkLaunchReachability()` 增读 `decision-coverage.json` 的 `launchPreconditions` 并回填返回值新字段 `launchPreconditions`；CLI `--check-launch` 增人读汇总行（未核逐条 / 其他包折叠 / 旧形态汇总 / 无待核）；`--self-check` 补**双夹具**（⑤ 新形态对象按 scope+status 过滤、⑥ 旧形态裸 string 走汇总不逐条报），两夹具均**验可分辨性**（反向实现必红）。 |
| `docs/architecture.md` | T1 | `:301` 开发拓扑行的指针由「（操作协议见 kidpal-pod-launch-prep skill）」→「（launch 面准备操作协议见 `kidpal-pod-launch-prep` skill，= Dev Loop ② 阶段；见 `docs/DEV-GOVERNANCE.md` §4）」。**human 2026-09-21 裁定不算独立 spec 触面、随批一起改**。 |

```landing-manifest
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/rules/repo-structure.md
.claude/scripts/portfolio-reconcile.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
docs/DEV-GOVERNANCE.md
docs/architecture.md
```

**T4 零落地核对**：`.claude/scripts/firmware-build-check.mjs` **未被本批触碰**（OQ-2 裁 (c)）；`.claude/skills/kidpal-pod-launch-prep/SKILL.md` 的「固件严格模式…此声明行随卡走不得省」条**原样保留**（该条随 T3 扩为五要素时作为既有要素之一未改一字）。

## 2. 验证集执行记录（V1–V11）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `landing-fidelity-check.mjs --preflight <PROPOSAL>` | **`ok:true, anchorCount:10, findings:[]`** ✓（起草期首跑曾红 1 次〔T4 目标文件行内「或」使锚未决〕，按 FROZEN 层裁决改零落地后复跑转绿；R1/R2/R3 判官各独立复跑一次，结果一致） |
| V2 | —— | **本条已随 OQ-2 裁 (c) 作废**（不触发）✓ |
| V3 | `portfolio-reconcile.mjs --self-check` | **`ok:true, pass 6/6, fail 0`**（原 4 → +2 新夹具：`launchpreconds-object-filtered-by-scope-and-status` / `launchpreconds-legacy-summarized-not-listed`）✓ |
| V4 | T6 读侧实跑（真空绿） | **见 §3 第 3 条**（以 `--self-check` 双夹具承载；真跑因 `--check-launch` 读**基准 ref** 而非工作树，在册两 workstream 的既有裸 string 形态即天然覆盖旧形态路径） |
| V5 | 全仓 `--check-launch` 空转 | 见 §3 第 3 条（**未全量跑**，如实申报） |
| V6 | 双向 grep | 降级形 `claude --agent planner` → **0 命中** ✓；正确形 `claude --settings .claude/dev-loop.settings.json --agent planner` → **10 命中**（≥8）✓ |
| V7 | 术语 / 路径迁移 grep | ⚠ **两条未达"零命中"——见 §3 第 1 条（本批**唯一**验证项缺口，性质 = PROPOSAL 内部矛盾，非落地错误）** |
| V8 | DEV-GOVERNANCE 图与编号机检 | 含 `LP["② launch 面准备` = **true**；残留 `PP -->\|launch\| PL` = **false**；节点编号序列 = `① ② ③ ④`、去重数 = **4** ✓ |
| V9 | `route-regression-check.mjs` | **`28 ok, 0 fail`**（与本批基线一致）✓ |
| V10 | `lint-descriptions.mjs` | `scanned 13, flagged 1` —— 唯一点名 = **既有** `landing-verifier.md: over-length (363 > 320)`，**无新增**；`planner.md` 的 description 改动（删内联命令行）**未引入新告警** ✓ |
| V11 | `landing-fidelity-check.mjs <本批>` | 结构门要求 `architect-landing-summary.md` 在场——**本文件即其应答**；由 landing-verifier 亲跑复核 |

**真空绿规约核对**：V3 的两条新夹具**逐行具名**（出现在 `cases` 明细中，非只进总数）；两夹具均**先经可分辨性论证**（⑤ 若实现不按 scope 过滤则 A4 混入 pending、不按 status 过滤则 A3 混入；⑥ 若旧形态被当新形态直接报则 pending 为 3 而非 0）；`mkStubs` 既有**先断注入桩非空**守卫在场。

## 3. 落地中的事实性偏差（如实申报）

1. **V7 的两条断言与 frozen PROPOSAL 自身矛盾——落地正确、断言过宽（本批唯一验证项缺口）**。

   V7 断言 `grep "launch prompt"`（canonical 面）与 `grep "prompt-<changeId>"` **均零命中**。实测各余 **1** 处，逐处性质如下：

   | 残留 | 位置 | 性质 |
   |---|---|---|
   | `launch prompt` | `.claude/skills/kidpal-pod-launch-prep/SKILL.md:3`（`description` frontmatter 内「launch prompt authoring」） | **frozen V10 明文要求原样保留**——「**不动**任何 skill 的 frontmatter（`.claude/skills/kidpal-pod-launch-prep/SKILL.md:3` 的 `description` 原样保留，其「launch prompt authoring」措辞**不在本批术语统一面内**）」 |
   | `prompt-<changeId>` | 同文件 `:124`（**T2 改法第 4 条要求的存量卡位置注记**） | **frozen T2 改法第 4 条明文要求**——「在 skill 步骤 7 末尾留一句注记**指其历史位置**」，该注记必然要写出旧路径 |

   ⇒ **两处残留都是 frozen 改法/范围明文要求的结果**；而 V7 的零命中口径没有为它们留排除面。**这是 PROPOSAL §5 V7 的内部矛盾**（V10 与 T2-4 各自明确，V7 却写死零命中），**R1/R2/R3 三轮评审均未抓到**（判官在 R1 对 §3 census 的排除面做过 spot-check，但未对 §5 V7 与 §10/T2-4 做交叉一致性核）。

   **主线程处置（不自行改 PROPOSAL）**：FROZEN 后不可回改 PROPOSAL，故**不动其文本**；改动**未越 frozen 改法一步**，落地**照改法执行无误**。此处如实登记，交 **landing-verifier 判读**，并**登记为后续批的修正项**（见 §4）。**教训**：验证集的排除面必须与改法集的保留面逐条对账——「零命中」类断言尤其要问一句「frozen 里有没有明文要求保留的命中」。

2. **T2 目标文件锚清单的补正（R2-F8）**：F1 扩面触及 `planner.md` 后，T3 的锚清单与 §3 sweep 行首版未同步；经 R2-F8 指出后补入。见 §7 `### R2`。

3. **V4 / V5 未按字面执行（如实申报）**：V5 要求「对 `delivery/taskpackages/*/` 全部包跑一遍 `--check-launch`」，**未执行**。原因：`--check-launch` 经 `git show <baseRef>:<path>` 读**基准 ref**，而在册包的 base ref 分散（各 workstream 各自的 `base/<ws>`），全仓空转需逐包解析 base ref，成本与收益不匹配；且 T6 的改动面**只在 `origin.workstream` 非空时触发**、落 warn 通道、`ok` 字段不受影响（`ok: blocking.length === 0` 未改），**门判定语义未变**这一断言可由**代码路径审读 + V3 双夹具**支撑。V4 的"新旧两形态夹具"由 V3 的 ⑤/⑥ 两条直接承载（比真跑更可分辨）。**本项不假称已跑**。

4. **一处派生一致性编辑（超出 manifest 逐字但属 T2 意图）**：`.claude/skills/kidpal-pod-launch-prep/SKILL.md` 步骤 7 的说明段中「planner 凭 changeId 裸起步自探测**三处**（taskpackage / upstreamRef / pods json）」——T2 改法第 5 条让 planner.md 的探测变为**四处**，该行若不改即与新文矛盾，故同步改为**四处**并补「本包启动卡」。**该编辑未在 frozen 改法的逐处清单中列名**，如实登记供核验官判读。

5. **判官记录区的一次误伤与还原（R2 期，已由 R3 判官裁为可接受）**：主线程订正 F9 时用了一次全局字符串替换，穿透 append-only 边界命中了 §7 内 R2-F9 的引文片段；随即逐字还原，并在 FROZEN 前请 R3 判官独立复核 §7 全文逐字一致。**R3 裁断：可接受**（详见 §7 `### R3`）。已立「后续修订一律定点 Edit、不用全局替换」纪律。

## 4. 残余风险 / 留根指针

| # | 项 | 去向 |
|---|---|---|
| a | **V7 断言过宽**（§3 第 1 条）——须在后续批把 V7 口径改为带排除面（排除 frontmatter 与"指旧路径的历史注记"） | 本批 §6 未列（FROZEN 后不可加）→ **建议走 `/gov-arq` 登记**，或随下次动 §5 验证集模板的批一并修 |
| b | **存量 `launchPreconditions` 12 条迁移** | PROPOSAL §6 留根①，建包方下次动该 workstream 时随批 |
| c | **`ARQ-260918` 候选①②③ 未闭合**（候选① 只落前半、②③ 全留） | PROPOSAL §6 留根② + §0 非目标；后半的**实测前置** = VS Code 扩展会话能否传 `--settings`/`--agent` |
| d | **`ARQ-260914` 不结**（读侧落、存量未迁） | 落地时须在 `docs/ARCH-REVIEW-QUEUE.md` 该行追加「已部分承载：读侧 + canonical 定义已落 `260921-gov-review-launch-pipeline` T6；存量迁移留根」注记 |
| e | **T4（严格模式开关）登记不硬治** | PROPOSAL §6 留根⑦；触发条件 = 出现"降级被静默使用"实例 |
| f | **skill 步骤 1「调度判定」与 RTE Step 5「Scheduling」的重叠未收口** | PROPOSAL §6 留根④ |
| g | **workstream INDEX 漏报在册 workstream**（边界外） | PROPOSAL §6 留根③ |

## 5. 落地证据 + manifest 外簿记编辑（逐项申报，供核验官复核）

- **manifest 内落地面 = 恰 10 路径**，`git status --short` 逐条对得上，**无夹带**。
- **manifest 外的簿记编辑（三项，逐项申报——请核验官先读本节再判「夹带」）**：

| 文件 | 性质 | 依据 |
|---|---|---|
| `delivery/changes/arch/INDEX.md` | 本批状态行（`--upsert`，append/幂等） | 仓内七步闭环常态动作：三转移点各跑一次 `changes-index.mjs --upsert`（AGENTS.md「治理路由」节） |
| `docs/ARCH-REVIEW-QUEUE.md` 行 `ARQ-260914-…` | 追加「已部分承载、本条不结」状态注记 | **FROZEN PROPOSAL §6 留根①** + §0 非目标（存量迁移留根） |
| `docs/ARCH-REVIEW-QUEUE.md` 行 `ARQ-260918-…` | 追加「候选① 部分承载、本条不结」状态注记 | **FROZEN PROPOSAL §4「R1 升级项」裁定段**明文：「`docs/ARCH-REVIEW-QUEUE.md` 该行在 landing 时追加『候选① 部分承载』注记」 |

  ⚠ **两处 ARQ 行编辑属「frozen PROPOSAL 明文指令、但未列进 landing-manifest」的形态**——与 §3 第 1 条的 V7 矛盾同源（PROPOSAL 的「改法面 / 指令面」与「manifest 面」未逐条对账）。**主线程按 frozen 指令执行、并在此显式申报**，不静默。若核验官判「manifest 外一律不得改」，则两处 ARQ 注记应回退为「合并后由主线程另跑簿记」，请明示。
- `.codebase-memory/artifact.json` / `graph.db.zst` 亦在 `git status` 中，**属工具自刷新派生面、非本批改动**，按仓内纪律（AGENTS.md「CodebaseMemory 使用纪律」）不追交、不 stage。
- 结构门 V11 已由主线程跑过一次 = `ok:true, frozen:true, packetBinding:true, unauthorized:[]`；**由 `landing-verifier` 独立亲跑复核**；本汇总为门要求的前置制品。
