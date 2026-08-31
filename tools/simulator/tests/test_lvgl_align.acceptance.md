# 冻结验收 — TaskPackage 260831-simulator-lvgl-align（subsystem: tools，testHost: host+SDL）

> 本文件是 builder/fixer 只读的冻结验收。「做完」= 下述每条断言对号入座判绿。
> 判据一律是**可机判或可目测的明确判据**（版本号 grep、conf 键值比对、快照 PNG 阈值化输出、
> 8 资产逐个渲染对照、MSBuild 退出码 + 产物存在性）。「没报错就算过」在本文档不成立。
>
> 故事（stroy 摘要）：把 LVGL PC 模拟器从 9.5.0 降级/锁定到与固件同版 9.3.x，`lv_conf.h`
> 以固件 `sdkconfig` 为基准对齐，把 1bpp 阈值化快照自查正式化，并让昨天在 9.5 达标的 P1 在
> 9.3 下回归——重新成立「模拟器验证结论 ≈ 真机结论」，作为 UI 资产全量验证（TP-B）的前置。

---

## 0. Coverage map（acceptanceRef → 小节）

| acceptanceRef | 本文件小节 | 断言内容 |
|---|---|---|
| SLA-A2 | §2 | 模拟器 LVGL 降级/锁定 9.3.x，`lv_version.h` 三版本号 = 9.3.x，工程编译跑通（机械断言） |
| SLA-A3 | §3 | `lv_conf.h` 以固件 sdkconfig 为基准（ThorVG 关、单色阈值一致、FreeType 栈 ≥32KB） |
| SLA-A4 | §4 | 1bpp 阈值化快照一键自查正式化（snapshot_timer 收编为正式工具，验收图一律阈值化 PNG） |
| SLA-A5 | §5 | P1 现有 8 资产在 9.3 下渲染无回归，重点盯 I1/调色板行为差异是否复现 |
| SLA-A6 | §6 | 实验代码正式化（kidpal_ui.c / gen_assets.mjs / vcxproj 资产挂接 由本包接管重写或正式化） |
| （边界/错误） | §7 | 版本仍未对齐、conf 键不一致、快照非阈值化、资产缺失、构建失败 → 必须红 |
| （决策承载反查） | §8 | decisionCoverage 3 决策逐条落 acceptance |

---

## 1. 红基线重跑结果（本次实测，2026-09-01）

**命令**（TaskPackage `redBaselineCommand`）：

```bash
grep -E 'LVGL_VERSION_(MAJOR|MINOR|PATCH)' tools/simulator/LvglPlatform/lvgl/lv_version.h
```

**逐字输出**：

```
#define LVGL_VERSION_MAJOR 9
#define LVGL_VERSION_MINOR 5
#define LVGL_VERSION_PATCH 0
```

**判定：红（确认现状错位）**。模拟器 = **9.5.0**；固件三方一致锁定 9.3.0：

| 证据 | 值 |
|---|---|
| `firmware/main/idf_component.yml` | `lvgl/lvgl: ~9.3.0`（第 43 行） |
| `firmware/dependencies.lock`（`lvgl/lvgl` 条目） | `version: 9.3.0` |
| `firmware/managed_components/lvgl__lvgl/lv_version.h` | 9.3.0 |

红原因 = 当前 story 缺失的真实行为：版本对齐尚未发生。昨天 P1 全部在 9.5 上验证，9.3 一个都没验；
9.5 软件渲染 I1 调色板被绕过的实测差异印证「9.5 过 ≠ 9.3 无坑」。本包把模拟器对齐到 9.3。

---

## 2. SLA-A2 — 模拟器 LVGL 降级/锁定 9.3.x + 编译跑通

### A2.1 版本三号与固件逐字节一致
`coverage: SLA-A2`

```bash
diff \
  <(grep -E 'LVGL_VERSION_(MAJOR|MINOR|PATCH)' tools/simulator/LvglPlatform/lvgl/lv_version.h) \
  <(grep -E 'LVGL_VERSION_(MAJOR|MINOR|PATCH)' firmware/managed_components/lvgl__lvgl/lv_version.h)
```

**通过判据**：diff 输出为空（两个 `lv_version.h` 的 MAJOR/MINOR/PATCH 三行逐字节相同）。
**当前状态**：红（9.5.0 vs 9.3.0，diff 非空）。锁定目标 = 固件 `~9.3.0` → 三号应为
`MAJOR 9 / MINOR 3 / PATCH 0`；若 builder 取 9.3.x 其它 patch，必须与本 A2.1 锁定及固件
`dependencies.lock` 的 `9.3.0` 收口一致，不得只改 `lv_version.h` 头文件而不换组件实树。

### A2.2 工程在 9.3 上编译跑通
`coverage: SLA-A2`（机械断言：构建退出码 + 产物存在）

```powershell
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe' `
  'C:\AI\KidPal\tools\simulator\LvglWindowsSimulator\LvglWindowsSimulator.vcxproj' `
  /p:Configuration=Debug /p:Platform=x64 /m
```

**通过判据**：
1. MSBuild 退出码 = 0；
2. `tools/simulator/Output/Binaries/Debug/x64/LvglWindowsSimulator.exe` 存在且文件非空。

**注意（踩坑 §1.3）**：必须用 PowerShell/CMD 跑 MSBuild，不能用 Git Bash（`/p:` 会被解释成根路径）。
**当前状态**：仅证过 9.5 上的编译；本包目标 = 降级 9.3 **后**编译仍绿（9.3 组件实树下重新编译，不得靠残留 9.5 产物）。

---

## 3. SLA-A3 — lv_conf.h 以固件 sdkconfig 为基准对齐

基准 = `firmware/sdkconfig` 的 LVGL 配置（Kconfig 键），模拟器侧 = `tools/simulator/LvglWindowsSimulator/lv_conf.h`。
仅本包声明的对齐面，逐键比对，不扩范围。

### A3.1 ThorVG 关 / 矢量图形关
`coverage: SLA-A3`

```bash
grep -E '^#define (LV_USE_VECTOR_GRAPHIC|LV_USE_THORVG_INTERNAL|LV_USE_THORVG_EXTERNAL)' \
  tools/simulator/LvglWindowsSimulator/lv_conf.h
```

**通过判据**：三行全部为 `0`（`LV_USE_VECTOR_GRAPHIC 0`、`LV_USE_THORVG_INTERNAL 0`、
`LV_USE_THORVG_EXTERNAL 0`），与固件 `sdkconfig` 的
`# CONFIG_LV_USE_VECTOR_GRAPHIC is not set`、`# CONFIG_LV_USE_THORVG is not set` 同语义。
**当前状态**：9.5 的 lv_conf.h 已是 0（三条在），降级 9.3 后须保持（9.3 树内 lv_conf.h 重新落位这些值）。

### A3.2 单色阈值一致（I1 亮度阈值 = 固件 127）
`coverage: SLA-A3`

```bash
grep -nE 'I1_LUM_THRESHOLD|LUM_THRESHOLD' tools/simulator/LvglWindowsSimulator/lv_conf.h
grep -nE 'CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD' firmware/sdkconfig
```

**通过判据**：
1. 模拟器 `LV_DRAW_SW_I1_LUM_THRESHOLD == 127`；
2. 与固件 `CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD=127` 数值相等；
3. **阈值常量单一源**：`tools/asset-pipeline/gen_assets.mjs` 与 §4 的正式快照工具所用的阈值化常量
   必须引用 / 等于该 `127`，不得各写一份可漂移的魔数。**已知待收口**：现 `gen_assets.mjs` 第 59 行
   `const black = data[y * w + x] < 128;` 用 `< 128`，与固件 `127` 有 1 档错位（127 vs 128 边界像素成反相），
   本包须统一到 `127` 语义并写注释说明，防单色资产在真机 flush 路径下出现边界像素反转。

### A3.3 FreeType 栈 ≥ 32KB
`coverage: SLA-A3`

```bash
grep -nE 'LV_DRAW_THREAD_STACK_SIZE' tools/simulator/LvglWindowsSimulator/lv_conf.h
```

**通过判据**：模拟器 `LV_DRAW_THREAD_STACK_SIZE` 的值 ≥ `32 * 1024`（如 `(32 * 1024)`），
且 `LV_USE_FREETYPE 1`（模拟器快速迭代走 FreeType）。**注**：固件 `CONFIG_LV_DRAW_THREAD_STACK_SIZE=8192`
是**有意的分歧**（固件 `CONFIG_LV_USE_FREETYPE is not set`，走 `.bin` 位图字体，无需大栈）；
本断言只约束模拟器侧 ≥32KB，不要求模拟器栈 == 固件 8KB。

---

## 4. SLA-A4 — 1bpp 阈值化快照一键自查正式化

现状：`tools/simulator/LvglWindowsSimulator/kidpal_ui.c` 的 `snapshot_timer`（第 272 行）用
`lv_snapshot_take(scr, LV_COLOR_FORMAT_ARGB8888)` + `lodepng_encode32` 直写 `C:/tmp/sim_canvas.png`，
**未阈值化**（真彩 ARGB8888，含灰度抗锯齿），另有第 224-234 行一段吉祥物缓释的
`C:/tmp/dbg_mascot.png` 实验残渣。此为「实验代码」，本包收编为正式工具。

### A4.1 存在正式快照工具 + 输出严格 2 级阈值化 PNG
`coverage: SLA-A4`

**通过判据**（机判）：
1. 存在一个非实验命名、路径稳定的快照工具（builder 定名/定位，见 A6.1），一键完成「画布快照 → 阈值化 → PNG 落盘」；
2. 其输出的 PNG **逐像素只有两级**：每像素 RGB 三通道同值且取值 ∈ {0, 255}（纯黑/纯白），
   不存在 1..254 的灰度中间值。**机判命令**（用 sharp/node 扫描输出 PNG）：
   `node -e "导入输出 PNG，统计 distinct pixel value 集合 ⊆ {0,255}×3 同通道"` ← 输出唯一值集合必须
   `{{0,0,0},{255,255,255}}`（最多两级），任何第三值即红。

### A4.2 阈值常量 = 固件 I1 阈值（127），不是硬编码魔数
`coverage: SLA-A4`

快照工具的阈值化常量必须与 §A3.2 同一来源（127），与固件 `lv_conf` 的 `LV_DRAW_SW_I1_LUM_THRESHOLD`
一致；工具内注释写明「与真机 flush 后处理同一阈值」。**当前状态**：红——现 snapshot 无阈值化步骤。

### A4.3 一键 + 脚本化（截完即退、路径确定）
`coverage: SLA-A4`

**通过判据**：快照工具截完即退（现 `PostQuitMessage(0)` 语义保留），产物路径确定可预测
（不得再散落 `C:/tmp/sim_canvas.png` 这类硬编码实验路径），便于脚本化回归（§5 的 9.3 对照即依赖此）。

### A4.4 验收图一律以阈值化版本为准
`coverage: SLA-A4`（出口 = 收敛到 exitCriteria SLA-E3 的产物）

本包落地产物 `changes/dev/260831-simulator-lvgl-align/` 下归档的 P1 回归截图必须是 A4.1 的阈值化 PNG
版本，**不得提交非阈值化的真彩 ARGB 截图作为「验收图」**。

---

## 5. SLA-A5 — P1 现有 8 资产在 9.3 下渲染无回归（盯 I1/调色板差异）

8 资产白名单（`tools/simulator/LvglWindowsSimulator/assets/png/` 现有）：
`pose_daily_96`、`doodle_star_26`、`doodle_cloud_52`、`drop_baby`、`ic_batt_32`、`ic_bell_13`、`thermo_man`、`time_10_24`。

### A5.1 资产格式 = RGB565，不是 I1（结构上打消 9.5 调色板绕过）
`coverage: SLA-A5`（I1/调色板差异的根因封锁）

```bash
grep -nE 'LV_COLOR_FORMAT_(RGB565|I1|L8)' tools/simulator/LvglWindowsSimulator/assets/assets.h
```

**通过判据**：`assets.h` 中 8 个 `lv_image_dsc_t` 的 `.cf` 全部为 `LV_COLOR_FORMAT_RGB565`，任何一项为
`LV_COLOR_FORMAT_I1`/`L8` 即红（I1 是 9.5 软件渲染路径绕过调色板、把位图当 alpha/灰度的起火点，
9.3 上复用 I1 才可能复现该差异；管线已定 RGB565 为最稳格式，A5.1 把它钉成硬约束）。

### A5.2 8 资产逐枚在 9.3 下渲染对照源资产（机判：非空 + 黑白占比容差）
`coverage: SLA-A5`

**通过判据**：用 §4 正式快照工具在 9.3 模拟器上渲染 P1（或逐资产渲染），对每枚资产取其边界盒区域，
与源 PNG（`assets/png/<name>.png`，本身已阈值化）做**逐资产对照**：
1. 该区域黑像素数 > 0 且白像素数 > 0（非全黑/非全白/非空白 → 排除渲染为空白或纯色块）；
2. 该区域黑像素占比 与源 PNG 黑占比 之差 ≤ 容差（建议 5%，builder 定后写死），超出即判回归
   （覆盖「灰阶失真导致细线/小字糊成一团」类 I1/调色板/阈值化差异复现）。
   机判命令 = 用 sharp 读取 9.3 快照裁边盒 + 读出像素统计，与源 PNG 逐资产对照。

### A5.3 P1 全屏 9.3 阈值化快照：8 资产区域全部在场
`coverage: SLA-A5`

**通过判据**：9.3 下 P1 首页全屏阈值化快照（A4.4 的产物）中，8 资产的预期位置各有非白内容
（映射到 kidpal_ui.c 的 `img_at` 布局坐标：时间字块 `time_10_24 @(14,42)`、吉祥物
`pose_daily_96 @(8,172)`、电池 `ic_batt_32 @(330,11)`、涂鸦星 `@(228,14)`、云 `@(230,30)`、
温度计小人 `@(118,196)`、水滴 `@(216,204)`、铃铛 `@(372,184)`），证明 8 资产在 9.3 下确实被
`lv_image` 挂载渲染（不只是 `lv_image_dsc_t` 存在）。

**当前状态**：红——无任何 9.3 渲染产物；8 资产对照 harness 不存在；昨天达标结论全部来自 9.5。
「无回归」的**视觉保真**判断（结构/风格是否达标）归 human 目测 / design review（避免把间距、
观感纯美学钉成脆断言），本小节的机判只钉「资产非空白、占比不漂移、全在场」三类真实结果。

---

## 6. SLA-A6 — 实验代码正式化（kidpal_ui.c / gen_assets.mjs / vcxproj 资产挂接）

策略 §6/§8.4 留根：模拟器实验代码（`kidpal_ui.c` / `gen_assets.mjs` / vcxproj 资产挂接）由 simulator
dev-loop 包接管重写或正式化，避免实验代码沉淀为事实标准。

### A6.1 snapshot 实验残渣清零
`coverage: SLA-A6`（与 A4 同一收编动作的残渣侧）

```bash
grep -nE 'C:/tmp/sim_canvas|C:/tmp/dbg_mascot|snapshot_timer|lodepng_encode32' \
  tools/simulator/LvglWindowsSimulator/kidpal_ui.c
```

**通过判据**：命中数为 0 或仅剩「调用正式工具入口」的合法引用（不得残留
`lodepng_encode32` 直写 `C:/tmp/…` 的实验散落代码）。`snapshot_timer` 的阈值化逻辑迁入 §4 正式工具，
`kidpal_ui.c` 不再内联实验快照（`debug_mascot` 那 10 行实验残渣同删）。

### A6.2 gen_assets.mjs 正式化（接口稳定 + 非实验命名 + 阈值统一）
`coverage: SLA-A6`

**通过判据**：
1. `tools/asset-pipeline/gen_assets.mjs` 保持稳定 CLI 契约（`node gen_assets.mjs <design.html> <outDir>`），
   头注释标明本包接管后的 owner 与「生成勿手改」语义（`assets.c`/`assets.h` 已含
   `Auto-generated … 勿手改` 头，保留并核实）；
2. 生成产物落位与 §2/§5 的 `assets.{c,h}` + `png/*.png` 一致（RGB565 + 阈值化 PNG 双出，见 A5.1/A3.2）；
3. 文件内无「实验/一次性验证」性质的连篇直写注释仍暗示其临时身份；阈值常量统一到 127（A3.2）。

### A6.3 vcxproj 资产挂接显式、非脆弱
`coverage: SLA-A6`

**通过判据**：`LvglWindowsSimulator.vcxproj` 中 `kidpal_ui.c`、`assets/assets.c` 以显式
`<ClCompile Include=...>` 挂接、`assets/assets.h` 可被 include（踩坑 §2.1：文件放目录不会被 VS 自动编译，
必须在 `.vcxproj` 显式加行）。机判命令：

```bash
grep -nE 'kidpal_ui\.c|assets[\\/]assets\.c' tools/simulator/LvglWindowsSimulator/LvglWindowsSimulator.vcxproj
```

**通过判据**：两行 `<ClCompile>` 均在；未命中即「实验代码靠手动/残留才编译」→ 红。

---

## 7. 边界 / 错误断言（必须红的情形）

| 断言 | 触发条件 | 判红方式 |
|---|---|---|
| B1 版本未对齐 | `lv_version.h` 仍 9.5.x（或 MAJOR≠9 或 MINOR≠3） | A2.1 diff 非空 → 红 |
| B2 conf 键漂移 | ThorVG 任一为 1 / I1 阈值 ≠127 / 栈 <32KB | A3.1/A3.2/A3.3 grep 不匹配 → 红 |
| B3 快照非阈值化 | 快照 PNG 存在 1..254 中间灰度值 | A4.1 唯一值集合 ⊄ {0,255} → 红 |
| B4 资产缺失 | 8 白名单任一不在 `assets.h` 或不在 `png/` | A5.1/A5.2 枚举缺项 → 红 |
| B5 资产退回 I1 | 任一 `lv_image_dsc_t.cf == LV_COLOR_FORMAT_I1` | A5.1 → 红（调色板绕过风险重开） |
| B6 构建失败 | MSBuild 退出码 ≠0 或 exe 缺失 | A2.2 → 红 |
| B7 实验残渣残留 | `kidpal_ui.c` 仍含 `C:/tmp/*` 直写 | A6.1 → 红 |
| B8 阈值错位 | 任一路径阈值常量 ≠127（含 `<128` 残留） | A3.2/A4.2 → 红 |

---

## 8. 决策承载反查（decisionCoverage 3 决策 → acceptance）

| decision | 摘要 | 承载 |
|---|---|---|
| strategy-§3-版本对齐 | 模拟器降级 9.3.x + lv_conf 对齐 + 1bpp 验证手段 | SLA-A2 / SLA-A3 / SLA-A4 |
| strategy-§8.4-实验代码接管 | kidpal_ui.c / gen_assets.mjs / vcxproj 挂接正式化 | SLA-A6（§6） |
| ARQ-260831-simulator-lvgl-version-drift | 模拟器↔固件版本对齐常驻风险，本包顺手做 | SLA-A2（§2） |

三条决策全部有 acceptance 承载，无 `dropped` 未归口项。

---

## 9. qualityNotes（作者自检）

1. **非纯结构集**：本包是「配置/版本对齐」类工作，版本号 grep / conf 键值比对是 freeze 目标本身
   （不是拿源码魔法数当运行时行为证据的反模式）。集合内嵌了运行时效应断言——A2.2 编译跑通
   （退出码 + exe 产物）、A5.2/A5.3 的 9.3 逐资产渲染对照——避免纯结构空过。
2. **真渲染、非 mock**：快照走 host+SDL 的真实软件渲染 + 真实 SVG 栅格化管线，无 mock 层。
   「真机 1bpp 前光观感」与模拟器阈值化快照仍有系统性差，已在 A5/A4.4 标为 residual risk，
   靠阈值化快照逼近 + 阈值统一到固件 I1 阈值（127）缓解，不伪装成低风险通过。
3. **9.5 过≠9.3 无坑的实证锚**：A5.1 把 RGB565（非 I1）钉成硬约束，配合 A3.2 阈值单一源，
   是从根因（I1 调色板绕过 + 阈值魔数漂移）封锁差异复现，而不是事后肉眼对照。
4. **无回归的视觉保真**（结构/风格是否达标）明确归 human 目测 / design review，不写成脆断言；
   本文件只钉机器可判的「非空白 / 占比容差 / 全在场」。
5. **红基线诚实映射**：红基线（§1）红在版本错位（story 缺失的真实行为）；其余 RED 状态逐条标注
   于 A4/A5/A6，对应各自缺失行为（阈值化工具未成形 / 无 9.3 渲染对照 / 实验残渣未清）。A3 的
   conf 键在 9.5 上一部分已齐，但必须随 9.3 降级后的 lv_conf.h 重新落位判绿，属「随 A2 依赖」。