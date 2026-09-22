# input-brief · builder · 260922-m3-k5-sntp-time-sync

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-22T09:40:55.916Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/260922-m3-k5-sntp-time-sync/taskpackage.json` · subsystem: firmware · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/requirements.md` | `S-F1.1` | S-F1.1 · 常显首页看「接下来干嘛」 | 224 | ok |
| `docs/requirements.md` | `NFR-OFF-1` | NFR-OFF-1 · 离线边界 | 591 | ok |
| `delivery/workstreams/m3-schedule/progress.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/application.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/ota.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/lvgl_display/lvgl_display.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/home_p1.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/p1_home_battery.acceptance.c` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 单子系统纯固件变更（firmware 时间内聚簇：SNTP 挂点 + PCF85063 读写 + render_node inject + 状态栏守门），无跨包行为对偶面

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260921-m3-k2-p2-week-grid-page | `firmware/main/test/schedule_grid.acceptance.c` |
| 260921-m3-k4-p1-cjk-dynamic-text | `firmware/main/test/p1_cjk_name.acceptance.c` |
| 260920-m2-charwindow-card-fix | `firmware/main/test/char_stroke.acceptance.c` · `firmware/main/test/char_window_overlay.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

