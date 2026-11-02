# acceptance-wiring-patch · 261028-m6-k2b-p2-schedule-16px

> 作者 = acceptance-author（2026-11-02 冻结轮；host CMakeLists 写边界不在 acceptance-author 授权面，增量交 planner/builder 落线——P1 先例：261028-m6-k2a-p1-home-16px 同名文件同法）；落线责任 = **builder step-0**。本文 = `firmware/main/test/host_app/main/CMakeLists.txt` 六处增量，逐字落线（不改文本、不改顺序）；落线后按冻结件 `firmware/main/test/schedule_grid_16px.acceptance.c` 头注「RED baseline 判读口径」通道③现跑（判读 = taskpackage redBaselineResult 红名单逐条对照，不数总数——P1 retro F4 教训）。**本补丁不触碰冻结验收件本体。**

## 增量 1 — HOST_ACCEPTANCE_SRCS

在 `"../../p1_home_16px.acceptance.c"` 条目之后、host_infra_smoke 之前插入：

```cmake
    # 包 261028-m6-k2b-p2-schedule-16px（课表网格 16px：课名直连 P0 [K4]
    # kidpal_font_body + pitch 19 + 槽位带 78/78/69 + 容量 4/4/3 静默截断 + 临时课
    # 课名下 2px 下划线/※ 退役/★ 不变——F-1/F-3/F-8/F-9 课表半）冻结验收。行为齿
    # 强符号源 = display/schedule_page.c（已在册，本包同文件改造）；零新 seam 头
    # 文件（schedule_page.c 为 LVGL-ful TU，直接消费 [K4]——seam 形态决策见冻结件
    # 头注 [F2]）。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行）。
    "../../schedule_grid_16px.acceptance.c"
```

## 增量 2 — FAS 路径常量

`set(FAS_HOST_SEAM ...)` 行后追加：

```cmake
set(FAS_SCHEDULE_C  "${FAS_DISPLAY}/schedule_page.c")
```

（`FAS_SCHEDULE_H` 既有；本包无新头文件。）

## 增量 3 — host SRCS 事实

`FAS_SRCS_P1_HOME_16PX` 判定段后追加：

```cmake
set(FAS_SRCS_SCHED_GRID_16PX 0)
string(FIND "${FAS_SRCS_JOINED}" "schedule_grid_16px.acceptance.c" _fas_p)
if(_fas_p GREATER -1)
    set(FAS_SRCS_SCHED_GRID_16PX 1)
endif()
```

## 增量 4 — 内容事实组（负向键语义 = 旧值/退役面缺席）

`FAS_P16_H_NO_TEMP` 判定段后追加：

```cmake
fas_fact_contains(FAS_SCHED_FONT_SEAM  "${FAS_SCHEDULE_C}" "kidpal_font_body")
fas_fact_contains(FAS_SCHED_PITCH_M    "${FAS_SCHEDULE_C}" "#define KSG_ROW_PITCH")
fas_fact_contains(FAS_SCHED_PITCH16_A  "${FAS_SCHEDULE_C}" "KSG_ROW_PITCH  16")
fas_fact_contains(FAS_SCHED_PITCH16_B  "${FAS_SCHEDULE_C}" "KSG_ROW_PITCH 16")
fas_fact_contains(FAS_SCHED_TEMP_CP_A  "${FAS_SCHEDULE_C}" "0x203B")
fas_fact_contains(FAS_SCHED_TEMP_CP_B  "${FAS_SCHEDULE_C}" "U+203B")
fas_fact_contains(FAS_SCHED_TEMP_FB    "${FAS_SCHEDULE_C}" "ksg_marker_temp_proc")
fas_fact_contains(FAS_SCHED_STAR       "${FAS_SCHEDULE_C}" "0x2605")
fas_fact_contains(FAS_SCHED_H_BAND_A   "${FAS_SCHEDULE_H}" "y[30..107]")
fas_fact_contains(FAS_SCHED_H_BAND_B   "${FAS_SCHEDULE_H}" "y[186..254]")
if(FAS_SCHED_PITCH16_A EQUAL 0 AND FAS_SCHED_PITCH16_B EQUAL 0)
    set(FAS_SCHED_PITCH_NOT16 1)
else()
    set(FAS_SCHED_PITCH_NOT16 0)
endif()
if(FAS_SCHED_TEMP_CP_A EQUAL 0 AND FAS_SCHED_TEMP_CP_B EQUAL 0)
    set(FAS_SCHED_NO_TEMP_CP 1)
else()
    set(FAS_SCHED_NO_TEMP_CP 0)
endif()
if(FAS_SCHED_TEMP_FB EQUAL 0)
    set(FAS_SCHED_NO_TEMP_FB 1)
else()
    set(FAS_SCHED_NO_TEMP_FB 0)
endif()
if(FAS_SCHED_H_BAND_A GREATER 0 AND FAS_SCHED_H_BAND_B GREATER 0)
    set(FAS_SCHED_H_BANDS 1)
else()
    set(FAS_SCHED_H_BANDS 0)
endif()
```

## 增量 5 — CMAKE_CONFIGURE_DEPENDS

既有 set_property 追加：`"${FAS_SCHEDULE_C}"`（`FAS_SCHEDULE_H` 已在既有清单；本包无新头文件，故只增一件）。

## 增量 6 — 清单输出键

`p1_home_p1_h_no_temp_field` 行后追加：

```cmake
string(APPEND FAS_OUT "\nhost_srcs_schedule_grid_16px=${FAS_SRCS_SCHED_GRID_16PX}")
string(APPEND FAS_OUT "\nsched_c_font_seam=${FAS_SCHED_FONT_SEAM}")
string(APPEND FAS_OUT "\nsched_c_pitch_macro=${FAS_SCHED_PITCH_M}")
string(APPEND FAS_OUT "\nsched_c_pitch_not16=${FAS_SCHED_PITCH_NOT16}")
string(APPEND FAS_OUT "\nsched_c_no_temp_cp=${FAS_SCHED_NO_TEMP_CP}")
string(APPEND FAS_OUT "\nsched_c_no_temp_fallback=${FAS_SCHED_NO_TEMP_FB}")
string(APPEND FAS_OUT "\nsched_c_star_kept=${FAS_SCHED_STAR}")
string(APPEND FAS_OUT "\nsched_h_band_anchors=${FAS_SCHED_H_BANDS}")
```

（复用键 `schedule_ksg_course_cap` / `schedule_ksg_font_min_px` / `fas_manifest_version` 既有，零增量。）

## 已核验的清单事实值（设计态，2026-11-02 冻结时点现状）

`kidpal_font_body` 在 schedule_page.c 零命中 → sched_c_font_seam=0 红；`#define KSG_ROW_PITCH  16` 在世 → sched_c_pitch_not16=0 红；`0x203B`（ksg_draw_marker 调用实参）与 `U+203B`（注释）在世 → sched_c_no_temp_cp=0 红；`ksg_marker_temp_proc` 在世 → sched_c_no_temp_fallback=0 红；`0x2605` 在世 → sched_c_star_kept=1 绿；schedule_page.h 无新带字面 → sched_h_band_anchors=0 红；`KSG_COURSE_CAP 84` / `KSG_FONT_MIN_PX 14` 在世 → 两复用键=1 绿；接线键缺 = infra 红（增量 1/3 未落线时）。
