# Review Packet —— 261013-p1-tomorrow-row-sliver-fix

> integrator 制品轮产出（2026-10-13），补充轮收口更新（human 裁定不开新包、并入本包）。状态：**verifier scoped round 5 全绿**（八闸 PASS，bearing fact = `verifier-evidence/rounds/r5.json`）+ 补充轮三变异取证齐（taskpackage `mutationEvidence.perAcceptance` 全数 turnedRed=true）+ closeout 制品齐 + TR-A5/TR-A9 真机轮待 human 持板（本包停门项、不停等烧录），feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。rigor=lite 单包、sourceType=regression、riskTier=low。
>
> 轮次沿革：主轮 r1（红）→ r2（红）→ r3（全绿）→ **补充轮 r5（全绿，当前收口证据态）**；r4 为中间轮（补充轮早期 scoped 轮，证据留档 `rounds/r4.json`）。

## 来源链

主轮：human 真机观测（今天区空 AC5 话术时首页 P1「明天」区第 2 个日程项被剃成 1px 残线）→ 本会话 triage：architect `/triage-change` 分流 review 产出 plan.md（2026-10-13，1g 前置技术预评本体，frontmatter reviewed-by 行为证）+ human triage 触点①（档位确认）已裁 → 本包 Lite 单包（自建 intake 形状：无 portfolio taskpackage、无 devloop/pods 条目、无 PROPOSAL，launch-card.md 缺失如实记录不设门）。

补充轮：261013 残线修复真机 PASS 后，human 持板目击行间距偏大 → **human 直令补充（原话「作为上一个包的补充，别再出一个包了，太小了」，非 proposal 来源、sourceType 保留 regression、origin 不变）** → architect `/triage-change` 二次 review 修订版搬运落盘为 plan.md「修订 A」（快审：② Lite / riskTier low / sourceType human-ad-hoc）+ human 真机预览定稿（`line_h=22` 参数族合意、锚点保留现状 `max(2, s_count)` 紧凑上浮，留痕 sibling `preview-round.md`）+ human 载体改判（changeManifest 增量加 `p1_cjk_name.acceptance.c`）。

## 根因与修法（主轮一段）

根因 = home_p1.c `p1_draw_card` L1173 明天区行流独立 clip 底界 `pv_cy1=218` 硬编码（约束来源是已退役的旧 PREVIEW_NEG 负断言带，非当前布局必需），叠加 `p1_blit_flow` 逐像素行 clip（`py > cy1` 跳过）把部分越界行只画顶部 1px 墨线；今天区第 7 项同病。修法 = ① pv_cy1 218→242 对齐卡区底界（TR-A1）+ ② 明天区 preview 行循环与今天区后续事项行循环各加「行带底越界即整行让位」break 守卫（TR-A2）+ ③ 冻结件钉区重锚（TR-A3）+ ④ 回归齿防复发（TR-A4）。

## 补充轮修法（一段）

行距参数族压缩 + 组级容纳门（plan 修订 A 改动点 A1）：`line_h 28→22` / `text_yoff 6→4` / 组间虚线 `ty+22→ty+8` / 明天帽标 `ty+33→ty+13`（46x24 资产原尺寸直出，兜底描边 rect 同步 `151,ty+13,197,ty+37`）/ 明天项行与 AC6/AC7 话术起点 `ty+60→ty+43`（TR-A6）；锚点 `rows=max(2,s_count)` 保持零改（human 裁定紧凑上浮，版式随数据浮动为选定语义）；新增**组级容纳门**——明天区整块包在 `if (ty + 43 + line_h - 1 <= pv_cy1)` 内（ty ≤ 178），今天 ≥6 项时整区让位、不画半截帽标/孤儿虚线，结构性关闭锚点下推越卡隐患（TR-A7）。整行容纳守卫（主轮落地）语义零改、随新 L 平移。两冻结件（HPB 7 常量 + 2 齿 / PCN 4 常量）随公式平移重锚（TR-A8）。

## Change Size

- 主轮实现面 = `home_p1.c`（pv_cy1 常量 + 函数头/行注释 + 两处守卫 break）+ `p1_home_battery.acceptance.c`（重锚常量与注释同步 + 回归齿 tooth 4/5 + REVISION LOG append）。
- **补充轮实现面 = changeManifest 三文件逐一对账**：`home_p1.c`（参数族压缩 + 组级容纳门 + 函数头注释几何重写）、`p1_home_battery.acceptance.c`（7 常量 + 2 齿重锚：tooth 4 改多行容量回归齿、tooth 5 夹具 7→8 项 + 新负断言带，头注/coverage map/REVISION LOG append）、`p1_cjk_name.acceptance.c`（**本文件首次入本包 changeManifest = human 载体改判**：4 常量重锚 + 头注 [S3] 几何重锚注记 + REVISION LOG append）。
- 净 diff stat（工作树态，补充分）：13 文件 +624/−210（含 delivery 簿记；固件三文件实际 = `home_p1.c` +/− 参数族、`p1_home_battery.acceptance.c` 重锚批量、`p1_cjk_name.acceptance.c` 重锚批量）。
- Scope: 纯板端显示渲染行为修复 + 版式参数压缩——零 wire 形状变更（`crossEnd.exempt` 在案）、零云端面、零契约面、零 docs 面。
- Split recommendation: 不拆——human 明确裁定并入本包（「太小了」），单根因单主题原子变。

## Verification Evidence

- **收口证据态 = r5（bearing fact）**：`verifier-evidence/rounds/r5.json`，phase=scoped、round=5、invocationId=`4d95d1af-f0b4-4233-b9c1-66dae00af339`、ok=true、observedGateExit=0——八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（设备侧真编译，xiaozhi.bin 0x312120 bytes、app 分区余量 0x5dee0 = 11%）/ contract-lint / acceptance-coverage（functional = TR-A1~A4 + TR-A6~A8；structural 豁免 = TR-A5 + TR-A9；files = p1_home_battery + p1_cjk_name）/ mutation-check / docs-sync（docsSyncRequired=false）/ security。verify fact 程序 fs-write，integrator 只消费不重跑。
- 红基线（acceptance-author 补充轮重锚批实跑 2026-10-13，命令 = taskpackage `redBaselineCommand` 逐字，重锚批落盘后、builder 落 A1 前）：**165 Tests / 4 Failures**——4 红全为推导表内设计态红、推导表之外变红：无（TEST 4.5 tooth 4 多行容量回归齿墨 673/246/84 须 ≥110 / PCN TEST 1 tooth 3 name 墨差 16/79 落 ROW2 带外 / TEST 5 A3 preview 行带墨 0 < 8 / TEST 8 tooth 4 time 墨差 46/46 落预告项行带外）。阈值 `HPB_INK_MIN_PV_ROW2` 族按 R<T<F 纪律重标定（R=84 / F≈157 / T=110，原 T=80 在旧几何下假绿）；重标定方法记两 REVISION LOG。
- **落地后 host 终验（integrator 补充轮独立复跑 2026-10-13，命令同上逐字；build 通道 ninja no-work-to-do = elf 与当前源面同 fresh）：165 Tests / 0 Failures / 0 Ignored, OK**——4 设计态红全数转绿，与 r5 终态一致；回归观察面 p1_cjk_name（重锚后）、canary schedule_grid 全绿、相对基线零新增红。
- **补充轮三变异取证**（taskpackage `mutationEvidence.perAcceptance` 补充轮三条，实测 2026-10-13，全数 `turnedRed=true && sensitive=true`）：
  1. **TR-A6** 参数族整族回退（line_h 22→28 + text_yoff 4→6 + 虚线 ty+22 + 帽标 ty+33 + 项行 ty+60）→ TEST 4.5 tooth 4 红（165/4，目标齿 verbatim「TR-A8 RED: 明天区多行容量回归齿——第 1/2/3 行带墨 673/246/84 分别须 >= 110」，673/246/84 与标定 R=84/T=110 逐字吻合）；还原复绿 165/0；grep MUTATION 零残留。
  2. **TR-A7** 摘组级容纳门（if 条件 → if(1)）→ TEST 4.5 tooth 6 孤立红（165/1，verbatim「TR-A7 RED: 今天 6 项时 preview 3 项帧 vs 0 项帧在明天区宽域带 diff 799 != 0——组级容纳门缺失…ty=200 > 178 应整区让位」）；还原复绿 165/0。
  3. **TR-A8** 重锚判别力同源承载（参数族回退变体同时平移今天/明天锚点 → HPB tooth 4 红 + PCN TEST 1 tooth 3 / TEST 5 A3 / TEST 8 tooth 4 三齿附带红——重锚常量之意义即令几何错位可测）；冻结件常量回退变异越 builder 写权、如实 deferred（builder 申报）；TR-A2 tooth 5 补充轮新带变异另证（摘今天区守卫 → 165/1 红「TR-A2 RED: 今天区卡底残线带 {152,222,392,242} 墨 128 != 0」）。
  （主轮变异三条 TR-A1/TR-A4 共源 + TR-A2 + TR-A3 见轮沿革，历史留痕在 taskpackage mutationEvidence。）
- **影响面核对记账（Step 9b advisory，detect_changes 补充轮重跑 2026-10-13）**：Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base=2a58ca2，inbound depth=2，含工作树未 commit diff——**72 seed / 4 impacted，全落 `firmware/main` 单模块、零跨模块越界**：`kidpal_home_p1_render_frame`（p1_draw_card 唯一调用方）、`happy_flow_capture.test.c`（归档快照测试）、`home_p1_screenshot.c`（截图工具）、`p1_cjk_name.acceptance.c`（补充轮新纳载体），全部为既有消费方被影响、非被改动；host 165/0 实跑实证零回归。无降级（per-Pod 图索引用常，三分法正常圈定）。test-scope 清单落盘 = `delivery/taskpackages/261013-p1-tomorrow-row-sliver-fix/test-scope-final.txt`（圈定 = 影响面圈定清单：两冻结验收件 ∪ blast radius 命中面 ∪ impactedTestsSeed 种子 ∪ 单 ELF 套件级全量先例口径；floor = 本包两冻结件，单包无对端冻结件）。
- **diff 面核对（补充轮）**：`git status` 实证改动面 == **补充轮 changeManifest 三文件**（`home_p1.c` / `p1_home_battery.acceptance.c` / `p1_cjk_name.acceptance.c`）+ delivery/ 簿记（本包 taskpackage 目录 + current/ 四投影 + changes/dev/ 本 closeout 目录）；**零触碰面**（`home_p1.h` / `schedule_grid.acceptance.c` / `host_app/` / schedule 数据链路 / `docs/**` / `docs/contracts/**` / `server/**` / `.claude/**`）`git status` 零命中——exitCriteria「PH-E1 canary 绿」与「schedule_grid 零改动 canary 绿」机械满足。`grep MUTATION` 三源零残留；`TEMP-CALIB` 仅存于头注阈值标定方法记述（历史文档，非活跃探针）。

## 未覆盖面申报（对照 acceptance[] 逐条）

- **TR-A1 / TR-A2 / TR-A3 / TR-A4 / TR-A6 / TR-A7 / TR-A8：已覆盖**（host 像素齿承载 + 变异取证齐 + r5 八闸全绿），零缺口。
- **TR-A5（主轮真机轮 live-manual structural 豁免，TB-A4 261012 同款先例口径）+ TR-A9（补充轮真机轮，同款口径）：均不计入 runner 通过面——状态 = 待 human 持板，本包收尾停门项**。
  - TR-A5 四项（主轮）：①复现场景（今天空 AC5 + 明天 ≥2 项）第 2 项完整字形无 1px 残线（截图对比）；②明天 ≥3 项第 3 项起整行让位；③今天 ≥7 项卡底 y=242 无残线、放不下项整行让位；④烧录 = app 分区单烧。**真机轮留痕（2026-10-13 human 持板，主线程 append）**：①app 分区单烧 0x20000 完成、hash verified；②30s 追端口捕获启动干净（`render home render_rc=0 state_rc=0`、uptime 平滑、0 RE-ENUM，c:/tmp/boot-check-261013.log）；③场景 1 **PASS**——今天空 AC5 话术 + 明天 2 项两项完整字形渲染、无残线，修复前后照片对留证（`verifier-evidence/device-round/{before-fix-sliver.jpg, TR-A5-scenario1-pass.jpg}`）；④场景 2/3 数据依赖（明日仅 2 项、今日无 7 项夹具）——host 侧回归代理 tooth 4/5 已绿 + 变异取证 turnedRed 实证判别力，真机目击 deferred 至相应数据自然出现（如实申报，不虚报全过）。
  - **TR-A9 四项（补充轮，deferred 到 human 持板，本包收尾停门项）**：①**容量达成**（今天空/≤2 + 明天 ≥3 → 明天 3-4 行全显、第 4 行带底贴卡底 242；今天 3 + 明天 3 → 3+3 全显）；②**紧凑上浮语义**（今天空 vs 今天 3 项两档虚线/帽标位不同为预期行为，120/125 vs 142/147）；③**行距 22px 全量数据态观感复核**（已预览轮拍板合意）；④**布局优先级在世**（今天 4 项→明天 2 行 / 5 项→1 行 / ≥6 项→明天整区让位无半截帽标）。烧录 = 纯代码变更仅 app 分区。**本包不停等烧录**——host 面转绿后停该门等 human，真机四项归后续真机轮承接（demo 对账归真机轮；本包非 workstream 包，workstream 增锚两条不适用）。
- exitCriteria 其余各条：①设计态红转绿已满足；②p1_cjk_name 重锚后全绿已满足（补充轮口径）；③schedule_grid canary 绿已满足；④零新增红已满足；⑤两冻结件 REVISION LOG append 新条目在场（授权链 = TaskPackage 261013 补充轮）、含逐齿推导表与 mutation 面迁移记载；⑥docs-sync 显式裁定 = docsSyncRequired=false（见 docs-delta.md）；⑦TR-A5 真机轮留痕 + TR-A9 deferred = 停门项如实申报（上文）。

## 已知地雷申报

- **supervisor 机械闸 all-mock 判定风险（先例申报口径，261012 / 261010 两包同款）**：本包 host 齿为真实渲染帧像素墨量断言（hpb_count_ink 探真，非纯结构断言），all-mock 风险低于两先例包，仍如实申报——若机械闸判 all-mock 触 riskTier 重算 high，需 human 放行（taskpackage notes 前置在案）。
- 真机轮烧录口径 = app 分区单烧（零 spiffs 资产变更）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/home_p1.c / firmware/main/test/p1_home_battery.acceptance.c / firmware/main/test/p1_cjk_name.acceptance.c | 本包改动面 `git diff` grep `minimal:` 零命中——零新增债务；改动文件内既有标记（如有）原样在世、不属本批 | n/a | n/a |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 冻结件守卫判别域 | 守卫变异判别单侧承载——主轮由今天区承载（明天区守卫在 pv_cy1=242 下摘除后第 3 行整行越出 clip 零墨、像素不可观测）；补充轮 L=22 下 261013 残线场景几何不可达（两区文字带结构上不跨 242），守卫降级为 belt，判别面转由参数族回退齿（TR-A6/TR-A8）承载——Lite 比例裁量既定口径（冻结件 mutation 断言面申报在案），非缺陷 | changed-code hunk（home_p1.c 明天区循环守卫 + 组级容纳门 + 两冻结件 REVISION LOG append 论证段）+ 判别力保全 heuristic | 无（申报在案即可；扩容包立项时顺带复核） |
| NIT | judgement-call | 组级容纳门极端档 | 今天 ≥6 项时明天区整体消失（含 AC6/AC7 话术）——极端档降级路径，无既有齿覆盖（净 HEAD 基线即绿 tooth 6 只验让位不验语义），真机判定 TR-A9 ④ 目测确认承载 | changed-code hunk（home_p1.c 组级容纳门 if 条件）+ plan 修订 A A3 回归风险 3 诚实申报 | 无（TR-A9 ④ 目测确认承载） |
| NIT | judgement-call | r4/r5 轮制品 | 补充轮 verifier 轨迹 r4（补充轮早期 scoped 轮）→ r5（全绿）在预算内收敛；主轮 r1/r2 两次返工亦收敛于预算内（iterationBudget=2 / fixBudget=3）——红因与 fixer 轮细节由轮 fact 承载，本 packet 只记轮次结局，Lite 包不展开逐轮 narrative | r1/r2/r3/r5 fact 顶层 ok 字段 + 诚实留痕 heuristic | 无 |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：无任何 hard 级 finding；契约/spec 类 finding 的起引与台账核销约定同 pre-merge-escalation-gate 的字面集定义（B-4 写作侧对齐），本轮 none。

## Five-Axis Review

- 安全：零新增攻击面——纯显示层 clip 边界 + 行流绘制行为 + 版式参数变更，零 wire / 零持久化 / 零新增依赖 / 零新增出网路径；守卫 break 与组级容纳门只减少绘制墨量、不改变任何输入处理面。
- 契约：零触（preview 载荷形状与 schedule 链路零变更，contract-lint PASS）；重锚后负断言语义判别力保持（变异实证），无影子实现绕 contracts 面。
- 边界与依赖卫生：零新增依赖；改动收敛单函数 `p1_draw_card` + 两冻结件钉区，零跨层调用（detect_changes 4 impacted 零越界佐证）；pv_cy1 与 cy1 合一消「双底界漂移」敞口——旧 218 硬编码即双源漂移实证，本次归一为单一 containment 依据；组级容纳门结构性关闭锚点下推越卡隐患（旧参数 s_count≥7 越卡绘制）。
- 可运维性：回滚 = 本 commit revert（单包原子，零跨面耦合）；REVISION LOG 授权链留痕使钉区重锚可追溯；真机复验路径清晰（TR-A5/TR-A9 清单化，烧录 = app 分区单烧）。
- 资源与功耗：xiaozhi.bin 0x312120 bytes、app 分区余量 11%（r5 firmware-build 实测）；两处整行比较 break + 组级门 O(1) 分支，零可测运行时开销变化。

## Residual Risk

- **plan 修订 A「A3. 回归风险 / 非目标」5 项**：①冻结件重锚批量大（HPB 7 常量 + 2 齿、PCN 4 常量）——逐齿推导表随 plan 修订 A 携带、acceptance-author/verifier 按表复核（已按表落盘并实证）；②band 守卫容量边角（今天第 8 项行带越卡底 1px 被整行让位，文字本可放下 226-239）——261013 冻结语义保留的有意取舍，登记非目标；③组级容纳门新语义（今天 ≥6 项时明天区整体消失含 AC6/AC7 话术）——极端档降级路径、无既有齿覆盖，真机判定 TR-A9 ④ 承载目测确认；④版式随数据浮动回归（紧凑上浮）——human 选定语义、非缺陷，验收勿误报；⑤换行续行跨底残线残余（主轮已登记）在 L=22 下仅理论可达，两区文字带结构上不跨 242、风险进一步降低。
- Known limitation: TR-A5（主轮）+ TR-A9（补充轮）真机轮待 human 持板（host 面闭环、真机探真未闭环——host 全绿不构成真机目击，tooth 4/5/6 只是 host 帧回归代理）；主轮 plan 三项非目标（明天区锚点下推越卡底 / 换行续行跨底界残线 / 明天区容量与设计线框差距）为已知残余，建议登记 TECH CRQ 但不绑本包义务、留 human/后续布局包裁量。
- Follow-up recommended: TECH CRQ 三项非目标登记（归后续布局包或 human 裁量）。
- Human decision needed: TR-A5 + TR-A9 真机轮持板（human，本包停门项）；all-mock 申报下如被 supervisor 机械闸重算 riskTier=high 则升 human 放行（同上地雷申报）。taskpackage 未启用 decisionCoverage 字段——`landsIn: dropped && reason: risk-accepted` 条目 = 零，Residual Risk 节按模板申报零条目。

## 集成执行回报

本轮无 merge-driver 冲突消解（补充轮为单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包两冻结验收件 `p1_home_battery.acceptance.c` + `p1_cjk_name.acceptance.c`（host 165/0 全量单 ELF 实跑承载，含 schedule_grid / key_layer_dispatch 等全部同 ELF 套件）。detect_changes 记账见 Verification Evidence（4 impacted 零越界，无降级）。全量回归 = 人工触发位，本轮为 Lite 包按派单口径收口。
