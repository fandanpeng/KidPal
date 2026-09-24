---
id: ARQ-260906-nvs-csv-wifi-injection
title: NVS CSV 直注 WiFi 凭据后设备连不上（根因未查，本包绕行走设备配网口）
domain: tech
status: deferred
severity: COULD
owner: dev-loop（下个配网相关包）
trigger: 下个涉及配网/NVS 直注的包
opened: 2026-09-06
closed: ""
closedBy: ""
paths: []
source: 260903-device-ws-switch 联调（2026-09-05，绕行走设备配网口）
---

**证据**（260903-device-ws-switch 联调，2026-09-05）：用 `nvs_flash_gen.py` 由 CSV 直生成 NVS bin（含 wifi ssid/password + websocket url/token/version）烧入后，设备连不上 WiFi；本包未追根因，绕行 = 走设备自己的配网口写 WiFi、NVS 只补 websocket 命名空间。疑似方向（未证）：CSV 的 wifi 命名空间键名/类型与固件网络栈实际读取口径不符，或与直写 websocket 命名空间的 namespace 声明/索引布局冲突（布局见 [[kidpal-nvs-ops]]）。**影响面**：NVS CSV 直注本是「免开配网口、批量烧录」的快路径，当前不可用即每次都得走设备配网。**建议改动**（候选）：下个涉及配网/NVS 的包顺手查根因（对照固件实际读取的命名空间/键名/类型），或确认废弃 CSV 直注路径并文档化「只走设备配网口」。**riskTier: low**。owner=dev-loop（下个配网相关包）。
