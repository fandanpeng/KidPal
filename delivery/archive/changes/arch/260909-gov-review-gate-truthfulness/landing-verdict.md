# landing-verdict · 260909-gov-review-gate-truthfulness

> landing-verifier 独立核验（2026-09-09，两轮）。packet = FROZEN PROPOSAL v1.0（c110956 钉冻；489e3dc 冻结层追认格式修正在册、零语义改动）+ landing-manifest（PROPOSAL :13-22 / summary 围栏块，逐字一致）+ architect-landing-summary。landing commit = `757ba61`，格式修复 commit = `489e3dc`。本 verdict 只读 codebase、只写本文件。

## 总结论（终态）

**ok: true（GREEN）——放行建议（merge 仍归 human manual-merge 门；PROPOSAL §8 追认段随 merge 前由 human 确认）。**

- **结构门（两轮）**：第一轮 ok:false（summary 缺 manifest 块，B2 fail-loud）。主线程 489e3dc 修两格式项后**第二轮复跑 ok:true**（baseRef=c110956；`frozen:true`、`packetBinding:true`、proposalTargets == summaryTargets（8/8）、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`、`changedCanonicalNotInProposal:[]`、verbatimQuoteAdvisory 无 missing）。**设计侧门 N/A（Track B）**。
- **语义保真**：manifest 8 文件逐文件人读 diff（c110956..757ba61）全部 == PROPOSAL §2 T1–T7 改法（第一轮详核结论维持有效——489e3dc 只动批内两件证据件、零 canonical 触面）。
- **D1 判可接受**（等价载体、目标效果不损、申报在册，理由与亲跑证据见下）。
- **验证集 V1–V6 全部亲跑复验一致**；V7 首证在册。

## 第一轮 RED 与两修复点复核（489e3dc）

| 项 | 修复内容 | 本 verifier 复核 |
|---|---|---|
| B2/A9 · summary 缺 manifest 块 | summary 补 ```landing-manifest 围栏块 + packet-binding 引述行 | 块 8 行与 PROPOSAL :13-22 **byte 级 diff 空集**（MANIFEST_VERBATIM_IDENTICAL）；引述行含 packet-id + 被批 review id ✓ |
| B7 · §2 锚行连写形态 | T3/T4/T5/T6 四条「对应 Px。目标文件：`…`」拆为「目标文件：」独立行 | diff 亲核 = 纯拆行、零语义改动（改法集/目标文件集/manifest 三者零变动）；B7 锚对账机检现过（§2 锚并集 ≡ manifest 8）✓ |
| 冻结层追认段（PROPOSAL §8） | 两 item 追认 + 260908 §9 先例 + 追认人 human 随 merge 前确认 | 形态在册、归因准确（B7 只认行首形态、B2 双侧含块）、零语义改动声明与 diff 实况一致；追认链闭合条件（human merge 前确认）已明文 ✓ |

## 逐文件语义核对（git diff c110956 757ba61 亲读，第一轮结论维持）

| # | file | structuralTouched | semanticMatch | expected（PROPOSAL §2） | actual（diff 亲核） |
|---|---|---|---|---|---|
| 1 | `.claude/scripts/acceptance-run-check.mjs` | ✓ | ok | T1+T2：删文件级排除；`-m "not live"`+`--import-mode=importlib` 条目级筛；fileHasLiveMark 降为统计；collect 0 → `allLiveDeselected:true`；自检 +2 | 完全一致（pytestCollectNotLive/parseCollectSummary、执行串、allLiveDeselected 分支、自检名 `mixed-file-structural-runs`/`all-live-deselected-reported`、fail-safe 语义保留）✓ |
| 2 | `.claude/scripts/unit-run-check.mjs` | ✓ | ok | T1+T2 同构 | 与 #1 同形同字段同自检名 ✓ |
| 3 | `.claude/scripts/firmware-build-check.mjs` | ✓ | ok | T3：双路探测常量 + skill 同源注释；降级 `buildAttempted:false`/`toolchainProbes`/`strictModeEnv`；required 判据保留（OQ1=(b) 显式开启） | 完全一致（`IDF_VENV_PYTHON`/`IDF_RUN_WRAPPER` + 同源注释、probePathIdf/probeVenvWrapper/probeToolchain、decide 三字段、按通路选真编命令、自检 `toolchain-dual-probe`/`degraded-fields-present`）✓ |
| 4 | `.claude/hooks/verify.mjs` | ✓ | ok（含 D1 申报偏差） | T4：liftNotExecuted → `notExecuted`/`notExecutedReason` 进 results[]；汇总 `gatesNotExecuted`/`gatesRun`；不改 ok 判定 | liftNotExecuted/summarizeNotExecuted 落地；results[] 逐项 spread；汇总进 child payload + verdict.json + legacy 路；ok 判定路径零改动 ✓ |
| 5 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | ✓ | ok | T5 三 item（步骤 2 已知红机械来源绝对路径口径 + progress.md 落行 + knownRedFloor 引用；环境段 ①VERIFY_FW_BUILD_REQUIRED=1 ②brief-inputs 两句 ③C 侧 root 指针只登记） | 三 item 全落，无改码越界 ✓ |
| 6 | `.claude/scripts/changes-index.mjs` | ✓ | ok | T6：末 commit 判据、派生抽单函数双路同调、`:16`/`:89` 注释同步、status 与 mergedInMain 解耦 | `gitLastCommitSha` + `dirMergedInMain`、两派生点同改、注释更新；首 commit 函数保留供日期派生（正当复用）✓ |
| 7 | `.claude/references/arch-review-protocol.md` | ✓ | ok | T7：步 7「治理批恒 ff-only」全句（含 3 例旧 --no-ff 诚实括注、revert 回滚、--no-ff 仅 dev 通道） | 与 T7 钉死文本逐字一致（补批号全名 + 尾句移位，语义无变）；AGENTS.md 未动 ✓ |
| 8 | `docs/DEV-GOVERNANCE.md` | ✓ | ok | T1c+T4 义务句合并一段（条目级两态判据 + allLiveDeselected 显式报 + notExecuted 非零逐条点名义务 + 不当第二道门声明） | §1 末新增段与钉死句一致 ✓ |

## D1 判定：**可接受，不走冻结层追认链**

**一句话理由**：偏差异于 packet 外强制约束——round fact 顶层键白名单封闭（`verifier-evidence-check.mjs:281` allowed 集 13 键无 `gatesNotExecuted`，unknown field 即拒），而该文件本批 §3 明文承诺零触、不在 manifest，frozen 字面在 packet 边界内**物理不可满足**；等价载体信息零丢失（逐闸 `notExecuted`/`notExecutedReason` 经 `results[]` 进 round fact——validateFactObject 对 results 逐项不拒额外键，亲核确认持久化无损；汇总数组落 co-located `verdict.json` + child payload），**ok 判定路径零改动**，verifier 点名义务读的正是逐闸 `notExecuted`——目标效果不损 + 申报在册，判可接受。

支撑（亲跑）：V4 stub fixture（`CLAUDE_PROJECT_DIR` 重定向 temp 根 + 3 stub 闸，用后清理）复现 `{stub-degraded:true, stub-skipped:true, stub-real:false}` + `gatesNotExecuted:[stub-degraded,stub-skipped]` + `gatesRun:1`，该轮 `ok:true` 不变——与 D1 申报形态逐字吻合。

## 验证集复核（V1–V6 亲跑复验，V7 在册）

- **V1 ✅**：三闸 `--self-check` exit 0；自检名实测 = `mixed-file-structural-runs` / `all-live-deselected-reported` / `toolchain-dual-probe` / `degraded-fields-present`，与 frozen 名单逐字一致。
- **V2 ✅**：acceptance 闸实测 `phase:"executor-run", structuralCollected:127, liveDeselected:41, filesWithLiveMark:"9/9"`（不再 executor-skipped）；pytest `8 failed, 119 passed, 41 deselected`，条目 id 全带 = D3 实况。
- **V3 ✅（D2 可接受）**：decide 非导出、仓内无直调通道，经自检电池覆盖两臂（`idf-missing-degraded-green` + `degraded-fields-present` + `idf-missing-required-red` 全绿）。
- **V4 ✅**：见 D1 节。
- **V5 ✅**：`fileHasLiveMark(join` = 2 命中均报告用途（豁免）；`live-only on-target` = 0；六模式 census 全为定义/报告/判据/成文面，零排除级残留。
- **V6 ✅**：INDEX 本批行 `frozen | no`（新判据负例转正机判证据，旧判据此处必误报 yes）；state 缓存 18 友目录 yes / 本批 no，backfill 重钉无错钉；protocol ff-only 新句在场、旧表述零残留。
- **V7 ✅（首证）**：本批 landing 即触发首轮真跑红 8 条（全带数字与条目 id，零「0 collected 却 ok:true」）；三个待 launch Pod 为 pilot，交日落复盘。

## 破坏性自检（两轮合并）

- **landing 全触面**：757ba61（11 文件 = manifest 8 + 簿记 2 + 证据件 1）∪ 489e3dc（2 证据件）——无 manifest 外 canonical，契约面 / verify-gates.json / verifier-evidence-check.mjs / pytest.ini / C 测试面 / AGENTS.md / agents def / scope-map / settings 全零触 ✓。
- **packet-binding**：summary 引 packet-id = 本批 review id ✓；landing 实际范围 ⊆ packet ✓。
- **summary 声称 ↔ diff**：8/8 双向对照 ✓；敏感面（G3 四脚本 / G1 DEV-GOVERNANCE / G2 protocol+SKILL）逐字核，新字段只产信息、零拦截判定扩张 ✓。
- **verifier footprint**：误跑 verify.mjs 污染的两个运行时文件已还原、gitignored log 已删；工作树除 .codebase-memory 机器自刷新（human 指示不追交）+ 本 verdict 外零残留 ✓。

## 残余观察（不阻塞，5 条维持并更新）

1. ~~summary 补块重跑结构门~~ —— **已关闭**（489e3dc 修复 + 第二轮 gate ok:true）。
2. **D1 留根**：fact 顶层 `gatesNotExecuted` 如需，连同 verifier-evidence-check.mjs 白名单扩展同批做（D1 已登记）。
3. **状态缓存时序**：`.changes-index-state.json` 本批 `landed:false` 待本 verdict 入册后由下一次 upsert/backfill 推导翻转（机制固有）。
4. **acceptance 闸现恒红 8 条**（D3/V7 已申报，预期向严；断言本体消解走 dev-loop/独立批）。
5. **格式契约可见性**（更新）：B2 双侧含块 + A9 packet-binding 引述 + B7 锚行行首形态三条格式契约曾只活在门脚本错误消息里——本批 PROPOSAL §8 教训在册已建议写入 `docs/templates/PROPOSAL.template.md` 与结构字段硬条款段，**建议 retro 闭环确认该建议被下批承接**。
{"verdict": "GREEN", "changeId": "260909-gov-review-gate-truthfulness", "note": "终态机器锚，补 ok:true（GREEN）标题行形态未被 deriveLanded 识别的解析缺口"}
