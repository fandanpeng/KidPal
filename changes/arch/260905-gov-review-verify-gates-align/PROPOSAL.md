# PROPOSAL · 260905-gov-review-verify-gates-align

**状态：FROZEN v1.0 · 2026-09-05 · 已立项（human 立项 ack 2026-09-05 + §4 OQ1=(a)、OQ2=(a)）；R1「需修订」（F1/F2+NIT×2）已修订 → R2（2026-09-05）GREEN。主线程凭「立项 ack + GREEN」标 FROZEN，packet-binding 自本标记起生效。本行行首无引用前缀是为命中升级闸 readGovAnchors 机锚正则（T4 落地前现行形态；落地后双形态命中）**
> **承载 ARQ**：`ARQ-260905-verify-gates-types-missing`（HIGH；单根全收：QUICK_CHECK_IDS/oracle/verify-gates.json 三处对齐 + 运行时通道恢复；OQ2=(a) 附收 `readGovAnchors` FROZEN 机锚正则既有错位）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。不触契约面、非跨系统结构变更，无升级判据命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触 G3 合并执行链语义；但改法 = 恢复迁移时既有意图、冒烟子集只减不增、无新放行路径、字面 oracle 保全 + live dry-run 冒烟双向验证，向严默认保持）。
> **review profile**：G3（`.claude/scripts/merge-driver.mjs` + `.claude/scripts/pre-merge-escalation-gate.mjs` 一行正则）+ G2 文档面（verify-gates.json `_doc` 注、pod-coordination.md 门名同步、ARQ 队列）。G1/契约面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/merge-driver.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/verify-gates.json
.claude/references/pod-coordination.md
docs/ARCH-REVIEW-QUEUE.md
changes/arch/260905-gov-review-verify-gates-align/PROPOSAL.md
```

> manifest 注：6 路径（R1-F1 修订：pod-coordination.md 实点门名须同批同步，增补入 manifest）。

---

## 0. 想实现的效果与差距

**目标效果一句话**：`merge-driver` 的 quick-check 冒烟闸在本仓真实可用——id 表、字面 oracle、verify-gates.json 单源三处自洽，`--self-check-quick-checks` 全绿，且**任何一次真实 `merge-driver --pod <id>` 合并（含 APPROVE 包）不再在 `buildQuickChecks` 处无条件 FATAL**——机械合并通道自迁移以来首次真正恢复。

**现状差距**：`merge-driver.mjs:807` 主流程无条件调 `buildQuickChecks`（先于 DRY_RUN 判断），按 `QUICK_CHECK_IDS = ['types','schema','contract-lint','acceptance']`（`:403`）从 verify-gates.json 取门；本仓 json 无 `types`/`schema` id（迁移 `_doc` 诚实记录了折算决策但没留映射条目）→ `:425` errorExit。后果三层：① 自检恒 FATAL（2/3）；② **运行时全死**——本仓近期所有合并（Pod S `e33278d`、G 包、host-test-infra）全为手工，此前仅归因 ESCALATE 无人批路径，本 ARQ 复查证实此为其二真根因；③ 即便补齐 json，字面 oracle `EXPECTED_QUICK_CHECK_CMDS`（`:407-411`，2 条）与 4 门 id 表结构上永不相等——自检 oracle 自身矛盾。

**非目标（本批不做）**：
- 不给 Python/server 面新增 types 闸（mypy 启用与否是独立决策，`_doc` 缺口记账不动）。
- 不改 verifier（verify.mjs）的 scoped/full 相位门集——那是既有单源，本批只对齐 merge-driver 的冒烟子集。
- 不改 merge-driver 其余合并链语义（rebase/quick-check 重试/human-exemption/token 串行化均不动）。
- 不追记历史手工合并的补录手续（Pod S/G 包合并已成事实，留根不翻账）。

## 1. 问题与根因（地面真值）

### 三层错位（证据行号 2026-09-05 实核，HEAD `aaf0067`）

- **层 1 · json 缺映射条目**：`.claude/verify-gates.json` gates 数组（`:27-83`）九门：taskpackage-required / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance——无 `types`、无 `schema`。`_doc`（`:19-26`）明记折算决策：「schema 闸等价物 = firmware-build + contract-lint」「types 闸：server 面 Python 无强制类型闸……记为缺口」。**折算做了、id 映射没留**。
- **层 2 · 运行时无条件死**：`merge-driver.mjs:807` `const quickChecks = buildQuickChecks(projectDir);` 在 `:808` DRY_RUN 判断**之前**无条件执行；`buildQuickChecks`（`:420-428`）按 id 表逐个 `byId.get(id)`，取不到即 `errorExit`（`:425`）。实测（2026-09-05）：`node .claude/scripts/merge-driver.mjs --self-check-quick-checks` → `FATAL: verify-gates.json missing quick-check gate id="types" (ARQ-097 single source)`；HEAD 复跑同报错（存量，非本批引入——`git stash` 实证于 260904 批 landing V3）。
- **层 3 · oracle 自身矛盾**：`EXPECTED_QUICK_CHECK_CMDS`（`:407-411`）= 2 条（contract-lint + acceptance-run-check），注释明写「本仓：无 tsc/prisma 等价物——结构闸走 idf.py build……契约闸走 contract-lint」；而 `QUICK_CHECK_IDS`（`:403`）仍是源仓 4 门。`runQuickChecksSelfCheck`（`:451-459`）断言 gotCmds（4 门时 4 条）== EXPECTED（2 条）恒假——**即便层 1 补齐，自检仍红**。迁移意图的地面真值 = oracle 的 2 门，id 表是漏改的源仓残留。

### 相邻既有错位（OQ2 是否折入）

- `pre-merge-escalation-gate.mjs:114`（260904 批热修后行号）`readGovAnchors` 的 frozen 判据 `/^\*\*状态：FROZEN v\d+\.\d+/m` 要求行首 `**状态：`，而本仓 PROPOSAL 惯用 `> ` 引用前缀——**该机锚在本仓从未命中过任何实际 PROPOSAL**（含已合并的 260830 批）。260904 批被迫把 PROPOSAL 状态行去前缀适配机器（如实记录于其 landing summary §4）。同目标（机械合并/合并门可用）、一行修，是否折入本批 = OQ2。

## 2. 改法（T1–T5）

### T1 · merge-driver 冒烟子集对齐（两门案，OQ1 裁定为准）

改法（对层 1+2+3）：

1. `QUICK_CHECK_IDS`（`:403`）源仓 4 门改为本仓 2 门：`['contract-lint', 'acceptance']`；注释同步改写：说明 firmware-build（types+schema 折算物）**有意不入冒烟子集**——真跑 idf.py build 分钟级不「quick」、无工具链时 fail-open（多数开发机等于没拦）、verifier scoped 相位已在 ready-to-merge 前跑过它；冒烟子集保留「契约结构 + 验收入口」两个快门。
2. `EXPECTED_QUICK_CHECK_CMDS`（`:407-411`）**保持 2 条不动**（它就是迁移意图的地面真值，本批向它对齐）；注释补一句「260905 对齐：QUICK_CHECK_IDS 由源仓 4 门残留改为本仓 2 门，本 oracle 未动」。
3. `buildQuickChecks` / `runQuickChecksSelfCheck` / `warnWorktreeGateDivergence` 逻辑不动——id 表对齐后三者自然自洽。
4. `:807` 无条件 `buildQuickChecks` 位置不动（取门成功后 DRY_RUN 语义不变）；运行时恢复由 T1.1 自然达成（取得到门即不再 FATAL）。
5. 同文件陈旧门名文案同步（R1 NIT + census 已录）：`:687-688` 豁免例外注释「其余 types/schema/contract-lint 已过、照拦」→「其余 contract-lint 已过、照拦」；`:704` warn 文案同串同步。注释/warn 是对 id 表的历史快照，id 表改后两者必须同批对齐，防读码误导。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T2 · verify-gates.json `_doc` 补记

改法（防 SSOT 再漂移）：`_doc` 数组追加一行注：「merge-driver quick-check 冒烟子集 = contract-lint + acceptance 两门（260905 对齐，QUICK_CHECK_IDS 单源消费）；firmware-build 有意不入冒烟（分钟级 + fail-open），由 verifier scoped 相位承担；types 闸缺口照旧（ARQ-260827-contract-mech-gate-gap 邻域）」。

目标文件：`.claude/verify-gates.json`（改 `_doc` 注，gates 数组零改动）

### T3 · ARQ 移已结 + 索引钉行

改法：landing 完成 + landing-verifier 绿后，`docs/ARCH-REVIEW-QUEUE.md` 把 `ARQ-260905-verify-gates-types-missing` 从 §1.2 移除、§2 已结索引加一行（evidence 指本目录，注明 OQ1/OQ2 裁断与三层修复对照），并跑 `node .claude/scripts/changes-index.mjs --upsert 260905-gov-review-verify-gates-align`。

目标文件：`docs/ARCH-REVIEW-QUEUE.md`（改）

### T4 · 升级闸 FROZEN 机锚正则折算修复（OQ2=(a)）

改法：`pre-merge-escalation-gate.mjs` `readGovAnchors` frozen 正则 `/^\*\*状态：FROZEN v\d+\.\d+/m` 放宽为容忍引用前缀形态（`/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m`），使仓内惯用 `> **状态：FROZEN vX.Y …` 与 260904 批行首形态**双命中**；260904 批 PROPOSAL 状态行保持现状不动（两形态都命中，互为冗余）。升级闸 self-check gov 档用例是注入式（不经正则），正则正确性由 §5 V5 双形态断言担纲。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改一行）

### T5 · pod-coordination.md 门名同步（R1-F1）

改法：`.claude/references/pod-coordination.md:72`「并跑 quick-check（firmware-build + contract-lint + acceptance）抓 rebase 后集成问题」→「并跑 quick-check（contract-lint + acceptance，260905 对齐后两门案；firmware-build 有意不入冒烟——分钟级 + fail-open，由 verifier scoped 相位承担）抓 rebase 后集成问题」。该行是 pre-existing 陈旧（点名源仓三门集形态），R1-F1 判定按锚点纪律须同批同步，故入 manifest。

目标文件：`.claude/references/pod-coordination.md`（改一行）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

census 命令（2026-09-05 实跑）：`grep -rn "QUICK_CHECK_IDS\|EXPECTED_QUICK_CHECK_CMDS\|buildQuickChecks\|verify-gates" .claude/ --include='*.mjs'` + `grep -rn "quick.check\|冒烟" .claude/references/ .claude/agents/ docs/ --include='*.md'`。逐条分类：

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 合并执行 | `.claude/scripts/merge-driver.mjs` | G3 改 | T1，id 表对齐（:403）+ 注释（:400-411） |
| G3 门单源 | `.claude/verify-gates.json` | G2 改 | T2，仅 `_doc` 追注；gates 数组、verify.mjs 消费语义零改动 |
| 工作队列 | `docs/ARCH-REVIEW-QUEUE.md` | 改 | T3，landing 后 ARQ 移已结 |
| G3 升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 改 | T4（OQ2=(a)），readGovAnchors frozen 正则放宽一行 |
| verifier 消费侧 | `.claude/hooks/verify.mjs` | 未触 | `loadGates()` 读全量 gates，不消费 QUICK_CHECK_IDS；本批不改其门集 |
| merge-driver 自检 | `--self-check-quick-checks`（`:451-459`） | 未触（逻辑） | 逻辑不动，对齐后自然转绿（V1 验证） |
| merge-driver D1 分歧 warn | `warnWorktreeGateDivergence`（`:432-448`） | 未触 | 消费 mainQuickChecks（对齐后 2 门），worktree 侧同 id 比对，逻辑自洽 |
| 既有豁免先例 | `--human-approved-exemption`（`:677-699` 形态） | 未触（语义） | acceptance 门豁免语义零改动；其注释（`:687-688`）与 warn 文案（`:704`）中 types/schema 字样经 T1.5 同批对齐 |
| prose 引用 | `.claude/references/dev-loop.md` / `escalation-playbook.md` | 未触 | 两处只提「quick-check 重试 / 闸绿」，不点门名，无陈旧风险（R1-F1 修订后口径） |
| prose 引用（实点门名） | `.claude/references/pod-coordination.md` | G2 改 | T5，:72 三门集名改为两门案表述（R1-F1：pre-existing 陈旧须同批同步） |
| DEV-GOVERNANCE | `docs/DEV-GOVERNANCE.md` | 未触 | §6 强制执行层行覆盖 G3 脚本存在性，不涉门集构成 |
| 本批制品 | `changes/arch/260905-gov-review-verify-gates-align/` | 新增 | PROPOSAL + landing-summary / verdict / retro 随七步产出 |

## 4. human 取舍点（OQ）

> **裁决结果（human，2026-09-05，随立项 ack 同场裁断）：OQ1=(a)、OQ2=(a)。** §2 各 T 按裁断落地：T1 两门案 `['contract-lint', 'acceptance']`；T4 正式件（升级闸 FROZEN 机锚正则放宽）。

### OQ1 · quick-check 冒烟子集的门构成

- **(a)（推荐）两门案**：`['contract-lint', 'acceptance']`。理由：向迁移意图地面真值（EXPECTED oracle 注释）对齐；`firmware-build` 真跑分钟级不「quick」、无工具链 fail-open 等于多数开发机没拦、verifier scoped 相位已承担；冒烟保「契约结构 + 验收入口」两快门，post-rebase 冒烟语义保留。
- (b) 三门案：加 `firmware-build`。post-rebase 编译冒烟更强，但 merge 每包多分钟级、失败重试翻倍，且 fail-open 让保护在有工具链机器上才生效——收益/成本不称。
- (c) 四门案：json 补 `types`/`schema` 条目（schema→firmware-build 别名）。保源仓形态但制造伪门/别名门，违 ARQ-097 单源诚实语义，不推荐。

### OQ2 · 升级闸 FROZEN 机锚正则错位是否折入本批

- **(a)（推荐）折入**（= T4 生效 + manifest 增行）。理由：同目标（机械合并通道可用）同量级（一行正则）、本批已实证 260904 批被迫「文档适配机器」的反向错位，顺手归位成本低；分开走要再立一track。
- (b) 不折：留观察（已在 ARQ-260905-execfn 已结 evidence 里记账），本批 manifest 只 4 行。日后治理批再收。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · quick-check 自检转绿**：`node .claude/scripts/merge-driver.mjs --self-check-quick-checks` → `ok:true` 且 `gotIds == ["contract-lint","acceptance"]`、`gotCmds == EXPECTED_QUICK_CHECK_CMDS`（2 条）。
- **V2 · 语法闸**：`node --check .claude/scripts/merge-driver.mjs`（OQ2=(a) 时加 `pre-merge-escalation-gate.mjs`）零输出通过。
- **V3 · merge-driver 既有自检回归**：`--self-check-detect-changes` / `--self-check-rebase-target` 全绿（行为保持）。
- **V4 · 运行时通道 live 冒烟**：`node .claude/scripts/merge-driver.mjs --dry-run --pod <任一 changeId>` 与 `node .claude/scripts/merge-driver.mjs --dry-run` 输出**不再含** `FATAL: verify-gates.json missing`，转而走到后续阶段（no ready-to-merge pods / not in ready-to-merge state 等）——证明 `:807` 取门通过、运行时 FATAL 消除。真空绿规约：断言 FATAL 字样零命中。
- **V5（条件件，OQ2=(a)）· FROZEN 锚双形态断言**：`node -e` 对 `readGovAnchors` 同正则分别测 `> **状态：FROZEN v1.0`、行首 `**状态：FROZEN v1.0`、`**状态：DRAFT` 三输入 → 前二 frozen:true、第三 false；升级闸 self-check 18 例回归全绿。
- **V6 · 索引钉行**：`node .claude/scripts/changes-index.mjs --upsert 260905-gov-review-verify-gates-align` 单行进 `changes/INDEX.md` arch 段。

## 6. 留根清单

- **types 闸缺口照旧**：server 面无强制类型闸（mypy 未启用），`_doc` 缺口记账不动；将来启用 mypy 时新增真门 + 重新评估冒烟子集是否纳入。
- **post-rebase 固件漂移残余**：两门案下 rebase 后固件编译冒烟不在 merge 期跑，依赖 verifier scoped 前置 + acceptance 相位台架兜底；若将来出现「rebase 引入固件编译破裂且闸未拦」实例，重启 OQ1(b) 评估。
- **历史手工合并不翻账**：Pod S / G 包 / host-test-infra 的手工合并归因（ESCALATE + 本 ARQ 双根因）已如实记录于各自证据面，不做补手续。
- **方向 B 留根 10 销账前置条件齐备**：本批 + 260904 批合计恢复机械合并通道全部环节（升级闸人批口 + 运行时取门），G/WS 包 ESCALATE 合并时可全机械走通。
- **retro 义务（R1 NIT）**：「迁移意图地面真值 = EXPECTED oracle 2 门」的表述与 260829 迁移批记录的适配映射存在历史分歧，retro 须记一句澄清（本批以 git 历史 + 注释自洽性为准据，迁移批记录面如有冲突以本批 R1 实核结论收口）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect 判官评审（2026-09-05，HEAD `aaf0067`，medium tier）

**证据实核（抽核 ≥ 一半关键项，全部命中）**：

- `merge-driver.mjs:403` QUICK_CHECK_IDS 四门逐字 ✓；`:407-411` EXPECTED_QUICK_CHECK_CMDS 2 条 + 注释「本仓：无 tsc/prisma 等价物……」逐字 ✓；`:420-428` buildQuickChecks / `:425` errorExit 文案逐字 ✓；`:451-459` runQuickChecksSelfCheck 断言结构 ✓；`:704` warn 文案「types/schema/contract-lint already passed and remain enforced」逐字 ✓。
- `:795-807` main 流实读：`:807` 无条件 `buildQuickChecks` 先于 `:808` DRY_RUN 判断；`--pod`/`--dry-run` 旗标解析实存（`:36-41`）——V4 断言路径确覆盖运行时 FATAL 点 ✓。
- `verify-gates.json` `:19-26` `_doc` 折算注（schema→firmware-build+contract-lint 在 `:20-22`、types 缺口在 `:25`）、`:27-83` gates 九门、无 `types`/`schema` ✓。
- 升级闸 `pre-merge-escalation-gate.mjs:114` frozen 正则 `/^\*\*状态：FROZEN v\d+\.\d+/m` 逐字 ✓（260904 批热修后行号命中）。
- 实测复现：`--self-check-quick-checks` → `FATAL: verify-gates.json missing quick-check gate id="types" (ARQ-097 single source)` exit 1，与 §1 层 2 引文逐字一致 ✓；升级闸 `--self-check` 实测 18/18 绿（§5 V5「18 例」数字正确）✓。
- HEAD 复核：`git rev-parse` = `aaf0067` 与 §1 声明一致 ✓；ARQ `ARQ-260905-verify-gates-types-missing` 在 `docs/ARCH-REVIEW-QUEUE.md:37`（§1.2）在场 ✓。

**改法论证核查**：

- **EXPECTED oracle「迁移意图地面真值」可证伪性——部分成立**：`git log -L` 证明 merge-driver 于 `4e9d776`（档 2/3 迁移）一次性引入，EXPECTED oracle 注释当时即为本仓语境（「本仓契约面 docs/contracts/**」「档 2 新写」），QUICK_CHECK_IDS 保留源仓 4 门形态——「id 表是漏改的源仓残留」成立。但 `changes/arch/260829-gov-review-migration-tier23/PROPOSAL.md:43` 迁移批自身适配表记录的 quick-check 映射为「tsc/prisma → firmware-build/contract-lint」，与 oracle 2 门（contract-lint + acceptance）不一致——「迁移意图」存在两份冲突的历史记录（详见 R1-F4）。OQ1=(a) 的裁断效力不依赖此论证（分钟级 / fail-open / verifier scoped 承担三条独立成立），T1 两门案照裁断执行。
- **T1 运行时恢复论证成立**：`:807` 取门通过即不再 errorExit；`buildQuickChecks`/`runQuickChecksSelfCheck`/`warnWorktreeGateDivergence` 三者均纯按 id 表驱动，2 门下逻辑自洽（`:432-448` worktree 分歧按同 id 比对，2 门收窄无害）。
- **census 补查**：`QUICK_CHECK_IDS|EXPECTED_QUICK_CHECK_CMDS|buildQuickChecks` 全仓 `.mjs` 仅 merge-driver.mjs 一处消费 ✓；verify.mjs 自带独立 `loadGates()`（`:57`）按 timing/phase 过滤全量 gates、不消费 id 表（§3「未触」判定成立）✓。
- **安全方向**：冒烟子集「只减不增」实为「从死门集合到活门集合」——现行 4 门中 types/schema 本就是取不到的 FATAL 死门，firmware-build 从未真正入过合并期冒烟，T1 无新增放行路径；post-rebase 固件漂移兜底 = verifier scoped 相位 firmware-build（timing=scoped 每轮跑，`verify-gates.json:38`）+ §6 留根重启 OQ1(b) 条件，闭环成立。T4 正则放宽评估见 R1-F5（FYI）。

**findings**：

- **R1-F1（SHOULD，打回）· census prose 行断言被证伪**：§3 表「prose 引用」行称三处 references「不点门名，无陈旧风险」——实核 `.claude/references/pod-coordination.md:72` 明写「并跑 quick-check（firmware-build + contract-lint + acceptance）抓 rebase 后集成问题」，点名的三门集既不同于现行 4 门代码、也不同 T1 后 2 门（pre-existing 陈旧：代码里 firmware-build 从未入过 QUICK_CHECK_IDS）。按 `arch-review-protocol.md`「机械面枚举的锚点纪律」（注册表变更批次须同批核对指向它的 prose 锚点是否仍实，判官评审义务项），本批必须处置。**打回改法**：pod-coordination.md:72 门集文案同步为两门案（或去硬编码门名、改指 `verify-gates.json` 注册表锚点），折入 T2 或新增 T5；landing-manifest 相应增行 `.claude/references/pod-coordination.md`（5→6 路径）。
- **R1-F2（SHOULD，打回）· 头部 packet-id 行与改法集不一致**：PROPOSAL 头部结构字段「packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集」写死 T1–T3，而 §4 顶部裁决行（OQ2=(a)）后 §2 实含 T4、manifest 已 5 路径。头部硬条款要求「GREEN 结论前必须在场（且判官核对）」。**打回改法**：packet-id 行更新为 T1–T4（并按 R1-F1 结果同步路径数）。
- **R1-F3（NIT，不阻塞）· `:687` 注释同类陈旧漏面**：`merge-driver.mjs:687` 豁免注释「其余 types/schema/contract-lint 已过、照拦」与 `:704` warn 串同类陈旧，census/T1 只覆盖 `:704`。建议并入 T1 注释同步（同一 manifest 文件面内，无 scope 扩张）。
- **R1-F4（NIT，不阻塞）· 「迁移意图地面真值」表述需收敛**：如上核查所示，260829 迁移批记录的适配映射（firmware-build/contract-lint）与 oracle（contract-lint/acceptance）冲突。建议 T2 `_doc` 注或本批 retro 记一句该历史分歧，避免后人考古时以 260829 记录否定本批对齐方向。
- **R1-F5（FYI，不阻塞）· T4 正则放宽误判面有界**：`/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m` 的新增误判面 = 正文 / 代码围栏内引用形态的 FROZEN 状态行（原正则对行首形态已有同类面）；误判方向为 frozen:true（fail-open），但 gov 档另需 green 锚 + landing-verdict.md 在场双锚兜底，爆炸半径有界。`**状态：DRAFT` 不误匹配（V5 第三断言覆盖）。可接受；日后可收紧为限文件头部 N 行。

**结论：需修订**。核心改法（T1 两门案 + T2/T3/T4）经实核成立、无 BLOCKING，但两处 SHOULD 打回项（R1-F1 census prose 锚点、R1-F2 packet-id 行一致）须在 FROZEN 前修订落稿；两处 NIT 建议顺手折入。修订后判官快速复核（不重跑全轮）即可 GREEN。

**packet-binding 现状核对**：```landing-manifest``` 围栏块在场（5 路径）且与 T1–T4 改法文件集一致；packet-id 行在场但内容滞后（R1-F2）。GREEN 前须达成：manifest 与修订后改法集（含 F1 可能的 6 路径）一致 + packet-id 行同步。

### R2 · architect 判官复核（2026-09-05，HEAD `aaf0067`，medium tier）

**R1 打回项逐条复核（4/4 到位）**：

- **R1-F1 → 到位**。§3 census「prose 引用」行拆分为两行：`dev-loop.md` / `escalation-playbook.md` 维持「未触」（修订后口径）；`.claude/references/pod-coordination.md` 升类「G2 改」并新增 **T5**（`:72` 三门集名「firmware-build + contract-lint + acceptance」→ 两门案表述，引文与实文件 `:72` 逐字吻合）。landing-manifest 增补 `pod-coordination.md` → **6 路径**，manifest 注同步注明 R1-F1 修订缘由。补充实核：pod-coordination.md 全文仅 `:72` 一处点名门集（`:69`/`:82` 的 acceptance 为相位名非门集枚举，不陈旧）——T5 范围恰当且完备；`dev-loop.md` / `escalation-playbook.md` 实存且全文无 quick/门名提及（census「只提 quick-check 重试」措辞偏宽，但「不点门名、无陈旧风险」的承重断言保守成立，FYI 不阻塞）。
- **R1-F2 → 到位**。头部 packet-id 行更新为「§2 改法集（T1–T5）+ §5 验证集」（T1–T4 基础上按 F1 折入结果取 T1–T5 超集，正确）；§2 标题同步「改法（T1–T5）」；状态行 v0.3 待 R2，口径自洽。
- **R1 NIT-1 → 到位**。折入 **T1.5**：`:687-688` 豁免注释「其余 types/schema/contract-lint 已过、照拦」→「其余 contract-lint 已过、照拦」，`:704` warn 串同批对齐，并补「注释/warn 是 id 表历史快照须同批对齐」的防再漂移理由；census 对应行（既有豁免先例）同步标注 T1.5。改动均在 merge-driver.mjs 单文件面内，无 scope 扩张。
- **R1 NIT-2 → 到位**。§6 留根新增 retro 义务行：澄清「迁移意图地面真值 = EXPECTED oracle 2 门」与 260829 迁移批记录适配映射的历史分歧，以本批 R1 实核（git 历史 `4e9d776` + 注释自洽性）为准据收口——与 R1-F4 建议一致。
- R1-F5（FYI）无需动作，维持原判。

**manifest ↔ T1–T5 改法集一致性核对（恰为并集，无缺无余）**：

| T | 目标文件 | manifest 在场 |
|---|---|---|
| T1（含 T1.5） | `.claude/scripts/merge-driver.mjs` | ✓ |
| T2 | `.claude/verify-gates.json` | ✓ |
| T3 | `docs/ARCH-REVIEW-QUEUE.md` | ✓ |
| T4 | `.claude/scripts/pre-merge-escalation-gate.mjs` | ✓ |
| T5 | `.claude/references/pod-coordination.md` | ✓ |
| 制品 | `changes/arch/260905-gov-review-verify-gates-align/PROPOSAL.md` | ✓ |

**结论：GREEN（可冻结）**。R1 全部打回项与 NIT 修订到位，无新增 finding；核心改法（T1 两门案运行时恢复 + T2 单源补记 + T3 队列收口 + T4 正则放宽 + T5 prose 锚点同步）经 R1 实核成立、human OQ1=(a)/OQ2=(a) 裁断在案。

**packet-binding 确认（GREEN 前提核对，在场且一致）**：```landing-manifest``` 围栏块在场，恰 6 路径，与 T1–T5 改法文件集 + 本批制品逐一对应（见上表）；packet-id 行在场，内容「§2 改法集（T1–T5）+ §5 验证集」与实际改法集一致。主线程标 FROZEN 前按头部硬条款再人工复核一次。
