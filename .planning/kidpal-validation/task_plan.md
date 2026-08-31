---
workstream: KidPal 功能支撑验证批
slug: kidpal-validation
status: active
opened: 2026-08-31
closed:
summary: 把 spec/UI 设计稿里待验证项逐个跑出可机判/可目测结论，产 V3 设计输入与云端 MVP 链路打通证明
owner: portfolio-planner
area: systems
changes:
  - 260831-cloud-link-bringup
  - 260831-simulator-lvgl-align
  - 260831-ui-art-assets
---

# kidpal-validation · task_plan

## 目的

需求已定稿（spec §4 story + UI 设计稿 v2），但两条主线的「能不能实现」未实证：UI 侧（手绘框族 / 吉祥物姿态 / 图标族 / 字体等方案已定未出图）与 OpenClaw 对接侧（ECS 实盘四块均未装/未通）。本批把「待验证」项逐个跑出可机判/可目测的验收结论（可行 / 不可行+备选），产出 ① V3 设计输入 ② 云端 MVP 链路打通证明。

## 边界

- 本批全是**验证/实证**工作，不改任何 story 功能语义、不触契约面 `docs/contracts/**`（只读引用 D3/E1/B1）。
- human 已定：UI+OpenClaw 全量、拆 3 个独立 TaskPackage、**只模拟器+云端、不烧真机**（真机项登记留根）。

## 拆包与依赖

- TP-A `260831-cloud-link-bringup`（server，P0）：开 OpenAI 端点 → 验 DeepSeek display 格式遵循度 → 装 whisper/edge-tts → 起最小 shim → 验 schedule.md 消费。
- TP-C `260831-simulator-lvgl-align`（tools，P0）：模拟器 LVGL 9.5→9.3 对齐 + lv_conf + 1bpp 阈值化快照正式化 + P1 回归。TP-B 前置。
- TP-B `260831-ui-art-assets`（tools，P1，依赖 C）：7 姿态全量 / 手绘框族 / 网点平铺 / 图标族 / 字体授权+栅格 / 资产预算核算。

## 关联

- 清单：[docs/notes/feature-inventory.md](../../docs/notes/feature-inventory.md)
- 上游：[changes/arch/260830-arch-review-ui-impl-strategy/](../../changes/arch/260830-arch-review-ui-impl-strategy/)
