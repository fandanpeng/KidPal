# 变异证据 · 261116-channel-resident-core · 真机轮 C 修复轮（2026-11-18）

> 执行者 = builder（dispatch `a25b51d4b21a6d53c`）；落盘 = planner（builder 对 `delivery/taskpackages/**` 无写权，按派发书硬约定③代落）。
> 还原方式 = 原文件整文件写回 + `sha256` 比对（8/8 `sha256_restored_matches = true`）；未用 `git checkout`。
> 基线 sha256（`server/gateway/front/front_app.py`，工作树 CRLF 字节口径）= `ea09f0b505229ff8dc51a33597466dcf31fb0bdf1f5d41b73966910b43bd8bcc`。
> 统一命令：`python -m pytest -q --tb=no -rf --import-mode=importlib -m "not live" server/acceptance/test_session_channel_resident.acceptance.py server/tests/test_archive_guard_units.py`（仓根起）。

## 一、基线

| 面 | 结果 |
|---|---|
| 目标件（acceptance 本件 + 归档守卫单测） | **47 passed / 0 failed / 99 subtests passed**（3.03s） |
| server 全量（acceptance + tests） | **799 passed / 0 failed / 9 deselected / 137 subtests passed**（65.76s） |
| host 面 | 本轮零改动，沿用 321 Tests / 3 Failures（3 红 = 上游存量 `p1_auxbar_style`） |

## 二、变异表（8 条，全部实跑）

| # | 精确变异 | 转红测试（含 co-red） | 还原 |
|---|---|---|---|
| 1 | 删 `_organize_body_reject_reason` 的 R4 括号判据块 | SCRF-C2 判别力：`【无】` / `（本轮素材不足）`；单测 R1–R4 同名子测 + R4 双层/16 字边界 | sha256 一致 |
| 2 | `_ORGANIZE_BODY_MIN_CHARS 2 → 0` | SCRF-C2 判别力：`嗯`；单测 R2 子测 | sha256 一致 |
| 3 | 删 `_story_feed_from_stt` 段级过滤调用 | SCRF-C1；SCRF-C4 命中族 / pending 不即时释放 / 集合不变式 | sha256 一致 |
| 4 | 段级集合加入单字 `嗯` | **SCRF-B10 case 2**（收尾沿被吞 → `fin==[]`）；SCRF-C4 命中族 / 集合不变式；单测命中/不变式子测 | sha256 一致（**不变式判别力承重**） |
| 5 | 静默释放分支加回 `_speak_to_legs(ORGANIZING_FAIL_SOOTHING, legs)` | SCRF-C2 端到端（零安抚断言） | sha256 一致 |
| 6 | 静默释放分支去掉 `_push_story_list` 页面退出帧 | SCRF-C2 端到端；SCRF-C3 `llm_unavailable`（co-red） | sha256 一致（**R6 困页防线**） |
| 7 | `reason=organize_placeholder` → `reason=empty_material` | SCRF-C2 端到端审计分档断言 | sha256 一致 |
| 8 | 单行前置守卫 + 主体守卫一起移到 `archive_story` 之后 | SCRF-C2 端到端零归档/seq 断言；SCRF-C2 判别力八样本 | sha256 一致（**seq 先占号防线**） |

## 三、观察项（如实登记，不阻塞）

1. **#8 首版只移主体守卫 → 0 红**：单行分支的前置 `_organize_body_reject_reason(cand)` 先接住八个单行样本，未触及 `archive_story`。改为同时移走单行前置与主体守卫后按预期转红。结论：单行前置守卫是独立的第一道产物闸，变异覆盖面须同时移两处才能钉住「守卫在 archive 前」的结构性质。
2. **R4 判序实现为 R1→R4→R2→R3**（规格枚举为 R1→R2→R3→R4）：冻结验收 C2 对 `【无】` 期望 `placeholder_body`，而其 compact=1 会先命中 R2 `insignificant_body`；验收不可改，故让结构判据 R4 先于长度判据 R2。两类 reason 同属静默释放档（行为零差），判据集合与语义未变；已在代码 docstring 与进度快照留痕，planner 裁定 = 采纳（见 plan.md 轮 C 节补记）。
3. **`_load_output_guard` wanted 增列 `_FRONT_GUARD_STRIP_RE`**：`_guard_compact` 函数体直接引用该模块级正则，不抽取会在受限 exec 期 NameError（假红）。既有 `_load_guard` 的 wanted/ns 未动。
4. **`not material` 直调分支 reason = `empty_body`**：收尾单源在调用前已短路空素材，仅直调形态可达，归静默释放类。