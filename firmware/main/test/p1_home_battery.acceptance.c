/*
 * Frozen acceptance tests — TaskPackage 260915-m1-n1b-p1-home-battery (subsystem:
 * firmware; M1-N1b; 首页新版式 + 电量 ADC 接线 · mergeOrder 24 · rigor=full).
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1–AC4）。设计权威 = docs/requirements.md
 * S-F1.1（WF-F1-01 v1.4 重排序：辅助条小图标前置 + 「接下来」为主体）+ arch §5.5.4。
 * 电量端到端 owner = 本包独占（crossPackageBehaviors 已登记）：板级 ADC →
 * GetBatteryLevel override → 渲染节点电池注入缝 → 辅助条显示。
 *
 * 现状差距（plan.md 背景摘录）：联网小标链路已实现（基座 UpdateStatusBar 每 10s 刷
 * GetNetworkStateIcon）；电量链路三处断点——板类 adc1_handle/cali_handle/vbat_status
 * 死成员、无 GetBatteryLevel override（board.cc 基座默认 return false）、渲染节点无
 * 电池注入缝（s_battery_pct 恒 100）。P1-A4 系注入假状态验收——本文件 PH-A3 机制齿
 * 必须能区分「真 ADC 数据到达渲染缝」vs「恒值注入」。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan) + BEHAVIORAL (pixel-probe / 决策表) frozen
 * acceptance，key_layer_dispatch.acceptance.c（K1a，同目录先例）+ 归档 home_p1.
 * acceptance.c（FROZEN OFFLINE CONTRACT 先例，260829-home-p1）双范式合体。低电闪烁
 * 机制（doze_state / blink_visible / 1s 周期）沿用归档 home_p1 契约在世实现
 *（firmware/main/display/home_p1.c），本文件把它重新编入 host runner 并钉进新版式。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证；「home =」为冻结落点）:
 *
 *   [1] 渲染节点电池注入缝（home = firmware/main/display/kidpal_render_node.{h,cc}）:
 *       void kidpal_render_node_inject_battery(int battery_pct, int charging);
 *         -- 命名与签名对齐既有 inject_rtc / inject_env 注入缝家族（.h 声明 C linkage
 *            + .cc 定义）。函数体写 s_battery_pct（钳位 0..100，越界丢弃保持原值）与
 *            s_charging（非 0 归 1）。「s_battery_pct 恒 100」销账：100 只保留在声明
 *            初始化器（保守默认值），注入函数是唯二写点。三个 hp1_state 组装点
 *           （rn_render_home_locked / rn_render_kidpal_page_locked HOME 分支 /
 *            kidpal_render_node_tick）继续消费 s_battery_pct / s_charging（现状已在读，
 *            守卫齿钉「不被拆」）。板级在 InitializeSensors 后、inject_rtc/inject_env
 *            同一装配点调用注入（LVGL 就绪后）。
 *       结构齿钉注入缝函数体（region 切区：签名行 → get_page 签名行）内含
 *         s_battery_pct 与 s_charging 写点；且 kidpal_render_node.cc 内
 *         s_battery_pct 出现次数 >= 5（现状 4 = 声明 + 三读、无注入写点 = 恒值注入
 *         在世证据）。
 *
 *   [2] 板级 ADC 接线（home = firmware/main/boards/waveshare-s3-rlcd-4.2/
 *       waveshare-s3-rlcd-4.2.cc，参考树 03_ADC_Test / Adc_GetBatteryLevel 只读重写）:
 *       - CustomBoard override GetBatteryLevel(int &level, bool &charging,
 *         bool &discharging)（board.cc 基座默认 return false 的现状销账）；
 *       - AdcBatteryMonitor（boards/common/adc_battery_monitor.h 基座）实装接入；
 *       - 三个死成员 adc1_handle / cali_handle / vbat_status 接线（各自出现次数
 *         >= 2 = 声明 + 真实使用；现状各恰 1 = 死成员在世证据）；
 *       - kidpal_render_node_inject_battery( 被板级消费（ADC 读数 → 注入缝，
 *         「真数据到达渲染缝」的接线证据；恒值注入无法满足本齿 + [1] 函数体齿）。
 *
 *   [3] 辅助条新版式（home = firmware/main/display/home_p1.c，纯逻辑渲染，WF-F1-01
 *       v1.4 重排序）：400x300 帧，①辅助条 y[0..39] 横贯条，左→右 = 时间 · 日期 ·
 *       温度(图标前置) · 湿度(图标前置，**不裸数字**) · 电量(图标前置)——联网小标
 *       打头由基座 chrome 状态栏承载（LvglDisplay::UpdateStatusBar 消费
 *       GetNetworkStateIcon，链路已实现，本包只钉点 = test 6），不进 home_p1 帧。
 *       ②日程主体：小伴常驻演出（左）+ 「接下来」最近一项最醒目（右）+ 倒计时贴纸；
 *       ③后续事项逐项（小一号）+ 明日预告。布局钉区域矩形（含端点，builder 须把对应
 *       元素画进对应区域；钉区是布局在位钉，不是字体/颜色钉——纯美学留 design review）：
 *
 *         HPB_RECT_AUX_STRIP  {  0,  0, 399,  39 }  辅助条横贯带
 *         HPB_RECT_AUX_TIME   { 36,  6, 108,  33 }  时间 "10:24"（小字）
 *         HPB_RECT_AUX_DATE   {112,  6, 208,  33 }  日期星期
 *         HPB_RECT_AUX_TEMP   {212,  6, 264,  33 }  温度（整元素）
 *           HPB_RECT_AUX_TEMP_ICON {212,  6, 232,  33}   图标半（前置）
 *           HPB_RECT_AUX_TEMP_VAL  {236,  6, 264,  33}   数值半
 *         HPB_RECT_AUX_HUM    {268,  6, 318,  33 }  湿度（整元素）
 *           HPB_RECT_AUX_HUM_ICON  {268,  6, 288,  33}   图标半（"45%" 不裸数字）
 *           HPB_RECT_AUX_HUM_VAL   {292,  6, 318,  33}   数值半
 *         HPB_RECT_AUX_BATT   {322,  6, 396,  33 }  电量（整元素）
 *           HPB_RECT_AUX_BATT_ICON {322,  6, 344,  33}   图标半（前置）
 *           HPB_RECT_AUX_BATT_VAL  {348,  6, 396,  33}   数值半
 *         HPB_RECT_MASCOT     {  8, 48, 128, 208 }  小伴常驻演出（v1.4 内容区左）
 *         HPB_RECT_NEXT       {136, 48, 392, 168 }  「接下来」最近一项（主体）
 *         HPB_RECT_STICKER    {268, 50, 390, 130 }  倒计时贴纸（接下来卡右部）
 *         HPB_RECT_NEXTLIST   {136,172, 392, 214 }  ③后续事项逐项（小一号）
 *         HPB_RECT_PREVIEW    {136,220, 392, 258 }  明日预告
 *       兜底义务（host 全量默认跑不挂 art 资产）：每个「有墨」钉区必须在 art 未挂载
 *       的兜底路径下也有墨（描边/兜底字形同款纪律，归档 home_p1 先例）；每个「零墨」
 *       钉区在两条路径下都必须严格零墨。温湿度缺席（env_valid=0）= 整元素隐藏、
 *       不画假数据（归档契约语义沿用）。
 *
 *   [4] 低电闪烁（PH-A4 / AC3，归档 home_p1 契约沿用）：doze = battery_pct <=
 *       HP1_DOZE_BATTERY_THRESHOLD_PCT(15) 且 charging==0；充电恒醒（覆盖任何电量）。
 *       doze 灭相窗（HP1_BLINK_PERIOD_BATTERY_MS=1000，可见窗 3/4 周期）内辅助条电池
 *       **整元素**（图标 + 数值）不画 = 区内严格零墨，亮相恢复；充电即停闪恒画。
 *       **已砍项零触碰**：低电不占小伴姿态通道——home_p1.c / kidpal_render_node.cc
 *       零 pose_sleep 词（§8 已砍清单）。
 *
 *   [5] 范围纪律（PH-E1）：首页日程数据口径零改动——home_p1.h 六个 schedule API
 *       （schedule_inject/count/item/preview + day_rollover + countdown）签名面在世，
 *       本包只动版式与辅助条，不触数据链路（课表数据接首页归 M3 决策 26）。需求/契约
 *       面零修订 canary。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: PH-A1 首页版式（接下来主体 + 倒计时贴纸 + 小伴常驻 + v1.4 重排序）
 *             -> test 1（pixel-probe: 主体/贴纸/小伴/后续/预告区在位 + 空日程让位 +
 *             旧版式大时间让位判别——v1.4 重排序落齿）
 *   coverage: PH-A2 辅助条小图标前置 -> test 2（pixel-probe: 五元素区 + 温/湿/电
 *             图标半/数值半两两在位 = 图标前置、湿度不裸数字；env 缺席隐藏）+
 *             test 3（结构半: 辅助条渲染消费 hp1_state 电量字段）
 *   coverage: PH-A3 电量 ADC 接线机制齿 -> test 1（行为半: 注入假状态驱动渲染缝——
 *             恒值注入 100 无法满足低电灭相零墨齿）+ test 3（结构半: 注入缝 [1] +
 *             板级接线 [2]，恒值注入现状必红）
 *   coverage: PH-A4 低电闪烁 -> test 1（行为半: ≤15% 未充电每秒闪 + 充电即停）+
 *             test 2（决策表: doze 阈值相对行为 + 1s 闪相位 + 睡姿演出零触碰守卫）
 *   coverage: PH-A5 断网态钉点 -> test 4（WiFi 斜杠链路在世钉点 + chrome 消费守卫 +
 *             首页本地渲染零网络依赖）
 *   coverage: PH-A6 真机确认轮 -> 不在本文件（kind:structural 对象条目，human 持板
 *             ADC 实测；证据落 delivery/changes/dev/260915-m1-n1b-p1-home-battery/）
 *   coverage: PH-A7 host runner 终跑 -> test 5（注册/接线结构守卫）+ 本文件尾
 *             「PH-A7 终跑判读」段（suite 级判读：相对有效基线零新增红 + 本包新齿全绿）
 *   coverage: PH-E1 范围纪律 -> test 5 tooth 2/3（schedule API 面零触碰 + 需求契约
 *             canary）
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) firmware/main/display/home_p1.c 按 [3] 冻结区域重排版式（辅助条图标前置 +
 *      「接下来」主体 + 兜底义务 + [4] doze 整元素闪 gate）。
 *   2) firmware/main/display/kidpal_render_node.{h,cc} 落 [1] 注入缝（声明 + 定义 +
 *      三组装点消费保持；定义置于 get_page 之前——结构齿切区锚）。
 *   3) firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc 落 [2]
 *      板级接线（AdcBatteryMonitor + GetBatteryLevel override + 死成员接线 +
 *      inject_battery 装配点调用）。
 *   4) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../p1_home_battery.acceptance.c" 与
 *      "../../../display/home_p1.c"（行为齿强符号覆盖 weak 哨兵的关键），置于
 *      "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行）。
 *      art_assets.c 可选接线（挂资产后图标齿探真资产像素；不接则走兜底路径，钉区
 *      有墨义务同样成立）。
 *   5) mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *      子串**（KR-A3 _TOGGLE_X 教训——strstr 子串仍命中则变异不红；用 _STUB 类整词
 *      替换），mutationEvidence.perAcceptance 逐条回填。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *      （git-bash 下 MSYS_NO_PATHCONV=1 必加；idf.py 无 -T 选项）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *      layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

/* home_p1 冻结契约头（归档 home_p1.acceptance.c 的在世实现面；形状/常量零偏移）。 */
#include "../display/home_p1.h"
/* art 资产契约头（仅为 weak 哨兵签名一致；被测面 = home_p1.c 消费的 5 原语）。 */
#include "../display/art_assets.h"

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes / regions（seam [3]；帧规格 == home_p1.c P1_FB_*，不得偏移）
 * ------------------------------------------------------------------------ */
#define HPB_UNIMPLEMENTED (-2) /* weak 哨兵 — 永不是合法返回值 */

#define HPB_FRAME_W 400
#define HPB_FRAME_H 300
#define HPB_INK_WHITE 0xFFFFFFul
#define HPB_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 「有墨」钉区最小墨像素数；整元素区 >= 8、图标/数值半区 >= 4、大区 >= 20。 */
#define HPB_INK_MIN_ELEM 8
#define HPB_INK_MIN_HALF 4
#define HPB_INK_MIN_LARGE 20

/* 区域矩形 {x1, y1, x2, y2} 含端点（seam [3] 布局在位钉）。 */
static const int hpb_rect_aux_strip[4]    = {  0,  0, 399,  39 };
static const int hpb_rect_aux_time[4]     = { 36,  6, 108,  33 };
static const int hpb_rect_aux_date[4]     = {112,  6, 208,  33 };
static const int hpb_rect_aux_temp[4]     = {212,  6, 264,  33 };
static const int hpb_rect_aux_temp_icon[4]= {212,  6, 232,  33 };
static const int hpb_rect_aux_temp_val[4] = {236,  6, 264,  33 };
static const int hpb_rect_aux_hum[4]      = {268,  6, 318,  33 };
static const int hpb_rect_aux_hum_icon[4] = {268,  6, 288,  33 };
static const int hpb_rect_aux_hum_val[4]  = {292,  6, 318,  33 };
static const int hpb_rect_aux_batt[4]     = {322,  6, 396,  33 };
static const int hpb_rect_aux_batt_icon[4]= {322,  6, 344,  33 };
static const int hpb_rect_aux_batt_val[4] = {348,  6, 396,  33 };
static const int hpb_rect_mascot[4]       = {  8, 48, 128, 208 };
static const int hpb_rect_next[4]         = {136, 48, 392, 168 };
static const int hpb_rect_sticker[4]      = {268, 50, 390, 130 };
static const int hpb_rect_nextlist[4]     = {136,172, 392, 214 };
static const int hpb_rect_preview[4]      = {136,220, 392, 258 };

/* 板级装配文件（seam [2] home）与渲染节点/显示链被扫文件。 */
#define HPB_BOARD_CC   "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define HPB_RN_H       "firmware/main/display/kidpal_render_node.h"
#define HPB_RN_CC      "firmware/main/display/kidpal_render_node.cc"
#define HPB_HOME_P1_C  "firmware/main/display/home_p1.c"
#define HPB_HOME_P1_H  "firmware/main/display/home_p1.h"
#define HPB_WIFI_CC    "firmware/main/boards/common/wifi_board.cc"
#define HPB_LVGL_CC    "firmware/main/display/lvgl_display/lvgl_display.cc"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * home_p1.c（归档契约实现，在世）由 builder 按第 4) 条接线进 host runner 后，
 * 其强定义覆盖以下哨兵；接线前哨兵返回 HPB_UNIMPLEMENTED，红基线读作
 * 「frozen contract not implemented（host 接线缺失）」——真实缺失行为红，绝非链接
 * 错误。art 原语哨兵使 home_p1.c 单独接线也能链（未接 art_assets.c 时走兜底路径）。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_home_p1_schedule_inject(const char *day_key,
                                                         const hp1_item_t *items,
                                                         int count,
                                                         const hp1_item_t *next_day_preview)
{
    (void)day_key; (void)items; (void)count; (void)next_day_preview;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_schedule_count(void)
{
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) const hp1_item_t *kidpal_home_p1_schedule_item(int index)
{
    (void)index;
    return NULL;
}
__attribute__((weak)) int kidpal_home_p1_schedule_preview(hp1_item_t *out)
{
    (void)out;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_countdown(const hp1_item_t *items, int count,
                                                   int now_h, int now_m,
                                                   int *out_index, int *out_hours,
                                                   int *out_minutes)
{
    (void)items; (void)count; (void)now_h; (void)now_m;
    (void)out_index; (void)out_hours; (void)out_minutes;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_blink_visible(int tick_ms, int period_ms)
{
    (void)tick_ms; (void)period_ms;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_doze_state(int battery_pct, int charging)
{
    (void)battery_pct; (void)charging;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_set_state(const hp1_state_t *st)
{
    (void)st;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_render_frame(void)
{
    return HPB_UNIMPLEMENTED;
}

/* art 原语哨兵（签名 == art_assets.h；未接 art_assets.c 时 home_p1 兜底路径生效）。 */
__attribute__((weak)) int kidpal_art_mount(const char *root)
{
    (void)root;
    return -1;
}
__attribute__((weak)) int kidpal_art_load(const char *path, kidpal_art_img_t *out)
{
    (void)path; (void)out;
    return -1;
}
__attribute__((weak)) int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y)
{
    (void)img; (void)x; (void)y;
    return 0;
}
__attribute__((weak)) int kidpal_art_glyph(char ch, kidpal_art_img_t *out)
{
    (void)ch; (void)out;
    return -1;
}
__attribute__((weak)) int kidpal_art_glyph_advance(void)
{
    return 0;
}

/* host 渲染缝（host_app/main/host_infra_seam.c 提供，恒链入 runner）。只探针。 */
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，K1a 先例）。全部扫描跑在真实工作树上，
 * root 以手持 cwd 定位（仓根或 firmware/main/test/host_app 均可）。test / host_app /
 * build / .git / assets 目录排除——test 目录住着本冻结文件自身（其注释合法提及 needle）。
 * ------------------------------------------------------------------------ */
#define HPB_MAX_PATH 1024
#define HPB_MAX_READ (8u * 1024u * 1024u)

static char hpb_root[HPB_MAX_PATH];
static int hpb_root_ready;

static int hpb_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int hpb_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int hpb_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int hpb_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[HPB_MAX_PATH];

    if (hpb_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (hpb_is_regular(probe)) {
            snprintf(hpb_root, sizeof(hpb_root), "%s", cands[i]);
            hpb_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int hpb_rel(char *out, size_t cap, const char *rel)
{
    if (!hpb_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", hpb_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", hpb_root, rel);
    return 1;
}

static int hpb_exists(const char *rel)
{
    char p[HPB_MAX_PATH];
    return hpb_rel(p, sizeof(p), rel) && hpb_is_regular(p);
}

static char *hpb_read(const char *rel, size_t *out_len)
{
    char p[HPB_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!hpb_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > HPB_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int hpb_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int hpb_file_contains(const char *rel, const char *needle)
{
    char *buf = hpb_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = hpb_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int hpb_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = hpb_read(rel, NULL);
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

static int hpb_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[HPB_MAX_PATH];
        char rel_next[HPB_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            hpb_is_skipped_dir(e->d_name)) {
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
            count += hpb_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > HPB_MAX_READ) {
                continue;
            }
            {
                char *buf = hpb_read(rel_next, NULL);
                if (buf != NULL) {
                    if (hpb_contains(buf, needle)) {
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

static int hpb_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[HPB_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!hpb_rel(abs, sizeof(abs), rel_dir) || !hpb_is_dir(abs)) {
        return 0;
    }
    return hpb_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent). Caller frees. */
static char *hpb_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = hpb_read(rel, NULL);
    char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int hpb_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

#define HPB_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(hpb_locate_root(), \
    "HPB RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented/wired yet. */
static void hpb_require_implemented(int value, const char *what)
{
    if (value == HPB_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "HPB RED: frozen seam NOT implemented/wired — %s "
                 "(builder must wire firmware/main/display/home_p1.c and this file into "
                 "HOST_ACCEPTANCE_SRCS and land the seam per the contract header in "
                 "firmware/main/test/p1_home_battery.acceptance.c)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（K1a 先例）。 */
static char hpb_fm_buf[1024];
static const char *hpb_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *hpb_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(hpb_fm_buf, sizeof(hpb_fm_buf), fmt, ap);
    va_end(ap);
    return hpb_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Pixel-probe helper: count ink (non-white, non-sentinel) pixels inside a frozen
 * region. Returns -1 on probe failure (not rendered / seam broken) so callers can
 * fail loudly instead of reading a missing frame as "zero ink".
 * ------------------------------------------------------------------------ */
static int hpb_count_ink(const int rect[4])
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == HPB_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            if ((uint32_t)px != HPB_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

/* Fixture: full-state inject（时间/温湿度/电量/充电/tick 全注入，P1-E2 硬件殿后纪律）。 */
static void hpb_set_fixture(int hour, int minute, int env_valid, int temp_c,
                            int humidity_pct, int battery_pct, int charging,
                            int tick_ms)
{
    hp1_state_t st;
    int rc;

    memset(&st, 0, sizeof(st));
    st.hour = hour;
    st.minute = minute;
    st.env_valid = env_valid;
    st.temp_c = temp_c;
    st.humidity_pct = humidity_pct;
    st.battery_pct = battery_pct;
    st.charging = charging;
    st.tick_ms = tick_ms;
    rc = kidpal_home_p1_set_state(&st);
    hpb_require_implemented(rc, "kidpal_home_p1_set_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): set_state fixture rejected (rc=%d)——校验门把合法夹具"
               "当非法，渲染面无法驱动", rc));
}

/* Fixture: schedule inject（2 项今日日程 + 次日预告，producer 分组结果 verbatim）。 */
static void hpb_inject_schedule_two_items(void)
{
    hp1_item_t items[2];
    hp1_item_t preview;
    int rc;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    memset(&preview, 0, sizeof(preview));
    snprintf(preview.time, sizeof(preview.time), "09:00");
    snprintf(preview.name, sizeof(preview.name), "游泳课");

    rc = kidpal_home_p1_schedule_inject("2026-09-15", items, 2, &preview);
    hpb_require_implemented(rc, "kidpal_home_p1_schedule_inject unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject fixture rejected (rc=%d)", rc));
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — PH-A1/PH-A3/PH-A4 行为半：注入缝驱动渲染 + 新版式主体在位
 * 「接下来」主体 + 倒计时贴纸 + 小伴常驻 + 后续/预告 + 旧版式大时间让位判别（PH-A1）；
 * 电池注入缝假状态驱动渲染缝（PH-A3 行为半——恒值注入 100 无法满足低电灭相零墨齿，
 * P1-A4 注入假状态教训落齿）；低电 ≤15% 未充电每秒闪 + 充电即停（PH-A4 行为半）。
 * RED today: home_p1.c 未接进 host runner（weak 哨兵 -2）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A1 -- 新版式主体在位：接下来卡+倒计时贴纸+小伴常驻+后续/预告区在位；空日程让位 + 旧版式大时间让位判别（v1.4 重排序落齿） */
/* coverage: PH-A3 -- 电池注入缝行为半：注入假状态驱动渲染缝——低电灭相窗零墨齿，恒值注入(100)必红 */
/* coverage: PH-A4 -- 低电闪烁行为半：≤15% 未充电 1s 闪 + 充电即停恒画 */
TEST_CASE("首页电量: 注入缝驱动渲染+新版式主体+低电闪停 (PH-A1/PH-A3/PH-A4)",
          "[p1_home_battery][PH-A1][PH-A3][PH-A4][behavior]")
{
    int ink;

    /* ---- fixture A: 有日程 + 电量 85（PH-A1 主体在位） ---------------------- */
    hpb_inject_schedule_two_items();
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 0);
    {
        int rc = kidpal_home_p1_render_frame();
        hpb_require_implemented(rc, "kidpal_home_p1_render_frame unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame rc=%d——P1 帧渲染失败", rc));
    }

    /* tooth 1 (PH-A1): 「接下来」最近一项主体区在位（最醒目）。 */
    ink = hpb_count_ink(hpb_rect_next);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_LARGE,
        hpb_fm("HPB-A1 RED: 接下来主体区墨 %d < %d——最近一项主体未落新版式钉区 "
               "(WF-F1-01 v1.4 ②)", ink, HPB_INK_MIN_LARGE));

    /* tooth 2 (PH-A1): 倒计时贴纸区在位（最近一项带倒计时贴纸）。 */
    ink = hpb_count_ink(hpb_rect_sticker);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A1 RED: 倒计时贴纸区墨 %d < %d——贴纸存在性缺失 (AC1)", ink,
               HPB_INK_MIN_HALF));

    /* tooth 3 (PH-A1): 小伴常驻演出区在位（姿态/表情保留，非冷冰冰任务清单）。 */
    ink = hpb_count_ink(hpb_rect_mascot);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_LARGE,
        hpb_fm("HPB-A1 RED: 小伴常驻演出区墨 %d < %d——常驻演出被新版式挤掉 (AC1)",
               ink, HPB_INK_MIN_LARGE));

    /* tooth 4 (PH-A1): 后续事项区 + 明日预告区在位。 */
    ink = hpb_count_ink(hpb_rect_nextlist);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A1 RED: 后续事项区墨 %d < %d——逐项列表缺失 (AC1 ③)", ink,
               HPB_INK_MIN_ELEM));
    ink = hpb_count_ink(hpb_rect_preview);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A1 RED: 明日预告区墨 %d < %d——预告缺失 (AC1)", ink,
               HPB_INK_MIN_ELEM));

    /* tooth 5 (PH-A1, v1.4 重排序判别齿): 空日程时主体区零墨——旧版式大时间
     * 兜底描边（x<=182,y<=92）越界压进主体钉区（x>=136），本齿对旧版式必红；
     * 新版式下时间已让位辅助条、主体区随空日程严格让位。 */
    {
        int rc;
        rc = kidpal_home_p1_schedule_inject("2026-09-15", NULL, 0, NULL);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(count=0) rc=%d——「今日无待办」"
                   "清空路径回归", rc));
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(empty) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_next);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HPB-A1 RED: 空日程时主体区墨 %d != 0——无待办未让位，或旧版式大时间"
               "仍压在内容区（v1.4 重排序未落地）", ink));

    /* ---- fixture B: PH-A3 行为半 + PH-A4 行为半（电池注入缝驱动渲染缝） ---- */
    /* tooth 6 (PH-A4): 低电 10% 未充电，灭相窗内（tick=900，周期 1000 可见窗 3/4）
     * 电池整元素（图标+数值）零墨——「显示 100」恒值注入现状对本齿必红
     * (P1-A4 注入假状态教训: 断言必须能区分真数据到达渲染缝 vs 恒值注入)。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 0, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(doze off-phase) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HPB-A4 RED: 低电灭相窗电池区墨 %d != 0——doze 闪烁未门控渲染，或"
               "电量恒值注入(100)未销账 (AC3)", ink));

    /* tooth 7 (PH-A4): 同状态亮相窗（tick=0）电池区在位——闪 = 相位门控而非消失。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 0, 0);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(doze on-phase) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 低电亮相窗电池区墨 %d < %d——电池元素丢失（闪变成熄灭）",
               ink, HPB_INK_MIN_ELEM));

    /* tooth 8 (PH-A4): 充电即停闪——低电 + charging=1 + 灭相窗 tick 仍恒画 (AC3)。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 1, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(charging) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 充电态灭相窗电池区墨 %d < %d——充电「叫醒」未覆盖 "
               "doze 灭相 (AC3)", ink, HPB_INK_MIN_ELEM));

    /* tooth 9 (PH-A4): 非低电恒画——85% + 灭相窗 tick 不闪。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(normal batt) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 非低电电池区墨 %d < %d——doze 误伤正常电量显示", ink,
               HPB_INK_MIN_ELEM));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — PH-A4 决策表 + 1s 闪相位 + 已砍项零触碰守卫
 * doze_state 阈值相对行为（≤15 触发、充电恒醒、防御 -1）+ blink_visible 1s 周期
 * 相位窗 + pose_sleep 零命中（低电不占姿态通道，§8 已砍）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A4 -- 低电打盹决策表：阈值相对行为+充电叫醒+防御；1s 闪相位窗；pose_sleep 零触碰守卫 */
TEST_CASE("首页电量: 低电决策表+闪相位+睡姿零触碰 (PH-A4)",
          "[p1_home_battery][PH-A4][decision]")
{
    int d;

    d = kidpal_home_p1_doze_state(10, 0);
    hpb_require_implemented(d, "kidpal_home_p1_doze_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(d == 1,
        hpb_fm("HPB-A4 RED: doze_state(10,0)=%d 期望 1——电量低于阈值未触发低电态 "
               "(AC3)", d));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT, 0) == 1,
        "HPB-A4 RED: doze_state(15,0)!=1——阈值边界（<= 触发）漂移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT + 1, 0) == 0,
        "HPB-A4 RED: doze_state(16,0)!=0——阈值上方误触发");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(100, 0) == 0,
        "HPB-A4 RED: doze_state(100,0)!=0——满电误触发");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(0, 1) == 0 &&
                                 kidpal_home_p1_doze_state(100, 1) == 0,
        "HPB-A4 RED: 充电态 doze!=0——充电「叫醒」未覆盖任何电量 (AC3)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(-1, 0) == -1 &&
                                 kidpal_home_p1_doze_state(101, 0) == -1 &&
                                 kidpal_home_p1_doze_state(50, 2) == -1,
        "HPB-A4 RED: 非法入参未防御（-1）——电量/充电越界值被静默接受");

    /* 1s 闪相位窗（与冒号眨眼共用相位函数；可见窗 = 周期 3/4）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(0, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
                                 kidpal_home_p1_blink_visible(749, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
                                 kidpal_home_p1_blink_visible(750, HP1_BLINK_PERIOD_BATTERY_MS) == 0 &&
                                 kidpal_home_p1_blink_visible(999, HP1_BLINK_PERIOD_BATTERY_MS) == 0 &&
                                 kidpal_home_p1_blink_visible(1000, HP1_BLINK_PERIOD_BATTERY_MS) == 1,
        "HPB-A4 RED: blink_visible 1s 周期相位窗漂移——每秒闪的灭窗不在 [750,1000)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(5, 0) == 1 &&
                                 kidpal_home_p1_blink_visible(-1, 1000) == 1,
        "HPB-A4 RED: blink_visible 防御语义漂移（period<=0 / tick<0 须恒可见）");

    /* 已砍项零触碰守卫：低电不占小伴姿态通道（§8 已砍清单）。 */
    HPB_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_file(HPB_HOME_P1_C, "pose_sleep") == 0 &&
                                 hpb_count_in_file(HPB_RN_CC, "pose_sleep") == 0,
        "HPB-A4 RED: 首页渲染面出现 pose_sleep——低电打盹演出被复活（§8 已砍项"
        "零触碰红线：低电只保留辅助条电池图标每秒闪最小信号）");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — PH-A2/PH-A3 结构半：辅助条图标前置 pixel-probe + 注入缝/板级接线 tree-scan
 * pixel 半（fixture: env 有效 + 电量 85）：辅助条五元素区 + 温/湿/电图标半与数值半
 * 两两在位（图标前置 = 「湿度 45% 不裸数字」的结构判别）+ env 缺席隐藏；tree 半：
 * 注入缝 [1]（.h 声明 + .cc 函数体写点 + 三文件消费链 + s_battery_pct >= 5）+
 * 板级接线 [2]（AdcBatteryMonitor + GetBatteryLevel override + 三死成员各 >= 2）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A2 -- 辅助条图标前置：五元素区在位 + 温/湿/电图标半+数值半两两在位（湿度不裸数字）+ env 缺席隐藏 */
/* coverage: PH-A3 -- 电量 ADC 接线结构齿：注入缝符号级（.h+.cc 函数体写点+三文件消费链+s_battery_pct>=5）+ 板级 override+死成员接线 */
TEST_CASE("首页电量: 辅助条图标前置+ADC接线结构 (PH-A2/PH-A3)",
          "[p1_home_battery][PH-A2][PH-A3][structural]")
{
    char hit[HPB_MAX_PATH];
    char *region;
    int n, ink, rc;

    /* ---- pixel 半: 辅助条五元素 + 图标半/数值半 ----------------------------- */
    hpb_inject_schedule_two_items();
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 0);
    rc = kidpal_home_p1_render_frame();
    hpb_require_implemented(rc, "kidpal_home_p1_render_frame unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame rc=%d", rc));

    /* tooth 0: 辅助条横贯带整体有墨（y[0..39] 版式带存在）。 */
    ink = hpb_count_ink(hpb_rect_aux_strip);
    TEST_ASSERT_TRUE_MESSAGE(ink >= 30,
        hpb_fm("HPB-A2 RED: 辅助条横贯带墨 %d < 30——y[0..39] 版式带缺失 (v1.4 ①)",
               ink));

    /* tooth 1: 时间/日期区在位（辅助条左段，v1.4 重排序落位）。 */
    ink = hpb_count_ink(hpb_rect_aux_time);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A2 RED: 辅助条时间区墨 %d < %d——时间未收进辅助条 (v1.4 ①)",
               ink, HPB_INK_MIN_ELEM));
    ink = hpb_count_ink(hpb_rect_aux_date);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A2 RED: 辅助条日期区墨 %d < %d——日期未收进辅助条", ink,
               HPB_INK_MIN_ELEM));

    /* tooth 2: 温度元素 = 图标半 + 数值半两两在位（图标前置）。 */
    ink = hpb_count_ink(hpb_rect_aux_temp_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条温度图标半区墨 %d < %d——温度缺前置小图标", ink,
               HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_temp_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条温度数值半区墨 %d < %d——温度数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 3 (AC1 湿度不裸数字): 湿度元素 = 图标半 + 数值半两两在位。 */
    ink = hpb_count_ink(hpb_rect_aux_hum_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条湿度图标半区墨 %d < %d——湿度 45%% 裸数字挨电池"
               "（缺前置小图标）", ink, HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_hum_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条湿度数值半区墨 %d < %d——湿度数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 4: 电量元素 = 图标半 + 数值半两两在位。 */
    ink = hpb_count_ink(hpb_rect_aux_batt_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条电池图标半区墨 %d < %d——电量缺前置小图标", ink,
               HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_batt_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条电池数值半区墨 %d < %d——电量数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 5: env 缺席（env_valid=0）= 温/湿整元素隐藏、不画假数据；时间/日期/
     * 电量照常（断网/缺传感时本地数据照常的渲染半，服务 PH-A5）。 */
    hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(env absent) rc=%d", rc));
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_ink(hpb_rect_aux_temp) == 0 &&
                                 hpb_count_ink(hpb_rect_aux_hum) == 0,
        "HPB-A2 RED: env_valid=0 时温/湿区仍有墨——缺席未隐藏或画了假数据"
        "（归档 home_p1 契约: 缺席即隐藏）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_ink(hpb_rect_aux_time) >= HPB_INK_MIN_ELEM &&
                                 hpb_count_ink(hpb_rect_aux_batt) >= HPB_INK_MIN_ELEM,
        "HPB-A2 RED: env 缺席时时间/电量区失墨——本地数据未照常显示");

    /* ---- tree 半: 注入缝 [1] + 板级接线 [2] -------------------------------- */
    HPB_REQUIRE_ROOT();

    /* tooth 6: 注入缝声明落 .h（消费面 C linkage，inject_rtc/inject_env 家族同款）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_RN_H,
                                               "kidpal_render_node_inject_battery"),
        "HPB-A3 RED: kidpal_render_node.h 未声明 kidpal_render_node_inject_battery——"
        "渲染节点电池注入缝缺失（s_battery_pct 恒 100 现状在世）");

    /* tooth 7: 注入缝函数体写 s_battery_pct / s_charging（真写点，非空壳）。
     * 冻结落点：定义置于 get_page 之前（结构齿切区锚，BUILDER DUTY 2))。 */
    region = hpb_region_between(HPB_RN_CC,
                                "void kidpal_render_node_inject_battery(",
                                "int kidpal_render_node_get_page(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "HPB-A3 RED: kidpal_render_node.cc 的 inject_battery 函数区切取失败——"
        "定义缺失，或未置于 get_page 之前（冻结落点见 seam [1]）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_slice(region, "s_battery_pct") >= 1 &&
                                 hpb_count_in_slice(region, "s_charging") >= 1,
        "HPB-A3 RED: inject_battery 函数体未写 s_battery_pct/s_charging——注入缝"
        "空壳，电池数据到不了渲染状态（恒值注入在世）");
    free(region);

    /* tooth 8: 注入缝三文件消费链（.h 声明 + .cc 定义 + 板级装配点调用）>= 3 文件。 */
    n = hpb_scan_count("firmware/main", "kidpal_render_node_inject_battery(", hit,
                       sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        hpb_fm("HPB-A3 RED: kidpal_render_node_inject_battery( 仅 %d 文件引用"
               "（期望 >= 3: 声明 + 定义 + 板级调用）——ADC 读数到渲染缝的接线链断裂",
               n));

    /* tooth 9: 板级 override + AdcBatteryMonitor 实装（基座默认 false 销账）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_BOARD_CC, "GetBatteryLevel") &&
                                 hpb_file_contains(HPB_BOARD_CC, "AdcBatteryMonitor"),
        "HPB-A3 RED: 板类无 GetBatteryLevel override / AdcBatteryMonitor 实装——"
        "board.cc 基座默认 return false 的现状未销账，真机电量仍取不到");

    /* tooth 10: 三死成员接线（各出现 >= 2 = 声明 + 真实使用；现状各恰 1 = 死成员）。 */
    n = hpb_count_in_file(HPB_BOARD_CC, "adc1_handle");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 adc1_handle 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));
    n = hpb_count_in_file(HPB_BOARD_CC, "cali_handle");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 cali_handle 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));
    n = hpb_count_in_file(HPB_BOARD_CC, "vbat_status");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 vbat_status 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));

    /* tooth 11 (guard): s_battery_pct 出现次数 >= 5（声明 + 注入写点 + 三组装点读；
     * 现状 4 = 恒值注入在世证据）。组装点读被拆 = 渲染面拿不到电池数据。 */
    n = hpb_count_in_file(HPB_RN_CC, "s_battery_pct");
    TEST_ASSERT_TRUE_MESSAGE(n >= 5,
        hpb_fm("HPB-A3 RED: kidpal_render_node.cc s_battery_pct 仅 %d 处（期望 "
               ">= 5: 声明+注入写点+三组装点读）——注入写点缺失（恒值注入）或组装点"
               "读被拆", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — PH-A5 断网态钉点（链路已实现，本包只钉点）
 * WiFi 斜杠链路在世钉点（wifi_board.cc GetNetworkStateIcon 断网分支）+ chrome 消费
 * 守卫（lvgl_display.cc UpdateStatusBar 每 10s 刷）+ 首页本地渲染零网络依赖
 * （断网时时间/温湿度/电量照常本地显示的渲染半）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A5 -- 断网态钉点：GetNetworkStateIcon 断网 WiFi-slash 分支在世 + UpdateStatusBar 消费守卫 + 首页本地渲染零网络依赖 */
TEST_CASE("断网态: WiFi斜杠钉点+本地数据照常 (PH-A5)",
          "[p1_home_battery][PH-A5][structural]")
{
    char *region;

    HPB_REQUIRE_ROOT();

    /* tooth 1: 断网分支在世——GetNetworkStateIcon 函数区内含 !IsConnected ->
     * FONT_AWESOME_WIFI_SLASH（AC4 联网小标变断网态的链路事实源）。 */
    region = hpb_region_between(HPB_WIFI_CC,
                                "const char* WifiBoard::GetNetworkStateIcon()",
                                "std::string WifiBoard::GetBoardJson");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "HPB-A5 RED(infra): wifi_board.cc 的 GetNetworkStateIcon 函数区切取失败"
        "（签名锚漂移——链路被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_contains(region, "FONT_AWESOME_WIFI_SLASH") &&
                                 hpb_contains(region, "IsConnected"),
        "HPB-A5 RED: GetNetworkStateIcon 断网分支（!IsConnected -> WiFi_SLASH）"
        "丢失——断网时联网小标不再变 WiFi 斜杠 (AC4)");
    free(region);

    /* tooth 2 (guard): chrome 消费守卫——lvgl_display.cc UpdateStatusBar 消费
     * GetNetworkStateIcon（基座每 10s 刷联网小标；「联网小标打头」的承载位）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_LVGL_CC, "GetNetworkStateIcon"),
        "HPB-A5 RED: lvgl_display.cc 不再消费 GetNetworkStateIcon——联网小标刷新链路"
        "被拆（PH-A5 只钉点、零触碰红线）");

    /* tooth 3: 首页本地渲染零网络依赖——home_p1.c 无网络词（断网时时间/温湿度/
     * 电量照常本地显示、日程区不更新的渲染半；网络态由 chrome 层承载）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_file(HPB_HOME_P1_C, "GetNetworkStateIcon") == 0 &&
                                 hpb_count_in_file(HPB_HOME_P1_C, "websocket") == 0 &&
                                 hpb_count_in_file(HPB_HOME_P1_C, "esp_http") == 0,
        "HPB-A5 RED: home_p1.c 出现网络依赖词——首页本地渲染被网络门控（断网时"
        "本地数据必须照常显示，NFR-OFF-1 / AC4）");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — PH-A7/PH-E1 终跑守卫：注册接线 + 数据口径零触碰 + 需求契约 canary +
 * 扫描器自检
 *   PH-A7 结构半（本验收文件 + home_p1.c 是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   PH-E1 范围纪律（schedule API 面零触碰 + 需求/契约面零修订 canary）
 *   扫描器自检（结构齿负向必须能红，防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: PH-A7 -- host runner 终跑：本文件+home_p1.c 注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚 */
/* coverage: PH-E1 -- 范围纪律守卫：schedule API 面零触碰（数据口径零改动）+ 需求/契约面零修订 canary */
TEST_CASE("终跑守卫: 注册接线+数据口径零触碰+需求契约canary+扫描器自检 (PH-A7/PH-E1)",
          "[p1_home_battery][PH-A7][PH-E1][boundary]")
{
    char *mk;
    char hit[HPB_MAX_PATH];
    int n;

    HPB_REQUIRE_ROOT();

    /* tooth 1: 本文件 + home_p1.c 接线进 HOST_ACCEPTANCE_SRCS 且排在 smoke 之前
     *（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = hpb_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "HPB RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "p1_home_battery.acceptance.c");
        const char *impl = strstr(mk, "../../../display/home_p1.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "PH-A7 RED: p1_home_battery.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— host 验收构建未接本包（BUILDER DUTY 4))");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "PH-A7 RED: firmware/main/display/home_p1.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— 行为齿强符号无处落地（weak 哨兵将持续红）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "HPB RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失 — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "PH-A7 RED: p1_home_battery.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (PH-E1 guard): 日程数据口径零改动——home_p1.h 六个 schedule API
     * 声明面在世（inject/count/item/preview/day_rollover/countdown），home_p1.c
     * 实现在世。本包只动版式与辅助条，不触数据链路（课表接首页归 M3 决策 26）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_HOME_P1_H,
                                               "kidpal_home_p1_schedule_inject") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_count") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_item") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_preview") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_day_rollover") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_countdown"),
        "PH-E1 RED: home_p1.h schedule API 声明面缺失——日程数据口径被本包触碰"
        "（PH-E1 零改动红线：数据链路归 M3 决策 26）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_HOME_P1_C,
                                               "kidpal_home_p1_schedule_inject"),
        "PH-E1 RED: home_p1.c schedule_inject 实现缺失——日程数据链路被拆");

    /* tooth 3 (PH-E1 canary): 需求面在世且 S-F1.1 锚在世 + 契约面在世（零修订）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists("docs/requirements.md"),
        "PH-E1 RED(infra): docs/requirements.md 丢失——需求面被触碰");
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains("docs/requirements.md", "S-F1.1"),
        "PH-E1 RED: requirements S-F1.1 锚丢失——需求面被修订（发现冲突走 spec-lane "
        "逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists("docs/contracts/interface-contract.md"),
        "HPB RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists(HPB_RN_CC),
        "HPB RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(hpb_exists("firmware/main/__hpb_no_such__.cc"),
        "HPB RED(infra): scanner reports a nonexistent file as present");
    n = hpb_scan_count("firmware/main", "__hpb_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "HPB RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = hpb_count_in_file(HPB_RN_CC, "s_battery_pct");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "HPB RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region2 = hpb_region_between(HPB_RN_CC,
                                           "void kidpal_render_node_inject_rtc(",
                                           "void kidpal_render_node_inject_env(");
        TEST_ASSERT_NOT_NULL_MESSAGE(region2,
            "HPB RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_slice(region2, "s_rtc_hour") >= 1,
            "HPB RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region2);
    }
}

/* ---------------------------------------------------------------------------
 * PH-A7 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = 2026-09-15 本文件冻结时 host 面全量既有齿实跑（现役齿 = K1a
 *   key_layer_dispatch + 包 0 host_infra_smoke，2026-09-15 544a804 归档后收敛），
 *   实测数字与在册红逐条点名见 taskpackage redBaselineResult（planner 转写）。
 *   判读纪律 = 新增红才是本包责任，在册红逐条点名、不代修。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1-3（home_p1.c 未接进 runner 的 weak 哨兵红 + seam [1]/[2]
 *       未落地的结构红——真实缺失行为）;
 *     - test 5 tooth 1（注册接线守卫, builder 接线后转绿）。
 *   MUST-STAY-GREEN at baseline（钉点/守卫齿, 现状即绿）:
 *     - test 2 决策表半（doze/blink 归档契约实现已在与面——随 home_p1.c 接线进
 *       runner 后同样必绿; pose_sleep 守卫现状绿）;
 *     - test 4 全部（PH-A5 钉点: 断网链路已实现, 本包只钉点）;
 *     - test 5 tooth 2/3/4（数据口径零触碰 + 需求契约 canary + 扫描器自检）。
 *   PH-A6 真机确认轮: human 持板 ADC 实测（电量值合理、低电闪烁/充电停闪真机可见）,
 *   证据落 delivery/changes/dev/260915-m1-n1b-p1-home-battery/。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑（新齿未接线前）= 有效基线（见 PH-A7 终跑判读）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/p1_home_battery.
 *         acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性（逐齿对现状必红的
 *     事实源, 2026-09-15 grep 实测）:
 *       - kidpal_render_node_inject_battery: .h/.cc/board.cc 全零命中;
 *       - 板类 adc1_handle/cali_handle/vbat_status 各恰 1 处（死成员=仅声明）,
 *         AdcBatteryMonitor / GetBatteryLevel 板类零命中;
 *       - kidpal_render_node.cc s_battery_pct 恰 4 处（声明+三读, 无注入写点 =
 *         恒值注入在世）;
 *       - home_p1.c / kidpal_render_node.cc 零 pose_sleep（守卫齿现状绿）;
 *       - wifi_board.cc GetNetworkStateIcon 区含 FONT_AWESOME_WIFI_SLASH + IsConnected
 *         （钉点齿现状绿）;
 *       - host_app/main/CMakeLists.txt 无 p1_home_battery.acceptance.c 与
 *         display/home_p1.c 接线行（test 5 tooth 1 红）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-15  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-n1b-p1-home-battery).
 * --------------------------------------------------------------------------- */
