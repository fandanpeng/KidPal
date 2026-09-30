# Mutation 证据 · 260924-m4-k3-record-session-live

> builder 源级第一方重放账（2026-09-30），planner 代存（builder 对 delivery/taskpackages/** 无写权，回执转存）。
> sensitive 七条逐条：变异 → 红（RED verbatim）→ 还原 → 复绿。host 通道基线 = 136T；server 通道基线 = 203P。
> taskpackage.json mutationEvidence[] 为结构化投影（含 acceptanceEntry 原文逐字 + sensitive 信号）；本文件为重放过程账。

## M4K3-A1（sensitive）

- 变异：摘 application.cc 发送调用
- RED：136T **3F** —— RSL-A2「HandleStateChangedEvent Listening 分支内 SendKidpalSessionCtx 调用数 != 1（0 = 会话发起发送半未接线…）」+ RSL-A1「SendKidpalSessionCtx 仅 2/3 文件命中（期望 >= 3…）」（RSL-A3 随之红）
- 还原：136T 0F

## M4K3-A2（sensitive）

- 变异：guide_bubble 返 NULL（重建）
- RED：136T **1F** —— RSL-A4「kidpal_story_guide_bubble() 返回 NULL——引导态气泡单源 frozen seam [S2] 未实现（AC1 屏显引导无承载）」
- 还原：136T 0F

## M4K3-A3（sensitive）

- 变异：STOP case 改 CloseAudioChannel
- RED：136T **1F** —— RSL-A5「KSPV2_ACT_STOP_RECORD case 零 StopListening 接线——KEY 兜底收尾…front listen-stop 归档缝收不到上行 = 永不归档」
- 还原：136T 0F

## M4K3-A4（守卫齿，不重放）

- must-stay-green 守卫（真实连接流程两齿），重放清单外；终跑两齿保持绿。敏感性由 front cancel 语义面承载（断连回潮即红——taskpackage mutationEvidence 记录在案）。

## M4K3-A5（sensitive）

- 变异：echo 满容量不挤最旧（重建）
- RED：136T **1F** —— RSL-A6「滚出后 line(0) != 「它住在森林里」——FIFO 滚出错位（最旧句「从前有一只小猫」应被挤出）」
- 还原：136T 0F

## M4K3-A6（sensitive）

- 变异：gate 恒 0（重建）
- RED：136T **1F** —— RSL-A7「VOICE 前台 + LIST 页未判只装缓存（期望 1）——…defer 包真机缺陷形态复潮」
- 还原：136T 0F

## M4K3-A7（sensitive）

- 变异：ctx 分支沿判置 False
- RED：pytest **2F** —— test_a7_ctx_story_first_pushes_exactly_one_story_list + test_a7_ctx_duplicate_pushes_no_extra_frame（= 冻结红基线同名）
- 还原：203P 0F

## M4K3-A8（sensitive，两方向）

- 变异①：v8 删双指令段 → test_a8_current_prompt_dual_trigger_and_boundary 红（1F）
- 变异②：provider deploy_target 漏翻留 v7 → test_a8_prompt_three_faces_version_agnostic 红（1F）
- 还原：203P 0F

## M4K3-A9（structural / 套件级回归门）

- 豁免逐 AC 重放（M-4 成文）；canary/注册序由 RSL-C1 与 K1/K2 件自身齿承载；全量通道零偏移即其证据形态。
