# device-round-record · 260915-m1-k2-keybar-three-slot（KB-A5 真机确认轮）

> KB-A5 = structural 验收（human 持板）。本记录如实分列各轮：时间线、现象、根因、修法、human 裁定、证据引用。**KB-A5 最终裁定 = human 四项全 PASS（2026-09-16）**：① 三槽形态 ② 页点显示与翻页跟随 ③ 覆盖层隐藏页点 ④ 随态文案。终态固件已烧录并稳定运行（Hash verified + 30s 启动稳定）。
> 证据文件（仓外，c:/tmp/）：`k2-bootloop.log` / `k2-bootloop2.log`（复位循环捕获）、`k2-diag.log` / `k2-diag2.log` / `k2-diag3.log`（K2DIAG 仪器化日志）、`k2-device-round*.log`（被动串口捕获）。文中「摘录」= 逐字引日志行；未引处 = 轮操作记录转写，已注明。

## 时间线与各轮（2026-09-16）

### 第 1 轮 — FAIL（烧录偏移伪影 → 复位循环）

- **时间锚**：17:27–17:32（k2-device-round.log 空捕获 0 字节；k2-bootloop.log 17:30 / k2-bootloop2.log 17:32）。
- **现象**：烧录后设备约 7 秒周期复位循环、屏幕全黑、Windows 持续播 USB 接入音（USB-Serial/JTAG 每次复位重枚举）；被动捕获抓不到业务字节。摘录（k2-bootloop.log，全文 16 行同款）：
  ```
  [[RE-ENUM]]
  ```
- **根因（bisect 3 轮实证）**：烧录偏移用了 ESP32 惯例的 `0x10000`，本板分区表 `firmware/partitions/v2/16m.csv`（sdkconfig `CONFIG_PARTITION_TABLE_CUSTOM_FILENAME` 实指）**无 factory 分区**，app 在 **ota_0 @ 0x20000**——镜像整体右移、头部缺失，bootloader 校验失败。bisect 3 轮隔离变量：同段代码错误偏移必循环、正确偏移启动稳定 → **偏移是唯一变量，代码无罪**。无 factory 表下 `0x10000` 落在 bootloader/分区表区域之后的空隙，属惯例猜测伪影，非本包改动引入。
- **修法**：按分区表实偏移 `0x20000` 重烧（全量走 `idf_run.py flash`；仅 app 保 NVS 走 esptool `write_flash 0x20000 build/xiaozhi.bin`）。判定法与症状表沉淀进 `.claude/skills/kidpal-flash-device/SKILL.md`（本包 launch 外新增件，随本包 commit）。

### 第 2 轮 — FAIL（左右槽无字 → 仪器化定位字体根因）

- **时间锚**：18:04–19:45（k2-device-round2/3.log 被动捕获；k2-diag.log 19:39 / k2-diag2.log 19:45 仪器化轮）。
- **现象**：设备正常启动、形态/页点/覆盖层行为可见，但**左右槽无字**（区域空白）。
- **仪器化（K2DIAG 日志）实证逻辑链路全通**。摘录（k2-diag3.log:15-22，首页基层；1288085 行起覆盖层）：
  ```
  I (1272955) LcdDisplay: K2DIAG sync entry=page-render
  I (1272955) LcdDisplay: K2DIAG sync page=0 rn_page=1 overlay_top=0 L=1 R=1 box=1
  I (1272965) LcdDisplay: K2DIAG slots rc=0 scene=0 left=说话 right=翻页
  I (1272965) LcdDisplay: K2DIAG dots rc=0 vis=1 total=4 active=3 box=1
  ...
  I (1288085) LcdDisplay: K2DIAG sync entry=front-bridge voice_front=1
  I (1288085) LcdDisplay: K2DIAG sync page=0 rn_page=0 overlay_top=1 L=1 R=1 box=1
  I (1288085) LcdDisplay: K2DIAG slots rc=0 scene=0 left=说完啦 right=结束
  I (1288095) LcdDisplay: K2DIAG dots rc=0 vis=0 total=4 active=0 box=1
  ```
  即：sync 被驱动（page-render / front-bridge / apply-look 三入口全触发）、slots 返回 rc=0 且值正确（覆盖层随态 left=说完啦/打断、right=结束；主页面 left=说话/问课程、right=翻页）、页点 vis 主页面=1 覆盖层=0——**断点在字体渲染之后的显示层**，逻辑层（keybar 单源 + layer_top 同源）无罪。
- **根因**：板级字体 `font_puhui_basic_30_4` 为稀疏子集（787 字形），缺键帽用字（说/话/翻/页/课/程等）——字形查不到即渲染空白。
- **修法**：左右槽改挂全量 `font_puhui_14_1`（14px 普惠体，键帽文案字形全覆盖；`lcd_display.cc` LV_FONT_DECLARE 处留根因注释）。

### 第 3 轮 — 修复后复确认 → human 四项全 PASS

- **时间锚**：20:12–20:33（k2-diag3.log 20:12 终验；k2-device-round4.log 20:33 被动捕获）。
- **现象**：三槽形态、页点（主页面 4 点、当前页实心放大、随翻页移动、覆盖层内隐藏）、随态文案（聆听「说完啦」/播报「打断」/提醒「知道了」域）真机全部可见正确。
- **human 裁定（2026-09-16）**：KB-A5 四项——三槽形态 / 页点 / 覆盖层隐藏 / 随态文案——**全 PASS**。
- **终态烧录**：修复版固件已烧录，Hash verified + 30s 启动稳定（轮操作记录转写，串口捕获文件内无 esptool 输出，不引伪日志行）。
- **字号 tradeoff（挂账 b）**：30px 全量字体超 ota 分区 gate（firmware-build 闸 check_sizes 拦截），本轮以 14px 全量体交付——14px 与 page body 同字号可读；专属子集字库归后续轮。

## 真机轮期间目击（挂账 c）

- 轮期间目击一次**计划外重启**（一次，未复现，无对应日志锚）；挂观察，不阻断本包收口，归后续轮复现跟踪。

## 遗留观察（挂账 a，owner = 260915-m1-k1a-key-layer-dispatch）

- BOOT 翻页序 P1→P4→P2→P3 = k1a 渲染节点 `KP_PAGE_*` 枚举序（`cycle_next` 线性遍历）所致；KB-E3 边界——keybar 只读显隐、零覆盖层/页序裁决权，本包未修，owner = key-layer-dispatch 复核。

## 证据清单

| 证据 | 路径 | 内容 |
|---|---|---|
| 复位循环捕获 | c:/tmp/k2-bootloop.log、k2-bootloop2.log | `[[RE-ENUM]]` 循环（第 1 轮伪影） |
| K2DIAG 仪器化日志 | c:/tmp/k2-diag.log、k2-diag2.log、k2-diag3.log | 逻辑链路全通 + 覆盖层显隐 + 随态值（第 2/3 轮） |
| 被动串口捕获 | c:/tmp/k2-device-round.log（空）~round4.log | 各轮运行期输出 |
| 沉淀 skill | .claude/skills/kidpal-flash-device/SKILL.md | 偏移规则 + 症状表 + A/B 判定法 |
