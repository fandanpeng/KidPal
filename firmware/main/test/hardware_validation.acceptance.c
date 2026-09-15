/*
 * Frozen acceptance tests — TaskPackage 260829-hardware-validation（包 9b）
 * (subsystem: firmware+server; 物理冒烟 + 端到端真机验收 + F2 故事生产接线 +
 *  v0.4 续航基准/常驻 WS 功耗/证书握手; mergeOrder 13; rigor=full; riskTier=high;
 *  testHost: device; dependsOn 2/3/4/5/6/7/8/U 全 merged).
 *
 * Story: 把 B 段在 host/PC 验过的能力放到真实硬件上确认（按键手感/双麦/RTC 断电续航/
 * 低电/RLCD 真屏观感 + 全功能端到端），并承接 human 裁的两个扩界项：F2 故事生产接线
 * （录制→归档→盖章）+ ECS 部署 live 五齿；v0.4 新增实机续航基准/常驻 WS 功耗观察/
 * 证书真实设备握手冒烟。
 *
 * Nature: host-runnable STRUCTURAL + guard frozen acceptance（device_ws_switch /
 * voice_roundtrip_b / story_p3 tree-scan 先例）。本文件**不引入新的 frozen offline
 * contract**——F2 设备侧四个符号（kidpal_story_page_input / _on_saved /
 * _rec_tick / _stamp_spec）的纯逻辑/盖章几何契约已由 story_p3.acceptance.c 冻结，
 * 本文件只钉它们的**生产接线**（「函数落盘且被生产路径调用 ≥1」= 装配事实，非浮文件）；
 * driver 绑定（RTC/SHTC3/麦/低电打盹阈值=15）与资产/SPIFFS 沿用（U 已并、
 * 不重造）是结构齿。红基线红在真实缺失行为/结构（接线缺口、driver 未绑定），
 * 不是链接/编译错误（本文件 BUILD_CLEAN 佐证）。
 *
 * HWV-A2~A8 / 续航基准 / 证书握手的**物理真机半**（按键手感、双麦采音、RLCD 观感、
 * 16px 以下 CJK 1bpp 可读性、前光、续航实测）全部落在
 * delivery/taskpackages/260829-hardware-validation/device-acceptance-checklist.md
 * （human 持板执行 + 逐项勾验记录回写），host 段无法冒充真机已验（HWV-E2 证据纪律）——
 * 本文件绝不把真机事实伪造成 host 断言。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria / 新增 ID -> test):
 *   coverage: HWV-A1  structural（kind=structural）-> 不落 C 断言, 仅登记;
 *             结构性承载并入 HWV-A4/A5/A6 结构半（test 2/3）。
 *   coverage: F2 设备侧输入生产接线（新增 HWV-A9 建议，绑定 sourceRefs=story-p3 §9
 *             F7 殿后 / intake 裁定「扩界接线」）
 *             -> test 1（story_page input/on_saved/rec_tick/stamp_spec 四函数
 *                「落盘且被生产路径调用 ≥1」：三个跨文件 >=2 文件 + stamp_spec 在
 *                story_page.c 内被渲染路径消费 >=2 处）
 *   coverage: HWV-A4 结构半（RTC 走时 + SHTC3 直读 driver 绑定）+
 *             HWV-A5 结构半（低电打盹阈值=15 常量钉行）+
 *             HWV-A3 结构半（双麦 ES7210 绑定守卫）
 *             -> test 2（RTC PCF85063 / SHTC3 / ES7210 绑定在 rlcd-4.2 板 +
 *                HP1_DOZE_BATTERY_THRESHOLD_PCT == 15 数值守卫 + doze_state 消费点）
 *   coverage: HWV-A6 结构半（换皮后 UI 沿用 art_assets 单一资产层 + SPIFFS 分区, 不重造）
 *             -> test 3（art_assets.c 设备构建接线 + assets 分区 canary; 均为
 *                must-stay-green 守卫, U 包已并）
 *   coverage: HWV-A2~A8 / 续航基准 / 证书握手 物理半 -> device-acceptance-checklist.md
 *             （本文件不承载; 见头注）
 *   coverage: boundary/security -> test 4（本文件接线序守卫 + 契约 canary +
 *             设备不直连 OpenClaw/18789 + 扫描器自检防假绿）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring: add "../../hardware_validation.acceptance.c" to
 *      HOST_ACCEPTANCE_SRCS in firmware/main/test/host_app/main/CMakeLists.txt,
 *      BEFORE "../../host_infra_smoke.acceptance.c"（smoke 保持列表末位 = 最先执行,
 *      包 0 boundary 齿注册序守卫——本文件 test 4 tooth 1 校验此序）。
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Freeze-phase red baseline used the standalone stub per
 *   firmware/main/test/acceptance/red-baseline/:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/hwv_red_runner \
 *         ../../hardware_validation.acceptance.c runner.c
 *     cd <repo root> && wsl -d Ubuntu-24.04 -- /tmp/hwv_red_runner
 * ---------------------------------------------------------------------------
 * Red-baseline 判读口径（2026-09-09 acceptance-author 实测）:
 *   本文件全部结构齿 RED on 真实缺失结构（F2 设备侧接线缺口 / rlcd-4.2 板无 RTC SHTC3
 *   driver 绑定 / 本文件未接线进 HOST_ACCEPTANCE_SRCS），守卫齿（ES7210/打盹阈值=15/
 *   art_assets 沿用/扫描器自检）GREEN（must-stay-green）。无一红因是编译/链接错误。
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

#define HWV_MAX_PATH 1024
#define HWV_MAX_READ (8u * 1024u * 1024u)

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), WSA/VRT/KSP/KSTORY
 * precedent. All scans run against the REAL working tree located from the runner
 * cwd. Directories named test / host_app / build / .git / assets are EXCLUDED
 * (test/ hosts THIS frozen file whose comments legitimately mention the frozen
 * needles; host_app/build are build artifacts; assets are binary art).
 * ------------------------------------------------------------------------ */

static char hwv_root[HWV_MAX_PATH];
static int hwv_root_ready;

static int hwv_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int hwv_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int hwv_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int hwv_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[HWV_MAX_PATH];

    if (hwv_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (hwv_is_regular(probe)) {
            snprintf(hwv_root, sizeof(hwv_root), "%s", cands[i]);
            hwv_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int hwv_rel(char *out, size_t cap, const char *rel)
{
    if (!hwv_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", hwv_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", hwv_root, rel);
    return 1;
}

static int hwv_exists(const char *rel)
{
    char p[HWV_MAX_PATH];
    return hwv_rel(p, sizeof(p), rel) && hwv_is_regular(p);
}

static char *hwv_read(const char *rel, size_t *out_len)
{
    char p[HWV_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!hwv_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > HWV_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int hwv_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int hwv_contains_ci(const char *hay, const char *needle)
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

static int hwv_file_contains(const char *rel, const char *needle)
{
    char *buf = hwv_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = hwv_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int hwv_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = hwv_read(rel, NULL);
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

/* Count files under rel_dir whose content matches needle (ci=1 case-insensitive). */
static int hwv_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, int ci,
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
        char abs_next[HWV_MAX_PATH];
        char rel_next[HWV_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            hwv_is_skipped_dir(e->d_name)) {
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
            count += hwv_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                 first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > HWV_MAX_READ) {
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
                int m = ci ? hwv_contains_ci(buf, needle) : hwv_contains(buf, needle);
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

static int hwv_scan_count(const char *rel_dir, const char *needle, int ci,
                          char *first_hit, size_t cap)
{
    char abs[HWV_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!hwv_rel(abs, sizeof(abs), rel_dir) || !hwv_is_dir(abs)) {
        return 0;
    }
    return hwv_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

#define HWV_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(hwv_locate_root(), \
    "HWV RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper: dynamic diagnostics are pre-formatted into a static buffer
 * then passed to the single-string assert macros (precedent KRM/KSTORY). */
static char hwv_fm_buf[1024];
static const char *hwv_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *hwv_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(hwv_fm_buf, sizeof(hwv_fm_buf), fmt, ap);
    va_end(ap);
    return hwv_fm_buf;
}

#define HWV_ASSERT_FILE_COUNT(n, min_n, what, red_hint)                       \
    TEST_ASSERT_TRUE_MESSAGE((n) >= (min_n),                                  \
        hwv_fm("HWV RED: %s referenced by %d file(s), expected >= %d — %s",   \
               (what), (n), (min_n), (red_hint)))

/* -------------------------------------------------------------------------- *
 * TEST 1 — F2 设备侧页内输入生产接线（structural + HWV-A9 建议）
 * F7 留根：story_page 四个冻结符号（kidpal_story_page_input / _on_saved /
 * _rec_tick / _stamp_spec）在 story_p3 落地时均为「定义但零生产调用者」。9b 扩界
 * 承接把「录制→归档→盖章」真接进设备生产路径：KEY/BOOT 页内手势到达 story_page_input、
 * 停录/保存成功 ack 到达 on_saved、录音态演出 tick 到达 rec_tick、盖章几何（rings/angle/
 * hold）被 stamp 渲染面真实消费。
 * 计数口径：story_page.h 冻结声明 + story_page.c 定义 = 稳定 2 文件；一个真实生产
 * 消费调用点（第三个文件, 按键路由/mcp 消费/显示循环）把计数抬到 3。故 input/on_saved/
 * rec_tick 三齿钉 >= 3 文件（声明 + 定义 + 至少一个生产消费点）——只计「引用文件数」
 * 会被 .h 声明虚增（假绿），必须钉出真实的跨文件消费调用点。
 * stamp_spec 的消费点在 story_page.c 内部（stamp 渲染面调冻结几何），钉「story_page.c
 * 内 定义 + 渲染消费 >= 2 处」。
 * RED today: input/on_saved/rec_tick 各 2 文件（.h 声明 + .c 定义, 零生产消费）;
 * stamp_spec 在 story_page.c 内仅 1 处（定义, 渲染面硬编码未消费）。
 * ------------------------------------------------------------------------ */
TEST_CASE("hardware validation: F2 设备侧页内输入生产接线——story 四函数落盘并被生产路径调用 (HWV-A9)",
          "[hardware_validation][HWV-A9][structural]")
{
    char hit[HWV_MAX_PATH];
    int n;

    HWV_REQUIRE_ROOT();

    /* tooth 1: kidpal_story_page_input —— 声明(.h) + 定义(.c) = 2; 设备按键路由层必须
     * 再有一处消费调用点（>= 3 文件: 声明 + 定义 + 按键分派）。 */
    n = hwv_scan_count("firmware/main", "kidpal_story_page_input(", 0, hit, sizeof(hit));
    HWV_ASSERT_FILE_COUNT(n, 3, "kidpal_story_page_input",
        "P3 故事页内输入特例(光标/进阅读/停录/取消/朗读/翻屏)没有接进设备按键路由层——"
        "spec §4.0「页内特例」按键手感在真机上不可达 (F7 殿后接线缺口, HWV-A9)");

    /* tooth 2: kidpal_story_on_saved —— 云端保存成功 ack -> 盖章, 必须有一处生产
     * 消费点（>= 3 文件: 声明 + 定义 + 保存成功消费）。 */
    n = hwv_scan_count("firmware/main", "kidpal_story_on_saved(", 0, hit, sizeof(hit));
    HWV_ASSERT_FILE_COUNT(n, 3, "kidpal_story_on_saved",
        "「上传成功那一瞬 -> 盖章帧」链路没有生产触发点 (S-F2.2/S-F2.4), 孩子录完故事"
        "永远看不到盖章帧 (F7 殿后接线缺口, HWV-A9)");

    /* tooth 3: kidpal_story_rec_tick —— 录音态演出帧必须被显示循环真实驱动
     * （>= 3 文件: 声明 + 定义 + REC tick 消费）。 */
    n = hwv_scan_count("firmware/main", "kidpal_story_rec_tick(", 0, hit, sizeof(hit));
    HWV_ASSERT_FILE_COUNT(n, 3, "kidpal_story_rec_tick",
        "REC 闪点/声波/大时长演出没有显示循环驱动——录音时「正在录」不可观察 "
        "(S-F2.2 p3-recording; F7 殿后接线缺口, HWV-A9)");

    /* tooth 4: kidpal_story_stamp_spec —— 冻结盖章几何(rings=2/内虚/-8°/800ms)必须被
     * stamp 渲染面真实消费（story_page.c 内 定义 + 渲染消费 >= 2 处; 现只定义零消费）。 */
    n = hwv_count_in_file("firmware/main/display/story_page.c", "kidpal_story_stamp_spec(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hwv_fm("HWV RED: kidpal_story_stamp_spec has %d occurrence(s) in story_page.c, "
               "expected >= 2 — 盖章几何被硬编码而非经冻结契约消费——story_p3 冻结的 "
               "rings/内虚/-8°/800ms 没有落进 stamp 渲染面（渲染与契约漂移, 双源风险; "
               "F7 殿后接线缺口, HWV-A9）", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — driver 绑定结构面（structural; HWV-A4 RTC/SHTC3 + HWV-A5 打盹=15 +
 * HWV-A3 双麦守卫）
 * rlcd-4.2 是 KidPal 板（waveshare-s3-rlcd-4.2）。RTC(PCF85063)/SHTC3 是包里
 * 「硬件强绑定子能力」的 driver 绑定落点——host 段从未测（home_p1 注释 P1-E2 殿后
 * 9b），9b 把物理直读真接进板级。麦(ES7210) 与打盹阈值=15 已在世（守卫）。
 * RED today: rlcd-4.2 板无 RTC 无 SHTC3 driver 绑定（config.h 只有 ES7210/ES8311）。
 * ------------------------------------------------------------------------ */
TEST_CASE("hardware validation: driver 绑定结构面——RTC/SHTC3/双麦/打盹阈值=15 (HWV-A3/A4/A5)",
          "[hardware_validation][HWV-A4][HWV-A5][structural]")
{
    const char *board_dir = "firmware/main/boards/waveshare-s3-rlcd-4.2";
    char hit[HWV_MAX_PATH];
    int n;

    HWV_REQUIRE_ROOT();

    /* tooth 1: RTC(PCF85063) driver 绑定在 rlcd-4.2 板（KidPal 板自身的板级绑定,
     * 不做树级兜底——树级会误中其它板子的同款 driver, 假绿）。 */
    n = hwv_scan_count(board_dir, "PCF85063", 0, hit, sizeof(hit));
    if (n == 0) {
        n = hwv_scan_count(board_dir, "pcf85063", 0, hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "HWV-A4 RED: rlcd-4.2 板无 PCF85063(RTC) driver 绑定——RTC 走时+断电续航真机直读"
        "没有落点 (spec §2 硬件盘点 PCF85063·I2C·独立备用电池; 包 5 留根 RTC 殿后项, "
        "9b 承接; 树级有无与 KidPal 板绑定无关, 不兜底)");

    /* tooth 2: SHTC3 温湿度 driver 绑定在 rlcd-4.2 板（同上, 板级绑定不做树级兜底）。 */
    n = hwv_scan_count(board_dir, "SHTC3", 0, hit, sizeof(hit));
    if (n == 0) {
        n = hwv_scan_count(board_dir, "shtc3", 0, hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "HWV-A4 RED: rlcd-4.2 板无 SHTC3 温湿度 driver 绑定——P1 温湿度真机直读/前光"
        "观感前置没有落点 (spec §2 硬件盘点 SHTC3; 包 5 留根 SHTC3 殿后项, 9b 承接)");

    /* tooth 3 (guard): 双麦 ES7210 板级绑定在世（config.h 已钉 AUDIO_CODEC_ES7210_ADDR）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        hwv_file_contains("firmware/main/boards/waveshare-s3-rlcd-4.2/config.h",
                          "AUDIO_CODEC_ES7210_ADDR"),
        "HWV-A3 RED(infra): rlcd-4.2 板丢失 ES7210 codec 地址绑定——双麦采音(ADC)通道"
        "底座被破坏 (spec §2 硬件盘点 ES7210 双麦阵列)");

    /* tooth 4 (guard): 低电打盹阈值 == 15 常量钉行（home_p1 已冻结, 9b owns 调定）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        hwv_file_contains("firmware/main/display/home_p1.h",
                          "HP1_DOZE_BATTERY_THRESHOLD_PCT 15"),
        "HWV-A5 RED(infra): 打盹电量阈值常量漂移——必须钉死为 15 (HP1_DOZE_BATTERY_"
        "THRESHOLD_PCT 15, <= 触发; 9b owns 调定, 真机低电打盹触发的判红判绿靠它)");

    /* tooth 5 (guard): doze_state 被首页渲染真实消费（阈值非死代码）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        hwv_count_in_file("firmware/main/display/home_p1.c", "kidpal_home_p1_doze_state(") >= 1,
        "HWV-A5 RED: 首页渲染面不再消费打盹判定——低电打盹阈值 15 成了死代码 "
        "(spec §4.0 低电打盹真机触发的前置结构)");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — 资产/SPIFFS 沿用守卫（structural; HWV-A6 换皮结构半）
 * U 包已落的单一资产层（art_assets）与 SPIFFS assets 分区必须保持不变——9b 换皮后
 * 的真机 UI 沿用同一资产层、不重造第二套。全部 must-stay-green。
 * ------------------------------------------------------------------------ */
TEST_CASE("hardware validation: 资产/SPIFFS 沿用守卫——art_assets 单一资产层不重造 (HWV-A6)",
          "[hardware_validation][HWV-A6][structural]")
{
    HWV_REQUIRE_ROOT();

    /* tooth 1: art_assets.c 仍编入设备构建（U 包的共享资产层单一源）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        hwv_file_contains("firmware/main/CMakeLists.txt", "art_assets.c"),
        "HWV-A6 RED(infra): art_assets.c 不再编入设备构建——换皮后 UI 的资产加载底座"
        "被移除 (U 包共享资产层单一源, 9b 真机 UI 沿用)");

    /* tooth 2: SPIFFS assets 分区 canary 在世（UAA-A9 钉死 0x800000/8M）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        hwv_file_contains("firmware/partitions/v2/16m.csv", "assets,") &&
        hwv_file_contains("firmware/partitions/v2/16m.csv", "spiffs"),
        "HWV-A6 RED(infra): assets 分区行从 partitions/v2/16m.csv 丢失——V3 美术资产"
        "真机 SPIFFS 落地面被拆 (U 包已并, 9b 真机观感依赖)");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — 接线序守卫 + 契约 canary + 设备不直连 OpenClaw/18789 + 扫描器自检。
 * ------------------------------------------------------------------------ */
TEST_CASE("hardware validation: 接线序 + 契约 canary + 直连守卫 + 扫描器自检 (boundary/security)",
          "[hardware_validation][boundary][security]")
{
    char hit[HWV_MAX_PATH];
    char *mk;
    int n;

    HWV_REQUIRE_ROOT();

    /* tooth 1: 本文件须接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke 之前
     * （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = hwv_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk, "HWV RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "hardware_validation.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "HWV RED: hardware_validation.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — "
            "host 验收构建未接本包 (BUILDER DUTY: 与 U/char_stroke 同款接线)");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "HWV RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失 — "
            "包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "HWV RED: hardware_validation.acceptance.c 必须排在 host_infra_smoke.acceptance.c "
            "之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2: 契约 canary（零契约触碰红线）。 */
    TEST_ASSERT_TRUE_MESSAGE(hwv_exists("docs/contracts/interface-contract.md"),
        "HWV RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3: 设备不直连 OpenClaw / 18789（WSA/VRT/KSP/KSTORY 守卫对偶）。 */
    n = hwv_scan_count("firmware/main", "openclaw", 1, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references openclaw directly — 设备只连自建网关");
    n = hwv_scan_count("firmware/main", "18789", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references port 18789 — OpenClaw 端口不得出现在设备侧");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(hwv_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(hwv_exists("firmware/main/display/__hwv_no_such__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(hwv_read("firmware/main/display/__hwv_no_such__.c", NULL),
        "boundary RED(infra): hwv_read returned data for a nonexistent file");
    n = hwv_scan_count("firmware/main/display", "__hwv_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = hwv_count_in_file("firmware/main/display/view_router.c", "VRT_ROUTE_STORY_LIST");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
}

/* --------------------------------------------------------------------------- *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-09  initial freeze (acceptance-author, TaskPackage 260829-hardware-validation).
 * --------------------------------------------------------------------------- */