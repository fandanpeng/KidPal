/*
 * record_dialogue_v7_fix.test.c — 261019-p3-record-dialogue-guide fixer 轮二 guard 单测
 * （2026-10-20）。承载两缺陷族修复的回归守卫：
 *
 *   D2 固件三次静默崩（UAF / 陈旧 s_local_page / 定时器真相核 / 记录语境守卫）：
 *     - TEST 1 行为半：keybar v7 键帽集（录音 收尾/丢弃、整理 双槽留空、帧分叉
 *       打断/留空）+ 既有子态零回归 + 语音轮冻结闭集字面零混入（KB-E2 同源红线）。
 *     - TEST 2 结构半（源锚; 渲染节点 / mcp 定时器为设备 TU, host 不可编）：
 *       clean 联动缝消费点、渲染成功出口页归属真相缝、定时器前台真相核、
 *       show_home / voice_end 记录语境守卫、指针置空单点。
 *
 *   D3 P3 会话两页 v7 重排：资产落点 + 气泡文案单源消费（TEST 2 同文件结构半）。
 *
 * 放置纪律（HOST_TEST_CONVENTIONS.md [C4.2]/[C4.3]）：普通单测 *.test.c；至少一个
 * 真实 assert() 逻辑守卫（本文件 TEST 1 尾部 invariants 块）。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "unity.h"
#include "keybar.h"
#include "story_page.h"

/* -------------------------------------------------------------------------- *
 * 源锚工具（读真实工作树文件; record_dialogue_guide.acceptance.c rdl_* 同款裁法,
 * 自足式重实现避免消费冻结件内部符号）。仓根定位 = 候选点探测（不依赖调用 cwd 的
 * 绝对路径假设, rdl_locate_root 同纪律）。
 * ------------------------------------------------------------------------ */

static char rdf_root[512];
static int  rdf_root_ready;

static void rdf_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[600];
    size_t i;

    if (rdf_root_ready) {
        return;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (access(probe, R_OK) == 0) {
            snprintf(rdf_root, sizeof(rdf_root), "%s", cands[i]);
            rdf_root_ready = 1;
            return;
        }
    }
}

static char *rdf_read_file(const char *repo_rel)
{
    char path[1024];
    FILE *f;
    long sz;
    char *buf;
    size_t got;

    rdf_locate_root();
    if (!rdf_root_ready) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", rdf_root, repo_rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly */
    got = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[got] = '\0';
    return buf;
}

/* 剥 C 块注释与行注释（源锚计数口径 = 代码面, 注释不算数）。 */
static char *rdf_strip_comments(const char *src)
{
    size_t cap = strlen(src) + 1;
    char *out = (char *)malloc(cap);
    size_t i = 0, o = 0;
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    while (src[i] != '\0') {
        if (src[i] == '/' && src[i + 1] == '*') {
            i += 2;
            while (src[i] != '\0' && !(src[i] == '*' && src[i + 1] == '/')) {
                i++;
            }
            if (src[i] != '\0') {
                i += 2;
            }
            continue;
        }
        if (src[i] == '/' && src[i + 1] == '/') {
            while (src[i] != '\0' && src[i] != '\n') {
                i++;
            }
            continue;
        }
        out[o++] = src[i++];
    }
    out[o] = '\0';
    return out;
}

static int rdf_count(const char *hay, const char *needle)
{
    int n = 0;
    size_t nl = strlen(needle);
    const char *p = hay;
    if (nl == 0) {
        return 0;
    }
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p += nl;
    }
    return n;
}

/* 区间切片（start 锚后起, 至 end 锚前止; 任一锚缺失返回 NULL）。 */
static char *rdf_slice(const char *src, const char *start, const char *end)
{
    const char *s = strstr(src, start);
    const char *e;
    char *out;
    size_t len;

    if (s == NULL) {
        return NULL;
    }
    e = strstr(s, end);
    if (e == NULL || e < s) {
        return NULL;
    }
    len = (size_t)(e - s);
    out = (char *)malloc(len + 1);
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}

static void rdf_require_root(void)
{
    /* 仓根探测（候选点 + firmware/main/CMakeLists.txt 探针, rdl_locate_root 同纪律）。 */
    rdf_locate_root();
    TEST_ASSERT_TRUE_MESSAGE(rdf_root_ready,
        "RDV7 RED(infra): 仓根不可达——候选点探测零命中（firmware/main/CMakeLists.txt）");
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — D3/D2 行为半: keybar v7 键帽集（keybar.c 纯逻辑单源, host 强符号真跑）。
 * ------------------------------------------------------------------------ */
TEST_CASE("record guide v7 fix: keybar story/rec slots v7 keycap set (RDV7-A1)",
          "[record_dialogue_v7_fix][RDV7-A1][behavior]")
{
    const char *left = NULL;
    const char *right = NULL;

    /* 帧 A 常规录音: 左「收尾」右「丢弃」（v7 破坏性提示; 旧右「结束」退役）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_slots(KSTORY_PAGE_RECORDING, &left, &right) == 0 &&
        strcmp(left, "收尾") == 0 && strcmp(right, "丢弃") == 0,
        "RDV7-A1 RED: 录音态键帽非 [收尾][丢弃]——v7 p3-recording 键帽集未落 keybar "
        "单源（右槽破坏性提示「丢弃」承担 BOOT 误按缓释）");

    /* 整理过渡态: 双槽留空（非交互, 键帽条照常驻留）。 */
    left = right = NULL;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_slots(KSTORY_PAGE_ORGANIZING, &left, &right) == 0 &&
        strcmp(left, "") == 0 && strcmp(right, "") == 0,
        "RDV7-A1 RED: 整理态键帽非双槽留空——v7 state-organizing 非交互裁定未落");

    /* 帧分叉: 常规 -> 收尾/丢弃; 引导播报中 -> 打断/留空（BOOT 无操作）。 */
    left = right = NULL;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_rec_slots(0, &left, &right) == 0 &&
        strcmp(left, "收尾") == 0 && strcmp(right, "丢弃") == 0,
        "RDV7-A1 RED: rec_slots(0) 非 [收尾][丢弃]——帧 A 键帽分叉缺位");
    left = right = NULL;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_rec_slots(1, &left, &right) == 0 &&
        strcmp(left, "打断") == 0 && strcmp(right, "") == 0,
        "RDV7-A1 RED: rec_slots(1) 非 [打断][留空]——帧 B 键位分叉缺位（v7 "
        "p3-recording-guide R2 裁定: 右留空不画键帽盒, 免诱导误按）");

    /* 既有子态零回归（v1.47/v6 冻结值域原样）。 */
    left = right = NULL;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_slots(KSTORY_PAGE_READER, &left, &right) == 0 &&
        strcmp(left, "长按退出") == 0 && strcmp(right, "翻屏") == 0 &&
        kidpal_keybar_story_slots(KSTORY_PAGE_DELETE, &left, &right) == 0 &&
        strcmp(left, "删掉") == 0 && strcmp(right, "算了") == 0 &&
        kidpal_keybar_story_slots(KSTORY_PAGE_BODY_UNAVAILABLE, &left, &right) == 0 &&
        strcmp(left, KSTORY_BODY_UNAVAILABLE_KEYCAP_KEY) == 0 &&
        strcmp(right, KSTORY_BODY_UNAVAILABLE_KEYCAP_BOOT) == 0,
        "RDV7-A1 RED: 既有 story_slots 子态值域漂移——v7 重排只许动录音/整理两行");

    /* 防御: NULL out -> -1。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_story_slots(KSTORY_PAGE_RECORDING, NULL, &right) == -1 &&
        kidpal_keybar_story_rec_slots(0, &left, NULL) == -1,
        "RDV7-A1 RED: NULL out 未拒绝（期望 -1）——畸形入参不得 UB");

    /* 逻辑守卫（conventions [C4.2] 真实 assert 义务）: 留空槽必须显式空串
     * （消费方以 '\0' 判留空隐藏键帽盒; NULL 会跳过置空 = 旧文案残留）。 */
    assert(kidpal_keybar_story_rec_slots(1, &left, &right) == 0);
    assert(right[0] == '\0');

    /* KB-E2 同源红线: 语音轮冻结闭集字面（「说完啦」等）不得混入 keybar.c 记录
     * 语境键帽（记录语境不是语音轮态; keybar_three_slot KKB-E2 同款树扫负锚）。 */
    {
        char *kbc;
        rdf_require_root();
        kbc = rdf_strip_comments(rdf_read_file("firmware/main/display/keybar.c"));
        TEST_ASSERT_NOT_NULL_MESSAGE(kbc, "RDV7-A1 RED(infra): keybar.c unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(kbc, "说完啦") == 0 && rdf_count(kbc, "不等了") == 0,
            "RDV7-A1 RED: keybar.c 混入语音轮冻结闭集 keycap 字面——记录语境键帽 "
            "集与语音轮闭集串域（KB-E2 红线）");
        free(kbc);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — D2 结构半（源锚; 设备 TU 不进 host 构建, 判据 = 源锚代理）。
 * ------------------------------------------------------------------------ */
TEST_CASE("record guide v7 fix: D2 linkage seams and guards source anchors (RDV7-A2)",
          "[record_dialogue_v7_fix][RDV7-A2][structural]")
{
    char *sp;
    char *win;

    rdf_require_root();

    /* ---- tooth 1: story 渲染入口 clean 联动（UAF 根修承重点）---------------- */
    sp = rdf_strip_comments(rdf_read_file("firmware/main/display/story_page.c"));
    TEST_ASSERT_NOT_NULL_MESSAGE(sp, "RDV7-A2 RED(infra): story_page.c unreadable");
    win = rdf_slice(sp, "int kidpal_story_page_render(", "kidpal_story_view_init(");
    TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A2 RED(infra): 渲染函数区切片失败");
    {
        const char *clean_at = strstr(win, "lv_obj_clean(scr);");
        const char *seam_at;
        TEST_ASSERT_NOT_NULL_MESSAGE(clean_at,
            "RDV7-A2 RED: 渲染入口 lv_obj_clean 缺席——渲染面被改写? 须复核");
        seam_at = strstr(clean_at, "kidpal_render_node_notify_screen_cleaned()");
        TEST_ASSERT_NOT_NULL_MESSAGE(seam_at,
            "RDV7-A2 RED: lv_obj_clean 后未联动 kidpal_render_node_notify_screen_cleaned"
            "——clean 销毁渲染节点 s_kidpal_img 而指针不清 = 悬垂（lv_obj_is_valid 出帧守卫"
            "对地址复用假阳性不可靠, 现场 1/3 崩点同型; UAF 根修承重点）");
        TEST_ASSERT_TRUE_MESSAGE(
            (size_t)(seam_at - clean_at) < 400,
            "RDV7-A2 RED: clean 联动缝与 lv_obj_clean 距离过远——联动必须紧贴 clean "
            "时刻（指针置空晚于任何 tick 复用窗口 = 修不闭环）");
    }

    /* ---- tooth 2: 渲染成功出口页归属真相缝（陈旧 s_local_page 根修承重点）---- */
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "kidpal_render_node_story_page_front()") >= 1,
        "RDV7-A2 RED: 渲染成功出口未消费 kidpal_render_node_story_page_front——"
        "s_local_page 停留陈旧 HOME（voice_end [R3] on_story_front 误判走首页重建 + "
        "首页动画 tick 悬垂 UAF 的共同前提, 现场 1 45115 / 现场 3 68335 崩链）");
    free(win);
    win = NULL;

    /* ---- tooth 3: 录音演出定时器前台真相核 ---------------------------------- */
    {
        char *mcp = rdf_strip_comments(rdf_read_file("firmware/main/mcp_server.cc"));
        TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "RDV7-A2 RED(infra): mcp_server.cc unreadable");
        win = rdf_slice(mcp, "void StoryRecTimerCb(", "void StartStoryRecTimer()");
        TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A2 RED(infra): StoryRecTimerCb 区切片失败");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(win, "kidpal_render_node_story_page_is_local()") >= 1,
            "RDV7-A2 RED: 录音演出定时器渲染前零前台真相核——基层被其它路径重建后"
            "仍每 100ms 盖渲染录音页（现场 1 45175 同型; 演出断一拍无害, 盖回 = 错）");
        free(win);
        free(mcp);
    }

    /* ---- tooth 4: 渲染节点守卫与指针置空单点 -------------------------------- */
    {
        char *rn = rdf_strip_comments(
            rdf_read_file("firmware/main/display/kidpal_render_node.cc"));
        TEST_ASSERT_NOT_NULL_MESSAGE(rn, "RDV7-A2 RED(infra): kidpal_render_node.cc unreadable");
        win = rdf_slice(rn, "void kidpal_render_node_notify_screen_cleaned(void)",
                        "void kidpal_render_node_story_page_front(void)");
        TEST_ASSERT_NOT_NULL_MESSAGE(win,
            "RDV7-A2 RED: clean 联动缝定义缺席（kidpal_render_node.cc）");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(win, "s_kidpal_img = NULL;") >= 1,
            "RDV7-A2 RED: clean 联动缝未置空 s_kidpal_img——悬垂指针根修单点缺位");
        free(win);
        win = rdf_slice(rn, "void kidpal_render_node_show_home(void)",
                        "void kidpal_render_node_show_chat(void)");
        TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A2 RED(infra): show_home 区切片失败");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(win, "rn_record_session_live_locked()") >= 1,
            "RDV7-A2 RED: show_home 零记录语境守卫——记录会话覆盖演出期间首页重建"
            "未降级 no-op（现场 1「录音会话中渲染了首页」的服务端路由候选封口）");
        free(win);
        win = rdf_slice(rn, "void kidpal_render_node_voice_end(void)",
                        "void kidpal_render_node_begin_page(void)");
        TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A2 RED(infra): voice_end 区切片失败");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(win, "rn_record_session_live_locked()") >= 1,
            "RDV7-A2 RED: voice_end 前台回落判据未放宽联挂记录语境——记录语境下"
            "else 首页重建分支残缝（现场 1 崩链前提之一）");
        free(win);
        free(rn);
    }

    /* ---- tooth 5: D3 v7 资产落点 + 气泡文案单源消费 ------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(sp, "pose_record_96.bin") >= 1 &&
        rdf_count(sp, "pose_think_96.bin") >= 1 &&
        rdf_count(sp, "qmark_trio.bin") >= 1 &&
        rdf_count(sp, "rec_wave.bin") >= 1 &&
        rdf_count(sp, "ic_wave_40.bin") >= 1 &&
        rdf_count(sp, "ic_cloud_40_t2.bin") >= 1,
        "RDV7-A2 RED: P3 会话三帧 v7 资产落点缺席（pose-record / pose-think / "
        "qmark_trio / rec_wave / ic_wave_40 / ic_cloud_40_t2——/art 图集闭集在库）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(sp, "KSTORY_RECORDING_BUBBLE") >= 1 &&
        rdf_count(sp, "KSTORY_ORGANIZING_BUBBLE") >= 1,
        "RDV7-A2 RED: 帧 A/帧 C 气泡文案未经 story_page.h 单源宏消费（措辞单源 = "
        "v7 设计稿规格表, 渲染面零字面重抄）");
    free(sp);

    /* ---- tooth 6: 气泡文案单源逐字（story_page.h 宏面）---------------------- */
    {
        char *hdr = rdf_read_file("firmware/main/display/story_page.h");
        TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "RDV7-A2 RED(infra): story_page.h unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            rdf_count(hdr, "你讲吧，我听着呢") >= 1 &&
            rdf_count(hdr, "我来整理一下…") >= 1,
            "RDV7-A2 RED: story_page.h 无 v7 终稿气泡文案单源宏（逐字 = v7 P3 规格"
            "表「气泡文案（终稿）」行）");
        free(hdr);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — D4-5 置位链结构半（261020 fixer R4; 源锚; 设备 TU 不进 host 构建）:
 * 「引导播报中」子态置位链（escalation 067bd067 销账）——tts 帧沿派生桥在世：
 * application.cc tts start 分支置位沿(1) / tts stop 分支复位沿(0)（复位沿必须先于
 * Speaking 出口守卫——打断离场后 tts stop 到达时出口判据不吞复位沿）+ mcp_server.cc
 * 桥定义（RECORDING 页态门 + 幂等 + 渲染节点缓存同步）。
 * ------------------------------------------------------------------------ */
TEST_CASE("record guide v7 fix: D4-5 guide speech substate edge chain source anchors (RDV7-A3)",
          "[record_dialogue_v7_fix][RDV7-A3][structural]")
{
    char *app;
    char *mcp;
    char *win;

    rdf_require_root();

    /* ---- tooth 1: application.cc tts start 分支置位沿(1) --------------------- */
    app = rdf_strip_comments(rdf_read_file("firmware/main/application.cc"));
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "RDV7-A3 RED(infra): application.cc unreadable");
    win = rdf_slice(app, "strcmp(type->valuestring, \"tts\") == 0",
                    "strcmp(state->valuestring, \"sentence_start\") == 0");
    TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A3 RED(infra): tts 分支头段切片失败（锚漂移须复核）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "KidpalStoryGuideSpeechEdge(1)") >= 1,
        "RDV7-A3 RED: tts start 分支无置位沿(1) —— 引导播报中子态置位链缺失回归"
        "（escalation 067bd067 本体: 全仓无路径置 rec_substate=1, 键帽不切帧 B, "
        "BOOT 毁稿分叉敞口）");
    /* 负锚（VQ-A1 增补齿同域）: 置位链不得引入语音轮场景切换载体。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "SetDeviceState(kDeviceStateSpeaking)") == 0 &&
        rdf_count(win, "kidpal_voice_round_scene(") == 0,
        "RDV7-A3 RED: tts start 分支混入语音轮场景切换载体——KVR-A1 冻结齿同域回归");
    free(win);

    /* ---- tooth 2: tts stop 分支复位沿(0) 先于 Speaking 出口守卫 --------------- */
    win = rdf_slice(app, "strcmp(state->valuestring, \"stop\") == 0",
                    "strcmp(state->valuestring, \"sentence_start\") == 0");
    TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A3 RED(infra): tts stop 分支切片失败");
    {
        const char *edge_at = strstr(win, "KidpalStoryGuideSpeechEdge(0)");
        const char *guard_at = strstr(win, "GetDeviceState() == kDeviceStateSpeaking");
        TEST_ASSERT_NOT_NULL_MESSAGE(
            edge_at,
            "RDV7-A3 RED: tts stop 分支无复位沿(0) —— 引导播报结束后子态悬挂帧 B");
        TEST_ASSERT_NOT_NULL_MESSAGE(guard_at, "RDV7-A3 RED(infra): tts stop 出口守卫锚漂移");
        TEST_ASSERT_TRUE_MESSAGE(
            edge_at < guard_at,
            "RDV7-A3 RED: 复位沿晚于 Speaking 出口守卫 —— 打断离场后 tts stop 到达时"
            "设备已非 Speaking, 复位沿被出口判据吞掉 = 子态悬挂");
    }
    free(win);
    free(app);

    /* ---- tooth 3: mcp_server.cc 桥定义（页态门 + 幂等 + 缓存同步）------------- */
    mcp = rdf_strip_comments(rdf_read_file("firmware/main/mcp_server.cc"));
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "RDV7-A3 RED(infra): mcp_server.cc unreadable");
    win = rdf_slice(mcp, "void KidpalStoryGuideSpeechEdge(int speaking)",
                    "mcp_json_get_string");
    TEST_ASSERT_NOT_NULL_MESSAGE(win, "RDV7-A3 RED(infra): 桥定义区切片失败");
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "KSTORY_PAGE_RECORDING") >= 1,
        "RDV7-A3 RED: 桥缺 RECORDING 页态门 —— 非录音态 TTS（欢迎语/整理安抚句）"
        "误置引导子态");
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "g_story_view.rec_substate == next") >= 1,
        "RDV7-A3 RED: 桥缺幂等门 —— 重复沿（tts start 重发）整页重渲抖动");
    TEST_ASSERT_TRUE_MESSAGE(
        rdf_count(win, "ScheduleStoryRender()") >= 1,
        "RDV7-A3 RED: 桥零渲染节点缓存同步 —— 键帽分叉消费面"
        "（kidpal_render_node_story_rec_substate）读不到新子态, 帧 B 键帽不切");
    free(win);
    free(mcp);
}
