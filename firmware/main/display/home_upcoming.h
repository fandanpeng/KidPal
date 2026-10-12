/*
 * home_upcoming.h — KidPal 首页「接下来」注入接线层（A5 纯逻辑面,
 * TaskPackage 260921-m3-k2-p2-week-grid-page, 契约 §1.3⑥ home 可选 data.upcoming）。
 *
 * 语义权威 = 冻结验收 firmware/main/test/schedule_grid.acceptance.c 文件头 A5 段。
 * 本模块**只消费** home_p1 冻结 seam（kidpal_home_p1_schedule_inject），
 * home_p1.{c,h} 与归档 home_p1.acceptance.c 零修改（铁律）。
 *
 * data_json = mcp kidpal/display home 路由持有的 `data` 参数字符串。形状 =
 * {"upcoming":{"day_key":…,"items":[{time,name}…],
 *             "preview":[{time,name}…]|[]|null}}（2026-10-11 契约门多项数组化, CCP
 * 261011-p1-home-v5-rework 触点①；requirements v1.61 AC2/AC6/AC7）。preview 三态：
 * 非空数组 = 明日全量列表 / [] = 云端确认明日无项（AC6 话术载体）/ null·键缺省 =
 * 未提供（AC7 数据未达态）；板端对旧单对象 {time,name} 保留一个过渡版的解析容忍
 * （部署缓冲, 不入契约形状）。
 *
 * C linkage（被 .cc mcp_server 消费，同 view_router/home_p1 统一 extern "C"）。
 */
#ifndef KIDPAL_HOME_UPCOMING_H
#define KIDPAL_HOME_UPCOMING_H

#ifdef __cplusplus
extern "C" {
#endif

/* 消费 home 路由 data 字符串并把 data.upcoming 经 kidpal_home_p1_schedule_inject
 * 注入冻结 seam（seam 全量校验，拒绝即整包拒）。
 *
 * - data_json NULL / "" / "{}" / 无 "upcoming" 键 -> 返回 0 且 seam 状态零变化
 *   （旧 {view:home} 无 data 行为完全不变, 向后兼容——契约 §1.3⑥ additive）。
 * - "upcoming" 在：day_key 必须为 1..15 字节字符串；items 数组每项 {time,name}
 *   且 time 可解析 H:MM/HH:MM、name 非空；count <= HP1_ITEM_MAX；preview 三态
 *   （非空数组 <= HP1_PREVIEW_MAX 逐项同 items 校验 / [] = 确认空 / null·键缺省 =
 *   未提供；旧单对象容忍为 1 项列表）。成功返回 0；任何畸形/越界/seam 拒收
 *   -> 返回 -1 且 seam 状态零变化（绝不半写、绝不盲渲染）。
 * - preview.time 非 H:MM/HH:MM（如「明早」字面）-> -1（K1 裁决口径：载荷钉 HH:MM,
 *   自然语言标签不入冻结 seam）。preview 数组超 HP1_PREVIEW_MAX(8) -> -1（整包拒）。 */
int kidpal_home_upcoming_apply(const char *data_json);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_HOME_UPCOMING_H */
