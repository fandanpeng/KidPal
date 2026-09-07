/*
 * Frozen acceptance tests — TaskPackage 260829-home-p1
 * (subsystem: firmware; 首页常显信息牌; mergeOrder 8; rigor=lite; riskTier=medium;
 *  testHost: host+SDL; dependsOn: 260829-host-test-infra + 260829-voice-roundtrip).
 *
 * Story: 首页常显信息牌 = RTC 时间 + SHTC3 温湿度 + 今日日程下发缓存 + 低电打盹。
 * 孩子瞟一眼就知道现在几点、待会干嘛、屋里冷暖 (spec §4.1 S-F1.1 / §5.1 / §5.5)。
 *
 * Nature: host-runnable structural + BEHAVIORAL frozen acceptance, following the
 * voice_roundtrip_b.acceptance.c (VRT) weak-symbol frozen-offline-contract
 * precedent: builder MUST implement the pure-logic module below (numeric codes +
 * semantics frozen HERE), so the red baseline is RED on real missing behaviour,
 * never on a build/link error.
 *
 * ===========================================================================
 * PLANNER INTAKE 设计口径（冻结进本头注；builder 只接受签名，语义以本文件为准）
 * ===========================================================================
 * - 消费面形状 (P1-A3 / P1-E3)：设备侧新建本地日程缓存纯逻辑模块，建议落
 *   firmware/main/display/home_p1.c/h（C linkage、零芯片依赖、host 可编，
 *   view_router 同款）。缓存数据模型 = producer（包 6 OpenClaw）算好的分组结果
 *   逐项 {time:"H:MM"/"HH:MM", name} 数组 + 可选次日预告项；设备不重算分组、
 *   不合并学校课（「上学」合并是 producer 语义，设备渲染分组结果 verbatim）。
 *   缓存提供 inject/update seam（测试与未来 producer 接线共用同一入口）+ 读取 API。
 *   不实现任何下发链路/WS/mcp 协议代码（P1-E3 红线：只消费包 2 display 路由
 *   底座，通道接线归包 6）。
 * - 倒计时贴纸 (P1-A3)：给定分组 items + 当前时刻，选最近未开始项、算剩余
 *   时/分（「还有 3 个半小时」的数值半）；全部已开始/过期 -> 无贴纸。
 * - 低电打盹 (P1-A4)：电量 <= 阈值 -> state-sleepy（pose-sleep + 电池图形每秒
 *   闪）；充电中 -> 唤醒覆盖。阈值 = 具名常量 HP1_DOZE_BATTERY_THRESHOLD_PCT
 *   （本文件钉默认 15；**9b 真机续航验收 owns 最终阈值调定**——机制断言只测
 *   相对阈值行为，不测魔法数本身）。
 * - 眨眼冒号 (P1-A2)：给定 tick 时刻返回冒号可见相位，周期常量 4.4s（「眨眼
 *   双点」）；打盹态电池闪 = 1s 周期同款相位函数。可见窗钉 3/4 周期
 *   （HP1_BLINK_ON_NUM/DEN）——冒号每 4.4s 眨一下（灭 1.1s），电池每 1s 闪
 *   一下（灭 0.25s）。
 * - 跨天滚动 (v0.4 刷新三触发的设备半)：跨 0:00 把今日缓存日状态滚动（清空
 *   items + 次日预告并更新日键）。「连接建立推送」「新建日程即时推」两触发的
 *   产生端在 producer/网关侧，设备半只是被动收——不进本包断言面，头注留根
 *   （producer 接线归包 6，本包只钉 inject seam 与跨天滚动设备半）。
 * - 布局优先级 (P1-A5 / spec §5.1)：给定元素在位/尺寸冲突输入，贪心按解析序
 *   时间 > 接下来 > 温湿度；冲突时温湿度让位 = **隐藏**（不是缩略——钉死为
 *   隐藏语义，builder 照实现；温湿度数据缺席 env_valid=0 同样隐藏）。
 * - 版面渲染 (P1-A2/A3/A5 的 host+SDL 半)：经 host_infra 渲染缝
 *   （host_infra_read_pixel 像素探针，LVGL 软离屏渲染）测布局/逻辑——假数据
 *   注入 + 区域墨水探针断言大时间/日期/温湿度/电量/接下来卡/贴纸/预告在位；
 *   不测字体美学、不测真机（Baloo 数字/反白牌样式等纯审美留 design review）。
 *   区域矩形是布局在位钉（builder 须把对应元素画进对应区域），不是字体/颜色钉。
 * - RTC 走时 + SHTC3 温湿度直读是硬件强绑定子能力 (P1-E2)：host 段只测布局/
 *   逻辑（时间/温湿度经 hp1_state_t 注入），真实硬件读取显式殿后包 9b device
 *   验收 (PROPOSAL §3 留根)。
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure logic, host-compilable,
 * no chip/FreeRTOS/ESP-component deps; C linkage — extern "C" if in a .cc;
 * recommended home: firmware/main/display/home_p1.c + home_p1.h; the struct
 * shapes below MUST match home_p1.h exactly):
 *
 *   --- 类型形状（冻结） ---
 *   typedef struct { char time[6]; char name[32]; } hp1_item_t;
 *       time = "H:MM" 或 "HH:MM"（producer 分组结果 verbatim，含「8:00 上学」
 *       形态），有效字符集 = 数字 + ':'，小时 <= 23、分钟 <= 59；
 *       name = 非空课程/事项名（<= 31 字符 + NUL）。
 *   typedef struct {
 *       int hour;         // 0..23
 *       int minute;       // 0..59
 *       int env_valid;    // 1 = 温湿度有效; 0 = 无数据 -> 温湿度区隐藏
 *       int temp_c;       // -40..60  (env_valid=1 时有效)
 *       int humidity_pct; // 0..100   (env_valid=1 时有效)
 *       int battery_pct;  // 0..100
 *       int charging;     // 0 = 电池, 1 = 充电中
 *       int tick_ms;      // 动画相位 tick, >= 0 (眨眼冒号 / 打盹电池闪)
 *   } hp1_state_t;
 *
 *   --- 常量（冻结数值码） ---
 *   HP1_ITEM_MAX 8                        // 缓存容量上限（today 分组项）
 *   HP1_UNIMPLEMENTED (-2)                // weak 哨兵 — 永不是合法返回值
 *   HP1_DOZE_BATTERY_THRESHOLD_PCT 15     // 打盹电量阈值（<= 触发; 9b owns 调定）
 *   HP1_BLINK_PERIOD_COLON_MS   4400      // 冒号眨眼周期
 *   HP1_BLINK_PERIOD_BATTERY_MS 1000      // 打盹电池闪周期
 *   HP1_BLINK_ON_NUM 3 / HP1_BLINK_ON_DEN 4   // 可见窗 = period*3/4
 *
 *   --- 函数契约（全部返回 int，除注明外） ---
 *   int kidpal_home_p1_schedule_inject(const char *day_key,
 *                                      const hp1_item_t *items, int count,
 *                                      const hp1_item_t *next_day_preview);
 *       整体替换式写入今日分组日程缓存（replace 不 append）+ 存 day_key。
 *       day_key 非空且 <= 15 字符；count 0..HP1_ITEM_MAX；count==0 且 items
 *       可为 NULL = 「今日无待办」推送，清空缓存；count>0 时 items 必非 NULL
 *       且逐项过 time/name 校验；next_day_preview 可 NULL = 无预告，非 NULL
 *       时同过校验。任一无效 -> 返回 -1 且缓存保持原样（拒绝整包，不半写）。
 *       成功返回 0。count > HP1_ITEM_MAX -> -1。
 *
 *   int kidpal_home_p1_schedule_count(void);
 *       当前缓存项数，>= 0。（weak 哨兵 -2 = 未实现，永非合法值。）
 *   const hp1_item_t *kidpal_home_p1_schedule_item(int index);
 *       0..count-1 返回缓存项指针（verbatim 原样）；越界/NULL 入参语义 ->
 *       NULL。前提：count 已非哨兵。
 *   int kidpal_home_p1_schedule_preview(hp1_item_t *out);
 *       次日预告存在 -> 拷入 *out 并返回 0；无预告或 out 为 NULL -> 返回 -1。
 *
 *   int kidpal_home_p1_day_rollover(const char *now_day);
 *       跨 0:00 日滚动（设备半）：缓存日键 != now_day -> 清空 items + 预告、
 *       更新日键、返回 1；相同 -> 返回 0（缓存原样）；now_day NULL/空 -> -1
 *       不崩。（从未 inject 过 = 无日键 -> 返回 0。）
 *
 *   int kidpal_home_p1_countdown(const hp1_item_t *items, int count,
 *                                int now_h, int now_m,
 *                                int *out_index, int *out_hours, int *out_minutes);
 *       倒计时贴纸纯函数：在 items（可乱序）中选开始时刻 > 当前时刻的最近项，
 *       算剩余时/分（「还有 3 个半小时」= 210 分钟 -> 3 时 30 分）。
 *       返回 0 = 有贴纸（out_* 填写：选中项下标 + 剩余 h/m）；
 *       返回 1 = 无贴纸（count==0 或全部已开始/过期；开始时刻 == 当前算已开始）；
 *       返回 -1 = 无效入参（count>0 且 items NULL、任一 out NULL、count<0、
 *       now_h 不在 0..23、now_m 不在 0..59）。绝不除零/越界崩溃。
 *
 *   int kidpal_home_p1_blink_visible(int tick_ms, int period_ms);
 *       相位纯函数（眨眼冒号 + 打盹电池闪共用）：(tick_ms mod period_ms) <
 *       period_ms*HP1_BLINK_ON_NUM/HP1_BLINK_ON_DEN -> 1（可见），否则 0。
 *       防御：period_ms <= 0 或 tick_ms < 0 -> 返回 1（可见，永不崩）。
 *       每周期恰有一个灭窗（period*1/4）。
 *
 *   int kidpal_home_p1_doze_state(int battery_pct, int charging);
 *       低电打盹态：battery_pct <= HP1_DOZE_BATTERY_THRESHOLD_PCT 且
 *       charging==0 -> 1（state-sleepy: pose-sleep + 电池每秒闪）；
 *       charging==1 -> 恒 0（充电即叫醒，覆盖任何电量）；否则 0。
 *       battery_pct 不在 0..100 或 charging 不在 {0,1} -> -1。
 *
 *   int kidpal_home_p1_layout_resolve(int time_h, int next_h, int th_h, int avail_h);
 *       布局优先级解析纯函数 (P1-A5 / spec §5.1: 时间 > 接下来 > 温湿度)。
 *       输入三元素请求高度 + 可用高度；贪心按优先级序放置：元素保留当且仅当
 *       已用 + 请求高 <= avail_h。返回位集：
 *         HP1_ELEM_BIT_TIME(1) | HP1_ELEM_BIT_NEXT(2) | HP1_ELEM_BIT_TH(4)
 *       冲突语义（钉死）：装不下时**低优先级先让位**——温湿度位被清、时间/
 *       接下来位保住；avail 装不下更高优先级元素时更低者一并让位。
 *       防御：avail_h < 0 -> 0；请求高 < 0 的元素按不在位处理（不崩）。
 *
 *   int kidpal_home_p1_set_state(const hp1_state_t *st);
 *       渲染状态注入缝（测试与设备侧状态馈入共用；host 段时间/温湿度/电量/
 *       充电/tick 全部注入——P1-E2 硬件殿后）。逐字段校验（范围见结构体注），
 *       任一无效 -> -1 且保持先前状态；成功 -> 0。
 *
 *   int kidpal_home_p1_render_frame(void);
 *       把当前 P1 首页画面（按注入状态 + 缓存 + 布局解析 + 打盹/眨眼相位）
 *       经 host 渲染缝写入可探针帧缓冲，返回 0 = 一帧已刷新（此后
 *       host_infra_read_pixel 可探针）；渲染失败 -> -1。
 *       实现面自由：可扩展 host_infra_seam.c 增加 P1 渲染入口（additive，
 *       不改包 0 冻结断言），或自带 LVGL 场景刷新入缝。
 *
 * While the implementation is absent, __attribute__((weak)) defaults below
 * return the sentinel HP1_UNIMPLEMENTED (-2)（指针返回件返回 NULL，但所有
 * 用它的测试先过 count 的非哨兵检查），so every behavioral test asserts
 * NOT-sentinel first: the red baseline reads "frozen contract not implemented"
 * — a real missing-behaviour red, not a link error.
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test):
 *   coverage: P1-A1 (+P1-E1) structural -> test P1-A1 (验收文件在世于
 *               taskpackage 钉的路径 + 头注 builder duty 清单在位 + 设备树
 *               home_p1 模块存在并编入 firmware/main/CMakeLists.txt)
 *   coverage: P1-E3 guard   -> test P1-E3-guard (包 2 view_router 契约底座
 *               不回归: home/schedule 路由 + NULL 降级；契约 §1.3 canary；
 *               home_p1 模块纯净性: 无 mcp/ws 下发链路词、无芯片直读词
 *               (E2 host 可编纪律))
 *   coverage: P1-A3/E3      -> test cache (inject 整体替换 + verbatim 分组
 *               结果不重算 + 次日预告读写 + 容量/形状/防御拒绝)
 *   coverage: P1-E3 (v0.4 刷新三触发设备半) -> test rollover (跨 0:00 日滚动:
 *               同日原样 / 跨日清空并更新日键 / 防御)
 *   coverage: P1-A3         -> test countdown (最近未开始项选择 + 「3 个半
 *               小时」数值半 + 已开始边界 + 全过期无贴纸 + 防御)
 *   coverage: P1-A2         -> test blink (4.4s 冒号相位 + 1s 电池闪同款函数 +
 *               周期灭窗唯一性 + 防御)
 *   coverage: P1-A4         -> test doze (阈值相对行为 + 充电叫醒覆盖 + 防御;
 *               阈值默认 15, 9b owns 调定)
 *   coverage: P1-A5         -> test layout (贪心优先级 时间>接下来>温湿度 +
 *               冲突时温湿度让位隐藏 + 精确装满边界 + 防御)
 *   coverage: P1-A2 (render)-> test render_p1a2 (大时间/日期星期/温湿度/电量
 *               区域在位 + 冒号眨眼墨水差 + 温湿度缺席隐藏)
 *   coverage: P1-A3 (render)-> test render_p1a3 (接下来卡 + 倒计时贴纸 + 次日
 *               预告在位; 无待办/全开始边界区域空)
 *   coverage: P1-A4 (render)-> test render_p1a4 (打盹电池每秒闪墨水相位 +
 *               充电叫醒覆盖不闪灭)
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，[C4.2] 约定）：
 *   1) firmware/main/display/home_p1.c + home_p1.h 实现上述 frozen offline
 *      contract（C linkage、零芯片依赖、数值码/语义以本文件为准；hp1_item_t /
 *      hp1_state_t 形状须与本文件完全一致）。
 *   2) 设备构建装配：firmware/main/CMakeLists.txt 的 SOURCES 加
 *      "display/home_p1.c"（view_router.c 同款位置）。
 *   3) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../home_p1.acceptance.c" 与
 *      "../../../display/home_p1.c"（强符号覆盖 weak 哨兵的关键，WSA/VRT 同款）。
 *   4) 渲染入缝：实现 kidpal_home_p1_render_frame 时如需扩展
 *      host_app/main/host_infra_seam.c 的渲染入口，做 additive 扩展，不得破坏
 *      包 0 冻结断言（host_infra_smoke.acceptance.c 必须保持绿）。
 *   5) 模块纯净性（P1-E3/E2 红线）：home_p1.c/h 内不得出现 mcp/websocket/wss
 *      下发链路词与 shtc3_/pcf85063_/driver/i2c 芯片直读词（guard 齿会扫）；
 *      时间/温湿度/电量一律经 hp1_state_t / inject seam 注入。
 *
 * RED baseline (vehicle state, 2026-09-07 acceptance-author 实测，见
 * firmware/main/test/acceptance/red-baseline/red-baseline-summary.md):
 *   - firmware/main 无 home_p1 模块（frozen contract 未实现，weak 哨兵 -2）；
 *     P1 首页渲染面不存在。
 *   - 包 2 view_router 契约底座 + 契约 §1.3 canary 为 MUST-STAY-GREEN 守卫。
 *   -> 10 条行为/结构齿 RED（各自真实缺失行为）；1 条 guard 齿 GREEN。
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY。
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-07  initial freeze (acceptance-author).
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes + constants (== firmware/main/display/home_p1.h, 不得偏移)
 * ------------------------------------------------------------------------ */
#define HP1_ITEM_MAX 8
#define HP1_TIME_LEN 6   /* "H:MM"=4 / "HH:MM"=5 chars + NUL */
#define HP1_NAME_LEN 32  /* 31 chars + NUL */

#define HP1_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define HP1_DOZE_BATTERY_THRESHOLD_PCT 15

#define HP1_BLINK_PERIOD_COLON_MS   4400
#define HP1_BLINK_PERIOD_BATTERY_MS 1000
#define HP1_BLINK_ON_NUM 3
#define HP1_BLINK_ON_DEN 4

#define HP1_ELEM_BIT_TIME 1u
#define HP1_ELEM_BIT_NEXT 2u
#define HP1_ELEM_BIT_TH   4u

/* P1 帧与布局在位钉（spec §5.5 线框映射；布局钉非字体/颜色钉）。
 * 背景 = 白 (0xFFFFFF, 1bpp 黑白设计)；墨水 = 非 0xFFFFFF 且非哨兵的探针像素。 */
#define HP1_FRAME_W 400
#define HP1_FRAME_H 300
#define HP1_INK_WHITE 0xFFFFFFul
#define HP1_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 区域矩形 {x1, y1, x2, y2} 含端点（builder 须把对应元素画进对应区域）。 */
static const int hp1t_rect_time[4]    = { 15,  15, 185,  95 }; /* 大时间 */
static const int hp1t_rect_date[4]    = {195,  15, 300,  95 }; /* 日期星期 */
static const int hp1t_rect_battery[4] = {310,  15, 395,  80 }; /* 电量 */
static const int hp1t_rect_th[4]      = { 15, 110, 120, 240 }; /* 温湿度盒 */
static const int hp1t_rect_next[4]    = {140, 110, 390, 195 }; /* 接下来卡 */
static const int hp1t_rect_sticker[4] = {140, 205, 390, 250 }; /* 倒计时贴纸 */
static const int hp1t_rect_preview[4] = {140, 258, 390, 292 }; /* 次日预告 */

typedef struct {
    char time[HP1_TIME_LEN];
    char name[HP1_NAME_LEN];
} hp1_item_t;

typedef struct {
    int hour;
    int minute;
    int env_valid;
    int temp_c;
    int humidity_pct;
    int battery_pct;
    int charging;
    int tick_ms;
} hp1_state_t;

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak defaults (sentinel -2 / NULL).
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_home_p1_schedule_inject(const char *day_key,
                                                         const hp1_item_t *items,
                                                         int count,
                                                         const hp1_item_t *next_day_preview)
{
    (void)day_key; (void)items; (void)count; (void)next_day_preview;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_schedule_count(void)
{
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) const hp1_item_t *kidpal_home_p1_schedule_item(int index)
{
    (void)index;
    return NULL;
}
__attribute__((weak)) int kidpal_home_p1_schedule_preview(hp1_item_t *out)
{
    (void)out;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_day_rollover(const char *now_day)
{
    (void)now_day;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_countdown(const hp1_item_t *items, int count,
                                                   int now_h, int now_m,
                                                   int *out_index, int *out_hours,
                                                   int *out_minutes)
{
    (void)items; (void)count; (void)now_h; (void)now_m;
    (void)out_index; (void)out_hours; (void)out_minutes;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_blink_visible(int tick_ms, int period_ms)
{
    (void)tick_ms; (void)period_ms;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_doze_state(int battery_pct, int charging)
{
    (void)battery_pct; (void)charging;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_layout_resolve(int time_h, int next_h,
                                                        int th_h, int avail_h)
{
    (void)time_h; (void)next_h; (void)th_h; (void)avail_h;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_set_state(const hp1_state_t *st)
{
    (void)st;
    return HP1_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_render_frame(void)
{
    return HP1_UNIMPLEMENTED;
}

/* host 渲染缝（包 0 落地，host_app/main/host_infra_seam.c 提供；红基线 stub
 * 脚手架由 red-baseline/seam_stub.c 供链）。只探针，不在此定义。 */
int     host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

/* 包 2 已 merged 的 display 路由冻结契约（firmware/main/display/view_router.c
 * 强符号；gate runner 与红基线 stub run 都显式链入）。本文件不提供 weak 默认——
 * 该契约已在世，guard 齿断言的是「不回归」。 */
extern int kidpal_display_route(const char *view);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (structural teeth), VRT/WSA precedent. Excluded
 * dirs: test(本文件所在)/host_app/build/.git/assets 均非实现面。
 * ------------------------------------------------------------------------ */
#define HP1T_MAX_PATH 1024
#define HP1T_MAX_READ (8u * 1024u * 1024u)

static char hp1t_root[HP1T_MAX_PATH];
static int hp1t_root_ready;

static int hp1t_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int hp1t_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int hp1t_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int hp1t_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[HP1T_MAX_PATH];

    if (hp1t_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (hp1t_is_regular(probe)) {
            snprintf(hp1t_root, sizeof(hp1t_root), "%s", cands[i]);
            hp1t_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int hp1t_rel(char *out, size_t cap, const char *rel)
{
    if (!hp1t_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", hp1t_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", hp1t_root, rel);
    return 1;
}

static int hp1t_exists(const char *rel)
{
    char p[HP1T_MAX_PATH];
    return hp1t_rel(p, sizeof(p), rel) && hp1t_is_regular(p);
}

static char *hp1t_read(const char *rel, size_t *out_len)
{
    char p[HP1T_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!hp1t_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > HP1T_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int hp1t_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    n = strlen(needle);
    if (n == 0) {
        return 1;
    }
    for (const char *h = hay; *h != '\0'; h++) {
        size_t i = 0;
        while (i < n && h[i] != '\0' &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static int hp1t_file_contains_ci(const char *rel, const char *needle)
{
    char *buf = hp1t_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = hp1t_contains_ci(buf, needle);
    free(buf);
    return r;
}

static int hp1t_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                        const char *needle, char *first_hit,
                                        size_t cap, int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[HP1T_MAX_PATH];
        char rel_next[HP1T_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            hp1t_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += hp1t_count_files_with_needle(abs_next, rel_next, needle,
                                                  first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > HP1T_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL); /* logic guard */
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                if (hp1t_contains_ci(buf, needle)) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int hp1t_scan_count(const char *rel_dir, const char *needle,
                           char *first_hit, size_t cap)
{
    char abs[HP1T_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!hp1t_rel(abs, sizeof(abs), rel_dir) || !hp1t_is_dir(abs)) {
        return 0;
    }
    return hp1t_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

#define HP1T_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(hp1t_locate_root(), \
    "HP1 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static void hp1t_require_impl(int value, const char *what)
{
    if (value == HP1_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "HP1 RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic home_p1 module per the frozen "
                 "contract in firmware/main/test/home_p1.acceptance.c header and "
                 "wire it into HOST_ACCEPTANCE_SRCS + firmware/main/CMakeLists.txt)",
                 what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* 墨水探针：区域 [x1..x2]x[y1..y2] 内非白非哨兵像素数。 */
static int hp1t_ink(int x1, int y1, int x2, int y2)
{
    int ink = 0;

    assert(x1 >= 0 && y1 >= 0 && x2 < HP1_FRAME_W && y2 < HP1_FRAME_H);
    assert(x1 <= x2 && y1 <= y2);
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            int32_t p = host_infra_read_pixel(x, y);
            if (p != (int32_t)HP1_INK_WHITE && p != HP1_NOT_RENDERED_SENTINEL) {
                ink++;
            }
        }
    }
    return ink;
}

static int hp1t_ink_rect(const int *r)
{
    return hp1t_ink(r[0], r[1], r[2], r[3]);
}

/* -------------------------------------------------------------------------- *
 * P1-A1 (+P1-E1) — structural: 验收文件在世 + builder duty 头注 + 模块装配。
 *   1) 本验收文件在世于 taskpackage 钉的路径 firmware/main/test/home_p1.acceptance.c;
 *   2) 头注 BUILDER DUTY 清单在位（自扫 HOST_ACCEPTANCE_SRCS / display/home_p1.c 标记）;
 *   3) 设备树存在实现 frozen contract 的 home_p1 模块（needle
 *      kidpal_home_p1_schedule_inject(）;
 *   4) 模块编入设备构建：firmware/main/CMakeLists.txt 引用 home_p1.c（装配事实，
 *      非浮文件）。
 * RED today: teeth 3) 4) 无实现。teeth 1) 2) 是在世守卫（must stay green）。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 验收文件在世 + home_p1 模块装配结构 (P1-A1/P1-E1)",
          "[home_p1][P1-A1][structural]")
{
    char hit[HP1T_MAX_PATH];
    int n_files;

    HP1T_REQUIRE_ROOT();

    /* tooth 1: 验收文件在世于 taskpackage.acceptanceTestsFile 钉的路径 */
    TEST_ASSERT_TRUE_MESSAGE(
        hp1t_exists("firmware/main/test/home_p1.acceptance.c"),
        "P1-A1 RED(infra): the frozen acceptance file is missing from its "
        "taskpackage-pinned path firmware/main/test/home_p1.acceptance.c");

    /* tooth 2: 头注 BUILDER DUTY 清单在位（[C4.2] 接线项已列） */
    {
        char *self = hp1t_read("firmware/main/test/home_p1.acceptance.c", NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(self, "P1-A1 RED(infra): acceptance file unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            hp1t_contains_ci(self, "HOST_ACCEPTANCE_SRCS") &&
            hp1t_contains_ci(self, "display/home_p1.c"),
            "P1-A1 RED(infra): acceptance file header lost the builder-duty wiring "
            "markers (HOST_ACCEPTANCE_SRCS + display/home_p1.c) — 接线职责清单缺失");
        free(self);
    }

    /* tooth 3: 设备树存在 home_p1 模块（frozen contract 实现面） */
    n_files = hp1t_scan_count("firmware/main", "kidpal_home_p1_schedule_inject(",
                              hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_files >= 1,
        "P1-A1 RED: no module under firmware/main implements the frozen home_p1 "
        "offline contract (kidpal_home_p1_schedule_inject absent) — 首页日程缓存/"
        "渲染纯逻辑模块缺失 (P1-A1/P1-E1)");

    /* tooth 4: 模块编入设备构建（装配，非浮文件） */
    TEST_ASSERT_TRUE_MESSAGE(
        hp1t_file_contains_ci("firmware/main/CMakeLists.txt", "home_p1.c"),
        "P1-A1 RED: home_p1.c is not referenced by firmware/main/CMakeLists.txt "
        "— 模块未编入设备构建（装配缺失; BUILDER DUTY 2）");
}

/* -------------------------------------------------------------------------- *
 * P1-E3 guard — 包 2 契约底座不回归 + 契约 canary + 模块纯净性（必须保持绿）。
 *   - view_router 冻结契约（包 2 已 merged）：home->1 / schedule->4 / text->0 /
 *     NULL->-1 强符号真执行（红基线 stub run 显式链 view_router.c）;
 *   - 契约 §1.3 canary：interface-contract.md 在世且 home/schedule view 定义
 *     仍在（本包只读消费、零契约触碰的 canary）;
 *   - home_p1 模块纯净性（P1-E3: 不重复实现下发链路；P1-E2: 零芯片依赖
 *     host 可编）——模块落地后为强守卫，未落地时平凡绿。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 包 2 路由底座不回归 + 契约 canary + 模块纯净性 (P1-E3/E2 guard)",
          "[home_p1][P1-E3][guard]")
{
    HP1T_REQUIRE_ROOT();

    /* view_router 冻结契约不回归（包 2 底座, VRT 数值码） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("home") == 1,
        "P1-E3 RED(regression): view_router \"home\" route broken — 包 2 display "
        "路由底座回归 (契约 §1.3 ⑥)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("schedule") == 4,
        "P1-E3 RED(regression): view_router \"schedule\" route broken — 包 5/6 底座 "
        "路由回归 (契约 §1.3 ②)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("text") == 0,
        "P1-E3 RED(regression): view_router \"text\" route broken");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route(NULL) == -1,
        "P1-E3 RED(regression): view_router NULL view must degrade to -1 (V27)");

    /* 契约 §1.3 canary（零契约触碰的在世守卫） */
    TEST_ASSERT_TRUE_MESSAGE(
        hp1t_exists("docs/contracts/interface-contract.md") &&
        hp1t_file_contains_ci("docs/contracts/interface-contract.md", "home"),
        "P1-E3 RED(infra): contract file lost or home-view anchor gone — 契约面被"
        "触碰（本包只读消费, 零契约触碰红线）");

    /* home_p1 模块纯净性：无下发链路词（P1-E3 红线: 通道接线归包 6） */
    {
        const char *const module_files[] = {
            "firmware/main/display/home_p1.c", "firmware/main/display/home_p1.h"
        };
        const char *const channel_needles[] = { "mcp", "websocket", "wss:" };
        const char *const chip_needles[] = { "shtc3_", "pcf85063_", "driver/i2c" };

        for (size_t i = 0; i < 2; i++) {
            if (!hp1t_exists(module_files[i])) {
                continue; /* 未落地时平凡绿; 落地后为强守卫 */
            }
            for (size_t k = 0; k < 3; k++) {
                char msg[512];
                snprintf(msg, sizeof(msg),
                         "P1-E3 RED(scope): %s mentions \"%s\" — home_p1 模块不得"
                         "实现/引用任何下发链路（mcp/WS 归包 6, P1-E3 红线: 只消费"
                         "包 2 底座 + inject seam 是唯一数据入口）",
                         module_files[i], channel_needles[k]);
                TEST_ASSERT_FALSE_MESSAGE(
                    hp1t_file_contains_ci(module_files[i], channel_needles[k]), msg);
            }
            for (size_t k = 0; k < 3; k++) {
                char msg[512];
                snprintf(msg, sizeof(msg),
                         "P1-E2 RED(scope): %s mentions \"%s\" — home_p1 模块零芯片"
                         "依赖（RTC/SHTC3 直读殿后 9b, 数据经 hp1_state_t 注入; host"
                         " 可编纪律）",
                         module_files[i], chip_needles[k]);
                TEST_ASSERT_FALSE_MESSAGE(
                    hp1t_file_contains_ci(module_files[i], chip_needles[k]), msg);
            }
        }
    }
}

/* -------------------------------------------------------------------------- *
 * P1-A3 / P1-E3 — 今日日程下发缓存（behavioral, frozen offline contract）。
 *   - inject 整体替换（不 append）+ verbatim 分组结果（「8:00 上学」原样，
 *     设备不重算分组/不合并学校课）+ 次日预告读写;
 *   - 「今日无待办」count==0 推送清空缓存;
 *   - 容量/形状/防御拒绝（超容量、NULL、坏 time 形状、坏预告、无效 day_key），
 *     且拒绝必须整包不半写。
 * RED today: weak-default UNIMPLEMENTED sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 今日日程缓存 inject/读取 verbatim + 防御 (P1-A3/E3)",
          "[home_p1][P1-A3][cache]")
{
    hp1_item_t items[3];
    hp1_item_t preview;
    const hp1_item_t *it;

    memset(&items, 0, sizeof(items));
    memset(&preview, 0, sizeof(preview));

    /* 整体替换写入 + verbatim 读回（producer 分组结果, 设备不重算） */
    snprintf(items[0].time, sizeof(items[0].time), "8:00");
    snprintf(items[0].name, sizeof(items[0].name), "上学"); /* spec §4.1 分组示例 */
    snprintf(items[1].time, sizeof(items[1].time), "14:00");
    snprintf(items[1].name, sizeof(items[1].name), "钢琴课");
    snprintf(preview.time, sizeof(preview.time), "09:00");
    snprintf(preview.name, sizeof(preview.name), "游泳课");

    hp1t_require_impl(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, &preview),
                      "kidpal_home_p1_schedule_inject unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, &preview) == 0,
        "P1-A3 RED: a valid grouped-schedule inject was rejected — 分组结果注入失败");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 2,
        "P1-A3 RED: cache count != 2 after inject — 缓存计数错误");

    it = kidpal_home_p1_schedule_item(0);
    TEST_ASSERT_NOT_NULL_MESSAGE(it, "P1-A3 RED: item(0) is NULL — 缓存读取失败");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(it->time, "8:00") == 0 && strcmp(it->name, "上学") == 0,
        "P1-A3 RED: item(0) not verbatim (\"8:00\"/\"上学\") — 设备不得重算分组/"
        "不得改写 producer 分组结果 (P1-A3 分组呈现消费语义)");
    it = kidpal_home_p1_schedule_item(1);
    TEST_ASSERT_NOT_NULL_MESSAGE(it, "P1-A3 RED: item(1) is NULL");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(it->time, "14:00") == 0 && strcmp(it->name, "钢琴课") == 0,
        "P1-A3 RED: item(1) not verbatim (\"14:00\"/\"钢琴课\")");

    /* 次日预告 verbatim */
    {
        hp1_item_t pv;
        memset(&pv, 0, sizeof(pv));
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(&pv) == 0,
            "P1-A3 RED: injected next-day preview not readable — 次日预告缺失");
        TEST_ASSERT_TRUE_MESSAGE(strcmp(pv.time, "09:00") == 0 && strcmp(pv.name, "游泳课") == 0,
            "P1-A3 RED: preview content not verbatim (\"09:00\"/\"游泳课\")");
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(NULL) == -1,
        "P1-A3 RED: NULL out for preview must be rejected (-1), not crash");

    /* 越界防御 */
    TEST_ASSERT_NULL_MESSAGE(kidpal_home_p1_schedule_item(-1),
        "P1-A3 RED: item(-1) must be NULL, not OOB");
    TEST_ASSERT_NULL_MESSAGE(kidpal_home_p1_schedule_item(HP1_ITEM_MAX + 1),
        "P1-A3 RED: out-of-range item() must be NULL, not OOB");

    /* 整体替换（re-inject 不 append） */
    {
        hp1_item_t one;
        hp1_item_t pv_after;
        memset(&one, 0, sizeof(one));
        memset(&pv_after, 0, sizeof(pv_after));
        snprintf(one.time, sizeof(one.time), "16:30");
        snprintf(one.name, sizeof(one.name), "英语培训");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &one, 1, NULL) == 0,
            "P1-A3 RED: re-inject rejected");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 1,
            "P1-A3 RED: re-inject appended instead of replacing — 缓存必须整体替换");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(&pv_after) == -1,
            "P1-A3 RED: re-inject without preview must clear the old preview");
    }

    /* 「今日无待办」推送清空 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", NULL, 0, NULL) == 0,
        "P1-A3 RED: count==0 (今日无待办) push rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 0,
        "P1-A3 RED: count==0 push must clear the cache");

    /* 容量与防御：拒绝必须整包、缓存保持原样 */
    {
        hp1_item_t many[HP1_ITEM_MAX + 1];
        memset(&many, 0, sizeof(many));
        for (int i = 0; i <= HP1_ITEM_MAX; i++) {
            snprintf(many[i].time, sizeof(many[i].time), "%d:00", (i % 23) + 1);
            snprintf(many[i].name, sizeof(many[i].name), "item%d", i);
        }
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_home_p1_schedule_inject("2026-09-07", many, HP1_ITEM_MAX + 1, NULL) == -1,
            "P1-A3 RED: over-capacity inject must be rejected (-1)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 0,
            "P1-A3 RED: a rejected inject must not half-write the cache");
    }

    /* 形状/入参防御（time 形状 H:MM|HH:MM, 时分范围, NULL, day_key） */
    {
        hp1_item_t bad;
        memset(&bad, 0, sizeof(bad));
        snprintf(bad.time, sizeof(bad.time), "24:00");
        snprintf(bad.name, sizeof(bad.name), "x");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, 1, NULL) == -1,
            "P1-A3 RED: hour 24 must be rejected (坏数据不得入缓存, V27 精神)");
        snprintf(bad.time, sizeof(bad.time), "14:60");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, 1, NULL) == -1,
            "P1-A3 RED: minute 60 must be rejected");
        snprintf(bad.time, sizeof(bad.time), "ab:00");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, 1, NULL) == -1,
            "P1-A3 RED: non-numeric time must be rejected");
        snprintf(bad.time, sizeof(bad.time), "14");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, 1, NULL) == -1,
            "P1-A3 RED: malformed time \"14\" must be rejected");
        snprintf(bad.time, sizeof(bad.time), "14:00");
        bad.name[0] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, 1, NULL) == -1,
            "P1-A3 RED: empty name must be rejected");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject(NULL, &bad, 0, NULL) == -1,
            "P1-A3 RED: NULL day_key must be rejected");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("", &bad, 0, NULL) == -1,
            "P1-A3 RED: empty day_key must be rejected");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", NULL, 2, NULL) == -1,
            "P1-A3 RED: NULL items with count>0 must be rejected");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", &bad, -1, NULL) == -1,
            "P1-A3 RED: negative count must be rejected");

        /* 坏预告整包拒绝 */
        bad.name[0] = 'x';
        bad.time[0] = '\0';
        strcpy(bad.time, "xx:00");
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_home_p1_schedule_inject("2026-09-07", items, 1, &bad) == -1,
            "P1-A3 RED: invalid preview must reject the whole inject (不半写)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 0,
            "P1-A3 RED: rejected inject (bad preview) must leave cache unchanged");
    }
}

/* -------------------------------------------------------------------------- *
 * P1-E3 — 跨天滚动（v0.4 刷新三触发的设备半, behavioral）。
 *   同日 -> 0 缓存原样；跨 0:00 -> 1 且 items+预告清空、日键更新；
 *   NULL/空日键 -> -1 不崩。
 * （「连接建立推送」「新建日程即时推」的产生端在 producer/网关侧，设备半只是
 *  被动收——不进本包断言面, 头注留根。）
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 跨 0:00 日滚动——缓存日状态清空 (P1-E3)", "[home_p1][P1-E3][rollover]")
{
    hp1_item_t items[2];
    hp1_item_t pv;
    int r;

    memset(&items, 0, sizeof(items));
    memset(&pv, 0, sizeof(pv));
    snprintf(items[0].time, sizeof(items[0].time), "10:00");
    snprintf(items[0].name, sizeof(items[0].name), "美术班");
    snprintf(items[1].time, sizeof(items[1].time), "15:00");
    snprintf(items[1].name, sizeof(items[1].name), "牙医复诊");
    snprintf(pv.time, sizeof(pv.time), "09:00");
    snprintf(pv.name, sizeof(pv.name), "游泳课");

    hp1t_require_impl(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, &pv),
                      "kidpal_home_p1_schedule_inject unimplemented");
    hp1t_require_impl(kidpal_home_p1_day_rollover("2026-09-07"),
                      "kidpal_home_p1_day_rollover unimplemented");

    /* 同日：不动缓存 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_day_rollover("2026-09-07") == 0,
        "P1-E3 RED: same-day rollover must be a no-op (return 0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 2,
        "P1-E3 RED: same-day rollover must not clear the cache");

    /* 跨 0:00：清空 items + 预告, 日键更新 */
    r = kidpal_home_p1_day_rollover("2026-09-08");
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        "P1-E3 RED: crossing 0:00 must roll the cache day state (return 1) — "
        "跨天滚动设备半缺失 (v0.4 刷新三触发)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 0,
        "P1-E3 RED: after rollover the stale today-items must be cleared");
    {
        hp1_item_t pv2;
        memset(&pv2, 0, sizeof(pv2));
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(&pv2) == -1,
            "P1-E3 RED: after rollover the next-day preview must be cleared too");
    }

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_day_rollover(NULL) == -1,
        "P1-E3 RED: NULL now_day must be rejected (-1), not crash");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_day_rollover("") == -1,
        "P1-E3 RED: empty now_day must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * P1-A3 — 倒计时贴纸纯函数（behavioral）。
 *   最近未开始项选择（乱序数组也选最小未来开始时刻）+「还有 3 个半小时」数值半
 *   （210 分钟 -> 3 时 30 分）+ 开始时刻==当前算已开始 + 全部过期 -> 无贴纸(1)
 *   + 防御(-1)。
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 倒计时贴纸——最近未开始项 + 数值半 + 边界 (P1-A3)",
          "[home_p1][P1-A3][countdown]")
{
    hp1_item_t items[3];
    int idx = -99, h = -99, m = -99, r;

    memset(&items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "16:30"); /* 乱序放置 */
    snprintf(items[0].name, sizeof(items[0].name), "英语培训");
    snprintf(items[1].time, sizeof(items[1].time), "14:00");
    snprintf(items[1].name, sizeof(items[1].name), "钢琴课");

    hp1t_require_impl(kidpal_home_p1_countdown(items, 2, 10, 30, &idx, &h, &m),
                      "kidpal_home_p1_countdown unimplemented");

    /* 10:30 -> 最近未开始项 = 14:00 钢琴课, 剩余 3h30m（「还有 3 个半小时」数值半） */
    r = kidpal_home_p1_countdown(items, 2, 10, 30, &idx, &h, &m);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "P1-A3 RED: a future item must yield a countdown sticker (return 0)");
    TEST_ASSERT_TRUE_MESSAGE(idx == 1,
        "P1-A3 RED: nearest not-yet-started item not selected (乱序数组须选最小"
        "未来开始时刻 14:00=index 1)");
    TEST_ASSERT_TRUE_MESSAGE(h == 3 && m == 30,
        "P1-A3 RED: remaining time wrong (expected 3h30m = 「还有 3 个半小时」数值半)");

    /* 13:59 -> 0h1m 边界 */
    r = kidpal_home_p1_countdown(items, 2, 13, 59, &idx, &h, &m);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && h == 0 && m == 1,
        "P1-A3 RED: 13:59 -> 14:00 must be 0h1m sticker");

    /* 开始时刻 == 当前算已开始（不再贴） */
    r = kidpal_home_p1_countdown(items, 2, 14, 0, &idx, &h, &m);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idx == 0 && h == 2 && m == 30,
        "P1-A3 RED: at 14:00 the 16:30 item must be the sticker target (2h30m) — "
        "开始时刻==当前 的项算已开始, 不得再贴");

    /* 全部已开始/过期 -> 无贴纸 */
    r = kidpal_home_p1_countdown(items, 2, 17, 0, &idx, &h, &m);
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        "P1-A3 RED: all-started/expired items must yield NO sticker (return 1) "
        "— 全过期边界行为缺失");

    /* 空缓存 -> 无贴纸 */
    r = kidpal_home_p1_countdown(NULL, 0, 10, 30, &idx, &h, &m);
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        "P1-A3 RED: empty schedule must yield no sticker (return 1)");

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_countdown(NULL, 2, 10, 30, &idx, &h, &m) == -1,
        "P1-A3 RED: NULL items with count>0 must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_countdown(items, 2, 10, 30, NULL, &h, &m) == -1,
        "P1-A3 RED: NULL out_index must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_countdown(items, 2, 24, 0, &idx, &h, &m) == -1,
        "P1-A3 RED: now_h=24 must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_countdown(items, 2, 10, 60, &idx, &h, &m) == -1,
        "P1-A3 RED: now_m=60 must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_countdown(items, -1, 10, 30, &idx, &h, &m) == -1,
        "P1-A3 RED: negative count must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * P1-A2 — 眨眼冒号相位纯函数（behavioral）。4.4s 周期「眨眼双点」+ 打盹电池
 *   1s 闪同款函数 + 每周期恰一个灭窗（period*1/4）+ 防御。
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 眨眼冒号相位——4.4s 一眨 + 电池 1s 同款 (P1-A2)",
          "[home_p1][P1-A2][blink]")
{
    int on_num = 0, total = 0;
    const int on_window = HP1_BLINK_PERIOD_COLON_MS * HP1_BLINK_ON_NUM / HP1_BLINK_ON_DEN;

    hp1t_require_impl(kidpal_home_p1_blink_visible(0, HP1_BLINK_PERIOD_COLON_MS),
                      "kidpal_home_p1_blink_visible unimplemented");

    /* 冒号 4.4s 周期: 0..3299 可见, 3300..4399 灭, 4400 回到可见（wrap） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(0, HP1_BLINK_PERIOD_COLON_MS) == 1,
        "P1-A2 RED: tick 0 must be colon-visible");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_blink_visible(on_window - 1, HP1_BLINK_PERIOD_COLON_MS) == 1,
        "P1-A2 RED: last visible tick of the blink window must be visible");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_blink_visible(on_window, HP1_BLINK_PERIOD_COLON_MS) == 0,
        "P1-A2 RED: colon must be hidden in the blink-off window (4.4s 一眨)");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_blink_visible(HP1_BLINK_PERIOD_COLON_MS - 1,
                                     HP1_BLINK_PERIOD_COLON_MS) == 0,
        "P1-A2 RED: end of period must still be hidden");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_blink_visible(HP1_BLINK_PERIOD_COLON_MS,
                                     HP1_BLINK_PERIOD_COLON_MS) == 1,
        "P1-A2 RED: tick wrap must restart the visible window");

    /* 性质: 每周期恰一个灭窗 = period*1/4 */
    for (int t = 0; t < HP1_BLINK_PERIOD_COLON_MS; t++) {
        total++;
        if (kidpal_home_p1_blink_visible(t, HP1_BLINK_PERIOD_COLON_MS) == 0) {
            on_num++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(total == HP1_BLINK_PERIOD_COLON_MS,
        "P1-A2 RED(infra): phase sampler did not run the full period");
    TEST_ASSERT_TRUE_MESSAGE(on_num == HP1_BLINK_PERIOD_COLON_MS - on_window,
        "P1-A2 RED: off-window per period != period*1/4 — 相位函数漂移");

    /* 打盹电池闪 = 1s 周期同款相位函数 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_blink_visible(0, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
        kidpal_home_p1_blink_visible(749, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
        kidpal_home_p1_blink_visible(750, HP1_BLINK_PERIOD_BATTERY_MS) == 0,
        "P1-A2 RED: 1s battery-blink period must use the same phase function "
        "(visible 0..749, hidden 750..999)");

    /* 防御: period<=0 / tick<0 不崩、恒可见 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(123, 0) == 1,
        "P1-A2 RED: period 0 must be defended (visible), not divide-by-zero");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(123, -5) == 1,
        "P1-A2 RED: negative period must be defended (visible)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(-1, HP1_BLINK_PERIOD_COLON_MS) == 1,
        "P1-A2 RED: negative tick must be defended (visible)");
}

/* -------------------------------------------------------------------------- *
 * P1-A4 — 低电打盹态判定（behavioral）。阈值相对行为（默认 15, 9b owns 调定）
 *   + 充电叫醒覆盖 + 防御。
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 低电打盹——阈值相对行为 + 充电叫醒 (P1-A4)",
          "[home_p1][P1-A4][doze]")
{
    hp1t_require_impl(kidpal_home_p1_doze_state(50, 0),
                      "kidpal_home_p1_doze_state unimplemented");

    /* 阈值相对行为: <= TH 且非充电 -> 1; > TH -> 0 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT, 0) == 1,
        "P1-A4 RED: battery at threshold must enter state-sleepy (<=)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT - 1, 0) == 1,
        "P1-A4 RED: battery below threshold must enter state-sleepy");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT + 1, 0) == 0,
        "P1-A4 RED: battery above threshold must stay awake");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(0, 0) == 1,
        "P1-A4 RED: 0%% battery must enter state-sleepy");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(100, 0) == 0,
        "P1-A4 RED: full battery must stay awake");

    /* 充电即叫醒（覆盖任何电量, 哪怕 0%） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT, 1) == 0,
        "P1-A4 RED: charging at threshold must WAKE the device (充电叫醒覆盖)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(0, 1) == 0,
        "P1-A4 RED: charging at 0%% must wake the device");

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(101, 0) == -1,
        "P1-A4 RED: battery 101 must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(-1, 0) == -1,
        "P1-A4 RED: battery -1 must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(50, 2) == -1,
        "P1-A4 RED: charging=2 must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * P1-A5 — 布局优先级解析纯函数（behavioral）。spec §5.1: 时间 > 接下来 > 温湿度;
 *   贪心按优先级序放置; 冲突时温湿度让位 = 隐藏（钉死语义）; 精确装满边界;
 *   防御。
 * RED today: weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: 布局优先级——冲突时温湿度让位 (P1-A5)", "[home_p1][P1-A5][layout]")
{
    int r;

    hp1t_require_impl(kidpal_home_p1_layout_resolve(40, 60, 30, 200),
                      "kidpal_home_p1_layout_resolve unimplemented");

    /* 无冲突: 全保留 */
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 200);
    TEST_ASSERT_TRUE_MESSAGE(r == (int)(HP1_ELEM_BIT_TIME | HP1_ELEM_BIT_NEXT | HP1_ELEM_BIT_TH),
        "P1-A5 RED: no-conflict layout must keep all three elements");

    /* 精确装满边界: sum == avail -> 全保留 */
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 130);
    TEST_ASSERT_TRUE_MESSAGE(r == (int)(HP1_ELEM_BIT_TIME | HP1_ELEM_BIT_NEXT | HP1_ELEM_BIT_TH),
        "P1-A5 RED: exact-fit (sum == avail) must keep all elements");

    /* 冲突 1px: 温湿度让位（隐藏）, 时间/接下来保住 —— 优先级序的行为承载 */
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 129);
    TEST_ASSERT_TRUE_MESSAGE(r == (int)(HP1_ELEM_BIT_TIME | HP1_ELEM_BIT_NEXT),
        "P1-A5 RED: on conflict the 温湿度 element must yield (hidden) while "
        "时间/接下来 stay — 布局优先级 时间>接下来>温湿度 (spec §5.1) 缺失");

    /* 更深冲突: 温湿度+接下来都让位, 时间保住 */
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 100);
    TEST_ASSERT_TRUE_MESSAGE(r == (int)HP1_ELEM_BIT_TIME,
        "P1-A5 RED: deeper conflict must drop 接下来 too, keeping 时间 — "
        "低优先级先让位的贪心序缺失");

    /* 连时间都装不下 -> 空布局 */
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 39);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "P1-A5 RED: when even 时间 does not fit, nothing can render (empty set)");

    /* 防御 */
    r = kidpal_home_p1_layout_resolve(-1, 60, 30, 200); /* 不在位元素按缺席 */
    TEST_ASSERT_TRUE_MESSAGE(r == (int)(HP1_ELEM_BIT_NEXT | HP1_ELEM_BIT_TH),
        "P1-A5 RED: negative height element must be treated as absent");
    r = kidpal_home_p1_layout_resolve(40, 60, 30, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "P1-A5 RED: zero available height must yield an empty layout");
    r = kidpal_home_p1_layout_resolve(40, 60, 30, -5);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "P1-A5 RED: negative available height must be defended (empty, no crash)");
}

/* -------------------------------------------------------------------------- *
 * P1-A2 render — P1 首页版面在位（host+SDL 渲染缝像素探针）。
 *   假数据注入 -> kidpal_home_p1_render_frame -> 区域墨水探针:
 *   大时间 / 日期星期 / 温湿度 / 电量 在位 + 冒号眨眼墨水差（灭相 ink < 亮相）
 *   + 温湿度缺席隐藏边界。
 * 区域 = 布局在位钉（builder 须画进对应区域）; 字体美学不测（design review 面）。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: P1 版面——大时间/日期/温湿度/电量在位 + 冒号眨眼 (P1-A2)",
          "[home_p1][P1-A2][render]")
{
    hp1_state_t st;
    int ink_on, ink_off, r;

    memset(&st, 0, sizeof(st));

    /* 假数据注入: 10:24 / 22C 45% / 85% 电池 / 非充电 / tick 0 + 今日日程 */
    {
        hp1_item_t items[2];
        memset(&items, 0, sizeof(items));
        snprintf(items[0].time, sizeof(items[0].time), "14:00");
        snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
        snprintf(items[1].time, sizeof(items[1].time), "16:30");
        snprintf(items[1].name, sizeof(items[1].name), "英语培训");
        hp1t_require_impl(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, NULL),
                          "kidpal_home_p1_schedule_inject unimplemented");
    }

    st.hour = 10; st.minute = 24; st.env_valid = 1;
    st.temp_c = 22; st.humidity_pct = 45;
    st.battery_pct = 85; st.charging = 0; st.tick_ms = 0;

    hp1t_require_impl(kidpal_home_p1_set_state(&st),
                      "kidpal_home_p1_set_state unimplemented");
    hp1t_require_impl(kidpal_home_p1_render_frame(),
                      "kidpal_home_p1_render_frame unimplemented (P1 版面未实现)");

    r = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "P1-A2 RED: render_frame failed — P1 首页一帧未刷新入缝");

    /* 四元素区域在位（墨水 > 0） */
    ink_on = hp1t_ink_rect(hp1t_rect_time);
    TEST_ASSERT_TRUE_MESSAGE(ink_on > 0,
        "P1-A2 RED: 大时间 region has no ink — 超大时间未渲染进布局钉区域 "
        "(spec §5.5 线框; seam 未入缝也会在此红)");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_date) > 0,
        "P1-A2 RED: 日期星期 region has no ink — 日期/星期未渲染");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_battery) > 0,
        "P1-A2 RED: 电量 region has no ink — 电量未渲染");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_th) > 0,
        "P1-A2 RED: 温湿度 region has no ink — 温度小人/水滴宝宝未渲染 "
        "(spec §4.1 S-F1.1)");

    /* 冒号眨眼: 灭相 tick=4000 (>=3300 灭窗) 时时间区墨水严格变少 */
    ink_off = -1;
    st.tick_ms = 4000;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A2 RED: set_state with blink-off tick rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A2 RED: re-render at blink-off tick failed");
    ink_off = hp1t_ink_rect(hp1t_rect_time);
    TEST_ASSERT_TRUE_MESSAGE(ink_off < ink_on,
        "P1-A2 RED: colon blink must REMOVE ink from the time region at the "
        "blink-off phase (眨眼双点 4.4s 一眨)");

    /* 温湿度缺席边界: env_valid=0 -> 温湿度区隐藏（无墨水） */
    st.env_valid = 0;
    st.tick_ms = 0;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A2 RED: set_state env_valid=0 rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A2 RED: re-render without env data failed");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_th) == 0,
        "P1-A2 RED: 温湿度 region must be hidden (no ink) when env_valid=0 — "
        "无数据仍渲染温湿度区 = 展示假数据");
}

/* -------------------------------------------------------------------------- *
 * P1-A3 render — 接下来卡 + 倒计时贴纸 + 次日预告在位（host+SDL 渲染缝）。
 *   有待办 -> 三区域在位; 无待办 -> 三区域全空; 全部已开始 -> 卡在、贴纸空。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: P1 版面——接下来卡/倒计时贴纸/次日预告 (P1-A3)",
          "[home_p1][P1-A3][render]")
{
    hp1_state_t st;
    hp1_item_t items[2];
    hp1_item_t pv;

    memset(&st, 0, sizeof(st));
    memset(&items, 0, sizeof(items));
    memset(&pv, 0, sizeof(pv));

    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    snprintf(pv.time, sizeof(pv.time), "09:00");
    snprintf(pv.name, sizeof(pv.name), "游泳课");

    hp1t_require_impl(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, &pv),
                      "kidpal_home_p1_schedule_inject unimplemented");

    st.hour = 10; st.minute = 30; st.env_valid = 1;
    st.temp_c = 22; st.humidity_pct = 45;
    st.battery_pct = 85; st.charging = 0; st.tick_ms = 0;

    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A3 RED: set_state rejected");
    hp1t_require_impl(kidpal_home_p1_render_frame(),
                      "kidpal_home_p1_render_frame unimplemented (P1 版面未实现)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A3 RED: render_frame failed");

    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_next) > 0,
        "P1-A3 RED: 接下来卡 region has no ink — 分组日程卡未渲染 (P1-A3)");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_sticker) > 0,
        "P1-A3 RED: 倒计时贴纸 region has no ink — 最近项倒计时贴纸未渲染 "
        "(P1-A3, spec §4.1 「还有 3 个半小时」)");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_preview) > 0,
        "P1-A3 RED: 次日预告 region has no ink — 次日项未渲染 (P1-A3)");

    /* 边界: 「今日无待办」-> 三区域全空（离线/无日程不渲染假卡） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", NULL, 0, NULL) == 0,
        "P1-A3 RED: empty-schedule push rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A3 RED: re-render with empty schedule failed");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_next) == 0 &&
                             hp1t_ink_rect(hp1t_rect_sticker) == 0 &&
                             hp1t_ink_rect(hp1t_rect_preview) == 0,
        "P1-A3 RED: with no today-items the 接下来/贴纸/预告 regions must all be "
        "empty — 无待办仍渲染卡 = 假数据");

    /* 边界: 全部已开始(17:00 > 16:30) -> 卡在、贴纸空 */
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_inject("2026-09-07", items, 2, NULL) == 0,
        "P1-A3 RED: re-inject for all-started case rejected");
    st.hour = 17; st.minute = 0;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A3 RED: set_state 17:00 rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A3 RED: re-render for all-started case failed");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_next) > 0,
        "P1-A3 RED: 接下来卡 must still render today-items after they started");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_sticker) == 0,
        "P1-A3 RED: 倒计时贴纸 must disappear when all items started/expired "
        "(全过期 -> 无贴纸, P1-A3 冻结边界)");
}

/* -------------------------------------------------------------------------- *
 * P1-A4 render — 打盹态电池每秒闪 + 充电叫醒覆盖（host+SDL 渲染缝）。
 *   电量 5% 非充电: tick 0 (亮相) 电池区有墨, tick 750 (灭相) 电池区无墨;
 *   充电中同电量同灭相 tick: 电池区仍有墨（充电叫醒, 不闪灭）。
 * RED today: render weak sentinel。
 * ------------------------------------------------------------------------ */
TEST_CASE("home p1: P1 版面——打盹电池每秒闪 + 充电叫醒 (P1-A4)",
          "[home_p1][P1-A4][render]")
{
    hp1_state_t st;

    memset(&st, 0, sizeof(st));
    st.hour = 22; st.minute = 0; st.env_valid = 1;
    st.temp_c = 20; st.humidity_pct = 50;
    st.battery_pct = 5; st.charging = 0; st.tick_ms = 0;

    hp1t_require_impl(kidpal_home_p1_set_state(&st),
                      "kidpal_home_p1_set_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A4 RED: set_state doze case rejected");
    hp1t_require_impl(kidpal_home_p1_render_frame(),
                      "kidpal_home_p1_render_frame unimplemented (P1 版面未实现)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A4 RED: doze-state render failed");

    /* 打盹 + 亮相 tick 0: 电池图形在 */
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_battery) > 0,
        "P1-A4 RED: doze battery icon missing at blink-on phase");

    /* 灭相 tick 750: 电池图形闪灭（每秒闪, 1s 周期同款相位函数） */
    st.tick_ms = 750;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A4 RED: set_state blink-off tick rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A4 RED: blink-off re-render failed");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_battery) == 0,
        "P1-A4 RED: battery icon must blink OFF at the blink-off phase in doze "
        "(pose-sleep + 电池图形每秒闪, spec §4.1/§4.0 state-sleepy)");

    /* 充电叫醒覆盖: 同电量同灭相 tick, 充电中不闪灭 */
    st.charging = 1;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "P1-A4 RED: set_state charging case rejected");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "P1-A4 RED: charging render failed");
    TEST_ASSERT_TRUE_MESSAGE(hp1t_ink_rect(hp1t_rect_battery) > 0,
        "P1-A4 RED: charging must override doze (充电即叫醒) — battery region "
        "must NOT blink off while charging");
}

/* -------------------------------------------------------------------------- *
 * 附注（不在断言面，留根）：
 * - P1-E2: RTC 走时 + SHTC3 温湿度直读为硬件强绑定子能力，本文件 host 段只用
 *   hp1_state_t 注入测布局/逻辑；真实硬件读取殿后包 9b device 验收
 *   (PROPOSAL §3 留根)。模块纯净性齿（无 shtc3_/pcf85063_/driver/i2c）在
 *   P1-E3 guard 扫描。
 * - v0.4 刷新三触发: 「连接建立推送」「新建日程即时推」产生端在 producer/
 *   网关侧，设备半只是被动收 inject seam——不进本包断言面。
 * - 反白标签牌样式、Baloo 数字字体、眨眼动画观感等纯审美 = design review 面，
 *   不进脆验收（acceptance-evidence.md「UI 验收 vs 设计 review」两桶纪律）。
 * --------------------------------------------------------------------------- */
