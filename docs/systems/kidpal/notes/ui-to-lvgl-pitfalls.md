# UI 设计稿 → LVGL 实现踩坑记录（KidPal P1 home 页面）

> 记录人：Claude Code
> 日期：2026-08-30
> 关联：KidPal P1 home 页面 / LVGL v9.5.0 Windows PC simulator / ESP32-S3-RLCD-4.2（400×300 横屏）
> 状态：初稿，待后续页面经验补充后再沉淀为 skill

## 1. 环境/项目基础

### 1.1 模拟器工程来源
- 使用 `lvgl/lv_port_pc_visual_studio`（LVGL v9.5.0）。
- 复制到本仓 `tools/simulator/`，不要直接改原始仓库。
- 主入口是 `LvglWindowsSimulator/LvglWindowsSimulator.cpp`。

### 1.2 分辨率必须显式改成真机尺寸
- 原工程默认 `800×480`；KidPal 是 `400×300` 横屏。
- 修改 `LvglWindowsSimulator.cpp` 中 `lv_windows_create_display(..., 400, 300, ...)`。
- 注意顺序是 `宽, 高`，不要写成竖屏 `300, 400`。

### 1.3 VS2022 构建要用 PowerShell / CMD，不能用 Git Bash
- Git Bash 会把 MSBuild 的 `/p:` 开关解释成根路径，报 `MSB1008: Only one project can be specified`。
- 正确命令（PowerShell）：
  ```powershell
  & 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe' `
    'C:\AI\ESP32\tools\simulator\LvglWindowsSimulator\LvglWindowsSimulator.vcxproj' `
    /p:Configuration=Debug /p:Platform=x64 /m
  ```

## 2. 新增 C 文件进工程

### 2.1 必须加入 `.vcxproj`
- 直接把 `kidpal_ui.c` / `kidpal_ui.h` 放目录下不会被 VS 自动编译。
- 要在 `.vcxproj` 的 `ItemGroup` 里加一行：
  ```xml
  <ClCompile Include="$(MSBuildThisFileDirectory)kidpal_ui.c" />
  ```
- 同时 `LvglWindowsSimulator.cpp` 不要只写 `extern ...`，要 `#include "kidpal_ui.h"`，否则 C++ name mangling 会导致链接错误 `unresolved external symbol`。
- `kidpal_ui.h` 里必须包 `extern "C"`：
  ```c
  #ifdef __cplusplus
  extern "C" { #endif
  void kidpal_ui_create(lv_obj_t* parent);
  #ifdef __cplusplus
  } #endif
  ```

## 3. 中文字体/FreeType

### 3.1 启用 FreeType 后栈要开大
- `lv_conf.h` 默认 `LV_DRAW_THREAD_STACK_SIZE (8 * 1024)`，编译报错：
  `#error "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."`
- 改成 `(32 * 1024)`。

### 3.2 字体文件路径/位置
- FreeType 通过 `lv_freetype_font_create(path, ...)` 加载文件，运行时按工作目录找文件。
- 调试最稳的方式是用绝对路径指向字体，例如 `C:/Windows/Fonts/msjh.ttc`。
- 也可以把 `.ttf/.ttc` 复制到输出目录 `Output/Binaries/Debug/x64/` 或源码目录。

### 3.3 中文标签必须显式指定中文字体
- `LV_FONT_DEFAULT` 是内置位图字体，不支持中文，会显示成方块（□□）。
- 不要把中文 label 的字体参数写成 `LV_FONT_DEFAULT`。
- 做法：加载一个 FreeType 字体指针 `g_cjk_font`，封装 `make_label(..., font)` 时中文传 `NULL`/`g_cjk_font`，数字/英文传 `&lv_font_montserrat_XX`。

### 3.4 Variable font 不一定可靠
- `NotoSansSC-VF.ttf` 加载后部分字形能显示部分不能，表现为某些中文字缺字。
- 换成普通 TrueType Collection `msjh.ttc`（微软雅黑）后正常。
- 如果以后用 `NotoSansSC`，建议下载静态 Regular/Normal 版本，不要用 VF。

### 3.5 缓存大小
- `LV_FREETYPE_CACHE_FT_GLYPH_CNT` 默认 256，中文页面不够会淘汰字形。
- 本次改为 2048 保险；后续可按页面最大字符数调整。

## 4. 坐标/对齐

### 4.1 `LV_ALIGN_TOP_RIGHT` 会让对象的右上角对齐到 `set_pos` 的坐标点
- 本来以为 x=376 右对齐会把文本右边贴在 x=376，结果文本区域向左超出，可能被父容器 clip 掉。
- 对不确定宽度的文本，优先用 `LV_ALIGN_TOP_LEFT` + 实测 x 偏移，避免消失。

### 4.2 绝对定位容器默认不透明
- 用 `lv_obj_create` 做组合对象（如吉祥物）时，容器默认有背景色，会遮挡后面元素。
- 对“组合层”要：`lv_obj_remove_style_all(c)` + `lv_obj_set_style_bg_opa(c, LV_OPA_TRANSP, 0)`。
- 子对象（头、身体）保持白色填充和黑色边框。

### 4.3 屏幕底部会被 simulator 的 perf/monitor 条遮挡
- `lv_conf.h` 里 `LV_USE_PERF_MONITOR` / `LV_USE_MEM_MONITOR` 默认显示在底部，会盖住 UI 的底部 key bar。
- 开发预览阶段可把它们移到顶部角落：
  ```c
  #define LV_USE_PERF_MONITOR_POS LV_ALIGN_TOP_RIGHT
  #define LV_USE_MEM_MONITOR_POS  LV_ALIGN_TOP_LEFT
  ```
- 注意：本次尝试把两个 monitor 同时关闭后窗口全白，原因未深入排查，先保留并改位置。

## 5. 绘图/形状

### 5.1 v9 的 `lv_line_set_points` 需要 `lv_point_precise_t`
- 旧代码/示例里用 `lv_point_t` 会报 `incompatible types` warning（C4133）。
- 统一改成 `static lv_point_precise_t pts[] = {...}`。

### 5.2 旋转小耳朵会变成菱形
- `lv_obj_set_style_transform_rotation(el, 450, 0)` 旋转的是整个带边框的矩形对象，不是三角形。
- 想要真正的三角形耳朵，需要用 `lv_canvas` + 自定义绘制，或接受菱形近似。
- 当前吉祥物用菱形耳朵近似，后续若要求严格复刻 SVG，需换方案。

### 5.3 圆角矩形/按钮的样式要手动清默认
- `lv_obj_create` 默认有内边距、滚动条、阴影等。
- 做纯形状时：`lv_obj_remove_style_all(obj)` + 显式设置 bg/border/radius + `lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE)`。

## 6. 动画

### 6.1 冒号眨眼
- 把冒号做成两个黑色圆角矩形，每 4.4s 同步压扁高度即可。
- 注意截图时可能正好拍到闭眼状态，看起来是横线；实际运行没问题。

## 7. 字体启用

- 用到 `lv_font_montserrat_48` 时，必须在 `lv_conf.h` 开启：
  ```c
  #define LV_FONT_MONTSERRAT_48 1
  ```
- 默认大字号只到 40 左右，48 是关闭的，直接用会报 `undeclared identifier`。

## 7.5 真机字体方案：小智的 SPIFFS .bin 字库（调查结果）

> 证据来源：XiaoZhiCode V2.1.0（`ESP32-S3-RLCD-4.2-Demo/03_XiaoZhi/XiaoZhiCode_V2.1.0/`），已建 CodebaseMemory 图索引（项目名 `xiaozhi-rlcd`）。

### 方案本质
小智**不用 FreeType，也不把字库编进固件**，而是第三条路：
1. 离线用 LVGL 官方 `lv_font_conv` 把**阿里巴巴普惠体（PuHui）**转成 `.bin` 位图字库：
   - 正文：`font_puhui_14_1` / `font_puhui_basic_14_1` / `font_puhui_common_20_4`（命名规律 = `puhui_尺寸_bpp`，1 单色、4 灰度抗锯齿）。
   - 图标：`font_awesome_14_1`（FontAwesome 图标字，图标当字符显示）。
2. 转好的 `.bin` 通过 `scripts/spiffs_assets/build.py` 打进 SPIFFS 资源分区（`assets.bin`），和唤醒模型、emoji 图片一起。
3. 运行时 `main/assets.cc` 从 SPIFFS 读取 `.bin`，用 `LvglCBinFont` 包装 `cbin_font_create(data)` 生成 `lv_font_t*`，挂到主题上。

### 关键代码位置
| 文件 | 作用 |
|------|------|
| `main/CMakeLists.txt` | 定义 `BUILTIN_TEXT_FONT`/`BUILTIN_ICON_FONT` 宏（如 `font_puhui_14_1`） |
| `main/display/lvgl_display/lvgl_font.h` | `LvglBuiltInFont`（编译期）/`LvglCBinFont`（运行时 .bin）两类封装 |
| `main/display/lvgl_display/lvgl_font.cc` | `LvglCBinFont` 调 `cbin_font_create(uint8_t* data)` |
| `main/assets.cc` | 从 SPIFFS assets 读 `.bin`，失败打 `Failed to load fonts.bin` |
| `main/display/lcd_display.cc` / `oled_display.cc` | 把 text/icon font 挂到 light/dark theme |
| `scripts/spiffs_assets/build.py` | 打包 `.bin` 进 assets |

### 为什么适合 KidPal
- 这块屏是 RLCD 反射屏：低功耗、刷新慢、CPU/RAM 预算紧。
- `.bin` 位图直接加载到内存，省掉 FreeType 栅格化的 CPU/RAM 开销。
- 不编进固件，Flash 占用灵活，可随 UI 迭代换字库。
- 图标单独一套 FontAwesome 字体，电池/WiFi/静音等图标"打一个字"即可，不用画图片。

### KidPal 落地方案（推荐）
1. **模拟器阶段（现在）**：继续用 FreeType + 系统字体，改 UI 最快。
2. **上真机阶段**：切换为小智同款方案：
   - 把 P1–P4 所有文案的字符提取成去重集合；
   - 用 `lv_font_conv` 生成 PuHui 子集 `.bin`（正文 + 图标分开）；
   - 放 SPIFFS 资源分区，运行时加载；
   - 预留 `LvglCBinFont` 封装或参考其结构。
3. 该方案和笔顺 JSON、课表 MD 一样走资源分区，保持统一。

### 完整方案对比（按 MCU 场景排序）
| 方案 | 做法 | 优点 | 缺点 | 适合 |
|------|------|------|------|------|
| ① 编译期位图子集 | `lv_font_conv` → C 数组编进固件 | 最快、零运行时开销 | 中文全字库几 MB，只能转子集 | 文字固定的界面 |
| ② 外部文件位图（**小智同款**） | `lv_font_conv` → `.bin`，运行时 SPIFFS 加载 | 不编固件、可随时换、现成位图 | 占 RAM 装缓存、仍要离线转子集 | **KidPal 推荐** |
| ③ FreeType 矢量 | 运行时加载 `.ttf` | 任意字号、字形全 | RAM/CPU 开销大，真机慎用 | PC 模拟器快速预览 |
| ④ TinyTTF 轻量矢量 | `LV_USE_TINY_TTF` | 矢量但比 FreeType 轻 | 功能弱于 FreeType | 想要矢量但 RAM 紧张 |
| ⑤ imgfont 图片字体 | 每个字符一张图片 | 彩色/emoji/艺术字 | 不是真文字，排版麻烦 | emoji、小伴表情 |

## 8. 后续待补

- [ ] 多页面切换（P2 课表 / P3 故事 / P4 查字）的实现方式。
- [ ] 网点纹理（d50/d25/d12）在 LVGL 中如何用 canvas/图片/自定义绘制模拟。
- [ ] SVG 吉祥物精确移植方案（canvas vs image vs vector）。
- [ ] 真机字体子集生成脚本：从 P1–P4 文案提取字符 → `lv_font_conv` 参数 → `.bin` 输出 → SPIFFS 打包（参考小智 `scripts/spiffs_assets/build.py`）。
- [ ] 设计稿中手绘不规则圆角（border-radius 非对称）在 LVGL 中的实现策略。

## 9. 关键文件清单

| 文件 | 作用 |
|------|------|
| `tools/simulator/LvglWindowsSimulator/LvglWindowsSimulator.cpp` | 入口、display 尺寸 |
| `tools/simulator/LvglWindowsSimulator/lv_conf.h` | FreeType、monitor、字体开关、栈大小 |
| `tools/simulator/LvglWindowsSimulator/kidpal_ui.h` | UI 接口（extern "C"） |
| `tools/simulator/LvglWindowsSimulator/kidpal_ui.c` | P1 home 实现 |
| `tools/simulator/LvglWindowsSimulator/LvglWindowsSimulator.vcxproj` | 工程文件，要加入 `kidpal_ui.c` |
| `tools/simulator/LvglWindowsSimulator/NotoSansSC-VF.ttf`（已复制，未最终使用） | 备选字体 |

## 10. 一页式速查

```c
// 1. 加载中文字体
static lv_font_t* g_cjk_font = NULL;
g_cjk_font = lv_freetype_font_create(
    "C:/Windows/Fonts/msjh.ttc",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    16,
    LV_FREETYPE_FONT_STYLE_NORMAL);

// 2. 中文 label
lv_obj_t* lbl = lv_label_create(parent);
lv_label_set_text(lbl, "钢琴课");
lv_obj_set_style_text_font(lbl, g_cjk_font, 0);

// 3. 组合层透明容器
lv_obj_t* group = lv_obj_create(parent);
lv_obj_remove_style_all(group);
lv_obj_set_style_bg_opa(group, LV_OPA_TRANSP, 0);
lv_obj_clear_flag(group, LV_OBJ_FLAG_SCROLLABLE);
```
