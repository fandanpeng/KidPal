/*
 * char_window.h — KidPal 查字窗驻留计时纯逻辑单源 (TaskPackage
 * 260917-m2-k3-char-window-overlay, M2K3-A3 5 分钟兜底)。
 *
 * 语义权威 = docs/requirements.md F3 窗口交互包 / §2.2 行 68 查字窗行（v1.43 兜底）:
 * 「驻留到按键或 5 分钟无操作自动关窗回原页（不随会话关闭；计时随任意按键重计）」。
 * 关窗落地面 = kidpal_render_node_dismiss_front_overlay（回原页、不翻页），与 BOOT /
 * KEY 长按同语义。
 *
 * 纯逻辑约束（照 char_stroke.c / voice_round.c 先例）：时间全部为显式注入的毫秒数
 * （无真实 timer、无 FreeRTOS）; 无 chip / ESP-component / LVGL 依赖 -> 可编入
 * host_app linux target 与设备树两侧（host 强符号覆盖冻结验收文件的 weak 哨兵）。
 * 实现见 char_window.c。冻结验收 = firmware/main/test/char_window_overlay.acceptance.c
 * 文件头 seam [3]。
 */
#ifndef KIDPAL_CHAR_WINDOW_H
#define KIDPAL_CHAR_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/* 查字窗无操作驻留上限 = 5 分钟（v1.43 兜底口径，冻结值 300000）。 */
#define KIDPAL_CHAR_WINDOW_IDLE_MS 300000UL

/* 驻留计时步进（显式注入毫秒，无真实 timer）：
 *   idle_ms 进/出 = 距最近一次操作的毫秒数（调用方持有、本函数就地推进）；
 *   key_pressed 非 0 = 本拍有按键 -> *idle_ms 归零（**计时随任意按键重计**）且返回 0
 *     （按键本身是操作，不关窗）;
 *   否则 *idle_ms += dt_ms，返回 (*idle_ms >= KIDPAL_CHAR_WINDOW_IDLE_MS) ? 1 : 0
 *     （1 = 该关窗; 持续超时幂等返回 1，窗不复活）。
 *   防御: idle_ms == NULL -> -1（从不为 UB）。 */
int kidpal_char_window_idle_step(unsigned long *idle_ms, unsigned long dt_ms,
                                 int key_pressed);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_CHAR_WINDOW_H */