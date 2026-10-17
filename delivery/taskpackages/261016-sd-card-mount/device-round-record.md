# device-round-record — 261016-sd-card-mount（TF 卡接入 · 真机轮 SD-A5 / SD-A6）

> integrator 归档 · 2026-10-16 · 分支 `dev/261016-sd-card-mount`（base = main `bd4beac`）。
> 条目：**SD-A5**（真机有卡轮，**已执行 PASS**）+ **SD-A6**（真机无卡轮，**本机不可执行豁免 + 结构性证据替代**），两条 `kind:"structural"`（human 持板 device-round，live-manual 一等豁免通道，先例 260911-key-route-a1 KR-A5 / 261015-p1-auxbar-style-fix AB-A7）。
> 判定依据（pod-events）：SD-A5 PASS = human-decision `0263a7a3`；SD-A6 豁免 = human-decision `416e5c42`（human 原话「不好取，不取了，以后也不会拿出来」）。
> 执行模式：主线程代跑真机轮（headless 停等条款），烧录 `firmware/build/xiaozhi.bin` 至 app 分区 `0x20000`，`t=0` 复位捕获串口日志 `C:/tmp/sd-boot-a5b.log`（本目录副本 `device-round-sd-a5-boot.log`）。

## 1. 烧录事实

| 项 | 值 |
|---|---|
| 分区 | app @ `0x20000`（**只烧 app 分区**——本包无 spiffs_root 资产变更，资产双分区烧录规则不适用）|
| 载荷 | `firmware/build/xiaozhi.bin`（本包设备构建产物，final `firmware-build` 闸真编译 `build-pass`，二进制 `0x31e9f0` = 3275248 字节）|
| hash | verified（回读校验通过）|
| NVS | 保留（未擦除）|
| 复位 | `t=0` 复位（日志源头 `ESP-ROM:esp32s3-20210327` + `rst:0x15 (USB_UART_CHIP_RESET)`）|

## 2. 日志 ↔ 观察互证表（SD-A5 有卡轮）

| # | 观察项 | 串口日志证据（逐字引用 + 行号） |
|---|---|---|
| 1 | 上电（**插卡**）启动日志见 SD 卡信息打印 | `Name: asdfg`（L75）/ `Type: SDHC`（L76）/ `Speed: 20.00 MHz (limit: 20.00 MHz)`（L77）/ `CSD: ver=2, sector_size=512, capacity=30535680 read_bl_len=9`（L79）——`sdmmc_card_print_info(stdout, card)` 输出（printf 裸行，无 ESP_LOG 前缀）；旁证 `SSR: bus_width=1`（L80）= 1-bit 模式实证 |
| 2 | 挂载成功 | `I (1262) waveshare_rlcd_4_2: SD card mounted at /sdcard (SDMMC 1-bit)`（L81） |
| 3 | 经板级入口完成写-读-删 roundtrip（读回与写入一致、验后即删不驻留） | `I (1272) waveshare_rlcd_4_2: SD ROUNDTRIP PASS path=/sdcard/SDTEST.TXT corr_id=kdp-1262 bytes=63 (write-read-compare-delete ok, 验后即删不驻留)`（L82）——探针写 63 字节固定已知序列（`KIDPAL-SD-ROUNDTRIP-PROBE-...`，`sizeof-1=63`）→ 读回逐字节比对一致 → `remove` 删除不驻留 |
| 4 | 降级不崩（有卡路径正向对照：挂载失败才走降级，此处走成功路径，启动不受阻） | L81/L82 之后正常续行：L83 `kidpal_rn: ... initialized` → L182 `Application: Network connected` → L190 `StateMachine: State: activating -> idle` → L228 `WS: Session ID: def522d2-...`（语音主链路正常起来）|

> **SD-A5 判定 = PASS**：观察项 ①②③ 逐项真机兑现，日志逐行互证。观察项② 真机兑现行 = `SD ROUNDTRIP PASS ... corr_id=kdp-1262 bytes=63`（探针在真机的首轮通过，无 FAIL 行）。

## 3. SD-A6（真机无卡轮）—— 本机不可执行豁免 + 结构性证据替代

- **豁免事由**（human 裁定 `416e5c42`）：卡永久在位、无卡态在本机不可达（human 原话「不好取，不取了，以后也不会拿出来」）→ 真机无卡实测本机不可得。
- **替代证据口径**（**代码面静态结构断言，非运行时真机观察——明确不冒充真机观察**）：
  - ① 降级分支 `ESP_ERROR_CHECK_WITHOUT_ABORT` 不 panic 的代码面断言，由 **SD-A3** 承载（冻结件 `sd_card_mount.acceptance.c` test 3）：`WITHOUT_ABORT` 在世 + 裸 `ESP_ERROR_CHECK(` panic 形态计数 = 0 负向齿 + `s_sd_mounted` 与 `false` 同行降级置假 + `ESP_LOGW` WARN 继续不静默。
  - ② 板级查询入口读标志观察锚，由 **SD-A4** 承载（test 4：`SdCardMounted` 定义体读 `s_sd_mounted`）。
- **覆盖映射与差集**：原三观察项中「正常启动进主界面（降级不崩、不阻断启动）」由 ① panic 负向齿静态担保；「启动日志见降级 WARN」由 SD-A3 `ESP_LOGW` 结构齿担保；「**KEY 发起会话冒烟（语音主链路不受影响）**」**本机不可得、无结构齿承载**——如实列为覆盖面差集（去向 = `review-packet.md` 未覆盖面申报）。
- 真机无卡实测本机不可得，本项**不表述为真机已验证**、不落 C 齿。

## 4. 原理图三根脚目视核对（原 SD-A5 观察项③ 修订）

- 原观察项③ = 原理图三根脚（CLK38 / CMD21 / D039）目视核对、销 GPIO21 历史疑问——**human 未单独执行**（human-decision `0263a7a3` scope 明记）。
- 修正形态 = 「config.h 宏取值 + 真机挂载成功」实证反证：`config.h:43-45` 宏取 `SD_MMC_CLK_GPIO = GPIO_NUM_38` / `SD_MMC_CMD_GPIO = GPIO_NUM_21` / `SD_MMC_D0_GPIO = GPIO_NUM_39`；真机 mount 成功（L81）即三脚接线正确（三脚任一错接会走降级 WARN 分支，日志无 WARN、有挂载成功行）→ GPIO21 历史冲突疑问销账。
- **如实声明：原理图目视核对未单独执行**，本项以实证反证形态承载，非目视核对。

## 5. 证据件归档

- `device-round-sd-a5-boot.log`（本目录）：真机串口捕获全量（**249 行 / 14653 字节**），自 `C:/tmp/sd-boot-a5b.log` **原样字节拷贝**（`cp`，未做 EOL 归一；sha256 双侧一致 = `def36b1f1b165e3be026efa3214b8d3638d2702ef1ebd88f58bbd9588cd2043a`）。

> 归档人 = integrator（本相位）。本文件是 SD-A5 / SD-A6 真机轮的承重 human 证据叙事；判绿依据 = pod-events `0263a7a3`（SD-A5 PASS）+ `416e5c42`（SD-A6 豁免）+ 上述日志逐项互证。
