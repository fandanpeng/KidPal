/*
 * Frozen acceptance tests — TaskPackage 261104-k5-story-cover-nav (subsystem:
 * firmware P4 封皮与故事层级导航 + story_list 共享帧消费 + 书封资产;
 * rigor=full / riskTier=high / mergeOrder 76; dependsOn K2/K4 已 merged)。
 *
 * Story 语义权威 = docs/requirements.md：
 *   S-F2.6（故事封皮与分层导航 AC1–AC4, v1.71 新增）= 本包主承载；
 *   S-F2.1（看「星星记录本」列表 AC1–AC7, 回归锚）、S-F2.2-AC1（对话式引导记录入口回归）、
 *   S-F2.3-AC1/AC2（回看记录）、S-F2.4-AC2（现存篇数计数 / 删除同步减）。
 * 契约形状权威 = docs/contracts/interface-contract.md §1.3③（story_list = P4 故事模块
 * 共享数据帧; total 唯一计数源; restore 分流; 薄载荷展开进封皮）与 §四 kidpal_session_ctx
 * （source_page 闭集 story_cover 普通语境）。决策事实 = PROPOSAL §3.1 D-01/D-07/D-08/D-09
 * + ADR-018（否决 story_cover view / cover_total）。技术落点 = 本包 plan.md
 * 「技术方案（planner+architect 预评）」A–G 节（planner 只读裁定）。
 *
 * Nature: 行为级（封皮渲染真帧探针 + 页面/键手势决策表 + restore 缓存装载纯逻辑）+
 * 结构级（设备专属 TU mcp_server.cc / kidpal_render_node.cc / 板级 .cc 源锚代理 tree-scan,
 * 沿 story_page_v2 / record_session_live 先例形态）+ 资产面（KAP1 生成件头 + 产线白名单）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [C1] 封皮子页态：story_page.h 子页枚举尾部追加 `KSTORY_PAGE_COVER = 7`
 *        （home = firmware/main/display/story_page.h；冻结数值 7，只增不改）。
 *        封皮是 KP_PAGE_STORY 的一个子态，s_local_page 全程 KP_PAGE_STORY
 *        （四点页点第 4 实心 + 页循环在模块内断开自然成立；KP_PAGE_* 枚举零改）。
 *
 *   [C2] 封皮渲染分支：home = firmware/main/display/story_page.c
 *        kidpal_story_page_render()。v->page == KSTORY_PAGE_COVER 时绘制「书封形态
 *        +『星星记录本』标题 + 星星计数 + 徽章」（书封 = rbox + 书脊线 + 原生几何;
 *        不做整页位图）。渲染入参语义冻结：
 *          - list != NULL  = 数据已送达 -> 画星星计数（含 ×0 诚实计数）；
 *          - list == NULL  = 数据未达   -> 画书封 + 标题, **不画计数**（不显 ×0 误导）,
 *            且 rc 恒 0（不因缺数据降级成 -1/黑屏——S-F2.6-AC1 补充规则「数据未达两态
 *            沿用 S-F2.1 AC5/AC6 同型判据」）。
 *        计数源恒 `kidpal_story_counter(list)`（total 优先；不另造 cover_total）。
 *
 *   [C3] 输入表（home = story_page.c kidpal_story_page_input；矩阵 = requirements §2.3
 *        + S-F2.6-AC2/AC3）。act 闭集尾部追加（冻结数值 = 本文件权威）：
 *          KSPV2_ACT_COVER_OPEN = 13   COVER KEY 短按 -> 进列表（并置 page = LIST）
 *          KSPV2_ACT_COVER_BACK = 14   LIST  KEY 长按 -> 回封皮（并置 page = COVER）
 *        COVER 行全表：
 *          KEY 短=COVER_OPEN（page -> LIST）/ KEY 长=GLOBAL（回首页）/
 *          KEY 双=NONE / BOOT 短·双·长=GLOBAL（4 页循环 wrap 回 P1 首页）。
 *        LIST KEY 长压由 GLOBAL 翻正为 COVER_BACK（逐层返回「列表 → 封皮」；原「一层
 *        即到家」口径随封皮下沉改为逐层）。READER KEY 长 = 既有 READER_EXIT 零改。
 *        COVER 不进 300ms 单双击消歧：kidpal_story_debounce_gate(COVER) == 0
 *        （冻结门控只认 LIST）。
 *
 *   [C4] 封皮入口 reset（human 2026-11-15 裁定 = 板级 BOOT 钩子）：
 *        home = boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc GLOBAL_ACT_PAGE_NEXT
 *        分支（当前页 = P3 时）调 McpServer::StoryEnterCover()（经 ScheduleStoryRender()
 *        落主任务），保证冷启动/异常退出后首翻 P4 也见封皮。**不动** [S1] 签名与
 *        kidpal_story_view_init 的 page=LIST 语义。
 *
 *   [C5] restore 缓存装载纯逻辑（home = story_page.c；plan D 技术要求）：
 *          int kidpal_story_list_load(kidpal_story_view_t *v,
 *                                     const kidpal_story_list_t *list);
 *            -- 只装 count / counter(=counter(list)) / screens / 影子 id（s_ksy_item_ids）
 *               + cursor 钳制到 [0, count-1]（count==0 -> 0）；**page / rec 子态
 *               / 盖章字段零触碰**（restore 帧只填缓存不导航、活动子态零重置）。
 *            -- NULL v 或 NULL list -> -1（调用方降级，不盲装）；成功 -> 0。
 *        设备侧（mcp_server.cc VRT_ROUTE_STORY_LIST 区）restore 分支改用本缝，
 *        **不再调用 kidpal_story_view_init**（旧实现无条件重装会把活动子态重置成 LIST，
 *        restore 常态化后放大 TECH-261022 churn）；非 restore 完整帧 = list_load +
 *        page=KSTORY_PAGE_COVER（进场帧进封皮）。
 *
 *   [C9] 资产：`firmware/main/assets/spiffs_root/ic_coin_52_52.bin`（KAP1 头
 *        magic 0x3150414b / format 1 / w=h=52 / stride 7 / 380B）、产线白名单
 *        tools/asset-pipeline/gen_art_spiffs.mjs 尾部追加 'ic_coin_52_52'、生成器
 *        gen_assets.mjs 同名 job、模拟器 assets.c 同名符号；story_page.c 的
 *        KSTORY_ART_CACHE_SLOTS 扩容至 >= 8（封面徽章 ic_coin 52px 与既有 7 枚并存）。
 *
 * ===========================================================================
 * Coverage map（acceptanceRef -> TEST_CASE）:
 *   coverage: S-F2.6  -> KCN-C1..C5（封皮四件/计数两态/手势矩阵/页态序列/restore）
 *   coverage: S-F2.6-AC1 -> KCN-C1（封皮四件）、KCN-C2（计数两态）、KCN-C5（进场帧）
 *   coverage: S-F2.6-AC2 -> KCN-C3（KEY 短进列表 / BOOT wrap 回家 / KEY 长回首页）
 *   coverage: S-F2.6-AC3 -> KCN-C3 / KCN-C4（阅读→列表→封皮→首页逐层返回, 每次一层）
 *   coverage: S-F2.6-AC4 -> KCN-C5（语音「打开故事」= 非 restore 完整帧进封皮, 不跳列表）
 *   coverage: S-F2.1-AC1/AC2 -> KCN-C6（列表键帽零回归 + 手势回归）
 *   coverage: S-F2.1-AC5/AC6 -> KCN-C2 / KCN-C9（计数两态）
 *   coverage: S-F2.2-AC1 -> KCN-C9（记录语境入口回归, 列表仍 story）
 *   coverage: S-F2.3-AC1/AC2 -> KCN-C9（阅读键回归）
 *   coverage: S-F2.4-AC2 -> KCN-C9（删除同步减计数）
 *   coverage: K5S-E1 -> KCN-C1..C5、K5S-E3 -> KCN-C9（资产面离线半）
 *   coverage: §2.3 -> KCN-C3（按键手势矩阵 P4 行）
 *   coverage: §2.4 -> KCN-C1（四页循环 / 第 4 页点实心）
 *   coverage: D-01 -> KCN-C3 / KCN-C4（封皮下沉 + 逐层返回）
 *   coverage: D-07 -> KCN-C5（restore 只填缓存）
 *   coverage: D-08 -> KCN-C5（story_list P4 共享帧消费 / 不新增 view）
 *   coverage: D-09 -> KCN-C9（书封/ic-coin 走既有资产管线）
 *   coverage: §5 -> KCN-C4（导航并集收口半）
 *
 * ===========================================================================
 * RED baseline（acceptance-author 落盘时实测; 设计态红因 = 实现缺席, 非环境问题）:
 *   - story_page.h 无 KSTORY_PAGE_COVER（本文件以 KCN_PAGE_COVER=7 字面钉数值）;
 *   - story_page.c 无 COVER 渲染分支 / 无 COVER 输入行 / 无 kidpal_story_list_load
 *     （本文件以 weak 哨兵 -9999 承接 -> KCN-C5 行为断言红）;
 *   - keybar.c P4 页槽行仍是 (说话·双击看, 下移)（KCN-C1 键帽断言红）;
 *   - story 路由区仍无条件 kidpal_story_view_init（KCN-C5 结构断言红）;
 *   - 无 ic_coin_52_52.bin / 白名单缺项 / KSTORY_ART_CACHE_SLOTS = 7（KCN-C9 红）。
 *   指令 = 在 host_app 目录 build 后跑 build/host_infra_test.elf。
 * ===========================================================================
 */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <lvgl.h>

#include "../display/story_page.h"
#include "../display/keybar.h"
#include "../display/kidpal_font.h"

/* host 渲染缝（host_infra_seam.c 提供, 恒链入 runner）。只探针。 */
int host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * 冻结数值镜像（本文件为 COVER 子态/新动作码的数值权威）。
 *   - page 枚举既有值（LIST..ORGANIZING）由 story_page.h include 直读（真实类型
 *     kidpal_story_view_t 直接用——无镜像布局漂移风险, 优于 SPV2 的镜像形态）;
 *   - COVER 子态与两枚新动作码在 story_page.h 落定前**以字面钉数值**，故本地定义。
 * ------------------------------------------------------------------------ */
#define KCN_PAGE_COVER     7   /* KSTORY_PAGE_COVER（[C1] 冻结值） */
#define KCN_ACT_COVER_OPEN 13  /* COVER KEY 短 -> 进列表（[C3] 冻结值） */
#define KCN_ACT_COVER_BACK 14  /* LIST  KEY 长 -> 回封皮（[C3] 冻结值） */

/* 既有动作码（story_page.h kidpal_story_act_t 直读; 此处仅取本文件断言用到的）。 */
#define KCN_ACT_GLOBAL       KSPV2_ACT_GLOBAL
#define KCN_ACT_NONE         KSPV2_ACT_NONE
#define KCN_ACT_DELETE_ENTER KSPV2_ACT_DELETE_ENTER
#define KCN_ACT_DELETE_CONFIRM KSPV2_ACT_DELETE_CONFIRM
#define KCN_ACT_PAGE_TURN    KSPV2_ACT_PAGE_TURN
#define KCN_ACT_READER_EXIT  KSPV2_ACT_READER_EXIT

#define KCN_UNIMPLEMENTED   (-9999) /* [C5] weak 哨兵：永不是合法返回值 */
#define KCN_NOT_RENDERED    ((int32_t)-1)
#define KCN_TOTAL_ABSENT    KSTORY_TOTAL_ABSENT
#define KCN_LAYER_NONE      0       /* KIDPAL_LAYER_NONE（kidpal_layer.h 冻结值） */

#define KCN_FB_W 400
#define KCN_FB_H 300
#define KCN_WHITE 0xFFFFFFul

/* -------------------------------------------------------------------------- *
 * 封皮版面覆盖带（来源 = docs/ui/V9/story-cover.png 400x300 页级 PNG = 布局坐标
 * 定位权威）。覆盖带只钉「四件在墨/计数两态可判别」，不钉像素级观感（观感归
 * uiuxLevel 4 design review）。builder 若实测位移且几何正常，按 tighten-only 窗口
 * 携证据重基线走 planner，不静默放宽。
 * ------------------------------------------------------------------------ */
#define KCN_BOOK_V_X0   85    /* 书封左边框 / 书脊线竖带 */
#define KCN_BOOK_V_X1   140
#define KCN_BOOK_V_Y0   30
#define KCN_BOOK_V_Y1   245
#define KCN_BOOK_H_X0   90    /* 书封上边框横带 */
#define KCN_BOOK_H_X1   310
#define KCN_BOOK_H_Y0   25
#define KCN_BOOK_H_Y1   60
#define KCN_BOOK_RUN_MIN 120  /* 书封边框最短连续墨段（书封约 200x200 框, 留足容忍） */

#define KCN_TITLE_X0    90    /* 「星星记录本」标题带（在徽章之下、书封之内） */
#define KCN_TITLE_X1    320
#define KCN_TITLE_Y0    130
#define KCN_TITLE_Y1    182

#define KCN_BADGE_X0    158   /* 大星徽（双圈印章） */
#define KCN_BADGE_X1    240
#define KCN_BADGE_Y0    52
#define KCN_BADGE_Y1    122
#define KCN_BADGE_INK_MIN 80  /* 星形实心/描边 + 双圈 -> 远超此值；缺画 -> 0 */

#define KCN_COUNT_X0    315   /* 右上角星星计数「★ ×N」 */
#define KCN_COUNT_X1    400
#define KCN_COUNT_Y0    0
#define KCN_COUNT_Y1    45

/* 既有 4 页页循环（P4 = 星星记录本; 页点第 4 实心）。 */
#define KCN_PAGE_INDEX_P4 3

/* -------------------------------------------------------------------------- *
 * [C5] weak 哨兵（强定义 home = story_page.c; builder 落 kidpal_story_list_load
 * 后覆盖 -> KCN-C5 行为断言转绿。落定前哨兵恒红 = 「frozen seam not implemented」）。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_story_list_load(kidpal_story_view_t *v,
                                                 const kidpal_story_list_t *list)
{
    (void)v;
    (void)list;
    return KCN_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 工具：消息格式化 + 仓根定位 + 文件读取（kld/SPV2 先例形态, root 注入纪律 =
 * 不依赖调用 cwd 的绝对路径假设）。
 * ------------------------------------------------------------------------ */
static char kcn_fm_buf[1024];
static const char *kcn_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kcn_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kcn_fm_buf, sizeof(kcn_fm_buf), fmt, ap);
    va_end(ap);
    return kcn_fm_buf;
}

#define KCN_MAX_PATH 1024
#define KCN_MAX_READ (8u * 1024u * 1024u)

static char kcn_root[KCN_MAX_PATH];
static int kcn_root_ready = 0;

static int kcn_is_regular(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

static int kcn_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KCN_MAX_PATH];
    size_t i;

    if (kcn_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kcn_is_regular(probe)) {
            snprintf(kcn_root, sizeof(kcn_root), "%s", cands[i]);
            kcn_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

#define KCN_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kcn_locate_root(), \
    "KCN RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app")

static uint8_t *kcn_read_bin_rel(const char *rel, size_t *out_len)
{
    char path[KCN_MAX_PATH];
    FILE *f;
    long sz;
    uint8_t *buf;

    if (out_len != NULL) {
        *out_len = 0;
    }
    if (!kcn_locate_root()) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", kcn_root, rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KCN_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    assert(buf != NULL); /* 真实 OOM 守卫（闸认断言形态）: host OOM 必须大声失败 */
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

static char *kcn_read_text_rel(const char *rel)
{
    size_t len = 0;
    uint8_t *raw = kcn_read_bin_rel(rel, &len);
    char *buf;

    if (raw == NULL) {
        return NULL;
    }
    buf = (char *)malloc(len + 1);
    assert(buf != NULL);
    memcpy(buf, raw, len);
    buf[len] = '\0';
    free(raw);
    return buf;
}

static int kcn_file_contains(const char *rel, const char *needle)
{
    char *buf;
    int ok;

    if (!kcn_locate_root()) {
        return 0;
    }
    buf = kcn_read_text_rel(rel);
    ok = (buf != NULL && strstr(buf, needle) != NULL) ? 1 : 0;
    free(buf);
    return ok;
}

static int kcn_count_in(const char *hay, const char *needle)
{
    int n = 0;
    size_t ln;
    const char *p;

    if (hay == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }
    ln = strlen(needle);
    p = hay;
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p += ln;
    }
    return n;
}

/* 文件内 [start, end) 切片（start 缺失 -> NULL; end 缺失 -> EOF）。
 * 返回 malloc 串, 调用方 free。锚漂移 = 断言自身 RED(infra) 须复核。 */
static char *kcn_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf;
    char *s, *e;
    char *slice;
    size_t span;

    if (!kcn_locate_root()) {
        return NULL;
    }
    buf = kcn_read_text_rel(rel);
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
        e = buf + strlen(buf);
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* 从源文本抽 `#define <name> <int>` 的整数值。返回 0 = 未命中 / 非整数。 */
static int kcn_define_int(const char *rel, const char *name)
{
    char *buf;
    char needle[96];
    const char *p;
    long v;
    char *endp;

    if (!kcn_locate_root()) {
        return 0;
    }
    buf = kcn_read_text_rel(rel);
    if (buf == NULL) {
        return 0;
    }
    snprintf(needle, sizeof(needle), "#define %s", name);
    p = strstr(buf, needle);
    if (p == NULL) {
        free(buf);
        return 0;
    }
    p += strlen(needle);
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    v = strtol(p, &endp, 10);
    free(buf);
    if (endp == p) {
        return 0;
    }
    return (int)v;
}

/* -------------------------------------------------------------------------- *
 * 帧探针（story_typography sgx_ 先例同款）。探针未渲染大声红，绝不读成零墨。
 * ------------------------------------------------------------------------ */
static int32_t kcn_fb_a[KCN_FB_W * KCN_FB_H];
static int32_t kcn_fb_b[KCN_FB_W * KCN_FB_H];

static int kcn_snap(int32_t *buf)
{
    int x, y;
    for (y = 0; y < KCN_FB_H; y++) {
        for (x = 0; x < KCN_FB_W; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == KCN_NOT_RENDERED) {
                return -1;
            }
            buf[y * KCN_FB_W + x] = px;
        }
    }
    return 0;
}

/* 渲染 + 直刷 + 收帧（SPV2-B1 R5 时序先例：render_frame 起手一次, render 后
 * lv_refr_now 直刷再探针——render_frame 在 render 后调会整帧盖掉被测内容）。 */
static void kcn_render_snap(const kidpal_story_view_t *v,
                            const kidpal_story_list_t *list,
                            const kidpal_story_text_t *story,
                            int32_t *buf)
{
    int rc = kidpal_story_page_render(v, list, story);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        kcn_fm("KCN RED: kidpal_story_page_render rc=%d——渲染面显式降级（封皮应容忍 "
               "list==NULL 数据未达态; fixture 前提被破坏）", rc));
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE_MESSAGE(kcn_snap(buf) == 0,
        "KCN RED(infra): frame not rendered — 探针缝断裂");
}

/* 带内暗像素计数（非白即墨, kst_ink_count 同款）。 */
static int kcn_ink_count(const int32_t *f, int x0, int y0, int x1, int y1)
{
    int n = 0, x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int32_t p = f[y * KCN_FB_W + x];
            if (p >= 0 && p != (int32_t)KCN_WHITE) {
                n++;
            }
        }
    }
    return n;
}

/* 带内最长连续水平墨段（书封上边框形态证据）。 */
static int kcn_max_h_run(const int32_t *f, int x0, int y0, int x1, int y1)
{
    int best = 0, y, x;
    for (y = y0; y <= y1; y++) {
        int run = 0;
        for (x = x0; x <= x1; x++) {
            int32_t p = f[y * KCN_FB_W + x];
            if (p >= 0 && p != (int32_t)KCN_WHITE) {
                run++;
                if (run > best) {
                    best = run;
                }
            } else {
                run = 0;
            }
        }
    }
    return best;
}

/* 带内最长连续垂直墨段（书封左边框/书脊线形态证据）。 */
static int kcn_max_v_run(const int32_t *f, int x0, int y0, int x1, int y1)
{
    int best = 0, x, y;
    for (x = x0; x <= x1; x++) {
        int run = 0;
        for (y = y0; y <= y1; y++) {
            int32_t p = f[y * KCN_FB_W + x];
            if (p >= 0 && p != (int32_t)KCN_WHITE) {
                run++;
                if (run > best) {
                    best = run;
                }
            } else {
                run = 0;
            }
        }
    }
    return best;
}

/* -------------------------------------------------------------------------- *
 * Fixture helpers（字形池 = fonts_root 全量 CJK 面; 「星星记录本」标题字形在场，
 * 缺场按 infra 红申明, 不读成设计态零墨）。
 * ------------------------------------------------------------------------ */
#define KCN_FONTS_ROOT "firmware/main/assets/fonts_root"

static int kcn_font_reader(const char *name, const uint8_t **data, size_t *size)
{
    char rel[KCN_MAX_PATH];
    uint8_t *buf;
    size_t len;

    snprintf(rel, sizeof(rel), "%s/%s", KCN_FONTS_ROOT, name);
    buf = kcn_read_bin_rel(rel, &len);
    if (buf == NULL) {
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

static int kcn_missing_reader(const char *name, const uint8_t **data, size_t *size)
{
    (void)name;
    (void)data;
    (void)size;
    return -1;
}

static void kcn_mount_gate(void)
{
    int rc = kidpal_font_mount_assets(kcn_font_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        kcn_fm("KCN RED(infra): kidpal_font_mount_assets(rc=%d) 非完成态——fonts_root "
               "资产/装载底座缺场（P0 交付面, 非本包设计态）", rc));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_body() != NULL,
        "KCN RED(infra): kidpal_font_body() NULL——字体句柄 seam 断裂");
}

static void kcn_unmount(void)
{
    (void)kidpal_font_mount_assets(kcn_missing_reader);
}

static void kcn_make_list(kidpal_story_list_t *l, int n)
{
    int i;
    static const char *const titles[3] = {
        "小兔子找朋友", "勇敢的小明", "我的大冒险"
    };
    memset(l, 0, sizeof(*l));
    if (n > KSTORY_LIST_MAX) {
        n = KSTORY_LIST_MAX;
    }
    if (n > 3) {
        n = 3;
    }
    l->total = n;
    l->count = n;
    for (i = 0; i < n; i++) {
        snprintf(l->items[i].id, sizeof(l->items[i].id), "2026-08-2%d-id%d", i, i);
        snprintf(l->items[i].title, sizeof(l->items[i].title), "%s", titles[i]);
        snprintf(l->items[i].date, sizeof(l->items[i].date), "08-2%d", i);
    }
}

static void kcn_view_cover(kidpal_story_view_t *v, const kidpal_story_list_t *l)
{
    memset(v, 0, sizeof(*v));
    v->page = KCN_PAGE_COVER;
    v->stamp_serial = KSTORY_SEQ_ABSENT;
    if (l != NULL) {
        v->count = l->count;
        v->counter = kidpal_story_counter(l);
    }
    v->screens = 1;
}

/* ========================================================================== *
 * TEST 1 — KCN-C1: 封皮四件 + 四点第 4 实心 + 键帽（S-F2.6-AC1）
 *   tooth 1 书封形态（竖边框/书脊线 + 上边框真墨）；
 *   tooth 2 标题「星星记录本」在墨；
 *   tooth 3 星星计数在墨（与 C2 两态配对）；
 *   tooth 4 大星徽在墨；
 *   tooth 5 四点、第 4 实心（kidpal_keybar_dots(page=P4)）；
 *   tooth 6 覆盖层页点消失（overlay_top!=0 -> visible 0）；
 *   tooth 7 键帽「打开 / 下一页」（kidpal_keybar_slots(page=P4)）。
 * RED today: 无 COVER 渲染分支（书封/标题/徽章/计数全零墨）+ P4 页槽仍 (说话·双击看/下移)。
 * ========================================================================== */
/* coverage: S-F2.6-AC1 -- 封皮四件（书封/标题/星星计数/键帽）+ 四点第4实心 */
/* coverage: S-F2.6 -- 故事封皮与分层导航（主承载 AC1 四件） */
TEST_CASE("story_cover_nav: 封皮四件+四点第4实心+键帽 (KCN-C1)",
          "[story_cover_nav][KCN-C1][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    kidpal_keybar_dots_t dots;
    const char *left = NULL;
    const char *right = NULL;

    assert(KCN_FB_W == 400 && KCN_FB_H == 300); /* 闸认真实断言 + 版面不变量 */
    KCN_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KCN RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");
    kcn_mount_gate();

    kcn_make_list(&l, 3);
    kcn_view_cover(&v, &l);
    kcn_render_snap(&v, &l, NULL, kcn_fb_a);

    /* tooth 1: 书封形态——竖边框/书脊线与上边框各成 >120px 连续墨段。 */
    {
        int vrun = kcn_max_v_run(kcn_fb_a, KCN_BOOK_V_X0, KCN_BOOK_V_Y0,
                                 KCN_BOOK_V_X1, KCN_BOOK_V_Y1);
        int hrun = kcn_max_h_run(kcn_fb_a, KCN_BOOK_H_X0, KCN_BOOK_H_Y0,
                                 KCN_BOOK_H_X1, KCN_BOOK_H_Y1);
        TEST_ASSERT_TRUE_MESSAGE(vrun >= KCN_BOOK_RUN_MIN && hrun >= KCN_BOOK_RUN_MIN,
            kcn_fm("KCN-C1 RED: 书封形态缺件（竖最长墨段 %d / 横最长墨段 %d, 均需 >=%d）"
                   "——封皮未落 story_page.c COVER 渲染分支（书封 rbox+书脊线原生几何）；"
                   "story-cover.png 布局权威", vrun, hrun, KCN_BOOK_RUN_MIN));
    }

    /* tooth 2: 标题「星星记录本」在墨（徽章下方标题带）。 */
    {
        int ink = kcn_ink_count(kcn_fb_a, KCN_TITLE_X0, KCN_TITLE_Y0,
                                KCN_TITLE_X1, KCN_TITLE_Y1);
        TEST_ASSERT_TRUE_MESSAGE(ink > 0,
            kcn_fm("KCN-C1 RED: 封皮标题带零墨（x[%d..%d] y[%d..%d]）——「星星记录本」"
                   "标题缺失或字形缺场（字形缺场按 infra 红申明, 不静默）",
                   KCN_TITLE_X0, KCN_TITLE_X1, KCN_TITLE_Y0, KCN_TITLE_Y1));
    }

    /* tooth 3: 星星计数在墨（右上角条带; 与 C2 两态的「有数据」半配对）。 */
    {
        int ink = kcn_ink_count(kcn_fb_a, KCN_COUNT_X0, KCN_COUNT_Y0,
                                KCN_COUNT_X1, KCN_COUNT_Y1);
        TEST_ASSERT_TRUE_MESSAGE(ink > 0,
            "KCN-C1 RED: 封皮右上角星星计数条带零墨——数据已送达（list 非空）却未画"
            "「★ ×N」计数（S-F2.6-AC1 已收集星星数缺失）");
    }

    /* tooth 4: 大星徽在墨。 */
    {
        int ink = kcn_ink_count(kcn_fb_a, KCN_BADGE_X0, KCN_BADGE_Y0,
                                KCN_BADGE_X1, KCN_BADGE_Y1);
        TEST_ASSERT_TRUE_MESSAGE(ink >= KCN_BADGE_INK_MIN,
            kcn_fm("KCN-C1 RED: 封皮大星徽带墨量 %d < %d——书封徽章缺失（书封四件之一）",
                   ink, KCN_BADGE_INK_MIN));
    }

    /* tooth 5: 四点、第 4 实心（P4 = 页循环第 4 页）。 */
    {
        int rc = kidpal_keybar_dots(KCN_PAGE_INDEX_P4, 0, &dots);
        TEST_ASSERT_TRUE_MESSAGE(
            rc == 0 && dots.visible == 1 && dots.total == 4 && dots.active == 3,
            kcn_fm("KCN-C1 RED: 封皮页点 (rc/visible/total/active) = (%d/%d/%d/%d), 期望 "
                   "(0/1/4/3)——四点、第 4 实心缺失（S-F2.6-AC1）",
                   rc, dots.visible, dots.total, dots.active));
    }

    /* tooth 6: 覆盖层页点消失（删除确认等覆盖层顶层 -> 页点隐藏）。 */
    {
        kidpal_keybar_dots_t ov;
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_keybar_dots(KCN_PAGE_INDEX_P4, 1, &ov) == 0 && ov.visible == 0,
            "KCN-C1 RED: 封皮覆盖层态页点未隐藏（visible != 0）——覆盖层页点消失"
            "（K5S-E1 / S-F2.1-AC4 同源口径）被破");
    }

    /* tooth 7: 封皮键帽「打开 / 下一页」（P4 基层页槽行; 封皮 = P4 子态回落页槽表）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_slots(KCN_PAGE_INDEX_P4, 0, 0, &left, &right) == 0 &&
            left != NULL && right != NULL &&
            strcmp(left, "打开") == 0 && strcmp(right, "下一页") == 0,
        kcn_fm("KCN-C1 RED: P4 页槽键帽 (左=%s, 右=%s) 期望 (打开, 下一页)——封皮键帽"
               "「打开 / 下一页」未落（S-F2.6-AC1; 现状 (说话·双击看, 下移) = 列表行残留）",
               left != NULL ? left : "(null)", right != NULL ? right : "(null)"));

    kcn_unmount();
}

/* ========================================================================== *
 * TEST 2 — KCN-C2: 封皮星星计数两态（S-F2.6-AC1 补充规则 / S-F2.1-AC5/AC6 同型）
 *   tooth 1 数据已送达（list->total==0）-> 画 ×0（诚实计数、不隐瞒）；
 *   tooth 2 数据已送达（list->total==5）-> 画计数；
 *   tooth 3 数据未达（list==NULL）-> rc==0、书封+标题仍在墨、计数带零墨（不显 ×0 误导）；
 *   tooth 4 结构：render_node.cc KP_PAGE_STORY 分支出现 KSTORY_PAGE_COVER（封皮经
 *           story_page.c 渲染面分派, 不被列表空态分支截走）。
 * RED today: 无 COVER 分支 -> tooth 1/2/3 计数带零墨/标题零墨; tooth 4 源码零命中。
 * ========================================================================== */
/* coverage: S-F2.6-AC1 -- 封皮星星计数两态（数据已送达含 ×0 / 数据未达不显计数） */
/* coverage: S-F2.1-AC5 -- 封皮计数数据已送达如实显示（含 ×0） */
/* coverage: S-F2.1-AC6 -- 封皮计数数据未达不显示（不显 ×0 误导） */
/* coverage: S-F2.6 -- 封皮计数两态与列表同型判据 */
TEST_CASE("story_cover_nav: 封皮计数两态 (KCN-C2)",
          "[story_cover_nav][KCN-C2][behavior][structural]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    char *region;
    int ink_zero, ink_five, ink_absent;

    assert(KCN_FB_W == 400 && KCN_FB_H == 300);
    KCN_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KCN RED(infra): host_infra_render_frame failed");
    kcn_mount_gate();

    /* tooth 1: total==0（数据已送达、确无篇目）-> ×0 诚实计数（计数带在墨）。 */
    kcn_make_list(&l, 0);
    l.total = 0;
    l.count = 0;
    kcn_view_cover(&v, &l);
    kcn_render_snap(&v, &l, NULL, kcn_fb_a);
    ink_zero = kcn_ink_count(kcn_fb_a, KCN_COUNT_X0, KCN_COUNT_Y0,
                             KCN_COUNT_X1, KCN_COUNT_Y1);
    TEST_ASSERT_TRUE_MESSAGE(ink_zero > 0,
        "KCN-C2 RED: 数据已送达（total=0）时封皮计数带零墨——×0 诚实计数缺失"
        "（S-F2.1-AC5 同型判据; 数据已送达必须如实显示, 含 ×0）");

    /* tooth 2: total==5 -> 画计数（计数带在墨）。 */
    kcn_make_list(&l, 3);
    l.total = 5; /* 现存篇数 5 而窗口 3 条 -> 计数只信 total */
    kcn_view_cover(&v, &l);
    kcn_render_snap(&v, &l, NULL, kcn_fb_b);
    ink_five = kcn_ink_count(kcn_fb_b, KCN_COUNT_X0, KCN_COUNT_Y0,
                             KCN_COUNT_X1, KCN_COUNT_Y1);
    TEST_ASSERT_TRUE_MESSAGE(ink_five > 0,
        kcn_fm("KCN-C2 RED: 数据已送达（total=5, items=3 窗口）时封皮计数带零墨——"
               "计数缺失或以 items.length 顶替 total（契约 §1.3③ total 唯一源）"));

    /* tooth 3: 数据未达（list==NULL）-> rc 0、标题在墨、计数带零墨。 */
    kcn_view_cover(&v, NULL);
    kcn_render_snap(&v, NULL, NULL, kcn_fb_a);
    {
        int title_ink = kcn_ink_count(kcn_fb_a, KCN_TITLE_X0, KCN_TITLE_Y0,
                                      KCN_TITLE_X1, KCN_TITLE_Y1);
        ink_absent = kcn_ink_count(kcn_fb_a, KCN_COUNT_X0, KCN_COUNT_Y0,
                                   KCN_COUNT_X1, KCN_COUNT_Y1);
        TEST_ASSERT_TRUE_MESSAGE(title_ink > 0,
            "KCN-C2 RED: 数据未达（list==NULL）时封皮书封/标题零墨——数据未达仍须"
            "显示封皮骨架（不黑屏, S-F2.6-AC1 补充规则）");
        TEST_ASSERT_TRUE_MESSAGE(ink_absent == 0,
            kcn_fm("KCN-C2 RED: 数据未达（list==NULL）时封皮仍画计数（带墨 %d）——"
                   "篇数未知不能显 ×0 误导（S-F2.1-AC6 同型判据）", ink_absent));
    }

    /* tooth 4（结构源锚代理）: render_node.cc KP_PAGE_STORY 分支出现封皮子态分派。 */
    region = kcn_region_between("firmware/main/display/kidpal_render_node.cc",
                               "case KP_PAGE_STORY:", "default:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KCN-C2 RED(infra): render_node.cc KP_PAGE_STORY 分支锚漂移——须复核本断言锚");
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "KSTORY_PAGE_COVER") != NULL,
        "KCN-C2 RED: render_node.cc 的 P4 页分支无 KSTORY_PAGE_COVER 分派——封皮未接"
        "渲染面（列表空态分支会把封皮截成列表空态/黑屏; S-F2.6-AC1）");
    free(region);

    kcn_unmount();
}

/* ========================================================================== *
 * TEST 3 — KCN-C3: 手势矩阵（S-F2.6-AC2/AC3 + §2.3）
 *   COVER: KEY 短=COVER_OPEN(13) 且 page->LIST / KEY 长=GLOBAL / KEY 双=NONE /
 *          BOOT 短·双·长=GLOBAL（4 页 wrap）。
 *   LIST:  KEY 长=COVER_BACK(14) 且 page->COVER（逐层返回翻正）。
 *   READER: KEY 长=READER_EXIT(9) 且 page->LIST（零改回归）。
 *   COVER 不进 300ms 消歧：debounce_gate(COVER)==0、gate(LIST)==1。
 * RED today: page=7 走 default -> GLOBAL（KEY 短 = -2 非 13, LIST KEY 长 = -2 非 14）。
 * ========================================================================== */
/* coverage: S-F2.6-AC2 -- 封皮 KEY 短进列表 / BOOT wrap 回家 / KEY 长回首页 */
/* coverage: S-F2.6-AC3 -- 列表 KEY 长回封皮（逐层返回一层） */
/* coverage: S-F2.1-AC2 -- 列表 KEY 长按路径随封皮下沉改逐层（不再一步到家） */
/* coverage: S-F2.6 -- 手势矩阵封皮/列表/阅读三行 */
TEST_CASE("story_cover_nav: 手势矩阵 COVER/LIST/READER (KCN-C3)",
          "[story_cover_nav][KCN-C3][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    int act;

    assert(KCN_ACT_COVER_OPEN == 13 && KCN_ACT_COVER_BACK == 14);
    KCN_REQUIRE_ROOT();
    kcn_make_list(&l, 2);

    /* ---- COVER 行 ---- */
    kcn_view_cover(&v, &l);

    /* tooth 1: KEY 短 = COVER_OPEN 且 page -> LIST（进列表）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_COVER_OPEN && v.page == KSTORY_PAGE_LIST,
        kcn_fm("KCN-C3 RED: COVER KEY 短按 act=%d page=%d, 期望 COVER_OPEN(=%d) 且 "
               "page=LIST(%d)——封皮 KEY 短按不进列表（S-F2.6-AC2）",
               act, v.page, KCN_ACT_COVER_OPEN, (int)KSTORY_PAGE_LIST));

    /* tooth 2: KEY 长 = GLOBAL（回首页, 4 页循环 wrap 由全局层 PAGE_NEXT/HOME 承载）。 */
    kcn_view_cover(&v, &l);
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS) == KCN_ACT_GLOBAL,
        "KCN-C3 RED: COVER KEY 长按未交全局——「KEY 长按回首页」缺失（S-F2.6-AC2）");

    /* tooth 3: KEY 双 = NONE（封皮无双击语义）。 */
    kcn_view_cover(&v, &l);
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK) == KCN_ACT_NONE,
        "KCN-C3 RED: COVER KEY 双击非 NONE——封皮无双击语义（须消费不动, 不漏网）");

    /* tooth 4: BOOT 短/双/长 = GLOBAL（翻回 P1 首页 wrap; 页循环四页成立）。 */
    kcn_view_cover(&v, &l);
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK) == KCN_ACT_GLOBAL &&
            kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK) ==
                KCN_ACT_GLOBAL &&
            kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS) ==
                KCN_ACT_GLOBAL,
        "KCN-C3 RED: COVER BOOT 族未交全局翻页——「BOOT 短按翻回 P1 首页（4 页循环 "
        "wrap）」缺失（S-F2.6-AC2）");

    /* ---- LIST 行：KEY 长 = COVER_BACK 且 page -> COVER ---- */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "KCN-C3 RED(infra): view_init 失败——fixture 前提被破坏");
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_COVER_BACK && v.page == KCN_PAGE_COVER,
        kcn_fm("KCN-C3 RED: LIST KEY 长按 act=%d page=%d, 期望 COVER_BACK(=%d) 且 "
               "page=COVER(%d)——列表 KEY 长按未回封皮（仍 GLOBAL 一步到家 = 逐层返回"
               "缺失; S-F2.6-AC3）", act, v.page, KCN_ACT_COVER_BACK, KCN_PAGE_COVER));

    /* ---- READER 行：KEY 长 = READER_EXIT 且 page -> LIST（零改回归） ---- */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "KCN-C3 RED(infra): view_init 失败");
    v.page = KSTORY_PAGE_READER;
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_READER_EXIT && v.page == KSTORY_PAGE_LIST,
        kcn_fm("KCN-C3 RED: READER KEY 长按 act=%d page=%d, 期望 READER_EXIT(=%d) 且 "
               "page=LIST——阅读退出回列表既有语义被误伤（S-F2.6-AC3 零改）",
               act, v.page, KCN_ACT_READER_EXIT));

    /* ---- 消歧：COVER 不进 300ms 双击消歧（冻结门控只认 LIST） ---- */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_gate(KCN_PAGE_COVER) == 0,
        "KCN-C3 RED: COVER 被纳入 300ms 单双击消歧门控——封皮单击被挂起/误吞"
        "（冻结门控只认 LIST）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_gate(KSTORY_PAGE_LIST) == 1,
        "KCN-C3 RED: LIST 未挂消歧窗——门控函数失效或被误改（零回归）");
}

/* ========================================================================== *
 * TEST 4 — KCN-C4: 页态逐层返回序列（S-F2.6-AC3）
 *   READER --KEY 长--> LIST --KEY 长--> COVER --KEY 长--> GLOBAL（首页）。
 *   每按一次只上一层（READER 长按不直达 COVER; LIST 长按不直达全局）。
 *   tooth 5 结构：板级 BOOT 钩子入口 reset（human 2026-11-15 裁定）——PAGE_NEXT 分支
 *           出现 StoryEnterCover（冷启动/异常退出后首翻 P4 见封皮）。
 * RED today: LIST KEY 长=GLOBAL（跳层）; 板级/mcp 无 StoryEnterCover。
 * ========================================================================== */
/* coverage: S-F2.6-AC3 -- 阅读→列表→封皮→首页逐层返回, 每按一次只上一层 */
/* coverage: S-F2.6-AC1 -- 首翻 P4 见封皮（板级 BOOT 入口 reset 结构锚） */
/* coverage: S-F2.6 -- 页态序列 */
TEST_CASE("story_cover_nav: 页态逐层返回序列 (KCN-C4)",
          "[story_cover_nav][KCN-C4][behavior][structural]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    int act;
    char *region;

    assert(KCN_PAGE_COVER == 7);
    KCN_REQUIRE_ROOT();
    kcn_make_list(&l, 3);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "KCN-C4 RED(infra): view_init 失败");

    /* 第 0 层：阅读态。 */
    v.page = KSTORY_PAGE_READER;
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_READER_EXIT && v.page == KSTORY_PAGE_LIST,
        kcn_fm("KCN-C4 RED: 阅读态长按后 page=%d(非 LIST)——逐层返回第一步失败或跨层",
               v.page));

    /* 第 1 层：列表态长按 -> 封皮（不是首页）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_COVER_BACK && v.page == KCN_PAGE_COVER,
        kcn_fm("KCN-C4 RED: 列表态长按后 act=%d page=%d, 期望 COVER_BACK/COVER——"
               "「每按一次只上一层」被破（跨层直达首页）", act, v.page));

    /* 第 2 层：封皮长按 -> 交全局回首页（本层交出, 不越级改页）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KCN_ACT_GLOBAL && v.page == KCN_PAGE_COVER,
        kcn_fm("KCN-C4 RED: 封皮长按 act=%d page=%d, 期望 GLOBAL 且页态由全局层接管"
               "（封皮->首页）", act, v.page));

    /* tooth 5（结构源锚代理）: 板级 BOOT 钩子入口 reset。 */
    region = kcn_region_between("firmware/main/boards/waveshare-s3-rlcd-4.2/"
                                "waveshare-s3-rlcd-4.2.cc",
                                "case GLOBAL_ACT_PAGE_NEXT:", "case GLOBAL_ACT_OVERLAY_CLOSE:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KCN-C4 RED(infra): 板级 GLOBAL_ACT_PAGE_NEXT 分支锚漂移——须复核本断言锚");
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "StoryEnterCover") != NULL,
        "KCN-C4 RED: 板级 PAGE_NEXT 分支无 StoryEnterCover——封皮入口 reset 缺失"
        "（冷启动/异常退出后首翻 P4 见列表而非封皮; S-F2.6-AC1; human 2026-11-15 裁定）");
    free(region);
    TEST_ASSERT_TRUE_MESSAGE(
        kcn_file_contains("firmware/main/mcp_server.cc", "StoryEnterCover"),
        "KCN-C4 RED: mcp_server.cc 无 StoryEnterCover 定义——板级钩子无落地缝"
        "（S-F2.6-AC1）");
}

/* ========================================================================== *
 * TEST 5 — KCN-C5: restore 三态（S-F2.6-AC1 / 契约 §1.3③ D-07/D-08）
 *   tooth 1 行为（[C5] kidpal_story_list_load）: 只填缓存不导航——遍历活动子态
 *           {COVER/LIST/READER/STAMP/DELETE/RECORDING/ORGANIZING}, page 全不变;
 *           count/counter 装新值; cursor 钳制到 [0,count-1]。
 *   tooth 2 行为: NULL 入参 -> -1。
 *   tooth 3 行为: 畸形帧降级——kidpal_story_list_parse("{}")==-1、NULL==-1。
 *   tooth 4 结构: story 路由区改用 list_load + page=COVER, 且不再 view_init（旧实现
 *           无条件重装会把活动子态重置成 LIST）。
 * RED today: list_load weak 哨兵 -9999; 路由区仍 view_init。
 * ========================================================================== */
/* coverage: S-F2.6-AC1 -- restore 帧只填缓存不导航 / 非 restore 完整帧进封皮 */
/* coverage: S-F2.6-AC4 -- 语音「打开故事」= 非 restore 完整帧进封皮（不跳列表） */
/* coverage: S-F2.1-AC6 -- restore 只填缓存（冷启动恢复口径, 不抢前台） */
/* coverage: S-F2.6 -- restore 三态 */
TEST_CASE("story_cover_nav: restore 三态与缓存装载 (KCN-C5)",
          "[story_cover_nav][KCN-C5][behavior][structural]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    static const int pages[] = {
        KCN_PAGE_COVER, KSTORY_PAGE_LIST, KSTORY_PAGE_READER, KSTORY_PAGE_STAMP,
        KSTORY_PAGE_DELETE, KSTORY_PAGE_RECORDING, KSTORY_PAGE_ORGANIZING
    };
    int i;

    assert(KSTORY_LIST_MAX == 16);
    KCN_REQUIRE_ROOT();

    /* tooth 1: 活动子态零重置——缓存装载不动 page / rec 字段。 */
    for (i = 0; i < (int)(sizeof(pages) / sizeof(pages[0])); i++) {
        int rc;
        int keep_rec = 37 + i;
        int keep_sub = 1;
        memset(&l, 0, sizeof(l));
        l.total = 4;
        l.count = 2;
        snprintf(l.items[0].id, sizeof(l.items[0].id), "2026-08-20-x");
        snprintf(l.items[0].title, sizeof(l.items[0].title), "小兔子找朋友");
        snprintf(l.items[0].date, sizeof(l.items[0].date), "08-20");
        snprintf(l.items[1].id, sizeof(l.items[1].id), "2026-08-25-y");
        snprintf(l.items[1].title, sizeof(l.items[1].title), "勇敢的小明");
        snprintf(l.items[1].date, sizeof(l.items[1].date), "08-25");

        memset(&v, 0, sizeof(v));
        v.page = pages[i];
        v.cursor = 99;      /* 越界 -> load 后钳制 */
        v.count = 0;
        v.counter = -7;
        v.rec_seconds = keep_rec;
        v.rec_substate = keep_sub;
        v.stamp_serial = 42;

        rc = kidpal_story_list_load(&v, &l);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            kcn_fm("KCN-C5 RED: kidpal_story_list_load(page=%d) rc=%d——restore 缓存装载"
                   "缝未实现（[C5] 冻结签名, 哨兵 = 未实现）", pages[i], rc));
        TEST_ASSERT_TRUE_MESSAGE(v.page == pages[i],
            kcn_fm("KCN-C5 RED: list_load 改动了 page（%d -> %d）——restore 帧必须只填"
                   "缓存不导航、活动子态零重置（plan D; TECH-261022 churn 防线）",
                   pages[i], v.page));
        TEST_ASSERT_TRUE_MESSAGE(v.rec_seconds == keep_rec && v.rec_substate == keep_sub &&
                                     v.stamp_serial == 42,
            kcn_fm("KCN-C5 RED: list_load 触碰录音/子态/盖章字段（rec_seconds=%d "
                   "rec_substate=%d stamp_serial=%d）——活动子态零重置被破",
                   v.rec_seconds, v.rec_substate, v.stamp_serial));
        TEST_ASSERT_TRUE_MESSAGE(v.count == 2 && v.counter == 4,
            kcn_fm("KCN-C5 RED: list_load 后 count=%d counter=%d, 期望 2/4——缓存计数"
                   "只信 total（契约 §1.3③）", v.count, v.counter));
        TEST_ASSERT_TRUE_MESSAGE(v.cursor >= 0 && v.cursor < v.count,
            kcn_fm("KCN-C5 RED: list_load 后 cursor=%d 未钳制到 [0,%d)——restore 缓存"
                   "装载须收敛 cursor", v.cursor, v.count));
    }

    /* tooth 2: NULL 入参降级。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_load(NULL, &l) == -1,
        "KCN-C5 RED: list_load(NULL view) 未返回 -1——NULL 防御缺失（不盲装）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_load(&v, NULL) == -1,
        "KCN-C5 RED: list_load(NULL list) 未返回 -1——NULL 防御缺失（不盲装）");

    /* tooth 3: 畸形帧降级。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_parse("{}", &l) == -1,
        "KCN-C5 RED: 畸形 story_list data（{} 缺 items）未降级为 -1——V27 盲渲染防线破");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_parse(NULL, &l) == -1,
        "KCN-C5 RED: NULL story_list data 未降级为 -1");

    /* tooth 4（结构源锚代理）: 路由区改 list_load + page=COVER, 不再 view_init。 */
    {
        char *region = kcn_region_between("firmware/main/mcp_server.cc",
                                          "case VRT_ROUTE_STORY_LIST:",
                                          "case VRT_ROUTE_STORY_TEXT:");
        int view_init_n;
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "KCN-C5 RED(infra): story 路由区锚漂移——须复核本断言锚");
        TEST_ASSERT_TRUE_MESSAGE(strstr(region, "kidpal_story_list_load") != NULL,
            "KCN-C5 RED: story 路由区无 kidpal_story_list_load——restore 分流仍走"
            "kidpal_story_view_init 重装（活动子态被重置成 LIST; plan D）");
        TEST_ASSERT_TRUE_MESSAGE(strstr(region, "KSTORY_PAGE_COVER") != NULL,
            "KCN-C5 RED: story 路由区无 KSTORY_PAGE_COVER——非 restore 完整帧未进封皮"
            "（S-F2.6-AC4 语音「打开故事」直达列表 = 跳封皮）");
        view_init_n = kcn_count_in(region, "kidpal_story_view_init(");
        TEST_ASSERT_TRUE_MESSAGE(view_init_n == 0,
            kcn_fm("KCN-C5 RED: story 路由区仍调用 kidpal_story_view_init %d 处——"
                   "restore 必须从 view_init 重装改为只装缓存不导航（页态零重置）",
                   view_init_n));
        free(region);
    }
}

/* ========================================================================== *
 * TEST 6 — KCN-C9: 书封 / ic-coin 资产（D-09 / K5S-E3）
 *   tooth 1 ic_coin_52_52.bin 存在且 KAP1 头正确（magic/format/52x52/stride 7/380B）;
 *   tooth 2 命名律 = 生成器以 `<name>.bin` 落盘（符号名 + 尺寸后缀与文件头自洽）;
 *   tooth 3 产线白名单三处登记（gen_art_spiffs / gen_assets / 模拟器 assets.c）;
 *   tooth 4 KSTORY_ART_CACHE_SLOTS 扩容 >= 8（封皮资产与既有 7 枚并存不静默逐出）。
 * RED today: 资产缺、白名单缺项、slots=7。
 * ========================================================================== */
/* coverage: S-F2.6 -- 书封与 ic-coin 走既有资产管线（D-09） */
TEST_CASE("story_cover_nav: ic_coin 资产与产线白名单 (KCN-C9)",
          "[story_cover_nav][KCN-C9][asset][structural]")
{
    static const char *const rel = "firmware/main/assets/spiffs_root/ic_coin_52_52.bin";
    uint8_t *raw;
    size_t len = 0;
    int slots;

    assert(KSTORY_LIST_MAX == 16);
    KCN_REQUIRE_ROOT();

    /* tooth 1: 生成件存在 + KAP1 头自洽。 */
    raw = kcn_read_bin_rel(rel, &len);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw,
        "KCN-C9 RED: firmware/main/assets/spiffs_root/ic_coin_52_52.bin 缺失——"
        "封皮大星徽（ic-coin 52px）未走既有资产管线产出（D-09 / K5S-E3）");
    TEST_ASSERT_TRUE_MESSAGE(len == 380,
        kcn_fm("KCN-C9 RED: ic_coin_52_52.bin 大小 %u != 380（16B KAP1 头 + 52x7B 位图）",
               (unsigned)len));
    {
        unsigned magic = (unsigned)raw[0] | ((unsigned)raw[1] << 8) |
                         ((unsigned)raw[2] << 16) | ((unsigned)raw[3] << 24);
        unsigned format = (unsigned)raw[4] | ((unsigned)raw[5] << 8);
        unsigned w = (unsigned)raw[8] | ((unsigned)raw[9] << 8);
        unsigned h = (unsigned)raw[10] | ((unsigned)raw[11] << 8);
        unsigned stride = (unsigned)raw[12] | ((unsigned)raw[13] << 8);
        TEST_ASSERT_TRUE_MESSAGE(magic == 0x3150414bu,
            kcn_fm("KCN-C9 RED: ic_coin_52_52.bin KAP1 magic=0x%08x, 期望 0x3150414b——"
                   "非 gen_art_spiffs 产线生成件", magic));
        TEST_ASSERT_TRUE_MESSAGE(format == 1u && w == 52u && h == 52u && stride == 7u,
            kcn_fm("KCN-C9 RED: ic_coin_52_52.bin 头 (format=%u w=%u h=%u stride=%u), "
                   "期望 (1/52/52/7)——尺寸与命名律不符（V9 52px 口径）",
                   format, w, h, stride));
    }
    free(raw);

    /* tooth 2: 命名律 = 生成器以 <符号名>.bin 落盘，与文件头尺寸后缀 <w>_<h> 自洽
     * （产物名 = `<name>.bin`; 本条钉产线落盘律, 非文件路径自证）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kcn_file_contains("tools/asset-pipeline/gen_art_spiffs.mjs", "${name}.bin") &&
            strstr(rel, "ic_coin_52_52.bin") != NULL,
        "KCN-C9 RED: 生成件命名律偏移——gen_art_spiffs 仍应以 `${name}.bin` 落盘且产物名 = "
        "符号名（ic_coin_52_52.bin = 符号 ic_coin_52_52 + 尺寸 52x52）");

    /* tooth 3: 产线白名单三处登记。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kcn_file_contains("tools/asset-pipeline/gen_art_spiffs.mjs", "'ic_coin_52_52'"),
        "KCN-C9 RED: gen_art_spiffs.mjs WHITELIST 未追加 'ic_coin_52_52'——生成件无产线"
        "登记（纯追加 + 其余还原, K4 先例）");
    TEST_ASSERT_TRUE_MESSAGE(
        kcn_file_contains("tools/asset-pipeline/gen_assets.mjs", "ic_coin_52_52"),
        "KCN-C9 RED: gen_assets.mjs 无 ic_coin_52_52 job——job name = 产物符号名的命名律"
        "未落实（D-09）");
    TEST_ASSERT_TRUE_MESSAGE(
        kcn_file_contains("tools/simulator/LvglWindowsSimulator/assets/assets.c",
                          "ic_coin_52_52"),
        "KCN-C9 RED: 模拟器 assets.c 无 ic_coin_52_52 符号——生成件母版缺失");

    /* tooth 4: 资产缓存槽扩容（封皮资产与既有 7 枚并存）。 */
    slots = kcn_define_int("firmware/main/display/story_page.c",
                           "KSTORY_ART_CACHE_SLOTS");
    TEST_ASSERT_TRUE_MESSAGE(slots >= 8,
        kcn_fm("KCN-C9 RED: KSTORY_ART_CACHE_SLOTS = %d < 8——7 枚已被既有资产占满, "
               "封皮再加资产会静默不可用（plan F 风险点; 须扩容为纯逻辑常量）", slots));
}

/* ========================================================================== *
 * TEST 7 — KCN-C10: 回归锚（S-F2.1-AC5/AC6、S-F2.4-AC2、S-F2.2-AC1、S-F2.3-AC1/AC2、
 * K0a 阅读键）+ host 登记序守卫
 *   tooth 1 counter 只信 total（含 ×0 / items 顶替负锚）;
 *   tooth 2 删除同步减计数（现存篇数口径 S-F2.4-AC2）;
 *   tooth 3 K0a 阅读键零回归（READER KEY 短按零响应, 不落全局; key_owns 归属）;
 *   tooth 4 记录语境入口回归（列表仍 story; 本包只回归不承接 TECH-261028）;
 *   tooth 5 host 登记序（新验收件已登记且置于 smoke 之前）。
 * ========================================================================== */
/* coverage: S-F2.1-AC5 -- 计数 ×0 诚实计数（counter 只信 total） */
/* coverage: S-F2.1-AC6 -- 计数数据未达不显（counter 语义与 seen_data 分离承载于 C2） */
/* coverage: S-F2.4-AC2 -- 星星计数 = 现存篇数, 删除同步减 */
/* coverage: S-F2.2-AC1 -- 记录语境入口（列表仍 story）回归 */
/* coverage: S-F2.3-AC1 -- 阅读态进入回归 */
/* coverage: S-F2.3-AC2 -- 阅读 KEY 短按零响应（朗读已砍）回归 */
/* coverage: S-F2.1-AC1 -- 列表计数/行口径回归 */
/* coverage: S-F2.1-AC2 -- 列表手势回归（KEY 短=记录语境 / BOOT 短=下移） */
/* coverage: S-F2.1-AC3 -- 常驻语音入口（列表渲染真帧）回归 */
/* coverage: S-F2.1-AC4 -- 删除确认子态本地计数 −1 回归 */
/* coverage: S-F2.1-AC7 -- 正文取不回可见提示（既有覆盖层页态保留, 见负半注记） */
/* coverage: S-F2.2 -- 对话式引导记录入口回归 */
/* coverage: S-F2.3 -- 回看记录回归 */
/* coverage: S-F2.4-AC2 -- 删除同步减计数回归 */
/* coverage: S-F2.1 -- 看星星记录本列表回归锚 */
TEST_CASE("story_cover_nav: 回归锚+登记序 (KCN-C10)",
          "[story_cover_nav][KCN-C10][behavior][structural]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    char *src;
    int ci, c0, c2, c12;

    assert(KCN_PAGE_COVER == 7);
    KCN_REQUIRE_ROOT();

    /* tooth 1: counter 恒信 total（12 篇 vs 窗口 2 条; total=0 诚实 ×0）。 */
    memset(&l, 0, sizeof(l));
    l.total = 0;
    l.count = 0;
    c0 = kidpal_story_counter(&l);
    memset(&l, 0, sizeof(l));
    l.total = 2;
    l.count = 2;
    c2 = kidpal_story_counter(&l);
    memset(&l, 0, sizeof(l));
    l.total = 12;
    l.count = 2;
    c12 = kidpal_story_counter(&l);
    TEST_ASSERT_TRUE_MESSAGE(c0 == 0 && c2 == 2 && c12 == 12,
        kcn_fm("KCN-C10 RED: counter 语义漂移（total=0->%d / 2->%d / 12(items=2)->%d）"
               "——星星计数须恒信 story_list.total（契约 §1.3③ / S-F2.4-AC2 现存篇数）",
               c0, c2, c12));

    /* tooth 2: 删除确认本地即时减计数（现存篇数 −1）。 */
    kcn_make_list(&l, 3);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "KCN-C10 RED(infra): view_init 失败");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS) ==
            KCN_ACT_DELETE_ENTER && v.page == KSTORY_PAGE_DELETE,
        "KCN-C10 RED: BOOT 长按未进删除确认子态（S-F2.1-AC4 回归）");
    ci = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(ci == KCN_ACT_DELETE_CONFIRM && v.count == 2 && v.counter == 2 &&
                                 v.page == KSTORY_PAGE_LIST,
        kcn_fm("KCN-C10 RED: 删除确认后 act=%d count=%d counter=%d page=%d——现存篇数"
               "删除同步减缺失（S-F2.4-AC2 / S-F2.1-AC4）",
               ci, v.count, v.counter, v.page));

    /* tooth 3: K0a 阅读键零回归——READER KEY 短按 = NONE（hud 零响应）, 不落 SESSION_TOGGLE;
     * key_owns 归属判据仍认故事页（基层 P4 吞键）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "KCN-C10 RED(infra): view_init 失败");
    v.page = KSTORY_PAGE_READER;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK) == KCN_ACT_NONE,
        "KCN-C10 RED: READER KEY 短按非 NONE——阅读态朗读已砍（v1.52）, 须零响应"
        "（S-F2.3-AC2; 绝不再落全局 SESSION_TOGGLE = TECH-261028 回潮）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(KCN_LAYER_NONE, 3 /* KP_PAGE_STORY */,
                              KSTORY_PAGE_READER, 0) == 1,
        "KCN-C10 RED: key_owns(基层 P4, READER) != 1——阅读键归属判据回退"
        "（KEY 手势漏回全局路由 = TECH-261028 毁稿/误开会话面）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(KCN_LAYER_NONE, 3 /* KP_PAGE_STORY */,
                              KSTORY_PAGE_RECORDING, 1) == 1,
        "KCN-C10 RED: key_owns(记录语境录音态) != 1——录音态 KEY 归属回退（毁稿防线）");

    /* tooth 4: 记录语境入口回归——列表仍映射 source_page=story（本包只回归）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strcmp(kidpal_story_ctx_source_page(3 /* KP_PAGE_STORY */), "story") == 0,
        "KCN-C10 RED: kidpal_story_ctx_source_page(KP_PAGE_STORY) != \"story\"——"
        "列表记录语境回归（词表外溢/回退被破; [S1] 冻结签名零改）");

    /* tooth 5: host 登记序——新验收件已登记且置于 host_infra_smoke 之前。 */
    src = kcn_read_text_rel("firmware/main/test/host_app/main/CMakeLists.txt");
    TEST_ASSERT_NOT_NULL_MESSAGE(src,
        "KCN-C10 RED(infra): host_app/main/CMakeLists.txt 读取失败");
    {
        char *self = strstr(src, "story_cover_nav.acceptance.c");
        char *smoke = strstr(src, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(self != NULL,
            "KCN-C10 RED: CMakeLists 未登记 story_cover_nav.acceptance.c——冻结验收未进"
            "host runner（空过防线）");
        TEST_ASSERT_TRUE_MESSAGE(self != NULL && smoke != NULL && self < smoke,
            "KCN-C10 RED: story_cover_nav.acceptance.c 未置于 host_infra_smoke 之前——"
            "smoke 须保持末位/最先执行（包 0 注册序守卫）");
    }
    free(src);
}
