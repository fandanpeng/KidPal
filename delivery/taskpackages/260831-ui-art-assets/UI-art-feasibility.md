# UI 美术资产管线全量验证可行性报告 — TaskPackage 260831-ui-art-assets

> 本报告是 V3 设计稿（实现导向）的可行性输入。所有资产经「设计稿 SVG/CSS 唯一母版 → `gen_assets.mjs` 抽取 →
> sharp 真实栅格化 → 阈值化(LUM_THRESHOLD=127) → RGB565」产出，逐元素结论见下。机器断言对齐
> `tests/test_art_assets.acceptance.md`，观感类（造型/接缝/锯齿/字迹/平铺）的最终判断归 human 目测 OQ-3(a)。

## B1 吉祥物 7 姿态 — 可行

7 个 `<symbol>`（viewBox 均 `0 0 200 200`）全部经管线出图，同族同尺寸 **96×96**（0°）。

- 产物：`pose_daily/listen/think/talk/record/sleep/jump_96`，7 枚，逐枚尺寸一致（防换姿态跳位）。
- **口径变更（记录）**：`pose_daily_96` 旧为 98×98(-1°) → 本包中立化为 96×96(0°)。理由：-1° 倾斜是 P1 页面的
  摆放层关注点（CSS `tilt1`），不是姿态族固有属性；把倾斜烘进姿态族会导致「换姿态 + 换倾斜」双重跳位。姿态族
  统一 0°，倾斜回归运行时/页面层。
- 造型达标（摇头耳朵成菱形、五官/胡须/卷尾保真、可爱度）归 human 目测，见 `report/snapshots/poses_row.png`。

## B2 手绘框族 9 宫格 — 可行（含一条 9 宫格忠实性红线）

3 种框型（`.frm` 大框 / `.frm-sm` 小框 / `.frm-flat` 底框）从 CSS 八值 border-radius 构造 SVG 父本 出图 + 9 宫格切分：

- `--hand-r: 255 15 225 15 / 15 225 15 255`；`--hand-r-sm: 125 12 145 14 / 14 135 12 125`（已按 CSS 比例缩放算法收进父本）。
- 产物：整框 RGB565（`frm_236_236`/`frm_sm_160_120`/`frm_flat_240_96`）+ 9 宫格切片（3 框 × 9 片 PNG + 元数据
  JSON，角片 `stretch=false`）。
- 机判已证：拼回原始尺寸逐像素 diff=0（接缝不可见的机判近似）。
- **9 宫格忠实性红线（需 V3 决策）**：`.frm` 的 255px/225px 水平半径是「近乎平直的手绘角」（曲线半径 >> 角片 24px）。
  传统小角片 9 宫格只截取该角曲线的前 24px，会把「平缓手绘角」渲染成「24px 紧弯角」，扭曲手绘不对齐感。两择：
  ① 角片尺寸对齐最大半径（不可能，255px > 框宽）；② **背书「角片 24px 协议」**，接受大半径角被裁成紧弯——
  V3 前需 human 对 `report/snapshots/frm_*_stretch_*.png` 目测裁定；若不可接受，改「框型按固定尺寸整烘」或「拆角片
  档位（大框 40px 角片 / 小框 20px 角片）」。

## B3 元素倾斜烘进资产 — 可行

- 微倾贴纸（±1–2°）：`thermo_man`(-1°)、`drop_baby`(+2°) 已在 P1 既有 8 枚中按 tilt 烘入；管线 `rasterize()` 内
  `rotate()` 先于 `flatten()`，倾斜离线烘进、运行时零成本（OQ-1a 落地）。
- 大倾印章（-8°）：见下方单列段。

## B4 三档网点 d50/d25/d12 — 可行

CSS `data:image/svg+xml`（`<circle cx=2 cy=2 r=1.6/1.13/0.79>`）出 3 枚精确 4×4 平铺贴图。

- 4×4 直接栅格化会因中心采样把 d50/d25 都塌成 2×2、d12 塌成空白；改用**超采样取覆盖面积 + Bayer 4×4 有序抖动**，
  得到忠实网屏占比：d50=56.3% / d25=25.0% / d12=12.5%（理论 50/25/12，容差 ±10% 绝对，写死于 `verify_art_assets.mjs`）。
- 2×2 平铺 8×8 逐 4×4 单元格一致（周期性成立）。

## B5 全图标族 14 枚 — 可行

14 个 `ic-*` `<symbol>` 全出图，13 枚「48 盒」统一 48×48，`ic-wave` 波浪线 48×14 特例（宽高比非 1:1）。

- **取舍记录（离线位图资产 vs FontAwesome/imgfont 图标字）**：**本包路径 = 离线位图资产**。理由：① 14 枚图标是
  1bpp 手绘胖线条风格（4px 圆头笔画），不是现成 FontAwesome 的可复用族；② 图标字（imgfont）适合「同字形逐字符变化」
  （数字/字母），图标是「一次性绘形」，位图直烘零运行时字形引擎成本、无字体子集化负担；③ 与小智备选路线结论一致
  （显示层倾向位图，动态文本才上 imgfont）。唯一 imgfont 例外是 B7 时间数字（真等宽表格线对齐需求）。

## B6 站酷快乐体（ZCOOL KuaiLe）— 可行

- **授权落档**：`tools/asset-pipeline/fonts/OFL-ZCOOLKuaiLe.txt`（SIL OFL 1.1 原文）+ `ZCOOLKuaiLe-Regular.ttf`。
- **免费商用条款逐字引用**（OFL PERMISSION & CONDITIONS）：
  > Permission is hereby granted, free of charge, to any person obtaining a copy of the Font Software, to use, study, copy,
  > merge, embed, modify, redistribute, and sell modified and unmodified copies of the Font Software, subject to the following
  > conditions: … 2) Original or Modified Versions of the Font Software may be bundled, redistributed and/or sold with any
  > software, provided that each copy contains the above copyright notice and this license.
- **可内嵌/可子集化**：上引 "embed" 与 "modify" 明示允许内嵌与修改（子集化属 modify），condition 5 不约束「用字体排版
  的成品」("does not apply to any document created using the Font Software"）。
- **改名禁则**（condition 3，逐字）："No Modified Version of the Font Software may use the Reserved Font Name(s) unless
  explicit written permission is granted by the corresponding Copyright Holder." — 本 OFL.txt 未见显式 Reserved Font Name
  声明行（站酷快乐体 RFN 声明在 google/fonts 的 METADATA.pb 层）；**保守结论：设备端子集化（=修改）后重分发，若未逐字
  核对 METADATA.pb 的 RFN，应改名分发以规避 condition 3**。
- **真体 vs 替身**：本机已按 per-user 安装 ZCOOL KuaiLe（`%LOCALAPPDATA%\Microsoft\Windows\Fonts` + HKCU 注册），
  `gen_assets.mjs` 的 `doFontVerdict()` 机判 **REAL（渲染结果与 Microsoft YaHei 回落字体逐像素不一致 → 真体生效）**。
  样字 `font_zcool_30`（「我的故事 该上课啦」30px 档）非空白。**注意**：设备端仍需真机重烘（FreeType 子集），
  且 30px 1bpp CJK 可读性落在 ARQ-260831-small-cjk-1bpp-readability 风险区，本包只出样不锁下限。

## B7 Baloo 2 动态时间 imgfont — 可行

数字 0–9 + 冒号 11 枚等宽箱体 **48×64**，逐位换字组成动态时间。

- **真体**：Baloo 2 ExtraBold 以 `@font-face` data-uri 内嵌进 SVG，`doFontVerdict()` 机判 **REAL**（与回落字体不一致）。
  `tools/asset-pipeline/fonts/Baloo2-800.ttf`（wght 800 静态实例）+ `Baloo2[wght].ttf`（variable）+ `OFL-Baloo2.txt` 落档。
- **等宽/基线**：数字 0–9 逐枚同宽同高 48×64（strict）；墨迹 bottom-y 一致（基线对齐）；冒号 bbox 垂直居中于数字中线。
- **字体替身坑记录**：librsvg 对 Baloo 的 @font-face data-uri 生效、对 ZCOOL(1.5MB full) 的 data-uri 静默忽略——故 Baloo
  走内嵌 data-uri（不依赖本机），ZCOOL 走 per-user 安装名解析。无论哪条出错都会在 `FONTVERDICT` 被打回替身并记录。
- 冒号用 Baloo 的 `:` 字形（本包）；设计稿 P1 的「两只眨眼眼睛」冒号是独立自定义资产，属后续 V3 范畴（已记录，非缺漏）。

## B8 资产总量预算 — 未超（margin 54.1%，不触发 I1）

见 `report/budget.md`。已出图 47 枚合计 599,698 字节（588 KiB）+ 预估 P2–P4 3,252,096 字节 = 预计总占用 3,851,794 字节，
对 8M(=8,388,608) margin 4,536,814 字节。**结论：不触发 I1**；P4 笔顺字盘是唯一隐愚（保守整帧预估 3.1 MB），V3 前须锁定
「笔形 3 态叠加复用 + 米字格单份背景」打包策略。

---

## -8° 成就印章锯齿结论（单列，不混入 ±1–2° 微倾）

`.stamp-seal`（128×128 双圈 + -8° 斜盖 → 烘出 145×145）在 1bpp 阈值化后**外实内虚双圈结构保全**：机判环形黑像素分布
= 外圈实线 1956 px / 内圈 2.5px 虚线 196 px / 中心文字 1300 px，双圈 + 文字均在场、非空白。

**结论：可接受（对「盖章」语义）。** -8° 大角在 1bpp 下必然产生台阶状锯齿（比 ±1–2° 微倾明显一档），但对一枚
「斜盖的橡皮章」而言，边缘粗糙反而贴合盖章质感，不外圈 4px 实线足够粗、锯齿主要落在圆最外圈，肉眼看是「章边毛边」
而非「断线」。**需 human 目测确认的脆弱点**：内圈是 2.5px 虚线，是全场最细笔画，-8° 后虚线短划有断口/糊连风险；
若 human 认内圈虚线断得难看，**备选**：① 内圈虚线增粗至 3.5px；② 降角至 -5°；③ 若仍不可接受，改「运行时两圈分离
叠加」实现（外圈一个 lv_image、内圈虚线一个 lv_image，分别不旋转只做整体 -8° 偏移）。判定口径 = 阈值化 PNG
`png/stamp_seal_r8.png` 肉眼看锯齿是否可接受（OQ-1a 已裁离线倾斜锯齿可见可控）。

---

## 字体实际使用记录（A8.3 / 任务要求 #6）

| 资产 | 声明字体 | 实际出图字体（机判） | 证据 |
|---|---|---|---|
| `font_zcool_30` 样字 | ZCOOL KuaiLe | **真 ZCOOL KuaiLe**（per-user 安装后按名解析） | FONTVERDICT REAL（≠ Microsoft YaHei 回落）|
| `imgfont_digit_0..9` + `colon` | Baloo 2 ExtraBold | **真 Baloo 2**（@font-face data-uri 内嵌） | FONTVERDICT REAL（≠ Microsoft YaHei 回落）|
| `stamp_seal_r8` 印章文字 | ZCOOL KuaiLe | **真 ZCOOL KuaiLe**（同上） | 与样字同一字体路径 |

无字体落回 Segoe UI 替身；若 pipeline 在未安装 ZCOOL 的机器上重跑，`FONTVERDICT` 会打出 SUBSTITUTE 并无条件记录，
真机重烘仍以 Baloo 2 ExtraBold / ZCOOL KuaiLe 子集为准。

## 资产与出样落位

- 全量资产：`tools/simulator/LvglWindowsSimulator/assets/{assets.c, assets.h, png/*.png, 9slice/*}`（47 枚 RGB565 + 3 框
  9 宫格切片）。
- 阈值化出样截图（供目测）：`tools/asset-pipeline/report/snapshots/`（7 姿态拼版 / 14 图标拼版 / imgfont 连排 / 框族+
  印章+网点拼版 / 3 框 9 宫格拉伸至 2 种尺寸×3=6 张）。
- 三档网点出样：`png/d50.png` `d25.png` `d12.png`（4×4）。