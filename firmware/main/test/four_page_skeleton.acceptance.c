/*
 * Frozen acceptance tests — TaskPackage 261104-k0a-four-page-skeleton (subsystem:
 * firmware; 四页骨架——P3 积分占位、P4 故事原地迁移、枚举驱动语境映射; rigor=full /
 * riskTier=high; mergeOrder 70; dependsOn 空; workType = new-capability + remediation)。
 *
 * Story 语义权威 = docs/requirements.md 现文：S-F6.1 AC4（数据未达空态半——本包只钉
 * 静态占位，不伪造 0 余额、不画进度线；数据已送达零分录态归 K0/K2）+ §2.3 按键手势
 * （:131 权威键位矩阵）+ §2.4 页面结构（:176 四页循环，本批 3→4）。裁定权威 =
 * delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md §3.1 D-01（四页循环
 * + 四点页点骨架）与 D-08（source_page 枚举驱动、废除数值 case 2）。契约形状权威 =
 * docs/contracts/interface-contract.md §四 kidpal_session_ctx 行（:347 内联闭集六值
 * home / schedule / points_home / points_ledger / story_cover / story；未知值拒绝并
 * 记日志、不得回退成 story——本包产出其中主页四值，points_ledger/story_cover 归
 * K2/K5 不提前）。本包故事仍是 P4 平级页（封皮/逐层返回 = D-01-cover landsIn K5，
 * points_ledger 语境 = D-02 landsIn K2——均不在此钉）。
 *
 * 边界（与相邻冻结件的分工，防双头）：
 *   - 四页循环/枚举/页点的**字面 4 单点钉死**住 retract_p4_three_page.acceptance.c
 *     test 3（2026-11-04 重锚后沿用其「字面单点」纪律）；本文件对 PAGE_COUNT 不取
 *     字面（同源判别跟随），字面重复会互相遮蔽。
 *   - 页槽表键帽**字面值**单源住 keybar_three_slot.acceptance.c（2026-11-04 重锚后
 *     P3=积分/P4=故事）；本文件只断言四页槽位存在性（rc==0 非空），不重抄字面。
 *   - 阅读态 KEY 短按零响应（TECH-261028 收紧）住 story_page_v2.acceptance.c
 *     SPV2-RK1（2026-11-04 additive）；本文件不重复其归属判据断言面。
 *   - story 三 story 行为零回归基线 = story_page_v2.acceptance.c 既有断言原样保留。
 *
 * device-outcome: K0A-E3（四页循环/四点页点/逐页 source_page/阅读键短按零响应的
 *   真机看屏半; launch-card deviceOutcome.userVisible=true 同源）
 * host-uncompilable-path: firmware/main/display/kidpal_render_node.cc（四页渲染
 *   权威 + P3 占位渲染分支——本文件 test 2/4 的结构齿为源锚代理, 看屏语义归
 *   K0A-E3 真机轮; mcp_server.cc / application.cc 消费面同族, 与 launch-card
 *   hostUncompilablePaths 申报一致）
 *
 * Nature: HOST-SIDE BEHAVIORAL（key_page_next / kidpal_keybar_dots / kidpal_keybar_
 * slots —— key_gesture_parser.cc 与 display/keybar.c 均已编入 host runner，真行为面，
 * 走 weak 哨兵 + 独立红跑通道编入真实现源）+ HOST-SIDE STRUCTURAL（设备专属面
 * kidpal_render_node.{h,cc} / story_page.c 的 source_page 映射区不进 host 独立编译
 * ——story 行为缝以 weak 哨兵接 host runner 强符号，映射区另加切区结构齿；树扫描
 * 形态照 key_layer_dispatch / retract_p4 先例：稳定签名锚切区，零行号钉点）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 真调
 * 已入 runner 的纯逻辑实现; 「home =」为冻结落点）:
 *
 *   [1] 键路由四页枚举（home = firmware/main/key_gesture_parser.h）:
 *       typedef enum { PAGE_P1 = 0, PAGE_P2, PAGE_P3, PAGE_P4, PAGE_COUNT } page_id_t;
 *         -- PAGE_P4 **复活**（D-01 四页; 字面值 3, PAGE_COUNT = 4——字面 4 单点在
 *            retract test 3 重锚齿）。
 *       key_page_next 步进 = (pg + 1) % PAGE_COUNT（同源不写死）；非法页回 PAGE_P1
 *         （防御语义零改动）。
 *
 *   [2] 渲染节点四页枚举（home = firmware/main/display/kidpal_render_node.h）:
 *       KP_PAGE_HOME = 0 / KP_PAGE_SCHEDULE = 1 / KP_PAGE_POINTS_HOME = 2 /
 *       KP_PAGE_STORY = 3 / KP_PAGE_COUNT = 4——与 [1] 逐项同序同值（P3 = 积分主页、
 *       P4 = 故事列表原地迁移；**不新增** KP_PAGE_STORY_COVER / KP_PAGE_POINTS_LEDGER
 *       ——K5 封皮语境与 K2 详细账本语境不得提前进枚举，越包红线）。
 *       头注页序 prose 同步改写（「三页循环」「home→schedule→story→home」零残留）。
 *
 *   [3] keybar 四点页点（home = firmware/main/display/keybar.c 纯逻辑单源，零新数字面）:
 *       kidpal_keybar_dots: overlay_top==0 -> visible=1 / total=PAGE_COUNT(=4) /
 *         active=page（当前页实心）；覆盖层隐藏不抹状态；page >= PAGE_COUNT -> -1。
 *       kidpal_keybar_slots 页槽表四行（值域字面住 keybar_three_slot 重锚齿）:
 *         P1 说话/翻页 · P2 问课程/翻页 · P3 问一问/下一页（S-F6.1 AC6 键帽行 +
 *         docs/ui/V9/points-home-nodata.png）· P4 说话·双击看/下移（原 P3 故事列表
 *         行原地迁移，docs/ui/V9/canvas-p3.png）。
 *
 *   [4] P3 积分占位数据未达口径（home = firmware/main/display/kidpal_render_node.{h,cc};
 *       S-F6.1-AC4 静态占位半 + decisionCoverage D-01-skeleton）:
 *       #define KP_POINTS_NODATA_HINT "积分数据还在路上，等一等再来看"
 *         -- 单源住 kidpal_render_node.h（逐字 = docs/ui/V9/points-home-nodata.png
 *            设计权威 + AC4 话术；K0 points_page.c 接管后由其继承消费，K0a 不预钉
 *            points_page.c 形状）。
 *       rn_render_kidpal_page_locked() 增 `case KP_PAGE_POINTS_HOME:`（四页渲染权威
 *         单一入口，cycle_next 翻到 P3 必经）：画页名「星星积分本」+ 居中等待话术
 *         （消费 [4] 单源宏，不重抄字面）。**负锚**：占位分支不显示任何数字
 *         （零 "%d" 数字格式化——不伪造 0 余额）、不画进度线（零 lv_bar/lv_arc/
 *         lv_slider 控件）、不借用故事空态宏（零 KSTORY_EMPTY_HINT*——两页话术
 *         各归各单源，S-F6.1-AC4 与 S-F2.1-AC5/AC6 话术可区分）。
 *
 *   [5] source_page 枚举驱动映射（home = firmware/main/display/story_page.c
 *       kidpal_story_ctx_source_page 重写; 契约 §四 :347 闭集 + decisionCoverage
 *       D-08-context; 废除数值 case 2——TECH-261028 同族「映射散落/数值耦合」根因面）:
 *       const char *kidpal_story_ctx_source_page(int page);
 *         -- switch 按 KP_PAGE_* **符号**分流（映射区出现 KP_PAGE_HOME/SCHEDULE/
 *            POINTS_HOME/STORY 符号 = 枚举驱动结构锚；新增页改枚举不再漏改映射）:
 *            KP_PAGE_HOME -> "home"; KP_PAGE_SCHEDULE -> "schedule";
 *            KP_PAGE_POINTS_HOME -> "points_home"（**新值**——P3 积分主页 = 普通问答
 *            语境, 契约 §四「三者均为普通问答语境，不得误切记录语境」）;
 *            KP_PAGE_STORY -> "story"（记录语境——原行为保留）。
 *         -- 其余（负值/未知/KP_PAGE_COUNT）-> "home"（闭集缺省兜底沿用——绝不外溢
 *            新取值、**绝不回退成 story**；对端 front 未知值拒绝并记日志，本侧缺省
 *            home 与其对齐为「普通会话」）。返回静态串字面指针, 永不 NULL。
 *       wire 相容（对端实读 = server/gateway/front/front_app.py _consume_kidpal_control
 *       :629-676）：对端只对 source_page=="story" 切累积态、其余取值整条消费无行为
 *       （闭集预留）——本包新增 "points_home" 字符串值对端零改动即相容（普通会话）；
 *       wire 形状零改动（仍是 {"type":"kidpal_session_ctx","source_page":"<串>"}）。
 *
 *   [6] Builder FIRST ACTION（本文件进 runner 的唯一前置）: 向
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *       追加一行（相对 host_app/main）:
 *         "../../four_page_skeleton.acceptance.c"
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行，
 *       包 0 boundary 注册序守卫；本文件 K0A-C1 tooth 1 校验此序）。被测实现
 *       key_gesture_parser.cc（test A1/A2 真行为源）与 display/keybar.c（test A3 真
 *       行为源，+ 其链接依赖 display/kidpal_voice_states.c）**已在册**；story 侧
 *       行为缝（kidpal_story_ctx_source_page）强符号源 display/story_page.c 已在册
 *       ——本包零新增被测实现接线（+1/-0）。
 *
 *   [7] mutation 自证（mutationRequired=true）：变异命名不得保留被测符号原字面为
 *       子串（KR-A3 教训；perAcceptance 草案见 acceptance-author 交付报告，实跑归
 *       builder/verifier 相位）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K0A-A1 四页 BOOT 循环行为齿 -> test 1（key_page_next P1→P2→P3→P4→P1
 *             回绕逐边 + PAGE_COUNT 步回原点同源环 + 非法页/越界回 P1 错误路径）
 *             【回引: §2.4 四页循环图逐边 + decisionCoverage D-01-skeleton】
 *   coverage: K0A-A2 枚举重锚齿 -> test 2（两枚举四页符号在世 + PAGE_P4 复活结构锚
 *             + KP_PAGE_POINTS_HOME=2/STORY=3 序锚 + K2/K5 语境值不提前负锚 + 头注
 *             三页 prose 退役）【回引: §2.4 + §2.3 对账② + D-01/D-08】
 *   coverage: K0A-A3 四点页点齿 -> test 3（四页各自 dots visible/total 同源/active +
 *             第 4 页 dots/slots 存在性 + 覆盖层隐藏不抹 + 越界防御）【回引:
 *             §2.2 comp-keybar「中 4 页点 / 页点随之 3→4」+ D-01】
 *   coverage: K0A-A4 P3 静态占位数据未达齿 -> test 4（话术单源宏 + 渲染 case 消费 +
 *             页名 + 零数字/零进度线/零故事宏借用负锚）【回引: S-F6.1-AC4 +
 *             decisionCoverage D-01-skeleton「不伪造 0 余额或进度线」】
 *   coverage: K0A-A5 source_page 枚举驱动映射齿 -> test 5（四值行为决策表 + 未知值
 *             缺省 home 永不 story + 映射区 KP_PAGE_* 符号结构锚）【回引: 契约 §四
 *             :347 闭集 + D-08 + S-F2.2-AC1 回归半】
 *   coverage: K0A-A6 P3 不切记录语境防回归齿 -> test 6（points_home 语境专属负锚 +
 *             K2/K5 子页语境值不产出 + 闭集值域守卫）【回引: 契约 §四 :347「P3 积分
 *             主页……普通问答语境，不得误切记录语境」+ D-08 核心】
 *   coverage: K0A-C1 注册序守卫 + 需求/契约 canary + 扫描器自检 -> test 7
 *   （story 三 story 零回归与阅读键归属 = story_page_v2.acceptance.c 承载（零回归
 *    基线原样保留 + SPV2-RK1 additive），非本文件断言面——见本头注「边界」节。）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [6]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m2/
 *      firmware/main/test/host_app/espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash -c "cd /mnt/c/AI/KidPal-pod-m2/
 *      firmware/main/test/host_app && ./build/host_infra_test.elf"
 *      （elf 相对 host_app/ 目录，从仓根跑 ./build/... 必报 No such file）
 *   Freeze-phase（本文件未接线前，DEV-GOVERNANCE 验收测试纪律 §3/§4 standalone 实编
 *   实跑；story 行为缝 weak 哨兵在独立通道恒哨兵红——其真行为红由 host runner 强符号
 *   通道 + 映射区结构齿解析性红定性联合承载, 判读见文件尾）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     g++ -c -o /tmp/k0a/kgp.o ../../../key_gesture_parser.cc
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/k0a/k0a_elf \
 *         ../../four_page_skeleton.acceptance.c ../../../display/keybar.c \
 *         ../../../display/kidpal_voice_states.c /tmp/k0a/kgp.o runner.c
 *     cd <仓根> && /tmp/k0a/k0a_elf
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

/* 真行为面（key_gesture_parser.cc / display/keybar.c 均已在 HOST_ACCEPTANCE_SRCS
 * 在册；独立红跑通道按 Run protocol 显式编入同批真实现源——强符号覆盖本文件 weak
 * 哨兵；若在册实现被误摘，链接期即响亮失败，非假绿）。 */
#include "../key_gesture_parser.h"

/* 层值（KIDPAL_LAYER_*）与故事子态（KSTORY_PAGE_*）在世冻结闭集——纯声明头,
 * host 可编（keybar.h 消费链同款; 本文件只读常量, 不消费 LVGL 面）。 */
#include "../display/kidpal_layer.h"
#include "../display/story_page.h"

#define K0A_MAX_PATH 1024
#define K0A_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根；行号快照非长期事实源, 齿锚全部用符号锚）。 */
#define K0A_KEY_H     "firmware/main/key_gesture_parser.h"
#define K0A_RN_H      "firmware/main/display/kidpal_render_node.h"
#define K0A_RN_CC     "firmware/main/display/kidpal_render_node.cc"
#define K0A_STORY_C   "firmware/main/display/story_page.c"
#define K0A_CMAKELISTS "firmware/main/test/host_app/main/CMakeLists.txt"
#define K0A_REQUIREMENTS "docs/requirements.md"
#define K0A_CONTRACT  "docs/contracts/interface-contract.md"

/* -------------------------------------------------------------------------- *
 * 页枚举数值镜像（RSL/RSR 先例形态）: KP_PAGE_* 四值镜像 = retract_p4_three_page.
 * acceptance.c test 3（2026-11-04 重锚后）冻结等值; kidpal_render_node.h 枚举漂移时
 * 重锚齿先红、本文件行为齿随之红。本文件不 include kidpal_render_node.h——其枚举
 * 在 builder 落地 KP_PAGE_POINTS_HOME 前不含该成员, include 后引用镜像成员不可编
 * （C 类冻结验收实编纪律 = 本文件在冻结相位必须可编译）。字面 4 的单点钉死在
 * retract test 3, 本镜像数值与该齿联动（两处同值, 漂移 = retract 先红）。
 * ------------------------------------------------------------------------ */
#define K0A_KP_PAGE_HOME       0
#define K0A_KP_PAGE_SCHEDULE   1
#define K0A_KP_PAGE_POINTS_HOME 2
#define K0A_KP_PAGE_STORY      3
/* PAGE_P4 镜像（同上先例; PAGE_P4 符号在 builder 落地 [1] 前不存在, 镜像消费,
 * 漂移由 retract test 3 重锚齿先红）。 */
#define K0A_PAGE_P4            3

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖; 独立红跑通道无 story
 * 侧实现 -> 恒哨兵红 = 解析性红定性由结构齿 + host runner 通道补齐, 见头注）。
 * kidpal_keybar_dots / kidpal_keybar_slots / kidpal_keybar_story_slots 强定义 home =
 * display/keybar.c（已在册）; kidpal_story_ctx_source_page 强定义 home =
 * display/story_page.c（已在册）。
 * ------------------------------------------------------------------------ */
#define K0A_UNIMPLEMENTED (-2)

typedef struct {
    int visible; /* 1 = 页点显示（仅主页面基层）; 0 = 覆盖层隐藏 */
    int total;   /* 页点总数 = 现行页循环页数（同源 PAGE_COUNT） */
    int active;  /* 当前实心页点下标（0-based） */
} k0a_keybar_dots_t;

__attribute__((weak)) int kidpal_keybar_dots(int page, int overlay_top,
                                             k0a_keybar_dots_t *out)
{
    (void)page; (void)overlay_top; (void)out;
    return K0A_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_keybar_slots(int page, int scene, int overlay_top,
                                              const char **out_left,
                                              const char **out_right)
{
    (void)page; (void)scene; (void)overlay_top; (void)out_left; (void)out_right;
    return K0A_UNIMPLEMENTED;
}
__attribute__((weak)) const char *kidpal_story_ctx_source_page(int page)
{ (void)page; return NULL; }

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿, key_layer_dispatch / retract_p4 先例）。
 * 全部扫描跑在真实工作树上, root 以手持 cwd 定位（从仓根或 firmware/main/test/host_app
 * 两处都可跑）。test / host_app / build / .git / assets 目录排除——test 目录里住着
 * 本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char k0a_root[K0A_MAX_PATH];
static int k0a_root_ready;

static int k0a_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int k0a_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int k0a_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    size_t i;
    for (i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int k0a_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[K0A_MAX_PATH];
    size_t i;

    if (k0a_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (k0a_is_regular(probe)) {
            snprintf(k0a_root, sizeof(k0a_root), "%s", cands[i]);
            k0a_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int k0a_rel(char *out, size_t cap, const char *rel)
{
    if (!k0a_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", k0a_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", k0a_root, rel);
    return 1;
}

static int k0a_exists(const char *rel)
{
    char p[K0A_MAX_PATH];
    return k0a_rel(p, sizeof(p), rel) && k0a_is_regular(p);
}

static char *k0a_read(const char *rel, size_t *out_len)
{
    char p[K0A_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!k0a_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > K0A_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int k0a_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int k0a_file_contains(const char *rel, const char *needle)
{
    char *buf = k0a_read(rel, NULL);
    int r;
    if (buf == NULL) {
        return 0;
    }
    r = k0a_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int k0a_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = k0a_read(rel, NULL);
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

static int k0a_count_files_probe(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[K0A_MAX_PATH];
        char rel_next[K0A_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            k0a_is_skipped_dir(e->d_name)) {
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
            count += k0a_count_files_probe(abs_next, rel_next, needle, first_hit,
                                           cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > K0A_MAX_READ) {
                continue;
            }
            {
                char *buf = k0a_read(rel_next, NULL);
                if (buf != NULL) {
                    if (k0a_contains(buf, needle)) {
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

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent). Used for function-body region pins
 * （rn_render_kidpal_page_locked 四页 case 区 / source_page 映射区）——两个 needle
 * 都取稳定签名行, 禁行号钉点。 */
static char *k0a_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = k0a_read(rel, NULL);
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
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int k0a_count_in_slice(const char *slice, const char *needle)
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

#define K0A_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(k0a_locate_root(), \
    "K0A RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/RSL 先例）。 */
static char k0a_fm_buf[1024];
static const char *k0a_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *k0a_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(k0a_fm_buf, sizeof(k0a_fm_buf), fmt, ap);
    va_end(ap);
    return k0a_fm_buf;
}

/* 专用红因消息（预格式化进静态缓冲再传单串断言宏, K0A-A1/A2 各齿）。 */
static const char *rtp_k0a_fm_page3(void)
{
    return k0a_fm("K0A-A1 RED: key_page_next(P3) = %d 期望 PAGE_P4(3)——P3 积分主页"
                  "的下一跳未落第 4 页停靠位（四页骨架缺失; requirements §2.4 "
                  "「BOOT 单击 → 切下一页（4 页循环）」P3 积分→P4 故事边）",
                  (int)key_page_next(PAGE_P3));
}
static const char *rtp_k0a_fm_page4(void)
{
    return k0a_fm("K0A-A1 RED: key_page_next(P4) = %d 期望回绕 PAGE_P1(0)——四页环"
                  "未闭合（现状 PAGE_COUNT=%d, 第 4 页停靠位缺失 = 真实缺失行为）",
                  (int)key_page_next((page_id_t)K0A_PAGE_P4), (int)PAGE_COUNT);
}
static const char *rtp_k0a_fm_four_step(int landed)
{
    return k0a_fm("K0A-A1 RED: 首页起连翻 4 步落在页 %d 期望回 P1(0)——四步走不完"
                  "一轮循环（现循环步数 = %d, 非 4; §2.4 四页循环图逐字对账）",
                  landed, (int)PAGE_COUNT);
}
static const char *rtp_k0a_fm_count_step(int count, int landed)
{
    return k0a_fm("K0A-A1 RED(infra): 走 PAGE_COUNT(%d) 步未回原点（落在 %d）——"
                  "步进基数与页数不同源（key_page_next 必须 (pg+1) %% PAGE_COUNT）",
                  count, landed);
}
static const char *rtp_k0a_fm_invalid(void)
{
    return k0a_fm("K0A-A1 RED: 非法页未回 PAGE_P1（PAGE_COUNT=%d -> %d, +1 -> %d, "
                  "99 -> %d, -1 -> %d）——越界防御面被四页化改动",
                  (int)PAGE_COUNT, (int)key_page_next((page_id_t)PAGE_COUNT),
                  (int)key_page_next((page_id_t)(PAGE_COUNT + 1)),
                  (int)key_page_next((page_id_t)99),
                  (int)key_page_next((page_id_t)-1));
}
static const char *rtp_k0a_fm_prose1(int n)
{
    return k0a_fm("K0A-A2 RED(drift): kidpal_render_node.h 仍用「home→schedule→"
                  "story→home」描述翻页序 %d 处——枚举已改（或将改）四页而页序自述"
                  "未理顺，§2.3 对账② 的漂移在文档面复发", n);
}
static const char *rtp_k0a_fm_prose2(int n)
{
    return k0a_fm("K0A-A2 RED(drift): kidpal_render_node.h 仍自称「三页循环」%d 处"
                  "——页集已批 3→4（requirements §2.4）而头注自述未随动（prose 与"
                  "实际枚举冲突 = 漂移定义, 留它 = 留坑）", n);
}

/* 扫描器自检辅助：以不可能存在的 needle 扫 firmware/main 全树（防负向齿假绿）。 */
static int k0a_scan_probe(char *first_hit, size_t cap)
{
    char abs[K0A_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!k0a_rel(abs, sizeof(abs), "firmware/main") || !k0a_is_dir(abs)) {
        return -1;
    }
    return k0a_count_files_probe(abs, "firmware/main", "__k0a_no_such_needle__",
                                 first_hit, cap, 0);
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — K0A-A1 四页 BOOT 循环行为齿（behavioral, 真实现源 key_gesture_parser.cc
 * 已在册; seam [1]）。RED today: PAGE_COUNT=3, key_page_next(P3) 回绕 P1（第 4 页
 * 停靠位缺失）——P4 积分后的故事停靠与回绕是本包核心缺失行为。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A1 -- 四页循环行为半：key_page_next P1→P2→P3→P4→P1 回绕（spec §2.4 四页循环图逐边）+ PAGE_COUNT 步回原点同源环 + 非法页/越界一律回 P1（防御语义零改动） */
TEST_CASE("四页骨架: key_page_next 四页回绕——P3→P4→P1 + 非法页防御 (K0A-A1)",
          "[four_page_skeleton][K0A-A1][behavior]")
{
    int p, i;

    /* tooth 1 (must-stay green): 前两跳 = P1→P2→P3, 三页时代路径零回归。 */
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P1) == (int)PAGE_P2,
        "K0A-A1 RED(guard): key_page_next(P1) != PAGE_P2——首页→课表第一跳被本包"
        "误伤（requirements §2.4 页序）");
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P2) == (int)PAGE_P3,
        "K0A-A1 RED(guard): key_page_next(P2) != PAGE_P3——课表→积分第二跳被本包"
        "误伤（requirements §2.4 四页序 P1→P2→P3→P4）");

    /* tooth 2 (本包红齿): 第三跳 = P3 积分 → P4 故事（新停靠页入循环; PAGE_P4
     * 以镜像消费——符号在 builder 落地 [1] 前不存在, 漂移由 retract 重锚齿先红）。 */
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P3) == K0A_PAGE_P4,
        rtp_k0a_fm_page3());

    /* tooth 3 (本包红齿): 第四跳回绕 P1（四页环闭合; 现状 PAGE_COUNT=3, P3 直回
     * P1 = 第 4 页停靠位缺失的真实行为红）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_page_next((page_id_t)K0A_PAGE_P4) == (int)PAGE_P1,
        rtp_k0a_fm_page4());

    /* tooth 4: 从首页起按四步 = 回到首页（循环停靠页恰四页, 与 §2.4 图逐字对账）。 */
    p = (int)PAGE_P1;
    for (i = 0; i < 4; i++) {
        p = (int)key_page_next((page_id_t)p);
    }
    TEST_ASSERT_TRUE_MESSAGE(p == (int)PAGE_P1,
        rtp_k0a_fm_four_step(p));

    /* tooth 5 (同源判别, must-stay green): PAGE_COUNT 步必回原点——基数与页数同源,
     * 本齿自动跟随页集（字面 4 的唯一钉点在 retract test 3 重锚齿）。 */
    p = (int)PAGE_P1;
    for (i = 0; i < (int)PAGE_COUNT; i++) {
        p = (int)key_page_next((page_id_t)p);
    }
    TEST_ASSERT_TRUE_MESSAGE(p == (int)PAGE_P1,
        rtp_k0a_fm_count_step((int)PAGE_COUNT, p));

    /* tooth 6 (错误/边界路径): 非法页一律回 P1, 防御语义零改动（四页化不得把越界
     * 变成未定义行为, 也不得把越界索引当成合法页继续步进）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_page_next((page_id_t)PAGE_COUNT) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)(PAGE_COUNT + 1)) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)99) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)-1) == (int)PAGE_P1,
        rtp_k0a_fm_invalid());
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — K0A-A2 枚举重锚齿（结构半, 树扫两 header）。PAGE_P4 复活 + 四页符号
 * 在世 + K2/K5 语境值不提前 + 三页 prose 退役。
 * RED today: 两 header 无 POINTS_HOME/PAGE_P4 三页 prose 在世。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A2 -- 枚举重锚结构齿：key_gesture_parser.h 枚举区 PAGE_P4 复活在世 + kidpal_render_node.h 枚举区 KP_PAGE_POINTS_HOME 在世且序在 SCHEDULE 与 STORY 之间 + KP_PAGE_STORY 符号在世 + K2/K5 语境值（STORY_COVER/POINTS_LEDGER）不进枚举负锚 + KP_PAGE_CHAR 退役零残留保持 + 头注「三页循环」prose 退役 */
TEST_CASE("四页骨架: 两页枚举四页重锚——PAGE_P4 复活+POINTS_HOME 入列+语境值不提前 (K0A-A2)",
          "[four_page_skeleton][K0A-A2][structural]")
{
    char *kg_enum, *rn_enum;

    K0A_REQUIRE_ROOT();

    /* tooth 1: key_gesture_parser.h 枚举区 PAGE_P4 复活（2026-09-18 撤页齿「PAGE_P4
     * 零残留」随 D-01 四页批准翻正为正向在世锚——见 retract REVISION LOG 2026-11-04;
     * 数值 3 的字面钉点在该文件 test 3, 本齿只钉符号在世）。 */
    kg_enum = k0a_region_between(K0A_KEY_H, "typedef enum {", "} page_id_t;");
    TEST_ASSERT_NOT_NULL_MESSAGE(kg_enum,
        "K0A-A2 RED(infra): key_gesture_parser.h 页枚举区切取失败（typedef enum / "
        "page_id_t 锚漂移——须复核本齿锚, 不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(k0a_contains(kg_enum, "PAGE_P4"),
        "K0A-A2 RED: key_gesture_parser.h 页枚举区零 PAGE_P4——第 4 页位未复活 "
        "（D-01 四页骨架; requirements §2.4「页面结构（4 页循环，本批 3→4）」）");
    TEST_ASSERT_TRUE_MESSAGE(k0a_contains(kg_enum, "PAGE_P1") &&
                                 k0a_contains(kg_enum, "PAGE_P2") &&
                                 k0a_contains(kg_enum, "PAGE_P3") &&
                                 k0a_contains(kg_enum, "PAGE_COUNT"),
        "K0A-A2 RED(guard): key_gesture_parser.h 页枚举既有成员缺失——四页化不得"
        "连带拆掉三页枚举（减法面不是重建面）");
    free(kg_enum);

    /* tooth 2: kidpal_render_node.h 枚举区 KP_PAGE_POINTS_HOME 在世, 且序在
     * KP_PAGE_SCHEDULE 与 KP_PAGE_STORY 之间（P2 课表→P3 积分→P4 故事 = §2.4 四页
     * 循环序; 数值 2/3 的字面钉点在 retract test 3 重锚齿, 本齿钉符号与序）。 */
    rn_enum = k0a_region_between(K0A_RN_H, "typedef enum {", "} kidpal_page_id_t;");
    TEST_ASSERT_NOT_NULL_MESSAGE(rn_enum,
        "K0A-A2 RED(infra): kidpal_render_node.h 页枚举区切取失败（锚漂移——须复核"
        "本齿锚）");
    {
        const char *p_sched = strstr(rn_enum, "KP_PAGE_SCHEDULE");
        const char *p_pts = strstr(rn_enum, "KP_PAGE_POINTS_HOME");
        const char *p_story = strstr(rn_enum, "KP_PAGE_STORY");
        TEST_ASSERT_TRUE_MESSAGE(p_pts != NULL,
            "K0A-A2 RED: kidpal_render_node.h 页枚举区零 KP_PAGE_POINTS_HOME——"
            "P3 积分主页未入渲染页枚举（D-01; §2.4 四页循环图 P3 = 星星积分本）");
        TEST_ASSERT_TRUE_MESSAGE(p_sched != NULL && p_story != NULL,
            "K0A-A2 RED(guard): kidpal_render_node.h 既有 KP_PAGE_SCHEDULE/STORY "
            "缺失——四页化不得连带拆掉既有页枚举");
        TEST_ASSERT_TRUE_MESSAGE(
            p_sched != NULL && p_pts != NULL && p_story != NULL &&
                p_sched < p_pts && p_pts < p_story,
            "K0A-A2 RED: KP_PAGE_POINTS_HOME 枚举序错位——积分主页必须夹在课表与"
            "故事之间（§2.4 P1→P2→P3 积分→P4 故事; 序错 = BOOT 翻页手感与页点实心位"
            "全错）");
    }
    /* tooth 3 (负锚 · K2/K5 越包红线): 子页语境值不得提前进页枚举——points_ledger
     * （K2 详细账本）与 story_cover（K5 封皮）不是主页面, 进枚举 = 提前钉 K2/K5
     * scope（TaskPackage decisionCoverage D-02/D-01-cover landsIn 明示归属）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_slice(rn_enum, "POINTS_LEDGER") == 0 &&
                                 k0a_count_in_slice(rn_enum, "STORY_COVER") == 0,
        "K0A-A2 RED(负锚): 页枚举出现 POINTS_LEDGER/STORY_COVER——K2/K5 子页语境"
        "提前入主页面枚举（越包; source_page 子页扩展归 K2/K5, TaskPackage "
        "crossPackageBehaviors 承接条在案）");
    /* tooth 4 (must-stay green): KP_PAGE_CHAR 退役零残留保持（2026-09-18 撤页事实
     * 不随四页化回潮——查字仍是窗, 不是常驻页）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_slice(rn_enum, "KP_PAGE_CHAR") == 0,
        "K0A-A2 RED: kidpal_render_node.h 页枚举区出现 KP_PAGE_CHAR——已撤查字"
        "常驻页位回潮（M2-K2 撤页事实; 查字窗语义归 char_window 面不变）");
    TEST_ASSERT_TRUE_MESSAGE(k0a_contains(rn_enum, "KP_PAGE_COUNT"),
        "K0A-A2 RED(guard): kidpal_render_node.h 枚举区零 KP_PAGE_COUNT——页数"
        "基数丢失（keybar 页点与循环步进的同源取数点）");
    free(rn_enum);

    /* tooth 5 (prose 退役): kidpal_render_node.h 头注三页 prose 零残留——枚举改四页
     * 而页序自述不改 = §2.3 对账②「prose 与实际枚举冲突」漂移在文档面复发
     * （retract test 3 tooth 8 同款纪律, needle 换三页时代的两种自述形态）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_file(K0A_RN_H, "home→schedule→story→home") == 0,
        rtp_k0a_fm_prose1(k0a_count_in_file(K0A_RN_H, "home→schedule→story→home")));
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_file(K0A_RN_H, "三页循环") == 0,
        rtp_k0a_fm_prose2(k0a_count_in_file(K0A_RN_H, "三页循环")));
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — K0A-A3 四点页点齿（behavioral, 真实现源 display/keybar.c 已在册; seam
 * [3]）。页点值域字面住 keybar_three_slot 重锚齿（头注「边界」节）; 本齿钉四页
 * dots 机制值 + 第 4 页槽位存在性（值不重抄）。
 * RED today: dots(3) = -1（第 4 页位不可渲染页点）。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A3 -- 四点页点行为齿：四页各自 base visible=1/total 同源 PAGE_COUNT/active=当前页 + 第 4 页（积分后故事页）dots/slots 存在性（值域字面单源住 keybar_three_slot, 本齿不重抄）+ 覆盖层隐藏不抹状态 + 越界防御零改动 */
TEST_CASE("四页骨架: 键帽条页点四点——第 4 页入列+覆盖层隐藏+防御 (K0A-A3)",
          "[four_page_skeleton][K0A-A3][behavior]")
{
    k0a_keybar_dots_t d;
    const char *left = NULL, *right = NULL;
    int page, rc;

    /* tooth 1: 既有三页基层列 must-stay（visible/total 同源/active 零回归——页集
     * 扩到四页不得动既有页的页点机制）。 */
    for (page = 0; page < 3; page++) {
        memset(&d, 0, sizeof(d));
        rc = kidpal_keybar_dots(page, 0, &d);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0 && d.visible == 1 &&
                                     d.total == (int)PAGE_COUNT && d.active == page,
            k0a_fm("K0A-A3 RED(guard): dots(page=%d, base) = (rc=%d, visible=%d, "
                   "total=%d, active=%d)——既有页页点机制被四页化破坏（§2.2 "
                   "comp-keybar; total 必须同源 PAGE_COUNT=%d）",
                   page, rc, d.visible, d.total, d.active, (int)PAGE_COUNT));
    }

    /* tooth 2 (本包红齿): 第 4 页（积分后的故事页）页点入列——visible/total 同源/
     * active=3。现状 dots(3) = -1（越界防御列）= 真实缺失行为。 */
    memset(&d, 0, sizeof(d));
    rc = kidpal_keybar_dots(K0A_KP_PAGE_STORY, 0, &d);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && d.visible == 1 &&
                                 d.total == (int)PAGE_COUNT &&
                                 d.active == K0A_KP_PAGE_STORY,
        k0a_fm("K0A-A3 RED: dots(page=3, base) = (rc=%d, visible=%d, total=%d, "
               "active=%d) 期望 (0, 1, PAGE_COUNT, 3)——第 4 页位仍落越界防御列, "
               "页点未随页循环 3→4（requirements §2.2「中 4 页点 · 页点随之 3→4」; "
               "PAGE_COUNT=%d）",
               rc, d.visible, d.total, d.active, (int)PAGE_COUNT));

    /* tooth 3: 第 4 页槽位存在性（左右槽非 NULL——键帽字面值单源住 keybar_three_slot
     * 重锚齿, 本齿不重抄, 防双头漂移）。现状 slots(3) = -1。 */
    left = NULL;
    right = NULL;
    rc = kidpal_keybar_slots(K0A_KP_PAGE_STORY, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL,
        k0a_fm("K0A-A3 RED: kidpal_keybar_slots(page=3, base) rc=%d left=%s right=%s"
               " 期望 (0, 非空, 非空)——第 4 页（故事列表）页槽行缺失（keybar 页槽表"
               "未随四页化加行; 值域权威 = keybar_three_slot KKB-A3 重锚齿）",
               rc, left ? left : "(null)", right ? right : "(null)"));
    left = NULL;
    right = NULL;
    rc = kidpal_keybar_slots(K0A_KP_PAGE_POINTS_HOME, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL,
        k0a_fm("K0A-A3 RED: kidpal_keybar_slots(page=2, base) rc=%d——P3 积分主页"
               "页槽行缺失（新页点/槽位不齐 = 翻到 P3 键帽条空槽; 值域权威 = "
               "keybar_three_slot KKB-A3 重锚齿）", rc));

    /* tooth 4 (must-stay green): 覆盖层隐藏不抹状态（四页化零触碰该语义）。 */
    memset(&d, 0, sizeof(d));
    rc = kidpal_keybar_dots(K0A_KP_PAGE_POINTS_HOME, 1, &d);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && d.visible == 0 &&
                                 d.total == (int)PAGE_COUNT &&
                                 d.active == K0A_KP_PAGE_POINTS_HOME,
        k0a_fm("K0A-A3 RED(guard): 覆盖层列 dots(page=2, overlay) = (rc=%d, %d, %d, "
               "%d) 期望 (0, 隐藏, total==PAGE_COUNT, active==2)——覆盖层不显示页点"
               "或隐藏不抹机制状态被四页化改动（§2.2）",
               rc, d.visible, d.total, d.active));

    /* tooth 5 (边界, must-stay green): 越界防御零改动——PAGE_COUNT 索引（四页态下
     * = 4）与负值一律 -1（dots/slots 两径; 撤页时代口径平移, 不钉死字面索引）。 */
    memset(&d, 0, sizeof(d));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_dots((int)PAGE_COUNT, 0, &d) == -1,
        "K0A-A3 RED: dots(page=PAGE_COUNT) 未防御（-1）——页集扩到四页不得把越界"
        "索引当合法页渲染页点");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_dots(-1, 0, &d) == -1,
        "K0A-A3 RED: dots(page=-1) 未防御（-1）");
    left = NULL;
    right = NULL;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_slots((int)PAGE_COUNT, 0, 0, &left,
                                                 &right) == -1,
        "K0A-A3 RED: slots(page=PAGE_COUNT) 未防御（-1）——越界页交防御列（四页"
        "页槽表之外无第 5 行兜底）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — K0A-A4 P3 积分占位数据未达齿（structural, 树扫 render_node; seam [4]）。
 * RED today: .h 无 KP_POINTS_NODATA_HINT 宏; .cc 渲染权威区无 case KP_PAGE_POINTS_HOME。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A4 -- P3 静态占位结构齿（S-F6.1-AC4 数据未达半）：KP_POINTS_NODATA_HINT 话术单源宏在 kidpal_render_node.h（逐字 = points-home-nodata.png + AC4）+ rn_render_kidpal_page_locked 区 case KP_PAGE_POINTS_HOME 消费单源宏（.cc 零字面重抄）+ 页名「星星积分本」+ 占位分支零数字格式化（不伪造 0 余额）/ 零进度线控件 / 零故事空态宏借用负锚 */
TEST_CASE("四页骨架: P3 积分占位数据未达——话术单源+零数字+零进度线 (K0A-A4)",
          "[four_page_skeleton][K0A-A4][structural]")
{
    char *pts_region;

    K0A_REQUIRE_ROOT();

    /* tooth 1: 话术单源宏住 kidpal_render_node.h（逐字 = docs/ui/V9/points-home-
     * nodata.png 设计权威 + S-F6.1-AC4 话术句; 文案单源纪律 = KSTORY_EMPTY_HINT
     * 先例同款——定义一处, 消费处零重抄）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_count_in_file(K0A_RN_H, "#define KP_POINTS_NODATA_HINT") >= 1,
        "K0A-A4 RED: kidpal_render_node.h 零 KP_POINTS_NODATA_HINT 宏定义——P3 "
        "数据未达话术无单源（S-F6.1-AC4「积分数据还在路上」类等待话术的冻结落点; "
        "seam [4]）");
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_file_contains(K0A_RN_H, "积分数据还在路上，等一等再来看"),
        "K0A-A4 RED: kidpal_render_node.h 零 AC4 等待话术字面——话术偏移设计权威"
        "（docs/ui/V9/points-home-nodata.png 逐字「积分数据还在路上，等一等再来看」; "
        "AC4 口径 = 纯等待、不暗示故障）");

    /* tooth 2: 渲染权威区 case KP_PAGE_POINTS_HOME 在世（四页渲染单一入口——
     * cycle_next 翻到 P3 必经 rn_render_kidpal_page_locked; 区锚 = 积分 case 起、
     * 故事 case 止, 稳定签名）。 */
    pts_region = k0a_region_between(K0A_RN_CC, "case KP_PAGE_POINTS_HOME",
                                    "case KP_PAGE_STORY");
    TEST_ASSERT_NOT_NULL_MESSAGE(pts_region,
        "K0A-A4 RED: kidpal_render_node.cc 渲染权威区零 case KP_PAGE_POINTS_HOME"
        "——P3 积分占位页未接入四页渲染（BOOT 翻到 P3 落 default 回首页 = 循环面"
        "缺失; requirements §2.4 四页循环图）");

    /* tooth 3: 占位分支消费单源宏（.cc 区内零话术字面重抄——文案单源纪律）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_contains(pts_region, "KP_POINTS_NODATA_HINT"),
        "K0A-A4 RED: P3 占位渲染分支未消费 KP_POINTS_NODATA_HINT 单源——等待话术"
        "另立第二来源（KSTORY_EMPTY_HINT 单源纪律同款红线; 文案漂移时两处不同步）");
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_slice(pts_region, "积分数据还在路上") == 0,
        "K0A-A4 RED: P3 占位渲染分支重抄话术字面——必须经 kidpal_render_node.h "
        "单源宏消费（字面只住 .h, KB-E2/KSTORY_EMPTY_HINT 先例同款）");

    /* tooth 4: 页名「星星积分本」在世（页身份 = §2.4 P3 行「星星积分本」+ 设计帧
     * 左上标题; 措辞归设计稿, 本齿钉其存在性不钉坐标/字号）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_contains(pts_region, "星星积分本"),
        "K0A-A4 RED: P3 占位渲染分支零页名「星星积分本」——页身份缺失（§2.4 P3 "
        "行; 孩子无法辨认这是积分本页）");

    /* tooth 5 (负锚 · S-F6.1-AC4「不显示数字」): 占位分支零数字格式化——"%d" 缺席
     * = 无余额/无计数可显（不伪造 0 余额; decisionCoverage D-01-skeleton「不伪造
     * 0 余额或进度线」）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_slice(pts_region, "%d") == 0,
        "K0A-A4 RED(负锚): P3 占位渲染分支出现 \"%%d\" 数字格式化——数据未达态"
        "显示数字（S-F6.1-AC4「余额与记录区不显示数字（未知显数会误导）」; 本包"
        "不伪造 0 余额红线）");

    /* tooth 6 (负锚 · 不画进度线): 占位分支零进度线/仪表类控件——进度线归 K2 正式
     * 积分主页（S-F6.1-AC2, D-02 landsIn package:261104-k2-points-device-ui）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_count_in_slice(pts_region, "lv_bar") == 0 &&
            k0a_count_in_slice(pts_region, "lv_arc") == 0 &&
            k0a_count_in_slice(pts_region, "lv_slider") == 0 &&
            k0a_count_in_slice(pts_region, "lv_line") == 0,
        "K0A-A4 RED(负锚): P3 占位渲染分支出现 lv_bar/lv_arc/lv_slider/lv_line"
        "——数据未达态预画进度线（S-F6.1-AC2 进度线归 K2; 本包静态占位只有等待"
        "话术, decisionCoverage D-01-skeleton）");

    /* tooth 7 (负锚 · 话术可区分): 占位分支零故事空态宏借用——AC4 等待话术与
     * S-F2.1 AC5/AC6 故事空态话术必须可区分（两页各归各单源）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_count_in_slice(pts_region, "KSTORY_EMPTY_HINT") == 0,
        "K0A-A4 RED(负锚): P3 积分占位分支借用 KSTORY_EMPTY_HINT* 故事空态宏——"
        "两页话术混淆（S-F6.1-AC4 与 S-F2.1-AC5/AC6「话术保持可区分」; 积分话术"
        "单源 = KP_POINTS_NODATA_HINT）");
    free(pts_region);
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — K0A-A5 source_page 枚举驱动映射齿（behavioral + structural; seam [5]）。
 * 行为半走 weak 哨兵（独立红跑通道哨兵红; host runner 由 display/story_page.c 强
 * 符号解析为真行为红——现状数值 case 2 对现状必红, 解析性定性见文件尾）。结构半
 * 树扫 story_page.c 映射区 KP_PAGE_* 符号（D-08 枚举驱动锚）。
 * RED today: 哨兵 NULL（standalone）; 映射区零 KP_PAGE_* 符号（case 2 数值在世）。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A5 -- source_page 枚举驱动映射（D-08 + 契约 §四 :347 闭集 + S-F2.2-AC1 回归半）：home/schedule/points_home/story 四值决策表 + 未知值/负值/KP_PAGE_COUNT 缺省 home 永不 story + 映射区 KP_PAGE_HOME/SCHEDULE/POINTS_HOME/STORY 符号在世（数值 case 2 废除的枚举驱动结构锚） */
TEST_CASE("四页骨架: source_page 枚举驱动——四值映射+未知缺省+符号结构锚 (K0A-A5)",
          "[four_page_skeleton][K0A-A5][behavior][structural]")
{
    const char *sp;

    K0A_REQUIRE_ROOT();

    /* tooth 0 (哨兵判别): story 行为缝的独立红跑通道无强符号源（story_page.c 是
     * LVGL-ful TU）——哨兵 NULL 时本 TEST_CASE 的红 = 「独立通道无实现」通道态,
     * 真行为红由 host runner 强符号通道 + tooth 3 结构齿 + grep 解析性定性联合
     * 承载（判读见文件尾 RED baseline 判读口径）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_ctx_source_page(K0A_KP_PAGE_HOME) != NULL,
        "K0A-A5 RED(哨兵): kidpal_story_ctx_source_page 独立红跑通道哨兵 NULL——"
        "非行为红; 真行为判据在 host runner 通道（display/story_page.c 已在册强符号）"
        "+ test 5 tooth 3 结构齿（映射区 KP_PAGE_* 符号, 现状零命中必红）");

    /* tooth 1: 四值决策表（镜像数值 = K0A_KP_PAGE_*; 契约 §四 :347 闭集主页四值;
     * home/schedule 原行为保留 = S-F2.2-AC1 回归半）。 */
    sp = kidpal_story_ctx_source_page(K0A_KP_PAGE_HOME);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "K0A-A5 RED: source_page(KP_PAGE_HOME) != \"home\"——首页会话来源回归被破"
        "（S-F2.2-AC1 原行为保留; 哨兵 NULL = 独立通道无实现, host runner 通道判真）");
    sp = kidpal_story_ctx_source_page(K0A_KP_PAGE_SCHEDULE);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "schedule") == 0,
        "K0A-A5 RED: source_page(KP_PAGE_SCHEDULE) != \"schedule\"——课表会话来源"
        "回归被破（契约 §四闭集）");
    sp = kidpal_story_ctx_source_page(K0A_KP_PAGE_POINTS_HOME);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "points_home") == 0,
        "K0A-A5 RED: source_page(KP_PAGE_POINTS_HOME) != \"points_home\"——P3 积分"
        "主页会话来源缺失（契约 §四 :347 闭集新值; D-08 枚举驱动; 现状该索引映 "
        "\"story\" = 记录语境误切根因）");
    sp = kidpal_story_ctx_source_page(K0A_KP_PAGE_STORY);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "story") == 0,
        "K0A-A5 RED: source_page(KP_PAGE_STORY) != \"story\"——故事页记录语境信号"
        "丢失（S-F2.2-AC1: source_page=story, front 拦截切累积态的板端半; 原行为"
        "随页位迁移保值不变）");

    /* tooth 2: 未知值/负值/KP_PAGE_COUNT 缺省 \"home\"——绝不外溢新取值、绝不回退
     * 成 story（契约 §四「未知值拒绝并记日志，不得回退成 story」的本侧半 = 产出侧
     * 只产闭集值; 对端拒绝半由 front 承载, 本齿钉产出侧不产未知值）。 */
    sp = kidpal_story_ctx_source_page(-1);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "K0A-A5 RED: 负值页未缺省 \"home\"——闭集缺省兜底缺失（绝不 NULL / 绝不 "
        "story）");
    sp = kidpal_story_ctx_source_page(99);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "K0A-A5 RED: 未知页值未缺省 \"home\"——同上（闭集防御）");
    sp = kidpal_story_ctx_source_page(4 /* = KP_PAGE_COUNT（四页态越界哨兵） */);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "K0A-A5 RED: KP_PAGE_COUNT 哨兵值未缺省 \"home\"——四页化后越界一格的"
        "防御口径缺失（越界值若外溢 = 对端拒绝路径被无谓触发）");

    /* tooth 3 (结构 · D-08 枚举驱动锚): story_page.c 映射区出现 KP_PAGE_* 符号
     * （switch 按 KP_PAGE_* 分流 = 新增页改枚举不再漏改映射; 数值 case 2 的废除由
     * 本符号锚 + tooth 1 行为齿联合承载——映射区无符号而行为对 = 仍可能数值耦合,
     * 两齿缺一不可）。 */
    {
        char *region = k0a_region_between(K0A_STORY_C,
                                          "const char *kidpal_story_ctx_source_page(",
                                          "kidpal_story_guide_bubble");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "K0A-A5 RED(infra): story_page.c 的 kidpal_story_ctx_source_page 映射区"
            "切取失败（函数锚漂移——须复核本齿锚, 不得改判为绿）");
        TEST_ASSERT_TRUE_MESSAGE(
            k0a_contains(region, "KP_PAGE_HOME") &&
                k0a_contains(region, "KP_PAGE_SCHEDULE") &&
                k0a_contains(region, "KP_PAGE_POINTS_HOME") &&
                k0a_contains(region, "KP_PAGE_STORY"),
            "K0A-A5 RED: source_page 映射区零 KP_PAGE_* 符号——数值 case 2 耦合在世"
            "（D-08: 废除数值页位判断, 按 KP_PAGE_* 枚举驱动; PROPOSAL §3.1-D-08 + "
            "plan.md 跨包事实「任何包都不得恢复数字页位判断」）");
        free(region);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — K0A-A6 P3 不切记录语境防回归齿（behavioral; D-08 核心防回归 +
 * 契约 §四 :347「P3 积分主页……普通问答语境，不得误切记录语境」）。
 * RED today: 哨兵 NULL（standalone; 现状映射对索引 2 产 "story" = 误切记录语境
 * 的真实现状——host runner 通道下 tooth 1 必红）。
 * ------------------------------------------------------------------------ */
/* coverage: K0A-A6 -- P3 积分主页不切记录语境（D-08 核心防回归）：points_home != story 专属负锚 + K2/K5 子页语境值（points_ledger/story_cover）不产出 + 四页产出值域恒在契约闭集内 */
TEST_CASE("四页骨架: P3 积分主页不切记录语境——points_home!=story 语境防回归 (K0A-A6)",
          "[four_page_skeleton][K0A-A6][behavior]")
{
    static const char *const k0a_closed_set[] = {
        "home", "schedule", "points_home", "story",
        "points_ledger", "story_cover" /* 闭集后两值 = 契约 §四 :347 全集成员,
                                        * 本包映射不得产出（K2/K5 scope） */
    };
    int page, i, matched;

    K0A_REQUIRE_ROOT();

    /* tooth 0 (哨兵判别): 同 K0A-A5 tooth 0——独立通道哨兵 NULL = 通道态红, 真行为
     * 红判据 = host runner 通道 + grep 解析性定性（story_page.c:2285-2298 现役实现
     * 为数值 case 2 -> 索引 2 产 "story" = tooth 1 对现状必红的机械证据）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_ctx_source_page(K0A_KP_PAGE_HOME) != NULL,
        "K0A-A6 RED(哨兵): kidpal_story_ctx_source_page 独立红跑通道哨兵 NULL——"
        "非行为红; host runner 通道下 tooth 1 对现状数值 case 2 必红（D-08 真实缺失"
        "行为）, 结构面由 K0A-A5 tooth 3 承载");

    /* tooth 1 (本齿核心 · D-08 防回归): P3 积分主页的会话来源不是 "story"——积分页
     * 按 KEY 开会话若带 source_page=story, front 会把会话切进记录累积态（故事归档
     * 链路误吞积分问答）= riskRationale 点名的头号回归形态。 */
    {
        const char *sp = kidpal_story_ctx_source_page(K0A_KP_PAGE_POINTS_HOME);
        TEST_ASSERT_TRUE_MESSAGE(
            sp != NULL && strcmp(sp, "story") != 0,
            "K0A-A6 RED: source_page(P3 积分主页) == \"story\"——积分页按 KEY 被"
            "误切故事记录语境（front acc.start 吞积分会话; PROPOSAL §2 现状差距"
            "「插入 P3 后……积分页按 KEY 会被误报为故事记录语境」; D-08 废除数值 "
            "case 2 的核心动因; 哨兵 NULL = 独立通道无实现, host runner 通道判真）");
        TEST_ASSERT_TRUE_MESSAGE(
            sp != NULL && strcmp(sp, "points_home") == 0,
            "K0A-A6 RED: source_page(P3 积分主页) != \"points_home\"——普通问答"
            "语境信号缺失（契约 §四 :347「P3 积分主页……产生 points_home……普通问答"
            "语境」）");
    }

    /* tooth 2 (负锚 · K2/K5 不提前): 四页合法索引的产出恒非子页语境值——
     * points_ledger（K2 详细账本）与 story_cover（K5 封皮）不由主页映射产出。 */
    for (page = 0; page <= 3; page++) {
        const char *sp = kidpal_story_ctx_source_page(page);
        TEST_ASSERT_TRUE_MESSAGE(
            sp != NULL && strcmp(sp, "points_ledger") != 0 &&
                strcmp(sp, "story_cover") != 0,
            k0a_fm("K0A-A6 RED(负锚): source_page(page=%d) == 子页语境值——K2/K5 "
                   "子页语境被主页映射提前产出（越包; decisionCoverage D-02/"
                   "D-01-cover landsIn 明示归属 K2/K5）", page));
    }

    /* tooth 3 (闭集守卫): 四页产出值恒在契约 §四 :347 六值闭集内（对端 front 对
     * 未知值拒绝并记日志——产出侧出闭集 = 无谓触发对端拒绝路径; 逐页核对）。 */
    for (page = 0; page <= 3; page++) {
        const char *sp = kidpal_story_ctx_source_page(page);
        matched = 0;
        for (i = 0; i < (int)(sizeof(k0a_closed_set) / sizeof(k0a_closed_set[0]));
             i++) {
            if (sp != NULL && strcmp(sp, k0a_closed_set[i]) == 0) {
                matched = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(matched,
            k0a_fm("K0A-A6 RED: source_page(page=%d) = \"%s\" 出契约 §四闭集"
                   "（home/schedule/points_home/points_ledger/story_cover/story "
                   "六值; 未知值 = front 拒绝路径被无谓触发）",
                   page, sp ? sp : "(null)"));
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — boundary：本文件注册序守卫 + 需求/契约 canary + 扫描器自检
 *   K0A-C1 结构半（本文件是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   canary（requirements 四页锚 + 契约 §四 六值闭集行在世——需求/契约面零触碰,
 *     本包零 docs 修订; TaskPackage docsSyncRequired=false）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: K0A-C1 -- host 全量终跑：本文件注册进 HOST_ACCEPTANCE_SRCS（置于 smoke 之前）结构守卫 + requirements §2.4 四页锚 / §2.2 中 4 页点锚 + 契约 §四 kidpal_session_ctx 六值闭集行 canary + 扫描器自检 */
TEST_CASE("四页骨架: 注册序守卫+需求契约canary+扫描器自检 (K0A-C1)",
          "[four_page_skeleton][K0A-C1][boundary]")
{
    char *mk;
    char hit[K0A_MAX_PATH];
    int n;

    K0A_REQUIRE_ROOT();

    /* tooth 1: 本文件须接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke 之前
     * （smoke 末位 = 最先执行, 包 0 boundary 注册序守卫）。RED until builder wires. */
    mk = k0a_read(K0A_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "K0A RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "four_page_skeleton.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "K0A-C1 RED: four_page_skeleton.acceptance.c 未接线进 "
            "HOST_ACCEPTANCE_SRCS——本包冻结验收未进 host 执行面（BUILDER DUTY: 见"
            "本文件头注 [6]，+1/-0，被测实现 key_gesture_parser.cc / keybar.c / "
            "story_page.c 已在册）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "K0A RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失——包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "K0A-C1 RED: four_page_skeleton.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前（smoke 末位 = 最先执行, boundary 注册序守卫）");
    }
    free(mk);

    /* tooth 2 (canary): 需求面四页锚在世（v1.71 已随批落——本包零需求修订的机械
     * 证据; 锚选在「四页化后仍成立」的节标题与页点句）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_exists(K0A_REQUIREMENTS),
        "K0A-C1 RED(infra): docs/requirements.md 丢失——需求面被触碰（零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_file_contains(K0A_REQUIREMENTS, "页面结构（4 页循环，本批 3→4）"),
        "K0A-C1 RED: requirements §2.4「页面结构（4 页循环，本批 3→4）」节标题锚"
        "丢失——需求面被修订（发现冲突走需求面 loop, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_file_contains(K0A_REQUIREMENTS, "中 4 页点") &&
            k0a_file_contains(K0A_REQUIREMENTS, "页点随之 3→4"),
        "K0A-C1 RED: requirements §2.2 comp-keybar「中 4 页点 / 页点随之 3→4」锚"
        "丢失——四点页点的需求权威被改动");
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_file_contains(K0A_REQUIREMENTS, "P3 积分主页") &&
            k0a_file_contains(K0A_REQUIREMENTS, "P4 封皮"),
        "K0A-C1 RED: requirements §2.3/§2.4 的「P3 积分主页 / P4 封皮」锚丢失——"
        "四页语义权威被改动（本包零需求触碰红线）");

    /* tooth 3 (canary): 契约 §四 kidpal_session_ctx 六值闭集行在世（本包产出其中
     * 主页四值; 契约零改动红线——改动走 /arch-change）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_exists(K0A_CONTRACT),
        "K0A RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        k0a_file_contains(K0A_CONTRACT,
                          "home / schedule / points_home / points_ledger / "
                          "story_cover / story"),
        "K0A-C1 RED: 契约 §四 kidpal_session_ctx source_page 六值闭集行丢失——"
        "source_page 闭集权威被改（越包; 改动走契约门）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(k0a_exists("firmware/main/key_gesture_parser.cc"),
        "K0A RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(k0a_exists("firmware/main/__k0a_no_such__.cc"),
        "K0A RED(infra): scanner reports a nonexistent file as present");
    n = k0a_count_in_file(K0A_KEY_H, "PAGE_COUNT");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "K0A RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region = k0a_region_between("firmware/main/key_gesture_parser.cc",
                                          "page_id_t key_page_next(",
                                          "global_action_t key_route_global(");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "K0A RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(k0a_count_in_slice(region, "PAGE_COUNT") >= 2,
            "K0A RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region);
    }
    n = k0a_scan_probe(hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "K0A RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
}

/* ---------------------------------------------------------------------------
 * 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   存量基线锚 = launch 侧 2026-11-04 现跑 firmware host 全量: **265 Tests / 3
 *   Failures @ 2f2c5871**（3 红全为 p1_auxbar AB 存量族 AB-A1/A2/A4, 判读按名单
 *   逐条对表不按总数; 判停引用 delivery/taskpackages/261103-star-points-story-cover/
 *   progress.md 两基线锚行）。既有套件零新增红 = 本包判据的干净起点。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1 tooth 2/3/4（四页循环缺失）; test 2 全部（枚举/prose 重锚面）;
 *       test 3 tooth 2/3（第 4 页页点/槽位缺失）; test 4 全部（P3 占位缺失）;
 *       test 5 tooth 1 的 points_home 半 + story 半（K0A_KP_PAGE_STORY=3 镜像下现役
 *       数值 case 2 走 default -> "home", 设计态红——story 原行为保留在四页世界里
 *       无法用索引 2 表达, 索引 2 恰是 D-08 要改成 points_home 的位置）+ tooth 3
 *       （映射区符号）——home/schedule 两半现状绿（原行为保留的 must-stay 证据）;
 *       test 6 tooth 1/2/3
 *       （standalone 哨兵红; host runner 通道下 tooth 1 对现状数值 case 2 必红 =
 *       D-08 真实缺失行为）;
 *     - 本文件 test 7 tooth 1（注册接线守卫, builder 首步 +1/-0 后转绿）。
 *   MUST-STAY-GREEN（守卫齿, 现状即绿、绿才是本包做对）:
 *     test 1 tooth 1/5/6; test 2 tooth 1 的既有成员半与 tooth 4（KP_PAGE_CHAR 零
 *       残留）; test 3 tooth 1/4/5; test 5 tooth 1 的 home/schedule 半与
 *       tooth 2（现状映射对 -1/99 已缺省 home）; test 7 tooth 2/3/4（canary +
 *       自检——requirements/契约四页锚 v1.71 已随批落, 现状绿）。
 *   判读纪律: 红基线逐齿对表（新增红才是本包责任; 3 红存量名单见上）。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-11-04 实跑）:
 *   通道① 编译通道（DEV-GOVERNANCE「验收测试纪律」§4 C 类冻结验收实编）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline firmware/main/test/four_page_skeleton.acceptance.c
 *     以及完整链接编译（g++ -c key_gesture_parser.cc + keybar.c + kidpal_voice_
 *     states.c）——两 TU 日志皆空 = 编译通道 error 0、warning 0（本文件在冻结相位
 *     可编译 = 镜像/哨兵策略成立: 不引用尚未落地的 KP_PAGE_POINTS_HOME 枚举成员）。
 *   通道② 执行通道（§3 冻结即冒烟）= 同一份冻结件 + 已在册真实现源（key_gesture_
 *     parser.cc / keybar.c / kidpal_voice_states.c）经边界内脚手架（red-baseline 下
 *     unity.h stub + runner.c）编成原生 Linux ELF 当场执行。story 行为缝
 *     （kidpal_story_ctx_source_page）的强符号源 display/story_page.c 是 LVGL-ful
 *     TU、不进独立通道——该缝齿在独立通道为哨兵红, 其真实行为红由 (a) host runner
 *     强符号通道（builder 接线后全量跑）+ (b) test 5 tooth 3 映射区结构齿（现状
 *     零 KP_PAGE_* 符号, 对现状必红）+ (c) grep 解析性定性（story_page.c:2285-2298
 *     现役实现为数值 case 2 -> 索引 2 产 "story"）三方联合承载。退出码取证用
 *     `wsl -- <elf>` 直调形态（wsl.exe 参数重组伪影见 red-baseline-summary.
 *     char-stroke.md 同记）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）; 凡引用 delivery 目录下证据路径一律按相对路径 + 仓根 cwd 前提。
 * ---------------------------------------------------------------------------
 * fixture 自含性声明（断言设计透镜, 逐条）:
 *   - test 1/3 行为齿 fixture = 纯值域入参（页枚举/整数）, 无资产/时间真值/环境
 *     前提; 依赖 = key_gesture_parser.cc 与 keybar.c 在册（误摘 = 链接期响亮失败）。
 *   - test 2/4/5 结构齿依赖 = 仓根可定位（cwd 相对探测, REQUIRE_ROOT 前置）+ 被
 *     扫源文件在世; 齿锚全符号锚零行号（签名漂移 = RED(infra) 复核, 不改判绿）。
 *   - test 5/6 story 缝齿 = host/独立双通道: host runner 真行为、独立通道哨兵红
 *     （近似假设显式化: 独立通道的哨兵红不是行为证据, 行为判据归 host runner）。
 *   - 驱动方闭环: 四页循环驱动方 = 板级 BOOT 手势 -> key_route_layer(0) ->
 *     keya_dispatch_global_action(PAGE_NEXT) -> cycle_next（KLD 既有齿在世）;
 *     source_page 驱动方 = Listening 分支单点消费（RSL-A2/A3 既有齿在世）——本包
 *     两缝的消费端均既有, 无缺驱动方盲区。P3 占位的消费端 = 孩子看屏（视觉半归
 *     K0A-E3 真机轮 + design review, host 只钉结构单源——host 覆盖差集显式申报）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-11-04  initial freeze (acceptance-author, TaskPackage
 *               261104-k0a-four-page-skeleton)。
 *   2026-11-04  红基线实跑回填（§3 冻结即冒烟 + §4 C 类实编合一——本件已由 planner
 *               代落 HOST_ACCEPTANCE_SRCS 登记行, host build 全量编译 error 0）:
 *               **273 Tests / 21 Failures / 0 Ignored, EXIT=21**（存量基线 265/3 +
 *               本件 7 TEST_CASE + story_page_v2 SPV2-RK1 1 = 273）。21 红逐条
 *               对表: ① 存量 3 红 = p1_auxbar AB-A1/A2/A4（引用锚判停, 非本包责任）;
 *               ② 本包义务红 18 = 本件 K0A-A1/A2/A3/A4/A5/A6（C1 注册齿绿——
 *               登记行在位）+ retract M2K2-A1(行为半)/M2K2-A1(结构半)/M2K2-A2×2/
 *               M2K2-A3 + key_layer_dispatch KD-A3/KD-E1 + keybar_three_slot
 *               KB-A3 + record_session_live RSL-A3 + story_page_v2 SPV2-RK1 +
 *               record_session_repair RSR-A1/RSR-A5（planner 授权镜像重锚 2→3 的
 *               基层判据半）+ diag_log DLG-A2（planner 授权页名闭集重锚）——全部为
 *               四页骨架缺失的真实行为红, 每条失败消息与声明缺失行为逐条比对过;
 *               schedule_grid KSG-A6 同源判别齿两态皆绿（守卫性质保持）。零
 *               「本应红现绿」、零「本应绿现红」。
 * --------------------------------------------------------------------------- */
