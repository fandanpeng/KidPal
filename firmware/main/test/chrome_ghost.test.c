/*
 * Builder unit tests — TaskPackage 261022-p3-record-chrome-ghost-fix
 * （P3 记录会话 chrome 鬼影修复：语音会话收尾沿场景复位 + story -1 过渡窗键帽裁决）。
 *
 * 定位：**冻结验收 chrome_ghost.acceptance.c 的补强**，不是替代——冻结件 TEST 1/2 已
 * 钉行为本体（复位值出闭集 / 三态决策表 / 在途三子态零偏移 / 语音残留负扫描 / NULL
 * 防御）。本件只补冻结集未穷尽的分支与边界：
 *   1. 复位落点常量：复位必须落回头文件声明的默认常量且该常量出语音场景闭集——防
 *      「未来把默认值改回 0 = KVST_SCENE_LISTEN」的静默回归（复位齿只钉了「出闭集」）;
 *   2. scene 存真语义 = 任意 int 原样存取（闭集两端值 / 闭集外值 / 负值）;
 *   3. 过渡窗缝对**非 -1 的有效子态**一律回落（冻结件只测了 READER 一值），且在途
 *      子态表外的有效子态（LIST/STAMP/DELETE/BODY_UNAVAILABLE）同样零触碰;
 *   4. 帧分叉入参边界：READER / ORGANIZING 不吃 rec_sub（任意 rec_sub 同产物）;
 *      RECORDING 的 rec_sub 只做「0 / 非 0」二值分叉（负值同帧 B）。
 *   5. 引导播报子态看门狗复位沿幂等（真机轮追补 261022, KPLOG006 22:28:03 实证）：
 *      引擎 abort 竞态泄漏 tts start 帧 -> rec_substate 卡 1 -> 键帽悬挂。设备侧韧性
 *      补在 mcp_server.cc（设备 TU 不进 host 构建），故本条走源锚代理；行为半归
 *      真机观测项（CG-A5 口径扩展）。
 *
 * 纯逻辑、零芯片依赖（keybar.c 在 host runner 与设备树两侧同源）。
 * 用例内部均先写后读，不依赖整轮执行序（无顺序耦合）。
 */

#include <unity.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../display/keybar.h"

/* 补充断言的统一失败文案前缀（本件自用，不与冻结件符号冲突）。 */
#define CG_BUILDER_RED "CG builder RED: "

/* -------------------------------------------------------------------------- *
 * TEST 1 — scene 单源复位落点与存真语义（补冻结件 TEST 1 未覆盖的边界）。
 * 复位落点必须是**常量**「非语音场景默认」且出闭集——我们要的是「没有任何语音场景
 * 在演」，不是「默认演聆听」；把默认值改回 0(=KVST_SCENE_LISTEN) 就等于没修。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: keybar scene reset lands on a constant out-of-set default",
          "[chrome_ghost][builder][linux]")
{
    /* 复位落点 = 头文件声明的默认常量, 且该常量出语音场景闭集（EQ-3 的静态前提:
     * 出闭集 -> 通用分支 kidpal_keybar_slots 落 scene 越界 -1 防御 -> 残留键帽画不出）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        KIDPAL_KEYBAR_SCENE_NONE < KVST_SCENE_LISTEN ||
        KIDPAL_KEYBAR_SCENE_NONE > KVST_SCENE_LOWBATT,
        CG_BUILDER_RED "场景默认常量落在语音场景闭集内——复位后通用分支照画语音残留"
        "键帽（默认值 = 0 等于没修）");

    kidpal_keybar_scene_set(KVST_SCENE_THINK);
    kidpal_keybar_scene_reset();
    TEST_ASSERT_EQUAL_INT_MESSAGE(KIDPAL_KEYBAR_SCENE_NONE, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "reset 未落回声明常量（复位实现与头声明漂移）");

    /* 存真语义：任意 int 原样存取（闭集两端值 / 闭集外值 / 负值）。 */
    kidpal_keybar_scene_set(KVST_SCENE_LOWBATT);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_LOWBATT, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(闭集右端 LOWBATT) 未原样存真");
    kidpal_keybar_scene_set(42);
    TEST_ASSERT_EQUAL_INT_MESSAGE(42, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(42) 未原样存真——场景单源被夹到闭集内/被过滤（契约: 任意 "
        "int 原样存储, 越界值由 kidpal_keybar_slots 消费侧防御）");
    kidpal_keybar_scene_set(-7);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-7, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(-7) 未原样存真（负值同样原样存储）");

    /* reset 幂等：多次复位恒回同一默认值（复位必须是常量落点）。 */
    kidpal_keybar_scene_reset();
    {
        const int once = kidpal_keybar_scene_get();
        kidpal_keybar_scene_reset();
        kidpal_keybar_scene_reset();
        TEST_ASSERT_EQUAL_INT_MESSAGE(once, kidpal_keybar_scene_get(),
            CG_BUILDER_RED "reset 非幂等——连调后返回值漂移");
    }

    /* 收尾：复位后仍可被 set 驱动（复位不得冻结写沿）, 并回到默认态交后续用例。 */
    kidpal_keybar_scene_set(KVST_SCENE_TALK);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_TALK, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "复位后 set 不生效——复位冻结了场景写沿");
    kidpal_keybar_scene_reset();
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — 过渡窗缝对有效子态一律回落（冻结件只测了 READER 一值）。
 * 「过渡窗」的判据是 story_sub == -1；只要子态有效（含 LIST/STAMP/DELETE/
 * BODY_UNAVAILABLE 这些不在在途集的有效子态、乃至越界值），裁决缝必须零触碰回落，
 * 把裁决权交还调用方既有路径（既有子态表 / 查字窗 / 通用分支）。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: transition seam falls back for every valid story substate",
          "[chrome_ghost][builder][linux]")
{
    static const int valid_subs[] = {
        KSTORY_PAGE_LIST, KSTORY_PAGE_READER, KSTORY_PAGE_RECORDING,
        KSTORY_PAGE_STAMP, KSTORY_PAGE_DELETE, KSTORY_PAGE_BODY_UNAVAILABLE,
        KSTORY_PAGE_ORGANIZING
    };
    const char *sentinel_l = "sentinel-l";
    const char *sentinel_r = "sentinel-r";
    int iu;

    for (iu = 0; iu < (int)(sizeof(valid_subs) / sizeof(valid_subs[0])); iu++) {
        const char *left = sentinel_l;
        const char *right = sentinel_r;
        /* 即便「最后已知子态」在途，只要子态本身有效就不是过渡窗——不接管。 */
        const int rc = kidpal_keybar_story_transition_slots(
            valid_subs[iu], KSTORY_PAGE_RECORDING, 0, &left, &right);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc,
            CG_BUILDER_RED "有效子态被判成过渡窗（返回非 0）——既有子态表 / 查字窗 / "
            "通用分支路径会被本缝抢走");
        TEST_ASSERT_TRUE_MESSAGE(left == sentinel_l && right == sentinel_r,
            CG_BUILDER_RED "回落路径改写了 out_*——回落语义必须零触碰");
    }

    /* 越界（非枚举值）的子态同样回落：不是过渡窗就不接管。 */
    {
        const char *left = sentinel_l;
        const char *right = sentinel_r;
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_transition_slots(99, KSTORY_PAGE_READER, 0,
                                                 &left, &right),
            CG_BUILDER_RED "越界子态(99) 被判成过渡窗");
        TEST_ASSERT_TRUE_MESSAGE(left == sentinel_l && right == sentinel_r,
            CG_BUILDER_RED "越界子态回落路径改写了 out_*");
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — 帧分叉入参边界（rec_sub 只对 RECORDING 的二值分叉有意义）。
 * READER / ORGANIZING 的产物必须与 rec_sub 无关（纯函数无隐藏状态、同入参恒同出）;
 * RECORDING 的 rec_sub 语义 = 「0 / 非 0」，负值与正非零同帧（冻结件只测了 0 / 1）。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: transition seam rec_sub boundary (ignored off RECORDING)",
          "[chrome_ghost][builder][linux]")
{
    const char *l_a = NULL, *r_a = NULL;
    const char *ref_l = NULL, *ref_r = NULL;
    static const int recs[] = { 0, 1, -3, 2147483647 };
    int ir;

    /* READER：rec_sub 全值域同产物 == 既有冻结子态表。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_READER, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + READER 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_slots(KSTORY_PAGE_READER, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_slots(READER) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "READER 产物随 rec_sub 漂移——rec_sub 对非 RECORDING 子态必须"
            "零影响（纯函数无隐藏状态）");
    }

    /* ORGANIZING：rec_sub 全值域同产物 == 既有冻结子态表（双槽留空）。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_ORGANIZING, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + ORGANIZING 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_slots(KSTORY_PAGE_ORGANIZING, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_slots(ORGANIZING) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "ORGANIZING 产物随 rec_sub 漂移——整理态双槽留空与 rec_sub 无关");
    }

    /* RECORDING：rec_sub 只做 0 / 非 0 二值分叉——帧 A 与帧 B 各自对齐既有帧表,
     * 且负值（非 0）落帧 B 而非未定义行为。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        const int frame = (recs[ir] == 0) ? 0 : 1; /* 单源即 story_rec_slots 的分叉语义 */
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_RECORDING, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + RECORDING 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_rec_slots(frame, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_rec_slots(frame) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "过渡窗 RECORDING 的 rec_sub 分叉未与 story_rec_slots 同源"
            "（rec_sub 语义必须是 0 / 非 0 二值, 负值同帧 B）");
    }
}

/* -------------------------------------------------------------------------- *
 * 结构扫描助手（本件自用前缀 cgw_；cg_ 先例族同款：仓根注入 / 注释剥离 / 区切取 /
 * 裸源计数）。mcp_server.cc 是设备 TU, 不进 host 构建——源锚代理是唯一可断言面,
 * 注释剥离是防假绿的前提（正向锚只认可执行源字面, 260917-m2-k3 sealing pass 教训）。
 * ------------------------------------------------------------------------ */
#define CGW_MAX_PATH 1024
#define CGW_MAX_READ (8u * 1024u * 1024u)

static char cgw_root[CGW_MAX_PATH];
static int cgw_root_ready;

static int cgw_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int cgw_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[CGW_MAX_PATH];
    const char *env_root;
    size_t i;

    if (cgw_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (cgw_is_regular(probe)) {
            snprintf(cgw_root, sizeof(cgw_root), "%s", env_root);
            cgw_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (cgw_is_regular(probe)) {
            snprintf(cgw_root, sizeof(cgw_root), "%s", cands[i]);
            cgw_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *cgw_read(const char *rel)
{
    char p[CGW_MAX_PATH * 2];
    FILE *f;
    long sz;
    char *buf;

    if (!cgw_locate_root()) {
        return NULL;
    }
    snprintf(p, sizeof(p), "%s/%s", cgw_root, rel);
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > CGW_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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
    return buf;
}

/* 注释剥离（保持缓冲长度；字符串/字符字面量体保留）。调用方 free。 */
static char *cgw_strip(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL); /* logic guard: NULL source is a test bug, not a red */
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL);
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

/* 区切取：start_needle 首现起至（exclusive）其后首个 end_needle（无命中 -> EOF）。
 * NULL = 切取失败（锚漂移）。调用方 free。 */
static char *cgw_slice(char *src, const char *start_needle,
                       const char *end_needle)
{
    char *s, *e;
    char *slice;
    size_t span;

    if (src == NULL || start_needle == NULL) {
        return NULL;
    }
    s = strstr(src, start_needle);
    if (s == NULL) {
        return NULL;
    }
    if (end_needle == NULL ||
        (e = strstr(s + strlen(start_needle), end_needle)) == NULL) {
        e = src + strlen(src);
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    return slice;
}

/* 裸源子串出现计数（传入已剥注释窗时天然只计可执行源）。 */
static int cgw_count(const char *hay, const char *needle)
{
    int n = 0;
    const char *p = hay;
    size_t nl;

    if (hay == NULL || needle == NULL) {
        return 0;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return 0;
    }
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p++;
    }
    return n;
}

#define CGW_MCP_CC "firmware/main/mcp_server.cc"
#define CGW_APP_CC "firmware/main/application.cc"

/* -------------------------------------------------------------------------- *
 * TEST 4 — 引导播报子态看门狗复位沿幂等 + 音频感知窗（真机轮追补 261022；源锚代理）。
 *
 * 真机轮实证（KPLOG006 22:27:43-22:29:09）：引导 TTS 多句流式播放中孩子继续说话
 * 触发 barge-in abort（引擎 abortHandle: clear_queues + 发 tts stop）——句 1 的
 * tts stop 正常到达（edge 0 @22:28:03.4），但句 2 的 tts start 帧竞态泄漏
 * （edge 1 同刻到达, 其音频与收尾 stop 已被 abort 清掉）-> 设备 rec_substate 卡 1
 * -> 键帽「打断/留空」悬挂 66 秒（直到 22:29:09 下一句引导的 stop 才解开）。
 * 根因在第三方引擎树（不可改），设备侧修韧性缺口 = 引导子态看门狗。
 *
 * r2（真机反馈 2026-10-22「12 秒打断窗口体验太差」）：12s 盲等改为**音频感知 3s**——
 * 到期回调先看 g_story_guide_audio_seen（application.cc 收包路径置位）: 见过音频
 * 不复位直接撤表（真句子在放, 等它的 stop 沿）; 没见过才复位（audio=none 日志）。
 * 3s 依据: 引擎 TTS 首包实测 ~1-2s, 1.5x 余量; 孤儿 start 恒无音频必命中。
 *
 * 设备 TU 不进 host 构建 -> 判据 = 源锚代理（diag_log TEST 4 先例形态；注释剥离后
 * 只认可执行源）。四锚：
 *   ① 到期回调**幂等复位 + 音频感知分叉**：仅在 rec_substate 仍为 1 时写 0（stop 沿
 *      已复位则不重渲——与 KidpalStoryGuideSpeechEdge 的幂等沿语义汇合），见过音频
 *      早退不复位，且一次性自删；
 *   ② start 沿布防（清音频标志 = 开新窗） / stop 沿撤防（每句重布防, 先撤旧再建新）；
 *   ③ 会话收尾路径撤防（跨会话泄漏防线, D4b 根因形态）；
 *   ④ application.cc 收包路径置位（状态门内、解码入队处）——缺此锚则标志恒 false,
 *      3s 窗会把正常播放的句子也当孤儿帧误伤。
 * 到期后的行为半（键帽真的复位、真机上 3s 内自愈）= 真机观测项（CG-A5 口径）。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: guide speech watchdog — idempotent reset edge and disarm paths",
          "[chrome_ghost][builder][structural]")
{
    char *raw;
    char *code;
    char *cb_win;
    char *arm_win;
    char *edge_win;
    char *end_win;
    char *app_raw;
    char *app_code;
    char *app_win;

    TEST_ASSERT_TRUE_MESSAGE(cgw_locate_root(),
        CG_BUILDER_RED "repo root not located — run from the repo root, "
        "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, "
        "or pass KIDPAL_REPO_ROOT");

    raw = cgw_read(CGW_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw,
        CG_BUILDER_RED "mcp_server.cc unreadable（源锚扫描面失效）");
    code = cgw_strip(raw);
    free(raw);
    TEST_ASSERT_NOT_NULL(code);

    /* ---- ① 到期回调：音频感知分叉 + 幂等复位 + 一次性自删 ------------------ */
    cb_win = cgw_slice(code, "void StoryGuideWatchdogCb(lv_timer_t* timer)",
                       "void ArmStoryGuideWatchdog()");
    TEST_ASSERT_NOT_NULL_MESSAGE(cb_win,
        CG_BUILDER_RED "看门狗到期回调区切取失败——引导子态看门狗未落（KPLOG006 "
        "22:28:03 泄漏 start 帧 -> rec_substate 卡 1 -> 键帽悬挂 66s；设备侧韧性"
        "缺口 = 无到期兜底复位）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "g_story_view.rec_substate != 1") >= 1,
        CG_BUILDER_RED "看门狗回调缺幂等门（rec_substate != 1 早退）——stop 沿已复位"
        "的场合到期仍写 0 + 重渲 = 多余整页重绘抖动（复位沿必须与既有幂等沿语义汇合）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "g_story_guide_audio_seen") >= 1,
        CG_BUILDER_RED "看门狗回调缺音频感知判据（g_story_guide_audio_seen）——r2 收紧"
        "到 3s 后, 无此判据会把正常播放的句子（音频在放、stop 未到）当孤儿帧误伤, "
        "键帽在句中被打回收尾/丢弃（比 12s 盲等更坏的回归）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "audio=none") >= 1,
        CG_BUILDER_RED "看门狗复位日志缺 audio=none 标记——真机轮判读分不清「孤儿 start "
        "无音频复位」与「真音频场景误判」（KPLOG 判读面回归）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "g_story_view.rec_substate = 0") >= 1,
        CG_BUILDER_RED "看门狗回调无子态复位写——到期复位本体缺失（悬挂键帽治不了）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "ScheduleStoryRender()") >= 1,
        CG_BUILDER_RED "看门狗回调复位后零渲染同步——渲染节点缓存与键帽分叉读不到新"
        "子态, 屏幕仍显「打断/留空」");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(cb_win, "lv_timer_delete(timer)") >= 1,
        CG_BUILDER_RED "看门狗非一次性（回调未自删）——到期后按周期反复触发, 常驻"
        "定时器开销与重复复位风险（StoryRecTimerCb 自删先例同款纪律）");
    free(cb_win);

    /* ---- ①b 时限常量：音频感知 3s（r2） ----------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        cgw_count(code, "#define KSTORY_GUIDE_SPEECH_WD_TIMEOUT_MS 3000") == 1,
        CG_BUILDER_RED "看门狗时限不是音频感知 3s（r2, 真机反馈 12s 打断窗口体验差）"
        "——盲等越短+音频判据兜正确性, 孤儿帧才能秒级自愈");

    /* ---- ② 沿桥：start 布防（清音频标志） / stop 撤防 ---------------------- */
    arm_win = cgw_slice(code, "static void ArmStoryGuideWatchdog()",
                        "int KidpalStoryViewPageState()");
    TEST_ASSERT_NOT_NULL_MESSAGE(arm_win,
        CG_BUILDER_RED "ArmStoryGuideWatchdog 区切取失败——布防函数锚漂移");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(arm_win, "g_story_guide_audio_seen = false") >= 1,
        CG_BUILDER_RED "布防未清音频感知标志——旧窗口的 true 会跨句残留（多句流每句"
        "重布防）, 孤儿句继承前句 true 即漏判, 3s 兜底失效");
    free(arm_win);

    edge_win = cgw_slice(code, "void KidpalStoryGuideSpeechEdge(int speaking)",
                         "mcp_json_get_string");
    TEST_ASSERT_NOT_NULL_MESSAGE(edge_win,
        CG_BUILDER_RED "KidpalStoryGuideSpeechEdge 区切取失败——既有沿桥锚漂移");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(edge_win, "ArmStoryGuideWatchdog()") >= 1,
        CG_BUILDER_RED "引导播报置位沿未布防看门狗——泄漏的 tts start 帧（其 stop 被 "
        "abort 清掉）无到期兜底, 子态悬挂复发（KPLOG006 22:28:03）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(edge_win, "StopStoryGuideWatchdog()") >= 1,
        CG_BUILDER_RED "引导播报复位沿未撤防看门狗——正常播完/被打断后残留定时器会在"
        "3s 后对无关会话误判");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(edge_win, "g_story_view.rec_substate == next") >= 1,
        CG_BUILDER_RED "既有幂等沿被删——重复 tts start 将整页重渲抖动");
    free(edge_win);

    /* ---- ③ 会话收尾路径撤防（跨会话泄漏防线） ----------------------------- */
    end_win = cgw_slice(code, "void KidpalStorySessionEndApply(int list_page)",
                        "void KidpalStoryGuideSpeechEdge(int speaking)");
    TEST_ASSERT_NOT_NULL_MESSAGE(end_win,
        CG_BUILDER_RED "KidpalStorySessionEndApply 区切取失败——会话收尾桥锚漂移");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(end_win, "StopStoryGuideWatchdog()") >= 1,
        CG_BUILDER_RED "会话收尾路径未撤看门狗——定时器跨会话存活（D4b 根因形态: "
        "收尾后首页重建, 旧定时器仍对下一个会话的子态判决权在手）");
    free(end_win);

    free(code);

    /* ---- ④ application.cc 收包路径置位（音频感知判据的写端） --------------- */
    app_raw = cgw_read(CGW_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(app_raw,
        CG_BUILDER_RED "application.cc unreadable（音频感知标志定义/置位面失效）");
    app_code = cgw_strip(app_raw);
    free(app_raw);
    TEST_ASSERT_NOT_NULL(app_code);
    TEST_ASSERT_TRUE_MESSAGE(
        cgw_count(app_code, "extern \"C\" bool g_story_guide_audio_seen;") >= 1 &&
        cgw_count(app_code, "bool g_story_guide_audio_seen = false") >= 1,
        CG_BUILDER_RED "application.cc 缺 g_story_guide_audio_seen 定义（extern \"C\" "
        "声明 + 定义两行, 见应用侧注释）——mcp_server.cc 的 extern 声明将链接失败"
        "或读到错链接符号");
    app_win = cgw_slice(app_code, "protocol_->OnIncomingAudio(", "protocol_->OnAudioChannelOpened(");
    TEST_ASSERT_NOT_NULL_MESSAGE(app_win,
        CG_BUILDER_RED "OnIncomingAudio 收包区切取失败——音频路径锚漂移（置位面无法"
        "判读）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(app_win, "g_story_guide_audio_seen = true") >= 1,
        CG_BUILDER_RED "收包路径未置音频感知标志——标志恒 false, 3s 窗会把每一句正常"
        "播放当成孤儿帧, 句中断帽子态被打回复位（r2 回归主形态）");
    TEST_ASSERT_TRUE_MESSAGE(cgw_count(app_win, "PushPacketToDecodeQueue") >= 1,
        CG_BUILDER_RED "收包区解码入队调用消失——置位锚与真实音频路径脱钩（音频路径"
        "零行为变更前提被破坏）");
    free(app_win);
    free(app_code);
}
