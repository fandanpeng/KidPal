/*
 * reminder_page.test.c — KidPal 提醒弹窗消费面自带单元测试
 * (TaskPackage 260829-schedule-reminder, firmware/main/display/reminder_page.c)。
 *
 * 与冻结验收的分工：firmware/main/test/schedule_reminder_b.acceptance.c 是**规格**（不
 * 得偏移、本文件不重复它已钉的断言），本文件补实现自带的边界与错误路径：
 *   1) kidpal_fire_at_minutes 的日历合法性（闰年/2 月 30 日/13 月）、小数秒向下截断、
 *      负偏移与 Z 形式、't' 分隔符、缺冒号偏移、尾部垃圾；
 *   2) kidpal_alert_parse 的 \\uXXXX 与代理对还原、空白容忍、嵌套未知对象跳过、
 *      speech/view 非串拒绝、**UTF-8 码点安全截断**（中文大字不被截成乱码）；
 *   3) known-id 集满时**淘汰不得摘除驻留/排队项**（否则同 id 重推会二次弹窗）；
 *   4) 满队列 FIFO + V18 顶替的顺序正确性（8 项逐个弹）；
 *   5) kidpal_reminder_shown_alert 拷出面、按键不介入与 NULL 防御。
 *
 * 形态：与 seam_bounds.test.c 同族（host runner 内的自带单测，不是冻结件）。状态实例
 * 按冻结件同款纪律用定长裸缓冲承载（模块内 _Static_assert 保证 <= 4096 字节）。
 * 构建/运行协议同 HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]。
 */

#include <unity.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "../display/reminder_page.h"

#define KRM_UT_BASE 29815695LL   /* 2026-09-09T16:15:00+08:00（冻结参考点） */

static unsigned char store_raw[4096];

static kidpal_reminder_t *fresh(void)
{
    kidpal_reminder_t *s = (kidpal_reminder_t *)store_raw;

    memset(store_raw, 0x5A, sizeof(store_raw));
    kidpal_reminder_init(s);
    return s;
}

static void fill(kidpal_alert_t *a, const char *id, const char *fire_at, const char *speech)
{
    memset(a, 0, sizeof(*a));
    snprintf(a->id, sizeof(a->id), "%s", id);
    snprintf(a->fire_at, sizeof(a->fire_at), "%s", fire_at);
    snprintf(a->title, sizeof(a->title), "%s", id);
    if (speech != NULL) {
        snprintf(a->speech, sizeof(a->speech), "%s", speech);
    }
}

/* ------------------------------------------------------------------------- */

TEST_CASE("reminder unit: fire_at 日历合法性与偏移算术", "[reminder][unit][fire_at]")
{
    long long base = kidpal_fire_at_minutes("2026-09-09T16:15:00+08:00");

    TEST_ASSERT_EQUAL_INT64_MESSAGE(KRM_UT_BASE, base, "冻结参考点漂移（16:15+08）");
    /* 同一 UTC 瞬时不同写法必须相等（V18 顶替按绝对时刻，不按字面） */
    TEST_ASSERT_EQUAL_INT64(base, kidpal_fire_at_minutes("2026-09-09T08:15:00Z"));
    TEST_ASSERT_EQUAL_INT64(base, kidpal_fire_at_minutes("2026-09-09T08:15:00+00:00"));
    /* 负偏移：16:15-05:00 = 21:15Z = 比 +08 写法晚 13 小时 */
    TEST_ASSERT_EQUAL_INT64(base + 13 * 60, kidpal_fire_at_minutes("2026-09-09T16:15:00-05:00"));
    /* 小写 t 分隔符与小数秒（向下截断到整分钟）、秒位满 59 不改分钟 */
    TEST_ASSERT_EQUAL_INT64(base, kidpal_fire_at_minutes("2026-09-09t16:15:00.999+08:00"));
    TEST_ASSERT_EQUAL_INT64(base, kidpal_fire_at_minutes("2026-09-09T16:15:59+08:00"));
    /* 闰年规则：2028 闰 / 2026 平 / 2000 闰（%400）/ 1900 平（%100 但不 %400） */
    TEST_ASSERT_TRUE(kidpal_fire_at_minutes("2028-02-29T00:00:00Z") >= 0);
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-02-29T00:00:00Z"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-02-30T00:00:00Z"));
    TEST_ASSERT_TRUE(kidpal_fire_at_minutes("2000-02-29T00:00:00Z") >= 0);
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("1900-02-30T00:00:00Z"));
    /* 非法形状一律 -1（绝不当成 0 —— 0 会被算成 1970 年从而永远「事件已过」） */
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-13-01T00:00:00Z"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-00-01T00:00:00Z"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T24:00:00Z"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T12:60:00Z"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T12:00:00+08:60"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T12:00:00"));      /* 缺偏移 */
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T12:00:00+0800")); /* 偏移缺冒号 */
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes("2026-09-09T12:00:00Zjunk"));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes(""));
    TEST_ASSERT_EQUAL_INT64(-1, kidpal_fire_at_minutes(NULL));
    /* 跨分/跨日/跨年严格按绝对分钟递增 */
    TEST_ASSERT_TRUE(kidpal_fire_at_minutes("2026-09-09T16:15:00+08:00") <
                     kidpal_fire_at_minutes("2026-09-09T16:30:00+08:00"));
    TEST_ASSERT_TRUE(kidpal_fire_at_minutes("2026-09-09T23:59:00+08:00") <
                     kidpal_fire_at_minutes("2026-09-10T00:00:00+08:00"));
    TEST_ASSERT_TRUE(kidpal_fire_at_minutes("2026-12-31T23:59:00+08:00") <
                     kidpal_fire_at_minutes("2027-01-01T00:00:00+08:00"));
}

TEST_CASE("reminder unit: 载荷解析 unicode 还原、空白容忍、非串与截断边界", "[reminder][unit][parse]")
{
    kidpal_alert_t a;

    /* \uXXXX 基本平面 + 代理对（增补平面）还原成 UTF-8；键间空白与换行容忍 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_alert_parse(
        "{ \"id\" : \"rmd-u1\" ,\n  \"fire_at\":\"2026-09-09T07:45:00Z\" ,"
        "\"title\":\"\\u94a2\\u7434\\u8bfe\",\"speech\":\"\\ud83d\\udc0dan\" }", &a));
    TEST_ASSERT_EQUAL_STRING("\xE9\x92\xA2\xE7\x90\xB4\xE8\xAF\xBE", a.title);  /* 钢琴课 */
    /* U+1F40D -> F0 9F 90 8D，后接 ASCII（相邻字面量拆分，避免 \x 贪婪吞掉后续 hex 字母） */
    TEST_ASSERT_EQUAL_STRING_LEN("\xF0\x9F\x90\x8D" "an", a.speech, 5);
    TEST_ASSERT_EQUAL_STRING("", a.view_json);                       /* 缺 view -> "" */

    /* speech / view 存在但非串 = 脏载荷 -> 显式降级（不盲投递） */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse(
        "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"t\",\"speech\":7}", &a));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse(
        "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"t\","
        "\"view\":{\"view\":\"text\"}}", &a));
    /* 未知扩展字段（含嵌套对象/数组）合法跳过；缺 title -> -1 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_alert_parse(
        "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"t\","
        "\"ext\":{\"a\":[1,2,{\"b\":null}]},\"ok\":true}", &a));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse(
        "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\"}", &a));
    /* 孤立高代理、未闭合字符串、尾部垃圾 = 畸形 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse(
        "{\"id\":\"\\ud83d\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"t\"}", &a));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse("{\"id\":\"x\"}", &a));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_alert_parse(
        "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"t\"}junk", &a));

    /* UTF-8 码点安全截断：中文大字截在半个汉字上会在 RLCD 上糊成乱码 */
    {
        char payload[512];
        char *cursor;
        int i;

        /* 30 个「谱」(各 3 字节) = 90 字节 -> title[64] 只装得下 21 个 = 63 整字节 */
        strcpy(payload, "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"");
        cursor = payload + strlen(payload);
        for (i = 0; i < 30; i++) {
            strcpy(cursor, "\xE8\xB0\xB1");
            cursor += 3;
        }
        strcpy(cursor, "\"}");
        memset(&a, 0, sizeof(a));
        TEST_ASSERT_EQUAL_INT(0, kidpal_alert_parse(payload, &a));
        TEST_ASSERT_EQUAL_UINT(63, (unsigned)strlen(a.title));
        TEST_ASSERT_EQUAL_UINT(0, (unsigned)strlen(a.title) % 3u);

        /* 62 个 ASCII + 1 个汉字（共 65 字节）：截到 63 会落在汉字中间 -> 回退到 62 */
        strcpy(payload, "{\"id\":\"x\",\"fire_at\":\"2026-09-09T07:45:00Z\",\"title\":\"");
        cursor = payload + strlen(payload);
        for (i = 0; i < 62; i++) {
            cursor[i] = 'a';
        }
        strcpy(cursor + 62, "\xE8\xB0\xB1\"}");
        memset(&a, 0, sizeof(a));
        TEST_ASSERT_EQUAL_INT(0, kidpal_alert_parse(payload, &a));
        TEST_ASSERT_EQUAL_UINT(62, (unsigned)strlen(a.title));
    }
}

TEST_CASE("reminder unit: known-id 集满淘汰不得摘除驻留/排队项", "[reminder][unit][security]")
{
    kidpal_reminder_t *s = fresh();
    kidpal_alert_t a;
    char id[24];
    char shown[64];
    int i;
    int queue_before;

    /* 驻留项：known 集被后续灌入冲满 -> 淘汰必须绕开它，否则同 id 重推被当成新提醒
     * （二次弹窗根因）。 */
    fill(&a, "rmd-resident", "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_QUEUED, kidpal_reminder_receive(s, &a));
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_raise_next(s, KRM_UT_BASE));
    for (i = 0; i < KRM_KNOWN_CAP + 12; i++) {
        snprintf(id, sizeof(id), "rmd-flood-%d", i);
        fill(&a, id, "2026-09-09T16:15:00+08:00", "文案");
        (void)kidpal_reminder_receive(s, &a);
    }
    TEST_ASSERT_TRUE(kidpal_reminder_known_count(s) <= KRM_KNOWN_CAP);
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_id(s, shown, sizeof(shown)));
    TEST_ASSERT_EQUAL_STRING("rmd-resident", shown);
    fill(&a, "rmd-resident", "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_RECEIVED, kidpal_reminder_receive(s, &a)); /* 仍被认得 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_raise_next(s, KRM_UT_BASE));       /* 未二次弹窗 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_id(s, shown, sizeof(shown)));

    /* 排队项同理：集满淘汰时不得忘掉仍在队列里的 id（重推会重复入队）。 */
    s = fresh();
    fill(&a, "rmd-q1", "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_QUEUED, kidpal_reminder_receive(s, &a));
    for (i = 0; i < KRM_KNOWN_CAP; i++) {
        snprintf(id, sizeof(id), "rmd-pad-%d", i);
        fill(&a, id, "2026-09-09T16:15:00+08:00", "文案");
        (void)kidpal_reminder_receive(s, &a);
    }
    queue_before = kidpal_reminder_queue_count(s);
    fill(&a, "rmd-q1", "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_RECEIVED, kidpal_reminder_receive(s, &a)); /* 幂等屏蔽 */
    TEST_ASSERT_EQUAL_INT(queue_before, kidpal_reminder_queue_count(s));        /* 未重复入队 */
    /* 弹窗面第一次弹出时，最早到达且仍在队列里的 rmd-q1 必须在队头（FIFO 未被破坏） */
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_raise_next(s, KRM_UT_BASE));
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_id(s, shown, sizeof(shown)));
    TEST_ASSERT_EQUAL_STRING("rmd-q1", shown);
}

TEST_CASE("reminder unit: 满队列 FIFO 逐个弹 + V18 顶替顺序", "[reminder][unit][queue]")
{
    kidpal_reminder_t *s = fresh();
    kidpal_alert_t a;
    char id[24];
    char shown[64];
    int i;

    for (i = 0; i < KRM_QUEUE_CAP; i++) {
        snprintf(id, sizeof(id), "rmd-fifo-%d", i);
        fill(&a, id, "2026-09-09T16:15:00+08:00", "文案");
        TEST_ASSERT_EQUAL_INT(KRM_STATE_QUEUED, kidpal_reminder_receive(s, &a));
    }
    TEST_ASSERT_EQUAL_INT(KRM_QUEUE_CAP, kidpal_reminder_queue_count(s));
    snprintf(id, sizeof(id), "rmd-fifo-%d", KRM_QUEUE_CAP);
    fill(&a, id, "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_RECEIVED, kidpal_reminder_receive(s, &a)); /* 溢出屏蔽 */
    TEST_ASSERT_EQUAL_INT(KRM_QUEUE_CAP, kidpal_reminder_queue_count(s));

    /* 逐个弹：关掉当前才弹下一条，顺序 = 到达顺序；每条只在渲染完成后允许一次回执 */
    for (i = 0; i < KRM_QUEUE_CAP; i++) {
        page_id_t out = PAGE_P1;
        char expect[24];

        TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_raise_next(s, KRM_UT_BASE + i));
        TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_id(s, shown, sizeof(shown)));
        snprintf(expect, sizeof(expect), "rmd-fifo-%d", i);
        TEST_ASSERT_EQUAL_STRING(expect, shown);
        TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_mark_rendered(s, shown));
        TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_may_emit_receipt(s));
        TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_mark_acked(s, shown));
        TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_may_emit_receipt(s));
        TEST_ASSERT_EQUAL_INT(KRM_KEY_DISMISS,
                              kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK, PAGE_P2, &out));
        TEST_ASSERT_EQUAL_INT((int)PAGE_P2, (int)out);
        TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_shown_id(s, shown, sizeof(shown)));
    }
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_raise_next(s, KRM_UT_BASE + 999));
    TEST_ASSERT_EQUAL_INT(KRM_QUEUE_CAP + 1, kidpal_reminder_known_count(s));

    /* V18 顶替：事件已过 + 已渲染 + 未按键 -> 头顶替；known 计数不变（非追加送达） */
    s = fresh();
    fill(&a, "rmd-v1", "2026-09-09T16:15:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    fill(&a, "rmd-v2", "2026-09-09T19:00:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRM_UT_BASE);
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_mark_rendered(s, "rmd-v1"));
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_mark_acked(s, "rmd-v1"));
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_raise_next(s, KRM_UT_BASE - 1)); /* 事件未到不顶替 */
    TEST_ASSERT_EQUAL_INT(2, kidpal_reminder_raise_next(s, KRM_UT_BASE));
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_id(s, shown, sizeof(shown)));
    TEST_ASSERT_EQUAL_STRING("rmd-v2", shown);
    TEST_ASSERT_EQUAL_INT(2, kidpal_reminder_known_count(s));
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_may_emit_receipt(s));             /* 新弹未渲染不回执 */
}

TEST_CASE("reminder unit: shown_alert 拷出面、按键不介入与 NULL 防御", "[reminder][unit][gesture]")
{
    kidpal_reminder_t *s = fresh();
    kidpal_alert_t a;
    kidpal_alert_t out;
    page_id_t page = PAGE_P3;

    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_shown_alert(NULL, &out));
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_shown_alert(s, &out));     /* 无弹窗 */
    fill(&a, "rmd-copy", "2026-09-09T16:15:00+08:00", "记得带琴谱");
    snprintf(a.title, sizeof(a.title), "钢琴课 16:30");
    TEST_ASSERT_EQUAL_INT(KRM_STATE_QUEUED, kidpal_reminder_receive(s, &a));
    TEST_ASSERT_EQUAL_INT(0, kidpal_reminder_shown_alert(s, &out));     /* 排队未弹出 -> 无驻留 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_raise_next(s, KRM_UT_BASE));
    memset(&out, 0, sizeof(out));
    TEST_ASSERT_EQUAL_INT(1, kidpal_reminder_shown_alert(s, &out));
    TEST_ASSERT_EQUAL_STRING("rmd-copy", out.id);
    TEST_ASSERT_EQUAL_STRING("2026-09-09T16:15:00+08:00", out.fire_at);
    TEST_ASSERT_EQUAL_STRING("\xE9\x92\xA2\xE7\x90\xB4\xE8\xAF\xBE 16:30", out.title);
    /* 三件套动作集：弹窗 + 提示音恒在，有文案才要播报（语音在引擎腿，见裁定二） */
    TEST_ASSERT_EQUAL_INT(KRM_ACT_SHOW | KRM_ACT_TONE | KRM_ACT_SPEAK,
                          kidpal_reminder_render_actions(&out));
    out.speech[0] = '\0';
    TEST_ASSERT_EQUAL_INT(KRM_ACT_SHOW | KRM_ACT_TONE, kidpal_reminder_render_actions(&out));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_render_actions(NULL));

    /* 无弹窗时按键完全不介入；越界键位（模拟 PWR）PASS；NULL 参数全防御 */
    s = fresh();
    TEST_ASSERT_EQUAL_INT(KRM_KEY_PASS,
                          kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_CLICK, PAGE_P3, &page));
    TEST_ASSERT_EQUAL_INT((int)PAGE_P3, (int)page);
    page = PAGE_P1;
    TEST_ASSERT_EQUAL_INT(KRM_KEY_PASS,
                          kidpal_reminder_key(s, KEY_ID_COUNT, KEY_GESTURE_CLICK, PAGE_P1, &page));
    TEST_ASSERT_EQUAL_INT((int)PAGE_P1, (int)page);
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_key(NULL, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                  PAGE_P1, &page));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                  PAGE_P1, NULL));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_known_count(NULL));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_queue_count(NULL));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_raise_next(NULL, KRM_UT_BASE));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_shown_id(s, NULL, 8));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_mark_rendered(s, NULL));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_reminder_mark_acked(NULL, "x"));
    TEST_ASSERT_EQUAL_INT(KRM_STATE_INVALID, kidpal_reminder_receive(NULL, &a));
    kidpal_reminder_init(NULL); /* 不崩 */
}

/* --------------------------------------------------------------------------- *
 * REVISION LOG (builder unit tests; not a frozen acceptance file):
 *   2026-09-08  initial (builder, TaskPackage 260829-schedule-reminder).
 * --------------------------------------------------------------------------- */
