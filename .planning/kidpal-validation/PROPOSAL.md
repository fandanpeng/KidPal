# KidPal 功能支撑验证批 — 工作流 PROPOSAL

> 类型：planning-workstream（功能支撑验证 / 可行性实证批）
> 日期：2026-08-31
> 关联清单：[docs/notes/feature-inventory.md](../../docs/notes/feature-inventory.md)（UI 元素分级 + OpenClaw 对接状态）
> 上游依据：human 2026-08-31 口头立项（"做一个架构验证或功能支撑验证的 taskpackage"）+ [260830-arch-review-ui-impl-strategy](../changes/arch/260830-arch-review-ui-impl-strategy/) 的实现策略定论。
> 性质：本批全是**验证/实证**工作，不改任何 story 功能语义、**不触契约面** `docs/contracts/**`（只读引用 D3/E1/B1；若验证发现契约需改，登记 ARQ 走契约门，不在本批直改）。

## 0. 目的

需求已定稿（spec §4 story + UI 设计稿 v2），但两条主线的"能不能实现"未实证：

- **UI 侧**：260830 验证只实跑了 P1 一页的 8 枚静态资产 + 眨眼动画。手绘倾斜框、吉祥物其余 6 姿态、网点纹理、全图标族、成就印章、动态时间字体、站酷快乐体——方案已定但没出图。V3 设计稿（实现导向）需要这些元素的可行性结论作输入。
- **OpenClaw 对接侧**：2026-08-31 实盘 ECS（8.137.91.16）发现 spec"待装"四块**目前都没有**——OpenAI 兼容端点 404 未开、shim 不存在、faster-whisper 与 edge-tts 未装（系统 Python 仅 3.6.8，whisper 需 ≥3.8）、DeepSeek 吐 ` ```display ` 块格式遵循度未实测。MVP 链路主线一块未通。

本批目的：把"待验证"项逐个跑出**可机判/可目测的验收结论**（可行 / 不可行+备选），产出 ① V3 设计输入 ② 云端 MVP 链路打通证明。

**human 已定边界**：UI+OpenClaw 全量、拆 3 个独立 TaskPackage、**本批只模拟器+云端、不烧真机**（真机项登记留根）。

## 1. 拆包与依赖

```
TP-C 260831-simulator-lvgl-align（模拟器 9.3 地基，P0）─┐
                                                      ├─→ TP-B 260831-ui-art-assets（UI 资产全量，P1，依赖 C）
TP-A 260831-cloud-link-bringup（云端链路打通，P0）─────┘  （A 与 C/B 并行，无依赖）
```

- **TP-A**（subsystem=server）：开 OpenAI 端点 → 验 DeepSeek display 格式遵循度（命门）→ 装 whisper/edge-tts → 起最小 shim `/v1/chat` 一路 → 验 schedule.md 消费。
- **TP-C**（subsystem=tools）：模拟器 LVGL 9.5→9.3 对齐 + lv_conf 对齐 + 1bpp 阈值化快照正式化 + P1 回归 + 实验代码正式化。是 TP-B 的前置（否则又在 9.5 上验，结论失真）。
- **TP-B**（subsystem=tools）：7 姿态全量 / 手绘框族 9 宫格 / 倾斜烘资产 / 网点平铺 / 全图标族 / 站酷快乐体授权+栅格 / Baloo 2 imgfont 动态时间 / 资产总量预算核算。

## 2. 真机留根项（本批不做，登记 ARQ）

- I1 格式在真机 9.3 + SPIFFS 文件源路径可用性（若 TP-B 预算核算判定必须上 I1 则升 P0）。
- 16px 以下 CJK 在 1bpp 真机可读性实测（定 V3 字号下限）。
- 前光确认、AEC 喊停实测调优、功耗实测。

## 3. 验收总则

- 各包验收以**可机判断言**为主（grep/curl/版本头/JSON 可解析率），UI 观感类以 human 按 OQ-3(a)「结构+风格保真、像素级不追求」目测阈值化 PNG 为准。
- 三包各出独立验证报告，收口汇总成《V3 设计输入结论》+《云端 MVP 链路打通证明》。
