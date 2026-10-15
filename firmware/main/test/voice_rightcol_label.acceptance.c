/*
 * Frozen acceptance tests — TaskPackage 261015-voice-rightcol-label-overflow
 * (subsystem: firmware/main/display; rigor lite; sourceType regression; riskTier
 * medium; 语音轮右列文字溢出修复——echo/answer/标头三 label 宽 264 旧版式遗留值收窄
 * 至 196)。
 *
 * Story: S-F1.3 · 语音问课表（AC1/AC3——答复/回显文字上屏显示完整性）。语义权威 =
 * docs/requirements.md 现文 S-F1.3 节; 行为权威 = reviewed plan.md（architect
 * /triage-change 分流 review 2026-10-15, human 触点①确认 ②单包 Lite + 标头 :1187
 * 收进——frontmatter reviewed-by 为证）。非新语义包: 修复既有真机显示缺陷（human
 * 真机照片实证 TALK 态问答框行尾 1~2 字被裁）, 不改任何需求语义。
 *
 * 根因（plan.md, architect 复核确认）: 语音页右列容器在 ui-v5-align 阶段3 收窄为
 * x=192、宽 196（版式数学 400-192-12; lcd_display.cc 容器 set_size(196, 172) +
 * set_pos(192, 80)）, 但盒内两个 label 的宽度 264 是旧版式遗留值（260915-m1-k3
 * 时代右列 x=124 起、宽 264）——容器改了、label 没跟着改。LV_LABEL_LONG_WRAP 下
 * label 按 264px 换行、容器视口仅 196（pad_all=0）, LVGL 默认裁剪溢出 → 每行右端
 * 约 68px 被裁不可见。
 *
 * Nature: HOST-SIDE STRUCTURAL（源锚齿 tree-scan, MCD-A1 形态先例 =
 * char_window_overlay.acceptance.c test 10 同款; 助手族自包含复制、前缀 vrx_、
 * static 链接不与先例文件符号冲突）。lcd_display.cc 设备装配面非 host 编译——源级
 * 树扫承载。正向齿只认**注释剥离后可执行源**的字面整数（260917-m2-k3 sealing pass
 * 教训: 只出现在注释里的锚值会假绿, 注释无法满足正向齿）; 反证齿行级双针走**裸源**
 * （负向齿不剥注释——剥注释会放松负向齿, sealing pass 纪律; lcd_display.cc 现存
 * 264 字面另有两处注释提及——y262-264 底条上沿实线设计注记, 行级双针天然排除注释
 * 假红, 实测核对）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements; 本文件只读扫工作树;
 * 「home =」为冻结落点）:
 *
 *   [1] 三 label 宽度收窄 264 -> [188, 196]（VR-A1/A2/A3; home =
 *       firmware/main/display/lcd_display.cc 单文件三处 lv_obj_set_width）:
 *         g_kidpal_voice_echo_label   （回显区, LISTEN/THINK 态）        -- VR-A1
 *         g_kidpal_voice_answer_label （问答框, TALK 态——human 报 bug 面）-- VR-A2
 *         g_kidpal_voice_header_label （标头, human 触点①裁定一并收进）  -- VR-A3
 *       三处 lv_obj_set_width 的字面宽度实参落**区间 [188, 196]**（不钉单一魔数,
 *       MCD-A1 先例——W 终判归 builder + 真机轮）。区间两端依据: 上界 196 = plan 钉死
 *       （右列容器内容宽 196——容器 set_size 宽 196、pad_all=0, label 宽 196 = 与容器
 *       视口一致, 换行点不越视口、行尾不再被裁）; 下界 188 = plan 让位自由度地板
 *       （architect 查证: 固件 LVGL 9.3.0 默认主题滚动条 ≈5px 盖在右缘不缩内容区,
 *       builder+真机轮终判若观感不佳可让位改 188~190——**下界 188 采纳理由**: 188 是
 *       plan 明文授权域的闭端; 低于 188 无授权, 且 label 宽较容器缩 >8px = 换行点早于
 *       容器视口的反向收敛缺陷, 与本包「显示完整性」语义同向矛盾）。264 旧值在区间外
 *       → 三处任一残留 264 即红（与 [2] 反证齿同源双红）。
 *       **几何锚 = 符号名**（g_kidpal_voice_*_label）, 禁行号钉点; 宽度须以**字面
 *       整数**书写——宏间接/符号常量形态取值失败 = 红, 不改判（几何锚冻结形态, MCD-A1
 *       同款; 若实现必须走宏间接, 返 planner 重派 acceptance-author 重钉）。
 *       零联动声明（非目标; 越界归 VR-A5 回归观察面）: 标头 x=124 位置不动（WF-B
 *       标头位是独立版式决策, 本齿只钉宽度不钉位置）; THINK 让位路径（apply_look 区
 *       只 set_pos/set_size echo_box, 不碰 label 宽）零联动。
 *
 *   [2] 反证齿 264 销账（VR-A4）: lcd_display.cc 全文**行级双针**负向齿——同时含
 *       "lv_obj_set_width(" 与 "264" 的行数 = 0（裸源不剥注释）。判别力论证:
 *       (a) 正向齿 W<=196 已使「三处调用点任一回 264」红（264 > 196 天然红）——主
 *       承载, TaskPackage VR-A4 明文认可此承载路径; (b) 本齿独立加防「首个同符号
 *       set_width 已转绿、后位残留 264 重复调用/死代码」假绿——字面整数取值器只取
 *       首个同锚出现, 行级双针对全文件任何 set_width×264 行红, 覆盖 264 字面落在
 *       三个冻结调用点之外的 set_width 面残留。变异承载 = TaskPackage
 *       mutationEvidence.perAcceptance 三条——VR-A1/A2/A3 各回退 264 时各自正向齿
 *       与本反证齿同源双红（红名双份留档）。
 *
 *   [3] Builder FIRST ACTION（本文件进 host runner 的唯一前置; 接线行**不在
 *       acceptance-author 写权内**, scope-map 分权——TaskPackage changeManifest
 *       第三件预指派 builder 代落）: 向 firmware/main/test/host_app/main/
 *       CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 追加一行:
 *         "../../voice_rightcol_label.acceptance.c"
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, 包 0 boundary 齿注册序守卫, 既有惯例）。本文件纯树扫、零被测实现接线
 *       （lcd_display.cc 不进 host 构建）。
 *
 *   [4] 冻结授权链: TaskPackage 261015-voice-rightcol-label-overflow（acceptance[]
 *       VR-A1~A6 + mutationEvidence 骨架为冻结边界; plan.md reviewed + human 触点①
 *       确认 ②单包 Lite + 标头收进）。本文件冻结后 builder/fixer tighten-only
 *       （纯 additive 补断言/修自身 fixture; 禁放宽、禁改断言语义）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: VR-A1 -- echo label 宽度源锚（test 1）
 *   coverage: VR-A2 -- answer label 宽度源锚（test 2; human 报 bug 面主齿）
 *   coverage: VR-A3 -- header label 宽度源锚（test 3; 触点①收进）
 *   coverage: VR-A4 -- 264 销账反证齿 + 扫描器自检（test 4）; 真实 assert() 守卫随
 *             助手族 logic guard 在世（C4.2 纪律——见 vrx_read / vrx_contains /
 *             vrx_strip_c_comments 内 assert, host OOM / NULL 针大声失败, 不静默）
 *   coverage: VR-A5 -- 不落本文件齿: voice_round_ui.acceptance.c 零改动全绿 = host
 *             套件回归观察面（architect 已核查该文件全文无对三 label 的 set_width/
 *             set_size/set_pos needle, 归档件同查无）; 「相对有效基线零新增红」=
 *             既有齿全绿 + 本文件四齿转绿的推论面, 归红基线/终跑判读, 无独立断言
 *             对象（落齿即越权复钉既有冻结件断言面）。
 *   coverage: VR-A6 -- 真机轮 live-manual structural 豁免（TR-A5 先例口径, 不计入
 *             runner 通过面）, 不落 C 齿: human 持板目测三态右缘 + 截图对比 +
 *             THINK 让位换行一致性 + 标头不回归 + 滚动条瞬态观感终判; host 转绿后
 *             停门等 human, 不自行烧录（烧录 = app 分区单烧, 纯代码变更无 spiffs
 *             资产变更）。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder wiring 见 [3]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加）
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf   （elf 相对 host_app/ 目录）
 *   本文件另可独立冒烟（red-baseline 独立执行架先例, 冻结相位实编 + 咬合力取证用）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -I. -o /tmp/vrl_red_runner ../../voice_rightcol_label.acceptance.c runner.c
 *     cd ../../../../..  (repo root);  /tmp/vrl_red_runner
 *
 * RED baseline（freeze 相位预期申报——净 HEAD 264 在世; acceptance-author 按派发不跑
 * host_app 构建, planner 统一实跑后逐条比对; 冻结落盘时已按「冻结即冒烟/§4 C 类实编」
 * 以独立冒烟通道实编 + 实跑一次, 见交付报告）:
 *   - test 1/2/3（VR-A1/A2/A3）: 设计态红, 红名要点 = "VRL-A? RED: g_kidpal_voice_*_
 *     _label 字面宽度 W=264 超上界 196"（present 断言过、上界断言红, 红名携带实测
 *     W=264）; 若红名是「未取到…字面宽度实参」= 锚漂移或调用点被删, 属 infra 级异常
 *     而非设计态红, 须回查。
 *   - test 4（VR-A4）: 设计态红, 红名要点 = "VRL-A4 RED: lcd_display.cc 仍有 3 行
 *     lv_obj_set_width 调用携带 264 字面"（销账断言红, 行数 = 3）; 同 test 内两枚
 *     扫描器自检（行级双针自命中 >= 3 行 / 取值器对 echo_box (196,172) 命中）绿。
 *   - 既有套件（voice_round_ui / char_window_overlay 等）: 全绿零新增红（VR-A5 面）。
 *   - 转绿判据: builder 三处 264 -> 196（或授权让位 188~190）后四齿全绿; 任一齿红而
 *     W 已在 [188,196] = 锚漂移, 复核本文件锚, 不得改判为绿。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

#define VRX_MAX_PATH 1024
#define VRX_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根; 行号快照非长期事实源, 齿锚全部用符号锚）。 */
#define VRX_LCD_CC "firmware/main/display/lcd_display.cc"

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（自包含复制自 char_window_overlay.acceptance.c 的
 * cwx_* 助手族, 精简为本文件所需子集; 全 static 链接, 命名前缀 vrx_ 不与先例冲突）。
 * 全部扫描跑在真实工作树上, root 以手持 cwd 定位（仓根 / firmware/main/test/host_app
 * 两处都可跑——host_app build 目录与仓根两处 cwd 均可跑的 walk-up 定根纪律）。
 * ------------------------------------------------------------------------ */

static char vrx_root[VRX_MAX_PATH];
static int vrx_root_ready;

static int vrx_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int vrx_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[VRX_MAX_PATH];
    size_t i;

    if (vrx_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (vrx_is_regular(probe)) {
            snprintf(vrx_root, sizeof(vrx_root), "%s", cands[i]);
            vrx_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int vrx_rel(char *out, size_t cap, const char *rel)
{
    if (!vrx_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", vrx_root, rel);
    return 1;
}

static char *vrx_read(const char *rel, size_t *out_len)
{
    char p[VRX_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!vrx_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > VRX_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/CWX 先例同款）。 */
static char vrx_fm_buf[1024];
static const char *vrx_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *vrx_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(vrx_fm_buf, sizeof(vrx_fm_buf), fmt, ap);
    va_end(ap);
    return vrx_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Comment-stripping scanner support（260917-m2-k3 sealing pass 同款）:
 *   裸源扫描分不清「代码」与「注释」——一个只出现在**注释**里的锚值会满足正向齿
 *   （builder M4 实证教训）。本助手把注释字节抹成空格, 使正向齿只能被**可执行源**
 *   满足。C/C++ `//` 行注释 + 块注释; 字符串/字符字面量体保留（扫描器跳过字面量体,
 *   不把 "..." / '...' 里的 // 误判成注释）。保持缓冲长度（注释字节 -> 空格）,
 *   调用方负责 free; 不可读/NULL -> NULL。
 * ------------------------------------------------------------------------ */

static char *vrx_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL); /* logic guard: NULL source is a test bug, not a red */
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(out, src, n + 1);

    while (i < n) {
        char c = out[i];
        if (c == '"' || c == '\'') {
            char q = c;
            i++;
            while (i < n) {
                if (out[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (out[i] == q) { i++; break; }
                i++;
            }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '/') {
            while (i < n && out[i] != '\n') { out[i] = ' '; i++; }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '*') {
            out[i] = ' '; out[i + 1] = ' '; i += 2;
            while (i < n) {
                if (out[i] == '*' && i + 1 < n && out[i + 1] == '/') {
                    out[i] = ' '; out[i + 1] = ' '; i += 2; break;
                }
                if (out[i] != '\n') { out[i] = ' '; }
                i++;
            }
            continue;
        }
        i++;
    }
    return out;
}

static char *vrx_strip_c_comments_file(const char *rel)
{
    char *buf = vrx_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = vrx_strip_c_comments(buf);
    free(buf);
    return out;
}

/* Signed integer literals that appear AFTER the first occurrence of anchor in hay,
 * up to the closing ')' or end-of-buffer。用于几何源锚齿（set_width 宽度实参）——
 * 值以真实源落点为锚, 禁行号钉点; 字符指针/常量名不作数（只取数字字面量, 故齿的
 * 冻结形态要求几何锚以**字面整数**书写; 非字面形态 -> count 不足 = 红, 不改判）。
 * 返回写入 out 的个数（anchor 缺席 / 无可解析数字 -> 0）。 */
static int vrx_ints_after(const char *hay, const char *anchor, long *out, int max_args)
{
    const char *p;

    if (hay == NULL || anchor == NULL || out == NULL || max_args <= 0) {
        return 0;
    }
    p = strstr(hay, anchor);
    if (p == NULL) {
        return 0;
    }
    p += strlen(anchor);
    {
        int n = 0;
        while (*p != '\0' && n < max_args) {
            if (*p == ')' || *p == ';') {
                break;
            }
            if (*p == '-' || (*p >= '0' && *p <= '9')) {
                char *end = NULL;
                long v = strtol(p, &end, 10);
                if (end == p) {
                    break;
                }
                out[n++] = v;
                p = end;
                continue;
            }
            p++;
        }
        return n;
    }
}

/* 行级判定（raw 不剥注释——负向齿专用, 260920-m2-charwindow-card-fix 先例同款）:
 * slice 内同时含 needle1 与 needle2、且不含 excl（NULL = 不排除）的行数。负向齿用
 * 行级双针而非全文计数: 注释行只提及 264（如 y262-264 底条设计注记）不会误红, 而
 * 剥注释会放松负向齿（sealing pass 纪律: 负向齿不剥注释）。 */
static int vrx_count_lines_with_both(const char *slice, const char *needle1,
                                     const char *needle2, const char *excl)
{
    int count = 0;
    const char *line;

    if (slice == NULL) {
        return 0;
    }
    line = slice;
    while (*line != '\0') {
        const char *eol = strchr(line, '\n');
        size_t len = eol ? (size_t)(eol - line) : strlen(line);
        char tmp[VRX_MAX_PATH];
        size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;

        memcpy(tmp, line, cp);
        tmp[cp] = '\0';
        if (strstr(tmp, needle1) != NULL && strstr(tmp, needle2) != NULL &&
            (excl == NULL || strstr(tmp, excl) == NULL)) {
            count++;
        }
        if (eol == NULL) {
            break;
        }
        line = eol + 1;
    }
    return count;
}

#define VRX_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(vrx_locate_root(), \
    "VRL RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* -------------------------------------------------------------------------- *
 * TEST 1 — VR-A1 echo label（回显区, LISTEN/THINK 态）宽度源锚齿（structural）
 * 几何锚 = 符号名 g_kidpal_voice_echo_label（禁行号钉点）; 宽度实参须为字面整数,
 * 冻结区间 [188, 196]（上界 196 = 容器内容宽, plan 钉死; 下界 188 = 让位授权域地板,
 * 见头注 [1]）。正向齿走注释剥离后源——注释无法满足。
 * RED today（设计态）: :1209 快照位 lv_obj_set_width(g_kidpal_voice_echo_label, 264)
 * ——264 > 196 上界红（present 断言过、上界断言红）。
 * ------------------------------------------------------------------------ */
/* coverage: VR-A1 -- 右列 echo label（回显区, LISTEN/THINK 态）宽度 264->196: lcd_display.cc g_kidpal_voice_echo_label 的 lv_obj_set_width 字面宽度在 [188,196]（与右列容器内容宽一致, 容器 x=192 宽 196、pad_all=0）; 换行点从 264 前移到 W、行数变多/滚动更早出现属冻结设计预期行为（VQ-A2 超屏高滚出 / VQ-A3 长文滚动看完）, 非副作用; 行尾不再被容器视口裁剪 */
TEST_CASE("语音轮右列 label 宽度源锚: echo 回显区 188..196 (VR-A1)",
          "[voice_rightcol_label][VR-A1][structural]")
{
    char *code;
    long a[8];
    int n;

    VRX_REQUIRE_ROOT();

    /* 注释剥离后的源: 几何锚只认**可执行源**的字面整数（注释里的锚不作数）。 */
    code = vrx_strip_c_comments_file(VRX_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "VRL RED(infra): lcd_display.cc unreadable");

    /* tooth 1 (presence): 字面宽度实参在场（取值失败 = 红不改判——几何锚须以字面整数
     * 书写; 宏间接/符号常量形态同红, 返 planner 重钉; 锚 = 符号名禁行号钉点）。 */
    n = vrx_ints_after(code, "lv_obj_set_width(g_kidpal_voice_echo_label,", a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        vrx_fm("VRL-A1 RED(infra): lcd_display.cc 未取到 lv_obj_set_width("
               "g_kidpal_voice_echo_label, W) 的字面宽度实参（取到 %d 个）——echo label "
               "宽度锚漂移、调用点被删或非字面形态（宽度锚冻结形态 = 字面整数; 须复核"
               "锚, 不得改判为绿）", n));

    /* tooth 2 (本包设计态红齿, 上界): W <= 196——264 旧版式遗留值在世即红。 */
    TEST_ASSERT_TRUE_MESSAGE(a[0] <= 196,
        vrx_fm("VRL-A1 RED: g_kidpal_voice_echo_label 字面宽度 W=%ld 超上界 196——右列"
               "容器收窄 196（x=192, 宽 400-192-12, pad_all=0）后 label 仍按旧版式 264 "
               "换行, 每行右端约 68px 被容器视口裁剪（human 真机实证行尾字被裁; "
               "TaskPackage 261015-voice-rightcol-label-overflow VR-A1; 修复 = 字面宽度"
               "收进区间 [188, 196]）", a[0]));

    /* tooth 3 (下界): W >= 188——plan 让位授权域地板（滚动条让位 188~190 授权域）;
     * 低于 188 无授权且构成反向收敛缺陷（换行点早于容器视口, 右缘死区）。 */
    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 188,
        vrx_fm("VRL-A1 RED: g_kidpal_voice_echo_label 字面宽度 W=%ld 低于下界 188——"
               "plan 授权域 = [188, 196]（196 = 容器内容宽; 188 = 滚动条让位地板, "
               "builder+真机轮终判自由度 188~190, MCD-A1 不钉单一魔数先例）; 低于 188 "
               "= label 宽与容器视口脱节超授权, 换行点早于容器视口（反向收敛缺陷）",
               a[0]));

    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — VR-A2 answer label（问答框, TALK 态——human 报 bug 面主齿）宽度源锚齿
 * RED today（设计态）: :1229 快照位 lv_obj_set_width(g_kidpal_voice_answer_label,
 * 264)——264 > 196 上界红。
 * ------------------------------------------------------------------------ */
/* coverage: VR-A2 -- 右列 answer label（问答框, TALK/说话态——human 真机报的 bug 面）宽度 264->196: lcd_display.cc g_kidpal_voice_answer_label 的 lv_obj_set_width 字面宽度在 [188,196]; 显示语义同 VR-A1 */
TEST_CASE("语音轮右列 label 宽度源锚: answer 问答框 188..196 (VR-A2)",
          "[voice_rightcol_label][VR-A2][structural]")
{
    char *code;
    long a[8];
    int n;

    VRX_REQUIRE_ROOT();

    code = vrx_strip_c_comments_file(VRX_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "VRL RED(infra): lcd_display.cc unreadable");

    n = vrx_ints_after(code, "lv_obj_set_width(g_kidpal_voice_answer_label,", a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        vrx_fm("VRL-A2 RED(infra): lcd_display.cc 未取到 lv_obj_set_width("
               "g_kidpal_voice_answer_label, W) 的字面宽度实参（取到 %d 个）——answer "
               "label 宽度锚漂移、调用点被删或非字面形态（宽度锚冻结形态 = 字面整数; "
               "须复核锚, 不得改判为绿）", n));

    TEST_ASSERT_TRUE_MESSAGE(a[0] <= 196,
        vrx_fm("VRL-A2 RED: g_kidpal_voice_answer_label 字面宽度 W=%ld 超上界 196——"
               "问答框（TALK 态）label 仍按旧版式 264 换行, 每行右端约 68px 被容器视口"
               "裁剪（human 真机照片实证: 行尾 1~2 字/行尾星号被裁——本包 bug 面主齿; "
               "TaskPackage 261015-voice-rightcol-label-overflow VR-A2; 修复 = 字面"
               "宽度收进区间 [188, 196]）", a[0]));

    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 188,
        vrx_fm("VRL-A2 RED: g_kidpal_voice_answer_label 字面宽度 W=%ld 低于下界 188——"
               "plan 授权域 = [188, 196]（196 = 容器内容宽; 188 = 滚动条让位地板, "
               "builder+真机轮终判自由度 188~190）; 低于 188 = 反向收敛缺陷（换行点"
               "早于容器视口）", a[0]));

    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — VR-A3 header label（标头, human 触点①裁定一并收进）宽度源锚齿
 * RED today（设计态）: :1187 快照位 lv_obj_set_width(g_kidpal_voice_header_label,
 * 264)——264 > 196 上界红。位置齿不落: 标头 x=124 是 WF-B 独立版式决策（非目标,
 * 位置漂移归 VR-A5 回归观察面）。
 * ------------------------------------------------------------------------ */
/* coverage: VR-A3 -- 标头 label 宽度 264->196 一并收进（human 触点①一句话改判授权, 统一版式卫生）: lcd_display.cc g_kidpal_voice_header_label 的 lv_obj_set_width 字面宽度在 [188,196]; 标头 x=124 位置零触碰（WF-B 标头位是独立版式决策, 非本包范围——本齿只钉宽度不钉位置） */
TEST_CASE("语音轮右列 label 宽度源锚: header 标头 188..196 (VR-A3)",
          "[voice_rightcol_label][VR-A3][structural]")
{
    char *code;
    long a[8];
    int n;

    VRX_REQUIRE_ROOT();

    code = vrx_strip_c_comments_file(VRX_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "VRL RED(infra): lcd_display.cc unreadable");

    n = vrx_ints_after(code, "lv_obj_set_width(g_kidpal_voice_header_label,", a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        vrx_fm("VRL-A3 RED(infra): lcd_display.cc 未取到 lv_obj_set_width("
               "g_kidpal_voice_header_label, W) 的字面宽度实参（取到 %d 个）——header "
               "label 宽度锚漂移、调用点被删或非字面形态（宽度锚冻结形态 = 字面整数; "
               "须复核锚, 不得改判为绿）", n));

    TEST_ASSERT_TRUE_MESSAGE(a[0] <= 196,
        vrx_fm("VRL-A3 RED: g_kidpal_voice_header_label 字面宽度 W=%ld 超上界 196——"
               "标头 264 旧版式遗留值未随右列收窄一并收进（human 触点①一句话改判授权:"
               " 统一版式卫生, 一并收进; TaskPackage 261015-voice-rightcol-label-"
               "overflow VR-A3; 修复 = 字面宽度收进区间 [188, 196]; 标头 x=124 位置"
               "不动）", a[0]));

    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 188,
        vrx_fm("VRL-A3 RED: g_kidpal_voice_header_label 字面宽度 W=%ld 低于下界 188——"
               "plan 授权域 = [188, 196]（188 = 滚动条让位地板; 低于 188 无授权, 反向"
               "收敛缺陷）", a[0]));

    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — VR-A4 反证齿: 264 旧值销账（structural; 行级双针裸源, 负向齿不剥注释）
 * + 扫描器自检（机器防假绿——取值器/计数器坏则全部齿失去咬合力, 先例同款）。
 * RED today（设计态）: :1187/:1209/:1229 三行 set_width 携带 264 字面——销账断言红
 * （行数 = 3）; 两枚自检绿。
 * ------------------------------------------------------------------------ */
/* coverage: VR-A4 -- 反证齿: 264 旧值在三个 lv_obj_set_width 调用点销账——行级双针（"lv_obj_set_width(" × "264" 同行）计数 = 0（裸源不剥注释; 独立加防首个同符号 set_width 转绿后后位残留 264 死代码/重复调用的假绿——取值器只取首个同锚出现）; 主承载 = 正向齿 W<=196（264 > 196 天然红）; 扫描器自检两枚防取值器/计数器失灵假绿 */
TEST_CASE("语音轮右列 264 旧值销账反证齿+扫描器自检 (VR-A4)",
          "[voice_rightcol_label][VR-A4][structural]")
{
    char *buf;
    char *code;
    long a[8];
    int n;

    VRX_REQUIRE_ROOT();

    /* ---- 反证齿（裸源行级双针——负向齿不剥注释, sealing pass 纪律）------------- */
    buf = vrx_read(VRX_LCD_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "VRL RED(infra): lcd_display.cc unreadable");
    n = vrx_count_lines_with_both(buf, "lv_obj_set_width(", "264", NULL);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        vrx_fm("VRL-A4 RED: lcd_display.cc 仍有 %d 行 lv_obj_set_width 调用携带 264 "
               "字面——264 旧版式遗留值未销账（右列容器收窄 196 后 label 宽 264 = 每行"
               "右端约 68px 被裁, human 真机 bug 本体数值; TaskPackage 261015-voice-"
               "rightcol-label-overflow VR-A4; 三处调用点 264 -> [188, 196] 全部销账后"
               "转绿; 变异取证口径: VR-A1/A2/A3 各回退 264 时本齿与对应正向齿同源双红)",
               n));

    /* 扫描器自检 1: 行级双针计数器对已知在世形态能命中（>= 3 行 = 三处调用点;
     * 负向齿机器防假绿——计数器坏则反证齿永远 0 假绿）。 */
    n = vrx_count_lines_with_both(buf, "lv_obj_set_width(", "lv_obj_set_width(", NULL);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        vrx_fm("VRL RED(infra): line-pair counter sees only %d lv_obj_set_width line(s) "
               "——行级计数器失灵（期望 >= 3 行: header/echo/answer 三处调用点在世; 反证"
               "齿失去咬合力）", n));
    free(buf);

    /* 扫描器自检 2: 字面整数取值器对已知在世形态能命中——echo_box 容器几何
     * （set_size(196, 172), 首个出现 = 创建区快照位; THINK 让位路径后位出现不改判,
     * 取值器只取首个同锚出现）。取值器坏则 VR-A1/A2/A3 齿失去咬合力。 */
    code = vrx_strip_c_comments_file(VRX_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "VRL RED(infra): lcd_display.cc unreadable");
    n = vrx_ints_after(code, "lv_obj_set_size(g_kidpal_voice_echo_box,", a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 2 && a[0] == 196 && a[1] == 172,
        vrx_fm("VRL RED(infra): vrx_ints_after 对已知 echo_box 锚失灵（取到 %d 参, "
               "期望 (196, 172)）——取值器坏, VR-A1/A2/A3 齿失去咬合力", n));
    free(code);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-15 初版冻结（TaskPackage 261015-voice-rightcol-label-overflow;
 *               acceptance[] VR-A1~A6 + mutationEvidence 骨架为冻结边界; plan.md
 *               reviewed + human 触点①确认 ②单包 Lite + 标头收进）。四 TEST_CASE:
 *               VR-A1/A2/A3 正向源锚齿（区间 [188,196]——上界 plan 钉死 + 下界让位
 *               授权域地板, 符号锚禁行号钉点, 字面整数, 注释剥离后取值）+ VR-A4
 *               反证齿（行级双针裸源销账 264）+ 扫描器自检两枚 + 助手族真实 assert()
 *               logic guard（C4.2）。VR-A5 回归观察面 / VR-A6 真机轮 structural
 *               豁免不落 C 齿（见 Coverage map）。冻结落盘冒烟 = red-baseline 独立
 *               执行架实编 + 实跑（净 HEAD 设计态红形状逐齿核对, 见交付报告）。
 * --------------------------------------------------------------------------- */
