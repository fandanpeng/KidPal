# Mutation Evidence · K7-A7 对端单源 face（M4）
- mutation: 摘 `schedule_push.py` 的 `def grid_restore_payload`
- cmd: `python -m pytest ... -k a1_connect_frames` → 1 failed
```
E  AssertionError: Lists differ: ['home'] != ['home', 'schedule']
   （grid_restore_payload 缺失 → front try/except 吞 AttributeError → grid 帧未发）;
   同时 native TEST4 tooth4 结构齿 face（schedule_push.py 无 def grid_restore_payload）即红。
log: WARNING kidpal-front: front_app.py:577 schedule_connect_grid_push_err err=AttributeError
```
- 状态: 已 revert；转存自 builder 实跑输出。
