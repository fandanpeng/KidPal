# Mutation Evidence · K7-E2 注册 canary（M5）
- mutation: 摘 HOST_ACCEPTANCE_SRCS 注册行（含清注释内路径字面量）
- cmd: `k7_red_wrapper.sh` → 5 Tests 2 Failures
```
TEST5 K7-E2 ... FAIL: K7-E2 RED: schedule_grid_restore.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS
```
- 状态: 已 revert；转存自 builder 实跑输出。
