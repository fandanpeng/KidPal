#ifndef KIDPAL_SNAPSHOT_H
#define KIDPAL_SNAPSHOT_H

/*
 * KidPal 1bpp 阈值化快照工具（正式版）。
 *
 * Owner: tools/simulator（dev-loop, TaskPackage 260831-simulator-lvgl-align）。
 * 目的: 模拟器是 32bit 真彩软件渲染, 与真机 1bpp 纯黑白观感有系统性差。
 *       本工具把「画布快照 → 亮度阈值化 → 2 级 PNG 落盘」做成一键自查,
 *       阈值与真机 flush 后处理同一来源(见 kidpal_snapshot.c), 使
 *       「模拟器验证结论 ≈ 真机结论」重新成立(策略文档 §3 / §8.4)。
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 一键截当前活跃屏 → 阈值化 PNG(逐像素仅 {黑,白} 两级) → 落盘 → 请求退出。
 * 落盘路径: 环境变量 KIDPAL_SNAPSHOT_PATH 指定; 未指定则写工作目录
 * kidpal_snapshot.png。截完即退(PostQuitMessage), 便于脚本化回归。 */
void kidpal_snapshot_capture_and_quit(void);

/* 在 UI settle(首帧渲染完成)后安排一次一键快照: 挂 1500ms 一次性 timer,
 * 到点调用 kidpal_snapshot_capture_and_quit()。kidpal_ui.c 只调本函数,
 * 不再内联任何快照/阈值/lodepng 逻辑。 */
void kidpal_snapshot_schedule(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_SNAPSHOT_H */