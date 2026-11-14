# dispatch-progress-builder · 261104-k4-reward-goal-config

> builder 进度快照（交付工件，非权威）。最后更新 2026-11-15。

## 状态

| 面 | 状态 | 证据 |
|---|---|---|
| Step 1 硬前置（CMakeLists 登记行） | done | `reward_goal_config.acceptance.c` 已置于 `points_device_ui.acceptance.c` 之后、`host_infra_smoke.acceptance.c` 之前 |
| server CLI（target-add / target-switch / target-delete） | done | `points.py`：三子命令 + 回执 + 传输幂等（`target:` 命名空间）+ `read_state` additive 两键 |
| SKILL.md 目标配置纪律 | done | §8 旧硬阻断句删除改写；frontmatter/§2/§5/新增 §9 |
| 资产管线（4 枚新场景标） | done | `gen_assets.mjs` 4 job / `gen_art_spiffs.mjs` WHITELIST +4 与计数 82；`spiffs_root` 4 A、0 M |
| firmware seam（H1/H2） | done | `points_page.c/.h` 新增 `kidpal_points_scene_asset` / `kidpal_points_scale_x` |
| render_node 场景槽按名重载（方案 A） | done | 3 槽固定分工 + strcmp 命中 + 换名释放旧缓冲重载；场景标取映射结果 |
| 冻结验收 server 面 | **11/11 绿**（acceptance-author 修好 K4G-A1 / K4G-A7 两处冻结件缺陷后复跑） |
| 冻结验收 host 面 | 5/5 绿 | 292 tests / 3 failures（全部为 p1_auxbar 存量族） |
| 新 unit 层 | 18/18 绿 | `server/tests/test_points_goal_units.py` |
| 既有 server 回归 | 全量 `server/` = 808 passed / 9 skipped / 60 subtests，零红 | 扩单后 `test_points_spike_units.py` 51 passed |

## 阻塞（已全部关闭）

1. **K4G-A1** 未满足断言 → acceptance-author 已修，复跑绿。
2. **K4G-A7** 与 K4G-A2 冲突 → acceptance-author 已修，复跑绿。
3. **manifest 扩单** → architect APPROVE，manifest 已 append `server/tests/test_points_spike_units.py`（19 条）；
   builder 按 architect 给的最小形态把 `test_missing_file_is_zero_state` 的精确三键 equality 改为
   投影 equality + 出厂两键 + `receipts` 负断言；同文件其它断言零触碰。

## 未做

- 真机轮 / 真实 QQ 轮（K4G-L1，dev-loop 后续轮次）。
- 三条验收阻塞修复（不属 builder 写权）。