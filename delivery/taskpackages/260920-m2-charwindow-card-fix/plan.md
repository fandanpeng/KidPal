---
rigor: lite
riskTier: medium
sourceType: human-confirmed-plan
reviewed-by: architect
date: 2026-09-20
---

# 260920-m2-charwindow-card-fix · 查字窗右栏真机修复（字体接线 + 释义行上屏）

> 来源：human 真机确认轮 finding（查「勇」右栏整栏隐身 + 释义须上屏）。spec v1.56 + 勘误已落账（`a24a718` / `5982d82`，human 授权直改、如实记录跳过 Track B）。
> 本 plan = architect /triage-change 比例化 review（2026-09-20）的 reviewed plan 搬运，human 触点①确认：② 单包 Lite / medium、font_puhui_14_1 显式接线、meaning 不纳入 validate 闭集。主线程只搬运不改内容。

## 背景（真机 finding + 根因）

- **现象**：真机查「勇」时右栏速查栏与底部提示条整栏未上屏；TTS 播报（引导语 + 逐笔笔名 + 组词）正常。
- **根因（architect 对磁盘核验）**：`char_page.cc:555` 是**全树唯一**在中文 label 上显式钉 `LV_FONT_DEFAULT`（= 内置 montserrat_14，纯拉丁、无汉字与带声调韵母字形）的中文 label；提示条 label 无显式字体（s_kidpal_scr 字体继承存在未解张力，不赌继承）。
- **机制选型（human 已裁）**：显式接线编译内置全量 14px 普惠体 `font_puhui_14_1`（`lcd_display.cc` 键帽条左右槽 + 语音轮 header/echo/answer 五处真机验证同款先例）。**font_body 运行时接线 = RP-A4 已裁 residual 后续包，不在本包**（`lv_font_load` 全树零运行时调用——v1.56 对账④原表述已勘误，`5982d82`）。
- **释义数据**：D3 契约 view=char `data.meaning` 字段既有、OpenClaw 生产端（`char_stroke_lib.py` 取词典 definition）已在下发——零契约改动、零云面改动。
- **降级口径（human 已裁）**：`meaning` 不纳入 validate 必需键闭集（缺释义只损右栏一行、不构成盲渲染风险；旧 payload 整窗降级 = 行为回归）；缺失时右栏兜底「释义：—」占位，形态钉进齿不留现场裁量。

## 改动面清单（architect reviewed plan 原文）

1. **`firmware/main/display/char_stroke.h`**：`kidpal_char_info` 增 `const char *meaning`（**NULL 容忍**，与 pinyin/radical/words 的必非空语义不同——meaning 是新增可选面）；`kidpal_char_card` 增 `char meaning[96]`（容量对齐 words 惯例）。注意头注「本头声明与冻结验收 fallback 块逐字一致」的自述已过时（在世 M2-K1 件无该块、归档件不编入）——本包在 REVISION LOG 留痕即可，不顺手改冻结语义。
2. **`firmware/main/display/char_stroke.c`** `kidpal_char_card_build`：meaning 不进 NULL 拒绝集；`info->meaning == NULL` → `out->meaning[0] = '\0'` 且 rc=0（缺义不降级整窗）；非空 → snprintf 截断安全拷贝。**parse/raster/anim/card 既有四字段 + progress 零漂移**（M2-K1 冻结面零回归，该件 test 2 参照表守卫兜底；实测 M2-K1 验收件不含 card_build 齿，扩展与其零交集）。
3. **`firmware/main/display/char_page.cc`**：① `LV_FONT_DECLARE(font_puhui_14_1)` + cardlabel 字体设值从 `LV_FONT_DEFAULT` 改 `&font_puhui_14_1`；hint_label 同款显式设值（不赌继承）。② cJSON 解析区取 `meaning`（防御消费：缺 → 兜底，参照既有 "?" 先例）；③ 速查卡 snprintf 四行 → 五行（加「释义：%s」；meaning 空时兜底「释义：—」占位）；④ **两处 positional initializer**（:312 charpage_refresh_hint、:542 show 内）同步补 meaning 成员——建议改 designated initializer 形态（`.words = ...`），或至少 plan 明列两处。⑤ `charpage_refresh_hint` 装配区补 meaning。
4. **`firmware/main/mcp_server.cc` case 3：零改动（human 已裁）**。validate 六键闭集不动、degrade_reason 文案不动（K3 冻结齿锚 `kidpal_char_data_validate(` / route=char 在世守卫零触碰）。
5. **`firmware/main/test/char_window_overlay.acceptance.c`**：REVISION LOG 留痕修订（tighten-only）——A5 覆盖随 spec v1.56 四行→五行；A5a 区末端锚 `lv_obj_set_style_text_font(cardlabel` 若被改写须同步锚修订（只改实参不动锚字面则锚存活）。

## 关键风险（architect 原文）

- **R1 字形覆盖（真机腿）**：font_puhui_14_1 对带声调韵母（ǔ/ǒ/ò…）的覆盖未实证——字表 ≈6813 字形是否含带调拉丁无据；font_body（GB2312 全集含拼音区）必有但接线是 residual 包。A7 真机腿必验「yǒng」实渲；缺字 → 升级 font_body 接线包，本包不预支。
- **R2 冻结件纪律**：char_stroke 扩展 additive-only；K3 件修订 tighten-only、负向齿与锚齿不剥注释（sealing pass 纪律）。
- **R3 链接可达性**：font_puhui_14_1 定义在 xiaozhi-fonts 组件，lcd_display.cc（同 main 组件）先例已证可达——builder 验证编译即可，host 面无此依赖（char_page.cc 非 host 编译，结构齿扫源码承载）。
- **R4 host/真机边界诚实**：host 无 SPIFFS/内置字体字形与真机恒不等（HOST_TEST_CONVENTIONS「字体现实」节明文）——齿只钉接线与数据面行为，字形终审归 A7。

## 验收齿形态（architect 原文）

- **行为半（host runner 真调 char_stroke.c 强符号）**：card_build meaning 三态——非空 verbatim 截断安全 / NULL → 空 + rc=0 / 既有字段与 progress 文案零回归。
- **结构半（源级树扫，注释剥离后计数——sealing pass 先例）**：① **负向齿**：char_page.cc `LV_FONT_DEFAULT` 作 text_font 实参出现数 == 0（**现状红 = 真实缺陷信号，RED→GREEN 可判**——本次防伪齿的核心）；② **正向齿**：cardlabel 与 hint_label 两个 label 创建/设值区各有 `&font_puhui_` 字面在世（剥离注释后）；③ 五行拼装区含 `card.meaning` 实参；④ refresh_hint 装配区含 meaning。
- **真机腿 deferred（M2K1-A4 先例形态，不造 host 伪断言）**：A7 复验 = 右栏五行可见 + 带调拼音字形 + 提示条可见 + 顺带 schedule/story 中文可见性一句取证（对账④对比勘误证据）。

## 依赖与顺序（architect 原文）

acceptance-author 先钉齿（含 K3 件 REVISION LOG 修订、RED baseline 实跑含新负向齿现状红）→ builder（char_stroke 数据面 → char_page 装配面，CMake 零新增接线）→ verifier↔fixer → integrator → supervisor；A7 真机腿归 human 持板轮（单真机不两轮撞车纪律沿用）。mutation 义务：撤 font_puhui 接线 → 结构齿红；撤 card_build meaning → 行为齿红。

## 铁律约束

- 零契约改动（D3 `meaning` 既有）、零 spec 改动（v1.56 + 勘误已落，本包只消费）、零云面改动（`server/` 零触碰）、零 agent 定义/gate 改动。
- 冻结件修订只走 acceptance-author 留痕（additive / tighten-only）；builder 零写权。
- mcp_server case 3 与 validate 闭集零改动（human 已裁）。
