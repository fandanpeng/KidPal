# Review Packet —— 261013-p1-tomorrow-row-sliver-fix

> integrator 制品轮产出（2026-10-13）。状态：verifier scoped round 3 全绿（八闸 PASS，bearing fact 在案）+ mutation 取证三条齐 + closeout 制品齐 + TR-A5 真机轮待 human 持板（本包停门项、不停等烧录），feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。rigor=lite 单包、sourceType=regression、riskTier=low。

## 来源链

human 真机观测（今天区空 AC5 话术时首页 P1「明天」区第 2 个日程项被剃成 1px 残线）→ 本会话 triage：architect `/triage-change` 分流 review 产出 plan.md（2026-10-13，1g 前置技术预评本体，frontmatter reviewed-by 行为证）+ human triage 触点①（档位确认）已裁 → 本包 Lite 单包（自建 intake 形状：无 portfolio taskpackage、无 devloop/pods 条目、无 PROPOSAL，launch-card.md 缺失如实记录不设门）。

## 根因与修法（一段）

根因 = home_p1.c `p1_draw_card` L1173 明天区行流独立 clip 底界 `pv_cy1=218` 硬编码（约束来源是已退役的旧 PREVIEW_NEG 负断言带，非当前布局必需），叠加 `p1_blit_flow` 逐像素行 clip（`py > cy1` 跳过）把部分越界行只画顶部 1px 墨线；今天区第 7 项同病。修法 = ① pv_cy1 218→242 对齐卡区底界（TR-A1）+ ② 明天区 preview 行循环与今天区后续事项行循环各加「行带底越界即整行让位」break 守卫（TR-A2）+ ③ 冻结件钉区重锚（PREVIEW_NEG {152,220,392,258}→{152,244,392,258}、tomorrow {136,142,392,218}→{136,142,392,242}，授权链 = TaskPackage 261013-p1-tomorrow-row-sliver-fix（human triage 触点① 确认），REVISION LOG 2026-10-13 条 append 留痕，TR-A3）+ ④ 回归齿防复发（TR-A4 采纳：TEST 4.5 tooth 4 墨量阈值断言 T=80，标定 R=15/F=157）+ tooth 5 今天区卡底残线带严格零墨（TR-A2 守卫负断言）。

## Change Size

- 实现面 = changeManifest 两文件逐一对账：`home_p1.c`（+17−内：pv_cy1 常量 + 函数头/行注释 + 两处守卫 break）+ `p1_home_battery.acceptance.c`（+213−内：重锚常量与注释同步 + 回归齿 tooth 4/5 + REVISION LOG append + 头注 coverage map/BUILDER DUTY/[TR5] 豁免段）。净 diff stat：7 文件 +469/−195（含 delivery 簿记；固件实现面实际约 +25 行级改动，其余为验收件 additive）。
- Scope: 纯板端显示渲染行为修复——零 wire 形状变更（preview 数据形状与 schedule 链路零触碰，`crossEnd.exempt` 在案）、零云端面、零契约面、零 docs 面。
- Split recommendation: 不拆——单根因单修法单包原子变（Lite 分包即回炉）。

## Verification Evidence

- 红基线（acceptance-author 冻结实跑 2026-10-13，命令 = taskpackage `redBaselineCommand` 逐字）：build 通道 error 零；实跑 **165 Tests / 1 Failures**——唯一设计态红 = TEST 4.5 tooth 4「TR-A4 RED: 明天区第 2 行带 {152,212,392,239} 墨 15 < 80」（阈值 HPB_INK_MIN_PV_ROW2=80 由 TEMP-CALIB 探针一次测量标定：净 HEAD 残线墨 R=15 / 整行墨 F=157，定稿前探针已移除）；tooth 5 为同 CASE Unity abort 屏蔽的解析性红；重锚齿（TEST 1 tooth 4 / TEST 4.5 tooth 1-3）净 HEAD 实证绿无齿漂移。
- 落地后 host 终验（integrator 独立复跑 2026-10-13，命令同上逐字；build 通道 ninja no-work-to-do = elf 与当前源面同 fresh）：**165 Tests / 0 Failures / 0 Ignored, OK**——设计态红全数转绿，与 r3 终态一致。
- verifier 轨迹：r1（scoped，红——acceptance-coverage 闸）→ r2（scoped，红——mutation-check 闸）→ **r3 全绿**（bearing fact：`delivery/taskpackages/261013-p1-tomorrow-row-sliver-fix/verifier-evidence/rounds/r3.json`，phase=scoped、round=3、invocationId=4fb758f7-9553-4ccc-adb4-240f01c53b94、ok=true、observedGateExit=0）——八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（设备侧真编译，xiaozhi.bin 0x312120 bytes、app 分区余量 0x5dee0 = 11%）/ contract-lint / acceptance-coverage（TR-A1~A5 全落，structural 豁免 = TR-A5）/ mutation-check / docs-sync（docsSyncRequired=false）/ security。本包 Lite 制品轮按派单口径以 r3 为证据态记录，integrator 未另跑 final 相位轮（verify fact 程序 fs-write，integrator 只消费不重跑；r3 顶层 `phase` 字段 = scoped 如实留痕，supervisor 裁断时可见）。
- 变异取证（taskpackage `mutationEvidence.perAcceptance` 四条，实测 2026-10-13，全数 `turnedRed=true && sensitive=true`）：
  1. TR-A4 / TR-A1 共源变异（home_p1.c 改回 pv_cy1=218）→ TEST 4.5 tooth 4 红（墨 0 < 80；此变异下守卫因 212+27=239>218 亦触发 break 故墨 0 非 15，红仍由 tooth 4 承载判别力成立）；还原复绿 165/0；grep MUTATION 零残留。
  2. TR-A2 变异（摘今天区整行容纳守卫 break）→ TEST 4.5 tooth 5 红（卡底残线带 {152,242,392,242} 墨 18 != 0）；还原复绿 165/0。明天区守卫在 pv_cy1=242 下摘除后第 3 行整行越出 clip 零墨、像素不可观测——守卫变异判别由今天区承载（acceptance-author 冻结时逐齿推演论证在案）。
  3. TR-A3 变异（冻结件钉区常量回滚 preview_neg {152,244,392,258}→{152,220,392,258}）→ TEST 4.5 tooth 3 红（重锚带墨 105 != 0）——重锚判别力实证闭环；acceptance-author 实测（唯一 *.acceptance.c 写点角色），还原后 sha256 逐字节一致零残留，红跑日志留档 c:/tmp/tr_a3_mutation_run.log。
- 影响面核对记账（Step 9b advisory，detect_changes 实跑 2026-10-13：Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base=0e457f0，inbound depth=2，含工作树未 commit diff）：3 seed / **3 impacted 全落 `firmware/main` 单模块、零跨模块越界**——`kidpal_home_p1_render_frame`（p1_draw_card 唯一调用方）、`happy_flow_capture.test.c`（归档快照测试）、`home_p1_screenshot.c`（截图工具），全部为既有消费方被影响、非被改动；host 165/0 实跑实证零回归。无降级（per-Pod 图索引用常，三分法正常圈定）。test-scope 清单落盘 = `delivery/taskpackages/261013-p1-tomorrow-row-sliver-fix/test-scope-final.txt`（圈定 = 影响面圈定清单：acceptanceTestsFile ∪ blast radius 命中面 ∪ impactedTestsSeed 种子 ∪ 单 ELF 套件级全量先例口径）。
- diff 面核对：`git status` 实证改动面 == changeManifest 两固件文件 + delivery/ 簿记（本包 taskpackage 目录 + current/ 四投影 + changes/dev/INDEX.md）+ 本 closeout 目录；零触碰面（p1_cjk_name.acceptance.c / home_p1.h / schedule 数据链路 / docs/** / docs/contracts/** / server/** / .claude/**）零命中——exitCriteria「p1_cjk_name 零文件改动全绿」与「PH-E1 canary 绿」机械满足。

## 未覆盖面申报（对照 acceptance[] 逐条）

- **TR-A1 / TR-A2 / TR-A3 / TR-A4：已覆盖**（host 像素齿承载 + 变异取证齐 + r3 八闸全绿），零缺口。
- **TR-A5（真机轮 live-manual structural 豁免，TB-A4 261012 同款先例口径，不计入 runner 通过面）——状态 = 待 human 持板，本包收尾停门项**，四项清单：
  1. 复现场景（今天空 AC5 话术 + 明天 ≥2 项）：明天区第 2 项**完整字形渲染**、无 1px 残线（截图与修复前对比留证）；
  2. 明天 ≥3 项：第 3 项起**整行让位**干净不出现（不出现半行/残线）；
  3. 今天 ≥7 项：卡底 y=242 无残线、放不下的项整行让位（host 侧回归代理 = tooth 5，已绿；真机字形/观感仍归 human 在场确认）；
  4. 烧录 = app 分区单烧（纯代码变更、无 spiffs 资产变更，flash-asset-partition-rule 口径核对通过；r3 firmware-build 的 art.bin 仅为 spiffsgen 常规重打包）。
  **本包不停等烧录**——host 面转绿后停该门等 human，真机轮四项归后续真机轮承接（demo 对账归真机轮；本包非 workstream 包，workstream 增锚两条不适用）。
- exitCriteria 其余各条：①设计态红转绿已满足；②p1_cjk_name 零改动全绿已满足；③schedule_grid canary 绿已满足；④零新增红已满足；⑤REVISION LOG 2026-10-13 条在场（append，授权链与判别力保全论证在冻结件尾）；⑥docs-sync 显式裁定 = docsSyncRequired=false（见 docs-delta.md）；⑦TR-A5 真机轮留痕 = 停门项如实申报（上文）。
- Lite 包 workstream 增锚两条（demo 对账行 + 未覆盖面申报）不适用（非 workstream 包）——demo 对账归真机轮；未覆盖面申报即本节。

## 已知地雷申报

- **supervisor 机械闸 all-mock 判定风险（先例申报口径，261012 / 261010 两包同款）**：本包 host 齿为真实渲染帧像素墨量断言（hpb_count_ink 探真，非纯结构断言），all-mock 风险低于两先例包，仍如实申报——若机械闸判 all-mock 触 riskTier 重算 high，需 human 放行（taskpackage notes 前置在案）。
- 真机轮烧录口径 = app 分区单烧（零 spiffs 资产变更）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/home_p1.c / firmware/main/test/p1_home_battery.acceptance.c | 本包改动面 `git diff` grep `minimal:` 零命中——零新增债务；改动文件内既有标记（如有）原样在世、不属本批 | n/a | n/a |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 冻结件回归齿判别域 | tooth 5 守卫负断言由今天区承载、明天区守卫摘除后像素不可观测（pv_cy1=242 下第 3 行整行越出 clip 零墨）——守卫变异判别单侧承载为 Lite 比例裁量既定口径（冻结件 mutation 断言面申报在案），非缺陷；如后续布局包扩明天区容量，守卫判别可双边补齐 | changed-code hunk（home_p1.c 明天区循环守卫 + 冻结件 REVISION LOG 2026-10-13 条②论证段）+ 判别力保全 heuristic | 无（申报在案即可；扩容包立项时顺带复核） |
| NIT | judgement-call | r1/r2 轮制品 | verifier 轨迹 r1（acceptance-coverage 红）/ r2（mutation-check 红）两次返工均收敛于预算内（iterationBudget=2 / fixBudget=3），红因与 fixer 轮细节由轮 fact 承载，本 packet 只记轮次结局——Lite 包不展开逐轮 narrative | r1/r2/r3 fact 顶层 ok 字段 + 诚实留痕 heuristic | 无 |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：无任何 hard 级 finding；契约/spec 类 finding 的起引与台账核销约定同 pre-merge-escalation-gate 的字面集定义（B-4 写作侧对齐），本轮 none。

## Five-Axis Review

- 安全：零新增攻击面——纯显示层 clip 边界与行流绘制行为变更，零 wire / 零持久化 / 零新增依赖 / 零新增出网路径；守卫 break 只减少绘制墨量、不改变任何输入处理面。
- 契约：零触（preview 载荷形状与 schedule 链路零变更，contract-lint PASS）；重锚后负断言语义「不越卡区底界」判别力保持（变异③实证），无影子实现绕 contracts 面。
- 边界与依赖卫生：零新增依赖；改动收敛单函数 `p1_draw_card` + 单冻结件钉区，零跨层调用（detect_changes 3 impacted 零越界佐证）；pv_cy1 与 cy1 合一消「双底界漂移」敞口——旧 218 硬编码即双源漂移实证，本次归一为单一 containment 依据。
- 可运维性：回滚 = 本 commit revert（单包原子，零跨面耦合）；REVISION LOG 授权链留痕使钉区重锚可追溯；真机复验路径清晰（TR-A5 四项清单化，烧录 = app 分区单烧）。
- 资源与功耗：xiaozhi.bin 0x312120 bytes、app 分区余量 11%（r3 firmware-build 实测）；两处整行比较 break 为 O(1) 分支，零可测运行时开销变化。

## Residual Risk

- Known limitation: TR-A5 真机轮待 human 持板（host 面闭环、真机探真未闭环——host 全绿不构成真机目击，tooth 4/5 只是 host 帧回归代理）；plan 三项非目标（明天区锚点下推越卡底 / 换行续行跨底界残线 / 明天区容量与设计线框差距）为已知残余，建议登记 TECH CRQ 但不绑本包义务、留 human/后续布局包裁量（plan 回归风险 3：续行跨底界残线现存夹具与已知真机场景均不触发）。
- Follow-up recommended: TECH CRQ 三项非目标登记（归后续布局包或 human 裁量）。
- Human decision needed: TR-A5 真机轮持板（human，本包停门项）；all-mock 申报下如被 supervisor 机械闸重算 riskTier=high 则升 human 放行（同上地雷申报）。taskpackage 未启用 decisionCoverage 字段——`landsIn: dropped && reason: risk-accepted` 条目 = 零，Residual Risk 节按模板申报零条目。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包冻结验收件 `p1_home_battery.acceptance.c`（host 165/0 全量实跑承载，含 p1_cjk_name / schedule_grid / key_layer_dispatch 等全部同 ELF 套件）。detect_changes 记账见 Verification Evidence 末行（3 impacted 零越界，无降级）。全量回归 = 人工触发位，本轮为 Lite 包按派单口径收口。
