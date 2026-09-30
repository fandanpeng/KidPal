/*
 * story_page.h — KidPal P3 星星记录本消费面：列表 / 录制归档设备面 / 阅读态 /
 * 成就盖章帧 / 删除确认子态 的声明头。
 *
 * **退役声明（notes 边角①）**：本头旧「FROZEN OFFLINE CONTRACT（TaskPackage
 * 260829-story-p3，语义与数值码以冻结验收 story_p3.acceptance.c 文件头为准）」
 * 已随该验收件归档退役（firmware/main/test/acceptance/archive/）。**现役冻结权威 =
 * firmware/main/test/story_page_v2.acceptance.c 文件头 seam contract [K1]..[K10]**
 * （TaskPackage 260924-m4-k2-p3-star-notebook-ui；v1.47 输入表矩阵重钉、动作码闭集
 * 重钉、删除确认子态/隐式 ack 重发/seq 消费新增）。本头形状与该文件内的类型镜像
 * 公共前缀逐字同序同型（kidpal_story_view_t 的旧易失盖章流水号语义位随退役改名为
 * stamp_serial、同槽同型；新增字段一律尾部追加——冻结纪律：只增不改/移）。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。C linkage。
 *
 * 降级语义：kidpal_story_list_parse / kidpal_story_text_parse 对畸形/缺字段/越界
 * cap 一律 -1 —— 调用方必须展示友好降级提示、绝不盲渲染 (V27，与
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

/* ---- ① story_list 载荷消费 (契约 §1.3③) ---- */
typedef struct {
    char id[64];      /* 契约 item.id (回看/删除确认的键), safe-truncated */
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

/* ---- ② story_text 载荷消费 (契约 §1.3④) ---- */
typedef struct {
    char id[64];
    char title[96];   /* 缺省时 "" */
    char text[2048];  /* 正文, safe-truncated (分页在渲染面, 不在解析处丢) */
    /* additive 尾部追加（seam [K6]; 冻结纪律: 只增不改/移）: 入册流水号 seq,
     * 唯一权威源 = 契约 §1.3④ data.seq（账本 owner = K1）。**解析器不写本字段**
     * （镜像公共前缀外写入会破坏验收镜像 ABI——消费方须经 kidpal_story_text_seq
     * 探针从原始 data JSON 取值后自行赋值）。缺省哨兵 KSTORY_SEQ_ABSENT。 */
    int seq;
} kidpal_story_text_t;

#define KSTORY_SEQ_ABSENT (-1) /* story_text 载荷未携带 seq / seq 畸形 */

/* ---- ③ P3 页视图态 + 页内输入特例 (seam [K1]) ---- */
typedef enum {
    KSTORY_PAGE_LIST      = 0,   /* 列表态 (S-F2.1) */
    KSTORY_PAGE_READER    = 1,   /* 阅读态 (S-F2.3) */
    KSTORY_PAGE_RECORDING = 2,   /* 录音态演出 (S-F2.2, manual 聆听) */
    KSTORY_PAGE_STAMP     = 3,   /* 盖章帧 (S-F2.4) */
    /* additive 追加位（seam [K1]; 冻结数值 = 4）: 删除确认子态（整屏覆盖层,
     * WF-F2-05; 层值 KIDPAL_LAYER_STORY_DELETE, seam [K10]）。 */
    KSTORY_PAGE_DELETE    = 4
} kidpal_story_page_t;

typedef struct {
    int page;            /* kidpal_story_page_t */
    int cursor;          /* LIST: 选中行 (0-based, 带边框不反白) */
    int count;           /* LIST: 行数 */
    int counter;         /* LIST: 右上角星星计数 ×N（恒信 story_list.total） */
    int screen;          /* READER/LIST: 当前屏 (0-based) */
    int screens;         /* READER/LIST: 总屏数 (>=1) */
    int rec_seconds;     /* RECORDING: 已录秒数 (大时长分子) */
    int stamp_serial;    /* STAMP: 「第 N 颗已入册」N = 入册流水号 seq（只增不减,
                          * 不被删除改写）; 缺省 KSTORY_SEQ_ABSENT = 降级演出不带 N。
                          * （旧易失本地流水号语义位退役——seam [K7] 负锚。） */
    int archive_pend;    /* 停录 -> 待归档(整段转写上传)请求计数 */
    int cancel_count;    /* V31 取消/丢弃次数 */
    int tts_requests;    /* 阅读态朗读请求计数（朗读 v1.52 已砍, 恒 0——退役保留位） */
    /* additive (冻结纪律: 只增不改/移): 盖章幂等去重用, 记录最近一次 saved_id。 */
    char last_saved_id[64];
    /* additive 尾部追加（seam [K1]/[K5]）: 待删条目 id 记账（确认删除后、story_list
     * 重推对账冲销前在账; 隐式 ack 重发与对账的锚）。空串 = 无待账。 */
    char pending_delete_id[64];
} kidpal_story_view_t;

/* ---- ④ 盖章帧几何 (spec S-F2.4) ---- */
typedef struct {
    int rings;         /* 双圈印章 -> 2 */
    int inner_dashed;  /* 外实内虚: 内圈虚线 -> 1 */
    int angle_deci;    /* 斜盖 -8° -> -80 */
    int hold_ms;       /* 定格 0.8s -> 800 */
} kidpal_story_stamp_spec_t;

/* ---- ⑤ 页内动作码闭集（seam [K1] 冻结数值; 旧 KSA_* 闭集随 archive 验收退役。
 * mcp_server.cc 本地镜像注释块按本枚举同步（单源升级路径已走通——旧注释自declared）。 */
typedef enum {
    KSPV2_ACT_DEGRADE        = -3, /* 畸形入参/越界键位（PWR 面）——零污染 */
    KSPV2_ACT_GLOBAL         = -2, /* 页不拥有该手势 -> 交回全局路由层 */
    KSPV2_ACT_NONE           = 0,  /* 页拥有但无副作用（不响应 = 消费不动） */
    KSPV2_ACT_OPEN_READER    = 2,  /* LIST  KEY 双击 -> 进阅读态 */
    KSPV2_ACT_CURSOR_NEXT    = 3,  /* LIST  BOOT 短按 -> 光标下移（屏感知+wrap） */
    KSPV2_ACT_SCREEN_JUMP    = 4,  /* LIST  BOOT 双击 -> 翻下一屏落该屏首项 */
    KSPV2_ACT_DELETE_ENTER   = 5,  /* LIST  BOOT 长按 -> 删除确认子态 */
    KSPV2_ACT_DELETE_CONFIRM = 6,  /* DELETE KEY 短按 -> 确认删除（本地即时移除
                                    * + 计数 -1 + 发送 kidpal_story_delete） */
    /* 数值 7 双语义位（页态消歧）: DELETE 态 = BOOT 任一手势/10s idle 取消回列表;
     * LIST 态 = KEY 短按 -> 记录语境会话（K3 会话动作接线消费位）。 */
    KSPV2_ACT_DELETE_CANCEL  = 7,
    KSPV2_ACT_REC_SESSION    = 7,
    KSPV2_ACT_PAGE_TURN      = 8,  /* READER BOOT 短按（双击同短按）-> 翻屏 wrap */
    KSPV2_ACT_READER_EXIT    = 9,  /* READER KEY 长按 -> 退出阅读回列表 */
    KSPV2_ACT_STOP_RECORD    = 10, /* RECORDING KEY 短按 & 长按同义（保存, v1.47） */
    KSPV2_ACT_DISCARD_RECORD = 11  /* RECORDING BOOT 任一手势（丢弃退回列表） */
} kidpal_story_act_t;

/* ---- ⑥ 空态文案单源（notes 边角③: 列表态空态渲染入口两处——story_page.c 与
 * kidpal_render_node.cc——必须同源一致, 不得双写文案）。逐字内容归 PNG/design
 * review, 冻结齿不锁文案只锁单源。 */
#define KSTORY_EMPTY_HINT "还没有星星记录，说「我要讲故事」开录吧"

/* ---- ⑦ 列表态分屏行数单源（400x300 版面行流区约 4 行/屏, 与阅读态分屏同心智;
 * cursor_step/screen_next 及渲染窗口共用本单源, 不得第二处魔数）。 */
#define KSTORY_LIST_ROWS_PER_SCREEN 4

/* 阅读态分页行高单源（seam [K1]/SPV2-B2 tooth 9 锁单源在世, 不锁数值）: 400x300 版面、
 * 正文排版按 NFR-READ-1（13.5px 起、行高 1.72 -> ~23px/视觉行, content 区 ~200px 可容
 * ~8 视觉行 @ 每自然行可折 ~2 视觉行）-> 4 自然行/屏, 与既有设备分页行为一致、
 * 本次实测重冻维持 4。切片按 '\n' 自然行计（open_reader 同性质）。
 * >12 屏显示位截断已退役（notes 边角⑧: 真实分栏、长文全篇可达——分屏数与切片不封顶）。 */
#define KSTORY_READER_LINES_PER_SCREEN 4

/* ---- ⑧ 常驻语音入口双指令（S-F2.1 AC3 + intro 对账③）: 「我要讲故事」+「帮我记
 * 下来」双触发词逐字住 story_page.c 渲染面（冻结齿 SPV2-B2 tooth 3 扫描 .c 文件
 * 字面, 不经宏转发——防间接层吃掉扫描锚）。 */

/* ---- 解析与计数 (契约 §1.3③) ---- */
int kidpal_story_list_parse(const char *data_json, kidpal_story_list_t *out);
int kidpal_story_counter(const kidpal_story_list_t *list);
int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out);

/* ---- 视图与输入 (seam [K1]) ---- */
int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list);
int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g);
int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);

/* ---- 录音态演出 (spec S-F2.2) ---- */
int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms, int *flash,
                          int *wave);
int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out, size_t cap);

/* ---- 阅读态 (S-F2.3 / seam [K8] 同心智) ---- */
int kidpal_story_open_reader(kidpal_story_view_t *v, const kidpal_story_text_t *st,
                             int screen_lines);
int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out, size_t cap);
const char *kidpal_story_reader_slice(const kidpal_story_text_t *st, int screen,
                                      int lines_per_screen, int *out_len);

/* ---- 盖章帧 (S-F2.4 / seam [K7]: N = 入册流水号 seq 唯一权威源) ---- */
int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out);
/* 三参改形（旧两参 on_saved 退役, seam [K7]）: stamp N = seq（KSTORY_SEQ_ABSENT =
 * 降级演出不带 N）; counter 零触碰（现存篇数权威恒 = story_list.total）; 同
 * saved_id 幂等。 */
int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id, int seq);
int kidpal_story_stamp_serial(const kidpal_story_view_t *v);
/* 文案逐字二值（seam [K7]）: serial >= 0 -> 「星星徽章·第 %d 颗已入册！」;
 * serial < 0 -> 「星星徽章已入册！」; cap 不足 / NULL -> -1（拒绝, 绝不截断/溢出）。 */
int kidpal_story_stamp_text(int serial, char *out, size_t cap);

/* ---- 渲染面 ---- */
int kidpal_story_page_render(const kidpal_story_view_t *v,
                             const kidpal_story_list_t *list,
                             const kidpal_story_text_t *story);

/* ---- ⑨ LIST 分屏纯逻辑三缝（seam [K8]; 原语形态, 行/屏数显式注入） ---- */
/* BOOT 短按光标下移: cursor+1; 越过当前屏底 -> 翻下一屏并落该屏首项; 到底 -> wrap
 * 回首项（永不离页）。count<=0 -> 0（零副作用, out 不触碰）; rows<=0 -> 按 1 行/屏
 * 防御; NULL out -> -1; 其余 -> 1。 */
int kidpal_story_list_cursor_step(int cursor, int count, int rows, int screen,
                                  int *out_cursor, int *out_screen);
/* BOOT 双击跳屏: screen=(screen+1)%screens, cursor 落该屏首项。screens<=0 / NULL
 * out -> -1; 其余 -> 1。 */
int kidpal_story_list_screen_next(int screen, int screens, int rows, int count,
                                  int *out_screen, int *out_cursor);
/* 分屏指示逐字（v1.28, 与阅读态同心智）: 「第 %d 屏 · 共 %d 屏」（1-based）;
 * screen<0 / screens<1 / cap 不足 / NULL -> -1。 */
int kidpal_story_list_screen_label(int screen, int screens, char *out, size_t cap);

/* ---- ⑩ 300ms 单双击消歧纯逻辑三缝（seam [K2]; 时间戳显式注入, 无真实 timer;
 * 原语形态零共享态——状态归调用方/mcp 消费侧）。冻结数值 KSPV2_DISAMBIG_MS = 300
 * （严格短于板端 iot_button 双击窗 350ms——消歧窗是双击窗的子集, 防重复触发）。 */
#define KSPV2_DISAMBIG_MS 300u
/* 状态门控（notes 边角②）: 仅 page == KSTORY_PAGE_LIST 返回 1（挂起消歧窗）;
 * 其余页态返回 0（单击零延迟直发）。 */
int kidpal_story_debounce_gate(int page);
/* 挂起单击到期判: now - arm_ts >= KSPV2_DISAMBIG_MS -> 1（直发单击动作）。 */
int kidpal_story_debounce_expired(uint32_t arm_ts_ms, uint32_t now_ms);
/* 双击到达冲销判: 0 <= now - arm_ts < KSPV2_DISAMBIG_MS -> 1（冲销挂起单击、执行
 * 双击动作）; 到点后到达的迟到双击 -> 0（单击已直发）。 */
int kidpal_story_debounce_cancel(uint32_t arm_ts_ms, uint32_t now_ms);

/* ---- ⑪ 删除确认子态 10s idle-step（seam [K3]; char_window
 * kidpal_char_window_idle_step 同款签名/语义缩参, 时长 10s; **静默**取消——不显示
 * 倒计时）。累计 idle 并判到点: >= KSPV2_DELETE_IDLE_MS 返回 1（调用方静默取消回
 * 列表并复位）; 未到点 0; NULL -> -1。任意手势重计 = 输入函数 DELETE 态消费侧义务
 * （mcp 消费侧每次手势驱动即复位累计）。 */
#define KSPV2_DELETE_IDLE_MS 10000UL
int kidpal_story_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms);

/* ---- ⑫ 隐式 ack 重发决策（seam [K4]; 契约 §四 Q5）: tries < 上限 且 age >= 间隔
 * -> 1（调用方重发同一 id 并 tries++）; 否则 0（放弃后静默停发）。 */
#define KSPV2_DELETE_RESEND_MS  5000UL
#define KSPV2_DELETE_RESEND_MAX 3
int kidpal_story_delete_resend_due(int tries, uint32_t age_ms);

/* ---- ⑬ story_list 重推对账冲销（seam [K5]） ---- */
/* 待删 id 只读探针: 无 pending -> 空串; 有 -> NUL 结尾 id 串指针（v == NULL ->
 * NULL）。 */
const char *kidpal_story_pending_delete_id(const kidpal_story_view_t *v);
/* 重推到达判: pending_id 非空且命中 items 窗口 -> 1（仍存在, 不冲销）; 不在窗口
 * （含空窗口）/ pending 为空串 -> 0（调用方冲销 pending 并以 list->total 刷新计数
 * ——counter 恒信 total）; NULL 入参 -> -1。 */
int kidpal_story_delete_reconcile_hit(const char *pending_id,
                                      const kidpal_story_list_t *list);

/* ---- ⑭ seq 只读探针（seam [K6]）: story_text data 的 seq 值: 缺省 / 畸形（非整数
 * / 字符串）-> KSTORY_SEQ_ABSENT 同径降级（seq 是 additive 装饰字段, 不为它整包拒
 * ——正文照常入阅读态）; 非负整数 -> 值。 */
int kidpal_story_text_seq(const char *data_json);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_STORY_PAGE_H */
