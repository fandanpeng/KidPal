/*
 * schedule_page.h — KidPal P2 一周总览网格页消费面 (TaskPackage
 * 260921-m3-k2-p2-week-grid-page, 契约 §1.3② schedule 周网格 data / S-F1.2 /
 * NFR-READ-1; 2026-09-21 契约门定稿)。
 *
 * 两个 FROZEN OFFLINE CONTRACT 函数 + 网格类型/常量的声明。语义权威 = 冻结验收
 * firmware/main/test/schedule_grid.acceptance.c 文件头 —— 本头仅供设备消费面
 * (mcp_server.cc 的 kidpal/display tool schedule 路由 + kidpal_render_node.cc 的
 * cycle_next P2 分支) 引用，不得偏移。(实现: firmware/main/display/schedule_page.c)
 *
 * 纯逻辑 + 纯 LVGL 渲染面（lv_obj_/lv_label only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。解析器为自足式最小 JSON 扫描
 * （无外部 JSON 库依赖，frozen contract 要求）。
 *
 * 降级语义：kidpal_schedule_grid_parse 对畸形/缺字段/越界 cap 一律返回 -1 ——
 * 调用方必须友好降级、绝不盲渲染 (V27 / spec §4.0)。旧列表形状 {title,items} 已随
 * 契约门 2026-09-21 废止（旧 260829-schedule-p2 五函数与列表版式整体退役，冻结验收
 * schedule_p2.acceptance.c 已归档，重写不走 supersede）。
 */
#ifndef KIDPAL_SCHEDULE_PAGE_H
#define KIDPAL_SCHEDULE_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 网格契约常量（== 冻结验收 schedule_grid.acceptance.c 文件头，不得偏移） ---- */
#define KSG_NAME_CAP   48  /* 课程名 UTF-8 字节容量（safe-truncate, 恒 NUL 结尾） */
#define KSG_FLAG_TEMP  0x1 /* bit0 ※ 临时安排（B1 一次性安排区块） */
#define KSG_FLAG_STAR  0x2 /* bit1 星标呼应（group_upcoming 当前「接下来」项） */
#define KSG_COURSE_CAP 84  /* 周网格缓存容量 = 7 天 × 3 格 × 每格 4 门（定容冻结值） */
#define KSG_GRID_BOTTOM 254 /* 网格可用区下缘（2026-09-26 真机对齐轮 251→254：设计 canvas-p2
                              * 实测网格底线 y254、底条上沿实线 y262-264 之间有 7px 固有间隙——旧
                              * 值 251 系贴旧底条顶 252 的适配位，底条改 262 后不再成立）；
                              * y>=255 = 网格外带（底条之上一段），网格严格零墨 */
#define KSG_FONT_MIN_PX 14 /* NFR-READ-1 格内课程名行 pitch 下限（1bpp CJK 实测） */

/* 契约 §1.3② schedule view 周网格课程条目（data.week.courses 元素）。 */
typedef struct {
    int  day;                /* 0..6, 0=周一..6=周日（契约 §1.3② day 0 基） */
    int  slot;               /* 0..2, 0=上午 1=下午 2=晚上 */
    char name[KSG_NAME_CAP]; /* 课程名 UTF-8 bytes（safe-truncated, 恒 NUL 结尾） */
    int  flags;              /* KSG_FLAG_TEMP(0x1 ※) | KSG_FLAG_STAR(0x2 星标), 0..3 */
} kidpal_schedule_course_t;

/* 解析 schedule view 的 data 对象 JSON 字符串（mcp kidpal/display handler 持有的
 * `data` 参数，非整个 {view,…} 信封）。形状 = {"week":{"courses":[…]}}；每条目键集
 * 精确 {day,slot,name,flags}（day/slot/flags 必须是 JSON 数字且在 0..6 / 0..2 / 0..3
 * 值域；name 必须非缺失非空字符串）。返回课程条数（>=0，{"week":{"courses":[]}} = 0
 * 空课表合法页）；-1 于：NULL / 畸形 JSON / 缺 "week" / courses 非数组 / 条目非对象 /
 * 缺 day·slot·name / 越界值 / flags 越域或非数字 / cap < 条数（绝不溢出/静默截断）。
 * 尾部垃圾 = 畸形 → -1。容忍（additive）：flags 缺省解析为 0；未知扩展字段忽略；
 * JSON 空白（wire json.dumps 默认 ": " / ", "）照常解析。name 按 KSG_NAME_CAP
 * safe-truncate。 -1 = 设备降级信号：调用方必须友好降级、绝不盲渲染（V27）。 */
int kidpal_schedule_grid_parse(const char *data_json,
                               kidpal_schedule_course_t *courses, int cap);

/* P2 一周总览网格渲染面（纯 LVGL 画在 ACTIVE screen）：先清屏（幂等可反复调用），
 * 画 7 列（周一..周日）× 3 行（上午/下午/晚上）整屏网格 + 行首槽位标签列，页内无
 * 标题无提示行（v1.20 已删，操作提示归全局键帽条）。版式区钉（== 冻结验收文件头）：
 * 网格可用区 y[0..KSG_GRID_BOTTOM]；y>=255（底条顶 262 之上外带）严格零墨；表头带 y[0..29]；
 * 槽位行带 slot0 y[30..103] / slot1 y[104..177] / slot2 y[178..251]；标签列
 * x[0..39]；day 列带 x[40+51*d .. 90+51*d]。(day,slot) 格内多课逐门竖排，课程名行
 * pitch >= KSG_FONT_MIN_PX；不做格内滚动，格内溢出走截断降级。
 * today_col ∈ 0..6 → 该 day 列带整列高亮、墨不越带；越域（含 -1 = 未注入星期）→
 * 无任何列高亮（不画假「今天」）。flags bit0/bit1 → ※/星标与课程名同格可见。
 * 返回 n（n=0 画空网格骨架）；非法参数（n<0、n>0 且 courses==NULL）-> -1；绝不崩溃。 */
int kidpal_schedule_grid_render(const kidpal_schedule_course_t *courses, int n,
                                int today_col);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_SCHEDULE_PAGE_H */
