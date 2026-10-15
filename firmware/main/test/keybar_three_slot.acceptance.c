/*
 * Frozen acceptance tests — TaskPackage 260915-m1-k2-keybar-three-slot (subsystem:
 * firmware; M1-K2; 底部键帽条三槽改造：左 KEY 提示 · 中页点 · 右 BOOT 提示)。
 *
 * Story: 无 story——共享底座键帽条。语义权威 = docs/requirements.md §2.2 键帽条行
 *（comp-keybar 三槽规格，v1.15 重写 + v1.21/v1.22/v1.23 线框行）+ §2.4 页面结构
 *（3 页循环 / P4 撤页）+ docs/ui/brand-spec.md 姿态规则 + 设计稿 v4 §03 comp-keybar。
 * 现状差距（plan.md 背景摘录）：键帽条条体常驻已实现，但只有单行居中文案随态联动
 *（lcd_display.cc g_kidpal_keycap_label，lv_label_set_text(look.keycap) 直写）；
 * 三槽结构、页点、覆盖层页点隐藏全部缺失。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan) + BEHAVIORAL (纯逻辑决策表) frozen
 * acceptance，key_layer_dispatch.acceptance.c（K1a，同目录先例）+ p1_home_battery.
 * acceptance.c（N1b，同目录先例）范式。键帽条渲染面住 lcd_display.cc（LVGL 设备侧，
 * host runner 不可运行）——结构齿树扫工作树钉三槽接线事实；机制值（页点显隐/总数/
 * 实心位、左右槽文案值）钉在纯逻辑单源（home = firmware/main/display/keybar.{h,c}，
 * home_p1.c / kidpal_voice_states.c 同款「纯 C、无 chip/LVGL 依赖、host runner 与
 * 设备两侧同源可编」纪律），host runner 行为级可验（weak 哨兵，强定义落地后覆盖）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证; 「home =」为冻结落点）:
 *
 *   [1] keybar 纯逻辑单源（home = firmware/main/display/keybar.{h,c}，纯 C、
 *       无 LVGL/chip 依赖，强符号落 keybar.c）:
 *
 *       #define KKB_UNIMPLEMENTED (-2)   -- weak 哨兵值，永不是合法返回值
 *
 *       typedef struct {
 *           int visible;  -- 1 = 页点显示（仅主页面基层）; 0 = 覆盖层隐藏
 *           int total;    -- 页点总数 = 现行页循环页数（同源 key_gesture_parser.h
 *                            PAGE_COUNT，阶段一 = 4——deviation 对账锚 KB-E1；
 *                            2026-09-18 随 260917-m2-k2-retract-p4-three-page 撤 P4，
 *                            deviation 收回、随枚举 = 3，本模块永不写死数字面）
 *           int active;   -- 当前实心页点下标（0-based，= 当前页）
 *       } kidpal_keybar_dots_t;
 *
 *       int kidpal_keybar_dots(int page, int overlay_top, kidpal_keybar_dots_t *out);
 *         -- 页点槽裁决（KB-A2/KB-A4 机制单源）:
 *         overlay_top != 0（覆盖层顶层，取值来自 kidpal_render_node_front_is_overlay()
 *           ——与键路由同源，见 [3]）-> visible = 0（页点隐藏; total/active 仍报当前
 *           机制值，隐藏不抹状态）;
 *         overlay_top == 0（主页面基层）-> visible = 1, total = PAGE_COUNT, active = page
 *           （当前页实心; 「实心放大」视觉形态归设计稿 v4 §03 + 真机轮 KB-A5, host 齿
 *           钉机制值）;
 *         防御: out == NULL 或 page 越界（<0 或 >= PAGE_COUNT）-> -1。
 *
 *       int kidpal_keybar_slots(int page, int scene, int overlay_top,
 *                               const char **out_left, const char **out_right);
 *         -- 左右槽文案裁决（KB-A3 机制单源）。冻结映射（规格权威 = requirements
 *            §2.2 线框行逐字: WF-F1-01 P1 行「[KEY 说话] [BOOT 翻页]」/ WF-F1-02 P2
 *            行「[KEY 问课程] [BOOT 翻页]」/ P3 列表态线框「[KEY 说话·双击看]
 *            [BOOT 下移]」/ WF-B-01~03 语音轮「[BOOT 结束]」+ 版式通则「右槽 BOOT
 *            结束按 §2.1 结束方式③写定」/ WF-B-04 提醒「[KEY 知道了][BOOT 关闭]」）:
 *         overlay_top == 0（主页面基层, page 槽表）:
 *           P1 -> 左「说话」   右「翻页」
 *           P2 -> 左「问课程」 右「翻页」
 *           P3 -> 左「说话·双击看」右「下移」
 *           P4 -> 左右 = 撤页过渡兜底（v1.36 撤 P4、页循环仍 4——KB-E1 deviation;
 *                 页槽值 spec 未定, 本齿只钉 rc=0 且左右非 NULL, 值钉定须 human 裁定
 *                 后 tighten-only 修订——ADV 已登记 planner）;
 *                 2026-09-18 随 260917-m2-k2-retract-p4-three-page 该兜底 case 消解
 *                 （第 4 页位整值撤出 -> 该索引落入越界防御列 -1; 原「存在性」齿已
 *                 同步翻正，见本文件 REVISION LOG，ADV 随 deviation 收回一并消解）;
 *           scene 入参在基层列被忽略（任意 int 均合法——主页面无语音场景语境）。
 *         overlay_top != 0（覆盖层随态, scene 槽表）:
 *           左槽 = kidpal_voice_state_look(scene).keycap（冻结闭集单源——内部消费
 *             [2] 的 look, 零复制零偏移, KB-E2; 禁止在本模块重抄闭集字面）;
 *           右槽 = LISTEN/THINK/TALK/LOWBATT -> 「结束」（语音轮/低电, §2.1 结束
 *             方式③）; REMIND -> 「关闭」（提醒弹窗, WF-B-04 v1.39 隔离口径）;
 *           scene 越界（< KVST_SCENE_LISTEN 或 > KVST_SCENE_LOWBATT）-> -1。
 *         防御: out_left/out_right 任一 NULL 或 page 越界 -> -1。
 *
 *   [2] 闭集单源零偏移（KB-A3/KB-E2; home = firmware/main/display/kidpal_voice_states.c,
 *       已在世实现, 本包零触碰——只接线 + 行为级复核）:
 *       kidpal_voice_states 冻结闭集五成员（说完啦/不等了/打断/知道了/叫醒它）行为级
 *       逐字复核（强符号接进 host runner 后覆盖本文件 weak 哨兵）; 结构守卫: 闭集
 *       kCap 静态成员恰 5 个、文件内零 BOOT 提示字面（「结束」/「关闭」不混入闭集
 *       ——不增成员）。
 *
 *   [3] 页点显隐 layer_top 同源（KB-A4; home = firmware/main/display/lcd_display.cc
 *       设备侧接线 + keybar.c 纯逻辑只读入参）:
 *       lcd_display.cc 页点显隐消费 kidpal_render_node_front_is_overlay()（与板级
 *       dispatch 键分流同一事实源, K1a 建立的分层模型; 不另立第二真相源）, 换算
 *       overlay_top 后调 kidpal_keybar_dots / kidpal_keybar_slots（机制值全部出自
 *       [1] 纯逻辑单源, 设备侧零重算）。
 *
 *   [4] 三槽结构（KB-A1; home = firmware/main/display/lcd_display.cc SetupUI
 *       WeChat 分支 bottom_bar_ 段）:
 *       三个冻结符号（builder 按名落地, 文件级 static）:
 *         g_kidpal_keybar_left_label  -- 左 KEY 提示槽（lv_label_create(bottom_bar_)）
 *         g_kidpal_keybar_dots_box    -- 中页点槽容器（lv_obj_create(bottom_bar_),
 *                                          页点子件建其上）
 *         g_kidpal_keybar_right_label -- 右 BOOT 提示槽（lv_label_create(bottom_bar_)）
 *       通栏贴底 = 三槽全部建在 bottom_bar_（既有常驻 chrome 载体, 通栏贴底对齐在世
 *       ——LV_HOR_RES 宽 + LV_ALIGN_BOTTOM_MID, 本包零触碰其几何）。
 *       旧形态替换 = g_kidpal_keycap_label（旧单行居中文案 label）全文件零残留。
 *       左右槽随态写点: kidpal_voice_apply_look 函数区内左槽消费 look.keycap
 *       （[2] 单源）、右槽经 [1] slots 取值——键帽条文案不再有第二条来源。
 *
 *   [5] Builder FIRST ACTION: 把本验收文件与被测实现接进
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS:
 *         "../../keybar_three_slot.acceptance.c"
 *         "../../../display/keybar.c"         （[1] 强符号源）
 *         "../../../display/kidpal_voice_states.c"（[2] 强符号源, 在世实现首次入 runner）
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, boundary 齿注册序守卫; 本文件 test 5 tooth 1 校验此序）。
 *   [6] mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *       子串**（KR-A3 _TOGGLE_X 教训——strstr 子串仍命中则变异不红; 用 _STUB 类整词
 *       替换），mutationEvidence.perAcceptance 逐条回填，acceptanceId 与 taskpackage
 *       acceptance[] 正文逐字节一致。
 *
 *   [7] 查字窗键帽槽值域（ACC-5; home = firmware/main/display/keybar.{h,c};
 *       260928-char-window-key-triggered-play additive 追加; 需求权威 = requirements
 *       v1.59 WF-F3-01 键帽随态变注 :480——未起播「播报」、起播后「重播」, 右槽 BOOT
 *       「结束」; plan.md reviewed 方案 B——keybar.c 新并行函数, 既有
 *       kidpal_keybar_slots 签名与值域零触碰, additive 既有齿不破）:
 *       int kidpal_keybar_char_slots(int started, const char **out_left,
 *                                    const char **out_right);
 *         -- started == 0（演示未起播）-> 左「播报」右「结束」;
 *            started != 0（演示已起播）-> 左「重播」右「结束」;
 *            防御: out_left/out_right 任一 NULL -> -1; 成功返回 0。
 *         keybar.h 同步声明（lcd_display.cc sync 查字窗分支经 include 消费——ACC-6,
 *         结构齿钉声明在世）。强符号源 keybar.c 已在 runner 在册（既有接线）,
 *         builder 落地新函数后覆盖本文件 weak 哨兵, 零新增接线。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: KB-A1 三槽结构（通栏贴底三槽 + 常驻 chrome + 旧单行形态替换）-> test 4
 *             （结构齿: 三槽冻结符号 + bottom_bar_ 载体 + 旧 label 零残留 + 设备侧
 *             消费 [1] 单源）+ test 1/2（行为半: 三槽机制值出自纯逻辑单源）
 *   coverage: KB-A2 页点机制 -> test 1（决策表: 基层 visible/total/active + 覆盖层
 *             隐藏 + 数量 = PAGE_COUNT 同源判别（2026-09-18 随 M2-K2 撤 P4：原
 *             PAGE_COUNT(4) deviation 对账锚收回为同源判别）+ 防御）+ test 4
 *             （结构半: 设备侧消费 kidpal_keybar_dots）
 *   coverage: KB-A3 左右槽随态文案 -> test 2（决策表: 页槽表 + 随态表 + 闭集左槽
 *             与 look.keycap 逐字相等 + BOOT 右槽冻结值）+ test 3（闭集零偏移行为
 *             复核 + 不增成员结构守卫）+ test 4（结构半: apply_look 消费 look.keycap
 *             单源、keybar 模块零闭集字面重抄）
 *   coverage: KB-A4 页点显隐读 layer_top（与键路由同源）-> test 4（结构齿:
 *             lcd_display.cc 消费 kidpal_render_node_front_is_overlay + keybar 模块
 *             零 layer 进出裁决权）+ test 1（行为半: overlay_top 入参即 layer_top
 *             归一化值的裁决表）
 *   coverage: KB-A5 真机确认轮 -> 不在本文件（kind:structural 对象条目, human 持板;
 *             三槽条 + 页点显隐 + 随态文案真机可见; 证据落
 *             delivery/taskpackages/260915-m1-k2-keybar-three-slot/）——见文件尾
 *             「KB-A5 真机证据要求」段
 *   coverage: KB-A6 host runner 终跑 -> test 5（注册接线结构守卫）+ 本文件尾
 *             「KB-A6 终跑判读」段（suite 级判读: 相对有效基线零新增红 + 本包新齿全绿）
 *   coverage: KB-E1 范围纪律（页点 4→3 收口归 M2 + spec 零修订）-> test 1 tooth 1
 *             （deviation 对账锚：2026-09-18 随 260917-m2-k2-retract-p4-three-page
 *             撤 P4 收回 —— 原字面 PAGE_COUNT == 4 已改为本齿自注早已承诺的同源判别
 *             「total 随枚举单源走」，字面 3 的唯一钉点移住
 *             firmware/main/test/retract_p4_three_page.acceptance.c test 3）
 *             + test 5 tooth 3（requirements/契约 canary）
 *   coverage: KB-E2 闭集零偏移 -> test 2（左槽值域 = look.keycap 单源）+ test 3
 *             （五成员逐字复核 + kCap 恰 5 + 零 BOOT 提示字面混入）
 *   coverage: KB-E3 零裁决权 -> test 4 tooth 4/5（keybar 模块零 kidpal_layer_push/
 *             kidpal_layer_pop/kidpal_layer_stack_t——只读显隐、零覆盖层进出裁决）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [5]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *      （git-bash 下 MSYS_NO_PATHCONV=1 必加; idf.py 无 -T 选项）
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
#include <assert.h>
#include <stdarg.h>

/* 键路由共享底座头（页枚举 PAGE_COUNT 单源; K1a 先例同款 include）。 */
#include "../key_gesture_parser.h"
/* 语音轮五态闭集契约头（[2] 冻结形状: 场景码/look 结构/keycap 值域）。 */
#include "../display/kidpal_voice_states.h"

#define KKB_MAX_PATH 1024
#define KKB_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根）。 */
#define KKB_LCD_CC       "firmware/main/display/lcd_display.cc"
#define KKB_KEYBAR_C     "firmware/main/display/keybar.c"
#define KKB_KEYBAR_H     "firmware/main/display/keybar.h"
#define KKB_VOICE_C      "firmware/main/display/kidpal_voice_states.c"
#define KKB_CMAKELISTS   "firmware/main/test/host_app/main/CMakeLists.txt"
#define KKB_REQUIREMENTS "docs/requirements.md"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * kidpal_keybar_dots / kidpal_keybar_slots 强定义 home = display/keybar.c（[1],
 * builder 落地 + 接线后覆盖）; kidpal_voice_state_look 强定义 home =
 * display/kidpal_voice_states.c（[2], 在世实现, builder 接线后覆盖）。
 * 落地/接线前哨兵返回 KKB_UNIMPLEMENTED：行为齿先断「非哨兵」，红基线读作
 * 「frozen contract not implemented/wired」——真实缺失行为红，绝非链接错误。
 * ------------------------------------------------------------------------ */
#define KKB_UNIMPLEMENTED (-2)

typedef struct {
    int visible; /* 1 = 页点显示（仅主页面基层）; 0 = 覆盖层隐藏 */
    int total;   /* 页点总数 = 现行页循环页数（同源 PAGE_COUNT） */
    int active;  /* 当前实心页点下标（0-based） */
} kidpal_keybar_dots_t;

__attribute__((weak)) int kidpal_keybar_dots(int page, int overlay_top,
                                             kidpal_keybar_dots_t *out)
{
    (void)page; (void)overlay_top; (void)out;
    return KKB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_keybar_slots(int page, int scene, int overlay_top,
                                              const char **out_left,
                                              const char **out_right)
{
    (void)page; (void)scene; (void)overlay_top; (void)out_left; (void)out_right;
    return KKB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_state_look(int scene, kidpal_voice_look_t *out)
{
    (void)scene; (void)out;
    return KKB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_keybar_char_slots(int started, const char **out_left,
                                                   const char **out_right)
{
    (void)started; (void)out_left; (void)out_right;
    return KKB_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，key_layer_dispatch 先例）。全部扫描跑
 * 在真实工作树上，root 以手持 cwd 定位（从仓根或 firmware/main/test/host_app 两处
 * 都可跑）。test / host_app / build / .git / assets 目录排除——test 目录里住着本
 * 冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char kkb_root[KKB_MAX_PATH];
static int kkb_root_ready;

static int kkb_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kkb_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kkb_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kkb_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KKB_MAX_PATH];

    if (kkb_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kkb_is_regular(probe)) {
            snprintf(kkb_root, sizeof(kkb_root), "%s", cands[i]);
            kkb_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kkb_rel(char *out, size_t cap, const char *rel)
{
    if (!kkb_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kkb_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kkb_root, rel);
    return 1;
}

static int kkb_exists(const char *rel)
{
    char p[KKB_MAX_PATH];
    return kkb_rel(p, sizeof(p), rel) && kkb_is_regular(p);
}

static char *kkb_read(const char *rel, size_t *out_len)
{
    char p[KKB_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kkb_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KKB_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int kkb_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kkb_file_contains(const char *rel, const char *needle)
{
    char *buf = kkb_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kkb_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int kkb_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kkb_read(rel, NULL);
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

/* Region slice: from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it (end_needle == NULL -> slice to EOF). Caller
 * frees. NULL = 切取失败（锚漂移/缺失）。 */
static char *kkb_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = kkb_read(rel, NULL);
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
    if (end_needle == NULL ||
        (e = strstr(s + strlen(start_needle), end_needle)) == NULL) {
        e = buf + strlen(buf); /* 尾锚 NULL 或 start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int kkb_count_in_slice(const char *slice, const char *needle)
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

#define KKB_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kkb_locate_root(), \
    "KKB RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented/wired yet. */
static void kkb_require_implemented(int value, const char *what)
{
    if (value == KKB_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KKB RED: frozen seam NOT implemented/wired — %s "
                 "(builder must land display/keybar.{h,c} per the seam contract in "
                 "firmware/main/test/keybar_three_slot.acceptance.c and wire keybar.c + "
                 "kidpal_voice_states.c + this file into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/HPB 先例）。 */
static char kkb_fm_buf[1024];
static const char *kkb_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kkb_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kkb_fm_buf, sizeof(kkb_fm_buf), fmt, ap);
    va_end(ap);
    return kkb_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — KB-A2/KB-A4 页点槽裁决决策表（behavioral, frozen seam [1] dots 半）
 * 主页面基层: visible=1 / total=PAGE_COUNT(4, KB-E1 deviation 对账锚) / active=当前页;
 * 覆盖层顶层: visible=0（隐藏不抹状态）; 越界防御。RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: KB-A2 -- 页点机制决策表：基层 visible=1/total=现行页循环/active=当前页实心 + 覆盖层 visible=0 + 数量 deviation 对账锚 KB-E1 + 防御 */
/* coverage: KB-A4 -- 页点显隐行为半：overlay_top（layer_top 归一化值）入参裁决，与键路由同源的归一化语义（0=基层/非0=覆盖层顶层） */
TEST_CASE("键帽条三槽: 页点槽裁决——基层显隐+当前页实心+覆盖层隐藏 (KB-A2/KB-A4)",
          "[keybar_three_slot][KB-A2][KB-A4][behavior]")
{
    kidpal_keybar_dots_t d;
    int page, rc;

    /* tooth 1 (KB-E1 deviation 对账锚 —— 2026-09-18 随 260917-m2-k2-retract-p4-three-
     * page 撤 P4 收回，留痕修订): 页点数量 = 现行页循环页数，同源 key_gesture_parser.h
     * PAGE_COUNT。spec §2.2 冻结口径本就是 3 点（v1.36 页循环 4→3）——阶段一按现行 4
     * 页循环渲染 4 点的 deviation 由 human 2026-09-16 拍板认可（workstream
     * launchPreconditions 第 3 条），4→3 对账归 M2 撤 P4 包、现已落地。本齿按原自注
     * 承诺的形态改写：原写法 `PAGE_COUNT == 4` 把 deviation 的字面值钉成了断言，与
     * 「M2 后随枚举自动 4→3、不需改本文件」自相矛盾（撤页必假红）；现钉**同源判别**
     * ——dots 报出的 total 必须等于这枚枚举的当前值，且 keybar.c 的取源确实是它
     * （不被硬编码顶掉）。字面 3 的单点钉死移住 retract_p4_three_page.acceptance.c
     * test 3（两处并存会互相遮蔽），本文件不再对 PAGE_COUNT 取任何字面值。 */
    rc = kidpal_keybar_dots(0, 0, &d);
    kkb_require_implemented(rc, "kidpal_keybar_dots unimplemented/wired");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && d.total == (int)PAGE_COUNT &&
                                 kkb_count_in_file(KKB_KEYBAR_C, "(int)PAGE_COUNT") >= 2,
        kkb_fm("KKB-E1 RED: 页点总数与 PAGE_COUNT(%d) 脱钩（dots(0,base) rc=%d 报 %d, "
               "keybar.c 内 (int)PAGE_COUNT 取源 %d 处）——页点数量不再随页枚举单源走"
               "（KB-E1 deviation 已随 M2-K2 撤 P4 收回: 数量 = 现行页循环页数）",
               (int)PAGE_COUNT, rc, d.total,
               kkb_count_in_file(KKB_KEYBAR_C, "(int)PAGE_COUNT")));

    /* ---- 主页面基层列（overlay_top == 0）----------------------------------- */
    for (page = 0; page < (int)PAGE_COUNT; page++) {
        memset(&d, 0, sizeof(d));
        rc = kidpal_keybar_dots(page, 0, &d);
        kkb_require_implemented(rc, "kidpal_keybar_dots unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            kkb_fm("KKB-A2 RED: dots(page=%d, base) rc=%d 期望 0——合法主页面入参被拒",
                   page, rc));
        TEST_ASSERT_TRUE_MESSAGE(d.visible == 1,
            kkb_fm("KKB-A2 RED: 主页面基层 page=%d 页点 visible=%d 期望 1——页点仅主页面"
                   "显示的基层半被破坏 (§2.2 comp-keybar)", page, d.visible));
        TEST_ASSERT_TRUE_MESSAGE(d.total == (int)PAGE_COUNT,
            kkb_fm("KKB-A2 RED: page=%d 页点总数 %d != PAGE_COUNT(%d)——页点数量与现行"
                   "页循环不同源 (KB-E1 deviation 口径: 数量 = 现行页循环页数)",
                   page, d.total, (int)PAGE_COUNT));
        TEST_ASSERT_TRUE_MESSAGE(d.active == page,
            kkb_fm("KKB-A2 RED: page=%d 实心页点下标 %d != %d——当前页实心机制漂移 "
                   "(当前页实心放大, §2.2)", page, d.active, page));
    }

    /* ---- 覆盖层列（overlay_top != 0）: 页点隐藏（提醒弹窗/查字窗/语音轮演出,
     * §2.2「覆盖层不显示页点」; P3 删除确认弹窗同覆盖层口径 WF-F2-05）---------- */
    {
        static const int overlay_vals[] = { 1, 7 }; /* 归一化语义: 非 0 即覆盖层顶层 */
        int ov;
        for (ov = 0; ov < (int)(sizeof(overlay_vals) / sizeof(overlay_vals[0])); ov++) {
            memset(&d, 0, sizeof(d));
            rc = kidpal_keybar_dots(2, overlay_vals[ov], &d);
            kkb_require_implemented(rc, "kidpal_keybar_dots unimplemented");
            TEST_ASSERT_TRUE_MESSAGE(rc == 0,
                kkb_fm("KKB-A2 RED: dots(page=2, overlay=%d) rc=%d 期望 0",
                       overlay_vals[ov], rc));
            TEST_ASSERT_TRUE_MESSAGE(d.visible == 0,
                kkb_fm("KKB-A2 RED: 覆盖层顶层(overlay=%d) 页点 visible=%d 期望 0——"
                       "覆盖层不显示页点被破坏 (§2.2 comp-keybar; 提醒弹窗/查字窗/"
                       "语音轮演出/删除确认全覆盖)", overlay_vals[ov], d.visible));
            TEST_ASSERT_TRUE_MESSAGE(d.total == (int)PAGE_COUNT && d.active == 2,
                kkb_fm("KKB-A2 RED: 覆盖层隐藏时 total/active 被抹（%d/%d 期望 %d/2）"
                       "——隐藏不抹机制状态, 关层回原页后页点须立即复位显示",
                       d.total, d.active, (int)PAGE_COUNT));
        }
    }

    /* ---- 防御列 ------------------------------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_dots(0, 0, NULL) == -1,
        "KKB-A2 RED: NULL out 未防御（-1）——页点裁决把空指针当合法");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_dots(-1, 0, &d) == -1,
        "KKB-A2 RED: page=-1 未防御（-1）——越界页被静默接受");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_dots((int)PAGE_COUNT, 0, &d) == -1,
        "KKB-A2 RED: page=PAGE_COUNT 未防御（-1）——越界页被静默接受");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — KB-A3 左右槽随态文案决策表（behavioral, frozen seam [1] slots 半）
 * 主页面基层页槽表（P1 说话/翻页 · P2 问课程/翻页 · P3 说话·双击看/下移 · P4 撤页
 * 过渡兜底只钉存在性）+ 覆盖层随态表（左槽 = look.keycap 闭集单源逐字相等 + 右槽
 * BOOT 提示冻结值 结束/关闭）+ 防御。RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: KB-A3 -- 左右槽随态文案决策表：基层页槽表逐字（线框行权威）+ 覆盖层随态表（左槽 = 闭集 keycap 单源 + 右槽 结束/关闭）+ 防御 */
TEST_CASE("键帽条三槽: 左右槽随态文案——页槽表+闭集单源+BOOT提示 (KB-A3)",
          "[keybar_three_slot][KB-A3][behavior]")
{
    const char *left = NULL, *right = NULL;
    kidpal_voice_look_t look;
    int scene, rc;

    /* ---- 主页面基层列（overlay_top == 0, 页槽表; 规格权威 = requirements §2.2
     * 线框行逐字）------------------------------------------------------------ */
    rc = kidpal_keybar_slots(0, 0, 0, &left, &right);
    kkb_require_implemented(rc, "kidpal_keybar_slots unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL &&
                             strcmp(left, "说话") == 0 && strcmp(right, "翻页") == 0,
        kkb_fm("KKB-A3 RED: P1 页槽 (左=%s, 右=%s) 期望 (说话, 翻页)——WF-F1-01 全局"
               "键帽条行漂移 (v1.21: 操作提示归 comp-keybar, 页内不重复画)",
               left ? left : "(null)", right ? right : "(null)"));

    rc = kidpal_keybar_slots(1, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL &&
                             strcmp(left, "问课程") == 0 && strcmp(right, "翻页") == 0,
        kkb_fm("KKB-A3 RED: P2 页槽 (左=%s, 右=%s) 期望 (问课程, 翻页)——WF-F1-02 "
               "键帽条行漂移 (v1.20)", left ? left : "(null)", right ? right : "(null)"));

    rc = kidpal_keybar_slots(2, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL &&
                             strcmp(left, "说话·双击看") == 0 &&
                             strcmp(right, "下移") == 0,
        kkb_fm("KKB-A3 RED: P3 页槽 (左=%s, 右=%s) 期望 (说话·双击看, 下移)——P3 列表"
               "态线框漂移 (BOOT 短按=光标下移, §2.4 P3 行)",
               left ? left : "(null)", right ? right : "(null)"));

    /* 第 4 页槽位齿（KB-E1 deviation 面 —— 2026-09-18 随 260917-m2-k2-retract-p4-
     * three-page 撤 P4 落地做留痕修订）: 原齿钉「index 3 有兜底槽位」（v1.36 撤 P4、
     * 页循环仍 4 的过渡形态，只钉存在性，值钉定归 human——ADV 已登记 planner）。撤页
     * 后 keybar.c 的 `case PAGE_P4:` 兜底分支随枚举整值消解，原齿的字面 index 3 恰好
     * 等于新的 PAGE_COUNT -> 会变成「要求已撤页位仍存在」的假红。现按同源形态翻正为
     * **越界防御齿**: page = PAGE_COUNT（撤页前后皆为不存在的第 4 页索引）一律 -1;
     * human 待裁定的 P4 槽值 ADV 随 deviation 收回一并消解（不再有该页位）。 */
    left = NULL;
    right = NULL;
    rc = kidpal_keybar_slots((int)PAGE_COUNT, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == -1,
        kkb_fm("KKB-A3 RED: slots(page=PAGE_COUNT=%d) rc=%d left=%s right=%s 期望 -1——"
               "不存在的页索引被当成合法页给槽文案（撤页后页槽表只三行, 越界页交防御列）",
               (int)PAGE_COUNT, rc, left ? left : "(null)", right ? right : "(null)"));

    /* 基层列 scene 入参被忽略（主页面无语音场景语境, 任意 int 合法——[1] 契约）。 */
    rc = kidpal_keybar_slots(0, 99, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && strcmp(left, "说话") == 0,
        "KKB-A3 RED: 基层列 scene 入参未被忽略——主页面键帽被无效场景码劫持");

    /* ---- 覆盖层列（overlay_top != 0, scene 槽表）: 左槽 = 闭集单源（与
     * kidpal_voice_state_look 的 keycap 逐字相等——值域只在闭集内引用, KB-E2）+
     * 右槽 BOOT 提示冻结值（语音轮「结束」= §2.1 结束方式③ / 提醒「关闭」=
     * WF-B-04 v1.39 隔离口径）------------------------------------------------ */
    for (scene = KVST_SCENE_LISTEN; scene <= KVST_SCENE_LOWBATT; scene++) {
        rc = kidpal_keybar_slots(0, scene, 1, &left, &right);
        kkb_require_implemented(rc, "kidpal_keybar_slots unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            kkb_fm("KKB-A3 RED: slots(scene=%d, overlay) rc=%d 期望 0", scene, rc));
        rc = kidpal_voice_state_look(scene, &look);
        kkb_require_implemented(rc, "kidpal_voice_state_look unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(look.keycap != NULL && left != NULL &&
                                 strcmp(left, look.keycap) == 0,
            kkb_fm("KKB-A3 RED: 覆盖层 scene=%d 左槽 (%s) != 闭集 keycap (%s)——左槽"
                   "值域脱离 kidpal_voice_states 冻结闭集单源 (KB-E2 零偏移红线)",
                   scene, left ? left : "(null)", look.keycap ? look.keycap : "(null)"));
        TEST_ASSERT_TRUE_MESSAGE(right != NULL &&
                                 strcmp(right, (scene == KVST_SCENE_REMIND) ? "关闭"
                                                                             : "结束") == 0,
            kkb_fm("KKB-A3 RED: 覆盖层 scene=%d 右槽 (%s) 期望 (%s)——BOOT 提示漂移"
                   "（语音轮=结束/提醒=关闭, WF-B-01~04 + §2.1 结束方式③）",
                   scene, right ? right : "(null)",
                   (scene == KVST_SCENE_REMIND) ? "关闭" : "结束"));
    }

    /* ---- 防御列 ------------------------------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_slots(0, 0, 0, NULL, &right) == -1,
        "KKB-A3 RED: NULL out_left 未防御（-1）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_slots(0, 0, 0, &left, NULL) == -1,
        "KKB-A3 RED: NULL out_right 未防御（-1）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_slots(-1, 0, 0, &left, &right) == -1,
        "KKB-A3 RED: page=-1 未防御（-1）——越界页被静默接受");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_slots(0, 99, 1, &left, &right) == -1,
        "KKB-A3 RED: 覆盖层列 scene=99 未防御（-1）——非法场景码被静默接受");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — KB-A3/KB-E2 kidpal_voice_states 冻结闭集零偏移（behavioral + 结构守卫）
 * 行为半: 五场景 look 逐字复核（keycap 五成员 + LISTEN 气泡 + 姿态码 + 防御——强
 * 符号接进 host runner 后覆盖 weak 哨兵, 在世实现首次入 runner）。结构半: 闭集
 * kCap 静态成员恰 5 个 + 文件内零 BOOT 提示字面（不增成员——「结束」/「关闭」属
 * keybar 右槽值域, 混入闭集 = 增成员, KB-E2 红线）。
 * RED today: kidpal_voice_states.c 未接进 host runner（weak 哨兵 -2）。
 * ------------------------------------------------------------------------ */
/* coverage: KB-A3 -- 闭集行为级复核：五场景 keycap/bubble/pose 逐字（冻结映射零偏移） */
/* coverage: KB-E2 -- 闭集零偏移守卫：kCap 成员恰 5（不删）+ 零 BOOT 提示字面混入（不增）+ kidpal_voice_states.c 在世 */
TEST_CASE("键帽条三槽: 语音闭集零偏移——五场景逐字+成员数守卫 (KB-A3/KB-E2)",
          "[keybar_three_slot][KB-A3][KB-E2][closed-set]")
{
    kidpal_voice_look_t look;
    int rc;

    /* ---- 行为半: 五场景逐字（键帽短语 = V3 §06 逐字, 五态键帽行不得偏移）----- */
    rc = kidpal_voice_state_look(KVST_SCENE_LISTEN, &look);
    kkb_require_implemented(rc, "kidpal_voice_state_look unimplemented (wire "
                                "display/kidpal_voice_states.c into HOST_ACCEPTANCE_SRCS)");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && look.keycap != NULL &&
                             strcmp(look.keycap, "说完啦") == 0 &&
                             look.bubble != NULL && strcmp(look.bubble, "我在听！") == 0 &&
                             look.pose == KVST_POSE_LISTEN,
        "KKB-A3 RED: LISTEN look 漂移——keycap 须为「说完啦」/ bubble「我在听！」/"
        "pose LISTEN（冻结映射闭集零偏移, KB-E2）");

    rc = kidpal_voice_state_look(KVST_SCENE_THINK, &look);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && look.keycap != NULL &&
                             strcmp(look.keycap, "不等了") == 0 &&
                             look.pose == KVST_POSE_THINK,
        "KKB-A3 RED: THINK look 漂移——keycap「不等了」/ pose THINK（闭集零偏移）");

    rc = kidpal_voice_state_look(KVST_SCENE_TALK, &look);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && look.keycap != NULL &&
                             strcmp(look.keycap, "打断") == 0 &&
                             look.pose == KVST_POSE_TALK,
        "KKB-A3 RED: TALK look 漂移——keycap「打断」/ pose TALK（闭集零偏移）");

    rc = kidpal_voice_state_look(KVST_SCENE_REMIND, &look);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && look.keycap != NULL &&
                             strcmp(look.keycap, "知道了") == 0 &&
                             look.pose == KVST_POSE_JUMP,
        "KKB-A3 RED: REMIND look 漂移——keycap「知道了」/ pose JUMP（闭集零偏移）");

    rc = kidpal_voice_state_look(KVST_SCENE_LOWBATT, &look);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && look.keycap != NULL &&
                             strcmp(look.keycap, "叫醒它") == 0 &&
                             look.pose == KVST_POSE_SLEEP,
        "KKB-A3 RED: LOWBATT look 漂移——keycap「叫醒它」/ pose SLEEP（闭集零偏移）");

    /* 防御语义（在世契约, v3_restore FROZEN OFFLINE CONTRACT 沿用复核）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_LISTEN, NULL) == -1 &&
                             kidpal_voice_state_look(-1, &look) == -1 &&
                             kidpal_voice_state_look(KVST_SCENE_LOWBATT + 1, &look) == -1,
        "KKB-A3 RED: kidpal_voice_state_look 防御语义漂移——NULL out / 非法场景须 -1");

    /* ---- 结构半: 闭集成员数守卫（KB-E2 不增删改）---------------------------- */
    KKB_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(kkb_exists(KKB_VOICE_C),
        "KKB-E2 RED(infra): kidpal_voice_states.c 丢失——冻结闭集实现面被触碰");

    /* kCap 静态成员恰 5（删成员/并成员即漂移——五态键帽行一一对应）。 */
    {
        int n = kkb_count_in_file(KKB_VOICE_C, "static const char *const kCap");
        TEST_ASSERT_TRUE_MESSAGE(n == 5,
            kkb_fm("KKB-E2 RED: kidpal_voice_states.c 闭集 kCap 成员 %d 个, 期望恰 5"
                   "（说完啦/不等了/打断/知道了/叫醒它）——闭集成员被增删 (KB-E2 "
                   "零偏移红线; 新增态文案归承载态的包按冻结映射走)", n));
    }

    /* 零 BOOT 提示字面混入（「结束」/「关闭」属 keybar 右槽值域, 不属闭集——
     * 混入 = 增成员, keybar 右槽与闭集的值域边界被打破）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_VOICE_C, "结束") == 0 &&
                             kkb_count_in_file(KKB_VOICE_C, "关闭") == 0,
        "KKB-E2 RED: kidpal_voice_states.c 出现「结束」/「关闭」——BOOT 提示字面混入"
        "冻结闭集（不增成员红线; 右槽值域住 keybar 模块, 见本文件 seam [1]）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — KB-A1/KB-A4/KB-E3 三槽结构 + layer_top 同源 + 零裁决权（structural）
 * 树扫 lcd_display.cc: 三槽冻结符号落位（bottom_bar_ 载体）+ 旧单行居中文案形态
 * 零残留 + 设备侧消费 [1] 纯逻辑单源 + 页点显隐消费 kidpal_render_node_front_is_
 * overlay（与键路由同源）; 树扫 keybar.{h,c}: 零闭集字面重抄 + 零 layer 进出裁决。
 * RED today: 各 needle 零命中 / keybar.{h,c} 不存在 / 旧 label 在世（10 处）。
 * ------------------------------------------------------------------------ */
/* coverage: KB-A1 -- 三槽结构结构齿：三槽冻结符号 + bottom_bar_ 通栏贴底载体 + 旧单行居中文案形态零残留 + 设备侧消费纯逻辑单源 */
/* coverage: KB-A4 -- 页点显隐 layer_top 同源结构齿：lcd_display.cc 消费 kidpal_render_node_front_is_overlay（与板级键分流同源） */
/* coverage: KB-A3 -- 左右槽单源结构齿：apply_look 左槽消费 look.keycap、keybar 模块零闭集字面重抄 */
/* coverage: KB-E3 -- 零裁决权守卫：keybar 模块零 kidpal_layer_push/pop/stack（只读显隐, 覆盖层进出裁决 owner = key-layer-dispatch） */
TEST_CASE("键帽条三槽: 三槽结构+layer_top同源+零裁决权 (KB-A1/KB-A4/KB-E3)",
          "[keybar_three_slot][KB-A1][KB-A4][KB-E3][structural]")
{
    char *region;
    int n;

    KKB_REQUIRE_ROOT();

    /* tooth 1: 纯逻辑单源文件在世（[1] home; 后续负向齿以在世为前提, 防假绿）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_exists(KKB_KEYBAR_C) && kkb_exists(KKB_KEYBAR_H),
        "KKB-A1 RED: firmware/main/display/keybar.{h,c} 不存在——keybar 纯逻辑单源"
        "未落地（seam [1]; 页点/左右槽机制值必须住单源, 不得散落 lcd_display.cc）");

    /* ---- KB-A1 三槽结构（lcd_display.cc SetupUI bottom_bar_ 段）------------- */

    /* tooth 2: 三槽冻结符号落位（builder 按名落地, seam [4]）。 */
    n = kkb_count_in_file(KKB_LCD_CC, "g_kidpal_keybar_left_label");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kkb_fm("KKB-A1 RED: lcd_display.cc g_kidpal_keybar_left_label 仅 %d 处（期望 "
               ">= 3: 声明+创建+随态写点）——左 KEY 提示槽未落三槽结构", n));
    n = kkb_count_in_file(KKB_LCD_CC, "g_kidpal_keybar_dots_box");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kkb_fm("KKB-A1 RED: lcd_display.cc g_kidpal_keybar_dots_box 仅 %d 处（期望 "
               ">= 2: 声明+创建）——中页点槽容器未落三槽结构", n));
    n = kkb_count_in_file(KKB_LCD_CC, "g_kidpal_keybar_right_label");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kkb_fm("KKB-A1 RED: lcd_display.cc g_kidpal_keybar_right_label 仅 %d 处（期望 "
               ">= 3: 声明+创建+随态写点）——右 BOOT 提示槽未落三槽结构", n));

    /* tooth 3: 通栏贴底载体——三槽建在 bottom_bar_（既有常驻 chrome 载体, 通栏贴底
     * 对齐在世零触碰; 左右 label + 页点容器各按创建原语计数）。 */
    n = kkb_count_in_file(KKB_LCD_CC, "lv_label_create(bottom_bar_)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kkb_fm("KKB-A1 RED: lcd_display.cc lv_label_create(bottom_bar_) 仅 %d 处（期望 "
               ">= 3: 聊天 label + 左右两槽）——左右槽未建在常驻条上 (KB-A1 通栏贴底"
               "三槽; 条本身任何状态常驻已由 bottom_bar_ 载体承载)", n));
    n = kkb_count_in_file(KKB_LCD_CC, "lv_obj_create(bottom_bar_)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        kkb_fm("KKB-A1 RED: lcd_display.cc lv_obj_create(bottom_bar_) 仅 %d 处（期望 "
               ">= 1: 页点槽容器）——中槽容器未建在常驻条上", n));

    /* tooth 4: 旧形态替换——g_kidpal_keycap_label（旧单行居中文案 label）全文件零
     * 残留（现状 10 处 = 声明/创建/随态写点全在世; 三槽左槽是其继任, 不是并存）。 */
    n = kkb_count_in_file(KKB_LCD_CC, "g_kidpal_keycap_label");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        kkb_fm("KKB-A1 RED: lcd_display.cc 旧单行居中文案 g_kidpal_keycap_label 仍"
               "残留 %d 处——三槽替换未拆除旧形态 (KB-A1: 替换现单行居中文案形态)",
               n));

    /* tooth 5: 设备侧消费 [1] 纯逻辑单源（页点/左右槽机制值全部出自 keybar 模块,
     * 设备侧零重算——防设备侧另立一套槽位语义, key_route_layer 单源纪律同款）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_LCD_CC, "kidpal_keybar_dots(") >= 1,
        "KKB-A1 RED: lcd_display.cc 未消费 kidpal_keybar_dots——页点显隐设备侧重算, "
        "纯逻辑单源被绕过 (seam [3]; 防第二真相源)");
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_LCD_CC, "kidpal_keybar_slots(") >= 1,
        "KKB-A1 RED: lcd_display.cc 未消费 kidpal_keybar_slots——左右槽文案设备侧重算, "
        "纯逻辑单源被绕过 (seam [3])");

    /* ---- KB-A4 layer_top 同源 ------------------------------------------------ */

    /* tooth 6: 页点显隐与键路由同源——lcd_display.cc 消费 kidpal_render_node_
     * front_is_overlay()（K1a 分层模型的前台归一化查询, 板级 dispatch 键分流消费的
     * 同一事实源; 不另立第二真相源, KB-A4/取舍 B）。 */
    n = kkb_count_in_file(KKB_LCD_CC, "kidpal_render_node_front_is_overlay");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KKB-A4 RED: lcd_display.cc 零 kidpal_render_node_front_is_overlay——"
        "页点显隐未读 layer_top（与键路由同源红线, KB-A4/取舍 B; 现状 0 处）");

    /* ---- KB-A3 左右槽单源 ----------------------------------------------------- */

    /* tooth 7: apply_look 左槽消费 look.keycap（闭集单源——左槽随态写点住五态 look
     * 消费函数内, kidpal_voice_state_look 调用点与 keycap 写点同区）。 */
    region = kkb_region_between(KKB_LCD_CC,
                                "static void kidpal_voice_apply_look(int scene)",
                                "static void kidpal_chrome_mid_sync_locked(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KKB-A3 RED(infra): lcd_display.cc 的 kidpal_voice_apply_look 函数区切取失败"
        "（签名锚漂移——函数被改名/移动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(kkb_contains(region, "kidpal_voice_state_look(") &&
                             kkb_contains(region, "look.keycap") &&
                             kkb_contains(region, "g_kidpal_keybar_left_label"),
        "KKB-A3 RED: kidpal_voice_apply_look 区内左槽未消费 look.keycap（闭集单源）"
        "或未写左槽符号——左右槽随态切换另立文案来源 (KB-E2 零偏移红线)");
    free(region);

    /* tooth 8: keybar 模块零闭集字面重抄（五键帽短语只住 kidpal_voice_states.c——
     * keybar.c 重抄 = 第二来源, 闭集漂移时两处不同步）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_KEYBAR_C, "说完啦") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_C, "不等了") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_C, "打断") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_C, "知道了") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_C, "叫醒它") == 0,
        "KKB-E2 RED: keybar.c 重抄闭集键帽字面——左槽值域必须经 kidpal_voice_state_"
        "look 单源取 keycap (KB-E2; 字面只住 kidpal_voice_states.c)");

    /* ---- KB-E3 零裁决权 ------------------------------------------------------ */

    /* tooth 9: keybar 模块零 layer 进出裁决（只读显隐——零 kidpal_layer_push/pop/
     * stack_t; 覆盖层进出裁决 owner = 260915-m1-k1a-key-layer-dispatch, KB-E3）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_KEYBAR_C, "kidpal_layer_push") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_C, "kidpal_layer_pop") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_H, "kidpal_layer_push") == 0 &&
                             kkb_count_in_file(KKB_KEYBAR_H, "kidpal_layer_pop") == 0,
        "KKB-E3 RED: keybar 模块出现 layer 进出调用——keybar 获得覆盖层进出裁决权 "
        "(KB-E3 红线: keybar 只读 layer_top 显隐, 裁决 owner = key-layer-dispatch)");
    TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_file(KKB_KEYBAR_C, "kidpal_layer_stack_t") == 0,
        "KKB-E3 RED: keybar.c 自持 layer 栈——第二真相源 (KB-A4/KB-E3: 显隐只读"
        "front_is_overlay 归一化值, 不自建栈)");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — boundary：本文件注册序守卫 + KB-E1 spec/契约 canary + 扫描器自检
 *   KB-A6 结构半（本文件 + keybar.c + kidpal_voice_states.c 是否被 builder 接线进
 *   HOST_ACCEPTANCE_SRCS）
 *   KB-E1 需求面零修订 canary（requirements §2.2 comp-keybar 锚 + 契约面在世）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: KB-A6 -- host runner 终跑：本文件+keybar.c+kidpal_voice_states.c 注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚 */
/* coverage: KB-E1 -- 范围纪律 canary：requirements §2.2 comp-keybar 锚在世（spec 零修订）+ 契约面在世 */
TEST_CASE("键帽条三槽: 注册序守卫+需求契约canary+扫描器自检 (KB-A6/KB-E1)",
          "[keybar_three_slot][KB-A6][KB-E1][boundary]")
{
    char *mk;
    int n;

    KKB_REQUIRE_ROOT();

    /* tooth 1: 本文件 + keybar.c + kidpal_voice_states.c 接线进 HOST_ACCEPTANCE_
     * SRCS 且排在 host_infra_smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿
     * 注册序守卫）。RED until builder wires（seam [5]）。 */
    mk = kkb_read(KKB_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "KKB RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "keybar_three_slot.acceptance.c");
        const char *impl = strstr(mk, "../../../display/keybar.c");
        const char *voice = strstr(mk, "../../../display/kidpal_voice_states.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "KB-A6 RED: keybar_three_slot.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
            " — host 验收构建未接本包（seam [5] BUILDER DUTY）");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "KB-A6 RED: display/keybar.c 未接线进 HOST_ACCEPTANCE_SRCS — 行为齿强符号"
            "无处落地（weak 哨兵将持续红）");
        TEST_ASSERT_TRUE_MESSAGE(voice != NULL,
            "KB-A6 RED: display/kidpal_voice_states.c 未接线进 HOST_ACCEPTANCE_SRCS"
            " — 闭集复核强符号无处落地（weak 哨兵将持续红）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "KKB RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失"
            " — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "KB-A6 RED: keybar_three_slot.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (KB-E1 canary): 需求面在世且 §2.2 comp-keybar 三槽锚仍在（spec 零
     * 修订红线——页点数量 deviation 走 workstream launchPreconditions 显式登记, 不
     * 预先改需求; spec 冲突走 spec-lane 逃生门）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_exists(KKB_REQUIREMENTS),
        "KB-E1 RED(infra): docs/requirements.md 丢失——需求面被触碰（KB-E1 零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(kkb_file_contains(KKB_REQUIREMENTS, "comp-keybar") &&
                             kkb_file_contains(KKB_REQUIREMENTS, "三槽"),
        "KB-E1 RED: requirements §2.2 comp-keybar 三槽锚丢失——需求面被修订（KB-E1: "
        "spec 面零修订, deviation 只能以显式登记形态存在）");
    TEST_ASSERT_TRUE_MESSAGE(kkb_exists("docs/contracts/interface-contract.md"),
        "KKB RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(kkb_exists(KKB_VOICE_C),
        "KKB RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(kkb_exists("firmware/main/__kkb_no_such__.cc"),
        "KKB RED(infra): scanner reports a nonexistent file as present");
    n = kkb_count_in_file(KKB_VOICE_C, "__kkb_no_such_needle__");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KKB RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = kkb_count_in_file(KKB_VOICE_C, "kCap");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KKB RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region2 = kkb_region_between(KKB_VOICE_C,
                                           "int kidpal_voice_state_look(int scene",
                                           NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(region2,
            "KKB RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(kkb_count_in_slice(region2, "kCapListen") >= 1,
            "KKB RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region2);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — ACC-5 查字窗键帽随态值域（behavioral, frozen seam [7]; 强符号 home =
 * display/keybar.c——已在 runner 在册, builder 落地新并行函数后覆盖本文件 weak 哨兵,
 * 零新增接线; TaskPackage 260928-char-window-key-triggered-play additive）
 * RED today: weak 哨兵 -2（keybar.c 尚无 kidpal_keybar_char_slots——设计态红）。
 * ------------------------------------------------------------------------ */
/* coverage: ACC-5 -- 查字窗键帽随态值域行为齿（WF-F3-01 键帽随态变注, requirements v1.59）: kidpal_keybar_char_slots 决策表——started=0 -> (播报, 结束) / started=1 -> (重播, 结束) / 任意非 0 started -> (重播, 结束) / NULL out 防御 -1 + keybar.h 声明在世结构半（ACC-6 接线前提） */
TEST_CASE("键帽条三槽: 查字窗键帽随态——未起播「播报」/起播后「重播」 (ACC-5)",
          "[keybar_three_slot][ACC-5][behavior]")
{
    const char *left = NULL;
    const char *right = NULL;
    int rc;

    /* tooth 1 (本包红齿): started = 0（演示未起播）-> (播报, 结束)。 */
    rc = kidpal_keybar_char_slots(0, &left, &right);
    kkb_require_implemented(rc, "kidpal_keybar_char_slots unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(
        rc == 0 && left != NULL && right != NULL &&
            strcmp(left, "播报") == 0 && strcmp(right, "结束") == 0,
        kkb_fm("ACC-5 RED: kidpal_keybar_char_slots(0) = (rc=%d, 左=%s, 右=%s) 期望 "
               "(0, 播报, 结束)——查字窗未起播键帽漂移（WF-F3-01 键帽随态变注 v1.59: "
               "未起播显示「播报」; 右槽 BOOT「结束」）",
               rc, left ? left : "(null)", right ? right : "(null)"));

    /* tooth 2 (本包红齿): started = 1（演示已起播）-> (重播, 结束)。 */
    rc = kidpal_keybar_char_slots(1, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(
        rc == 0 && left != NULL && right != NULL &&
            strcmp(left, "重播") == 0 && strcmp(right, "结束") == 0,
        kkb_fm("ACC-5 RED: kidpal_keybar_char_slots(1) = (rc=%d, 左=%s, 右=%s) 期望 "
               "(0, 重播, 结束)——查字窗已起播键帽漂移（WF-F3-01 键帽随态变注 v1.59: "
               "起播后显示「重播」）",
               rc, left ? left : "(null)", right ? right : "(null)"));

    /* tooth 3: started 语义 = 未起播 0 / 已起播非 0——任意非 0 值须落「重播」列
     * （不为 1 特判留缝）。 */
    rc = kidpal_keybar_char_slots(2, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(
        rc == 0 && left != NULL && strcmp(left, "重播") == 0,
        kkb_fm("ACC-5 RED: kidpal_keybar_char_slots(2) 左槽 = %s 期望 重播——started "
               "非 0 未一律落「重播」列（started 语义 = 未起播 0 / 已起播非 0）",
               left ? left : "(null)"));

    /* tooth 4 (防御): NULL out 任一 -> -1（从不为 UB）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_char_slots(0, NULL, &right) == -1,
        "ACC-5 RED: NULL out_left 未防御（-1）——查字窗键帽槽把空指针当合法");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_char_slots(0, &left, NULL) == -1,
        "ACC-5 RED: NULL out_right 未防御（-1）——查字窗键帽槽把空指针当合法");

    /* tooth 5 (结构半, 本包红齿): keybar.h 声明 kidpal_keybar_char_slots 在世
     * （lcd_display.cc sync 查字窗分支经 include 消费——ACC-6 接线前提）。 */
    KKB_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(
        kkb_count_in_file(KKB_KEYBAR_H, "kidpal_keybar_char_slots") >= 1,
        "ACC-5 RED: keybar.h 零 kidpal_keybar_char_slots 声明——查字窗键帽槽值域未进"
        "纯逻辑单源头（lcd_display.cc 消费无从 include; plan.md 方案 B）");
}

/* ---------------------------------------------------------------------------
 * KB-A6 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = 2026-09-16 本文件冻结时 host 面全量既有齿实跑（Pod intake 通道①）:
 *   13 Tests / 0 Failures / 0 Ignored（K1a key_layer_dispatch 5 + N1b p1_home_battery
 *   5 + 包 0 host_infra_smoke 3）, 与在册锚逐条一致（n1b final r12 锚, 量测锚
 *   e990eb2; consumer 差异判停 git diff e990eb2 HEAD -- firmware/ 零差异, 本 HEAD
 *   a398264 有效引用）。在册红 = 零, 判读纪律 = 新增红才是本包责任。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1-3（keybar.{h,c} 未落地 + kidpal_voice_states.c/keybar.c 未接
 *       进 runner 的 weak 哨兵红——真实缺失行为）;
 *     - test 4 全部（三槽结构/layer_top 同源/零裁决权结构红: 三槽冻结符号零命中、
 *       旧 label 10 处在世、keybar.{h,c} 不存在）;
 *     - test 5 tooth 1（注册接线守卫, builder 接线后转绿）。
 *   MUST-STAY-GREEN at baseline（钉点/守卫齿, 现状即绿）:
 *     - test 5 tooth 2/3（需求契约 canary: requirements §2.2 comp-keybar/三槽锚在世
 *       + 契约面在世; 扫描器自检）。
 *   KB-A5 真机确认轮: human 持板（三槽条 + 页点显隐 + 随态文案真机可见）, 证据落
 *   delivery/taskpackages/260915-m1-k2-keybar-three-slot/。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑（新齿未接线前）= 有效基线 13/0（见上, 2026-09-16
 *   实跑, 命令见 Run protocol; 记录 = delivery/taskpackages/260915-m1-k2-keybar-
 *   three-slot/red-baseline-freeze.md 五要素）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/keybar_three_slot.
 *         acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性（逐齿对现状必红的
 *     事实源, 2026-09-16 grep 实测）:
 *       - firmware/main/display/keybar.{h,c} 不存在; kidpal_keybar_dots/kidpal_
 *         keybar_slots 全仓零命中（test 1/2 行为哨兵红 + test 4 tooth 1/5 红）;
 *       - lcd_display.cc: g_kidpal_keybar_* 三符号零命中、g_kidpal_keycap_label 10 处
 *         在世、lv_obj_create(bottom_bar_) 0 处、lv_label_create(bottom_bar_) 恰 2 处
 *         （< 3）、kidpal_render_node_front_is_overlay 0 处（test 4 tooth 2-4/6 红;
 *         tooth 7 区锚 apply_look 在世, 区内无左槽符号 = 红）;
 *       - kidpal_voice_states.c 未接进 host runner（test 3 行为半哨兵红; 结构半
 *         kCap 恰 5 + 零 结束/关闭 字面 = 现状绿, 接线后同样绿）;
 *       - host_app/main/CMakeLists.txt 无 keybar_three_slot.acceptance.c / keybar.c /
 *         kidpal_voice_states.c 接线行（test 5 tooth 1 红）;
 *       - canary 绿: requirements.md 含 comp-keybar/三槽锚、契约面在世、PAGE_COUNT=4
 *         （test 1 tooth 1 + test 5 tooth 2 现状绿）。
 *         〔2026-09-18 留痕注记（acceptance-author, TaskPackage 260917-m2-k2-retract-
 *         p4-three-page）: 上行为 2026-09-16 冻结相位的历史实测记录，不改写。该相位
 *         的 PAGE_COUNT=4 已随本包撤 P4 变为 3；test 1 tooth 1 与 test 2 第 4 页槽位齿
 *         已同步改为同源/越界防御形态，**撤页前后皆绿、不新增红**，字面 3 的单点钉死
 *         移住 firmware/main/test/retract_p4_three_page.acceptance.c test 3。详见本文件
 *         尾 REVISION LOG 2026-09-18 条目。〕
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）。
 * ---------------------------------------------------------------------------
 * KB-A5 真机证据要求（本文件不承载, human 持板轮）:
 *   ① 三槽条真机形态: 左 KEY 提示 / 中页点 / 右 BOOT 提示, 通栏贴底, 覆盖层
 *      （提醒弹窗/查字窗/语音轮演出）盖不掉条体; ② 页点: 仅主页面显示、当前页实心
 *      放大、覆盖层内消失; ③ 左右槽随态文案: 语音轮左槽逐字（说完啦/不等了/打断/
 *      知道了）+ 右槽（结束/关闭）。证据（照片/记录）落 delivery/taskpackages/
 *   260915-m1-k2-keybar-three-slot/。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-16  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-k2-keybar-three-slot).
 *   2026-09-18  KB-E1 deviation 对账收回的两处同源化修订（acceptance-author 留痕修订，
 *               授权通道 = TaskPackage 260917-m2-k2-retract-p4-three-page M2K2-E2）。
 *               ① test 1 tooth 1：原 `PAGE_COUNT == 4` 与本齿自注「M2 后随枚举自动
 *               4→3、不需改本文件」自相矛盾（撤页必假红），改为同源判别 dots 报出的
 *               total == PAGE_COUNT + keybar.c 取源仍是这枚枚举（(int)PAGE_COUNT 计数
 *               >= 2）；本文件不再对 PAGE_COUNT 取字面值。② test 2 第 4 页槽位齿：原
 *               `slots(3, …)` 钉「存在性」，撤页后 index 3 == 新 PAGE_COUNT 会假红，
 *               改为 `slots(PAGE_COUNT, …) == -1` 的越界防御齿（同 test 防御列已含
 *               page=-1 齿，不重复）。为何属留痕通道而非绕闸：只随已冻结的需求事实
 *               （§2.2「中 3 页点 / 页点随之 4→3」）把 deviation 锚收回为同源形态，未
 *               放宽任何本包语义断言；test 3/4/5 与本文件其余齿零改动。红态后果：两处
 *               修订在撤页前后皆绿（不新增红），本包设计态红全部由
 *               firmware/main/test/retract_p4_three_page.acceptance.c 承载。
 *   2026-09-28  ACC-5 additive（acceptance-author, TaskPackage
 *               260928-char-window-key-triggered-play）: 追 seam [7] + test 6——查字窗
 *               键帽随态值域 kidpal_keybar_char_slots 行为齿（started=0 -> 播报 /
 *               started=1 及任意非 0 -> 重播 / 右槽恒 结束 / NULL 防御 -1）+ keybar.h
 *               声明在世结构半。需求权威 = requirements v1.59 WF-F3-01 键帽随态变注;
 *               home = keybar.{h,c} 新并行函数（既有 kidpal_keybar_slots 签名与值域
 *               零触碰, additive 既有齿不破）。强符号源 keybar.c 已在 runner 在册
 *               （既有接线）, 零新增接线。本包义务红（设计态红, builder 落地后转绿）
 *               = test 6 全部（weak 哨兵 + keybar.h 零声明）; MUST-STAY-GREEN =
 *               本文件其余全部既有齿（值域 additive 不破）。
 * --------------------------------------------------------------------------- */
