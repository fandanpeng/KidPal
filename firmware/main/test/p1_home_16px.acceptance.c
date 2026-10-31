/*
 * Frozen acceptance tests — TaskPackage 261028-m6-k2a-p1-home-16px (subsystem:
 * firmware/main/display 首页 P1 帧渲染与设备/host 字体消费 seam; M6 列车 P1,
 * 首页 P1 日程卡 16px · mergeOrder 61 · rigor=full / riskTier=high ·
 * sourceType=proposal, dependsOn 261028-m6-k1-font-assets-runtime-loading 已合并
 * @ 789c0244)。
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1、AC2）+ WF-F1-01 临时事件下划线
 * 口径（requirements v1.69）。决策面 = PROPOSAL §3.1 F-1（首页正文 16px、辅助条
 * 维持 14px）/ F-2（line_h=22 不动、只调 text_yoff、容量零损失）/ §3.1 F-9 + spine
 * openQ q5（全设备临时标记 ※ 前缀退役改事件名下 2px 下划线之首页半）+ §3.2 OQ2
 * （母版 v8 只重出四页之首页帧，本包只消费 docs/ui/V8/canvas-p1.png）。
 * 授权链 = TaskPackage 261028-m6-k2a-p1-home-16px（exitCriteria P1H-E1/E2/E3）
 * + requirements v1.69 版本行 + brief v7 对齐注记（均在位）。
 *
 * ===========================================================================
 * 三分申报（launch-card 留根条款 + HOST_TEST_CONVENTIONS.md INF-E2 矩阵同源；
 * 设备 TU 不进 host 构建——本文件一开始就把三分申报写死）:
 *
 *   [host 可验]（本文件全部 TEST_CASE）:
 *     - [F1]/[S7] 字面源切换的行为级证据（[S1] 字形输出 == P0 16px body 句柄字形
 *       输出逐字节）+ 判别力负控（14px/16px 字形度量确可判别）。
 *     - 16px 帧几何半：行容量零损失（三条 preview 行带整行墨，F-2）、行带
 *       containment、F-9 下划线/前缀退役/固定日程不画线（帧 diff 定位）。
 *     - 结构半走 FAS-MANIFEST 构建清单机制（P0 font_assets_runtime.acceptance.c
 *       同款——human 2026-10-28 定向裁定树扫弃用的延续：运行时禁扫源码树、禁影子
 *       树镜像；本包结构面断言在既有 FAS 清单块上扩键，判由头注 [M] 节留痕）。
 *   [设备专有]（host 构建不覆盖，不写假行为断言，由 device build 闸 + P0 机制
 *     承载）:
 *     - 设备 [S1] 强符号（kidpal_render_node.cc）真消费 16px cbin 句柄——host 半由
 *       TEST 1 字节等价承载（同构 core，F-7 同源）；设备 cbin 装载等价性归真机轮
 *       （P0 预基线修正裁定 2 口径不变）。
 *   [真机轮专属]（P1H-E4 / PROPOSAL §5，本文件零 host 齿、不伪装）:
 *     - 首页 16px 真机观感、缺 assets 降级复证、下划线真机可读性——归
 *       261028-m6-k3-p3-story-typography 终端收口轮（crossPackageBehaviors 串行）；
 *       本文件 host 断言不以 host 字形冒充真机终判（host 截图字形 ≠ 真机，
 *       HOST_TEST_CONVENTIONS「字体现实」节）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；本文件 weak 哨兵在实现
 * 接线进 host runner 后被强定义覆盖，多重 weak 合法，p1_cjk_name [S1] 先例）:
 *
 *   [F1] 16px 字面源缝 [S7]（P1 新增；home = firmware/main/display/kidpal_p1_font.h
 *        声明 + firmware/main/display/kidpal_render_node.cc 设备强定义 +
 *        firmware/main/test/host_app/main/host_infra_seam.c host 强定义——三落点
 *        均在本包接线面内）:
 *
 *     const lv_font_t *kidpal_p1_font_face(void);
 *       返回 P0 统一句柄 seam [K4] 的 16px 正文档句柄 kidpal_font_body()（设备侧
 *       经 P0 已落的 kidpal_render_node_body_font() 透传口，host 侧直接
 *       kidpal_font_body()——同源同值）。恒不崩；返回 NULL = 未实现/降级。
 *       消费纪律：**[S1] 两强符号（设备 kidpal_render_node.cc / 宿主
 *       host_infra_seam.c）的字形获取改为「[S7] 取字面 + NULL 回落既有 14px 字面」**
 *       ——设备回落 font_puhui_14_1、宿主回落 lv_font_source_han_sans_sc_14_cjk
 *       （回落分支保留 = 缺 assets 仍走 P0 的 14px 降级、P0 FAS-A7
 *       render_node_puhui14 canary 不破、辅助条/键帽条等 14px 域零触——F-1 边界）。
 *       [S1] 签名/语义零改（1bpp 行主序 MSB 先行、stride=(w+7)/8、盒上限、有效期）。
 *       本文件 weak 哨兵：[S7] 未实现时返回 NULL（设计态红判别面）。
 *
 *   [U1] 临时事件标记下划线（F-9 首页半；home = firmware/main/display/home_p1.c
 *        行流消费面）:
 *       - 临时信号通道 = 既有 wire name 字符串：item name **首字符 '※'（U+203B，
 *         UTF-8 E2 80 BB）** = 临时事件。零新 wire 字段、零 producer 形状改（契约
 *         §1.3⑥ {time,name} 零偏移；跨端事实申报见头注「跨端申报」节）。
 *       - 渲染语义：① ※ 字形退役——不画、不占笔进（名不扣前缀宽，brief v7「画线
 *         随名宽、不占事件名前缀位」）；② 名剩余部分照常逐字形 blit；③ **名墨右缘
 *         x 跨度上、名墨底缘下方相邻行起画 2px 横线**（宽 = 名部分 x 跨度含端点、
 *         色 = 行墨色、clip 进行带与卡区）。今天区与明天区行流同语义。
 *       - 固定日程（name 首字符非 ※）渲染零变化（不画线）。
 *       - hp1_item_t / kidpal_home_p1_schedule_* seam 形状零改（PCN TEST 5 canary
 *         继续在世；本文件 [M] home_p1_h_no_temp_field 负向键承载「不扩字段」）。
 *       - 结构锚 = home_p1.c 含 marker 常量字面 "0x203B"（FAS-MANIFEST 键）。
 *
 *   [G1] text_yoff 16px 重基线锚（F-2「只调 text_yoff」；home = home_p1.c
 *        p1_draw_card 参数族）:
 *       const int text_yoff = 3;   -- 16px 字形盒在 22px 行带内垂直居中
 *       （(22-16)/2 = 3；V8 canvas-p1 行内文字垂直锚——带内居中续接 14px yoff=4 的
 *       既有居中口径 4/4 -> 3/3。线距 22 与行锚点族（ty+8/ty+13/ty+43）零改 = F-2
 *       容量零损失）。结构锚 = FAS-MANIFEST 键 home_p1.c 含 "const int text_yoff
 *       = 3"（设计态 = 旧值 4，键 0 红）。**容量表零损失由 p1_home_battery TEST
 *       4.5 既有齿（行带 155-176/177-198/199-220 + 组级容纳门）继续承载——本包
 *       supersede 只重基线行墨阈值，行带矩形与 line_h=22 不撤**。
 *
 *   [M] FAS-MANIFEST 扩键（结构面断言采集通道；**判 = 在既有 FAS 清单块上扩键**，
 *       不建独立清单——理由：同一 configure 期采集机制、同一构建产物、同一
 *       CMAKE_CONFIGURE_DEPENDS 新鲜度纪律，双清单只增维护面不增判别力；P0
 *       font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节机制全文适用）:
 *         host_srcs_p1_home_16px      本验收件编入 HOST_ACCEPTANCE_SRCS
 *         p1_kidpalfont_h_s7_decl     kidpal_p1_font.h 含 "kidpal_p1_font_face"
 *         p1_rn_s1_face_seam          kidpal_render_node.cc 含 "kidpal_p1_font_face"
 *         p1_hostseam_s1_face_seam    host_infra_seam.c 含 "kidpal_p1_font_face"
 *         p1_home_text_yoff16         home_p1.c 含 "const int text_yoff = 3"
 *         p1_home_c_marker_const      home_p1.c 含 "0x203B"
 *         p1_home_p1_h_no_temp_field  home_p1.h **不**含 "flags"（负向事实——
 *                                     不把临时标记扩成新字段；现有 hp1_state_t
 *                                     无 flags 词，负向探针无碰撞）
 *       被扫文件新增 CMAKE_CONFIGURE_DEPENDS 登记：home_p1.c / home_p1.h /
 *       kidpal_p1_font.h / host_app/main/host_infra_seam.c（接线增量见
 *       delivery/taskpackages/261028-m6-k2a-p1-home-16px/acceptance-wiring-patch.md）。
 *
 * ---------------------------------------------------------------------------
 * 跨端申报（acceptance 纪律：跨端 wire 形状断言对照对端实际行为钉，不按本侧想象）:
 *   对端 = server/gateway/schedule.py home_upcoming_payload（crossEnd counterpart，
 *   本包 launch-card 钉零 producer diff）。实测：producer 逐项 {time,name} verbatim
 *   ——**现网 producer 不往 name 写任何临时标记**（一次性安排的临时性在服务端
 *   group_upcoming/week_grid 由 flags bit0 承载，home 载荷剥 flags）。因此 [U1]
 *   钉的「name 首字符 ※ = 临时」是 firmware 侧消费约定；**上游（front/OpenClaw
 *   写入侧）须按该约定在临时事件 name 落 '※' 前缀**，否则本机制 host 全绿而真机
 *   永不触发。该上游约定缺口已由 acceptance-author 升级 planner（不在 Pod 内
 *   自裁改 producer——producer 改动违反本包零 producer diff 事实与铁律）；planner
 *   裁定前 [U1] 断言照钉（firmware 消费半是本包职责面，先行冻结不阻塞）。
 *
 * ---------------------------------------------------------------------------
 * 契约条目 ↔ 断言的强度申报（acceptance-evidence.md 体例）:
 *
 *   P16-A1（P1H-E1/F-1 正文切 16px 运行时句柄）
 *     -> TEST 1 承载：行为级（[S1] 字形输出 == body 句柄字形输出逐字节——机制级
 *        消费事实的行为本体兑现）+ 机制级（[M] 三落点键 + TEST 1 tooth 3 [S7]==body
 *        直钉）。判别力负控 = tooth 1（14px/16px 度量确可判别，防 body 意外 14px
 *        时等价断言假绿）。弱于契约处显式声明：设备侧 [S1] 真消费行为归 device
 *        build 闸 + 真机轮（三分申报），host 半经同构 core 字节等价承载（F-7
 *        同源假设，P0 裁定 2 口径）。
 *   P16-A2（P1H-E1/F-2 容量零损失 + 帧几何 + P1H-E2/F-9 下划线首页半）
 *     -> TEST 2 承载：行为级（三条 preview 行带整行墨 = 容量零损失、行带
 *        containment、temp/fixed 帧 diff 定位 = 下划线在世 + 前缀退役 + 固定不画
 *        线 + 2px 随名宽）。容量/行带语义与 p1_home_battery TEST 4.5 既有齿并列
 *        互证（彼文件承载 22px 参数族与组级门，本文件承载 16px 面下的行墨）。
 *        弱于契约处：下划线 x 跨度只钉下界/上界窗（12..60 px/行）与 2 行高，不钉
 *        逐像素形状——像素级形态归 design review（P1H-E4）+ 真机轮。
 *   P16-A3（P1H-E3 结构面 + 接线守卫）
 *     -> TEST 3 承载：构建清单档（[M] 七键，configure 期事实）。行为本体由
 *        TEST 1/2 兑现，两档并列互证。负向键 home_p1_h_no_temp_field = 「不扩
 *        字段」的机制级承载（正则级防writability：任何 flags/temp 字段词入
 *        home_p1.h 即红）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map（acceptanceRef -> test）:
 *   coverage: P16-A1 -- [S1] 字面切 16px body 句柄（行为级字节等价 + 负控 + [S7]==body 机制钉）-> TEST 1
 *   coverage: P16-A2 -- 16px 行容量零损失（三 preview 行带）+ 行带 containment + F-9 下划线/前缀退役/固定不画线/2px 随名宽（帧 diff）-> TEST 2
 *   coverage: P16-A3 -- FAS-MANIFEST 扩键结构半（七键 + 负向探针 + 清单自检）-> TEST 3
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定；接线增量逐字落
 * acceptance-wiring-patch.md）:
 *   1) firmware/main/display/kidpal_p1_font.h：[S7] 声明（签名逐字见 [F1]）。
 *   2) firmware/main/display/kidpal_render_node.cc：[S7] 设备强定义（经
 *      kidpal_render_node_body_font() 透传口）+ [S1] 字形获取改「[S7] 取字面 +
 *      NULL 回落 font_puhui_14_1」（ESP_PLATFORM 分支与 host 分支同改；回落分支
 *      保留——FAS-A7 canary 不破、14px 降级语义保留）。
 *   3) firmware/main/test/host_app/main/host_infra_seam.c：[S7] host 强定义
 *      （kidpal_font_body()）+ [S1] 同款切换（NULL 回落 source_han 子集）。
 *   4) firmware/main/display/home_p1.c：text_yoff 4->3（[G1]）+ [U1] 行流临时
 *      标记处理（'※' 前缀退役 + 名下 2px 下划线；marker 常量字面 0x203B 在世）。
 *   5) host 接线：host_app/main/CMakeLists.txt 注册本验收件（smoke 之前）+ FAS
 *      块扩 [M] 七键 + 新增 CMAKE_CONFIGURE_DEPENDS 四文件——逐字增量见
 *      acceptance-wiring-patch.md。
 *   6) mutation 自证（mutationRequired=true）：变异不得保留被测符号原字面为子串
 *      （KR-A3 教训）；预申报断言面 = TEST 1 tooth 2（[S1] 字面回退 14px -> 字节
 *      等价红）+ TEST 2 tooth 3/4/5（摘下划线 / 画回 ※ 字形 / 画线改 1px 或 3px
 *      -> 对应齿红）+ [M] 键回退（text_yoff 改回 4 / marker 常量删除 -> TEST 3
 *      红）；取证由 builder/fixer 实测、planner 代落 taskpackage
 *      mutationEvidence，本文件不落取证。
 *   7) device build 闸 + 真机（P1H-E4）：xiaozhi.bin 构建过；真机观感/降级复证
 *      归 P3 终端轮（本包不另占轮、不冒充终判）。
 *
 * ---------------------------------------------------------------------------
 * Run protocol（HOST_TEST_CONVENTIONS.md [C4.4]）:
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
 *      （或 cd firmware/main/test/host_app 后 ./build/host_infra_test.elf——本文件
 *      root 自手持 cwd 定位，两处均可）
 *   RED baseline 判读（本冻结时点，接线增量未落线）:
 *     - 本文件未编入 host runner（host_srcs_p1_home_16px 键尚不存在）——红基线
 *       通道 = C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4；编译通道 error
 *       为零）+ 解析性红定性（逐齿对现状必红的事实源，见文件尾「RED baseline
 *       判读口径」段）。builder 落 BUILDER DUTY 5) 后转 host runner 全量通道。
 *     - C 类实编命令（仓根执行；lvgl/kidpal_font 头经 managed_components 与
 *       display include 路径解析，unity.h 经 red-baseline 脚手架）:
 *       gcc -std=c11 -fsyntax-only -Wall -Wextra -Wno-unused-parameter \
 *         -I firmware/main/test/acceptance/red-baseline \
 *         -I firmware/main/display \
 *         -I firmware/managed_components/lvgl__lvgl \
 *         firmware/main/test/p1_home_16px.acceptance.c
 *     - 执行通道红基线（接线落线后）期望见文件尾判读段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

#include "lvgl.h"

#include "../display/kidpal_font.h"   /* P0 [K3]/[K4] 统一句柄 seam（强定义 =
                                       * display/kidpal_font.cc，host runner 在世） */
#include "../display/kidpal_p1_font.h" /* [S1] 字形 seam 生产侧声明（签名零偏移；
                                        * kidpal_p1_glyph_t 与本文件哨兵/局部类型同源） */
#include "../display/home_p1.h"       /* hp1_item_t / hp1_state_t / seam 声明（形状
                                       * 零偏移消费；强定义 = home_p1.c） */

/* 编译内 14px 兜底（font_puhui_14_1.c 已编入 host runner，强定义在世）。 */
extern const lv_font_t font_puhui_14_1;

/* -------------------------------------------------------------------------- *
 * Seam 弱哨兵（强定义 = host_infra_seam.c / home_p1.c；多重 weak 合法先例）。
 * P16_UNIMPLEMENTED(-2) = 「frozen seam not implemented」解析性红标记。
 * ------------------------------------------------------------------------ */
#define P16_UNIMPLEMENTED (-2)

/* [S6]（p1_cjk_name 第三追加轮追加 seam——本文件消费其 host 强符号做逐字节 oracle）。
 * 字形类型直接用 kidpal_p1_font.h 的 kidpal_p1_glyph_t（[S1] 声明同源, 防跨类型
 * 指针告警; ABI 与 pcn_glyph_t/kidpal_p1_glyph_t 先例一致）。 */

__attribute__((weak)) int kidpal_p1_font_glyph_for_font(const lv_font_t *font,
                                                        uint32_t codepoint,
                                                        kidpal_p1_glyph_t *out)
{
    (void)font; (void)codepoint; (void)out;
    return -1; /* 弱默认 = 不可用（强定义在世时覆盖） */
}

/* [S7]（本包新增，[F1]）：weak 哨兵 = 未实现（NULL）——TEST 1 tooth 3 设计态红。 */
__attribute__((weak)) const lv_font_t *kidpal_p1_font_face(void)
{
    return NULL;
}

/* home_p1 seam 弱哨兵（HPB/PCN 同款：独立编译路径下 -2 = 解析性红）。 */
__attribute__((weak)) int kidpal_home_p1_schedule_inject(const char *day_key,
                                                         const hp1_item_t *items,
                                                         int count,
                                                         const hp1_item_t *next_day_items,
                                                         int next_day_count)
{
    (void)day_key; (void)items; (void)count;
    (void)next_day_items; (void)next_day_count;
    return P16_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_set_state(const hp1_state_t *st)
{
    (void)st;
    return P16_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_render_frame(void)
{
    return P16_UNIMPLEMENTED;
}

/* host 渲染缝（host_infra_seam.c 提供，恒链入 runner）。只探针。 */
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes / regions（帧规格 == home_p1.c P1_FB_*；行带 = line_h=22
 * 参数族 @ 2026-10-13 补充轮锚，F-2 零改——本文件消费、不重锚）。
 * ------------------------------------------------------------------------ */
#define P16_FRAME_W 400
#define P16_FRAME_H 300
#define P16_INK_WHITE 0xFFFFFFul
#define P16_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 16px 行带整行墨下界（P16-A2 容量齿；重基线依据见文件尾 REVISION LOG 预留段与
 * p1_home_battery HPB_INK_MIN_PV_ROW2 2026-10-29 重基线注记——两文件同源同值）。 */
#define P16_INK_MIN_ROW 130

/* 临时下划线判读窗（[U1]；名墨底缘下方相邻行起 2px——判读以帧 diff 动态定位，
 * 本窗只做行归属上限：diff 行必须落在行带内且 > 名墨最大行）。 */
#define P16_UNDERLINE_MIN_PX_PER_ROW 12  /* 1 字 16px 名的画线宽下界（随名宽） */
#define P16_UNDERLINE_MAX_PX_PER_ROW 60  /* 防整带画线错形上界（2 字名 ≈ 32px） */

/* preview 行带矩形 {x1, y1, x2, y2} 含端点（今天空 s_count=0 -> ty=112 ->
 * 项行 ty+43 = 155 起；line_h=22 三行容量——p1_home_battery hpb_rect_pv_row* 同源）。 */
static const int p16_rect_pv_row1[4] = {152, 155, 392, 176};
static const int p16_rect_pv_row2[4] = {152, 177, 392, 198};
static const int p16_rect_pv_row3[4] = {152, 199, 392, 220};
/* 卡区底界之下退役负断言带（p1_home_battery hpb_rect_preview_neg 同源同值）。 */
static const int p16_rect_below_card[4] = {152, 244, 392, 258};

/* [U1] marker 常量（结构锚字面，与 home_p1.c 实现常量同值）。 */
#define P16_MARKER_CP 0x203Bu /* '※' U+203B */

/* FAS-MANIFEST 构建产物清单路径（repo 根相对；P0 font_assets_runtime 同款——
 * 运行时只读构建产物，不扫源码树）。 */
#define P16_FAS_MANIFEST "firmware/main/test/host_app/build/fas_structure_manifest.txt"

/* -------------------------------------------------------------------------- *
 * Repo-root 定位（pcn_/fas_ 先例同款：root 自手持 cwd 定位，相对路径——ELF fopen
 * 不认 /mnt/c 绝对路径）。定位只服务清单产物 / fonts_root 路径解析，不扫源文件。
 * ------------------------------------------------------------------------ */
#define P16_MAX_PATH 1024
#define P16_MAX_READ (8u * 1024u * 1024u)

static char p16_root[P16_MAX_PATH];
static int p16_root_ready;

static int p16_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int p16_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[P16_MAX_PATH];
    size_t i;

    if (p16_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (p16_is_regular(probe)) {
            snprintf(p16_root, sizeof(p16_root), "%s", cands[i]);
            p16_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int p16_rel(char *out, size_t cap, const char *rel)
{
    if (!p16_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", p16_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", p16_root, rel);
    return 1;
}

static int p16_exists(const char *rel)
{
    char p[P16_MAX_PATH];
    return p16_rel(p, sizeof(p), rel) && p16_is_regular(p);
}

/* 读整文件（二进制）。 */
static uint8_t *p16_read_bin(const char *rel, size_t *out_len)
{
    char p[P16_MAX_PATH];
    FILE *f;
    long sz;
    uint8_t *buf;

    if (out_len != NULL) {
        *out_len = 0;
    }
    if (!p16_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > P16_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (uint8_t *)malloc((size_t)sz);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

/* 文本读（NUL 结尾；清单消费用）。 */
static char *p16_read_text(const char *rel)
{
    size_t len;
    uint8_t *raw = p16_read_bin(rel, &len);
    char *buf;
    if (raw == NULL) {
        return NULL;
    }
    buf = (char *)malloc((size_t)len + 1);
    assert(buf != NULL);
    memcpy(buf, raw, len);
    buf[len] = '\0';
    free(raw);
    return buf;
}

static int p16_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL);
    return strstr(hay, needle) != NULL;
}

#define P16_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(p16_locate_root(), \
    "P16 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static char p16_fm_buf[1024];
static const char *p16_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *p16_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(p16_fm_buf, sizeof(p16_fm_buf), fmt, ap);
    va_end(ap);
    return p16_fm_buf;
}

static void p16_require_seam(int rc, const char *what)
{
    if (rc == P16_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "P16 RED: frozen seam NOT implemented/wired — %s (builder must land "
                 "[F1]/[U1] per the contract in p1_home_16px.acceptance.c)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * FAS-MANIFEST 消费面（fas_mfact 同款：行首锚定 key=，未知键返回 -1 = infra 红
 * 语义；机制全文见 P0 font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节）。
 * ------------------------------------------------------------------------ */
static int p16_mfact(const char *key)
{
    static char cache[8 * 1024];
    static char val[32];
    static int loaded;
    const char *line;
    size_t klen = strlen(key);

    if (!loaded) {
        char *txt = p16_read_text(P16_FAS_MANIFEST);
        loaded = 1;
        if (txt != NULL) {
            if (strlen(txt) < sizeof(cache)) {
                memcpy(cache, txt, strlen(txt) + 1);
            }
            free(txt);
        }
    }
    for (line = cache; (line = strstr(line, key)) != NULL; line += 1) {
        const char *v;
        const char *e;
        size_t n;
        if ((line != cache && line[-1] != '\n') || line[klen] != '=') {
            continue;
        }
        v = line + klen + 1;
        e = strchr(v, '\n');
        n = (e != NULL) ? (size_t)(e - v) : strlen(v);
        if (n == 0 || n >= sizeof(val)) {
            return -1;
        }
        memcpy(val, v, n);
        val[n] = '\0';
        return atoi(val);
    }
    return -1;
}

static void p16_require_fact(int fact, const char *key, const char *what)
{
    if (fact == -1) {
        TEST_ASSERT_TRUE_MESSAGE(0, p16_fm(
            "P16 RED(infra): 结构清单键 %s 缺失或清单未生成——重跑 host build 让 "
            "configure 期重采集（键集 = acceptance-wiring-patch.md 增量 5；机制见 "
            "P0 font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节）", key));
        return;
    }
    TEST_ASSERT_TRUE_MESSAGE(fact == 1, p16_fm(
        "P16 RED: 结构事实 %s 不成立——%s（事实由 host CMakeLists configure 期采集，"
        "机制见冻结件头注 [M] 节）", key, what));
}

/* -------------------------------------------------------------------------- *
 * [K1] host 测试 reader：fonts_root 直读（F-7 同源；P0 fas_host_reader 同款）。
 * ------------------------------------------------------------------------ */
#define P16_FONTS_ROOT "firmware/main/assets/fonts_root"

static int p16_host_reader(const char *name, const uint8_t **data, size_t *size)
{
    char rel[P16_MAX_PATH];
    uint8_t *buf;
    size_t len;

    snprintf(rel, sizeof(rel), "%s/%s", P16_FONTS_ROOT, name);
    buf = p16_read_bin(rel, &len);
    if (buf == NULL) {
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

/* 全缺 reader（复位 14px 兜底态用——mount(NULL 等价语义的全缺 reader) 后 body()/
 * title() 恒等 font_puhui_14_1，P0 FAS-A4 语义；防本文件 16px mount 态泄漏进
 * 后续执行的其它冻结件）。 */
static int p16_missing_reader(const char *name, const uint8_t **data, size_t *size)
{
    (void)name; (void)data; (void)size;
    return -1;
}

/* -------------------------------------------------------------------------- *
 * 帧探针 helpers（pcn_ 先例同款）：区内墨计数 / 整帧快照 / diff 统计（总数 +
 * 落带数）/ 带内最大墨行。探针失败（未渲染）大声红，绝不读成零墨。
 * ------------------------------------------------------------------------ */
static int p16_count_ink(const int rect[4])
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == P16_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            if ((uint32_t)px != P16_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

static int32_t p16_f_a[P16_FRAME_W * P16_FRAME_H];
static int32_t p16_f_b[P16_FRAME_W * P16_FRAME_H];

static int p16_snap(int32_t *buf)
{
    int x, y;

    for (y = 0; y < P16_FRAME_H; y++) {
        for (x = 0; x < P16_FRAME_W; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == P16_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            buf[y * P16_FRAME_W + x] = px;
        }
    }
    return 0;
}

static void p16_diff_stats(const int32_t *a, const int32_t *b, const int rect[4],
                           int *out_total, int *out_in_band)
{
    int total = 0, in_band = 0;
    int x, y;

    for (y = 0; y < P16_FRAME_H; y++) {
        for (x = 0; x < P16_FRAME_W; x++) {
            if (a[y * P16_FRAME_W + x] != b[y * P16_FRAME_W + x]) {
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

/* rect 带内（取自快照帧）最大墨行号；无墨返回 rect[1]-1（调用方先断言有墨）。 */
static int p16_max_ink_row(const int rect[4], const int32_t *f)
{
    int x, y;
    int max_row = rect[1] - 1;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            if (f[y * P16_FRAME_W + x] != (int32_t)P16_INK_WHITE) {
                max_row = y;
                break;
            }
        }
    }
    return max_row;
}

/* rect 带内（快照帧）行 y 的墨像素数。 */
static int p16_row_ink(const int rect[4], const int32_t *f, int y)
{
    int x;
    int ink = 0;

    for (x = rect[0]; x <= rect[2]; x++) {
        if (f[y * P16_FRAME_W + x] != (int32_t)P16_INK_WHITE) {
            ink++;
        }
    }
    return ink;
}

/* rect 带内逐行 diff 像素数（a vs b），out_rows = 有 diff 的离散行数、
 * out_min/max_row = diff 行界。 */
static void p16_diff_rows(const int32_t *a, const int32_t *b, const int rect[4],
                          int *out_rows, int *out_min_row, int *out_max_row)
{
    int x, y;
    int rows = 0, min_row = -1, max_row = -1;

    for (y = rect[1]; y <= rect[3]; y++) {
        int row_diff = 0;
        for (x = rect[0]; x <= rect[2]; x++) {
            if (a[y * P16_FRAME_W + x] != b[y * P16_FRAME_W + x]) {
                row_diff++;
            }
        }
        if (row_diff > 0) {
            rows++;
            if (min_row < 0) {
                min_row = y;
            }
            max_row = y;
        }
    }
    *out_rows = rows;
    *out_min_row = min_row;
    *out_max_row = max_row;
}

/* -------------------------------------------------------------------------- *
 * Fixture helpers。
 * ------------------------------------------------------------------------ */
static void p16_set_fixture(void)
{
    hp1_state_t st;
    int rc;

    memset(&st, 0, sizeof(st));
    st.hour = 10;
    st.minute = 24;
    st.wday = 1;       /* 周二（canvas-p1 "Tue 22 Sep" 同款；不靠零值拿假星期） */
    st.env_valid = 1;
    st.temp_c = 22;
    st.humidity_pct = 45;
    st.battery_pct = 85;
    st.charging = 0;
    st.tick_ms = 0;
    rc = kidpal_home_p1_set_state(&st);
    p16_require_seam(rc, "kidpal_home_p1_set_state");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): set_state fixture rejected (rc=%d)", rc));
}

/* preview 三项注入：item0 = temp 名（"※春游"）或 fixed 名（"春游"），item1/2 固定。
 * today 空（AC5 话术态, s_count=0）-> ty=112 -> preview 三行带 155/177/199
 * （容量齿判读面；p1_home_battery TEST 4.5 tooth 4 同款夹具族）。 */
static void p16_inject_preview3(const char *item0_name)
{
    hp1_item_t pv[3];
    int rc;

    memset(pv, 0, sizeof(pv));
    snprintf(pv[0].time, sizeof(pv[0].time), "10:30");
    snprintf(pv[0].name, sizeof(pv[0].name), "%s", item0_name);
    snprintf(pv[1].time, sizeof(pv[1].time), "19:00");
    snprintf(pv[1].name, sizeof(pv[1].name), "编程");
    snprintf(pv[2].time, sizeof(pv[2].time), "20:00");
    snprintf(pv[2].name, sizeof(pv[2].name), "游泳课");

    rc = kidpal_home_p1_schedule_inject("2026-10-29", NULL, 0, pv, 3);
    p16_require_seam(rc, "kidpal_home_p1_schedule_inject");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): schedule_inject(preview3) rejected (rc=%d)", rc));
}

static void p16_render_ok(void)
{
    int rc = kidpal_home_p1_render_frame();
    p16_require_seam(rc, "kidpal_home_p1_render_frame");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): render_frame rc=%d——P1 帧渲染失败", rc));
}

/* ========================================================================== *
 * TEST 1 — P16-A1：[S1] 字面切 16px body 句柄（行为级 + 负控 + 机制钉）
 *   tooth 0 (infra gate): mount fonts_root -> body() 真资产句柄（非 14px、16 档、
 *            「中」glyph 可查）+ tooth 0b 字形度量完整性 gate（cbin glyph dsc 布局
 *            与消费侧 LV_FONT_FMT_TXT_LARGE 构建旗匹配性——P0 资产链缺陷的
 *            自诊断齿, 2026-10-29 冻结时点实测红 = P0 交付面缺陷, 升级项）。
 *   tooth 1 (判别力负控): 探针码点中至少一个 [S6](puhui14) 与 [S6](body16) 度量
 *            不同——14px/16px 可判别（防 body 意外 14px 时 tooth 2 等价假绿；
 *            全同 = infra 红自诊断，不静默）。
 *   tooth 2 (行为级): 探针码点 [S1](cp) 输出 == [S6](kidpal_font_body(), cp) 逐字节
 *            （w/h/advance + bits）——[S1] 两强符号真消费 16px body 句柄（[F1]）。
 *   tooth 3 (机制级): kidpal_p1_font_face() 非 NULL 且 == kidpal_font_body()
 *            （[S7] 直钉；weak 哨兵在世 = NULL = 设计态红）。
 *   tooth 4 (复位): mount(missing reader) 复位 14px 兜底态——防本文件 mount 态
 *            泄漏进后续执行的其它冻结件（执行序 = HOST_ACCEPTANCE_SRCS 逆序，
 *            smoke 末位最先执行；复位后本齿在 TEST_CASE 尾部执行）。
 *   RED（设计态）: [S1] host 强符号仍 source_han 14px（tooth 2 红）+ [S7] weak
 *            哨兵 NULL（tooth 3 红）；tooth 0 现状即绿（P0 已交付）。
 * ========================================================================== */
/* coverage: P16-A1 -- [S1] 字面切 16px body 句柄：[S1]==[S6](body) 字节等价 + 14/16 判别负控 + [S7]==body 机制钉 + mount 态复位 */
TEST_CASE("p1 home 16px: [S1]字面切16px body句柄 (P16-A1)",
          "[p1_home_16px][P16-A1][behavior]")
{
    static const uint32_t probe_cps[] = {
        0x8BFEu, /* 课 */ 0x7A0Bu, /* 程 */ 0x94A2u, /* 钢 */ 0x7434u, /* 琴 */
        0x6625u, /* 春 */ 0x6E38u, /* 游 */ 0x0038u, /* '8' */ 0x0057u, /* 'W' */
    };
    /* 探针集取材 = p1_cjk_name A11 已核验的 font_puhui_14_1 在场码点（tooth 1 负控
     * 两态可判别）∪ 16px 全量 CJK 必含——两态字形集交集，防兜底态缺字假红。 */
    static const uint32_t t2_cps[] = {
        0x0038u, /* '8' */ 0x0057u, /* 'W' */ 0x003Au, /* ':' */
        0x6625u, /* 春 */ 0x4F53u, /* 体 */ 0x80B2u, /* 育 */ 0x7A0Bu, /* 程 */
    };
    /* tooth 2 探针集 = host 子集 ∩ puhui14 兜底面 ∩ 16px 全量面三字面态全在场
     * （p1_cjk_name pcn_a11_t5_cps 同源同值）——本齿设计态红 = 度量/字节不一致
     * 判别器（[S1] 旧 source_han 字面对这三态码点全部在场）, 不以「码点缺场」
     * 偶然红冒充。 */
    const lv_font_t *body;
    kidpal_p1_glyph_t g_s1, g_body, g_14;
    int discriminated = 0;
    int rc;
    size_t i;

    P16_REQUIRE_ROOT();

    /* tooth 0: 16px 真资产句柄（infra gate）。 */
    rc = kidpal_font_mount_assets(p16_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): mount_assets(rc=%d) 非完成态——P0 资产/装载底座缺场"
               "（font_body_16.bin / kidpal_font.cc 接线，P0 包交付面）", rc));
    body = kidpal_font_body();
    TEST_ASSERT_TRUE_MESSAGE(body != NULL,
        "P16 RED(infra): kidpal_font_body() == NULL——[K4] 消费口缺场（P0 交付面）");
    TEST_ASSERT_TRUE_MESSAGE(body != &font_puhui_14_1,
        "P16 RED(infra): body() == font_puhui_14_1——资产在库却落 14px 兜底（P0 "
        "装载面断裂，非本包设计态）");
    TEST_ASSERT_TRUE_MESSAGE(body->line_height >= 20 && body->line_height <= 25,
        p16_fm("P16 RED(infra): body() line_height=%d 越出 16px 档 [20..25]（P0 "
               "FAS-A2 标定档）", (int)body->line_height));
    /* tooth 0b (P0 链完整性 gate, 2026-10-29 冻结时点新增; 2026-10-29 architect
     * advisory 文字更正——host 单侧错配): 16px 句柄字形度量完整性——cbin 装载后
     * 的 glyph dsc 必须给出 16px 量级的合理盒（16px CJK box 4..32）。冻结时点
     * 实测红：78 fork cbin writer 恒产 LV_FONT_FMT_TXT_LARGE=1 16 字节 glyph dsc
     * 布局（tools/asset-pipeline/node_modules/lv_font_conv/lib/writers/cbin/
     * lv_table_glyf.js toCBin 硬编码），而 **host 侧** LVGL 构建未开该旗
     * （host_app sdkconfig :665 not set）→ lv_font_get_glyph_dsc_fmt_txt 按
     * 8 字节 LARGE=0 布局误读 → 盒度量垃圾（实测 '8' box=71x0 / '中' box=0x1）
     * → 16px/20px cbin 资产在 host 半不可正确消费。**设备侧 sdkconfig.defaults:57
     * CONFIG_LV_FONT_FMT_TXT_LARGE=y（P0 时点已如此）——host 单侧错配已证实
     * （architect advisory 2026-10-29），设备行为待 FAR-E3 真机验证**。缺陷归属
     * = P0 fixer 轮（architect 裁定: 仅 host 开 LARGE——host_app/sdkconfig.defaults
     * + 完整重编; font_puhui_14_1 指定初始化器自动重排无需重产; A11 独立参考
     * 解码器按字段名取值不受布局步长影响），不由 P1 代偿。修后本齿转绿。 */
    {
        lv_font_glyph_dsc_t g0;
        int ok0 = lv_font_get_glyph_dsc(body, &g0, 0x4E2Du /* 中 */, 0);
        TEST_ASSERT_TRUE_MESSAGE(ok0 && g0.box_w >= 4 && g0.box_w <= 32 &&
                                     g0.box_h >= 4 && g0.box_h <= 32,
            p16_fm("P16 RED(P0-chain): body16 字形度量完整性 gate 失败（'中' "
                   "dsc_ok=%d box=%dx%d）——cbin glyph dsc 布局与消费侧 LVGL "
                   "LV_FONT_FMT_TXT_LARGE 构建旗不匹配（**host 单侧错配已证实**: "
                   "host sdkconfig not set vs 78 fork cbin writer 恒产 LARGE=1 "
                   "16 字节布局; 设备 sdkconfig.defaults:57 LARGE=y 在案、真机"
                   "消费验证归 FAR-E3）= P0 资产链缺陷, 修复归属 P0 fixer 轮, "
                   "非本包设计态", ok0, ok0 ? (int)g0.box_w : -1,
                   ok0 ? (int)g0.box_h : -1));
    }

    /* tooth 1: 判别力负控——14px 与 16px 字形度量确可判别。 */
    for (i = 0; i < sizeof(probe_cps) / sizeof(probe_cps[0]); i++) {
        uint32_t cp = probe_cps[i];
        rc = kidpal_p1_font_glyph_for_font(&font_puhui_14_1, cp, &g_14);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            p16_fm("P16 RED(infra): [S6] 对 font_puhui_14_1 U+%04X 不可用 rc=%d"
                   "——14px 兜底字形缺场（host 编入面被拆）", cp, rc));
        rc = kidpal_p1_font_glyph_for_font(body, cp, &g_body);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            p16_fm("P16 RED(infra): [S6] 对 body16 U+%04X 不可用 rc=%d——16px 资产"
                   "字形缺口（gen_fonts 字表，P0 交付面）", cp, rc));
        if (g_14.w != g_body.w || g_14.h != g_body.h ||
            g_14.advance != g_body.advance) {
            discriminated = 1;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(discriminated,
        "P16 RED(infra): 探针码点上 14px 与 16px 字形度量全同——tooth 2 等价断言"
        "失去判别力（body 意外 14px 形态或探针集漂移），infra 修正不静默");

    /* tooth 2: [S1] == [S6](body16) 逐字节（[F1] 行为本体）。 */
    for (i = 0; i < sizeof(t2_cps) / sizeof(t2_cps[0]); i++) {
        uint32_t cp = t2_cps[i];
        int stride, b, mm = -1;

        rc = kidpal_p1_font_glyph(cp, &g_s1); /* [S1] host 入口 */
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            p16_fm("P16-A1 RED: [S1] 对 U+%04X 不可用 rc=%d——[S1] 字形获取断裂或"
                   "字面态缺该码点（三态探针集下不应发生, infra 修正不静默）",
                   cp, rc));
        {
            unsigned char snap_s1[512];
            stride = (g_s1.w + 7) / 8;
            memcpy(snap_s1, g_s1.bits, (size_t)(g_s1.h * stride));
            rc = kidpal_p1_font_glyph_for_font(body, cp, &g_body);
            TEST_ASSERT_TRUE_MESSAGE(rc == 0,
                p16_fm("P16 RED(infra): [S6](body) U+%04X 不可用 rc=%d", cp, rc));
            TEST_ASSERT_TRUE_MESSAGE(g_s1.w == g_body.w && g_s1.h == g_body.h &&
                                         g_s1.advance == g_body.advance,
                p16_fm("P16-A1 RED: [S1] 字形 U+%04X 度量 %dx%d/%d != body16 %dx%d/%d"
                       "——[S1] 未切 16px body 句柄（[F1] 未实现, 设计态红；host 强"
                       "符号仍 source_han 14px 字面）", cp, g_s1.w, g_s1.h,
                       g_s1.advance, g_body.w, g_body.h, g_body.advance));
            {
                unsigned char snap_body[512];
                int bstride = (g_body.w + 7) / 8;
                memcpy(snap_body, g_body.bits, (size_t)(g_body.h * bstride));
                for (b = 0; b < g_body.h * bstride; b++) {
                    if (snap_s1[b] != snap_body[b]) {
                        mm = b;
                        break;
                    }
                }
            }
            TEST_ASSERT_TRUE_MESSAGE(mm < 0,
                p16_fm("P16-A1 RED: [S1] 与 body16 字形 U+%04X 第 %d 字节不一致"
                       "——[S1] 消费面未切 16px（[F1]）", cp, mm));
        }
        (void)stride;
    }

    /* tooth 3: [S7] 机制钉。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_p1_font_face() != NULL,
        "P16-A1 RED: kidpal_p1_font_face() == NULL——[S7] 字面源缝未实现（weak 哨兵"
        "在世, 设计态红; [F1]）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_p1_font_face() == kidpal_font_body(),
        "P16-A1 RED: kidpal_p1_font_face() != kidpal_font_body()——[S7] 未消费 P0 "
        "16px body 句柄（字面源脱钩, [F1]）");

    /* tooth 4: 复位 14px 兜底态（防 mount 态跨文件泄漏；P0 FAS-A4 全缺 = 合法完成态）。 */
    rc = kidpal_font_mount_assets(p16_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): 复位 mount rc=%d——全缺 reader 非完成态（P0 [K3] "
               "降级语义被破）", rc));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_body() == &font_puhui_14_1,
        "P16 RED(infra): 复位后 body() != font_puhui_14_1——14px 兜底语义被破");
}

/* ========================================================================== *
 * TEST 2 — P16-A2：16px 帧几何（容量零损失 + containment）+ F-9 下划线首页半
 *   夹具 = 今天空 + preview 3 项（item0 temp "※春游" vs fixed "春游" 两帧）。
 *   tooth 1: 三条 preview 行带整行墨 >= P16_INK_MIN_ROW（16px 行容量零损失, F-2;
 *            基线 14px 面 ~157 亦过阈 = MUST-STAY-GREEN——本齿钉容量不变式,
 *            16px 切换由 TEST 1 承载）。
 *   tooth 2: containment——卡底之下负断言带零墨（temp 帧渲染溢出走截断降级）。
 *   tooth 3 (F-9 下划线在世): temp 帧 vs fixed 帧 diff > 0 且全部落 item0 行带。
 *   tooth 4 (前缀退役): 全部 diff 行 > fixed 帧 item0 行带名墨最大行——diff 只在
 *            名墨之下（= 下划线），※ 字形/文字位移若在世必侵文字行 -> 红。
 *   tooth 5 (2px 随名宽): diff 离散行数 == 2 且各行 diff 像素 ∈
 *            [P16_UNDERLINE_MIN_PX_PER_ROW .. P16_UNDERLINE_MAX_PX_PER_ROW]。
 *   tooth 6 (固定日程不画线): fixed 帧 item0/item1/item2 行带内名墨最大行之下的
 *            带内剩余行严格零墨。
 *   RED（设计态）: tooth 3/4/5 红（下划线未实现——[S1] 现状对 ※ 无标记处理：
 *            host 字面缺 ※ 字形则 diff=0 -> tooth 3 红；有 ※ 字形则 ※ 墨/位移入
 *            文字行 -> tooth 4 红）；tooth 1/2/6 现状即绿（容量/containment/
 *            固定零画线不变式在世）。
 * ========================================================================== */
/* coverage: P16-A2 -- 16px 行容量零损失（三 preview 行带整行墨）+ 卡底 containment + F-9 下划线在世/前缀退役/2px 随名宽/固定不画线（帧 diff 定位） */
TEST_CASE("p1 home 16px: 行容量不变+F-9临时下划线 (P16-A2)",
          "[p1_home_16px][P16-A2][behavior]")
{
    int rc, ink, total, in_band, rows, min_row, max_row, m, y;

    P16_REQUIRE_ROOT();

    /* 16px mount（[S1] 切换后真消费；基线态 [S1] 仍 14px——容量齿两态均过阈）。 */
    rc = kidpal_font_mount_assets(p16_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): mount_assets(rc=%d)——P0 底座缺场（同 TEST 1 tooth 0）",
               rc));

    p16_set_fixture();

    /* temp 帧（item0 = "※春游"）。 */
    p16_inject_preview3("※春游");
    p16_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(p16_snap(p16_f_a) == 0,
        "P16 RED(infra): frame not rendered — 探针缝断裂");

    /* tooth 1: 三行带整行墨（容量零损失, F-2）。 */
    ink = p16_count_ink(p16_rect_pv_row1);
    TEST_ASSERT_TRUE_MESSAGE(ink >= P16_INK_MIN_ROW,
        p16_fm("P16-A2 RED: preview 第 1 行带墨 %d < %d——16px 行流未整行落带"
               "（容量损失或行带错位, F-2）", ink, P16_INK_MIN_ROW));
    ink = p16_count_ink(p16_rect_pv_row2);
    TEST_ASSERT_TRUE_MESSAGE(ink >= P16_INK_MIN_ROW,
        p16_fm("P16-A2 RED: preview 第 2 行带墨 %d < %d——容量零损失被破", ink,
               P16_INK_MIN_ROW));
    ink = p16_count_ink(p16_rect_pv_row3);
    TEST_ASSERT_TRUE_MESSAGE(ink >= P16_INK_MIN_ROW,
        p16_fm("P16-A2 RED: preview 第 3 行带墨 %d < %d——容量零损失被破", ink,
               P16_INK_MIN_ROW));

    /* tooth 2: 卡底 containment。 */
    ink = p16_count_ink(p16_rect_below_card);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        p16_fm("P16-A2 RED: 卡底之下负断言带 {152,244,392,258} 墨 %d != 0——16px "
               "行流越卡区底界 y242（渲染溢出必须截断降级）", ink));

    /* fixed 帧（item0 = "春游"，其余同）。 */
    p16_inject_preview3("春游");
    p16_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(p16_snap(p16_f_b) == 0,
        "P16 RED(infra): frame not rendered");

    /* tooth 3: 下划线在世——diff 非零且全落 item0 行带。 */
    p16_diff_stats(p16_f_a, p16_f_b, p16_rect_pv_row1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "P16-A2 RED: 临时名（※ 前缀）帧与固定名帧 diff 为零——临时事件下划线未"
        "实现且 ※ 字形未被画（[U1] 未落地, 设计态红；若实现误删 ※ 且未画线同红）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        p16_fm("P16-A2 RED: 临时标记帧差 %d/%d 落在 item0 行带 {152,155,392,176} "
               "之外——下划线溢出行带或渲染越区", total - in_band, total));

    /* tooth 4: 前缀退役——diff 只在名墨之下。 */
    m = p16_max_ink_row(p16_rect_pv_row1, p16_f_b);
    TEST_ASSERT_TRUE_MESSAGE(m >= p16_rect_pv_row1[1],
        "P16 RED(infra): fixed 帧 item0 行带无墨——夹具字形缺场（fixture 对齐"
        "协商, 不静默）");
    p16_diff_rows(p16_f_a, p16_f_b, p16_rect_pv_row1, &rows, &min_row, &max_row);
    TEST_ASSERT_TRUE_MESSAGE(min_row > m,
        p16_fm("P16-A2 RED: 临时标记帧差最早出现在行 %d <= fixed 帧名墨最大行 %d"
               "——※ 前缀字形未退役（被画进文字行）或名笔进被占（前缀宽未让位, "
               "[U1] ①）", min_row, m));

    /* tooth 5: 2px 随名宽。 */
    TEST_ASSERT_TRUE_MESSAGE(rows == 2,
        p16_fm("P16-A2 RED: 下划线离散 diff 行数 %d != 2——画线高度非 2px（[U1] "
               "③: 名墨底缘下方相邻行起 2px 横线）", rows));
    for (y = min_row; y <= max_row; y++) {
        int rd = p16_row_ink(p16_rect_pv_row1, p16_f_a, y) -
                 p16_row_ink(p16_rect_pv_row1, p16_f_b, y);
        if (rd < 0) {
            rd = -rd;
        }
        TEST_ASSERT_TRUE_MESSAGE(rd >= P16_UNDERLINE_MIN_PX_PER_ROW &&
                                     rd <= P16_UNDERLINE_MAX_PX_PER_ROW,
            p16_fm("P16-A2 RED: 下划线行 %d diff 像素 %d 越出 [%d..%d]——画线宽度"
                   "不随名宽（过窄 = 画线缺失/过宽 = 整带画线错形, [U1] ③）",
                   y, rd, P16_UNDERLINE_MIN_PX_PER_ROW,
                   P16_UNDERLINE_MAX_PX_PER_ROW));
    }

    /* tooth 6: 固定日程不画线——fixed 帧三行带名墨最大行之下的带内剩余行零墨。 */
    {
        static const int *bands[3] = {
            p16_rect_pv_row1, p16_rect_pv_row2, p16_rect_pv_row3,
        };
        int bi;
        for (bi = 0; bi < 3; bi++) {
            const int *rect = bands[bi];
            int mb = p16_max_ink_row(rect, p16_f_b);
            int clean = 1;
            TEST_ASSERT_TRUE_MESSAGE(mb >= rect[1],
                p16_fm("P16 RED(infra): fixed 帧 preview 行带 %d 无墨——夹具字形"
                       "缺场", bi + 1));
            for (y = mb + 1; y <= rect[3]; y++) {
                if (p16_row_ink(rect, p16_f_b, y) > 0) {
                    clean = 0;
                    break;
                }
            }
            TEST_ASSERT_TRUE_MESSAGE(clean,
                p16_fm("P16-A2 RED: fixed 帧 preview 第 %d 行带名墨最大行 %d 之下"
                       "仍有墨（行 %d）——固定日程被画线（[U1] 固定不画线义务被破）"
                       "或 16px 文字越出行带", bi + 1, mb, y));
        }
    }

    /* 复位 14px 兜底态（同 TEST 1 tooth 4——防跨文件泄漏）。 */
    rc = kidpal_font_mount_assets(p16_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        p16_fm("P16 RED(infra): 复位 mount rc=%d", rc));
}

/* ========================================================================== *
 * TEST 3 — P16-A3：FAS-MANIFEST 扩键结构半（构建清单档；[M] 七键）
 *   tooth 1: 接线/实现七键逐键（键缺 = infra 红自诊断；键 0 = 结构红设计态）。
 *   tooth 2: 清单面自检（版本键 + 负向探针——未知键返回缺；负向键
 *            p1_home_p1_h_no_temp_field 语义 = home_p1.h 不含 "flags"）。
 *   RED（设计态）: p1_rn_s1_face_seam / p1_hostseam_s1_face_seam /
 *            p1_kidpalfont_h_s7_decl（[S7] 未落）/ p1_home_text_yoff16（yoff 仍
 *            4）/ p1_home_c_marker_const（marker 常量未落）= 结构红；接线键缺 =
 *            infra 红（acceptance-wiring-patch.md 增量未落线）。
 * ========================================================================== */
/* coverage: P16-A3 -- FAS-MANIFEST 扩键结构半：[S7] 三落点 + text_yoff16 + marker 常量 + wire 不扩字段负向键 + 接线注册 */
TEST_CASE("p1 home 16px: 结构半-FAS清单扩键 (P16-A3)",
          "[p1_home_16px][P16-A3][structural]")
{
    int fact;

    P16_REQUIRE_ROOT();

    /* tooth 1: 七键。 */
    p16_require_fact(p16_mfact("host_srcs_p1_home_16px"), "host_srcs_p1_home_16px",
                     "p1_home_16px.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
                     "（acceptance-wiring-patch.md 增量 1）");
    p16_require_fact(p16_mfact("p1_kidpalfont_h_s7_decl"), "p1_kidpalfont_h_s7_decl",
                     "kidpal_p1_font.h 无 [S7] kidpal_p1_font_face 声明（BUILDER "
                     "DUTY 1)）");
    p16_require_fact(p16_mfact("p1_rn_s1_face_seam"), "p1_rn_s1_face_seam",
                     "kidpal_render_node.cc 无 [S7] 字面源缝——设备 [S1] 未切 16px "
                     "body 句柄（BUILDER DUTY 2)）");
    p16_require_fact(p16_mfact("p1_hostseam_s1_face_seam"), "p1_hostseam_s1_face_seam",
                     "host_infra_seam.c 无 [S7] 字面源缝——host [S1] 未切 16px body "
                     "句柄（BUILDER DUTY 3)）");
    p16_require_fact(p16_mfact("p1_home_text_yoff16"), "p1_home_text_yoff16",
                     "home_p1.c text_yoff 未按 [G1] 重基线为 3（16px 在 22px 行带"
                     "垂直居中；设计态旧值 4）");
    p16_require_fact(p16_mfact("p1_home_c_marker_const"), "p1_home_c_marker_const",
                     "home_p1.c 无 [U1] marker 常量 0x203B——临时标记识别未落地");

    /* 负向键：home_p1.h 不含 "flags"（不把临时标记扩成新字段；wire 形状零改）。 */
    fact = p16_mfact("p1_home_p1_h_no_temp_field");
    if (fact == -1) {
        TEST_ASSERT_TRUE_MESSAGE(0, p16_fm(
            "P16 RED(infra): 结构清单键 p1_home_p1_h_no_temp_field 缺失——接线增量"
            "未落线（acceptance-wiring-patch.md 增量 5）"));
    }
    TEST_ASSERT_TRUE_MESSAGE(fact == 1, p16_fm(
        "P16 RED: home_p1.h 出现 flags 词——临时标记被扩成新字段（[U1] 零新 wire "
        "字段铁律被破; 契约 §1.3⑥ {time,name} 形状零偏移）"));

    /* tooth 2: 清单面自检（版本键 + 负向探针）。 */
    TEST_ASSERT_TRUE_MESSAGE(p16_mfact("fas_manifest_version") == 1,
        "P16 RED(infra): 清单版本键缺失——FAS 清单机制失灵或版本漂移");
    TEST_ASSERT_TRUE_MESSAGE(p16_mfact("__p16_no_such_key__") == -1,
        "P16 RED(infra): 负向探针失败——未知键被误报为在（清单解析失灵）");
    TEST_ASSERT_TRUE_MESSAGE(p16_exists(P16_FAS_MANIFEST),
        "P16 RED(infra): 清单产物不可见——未 build 或路径漂移");
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *
 *   通道① host runner 全量（本文件未接线前）= supersede 后既有件红基线（见
 *     p1_home_battery / p1_cjk_name 各自文件尾 2026-10-29 追记）；本文件不在
 *     runner 内、不占 Tests 数。
 *   通道② C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4，命令见头注 Run
 *     protocol）：编译通道 error 为零 = 验收文件本身可编译。
 *   通道③ 接线落线后（builder step-0 现跑）执行通道期望：
 *     - Tests = 253（250 基线 + 本文件 3 CASE）；存量红 = p1_auxbar AB-A1/A2/A4
 *       三条（launch-card 基线锚在册）；**P0 链缺陷在册红（升级项, 非 P1 责任面）**
 *       = TEST 1 tooth 0b（P16 RED(P0-chain): cbin glyph dsc 布局旗不匹配——78 fork
 *       cbin writer 恒产 LARGE=1 16 字节 glyph dsc, **host 侧** LVGL 构建 LARGE=0
 *       （设备 sdkconfig.defaults:57 LARGE=y 在案 = host 单侧错配, architect
 *       advisory 2026-10-29; 设备真机消费验证归 FAR-E3）；修复归属 P0 fixer 轮）——
 *       该红未修则 TEST 1 tooth 1/2 与 PCN TEST 10 tooth 5 的 body16 消费齿连锁红
 *       （红名均指认 tooth 0b 同源）；P0 修复（host 开 LARGE）后转绿。
 *     - 本包义务红（设计态, builder 落 BUILDER DUTY 1)-4) 后转绿）= TEST 1
 *       tooth 2/3（[S1] 未切 / [S7] 未实现）+ TEST 2 tooth 3/4/5（下划线未实现）
 *       + TEST 3 结构红五键（[S7] 三落点 + text_yoff16 + marker_const）。
 *     - MUST-STAY-GREEN at baseline（现状即真）: TEST 1 tooth 0a/1/4 + TEST 2
 *       tooth 1/2/6 + TEST 3 接线键与负向键 + 清单自检。
 *     - 除上述义务红、P0 链在册红与 p1_auxbar×3 存量外零其它新增红；任何 infra 红
 *       （清单键缺/探针失灵/mount 失败）= 接线或底座缺陷，不冒充设计态红——停下
 *       回报。**零段错误**。
 * --------------------------------------------------------------------------- */
/* REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-29  initial freeze (acceptance-author, TaskPackage
 *               261028-m6-k2a-p1-home-16px; 授权链 = 本包 exitCriteria
 *               P1H-E1/E2/E3 + requirements v1.69 + brief v7 + FROZEN PROPOSAL
 *               F-1/F-2/F-9 + spine openQ q5).
 *               ── 冻结时点红基线实跑发现（升级项, 非 supersede; 2026-10-29
 *               architect advisory 文字更正）── host 全量现跑（250 Tests /
 *               4 Failures）暴露 **P0 资产链缺陷（host 单侧错配）**：78 fork
 *               cbin writer（tools/asset-pipeline/node_modules/lv_font_conv/lib/
 *               writers/cbin/lv_table_glyf.js toCBin）恒产 LV_FONT_FMT_TXT_LARGE=1
 *               16 字节 glyph dsc 布局（bitmap_index u32 + adv_w u32 + box_w/
 *               box_h u16 + ofs_x/ofs_y i16——与 LVGL 9.3 LARGE=1 布局逐字段一致），
 *               而 **host 侧** LVGL 构建未开该旗（host_app sdkconfig :665
 *               CONFIG_LV_FONT_FMT_TXT_LARGE not set）→ host 按 LARGE=0 8 字节
 *               布局解读 → 字形度量垃圾（scratch 探针实测：'8' box=71x0 / '中'
 *               box=0x1, line_height=23 正常）→ 16px/20px cbin 资产在 host 半
 *               不可正确消费（[S6] 核心 box_w==0 守卫拒收 → rc=-1）。**设备侧
 *               sdkconfig.defaults:57 CONFIG_LV_FONT_FMT_TXT_LARGE=y（P0 时点
 *               已如此, build/config/sdkconfig.h:1115 展开证实）——host 单侧错配
 *               已证实（architect advisory 2026-10-29），设备行为待 FAR-E3 真机
 *               验证**。P0 冻结件 FAS TEST 2/3/5 未拦（其断言只看 dsc 返回码与
 *               两坏读数互等，不钉度量合理性）。处置 = **归属 P0 fixer 轮，不由
 *               P1 代偿**（architect 裁定修法: 仅 host 开 LARGE——host_app/
 *               sdkconfig.defaults + 完整重编; font_puhui_14_1 指定初始化器自动
 *               重排无需重产; A11 独立参考解码器按字段名取值不受布局步长影响,
 *               tooth 1-4 原样绿）。本文件 tooth 0b gate 把该缺陷钉成自诊断红
 *               （P16 RED(P0-chain)），P0 修复后转绿；P0 链未修期间 tooth 1/2
 *               与 PCN TEST 10 tooth 5 的 body16 消费齿连锁红（红名同源指认）。
 *               本缺陷在真机轮前由本 host 齿先期暴露。
 * --------------------------------------------------------------------------- */
