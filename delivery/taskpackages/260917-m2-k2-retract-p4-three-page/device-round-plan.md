# M2-K2 真机验证轮 · 测试步骤与验证点（planner 备，human 持板）

> 目的：合并门（supervisor 机械闸裁 ESCALATE，管辖依据是本包声明的 high 风险档）前先做一轮 human 持板目击。
> 若下方主验 1~7 全 PASS，本目击**就地兑现 M2K2-A5**（原 deferred 给 K3 设备轮的「3 页循环手感 + 页点 3 点目击」），随后再走合并人批。
> 产物指纹（烧之前核一眼）：`firmware/build/xiaozhi.bin` = **3,192,560 bytes / 2026-09-18 22:42 / sha256 前缀 `af61e6fc…`**。已复跑增量构建确认未触发重编 ⇒ 该 bin ≡ 当前 HEAD 源码（实现 commit `3109717`）。

## A. 烧录

端口 = **COM5**（当前无进程占用，可直接烧）。**app 偏移 = `0x20000`**（本板分区表 `partitions/v2/16m.csv` 无 factory 分区，app 在 ota_0）。

① 仅烧 app（快，保 NVS 配置——NVS 分区不在烧录清单内）：

```bash
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" -m esptool --chip esp32s3 --port COM5 --baud 460800 write_flash 0x20000 "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/build/xiaozhi.bin"
```

② 或全量（bootloader/分区表/ota_data/art/assets 一并按表落位）：

```bash
cd "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware"
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py -p COM5 flash
```

⚠️ **别用 `0x10000`**：本板无 factory 分区，烧错偏移 = 镜像整体右移、头部缺失 → 约 7 秒周期复位循环 + 黑屏 + Windows 反复播 USB 接入音（串口还常报 `ClearCommError PermissionError(13)`）。出现这组症状**先怀疑偏移，别怀疑代码**。

## B. 启动确认（屏幕亮了可跳过）

若串口不配合，用追端口捕获 30 秒：`"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/serial_catch_tight.py 30 c:/tmp/boot-check.log`
判据 = **0 个 `[[RE-ENUM]]`** + 有 RX 字节 + uptime 时间戳平滑推进。⚠️ 该脚本端口稳定后不会自退且锁死 COM5，用完必须 kill（`tasklist //FI "IMAGENAME eq python.exe"` 核对）。

## C. 验证点（主验 1~7；FAIL 就给一句现场描述）

| # | 验证点 | 期望 | 备注 |
|---|---|---|---|
| 1 | **BOOT 三页循环** | 短按 BOOT：首页 → 课表页 → 故事页 → 回首页；**不再出现「查生字」页**（撤页前第 2 跳是它）；连按 4 下回到起点而不是转到第 4 页 | 本包主验（A5 撤页半） |
| 2 | **页点 3 点** | 底部键帽条中槽 **3 个点**（此前 4 个）；当前页的点实心放大并随翻页移动（首页=1 / 课表=2 / 故事=3）；**无第 4 点** | 本包主验（A5 页点半） |
| 3 | 左右槽文案随页 | 首页「说话 / 翻页」· 课表「问课程 / 翻页」· 故事「说话·双击看 / 下移」 | 本包未改，作对照 |
| 4 | **查字能力仍在（撤页不拆通路）** | 按 KEY 说「勇怎么写」→ 查字内容仍上屏（米字格 + 笔顺逐笔 + 右栏拼音/共 N 画/部首/组词） | **本包最关键的「没拆错」检查**；依赖 live 链路（设备在线 + 引擎 + OpenClaw），链路不通则标 inconclusive、不阻塞 |
| 5 | 覆盖层 BOOT 隔离 | 查字内容在屏上时按 BOOT → **关掉它回原页、不翻页**（关之前在哪页就回哪页） | v1.39 语义，撤页后不得回退 |
| 6 | KEY 长按回首页 | 任意页长按 KEY → 回首页 | 回归 |
| 7 | 首页常显面正常 | 时间/日期/温湿度/电量辅助条 + 小伴演出照常 | 撤页不该碰首页 |

可选旁证 8（方便就做）：提醒弹窗 —— 弹出压过一切、KEY 关闭回当前页、BOOT 关弹窗回原页不翻页（K4 语义，本包不该碰）。

## D. 记录口径

- 逐点报 PASS / FAIL / inconclusive；FAIL 附一句实际现象。
- **全 PASS** → 我落一条真机目击记录（`device-round-1.md` 同域形态）进本包证据面 + pod-events，A5 由 deferred 转达成；随后仍请你一句合并人批凭据（M2-K2 riskTier=high 的既定人批门）。
- 任一 FAIL → 我据现场描述判「退 fixer 修」还是「转 K3 设备轮」，并如实登记，不硬合。
- 想要串口证据：被动捕获 `c:/tmp/serial_passive.py`（一进程独占 COM5，开之前先 kill 追端口捕获）。

---

## E. 现场状态与接续（2026-09-18 夜 · planner 记；真机轮推迟到 2026-09-19）

**已发生**：固件**已烧**（app-only、偏移 `0x20000`、保 NVS）——`Wrote 3192560 bytes at 0x00020000` + **`Hash of data verified.`** + `Hard resetting via RTS pin`。启动抓包 30 秒：**0 个 `[[RE-ENUM]]`**、18124 字节、uptime 平滑推至 108 秒 ⇒ 无偏移伪影、无复位循环。日志留存 `c:/tmp/boot-check.log`。

**卡点（明天第一件事）**：设备有存好的 WiFi 凭据但**扫不到那个 AP**，60 秒后自行进配网——
```
I (39383) WifiStation: No AP found, next scan in 40 seconds
I (62123) WifiStation: Stopping WiFi station
I (62143) kidpal_rn: begin_provision: PROVISION push (top=6)
```
配网层在最顶（top=6）⇒ 屏幕是配网页，本地页被盖住；**连上网才会自动退**。所以先解决联网，再做下面清单。
① 最快 = 把它记得的那个 AP 开起来（若是手机热点就开一下）→ 复位板子 → 自行连上回首页；
② 或重配 = 手机连板子开的热点（AP 名/密码显示在配网页上）→ 浏览器 `192.168.4.1` → 选 WiFi 填密码。

**联网后**：先抓 30 秒日志确认 `WifiStation` 连上 + websocket 通，**验证点 4/5（查字通路 + 覆盖层 BOOT 隔离）才有意义**；验证点 1/2/3/6/7 离线即可，退出配网页就能看。

**合并门状态**：本包已由 human 于 2026-09-18 批合并（ESCALATE 人批凭据 pod-events `f06ccad5-7651-49af-94a0-2ce05220d1f8`），合并不等真机轮；**真机结果若异常 → 走后续修复批（本包已合并，不回滚先修）**。真机目击若 PASS，则补记为 M2K2-A5 的落地证据（原 deferred 给 K3 设备轮）。