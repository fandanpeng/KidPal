# Mutation Evidence · K7-A3 ECS 开关默认关
- mutation: `schedule_push.py` `grid_restore_enabled` `not isinstance -> return False` 改 `return True`（默认开）
- cmd: `python -m pytest ... -k K7A3`
- 结果: 1 failed, 1 passed（target red）
```
FAILED ...::K7A3SwitchAcceptance::test_a3_switch_decision_face_default_off
E  AssertionError: True is not false : K7-A3 RED: grid_restore_enabled(None) = True —— 默认关偏置破
```
- 状态: 已 revert；转存自 builder 实跑输出。
