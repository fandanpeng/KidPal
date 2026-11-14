/*
 * reward_goal_config.acceptance.c — 冻结验收（TaskPackage 261104-k4-reward-goal-config，
 * firmware host 面）。
 *
 * 承载 story / 语义权威 = docs/requirements.md：
 *   S-F6.5-AC1/AC2（四预置场景美术 + 自定义目标通用标记图 sc-flag）
 *   S-F6.5-AC3（切换后设备端进度线场景标与刻度随之更新）
 *   S-F6.1-AC2（进度线起点/分值刻度/场景标/超目标同页结构）
 *   S-F6.1-AC3（兑换重画按新余额）与 S-F6.3-AC3（只认激活分值）的设备数据显示半。
 * 契约只读消费 = docs/contracts/interface-contract.md §1.3⑦（target 四键 / asset 闭集
 *   sc-choco、sc-movie、sc-wheel、sc-gift、sc-flag）与 docs/architecture.md §5.5.2
 *   （预栅格化 1bpp KAP1 资产管线）。
 *
 * device-outcome: K4G-L1
 * host-uncompilable-path: firmware/main/display/kidpal_render_node.cc
 *   —— 场景标真实绘制落点 = kidpal_render_node.cc（设备 TU，host 不可编；本文件以
 *   源锚结构齿近似 + 真机看屏终裁）。K4G-L1 = 真实 QQ 自定义/切换/删除 + 一轮真机
 *   场景标/刻度/大分值/restore 看屏（载体 device-round-evidence.md），不落自动断言。
 *
 * Scope of record（K4G-H1..H5 → 本文件 TEST_CASE；marker 形态对齐
 * .claude/scripts/acceptance-coverage-check.mjs 的 `coverage: <id>` 判据）:
 *
 *   coverage: K4G-H1 —— 五键 asset→bin 名映射全覆盖 + 未知标识返回 NULL 不猜图
 *                        （S-F6.5-AC1/AC2）：
 *     · TEST 1 "reward goal config: scene asset to bin name mapping (K4G-H1)"
 *   coverage: K4G-H2 —— 刻度伸缩（5 / 200 两档）+ 越目标版面
 *                        （S-F6.5-AC3 / S-F6.1-AC2）：
 *     · TEST 2 "reward goal config: progress scale stretch and overshoot (K4G-H2)"
 *   coverage: K4G-H3 —— KAP1 机检：四枚新 bin + sc_wheel 只读断言
 *                        （S-F6.5-AC1/AC2、arch §5.5.2）：
 *     · TEST 3 "reward goal config: KAP1 spiffs assets (K4G-H3)"
 *   coverage: K4G-H4 —— 出厂态回归：默认游乐场 40 / sc-wheel / 五逻辑标识可解析（additive）：
 *     · TEST 4 "reward goal config: factory target and asset set regression (K4G-H4)"
 *   coverage: K4G-H5 —— 接线/登记守卫：host 注册序 + seam 落点 + render_node 源锚
 *                        （S-F6.5-AC3）：
 *     · TEST 5 "reward goal config: host wiring and render node anchors (K4G-H5)"
 *
 * ========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement) — 对齐 plan.md「技术方案」A/B/E 节：
 *
 * [G1] display/points_page.c + points_page.h 新增（additive，K0/K2 形状零触碰）：
 *        const char *kidpal_points_scene_asset(const char *target_asset);
 *      五键映射（唯一单源）：
 *        "sc-choco" -> "sc_choco_42_42"   "sc-movie" -> "sc_movie_42_42"
 *        "sc-wheel" -> "sc_wheel_42_42"   "sc-gift"  -> "sc_gift_42_42"
 *        "sc-flag"  -> "sc_flag_42_42"
 *      未知 / NULL / 空串 -> NULL（绝不猜图；调用方不画场景标）。
 * [G2] display/points_page.c + points_page.h 新增刻度几何决策表：
 *        int kidpal_points_scale_x(int target_points, int points, int track_w, int *x_out);
 *      target_points <= 0 / track_w <= 0 / x_out == NULL -> -1（不画）；
 *      points <= 0 -> *x_out = 0；
 *      否则 *x_out = points * track_w / target_points（整数除法、向下取整）——
 *      points == target_points 恰落在 track_w（刻度画到目标分值）；points > target
 *      继续延伸到 track_w 之外（超目标实线越场景标，绘制裁切归 render_node/design）。
 *      成功 0。
 * [G3] display/kidpal_render_node.cc（设备 TU，host 不可编）：场景槽命中判据由
 *      `slot->name == asset` 指针相等改为 **strcmp 按名比较**，槽位名字面量
 *      `sc_wheel_42_42` 改为 **kidpal_points_scene_asset(frame->target_asset)** 结果，
 *      名字变化时重载槽（方案 A：3 槽 + 按名 strcmp 重载，RAM 零增长）。
 * [G4] tools/asset-pipeline：gen_assets.mjs sc 段追加 sc_choco/sc_movie/sc_gift/sc_flag
 *      四 job（42x42、rotate 0、kind 'svg'）；gen_art_spiffs.mjs WHITELIST 纯追加四名
 *      （sc_wheel_42_42 为 K2 已交付件，只读不改）。落盘 = spiffs_root/<name>.bin。
 * [G5] KAP1 冻结格式（arch §5.5.2 / gen_art_spiffs.mjs 头注）：offset0 u32 magic
 *      0x3150414B（'K''A''P''1'）/ offset4 u16 format=1 / offset6 u16 flags=0 /
 *      offset8 u16 w / offset10 u16 h / offset12 u16 stride=(w+7)/8 / offset14 u16
 *      reserved=0 / offset16 像素 stride*h 字节（MSB-first，bit=1 墨）。42x42 =>
 *      stride 6、data 252、文件长 268。
 *
 * 运行纪律：本文件运行时只读仓内文件（仓根经 KIDPAL_REPO_ROOT 或相对候选点注入，
 * 不依赖调用 cwd；acceptance-evidence「root 注入」节）。fixture 自含（全部字面
 * 帧 / 常量），无时间真值、无网络。**必须置于 host smoke acceptance 之前**（TEST 5
 * 校验注册序，登记行缺失即红）。
 *
 * 设计态红（冻结时点）：points_page.c/.h 无 [G1]/[G2] seam（weak 哨兵：映射 NULL /
 * scale -99）；spiffs_root 只有 sc_wheel_42_42.bin，四枚新 bin 缺席；render_node.cc
 * 仍是 `sc_wheel_42_42` 字面量 + 指针相等判据。TEST 4 为 must-stay-green 回归半
 * （默认游乐场 40 / 五逻辑标识解析 = K0/K2 既有语义，非空过）。
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-K4G-H1 场景标真实绘制（像素落位 / 旧槽逐出后不再出现的帧证据）= 设备 TU
 *                kidpal_render_node.cc，host 不可编 -> K4G-L1 真机看屏。
 *   GAP-K4G-H2 刻度像素间距/字号观感 = design review（本件只钉分值→x 几何与越目标
 *                数据面，不钉视觉打磨）。
 *   GAP-K4G-H3 RAM 资源预算（3 槽重载）实测 -> 真机轮证据。
 */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "points_page.h"

/* ------------------------------------------------------------------------ *
 * 冻结 seam 声明（未实现时由下方 weak 哨兵承载；builder 落地 display/points_page.c
 * 强符号覆盖）。
 * ------------------------------------------------------------------------ */

const char *kidpal_points_scene_asset(const char *target_asset);
int kidpal_points_scale_x(int target_points, int points, int track_w, int *x_out);

#define K4G_UNIMPLEMENTED (-99)

__attribute__((weak)) const char *kidpal_points_scene_asset(const char *target_asset)
{
    (void)target_asset;
    return (const char *)0;
}

__attribute__((weak)) int kidpal_points_scale_x(int target_points, int points,
                                                int track_w, int *x_out)
{
    (void)target_points;
    (void)points;
    (void)track_w;
    (void)x_out;
    return K4G_UNIMPLEMENTED;
}

/* ------------------------------------------------------------------------ *
 * 工具：失败消息 / 仓根定位 / 文件读取（自包含；命名前缀 k4g_；全 static）。
 * ------------------------------------------------------------------------ */

#define K4G_MAX_PATH 1024
#define K4G_MAX_READ (8u * 1024u * 1024u)

static char k4g_fm_buf[1024];
static const char *k4g_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *k4g_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(k4g_fm_buf, sizeof(k4g_fm_buf), fmt, ap);
    va_end(ap);
    return k4g_fm_buf;
}

static int k4g_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char k4g_root[K4G_MAX_PATH];
static int k4g_root_ready;

/* 仓根定位（acceptance-evidence root 注入纪律）：优先 KIDPAL_REPO_ROOT，其次相对候选点。 */
static int k4g_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[K4G_MAX_PATH];
    const char *env_root;
    size_t i;

    if (k4g_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (k4g_is_regular(probe)) {
            snprintf(k4g_root, sizeof(k4g_root), "%s", env_root);
            k4g_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (k4g_is_regular(probe)) {
            snprintf(k4g_root, sizeof(k4g_root), "%s", cands[i]);
            k4g_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

/* 读仓内文件（二进制安全；追加一个 '\0' 方便文本扫描，长度另出）。失败 NULL。 */
static unsigned char *k4g_read(const char *rel, size_t *out_len)
{
    char p[K4G_MAX_PATH * 2 + 2];
    FILE *f;
    long sz;
    unsigned char *buf;

    if (!k4g_locate_root()) {
        return NULL;
    }
    snprintf(p, sizeof(p), "%s/%s", k4g_root, rel);
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > K4G_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (unsigned char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* 逻辑守卫：host OOM 必须响亮失败，不得被读成「红」（K2 先例） */
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
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

#define K4G_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(k4g_locate_root(), \
    "K4G RED(infra): repo root not located — run from the repo root or " \
    "firmware/main/test/host_app, or pass KIDPAL_REPO_ROOT (root injection rule)")

static unsigned int k4g_u16le(const unsigned char *b, size_t off)
{
    return (unsigned int)b[off] | ((unsigned int)b[off + 1] << 8);
}

static unsigned long k4g_u32le(const unsigned char *b, size_t off)
{
    return (unsigned long)b[off] | ((unsigned long)b[off + 1] << 8) |
           ((unsigned long)b[off + 2] << 16) | ((unsigned long)b[off + 3] << 24);
}

/* 1bpp MSB-first 墨位计数（bit=1 墨）。越界回 -1。 */
static int k4g_ink_count(const unsigned char *b, size_t sz, int w, int h, int stride)
{
    int ink = 0;
    int x, y;

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            size_t off = (size_t)16 + (size_t)y * (size_t)stride + (size_t)(x >> 3);
            if (off >= sz) {
                return -1;
            }
            if ((b[off] & (unsigned char)(0x80u >> (x & 7))) != 0) {
                ink++;
            }
        }
    }
    return ink;
}

/* ========================================================================== *
 * TEST 1 — K4G-H1 五键 asset→bin 名映射 + 未知标识不猜图。
 * ========================================================================== */
TEST_CASE("reward goal config: scene asset to bin name mapping (K4G-H1)",
          "[reward_goal_config][K4G-H1][linux][behavior]")
{
    static const char *const pairs[][2] = {
        {"sc-choco", "sc_choco_42_42"},
        {"sc-movie", "sc_movie_42_42"},
        {"sc-wheel", "sc_wheel_42_42"},
        {"sc-gift",  "sc_gift_42_42"},
        {"sc-flag",  "sc_flag_42_42"},
    };
    size_t i;
    const char *got;

    /* 哨兵守卫：已知键必须已实现（NULL = [G1] 未落地）。 */
    TEST_ASSERT_NOT_NULL_MESSAGE(
        kidpal_points_scene_asset("sc-wheel"),
        "K4G-H1 RED: kidpal_points_scene_asset 未实现（weak 哨兵返回 NULL）——builder 落地 "
        "display/points_page.c [G1] 五键映射（sc-wheel -> sc_wheel_42_42）。");

    for (i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
        got = kidpal_points_scene_asset(pairs[i][0]);
        TEST_ASSERT_NOT_NULL_MESSAGE(
            got, k4g_fm("K4G-H1 RED: 逻辑标识 %s 无映射 —— 场景标将不画。", pairs[i][0]));
        TEST_ASSERT_EQUAL_STRING_MESSAGE(
            pairs[i][1], got,
            k4g_fm("K4G-H1 RED: %s 映射错（期望 %s，实得 %s）—— 条目名 = assets.h 符号名 "
                   "= <name>.bin 根名（gen_art_spiffs 命名律）。",
                   pairs[i][0], pairs[i][1], got));
    }

    /* 未知标识绝不猜图（含淘汰标识 sc-castle / 空串 / NULL）。 */
    TEST_ASSERT_NULL_MESSAGE(
        kidpal_points_scene_asset("sc-castle"),
        "K4G-H1 RED: 已淘汰标识 sc-castle 仍返回映射 —— 不得回用淘汰美术。");
    TEST_ASSERT_NULL_MESSAGE(
        kidpal_points_scene_asset("sc-unicorn"),
        "K4G-H1 RED: 未知标识返回了映射 —— 禁按奖励名猜图。");
    TEST_ASSERT_NULL_MESSAGE(
        kidpal_points_scene_asset(""),
        "K4G-H1 RED: 空串返回了映射 —— 调用方将画出无主场景标。");
    TEST_ASSERT_NULL_MESSAGE(
        kidpal_points_scene_asset(NULL),
        "K4G-H1 RED: NULL 返回了映射 —— 入参防御缺失。");
}

/* ========================================================================== *
 * TEST 2 — K4G-H2 刻度伸缩（5 / 200）+ 越目标版面（progress 决策表同页结构）。
 * ========================================================================== */
TEST_CASE("reward goal config: progress scale stretch and overshoot (K4G-H2)",
          "[reward_goal_config][K4G-H2][linux][behavior]")
{
    int x = -1;
    kidpal_points_home_t f;
    kidpal_points_progress_t pg;

    /* ---- [G2] 刻度几何 --------------------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_points_scale_x(5, 1, 10, &x) != K4G_UNIMPLEMENTED,
        "K4G-H2 RED: kidpal_points_scale_x 未实现（weak 哨兵 -99）—— 刻度伸缩无单源。");

    /* 5 分目标：刻度画到 5，坐标 = points * track / target。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(5, 3, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(120, x, "K4G-H2 RED: 5 分目标 3 分应落在 3/5 刻度位");
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(5, 5, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(200, x, "K4G-H2 RED: 达标点须恰落 track 末端（刻度画到目标）");
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(5, 0, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, x, "K4G-H2 RED: 零余额须落起点");
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(5, 7, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(280, x, "K4G-H2 RED: 超目标须越过 track 末端继续延伸（同页结构）");

    /* 200 分目标：同一把尺随目标伸缩（同余额比例不同 x）。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(200, 1, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, x, "K4G-H2 RED: 200 分目标 1 分刻度位错");
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(200, 200, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(200, x, "K4G-H2 RED: 200 分达标点须落 track 末端");
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_scale_x(200, 250, 200, &x));
    TEST_ASSERT_EQUAL_INT_MESSAGE(250, x, "K4G-H2 RED: 200 分目标 250 分须越标延伸");

    /* 边界：非法入参不画。 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_points_scale_x(0, 3, 200, &x));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_points_scale_x(-5, 3, 200, &x));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_points_scale_x(5, 3, 0, &x));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_points_scale_x(5, 3, 200, NULL));

    /* ---- 越目标版面数据面（K2 既有 kidpal_points_progress 回归） ---------- */
    memset(&f, 0, sizeof(f));
    f.revision = 1;
    f.balance = 250;
    f.target_points = 200;
    snprintf(f.target_name, sizeof(f.target_name), "%s", "礼物");
    snprintf(f.target_asset, sizeof(f.target_asset), "%s", "sc-gift");
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_progress(&f, &pg),
                                  "K4G-H2 RED: 200 分目标 progress 决策表失败");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.valid, "K4G-H2 RED: 越目标仍须可画");
    TEST_ASSERT_EQUAL_INT_MESSAGE(200, pg.target_points,
                                  "K4G-H2 RED: progress.target_points 未随激活目标（200）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(250, pg.pos, "K4G-H2 RED: 越目标小伴位置须越标");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.overshoot,
                                  "K4G-H2 RED: 超激活目标须标 overshoot（同页结构不变）");

    /* 5 分目标：小分值刻度下钳位语义不变。 */
    f.balance = 3;
    f.target_points = 5;
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_progress(&f, &pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, pg.pos, "K4G-H2 RED: 5 分目标 3 分位置错");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, pg.solid_len, "K4G-H2 RED: 实线段长错");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.overshoot, "K4G-H2 RED: 未达标不得标 overshoot");

    /* 负余额钳位（K2 回归；负值可见由 frame.balance 承载，本 seam 不画负位）。 */
    f.balance = -2;
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT(0, kidpal_points_progress(&f, &pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.pos, "K4G-H2 RED: 负余额须钳在起点");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.solid_len, "K4G-H2 RED: 负余额须全虚线");
}

/* ========================================================================== *
 * TEST 3 — K4G-H3 KAP1 机检（四枚新 bin + sc_wheel 只读）。
 * ========================================================================== */
TEST_CASE("reward goal config: KAP1 spiffs assets (K4G-H3)",
          "[reward_goal_config][K4G-H3][linux][behavior]")
{
    static const char *const bins[] = {
        "firmware/main/assets/spiffs_root/sc_choco_42_42.bin",
        "firmware/main/assets/spiffs_root/sc_movie_42_42.bin",
        "firmware/main/assets/spiffs_root/sc_wheel_42_42.bin",
        "firmware/main/assets/spiffs_root/sc_gift_42_42.bin",
        "firmware/main/assets/spiffs_root/sc_flag_42_42.bin",
    };
    size_t i;

    K4G_REQUIRE_ROOT();
    for (i = 0; i < sizeof(bins) / sizeof(bins[0]); i++) {
        unsigned char *b;
        size_t sz = 0;
        int ink;

        b = k4g_read(bins[i], &sz);
        TEST_ASSERT_NOT_NULL_MESSAGE(
            b, k4g_fm("K4G-H3 RED: %s 不在位 —— 资产管线未产该 KAP1 件。", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(268, (int)sz,
                                  k4g_fm("K4G-H3 RED: %s 文件长须 268（16 头 + 252 像素）；"
                                         "实得 %d", bins[i], (int)sz));
        TEST_ASSERT_EQUAL_MESSAGE(0x4Bu, (int)b[0], k4g_fm("K4G-H3 RED: %s magic 'K' 坏", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0x41u, (int)b[1], k4g_fm("K4G-H3 RED: %s magic 'A' 坏", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0x50u, (int)b[2], k4g_fm("K4G-H3 RED: %s magic 'P' 坏", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0x31u, (int)b[3], k4g_fm("K4G-H3 RED: %s magic '1' 坏", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0x3150414BUL, k4g_u32le(b, 0),
                                  k4g_fm("K4G-H3 RED: %s magic u32le 非 0x3150414B", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(1u, k4g_u16le(b, 4),
                                  k4g_fm("K4G-H3 RED: %s format 非 1bpp", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0u, k4g_u16le(b, 6), k4g_fm("K4G-H3 RED: %s flags 非 0", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(42u, k4g_u16le(b, 8), k4g_fm("K4G-H3 RED: %s w 非 42", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(42u, k4g_u16le(b, 10), k4g_fm("K4G-H3 RED: %s h 非 42", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(6u, k4g_u16le(b, 12), k4g_fm("K4G-H3 RED: %s stride 非 6", bins[i]));
        TEST_ASSERT_EQUAL_MESSAGE(0u, k4g_u16le(b, 14),
                                  k4g_fm("K4G-H3 RED: %s reserved 非 0", bins[i]));
        ink = k4g_ink_count(b, sz, 42, 42, 6);
        TEST_ASSERT_TRUE_MESSAGE(
            ink > 0 && ink < 42 * 42,
            k4g_fm("K4G-H3 RED: %s 墨位计数 %d 越界 —— 全空/全墨件即阈值化产线坏了。",
                   bins[i], ink));
        free(b);
    }
}

/* ========================================================================== *
 * TEST 4 — K4G-H4 出厂态回归（additive；must-stay-green）。
 * ========================================================================== */
TEST_CASE("reward goal config: factory target and asset set regression (K4G-H4)",
          "[reward_goal_config][K4G-H4][linux][behavior]")
{
    static const char *const assets[] = {
        "sc-choco", "sc-movie", "sc-wheel", "sc-gift", "sc-flag",
    };
    kidpal_points_home_t f;
    char json[768];
    size_t i;

    /* 出厂样本帧（K0 冻结件同形）：默认游乐场 40 / sc-wheel。 */
    snprintf(json, sizeof(json),
             "{\"revision\": 12, \"balance\": 23, "
             "\"target\": {\"id\": \"preset-playground\", \"name\": \"游乐场\", "
             "\"points\": 40, \"asset\": \"sc-wheel\"}, "
             "\"recent\": [{\"id\": \"pt_01Jabcd\", \"delta\": 5, "
             "\"reason\": \"今天扫地\", \"occurred_at\": \"2026-11-04T09:00:00+08:00\", "
             "\"balance_before\": 18, \"balance_after\": 23}]}");
    memset(&f, 0, sizeof(f));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_home_parse(json, &f),
                                  "K4G-H4 RED: 出厂帧解析失败 —— K0/K2 语义回归破坏");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("游乐场", f.target_name,
                                     "K4G-H4 RED: 出厂默认目标名漂移（须 游乐场）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(40, f.target_points,
                                  "K4G-H4 RED: 出厂默认目标分值漂移（须 40）");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("sc-wheel", f.target_asset,
                                     "K4G-H4 RED: 出厂默认场景标识漂移（须 sc-wheel）");

    /* 五逻辑标识均可入 f.target_asset（cap 16 零偏移），并各自可映射到 bin 名。 */
    for (i = 0; i < sizeof(assets) / sizeof(assets[0]); i++) {
        snprintf(json, sizeof(json),
                 "{\"revision\": 1, \"balance\": 0, "
                 "\"target\": {\"id\": \"preset-playground\", \"name\": \"游乐场\", "
                 "\"points\": 40, \"asset\": \"%s\"}, \"recent\": []}",
                 assets[i]);
        memset(&f, 0, sizeof(f));
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            0, kidpal_points_home_parse(json, &f),
            k4g_fm("K4G-H4 RED: asset 闭集标识 %s 解析失败 —— 契约 §1.3⑦ 闭集漂移。", assets[i]));
        TEST_ASSERT_EQUAL_STRING_MESSAGE(
            assets[i], f.target_asset,
            k4g_fm("K4G-H4 RED: asset 透传失真（期望 %s，实得 %s）。", assets[i], f.target_asset));
        TEST_ASSERT_NOT_NULL_MESSAGE(
            kidpal_points_scene_asset(assets[i]),
            k4g_fm("K4G-H4 RED: 闭集标识 %s 无 bin 映射 —— 五键映射不完备。", assets[i]));
    }
}

/* ========================================================================== *
 * TEST 5 — K4G-H5 接线/登记守卫（结构齿；源锚代理）。
 * ========================================================================== */
TEST_CASE("reward goal config: host wiring and render node anchors (K4G-H5)",
          "[reward_goal_config][K4G-H5][linux][structure]")
{
    unsigned char *cmake;
    unsigned char *hdr;
    unsigned char *impl;
    unsigned char *rn;
    size_t len = 0;
    char *pos_self;
    char *pos_smoke;

    K4G_REQUIRE_ROOT();

    /* ---- host 注册序（本件必须在 smoke 之前） ------------------------------ */
    cmake = k4g_read("firmware/main/test/host_app/main/CMakeLists.txt", &len);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "K4G-H5 RED(infra): host CMakeLists 不可读");
    pos_self = strstr((const char *)cmake, "reward_goal_config.acceptance.c");
    pos_smoke = strstr((const char *)cmake, "host_infra_smoke.acceptance.c");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        pos_self, "K4G-H5 RED: reward_goal_config.acceptance.c 未登记进 HOST_ACCEPTANCE_SRCS"
                  "（登记行缺失即红）。");
    TEST_ASSERT_NOT_NULL_MESSAGE(pos_smoke, "K4G-H5 RED(infra): smoke 登记行不存在");
    TEST_ASSERT_TRUE_MESSAGE(pos_self < pos_smoke,
                             "K4G-H5 RED: 本件须置于 host smoke acceptance 之前（smoke 恒末位）。");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)cmake, "display/points_page.c"),
        "K4G-H5 RED: display/points_page.c 强符号源不在册 —— 弱哨兵不被覆盖。");
    free(cmake);

    /* ---- seam 落点：头文件声明 + 实现在 host 可编文件 ---------------------- */
    hdr = k4g_read("firmware/main/display/points_page.h", &len);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "K4G-H5 RED(infra): points_page.h 不可读");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)hdr, "kidpal_points_scene_asset"),
        "K4G-H5 RED: points_page.h 未声明 kidpal_points_scene_asset（[G1] 单源落点）。");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)hdr, "kidpal_points_scale_x"),
        "K4G-H5 RED: points_page.h 未声明 kidpal_points_scale_x（[G2] 单源落点）。");
    free(hdr);

    impl = k4g_read("firmware/main/display/points_page.c", &len);
    TEST_ASSERT_NOT_NULL_MESSAGE(impl, "K4G-H5 RED(infra): points_page.c 不可读");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)impl, "kidpal_points_scene_asset"),
        "K4G-H5 RED: points_page.c 未实现 kidpal_points_scene_asset（映射须下沉 host 可编面）。");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)impl, "kidpal_points_scale_x"),
        "K4G-H5 RED: points_page.c 未实现 kidpal_points_scale_x（刻度几何须下沉 host 可编面）。");
    free(impl);

    /* ---- 设备 TU 源锚（host 不可编的精确代理） ----------------------------- */
    rn = k4g_read("firmware/main/display/kidpal_render_node.cc", &len);
    TEST_ASSERT_NOT_NULL_MESSAGE(rn, "K4G-H5 RED(infra): kidpal_render_node.cc 不可读");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)rn, "kidpal_points_scene_asset"),
        "K4G-H5 RED: kidpal_render_node.cc 仍用 sc_wheel_42_42 字面量 —— 场景槽须改为 "
        "kidpal_points_scene_asset(frame->target_asset) 映射结果（切目标场景标才更新）。");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr((const char *)rn, "strcmp"),
        "K4G-H5 RED: kidpal_render_node.cc 场景槽仍按指针相等命中 —— 须改 strcmp 按名比较"
        "（方案 A：按名重载，RAM 零增长）。");
    TEST_ASSERT_NULL_MESSAGE(
        strstr((const char *)rn, "slot->name == asset"),
        "K4G-H5 RED: kidpal_render_node.cc 仍存在 `slot->name == asset` 指针相等判据 —— "
        "换目标后旧槽命中判据不会失效。");
    free(rn);
}