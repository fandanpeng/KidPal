# Mutation Evidence · K7-A6 语音接管回归守卫（M3）
- mutation: 重排（cache 提到 render 前，破 begin_page→render→cache 序，M3-K2 冻结链拆动）
- cmd: `k7_red_wrapper.sh` → 5 Tests 2 Failures
```
TEST3 K7-A6 ... FAIL: K7-A6 RED: 薄载荷路径调用链缺件（begin_page/render/cache）——M3-K2 冻结实现被 restore 改造连带拆动
```
- 状态: 已 revert；转存自 builder 实跑输出。
