/*
 * view_router.h — KidPal display view 路由的共享逻辑底座 (TaskPackage
 * 260829-voice-roundtrip, 方向 B display 消费链路).
 *
 * 三个 FROZEN OFFLINE CONTRACT 函数的声明。数值码与语义以冻结验收
 * firmware/main/test/voice_roundtrip_b.acceptance.c 文件头为准 —— 本头仅供
 * 设备消费面(mcp_server.cc 的 kidpal/display tool)与后继包 4/5/6/8 引用，
 * 不得偏移。(实现与数值码: firmware/main/display/view_router.c)
 *
 * C linkage：供 .c 冻结验收与被 .cc 消费，统一 extern "C"。
 *
 * 降级语义：白名单(view ∈ char/schedule/story_list/story_text/text/home/control)
 * 之外的 view 一律路由到显式 UNKNOWN 降级路径(值 -1)，调用方必须展示友好
 * 降级提示、绝不盲渲染 (V27 / spec §4.0 格式不遵循兜底行)。NULL 同样归降级。
 */
#ifndef KIDPAL_VIEW_ROUTER_H
#define KIDPAL_VIEW_ROUTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* view 路由数值码 (== C 冻结验收 VRT_ROUTE_*，不得偏移) */
enum {
    VRT_ROUTE_TEXT = 0,         /* text       全文上屏滚动 (§1.3 ⑤, S-F4.1) */
    VRT_ROUTE_HOME = 1,         /* home       回 P1 首页常显 (§1.3 ⑥) */
    VRT_ROUTE_CONTROL = 2,      /* control    控制动作执行 (§1.4) */
    VRT_ROUTE_CHAR = 3,         /* char       查生字 (§1.3 ①, 包 4) */
    VRT_ROUTE_SCHEDULE = 4,     /* schedule   课表 (§1.3 ②, 包 5/6) */
    VRT_ROUTE_STORY_LIST = 5,   /* story_list 故事列表 (§1.3 ③, 包 8) */
    VRT_ROUTE_STORY_TEXT = 6,   /* story_text 故事正文 (§1.3 ④, 包 8) */
    VRT_ROUTE_UNKNOWN = (-1)    /* 白名单外 view / NULL — 显式降级路径 */
};

/* control view 动作数值码 (== C 冻结验收 VRT_CTRL_*，不得偏移) */
enum {
    VRT_CTRL_END_SESSION = 0,       /* end_session     结束会话回首页 */
    VRT_CTRL_STOP_SPEAKING = 1,     /* stop_speaking   云端确认停播补充 */
    VRT_CTRL_START_RECORDING = 2,   /* start_recording 包 8 录音接合 */
    VRT_CTRL_UNKNOWN = (-1)         /* 未知/NULL 动作 — 降级 */
};

/* 路由一个 E1 display payload 的 view (契约 §1.3 白名单) 到对应渲染面。
 * 返回 VRT_ROUTE_* 数值码；白名单外 / NULL -> VRT_ROUTE_UNKNOWN (降级信号)。 */
int kidpal_display_route(
        const char *view);

/* 解析 control view 的动作 (契约 §1.4)。返回 VRT_CTRL_*；未知/NULL -> VRT_CTRL_UNKNOWN。 */
int kidpal_display_control_action(const char *action);

/* 全文分页 (S-F4.1: 答案全文上屏, human 裁定否摘要)：返回 screen_lines 行/屏下
 * 装下 text 全文所需的屏数，>=1；NULL 计为 1 (空屏)；screen_lines <= 0 防御返回 1，
 * 永不分除零崩溃。性质: pages * screen_lines >= 行数 (显示端不丢内容)。 */
int kidpal_display_text_page_count(const char *text, int screen_lines);

/* 换皮消费接线 (包 9b)：把 U 单一资产层挂到装配层注册的 VFS 根 (设备 = esp_vfs_spiffs
 * base_path，host = 真实目录)。转发 kidpal_art_mount(root)；失败返回 -1，绝不 panic，
 * 后续 load 未 mounted 自动降级为占位。root 由板级装配层在注册 SPIFFS 后传入。 */
int kidpal_display_art_mount(const char *root);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_VIEW_ROUTER_H */