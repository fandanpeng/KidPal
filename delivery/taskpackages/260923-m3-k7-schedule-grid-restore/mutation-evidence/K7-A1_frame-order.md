# Mutation Evidence · K7-A1 建连帧序
- mutation: 对调 `_push_schedule_connect` 内 home/grid 推序（grid 先）
- cmd: `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_schedule_grid_restore.acceptance.py -k a1`
- 结果: 1 failed, 3 passed（target red）
```
E  AssertionError: Lists differ: ['schedule', 'home'] != ['home', 'schedule']
   ...: K7-A1 RED: 建连帧序 [('schedule', True), ('home', None)] != [home, schedule]
```
- 状态: 已 revert；原始输出由 builder 实跑提供，本文件由 planner 转存（builder scope 限制）。
