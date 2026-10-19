---
changeId: 261018-sd-log-persist
title: 板端诊断日志落 TF 卡（有界轮转，含 STT/LLM 对话原文）
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect
date: 2026-10-18
---

# plan · 板端诊断日志落 TF 卡

> 来源：spine [delivery/intake/261018-sd-log-persist.md](../intake/261018-sd-log-persist.md)（human 2026-10-18 原话 + 两项拍板：含对话原文、常开 + 有界轮转）。
> reviewed plan 由 architect 于 `/triage-change` 比例化 review 产出（2026-10-18，触点①确认 ②单包 Full / riskTier=high）；主线程只搬运、未改内容。

## 目标与边界

**目标**：设备脱离串口独立运行时也能事后查真相（真机长尾：课表幻觉 TECH-261018、ASR 误听等）。采集 `esp_log` 输出（默认 INFO 及以上），经 PSRAM 环形缓冲 + 低优先级写盘任务落到 `/sdcard/logs/KPLOGnnn.TXT`，单文件 512KB × 32 个 ≈ 16MB 目录封顶、循环覆盖最旧。

**边界（v1 明确不做）**：panic/OOPS 输出不落卡（panic handler 绕开 vprintf，直写 UART）；bootloader 日志抓不到；运行中热插拔不做（写失败 30s 重试）；组件内 `printf` 不经钩子；不做手动诊断模式（常开 + 有界轮转）。

**human 已拍板**：① 包含对话原文（STT/LLM）；② 常开 + 有界轮转；③ 分区表与 sdkconfig 零改动。

## 设计要点

1. **采集**：`app_main` 最早期 `esp_log_set_vprintf()` 装钩子。**钩子必须链式调用原 vprintf**（保存旧函数指针、先转调再写环形缓冲），否则串口日志（COM5，arch §6.6 三日志源之一）会停。环形缓冲 128KB 落在 PSRAM（`heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`）。
2. **缓冲**：环形缓冲写入须 ISR-safe（ESP_LOG 可能来自任意任务乃至 ISR；用关中断/临界区或 lock-free ring）。写盘任务 1s 或 4KB 批量刷盘，**写盘任务内禁调 ESP_LOG**（防 vprintf 钩子递归回灌）。
3. **落盘**：`/sdcard/logs/KPLOGnnn.TXT`（8.3 名，与 story 批开的 LFN 无关、兼容任意 LFN 配置；sdkconfig 零改动成立）。单文件 512KB × 32 个循环覆盖最旧。
4. **接线**：
   - `firmware/main/main.cc:15` app_main 最早期装钩子 + 环形缓冲 init（早于 NVS init）。
   - `waveshare-s3-rlcd-4.2.cc` 挂载成功处（`:865-867` 之后、`RunBootRoundtripProbe()` 之前）调 `KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)`。
   - `max_files` 5→8（`:850`，给日志文件 FD + story_sd_io + 探针留头）。
5. **早启动到挂载间的留存语义**：钩子在 app_main 最早装、SD 在 Board 构造期才挂载——环形缓冲须从 app_main 起就累积（不清空），挂载成功后写盘任务从最旧条目开始 drain，使 NVS/WiFi 早期日志可落。
6. **纯逻辑/芯片面分离（建议，复用 story_cache 先例）**：环形缓冲满/轮转/文件名格式（KPLOG000→KPLOG031 wrap）为纯逻辑，抽成 host 可测 seam（如 `diagnostics/sd_log.c`）；`esp_log_set_vprintf` 钩子 + FATFS 写 + attach 为芯片面（`diagnostics/sd_log_fs.cc` 或板级接入）。

## changeManifest 种子清单

- 新增 `firmware/main/diagnostics/sd_log.h`（纯逻辑 + chip 面声明）
- 新增 `firmware/main/diagnostics/sd_log.c`（环形缓冲/轮转/文件名纯逻辑，host 同源）
- 新增 `firmware/main/diagnostics/sd_log_fs.cc`（vprintf 钩子 + FATFS 写任务 + attach，chip 面）
- 改 `firmware/main/main.cc`（app_main 早期装钩子）
- 改 `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（挂载成功处 attach；max_files 5→8）
- 改 `firmware/main/CMakeLists.txt`（SOURCES 显式清单补 `diagnostics/sd_log.c` / `diagnostics/sd_log_fs.cc`）——**注意：main 目录非 GLOB，必须手工登记**（见 findings F1）
- 改 `firmware/main/test/host_app/main/CMakeLists.txt`（host 齿登记 sd_log 纯逻辑）
- 新增 `firmware/main/test/sd_log.acceptance.c`（host 齿）

> 预评注记：`firmware/main/CMakeLists.txt` 与 host_app CMakeLists 两处登记不在原方案清单内，为 architect 补入的缺口。

## 验收计划

**host 齿（`sd_log.acceptance.c`，纯逻辑）**：
- 环形缓冲：满/溢出丢弃策略、多写者写入不撕裂、drain 后指针推进。
- 轮转：文件名 `KPLOG000.TXT`→`KPLOG031.TXT` wrap、512KB 分片边界、目录 32 文件封顶覆盖最旧。
- 文件名格式：8.3 合法（`KPLOG` + 3 位十进制 + `.TXT`）。
- 结构化齿（source 锚，仿 sd_card_mount.acceptance.c）：`esp_log_set_vprintf` 在 app_main 最早调用点、`KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)` 在挂载成功分支、`max_files=8` 字面。

**真机轮（live-manual，human 持板）**：
- 日志文件真实落 `/sdcard/logs/`，含 STT/LLM 对话原文片段；串口输出不因装钩子而中断（链式 vprintf 生效）。
- 循环覆盖：写满 32 个 512KB 后最旧文件被覆盖、目录 ≤16MB。
- 写失败 30s 重试：拔卡/坏卡降级不崩（整层失效、不 panic）。

## 风险与降级

- **隐私/机密性（riskTier=high 主因）**：对话原文（儿童语音转写 + AI 回复）明文落可拔 SD 卡，读卡器直读。威胁模型 = TECH-261016-sd-story-confidentiality 同族、且更敏感。human 已拍「含对话原文」= 有意识接受，但**记账义务在**：须在 TECH-261016 正文 append 一句或随架构 carve-out 措辞记账，防随批次遗忘（越晚补混淆，存量迁移成本越高）。
- **写盘任务资源**：低优先级 + 批量刷盘，避免与语音/AEC 争实时；环形缓冲溢出时丢最旧（不阻塞业务）。
- **FATFS 并发**：写盘任务与 story_sd_io 共享 `/sdcard`，靠 `max_files=8` 给足 FD；单写者纪律（日志文件只由写盘任务写）。
- **降级**：无卡/坏卡/写失败 → 整层失效不落盘、不崩溃、不阻断语音主链路（对齐 story_sd_io 的「无卡短路绝不 fopen」范式）。

## 依赖 / 前置

- **硬前置**：架构面 `/arch-change` 未冻结（决策 24 carve-out + 六处镜像 PROPOSAL FROZEN + landing）前，本承载包**不得 launch**。
- 契约面零依赖；需求面零依赖；治理面零依赖。
- 技术前置已就绪：SD 挂载能力（`261016-sd-card-mount`）+ SD 读写范式（`story_sd_io.cc` seam contract C3 先例）。
