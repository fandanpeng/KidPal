# dispatch-progress · acceptance-author · 261104-k3-qq-points-channel

> 自包含进度快照（可覆盖重写）。生成 2026-11-14。

## 产出
- 冻结验收文件（新建，唯一写点）：`server/acceptance/test_qq_points_channel.acceptance.py`
- 覆盖面 K3Q-A1..A7，共 20 个测试方法（19 设计态红 + 1 must-stay-green 回归）
- sourceRefs：S-F6.4-AC1..AC5 / S-F6.3-AC3·AC4·AC5 / S-F6.2-AC4 / NFR-PERF-4
- 不含 device-outcome / host-uncompilable-path 标记（本包合并 diff 无固件路径）

## 红基线命令与结果
`python -m pytest -q --import-mode=importlib server/acceptance/test_qq_points_channel.acceptance.py`
- 结果：**19 failed, 1 passed**（1 passed = `test_k3q_a7_deploy_verify_manifest_driven`，must-stay-green 回归）
- 红因（设计态，皆为行为缺席而非测试错误）：
  - points.py 六子命令 CLI 缺席：add 旗标形 / redeem --points / delete / show / recent / targets /
    revision+command_id 回执 / `_delete_entry_locked` 均不在位（当前仅 add/redeem 位置参数）
  - `kidpal-points-write/SKILL.md` 不在位（K3Q-A5 四项 setUp 直接红）
  - manifest 无 kidpal-points-write 条目（K3Q-A7）

## 既有 points 套件（回归）
`python -m pytest -q --import-mode=importlib server/acceptance/test_points_*.acceptance.py`
- 结果：**20 passed, 2 warnings**（零新增红；warning 为既有 asyncio 弃用 + cp1252 线程告警）

## 留给真实 QQ 轮（K3Q-E4，不落自动断言）
- ask_user 不触发 / 无 900s 卡死 / 后续消息不排队
- NFR-PERF-4 实测时延（QQ 消息→积分页原地刷新，记值不写死）
- 其他页不抢前台 / 积分页原地刷新（设备看屏串口）
- 模型真实遵守 SKILL.md
- command_id 平台消息 id 可得性与降级哈希
- 兑换「真实兑现后才扣」人工确认流程
- 同一次兑换真实重发不重复扣、歧义输入普通文本回复到达

## 下一步
- builder 按文件头 FROZEN OFFLINE CONTRACT 实现 points.py 六子命令 CLI + SKILL.md + manifest 条目
- planner Step 8 回填 acceptance[]（K3Q-A1..A7）、核验收对 story 忠实度