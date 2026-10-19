---
changeId: 261018-sd-log-usb-read
title: 诊断日志按需 USB 串口取回（只读命令，不自动 dump、不上云）
rigor: lite
riskTier: medium
sourceType: human-ad-hoc
reviewed-by: architect
date: 2026-10-18
---

# plan · 诊断日志按需 USB 串口取回

> 来源：human 2026-10-18 原话逐字——「我的想法是没有必要让它自动输出。我们需要的时候，只要有一个地方能输入命令或者通过其他方式，把日志拿出来就行了。自动输入出来，我不知道会不会影响我们将来的串口监控，或者是烧程序这些。也没必要其实，每次让它自动输入，我插的时候也是通过 Cloud Code 让它去查的，我不自己去看呀。」
> 上游依赖 = `261018-sd-log-persist`（日志落卡，实现完成、十闸绿、supervisor ESCALATE 停 human 合并门）。reviewed plan 由 architect 于 `/triage-change` 比例化 review 产出（2026-10-18）；主线程只搬运、不改内容。

## 目标与边界

**目标**：设备插 USB（COM5，原生 USB-Serial/JTAG）后，由 Claude Code 调 host 脚本按需取回 `/sdcard/logs/KPLOGnnn.TXT` 内容：列目录、看最新文件尾部、读指定文件全文。**不自动输出、不上云、不改正常日志流**；拔卡读仍可用。

**边界（v1 明确不做）**：
- 不做自动 dump / 开机打印 / 周期上报；不新增提示音、屏幕提示或日志行。
- 不做写、删、格式化、改文件；不做任意路径读取；不做云端/MCP 取回。
- 不做 USB MSC（与 USB-Serial/JTAG 争 USB 控制器）。
- 不做 UART0 输入通道（COM5 实测为原生 USB-Serial/JTAG，UART0 未引到 Type-C）。
- 不改分区表、sdkconfig、决策 24 白名单、§6.6 canonical 文本。

## 设计要点

1. **命令面（只读，索引白名单）**：行协议，仅识别 `kplog ` 前缀；其余输入静默丢弃（防串口噪声触发输出）。
   - `kplog list` → 逐行 `KPLOGnnn.TXT <bytes>`（存在者）+ `KPLOG TOTAL <bytes> <n> files`；当前写入文件行尾标 `*`。
   - `kplog tail [N]` → 当前写入文件末 N 行，默认 200、上限 1000。
   - `kplog cat <idx>` → 指定文件（0..31）全文，1KB 块流式输出。
   - 统一帧：`=== KPLOG BEGIN <cmd> ===` … `=== KPLOG END ===`；错误也走帧（`KPLOG ERROR no-card|no-file|bad-cmd|bad-index`），host 解析统一。
2. **端口读取（实测可行）**：COM5 枚举为 `USB VID:PID=303A:1001`（Espressif 原生 USB JTAG/serial debug unit，非桥片）。IDF 5.5.1 在 `CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG=y` 下已注册 `/dev/secondary` VFS（含 read ops），应用 `open("/dev/secondary", O_RDONLY|O_NONBLOCK)` 后 `read()` 非阻塞取字节即可，**无需 `usb_serial_jtag_driver_install`、无需改 sdkconfig、无 console REPL 冲突**。注意：Kconfig 帮助文本说 secondary「仅输出」指的是 stdio/REPL 输入；直开 VFS 读是支持的。
3. **输出走 stdout，不走 ESP_LOG**：响应用 `fwrite`/`printf` 到 stdout（console 同时写 UART0 + USB-Serial/JTAG）。`ESP_LOG` 会被 sd_log 的 vprintf 钩子再收进环形缓冲，造成自污染与递归风险——读任务函数体内零 `ESP_LOG`（沿用写盘任务纪律）。
4. **读盘并发**：FATFS `CONFIG_FATFS_FS_LOCK=0`（sdkconfig:1733）允许同文件重复打开；读任务只以 `rb` 打开、一次一个 FD、读完即关，与写盘任务单写者纪律不冲突（写方 `fflush` 后为完整记录）。`max_files=8` 预算够用。
5. **看「最新文件」需要当前轮转索引**：在 `sd_log_fs.cc` 增加只读访问器 `int KidpalSdLogCurrentIndex(void)`（未 init/未 attach 返回 -1），供 `tail` 与 `list` 的 `*` 标记使用；不靠 mtime（FAT 时间粒度 2s、RTC 未同步时会退化）。
6. **纯逻辑/芯片面分离**：解析、行装配、帧格式化、尾部窗口选取 = `diagnostics/log_cmd.c`（零 FATFS/零 FreeRTOS，host 同源）；VFS 读、任务、文件 I/O、stdout 输出 = `diagnostics/log_cmd_port.cc`（不进 host 构建）。
7. **任务形态**：`main.cc` 在 vprintf 钩子之后 `KidpalLogCmdStart()`；低优先级（prio 2，写盘任务为 1）、栈 4KB、50–100ms 轮询读 64B 块；无命令零输出。无卡/未挂载时只回错误帧，不崩、不阻断语音主链路。
8. **host 侧脚本** `tools/diag/kidpal_logs.py`（用 IDF python env 的 pyserial）：`--port COM5 --cmd list|tail|cat --n N --index i --out <file> --timeout`；打开端口后立即 `dtr=False/rts=False`（防复位伪影，遵循 `kidpal-serial-monitor` 纪律），写一条命令 + `\n`，丢弃 BEGIN 前的普通日志，收集到 END 为止，按字节落本地文件（UTF-8，errors="replace"）。单进程独占 COM5（与监控/烧录互斥，跑前关其它串口工具）。
9. **打印/烧录不回归**：读任务只在有命令时输出；esptool/串口监控链路零改动。

## changeManifest 种子清单

- 新增 `firmware/main/diagnostics/log_cmd.h`（纯逻辑 + 命令启动声明）
- 新增 `firmware/main/diagnostics/log_cmd.c`（解析/行装配/帧格式化/尾部窗口，纯逻辑 host 同源）
- 新增 `firmware/main/diagnostics/log_cmd_port.cc`（`/dev/secondary` 读任务 + FATFS 只读 + stdout 输出，芯片面）
- 改 `firmware/main/diagnostics/sd_log.h`（声明 `KidpalSdLogCurrentIndex`）
- 改 `firmware/main/diagnostics/sd_log_fs.cc`（实现当前轮转索引访问器，只读）
- 改 `firmware/main/main.cc`（钩子之后启动命令任务）
- 改 `firmware/main/CMakeLists.txt`（SOURCES 显式补 `diagnostics/log_cmd.c` / `diagnostics/log_cmd_port.cc`——main 目录非 GLOB，必须手工登记）
- 改 `firmware/main/test/host_app/main/CMakeLists.txt`（HOST_ACCEPTANCE_SRCS 登记纯逻辑与验收件）
- 新增 `firmware/main/test/log_cmd.acceptance.c`（host 冻结验收）
- 新增 `tools/diag/kidpal_logs.py`（host 取回脚本）

> 预评注记：`sd_log.h` / `sd_log_fs.cc` 属上游 persist 包的已提交文件，本包只加只读访问器、不动其行为与冻结验收；两处 CMakeLists 登记是比例化预评补入的缺口（沿用 persist 包教训）。

## 验收计划

**host 齿（`log_cmd.acceptance.c`，纯逻辑 + 结构扫描）**：
- LUR-A1 解析：`list` / `tail` / `tail N` / `cat N` 合法形态；默认 N=200、上限 1000、idx 闭集 0..31。
- LUR-A2 拒绝：idx ≥32 / 负数 / 非数字 / 路径字符（`../`、`/`、`.`）/ 多余 token / 未知子命令 / 超长行 → `bad-index|bad-cmd` 且不产生任何文件访问。
- LUR-A3 行装配：跨读块边界、CRLF/LF、超长行丢弃、非 `kplog` 前缀静默不输出（不自动 dump 的行为齿）。
- LUR-A4 帧格式：BEGIN/END 精确字面；list 行格式（名字 + 字节数 + 当前标记）；错误帧格式。
- LUR-A5 尾部选取：样本字节流取末 N 行（N=1 / N>行数 / CRLF / 无结尾换行）边界正确。
- LUR-A6 结构齿：文件名只经 `kidpal_sd_log_file_name` 或受校验整数构造（禁用户串直进路径）；端口只 open `/dev/secondary`；启动函数零打印；读任务零 `ESP_LOG`，响应只经 stdout。

**真机轮（live-manual，human 持板）**：
1. 烧录 persist + 本包固件；插 USB 后 COM5 除正常日志外**静默无自动输出**；不插线时正常启动。
2. `list` → 文件名 + 大小 + 总量；当前文件有 `*`。
3. `tail 50` → 末 50 行、含当轮 STT/LLM 原文片段、帧完整。
4. `cat 0` → 全文逐字节正确（中文 UTF-8 不乱码）。
5. 产生新日志后再 `list` / `tail` → 大小增长、tail 含新行（读不干扰写）。
6. 非法命令（`cat 99`、`tail abc`、乱敲前缀后）→ 错误帧、不崩、不输出多余内容。
7. 拔卡 → `no-card` 错误帧；设备不崩、语音主链路不受影响；插回重启后恢复。
8. 关脚本后 esptool 正常烧录；串口监控正常（与既有链路零回归）。

## 风险与降级

- **`/dev/secondary` 直读未达（首要技术风险）**：Kconfig 帮助文本提示 secondary 主要用于输出。降级 A = HAL 直读 `usb_serial_jtag_ll_read_rxfifo`（同外设、非阻塞轮询，不装 driver）；降级 B = console primary 切 USB-SERIAL-JTAG（改动大、触 sdkconfig 与全链路，须回 `/triage-change` 升档重判）。**真机轮第 1 步前先做一次单命令 smoke 验证本条**。
- **并发读盘**：只读打开、一次一 FD、失败只回错误帧；不 rename/remove，避免 FATFS 非法操作。
- **输出阻塞/长文**：cat 按 1KB 块 `fwrite` + `fflush`，host 超时兜底；任务低优先级，不阻塞音频/LVGL。
- **隐私**：只读白名单文件；不新增数据类、不出网；输出帧不进 sd_log 环形缓冲（保持「不改正常日志流」）。威胁模型与 `TECH-261016-sd-story-confidentiality` 同族且未加宽，不要求新记账。
- **复位伪影**：host 脚本打开端口立即拉低 DTR/RTS；判据遵循 `kidpal-serial-monitor` skill（与启停秒级吻合 = 伪影）。
- **单进程独占**：脚本与串口监控/烧录不能同时持 COM5；脚本报错即提示关闭其它占用。

## 依赖 / 前置

- **硬依赖**：`261018-sd-log-persist` 的代码（`sd_log_fs.cc` / `sd_log.h` / `main.cc` 接线）必须已在 launch 基线内。该包当前未合 main（supervisor ESCALATE 停 human；真机轮未跑）。
- **基线建议（OQ-1）**：persist 真机轮通过 → human 批准合并 persist → 本包从更新后的 main rebase/起 Pod → 立即合并（两笔干净合并，仍满足 human「取回做好一起合」的意图）。若 human 坚持一次合成合并，planner 须在合并前验证机械闸对合并 diff 的凭据覆盖（两包 PROPOSAL/verdict 证据都在场），否则闸会 ESCALATE。
- 契约面 / 需求面 / 治理面零依赖；技术前置（SD 挂载、日志落卡、原生 USB 串口）已就绪。

## 档位裁定（architect 2026-10-18）

- **②单包 Lite**，rigor=lite、riskTier=medium、sourceType=human-ad-hoc。
- 非 ②Full：不新增数据类/网络面/写面，物理 USB 访问与拔卡同威胁模型，security 轴不成立；无状态机/DB/AI 成本。
- 非 nano：多文件 + 新行为验收语义；非 ③：单切片单 Pod。
- **lite 判注**：host 齿可闭环；设备编译面深（VFS 读 / FATFS / 板级），**快路径不适用、按 full 环境备**。
