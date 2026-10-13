---
changeId: 261013-p1-tomorrow-row-sliver-fix
title: 首页 P1 明天区行流 1px 残线修复（pv_cy1 对齐卡底 + 整行容纳守卫）
rigor: lite
riskTier: low
sourceType: regression
reviewed-by: architect (/triage-change 分流 review)
date: 2026-10-13
---

# 根因

真机截图实证：今天区空（AC5 话术）时首页 P1「明天」区第 2 个日程项被剃成 1px 残线。

- `firmware/main/display/home_p1.c` L1173：明天区行流独立 clip 底界 `pv_cy1 = 218` 硬编码，
  约束来源是已退役的旧 PREVIEW_NEG 负断言带（上缘 220 + 2px 缓冲），非当前布局必需。
- L826 `p1_blit_flow` 逐像素行 clip（`py > cy1` 跳过）：部分越界的行不是整行让位，而是
  只画文字顶部恰好落在界内的 1px —— 残线伪影的机制根源。
- 复现几何：今天区空 → 明天区锚点 ty = 68 + max(2,0)*28 = 124；帽标 157..180；项行流
  fy_pv = 184。第 1 行带 184..211 完整；第 2 行带 212..239（文字 218..231）仍在卡区
  cy1=242 之内，但被 pv_cy1=218 剃成 y=218 一条 1px 墨线。
- 同类隐患：今天区行流 clip 到 cy1=242，第 7 项（行带 236..263、文字起笔 242）同样
  出 1px 残线——两区同病，本包一并修。
- 219..242 × x152..392 现无任何其他绘制（波浪/四角星/小伴均在卡区左界 152 之外），
  放宽 pv_cy1 至卡底安全。

# 修复

## 改动点 1：firmware/main/display/home_p1.c `p1_draw_card()`（~L1168-1260）

1. L1173：`pv_cy1` 218 → 242（对齐卡区底界 cy1）。同步改 L1166-1167 函数头注释与
   L1173 行注释：底界依据改为「卡区底界 containment」，注明旧 PREVIEW_NEG 退役面
   约束随本包退役、HPB 负断言带已重锚至卡底之下（见改动点 2）。
2. 明天区 preview 行循环（L1241-1250）：每次 blit 前加**整行容纳守卫**——
   `if (fy_pv + line_h - 1 > pv_cy1) { break; }`（行带底越界即整行让位，后续项不再画）。
3. 今天区后续事项行循环（L1206-1215）：同款守卫——
   `if (fy_today + line_h - 1 > cy1) { break; }`。
   （今天区第 1 项 fy=68 恒容纳，无需守卫；AC5/AC6/AC7 话术均为锚点顶部单行，同理免守卫。）
4. 守卫语义 = break 而非 continue（行流顺序下排，前项放不下后项必放不下）。

## 改动点 2：firmware/main/test/p1_home_battery.acceptance.c（冻结件钉区重锚，须授权链留痕）

1. L247：`hpb_rect_preview_neg` {152,220,392,258} → **{152,244,392,258}**
   （卡底 242 + 2px 缓冲，沿用「合法底界 + 2px」既有纪律；负断言语义从「不压旧预告
   退役带」改为「不越卡区底界」——判别力保持：任何越卡底溢出仍红）。同步更新 L81
   头注区表行、L223 区域注释、L840 / L1292 两处断言消息中的矩形数值。
2. L251：`hpb_rect_tomorrow` {136,142,392,218} → **{136,142,392,242}**（宽域带对齐
   明天区真实足迹；该带仅用于两态帧分叉 / 满载 containment 判读，不钉行位——
   L248-250 注释同步改）。既有用法逐齿推演均仍成立。
3. 文件尾 REVISION LOG **append**（不覆盖既有行）：新增 2026-10-13 条目，授权链 =
   「TaskPackage 261013-p1-tomorrow-row-sliver-fix（human triage 触点① 确认）」，
   记载两钉区重锚的原因（pv_cy1 对齐卡底后明天区第 2 行合法入 220..242 域）与
   判别力保全论证（越卡底溢出仍由重锚后负断言带 + PCN A10 底界外带齿承载）。
4. 建议新增 1 齿（回归钉，防复发）：TEST 4.5 追加 tooth——注入今天 1 项 + preview 2 项
   （或今天空 + preview 2 项），断言明天区第 2 行带 {152,212,392,239} 墨 ≥ 阈值
   （阈值由 acceptance-author 按判别力标定：须满足「1px 残线墨 < 阈值 < 完整行墨」，
   修复前红 / 修复后绿）。加齿与否由 acceptance-author 按 Lite 比例裁量，非硬性。

## 明确零触碰面

- `firmware/main/test/p1_cjk_name.acceptance.c`：零改动（pcn_rect_preview_neg 限定
  标准夹具、preview 单行 184..211，修复后仍零墨；A10 各齿不受影响——已逐齿推演）。
- `firmware/main/display/home_p1.h` 及 schedule 数据链路：零触碰（PH-E1 canary 须绿）。
- `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`：零 diff。

# 验收要点

## host（模拟器/宿主 runner，相关子集）

1. `p1_home_battery.acceptance.c` 全绿（含重锚后的 TEST 1 tooth 4、HV5-AC2 tooth 3、
   及新增回归齿若采纳）。
2. `p1_cjk_name.acceptance.c` 全绿且**零文件改动**——本包最重要的回归观察面
   （冻结件未被本包触碰却红了 = 修复越界）。
3. `schedule_grid.acceptance.c` 全绿（数据半零扰动 canary）。
4. 相对有效基线零新增红。

## 真机（判定标准）

1. 复现场景（今天空 AC5 话术 + 明天 ≥2 项）：明天区第 2 项**完整字形渲染**，
   无 1px 残线——截图与修复前对比留证。
2. 明天 ≥3 项：第 3 项起**整行让位**（干净不出现），不出现半行/残线。
3. 今天 ≥7 项：卡底 y=242 无残线，放不下的项整行让位。
4. 烧录：纯代码变更、无 spiffs 资产变更，仅烧 app 分区即可。

# 回归风险

1. **负断言带重锚的判别力**：220..242 段退役为合法域，卡底 containment 改由
   重锚后 PREVIEW_NEG {152,244,392,258} + PCN A10 底界外带 {y243..251} + 整行守卫
   三重承载——判别力不减（越卡底仍红），已写入 REVISION LOG。
2. **今天区满载视觉行为变化**：第 7 项起从「1px 残线」变「整行消失」——是修复目标
   本身，真机目测确认即可，无冻结断言依赖旧行为。
3. **守卫仅作用行首**：行内换行的续行若恰好跨底界仍可能出残线（如续行落 fy=236、
   文字 242..255 → y=242 一条 1px）——现存夹具与已知真机场景均不触发，登记为已知
   残余（见非目标），不扩本包范围。

# 非目标 / 已知残余（建议登记 TECH CRQ，不扩本包）

1. **明天区锚点随今天区行数下推**：ty = 68 + max(2, s_count)*28，今天 ≥5 项时
   「明天」帽标被推到卡底之下（p1_blit_art 仅 clip 到帧缓冲、不 clip 卡区，会越卡底
   绘制）；今天 8 项时锚点越帧。容量与锚点策略归后续布局包。
2. **换行续行跨底界残线**（见回归风险 3）：彻底解法需行流内部逐行守卫，改动面扩至
   p1_blit_flow 语义，超出 Lite 比例。
3. **明天区容量**：今天空/≤2 项时可容 2 行（184、212），设计线框示 3 行——容量差距
   属布局设计决策，非本 bug 修复范围。

# changeManifest（预定改动面，仓根相对）

- firmware/main/display/home_p1.c
- firmware/main/test/p1_home_battery.acceptance.c

# impactedTests 种子

- firmware/main/test/p1_home_battery.acceptance.c（钉区重锚 + 新增回归齿）
- firmware/main/test/p1_cjk_name.acceptance.c（零改动回归观察面）
- firmware/main/test/schedule_grid.acceptance.c（数据半 canary）
- firmware/main/test/host_app/main/CMakeLists.txt（接线面不动，仅确认在册）
