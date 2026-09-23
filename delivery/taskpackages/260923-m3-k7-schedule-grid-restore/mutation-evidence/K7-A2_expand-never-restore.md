# Mutation Evidence · K7-A2 薄载荷展开恒不置 restore
- mutation: 在 schedule.py `expand_schedule_intent_payload` 返回前对展开产物置 `data["restore"] = True`（破「展开恒不置」生产纪律）
- cmd: `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_schedule_grid_restore.acceptance.py -k a2`
- 结果: 1 failed, 2 passed（target red）
```
            self.fail("K7-A2 RED(infra): " + err)
        model = _parse(B1_MD)
        today = _today_iso()
FAILED server\acceptance\test_schedule_grid_restore.acceptance.py::K7A2ExpandNeverRestoreAcceptance::test_a2_expand_output_never_carries_restore
```
- 状态: 已 `git checkout` 还原（工作树零残留）；原始输出转存（planner 实跑，integrator 因未提交 diff 风险未跑，实现已 commit 1ea01a5 后补跑安全）。
