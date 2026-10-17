# 真机轮操作单 · 261016-p3-story-body-sd-cache（human 持板）

> 本包已收口到合并门前：verifier final r6 全绿、Review Packet 无未决 BLOCKING、`supervisor-verdict-gate.mjs` = **ESCALATE**（`riskTier=critical`，因 diff 命中 `firmware/sdkconfig` 关键路径）。按预期路径停在 human：**真机 live-manual 轮 + 合并放行**。

## 一、烧录

- 构建产物：本树 `firmware/build/xiaozhi.bin`（final r6 设备真编 build-pass，0x3217f0，分区余 9%）。
- 烧录 = app 分区单烧（`ota_0 @ 0x20000`，**不是 0x10000**）；烧录前确认 TF 卡在位、FAT32。
- 观测串口：COM5（一进程独占；先关其它串口工具）。

## 二、真机观察项（SC-A9，逐条记录结果）

| # | 观察项 | 判据 |
|---|---|---|
| 1 | **SD 读回 roundtrip** | 录一篇新故事归档后，`/sdcard/story/<id>.txt` 存在；重启设备后翻 P3 双击该篇仍能打开 |
| 2 | **双击三类记录进阅读** | 本机录的 / 重启前录的 / 云端恢复回来的老故事，双击都能进阅读态 |
| 3 | **离线双击已留存篇目** | 断网（或断 WS）后双击已留存篇 → 照常打开（NFR-OFF-1 窄例外边界） |
| 4 | **离线双击未留存篇 → WF-F2-06** | 断网双击未留存篇 → 整屏覆盖层：大字「这篇暂时打不开」+ 副句「等会儿再试试吧」+ 所选条目行；**底栏键帽槽 = 「知道了」/「关闭」**；**无页点**；10 秒无操作自动消失；KEY/BOOT 任一立即关闭回列表；全程不显示任何正文 |
| 5 | **id 错配回归位** | 双击「非本机录制的那一篇」→ 显示的是光标选中那篇，不是缓存残留篇 |
| 6 | **预取期语音不卡顿** | 列表停留（预取在跑）时发起对话 → 语音轮正常、无卡顿/丢字 |
| 7 | **LFN heap 水位（OQ-4 代价）** | `CONFIG_FATFS_LFN_HEAP` 生效；记录挂载后与写卡后的 internal / SPIRAM 可用堆读数 |

**观感项（非 SC-A9 自动面）**：覆盖层标题字号目前为 14px + 居中（「大字」因 host sdkconfig 未开 `LV_FONT_FMT_TXT_LARGE` + 设备内置 30px 字体缺字而 back-out）。若真机看屏觉得小，记一条观感问题交后续视觉批，不在本轮改。

## 三、真机轮之后（human 决定）

- **通过** → 落一条 pod-events `human-decision`（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 跑 `node .claude/scripts/merge-driver.mjs --pod 261016-p3-story-body-sd-cache --human-approval-ref <eventId>`（本地合并，可回滚）。
- **有问题** → 把观察项编号 + 现象交回，我再派 fixer（同 changeId 继续，不重开包）。
- 合并前复核未覆盖面（Review Packet）：隐私加密 deferred（TECH-261016）、缓存淘汰 / 光标预取 / 热插拔 = 非目标、标题大字 = 视觉批候选。
