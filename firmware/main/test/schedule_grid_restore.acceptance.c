/*
 * Frozen acceptance tests — TaskPackage 260923-m3-k7-schedule-grid-restore (subsystem:
 * firmware; workstream m3-schedule; 课表网格冷启动恢复 · rigor=full, riskTier=medium;
 * sourceType = arch-change-handoff, 上游冻结 2026-09-23 commit 565cb2d).
 *
 * Story: S-F1.2 · 课表页看「一周总览」——冷启动/重启后翻到课表页即见网格（AC1 冷启动
 * 场景成立）：云端三触发附带推 data.restore 周网格恢复性载荷，设备收到后**只填 RAM
 * 网格缓存、不接管前台**，P2 在屏时仅重绘本页。语义权威 = plan.md + ADR-005（docs/
 * ADR.md）+ 契约 §1.3②（docs/contracts/interface-contract.md L85 data.restore additive
 * / L86 生产纪律 / L87 板子行为句 modifying，2026-09-23 契约门批准，landing commit
 * 565cb2d）；server 对端面 = test_schedule_grid_restore.acceptance.py（本文件按对端
 * 实际接受什么钉 wire 锚，不按本侧想象——260907-g-relay-fix 教训）。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan, K6 boot_auto_connect 先例) + restore wire
 * 容忍 BEHAVIORAL（weak 哨兵, schedule_grid.acceptance.c 先例）frozen acceptance。
 * restore 消费分支宿主 = mcp_server.cc（设备组件树, host 不可链）+ kidpal_render_node.cc
 * 在屏缝（同因）——故三前台态层栈行为、缓存重复注入字节幂等的**行为本体** host 钉不到，
 * 以结构代理 + 强度申报显式化（见下），真实行为归真机窗 K7-A8（头注 DEFERRED 段）。
 *
 * ===========================================================================
 * Frozen seam contract pinned by this file（builder implements，本文件只读扫真实
 * 工作树；「home =」为冻结落点，与 plan.md §3 改动面一致）:
 *
 *   [S1] restore 消费分支（home = firmware/main/mcp_server.cc schedule 路由 case 4,
 *        现锚 L605-648）: 路由内读 data.restore 布尔（JSON 键字面 "restore"）——
 *        true 走恢复性分支:
 *          - kidpal_schedule_grid_parse 解析（与薄载荷路径共用同一解析, 单一解析点）;
 *          - 成功: **显式持 LVGL 锁**（lvgl_port_lock——restore 缓存写路径无
 *            begin_page 隐式持锁, challenger C3 canary, plan 红线）+
 *            kidpal_render_node_cache_schedule_grid 填缓存 + 若
 *            kidpal_render_node_schedule_on_screen() 真 -> kidpal_schedule_grid_render
 *            仅重绘本页（ADR-005 R10: P2 在屏重绘、接管态仅填缓存）;
 *          - **零 kidpal_render_node_begin_page**（不推 SERVER_PAGE、不接管前台——
 *            文件级 begin_page 调用计数冻结基线恒 5, restore 分支不得新增）;
 *          - 解析失败降级 = **静默记日志**（schedule 路由区 SetChatMessage 计数
 *            冻结基线恒 1——既有非 restore 降级那处; restore 降级绝不弹提示,
 *            ADR-005「区别于主动展示路径的 SetChatMessage 降级」）。
 *        data.restore 缺省/显式 false = 现行为完全不变（契约 L85 additive; R7
 *        absent/false 同判——非 restore 路径 begin_page + SERVER_PAGE 接管照旧）。
 *
 *   [S2] 在屏查询缝（home = firmware/main/display/kidpal_render_node.{h,cc}）:
 *        int kidpal_render_node_schedule_on_screen(void);
 *          -- P2 在屏谓词，沿 home on_screen 先例口径逐字同构
 *             （kidpal_render_node_home_on_screen, L472-479）:
 *             kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE &&
 *             s_local_page == KP_PAGE_SCHEDULE。三前台态语义（K7-A5）:
 *             P1 在屏（HOME）-> 0 仅填缓存 / P2 在屏（SCHEDULE）-> 1 重绘本页 /
 *             接管态（SERVER_PAGE 等顶层）-> 0 仅填缓存不重绘。.h 声明 + .cc 定义。
 *
 *   [S3] 薄载荷路径零回归（K7-A6, M3-K3 冻结行为回归守卫）: schedule 路由区内
 *        kidpal_render_node_begin_page -> kidpal_schedule_grid_render ->
 *        kidpal_render_node_cache_schedule_grid 调用序零偏移（先接管渲染再填缓存
 *        的既有序）; begin_page 函数区推 KIDPAL_LAYER_SERVER_PAGE 在世;
 *        home 路由「不抢前台」先例区（home_on_screen + home_upcoming_apply）在世
 *        —— R10 谓词口径的机制参照系。
 *
 *   [S4] restore wire 容忍（K7-A7 跨端形态锚, weak 哨兵行为齿）: 对端 grid restore
 *        帧 data 字符串 = json.dumps({"restore": <bool>, "week": {...}})——restore
 *        键在 data 对象顶层（先/后位序都可能）。设备解析面 = 冻结
 *        kidpal_schedule_grid_parse（M3-K2）: 数据对象级未知键经 kspj_skip_value
 *        忽略（true/false literal 分支在世, schedule_page.c L280-289 实证）——
 *        本文件 weak 哨兵在 host runner 被 schedule_page.c 强定义覆盖后真验:
 *        带 restore 键（true/false、先/后位序）解析结果 == 不带键同参（count 与
 *        逐字段）。弱于契约处: standalone 线束（无强符号）只能红在哨兵 -2。
 *
 *   [S5] 注册 + canary（K7-A6/E2 义务）: 本验收件接线 HOST_ACCEPTANCE_SRCS（smoke
 *        末位序守卫）; 既有齿注册在世; 需求/契约面 canary（恢复性推送句 +
 *        data.restore bullet——上游 565cb2d 已落, 本包零契约触碰红线）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K7-A5 firmware restore 消费锚 -> test 1（restore 分支: "restore" 键
 *             读取 + 显式 LVGL 锁 + on-screen 谓词消费 + begin_page 计数恒 5 +
 *             SetChatMessage 区计数恒 1 静默降级）+ test 2（三前台态谓词形态 +
 *             缓存覆写形态 = memcmp 幂等的结构承载）。真实三前台态层栈快照与
 *             重复注入字节幂等的行为本体 = deferred 设备轮（K7-A8③）。
 *   coverage: K7-A6 语音接管回归守卫 -> test 3（薄载荷路径 begin_page/render/cache
 *             调用序 + begin_page 区 SERVER_PAGE push + home 不抢前台先例区——
 *             全 must-stay-green, 红基线即绿; 变异敏感点见 MUTATION REPLAY M3）。
 *   coverage: K7-A7 piggyback 形态锚 -> test 4（restore wire 容忍行为齿, weak
 *             哨兵 + server 单源 producer face 结构锚）。server 侧三触发帧序/恰
 *             一次/开关门控 = test_schedule_grid_restore.acceptance.py A7 齿;
 *             触发时序归真机窗 K7-A8①。
 *   coverage: K7-A8 真机锚（四子锚）-> 冻齿侧零 host 齿，仅本头注 deferred 登记
 *             （①冷启动 P1 恢复 + grid restore 接收日志（开关开后）；②冷启动直翻
 *             P2 网格有内容 = S-F1.2 AC1 冷启动成立（本包目标场景）；③语音「我的
 *             课表」接管照旧（开关开/关两态）；④旧固件 + 开关开 = 确定性接管顶到
 *             P2 复核实测（C1 evidence））。挂设备轮窗口，human 在场，不阻塞 Pod。
 *   coverage: K7-E2 注册/铁律 canary -> test 5（本件注册 + 既有齿注册在世 + 需求/
 *             契约 canary + 扫描器自检）。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载齿 -> 性质档; acceptance-evidence T2 纪律）:
 *   契约 §1.3②「restore=true 只填缓存不接管前台（L87 板子行为句）」-> K7-A5 齿组
 *     -> 机制级（restore 分支零 begin_page 计数钉 + on-screen 谓词消费 + 显式持锁）
 *     + 源锚代理（SetChatMessage 区计数 = 静默降级的文本代理）。行为等价假设
 *     （显式化）:
 *     ① 「begin_page 计数恒 5 ≙ restore 分支不推 SERVER_PAGE」——依据 = 薄载荷
 *        路径既有调用恰 1 处（区钉）+ 其余 4 处皆其它路由既有锚; 若 builder 在
 *        restore 分支加接管调用, 计数 6 -> 红（敏感）。
 *     ② 「谓词形态 == 三前台态语义」——LOCAL_PAGE 判定在世钉「接管态 -> 0」的
 *        谓词口径（R10）; 真层栈行为归真机 K7-A8③, host 不可执行设备静态态。
 *     ③ 「缓存覆写形态 ≙ 重复注入幂等」——cache setter memcpy 覆写 + s_grid_n
 *        重赋值形态在世（M3-K2 冻结形, append/累加形态无语法位）; 字节级幂等
 *        归真机 K7-A8② 顺带核。
 *   ADR-005 部署时序（开关默认关）-> server pytest A3 齿承载（本文件无齿, 唯
 *     对端 producer face 结构锚 test 4 tooth 4）。
 *   K7-A7 wire 容忍 -> 行为级（weak 哨兵 + host runner 强定义覆盖后真解析断言）;
 *     standalone 线束只能红在哨兵（-2）——设计态红判读见文件尾。
 *   K7-A6 冻结行为 -> 机制级（调用序位次 + SERVER_PAGE push 在世 + home 先例区
 *     在世）; 现状即绿 = 回归守卫的设计态（红基线通道② 唯一 must-stay TEST）。
 *
 * ---------------------------------------------------------------------------
 * DEFERRED（真机半 + 相位义务，不阻塞 host 半收口）:
 *   - K7-A8 四子锚真机实证（设备轮窗口，human 在场; K7-E4）: ①冷启动无按键 ->
 *     P1「接下来」恢复 + 串口见 grid restore 载荷接收日志（开关开后）；②冷启动
 *     直翻 P2 网格有内容；③语音「我的课表」接管照旧（开关开/关两态各一次）；
 *     ④旧固件 + 开关开 = 确定性接管顶到 P2 实测记录（烧新固件前）。冻齿侧零
 *     host 齿、不伪造 host 证据。
 *   - device build 闸: xiaozhi.bin 构建过 + 体积余量记录（builder/verifier 相位，
 *     K7-E1）。
 *   - mutation 实跑: mutationEvidence.perAcceptance 三条 expectedRed 补跑（builder/
 *     收尾相位），原始输出逐 acceptance 落 delivery/taskpackages/
 *     260923-m3-k7-schedule-grid-restore/mutation-evidence/（K6 纪律延续）。
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，HOST_TEST_CONVENTIONS.md [C4.2] 约定）:
 *   1) firmware/main/mcp_server.cc schedule 路由（case 4）: [S1] restore 消费分支
 *      （读 "restore" 布尔; true 分支显式 lvgl_port_lock + cache + on-screen 重绘;
 *      零 begin_page; 降级静默 log; 非 restore 现行为零偏移）。
 *   2) firmware/main/display/kidpal_render_node.h: 声明
 *      int kidpal_render_node_schedule_on_screen(void);（[S2]）。
 *   3) firmware/main/display/kidpal_render_node.cc: 定义 [S2] 谓词（home_on_screen
 *      同构; 只读静态态, 零副作用）。
 *   4) host 接线: firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../schedule_grid_restore.acceptance.c"，置于
 *      "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位）。
 *   5) device build 闸: xiaozhi.bin 构建过 + 体积余量记录（builder/verifier 相位）。
 *   6) mutation 自证（mutationRequired=true）: 变异态红由通道②独立命令复现（映射
 *      表见文件尾 MUTATION REPLAY MAP），原始输出落
 *      delivery/taskpackages/260923-m3-k7-schedule-grid-restore/mutation-evidence/。
 *      变异编辑不得顺手改动既有齿与未列面。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY（本验收件注册 HOST_ACCEPTANCE_SRCS 后随
 *      host runner 全量执行; 本文件零新实现源——结构齿扫真实工作树, TEST 4 行为
 *      齿经 weak 哨兵被在世 schedule_page.c 强定义覆盖）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* -------------------------------------------------------------------------- *
 * 被扫文件（真实工作树，root 自手持 cwd 定位——evidence 类扫描齿 root 注入纪律，
 * 禁依赖调用点 cwd; test 目录排除——本冻结文件自身住 test 目录，其注释合法提及
 * needle，不进扫描）。
 * ------------------------------------------------------------------------ */
#define KSR_MCP       "firmware/main/mcp_server.cc"
#define KSR_RN_CC     "firmware/main/display/kidpal_render_node.cc"
#define KSR_RN_H      "firmware/main/display/kidpal_render_node.h"
#define KSR_SCHED_C   "firmware/main/display/schedule_page.c"
#define KSR_HOST_CML  "firmware/main/test/host_app/main/CMakeLists.txt"
#define KSR_REQ_MD    "docs/requirements.md"
#define KSR_CONTRACT  "docs/contracts/interface-contract.md"
#define KSR_PUSH_PY   "server/gateway/schedule_push.py"
#define KSR_FRONT_PY  "server/gateway/front/front_app.py"

#define KSR_MAX_PATH 1024
#define KSR_MAX_READ (8u * 1024u * 1024u)

/* 冻结 needle / 基线（本文件为单源; 值 = 2026-09-23 冻结基线逐 needle 实证）:
 *   - mcp_server.cc `kidpal_render_node_begin_page` 调用恰 5 处（:104 既有 +
 *     :638 schedule 薄载荷 + :722/:776/:796 story 族——restore 分支不得新增第 6 处,
 *     K7-A5「不接管前台」的计数承载）。
 *   - mcp_server.cc schedule 路由区（VRT_ROUTE_SCHEDULE -> VRT_ROUTE_CHAR）:
 *     SetChatMessage 恰 1 处（:630 既有降级）——restore 降级静默的负 needle 基线;
 *     kidpal_render_node_begin_page 恰 1 处（薄载荷路径在世）;
 *     lvgl_port_lock 0 处（现状; builder 落显式持锁后 >= 1）;
 *     `"restore"`（JSON 键字面）0 处（现状; builder 落分支后 >= 1）。
 *   - kidpal_render_node.cc `kidpal_render_node_schedule_on_screen` 全仓 0 命中
 *     （设计态红前提, 2026-09-23 grep 实证）。 */
#define KSR_BEGIN_PAGE_ND   "kidpal_render_node_begin_page"
#define KSR_RESTORE_KEY_ND  "\"restore\""
#define KSR_ON_SCREEN_ND    "kidpal_render_node_schedule_on_screen"
#define KSR_GRID_RENDER_ND  "kidpal_schedule_grid_render"
#define KSR_CACHE_ND        "kidpal_render_node_cache_schedule_grid"
#define KSR_CHATMSG_ND      "SetChatMessage"
#define KSR_SCHED_START     "VRT_ROUTE_SCHEDULE"
#define KSR_SCHED_END       "VRT_ROUTE_CHAR"
#define KSR_HOME_START      "VRT_ROUTE_HOME"
#define KSR_HOME_END        "VRT_ROUTE_CONTROL"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — 常量 + weak 哨兵（schedule_grid.acceptance.c 先例;
 * host runner 里由在世 schedule_page.c 强定义覆盖, 多重 weak 合法、语义一致）。
 * ------------------------------------------------------------------------ */
#define KSR_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define KSG_NAME_CAP   48
#define KSG_COURSE_CAP 84

typedef struct {
    int  day;
    int  slot;
    char name[KSG_NAME_CAP];
    int  flags;
} kidpal_schedule_course_t;

__attribute__((weak)) int kidpal_schedule_grid_parse(const char *data_json,
                                                     kidpal_schedule_course_t *courses,
                                                     int cap)
{
    (void)data_json; (void)courses; (void)cap;
    return KSR_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), K6/KSP precedent.
 * All scans run against the REAL working tree located from the runner cwd.
 * ------------------------------------------------------------------------ */
static char ksr_root[KSR_MAX_PATH];
static int ksr_root_ready;

static int ksr_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int ksr_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../..", "../../../../../../.."
    };
    char probe[KSR_MAX_PATH];

    if (ksr_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (ksr_is_regular(probe)) {
            snprintf(ksr_root, sizeof(ksr_root), "%s", cands[i]);
            ksr_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int ksr_rel(char *out, size_t cap, const char *rel)
{
    if (!ksr_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", ksr_root, rel);
    return 1;
}

static int ksr_exists(const char *rel)
{
    char p[KSR_MAX_PATH];
    return ksr_rel(p, sizeof(p), rel) && ksr_is_regular(p);
}

static char *ksr_read(const char *rel, size_t *out_len)
{
    char p[KSR_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ksr_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSR_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int ksr_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int ksr_file_contains(const char *rel, const char *needle)
{
    char *buf = ksr_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = ksr_contains(buf, needle);
    free(buf);
    return r;
}

static int ksr_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = ksr_read(rel, NULL);
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

static int ksr_count_in_slice(const char *slice, const char *needle)
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

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL.
 * Caller frees. end_needle == NULL -> slice to EOF. */
static char *ksr_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = ksr_read(rel, NULL);
    const char *s, *e;
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
    if (end_needle == NULL) {
        e = buf + strlen(buf); /* 无尾锚——切到 EOF */
    } else {
        e = strstr(s + strlen(start_needle), end_needle);
        if (e == NULL) {
            e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
        }
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* 函数区切取：start_needle 起至最近的下一个函数定义头（候选前缀取最早者）或 EOF。
 * 供新缝函数（无既有尾锚）使用（K6 kac_region_member_fn 的多前缀泛化形）。 */
static char *ksr_region_until_next_fn(const char *rel, const char *start_needle)
{
    static const char *const sigs[] = {
        "\nvoid kidpal_render_node_", "\nint kidpal_render_node_",
        "\nstatic void rn_", "\nstatic int rn_", "\nextern \"C\" void",
        "\nextern \"C\" int"
    };
    char *buf = ksr_read(rel, NULL);
    const char *s;
    const char *e = NULL;
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
    for (size_t i = 0; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
        const char *hit = strstr(s + strlen(start_needle), sigs[i]);
        if (hit != NULL && (e == NULL || hit < e)) {
            e = hit;
        }
    }
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

#define KSR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(ksr_locate_root(), \
    "K7 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper（K1a 先例：动态诊断先格式化进静态缓冲再进单串断言宏）。 */
static char ksr_fm_buf[1024];
static const char *ksr_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *ksr_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ksr_fm_buf, sizeof(ksr_fm_buf), fmt, ap);
    va_end(ap);
    return ksr_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — K7-A5 restore 消费分支正锚：data.restore 读取 + 显式 LVGL 持锁 +
 * on-screen 谓词消费 + begin_page 计数恒 5 + SetChatMessage 区计数恒 1。
 * MUST-STAY-GREEN 半：schedule 路由区既有锚在世（薄载荷路径零偏移）+ 扫描器自检。
 * RED today：restore 分支未实现（"restore" 键读取全文件 0 命中, grep 实证）。
 * ------------------------------------------------------------------------ */
/* coverage: K7-A5 -- restore 消费分支：路由读 data.restore + 显式 lvgl_port_lock（C3 canary）+ on-screen 谓词消费 + 文件级 begin_page 计数恒 5（不推 SERVER_PAGE 不接管前台）+ 路由区 SetChatMessage 恒 1（restore 降级静默） */
TEST_CASE("课表网格冷启动恢复: restore消费分支正锚 (K7-A5)",
          "[schedule_grid_restore][K7-A5][structural]")
{
    char *region;
    int n;

    KSR_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 扫描器自检防假绿): 已知文件可读 + 永不存在的
     * needle 零命中。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_exists(KSR_MCP) && ksr_exists(KSR_RN_CC) &&
                                 ksr_exists(KSR_RN_H),
        "K7 RED(infra): scanner cannot see known-good files — 扫描器失灵");
    TEST_ASSERT_TRUE_MESSAGE(ksr_count_in_file(KSR_MCP,
        "__k7_no_such_needle__") == 0,
        "K7 RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");

    /* tooth 2 (MUST-STAY-GREEN, 薄载荷路径零偏移): schedule 路由区既有冻结锚在世
     * ——begin_page 恰 1（薄载荷接管）、grid_render、cache 各在世。restore 改造
     * 不得拆既有路径（K7-A6/K7-E2 零回归义务）。 */
    region = ksr_region_between(KSR_MCP, KSR_SCHED_START, KSR_SCHED_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "K7 RED(infra): schedule 路由区切取失败——VRT_ROUTE_SCHEDULE/VRT_ROUTE_CHAR "
        "锚漂移须复核（禁整路由退役绕过）");
    TEST_ASSERT_TRUE_MESSAGE(ksr_count_in_slice(region, KSR_BEGIN_PAGE_ND) == 1 &&
                                 ksr_contains(region, KSR_GRID_RENDER_ND) &&
                                 ksr_count_in_slice(region, KSR_CACHE_ND) == 1,
        "K7-A5 RED: schedule 路由区薄载荷路径既有锚偏移（begin_page/grid_render/"
        "cache）——M3-K2 冻结行为被 restore 改造连带拆动（零回归义务）");

    /* tooth 3 (RED today): 路由读 data.restore 布尔——JSON 键字面 "restore" 在
     * mcp_server.cc >= 1（现状 0, 2026-09-23 grep 实证）。 */
    TEST_ASSERT_TRUE_MESSAGE((n = ksr_count_in_file(KSR_MCP, KSR_RESTORE_KEY_ND)) >= 1,
        ksr_fm("K7-A5 RED: mcp_server.cc 无 \"restore\" 键读取（%d 处）——data.restore "
               "消费分支未实现（冷启动恢复缺口 = 本包根因现状在世, BUILDER DUTY 1)）",
               n));

    /* tooth 4 (RED today, C3 canary): restore 缓存写路径显式持 LVGL 锁——路由区
     * lvgl_port_lock >= 1（无 begin_page 隐式持锁, plan 红线; 现状 0）。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_contains(region, "lvgl_port_lock"),
        "K7-A5 RED: schedule 路由区无 lvgl_port_lock——restore 缓存写路径未显式持 "
        "LVGL 锁（challenger C3 canary: 无 begin_page 隐式持锁, 裸写 = 与 LVGL "
        "任务竞态）");

    /* tooth 5 (RED today): restore 分支消费在屏谓词——kidpal_render_node_schedule_
     * on_screen 在路由区（P2 在屏重绘 / 接管态仅填缓存的 R10 分流落点）。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_contains(region, KSR_ON_SCREEN_ND),
        "K7-A5 RED: schedule 路由区未消费 kidpal_render_node_schedule_on_screen——"
        "P2 在屏重绘分流缺失（restore 到达恒只填缓存, P2 在屏看不到新数据, "
        "ADR-005 R10）");

    /* tooth 6 (MUST-STAY-GREEN 计数钉, mutation 敏感): 文件级 begin_page 调用计数
     * 恒 5——restore 分支零接管调用（不推 SERVER_PAGE 不抢前台; 现状基线 2026-09-23
     * 逐行实证; 任何第 6 处 = restore 分支接管回潮或未列面被顺手改）。 */
    n = ksr_count_in_file(KSR_MCP, KSR_BEGIN_PAGE_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 5,
        ksr_fm("K7-A5 RED: mcp_server.cc kidpal_render_node_begin_page = %d 处（冻结"
               "基线恒 5）——restore 分支引入接管调用（只填缓存不接管前台语义破, "
               "恢复性推送变成确定性顶前台回潮）或未列面被改（须复核）", n));

    /* tooth 7 (MUST-STAY-GREEN 负 needle, mutation 敏感): 路由区 SetChatMessage 恒
     * 1——restore 解析失败降级静默（记日志, 不弹提示; 现状那 1 处 = 既有非 restore
     * 降级, 契约 L85「板端静默降级」）。 */
    n = ksr_count_in_slice(region, KSR_CHATMSG_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        ksr_fm("K7-A5 RED: schedule 路由区 SetChatMessage = %d 处（冻结基线恒 1）——"
               "restore 降级走了弹提示（板端静默降级语义破: ADR-005 明文区别于主动"
               "展示路径的 SetChatMessage 降级）", n));

    free(region);
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — K7-A5 三前台态谓词 + 缓存幂等形态：kidpal_render_node_schedule_on_screen
 * （home on_screen 逐字同构）+ cache setter 覆写形态。
 * MUST-STAY-GREEN 半：home on_screen 先例区 + cache 冻结形在世。
 * RED today：谓词缝未实现（全仓 0 命中, grep 实证）。
 * ------------------------------------------------------------------------ */
/* coverage: K7-A5 -- 在屏查询缝：schedule_on_screen 定义（.h 声明 + .cc 定义 + LOCAL_PAGE/SCHEDULE/top 谓词形态 = 三前台态口径, 接管态恒 0 仅填缓存）+ home on_screen 先例区在世 + cache setter memcpy 覆写形态（重复注入幂等的结构承载） */
TEST_CASE("课表网格冷启动恢复: 在屏谓词+缓存幂等形态 (K7-A5)",
          "[schedule_grid_restore][K7-A5][structural]")
{
    char *seam, *home_region, *cache_region;
    int n;

    KSR_REQUIRE_ROOT();

    /* tooth 1 (RED today): .h 声明在世。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_RN_H, KSR_ON_SCREEN_ND),
        "K7-A5 RED: kidpal_render_node.h 无 kidpal_render_node_schedule_on_screen "
        "声明——在屏查询缝未落（BUILDER DUTY 2)）");

    /* tooth 2 (RED today): .cc 定义区可切 + 谓词形态齐——LOCAL_PAGE 顶层判定
     * （接管态 -> 0, R10 括注）+ KP_PAGE_SCHEDULE 页判定 + layer_top 单一裁决
     * （home on_screen 先例逐字同构; 缺任一 = 谓词口径漂移）。 */
    seam = ksr_region_until_next_fn(KSR_RN_CC,
                                    "int kidpal_render_node_schedule_on_screen(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "K7-A5 RED: kidpal_render_node.cc 无 int kidpal_render_node_schedule_on_"
        "screen(void) 定义——在屏查询缝未实现（三前台态分流无判据, BUILDER DUTY 3)）");
    TEST_ASSERT_TRUE_MESSAGE(
        (n = ksr_count_in_slice(seam, "KIDPAL_LAYER_LOCAL_PAGE")) >= 1 &&
            ksr_contains(seam, "KP_PAGE_SCHEDULE") &&
            ksr_contains(seam, "kidpal_layer_top"),
        ksr_fm("K7-A5 RED: schedule_on_screen 谓词形态缺件（LOCAL_PAGE=%d/"
               "KP_PAGE_SCHEDULE/top）——必须沿 home on_screen 先例口径（layer_top "
               "== LOCAL_PAGE && s_local_page == KP_PAGE_SCHEDULE）", n));
    TEST_ASSERT_TRUE_MESSAGE(ksr_contains(seam, "== KIDPAL_LAYER_LOCAL_PAGE"),
        "K7-A5 RED: 谓词缺 == KIDPAL_LAYER_LOCAL_PAGE 判定——接管态（SERVER_PAGE "
        "顶层）会被误判 P2 在屏而重绘（R10: 接管态仅填缓存不重绘; ADR-005 括注）");
    free(seam);

    /* tooth 3 (MUST-STAY-GREEN, 先例参照系): home on_screen 函数区冻结形在世
     * （LOCAL_PAGE + KP_PAGE_HOME 谓词）——R10 谓词口径的机制参照; 被顺手改 =
     * 先例失真。 */
    home_region = ksr_region_until_next_fn(KSR_RN_CC,
                                           "int kidpal_render_node_home_on_screen(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(home_region,
        "K7-A5 RED(infra): home_on_screen 函数区切取失败——先例锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(
        ksr_count_in_slice(home_region, "KIDPAL_LAYER_LOCAL_PAGE") >= 1 &&
            ksr_contains(home_region, "KP_PAGE_HOME"),
        "K7-A5 RED: home_on_screen 先例谓词形态偏移——home 不抢前台判据被连带改"
        "（M3-K2 A5 冻结行为零回归义务）");
    free(home_region);

    /* tooth 4 (MUST-STAY-GREEN, 幂等形态): cache setter 覆写形态在世——memcpy
     * 覆写 + s_grid_n 重赋值（M3-K2 冻结形; 无 append/累加语法位 = 重复注入
     * 同数据缓存零偏移的结构承载; 字节级幂等行为归真机 K7-A8② 顺带核）。 */
    cache_region = ksr_region_until_next_fn(
        KSR_RN_CC,
        "void kidpal_render_node_cache_schedule_grid(const kidpal_schedule_course_t *");
    TEST_ASSERT_NOT_NULL_MESSAGE(cache_region,
        "K7-A5 RED(infra): cache_schedule_grid 函数区切取失败——缓存 setter 锚漂移");
    TEST_ASSERT_TRUE_MESSAGE(
        ksr_count_in_slice(cache_region, "memcpy") >= 1 &&
            ksr_count_in_slice(cache_region, "s_grid_n = copy") == 1,
        "K7-A5 RED: cache setter 覆写形态偏移（memcpy/s_grid_n = copy）——缓存写"
        "被改成累积/拼接形态则重复注入网格会漂移（memcmp 幂等破）");
    free(cache_region);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — K7-A6 语音接管回归守卫：薄载荷路径 begin_page + SERVER_PAGE 接管照旧
 * （M3-K3 冻结行为）。现状即绿（红基线通道② 唯一 must-stay TEST——回归守卫的
 * 设计态）; 变异敏感点见文件尾 MUTATION REPLAY M3。
 * ------------------------------------------------------------------------ */
/* coverage: K7-A6 -- 语音接管回归守卫：schedule 路由区 begin_page -> grid_render -> cache 调用序零偏移（薄载荷 = 主动展示接管前台）+ begin_page 函数区推 KIDPAL_LAYER_SERVER_PAGE 在世 + home 路由「不抢前台」先例区（home_on_screen + home_upcoming_apply）在世 */
TEST_CASE("课表网格冷启动恢复: 语音接管回归守卫 (K7-A6)",
          "[schedule_grid_restore][K7-A6][structural]")
{
    char *region, *begin_region, *home_region;
    const char *p_begin, *p_render, *p_cache;

    KSR_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 序钉): schedule 路由区薄载荷调用序 begin_page ->
     * grid_render -> cache（接管渲染再填缓存的既有序; restore 改造不得重排/
     * 拆薄载荷路径）。位序以 begin_page 调用点为起点链式后搜——区首 case 注释里
     * 合法提及 kidpal_schedule_grid_render（L608），裸 first-occurrence 比位次会
     * 被注释命中毒。 */
    region = ksr_region_between(KSR_MCP, KSR_SCHED_START, KSR_SCHED_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "K7-A6 RED(infra): schedule 路由区切取失败（TEST 3 tooth 1）");
    p_begin = strstr(region, KSR_BEGIN_PAGE_ND);
    p_render = (p_begin != NULL) ? strstr(p_begin, KSR_GRID_RENDER_ND) : NULL;
    p_cache = (p_render != NULL) ? strstr(p_render, KSR_CACHE_ND) : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_begin != NULL && p_render != NULL && p_cache != NULL,
        "K7-A6 RED: 薄载荷路径调用链缺件（begin_page/render/cache）——M3-K2 冻结"
        "实现被 restore 改造连带拆动（零回归义务）");
    TEST_ASSERT_TRUE_MESSAGE(p_begin < p_render && p_render < p_cache,
        "K7-A6 RED: 薄载荷路径调用序偏移（begin_page -> render -> cache）——"
        "M3-K3「帮我显示课表」接管前台的冻结实现序被重排（语音接管回归）");
    free(region);

    /* tooth 2 (MUST-STAY-GREEN): begin_page 函数区推 KIDPAL_LAYER_SERVER_PAGE 在世
     * ——SERVER_PAGE 接管机制的单一权威未失。 */
    begin_region = ksr_region_between(KSR_RN_CC,
                                      "void kidpal_render_node_begin_page(void)",
                                      "void kidpal_render_node_end_page(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(begin_region,
        "K7-A6 RED(infra): begin_page 函数区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(
        ksr_count_in_slice(begin_region, "KIDPAL_LAYER_SERVER_PAGE") >= 1,
        "K7-A6 RED: begin_page 区无 KIDPAL_LAYER_SERVER_PAGE 推入——服务端页接管"
        "机制被拆（薄载荷展开产物无法接管前台, M3-K3 冻结行为回归）");
    free(begin_region);

    /* tooth 3 (MUST-STAY-GREEN, home 先例区): home 路由「不抢前台」先例在世——
     * home_on_screen + home_upcoming_apply 消费（R10 谓词口径与 restore 在屏分流
     * 的机制参照系; K6 锚① home upcoming 零回归面）。 */
    home_region = ksr_region_between(KSR_MCP, KSR_HOME_START, KSR_HOME_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(home_region,
        "K7-A6 RED(infra): home 路由区切取失败——VRT_ROUTE_HOME/VRT_ROUTE_CONTROL "
        "锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(
        ksr_contains(home_region, "kidpal_render_node_home_on_screen") &&
            ksr_contains(home_region, "kidpal_home_upcoming_apply"),
        "K7-A6 RED: home 路由「不抢前台」先例区偏移（home_on_screen/"
        "home_upcoming_apply）——additive data 消费先例被连带拆动");
    free(home_region);
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — K7-A7 piggyback 形态锚（跨端 wire 容忍行为齿 + server 单源 face）。
 * 行为半: restore 键（true/false、先/后位序）解析 == 不带键同参（weak 哨兵在
 * host runner 被 schedule_page.c 强定义覆盖后真验; standalone 线束红在哨兵）。
 * 结构半: 对端 producer face 在世（schedule_push.py / front_app.py）。
 * RED today：对端 producer face 未实现（grep 实证）+ standalone 哨兵 -2。
 * ------------------------------------------------------------------------ */
/* coverage: K7-A7 -- restore wire 容忍（data 对象顶层 restore 键 true/false、先/后位序解析容忍, 逐字段保真）+ server 单源 producer face 结构锚（grid_restore_payload / GRID_RESTORE_ENV / front 引用）。三触发帧序/恰一次/开关门控行为齿归 server pytest A7 */
TEST_CASE("课表网格冷启动恢复: restore wire容忍+对端face (K7-A7)",
          "[schedule_grid_restore][K7-A7][behavioral]")
{
    static const char *const plain =
        "{\"week\": {\"courses\": [{\"day\": 0, \"slot\": 0, \"name\": \"语文\", \"flags\": 0}, "
        "{\"day\": 0, \"slot\": 1, \"name\": \"钢琴课\", \"flags\": 0}]}}";
    static const char *const restore_first =
        "{\"restore\": true, \"week\": {\"courses\": [{\"day\": 0, \"slot\": 0, \"name\": \"语文\", \"flags\": 0}, "
        "{\"day\": 0, \"slot\": 1, \"name\": \"钢琴课\", \"flags\": 0}]}}";
    static const char *const restore_last_false =
        "{\"week\": {\"courses\": [{\"day\": 0, \"slot\": 0, \"name\": \"语文\", \"flags\": 0}, "
        "{\"day\": 0, \"slot\": 1, \"name\": \"钢琴课\", \"flags\": 0}]}, \"restore\": false}";
    kidpal_schedule_course_t got[8];
    kidpal_schedule_course_t ref[8];
    int n_plain, n_first, n_last;

    KSR_REQUIRE_ROOT();

    /* tooth 1 (behavioral): 无键基线可解析且恰 2 条——wire 空白容忍（json.dumps
     * 默认 \": \" / \", \")随 canonical 载荷实测; -2 = weak 哨兵（standalone 线束
     * 无强符号, 设计态红）; 真 -1 = 解析面回归。 */
    n_plain = kidpal_schedule_grid_parse(plain, ref, 8);
    TEST_ASSERT_TRUE_MESSAGE(n_plain != KSR_UNIMPLEMENTED,
        "K7-A7 RED(sentinel): kidpal_schedule_grid_parse weak 哨兵未被强定义覆盖"
        "——standalone 线束设计态（本齿在 host runner 由 schedule_page.c 强定义"
        "覆盖后真验）; builder 接线后以通道① host runner 判真");
    TEST_ASSERT_TRUE_MESSAGE(n_plain == 2,
        ksr_fm("K7-A7 RED: 无键基线解析 = %d（期望 2）——解析面被 restore 改造连带"
               "偏移或 wire 空白容忍回潮", n_plain));

    /* tooth 2 (behavioral, 跨端锚核心): restore:true 前位序解析 == 无键同参
     * （count + 逐字段保真）——对端 grid restore 帧的真实形状。 */
    n_first = kidpal_schedule_grid_parse(restore_first, got, 8);
    TEST_ASSERT_TRUE_MESSAGE(n_first == n_plain,
        ksr_fm("K7-A7 RED: restore:true 前位序解析 = %d != 基线 %d——data 对象顶层 "
               "restore 键不被容忍（对端恢复性载荷整帧被 -1 静默降级, 冷启动恢复"
               "功能死在解析门, g-relay-fix 同型跨端断裂）", n_first, n_plain));
    TEST_ASSERT_TRUE_MESSAGE(n_first > 0 &&
                                 memcmp(got, ref, sizeof(ref[0]) * (size_t)n_first) == 0,
        "K7-A7 RED: restore:true 帧解析结果逐字段漂移——课程数据被 restore 键"
        "破坏（保真破）");

    /* tooth 3 (behavioral, R7 false 态): restore:false 后位序解析 == 无键同参
     * （absent/false 同判的设备侧承载——false 不得当未知畸形 -1）。 */
    n_last = kidpal_schedule_grid_parse(restore_last_false, got, 8);
    TEST_ASSERT_TRUE_MESSAGE(n_last == n_plain,
        ksr_fm("K7-A7 RED: restore:false 后位序解析 = %d != 基线 %d——false 态未"
               "同判（R7: 显式 false 设备行为必须与 absent 完全一致）", n_last,
               n_plain));

    /* tooth 4 (structural, 对端 face): server 单源 producer face 在世——
     * schedule_push.py grid_restore_payload 定义 + GRID_RESTORE_ENV 常量 +
     * front_app.py 引用（对端不落 = 本侧容忍齿空转, 跨端两半都要在世）。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_PUSH_PY, "def grid_restore_payload") &&
                                 ksr_file_contains(KSR_PUSH_PY, "GRID_RESTORE_ENV"),
        "K7-A7 RED: schedule_push.py 无 grid_restore_payload/GRID_RESTORE_ENV——"
        "对端 grid 变体单源 producer / ECS 开关决策面未落（frozen seam, 见 server "
        "pytest 文件头）");
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_FRONT_PY, "grid_restore"),
        "K7-A7 RED: front_app.py 无 grid_restore 引用——三触发附带 relay 未接线"
        "（restore 载荷永不下发, 设备侧容忍齿空转）");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — K7-E2 注册 + 既有齿零偏移 + 需求/契约 canary + 扫描器自检。
 * MUST-STAY-GREEN 半：既有齿注册在世 + canary（现状即绿）。
 * RED today：本验收件未接线 HOST_ACCEPTANCE_SRCS。
 * ------------------------------------------------------------------------ */
/* coverage: K7-E2 -- 本验收件接线 HOST_ACCEPTANCE_SRCS（smoke 末位序守卫）+ 既有齿注册在世（零偏移回归面）+ 需求面「恢复性推送」句与契约 data.restore canary（上游 565cb2d 落地锚, 本包零契约触碰）+ 扫描器自检 */
TEST_CASE("课表网格冷启动恢复: 注册+canary (K7-E2)",
          "[schedule_grid_restore][K7-E2][structural]")
{
    char *mk;

    KSR_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 扫描器自检): 已知文件可读 + 永不存在 needle 零命中。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_exists(KSR_MCP) && ksr_exists(KSR_SCHED_C) &&
                                 ksr_exists(KSR_HOST_CML) && ksr_exists(KSR_REQ_MD) &&
                                 ksr_exists(KSR_CONTRACT),
        "K7 RED(infra): scanner cannot see known-good files — 扫描器失灵");
    TEST_ASSERT_TRUE_MESSAGE(ksr_count_in_file(KSR_REQ_MD,
        "__k7_no_such_needle__") == 0,
        "K7 RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");

    /* tooth 2 (MUST-STAY-GREEN, 需求/契约 canary): 上游冻结锚在世——requirements
     * 恢复性推送句（landing L1）+ 契约 data.restore bullet（landing L7, commit
     * 565cb2d）; spec/契约冻结面零改（发现冲突走 spec-lane 逃生门）。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_REQ_MD, "恢复性推送") &&
                                 ksr_file_contains(KSR_REQ_MD, "S-F1.2"),
        "K7 RED: requirements 恢复性推送句 / S-F1.2 锚丢失——需求面被修订");
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_CONTRACT, "data.restore"),
        "K7 RED(infra): contract data.restore bullet 丢失——契约面被触碰（本包零"
        "契约触碰红线; 上游 565cb2d 冻结锚）");

    /* tooth 3 (MUST-STAY-GREEN, 对端解析面实现在世): schedule_page.c 强定义源
     * 在世（TEST 4 行为齿在 host runner 的强符号覆盖前提）。 */
    TEST_ASSERT_TRUE_MESSAGE(ksr_file_contains(KSR_SCHED_C,
                                               "kidpal_schedule_grid_parse"),
        "K7 RED(infra): schedule_page.c 解析面不在世——TEST 4 weak 哨兵将无强定义"
        "可覆盖（行为齿空转）");

    /* tooth 4 (RED today): 本验收件接线 HOST_ACCEPTANCE_SRCS，置于 smoke 之前。 */
    mk = ksr_read(KSR_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "K7 RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *acc = strstr(mk, "schedule_grid_restore.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(acc != NULL,
            "K7-E2 RED: schedule_grid_restore.acceptance.c 未接线进 "
            "HOST_ACCEPTANCE_SRCS — host 验收构建未接本包（BUILDER DUTY 4)）");
        TEST_ASSERT_TRUE_MESSAGE(acc != NULL && smoke != NULL && acc < smoke,
            "K7-E2 RED: schedule_grid_restore 注册行必须排在 "
            "host_infra_smoke.acceptance.c 之前（smoke 末位 = 最先执行, boundary "
            "齿注册序守卫）");
    }

    /* tooth 5 (MUST-STAY-GREEN): 既有齿注册在世——回归面零偏移（key_layer_dispatch /
     * schedule_grid / boot_auto_connect / p1_home_battery / time_sync / smoke）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(mk, "key_layer_dispatch.acceptance.c") != NULL &&
            strstr(mk, "schedule_grid.acceptance.c") != NULL &&
            strstr(mk, "boot_auto_connect.acceptance.c") != NULL &&
            strstr(mk, "p1_home_battery.acceptance.c") != NULL &&
            strstr(mk, "time_sync.acceptance.c") != NULL &&
            strstr(mk, "host_infra_smoke.acceptance.c") != NULL,
        "K7-E2 RED: 既有冻结齿注册行丢失——回归面被本包拆（K7-E1 既有齿零偏移义务）");
    free(mk);
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-23 实跑回填）:
 *   通道① 既有 host runner 全量实跑（本文件未接线前, 既有齿零偏移通道）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *       key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *     cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *       ./build/host_infra_test.elf
 *     实跑（2026-09-23）= build 过 + **98 Tests / 0 Failures 0 Ignored**（K6 记账
 *     92/0 + K6 六件 = 98, 既有面零偏移, 回归判据成立）。
 *   通道② 本文件新齿红通道（C 类冻结验收实编 + 实跑, DEV-GOVERNANCE 验收测试纪律
 *   §4/§3; 独立线束 = red-baseline/runner.c + unity stub + k7_red_wrapper.sh, 不链
 *   任何实现——结构齿扫真实工作树, TEST 4 行为齿红在 weak 哨兵）:
 *     bash firmware/main/test/acceptance/red-baseline/k7_red_wrapper.sh
 *     （等价展开: cd firmware/main/test/acceptance/red-baseline && gcc -std=c11
 *     -Wall -Wextra -I. -o /tmp/k7_red_runner
 *     ../../schedule_grid_restore.acceptance.c runner.c && /tmp/k7_red_runner;
 *     exit code = 失败 TEST_CASE 数）
 *     编译通道 error 为零（验收文件本身可编译, 验收测试纪律 §4）; 设计态红判读:
 *       - TEST 1 红 = tooth 3 起（mcp_server.cc "restore" 键读取 0 命中——frozen
 *         seam not implemented, 真实缺失; tooth 1/2 MUST-STAY-GREEN 先行实跑证明绿）;
 *       - TEST 2 红 = tooth 1 起（schedule_on_screen 全仓 0 命中; tooth 3/4
 *         MUST-STAY-GREEN 被 abort 屏蔽——解析性必绿齿, builder 实现后随通道①转绿）;
 *       - TEST 3 = **GREEN（设计态）**——K7-A6 纯回归守卫, 全齿 must-stay-green
 *         （现状即绿; 变异敏感性见 MUTATION REPLAY M3）;
 *       - TEST 4 红 = tooth 1 起（weak 哨兵 -2, standalone 无强符号; tooth 4 对端
 *         face 结构齿同为真实缺失——schedule_push.py/front_app.py 零 grid_restore,
 *         grep 实证）;
 *       - TEST 5 红 = tooth 4 起（本件未接线 HOST_ACCEPTANCE_SRCS; tooth 1-3/5
 *         MUST-STAY-GREEN 先行）。
 *     Unity abort-on-first-fail 语义: 每 TEST 首红 tooth 即 abort, 同 TEST 后续
 *     解析性必红齿不逐条打印（与 K6 同款判读口径）。
 *   通道② 实跑（2026-09-23, k7_red_wrapper.sh）: COMPILE_EXIT=0（-Wall -Wextra
 *     零 error 零 warning）; 实跑 = **5 Tests / 4 Failures**（RUN_EXIT=4）,
 *     逐 TEST 首 红 tooth 与预判一致: TEST 1 = tooth 3（"restore" 0 命中）、
 *     TEST 2 = tooth 1（.h 无声明）、TEST 3 = PASS（must-stay 设计态绿）、
 *     TEST 4 = tooth 1（weak 哨兵 -2）、TEST 5 = tooth 4（注册行缺失）。
 *
 *   MUTATION REPLAY MAP（K6 纪律: 五条 expectedRed 补跑手法逐字可执行; builder/
 *   收尾相位在「builder 实现已全绿」态执行——每条 = 变异编辑 + 通道②独立命令 +
 *   原始输出落 mutation-evidence/<条目>.txt + git checkout 还原）:
 *     M1 (K7-A5, taskpackage mutationEvidence#1) 删 restore 分支（恢复消费回潮
 *        接管: 分支加 kidpal_render_node_begin_page 或 SetChatMessage 降级、或摘
 *        lvgl_port_lock / on-screen 谓词消费 / "restore" 键读取）→ 期望 TEST 1 红
 *        （tooth 3/4/5 缺件 / tooth 6 计数 6 ≠ 5 / tooth 7 计数 2 ≠ 1）。
 *     M2 (K7-A5) 摘 kidpal_render_node_schedule_on_screen 或谓词改恒真（去
 *        == KIDPAL_LAYER_LOCAL_PAGE 判定 = 接管态误重绘）→ 期望 TEST 2 红
 *        （tooth 1/2 缺件 / tooth 2' == 判定缺）。
 *     M3 (K7-A6) 摘薄载荷路径 begin_page 调用或重排 begin_page->render->cache 序、
 *        或摘 begin_page 区 SERVER_PAGE 推入 → 期望 TEST 3 红（tooth 1 序破 /
 *        tooth 2 推入缺）; 该 TEST 基线绿, 红即真回归。
 *     M4 (K7-A7) schedule_page.c kspj_skip_value 摘 true/false literal 分支
 *        （restore 键值畸形 -> -1 = 对端载荷整帧拒收）→ 期望 TEST 4 红（tooth 2/3
 *        解析不等）; 或摘 server grid_restore_payload → TEST 4 tooth 4 红。
 *     M5 (K7-E2) 摘 HOST_ACCEPTANCE_SRCS 注册行 → 期望 TEST 5 红（tooth 4）。
 * --------------------------------------------------------------------------- */
