/*
 * Frozen acceptance tests — TaskPackage 260901-ui-art-integration（包 U）
 * (subsystem: firmware+tools; 美术资产集成: 固件侧资产管线落成（SPIFFS assets 分区 +
 *  1bpp 位图/imgfont/9-slice/抖动网点加载）+ V3 美术资产本体落地 + 各 UI 面统一接入换皮;
 *  mergeOrder 12; rigor=full; riskTier=high; testHost: host+SDL;
 *  dependsOn: 260829-host-test-infra/voice-roundtrip/char-stroke/home-p1/schedule-p2/
 *  schedule-reminder/story-p3; 上游资产包 260831-ui-art-assets 已 merged).
 *
 * Story: 固件侧资产管线消费上游 47 枚已出图资产（tools/simulator/LvglWindowsSimulator/
 * assets/，清单 = changes/dev/260831-ui-art-assets/assets-manifest.md），经 SPIFFS
 * assets 分区按路径加载并渲染；各页面渲染面（包 2/4/5/6/7/8）统一接入换皮；换皮后
 * 各页面 host 回归独立于 9b 真机观感（ART-E3: 不 supersede 页面包既有验收）。
 *
 * Nature: host-runnable structural + BEHAVIORAL frozen acceptance, following the
 * home_p1.acceptance.c (HP1) weak-symbol frozen-offline-contract precedent: builder
 * MUST implement the pure-logic asset layer below (numeric codes + semantics frozen
 * HERE), so the red baseline is RED on real missing behaviour ("frozen contract not
 * implemented" weak sentinel / absent structural facts), never on a link or compile
 * error.
 *
 * ===========================================================================
 * V3 定稿口径（2026-09-09，docs/ui/kidpal-ui-design-v3.html 修订记录全文钉进判断）
 * ===========================================================================
 * - ① 9 宫格「24px 角片协议」接受（B2 决策债销）——本文件钉
 *   KIDPAL_ART_9SLICE_CORNER_PX == 24（三份 9slice 元数据 JSON 实测 corner 均 = 24）。
 * - ② 印章 2.5px 虚线现状接受（stamp_seal_r8 145×145，纯资产事实，无运行时行为）。
 * - ③ 吉祥物 mp-head 共享组件新增左侧头顶黑色剪影蝴蝶结（抱左耳根、向左下倾 18°、
 *   白色隔缝分离轮廓），7 姿态同位同形——凡断言姿态位图内容以重出图后的资产为准；
 *   蝴蝶结的**像素级造型观感归 design review / 9b 真机**，本文件只钉可机判近似
 *   （v3 母版溯源 + 裁定快照在世 + 7 姿态逐枚互异）。
 * - ④ B8 字盘打包策略锁定「笔形 3 态叠加复用 + 米字格单份背景」（禁逐笔逐态整帧，
 *   保守预估 ~3.1MB 顶预算）——承载齿 = 落地资产集 == 48 枚闭环白名单（无任何
 *   逐笔/逐态烘焙帧的容身位）+ P4 米字格大框单份背景 = frm_236_236（236×236，
 *   与 v3 稿 P4 .frm 236×236 同源）。
 * - ⑤ 字号下限留根：16px 以下 CJK 1bpp 真机可读性归包 9b（HWV-A7，ARQ-
 *   260831-small-cjk-1bpp-readability）实测后回写校准；本包 host+SDL 按现行字号落，
 *   不在本文件锁任何字号下限断言。
 *
 * ===========================================================================
 * 契约零变更口径
 * ===========================================================================
 * 本包不动 display 统一响应块 E1 契约措辞；本文件对 docs/contracts 目录只读 canary
 * （在世守卫）。验收不得要求改契约面。
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, C linkage, host-compilable,
 * no chip/FreeRTOS/ESP-component deps; recommended home:
 * firmware/main/display/art_assets.(c|h))
 * ===========================================================================
 * 资产层 = 页面渲染面的共享取资/绘制底座：loader（SPIFFS 按路径加载，host 上为
 * 同一 stdio 通路读真实目录）+ 绘制原语（LVGL，与页面同一条渲染路径）。设备侧
 * SPIFFS VFS 注册（esp_spiffs）放芯片装配层、不进本模块（host 可编纪律，纯净性
 * 齿扫 esp_spiffs 词）；mount() 只收 root 路径字符串。
 *
 *   --- SPIFFS 资产文件格式（冻结，little-endian；转换脚本由 builder 供） ---
 *     offset 0  : magic  u32 = 0x3150414B（字节序 'K''A''P''1'）
 *     offset 4  : format u16 = 1 (KIDPAL_ART_FMT_1BPP) | 2 (KIDPAL_ART_FMT_RGB565)
 *     offset 6  : flags  u16 = 0（预留）
 *     offset 8  : w      u16（像素）
 *     offset 10 : h      u16（像素）
 *     offset 12 : stride u16（每行字节数）
 *     offset 14 : reserved u16 = 0
 *     offset 16 : 像素数据，stride * h 字节
 *   1BPP 语义：每行 MSB-first 紧排，bit=1 为墨（黑），与 char_stroke 光栅同约定。
 *   RGB565 语义：LE，0x0000 = 墨（黑）、0xFFFF = 纸（白）——上游阈值化产物
 *   （LUM_THRESHOLD=127）语义；任何 1..0xFFFE 中间值 = 非阈值化产物 = 语义错误。
 *
 *   --- 落地面（冻结路径与命名） ---
 *   SPIFFS 落地根 = firmware/main/assets/spiffs_root/（设备 = 该目录树烧进
 *   partitions/v2/16m.csv 的 assets 分区 [data/spiffs, 0x800000, 8M]；host = loader
 *   的 root 直读此目录）。平铺 48 枚 .bin（原 47 枚 + bubble_tail_24, REVISION LOG
 *   2026-09-12），文件名 = 上游管线资产名 + ".bin"
 *   （pose_daily_96.bin / ic_wave_48.bin / d50.bin / frm_236_236.bin /
 *   imgfont_digit_3.bin / ...，闭集白名单见 artt_expected[] 表）。
 *
 *   --- 类型形状（冻结） ---
 *   typedef struct {
 *       int w;              // 像素宽, > 0
 *       int h;              // 像素高, > 0
 *       int stride_bytes;   // >= 每行最小字节数（1BPP: (w+7)/8; RGB565: w*2）
 *       int format;         // KIDPAL_ART_FMT_1BPP | KIDPAL_ART_FMT_RGB565
 *       const uint8_t *data;// 像素数据（loader 持有；有效期至下一次 mount/load）
 *   } kidpal_art_img_t;
 *
 *   --- 常量（冻结数值码） ---
 *   KIDPAL_ART_UNIMPLEMENTED (-2)      // weak 哨兵 — 永不是合法返回值
 *   KIDPAL_ART_FMT_1BPP   1
 *   KIDPAL_ART_FMT_RGB565 2
 *   KIDPAL_ART_MAGIC      0x3150414Bu  // "KAP1"
 *   KIDPAL_ART_9SLICE_CORNER_PX 24    // v3 定稿 24px 角片协议（三框通用）
 *
 *   --- 函数契约（全部返回 int，除注明外；-2 = 未实现哨兵，-1 = 防御/无效） ---
 *   int kidpal_art_mount(const char *root);
 *       设定资产根（设备 = SPIFFS 挂载点路径；host = 真实目录）。root NULL/空/
 *       不可访问（opendir 失败）-> -1；成功 -> 0。可重入（重 mount 切换 root，
 *       后续 load/glyph/draw 全部解析到当前 root）。
 *
 *   int kidpal_art_load(const char *path, kidpal_art_img_t *out);
 *       从当前 root 按相对路径加载一枚资产并解析 KAP1 头。path/out NULL -> -1；
 *       文件不存在/不可读 -> -1；magic 不符 / format 非法 / w==0 / h==0 /
 *       stride 小于该 format 每行最小值 / 文件被截断（< 16 + stride*h）-> -1。
 *       成功 -> 0 且 *out 填齐（data 非空；有效期至下一次 mount/load，单槽缓存
 *       即可——测试在两次 load 之间消费）。
 *
 *   int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y);
 *       1bpp 语义像素访问（两种存储 format 归一）：返回 1 = 墨（黑）、0 = 纸（白）。
 *       img NULL / x,y 越界 / format 非法 -> -1；RGB565 像素值非 {0x0000,0xFFFF}
 *       -> -1（非阈值化产物语义错误）。
 *
 *   int kidpal_art_glyph(char ch, kidpal_art_img_t *out);
 *       Baloo 2 动态时间 imgfont 字形查取（spec §5.7 决策 17：图标走离线位图、
 *       imgfont 只留给动态时间数字）。'0'..'9' -> "imgfont_digit_<ch>.bin"，
 *       ':' -> "imgfont_colon.bin"，经当前 root 加载（同 load 语义与防御）。
 *       其余字符（含字母/符号/负值 char）-> -1。
 *
 *   int kidpal_art_glyph_advance(void);
 *       时间数字排版步进（px）。冻结：> 0 且 >= 数字字形宽（现资产 48）且
 *       <= 2*数字字形宽（防失控字距）。当前 root 无 imgfont 族 -> -1。
 *
 *   int kidpal_art_render_asset(const char *path, int dx, int dy);
 *       把一枚资产 1:1（无缩放）画到当前屏幕 (dx,dy) 左上角并刷新渲染缝整帧
 *       （LVGL lv_image 同条页面渲染路径；host 上像素经 host_infra_read_pixel
 *       可探针）。首次调用前确保渲染缝初始化（可先调 host_infra_render_frame()，
 *       或实现内自含等价初始化——实现面自由）。path NULL / 资产不存在 -> -1。
 *
 *   int kidpal_art_draw_nine_slice(const char *name, int dx, int dy, int w, int h);
 *       按 9 宫格协议把 name 框（如 "frm_236_236"）拉伸绘制到 (dx,dy,w,h)：
 *       4 角片 KIDPAL_ART_9SLICE_CORNER_PX 见方原样（stretch=false，角部不变形）、
 *       4 边片单轴拉伸、中心片双轴拉伸；绘制后刷新渲染缝整帧。w 或 h <
 *       2*corner、name NULL/未知、资产缺失 -> -1。
 *
 *   int kidpal_art_draw_dots(const char *name, int dx, int dy, int w, int h);
 *       把 name 网点贴图（d50/d25/d12，4×4）自 (dx,dy) 起按 4px 步进平铺覆盖
 *       w×h 区域（边缘不足 4px 的整贴裁剪），绘制后刷新渲染缝整帧。name
 *       NULL/未知、w<=0、h<=0 -> -1。
 *
 *   int kidpal_art_draw_time(const char *hhmm, int dx, int dy);
 *       动态时间 imgfont 组合渲染：hhmm 逐字符经 kidpal_art_glyph 取字形，第 i
 *       字符 1:1 画于 x = dx + i*kidpal_art_glyph_advance()、y = dy；绘制后刷新
 *       渲染缝整帧。hhmm NULL/空/含 0-9 与 ':' 之外字符 -> -1。
 *
 * While the implementation is absent, __attribute__((weak)) defaults below
 * return the sentinel KIDPAL_ART_UNIMPLEMENTED (-2), so every behavioral test
 * asserts NOT-sentinel first: the red baseline reads "frozen contract not
 * implemented" — a real missing-behaviour red, not a link error.
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test):
 *   coverage: ART-A1  structural -> （TaskPackage kind=structural，不落 C 断言，
 *               仅登记；结构性承载并入 ART-A2 装配齿）
 *   coverage: ART-A2  structural -> test 1 (SPIFFS assets 分区 CSV canary + 设备构建
 *               spiffs 落地接线 + art_assets 模块在树并编入设备 CMakeLists +
 *               host_app HOST_ACCEPTANCE_SRCS 接线与 smoke 末位守卫 + 契约 canary)
 *   coverage: ART-A2  behavioral -> test 2 (mount/load 路径语义 + KAP1 双 format
 *               fixture 解码 + 1bpp 像素语义 + 坏 magic/截断/中间灰/防御)
 *   coverage: ART-A2  render    -> test 6 (真姿态资产 1:1 渲染逐像素比对渲染缝)
 *   coverage: ART-A3  behavioral -> test 5 (imgfont 11 枚等宽箱体 + 逐字符互异 + 防御)
 *   coverage: ART-A3  render    -> test 9 (动态时间逐字符单元格比对)
 *   coverage: ART-A4  render    -> test 7 (9-slice 24px 角片逐像素不变形 + 边框连续)
 *                                  + test 8 (网点 4px 平铺周期 + d50>d25>d12 密度)
 *   coverage: ART-A5  landing   -> test 3 (48 枚闭集落地 + 真实落盘尺寸逐枚对账)
 *                                  + test 4 (v3 母版溯源 + 蝴蝶结裁定快照在世 +
 *                                  7 姿态逐枚互异 + B8 打包红线)
 *   coverage: ART-A6  wiring    -> test 10 (包 2/4/5/6/7/8 六页渲染面统一消费共享
 *               资产层：art_assets.h include 或 kidpal_art_* 调用)
 *   coverage: ART-A7  regression-> test 11 (六包冻结契约抽查——换皮后布局/逻辑不破;
 *               页面包原验收套件在同 runner 全量复跑为附加通道, ART-E3 不 supersede)
 *   coverage: ART-E2  留根（不落断言）：SPIFFS 真机文件源路径 / 1bpp 真机观感 /
 *               16px 以下 CJK 真机可读性（HWV-A7）/ 前光确认 —— 显式殿后 9b。
 *   coverage: ART-E3  留根（不落断言）：本文件不 supersede 页面包既有验收；页面
 *               原验收（home_p1/schedule_p2/char_stroke/schedule_reminder_b/story_p3/
 *               voice_roundtrip_b）在同一 host runner 全量复跑即其机器承载。
 *   coverage: ART-E4  前置输入已满足（v3 定稿 + 母版指针已切 v3 + 重出图 47 枚机判
 *               30/30 绿）—— test 4 溯源齿为其在世守卫。
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，[C4.2] 约定）：
 *   1) firmware/main/display/art_assets.c + art_assets.h 实现上述 frozen contract
 *      （C linkage、零芯片依赖、数值码/语义以本文件为准；kidpal_art_img_t 形状须
 *      与本文件完全一致；设备侧 esp_spiffs VFS 注册放芯片装配层、不进本模块）。
 *   2) SPIFFS 落地面：firmware/main/assets/spiffs_root/ 平铺 48 枚 .bin（KAP1 冻结
 *      格式），由上游管线产物（tools/simulator/LvglWindowsSimulator/assets/ 的
 *      assets.c RGB565 数据 + 9slice 元数据）转换生成（转换脚本落点 builder 定，
 *      建议随 tools/asset-pipeline/ 扩展，保持单一母版纪律）。
 *   3) 设备构建装配：firmware/main/CMakeLists.txt 加 display/art_assets.c 编入 +
 *      assets 分区 SPIFFS 镜像接线（引用 spiffs_root 落地目录）。注意既有
 *      CONFIG_FLASH_DEFAULT_ASSETS / esptool_py_flash_to_partition 机制同用 assets
 *      分区——与 U SPIFFS 镜像的共存/接管是集成决策，冲突时回 planner 裁定，
 *      不得静默双写。
 *   4) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../ui_art_integration.acceptance.c" 与
 *      "../../../display/art_assets.c"（强符号覆盖 weak 哨兵的关键，HP1 同款），
 *      **必须置于 host_infra_smoke.acceptance.c 之前（smoke 保持列表末位 = 最先
 *      执行，包 0 boundary 齿注册序守卫；结构齿 5 校验此序）**。
 *   5) 渲染入缝：kidpal_art_render_asset / draw_* 首调前确保渲染缝初始化；如需
 *      扩展 host_app/main/host_infra_seam.c 做 additive 扩展，不得破坏包 0 冻结
 *      断言（host_infra_smoke.acceptance.c 必须保持绿）。
 *   6) 换皮接线（ART-A6）：display/ 下 view_router.c（资产层 mount 初始化接线点）、
 *      char_stroke.c、home_p1.c、schedule_page.c、reminder_page.c、story_page.c 六个
 *      渲染面模块统一经共享资产层取资产（include "art_assets.h" 或调用
 *      kidpal_art_*）；保持 additive，不重排他人文件结构；页面原验收渲染齿的
 *      布局钉区域在位语义必须保持（ART-E3）。
 *   7) 模块纯净性（E2/E3 红线）：art_assets.c/h 不得出现 mcp/websocket/wss 下发
 *      链路词与 esp_spiffs/driver/i2c/driver/spi 芯片直读词（guard 齿会扫）。
 *   8) B8 红线：落地资产集 == 48 枚闭集白名单（原 47 + bubble_tail_24, 2026-09-12
 *      闭集修订）；不得新增逐笔/逐态烘焙帧（结构齿 4
 *      按闭集枚举机判）；P4 米字格大框单份背景 = frm_236_236 经 9-slice 绘制。
 *
 * RED baseline（2026-09-09 acceptance-author 实测，见
 * firmware/main/test/acceptance/red-baseline/red-baseline-summary.md U 包段）：
 *   - firmware/main 无 art_assets 模块（frozen contract 未实现，weak 哨兵 -2）；
 *     SPIFFS 落地面 firmware/main/assets/spiffs_root/ 不存在；设备/host 构建均未
 *     接线本验收；六页渲染面未接资产层。
 *   - 页面包冻结契约（包 2/4/5/6/7/8）为 MUST-STAY-GREEN 守卫（stub harness 下
 *     未链入按 skip 记录，官方 runner 上为硬断言）。
 *   -> 行为/结构齿 RED（各自真实缺失行为）；守卫齿 GREEN。
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   cwd 语义：仓根定位 = 自 runner cwd 逐级上探 firmware/main/CMakeLists.txt（从
 *   仓根或 host_app 目录取数均可）；本文件所有仓内相对路径（firmware/、tools/、
 *   docs/）均经上探根拼接，不依赖裸 cwd。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-09  initial freeze (acceptance-author).
 *   2026-09-12  U 闭集修订（TaskPackage 260911-render-pages RP-A4/F1, S0.1 决策件
 *               font-decision-s01.md「气泡尾巴」行 + 跨包留痕）: 白名单 47→48,
 *               唯一新增 KAP1 = bubble_tail_24.bin（24×24, V3 §06 气泡「手绘圆角框
 *               ＋左下斜切小尾巴」; 消费接线由 v3_restore.acceptance.c RP-A9 齿
 *               承载）。计数叙述同步 47→48（test 3/4 消息串、BUILDER DUTY 2/8、
 *               B8 红线注记）; 断言机制零改动（ARTT_EXPECTED_N 自动随表）。字体
 *               资产 NOT 并入本闭集（lv_font_conv 产物非 KAP1, 独立 fonts_root
 *               落地, 分型齿在 v3_restore.acceptance.c）; 压暗用既有 d50 网点
 *               资产, 零新增。
 *   2026-09-12  coverage: RP-A6 冻结面零越界在本件留痕声明承载：本件仅
 *               acceptance-author 留痕修订（U 闭集 47→48 条目 + 计数叙述同步 +
 *               本条）, 断言机制零改动, docs/contracts/** 与
 *               managed_components/** 零触碰。
 * --------------------------------------------------------------------------- */

/* mkdtemp (POSIX 2008) — linux-target host runner 与红基线 stub 编译均为 glibc。 */
#define _DEFAULT_SOURCE 1

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes + constants (== firmware/main/display/art_assets.h, 不得偏移)
 * ------------------------------------------------------------------------ */
#define KIDPAL_ART_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */
#define KIDPAL_ART_FMT_1BPP   1
#define KIDPAL_ART_FMT_RGB565 2
#define KIDPAL_ART_MAGIC      0x3150414Bu /* "KAP1" little-endian */
#define KIDPAL_ART_9SLICE_CORNER_PX 24    /* v3 定稿 24px 角片协议 */

/* 落地根（BUILDER DUTY 2 冻结路径）。 */
#define ARTT_LANDING_REL "firmware/main/assets/spiffs_root"

/* 帧与探针（spec §5.5 设备屏 400×300；渲染缝同尺寸）。 */
#define ARTT_FRAME_W 400
#define ARTT_FRAME_H 300
#define ARTT_PAPER   0xFFFFFFul /* 1bpp 语义纸色（阈值化白）经缝掩码后的值 */
#define ARTT_INK     0x000000ul /* 1bpp 语义墨色 */
#define ARTT_NOT_RENDERED ((int32_t)-1)

typedef struct {
    int w;
    int h;
    int stride_bytes;
    int format;
    const uint8_t *data;
} kidpal_art_img_t;

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak defaults (sentinel -2).
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_art_mount(const char *root)
{
    (void)root;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_load(const char *path, kidpal_art_img_t *out)
{
    (void)path; (void)out;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y)
{
    (void)img; (void)x; (void)y;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_glyph(char ch, kidpal_art_img_t *out)
{
    (void)ch; (void)out;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_glyph_advance(void)
{
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_render_asset(const char *path, int dx, int dy)
{
    (void)path; (void)dx; (void)dy;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_draw_nine_slice(const char *name, int dx, int dy,
                                                     int w, int h)
{
    (void)name; (void)dx; (void)dy; (void)w; (void)h;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_draw_dots(const char *name, int dx, int dy,
                                               int w, int h)
{
    (void)name; (void)dx; (void)dy; (void)w; (void)h;
    return KIDPAL_ART_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_art_draw_time(const char *hhmm, int dx, int dy)
{
    (void)hhmm; (void)dx; (void)dy;
    return KIDPAL_ART_UNIMPLEMENTED;
}

/* host 渲染缝（包 0 落地，host_app/main/host_infra_seam.c 提供；红基线 stub
 * 脚手架由 red-baseline/seam_stub.c 供链）。只探针，不在此定义。 */
int     host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * 六包页面冻结契约（ART-A7 回归抽查的 extern 面）。weak 声明：官方 runner 里
 * 由各页面包实现源（强符号）或其验收文件的 weak 哨兵（-2）供链——前者真断言、
 * 后者硬红「page contract not implemented」；红基线 stub harness 未链入任何
 * 页面包 -> 弱符号解析为 NULL -> 按 skip 记录（stub 编译带 -DKIDPAL_ART_RB_STUB；
 * 官方构建不得定义该宏，否则回归齿被静默跳过 = 违反本冻结件）。
 * ------------------------------------------------------------------------ */
extern __attribute__((weak)) int kidpal_display_route(const char *view);          /* 包 2 */
extern __attribute__((weak)) int kidpal_schedule_dot_state(const char *start_hhmm,
                                                           const char *now_hhmm,
                                                           int is_tomorrow);      /* 包 6 */
extern __attribute__((weak)) int kidpal_char_transform(float x, float y,
                                                       int *sx, int *sy);         /* 包 4 */
/* hp1_item_t 布局冻结于 home_p1.acceptance.c（char time[6]; char name[32]）——
 * 此处按冻结形状镜像，错位会在断言值上显形。 */
typedef struct { char time[6]; char name[32]; } artt_hp1_item_t;
extern __attribute__((weak)) int kidpal_home_p1_countdown(const artt_hp1_item_t *items,
                                                          int count, int now_h,
                                                          int now_m, int *out_index,
                                                          int *out_hours,
                                                          int *out_minutes);      /* 包 5 */
extern __attribute__((weak)) long long kidpal_fire_at_minutes(const char *fire_at_iso); /* 包 7 */
/* kidpal_story_stamp_spec_t 布局冻结于 story_p3.acceptance.c。 */
typedef struct { int rings; int inner_dashed; int angle_deci; int hold_ms; } artt_stamp_spec_t;
extern __attribute__((weak)) int kidpal_story_stamp_spec(artt_stamp_spec_t *out); /* 包 8 */

#ifdef KIDPAL_ART_RB_STUB
#define ARTT_RB_REQUIRE_LINKED(fn, what)                                     \
    do {                                                                     \
        if ((fn) == NULL) {                                                  \
            printf("    [art-rb] %s not linked in stub harness — "           \
                   "regression tooth skipped (official runner: hard assert)\n", what); \
            return;                                                          \
        }                                                                    \
    } while (0)
#else
#define ARTT_RB_REQUIRE_LINKED(fn, what)                                     \
    do {                                                                     \
        if ((fn) == NULL) {                                                  \
            TEST_ASSERT_TRUE_FM(0,                                      \
                "ART-A7 RED(infra): " what " symbol not linked into the host " \
                "runner — 换皮回归面断裂（页面包实现/验收未编入 HOST_ACCEPTANCE_SRCS）"); \
        }                                                                    \
    } while (0)
#endif

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (structural teeth), HP1/KSP precedent. Excluded
 * dirs: test(本文件所在)/host_app/build/.git 均非实现面（landing 落地目录不经
 * 扫描器枚举，用直连 opendir 单独核对）。
 * ------------------------------------------------------------------------ */
#define ARTT_MAX_PATH 1024
#define ARTT_MAX_READ (8u * 1024u * 1024u)

static char artt_root[ARTT_MAX_PATH];
static int artt_root_ready;

static int artt_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int artt_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int artt_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int artt_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[ARTT_MAX_PATH];

    if (artt_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (artt_is_regular(probe)) {
            snprintf(artt_root, sizeof(artt_root), "%s", cands[i]);
            artt_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int artt_rel(char *out, size_t cap, const char *rel)
{
    if (!artt_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", artt_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", artt_root, rel);
    return 1;
}

static int artt_exists(const char *rel)
{
    char p[ARTT_MAX_PATH];
    return artt_rel(p, sizeof(p), rel) && artt_is_regular(p);
}

static char *artt_read(const char *rel, size_t *out_len)
{
    char p[ARTT_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!artt_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > ARTT_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int artt_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    n = strlen(needle);
    if (n == 0) {
        return 1;
    }
    for (const char *h = hay; *h != '\0'; h++) {
        size_t i = 0;
        while (i < n && h[i] != '\0' &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static int artt_file_contains_ci(const char *rel, const char *needle)
{
    char *buf = artt_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = artt_contains_ci(buf, needle);
    free(buf);
    return r;
}

static int artt_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                        const char *needle, char *first_hit,
                                        size_t cap, int depth)
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
        char abs_next[ARTT_MAX_PATH];
        char rel_next[ARTT_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            artt_is_skipped_dir(e->d_name)) {
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
            count += artt_count_files_with_needle(abs_next, rel_next, needle,
                                                  first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > ARTT_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL); /* logic guard */
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                if (artt_contains_ci(buf, needle)) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int artt_scan_count(const char *rel_dir, const char *needle,
                           char *first_hit, size_t cap)
{
    char abs[ARTT_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!artt_rel(abs, sizeof(abs), rel_dir) || !artt_is_dir(abs)) {
        return 0;
    }
    return artt_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

#define ARTT_REQUIRE_ROOT() TEST_ASSERT_TRUE_FM(artt_locate_root(), \
    "ART RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf 风格断言包装：unity 的 *_MESSAGE 宏只收单串消息，本文件大量失败诊断
 * 带动态字段（资产名/坐标/计数）——先格式化进静态缓冲再走原宏（原宏在条件为
 * 真时不求值消息参数，静态缓冲单次使用安全）。 */
#include <stdarg.h>
static char artt_fm_buf[1024];
static const char *artt_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *artt_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(artt_fm_buf, sizeof(artt_fm_buf), fmt, ap);
    va_end(ap);
    return artt_fm_buf;
}
#define TEST_ASSERT_TRUE_FM(cond, ...)  TEST_ASSERT_TRUE_MESSAGE((cond), artt_fm(__VA_ARGS__))
#define TEST_ASSERT_FALSE_FM(cond, ...) TEST_ASSERT_FALSE_MESSAGE((cond), artt_fm(__VA_ARGS__))
#define TEST_ASSERT_NOT_NULL_FM(p, ...) TEST_ASSERT_NOT_NULL_MESSAGE((p), artt_fm(__VA_ARGS__))
#define TEST_ASSERT_NULL_FM(p, ...)     TEST_ASSERT_NULL_MESSAGE((p), artt_fm(__VA_ARGS__))

static void artt_require_impl(int value, const char *what)
{
    if (value == KIDPAL_ART_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "ART RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the asset layer per the frozen contract in "
                 "firmware/main/test/ui_art_integration.acceptance.c header and "
                 "wire display/art_assets.c into HOST_ACCEPTANCE_SRCS + "
                 "firmware/main/CMakeLists.txt)",
                 what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg); /* msg 已 snprintf 成单串, 不走 artt_fm */
    }
}

/* 落地根绝对路径（仓根拼接）；返回 0 = 仓根未定位。 */
static int artt_landing_path(char *out, size_t cap)
{
    return artt_rel(out, cap, ARTT_LANDING_REL);
}

/* 像素快照：经被测 kidpal_art_pixel 把整图抻平成 0/1 数组（测试内自用）。
 * 返回 0 成功；pixel 语义错误（-1）-> -1。 */
static int artt_snap(const kidpal_art_img_t *img, uint8_t *out)
{
    assert(img != NULL && img->w > 0 && img->h > 0);
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            int p = kidpal_art_pixel(img, x, y);
            if (p != 0 && p != 1) {
                return -1;
            }
            out[(size_t)y * (size_t)img->w + (size_t)x] = (uint8_t)p;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * V3 资产闭集白名单（48 枚 = 原 47 枚 + bubble_tail_24（REVISION LOG 2026-09-12,
 * 260911-render-pages U 闭集修订: V3 气泡尾巴, 唯一新增 KAP1; S0.1 决策件
 * font-decision-s01.md「气泡尾巴 | KAP1 闭集修订（唯一新增 KAP1）」行承载）,
 * 真实落盘尺寸 = 2026-09-09 自 tools/simulator/LvglWindowsSimulator/assets/
 * assets.c 描述符逐枚实测采样；命名 = 上游管线资产名 + ".bin"）。此表即 ART-A5
 * 落地对账基准 + B8 打包红线的闭集（表外任何落地文件 = 违规）。
 * ------------------------------------------------------------------------ */
typedef struct {
    const char *name;
    int w;
    int h;
} artt_expect_t;

static const artt_expect_t artt_expected[] = {
    /* 吉祥物 7 姿态（mp-head 共享组件含 v3 蝴蝶结；96×96 同族同尺寸） */
    { "pose_daily_96.bin",  96, 96 },
    { "pose_listen_96.bin", 96, 96 },
    { "pose_think_96.bin",  96, 96 },
    { "pose_talk_96.bin",   96, 96 },
    { "pose_record_96.bin", 96, 96 },
    { "pose_sleep_96.bin",  96, 96 },
    { "pose_jump_96.bin",   96, 96 },
    /* 图标族 14（48 盒统一，ic_wave 48×14 波浪线特例） */
    { "ic_apple_48.bin",   48, 48 },
    { "ic_book_48.bin",    48, 48 },
    { "ic_mic_48.bin",     48, 48 },
    { "ic_drop_48.bin",    48, 48 },
    { "ic_thermo_48.bin",  48, 48 },
    { "ic_bell_48.bin",    48, 48 },
    { "ic_batt_48.bin",    48, 48 },
    { "ic_mag_48.bin",     48, 48 },
    { "ic_star_48.bin",    48, 48 },
    { "ic_cloud_48.bin",   48, 48 },
    { "ic_penhand_48.bin", 48, 48 },
    { "ic_note_48.bin",    48, 48 },
    { "ic_moon_48.bin",    48, 48 },
    { "ic_wave_48.bin",    48, 14 },
    /* 手绘框族 3（9 宫格 24px 角片协议） */
    { "frm_236_236.bin",     236, 236 },
    { "frm_sm_160_120.bin",  160, 120 },
    { "frm_flat_240_96.bin", 240, 96 },
    /* V3 气泡尾巴（REVISION LOG 2026-09-12 260911-render-pages 闭集修订: 唯一新增
     * KAP1; frm-sm 手绘圆角框 + 左下斜切小尾巴, V3 §06 气泡规格; 消费接线由
     * v3_restore.acceptance.c RP-A9 齿承载） */
    { "bubble_tail_24.bin",  24, 24 },
    /* 抖动网点 3（4px 格防糊红线） */
    { "d50.bin",  4, 4 },
    { "d25.bin",  4, 4 },
    { "d12.bin",  4, 4 },
    /* 印章（-8° 烘入 145×145）+ 站酷快乐体样字 */
    { "stamp_seal_r8.bin", 145, 145 },
    { "font_zcool_30.bin", 300, 52 },
    /* Baloo 2 动态时间 imgfont 11 枚（等宽箱体 48×64） */
    { "imgfont_digit_0.bin", 48, 64 },
    { "imgfont_digit_1.bin", 48, 64 },
    { "imgfont_digit_2.bin", 48, 64 },
    { "imgfont_digit_3.bin", 48, 64 },
    { "imgfont_digit_4.bin", 48, 64 },
    { "imgfont_digit_5.bin", 48, 64 },
    { "imgfont_digit_6.bin", 48, 64 },
    { "imgfont_digit_7.bin", 48, 64 },
    { "imgfont_digit_8.bin", 48, 64 },
    { "imgfont_digit_9.bin", 48, 64 },
    { "imgfont_colon.bin",   48, 64 },
    /* P1 既有 7（上游 v3 重出图随全量落盘） */
    { "time_10_24.bin",     246, 112 },
    { "thermo_man.bin",      41,  67 },
    { "drop_baby.bin",       32,  51 },
    { "ic_batt_32.bin",      32,  26 },
    { "ic_bell_13.bin",      13,  13 },
    { "doodle_star_26.bin",  26,  26 },
    { "doodle_cloud_52.bin", 52,  52 },
};

#define ARTT_EXPECTED_N (sizeof(artt_expected) / sizeof(artt_expected[0]))

/* -------------------------------------------------------------------------- *
 * Test 1 — ART-A2 structural: SPIFFS 分区面 + 模块装配 + host 接线序守卫。
 *   1) 验收文件在世于 taskpackage 钉的路径（在世守卫）;
 *   2) 头注 BUILDER DUTY 标记在位（在世守卫）;
 *   3) SPIFFS 分区 canary：partitions/v2/16m.csv 的 assets 行（data/spiffs,
 *      0x800000, 8M）在世;
 *   4) 设备树存在实现 frozen contract 的 art_assets 模块;
 *   5) 模块编入设备构建（firmware/main/CMakeLists.txt 引用 art_assets.c）+
 *      assets 分区 SPIFFS 落地接线（引用 spiffs_root）;
 *   6) host 接线：HOST_ACCEPTANCE_SRCS 含本验收文件，且排在
 *      host_infra_smoke.acceptance.c 之前（smoke 末位 = boundary 齿最先执行）;
 *   7) 契约 canary：interface-contract.md 在世（零契约触碰红线）。
 * RED today: teeth 4) 5) 6) 无实现/未接线。teeth 1) 2) 3) 7) 是在世守卫。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 验收文件在世 + art_assets 模块装配 + SPIFFS 分区面 + 接线序守卫 (ART-A2 structural)",
          "[art_u][ART-A2][structural]")
{
    char hit[ARTT_MAX_PATH];
    int n_files;

    ARTT_REQUIRE_ROOT();

    /* tooth 1: 验收文件在世于 taskpackage.acceptanceTestsFile 钉的路径 */
    TEST_ASSERT_TRUE_FM(
        artt_exists("firmware/main/test/ui_art_integration.acceptance.c"),
        "ART-A2 RED(infra): the frozen acceptance file is missing from its "
        "taskpackage-pinned path firmware/main/test/ui_art_integration.acceptance.c");

    /* tooth 2: 头注 BUILDER DUTY 清单在位（[C4.2] 接线项已列） */
    {
        char *self = artt_read("firmware/main/test/ui_art_integration.acceptance.c", NULL);
        TEST_ASSERT_NOT_NULL_FM(self, "ART-A2 RED(infra): acceptance file unreadable");
        TEST_ASSERT_TRUE_FM(
            artt_contains_ci(self, "HOST_ACCEPTANCE_SRCS") &&
            artt_contains_ci(self, "display/art_assets.c") &&
            artt_contains_ci(self, "spiffs_root"),
            "ART-A2 RED(infra): acceptance file header lost the builder-duty wiring "
            "markers (HOST_ACCEPTANCE_SRCS + display/art_assets.c + spiffs_root) — "
            "接线职责清单缺失");
        free(self);
    }

    /* tooth 3: SPIFFS 分区 canary（UAA-A9 钉死的对照值 0x800000 / 8M） */
    {
        char *csv = artt_read("firmware/partitions/v2/16m.csv", NULL);
        TEST_ASSERT_NOT_NULL_FM(csv,
            "ART-A2 RED(infra): firmware/partitions/v2/16m.csv unreadable — 分区表"
            "事实源漂移（UAA-A9 钉死 assets 行在世）");
        TEST_ASSERT_TRUE_FM(
            artt_contains_ci(csv, "assets,") && artt_contains_ci(csv, "spiffs") &&
            artt_contains_ci(csv, "0x800000") && artt_contains_ci(csv, "8M"),
            "ART-A2 RED(infra): assets partition row (data/spiffs, 0x800000, 8M) "
            "lost from partitions/v2/16m.csv — SPIFFS assets 分区锚漂移");
        free(csv);
    }

    /* tooth 4: 设备树存在 art_assets 模块（frozen contract 实现面） */
    n_files = artt_scan_count("firmware/main", "kidpal_art_load(", hit, sizeof(hit));
    TEST_ASSERT_TRUE_FM(n_files >= 1,
        "ART-A2 RED: no module under firmware/main implements the frozen asset "
        "contract (kidpal_art_load absent) — SPIFFS assets 分区 + 1bpp 加载管线"
        "未落成 (ART-A2)");

    /* tooth 5: 模块编入设备构建 + SPIFFS 落地接线（装配，非浮文件） */
    TEST_ASSERT_TRUE_FM(
        artt_file_contains_ci("firmware/main/CMakeLists.txt", "art_assets.c"),
        "ART-A2 RED: art_assets.c is not referenced by firmware/main/CMakeLists.txt "
        "— 资产层模块未编入设备构建（装配缺失; BUILDER DUTY 3）");
    TEST_ASSERT_TRUE_FM(
        artt_file_contains_ci("firmware/main/CMakeLists.txt", "spiffs_root"),
        "ART-A2 RED: firmware/main/CMakeLists.txt does not wire the spiffs_root "
        "landing dir into the assets partition — SPIFFS 分区镜像接线缺失 "
        "(BUILDER DUTY 3; 与既有 CONFIG_FLASH_DEFAULT_ASSETS 共存冲突须回 planner)");

    /* tooth 6: host 接线在位 + smoke 末位守卫（boundary 齿注册序） */
    {
        char *mk = artt_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
        TEST_ASSERT_NOT_NULL_FM(mk,
            "ART-A2 RED(infra): host_app/main/CMakeLists.txt unreadable");
        const char *mine = strstr(mk, "ui_art_integration.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_FM(mine != NULL,
            "ART-A2 RED: ui_art_integration.acceptance.c is not wired into "
            "HOST_ACCEPTANCE_SRCS — host 验收构建未接线 (BUILDER DUTY 4)");
        TEST_ASSERT_TRUE_FM(smoke != NULL,
            "ART-A2 RED(infra): host_infra_smoke.acceptance.c lost from "
            "HOST_ACCEPTANCE_SRCS — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_FM(mine < smoke,
            "ART-A2 RED: ui_art_integration.acceptance.c must be listed BEFORE "
            "host_infra_smoke.acceptance.c (smoke 末位 = 最先执行, boundary 齿"
            "注册序守卫; BUILDER DUTY 4)");
        free(mk);
    }

    /* tooth 7: 契约 canary（零契约触碰的在世守卫） */
    TEST_ASSERT_TRUE_FM(
        artt_exists("docs/contracts/interface-contract.md"),
        "ART-A2 RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* 模块纯净性（未落地时平凡绿; 落地后为强守卫; BUILDER DUTY 7） */
    {
        const char *const module_files[] = {
            "firmware/main/display/art_assets.c", "firmware/main/display/art_assets.h"
        };
        const char *const bad_needles[] = {
            "mcp", "websocket", "wss:", "esp_spiffs", "driver/i2c", "driver/spi"
        };
        for (size_t i = 0; i < 2; i++) {
            if (!artt_exists(module_files[i])) {
                continue;
            }
            for (size_t k = 0; k < sizeof(bad_needles) / sizeof(bad_needles[0]); k++) {
                char msg[512];
                snprintf(msg, sizeof(msg),
                         "ART-E2/E3 RED(scope): %s mentions \"%s\" — 资产层模块纯净"
                         "性违规（下发链路/芯片直读不进 loader；设备侧 esp_spiffs VFS"
                         "注册放芯片装配层）",
                         module_files[i], bad_needles[k]);
                TEST_ASSERT_FALSE_MESSAGE(
                    artt_file_contains_ci(module_files[i], bad_needles[k]), msg);
            }
        }
    }
}

/* -------------------------------------------------------------------------- *
 * KAP1 fixture helpers（离线 fixture ≥1 条按真形状造；launch 事实 6）。
 * ------------------------------------------------------------------------ */
static size_t artt_kap1_hdr(uint8_t *buf, uint16_t fmt, uint16_t w, uint16_t h,
                            uint16_t stride)
{
    buf[0] = 'K'; buf[1] = 'A'; buf[2] = 'P'; buf[3] = '1';
    buf[4] = (uint8_t)(fmt & 0xFFu);
    buf[5] = (uint8_t)((fmt >> 8) & 0xFFu);
    buf[6] = 0; buf[7] = 0; /* flags */
    buf[8] = (uint8_t)(w & 0xFFu);
    buf[9] = (uint8_t)((w >> 8) & 0xFFu);
    buf[10] = (uint8_t)(h & 0xFFu);
    buf[11] = (uint8_t)((h >> 8) & 0xFFu);
    buf[12] = (uint8_t)(stride & 0xFFu);
    buf[13] = (uint8_t)((stride >> 8) & 0xFFu);
    buf[14] = 0; buf[15] = 0; /* reserved */
    return 16;
}

static int artt_write_file(const char *dir, const char *name,
                           const uint8_t *bytes, size_t n)
{
    char p[ARTT_MAX_PATH];
    FILE *f;
    size_t w;

    snprintf(p, sizeof(p), "%s/%s", dir, name);
    f = fopen(p, "wb");
    if (f == NULL) {
        return -1;
    }
    w = fwrite(bytes, 1, n, f);
    fclose(f);
    return w == n ? 0 : -1;
}

/* -------------------------------------------------------------------------- *
 * Test 2 — ART-A2 behavioral: mount/加载路径语义 + KAP1 双 format fixture 解码
 *   + 1bpp 像素语义 + 防御（坏 magic / 截断 / 非法 format / 中间灰 / NULL）。
 *   fixture 写入临时目录（不改仓内落地面）。
 * RED today: weak sentinel（mount 首断言即红）。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: mount/加载路径语义 + KAP1 离线 fixture 解码 + 防御 (ART-A2)",
          "[art_u][ART-A2][loader]")
{
    char tmpl[] = "/tmp/kidpal_art_rb_XXXXXX";
    char *tmpdir;
    uint8_t buf[64];
    size_t n;
    kidpal_art_img_t img;

    artt_require_impl(kidpal_art_mount("/tmp"), "kidpal_art_mount unimplemented");

    /* mount 防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(NULL) == -1,
        "ART-A2 RED: NULL root must be rejected (-1), not crash");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount("") == -1,
        "ART-A2 RED: empty root must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount("/kidpal_no_such_dir_rb") == -1,
        "ART-A2 RED: nonexistent root must be rejected (-1)");

    tmpdir = mkdtemp(tmpl);
    TEST_ASSERT_NOT_NULL_FM(tmpdir,
        "ART RED(infra): mkdtemp failed — fixture harness broken, not a behaviour red");

    /* 1BPP fixture: 8×2, stride 1; row0 = 10110001, row1 = 01000000 (MSB-first) */
    n = artt_kap1_hdr(buf, KIDPAL_ART_FMT_1BPP, 8, 2, 1);
    buf[n++] = 0xB1; buf[n++] = 0x40;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_1bpp.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* RGB565 fixture: 3×2, stride 6; 仅 (1,0) 为白 0xFFFF，其余墨 0x0000 */
    n = artt_kap1_hdr(buf, KIDPAL_ART_FMT_RGB565, 3, 2, 6);
    buf[n++] = 0x00; buf[n++] = 0x00; buf[n++] = 0xFF; buf[n++] = 0xFF;
    buf[n++] = 0x00; buf[n++] = 0x00;
    buf[n++] = 0x00; buf[n++] = 0x00; buf[n++] = 0x00; buf[n++] = 0x00;
    buf[n++] = 0x00; buf[n++] = 0x00;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_565.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* 中间灰 fixture: 1×1 RGB565 值 0x8000 = 非阈值化产物 */
    n = artt_kap1_hdr(buf, KIDPAL_ART_FMT_RGB565, 1, 1, 2);
    buf[n++] = 0x00; buf[n++] = 0x80;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_gray.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* 坏 magic fixture */
    n = artt_kap1_hdr(buf, KIDPAL_ART_FMT_1BPP, 1, 1, 1);
    buf[0] = 'X';
    buf[n++] = 0x00;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_badmagic.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* 非法 format fixture */
    n = artt_kap1_hdr(buf, 7, 1, 1, 1);
    buf[n++] = 0x00;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_badfmt.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* 截断 fixture（头宣称 8×2 但数据只有 1 字节） */
    n = artt_kap1_hdr(buf, KIDPAL_ART_FMT_1BPP, 8, 2, 1);
    buf[n++] = 0xB1;
    TEST_ASSERT_TRUE_FM(artt_write_file(tmpdir, "fixture_trunc.bin", buf, n) == 0,
        "ART RED(infra): fixture write failed");

    /* 切到 fixture root */
    artt_require_impl(kidpal_art_mount(tmpdir), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(tmpdir) == 0,
        "ART-A2 RED: mounting an existing directory failed");

    /* 1BPP 解码 + MSB-first 语义 */
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_1bpp.bin", &img) == 0,
        "ART-A2 RED: valid 1BPP fixture failed to load — KAP1 解码路径缺失 (ART-A2)");
    TEST_ASSERT_TRUE_FM(img.w == 8 && img.h == 2 &&
                             img.stride_bytes == 1 &&
                             img.format == KIDPAL_ART_FMT_1BPP &&
                             img.data != NULL,
        "ART-A2 RED: 1BPP fixture header fields wrong (w/h/stride/format/data)");
    TEST_ASSERT_TRUE_FM(
        kidpal_art_pixel(&img, 0, 0) == 1 && kidpal_art_pixel(&img, 1, 0) == 0 &&
        kidpal_art_pixel(&img, 2, 0) == 1 && kidpal_art_pixel(&img, 3, 0) == 1 &&
        kidpal_art_pixel(&img, 4, 0) == 0 && kidpal_art_pixel(&img, 7, 0) == 1 &&
        kidpal_art_pixel(&img, 0, 1) == 0 && kidpal_art_pixel(&img, 1, 1) == 1 &&
        kidpal_art_pixel(&img, 2, 1) == 0 && kidpal_art_pixel(&img, 7, 1) == 0,
        "ART-A2 RED: 1BPP MSB-first pixel semantics wrong (bit=1 为墨, 每行 MSB-first)");

    /* RGB565 解码 + 阈值化墨/纸语义 */
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_565.bin", &img) == 0,
        "ART-A2 RED: valid RGB565 fixture failed to load");
    TEST_ASSERT_TRUE_FM(img.w == 3 && img.h == 2 && img.stride_bytes == 6 &&
                             img.format == KIDPAL_ART_FMT_RGB565,
        "ART-A2 RED: RGB565 fixture header fields wrong");
    TEST_ASSERT_TRUE_FM(
        kidpal_art_pixel(&img, 0, 0) == 1 && kidpal_art_pixel(&img, 1, 0) == 0 &&
        kidpal_art_pixel(&img, 2, 0) == 1 && kidpal_art_pixel(&img, 0, 1) == 1 &&
        kidpal_art_pixel(&img, 2, 1) == 1,
        "ART-A2 RED: RGB565 thresholded semantics wrong (0x0000=墨, 0xFFFF=纸)");

    /* 中间灰 = 非阈值化产物 = 语义错误 */
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_gray.bin", &img) == 0,
        "ART-A2 RED: gray fixture should LOAD (format legal)…");
    TEST_ASSERT_TRUE_FM(kidpal_art_pixel(&img, 0, 0) == -1,
        "ART-A2 RED: intermediate gray pixel must be a semantics error (-1), "
        "not silently paper/ink — 非阈值化产物不得混过 1bpp 语义");

    /* 路径/文件防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_load(NULL, &img) == -1,
        "ART-A2 RED: NULL path must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_1bpp.bin", NULL) == -1,
        "ART-A2 RED: NULL out must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_load("missing_asset.bin", &img) == -1,
        "ART-A2 RED: missing file must be rejected (-1), not fake-success");
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_badmagic.bin", &img) == -1,
        "ART-A2 RED: bad magic must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_badfmt.bin", &img) == -1,
        "ART-A2 RED: illegal format must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_trunc.bin", &img) == -1,
        "ART-A2 RED: truncated pixel data must be rejected (-1)");

    /* pixel 防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_load("fixture_1bpp.bin", &img) == 0,
        "ART-A2 RED: reload for pixel defense failed");
    TEST_ASSERT_TRUE_FM(kidpal_art_pixel(NULL, 0, 0) == -1,
        "ART-A2 RED: NULL img to pixel() must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_pixel(&img, -1, 0) == -1 &&
                             kidpal_art_pixel(&img, 0, -1) == -1 &&
                             kidpal_art_pixel(&img, 8, 0) == -1 &&
                             kidpal_art_pixel(&img, 0, 2) == -1,
        "ART-A2 RED: out-of-range pixel coords must be rejected (-1), not OOB");

    /* 清理 + 还原 root（后续测试各自重新 mount） */
    {
        char p[ARTT_MAX_PATH];
        const char *const names[] = {
            "fixture_1bpp.bin", "fixture_565.bin", "fixture_gray.bin",
            "fixture_badmagic.bin", "fixture_badfmt.bin", "fixture_trunc.bin"
        };
        for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
            snprintf(p, sizeof(p), "%s/%s", tmpdir, names[i]);
            unlink(p);
        }
        rmdir(tmpdir);
    }
}

/* -------------------------------------------------------------------------- *
 * Test 3 — ART-A5: V3 资产本体 48 枚闭集落地 + 真实落盘尺寸逐枚对账。
 *   落地根 = firmware/main/assets/spiffs_root/（冻结路径）；48 枚命名/尺寸 =
 *   上游管线产物描述符实测采样（真记录字段抽样冻结进 artt_expected[]）。
 *   逐枚: load 成功 + w/h 精确 + format 合法 + data 非空 + 非空白（0 < 墨 < 全图）。
 * RED today: 模块未实现（weak 哨兵）；实现后若落地面缺失，红在「落地根不存在」。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: V3 资产本体 48 枚落地 + 真实落盘尺寸 (ART-A5)",
          "[art_u][ART-A5][landing]")
{
    char root[ARTT_MAX_PATH];
    int unimpl_seen = 0;
    int miss_root = 0;
    int miss_dim = 0;
    int miss_shape = 0;
    int miss_blank = 0;
    char first_problem[ARTT_MAX_PATH];

    first_problem[0] = '\0';
    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");

    {
        int mr = kidpal_art_mount(root);
        if (mr == KIDPAL_ART_UNIMPLEMENTED) {
            artt_require_impl(mr, "kidpal_art_mount unimplemented");
        }
        TEST_ASSERT_TRUE_FM(mr == 0,
            "ART-A5 RED: SPIFFS landing root firmware/main/assets/spiffs_root/ "
            "does not exist or is unreadable — V3 美术资产本体未落地固件 (ART-A5, "
            "BUILDER DUTY 2)");
    }

    for (size_t i = 0; i < ARTT_EXPECTED_N; i++) {
        const artt_expect_t *e = &artt_expected[i];
        kidpal_art_img_t img;

        if (kidpal_art_load(e->name, &img) != 0) {
            if (kidpal_art_load(e->name, &img) == KIDPAL_ART_UNIMPLEMENTED) {
                unimpl_seen = 1;
            } else {
                miss_root = 1;
            }
            if (first_problem[0] == '\0') {
                snprintf(first_problem, sizeof(first_problem), "%s", e->name);
            }
            continue;
        }
        if (img.w != e->w || img.h != e->h || img.data == NULL ||
            (img.format != KIDPAL_ART_FMT_1BPP && img.format != KIDPAL_ART_FMT_RGB565)) {
            miss_dim = 1;
            if (first_problem[0] == '\0') {
                snprintf(first_problem, sizeof(first_problem), "%s", e->name);
            }
            continue;
        }
        /* 每行最小 stride（落盘形状红线） */
        {
            int min_stride = (img.format == KIDPAL_ART_FMT_1BPP)
                                 ? (img.w + 7) / 8
                                 : img.w * 2;
            if (img.stride_bytes < min_stride) {
                miss_shape = 1;
                if (first_problem[0] == '\0') {
                    snprintf(first_problem, sizeof(first_problem), "%s", e->name);
                }
                continue;
            }
        }
        /* 非空白（排除全黑/全白/空图; 上游 UAA 同款机判近似） */
        {
            long ink = 0;
            long total = (long)img.w * (long)img.h;
            for (int y = 0; y < img.h && miss_blank == 0; y++) {
                for (int x = 0; x < img.w; x++) {
                    int p = kidpal_art_pixel(&img, x, y);
                    if (p != 0 && p != 1) {
                        miss_blank = 2; /* 像素语义错误 */
                        break;
                    }
                    ink += p;
                }
            }
            if (miss_blank == 0 && (ink <= 0 || ink >= total)) {
                miss_blank = 1;
            }
            if (miss_blank != 0 && first_problem[0] == '\0') {
                snprintf(first_problem, sizeof(first_problem), "%s", e->name);
            }
        }
    }

    if (unimpl_seen) {
        artt_require_impl(KIDPAL_ART_UNIMPLEMENTED, "kidpal_art_load unimplemented");
    }
    TEST_ASSERT_TRUE_FM(!miss_root,
        "ART-A5 RED: landing asset load failed (first: %s) — 落地面缺枚/缺文件 "
        "(48 枚闭集必须全量落地, ART-A5)",
        first_problem[0] ? first_problem : "?");
    TEST_ASSERT_TRUE_FM(!miss_dim,
        "ART-A5 RED: landing asset dims/format mismatch against the sampled "
        "pipeline truth (first: %s) — 落盘尺寸与上游 v3 出图产物不符",
        first_problem[0] ? first_problem : "?");
    TEST_ASSERT_TRUE_FM(!miss_shape,
        "ART-A5 RED: landing asset stride below per-row minimum (first: %s) — "
        "KAP1 落盘形状违规",
        first_problem[0] ? first_problem : "?");
    TEST_ASSERT_TRUE_FM(miss_blank == 0,
        "ART-A5 RED: landing asset blank/invalid pixels (first: %s, %s) — "
        "资产渲染为空白或像素语义错误",
        first_problem[0] ? first_problem : "?", miss_blank == 2 ? "pixel -1" : "blank");
}

/* -------------------------------------------------------------------------- *
 * Test 4 — ART-A5 provenance/B8: v3 母版溯源闭环 + 裁定快照在世 + 姿态族逐枚
 *   互异 + 落地集闭集枚举（B8 打包红线 = 表外零容身）。
 *   guard-class: landing（钉死 v3 定稿母版 + v3 裁定快照——落盘类守卫钉死某版
 *   合理，ART-E4 钉 V3 为前置输入；勿按接线类守卫做版本无关化）。
 * RED today: weak sentinel（姿态快照依赖 loader）；溯源锚为在世守卫。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: v3 母版溯源闭环 + 姿态族逐枚互异 + B8 打包红线 (ART-A5)",
          "[art_u][ART-A5][provenance]")
{
    char root[ARTT_MAX_PATH];

    ARTT_REQUIRE_ROOT();

    /* 溯源锚 1: 管线机判脚本的母版指针已切 v3 定稿（verify_art_assets.mjs 的
     * HTML 指针；gen_assets.mjs 的母版经 CLI 参 design.html 传入, 无硬编码） */
    TEST_ASSERT_TRUE_FM(
        artt_file_contains_ci("tools/asset-pipeline/verify_art_assets.mjs",
                              "kidpal-ui-design-v3.html"),
        "ART-A5 RED(provenance): verify_art_assets.mjs HTML master pointer is not "
        "the v3 finalized design — 资产管线母版未钉 v3 定稿 (ART-E4 前置输入漂移)");

    /* 溯源锚 2: v3 稿承载蝴蝶结裁定（mp-head 共享组件 + 蝴蝶结注记在世） */
    TEST_ASSERT_TRUE_FM(
        artt_file_contains_ci("docs/ui/kidpal-ui-design-v3.html", "mp-head") &&
        artt_file_contains_ci("docs/ui/kidpal-ui-design-v3.html", "蝴蝶结"),
        "ART-A5 RED(provenance): v3 design master lost the mp-head/bow ruling — "
        "v3 定稿裁定③（吉祥物左侧头顶蝴蝶结, 7 姿态同位同形）锚点缺失");

    /* 溯源锚 3: human 裁定快照证据在世（pose_daily_bow_3x.png） */
    TEST_ASSERT_TRUE_FM(
        artt_exists("tools/asset-pipeline/report/snapshots/pose_daily_bow_3x.png"),
        "ART-A5 RED(provenance): bow ruling snapshot pose_daily_bow_3x.png missing "
        "from tools/asset-pipeline/report/snapshots/ — v3 裁定快照证据丢失");

    /* B8 红线: 笔形渲染保持包 4 矢量路径——若包 4 渲染侧接了资产层，则不得
     * 请求任何逐笔/逐态烘焙帧资产名；P4 米字格大框单份背景 = frm_236_236
     * （236×236, v3 稿 P4 .frm 同源, 闭集枚举见下） */
    {
        char *cs = artt_read("firmware/main/display/char_stroke.c", NULL);
        if (cs != NULL) {
            /* 若包 4 渲染侧接了资产层，则不得请求任何逐笔/逐态烘焙帧资产名 */
            if (artt_contains_ci(cs, "kidpal_art_load") ||
                artt_contains_ci(cs, "kidpal_art_render_asset")) {
                TEST_ASSERT_FALSE_FM(
                    artt_contains_ci(cs, "stroke_state") ||
                    artt_contains_ci(cs, "chst") ||
                    artt_contains_ci(cs, "stroke_b"),
                    "ART-A5 RED(B8): char_stroke.c requests baked per-stroke/per-state "
                    "frame assets — B8 打包策略锁定「笔形 3 态叠加复用 + 米字格单份"
                    "背景」被违反（禁逐笔逐态整帧）");
            }
            free(cs);
        }
    }

    /* 落地集闭集枚举：目录平铺 .bin == 48 枚白名单，无表外文件（B8 无容身位）
     * + 7 姿态逐枚互异（换姿态族真差异; 同位同形的像素级观感归 design review/9b） */
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");
    {
        int mr = kidpal_art_mount(root);
        if (mr == KIDPAL_ART_UNIMPLEMENTED) {
            artt_require_impl(mr, "kidpal_art_mount unimplemented");
        } else if (mr == 0) {
            DIR *d = opendir(root);
            TEST_ASSERT_NOT_NULL_FM(d,
                "ART-A5 RED: landing root not enumerable — 落地目录缺失");
            if (d != NULL) {
                int found = 0;
                int foreign = 0;
                char foreign_name[ARTT_MAX_PATH];
                struct dirent *e;
                foreign_name[0] = '\0';
                while ((e = readdir(d)) != NULL) {
                    int known = 0;
                    if (e->d_name[0] == '.') {
                        continue;
                    }
                    for (size_t i = 0; i < ARTT_EXPECTED_N; i++) {
                        if (strcmp(e->d_name, artt_expected[i].name) == 0) {
                            known = 1;
                            break;
                        }
                    }
                    if (!known) {
                        foreign = 1;
                        if (foreign_name[0] == '\0') {
                            snprintf(foreign_name, sizeof(foreign_name), "%s", e->d_name);
                        }
                    } else {
                        found++;
                    }
                }
                closedir(d);
                TEST_ASSERT_TRUE_FM(foreign == 0,
                    "ART-A5 RED(B8): landing root contains non-whitelisted file \"%s\" — "
                    "落地资产集必须 == 48 枚闭集白名单（禁逐笔逐态整帧烘焙, 表外零容身）",
                    foreign_name[0] ? foreign_name : "?");
                TEST_ASSERT_TRUE_FM(found == (int)ARTT_EXPECTED_N,
                    "ART-A5 RED(B8): landing whitelist hit count = %d, expected %d — "
                    "闭集枚举不齐（与逐枚 load 对账互为印证）",
                    found, (int)ARTT_EXPECTED_N);
            }

            /* 7 姿态逐枚互异 */
            {
                uint8_t *snaps[7];
                kidpal_art_img_t imgs[7];
                const char *pose_names[7] = {
                    "pose_daily_96.bin", "pose_listen_96.bin", "pose_think_96.bin",
                    "pose_talk_96.bin", "pose_record_96.bin", "pose_sleep_96.bin",
                    "pose_jump_96.bin"
                };
                size_t npix = (size_t)96 * (size_t)96;
                int loaded = 1;

                for (int i = 0; i < 7; i++) {
                    snaps[i] = NULL;
                    if (kidpal_art_load(pose_names[i], &imgs[i]) != 0 ||
                        imgs[i].w != 96 || imgs[i].h != 96) {
                        loaded = 0;
                        break;
                    }
                    snaps[i] = (uint8_t *)malloc(npix);
                    assert(snaps[i] != NULL); /* logic guard */
                    if (artt_snap(&imgs[i], snaps[i]) != 0) {
                        loaded = 0;
                        break;
                    }
                }
                if (loaded) {
                    for (int i = 0; i < 7; i++) {
                        for (int j = i + 1; j < 7; j++) {
                            TEST_ASSERT_TRUE_FM(
                                memcmp(snaps[i], snaps[j], npix) != 0,
                                "ART-A5 RED: two pose bitmaps are pixel-identical "
                                "(pose pair %d/%d) — 姿态族未逐枚出图（占位复制=假落地）",
                                i, j);
                        }
                    }
                }
                for (int i = 0; i < 7; i++) {
                    free(snaps[i]);
                }
                if (!loaded) {
                    artt_require_impl(KIDPAL_ART_UNIMPLEMENTED,
                                      "pose assets not loadable (loader/landing missing)");
                }
            }
        } else {
            TEST_ASSERT_TRUE_FM(0,
                "ART-A5 RED: landing root firmware/main/assets/spiffs_root/ missing "
                "— 闭集枚举与姿态族互异无法执行（BUILDER DUTY 2）");
        }
    }
}

/* -------------------------------------------------------------------------- *
 * Test 5 — ART-A3 behavioral: imgfont 字形族——等宽箱体 + 逐字符变化 + 防御。
 *   11 枚（0-9 + 冒号）逐枚同宽同高（48×64 采样真值）；数字族两两互异（同字形
 *   逐字符变化——动态时间逐位换字的语义承载）；冒号与数字互异；advance 合理。
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: imgfont 字形族——等宽箱体 + 逐字符变化 + 防御 (ART-A3)",
          "[art_u][ART-A3][imgfont]")
{
    char root[ARTT_MAX_PATH];
    uint8_t *snaps[11];
    kidpal_art_img_t imgs[11];
    const char chars[11] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':' };
    size_t npix = (size_t)48 * (size_t)64;
    int adv;
    int loaded = 1;

    memset(snaps, 0, sizeof(snaps));
    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");

    artt_require_impl(kidpal_art_mount(root), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(root) == 0,
        "ART-A3 RED: mounting the landing root failed");

    for (int i = 0; i < 11; i++) {
        if (kidpal_art_glyph(chars[i], &imgs[i]) != 0) {
            loaded = 0;
            break;
        }
        if (imgs[i].w != 48 || imgs[i].h != 64 || imgs[i].data == NULL) {
            loaded = 0;
            break;
        }
        snaps[i] = (uint8_t *)malloc(npix);
        assert(snaps[i] != NULL); /* logic guard */
        if (artt_snap(&imgs[i], snaps[i]) != 0) {
            loaded = 0;
            break;
        }
    }
    artt_require_impl(loaded ? 0 : KIDPAL_ART_UNIMPLEMENTED,
                      "kidpal_art_glyph unimplemented (或 imgfont 族未落地/尺寸漂移)");
    TEST_ASSERT_TRUE_FM(loaded,
        "ART-A3 RED: imgfont glyph family not loadable at uniform 48x64 box — "
        "Baloo 2 动态时间 imgfont 缺枚/等宽箱体破坏 (ART-A3)");

    /* 数字族两两互异 + 冒号互异（同字形逐字符变化） */
    for (int i = 0; i < 11; i++) {
        for (int j = i + 1; j < 11; j++) {
            TEST_ASSERT_TRUE_FM(memcmp(snaps[i], snaps[j], npix) != 0,
                "ART-A3 RED: glyphs '%c' and '%c' are pixel-identical — 逐字符变化"
                "缺失（动态时间逐位换字无意义）", chars[i], chars[j]);
        }
    }

    /* advance 步进合理（>= 字形宽, <= 2 倍, > 0） */
    artt_require_impl(kidpal_art_glyph_advance(), "kidpal_art_glyph_advance unimplemented");
    adv = kidpal_art_glyph_advance();
    TEST_ASSERT_TRUE_FM(adv >= 48 && adv <= 96,
        "ART-A3 RED: glyph advance %d outside [48,96] — 时间数字排版步进失控 "
        "(冻结: >= 数字字形宽且 <= 2 倍)", adv);

    /* 防御：非时间字符一律拒绝 */
    TEST_ASSERT_TRUE_FM(kidpal_art_glyph('a', &imgs[0]) == -1,
        "ART-A3 RED: glyph('a') must be rejected (-1) — imgfont 只留给动态时间");
    TEST_ASSERT_TRUE_FM(kidpal_art_glyph(' ', &imgs[0]) == -1,
        "ART-A3 RED: glyph(' ') must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_glyph('.', &imgs[0]) == -1,
        "ART-A3 RED: glyph('.') must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_glyph('0', NULL) == -1,
        "ART-A3 RED: NULL out must be rejected (-1)");

    for (int i = 0; i < 11; i++) {
        free(snaps[i]);
    }
}

/* -------------------------------------------------------------------------- *
 * Test 6 — ART-A2 render: 1bpp 位图渲染像素语义——真姿态资产 1:1 渲染进渲染缝，
 *   96×96 区域逐像素 == 资产位图（墨/纸两值域内比对，渲染像素语义非「没报错」）。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 1bpp 位图渲染像素语义——真姿态资产逐像素比对 (ART-A2 render)",
          "[art_u][ART-A2][render]")
{
    char root[ARTT_MAX_PATH];
    kidpal_art_img_t img;

    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");
    artt_require_impl(kidpal_art_mount(root), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(root) == 0,
        "ART-A2 RED(render): mount failed");

    TEST_ASSERT_TRUE_FM(host_infra_render_frame() == 0,
        "ART RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    artt_require_impl(kidpal_art_render_asset("pose_daily_96.bin", 120, 80),
                      "kidpal_art_render_asset unimplemented (1bpp 渲染面未实现)");
    TEST_ASSERT_TRUE_FM(kidpal_art_render_asset("pose_daily_96.bin", 120, 80) == 0,
        "ART-A2 RED(render): rendering the real pose asset failed — 资产未按路径"
        "加载并渲染 (ART-A2)");

    TEST_ASSERT_TRUE_FM(kidpal_art_load("pose_daily_96.bin", &img) == 0 &&
                             img.w == 96 && img.h == 96,
        "ART-A2 RED(render): reference load of pose_daily_96 failed — 对账基准缺失");

    for (int y = 0; y < 96; y++) {
        for (int x = 0; x < 96; x++) {
            int32_t p = host_infra_read_pixel(120 + x, 80 + y);
            int ap = kidpal_art_pixel(&img, x, y);
            if (p == ARTT_NOT_RENDERED) {
                TEST_ASSERT_TRUE_FM(0,
                    "ART-A2 RED(render): seam returned NOT_RENDERED inside the "
                    "drawn rect — 渲染缝未刷新（帧未入缝）");
            }
            TEST_ASSERT_TRUE_FM(
                (ap == 1 && p == (int32_t)ARTT_INK) ||
                (ap == 0 && p == (int32_t)ARTT_PAPER),
                "ART-A2 RED(render): rendered pixel (%d,%d) = 0x%lX != asset pixel "
                "%d — 渲染像素语义与资产位图不符（非 1:1 资产绘制）",
                x, y, (unsigned long)p, ap);
        }
    }

    /* 防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_render_asset(NULL, 0, 0) == -1,
        "ART-A2 RED(render): NULL path must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(
        kidpal_art_render_asset("no_such_asset_rb.bin", 0, 0) == -1,
        "ART-A2 RED(render): unknown asset must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * Test 7 — ART-A4 render: 9-slice 手绘框拉伸——24px 角片逐像素不变形（4 角 24×24
 *   与原框角片逐像素一致）+ 拉伸区边框连续在场。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 9-slice 拉伸渲染——24px 角片不变形 + 边框连续 (ART-A4)",
          "[art_u][ART-A4][nineslice]")
{
    char root[ARTT_MAX_PATH];
    kidpal_art_img_t base;
    const int C = KIDPAL_ART_9SLICE_CORNER_PX;
    const int dx = 20, dy = 50, w = 356, h = 220;

    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");
    artt_require_impl(kidpal_art_mount(root), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(root) == 0,
        "ART-A4 RED(render): mount failed");

    TEST_ASSERT_TRUE_FM(host_infra_render_frame() == 0,
        "ART RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    artt_require_impl(kidpal_art_draw_nine_slice("frm_236_236", dx, dy, w, h),
                      "kidpal_art_draw_nine_slice unimplemented (9-slice 渲染面未实现)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_nine_slice("frm_236_236", dx, dy, w, h) == 0,
        "ART-A4 RED(render): 9-slice draw of frm_236_236 failed — 手绘框族拉伸"
        "渲染缺失 (ART-A4)");

    TEST_ASSERT_TRUE_FM(kidpal_art_load("frm_236_236.bin", &base) == 0 &&
                             base.w == 236 && base.h == 236,
        "ART-A4 RED(render): base frame load failed — 9-slice 对账基准缺失");

    /* 4 角 24×24 逐像素不变形（stretch=false 的渲染承载） */
    {
        const int cx[4] = { 0, base.w - C, 0, base.w - C };
        const int cy[4] = { 0, 0, base.h - C, base.h - C };
        const int sx[4] = { dx, dx + w - C, dx, dx + w - C };
        const int sy[4] = { dy, dy, dy + h - C, dy + h - C };
        const char *const corner_names[4] = { "tl", "tr", "bl", "br" };

        for (int k = 0; k < 4; k++) {
            for (int yy = 0; yy < C; yy++) {
                for (int xx = 0; xx < C; xx++) {
                    int32_t p = host_infra_read_pixel(sx[k] + xx, sy[k] + yy);
                    int ap = kidpal_art_pixel(&base, cx[k] + xx, cy[k] + yy);
                    TEST_ASSERT_TRUE_FM(
                        (ap == 1 && p == (int32_t)ARTT_INK) ||
                        (ap == 0 && p == (int32_t)ARTT_PAPER),
                        "ART-A4 RED(render): 9-slice corner %s pixel (%d,%d) distorted "
                        "(fb=0x%lX, base=%d) — 24px 角片协议被拉伸破坏 (v3 定稿裁定①)",
                        corner_names[k], xx, yy, (unsigned long)p, ap);
                }
            }
        }
    }

    /* 拉伸区边框连续在场（顶边中点窗口有墨——手绘框边线随边片拉伸延展） */
    {
        int mid_ink = 0;
        for (int yy = dy; yy < dy + 10; yy++) {
            for (int xx = dx + w / 2 - 8; xx <= dx + w / 2 + 8; xx++) {
                int32_t p = host_infra_read_pixel(xx, yy);
                if (p != ARTT_NOT_RENDERED && p != (int32_t)ARTT_PAPER) {
                    mid_ink++;
                }
            }
        }
        TEST_ASSERT_TRUE_FM(mid_ink > 0,
            "ART-A4 RED(render): stretched top-border window has no ink — 边片未"
            "沿单轴拉伸（9 宫格边中段断裂）");
    }

    /* 防御 */
    TEST_ASSERT_TRUE_FM(
        kidpal_art_draw_nine_slice("frm_236_236", 0, 0, 2 * C - 1, 100) == -1,
        "ART-A4 RED(render): target width below 2*corner must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(
        kidpal_art_draw_nine_slice("frm_236_236", 0, 0, 100, 2 * C - 1) == -1,
        "ART-A4 RED(render): target height below 2*corner must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_nine_slice(NULL, 0, 0, 100, 100) == -1,
        "ART-A4 RED(render): NULL name must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(
        kidpal_art_draw_nine_slice("no_such_frame_rb", 0, 0, 100, 100) == -1,
        "ART-A4 RED(render): unknown frame name must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * Test 8 — ART-A4 render: 抖动网点 d50/d25/d12——4px 平铺周期 + 三档密度严格
 *   递增（d50 > d25 > d12）+ 非空白非全黑。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 抖动网点渲染——4px 平铺周期 + 三档密度递增 (ART-A4)",
          "[art_u][ART-A4][dots]")
{
    char root[ARTT_MAX_PATH];
    const char *const names[3] = { "d50", "d25", "d12" };
    const int dx = 60, dy = 60, w = 16, h = 12;
    int ink[3];

    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");
    artt_require_impl(kidpal_art_mount(root), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(root) == 0,
        "ART-A4 RED(render): mount failed");

    TEST_ASSERT_TRUE_FM(host_infra_render_frame() == 0,
        "ART RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    for (int k = 0; k < 3; k++) {
        artt_require_impl(kidpal_art_draw_dots(names[k], dx, dy, w, h),
                          "kidpal_art_draw_dots unimplemented (网点渲染面未实现)");
        TEST_ASSERT_TRUE_FM(kidpal_art_draw_dots(names[k], dx, dy, w, h) == 0,
            "ART-A4 RED(render): dots draw of %s failed — 抖动网点渲染缺失 (ART-A4)",
            names[k]);

        /* 4px 平铺周期：格内任意 (x,y) 与 (x+4,y)/(x,y+4) 同值 */
        for (int yy = 0; yy < h; yy++) {
            for (int xx = 0; xx < w; xx++) {
                int32_t p = host_infra_read_pixel(dx + xx, dy + yy);
                if (p == ARTT_NOT_RENDERED) {
                    TEST_ASSERT_TRUE_FM(0,
                        "ART-A4 RED(render): seam returned NOT_RENDERED inside the "
                        "dotted rect — 渲染缝未刷新");
                }
                if (xx + 4 < w) {
                    int32_t q = host_infra_read_pixel(dx + xx + 4, dy + yy);
                    TEST_ASSERT_TRUE_FM(p == q,
                        "ART-A4 RED(render): dots tiling not 4px-periodic in x at "
                        "(%d,%d) — 网点单元非 4px 格（防糊红线）", xx, yy);
                }
                if (yy + 4 < h) {
                    int32_t q = host_infra_read_pixel(dx + xx, dy + yy + 4);
                    TEST_ASSERT_TRUE_FM(p == q,
                        "ART-A4 RED(render): dots tiling not 4px-periodic in y at "
                        "(%d,%d) — 网点单元非 4px 格（防糊红线）", xx, yy);
                }
            }
        }

        /* 窗口墨计数（同尺寸窗口密度对账用） */
        ink[k] = 0;
        for (int yy = 0; yy < h; yy++) {
            for (int xx = 0; xx < w; xx++) {
                int32_t p = host_infra_read_pixel(dx + xx, dy + yy);
                if (p != ARTT_NOT_RENDERED && p != (int32_t)ARTT_PAPER) {
                    ink[k]++;
                }
            }
        }
        TEST_ASSERT_TRUE_FM(ink[k] > 0 && ink[k] < w * h,
            "ART-A4 RED(render): %s window ink=%d must be inside (0, %d) — "
            "网点渲染空白或全黑", names[k], ink[k], w * h);
    }

    /* 三档密度严格递增（上游 UAA-A5.2 同款判据的渲染承载） */
    TEST_ASSERT_TRUE_FM(ink[0] > ink[1] && ink[1] > ink[2],
        "ART-A4 RED(render): dot density not strictly d50 > d25 > d12 (%d/%d/%d) "
        "— 三档网点密度失序", ink[0], ink[1], ink[2]);

    /* 防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_dots("d50", 0, 0, 0, 10) == -1,
        "ART-A4 RED(render): zero width must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_dots("d50", 0, 0, 10, -1) == -1,
        "ART-A4 RED(render): negative height must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_dots(NULL, 0, 0, 10, 10) == -1,
        "ART-A4 RED(render): NULL name must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_dots("d99", 0, 0, 10, 10) == -1,
        "ART-A4 RED(render): unknown dots name must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * Test 9 — ART-A3 render: 动态时间 imgfont——"12:34" 逐字符单元格与对应字形
 *   位图逐像素比对（第 i 字符画于 x = dx + i*advance, 1:1 原尺寸）。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 动态时间 imgfont 渲染——逐字符单元格比对 (ART-A3 render)",
          "[art_u][ART-A3][render]")
{
    char root[ARTT_MAX_PATH];
    const char *hhmm = "12:34";
    int adv;
    int x0 = 60, y0 = 80;

    ARTT_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_FM(artt_landing_path(root, sizeof(root)),
        "ART RED(infra): repo root not located");
    artt_require_impl(kidpal_art_mount(root), "kidpal_art_mount unimplemented");
    TEST_ASSERT_TRUE_FM(kidpal_art_mount(root) == 0,
        "ART-A3 RED(render): mount failed");

    artt_require_impl(kidpal_art_glyph_advance(),
                      "kidpal_art_glyph_advance unimplemented");
    adv = kidpal_art_glyph_advance();
    TEST_ASSERT_TRUE_FM(x0 + 5 * adv + 48 <= ARTT_FRAME_W &&
                             y0 + 64 <= ARTT_FRAME_H,
        "ART RED(infra): time draw rect exceeds the 400x300 seam frame — 测试"
        "布局错（非行为红）");

    TEST_ASSERT_TRUE_FM(host_infra_render_frame() == 0,
        "ART RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    artt_require_impl(kidpal_art_draw_time(hhmm, x0, y0),
                      "kidpal_art_draw_time unimplemented (动态时间渲染面未实现)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_time(hhmm, x0, y0) == 0,
        "ART-A3 RED(render): draw_time(\"12:34\") failed — 动态时间 imgfont 组合"
        "渲染缺失 (ART-A3)");

    for (int i = 0; i < 5; i++) {
        kidpal_art_img_t g;
        TEST_ASSERT_TRUE_FM(kidpal_art_glyph(hhmm[i], &g) == 0,
            "ART-A3 RED(render): glyph '%c' lookup failed during verification",
            hhmm[i]);
        for (int yy = 0; yy < g.h; yy++) {
            for (int xx = 0; xx < g.w; xx++) {
                int32_t p = host_infra_read_pixel(x0 + i * adv + xx, y0 + yy);
                int ap = kidpal_art_pixel(&g, xx, yy);
                TEST_ASSERT_TRUE_FM(
                    (ap == 1 && p == (int32_t)ARTT_INK) ||
                    (ap == 0 && p == (int32_t)ARTT_PAPER),
                    "ART-A3 RED(render): time cell %d ('%c') pixel (%d,%d) = 0x%lX "
                    "!= glyph %d — 逐字符字形未按 advance 步进 1:1 绘制",
                    i, hhmm[i], xx, yy, (unsigned long)p, ap);
            }
        }
    }

    /* 同一时刻不同位互异的渲染承载由字形互异 + 逐格比对保证（'1' 格 != '2' 格） */
    {
        int32_t a = host_infra_read_pixel(x0 + 8, y0 + 8);
        int32_t b = host_infra_read_pixel(x0 + adv + 8, y0 + 8);
        /* '1' 与 '2' 字形互异（test 5 已钉），同偏移像素不必互异但两格整图互异
         * 已由逐格比对覆盖；此处只探针渲染确实发生了（两格均非 NOT_RENDERED）。 */
        TEST_ASSERT_TRUE_FM(a != ARTT_NOT_RENDERED && b != ARTT_NOT_RENDERED,
            "ART-A3 RED(render): time cells not rendered — 渲染缝未刷新");
    }

    /* 防御 */
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_time(NULL, 0, 0) == -1,
        "ART-A3 RED(render): NULL hhmm must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_time("", 0, 0) == -1,
        "ART-A3 RED(render): empty hhmm must be rejected (-1)");
    TEST_ASSERT_TRUE_FM(kidpal_art_draw_time("12a34", 0, 0) == -1,
        "ART-A3 RED(render): non-time character must be rejected (-1) — imgfont "
        "只留给动态时间数字");
}

/* -------------------------------------------------------------------------- *
 * Test 10 — ART-A6: 换皮接线——包 2/4/5/6/7/8 六页渲染面统一消费共享资产层。
 *   每个渲染面模块 include "art_assets.h" 或调用 kidpal_art_*（结构齿; 装配
 *   事实，非浮扫描）。
 * RED today: 六页均未接资产层。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 换皮接线——六页渲染面统一消费共享资产层 (ART-A6)",
          "[art_u][ART-A6][wiring]")
{
    const char *const faces[] = {
        "firmware/main/display/view_router.c",   /* 包 2 路由底座（mount 接线点） */
        "firmware/main/display/char_stroke.c",   /* 包 4 查字 */
        "firmware/main/display/home_p1.c",       /* 包 5 首页 */
        "firmware/main/display/schedule_page.c", /* 包 6 课表 */
        "firmware/main/display/reminder_page.c", /* 包 7 提醒弹窗 */
        "firmware/main/display/story_page.c",    /* 包 8 故事 */
    };

    ARTT_REQUIRE_ROOT();

    for (size_t i = 0; i < sizeof(faces) / sizeof(faces[0]); i++) {
        TEST_ASSERT_TRUE_FM(artt_exists(faces[i]),
            "ART-A6 RED(infra): page render face %s missing — 页面包实现面被移除"
            "（换皮必须 additive, ART-E3 不 supersede）", faces[i]);
        TEST_ASSERT_TRUE_FM(
            artt_file_contains_ci(faces[i], "art_assets.h") ||
            artt_file_contains_ci(faces[i], "kidpal_art_"),
            "ART-A6 RED: %s does not consume the shared asset layer (no "
            "art_assets.h include / kidpal_art_* call) — 渲染面未接入换皮 (ART-A6)",
            faces[i]);
    }
}

/* -------------------------------------------------------------------------- *
 * Test 11 — ART-A7: 换皮后各页面 host 回归——六包冻结契约抽查一条。
 *   抽查集 = 包 2 路由 / 包 6 圆点两态 / 包 4 坐标变换 / 包 5 倒计时数值半 /
 *   包 7 fire_at 解析 / 包 8 盖章几何。官方 runner 上为硬断言（回归面）；
 *   stub harness（-DKIDPAL_ART_RB_STUB）下页面包未链入按 skip 记录。
 *   页面包原验收套件在同 runner 全量复跑为附加通道（ART-E3 不 supersede）。
 * ------------------------------------------------------------------------ */
TEST_CASE("art u: 换皮后页面 host 回归——六包冻结契约抽查 (ART-A7)",
          "[art_u][ART-A7][regression]")
{
    ARTT_REQUIRE_ROOT();

    /* 包 2: view 路由底座（VRT 数值码; NULL -> -1 降级） */
    ARTT_RB_REQUIRE_LINKED(kidpal_display_route, "kidpal_display_route");
    TEST_ASSERT_TRUE_FM(kidpal_display_route("home") == 1,
        "ART-A7 RED(regression): view_router \"home\" route broken — 换皮动到"
        "包 2 路由底座 (契约 §1.3 ⑥)");
    TEST_ASSERT_TRUE_FM(kidpal_display_route("schedule") == 4,
        "ART-A7 RED(regression): view_router \"schedule\" route broken");
    TEST_ASSERT_TRUE_FM(kidpal_display_route("text") == 0,
        "ART-A7 RED(regression): view_router \"text\" route broken");
    TEST_ASSERT_TRUE_FM(kidpal_display_route(NULL) == -1,
        "ART-A7 RED(regression): view_router NULL must degrade to -1");

    /* 包 6: 课表圆点两态（空心=已上完/明天, 实心=待上） */
    ARTT_RB_REQUIRE_LINKED(kidpal_schedule_dot_state, "kidpal_schedule_dot_state");
    {
        int v = kidpal_schedule_dot_state("14:00", "13:00", 0);
        TEST_ASSERT_TRUE_FM(v != KIDPAL_ART_UNIMPLEMENTED,
            "ART-A7 RED(regression): schedule_dot_state sentinel -2 — 包 6 冻结"
            "契约未链接（回归面断裂）");
        TEST_ASSERT_TRUE_FM(kidpal_schedule_dot_state("14:00", "13:00", 0) == 1,
            "ART-A7 RED(regression): 待上(13:00<14:00) must be solid dot (1)");
        TEST_ASSERT_TRUE_FM(kidpal_schedule_dot_state("14:00", "14:00", 0) == 0,
            "ART-A7 RED(regression): 已开始(14:00==14:00) must be hollow dot (0)");
        TEST_ASSERT_TRUE_FM(kidpal_schedule_dot_state("14:00", NULL, 1) == 0,
            "ART-A7 RED(regression): 明天全空心语义 broken");
    }

    /* 包 4: 查字坐标变换（屏内 + 字形偏上 + Y 翻转单调承载点） */
    ARTT_RB_REQUIRE_LINKED(kidpal_char_transform, "kidpal_char_transform");
    {
        int sx = -99, sy = -99;
        int v = kidpal_char_transform(512.0f, 512.0f, &sx, &sy);
        TEST_ASSERT_TRUE_FM(v != KIDPAL_ART_UNIMPLEMENTED,
            "ART-A7 RED(regression): char_transform sentinel -2 — 包 4 冻结契约"
            "未链接（回归面断裂）");
        TEST_ASSERT_TRUE_FM(v == 0 && sx >= 0 && sx < 400 && sy >= 0 && sy < 150,
            "ART-A7 RED(regression): char_transform(512,512) must map in-screen "
            "and strictly above middle (CHR-A3 P1/P4) — 包 4 坐标契约被换皮破坏");
    }

    /* 包 5: 倒计时贴纸数值半（210 分钟 -> 3 时 30 分; 最近未开始项） */
    ARTT_RB_REQUIRE_LINKED(kidpal_home_p1_countdown, "kidpal_home_p1_countdown");
    {
        artt_hp1_item_t items[2];
        int idx = -99, h = -99, m = -99;
        int v;

        memset(items, 0, sizeof(items));
        snprintf(items[0].time, sizeof(items[0].time), "16:30");
        snprintf(items[0].name, sizeof(items[0].name), "英语培训");
        snprintf(items[1].time, sizeof(items[1].time), "14:00");
        snprintf(items[1].name, sizeof(items[1].name), "钢琴课");
        v = kidpal_home_p1_countdown(items, 2, 10, 30, &idx, &h, &m);
        TEST_ASSERT_TRUE_FM(v != KIDPAL_ART_UNIMPLEMENTED,
            "ART-A7 RED(regression): home_p1_countdown sentinel -2 — 包 5 冻结"
            "契约未链接（回归面断裂）");
        TEST_ASSERT_TRUE_FM(v == 0 && idx == 1 && h == 3 && m == 30,
            "ART-A7 RED(regression): countdown 10:30 -> 14:00 must pick index 1 "
            "with 3h30m remaining — 包 5 倒计时数值半被换皮破坏");
    }

    /* 包 7: 提醒 fire_at 解析（冻结锚: +08:00 与 Z 等价, 16:15 == 29815695 分） */
    ARTT_RB_REQUIRE_LINKED(kidpal_fire_at_minutes, "kidpal_fire_at_minutes");
    {
        long long m = kidpal_fire_at_minutes("2026-09-09T16:15:00+08:00");
        TEST_ASSERT_TRUE_FM(m != (long long)KIDPAL_ART_UNIMPLEMENTED,
            "ART-A7 RED(regression): fire_at_minutes sentinel -2 — 包 7 冻结契约"
            "未链接（回归面断裂）");
        TEST_ASSERT_TRUE_FM(m == 29815695LL,
            "ART-A7 RED(regression): fire_at_minutes value drifted — 包 7 时间"
            "解析契约被换皮破坏");
        TEST_ASSERT_TRUE_FM(
            kidpal_fire_at_minutes("2026-09-09T08:15:00Z") == m,
            "ART-A7 RED(regression): Z vs +08:00 equivalence broken");
    }

    /* 包 8: 盖章几何（rings=2, inner_dashed=1, angle_deci=-80, hold_ms=800） */
    ARTT_RB_REQUIRE_LINKED(kidpal_story_stamp_spec, "kidpal_story_stamp_spec");
    {
        artt_stamp_spec_t sp;
        int v;

        memset(&sp, 0, sizeof(sp));
        v = kidpal_story_stamp_spec(&sp);
        TEST_ASSERT_TRUE_FM(v != KIDPAL_ART_UNIMPLEMENTED,
            "ART-A7 RED(regression): story_stamp_spec sentinel -2 — 包 8 冻结"
            "契约未链接（回归面断裂）");
        TEST_ASSERT_TRUE_FM(v == 0 && sp.rings == 2 && sp.inner_dashed == 1 &&
                                 sp.angle_deci == -80 && sp.hold_ms == 800,
            "ART-A7 RED(regression): stamp geometry drifted (rings/inner_dashed/"
            "angle_deci/hold_ms) — 包 8 盖章几何被换皮破坏");
    }
}

/* -------------------------------------------------------------------------- *
 * 附注（不在断言面，留根）：
 * - ART-E2（显式殿后 9b）：SPIFFS 真机文件源路径、1bpp 真机观感、16px 以下 CJK
 *   真机可读性（HWV-A7, ARQ-260831-small-cjk-1bpp-readability）、前光确认——本
 *   文件 host+SDL 段只测加载路径/落盘形状/渲染像素语义。
 * - ART-E3：本文件不 supersede 页面包既有验收；六页原冻结验收在同 runner 全量
 *   复跑（布局钉区域在位语义由各包原齿承载），test 11 只做跨包抽查。
 * - 蝴蝶结 7 姿态同位同形、网点/框族/印章观感、字号观感 = design review 面
 *   （acceptance-evidence.md「UI 验收 vs 设计 review」两桶纪律），不进脆断言。
 * - 契约零变更：display 统一响应块 E1 措辞不动；本包验收不要求改
 *   docs/contracts 目录（tooth 7 canary 在世守卫）。
 * --------------------------------------------------------------------------- */
