/*
 * story_page.h — KidPal P3 故事页消费面：故事列表 / 录制归档设备面 / 回看回听阅读 /
 * 成就盖章帧 的 FROZEN OFFLINE CONTRACT 声明 (TaskPackage 260829-story-p3,
 * 契约 §1.3 ③/④ / §1.4 / spec S-F2.1..S-F2.4)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/story_p3.acceptance.c 文件头
 * FROZEN OFFLINE CONTRACT 为准，不得偏移；本头与该文件 fallback 块逐字一致
 * （同 struct 字段名、同签名；只 additive 增补 last_saved_id 字段用于盖章幂等，
 * 未改名/移除/复用任何冻结字段）。实现见 story_page.c。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label/lv_anim only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。C linkage。
 *
 * 降级语义：kidpal_story_list_parse / kidpal_story_text_parse 对畸形/缺字段/越界
 * cap 一律 -1 —— 调用方必须展示友好降级提示、绝不盲渲染 (V27 / spec §4.0，与
 * kidpal_schedule_parse 同纪律)。
 */
#ifndef KIDPAL_STORY_PAGE_H
#define KIDPAL_STORY_PAGE_H

#include <stddef.h>
#include <stdint.h>

/* 相对本头目录解析（firmware/main/display/ -> firmware/main/），与冻结验收
 * 文件的 "../key_gesture_parser.h" 同款模式；kidpal_story_page_input 需要
 * key_id_t / key_gesture_t，host_app linux target 与设备树两侧 include 均可达。 */
#include "../key_gesture_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- ① story_list 载荷消费 (P3-A2, 契约 §1.3 ③) ---- */
typedef struct {
    char id[64];      /* 契约 item.id (回看/回听取正文的键), safe-truncated */
    char title[96];   /* 契约 item.title (列表行标题), safe-truncated */
    char date[16];    /* 契约 item.date "MM-DD"; 载荷缺省该项时为 "" */
} kidpal_story_item_t;

#define KSTORY_LIST_MAX     16   /* 单页 items 上限 (超出 = -1 降级, 不静默截断) */
#define KSTORY_TOTAL_ABSENT (-1) /* story_list 载荷未携带 total 字段 */

typedef struct {
    int total;                        /* 契约 total (>=0) 或 KSTORY_TOTAL_ABSENT */
    int count;                        /* items 行数 (0..KSTORY_LIST_MAX) */
    kidpal_story_item_t items[KSTORY_LIST_MAX];
} kidpal_story_list_t;

/* ---- ② story_text 载荷消费 (P3-A5, 契约 §1.3 ④) ---- */
typedef struct {
    char id[64];
    char title[96];   /* 缺省时 ""; 契约无「生字标注」字段 (P3-E4 已裁不做反白高亮) */
    char text[2048];  /* 正文, safe-truncated (分页在渲染面, 不在解析处丢) */
} kidpal_story_text_t;

/* ---- ③ P3 页视图态 + 页内输入特例 (P3-A3/A4/A5) ---- */
typedef enum {
    KSTORY_PAGE_LIST      = 0,   /* 列表态 (S-F2.1) */
    KSTORY_PAGE_READER    = 1,   /* 阅读态 (S-F2.3) */
    KSTORY_PAGE_RECORDING = 2,   /* 录音态演出 (S-F2.2, manual 聆听) */
    KSTORY_PAGE_STAMP     = 3    /* 盖章帧 (S-F2.4) */
} kidpal_story_page_t;

typedef struct {
    int page;            /* kidpal_story_page_t */
    int cursor;          /* LIST: 反白选中行 (0-based) */
    int count;           /* LIST: 行数 */
    int counter;         /* LIST: 右上角故事果计数 ×N */
    int screen;          /* READER: 当前屏 (0-based) */
    int screens;         /* READER: 总屏数 (>=1) */
    int rec_seconds;     /* RECORDING: 已录秒数 (大时长分子) */
    int stamp_index;     /* STAMP: 「第 N 颗」 */
    int archive_pend;    /* 停录 -> 待归档(整段转写上传)请求计数 */
    int cancel_count;    /* V31 取消/丢弃次数 */
    int tts_requests;    /* 阅读态 KEY「念给我听」朗读请求计数 */
    /* additive (冻结纪律: 只增不改/移): 盖章 N 幂等去重用, 记录最近一次 saved_id。 */
    char last_saved_id[64];
} kidpal_story_view_t;

/* ---- ④ 盖章帧几何 (P3-A6, spec S-F2.4) ---- */
typedef struct {
    int rings;         /* 双圈印章 -> 2 */
    int inner_dashed;  /* 外实内虚: 内圈虚线 -> 1 */
    int angle_deci;    /* 斜盖 -8° -> -80 */
    int hold_ms;       /* 定格 0.8s -> 800 */
} kidpal_story_stamp_spec_t;

int kidpal_story_list_parse(const char *data_json, kidpal_story_list_t *out);
int kidpal_story_counter(const kidpal_story_list_t *list);
int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out);
int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list);
int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g);
int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);
int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms, int *flash,
                          int *wave);
int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_open_reader(kidpal_story_view_t *v, const kidpal_story_text_t *st,
                             int screen_lines);
int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out);
int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id);
int kidpal_story_stamp_text(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_page_render(const kidpal_story_view_t *v,
                             const kidpal_story_list_t *list,
                             const kidpal_story_text_t *story);

/* ---- ⑤ 渲染切片 (RP-A2/F5, TaskPackage 260911-render-pages, additive-only 增补) ----
 * 阅读态 (READER) 渲染切片单源: 返回指向 st->text 内部 (指针/偏移, 不复制) 的第
 * screen 屏正文切片, 长度经 out_len 回传 (可为 NULL)。实现禁止大栈中转缓冲
 * (ssl_receive 4096B 栈纪律, story_page_stack 守卫同源)。越界屏/NULL story/
 * lines_per_screen<=0 -> NULL (防御, 绝不 OOB)。 */

/* 分页行高单源 (替换 mcp_server.cc open_reader 硬编码 4 的「最小估算」): 400x300
 * 版面、正文 14px 行高、content 区约 200px 高 @ 每自然行可折 ~2 视觉行 -> ~4
 * 自然行/屏, 与既有设备分页行为一致。切片按 '\n' 自然行计 (open_reader 同性质)。 */
#define KSTORY_READER_LINES_PER_SCREEN 4

const char *kidpal_story_reader_slice(const kidpal_story_text_t *st, int screen,
                                      int lines_per_screen, int *out_len);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_STORY_PAGE_H */
