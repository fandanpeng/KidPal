# 变异证据 · 261116-channel-resident-core · 真机轮 B 修复轮（2026-11-18）

> 执行者 = builder（dispatch `a54ef4b29ead8b101` 首版实现 + `a495ba0c7b59b5737` planner 白盒复核收紧补丁）；落盘 = planner（builder 对 `delivery/taskpackages/**` 无写权，按派发书硬约定③代落）。
> 还原方式 = 原文件全文写回 + `sha256` 比对（13/13 `restore_sha256_ok = true`，终态重建后逐字吻合）；未用 `git checkout`。
> 命令基准：host = 执行目录 `firmware/main/test/host_app`，`wsl -d Ubuntu-24.04 -- bash espidf_wsl.sh build` 后 `wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`；server = 仓根 `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`。

## 一、基线（收紧补丁后的终态）

| 文件 | 基线 sha256 |
|---|---|
| firmware/main/session_voice_watch.c | 5ce7d18fec4d570682a99a93820b6ad25e3fd85773deb62afb12732fa7209ded |
| firmware/main/session_voice_watch.h | 80a94f4b718589368f207b0dbc6a2bbd02fb70917340df12b71dea65d8f5d503 |
| firmware/main/application.cc | 3800ab24eeb9c7163d2078dc513c189634f79e653fd9f0d16ec0f17911be13bf |

- host：`321 Tests / 3 Failures / 0 Ignored`（3 红 = 上游存量 `p1_auxbar_style` AB-A1/A2/A4）
- server：`486 passed / 0 failed / 9 deselected`（front 零 diff）
- 设备编译：`firmware-build-check` → `build-pass`；`session_voice_watch.c.obj` + `application.cc.obj` 构建并链接，`xiaozhi.bin` 0x329580 字节（app 分区 8% free）

## 二、变异表（13 条，新基线实跑）

执行口径：逐条精确字符串替换（断言命中数恰 1）→ **先落变异、再重建、再实跑** → `host_infra_test.elf` → 整文件写回并比对 `sha256`。AB-A1/A2/A4 三条上游存量红在每条变异里都在场，下表「转红」只列本包相关项。

| # | 精确变异 | 命令 | 转红测试（本包相关，含 co-red） | 期望红命中 |
|---|---|---|---|---|
| 1 | `session_voice_watch.c`：收尾比较 `quiet_s >= idle_timeout_s` → `>` | build + elf | SCR-A9（co-red SCR-A10/A11；builder `asr_seen` 门 + 非正 cfg 用例） | ✓ |
| 2 | `session_voice_watch.c`：`kvw_level_sustained` 删 `&& level_run_s <= cfg->vad_trust_s`（上界恒真） | build + elf | SCR-A10（co-red builder 窗口边界） | ✓（**kill-shot，等价于轮 B 缺陷形态**） |
| 3 | `session_voice_watch.c`：`level_run_s >= cfg->voice_sustain_s` → `>= 1` | build + elf | SCR-A11（co-red builder 窗口边界） | ✓ |
| 4 | `session_voice_watch.h`：`KIDPAL_VW_VOICE_SUSTAIN_S 3 → 5`（强制验收 TU 重编） | build + elf | SCR-A11 + SCR-A13（cfg_default 同源 co-red） | ✓ |
| 5 | `session_voice_watch.c`：`voice_evidence = sustained \|\| asr_fresh` → `sustained \|\| (0 && asr_fresh)` | build + elf | SCR-A12 | ✓ |
| 6 | `session_voice_watch.h`：`KIDPAL_VW_VAD_TRUST_S 100 → 30`（强制验收 TU 重编） | build + elf | SCR-A12 + SCR-A10/A13（co-red） | ✓（**A7 承重**） |
| 7 | `session_voice_watch.h`：`KIDPAL_VW_DIAG_MIN_GAP_S 3 → 0`（强制验收 TU 重编） | build + elf | SCR-A14（行为半）+ builder `diag_due` | ✓ |
| 8 | `session_voice_watch.c`：`reset()` 删 `w->asr_seen = 0;` | build + elf | SCR-A13 + SCR-A9/A11 + builder 四条 | ✓ |
| 9 | `application.cc`：删 stt 分支 `kidpal_voice_watch_note_asr(&s_voice_watch);` | elf（源扫描） | SCR-A15② | ✓ |
| 10 | `application.cc`：A6 判据加回 `&& !s_vad_speaking` | elf（源扫描） | SCR-A15④ + SCR-A5（双保险同事实 co-red） | ✓（**电平门回潮回归防线**） |
| 11 | `application.cc`：诊断触发 `diag_due(...) == 1` → `clock_ticks_ % 10 == 0` | elf（源扫描） | SCR-A14 + SCR-A8 + DLG-A4 **三处同红** | ✓ |
| 12 | `session_voice_watch.c`：`reset_soft()` 退化成全清（`asr_seen` 也清 0） | build + elf | SCR-A13 + builder TEST 7 | ✓（**素材证据保活承重**） |
| 13 | `application.cc`：Listening case 退化成无条件 `kidpal_voice_watch_reset(&s_voice_watch)`（删条件分派） | elf（源扫描） | SCR-A15③（`reset_soft` 与 `kDeviceStateSpeaking` 双缺） | ✓（**丢稿回归防线**） |

与首版 11 条对比：既有 11 条在新基线上形态无变化（#8 的 co-red 多一条 builder TEST 7，属预期同向；#2/#6 co-red 分布一致）。#12、#13 为本轮收紧补丁新增，分别钉死「软复位退化」与「条件分派被删」两种回归。

## 三、如实申报（观察项，非阻塞）

1. **头宏变异（#4/#6/#7）沿用强制重编**：`session_voice_watch.h` 在 acceptance-author 编红基线时尚不存在，验收 TU 的 depfile 从未登记它；首跑改头不触发验收 TU 重编（表现为 SCR-A15 假红、SCR-A14 行为半未被激发）。证据口径 = 先删 `session_channel_resident.acceptance.c.o` 与 `session_voice_watch.c.o` 强制重编。**该缺口是既有构建卫生问题**，已记 review-packet residual。
2. **co-red 申报**：#4/#6 同打红 SCR-A13 的 `cfg_default` 三阈值断言（同一头宏单源）；#11 同打红 SCR-A14 结构半、SCR-A8 区内负锚与 `diag_log` DLG-A4⑤（同一字面三处承载）——均属预期同向，不作独立判别力计。
3. **builder 变异驱动的自身缺陷（已修正重跑）**：首版曾「先重建后落变异」导致 seam 行为断言漏红；另有一次文本模式读写把 `application.cc` 行尾由 CRLF 写成 LF。修正后改为「先落变异 → 重建 → 实跑」并全程二进制读写；`application.cc` 已按 CRLF 写回，sha256 与基线逐字一致。上表为修正后结果。
4. **未上真机**：本轮只跑 host / 设备编译 / server；运行期无新观测，不作已兑现申报。