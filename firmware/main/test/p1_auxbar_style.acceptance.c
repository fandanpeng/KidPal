/*
 * Frozen acceptance tests — TaskPackage 261015-p1-auxbar-style-fix
 * (subsystem: firmware/main/display（home_p1.c 辅助条四观感件）+ 资产管线; rigor
 * lite; sourceType human-ad-hoc; riskTier low; P1 首页辅助条像素级观感修复——
 * human 原话「首页 最上面那一条，整体看起来很不精致」)。
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1——日期英文简写示例锚「Tue 22 Sep」）。
 * 语义权威 = docs/requirements.md 现文 S-F1.1 节; 行为权威 = reviewed plan.md
 * （architect /triage-change 分流 review 2026-10-15, frontmatter reviewed-by 为证;
 * human 触点①三决已裁: ②单包 Lite / Tier-2④a 日期全大写直接做 / Tier-2④b 冒号眨眼
 * 不做出包——「那是时间，咋能不变呢」）。非新语义包: 像素级观感修复, 不改需求语义。
 *
 * Nature: HOST-SIDE STRUCTURAL（源锚齿 tree-scan, MCD-A1 形态先例 =
 * voice_rightcol_label.acceptance.c 261015-voice 同款; 助手族自包含复制、前缀 abx_、
 * static 链接不与先例文件符号冲突）。home_p1.c 设备装配面非 host 编译——源级树扫
 * 承载。正向齿只认**注释剥离后可执行源**的字面实参（260917-m2-k3 sealing pass 教训:
 * 只出现在注释里的锚值会假绿, 注释无法满足正向齿）; 反证销账齿走**裸源引号字面**
 * 计数（资产消费名以引号字符串字面量形态只出现在可执行源, 注释提及不带引号不带
 * .bin 后缀——实测核对 home_p1.c :601/:1037/:1067/:1100 四处注释均无引号形态）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements; 本文件只读扫工作树）:
 *
 *   [1] 电池图标换原生直出件（AB-A1）: home_p1.c p1_draw_aux_batt 消费
 *       ic_batt_15.bin（15x12 原生, gen_assets ic_batt_15 job w:15 h:12, 已在
 *       spiffs_root）直出不缩放——p1_blit_art(file,x,y,w,h) 或
 *       p1_blit_art_scaled(file,x,y,dw,dh,…) 且 dw/dh=原生 (15,12) 皆可（plan ①
 *       双通道授权）。落位 = 图标半区冻结钉区 HPB_RECT_AUX_BATT_ICON {342,6,362,29}
 *       （权威 = p1_home_battery.acceptance.c 钉区表, plan 底缘 33 措辞漂移已由 1g
 *       快照核验裁定以钉区表 y2=29 为准）内居中、墨带中线 y≈19（与文字 11px 高同
 *       视觉级）。弃用 22x40 非等比盒, "ic_batt_48.bin" 引号字面消费销账（裸源
 *       计数 = 0）。art 未挂载兜底描边保持钉区内（HPB 兜底义务由
 *       p1_home_battery.acceptance.c:101-104 既有齿承载, 本文件零重锚不重复钉）。
 *       落位授权域推导（不钉单一魔数, MCD-A1 先例）: 半区 x 中心 = (342+362)/2 =
 *       352, 图标 dw=15 → x0 = 344.5 → 字面取整 344/345, 授权 ±3px 取整让位 →
 *       x0 ∈ [342,347]; 纵向墨带中线 19±2px → y0 = 19 - dh/2 = 13 → y0 ∈ [11,15]
 *       （dh=12）。x0 ∈ [342,347] ∧ y0 ∈ [11,15] ∧ (dw,dh)=(15,12) 蕴含墨盒
 *       x ≤ 361 ∧ y ≤ 26, 收进钉区 {342,6,362,29} 内——落位即 contained。
 *
 *   [2] 温度图标换原生直出件（AB-A2）: p1_draw_aux_temp 消费 ic_thermo_12.bin
 *       （12x12 原生）直出不缩放, 半区 HPB_RECT_AUX_TEMP_ICON {210,6,230,29} 内
 *       居中、中线 y≈19。"thermo_man.bin" 引号字面消费销账（41x67→20x20 最近邻
 *       硬压路径退役; 资产本体仍在 gen_art_spiffs 白名单 P1 既有 7 组, 非销账
 *       对象）。授权域: x0 ∈ [211,217]（中心 220, 214 取整 ±3）; y0 ∈ [11,15]。
 *       兜底同 [1] 纪律。
 *
 *   [3] 湿度图标换原生直出件（AB-A3）: p1_draw_aux_hum 消费 ic_drop_12.bin
 *       （12x12 原生）直出不缩放, 半区 HPB_RECT_AUX_HUM_ICON {282,6,302,29} 内
 *       居中、中线 y≈19。"drop_baby.bin" 引号字面消费销账（32x51→20x20 硬压路径
 *       退役）。授权域: x0 ∈ [283,289]（中心 292, 286 取整 ±3）; y0 ∈ [11,15]。
 *       兜底同 [1] 纪律。
 *
 *   [4] wifi 资产重出守卫齿（AB-A4; **设计态绿**——同名同尺寸重出在 HEAD 无
 *       pre-change 状态差, 变异取证豁免面 per TaskPackage mutationEvidence.reason）:
 *       ① gen_assets.mjs ic-wifi job 同行锚在世（'ic_wifi_14' 与 svgUse('ic-wifi')
 *       同行）且输出 (w,h)=(14,14)——同名同尺寸 14x14 重出义务, 白名单 68 枚不动、
 *       验收零重锚; ② 母版 docs/ui/kidpal-ui-design-v5.html id="ic-wifi" symbol
 *       在世（母版 symbol 面守卫——内容级重设终形归看屏轮 human 拍板, **本齿不钉
 *       symbol 内容**, 钉内容即抢跑 human 仲裁面）; ③ home_p1.c ic_wifi_14.bin
 *       消费几何 (12,12,14,14) 零重锚守卫（该齿兼扫描器自检——取值器对已知在世
 *       形态能命中, 失灵则 [1][2][3] 落位齿失去咬合力）。
 *
 *   [5] 日期简写全大写（AB-A5; Tier-2④a human 触点①拍板「直接做」纳入）:
 *       home_p1.c k_p1_month_abbr / k_p1_wday_abbr 表串全大写（"Tue 22 Sep" →
 *       "TUE 22 SEP"）——规避 12x16 降采样小写笔画侵蚀与大小写混排参差。断言形态
 *       = 逐 token 全大写（非钉死 JAN..DEC 具体串——大写约束是设计决策, 具体
 *       拼写已由表名/表长钉死）+ 非空表串数（月 12 / 星期 7）在世（防清空表体
 *       假绿——空表全大写断言空过）。结构齿（表长 13/7 取值）兼扫描器自检。
 *       requirements AC1 示例措辞全大写同步 = 殿后项（human 触点①拍板, 档 0
 *       勘误或 human 直改, 不在本齿扫描域）。
 *
 *   [6] ④b 负面锚 + 零触碰守卫（AB-A6; **设计态绿**——human 拍板不做, HEAD 无
 *       pre-change 状态差）: p1_draw_aux_time 冒号眨眼门控零触碰——
 *       colon_visible 赋值（kidpal_home_p1_blink_visible(s_state.tick_ms,
 *       HP1_BLINK_PERIOD_COLON_MS)）在世且仍门控 blit（colon_visible 在函数窗内
 *       出现 ≥ 2 次 = 赋值 + 消费）; home_p1.h HP1_BLINK_PERIOD_COLON_MS 常量
 *       定义行在世且值 4400 同行（line 级双针, ④b 不做 = 周期退役授权不存在）;
 *       kidpal_home_p1_blink_visible 函数定义本体在世（blink_visible 符号钉
 *       p1_cjk_name:1342 / HPB-A4 活齿 :1047-1056 的依赖面）。p1_home_battery /
 *       p1_cjk_name / time_sync / voice_rightcol_label 四冻结件零改动全绿 = host
 *       套件回归观察面（红基线/终跑判读承载, 落齿即越权复钉既有冻结件断言面,
 *       voice_rightcol VR-A5 同款口径）。
 *
 *   [7] Builder FIRST ACTION（本文件进 host runner 的唯一前置; 接线行**不在
 *       acceptance-author 写权内**, scope-map 分权——TaskPackage changeManifest
 *       预指派 builder 代落）: 向 firmware/main/test/host_app/main/
 *       CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 追加一行:
 *         "../../p1_auxbar_style.acceptance.c"
 *       置于 "../../voice_rightcol_label.acceptance.c" 之后、
 *       "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, 包 0 boundary 齿注册序守卫, 既有惯例）。本文件纯树扫、零被测实现
 *       接线（home_p1.c 不进 host 构建）。
 *
 *   [8] 冻结授权链: TaskPackage 261015-p1-auxbar-style-fix（acceptance[]
 *       AB-A1~A7 + mutationEvidence 骨架为冻结边界; plan.md reviewed + human
 *       触点①三决）。本文件冻结后 builder/fixer tighten-only（纯 additive 补
 *       断言/修自身 fixture; 禁放宽、禁改断言语义）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: AB-A1 -- 电池图标 ic_batt_15 原生直出 + 落位授权域 + ic_batt_48
 *             销账（test 1）
 *   coverage: AB-A2 -- 温度图标 ic_thermo_12 原生直出 + 落位授权域 +
 *             thermo_man 销账（test 2）
 *   coverage: AB-A3 -- 湿度图标 ic_drop_12 原生直出 + 落位授权域 + drop_baby
 *             销账（test 3）
 *   coverage: AB-A4 -- wifi 同名同尺寸重出守卫三齿 + 扫描器自检（test 4; 设计态
 *             绿——pre-change 状态差不存在, 变异取证豁免面）
 *   coverage: AB-A5 -- 日期简写全大写表串齿 + 表长结构自检（test 5）
 *   coverage: AB-A6 -- 冒号眨眼门控零触碰负面锚三齿（test 6; 设计态绿——④b
 *             拍板不做）; 四冻结件回归观察面不落齿（见 [6] 尾段）
 *   coverage: AB-A7 -- 真机轮 live-manual structural 豁免（VR-A6 261015-voice /
 *             TR-A5 261013 先例口径, 不计入 runner 通过面）, 不落 C 齿: human
 *             持板目测——① wifi 不再读作「两朵重叠」; ② 电池/温/湿图标与文字同
 *             视觉级（墨高约 11-15px、中线 y≈19、无硬切感）; ③ 日期全大写观感
 *             确认; ④ 烧录 = app+art 双分区（spiffs 资产变更, 261011 先例）。
 *             host 面转绿后停该门等 human, 不自行烧录。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder wiring 见 [7]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加）
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf   （elf 相对 host_app/ 目录）
 *   本文件另可独立冒烟（red-baseline 独立执行架先例, 冻结相位实编 + 咬合力取证用）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -I. -o /tmp/pas_red_runner ../../p1_auxbar_style.acceptance.c runner.c
 *     cd ../../../../..  (repo root);  /tmp/pas_red_runner
 *
 * RED baseline（freeze 相位预期申报——净 HEAD: 消费仍为 ic_batt_48/thermo_man/
 * drop_baby, 缩写表仍 title-case; 冻结落盘时已按「冻结即冒烟/§4 C 类实编」以独立
 * 冒烟通道实编 + 实跑一次, 红名 verbatim 见交付报告）:
 *   - test 1/2/3（AB-A1/A2/A3）: 设计态红, 首发红名要点 = "AB-A? RED: home_p1.c
 *     p1_draw_aux_* 未取到 ic_* 原生直出件消费点"（新消费名在世断言红; 第二枚
 *     销账齿同相位红, 被 unity 短路不打印——presence 先 fire）。若修后红名变为
 *     「未取到…字面消费实参」/ 落位授权域红 = 锚漂移或非字面形态, 属 infra 级
 *     异常须回查, 不得改判为绿。
 *   - test 5（AB-A5）: 设计态红, 首发红名要点 = "AB-A5 RED: k_p1_month_abbr 月
 *     简写表串含小写字母（首犯 \"Jan\"）"（title-case 在世; 星期表齿同相位红,
 *     首犯 "Mon"）。
 *   - test 4/6（AB-A4/A6 守卫齿）: 设计态绿（同 [4][6] 头注——pre-change 状态差
 *     不存在; 红基线判据 = 消费名/表串四类设计态红, 守卫齿转红即 infra 异常）。
 *   - 既有套件（p1_home_battery / p1_cjk_name / time_sync / voice_rightcol_label
 *     等）: 全绿零新增红（AB-A6 回归观察面; 上游基线 = voice 收尾态 172T/0F）。
 *   - 转绿判据: builder 三处消费切换 + 销账 + 表串全大写后 6 齿全绿; 任一设计态
 *     红齿仍红而消费名/表串已切换 = 锚漂移, 复核本文件锚, 不得改判为绿。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

#define ABX_MAX_PATH 1024
#define ABX_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根; 行号快照非长期事实源, 齿锚全部用符号/字面锚）。 */
#define ABX_HOME_P1_C  "firmware/main/display/home_p1.c"
#define ABX_HOME_P1_H  "firmware/main/display/home_p1.h"
#define ABX_GEN_ASSETS "tools/asset-pipeline/gen_assets.mjs"
#define ABX_MASTER_HTML "docs/ui/kidpal-ui-design-v5.html"

/* 函数窗: 锚定被测函数符号后的扫描窗（禁行号钉点的函数级绑定位; 窗宽 = 函数体
 * 量级 2 倍以上, 与相邻函数粘连属预期——窗内只作 needle 在场判定, 不作唯一性
 * 判定, 粘连不误红）。 */
#define ABX_FN_WINDOW 3000
#define ABX_TIME_WINDOW 1500

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（自包含复制自 voice_rightcol_label.acceptance.c 的
 * vrx_* 助手族, 精简/增补为本文件所需子集; 全 static 链接, 命名前缀 abx_ 不与
 * 先例冲突）。全部扫描跑在真实工作树上, root 以手持 cwd 定位（仓根 /
 * firmware/main/test/host_app 两处都可跑——walk-up 定根纪律）。
 * ------------------------------------------------------------------------ */

static char abx_root[ABX_MAX_PATH];
static int abx_root_ready;

static int abx_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int abx_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[ABX_MAX_PATH];
    size_t i;

    if (abx_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (abx_is_regular(probe)) {
            snprintf(abx_root, sizeof(abx_root), "%s", cands[i]);
            abx_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int abx_rel(char *out, size_t cap, const char *rel)
{
    if (!abx_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", abx_root, rel);
    return 1;
}

static char *abx_read(const char *rel, size_t *out_len)
{
    char p[ABX_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!abx_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > ABX_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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
 * passed to the single-string assert macros（KLD/CWX/VRL 先例同款）。 */
static char abx_fm_buf[1024];
static const char *abx_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *abx_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(abx_fm_buf, sizeof(abx_fm_buf), fmt, ap);
    va_end(ap);
    return abx_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Comment-stripping scanner support（260917-m2-k3 sealing pass / VRL 同款）:
 * 正向齿只被**可执行源**满足, 注释字节抹成空格; 字符串/字符字面量体保留（跳过
 * "..." / '...' 体, 不把字面量里的 // 误判成注释——资产名引号字面因此不受剥注释
 * 影响）。保持缓冲长度（注释字节 -> 空格）, 调用方负责 free。
 * ------------------------------------------------------------------------ */

static char *abx_strip_c_comments(const char *src)
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

static char *abx_strip_c_comments_file(const char *rel)
{
    char *buf = abx_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = abx_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（函数窗内 needle 在场判定）。返回窗内首个命中位, 未命中 -> NULL。 */
static const char *abx_find_in(const char *hay, const char *end, const char *needle)
{
    size_t nl;

    if (hay == NULL || needle == NULL || end == NULL || hay >= end) {
        return NULL;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return hay;
    }
    for (; hay + nl <= end; hay++) {
        if (hay[0] == needle[0] && memcmp(hay, needle, nl) == 0) {
            return hay;
        }
    }
    return NULL;
}

/* 有界整数提取: [p, end) 内、首个 ')' 或 ';' 前的带符号十进制字面量序（VRL
 * vrx_ints_after 有界变体——消费几何实参须为**字面整数**书写, 宏间接/符号常量
 * 形态取值失败 = 红, 不改判）。返回写入 out 的个数。 */
static int abx_ints_between(const char *p, const char *end, long *out, int max_args)
{
    int n = 0;

    if (p == NULL || end == NULL || out == NULL || max_args <= 0) {
        return 0;
    }
    while (p < end && n < max_args) {
        if (*p == ')' || *p == ';') {
            break;
        }
        if (*p == '-' || (*p >= '0' && *p <= '9')) {
            char *e = NULL;
            long v = strtol(p, &e, 10);
            if (e == p) {
                break;
            }
            out[n++] = v;
            p = e;
            continue;
        }
        p++;
    }
    return n;
}

/* 裸源子串出现计数（销账负向齿专用——引号字面形态只出现在可执行源, 不剥注释;
 * 注释提及不带引号, 天然不误红, sealing pass 纪律的引号字面等价形态）。 */
static int abx_count_sub(const char *hay, const char *needle)
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

/* 行级判定（raw 不剥注释, VRL 同款）: slice 内同时含 needle1 与 needle2 的行数。 */
static int abx_count_lines_with_both(const char *slice, const char *needle1,
                                     const char *needle2)
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
        char tmp[ABX_MAX_PATH];
        size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;

        memcpy(tmp, line, cp);
        tmp[cp] = '\0';
        if (strstr(tmp, needle1) != NULL && strstr(tmp, needle2) != NULL) {
            count++;
        }
        if (eol == NULL) {
            break;
        }
        line = eol + 1;
    }
    return count;
}

/* 表串提取: 从锚位起找 '{', 收集至 '}' 前的全部 "..." 字面量（缩写表体量 ≤ 7
 * 字符/token; 防御性截断）。返回收集个数。 */
static int abx_strings_in_braces(const char *anchor_pos, char toks[][8], int max_toks)
{
    const char *p;
    int n = 0;

    if (anchor_pos == NULL || toks == NULL || max_toks <= 0) {
        return 0;
    }
    p = strchr(anchor_pos, '{');
    if (p == NULL) {
        return 0;
    }
    p++;
    while (*p != '\0' && *p != '}' && n < max_toks) {
        if (*p == '"') {
            size_t i = 0;
            p++;
            while (*p != '\0' && *p != '"') {
                if (i < 7) {
                    toks[n][i++] = *p;
                }
                p++;
            }
            toks[n][i] = '\0';
            n++;
            if (*p == '"') {
                p++;
            }
            continue;
        }
        p++;
    }
    return n;
}

/* 全大写判定（空串 = 真——空 token 只合法出现在月表 index 0, 数量义务另钉）。 */
static int abx_all_upper_or_empty(const char *s)
{
    for (; *s != '\0'; s++) {
        if (*s < 'A' || *s > 'Z') {
            return 0;
        }
    }
    return 1;
}

#define ABX_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(abx_locate_root(), \
    "AB-Ax RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* -------------------------------------------------------------------------- *
 * TEST 1 — AB-A1 电池图标源锚齿: p1_draw_aux_batt 消费 ic_batt_15.bin 原生
 * 15x12 直出不缩放 + 图标半区 {342,6,362,29} 内居中/中线 y≈19 + "ic_batt_48.bin"
 * 引号字面销账。
 * RED today（设计态）: 消费仍为 ic_batt_48.bin（:1104 快照位, 22x40 非等比盒）,
 * ic_batt_15.bin 消费点不在世 -> 首发红 = presence; 销账齿同相位红（被 unity
 * 短路不打印）。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A1 -- 电池图标换原生直出件: home_p1.c p1_draw_aux_batt 消费 ic_batt_15.bin（15x12 原生）直出不缩放（p1_blit_art 或 p1_blit_art_scaled 且 dw/dh=(15,12) 原生——plan ① 双通道授权）, 图标半区 HPB_RECT_AUX_BATT_ICON {342,6,362,29} 内居中、墨带中线 y≈19（授权域 x0∈[342,347] y0∈[11,15], 蕴含墨盒收进钉区）; "ic_batt_48.bin" 引号字面消费销账 = 0（裸源计数——22x40 非等比盒路径退役）; art 未挂载兜底描边保持钉区内（HPB 兜底义务由 p1_home_battery.acceptance.c:101-104 既有齿承载, 零重锚） */
TEST_CASE("P1 辅助条电池图标源锚: ic_batt_15 原生直出 + ic_batt_48 销账 (AB-A1)",
          "[p1_auxbar_style][AB-A1][structural]")
{
    char *code;
    char *raw;
    const char *fn;
    const char *fn_end;
    const char *hit;
    long a[16];
    int n;
    int old_n;

    ABX_REQUIRE_ROOT();

    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A1 RED(infra): home_p1.c unreadable");

    /* tooth 1 (函数锚在世): p1_draw_aux_batt 定义不在世 = 锚漂移/函数被删。 */
    fn = strstr(code, "p1_draw_aux_batt");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "AB-A1 RED(infra): home_p1.c p1_draw_aux_batt 符号缺席——函数被删/改名, "
        "须复核锚不得改判为绿");
    fn_end = fn + ABX_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }

    /* tooth 2 (本包设计态红齿, presence): 原生直出件消费点在场（函数窗绑定——
     * 消费须落在 p1_draw_aux_batt 函数窗内, 挪去其它函数 = 红(infra) 重钉）。 */
    hit = abx_find_in(fn, fn_end, "\"ic_batt_15.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A1 RED: home_p1.c p1_draw_aux_batt 未取到 ic_batt_15.bin 消费点（15x12 "
        "原生直出件未接入）——电池图标仍走 ic_batt_48 48x48 塞 22x40 非等比盒旧路径"
        "（横压 2.18x + y0=0 clip 硬切, 全条最重最黑）; 修复 = 消费切换 ic_batt_48.bin"
        " -> ic_batt_15.bin 原生 15x12 直出不缩放");

    /* tooth 3 (原生几何): dw/dh = (15, 12) 字面（原生直出不缩放义务; 双通道调用
     * 形态前四参同形 (x0, y0, w, h)）。 */
    n = abx_ints_between(hit + strlen("\"ic_batt_15.bin\""), fn_end, a, 16);
    TEST_ASSERT_TRUE_MESSAGE(n >= 4 && a[2] == 15 && a[3] == 12,
        abx_fm("AB-A1 RED(infra): p1_draw_aux_batt ic_batt_15.bin 消费几何非原生"
               "（取到 %d 参, dw=%ld dh=%ld, 期望 15x12 字面）——原生直出不缩放义务"
               "被破坏（宽度锚冻结形态 = 字面整数; 宏间接/符号常量形态取值失败同红, "
               "须复核锚不得改判为绿）", n, n >= 3 ? a[2] : -1, n >= 4 ? a[3] : -1));

    /* tooth 4 (落位授权域): 半区内居中 + 中线 y≈19（授权域推导见头注 [1];
     * x0∈[342,347] ∧ y0∈[11,15] ∧ (dw,dh)=(15,12) 蕴含墨盒收进钉区
     * {342,6,362,29}）。 */
    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 342 && a[0] <= 347 && a[1] >= 11 && a[1] <= 15,
        abx_fm("AB-A1 RED: ic_batt_15.bin 消费落位越出图标半区 HPB_RECT_AUX_BATT_ICON "
               "{342,6,362,29} 或偏离居中/中线 y≈19（x0=%ld y0=%ld dw=%ld dh=%ld; "
               "授权域 x0∈[342,347] y0∈[11,15]——横向往中 ±3px 取整让位, 纵向墨带"
               "中线 19±2）——落位义务被破坏", a[0], a[1], a[2], a[3]));

    /* tooth 5 (销账负向齿, 设计态红): "ic_batt_48.bin" 引号字面计数 = 0（裸源
     * 不剥注释; 注释提及不带引号不带 .bin, 天然不误红）。 */
    raw = abx_read(ABX_HOME_P1_C, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A1 RED(infra): home_p1.c unreadable");
    old_n = abx_count_sub(raw, "\"ic_batt_48.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(old_n == 0,
        abx_fm("AB-A1 RED: home_p1.c 仍有 %d 处 \"ic_batt_48.bin\" 字面消费未销账"
               "——ic_batt_48 22x40 非等比盒退役未完成（横压 2.18x + y0=0 clip 硬切; "
               "修复 = 消费点删除; screenshot 探针 home_p1_screenshot.c 同步更新归 "
               "builder 面, 不在本齿扫描域）", old_n));
    free(raw);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — AB-A2 温度图标源锚齿: p1_draw_aux_temp 消费 ic_thermo_12.bin 原生
 * 12x12 直出不缩放 + 半区 {210,6,230,29} 内居中/中线 y≈19 + "thermo_man.bin"
 * 引号字面销账。
 * RED today（设计态）: 消费仍为 thermo_man.bin（:1040 快照位, 41x67→20x20 硬压）,
 * ic_thermo_12.bin 消费点不在世 -> 首发红 = presence。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A2 -- 温度图标换原生直出件: home_p1.c p1_draw_aux_temp 消费 ic_thermo_12.bin（12x12 原生）直出不缩放, 图标半区 HPB_RECT_AUX_TEMP_ICON {210,6,230,29} 内居中、中线 y≈19（授权域 x0∈[211,217] y0∈[11,15]）; "thermo_man.bin" 引号字面消费销账 = 0（资产本体仍在 gen_art_spiffs 白名单 P1 既有 7 组, 非销账对象）; 兜底同 AB-A1 纪律 */
TEST_CASE("P1 辅助条温度图标源锚: ic_thermo_12 原生直出 + thermo_man 销账 (AB-A2)",
          "[p1_auxbar_style][AB-A2][structural]")
{
    char *code;
    char *raw;
    const char *fn;
    const char *fn_end;
    const char *hit;
    long a[16];
    int n;
    int old_n;

    ABX_REQUIRE_ROOT();

    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A2 RED(infra): home_p1.c unreadable");

    fn = strstr(code, "p1_draw_aux_temp");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "AB-A2 RED(infra): home_p1.c p1_draw_aux_temp 符号缺席——函数被删/改名, "
        "须复核锚不得改判为绿");
    fn_end = fn + ABX_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }

    hit = abx_find_in(fn, fn_end, "\"ic_thermo_12.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A2 RED: home_p1.c p1_draw_aux_temp 未取到 ic_thermo_12.bin 消费点（12x12 "
        "原生直出件未接入）——温度图标仍走 thermo_man 41x67 最近邻硬压 20x20 旧路径"
        "（五官细节全毁）; 修复 = 消费切换 thermo_man.bin -> ic_thermo_12.bin 原生 "
        "12x12 直出不缩放");

    n = abx_ints_between(hit + strlen("\"ic_thermo_12.bin\""), fn_end, a, 16);
    TEST_ASSERT_TRUE_MESSAGE(n >= 4 && a[2] == 12 && a[3] == 12,
        abx_fm("AB-A2 RED(infra): p1_draw_aux_temp ic_thermo_12.bin 消费几何非原生"
               "（取到 %d 参, dw=%ld dh=%ld, 期望 12x12 字面）——原生直出不缩放义务"
               "被破坏（宽度锚冻结形态 = 字面整数; 宏间接/符号常量形态取值失败同红, "
               "须复核锚不得改判为绿）", n, n >= 3 ? a[2] : -1, n >= 4 ? a[3] : -1));

    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 211 && a[0] <= 217 && a[1] >= 11 && a[1] <= 15,
        abx_fm("AB-A2 RED: ic_thermo_12.bin 消费落位越出图标半区 HPB_RECT_AUX_TEMP_ICON "
               "{210,6,230,29} 或偏离居中/中线 y≈19（x0=%ld y0=%ld dw=%ld dh=%ld; "
               "授权域 x0∈[211,217] y0∈[11,15]——横向往中 ±3px 取整让位, 纵向墨带"
               "中线 19±2）——落位义务被破坏", a[0], a[1], a[2], a[3]));

    raw = abx_read(ABX_HOME_P1_C, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A2 RED(infra): home_p1.c unreadable");
    old_n = abx_count_sub(raw, "\"thermo_man.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(old_n == 0,
        abx_fm("AB-A2 RED: home_p1.c 仍有 %d 处 \"thermo_man.bin\" 字面消费未销账"
               "——thermo_man 41x67→20x20 最近邻硬压缩放路径退役未完成（五官细节"
               "全毁; 修复 = 辅助条消费点删除; 资产本体仍在 gen_art_spiffs 白名单 "
               "P1 既有 7 组, 非销账对象）", old_n));
    free(raw);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — AB-A3 湿度图标源锚齿: p1_draw_aux_hum 消费 ic_drop_12.bin 原生
 * 12x12 直出不缩放 + 半区 {282,6,302,29} 内居中/中线 y≈19 + "drop_baby.bin"
 * 引号字面销账。
 * RED today（设计态）: 消费仍为 drop_baby.bin（:1070 快照位, 32x51→20x20 硬压）,
 * ic_drop_12.bin 消费点不在世 -> 首发红 = presence。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A3 -- 湿度图标换原生直出件: home_p1.c p1_draw_aux_hum 消费 ic_drop_12.bin（12x12 原生）直出不缩放, 图标半区 HPB_RECT_AUX_HUM_ICON {282,6,302,29} 内居中、中线 y≈19（授权域 x0∈[283,289] y0∈[11,15], "45%" 不裸数字挨电池——图标半前置纪律维持）; "drop_baby.bin" 引号字面消费销账 = 0; 兜底同 AB-A1 纪律 */
TEST_CASE("P1 辅助条湿度图标源锚: ic_drop_12 原生直出 + drop_baby 销账 (AB-A3)",
          "[p1_auxbar_style][AB-A3][structural]")
{
    char *code;
    char *raw;
    const char *fn;
    const char *fn_end;
    const char *hit;
    long a[16];
    int n;
    int old_n;

    ABX_REQUIRE_ROOT();

    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A3 RED(infra): home_p1.c unreadable");

    fn = strstr(code, "p1_draw_aux_hum");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "AB-A3 RED(infra): home_p1.c p1_draw_aux_hum 符号缺席——函数被删/改名, "
        "须复核锚不得改判为绿");
    fn_end = fn + ABX_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }

    hit = abx_find_in(fn, fn_end, "\"ic_drop_12.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A3 RED: home_p1.c p1_draw_aux_hum 未取到 ic_drop_12.bin 消费点（12x12 "
        "原生直出件未接入）——湿度图标仍走 drop_baby 32x51 最近邻硬压 20x20 旧路径"
        "（五官细节全毁）; 修复 = 消费切换 drop_baby.bin -> ic_drop_12.bin 原生 "
        "12x12 直出不缩放");

    n = abx_ints_between(hit + strlen("\"ic_drop_12.bin\""), fn_end, a, 16);
    TEST_ASSERT_TRUE_MESSAGE(n >= 4 && a[2] == 12 && a[3] == 12,
        abx_fm("AB-A3 RED(infra): p1_draw_aux_hum ic_drop_12.bin 消费几何非原生"
               "（取到 %d 参, dw=%ld dh=%ld, 期望 12x12 字面）——原生直出不缩放义务"
               "被破坏（宽度锚冻结形态 = 字面整数; 宏间接/符号常量形态取值失败同红, "
               "须复核锚不得改判为绿）", n, n >= 3 ? a[2] : -1, n >= 4 ? a[3] : -1));

    TEST_ASSERT_TRUE_MESSAGE(a[0] >= 283 && a[0] <= 289 && a[1] >= 11 && a[1] <= 15,
        abx_fm("AB-A3 RED: ic_drop_12.bin 消费落位越出图标半区 HPB_RECT_AUX_HUM_ICON "
               "{282,6,302,29} 或偏离居中/中线 y≈19（x0=%ld y0=%ld dw=%ld dh=%ld; "
               "授权域 x0∈[283,289] y0∈[11,15]——横向往中 ±3px 取整让位, 纵向墨带"
               "中线 19±2）——落位义务被破坏", a[0], a[1], a[2], a[3]));

    raw = abx_read(ABX_HOME_P1_C, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A3 RED(infra): home_p1.c unreadable");
    old_n = abx_count_sub(raw, "\"drop_baby.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(old_n == 0,
        abx_fm("AB-A3 RED: home_p1.c 仍有 %d 处 \"drop_baby.bin\" 字面消费未销账"
               "——drop_baby 32x51→20x20 最近邻硬压缩放路径退役未完成（五官细节"
               "全毁; 修复 = 辅助条消费点删除; 资产本体仍在 gen_art_spiffs 白名单 "
               "P1 既有 7 组, 非销账对象）", old_n));
    free(raw);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — AB-A4 wifi 资产重出守卫齿（**设计态绿**——同名同尺寸重出在 HEAD 无
 * pre-change 状态差, 变异取证豁免面）: gen_assets.mjs ic-wifi job 同行锚 + 输出
 * (w,h)=(14,14) + 母版 ic-wifi symbol 在世 + home_p1.c 消费几何 (12,12,14,14)
 * 零重锚守卫。含扫描器自检（行级双针计数器对已知在世形态能命中——计数器坏则
 * 全部齿失去咬合力）。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A4 -- wifi 资产按 14px 重设重出（母版 symbol 内容级重设归 uiux-author, 终形看屏轮 human 拍板——本齿不钉 symbol 内容, 钉内容即抢跑 human 仲裁面）: gen_assets ic-wifi job 重出 ic_wifi_14.bin 同名同尺寸 14x14（job 同行锚 + (w,h)=(14,14)）, 白名单 68 枚不动、验收零重锚（home_p1.c 消费几何 (12,12,14,14) 守卫）; 设计态绿守卫齿——pre-change 状态差不存在, 变异取证豁免面（TaskPackage mutationEvidence.reason 同款口径） */
TEST_CASE("P1 辅助条 wifi 资产重出守卫: ic_wifi_14 同名同尺寸 + 母版 symbol + 消费零重锚 (AB-A4)",
          "[p1_auxbar_style][AB-A4][structural]")
{
    char *code;
    char *raw;
    const char *hit;
    long a[8];
    int n;

    ABX_REQUIRE_ROOT();

    /* ---- 守卫齿 1: gen_assets.mjs ic-wifi job 同行锚 + 输出尺寸 (14,14) ----- */
    raw = abx_read(ABX_GEN_ASSETS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A4 RED(infra): gen_assets.mjs unreadable");
    n = abx_count_lines_with_both(raw, "'ic_wifi_14'", "svgUse('ic-wifi')");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "AB-A4 RED(infra): gen_assets.mjs 未取到 ic-wifi 重出 job 同行锚"
        "（'ic_wifi_14' 与 svgUse('ic-wifi') 同行在世）——ic_wifi_14 同名同尺寸 "
        "14x14 重出面锚漂移, 须复核锚不得改判为绿");

    hit = strstr(raw, "svgUse('ic-wifi')");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A4 RED(infra): gen_assets.mjs svgUse('ic-wifi') 锚缺席——重出面锚漂移, "
        "须复核锚不得改判为绿");
    n = abx_ints_between(hit + strlen("svgUse('ic-wifi')"), hit + 120, a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 2 && a[0] == 14 && a[1] == 14,
        abx_fm("AB-A4 RED(infra): gen_assets.mjs ic-wifi job 输出尺寸漂移"
               "（svgUse('ic-wifi') 后取到 %d 参, 期望 (w,h)=(14,14)）——同名同尺寸 "
               "14x14 重出义务被破坏（白名单 68 枚不动、验收零重锚; 越界改尺寸 = "
               "gen_art_spiffs 白名单连锁, 返 planner 重派）", n));
    free(raw);

    /* ---- 守卫齿 2: 母版 ic-wifi symbol 在世（symbol 面守卫, 不钉内容）-------- */
    raw = abx_read(ABX_MASTER_HTML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A4 RED(infra): kidpal-ui-design-v5.html unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "id=\"ic-wifi\"") != NULL,
        "AB-A4 RED(infra): docs/ui/kidpal-ui-design-v5.html 母版 ic-wifi symbol 缺席"
        "（id=\"ic-wifi\" 不在世）——母版 symbol 面漂移, 须复核锚不得改判为绿");
    free(raw);

    /* ---- 扫描器自检: 行级双针计数器对已知在世形态能命中 ---------------------- */
    raw = abx_read(ABX_HOME_P1_C, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A4 RED(infra): home_p1.c unreadable");
    n = abx_count_lines_with_both(raw, "p1_blit_art_scaled(", ".bin\"");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        abx_fm("AB-A4 RED(infra): 行级双针计数器失灵（p1_blit_art_scaled × .bin 同行"
               "只命中 %d 行, 期望 >= 1——ic_wifi_14 消费在世形态）——计数器坏, 本文件"
               "销账齿失去咬合力", n));

    /* ---- 守卫齿 3: home_p1.c ic_wifi_14.bin 消费几何 (12,12,14,14) 零重锚 ----
     * （兼整数取值器自检——取值器坏则 AB-A1/A2/A3 落位齿失去咬合力）。 */
    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A4 RED(infra): home_p1.c unreadable");
    hit = strstr(code, "\"ic_wifi_14.bin\"");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A4 RED(infra): home_p1.c ic_wifi_14.bin 消费点缺席——wifi 消费零重锚义务"
        "被破坏, 须复核锚不得改判为绿");
    n = abx_ints_between(hit + strlen("\"ic_wifi_14.bin\""),
                         hit + strlen("\"ic_wifi_14.bin\"") + 80, a, 8);
    TEST_ASSERT_TRUE_MESSAGE(n >= 4 && a[0] == 12 && a[1] == 12 &&
                             a[2] == 14 && a[3] == 14,
        abx_fm("AB-A4 RED(infra): home_p1.c ic_wifi_14.bin 消费点几何漂移（取到 %d 参, "
               "期望 (12, 12, 14, 14)）——wifi 消费零重锚义务被破坏", n));
    free(code);
    free(raw);
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — AB-A5 日期简写全大写表串源锚齿（Tier-2④a human 触点①拍板「直接做」）:
 * k_p1_month_abbr / k_p1_wday_abbr 表串全大写 + 非空表串数义务（月 12 / 星期 7,
 * 防清空表体假绿）。结构齿（表长 13/7 取值在世）兼表串提取器自检。
 * RED today（设计态）: 两表仍 title-case -> 首犯 "Jan" / "Mon" 红。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A5 -- 日期简写全大写（"Tue 22 Sep"→"TUE 22 SEP", 规避 12x16 降采样小写笔画侵蚀与大小写混排参差）: home_p1.c k_p1_month_abbr / k_p1_wday_abbr 表串逐 token 全大写（大写约束是设计决策, 具体拼写由表名/表长钉死）+ 非空表串数 月12/星期7 在世（防清空表体假绿）; 零新资产（imgfont_letter_A..Z 全套在库）; requirements AC1 示例措辞同步 = 殿后项不在本齿扫描域 */
TEST_CASE("P1 辅助条日期简写全大写表串源锚 (AB-A5)",
          "[p1_auxbar_style][AB-A5][structural]")
{
    char *code;
    const char *tab;
    char toks[16][8];
    int n, i, nonempty;

    ABX_REQUIRE_ROOT();

    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A5 RED(infra): home_p1.c unreadable");

    /* ---- 月表: 锚在世 + 13 token（兼自检）+ 非空 12 + 全大写 ----------------- */
    tab = strstr(code, "k_p1_month_abbr[13]");
    TEST_ASSERT_TRUE_MESSAGE(tab != NULL,
        "AB-A5 RED(infra): home_p1.c k_p1_month_abbr[13] 定义锚缺席——表锚漂移, "
        "须复核锚不得改判为绿");
    n = abx_strings_in_braces(tab, toks, 16);
    TEST_ASSERT_TRUE_MESSAGE(n == 13,
        abx_fm("AB-A5 RED(infra): home_p1.c k_p1_month_abbr 表体取值失灵（取到 %d 个"
               "字符串字面量, 期望 13）——表锚漂移或表结构重写, 须复核锚不得改判为绿"
               "（兼表串提取器自检——提取器坏则本齿全齿失去咬合力）", n));
    nonempty = 0;
    for (i = 0; i < n; i++) {
        if (toks[i][0] != '\0') {
            nonempty++;
        }
        TEST_ASSERT_TRUE_MESSAGE(abx_all_upper_or_empty(toks[i]),
            abx_fm("AB-A5 RED: k_p1_month_abbr 月简写表串含小写字母（首犯 \"%s\"）——"
                   "日期月/星期简写仍为 title-case, 12x16 降采样下小写笔画侵蚀与"
                   "大小写混排参差在世（Tier-2④a human 触点①拍板「直接做」; 修复 = "
                   "表串全大写 JAN..DEC, 零新资产 imgfont_letter_A..Z 全套在库）",
                   toks[i]));
    }
    TEST_ASSERT_TRUE_MESSAGE(nonempty == 12,
        abx_fm("AB-A5 RED: k_p1_month_abbr 月简写非空表串数 = %d（期望 12）——12 月"
               "全量简写在世义务被破坏（防清空表体假绿）", nonempty));

    /* ---- 星期表: 锚在世 + 7 token + 非空 7 + 全大写 -------------------------- */
    tab = strstr(code, "k_p1_wday_abbr[7]");
    TEST_ASSERT_TRUE_MESSAGE(tab != NULL,
        "AB-A5 RED(infra): home_p1.c k_p1_wday_abbr[7] 定义锚缺席——表锚漂移, "
        "须复核锚不得改判为绿");
    n = abx_strings_in_braces(tab, toks, 16);
    TEST_ASSERT_TRUE_MESSAGE(n == 7,
        abx_fm("AB-A5 RED(infra): home_p1.c k_p1_wday_abbr 表体取值失灵（取到 %d 个"
               "字符串字面量, 期望 7）——表锚漂移或表结构重写, 须复核锚不得改判为绿",
               n));
    nonempty = 0;
    for (i = 0; i < n; i++) {
        if (toks[i][0] != '\0') {
            nonempty++;
        }
        TEST_ASSERT_TRUE_MESSAGE(abx_all_upper_or_empty(toks[i]),
            abx_fm("AB-A5 RED: k_p1_wday_abbr 星期简写表串含小写字母（首犯 \"%s\"）——"
                   "日期星期简写仍为 title-case（同月表 ④a 拍板; 修复 = 表串全大写 "
                   "MON..SUN）", toks[i]));
    }
    TEST_ASSERT_TRUE_MESSAGE(nonempty == 7,
        abx_fm("AB-A5 RED: k_p1_wday_abbr 星期简写非空表串数 = %d（期望 7）——7 天"
               "全量简写在世义务被破坏（防清空表体假绿）", nonempty));

    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — AB-A6 冒号眨眼门控零触碰负面锚（④b human 拍板不做; **设计态绿**）:
 * colon_visible 门控在世且仍门控 blit + HP1_BLINK_PERIOD_COLON_MS 常量 4400 同行
 * 在世 + kidpal_home_p1_blink_visible 函数定义本体在世。
 * ------------------------------------------------------------------------ */
/* coverage: AB-A6 -- ④b 负面锚 + 既有冻结件回归面: p1_draw_aux_time 冒号眨眼门控零触碰（colon_visible 赋值与消费在世——眨眼即时钟跳动语义, human 触点①拍板「不做、维持眨眼」）; home_p1.h HP1_BLINK_PERIOD_COLON_MS=4400 定义行在世（周期退役授权不存在）; kidpal_home_p1_blink_visible 函数本体在世（p1_cjk_name:1342 符号钉 / HPB-A4 活齿依赖面）; p1_home_battery / p1_cjk_name / time_sync / voice_rightcol_label 四冻结件零改动全绿 = host 套件回归观察面（红基线/终跑判读承载, 落齿即越权复钉既有冻结件断言面, VR-A5 同款口径） */
TEST_CASE("P1 辅助条冒号眨眼门控零触碰负面锚 (AB-A6)",
          "[p1_auxbar_style][AB-A6][structural]")
{
    char *code;
    char *raw;
    const char *fn;
    const char *fn_end;
    const char *hit;
    int n;

    ABX_REQUIRE_ROOT();

    code = abx_strip_c_comments_file(ABX_HOME_P1_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "AB-A6 RED(infra): home_p1.c unreadable");

    /* ---- 守卫齿 1: colon_visible 门控在世且仍门控 blit（函数窗绑定）---------- */
    fn = strstr(code, "p1_draw_aux_time");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "AB-A6 RED(infra): home_p1.c p1_draw_aux_time 符号缺席——函数被删/改名, "
        "须复核锚不得改判为绿");
    fn_end = fn + ABX_TIME_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    hit = abx_find_in(fn, fn_end,
        "kidpal_home_p1_blink_visible(s_state.tick_ms, HP1_BLINK_PERIOD_COLON_MS)");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "AB-A6 RED(infra): home_p1.c p1_draw_aux_time 冒号眨眼门控漂移——colon_visible "
        "赋值（kidpal_home_p1_blink_visible(s_state.tick_ms, HP1_BLINK_PERIOD_COLON_MS)"
        "）缺席——④b human 拍板不做、冻结语义被触碰, 须复核锚不得改判为绿");

    {
        /* 只统计窗内出现（colon_visible 在窗外的合法出现不算——先截窗再计数）。 */
        size_t win = (size_t)(fn_end - fn);
        char *winbuf = (char *)malloc(win + 1);
        assert(winbuf != NULL); /* logic guard: host OOM must fail loudly */
        memcpy(winbuf, fn, win);
        winbuf[win] = '\0';
        n = abx_count_sub(winbuf, "colon_visible");
        free(winbuf);
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        abx_fm("AB-A6 RED(infra): p1_draw_aux_time colon_visible 门控失联（窗内出现 "
               "%d 次, 期望 >= 2 = 赋值 + blit 消费）——门控只赋值不消费或被改写, "
               "须复核锚不得改判为绿", n));

    /* ---- 守卫齿 2: home_p1.h 常量定义行在世且值 4400 同行（line 级双针）------ */
    raw = abx_read(ABX_HOME_P1_H, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "AB-A6 RED(infra): home_p1.h unreadable");
    n = abx_count_lines_with_both(raw, "HP1_BLINK_PERIOD_COLON_MS", "4400");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "AB-A6 RED(infra): home_p1.h HP1_BLINK_PERIOD_COLON_MS 常量漂移（定义行缺席"
        "或周期值 4400 不同行在世）——④b human 拍板不做、冒号眨眼 4400ms 冻结语义"
        "被触碰, 须复核锚不得改判为绿");
    free(raw);

    /* ---- 守卫齿 3: kidpal_home_p1_blink_visible 函数定义本体在世 ------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(code, "int kidpal_home_p1_blink_visible(int tick_ms, int period_ms)") != NULL,
        "AB-A6 RED(infra): home_p1.c kidpal_home_p1_blink_visible 函数定义缺席——"
        "blink_visible 符号钉（p1_cjk_name:1342 / HPB-A4 活齿 :1047-1056）依赖的"
        "函数本体不在世, 须复核锚不得改判为绿");

    free(code);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-16 初版冻结（TaskPackage 261015-p1-auxbar-style-fix; acceptance[]
 *               AB-A1~A7 + mutationEvidence 骨架为冻结边界; plan.md reviewed +
 *               human 触点①三决: ②单包 Lite / ④a 直接做 / ④b 不做出包）。六
 *               TEST_CASE: AB-A1/A2/A3 正向源锚齿（消费切换 + 原生几何字面 +
 *               落位授权域 ±3px/±2px——MCD-A1 不钉单一魔数先例）+ 引号字面销账
 *               负向齿（裸源计数, 注释提及不带引号天然不误红）; AB-A4 守卫齿
 *               （设计态绿——同名同尺寸重出无 pre-change 状态差, 变异取证豁免）
 *               + 扫描器自检两枚; AB-A5 全大写表串齿 + 表长结构自检; AB-A6 零
 *               触碰负面锚三齿（设计态绿）。AB-A7 真机轮 structural 豁免不落
 *               C 齿。冻结落盘冒烟 = red-baseline 独立执行架实编 + 实跑（净
 *               HEAD 设计态红形状逐齿核对, 红名 verbatim 见交付报告）。
 * --------------------------------------------------------------------------- */
