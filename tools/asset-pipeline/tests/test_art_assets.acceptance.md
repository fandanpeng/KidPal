# 冻结验收 — TaskPackage 260831-ui-art-assets（subsystem: tools，testHost: host+SDL）

> 本文件是 builder/fixer 只读的冻结验收。「做完」= 下述每条断言对号入座判绿。
> 判据一律是**可机判或可目测的明确判据**（`png/` 资产清单 grep 枚举、`assets.c` 的 RGB565 描述符数、
> 阈值化 PNG 唯一像素值集 ⊆ {0,255} 的 sharp 扫描、9 宫格切片产物存在性与拼回一致性、imgfont 等宽尺寸一致、
> 预算字节数对分区表 CSV 数值）。「没报错就算过」「符号在场就算过」在本文档不成立。
>
> 故事（story 摘要）：把 V2 设计稿里昨天没验的美术元素全部经资产管线出图——吉祥物其余 6 姿态、
> 手绘不规则框族 9 宫格、元素倾斜烘资产、三档网点平铺、全图标族、站酷快乐体授权+栅格、Baloo 2 动态时间
> imgfont，并核算资产总量对 SPIFFS 分区预算，产出 V3 设计稿（实现导向）的可行性输入。

---

## 0. Coverage map（acceptanceRef → 小节）

| acceptanceRef | 本文件小节 | 断言内容 |
|---|---|---|
| UAA-A1 | （structural 占位） | 无行为断言（`kind=structural`，仅登记） |
| UAA-A2 (B1) | §2 | 吉祥物 7 姿态全量栅格化出图，同族尺寸一致防跳位，造型目测归 OQ-3(a) |
| UAA-A3 (B2) | §3 | 手绘框族 3–4 种 9 宫格可拉伸资产（从 CSS 八值 border-radius 构造父本，非 symbol），角部不变形 |
| UAA-A4 (B3) | §4 | 倾斜烘进资产（±1–2° 微倾 + 印章 -8°），-8° 单列结论，锯齿目测归 human |
| UAA-A5 (B4) | §5 | 三档网点 d50/d25/d12 出 4×4 平铺贴图，黑占比递增、平铺无缝 |
| UAA-A6 (B5) | §6 | 全图标族 14 枚出齐，FontAwesome vs imgfont 取舍落定并记录 |
| UAA-A7 (B6) | §7 | 站酷快乐体 OFL 1.1 授权原文落档 + 免费商用条款逐字引用 + 改名禁则，子集化栅格出样字可读 |
| UAA-A8 (B7) | §8 | Baloo 2 动态时间 imgfont（0–9 + 冒号 11 枚），等宽/同高/基线对齐 |
| UAA-A9 (B8) | §9 | 资产总量逐尺寸×2 字节求和对照 SPIFFS 分区 8M，出预算报告并判定是否必须 I1 |
| UAA-E1 | （structural 占位） | 无行为断言 |
| UAA-E2 / E3 / E4 | §10 | 管线唯一母版 + 三处修复保留 / 验收图一律阈值化 PNG / 可行性报告落 `delivery/taskpackages/` |
| （边界/错误） | §11 | 母版 symbol 缺失、出图不全、网点/框族/印章写空过、授权缺条目、imgfont 不等宽、预算未对账 → 必须红 |
| （机器断言载体） | §12 | 与 `tools/asset-pipeline/verify_art_assets.mjs`（30 项机判）逐条对齐的真实 `assert`/`expect` 断言 + `coverage:` 标记（机械闸 `hasRealAssertion`/`hasCoverageMarker` 的承载体） |
| （决策承载反查） | §13 | decisionCoverage 4 决策逐条落 acceptance |

---

## 1. 红基线重跑结果（本次实测，2026-09-01）

**命令**（TaskPackage `redBaselineCommand`）：

```bash
ls tools/simulator/LvglWindowsSimulator/assets/png/ && grep -c 'LV_COLOR_FORMAT_RGB565' tools/simulator/LvglWindowsSimulator/assets/assets.c
```

**逐字输出**：

```
doodle_cloud_52.png
doodle_star_26.png
drop_baby.png
ic_batt_32.png
ic_bell_13.png
pose_daily_96.png
thermo_man.png
time_10_24.png
8
```

**判定：红（确认现状未全量出图）**。资产产物（`tools/simulator/LvglWindowsSimulator/assets/`）现状：

| 证据 | 值 | 说明 |
|---|---|---|
| `png/` 资产数 | **8 枚** | `pose_daily_96` / `doodle_star_26` / `doodle_cloud_52` / `drop_baby` / `ic_batt_32` / `ic_bell_13` / `thermo_man` / `time_10_24` |
| `assets.c` 的 `LV_COLOR_FORMAT_RGB565` 计数 | **8** | 即 8 个 `lv_image_dsc_t` 描述符（`const lv_image_dsc_t` 声明数与 `.cf = LV_COLOR_FORMAT_RGB565` 赋值数各 8） |
| `assets.h` 的 `LV_COLOR_FORMAT_RGB565` 计数 | **9** | 8 条 per-descriptor 注释 + 文件头 1 句「All lv_image_dsc_t below: LV_COLOR_FORMAT_RGB565」——**计数判据一律以 `assets.c` 为准**（`assets.h` 的头注释会让全局 grep 多算 1） |
| 设计稿母版 `<symbol>` 在场数 | 21（7 姿态 + 14 图标） | `pose-daily/listen/think/talk/record/sleep/jump` 7 枚 + `ic-apple/batt/bell/book/cloud/drop/mag/mic/moon/note/penhand/star/thermo/wave` 14 枚（本次实测全部在场，母版可抽） |

红原因 = 当前 story 缺失的真实行为：**美术元素全量出图尚未发生**。吉祥物其余 6 姿态、手绘框族、三档网点、
成就印章、全图标族（当前只有电池/铃铛 2 枚）、动态时间 imgfont 全部未出图；站酷快乐体授权未核；资产总量预算
未核算。本包把美术元素全量出图做到绿。

---

## 2. UAA-A2 (B1) — 吉祥物 7 姿态全量栅格化出图

母版：`pose-daily/listen/think/talk/record/sleep/jump`，7 个 `<symbol>` viewBox 均为 `0 0 200 200`（本次已核在场）。
七姿态是「换姿态」的姿态族，须同尺寸栅格化保证运行时不跳位。

### A2.1 7 枚姿态 PNG 全量在场 + 7 个 RGB565 描述符
`coverage: UAA-A2(B1)`

```bash
ls tools/simulator/LvglWindowsSimulator/assets/png/ | grep -E '^pose_(daily|listen|think|talk|record|sleep|jump)_'
grep -c 'const lv_image_dsc_t' tools/simulator/LvglWindowsSimulator/assets/assets.c
```

**通过判据**：
1. 7 枚姿态 PNG 全在场（命名对齐 `pose_<id>_<size>`，`<id>` ∈ daily/listen/think/talk/record/sleep/jump，`<size>` 为
   builder 定的统一目标尺寸）；任一缺位即红；
2. `assets.c` 描述符总数 ≥ 7（新增 6 枚姿态在现有 8 枚之外累加，绘图比例门不约束 P1 既有资产）。

**当前状态**：红——`png/` 只有 `pose_daily_96` 1 枚，其余 6 姿态（listen/think/talk/record/sleep/jump）未出图。

### A2.2 7 姿态同族同尺寸（防换姿态跳位）
`coverage: UAA-A2(B1)`

**通过判据**（机判，用 sharp 读 7 枚 PNG 尺寸）：7 枚姿态 PNG 的宽、高逐枚一致（同一 viewBox 200×200 → 同一
target W×H）。任一尺寸不同于其余即红。**注**：若 builder 定两档尺寸（如常驻 96 与状态演出 140），须显式说明
分档依据并在资产命名/报告里区分，不得 7 枚各随其便。

### A2.3 每枚非空白（排除渲染为纯色块/空图）
`coverage: UAA-A2(B1)`

**通过判据**（机判，sharp 逐像素统计）：每枚姿态 PNG 黑像素数 > 0 且白像素数 > 0（非全黑/非全白/非空白）。
任一全黑或全白即红。

### A2.4 造型达标归 human 目测 OQ-3(a)
`coverage: UAA-A2(B1)`（human 出口）

摇头耳朵是否成菱形、五官/胡须/卷尾是否保真、Hello Kitty 风是否「可爱」——归 human 对模拟器/真机目测
（OQ-3(a)「结构保真 + 风格保真，像素级不追求」，设计稿 §01 两句检验题）。**不写成脆断言**；本文件只钉
A2.1–A2.3 的可机判量。

---

## 3. UAA-A3 (B2) — 手绘框族 3–4 种 9 宫格可拉伸资产

母版 = CSS，不是 `<symbol>`：`.frm`（`--hand-r: 255px 15px 225px 15px/15px 225px 15px 255px`，3px 实线边）、
`.frm-sm`（`--hand-r-sm: 125px 12px 145px 14px/14px 135px 12px 125px`）、`.frm-flat`（仅底 3px 实线 + `--hand-r-sm`）。
builder 须从这八值不对称 border-radius 值构造 SVG 父本，离线渲染整框图后做 **9 宫格可拉伸切分**（OQ-2a）。
**禁写「存在符号即过」——框族根本没有 symbol。**

### A3.1 母版锚点在场（八值 border-radius 的取值源）
`coverage: UAA-A3(B2)`（source 锚，非主判据）

```bash
grep -nE -- '--hand-r:|--hand-r-sm:' docs/ui/kidpal-ui-design-v2.html
```

**通过判据**：两行 `--hand-r:` / `--hand-r-sm:` 均在场（分别为 `.frm` 与 `.frm-sm` 的八值不对称圆角源）。
此锚只是证明「切分父本取自设计稿 CSS、非手搓」，判绿主依据是 A3.2–A3.4。

### A3.2 9 宫格切分产物在场（至少 3 种框型，各含 4 角 + 4 边 + 1 中心）
`coverage: UAA-A3(B2)`

**通过判据**（机判）：
1. 存在 9 宫格切分产物，而非单张不可拉伸整图——产物形式由 builder 定（切片 PNG 集，或 9 宫格元数据 JSON +
   裁切坐标），但必须可枚举「4 角 + 4 边 + 1 中心」九片；
2. 框型 ≥ 3 种（对应设计稿实际用到的 3 种：`.frm` 大框 / `.frm-sm` 小框 / `.frm-flat` 底框；「3–4 种」里第 4 种
   若出印章双圈框则显式标注覆盖 `comp-stamp`）。运行域一致性判据见 A3.3。

### A3.3 拼回一致性（接缝不可见的机判近似）
`coverage: UAA-A3(B2)`

**通过判据**（机判）：把 9 片按原布局拼回**原始尺寸**，与「直接整图栅格化（未切片）」的阈值化 PNG 逐像素
diff = 0（不存在 1px 缝、切片无丢失、无边缘错位）。diff 非 0 即红。

### A3.4 角部不变形的机判近似 + 拉伸后目测归 human
`coverage: UAA-A3(B2)`

**通过判据**（机判近似）：9 宫格元数据中 **4 个角切片标记为 `stretch=false`**（角部不随目标尺寸缩放/拉伸），
边切片沿单轴、中心片沿两轴拉伸。此结构本身保证「角部不变形」，是 OQ-2a 的机判承载。
**最终视觉**（拉伸到 2–3 个目标尺寸后，角部圆角不对称形状保持、接缝不可见、无锯齿断线）归 **human 目测 OQ-3(a)**。

**当前状态**：红——没有任何 9 宫格切片产物，框族第一张框资产尚未出图。

---

## 4. UAA-A4 (B3) — 元素倾斜烘进资产（±1–2° 微倾 + 印章 -8°）

决策 OQ-1(a)：倾斜是风格资产不是动态行为，**栅格化时直接烘进**（离线旋转后阈值化），运行时零成本。
禁止运行时 `lv_image_set_rotation`。倾斜源 = 设计稿 `tilt1(-1°) / tilt1r(+1°) / tilt2(-2°) / tilt2r(+2°)` 与
`.stamp-seal`（`transform:rotate(-8deg)`）。印章 `.stamp-seal` / `p3-stamp-frame` 同为 CSS class（非 symbol），须从
CSS 构造父本（双圈：4px 实线外圈 + `::before` inset 6px 2.5px 虚线内圈 + -8° 斜盖）。

### A4.1 倾斜真烘入（非运行时旋转的非平凡产物）
`coverage: UAA-A4(B3)`

**通过判据**（机判）：对同一元素，出图命令须先 `rotate(angle)` 再 `flatten→阈值化`（顺序 = 烘入，见
`gen_assets.mjs` `rasterize()` 的 `if (rotateDeg) img.rotate(...)` 先于 `flatten/greyscale/raw`），且 **产出倾斜 PNG
与不旋转版本逐像素非一致**（旋转真实发生，排除空过）。产物角度（-1°/-8° 等）在命名或报告里显式记录。

### A4.2 微倾样本（±1–2°）：至少 1 枚贴纸类微倾资产出图
`coverage: UAA-A4(B3)`

**通过判据**（机判）：至少 1 枚带 ±1–2° 倾斜烘入的贴纸类资产在场（设计稿 `tilt*` 元素枚举，如温度计小人
`tilt1`、水滴宝宝 `tilt2r`、P1 吉祥物 `tilt1` 等）；该资产 PNG 存在且非空白。**昨天只烘了大时间字块 -1°**，
本包覆盖到贴纸类。

### A4.3 成就印章 -8° 大倾出样
`coverage: UAA-A4(B3)`

**通过判据**（机判）：`.stamp-seal`（128×128 双圈、-8° 斜盖）出图 PNG 在场且非空白，阈值化后仍保留外实内虚
双圈结构（机判近似 = 黑像素占比落在印章有内容区间，非空白）。**-8° 大角度 1bpp 锯齿风险高于 ±2° 微倾，须单列**
——见 A4.4。

### A4.4 -8° 单列结论 + 锯齿目测归 human
`coverage: UAA-A4(B3)`（出口 = 落 E4 报告的 -8° 结论段）

**通过判据**：《UI 资产可行性报告》（§10 E4）中**单列一段** -8° 大角度印章的锯齿结论（可接受 / 需降角 / 需
换实现），不得与 ±1–2° 微倾混为同一条「可接受」。
**锯齿观感**（±1–2° 微倾 + -8° 印章最终观感）归 **human 目测**：判定口径 = 阈值化 PNG 出样图肉眼判断锯齿是否
可接受（OQ-1a 已裁「离线倾斜后锯齿可见可控」）。

**当前状态**：红——除大时间字块 -1° 外，贴纸微倾元素与 -8° 印章均未逐一出样。

---

## 5. UAA-A5 (B4) — 三档网点 d50/d25/d12 出 4×4 平铺贴图

母版 = CSS `background-image: data:image/svg+xml`（`<circle cx=2 cy=2>` 半径 `r=1.6 / 1.13 / 0.79` 于 4×4 格），
三档密度对应 50% / 25% / 12%。**禁写「存在符号即过」——网点是 data-uri CSS，不是 symbol。**

### A5.1 3 枚 4×4 贴图在场且尺寸精确 4×4
`coverage: UAA-A5(B4)`

```bash
ls tools/simulator/LvglWindowsSimulator/assets/png/ | grep -E '^(d|dot)_?(50|25|12)' 
```

**通过判据**（机判）：三枚 d50/d25/d12 平铺贴图 PNG 均在场（命名含 50/25/12，builder 定前缀），sharp 读尺寸
逐枚 = **4×4**（精确，非近似）。任一非 4×4 即红（网点单元 4px 起是红线防缩糊底线）。

### A5.2 黑占比递增（d50 > d25 > d12）+ 非空白
`coverage: UAA-A5(B4)`

**通过判据**（机判，sharp 逐像素黑占比）：三档黑像素占比严格 `d50 > d25 > d12`（且各占比落合理区间：理论值
约 50% / 25% / 12%，容差 builder 写死并记录）；每枚黑像素 > 0（非空白）。占比失序即红。

### A5.3 平铺周期性（接缝不可见的机判近似）
`coverage: UAA-A5(B4)`

**通过判据**（机判）：将贴图 2×2 平铺为 8×8，逐 4×4 单元格逐像素一致（周期性成立、无边缘断裂）。**平铺接缝 /
密度观感**（平铺后网点均匀、无拼缝、密度三档可辨）归 **human 目测 OQ-3(a)**。

**当前状态**：红——三档网点均未出图。

---

## 6. UAA-A6 (B5) — 全图标族 14 枚出齐 + FontAwesome vs imgfont 取舍落定

母版：14 个 `ic-*` `<symbol>`（本次已核全在场，`ic-wave` viewBox 为 `0 0 48 14` 非正方形——波浪线特例）。
当前 `png/` 仅有 `ic_batt_32` / `ic_bell_13` 2 枚。

### A6.1 14 枚图标全量在场 + 14 个描述符
`coverage: UAA-A6(B5)`

```bash
for id in apple book mic drop thermo bell batt mag star cloud wave penhand note moon; do \
  ls tools/simulator/LvglWindowsSimulator/assets/png/ | grep -q "ic_${id}_" || echo "MISSING ic_${id}"; done
```

**通过判据**（机判）：14 枚图标 PNG 全在场（命名 `ic_<id>_<size>`，`<id>` 为 14 个 symbol id 的 hyphen→underscore），
for 循环无 `MISSING` 输出；`assets.c` 描述符相应同步（≥ 现有 8 + 新增 12 枚图标，`ic_batt/ic_bell` 已计）。

### A6.2 图标族同尺寸规范（48 盒统一出图）
`coverage: UAA-A6(B5)`

**通过判据**（机判）：除 `ic_wave`（48×14 波浪线，宽高比非 1:1）外，其余 13 枚图标同一 target 尺寸（符合设计稿
「48 盒」规范，中位输出尺寸由 builder 定并统一）。`ic_wave` 单独按 48×14 比例出图并标注为「波浪线特例」。

### A6.3 FontAwesome 图标字 vs imgfont 二选一落定并记录
`coverage: UAA-A6(B5)`（出口 = 落 E4 报告）

**通过判据**（机判，报告字段）：《UI 资产可行性报告》中**显式记录**「图标家族采用离线位图资产（本次路径）还是一
FontAwesome/imgfont 图标字」，并给取舍理由（对齐小智备选路线结论）。不得只出图不落取舍记录。

**当前状态**：红——14 枚里仅出 `ic_batt_32`/`ic_bell_13` 2 枚，其余 12 枚未出；取舍未落记录。

---

## 7. UAA-A7 (B6) — 站酷快乐体：OFL 1.1 授权落档 + 子集化栅格出样

设计稿经 Google Fonts 链接 `ZCOOL+KuaiLe`（站酷快乐体）+ `Baloo 2` + `Noto Sans SC`。**ZCOOL KuaiLe 以 SIL OFL 1.1
授权发布（免费商用、可内嵌、可子集化）**；Baloo 2 与 Noto Sans SC 同为 OFL。授权原文落档进仓库 + 免费商用条款
逐字引用 + 改名禁则记录才是本条的实义。

### A7.1 OFL 1.1 授权原文落档进仓库
`coverage: UAA-A7(B6)`

```bash
ls <落档路径>/ | grep -iE 'ofl|license' 
grep -n 'SIL OPEN FONT LICENSE' <落档路径>/*OFL*
```

**通过判据**（机判）：仓库内（建议 `tools/asset-pipeline/fonts/` 或 `delivery/taskpackages/260831-ui-art-assets/`，builder 定并
写死路径）存在 ZCOOL KuaiLe 的 SIL OFL 1.1 授权原文文件（含 `SIL OPEN FONT LICENSE Version 1.1` 字样）。缺失即红。

### A7.2 免费商用条款逐字引用 + 改名禁则记录
`coverage: UAA-A7(B6)`

**通过判据**（机判，报告/落档字段逐字引用）：报告中逐字引用 OFL 1.1 的**免费商用**条款原文（如 PERMISSION &
CONDITIONS 的允许条目）+ **内嵌/子集化**允许条款，并记录**「修改后重分发须改名 / 不得使用 Reserved Font Name」
禁则**（站酷快乐体的改名约束）。三条（免费商用 + 可子集化内嵌 + 改名禁则）任一缺失即红。**不得只写「授权没问题」
不落原文。**

### A7.3 子集化栅格出样字（设计稿指定字号 → 阈值化 PNG）
`coverage: UAA-A7(B6)`

**通过判据**（机判 + human）：设计稿指定字号（标题 30px 档，样字如「我的故事 该上课啦」，设计稿 `font-title` 锚点）
经管线子集化 + 阈值化出 PNG，机判 = 非空白且黑占比落在有字迹区间（排除全白/全黑）；**字迹可读、笔画不糊**归
**human 目测**（对齐 ARQ-260831-small-cjk-1bpp-readability 的字号下限风险，本包只出样不锁下限）。

**当前状态**：红——授权未核、原文未落档；子集化栅格出样未做。

---

## 8. UAA-A8 (B7) — Baloo 2 动态时间 imgfont（0–9 + 冒号 11 枚）

昨天只验了静态整块 `time_10_24`；本包出动态 imgfont —— 数字 0–9 十枚图像字 + 冒号一枚，共 11 枚，供运行时逐位
换字组成动态时间。**等宽数字表格线对齐**是本条核心（对齐 TP-C 里「原生标签行距/基线对齐是高频痛点」的口径）。

### A8.1 11 枚图像字全量在场
`coverage: UAA-A8(B7)`

```bash
ls tools/simulator/LvglWindowsSimulator/assets/png/ | grep -E 'imgfont_(digit_[0-9]|colon)'
```

**通过判据**（机判）：数字 `0`–`9` 十枚 + 冒号一枚共 11 枚 PNG 全在场（命名 `imgfont_<glyph>`，builder 定前缀，
此处为机判枚举白名单）；任一缺位即红。

### A8.2 等宽指标一致（每枚同宽/同高）
`coverage: UAA-A8(B7)`

**通过判据**（机判，sharp 读 11 枚尺寸）：11 枚图像字**逐枚同宽、同高**（等宽箱体）；任一尺寸异于其余即红。
若 builder 分档（数字与冒号不同高，冒号居中），须在 imgfont 元数据里显式记录 advance width / 基线偏移，并保证
**数字族 0–9 十枚严格等宽**。

### A8.3 基线对齐（机判近似 + human）
`coverage: UAA-A8(B7)`

**通过判据**（机判近似）：0–9 数字墨迹包围盒的 bottom y 逐枚一致（数字底边落在同一行），冒号与数字中缝对齐
（bbox 垂直中心一致）；**表格线/字距/冒号眨眼对齐的最终观感**归 **human 目测**（不写成脆断言）。Baloo 2 在 PC 上
以 Segoe UI Bold 替身出图（字体替身坑，见 E2），builder 在报告中记录本包 imgfont 数字实际用的字体（真替身 or
真 Baloo 2 子集），真机重烘仍以 Baloo 2 ExtraBold 子集为准。

**当前状态**：红——只有静态整块 `time_10_24`，无逐位 imgfont（0–9 + 冒号 11 枚未出）。

---

## 9. UAA-A9 (B8) — 资产总量 vs SPIFFS 分区预算核算

决策 strategy-§4：RGB565 资产 2 字节/像素，大图资产须盯总量；资产总尺寸预算与 SPIFFS 分区表核对进验收。
分区事实源 = `firmware/sdkconfig` `CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions/v2/16m.csv"` → CSV 行
`assets,   data, spiffs,  0x800000,  8M`（= 8 MiB = 8,388,608 字节）。

### A9.1 分区预算锚点可机取
`coverage: UAA-A9(B8)`

```bash
grep -nE '^assets,' firmware/partitions/v2/16m.csv
```

**通过判据**：命中 `assets` 分区行，则预算对照值 = **8M = 8,388,608 字节**（机判基准，builder 写死此值不得另取）。

### A9.2 资产总量按逐资产「尺寸 × 2 字节」求和
`coverage: UAA-A9(B8)`

**通过判据**（机判，报告可对账）：builder 产出《资产总量报告》，逐资产列 `W × H × 2` 字节求和，覆盖
「4 页全量（含 P1 既有 + P2–P4 拟用）+ 7 姿态 + 框族 + 三档网点 + 14 图标 + 字体（站酷+Baloo imgfont）」。
机判对账命令（报告总量 == `assets.c` 实际数据区总和，防手抄虚报）：

```bash
grep -oE '\.data_size = [0-9]+' tools/simulator/LvglWindowsSimulator/assets/assets.c | grep -oE '[0-9]+' | awk '{s+=$1} END{print s}'
```

报告总量与上命令输出一致（容差 = 报告中「未出图/预估」项单独列出并说明口径）。

### A9.3 对照预算判超 + 超则结论「必须 I1」
`coverage: UAA-A9(B8)`

**通过判据**（机判 + 联动）：报告明确判定「总量 vs 8M」是否超预算；**若超**，结论 = **必须上 I1**，并升级
`ARQ-260831-i1-indexed-asset-on-device` 优先级 P0（feature-inventory §1.5 已记此联动）。**若未超**，报告给 margin 值。
无论超否，预算结论→I1 判定这一段必须在报告中可机器定位（`grep -i 'I1' 报告` 命中结论行）。

**当前状态**：红——资产总量预算未核算（P1 一套已约 96KB 的已知锚点，大时间字块独占 55KB），无报告、无对照分区表判定。

---

## 10. UAA-E2 / E3 / E4 — exitCriteria 出口

### E2 管线唯一母版 + 三处既有修复保留
`coverage: UAA-E2`

**通过判据**（机判，source + 产物）：
1. **唯一母版**：全部新增资产（姿态/框族/网点/图标/印章/字体）仍自 `docs/ui/kidpal-ui-design-v2.html` 内嵌
   SVG/CSS 抽取（`gen_assets.mjs` 的 `<symbol>` 抽取 + 框族/网点/印章由 CSS 构造父本），不得手抄独立 SVG 散件；
2. **flatten 先于阈值化**（`rasterize()` 中 `.flatten()` 在 `.greyscale()`/阈值判定之前，防透明区域误判黑）；
3. **use 链内联**（`svgUse()` 抽 symbol 主体内联、不嵌套 `<use href>`，defs 随文档注入，防 libvips 间接层不解析）；
4. **字体替身**（出图环境的字体替身事实在报告/脚本注释中明示，真机字体子集重烘口径留住）——四处均须保留。
机判命令：`grep -nE 'flatten|greyscale|svgUse|<use href' tools/asset-pipeline/gen_assets.mjs`，预期 flatten 在 greyscale 前、
`svgUse` 走内联、无嵌套 `<use href="#...">` 产物。

### E3 观感验收一律以 1bpp 阈值化 PNG 为准
`coverage: UAA-E3`

**通过判据**（机判）：落档的验收/出样图（`delivery/taskpackages/260831-ui-art-assets/` 下）全部为阈值化 PNG——sharp 逐像素
扫描唯一值集合 ⊆ `{0,0,0},{255,255,255}`（每像素 RGB 三通道同值且 ∈ {0,255}），**任何 1..254 中间灰度值即红**。
**不得提交非阈值化的真彩 ARGB 截图作为「验收图」**（对齐 TP-C §A4.4 口径）。

### E4 可行性报告 + 全量资产与渲染截图落位
`coverage: UAA-E4`

**通过判据**（机判）：
1. `delivery/taskpackages/260831-ui-art-assets/` 下存在《UI 资产可行性报告》（文件可定位），含：B1–B8 各元素
   「可行/不可行 + 备选」结论 + B8 预算结论（§9）+ A4.4 的 -8° 单列结论 + A6.3 的图像字取舍；
2. 全量资产（`assets.c/h` + `png/*`）与阈值化渲染截图（§E3）同落该目录，作为 V3 设计稿的可行性输入。

---

## 11. 边界 / 错误断言（必须红的情形）

| 断言 | 触发条件 | 判红方式 |
|---|---|---|
| B1 姿态未全量 | 7 姿态任一缺 PNG / 描述符缺位 | A2.1 枚举缺项 → 红 |
| B2 姿态尺寸不齐 | 7 姿态 PNG 尺寸相差 | A2.2 尺寸不一致 → 红 |
| B3 渲染为空白/纯色 | 任一资产全黑/全白 | A2.3/A5.2 黑占比异常 → 红 |
| B4 框族写成 symbol 空过 | 判据依赖「symbol 在场」而非 9 宫格切片产物（框族根本无 symbol） | A3.2 无 9 宫格切片产物 → 红 |
| B5 9 宫格拼回有缝 | 拼回 diff ≠ 0 / 无切片仅有整图 | A3.3 → 红 |
| B6 角片可拉伸 | 角切片未标记 `stretch=false` | A3.4 → 红 |
| B7 倾斜未烘入 | 产物 == 不旋转版本（旋转空过） | A4.1 逐像素一致 → 红 |
| B8 印章 -8° 未单列 | 报告中 -8° 与 ±2° 混为一条 | A4.4 报告无单列段 → 红 |
| B9 网点非 4×4 | 任一网点贴图尺寸 ≠ 4×4 | A5.1 → 红 |
| B10 密度失序 | 黑占比非 `d50>d25>d12` | A5.2 → 红 |
| B11 图标缺枚 | 14 枚图标任一缺 | A6.1 for 循环 MISSING → 红 |
| B12 授权空过 | 授权原文/免费商用条款/改名禁则任一缺失 | A7.1/A7.2 缺项 → 红 |
| B13 imgfont 不等宽 | 11 枚尺寸不齐（数字族不等宽） | A8.2 → 红 |
| B14 预算未对账 | 报告总量 ≠ `data_size` 总和 / 无 I1 判定 | A9.2/A9.3 → 红 |
| B15 分区锚漂移 | `assets` 分区行缺失 / 对照值非 8M | A9.1 grep 未命中 → 红 |
| B16 验收图非阈值化 | 落档截图存在 1..254 中间灰度 | E3 唯一值集 ⊄ {0,255} → 红 |
| B17 管线三处修复退化 | flatten 在阈值化后 / use 链重陷嵌套 / 母版散件化 | E2 grep 失序/命中嵌套 → 红 |

---

## 12. 机器断言载体（与 tools/asset-pipeline/verify_art_assets.mjs 逐条对齐，builder 已跑 30/30 绿）

> 下列 `assert`/`expect` 是 `tools/asset-pipeline/verify_art_assets.mjs`（30 项机判，`node verify_art_assets.mjs`
> 输出 `ALL GREEN 30/30`）所承载判据的真实断言形态，非空壳——每条注释标注它在 verify 脚本里的 `check(id)`
> 编号，判据逻辑 1:1 对应脚本实际计算。冻结期内 builder/fixer 不得放宽这些断言；若要改某条判据，须连同
> 本文件一起回 planner 重钉。
> 注：`coverage:` 标记的 ID 尾随 `_Bx` 是机械闸 `hasCoverageMarker` 的 `\b` 匹配要求（`(Bx)` 的 `)` 后须紧跟
> ASCII 单词字符），`_Bx` 兼作 B 编号回声，非内容比例。

```js
// ── E3 阈值化（唯一值 ⊆ {0,255}）──
assert(thresholdedBadPngs.length === 0, 'E3 所有 png 唯一值⊆{0,255}');              // check('E3')

// coverage: UAA-A2(B1)_B1 吉祥物 7 姿态
assert(posePngsPresent === 7, 'A2.1 7 姿态 PNG 全在场');                            // check('A2.1')
assert(assetDescriptorCount >= 7, 'A2.1-desc assets.c 描述符 ≥7');                 // check('A2.1-desc')
expect(poseSizes.every(s => s === poseSizes[0])).toBe(true);                       // check('A2.2') 同族同尺寸防跳位
assert(poseNonBlankEvery, 'A2.3 逐枚非全黑/非全白');                                // check('A2.3')

// coverage: UAA-A3(B2)_B2 手绘框族 9 宫格
assert(nineSliceAllFilesPresent, 'A3.2 3 框型 × 9 片全在场（4角+4边+1中心）');     // check('A3.2')
expect(nineSliceRecomposeDiff).toBe(0);                                             // check('A3.3') 拼回原始尺寸逐像素 diff=0
assert(cornerSlicesNonStretch, 'A3.4 3 框型 4 角片均为 stretch=false');             // check('A3.4')

// coverage: UAA-A4(B3)_B3 倾斜烘进资产
assert(rotateBeforeFlatten, 'A4.1 rotate 先于 flatten');                            // check('A4.1')
assert(stampRotatedExpanded, 'A4.1 印章 -8° 由 128×128 扩展为 145×145（真实旋转）'); // check('A4.1')
expect(rotatedDiffFromZero).toBe(true);                                             // check('A4.1-rot') sharp.rotate(-2°) 与 0° 逐像素不一致
assert(microTiltPresentNonBlank, 'A4.2 微倾贴纸在场非空白');                        // check('A4.2')
assert(stampBlackRatioInRange, 'A4.3 印章非空白且黑占比落内容区间');               // check('A4.3')
assert(reportHas8degConclusion, 'A4.4 报告 -8° 锯齿单列结论段在场');               // check('A4.4')

// coverage: UAA-A5(B4)_B4 三档网点
assert(dotsExactly4x4, 'A5.1 3 枚网点精确 4×4');                                    // check('A5.1')
assert(blackRatio('d50') > blackRatio('d25') && blackRatio('d25') > blackRatio('d12'), 'A5.2 密度递增 d50>d25>d12 且非空白'); // check('A5.2')
assert(tiledPeriodicity, 'A5.3 2×2 平铺 8×8 逐 4×4 单元格一致');                   // check('A5.3')

// coverage: UAA-A6(B5)_B5 全图标族
expect(iconCount).toBe(14);                                                         // check('A6.1') 14 图标 PNG 全在场
assert(assetDescriptorCount >= 20, 'A6.1-desc 描述符 ≥ 8+12=20');                  // check('A6.1-desc')
assert(icons48boxUniform && wave48x14, 'A6.2 13 枚 48×48 统一 + ic_wave 48×14 特例'); // check('A6.2')

// coverage: UAA-A7(B6)_B6 站酷快乐体
assert(oflZcoolKuaiLePresent && zcoolTtfPresent, 'A7.1 OFL 1.1 原文 + TTF 落档');  // check('A7.1')
assert(reportQuotesOflTerms, 'A7.2 免费商用条款（PERMISSION）+ Reserved Font Name 改名禁则逐字引用'); // check('A7.2')
assert(zcoolSampleNonBlank, 'A7.3 站酷样字非空白（黑占比>1%）');                   // check('A7.3')

// coverage: UAA-A8(B7)_B7 Baloo 2 动态时间 imgfont
assert(imgfontGlyphCount === 11, 'A8.1 11 枚 imgfont（0-9+冒号）在场');            // check('A8.1')
expect(imgfontDims.every(d => d === imgfontDims[0])).toBe(true);                   // check('A8.2') 11 枚等宽箱体逐枚一致
assert(digitBottomsAligned && colonCenteredAligned, 'A8.3 数字 bottom-y 一致 + 冒号垂直中心对齐'); // check('A8.3')

// coverage: UAA-A9(B8)_B8 资产总量 vs SPIFFS 预算
assert(partitionAssetsLinePresent, 'A9.1 分区表 assets 行在场');                   // check('A9.1')
expect(reportTotalBytes).toEqual(dataSizeSumFromAssetsC());                        // check('A9.2') 报告 total == data_size 求和
assert(reportHasI1Conclusion, 'A9.3 预算报告含 I1 判定结论行');                    // check('A9.3')

// ── E2 / E4 管线出口 ──
assert(flattenBeforeGreyscale && svgUseInline, 'E2 flatten 先于 greyscale + svgUse 内联'); // check('E2')
assert(feasibilityReportPresent && budgetReportPresent, 'E4 可行性报告+预算报告落位');      // check('E4')
```

---

## 13. 决策承载反查（decisionCoverage 4 决策 → acceptance）

| decision | 摘要 | 承载 |
|---|---|---|
| strategy-§2.2-美术元素资产化 | 吉祥物七姿态、手绘框族、三档网点、涂鸦点缀、印章全部离线从设计稿 SVG 栅格化为位图资产，运行时 lv_image 渲染 | UAA-A2（§2 七姿态）+ 同族框/网点/图标/印章分别由 UAA-A3/A5/A6/A4 承载（§3/§5/§6/§4） |
| OQ-1a-倾斜烘进资产 | 倾斜是风格资产不是动态行为，栅格化时直接烘进资产，运行时零成本 | UAA-A4（§4） |
| OQ-2a-框族9宫格资产化 | 手绘框族离线渲染成 9 宫格可拉伸框图，全站统一 3–4 种框型，运行时按尺寸拉伸角部不变形 | UAA-A3（§3） |
| strategy-§4-资产总量盯防 | RGB565 资产 2 字节/像素，大图资产须盯总量；资产总尺寸预算与 SPIFFS 分区表核对进验收 | UAA-A9（§9） |

四条决策全部有 acceptance 承载，无 `dropped` 未归口项。

---

## 14. qualityNotes（作者自检）

1. **非纯结构集**：本包是「美术元素全量出图」类工作，`png/` 清单 grep、`assets.c` 描述符数、授权文件在场是
   close 目标本身；集合内嵌了运行时效应断言——A2.3 非空黑占比、A3.3 9 宫格拼回一致性、A5.2 密度递增、A8.2
   imgfont 等宽、A9.2 求和对账——避免纯结构空过。
2. **真实栅格化、非 mock**：走 sharp/libvips 真实 SVG 栅格化 + host+SDL 真实软件渲染，无 mock 层；「模拟器阈值化
   PNG」与「真机 1bpp 前光观感」仍有系统性差（FreeType 灰度抗锯齿 vs 阈值化），已按 E3 钉「验收图一律阈值化 PNG」
   并保留 ARQ-260831-simulator-truecolor-vs-1bpp 的 residual risk 标注，不伪装成低风险通过。
3. **CSS class 不是 symbol（核心红线）**：框族/网点/印章母版是 CSS class（`.frm` 八值 border-radius / `.d50..d12`
   data-uri / `.stamp-seal`），builder 须从 CSS 构造 SVG 父本、做 9 宫格切分/平铺/烘倾斜；本文档对这三条（A3/A4/A5）
   **禁止写「存在符号即过」**，只钉切分产物、拼回一致性、密度、倾斜烘入等真实结果。
4. **观感类归 human 目测 OQ-3(a)**：造型达标（A2.4）、接缝/角部观感（A3.4）、锯齿可接受（A4.4）、平铺观感（A5.3）、
   子集字可读（A7.3）、等宽表格线（A8.3）全部明确归 human 出口，验收只钉可机判近似（非空白/黑占比/尺寸/切分产物）。
5. **红基线诚实映射**：红基线（§1）红在「美术元素全量出图尚未发生」（story 缺失的真实行为），每条 UAA 的 RED
   状态逐条标注对应缺失行为（其余 6 姿态 / 框族 / 网点 / 印章 / 12 枚图标 / 授权 / imgfont 11 枚 / 预算报告）。
6. **预算联动钉死**：A9 超预算 → 结论「必须上 I1」→ 升 `ARQ-260831-i1-indexed-asset-on-device` P0（feature-inventory
   §1.5 与 debt 账本该条目（`docs/debt/tech/ARQ-260831-i1-indexed-asset-on-device.md`）trigger 已记此联动）。RGB565 2 字节/像素 + 分区 8M 对照值在本文档写死为机判基准。