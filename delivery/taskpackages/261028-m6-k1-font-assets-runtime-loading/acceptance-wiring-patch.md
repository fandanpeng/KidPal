# acceptance-wiring-patch · 261028-m6-k1-font-assets-runtime-loading（v2 · 预基线修正）

> 作者 = acceptance-author；v2 = 2026-10-28 human 双裁（provenance 换 78 fork --format cbin / host 半 = 宽度感知解码器）后的**增量版**。v1 三处改动已由 builder 落线一轮（SRCS cbin_font.c+验收件、INCLUDE_DIRS 注入、FAS 清单块、外加 builder 自加的 malloc force-include 块）；step-0 红基线 TEST 2 段错误实测（cbin_font_create 32-bit 假设在 64-bit host 直调必崩，dsc 槽 @24 vs @40）+ 双裁 => **cbin_font.c 退出 host 构建**，host 装载半改经 [K2] kidpal_font 编译期宽度感知解码器（冻结验收件已同步修订，头注「预基线修正 2026-10-28」段留痕）。
>
> 本文 = 对**当前已落线态**的增量，builder step-0 逐字落线（不改文本、不改顺序）；落线后立刻现跑红基线（命令与判读见文末），确认 == 设计态红才准动任何实现。**本补丁不触碰冻结验收件本体，不改 CMakeLists 以外任何文件。**

## 增量 1 — HOST_ACCEPTANCE_SRCS：摘除 cbin_font.c 编入行

当前（约 :317-325）：

```cmake
    # 包 261028-m6-k1-font-assets-runtime-loading（字体资产产线 + assets/cbin 运行时
    # 装载 + host 同源字体 seam；P0 spike）被测实现 + 冻结验收。cbin_font.c = cbin
    # 装载核心（纯 C、host 可编；managed_components 第三方面只读——编入 = 消费非
    # 修改，font_puhui_14_1.c 同款先例，include dir 见 INCLUDE_DIRS 注入行）；seam
    # [K1]-[K4] 弱哨兵强符号源 = display/kidpal_font.cc（BUILDER DUTY 4)，builder
    # 接线）。相对 host_app/main 分别为 ../../../../managed_components/ 与 ../../。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行）。
    "../../../../managed_components/78__xiaozhi-fonts/src/cbin_font.c"
    "../../font_assets_runtime.acceptance.c"
```

改为（注释同步改写，cbin_font.c 行删除）：

```cmake
    # 包 261028-m6-k1-font-assets-runtime-loading（字体资产产线 + assets/cbin 运行时
    # 装载 + host 同源字体 seam；P0 spike）冻结验收。cbin_font.c **不进 host 构建**
    # （预基线修正裁定 2：32-bit 假设装载器在 64-bit host 直调必崩，dsc 槽 @24 vs
    # @40；host 装载半 = kidpal_font 编译期宽度感知解码器，设备面 cbin_font_create
    # 仅设备构建）；seam [K1]-[K4] 弱哨兵强符号源 = display/kidpal_font.cc（BUILDER
    # DUTY 4)，builder 接线）。相对 host_app/main 为 ../../。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行）。
    "../../font_assets_runtime.acceptance.c"
```

## 增量 2 — INCLUDE_DIRS：还原单行（摘除 78 include dir 注入）

当前（约 :336-339）：

```cmake
    INCLUDE_DIRS "." "../../../display"
        # ../../../display: __has_include("char_stroke.h") 契约头探针
        "../../../../managed_components/78__xiaozhi-fonts/include"
        # cbin_font.h 装载面（261028-m6-k1；cbin_font.c 自身 include 也经此解析）
    REQUIRES unity lvgl
```

改为（验收件已不含 cbin_font.h，注入无消费方）：

```cmake
    INCLUDE_DIRS "." "../../../display"  # ../../../display: __has_include("char_stroke.h") 契约头探针
    REQUIRES unity lvgl
```

## 增量 3 — 删除 cbin_font.c 的 malloc force-include 块（builder 自加块，随增量 1 成死块）

整块删除（约 :349-358）：

```cmake
# 包 261028-m6-k1（builder 接线，2026-10-28）: cbin_font.c 是 xiaozhi-fonts 第三方面
# 只读 TU（编入 = 消费非修改）。它自身未 `#include <stdlib.h>` 却直接调用 malloc——
# 设备 32-bit 目标上隐式声明 int 返回值恰好与指针同宽（且设备工具链不把隐式声明当
# 硬错），故线上无恙；host linux（64-bit，GCC14 默认把隐式函数声明当 error）既编不过，
# 且若仅降为 warning，malloc 的返回值会被当 32-bit int 截断 → 堆指针高位丢失 → cbin
# 装载崩溃/数据损坏。正确修法 = 对该 TU force-include <stdlib.h>（正确声明 malloc 返回
# void*），不改第三方文件、不动其它 TU。仅 host test 组件，设备构建零影响。
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../managed_components/78__xiaozhi-fonts/src/cbin_font.c"
    PROPERTIES COMPILE_OPTIONS "-include;stdlib.h")
```

（其机制分析仍有效——但仅适用于设备 32-bit 工具链语义，cbin_font.c 回归设备构建后由设备侧承载，host 构建不再触及该 TU。）

## 增量 4 — FAS 结构清单块三处小改（键语义随双裁更新）

### 4a — gen_fonts.sh 事实组加 provenance 键

`set(FAS_GEN_FONTS_ROOT 0)` 行后加一行：

```cmake
set(FAS_GEN_FONTS_ROOT 0)
set(FAS_GEN_FORMAT_CBIN 0)
```

`if(EXISTS "${FAS_GEN_FONTS}")` 块内，`fonts_root` 的 FIND/if 段之后、`endif()` 之前加：

```cmake
    string(FIND "${_fas_gen}" "--format cbin" _fas_p)
    if(_fas_p GREATER -1)
        set(FAS_GEN_FORMAT_CBIN 1)
    endif()
```

### 4b — host SRCS 事实换负向键

当前：

```cmake
set(FAS_SRCS_CBIN_FONT 0)
set(FAS_SRCS_ACCEPTANCE 0)
set(FAS_SRCS_PUHUI14 0)
string(FIND "${FAS_SRCS_JOINED}" "cbin_font.c" _fas_p)
if(_fas_p GREATER -1)
    set(FAS_SRCS_CBIN_FONT 1)
endif()
```

改为（语义 = cbin_font.c **不得**在 host SRCS）：

```cmake
set(FAS_SRCS_NO_CBIN_FONT 1)
set(FAS_SRCS_ACCEPTANCE 0)
set(FAS_SRCS_PUHUI14 0)
string(FIND "${FAS_SRCS_JOINED}" "cbin_font.c" _fas_p)
if(_fas_p GREATER -1)
    set(FAS_SRCS_NO_CBIN_FONT 0)
endif()
```

### 4c — 清单输出键两处

`string(APPEND FAS_OUT "\ngen_fonts_sh_bpp1_ge2=${FAS_GEN_BPP1_GE2}")` 行后加：

```cmake
string(APPEND FAS_OUT "\ngen_fonts_sh_format_cbin=${FAS_GEN_FORMAT_CBIN}")
```

`string(APPEND FAS_OUT "\nhost_srcs_cbin_font=${FAS_SRCS_CBIN_FONT}")` 行改为：

```cmake
string(APPEND FAS_OUT "\nhost_srcs_no_cbin_font=${FAS_SRCS_NO_CBIN_FONT}")
```

## 落线后红基线现跑（builder step-0，逐字命令与判读）

```
wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
```

期望（冻结件 Run protocol 同源）：**250 Tests**（242+8）/ 存量红 = p1_auxbar AB-A1/A2/A4 三条 / 新增设计态红 = FAS-A1（清单事实 size16=0 / size20=0 / **format_cbin=0（增量后仍 0，builder 切 fork 后转 1）** / device_cmakelists_fonts_root=0）+ FAS-A2（.bin 未产→前置存在性红）+ FAS-A3/A4/A5（weak 哨兵 seam 未实现）+ FAS-A6（kidpal_font 未落 + **host_srcs_no_cbin_font=0 若增量 1 未摘净**）/ **FAS-A7/A8 恒绿**。除上述外**零其它新增红**；任何 infra 红（清单缺失/键不在/负向探针失败）= 接线缺陷，不冒充设计态红——停下来回报，不硬磕。**特别地：本轮不得再出现段错误/崩溃**——TEST 2 已无 cbin_font_create 直调（[K2] 弱哨兵返回 NULL 走 fas_require_handle 解析性红）。

## acceptance-author 已核验的清单事实值（增量 4 落线后 configure 即产）

gen_fonts_sh_exists=1 / size16=0（设计态红）/ size20=0（设计态红）/ bpp1_ge2=1（现 3 行）/ **format_cbin=0（设计态红；builder 按 BUILDER DUTY 1) 切 78 fork --format cbin 后转 1）** / fonts_root_out=1；device_cmakelists_fonts_root=0（设计态红）；kidpal_font_h_exists=0（设计态红）/ kidpal_font_cc_exists=0（设计态红）；**host_srcs_no_cbin_font=0（增量 1 摘除前）/ 1（摘除后，恒绿义务）** / host_srcs_acceptance=1 / host_srcs_puhui14=1；font_test_10..32 九档 exists=1 且 in_cmake=1（设备 CMakeLists :101-109）；size_test_page exists/in_cmake=1；render_node_puhui14=1、render_node_p1_glyph=1；p1cjk_acceptance_exists=1、p1cjk_s1_signature=1（:383）；schedule/story 四常量=1（schedule_page.h :31/:36、story_page.h :163/:170）。

---

## 附注（planner 落盘时加）

红基线期望的 Tests 数 = 242+8；legacy tooth 0 删除是同一文件内 tooth 数变化，TEST_CASE 总数仍 8，Suite 总数不变。
