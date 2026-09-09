# architect-landing-summary · 260909-gov-review-gate-truthfulness

> 主线程 packet-bound landing 执行记录（2026-09-09）。packet = FROZEN PROPOSAL v1.0（§2 T1–T7 + §5 V1–V7）+ landing-manifest（8 行）+ 本文件。判官 GREEN = R2（architect-review-r1.md R2 节）。

## 1. 落点对照（manifest 8/8）

| # | manifest 路径 | 改法 | 落地实况 |
|---|---|---|---|
| 1 | `.claude/scripts/acceptance-run-check.mjs` | T1+T2 | ✅ 文件级排除删除；`-m "not live"` + `--import-mode=importlib` 条目级筛；`fileHasLiveMark` 降为统计（filesWithLiveMark 字段）；collect 0 条 → `allLiveDeselected:true`；自检 +2 |
| 2 | `.claude/scripts/unit-run-check.mjs` | T1+T2 | ✅ 同构落地（同函数形、同字段、同自检 +2） |
| 3 | `.claude/scripts/firmware-build-check.mjs` | T3 | ✅ 双路探测（PATH idf.py ∨ venv python + `c:/tmp/idf_run.py` wrapper，顶部常量 + skill 同源注释，R1-C3）；降级带 `buildAttempted:false`/`toolchainProbes`/`strictModeEnv`；真编命令按通路选择；自检 +2 |
| 4 | `.claude/hooks/verify.mjs` | T4 | ✅ `liftNotExecuted`（degraded ∨ executor-skipped ∨ allLiveDeselected → `notExecuted`/`notExecutedReason` 进 results[]）+ `summarizeNotExecuted` 汇总（载体见偏差 D1） |
| 5 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | T5 | ✅ 步骤 2 已知红计数（绝对路径口径 + progress.md 落行 + knownRedFloor 改引用）；步骤 7 环境段 +3 条（VERIFY_FW_BUILD_REQUIRED=1 注入 / brief-inputs.mjs 两句 / C 侧 root 指针只登记） |
| 6 | `.claude/scripts/changes-index.mjs` | T6 | ✅ 末 commit 判据：`gitLastCommitSha` + `dirMergedInMain` 单函数双路同调（upsertArchRow + backfill）；`:16`/`:89` 注释同步 |
| 7 | `.claude/references/arch-review-protocol.md` | T7 | ✅ 步 7 改 ff-only 口径（含 R1-B1 更正后的「3 例旧文本 --no-ff」诚实括注） |
| 8 | `docs/DEV-GOVERNANCE.md` | T1c+T4 义务 | ✅ §1 末合并落一段：条目级两态判据 + `allLiveDeselected` 显式报 + `notExecuted` 非零 verifier 报告逐条点名义务 |

## 2. 偏差申报（landing == frozen 的偏离，逐条）

- **D1 · T4 汇总字段载体迁移（V4 字面不可满足 → 等价载体）**：frozen V4/T4 写「`gatesNotExecuted`/`gatesRun` 进 round fact」；实测 round fact 顶层键白名单封闭（`verifier-evidence-check.mjs:281` `unknown field` 即拒，该文件**不在 manifest 内**、本批零改动是其 §3 明文承诺）。落地形态 = 逐闸 `notExecuted`/`notExecutedReason` **确在** round fact（`results[]` 条目无键白名单，随 fact 持久化）；汇总 `gatesNotExecuted`/`gatesRun` 落 **child/parent payload + verdict.json** 双载体。目标效果（「上游与仪表盘能判」+ verifier 点名义务读 `results[].notExecuted`）不损；若须 fact 顶层键，留根下批连同 verifier-evidence-check 白名单一起做。**此条请 landing-verifier 重点判**（判 RED 则按先例走冻结层追认链升 human）。
- **D2 · V3 执行通道**：frozen V3 写「直调 `decide()`」；`decide` 非模块导出，仓内直调通道 = `--self-check` 决策电池。V3 实际经自检电池执行：`idf-missing-degraded-green`（= decide({fwPresent:true,idfOk:false,required:false}) 输入字面）+ `degraded-fields-present`（buildAttempted===false ∧ toolchainProbes 数组 ∧ strictModeEnv 字符串）+ `idf-missing-required-red`（required:true → ok:false）。三断言全绿，覆盖 V3 两臂。
- **D3 · acceptance 闸落地即红（预期向严，V7 首证）**：T1+T2 落地后 acceptance 闸对现仓真跑首轮 = **8 failed / 119 passed / 41 deselected**（`test_smoke_e2e` 7 条 + `test_voice_roundtrip_b` 1 条历史潜伏红）。非本批缺陷、本批不修断言（非目标条）；按 T5 新机制即三个待 launch Pod 的「上游已知红」基线（8 条）。dev 面消解走 dev-loop/独立批。

## 3. 验证集执行记录（V1–V7）

- **V1 ✅** 三闸自检全绿。实际自检名（R1-C4 回填）：`mixed-file-structural-runs` / `all-live-deselected-reported`（两 pytest 闸各 +2）；`toolchain-dual-probe` / `degraded-fields-present`（firmware-build +2）。与冻结名单逐字一致，无凑名。
- **V2 ✅** 闸内真跑实证：`node .claude/scripts/acceptance-run-check.mjs` → 输出含 `phase:"executor-run", structuralCollected:127, liveDeselected:41, filesWithLiveMark:"9/9"`，**不再 executor-skipped**；pytest 汇总 `8 failed, 119 passed, 41 deselected`，闸 exit=1（红带 collected/deselected 数字与失败条目 id）。collect-only 对照 `14/18 tests collected (4 deselected)`。
- **V3 ✅** 经自检电池（见 D2）；另实测 `toolchain-dual-probe` 现跑 = `PATH:idf.py=false | venv+wrapper=true`——双路探测在本机真实接电。
- **V4 ✅** fixture 轮（`C:\tmp\v4-fixture`，CLAUDE_PROJECT_DIR 重定向 + 3 stub 闸 + 最小 verify-gates.json，用后已清理）：逐闸 `notExecuted` = {stub-degraded:true, stub-skipped:true, stub-real:false}，汇总 `gatesNotExecuted:[stub-degraded,stub-skipped]`、`gatesRun:1`（payload + verdict.json 双载体），**该轮 ok:true 不因新字段改变**（向严不越界证伪点达成）。
- **V5 ✅** 调用级残留：`fileHasLiveMark(join` 仅 2 命中 = 两闸报告用途计数行（V5 明文豁免）；`live-only on-target` = 0。六模式集命中数：fileHasLiveMark 5 / hasLiveMark 0 / live-only 0 / executor-skipped 7 / pytest\.mark\.live 3 / pytestmark 2——全为定义/报告/成文面，零排除级调用残留。
- **V6 ✅** FROZEN 转移点 upsert：旧判据写 `frozen|yes`（commit c110956 前实测）、T6 落地后同点重跑 → `frozen|no`（负例转正机判证据）；`--backfill` 重钉 19 目录后本批行不翻转（yes 18 / no 1 = 本批）；protocol `:61` ff-only 在场、旧 `--no-ff 可回滚` 表述零残留。
- **V7 ✅（首证，持续观察交日落复盘）**：本批自身 landing 即触发首轮真跑红（D3 的 8 条），红全部带 collected/deselected 数字与失败条目 id，零「一条都没跑却 ok:true」。三个待 launch Pod 为后续 pilot。
- **附 · T3 端到端实证（计划外硬证据）**：landing 期本闸在主仓真跑一轮完整 `idf.py build`（经 venv+wrapper 路），`phase:"build-pass"` 真编译通过——46/46 全 degraded 的历史在落地当天终结。

## 4. 自指面与越界核对

- T7 改动已按新口径自洽（本批后续 merge 将按 ff-only 执行 = 新旧文本在该场景同一动作，判官 R1-F 节判定维持）。
- 契约面零触（manifest 无 `docs/contracts/**`，落地零触）；自有实现面零触（`server/pytest.ini`、`firmware/main/test/**` 未动，C 侧只写指针）；`verify-gates.json` 门数/门序零改动；`verifier-evidence-check.mjs` 零改动（白名单封闭即 D1 成因）。
