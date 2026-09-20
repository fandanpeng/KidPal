# input-brief · builder · 260920-m2-charwindow-card-fix

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-20T04:31:27.473Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> regenerated-because: sourceType 字段移除后重生成，refs/crossEnd 零变化（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/260920-m2-charwindow-card-fix/taskpackage.json` · subsystem: firmware · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `S-F3.2` | S-F3.2 · 看拼音/笔画/部首/释义/组词 | 497 | ok |
| `docs/requirements.md` | `F3` | F3 查生字（2026-09-14 v1.36 改窗：原 P4 常驻页撤销） | 458 | ok |
| `docs/requirements.md` | `§2.2` | 2.2 状态表（会话生命周期 × UI 演出） | 58 | ok |
| `firmware/main/display/char_page.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/char_stroke.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/char_stroke.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/lcd_display.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/char_stroke.acceptance.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/char_window_overlay.acceptance.c` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-patch/char_stroke_lib.py` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 板端显示面字体接线与释义渲染，零跨端 wire 形状改动——D3 data.meaning 字段既有、OpenClaw 生产端（char_stroke_lib.py 取词典 definition）已在下发，契约与云面零改动

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260917-m1-k4-rma8-fixbatch | `firmware/main/test/reminder_overlay_unify.acceptance.c` |
| 260917-m2-k1-char-link-defect-fix | `firmware/main/test/char_stroke.acceptance.c` |
| 260917-m2-k2-retract-p4-three-page | `firmware/main/test/retract_p4_three_page.acceptance.c` · `firmware/main/test/keybar_three_slot.acceptance.c` · `firmware/main/test/key_layer_dispatch.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

