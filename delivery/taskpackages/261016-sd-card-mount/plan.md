---
changeId: 261016-sd-card-mount
title: TF 卡接入——SDMMC 1-bit 挂载 + /sdcard 存储访问能力就绪（降级不崩）
rigor: full
riskTier: medium
sourceType: human-ad-hoc
reviewed-by: architect (/triage-change 分流 review)
date: 2026-10-16
---

# TF 卡接入（能力就绪包）

> 来源：human 原话（2026-10-16）：「走triage，那这次的 Scope 只是把 TF 卡接进来，至于 story
> 怎么存、将来日志怎么存，这不是这个包要做的。」Scope 已收窄 = **只做 SDMMC 挂载 + SD 存储
> 访问能力就绪**；story 存储、日志落盘均不在本包（将来另包，且须先过决策 24 介质禁令的
> spec 裁决）。本包不改任何用户可见行为（UIUX L0）。

## 建什么

设备具备「卡在位时 `/sdcard` 可读写」的基础能力：启动期 SDMMC 1-bit 挂载，成功置标志供
将来消费方查询；卡不在位 / 坏卡 / 挂载失败 → 降级标志 + WARN 日志、继续跑，不 panic、
不碰语音主链路。热插拔检测第一版不做。

## 方案

### 1. config.h 引脚宏（firmware/main/boards/waveshare-s3-rlcd-4.2/config.h）

新增（注释标注本包号 + 来源 = 官方例程 sdcard_bsp.h:15 默认脚位，只读参考重写）：

- `SD_MMC_CLK_GPIO  GPIO_NUM_38`
- `SD_MMC_CMD_GPIO  GPIO_NUM_21`
- `SD_MMC_D0_GPIO   GPIO_NUM_39`
- `SD_MMC_MOUNT_ROOT "/sdcard"`

triage 已实测：现役 config.h 音频(8/9/10/16/45/46)、I2C(13/14)、LCD(5/6/11/12/40/41)、
按键(0/18)、ADC(4) 与全 firmware/main 对 21/38/39 零占用，无冲突。

### 2. 板 .cc 挂载初始化与降级标志（waveshare-s3-rlcd-4.2.cc）

- 新增 `InitializeSdCard()`，挂进构造函数初始化序列（:836-841，InitializeArt 同排）：
  `esp_vfs_fat_sdmmc_mount(SD_MMC_MOUNT_ROOT, ...)`，`slot.width=1`、clk/cmd/d0 取上节宏；
  mount config `format_if_mount_failed=false`（坏卡/异格式不销毁数据、只报错降级——
  对偶 InitializeArt :805 既有范式）、`max_files=5`。
- 失败路径：`ESP_ERROR_CHECK_WITHOUT_ABORT` + 返回码/卡句柄判空 → 置板级标志
  （如 `s_sd_mounted=false`）+ `ESP_LOGW` 继续，**不 panic、不阻断启动**；成功 →
  `sdmmc_card_print_info` + 标志置真。
- 暴露板级查询入口（成员/静态 accessor，如 `SdCardMounted()`），供将来消费方判标志；
  本包自身零消费方。
- 参考实现 = `ESP32-S3-RLCD-4.2-Demo/02_ESP-IDF/06_SD_Card/components/port_bsp/sdcard_bsp.cpp`
  （只读参考，自有代码重写，不直接改参考树）。

### 3. sdkconfig 裁决：零改动（含 LFN 裁决）

- SOC_SDMMC_HOST_SUPPORTED / SOC_SDMMC_USE_GPIO_MATRIX 已 y；FATFS_VOLUME_COUNT=2 够用
  （assets/art 走 SPIFFS 不占 FAT 卷）。挂载本身零 sdkconfig 改动。
- **LFN 裁决：本包不开长文件名**（维持 CONFIG_FATFS_LFN_NONE）。能力就绪不预支消费方
  命名需求；验收限 8.3 命名。LFN / 中文文件名归首个真实消费包按其需求裁决（届时涉及
  codepage/内存取舍）。builder 若实测发现挂载缺配置项，走 planner 扩单通道回补，不私扩。
  （LFN 裁决经人类触点①确认 2026-10-16。）

### 4. 决策 24 介质禁令遵守（architecture.md :416）

本包不写任何业务/身份/事项状态数据；验收的写-读-删 roundtrip 文件是**临时测试探针**
（8.3 名，如 `/sdcard/SDTEST.TXT`），验后即删、不驻留。story/日志落板 = 将来包的
架构面决策（与决策 24 有张力，须先 spec 裁决），本包不预支、不开口子。

## 验收方式（方向；具体齿归 acceptance-author）

- **A1（真机·插卡）**：启动日志见 SD 卡信息打印 + 挂载成功；经板级入口完成
  写-读-删 roundtrip，读回内容与写入一致。
- **A2（真机·无卡）**：正常启动进主界面、降级 WARN 在日志；语音主链路不受影响
  （至少完成一次 KEY 发起会话冒烟或等效确认）。
- **A3（结构）**：设备 target 全量编译过（firmware-build 闸；本包无可 host 化齿，
  host_app 盖不住 SDMMC seam，快路径不适用）。
- 真机轮需 human 持板；建议顺带复核 SD 三根脚与原理图一致（记忆条目曾记 GPIO21
  冲突，现役 config.h 实测无冲突，真机信号面一次确认销账）。

## 随批 doc-sync 义务（事实态盘点同步，无设计决策变更；经人类触点①确认 2026-10-16）

- `docs/architecture.md:232` §5.3.1 盘点行「SD 槽固件未接入」→ 改记已接入
  （SDMMC 1-bit `/sdcard` 能力就绪；业务数据不落板，决策 24 不变）。
- `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:61-62` 同步（该行 KEY「未接入」
  亦已随 260911-key-route-a1 变 stale，可顺带一并修正）。

## 铁律约束与预定改动面

- 预定改动文件：`firmware/main/boards/waveshare-s3-rlcd-4.2/config.h`、
  `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`、
  `docs/architecture.md`（一行）、`docs/notes/board-hardware-esp32-s3-rlcd-4.2.md`。
- 不碰分区表（assets/art canary 不动）；本包无 spiffs_root 资产变更 → 只烧 app 分区，
  资产双分区烧录规则不适用。
- 参考树只读；烧录/真机轮按 headless 姿势留痕。
- sourceRefs 建议：`TEMP-2026-10-16-sd-card-mount`（human 原话）+
  参考实现锚点（只读）。
- gateProfile 建议：`["docs-sync", "test-quality", "evidence-integrity"]`。

## triage 裁决留痕（主线程记，2026-10-16）

- 四档裁决 = ② 单包 Full（rigor: full，riskTier: medium），rationale：形状够 Lite 但
  SDMMC 为设备专有编译单元、host_app target 盖不住（快路径不适用），触启动初始化序列，
  Full 留痕成本极低收益为正。
- 人类触点①（AskUserQuestion，2026-10-16）：三项全部确认 = ①确认 ②Full；②确认 LFN
  不开；③确认 architecture.md:232 随批 doc-sync、不升级架构面。
- 四面探测：需求/契约/debt/intake/治理面零触；架构面两处（盘点行 doc-sync 义务 +
  决策 24 介质禁令遵守，均非面变更）→ 实现面单面，不建 spine。
- 逃生门：无契约触发，不需 architect-reviewer 四分类。
