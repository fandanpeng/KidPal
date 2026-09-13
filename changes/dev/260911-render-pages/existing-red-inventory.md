---
changeId: 260911-render-pages
date: 2026-09-12
author: integrator（红证据 ESCALATE 豁免件——supervisor 机械闸按形状检查；事实源 = red-baseline-freeze.md + r5 final fact）
status: EXISTING-RED-INVENTORY
---

# existing-red-inventory · 260911-render-pages（final r5 ok:false 红源逐条归位）

> 承重 fact = `.claude/taskpackages/260911-render-pages/verifier-evidence/rounds/r5.json`（phase=final，round=5，inv `7f3c2a41-9b6e-4d58-a1c2-84e0d7b91f30`，`ok:false`）。
> 红源共两类：final 闸红（unit/acceptance 同源 1 条）+ host runner 基线红（3 条）。**全部为在册既存红（owner 各归原包），零新增红，本包不代修不代采**。在册第 4 条红 P2-A1 已由 RP-A1 修齿消解出失败清单（见 §三）。

## 一、final 闸红（unit/acceptance 两闸同源）

| # | 红 | 位置 | owner | 处置 |
|---|----|------|-------|------|
| R1 | SMK-A4:422 `test_a4_display_line_evidence` | `server/acceptance/test_smoke_e2e.acceptance.py:422`（`server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据） | 260829-smoke-e2e | 留红如案，提 human 按红证据 ESCALATE 通道裁（红转交 owner 或独立批补采）；本包 `git diff -- server/` 零改动，与先例 PKG-4（260911-display-first-m1）合并夜同一红同名单 |

r5 实测计数：unit `1 failed / 168 passed / 6 skipped / 45 deselected`；acceptance `1 failed / 131 passed / 6 skipped / 45 deselected`——两闸唯一红同源即 R1。

## 二、host runner 基线红（WSL ELF 全量，red-baseline-freeze §一引证、不重跑）

| # | 红 | owner | 处置 |
|---|----|-------|------|
| R2 | WSA-A6:830 | 260903-device-ws-switch | 在册他人红，不代修 |
| R3 | WSA-A4:663 | 260903-device-ws-switch | 在册他人红，不代修 |
| R4 | VRT-A2·E2:878 | 260829-voice-roundtrip（cwd 敏感，判读钉明 cwd） | 在册他人红，不代修 |

出处 = changes/dev/260911-screen-unification/existing-red-inventory.md + changes/dev/260911-display-first-m1/existing-red-inventory.md（合并夜追记）+ 本包 red-baseline-freeze.md §一（human 指示 2026-09-12：不重跑，引在册）。KEYA 5/5 + SU 7/7 绿。

## 三、在册红 P2-A1 消解记录（本包 RP-A1 兑现证据）

P2-A1:571（schedule_p2.acceptance.c 齿 6 first-match 定位歧义恒红）= 在册第 4 条红、本包 RP-A1 修复对象——acceptance-author 方案 B 修齿（定义签名 `int kidpal_schedule_page_render(` 文件族定位，tightly-only + REVISION LOG，方案 A 补哨兵禁令遵守）后，schedule_p2 7/7 绿，host 全量 124 Tests / 3 Failures 复核确认 P2-A1 不在失败清单。

## 四、零新增红声明

本包齿终态（red-baseline-freeze §七）：v3_restore 6/6、story_p3 10/10、schedule_p2 7/7、ui_art_integration 11/11、story_page_stack 3/3 全绿；失败清单逐条比对后本包零新增红。scoped 7 闸 r5 全绿（含 firmware-build 真编译 xiaozhi.bin 0x2c35b0）。
