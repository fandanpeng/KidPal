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
#define HP1_PREVIEW_MAX 8                 /* 明天区 preview 缓存容量上限（超容整包拒收,
                                           * 与 HP1_ITEM_MAX 同纪律; CCP 261011-p1-home-
                                           * v5-rework 触点③ + requirements v1.61 AC2/AC6） */
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
    /* 日期（包 260915-m1-k1a-key-layer-dispatch，M1-N1b 日期 DD-Mon）：0 = 未知/缺
     * RTC 日期数据（渲染叠描边框兜底，不画假日期）。month 1..12、mday 1..31。 */
    int month;         /* 1..12 (0 = 未知) */
    int mday;          /* 1..31 (0 = 未知) */
    /* 星期（2026-09-26 真机对齐轮，canvas-p1 "Tue 22 Sep"）：周一=0..周日=6（与
     * kidpal_render_node_inject_weekday 同基），-1 = 未注入/越域（日期渲染退回
     * "DD Mon"，不画假星期）。数据源 = 板级 RTC 星期（inject_weekday 缝, M3-K2 同源）。 */
    int wday;          /* -1 = 未知; 0..6 = 周一..周日 */
    int env_valid;     /* 1 = 温湿度有效; 0 = 无数据 -> 温湿度区隐藏 */
    int temp_c;        /* -40..60 (env_valid=1 时有效) */
    int humidity_pct;  /* 0..100   (env_valid=1 时有效) */
    int battery_pct;   /* 0..100 */
    int charging;      /* 0 = 电池, 1 = 充电中 */
    int tick_ms;       /* 动画相位 tick, >= 0 */
} hp1_state_t;

/* ---- FROZEN OFFLINE CONTRACT 声明（签名以验收文件头注为准） ----
 *
 * 2026-10-11 seam 同形改（CCP 261011-p1-home-v5-rework 触点③ HUMAN_APPROVED +
 * requirements v1.61 S-F1.1 AC2/AC5/AC6/AC7）——**supersede 旧 4 参 inject
 * kidpal_home_p1_schedule_inject(day_key, items, next_day_preview) 与旧单项 preview
 * 访问族 kidpal_home_p1_schedule_preview(hp1_item_t*) 同批退役**（旧形与本形不共存）：
 *   - next_day_preview 单指针 -> next_day_items + next_day_count 数组+计数（明天区
 *     多项化：学校课合并「XX:XX 上学」与课外班、临时事件同列，按开始时间升序）；
 *   - preview 访问族改多项：preview_count() / preview_item(index)（单项访问族退役,
 *     防双访问族语义漂移）。
 * next_day_count 单值三态（与读侧 preview_count() 返回域同构, 无第四态可漂移）：
 *   >= 1  = preview 列表（next_day_items 非 NULL 且逐项 valid; > HP1_PREVIEW_MAX -> -1）
 *   == 0  = 云端确认明日无项（wire preview:[] 的 seam 形; next_day_items 必须 NULL）
 *   == -1 = 本推未提供 preview（wire preview:null/键缺省 的 seam 形; next_day_items
 *           必须 NULL）
 *   其它负值 -> -1 拒收。任一半区畸形 -> 整包拒 -1 且状态零变化（全量替换语义不变）。
 * 归档件 firmware/main/test/acceptance/archive/home_p1.acceptance.c 仍零触碰（archive
 * 纪律：移位即退役）; supersede 由本注记 + schedule_grid.acceptance.c REVISION LOG 承载。 */
int kidpal_home_p1_schedule_inject(const char *day_key,
                                   const hp1_item_t *items, int count,
                                   const hp1_item_t *next_day_items,
                                   int next_day_count);
int kidpal_home_p1_schedule_count(void);
const hp1_item_t *kidpal_home_p1_schedule_item(int index);
/* 明天区三态读侧（2026-10-11 preview 多项访问族）: >=1 = 列表项数; ==0 = 确认空
 * （AC6 话术态）; ==-1 = 未提供/从未注入/day_rollover 已清（AC7 数据未达态）。 */
int kidpal_home_p1_schedule_preview_count(void);
/* 0 <= index < preview_count() 时返回项指针（{time,name} verbatim），否则 NULL。 */
const hp1_item_t *kidpal_home_p1_schedule_preview_item(int index);
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