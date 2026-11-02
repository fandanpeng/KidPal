# acceptance-wiring-patch · 261028-m6-k3-p3-story-typography
> 作者 = acceptance-author（2026-11-02 冻结轮）；落线责任 = builder step-0。
> 六处增量逐字落线；含一处既有键退役换锚（story_reader_lines4 旧键退役，与
> font_assets_runtime.acceptance.c FAS-A8 tooth 3 收紧型 supersede〔REVISION LOG
> revision 3〕配对）。本补丁不触碰冻结验收件本体。

## 增量 1 — HOST_ACCEPTANCE_SRCS
在 `"../../schedule_grid_16px.acceptance.c"` 条目之后、host_infra_smoke 之前插入：
```cmake
    # 包 261028-m6-k3-p3-story-typography（故事列表/阅读 16px/20px 排版：列表行/
    # 阅读正文直连 P0 [K4] kidpal_font_body + 阅读标题 kidpal_font_title 20px 居中
    # 单行省略 + 长标题省略不压日期列 + 自然行 4→3 + 未变更页态 14px 负半——
    # F-1/F-4/F-6 + 目的2/4 故事半）冻结验收。行为齿强符号源 = display/story_page.c
    # + display/kidpal_font.cc（均已在册）；story_page.h 常量直读（include 绑定）。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行）。
    "../../story_typography.acceptance.c"
```

## 增量 2 — FAS 路径常量
`set(FAS_SCHEDULE_C  "${FAS_DISPLAY}/schedule_page.c")` 行后追加：
```cmake
set(FAS_STORY_C     "${FAS_DISPLAY}/story_page.c")
set(FAS_MCP_CC      "${FAS_FIRMWARE_MAIN}/mcp_server.cc")
```

## 增量 3 — host SRCS 事实
`FAS_SRCS_SCHED_GRID_16PX` 判定段后追加：
```cmake
set(FAS_SRCS_STORY_TYPO 0)
string(FIND "${FAS_SRCS_JOINED}" "story_typography.acceptance.c" _fas_p)
if(_fas_p GREATER -1)
    set(FAS_SRCS_STORY_TYPO 1)
endif()
```

## 增量 4 — 内容事实组（含既有键退役换锚）
4a. 删除既有行（其输出行 story_reader_lines4 由增量 6a 一并退役）：
```cmake
fas_fact_contains(FAS_KSTORY_LINES "${FAS_STORY_H}" "#define KSTORY_READER_LINES_PER_SCREEN 4")
```
（FAS_KSTORY_ROWS / 输出键 story_list_rows4 保留不动。）
4b. `FAS_SCHED_H_BANDS` 判定段后追加：
```cmake
# 261028-m6-k3-p3-story-typography：story 排版 16px/20px/自然行 3（键语义见
# story_typography.acceptance.c 头注 [M] 节；旧 story_reader_lines4 键退役换锚
# = font_assets_runtime FAS-A8 tooth 3 REVISION LOG revision 3）。
fas_fact_contains(FAS_STORY_C_BODY_SEAM  "${FAS_STORY_C}" "kidpal_font_body")
fas_fact_contains(FAS_STORY_C_TITLE_SEAM "${FAS_STORY_C}" "kidpal_font_title")
fas_fact_contains(FAS_STORY_C_PUHUI14    "${FAS_STORY_C}" "font_puhui_14_1")
fas_fact_contains(FAS_STORY_H_LINES3     "${FAS_STORY_H}" "#define KSTORY_READER_LINES_PER_SCREEN 3")
fas_fact_contains(FAS_STORY_H_LINES4_A   "${FAS_STORY_H}" "KSTORY_READER_LINES_PER_SCREEN 4")
fas_fact_contains(FAS_STORY_H_LINES4_B   "${FAS_STORY_H}" "KSTORY_READER_LINES_PER_SCREEN  4")
file(READ "${FAS_STORY_C}" _fas_story_c_buf)
string(REGEX MATCHALL "LV_LABEL_LONG_DOT" _fas_story_dot "${_fas_story_c_buf}")
list(LENGTH _fas_story_dot FAS_STORY_C_DOT_N)
file(READ "${FAS_MCP_CC}" _fas_mcp_buf)
string(REGEX MATCHALL "KSTORY_READER_LINES_PER_SCREEN" _fas_mcp_lines "${_fas_mcp_buf}")
list(LENGTH _fas_mcp_lines FAS_MCP_LINES_N)
if(FAS_STORY_H_LINES4_A EQUAL 0 AND FAS_STORY_H_LINES4_B EQUAL 0)
    set(FAS_STORY_H_LINES_NOT4 1)
else()
    set(FAS_STORY_H_LINES_NOT4 0)
endif()
if(FAS_STORY_C_DOT_N GREATER_EQUAL 3)
    set(FAS_STORY_C_DOT_GE3 1)
else()
    set(FAS_STORY_C_DOT_GE3 0)
endif()
if(FAS_MCP_LINES_N GREATER_EQUAL 2)
    set(FAS_MCP_LINES_SYNC 1)
else()
    set(FAS_MCP_LINES_SYNC 0)
endif()
```

## 增量 5 — CMAKE_CONFIGURE_DEPENDS
既有 set_property 追加：`"${FAS_STORY_C}" "${FAS_MCP_CC}"`（FAS_STORY_H 已在清单）。

## 增量 6 — 清单输出键
6a. 删除：`string(APPEND FAS_OUT "\nstory_reader_lines4=${FAS_KSTORY_LINES}")`
6b. `sched_h_band_anchors` 行后追加：
```cmake
string(APPEND FAS_OUT "\nhost_srcs_story_typography=${FAS_SRCS_STORY_TYPO}")
string(APPEND FAS_OUT "\nstory_c_font_body_seam=${FAS_STORY_C_BODY_SEAM}")
string(APPEND FAS_OUT "\nstory_c_font_title_seam=${FAS_STORY_C_TITLE_SEAM}")
string(APPEND FAS_OUT "\nstory_c_puhui14_kept=${FAS_STORY_C_PUHUI14}")
string(APPEND FAS_OUT "\nstory_c_dot_ge3=${FAS_STORY_C_DOT_GE3}")
string(APPEND FAS_OUT "\nstory_h_reader_lines3=${FAS_STORY_H_LINES3}")
string(APPEND FAS_OUT "\nstory_h_reader_lines_not4=${FAS_STORY_H_LINES_NOT4}")
string(APPEND FAS_OUT "\nstory_mcp_lines_sync=${FAS_MCP_LINES_SYNC}")
```

## 已核验的清单事实值（设计态现状）
body/title seam 两键=0 红（设计态）；puhui14_kept=1 绿；lines3=0 红 + lines_not4=0 红（设计态）；LV_LABEL_LONG_DOT 现役 2 → dot_ge3=0 红（设计态）；mcp 符号引用现役 3（:691/:1880 注释/:1885）→ story_mcp_lines_sync=1 绿；story_list_rows4=1 绿；接线键缺 = infra 红。
