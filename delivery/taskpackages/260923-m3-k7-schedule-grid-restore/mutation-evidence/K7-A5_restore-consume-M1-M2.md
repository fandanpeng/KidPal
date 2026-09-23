# Mutation Evidence · K7-A5 firmware restore 消费（M1/M2）
## M1 = 摘 restore 分支 `lvgl_port_lock(0)`
- cmd: `k7_red_wrapper.sh` → 5 Tests 2 Failures
```
TEST1 K7-A5 ... FAIL: K7-A5 RED: schedule 路由区无 lvgl_port_lock——restore 缓存写路径未显式持 LVGL 锁（challenger C3 canary）
```
## M2 = `schedule_on_screen` 去 `== KIDPAL_LAYER_LOCAL_PAGE` 判定
- cmd: `k7_red_wrapper.sh` → 5 Tests 2 Failures
```
TEST2 K7-A5 ... FAIL: K7-A5 RED: schedule_on_screen 谓词形态缺件（LOCAL_PAGE=0/KP_PAGE_SCHEDULE/top）
```
- 状态: 均已 revert；转存自 builder 实跑输出。
