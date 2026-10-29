# acceptance-wiring-patch · 261028-m6-k2a-p1-home-16px

> 作者 = acceptance-author（2026-10-29 冻结轮；host CMakeLists 写边界被拦，增量交 planner 落盘）；落线责任 = **builder step-0**（P0 先例：human 2026-10-28 裁定延续）。本文 = `firmware/main/test/host_app/main/CMakeLists.txt` 六处增量，逐字落线（不改文本、不改顺序）；落线后按冻结件头注「RED baseline 判读口径」通道③现跑（**注意：P0-chain 缺陷修复前，通道③的判读含 P0 链在册红**——见 taskpackage notes 升级项）。**本补丁不触碰冻结验收件本体。**

## 增量 1 — HOST_ACCEPTANCE_SRCS

在 `"../../font_assets_runtime.acceptance.c"` 条目之后、host_infra_smoke 之前插入：

```cmake
    # 包 261028-m6-k2a-p1-home-16px（首页 P1 日程卡 16px：[S7] 字面源缝 16px body
    # 切换 + [G1] text_yoff 16px 重基线 + [U1] 临时事件 ※ 前缀退役改事件名下 2px
    # 下划线——F-1/F-2/F-9 首页半）冻结验收。行为齿强符号源 = display/home_p1.c
    # （已在册）；[S6]/[S7] seam 强符号源 = host_infra_seam.c（BUILDER DUTY 3)）。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行）。
    "../../p1_home_16px.acceptance.c"
```

## 增量 2 — FAS 路径常量

`set(FAS_P1CJK ...)` 行后追加：

```cmake
set(FAS_HOME_P1_C   "${FAS_DISPLAY}/home_p1.c")
set(FAS_HOME_P1_H   "${FAS_DISPLAY}/home_p1.h")
set(FAS_P1FONT_H    "${FAS_DISPLAY}/kidpal_p1_font.h")
set(FAS_HOST_SEAM   "${CMAKE_CURRENT_SOURCE_DIR}/host_infra_seam.c")
```

## 增量 3 — host SRCS 事实

`FAS_SRCS_PUHUI14` 判定段后追加：

```cmake
set(FAS_SRCS_P1_HOME_16PX 0)
string(FIND "${FAS_SRCS_JOINED}" "p1_home_16px.acceptance.c" _fas_p)
if(_fas_p GREATER -1)
    set(FAS_SRCS_P1_HOME_16PX 1)
endif()
```

## 增量 4 — 内容事实组（负向键语义 = 不扩临时标记字段）

`FAS_KSTORY_LINES` 行后追加：

```cmake
fas_fact_contains(FAS_P1FONT_H_S7  "${FAS_P1FONT_H}" "kidpal_p1_font_face")
fas_fact_contains(FAS_RN_S7        "${FAS_RENDER_NODE}" "kidpal_p1_font_face")
fas_fact_contains(FAS_SEAM_S7      "${FAS_HOST_SEAM}" "kidpal_p1_font_face")
fas_fact_contains(FAS_HOME_YOFF16  "${FAS_HOME_P1_C}" "const int text_yoff = 3")
fas_fact_contains(FAS_HOME_MARKER  "${FAS_HOME_P1_C}" "0x203B")
fas_fact_contains(FAS_HOME_H_FLAGS "${FAS_HOME_P1_H}" "flags")
if(FAS_HOME_H_FLAGS EQUAL 0)
    set(FAS_P16_H_NO_TEMP 1)
else()
    set(FAS_P16_H_NO_TEMP 0)
endif()
```

## 增量 5 — CMAKE_CONFIGURE_DEPENDS

既有 set_property 追加：`"${FAS_HOME_P1_C}" "${FAS_HOME_P1_H}" "${FAS_P1FONT_H}" "${FAS_HOST_SEAM}"`。

## 增量 6 — 清单输出键

`story_reader_lines4` 行后追加：

```cmake
string(APPEND FAS_OUT "\nhost_srcs_p1_home_16px=${FAS_SRCS_P1_HOME_16PX}")
string(APPEND FAS_OUT "\np1_kidpalfont_h_s7_decl=${FAS_P1FONT_H_S7}")
string(APPEND FAS_OUT "\np1_rn_s1_face_seam=${FAS_RN_S7}")
string(APPEND FAS_OUT "\np1_hostseam_s1_face_seam=${FAS_SEAM_S7}")
string(APPEND FAS_OUT "\np1_home_text_yoff16=${FAS_HOME_YOFF16}")
string(APPEND FAS_OUT "\np1_home_c_marker_const=${FAS_HOME_MARKER}")
string(APPEND FAS_OUT "\np1_home_p1_h_no_temp_field=${FAS_P16_H_NO_TEMP}")
```

## 已核验的清单事实值（设计态）

text_yoff=4 在世 → p1_home_text_yoff16=0 红；0x203B 零命中 → marker_const=0 红；home_p1.h 无 "flags" → 负向键=1 绿；kidpal_p1_font_face 三落点全零 → 三键=0 红；既有键零漂移。
