# 轻量设计 PROPOSAL · 261120-gov-review-verify-gate-integrity-cluster

> **状态：FROZEN v0.2**（2026-11-21 主线程凭「human 对 OQ-1 的裁定 = 甲 + R1/R2 设计审 GREEN」标；冻意图、边界、关键方案与验收标准，不冻未来字节。DRAFT v0.2 修订说明与 §9 评审记录保留）
> **gov-flow**: direct-implementation
> **承载 ARQ**：`ARQ-261117-change-manifest-cutoff-projection-bypass`（MAJOR → T1）· `ARQ-261117-acceptance-coverage-assertion-and-flag-gaps`（SHOULD → T2）。两条均为既定波次、human 已拍启动，方向确定，本设计只做落点与判别力设计，不重新取舍。
> **riskTier**：low（与两条 ARQ 自评一致）。判档归 architect-reviewer；本设计只在 §7 提供可能影响判档的评估事实，不自判、不自评通过。R1 已判「常规 direct-implementation + 轻条件加强（实现评审侧）」，见 §9。
> **批主题**：验证闸说假话簇（批一）——两闸的失败模式方向相反却同类：T1 是**该红的地方静默绿**（fail-open 假绿），T2 是**该绿的地方假红 + 误用旗标时一个静默绿一个报错误话**。

## 1. 目标与非目标

### 目标

- **T1 · cutoff 基准与回显**：让 `change-manifest-check.mjs` 的 pre-cutoff 存量放行谓词改用与全仓其余 8 处 cutoff 判据同基准的**包目录首提日**，使 2026-09-28 之后起包的包真正进入 changeManifest 对账；同时让 `skipped` 不再是黑箱——输出如实回显实际取到的基准路径、首提日与 cutoff 值。
- **T1 · 旗标面收口（human 2026-11-21 裁定 = OQ-1 甲，本批一并落）**：`change-manifest-check.mjs` 对齐 `mutation-check.mjs` 既有先例——① 支持 `--change-id <id>` → `delivery/taskpackages/<id>/taskpackage.json`（优先级最高，位置参数与环境变量兼容不回退）；② 未知 `--` 旗标 **fail-loud 非 0 退出** + 打印支持的旗标集；③ `--help` / `-h` → usage + **exit 0**；④ 把两类早退分开——「压根没给上下文」保留既有 `skipped:'no-taskpackage-context'` 的 N/A 语义与 exit 0，「显式给了上下文但不可解析」（旗标值 / 包路径打错 / 文件不可读 / JSON 不可解析）改为 **fail-loud 非 0**。②④ 是本批**唯一一处 blocking 判据方向变更（绿→红），已获 human 授权**（裁定留痕见 §7 OQ-1，落点见 §3.2 / §3.3，验收见 §6 V5）。
- **T2**：让 `acceptance-coverage-check.mjs` 的断言识别面认得 ESP-IDF Unity 形态（本仓固件 host 验收主力形态），消除纯 Unity 验收件假红；同时把旗标面对齐 `mutation-check.mjs` 既有先例（`--change-id` / 位置参数兼容 / 未知旗标 fail-loud / `--help`），使两闸调用形状一致。
- **判别力**：两 T 的关键断言都必须落在**真实消费层**（无参 CLI 形态，即 `verify-gates.json` 注册的那条命令）上的临时 git 仓 / 临时目录夹具，并且反转实现必须让对应断言变红。P8/P9 批的实测教训（自检电池全绿而消费层带洞，因为夹具只打在判据计算层）在本批以验收标准的形态强制闭合，见 §6 V1 / V4；R1 已把「reviewer 亲自 spawn 真实无参 CLI 复跑」列为条件加强项（§9 SHOULD-1），本设计不以作者自证替代。

### 非目标

- 不改 `MANIFEST_CUTOFF` 的日期值，不改对账面定义（`RECONCILE_FACE_RE` = `firmware/** + server/**`），不改 glob 判据与非法形态判据，不改 `taskpackage-required-check.mjs` 的任何判据（它是本批的**基准对照件**，只读）。
- 不新增子命令、不新建门、不改 `.claude/verify-gates.json` 的闸注册（两闸继续以无参形态被 `verify.mjs` spawn；旗标面新增对既有注册向后兼容）。
- 不改 `lib/current-pointer-shared.mjs`（本日实测 15 件 import 方 = 3 钩子 + 12 脚本共享该单源；T1 只是开始消费它**已经返回**却被丢弃的 `.changeId`，不需要改库、不改回显文本格式）。
- 不做全仓闸旗标面对齐：`.claude/verify-gates.json` 注册的 10 个闸中仅 `mutation-check` 支持 `--change-id`（实测 1/10，见 §2.3）。本批按 human 裁定把 `change-manifest` 与 `acceptance-coverage` 两闸对齐到同一旗标集（支持该旗标的闸由 1 件增至 3 件），**其余 7 闸（`taskpackage-required` / `firmware-build` / `contract-lint` / `docs-sync` / `security` / `unit` / `acceptance`）的旗标面不触**；是否另立波次归主线程。
- 不为 T2 引入注释 / 字符串剥离（`acceptance-coverage-check.mjs:96-100` 明文记载「要剥注释 = 门语义变更，须另行走治理批 + human」；实测影响面见 §2.3。该项为 OQ-2 的 (b) 分支，**reviewer R1 已裁定不取**，本批落 (c) 行首锚定 + 容纳 `#define` 行，见 §4.1 / §7 OQ-2）。
- 不把 pre-cutoff / no-context 两类既有 `skipped` 语义改成 `notExecuted` 信号（存量放行是「不适用」而非「未执行」；`verify.mjs:211-223` 的 `liftNotExecuted` 三信号面不动）。
- 不追溯重判 53 个历史包的已归档 verifier evidence（历史只读；本批只让**今后**的 verify 轮次真正对账）。
- 不在本批结项或搬动 ARQ 条目文件；账本状态与归位由主线程按收口流程处理（manifest 已预留记账行，见 §8）。
- 不碰 `.codebase-memory/`、契约面、spec 面、scope-map、settings、指令根、任何 agent 定义。

## 2. 现状复证（2026-11-20 起草期实测；两条 ARQ 均**未消化**）

### 2.1 T1 · cutoff 基准错读 current/ 投影文件 —— 未消化，且已量化到 53 包 / 200 份轮次证据

代码实核（行号为本日起稿时的实际行号，ARQ 条目所记 `:53-58` 已漂移，见 §2.4）：

- `change-manifest-check.mjs:111-115` `taskPackagePath()`：`explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH`；无显式参数时 `return resolveCurrentTaskPackage('change-manifest', projectDir).path`。**只取 `.path`**——`lib/current-pointer-shared.mjs:38-51` 的同一调用还返回 `.changeId`（且实测解析正确），被丢弃。
- `:30` `MANIFEST_CUTOFF = '2026-09-28'`；`:137-139` `const firstDate = gitFirstCommitDate(tpPath); if (firstDate && firstDate < MANIFEST_CUTOFF) → print({ok:true, skipped:'pre-cutoff'}) ; return`。即 cutoff 日期取自**解析出的 taskpackage.json 文件路径**的首提日。

git 实测（本仓，2026-11-20）：

| 路径 | `git log --diff-filter=A --follow --format=%cs` 最早值 | 与 cutoff 比较 |
|---|---|---|
| `delivery/taskpackages/current/taskpackage.json`（闸实际所用基准） | **2026-09-11** | < cutoff → 恒放行 |
| `delivery/taskpackages/current`（目录，「上溯」候选基准） | **2026-09-15** | < cutoff → 同样恒放行 |
| `delivery/taskpackages/261104-k5-story-cover-nav`（包目录，正确基准） | **2026-11-04** | > cutoff → 应对账 |
| `delivery/taskpackages/261104-k2-points-device-ui`（包目录） | **2026-11-04** | > cutoff → 应对账 |

当场无参实跑（真实消费层形态，gov worktree，current → `261104-k5-story-cover-nav`）：

```
stderr: [change-manifest] TaskPackage 上下文解析：changeId=261104-k5-story-cover-nav（来源：delivery/taskpackages/current/taskpackage.json）
stdout: {"ok":true,"gate":"change-manifest","skipped":"pre-cutoff",
         "message":"gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28 → 存量放行"}
exitCode: 0
```

即：**changeId 解析完全正确，cutoff 日期取错文件**。同一包用显式 canonical 路径调（ARQ 记载的 workaround）当场即进入真正对账：`ok:true / manifestSize:29 / reconciledFaceCount:0 / exitCode 0`——证明包本身的 manifest 是完备的，被跳过的纯粹是「对账动作」。

历史证据面量化（本日起稿实测）：`grep -rl "gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF" delivery/taskpackages/*/verifier-evidence` = **200 份轮次证据文件，覆盖 53 个不同包**。主线程提供的逐字样本经本 agent 亲核为实：`delivery/taskpackages/261104-k2-points-device-ui/verifier-evidence/rounds/r4.json:31-43` 的 change-manifest 条目为 `ok:true / exit:0 / exitCode:0 / timing:"scoped"`，stdout 逐字含 `"skipped": "pre-cutoff"` 与 `gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28 → 存量放行`，stderr 逐字为 `[change-manifest] TaskPackage 上下文解析：changeId=261104-k2-points-device-ui（来源：delivery/taskpackages/current/taskpackage.json）`。同型命中数：`261031-openclaw-cron-trigger-gating` 2 份、`261104-k5-story-cover-nav` 5 份、`261028-m6-k2b-p2-schedule-16px` 2 份、`261104-k2-points-device-ui` 4 份（每轮恰一条）。

影响面量化：104 个有 canonical `taskpackage.json` 的包中，**58 个包目录首提日 ≥ 2026-09-28**（起于 `260928-char-window-key-triggered-play`，止于 `261104-k5-story-cover-nav`），全部落在静默跳过面内；46 个为真存量包（本批后仍应放行）。

姊妹闸基准对照（同一 cutoff 谓词家族，全部用**包目录**形态，change-manifest 是唯一例外）：

| 落点 | cutoff | 基准路径形态 |
|---|---|---|
| `taskpackage-required-check.mjs:148`（changeManifest 完备性判据） | `MANIFEST_CUTOFF` | `delivery/taskpackages/${cid}` |
| `taskpackage-required-check.mjs:172`（AC 回引判据） | `AC_BACKREF_CUTOFF` | `delivery/taskpackages/${cid}` |
| `taskpackage-required-check.mjs:422`（changeId 前缀判据） | `CHANGE_ID_CUTOFF` | `delivery/taskpackages/${cid}` |
| `devloop-artifact-check.mjs:167-171` | `CHANGE_ID_CUTOFF` | `delivery/taskpackages/${changeIdStr}` |
| `closeout-check.mjs:154` / `:244` | `CHANGE_ID_CUTOFF` | `delivery/taskpackages/${changeId}` |
| `changes-index.mjs:187` | — | `delivery/taskpackages/${name.name}` |
| **`change-manifest-check.mjs:137`** | `MANIFEST_CUTOFF` | **解析出的 taskpackage.json 文件路径** ← 唯一例外 |

由此产生一个此前未被记载的**不对称**：同一 cutoff 谓词下，「manifest 必须在场且非空」这条判据（`taskpackage-required` 侧，包目录基准）**是活的**，而「manifest 必须覆盖实际改动」这条判据（`change-manifest` 侧，投影文件基准）**从未生效**。缺 manifest 会红，manifest 写漏不会红——恰好掩盖 ARQ 影响面节所说的 manifest 起草缺口（261104-k2 / k5 的漏编译单元、漏生成件命名律）。

**anti-假绿 机器为何没抓到**：`verify.mjs:211-223` 的 `liftNotExecuted` 只认 `degraded:true` / `phase:"executor-skipped"` / `allLiveDeselected:true` 三信号；本闸的 `skipped:'pre-cutoff'` 不含任何一者，于是被记为 `ok:true / notExecuted:false` 的**如实通过**。全仓无任何机械消费方解析 `pre-cutoff` 字面（仅注释与本闸自身输出）。所以这不是「已声明未执行」，是「声称执行并通过」。

**起草期实测否决 ARQ 候选①的第二分支**：ARQ 建议①写「从 `taskPackage.changeId` 拼 `delivery/taskpackages/<cid>`，**或从解析出的路径上溯两级**」。第二分支经实测否决——无参形态下解析路径的父目录 = `delivery/taskpackages/current`，其首提日 **2026-09-15**（合成夹具中为 2026-09-11），同样早于 cutoff，上溯等于没修。基准必须来自**解析出的 changeId**，不能来自路径几何。

**合成夹具已复现并证明可判别**（本日起稿期实测，node fs + `git init` + `GIT_COMMITTER_DATE` 回填日期，仓外临时目录，验后已删）：commit A 回填 2026-09-11 只建 `current/taskpackage.json`；commit B 当日建新包目录（`changeManifest:["server/**"]`）并把 current/ 重指向新包；工作树留一个未提交、未被 manifest 覆盖的 `firmware/y.c`。三基准实测 = 投影文件 2026-09-11 / `current` 目录 2026-09-11 / 新包目录 2026-11-20。无参跑闸 → `ok:true / skipped:"pre-cutoff" / exitCode 0`（假绿，与真仓 53 包同型）；同一夹具下按正确基准 → `ok:false / outOfManifest:["firmware/"] / exitCode 1`。**注意夹具陷阱**：起草期第一版夹具把两个文件都放进回填 commit，导致两基准同日期、夹具零判别力（修前修后同绿）——§6 V1 因此把「只回填投影、包目录当日」写成夹具构造硬条件。

### 2.2 T1 附带发现 · 第三处 fail-open：显式路径不可解析也报绿

同一 `skipped` 黑箱还吞掉**调用方错误**。当场实测（gov worktree，无参以外的三种误用）：

| 调用 | 输出 | exitCode |
|---|---|---|
| `change-manifest-check.mjs --change-id 261104-k5-story-cover-nav` | `ok:true, skipped:"no-taskpackage-context"`（`--change-id` 被当路径解析） | **0（绿）** |
| `change-manifest-check.mjs delivery/taskpackages/no-such-pkg/taskpackage.json` | `ok:true, skipped:"no-taskpackage-context"`（包名打错） | **0（绿）** |
| `change-manifest-check.mjs --help` | `ok:true, skipped:"no-taskpackage-context"` | **0（绿）** |
| `acceptance-coverage-check.mjs --change-id <id>`（同一误用） | `ok:false, "TaskPackage file does not exist", taskPackage:"<repo>/--change-id"` | 1（红，但报错误话） |
| `mutation-check.mjs --bogus`（先例） | `未知旗标：--bogus` + usage | 2（fail-loud） |

成因：`change-manifest-check.mjs:131-134` 把「显式传了路径但不可解析」与「压根没有上下文（human 跳流程例外）」并入同一 `ok:true` 早退。三个闸对同一误用给出三种方向（绿 / 红错话 / exit 2），属本批主题「闸说假话」的第三个实例。起草时按边界要求不自行扩面、作为 **OQ-1** 交裁（reviewer R1 亦判「升级 human，不替人裁」）；**human 2026-11-21 裁定 = 甲（本批一并收口）**，甲乙对照与裁定留痕见 §7 OQ-1，落点见 §3.2 / §3.3，验收见 §6 V5。表中三种误用的「修前 exit 0 绿」即 V5 的红基线证据。

### 2.3 T2 · Unity 断言不识别 + 旗标面缺失 —— 未消化，且量化出识别面与 prose 面的真实边界

代码实核：`acceptance-coverage-check.mjs:74-85` `hasRealAssertion` 的六支模式 = `\bexpect\s*\(` / `\bassert\s*[.(]` / `\.to(?:Be|Equal|Have|Contain|Match|Throw)` / `\.assert[A-Z]\w*\s*\(` / `(?:^|\n)\s*assert\s+\S`（行首锚定，注释明写「避 prose 误报」）/ `\.fail\s*\(`。`TEST_ASSERT_EQUAL_MESSAGE(` 中 `ASSERT` 前是下划线（词字符），`\bassert` 词边界不成立 → 全部 Unity 形态不匹配。

当场正则实测：`TEST_ASSERT_EQUAL_MESSAGE(3, got, "ctx")` / `TEST_ASSERT_TRUE(ok)` / `TEST_FAIL_MESSAGE("...")` / `TEST_ASSERT_EQUAL_MEMORY(...)` / `TEST_ASSERT_NULL(p)` → 全 **false**；libc `assert(p != NULL);` → **true**（印证 261104-k4 踩坑记载「K2 能过只是因为恰有一处 libc `assert()`」）。

当场 CLI 层夹具实测（临时目录 + `current/taskpackage.json` + 一个带 `// coverage: UP-A1` 标记的 `.c` 验收件，无参跑闸；coverage 判定已满足，被测的只是断言识别）：

| # | 验收件内容形态 | 实测 exit / message |
|---|---|---|
| 1 | 纯 Unity 行首 `TEST_ASSERT_EQUAL_MESSAGE(...)` + `TEST_ASSERT_TRUE(...)` | **1 · "Acceptance test file has no recognizable assertion"（假红）** |
| 2 | 同上 + 一处 libc `assert(p)` | 0（= k4 的绕过手法，实测成立） |
| 3 | 注释行内含**带括号**的 `TEST_ASSERT_EQUAL_MESSAGE(...)` 字样 | 1 |
| 4 | 字符串字面量内含**带括号**的 `TEST_ASSERT_TRUE(x)` 字样 | 1 |
| 5 | 真无任何断言 | 1 |
| 6 | 仅 `TEST_FAIL_MESSAGE(...)` | **1（假红）** |

全仓识别面普查（`git ls-files firmware server`，本日实测）：

- `TEST_ASSERT_*` / `TEST_FAIL*` 字样 **4804 处，分布 60 个文件**；其中**调用形态**（token 后紧跟 `(`）**4800 处**。
- 4800 处调用形态中，落在注释 / 字符串字面量里的 = **0 处**；落在 `#define` 预处理行的 = **78 处**（全是 `#define XXX_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(...)` 一类**真断言宏包装**，含 `firmware/main/test/acceptance/red-baseline/unity.h` 的 stub 宏定义）。
  → 结论：**无锚定的调用形态加宽在当日语料上零 prose 误报**；但同时意味着「prose 负例」在真实语料里不存在，只能靠合成夹具钉（§6 V4 case 4/5）。
- 今天处于假红态（含 Unity 字样却 `hasRealAssertion=false`）的 5 个文件：`firmware/main/test/acceptance/archive/happy_flow_capture.test.c`、`firmware/main/test/acceptance/archive/reminder_page.test.c`、`firmware/main/test/acceptance/red-baseline/unity.h`、`firmware/main/test/char_window.test.c`、`firmware/main/test/story_page_v2.test.c`。
- **两种加宽变体的实测差异 = 恰 1 个文件**：无锚定变体绿而行首锚定变体红的唯一文件 = `firmware/main/test/acceptance/red-baseline/unity.h`（其断言只以 `#define` 宏包装形态出现，全在行中）。该文件在 104 个包的 `acceptanceTestsFile` 中**从未被引用**（实测 0 命中），故当下无实包受害，但属潜在假红面。

prose 负例强度分叉（实测数据；起草时交 **OQ-2** 裁，**reviewer R1 已裁定取 (c)**，裁定留痕见 §7 OQ-2，行为契约见 §4.1）：

| 变体 | case 1/2/6（纯 Unity 正例） | case 3/4（注释 / 字符串内**带括号**字样） | `#define` 宏包装件（unity.h 形态） | 与既有已声明限制的关系 |
|---|---|---|---|---|
| (a) 无锚定调用形态加宽 | 绿（修好） | **绿 = 新增 fail-open 面** | 绿 | 与 `:96-100` 既有已声明限制同类（既有 JS/Python 模式下注释里的 `self.assertIn(a, b)` 同样算断言） |
| (c) 行首锚定 + 允许 `#define` 行 | 绿（修好） | **红（保住负例）** | 绿（需显式允许预处理行，否则 unity.h 假红） | 严于既有限制；沿用本文件 `:83` 裸 `assert` 的行首锚定先例，新识别面零新增 fail-open |
| (b) 全模式剥离注释 / 字符串 / 预处理 | 绿 | 红 | 需保留宏定义判定 | **触既有门语义**：实测 154 个今天被认作「有断言」的文件中 **2 个**会翻红（`firmware/main/display/points_page.c`、`tools/simulator/LvglWindowsSimulator/lv_conf.h`，均非验收件）；`:96-100` 明文要求「另行走治理批 + human」→ 本批不取 |

旗标面实核：`acceptance-coverage-check.mjs:22-28` 直接用 `process.argv[2]`，无 `--change-id`、无 `--help`、无未知旗标拒绝（`--help` 当场被当路径解析，实测 exit 1 + `taskPackage:"<repo>/--help"`）。先例完整存在于 `mutation-check.mjs`：`:18-23` usage 文本、`:25-30` `flagVal`、`:33-35` `changeIdTaskPackagePath`、`:37-48` 三形态优先级 + `echoResolution` 来源如实标注、`:243-258` `--help` exit 0 与未知旗标 exit 2、`:209-230` **CLI 层（spawn）自检先例**（未知旗标 exit 2 + 位置参数兼容两条判别 case）。

`--change-id` 的真实普及度（本日起稿精确普查，纠正 ARQ 条目「与 mutation-check 不一致」可能被读成的「多数闸都支持」印象）：全仓**实际解析该旗标**的机器面文件共 **9 件** = 脚本 7（`brief-inputs:41`、`loop-metrics:62-64`、`mutation-check:38`、`pod-events-aggregate:29`、`pod-events-append:123`、`retro-efficiency:975`、`verifier-evidence-check:514`）+ 钩子 2（`hooks/verify.mjs:413`、`hooks/supervisor-verdict-gate.mjs:676`）；`dispatch-stamp.mjs:260` 用的是**另一个**旗标 `--expect-change-id`，不计入。而在 `.claude/verify-gates.json` 注册的 **10 个闸里只有 `mutation-check` 一件支持**（1/10），其余 9 闸（含本批两件）均不支持。故本批 T2 的旗标对齐是「向唯一闸内先例 + 9 件工具面惯例靠齐」，**不是**「补齐两个落后者」；全仓 9 闸的旗标面对齐属另一波，明确出本批范围（§1 非目标）。

### 2.4 两条 ARQ 的行号引用已漂移（实质结论不变）

`ARQ-261117-change-manifest-cutoff-projection-bypass` 记 `taskPackagePath()` 在 `:53-58`，实际现为 `:111-115`；`ARQ-261117-acceptance-coverage-assertion-and-flag-gaps` 记 `hasRealAssertion` 在 `:74-83`，实际现为 `:74-85`（260902 Python 热修后增行）。两处的谓词、常量值、分支结构与影响面结论经本日起稿逐项亲核**均仍成立**，仅行号需按「符号名引用、不引行号」口径更新（先例：`lib/governance-gate-shared.mjs:47` 明写「按符号名引用，不引行号——同批删常量后行号即漂」）。ARQ 条目文件本批不改（账本归主线程），此处只记事实。

## 3. T1 · change-manifest cutoff 基准修正与 skipped 去黑箱

### 3.1 基准（唯一判据）

pre-cutoff 判定的日期来源改为**包目录** `delivery/taskpackages/<changeId>` 的首提日，与 §2.1 表中其余 8 处 cutoff 判据同基准；`<changeId>` 取自解析结果（`resolveCurrentTaskPackage` / `resolveExplicitTaskPackage` 已返回的 `.changeId`），**不得**由解析路径的父目录 / 上溯推导（§2.1 已实测否决）。cutoff 日期值、比较方向（`<` 放行）、`gitFirstCommitDate` 的 git 调用形态（`--diff-filter=A --follow --format=%cs`，取最早一条）三项**不变**——本 T 只换喂给它的路径，不换谓词。

### 3.2 三种调用形态都必须成立（三形态均为本批必落项）

| 形态 | changeId 来源 | 期望基准 |
|---|---|---|
| 无参（= `verify-gates.json:64` 注册形态，真实消费层） | `resolveCurrentTaskPackage(...).changeId` | `delivery/taskpackages/<该 changeId>` |
| 显式包路径（`argv[2]` / `DEV_LOOP_TASKPACKAGE` / `TASKPACKAGE_PATH`） | `resolveExplicitTaskPackage(...).changeId` | 同上（由文件内 changeId 派生，不用传入路径本身） |
| `--change-id <id>` | 旗标值 | `delivery/taskpackages/<id>` |

第三形态原为 OQ-1 待裁项，**human 2026-11-21 裁定 = 甲后已入本批必落项**，三形态一律成立。优先级 = `--change-id` 旗标 > 位置参数 / 环境变量 > `current/` 指针回退，与 `mutation-check.mjs:37-48` 先例一致；解析来源继续经 `echoResolution` 如实三分类标注，不固定写某一类。**无参形态仍是验收判别的主落点**（§6 V1），因为它是 `verify-gates.json:64` 注册、`verify.mjs:271-272` 原样 spawn 的真实消费形态；旗标面新增**不得改变无参路径上的任何判定**（该路径唯一的行为变化 = §3.1 的 cutoff 基准修正）。

### 3.3 两类早退分开（无上下文 N/A 不变 · 显式上下文不可解析 fail-loud）+ changeId 不可得的方向

现状 `change-manifest-check.mjs:131-134` 把两类完全不同的情形并入同一 `ok:true / skipped:'no-taskpackage-context'` 早退（§2.2 实测三种误用皆绿）。本批按 human 裁定分开，三分支的判据与行为如下：

| 情形 | 判据 | 修后行为 | 修前实测 |
|---|---|---|---|
| **压根没给上下文** | 无位置参数、无 `DEV_LOOP_TASKPACKAGE` / `TASKPACKAGE_PATH`、无 `--change-id`，且 `current/` 投影不可得或不可解析 | 保留 `skipped:'no-taskpackage-context'` + `ok:true` + **exit 0**（human 跳流程例外的既有 N/A 语义，一字不改） | 同（**不变**） |
| **显式给了上下文但不可解析** | 位置参数 / 环境变量 / `--change-id` 任一在场，而目标文件不存在、不可读或 JSON 不可解析 | **fail-loud 非 0 退出**，报错点名实际收到的值与期望形态（不再说「无上下文」这句假话） | `ok:true` + **exit 0（假绿）** |
| **未知 `--` 旗标** | argv 含不在支持集内的 `--` 旗标 | **非 0 退出 + usage 列出支持旗标集**（先例 `mutation-check.mjs:251-256`，exit 2） | 被当路径解析 → **exit 0（假绿）** |

边界与不受影响面（三条，实施时须逐条成立）：

- **`verify.mjs` 既有调用不受影响**：注册形态是无参（`verify-gates.json:64`），`verify.mjs:271-272` 对 `timing:scoped` 闸原样 spawn 该 cmd、不追加任何旗标，故真实闸链恒走「无参 + `current/` 回退」路径，落在上表第一行与 §3.1 的基准修正上；fail-loud 两分支只在**调用方显式传参或传错旗标**时才可能触发，不改变任何一轮真实 verify 的既有判定方向。
- **「changeId 不可得」≠「上下文不可解析」**：显式路径可读、JSON 可解析、但缺 `changeId` 字段（或包目录在该 worktree 尚未提交致 `gitFirstCommitDate` 返回 null）时，属「上下文已给且可读」，**不走 fail-loud 分支**，而是按 cutoff 基准不可得处理 → 按 post-cutoff 进入真正对账（`gitFirstCommitDate` 返回 null 时既有分支本就不跳过，方向天然 fail-closed）；字段缺失本身由 `taskpackage-required` 侧既有必填字段校验承接，本闸不新增字段校验。
- **不得回退到任何路径几何基准**（父目录 / 上溯推导即复现本 bug，§2.1 已实测否决）；回显须如实标注基准不可得，不得静默按某个默认日期比较。

### 3.4 skipped 去黑箱（回显义务）

无论走跳过还是走对账，输出都必须携带**实际使用的 cutoff 判据三元组**：基准种类（包目录 / 不可得）、基准路径、取到的首提日，外加 cutoff 常量值与解析出的 changeId。目标是让 verifier evidence 的轮次 JSON 自身就能回答「这次为什么跳过 / 为什么没跳过」，不需要再读源码或猜。既有 `echoResolution` 的 stderr 回显（changeId + 来源三分类）保持不动，本节新增的是 **cutoff 基准**这一维。`skipped:'pre-cutoff'` 与 `skipped:'no-taskpackage-context'` 两个字面值保留（历史证据可比对；无机械消费方，见 §2.1）。

### 3.5 本 T 的行为净效果（如实声明，不粉饰）

T1 是**激活一个休眠的 fail-closed 阻塞闸**：58 个 post-cutoff 包今后的 verify 轮次会真正对账，manifest 写漏即红（退出码非 0），处置通道 = 既有的「builder/fixer 停手 → planner 带根因假设派 architect 扩单」，本批不新建豁免、不放宽判据。46 个真存量包的放行不变（§6 V2 钉）。历史 53 包 / 200 份已归档轮次证据不追溯重判。gov lane 本身不跑 `verify.mjs`，故本批自身合并不受该激活影响。

此外，按 human 裁定的旗标面收口（§3.3）新增一处**方向变更**：显式传参打错、未知旗标两类误用由 `exit 0`（假绿）改为非 0（fail-loud）。该变更**不触真实闸链**（`verify.mjs` 恒以无参形态 spawn，见 §3.3 第一条），受影响的是人工/派单直接调闸时的误用形态——即原本会静默放过派单写错的那一类。无上下文 N/A 语义（human 跳流程例外）与 pre-cutoff 存量放行语义两项均**一字不改**。

## 4. T2 · acceptance-coverage 断言识别加宽与旗标面对齐

### 4.1 Unity 识别面（写成行为契约，不写正则字节）

新增识别面必须同时满足以下四条**可判行为**（正负例的机械夹具见 §6 V4）。**变体已由 reviewer R1 裁定 = (c) 行首锚定 + 显式容纳 `#define` 预处理行**（裁定依据与实测数据见 §7 OQ-2），故下列四条全为必落项：

1. **纯 Unity 验收件不再假红**：以 `TEST_ASSERT_*`（含 `_MESSAGE` 等全部后缀变体）与 `TEST_FAIL*` 的调用形态承载断言的文件，判为「有可识别断言」。
2. **宏包装形态不漏判**：断言只以 `#define` 宏包装形态出现的文件（`red-baseline/unity.h` 形态，实测 78 处）同样判为有断言——即行首锚定策略必须显式容纳预处理定义行。**已知宽松面（诚实注记，R1 NIT-4）**：容纳 `#define` 行会把「仅定义 `TEST_ASSERT_*` 宏而不调用」的 header 认作有断言；这是本条的刻意选择（避免 `unity.h` 假红），且该文件在 104 个包的 `acceptanceTestsFile` 中实测 0 命中，属可接受宽松面，不再收紧。
3. **不新增 prose fail-open 面**：注释行 / 字符串字面量内**带括号**的 Unity 字样不得单独构成「有断言」。裁定的 (c) 变体满足本条（实测 4800 处调用形态中 4722 处行首 + 78 处 `#define`，(c) 零漏判，同时 §6 V4 case 4/5 两类 prose 负例保持红）。**前向边界（诚实注记，R1 OQ-2 要求补记）**：行首锚定会让「仅以行中调用承载断言」的**未来**验收件假红（当日语料 0 例；与本文件 `:83` 既有 libc 裸 `assert` 的行首锚定先例同类）。若今后出现该形态，处置 = 把断言写成行首（首选，与既有先例一致）或另立治理批放宽锚定，**不得**在本闸就地放宽。若今后改用无锚定 (a) 变体，则本条不成立、须在批记录显式记为「 knowingly 接受与既有限制同类的新增 fail-open 面」并同步改 §6 V4 case 4/5 的预期，**不得静默调整预期凑绿**。
4. **既有识别面零回归**：JS（`expect(` / `.toBe` 等）、Python（`self.assertXxx(` / 行首裸 `assert` / `.fail(`）与 libc `assert(` 六支既有模式的判定不变，`:101-119` 既有 5 条自检 case 全保留、预期不改。

`isShallowStatusOnly`、`hasCoverageMarker`、structural / suiteGate 豁免面、coverage 对账逻辑**全部不动**（本 T 只动「有没有断言」这一问）。不引入注释 / 字符串剥离（(b) 变体，触既有门语义、按 `:96-100` 须另批 + human，本批不取，见 §7 OQ-2）。

### 4.2 旗标面（对齐 `mutation-check.mjs` 先例；与 §3.2 / §3.3 的 change-manifest 侧为同一旗标集）

- 支持 `--change-id <id>` → `delivery/taskpackages/<id>/taskpackage.json`，优先级最高；位置参数与 `DEV_LOOP_TASKPACKAGE` / `TASKPACKAGE_PATH` 兼容不回退（既有派单写法不坏）。
- 未知 `--` 旗标 **fail-loud**：非 0 退出 + 打印支持的旗标集，不再把旗标当路径静默解析（当日实测的 `<repo>/--change-id` 误报形态必须消失）。本闸当前对误用已是 exit 1（红，但报错误话），故此条对本闸**不涉绿→红方向变更**；方向变更只在 change-manifest 侧（§3.3，已获 human 授权）。
- `--help` / `-h` → usage + 退出码 0（当前 `--help` 被当路径解析并报错，属可用性缺陷）。
- 解析来源继续经 `echoResolution` 如实三分类标注（`--change-id 旗标` / `位置参数/环境变量` / `current/ 指针`），沿用 `lib/current-pointer-shared.mjs:9-11` 的「始终回显」纪律，不固定写某一类。
- `--self-check` 保持既有入口与既有 5 case，新增 case 按 §6 V4 的**CLI 层**要求组织（spawn 自身，先例 = `mutation-check.mjs:209-230`）。

### 4.3 两闸旗标面一致性的边界（human 裁定后的落地范围）

按 human 2026-11-21 裁定（OQ-1 = 甲），本批后 `change-manifest-check` 与 `acceptance-coverage-check` **两闸支持同一旗标集**：`--change-id <id>` / 位置参数与环境变量兼容 / 未知旗标 fail-loud 非 0 / `--help` exit 0 / `--self-check`，与 `mutation-check` 先例同形。两闸唯一差异 = change-manifest 侧的未知旗标与不可解析路径由**绿转非 0**（方向变更，已获 human 授权），acceptance-coverage 侧本就非 0（只改报错文本与退出码取值，不改方向）。**不接受的结果**：两闸各自半改、形成第三种调用形状。

须如实标注的一致性天花板：闸内先例此前只有 `mutation-check` 一件（10 闸中 1/10，§2.3），本批把「支持 `--change-id` 的闸」从 1 件推到 3 件，**仍不构成全仓闸旗标面一致**；其余 7 闸（`taskpackage-required` / `firmware-build` / `contract-lint` / `docs-sync` / `security` / `unit` / `acceptance`）仍以无参 + 位置参数为唯一形态，本批不触（§1 非目标）。写在此处是为了防止批记录把「两闸一致」误读成「闸面已统一」。

## 5. 保护边界与牵连面

- **两件实现文件均为 ordinary**：`.claude/scripts/change-manifest-check.mjs`、`.claude/scripts/acceptance-coverage-check.mjs` 在 architect 现有 allow 内，不属条件加强的核心落点清单（前置升级闸 / 共享闸库 / merge-driver / deny-main-push / enforce-scope / INDEX landed 推导），也不属保护面（指令根、scope-map、settings、verify-gates、自身与 review-loop agent 定义、独立评审/验证凭据）。本批**零保护面落地**，全部由 architect 在 gov worktree 非 main 分支实施。
- **`.claude/verify-gates.json` 不改**（保护面）：两闸注册形态（无参 `node .claude/scripts/<x>.mjs`）与 `timing:scoped` 均无需变更，旗标新增向后兼容。`:66` 的 `_doc` 所述「与 taskpackage-required-check 共享同一 changeId cutoff 谓词（pre-cutoff 在途包两闸同豁免）」在修后**比修前更贴近事实**，无需改写；若 reviewer 认为 `_doc` 应显式写明基准为包目录，该行属保护面，由主线程按同一份设计处理，本 agent 不写。
- **`lib/current-pointer-shared.mjs` 不改**：本日实测 15 件 import 方（3 钩子 + 12 脚本）共享该单源，本批两件实现文件均在其内；只开始消费库已返回的 `.changeId`，不改库、不改回显文本格式（避免波及另 13 个消费方）。
- **`taskpackage-required-check.mjs` 不改**：它是 T1 的基准对照件，本批只读它取基准形态；其 `:148-149` / `:172-173` 的「同参重复 null 重试」冗余（`devloop-artifact-check.mjs:167-171` 同型，`devPath` 与 `tpPath` 为同一字符串）**不在本批**，如实记为观察项，是否另立 ARQ 归主线程。
- **牵连面 · 行号引用漂移（本批须处置）**：`.claude/references/taskpackage-authoring.md:38` 以 `acceptance-coverage-check.mjs:48` 形式引用 `hasCoverageMarker`；T2 的旗标解析落在 `taskPackagePath()`（`:22-28`），位于 `:48` **之前**，插入即致该行号失真。处置候选（ordinary reference 文件，architect 可写）：把该引用改为**符号名形态**（`hasCoverageMarker`），沿用 `governance-gate-shared.mjs:47` 的「按符号名引用、不引行号」先例，一次消除复发面；不改该句的规范内容（「一标一 id」的写法要求本身不动，那是 `ARQ-260907` 已归档条目的六波复现议题，不属本批）。该文件因此入 manifest。
- **无机械消费方破损**：全仓无脚本 / 钩子解析 change-manifest 的 `skipped` 或 `pre-cutoff` 字面（§2.1）；`verify.mjs` 只按退出码判 `ok`，新增回显字段是 stdout JSON 的**加字段**（形状兼容，不改既有键名与语义）；verifier evidence 轮次文件为历史只读，不回填。
- **批记账件预留**（P9 批实测摩擦教训：main-thread 记账件须由 architect 在 manifest 预留行，否则闸 scope 核验拦）：两条 ARQ 条目、`docs/debt/gov/INDEX.md`（`debt-render.mjs` 生成件）、结项时的 `docs/debt/gov/_archive/2026/`（渲染器自动归位，人不搬文件）、`delivery/workstreams/gov-debt-reduction/task_plan.md`（波次追注）。证据尾豁免面按 `isEvidenceTailPath`（`governance-gate-shared.mjs:121-126`）实测为**三项**——本批目录 `delivery/review/261120-gov-review-verify-gate-integrity-cluster/` + `delivery/review/INDEX.md` + `delivery/review/.changes-index-state.json`（不是任意 `delivery/review/**` 他批目录；R1 NIT-2 已收窄措辞），三项均不入 manifest。
- **blocking 判据语义变更的如实声明**（原表述「不改任何 blocking 判据语义」已按 human 裁定修正）：本批含**一处经 human 授权的方向变更**——`change-manifest-check` 对「显式给了上下文但不可解析」与「未知旗标」两类误用由 `exit 0`（假绿）改为 fail-loud 非 0（§3.3 表第二、三行；human 2026-11-21 裁定 = OQ-1 甲，留痕见 §7）。除此之外**其余判据一律不变**：T1 不改 cutoff 谓词、日期值、对账面、glob 判据，只让既有判据真正被喂对输入（激活一条休眠的 fail-closed 判据，方向 = 更多红，非放宽）；无上下文 N/A 语义与 pre-cutoff 存量放行语义一字不改；T2 只加宽识别面 + 对齐旗标面（acceptance-coverage 侧不涉方向变更）。真实闸链（`verify.mjs` 无参 spawn）不经过该方向变更分支，见 §3.3 第一条。

## 6. 验收标准与自测计划（机械可执行；每条给出预期红/绿形态）

> 判别力总则：以下 V1 / V4 是**消费层**判别（以 `verify-gates.json` 注册的那条无参命令形态 spawn 闸本体），V2 / V3 / V5 / V6 是边界与不回归判别。凡标「反转必须变红」的条目，实施记录须附**实际做过的反转动作与观察到的红**（仓外临时副本或临时开关，不触仓内历史件），沿用 P9 批 §10「判别力反转抽验」的先例形态。

### V1 · T1 消费层判别（本批核心，对应「至少一条判别落在真实消费层」）

**夹具构造（硬条件，构造错则判别力为零——§2.1 已实测踩过）**：临时目录内 `git init`（先例：`landing-fidelity-check.mjs:411-412`、`merge-driver.mjs:778-784`），`user.email` / `user.name` 本地配置；
- commit A：以 `GIT_COMMITTER_DATE` + `git commit --date` **回填到 cutoff 之前**（如 `2026-09-11`），内容**只有** `delivery/taskpackages/current/taskpackage.json`；
- commit B：**当日**提交 `delivery/taskpackages/<post-cutoff-pkg>/taskpackage.json`（`changeManifest: ["server/**"]`）+ 一个已跟踪的 `firmware/base.c`，并把 `current/taskpackage.json` 重指向该包（重指向是修改、不改其首提日）；
- 工作树留一个**未提交**的 `firmware/new.c`（实现面，manifest 未覆盖）。
- 断言前须先自证夹具成立：三个基准日期分别为「投影文件 < cutoff」「包目录 ≥ cutoff」，两者必须**不同侧**；同侧即夹具无效，必须重造，不得据以判绿。

**执行形态**：`spawnSync(process.execPath, [<repo>/.claude/scripts/change-manifest-check.mjs])`，**零位置参数**，`cwd` 与 `CLAUDE_PROJECT_DIR` 同指临时仓——与 `verify.mjs:271-272` 对 `timing:scoped` 闸的调用为**同形态消费层调用**（R1 NIT-3 更正措辞：`verify.mjs` 用 `spawnSync(cmd, { cwd: projectDir, shell:true })` 且 cmd 为字符串，本夹具用数组参数直起 node，二者非逐字同形）。消费层属性一致的三点 = 零位置参数、`cwd` 与 `CLAUDE_PROJECT_DIR` 同指项目根、走闸自身 argv 解析与 git 调用，判别力不受该差异影响。

| 断言 | 预期（修后） | 修前实测 | 反转要求 |
|---|---|---|---|
| 退出码 | **非 0（红）** | 0 | 恢复投影文件基准 → 变 0 → 断言红 |
| stdout 不含 `"skipped"` 键 | 成立 | 含 `skipped:"pre-cutoff"` | 同上 |
| stdout `outOfManifest` 含 `firmware/new.c` | 成立 | 无该键 | 同上 |
| stdout 回显的 cutoff 基准 = 包目录路径 + ≥ cutoff 的首提日 | 成立 | 回显 `2026-09-11`（投影文件） | 同上 |

（可选更强形态，非必做：若能在同一夹具上跑通完整 `verify.mjs --phase=scoped` 闸链，把该轮 evidence 里 change-manifest 条目的 `ok:false` 一并记入实施记录，作为消费层证据的加强；跑不通不阻验收，无参 CLI 同形已足够。）

### V2 · T1 不over-red：三类边界必须保持既有语义

同一夹具族，同样以无参形态跑：

| 变体 | 预期 | 判别意义 |
|---|---|---|
| manifest 改为 `["firmware/**","server/**"]`（覆盖 `firmware/new.c`） | **exit 0 / `ok:true` / 无 `skipped` / `reconciledFaceCount ≥ 1`** | 证明修复不是「恒红」；反转成「无条件 fail」会让本条红 |
| 包目录也回填到 cutoff 之前（真存量包形态） | **exit 0 / `skipped:"pre-cutoff"`**，且回显基准为包目录 + 回填日期 | 证明 46 个真存量包的 carve-out 不被本批吃掉 |
| 无 `current/`、无任何参数（human 跳流程例外形态） | **exit 0 / `skipped:"no-taskpackage-context"`** | 证明既有 N/A 语义未变 |
| 包目录在该 worktree 未提交（`gitFirstCommitDate` 返回 null） | 不跳过、进入对账（fail-closed 方向），回显如实标注基准不可得 | 钉 §3.3 的方向，防「取不到日期就放行」的新 fail-open |

### V3 · T1 真仓复证（历史包前后对比，ARQ 风险节要求的验证方式）

- `node .claude/scripts/change-manifest-check.mjs`（gov worktree 无参实跑，current → `261104-k5-story-cover-nav`）：预期**不再出现** `skipped:"pre-cutoff"`，输出 `ok:true` + `manifestSize:29` + `reconciledFaceCount:0`（与今日显式路径实跑结果一致，本设计已实测该值）+ cutoff 基准回显 `delivery/taskpackages/261104-k5-story-cover-nav` / `2026-11-04`。修前实测 = `skipped:"pre-cutoff"` / `2026-09-11`，构成前后对比。
- `node .claude/scripts/change-manifest-check.mjs --self-check`：全绿（exit 0），且既有 16 条纯函数 case（glob 匹配 6 + glob 非法形态 2 + porcelain 解析 5 + 对账面过滤 3；本日起稿实测 `total:16 / failed:0`）零回归、预期不改。
- 抽一个 ARQ 点名的包（`261031-openclaw-cron-trigger-gating` 或 `261104-k5-story-cover-nav`）以显式路径与无参两形态各跑一次，两者对同一包的 cutoff 结论必须一致（修前两形态结论不一致 = bug 的另一面）。

### V4 · T2 消费层判别（CLI 入口，非仅调内部函数）

**夹具**：临时目录 + `delivery/taskpackages/current/taskpackage.json`（`acceptance: ["UP-A1: unity 断言识别"]`、`acceptanceTestsFile: ["firmware/main/test/up.acceptance.c"]`）+ 该验收件内含 `// coverage: UP-A1` 标记（让 coverage 判定先满足，被测的只是断言识别）。执行形态同 V1（无参 spawn + `CLAUDE_PROJECT_DIR`）。本设计已把全部 7 类的**修前**实测值填入下表第三列。

| # | 验收件内容形态 | 修后预期 | 修前实测（本日） | 反转要求 |
|---|---|---|---|---|
| 1 | 纯 Unity 行首 `TEST_ASSERT_EQUAL_MESSAGE(...)` + `TEST_ASSERT_TRUE(...)` | **exit 0** | exit 1 假红 | 去掉 Unity 识别面 → 变 exit 1 → 断言红 |
| 2 | 仅 `TEST_FAIL_MESSAGE(...)` | **exit 0** | exit 1 假红 | 同上 |
| 3 | 仅 `#define` 宏包装断言（`red-baseline/unity.h` 形态） | **exit 0** | exit 1 假红 | 若采锚定变体却不容纳预处理行 → 本条红（这正是 §2.3 实测的 (a)/(c) 唯一差异文件） |
| 4 | 注释行内含**带括号**的 `TEST_ASSERT_EQUAL_MESSAGE(...)` 字样 | **exit 1（保持红）** | exit 1 | 取无锚定 (a) 变体 → 变 exit 0 → 本条红（OQ-2 的判别点；改预期须显式记录，见 §4.1 条 3） |
| 5 | 字符串字面量内含**带括号**的 `TEST_ASSERT_TRUE(x)` 字样 | **exit 1（保持红）** | exit 1 | 同上 |
| 6 | 真无任何断言 | **exit 1** | exit 1 | 识别面放宽到恒真 → 本条红 |
| 7 | Python `self.assertIn(b, a)` / 行首裸 `assert x` 两件 | **exit 0（零回归）** | exit 0 | 既有模式被改坏 → 本条红 |

补充：`node .claude/scripts/acceptance-coverage-check.mjs --self-check` 全绿，既有 5 case（`python-unittest-positive` / `python-bare-assert-positive` / `prose-comment-negative` / `prose-string-negative` / `empty-negative`）预期逐字不改；`:96-100` 的既有已声明限制注记若因 OQ-2 选择而需改写，改写须在批记录点名（不得静默删注记）。

### V5 · 两闸旗标面判别（CLI 层，先例 = `mutation-check.mjs:209-230`）——**两闸均为必跑验收**

human 2026-11-21 裁定 OQ-1 = 甲后，change-manifest 侧由「若批准则重跑」升为**必跑验收**（V5-B），不再是条件项。两侧同一旗标集、同一判别形态；执行方式 = 临时仓夹具 + `spawnSync` 直起脚本（同 V1 执行形态注），逐条记退出码与关键输出。

**V5-A · `acceptance-coverage-check.mjs`**

| 调用 | 预期（修后） | 修前实测（红基线） |
|---|---|---|
| `--change-id <夹具包 id>` | 解析到 `delivery/taskpackages/<id>/taskpackage.json`，stderr 回显来源 = `--change-id 旗标`，判定结果与无参形态一致 | exit 1 + `taskPackage:"<repo>/--change-id"`（报错误话） |
| 位置参数传 canonical 路径 | 与今日行为一致（兼容不回退），回显来源 = `位置参数/环境变量` | 同（不变） |
| `--bogus-probe`（未知旗标） | **非 0 退出 + usage 列出支持旗标集** | exit 1 但报错误话（`TaskPackage file does not exist`） |
| `--help` | **exit 0 + usage** | exit 1（被当路径解析） |
| `--self-check` | 全绿，不被旗标面改动劫持（`isMain` 守卫先例见 `current-pointer-shared.mjs:110-114`） | 5/5 绿 |

**V5-B · `change-manifest-check.mjs`（必跑；含方向变更的红基线对照）**

| 调用 | 预期（修后） | 修前实测（红基线，§2.2 已逐条实测） |
|---|---|---|
| `--change-id <夹具包 id>` | 解析到 `delivery/taskpackages/<id>/taskpackage.json`，回显来源 = `--change-id 旗标`，判定结果与无参形态**一致**（同进对账、同结论） | **exit 0 绿** + `skipped:"no-taskpackage-context"`（旗标被当路径） |
| 位置参数传 canonical 路径 | 与今日行为一致（进入真正对账），回显来源 = `位置参数/环境变量` | 同（不变） |
| `--bogus-probe`（未知旗标） | **非 0 退出 + usage 列出支持旗标集** | **exit 0 绿** + `skipped:"no-taskpackage-context"` |
| `--help` | **exit 0 + usage**（不是绿跳过，也不是报错） | **exit 0 绿** + `skipped:"no-taskpackage-context"` |
| `--self-check` | 全绿，既有 16 case 零回归 + 新增消费层 case（V1/V2 夹具）全绿 | 16/16 绿 |

**V5-C · 正负配对两条（钉住 fail-loud 不越界吃掉 N/A 语义，缺一不可）**

| # | 调用 | 预期（修后） | 修前实测 | 判别意义 |
|---|---|---|---|---|
| 负 | `change-manifest-check.mjs delivery/taskpackages/no-such-pkg/taskpackage.json`（**显式路径打错**） | **非 0**，报错点名收到的路径与期望形态 | **exit 0 绿** + `skipped:"no-taskpackage-context"` | 派单写错包名不再静默假绿——本批方向变更的正面收益 |
| 正 | 夹具内**无 `current/`、无任何参数**（压根没给上下文） | **exit 0** + `skipped:"no-taskpackage-context"`（N/A 语义一字不改，与 V2 第三行同源互证） | 同（不变） | 防「fail-loud 过度扩张」把 human 跳流程例外的既有 N/A 通道一起改红 |

反转要求：把 fail-loud 分支改回「一律 N/A 绿跳过」→ V5-B 的 `--bogus-probe` / `--help` 两行与 V5-C 负例全部变红；把 N/A 分支也改成 fail-loud → V5-C 正例与 V2 第三行变红。两个方向都必须实测过并记入实施记录。

### V6 · 真仓不回归普查（机械可执行，防「加宽 = 到处变绿」与「锚定 = 到处变红」）

- **T2 假红面清零**：对 §2.3 列出的 5 个今天假红文件逐一判定，修后必须全部识别为有断言（预期 5/5）。语料口径（须照抄以便复跑）= `git ls-files firmware server` 过滤扩展名 `.c|.h|.cc|.cpp|.py`，判据 = 文件含 `TEST_ASSERT_*`/`TEST_FAIL*` 字样但修前 `hasRealAssertion` 为 false。命令形态 = 以临时 TaskPackage 夹具逐个指向该文件跑无参 CLI，或直接跑导出的识别函数——**二者至少其一必须是 CLI 形态**（V4 已覆盖 CLI，本条可函数级，但须记明形态）。
- **T2 零翻红**：语料口径 = `git ls-files firmware server tools` 过滤扩展名 `.c|.h|.cc|.cpp|.py|.js|.mjs|.ts`（本日起稿实测该语料下修前判为「有断言」者 **154 件**）；对这 154 件跑修后识别面，翻红数必须为 **0**。本设计已实测 (b) 剥离变体的翻红面为 **2 件**（`firmware/main/display/points_page.c`、`tools/simulator/LvglWindowsSimulator/lv_conf.h`），故 (b) 不在本批；若 reviewer 裁定取 (b)，本条预期改为 2 并须在批记录点名该两处。
- **T1 零历史改写**：`git status` 确认 `delivery/taskpackages/**/verifier-evidence/**` 零改动（历史只读）；`.claude/verify-gates.json`、`.claude/scripts/taskpackage-required-check.mjs`、`.claude/scripts/lib/current-pointer-shared.mjs` 三件零改动。
- **卫生**：两件实现文件 `node --check` 通过；`node .claude/scripts/route-regression-check.mjs --self-check` 与 `node .claude/scripts/lint-descriptions.mjs`（若适用）零新增 flagged，沿用 P9 批 §9 条 7 的收口习惯。

### V7 · 范围与不自评

实施记录须列实际 diff 文件清单并逐件对照 §8 manifest（实际非证据 diff ⊆ 预计 scope；出 scope 必须回 reviewer，作者不得自扩自过）；reviewer 另核意图保真、判别力反转实证、消费方零破损。本设计不自评通过、不预判档位。

## 7. 开放问题（OQ）裁定留痕与判档评估事实

> 三条 OQ 均已裁定（OQ-1 = human，OQ-2 / OQ-3 = architect-reviewer R1）。本节保留原始甲乙对照与全部实测依据作为**决策留痕**，不删除；裁定结果已回灌 §1 / §3 / §4 / §5 / §6 / §8。

### OQ-1 · change-manifest 的旗标面与 fail-open 跳过是否本批一并收口？——**已裁：human 2026-11-21 裁定 = 甲（本批一并收口）**

**裁定**：human 2026-11-21 一句裁定取**甲**。**绿→红方向变更已获 human 授权**（范围 = `change-manifest-check` 对「显式给了上下文但不可解析」与「未知 `--` 旗标」两类误用由 `exit 0` 假绿改为 fail-loud 非 0）；「压根没给上下文」的 N/A 语义与 pre-cutoff 存量放行语义**不在授权范围内、一字不改**。落点 = §1 目标 T1 旗标面收口条、§3.2 三形态必落、§3.3 三分支表、§5 末条如实声明；验收 = §6 V5-B（必跑）+ V5-C（正负配对）。reviewer R1 原判「升级 human，不替人裁」，本条即该升级的回收。

原始选项对照（保留作为决策依据，逐字未删）：

- **背景**：§2.2 实测该闸对「未知旗标 / 包名打错 / `--help`」三种误用一律 `ok:true` + exit 0（绿），而同族 `acceptance-coverage` 报错误话（红）、`mutation-check` exit 2（fail-loud）。三闸三方向，属本批主题「闸说假话」的第三个实例，且与 T1 同源（同一个 `skipped` 黑箱早退分支）。
- **选项甲（human 所取，原为设计推荐）**：本批一并收口——补 `--change-id`、未知旗标非 0、`--help` exit 0，并把「显式给了上下文但不可解析」与「压根没有上下文」分开：后者保留 `skipped:'no-taskpackage-context'`（human 跳流程例外的既有语义），前者 fail-loud 非 0。**代价**：这是**绿→红方向变更**，超出「不改任何 blocking 判据语义」的批边界字面，须 reviewer 认可为「T1 让既有闸真正生效」的同一意图范围内，或升级 human 一句确认（已确认）。**收益**：两闸旗标面一次对齐（§4.3），派单写错不再静默假绿。
- **选项乙（未取）**：本批只修 cutoff 基准 + 回显，旗标面只动 acceptance-coverage 侧；change-manifest 的旗标缺口与 fail-open 跳过留 ARQ（主线程登记）。**代价**：§4.3 的「两闸一致」本批达不成，且 T1 修好后，误用旗标仍会静默绿（假绿面缩小但未闭合）。
- **为何该由人定**：这是「批边界字面（不改 blocking 判据语义）」与「批主题实质（闸不得说假话）」的取舍，不是技术事实问题；起草时本 agent 不自行扩面，reviewer 亦不替人裁。

### OQ-2 · Unity 识别面的 prose 负例强度取哪个变体？——**已裁：architect-reviewer R1 裁定 = (c) 行首锚定 + 显式容纳 `#define` 行**

**裁定**：reviewer R1 取 **(c)**，属 reviewer 可直裁（不触既有门语义）。裁定已回灌 §4.1（四条行为契约全为必落项）与 §6 V4（case 3 = `#define` 宏包装正例、case 4/5 = prose 负例保持红）。R1 另要求补记的**前向边界诚实注记**（行首锚定会让「仅以行中调用承载断言」的未来验收件假红，当日 0 例）已写入 §4.1 条 3；NIT-4 的已知宽松面（容纳 `#define` 行会把「仅定义宏而不调用」的 header 认作有断言）已写入 §4.1 条 2。

原始对照与实测依据（保留作为决策留痕）：

- **背景**：§2.3 表列出 (a) 无锚定 / (c) 行首锚定 + 容纳 `#define` 行 / (b) 全模式剥离注释字符串三变体，实测差异已量化：(a) 与 (c) 在当日语料上只差 1 个文件（`red-baseline/unity.h`，且从未被任何包列为验收件）；(a) 会让「注释 / 字符串内带括号字样」两类合成负例转绿（新增 fail-open 面，但与 `:96-100` 既有已声明限制同类）；(c) 保住两类负例且不漏宏包装件；(b) 触既有门语义、实测翻红 2 个跟踪文件、按本文件注记须另批 + human。
- **设计原推荐 = (c)**，理由与 reviewer 裁定一致：本批主题是「闸不说假话」，(a) 在修一个假红的同时新开一个假绿方向，与批主题相悖；(c) 沿用本文件 `:83` 已有的行首锚定先例（其注释明写「避 prose 误报」），且对当日 4800 处调用形态零漏判（4722 行首 + 78 `#define`，R1 逐行复核成立）。**代价**：识别逻辑比 (a) 多一条预处理行例外，需在自检里钉住（V4 case 3 即该判别点）。
- **(b) 不取**：触既有门语义（`:96-100` 明写须另批 + human），实测翻红面 2 件。

### OQ-3 · `taskpackage-authoring.md:38` 的行号引用处置——**已裁：architect-reviewer R1 裁定 = 采纳符号名形态**

**裁定**：reviewer R1 采纳**符号名形态**（改指 `hasCoverageMarker`，不引行号），沿用 `governance-gate-shared.mjs:47` 先例；该句「一标一 id」的规范内容不动。已回灌 §5 牵连面条与 §8 manifest 第 3 行（该行由「OQ-3 若裁不搭车则删」转为**确定保留**）。R1 另记 NIT-1：已归档 `docs/debt/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md:24` 引 `:47-50` 同样会漂移，但归档件 append-only、历史只读、按 §1 非目标不追溯，**本批不修**，仅记录以免实施时误判为漏列。

### 判档评估事实（只给事实，判档归 architect-reviewer；R1 已判「常规 + 轻条件加强」）

- 支持 low / 常规：两件实现文件均 ordinary、非条件加强核心落点清单成员；零保护面落地；T1 的实质改动是「换喂给既有谓词的路径」，谓词、日期值、对账面、glob 判据全不动，单点可回滚；T2 是识别面加宽 + CLI 旗标面，两者皆有同仓完整先例（`mutation-check.mjs`）。R1 另核出强证据：`change-manifest-check.mjs:32-42` 的 `gitFirstCommitDate` 与 `taskpackage-required-check.mjs:108-118` **逐字相同**，而姊妹闸早已把同一函数喂给目录形态并在线生效——T1 = 复用姊妹闸已验证的调用形态，非工程新险。
- 可能支持加强的事实（如实列，不自判）：① T1 **激活一个休眠的 fail-closed 阻塞闸**，影响面 = 58 个 post-cutoff 包今后的每一轮 verify，方向为「更多红」，红后处置依赖既有扩单通道是否顺畅（若扩单通道有摩擦，会以 dev lane 阻塞形态显现，而非本批回滚）；② 本批验收标准包含**新的夹具技术**（`GIT_COMMITTER_DATE` 回填日期构造跨 cutoff 两侧的历史），全仓既有夹具无先例（R1 实测 `.claude/scripts` 内 `GIT_COMMITTER_DATE` 命中 0 件、`commit --date` 命中 0 件；`git init` 夹具则有先例 = `landing-fidelity-check.mjs` 的 `makeGitFixtureRepo`），其正确性依赖 §6 V1 的「夹具自证」硬条件，若该条件被实施时省略则判别力静默归零（起草期已实测踩过一次）；③ OQ-1 甲案的绿→红方向变更**已获 human 授权**（2026-11-21），不再是待裁风险，但仍是本批唯一一处 blocking 方向变更，须按 §6 V5-B / V5-C 双向反转实测钉住；④ R1 已据此判「轻条件加强（实现评审侧）」= reviewer 亲自 spawn 真实无参 CLI 复跑 V1 / V4 与 V3 真仓前后对比、亲核 V1 夹具三基准自证，不接受作者自证（§9 SHOULD-1），本设计同意并已在 §1 判别力条与 §6 V7 呼应。
- 两 T 语义上不耦合（不同脚本、不同失败方向），但同属「闸输出不可信」一根；本批合批的根据 = 两条 ARQ 的 trigger 均为「下一个动该脚本的治理批」，且判别力教训同源（P8/P9 的消费层 vs 判据层）。R1 未要求拆批；若今后需要拆，§3 与 §4 各自带完整验收段，可无损拆分。

## 8. 轻量 landing-manifest

**manifest 不变：仍为 8 件**（3 件 architect + 5 件 main-thread，全 ordinary，零保护面）。OQ-1 甲案的旗标面收口落在**已在册**的 `.claude/scripts/change-manifest-check.mjs` 内，OQ-2 / OQ-3 的裁定落在**已在册**的 `.claude/scripts/acceptance-coverage-check.mjs` 与 `.claude/references/taskpackage-authoring.md` 内，**未新增任何文件**；`.claude/verify-gates.json`（保护面）与 `lib/current-pointer-shared.mjs`、`taskpackage-required-check.mjs` 仍零改动（§5、§6 V6 钉）。仅第 1 / 3 行的理由文本按裁定更新。

```landing-manifest
.claude/scripts/change-manifest-check.mjs | architect | ordinary | T1：cutoff 基准改包目录首提日、三形态取 changeId、skipped 去黑箱回显、旗标面收口（--change-id / 未知旗标 fail-loud / --help / 显式上下文不可解析 fail-loud，human 2026-11-21 授权的方向变更）、消费层夹具自检
.claude/scripts/acceptance-coverage-check.mjs | architect | ordinary | T2：Unity 识别面加宽（OQ-2 裁定 = (c) 行首锚定 + 容 #define 行；含 prose 负例与宏包装件）、--change-id / 未知旗标 fail-loud / --help、CLI 层自检
.claude/references/taskpackage-authoring.md | architect | ordinary | 牵连面：:38 对 hasCoverageMarker 的行号引用改符号名（OQ-3 裁定采纳；T2 插入旗标解析必致行号漂移）
docs/debt/gov/ARQ-261117-change-manifest-cutoff-projection-bypass.md | main-thread | ordinary | 批记账：T1 结项或范围追注（含 OQ-1 = 甲 的 human 裁定留痕、行号引用按符号名口径更新）
docs/debt/gov/ARQ-261117-acceptance-coverage-assertion-and-flag-gaps.md | main-thread | ordinary | 批记账：T2 结项或范围追注（含 OQ-2 = (c) 裁定留痕、行号引用更新）
docs/debt/gov/INDEX.md | main-thread | ordinary | 生成件：debt-render.mjs 重跑（状态变更投影，勿手改）
docs/debt/gov/_archive/2026/ | main-thread | ordinary | 生成件：条目结项时渲染器自动归位两件 + 重生成该年 INDEX（未结项则本行不动）
delivery/workstreams/gov-debt-reduction/task_plan.md | main-thread | ordinary | 批记账：本波（验证闸说假话簇 批一）追注与收口记录
```

## 9. 设计评审记录（architect-reviewer）

### R1 - 2026-11-20 - architect-reviewer

verdict: GREEN

本批为治理面 `/gov-change` 常规 direct-implementation（纯 `.claude/scripts/**` 面），头部 `gov-flow` 标记已在第 4 行。R1 全面审，四查与判档均亲核，结论：设计无 BLOCKING 缺陷，给 GREEN；OQ-1 一项涉及绿→红方向变更与超出两条 ARQ 的扩面，按升级线交 human，不替人裁。落地前不冻结未来字节的纪律照旧。

#### 判档：常规 direct-implementation + 轻条件加强（实现评审侧）

事实依据（逐项亲核，非路径推导）：

1. **非核心落点**：`gov-change.md` 列的核心落点六件 = 前置升级闸、共享闸库、merge-driver、deny-main-push、enforce-scope、INDEX landed 推导。两件实现文件（`change-manifest-check.mjs`、`acceptance-coverage-check.mjs`）均不在其中；实测两文件只 import `lib/current-pointer-shared.mjs`（与 `lib/acceptance-id-shared.mjs`），本批不改这两个库，故不构成共享闸库/共享解析谓词改动。
2. **零保护面落地**：`.claude/verify-gates.json` 注册形态（无参 `node .claude/scripts/<x>.mjs`）与 `timing:scoped` 均无需变，本批不动它；不触 scope-map/settings/agent 定义/指令根。保护面零改，故无需主线程代落地。
3. **低风险、单点可回滚**：两件各自单脚本、单点 revert；T1 的 58 包影响是**今后 dev-lane verify 轮次**的「更多红」，gov lane 本批自身不跑 `verify.mjs`，且红后走既有「停手→planner 派 architect 扩单」通道，非卡死合并，不构成「高影响难回滚」。
4. **需要加强的真实事实**（这是为何给「轻条件加强」而非纯常规）：① T1 激活一条休眠的 fail-closed 阻塞闸（方向 = 更多红），其正确性依赖全仓**无先例**的夹具技术——实测 `.claude/scripts` 内 `GIT_COMMITTER_DATE` 命中 0 件、`commit --date` 命中 0 件，`git init` 夹具则已有先例（`landing-fidelity-check.mjs` 的 `makeGitFixtureRepo`）；② P8/P9 教训是「自检电池全绿而消费层带洞」，判别力必须落在真实无参 CLI / `verify.mjs` 消费层，作者自证不足以采信。→ 条件加强 = **实现评审阶段由本 reviewer 亲自 spawn 真实无参 CLI 复跑 V1/V4 判别、V3 真仓前后对比，并亲核 V1 夹具的「三基准分处 cutoff 两侧」自证条件成立**；不接受作者实施记录的自证作为判别力凭据，实施记录须附反转动作的实测红。
5. 不启用独立 landing-verifier：本批非核心合并/权限自改、非高影响难回滚，上述 reviewer 侧复跑已足以承担判别力核验；若实现评审出现实质性争议，再按 `gov-change.md` 派第二独立核验。

#### 四查

**一、意图保真**：批主题「验证闸说假话簇（批一）」与两条 ARQ 逐字对得上——T1 的 fail-open 假绿（MAJOR）、T2 的 Unity 假红 + 旗标面误用（SHOULD）均为既定触发（「下一个动该脚本的治理批」），human 已拍启动，设计只做落点与判别力，不重开取舍，与 ARQ 意图一致。未见起草者自补意图。

**二、纪律合规**：两件实现文件 + `.claude/references/taskpackage-authoring.md` 均在 architect 的 `scope-map.json` allow 内（实测 allow 含 `.claude/scripts/**`、`.claude/references/**`），不在 deny（deny = scope-map/settings/AGENTS/CLAUDE/DEV-GOVERNANCE/自身与 review-loop agent 定义/implementation-review.json）。5 件记账件标 `main-thread`（ARQ 两件、`docs/debt/gov/INDEX.md`、`_archive/2026/`、`task_plan.md`）——architect 的 allow 不含 `docs/debt/**`，标 main-thread 正确。轻量 manifest 围栏块与 `gov-flow` 标记在场，8 行字段齐全（路径｜architect/main-thread｜ordinary｜理由），`_archive/2026/` 为目录条目（gov-change 允许目录）。零契约面、零 G1/G3。

**三、撞车复核**：(a) 无机械消费方解析 `change-manifest` 的 `skipped`/`pre-cutoff` 字面——实测 `.claude` 内 `pre-cutoff` 字样只散落在各脚本各自的 `CHANGE_ID_CUTOFF` 自留 carve-out 注释里（`supervisor-gate-check.mjs:300-304`、`changes-index.mjs:700`、`closeout-check.mjs:9/279`、`dispatch-stamp.mjs:166/293`、`loop-health-report.mjs:219`），无一在解析 change-manifest 的 stdout；`change-manifest` 字面只出现在其自身与 `taskpackage-required-check.mjs` 的注释里。(b) `verify-gates.json _doc:66` 声称「与 taskpackage-required 共享同一 changeId cutoff 谓词」——修前两者基准其实不同（文件投影 vs 包目录），修后**更贴近**该句，无需改 `_doc`（且它是保护面）。(c) 无 spec/contract/ADR 与本改冲突。T1 与姊妹闸同基准的可行性另有强证据：实测 `change-manifest-check.mjs:32-42` 的 `gitFirstCommitDate` 与 `taskpackage-required-check.mjs:108-118` **逐字相同**，而姊妹闸早已把同一函数喂给**目录** `delivery/taskpackages/${cid}`（`:148/:172/:422-423`）并在线生效——T1 的「换喂给既有谓词的路径」= 复用姊妹闸已验证的调用形态，非工程新险。

**四、牵连面完整**：行号引用漂移普查实测——`acceptance-coverage-check.mjs:<line>` 与 `change-manifest-check.mjs:<line>` 的引用共落在四处：`taskpackage-authoring.md:38`（引 `:48` = `hasCoverageMarker`，T2 在其前的 `taskPackagePath()` 插入旗标解析必致漂移，已被 OQ-3 捕获并入 manifest）、两条在飞 ARQ（已列 main-thread 记账行，含「行号引用更新」）、以及**一份已归档** `docs/debt/gov/_archive/2026/ARQ-260907...md:24`（引 `:47-50` = `hasCoverageMarker`）。归档件 append-only、历史只读，按 §1 非目标「不追溯」口径不修，属可接受漂移（见 NIT-1），非漏列 live 件。其余牵连面：`lib/current-pointer-shared.mjs` 实测 15 个 import 方（3 钩子 + 12 脚本），本批不改库、只消费已返回的 `.changeId`；`verify-gates.json` 无参注册形态不变；`delivery/review/**` 本批目录 + `INDEX.md` + `.changes-index-state.json` 由 `isEvidenceTailPath`（`governance-gate-shared.mjs:121-126`）豁免不入 manifest。均无破损。

#### 数值亲核留痕（抽查 §2 量化，全部复核成立）

- git 首提日（`git log --diff-filter=A --follow --format=%cs`，本仓 2026-11-20）：`current/taskpackage.json`=2026-09-11、`current` 目录=2026-09-15、`261104-k5` 包目录=2026-11-04、`261104-k2` 包目录=2026-11-04——与 §2.1 表逐格一致。
- 历史证据面：`grep -rl "gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF" delivery/taskpackages/*/verifier-evidence` = **200 份文件 / 53 个包**，与 §2.1 一致。
- 包数与 post-cutoff 数：有 canonical `taskpackage.json` 的包（排除 `current/`）=104；包目录首提 ≥ 2026-09-28 者 =**58**，与 §2.1 一致。
- 无参 vs 显式实跑：无参 → `changeId=261104-k5-story-cover-nav` / `skipped:"pre-cutoff"` / `gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28 → 存量放行` / exit 0；显式 k5 路径 → `ok:true` / `manifestSize:29` / `reconciledFaceCount:0` / exit 0——与 §2.1、V3 一致。
- §2.2 误用三方向：`change-manifest --change-id <k5>` → `skipped:"no-taskpackage-context"` exit 0；`--help` → exit 0；`acceptance-coverage --change-id <k5>` → exit 1 `TaskPackage file does not exist` / `taskPackage:"<repo>/--change-id"`；`mutation-check --bogus` → `未知旗标：--bogus` exit 2——逐条复现一致。
- V4/hasRealAssertion 正则：`TEST_ASSERT_EQUAL_MESSAGE(...)`/`TEST_ASSERT_TRUE(ok)`/`TEST_FAIL_MESSAGE(...)`/`TEST_ASSERT_EQUAL_MEMORY`/`TEST_ASSERT_NULL` 全 false；libc `assert(p != NULL)` true——与 §2.3 一致。
- Unity 语料普查（`git ls-files firmware server`）：word 边界 token `\bTEST_ASSERT\w*|\bTEST_FAIL\w*` = **4804**；token 紧跟 `(` 的调用形态 = **4800**；落 `#define` 行的调用形态 = **78**；调用形态落在 `//`、块注释 `*`、双引号字符串内 = **0 / 0 / 0**——与 §2.3 全数一致。逐行核：4800 调用形态中行首锚定 = 4722、`#define` 行 = 78、纯行中等 = 0，即 (c) 行首锚定 + 容 `#define` 行对 4800 处零漏判，自洽。
- 假红 5 文件：以真实 `hasRealAssertion` 逻辑扫 `firmware server` 全部含 Unity token 的文件，`false` 者恰 **5** 个 = `happy_flow_capture.test.c`、`reminder_page.test.c`、`red-baseline/unity.h`、`char_window.test.c`、`story_page_v2.test.c`——与 §2.3 逐名一致；其中仅 `unity.h` 行首调用数=0（其余 25/103/15/75），故 (a)/(c) 差异恰此 1 件成立。
- `unity.h` 是否被当验收件：扫 104 包 `acceptanceTestsFile` 命中 `red-baseline/unity.h`=**0**（其唯一提及在 `261016-p3-story-body-sd-cache` 的 `changeManifest`，非验收件）——§2.3「从未被引用」成立。
- V6 宽语料：`firmware server tools`（`.c|.h|.cc|.cpp|.py|.js|.mjs|.ts`）修前判「有断言」= **154** 件，与 §6 一致；(b) 剥离变体会翻红的 2 件机制成立（`points_page.c` 仅靠 `#include <assert.h>` 的 `assert.` 命中 `\bassert\s*[.(]`、`lv_conf.h` 仅靠注释「assert.」命中，剥离注释/预处理即翻红）。
- `--change-id` 普及度：实际解析该旗标的机器面 = 9 件（脚本 7 + 钩子 2，`verify.mjs:413`、`verifier-evidence-check.mjs:514` 经 `args['change-id']` 解析），`verify-gates.json` 注册的 10 闸中仅 `mutation-check` 支持——与 §2.3、§4.3 一致。
- 基线 self-check：`change-manifest --self-check` = total 16 / failed 0；`acceptance-coverage --self-check` = 5/5 绿——与 §6 一致。

#### OQ 逐条裁

- **OQ-1（change-manifest 旗标面 + fail-open 跳过是否本批收口）**：**升级 human。** 事实：选项甲含**绿→红**方向变更（`--change-id`/`--help`/包名打错的显式路径，当前一律 `exit 0` 绿，甲改为非 0），且 §1 非目标与 §5「不改任何 blocking 判据语义」已把它逐字排除在批边界外；它只部分落在 ARQ-261117-acceptance-coverage #③「两个闸未知旗标 fail-loud」范围内——「显式路径打错→红」这一支不属「未知旗标」。这属「新增方向/新取舍」，按 `gov-change.md` 既定 ARQ 只做一句启动、新取舍回 human，由人裁，我不替人选。给 human 的具体问题（背景→现象→代价）：**change-manifest 对「未知旗标 / 包名打错 / `--help`」三种误用现在都静默报绿（exit 0），是否允许本批把它改成 fail-loud 非 0？** 甲 = 收口（派单写错不再静默假绿，代价是绿→红方向变更、超出两条 ARQ 核心范围）；乙 = 本批不动 change-manifest 旗标面、留 ARQ 由主线程登记（代价=假绿面缩小但未闭合）。**乙 是本批范围内默认，可无人类额外确认直接落地；甲 需 human 一句批准。** 设计本身已把两方向的后果摊开，裁定不阻塞 T1 核心（cutoff 基准修正）与 T2。
- **OQ-2（Unity 识别面 prose 负例强度）**：**裁定取 (c)（行首锚定 + 显式容 `#define` 行）。** 依据：亲核实测 (c) 对 4800 处调用形态零漏判（4722 行首 + 78 `#define`），5 个假红文件全部转绿（`unity.h` 靠 `#define` 行识别，其余靠行首调用），同时保住 V4 case 4/5（注释/字符串内带括号字样保持红），不新开 fail-open 面；(b) 触既有门语义（`:96-100` 明写须另批 + human，且实测翻红 2 个跟踪文件），不取；(a) 新开假绿面与批主题相悖。此裁定不触既有门语义，属 reviewer 可直裁。**前向边界须一句诚实注记**：行首锚定会让「仅以行中调用承载断言」的**未来**验收件假红（当日 0 例，且与本文件 `:83` 既有 libc 裸 `assert` 行首锚定先例一致），建议 §4.1 补一句、保留这一边界意识；不必升级 human。
- **OQ-3（`taskpackage-authoring.md:38` 行号引用）**：**裁定采纳符号名形态（改指 `hasCoverageMarker`）。** 亲核：`:38` 现引 `acceptance-coverage-check.mjs:48`，而 T2 在 `taskPackagePath()`（`:22-28`）插入旗标解析在该行之前，必致 `:48` 漂移；`governance-gate-shared.mjs:47` 有「按符号名引用，不引行号——同批删常量后行号即漂」先例可循。该文件在 architect allow 内、已入 manifest，改符号名一次消除复发面。该句「一标一 id」的规范内容不动。

#### Findings（均非 BLOCKING；GREEN 前提为按上述 OQ 裁定实施）

- **SHOULD-1（实现评审加强，已并入判档）**：判别力证据必须来自 reviewer 亲自 spawn 的真实无参 CLI（V1/V4）与 V3 真仓前后对比，并亲核 V1 夹具自证（三基准——投影文件 < cutoff、包目录 ≥ cutoff——分处两侧），不得以作者实施记录自证替代；实施记录须附反转动作的实测红。这是 P8/P9 教训的直接闭合项，非可选项。
- **NIT-1（牵连面备注，不要求本批修）**：已归档 `docs/debt/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md:24` 引 `acceptance-coverage-check.mjs:47-50`，T2 插入旗标解析后该行号将漂移。归档件 append-only、历史只读、按 §1 非目标不追溯，故不修，此处记录以免实施时误判为漏列。
- **NIT-2（措辞）**：§5 写「`delivery/review/**` 由 `isEvidenceTailPath` 豁免」，实测豁免面 = 本批目录 `delivery/review/<changeId>/` + `delivery/review/INDEX.md` + `delivery/review/.changes-index-state.json` 三项（不是任意 `delivery/review/**` 他批目录）。对本批 manifest 无影响，仅建议措辞收窄为「本批目录 + INDEX + state」。
- **NIT-3（措辞）**：V1 称「与 `verify.mjs:271-272` 逐字同形」略松——V1 用 `spawnSync(process.execPath, [<abs script>])`（数组参数、无 `shell:true`），`verify.mjs:271-272` 是 `spawnSync(cmd, { cwd: projectDir, shell:true })`（cmd 为 `node .claude/scripts/<x>.mjs` 字符串）。实质消费层属性（零位置参数、`cwd` 与 `CLAUDE_PROJECT_DIR` 同指项目根、走闸自身 argv 解析与 git 调用）一致，判别力不受影响；建议表述为「同形态消费层调用」而非「逐字同形」。
- **NIT-4（诚实注记）**：variant (c) 的 `#define` 容行会把「仅定义 `TEST_ASSERT_*` 宏而不调用」的 header（如 `red-baseline/unity.h` 形态）认作「有断言」——这是行为契约 #2 的刻意选择（避免 unity.h 假红），且该文件从未被任何包用作 `acceptanceTestsFile`（实测 0 命中），属已知宽松面，如实记录即可，无需收紧。

（本轮为设计审 R1；实施后在真实 diff 上另出实现评审，届时以行首 `reviewedCommit:` / `reviewedTree:` 钉点，缺件或后出 BLOCK 均不放行。）

### R2 - 2026-11-21 - architect-reviewer

verdict: GREEN

本轮为设计审 R2（聚焦复审，只核 OQ-1=甲 的回灌与授权边界，不做全量重核）。PROPOSAL 现为 DRAFT v0.2。结论：human 对 OQ-1 的裁定（甲）已如实回灌、方向变更的授权边界写死、未夹带新设计、V5 判别力与 manifest 齐备，给 GREEN；无剩余升级 human 项。

#### 逐项核（5 点）

**1. OQ-1=甲 回灌到位、无夹带**：确认。§1 目标增「T1 · 旗标面收口」四条（①②③④），并标「②④ 是本批唯一一处 blocking 判据方向变更（绿→红），已获 human 授权」（`:14`）；§3.2 三形态升必落（`--change-id` 优先级最高，无参形态仍是判别主落点、旗标面不得改变无参判定，`:152`）；§3.3 改三分支表（无上下文 N/A 不变／显式上下文不可解析 fail-loud 非 0／未知旗标非 0）；§4.2 acceptance-coverage 旗标面并明记「本闸不涉绿→红，方向变更只在 change-manifest 侧」（`:196`）；§4.3 两闸同一旗标集、仅 change-manifest 绿转红的差异写明；§5 末条把旧「不改任何 blocking 判据语义」如实改写为「一处经 human 授权的方向变更 + 其余一律不变」；§6 V5 新设 V5-B（必跑）+ V5-C（正负配对）；§7 OQ-1 改为「已裁=甲」并保留原始甲乙对照；§8 manifest 理由文本更新。逐条比对未见任何超出裁定范围的新机制——是回灌，不是新设计。

**2. 授权边界写死**：到位。授权面恰为两类——§3.3 表第二行「显式给了上下文但不可解析」、第三行「未知 `--` 旗标」；§3.3 表第一行「压根没给上下文」保留 `skipped:'no-taskpackage-context'`+exit 0「一字不改」，§3.4 与 §5 末条重复钉「pre-cutoff 放行 + 无上下文 N/A 均一字不改」。真实闸链不受影响有据：`verify-gates.json` 注册形态无参，`verify.mjs:271-272` 原样 spawn、不追加旗标；且全仓（含 `.claude` 与脚本）实测无任何 `DEV_LOOP_TASKPACKAGE`/`TASKPACKAGE_PATH` 的赋值/导出，故 spawn 继承的 env 不含显式上下文，闸恒走无参+current 回退（fail-loud 两分支只在调用方显式传参/传错旗标时触发）。§3.3 boundary bullet 2 另把「changeId 不可得（JSON 可读但缺字段 / 包目录未提交致首提日 null）」显式排除在 fail-loud 外、归入 T1 的 post-cutoff 对账（属 ARQ MAJOR 已授权方向，非 OQ-1 绿转红），避免把方向变更扩到「缺字段」面。

**3. V5 判别力**：到位。V5-B（change-manifest）升为「必跑验收」，五条（`--change-id` / 位置参数 / `--bogus-probe` / `--help` / `--self-check`）逐条列「修前实测红基线」——`--change-id`、`--bogus-probe`、`--help` 修前均 `exit 0 绿 + skipped:"no-taskpackage-context"`（与本 reviewer §2.2 复现一致）；V5-C 正负配对 = 负（显式路径打错→非 0）/ 正（无 current 无参数→仍 skipped+exit 0），反转要求双向齐全：把 fail-loud 改回绿跳过 → V5-B bogus/help 两行 + V5-C 负例变红；把 N/A 也改红 → V5-C 正例 + V2 第三行变红，两向均须实测（`:305`）。`--change-id` 行的判别靠「解析到包路径 + 回显来源=`--change-id 旗标`」与无参结论一致，未实装即回显/解析不符而红，具备判别力。

**4. §9 未改动**：确认。实测 §9（行 368-425）逐字与本 reviewer R1 原文一致（首行 `## 9.`、R1 `verdict: GREEN`、末行 R1 收尾括注皆逐字同）。

**5. manifest**：仍 8 件（3 architect：`change-manifest-check.mjs` / `acceptance-coverage-check.mjs` / `taskpackage-authoring.md`；5 main-thread：两条 ARQ / `docs/debt/gov/INDEX.md` / `_archive/2026/` / `task_plan.md`），全 ordinary、零保护面、未新增文件（OQ-1/2/3 落点全在已在册文件内，§8 末注 `:355`）。无漏列牵连件。

#### Findings

- 无 BLOCKING / SHOULD。
- **NIT-5（事实计数，建议改）**：§1 非目标（`:23`）与 §4.3（`:205`）称「其余 8 闸（`taskpackage-required` / `firmware-build` / `contract-lint` / `docs-sync` / `security` / `unit` / `acceptance` 等）不触」。`verify-gates.json` 注册闸共 **10** 件，本批后支持 `--change-id` 者 3 件（`mutation-check` / `change-manifest` / `acceptance-coverage`），未触者应为 **7** 件（正与所列 7 名相应）。此为 v0.1「10−2=8」旧计数在 OQ-1 转甲（基座新增 change-manifest）后未同步，应为 7。该措辞**不**把绿转红面读宽（它多算「不触」集合、只令变更面显得更小），故非 widening，纯计数笔误。
- **NIT-6（排版笔误，可一并修）**：§7 OQ-1 裁定段（`:329`）「（已确認）」用字「确認」为繁简混写，宜作「确认」。

（R2 聚焦复审完毕；本轮无新增升级 human 项——OQ-1 已由 human 2026-11-21 裁定回收。主线程可凭 R1+R2 GREEN 标 FROZEN；实施后本 reviewer 按 §9 SHOULD-1 亲自复跑消费层判别再钉 reviewedCommit/reviewedTree。）

## 10. 实施记录（architect · 2026-11-21，append-only）

**实现 commit**：`f09c19c6770adac565632b07363047158f35908f`（tree `1f109103f93c38843efd6980fd7d623d3f42d3aa`），分支 `gov/kidpal`，父 = `73f43637`（= `main`，merge-base 同点）。未 push、未 merge、未 rebase。

### 10.1 实际 diff 逐件对照 §8 manifest

`git diff --name-only main...f09c19c6` = **3 件**，件件在册、无 manifest 外文件：

| 实际改动 | manifest 行 | 归属/保护 | 一致 |
|---|---|---|---|
| `.claude/scripts/change-manifest-check.mjs`（+340/−18） | 第 1 行 | architect / ordinary | ✓ |
| `.claude/scripts/acceptance-coverage-check.mjs`（+205/−13） | 第 2 行 | architect / ordinary | ✓ |
| `.claude/references/taskpackage-authoring.md`（+1/−1） | 第 3 行 | architect / ordinary | ✓ |

manifest 第 4–8 行（5 件 main-thread 记账件：两条 ARQ、`docs/debt/gov/INDEX.md`、`_archive/2026/`、`task_plan.md`）**本 agent 未触碰**，按计划归主线程收口时处理；轻量 scope 语义允许计划内文件最终未改。`.codebase-memory/` 两件为工具自刷新常驻脏，未 stage、未提交（gov 分支禁私带快照 commit）。保护面零改动，实测见 §10.3 V6(c)/(d)。

### 10.2 落地的关键方案（对应 FROZEN §3 / §4）

- **T1① cutoff 基准**：新增导出纯函数 `cutoffBasis(changeId, gitFn = gitFirstCommitDate)`，基准路径 = `delivery/taskpackages/<changeId>`；changeId 取自已解析结果（`resolveCurrentTaskPackage` / `resolveExplicitTaskPackage` 返回的 `.changeId`，或 `--change-id` 旗标值）。**无任何路径几何回退**（changeId 不可得 → `kind:'unavailable'` + `firstCommitDate:null` → 调用方不跳过、进对账）。cutoff 日期值 `2026-09-28`、比较方向、`gitFirstCommitDate` 的 git 调用形态、对账面 `RECONCILE_FACE_RE`、glob 判据**均未改**。
- **T1② skipped 去黑箱**：`pre-cutoff` 跳过、对账成功、以及四类 `fail()` 输出全部携带 `cutoffBasis{kind,path,firstCommitDate,cutoff}` + `changeId` + `contextSource`（三分类如实标注）。`skipped` 两个字面值保留。
- **T1③ 旗标面（human 授权）**：`taskPackagePath()` 换为 `resolveContext()`，返回 `{path, changeId, found, explicit, source, received}`；入口分派顺序 = `--self-check` → `--help`/`-h`（exit 0 + usage）→ 未知 `--` 旗标（exit 2 + usage）→ `main()`（先例 `mutation-check.mjs:243-258`）。主流程按 `explicit` × `found` 分三支：`!explicit && !found` → 既有 `skipped:'no-taskpackage-context'` + exit 0（**一字未改**）；`explicit && !found` → `fail(...)` exit 1 并点名 `received` / `resolvedPath` / `hint`；其余进 cutoff 与对账。
- **T2① Unity 识别面**：新增模块级 `UNITY_ASSERT_RE = /(?:^|\n)[ \t]*(?:#[ \t]*define\b[^\n]*)?TEST_(?:ASSERT|FAIL)[A-Z0-9_]*[ \t]*\(/m`，作为 `hasRealAssertion` 的**第七支 `||` 追加**（OQ-2 裁定变体 (c)：行首锚定 + 容 `#define` 行）。既有六支模式逐字未动 → 结构上为单调加宽，不可能令任何文件由「有断言」翻为「无断言」（§10.3 V6(b) 已机械复证）。`isShallowStatusOnly` / `hasCoverageMarker` / 豁免面 / coverage 对账零改动；未引入注释字符串剥离（(b) 变体）。前向边界与已知宽松面按 R1 要求写入源码注记（`UNITY_ASSERT_RE` 上方 12 行）。
- **T2② 旗标面**：同 T1③ 的分派结构与 `--change-id` / 位置参数 / `current/` 三形态优先级；`echoResolution` 来源如实三分类。本闸修前对误用已 exit 1，故**无绿→红方向变更**。
- **牵连面**：`taskpackage-authoring.md` §6 那句的行号引用 `acceptance-coverage-check.mjs:48` → 符号名 `hasCoverageMarker`（OQ-3 裁定），该句规范内容一字未动。

### 10.3 自测结果（V1–V7 逐条；命令 + 实测输出摘要）

**V1 · T1 消费层判别（无参 CLI + 真 git 仓夹具）— PASS**
`node .claude/scripts/change-manifest-check.mjs --self-check` → **43/43，failed:0，exit 0**（16 条既有纯函数 case 预期逐字未改 + 6 条新纯函数 + 21 条消费层 CLI 夹具）。V1 四条断言全绿：退出码非 0 / stdout 无 `skipped` 键 / `outOfManifest` 点名 `firmware/new.c` / `cutoffBasis` = `package-dir` + 包目录首提日。
**夹具自证（FROZEN §6 V1 硬条件）已机械执行并作为断言**：`V1·夹具自证：current/ 投影首提日 < cutoff`、`V1·夹具自证：包目录首提日 ≥ cutoff`、`V1·夹具自证：两基准分处 cutoff 两侧（同侧 = 夹具无效）` 三条均为独立断言，不成立即自检红——即「夹具零判别力」这一 P8/P9 型失效在本批不可能静默通过。

**V2 · 不 over-red / 存量放行 / N/A / 基准不可得 — PASS（4/4）**
manifest 覆盖后 `exit 0 / ok:true / 无 skipped / reconciledFaceCount ≥ 1`；真存量包（包目录同回填）`exit 0 / skipped:"pre-cutoff"` 且回显基准为包目录 + 回填日期（该夹具的 `changeManifest: []`，若放行分支失效必红 → 强判别）；无上下文 `exit 0 / skipped:"no-taskpackage-context"`；包目录无提交历史（基准不可得）→ 不跳过、进对账并红、`cutoffBasis.firstCommitDate === null`。

**V3 · 真仓前后对比 — PASS（与 FROZEN §6 V3 预期逐项吻合）**
`node .claude/scripts/change-manifest-check.mjs`（无参，current → `261104-k5-story-cover-nav`）：**不再出现** `skipped:"pre-cutoff"`，输出 `ok:true / manifestSize:29 / reconciledFaceCount:0`，`cutoffBasis = {kind:"package-dir", path:"delivery/taskpackages/261104-k5-story-cover-nav", firstCommitDate:"2026-11-04", cutoff:"2026-09-28"}`，`contextSource:"current/ 指针"`。修前同命令实测 = `skipped:"pre-cutoff"` / `2026-09-11`（§2.1）。
三形态一致性：无参 / 显式 canonical 路径 / `--change-id 261104-k5-story-cover-nav` 三者对同一包的 cutoff 结论与对账结论**完全一致**（均 `manifestSize:29 / reconciledFaceCount:0 / basis 2026-11-04`），仅 `contextSource` 与 `taskpackage` 路径如实不同。ARQ 点名包 `261031-openclaw-cron-trigger-gating`（`--change-id`）→ `ok:true / 无 skipped / manifestSize:9 / basis 2026-10-31`。

**V4 · T2 消费层判别（CLI 入口，7 类）— PASS（8/8 CLI case）**
`node .claude/scripts/acceptance-coverage-check.mjs --self-check` → **28/28，fail:0，exit 0**（`assertionCases 14/14` + `cliCases 14/14`）。V4 表逐行：case1 纯 Unity 行首 → exit 0；case2 仅 `TEST_FAIL_MESSAGE` → exit 0；case3 仅 `#define` 宏包装 → exit 0；case4 注释内带括号字样 → **exit 1（保持红）**；case5 字符串内带括号字样 → **exit 1（保持红）**；case6 真无断言 → exit 1；case7a/7b Python 两件 → exit 0（零回归）。既有 5 条纯函数 case 名称与预期**逐字未改**，另加 9 条 Unity 纯函数正负例（含「仅行中调用」前向边界负例，钉住不得就地放宽锚定）。

**V5 · 旗标面（V5-A / V5-B / V5-C）— PASS**
V5-A（acceptance-coverage，CLI 夹具内）6 条全绿：`--change-id` 解析 canonical 且与无参同结论、回显来源 `--change-id 旗标`；位置参数兼容 + 回显 `位置参数/环境变量`；未知旗标非 0 + usage 且**不再出现** `"message": "TaskPackage file does not exist"`；`--help` exit 0 + usage；`--change-id no-such-pkg` 非 0 且点名。
V5-B（change-manifest，CLI 夹具内）5 条全绿，红基线对照见 §10.4。
V5-C 正负配对两条全绿：负例「显式路径打错」→ 非 0 且点名 `no-such-pkg`；正例「无上下文」→ `exit 0 + skipped:"no-taskpackage-context"`（N/A 语义未变）。
真仓补测（非夹具）：`change-manifest-check.mjs --bogus-probe` → **exit 2** + usage；`--help` / `-h` → **exit 0** + usage；打错包路径 → **exit 1** 并点名收到的值；`--change-id`（缺值）→ **exit 2**。`acceptance-coverage-check.mjs` 同四项分别为 exit 2 / exit 0 / exit 2。真包三形态（无参 / `--change-id` / 位置参数）结论一致：`ok=true`，`files=["firmware/main/test/story_cover_nav.acceptance.c","server/acceptance/test_story_cover_nav.acceptance.py"]`。

**V6 · 真仓不回归普查 — PASS（4/4）**
(a) **假红面清零（CLI 形态）**：§2.3 列出的 5 个今天假红文件逐个以「临时 TaskPackage 夹具 + 无参 CLI」实跑（`acceptance:[{id,kind:'structural'}]` 豁免 coverage 判定以隔离「有没有断言」这一问），**5/5 exit 0 且无 `no recognizable assertion`**——含 `firmware/main/test/acceptance/red-baseline/unity.h`（唯一只靠 `#define` 宏包装识别的件，实证 (c) 变体的 `#define` 容行为承重）。
(b) **零翻红（函数级形态，如实记明）**：语料 = `git ls-files firmware server tools` 过滤 `.c|.h|.cc|.cpp|.py|.js|.mjs|.ts` = 757 件；识别函数与 `UNITY_ASSERT_RE` 常量**从已落地源码抽取后 eval**（非手抄）。结果：修前判为有断言 **154 件**（与 FROZEN §6 记载一致）、修后新增识别 **恰 5 件**（= (a) 的同一 5 件，无意外新增）、**翻红 0 件**。
(c) **历史只读**：`git status --porcelain delivery/taskpackages/` 输出为空 → 53 包 / 200 份 verifier-evidence 零改动。
(d) **三件零改动**：`git status --porcelain .claude/verify-gates.json .claude/scripts/taskpackage-required-check.mjs .claude/scripts/lib/current-pointer-shared.mjs` 输出为空。
**卫生与消费方**：两件 `node --check` 通过；`node .claude/hooks/verify.mjs --self-check` → **18/18 ok**（两闸的共同消费方，hermetic tmpdir 沙箱）；`taskpackage-required-check --self-check` 23/23、`mutation-check --self-check` 14/14、`lib/current-pointer-shared` 7/7、`lib/acceptance-id-shared` 23/23、`lib/governance-gate-shared` 4/4 全绿；`route-regression-check --self-check` exit 0；`lint-descriptions` scanned 16 / flagged 0。

**V7 · 范围与不自评**：见 §10.1（3 件 ⊆ manifest 前三行）。本记录不自评通过；按 §9 SHOULD-1，判别力凭据以 reviewer 亲自 spawn 的消费层复跑为准。

### 10.4 反转判别实证（真做、真观察红；仓外临时副本，验后已删）

反转副本构造：临时目录内复制本闸 + 其 `lib/` 依赖（`current-pointer-shared.mjs`；acceptance-coverage 另需 `acceptance-id-shared.mjs`），对副本做定点文本替换后跑副本的 `--self-check`（副本自 spawn 自身，故判别落在同一消费层形态）。

**T1（三向，全部如预期变红）**

| 反转动作 | 结果 | 变红的断言 |
|---|---|---|
| ① 把 `cutoffBasis(ctx.changeId)` 换回 `gitFirstCommitDate(tpPath)`（= 恢复 current/ 投影基准，即修前 bug） | exit 1，43 中 **7 红** | V1 全部四条（退出码非 0 / 无 `skipped` / `outOfManifest` 点名 / 基准回显）+ V2 覆盖后绿 + V2 基准不可得 + V2 存量包基准回显 |
| ② 把 fail-loud 分支关掉、令任何 `!found` 都走 N/A 绿跳过（= 修前行为） | exit 1，**2 红** | V5-C 负例「显式路径打错 → 非 0」+「`--change-id` 指向不存在的包 → 非 0」 |
| ③ 把 N/A 分支也改成 fail-loud（过度扩张） | exit 1，**1 红** | V5-C 正例「无上下文 → exit 0 + `skipped:"no-taskpackage-context"`」 |

**T2（四向，全部如预期变红）**

| 反转动作 | 结果 | 变红的断言 |
|---|---|---|
| ① 删掉 `|| UNITY_ASSERT_RE.test(content)` 分支（= 修前） | exit 1，28 中 **9 红** | 4 条 Unity 纯函数正例 + V4-1/2/3 + V5-A 的 `--change-id` 与位置参数两条（其夹具验收件用 Unity 断言） |
| ② 换成无锚定变体 (a)（OQ-2 未取分支） | exit 1，**5 红** | `unity-comment-call-shaped-negative`、`unity-string-call-shaped-negative`、`unity-midline-only-negative-forward-boundary` + **V4-4 / V4-5**（两条 prose 负例转绿即被咬住） |
| ③ 行首锚定但去掉 `#define` 容行 | exit 1，**2 红** | `unity-define-wrapper-positive` + **V4-3**（`red-baseline/unity.h` 形态假红） |
| ④ 识别面恒真（过度放宽） | exit 1，**11 红** | 既有 3 条 prose/empty 负例 + 4 条 Unity 负例 + V4-4/5/6 |

**修前红基线的实证方式**：V4/V5 各表的「修前实测」列不是纸面推定——T2 反转①（删掉 Unity 分支）即修前状态，实测 9 红；T1 反转①②即修前状态，实测 7 红 + 2 红。另 §2.2 / §2.3 的真仓修前实测（`--change-id` / `--help` / 打错路径三者皆 `exit 0` 假绿；纯 Unity 件 `exit 1` 假红）为起草期直接观测值。

### 10.5 偏差与如实注记（6 条，均非 manifest 外改动）

1. **commit 署名行与本仓惯例不同**：本 commit 结尾为 `Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>`，而仓内近 40 个 commit 的惯例（含 P9 实现 commit `9534d71f`）与主线程派单要求均为 `Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>`。取值依据 = 本会话 harness 侧的署名指令（其明文规定只列出该行、且要求不添加它未列出的署名行），agent 派单不能覆盖。**如实上报，交主线程 / human 定夺**；若需统一为仓内惯例，`git commit --amend` 改署名即可，不影响任何代码内容与判别力（amend 会换 SHA，届时 reviewer 钉点须按新 SHA）。
2. **`--change-id` 缺值 → exit 2**（两闸同）：FROZEN §3.3 第三行只规定「不在支持集内的 `--` 旗标」非 0，未规定「已知旗标缺值」。实施按同批 fail-loud 精神补为 usage 错误 exit 2——否则该形态会静默回落 `current/` 指针、对**错包**跑闸（正是 `ARQ-260913-current-pointer-implicit-global` 的病灶形态）。属向严、不放宽任何判据；如实记为设计文本未明列的新增分支。
3. **自检标签改名**：`acceptance-coverage-check --self-check` 输出的 `selfCheck` 值由 `'acceptance-coverage-python-assert'` 改为 `'acceptance-coverage-assertion-and-flags'`（覆盖面已不止 Python 断言）。起草期实测全仓**无机械消费方**解析该标签；`ok` / `pass` / `fail` / `total` 四个键名保持不变，另加 `assertionCases` / `cliCases` 两个子计数与 `cliFailures` 明细。
4. **输出新增 `contextSource` 字段**：FROZEN §3.4 要求回显的是 cutoff 判据三元组 + cutoff 值 + changeId；实施另加 `contextSource`（`--change-id 旗标` / `位置参数/环境变量` / `current/ 指针`），与 `echoResolution` 的 stderr 三分类同源，属「去黑箱」同一意图的加字段（形状兼容，无机械消费方解析该键）。
5. **V4 case1/case2 夹具用行首形态**：FROZEN §2.3 的修前实测夹具把 Unity 断言写在行中（`void t(void){ TEST_ASSERT_...; }`），而 §6 V4 表 case1 明写「纯 Unity **行首**」。实施按 V4 表用行首形态（变体 (c) 的识别前提），并**另立一条纯函数负例** `unity-midline-only-negative-forward-boundary` 钉住行中形态当下不识别这一前向边界（§4.1 条 3）。两种形态修前均为红（反转①实证），故红基线不受此差异影响。
6. **两处夹具/断言自身缺陷在实施期被抓出并修正**（记录以免复犯）：① 首版夹具把 `current/taskpackage.json` 写成只含 `{changeId}` 的瘦投影，而无参形态下闸读的正是投影文件 → 判别落到「manifest 缺失」而非「清单外改动」，V1 的 `outOfManifest` 断言红；修正为 canonical 与 current/ **双写完整副本**（与真仓一致，`taskpackage-required-check.mjs:301-321` 本就强制两者散列一致）。② 「未知旗标不得再报错误话」的断言用裸字面 `TaskPackage file does not exist` 取反，而 usage 文本为解释病灶恰好引用了该短语 → 断言自相矛盾恒红；改为匹配 JSON 形态 `"message": "TaskPackage file does not exist"`。
7. **未做项（FROZEN 明列为可选）**：§6 V1 括注的「更强形态」——在夹具上跑通完整 `verify.mjs --phase=scoped` 闸链并取其 evidence 条目——**未执行**。理由：`verify.mjs` 会 `persistVerifyRuns` 落盘轮次证据，在夹具外有写入副作用，且 FROZEN 已明写「跑不通不阻验收，无参 CLI 同形已足够」；消费层属性已由无参 spawn（零位置参数 + `cwd` 与 `CLAUDE_PROJECT_DIR` 同指夹具根）承担，并以 `verify.mjs --self-check` 18/18 佐证消费方未破损。

## 11. 实现评审记录（architect-reviewer）

### R1 实现评审 - 2026-11-21 - architect-reviewer

verdict: GREEN

本轮为真实 diff 实现评审（按 §9 SHOULD-1 判档：reviewer 亲自 spawn 真实无参 CLI 复跑 V1/V4/V3/V5，不采信作者自证）。审对象 = `main...1c19f794`（实现 commit `f09c19c6` + 批记录 `d22e5636` + 主线程记账 `1c19f794`）。结论：实现与 FROZEN v0.2 意图/边界/验收一致，判别力经本 reviewer **独立夹具**（非作者自证）实证成立，scope 与记账事实吻合，给 GREEN。

**钉点取舍（重要）**：`reviewedCommit` 取 `1c19f794`（本批最后一个**非证据**提交）而非 `f09c19c6`。理由 = 前置升级闸 `collectDirectImplementationGate`（`pre-merge-escalation-gate.mjs:157/165-166`）要求 `reviewedCommit..source` 只剩证据尾改动；`1c19f794` 记账件（两条 ARQ 归位 `_archive/2026/`、`docs/debt/gov/INDEX.md`、`task_plan.md`）属**非证据尾**，若把 `f09c19c6` 当 reviewedCommit 会令 `f09c19c6..source` 含这批非证据改动而 BLOCK。故钉在 `1c19f794`，本 reviewer 已连同记账件一并审（见第四节）。

#### 一、scope（实际非证据 diff ⊆ §8 manifest）

`git diff --name-only main...1c19f794` = **11 件**，逐件归位：3 件实现面（`change-manifest-check.mjs` / `acceptance-coverage-check.mjs` / `taskpackage-authoring.md`）↔ manifest 第 1–3 行；`docs/debt/gov/_archive/2026/{ARQ 三件}` ↔ 第 7 行目录条目；`docs/debt/gov/INDEX.md` ↔ 第 6 行；`task_plan.md` ↔ 第 8 行；`delivery/review/261120.../{PROPOSAL,events}` 与 `delivery/review/INDEX.md` 由 `isEvidenceTailPath` 豁免（`governance-gate-shared.mjs:121-126`）。**非证据 diff 全数 ⊆ manifest，零出界**（rename 的两条 ARQ 源径已删除、不在 changed 集；目的径落在第 7 行目录前缀内，实测 `isDirectImplementationPathInScope` 段前缀匹配成立）。

**四件零改动实证**：`git diff --name-only main...1c19f794` 不含 `.claude/verify-gates.json`、`.claude/scripts/lib/current-pointer-shared.mjs`、`.claude/scripts/taskpackage-required-check.mjs`，亦不含 `delivery/taskpackages/**`（实现 commit 只动 3 件）。与 §6 V6(c)/(d) 一致。

#### 二、判别力（本 reviewer 独立夹具亲跑，非作者自证）

我先跑作者随闸的 `--self-check` 确认其断言全绿（`change-manifest` 43/43、`acceptance-coverage` 28/28 = 纯函数 14 + CLI 14），但**判别力凭据以下为我自建、不经作者夹具代码的独立实证**：

- **V1 独立两侧夹具**（临时 `git init` 仓：commit A 以 `GIT_COMMITTER_DATE` 回填 `2026-09-11` 只建 `current/taskpackage.json`；commit B 当日建 `269901-indep/` 包目录 + `firmware/base.c`；工作树留未提交、未被 `server/**` 覆盖的 `firmware/new.c`）：**先自证三基准分处两侧**（投影 `2026-09-11` < cutoff，包目录 `2026-11-21` ≥ cutoff，两者异侧）→ 无参 spawn 真闸（`process.execPath [script]`、cwd 与 `CLAUDE_PROJECT_DIR` 同指夹具仓）→ **exit 1 / 无 `skipped` / `outOfManifest:["firmware/new.c"]` / `cutoffBasis.kind=package-dir` / `firstCommitDate=2026-11-21`**。把 manifest 改覆盖 `firmware/**` 再跑 → **exit 0 / ok:true / 无 skipped / reconciledFaceCount:1**（证非恒红）。
- **反转复核（恢复投影基准必绿=测试必败）**：把闸复制到仓外、仅把 `cutoffBasis(ctx.changeId)` 换回 `gitFirstCommitDate(tpPath)`（= 修前 bug 的投影基准）后跑同一夹具 → **exit 0 / skipped:"pre-cutoff" / basis.kind=projection-file**。同一夹具下新实现红、恢复旧基准绿 —— 证明该判据**由 cutoff 基准修正承重**、判别力非空。
- **V0 真存量夹具**（投影与包目录**同**回填 `2026-09-11`，manifest 为空 `[]`）：无参 → **exit 0 / skipped:"pre-cutoff" / basis.kind=package-dir**。因 manifest 空时若放行分支失效必红，绿即证明 pre-cutoff 放行**由包目录基准驱动且语义未变**。
- **V3 真仓前后对比**：无参（current → `261104-k5-story-cover-nav`）→ `ok:true / manifestSize:29 / reconciledFaceCount:0 / basis package-dir 2026-11-04`（修前 `skipped:"pre-cutoff" / 2026-09-11`）；显式 k5 路径与 `261031-openclaw-cron-trigger-gating`（`manifestSize:9 / basis 2026-10-31`）结论一致，与 §10.3 / ARQ 结项追注逐字吻合。
- **V4 独立夹具**（临时目录 + current 投影 + 带 `// coverage: UP-A1` 的 `.c` 件，无参 spawn）：纯 Unity 行首断言件 → **exit 0**（修前假红已消）；仅注释内带括号 Unity 字样 → **exit 1**（prose 负例保持红）；仅 `#define` 宏包装件 → **exit 0**；真无断言 → **exit 1**。V5-A：`--change-id` → exit 0 且回显来源 `--change-id 旗标`；`--help` → exit 0；`--bogus` → exit 2。
- **V6 独立普查**（`git ls-files firmware server tools`，757 件）：5 个假红文件以新 `hasRealAssertion`（含 `UNITY_ASSERT_RE`）复判 **5/5 转绿**（含 `red-baseline/unity.h` 靠 `#define` 行）；语料修前 154 → 修后 159（= +5，无意外新增）；**翻红(true→false) = 0**（既有六支模式未删，第七支为纯 `||` 追加，结构上不可能翻红）。

#### 三、冻结边界未越

- **N/A 语义**：独立构造「无 current、无任何参数」临时目录无参跑 → **exit 0 / skipped:"no-taskpackage-context"**（一字未改）。§3.3 第一行分支经核与修前语义一致。
- **pre-cutoff 放行**：V0 夹具已证 exit 0 + `skipped:"pre-cutoff"`（见上）。
- **只有两类转非 0**：`explicit && !found`（显式路径打错 / `--change-id` 指向不存在包）→ exit 1；`--` 未知旗标 → exit 2。实测 V5-C 负例两向均 exit 1，`--bogus` exit 2。
- **`--help` exit 0**：change-manifest 与 acceptance-coverage 两闸实测均 exit 0 + usage，且不输出 `"ok":true` 假绿 JSON。
- **changeId 不可得 ≠ 上下文不可解析**：§3.3 bullet 2 的 `cutoffBasis` 不可得分支（包目录无提交历史）实跑 → 不跳过、进对账（fail-closed），`cutoffBasis.firstCommitDate===null`。
- **真实闸链不受影响**：`verify-gates.json` 注册无参、`verify.mjs:271-272` 原样 spawn 不追加旗标；全仓实测无 `DEV_LOOP_TASKPACKAGE`/`TASKPACKAGE_PATH` 赋值/导出，故真实 verify 恒走无参 + current 回退，fail-loud 两分支只在调用方显式误用时触发。

#### 四、消费方与记账事实

- **消费方不回归**：`verify.mjs --self-check` 18/18、`taskpackage-required-check --self-check` 23/23、`mutation-check --self-check` 14/14 全绿（本 reviewer 亲跑）。无机械消费方解析旧 `skipped`/`pre-cutoff` 字面、亦无消费方解析旧 self-check 标签 `acceptance-coverage-python-assert`（全仓 grep 零命中）。`--self-check` 输出 `selfCheck` 标签改名、`contextSource` 新增字段均为形状兼容加字段（§10.5 如实记）。
- **主线程记账件与实现事实一致**：两条 ARQ 已翻 `status: closed` / `closed 2026-11-21` / `closedBy=本批`，追注的 OQ-1=甲 / OQ-2=(c) / OQ-3 符号名、真仓前后对比（k5 29 / 2026-11-04；cron 9 / 2026-10-31）、自检 43/43 与 28/28 均与本 reviewer 亲跑逐项吻合；`task_plan.md` P10 行诚实标「实现落地、合并收口待补」；`delivery/review/INDEX.md` 该批状态 = `frozen`，与 PROPOSAL FROZEN v0.2 一致。

#### 五、偏差复核（§10.5 六条）

六条偏差均为「非 manifest 外改动」且已如实记，逐条核：① 实现 commit 署名行为 `Co-Authored-By: Claude Fable 5`，与本仓惯例（`Claude Opus 4.8`）及主线程派单不同——**非代码/非 tree 内容**，不影响 reviewedTree；作者已如实上报，交主线程 / human 定夺是否 `--amend`（**若 amend 则全部后代 SHA 变、本钉点须按新 SHA 重落**）。② `--change-id` 缺值 exit 2（设计文本未明列的向严新增，防静默回落 current 对错包跑闸）可接受。③ self-check 标签改名（无消费方）④ `contextSource` 加字段（形状兼容）⑤ V4 夹具用行首形态（合 §6 V4 表，并另钉行中前向边界负例）⑥ 两处夹具自缺陷实施期抓出并修正（瘦投影→双写、裸字面断言→JSON 形态）——均属如实、无害。⑦ 可选「完整 verify.mjs --phase=scoped 闸链」未做，合 FROZEN §6 V1 括注（非必做），消费层属性已由无参 spawn 承担。

#### Findings

- 无 BLOCKING / SHOULD（实现正确、判别力实证成立、边界与消费方无破损）。
- **NIT-7（记账，交主线程/human）**：实现 commit 的 `Co-Authored-By` 署名行（`Claude Fable 5`）异于仓内惯例与派单要求（见 §10.5 条 1）。不改代码、不改 tree；若统一为 `Claude Opus 4.8` 需 `--amend`，届时全部后代 SHA 变、本 §11 钉点须按新 SHA 重落并回审。
- **NIT-8（观察）**：`--change-id` 缺值 → exit 2 为设计文本未明列的向严分支（§10.5 条 2），本 reviewer 认可其方向（防对错包跑闸），仅记录在案。

（实现评审 verdict 见下钉点；本 reviewer 已亲跑前置升级闸，回报内含放行结果。）

reviewedCommit: 1c19f7946e03167666f1ede478fe90e7ad7505de
reviewedTree: 3c5476fc8564d440846f557dc435bf7949b3da43
