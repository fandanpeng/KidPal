/*
 * Frozen acceptance tests — TaskPackage 260921-m3-k4-p1-cjk-dynamic-text (subsystem:
 * firmware; M3-K4 P1; 首页 P1 动态日程名 CJK 渲染 · mergeOrder 36 · rigor=lite /
 * riskTier=medium).
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」。首页 P1 动态日程名（课程名/事项名，开放
 * 集合 CJK 文本）在「接下来」卡、后续事项行、明日预告行正常渲染（14px 普通字体，设备
 * font_puhui_14_1 / host Source Han 子集双分支），从「画不出即丢弃」变为「画得出、
 * 超宽截断不越区」。技术方案权威 = delivery/taskpackages/260921-m3-k4-p1-cjk-dynamic-text/
 * plan.md（弱符号字形 seam + 设备/host 双强符号 + UTF-8 解码逐字形 blit）。
 *
 * 现状差距（home_p1.c 在世证据）：p1_draw_next / p1_draw_nextlist / p1_draw_preview
 * 只画 time 数字与 art 图标，name 无渲染路径（「首项名中文无 imgfont -> 殿后清单」注释
 * 在世）——K1 热修轮 + K3 真机轮两轮目击：time（ASCII）可画、CJK 课程名缺失。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；本文件 weak 哨兵在实现
 * 接线进 host runner 后被强定义覆盖。多重 weak 合法、语义一致——schedule_grid
 * 先例；「home =」为冻结落点）:
 *
 *   [S1] 弱符号字形获取 seam（home = firmware/main/display/home_p1.c 弱默认 +
 *        firmware/main/display/kidpal_render_node.cc 设备强符号 + firmware/main/
 *        test/host_app/main/host_infra_seam.c host 强符号）:
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
 *       设备强符号 = kidpal_render_node.cc：ksg_text_font()（schedule_page.c 先例）
 *       同款双分支取 font_puhui_14_1，lv_font_get_glyph_dsc + 字形位图
 *       （A1/A4/A8 alpha 阈值化成 1bpp 墨）。host 强符号 = host_infra_seam.c：
 *       lv_font_source_han_sans_sc_14_cjk 子集同款路径（host_app REQUIRES lvgl
 *       已在，字体已开 CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK）。
 *       生产侧声明头（落点由 builder 定，可在既有头 additive 追加或新头）必须
 *       与本文件本地 kidpal_p1_glyph_t 布局逐字段一致（本文件按 C 符号名链接）。
 *
 *   [S2] home_p1.c 内新增：UTF-8 解码（纯逻辑）+ name 逐字形 blit（复用既有钉区
 *        clip / 固定起笔纪律——起笔 x 不随 name 字节数平移），落位进三钉区（[S3]）。
 *        ASCII time 维持现状（p1_blit_digits / p1_blit_str_scaled 路径零偏移）。
 *
 *   [S3] 布局钉区域矩形（含端点；布局在位钉，非字体/颜色钉——纯美学留 design
 *        review；与 p1_home_battery.acceptance.c HPB 区钉 / home_p1.c r4 裁定一致）:
 *         PCN_RECT_NEXT     {136, 48, 392, 168}  「接下来」最近一项（主体）
 *         PCN_RECT_NEXTLIST {136,172, 392, 214}  后续事项逐项（小一号）
 *         PCN_RECT_PREVIEW  {136,220, 392, 242}  明日预告（r4 底缘 242 口径）
 *        name 墨必须落在对应钉区内、绝不越区（含右界 x=392——截断不越右界）。
 *
 *   [S4] UTF-8 截半尾序列容错（home_upcoming.c:393–396 数据已定形状）：home_upcoming
 *        按字节把 name 截到 31 字节（HP1_NAME_LEN-1），会把 UTF-8 多字节字符截半
 *        （31 字节 ≈ 10 个 CJK 字符 + 1 个前导字节）。home_upcoming.{c,h} 本包零改，
 *        容错解法放 P1 侧：解码器遇不完整尾序列安全停笔——截半字符不画、不越界、
 *        不崩。本文件 TEST 3 以 hp1_item_t 直注该字节形状（行为等价假设见强度申报）。
 *
 * ---------------------------------------------------------------------------
 * 契约条目 ↔ 齿 的强度申报（acceptance-evidence.md「契约条目 ↔ 齿 的强度申报」节
 * 体例；消费方 = integrator 与判官按本头注核）:
 *
 *   P1CJK-A1（name 墨落钉区）
 *     -> TEST 1 承载：行为级（真帧 diff 定位——name 像素差必须非零且全部落在对应
 *        钉区带内，diff 对 = 仅 name 文本不同、字节等长），销账「画不出即丢弃」。
 *     -> TEST 6 tooth 1/2/3 承载：机制级（seam 三符号落点 = home_p1.c 弱默认+调用
 *        点 / host_infra_seam.c host 强符号 / kidpal_render_node.cc 设备强符号 +
 *        font_puhui_14_1）。
 *   P1CJK-A2（超宽截断不越右界）
 *     -> TEST 2 承载：行为级（最长合法 name 真帧 + 右界带 x[393..399] 严格零墨 +
 *        diff 锁区）。
 *     弱于契约处显式声明：截断笔迹是否真达右界依赖实现起笔 x 与字形 advance（起笔
 *        x 不冻结——布局在位钉纪律）。fixture 取 31 字形 ASCII 极宽名（31 x 'W'，
 *        逐字节构造），自任意区内起笔均触达 x>392；若 host 子集 ASCII 字形缺场
 *        （差集断言会带自诊断红名指认），fixture 降级为 CJK 长名、触达性减弱——
 *        该缺口在此显式登记，真机轮（P1CJK-A6）目测补位。
 *   P1CJK-A3（空日程/单项各「零墨」pin 回归守卫）
 *     -> 空日程半 = 既有 74 齿 p1_home_battery.acceptance.c TEST 1 tooth 5
 *        （空日程 next 区严格零墨），must-stay-green，本文件不重复造齿。
 *     -> 单项半 = 既有 74 齿**未覆盖**（HPB 夹具只有 0/2 项两态），本文件 TEST 5
 *        补 additive 守卫齿：单项时 NEXTLIST / PREVIEW 区严格零墨（现状即绿）。
 *   P1CJK-A4（UTF-8 截半安全停笔）
 *     -> TEST 3 承载：行为级（截半帧 == 完整 30 字节帧 diff==0 + 右界带零墨 +
 *        rc==0 不崩）。
 *     行为等价假设显式化：本齿以 hp1_item_t 直注 31 字节截半名，等价于
 *        home_upcoming.c:393–396 按字节截 31 字节的产物形状；等价假设 = 「seam 收到
 *        的 name 即该字节形状」——home_upcoming 本包零改，本齿不经过 home_upcoming
 *        调用链，该等价由 K2 冻结齿（schedule_grid.acceptance.c M3K2-A5 对
 *        home_upcoming 解析/safe-truncate 的行为齿）间接背书。
 *   P1CJK-A5（字形缺失退化不崩）
 *     -> TEST 4 承载：行为级（缺字形字符零墨、不画垃圾、不崩 + probe 自检）。
 *     弱于契约处显式声明：缺字形后 skip-vs-stop（续笔 vs 停笔）不钉（不超约束）；
 *        全缝不可用（设备无强符号/未实现）路径在 host 不可达，其退化语义由基线
 *        红态（本文件 TEST 1-4 RED = 全部 name 不画、帧照常）+ TEST 6 结构齿承载。
 *   P1CJK-A6（device build 闸 + 真机目测挂设备轮窗口）
 *     -> **不是 host 齿**（不造假齿）：冻结为闸证据条目，见本文件尾「P1CJK-A6
 *        闸证据条目（冻结文本）」；device 强符号装配事实半由 TEST 6 tooth 3 承载
 *        （M3K2-A7 结构半先例同款）。
 *   源锚代理档自陈：TEST 6 tooth 1/2/4 为源锚代理（符号/注册文本存在性，非行为
 *        本体）；其行为本体由 TEST 1-4 兑现，两档并列互证。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: P1CJK-A1 name 墨落钉区（pixel-probe diff: next/nextlist/preview 三区
 *             各自落墨 + 不越区）-> TEST 1（行为半）+ TEST 6 tooth 1/2/3（机制半）
 *   coverage: P1CJK-A2 超宽 name 截断不越钉区右界 -> TEST 2
 *   coverage: P1CJK-A3 空日程/单项各「零墨」pin 回归守卫 -> 空日程半 = 既有 74 齿
 *             （HPB TEST 1 tooth 5, must-stay-green）；单项半 = TEST 5（additive
 *             守卫, 基线绿）
 *   coverage: P1CJK-A4 UTF-8 截半尾序列安全停笔 -> TEST 3
 *   coverage: P1CJK-A5 字形缺失（弱默认路径）退化不崩 -> TEST 4
 *   coverage: P1CJK-A6 device build 闸 + 真机目测挂设备轮窗口 -> 无 host 齿（闸
 *             证据条目冻结文本见文件尾）+ TEST 6 tooth 3（device 强符号装配事实半）
 *   coverage: P1CJK-E2 铁律 canary（hp1_item_t 与 11 个冻结契约函数零偏移 +
 *             home_upcoming 零改语义 + 契约/需求面在世）-> TEST 5 canary 半
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) firmware/main/display/home_p1.c：落 [S1] 弱默认（恒不可用）+ [S2] UTF-8
 *      解码与 name 逐字形 blit（复用钉区 clip / 固定起笔纪律；ASCII time 路径
 *      零偏移；home_p1.h 冻结面零偏移——hp1_item_t 与 11 个冻结契约函数不动）。
 *   2) firmware/main/display/kidpal_render_node.cc：落 [S1] 设备强符号（双分支取
 *      font_puhui_14_1，lv_font_get_glyph_dsc + alpha 阈值化 1bpp）。
 *   3) firmware/main/test/host_app/main/host_infra_seam.c：落 [S1] host 强符号
 *      （lv_font_source_han_sans_sc_14_cjk 子集）。
 *   4) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../p1_cjk_name.acceptance.c"，置于
 *      "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行）。
 *      home_p1.c / schedule_page.c / home_upcoming.c 已在册，零新增实现接线。
 *   5) mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *      子串**（KR-A3 教训——strstr 子串仍命中则变异不红；用 _STUB 类整词替换）；
 *      counterEvidence 红名建议见 taskpackage.mutationEvidence（acceptance-author
 *      随冻结报告回填）。
 *   6) device build 闸（P1CJK-A6）：xiaozhi.bin 构建过 + 体积余量记录（无新增大
 *      静态量，帧缓冲不增——s_p1_fb 既有 PSRAM 面零偏移）；真机目测挂设备轮窗口。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-
 *      k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *      layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *      （或 cd firmware/main/test/host_app 后 ./build/host_infra_test.elf——结构齿
 *      root 自手持 cwd 定位，两处均可，HPB/KSG 先例同款）
 *   Freeze-phase red baseline（本文件冻结时点, builder 未接线）: 见文件尾
 *      「RED baseline 判读口径」段（独立 stub 脚手架 red-baseline/pcn_red_seam_stub.c
 *      + runner.c + unity.h + 在世 home_p1.c 当场编译执行）。
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

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes / regions（seam [S3]；与 p1_home_battery HPB 区钉 /
 * home_p1.c r4 裁定一致，不得偏移）
 * ------------------------------------------------------------------------ */
#define PCN_FRAME_W 400
#define PCN_FRAME_H 300
#define PCN_INK_WHITE 0xFFFFFFul
#define PCN_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 「有墨」钉区最小墨像素数（next/nextlist/preview 区与 HPB 同档）。 */
#define PCN_INK_MIN_ELEM 8

/* 区域矩形 {x1, y1, x2, y2} 含端点（seam [S3] 布局在位钉）。 */
static const int pcn_rect_next[4]     = {136,  48, 392, 168};
static const int pcn_rect_nextlist[4] = {136, 172, 392, 214};
static const int pcn_rect_preview[4]  = {136, 220, 392, 242};

/* 右界越界检查带：钉区右界之外 x[393..399] x 各区行带（帧右缘 7px，任何 name 墨
 * 越右界都会落进本带——P1CJK-A2 的边界证据带）。 */
static const int pcn_band_right_x0 = 393;
static const int pcn_band_right_x1 = 399;

/* 被扫文件（seam [S1] 三符号落点 + 注册面 + 铁律 canary 面）。 */
#define PCN_HOME_P1_C   "firmware/main/display/home_p1.c"
#define PCN_HOME_P1_H   "firmware/main/display/home_p1.h"
#define PCN_UPCOMING_C  "firmware/main/display/home_upcoming.c"
#define PCN_RN_CC       "firmware/main/display/kidpal_render_node.cc"
#define PCN_SEAM_C      "firmware/main/test/host_app/main/host_infra_seam.c"
#define PCN_HOST_CMAKE  "firmware/main/test/host_app/main/CMakeLists.txt"

/* -------------------------------------------------------------------------- *
 * Seam [S1] — FROZEN OFFLINE CONTRACT 弱哨兵（强定义落地后覆盖；多重 weak 合法，
 * schedule_grid 先例）。冻结相位（home_p1.c 未落弱默认 / host seam 未落强符号）
 * 本哨兵返回非 0 = 字形不可用 -> name 全部不画 -> TEST 1/2/3/4 的「name 在场」
 * 断言红 = 设计态红（frozen contract not implemented），绝非链接/编译错误。
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
static int32_t pcn_f_d[PCN_FRAME_W * PCN_FRAME_H];

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

/* 右界越界带墨计数：rect 行带 x x[393..399]（P1CJK-A2 边界证据带）。 */
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

/* 注入 2 项今日日程 + 次日预告（name 由调用方给定；time 维持 HPB 夹具口径）。 */
static void pcn_inject_full(const char *n0, const char *n1, const char *pv_name)
{
    hp1_item_t items[2];
    hp1_item_t pv;
    int rc;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    pcn_copy_name(items[0].name, sizeof(items[0].name), n0);
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    pcn_copy_name(items[1].name, sizeof(items[1].name), n1);
    memset(&pv, 0, sizeof(pv));
    snprintf(pv.time, sizeof(pv.time), "09:00");
    pcn_copy_name(pv.name, sizeof(pv.name), pv_name);

    rc = kidpal_home_p1_schedule_inject("2026-09-22", items, 2, &pv);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        pcn_fm("PCN RED(infra): schedule_inject fixture rejected (rc=%d)——合法夹具"
               "被拒，渲染面无法驱动", rc));
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
 * name——P1CJK-A2 fixture；HP1_NAME_LEN=32 恰容 31 字节 + NUL）。 */
static void pcn_make_long_ascii(char *buf)
{
    memset(buf, 'W', 31);
    buf[31] = '\0';
}

/* 31 字节截半名：10 个 CJK 字符（30 字节）+ 第 11 个字符的 UTF-8 前导字节——
 * home_upcoming.c:393–396 按字节截 31 字节对 11 字 CJK 名的产物形状（seam [S4]）。 */
static const char *PCN_NAME_FULL10 = "一二三四五六七八九十";
static const char *PCN_NAME_TRUNC_TAIL = "\xE9";

/* -------------------------------------------------------------------------- *
 * TEST 1 — P1CJK-A1 行为半：name 墨落钉区（diff 定位）
 * 三对字节等长 name（仅 name 文本不同）各自渲染，diff 像素必须非零且全部落在
 * 对应钉区带内——(a) name 真被画（「画不出即丢弃」销账，基线必红）；(b) name 墨
 * 不越区（P1CJK-A1 的「落对应钉区」判别）。
 * RED today: kidpal_p1_font_glyph 弱哨兵不可用 -> name 全部不画 -> diff == 0。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A1 -- name 墨落钉区：next/nextlist/preview 三区 diff 定位（仅 name 不同、字节等长）+ 不越区 */
TEST_CASE("P1CJK name: 墨落三钉区 diff 定位 (P1CJK-A1)",
          "[p1_cjk_name][P1CJK-A1][behavior]")
{
    int total, in_band;

    pcn_set_fixture();

    /* 基准帧 A：钢琴课 / 英语培训 / 春游 */
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered — 探针缝断裂");

    /* tooth 1 (A1-next): 仅换 item0 name（钢琴课 -> 游泳课, 9 字节等长）——
     * diff 必须非零且全部落在 next 钉区 {136,48,392,168}。 */
    pcn_inject_full("游泳课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_next, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换 name 帧差为零——日程 name 未被渲染（「画不出即丢弃」现状"
        "在世, P1CJK-A1 销账未落地; 弱哨兵不可用 = 设计态红, builder 落 seam 后转绿）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: next 区 name 墨差 %d/%d 落在钉区带外——name 墨越出 "
               "{136,48,392,168}（[S3] 布局在位钉）", total - in_band, total));

    /* tooth 2 (A1-nextlist): 仅换 item1 name（英语培训 -> 美术书法, 12 字节等长）
     * —— diff 全部落在 nextlist 钉区 {136,172,392,214}。 */
    pcn_inject_full("钢琴课", "美术书法", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_c) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_c, pcn_rect_nextlist, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换后续事项 name 帧差为零——nextlist 行 name 未被渲染（A1 三区 "
        "承载缺一）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: nextlist 区 name 墨差 %d/%d 落在钉区带外", total - in_band,
               total));

    /* tooth 3 (A1-preview): 仅换 preview name（春游 -> 秋游, 6 字节等长）——
     * diff 全部落在 preview 钉区 {136,220,392,242}。 */
    pcn_inject_full("钢琴课", "英语培训", "秋游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_d) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_d, pcn_rect_preview, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A1 RED: 换预告 name 帧差为零——明日预告 name 未被渲染（A1 三区承载缺一）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        pcn_fm("PCN-A1 RED: preview 区 name 墨差 %d/%d 落在钉区带外", total - in_band,
               total));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — P1CJK-A2：超宽 name 截断不越钉区右界
 * 最长合法 name（31 字节 = HP1_NAME_LEN-1，home_upcoming safe-truncate 上界）：
 * (a) 渲染不崩；(b) 三区行带 x[393..399] 右界带严格零墨（截断必须按字形 clip 在
 * 区内，绝不越 x=392）；(c) 长名 vs 短名 diff 非零且全部落在三钉区带并集（长名
 * 真被画 + 不画到区外任何地方）。
 * RED today: name 不画 -> (c) diff == 0。右界带零墨 (b) 基线即绿（守卫半）。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A2 -- 超宽 name 截断不越钉区右界：31 字节极宽名右界带零墨 + diff 锁三区并集 */
TEST_CASE("P1CJK name: 超宽截断不越右界 (P1CJK-A2)",
          "[p1_cjk_name][P1CJK-A2][behavior]")
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

    /* tooth 1 (A2 右界带守卫): 三区行带 x[393..399] 严格零墨——截断 clip 纪律的
     * 边界证据（基线即绿；实现让墨越 x=392 即红）。 */
    ink = pcn_right_band_ink(pcn_rect_next, pcn_f_a);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A2 RED: next 区行带右界带 x[393..399] 墨 %d != 0——超宽 name "
               "截断越出钉区右界（[S3] clip 纪律被突破）", ink));
    ink = pcn_right_band_ink(pcn_rect_nextlist, pcn_f_a);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A2 RED: nextlist 区行带右界带墨 %d != 0——截断越右界", ink));
    ink = pcn_right_band_ink(pcn_rect_preview, pcn_f_a);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A2 RED: preview 区行带右界带墨 %d != 0——截断越右界", ink));

    /* tooth 2 (A2 长名在场 + 锁区): 长名帧 vs 短名帧（钢琴课/英语培训/春游）——
     * diff 非零且全部落在三钉区并集。RED today: name 不画 -> diff == 0。 */
    pcn_inject_full("钢琴课", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    {
        int b1, b2, b3, unused;
        /* 单次整帧 diff 计数 D：pcn_diff_stats 的 out_total 是整帧 diff 总数
         * （非区带内计数），三次调用各自的 out_total 相同，取一次即可；
         * in_band = 三区带内计数求和（三钉区不相交，和 = 并集内 diff 数）。
         * 断言 in_band == D = 全部 diff 落在钉区并集内。
         * （REVISION 2026-09-22: 修复冻结件算术缺陷——旧式 total=t1+t2+t3
         * 实为 3×整帧计数，断言 D==3D 与 total>0 互斥，见 REVISION LOG。） */
        pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_next, &total, &b1);
        pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_nextlist, &unused, &b2);
        pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_preview, &unused, &b3);
        in_band = b1 + b2 + b3;
        TEST_ASSERT_TRUE_MESSAGE(total > 0,
            "PCN-A2 RED: 超长 name 帧差为零——name 未被渲染（设计态红: 弱哨兵不可用; "
            "或 fixture 字形在 host 子集缺场——若 builder 落地后仍红, 先核 host "
            "Source Han 子集 ASCII/CJK 覆盖再判真红）");
        TEST_ASSERT_TRUE_MESSAGE(in_band == total,
            pcn_fm("PCN-A2 RED: 超长 name 墨差 %d/%d 落在三钉区并集之外——name 画到"
                   "钉区以外（越区非截断）", total - in_band, total));
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — P1CJK-A4：UTF-8 截半尾序列安全停笔
 * fixture = home_upcoming.c:393–396 对 11 字 CJK 名按字节截 31 字节的产物形状
 * （30 字节完整 10 字 + 1 个 UTF-8 前导字节，seam [S4]；home_upcoming 本包零改，
 * 本齿以 hp1_item_t 直注该字节形状——等价假设见头注强度申报）。
 * (a) name 在场（完整 30 字节名 diff 非零, 基线必红）；
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
     * —— diff 非零且落 next 钉区。RED today: name 不画 -> diff == 0（设计态红）。 */
    pcn_inject_full(PCN_NAME_FULL10, "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_inject_full("钢", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_next, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A4 RED: CJK 长名帧差为零——name 未被渲染（设计态红: 弱哨兵不可用; "
        "builder 落 seam 后转绿; 截半安全停笔语义以本齿在场为前提）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        "PCN-A4 RED: 长名墨差落在 next 钉区带外");

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
    pcn_diff_stats(pcn_f_a, pcn_f_c, pcn_rect_next, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        pcn_fm("PCN-A4 RED: 截半帧 vs 完整帧 diff %d != 0——截半多字节字符被画"
               "（安全停笔义务: 截半字符不画）", total));

    /* tooth 3 (A4 不越界): 截半帧右界带零墨（不越界半）。 */
    ink = pcn_right_band_ink(pcn_rect_next, pcn_f_c);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A4 RED: 截半帧右界带墨 %d != 0——截半处理越出钉区右界", ink));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — P1CJK-A5：字形缺失（弱默认路径）退化为现状不崩
 * (a) probe 自检：经 seam 找一个真缺字形码点（候选表运行时探针，防 host 子集
 *     覆盖漂移假红）；
 * (b) name 带缺字形字符 vs 不带——diff==0：缺字形零墨、不画垃圾（缺字形路径返回
 *     未初始化字形描述/垃圾位图即红）；
 * (c) 纯缺字形名渲染 rc==0 不崩；
 * (d) name 管线在场（钢琴 vs 游泳 diff 非零, 基线必红）。
 * RED today: (d) diff == 0（设计态红）；(a)(b)(c) 基线即绿（守卫半）。
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

    /* tooth 2 (A5 name 在场): 钢琴 vs 游泳 diff 非零落 next 区。RED today（设计态）。 */
    pcn_inject_full("钢琴", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_inject_full("游泳", "英语培训", "春游");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_b) == 0,
        "PCN RED(infra): frame not rendered");
    pcn_diff_stats(pcn_f_a, pcn_f_b, pcn_rect_next, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "PCN-A5 RED: name 帧差为零——name 未被渲染（设计态红: 弱哨兵不可用）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total, "PCN-A5 RED: name 墨差落在钉区带外");

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
    pcn_diff_stats(pcn_f_a, pcn_f_c, pcn_rect_next, &total, &in_band);
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
 * TEST 5 — P1CJK-A3 单项半守卫 + P1CJK-E2 铁律 canary + 扫描器自检
 * （must-stay-green at baseline——现状即绿, 逐条点名见 RED baseline 判读口径）
 *   A3 单项半: 单项日程时 NEXTLIST / PREVIEW 区严格零墨（既有 74 齿只夹具 0/2 项
 *   两态, 单项零墨 pin 缺位——本齿 additive 补守卫; 空日程半由 HPB TEST 1 tooth 5
 *   承载, 不重复造齿）。
 *   E2 canary: hp1_item_t 与 11 个冻结契约函数声明面在世 + home_upcoming 零改
 *   语义（不回写字形 seam）+ 契约/需求面在世。
 * ------------------------------------------------------------------------ */
/* coverage: P1CJK-A3 -- 单项零墨守卫（nextlist/preview 区严格零墨）; 空日程半 = 既有 74 齿 HPB TEST1 tooth5 */
/* coverage: P1CJK-E2 -- 铁律 canary：11 个冻结契约函数声明面 + hp1_item_t 形状 + home_upcoming 零改语义 + 契约/需求面在世 */
TEST_CASE("P1CJK guards: 单项零墨+铁律canary+扫描器自检 (P1CJK-A3/E2)",
          "[p1_cjk_name][P1CJK-A3][P1CJK-E2][guards]")
{
    int ink;

    /* ---- A3 单项半: 1 项日程 + 无预告 ------------------------------------- */
    pcn_set_fixture();
    pcn_inject_one("钢琴课");
    pcn_render_ok();
    TEST_ASSERT_TRUE_MESSAGE(pcn_snap(pcn_f_a) == 0,
        "PCN RED(infra): frame not rendered");

    ink = pcn_count_ink(pcn_rect_next);
    TEST_ASSERT_TRUE_MESSAGE(ink >= PCN_INK_MIN_ELEM,
        pcn_fm("PCN-A3 RED(infra): 单项日程 next 区墨 %d < %d——next 区失墨（主体"
               "渲染被拆, 非本包预期）", ink, PCN_INK_MIN_ELEM));
    ink = pcn_count_ink(pcn_rect_nextlist);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A3 RED: 单项日程 nextlist 区墨 %d != 0——s_count>=2 门控被破"
               "（单项画了空列表兜底/ name 误落下一区带）", ink));
    ink = pcn_count_ink(pcn_rect_preview);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        pcn_fm("PCN-A3 RED: 无预告 preview 区墨 %d != 0——s_has_preview 门控被破",
               ink));

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
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — P1CJK-A1 机制半 + P1CJK-A6 装配事实半 + 注册接线（structural, RED today）
 *   tooth 1: home_p1.c 弱默认 + 调用点（>= 2 处）
 *   tooth 2: host_infra_seam.c host 强符号（Source Han 子集路径）
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

    /* tooth 2: host 强符号（host_infra_seam.c, Source Han 子集路径）。 */
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
     * 序断言由既有 HPB/KSG 注册齿家族口径承载, 本齿钉存在性）。RED until wiring。 */
    TEST_ASSERT_TRUE_MESSAGE(pcn_file_contains(PCN_HOST_CMAKE,
                                               "p1_cjk_name.acceptance.c"),
        "PCN RED: p1_cjk_name.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — host 验收"
        "构建未接本包（BUILDER DUTY 4))");

    /* tooth 5 (guard): home_p1.c 被测面在世。 */
    n = pcn_scan_count("firmware/main", "kidpal_home_p1_render_frame(", hit,
                       sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "PCN RED(infra): kidpal_home_p1_render_frame 定义丢失——被测面被拆");
}

/* ---------------------------------------------------------------------------
 * P1CJK-A6 闸证据条目（冻结文本——本条不是 host 齿, 不造假齿; 执行承载 = verifier
 * final 轮收口 + human 持板目击, 证据落 delivery/changes/dev/
 * 260921-m3-k4-p1-cjk-dynamic-text/）:
 *
 *   (1) device build 闸: idf.py 构建 xiaozhi.bin 过 + 体积余量记录（无新增大静态
 *       量——s_p1_fb 帧缓冲既有 PSRAM .ext_ram.bss 面零偏移、零新增帧缓冲; 字形
 *       位图经 [S1] seam 运行时获取、零新增大静态表）。device 强符号装配事实
 *       （kidpal_render_node.cc 含 kidpal_p1_font_glyph + font_puhui_14_1）由
 *       TEST 6 tooth 3 在 host 侧承载（源锚代理档, 自陈非行为本体）。
 *   (2) 真机目测点（挂设备轮窗口——可随 K3 设备轮顺带或独立开窗）: 接下来卡课程名
 *       可见且与 time 数字基线协调; 后续事项/预告 name 可见; 14px 1bpp 阈值化后
 *       可读性（阈值调整属设计迭代不触契约, P1CJK-E3 同口径）。
 *
 * ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author; builder 落地后 TEST 1-4/6
 * 转绿、TEST 5 恒绿）:
 *   通道① 既有 host runner 全量实跑（本文件未接线前）= 有效基线: 74 Tests /
 *       0 Failures / 0 Ignored, exit 0（2026-09-22 acceptance-author 实跑复核,
 *       面域 = firmware, 量测 @ 冻结时点工作树; K4 launch 段锚行 91b3b5c 起零差异）。
 *   通道② 本文件 C 类冻结验收实编 + 独立 stub 执行（DEV-GOVERNANCE 验收测试纪律
 *       §3/§4; 脚手架 = firmware/main/test/acceptance/red-baseline/ 下 unity.h +
 *       runner.c + pcn_red_seam_stub.c（本包新增, 非冻结件——提供 host_infra_
 *       render_p1_frame/read_pixel 强符号与 kidpal_art_* 桩, 使在世 home_p1.c 真
 *       实渲染、红落在字形 seam 真实缺失上）:
 *       gcc -std=c11 -I firmware/main/test/acceptance/red-baseline \
 *           -I firmware/main/display \
 *           -o /tmp/pcn_red_runner \
 *           firmware/main/test/p1_cjk_name.acceptance.c \
 *           firmware/main/display/home_p1.c \
 *           firmware/main/test/acceptance/red-baseline/runner.c \
 *           firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c
 *       （仓根 cwd 执行 /tmp/pcn_red_runner; 编译通道 error 为零 = 实编过）
 *   预期红（设计态, builder 落 [S1]/[S2] 后转绿, 不计违约）:
 *     - TEST 1/2/3/4 的「name 在场」断言（kidpal_p1_font_glyph 弱哨兵不可用 ->
 *       name 全部不画 -> diff == 0）;
 *     - TEST 6 tooth 1/2/3/4（seam 三符号未落 + 未注册接线——真实缺失行为）。
 *   MUST-STAY-GREEN at baseline（守卫齿, 现状即绿）:
 *     - TEST 2 tooth 1（右界带零墨——现状 name 不画, 兜底描边 x<=390 不越带）;
 *     - TEST 3 tooth 2/3（截半零墨/不越界——现状 name 不画, diff 恒零, 语义由
 *       tooth 1 在场前提兑现后生效）;
 *     - TEST 4 tooth 1/3/4（probe 自检 + 缺字形零墨/不崩——弱哨兵即全缺场退化）;
 *     - TEST 5 全部（A3 单项零墨守卫 + E2 canary + 扫描器自检）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）。
 * --------------------------------------------------------------------------- */
/* REVISION LOG (acceptance-author only; tighten-only discipline):
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
 * --------------------------------------------------------------------------- */
