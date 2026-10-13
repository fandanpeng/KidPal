# Review Packet —— 261012-p3-topbar-ink-bleach-fix

> integrator 制品轮产出（2026-10-13）。状态：verifier scoped round 2 全绿（八闸 PASS，本包证据态 = 派单在案）+ docs-sync 收口 + closeout 制品齐，feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。rigor=lite 单包、sourceType=regression。

## 来源链

- 261010-p3-storylist-cold-start-restore SL-A5 真机轮 human 观测（「P3 现在是记录本列表了，但是右上角没有计数」）→ TECH-261012-p3-star-count-invisible-on-device（根因权威：2026-10-12 四轮 TEMP-DIAG 真机实测钉死 chrome 顶带 `top_bar_` 50% 白蒙层 × 1bpp flush 阈值 `*buffer < 0x7fff` × LVGL 9.3 `lv_color_16_16_mix` 漂白 KSY_COLOR_INK 0x333333；定案试验 human 真机观测「可以看到星星 *2 了」）→ architect `/triage-change` plan.md（2026-10-12，1g 前置技术预评本体）→ 本包 Lite 单包。

## Change Size

- 实现面 = changeManifest 五文件逐一对账：firmware 4 实现（`custom_lcd_display.cc` +9 / `story_page.c` +2−1 / `story_page.h` +9 / `kidpal_render_node.cc` +4−3）+ 1 验收（`story_page_v2.acceptance.c` +276，新增 SLP-TB1/2/3 三齿 = TEST 18/19/20）；净 +302 / −3。文档与 delivery 制品面：TECH-261012 条目收口（tracked 化 + 归档归位）+ 两 INDEX 重渲染 + 本目录两制品 + 包目录制品。
- Scope: 纯板端显示行为修复（候选 A 结构性转正）——top_bar_ 背景透明化断根 + TEMP-DIAG 残留归零 + AC5 两入口颜色单源归一；零 wire 形状变更、零云端面、零契约面。
- Split recommendation: 不拆——单根因单修法单包原子变（Lite 分包即回炉）。

## Verification Evidence

- 红基线（acceptance-author 冻结实跑 2026-10-13，命令 = taskpackage `redBaselineCommand` 逐字）：build 通道 error 零；实跑 **165 Tests / 2 Failures**——2 红全为设计态红（SLP-TB1 tooth 2「SetTheme 区无 lv_obj_set_style_bg_opa(top_bar_, LV_OPA_TRANSP, 0)」+ SLP-TB3 tooth 1「story_page.h 无 #define KSY_COLOR_INK」）；SLP-TB2 负向齿守卫 PASS；既有齿（SPV2 全套 + SLP-SLA2/3/4/5）零新增红。
- 落地后 host 全量：**165 Tests / 0 Failures / 0 Ignored**（builder 实跑回执，mutation 三轮各还原复绿均 165/0 在案）——2 设计态红全数转绿、既有齿零损失。
- 三齿承载：SLP-TB1（TB-A1 top_bar_ bg_opa 常量存在性 + 位置序）/ SLP-TB2（TB-A2 三源文件 TEMP-DIAG 负断言，:2439）/ SLP-TB3（TB-A3 INK 单源消费不变式 + lv_color_black 复现探针）。
- 变异取证（taskpackage `mutationEvidence.perAcceptance` 三条，builder 实测 2026-10-13）：M-TB1（LV_OPA_TRANSP→LV_OPA_50）→ SLP-TB1 tooth 2 红、还原复绿；M-TB3×2（AC5 分支改回 lv_color_black() → tooth 2 红；story_page.h 摘 #define → tooth 1 红）各自还原复绿；M-TB2（story_page.c 插入 TEMP-DIAG 注释，运行时读盘齿不重编直跑）→ SLP-TB2 tooth 2 红（165/1 非塌陷式）、字节备份还原（md5 前后一致）复绿。全数 `turnedRed=true && sensitive=true`，零 MUTATION 残留。
- verifier 轨迹：r1（invocationId f345ec65…，mutation-check 红——SLP-TB2 变异敏感取证缺位）→ **r2 全绿**（bearing fact：`delivery/taskpackages/261012-p3-topbar-ink-bleach-fix/verifier-evidence/rounds/r2.json`，phase=scoped、round=2、invocationId=54d6f5e8-1d72-4a97-9962-61e6cca6d3dd、ok=true、observedGateExit=0）——八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 0x312110 bytes、app 分区余量 0x5def0 = 11%）/ contract-lint / acceptance-coverage（TB-A1~A4 全落）/ mutation-check / docs-sync / security。本包 Lite 制品轮按派单口径以 r2 为证据态记录，integrator 未另跑 final 相位轮（verify fact 程序 fs-write，integrator 只消费不重跑；r2 顶层 `phase` 字段 = scoped 如实留痕，supervisor 裁断时可见）。
- 影响面核对记账（Step 9b advisory，detect_changes 实跑 2026-10-13：Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base=3e5de94，inbound depth=2）：87 seed symbols / **26 impacted 全落 `firmware/main` 单模块，零跨模块越界**——命中面为既有显示 seam 消费方（p1_cjk_name / p1_home_battery / schedule_grid / host_infra_smoke 四 acceptance 套件探针函数族、electron-bot / otto-robot emoji 显示板件、waveshare keya 分流、mcp_server story 工具族、application 状态机 hop2），全部为既有在役消费方被影响、非被改动；host 165/0 实跑实证零回归。无降级（per-Pod 图索引用常）。

## 未覆盖面申报（对照 acceptance[] + exitCriteria 逐条）

- **TB-A1 / TB-A2 / TB-A3：已覆盖**（host 三齿承载 + 变异取证齐 + r2 闸全绿），零缺口。
- **TB-A4（真机轮 live-manual structural 豁免）——状态 = pending-human，尚未执行（非缺口豁免、非豁免通过）**，四项逐项（human 在场持板执行，261010 包 SL-A5 同款先例口径）：
  1. SL-A5 计数分项复验——翻 P3 右上角 ★/×N 可见（TECH 条目根因修复的直接真机目击项）；
  2. AC5 无内容空态 ×0 两入口可见——story_page.c 空态分支 + kidpal_render_node.cc AC5 分支各验一次（TECH 条目正文更正③的「一个入口显示一个不显示」同类缺陷面随本修归一后目测）；
  3. 阅读态标题顺带目测（story_page.c:1216，y=8 / 0x222222——TECH 条目连带预测：修复 A 后自愈）；
  4. P1 顶带与聊天态顶带无回归目测（top_bar_ 透明化作用于全部页顶带，跨页副作用确认；wifi/mute 图标为 bar 子件、不透明字形直画不受影响的口径核验）。
- exitCriteria 其余各条：①（设计态红转绿 + 既有齿零新增红）已满足；②（temp-diag-snapshot.diff 比对留痕）由 TB-A2 负断言绿机械承载 + 快照件在包目录；③（变异取证 perAcceptance）已满足；⑤（docs-sync）本批完成——TECH 条目收口 + INDEX 重渲染（见 docs-delta.md）。
- host 结构断言无 chrome / 无像素探真（TECH 条目「host 覆盖洞补注」口径）：TB-A1~A3 为源面常量/负断言，真机探真唯一归 TB-A4——上述 pending-human 项即该覆盖洞的承接口，无替位证据（host 全绿不构成真机目击）。

## 已知地雷申报

- **supervisor 机械闸 all-mock 判定风险**：TB-A1~A3 全为 host 结构断言（常量存在性 / 负断言 / 颜色不变式），无 DB / 事件断言——supervisor 机械闸可能判 all-mock 触 riskTier 重算 high，需 human 放行（**261012 prompt-v9 / 261010 storylist 两包同款先例申报**，taskpackage notes 前置在案）。
- host harness 无 chrome（top_bar_ 不在 host 面）——TB-A1 结构齿为源面常量断言非像素探真，真机探真归 TB-A4（见未覆盖面申报）。
- 真机轮烧录口径 = app 分区单烧（本批零 spiffs 资产变更，r2 firmware-build art.bin 仅 spiffsgen 常规重打包，flash-asset-partition-rule 口径核对通过）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/story_page.c:986/:1248/:1330/:1374 | 既有 `minimal:` 标记四处（非本包新增——`git diff` grep `minimal:` 零新增命中；本包改动文件内既有标记原样在世） | 各标记自带 ceiling 注记 | 沿用各标记既有 upgrade trigger，本批零新增债务 |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | taskpackage-required | sourceRefs 含锚点形态 `docs/requirements.md:S-F2.1`，闸报 warning「path-not-on-disk」（锚点形态非文件路径，warning 级不红）——锚点形态引用约定与闸解析能力的历史错配（261010 包同款，r1–r2 恒现） | gate stdout hunk（r2 taskpackage-required warnings）+ 留痕诚实性 heuristic | 无（warning 不阻塞；如后续静默化可在 anchor 解析层收口，非本批义务） |
| NIT | judgement-call | TECH 条目 trigger 字段 | 条目 frontmatter `trigger` 与 `title` 内「260912 第四轮 TEMP-DIAG」为笔误留存（事件实为 2026-10-12，正文与追加段均记 2026-10-12）——收口段以 2026-10-13 closed 日期为权威，讹误不承载机读状态 | changed-artifact hunk（frontmatter trigger 行 vs 正文追加段日期）+ 一致性 heuristic | 无（append-only 纪律下不回改既有行；归档件正文已自明，记录在案即可） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。无 hard-violation finding——如出现将以 `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` 字面前缀起引 Basis；本轮 none。

## Five-Axis Review

- 安全：零新增攻击面——纯显示层样式与绘制色值变更，零 wire / 零持久化 / 零新增依赖 / 零新增出网路径；top_bar_ 透明化不改变任何输入处理面。
- 契约：零触（story_list 载荷与 counter 语义零变更，contract-lint PASS）；SLP-TB2 负断言确认诊断残留归零，无影子实现绕 contracts 面。
- 边界与依赖卫生：零新增依赖；KSY_COLOR_INK 上提 story_page.h 单源消双写（C/C++ 混编单定义防重定义，story_page.c :17 include 关系核实）；top_bar_ 透明化 = 单一 choke point 解耦（免除「所有落 y[0..28] 的 INK/TEXT 内容逐个跟改」敞口），与候选 B 治标路线的取舍理由（0x7BEF 3% 余量脆弱性）在 TECH 条目在案。
- 可运维性：回滚 = 本 commit revert（单包原子，零跨面耦合）；诊断代码零残留（grep 全树 TEMP-DIAG 仅命中冻结件自带文本）；真机复验路径清晰（TB-A4 四项清单化，烧录 = app 分区单烧）。
- 资源与功耗：xiaozhi.bin 0x312110 bytes、app 分区余量 11%（r2 firmware-build 实测）；零运行时开销变化（一条样式 setter + 色值常数归一）。

## Residual Risk

- Known limitation: TB-A4 真机轮四项 pending-human（修复效果的真机目击尚未取得——TECH 条目定案试验一行已由 human 真机观测证实候选 A 有效，但完整四项复验待执行）；AC5 空态两入口真机可见性归该轮。
- Follow-up recommended: 无新增（261010 包遗留改进项不受本批影响）。
- Human decision needed: TB-A4 真机轮持板（human）；all-mock 申报下 riskTier=medium 如被 supervisor 机械闸重算 high 则升 human 放行（同上地雷申报）。taskpackage 未启用 decisionCoverage 字段——`landsIn: dropped && reason: risk-accepted` 条目 = 零，Residual Risk 节按模板申报零条目。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包冻结验收件 `story_page_v2.acceptance.c`（host 165/0 全量实跑承载，含 SPV2 / SLP-SLA 既有套件）。detect_changes 记账见 Verification Evidence 末行（26 impacted 零越界，无降级）。全量回归 = 人工触发位，本轮为 Lite 包按派单口径收口。