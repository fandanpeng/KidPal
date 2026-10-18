/*
 * Frozen acceptance tests — TaskPackage 261016-sd-card-mount
 * (subsystem: firmware/main/boards/waveshare-s3-rlcd-4.2（板级 SDMMC 挂载）;
 * rigor full; sourceType human-ad-hoc; riskTier medium; TF 卡接入——SDMMC 1-bit
 * 挂载 + /sdcard 存储访问能力就绪（降级不崩）)。
 *
 * Story: TEMP-2026-10-16-sd-card-mount · 行为权威 = reviewed plan.md（architect
 * /triage-change 分流 review 2026-10-16, frontmatter reviewed-by 为证; human 触点①
 * 三项确认: ①能力就绪 scope 收窄 ②LFN 不开（验收限 8.3 命名）③architecture.md:232
 * 随批 doc-sync 不升级架构面）。非新语义包: 能力就绪包, 不改任何用户可见行为
 * （UIUX L0）; story 存储 / 日志落盘不在本包（human 原话钉死, 将来另包且须先过
 * 决策 24 介质禁令 spec 裁决）。
 *
 * Nature: HOST-SIDE STRUCTURAL（源锚齿 tree-scan, 先例形态 =
 * p1_auxbar_style.acceptance.c 261015-p1-auxbar-style-fix 同款; 助手族自包含复制、
 * 前缀 sdm_、static 链接不与先例文件符号冲突）。板级 SDMMC 编译单元为设备专有
 * （esp_vfs_fat_sdmmc_mount 等 IDF 专有符号, host_app 盖不住——triage 实核, 快路径
 * 不适用）, 故对仓内源文件做源锚断言: 正向齿只认**注释剥离后可执行源**的字面形态
 * （260917-m2-k3 sealing pass 教训: 只出现在注释里的锚值会假绿）; 仓内先例形态
 * 校准 = wireless-tag-wtp4c5mp07s.cc 的 SDMMC 挂载函数（slot_config 字段赋值 +
 * designated-initializer mount_config 形态, 仓内实存）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements; 本文件只读扫工作树）:
 *
 *   [1] config.h 四宏（plan §1; 来源 = 官方例程 sdcard_bsp.h:15 默认脚位, 只读
 *       参考重写; 注释标注包号 + 来源）:
 *         SD_MMC_CLK_GPIO    GPIO_NUM_38
 *         SD_MMC_CMD_GPIO    GPIO_NUM_21
 *         SD_MMC_D0_GPIO     GPIO_NUM_39
 *         SD_MMC_MOUNT_ROOT  "/sdcard"
 *       断言形态 = 注释剥离后 #define 宏名与值同行在世（字面; 宏间接/符号常量
 *       形态取值失败同红, 须复核锚不得改判为绿）+ raw 源含包号 "261016-sd-card-mount"
 *       与来源锚 "sdcard_bsp"（注释面义务, 裸源级断言）。
 *
 *   [2] 板 .cc 挂载初始化（plan §2）: `void InitializeSdCard` 定义在世（定义形态
 *       锚, 非裸符号——防构造函数调用点先行误命中）, 挂进 CustomBoard 构造函数
 *       初始化序列（"CustomBoard() :" 构造窗内存在调用点 "InitializeSdCard();"）;
 *       函数窗内:
 *         - 挂载调用 = `esp_vfs_fat_sdmmc_mount(SD_MMC_MOUNT_ROOT`（根宏作首参,
 *           plan 逐字形态）;
 *         - `slot.width = 1`（1-bit 义务: 宽度赋值行首个整数 = 字面 1, 或
 *           WIDTH_1 枚举形态; 支持 slot.width / .width designated-initializer
 *           两种拼写——仓内先例 wireless-tag 为 slot_config 字段赋值形态）;
 *         - clk/cmd/d0 取 config.h 宏: 函数窗内 SD_MMC_CLK_GPIO / SD_MMC_CMD_GPIO /
 *           SD_MMC_D0_GPIO 三宏在世（消费绑定——宏挪出函数窗 = 锚漂移红(infra)）;
 *         - mount config: format_if_mount_failed = false（坏卡/异格式不销毁数据、
 *           只报错降级——对偶 InitializeArt 既有范式）且**禁止 true 形态**（负向齿）;
 *         - max_files = 8（赋值行首个整数 = 字面 8）。
 *
 *   [3] 失败降级路径（plan §2; 不 panic、不阻断启动）: 函数窗内——
 *         - `ESP_ERROR_CHECK_WITHOUT_ABORT`（降级形态）在世, 且裸 panic 形态
 *           `ESP_ERROR_CHECK(` 计数 = 0（负向齿; WITHOUT_ABORT 前缀不误伤——
 *           其 "CHECK(" 后接 "_WITH"）;
 *         - 返回码判空 ESP_OK + 卡句柄判空 NULL 双检在世;
 *         - 失败置标志: s_sd_mounted 与 false 同行（降级置假）;
 *         - ESP_LOGW 继续（降级 WARN, 不静默）;
 *         - 成功路径: s_sd_mounted 与 true 同行（置真）+ sdmmc_card_print_info
 *           （卡信息打印——真机 A1 观察点「启动日志见 SD 卡信息打印」的结构承载）。
 *
 *   [4] 板级查询入口（plan §2; 本包零消费方, 防死代码误删的观察锚）: `SdCardMounted`
 *       定义在世且读 s_sd_mounted 标志（定义窗内 s_sd_mounted + return 在世——
 *       禁常量真值 stub）。
 *
 *   [5] live-manual 条目（不计入 runner 通过面; 见 coverage map SD-A5/SD-A6）:
 *       - SD-A5 真机有卡轮 = **已执行 PASS**（2026-10-16 真机门停等点第 2 次接续;
 *         human-decision 0263a7a3; 取证日志 C:/tmp/sd-boot-a5b.log）。原 AB-A7 先例
 *         口径「真机轮 human 持板、不落 C 齿」对 SD-A5 已兑现完成。
 *       - SD-A6 真机无卡轮 = **本机不可执行豁免 + 结构性证据替代**（human 裁定
 *         2026-10-16; human-decision 416e5c42）: 卡永久在位、无卡态在本机不可达
 *         （human 原话「不好取，不取了，以后也不会拿出来」）→ 替代证据性质 =
 *         **代码面静态结构断言, 非运行时真机观察**（明确不冒充真机观察）, 由 SD-A3 +
 *         SD-A4 承载, 不落 C 齿（替代证据口径逐条见 coverage map SD-A6 行）。
 *
 *   [6] Builder FIRST ACTION（本文件进 host runner 的唯一前置; 接线行**不在
 *       acceptance-author 写权内**, scope-map 分权——TaskPackage changeManifest
 *       预指派 builder 代落）: 向 firmware/main/test/host_app/main/CMakeLists.txt
 *       的 HOST_ACCEPTANCE_SRCS 追加一行:
 *         "../../sd_card_mount.acceptance.c"
 *       置于 "../../p1_auxbar_style.acceptance.c" 之后、
 *       "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, 包 0 boundary 齿注册序守卫, 既有惯例）。本文件纯树扫、零被测实现
 *       接线（板级 config.h / .cc 不进 host 构建）。
 *
 *   [7] 冻结授权链: TaskPackage 261016-sd-card-mount（acceptance[] SD-A1~A6 +
 *       mutationEvidence 骨架为冻结边界; plan.md reviewed + human 触点①三决）。
 *       本文件冻结后 builder/fixer tighten-only（纯 additive 补断言/修自身
 *       fixture; 禁放宽、禁改断言语义）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: SD-A1 -- config.h 四宏在世且值正确 + 注释包号/来源标注（test 1）
 *   coverage: SD-A2 -- InitializeSdCard 定义 + 构造序列接线 + 挂载调用参数五件
 *             （root 宏首参 / width=1 / clk·cmd·d0 宏 / format_if_mount_failed=
 *             false+true 禁令 / max_files=8）（test 2）
 *   coverage: SD-A3 -- 失败降级路径六件（WITHOUT_ABORT + panic 禁令 + 返回码/
 *             句柄双检 + 降级置假 + WARN 继续 + 成功置真与卡信息打印）（test 3）
 *   coverage: SD-A4 -- 板级查询入口 SdCardMounted 读标志观察锚（test 4）
 *   coverage: SD-A5 -- 真机有卡轮 **已执行 PASS**（2026-10-16 真机门停等点第 2 次
 *             接续; human-decision 0263a7a3; 取证日志 C:/tmp/sd-boot-a5b.log）, 不计
 *             入 runner 通过面: human 持板——① 上电（**插卡**）启动日志见 SD 卡信息
 *             打印（sdmmc_card_print_info 输出: "Name: asdfg / Type: SDHC" + "CSD:
 *             ver=2, sector_size=512, capacity=30535680"）+ 挂载成功（"SD card
 *             mounted at /sdcard (SDMMC 1-bit)"）; ② 经板级入口完成写-读-删
 *             roundtrip: 8.3 探针文件 /sdcard/SDTEST.TXT, 写入内容与读回一致, 验后
 *             即删、不驻留（决策 24 介质禁令——临时测试探针非业务数据落板）; 真机
 *             兑现行 = "SD ROUNDTRIP PASS path=/sdcard/SDTEST.TXT corr_id=kdp-1262
 *             bytes=63"; ③ 修订 2026-10-16（真机门停等点第 2 次接续）: 原理图三根脚
 *             （CLK38/CMD21/D039）目视核对**未单独执行**——改由「config.h 宏取值 +
 *             真机挂载成功」实证反证（真机 mount 成功即 CLK38/CMD21/D039 接线正确,
 *             销 GPIO21 历史疑问）。
 *   coverage: SD-A6 -- **本机不可执行豁免 + 结构性证据替代**（human 裁定 2026-10-16;
 *             human-decision 416e5c42）, 不落 C 齿: 真机无卡态在本机不可达（卡永久
 *             在位, human 原话「不好取，不取了，以后也不会拿出来」）——替代证据性质
 *             = **代码面静态结构断言, 非运行时真机观察**（明确不冒充真机观察）: 降级
 *             分支 ESP_ERROR_CHECK_WITHOUT_ABORT 不 panic 的代码面断言由本文件 test 3 /
 *             SD-A3 承载（WITHOUT_ABORT 在世 + 裸 ESP_ERROR_CHECK( panic 形态计数 = 0
 *             负向齿 + s_sd_mounted 与 false 同行降级置假 + ESP_LOGW WARN 继续不静默）;
 *             现有 host/结构齿覆盖面 = SD-A3（失败降级路径结构齿）+ SD-A4（板级查询
 *             入口读标志观察锚）。真机观察面（**无卡**上电降级 WARN / 语音主链路不受
 *             影响）本机不可得, 豁免事由见上; 原 live-manual 停门口径（human 持板）
 *             对本条不再适用。修订 2026-10-16（真机门停等点第 2 次接续）: human 真机
 *             轮裁定 → SD-A6 由 live-manual 停门改为本机不可执行豁免 + 结构性证据替代。
 *   coverage: A3（plan §验收方式）-- 设备编译闸: 非验收齿, 归 verify
 *             firmware-build 闸（VERIFY_FW_BUILD_REQUIRED=1, rigor=full 无 defer）,
 *             不落 C 齿、不占 acceptance 条目（intake 约束 3）。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder wiring 见 [6]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加）
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf   （elf 相对 host_app/ 目录）
 *   本文件另可独立冒烟（red-baseline 独立执行架先例, 冻结相位实编 + 咬合力取证用）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -I. -o /tmp/sdm_red_runner ../../sd_card_mount.acceptance.c runner.c
 *     cd ../../../../..  (repo root);  /tmp/sdm_red_runner
 *
 * RED baseline（freeze 相位预期申报——净 HEAD: config.h 无任何 SD_MMC 宏、板 .cc
 * 无 InitializeSdCard / SdCardMounted / s_sd_mounted / sdmmc 符号（本会话 grep
 * 实证）; 冻结落盘时已按「冻结即冒烟/§4 C 类实编」以独立冒烟通道实编 + 实跑一次,
 * 红名 verbatim 见交付报告）:
 *   - test 1（SD-A1）: 设计态红, 首发红名要点 = "SD-A1 RED: config.h 未取到
 *     #define SD_MMC_CLK_GPIO 与 GPIO_NUM_38 同行宏定义"（四宏 + 挂载根宏 + 注释
 *     包号/来源齿同相位红, 被 unity 短路不打印——presence 先 fire）。若修后红名
 *     变为值漂移/注释标注红 = 宏值或注释面被破坏, 属设计态语义红, 不得改判为绿。
 *   - test 2（SD-A2）: 设计态红, 首发红名要点 = "SD-A2 RED: 板级挂载初始化
 *     InitializeSdCard 未落地"（挂载调用/参数/构造接线齿同相位红被短路）。
 *   - test 3（SD-A3）: 设计态红, 首发红名要点 = "SD-A3 RED: InitializeSdCard
 *     失败降级路径未落地"（WITHOUT_ABORT/降级置假/WARN/成功置真齿同相位红被短路）。
 *   - test 4（SD-A4）: 设计态红, 首发红名要点 = "SD-A4 RED: 板级查询入口
 *     SdCardMounted() 未落地"。
 *   - 既有套件（key_layer_dispatch / p1_home_battery / … / p1_auxbar_style 等）:
 *     全绿零新增红（板级 .cc / config.h 不入 host 编译面, 既有齿与本包正交——
 *     host 套件回归观察面, 红基线/终跑判读承载, AB-A6 同款口径）。
 *   - 转绿判据: builder 四宏 + 挂载初始化 + 降级路径 + 查询入口落地后 4 齿全绿;
 *     任一设计态红齿仍红而对应源锚已落地 = 锚漂移, 复核本文件锚, 不得改判为绿。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

#define SDM_MAX_PATH 1024
#define SDM_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根; 行号快照非长期事实源, 齿锚全部用符号/字面锚）。 */
#define SDM_CONFIG_H  "firmware/main/boards/waveshare-s3-rlcd-4.2/config.h"
#define SDM_BOARD_CC  "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"

/* 函数窗: 锚定被测函数符号后的扫描窗（禁行号钉点的函数级绑定位; 窗宽 = 函数体
 * 量级 2 倍以上, 与相邻函数粘连属预期——窗内只作 needle 在场判定, 不作唯一性
 * 判定, 粘连不误红）。 */
#define SDM_FN_WINDOW  3000
#define SDM_CTOR_WINDOW 4000
#define SDM_ACC_WINDOW  400

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（自包含复制自 p1_auxbar_style.acceptance.c 的
 * abx_* 助手族, 精简/增补为本文件所需子集; 全 static 链接, 命名前缀 sdm_ 不与
 * 先例冲突）。全部扫描跑在真实工作树上, root 以手持 cwd 定位（仓根 /
 * firmware/main/test/host_app 两处都可跑——walk-up 定根纪律）。
 * ------------------------------------------------------------------------ */

static char sdm_root[SDM_MAX_PATH];
static int sdm_root_ready;

static int sdm_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int sdm_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[SDM_MAX_PATH];
    size_t i;

    if (sdm_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (sdm_is_regular(probe)) {
            snprintf(sdm_root, sizeof(sdm_root), "%s", cands[i]);
            sdm_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int sdm_rel(char *out, size_t cap, const char *rel)
{
    if (!sdm_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", sdm_root, rel);
    return 1;
}

static char *sdm_read(const char *rel, size_t *out_len)
{
    char p[SDM_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!sdm_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SDM_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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
 * passed to the single-string assert macros（KLD/CWX/VRL/ABX 先例同款）。 */
static char sdm_fm_buf[1024];
static const char *sdm_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *sdm_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sdm_fm_buf, sizeof(sdm_fm_buf), fmt, ap);
    va_end(ap);
    return sdm_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Comment-stripping scanner support（260917-m2-k3 sealing pass / ABX 同款）:
 * 正向齿只被**可执行源**满足, 注释字节抹成空格; 字符串/字符字面量体保留（跳过
 * "..." / '...' 体, 不把字面量里的 // 误判成注释）。保持缓冲长度（注释字节 ->
 * 空格）, 调用方负责 free。
 * ------------------------------------------------------------------------ */

static char *sdm_strip_c_comments(const char *src)
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

static char *sdm_strip_c_comments_file(const char *rel)
{
    char *buf = sdm_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = sdm_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（函数窗内 needle 在场判定）。返回窗内首个命中位, 未命中 -> NULL。 */
static const char *sdm_find_in(const char *hay, const char *end, const char *needle)
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

/* 有界整数提取: [p, end) 内、首个 ')' 或 ';' 前的带符号十进制字面量序（ABX
 * abx_ints_between 有界变体——消费几何/配置实参须为**字面整数**书写, 宏间接/
 * 符号常量形态取值失败 = 红, 不改判）。返回写入 out 的个数。 */
static int sdm_ints_between(const char *p, const char *end, long *out, int max_args)
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

/* 行内首个整数提取: hay 内 needle 之后、同一行内首个整数字面量（宽度/文件句柄
 * 上限等赋值行形态 `x.y = N;` / `.field = N,`）。取到返回 1 并写 *out, 未命中
 * needle 或行内无整数字面量返回 0（宏间接形态取值失败 = 调用方判红）。 */
static int sdm_first_int_after_on_line(const char *hay, const char *needle, long *out)
{
    const char *p = strstr(hay, needle);
    const char *eol;
    long a[4];
    int n;

    if (p == NULL) {
        return 0;
    }
    p += strlen(needle);
    eol = strchr(p, '\n');
    if (eol == NULL) {
        eol = p + strlen(p);
    }
    n = sdm_ints_between(p, eol, a, 4);
    if (n < 1) {
        return 0;
    }
    *out = a[0];
    return 1;
}

/* 裸源子串出现计数（负向齿专用——panic 形态计数等; 不剥注释, 调用方传入已剥
 * 注释窗时天然只计可执行源）。 */
static int sdm_count_sub(const char *hay, const char *needle)
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

/* 行级判定: slice 内同时含 needle1 与 needle2 的行数。 */
static int sdm_count_lines_with_both(const char *slice, const char *needle1,
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
        char tmp[SDM_MAX_PATH];
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

/* 函数窗切片复制: [fn, fn_end) 拷入新分配 NUL 结尾缓冲（行级齿的窗内绑定载体,
 * ABX AB-A6 同款; 调用方负责 free）。 */
static char *sdm_window_copy(const char *fn, const char *fn_end)
{
    size_t win = (size_t)(fn_end - fn);
    char *winbuf = (char *)malloc(win + 1);

    assert(winbuf != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(winbuf, fn, win);
    winbuf[win] = '\0';
    return winbuf;
}

#define SDM_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(sdm_locate_root(), \
    "SD-Ax RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* -------------------------------------------------------------------------- *
 * TEST 1 — SD-A1 config.h 四宏源锚齿: SD_MMC_CLK_GPIO=GPIO_NUM_38 /
 * SD_MMC_CMD_GPIO=GPIO_NUM_21 / SD_MMC_D0_GPIO=GPIO_NUM_39 /
 * SD_MMC_MOUNT_ROOT="/sdcard" 注释剥离后 #define 宏名与值同行在世 + raw 源注释
 * 面包号 "261016-sd-card-mount" 与来源锚 "sdcard_bsp" 在世。含扫描器自检
 * （行级双针计数器对已知在世形态能命中——计数器坏则全部齿失去咬合力）。
 * RED today（设计态）: config.h 无任何 SD_MMC 宏 -> 首发红 = presence（CLK 行）。
 * ------------------------------------------------------------------------ */
/* coverage: SD-A1 -- config.h 四宏在世且值正确: #define SD_MMC_CLK_GPIO=GPIO_NUM_38 / SD_MMC_CMD_GPIO=GPIO_NUM_21 / SD_MMC_D0_GPIO=GPIO_NUM_39 / SD_MMC_MOUNT_ROOT="/sdcard"（注释剥离后可执行源同行字面; 宏间接形态取值失败同红, 须复核锚不得改判为绿）+ 注释面包号 261016-sd-card-mount 与来源锚 sdcard_bsp.h:15（裸源断言; 来源 = 官方例程默认脚位, 只读参考重写）; 扫描器自检 = 行级双针对已知在世形态命中 */
TEST_CASE("SD 卡挂载引脚宏源锚: config.h 四宏 + 挂载根 + 注释包号/来源 (SD-A1)",
          "[sd_card_mount][SD-A1][structural]")
{
    char *code;
    char *raw;
    int n;

    SDM_REQUIRE_ROOT();

    code = sdm_strip_c_comments_file(SDM_CONFIG_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SD-A1 RED(infra): config.h unreadable");

    /* ---- 扫描器自检: 行级双针计数器对已知在世形态能命中 ---------------------- */
    n = sdm_count_lines_with_both(code, "#define AUDIO_I2S_GPIO_MCLK", "GPIO_NUM_16");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        sdm_fm("SD-A1 RED(infra): 行级双针计数器失灵（#define AUDIO_I2S_GPIO_MCLK × "
               "GPIO_NUM_16 已知在世形态只命中 %d 行, 期望 >= 1）——计数器坏, 本文件"
               "全部齿失去咬合力", n));

    /* ---- tooth 2 (本包设计态红齿, presence): CLK 宏 + 值同行 ----------------- */
    n = sdm_count_lines_with_both(code, "#define SD_MMC_CLK_GPIO", "GPIO_NUM_38");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A1 RED: config.h 未取到 #define SD_MMC_CLK_GPIO 与 GPIO_NUM_38 同行宏定义"
        "（TF 卡接入引脚宏未落地）——SDMMC 1-bit 挂载四宏 + 挂载根宏未在世（来源 = "
        "官方例程 sdcard_bsp.h:15 默认脚位, 只读参考重写; 修复 = config.h 新增四宏, "
        "注释带包号 261016-sd-card-mount）");

    /* ---- tooth 3: CMD 宏 + 值同行 ------------------------------------------- */
    n = sdm_count_lines_with_both(code, "#define SD_MMC_CMD_GPIO", "GPIO_NUM_21");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A1 RED: config.h 未取到 #define SD_MMC_CMD_GPIO 与 GPIO_NUM_21 同行宏定义"
        "——引脚宏面不完整（triage 实核 21/38/39 在 firmware/main 对本板零占用, 无冲突; "
        "修复 = 补宏）");

    /* ---- tooth 4: D0 宏 + 值同行 -------------------------------------------- */
    n = sdm_count_lines_with_both(code, "#define SD_MMC_D0_GPIO", "GPIO_NUM_39");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A1 RED: config.h 未取到 #define SD_MMC_D0_GPIO 与 GPIO_NUM_39 同行宏定义"
        "——引脚宏面不完整（修复 = 补宏）");

    /* ---- tooth 5: 挂载根宏 + 值同行 ----------------------------------------- */
    n = sdm_count_lines_with_both(code, "#define SD_MMC_MOUNT_ROOT", "\"/sdcard\"");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A1 RED: config.h 未取到 #define SD_MMC_MOUNT_ROOT 与 \"/sdcard\" 同行宏定义"
        "——挂载根宏未落地（修复 = 补宏; VFS 根形态, 供将来消费方 fopen/opendir 前缀）");

    /* ---- tooth 6 (注释面义务, 裸源级): 包号 + 来源锚在世 --------------------- */
    raw = sdm_read(SDM_CONFIG_H, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "SD-A1 RED(infra): config.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "261016-sd-card-mount") != NULL,
        "SD-A1 RED: config.h SD 宏注释缺包号 261016-sd-card-mount——注释标注义务被"
        "破坏（plan §1: 注释标注本包号 + 来源）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "sdcard_bsp") != NULL,
        "SD-A1 RED: config.h SD 宏注释缺来源标注 sdcard_bsp——来源 = 官方例程 "
        "sdcard_bsp.h:15 默认脚位（只读参考重写）, 注释标注义务被破坏");

    free(raw);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — SD-A2 板级挂载初始化源锚齿: `void InitializeSdCard` 定义在世 + 挂进
 * CustomBoard 构造函数初始化序列 + 函数窗内挂载调用五件（root 宏首参 / width=1 /
 * clk·cmd·d0 宏消费 / format_if_mount_failed=false 且禁 true / max_files=8）。
 * RED today（设计态）: 板 .cc 无 InitializeSdCard -> 首发红 = presence。
 * ------------------------------------------------------------------------ */
/* coverage: SD-A2 -- 板 .cc 挂载初始化: void InitializeSdCard 定义在世 + CustomBoard 构造初始化序列调用点（InitializeArt 同排）+ esp_vfs_fat_sdmmc_mount(SD_MMC_MOUNT_ROOT 首参 + slot.width=1 字面（1-bit 义务; WIDTH_1 枚举形态等价接受）+ clk/cmd/d0 取 config.h 宏（SD_MMC_CLK_GPIO/SD_MMC_CMD_GPIO/SD_MMC_D0_GPIO 函数窗消费绑定）+ format_if_mount_failed=false（坏卡/异格式不销毁数据——对偶 InitializeArt 既有范式; true 形态负向禁令）+ max_files=8 字面 */
TEST_CASE("SD 卡挂载初始化源锚: InitializeSdCard + 构造接线 + 挂载调用参数 (SD-A2)",
          "[sd_card_mount][SD-A2][structural]")
{
    char *code;
    char *winbuf;
    const char *fn;
    const char *fn_end;
    const char *ctor;
    const char *ctor_end;
    const char *hit;
    long v = 0;
    int n;
    int ok;

    SDM_REQUIRE_ROOT();

    code = sdm_strip_c_comments_file(SDM_BOARD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SD-A2 RED(infra): 板 .cc unreadable");

    /* ---- tooth 1 (函数锚在世, 定义形态): void InitializeSdCard 定义不在世 = 锚
     *     漂移/函数被删（本包设计态红 = 函数未落地）。 */
    fn = strstr(code, "void InitializeSdCard");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "SD-A2 RED: 板级挂载初始化 InitializeSdCard 未落地（waveshare-s3-rlcd-4.2.cc "
        "void InitializeSdCard 定义缺席）——SDMMC 1-bit 挂载初始化未在世（修复 = 板 .cc "
        "新增 InitializeSdCard, 挂进 CustomBoard 构造函数初始化序列）");
    fn_end = fn + SDM_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    winbuf = sdm_window_copy(fn, fn_end);

    /* ---- tooth 2 (挂载调用 + 根宏首参): plan 逐字形态。 ---------------------- */
    hit = sdm_find_in(fn, fn_end, "esp_vfs_fat_sdmmc_mount(SD_MMC_MOUNT_ROOT");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "SD-A2 RED: InitializeSdCard 函数窗未取到 esp_vfs_fat_sdmmc_mount("
        "SD_MMC_MOUNT_ROOT 挂载调用——挂载根宏未作首参（plan §2 逐字形态; 修复 = 挂载"
        "调用以 SD_MMC_MOUNT_ROOT 为 mount point 首参）");

    /* ---- tooth 3 (1-bit 义务): 宽度赋值行首个整数 = 字面 1（slot.width / .width
     *     两种拼写; WIDTH_1 枚举形态等价接受; 宏间接形态取值失败同红, 须复核锚
     *     不得改判为绿）。 */
    ok = 0;
    {
        const char *line = winbuf;
        while (*line != '\0' && !ok) {
            const char *eol = strchr(line, '\n');
            size_t len = eol ? (size_t)(eol - line) : strlen(line);
            char tmp[SDM_MAX_PATH];
            size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;
            long w = 0;

            memcpy(tmp, line, cp);
            tmp[cp] = '\0';
            if (strstr(tmp, "width") != NULL) {
                if (strstr(tmp, "WIDTH_1") != NULL) {
                    ok = 1;
                } else if (sdm_first_int_after_on_line(tmp, "width", &w) && w == 1) {
                    ok = 1;
                }
            }
            if (eol == NULL) {
                break;
            }
            line = eol + 1;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(ok,
        "SD-A2 RED: InitializeSdCard 函数窗未取到 slot 宽度 = 1 赋值（slot.width=1 / "
        ".width=1 字面或 WIDTH_1 枚举形态; 1-bit 模式义务——SDMMC 四线模式需 D1-D3 "
        "脚, 本板只引出 D0）");

    /* ---- tooth 4 (clk/cmd/d0 宏消费绑定): 三宏在函数窗内在世（消费绑定——宏挪出
     *     函数窗 = 锚漂移红(infra)）。 */
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "SD_MMC_CLK_GPIO") != NULL,
        "SD-A2 RED: InitializeSdCard 函数窗未消费 SD_MMC_CLK_GPIO——clk 脚未取 "
        "config.h 宏（GPIO_NUM_38）");
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "SD_MMC_CMD_GPIO") != NULL,
        "SD-A2 RED: InitializeSdCard 函数窗未消费 SD_MMC_CMD_GPIO——cmd 脚未取 "
        "config.h 宏（GPIO_NUM_21）");
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "SD_MMC_D0_GPIO") != NULL,
        "SD-A2 RED: InitializeSdCard 函数窗未消费 SD_MMC_D0_GPIO——d0 脚未取 "
        "config.h 宏（GPIO_NUM_39）");

    /* ---- tooth 5 (format_if_mount_failed): false 同行在世 + true 形态负向禁令
     *     （坏卡/异格式不销毁数据、只报错降级——对偶 InitializeArt :805 既有范式）。 */
    n = sdm_count_lines_with_both(winbuf, "format_if_mount_failed", "false");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A2 RED: InitializeSdCard 函数窗未取到 format_if_mount_failed = false——"
        "坏卡/异格式不销毁数据、只报错降级义务被破坏（对偶 InitializeArt 既有范式）");
    n = sdm_count_lines_with_both(winbuf, "format_if_mount_failed", "true");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "SD-A2 RED: InitializeSdCard 函数窗存在 format_if_mount_failed = true 形态——"
        "坏卡/异格式会被格式化销毁数据（介质禁令对偶义务: 只报错降级）, 禁 true");

    /* ---- tooth 6 (max_files = 8 字面): 赋值行首个整数 = 8。 ------------------ */
    TEST_ASSERT_TRUE_MESSAGE(
        sdm_first_int_after_on_line(winbuf, "max_files", &v) && v == 8,
        sdm_fm("SD-A2 RED: InitializeSdCard 函数窗 max_files 取值非字面 8（取到 %ld; "
               "宏间接/符号常量形态取值失败同红, 须复核锚不得改判为绿）", v));

    /* ---- tooth 7 (构造序列接线): CustomBoard 构造窗内存在调用点。 ------------ */
    ctor = strstr(code, "CustomBoard() :");
    TEST_ASSERT_TRUE_MESSAGE(ctor != NULL,
        "SD-A2 RED(infra): waveshare-s3-rlcd-4.2.cc CustomBoard() : 构造锚缺席——"
        "构造函数被重写/改名, 须复核锚不得改判为绿");
    ctor_end = ctor + SDM_CTOR_WINDOW;
    if ((size_t)(ctor_end - code) > strlen(code)) {
        ctor_end = code + strlen(code);
    }
    hit = sdm_find_in(ctor, ctor_end, "InitializeSdCard();");
    TEST_ASSERT_TRUE_MESSAGE(hit != NULL,
        "SD-A2 RED: InitializeSdCard() 未挂进 CustomBoard 构造函数初始化序列（构造窗"
        "内无调用点; plan §2: InitializeArt 同排——挂载须发生在启动初始化序列内, 供"
        "将来消费方在启动后即查标志）");

    free(winbuf);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — SD-A3 失败降级路径源锚齿: InitializeSdCard 函数窗内 WITHOUT_ABORT
 * （不 panic）+ 裸 panic 形态禁令 + 返回码/句柄双检 + 失败置标志 s_sd_mounted=
 * false + ESP_LOGW 继续 + 成功置真 + sdmmc_card_print_info。
 * RED today（设计态）: InitializeSdCard 未落地 -> 首发红 = presence（自有锚）。
 * ------------------------------------------------------------------------ */
/* coverage: SD-A3 -- 失败降级路径（不 panic、不阻断启动）: ESP_ERROR_CHECK_WITHOUT_ABORT（降级形态）+ 裸 ESP_ERROR_CHECK( panic 形态计数 = 0 负向齿 + 返回码 ESP_OK / 卡句柄 NULL 双检 + 失败置标志 s_sd_mounted=false 同行在世 + ESP_LOGW 继续（降级 WARN 不静默）+ 成功 s_sd_mounted=true 置真 + sdmmc_card_print_info 卡信息打印（真机 A1「启动日志见 SD 卡信息打印」的结构承载） */
TEST_CASE("SD 卡挂载失败降级路径源锚: WITHOUT_ABORT + 置标志 + WARN 继续 + 成功打印 (SD-A3)",
          "[sd_card_mount][SD-A3][structural]")
{
    char *code;
    char *winbuf;
    const char *fn;
    const char *fn_end;
    int n;

    SDM_REQUIRE_ROOT();

    code = sdm_strip_c_comments_file(SDM_BOARD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SD-A3 RED(infra): 板 .cc unreadable");

    /* ---- tooth 1 (函数锚在世, 定义形态; 本包设计态红) ------------------------ */
    fn = strstr(code, "void InitializeSdCard");
    TEST_ASSERT_TRUE_MESSAGE(fn != NULL,
        "SD-A3 RED: InitializeSdCard 失败降级路径未落地（waveshare-s3-rlcd-4.2.cc 无"
        "挂载初始化可扫——void InitializeSdCard 定义缺席; 降级不崩 = 坏卡/无卡只置"
        "标志 + WARN, 绝不 panic、不阻断启动）");
    fn_end = fn + SDM_FN_WINDOW;
    if ((size_t)(fn_end - code) > strlen(code)) {
        fn_end = code + strlen(code);
    }
    winbuf = sdm_window_copy(fn, fn_end);

    /* ---- tooth 2 (降级形态): WITHOUT_ABORT 在世 ------------------------------ */
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "ESP_ERROR_CHECK_WITHOUT_ABORT") != NULL,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 ESP_ERROR_CHECK_WITHOUT_ABORT——"
        "失败路径未走降级形态（修复 = 挂载/总线初始化错误路径用 WITHOUT_ABORT 承接, "
        "不 panic）");

    /* ---- tooth 3 (panic 禁令负向齿): 裸 ESP_ERROR_CHECK( 计数 = 0 ------------- */
    n = sdm_count_sub(winbuf, "ESP_ERROR_CHECK(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        sdm_fm("SD-A3 RED: InitializeSdCard 函数窗存在 %d 处裸 ESP_ERROR_CHECK( panic "
               "形态——挂载失败即 abort 重启, 违背「降级不崩、不阻断启动」设计决策"
               "（WITHOUT_ABORT 前缀形态不误伤: 其 CHECK( 后接 _WITH）", n));

    /* ---- tooth 4 (返回码判空): ESP_OK 检查在世 -------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "ESP_OK") != NULL,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 ESP_OK 返回码判定——挂载返回码"
        "判定义务被破坏（修复 = esp_err_t 返回码与 ESP_OK 比较分流成功/失败）");

    /* ---- tooth 5 (句柄判空): NULL 检查在世 ------------------------------------ */
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "NULL") != NULL,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 NULL 句柄判空——卡句柄判空义务被"
        "破坏（plan §2: 返回码/卡句柄判空 -> 置标志降级）");

    /* ---- tooth 6 (失败置标志): s_sd_mounted 与 false 同行 --------------------- */
    n = sdm_count_lines_with_both(winbuf, "s_sd_mounted", "false");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 s_sd_mounted 置 false——失败降级"
        "置标志义务被破坏（将来消费方经 SdCardMounted() 查询降级态的依据）");

    /* ---- tooth 7 (WARN 继续): ESP_LOGW 在世 ----------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "ESP_LOGW") != NULL,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 ESP_LOGW——降级 WARN 日志义务被"
        "破坏（不静默、真机 A2 无卡观察点的日志承载）");

    /* ---- tooth 8 (成功路径): s_sd_mounted 置 true + 卡信息打印 ---------------- */
    n = sdm_count_lines_with_both(winbuf, "s_sd_mounted", "true");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 s_sd_mounted 置 true——挂载成功置"
        "标志义务被破坏");
    TEST_ASSERT_TRUE_MESSAGE(sdm_find_in(fn, fn_end, "sdmmc_card_print_info") != NULL,
        "SD-A3 RED: InitializeSdCard 函数窗未取到 sdmmc_card_print_info——挂载成功"
        "卡信息打印义务被破坏（真机 A1「启动日志见 SD 卡信息打印」的结构承载）");

    free(winbuf);
    free(code);
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — SD-A4 板级查询入口观察锚: SdCardMounted 定义在世且读 s_sd_mounted
 * 标志（本包零消费方——防死代码误删的观察锚; 禁常量真值 stub）。
 * RED today（设计态）: 符号不在世 -> 首发红 = presence。
 * ------------------------------------------------------------------------ */
/* coverage: SD-A4 -- 板级查询入口: SdCardMounted 定义在世（plan §2: 成员/静态 accessor, 供将来消费方判标志; 本包自身零消费方——观察锚, 防死代码误删）且读 s_sd_mounted 标志（定义窗内 s_sd_mounted + return 在世——禁常量真值 stub; 命名 = 冻结 seam contract [4] 钉死 SdCardMounted, 便于 mutation 定向取证） */
TEST_CASE("SD 卡挂载查询入口观察锚: SdCardMounted 读标志 (SD-A4)",
          "[sd_card_mount][SD-A4][structural]")
{
    char *code;
    const char *hit;
    int ok = 0;
    int scanned = 0;

    SDM_REQUIRE_ROOT();

    code = sdm_strip_c_comments_file(SDM_BOARD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "SD-A4 RED(infra): 板 .cc unreadable");

    /* 逐出现位扫描: 存在某一出现位, 其后窗内同时有 s_sd_mounted 与 return
     * （= 定义体读标志返回; 声明/调用点先行出现不误判——零消费方本包下即定义体）。 */
    hit = code;
    while ((hit = strstr(hit, "SdCardMounted")) != NULL && scanned < 8) {
        const char *win_end = hit + SDM_ACC_WINDOW;
        scanned++;
        if ((size_t)(win_end - code) > strlen(code)) {
            win_end = code + strlen(code);
        }
        if (sdm_find_in(hit, win_end, "s_sd_mounted") != NULL &&
            sdm_find_in(hit, win_end, "return") != NULL) {
            ok = 1;
            break;
        }
        hit += strlen("SdCardMounted");
    }

    TEST_ASSERT_TRUE_MESSAGE(ok,
        "SD-A4 RED: 板级查询入口 SdCardMounted() 未落地（waveshare-s3-rlcd-4.2.cc "
        "无定义体读 s_sd_mounted 的 accessor）——防死代码误删观察锚缺席（本包零消费"
        "方, plan §2: 暴露板级查询入口供将来消费方判标志; 修复 = 定义 bool "
        "SdCardMounted() 读 s_sd_mounted 返回）");

    free(code);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-16 初版冻结（TaskPackage 261016-sd-card-mount; acceptance[] SD-A1~A6
 *               + mutationEvidence 骨架为冻结边界; plan.md reviewed + human 触
 *               点①三决: ①能力就绪 scope 收窄 ②LFN 不开验收限 8.3 ③architecture
 *               .md:232 随批 doc-sync）。四 TEST_CASE 全源锚结构齿（SD-A1 四宏 +
 *               注释包号/来源 + 扫描器自检; SD-A2 定义 + 构造接线 + 挂载调用五
 *               件; SD-A3 降级路径七件含 panic 禁令负向齿; SD-A4 查询入口观察
 *               锚）。SD-A5/A6 真机轮 live-manual structural 豁免不落 C 齿
 *               （AB-A7 先例口径）; A3 设备编译闸不落 C 齿（intake 约束 3, 归
 *               verify firmware-build 闸）。冻结落盘冒烟 = red-baseline 独立
 *               执行架实编 + 实跑（净 HEAD 设计态红形状逐齿核对, 红名 verbatim
 *               见交付报告）+ host 套件全量基线（既有齿零新增红判读）。
 *   2026-10-16 修订（真机门停等点第 2 次接续; human 真机轮裁定 0263a7a3 SD-A5 PASS +
 *               416e5c42 SD-A6 豁免）: **仅注释面措辞修订, C 齿逻辑零改动**（SD-A1~A4
 *               断言字节不变）——[5] 节 + coverage map SD-A5/SD-A6 行改写: SD-A5 由
 *               「真机轮 live-manual 停门」改记「已执行 PASS」（有卡轮兑现: 卡信息
 *               打印 + /sdcard SDMMC 1-bit 挂载 + ROUNDTRIP PASS）, 子项③ 原理图
 *               三根脚目视核对改实证反证形态（未单独执行）; SD-A6 由「真机轮
 *               live-manual 停门」改「本机不可执行豁免 + 结构性证据替代」（卡永久
 *               在位、无卡态本机不可达; 替代证据 = SD-A3/SD-A4 代码面静态结构断言,
 *               明确不冒充真机观察）。SD-A5 判定 PASS 不变; 修订留痕逐条见 [5] 节与
 *               coverage 行。
 *   2026-10-18 重锚（261016 SD-A2 tooth 6 期望值 5 → 8 + 四处文字镜像）——**非
 *               tighten-only**: 执行者 acceptance-author, 执行包 261018-sd-log-persist
 *               （本文件已先入该包 changeManifest, 先扩单后动文件）, 所属包
 *               261016-sd-card-mount（本文件为该包冻结件）。
 *               Reason: 跨包字面冲突——261016 冻结件钉 max_files=5, 执行包
 *               261018-sd-log-persist 的 plan 要求 8, host 全量新增 1 红; 重锚前
 *               host 实跑 203 Tests 4 Failures（3 条 p1_auxbar 先存红 AB-A1/A2/A4
 *               + 1 条本冲突）。
 *               授权: human 2026-10-18 窄口径批准 A 原话要点「授权 acceptance-author
 *               把 261016 SD-A2 tooth 6 的期望值 5→8, 连同四处文字镜像一起重锚, 并在
 *               该文件尾 append REVISION LOG（不覆盖 2026-10-16 既有两条）, 其余断言
 *               与 261016 其他收尾制品零改动」, decision 88a08bbd-c542-41e5-bdc4-
 *               fb2739dc24ef; 配套 architect 1g 正式扩单回评（2026-10-18, 本包
 *               261018-sd-log-persist）。
 *               范围: 仅 tooth 6 期望值（:657 `v == 5` → `v == 8`）+ 四处文字镜像
 *               （:49 / :94 / :556 / :559 的 max_files 描述值）+ tooth 6 注释头与红名
 *               文本（:655 / :658）+ 本条日志; **C 齿判别机制与其余断言零改动**（比较
 *               运算符、扫描窗大小、helper 语义、其余 coverage 行、TEST_CASE 数与
 *               acceptance id 全部未动; 代码面 git diff --numstat = 7 / 7（日志
 *               append 前实测）, 全文件删除行恒为 7、增行全部落在本条日志区, 删除行
 *               恰为 :49 / :94 / :556 / :559 / :655 / :657 / :658 七行）。
 *               **本条为非 tighten-only 修订**: 改的是既有断言语义, 不属
 *               DEV-GOVERNANCE「验收测试纪律」§6 的纯 additive 窗口, 依据 = human
 *               2026-10-18 一次性窄口径授权（见上）。
 *               sha256: 6ff4fca19d05cd0ecf82fd073dd1750ab90d24ddec3e46f7535ef38af3d3ed4a
 *               → ec74a68ab41dd2a49552db21649da92d59daf92cfd7a21513d7eddf7b1dc8b74
 *               （后值口径 = 7 行重锚落盘后的工作树全文件实测, 本条日志 append 之前;
 *               自指不可闭合——含本条日志的最终字节哈希见 261018-sd-log-persist
 *               提交记录）。
 *               证据: 重锚后 host 全量实跑 = 203 Tests 3 Failures（仅 p1_auxbar
 *               AB-A1/A2/A4 先存红; 本文件四 TEST_CASE 全绿）——WSL host_app 全量
 *               build + host_infra_test.elf 实跑, 2026-10-18 重锚落盘当场执行。
 *               历史边界: 261016 的 taskpackage / review-packet / mutation 记录保留
 *               原文不动; 261016 mutation 记录中 m2「5 → 12」对本文件不再适用, 重放
 *               请改用 8 → 12 或 8 → 5。
 *               防先例（逐字）:
 *               本条为 human 2026-10-18 一次性窄口径授权（decision 88a08bbd-c542-41e5-bdc4-fb2739dc24ef）：仅覆盖 261016 SD-A2 tooth 6 期望值 5→8 及四处文字镜像，不构成『plan 提到某值即可改任何钉该值的冻结齿』的先例；后续任何冻结齿重锚仍须逐案 human 窄口径裁决 + architect 1g 扩单评估（或走正常改验收通道），且一律先扩 manifest 后动文件，不得援引本条为隐性先例，也不得据此放宽任何其他断言。
 * --------------------------------------------------------------------------- */
