# Review Packet · 260921-m3-k4-p1-cjk-dynamic-text

> workstream m3-schedule ｜ M3-K4 首页 P1 动态日程名 CJK 渲染（字形 seam 双强符号 + UTF-8 逐字形 blit，从「画不出即丢弃」到「画得出、超宽截断不越区」）｜ mergeOrder 36 ｜ rigor lite ｜ riskTier medium ｜ subsystem firmware ｜ origin.type human-confirmed-plan（挂账④立项，triage ②单包 Lite，human 确认不改判）
> integrator 收拢制品（2026-09-22）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。dependsOn []（K1/K2/K3 全 merged、mergeToken 无持有者，本批单包树池串行第 11 棒）。

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit）**：
  - `firmware/main/display/home_p1.c` +145——[S1] `kidpal_p1_font_glyph` 弱默认（恒返回非 0 = 不可用，out==NULL 防御）+ [S2] UTF-8 解码（`p1_utf8_seq_bytes` 首字节判长 + 非法续字节/不完整尾序列安全停笔）+ `p1_blit_name` 逐字形 blit（复用钉区 clip / FB 界双重裁剪 / 固定起笔纪律，字形盒上限 64、缺字形跳过不画垃圾）+ 三调用点（`p1_draw_next` / `p1_draw_nextlist` / `p1_draw_preview`，均不改兜底描边决策）。
  - `firmware/main/display/kidpal_render_node.cc` +85——[S1] 设备强符号 `extern "C" kidpal_p1_font_glyph`：`ksg_text_font()`（schedule_page.c 先例）同款双分支（`LV_FONT_SOURCE_HAN_SANS_SC_14_CJK` host 编入分支 / `ESP_PLATFORM` 取对话页同款 `font_puhui_14_1`），`lv_font_get_glyph_dsc` + A1/A2/A4/A8 alpha 阈值化成 1bpp 墨。
  - `firmware/main/display/kidpal_p1_font.h`（新文件）——[S1] 声明头（`kidpal_p1_glyph_t` {w,h,advance,bits} 与齿头注冻结布局逐字段一致）。
  - `firmware/main/test/host_app/main/host_infra_seam.c` +78——[S1] host 强符号（`lv_font_source_han_sans_sc_14_cjk` 子集路径，A1/A2/A4/A8 阈值化同构）。
  - `firmware/main/test/host_app/main/CMakeLists.txt` +7——`p1_cjk_name.acceptance.c` 注册进 HOST_ACCEPTANCE_SRCS（置于 host_infra_smoke 之前，smoke 末位 = 最先执行守卫不变）。
  - `firmware/main/test/p1_cjk_name.acceptance.c`（新文件，6 TEST_CASE 冻结齿）——A1 墨落三钉区 diff 定位 / A2 超宽截断右界带严格零墨 / A4 UTF-8 截半安全停笔 / A5 缺字形退化不崩 / A3 单项零墨守卫 + E2 铁律 canary / TEST 6 结构齿（seam 三符号落点 + font_puhui_14_1 装配事实半 + 注册接线守卫）；文件头「契约条目 ↔ 齿强度申报」+ [S1]–[S4] seam 契约冻结在案。
  - `firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c` + `pcn_red_wrapper.sh`（新文件，**非冻结脚手架**）——红基线独立编译通道（gcc 直编，seam stub 恒不可用，红因钉在字形 seam 缺失而非渲染 seam 缺失；不编入 host runner，与门 runner 分离）。
- **planner 元数据回填**：`delivery/taskpackages/260921-m3-k4-p1-cjk-dynamic-text/taskpackage.json`（+ current/ 指针同步）——contractRefs / acceptanceTestsFile / mutationEvidence.perAcceptance 六条（sensitive+mutation+expectedRed）/ redBaselineResult 回填；r1/r2 两轮机械闸红（contractRefs 缺失 / mutation-check 登记缺位）由此修复。
- **过程账**：`pod-events.jsonl`（6 事件）+ `verify-runs.jsonl` + `verifier-evidence/rounds/r1–r4.json`（verify.mjs 程序 fs-write 过程态）+ 双 input-brief（acceptance-author / builder 派单素材）。
- **closeout 制品**：本目录本件 + `docs-delta.md` + `verdict.json`（final r4 bearing fact 副本）+ `verification.json`（advisory 投影）。
- **Scope**：`firmware/main/display/**` + `firmware/main/test/**` + taskpackage 过程账；`server/**`、`docs/**`、`.claude/**` 零命中（`git diff -- docs/` 与 `git diff -- .claude/` 实跑零输出）。
- **Split recommendation**：不拆——seam（弱默认 + 双强符号）+ 解码 blit + 冻齿 + 注册接线是同一条「name 画得出」链的耦合四段，拆开任一段都无法独立验收。

## Verification Evidence

- **final round 4 九闸全绿（integrator，`8c6aa0de-09e7-42f4-a1ec-97b9ef760be1`，2026-09-22，`ok:true` / `observedGateExit:0`）**：evidenceRef `delivery/taskpackages/260921-m3-k4-p1-cjk-dynamic-text/verifier-evidence/rounds/r4.json`（immutable verifier fact，本 packet 只引用不复制）。round 链：scoped r1 红（taskpackage-required：contractRefs 缺失，planner 元数据修复）→ scoped r2 红（mutation-check：mutationEvidence 登记缺位，回填后修复）→ scoped r3 全绿（verifier，`9fcff52a-…`）→ **final r4 全绿（integrator）**。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260921-m3-k4-p1-cjk-dynamic-text --phase final --round 4 --invocation-id 8c6aa0de-…` integrator 实跑 **exit 0、`factOk:true`、`observedGateExit:0`**——final evidence 有效，可进 closeout。
- **integrator 独立复跑 host runner 全量（2026-09-22 实跑留证）**：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → ninja 判定 up-to-date（源与 ELF 同步）build rc=0；`./build/host_infra_test.elf` → **逐字证据行：`80 Tests 0 Failures 0 Ignored` / `OK` / RUN_RC=0**——既有 74 齿零偏移（must-stay-green 兑现）+ 本包新齿 6 TEST_CASE 全 PASS（逐行见 Demo 对账行）。
- **九闸逐闸**：taskpackage-required / docs-sync / security / contract-lint / mutation-check / acceptance-coverage 全绿；**firmware-build 真编译未降级**（xiaozhi.bin 0x30c5b0 bytes、app 分区 0x370000 bytes、余量 0x63a50 bytes (11%) free——A6 体积余量记录义务兑现，无新增大静态量：host/设备各一 64×9 字节静态行缓冲，帧缓冲 s_p1_fb 既有 PSRAM 面零偏移）；unit **95 passed**；acceptance **41 passed** 零新增红。
- **Impact audit（worktree 降级形态）**：detect_changes **N/A (worktree)**（Pod worktree diff 对主索引不可见）——替代抽查：① `git diff --stat` 闭集核对（见 Change Size，server/docs/.claude 零命中）；② 新符号 `kidpal_p1_font_glyph` / `kidpal_p1_glyph_t` 消费闭集 grep 实跑 = 恰 6 文件（home_p1.c 弱默认+调用点 / kidpal_p1_font.h 声明 / kidpal_render_node.cc 设备强符号 / host_infra_seam.c host 强符号 / 冻齿 / 红基线 stub），零其它消费方。
- **Docs sync**：`docsSyncRequired=false` 义务核清（判定与依据详见同目录 `docs-delta.md`）——spec/契约零改动。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。
- **Loop metrics / Pod Journal（post-cutoff dual-format）**：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`；`pod-events.jsonl` 在场（advisory 检查通过），缺失不阻塞 closeout。
- **机器证据投影**：`current/verdict.json`（verify.mjs 程序 fs-write，non-bearing advisory）已随 final r4 刷新；bearing fact = r4.json，本 closeout 目录晋升 `verdict.json`（fact 副本）+ `verification.json`（投影，只消费不重跑）。

## E2 铁律自查（P1CJK-E2，逐条证据）

| 铁律项 | 证据 | 结论 |
|---|---|---|
| `home_upcoming.{c,h}` 零改 | `git diff -- firmware/main/display/home_upcoming.c firmware/main/display/home_upcoming.h` 零输出（2026-09-22 integrator 实跑；两文件不在 diff stat）；TEST 5 E2 canary 双断言 PASS：home_upcoming.c 不得引用 `kidpal_p1_font_glyph`（P1 侧容错、不回写 K2 面）+ `kidpal_home_upcoming_apply` 冻结入口在世 | 零改 verified |
| `docs/contracts/**` 与 spec 零改 | `git diff -- docs/` 零输出；contract-lint 闸 r3/r4 绿；TEST 5 canary（`docs/contracts/interface-contract.md` 在世 + `docs/requirements.md` 含 `S-F1.1` 锚）PASS | 零改 verified |
| `schedule_page.c` 零触碰 | `firmware/main/display/schedule_page.c`（与 .h）不在 `git diff --stat` 闭集（diff 面 = home_p1.c / kidpal_render_node.cc / kidpal_p1_font.h 三文件 + host_app 测试面） | 零触碰 verified |
| `hp1_item_t` 与 11 个冻结契约函数零偏移 | `home_p1.h` 零 diff（不在 git status）；TEST 5 E2 canary PASS：`HP1_NAME_LEN==32 && HP1_TIME_LEN==6 && HP1_ITEM_MAX==8` + 11 函数（schedule_inject/count/item/preview、day_rollover、countdown、blink_visible、doze_state、layout_resolve、set_state、render_frame）声明面逐一在世；实现侧 home_p1.c diff 内零签名 hunk（仅新增 seam/blit/调用点 + p1_draw_preview 局部 has_pv 重构，契约函数语义零偏移——74 既有齿零偏移背书） | 零偏移 verified |
| 固定标签烘焙资产零改 | git status/diff 无 `firmware/main/assets/**`、art 资产文件命中；home_p1.c diff 无固定标签（接下来/明天/还有）烘焙段 hunk（diff 仅 seam + name blit 三调用点） | 零改 verified |

**结构闸结果**：final r4 九闸全绿（结构闸 PASS/FAIL 不复述为 finding）；author 五轴语义 review 段完整执行（见下）。零 BLOCKING、零 hard-violation。

## Demo 对账行（workstream 包硬性：progress.md 挂账④段效果核对）

**本包 demo 证据形态 = host runner pixel-probe 齿输出**（taskpackage `testHost`：「WSL host runner（pixel-probe 齿）+ device build 闸 + 真机目测（挂设备轮窗口）」——前两半本次兑现，第三半按 E3 口径挂账）。证据行：

- **全量 80 齿证据行（integrator 独立实跑逐字）**：`80 Tests 0 Failures 0 Ignored` / `OK` / `RUN_RC=0`（既有 74 齿零偏移）。
- **新齿 6 TEST_CASE PASS 逐行**（p1_cjk_name.acceptance.c）：
  1. `P1CJK name: 墨落三钉区 diff 定位 (P1CJK-A1):PASS`（:669）
  2. `P1CJK name: 超宽截断不越右界 (P1CJK-A2):PASS`（:733）
  3. `P1CJK name: UTF-8 截半尾序列安全停笔 (P1CJK-A4):PASS`（:802）
  4. `P1CJK name: 字形缺失退化不崩 (P1CJK-A5):PASS`（:862）
  5. `P1CJK guards: 单项零墨+铁律canary+扫描器自检 (P1CJK-A3/E2):PASS`（:953）
  6. `P1CJK structural: 字形 seam 三符号落点+注册接线 (P1CJK-A1/A6):PASS`（:1040）
- **device build 闸**（final r4 firmware-build 真编译）：xiaozhi.bin 0x30c5b0 bytes / 余量 11% free；设备强符号装配事实半 = TEST 6 tooth 3（kidpal_render_node.cc + font_puhui_14_1 双分支）PASS。

**与 progress.md 挂账④段的一致性结论**：挂账④（progress.md :106 ④ + :128 驱动实证 + :129 立项落包）效果承诺 =「动态内容（课程名/事项名，开放集合 CJK 文本）走字体——复用网格页已接字体、对话页同款普通字体」，plan.md 目标效果一句话 =「三钉区正常渲染、从『画不出即丢弃』变为『画得出、超宽截断不越区』」。对账：**host 侧效果全兑现**——A1 齿证明 CJK name 墨真落三钉区（next {136,48,392,168} / nextlist {136,172,392,214} / preview {136,220,392,242}，diff 定位逐区）、A2 齿证明超宽截断不越右界带 x[393..399] 零墨、A4 齿证明 home_upcoming 31 字节截半形状安全停笔、A5 齿证明缺字形退化不崩；「对话页同款普通字体」由 A6 装配事实齿承载（设备分支恰取 `font_puhui_14_1`，即 human 裁定的对话页同款）+ device build 闸过。**真机目测三点（课程名可见/与 time 数字基线协调/14px 1bpp 可读性）按 exitCriteria E3 原文口径挂设备轮窗口**（见未覆盖面申报），属设计态 deferred 非静默缩水。**一致性结论：demo 证据 == 挂账④效果段承诺（host 侧全兑现 + 真机目测按 E3 口径显式挂账），无口头宣称已验条目。**

## 未覆盖面申报（对照 acceptance[] + contractRefs + 挂账④效果段，逐条一行「设计性质 + 现状 + 去向」）

| # | 未覆盖面 | 设计性质 + 现状 | 去向 |
|---|---|---|---|
| 1 | **P1CJK-A6 真机目测三点**（课程名可见 / 与 time 数字基线协调 / 14px 1bpp 阈值化后可读性） | 设计性质 = 真机屏人眼目测面，host pixel-probe 齿不覆盖阈值化后的可读性观感（齿头注 A2/A5 弱于契约处亦显式指向真机轮补位）。现状 = device build 闸过（体积余量 11%）+ 设备强符号装配事实齿在（TEST 6 tooth 3）+ host Source Han 同源行为齿背书渲染路径正确 | **挂设备轮窗口（E3）**——planner 按 kidpal-serial-monitor 协调协议约开窗，可随下一 K 包设备轮顺带；阈值微调属设计迭代不触契约（exitCriteria E3 原文） |
| 2 | 其余 acceptance 面（A1–A5 行为齿 + A6 device build 闸半） | 逐条有行为级/结构级冻结齿承载，final r4 + host runner 全量双绿 | **none**（零缺口） |

## 冻结件 REVISION LOG 与红基线脚手架（如实入册）

- **红基线脚手架**：`firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c` + `pcn_red_wrapper.sh`（acceptance-author 脚手架，**非冻结件、不编入 host runner**，ksg 先例同款）——红基线独立编译通道（gcc 直编 + seam stub 恒不可用），使红钉在字形 seam 缺失（真实缺失行为）而非渲染 seam 缺失。红基线实跑落账 redBaselineResult：两通道（① 既有 74 齿全量 74 Tests 0 Failures rc=0 与 91b3b5c 锚零偏移；② 新齿 stub 通道 6 Tests / 5 Failures / exit 5，TEST 5 = A3 守卫 + E2 canary 基线即绿 MUST-STAY-GREEN），红因全部 = seam 未实现 → name 不画，零链接/编译/环境红。
- **冻齿 TEST 2 tooth 2 算术缺陷修复（REVISION LOG 2026-09-22，acceptance-author 执行、planner 核实，非 builder 实现缺陷）**：`pcn_diff_stats` 的 out_total 恒为整帧 diff 计数，旧断言 `(b1+b2+b3)==(t1+t2+t3)` 即 D==3D、与前置 total>0 互斥，对任何正确渲染恒红（builder 实测真整帧 diff 2086 全落三钉区并集内、测试假象 6258/4172）。修复 = total 取单次整帧计数 D、断言改 `in_band==D`（全部 diff 落钉区并集内）；判别力保全经变异复验：去右界 clip 变异（mutationEvidence P1CJK-A2）令越区像素 x>=393 落并集外 → `in_band<D` 仍转红，齿强度未弱化。修复属 tighten-only 纪律内的断言修正，冻结面（seam 契约 [S1]–[S4]、coverage map）零变。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | delivery/taskpackages/…/taskpackage.json（mutation 执行面留痕粒度） | **A3–A6 四条 mutation 的逐条执行实跑留痕在包内缺位**：包内可见的变异实跑证据 = 红基线（≙ P1CJK-A1 变异天然态「name 渲染分支删除/不调 seam」，6 Tests / 5 Failures 实跑落账）+ TEST 2 REVISION LOG 内 P1CJK-A2 去 clip 变异复验（仍转红）；A3/A4/A5/A6 仅 mutation+expectedRed 登记在册——mutation-check 闸绿是登记面 + 敏感性的机械判定，非逐条执行证明 | 包内制品 hunk（taskpackage.json mutationEvidence.perAcceptance 六条 + REVISION LOG 仅 A2 复验）+ 启发式：rigor lite 的 mutationRequired 由机械闸登记面承载，但「登记 ≠ 执行」的差距应在 packet 显式化而非默认等同 | 不阻塞；如判官/supervisor 要求执行面证明，交 planner 决定是否补一轮变异实跑（fixer 预算 iterationBudget 2 内可容纳）；本包行为齿全绿 + 红基线真红实证，误报风险低 |

## Five-Axis Review（author 语义审查）

1. **正确性**：UTF-8 解码器边界完备——首字节判长（1–4 序列）、非法首字节停笔、非法续字节停笔、不完整尾序列安全停笔（TEST 3 行为级：截半帧 == 完整 30 字节帧 diff==0 + 右界带零墨 + rc==0）；字形盒上限 64 防御（host/设备强符号 + blit 侧双重）；逐字形 advance 步进 + 钉区/FB 双重 clip；A1 diff 定位齿证明 name 墨恰落对应钉区带内。已知边界 = A2 强度申报（截断笔迹触达性依赖起笔 x 与 advance，fixture 自陈）+ A5 skip-vs-stop 不钉（不超约束，齿头注显式化）。
2. **边界**：name NULL → 返回 0 不崩；缺字形（弱默认/占位/越盒）跳过不画垃圾（TEST 4 零墨断言）；越界字形描述视为不可用；s_bits 静态行缓冲单渲染线程语义（设备侧 lvgl_port_lock 持有、host runner 单线程）与「有效期到本函数下一次调用」的 [S1] 契约一致。
3. **契约**：契约/spec 零触碰（E2 自查表五项全绿：git diff 闭集 + contract-lint 闸 + TEST 5 canary 双证）；hp1_item_t / 11 冻结契约函数 / home_upcoming / schedule_page / 烘焙资产零偏移（E2 表逐条证据）；ASCII time 路径零偏移（p1_blit_digits / p1_blit_str_scaled 调用面未动，74 既有齿零偏移背书）；seam 契约 [S1] 按齿头注冻结形状逐字落实现（kidpal_p1_font.h 声明与齿头注布局逐字段一致，TEST 6 结构齿钉三符号落点）。
4. **安全**：`securityReviewRequired=false`（r3/r4 闸绿）；无新增依赖；name 输入面来自 K2 解析层（HP1_NAME_LEN=32 上限 + 上游 safe-truncate，K2 冻结齿背书），解码器对任意字节序列安全（非法/截半/缺字形三路全不崩——TEST 3/4）；无 secrets 面、无注入新面。
5. **可运维性**：seam 双强符号对称（设备/宿主同签名同阈值化逻辑，差异仅字体源）；弱默认保留「画不出即丢弃」退化语义（未接强符号的平台行为不变，向后兼容）；kidpal_p1_font.h 集中声明；红基线脚手架与门 runner 分离（stub 不编入 host runner，CMakeLists 注册守卫 tooth 4 钉接线）；体积余量 11% 在案，无新增大静态量。

## Simplification Debt

`grep "minimal:"` 本包全部实现/测试文件（home_p1.c / kidpal_render_node.cc / kidpal_p1_font.h / host_infra_seam.c / p1_cjk_name.acceptance.c / pcn_red_seam_stub.c）**零命中**——无简化债务台账条目，全部 `no-trigger`。

## Residual Risk

- **decisionCoverage**：TaskPackage `decisionCoverage: []`（空）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 human 冒险接受项需列示。
- 残余风险实质 = 未覆盖面申报表 1 条（P1CJK-A6 真机目测三点挂设备轮窗口，E3 口径内）+ Findings 1 条 NIT（mutation 执行面留痕粒度）；行为面由 6 新齿 + 74 既有齿零偏移双垫底，device build 闸过，无新增高风险敞口，不存在静默漂移路径。

## 合并门输入摘要

- 前置依赖：dependsOn []，K1/K2/K3 全 merged、mergeToken 无持有者；base/m3-k4 @ 40ce8c7（check-launch 绿，progress.md「K4 launch 面就绪」段在案）。
- 回滚方式：单 commit revert 本包实现批即可整体回退（display 内聚簇：home_p1.c + kidpal_render_node.cc + kidpal_p1_font.h + host_app 测试面；弱符号 seam 向后兼容，revert 后设备行为回「画不出即丢弃」现状，无跨包耦合、无部署面联动）。
- supervisor 判读输入：final r4 `ok:true` + evidence-check PASS（factOk:true）+ E2 铁律五项 verified + demo 对账一致（真机目测按 E3 口径显式挂账）+ 未覆盖面 1 条挂账/1 条 none + findings 零 BLOCKING（1 NIT）。
- 本包不判定通过、不 merge、不 push——supervisor-verdict-gate.mjs 机械闸归 planner 主线程，其后 pre-merge-escalation-gate 放行 → merge-driver --pod 本地合并。
