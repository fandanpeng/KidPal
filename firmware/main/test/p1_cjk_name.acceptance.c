/*
 * Frozen acceptance tests — TaskPackage 260921-m3-k4-p1-cjk-dynamic-text (subsystem:
 * firmware; M3-K4 P1; 首页 P1 动态日程名 CJK 渲染 · mergeOrder 36 · rigor=lite /
 * riskTier=medium)。
 *
 * 本文件为**追加轮修订版**（2026-09-22 第二轮冻结）：主轮已合并（main f8703d5、
 * 80 齿全绿），现处合并后 live 轮 FAIL 修复 + human 范围扩展追加轮。追加轮权威 =
 * human 拍板 2026-09-22（pod-events human-decision「plan-B-decode-fix-plus-wireframe-
 * alignment」，本条即立项 ack）：
 *
 *   (a) A6 live FAIL 修复走**方案 B**：设备强符号去掉 req_raw_bitmap=1，改走 LVGL
 *       官方解码到 A8 静态 draw_buf、阈值 128 化 1bpp、字形盒上限收 32×32；双强符号
 *       （kidpal_render_node.cc + host_infra_seam.c）同改。seam [S1] 契约（签名/语义）
 *       不变——32×32 是对 [S1] 冻结上限 1..64 的**实现收紧**（子集），非契约变更。
 *   (b) 首页「接下来」卡版式对齐线框 WF-F1-01（docs/requirements.md WF-F1-01 原文 +
 *       S-F1.1 AC）：大 imgfont 时间退役；「接下来」= 烘焙图片大字标签（新烘焙资产，
 *       走既有 home_p1 KAP1 烘焙资产管线，[S5]）；下方文字行全部 14px font_puhui_14_1
 *       （B 方案解码后逐字形 blit 承载）并**正常换行**——最近项行（时间 + 课程名）、
 *       后续事项行（逐项 时间 + 名）、末行预告（「明早」烘焙标签 + HH:MM + 首项名，
 *       契约 §1.3⑥ preview 口径：载荷钉 HH:MM、「明早」为设备渲染标签，文案是
 *       「明早」不是「明天」，spec WF-F1-01 v1.21 权威）；倒计时贴纸本轮不做（随时钟
 *       缺口挂账），旧贴纸不再渲染；预告行不再作为独立钉区（旧 {136,220,392,242} 带
 *       退役），并入日程主体区第三行。固定标签「殿后不动」由 human 显式解除。
 *
 * 主轮 story 底稿：首页 P1 动态日程名（课程名/事项名，开放集合 CJK 文本）在「接下来」
 * 卡正常渲染，从「画不出即丢弃」变为「画得出、不越区」；主轮已兑现并合并。本轮在
 * 其上重钉行带 + 新增版式/烘焙资产齿。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；本文件 weak 哨兵在实现
 * 接线进 host runner 后被强定义覆盖。多重 weak 合法、语义一致——schedule_grid
 * 先例；「home =」为冻结落点）:
 *
 *   [S1] 弱符号字形获取 seam（home = firmware/main/display/home_p1.c 弱默认 +
 *        firmware/main/display/kidpal_render_node.cc 设备强符号 + firmware/main/
 *        test/host_app/main/host_infra_seam.c host 强符号）——**签名/语义零改**：
 *
 *     typedef struct {
 *         int w;                     -- 字形墨盒宽 px, 1..64
 *         int h;                     -- 字形墨盒高 px, 1..64
 *         int advance;               -- 笔进 px, >= 0
 *         const unsigned char *bits; -- 1bpp 行主序位图, stride = (w+7)/8,
 *                                       MSB 先行, 1 = 墨; 有效期到本函数下一次调用
 *     } kidpal_p1_glyph_t;
 *
 *     int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out);
 *       返回 0 = 字形可用（*out 已填充）; 非 0 = 不可用（out 不消费）。
 *       out == NULL -> 非 0（防御）。弱默认（home_p1.c 内）恒返回非 0 = 不可用。
 *       设备强符号 = kidpal_render_node.cc：方案 B = font_puhui_14_1（bpp=1 PLAIN）
 *       去 req_raw_bitmap=1，走 LVGL 官方解码到 A8 静态 draw_buf（修复 fdsc->stride=0
 *       跨行连续位流逐行漂移——live 轮 A6 FAIL 根因），阈值 128 化 1bpp，盒上限 32×32。
 *       host 强符号 = host_infra_seam.c：lv_font_source_han_sans_sc_14_cjk 子集同款
 *       方案 B 改造（官方解码 + A8 + 阈值 128）。阈值语义等价性（本文件期望零偏移的
 *       依据）：A8@128 ≡ A4@8 ≡ A2@2 ≡ A1@1（全部 = 50% 覆盖阈值），逐齿期望不变。
 *
 *   [S2] home_p1.c 内（主轮已落，本轮重钉落位）：UTF-8 解码（纯逻辑）+ 文字行逐字形
 *        blit（复用既有钉区 clip / 固定起笔纪律）。**行内时间 = [S1] seam ASCII 字形**
 *        （值承载、host 可探——冻结理由：imgfont 数字路径在 host 无资产时退化为值无关
 *        兜底描边，时间值 diff 齿不可实现；且「下方文字行全部 14px font_puhui_14_1
 *        承载」的统一口径下 ASCII 字形同缝承载）。ASCII time 值变化 -> 行带内 diff。
 *
 *   [S3] 布局钉区域矩形（含端点；布局在位钉，非字体/颜色钉——纯美学留 design
 *        review；与 p1_home_battery.acceptance.c HPB 区钉 / home_p1.c r4 裁定同纪律。
 *        **本轮重钉**：预告行从独立钉区并入主体流式第三行，旧独立带退役为负断言）:
 *         PCN_RECT_CARD        {136, 48, 392, 242}  日程卡区（版式承载硬边界）
 *         PCN_RECT_LABEL       {136, 48, 266,  92}  「接下来」烘焙大字标签带（左锚定）
 *         PCN_RECT_ROW1        {136, 96, 392, 122}  最近项行（时间 + 课程名）
 *         PCN_RECT_ROW2        {136,124, 392, 150}  后续事项行（逐项 时间 + 名；
 *                                                   标准夹具 = 第 2 项；换行续行亦落
 *                                                   ROW2 及以下流式带）
 *         PCN_RECT_ROW3        {136,152, 392, 178}  末行预告（明早标签 + HH:MM + 名；
 *                                                   标准夹具 2 项 + 预告 = 第三行）
 *         PCN_RECT_FLOW        {136,180, 392, 242}  换行/溢出承载带（仅 containment）
 *         PCN_RECT_MINGZAO     {136,152, 180, 178}  明早标签槽（行 3 左段；时间起笔
 *                                                   x>=184 在槽右——TEST 8 tooth 4）
 *       行流语义（冻结）：文字行自 ROW1 起按行流式下排（行距由 builder 按带内落墨），
 *       预告恒为**末行**（跟随事项数浮动；标准夹具下 = 第三行）；长行换行续行落下一
 *       行带；一切卡内墨（标签 + 文字行）不得越 PCN_RECT_CARD（右界 x=392 / 底界
 *       y=242 / 左界 x=136）。
 *       负断言区（版式退役面）:
 *         PCN_RECT_STICKER_NEG {268, 50, 390,  92}  旧倒计时贴纸/大时间右上区——退役，
 *                                                   任何夹具严格零墨（本轮「贴纸不做 +
 *                                                   大时间退役 + 预告不入右上独立区」
 *                                                   三重负断言承载）；
 *         PCN_RECT_PREVIEW_NEG {136,220, 392, 242}  旧预告独立钉区——标准夹具（2 项 +
 *                                                   预告、短名无换行）下严格零墨
 *                                                   （预告已上行第三行；多行溢出夹具
 *                                                   下本带属 FLOW 承载域，不适用）。
 *
 *   [S4] UTF-8 截半尾序列容错（home_upcoming.c:393–396 数据已定形状）：home_upcoming
 *        按字节把 name 截到 31 字节（HP1_NAME_LEN-1），会把 UTF-8 多字节字符截半。
 *        home_upcoming.{c,h} 本包零改，容错解法放 P1 侧：解码器遇不完整尾序列安全停笔
 *        ——截半字符不画、不越界、不崩。TEST 3 以 hp1_item_t 直注该字节形状（行为等价
 *        假设见强度申报；载荷形状零改——本轮纯渲染侧，canary 原样保留）。
 *
 *   [S5] 烘焙固定标签资产 seam（本轮新增；home = home_p1.c 经既有 KAP1 管线消费）：
 *         p1_label_jiexialai.bin   「接下来」大字标签（卡顶左锚定，[S3] LABEL 带）
 *         p1_label_mingzao.bin     「明早」标签（行 3 左槽，[S3] MINGZAO 槽）
 *       资产由 tools 资产管线烘焙落 firmware/main/assets/spiffs_root/（builder 消费
 *       管线，acceptance 不产资产）；home_p1.c 经 kidpal_art_load（p1_blit_art 族）
 *       消费、**不受 s_art_ok 门控**（s_art_ok 只探 imgfont 数字族——固定标签在场义务
 *       独立于数字资产挂载）。文件名为本 seam 冻结面（builder 改名 = 契约协商，走
 *       planner）。host 探针形态：art_assets.c 接进 host runner（BUILDER DUTY 5)）+
 *       本文件夹具私有资产根（仅拷入两枚标签 bin，其余资产维持兜底路径——host runner
 *       其余齿的兜底态零扰动，s_art_ok 恒 0）。
 *
 * ---------------------------------------------------------------------------
 * 契约条目 ↔ 齿 的强度申报（acceptance-evidence.md「契约条目 ↔ 齿 的强度申报」节
 * 体例；消费方 = integrator 与判官按本头注核）:
 *
 *   P1CJK-A1（文字行墨落行带——name 半）
 *     -> TEST 1 tooth 1/3/5 承载：行为级（真帧 diff 定位——name 像素差必须非零且全部
 *        落在对应行带内，diff 对 = 仅 name 文本不同、字节等长）。
 *     -> TEST 6 tooth 1/2/3 承载：机制级（seam 三符号落点）。
 *   P1CJK-A2（超宽 name 不越卡区——主轮「截断」语义随版式演进为「换行承载不越界」）
 *     -> TEST 2 承载：行为级（31 字节极宽名三槽 + 右界带 x[393..399] 严格零墨 + diff
 *        锁卡区 y>=96 域）。换行 vs clip 的机制判别归 TEST 9 tooth 3/4（A10）。
 *   P1CJK-A3（空日程/单项「零墨」pin 回归守卫）
 *     -> 空日程半 = 既有 HPB TEST 1 tooth 5（must-stay-green）——**跨包张力见文件尾
 *        「跨包冲击登记」**；单项半 = TEST 5 A3 段（新版式行带口径重钉）。
 *   P1CJK-A4（UTF-8 截半安全停笔）-> TEST 3 承载：行为级（载荷侧等价假设同主轮）。
 *   P1CJK-A5（字形缺失退化不崩）-> TEST 4 承载：行为级（方案 B 改造不改缺失语义）。
 *   P1CJK-A6（device build 闸 + 真机目测挂设备轮窗口）-> 非 host 齿，闸证据条目见
 *        文件尾；装配事实半 = TEST 6 tooth 3。
 *   P1CJK-A7（「接下来」烘焙大字标签墨在场）-> TEST 7 承载：机制级（home_p1.c 引用
 *        [S5] 冻结资产名 = 走烘焙管线非手绘基元）+ 行为级（资产可加载自检 + LABEL 带
 *        墨 >= 阈值）。弱于契约处显式声明：标签字号/字重/留白不钉（美学留 design
 *        review）；标签笔画渲染质量归真机目测（A6 挂窗）。host 私有资产根只放行两枚
 *        标签 bin，其余资产兜底态零扰动（s_art_ok 恒 0）——标签齿不依赖数字资产。
 *   P1CJK-A8（行内时间值承载）-> TEST 1 tooth 2/4/6 承载：行为级（time 值变化 -> 行带
 *        内 diff 定位）。实现前提 = [S2] 行内时间走 [S1] seam ASCII（冻结理由见 [S2]）。
 *   P1CJK-A9（末行预告构成 + 版式退役负断言）-> TEST 8 承载：行为级（明早标签槽墨
 *        在场 + 贴纸/大时间右上区任何夹具零墨 + 旧预告独立带标准夹具零墨 + 预告时间
 *        落标签槽右侧）。弱于契约处显式声明：明早槽宽 44px（x[136..180]）为实现前提
 *        ——builder 按槽布字；槽内墨阈值 12 为在场下界非形状钉。
 *   P1CJK-A10（版式承载不越卡区 + 换行行为）-> TEST 9 承载：行为级（8 项满载压力
 *        containment + 单项 31×'W' 极宽名换行证据）。几何假设显式化：14px 字形 'W'
 *        advance >= 8px，31 字形 >= 248px > 行 1 时间后可用宽（~213px）——极宽名必换
 *        行，换行证据（ROW1 以下带内墨 >= 阈值）可达；若 host 子集 'W' 缺场（diff 红
 *        名自诊断指认），fixture 降级口径在此登记。载荷上限（HP1_NAME_LEN=32）下单行
 *        CJK 名（10 字 = 140px）+ 时间永不越 257px 列宽——「正常换行」在当前载荷上限
 *        下由极宽 ASCII 名承载，CJK 长名不触换行为几何事实，非齿缺口。
 *   源锚代理档自陈：TEST 6 tooth 1/2/4 与 TEST 7 结构半为源锚代理（符号/资产名存在
 *        性，非行为本体）；其行为本体由 TEST 1-4/7/8/9 兑现，两档并列互证。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: P1CJK-A1 name 墨落行带（diff 定位：row1/row2/row3 各自落墨 + 不越带）
 *             -> TEST 1 tooth 1/3/5（行为半）+ TEST 6 tooth 1/2/3（机制半）
 *   coverage: P1CJK-A8 行内时间值承载（time 值 diff 落行带）-> TEST 1 tooth 2/4/6
 *   coverage: P1CJK-A2 超宽 name 不越卡区（右界带零墨 + diff 锁卡区 y>=96）-> TEST 2
 *             （机制判别换行 vs clip 归 TEST 9 tooth 3/4）
 *   coverage: P1CJK-A3 单项「零墨」pin（新版式行带口径；空日程半 = 既有 HPB 齿，
 *             跨包张力见「跨包冲击登记」）-> TEST 5 A3 段
 *   coverage: P1CJK-A4 UTF-8 截半尾序列安全停笔 -> TEST 3
 *   coverage: P1CJK-A5 字形缺失（弱默认路径）退化不崩 -> TEST 4
 *   coverage: P1CJK-A6 device build 闸 + 真机目测挂设备轮窗口 -> 无 host 齿（闸证据
 *             条目见文件尾）+ TEST 6 tooth 3（device 强符号装配事实半）
 *   coverage: P1CJK-A7 「接下来」烘焙大字标签墨在场 -> TEST 7（结构半 + 行为半）
 *   coverage: P1CJK-A9 末行预告构成 + 版式退役负断言（贴纸/大时间右上区、旧预告带）
 *             -> TEST 8
 *   coverage: P1CJK-A10 版式承载不越卡区（满载压力 containment）+ 换行行为 -> TEST 9
 *   coverage: P1CJK-E2 铁律 canary（hp1_item_t 与 11 个冻结契约函数零偏移 +
 *             home_upcoming 零改语义 + 契约/需求面在世）-> TEST 5 canary 段。
 *             **本轮追记**：追加轮 = 渲染侧纯版式（[S3] 重钉 + [S5] 新资产），载荷
 *             形状零改侧不新增断言——hp1_item_t / 契约函数 / home_upcoming canary
 *             原样保留即为完备面。
 *
 * ---------------------------------------------------------------------------
 * 跨包冲击登记（planner 协调项——本文件只登记不修他包冻结件）：
 *   human 版式扩展使 p1_home_battery.acceptance.c（PHB，v1.4 版式冻结）三齿在实现后
 *   预期转红：HPB TEST 1 tooth 2（倒计时贴纸区在位——贴纸退役）、tooth 4 预告半（预告
 *   上行第三行、旧带零墨）、tooth 5（空日程 next 区零墨——若「接下来」标签空日程仍画，
 *   线框口径下标签为固定卡题）。三齿语义修订须由 planner 派 HPB 冻结作者走修订窗口，
 *   builder/verifier 遇红不得自行放宽。本登记不构成对 HPB 的改动授权。
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) firmware/main/display/home_p1.c：按 [S3] 新钉区重排「接下来」卡版式——烘焙大字
 *      标签（[S5]）+ 行流式文字行（[S2]：行内时间 [S1] seam ASCII + name 逐字形 blit +
 *      正常换行）+ 末行预告（明早标签槽 + HH:MM + 首项名）；退役大 imgfont 时间、倒计时
 *      贴纸、旧预告独立带落位；home_p1.h 冻结面零偏移（hp1_item_t 与 11 个冻结契约函数
 *      不动——E2 canary 在世）。
 *   2) firmware/main/display/kidpal_render_node.cc：[S1] 设备强符号按方案 B 改造
 *      （去 req_raw_bitmap=1、官方解码 A8 draw_buf、阈值 128、盒上限 32×32；
 *      kidpal_p1_font_glyph + font_puhui_14_1 符号面保留——TEST 6 tooth 3）。
 *   3) firmware/main/test/host_app/main/host_infra_seam.c：[S1] host 强符号同款方案 B
 *      改造（阈值等价性见 [S1]——本文件期望零偏移）。
 *   4) tools 资产管线消费（acceptance 不产资产）：烘焙 p1_label_jiexialai.bin /
 *      p1_label_mingzao.bin 落 firmware/main/assets/spiffs_root/（[S5] 冻结名）。
 *   5) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 把
 *      ../../../display/art_assets.c 接进 host runner 源清单（本文件标签齿经
 *      kidpal_art_mount 私有资产根探真烘焙资产；未接线时 mount 走 HPB 弱桩恒败 =
 *      TEST 7 自诊断 infra 红）。未挂载态 art_assets.c 强符号语义 == 既有弱桩
 *      （load/glyph -1、advance -1）——其余齿兜底路径零扰动。p1_cjk_name.acceptance.c
 *      已注册在册，本轮零新增注册面。
 *   6) mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *      子串**（KR-A3 教训——strstr 子串仍命中则变异不红；用 _STUB 类整词替换）；
 *      counterEvidence 红名以本修订版红名为准（taskpackage.mutationEvidence 由
 *      planner 按本文件交付报告回填）。
 *   7) device build 闸（P1CJK-A6）：xiaozhi.bin 构建过 + 体积余量记录；真机目测点
 *      （新版式）= 大字标签可见/行版式与 WF-F1-01 对照/换行不越卡/明早文案（非明天）/
 *      B 方案解码后 1bpp 可读性——挂设备轮窗口。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-
 *      k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *      layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *      （或 cd firmware/main/test/host_app 后 ./build/host_infra_test.elf——结构齿
 *      root 自手持 cwd 定位，两处均可，HPB/KSG 先例同款）
 *   两段式红基线（本修订版冻结时点, builder 未实现追加轮）: 见文件尾
 *      「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* home_p1 冻结契约头（在世实现面；hp1_item_t / hp1_state_t / seam 注入 API 形状
 * 零偏移消费）。 */
#include "../display/home_p1.h"
/* art 资产契约头（[S5] 烘焙标签资产加载自检用；声明 only——本文件不定义 kidpal_art_*）。 */
#include "../display/art_assets.h"

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes / regions（seam [S3]；帧规格 == home_p1.c P1_FB_*，不得偏移）
 * ------------------------------------------------------------------------ */
#define PCN_FRAME_W 400
#define PCN_FRAME_H 300
#define PCN_INK_WHITE 0xFFFFFFul
#define PCN_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 「有墨」行带最小墨像素数（与 HPB 同档下界）。 */
#define PCN_INK_MIN_ELEM 8
/* 「接下来」烘焙大字标签带最小墨（大字 3 字符远超此下界；兜底描边框周长 ~300 亦超
 * ——本齿不靠计数区分烘焙/兜底，机制半由 TEST 7 结构断言（[S5] 资产名引用）承载）。 */
#define PCN_INK_MIN_LABEL 30
/* 「明早」标签槽最小墨（行 3 左槽在场下界，非形状钉）。 */
#define PCN_INK_MIN_MINGZAO 12
/* 换行证据最小墨（ROW1 以下带内，31×'W' 续行下界）。 */
#define PCN_INK_MIN_WRAP 20

/* 区域矩形 {x1, y1, x2, y2} 含端点（seam [S3] 布局在位钉）。 */
static const int pcn_rect_card[4]        = {136,  48, 392, 242};
static const int pcn_rect_label[4]       = {136,  48, 266,  92};
static const int pcn_rect_row1[4]        = {136,  96, 392, 122};
static const int pcn_rect_row2[4]        = {136, 124, 392, 150};
static const int pcn_rect_row3[4]        = {136, 152, 392, 178};
static const int pcn_rect_flow[4]        = {136, 180, 392, 242};
static const int pcn_rect_mingzao[4]     = {136, 152, 180, 178};
static const int pcn_rect_row3_time[4]   = {184, 152, 392, 178};
/* 负断言区（版式退役面，[S3]）。 */
static const int pcn_rect_sticker_neg[4] = {268,  50, 390,  92};
static const int pcn_rect_preview_neg[4] = {136, 220, 392, 242};
/* containment 证据带（[S3] 卡区硬边界的界外带）。 */
static const int pcn_rect_left_gap[4]    = {129,  48, 135, 242};
static const int pcn_rect_bottom_band[4] = {136, 243, 392, 251};
static const int pcn_rect_below_label[4] = {136,  96, 392, 242};
static const int pcn_rect_below_row1[4]  = {136, 124, 392, 242};

/* 右界越界检查带：卡区右界之外 x[393..399] x 各行带（帧右缘 7px，任何文字行墨越右界
 * 都会落进本带——P1CJK-A2/A10 的边界证据带）。 */
static const int pcn_band_right_x0 = 393;
static const int pcn_band_right_x1 = 399;

/* 被扫文件（seam [S1]/[S5] 落点 + 注册面 + 铁律 canary 面）。 */
#define PCN_HOME_P1_C   "firmware/main/display/home_p1.c"
#define PCN_HOME_P1_H   "firmware/main/display/home_p1.h"
#define PCN_UPCOMING_C  "firmware/main/display/home_upcoming.c"
#define PCN_RN_CC       "firmware/main/display/kidpal_render_node.cc"
#define PCN_SEAM_C      "firmware/main/test/host_app/main/host_infra_seam.c"
#define PCN_HOST_CMAKE  "firmware/main/test/host_app/main/CMakeLists.txt"

/* [S5] 烘焙固定标签资产冻结名（管线产出名 == home_p1.c 消费名 == 本文件探针名）。 */
#define PCN_ASSET_LABEL_NEXT    "p1_label_jiexialai.bin"
#define PCN_ASSET_LABEL_MINGZAO "p1_label_mingzao.bin"
/* host 夹具私有资产根（只含两枚标签 bin；其余资产维持兜底态——runner 其余齿零扰动）。 */
#define PCN_ASSET_ROOT_HOST "/tmp/pcj_assets_ft"
#define PCN_ASSET_SRC_DIR   "firmware/main/assets/spiffs_root"

/* -------------------------------------------------------------------------- *
 * Seam [S1] — FROZEN OFFLINE CONTRACT 弱哨兵（强定义落地后覆盖；多重 weak 合法，
 * schedule_grid 先例）。强符号（方案 B 改造后）在 host runner 在世——本哨兵仅为
 * red-baseline stub 脚手架独立编译路径保留。
 * ------------------------------------------------------------------------ */
typedef struct {
    int w;
    int h;
    int advance;
    const unsigned char *bits;
} pcn_glyph_t;

__attribute__((weak)) int kidpal_p1_font_glyph(uint32_t codepoint,
                                               pcn_glyph_t *out)
{
    (void)codepoint; (void)out;
    return -1; /* 弱默认 = 字形不可用（与 [S1] 弱默认语义一致） */
}

/* host 渲染缝（host_app/main/host_infra_seam.c 提供，恒链入 runner）。只探针。 */
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，HPB/KSG 先例同款）。全部扫描跑在
 * 真实工作树上，root 自手持 cwd 定位（仓根或 firmware/main/test/host_app 均可
 * ——文档命令位置两处同值，evidence 类扫描齿 root 注入纪律的先例形态）。
 * test / host_app / build / .git / assets 目录排除——test 目录住着本冻结文件自身
 * （其注释合法提及 needle）。
 * ------------------------------------------------------------------------ */
#define PCN_MAX_PATH 1024
#define PCN_MAX_READ (8u * 1024u * 1024u)

static char pcn_root[PCN_MAX_PATH];
static int pcn_root_ready;

static int pcn_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int pcn_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int pcn_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int pcn_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[PCN_MAX_PATH];

    if (pcn_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (pcn_is_regular(probe)) {
            snprintf(pcn_root, sizeof(pcn_root), "%s", cands[i]);
            pcn_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int pcn_rel(char *out, size_t cap, const char *rel)
{
    if (!pcn_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", pcn_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", pcn_root, rel);
    return 1;
}

static int pcn_exists(const char *rel)
{
    char p[PCN_MAX_PATH];
    return pcn_rel(p, sizeof(p), rel) && pcn_is_regular(p);
}

static char *pcn_read(const char *rel, size_t *out_len)
{
    char p[PCN_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!pcn_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > PCN_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int pcn_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int pcn_file_contains(const char *rel, const char *needle)
{
    char *buf = pcn_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = pcn_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int pcn_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = pcn_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

static int pcn_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, char *first_hit, size_t cap,
                                       int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[PCN_MAX_PATH];
        char rel_next[PCN_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            pcn_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += pcn_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > PCN_MAX_READ) {
                continue;
            }
            {
                char *buf = pcn_read(rel_next, NULL);
                if (buf != NULL) {
                    if (pcn_contains(buf, needle)) {
                        if (count == 0 && first_hit != NULL) {
                            snprintf(first_hit, cap, "%s", rel_next);
                        }
                        count++;
                    }
                    free(buf);
                }
            }
        }
    }
    closedir(d);
    return count;
}

static int pcn_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[PCN_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!pcn_rel(abs, sizeof(abs), rel_dir) || !pcn_is_dir(abs)) {
        return 0;
    }
    return pcn_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

#define PCN_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(pcn_locate_root(), \
    "PCN RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（HPB/KSG 先例）。 */
static char pcn_fm_buf[1024];
static const char *pcn_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *pcn_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pcn_fm_buf, sizeof(pcn_fm_buf), fmt, ap);
    va_end(ap);
    return pcn_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Pixel-probe helpers（HPB/KSG 先例同款）:
 *   pcn_count_ink(rect)  — 区内墨像素计数；-1 = 探针失败（帧未渲染/缝断裂），
 *                          调用方必须响红而非读成零墨。
 *   pcn_snap(buf)        — 整帧快照。
 *   pcn_diff_stats(a,b,rect,...) — 整帧 diff 总数 + 落在 rect 带内的数目
 *                          （diff 定位纪律：断言只锁「差集像素落在哪个钉区」，
 *                          schedule_grid 先例同款）。
 * ------------------------------------------------------------------------ */
static int pcn_count_ink(const int rect[4])
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == PCN_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            if ((uint32_t)px != PCN_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

static int32_t pcn_f_a[PCN_FRAME_W * PCN_FRAME_H];
static int32_t pcn_f_b[PCN_FRAME_W * PCN_FRAME_H];
static int32_t pcn_f_c[PCN_FRAME_W * PCN_FRAME_H];

static int pcn_snap(int32_t *buf)
{
    int x, y;

    for (y = 0; y < PCN_FRAME_H; y++) {
        for (x = 0; x < PCN_FRAME_W; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == PCN_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            buf[y * PCN_FRAME_W + x] = px;
        }
    }
    return 0;
}

static void pcn_diff_stats(const int32_t *a, const int32_t *b, const int rect[4],
                           int *out_total, int *out_in_band)
{
    int total = 0, in_band = 0;
    int x, y;

    for (y = 0; y < PCN_FRAME_H; y++) {
        for (x = 0; x < PCN_FRAME_W; x++) {
            if (a[y * PCN_FRAME_W + x] != b[y * PCN_FRAME_W + x]) {
                total++;
                if (x >= rect[0] && x <= rect[2] && y >= rect[1] && y <= rect[3]) {
                    in_band++;
                }
            }
        }
    }
    *out_total = total;
    *out_in_band = in_band;
}

/* 区内墨像素计数（快照帧版）：rect 带内非白计数，读法同 pcn_count_ink 但取自
 * 整帧快照 buf（快照经 pcn_snap 校验、不含 NOT_RENDERED 哨兵）——量历史帧时用
 * 本函数，勿误读 live 帧（TEST 9 tooth 4 修订先例）。 */
static int pcn_count_ink_snap(const int rect[4], const int32_t *f)
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            if (f[y * PCN_FRAME_W + x] != (int32_t)PCN_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

/* 右界越界带墨计数：rect 行带 x x[393..399]（P1CJK-A2/A10 边界证据带）。 */
static int pcn_right_band_ink(const int rect[4], const int32_t *f)
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = pcn_band_right_x0; x <= pcn_band_right_x1; x++) {
            if (f[y * PCN_FRAME_W + x] != (int32_t)PCN_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

/* -------------------------------------------------------------------------- *
 * Fixture helpers（真实 seam 注入；home_p1.c 在世强符号——非 mock 路径）。
 * name 拷贝走逐字节循环（HP1_NAME_LEN=32 恰容纳 31 字节截半名 + NUL）。
 * ------------------------------------------------------------------------ */
static void pcn_copy_name(char *dst, size_t cap, const char *src)
{
    size_t i;

    for (i = 0; src[i] != '\0' && i + 1 < cap; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

/* 夹具共用名（声明前置——pcn_inject_max_load 等夹具构造器先于 TEST 引用）。 */
static const char *PCN_NAME_FULL10 = "一二三四五六七八九十";
static const char *PCN_NAME_TRUNC_TAIL = "\xE9";

/* 全量注入（时间可调版）：2 项今日日程 + 次日预告，时间/名全由调用方给定。 */
static void pcn_inject_full_t(const char *t0, const char *n0,
                              const char *t1, const char *n1,
                              const char *pt, const char *pn)
{
    hp1_item_t items[2];
    hp1_item_t pv;
    int rc;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "%s", t0);
    pcn_copy_name(items[0].name, sizeof(items[0].name), n0);
    snprintf(items[1].time, sizeof(items[1].time), "%s", t1);
    pcn_copy_name(items[1].name, sizeof(items[1].name), n1);
    memset(&pv, 0, sizeof(pv));
    snprintf(pv.time, sizeof(pv.time), "%s", pt);
    pcn_copy_name(pv.name, sizeof(pv.name), pn);

    rc = kidpal_home_p1_schedule_inject("2026-09-22", items, 2, &pv);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): schedule_inject fixture rejected (rc=%d)——合法夹具"
               "被拒，渲染面无法驱动", rc));
}

/* 标准夹具（时间 HPB 口径：14:00 / 16:30 / 预告 09:00）。 */
static void pcn_inject_full(const char *n0, const char *n1, const char *pv_name)
{
    pcn_inject_full_t("14:00", n0, "16:30", n1, "09:00", pv_name);
}

static void pcn_inject_one(const char *n0)
{
    hp1_item_t items[1];
    int rc;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    pcn_copy_name(items[0].name, sizeof(items[0].name), n0);

    rc = kidpal_home_p1_schedule_inject("2026-09-22", items, 1, NULL);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): schedule_inject(one item) rejected (rc=%d)", rc));
}

/* 满载压力夹具：HP1_ITEM_MAX(8) 项 + 预告，全部 10 字 CJK 长名（P1CJK-A10）。 */
static void pcn_inject_max_load(void)
{
    hp1_item_t items[HP1_ITEM_MAX];
    hp1_item_t pv;
    int rc;
    int i;

    memset(items, 0, sizeof(items));
    for (i = 0; i < HP1_ITEM_MAX; i++) {
        snprintf(items[i].time, sizeof(items[i].time), "%02d:00", 8 + i);
        pcn_copy_name(items[i].name, sizeof(items[i].name), PCN_NAME_FULL10);
    }
    memset(&pv, 0, sizeof(pv));
    snprintf(pv.time, sizeof(pv.time), "09:00");
    pcn_copy_name(pv.name, sizeof(pv.name), PCN_NAME_FULL10);

    rc = kidpal_home_p1_schedule_inject("2026-09-22", items, HP1_ITEM_MAX, &pv);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): schedule_inject(max load) rejected (rc=%d)", rc));
}

/* 全量状态注入（时间/日期/温湿度/电量/充电/tick；HPB 夹具口径）。 */
static void pcn_set_fixture(void)
{
    hp1_state_t st;
    int rc;

    memset(&st, 0, sizeof(st));
    st.hour = 10;
    st.minute = 24;
    st.month = 9;
    st.mday = 22;
    st.env_valid = 1;
    st.temp_c = 22;
    st.humidity_pct = 45;
    st.battery_pct = 85;
    st.charging = 0;
    st.tick_ms = 0;
    rc = kidpal_home_p1_set_state(&st);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): set_state fixture rejected (rc=%d)", rc));
}

static void pcn_render_ok(void)
{
    int rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): render_frame rc=%d——P1 帧渲染失败（渲染缝断裂, "
               "非本包设计态红）", rc));
}

/* cp -> UTF-8 字节编码（1..4 字节），返回字节数。 */
static int pcn_utf8_encode(uint32_t cp, char *out)
{
    if (cp < 0x80u) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800u) {
        out[0] = (char)(0xC0u | (cp >> 6));
        out[1] = (char)(0x80u | (cp & 0x3Fu));
        return 2;
    }
    if (cp < 0x10000u) {
        out[0] = (char)(0xE0u | (cp >> 12));
        out[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        out[2] = (char)(0x80u | (cp & 0x3Fu));
        return 3;
    }
    out[0] = (char)(0xF0u | (cp >> 18));
    out[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
    out[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
    out[3] = (char)(0x80u | (cp & 0x3Fu));
    return 4;
}

/* 31 字节 'W' 极宽名（逐字节构造，31 字形 = host 子集 ASCII 若在场时的最宽合法
 * name——P1CJK-A2/A10 fixture；HP1_NAME_LEN=32 恰容 31 字节 + NUL）。 */
static void pcn_make_long_ascii(char *buf)
{
    memset(buf, 'W', 31);
    buf[31] = '\0';
}

/* 31 字节截半名 = PCN_NAME_FULL10（30 字节）+ PCN_NAME_TRUNC_TAIL（第 11 个字符的
 * UTF-8 前导字节, 见上方声明）——home_upcoming.c:393–396 按字节截 31 字节对 11 字
 * CJK 名的产物形状（seam [S4]）。 */

/* -------------------------------------------------------------------------- *
 * [S5] host 夹具私有资产根：只拷入两枚烘焙标签 bin（其余资产不在根内 = 数字/图标/
 * 小伴维持兜底路径、s_art_ok 恒 0——runner 其余齿的渲染态零扰动）。infra 门控：
 * art_assets.c 未接进 host runner（kidpal_art_mount 弱桩恒败）或标签 bin 未落盘
 * （builder 资产管线步缺失）都在此响 infra 红，不静默。
 * ------------------------------------------------------------------------ */
static int pcn_copy_file(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    FILE *out;
    unsigned char buf[4096];
    size_t n;

    if (in == NULL) {
        return -1;
    }
    out = fopen(dst, "wb");
    if (out == NULL) {
        fclose(in);
        return -1;
    }
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) {
            fclose(in);
            fclose(out);
            return -1;
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}

static void pcn_mount_label_assets(void)
{
    char rel[PCN_MAX_PATH];
    char src[PCN_MAX_PATH];
    char dst[PCN_MAX_PATH];
    kidpal_art_img_t img;

    PCN_REQUIRE_ROOT();
    mkdir(PCN_ASSET_ROOT_HOST, 0777); /* EEXIST 容忍；后续 fopen 失败会响红 */

    snprintf(rel, sizeof(rel), "%s/%s", PCN_ASSET_SRC_DIR, PCN_ASSET_LABEL_NEXT);
    TEST_ASSERT_TRUE_MESSAGE(pcn_rel(src, sizeof(src), rel),
        "PCN RED(infra): repo root not located — 标签资产源路径无法定位");
    snprintf(dst, sizeof(dst), "%s/" PCN_ASSET_LABEL_NEXT, PCN_ASSET_ROOT_HOST);
    TEST_ASSERT_TRUE_MESSAGE(pcn_copy_file(src, dst) == 0,
        pcn_fm("PCN RED(infra): 烘焙标签资产 %s 无法从 spiffs_root 拷入夹具根——"
               "builder 资产管线步缺失（BUILDER DUTY 4))", PCN_ASSET_LABEL_NEXT));
    snprintf(rel, sizeof(rel), "%s/%s", PCN_ASSET_SRC_DIR, PCN_ASSET_LABEL_MINGZAO);
    TEST_ASSERT_TRUE_MESSAGE(pcn_rel(src, sizeof(src), rel),
        "PCN RED(infra): repo root not located — 标签资产源路径无法定位");
    snprintf(dst, sizeof(dst), "%s/" PCN_ASSET_LABEL_MINGZAO, PCN_ASSET_ROOT_HOST);
    TEST_ASSERT_TRUE_MESSAGE(pcn_copy_file(src, dst) == 0,
        pcn_fm("PCN RED(infra): 烘焙标签资产 %s 无法从 spiffs_root 拷入夹具根——"
               "builder 资产管线步缺失（BUILDER DUTY 4))", PCN_ASSET_LABEL_MINGZAO));

    TEST_ASSERT_TRUE_MESSAGE(kidpal_art_mount(PCN_ASSET_ROOT_HOST) == 0,
        "PCN RED(infra): kidpal_art_mount 失败——art_assets.c 未接进 host runner"
        "（BUILDER DUTY 5))，标签齿无法探真烘焙资产；未接线时本红为自诊断 infra 红");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_art_load(PCN_ASSET_LABEL_NEXT, &img) == 0,
        pcn_fm("PCN RED(infra): 烘焙标签资产 %s 挂载后不可加载（KAP1 解码失败）",
               PCN_ASSET_LABEL_NEXT));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_art_load(PCN_ASSET_LABEL_MINGZAO, &img) == 0,
        pcn_fm("PCN RED(infra): 烘焙标签资产 %s 挂载后不可加载（KAP1 解码失败）",
               PCN_ASSET_LABEL_MINGZAO));
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — P1CJK-A1（name 半）+ P1CJK-A8（时间值承载）：行内容墨落行带（diff 定位）
 * 六对单变量帧差（仅 name 或仅 time 不同）各自渲染，diff 像素必须非零且全部落在对应
 * 行带内——(a) name/time 真被画进对应行（新版式行流落位）；(b) 行内容墨不越行带。
 * 标准夹具 = 2 项 + 预告、短名无换行 -> 行带确定性。
 * RED（追加轮设计态）: 主轮版式 name 落旧位（y143/184/228）、大时间/贴纸在世 ——
 * diff 落在新行带之外 -> in_band != total。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A1 -- name 墨落行带：row1/row2/row3 diff 定位（仅 name 不同、字节等长）+ 不越带 */
/* coverage: P1CJK-A8 -- 行内时间值承载：time 值 diff 落行带（[S2] seam ASCII 前提） */
TEST_CASE("P1CJK rows: 行内容墨落行带 diff 定位 (P1CJK-A1/A8)",
          "[p1_cjk_name][P1CJK-A1][P1CJK-A8][behavior]")
{
    int total, in_band;

    pcn_set_fixture();

    /* 基准帧 A：14:00 钢琴课 / 16:30 英语培训 / 预告 09:00 春游 */
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered — 探针缝断裂");

    /* tooth 1 (A1-row1 name): 仅换 item0 name（钢琴课 -> 游泳课, 9 字节等长）——
     * diff 必须非零且全部落在 ROW1 {136,96,392,122}。 */
    pcn_inject_full("游泳课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换 name 帧差为零——日程 name 未被渲染（渲染缝断裂, 非本包"
        "设计态红）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: 最近项行 name 墨差 %d/%d 落在 ROW1 {136,96,392,122} "
               "带外——新版式行流落位缺失（[S3] 重钉未实现, 追加轮设计态红）",
               total - in_band, total));

    /* tooth 2 (A8-row1 time): 仅换 item0 time（14:00 -> 15:00）——diff 全部落在
     * ROW1（行内时间值承载; [S2] seam ASCII 前提）。 */
    pcn_inject_full_t("15:00", "钢琴课", "16:30", "英语培训", "09:00", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A8 RED: 换 item0 time 帧差为零——行内时间值未承载（[S2] 行内时间走 "
        "[S1] seam ASCII 的实现前提缺失）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A8 RED: 最近项行 time 墨差 %d/%d 落在 ROW1 带外——大 imgfont "
               "时间未退役或时间未按新版式落行（追加轮设计态红）", total - in_band,
               total));

    /* tooth 3 (A1-row2 name): 仅换 item1 name（英语培训 -> 美术书法, 12 字节等长）
     * —— diff 全部落在 ROW2 {136,124,392,150}。 */
    pcn_inject_full("钢琴课", "美术书法", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row2, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换后续事项 name 帧差为零——ROW2 行 name 未被渲染（A1 行带承载"
        "缺一）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: 后续事项行 name 墨差 %d/%d 落在 ROW2 带外（追加轮设计态"
               "红）", total - in_band, total));

    /* tooth 4 (A8-row2 time): 仅换 item1 time（16:30 -> 17:30）——diff 全部落在 ROW2。 */
    pcn_inject_full_t("14:00", "钢琴课", "17:30", "英语培训", "09:00", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row2, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A8 RED: 换 item1 time 帧差为零——后续事项行时间值未承载");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A8 RED: 后续事项行 time 墨差 %d/%d 落在 ROW2 带外（追加轮设计态"
               "红）", total - in_band, total));

    /* tooth 5 (A1-row3 name): 仅换 preview name（春游 -> 秋游, 6 字节等长）——
     * diff 全部落在 ROW3 {136,152,392,178}（预告并入主体第三行）。 */
    pcn_inject_full("钢琴课", "英语培训", "秋游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row3, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换预告 name 帧差为零——末行预告 name 未被渲染（A1 行带承载"
        "缺一）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: 末行预告 name 墨差 %d/%d 落在 ROW3 带外——预告未上行"
               "主体第三行（旧独立带版式在世, 追加轮设计态红）", total - in_band,
               total));

    /* tooth 6 (A8-row3 time): 仅换 preview time（09:00 -> 10:00）——diff 全部落在
     * ROW3 且不侵明早标签槽（x>=184, 见 TEST 8 tooth 4）。 */
    pcn_inject_full_t("14:00", "钢琴课", "16:30", "英语培训", "10:00", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row3, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A8 RED: 换 preview time 帧差为零——预告行时间值未承载");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A8 RED: 预告行 time 墨差 %d/%d 落在 ROW3 带外（追加轮设计态红）",
               total - in_band, total));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — P1CJK-A2：超宽 name 不越卡区（主轮「截断」语义随版式演进为「换行承载」）
 * 最长合法 name（31 字节 = HP1_NAME_LEN-1，home_upcoming safe-truncate 上界）三槽
 * 全上：(a) 渲染不崩；(b) 卡区行带 x[393..399] 右界带严格零墨（换行或 clip, 墨绝不
 * 越 x=392）；(c) 长名 vs 短名 diff 非零且全部落在卡区 y>=96 域（标签带不受 name
 * 影响 = diff 零侵）。机制判别（换行证据）归 TEST 9（A10）。
 * 本 TEST 为守卫齿（基线即绿——现状 name blit 已 clip 进卡区; 实现让墨越界即红）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A2 -- 超宽 name 不越卡区：31 字节极宽名右界带零墨 + diff 锁卡区 y>=96 域 */
TEST_CASE("P1CJK rows: 超宽名承载不越卡区 (P1CJK-A2)",
          "[p1_cjk_name][P1CJK-A2][boundary]")
{
    char long_ascii[32];
    char long_cjk[32];
    int total, in_band, ink;

    pcn_make_long_ascii(long_ascii);
    snprintf(long_cjk, sizeof(long_cjk), "%s%s", PCN_NAME_FULL10, "W");

    pcn_set_fixture();

    /* 长名帧：三槽全上最长合法 name（31 字节 x3）。 */
    pcn_inject_full(long_ascii, long_cjk, long_ascii);
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");

    /* tooth 1 (A2 右界带守卫): 卡区行带 x[393..399] 严格零墨——换行/clip 纪律的
     * 边界证据（基线即绿；实现让墨越 x=392 即红）。 */
    ink = pcn_right_band_ink(pcn_rect_card, pcn_f_a);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A2 RED: 卡区行带右界带 x[393..399] 墨 %d != 0——超宽 name "
               "承载越出卡区右界（[S3] containment 纪律被突破）", ink));

    /* tooth 2 (A2 长名在场 + 锁区): 长名帧 vs 短名帧（钢琴课/英语培训/春游）——
     * diff 非零、全部落在卡区、且全部落在 y>=96 域（标签带不受 name 影响）。 */
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_card, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A2 RED: 超长 name 帧差为零——name 未被渲染（渲染缝断裂, 非设计态; "
        "或 fixture 字形在 host 子集缺场——先核 host Source Han 子集 ASCII/CJK "
        "覆盖再判真红）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A2 RED: 超长 name 墨差 %d/%d 落在卡区 {136,48,392,242} 之外"
               "——name 画到卡区以外（越区非承载）", total - in_band, total));
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_below_label, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A2 RED: 超长 name 墨差 %d/%d 侵及标签带 y<96——name diff 应全部"
               "落在 y>=96 文字行域", total - in_band, total));
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — P1CJK-A4：UTF-8 截半尾序列安全停笔
 * fixture = home_upcoming.c:393–396 对 11 字 CJK 名按字节截 31 字节的产物形状
 * （30 字节完整 10 字 + 1 个 UTF-8 前导字节，seam [S4]；home_upcoming 本包零改，
 * 本齿以 hp1_item_t 直注该字节形状——等价假设见头注强度申报）。
 * (a) name 在场（完整 30 字节名 diff 非零且落 ROW1）；
 * (b) 截半帧 == 完整帧（截半字符零墨——安全停笔, 画伪字形/越读即红）；
 * (c) 渲染 rc==0 不崩 + 截半帧右界带零墨（不越界）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A4 -- UTF-8 截半尾序列安全停笔：截半帧==完整帧 diff==0 + 不越界 + 不崩 */
TEST_CASE("P1CJK name: UTF-8 截半尾序列安全停笔 (P1CJK-A4)",
          "[p1_cjk_name][P1CJK-A4][boundary]")
{
    char trunc_name[32];
    int total, in_band, ink, rc;

    snprintf(trunc_name, sizeof(trunc_name), "%s%s", PCN_NAME_FULL10,
             PCN_NAME_TRUNC_TAIL);

    pcn_set_fixture();

    /* tooth 1 (A4 name 在场): 完整 30 字节名（一二三四五六七八九十）vs 短名（钢）
     * —— diff 非零且落 ROW1。追加轮设计态: 主轮版式 name 落旧位 -> 带外。 */
    pcn_inject_full(PCN_NAME_FULL10, "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_inject_full("钢", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A4 RED: CJK 长名帧差为零——name 未被渲染（渲染缝断裂, 非设计态）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        "PCN-A4 RED: 长名墨差落在 ROW1 带外——新版式行流落位缺失（追加轮设计态红）");

    /* tooth 2 (A4 截半安全停笔): 截半帧（31 字节, 尾 0xE9 前导字节）vs 完整帧
     * （30 字节）——diff 必须为零：截半字符不画（画伪字形/垃圾墨/按字节半画即红）。
     * 固定起笔纪律见头注 [S2]（起笔 x 不随 name 字节数平移）。 */
    pcn_inject_full(trunc_name, "英语培训", "春游");
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN-A4 RED: 截半尾序列渲染 rc=%d——解码器未对不完整尾序列安全停笔"
               "（不崩义务被破）", rc));
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_c) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_c, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        pcn_fm("PCN-A4 RED: 截半帧 vs 完整帧 diff %d != 0——截半多字节字符被画"
               "（安全停笔义务: 截半字符不画）", total));

    /* tooth 3 (A4 不越界): 截半帧右界带零墨（不越界半）。 */
    ink = pcn_right_band_ink(pcn_rect_row1, pcn_f_c);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A4 RED: 截半帧右界带墨 %d != 0——截半处理越出卡区右界", ink));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — P1CJK-A5：字形缺失（弱默认路径）退化为现状不崩
 * (a) probe 自检：经 seam 找一个真缺字形码点（候选表运行时探针，防 host 子集
 *     覆盖漂移假红）；
 * (b) name 带缺字形字符 vs 不带——diff==0：缺字形零墨、不画垃圾（缺字形路径返回
 *     未初始化字形描述/垃圾位图即红）；
 * (c) 纯缺字形名渲染 rc==0 不崩；
 * (d) name 管线在场（钢琴 vs 游泳 diff 非零落 ROW1）。
 * 方案 B 改造（[S1]）不改缺失语义——本齿期望零偏移。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A5 -- 字形缺失退化不崩：缺字形零墨/不画垃圾/不崩 + probe 自检 */
TEST_CASE("P1CJK name: 字形缺失退化不崩 (P1CJK-A5)",
          "[p1_cjk_name][P1CJK-A5][boundary]")
{
    static const uint32_t cands[] = { 0x9F98u, 0x3400u, 0x20000u, 0x2A700u };
    uint32_t miss_cp = 0;
    pcn_glyph_t g;
    char miss_name[32];
    char name_with_miss[32];
    int found = 0, total, in_band, rc;
    size_t i;

    /* tooth 1 (probe 自检): seam 可达且真有缺字形码点——候选表逐个探，取第一个
     * seam 报不可用的码点。全部在场 = infra 红（fixture 需换更稀有码点, 不静默）。 */
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        if (kidpal_p1_font_glyph(cands[i], &g) != 0) {
            miss_cp = cands[i];
            found = 1;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found,
        "PCN RED(infra): probe 字形候选全部在场——fixture 需换真缺字形码点"
        "（infra 修正, 非实现红）");

    pcn_set_fixture();

    /* tooth 2 (A5 name 在场): 钢琴 vs 游泳 diff 非零落 ROW1。追加轮设计态: 旧位。 */
    pcn_inject_full("钢琴", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_inject_full("游泳", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A5 RED: name 帧差为零——name 未被渲染（渲染缝断裂, 非设计态）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        "PCN-A5 RED: name 墨差落在 ROW1 带外——新版式行流落位缺失（追加轮设计态红）");

    /* tooth 3 (A5 缺字形零墨): 「钢琴 + 缺字形字符」vs「钢琴」——diff==0：缺字形
     * 字符零墨、不画垃圾（缺字形路径返回未初始化 w/h/bits = 垃圾墨即红）。 */
    pcn_copy_name(miss_name, sizeof(miss_name), "钢琴");
    {
        char enc[4];
        int n = pcn_utf8_encode(miss_cp, enc);
        int len = (int)strlen(miss_name);
        for (i = 0; i < (size_t)n && len + 1 < (int)sizeof(miss_name); i++) {
            miss_name[len++] = enc[i];
        }
        miss_name[len] = '\0';
    }
    pcn_inject_full(miss_name, "英语培训", "春游");
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN-A5 RED: 缺字形 name 渲染 rc=%d——缺字形未退化而是致命错（不崩"
               "义务被破）", rc));
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_c) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_c, pcn_rect_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        pcn_fm("PCN-A5 RED: 缺字形字符引入 diff %d != 0——缺字形路径画了墨（垃圾/"
               "未初始化字形描述), 退化义务 = 缺字形零墨", total));

    /* tooth 4 (A5 纯缺字形名): 整名只有一个缺字形字符——渲染 rc==0 不崩。 */
    {
        char enc[4];
        int n = pcn_utf8_encode(miss_cp, enc);
        int len = 0;
        for (i = 0; i < (size_t)n && len + 1 < (int)sizeof(name_with_miss); i++) {
            name_with_miss[len++] = enc[i];
        }
        name_with_miss[len] = '\0';
    }
    pcn_inject_full(name_with_miss, "英语培训", "春游");
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN-A5 RED: 纯缺字形名渲染 rc=%d——不崩义务被破", rc));
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — P1CJK-E2 铁律 canary + 扫描器自检 + P1CJK-A3 单项守卫（新版式行带口径）
 *   E2 canary（前段, 恒绿义务）: hp1_item_t 与 11 个冻结契约函数声明面在世 +
 *   home_upcoming 零改语义（不回写字形 seam）+ 契约/需求面在世。本轮追记：追加轮 =
 *   渲染侧纯版式，载荷形状零改侧不新增断言，canary 原样保留即为完备面。
 *   A3 单项半（后段）: 单项日程时 LABEL 带在场（固定卡题）+ ROW2/ROW3/FLOW 严格
 *   零墨（行流门控——无第 2 项/无预告则无行）；空日程半由 HPB TEST 1 tooth 5 承载
 *   （跨包张力见头注「跨包冲击登记」，不重复造齿）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-E2 -- 铁律 canary：11 个冻结契约函数声明面 + hp1_item_t 形状 + home_upcoming 零改语义 + 契约/需求面在世（载荷侧本轮零新增, 原样保留） */
/* coverage: P1CJK-A3 -- 单项零墨守卫（新版式：ROW2/ROW3/FLOW 严格零墨 + 标签在场）; 空日程半 = 既有 HPB 齿（跨包张力登记） */
TEST_CASE("P1CJK guards: 铁律canary+扫描器自检+单项零墨 (P1CJK-E2/A3)",
          "[p1_cjk_name][P1CJK-E2][P1CJK-A3][guards]")
{
    int ink;

    /* ---- E2 canary: hp1_item_t 形状 + 11 个冻结契约函数声明面 ------------- */
    TEST_ASSERT_TRUE_MESSAGE(HP1_NAME_LEN == 32 && HP1_TIME_LEN == 6 &&
                                 HP1_ITEM_MAX == 8,
        "PCN-E2 RED: hp1_item_t 形状常量漂移——冻结契约类型零偏移被破");
    TEST_ASSERT_TRUE_MESSAGE(
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_schedule_inject") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_schedule_count") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_schedule_item") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_schedule_preview") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_day_rollover") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_countdown") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_blink_visible") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_doze_state") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_layout_resolve") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_set_state") &&
        pcn_file_contains(PCN_HOME_P1_H, "kidpal_home_p1_render_frame"),
        "PCN-E2 RED: home_p1.h 冻结契约函数声明面缺失——11 函数零偏移铁律被破");

    /* ---- E2 canary: home_upcoming 零改语义（P1 侧容错, 不回写 K2 面）------ */
    PCN_REQUIRE_ROOT();
    TEST_ASSERT_FALSE_MESSAGE(pcn_file_contains(PCN_UPCOMING_C, "kidpal_p1_font_glyph"),
        "PCN-E2 RED: home_upcoming.c 引用字形 seam——K2 接线层被本包回写"
        "（home_upcoming.{c,h} 零改铁律; 截半容错必须在 home_p1.c 解码器内）");
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_UPCOMING_C, "kidpal_home_upcoming_apply"),
        "PCN-E2 RED(infra): home_upcoming.c 冻结入口丢失——K2 面被拆");

    /* ---- E2 canary: 契约/需求面在世（零修订） ---------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(pcn_exists("docs/requirements.md"),
        "PCN-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰");
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains("docs/requirements.md", "S-F1.1"),
        "PCN-E2 RED: requirements S-F1.1 锚丢失——需求面被修订");
    TEST_ASSERT_TRUE_MESSAGE(pcn_exists("docs/contracts/interface-contract.md"),
        "PCN-E2 RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* ---- 扫描器自检（防假绿: 负向齿必须能红） ---------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(pcn_exists(PCN_RN_CC),
        "PCN RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(pcn_exists("firmware/main/display/__pcn_no_such__.cc"),
        "PCN RED(infra): scanner reports a nonexistent file as present");
    {
        char hit[PCN_MAX_PATH];
        int n = pcn_scan_count("firmware/main", "__pcn_no_such_needle__", hit,
                               sizeof(hit));
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            "PCN RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    }
    TEST_ASSERT_TRUE_MESSAGE(pcn_count_in_file(PCN_HOME_P1_C,
                                               "kidpal_home_p1_render_frame") >= 1,
        "PCN RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* ---- A3 单项半（新版式行带口径）: 1 项日程 + 无预告 -------------------- */
    pcn_set_fixture();
    pcn_inject_one("钢琴课");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");

    ink = pcn_count_ink(pcn_rect_card);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_ELEM,
        pcn_fm("PCN-A3 RED(infra): 单项日程卡区墨 %d < %d——卡区失墨（标签+最近项行"
               "主体渲染被拆, 非本包预期）", ink, PCN_INK_MIN_ELEM));
    ink = pcn_count_ink(pcn_rect_label);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_LABEL,
        pcn_fm("PCN-A3 RED: 单项日程「接下来」标签带墨 %d < %d——烘焙大字标签缺失"
               "（[S5] 版式未实现, 追加轮设计态红）", ink, PCN_INK_MIN_LABEL));
    ink = pcn_count_ink(pcn_rect_row2);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A3 RED: 单项日程 ROW2 墨 %d != 0——行流门控被破（无第 2 项却画"
               "了行, 或大 imgfont 时间/贴纸未退役压进行带）", ink));
    ink = pcn_count_ink(pcn_rect_row3);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A3 RED: 单项日程无预告 ROW3 墨 %d != 0——s_has_preview 门控被破"
               "（预告缺位仍画行）", ink));
    ink = pcn_count_ink(pcn_rect_flow);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A3 RED: 单项日程 FLOW 带墨 %d != 0——无换行/溢出内容却画进承载带",
               ink));
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — P1CJK-A1 机制半 + P1CJK-A6 装配事实半 + 注册接线（structural, 恒绿义务）
 *   tooth 1: home_p1.c 弱默认 + 调用点（>= 2 处）
 *   tooth 2: host_infra_seam.c host 强符号（方案 B 改造后符号面不变）
 *   tooth 3: kidpal_render_node.cc 设备强符号（font_puhui_14_1 双分支）——A6 装配
 *            事实半（M3K2-A7 结构半先例同款）
 *   tooth 4: 本文件注册进 HOST_ACCEPTANCE_SRCS（builder wiring 守卫）
 *   tooth 5 (guard): home_p1.c 被测面在世（现状绿）
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A1 -- seam 机制半：home_p1.c 弱默认+调用点 / host_infra_seam.c host 强符号 */
/* coverage: P1CJK-A6 -- device 强符号装配事实半（kidpal_render_node.cc + font_puhui_14_1）; device build 闸与真机目测见文件尾闸证据条目 */
TEST_CASE("P1CJK structural: 字形 seam 三符号落点+注册接线 (P1CJK-A1/A6)",
          "[p1_cjk_name][P1CJK-A1][P1CJK-A6][structural]")
{
    char hit[PCN_MAX_PATH];
    int n;

    PCN_REQUIRE_ROOT();

    /* tooth 1: home_p1.c 弱默认 + name blit 调用点（weak 默认恒不可用语义见 [S1]）。 */
    n = pcn_count_in_file(PCN_HOME_P1_C, "kidpal_p1_font_glyph");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        pcn_fm("PCN-A1 RED: home_p1.c 仅 %d 处 kidpal_p1_font_glyph（期望 >= 2 = "
               "弱默认定义 + name 渲染调用点）——字形 seam 未接入 home_p1 渲染面", n));

    /* tooth 2: host 强符号（host_infra_seam.c, 方案 B 改造后符号面不变）。 */
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_SEAM_C, "kidpal_p1_font_glyph"),
        "PCN-A1 RED: host_infra_seam.c 无 kidpal_p1_font_glyph 强符号——host 分支"
        "缺场, host runner 上 name 永不渲染（TEST 1-4 将持续设计态红）");

    /* tooth 3: 设备强符号（kidpal_render_node.cc, font_puhui_14_1 双分支）——
     * A6 装配事实半（device build 闸的符号面证据）。 */
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_RN_CC, "kidpal_p1_font_glyph") &&
                                 pcn_file_contains(PCN_RN_CC, "font_puhui_14_1"),
        "PCN-A6 RED: kidpal_render_node.cc 无 kidpal_p1_font_glyph 强符号 / "
        "font_puhui_14_1 引用——设备分支缺场（对话页同款 14px 普通字体未接线, "
        "P1CJK-A6 装配事实面）");

    /* tooth 4: 本文件注册进 HOST_ACCEPTANCE_SRCS（置于 smoke 之前的接线守卫,
     * 序断言由既有 HPB/KSG 注册齿家族口径承载, 本齿钉存在性）。 */
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_HOST_CMAKE,
                                               "p1_cjk_name.acceptance.c"),
        "PCN RED: p1_cjk_name.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — host 验收"
        "构建未接本包");

    /* tooth 5 (guard): home_p1.c 被测面在世。 */
    n = pcn_scan_count("firmware/main", "kidpal_home_p1_render_frame(", hit,
                       sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "PCN RED(infra): kidpal_home_p1_render_frame 定义丢失——被测面被拆");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — P1CJK-A7：「接下来」烘焙大字标签墨在场（[S5] 新增）
 *   结构半: home_p1.c 引用两枚 [S5] 冻结资产名（走烘焙管线非手绘基元——源锚代理档,
 *   自陈非行为本体）；
 *   行为半: 夹具私有资产根挂载自检（infra 门控）+ LABEL 带 {136,48,266,92} 墨 >=
 *   阈值（标准夹具渲染下大字标签在场）。
 *   RED（追加轮设计态/接线态）: home_p1.c 无资产引用（结构半）+ 标签未画（行为半）；
 *   art_assets.c 未接线 / 标签 bin 未落盘 = infra 红（自诊断）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A7 -- 「接下来」烘焙大字标签墨在场：[S5] 资产名结构半 + LABEL 带行为半（host 私有资产根探真烘焙资产） */
TEST_CASE("P1CJK bake: 「接下来」烘焙大字标签墨在场 (P1CJK-A7)",
          "[p1_cjk_name][P1CJK-A7][bake]")
{
    int ink;

    PCN_REQUIRE_ROOT();

    /* 结构半（源锚代理档）: home_p1.c 消费两枚 [S5] 冻结资产。 */
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_HOME_P1_C, PCN_ASSET_LABEL_NEXT) &&
                                 pcn_file_contains(PCN_HOME_P1_C,
                                                   PCN_ASSET_LABEL_MINGZAO),
        pcn_fm("PCN-A7 RED: home_p1.c 未引用烘焙标签资产 %s / %s——固定标签未走"
               "既有烘焙资产管线（[S5] 缺场, 追加轮设计态红）",
               PCN_ASSET_LABEL_NEXT, PCN_ASSET_LABEL_MINGZAO));

    /* 行为半: 私有资产根挂载自检（infra 门控）+ 标准夹具渲染 + LABEL 带墨。 */
    pcn_mount_label_assets();
    pcn_set_fixture();
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");

    ink = pcn_count_ink(pcn_rect_label);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_LABEL,
        pcn_fm("PCN-A7 RED: 「接下来」标签带 {136,48,266,92} 墨 %d < %d——烘焙大字"
               "标签未渲染或未落卡顶左锚定带（[S3]/[S5] 版式未实现, 追加轮设计态红）",
               ink, PCN_INK_MIN_LABEL));
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — P1CJK-A9：末行预告构成 + 版式退役负断言（[S3]/[S5] 新增）
 *   tooth 1: 明早标签槽 {136,152,180,178} 墨 >= 阈值（「明早」烘焙标签在场——
 *            文案权威 = spec WF-F1-01 v1.21「明早」非「明天」；载荷侧 HH:MM 由
 *            K2 齿 + E2 canary 守, 槽内墨 = 设备渲染标签半）;
 *   tooth 2: 旧贴纸/大时间右上区 {268,50,390,92} 任何夹具严格零墨（倒计时贴纸退役 +
 *            大 imgfont 时间退役 + 预告不入右上独立区——三重负断言承载）;
 *   tooth 3: 旧预告独立带 {136,220,392,242} 标准夹具下严格零墨（预告上行主体第三行,
 *            旧独立钉区退役——「钉版式出入消除」）;
 *   tooth 4: 预告时间值 diff 全部落在标签槽右侧 {184,152,392,178}（时间起笔 x>=184,
 *            不侵明早槽）。
 *   RED（追加轮设计态）: 贴纸/大时间在世 -> tooth 2 红；预告在旧独立带 -> tooth 3/4 红；
 *   明早标签未画 -> tooth 1 红。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A9 -- 末行预告构成（明早标签槽 + 时间右置）+ 版式退役负断言（贴纸/大时间右上区、旧预告独立带零墨） */
TEST_CASE("P1CJK preview: 末行预告构成+版式退役负断言 (P1CJK-A9)",
          "[p1_cjk_name][P1CJK-A9][behavior]")
{
    int total, in_band, ink;

    pcn_mount_label_assets();
    pcn_set_fixture();

    /* 标准夹具基准帧。 */
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");

    /* tooth 1 (A9 明早标签槽): {136,152,180,178} 墨 >= 阈值。 */
    ink = pcn_count_ink(pcn_rect_mingzao);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_MINGZAO,
        pcn_fm("PCN-A9 RED: 明早标签槽 {136,152,180,178} 墨 %d < %d——「明早」烘焙"
               "标签未渲染或未落末行左槽（[S5] 版式未实现, 追加轮设计态红）",
               ink, PCN_INK_MIN_MINGZAO));

    /* tooth 2 (A9 负断言): 旧贴纸/大时间右上区任何夹具严格零墨。 */
    ink = pcn_count_ink(pcn_rect_sticker_neg);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A9 RED: 右上独立区 {268,50,390,92} 墨 %d != 0——倒计时贴纸/大 "
               "imgfont 时间未退役（本轮挂账项在世渲染）, 或预告被放进右上独立区",
               ink));

    /* tooth 3 (A9 负断言): 旧预告独立带标准夹具下严格零墨。 */
    ink = pcn_count_ink(pcn_rect_preview_neg);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A9 RED: 旧预告独立带 {136,220,392,242} 墨 %d != 0——预告未上行"
               "主体第三行（旧独立钉区版式在世, 钉版式出入未消除）", ink));

    /* tooth 4 (A9 预告时间右置): 仅换 preview time（09:00 -> 10:00）——diff 全部
     * 落在标签槽右侧 {184,152,392,178}（时间起笔 x>=184, 不侵明早槽）。 */
    pcn_inject_full_t("14:00", "钢琴课", "16:30", "英语培训", "10:00", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_row3_time, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A9 RED: 换 preview time 帧差为零——预告行时间值未承载");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A9 RED: 预告 time 墨差 %d/%d 落在标签槽右侧带 {184,152,392,178} "
               "之外——时间侵及明早槽或未落第三行（追加轮设计态红）",
               total - in_band, total));
}

/* -------------------------------------------------------------------------- *
 * TEST 9 — P1CJK-A10：版式承载不越卡区（满载压力 containment）+ 换行行为
 *   tooth 1: 满载压力（HP1_ITEM_MAX=8 项 + 预告, 全 10 字 CJK 名）渲染 rc==0 不崩 +
 *            卡区界外带（右 x[393..399] / 底 y[243..251] / 左缝 x[129..135]）严格
 *            零墨——版式承载硬边界在满载下不破（基线即绿守卫半）;
 *   tooth 2: 单项 31×'W' 极宽名——containment 同上（基线即绿）;
 *   tooth 3: 换行证据 gate——极宽名 vs 短名 diff 全部落在 y>=96 域且 ROW1 内有差
 *            （首行画进 ROW1 = 行流落位; 追加轮设计态红: 主轮版式 name 在 y143）;
 *   tooth 4: 换行证据——极宽名帧 ROW1 以下带 {136,124,392,242} 墨 >= 阈值（续行
 *            落带 = 「正常换行」行为; 几何可达性论证见头注 A10 强度申报）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A10 -- 版式承载不越卡区（8 项满载压力 containment）+ 换行行为（31x'W' 极宽名续行落带） */
TEST_CASE("P1CJK bounds: 满载不越卡区+换行行为 (P1CJK-A10)",
          "[p1_cjk_name][P1CJK-A10][boundary]")
{
    char long_ascii[32];
    int total, in_band, ink, rc;

    pcn_make_long_ascii(long_ascii);
    pcn_set_fixture();

    /* tooth 1 (A10 满载压力 containment): 8 项 + 预告 + 长名 -> 不崩 + 界外带零墨。 */
    pcn_inject_max_load();
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN-A10 RED: 满载日程渲染 rc=%d——版式承载在 HP1_ITEM_MAX 满载下"
               "不崩义务被破", rc));
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");
    ink = pcn_right_band_ink(pcn_rect_card, pcn_f_a);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A10 RED: 满载帧卡区右界带 x[393..399] 墨 %d != 0——行承载越右界",
               ink));
    ink = pcn_count_ink(pcn_rect_bottom_band);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A10 RED: 满载帧卡区底界外带 y[243..251] 墨 %d != 0——行承载越"
               "卡区底界", ink));
    ink = pcn_count_ink(pcn_rect_left_gap);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A10 RED: 满载帧卡区左缝带 x[129..135] 墨 %d != 0——卡内容侵小伴"
               "演出区隔离缝", ink));

    /* tooth 2 (A10 极宽名 containment): 单项 31×'W' + 无预告。 */
    pcn_inject_one(long_ascii);
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN-A10 RED: 极宽名渲染 rc=%d——不崩义务被破", rc));
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    ink = pcn_right_band_ink(pcn_rect_card, pcn_f_b);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A10 RED: 极宽名帧右界带墨 %d != 0——换行/clip 越右界", ink));
    ink = pcn_count_ink(pcn_rect_bottom_band);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A10 RED: 极宽名帧底界外带墨 %d != 0——换行续行越卡区底界", ink));

    /* tooth 3 (A10 换行证据 gate): 极宽名 vs 短名（钢琴课）——diff 全部落在 y>=96
     * 域且 ROW1 内有差（首行画进 ROW1）。追加轮设计态: 主轮版式 name 在 y143 ->
     * ROW1 内无差。 */
    pcn_inject_one("钢琴课");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_c) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_b, pcn_f_c, pcn_rect_below_label, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A10 RED: 极宽名 vs 短名帧差为零——name 未被渲染（渲染缝断裂, 非设计态）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A10 RED: 极宽名墨差 %d/%d 落在卡区 y<96 域——name diff 应全部"
               "落在文字行域", total - in_band, total));
    {
        int r1_total, r1_in;
        pcn_diff_stats(pcn_f_b, pcn_f_c, pcn_rect_row1, &r1_total, &r1_in);
        TEST_ASSERT_TRUE_MESSAGE(r1_in > 0,
            "PCN-A10 RED: 极宽名首行未画进 ROW1 {136,96,392,122}——行流落位缺失"
            "（追加轮设计态红: 主轮版式 name 落旧位 y143）");
    }

    /* tooth 4 (A10 换行证据): 极宽名帧 ROW1 以下带 {136,124,392,242} 墨 >= 阈值
     * ——31×'W' 续行落带 = 「正常换行」行为（几何可达性论证见头注 A10）。
     * 量极宽名帧快照 pcn_f_b（修订 2026-09-22: 原笔误读 live 帧——tooth 3 已把
     * live 帧重渲为短名单项帧, 短名不换行 -> below_row1 恒 0）。 */
    ink = pcn_count_ink_snap(pcn_rect_below_row1, pcn_f_b);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_WRAP,
        pcn_fm("PCN-A10 RED: 极宽名帧 ROW1 以下带墨 %d < %d——31×'W' 续行未落带"
               "（「正常换行」行为缺失: 旧截断/单行 clip 版式在世, 追加轮设计态红）",
               ink, PCN_INK_MIN_WRAP));
}

/* ---------------------------------------------------------------------------
 * P1CJK-A6 闸证据条目（冻结文本——本条不是 host 齿, 不造假齿; 执行承载 = verifier
 * final 轮收口 + human 持板目击, 证据落 delivery/changes/dev/
 * 260921-m3-k4-p1-cjk-dynamic-text/）:
 *
 *   (1) device build 闸: idf.py 构建 xiaozhi.bin 过 + 体积余量记录（无新增大静态
 *       量——s_p1_fb 帧缓冲既有 PSRAM .ext_ram.bss 面零偏移、零新增帧缓冲; 字形
 *       位图经 [S1] seam 运行时获取、烘焙标签经 [S5] SPIFFS 资产、零新增大静态表）。
 *       方案 B 改造（[S1]）后 device 强符号装配事实（kidpal_render_node.cc 含
 *       kidpal_p1_font_glyph + font_puhui_14_1）由 TEST 6 tooth 3 在 host 侧承载
 *       （源锚代理档, 自陈非行为本体）。
 *   (2) 真机目测点（挂设备轮窗口; **新版式口径**）: 「接下来」大字标签可见; 最近项
 *       行/后续事项行/末行预告与 WF-F1-01 对照; 长名换行不越卡; 明早文案（非
 *       「明天」）; 倒计时贴纸退役后版面无残墨; 方案 B 解码后 font_puhui_14_1
 *       CJK 字形无乱码/无逐行漂移（live 轮 A6 FAIL 修复的目测销账）。
 *
 * ---------------------------------------------------------------------------
 * RED baseline 判读口径（追加轮修订版冻结时点, builder 未实现追加轮）:
 *   通道① 修订前基线（主轮合并态, 2026-09-22 acceptance-author 实跑）: host runner
 *       全量 80 Tests / 0 Failures / 0 Ignored / OK / exit 0（面域 = firmware,
 *       量测 @ 修订前工作树 = 主轮合并态; 与派单「80 齿全绿」锚一致）。
 *   通道② 本修订版接入后（预期红 = 设计态/接线态, builder 落 [S2]/[S3]/[S5] +
 *       B 方案 [S1] 改造后转绿, 不计违约）:
 *       - TEST 1（A1/A8）: 全部六 tooth 红——主轮版式 name/时间落旧位, 新行带外;
 *       - TEST 3（A4）/TEST 4（A5）: name 在场 tooth 红（ROW1 带外）;
 *       - TEST 5: canary 段 + 扫描器自检 + 卡区墨守卫**绿**（恒绿义务）, A3 标签带
 *         在场 tooth 红（烘焙标签未实现）——同 CASE 内先绿后红, Unity 计 1 failure;
 *       - TEST 7（A7）: 结构半红（home_p1.c 无 [S5] 资产引用）——设计态（实跑红点
 *         即此, 资产拷贝门在其后未达）; 若结构半已实现而资产/接线缺位, 则先行
 *         infra 红（自诊断信息指认 BUILDER DUTY 4)/5));
 *       - TEST 8（A9）: 实跑红点 = 标签资产拷贝 infra 门（p1_label_*.bin 未烘焙落盘
 *         = BUILDER DUTY 4) 缺失, 自诊断）; 资产在位后红点移至 tooth 2（贴纸/大
 *         时间右上区在世）与 tooth 3（预告在旧独立带）;
 *       - TEST 9（A10）: tooth 1/2 守卫**绿**（满载/极宽名 containment 现状即绿,
 *         实跑已验证抵达 tooth 3）, tooth 3 红（首行未画进 ROW1）。
 *       MUST-STAY-GREEN at baseline: TEST 2 全部（A2 守卫）; TEST 6 全部（结构齿,
 *       符号面在世）; TEST 5 canary 段 + TEST 9 tooth 1/2。
 *   通道③ C 类冻结验收实编 + 独立 stub 执行（DEV-GOVERNANCE 验收测试纪律 §3/§4;
 *       脚手架 = firmware/main/test/acceptance/red-baseline/ 下 unity.h + runner.c +
 *       pcn_red_seam_stub.c（主轮已建, 非冻结件——提供 host_infra_render_p1_frame/
 *       read_pixel 强符号与 kidpal_art 星族桩, 使在世 home_p1.c 真实渲染）:
 *       gcc -std=c11 -I firmware/main/test/acceptance/red-baseline \
 *           -I firmware/main/display \
 *           -o /tmp/pcn_red_runner \
 *           firmware/main/test/p1_cjk_name.acceptance.c \
 *           firmware/main/display/home_p1.c \
 *           firmware/main/test/acceptance/red-baseline/runner.c \
 *           firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c
 *       （仓根 cwd 执行 /tmp/pcn_red_runner; 编译通道 error 为零 = 实编过）
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）; 标签资产私有根 = /tmp/pcj_assets_ft（WSL 内）。
 * --------------------------------------------------------------------------- */
/* REVISION LOG (acceptance-author only; 修订均经 planner 派发):
 *   2026-09-22  initial freeze (acceptance-author, TaskPackage
 *               260921-m3-k4-p1-cjk-dynamic-text).
 *   2026-09-22  修复 TEST 2 tooth 2 冻结件算术缺陷（planner 核实, 非 builder
 *               实现缺陷）: pcn_diff_stats 的 out_total 恒为整帧 diff 计数,
 *               旧断言 (b1+b2+b3)==(t1+t2+t3) 即 D==3D, 与前置 total>0 互斥,
 *               对任何正确渲染恒红（builder 实测真整帧 diff 2086 全落三钉区
 *               并集内, 测试假象 6258/4172）。修复 = total 取单次整帧计数 D,
 *               断言改 in_band==D（全部 diff 落钉区并集内）; 判别力保全:
 *               去右界 clip 变异（mutationEvidence P1CJK-A2）令越区像素
 *               x>=393 落并集外 -> in_band<D 仍转红, 齿强度未弱化。
 *   2026-09-22  追加轮重钉（planner 派发, human 拍板 plan-B-decode-fix-plus-
 *               wireframe-alignment = 立项 ack; 主轮已合并 80 齿全绿）:
 *               ① [S3] 重钉——旧三钉区（NEXT/NEXTLIST/PREVIEW）退役, 改行流
 *               口径（CARD/LABEL/ROW1/ROW2/ROW3/FLOW + 负断言区
 *               STICKER_NEG/PREVIEW_NEG）; 预告从独立钉区并入主体第三行,
 *               旧独立带转标准夹具零墨负断言（钉版式出入消除）。
 *               ② 逐齿评估结论: TEST 1 修订（三 name 齿重钉行带 + 新增三 time
 *               值承载齿 = A8）; TEST 2 修订（「截断」语义随版式演进为「换行
 *               承载不越卡区」, 右界守卫保留, 锁区改卡区 y>=96 域, 守卫齿基线
 *               即绿）; TEST 3/4 修订（区带重钉, 语义零改）; TEST 5 修订
 *               （canary 段原样保留并前置——载荷形状零改侧本轮不适用新增断言,
 *               渲染侧纯版式; A3 半按行带口径重钉）; TEST 6 保留（方案 B 改造
 *               不动符号面, 恒绿义务）; 新增 TEST 7/8/9（A7 烘焙标签 / A9 末行
 *               预告构成+退役负断言 / A10 满载 containment+换行）。废弃面:
 *               旧 PREVIEW 正向带钉（版式退役, 由 PREVIEW_NEG 负断言替代）,
 *               旧 A2「截断不越右界」措辞（语义演进, 判别力并入 TEST 9）。
 *               ③ 方案 B（[S1]）评估: A8 阈值 128 ≡ A4 阈值 8 ≡ A2 阈值 2 ≡
 *               A1 阈值 1（全部 50% 覆盖）, 官方解码只修 stride=0 逐行漂移
 *               （bpp=1 PLAIN 路径）, host 子集 A4 本就逐行正确——本文件既有
 *               期望零偏移; 盒上限 32×32 为 [S1] 冻结 1..64 的实现收紧（子集）,
 *               非契约变更。
 *               ④ [S5] 新增——烘焙固定标签资产 seam（p1_label_jiexialai.bin /
 *               p1_label_mingzao.bin 冻结名; 殿后解除权威 = human 立项 ack;
 *               文案「明早」非「明天」= spec WF-F1-01 v1.21）; host 探针走夹具
 *               私有资产根（只放行两枚标签 bin, s_art_ok 恒 0, runner 其余齿
 *               兜底态零扰动）; art_assets.c 接线 = BUILDER DUTY 5)。
 *               ⑤ 跨包冲击登记（头注）: HPB TEST 1 tooth 2/4/5 在实现后预期红,
 *               须 planner 派 HPB 冻结作者修订, 本文件不授权改他包冻结件。
 *   2026-09-22  修复 TEST 9 tooth 4 冻结件帧测量笔误（planner 核实, 非 builder
 *               实现缺陷）: 断言量「极宽名帧」ROW1 以下带, 代码却用 pcn_count_
 *               ink 读 live 帧——tooth 3 刚注入短名（钢琴课）重渲 live 帧, 短名
 *               不换行 -> below_row1 恒 0, 任何正确实现必红, 且短帧 below_row1
 *               非零会违反同 CASE A3 单项零墨语义（断言与自身契约互斥）。
 *               builder 插桩实证换行已在场（31×'W' ROW1 14 字 + 续行 13 字）。
 *               修复 = 新增 pcn_count_ink_snap(rect, frame) 快照读法 helper（与
 *               pcn_right_band_ink(rect, frame) 同风格）, tooth 4 改量极宽名帧
 *               快照 pcn_f_b, 断言与红名原文语义不变; 同 CASE tooth 1/2 快照读
 *               法零触碰, 判别力不变。
 * --------------------------------------------------------------------------- */
