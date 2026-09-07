/*
 * home_p1.h — KidPal 首页常显信息牌: FROZEN OFFLINE CONTRACT 声明
 * (TaskPackage 260829-home-p1).
 *
 * 数值码、常量、类型形状与语义均以冻结验收
 * firmware/main/test/home_p1.acceptance.c 文件头为准 —— 本头仅供设备消费面
 * (未来 producer/包 6 接线) 与后继包引用，不得偏移。(实现: firmware/main/display/home_p1.c)
 *
 * C linkage（供 .c 冻结验收与被 .cc 消费，统一 extern "C" —— view_router 同款）。
 * 纯逻辑、零芯片依赖：时间/温湿度/电量一律经 hp1_state_t / inject seam 注入，
 * host 可编 + 可编入设备树。
 */
#ifndef KIDPAL_HOME_P1_H
#define KIDPAL_HOME_P1_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 冻结数值码 / 常量（== 冻结验收 home_p1.acceptance.c，不得偏移） ---- */
#define HP1_ITEM_MAX 8                    /* 缓存容量上限（today 分组项） */
#define HP1_TIME_LEN 6                    /* "H:MM"=4 / "HH:MM"=5 chars + NUL */
#define HP1_NAME_LEN 32                   /* 31 chars + NUL */

#define HP1_UNIMPLEMENTED (-2)            /* weak 哨兵 — 永不是合法返回值 */
#define HP1_DOZE_BATTERY_THRESHOLD_PCT 15 /* 打盹电量阈值（<= 触发; 9b owns 调定） */
#define HP1_BLINK_PERIOD_COLON_MS   4400  /* 冒号眨眼周期 */
#define HP1_BLINK_PERIOD_BATTERY_MS 1000  /* 打盹电池闪周期 */
#define HP1_BLINK_ON_NUM 3
#define HP1_BLINK_ON_DEN 4                /* 可见窗 = period*3/4 */

#define HP1_ELEM_BIT_TIME 1u
#define HP1_ELEM_BIT_NEXT 2u
#define HP1_ELEM_BIT_TH   4u

/* ---- 类型形状（冻结，须与验收文件完全一致） ---- */
/* time = "H:MM" 或 "HH:MM"（producer 分组结果 verbatim）；name = 非空项名。 */
typedef struct {
    char time[HP1_TIME_LEN];
    char name[HP1_NAME_LEN];
} hp1_item_t;

typedef struct {
    int hour;          /* 0..23 */
    int minute;        /* 0..59 */
    int env_valid;     /* 1 = 温湿度有效; 0 = 无数据 -> 温湿度区隐藏 */
    int temp_c;        /* -40..60 (env_valid=1 时有效) */
    int humidity_pct;  /* 0..100   (env_valid=1 时有效) */
    int battery_pct;   /* 0..100 */
    int charging;      /* 0 = 电池, 1 = 充电中 */
    int tick_ms;       /* 动画相位 tick, >= 0 */
} hp1_state_t;

/* ---- FROZEN OFFLINE CONTRACT 声明（签名以验收文件头注为准） ---- */
int kidpal_home_p1_schedule_inject(const char *day_key,
                                   const hp1_item_t *items, int count,
                                   const hp1_item_t *next_day_preview);
int kidpal_home_p1_schedule_count(void);
const hp1_item_t *kidpal_home_p1_schedule_item(int index);
int kidpal_home_p1_schedule_preview(hp1_item_t *out);
int kidpal_home_p1_day_rollover(const char *now_day);
int kidpal_home_p1_countdown(const hp1_item_t *items, int count, int now_h,
                             int now_m, int *out_index, int *out_hours,
                             int *out_minutes);
int kidpal_home_p1_blink_visible(int tick_ms, int period_ms);
int kidpal_home_p1_doze_state(int battery_pct, int charging);
int kidpal_home_p1_layout_resolve(int time_h, int next_h, int th_h, int avail_h);
int kidpal_home_p1_set_state(const hp1_state_t *st);
int kidpal_home_p1_render_frame(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_HOME_P1_H */