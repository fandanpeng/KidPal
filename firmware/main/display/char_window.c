/*
 * char_window.c — KidPal 查字窗驻留计时纯逻辑实现 (TaskPackage
 * 260917-m2-k3-char-window-overlay, M2K3-A3)。纯逻辑实现，语义以冻结验收
 * firmware/main/test/char_window_overlay.acceptance.c 文件头 seam [3] 为准。
 *
 * 无 chip/FreeRTOS/ESP-component/LVGL 依赖; host runner 与设备树同源可编
 * （host 强符号覆盖验收文件的 weak 哨兵）。时间全为显式注入毫秒，无真实 timer。
 */

#include "char_window.h"

#include <stddef.h> /* NULL */

int kidpal_char_window_idle_step(unsigned long *idle_ms, unsigned long dt_ms,
                                 int key_pressed)
{
    if (idle_ms == NULL) {
        return -1; /* 空指针防御：从不为 UB */
    }
    if (key_pressed != 0) {
        /* 任意按键 = 操作：计时归零重计（v1.43「计时随任意按键重计」），且本次不关窗。 */
        *idle_ms = 0UL;
        return 0;
    }
    *idle_ms += dt_ms;
    return (*idle_ms >= KIDPAL_CHAR_WINDOW_IDLE_MS) ? 1 : 0;
}