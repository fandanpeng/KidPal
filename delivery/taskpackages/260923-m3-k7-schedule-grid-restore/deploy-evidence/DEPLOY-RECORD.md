# Deploy Evidence · 260923-m3-k7-schedule-grid-restore（C1 开闸留痕 + 真机半锚）

> 执行者：主线程（planner），human 当场显式授权（AskUserQuestion 2026-09-23「授权：部署 + 开闸」）。遵守 ADR-005 时序：**固件先行 → 部署 server → 后开闸**。

## 1. 时序执行记录（2026-09-23）

| # | 动作 | 结果 |
|---|---|---|
| 1 | 烧新固件（固件先行）：`idf_run.py flash` 全量（app @0x20000 + assets，含 bootloader/partition） | ✅ Hash verified、硬复位；启动稳定（0 RE-ENUM、uptime 平滑，`boot-check-k7.log`） |
| 2 | ECS 备份：`front_app.py.bak-260923-k7-restore` + `schedule_push.py.bak-260923-k7-restore` | ✅ 备份在位 |
| 3 | ECS 部署 K7 front 代码（漂移核对后仅覆盖两件）：`schedule_push.py` md5 `e61c8cb2…`、`front_app.py` md5 `4b32b20f…` | ✅ 两侧 md5 与本地 HEAD 一致；`schedule.py` **未部署**（ECS 侧有独有差异，K7 不改它；签名兼容已核） |
| 4 | 开闸：drop-in `/etc/systemd/system/kidpal-front.service.d/kidpal-grid-restore.conf` = `Environment=KIDPAL_GRID_RESTORE_PUSH=1` + `daemon-reload` + `restart kidpal-front` | ✅ `systemctl show` 确认 env 在位、服务 active |
| 5 | 冷启动验证（13:57:32 复位） | ✅ 见 §2 |

**回滚路径**：① 灭活（秒级）= 删 drop-in + `daemon-reload` + restart（grid restore relay 不再发出，固件行为随之无效化）；② 代码回滚 = `.bak-260923-k7-restore` 两件覆盖回位；③ 固件回滚 = 重烧 K6 版 app。三档互相独立。

## 2. 冷启动实证（目标场景，日志 = `device-coldstart-260923-1357.log`）

```
13:57:39.847  WS: Connecting to websocket server: wss://8.137.91.16:8000 with version: 1
13:57:40.933  MCP: display_dispatch view=schedule corr_id=kdp-9032 courses=37 degrade=false
13:57:40.966  MCP: display_dispatch view=home     corr_id=kdp-9032 upcoming_apply ok
13:57:41.918  MCP: schedule_grid_restore corr_id=kdp-9032 courses=37 cached=37
```

- **K7-A8②（本包目标）**：冷启动后周网格 `courses=37` 已入 RAM 缓存（`cached=37`）——翻 P2 即见（S-F1.2 AC1 冷启动成立）。
- **K7-A8①**：P1「接下来」`upcoming_apply ok`（K6 锚① 行为零回归）。
- **restore 语义**：全程**零 `render page` 接管日志、零 SetChatMessage、零 error/warn**——只填缓存不接管前台（契约 §1.3② + ADR-005 语义实证）。
- 双 corr_id（kdp-9032 / kdp-9042）双推 = 设备侧两次连接事件（front 重启后重连 + 冷启动建连），两侧各自完整走通，幂等（缓存覆盖写）。

## 3. C1 closeout 状态（截至本次）

| C1 要件 | 状态 |
|---|---|
| ① 开关实现 + 默认关交付 | ✅ 已满足（代码默认关偏置 + K7-A3 齿） |
| ② 开闸留痕 | ✅ 本次：drop-in 路径 + env 在位 + 服务重启记录（本文件 §1#4） |
| ③ 旧固件行为复核实测 | ⚠️ **未做，如实记**：开闸前已完成固件先行（#1），旧固件+开关开窗口从未存在——条件保护意图（防确定性接管回归窗口）由时序满足；**字面要求的实测已不可复现**（设备已烧新固件）。系统性替代证据 = 仓内代码定案（`schedule_page.c:607-613` + `mcp_server.cc:620-644`）+ host 齿 M4（对端单源 face）。**降级记录，非静默豁免**——如需字面闭环须回烧 K6 固件重做，human 可裁。 |
| ④ 烧板验证后开闸记录 | ✅ 本次：固件烧录（Hash verified + 启动稳定）→ 部署 → 开闸时序留痕（本文件） |

## 4. 真机锚 K7-A8 逐项

| 子锚 | 状态 |
|---|---|
| ① 冷启动 P1 恢复 + restore 接收日志 | ✅ 实证（§2） |
| ② 冷启动直翻 P2 网格有内容 | ✅ 日志侧证（cached=37）；**屏幕目视 = human 待确认**（P2 翻页目测） |
| ③ 语音「我的课表」接管照旧（开关开/关两态） | ⏳ human 在场试（开关开态）；关态归后续 |
| ④ 旧固件行为复核 | ⚠️ 见 §3③ |