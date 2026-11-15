# dispatch-progress · acceptance-author · 261104-k4-reward-goal-config

> 进度快照（非权威、非冻结件）。生成：2026-11-15，工作树 `C:/AI/KidPal-pod-m2`，分支 `dev/pod-m2`，base `cad07f2c`。

## 已交付

| 制品 | 路径 | 状态 |
|---|---|---|
| server 冻结验收 | `server/acceptance/test_reward_goal_config.acceptance.py`（K4G-A1..A9） | 已落盘 |
| firmware host 冻结验收 | `firmware/main/test/reward_goal_config.acceptance.c`（K4G-H1..H5） | 已落盘 |
| host CMakeLists 登记 | `firmware/main/test/host_app/main/CMakeLists.txt` | **未落盘（scope deny）** |

## 偏差申报（必须由 builder 补的一步）

- acceptance-author 的写边界（`.claude/hooks/enforce-scope.mjs`）不含
  `firmware/main/test/host_app/main/CMakeLists.txt`，Edit 被钩子拦截。
- 因此 `HOST_ACCEPTANCE_SRCS` 中**新增登记行由 builder 第一步补**（plan.md「技术方案」E 节
  本就规定 builder 登记）：
  ```
  "../../reward_goal_config.acceptance.c"
  ```
  位置：`points_device_ui.acceptance.c` 之后、`host_infra_smoke.acceptance.c` **之前**
  （smoke 恒末位）。未登记时 K4G-H5 TEST 5 首齿即红（登记行缺失即红，属设计态红）。
- 本文件已自带 weak 哨兵，登记前也能编译；登记后行为齿由 `display/points_page.c` 强符号覆盖。

## 红基线

- server：`python -m pytest -q --import-mode=importlib server/acceptance/test_reward_goal_config.acceptance.py`
  —— 期望 K4G-A1..A9 全红（target 三子命令未落地 / SKILL.md 旧硬阻断句仍在）。
- host：`wsl` 下 host_app 构建 + runner，期望 K4G-H1..H5 行为/结构齿红、K4G-H4 回归半绿。
- 实测结果见交付报告（本快照不复制运行输出）。

## 留待真实 QQ + 设备轮（K4G-L1 live-manual）

1. 真实 QQ 自定义新增 → 切换 → 删除全过程（含删预置/删激活中被拒的一次性回复话术）。
2. 切目标后设备其他页不抢前台、积分页原地刷新；顺带记录「QQ 消息→积分页刷新」实测时延（不写死阈值）。
3. 场景标/刻度随切看屏（sc-choco/sc-movie/sc-gift/sc-flag 四枚新美术 + 默认摩天轮）。
4. 大分值目标（200）与越目标（余额 > 目标）同页结构看屏。
5. 3 槽按名重载的 RAM 资源预算记录。
6. 旧场景标在切换后不再出现在帧里的设备截图证据（缓存方案 A 的代价项）。