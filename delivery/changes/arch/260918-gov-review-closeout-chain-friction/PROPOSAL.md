# PROPOSAL · 260918-gov-review-closeout-chain-friction

> **状态：DRAFT v0.7 · 2026-09-19 · 已立项 ack · 待 architect R3 确认**（凭据：human 2026-09-18 立项 ack + OQ 全裁；v0.5 按 human 2026-09-19 裁定移出 T1〔索引终态锚〕；R1 BLOCK(F1..F6) → v0.6 修订 → R2 判 F1–F6 全到位、新出 F7+N1–N3 → **v0.7 逐条收口**〔F7 危害定性回收 / N1 OQ 口径更正 / N2 计数口径 / N3 补第四路探针〕。FROZEN 行待补 R3 GREEN 凭据）。
> **承载 ARQ**：`ARQ-260916-acc-gate-archive-skew`（T1）、`ARQ-260913-current-pointer-implicit-global`（T2）、`ARQ-260908-tighten-only-unwritten`（T3）。**原承载的 `ARQ-260909-index-verdict-anchor-okform-gap` 已移出本批、退回队列**（该 ARQ 的处方经起草期实测证伪，见 §6 留根①）。不关的根写 §6 留根指针。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `-merge-chain-friction`（已 FROZEN v1.0）/ `-dispatch-chain-friction` / `-batch-drafting-friction`。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：T1 触**验收闸的枚举面**、T2 引入**新的 fail-loud 路径**（默认关、仅显式传参生效）；T3 为成文。各项方向为**收窄 / 向后兼容 / 向严**，无放松面、无数据丢失形态。T1〔索引锚〕移出后本批已不含生成器面改动）。
> **review profile**：**G1 + G3**（G1 = `docs/DEV-GOVERNANCE.md`「验收测试纪律」新增两节——非契约面，承 260907-gov-review-arq-governance-batch-2 T8「新节 + 引用行」同形态先例；G3 = `.claude/scripts/` 两个脚本）。契约面 / G2 / agent frontmatter 为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/dispatch-stamp.mjs
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把收口 / 簿记链上三处「写进去了、但状态或口径没跟上事实」的失准修齐——验收闸混数退役件、簿记写错包无声、加严纪律无成文。

**现状差距**：这三条都是**已实测确认**的失准：验收闸固件面枚举 **20 件、其中 13 件是归档退役件**（真跑闸 + 逐件拆分核实）；「当前包」指针指错时脚本零输出（3 包 retro 实证）；tighten-only 纪律在治理文档里 grep 零命中（现跑）——而它已被实际行使多次（**实测 20 份验收件带其页脚**，含 6 份现役）。

**非目标（本批不做什么）**：不动 `docs/contracts/**` 与系统 spec 两文件；不动任何验收冻结件（`*.acceptance.*`）；不改任何 agent frontmatter；不新增工程门（T1 只把既有集合补一个成员、T2 的新硬失败默认关）；**不动 `changes-index.mjs`**（T1〔索引终态锚〕已移出本批）。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节全部断言均为 2026-09-18/19 在 `gov/kidpal` 工作树上现跑实测所得，命令与关键输出逐条留痕；所有 commit SHA 经 `git rev-parse` 核实；所有行号为本批起草时的现行值。

### 问题 1：`acceptance-run-check` 枚举不跳 `archive` 退役面，且退役保留名约定无处成文

证据（verbatim 亲核）：`.claude/scripts/acceptance-run-check.mjs:34` =

```js
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo']);
```

`archive` 不在集内；同文件 `FW_ACC_RE = /\.acceptance\.(c|cpp|cc)$/`（`:31`）按扩展名形状匹配、不限深度。**闸的枚举根是 `firmware/`**（`:137` = `const fwDir = join(root, 'firmware')`），不是某个子目录——故 `firmware/main/test/acceptance/archive/` 下的归档件与 `firmware/main/test/` 下的现役件**同属该闸的枚举面**。

**起草期现跑实测（两条口径并列，勿混）**：

- **闸口径**（真跑 `node .claude/scripts/acceptance-run-check.mjs`，输出 verbatim）：`"surfaces": { "firmwareAcceptanceFiles": 20, "serverAcceptanceFiles": 0 }`——**20 件**。
- **该 20 件的构成**（按闸同款 `collectFiles` 逻辑复现，含 `:46` 的 `SKIP_DIRS.has(entry) || entry.startsWith('.')` 跳过规则）：**归档 13 件**（全在 `firmware/main/test/acceptance/archive/` 下）+ **现役 7 件**（全在 `firmware/main/test/` 下）。

  - 归档 13 件：`char_stroke` / `device_ws_switch` / `hardware_validation` / `home_p1` / `key_route_a1` / `schedule_p2` / `schedule_reminder_b` / `screen_unification` / `session_logic` / `story_p3` / `ui_art_integration` / `v3_restore` / `voice_roundtrip_b`（各 `.acceptance.c`）；
  - 现役 7 件：`char_stroke.acceptance.c` / `host_infra_smoke.acceptance.c` / `keybar_three_slot.acceptance.c` / `key_layer_dispatch.acceptance.c` / `p1_home_battery.acceptance.c` / `reminder_overlay_unify.acceptance.c` / `voice_round_ui.acceptance.c`。
- **补 `archive` 进 `SKIP_DIRS` 后**：闸口径 **20 → 7**（只跳退役面，7 件现役件全部保留）。

**【硬项 1a · 口径更正申报（R1-F1，主线程起草期失误）】** v0.5 曾把「`firmware/main/test/acceptance/` 子目录下 13 件全为归档、现役 0 件」**误升格为闸口径**，据此写出「该闸固件面的 oracle 输入集当前只剩退役件」「V1 补后 0 条」两处失准断言。**根因 = 起草期只扫了一个子目录、未核对闸的枚举根（`:137` = `firmware/`）**。R1 判官真跑闸抓到（`:206` `firmwareAcceptanceFiles: 20`），本版按实况更正：**该闸的固件面 7 件现役件一直在枚举集内，退役件只是其中 13/20**。**改法不受影响**（T1 仍是「集合补一个成员」，与件数无关），但**危害定性须据实回收**（R2-F7 采纳）：v0.6 前半句「按实况更正：7 件现役件一直在枚举集内、退役件只是其中 13/20」成立；**后半句「退役面上的断言失败会被计入现役验收结果」机制不成立、已删**——该闸固件面是**结构闸**（`fwAccFiles` 只喂 `decide()` 的**存在性判定**与 `surfaces` **计数**，见 `:85`/`:89-90`/`:206`，**不执行任何断言**），而 host_app runner 用**显式清单**（`firmware/main/test/host_app/main/CMakeLists.txt` 恰列那 7 件现役件、:`7` 注释并明言归档件「不再编入」）——**退役件无通道进入现役结果**。**据实回收为 ARQ-260916 原口径**：「退役件混入 oracle 输入集与清点统计（**虚高 2.85 倍**）；**将来 runner 判据变更 / 撤除时退役件会独立触发结构红**」（后者即 `:89-90` 的 `has-acceptance-files-no-runner`）。V1 亦据实改为**双探针**（子目录口径 13→0 + 闸口径 20→7），避免与 V2 自检输出的 `fwAcc=7` 互相矛盾。

**【硬项 1a · ARQ 数字更正申报】** `ARQ-260916-acc-gate-archive-skew` 原文记该目录下「**9 件**归档 `.acceptance.c`」，本批现跑为 **13 件**。两者不符，本批以现跑实测为准并把偏差如实登记（成因未追：或为 9-15 固件归档批之后又有 4 件于后续包并入 archive，起草期未回溯 git 历史）。**该更正不影响改法**（T1 的修法是集合补一个成员、与件数无关），但件数差 4 说明该 ARQ 的取证时点已陈旧。

**对偶闸已修而本闸未修**：`unit-run-check.mjs` 的 `SKIP_DIRS` 已于 260916 补 `archive`（ARQ-260916-unit-gate-archive-recursion 闭账 commit `4fc0969`，`git rev-parse 4fc0969` 核实存在；`:38` 现含 `'archive'` 亲核确认），两闸退役口径现不同步。

**第二条缺口**：`grep -n archive docs/DEV-GOVERNANCE.md` **零命中**——「任意深度名为 `archive` 的目录 = 退役保留名」这一约定现仅存于 `unit-run-check.mjs` 与 `server/pytest.ini` 两处**代码注释**，包作者不可见；将来现役测试误落该名会静默不跑。根因：退役机制由 260916 一次性直修引入，只改了代码、未成文；acceptance 闸是对偶面漏改。

### 问题 2：`current/` 指针是隐性全局状态，经它解析的脚本不回声、错了也静默

证据（verbatim 亲核）：`.claude/scripts/dispatch-stamp.mjs:60-68` 的 `resolveChangeId()` 从 `delivery/taskpackages/current/taskpackage.json` 读 `changeId` 并返回，**全程无任何输出**；调用点 `:165` = `const changeId = resolveChangeId();` 随后直接以该 changeId 写 `pod-events.jsonl`。

**失效形态（loop-retro-2026-09-13，3 包 retro 实证）**：

1. **home-p1**——worktree bootstrap 把上一包的 `current/` 残留带入，九闸跑在错误包上下文**全绿**（docRefs 全是上一包的 id），靠顺手瞄一眼才识破（该包教训 2）。
2. **display-first-m1**——dispatch-stamp / journal-append 经 `current/` 解析 changeId，指针一天没切到本包，**簿记写进上一批的 pod-events**，还得 `git checkout` 还原再重灌（该包卡点 2）。
3. **schedule-reminder**——intake 时 `current/taskpackage.json` 投影指着上一个包，不刷就静默假绿（该包 §四）。

**根因**：`current/` 是设计上的**单指针**，但每个经它解析的调用点都把它当**可信输入**，且解析结果不可见——**错与对在输出上无差别**。

### 问题 3：tighten-only 修订纪律只活在页脚与散落先例里

证据（亲核）：`grep -n tighten docs/DEV-GOVERNANCE.md` **零命中**——「## 验收测试纪律」节现存 §1（server host/live 两态，`:268`）/ §2（probe ≠ exercise，`:279`）/ §3（冻结即冒烟，`:283`）/ §4（C 类冻结验收实编，`:287`）四小节，**无 tighten-only 条**。

而该纪律已实际行使多次：260829-schedule-reminder 包两次行使——`7c6d7c5`（acceptance-author 修自己冻结件两处 fixture/采集缺陷，断言行数 179→180 净增一条自洽断言）、`7ab2d47`（tighten-only 补两齿，`git diff --numstat` = C 档 `101/0`、py 档 `200/0`，`git diff -U0` 删除行计数 **0**）。**授权链全程靠一次性 advisory**：planner 须 human 显式要求另开 ARCH-ADVISORY 档（pod-events `0e55cfab`）才敢动冻结件。

**既成事实面（起草期现跑实测，2026-09-18）**：仓内 **20 份**验收件自带 `REVISION LOG (acceptance-author only; tighten-only discipline)` 页脚（命令 = `grep -rl "REVISION LOG (acceptance-author only" --include="*.c" --include="*.py" firmware/ server/`，输出 20 条）。分布：

- `firmware/main/test/acceptance/archive/` **12 份**——`char_stroke:190` / `device_ws_switch:78` / `hardware_validation:550` / `home_p1:214` / `key_route_a1:694` / `schedule_p2:1087` / `schedule_reminder_b:1761` / `screen_unification:151` / `story_p3:2206` / `ui_art_integration:226` / `v3_restore:139` / `voice_roundtrip_b:111`；
- `firmware/main/test/`（**现役、未归档**）**6 份**——`char_stroke:157` / `keybar_three_slot:932` / `key_layer_dispatch:910` / `p1_home_battery:1156` / `reminder_overlay_unify:1321` / `voice_round_ui:1698`；
- `server/acceptance/archive/` **2 份**——`test_char_stroke.acceptance.py:100` / `test_schedule_reminder.acceptance.py:1762`。

**【硬项 1a · 数字与引据更正申报】** `ARQ-260908-tighten-only-unwritten` 原文记「**7 份**冻结件」并给出 7 个 file:line；本批现跑核实后，**7 个引据中 3 个已漂移**（`schedule_reminder_b` 实为 `:1761` 非 `:1700`、`schedule_p2` 实为 `:1087` 非 `:979`、`test_char_stroke.acceptance.py` 实为 `:100` 非 `:95`——原文引据指向的是正文行而非页脚行），且**总数由 7 变为 20**。本批以现跑实测为准、按硬项 1a「搬运期旧引据不得免检」逐条复证。**该更正强化而非削弱本条主张**：纪律的实际使用面比记录的宽得多（含 6 份**现役**件），故成文的必要性更高。

**影响面**：① 每包遇 tighten 场景都要现付一次 advisory/escalation 成本（该包实测 = 一次 architect 派发 20,374 tokens + 一次 human 触点）；② 更坏的是**反向漂移**——无成文边界时「补齿」与「改验收」的区分只靠执行者自律，**放宽断言可以伪装成 tighten**。

## 2. 改法（T1–T3）

> **同文件落地顺序钉死**：T1 与 T3 都改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节的尾部——**T1 先落 §5，T3 后落 §6**（T3 依赖 T1 已落，同批内顺序执行）。两节互不重叠。

### T1 · `acceptance-run-check` 枚举跳 `archive` + 退役保留名成文（§5）

承载 `ARQ-260916-acc-gate-archive-skew`。

① 改 `.claude/scripts/acceptance-run-check.mjs:34`：`SKIP_DIRS` 集合补 `'archive'`——镜像 `unit-run-check.mjs` 已于 260916 落地的同款修法（该闸 `SKIP_DIRS` 现含 archive，commit `4fc0969`），使两闸退役口径同步。自检电池补**实盘断言**（沿 unit 闸 `real-repo-no-archive-collected` 先例）：枚举命中 `archive` 路径即 FAIL，防 `SKIP_DIRS` 回退静默复发。

② 改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节**新增 §5「退役保留目录名」**：正文钉死「`archive` = 两面（firmware / server）退役保留目录名，现役测试禁落该名」+ 写明两处代码承载点（`.claude/scripts/unit-run-check.mjs` 与 `.claude/scripts/acceptance-run-check.mjs` 的 `SKIP_DIRS`、`server/pytest.ini` 的 `norecursedirs`）+ 写明「归档件按定义非现役面」的对偶语义（承 260916-unit-gate-archive-recursion）。为什么这样改：直击问题 1 的第二条缺口——约定现只活在代码注释里，包作者不可见。

目标文件：`.claude/scripts/acceptance-run-check.mjs`（改）、`docs/DEV-GOVERNANCE.md`（改：新增 §5）

### T2 · `dispatch-stamp` 回显所解析 changeId + 可选 `--expect-change-id` 硬校验

承载 `ARQ-260913-current-pointer-implicit-global`。

改 `.claude/scripts/dispatch-stamp.mjs`：

① `resolveChangeId()`（`:60-72`）的返回值在调用点（`:165`）**始终回显一行到 stderr**——**回显须如实反映本次实际来源**（R1-F6 采纳）：该函数有两条路径，`--task-package-id <id>` 显式给出时（`:61-62`）来源 = 该 flag、否则读 `current/taskpackage.json`（`:64-68`）；故形态为 `dispatch-stamp: resolved changeId = <id> (from --task-package-id)` 或 `... (from delivery/taskpackages/current/taskpackage.json)` **二选一**，不得固定写后者。② 新增**可选** flag `--expect-change-id <id>`：传了且与解析值不符即 **fail-loud 退出**（打印解析值与期望值两值 + 指向来源），不传则行为与今天**完全一致**。**并显式定义与既有「无上下文 exit 0」的交互**（R1-F6 采纳）：`:167-169` 现为 `if (!tpDir || !changeId) { … process.exit(0) }`——本 T 规定 **`--expect-change-id` 在场而无上下文（解析值 = null）时亦 fail-loud 退出**（原因：调用方明确表达了「本包应在场」的期望，静默 exit 0 正是本问题要治的「错与对无差别」形态）；**不传该 flag 时维持现有 exit 0 不变**。

为什么这样改：直击问题 2 的根因——错与对在输出上无差别；让每个经 `current/` 解析的调用点**看得见**解析值，并给调用方一个**零成本升级到硬校验**的开关。**选 `--expect-change-id` 而非无条件 fail-loud**：后者会打断跨包簿记的合法用法（如收口期为上一包补记），而前者把是否收紧留在调用方手里。**本批只改 `dispatch-stamp.mjs`**；`journal-append` / `verify.mjs` 等同族调用点只在本 T 说明里点名、不在本批改（§6 留根）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

### T3 · `DEV-GOVERNANCE` 验收测试纪律新增 §6「tighten-only 修订窗口」

承载 `ARQ-260908-tighten-only-unwritten`。

改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节，在 §5 之后新增 **§6 tighten-only 修订窗口**，正文按 **`docs/ARCH-REVIEW-QUEUE.md` 的 `ARQ-260908-tighten-only-unwritten` 行**所载「防滥用五件」落地。**溯源更正（R1-F4 采纳）**：该 ARQ 行自陈其来源为 architect advisory `0e55cfab`，但本批落的是**该行已操作化的形态**——它相对 advisory 原文含三处向严增补，**不等于 advisory 原文的连排条款**；v0.5 原写「直接采纳 advisory 里那份（逐字为据）」属溯源错标，本版更正为「以 ARQ 行的操作化形态为准、advisory 为上游出处」。human 2026-09-18 OQ 全裁采纳完整五件。

1. **纯 additive 机器可判**：`git diff -U0` 删除行数 = 0；
2. **新齿必须重跑当初暴露 vacuity 的原变异**并证明转红、`sha256` 还原核销（不许另造易过变异自证）。**核销证据落点成文（R1-F5 采纳）**：`sha256` 记入**冻结件 `REVISION LOG` 同一条**（与第 3 条三件记录同址），格式 `<前 sha256> → <后 sha256>`。**如实声明**：本仓现**无已执行的 `sha256` 核销先例**（`7ab2d47` 仅留行数计数、未记 hash），故本条落地后为**首例**、其工具面（是否需脚本辅助生成/比对）留 §6 留根⑧；
3. **三件记录**：冻结件 `REVISION LOG` append 带 `Reason` + `mutationEvidence` 该条 `sensitive:false→true` + 红基线摘要补一段；
4. **范围帽**：同 Pod、同既有冻结件、不新建文件；
5. **意图锚定**：新齿须钉既有 `acceptanceRef` / `decisionCoverage` 原意图；若需新 acceptance id 或改行为定义即属 re-spec、**升 human**。

**本 T 不落引用行一侧**：该 ARQ 原文落地面为「`docs/DEV-GOVERNANCE.md` 新增 §5 + acceptance-author agent def 引用行」——本 T **只落 DEV-GOVERNANCE 一侧**，引用行留 §6 留根（理由：动 agent def 触 self-def 判断点，不值得为一条引用行把本批拖进该争议；且本批 §0 已声明不动任何 frontmatter）。

为什么这样改：直击问题 3 的两条影响面——① 消灭每包现付的 advisory 成本（该纪律从此有据可依，**使用面实测 20 份**）；② 堵住「放宽断言伪装成 tighten」的反向漂移（五件里 1/2 两条是机器可判的硬闸）。human 明确该条同时是「给冻结件开后门」的**授权边界**，故五件完整落、不降强度。

目标文件：`docs/DEV-GOVERNANCE.md`（改：新增 §6）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 验收闸 | `.claude/scripts/acceptance-run-check.mjs` | G3（**枚举面改动，如实声明**） | T1① 跳 archive。消费方 = `verify.mjs` 的 acceptance 相位、`--self-check` |
| 簿记写入器 | `.claude/scripts/dispatch-stamp.mjs` | G3（**新 fail-loud 路径，如实声明**） | T2 回声 + 可选硬校验。消费方 = planner Step 9 `[DISPATCH-STAMP]` 纪律；`journal-append` / `verify.mjs` 同族**本批不改** |
| 治理规则正文 | `docs/DEV-GOVERNANCE.md` | **G1** | T1② 新增 §5 退役保留名、T3 新增 §6 tighten-only。承 260907 批 T8 同形态先例（新节非改判据） |
| 契约面 / G2 / agent frontmatter | — | **未触** | 零改动 |
| 索引生成器 | `.claude/scripts/changes-index.mjs` | **未触**（v0.5 移出） | 原 T1〔索引终态锚〕已移出本批，见 §6 留根① |
| 工程门新增 | — | **零新增** | T1 只给既有集合补一个成员；T2 的新硬失败默认关、仅显式传参时生效 |

**【硬项 1b · sweep 透镜】** T1① 落地新枚举口径，起草期已预跑原始输出（见 §1 问题 1 的**双口径**实测：闸口径 20→7、构成拆分 归档 13 + 现役 7）。T1② 的成文面已由 `grep -c archive docs/DEV-GOVERNANCE.md` 现跑核为 0（成文前）。**索引锚面随 T1〔原〕移出，其 census（11 条已合 main 却非 landed）转入 §6 留根① 交接下一批**，本批不再作为落地依据。

**【硬项 1c · 复扫模式完备性先验】** 本批 T1 涉**退役语面残留清扫**。模式集显式枚举全形态：`archive`（英文词形，目录名精确匹配）× 有 / 无 backtick × 两种消费形态（`SKIP_DIRS` 集合成员 / `norecursedirs` 配置项）。**完备性论证**：`SKIP_DIRS.has(entry)` 按**目录名精确相等**匹配（`acceptance-run-check.mjs:46` = `if (SKIP_DIRS.has(entry) || entry.startsWith('.')) continue;`，`entry` 为 `readdirSync` 的单段名），故无需考虑大小写变体或路径形态——`archive` 一词即完备。T2/T3 不涉退役语面。

## 4. human 取舍点（OQ）

### OQ1 · T1（验收闸枚举面）与 T2（新 fail-loud 路径）的闸 / 行为触点，走 Track B 判官判、还是升 full-adversarial + human

**背景（必须给全）**：本批两个 T 触到闸 / 脚本的**判定与行为语义**：T1 改**验收闸的枚举面**（承载 ARQ 自陈「**非门判定语义变更**——若判为门扩齿须如实升 review」）；T2 引入**新的 fail-loud 退出路径**（承载 ARQ 自陈「② 经 current/ 解析的脚本启动时回显所解析 changeId、与本包不符即 fail-loud」，方向为**向严**）。**human 2026-09-18 已裁「都留 Track B」**（该裁决对本批仍有效）。

**选项**：
- **(a) 留 Track B**（human 已裁；判官按 `track-b-judge-brief` 全面审）。理由：(1) 不命中两条 Track A 判据；(2) 两处方向均为**收窄 / 向后兼容 / 向严**——T1 收窄枚举面、T2 的硬失败默认关；(3) 半径已由起草期实测照出（**闸口径 20→7 两侧**；此处原记「13→0」为子目录口径，据 R1-F1 / R2-N1 更正——该更正是**口径修正**，不改本 OQ 的裁决内容）。
- **(b) 升 full-adversarial + human**。理由：T2 的新硬失败路径如果被误用（调用方误传 `--expect-change-id`）会**打断合法簿记**。
- **(c) 折中**：Track B，但 T2 的 `--expect-change-id` 在**落地后先只回显、不启用硬校验**，观察一段再启。

### OQ2 · T3（tighten-only §6）的落点与强度

**(a) 按五件原样落 `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6**（**human 2026-09-18 已裁**）。理由：该纪律是「给冻结件开后门」的**授权来源**，授权边界该由 human 划；五件里 1/2 两条是机器可判的硬闸。

### OQ 裁决（human 2026-09-18，v0.5 状态复核）

| OQ | 裁决 | v0.5 状态 |
|---|---|---|
| OQ1（闸触点轨道） | **(a) 留 Track B** | 维持（T1〔索引锚〕移出后，本批闸触点由「三处」降为「两处」，判断不变） |
| OQ2（§6 落点与强度） | **(a) 五件原样落 DEV-GOVERNANCE** | 维持，无变化 |
| ~~原 OQ1「T1〔索引终态锚〕…」~~ | — | **该 T 无 OQ**（其处方经起草期实测证伪 → 随 T 移出） |

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V2 为 `--self-check` 电池，其新增夹具须在输出中**具名出现**（防「加了夹具但没接线」的真空绿）；V3 的重放须先断言输入非空（枚举前命中数 > 0）；V5 为探针类，须覆盖「不带 flag」「带错误值」两态。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | **双探针**（R1-F1 采纳）：① 真跑 `node .claude/scripts/acceptance-run-check.mjs` 取 `surfaces.firmwareAcceptanceFiles`；② 按闸同款 `collectFiles` 逻辑枚举 `firmware/`，按路径含 `/archive/` 与否拆分计数 | ① 补 `archive` 前 = **20**（>0 非空基线）、补后 = **7**；② 补前 = 归档 **13** + 现役 **7**、补后 = 归档 **0** + 现役 **7**（**与 V2 自检输出的 `fwAcc=7` 必须一致**，前后各留完整输出） |
| V2 | `node .claude/scripts/acceptance-run-check.mjs --self-check` | 全绿 + 新增实盘断言（`archive` 路径零枚举）具名出现 |
| V3 | **四路探针**（**起草期已核实 `dispatch-stamp.mjs` 现无 `--self-check` 入口**，故不写自检、改实跑探针；第 ④ 路为 R2-N3 采纳、覆盖 F6 新定义态）：① `grep -c "resolved changeId" .claude/scripts/dispatch-stamp.mjs`；② 在 `current/` 指向某包的临时状态下跑一次真调用；③ 带 `--expect-change-id <不同值>` 再跑一次；④ **在无上下文状态**（`current/taskpackage.json` 不存在）下跑两次——带 `--expect-change-id <任意值>` 一次、不带一次 | ① ≥1；② stderr 出现 `resolved changeId = <该包 id>` 且**来源标注与实际路径一致**（读指针时为 `from delivery/taskpackages/current/taskpackage.json`；另测 `--task-package-id <id>` 显式路径应为 `from --task-package-id`）；③ **非零退出** + 打印两值；④ **带 flag 而无上下文 → 非零退出**（F6 定义态）、**不带 flag 而无上下文 → 仍 exit 0**（既有行为不变） |
| V4 | `grep -c "archive" docs/DEV-GOVERNANCE.md` + `grep -c "tighten" docs/DEV-GOVERNANCE.md` | 两条均 ≥1（**原均为 0**）；且 §5/§6 两节标题 `grep -c` 各 1（防两 T 互撞） |
| V5 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V6 | `git diff .claude/scripts/changes-index.mjs` | **零改动**（证明 T1〔索引锚〕确已移出、未夹带） |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 3 路径） |

## 6. 留根清单

**① T1〔索引终态锚〕——移出本批、退回 ARQ 队列，需重新设计**（human 2026-09-19 裁定；起草期实测触发）

本条的实质产出，须完整交接，否则下一批会重复本批的错误设计：

- **原处方被证伪**：`ARQ-260909-index-verdict-anchor-okform-gap` 的处方是「`deriveLanded` 补第四锚，识 `ok: true（GREEN/PASS）` 形态」。起草期实测：**19 份 `landing-verdict.md` 中只有 1 份**是该形态（260909-gov-review-gate-truthfulness）。补一条锚解决不了实际缺口。
- **实际缺口面（起草期 census，11 条「已合 main 但 INDEX 状态非 landed」）**：

  | 批次 | 实际 verdict 写法 | 锚为何不认 |
  |---|---|---|
  | `260907-gov-review-architect-dispatch-model-override` | `**verdict：APPROVE**` | token 集合 `(PASS\|GREEN\|RED\|FAIL\|BLOCK)` **无 APPROVE** |
  | `260908-gov-review-escalation-conflict-ladder` | `### 7.0 Verdict：**GREEN（…）**` | 编号正则 `\d{1,3}[.、]` 吃不了点分号 `7.0` |
  | `260908-gov-review-merge-chain-hardening` | 同上形态 | 同上 |
  | `260907-gov-review-merge-friction` | `**绿（GREEN）**——…` | 无「标签 + 冒号」 |
  | `260904-gov-review-escalate-merge-approval` | `**最终 verdict：GREEN。**` | `最终` 挡在标签前 |
  | `260830-arch-review-ui-impl-strategy` | `**绿（放行建议）——…**` | 同上 |
  | （另 5 条） | **无 `landing-verdict.md`** | 老惯例批 |

- **病灶定性（关键，与本批原判断不同）**：脚本**自己的设计意图是对的**——`.claude/scripts/changes-index.mjs:176` 注释写「新批强制规范 verdict 行（`> verdict: PASS/GREEN`）→ 100% 走机器锚；旧批发散散文不强行机械解析（太脆），归 ① 人工 adjudication」，且 `LANDED_ADJUDICATION` 人工 pin 表**已存在且已用**（`:36` 起 7 条，注释「发散散文旧批…人工核 verdict 结论为通过态」）。**坏的不是设计，是「新批没照做」**——260904～260915 这 6 条近批各写各的、一条都没进 pin 表，于是静默显示 frozen。**故「补一条锚」是治标、会变成打地鼠。**
- **影响面定性（降低判断难度）**：INDEX 是**投影、不是拦阻依据**（文件头自陈「不做拦阻依据」），故这是**显示真实性问题**、非闸缺陷；危害是误导读者（本批起草期即被误导一次）。
- **建议下一批的处置方向（不自批，交 review）**：优先按脚本自身设计意图收口——① 把上表 6 条已落地批按实况 pin 进 `LANDED_ADJUDICATION`（一行一条 + 人读理由，沿既有 7 条格式）；② 把「点分编号」这个**明显安全**的形态补进 `VERDICT_HEADING_RE`（`\d{1,3}` → 允许 `\d{1,3}(?:\.\d{1,3})?`）；③ verdict 行规范成文（`docs/templates/` + verifier 简报）把缺口堵在未来。**不追求用正则穷举所有历史写法**（`绿` / `最终 verdict` 类形态本身脆）。
- **队列状态**：`ARQ-260909-index-verdict-anchor-okform-gap` **仍在册未结**，已在 `docs/ARCH-REVIEW-QUEUE.md` 补注本批实测 census 与病灶定性，供下一批直接引用。

**② T3 的引用行一侧未落**（`acceptance-author` agent def 的 References 行）——本批只落 DEV-GOVERNANCE §6；引用行需动 agent def，触 self-def 判断点，留后续。

**③ `current/` 同族解析点未改——清单据实扩充（R1-F2 采纳；计数口径经 R2-N2 更正）**——本批只改 `dispatch-stamp.mjs`（见 T2 边界）。**计数口径须与所引命令一致**：`grep -rln "taskpackages', 'current'\|taskpackages/current" .claude/scripts .claude/hooks`（并集模式，覆盖 `join(...,'taskpackages','current')` 与路径字符串两种写法）= **17 个文件**；窄模式 `grep -rln "taskpackages/current"` = **10 个文件**（v0.6 曾引宽命令却只报 17 而未注明口径，R2-N2 更正）。其中「读指针取 changeId」式调用点至少含：`.claude/scripts/journal-append.mjs:51` / `pod-events-append.mjs:99` / `pod-events-aggregate.mjs:29` / `loop-metrics.mjs:64` / `mutation-check.mjs:17` / `security-check.mjs:13` / `acceptance-coverage-check.mjs:25` / `docs-sync-check.mjs:22` / `closeout-check.mjs:51` / `devloop-artifact-check.mjs:42` / `taskpackage-required-check.mjs:29` / `verifier-evidence-check.mjs:124` / `.claude/hooks/verify.mjs:123` / `supervisor-verdict-gate.mjs:468` / `supervisor-gate-check.mjs:618`（清单本身经 R2 逐一亲核、无误列）。**其中 `.claude/hooks/verify.mjs:123` 恰是 ARQ-260913 病例①「九闸跑在错误包上下文全绿」的承载者**——本批不动它，但它是该族病灶最该收口的一处。待观察 `--expect-change-id` 的实际采用率，再决定是否推广为常设硬校验（或直接在其余调用点上加回声）。

**④ `current/` 指针副本的行尾字节差同族面**（loop-retro-2026-09-13，screen-unification 改进 4：指针副本刷新须字节级复制、禁 Write 重打全文）——本批未承接（属 dev-loop 操作纪律，非治理脚本面）。**建议下个动 `current/` 刷新面的批承接**。

**⑤ `ARQ-260908-known-red-count-no-mechanical-source` 子项②③**——子项①已由 260909 T5 承接；②（evidence 齿 root 注入）落 `.claude/references/acceptance-evidence.md`，与本批 §0 非目标冲突；③（runner 报告头写执行目录）属 host 测试基建面。两项留根。

**⑥ 拆包族另三批**（`-merge-chain-friction` 已 FROZEN v1.0 / `-dispatch-chain-friction` / `-batch-drafting-friction`）——本批不承接。

**⑦ 移出本族的两个 launch 面 ARQ**（输入清单先例取旧、launch 前置清单不核）——留 ARQ 队列后续单独处理。

**⑧ T3 第 2 条 `sha256` 核销的工具面**（R1-F5 连带）——本批只成文「记入 `REVISION LOG` 同一条」这一落点，**不提供脚本辅助**（生成/比对 hash）。若首例执行时发现手工易错，随任一治理批补工具。

**⑨ `ARQ-260913-current-pointer-implicit-global` 建议改动①未承接**（R1-F3 采纳）——该 ARQ 给了三条建议：本批只落第②条（脚本回声 + 可选硬校验）；**第①条「起包 checklist 固定第一条：核 `current/` 指向本包」既未承接、v0.5 亦无留根指针**，属 `planner.md` 起包 checklist 条文面（触 agent def），留根待后续治理批；第③条（指针副本刷新走字节级复制）见本条④。

**交日落复盘项**：本批 T1〔原索引锚〕的 census（11 条）建议在下一批落地后复跑一次，确认是否清零；T1① 撤 archive 后观察 final 相位是否如预期不再枚举退役件。

**自律件**：起草期已按硬项 1a 亲核（§1 全部断言带命令与日期）；13 件清单与 20 份页脚清点均为现跑枚举、非转引；v0.5 的 T1〔索引锚〕移出为 human 裁定 + 起草期实测双重驱动，非主线程自裁。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-19 - architect

**verdict：BLOCK(F1..F6)**（F1 为 BLOCKING、F2–F5 为 SHOULD、F6 为 NIT；无「影响目标效果」级问题——三个 T 的修法方向均经亲核成立，OQ 两项 human 裁决与内容无实质冲突，故**无升级 human 项**，打回均为事实更正与留根补全，修订到位后可直接进 R2）。

#### 亲核留痕（R1 全面审，全部现跑实测，工具 = Read/Grep/Bash 于 `gov/kidpal` worktree，2026-09-19）

- **结构门**：landing-manifest 围栏块在场（3 路径）且 ≡ §2 各 T 目标文件并集（`acceptance-run-check.mjs` / `dispatch-stamp.mjs` / `docs/DEV-GOVERNANCE.md`）；packet-id 行在场（Status 头部块「packet-binding」行）。✓
- **§1 事实断言逐条**：`acceptance-run-check.mjs:31` `FW_ACC_RE`、`:34` `SKIP_DIRS`（无 `archive`）、`:46` 跳过规则、`unit-run-check.mjs:38`（含 `'archive'`，:34-37 注释在）、`dispatch-stamp.mjs:60-72` `resolveChangeId()` 全程无输出、`:165` 调用点、`server/pytest.ini:6` `norecursedirs` 含 `archive`——全部 verbatim 核实。✓
- **计数复核（不采信转述，自跑）**：① REVISION LOG 页脚 grep = **20 份** ✓（12 归档 C + 6 现役 C + 2 归档 py，逐文件名吻合）；② `firmware/main/test/acceptance/` 子目录枚举 = **13 件且全在 archive/ 下** ✓（与 PROPOSAL 一致）；③ **但闸真实口径（root=`firmware/`）枚举 = 20 件**（13 归档 + 7 现役 `main/test/*.acceptance.c`，含 `host_infra_smoke.acceptance.c`），补 `archive` 后 = **7 件**（见 F1）；④ `docs/DEV-GOVERNANCE.md` grep `archive` / `tighten` 均**零命中** ✓，「## 验收测试纪律」:264、§1 :268 / §2 :279 / §3 :283 / §4 :287 ✓；⑤ 三行 ARQ 原文逐字核对（`docs/ARCH-REVIEW-QUEUE.md:43/:63/:66`）✓——`ARQ-260916` 原文确记「9 件」、`ARQ-260908` 原文确记「7 份」且三处引据漂移（`:1700→:1761`、`:979→:1087`、`:95→:100`）经 sed 逐行核验属实，PROPOSAL 的两份【硬项 1a 数字更正申报】成立；⑥ commit `4fc0969` / `7c6d7c5` / `7ab2d47` 经 `git rev-parse` 全部存在 ✓，`0e55cfab` 为 pod-events `eventId`（`delivery/taskpackages/260829-schedule-reminder/pod-events.jsonl:23`，PROPOSAL 表述准确、本就未称其为 commit）✓；⑦ `7ab2d47` 限冻结件两文件 `git show --numstat` = `101/0` + `200/0`、删除行计数 0 ✓（整 commit 含 taskpackage.json 簿记改动有删除行，ARQ 口径本就限冻结件）；⑧ advisory `0e55cfab` payload ④ 防滥用条款原文亲读 ✓（见 F4）。
- **T1 边界透镜（dispatch 特别提示）**：补 `archive` 后两闸 `SKIP_DIRS` 完全一致（逐成员比对）✓；`server/` 下唯一 `archive` 目录 = `acceptance/archive`，而 `PY_ACC_RE` 要求 `acceptance/` 直接子文件，归档层文件本就不匹配、`server/acceptance/` 现役仅 `conftest.py`（py 枚举 = 0，补前后均 0）——**server 面零受影响**，改动效果恰收敛于固件面 13 件 ✓；「退役保留名」三处代码承载点（两 `SKIP_DIRS` + `pytest.ini` `norecursedirs`）与 T1② 成文内容逐一对得上 ✓；自检电池补实盘断言沿 `real-repo-no-archive-collected` 先例可行（本闸 `--self-check` 现输出 `real-repo-smoke ... fwAcc=20` 可直接改挂）✓。
- **T2 边界透镜**：`dispatch-stamp.mjs` 的 stderr 输出**无任何程序化消费方**（全仓 grep：引用方均为 prose——`planner.md` 纪律、references、事件 `source` 字段值回显；无脚本 parse 其 stderr）→ 回显一行不破坏任何消费方 ✓；但 `current/` 解析面枚举不全（见 F2）。
- **T3 透镜**：第 1 条（`-U0` 删除行 = 0）机械可判且有已执行先例（`7ab2d47`）✓；第 2 条见 F5；第 3 条三件记录在 `schedule_reminder_b.acceptance.c:1766/:1780` REVISION LOG 与红基线摘要（`7ab2d47` 新增 `88/0`）有真先例 ✓；第 4/5 条措辞机械可判 ✓。
- **V5 基线现跑**：`route-regression-check.mjs` 28 ok 0 fail ✓；`lint-descriptions.mjs` 仅既有 `landing-verifier.md` 超长告警（363 > 320）✓——与 §5 V5 预期一致。

#### findings

**F1（BLOCKING）· §0/§1 问题 1 的「验收闸固件面」口径断言与闸真实行为不符，V1 预期值同病**

- 证据（实测）：`node .claude/scripts/acceptance-run-check.mjs`（真跑，2026-09-19）输出 `"firmwareAcceptanceFiles": 20`；同款逻辑复现（root=`firmware/`、现行 `SKIP_DIRS`）= 20 件 = 13 归档（`main/test/acceptance/archive/`）+ **7 现役**（`main/test/char_stroke|host_infra_smoke|key_layer_dispatch|keybar_three_slot|p1_home_battery|reminder_overlay_unify|voice_round_ui` 各 `.acceptance.c`）；补 `archive` 后 = **7 件**（非 0）。
- 矛盾点（PROPOSAL verbatim）：§0「验收闸固件面枚举到的 13 件全在归档目录下、现役 0 件」；§1 问题 1「该闸固件面的 oracle 输入集当前**只剩退役件**」；§5 V1「补后 **0** 条」。三者把**子目录探针口径**（root=`firmware/main/test/acceptance/`，13→0 正确）误升格为**闸口径**（root=`firmware/`，实际 20→7）：闸的 oracle 输入集现役件非 0，且落地后闸枚举亦非 0。若 landing 验证按 V1 字面理解「补后 0」，将与 V2 自检实盘输出的 `fwAcc=7` 直接冲突，验证集自相矛盾。
- 定性：**不影响 T1 修法方向**（跳 archive 后退役 13 件确被剔净、现役 7 件保留、两闸口径同步——目标效果仍达成），属基线事实失准 + 验证集口径错标，按本批自立的硬项 1a 纪律必须更正后方可 FROZEN。
- 建议修法：① §0/§1 改为「闸口径枚举 20 件（13 归档 + 7 现役）；其中 `firmware/main/test/acceptance/` 子面 13 件全为退役件、现役 0」；② V1 改为两路探针并各钉枚举根：子目录根 13→0（保留）+ **闸根 `firmware/` 20→7**（新增，两侧留输出），使 V1 与 V2 的 `fwAcc=7` 自洽；③ F1 更正后同步核对 §2 T1① 与 §3 sweep 表中所有「13 件」表述的口径标注。

**F2（SHOULD）· 留根③ / §3 的 `current/` 同族调用点枚举不全**

- 证据（实测 grep `delivery/taskpackages/current/taskpackage.json`）：经 `current/` 解析的脚本共 **8** 处——`dispatch-stamp.mjs:64`、`journal-append.mjs:51`、`hooks/verify.mjs:123`、`acceptance-coverage-check.mjs:25`、`closeout-check.mjs:51`、`devloop-artifact-check.mjs:42`、`docs-sync-check.mjs:22`、`loop-metrics.mjs:64`。PROPOSAL 留根③ 只点名 `journal-append.mjs` / `verify.mjs` 两处、§3 sweep 表同（「journal-append / verify.mjs 同族」）。
- 定性：T2「只改一个」的边界本身成立（OQ1 已裁留 Track B，回声 + 默认关不依赖全量改）；但留根清单是下一批的交接面，少列 5-6 处会让推广决策（留根③「观察 `--expect-change-id` 采用率再推广」）建立在不完整面上。
- 建议修法：留根③ 补全 8 处清单（至少补 `hooks/verify.mjs`——它是 ARQ-260913 病例① home-p1 九闸假绿的直接承载者，恰是三病例中本批改法没覆盖的那一类）。

**F3（SHOULD）· `ARQ-260913` 建议改动① 无承接亦无留根指针**

- 证据：`docs/ARCH-REVIEW-QUEUE.md:63` 该 ARQ 建议改动三项：① 起包 checklist 固定「核 current/ 指向本包」；② 脚本回显 + 不符 fail-loud；③ 指针副本字节级复制。本批 T2 承接 ②（形态调整为可选 flag）、留根④ 承接 ③；**① 既未进任何 T 也无留根条目**，而该 ARQ 被本批声明为「承载」并预期闭账。
- 建议修法：留根新增一条登记建议①（起包 checklist 条文，落点 planner 起包纪律面），或在 T2 说明里显式声明不承接的理由。

**F4（SHOULD）· T3「逐字为据，非重写」的出处表述不准**

- 证据（verbatim 对照）：advisory `0e55cfab`（pod-events.jsonl:23）payload ④ 原文为连排条款「git diff 对冻结件须纯 additive；新齿必须重跑当初暴露 vacuity 的原变异证明转红并 sha256 还原；REVISION LOG append 带 Reason；mutationEvidence 该条 sensitive:false 翻 true；范围帽=…；意图锚定=…」——**不含**「`git diff -U0` 删除行数 = 0」「（不许另造易过变异自证）」「红基线摘要补一段」三个操作化增补。PROPOSAL §2 T3 的五件成文形态实与 **`ARQ-260908` 队列行**（`docs/ARCH-REVIEW-QUEUE.md:43` 建议改动段，登记期加工）逐字吻合。
- 定性：内容与 human OQ2 已裁「五件原样落」无冲突（human 看到的即是五件成文形态，且三个增补均为向严、不放松），但「逐字为据，非重写」的溯源标注错误——五件的操作化形态出自 ARQ 队列行而非 advisory 原文。按硬项 1a 引据纪律应更正出处。
- 建议修法：T3 说明改为「采纳 `ARQ-260908` 队列行所载五件（其形态为 advisory `0e55cfab` ④ 的登记期操作化，含三处向严增补）」。

**F5（SHOULD）· T3 第 2 条「sha256 还原核销」缺记录落点规约，按现文是半句可执行、半句口号**

- 证据：仓内 sha256 机械先例仅 `taskpackage-required-check.mjs`（taskpackage pin 逐字节比对，merge-driver.mjs:858 注释引用）与 merge-driver 自用，**无任何对冻结件「变异→还原」做 sha256 核销的已执行先例**（`7ab2d47` REVISION LOG 与红基线摘要均无 hash 记录；`schedule_reminder_b.acceptance.c:1766` 只记 `Reason: mutation evidence sensitive:false`）；advisory/ARQ 亦未指定核销结果落在哪。
- 定性：`sha256sum`/`git hash-object` 是零成本可执行命令，「还原」本身可机判；但**核销证据无落点** = 事后无法审计第 2 条是否被遵守，恰落在本批自立的「硬项可执行性」透镜上（口号 vs 机械可判的边界）。
- 建议修法：T3 落地正文给第 2 条补一句落点（如「核销证据 = REVISION LOG 该条附还原前后 hash 或核销命令输出摘录」），使其与第 3 条三件记录同构可审计。

**F6（NIT）· T2 两个未定义交互态**

- 证据：`dispatch-stamp.mjs:60-72` `resolveChangeId()` 优先取 `--task-package-id` 显式值、否则读 `current/`；`:167-170` 无上下文时 stderr 提示后 **exit 0**。
- 问题：① 回显行固定写「(from delivery/taskpackages/current/taskpackage.json)」在显式 flag 路径下是错误归因；② 传了 `--expect-change-id` 而解析值为 null（无上下文）时的行为未定义（现路径 exit 0 会把硬校验静默吞掉）。
- 建议修法：T2 落地说明补两句——回显按实际来源标注（flag / current/）；`--expect-change-id` 在场且解析 null 时同判不符、fail-loud。

#### R1 结论

BLOCK(F1..F6)。F1 为唯一 BLOCKING（闸口径基线失准 + V1 预期错标，修订为子目录/闸根双探针即可）；F2–F5 均为事实标注与留根交接面补全；F6 为落地说明补定义。三个 T 的改法方向、riskTier=medium、G1+G3 profile、Track B 轨道自评均经亲核成立；两条 OQ 的 human 裁决无需重开。**无升级 human 项。**

### R2 - 2026-09-19 - architect

**verdict：BLOCK(F7；附 N1–N3 NIT)**——F1–F6 六条修订五条全到位（F1 数字与 V1 双探针、F3 留根⑨、F4 溯源更正、F5 核销落点、F6 交互定义），F2 实质到位但方法引述有计数出入（N2）；**新引入一处危害定性失准（F7，SHOULD）**，一行措辞修掉即可 GREEN。**无升级 human 项**（不影响三个 T 的目标效果，不涉 OQ 重开）。

#### 亲核留痕（R2 聚焦复核，2026-09-19，commit `f3618a6` = 单文件 82+/15-，仅本 PROPOSAL，无夹带）

- **F1 到位性**：① §1 双口径数字与本判官 R1 实测**逐项一致**——真跑闸 `firmwareAcceptanceFiles: 20` ✓、构成 = 归档 13 + 现役 7 ✓、现役 7 件逐件列名与 R1 枚举完全吻合（含 `host_infra_smoke.acceptance.c`）✓、补后 20→7 ✓；`:137` 枚举根（`const fwDir = join(root, 'firmware')`）与 `:206` `firmwareAcceptanceFiles` 行号亲核属实 ✓。② V1 双探针（① 真跑闸 20→7；② 闸同款枚举拆分 13+7 → 0+7）**与 V2 自检 `fwAcc=7` 自洽**，原矛盾消除 ✓。③ §0（「枚举 20 件、其中 13 件是归档退役件」）、§3 硬项 1b（双口径）均已抄齐 ✓；残留见 N1（§4 OQ1 内一处旧口径括注）与 F7（危害定性新句，见下）。④ 危害定性：前半「20 件混 13 件退役、清点报数虚高 2.85 倍」成立（20/7 = 2.857，实测核算）✓；后半见 F7。
- **F2 到位性**：留根③ 所列 **15 个调用点逐一亲核全部属实、无一误列**——`pod-events-append.mjs:99` / `pod-events-aggregate.mjs:29`（均为 explicit 优先 + `current/` 回退）、`mutation-check.mjs:17` / `security-check.mjs:13` / `taskpackage-required-check.mjs:29`（同型）、`verifier-evidence-check.mjs:124`（current/ 对比基准）、`supervisor-verdict-gate.mjs:468`（`readCurrentChangeId`）、`supervisor-gate-check.mjs:618`（G0 载入）、`hooks/verify.mjs:123`、`journal-append.mjs:51`、`loop-metrics.mjs:64` 及 R1 已核 4 处 ✓；`hooks/verify.mjs:123` 的病例①承载者定性成立 ✓。**漏列检查**：`brief-inputs.mjs`（current/ 为**排除**语义）、`merge-driver.mjs`（current/ 投影 merge 特判，非取 changeId）不属解析点、可不列 ✓。唯方法引述有出入（N2）。
- **F3 到位性**：留根⑨ 补齐——三条建议逐条对账（② → T2、③ → 留根④、① → 留根⑨），定性与边界（`planner.md` 起包 checklist 条文面、触 agent def）准确 ✓。
- **F4 到位性**：T3 溯源更正准确——ARQ-260908 队列行自陈来源为 advisory `0e55cfab`（该行尾列原文核实）✓；「该行已操作化的形态、相对 advisory 原文含三处向严增补、不等于原文连排条款」与本判官 R1-F4 的 verbatim 对照结论一致（三处增补 = `-U0` 删除行数 / 不许另造易过变异 / 红基线摘要补一段）✓。不再 over-claim。
- **F5 到位性**：第 2 条补「核销证据记入冻结件 `REVISION LOG` 同一条、格式 `<前 sha256> → <后 sha256>`」+ 如实声明无已执行先例（首例）+ 工具面留根⑧。判为**可执行规约而非口号**：事后审计者可对冻结件现盘重算 hash 与「后 sha」比对（还原到位则两者相等）、`前→后` 相等性本身即还原证明；与第 3 条三件记录同址同构 ✓。
- **F6 到位性**：回声二选一如实反映来源（`:61-62` flag 路径 / `:64-68` current/ 路径，两处代码亲核）✓；`--expect-change-id` 在场 + 解析 null（无上下文）→ fail-loud、不传 → 维持现状 exit 0——定义自洽、与既有消费方零冲突（无任何既有调用方传该新 flag；默认关路径行为不变）✓。V3 探针未覆盖该新定义态（N3）。

#### findings

**F7（SHOULD，v0.6 新引入）· §1 口径更正申报的危害定性后半句机制不成立**

- 证据（实测）：句为「退役面上的断言失败会被计入现役验收结果」。但 ① acceptance 闸固件面是**结构闸**——`fwAccFiles` 仅喂 `decide()`（「有文件 → 须有 runner」存在性判定，`acceptance-run-check.mjs:87-97`）与计数上报，**不执行任何固件断言**；② 真正执行现役验收的 host_app runner 用**显式文件清单**（`firmware/main/test/host_app/main/CMakeLists.txt:36-94` 恰列 7 件现役 `.acceptance.c`），且其注释明言归档件「已移入 ../acceptance/archive/…**不再编入**」（`:12`/:18）。故「退役面上的断言失败」**没有任何通道进入现役验收结果**——该危害机制在本闸与 runner 的实际构造下不存在。
- 对照：`ARQ-260916` 原文自己的定性是准确的（「当前只影响 oracle 输入集与统计、不产红；但将来 host_app runner 判据变更/撤除时会被退役件打红」）。v0.6 把失准的「只剩退役件」换成了另一句失准的机制断言，属修订引入的新事实矛盾（按硬项 1a 不得带病 FROZEN）。
- 建议修法：该句改为「退役件混入 oracle 输入集与清点统计（虚高 2.85 倍）；若将来 host_app runner 判据变更/撤除，退役件会独立触发结构红（复刻 unit 闸病灶）」——即回收 ARQ 原口径，删「断言失败计入现役结果」半句。改完即可 GREEN（本判官无需 R3 全量复核，收一条措辞修订即可放行）。

**N1（NIT）· §4 OQ1 选项 (a) 理由 (3) 残留旧口径括注「（13→0 两侧）」**

- 证据：§4 OQ1 选项 (a) 仍写「半径已由起草期实测照出（13→0 侧）」——子目录口径旧数，闸口径实为 20→7。属 human 决策记录区，**建议加括注更正而非改写原文**（如「（v0.5 子目录口径；闸口径实为 20→7，见 §1 更正申报）」），保决策留痕不失真。

**N2（NIT）· 留根③ 的方法引述与实际计数不符（实质清单正确）**

- 证据（实测）：`grep -rln "taskpackages/current" .claude/scripts .claude/hooks` = **10** 个文件（非 17）；按「读指针取 changeId」语义实测解析面 = **16** 处（所列 15 + 本批在改的 `dispatch-stamp.mjs`）。15 个列点本身全部属实（见亲核留痕），无实质错误；「17 个文件」既不匹配所引命令也不匹配语义计数。建议改为「语义枚举 16 处（含 dispatch-stamp；清单 15 处如下）」或给出真实产出 17 的原命令。

**N3（NIT）· V3 未覆盖 F6 新定义态**

- 证据：F6 已定义「`--expect-change-id` 在场 + 无上下文（解析 null）→ fail-loud」，但 §5 V3 三路探针只测「不带 flag」「带错误值」两态。建议 V3 补第四路（临时移走/指向空 `current/` + 带 flag → 非零退出），防该新分支落地时无验证面（真空绿规约自身的「须覆盖两态」可顺势扩为三态）。

#### R2 结论

BLOCK(F7)。F1–F6 修订实质全部到位（F2 的方法计数出入记 N2）；唯一拦路项 F7 为 v0.6 新引入的一句危害机制断言失准（一行措辞更正，方向：回收 ARQ-260916 原口径），N1–N3 顺手修。三个 T 的改法、验证集（V1 双探针已与 V2 自洽）、riskTier / profile / 轨道自评维持 R1 判定成立。**无升级 human 项。**