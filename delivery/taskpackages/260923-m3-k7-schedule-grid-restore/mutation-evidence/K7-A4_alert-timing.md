# Mutation Evidence · K7-A4 alert 起轮时序
- mutation: 把 `_register_alert_legs(websocket, upstream)` 从起轮前**后置到建连推送之后**（破「登记腿 → 起轮询 → 建连推送」同序）
- cmd: `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_schedule_grid_restore.acceptance.py -k a4`
- 结果: 1 failed, 2 passed（target red）
```
E   AssertionError: False is not true : K7-A4 RED: ws_gateway  reg@3105 loop@2552 push@2910    ->  -> 
FAILED server\acceptance\test_schedule_grid_restore.acceptance.py::K7A4AlertTimingAcceptance::test_a4_legs_registered_before_loop_before_connect_push
```
- 状态: 已 `git checkout` 还原（工作树零残留）；原始输出转存（planner 实跑，同上）。
