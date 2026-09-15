/*
 * Frozen acceptance tests — TaskPackage 260911-render-pages（批次③ 渲染观感 + V3
 * 整屏还原双轨）RP-A11 新产 V3 保真齿（红基线先行）。
 * (subsystem: firmware; 承载: RP-A8 页面换皮框族面 / RP-A9 五态演出换皮内容面 /
 *  RP-A4 V3 字体分型（S0.1 决策件 delivery/changes/dev/260911-render-pages/font-decision-s01.md）;
 *  dependsOn 260911-screen-unification（聊天 layer 底座, 已合并 d80e319）).
 *
 * Nature: host-runnable STRUCTURAL teeth（schedule_p2 / story_p3 / screen_unification
 * 树级扫描先例）PLUS a frozen pure-logic offline contract for the 五态演出内容选择面
 * （voice_roundtrip_b / schedule_p2 weak-symbol 先例）: the builder MUST implement the
 * pure-logic contract below; the __attribute__((weak)) defaults return the sentinel -2
 * (KVST_UNIMPLEMENTED), so the red baseline is RED on real missing behaviour ("frozen
 * contract not implemented" / absent structural facts), never on a build/link error.
 *
 * ===========================================================================
 * 与 PKG-2（screen_unification.acceptance.c）的断言面拆分（零重叠, arch-review-
 * expansion §B.1）: PKG-2 齿断言 layer 化结构面（kidpal_layer 接线计数 / 单屏不变式 /
 * 零残留名 / 注册序）；本文件齿断言五态演出**内容面**（五态姿态资产引用 / 气泡 /
 * 键帽条 / 选择逻辑表 / d50 压暗 / 页面换皮框族 / 字体分型）。application.cc 三符号
 * （SetChatMessage/SetStatus/SetEmotion）双登记 = 语义源锚点级别共享, 本文件只作
 * must-stay-green 守卫, 不改其语义（RP-A9: layer 底座与优先级仲裁零触碰）。
 *
 * ===========================================================================
 * PKG-2 七不变式布防（arch-review-expansion §B.2, 本齿对新代码的扫描面即真实工作树）:
 * 本文件自身落在 test/（全部扫描器排除目录）, 不引入任何不变式冲突; builder 落地
 * 五态/换皮代码时必须遵守: 零残留五字符串（含注释）/ 不自建屏（lv_obj_create(NULL)
 * 恰 1 处）/ lv_screen_load 实参恒 s_kidpal_scr / firmware/main/CMakeLists.txt 既有
 * 条目不动（本文件接线只 ADD 一行）/ kidpal_layer 底座零触碰（chat_top[] 成员表
 * 内容级维护除外）。
 *
 * ===========================================================================
 * lcd_display.cc 双分支扫描陷阱布防（arch-review-expansion §B.3, P2-A1 同型教训）:
 * lcd_display.cc 有 #if CONFIG_USE_WECHAT_MESSAGE_STYLE 双分支（:358-:773 WeChat 死
 * 分支 / :773 #else 后活分支 :774-:970 + 其后共享面 SetEmotion/KidPalReparentChatToLayer）。
 * 本板 sdkconfig 未设 CONFIG_USE_WECHAT_MESSAGE_STYLE（firmware/sdkconfig:729）, 活分支
 * = 非 WeChat。**本文件全部对 lcd_display.cc 的断言禁 first-match / 全文件裸计数**:
 * 定位依据 = 「最后一个 void LcdDisplay::SetupUI() 定义出现点」——活分支 SetupUI 在
 * :774（最后一个）, WeChat 孪生 SetupUI 在 :359（第一个）; 死分支区间 = [首个
 * "#if CONFIG_USE_WECHAT_MESSAGE_STYLE", 最后一个 SetupUI 定义点), 活侧区间 = [最后
 * 一个 SetupUI 定义点, 文件尾)（含活分支全部内容 + 其后共享代码）。五态内容驱动的
 * 断言只认活侧区间命中 + 死分支区间零命中（封死「死分支里埋符号让齿假绿」路径）。
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure C, host-compilable, no
 * chip/FreeRTOS/ESP-component deps; recommended home:
 * firmware/main/display/kidpal_voice_states.{c,h}, 照 kidpal_layer.c 先例: 无
 * chip/LVGL 依赖、host 可编、weak 哨兵 + 强符号覆盖; arch-review-expansion §A.3
 * advisory 采纳裁定: acceptance-author 判定五态齿需要真实行为红基线, 源码扫描齿
 * 不足以钉「五态 → 姿态/气泡/键帽」内容映射, 故冻结纯逻辑契约):
 *
 *   #define KVST_UNIMPLEMENTED (-2)   // weak 哨兵 — 永不是合法返回值
 *
 *   --- 五态场景码（冻结, 数值不得偏移; 语义源 = application.cc:713-751 设备态分支
 *       + reminder_page.c 提醒弹窗 + low_battery_popup_ 低电, 见 intake-survey-s02-s03.md
 *       S0.3 落点清单） ---
 *   #define KVST_SCENE_LISTEN   0   -- 聆听 (pose-listen + 气泡「我在听！」)
 *   #define KVST_SCENE_THINK    1   -- 思考 (pose-think + 问号)
 *   #define KVST_SCENE_TALK     2   -- 播报 (pose-talk + 声波条 + 同步文字框)
 *   #define KVST_SCENE_REMIND   3   -- 提醒 (pose-jump + d50 压暗弹窗)
 *   #define KVST_SCENE_LOWBATT  4   -- 低电打盹 (pose-sleep + zZZ)
 *
 *   --- 姿态码（冻结; 映射 B8 闭集 KAP1 资产 pose_<name>_96.bin） ---
 *   #define KVST_POSE_LISTEN 0
 *   #define KVST_POSE_THINK  1
 *   #define KVST_POSE_TALK   2
 *   #define KVST_POSE_JUMP   3
 *   #define KVST_POSE_SLEEP  4
 *
 *   typedef struct {
 *       int pose;               -- KVST_POSE_* (该态小伴姿态)
 *       const char *bubble;     -- V3 气泡文案 (frm-sm 手绘圆角框 + 左下斜切尾巴);
 *                                  NULL = 该态无气泡（如思考态问号走 emotion 面）
 *       const char *keycap;     -- 底部键帽条 KEY 短语 (V3 §06, 恒非空; 键帽条任何
 *                                  状态常驻, 随五态联动换文案)
 *   } kidpal_voice_look_t;
 *
 *   int kidpal_voice_state_look(int scene, kidpal_voice_look_t *out);
 *       五态内容选择单源（换皮 owner 面）。scene -> *out:
 *         LISTEN  -> pose LISTEN, bubble "我在听！", keycap "说完啦"
 *         THINK   -> pose THINK,  keycap "不等了"   (bubble 不锁: 问号/气泡文案
 *                    归 design review, 只锁 bubble 为 NULL 或非空串)
 *         TALK    -> pose TALK,   keycap "打断"     (声波条/同步文字框为 LVGL 接线
 *                    面, 不在本纯逻辑契约)
 *         REMIND  -> pose JUMP,   keycap "知道了"
 *         LOWBATT -> pose SLEEP,  keycap "叫醒它"
 *       键帽短语 = V3 §06 逐字（kidpal-ui-design-v3.html 五态键帽行）, 不得偏移。
 *       out == NULL 或 scene 非法（<0 或 >LOWBATT）-> -1。返回 0 成功。
 *
 *   接线要求（结构性齿承载, 见 coverage map）:
 *     - LcdDisplay 活分支内容面（SetChatMessage/SetEmotion + LvglDisplay::SetStatus
 *       落点）改写为五态驱动: 活侧调用 kidpal_voice_state_look（lcd_display.cc 活
 *       侧区间命中 + WeChat 死分支零命中）;
 *     - 四新增姿态资产 pose_listen/pose_think/pose_talk/pose_sleep_96.bin 由内容面
 *       消费（pose_jump/pose_daily 既有消费为 must-stay 守卫）;
 *     - 底部键帽条任何状态常驻（V3 §06 硬要求）, 提示短语随五态联动;
 *     - V3 气泡尾巴资产 bubble_tail_24.bin（U 闭集修订新增的唯一 KAP1, 见
 *       ui_art_integration.acceptance.c REVISION LOG 2026-09-12）由气泡面消费。
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / decisionCoverage -> test):
 *   coverage: RP-A11 (TaskPackage 260911-render-pages) -> 本文件整体（V3 保真齿
 *               由 acceptance-author 红基线先行新产承载; crossEnd counterparts 11
 *               条 = 本文件对端断言面, 逐齿落点见上行各行）
 *   coverage: RP-A9  behavioral -> test 1 (五态选择纯逻辑: 五场景 pose/气泡/键帽
 *               逐字 + 非法场景/NULL out 防御)
 *   coverage: RP-A9  structural -> test 2 (五态内容驱动接线: lcd_display.cc 活侧
 *               驱动 kidpal_voice_state_look + WeChat 死分支零命中 + application.cc
 *               三语义源锚点在世 + LvglDisplay::SetStatus 在世)
 *   coverage: RP-A9  content   -> test 3 (五态姿态资产引用 4 新增 + 键帽条面 +
 *               气泡尾巴消费; pose_jump/pose_daily must-stay)
 *   coverage: RP-A9/F4 structural -> test 4 (提醒态 d50 网点压暗替换 LV_OPA_50)
 *   coverage: RP-A8  structural -> test 5 (P2/P3/P4 页面换皮框族接线: frm_ + 网点;
 *               涂鸦观感/点位 = design review / 真机轮, 不进脆断言)
 *   coverage: RP-A4  structural (S0.1 字体分型) -> test 6 (SPIFFS 非 KAP1 字体资产
 *               闭集: fonts_root 冻结落地 + 白名单闭集 + KAP1 magic 排除 +
 *               spiffs_root 字体零侵入（B8 test 3/4 不误伤）+ 加载接线)
 *   guards (must-stay-green): 各 test 内注明的语义源锚点 / 既有消费在世守卫 +
 *               扫描器自检（防假绿, test 6）。
 * RED baseline (2026-09-12 acceptance-author 冻结时点, 真实可红):
 *   - test 1: kidpal_voice_state_look 未实现（weak 哨兵 -2）;
 *   - test 2: lcd_display.cc 活侧无 kidpal_voice_state_look 驱动（五态演出零实施）;
 *   - test 3: 四新增姿态资产/键帽条面/气泡尾巴消费零命中（实现树仅 pose_daily/
 *             pose_jump 两枚在用）;
 *   - test 4: reminder_page.c 压暗仍 LV_OPA_50（:1144, F4 成立）、零 d50 网点消费;
 *   - test 5: 三个换皮页面零 frm_/零网点引用（纯 LVGL 控件堆叠现状）;
 *   - test 6: fonts_root 未落地、三字体零接线（S0.1 决策未实施）。
 *   guards native green（语义源锚点/既有消费/扫描器自检）。
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   本文件在 HOST_ACCEPTANCE_SRCS 中必须排在 host_infra_smoke.acceptance.c 之前
 *   （smoke 恒列表末位 = 包 0 boundary 齿注册序守卫; 本文件无渲染齿, 不影响该守卫;
 *   screen_unification s2 齿机械核查注册序）。builder 落地 kidpal_voice_states.c 后
 *   将 "../../../display/kidpal_voice_states.c" 一并接线（强符号覆盖 weak 哨兵）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-12  initial freeze (acceptance-author). RP-A11 V3 保真齿红基线先行:
 *               五态纯逻辑契约 + 五枚结构齿 + must-stay 守卫; 字体分型落点裁定于
 *               本文件（与 ui_art_integration B8 闭集齿分型, 不重复建闭集守门,
 *               仅 fonts_root 新闭集 + spiffs_root 字体零侵入）。
 *   2026-09-12  冻结验收自身冲突修正（builder 实现暴露, tightly-only: 修正不可
 *               实现的断言不构成放松）：test 6 原判据「spiffs_root font_ 前缀
 *               条目即红」与 U 包冻结 ART-A5 互斥——font_zcool_30.bin（300×52
 *               KAP1 位图样张）是 48 枚闭集白名单成员、必须落 spiffs_root, 任何
 *               符合契约的实现都不可满足旧判据。修法：改钉「font_ 前缀条目必须
 *               携 KAP1 magic」——KAP1 样张豁免照旧、lv_font_conv 产物（非 KAP1
 *               新字体）仍被拦在 spiffs_root 外（B8 test 3 KAP1-load / test 4
 *               闭集枚举不误伤的机械前提不变）。冲突事实与依据见本条。
 *   2026-09-12  coverage: RP-A6 冻结面零越界在本件留痕声明承载：本件为
 *               acceptance-author 新产冻结件（initial freeze + 冲突 1 修订 +
 *               coverage map 补记），断言语义不放松，docs/contracts/** 与
 *               managed_components/** 零触碰。
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

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — constants + weak default.
 * ------------------------------------------------------------------------ */
#define KVST_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define KVST_SCENE_LISTEN   0
#define KVST_SCENE_THINK    1
#define KVST_SCENE_TALK     2
#define KVST_SCENE_REMIND   3
#define KVST_SCENE_LOWBATT  4

#define KVST_POSE_LISTEN 0
#define KVST_POSE_THINK  1
#define KVST_POSE_TALK   2
#define KVST_POSE_JUMP   3
#define KVST_POSE_SLEEP  4

typedef struct {
    int pose;
    const char *bubble;
    const char *keycap;
} kidpal_voice_look_t;

__attribute__((weak)) int kidpal_voice_state_look(int scene, kidpal_voice_look_t *out)
{
    (void)scene; (void)out;
    return KVST_UNIMPLEMENTED;
}

static void kvst_require_implemented(int value, const char *what)
{
    if (value == KVST_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KVST RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the five-state look contract per the frozen "
                 "contract in firmware/main/test/v3_restore.acceptance.c header, "
                 "wire display/kidpal_voice_states.c into HOST_ACCEPTANCE_SRCS + "
                 "firmware/main/CMakeLists.txt, and drive it from the LcdDisplay "
                 "active-branch content face)",
                 what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), KSP/KSTORY/SUNU 先例.
 * All scans run against the REAL working tree located from the runner cwd.
 * Directories named test / host_app / build / .git / assets are EXCLUDED from
 * every scan (test/ hosts THIS frozen file; assets are binary art, not
 * implementation surface). cwd 语义: 仓根 = 自 runner cwd 逐级上探
 * firmware/main/CMakeLists.txt, 不依赖裸 cwd（VRT-A2 教训）。
 * ------------------------------------------------------------------------ */

#define KVST_MAX_PATH 1024
#define KVST_MAX_READ (8u * 1024u * 1024u)

static char kvst_root[KVST_MAX_PATH];
static int kvst_root_ready;

static int kvst_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kvst_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kvst_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kvst_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KVST_MAX_PATH];

    if (kvst_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kvst_is_regular(probe)) {
            snprintf(kvst_root, sizeof(kvst_root), "%s", cands[i]);
            kvst_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kvst_rel(char *out, size_t cap, const char *rel)
{
    if (!kvst_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kvst_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kvst_root, rel);
    return 1;
}

static int kvst_exists(const char *rel)
{
    char p[KVST_MAX_PATH];
    return kvst_rel(p, sizeof(p), rel) && kvst_is_regular(p);
}

static int kvst_dir_exists(const char *rel)
{
    char p[KVST_MAX_PATH];
    return kvst_rel(p, sizeof(p), rel) && kvst_is_dir(p);
}

static char *kvst_read(const char *rel, size_t *out_len)
{
    char p[KVST_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kvst_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KVST_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int kvst_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kvst_file_contains(const char *rel, const char *needle)
{
    char *buf = kvst_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kvst_contains(buf, needle);
    free(buf);
    return r;
}

/* 子串最后一次出现位（lcd_display.cc 活侧定位用）; 无命中 -> NULL。 */
static const char *kvst_rfind(const char *hay, const char *needle)
{
    const char *last = NULL;
    const char *h;

    assert(hay != NULL && needle != NULL);
    for (h = hay; (h = strstr(h, needle)) != NULL; h += 1) {
        last = h;
    }
    return last;
}

static int kvst_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[KVST_MAX_PATH];
        char rel_next[KVST_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            kvst_is_skipped_dir(e->d_name)) {
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
            count += kvst_count_files_with_needle(abs_next, rel_next, needle,
                                                  first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KVST_MAX_READ) {
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
                if (kvst_contains(buf, needle)) {
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

static int kvst_scan_count(const char *rel_dir, const char *needle,
                           char *first_hit, size_t cap)
{
    char abs[KVST_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!kvst_rel(abs, sizeof(abs), rel_dir) || !kvst_is_dir(abs)) {
        return 0;
    }
    return kvst_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

#define KVST_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kvst_locate_root(), \
    "KVST RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf 风格断言包装（ui_art_integration artt_fm 同款先例）：unity 的 *_MESSAGE 宏
 * 只收单串消息, 带动态字段的失败诊断先格式化进静态缓冲再走原宏（原宏在条件为真时
 * 不求值消息参数, 静态缓冲单次使用安全）。 */
#include <stdarg.h>
static char kvst_fm_buf[1024];
static const char *kvst_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kvst_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kvst_fm_buf, sizeof(kvst_fm_buf), fmt, ap);
    va_end(ap);
    return kvst_fm_buf;
}
#define TEST_ASSERT_TRUE_FM(cond, ...) TEST_ASSERT_TRUE_MESSAGE((cond), kvst_fm(__VA_ARGS__))
#define TEST_ASSERT_NOT_NULL_FM(p, ...) TEST_ASSERT_NOT_NULL_MESSAGE((p), kvst_fm(__VA_ARGS__))

/* lcd_display.cc 活侧/死分支区间提取（双分支陷阱布防, 见文件头「定位依据」）。
 * 返回 0 成功: *dead_* / *active_* 为 buf 内偏移区间（buf 被 kvst_read 分配, 调用方
 * free）; 结构锚缺失（无 #if 行或无 SetupUI 定义）-> -1（守卫齿会红）。 */
static int kvst_lcd_split(char *buf, size_t *dead_off, size_t *active_off)
{
    const char *if_pos;
    const char *last_setupui;

    if_pos = strstr(buf, "#if CONFIG_USE_WECHAT_MESSAGE_STYLE");
    if (if_pos == NULL) {
        return -1;
    }
    last_setupui = kvst_rfind(buf, "void LcdDisplay::SetupUI()");
    if (last_setupui == NULL || last_setupui < if_pos) {
        return -1;
    }
    *dead_off = (size_t)(if_pos - buf);
    *active_off = (size_t)(last_setupui - buf);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * Test 1 — RP-A9 behavioral: 五态演出内容选择（纯逻辑冻结契约）。
 * 五场景 pose/气泡/键帽逐字（键帽 = V3 §06 五态键帽行逐字）+ 防御。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: 五态演出内容选择——pose/气泡/键帽单源 (RP-A9)",
          "[v3_restore][VR-A9][behavioral]")
{
    kidpal_voice_look_t look;
    int r;

    memset(&look, 0, sizeof(look));
    r = kidpal_voice_state_look(KVST_SCENE_LISTEN, &look);
    kvst_require_implemented(r, "kidpal_voice_state_look unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "VR-A9 RED: kidpal_voice_state_look(LISTEN) must succeed");
    TEST_ASSERT_TRUE_MESSAGE(look.pose == KVST_POSE_LISTEN,
        "VR-A9 RED: 聆听态 must map to pose-listen (V3 §06 小伴七姿态)");
    TEST_ASSERT_TRUE_MESSAGE(look.bubble != NULL && strcmp(look.bubble, "我在听！") == 0,
        "VR-A9 RED: 聆听态气泡必须逐字为「我在听！」(V3 §06 聆听态 / plan S7.1)");
    TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && strcmp(look.keycap, "说完啦") == 0,
        "VR-A9 RED: 聆听态键帽提示必须逐字为「说完啦」(V3 §06 键帽条)");

    memset(&look, 0, sizeof(look));
    r = kidpal_voice_state_look(KVST_SCENE_THINK, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && look.pose == KVST_POSE_THINK,
        "VR-A9 RED: 思考态 must map to pose-think");
    TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && strcmp(look.keycap, "不等了") == 0,
        "VR-A9 RED: 思考态键帽提示必须逐字为「不等了」(V3 §06)");
    TEST_ASSERT_TRUE_MESSAGE(look.bubble == NULL || look.bubble[0] != '\0',
        "VR-A9 RED: 思考态 bubble must be NULL or non-empty (文案观感归 design review)");

    memset(&look, 0, sizeof(look));
    r = kidpal_voice_state_look(KVST_SCENE_TALK, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && look.pose == KVST_POSE_TALK,
        "VR-A9 RED: 播报态 must map to pose-talk");
    TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && strcmp(look.keycap, "打断") == 0,
        "VR-A9 RED: 播报态键帽提示必须逐字为「打断」(V3 §06)");

    memset(&look, 0, sizeof(look));
    r = kidpal_voice_state_look(KVST_SCENE_REMIND, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && look.pose == KVST_POSE_JUMP,
        "VR-A9 RED: 提醒态 must map to pose-jump (小伴跳进窗里报信, V3 §06)");
    TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && strcmp(look.keycap, "知道了") == 0,
        "VR-A9 RED: 提醒态键帽提示必须逐字为「知道了」(V3 §06 [KEY] 确认关闭)");

    memset(&look, 0, sizeof(look));
    r = kidpal_voice_state_look(KVST_SCENE_LOWBATT, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && look.pose == KVST_POSE_SLEEP,
        "VR-A9 RED: 低电态 must map to pose-sleep (V3 §06 低电打盹)");
    TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && strcmp(look.keycap, "叫醒它") == 0,
        "VR-A9 RED: 低电态键帽提示必须逐字为「叫醒它」(V3 §06 情绪化沟通)");

    /* 防御: 非法场景 / NULL out */
    r = kidpal_voice_state_look(KVST_SCENE_LOWBATT + 1, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "VR-A9 RED: an invalid scene must be rejected (-1), never UB");
    r = kidpal_voice_state_look(-1, &look);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "VR-A9 RED: negative scene must be rejected (-1)");
    r = kidpal_voice_state_look(KVST_SCENE_LISTEN, NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "VR-A9 RED: NULL out must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * Test 2 — RP-A9 structural: 五态内容驱动接线（lcd_display.cc 活侧驱动选择器）。
 * 双分支陷阱布防见文件头; 定位依据（最后一个 SetupUI 定义点）写明, 禁 first-match。
 * RED today: 活侧无 kidpal_voice_state_look（五态演出零实施）。
 * guards (must stay green): application.cc 三语义源锚点 + LvglDisplay::SetStatus
 * + 双分支结构锚在世。
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: 五态内容驱动接线——lcd_display 活侧驱动选择器 (RP-A9)",
          "[v3_restore][VR-A9][structural]")
{
    char *buf;
    size_t dead_off, active_off;
    int n;

    KVST_REQUIRE_ROOT();

    TEST_ASSERT_TRUE_MESSAGE(kvst_exists("firmware/main/display/lcd_display.cc"),
        "VR-A9 RED(infra): firmware/main/display/lcd_display.cc missing — 对端符号面被移除");

    buf = kvst_read("firmware/main/display/lcd_display.cc", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "VR-A9 RED(infra): lcd_display.cc unreadable");

    /* 结构锚守卫: 双分支 + 活分支 SetupUI 定义在世（定位依据失效 = 立即红, 不静默）。 */
    TEST_ASSERT_TRUE_MESSAGE(kvst_lcd_split(buf, &dead_off, &active_off) == 0,
        "VR-A9 RED(infra): lcd_display.cc dual-branch anchors lost (#if "
        "CONFIG_USE_WECHAT_MESSAGE_STYLE / last SetupUI definition) — 本齿活侧定位"
        "依据失效, 须由 acceptance-author 重钉");

    /* 活侧驱动: SetChatMessage/SetEmotion 内容面改写为五态驱动（调用选择器）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(buf + active_off, "kidpal_voice_state_look") != NULL,
        "VR-A9 RED: the LcdDisplay ACTIVE branch (non-WeChat) content face does not "
        "drive the five-state look selector — SetChatMessage/SetEmotion 落点未改写为"
        "五态演出驱动 (RP-A9; 定位 = 最后一个 SetupUI 定义点之后的活侧区间, 禁"
        "first-match, 见文件头双分支布防)");

    /* 死分支零命中: WeChat 孪生分支里埋选择器引用不算接线（封死假绿路径）。 */
    {
        char saved = buf[active_off];
        buf[active_off] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(strstr(buf + dead_off, "kidpal_voice_state_look") == NULL,
            "VR-A9 RED: the WeChat-style DEAD branch (#if CONFIG_USE_WECHAT_MESSAGE_"
            "STYLE region) references the five-state selector — 活分支 = 非 WeChat "
            "(sdkconfig 未设该宏), 死分支里的引用不构成接线（双分支扫描陷阱布防）");
        buf[active_off] = saved;
    }
    free(buf);

    /* LvglDisplay::SetStatus 在世（内容面落点, PKG-2 合并后结构）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kvst_file_contains("firmware/main/display/lvgl_display/lvgl_display.cc",
                           "LvglDisplay::SetStatus"),
        "VR-A9 RED(infra): LvglDisplay::SetStatus definition lost from "
        "lvgl_display/lvgl_display.cc — 五态内容面落点被移除");

    /* 语义源锚点守卫（application.cc 三符号, 双登记共享面, 语义零改动）: */
    TEST_ASSERT_TRUE_MESSAGE(kvst_file_contains("firmware/main/application.cc",
                                                "->SetChatMessage(") &&
                                 kvst_file_contains("firmware/main/application.cc",
                                                    "->SetStatus(") &&
                                 kvst_file_contains("firmware/main/application.cc",
                                                    "->SetEmotion("),
        "VR-A9 RED(infra): application.cc semantic anchors (SetChatMessage/SetStatus/"
        "SetEmotion call faces) lost — 五态语义源被移除（本包零触碰红线）");

    /* 提醒态落点在世（五态之一的内容面载体）。 */
    n = kvst_scan_count("firmware/main", "kidpal_reminder_page_render", NULL, 0);
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "VR-A9 RED(infra): kidpal_reminder_page_render lost — 提醒态落点被移除");
}

/* -------------------------------------------------------------------------- *
 * Test 3 — RP-A9 content face: 五态姿态资产引用 + 键帽条面 + V3 气泡尾巴消费。
 * RED today: 四新增姿态资产 / 键帽条 / 气泡尾巴消费零命中（实现树仅 pose_daily /
 * pose_jump 在用）。
 * guards (must stay green): pose_jump / pose_daily 既有消费在世。
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: 五态姿态资产引用 + 键帽条 + 气泡尾巴消费 (RP-A9)",
          "[v3_restore][VR-A9][content]")
{
    char hit[KVST_MAX_PATH];
    static const char *const new_poses[] = {
        "pose_listen_96.bin", "pose_think_96.bin",
        "pose_talk_96.bin",   "pose_sleep_96.bin"
    };
    int n;

    KVST_REQUIRE_ROOT();

    for (size_t i = 0; i < sizeof(new_poses) / sizeof(new_poses[0]); i++) {
        n = kvst_scan_count("firmware/main", new_poses[i], hit, sizeof(hit));
        TEST_ASSERT_TRUE_FM(n >= 1,
            "VR-A9 RED: no implementation file under firmware/main references "
            "五态姿态资产 (first missing: %s) — 五态演出内容面未消费小伴七姿态 "
            "(RP-A9; 资产本体在 B8 闭集, 齿钉的是消费接线)", new_poses[i]);
    }

    /* 底部键帽条内容面（V3 §06「任何状态下底部提示条永不消失」的代码载体）。 */
    n = kvst_scan_count("firmware/main", "键帽", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "VR-A9 RED: no implementation file under firmware/main carries the 键帽条 "
        "content face — V3 §06 底部键帽提示条零实施 (RP-A9)");

    /* V3 气泡尾巴消费（资产本体 = U 闭集修订新增唯一 KAP1, 落盘对账在
     * ui_art_integration ART-A5; 本齿钉内容面消费接线）。 */
    n = kvst_scan_count("firmware/main", "bubble_tail_24.bin", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "VR-A9 RED: no implementation file under firmware/main consumes the V3 "
        "bubble-tail asset bubble_tail_24.bin — V3 气泡（frm-sm + 左下斜切尾巴）"
        "零实施 (RP-A9 / S0.1 决策件)");

    /* guards: 既有姿态消费在世（盖章帧/提醒弹窗/P2 课表, 零越界红线）。 */
    n = kvst_scan_count("firmware/main", "pose_jump_96.bin", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "VR-A9 RED(infra): pose_jump_96.bin consumption lost — 盖章帧/提醒弹窗既有"
        "姿态消费被移除（换皮必须 additive, ART-E3 纪律）");
    n = kvst_scan_count("firmware/main", "pose_daily_96.bin", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "VR-A9 RED(infra): pose_daily_96.bin consumption lost — P2 课表页既有姿态"
        "消费被移除（换皮必须 additive, ART-E3 纪律）");
}

/* -------------------------------------------------------------------------- *
 * Test 4 — RP-A9/F4: 提醒态压暗 = d50 网点（非 LV_OPA_50 半透明）。
 * F4（arch-review-expansion §C）: reminder_page.c :1144 仍 LV_OPA_50, 文件头注释
 * :975 声称 d50 网点但代码未兑现——本齿钉「注释与实现一致」。
 * RED today: LV_OPA_50 在世 + 零 d50 网点消费。
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: 提醒态 d50 网点压暗替换 LV_OPA_50 (RP-A9/F4)",
          "[v3_restore][VR-F4][structural]")
{
    char *buf;

    KVST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(kvst_exists("firmware/main/display/reminder_page.c"),
        "VR-F4 RED(infra): firmware/main/display/reminder_page.c missing — 提醒弹窗"
        "消费面被移除");

    buf = kvst_read("firmware/main/display/reminder_page.c", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "VR-F4 RED(infra): reminder_page.c unreadable");

    TEST_ASSERT_TRUE_MESSAGE(
        kvst_contains(buf, "d50.bin") || kvst_contains(buf, "kidpal_art_draw_dots"),
        "VR-F4 RED: reminder_page.c does not consume the d50 dot asset — 提醒弹窗"
        "压暗未换 d50 网点（V3 §06「整屏压 50% 网点强制聚焦」; F4: 注释声称 d50 "
        "但实现仍是半透明填充）");

    TEST_ASSERT_TRUE_MESSAGE(!kvst_contains(buf, "LV_OPA_50"),
        "VR-F4 RED: reminder_page.c still uses LV_OPA_50 — 提醒态压暗仍是 LVGL 半透明"
        "填充而非 d50 网点（F4 成立面; 换皮后该词必须整体退役, 不得双源并存）");
    free(buf);
}

/* -------------------------------------------------------------------------- *
 * Test 5 — RP-A8: P2/P3/P4 页面换皮框族接线（9 宫格 frm_ + 网点）。
 * LVGL 骨架与动态文本语义保留（既有冻结齿承载）, 本齿钉换皮**内容面**:
 * 三个换皮页面消费手绘框族资产（frm_236_236 / frm_sm_160_120 / frm_flat_240_96
 * 名族）, 课表页消费网点弱底纹。RED today: 零 frm_/零网点引用（纯 LVGL 控件堆叠）。
 * 涂鸦（星/云/波浪 ≤2 处/屏）与单屏倾斜 ≤3 处为观感纪律, 归 design review /
 * 真机对照轮（RP-A5 human 持板）, 不进脆断言（acceptance-evidence 两桶纪律）。
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: P2/P3/P4 页面换皮框族接线——frm_ + 网点 (RP-A8)",
          "[v3_restore][VR-A8][structural]")
{
    char *buf;

    KVST_REQUIRE_ROOT();

    TEST_ASSERT_TRUE_MESSAGE(kvst_file_contains("firmware/main/display/schedule_page.c",
                                                "frm_"),
        "VR-A8 RED: schedule_page.c does not reference the hand-drawn frame family "
        "(frm_) — P2 课表页换皮零实施 (RP-A8: frm/frm-sm/frm-flat 9 宫格铺开)");
    TEST_ASSERT_TRUE_MESSAGE(kvst_file_contains("firmware/main/display/story_page.c",
                                                "frm_"),
        "VR-A8 RED: story_page.c does not reference the hand-drawn frame family "
        "(frm_) — P3 故事页换皮零实施 (RP-A8)");
    TEST_ASSERT_TRUE_MESSAGE(kvst_file_contains("firmware/main/display/char_stroke.c",
                                                "frm_"),
        "VR-A8 RED: char_stroke.c does not reference the hand-drawn frame family "
        "(frm_) — P4 笔顺页换皮零实施 (RP-A8; 米字格大框单份背景 = frm_236_236, "
        "B8 打包策略)");

    /* 课表页网点弱底纹（P2, plan S5.2; d50/d25/d12 三档任一或 draw_dots 通路）。 */
    buf = kvst_read("firmware/main/display/schedule_page.c", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "VR-A8 RED(infra): schedule_page.c unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        kvst_contains(buf, "d50.bin") || kvst_contains(buf, "d25.bin") ||
            kvst_contains(buf, "d12.bin") || kvst_contains(buf, "kidpal_art_draw_dots"),
        "VR-A8 RED: schedule_page.c does not consume any dot asset (d50/d25/d12) — "
        "P2 课表页网点弱底纹零实施 (RP-A8; 网点单元 4px / 单屏面积 <=10% 归 V3 红线"
        "清单核对, RP-E4)");
    free(buf);
}

/* -------------------------------------------------------------------------- *
 * Test 6 — RP-A4/S0.1: 字体分型——SPIFFS 非 KAP1 字体资产闭集守门。
 * S0.1 决策（font-decision-s01.md）: 标题（站酷快乐体）/ 数字（Baloo 2）/ 正文
 * （自产 GB2312 6763 字子集）字体 = lv_font_conv 产物（非 KAP1）, SPIFFS 独立落地,
 * 不得并入 B8 KAP1 闭集（否则 ui_art_integration B8 test 3 KAP1-load / test 4
 * 闭集枚举 foreign 双红）。本齿 = 分型新齿:
 *   1) 字体落地根（冻结路径）firmware/main/assets/fonts_root/ 在世;
 *   2) 落地集 == 字体闭集白名单（3 枚, 双向闭包: 表外文件红 + 缺枚红）;
 *   3) 每枚首 4 字节 != "KAP1"（非 KAP1 分型红线, 防字体被误转 KAP1 位图）;
 *   4) firmware/main/assets/spiffs_root/ 字体分型守卫（REVISION LOG 2026-09-12
 *      第二条修订: font_ 前缀条目必须携 KAP1 magic——KAP1 位图样张豁免, lv_font
 *      新字体不得入 spiffs_root, B8 test 3/4 不误伤的机械前提）;
 *   5) 加载接线: firmware/main 实现树引用每枚字体文件名。
 * RED today: fonts_root 未落地、三字体零接线（S0.1 决策未实施）。
 * 含扫描器自检（防假绿, house discipline）。
 * ------------------------------------------------------------------------ */
TEST_CASE("v3 restore: 字体分型——SPIFFS 非 KAP1 字体资产闭集 (RP-A4/S0.1)",
          "[v3_restore][VR-A4][fonts]")
{
    char root[KVST_MAX_PATH];
    char spiffs[KVST_MAX_PATH];
    static const char *const font_whitelist[] = {
        "font_body.bin",          /* 正文: 自产 GB2312 6763 字子集（S0.1 决策 2） */
        "font_title_zcool.bin",   /* 标题: 站酷快乐体 30px 1bpp 子集（S0.1 决策 3） */
        "font_num_baloo.bin",     /* 数字: Baloo 2 子集（S0.1 决策 3/4, 非预渲染位图） */
    };
    int found = 0;
    int foreign = 0;
    char foreign_name[KVST_MAX_PATH];
    DIR *d;
    struct dirent *e;

    KVST_REQUIRE_ROOT();

    /* 扫描器自检（防假绿: 负向齿必须能红） */
    TEST_ASSERT_TRUE_MESSAGE(kvst_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(kvst_exists("firmware/main/display/__kvst_no_such__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(kvst_read("firmware/main/display/__kvst_no_such__.c", NULL),
        "boundary RED(infra): kvst_read returned data for a nonexistent file");
    {
        char hit[KVST_MAX_PATH];
        int n = kvst_scan_count("firmware/main/display", "__kvst_no_such_needle__", hit,
                                sizeof(hit));
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            "boundary RED(infra): scan matched a needle that cannot exist — 负向断言假绿");
    }

    /* 1) 字体落地根在世（冻结路径; 与 spiffs_root 平级独立, 非 KAP1 分型落点）。 */
    TEST_ASSERT_TRUE_MESSAGE(kvst_dir_exists("firmware/main/assets/fonts_root"),
        "VR-A4 RED: font landing root firmware/main/assets/fonts_root/ missing — "
        "S0.1 字体决策未实施（标题/数字/正文字体走 SPIFFS 非 KAP1 独立落地, "
        "font-decision-s01.md 决策 3）");
    if (!kvst_dir_exists("firmware/main/assets/fonts_root")) {
        return; /* 后续枚举无对象, 红已落 */
    }

    /* 2)+3) 落地集 == 白名单闭集 + 每枚非 KAP1。 */
    TEST_ASSERT_TRUE_MESSAGE(kvst_rel(root, sizeof(root), "firmware/main/assets/fonts_root"),
        "VR-A4 RED(infra): fonts_root path resolution failed");
    d = opendir(root);
    TEST_ASSERT_NOT_NULL_MESSAGE(d, "VR-A4 RED(infra): fonts_root not enumerable");
    foreign_name[0] = '\0';
    while ((e = readdir(d)) != NULL) {
        int known = 0;
        char p[KVST_MAX_PATH];
        FILE *f;
        uint8_t magic[4];

        if (e->d_name[0] == '.') {
            continue;
        }
        for (size_t i = 0; i < sizeof(font_whitelist) / sizeof(font_whitelist[0]); i++) {
            if (strcmp(e->d_name, font_whitelist[i]) == 0) {
                known = 1;
                break;
            }
        }
        if (!known) {
            foreign = 1;
            if (foreign_name[0] == '\0') {
                snprintf(foreign_name, sizeof(foreign_name), "%s", e->d_name);
            }
            continue;
        }
        found++;
        snprintf(p, sizeof(p), "%s/%s", root, e->d_name);
        f = fopen(p, "rb");
        TEST_ASSERT_NOT_NULL_MESSAGE(f, "VR-A4 RED(infra): whitelisted font unreadable");
        size_t got = fread(magic, 1, 4, f);
        fclose(f);
        TEST_ASSERT_TRUE_MESSAGE(got == 4, "VR-A4 RED: font file too small (<4B)");
        TEST_ASSERT_TRUE_FM(!(magic[0] == 'K' && magic[1] == 'A' &&
                              magic[2] == 'P' && magic[3] == '1'),
            "VR-A4 RED: %s carries the KAP1 magic — 字体是 lv_font_conv 产物, 不得"
            "按 KAP1 位图格式落盘（S0.1 分型红线; KAP1 通道只归 B8 位图资产）",
            e->d_name);
    }
    closedir(d);
    TEST_ASSERT_TRUE_FM(foreign == 0,
        "VR-A4 RED: fonts_root contains non-whitelisted entry \"%s\" — 字体闭集被"
        "侵入（表外零容身; 扩白名单走 acceptance-author REVISION LOG）",
        foreign_name[0] ? foreign_name : "?");
    TEST_ASSERT_TRUE_FM(
        found == (int)(sizeof(font_whitelist) / sizeof(font_whitelist[0])),
        "VR-A4 RED: fonts_root whitelist hit count = %d, expected %d — 字体闭集缺枚"
        "（S0.1 决策 2/3: 正文/标题/数字三字体必须全量落地）",
        found, (int)(sizeof(font_whitelist) / sizeof(font_whitelist[0])));

    /* 4) spiffs_root 字体零侵入（REVISION LOG 2026-09-12 第二条修订——冻结验收
     * 自身冲突修正）：原判据「font_ 前缀条目即红」与 U 包冻结 ART-A5 互斥——
     * font_zcool_30.bin（300×52 KAP1 位图样张）是 48 枚闭集白名单成员、必须落在
     * spiffs_root，任何符合契约的实现都不可满足旧判据（自相矛盾齿）。修法
     * （修正不可实现断言，收紧方向不变）：判据改钉「font_ 前缀条目必须携 KAP1
     * magic」——KAP1 位图样张（B8 闭集成员）豁免照旧，lv_font_conv 产物（非
     * KAP1 新字体）不得入 spiffs_root；B8 test 3 KAP1-load / test 4 闭集枚举
     * 不误伤的机械前提保持（lv_font bin 仍被拦在闭集外）。 */
    TEST_ASSERT_TRUE_MESSAGE(kvst_rel(spiffs, sizeof(spiffs), "firmware/main/assets/spiffs_root"),
        "VR-A4 RED(infra): spiffs_root path resolution failed");
    d = opendir(spiffs);
    TEST_ASSERT_NOT_NULL_MESSAGE(d, "VR-A4 RED(infra): spiffs_root not enumerable");
    while ((e = readdir(d)) != NULL) {
        char p[KVST_MAX_PATH];
        FILE *f;
        uint8_t magic[4];
        size_t got;

        if (strncmp(e->d_name, "font_", 5) != 0) {
            continue; /* 非 font 类条目归 B8 test 4 闭集枚举管辖 */
        }
        snprintf(p, sizeof(p), "%s/%s", spiffs, e->d_name);
        f = fopen(p, "rb");
        TEST_ASSERT_NOT_NULL_FM(f,
            "VR-A4 RED(infra): spiffs_root font-class entry \"%s\" unreadable",
            e->d_name);
        got = fread(magic, 1, 4, f);
        fclose(f);
        TEST_ASSERT_TRUE_FM(got == 4 && magic[0] == 'K' && magic[1] == 'A' &&
                                magic[2] == 'P' && magic[3] == '1',
            "VR-A4 RED: spiffs_root font-class entry \"%s\" is NOT a KAP1 bitmap — "
            "lv_font_conv 字体不得入 spiffs_root（B8 KAP1 闭集只收位图资产; 字体唯一"
            "合法落点 = fonts_root）; KAP1 样张（如 font_zcool_30.bin）豁免照旧",
            e->d_name);
    }
    closedir(d);

    /* 5) 加载接线: 实现树引用每枚字体文件名。 */
    for (size_t i = 0; i < sizeof(font_whitelist) / sizeof(font_whitelist[0]); i++) {
        char hit[KVST_MAX_PATH];
        int n = kvst_scan_count("firmware/main", font_whitelist[i], hit, sizeof(hit));
        TEST_ASSERT_TRUE_FM(n >= 1,
            "VR-A4 RED: no implementation file under firmware/main references font "
            "asset %s — 字体加载接线缺失（S0.1: 正文/标题/数字逐字钉三分道路径, "
            "本枚 = SPIFFS 非 KAP1）", font_whitelist[i]);
    }
}

/* --------------------------------------------------------------------------- *
 * 附注（不在断言面，留根）：
 * - RP-A5（真机整屏 V3 对照轮）= human 持板证据轮, 本文件不承载（观感/两句检验题
 *   归真机证据, 落 delivery/changes/dev/260911-render-pages/）。
 * - V3 §07 红线清单（除 #000/#fff 无第三色 / 线宽 >=3px / 无渐变投影 / 动效白名单
 *   眨眼 4.4s·嘴部 240ms·声波·笔顺 950ms 等）= design review / 真机轮核对面
 *   （RP-E4 evidence）, 逐项机判会产出大量脆断言, 不进本冻结文件。
 * - 涂鸦四类用途白名单 / 单屏倾斜 <=3 处：同上, 真机对照轮核对。
 * - F3 rebase 活口已消解（arch-review-expansion §A）: 本文件五态齿对端符号全部
 *   已在树并钉实（lcd_display.cc 活侧 / lvgl_display.cc / reminder_page.c）,
 *   无需「符号 rebase 校正」条款。
 * --------------------------------------------------------------------------- */
