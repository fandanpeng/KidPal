# Review Packet — 260915-m1-n1b-p1-home-battery（M1-N1b：首页新版式「接下来」主体 + 电量 ADC 接线销账）

> integrator 产出 · 2026-09-16（**接棒修复批后刷新第二版**）· 分支 `dev/260915-m1-k1a-key-layer-dispatch`（本包在 544a804 新提交面直接开发，rebase 条款作废——planner launch 确认在册 taskpackage.notes）。
> 供 supervisor 合并门复核。本包 `riskTier=medium`（host-teeth / device-round gateProfile）。**本相位只收拢冻结，不合并**——PH-A6 真机轮已获 human **PASS（附带条件）**（视觉四项表态 `f6132282` + 收尾裁定 `45910f60`，附带条件 = 三项挂起观察项入档）；合并门推进由 planner 主线程承接（final full → VRT-A2 归属 → supervisor 机械闸）。
> 交付本体：`home_p1.c` v1.4 新版式（「接下来」日程主体 + 倒计时贴纸 + 辅助条小图标前置，WF-F1-01 v1.4 重排序）+ 字形/图标缩放 blit + 电池注入缝（`kidpal_render_node.{h,cc}`）+ ADC 外部句柄构造重载（`boards/common/adc_battery_monitor.{h,cc}`）+ 板级接线与 `GetBatteryLevel` override（`waveshare-s3-rlcd-4.2.cc`，`s_battery_pct` 恒 100 销账）+ chrome 电池残留摘除与顶带同线（`lcd_display` / `lvgl_display` / 各板 emoji/watcher 面）+ host 接线 + host 截图 harness（`host_app/screenshot/`）+ 冻结验收 `p1_home_battery.acceptance.c`（5 TEST_CASE）。
> **合并门必读**：承 bearing final fact **r6 非 full 绿**（`ok:false`），红 = `unit`/`acceptance` 两 full 闸 **3 failed**：SMK-A4 + SRE-E4（K1a closeout 锚在册 server 基线红）**+ VRT-A2（见 Findings 行 1——锚基线引用与验证树态间的漂移红，根因 = 544a804 验收归档 commit，非本包代码面引入）**；**接棒批 r7-r10 四轮 scoped 全绿（含 fix-visual / fix-visual2 / fix-align 三批修复后复跑）**。本包自身面（scoped 闸全绿 + host runner 13/0 + 新齿 5 条全绿 + mutation 10 条执行敏感 + PH-A6 human PASS）达标。

## 四阶段闭环事实

| 阶段 | 事实 | 锚点 |
|---|---|---|
| intake/planner 派单 | 拆包 M1-home-qa 开工批（块②），brief 落盘，K1a 已并本地 main、本包在 544a804 面直接开发 | taskpackage.notes（2026-09-15） |
| acceptance-author | 冻结 `p1_home_battery.acceptance.c`（5 TEST_CASE）+ 有效基线落账：host 通道① 8 Tests / 0 Failures（544a804 归档后现役面收敛，K1a 期 4 条在册红移出现役面、owner 不变）+ 通道② 新齿 gcc 实编 COMPILE_OK + 解析性红定性 | taskpackage.redBaselineResult（2026-09-15） |
| builder | 实现落盘（见 §Change Size）+ mutation 执行 10 变异全敏感（phase:"mutation" dispatch completed）；接棒后三批修复：fix-visual（旧电池图标残留/日期英文 DD Mon/WiFi 小标偏大，`88da59d6`）→ fix-visual2（imgfont 大小写编码 52 bin 重生成 + font_awesome_14_1 降档，`670b56e0`） | pod-events `aa63f058` / `b3d1b84f` / `88da59d6` / `670b56e0` |
| verifier | verify.mjs 十轮：r1 红（mutation-evidence 缺失）→ r2 scoped 绿 → r3/r6 final（3 failed，见 §红基线判读）→ r4/r5 scoped 绿（fixer 字形/图标两轮后）→ **接棒批 r7/r8 scoped 绿**（fix-visual 后）→ **r9 scoped 绿**（fix-visual2 后，接棒 escalation `7ea5111a` 消账 `47b3ea0b`）→ **r10 scoped 绿**（fix-align 后） | verify-runs.jsonl 十条 + rounds/r1-r10.json |
| integrator（本相位·刷新） | 承 bearing final fact r6 checker PASS + 真机轮证据归档（boot 捕获四份 + 记录件 PASS 刷新）+ closeout 制品刷新 + 接棒修复批 commit | 见 §Verification Evidence |
| fixer（fix-align） | % 真字形 `imgfont_pct.bin` + 辅助条单行垂直居中 + chrome 顶带同线（r3/r9 两轮真机看屏回报 `% ❌ / 对齐 ❌` 的收敛修复，`599fd15c`） | pod-events `83180e57` / `599fd15c` |

### 接棒重启与两轮修复（2026-09-16 晨，planner escalation `7ea5111a` 在案）

- 前一任 integrator/closeout 会话被 human 误停后 planner 接棒重启：盘面自检确认 fix-visual 批（旧电池图标残留 / 日期英文 DD Mon / WiFi 小标偏大）无 verifier 覆盖、工作树未提交、小写 imgfont bin 未生成。
- **新发现并修复 imgfont NTFS 大小写碰撞缺陷**：Windows NTFS 文件名大小写不敏感使大小写 bin 互相覆盖（bin 内容实为小写），host fopen 同样不区分大小写故截图侥幸正常（host 截图绿），但设备 SPIFFS 大小写敏感、小写 bin 实缺会渲染失败（真机 "Sep" 渲染成 "S"）——**host 截图绿 ≠ 真机绿的跨端分歧**。裁大小写编码文件名（`imgfont_letter_X` 大写 / `imgfont_letter_lc_x` 小写，52 枚重生成，`tools/asset-pipeline/gen_imgfont_letters.py`），解码侧 `art_assets.c` 同规则分流。
- 补做 human 裁定 `294dafb6`：联网小标 font_awesome_14_1 降档（与温/湿/电小标同档）。
- r9 烧录后 human 看屏回报（`83180e57`）：断网 WiFi 斜杠 ✅ / % 百分号仍不对 ❌ / 辅助条未对齐 ❌ → fixer fix-align 批（% 真字形 + 单行垂直居中 + chrome 顶带同线）→ r10 烧录后 human 看屏终验通过（`f6132282`）。

## Change Size

- Files changed（git diff --stat 实测，两版合计）：
  - **实现面 = 20 文件**：首版 7 文件（`home_p1.c` v1.4 新版式 + 缩放 blit、`kidpal_render_node.{h,cc}` 注入缝、`adc_battery_monitor.{h,cc}` 外部句柄重载、`waveshare-s3-rlcd-4.2.cc` ADC 接线 + override + 注入、`host_app/main/CMakeLists.txt` 接线批）+ 接棒修复批新增面：chrome 电池残留摘除 / 顶带同线（`lcd_display.{h,cc}` / `lvgl_display.{h,cc}` / `oled_display.cc` / `electron_emoji_display.cc` / `otto_emoji_display.cc` / `sensecap_watcher.cc` / `magiclick` 三板 / `custom_lcd_display.{h,cc}`）、`art_assets.c`（imgfont 大小写分流解码 + % 字形）、`home_p1.{h,c}`（% 真字形 + 辅助条单行垂直居中 + 日期 DD Mon）、`waveshare-s3-rlcd-4.2.cc`（补注入/时间面微调）。
  - **冻结验收件**：`firmware/main/test/p1_home_battery.acceptance.c`（新文件，5 TEST_CASE，PH-A1~A5/A7 机制级齿）——零修订。
  - **资产与工具**：`firmware/main/assets/spiffs_root/imgfont_letter_A..Z` + `imgfont_letter_lc_a..z`（52 枚，大小写编码）+ `imgfont_pct.bin`（% 真字形）= 本包交付资产；`tools/asset-pipeline/gen_imgfont_letters.py`（字母字形生成脚本，Baloo 2 800 母版同源）；`firmware/main/test/host_app/screenshot/home_p1_screenshot.c`（host 截图 harness——挂真资产走真字形路径出 PNG，视觉迭代工作法基建，human correction `416e4707` 在案）。
  - **delivery 记账面**：taskpackage 活文件 + input-briefs + pod-events.jsonl + verify-runs.jsonl + verifier-evidence/rounds/r1-r10 + current/ 指针投影 + `docs/ARCH-REVIEW-QUEUE.md`（ARQ-260916-home-date-format-english 登记）。
  - **closeout 制品**（`changes/dev/260915-m1-n1b-p1-home-battery/`）：`review-packet.md` / `docs-delta.md` / `device-round-record.md`（PASS 刷新版）+ boot 捕获日志四份。
- Scope: firmware 首页渲染 + 板级 ADC + chrome 共享底座（接棒批按 human 裁定扩入电池残留摘除与联网小标降档，均在 TaskPackage PH-A1/A2/A5 语义内）；`docs/contracts/**` 与 `docs/requirements.md` **零改动**（contract-lint r10 PASS；PH-E1 兑现——schedule API 面零触碰，小伴睡姿演出已砍项零触碰）。
- Split recommendation: **不拆**——版式、注入缝、ADC 接线、验收齿是「电量端到端 owner = 本包独占」（crossPackageBehaviors 在册）的单一语义收敛，拆开任一面都使「板级接线 ↔ 注入缝 ↔ 齿」失去同批可验性。

## Verification Evidence

> **承 bearing fact 声明（诚实口径）**：final completed-only round 仍为 **r6**（`phase:"final"`，`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`gatesRun:9`、`invocationId 56629d11-faa6-451b-adda-d6f0f5beedfb`）——接棒批后尚未跑新 final full（归 planner 主线程：final full → VRT-A2 归属 → supervisor 机械闸）。接棒后承重证据 = **r7-r10 四轮 scoped 全绿**（r10：`phase:"scoped"`、`ok:true`、`invocationId 5c39c8c0-f76c-42a4-b27d-10b0309959e6`、2026-09-16T04:13Z，七闸全绿含 firmware-build 真编译 `xiaozhi.bin 0x2c8070 / 0xa7f90 (19%) free`）。
> `current/verdict.json` 为投影（non-bearing compatibility projection，脚本 fs-write，现反映 r10）；closeout 判断以 rounds fact 为准。
> **checker**：r6 fact `verifier-evidence-check.mjs --phase final --round 6` → `{"ok":true,"factOk":false,...}`——final evidence 结构有效、走红证据 ESCALATE 通道进 closeout。

- **r6 逐闸（final）**：taskpackage-required / firmware-build 真编译（`xiaozhi.bin 0x2c6850`）/ contract-lint / acceptance-coverage（PH-A1..A7 全在册、`structural:["PH-A6"]`）/ mutation-check（10 条逐字节一致）/ docs-sync / security 全绿；**unit + acceptance 两 full 闸红 = 3 failed**（见 §红基线判读）。
- **接棒批 scoped 轮（r7/r8/r9/r10，均 `ok:true` 全绿）**：r7（00:58Z，接棒自检后）→ r8（01:34Z，fix-visual 批后）→ r9（03:40Z，fix-visual2 批后：imgfont 大小写 52 bin + font_awesome_14_1 降档）→ r10（04:13Z，fix-align 批后：% 真字形 + 单行对齐 + 顶带同线）。每轮 firmware-build 真编译通过；contract-lint / acceptance-coverage / mutation-check / docs-sync / security 均绿。
- **红基线判读（PH-A7 判据）**——两层分述，含一处与移交口径的偏差申报：
  - **host runner（板侧，本包面）**：有效基线 8 Tests / 0 Failures（acceptance-author 冻新齿时落账 + planner 独立复核双确认）。终跑 = **13 Tests / 0 Failures / 0 Ignored**（8 基线 + 本包新齿 5 条全绿，接棒修复批后零新增红）→ 「相对有效基线零新增红 + 本包新齿全绿」达标，PH-A7 板侧判据满足。
  - **server 面（full 闸）**：unit `3 failed, 166 passed` / acceptance `3 failed, 129 passed`。**偏差申报：移交口径称「仅 2 失败 = SMK-A4 / SRE-E4」，rounds 事实为 3 failed**——第三条 **VRT-A2**（`test_voice_roundtrip_b.acceptance.py::test_a2_protocol_profile_twin_aligned`：C-side twin 缺失）。根因 = commit **544a804**（human 裁定、跳过 dev-loop 的一次性验收归档）使 server 齿钉住的旧路径失配；K1a 锚基线（2 failed / 130 passed）取于归档前。本包验证窗口 r1→r10 恒 3 failed，**本包代码面零引入、零新增**；owner 判读**仍 pending human 归属裁定**（final full 后 planner 向 human 提出，pod-events `dfda0852` escalation resolution:pending）。SMK-A4（owner=260829-smoke-e2e）与 SRE-E4（owner 归 intake 判读）逐条命中 K1a 锚在册红，均与本包固件面零相关。
- **Mutation 敏感断言 provenance**：`taskpackage.mutationEvidence.perAcceptance` 10 条（PH-A1×2 / PH-A2×2 / PH-A3×2 / PH-A4×2 / PH-A5×1 / PH-A7×1），全部 `failed as expected` / `sensitive:true`，执行证据 = builder mutation 相位实跑（pod-events `b3d1b84f`）。两条边界如实记录：① `PCT_WRITE_BYPASS` tooth7 失敏为验收切片宽度设计边界（换 tooth11 命中）；② 变异命名纪律合规。接棒修复批未触碰冻结验收件（`p1_home_battery.acceptance.c` 零修订），mutation 登记面继续有效。
- **PH-A6 真机确认轮（kind=structural，human 持板）——PASS（附带条件）**：烧录四轮 + boot 捕获四份在册（PH-A3 端到端真机实证：`Use external ADC handle` + `battery_inject corr_id=kdp-1222 level=95 charging=0 vbat_ok=1`，真电量 95 非恒 100）；human 看屏四轮迭代收敛，最终轮逐字「可以了，整体都可以」——单行对齐 ✅ / % 百分号 ✅ / WiFi 同线 ✅ / 整体观感 ✅（`f6132282`）；收尾裁定 = 充电捕获/断网切换跳过、按 PASS 处理（`45910f60`）。**附带条件 = 三项挂起观察项入档、标「human 后续观察、非验证通过」**：① charging=1 捕获窗；② 联网→断网切换沿（开机默认态已验）；③ 低电闪烁真机可见性（PH-E2 降级在案）。证据与记录 = 同目录 `device-round-record.md`（PASS 刷新版）+ 四份 `device-round-*-boot-capture-20260916.log`。
- **Security**：r10 `ok:true`（`securityReviewRequired:false`）。无新公网入口 / secrets / 依赖；ADC 读取复用既有 `AdcBatteryMonitor` 组件、注入缝为固件内部 seam、无动态分配面新增。
- **Docs sync**：r10 `ok:true`（`docsSyncRequired:false`）。判定与逐条理由 = 同目录 `docs-delta.md`（含日期英文格式实现侧注记——ARQ-260916-home-date-format-english 在册）。**不改 docs/contracts/**。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 **N/A (worktree)**，以 `git diff --stat`（上列文件面）+ 关键共享符号 `trace_path` 抽查替代：图索引（main 侧历史面）在册 `AdcBatteryMonitor.GetBatteryLevel`（boards/common）与 `Board.GetBatteryLevel` 基座默认实现；waveshare 板级 override 不在索引 = 本包 worktree 新增面，与 git diff 面一致；接棒批 chrome 面改动（lcd_display/lvgl_display 各板）为既有符号体内修改，无跨文件面外溢。
- **Re-index 需求**：主索引（main 侧）尚不含本包新符号（板级 override、注入缝、screenshot harness 等），re-index 归主线程 / human，不虚报。

## Demo 对账行（workstream m1-home-qa 纪律）

| 证据形态 | 状态 | 说明 |
|---|---|---|
| host 截图（视觉自查 harness） | **在册** | `firmware/main/test/host_app/screenshot/home_p1_screenshot.c`（挂真资产走真字形路径出 PNG，human correction `416e4707` 工作法基建）；本轮 demo 证据 = `c:/tmp/p1shots/` 下 `home-normal_strip_5x.png` / `hum_batt_pct_7x.png` / `date_crop.png` / `auxstrip_7x.png` / `temp_7x.png` 等（**只引用路径不拷贝**，重资产留仓外） |
| host runner | **在册** | 13 Tests / 0 Failures / 0 Ignored（8 基线 + 新齿 5 条全绿，PH-A7 板侧判据）；mutation 10 条执行敏感 |
| 真机轮 | **PASS（附带条件）** | 四份 boot 捕获日志在册（PH-A3 端到端实证）；human 看屏终验四项全过（`f6132282`）+ 收尾裁定（`45910f60`）；三项挂起观察项入档 |

**与 roadmap 本包效果段逐条对账**（roadmap = `delivery/workstreams/m1-home-qa/roadmap.md` §260915-m1-n1b-p1-home-battery「完成后人能看到什么」）：

| roadmap 效果要素 | 对账结果 | 证据 |
|---|---|---|
| 辅助条各项图标前置（联网小标打头、温/湿/电各带小图标、湿度不裸数字挨电池） | **一致 ✅** | 真机看屏终验单行对齐/WiFi 同线 ✅（`f6132282`）；host 截图 `auxstrip_7x.png` |
| 电量 ADC 三处死成员接线 + 注入缝，真机电量首次显示真实值 | **一致 ✅** | 四份捕获日志 `battery_inject level=95`（真电量非恒 100）+ `Use external ADC handle` |
| 低电时电池图标每秒闪、充电即停 | **降级承载（在案偏差，非不一致隐瞒）** | PH-E2 降级 human 裁定 `4c10c620`：真机可见性 = 挂起观察项 ③，由 host 机制齿（PH-A4 两条 mutation 敏感齿）+ host 截图 `home-doze-visible/hidden` 承载 |
| 断网时联网小标变 WiFi 斜杠 | **一致（限开机默认态）** | 真机看屏回报 ✅（`83180e57` ①）；主动断网切换沿 = 挂起观察项 ②（human 裁跳过观察，`45910f60`） |

不一致处如实申报：即上表两处（低电真机可见性、断网切换沿），均为 human 明示裁定/跳过的挂起观察项，非未披露偏差。

## Simplification Debt

> 本包实现面 = 版式重排 + 缩放 blit + 注入缝 + 外部句柄构造重载 + imgfont 大小写分流 + % 真字形，**无新依赖、无跨层新调用（注入缝在既有 render node 接口内、ADC 复用既有组件）、无 `minimal:` 标记**（全仓 `grep minimal:` 无本包新增命中）。**无条目、无 no-trigger 项**——诚实声明「无简化债务」。删的是现状缺陷（`s_battery_pct` 恒 100、`GetBatteryLevel` 基座默认 false 空转、chrome 旧电池图标残留），是 `fix` 语义、不留债。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | final 闸红（锚基线漂移，跨包） | **VRT-A2 红 = 锚基线引用与验证树态间漂移，非本包引入**：移交口径「仅 2 失败（SMK-A4/SRE-E4）零新增」与 rounds 事实（3 failed）不符；第三条红根因 = 544a804 验收归档（human 裁定一次性操作）移走 C-side twin，server 齿未同步适配。本包窗口 r1→r10 恒 3 failed，代码面零引入。**归属裁定仍 pending**（final full 后 planner 向 human 提出） | Basis: r6 fact stdout（`VRT-A2 RED: C-side twin … missing`）+ `git show 544a804 --name-status`（R100 归档）+ K1a 锚取点早于归档的时间线；heuristic: `[evidence]` 基线引用漂移是申报义务，归属判读非本包权限，不自裁 | owner 判读归 planner/human：server 面 VRT-A2 齿做归档适配（改钉 archive 路径或撤存在性断言）或 human 裁例外；本包不代修。合并门放行前须与 SMK-A4/SRE-E4 一并进红证据 ESCALATE 裁决 |
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 + SRE-E4 命中 K1a 锚在册红（不伪造不代修）**：screen.md 证据件缺口（owner=260829-smoke-e2e）+ spec 送达语义记录红（owner 归 intake 判读）。均 server 面、与本包固件齿零相关 | Basis: r6 fact stdout + taskpackage.redBaselineResult 锚引用；heuristic: 在册留红如案、不越权代采 | 留红如案：维持各自 owner 挂账，本包不代修 |
| 观察项 | judgement-call | 真机挂起观察（human 后续观察、非验证通过） | **PH-A6 PASS 附带条件三项**：① charging=1 捕获窗（充电路径真机读数）；② 联网→断网切换沿（开机默认态已验、主动切换未验，MAC 隔离 human 裁跳过）；③ 低电闪烁真机可见性（PH-E2 降级在案）。human 逐字「这两个不好测，跳过吧，我后面观察」——三项标「human 后续观察、非验证通过」，不虚报绿 | Basis: human decision `45910f60`（收尾裁定）+ `4c10c620`（PH-E2 降级）+ `83180e57`（看屏回报限定「开机默认态」）；heuristic: human 明示跳过项按观察项入档，不升 BLOCKING（PASS 已裁）也不冒充已验 | 留档日常观察（`device-round-record.md` 挂起观察项节）；若后续观察发现问题另开修复批 |
| SHOULD | judgement-call | host 齿盲区（follow-up 候选，部分销账） | **验收 fixture 默认不挂 art（weak 哨兵走兜底），真字形/真图标渲染路径 host 零覆盖**：fixer 两轮真机迭代（字形溢出、图标裁半）正是该盲区漏放的缺陷。**接棒批部分销账**：host 截图 harness（`home_p1_screenshot.c`）已挂真资产走真字形路径出 PNG（human correction `416e4707`），且该 harness 恰好暴露了 NTFS 大小写碰撞跨端分歧的反面教训（host fopen 不区分大小写 → 截图绿不能证真机绿） | Basis: pod-events `1514491d` friction + `416e4707` human correction（「follow-up 4 的部分销账」原文）+ `home_p1_screenshot.c` 落盘；heuristic: 盲区收紧齿（「图标半区相邻空隙零墨」）仍未入 acceptance | 剩余部分：host harness 收紧齿入验收批，归后续测试基建批；planner 立项裁优先级 |
| 观察项 | judgement-call | 跨包交叉（部分消账） | **基座 chrome 共享面观感项**：联网小标偏大已由接棒批消账（font_awesome_14_1 降档，human 裁定 `294dafb6` 并入本轮修复）；旧电池图标残留同批摘除。剩余：日期描边框（设计内殿后，日期格式改 DD Mon 后已消）——无未决跨包观感项 | Basis: human decision `294dafb6`（追加裁定并入修复）+ r10 烧录版看屏终验 ✅；heuristic: 跨包归属项消账留痕 | 无（消账留痕） |

**Kind 说明**：五行均 `judgement-call`（无 hard-violation：行 1 基线漂移申报、行 2 在册留红、行 3 human 明示观察项、行 4 测试基建盲区部分销账、行 5 跨包消账留痕）。**无 BLOCKING finding**——PH-A6 已获 human PASS（附带条件），合并门停等项解除；行 1 为升级裁决项（进红证据 ESCALATE）。final fact `ok:false` 照 precedent 走红证据 ESCALATE 通道。

## Five-Axis Review

1. **Correctness（对 PH-A1~A5/A7 冻结接缝契约 + requirements S-F1.1 AC1–AC4）**：逐条兑现——PH-A1 版式 v1.4 重排（「接下来」主体 + 倒计时贴纸 + 小伴演出保留，空日程门齿钉死）+ 日期 DD Mon（human 裁定，实现先行）；PH-A2 辅助条小图标前置（env 有效性门 + 图标半区墨齿 + 单行垂直居中 + 联网小标降档）；PH-A3 三处死成员接线 + override 实装 + 注入缝（mutation 双变异敏感）+ **真机端到端实证（level=95 非恒 100，四份日志同径）**；PH-A4 低电闪烁相位门 + 阈值边界齿（真机项按 human 裁定 PH-E2 降级，不虚报绿）；PH-A5 断网 WiFi 斜杠钉点 + 真机开机默认态实证；PH-A7 host 13/0 达标。真机迭代缺陷（字形溢出/图标裁半/NTFS 大小写碰撞/% 基元不达标/辅助条错位/小标偏大/电池残留）全部修复并复验（r7-r10 scoped 全绿 + 重烧看屏）。
2. **Test quality**：冻结验收为机制级（注入缝符号级 + 相位门 + 阈值边界 + 注册接线序）而非纯行为级，负向 needle 齿防假绿；mutation 10 条**执行在册**全敏感；host 截图 harness 补真字形路径自查面（但 host 截图绿 ≠ 真机绿——NTFS 大小写碰撞教训在案，跨端分歧须真机钉点）。盲区剩余部分（收紧齿）如实申报为 follow-up。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/requirements.md` 零触碰（contract-lint r10 PASS + PH-E1 兑现：schedule API 面零改动、已砍演出零触碰）；电池注入缝是固件内部 seam 不暴露契约面；chrome 面改动（电池残留摘除/小标降档）为既有符号体内修改，各板类路径零回归（host 8 基线齿全绿佐证）。
4. **Operability / security**：无新公网入口 / secrets / 依赖；ADC 读数经既有组件校准路径（vbat_ok 门）、注入 corr_id（kdp-1222）链路完整可追；四轮真机无 crash/复位循环（boot 捕获日志在册，SRAM free 稳定 ~64KB）；rtc_pcf85063 时间面读数在册（date=01-02 为 RTC 未同步态，SystemInfo 日志同径在案，非本包 seam）。
5. **Maintainability / simplicity**：改动收敛于 display 层单 seam + 板级接线 + 资产生成脚本（`gen_imgfont_letters.py` 与既有 asset-pipeline 同源同格式，lineage 头注在册）；无过早抽象、无新依赖、无 `minimal:` 债务。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=medium`，风险面是渲染正确性与硬件读数（host 齿 + mutation + PH-A6 真机轮覆盖），非探索式攻击面，本 packet 如实记录「未声明 ⇒ 未执行」。

## UI design-review 证据（bootstrap 期纯文本口径）

- **Viewport coverage**：首页 400×300 单视口（真机渲染日志 `render home 400x300 render_rc=0` 四份在册）；v1.4 冻结版式分区坐标面承载，无滚动/多视口面。
- **A11y summary**：本包面无可访问性新义务（无新交互控件；低电闪烁为视觉信号，human 裁定 PH-E2 降级承载）；自动 a11y 脚本 bootstrap 期 deferred。
- **Design-drift summary（brief↔实现）**：WF-F1-01 v1.4 重排序逐区兑现（host 齿 + 真机四轮迭代收敛）；历轮 drift（字形溢出 → 图标裁半 → NTFS 大小写碰撞 → % 基元 → 辅助条错位 → 小标偏大/电池残留）全部由真机轮/host 截图抓出、修复闭环（`device-round-record.md` 在案）。**在案已知漂移一处**：日期格式 DD Mon 与 requirements/WF-F1-01 现文（中文日期）不一致——human 裁定实现先行、需求/设计殿后同步（`474491c9`，ARQ-260916-home-date-format-english 在册，同步完成前属在案已知漂移不另开 review）。
- **External artifact reference**：host 截图 `c:/tmp/p1shots/`（`home-normal_strip_5x.png` / `hum_batt_pct_7x.png` / `date_crop.png` 等，只引用路径不拷贝）；boot 捕获日志四份为本目录在册文本证据。
- **Residual risk**：视觉终验已过（human 四项 ✅）；残余 = 三项挂起观察项（充电读数/断网切换沿/低电可见性，human 后续观察）+ host 收紧齿缺位（Findings 行 4）。UI severity 复用 `code-review-gate.md` 既有轴，无 UI BLOCKING、不新增 gate。

## Residual Risk

- Known limitation / 留红如案（均非本包代码面引入、owner 已登记或判读归属已写明）：
  1. **SMK-A4 + SRE-E4 final 闸红**（K1a 锚在册 server 基线红）：本包不代修；SMK-A4 owner=260829-smoke-e2e，SRE-E4 归 intake 判读。
  2. **VRT-A2 锚基线漂移红**（544a804 归档副作用）：**仍 pending human 归属裁定**（final full 后 planner 向 human 提出，`dfda0852` resolution:pending）。
  3. **host 通道 K1a 期 4 红**（SU-E3/KR-A6/WSA-A4/WSA-A6）：随 544a804 归档移出现役面，owner 不变、基线消解走独立批。
- **PH-A6 挂起观察项三项（human 后续观察、非验证通过）**：① charging=1 捕获窗；② 联网→断网切换沿（开机默认态已验）；③ 低电闪烁真机可见性（PH-E2 降级）。风险 = host 机制齿已钉注入缝与相位门、boot 日志已证端到端接线，但充电路径真机读数与主动断网切换无真机实测。
- **挂起项台账（诚实全列）**：① 三项 PH-A6 观察项（human 后续观察）；② 日期格式 spec 殿后同步（ARQ-260916-home-date-format-english，human 自行同步）；③ host 收紧齿 follow-up（部分销账后剩余）；④ final full 未跑（r6 后四轮修复未做 full 复跑——scoped 全绿 + 修复面收敛于显示层，full 复跑归 planner 合并门前执行）。
- **Mutation 证据边界**：10 条执行在册（builder mutation 相位实跑），mutation-check 闸为结构闸；tooth7 切片宽度边界在册，如需可独立复验。
- Human decision needed：
  - **VRT-A2 owner 判读**（final full 后 planner 向 human 提出，ESCALATE）。
  - **红证据 ESCALATE**：final fact r6 `ok:false`（SMK-A4 + SRE-E4 在册红 + VRT-A2 漂移红），提 human 裁「接受本包合并（红转交各自 owner）或先修再合并」——由 supervisor 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 四条 decision 的 `landsIn` 全部指向 acceptance/exitCriteria（PH-A1/PH-A3/PH-A4/PH-E1），**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项。

## Closeout

- 制品面：`review-packet.md` / `docs-delta.md`（两件 blocking required 在场）；`device-round-record.md` = PH-A6 PASS（附带条件）刷新版 + boot 捕获日志四份（真机证据件）。
- 机器件（如实引用路径，不复制正文）：承 bearing final fact = `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/r6.json`（final 判读面）+ r7-r10.json（接棒批 scoped 全绿面）；`current/verdict.json` = 投影·advisory（脚本 fs-write）；checker PASS 见 §Verification Evidence。
- post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 检查满足，只增不改）。`pod-retro.md` 归 planner 收尾相位职责（Q5=warn advisory、零追溯白名单），本 integrator 不代写、不改动。
- `closeout-check.mjs`（默认只读）结果见交付报告；advisory 项如实记录不阻塞。
- milestone/workstream 登记（`milestones.jsonl` / `workstreams/m1-home-qa/progress.md`）归主线程对账轮统一渲染（本包不改，docs-delta.md 已申报）。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author/fixer 已落工作树（firmware 实现 20 文件 + 冻结验收件 + spiffs bin 53 枚资产 + 截图 harness + `tools/asset-pipeline/gen_imgfont_letters.py`），积分收拢提交（本相位）把 closeout 制品 6 件（含四份 boot 日志）+ dev-loop ledger（taskpackage 活文件 / input-briefs / pod-events / verify-runs / rounds r1-r10 / current/ 指针投影）+ ARQ 登记收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`（机器再生面常驻脏，Pod 分支禁止私带快照 commit）、不 stage `firmware/build/**`**。
- 可回滚路径：全部交付物为当前分支 `dev/260915-m1-k1a-key-layer-dispatch` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（恢复恒 100 注入 / 基座默认 GetBatteryLevel / v1.4 前版式）。main 零暴露。
- 合并相位：PH-A6 已 PASS（附带条件）→ planner 主线程 final verify full → VRT-A2 归属 ESCALATE → supervisor 机械闸（`supervisor-verdict-gate.mjs`）→ planner 前置升级闸 → 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 24）。本 integrator 不自行判定通过、不执行 merge、不 push。

## 合并门须知（短清单）

1. **PH-A6 已 PASS（附带条件）**：human 视觉四项表态 `f6132282` + 收尾裁定 `45910f60`；三项挂起观察项入档（human 后续观察、非验证通过）。合并门推进归 planner 主线程。
2. 承 bearing final fact = **r6**（phase final，scoped 7 闸绿 + unit/acceptance 3 failed：SMK-A4 + SRE-E4 在册锚红 + VRT-A2 漂移红）；接棒批后 **r7-r10 四轮 scoped 全绿**（最新 r10，含三批修复后复跑）。supervisor 复核读 `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/` 下 r6（final 判读）+ r10（最新 scoped）。
3. **偏差申报**：移交口径「仅 2 失败零新增」与 rounds 事实（3 failed）不符——VRT-A2 根因 544a804 归档（human 裁定一次性操作），本包窗口内零新增，owner 归属裁定仍 pending（final full 后 planner 向 human 提出，须进 ESCALATE）。
4. **final full 未复跑**：r6 后四批修复未做 full 复跑（scoped 全绿 + 修复面收敛于显示层/chrome），full 复跑归 planner 合并门前执行——合并门判读以届时新 final fact 为准。
5. host runner 13/0 = 8 基线 + 新齿 5 条全绿，PH-A7 板侧判据达标；mutation 10 条执行在册全敏感；冻结验收件零修订。
6. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道；本包自身面无 hard-violation、无 BLOCKING finding。
7. 冻结面（`docs/contracts/**`、`docs/requirements.md`）git 零触碰；`docs/ARCH-REVIEW-QUEUE.md` 仅新增 ARQ 登记（queue 面非 spec 面）。
8. demo 对账：host 截图（`c:/tmp/p1shots/` 路径引用）+ host 13/0 在册 + 真机轮 PASS（四份 boot 日志在册）；roadmap 效果段两处在案偏差（低电真机可见性、断网切换沿）均为 human 明示裁定的挂起观察项。

## 追记（2026-09-16，VRT-A2 退役批，integrator 收口）

> append-only 追加节，上文各行照旧存档；本节为准的状态更新如下。

- **VRT-A2 归属裁定已销账（human 裁定退出现役，非修绿）**：上文 Findings 行 1 与 Residual Risk 第 2 条、挂起项台账中「VRT-A2 owner 判读 pending」的状态**更新为 resolved（销账）**。human 裁定（2026-09-16，主线程 AskUserQuestion 留痕，pod-events human-decision `bba7d239`，decision=`vrt-a2-retire-out-of-scope`，逐字「不要跑这个了」）：`test_voice_roundtrip_b.acceptance.py` server 验收**整体退出现役运行范围**。
- **机制判读**：该件是 MVP 验证期验收的 server 半边，其设备半边（C 孪生 `firmware/main/test/voice_roundtrip_b.acceptance.c`）已被 544a804 归档退役——server 半边留现役纯属归档批**扫尾遗漏**（VRT-A2 孪生对齐齿 `C_TWIN` 硬编码指旧址 :113 故红），非产品缺陷、非本包引入。
- **落地形态**：模块级 skip（`server/acceptance/test_voice_roundtrip_b.acceptance.py` +16 行 `pytestmark = pytest.mark.skip(...)`，acceptance-author 落码；按 human 明示跳过流程的一次性操作落账）。bba7d239 事件初拟「移入 server/acceptance/archive/」，实际落码改为模块级 skip——原因在码内注释在案：gate 收集 glob 为 `server/` 递归、物理移目录仍被收集且需动治理脚本，故不搬目录。**回滚 = 删除该 pytestmark 块**（单文件单块，无其他耦合面）。
- **final r12 判读（承 bearing final fact 更新）**：r12（`phase:"final"`、`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`invocationId b680d6ba-eff3-45d8-8965-a22e492f60fc`、2026-09-16T06:03Z）——**七闸全绿**（taskpackage-required / firmware-build 真编译 `xiaozhi.bin 0x2c8070 / 0xa7f90 (19%) free` / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）+ unit/acceptance 两 full 闸**唯余 SMK-A4 / SRE-E4 两条在册红**（unit `2 failed, 159 passed, 14 skipped` / acceptance `2 failed, 122 passed, 14 skipped`）。**VRT-A2 已退出失败面**（skip 生效、失败计数 3→2），r6 时点的第三红消除。checker：`verifier-evidence-check.mjs --phase final --round 12` → `{"ok":true,"factOk":false,...}` exit 0——final evidence 结构有效，红证据照 precedent 走 ESCALATE 通道（红项均见下条红清单件）。
- **escalation `dfda0852` resolved**：r6 时点的 VRT-A2 归属 escalation 由 bba7d239 human 裁定消账。
- **在册红清单件**：本目录新增 `existing-red-inventory.md`（对偶 260915-m1-k1a 包同名件格式）——SMK-A4 / SRE-E4 两条在册设计态红（owner 不变、本包不代修）+ VRT-A2 条目（人裁退出现役销账，非修绿，引 bba7d239）。
- **扫尾项登记**：server 面可能仍有 MVP 期孪生/伴随齿未随 544a804 归档批退役（已实证一例 = 本件 server 半边），留独立批 sweep——`ARQ-260916-server-mvp-teeth-sweep` 已登记 `docs/ARCH-REVIEW-QUEUE.md` 队尾，**不阻本包**。
- **合并门须知更新**：上文「合并门须知」第 2/3/6 条中 VRT-A2 相关表述（漂移红、pending 裁定）以本节为准——final 判读面改读 r12；剩余合并门红项 = SMK-A4 / SRE-E4 两条在册锚红（launch 预报先存，红证据 ESCALATE 豁免件 = `existing-red-inventory.md`）。
