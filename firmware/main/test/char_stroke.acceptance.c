/*
 * Frozen acceptance tests — TaskPackage 260917-m2-k1-char-link-defect-fix
 * (subsystem: firmware; workstream m2-char-window; M2-K1; mergeOrder 28;
 *  查字链路两缺陷正式修复: ① SVG 解析器补 C 命令 ② char_page 动画帧去大栈数组 +
 *  taskLVGL 栈定容收编)。
 *
 * Story: 里程碑 delivery/milestones/milestones.jsonl#M2-char-window nextAction
 * 「live 查字链路修复（NFR-PERF-2 黄灯转绿前提）」; 缺陷登记权威 = docs/
 * requirements.md S-F3.1 可行性注记（2026-09-15 真机复验定案）+ NFR-PERF-2。
 *
 * 缺陷① 现状（acceptance-author 2026-09-18 冻结前实查 char_stroke.c）:
 *   kidpal_char_stroke_parse（:144, tokenize :49）tokenizer 接受集仅
 *   M/L/Q/Z（+小写 m/l/q/z）, 未知字符 -> -1 整笔丢弃。makemeahanzi
 *   graphics.txt 路径词汇表含 C（三次贝塞尔, 绝对命令）——勇 9 笔中 6 笔含 C
 *   被整笔丢弃, 示范字残缺（S-F3.1 真机复验实证）。D3 契约 strokes = makemeahanzi
 *   SVG path 自由文本（样例即含 Q）, C 命令属词汇表内解析补全, **非契约改动**
 *   （plan.md 裁定, architect 快审确认归类; 本文件零 docs 面触碰）。
 * 缺陷② 现状: char_page.cc charpage_render_frame（:185）每帧每笔声明
 *   pts[256]/sc[256] 局部大数组（+ parse 内部 toks[256], 合计 ~6.5KB 级栈）,
 *   打穿 esp_lvgl_port 默认 taskLVGL 栈 7168 -> panic; custom_lcd_display.cc:131
 *   的 16384 直改是 2026-09-15 一次性实验补丁（注记自带「本行届时随包收编」义务）,
 *   本包承接收编。
 *
 * Nature: HOST-SIDE BEHAVIORAL（char_stroke.c 纯逻辑数据面, 强符号消费）+
 *   STRUCTURAL（tree-scan, char_page.cc / custom_lcd_display.cc 为设备装配面、
 *   非 host 编译, 以源级结构断言承载）。reminder_overlay_unify.acceptance.c
 *   （K4/rma8）+ 归档件 char_stroke.acceptance.c（260829, CHR-A2 语义先例）范式。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements, 本文件只读扫工作树 +
 * 强符号行为级验证; 强定义 home = firmware/main/display/char_stroke.c——已入树,
 * builder 接线进 runner 后覆盖本文件 weak 哨兵, 见 [BUILDER DUTY]）:
 *
 *   [1] C 命令解析补全（M2K1-A1; home = char_stroke.c cs_tokenize + parse switch,
 *       与既有 Q 同款离散成点策略, 复用既有点集管线, 不引入新渲染路径——plan.md
 *       改法取舍）:
 *       kidpal_char_stroke_parse 对含 C（三次贝塞尔, 绝对命令大写, makemeahanzi
 *       词汇表）的 D3 stroke path:
 *         -- 绝不整笔丢弃: 返回 >= 1 点集（现状缺陷 = -1 整笔丢弃）;
 *         -- C 段语义: (x1,y1,x2,y2,x,y) 六 number = 两控制点 + 终点,
 *            B(t) = (1-t)^3·P0 + 3(1-t)^2·t·C1 + 3(1-t)·t^2·C2 + t^3·P1;
 *         -- 每 C 段贡献 >= 2 个点（对齐 Q 段「>= 2 内点」冻结语义, CHR-A2 先例）,
 *            段终点精确落于点集（与 Q「端点精确」同款）;
 *         -- 真离散非端点复制: 段中存在逼近 B(0.5) = (P0+3C1+3C2+P1)/8 的采样点
 *            （容差 40 源域单位, 1024 域; ≥5 均匀采样即满足, 8 段密度精确命中）;
 *         -- 与 Q/L/Z 共存于同一路径时零互扰（词汇表扩充不改既有命令消费）;
 *         -- 十进制/负坐标 number（makemeahanzi 实况形状）照常解析。
 *
 *   [2] M/L/Q/Z 既有命令回归（M2K1-A2; 强定义零改动守卫）:
 *       四命令解析点集与本文件冻结参照表**逐位一致**（参照值 = 修复前解析器
 *       2026-09-18 实测 dump, 与二次贝塞尔公式解析值核对一致; C 命令加入不得
 *       改变既有四命令任何一位输出）; 防御面零回归（NULL/空串/首 token 非命令/
 *       未知命令 X/截断参数/缓冲溢出 -> -1——**未知命令防御必须保持**, C 加入
 *       不得放宽 tokenizer 未知字符拒绝语义）。
 *
 *   [3] 栈修复结构齿（M2K1-A3; home = firmware/main/display/char_page.cc +
 *       firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc, 两文件
 *       均设备装配面、非 host 编译——结构断言以源级树扫承载, K4 test 7 先例）:
 *       a) char_page.cc 动画帧路径（charpage_render_frame 函数体区域）零
 *          ~6.5KB 级栈局部数组: 不再有 kidpal_cs_point [256] 形态的非 static
 *          局部声明（pts/sc 现状 3 处——缺陷②核心; 256·8B·3 ≈ 6KB + parse 内
 *          toks[256] ≈ 6.5KB 级口径与 plan.md 背景一致）;
 *       b) 动画帧路径零 kidpal_char_stroke_parse 调用（预解析去栈——plan.md
 *          改法取舍①「256 点局部数组改为静态/预解析堆承载」的机制面; parse
 *          自身 toks[256] 2KB 栈随调用点移出帧路径而离场。若实现采用其它等价
 *          去栈形态, 须返 planner/acceptance-author 复核返钉, 不得静默改齿）;
 *       c) custom_lcd_display.cc taskLVGL 栈定容收编: task_stack 赋值行不再是
 *          裸字面 16384（现状 :131）, 栈定容值收编为命名常量（同文件内含
 *          STACK 名字面 + 16384 值的定义行; plan.md 改法取舍②「16384 收编为
 *          命名常量」）;
 *       d) 一次性补丁注记替换: 旧注记义务尾「本行届时随包收编」销账（注记从
 *          「一次性实验」改写为正式定容依据——plan.md 改法取舍②; 以义务尾
 *          marker 销账承载, 新注记措辞不锁）。
 *
 *   [4] M2K1-A4（live 端到端复证）——**本文件不承载**: TaskPackage 已冻结为
 *       kind:structural, 真机齿 deferred 归 M2-K3 设备轮（单真机不两轮撞车,
 *       launchPreconditions 第 4 条; NFR-PERF-2 live 复测随同轮）。host 侧
 *       前置 = 本文件 [1]/[2]/[3] + device build 闸（M2K1-E3）。本文件不造
 *       live 伪断言。
 *
 *   [5] meaning 承载（MCF-A2; home = char_stroke.h + char_stroke.c
 *       kidpal_char_card_build; 260920-m2-charwindow-card-fix additive 追加;
 *       需求权威 = docs/requirements.md S-F3.2 AC1 v1.56 五行口径 + F3 对账⑤）:
 *       kidpal_char_info 增成员 `const char *meaning`（**NULL 容忍**——与
 *       pinyin/radical/words 必非空语义不同, **不进 NULL 拒绝集**; human 裁降级
 *       口径: 缺释义只损右栏一行、不构成盲渲染风险, 旧 payload 整窗降级 = 行为
 *       回归）; kidpal_char_card 增 `char meaning[96]`（容量对齐 words 惯例）。
 *       语义: 非空 -> verbatim 截断安全拷贝（恒 NUL 结尾, 与 pinyin/words 同款）;
 *             NULL -> out->meaning[0] = '\0' 且 rc = 0（缺义不降级整窗）;
 *             既有四字段 + progress 文案零回归（M2-K1 冻结面, test 2 参照表守卫
 *             兜底）。
 *       **同步冻结形态**: char_stroke.h 须 `#define KIDPAL_CHAR_STROKE_HAS_MEANING 1`
 *       ——本文件 MCF-A2 行为齿（test 6）的编译期 seam 探针: 落地前 fallback 0 =
 *       行为齿缺席编译（seam 缺失的红由 test 5 结构齿承载, 非空过; K3 件
 *       `#ifndef KIDPAL_LAYER_CHAR_WINDOW` fallback 宏同款先例）, 落地后行为齿自动
 *       生效。builder 漏定义该宏由 test 5 结构齿咬住; 改动该形态须返 planner 重派
 *       acceptance-author。
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: M2K1-A1 -> test 1（C 命令零丢笔 + 三次贝塞尔离散语义: 最小路径/
 *             多段混合 L/Z/与 Q 共存/十进制负坐标 + 段终点精确 + B(0.5) 逼近;
 *             RED today = 含 C 路径 -1 整笔丢弃, 修复后 GREEN, closeout 声明
 *             RED→GREEN）
 *   coverage: M2K1-A2 -> test 2（M/L/Q/Z 冻结参照表逐位回归 + 防御面零回归;
 *             freeze 相位即 GREEN = 钉住修复前行为, 修复后必须保持绿——回归守卫
 *             本性, 非假绿）
 *   coverage: M2K1-A3 -> test 3（char_page 帧路径去大栈数组 + 预解析去栈 +
 *             taskLVGL 栈定容命名常量收编 + 一次性补丁注记销账, 源级树扫;
 *             RED today = 义务红设计态, 修复后 GREEN）
 *   M2K1-A4（kind:structural, live deferred 归 M2-K3 设备轮）-> 无齿条目,
 *             见 [4]; 真机证据落 M2-K3 设备轮。
 *   M2K1-E1（host runner 全量零新增红）-> test 4（注册接线结构守卫, 防套件级
 *             假绿——本文件若未接线, 上述三齿在 runner 全量里静默缺席, E1 判读
 *             失去对象）+ 文件尾「RED baseline 判读口径」段。
 *   M2K1-E2（铁律: 不改 docs/contracts 契约面与 spec）-> test 4 tooth 2（契约面/spec
 *             面在世 canary——发现冲突走 spec-lane 逃生门, 不预先改 docs）。
 *   coverage: MCF-A2 -> test 5（结构半: 头双字段 + 编译期探针宏 + card_build meaning
 *             消费, 注释剥离后计数——sealing pass 纪律; RED today = seam 未落,
 *             builder 落地后 GREEN）+ test 6（行为半, 编译期探针门控: 非空 verbatim
 *             截断安全 / NULL -> 空串 rc=0 缺义不降级整窗 / 四字段 + progress 零
 *             回归; 探针宏缺席时本齿缺席编译——非空过, 由 test 5 咬住）。
 * ---------------------------------------------------------------------------
 *
 * BUILDER DUTY（接线, acceptance-author 被 enforce-scope 拦在 CMakeLists 外——
 * K2/K3/K4 同款先例）: 在 firmware/main/test/host_app/main/CMakeLists.txt 的
 * HOST_ACCEPTANCE_SRCS 追加两行, 置于 "../../host_infra_smoke.acceptance.c"
 * 之前（smoke 保持末位 = 最先执行, 包 0 boundary 齿注册序守卫; 本文件 test 4
 * tooth 1 校验此序）:
 *     "../../char_stroke.acceptance.c"
 *     "../../../display/char_stroke.c"   （[1]/[2] 强符号源; 其 art_assets.h 引用
 *       为纯声明头, parse 路径不调 art 函数, 无需另接 art_assets.c——260829 先例）
 *
 * mutation 证据期望（mutationRequired=true, builder 回传义务, 落 taskpackage
 * mutationEvidence.perAcceptance, acceptanceId 与 taskpackage acceptance[] 正文
 * 逐字节一致——勿到 verifier 轮才补）:
 *   M2K1-A1 —— 变异 = 撤除 C 命令支持（如 tokenizer 接受集退回 M/L/Q/Z 或
 *              switch 撤 C case）, 预期 test 1 转 RED, 还原后 GREEN;
 *   M2K1-A2 —— 变异 = 改动任一既有四命令行为（如 CS_BEZIER_SEGS 改值或坐标
 *              计算变体）, 预期 test 2 参照表转 RED, 还原后 GREEN;
 *   M2K1-A3 —— 变异 = char_page.cc 帧路径恢复任一 kidpal_cs_point [256] 栈
 *              局部（或 custom_lcd_display.cc 恢复裸 16384 字面）, 预期 test 3
 *              转 RED, 还原后 GREEN; 逐字证据随 closeout 回传。
 *   MCF-A2 —— 变异 = card_build 撤 meaning 装配（如把 meaning 纳入 NULL 拒绝集 /
 *              删 verbatim 拷贝分支）, 预期 test 6 转 RED（state 3 rc=-1 或
 *              state 1 verbatim 失败）+ test 5 结构齿同红, 还原后 GREEN。
 *              撤探针宏 = test 5 红 + 行为齿缺席编译（构建面暴露）。
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/
 *      build/host_infra_test.elf   （仓根 cwd 运行; 结构齿自手持 cwd 定位仓根）
 *   Freeze-phase standalone execution（接线前实跑, red-baseline 脚手架先例——
 *   device_ws_switch 260903 同款）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -Wall -I. -o /tmp/m2k1_red ../../char_stroke.acceptance.c \
 *         runner.c ../../../display/char_stroke.c -lm
 *     cd ../../../../../..  (repo root) && /tmp/m2k1_red
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-18 实测）:
 *   通道① 既有 host runner 全量基线**引用不重跑**（planner 判停口径）: 上游锚
 *      delivery/workstreams/m2-char-window/progress.md「上游已知红基线」行——
 *      git diff be3cf9f HEAD 于 server/ 与 firmware/ 均空, host 35 Tests /
 *      0 Failures、server 37 passed / 0 failed @ be3cf9f。
 *   通道② 本文件新齿 standalone 实跑（上方案例）, 2026-09-18 未修复树实测:
 *      4 Tests 3 Failures——
 *        test 1（M2K1-A1）FAIL = **缺陷①本体红**: 含 C 路径 -1 整笔丢弃（红名
 *          verbatim 见交付报告; 红因 = 当前真实缺失行为, 绝非环境/编译错）;
 *        test 2（M2K1-A2）PASS = 参照表钉住修复前行为（回归守卫本性）;
 *        test 3（M2K1-A3）FAIL = 义务红设计态（大栈数组/裸字面/旧注记仍在,
 *          builder 落地后转绿）;
 *        test 4（boundary）FAIL = 义务红（未接线, BUILDER DUTY 落地后转绿）。
 *   通道③ C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *      gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *          red-baseline -I firmware/main/test firmware/main/test/char_stroke.
 *          acceptance.c
 *      编译通道 error 为零（验收文件本身可编译, 含 weak 哨兵态）。
 *   修复后判读: host runner 全量零新增红 + test 1/3/4 转绿 + test 2 保持绿
 *      （M2K1-E1）; test 1 的 RED→GREEN 红名 verbatim + standalone 复跑证据随
 *      closeout 回传。
 *   260920-m2-charwindow-card-fix 增量判读（acceptance-author 2026-09-20 实跑）:
 *      test 5（MCF-A2 结构半）FAIL = 义务红设计态（头零 meaning 字段 / 零探针宏 /
 *      card_build 零 meaning 消费——seam 未落, builder 落地后转绿）; test 6 = 探针
 *      门控缺席编译（不进跑分, 红由 test 5 承载）。修复后期望: test 5 GREEN +
 *      test 6 自动生效并 GREEN（MCF-E1 半边, 另半边在 char_window_overlay.acceptance.c
 *      test 9——A1 负向齿从现状红转绿）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-18  initial freeze (acceptance-author, TaskPackage
 *               260917-m2-k1-char-link-defect-fix; A1 RED 实跑 + A2 参照表
 *               2026-09-18 未修复树实测 dump 冻结)。
 *   2026-09-20  MCF additive revision (acceptance-author, TaskPackage
 *               260920-m2-charwindow-card-fix; additive-only——既有 test 1–4 与
 *               参照表零触碰): 追 seam [5] + test 5（MCF-A2 结构半）+ test 6
 *               （MCF-A2 行为半, 编译期探针 KIDPAL_CHAR_STROKE_HAS_MEANING 门控,
 *               K3 件 #ifndef fallback 宏同款先例）——meaning 承载（S-F3.2 AC1
 *               v1.56 释义行数据面; NULL 容忍不进拒绝集, human 裁降级口径）。
 *               char_stroke.h 头注「本头声明与冻结验收 fallback 块逐字一致」的自述
 *               随本包语义追加进一步过时（本文件无 fallback 块, seam 以真实头为
 *               单源 + 探针宏门控）——按 plan.md 裁定只留痕不改头注（头注归
 *               builder 的 char_stroke.h 变更面）。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* 被测面冻结契约头（char_stroke.c 已入树, 直接消费真实声明——单源, 不走
 * __has_include 探针; 归档件 260829 范式的现役简化）。相对路径引用, host runner
 * 与 standalone 编译均可达。 */
#include "../display/char_stroke.h"

/* -------------------------------------------------------------------------- *
 * weak 哨兵（仅行为齿消费的解析入口）: 若 builder 只接线本验收文件而漏接
 * char_stroke.c, 强符号缺席 -> 哨兵 -2, 行为齿红名读作「frozen contract not
 * wired」——真实缺失面红, 绝非链接错误（K4 先例纪律）。
 * ------------------------------------------------------------------------ */
#define CSA_UNIMPLEMENTED (-2)

__attribute__((weak)) int kidpal_char_stroke_parse(const char *svg_path,
                                                   kidpal_cs_point *out, int max_pts)
{
    (void)svg_path; (void)out; (void)max_pts;
    return CSA_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * MCF-A2 seam 编译期探针（冻结 seam [5] 同步形态, 260920-m2-charwindow-card-fix）:
 * char_stroke.h 落 meaning 字段时同步 #define KIDPAL_CHAR_STROKE_HAS_MEANING 1。
 * 落地前 fallback 0 = 行为齿（test 6）缺席编译——非空过, seam 缺失的红由 test 5
 * 结构齿承载（K3 件 #ifndef fallback 宏同款先例）。
 * ------------------------------------------------------------------------ */
#ifndef KIDPAL_CHAR_STROKE_HAS_MEANING
#define KIDPAL_CHAR_STROKE_HAS_MEANING 0
#endif

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿, K4/kld 先例）。全部扫描跑在真实
 * 工作树上, root 以手持 cwd 定位（仓根或 firmware/main/test/host_app 均可）。
 * ------------------------------------------------------------------------ */

#define CSA_MAX_PATH 1024
#define CSA_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根; 行号快照非长期事实源, 齿锚全部用符号锚）。 */
#define CSA_CHAR_PAGE      "firmware/main/display/char_page.cc"
#define CSA_BOARD_LCD      "firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc"
#define CSA_CMAKELISTS     "firmware/main/test/host_app/main/CMakeLists.txt"
#define CSA_CONTRACT       "docs/contracts/interface-contract.md"
#define CSA_REQUIREMENTS   "docs/requirements.md"
#define CSA_CHAR_STROKE_H  "firmware/main/display/char_stroke.h"
#define CSA_CHAR_STROKE_C  "firmware/main/display/char_stroke.c"

static char csa_root[CSA_MAX_PATH];
static int csa_root_ready;

static int csa_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
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
    snprintf(out, cap, "%s/%s", csa_root, rel);
    return 1;
}

static int csa_exists(const char *rel)
{
    char p[CSA_MAX_PATH];
    return csa_rel(p, sizeof(p), rel) && csa_is_regular(p);
}

static char *csa_read(const char *rel)
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
    return buf;
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the
 * first occurrence of end_needle AFTER it（K4 krmu_region_between 先例）。 */
static char *csa_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = csa_read(rel);
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

/* Occurrence count of needle inside a slice/file（0 when unreadable/absent）。 */
static int csa_count_in_slice(const char *slice, const char *needle)
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

static int csa_count_in_file(const char *rel, const char *needle)
{
    char *buf = csa_read(rel);
    int r = csa_count_in_slice(buf, needle);
    free(buf);
    return r;
}

/* -------------------------------------------------------------------------- *
 * Comment-stripping scanner support（260920-m2-charwindow-card-fix 追加, K3 件
 * cwx_strip_c_comments 同款）: 正向结构齿只认**可执行源**——注释里的 needle 无法
 * 满足计数（builder M4 实证假绿教训）。负向齿不使用（剥注释会放松负向齿）。
 * 字符串/字符字面量体保留（释义格式字面等字符串针仍可命中）。注释字节 -> 空格,
 * 长度保持; 调用方负责 free; 不可读/NULL -> 计 0。
 * ------------------------------------------------------------------------ */
static char *csa_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL);
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

/* 注释剥离后的 needle 计数（正向结构齿用; 注释无法满足）。 */
static int csa_count_in_slice_code(const char *slice, const char *needle)
{
    char *stripped;
    int r;

    if (slice == NULL) {
        return 0;
    }
    stripped = csa_strip_c_comments(slice);
    r = csa_count_in_slice(stripped, needle);
    free(stripped);
    return r;
}

static int csa_count_in_file_code(const char *rel, const char *needle)
{
    char *buf = csa_read(rel);
    int r = csa_count_in_slice_code(buf, needle);
    free(buf);
    return r;
}

/* 行级判定: slice 内同时含 needle1 与 needle2、且不含 excl（NULL = 不排除）的
 * 行数——栈局部数组/裸字面收编齿的载体（声明形态行级匹配, 跨行声明不覆盖:
 * 误报只会漏报不会假红, mutation 证据兜底）。 */
static int csa_count_lines_with_both(const char *slice, const char *needle1,
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
        char tmp[CSA_MAX_PATH];
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

static int csa_caseci_contains(const char *hay, const char *lower_needle)
{
    size_t n = strlen(lower_needle);
    size_t hn = strlen(hay);

    if (n == 0 || n > hn) {
        return 0;
    }
    for (size_t i = 0; i + n <= hn; i++) {
        size_t j;
        for (j = 0; j < n; j++) {
            char hc = hay[i + j];
            if (hc >= 'A' && hc <= 'Z') {
                hc = (char)(hc - 'A' + 'a');
            }
            if (hc != lower_needle[j]) {
                break;
            }
        }
        if (j == n) {
            return 1;
        }
    }
    return 0;
}

#define CSA_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(csa_locate_root(), \
    "CSA RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static void csa_require_implemented(int value, const char *what)
{
    if (value == CSA_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "CSA RED: frozen seam NOT wired/implemented — %s "
                 "(builder must wire display/char_stroke.c + this file into "
                 "HOST_ACCEPTANCE_SRCS, see char_stroke.acceptance.c header "
                 "[BUILDER DUTY])", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper（KLD/KVR 先例）。 */
static char csa_fm_buf[1024];
static const char *csa_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *csa_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(csa_fm_buf, sizeof(csa_fm_buf), fmt, ap);
    va_end(ap);
    return csa_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * 点集比较 helpers（M2K1-A1 段语义 + M2K1-A2 参照表）。
 * 容差 1e-3 源域单位（1024 域, ~0.0004 屏 px 量级——参照值 %.6f dump 冻结,
 * round-trip 误差 <= 5e-7, 任何真实行为漂移都远超此阈）。
 * ------------------------------------------------------------------------ */
#define CSA_TOL 1e-3f

static int csa_has_point_exact(const kidpal_cs_point *pts, int n, float x, float y)
{
    for (int i = 0; i < n; i++) {
        if (fabsf(pts[i].x - x) <= CSA_TOL && fabsf(pts[i].y - y) <= CSA_TOL) {
            return 1;
        }
    }
    return 0;
}

static int csa_has_point_near(const kidpal_cs_point *pts, int n, float x, float y, float tol)
{
    for (int i = 0; i < n; i++) {
        if (fabsf(pts[i].x - x) <= tol && fabsf(pts[i].y - y) <= tol) {
            return 1;
        }
    }
    return 0;
}

static int csa_pts_match(const kidpal_cs_point *got, int n,
                         const float (*exp_pts)[2], int exp_n)
{
    if (n != exp_n) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        if (fabsf(got[i].x - exp_pts[i][0]) > CSA_TOL ||
            fabsf(got[i].y - exp_pts[i][1]) > CSA_TOL) {
            return 0;
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — M2K1-A1 C 命令解析齿（behavioral, seam [1]）
 * 含 C 的 makemeahanzi 形状路径零丢笔 + 三次贝塞尔离散语义（段终点精确 +
 * B(0.5) 逼近 + 与 Q/L/Z 共存零互扰 + 十进制/负坐标）。
 * RED today（2026-09-18 未修复树 standalone 实跑）: 含 C 路径 -1 整笔丢弃
 * （缺陷①本体红; 红名 verbatim 见交付报告）。修复后 GREEN, closeout 声明
 * RED→GREEN。
 * ------------------------------------------------------------------------ */
/* coverage: M2K1-A1 -- C 命令解析齿: 含 C 的 makemeahanzi 路径零丢笔（现状 -1 整笔丢弃 -> 修复后 >=1 点集）+ 三次贝塞尔离散语义（每段 >=2 点/段终点精确/B(0.5) 逼近/与 Q·L·Z 共存零互扰/十进制负坐标）; 修复前 RED, 补 C 命令离散成点后 GREEN, closeout 声明 RED→GREEN */
TEST_CASE("C 命令解析: 含 C 的 makemeahanzi 路径零丢笔+三次贝塞尔离散 (M2K1-A1)",
          "[char_stroke][M2K1-A1][cubic]")
{
    kidpal_cs_point pts[512];
    int n;

    /* ---- tooth 1: 最小 C 路径——绝不整笔丢弃（缺陷①的最小复现形状）。 ---- */
    n = kidpal_char_stroke_parse("M 0 0 C 10 0 20 10 30 30", pts, 512);
    csa_require_implemented(n, "kidpal_char_stroke_parse unimplemented/wired-missing");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        csa_fm("M2K1-A1 RED: 最小 C 路径解析返回 %d——含 C 命令的 makemeahanzi 笔画"
               "被整笔丢弃（现状缺陷①: tokenizer 仅 M/L/Q/Z, 未知命令 -> -1; "
               "S-F3.1 示范字残缺根因）。修复 = C 命令三次贝塞尔离散成点", n));

    /* tooth 2: 起点精确 + C 段终点精确落于点集（与 Q「端点精确」同款冻结语义,
     * CHR-A2 先例）。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 0.0f, 0.0f),
        "M2K1-A1 RED: M 起点未入点集——C 补全不应扰动 moveto 语义");
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 30.0f, 30.0f),
        "M2K1-A1 RED: C 段终点 (30,30) 未精确落于点集——三次贝塞尔离散端点精确"
        "语义缺失（Q 段同款: 端点精确落于曲线上）");

    /* tooth 3: 真离散非端点复制——存在逼近 B(0.5) 的采样点。
     * B(0.5) = (P0 + 3·C1 + 3·C2 + P1)/8 = (15, 7.5)。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_near(pts, n, 15.0f, 7.5f, 40.0f),
        "M2K1-A1 RED: C 段零 B(0.5)=(15,7.5) 附近采样点（容差 40 源域单位）——"
        "离散未沿三次贝塞尔曲线取点（端点直连/复制非真离散）");

    /* ---- tooth 4: 多段 C + L + Z 混合路径（勇形笔画实况形状）——整笔零丢。 ---- */
    n = kidpal_char_stroke_parse(
        "M 100 200 C 150 180 250 180 300 200 C 320 210 340 230 350 260 "
        "L 350 400 C 340 500 260 560 200 560 Z", pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(n >= 9,
        csa_fm("M2K1-A1 RED: 多段 C+L+Z 混合路径解析返回 %d（须 >= 9 = M1 + "
               "3 段×>=2 + L1 + Z1）——混合词汇表路径仍被整笔丢弃", n));

    /* tooth 5: 各 C 段终点精确 + Z 闭合到子路径起点（全程遍历完成的形状证据——
     * 末点 == (100,200) 说明 C 段全部被消费、Z 闭合在位, 无静默截断）。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 300.0f, 200.0f) &&
                                 csa_has_point_exact(pts, n, 350.0f, 260.0f) &&
                                 csa_has_point_exact(pts, n, 200.0f, 560.0f),
        "M2K1-A1 RED: 某一 C 段终点未精确落于点集——多段离散有段被吞或端点漂移");
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 100.0f, 200.0f),
        "M2K1-A1 RED: 路径末点非 Z 闭合点 (100,200)——C 段加入后 Z closepath 到"
        "子路径起点语义被破坏或路径被静默截断");
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_near(pts, n, 200.0f, 185.0f, 40.0f) &&
                                 csa_has_point_near(pts, n, 328.75f, 222.5f, 40.0f) &&
                                 csa_has_point_near(pts, n, 293.75f, 517.5f, 40.0f),
        "M2K1-A1 RED: 三段 C 的 B(0.5) 锚 (200,185)/(328.75,222.5)/(293.75,517.5) "
        "附近零采样点——离散未按三次贝塞尔曲线取点");

    /* ---- tooth 6: C 与 Q 共存零互扰（词汇表扩充不改既有命令消费）。 ---- */
    n = kidpal_char_stroke_parse(
        "M 50 50 Q 100 20 150 50 C 200 80 250 80 300 50 L 350 50 Z", pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(n >= 13,
        csa_fm("M2K1-A1 RED: Q+C 混合路径解析返回 %d（须 >= 13 = M1 + Q8 + "
               "C>=2 + L1 + Z1）——C 加入后与 Q 共存互扰", n));
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 300.0f, 50.0f) &&
                                 csa_has_point_near(pts, n, 225.0f, 72.5f, 40.0f),
        "M2K1-A1 RED: Q+C 混合路径 C 段语义漂移——终点 (300,50) 不精确或 "
        "B(0.5)=(225,72.5) 附近零采样");

    /* ---- tooth 7: 十进制/负坐标 number（makemeahanzi graphics.txt 实况形状）。 */
    n = kidpal_char_stroke_parse("M -10.5 -20.25 C -5.5 -10 10.5 10 30.75 30.5",
                                 pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        csa_fm("M2K1-A1 RED: 十进制/负坐标 C 路径解析返回 %d——makemeahanzi 实况"
               "number 形状（小数+负值）未被 C 分支消费", n));
    TEST_ASSERT_TRUE_MESSAGE(csa_has_point_exact(pts, n, 30.75f, 30.5f),
        "M2K1-A1 RED: 十进制 C 终点 (30.75,30.5) 不精确——number 解析精度损失");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — M2K1-A2 既有四命令回归齿（behavioral, seam [2]）
 * M/L/Q/Z 解析点集与冻结参照表逐位一致（参照 = 修复前解析器 2026-09-18 实测
 * dump, 与二次贝塞尔公式解析值核对一致——「修复前」的定义即本表）; 防御面零
 * 回归（含未知命令 X 必须**保持** -1: C 加入不得放宽 tokenizer 拒绝语义）。
 * freeze 相位即 GREEN（钉住修复前行为, 回归守卫本性非假绿）; 修复后必须保持绿。
 * ------------------------------------------------------------------------ */
/* coverage: M2K1-A2 -- M/L/Q/Z 既有命令回归齿: 解析点集与修复前冻结参照表逐位一致（M/L/Z 精确 + Q 离散含隐式续接 + 多子路径 + Z 闭合 + 混合路径, C 命令加入零改四命令行为）+ 防御面零回归（未知命令保持 -1, C 加入不放宽拒绝语义） */
TEST_CASE("M/L/Q/Z 回归: 解析点集与修复前逐位一致+防御面零回归 (M2K1-A2)",
          "[char_stroke][M2K1-A2][regression]")
{
    kidpal_cs_point pts[512];
    int n;

    /* ---- 参照表 1: M/L/Z（逐点精确 + Z 闭合到子路径起点）。 ---- */
    static const float EXP_MLZ[][2] = {
        {20.0f, 20.0f}, {80.0f, 20.0f}, {80.0f, 80.0f}, {20.0f, 20.0f}
    };
    n = kidpal_char_stroke_parse("M 20 20 L 80 20 L 80 80 Z", pts, 512);
    csa_require_implemented(n, "kidpal_char_stroke_parse unimplemented/wired-missing");
    TEST_ASSERT_TRUE_MESSAGE(csa_pts_match(pts, n, EXP_MLZ, 4),
        csa_fm("M2K1-A2 RED: M/L/Z 路径点集漂移（got %d 点, 参照 4 点逐位）——"
               "C 命令加入改动了既有 M/L/Z 行为（char_stroke 数据面回归）", n));

    /* ---- 参照表 2: Q 二次贝塞尔离散（8 样本/段, 端点精确, CS_BEZIER_SEGS=8
     * 密度; 参照值与 B(t) 公式解析核对: t=1/8 -> (128,8)）。 ---- */
    static const float EXP_Q[][2] = {
        {0.0f, 0.0f}, {128.0f, 8.0f}, {256.0f, 32.0f}, {384.0f, 72.0f},
        {512.0f, 128.0f}, {640.0f, 200.0f}, {768.0f, 288.0f}, {896.0f, 392.0f},
        {1024.0f, 512.0f}
    };
    n = kidpal_char_stroke_parse("M 0 0 Q 512 0 1024 512", pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(csa_pts_match(pts, n, EXP_Q, 9),
        csa_fm("M2K1-A2 RED: Q 离散点集漂移（got %d 点, 参照 9 点逐位）——"
               "贝塞尔采样密度/公式/端点精确语义被触碰", n));

    /* ---- 参照表 3: Q 隐式续接（连续 number 组按上一命令重复, SVG 语义）。 ---- */
    static const float EXP_QCHAIN[][2] = {
        {0.0f, 0.0f}, {25.0f, 0.78125f}, {50.0f, 3.125f}, {75.0f, 7.03125f},
        {100.0f, 12.5f}, {125.0f, 19.53125f}, {150.0f, 28.125f},
        {175.0f, 38.28125f}, {200.0f, 50.0f}, {225.0f, 62.5f}, {250.0f, 75.0f},
        {275.0f, 87.5f}, {300.0f, 100.0f}, {325.0f, 112.5f}, {350.0f, 125.0f},
        {375.0f, 137.5f}, {400.0f, 150.0f}
    };
    n = kidpal_char_stroke_parse("M 0 0 Q 100 0 200 50 300 100 400 150", pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(csa_pts_match(pts, n, EXP_QCHAIN, 17),
        csa_fm("M2K1-A2 RED: Q 隐式续接点集漂移（got %d 点, 参照 17 点逐位）——"
               "连续 number 组按上一命令重复的 SVG 语义被触碰", n));

    /* ---- 参照表 4: 多子路径（第二个 M 复位子路径起点, Z 闭合到 (100,100)）。 */
    static const float EXP_MULTI[][2] = {
        {0.0f, 0.0f}, {10.0f, 0.0f}, {100.0f, 100.0f}, {112.5f, 100.78125f},
        {125.0f, 103.125f}, {137.5f, 107.03125f}, {150.0f, 112.5f},
        {162.5f, 119.53125f}, {175.0f, 128.125f}, {187.5f, 138.28125f},
        {200.0f, 150.0f}, {100.0f, 100.0f}
    };
    n = kidpal_char_stroke_parse("M 0 0 L 10 0 M 100 100 Q 150 100 200 150 Z",
                                 pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(csa_pts_match(pts, n, EXP_MULTI, 12),
        csa_fm("M2K1-A2 RED: 多子路径点集漂移（got %d 点, 参照 12 点逐位）——"
               "M 复位子路径起点 / Z 闭合目标被触碰（CHR-A4 孔洞闭合依赖）", n));

    /* ---- 参照表 5: M/L/Q/Z 全混合（char_page 真实消费的复合形状）。 ---- */
    static const float EXP_MIX[][2] = {
        {372.0f, 594.0f}, {500.0f, 500.0f}, {525.0f, 488.4375f},
        {550.0f, 478.75f}, {575.0f, 470.9375f}, {600.0f, 465.0f},
        {625.0f, 460.9375f}, {650.0f, 458.75f}, {675.0f, 458.4375f},
        {700.0f, 460.0f}, {800.0f, 480.0f}, {372.0f, 594.0f}
    };
    n = kidpal_char_stroke_parse("M 372 594 L 500 500 Q 600 450 700 460 L 800 480 Z",
                                 pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(csa_pts_match(pts, n, EXP_MIX, 12),
        csa_fm("M2K1-A2 RED: M/L/Q/Z 混合路径点集漂移（got %d 点, 参照 12 点"
               "逐位）——复合形状消费面回归", n));

    /* ---- 防御面零回归（-1 语义逐项; C 加入不得放宽任何一项）。 ---- */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_stroke_parse(NULL, pts, 512) == -1 &&
        kidpal_char_stroke_parse("", pts, 512) == -1 &&
        kidpal_char_stroke_parse("10 20 L 30 40", pts, 512) == -1 &&
        kidpal_char_stroke_parse("M 0", pts, 512) == -1 &&
        kidpal_char_stroke_parse("M 0 0", NULL, 512) == -1 &&
        kidpal_char_stroke_parse("M 0 0", pts, 0) == -1 &&
        kidpal_char_stroke_parse("M 0 0 L 1 1 L 2 2 L 3 3", pts, 3) == -1,
        "M2K1-A2 RED: 防御面漂移（NULL/空串/首 token 非命令/截断/NULL out/"
        "max_pts<=0/溢出 须恒 -1）——C 命令加入触碰了防御降级语义");

    /* tooth: 未知命令拒绝必须**保持**——C 加入后 tokenizer 未知字符/未知命令
     * 仍 -1（词汇表闭集扩充 ≠ 拒绝语义放宽）。 */
    n = kidpal_char_stroke_parse("M 0 0 X 5 5", pts, 512);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        csa_fm("M2K1-A2 RED: 未知命令 X 解析返回 %d（须 -1）——C 加入放宽了"
               "未知命令拒绝语义, 词汇表闭集被打开", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — M2K1-A3 栈修复结构齿（structural, seam [3]; 源级树扫, K4 test 7 先例
 * ——char_page.cc / custom_lcd_display.cc 设备装配面非 host 编译, 结构断言以
 * 真实工作树扫描承载; 「纯源级不可机判部分」的 structural 依据: 两文件不在
 * host 组件树（HOST_TEST_CONVENTIONS.md INF-E2 矩阵 device-only 侧）, host 无
 * 其对象码可探, 源级扫描是该面在本 runner 的唯一机判形态, mutation 证据兜底
 * 假绿风险）
 * RED today: 义务红设计态（大栈数组/parse 调用/裸字面/旧注记仍在现状树）,
 * builder 落地后 GREEN。
 * ------------------------------------------------------------------------ */
/* coverage: M2K1-A3 -- 栈修复结构齿: char_page.cc 动画帧路径（charpage_render_frame 区域）零 ~6.5KB 级栈局部数组（kidpal_cs_point [256] 非 static 局部 = 0）+ 零 kidpal_char_stroke_parse 帧内调用（预解析去栈）+ custom_lcd_display.cc task_stack 赋值非裸 16384 字面且栈定容值收编为命名常量 + 一次性补丁注记义务尾「本行届时随包收编」销账 */
TEST_CASE("栈修复结构: char_page 帧路径去大栈数组+taskLVGL 栈定容收编 (M2K1-A3)",
          "[char_stroke][M2K1-A3][structural]")
{
    char *region;
    int n;

    CSA_REQUIRE_ROOT();

    /* ---- tooth 1 (M2K1-A3a): 动画帧路径零 ~6.5KB 级栈局部数组——
     * charpage_render_frame 函数体区域内非 static 的 kidpal_cs_point [256]
     * 局部声明 = 0（现状 pts[256]/sc[256]×2 三处, 缺陷②核心; static/堆承载
     * 形态豁免——plan.md 改法取舍①允许「静态/预解析堆承载」）。 */
    region = csa_region_between(CSA_CHAR_PAGE,
                                "static void charpage_render_frame",
                                "charpage_blit(size);");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "CSA RED(infra): char_page.cc charpage_render_frame 区域切取失败（锚漂移"
        "——函数签名/charpage_blit 调用被改动须复核本齿锚）");
    n = csa_count_lines_with_both(region, "kidpal_cs_point", "[256]", "static");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        csa_fm("M2K1-A3 RED: char_page.cc 渲染帧路径仍有 %d 处非 static "
               "kidpal_cs_point [256] 栈局部数组（pts/sc 形态, 单帧 ~6KB 栈, "
               "打穿 taskLVGL 7168 默认栈 -> panic 的缺陷②本体）——去栈改造"
               "未落成", n));

    /* ---- tooth 2 (M2K1-A3b): 渲染帧路径零 kidpal_char_stroke_parse 调用
     * （预解析去栈——plan.md 改法取舍①机制面; parse 自身 toks[256] 2KB 栈
     * 随调用点移出帧路径而离场。若实现采用其它等价去栈形态, 须返 planner/
     * acceptance-author 复核返钉, 不得静默改齿）。 */
    n = csa_count_in_slice(region, "kidpal_char_stroke_parse(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        csa_fm("M2K1-A3 RED: 渲染帧路径仍有 %d 处 kidpal_char_stroke_parse 调用"
               "——逐笔逐帧重复解析未收敛为预解析（帧路径栈需求不随去栈改造"
               "下降, plan.md 改法取舍①机制面缺失）", n));
    free(region);

    /* ---- tooth 3 (M2K1-A3c): task_stack 赋值行不再是裸 16384 字面（现状
     * :131; 命名常量收编后赋值行引用常量名）。 */
    n = csa_count_lines_with_both(csa_read(CSA_BOARD_LCD), "task_stack", "16384", NULL);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        csa_fm("M2K1-A3 RED: custom_lcd_display.cc 仍有 %d 行 task_stack 赋值"
               "携带裸 16384 字面——2026-09-15 一次性实验补丁未收编为命名常量"
               "（plan.md 改法取舍②）", n));

    /* ---- tooth 4 (M2K1-A3c): 栈定容值 16384 收编为命名常量（同文件内含
     * STACK 名字面 + 16384 值的定义行, #define/constexpr 形态均可; 常量 home =
     * custom_lcd_display.cc——plan.md 改法取舍②）。 */
    {
        char *buf = csa_read(CSA_BOARD_LCD);
        int hit = 0;
        if (buf != NULL) {
            const char *line = buf;
            while (*line != '\0' && !hit) {
                const char *eol = strchr(line, '\n');
                size_t len = eol ? (size_t)(eol - line) : strlen(line);
                char tmp[CSA_MAX_PATH];
                size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;
                memcpy(tmp, line, cp);
                tmp[cp] = '\0';
                if (strstr(tmp, "16384") != NULL &&
                    csa_caseci_contains(tmp, "stack")) {
                    hit = 1;
                }
                if (eol == NULL) {
                    break;
                }
                line = eol + 1;
            }
            free(buf);
        }
        TEST_ASSERT_TRUE_MESSAGE(hit,
            "M2K1-A3 RED: custom_lcd_display.cc 零「STACK + 16384」命名常量定义"
            "行——栈定容值未收编为命名常量（裸字面拆除但定容依据没有单源落点）");
    }

    /* ---- tooth 5 (M2K1-A3d): 一次性补丁注记义务尾销账——旧注记「本行届时随包
     * 收编」义务 marker 不再在世（注记改写为正式定容依据; 新注记措辞不锁,
     * 以义务尾销账承载「一次性补丁注记替换」语义）。 */
    n = csa_count_in_file(CSA_BOARD_LCD, "本行届时随包收编");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        csa_fm("M2K1-A3 RED: custom_lcd_display.cc 仍有 %d 处「本行届时随包收编」"
               "义务注记——一次性实验补丁的收编义务未销账（该注记自称随包收编, "
               "本包即其承接包）", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — boundary：注册接线守卫 + 契约面/spec 面 canary + 扫描器自检
 *   （无 acceptance marker——非 acceptance 条目, M2K1-E1 suite 级判读的结构
 *    前置 + E2 铁律 canary; 防套件级假绿: 本文件未接线则三齿在 runner 全量里
 *    静默缺席, E1「零新增红」失去判读对象）
 * ------------------------------------------------------------------------ */
TEST_CASE("boundary: 注册接线守卫+契约spec canary+扫描器自检 (M2K1-E1/E2)",
          "[char_stroke][boundary][wiring]")
{
    char *mk;
    char *region;
    int n;

    CSA_REQUIRE_ROOT();

    /* tooth 1: 本文件 + 被测实现 char_stroke.c 须接线进 HOST_ACCEPTANCE_SRCS,
     * 且排在 host_infra_smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿
     * 注册序守卫）。RED until builder wires. */
    mk = csa_read(CSA_CMAKELISTS);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "CSA RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "char_stroke.acceptance.c");
        const char *impl = strstr(mk, "display/char_stroke.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "M2K1-E1 RED: char_stroke.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— host 验收构建未接本包（BUILDER DUTY: 见本文件头注 [BUILDER DUTY]）");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "M2K1-E1 RED: display/char_stroke.c 未接线进 HOST_ACCEPTANCE_SRCS — "
            "强符号源缺失（weak 哨兵永不被覆盖, test 1/2 恒红 not-wired）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "CSA RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失 — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke && impl < smoke,
            "M2K1-E1 RED: 本包两行必须排在 host_infra_smoke.acceptance.c 之前 "
            "(smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2: 契约面/spec 面在世 canary（M2K1-E2 铁律: 本包零 docs 面改动——
     * C 命令属 D3 strokes makemeahanzi SVG path 自由文本词汇表内解析补全, 非契约
     * 改动; 发现冲突走 spec-lane 逃生门, 不预先改 docs）。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_exists(CSA_CONTRACT),
        "M2K1-E2 RED(infra): docs/contracts/interface-contract.md 丢失——契约面"
        "被触碰（本包零契约触碰红线）");
    TEST_ASSERT_TRUE_MESSAGE(csa_exists(CSA_REQUIREMENTS),
        "M2K1-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰（S-F3.1 "
        "可行性注记 = 缺陷登记权威, 本包零修订）");
    TEST_ASSERT_TRUE_MESSAGE(csa_exists(CSA_CHAR_STROKE_H),
        "CSA RED(infra): display/char_stroke.h 丢失——被测面契约头被拆");

    /* tooth 3: 扫描器自检（防假绿: 结构齿负向必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_exists(CSA_CHAR_PAGE),
        "CSA RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(csa_exists("firmware/main/__csa_no_such__.cc"),
        "CSA RED(infra): scanner reports a nonexistent file as present");
    n = csa_count_in_file(CSA_CHAR_STROKE_H, "kidpal_char_stroke_parse(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "CSA RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    region = csa_region_between(CSA_CHAR_PAGE,
                                "static void charpage_render_frame",
                                "charpage_blit(size);");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "CSA RED(infra): region slicer cannot slice a known region — 切区器失灵");
    /* 稳定锚: 三态分类调用在去栈改造后仍在帧路径内（不被本包修复移除）,
     * 不能用 parse 调用当自检锚——它正是 tooth 2 要归零的对象。 */
    TEST_ASSERT_TRUE_MESSAGE(csa_count_in_slice(region, "kidpal_char_stroke_state(") >= 1,
        "CSA RED(infra): region slicer lost a known literal — 切区器计数失灵"
        "（现状树帧路径含三态分类调用, 切区器应能数到）");
    free(region);
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — MCF-A2 meaning 承载 seam 结构齿（structural; seam [5];
 * 260920-m2-charwindow-card-fix additive。char_stroke.{h,c} 强定义 home 均在
 * host runner 在册——本齿以源级树扫承载 freeze 相位红, builder 落地后 GREEN;
 * 正向计数全部注释剥离后计数——sealing pass 纪律, 注释无法满足）
 * RED today（设计态）: 头零 meaning 字段 / 零探针宏 / card_build 零 meaning 消费。
 * ------------------------------------------------------------------------ */
/* coverage: MCF-A2 -- meaning 承载结构半: kidpal_char_info 增 const char *meaning（NULL 容忍, 不进拒绝集）+ kidpal_char_card 增 char meaning[96] + 编译期探针宏 KIDPAL_CHAR_STROKE_HAS_MEANING（行为齿编译门, 漏定义由本齿咬住）+ card_build 函数区 meaning 消费在世（NULL 容忍分支 + 拷贝分支 >= 2; 注释剥离后计数） */
TEST_CASE("meaning 承载 seam 结构: 头双字段+探针宏+card_build 消费 (MCF-A2)",
          "[char_stroke][MCF-A2][structural]")
{
    char *region;

    CSA_REQUIRE_ROOT();

    /* tooth 1: kidpal_char_info 增 const char *meaning（末位可选面, NULL 容忍）。 */
    region = csa_region_between(CSA_CHAR_STROKE_H, "} kidpal_cs_point;",
                                "} kidpal_char_info;");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "MCF-A2 RED(infra): char_stroke.h 的 kidpal_char_info 结构区切取失败（锚漂移"
        "——须复核本齿锚, 不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(csa_count_in_slice_code(region, "meaning") >= 1,
        "MCF-A2 RED: kidpal_char_info 零 meaning 成员——释义承载字段缺失（S-F3.2 AC1 "
        "v1.56 释义行的数据面 seam; plan.md 改动面 1, NULL 容忍不进拒绝集; 注释剥离后"
        "计数）");
    free(region);

    /* tooth 2: kidpal_char_card 增 char meaning[96]（容量对齐 words 惯例）。 */
    region = csa_region_between(CSA_CHAR_STROKE_H, "} kidpal_char_info;",
                                "} kidpal_char_card;");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "MCF-A2 RED(infra): char_stroke.h 的 kidpal_char_card 结构区切取失败（锚漂移）");
    TEST_ASSERT_TRUE_MESSAGE(csa_count_in_slice_code(region, "meaning") >= 1,
        "MCF-A2 RED: kidpal_char_card 零 meaning 缓冲——速查卡释义装配无落点"
        "（容量对齐 words 惯例 char meaning[96]; 注释剥离后计数）");
    free(region);

    /* tooth 3: 编译期探针宏（冻结 seam [5] 同步形态）——行为齿 test 6 的编译门;
     * 漏定义则行为齿静默缺席编译 = 套件级假绿缝隙, 本齿咬住。 */
    TEST_ASSERT_TRUE_MESSAGE(
        csa_count_in_file_code(CSA_CHAR_STROKE_H, "KIDPAL_CHAR_STROKE_HAS_MEANING") >= 1,
        "MCF-A2 RED: char_stroke.h 零 KIDPAL_CHAR_STROKE_HAS_MEANING 探针宏——MCF-A2 "
        "行为齿的编译期 seam 缺失（冻结 seam [5] 同步形态: 头落 meaning 字段时同步 "
        "#define KIDPAL_CHAR_STROKE_HAS_MEANING 1; 漏定义则行为齿永不编译）");

    /* tooth 4: card_build 函数区 meaning 消费在世（NULL 容忍分支 + verbatim 拷贝
     * 分支 >= 2 处; 注释剥离后计数——注释提及不算实现, builder M4 教训）。 */
    region = csa_region_between(CSA_CHAR_STROKE_C, "int kidpal_char_card_build(",
                                "int kidpal_char_data_validate(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "MCF-A2 RED(infra): char_stroke.c 的 card_build 函数区切取失败（签名锚漂移）");
    TEST_ASSERT_TRUE_MESSAGE(csa_count_in_slice_code(region, "meaning") >= 2,
        "MCF-A2 RED: kidpal_char_card_build 函数区零/弱 meaning 消费——释义装配未落地"
        "（NULL 容忍分支 + verbatim 截断安全拷贝分支 >= 2 处; 注释剥离后计数）");
    free(region);
}

#if KIDPAL_CHAR_STROKE_HAS_MEANING
/* -------------------------------------------------------------------------- *
 * TEST 6 — MCF-A2 meaning 承载行为齿（behavioral, 强定义 char_stroke.c 已在册;
 * 260920-m2-charwindow-card-fix additive。**编译期探针门控**: seam 未落时本齿缺席
 * 编译, 由 test 5 结构齿承载红——非空过）
 * 三态: 非空 verbatim + 截断安全 / NULL -> 空串 rc=0（缺义不降级整窗——human 裁
 * 降级口径核心）/ 既有四字段 + progress 文案零回归（M2-K1 冻结面共存）。
 * ------------------------------------------------------------------------ */
/* coverage: MCF-A2 -- meaning 承载行为半（编译期探针门控）: 非空 verbatim + 截断安全（>96 字节 -> 95 + NUL 恒不越界）/ NULL -> out->meaning 空串且 rc=0（缺义不降级整窗, meaning 不进 NULL 拒绝集）/ 既有四字段与 progress 文案零回归（M2-K1 冻结面） */
TEST_CASE("meaning 承载行为: verbatim+截断安全+NULL 容忍不降级 (MCF-A2)",
          "[char_stroke][MCF-A2][behavior]")
{
    kidpal_char_info info;
    kidpal_char_card card;
    char big[160];

    /* ---- state 1: 非空 verbatim + 与既有四字段共存零回归 -------------------- */
    memset(&info, 0, sizeof(info));
    info.pinyin = "yǒng";
    info.radical = "力";
    info.stroke_count = 9;
    info.words = "勇敢、勇气";
    info.meaning = "勇敢；有胆量";
    memset(&card, 0, sizeof(card));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == 0,
        "MCF-A2 RED: 非空 meaning 装配 rc != 0——card_build 拒收了合法装配面");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.meaning, "勇敢；有胆量") == 0,
        csa_fm("MCF-A2 RED: card.meaning=\"%s\" 期望 verbatim \"勇敢；有胆量\"——释义被"
               "改写/截断/丢弃（S-F3.2 AC1 v1.56 释义行数据面）", card.meaning));
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.pinyin, "yǒng") == 0 &&
                                 strcmp(card.radical, "力") == 0 &&
                                 strcmp(card.stroke_total, "共 9 画") == 0 &&
                                 strcmp(card.words, "勇敢、勇气") == 0 &&
                                 strcmp(card.progress, "写完啦·共 9 笔") == 0,
        csa_fm("MCF-A2 RED: meaning 加入后既有四字段/progress 漂移 (pinyin=%s, "
               "stroke_total=%s, progress=%s)——M2-K1 冻结面回归（参照表守卫语义同源）",
               card.pinyin, card.stroke_total, card.progress));

    /* ---- state 2: 截断安全（>96 字节 -> 95 + NUL, 恒不越界）------------------ */
    memset(big, 'a', sizeof(big));
    big[sizeof(big) - 1] = '\0';
    info.meaning = big;
    memset(&card, 0, sizeof(card));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == 0,
        "MCF-A2 RED: 超长 meaning 装配 rc != 0——截断路径被当作失败");
    TEST_ASSERT_TRUE_MESSAGE(strlen(card.meaning) == 95 && card.meaning[95] == '\0',
        csa_fm("MCF-A2 RED: 超长 meaning 截断后长度 %zu（期望 95 + NUL）——容量 "
               "meaning[96] 的截断安全语义缺失（越界/不截断/未恒 NUL 结尾）",
               strlen(card.meaning)));

    /* ---- state 3: NULL -> 空串 + rc=0（缺义不降级整窗——human 裁降级口径核心）-- */
    info.meaning = NULL;
    memset(&card, 0, sizeof(card));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == 0,
        "MCF-A2 RED: meaning=NULL 装配 rc != 0——缺义被降级整窗（human 裁: 缺释义只损"
        "右栏一行, meaning 不进 NULL 拒绝集; 旧 payload 整窗降级 = 行为回归）");
    TEST_ASSERT_TRUE_MESSAGE(card.meaning[0] == '\0',
        csa_fm("MCF-A2 RED: meaning=NULL 时 card.meaning=\"%s\" 非空串——缺义容忍语义"
               "缺失（「释义：—」兜底占位归 char_page 呈现面, 数据面须空串）",
               card.meaning));
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.words, "勇敢、勇气") == 0 &&
                                 strcmp(card.progress, "写完啦·共 9 笔") == 0,
        "MCF-A2 RED: meaning=NULL 时其余字段未照常装配——缺义容忍路径连带损了右栏"
        "其余行（缺义只损一行, 不降级整窗）");
}
#endif /* KIDPAL_CHAR_STROKE_HAS_MEANING */
