# PROPOSAL · 260918-gov-review-closeout-chain-friction

> **状态：DRAFT v0.1 · 2026-09-18 · 待 human 立项 ack**（FROZEN 行待补凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果）。
> **承载 ARQ**：`ARQ-260909-index-verdict-anchor-okform-gap`（T1）、`ARQ-260916-acc-gate-archive-skew`（T2）、`ARQ-260913-current-pointer-implicit-global`（T3）、`ARQ-260908-tighten-only-unwritten`（T4）。不关的根写 §6 留根指针。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `-merge-chain-friction` / `-dispatch-chain-friction` / `-batch-drafting-friction`。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：T2 触**验收闸的枚举面**、T3 引入**新的 fail-loud 路径**（但默认关、仅显式传参时生效）；T1 为生成器补锚、T4 为成文。各项方向为**收窄 / 向后兼容 / 向严**，无放松面、无数据丢失形态）。
> **review profile**：**G1 + G3**（G1 = `docs/DEV-GOVERNANCE.md`「验收测试纪律」新增两节——非契约面，承 260907-gov-review-arq-governance-batch-2 T8「新节 + 引用行」同形态先例；G3 = `.claude/scripts/` 三个脚本）。契约面 / G2 / agent frontmatter 为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/dispatch-stamp.mjs
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把收口 / 簿记链上四处「写进去了、但状态或口径没跟上事实」的失准修齐——索引推不出 landed、验收闸数退役件、簿记写错包无声、加严纪律无成文。

**现状差距**：这四条都是**已实测确认**的失准，不是猜测：索引表里 6 条已落地批次显示成 frozen（现跑数出）；验收闸固件面枚举到的 13 件全在归档目录下、现役 0 件（现跑数出）；「当前包」指针指错时脚本零输出（3 包 retro 实证）；tighten-only 纪律在治理文档里 grep 零命中（现跑）——而它已被实际行使多次。

**非目标（本批不做什么）**：不动 `docs/contracts/**` 与系统 spec 两文件；不动任何验收冻结件（`*.acceptance.*`）；不改任何 agent frontmatter；不新增工程门（T2 只把既有集合补一个成员、T3 的新硬失败默认关）；不改 `changes-index.mjs` 的 verdict 书写要求（容忍多形态本就是它的设计）。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节全部断言均为 2026-09-18 在 `gov/kidpal` 工作树上现跑实测所得，命令与关键输出逐条留痕；所有 commit SHA 经 `git rev-parse` 核实；所有行号为本批起草时的现行值。

### 问题 1：`changes-index` 终态锚不识 `ok: true（GREEN）` 标题行，landed 推导静默不动

证据（verbatim 亲核）：`.claude/scripts/changes-index.mjs:177-178` 两条锚定义 =

```js
const VERDICT_STATUS_RE = /^\s*(?:>+\s*)?(?:\*{0,2})\s*(?:verdict|总判|总判定|结论|复验结论|判定)\s*(?:\*{0,2})\s*[:：=]\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)\b/i;
const VERDICT_HEADING_RE = /^#{1,4}\s*(?:\d{1,3}[.、]\s*)?(?:verdict|总判|总判定|结论|复验结论)\s*[:：]?\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)?\b/i;
```

两条都要求行以 `verdict|总判|结论|…` 标签起头；`VERDICT_HEADING_RE` 另要求 `#` 标题形态。**`**ok: true（GREEN）——…**` 属加粗行、既无标签前缀也非标题**，两通道皆不命中。

**活证据（起草期现跑，命令 = `cat delivery/changes/arch/INDEX.md`）**：现行 INDEX 中以下批次显示 `frozen | 已合main = yes`——**已落地却推不出 landed**：

| changeId | INDEX 状态 | 实际 |
|---|---|---|
| `260908-gov-review-merge-chain-hardening` | frozen | landed（architect-landing-summary manifest 8/8 全落） |
| `260907-gov-review-merge-friction` | frozen | landed |
| `260908-gov-review-escalation-conflict-ladder` | frozen | landed |
| `260904-gov-review-escalate-merge-approval` | frozen | landed |
| `260907-gov-review-architect-dispatch-model-override` | frozen | landed（`agent-model-guard.mjs` 在场） |
| `260830-arch-review-ui-impl-strategy` | frozen | landed |

（另有三条 `draft | yes` 同族形态。）根因：`deriveLanded` 的三通道（标签+冒号 / 编号标题 / JSON 行锚）是对既有 verdict 书写习惯的**事后归纳**，未覆盖 `ok: true（GREEN）` 这一已被使用的形态。`ARQ-260909-index-verdict-anchor-okform-gap` 已在 260909-gov-review-gate-truthfulness 合并簿记期实测记录同一现象——本批 §1 的表格是其**跨批复现**（非孤例），故按该 ARQ 候选①（补第四锚）处置。

### 问题 2：`acceptance-run-check` 枚举不跳 `archive` 退役面，且退役保留名约定无处成文

证据（verbatim 亲核）：`.claude/scripts/acceptance-run-check.mjs:34` =

```js
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo']);
```

`archive` 不在集内；同文件 `FW_ACC_RE = /\.acceptance\.(c|cpp|cc)$/`（`:31`）按扩展名形状匹配、不限深度，故 `firmware/main/test/acceptance/archive/` 下的归档 `.acceptance.c` 仍被枚举为「在盘验收文件」。

**起草期现跑实测（按闸同款 `collectFiles` 逻辑复现，含 `:46` 的 `SKIP_DIRS.has(entry) || entry.startsWith('.')` 跳过规则）**：`firmware/main/test/acceptance/` 现枚举命中 **13 件，且 13 件全部位于 `archive/` 下**——该面**现役件为 0**，即该闸固件面的 oracle 输入集当前**只剩退役件**。逐件：`char_stroke` / `device_ws_switch` / `hardware_validation` / `home_p1` / `key_route_a1` / `schedule_p2` / `schedule_reminder_b` / `screen_unification` / `session_logic` / `story_p3` / `ui_art_integration` / `v3_restore` / `voice_roundtrip_b`（各 `.acceptance.c`）；补 `archive` 进 `SKIP_DIRS` 后同口径枚举 = **0 件**。

**【硬项 1a · 数字更正申报】** `ARQ-260916-acc-gate-archive-skew` 原文记该目录下「**9 件**归档 `.acceptance.c`」，本批现跑为 **13 件**。两者不符，本批以现跑实测为准并把偏差如实登记（成因未追：或为 9-15 固件归档批之后又有 4 件于后续包并入 archive，起草期未回溯 git 历史）。**该更正不影响改法**（T2 的修法是集合补一个成员、与件数无关），但件数差 4 说明该 ARQ 的取证时点已陈旧。

**对偶闸已修而本闸未修**：`unit-run-check.mjs` 的 `SKIP_DIRS` 已于 260916 补 `archive`（ARQ-260916-unit-gate-archive-recursion 闭账 commit `4fc0969`，`git rev-parse 4fc0969` 核实存在），两闸退役口径现不同步。

**第二条缺口**：`grep -n archive docs/DEV-GOVERNANCE.md` **零命中**——「任意深度名为 `archive` 的目录 = 退役保留名」这一约定现仅存于 `unit-run-check.mjs` 与 `server/pytest.ini` 两处**代码注释**，包作者不可见；将来现役测试误落该名会静默不跑。根因：退役机制由 260916 一次性直修引入，只改了代码、未成文；acceptance 闸是对偶面漏改。

### 问题 3：`current/` 指针是隐性全局状态，经它解析的脚本不回声、错了也静默

证据（verbatim 亲核）：`.claude/scripts/dispatch-stamp.mjs:60-68` 的 `resolveChangeId()` 从 `delivery/taskpackages/current/taskpackage.json` 读 `changeId` 并返回，**全程无任何输出**；调用点 `:165` = `const changeId = resolveChangeId();` 随后直接以该 changeId 写 `pod-events.jsonl`。

**失效形态（loop-retro-2026-09-13，3 包 retro 实证）**：

1. **home-p1**——worktree bootstrap 把上一包的 `current/` 残留带入，九闸跑在错误包上下文**全绿**（docRefs 全是上一包的 id），靠顺手瞄一眼才识破（该包教训 2）。
2. **display-first-m1**——dispatch-stamp / journal-append 经 `current/` 解析 changeId，指针一天没切到本包，**簿记写进上一批的 pod-events**，还得 `git checkout` 还原再重灌（该包卡点 2）。
3. **schedule-reminder**——intake 时 `current/taskpackage.json` 投影指着上一个包，不刷就静默假绿（该包 §四）。

**根因**：`current/` 是设计上的**单指针**，但每个经它解析的调用点都把它当**可信输入**，且解析结果不可见——**错与对在输出上无差别**。

### 问题 4：tighten-only 修订纪律只活在页脚与散落先例里

证据（亲核）：`grep -n tighten docs/DEV-GOVERNANCE.md` **零命中**——「## 验收测试纪律」节现存 §1（server host/live 两态，`:268`）/ §2（probe ≠ exercise，`:279`）/ §3（冻结即冒烟，`:283`）/ §4（C 类冻结验收实编，`:287`）四小节，**无 tighten-only 条**。

而该纪律已实际行使多次：260829-schedule-reminder 包两次行使——`7c6d7c5`（acceptance-author 修自己冻结件两处 fixture/采集缺陷，断言行数 179→180 净增一条自洽断言）、`7ab2d47`（tighten-only 补两齿，`git diff --numstat` = C 档 `101/0`、py 档 `200/0`，`git diff -U0` 删除行计数 **0**）。**授权链全程靠一次性 advisory**：planner 须 human 显式要求另开 ARCH-ADVISORY 档（pod-events `0e55cfab`）才敢动冻结件。

**既成事实面（起草期现跑实测，2026-09-18）**：仓内 **20 份**验收件自带 `REVISION LOG (acceptance-author only; tighten-only discipline)` 页脚（命令 = `grep -rl "REVISION LOG (acceptance-author only" --include="*.c" --include="*.py" firmware/ server/`，输出 20 条）。分布：

- `firmware/main/test/acceptance/archive/` **12 份**——`char_stroke:190` / `device_ws_switch:78` / `hardware_validation:550` / `home_p1:214` / `key_route_a1:694` / `schedule_p2:1087` / `schedule_reminder_b:1761` / `screen_unification:151` / `story_p3:2206` / `ui_art_integration:226` / `v3_restore:139` / `voice_roundtrip_b:111`；
- `firmware/main/test/`（**现役、未归档**）**6 份**——`char_stroke:157` / `keybar_three_slot:932` / `key_layer_dispatch:910` / `p1_home_battery:1156` / `reminder_overlay_unify:1321` / `voice_round_ui:1698`；
- `server/acceptance/archive/` **2 份**——`test_char_stroke.acceptance.py:100` / `test_schedule_reminder.acceptance.py:1762`。

**【硬项 1a · 数字与引据更正申报】** `ARQ-260908-tighten-only-unwritten` 原文记「**7 份**冻结件」并给出 7 个 file:line；本批现跑核实后，**7 个引据中 3 个已漂移**（`schedule_reminder_b` 实为 `:1761` 非 `:1700`、`schedule_p2` 实为 `:1087` 非 `:979`、`test_char_stroke.acceptance.py` 实为 `:100` 非 `:95`——原文引据指向的是正文行而非页脚行），且**总数由 7 变为 20**。本批以现跑实测为准、按硬项 1a「搬运期旧引据不得免检」逐条复证。**该更正强化而非削弱本条主张**：纪律的实际使用面比记录的宽得多（含 6 份**现役**件），故成文的必要性更高。

**影响面**：① 每包遇 tighten 场景都要现付一次 advisory/escalation 成本（该包实测 = 一次 architect 派发 20,374 tokens + 一次 human 触点）；② 更坏的是**反向漂移**——无成文边界时「补齿」与「改验收」的区分只靠执行者自律，**放宽断言可以伪装成 tighten**。

## 2. 改法（T1–T4）

> **同文件落地顺序钉死**：T2 与 T4 都改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节的尾部——**T2 先落 §5，T4 后落 §6**（T4 依赖 T2 已落，同批内顺序执行）。两节互不重叠。

### T1 · `changes-index` `deriveLanded` 增第四锚：`ok: true（GREEN/PASS）` 行

承载 `ARQ-260909-index-verdict-anchor-okform-gap`。

改 `.claude/scripts/changes-index.mjs`：在 `:177-178` 两条锚旁新增第三条锚正则（第四通道），识 `ok: true（GREEN）` / `ok:true(GREEN)` / `ok: true（PASS）` 等变体，接线进 `deriveLanded()`（`:179`）与既有三通道并列。为什么这样改：直击问题 1 的根因——三通道是对既有书写习惯的事后归纳、漏了已被实际使用的形态；补锚后 §1 表中 6 条 `frozen | yes` 批次经 `--upsert` / 全量重建即翻 `landed`（**这是本 T 的可执行验收面**）。自检电池（`--self-check`，`:512`）补两夹具：`ok: true（GREEN）` → true、`ok: false（RED）` → false。**不改** verdict 模板的书写要求、不强制单一形态。

目标文件：`.claude/scripts/changes-index.mjs`（改）

### T2 · `acceptance-run-check` 枚举跳 `archive` + 退役保留名成文（§5）

承载 `ARQ-260916-acc-gate-archive-skew`。

① 改 `.claude/scripts/acceptance-run-check.mjs:34`：`SKIP_DIRS` 集合补 `'archive'`——镜像 `unit-run-check.mjs` 已于 260916 落地的同款修法，使两闸退役口径同步。自检电池补**实盘断言**（沿 unit 闸 `real-repo-no-archive-collected` 先例）：枚举命中 `archive` 路径即 FAIL，防 `SKIP_DIRS` 回退静默复发。

② 改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节**新增 §5「退役保留目录名」**：正文钉死「`archive` = 两面（firmware / server）退役保留目录名，现役测试禁落该名」+ 写明两处代码承载点（`.claude/scripts/unit-run-check.mjs` 与 `.claude/scripts/acceptance-run-check.mjs` 的 `SKIP_DIRS`、`server/pytest.ini` 的 `norecursedirs`）+ 写明「归档件按定义非现役面」的对偶语义（承 260916-unit-gate-archive-recursion）。为什么这样改：直击问题 2 的第二条缺口——约定现只活在代码注释里，包作者不可见。

目标文件：`.claude/scripts/acceptance-run-check.mjs`（改）、`docs/DEV-GOVERNANCE.md`（改：新增 §5）

### T3 · `dispatch-stamp` 回显所解析 changeId + 可选 `--expect-change-id` 硬校验

承载 `ARQ-260913-current-pointer-implicit-global`。

改 `.claude/scripts/dispatch-stamp.mjs`：

① `resolveChangeId()`（`:60`）的返回值在调用点（`:165`）**始终回显一行到 stderr**（形态如 `dispatch-stamp: resolved changeId = <id> (from delivery/taskpackages/current/taskpackage.json)`）——使解析结果从不可见变为可见。

② 新增**可选** flag `--expect-change-id <id>`：传了且与解析值不符即 **fail-loud 退出**（打印解析值与期望值两值 + 指向 `current/` 路径），不传则行为与今天**完全一致**。

为什么这样改：直击问题 3 的根因——错与对在输出上无差别；让每个经 `current/` 解析的调用点**看得见**解析值，并给调用方一个**零成本升级到硬校验**的开关。**选 `--expect-change-id` 而非无条件 fail-loud**：后者会打断跨包簿记的合法用法（如收口期为上一包补记），而前者把是否收紧留在调用方手里。**本批只改 `dispatch-stamp.mjs`**；`journal-append` / `verify.mjs` 等同族调用点只在本 T 说明里点名、不在本批改（§6 留根）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

### T4 · `DEV-GOVERNANCE` 验收测试纪律新增 §6「tighten-only 修订窗口」

承载 `ARQ-260908-tighten-only-unwritten`。

改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节，在 §5 之后新增 **§6 tighten-only 修订窗口**，正文直接采纳 advisory `0e55cfab` 里那份**防滥用五件**（逐字为据，非重写）：

1. **纯 additive 机器可判**：`git diff -U0` 删除行数 = 0；
2. **新齿必须重跑当初暴露 vacuity 的原变异**并证明转红、`sha256` 还原核销（不许另造易过变异自证）；
3. **三件记录**：冻结件 `REVISION LOG` append 带 `Reason` + `mutationEvidence` 该条 `sensitive:false→true` + 红基线摘要补一段；
4. **范围帽**：同 Pod、同既有冻结件、不新建文件；
5. **意图锚定**：新齿须钉既有 `acceptanceRef` / `decisionCoverage` 原意图；若需新 acceptance id 或改行为定义即属 re-spec、**升 human**。

**本 T 不落引用行一侧**：该 ARQ 原文落地面为「`docs/DEV-GOVERNANCE.md` 新增 §5 + acceptance-author agent def 引用行」——本 T **只落 DEV-GOVERNANCE 一侧**，引用行留 §6 留根（理由：动 agent def 触 self-def 判断点，不值得为一条引用行把本批拖进该争议；且本批 §0 已声明不动任何 frontmatter）。

为什么这样改：直击问题 4 的两条影响面——① 消灭每包现付的 advisory 成本（该纪律从此有据可依）；② 堵住「放宽断言伪装成 tighten」的反向漂移（五件里 1/2 两条是机器可判的硬闸）。

目标文件：`docs/DEV-GOVERNANCE.md`（改：新增 §6）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| arch INDEX 生成器 | `.claude/scripts/changes-index.mjs` | G3（生成器，**非门**） | T1 补第四锚。消费方 = `--upsert` / 全量重建 / `--self-check`；INDEX 非拦阻依据（文件头自陈） |
| 验收闸 | `.claude/scripts/acceptance-run-check.mjs` | G3（**枚举面改动，如实声明**） | T2① 跳 archive。消费方 = `verify.mjs` 的 acceptance 相位、`--self-check` |
| 簿记写入器 | `.claude/scripts/dispatch-stamp.mjs` | G3（**新 fail-loud 路径，如实声明**） | T3 回声 + 可选硬校验。消费方 = planner Step 9 `[DISPATCH-STAMP]` 纪律；`journal-append` / `verify.mjs` 同族**本批不改** |
| 治理规则正文 | `docs/DEV-GOVERNANCE.md` | **G1** | T2② 新增 §5 退役保留名、T4 新增 §6 tighten-only。承 260907 批 T8 同形态先例（新节非改判据） |
| 契约面 / G2 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | T2 只给既有集合补一个成员；T3 的新硬失败默认关、仅显式传参时生效 |

**【硬项 1b · sweep 透镜】** T2① 落地新枚举口径，起草期已预跑原始输出（见 §1 问题 2 的 13 件清单 + 补集后 0 件的两侧实测）。T1 的新锚**已在现行 INDEX 上预跑**——§1 表格的 6 条 `frozen | yes` 即「补锚后应翻 landed」的 census（机械枚举自 `delivery/changes/arch/INDEX.md` 逐行读取，无截断形态）。

**【硬项 1c · 复扫模式完备性先验】** 本批 T2 涉**退役语面残留清扫**。模式集显式枚举全形态：`archive`（英文词形，目录名精确匹配）× 有 / 无 backtick × 两种消费形态（`SKIP_DIRS` 集合成员 / `norecursedirs` 配置项）。**完备性论证**：`SKIP_DIRS.has(entry)` 按**目录名精确相等**匹配（`acceptance-run-check.mjs:46` = `if (SKIP_DIRS.has(entry) || entry.startsWith('.')) continue;`，`entry` 为 `readdirSync` 的单段名），故无需考虑大小写变体或路径形态——`archive` 一词即完备。T1/T3/T4 不涉退役语面。

## 4. human 取舍点（OQ）

### OQ1 · T2（验收闸枚举面）与 T3（新 fail-loud 路径）的闸 / 行为触点，走 Track B 判官判、还是升 full-adversarial + human

**背景（必须给全）**：本批两个 T 触到闸 / 脚本的**判定与行为语义**：T2 改**验收闸的枚举面**（承载 ARQ 自陈「**非门判定语义变更**——若判为门扩齿须如实升 review」）；T3 引入**新的 fail-loud 退出路径**（承载 ARQ 自陈「② 经 current/ 解析的脚本启动时回显所解析 changeId、与本包不符即 fail-loud」，方向为**向严**）。两条先例方向不一致：260907 批把 DEV-GOVERNANCE 新节按 Track B 落，`ARQ-260908-versioned-guard-version-literal-coupling` 却把「改闸判据」判为硬排除 → full。**按 AGENTS.md「存疑向严」，该判断显式交 human。**

**选项**：
- **(a) 留 Track B**（判官按 `track-b-judge-brief` 全面审；双方随时可启「声明红队」升级）。**推荐**。理由：(1) 不命中两条 Track A 判据；(2) 两处方向均为**收窄 / 向后兼容 / 向严**——T2 收窄枚举面、T3 的硬失败默认关；(3) 半径已由起草期实测照出（13→0 两侧、6 条应翻 landed 的 census）；(4) 拆包后本批闸触点已从「5 处混在一包」降到「2 处同族（都是收口期脚本）」。
- **(b) 升 full-adversarial + human**。理由：T3 的新硬失败路径如果被误用（调用方误传 `--expect-change-id`）会**打断合法簿记**，红队更稳。
- **(c) 折中（推荐备选）**：Track B，但 T3 的 `--expect-change-id` 在**落地后先只回显、不启用硬校验**，观察一段再启（把新硬失败路径拆成两个相位落地）。

### OQ2 · T4 的 §6 落点与强度

**(a) 按五件原样落 `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6**（**推荐**；承 260907 批 T8「新节 + 引用行」同形态先例）。(b) 降强度：只落「纯 additive 机器可判 + 三件记录」两条，其余三条（原变异重放 / 范围帽 / 意图锚定）留参考面——理由：五件里只有前两条是机器可判的硬闸，后三条是判断、成文后仍需人读。**(c) 落 `.claude/references/acceptance-evidence.md`（G2）而非 DEV-GOVERNANCE（G1）**——理由：G1 的挂载面更宽。

**这项为什么该 human 裁**：§6 是**新增一条适用于全部未来包的验收纪律条款**（挂载面扩张），且它同时是「给冻结件开后门」的**授权来源**——授权边界该由 human 划。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V3/V4 为 `--self-check` 电池，其新增夹具须在输出中**具名出现**（防「加了夹具但没接线」的真空绿）；V6 的重建为全量重跑，须先断言输入非空（INDEX 源目录非空）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/changes-index.mjs --self-check` | 全绿 + 新增两夹具（`ok: true（GREEN）` → true、`ok: false（RED）` → false）具名出现 |
| V2 | `node .claude/scripts/changes-index.mjs`（全量重建）+ `cat delivery/changes/arch/INDEX.md` | §1 表中 6 条 `frozen \| yes` 批次翻 `landed`；`git diff` 仅该列变化 |
| V3 | `node .claude/scripts/acceptance-run-check.mjs --self-check` | 全绿 + 新增实盘断言（`archive` 路径零枚举）具名出现 |
| V4 | 按闸同款 `collectFiles` 逻辑枚举 `firmware/main/test/acceptance/` | 补 `archive` 前实测 **13** 条命中（全在 `archive/` 下）、补后 **0** 条（前后各留一次输出） |
| V5 | `grep -c "archive" docs/DEV-GOVERNANCE.md` + `grep -c "tighten" docs/DEV-GOVERNANCE.md` | 两条均 ≥1（**原均为 0**） |
| V6 | 三路探针（**起草期已核实 `dispatch-stamp.mjs` 现无 `--self-check` 入口**，故本项不写自检、改实跑探针）：① `grep -c "resolved changeId" .claude/scripts/dispatch-stamp.mjs`；② 在 `current/` 指向某包的临时状态下跑一次真调用；③ 带 `--expect-change-id <不同值>` 再跑一次 | ① ≥1；② stderr 出现 `resolved changeId = <该包 id>`；③ **非零退出** + 打印两值；**不带该 flag 时与今日行为一致**（exit 0） |
| V7 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V8 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 4 路径） |

## 6. 留根清单

**不关的根**：

1. **T4 的引用行一侧未落**（`acceptance-author` agent def 的 References 行）——本批只落 DEV-GOVERNANCE §6；引用行需动 agent def，触 self-def 判断点，留后续（若那时该争议已定，随任一治理批补一行即可）。
2. **`journal-append.mjs` / `verify.mjs` 的 `current/` 解析点未改**——本批只改 `dispatch-stamp.mjs`（见 T3 边界）。待观察 `--expect-change-id` 的实际采用率，再决定是否推广为常设硬校验。
3. **`current/` 指针副本的行尾字节差同族面**（loop-retro-2026-09-13，screen-unification 改进 4：指针副本刷新须字节级复制、禁 Write 重打全文）——本批未承接（属 dev-loop 操作纪律，非治理脚本面）。**建议下个动 `current/` 刷新面的批承接**。
4. **`ARQ-260908-known-red-count-no-mechanical-source` 子项②③**——子项①已由 260909 T5 承接；②（evidence 齿 root 注入）落 `.claude/references/acceptance-evidence.md`，与本批 §0 非目标冲突；③（runner 报告头写执行目录）属 host 测试基建面。两项留根。
5. **拆包族另三批**（`-merge-chain-friction` / `-dispatch-chain-friction` / `-batch-drafting-friction`）——本批不承接。
6. **移出本族的两个 launch 面 ARQ**（输入清单先例取旧、launch 前置清单不核）——留 ARQ 队列后续单独处理。

**交日落复盘项**：T1 落地后在 `/loop-retro` 观察——INDEX 全量重建后**是否仍有推不出 landed 的批次**；若有，即该锚面仍有未枚举形态（本批 §1 表格的 6 条是已知集合，出现第 7 类即信号）。

**自律件**：起草期已按硬项 1a 亲核（§1 全部断言带命令与日期）；13 件清单与 6 条 frozen 表均为现跑枚举、非转引。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）