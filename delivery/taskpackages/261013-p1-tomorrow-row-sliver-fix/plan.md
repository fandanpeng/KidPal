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

---

# 修订 A（2026-10-13，append）：行距压缩与双区容量提升补充轮

> 载体：**并入本包（261013）补充轮**，不开新包——human 裁定（原话「作为上一个包的补充，别再出一个包了，太小了」）。本节为 architect /triage-change 二次 review 修订版 reviewed plan（快审：② Lite / riskTier low / sourceType human-ad-hoc）搬运落盘，按 human 载体改判调整 changeId 引用与授权链口径；human 真机预览定稿留痕见 sibling preview-round.md。

## A0. 背景与目标（human 真机反馈 + 预览轮定稿）

261013 残线修复真机 PASS 后，human 持板目击行间距偏大（line_h=28），两区合计 4 行
偏紧张。目标：压缩行距、每区至少 3 行。human 预览轮定稿：**line_h=22 参数族合意；
锚点保留现状 `max(2, s_count)` 紧凑上浮**（今天区轻载时明天区上浮多显、版式随数据
浮动是选定语义，不取恒定网格）。

## A1. 方案（参数族 + 紧凑上浮锚点）

约束：卡内容域 y68 起、卡底 242。取 L=22、组头开销 H=43（虚线 3 + 帽标 24 原尺寸 + 间隙 16）。

### 改动点 A1：firmware/main/display/home_p1.c `p1_draw_card()`

1. `line_h` 28 → **22**；`text_yoff` 6 → **4**。
2. 锚点 `rows = max(2, s_count)` **保持现状零改**（human 裁定紧凑上浮）。
3. 组间虚线 `ty + 22` → **`ty + 8`**。
4. 「明天」帽标 `ty + 33` → **`ty + 13`**（46x24 资产原尺寸直出；兜底描边 rect 同步
   `151, ty+13, 197, ty+37`）。
5. 明天项行 / AC6 / AC7 话术起点 `ty + 60` → **`ty + 43`**。
6. **新增组级容纳门**：明天区整块（虚线 + 帽标 + 项行/话术三态）包在
   `if (ty + 43 + line_h - 1 <= pv_cy1)` 内（即 ty ≤ 178）——今天 ≥6 项时整区让位，
   不画半截帽标/孤儿虚线；同时结构性关闭「锚点下推越卡」隐患（虚线/帽标 blit 无
   卡区 clip，旧参数下 s_count≥7 越卡绘制）。
7. 整行容纳守卫（261013 主轮落地）语义零改、随新 L 自然平移。函数头注释几何描述
   整体重写（新参数族 + 容量表）。

### 落位几何（推导基准，测试带由此平移）

今天区行带：68-89 / 90-111 / 112-133 / 134-155 / 156-177 / 178-199 / 200-221
（文字 = 带顶 +4..+17，行间净空 8px = 1.57 倍字号）。

明天区（随 ty 浮动）：

| s_count | ty | 虚线 | 帽标 | 明天项行（行带） | 明天容量 |
|---|---|---|---|---|---|
| ≤2（含空/AC5/单项） | 112 | 120-122 | 125-148 | 155-176 / 177-198 / 199-220 / 221-242 | **4 行** |
| 3 | 134 | 142-144 | 147-170 | 177-198 / 199-220 / 221-242 | **3 行** |
| 4 | 156 | 164-166 | 169-192 | 199-220 / 221-242 | 2 行 |
| 5 | 178 | 186-188 | 191-214 | 221-242（带底恰贴卡底） | 1 行 |
| ≥6 | ≥200 | — | — | 组级门整区让位 | 0 |

AC6/AC7 话术落位 = ty+43+4（s≤2 档 159-172）。今天区自身 ≤7 行（第 8 项行带
222-243 越卡底 1px，整行容纳守卫让位）。

几何红利：L=22 自 y=68 锚定后，今天/明天两区文字带结构上不可能跨卡底 242——
261013 残线场景在此参数族下几何不可达，守卫降级为 belt。

### 改动点 A2：firmware/main/test/p1_home_battery.acceptance.c（冻结件重锚批）

钉区常量随公式平移（逐齿推导）：

| 常量 | 旧 | 新 | 依据 / 消费齿 |
|---|---|---|---|
| `hpb_rect_row2` | {152,96,392,122} | **{152,90,392,111}** | 今天第 2 行带（文字 94-107，旧带漏 94-95）；TEST 1 tooth 4 |
| `hpb_rect_mingzao` | {148,154,200,181} | **{148,124,200,149}** | 帽标 125-148（fixture A s=2 → ty=112）；TEST 1 tooth 4 |
| `hpb_rect_rows` | {152,68,392,140} | **{152,68,392,133}** | 今天区 3 行域（旧值 140 会把 s≤2 档虚线 120-122 卷入带内——各 diff 齿虽因虚线恒定仍绿，重钉取语义纯净域）；TEST 1 tooth 5 / TEST 4.5 tooth 2 |
| `hpb_rect_tomorrow` | {136,142,392,242} | **{136,118,392,242}** | 明天区宽域带对齐真实足迹（虚线 120 / 帽标 125-148 / 行 155-242，旧 y0=142 漏虚线与帽标顶）；TEST 4.5 tooth 1-3 |
| `hpb_rect_pv_row1` | {152,184,392,211} | **{152,155,392,176}** | 明天第 1 行带（TR-A4 夹具今天空 → ty=112 → 行 155-176，文字 159-172）；TEST 4.5 tooth 4 参照齿 |
| `hpb_rect_pv_row2` | {152,212,392,239} | **{152,177,392,198}** | 明天第 2 行带；TEST 4.5 tooth 4 |
| `hpb_rect_today_bot` | {152,242,392,242} | **{152,222,392,242}**（语义重锚，见下） | TEST 4.5 tooth 5 |

**保持零改动**（已逐齿验证仍成立）：`hpb_rect_preview_neg` {152,244,392,258}、
`hpb_rect_next` / `label` / `mascot` / `sticker_neg` / `row1` 族 / aux 族。

**齿重锚两处**：

1. TEST 4.5 tooth 4（TR-A4）：重定为**明天区多行容量回归齿**——夹具今天空（AC5
   态）+ preview 3 项，ty=112，行带 155/177/199（第 4 行 221 为设计余量、不断言）。
   断言：第 1 行带 {152,155,392,176} 墨 ≥ HPB_INK_MIN_ELEM（参照齿 F）+ 第 2 行带
   {152,177,392,198} 与**第 3 行带 {152,199,392,220}** 墨均 ≥ 阈值（阈值按
   R < T < F 纪律重标定，R = 带内仅错位/残墨实测）。261013 残线判别面在新几何下
   结构性不可达；mutation 面由 acceptance-author 按「参数族回退（line_h/组头偏移/
   pv_cy1 任一改回旧值）→ 行带错位必红」标定并记 REVISION LOG；残线 belt 由
   preview_neg / containment 齿承接。
2. TEST 4.5 tooth 5（TR-A2）：夹具 7 项 → **8 项**（L=22 下 7 行全容纳、无跨底
   项），负断言带改 {152,222,392,242} 零墨 = 第 8 项（行带 222-243 越卡底 1px）
   整行让位；mutation 面 = 摘守卫 → 第 8 项文字 226-239 入带必红（判别力保持）。

同步更新：头注区表（ROW2/ROWS/MINGZAO/PV 行带行）、TEST 1 tooth 5 与 TEST 4.5 各齿
消息中引用的矩形数值与复现几何描述、coverage map 相应行；文件尾 REVISION LOG
**append** 新条目（授权链 = 「TaskPackage 261013 补充轮（human triage 触点① 确认 +
真机预览定稿裁定 + 载体改判并入本包，见 preview-round.md）」，含逐齿推导表与
mutation 面迁移记载）。

### 改动点 A3：firmware/main/test/p1_cjk_name.acceptance.c（冻结件重锚批，**本文件首次入本包 changeManifest**）

标准夹具 = 2 今天项 + 1 preview（ty=112：帽标 125-148、preview 行 155-176）：

| 常量 | 旧 | 新 | 依据 / 消费齿 |
|---|---|---|---|
| `pcn_rect_row2` | {152,96,392,122} | **{152,90,392,111}** | 今天第 2 行文字 94-107，旧带漏 94-95 必红；TEST 1 name/time diff |
| `pcn_rect_row3` / `pcn_rect_row3_time` | {152,184,392,212} | **{152,155,392,176}** | preview 行文字 159-172，旧带 184 起漏 159-183 必红；TEST 1 preview 齿 / TEST 5 A3（AC7 未达话术同位 159-172）/ TEST 8 tooth 4 |
| `pcn_rect_mingzao` | {148,154,200,181} | **{148,124,200,149}** | 帽标 125-148，旧带 154 起漏顶 125-153 必红；TEST 8 tooth 1 |
| `pcn_rect_below_row1` | {152,96,392,242} | **{152,90,392,242}** | 极宽名续行文字 94-107；TEST 9 tooth 4 |

**保持零改动**（已逐齿验证）：`pcn_rect_row1` {152,68,392,94}（带 68-89 文字
72-85 在内）、`pcn_rect_preview_neg` {152,220,392,242}（PCN 全部夹具 preview 均
单项、行 155-176，零墨仍立）、`pcn_rect_bottom_band` {136,243,392,251}、
`pcn_rect_below_label` {152,68,392,242}、left_gap / card / label / sticker_neg /
flow(unused)。TEST 9 tooth 1 满载（8 今天 + 1 preview）：ty=244 → 组级门整区让位，
右/底/左缝零墨齿不受扰。TEST 2（A2 极宽名单项）：换行落 68-89 / 90-111 两带，
card / below_label 判读域不变。

同步更新：头注 [S3] 布局钉区段几何描述（含「2026-10-13 行距压缩 + 紧凑上浮锚点
保留」重钉注记）；REVISION LOG **append**（授权链同上）。

### 明确零触碰面

- `firmware/main/display/home_p1.h` 与 schedule 数据链路（PH-E1 canary 须绿）。
- `firmware/main/test/schedule_grid.acceptance.c`（数据半、几何无关，零改动回归绿）。
- `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`：零 diff
  （行距/容量非需求钉面；布局优先级「接下来 > 明天区」语义由浮动锚点 + 组级门显式保留）。

## A2. 验收要点

### host（相关子集）

1. `p1_home_battery.acceptance.c` 全绿（含重锚后 TEST 1 tooth 4/5、TEST 4.5 tooth 1-5）。
2. `p1_cjk_name.acceptance.c` 全绿（重锚 4 常量后 TEST 1/2/5/8/9 逐齿过）。
3. `schedule_grid.acceptance.c` 全绿（零改动 canary）。
4. 相对有效基线零新增红；阈值重标定（HPB_INK_MIN_PV_ROW2 族）按 R < T < F 纪律
   实测记载进 REVISION LOG。任何「推导表之外变红」的齿 = 越界信号，停下回评。

### 真机（human 持板判定标准）

1. **容量达成**：今天空/≤2 项 + 明天 ≥3 项 → 明天 3-4 行全显（第 4 行带底恰贴
   卡底 242）；今天 3 项 + 明天 3 项 → 3+3 全显。无残线、无越界。
2. **紧凑上浮语义**（human 选定）：今天空（AC5）与今天 3 项两档的虚线/帽标位置
   不同（120/125 vs 142/147）是预期行为；话术态（AC6/AC7）落 159-172。
3. **行距观感**：22px pitch 已预览轮拍板合意；本项仅复核全量数据态下的整体观感。
4. **布局优先级在世**：今天 4 项 → 明天 2 行；5 项 → 1 行；≥6 项 → 明天区整体
   让位（无半截帽标/孤儿虚线）。
5. 烧录：纯代码变更，仅 app 分区。

## A3. 回归风险 / 非目标

1. **冻结件重锚批量大**（HPB 7 常量 + 2 齿、PCN 4 常量）：逐齿推导表随本节携带，
   acceptance-author / verifier 按表复核。
2. **band 守卫容量边角**：今天第 8 项行带越卡底 1px 被整行让位（文字本可放下
   226-239）——261013 冻结语义保留的有意取舍，登记非目标。
3. **组级容纳门新语义**：今天 ≥6 项时明天区整体消失（含 AC6/AC7 话术）——极端档
   降级路径，无既有齿覆盖，真机判定 4 承载目测确认。
4. **版式随数据浮动回归**：紧凑上浮 = 版式跳动语义回归（human 选定），非缺陷，
   验收时勿误报。
5. 换行续行跨底残线残余（主轮已登记）在 L=22 下仅理论可达；两区文字带结构上不跨
   242，风险进一步降低。

## A4. 补充轮 changeManifest（增量，仓根相对）

- firmware/main/display/home_p1.c
- firmware/main/test/p1_home_battery.acceptance.c
- firmware/main/test/p1_cjk_name.acceptance.c（补充轮新增）

> **对齐注记（architect 补充轮定稿版细化，2026-10-13）**：与本节常量表完全一致，两处细化由 acceptance-author 消费——①改动点 A2 齿重锚 1 的第 3 行带断言可新增独立常量（如 `hpb_rect_pv_row3` {152,199,392,220}）；②改动点 A3 零改动清单中 `pcn_rect_bottom_band` {136,243,392,251} 的成立推导 = 明天末行文字底 238（s≤2 档第 4 行）与今天末行文字底 217 均 < 243；PCN 零改动纪律由本修订节显式解除（仅限 A3 重锚面）。
