# PROPOSAL · 260909-gov-review-gate-truthfulness

> **状态：DRAFT v0.1 · 2026-09-09 · 主线程起草完毕，待 human 立项 ack（ack 后送 architect 评审循环）**
> **承载 ARQ**：`ARQ-260908-firmware-build-degraded-gate`、`ARQ-260908-gate-pytest-import-mode-incompat`、`ARQ-260908-known-red-count-no-mechanical-source`（三条在册 SHOULD，本批 T3/T2/T5 各自承载）；另承载**本轮新实测出的两条未在册根因**——闸内 live 判据文件级粗判（T1，实测量级远超 import-mode 那条）与降级放行零机器可判面（T4），以及两条主线程新撞的簿记/口径失真（T6 `mergedInMain` 派生、T7 gov 批合并形态 ff vs `--no-ff`，human 2026-09-09 裁「不另登 ARQ、直接写进首批」）。子项不关的根（C 侧验收相对路径 root、supervisor/merge 对 degraded 的机械消费）见 §6。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。升级判据核对：不触 `docs/contracts/**`（契约面为零）、非跨系统结构性变更（治理/机械面单侧）→ 不升 Track A。**注（存疑向严）**：本批是治理批去改「治理批自己正在用的机器面」（含 `arch-review-protocol.md` 一处成文，T7），自指面如实申报，判官可按需升 human。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集（V1–V7）。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：判定**方向一律向严**——三条执行闸从「恒放行」变「真跑或显式记账」，会让历史上一向静默的面第一次给出红，downside = 首几轮 verifier 变红要修；但触面含 4 个 G3 脚本/钩子 + 1 个 G1 指令根成文 + 1 个 G2 协议文本，非纯成文，故不再 low-medium）。
> **review profile**：G1+G2+G3（G1 = `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」§1 两态约定一处成文对齐（T1c），按 260908 批 R1 判官已裁口径「DEV-GOVERNANCE.md 在 §6 表 `:212` 字面列名属 G1」；G2 = `arch-review-protocol.md`（T7）+ `kidpal-pod-launch-prep/SKILL.md`（T5）；G3 = `acceptance-run-check.mjs` / `unit-run-check.mjs` / `firmware-build-check.mjs` / `verify.mjs` / `changes-index.mjs`；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/unit-run-check.mjs
.claude/scripts/firmware-build-check.mjs
.claude/hooks/verify.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/scripts/changes-index.mjs
.claude/references/arch-review-protocol.md
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让 dev-loop 的**执行类结构闸说真话**——`acceptance` / `unit` / `firmware-build` 三闸要么真跑、要么把「没跑」记成机器可判的降级事实并被上游看见，不再以 `ok:true` 的形态把「一次都没执行」伪装成通过。

**现状差距**：本仓 13 个有轮次证据的包、47 轮、331 行闸结果实测（命令见 §1 P1 末）显示——`acceptance` 闸 **0 次真跑**（17 次放行 + 2 次红），`unit` 闸 47 行里只真跑 **2 次**，`firmware-build` 闸 **46 次全部 degraded 放行、零次真编译**。同期对照闸（`contract-lint` 46 真跑、`acceptance-coverage` 43、`mutation-check` 34、`taskpackage-required` 46）全部正常，证明**不是证据采集缺失，而是这三条闸本身在静默放行**。

**非目标（本批不做）**：
- 不改任何**验收断言本身**、不动冻结验收件（tighten-only 纪律另论，`ARQ-260908-tighten-only-unwritten` 在册）；本批改的是「闸跑不跑、怎么跑、跑不动怎么记」。
- 不做 supervisor-verdict / pre-merge-escalation 对 degraded 的**机械消费**（触两扇门语义，见 OQ4 与 §6 留根）——本批只把降级变成机器可判字段并成文「必须被上游点名」。
- 不修固件/C 侧验收件里「证据文件按相对路径读取、root 定位吃 cwd」那半根因（属自有实现面 `firmware/main/test/**.c`，走 dev-loop，§6 留根 + 交接）。
- 不动 `server/pytest.ini`（自有实现面，`AGENTS.md`「自有代码」条）——import-mode 单源落点见 OQ3。
- 不改闸的门数/门序（`verify-gates.json` 零改动，无新增工程门）。

## 1. 问题与根因（地面真值）

> 本节全部断言 2026-09-09 本会话实测；每条量化声明注明覆盖集合与命令；先例引用均已读原文。

### P1 · 三条执行类闸「恒放行」的实测量级

**现跑 census**（覆盖集合 = `.claude/taskpackages/*/verifier-evidence/rounds/*.json` ∪ `changes/dev/*/verifier-evidence/rounds/*.json`；遍历每份 round JSON 内嵌闸结果对象（键含 `gate` + `ok`/`exit`），按 `stdout` 中的状态串分类）：**47 份含闸明细轮次 / 331 行闸结果 / 13 包**。输出摘录（verbatim）：

```text
acceptance             {'live-only放行': 17, 'RED': 2}            ← 真跑 0
unit                   {'live-only放行': 15, 'RED': 2, '真跑': 2}
firmware-build         {'degraded放行': 46}                        ← 真编译 0
contract-lint          {'真跑': 46}
acceptance-coverage    {'真跑': 43, 'RED': 3}
mutation-check         {'真跑': 34, 'RED': 9}
taskpackage-required   {'真跑': 46, 'RED': 1}
docs-sync              {'真跑': 31, 'RED': 3}
security               {'真跑': 30, 'RED': 1}
```

**根因**：见 P2–P4 三条独立成因（不是同一个 bug 的三种表现）。**DEV-GOVERNANCE「验收测试纪律」§1 成文的「host/live 两态」里 host 面必 auto-run 这一半，在闸内一次都没兑现**。

### P2 · 根因一：闸内 live 判据是**文件级** regex，而本仓验收文件 100% 是混合件

`.claude/scripts/acceptance-run-check.mjs:100-106`（verbatim）：

```js
function fileHasLiveMark(absPath) {
  try {
    return /@pytest\.mark\.live|pytestmark\s*=\s*pytest\.mark\.live/.test(readFileSync(absPath, 'utf8'));
```

调用点 `:184`：`const hostFiles = real.pyAccFiles.filter((f) => !fileHasLiveMark(join(projectDir, 'server', f)));` → **文件里任何一处**出现 live 标记，整份文件出闸；`hostFiles` 为空即 `:186` 打 `executor-skipped / live-only on-target acceptance` 并 `ok:true`。`unit-run-check.mjs:195` 同构。

**混合件 census（机械枚举，命令 = 对 `server/**/*.py` 中 basename 以 `test_` 开头者，正则计 `@pytest.mark.live` 标记数与 `def test_` 条目数）**：`server/acceptance/` 下 **9 份 test_*.py，含 live 标记 9 份，其中混合件（有 live 标记且 `def test_` 条目数 > live 标记数）9 份 = 9/9**。名单：`test_char_stroke` / `test_cloud_link` / `test_g_relay_fix` / `test_gateway_directionB` / `test_schedule_p2` / `test_schedule_reminder` / `test_smoke_e2e` / `test_story_p3` / `test_voice_roundtrip_b`（各 `.acceptance.py`）。抽样实证 `test_g_relay_fix.acceptance.py`：`:494 class GRelayFixStructuralAcceptance` 与 `:1131 class GRelayFixLiveAcceptance`（其 `:1141 pytestmark = pytest.mark.live`）同文件并存——**结构面条目被 live 段连坐放行**。受害面：**10/19 包**的 `acceptanceTestsFile` 含 `server/` 路径（机械计数）。

**根因**：判据粒度错配——约定（DEV-GOVERNANCE §1）是**条目/类级**两态，实现是**文件级**。

### P3 · 根因二：闸内 pytest 执行口径与各包 `redBaselineCommand` 不同源（缺 `--import-mode=importlib`）

`acceptance-run-check.mjs:191` 与 `unit-run-check.mjs:195`（verbatim 同一句）：`execSync(\`python -m pytest -q ${hostFiles.join(' ')}\`, { cwd: join(projectDir, 'server'), stdio: 'inherit' })`。而包侧权威口径（`260907-g-relay-fix/taskpackage.json` 的 `redBaselineCommand`，verbatim 起段）：`cd server && python -m pytest acceptance/test_g_relay_fix.acceptance.py -q --import-mode=importlib`。

**实测对照**（2026-09-09，`cd server`，`--collect-only`，同一文件）：

```text
python -m pytest -q --collect-only acceptance/test_g_relay_fix.acceptance.py
  E   ModuleNotFoundError: No module named 'test_g_relay_fix'
  !!! Interrupted: 1 error during collection !!!   no tests collected, 1 error

python -m pytest -q --collect-only --import-mode=importlib acceptance/...acceptance.py
  18 tests collected
```

`server/acceptance/__init__.py` 不存在（实测 `ls` 报 no such file），故默认 prepend 模式对**点分文件名**（`.acceptance.py`）必然 import 失败。**后果**：P2 修好后若不同批修 P3，三闸会立刻从「恒绿放行」翻成「恒红收集失败」——**两条必须同批改，否则向严变成向噪**。

### P4 · 根因三：工具链缺失即降级放行，且逃生口零设置、降级零机器可判面

`firmware-build-check.mjs:27-30` 探测 = `spawnSync('idf.py', ['--version'])`（**只探 PATH**）；本仓 IDF 实际通路是 venv python + `idf_run.py` wrapper（`.claude/skills/kidpal-idf-env` 与本批实测：`node .claude/scripts/firmware-build-check.mjs` 现跑输出 `{"ok":true,...,"degraded":true,"reason":"idf.py 不可用：固件编译未在本机核验（降级放行，非假绿）"}`）。脚本 `:13` 自述有严格模式 `VERIFY_FW_BUILD_REQUIRED=1`——**全仓设置处 = 0**（对 `verify.mjs` / `kidpal-pod-launch-prep/SKILL.md` / `pod-coordination.md` / `verify-gates.json` 逐文件 `grep -c`，四者皆 0）。

叠加上游失明：`.claude/hooks/verify.mjs` 内 `degraded` 字面计数 = **0**（实测 `grep -c`）——闸打印的降级字段被**逐字塞进 round evidence 的 stdout 字符串**，但产线与任何机器判定都不认它。同型缺口在 pytest 两闸：`:187-188` 的「pytest 未安装（fail-open）」与 `:186` 的「live-only」都只打印 `phase: executor-skipped` + `ok:true`，与真跑通过**在结构上不可区分**。

**根因**：「诚实降级」的设计意图（脚本头注 `:10` 明说「非假绿」）**只落在人读 stdout 上，没落进机器可判字段**——于是它对上游闸、对 merge 判定、对仪表盘都等价于通过。这正是 260908 批 T2 advisory 那条「无人消费的 advisory 等于无」的同型病灶，只是这次在承重结构闸上。

### P5 · 簿记与口径两件（本轮主线程亲历，非执行闸面）

- **`mergedInMain` 派生失真**：`.claude/scripts/changes-index.mjs:328/:370` 用 `gitContainedInMain(sha)`，而该 `sha` 取批次目录**首 commit**。实测：本批首 commit `014adfe`（仅 PROPOSAL 入册）对合并前的 main（`c1e26a2`）跑 `git merge-base --is-ancestor 014adfe c1e26a2` → **成立**；于是批次**尚未合并**时 `--upsert` 就把 INDEX 写成 `frozen | yes`，随后再写 `landed | yes`（本会话 07:5x 与 09:2x 两次实测复现）。列义「已并入 main」被稀释为「目录在 main 上有祖先」。
- **gov 批合并形态口径不一致**：`arch-review-protocol.md:61`（verbatim）「放行 → 自动合并本地 main（`--no-ff` 可回滚，remote push 仍 human）」；而 `git reflog main` 实测两条 gov 批落点均为 **Fast-forward**（`main@{1}: merge gov/kidpal: Fast-forward`、`main@{3}: merge gov/kidpal: Fast-forward`），`.claude/skills/merge-to-main/SKILL.md` 步骤 4 亦写「ff-only」；`--no-ff` 双亲 commit（如 `8bd4a84`）实为 **merge-driver 的 dev Pod 通道**。三处文本/实务两两不一致。

### 先例论证（对偶迁移，已读先例原文确认形态）

- **向严判据 + fail-closed 的先例**：`firmware-build-check.mjs:44-46` 已有 `required` 分支（`VERIFY_FW_BUILD_REQUIRED=1` 时缺失即红）——T3 是把该逃生口**接上电**并加真实探测，不新造判据。
- **「文件级 vs 条目级」判据先例**：`acceptance-coverage-check.mjs` 家族与本仓 `lib/acceptance-id-shared.mjs` 均按**条目 id** 承载判定（`extractEntryId` 那套），证明条目级解析在本仓有既有形态可对齐。
- **「无人消费的 advisory 等于无」先例**：260908 批 R1/F 节对 brief-inputs WARN 宿主的论证 + 该批 `:122` 枚举「纯 advisory、绝不当门（DP-D4）」定性——本批 T4 承同一刀法：**记账面必须机器可读，但绝不因此变成第二道门**（两条纪律不冲突，前者管可见性、后者管拦截权）。

## 2. 改法（T1…T7）

### T1 · 闸内 live 判据从文件级降为**条目级**（消灭 9/9 连坐放行）

两脚本同改，判据换成 pytest 原生条目级筛（实测可行：`python -m pytest -q --collect-only --import-mode=importlib -m "not live" acceptance/test_g_relay_fix.acceptance.py` → `14/18 tests collected (4 deselected)`）：

1. **删除文件级排除逻辑**：`acceptance-run-check.mjs:184` / `unit-run-check.mjs` 同构行不再 `filter(!fileHasLiveMark)`，改为**只要有候选 test 文件就执行**，命令带 `-m "not live"`，由 pytest 按标记条目级筛除 live 面。
2. `fileHasLiveMark` **不删**，降级为「报告用」：统计每个文件 `def test_` 条目数与 live 标记数，输出一行 `structuralCollected` vs `liveDeselected` 计数进 JSON（让「这次真跑了多少条结构面」成为可判字段，而非只有一句 skipped/no-skipped）。
3. 「全为 live 面」的判定改由**执行结果**给出：`-m "not live"` 后 pytest 报 `no tests ran`（deselect 全部）时，才打 `phase: executor-skipped` 且 `reason` 必须含实测条目数；此时 `ok:true` 但 `allLiveDeselected: true`（供 T4 上收）。
4. 读不到文件的 fail-safe 语义保持不变（`:104` 注释「宁 auto-run 也不静默跳过未知」——本改法整体与该向严口径一致，因为不再有「一标记全屏蔽」路径）。

对应 P2。**为什么不是拆分验收文件**：拆文件要迁 9 份既有冻结件、动 tighten-only 面、且新包仍会重犯（约定不消灭判据缺陷）——见 OQ2。

目标文件：`.claude/scripts/acceptance-run-check.mjs`、`.claude/scripts/unit-run-check.mjs`

### T1c · DEV-GOVERNANCE 两态约定补条目级判据一句（G1，成文与闸同源）

`docs/DEV-GOVERNANCE.md`「## 验收测试纪律」§1 末追加（承重 landing 指令，原文引用钉死）：现 §1 讲「server host/live 两态 + `@pytest.mark.live` 执行器不自动跑」，追加句 = **「两态在**条目/类级**判：闸内以 `pytest -m "not live"` 执行，禁按文件级标记整份排除（实测病灶：`server/acceptance/` 9/9 混合件曾被文件级判据整份放行）；一文件内混排两态合法，全为 live 面时闸须显式报 `allLiveDeselected` 而非静默 skip。」**

对应 P2（成文↔实现同源，防「期望只活在闸里」镜像病灶）。

目标文件：`docs/DEV-GOVERNANCE.md`

### T2 · pytest 执行口径与包 `redBaselineCommand` 同源（补 `--import-mode=importlib`）

`acceptance-run-check.mjs:191` / `unit-run-check.mjs:195` 的执行串改为 `python -m pytest -q --import-mode=importlib -m "not live" <files>`（与 T1 同一处改动合成一句，避免两批改同一行）。理由与实测见 P3；**与 T1 同批是硬约束**（否则向严即向噪）。落点选址（闸侧 flag vs `server/pytest.ini` addopts 单源）见 **OQ3**——本批按 OQ3 裁断落，默认落 (c)：闸侧先修、单源化另登。

目标文件：`.claude/scripts/acceptance-run-check.mjs`、`.claude/scripts/unit-run-check.mjs`

### T3 · firmware-build 真实工具链探测 + 严格模式接上电

`firmware-build-check.mjs` 三处：

1. `idfAvailable()`（`:27-30`）扩为**两路探测**：PATH 上的 `idf.py` ∨ 本仓实际通路（`kidpal-idf-env` skill 登记的 venv python + `idf_run.py` wrapper 形态，探测命令与该 skill 文档同源，不新造路径字面）；两路都不通才算不可得。
2. **降级不再是静默 `ok:true`**：`degraded` 分支输出补 `{ ok: true, degraded: true, buildAttempted: false, toolchainProbes: [...], strictModeEnv: 'VERIFY_FW_BUILD_REQUIRED unset' }`（把「为什么没跑」变字段，供 T4 上收与 OQ1 裁断消费）。
3. `required`（`:70`）判据保留，**新增默认策略按 OQ1 裁断落**：(a) Pod 环境默认严格 → 本行改为 `required !== '0'`（默认严格，显式关）；(b) 维持显式开启但由 T5 在 launch 面钉上环境变量。本批按 OQ1=(b) 起草（launch 面钉 env + 降级显式记账），若 human 选 (a) 则本条与 §5 V3 同步改。

对应 P4。目标文件：`.claude/scripts/firmware-build-check.mjs`

### T4 · 降级/放行状态上收为 round evidence 的机器可判字段

`.claude/hooks/verify.mjs`（现内 `degraded` 字面 = 0）：闸结果对象入证据时，除现有 `ok`/`exit`/`stdout` 外，从闸 stdout JSON **解析并提升到结构字段** `{ notExecuted: <bool>, notExecutedReason: <string> }`，判定 = 闸输出含 `degraded:true` ∨ `phase:"executor-skipped"` ∨ `allLiveDeselected:true`；`verify.mjs` 汇总层加 `{ gatesNotExecuted: [<id...>], gatesRun: <n> }` 进 round fact。

效力边界（**绝不当第二道门，DP-D4 同型**）：本字段**不改本轮 ok 判定**、不改 merge 放行；它做的是「让上游与仪表盘能判」。真正的机械消费（supervisor-verdict / pre-merge 闸读它）按 **OQ4** 决定是否本批内做，默认留下一批。**verifier 报告义务成文**：`notExecuted` 非零时，verifier 报告必须逐条点名（此句落 T1c 同节的 DEV-GOVERNANCE 追加段，避免再造无人读的散文）。

对应 P4 的「降级零机器可判面」。目标文件：`.claude/hooks/verify.mjs`、`docs/DEV-GOVERNANCE.md`（与 T1c 同文件同节，合并一条 landing 指令）

### T5 · launch 面「上游已知红」机械来源 + root 定位不吃 cwd + 新机制接入两句

`.claude/skills/kidpal-pod-launch-prep/SKILL.md`：

1. 「上游已知红」计数由**现跑命令**给出：launch prep 第 6 步（check-launch 门）加一条 `python -m pytest -q --import-mode=importlib -m "not live" ...` / host ELF 计数的**绝对路径调用口径**（与 `-C` 同法消除 cwd 敏感），并把输出落进 handoff 制品（append 一行计数 + 命令 + 日期），taskpackage 的 `knownRedFloor` 字段引用该制品而非手写数字（现存量唯一使用者 = `260829-story-p3/taskpackage.json:76` 手写散文）。
2. **环境既成事实**段（现含 IDF 通路 `python c:\tmp\idf_run.py` 与「丢失按 pod-s-handoff/README.md §3.1 重写」）补两条：① `VERIFY_FW_BUILD_REQUIRED=1` 的注入位置（按 OQ1=(b)）；② 260908 批新机制接入两句——`node .claude/scripts/brief-inputs.mjs --change-id <id> --role acceptance-author` / `--role builder` 与「先补 `crossEnd` 判断（判不了向严填 counterparts，不填 exempt）」。
3. 该 skill 的 C 侧相对路径 root 根因（`voice_roundtrip_b.acceptance.c:878` 类齿读 `changes/dev/**` 证据文件）**只写指针不改码**（自有实现面 → dev-loop，§6 留根）。

对应 P1/P5 的 launch 侧。目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`

### T6 · `mergedInMain` 派生判据改「批次末 commit 是否已在 main」+ 与 status 解耦

`.claude/scripts/changes-index.mjs`：`gitContainedInMain(sha)`（`:328`、`:370`）的入参从**首 commit** 改为**该批次目录的最新 commit**（`git log -1 --format=%H -- changes/arch/<id>/`），语义 = 「本批全部制品已并入 main」；`--upsert` 与全量 `collectArch` 两路同源改（同一函数单源，防两路分叉——本批 §1 P5 的教训即派生判据双源）。另：**status 与 mergedInMain 不再互相暗示**——`landed` 仍由制品齐推，但 `mergedInMain` 只认末 commit ancestry；未合并但目录已在 main 的中间态如实显示为 `frozen | no`。

对应 P5 第一条。目标文件：`.claude/scripts/changes-index.mjs`

### T7 · gov 批落 main 合并形态口径收口（三处文本一致化）

`.claude/references/arch-review-protocol.md:61`「双轨路由」步 7 现文（verbatim）：「主线程跑前置升级闸（`pre-merge-escalation-gate.mjs`，含 off-main 校验），放行 → 自动合并本地 main（`--no-ff` 可回滚，remote push 仍 human）；升级闸拦 → 升级人。」改为：**「…放行 → 自动合并本地 main：治理批恒 `git merge --ff-only`（与 `git reflog main` 既有实务、`.claude/skills/merge-to-main` 步骤 4 一致；main 无独有 commit 时才成立，有分歧即停交 human），回滚口径 = `git revert` 逐 commit 或 human 裁；`--no-ff` 双亲合并仅用于 merge-driver 的 dev Pod 通道（该通道产物是 Pod 分支，需保留双亲以证合入事实）。remote push 仍 human。」**

对应 P5 第二条；同时消除「protocol ↔ skill ↔ 实务」三处两两矛盾。AGENTS.md「治理路由」节仅写「自动合并本地 main（可回滚）」无 flag 字面 → 与本改法不冲突，**不动 AGENTS.md**（G1 指令根非必要不触）。

目标文件：`.claude/references/arch-review-protocol.md`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 执行闸 · pytest | `.claude/scripts/acceptance-run-check.mjs`、`.claude/scripts/unit-run-check.mjs` | G3 行为向严（放行→真跑） | `verify.mjs` 按 `verify-gates.json` 跑；结果进 round evidence；verifier / fixer / supervisor 读 round fact |
| 执行闸 · 固件 | `.claude/scripts/firmware-build-check.mjs` | G3 探测扩 + 降级显式化 | 同上（`firmware-build` 闸，timing=scoped） |
| 证据产线 | `.claude/hooks/verify.mjs` | G3 加机器可判字段（不改判定） | verifier-evidence round fact；`verifier-evidence-check.mjs`（身份/格式校验，**不校验新字段**，零改动）；`/loop-retro` 与仪表盘 |
| 门配置 | `.claude/verify-gates.json` | **未触** | 门数/门序/timing 零改动（本批无新增工程门） |
| 上游判定 | `hooks/supervisor-gate-check.mjs`、`supervisor-verdict-gate.mjs`、`scripts/merge-driver.mjs`、`pre-merge-escalation-gate.mjs` | **未触**（本批只产字段不消费） | 消费改造按 OQ4，默认下一批；留根 §6 |
| 自有实现面 | `server/pytest.ini`、`firmware/main/test/**.c` | **未触（越界）** | pytest.ini 单源化 = OQ3(b) 分支或另登；C 侧 root 定位 = dev-loop 交接（§6） |
| launch 面 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | G2 成文 + 命令口径 | portfolio/主线程 launch 准备；launch prompt 文件本身在 git common dir 不入版本控制（零治理成本） |
| 簿记 | `.claude/scripts/changes-index.mjs` | G3 派生判据改正 | `changes/INDEX.md` 生成面；stale-report GC 点名；各批 FROZEN/landed 簿记 |
| 协议文本 | `.claude/references/arch-review-protocol.md` | G2 成文口径 | 主线程 merge 行为、判官/核验官简报（`track-b-judge-brief` 只引节名不引 flag，零改动）、`merge-to-main` skill |
| 指令根 | `docs/DEV-GOVERNANCE.md` | **G1 成文**（验收纪律 §1 补句 + verifier 报告义务一句） | 两闸判据同源；acceptance-author / verifier 行为 |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| taskpackage schema | 无新字段（`knownRedFloor` 用法从手写改引用制品） | G2 行为口径 | 存量包不追溯 |

**【硬项 1b · sweep 透镜义务】** 新判据全仓预跑（2026-09-09 现跑，命令与输出见 §1 P2 census 段与 P1 表）：
- **T1 新判据 `-m "not live"` 的影响集** = 9/9 `server/acceptance/` test 件（全为混合件，见 P2 名单，逐包机械枚举无截断省略形态）→ 全部从「整份排除」变「结构面条目真跑」；实测抽样：g-relay-fix 14 条结构面 collected / 4 条 live deselected。
- **T3 新判据（双路工具链探测）影响集** = 所有含 `firmware/` 的包，实测 `firmware-build` 闸出现于 46 行轮次、100% degraded；`VERIFY_FW_BUILD_REQUIRED` 在册设置 = 0（四文件 grep -c 全零，命令见 §1 P4）。
- **T6 新判据（末 commit ancestry）影响集**：`changes/arch/*/` 全 18 目录逐条重算，命中「首 commit 已在 main 但末 commit 不在 main」的中间态目录 = 本批自身（`260908-gov-review-input-surface-frontload`，实测 `git merge-base --is-ancestor 014adfe c1e26a2` 成立）；其余 17 批 landing 完整、两判据结论一致。
- 既有测试基线影响：T1+T2 后 `acceptance`/`unit` 首轮**可能给出历史未暴露的红**——属预期向严（OQ1/OQ4 处理其门语义），非本批缺陷。

**【硬项 1c · 复扫模式完备性先验】** 本批含一处退役语面清扫：文件级 live 判据的调用形态。模式集显式枚举（词形 × backtick/无 backtick）：`fileHasLiveMark` / `hasLiveMark` / `live-only` / `executor-skipped` / `pytest\.mark\.live` / `pytestmark`；完备性论证 = 本批改的是**判据函数本身与其调用点**，而 live 标记的合法书写形态只有装饰器与模块/类级 `pytestmark` 两种（现文 regex 已覆盖二者），故以 `grep -rn` 上述六模在 `.claude/scripts/**`、`.claude/hooks/**`、`.claude/references/**`、`docs/**` 全量复扫（landing 时执行并把「零残留」写进 §5 V5），非目标面（`server/**` 测试码内的标记本身）**保留不改**——它们是判据的输入而非判据。

## 4. human 取舍点（OQ）

- **OQ1 · 工具链/环境不可得时的门语义**：**(a) Pod 环境默认严格真编译，跑不动即红**（最严，代价 = venv/wrapper 一丢就整天红，历史 launch prompt 已记该通路易丢）；**(b) 允许放行，但降级必须机器可判 + launch 面注入 `VERIFY_FW_BUILD_REQUIRED=1` 为默认、显式关闭需留痕**（推荐——既消灭静默假绿，又不把环境噪声当行为破损；本批 T3+T4+T5 即按此编）；(c) 只显式化字段、不改默认（最轻，代价 = 现状延续，degraded 仍靠人眼）。
- **OQ2 · 混合验收文件的处置**：**(a) 条目级筛选常跑**（推荐，`-m "not live"` 已实测 14/18 collected；零迁件、对新旧包同时生效）；(b) 闸内自解析 AST/class 判哪些条目属 live（复杂、双源于 pytest 自身标记语义）；(c) 约定结构面/live 面拆两文件（要迁 9 份冻结件、动 tighten-only 面，且新包仍会重犯）。
- **OQ3 · import-mode 单源落点**：(a) 只在闸侧补 flag（**本批能做，G3 内**，但与包 `redBaselineCommand` 并存两处）；(b) 改 `server/pytest.ini` 加 `addopts = --import-mode=importlib`（真单源，一处生效全覆盖，**但属自有实现面 `server/`** → 须走 dev-loop 包或 human 明示一次性跳过流程）；**(c) 本批做 (a) 保住向严不向噪，(b) 另登 ARQ 由 dev-loop 或你一次性授权收编**（推荐）。
- **OQ4 · degraded 的机械消费放不放本批**：(a) 本批一起做（改 `supervisor-gate-check.mjs` + `pre-merge-escalation-gate.mjs` 读 `gatesNotExecuted`）——一次动**两扇门的判定语义**，批体量与风险翻倍，且这两扇刚被 260908/260909 两批评过口径问题；**(b) 本批只产字段 + 成文「verifier 报告必须点名」，机械消费另登 ARQ 下一批做**（推荐——先让事实可见，再改判定，符合「一次只动一层」）；(c) 干脆不产字段只改文档（等于放弃 T4，P4 根因不闭）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 真空绿规约：每条重放先断言输入非空；「闸真跑」的判据 = 输出含 collected 数字，不接受「没报错就算过」。

- **V1 · 三闸自检全绿**：`node .claude/scripts/firmware-build-check.mjs --self-check`、`node .claude/scripts/acceptance-run-check.mjs --self-check`、`node .claude/scripts/unit-run-check.mjs --self-check` → 各 exit 0，且 stdout 含本批新增自检名（`mixed-file-structural-runs`、`all-live-deselected-reported`、`toolchain-dual-probe`、`degraded-fields-present`——落地时以脚本实际自检名为准，不得为凑 grep 造空名）。
- **V2 · 条目级筛选实测（非假绿主证）**：`cd server && python -m pytest -q --collect-only --import-mode=importlib -m "not live" acceptance/test_g_relay_fix.acceptance.py` → 输出含 `collected` 且条数 **>0**、含 `deselected`；随后经闸跑一次同包（scoped 相位或直调脚本）→ 输出**不再是** `executor-skipped`，而含 `structuralCollected ≥ 1`。
- **V3 · 降级不再静默**：临时 `PATH` 去掉 idf（或直调 `decide({fwPresent:true, idfOk:false, required:false})`）→ 输出含 `buildAttempted:false` + `strictModeEnv` 字段；`required:true` 分支 → exit 1 点名。断言 fixture 非空先行。
- **V4 · 上收字段进证据**：造一次含 degraded/skipped 闸的 verify 轮（可用仓外 temp 包 fixture，用后清理，**严禁落 `.claude/taskpackages/` 内**）→ round fact 含 `gatesNotExecuted` 数组与非零长度，且**该轮 `ok` 不因新字段改变**（向严不越界的证伪点，OQ4=(b) 的直接验收）。
- **V5 · 残留清扫复扫**：`grep -rn "fileHasLiveMark(join\|live-only on-target" .claude/scripts .claude/hooks` → 除报告用途的计数函数与注释外**零调用级残留**；§3 硬项 1c 的六模式集全跑一遍并记命中数。
- **V6 · 簿记与口径**：`node .claude/scripts/changes-index.mjs --upsert 260909-gov-review-gate-truthfulness`（本批未合并时）→ INDEX 行须为 `draft | no`（旧判据此处会误报 yes，即负例转正的机判证据）；`grep -n "ff-only" .claude/references/arch-review-protocol.md` ≥1 且 `grep -n "\-\-no\` ff" ...` 无残留矛盾表述（以实际改写句为准锚）；`git reflog main -3` 出现 `merge gov/kidpal: Fast-forward` 与新口径一致。
- **V7 · 首轮真跑的下游噪声已备**（非门，交 pilot 观察）：本批 landing 后首个含 `server/` 验收的包，其 `acceptance`/`unit` 闸**允许给出历史未暴露的红**；判据 = 红必须带 collected/deselected 数字与失败条目 id，不得再出现「一条都没跑却 ok:true」。若出现 0 collected 却 ok:true → 本批未达标，回补 T1。

## 6. 留根清单

- **degraded 的机械消费**（OQ4=(b) 明写去向）：`supervisor-gate-check.mjs` 与 `pre-merge-escalation-gate.mjs` 读 `gatesNotExecuted` 并升级为 ESCALATE/例外，另登 ARQ（触两扇门语义，独立一批）。
- **`import-mode` 真单源化**（OQ3=(b)）：`server/pytest.ini` 加 `addopts`，属自有实现面 → dev-loop 包或 human 一次性授权。
- **C 侧验收证据文件 root 定位吃 cwd**：`firmware/main/test/**.acceptance.c` 读 `changes/dev/**` 证据的相对路径根因（`ARQ-260908-known-red-count-no-mechanical-source` 面 ② 的下半），走 dev-loop（自有实现面），本批 T5 只写指针。
- **仪表盘可见性**：`pod-events-aggregate.mjs` 与 260908 批 `actors` 列尚不覆盖「闸执行率」，`gatesNotExecuted` 需要 cadence 消费面 → 与上条 ARQ 并批或交 `/loop-retro`。
- **本批自指面申报**：T7 修改治理批自身正在使用的协议文本，属自指改动，如实标注交判官判是否须升 human（review 轨段已注）。
- **交日落复盘**：V7 首轮真跑噪声；三个待启动包（`ui-art-integration` / `hardware-validation` / `audio-bargein-hw`，全 riskTier=high 全固件向）即本批的 pilot——本批 landing 后它们才是「闸有没有开始说真话」的第一批证据来源。
- **自律件（不进模板）**：本批所有量化断言命令随文可重跑（P1 census / P2 混合件枚举 / P3 对照 / P4 计数）；后续引用本批数字时须重跑而非转引（260908 批 R1-F1/R2-F1/R3-F1 三起同根教训）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
