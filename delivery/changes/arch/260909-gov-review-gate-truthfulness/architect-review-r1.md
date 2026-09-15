# architect-review-r1 · 260909-gov-review-gate-truthfulness

> 判官 = architect（Track B 评审循环 R1，2026-09-09）。只读 canonical 与代码，本文件为唯一写制品（另按职责 append PROPOSAL §7 verdict 摘要）。评审对象 = PROPOSAL DRAFT v0.2（立项 ack 已落：OQ1=(b)/OQ2=(a)/OQ3=(c)/OQ4=(b)）。

## 结论

**NEEDS-WORK**——BLOCKING ×1 / SHOULD ×5 / COULD ×4。批次方向、三根因诊断、T1–T7 改法骨架、Track B 降档、riskTier=medium 全部站得住；全部实测断言（P1–P5 五条量化声明簇）判官用自己的命令独立重跑**全数复现**（见 §B），这是本批相对 260908 两批 R1 的显著改进。唯一 BLOCKING 是 P5 的历史归因失真且污染将落 canonical 的 T7 文本；五条 SHOULD 皆为机械修订（锚错位、验证集时点/机制钉死、T6 第二派生点与缓存面）；**无影响目标效果的打回项，无需升 human**（自指面判定见 §F）。

## A · 锚点亲核（派遣义务 1，逐条）

| PROPOSAL 引用 | 亲核结果 |
|---|---|
| `acceptance-run-check.mjs:100-106` `fileHasLiveMark` regex verbatim | ✓ 逐字一致（`:102` 同一 regex，`:104` fail-safe 注释「宁 auto-run 也不静默跳过未知」在） |
| `:184` hostFiles filter / `:186` live-only skip / `:187-188` pytest 未安装 fail-open / `:191` execSync 裸 pytest | ✓ 全部逐字一致 |
| `unit-run-check.mjs:195` 同构 execSync | ✓ 逐字一致（filter 在 `:188`） |
| `firmware-build-check.mjs:27-30` idfAvailable 只探 PATH / `:13` 严格模式自述 / `:70` required 分支 / `:10`「非假绿」 | ✓ 全部一致 |
| `verify.mjs` 内 `degraded` 字面计数 = 0 | ✓ 判官自跑 `grep -c` = 0 |
| `changes-index.mjs:328`/`:370` `gitContainedInMain(sha)` | ✓ 在场；且 `:327`/`:368` 实测 sha 均取 `gitFirstCommitSha`（首 commit），P5 诊断成立 |
| `arch-review-protocol.md:61`「`--no-ff` 可回滚」 | ✓ 逐字一致 |
| DEV-GOVERNANCE「## 验收测试纪律」§1 现文（host/live 两态、不 auto-run） | ✓（`:268-276`） |
| P2 抽样 `test_g_relay_fix.acceptance.py` `:494`/`:1131`/`:1141` | ✓ 三个行号逐字命中 |
| `knownRedFloor` 唯一使用者 = `260829-story-p3/taskpackage.json:76` | ✓ 全仓 grep 唯一命中，确为手写散文 |
| 三条承载 ARQ 在册 | ✓ ARCH-REVIEW-QUEUE `:48`/`:49`/`:52` 逐条在册 |
| T5.2② `brief-inputs.mjs --change-id/--role acceptance-author|builder` | ✓ 脚本实参 `:34-35`/`:438-439` 一致 |
| T4 引 DP-D4「纯 advisory、绝不当门」先例 | ✓ 260908-input-surface PROPOSAL `:181`/`:294` 引 `dev-loop-operating-system.md:122` 同语 verbatim |
| T3 双路探测与 kidpal-idf-env skill 同源 | ✓ skill 登记 venv python 字面路径 + `c:/tmp/idf_run.py` wrapper +「从 `firmware/` 调」三要素，探测命令可同源导出（维护性风险见 C3） |

锚点层面零漂移——与 260908 两批 R1（均因锚点/引文失真打回）相比，本批锚点纪律达标。

## B · 实测断言判官独立重跑（派遣义务 2，五条全跑，非抽样三条）

判官未照抄 PROPOSAL 命令输出，全部自行构造命令重跑：

1. **P1 census**：自写遍历脚本（两 roots ∪、round JSON `results[]` 内键含 gate+ok/exit 的对象、按 stdout 状态串分类）→ **47 轮 / 331 行 / 13 包**，九闸逐项计数与 PROPOSAL 表**逐数字一致**（acceptance `{live-only 17, RED 2}`、unit `{15, 2, 真跑 2}`、firmware-build `{degraded 46}`、contract-lint 46、acceptance-coverage `{43,3}`、mutation-check `{34,9}`、taskpackage-required `{46,1}`、docs-sync `{31,3}`、security `{30,1}`）。✓
2. **P2 混合件 census**：自写枚举 → `server/acceptance/` test_*.py = 9 份、含 live 标记 9 份、混合件 9 份（9/9），名单与 PROPOSAL 九件一致；「10/19 包」判官按 acceptanceTestsFile 字段 + 去重 + 排除 `current/` 指针副本重算 = **10/19 精确**（raw 20 含 current，与 260908 批 R1-F2 同口径）。✓
3. **P3 import-mode 对照**：亲跑三命令——裸 `--collect-only` → `ModuleNotFoundError: No module named 'test_g_relay_fix'` / `no tests collected, 1 error`；加 `--import-mode=importlib` → `18 tests collected`；再加 `-m "not live"` → `14/18 tests collected (4 deselected)`。`server/acceptance/__init__.py` 不存在 ✓。T1+T2 同批硬约束的实测基础成立。✓
4. **P4 firmware-build 现跑**：`node .claude/scripts/firmware-build-check.mjs` → `{"ok":true,...,"degraded":true,"reason":"idf.py 不可用：固件编译未在本机核验（降级放行，非假绿）"}` 与引用逐字一致；`VERIFY_FW_BUILD_REQUIRED` 四文件 grep 全 0 ✓。✓
5. **P5 两件**：`git merge-base --is-ancestor 014adfe c1e26a2` → 成立 ✓（014adfe 实测 =「三 draft PROPOSAL 入册」commit，触三个批目录）；`git reflog main` 近期 5 条合并全 `Fast-forward` ✓。**但 8bd4a84 归因失真——见 B1。** 另：T6 影响集判官自算全部 19 个 arch 目录首/末 commit 双判据，当前时点 19/19 两判据一致（census 时点的中间态已随合并闭合，见 C1）；本批 INDEX 行现态 = `draft | yes`（旧判据失真活体实例，支持 P5 诊断与 S3 修法）。

## C · findings

### BLOCKING

- **B1（P5 历史归因失真，污染将落 canonical 的 T7 文本）**
  - **断言**：P5 称「`--no-ff` 双亲 commit（如 `8bd4a84`）实为 **merge-driver 的 dev Pod 通道**」；T7 拟落 canonical 的新文本括注「治理批恒 `git merge --ff-only`（与 `git reflog main` **既有实务**…一致）」。
  - **证据锚**：`git show -s --format='%h parents=%p %s' 8bd4a84` → `parents=0775235 460c6ab`，message = 「Merge **260907-gov-review-merge-friction (gov Track B)** — 前置升级闸 PASS（**kind=gov**）+ landing-verdict 绿 + retro 齐…」（2026-09-07 23:50）——这是 **gov 批**按 protocol:61 旧文本执行的双亲合并，不是 dev Pod 通道产物。`git log main --first-parent --merges` 另有 `8c1cba3`（260907-gov-review-arq-governance-batch-2，「merge(gov)…本地合」）、`33d8009`（260906-gov-review-index-status-truth-source）两例 gov 双亲合并；merge-driver dev 通道的双亲合并另有其 message 形态（「(merge-driver) human-approval-ref=…」，如 `e8b73c0`/`8f51a95`）。即「既有实务」含至少 3 例 gov 批 --no-ff 反例，ff-only 是 **2026-09-08 起**的近期实务（reflog main@{0..3,5} 全 Fast-forward）。
  - **为什么不 BLOCKING 不可**：本批主题 = 「闸与簿记说真话」，§6 自律件明写「后续引用本批数字时须重跑而非转引」；P5 对自引例证的误归因若随 T7 落进 canonical protocol 文本，「与既有实务一致」按字面为假，canonical 将内嵌一条可被 `git log --merges` 三分钟证伪的声称——正是本批要消灭的病灶形态。规范方向（gov 恒 ff-only 向前看）本身不受影响。
  - **修法（机械）**：① P5 更正：「`8bd4a84`/`8c1cba3`/`33d8009` = gov 批按 protocol:61 旧文本执行的 --no-ff 双亲合并（三例，2026-09-06/07）；merge-driver dev Pod 通道的双亲合并实例为 message 含 `(merge-driver)` 者（如 `e8b73c0`）」；② T7 新文本括注「既有实务」→「2026-09-08 起近期实务；更早三例 gov 双亲合并系按本条旧文本执行，正是本次收口对象」。

### SHOULD

- **S1（T6 双路识别错误 + 缓存钉死面未覆盖）**
  - **断言**：T6 称「`--upsert` 与全量 `collectArch` 两路同源改（同一函数单源）」。
  - **证据锚**：`computeArchEntry:219` 只读缓存（`cached.mergedInMain !== undefined ? cached.mergedInMain : ''`），**不派生**；真正第二派生点是 `backfill():368-370`（`gitFirstCommitSha` + `gitContainedInMain` 与 `upsertArchRow:327-328` 平行重复一份入参逻辑——函数复用但 sha 派生两处各写，恰是 P5 教训的「派生判据双源」形态）。另两处未提的同文件面：① 头注 `:16`「已合 main = git branch --contains <首提交>」与 `:89` 注释「目录首提交被本地 main 包含」承载旧语义，须同批更新；② `.changes-index-state.json` 把 mergedInMain 当 immutable 钉死（`:326` 注释自承），旧语义钉值不会自动重推——实测当前无活体错钉残留（唯一曾错钉的 `260908-input-surface` 其末 commit `5b9be80` 现已真入 main，歪打正着），但机制缺口须成文。
  - **修法**：T6 改写为「`upsertArchRow:327-328` 与 `backfill:368-370` **两派生点**同改（sha 从首 commit 改末 commit，建议把『目录末 commit 解析 + ancestry』抽成单函数供两处调，消灭平行重复）；`:16`/`:89` 注释同步；landing 后跑一次 `--backfill` 按新语义重钉缓存（写进 §5 V6 或 T6 落地步骤）」。

- **S2（T5.1 步骤锚错位）**
  - **断言**：T5.1「launch prep **第 6 步**（check-launch 门）加一条…」。
  - **证据锚**：`kidpal-pod-launch-prep/SKILL.md` 实测 check-launch 门 = `### 2. launch 机械门（每包）`；`### 6.` 是 `board 预填`。按字面 landing 会把「上游已知红」计数段塞进 board 预填。
  - **修法**：「第 6 步」→「步骤 2」。（T5.2 的「环境既成事实段」锚在步骤 7 bullet `:90`，实测内容一致 ✓，无误。）

- **S3（V6 判据时点不可满足 + 状态列预期错误）**
  - **断言**：V6「`--upsert 260909-gov-review-gate-truthfulness`（本批未合并时）→ INDEX 行须为 `draft | no`（旧判据此处会误报 yes，即负例转正的机判证据）」。
  - **证据锚**：实测 `git rev-parse main gov/kidpal HEAD` 三者同指 `cc15432`——本批 draft 已随 draft 入册 post-merge 簿记进本地 main，目录首末 commit 均 ∈ main，**新判据（末 commit ancestry）此刻同样得 yes**，负例转正在本时点无法产出；且可产出时点（FROZEN commit 已上 gov/kidpal、merge 前的 FROZEN 转移点 upsert）状态列为 `frozen` 非 `draft`。现 INDEX 行实测 `draft | yes`——旧判据失真就活在本批自己头上（P5 诊断的眼前证据，此点反而支持批次必要性）。
  - **修法**：预期改「`mergedInMain` 列 = `no`，status 列以当时制品为准（预期 `frozen`）」；跑动时点钉死为「FROZEN 转移点 upsert（FROZEN 标记 commit 未入 main、merge 前）」——该时点新旧判据分叉可机判展示。

- **S4（V3 第一臂与 T3 双路探测自相矛盾）**
  - **断言**：V3「临时 `PATH` 去掉 idf（或直调 `decide({fwPresent:true, idfOk:false, required:false})`）→ 输出含 `buildAttempted:false` + `strictModeEnv`」。
  - **证据锚**：T3 新增的第二路探测（venv python + `c:/tmp/idf_run.py` wrapper，kidpal-idf-env skill `:Paths` 节实测在册）**不在 PATH 上**——PATH 去 idf 后 wrapper 路仍通，闸将真编译而非降级。PATH 臂按 T3 改后造不出降级态。
  - **修法**：V3 以直调 `decide()` 臂为主证（探针接线由 V1 的 `toolchain-dual-probe` 自检承载）；PATH 臂改为「同时使两路探测均不可得」或删除；「断言 fixture 非空先行」落实为对 decide 入参字面非空的显式断言（现语无 fixture 可指）。

- **S5（V4 fixture 机制未钉，禁令与入口间的可执行性空档）**
  - **断言**：V4「造一次含 degraded/skipped 闸的 verify 轮（可用仓外 temp 包 fixture，用后清理，严禁落 `.claude/taskpackages/` 内）」。
  - **证据锚**：`verify.mjs` 只从 `${CLAUDE_PROJECT_DIR}/.claude/taskpackages/` 取包（`:101` 枚举 / `:123` current / `:127` 按 changeId 解析）；`projectDir` 可由 `CLAUDE_PROJECT_DIR` 重定向（`:31`）。故「仓外 fixture」唯一可执行形态 = `CLAUDE_PROJECT_DIR` 指向仓外 temp 根 + 内置最小 verify-gates 与 stub 闸脚本（打印含 `degraded:true` / `phase:"executor-skipped"` 的固定 stdout）。V4 未钉此机制，landing 期现想易退化为落 `.claude/taskpackages/`（恰触禁令，260908 批 R1-F9 同类教训）或跳过。
  - **修法**：V4 补一句机制钉死（`CLAUDE_PROJECT_DIR=<仓外 temp 根>` + stub 闸 + 最小 `verify-gates.json` 副本 + 用后清理 temp 根），并前置断言 stub 输出非空。

### COULD

- **C1（§3「全 18 目录」计数漂移）**：现 `changes/arch/*/` = 19（第 19 = 本批自身目录）；「18」系 census 先于本批目录创建所留。判官自算 19/19 首末 commit 双判据当前一致（census 时点 input-surface 的中间态已随合并闭合）——实质结论不变，建议 pin as-of 或改述口径。（先例：260908 批 R1-F1 未 pin as-of 被判 BLOCKING；本条因结论不受漂移影响，降级 COULD。）
- **C2（P5/§3 「本批」指代错位）**：本文档（260909 批）内 P5「本批首 commit `014adfe`」与 §3「中间态目录 = 本批自身（`260908-gov-review-input-surface-frontload`）」的「本批」实指 260908-input-surface 批（同会话多批并行的起草残留）。括注给出真 id 可恢复，建议改全称以免转引误读。
- **C3（T3 探测路径维护性）**：wrapper/venv 路径字面从 skill 散文抄进 G3 脚本后，skill 改路径 → 脚本静默漂移回 degraded。建议落地时两路径抽成脚本顶部常量 + 注释「与 kidpal-idf-env SKILL.md 同源，改 skill 必同改此处」（或留 env 覆盖口）。不阻塞。
- **C4（V1 自检名待定口子）**：四自检名「以脚本实际自检名为准」留了命名自由度（有「不得为凑 grep 造空名」兜底），建议 landing-summary 记录实际自检名回填 V1 可追溯。

## D · 逐项义务复核

**T1+T2 同批硬约束闭环 ✓**：P3 实测证明缺一即向噪（post-T1 裸 pytest 对点分混合件必收集炸红）；两改法合成同一 exec 行、同文件、同 manifest 闭合。「全 live 面由执行结果给出（no tests ran → allLiveDeselected:true）」自洽；`:104` fail-safe 语义保持（文件级排除路径删除后「读不到按 host」语义不变）。

**T4 与 DP-D4 一致性 ✓**：先例原文亲核（260908-input-surface `:181`/`:294` 引 dev-loop-operating-system.md `:122`「纯 advisory、绝不当门（DP-D4）」verbatim）；T4「只产字段不改本轮 ok 判定、机械消费留下一批」与同型；verifier 点名义务落 T1c 同节避免再造散文，OQ4=(b) 与 §6 留根吻合。

**T5 三条均可落于 skill 文档 ✓**（S2 锚错位除外）。

**T6 两路同源**：名义错误（实为 upsert + backfill）+ 缓存面未覆盖 → S1。

**T7 三处一致化的第四处自查 ✓ 无遗漏**：判官全仓 grep `no-ff|ff-only|Fast-forward`——gov 合并形态的 flag 字面**仅** protocol:61 一处（本批目标）；`dev-loop-operating-system.md:160` / `dev-loop.md:105` / `pod-coordination.md:74` / `kidpal-pod-launch-prep/SKILL.md:92` / `DEV-GOVERNANCE.md:47` 五处 --no-ff 全部描述 merge-driver dev Pod 通道，与 T7 新分类法一致、不冲突；`merge-to-main` skill description + 步骤 4 + 失败信号节均 ff-only ✓；AGENTS.md「治理路由」与 DEV-GOVERNANCE:158 步 4 无 flag 字面 ✓。「不动 AGENTS.md（G1 非必要不触）」判断成立。

**landing-manifest 完备性 ✓**：围栏块 8 行与 T1–T7 目标文件逐条对照精确闭合（T1+T2 → 两闸脚本；T1c+T4 文档句 → DEV-GOVERNANCE.md；T3 → firmware-build-check.mjs；T4 → verify.mjs；T5 → SKILL.md；T6 → changes-index.mjs；T7 → arch-review-protocol.md），无缺件无多件。packet-id 行在场（`:7`）。**结构字段硬条款满足，但因 B1/S1–S5 未达 GREEN。**

**§3 sweep**：UNCLASSIFIED=0 成立（12 行全有类别）；硬项 1b 枚举无截断省略形态（9/9 名单全列、T6 影响集判官独立重算吻合，口径漂移见 C1）；硬项 1c 六模式集判官抽查其四（fileHasLiveMark/hasLiveMark/live-only/executor-skipped）在两目标脚本之外零命中 ✓。

**新引入扫描**：OQ 裁定与正文落点（T3.3 按 OQ1=(b)、T2 按 OQ3=(c)、T4/§6 按 OQ4=(b)、T1/OQ2=(a)）逐条吻合 ✓；计数错误 = C1 一处；指代错位 = C2。

**验证集真空绿扫描**：§5 头有「先断言输入非空 + collected 数字为判」总规约 ✓；V1 要求 stdout 含新增自检名（非「没报错就算过」）✓；V2 collected>0 + deselected ✓；V4 非零长度 + ok 不变证伪点 ✓（机制空档见 S5）；V5 六模式记命中数 ✓；V7 判据具体且明标非门 ✓；V3/V6 问题见 S3/S4。

## E · Track B 降档复核

**成立、力度足额**。升级判据两条零命中：① 契约面零触——manifest 8 文件逐一亲核无 `docs/contracts/**`，§3 表单列契约面「未触（零）」；② 非跨系统结构性——治理/机械面单侧（G3 脚本 + G2 成文 + G1 一句）。governance 类变更按双轨路由节明文不强制升级。承载的 `ARQ-260908-firmware-build-degraded-gate` 自评「Track A full-adversarial」不构成约束——作者不得自路由；且该 ARQ 的 Track A 论据是「存真设计分叉（强制真编译 vs 降级+ESCALATE）」，此分叉已作为 OQ1 摆到 human 面前并裁定 (b)，对抗要探索的开放判断已被 human 裁决吸收，Track A 的边际价值落空。riskTier=medium 复核成立（向严方向 + 4 个 G3 + 1 G1 + 1 G2 触面，证成句属实）。

## F · 自指面判定（派遣义务 5 重点）

**判定：不升 human。** 理由：① T7 改的是 protocol:61「双轨路由」步 7（merge 形态）一句成文，本评审循环正在遵循的是同节步 2（评审循环），被改文本管的是本评审**下游**的相位，评审行为本身不被自己评审中的改动影响；② 不触 agent 定义、不触第五阀面（该阀管 review-loop agent def，不管 protocol 散文）、不触门语义；③ 唯一的实质自指 = 本批自己的 merge 将按被自己改过的条款执行——但新旧文本在该场景产出同一动作（2026-09-08 起实务已是 ff-only），无行为分叉、无判官利益冲突；④ 自指申报已落在 Status 头部块，human 立项 ack 时可见并仍按推荐 ack，前端对齐义务已尽；⑤ 双轨路由节明文 governance 类变更不强制升级，「声明红队」为自愿逃生门，human 随时可启。B1 修法会让 T7 文本更诚实，与自指判定无涉。

## G · R2 复核面预告（供主线程修订后送审）

R2 复核聚焦五点：B1 两处文本更正（P5 归因 + T7 括注）；S1 T6 改写（两派生点 + 注释 + backfill 重钉步骤）；S2 步骤锚「6→2」；S3 V6 时点与状态列改写；S4/S5 V3/V4 机制钉死；另确认四条 COULD 处理意向。其余面（锚点、实测、降档、自指、manifest）R1 已闭合，无新矛盾不重开。

---

## R2 节 · 复核记录（2026-09-09，对 DRAFT v0.3）—— verdict：GREEN

### R2-0 · 总结

R1 全部 10 条 findings（B1 / S1–S5 / C1–C4）逐条亲核**闭合**，修订未引入新矛盾，无前轮漏审的同类残留。降档（Track B）与自指（不升 human）维持 R1 结论——v0.3 未改变该两面任何前提。**GREEN 前置核对：landing-manifest 围栏块在场（`:13-22` 八行未变，仍与 T1–T7 精确闭合）+ packet-id 行在场（`:7`）✓。**

### R2-1 · 逐条闭合核验（引 v0.3 行号 + 判官亲核）

- **B1 ✓（两处，与判官 R1 实测逐字对账）**：
  - P5 第二条（v0.3 `:105`）现文：「更早存在 3 例 gov 批 `--no-ff` 双亲合并——`8bd4a84`（260907-gov-review-merge-friction）/ `8c1cba3`（260907-gov-review-arq-governance-batch-2）/ `33d8009`（260906-gov-review-index-status-truth-source），均系按 protocol:61 旧文本执行…dev 通道双亲合并实例的 message 含 `(merge-driver)`，如 `e8b73c0`」。判官 R1 亲核留痕逐字支持：`git show -s 8bd4a84` message 含「(gov Track B)…kind=gov」；`git log main --first-parent --merges` 中 `8c1cba3`/`33d8009` 均为「merge(gov)…本地合」形态；`e8b73c0` message 含「(merge-driver) human-approval-ref=…」。归属全对。
  - T7（v0.3 `:178`）拟落文本括注：「与 `git reflog main` 2026-09-08 起近期实务、`.claude/skills/merge-to-main` 步骤 4 一致；更早 3 例 gov 双亲合并…系按本条旧文本执行，正是本次收口对象」——「2026-09-08 起近期实务」与判官 reflog 亲核（main@{0..3,5} 全 Fast-forward，2026-09-08/09）吻合；历史三例如实自述为收口对象。该文本落 canonical 后不再含可证伪声称。闭合。
- **S1 ✓**（v0.3 `:172` T6 全段重写）：派生点更正为 `upsertArchRow:327-328` + `backfill():368-370` 两处、明文「`computeArchEntry:219` 只读缓存不派生」、抽单函数消灭平行重复、`:16`/`:89` 注释同步、`.changes-index-state.json` 钉死面成文 + landing 后 `--backfill` 重钉落 V6——与判官 R1 亲核的脚本实况（`:219`/`:327-328`/`:368-370`/`:326`）逐字对得上。闭合。
- **S2 ✓**（v0.3 `:164`）：「launch prep 步骤 2『launch 机械门（每包）』（check-launch 门；R1-S2 锚位更正…）」——与 SKILL.md 实况（`### 2. launch 机械门（每包）`）一致，旧锚「第 6 步」零残留。闭合。
- **S3 ✓**（v0.3 `:225` V6 重写）：时点钉死「FROZEN 转移点 upsert」、预期「`mergedInMain` 列 = `no`，status 列以当时制品为准（预期 `frozen`）」、`draft | yes` 活体实例保留作旧判据失真证据、`--backfill` 重钉后核对本批行不翻转——可执行、机判可展示，与 T6 缓存重钉步骤咬合。闭合。
- **S4 ✓**（v0.3 `:222` V3 重写）：主证 = 直调 `decide()`（两分支断言齐：`buildAttempted:false`+`strictModeEnv` 字段 / `required:true` → exit 1），PATH 臂删除且删除理由成文（wrapper 路不在 PATH 上），入参非空显式断言落到字面，探针接线归 V1 `toolchain-dual-probe` 自检。闭合。
- **S5 ✓**（v0.3 `:223` V4 补机制段）：`CLAUDE_PROJECT_DIR=<仓外 temp 根>` 重定向 + 最小 `verify-gates.json` 副本 + stub 闸（打印 `degraded:true`/`phase:"executor-skipped"` 固定 stdout）+ 跑前断言 stub 输出非空 + 用后清理；所引 `verify.mjs:101/:123/:127` 三锚与判官 R1 亲核一致。禁令（严禁落 `.claude/taskpackages/`）保留且与机制自洽。闭合。
- **C1 ✓**（v0.3 `:204`）：§3 硬项 1b T6 影响集 pin as-of——「as-of 2026-09-09 census 时点 18 目录；R1 复核时点已 19，第 19 = 本批自身目录」+ 中间态已随合并闭合 + 「新判据只在『landing 后、merge 前』窗口与旧判据分叉」实质结论，与判官 19/19 重算结果一致。闭合。
- **C2 ✓**：P5 第一条（v0.3 `:104`）与 §3（`:204`）两处「本批」均改全称 `260908-gov-review-input-surface-frontload`，并如实注「初稿『本批』系同会话并行起草残留」。闭合。
- **C3 ✓**（v0.3 `:146` T3.1 末）：「两路探测的路径/命令字面抽为脚本顶部常量并注释『与 kidpal-idf-env SKILL.md 同源，改 skill 必同改此处』」——漂移防护成文。闭合。
- **C4 ✓**（v0.3 `:220` V1 末）：「landing-summary 须记录实际自检名回填本条，保 V1 可追溯」。闭合。

### R2-2 · 新引入扫描（v0.3 修订面 ↔ 未动面）

- T7 新文本三句互洽：「治理批恒 ff-only + 有分歧停交 human」/「回滚 = revert 逐 commit 或 human 裁」/「--no-ff 仅 merge-driver dev Pod 通道」——与 P5 更正后史实一致，与全仓其余五处 dev 通道 --no-ff 成文（R1 已枚举）分类一致，无新矛盾。
- V6 删除 v0.2 坏掉的第二 grep pattern（`\-\-no\` ff` 形态）后表述自洽；「以实际改写句为准锚」的语义判据由 landing-verifier 人读 diff 承载，可接受。
- T6 末段「中间态如实显示为 `frozen | no`」与 V6 预期口径一致；§3 硬项 1b 末句窗口结论与 V6 时点钉死一致。
- OQ 裁定落点维持吻合：OQ1=(b)→T3.3+T5.2①、OQ2=(a)→T1、OQ3=(c)→T2+§6 留根、OQ4=(b)→T4+§6 留根——v0.3 未触动。
- Status 头部 v0.3 行如实记 R1 结果（NEEDS-WORK 计数与判官发出一致）。
- **范围外微注（零成本、不阻塞、不修亦可）**：P5 第一条「014adfe（仅 PROPOSAL 入册）」的「仅」字宽口径下含 `changes/INDEX.md` 与另两批 PROPOSAL（该 commit 实触 4 文件）；从「对 input-surface 批而言首 commit 只入册 PROPOSAL、无 canonical landing」的语义读成立，判官不要求再改，留此注记防未来转引时按字面理解。

### R2-3 · 结论

**GREEN。** 批次可交主线程凭「立项 ack + GREEN」标 FROZEN；后续 packet-bound landing → landing-verifier 核验 → retro → 前置升级闸 → 自动合并本地 main 按既有七步走后四步。V6 的 FROZEN 转移点 upsert 与 `--backfill` 重钉核对是 landing 期的两个机械动作点，主线程按 v0.3 `:225` 执行即可。
