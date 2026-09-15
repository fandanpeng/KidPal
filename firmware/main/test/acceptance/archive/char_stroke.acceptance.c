/*
 * Frozen acceptance tests — TaskPackage 260829-char-stroke
 * (subsystem: firmware+server; 查生字 P4：D3 笔顺消费 + 贝塞尔离散 + 坐标变换 +
 *  1bpp 光栅 + 米字格逐笔动画 + 右栏速查卡 + 缺字段防御降级; mergeOrder 7;
 *  云侧 C1 = server/acceptance/test_char_stroke.acceptance.py 孪生).
 *
 * Nature: host-runnable STRUCTURAL teeth (device_ws_switch.acceptance.c tree-scan
 * precedent) PLUS a frozen pure-logic offline contract for the char-stroke
 * consumption face (voice_roundtrip_b.acceptance.c weak-symbol precedent): the
 * red baseline is RED on real missing behaviour ("frozen contract not
 * implemented" weak sentinel / absent structural facts), never on a link or
 * compile error.
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure logic, host-compilable,
 * no chip/FreeRTOS/ESP-component/cJSON deps; C linkage — extern "C" if in a .cc;
 * recommended home: firmware/main/display/char_stroke.(c|cc) + char_stroke.h;
 * builder wires the implementation source into HOST_ACCEPTANCE_SRCS in
 * firmware/main/test/host_app/main/CMakeLists.txt so the strong definitions
 * override the weak defaults below — same one-line pattern as view_router.c
 * (package 2). The acceptance file probes __has_include("char_stroke.h"):
 * once the header exists it is compiled against the REAL declarations
 * (single source of truth); before that the frozen fallback declarations in
 * THIS file apply. The header must carry these declarations VERBATIM
 * (不得偏移) — same numeric codes, same struct layout, same signatures.
 *
 *   typedef struct { float x, y; } kidpal_cs_point;
 *
 *   typedef struct {
 *       const char *pinyin;     带声调, verbatim from D3 (e.g. "yǒng")
 *       const char *radical;    verbatim (e.g. "力")
 *       int        stroke_count;  from D3 strokeCount
 *       const char *words;      、-joined (e.g. "勇敢、勇气"), verbatim
 *   } kidpal_char_info;
 *
 *   typedef struct {
 *       char pinyin[48];
 *       char radical[48];
 *       char stroke_total[48];   "共 <N> 画"
 *       char progress[48];       "第 <X> 笔·共 <N> 笔" | "写完啦·共 <N> 笔"
 *       char words[96];
 *   } kidpal_char_card;
 *
 *   int kidpal_char_stroke_parse(const char *svg_path, kidpal_cs_point *out, int max_pts);
 *       Parse ONE D3 stroke SVG path (makemeahanzi graphics.txt, 1024×1024
 *       coordinate system; commands M/Q/L/Z, quadratic bezier Q: control point
 *       C, B(t) = (1-t)^2*P0 + 2(1-t)t*C + t^2*P1, NOT on-curve) into discrete
 *       points (CHR-A2). Whitespace-tolerant; multiple M subpaths concatenate
 *       (compound outline); a Q segment contributes >= 2 points beyond its
 *       start point; Z closes the subpath by appending its start point.
 *       Returns the number of points written (>= 1), or -1 on NULL/empty/
 *       unknown command/buffer overflow (max_pts too small -> -1, no partial
 *       count; never writes more than max_pts points).
 *
 *   int kidpal_char_medians_parse(const char *median_json, kidpal_cs_point *out, int max_pts);
 *       Parse ONE stroke's median skeleton "[[x,y],[x,y],...]" (D3 medians,
 *       笔尖动画沿它走, spec §3.0.2) into points (CHR-A2). Whitespace-tolerant.
 *       Returns count (>= 1) or -1 on NULL/malformed/overflow (same rules).
 *
 *   int kidpal_char_transform(float x, float y, int *sx, int *sy);
 *       Map a makemeahanzi point (domain [0,1024]×[0,1024]) to the 400×300
 *       screen (CHR-A3). Frozen properties:
 *         P1 in-screen   : for any in-domain input, 0 <= *sx < 400 and
 *                          0 <= *sy < 300 (字形全部落在屏内);
 *         P2 Y 翻转      : screen y is STRICTLY DECREASING in source y
 *                          (makemeahanzi 坐标系与屏幕 Y 朝向相反, spec §3.0.2);
 *         P3 x 不镜像     : screen x is strictly increasing in source x;
 *         P4 字形偏上偏移 : the domain center (512,512) maps STRICTLY ABOVE
 *                          the screen vertical middle (*sy < 150 — 字形偏上,
 *                          下部留提示条空间; spec §3.0.2 翻转+偏移).
 *       Returns 0 on success, -1 on NULL out pointers.
 *
 *   int kidpal_char_raster_stroke(const kidpal_cs_point *outline, int npts,
 *                                 uint8_t *bitmap, int w, int h, int stride_bytes);
 *       Scanline-fill ONE stroke outline (treated as a closed polygon; the
 *       last point does not need to repeat the first) into a 1bpp bitmap,
 *       rows top-to-bottom, MSB-first within each row, 1 = ink (CHR-A4).
 *       Fill rule = EVEN-ODD (holes must render regardless of winding — CJK
 *       compound outlines). OVERLAY semantics: only SETS bits, never clears —
 *       strokes accumulate (逐笔叠加). Never writes outside the w×h bitmap
 *       (stride respected). Returns 0, or -1 on NULL/ npts < 3 / bad dims.
 *
 *   int kidpal_char_anim_state(int elapsed_ms, int n_strokes, int *cur_stroke, int *written);
 *       逐笔演示状态机 (CHR-A5, 纯逻辑). Cycle = n_strokes*950ms writing +
 *       2000ms rest (每笔 950ms 步进、演完歇 2 秒重来, S-F3.1); elapsed_ms
 *       wraps modulo the cycle (重来/replay = pass 0). Returns
 *         0 KIDPAL_CS_PHASE_WRITING : *cur = elapsed/950 (0-based),
 *                                     *written = *cur + 1;
 *         1 KIDPAL_CS_PHASE_REST    : 演完一轮, *written = n_strokes,
 *                                     *cur = -1 (无当前笔, 定格「写完啦」);
 *        -1 on defensive input (n_strokes <= 0 or NULL out pointers).
 *
 *   int kidpal_char_stroke_state(int stroke_index, int written_count, int cur_stroke);
 *       笔顺三态分类 (CHR-A5): returns
 *         0 KIDPAL_CS_STATE_WRITTEN (已写实心黑)
 *         1 KIDPAL_CS_STATE_CURRENT (当前笔白衬边+握笔小手)
 *         2 KIDPAL_CS_STATE_PENDING (未写虚线点链)
 *        -1 defensive (stroke_index < 0).
 *       Rules: index == cur_stroke (cur >= 0) -> CURRENT; else index <
 *       written_count -> WRITTEN; else PENDING. REST (cur = -1, written = n):
 *       everything WRITTEN.
 *
 *   int kidpal_char_dash_dot_pitch(void);
 *       未写态虚线点链的点距, 像素 (CHR-A5, S-F3.1: 6px 点距). Frozen: == 6.
 *
 *   int kidpal_char_card_build(const kidpal_char_info *info, int cur_stroke,
 *                              kidpal_char_card *out);
 *       右栏速查卡数据装配 (CHR-A6, 数据面纯逻辑): pinyin/radical/words copied
 *       VERBATIM (拼音带声调原样保留); out->stroke_total = "共 <N> 画";
 *       out->progress = "第 <cur+1> 笔·共 <N> 笔" (cur_stroke in [0, N)) or
 *       "写完啦·共 <N> 笔" (cur_stroke < 0 or >= N — S-F3.3 写完定格, ✔ 为图标
 *       不在文本内). Overlong fields are truncated but ALWAYS NUL-terminated
 *       (never overflow). Returns 0, or -1 on NULL info/out or NULL member
 *       strings.
 *
 *   int kidpal_char_data_validate(const char *data_json);
 *       缺字段 char 块防御性降级门 (闸 A 包内裁定, gate-a-prereview §2.2:
 *       MVP 不动网关 display_schema, 板子侧对缺必需字段的 view:"char" data 显式
 *       降级——友好提示、不盲渲染、不崩, 对齐 view_router UNKNOWN 降级语义).
 *       Input = the char view's data JSON object text. Required keys (render-
 *       critical): char, pinyin, radical, strokeCount, strokes, medians.
 *       structure/meaning/words optional (card tolerates absence). Returns
 *       0 = all required keys present (key":" — value shape unchecked, the
 *       deep parse is device-side), -1 = any required key missing / NULL /
 *       empty input. The mcp_server.cc case 3 arm MUST call this before
 *       rendering and degrade on -1.
 * ===========================================================================
 *
 * While the implementation is absent, __attribute__((weak)) defaults below
 * return the sentinel -2 (CSA_UNIMPLEMENTED): every behavioral test asserts
 * NOT-sentinel first, so the red baseline reads "frozen contract not
 * implemented" — a real missing-behaviour red, not a link error.
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test):
 *   coverage: CHR-A1  structural -> test CHR-A1 (查字页装配: 光栅契约模块在树 +
 *               编入设备 CMakeLists + mcp_server.cc case 3 接线(消费 kidpal_char_ 面,
 *               VRT_ROUTE_CHAR=3 只消费不改) + 降级语义标记; kidpal/display 到达面守卫)
 *   coverage: CHR-A2  behavioral -> test CHR-A2 (D3 解析: M/L/Z 逐点 + Q 二次贝塞尔
 *               离散(端点精确/曲线过控制点侧/样本密度) + 多子路径 + medians 骨架点 +
 *               防御输入; CHR-E2 纯函数 host 可测由本测试即承载)
 *   coverage: CHR-A3  behavioral -> test CHR-A3 (1024→400×300: 屏内 P1 + Y 翻转 P2 +
 *               x 不镜像 P3 + 偏上 P4 + NULL 防御)
 *   coverage: CHR-A4  behavioral -> test CHR-A4 (1bpp scanline 填充: 内部实心/外部空白 +
 *               even-odd 挖洞 + 逐笔叠加不清除 + 防御输入)
 *   coverage: CHR-A5  behavioral -> test CHR-A5 (状态机: 950ms 步进边界/歇 2s/重来 wrap/
 *               三态分类/6px 点距/重播=从零重现; CHR-E2)
 *   coverage: CHR-A5  structural -> test CHR-A5-page (米字格 + 握笔小手页面结构齿)
 *   coverage: CHR-A6  behavioral -> test CHR-A6 (速查卡: 拼音声调原样/共 N 画/第 X 笔·共
 *               N 笔/写完啦定格(S-F3.3)/组词部首原样/截断不溢出/防御)
 *   coverage: 闸 A 包内裁定 (gate-a-prereview §2.2, planner intake) -> test degrade
 *               (缺字段 char 块防御降级: 六必需键齐全/缺一即降/最小块降级/防御)
 *   coverage: CHR-A7  structural -> test CHR-A7 (教学字体红线: 示范字渲染路径 =
 *               strokes 光栅(定义+调用 >= 2 文件) + P4 光栅路径无 tilt(transform_angle
 *               零命中); 伴随文字可爱体不加约束, 观感殿后包 9b/U)
 *   coverage: CHR-E3  structural -> test boundary (makemeahanzi 数据集不入固件:
 *               零引用——设备只消费每查一次下发的 D3 JSON, 32MB 数据源留云端/OpenClaw 侧)
 *   boundary/security             -> test boundary (扫描器自检防假绿 + 设备不直连
 *               OpenClaw/18789 守卫, VRT 对偶)
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring (acceptance-author 被 enforce-scope 拦在 CMakeLists 外,
 *      WSA/包 2 同款先例 — 接线属 builder): add BOTH to HOST_ACCEPTANCE_SRCS in
 *      firmware/main/test/host_app/main/CMakeLists.txt:
 *        "../../char_stroke.acceptance.c"
 *        "../../../display/char_stroke.c"      (落地后; 强符号覆盖弱哨兵)
 *      AND the include dir for the frozen-contract header probe:
 *        target_include_directories(${COMPONENT_LIB} PRIVATE "../../../display")
 *      (this file probes __has_include("char_stroke.h"): once the header exists
 *      it compiles against the REAL declarations; before that the frozen
 *      fallback declarations in THIS file apply).
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   (Real-screen 观感 — RLCD 上的观感验收 — 显式殿后包 9b device 验收, CHR-E2.)
 *
 * Cloud face: server/acceptance/test_char_stroke.acceptance.py (CHR-A8/A9:
 * 静态字库查表拼装 + LRU/预热 + system prompt v2 + live 5 秒档, @live 不 auto-run).
 *
 * RED baseline (vehicle state, 2026-09-07 acceptance-author 实测, see
 * delivery/taskpackages/260829-char-stroke/red-baseline-summary.md):
 *   - firmware/main has NO char-stroke face: no kidpal_char_* symbols, no
 *     米字格/握笔 markup, mcp_server.cc kidpal/display switch handles case 0/1/2
 *     only (char=VRT_ROUTE_CHAR=3 falls to default degrade), no char CMake wiring.
 *   - Native faces pinned as MUST-STAY-GREEN guards: mcp_server.cc kidpal/display
 *     AddTool + case 0/1/2 arms (包 2 landed 消费链路, 本包只 additive 补 case 3).
 *   -> CHR-A1/A2/A3/A4/A5/A5-page/A6/A7/degrade RED each for its own
 *      missing-behaviour reason; boundary green-guards must stay green.
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-07  initial freeze (acceptance-author).
 *   2026-09-07  CHR-A4 even-odd tooth defect fixed (freeze-time contradiction,
 *               builder 裁定请求回派; 断言语义零放宽): the original hole probe
 *               rasterized outer+inner squares as TWO SEPARATE calls into one fb
 *               and required (14,14)==0 — a cross-call ERASE that is impossible
 *               under the OR/only-sets overlay semantics the SAME test pins in
 *               its second block (cross-call OR at (5,5)/(12,12)). Stateless
 *               contradiction, not an implementation bug. Fix: the hole is now
 *               pinned WITHIN a single call via a pentagram (continuous
 *               self-crossing polygon): arms filled + double-covered center
 *               EMPTY — the center hole is exactly the even-odd vs nonzero
 *               discriminator (nonzero fills the center), so the frozen "fill
 *               rule = EVEN-ODD, holes regardless of winding" semantic stays
 *               fully pinned, now honestly. Builder's candidate (flat 8-point
 *               {outer,inner} compound) rejected: the implied cut diagonals
 *               corrupt ring parity bands, probes would pass accidentally
 *               (fake green). Overlay block + defensive asserts unchanged.
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define CSA_MAX_PATH 1024
#define CSA_MAX_READ (8u * 1024u * 1024u) /* refuse to slurp files larger than 8 MB */

/* -------------------------------------------------------------------------- *
 * Frozen contract probes: use the real header once the builder lands it,
 * frozen fallback declarations before that (single source of truth switch).
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("char_stroke.h")
#include "char_stroke.h"
#define CSA_HAS_REAL_HEADER 1
#endif
#endif

#ifndef CSA_HAS_REAL_HEADER
/* FROZEN fallback declarations — char_stroke.h must carry these VERBATIM. */
typedef struct {
    float x, y;
} kidpal_cs_point;

typedef struct {
    const char *pinyin;
    const char *radical;
    int stroke_count;
    const char *words;
} kidpal_char_info;

typedef struct {
    char pinyin[48];
    char radical[48];
    char stroke_total[48];
    char progress[48];
    char words[96];
} kidpal_char_card;

int kidpal_char_stroke_parse(const char *svg_path, kidpal_cs_point *out, int max_pts);
int kidpal_char_medians_parse(const char *median_json, kidpal_cs_point *out, int max_pts);
int kidpal_char_transform(float x, float y, int *sx, int *sy);
int kidpal_char_raster_stroke(const kidpal_cs_point *outline, int npts,
                              uint8_t *bitmap, int w, int h, int stride_bytes);
int kidpal_char_anim_state(int elapsed_ms, int n_strokes, int *cur_stroke, int *written);
int kidpal_char_stroke_state(int stroke_index, int written_count, int cur_stroke);
int kidpal_char_dash_dot_pitch(void);
int kidpal_char_card_build(const kidpal_char_info *info, int cur_stroke, kidpal_char_card *out);
int kidpal_char_data_validate(const char *data_json);
#endif /* !CSA_HAS_REAL_HEADER */

/* Frozen numeric codes / constants (acceptance-local CSA_ names; the numeric
 * values are the contract — builder-side names may differ but values must not). */
#define CSA_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */
#define CSA_PHASE_WRITING 0
#define CSA_PHASE_REST    1
#define CSA_STATE_WRITTEN 0
#define CSA_STATE_CURRENT 1
#define CSA_STATE_PENDING 2

#define CSA_SCREEN_W 400  /* 1024×1024 -> 400×300 (spec §3.0.2) */
#define CSA_SCREEN_H 300
#define CSA_SRC_SIZE 1024
#define CSA_STROKE_MS 950  /* 每笔 950ms 步进 (S-F3.1) */
#define CSA_REST_MS 2000   /* 演完歇 2 秒重来 (S-F3.1) */
#define CSA_DASH_DOT_PITCH_PX 6 /* 未写虚线点链点距 (S-F3.1) */

__attribute__((weak)) int kidpal_char_stroke_parse(const char *svg_path, kidpal_cs_point *out, int max_pts)
{
    (void)svg_path; (void)out; (void)max_pts;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_medians_parse(const char *median_json, kidpal_cs_point *out, int max_pts)
{
    (void)median_json; (void)out; (void)max_pts;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_transform(float x, float y, int *sx, int *sy)
{
    (void)x; (void)y; (void)sx; (void)sy;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_raster_stroke(const kidpal_cs_point *outline, int npts,
                                                    uint8_t *bitmap, int w, int h, int stride_bytes)
{
    (void)outline; (void)npts; (void)bitmap; (void)w; (void)h; (void)stride_bytes;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_anim_state(int elapsed_ms, int n_strokes, int *cur_stroke, int *written)
{
    (void)elapsed_ms; (void)n_strokes; (void)cur_stroke; (void)written;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_stroke_state(int stroke_index, int written_count, int cur_stroke)
{
    (void)stroke_index; (void)written_count; (void)cur_stroke;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_dash_dot_pitch(void)
{
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_card_build(const kidpal_char_info *info, int cur_stroke,
                                                 kidpal_char_card *out)
{
    (void)info; (void)cur_stroke; (void)out;
    return CSA_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_char_data_validate(const char *data_json)
{
    (void)data_json;
    return CSA_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), WSA/VRT precedent.
 * All scans run against the REAL working tree located from the runner cwd;
 * paths are repo-root-relative with forward slashes. Directories named
 * test / host_app / build / .git are EXCLUDED from every scan (test/ hosts
 * THIS frozen file whose comments legitimately mention the frozen needles;
 * host_app/build are build artifacts; none is implementation surface).
 * "assets" is additionally skipped: binary 1bpp/font assets are not
 * implementation surface for any needle below.
 * ------------------------------------------------------------------------ */

static char csa_root[CSA_MAX_PATH];
static int csa_root_ready;

static int csa_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int csa_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int csa_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int csa_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[CSA_MAX_PATH];

    if (csa_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (csa_is_regular(probe)) {
            snprintf(csa_root, sizeof(csa_root), "%s", cands[i]);
            csa_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int csa_rel(char *out, size_t cap, const char *rel)
{
    if (!csa_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", csa_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", csa_root, rel);
    return 1;
}

static int csa_exists(const char *rel)
{
    char p[CSA_MAX_PATH];
    return csa_rel(p, sizeof(p), rel) && csa_is_regular(p);
}

/* Read a repo-relative file fully into a NUL-terminated malloc'd buffer, or NULL. */
static char *csa_read(const char *rel, size_t *out_len)
{
    char p[CSA_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!csa_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > CSA_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int csa_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int csa_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL);
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

static int csa_file_contains(const char *rel, const char *needle)
{
    char *buf = csa_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = csa_contains(buf, needle);
    free(buf);
    return r;
}

/* Count files under rel_dir whose content matches needle1 (AND needle2 when
 * needle2 != NULL). Fills first_hit with the first match (repo-relative). */
static int csa_count_files_with_needles(const char *abs_dir, const char *rel_prefix,
                                        const char *needle1, const char *needle2,
                                        char *first_hit, size_t cap, int depth)
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
        char abs_next[CSA_MAX_PATH];
        char rel_next[CSA_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            csa_is_skipped_dir(e->d_name)) {
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
            count += csa_count_files_with_needles(abs_next, rel_next, needle1, needle2,
                                                  first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > CSA_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = csa_contains(buf, needle1) &&
                        (needle2 == NULL || csa_contains(buf, needle2));
                if (m) {
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

static int csa_scan_count(const char *rel_dir, const char *needle, int ci,
                          char *first_hit, size_t cap)
{
    char abs[CSA_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!csa_rel(abs, sizeof(abs), rel_dir) || !csa_is_dir(abs)) {
        return 0;
    }
    /* ci unused for now (needles below are case-sensitive or CJK); kept for
     * helper-shape parity with the WSA/VRT precedent. */
    (void)ci;
    return csa_count_files_with_needles(abs, rel_dir, needle, NULL, first_hit, cap, 0);
}

/* Both-needle variant: count files containing needle1 AND needle2. */
static int csa_scan_count_both(const char *rel_dir, const char *needle1, const char *needle2,
                               char *first_hit, size_t cap)
{
    char abs[CSA_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!csa_rel(abs, sizeof(abs), rel_dir) || !csa_is_dir(abs)) {
        return 0;
    }
    return csa_count_files_with_needles(abs, rel_dir, needle1, needle2, first_hit, cap, 0);
}

#define CSA_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(csa_locate_root(), \
    "CSA RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void csa_require_implemented(int value, const char *what)
{
    if (value == CSA_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "CSA RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic char-stroke face per the frozen "
                 "contract in firmware/main/test/char_stroke.acceptance.c header and "
                 "wire the implementation source into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* 1bpp bitmap pixel probe (MSB-first rows), for CHR-A4 raster assertions. */
static int csa_bit_get(const uint8_t *bitmap, int w, int stride_bytes, int x, int y)
{
    assert(bitmap != NULL);
    assert(x >= 0 && x < w);
    return (bitmap[(size_t)y * (size_t)stride_bytes + (size_t)(x / 8)] >> (7 - (x % 8))) & 1u;
}

/* -------------------------------------------------------------------------- *
 * CHR-A1 — 查字页装配结构（structural, +CHR-E1 类结构齿）
 * Four teeth, each a REAL repo fact:
 *   1) a module under firmware/main implements the frozen contract (references
 *      kidpal_char_stroke_parse( — today NOTHING does, char-stroke face absent);
 *   2) that module is compiled into the DEVICE build (firmware/main/CMakeLists.txt
 *      lists its basename — assembly fact, not a floating file);
 *   3) the device integration point is wired: mcp_server.cc's kidpal/display
 *      switch handles the char view — "case 3" (VRT_ROUTE_CHAR=3, view_router.h
 *      已留位, 本包只消费不改路由) AND references the kidpal_char_ contract face;
 *   4) the contract module carries an explicit degrade semantic marker (缺字段/
 *      白名单外 -> 降级, 对齐 view_router UNKNOWN 语义, 闸 A 裁定).
 * Native guard (must stay green): mcp_server.cc still registers kidpal/display
 * (包 2 landed 到达面; 本包 additive 补 case 3, 不重排既有分支).
 * RED today: teeth 1)-4) have no implementation; guard green.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 查字页装配结构——case 3 接线 + 光栅契约模块在树 (CHR-A1)",
          "[char_stroke][CHR-A1][structural]")
{
    char hit[CSA_MAX_PATH];
    char *buf;
    int n_files;

    CSA_REQUIRE_ROOT();

    /* tooth 1: 光栅/查字契约模块存在于设备树 */
    n_files = csa_scan_count("firmware/main", "kidpal_char_stroke_parse(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_files >= 1,
        "CHR-A1 RED: no module under firmware/main implements the frozen char-stroke "
        "contract (kidpal_char_stroke_parse absent) — P4 查字页的 D3 消费/光栅面缺失 "
        "(spec §3.0.2, 契约 §1.3①)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件） */
    {
        char base[CSA_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(csa_file_contains("firmware/main/CMakeLists.txt", base),
            "CHR-A1 RED: the char-stroke contract source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失）");
    }

    /* tooth 3: mcp_server.cc kidpal/display switch 补 case 3（VRT_ROUTE_CHAR=3）
     * 并消费 kidpal_char_ 契约面 — 只消费不改 view_router 数值码 */
    buf = csa_read("firmware/main/mcp_server.cc", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "CHR-A1 RED: mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        csa_contains(buf, "case 3") && csa_contains_ci(buf, "kidpal_char_"),
        "CHR-A1 RED: mcp_server.cc kidpal/display switch does not wire the char view "
        "(case 3 / VRT_ROUTE_CHAR) to the kidpal_char_ consumption face — 查字 view "
        "仍落 default 降级, P4 查字页不可达 (view_router.h VRT_ROUTE_CHAR=3, 包 4 接线点)");
    free(buf);

    /* tooth 4: 契约模块带显式降级语义标记（缺字段/白名单外 -> 降级, 不盲渲染） */
    buf = csa_read(hit, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "CHR-A1 RED: char-stroke module unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        csa_contains_ci(buf, "降级") || csa_contains_ci(buf, "degrade") ||
        csa_contains_ci(buf, "UNKNOWN") || csa_contains_ci(buf, "unknown"),
        "CHR-A1 RED: the char-stroke module carries no degrade semantic marker — "
        "缺字段 char 块必须显式降级而非盲渲染 (闸 A 包内裁定 / view_router UNKNOWN 语义)");
    free(buf);

    /* native guard (must stay green): kidpal/display 到达面在世 (包 2 landed) */
    TEST_ASSERT_TRUE_MESSAGE(csa_file_contains("firmware/main/mcp_server.cc", "kidpal/display"),
        "CHR-A1 RED(infra): mcp_server.cc no longer registers the kidpal/display tool — "
        "display 消费到达面被破坏 (包 2 共享底座)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A2 — D3 笔顺解析：M/Q/L/Z 二次贝塞尔离散（behavioral, frozen contract）
 * makemeahanzi strokes SVG path (1024×1024). Pinned properties:
 *   - M 起点精确；L 逐点精确；
 *   - Q 二次贝塞尔（控制点不在曲线上）正确离散：端点精确、曲线过控制点一侧
 *     （fixture 控制点 (512,1024) 拉出 max y ∈ [450, 517] —— 把控制点当端点的
 *      折线实现 max y = 1024、当直线的实现 max y = 0，均被此区间打出）、
 *     样本密度 >= 2 点/Q 段（count >= 3）、x 单调推进；
 *   - Z 闭合（末点回到子路径起点）；多 M 子路径拼接；
 *   - medians 骨架点 JSON 逐点解析（含空白容忍）；
 *   - 防御：NULL/空/未知命令/缓冲不足 -> -1，不越界写。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: D3 笔顺解析——M/Q/L/Z 二次贝塞尔离散 + medians 骨架 (CHR-A2)",
          "[char_stroke][CHR-A2]")
{
    kidpal_cs_point pts[32];
    int r;

    /* M 起点 */
    r = kidpal_char_stroke_parse("M 547 677", pts, 32);
    csa_require_implemented(r, "kidpal_char_stroke_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && pts[0].x == 547.0f && pts[0].y == 677.0f,
        "CHR-A2 RED: a lone M command did not yield its exact start point — D3 起笔"
        "解析错误 (契约 D3 / spec §3.0.2)");

    /* L 逐点精确 */
    r = kidpal_char_stroke_parse("M 547 677 L 547 500 L 547 300", pts, 32);
    TEST_ASSERT_TRUE_MESSAGE(r == 3, "CHR-A2 RED: L segments lost points — 折线解析缺点");
    TEST_ASSERT_TRUE_MESSAGE(pts[1].x == 547.0f && pts[1].y == 500.0f &&
                             pts[2].x == 547.0f && pts[2].y == 300.0f,
        "CHR-A2 RED: L endpoint coordinates wrong — 折线坐标错误");

    /* Q 二次贝塞尔：控制点 (512,1024) 把曲线向下（Y 朝下方向）拉。
     * 真曲线 max y = B(0.5).y = 512；任何 >= 2 内点样本的实现 max y >= B(1/3).y
     * = 455.1。把控制点当曲线点的实现会采到 y=1024；把 Q 当直线的实现 max y=0。 */
    r = kidpal_char_stroke_parse("M 0 0 Q 512 1024 1024 0", pts, 32);
    TEST_ASSERT_TRUE_MESSAGE(r >= 3,
        "CHR-A2 RED: a Q (quadratic bezier) segment produced < 2 interior samples — "
        "贝塞尔离散密度不足 (CHR-A2: 正确解析 M/Q/L/Z 并离散成点)");
    {
        float max_y = 0.0f;
        int mono_x = 1;
        for (int i = 0; i < r; i++) {
            if (pts[i].y > max_y) {
                max_y = pts[i].y;
            }
            if (i > 0 && pts[i].x < pts[i - 1].x) {
                mono_x = 0;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(pts[0].x == 0.0f && pts[0].y == 0.0f &&
                                 pts[r - 1].x == 1024.0f && pts[r - 1].y == 0.0f,
            "CHR-A2 RED: quadratic bezier endpoints not exact — Q 端点漂移");
        TEST_ASSERT_TRUE_MESSAGE(mono_x,
            "CHR-A2 RED: bezier samples not ordered along the path — 离散点序紊乱");

        /* 曲线拟合度的失败详情单列（unity MESSAGE 不做 printf 格式化, 预格式化） */
        if (max_y < 450.0f || max_y > 517.0f) {
            char detail[256];
            snprintf(detail, sizeof(detail),
                     "CHR-A2 RED: bezier max_y=%.1f, expect ~[450,517] — Q 被当成折线/"
                     "控制点被当曲线点, 二次贝塞尔离散错误", (double)max_y);
            TEST_ASSERT_TRUE_MESSAGE(0, detail);
        }
    }

    /* Z 闭合 */
    r = kidpal_char_stroke_parse("M 100 100 L 900 100 L 900 900 Z", pts, 32);
    TEST_ASSERT_TRUE_MESSAGE(r >= 4 &&
                             pts[r - 1].x == pts[0].x && pts[r - 1].y == pts[0].y,
        "CHR-A2 RED: Z (closepath) did not close back to the subpath start — 轮廓"
        "未闭合, scanline 填充会漏 (CHR-A4 依赖)");

    /* 多 M 子路径拼接（复合轮廓, makemeahanzi 常见） */
    r = kidpal_char_stroke_parse("M 100 100 L 200 100 M 300 100 L 400 100", pts, 32);
    TEST_ASSERT_TRUE_MESSAGE(r == 4,
        "CHR-A2 RED: multiple M subpaths not concatenated — 复合轮廓子路径丢失");

    /* medians 骨架点（笔尖动画沿它走, spec §3.0.2） */
    r = kidpal_char_medians_parse("[[547,677],[547,500],[547,300]]", pts, 32);
    csa_require_implemented(r, "kidpal_char_medians_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 3 && pts[0].x == 547.0f && pts[1].y == 500.0f &&
                             pts[2].y == 300.0f,
        "CHR-A2 RED: medians skeleton points parsed wrong — 笔顺骨架点错误, 握笔小手"
        "领写动画无路可走 (D3 medians)");
    r = kidpal_char_medians_parse("[ [ 1 , 2 ] , [ 3 , 4 ] ]", pts, 32);
    TEST_ASSERT_TRUE_MESSAGE(r == 2 && pts[0].x == 1.0f && pts[1].y == 4.0f,
        "CHR-A2 RED: whitespace-tolerant medians parse failed — JSON 空白不容忍");

    /* 防御输入 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_parse(NULL, pts, 32) == -1,
        "CHR-A2 RED: NULL path must be defended (-1), not crash");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_parse("", pts, 32) == -1,
        "CHR-A2 RED: empty path must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_parse("X 1 2 3", pts, 32) == -1,
        "CHR-A2 RED: unknown path command must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_parse("M 1 1 L 2 2 L 3 3", pts, 2) == -1,
        "CHR-A2 RED: buffer overflow must be signalled (-1), never written past max_pts");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_medians_parse(NULL, pts, 32) == -1 &&
                             kidpal_char_medians_parse("nope", pts, 32) == -1,
        "CHR-A2 RED: malformed medians must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A3 — 坐标变换 1024×1024 → 400×300（behavioral, frozen contract）
 * Pinned: 屏内 (P1) + Y 翻转单调 (P2) + x 不镜像 (P3) + 字形偏上 (P4) + NULL 防御。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 坐标变换 1024→400×300——Y 翻转 + 字形偏上 (CHR-A3)",
          "[char_stroke][CHR-A3]")
{
    static const float grid[] = { 0.0f, 256.0f, 512.0f, 768.0f, 1024.0f };
    int sx, sy, r;

    /* P1: 全域网格点变换后全部落在屏内（字形输出全部落在屏内, CHR-A3） */
    for (size_t i = 0; i < sizeof(grid) / sizeof(grid[0]); i++) {
        for (size_t j = 0; j < sizeof(grid) / sizeof(grid[0]); j++) {
            r = kidpal_char_transform(grid[i], grid[j], &sx, &sy);
            csa_require_implemented(r, "kidpal_char_transform unimplemented");
            if (sx < 0 || sx >= CSA_SCREEN_W || sy < 0 || sy >= CSA_SCREEN_H) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                         "CHR-A3 RED: transform(%.0f,%.0f) -> (%d,%d) lands OUTSIDE the "
                         "400×300 screen — 字形变换越界, 屏上不可见 (CHR-A3)",
                         (double)grid[i], (double)grid[j], sx, sy);
                TEST_ASSERT_TRUE_MESSAGE(0, msg);
            }
        }
    }

    /* P2: Y 翻转 —— 源 y 增大, 屏 y 严格减小（makemeahanzi 与屏幕 Y 朝向相反） */
    r = kidpal_char_transform(512.0f, 100.0f, &sx, &sy);
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "CHR-A3 RED: transform failed on in-domain input");
    {
        int sy_top = sy;
        kidpal_char_transform(512.0f, 900.0f, &sx, &sy);
        TEST_ASSERT_TRUE_MESSAGE(sy_top > sy,
            "CHR-A3 RED: screen y is not strictly decreasing in source y — Y 轴翻转"
            "缺失, 字形会上下颠倒 (spec §3.0.2)");
    }

    /* P3: x 不镜像 */
    {
        int sx1, sx2;
        kidpal_char_transform(100.0f, 512.0f, &sx1, &sy);
        kidpal_char_transform(900.0f, 512.0f, &sx2, &sy);
        TEST_ASSERT_TRUE_MESSAGE(sx1 < sx2,
            "CHR-A3 RED: screen x is not increasing in source x — x 被镜像, 字形左右"
            "翻转");
    }

    /* P4: 字形偏上偏移 —— 域中心映射到屏幕垂直中点之上（下部留提示条空间） */
    kidpal_char_transform(512.0f, 512.0f, &sx, &sy);
    TEST_ASSERT_TRUE_MESSAGE(sy < CSA_SCREEN_H / 2,
        "CHR-A3 RED: the glyph center is not biased toward the upper screen — 字形"
        "偏上偏移缺失 (spec §3.0.2 翻转+偏移, 下部要留 S-F3.2 速查卡/提示条空间)");

    /* NULL 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_transform(512.0f, 512.0f, NULL, &sy) == -1,
        "CHR-A3 RED: NULL sx must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_transform(512.0f, 512.0f, &sx, NULL) == -1,
        "CHR-A3 RED: NULL sy must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A4 — 1bpp 光栅：scanline 填充 + 逐笔叠加（behavioral, frozen contract）
 * Pinned: 内部实心/外部空白、even-odd 挖洞（洞与绕向无关）、逐笔叠加只置位
 * 不清除、防御输入。CHR-A7 的「光栅产物与逐笔叠加语义」也在此承载。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 1bpp 光栅 scanline 填充 + 逐笔叠加 (CHR-A4)",
          "[char_stroke][CHR-A4]")
{
    static uint8_t fb[32 * 4]; /* 32×32, stride 4 bytes */
    kidpal_cs_point rect1[4] = { { 4, 4 }, { 27, 4 }, { 27, 27 }, { 4, 27 } };
    kidpal_cs_point rect2[4] = { { 8, 8 }, { 23, 8 }, { 23, 23 }, { 8, 23 } };
    int r;

    assert(CSA_SCREEN_W == 400 && CSA_SCREEN_H == 300); /* logic guard: frozen geometry */

    /* 单笔画填充：内部实心、外部空白 */
    memset(fb, 0, sizeof(fb));
    r = kidpal_char_raster_stroke(rect1, 4, fb, 32, 32, 4);
    csa_require_implemented(r, "kidpal_char_raster_stroke unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "CHR-A4 RED: a valid outline failed to rasterize");
    TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 10, 10) == 1 &&
                             csa_bit_get(fb, 32, 4, 20, 16) == 1,
        "CHR-A4 RED: the interior of a closed outline is not solid — scanline 填充"
        "缺失 (1bpp 位图中间镂空)");
    TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 1, 1) == 0 &&
                             csa_bit_get(fb, 32, 4, 30, 30) == 0,
        "CHR-A4 RED: pixels outside the outline were inked — 轮廓外泄漏");

    /* 逐笔叠加：第二笔画入同一 fb，第一笔的墨不得被清（只置位、不清除） */
    r = kidpal_char_raster_stroke(rect2, 4, fb, 32, 32, 4);
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "CHR-A4 RED: second stroke failed to rasterize");
    TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 5, 5) == 1,
        "CHR-A4 RED: rasterizing stroke 2 ERASED stroke 1 ink — 逐笔叠加语义破坏 "
        "(CHR-A4: 逐笔叠加显示, 后笔不得清前笔)");
    TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 12, 12) == 1,
        "CHR-A4 RED: stroke 2 interior not inked — 第二笔未落墨");

    /* even-odd 挖洞（单次调用内）：五角星 = 连续自交多边形（V0,V2,V4,V1,V3 跳连
     * 顺序）。奇偶规则下五个角臂填实、被路径双重覆盖的中心五边形为洞 —— 中心洞
     * 正是 even-odd 与 nonzero 的判别点（nonzero 绕向数=2 会把中心填实, 奇偶
     * 计数=2 成洞），即「洞与绕向无关」的机械承载。单次调用内成洞 + 跨调用只置
     * 位叠加（上块）两条语义并立, 无冲突。 */
    memset(fb, 0, sizeof(fb));
    {
        kidpal_cs_point star[5] = {
            { 16.0f, 3.0f },    /* V0 顶角 */
            { 8.36f, 26.51f },  /* V2 左下角 */
            { 28.36f, 11.98f }, /* V4 右臂 */
            { 3.64f, 11.98f },  /* V1 左臂 */
            { 23.64f, 26.51f }  /* V3 右下角 */
        };
        r = kidpal_char_raster_stroke(star, 5, fb, 32, 32, 4);
        TEST_ASSERT_TRUE_MESSAGE(r == 0,
            "CHR-A4 RED: pentagram (self-crossing outline) failed to rasterize");
        TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 16, 7) == 1,
            "CHR-A4 RED: the pentagram arm is not filled — scanline 填充在自交"
            "多边形上失效 (even-odd 单次调用语义)");
        TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 16, 16) == 0,
            "CHR-A4 RED: the double-covered center of a self-crossing outline was "
            "filled — 填充规则不是 even-odd（洞必须成洞; nonzero 绕向规则会把中心"
            "填实, CJK 部件内白会丢失）");
        TEST_ASSERT_TRUE_MESSAGE(csa_bit_get(fb, 32, 4, 1, 1) == 0,
            "CHR-A4 RED: pixels outside the pentagram were inked — 轮廓外泄漏");
    }

    /* 防御输入 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_raster_stroke(NULL, 4, fb, 32, 32, 4) == -1,
        "CHR-A4 RED: NULL outline must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_raster_stroke(rect1, 2, fb, 32, 32, 4) == -1,
        "CHR-A4 RED: npts < 3 must be rejected (-1) — 两点不成轮廓");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_raster_stroke(rect1, 4, NULL, 32, 32, 4) == -1,
        "CHR-A4 RED: NULL bitmap must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A5 — 笔顺动画状态机（behavioral, frozen contract）
 * 每笔 950ms 步进、演完一轮歇 2 秒重来、笔顺三态、未写虚线点链 6px 点距；
 * 重播（S-F3.3 KEY 短按）= 从零重现第一笔。RED today: weak sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 笔顺动画状态机——950ms 步进/歇 2s/三态/6px 点链 (CHR-A5)",
          "[char_stroke][CHR-A5]")
{
    int cur = -99, written = -99, r;

    assert(CSA_STROKE_MS == 950 && CSA_REST_MS == 2000); /* logic guard: frozen timings */

    /* 第一笔起 */
    r = kidpal_char_anim_state(0, 9, &cur, &written);
    csa_require_implemented(r, "kidpal_char_anim_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == CSA_PHASE_WRITING && cur == 0 && written == 1,
        "CHR-A5 RED: at t=0 the first stroke must be current with 1 written — 演示"
        "未从第一笔起步");

    /* 950ms 步进边界：949 仍在第一笔，950 进入第二笔 */
    kidpal_char_anim_state(949, 9, &cur, &written);
    TEST_ASSERT_TRUE_MESSAGE(cur == 0 && written == 1,
        "CHR-A5 RED: at 949ms the stroke must not yet step — 950ms 步进边界漂移");
    kidpal_char_anim_state(950, 9, &cur, &written);
    TEST_ASSERT_TRUE_MESSAGE(cur == 1 && written == 2,
        "CHR-A5 RED: at 950ms the demo must advance to stroke 2 — 每笔 950ms 步进"
        "缺失 (S-F3.1)");

    /* 演完一轮：9*950 = 8550 进入歇 2 秒（全写完、无当前笔） */
    kidpal_char_anim_state(9 * CSA_STROKE_MS, 9, &cur, &written);
    TEST_ASSERT_TRUE_MESSAGE(cur == -1 && written == 9,
        "CHR-A5 RED: after the last stroke the demo must rest with ALL strokes "
        "written and no current stroke — 演完一轮的定格态缺失");

    /* 歇 2 秒后重来（wrap） */
    r = kidpal_char_anim_state(9 * CSA_STROKE_MS + CSA_REST_MS, 9, &cur, &written);
    TEST_ASSERT_TRUE_MESSAGE(r == CSA_PHASE_WRITING && cur == 0 && written == 1,
        "CHR-A5 RED: after the 2s rest the demo must restart from stroke 1 — 演完"
        "歇 2 秒重来缺失 (S-F3.1)");

    /* wrap 幂等：再多一整个周期后与同相位结果一致 */
    {
        int cur2, written2;
        kidpal_char_anim_state(9 * CSA_STROKE_MS + CSA_REST_MS + (9 * CSA_STROKE_MS + CSA_REST_MS),
                               9, &cur2, &written2);
        TEST_ASSERT_TRUE_MESSAGE(cur2 == cur && written2 == written,
            "CHR-A5 RED: the cycle does not wrap modulo the cycle length — 长时间"
            "演示会漂移");
    }

    /* 重播语义（S-F3.3 KEY 短按重播 = 从零逐笔重来） */
    kidpal_char_anim_state(5000, 9, &cur, &written);
    kidpal_char_anim_state(0, 9, &cur, &written);
    TEST_ASSERT_TRUE_MESSAGE(cur == 0 && written == 1,
        "CHR-A5 RED: replaying from zero does not restart at stroke 1 — 重播"
        "(S-F3.3 KEY 短按) 无法从头逐笔");

    /* 笔顺三态分类 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_stroke_state(0, 3, 2) == CSA_STATE_WRITTEN &&
        kidpal_char_stroke_state(2, 3, 2) == CSA_STATE_CURRENT &&
        kidpal_char_stroke_state(3, 3, 2) == CSA_STATE_PENDING &&
        kidpal_char_stroke_state(8, 3, 2) == CSA_STATE_PENDING,
        "CHR-A5 RED: the three-state classification is wrong — 笔顺三态（已写实心黑/"
        "当前笔白衬边+握笔小手/未写虚线点链）语义错误 (S-F3.1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_state(5, 9, -1) == CSA_STATE_WRITTEN,
        "CHR-A5 RED: during REST every stroke must classify WRITTEN — 歇秒定格态"
        "分类错误");

    /* 未写虚线点链点距：6px（S-F3.1） */
    r = kidpal_char_dash_dot_pitch();
    TEST_ASSERT_TRUE_MESSAGE(r == CSA_DASH_DOT_PITCH_PX,
        "CHR-A5 RED: the dash-dot pitch of the unwritten chain != 6px — 未写态虚线"
        "点链点距漂移 (S-F3.1)");

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_anim_state(0, 0, &cur, &written) == -1,
        "CHR-A5 RED: n_strokes=0 must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_anim_state(0, 9, NULL, &written) == -1,
        "CHR-A5 RED: NULL cur out-pointer must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_stroke_state(-1, 3, 2) == -1,
        "CHR-A5 RED: negative stroke index must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A5 (structural) — 米字格 + 握笔小手页面结构齿
 * Pixel-level 观感殿后包 9b (CHR-E2); here we pin the PAGE-LEVEL structural
 * facts: the 米字格 guide cross and the 握笔小手 (pen-hand) element exist in
 * the device char-page implementation. RED today: neither marker in tree.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 米字格 + 握笔小手页面结构 (CHR-A5 structural)",
          "[char_stroke][CHR-A5][structural]")
{
    char hit[CSA_MAX_PATH];
    int n;

    CSA_REQUIRE_ROOT();

    n = csa_scan_count("firmware/main", "米字格", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "CHR-A5 RED: no 米字格 guide-cross implementation under firmware/main — "
        "米字格大字盘缺失 (P4 查字页 canvas-p4, S-F3.1)");

    n = csa_scan_count("firmware/main", "握笔", 0, hit, sizeof(hit));
    if (n == 0) {
        n = csa_scan_count("firmware/main", "penhand", 1, hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "CHR-A5 RED: no 握笔小手 (pen-hand) element under firmware/main — 当前笔"
        "白衬边+握笔小手领写元素缺失 (S-F3.1 / ic-penhand)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A6 — 右栏速查卡数据装配（behavioral, frozen contract）
 * 拼音带声调原样 + 共 N 画 + 第 X 笔·共 N 笔 + 写完啦定格 + 部首组词原样 +
 * 截断不溢出 + 防御。进度珠（N 颗/实心闪空心）由 anim 三态驱动, 分类语义已在
 * CHR-A5 钉死。RED today: weak sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 右栏速查卡数据装配 (CHR-A6)", "[char_stroke][CHR-A6]")
{
    kidpal_char_info info;
    kidpal_char_card card;
    char long_words[200];
    int r;

    info.pinyin = "yǒng";        /* 拼音带声调 (S-F3.2) */
    info.radical = "力";          /* 部首 */
    info.stroke_count = 9;
    info.words = "勇敢、勇气";    /* 组词 (、-连接由云侧拼装, 设备原样) */

    /* 第 3 笔·共 9 笔 */
    memset(&card, 0, sizeof(card));
    r = kidpal_char_card_build(&info, 2, &card);
    csa_require_implemented(r, "kidpal_char_card_build unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "CHR-A6 RED: card build failed on valid input");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.pinyin, "yǒng") == 0,
        "CHR-A6 RED: pinyin not copied verbatim — 拼音带声调必须原样呈现 (S-F3.2, "
        "改写/掉声调 = 教学错误)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.radical, "力") == 0,
        "CHR-A6 RED: radical not copied verbatim — 部首标签缺失/漂移 (S-F3.2)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.words, "勇敢、勇气") == 0,
        "CHR-A6 RED: words not copied verbatim — 组词漂移 (S-F3.2)");
    TEST_ASSERT_TRUE_MESSAGE(strstr(card.stroke_total, "共") != NULL &&
                             strstr(card.stroke_total, "9") != NULL &&
                             strstr(card.stroke_total, "画") != NULL,
        "CHR-A6 RED: stroke_total does not read 共 9 画 — 笔画数标签缺失 (S-F3.2)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.progress, "第 3 笔·共 9 笔") == 0,
        "CHR-A6 RED: progress label != 第 3 笔·共 9 笔 — 第 X 笔·共 N 笔 格式漂移 "
        "(S-F3.2 verbatim)");

    /* 第 9 笔（cur = 8）与越界 clamp（cur = 9 -> 写完啦） */
    kidpal_char_card_build(&info, 8, &card);
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.progress, "第 9 笔·共 9 笔") == 0,
        "CHR-A6 RED: cur=8 must label 第 9 笔 — 1-based 第 X 笔 off-by-one");
    kidpal_char_card_build(&info, 9, &card);
    TEST_ASSERT_TRUE_MESSAGE(strstr(card.progress, "写完啦") != NULL &&
                             strstr(card.progress, "共 9 笔") != NULL,
        "CHR-A6 RED: cur >= N must show the 写完啦 frozen frame — 写完定格缺失 "
        "(S-F3.3: ✔ 写完啦·共 9 笔; ✔ 为图标不在文本)");
    kidpal_char_card_build(&info, -1, &card);
    TEST_ASSERT_TRUE_MESSAGE(strstr(card.progress, "写完啦") != NULL,
        "CHR-A6 RED: cur < 0 (REST phase) must show 写完啦 — 歇秒定格漂移");

    /* 截断安全：超长 words 截断但绝不溢出（恒 NUL 结尾） */
    memset(long_words, 'a', sizeof(long_words));
    long_words[sizeof(long_words) - 1] = '\0';
    info.words = long_words;
    r = kidpal_char_card_build(&info, 0, &card);
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "CHR-A6 RED: overlong words must truncate, not fail");
    TEST_ASSERT_TRUE_MESSAGE(card.words[sizeof(card.words) - 1] == '\0',
        "CHR-A6 RED: words buffer not NUL-terminated on truncation — 溢出风险 "
        "(缓冲恒安全)");
    info.words = "勇敢、勇气";

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(NULL, 0, &card) == -1,
        "CHR-A6 RED: NULL info must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, 0, NULL) == -1,
        "CHR-A6 RED: NULL out must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * 闸 A 包内裁定 — 缺字段 char 块防御性降级（behavioral, frozen contract）
 * gate-a-prereview §2.2: MVP 不动网关 display_schema（不触 GWA-A5 冻结验收），
 * 板子侧对缺必需字段的 view:"char" data 显式降级（友好提示、不盲渲染、不崩），
 * 对齐 view_router UNKNOWN 降级语义。必需键 = 渲染关键六件：char/pinyin/
 * radical/strokeCount/strokes/medians；structure/meaning/words 可容忍缺席。
 * RED today: weak sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 缺字段 char 块防御性降级 (闸 A 包内裁定)",
          "[char_stroke][degrade][gateA]")
{
    static const char *const full_data =
        "{\"char\":\"勇\",\"pinyin\":\"yǒng\",\"radical\":\"力\",\"strokeCount\":9,"
        "\"strokes\":[\"M 547 677 Q 547 760 512 760\"],\"medians\":[[[547,677]]],"
        "\"structure\":\"⿱甬力\",\"meaning\":\"勇敢；有胆量\",\"words\":[\"勇敢\"]}";
    static const char *const six_only =
        "{\"char\":\"勇\",\"pinyin\":\"yǒng\",\"radical\":\"力\",\"strokeCount\":9,"
        "\"strokes\":[\"M 547 677\"],\"medians\":[[[547,677]]]}";
    static const char *const minimal_block_data = "{\"char\":\"勇\"}";

    int r = kidpal_char_data_validate(full_data);
    csa_require_implemented(r, "kidpal_char_data_validate unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "degrade RED: a complete §1.3① char data block must validate 0 — 全量块被"
        "误拒, 正常查字会被降级");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_data_validate(six_only) == 0,
        "degrade RED: six render-critical keys present must validate 0 — 可选键"
        "(structure/meaning/words) 缺席不该连坐降级");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_data_validate(minimal_block_data) == -1,
        "degrade RED: the LLM minimal block {\"char\":\"勇\"} must DEGRADE (-1) — "
        "缺 strokes/medians/pinyin/radical/strokeCount 的最小块不得盲渲染 (闸 A "
        "裁定: 板子侧防御降级, 网关 display_schema 本包不动)");

    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_data_validate(
            "{\"char\":\"勇\",\"pinyin\":\"yǒng\",\"radical\":\"力\",\"strokeCount\":9,"
            "\"medians\":[[[547,677]]]}") == -1,
        "degrade RED: missing strokes must degrade — 无 strokes 无法光栅示范字");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_data_validate(
            "{\"char\":\"勇\",\"pinyin\":\"yǒng\",\"radical\":\"力\",\"strokeCount\":9,"
            "\"strokes\":[\"M 547 677\"]}") == -1,
        "degrade RED: missing medians must degrade — 无 medians 无笔顺动画");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_data_validate(
            "{\"char\":\"勇\",\"radical\":\"力\",\"strokeCount\":9,"
            "\"strokes\":[\"M 547 677\"],\"medians\":[[[547,677]]]}") == -1,
        "degrade RED: missing pinyin must degrade — 速查卡缺拼音 (S-F3.2)");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_data_validate(NULL) == -1,
        "degrade RED: NULL data must be defended (-1), not crash");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_data_validate("") == -1,
        "degrade RED: empty data must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * CHR-A7 — 教学字体红线（structural, spec §5.6 v3 / S-F3.1）
 * 米字格示范字必须由 D3 strokes 数据光栅渲染（makemeahanzi 数据即标准规范字形,
 * 天然无 tilt/抖动）; 禁止用字体文件渲染示范字（可爱体只准伴随文字用）。
 * Mechanical teeth:
 *   1) the raster path has a real CALLER beyond its definition (>= 2 files
 *      reference kidpal_char_raster_stroke( — defined-but-never-used fails);
 *   2) NO file on the raster path carries a transform/tilt style call
 *      (set_style_transform_angle 零命中 — 示范字不得倾斜/抖动).
 * 伴随文字（拼音/部首/组词）可用可爱体 — 不加约束; LVGL 观感细节殿后包 9b/U。
 * RED today: tooth 1 has no implementation (0 files); tooth 2 vacuously green
 * and starts biting the moment the raster path exists.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 教学字体红线——示范字 = strokes 光栅非字体 (CHR-A7)",
          "[char_stroke][CHR-A7][structural]")
{
    char hit[CSA_MAX_PATH];
    int n;

    CSA_REQUIRE_ROOT();

    /* 示范字渲染路径 = strokes 光栅：定义 + 真实调用点 >= 2 个文件 */
    n = csa_scan_count("firmware/main", "kidpal_char_raster_stroke(", 0, hit, sizeof(hit));
    if (n < 2) {
        char detail[512];
        snprintf(detail, sizeof(detail),
                 "CHR-A7 RED: kidpal_char_raster_stroke is not wired as the demo-glyph "
                 "render path (definition + real caller required, got %d files) — 米字格"
                 "示范字必须由 D3 strokes 数据光栅渲染 (标准规范字形, makemeahanzi 天然"
                 "横平竖直不抖不斜), 禁用字体文件渲染示范字 (spec §5.6 教学字体红线)", n);
        TEST_ASSERT_TRUE_MESSAGE(0, detail);
    }

    /* 光栅路径上零 tilt：任何引用光栅调用的文件不得带 transform-angle 样式调用 */
    n = csa_scan_count_both("firmware/main", "kidpal_char_raster_stroke(",
                            "set_style_transform_angle", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "CHR-A7 RED: a file on the demo-glyph raster path carries "
        "set_style_transform_angle — 示范字不得倾斜/抖动 (spec §5.6: 禁 tilt, 孩子"
        "会照写, 不能教歪)");
}

/* -------------------------------------------------------------------------- *
 * boundary / security — 扫描器自检（防假绿）+ 数据集不入固件（CHR-E3）+ 直连守卫
 *   - scanner integrity: known file exists / bogus file does not / unreadable
 *     reads NULL / no-match needle matches nothing / counter counts a known
 *     literal — the negative teeth elsewhere must be CAPABLE of failing;
 *   - CHR-E3: the makemeahanzi dataset stays cloud-side (OpenClaw 侧静态字库,
 *     绝不入 shim — python 孪生守 gateway 面): firmware/main 零引用
 *     graphics.txt / makemeahanzi（设备只消费每查一次下发的 D3 JSON）;
 *   - 设备不直连 OpenClaw / 18789（VRT 对偶守卫, must stay green）.
 * ------------------------------------------------------------------------ */
TEST_CASE("char stroke: 扫描器自检 + 数据集不入固件 (boundary/security + CHR-E3)",
          "[char_stroke][boundary][security]")
{
    char hit[CSA_MAX_PATH];
    int n;

    CSA_REQUIRE_ROOT();

    /* scanner integrity */
    TEST_ASSERT_TRUE_MESSAGE(csa_exists("firmware/main/display/view_router.h"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(csa_exists("firmware/main/display/__csa_no_such_file__.h"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(csa_read("firmware/main/display/__csa_no_such_file__.h", NULL),
        "boundary RED(infra): csa_read returned data for a nonexistent file");
    n = csa_scan_count("firmware/main/display", "__csa_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = csa_scan_count("firmware/main/display", "VRT_ROUTE_CHAR", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* CHR-E3: 32MB 数据源不入固件（挂 OpenClaw 侧, 设备只消费 D3 JSON） */
    n = csa_scan_count("firmware/main", "makemeahanzi", 1, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "CHR-E3 RED: firmware references makemeahanzi — 数据集绝不入固件 (E3: 只消费"
        "契约 D3, 32MB 数据源留云端/OpenClaw 侧; 每查一次下发单字 JSON)");
    n = csa_scan_count("firmware/main", "graphics.txt", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "CHR-E3 RED: firmware references the raw dataset file graphics.txt — 设备"
        "不持有数据源文件");

    /* 设备不直连 OpenClaw（VRT 守卫对偶, must stay green） */
    n = csa_scan_count("firmware/main", "openclaw", 1, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references openclaw directly — 设备只连自建网关");
    n = csa_scan_count("firmware/main", "18789", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references port 18789 — OpenClaw 端口不得出现在设备侧");
}
