/*
 * story_page_stack.test.c — 包 260829-hardware-validation（真机语音轮 2）栈溢出
 * 修复的**回归守卫**补充单测（普通 `.test.c`，非冻结验收，HOST_TEST_CONVENTIONS [C4.3]）。
 *
 * 现场：设备处理服务端 kidpal/display 的 story_text 时 `***ERROR*** A stack overflow
 * in task main`（backtrace 腐蚀、每次凶手任务不同 = 跨任务腐蚀）。净根因不是 main：
 * 整条 MCP 调用链同步跑在 esp-ml307 的 `ssl_receive` 任务上，该任务栈 = 4096B
 * （managed_components/78__esp-ml307/src/esp/esp_ssl.cc:62，第三方件不可抬），而
 * story_text 路由在同一条栈上叠了 `kidpal_story_text_t parsed`(2208B) +
 * `kidpal_story_text_parse` 内部的 `text_buf[2048]` 等大中缓冲(2208B) ≥ 4.4KB。
 * 修法 = 大对象一律上堆 / 就地解析进 out。本文件把该纪律钉成机械齿，防止回归。
 *
 * 齿族（三类，缺一即守卫不完整）：
 *   S1 尺寸算术钉行 —— 冻结结构体尺寸 + 「两份大缓冲 > 4096B 栈预算」的事实钉行；
 *   S2 树级扫描齿 —— 已删除的大栈声明不得回潮（禁词），堆分配形态必须在位（须词）；
 *   S3 解析语义钉 —— 就地解析后 safe-truncate / NUL 终止 / 缺字段与畸形 -1 / 未知
 *      扩展位容忍 / title 缺省 "" 全部不变（out 预填垃圾，专测去掉中段拷贝后的终止性）。
 *
 * Run protocol: 随 host runner 一并执行（本文件已接线进
 * firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS）：
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 */
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_LINUX)

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "story_page.h"
#include "schedule_page.h"

/* esp-ml307 ssl_receive 任务栈预算（字节）——第三方件硬编码，见文件头。 */
#define SSL_RECEIVE_STACK_BUDGET 4096
/* 本守卫扫描的工作树文件（相对仓根）。 */
#define SSP_F_STORY_C "firmware/main/display/story_page.c"
#define SSP_F_MCP_CC  "firmware/main/mcp_server.cc"
#define SSP_MAX_READ  (8u * 1024u * 1024u)
#define SSP_MAX_PATH  1024

/* -------------------------------------------------------------------------- *
 * 工作树定位 + 读取（hardware_validation.acceptance.c 的 hwv_* 同款先例）
 * ------------------------------------------------------------------------ */

static char ssp_root[SSP_MAX_PATH];
static int ssp_root_ready;

static int ssp_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[SSP_MAX_PATH];

    if (ssp_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        FILE *f = fopen(probe, "rb");
        if (f != NULL) {
            fclose(f);
            snprintf(ssp_root, sizeof(ssp_root), "%s", cands[i]);
            ssp_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *ssp_read(const char *rel)
{
    char path[SSP_MAX_PATH];
    size_t root_len;
    size_t rel_len;
    FILE *f;
    long sz;
    char *buf;

    if (!ssp_locate_root()) {
        return NULL;
    }
    /* 手工拼接而非 snprintf("%s/%s")：gcc -Werror=format-truncation 无法证明 rel 长度
     * 上界（同 hwv_read 的语义，但那边 rel 全为字面量故不触发）。 */
    root_len = strlen(ssp_root);
    rel_len = strlen(rel);
    if (root_len + 1 + rel_len + 1 > sizeof(path)) {
        return NULL;
    }
    memcpy(path, ssp_root, root_len);
    path[root_len] = '/';
    memcpy(path + root_len + 1, rel, rel_len + 1);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SSP_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM 必须响，不得被读成「红」 */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

/* -------------------------------------------------------------------------- *
 * S1 — 尺寸算术钉行
 * ------------------------------------------------------------------------ */

TEST_CASE("story stack guard: frozen struct sizes pinned against the 4096B ssl_receive budget",
          "[story_page][stack][sizes]")
{
    /* 结构体尺寸 = 修复论证的算术基础；字段增删会改这些数（改数须同步改本守卫与
     * mcp_server.cc 头注的「参考尺寸」清单）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(2208, (int)sizeof(kidpal_story_text_t),
        "kidpal_story_text_t 尺寸偏移（64+96+2048）—— 栈预算论证须重算");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2824, (int)sizeof(kidpal_story_list_t),
        "kidpal_story_list_t 尺寸偏移（8 + 16*176）—— 栈预算论证须重算");
    TEST_ASSERT_EQUAL_INT_MESSAGE(108, (int)sizeof(kidpal_story_view_t),
        "kidpal_story_view_t 尺寸偏移（11*int + char[64]）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(112, (int)sizeof(kidpal_schedule_item_t),
        "kidpal_schedule_item_t 尺寸偏移（16+48+48）");

    /* 事实钉行：任意两份 story 大结构体同时在栈上即超 4096B 预算（连单一 list 加
     * 一条调用链也不宽裕）—— 这正是真机轮 2 的溢出形态，故三者必须堆分配。 */
    TEST_ASSERT_TRUE_MESSAGE(sizeof(kidpal_story_text_t) * 2 > SSL_RECEIVE_STACK_BUDGET,
        "算术前提失效：两份 text 结构体不再超 4096B 栈预算");
    TEST_ASSERT_TRUE_MESSAGE(sizeof(kidpal_story_list_t) + sizeof(kidpal_story_text_t) >
                                 SSL_RECEIVE_STACK_BUDGET,
        "算术前提失效：list + text 不再超 4096B 栈预算");
    /* view(108B) / alert(488B) 级仍是小对象，允许留栈（本包的许可上界）。 */
    TEST_ASSERT_TRUE_MESSAGE(sizeof(kidpal_story_view_t) <= 1024,
        "kidpal_story_view_t 越过 1KB 留栈上界 —— 须改堆分配并更新守卫");
}

/* -------------------------------------------------------------------------- *
 * S2 — 树级扫描齿：禁词不得回潮 / 须词必须在位
 * ------------------------------------------------------------------------ */

static int ssp_has(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle = 测试自身写错 */
    return strstr(hay, needle) != NULL;
}

TEST_CASE("story stack guard: large stack buffers stay out of the ssl_receive chain",
          "[story_page][stack][scan]")
{
    char *story_c = ssp_read(SSP_F_STORY_C);
    char *mcp_cc = ssp_read(SSP_F_MCP_CC);

    /* 读不到工作树文件 = 扫描器假绿风险，必须红（hardware_validation 扫描器自检同纪律）。 */
    TEST_ASSERT_NOT_NULL_MESSAGE(story_c, "stack 扫描器失效：读不到 " SSP_F_STORY_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp_cc, "stack 扫描器失效：读不到 " SSP_F_MCP_CC);

    /* --- 须词：修复形态必须在位（防止「把代码删了」混过禁词齿） --- */
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(mcp_cc, "std::make_unique<kidpal_story_text_t>"),
        "story_text 路由回潮栈分配：缺 std::make_unique<kidpal_story_text_t>");
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(mcp_cc, "std::make_unique<kidpal_story_list_t>"),
        "story_list 路由回潮栈分配：缺 std::make_unique<kidpal_story_list_t>");
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(mcp_cc, "std::make_shared<kidpal_story_list_t>"),
        "Schedule 闭包列表快照回潮栈拷贝：缺 std::make_shared<kidpal_story_list_t>");
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(mcp_cc, "std::shared_ptr<kidpal_story_text_t>"),
        "盖章/阅读态闭包缺 shared_ptr<kidpal_story_text_t>（unique_ptr 不可按值捕获）");
    TEST_ASSERT_TRUE_MESSAGE(
        ssp_has(mcp_cc, "std::make_shared<std::array<kidpal_schedule_item_t, 16>>"),
        "schedule 路由 items[16](1792B) 回潮栈分配");
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(story_c, "ksy_string(&c, out->text"),
        "kidpal_story_text_parse 回潮中段缓冲：应就地解析进 out->text");

    /* --- 防回归注释锚：两处纪律声明不得被顺手删掉 --- */
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(mcp_cc, "ssl_receive"),
        "mcp_server.cc 的 ssl_receive 栈纪律注丢失（回归防线本体）");
    TEST_ASSERT_TRUE_MESSAGE(ssp_has(story_c, "ssl_receive"),
        "story_page.c 的 ssl_receive 栈纪律注丢失（回归防线本体）");

    /* --- 禁词：真机轮 2 的大栈声明形态 --- */
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_story_text_t parsed = {}"),
        "回潮：kidpal_story_text_t 作栈局部（2208B 压在 4096B 的 ssl_receive 栈上）");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_story_list_t parsed = {}"),
        "回潮：kidpal_story_list_t 作栈局部（2824B）");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_story_list_t list_snapshot = "),
        "回潮：列表快照作栈局部再按值捕获进 Schedule 闭包");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_story_text_t ts = "),
        "回潮：ScheduleStoryRender 正文快照作栈局部");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_story_list_t ls = "),
        "回潮：ScheduleStoryRender 列表快照作栈局部");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(mcp_cc, "kidpal_schedule_item_t items[16]"),
        "回潮：课表 items[16] 作栈局部（1792B ×2）");
    TEST_ASSERT_TRUE_MESSAGE(!ssp_has(story_c, "char text_buf[2048]"),
        "回潮：kidpal_story_text_parse 内 2048B 中段缓冲");

    free(story_c);
    free(mcp_cc);
}

/* -------------------------------------------------------------------------- *
 * S3 — 解析语义钉（就地写入 out 后仍与冻结 contract 逐字等价）
 * ------------------------------------------------------------------------ */

TEST_CASE("story stack guard: in-place parse keeps truncate/termination/reject semantics",
          "[story_page][stack][semantics]")
{
    /* out 预填垃圾（堆上，模拟调用方复用内存）：缺 title 必须落 ""、正文必须 NUL 终止，
     * 不再经中段拷贝 —— 本齿专测该路径。 */
    kidpal_story_text_t *st = (kidpal_story_text_t *)malloc(sizeof(*st));
    assert(st != NULL);
    memset(st, 'X', sizeof(*st));

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, kidpal_story_text_parse("{\"id\":\"2026-08-27-maoxian\",\"text\":\"从前\"}", st),
        "canonical 形状载荷解析失败（就地解析回归）");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2026-08-27-maoxian", st->id, "id 未就地写入");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", st->title, "title 缺省须容错为 \"\"");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("从前", st->text, "text 未就地写入");

    /* 超长正文：safe-truncate 至 sizeof-1 且恒 NUL 终止（旧中段缓冲同款上界）。 */
    {
        size_t body = 3000;
        char *big = (char *)malloc(body + 64);
        size_t i;
        assert(big != NULL);
        memcpy(big, "{\"id\":\"z\",\"text\":\"", 18);
        for (i = 0; i < body; i++) {
            big[18 + i] = 'a';
        }
        big[18 + body] = '"';
        big[19 + body] = '}';
        big[20 + body] = '\0';
        memset(st, 'Y', sizeof(*st));
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_text_parse(big, st),
            "超长正文仍须解析成功（分页在渲染面，不在解析处丢）");
        TEST_ASSERT_EQUAL_INT_MESSAGE(2047, (int)strlen(st->text),
            "超长正文未截断到 sizeof(text)-1");
        free(big);
    }

    /* 畸形面：一律 -1，调用方必须走显式降级。 */
    memset(st, 'Z', sizeof(*st));
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_text_parse(NULL, st), "NULL 载荷须 -1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_text_parse("{\"id\":\"x\"}", st),
        "缺 text 须 -1（无正文不可阅读）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_text_parse("{\"text\":\"从前\"}", st),
        "缺 id 须 -1（id 是回看/回听键）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_text_parse("{\"id\":1,\"text\":\"a\"}", st),
        "id 非 string 须 -1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_story_text_parse("{\"id\":\"x\",\"text\":\"a\",}", st),
        "尾逗号（结构畸形）须 -1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_story_text_parse("{\"id\":\"x\",\"text\":\"a\"}junk", st),
        "尾部垃圾须 -1");
    /* 未知扩展位（含生字标注）容忍不消费（P3-E4）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_story_text_parse("{\"id\":\"z1\",\"text\":\"猫\",\"newWordMarks\":[\"猫\"],"
                                "\"story_highlight\":[0,1]}", st),
        "未知扩展字段须容忍");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("z1", st->id, "扩展位应不影响已解析字段");
    free(st);
}

#endif /* CONFIG_IDF_TARGET_LINUX */
