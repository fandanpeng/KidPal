---
changeId: 260929-char-window-v5-trim
rigor: lite
riskTier: medium
sourceType: human-ad-hoc
reviewed-by: architect（/triage-change 分流 review 2026-09-29，②单包 Lite 裁决 + reviewed plan；human 触点①确认路由并改判框方案=重烘资产）
date: 2026-09-29
spine: delivery/changes/intake/260929-char-window-v5-trim.md
storyRefs: "S-F3.2 · 看拼音/笔画/部首/释义（定稿 v1.60）"
---

# plan · 260929-char-window-v5-trim 查字窗视觉收敛（去装饰星 / 去组词行 / 框重烘圆角矩形 / 右栏让位）

> reviewed plan 由 architect 分流 review 产出、主线程按 change-triage §4 只搬运落盘（框方案一项按 human 触点① 2026-09-29 裁定由「程序化画框（推荐案）」改判为「重烘 frm_236_236 资产」，其余未动）。需求面已先行定稿 v1.60（spine anchors.req 已回填，launchPrecondition 满足）。

## 为什么改（human 意图）

真机轮四件实测（2026-09-28 真机照片）：① 右栏底部装饰星在屏，human 裁去掉；② 组词行运行时数据恒空（OpenClaw 最小意图块只带 char+meaning、补充源未提供 words），实际呈现即释义行，human 裁把组词改成语义（卡面收敛四行）；③ 米字格外框设计母版 v5 已改正圆角矩形，实现仍是过时的手绘斜框资产（frm_236_236.bin），且米字引导线灰度（0x99/0xbb）在 RLCD 阈值化后不可见，真机呈「空斜框」；④ 右栏文本左缘被框右缘压住（cardlabel x228 < 框右缘 235）。

需求面 v1.60 已收敛口径：S-F3.2 四行（拼音带声调/共 N 画/部首/释义）+ 五处枚举同步 + §8 已砍登记 + 实现侧对账 ⑥。本包收实现侧账。

## 改动面（文件级）

### 1. `firmware/main/display/char_page.cc`

- **①删装饰星**：`s_cp_star_dsc/px/st` 三静态 + `charpage_art_star_dsc()` 整函数（~:486-524）+ show() 内消费块（~:648-658）。全树零其他消费、活测试零齿——干净删除。
- **②删组词行**：words 解析/join 块（~:676-718）整删、拼装 words 行（~:767-769）删、默认值与 `info_o.words` 改传 `""`（card_build 非 NULL 要求由空串满足，冻结面零触碰）、五行口径注释全部改四行（拼音/共 N 画/部首/释义）。
- **③框改圆角矩形（重烘资产案，human 触点①裁）**：保留 `charpage_art_frame_dsc()`/`charpage_art_overlay()` 消费链不动代码形状，**重烘 `frm_236_236.bin`** 为圆角矩形（对齐设计母版 v5：rbox 圆角 ~14px、边框纯黑、白底内芯透明语义不变）。资产管线出图后须过 1bpp 阈值化目检——圆角处不得出锯齿/断线（阈值化伪影是重烘案已知风险，出图即检、不合格回管线调参）。**连带义务：spiffs_root 资产变更必须 app+art 双分区烧录**（K4 教训：只烧 ota_0 新资产不生效）——真机轮烧录纪律入包。
- **③a 米字引导线保留改黑**（human 触点①裁）：`charpage_draw_grid`（~:225-239）十字/对角灰度 0x99/0xbb 改纯黑点线（RLCD 阈值化后可见）；点距/虚实参数真机轮调。设计口径 = 母版 v5 框内八向点线保留。
- **④右栏让位**：cardlabel 起点 x 228 → 清出框右缘（architect 建议 244 / width 160→144，或对齐母版 262/126——数值终判归 builder + 真机轮，原则 = 文本左缘不与框右缘重叠）。
- canvas 自带 1px 0xbb 边框（~:611-612）随重烘框接管处理去留（builder 裁：框资产已含边则删，避免双框）。

### 2. `firmware/main/test/char_window_overlay.acceptance.c`（acceptance-author 正规 authorship）

- test 10（MCD-A1）：width 齿 160 → 新值（可加 x 清让齿）；canvas (4,8) 扫描器自检齿不动。
- test 11（MCD-A3/A4）：删 A4 三齿（拼装区 words[0] 判空闸缩为 radical[0]、解析区「、」/ for+GetArraySize 遍历齿删）；A3 释义行/缺义兜底齿原样保绿。
- **ACC-4 区域末端锚换锚**：现以 `static const lv_image_dsc_t *charpage_art_frame_dsc(void)` 签名为切取末端锚（~:1915）——本包保留该函数（重烘案不删），核对锚仍有效即可；若 builder 重构改名则同批换锚。
- 增负向齿（建议）：char_page.cc 不含 ic_star_22 消费、拼装区无 words 拼装。
- REVISION LOG 追加（append-only）：如实记 MCD-A4 组词 join（2026-09-20 落地）九天后整组回撤——**依据 = human 2026-09-29 真机轮裁定（运行时恒空、口径收敛），非缺陷修复**。

### 3. 资产管线（重烘案新增面）

- `frm_236_236.bin` 重烘为圆角矩形（源 = 设计母版 v5 查字窗框段）；管线入口在 tools/ 资产管线，出图替换 spiffs 资产。
- `ic_star_22.bin` 成孤儿资产：本包不删文件（烧录面最小变更），账面注记（ui-v5-align findings §3 资产账 + 收口 summary）。

## 明确不动

- `char_stroke.c`/`char_stroke.h`（U 冻结纯逻辑合约）——**kidpal_char_card.words 成员裁定保留**（删成员触冻结面 + char_stroke.acceptance.c :1022/:1033/:1061 verbatim 齿，零行为收益换大爆炸半径；死成员清理登记 residual 后续包）。
- `server/**` 零改（payload words 继续下发，契约面不动）。
- `docs/contracts/**` 零触（分流 review 四分类=无需改）。
- keybar/application/mcp 路由零触。

## impactedTests

- host runner 相关子集：char_window_overlay.acceptance.c（修订）+ char_stroke.acceptance.c（must-stay-green 零改）+ char_window.test.c（零改确认）。
- 真机轮目检四项：无星、右栏四行、圆角框无斜边无锯齿、米字黑点线可见、右栏不被压；**烧录 = app+art 双分区**。

## 残余登记

- 设计母版 v5 HTML 查字窗段仍画组词行/星（spine openQ q3 deferred）——收口阶段随 ui-align 账面注记一并呈 human 裁母版更新。
- 设计稿与 ui-v5-align findings §2 行 4 / §3 资产账随收口注记（frm_236_236 重烘后形状对齐 v5、ic_star_22 转孤儿）。
