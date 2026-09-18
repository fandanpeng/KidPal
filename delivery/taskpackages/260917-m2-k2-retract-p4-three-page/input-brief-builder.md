# input-brief · builder · 260917-m2-k2-retract-p4-three-page

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-18T12:15:09.810Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/260917-m2-k2-retract-p4-three-page/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `delivery/milestones/milestones.jsonl` | `M2-char-window` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `§2.4` | 2.4 页面结构（3 页循环） | 172 | ok |
| `docs/requirements.md` | `§2.2` | 2.2 状态表（会话生命周期 × UI 演出） | 58 | ok |
| `docs/requirements.md` | `F3` | F3 查生字（2026-09-14 v1.36 改窗：原 P4 常驻页撤销） | 458 | ok |
| `firmware/main/display/kidpal_render_node.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/key_gesture_parser.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/key_gesture_parser.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/keybar.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/lcd_display.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/mcp_server.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/host_app/main/CMakeLists.txt` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/display/kidpal_render_node.cc` | `kidpal_render_node_cycle_next` | :666-692 | — |
| `firmware/main/display/keybar.c` | `kidpal_keybar_dots` | :18-30 | — |
| `firmware/main/key_gesture_parser.cc` | `key_page_next` | :95-102 | — |
| `firmware/main/display/lcd_display.cc` | `kidpal_keybar_sync_locked` | :457-591 | 1099, 1314, 1446, 2094, 2158 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260917-m1-k4-rma8-fixbatch | `firmware/main/test/reminder_overlay_unify.acceptance.c` |
| 260917-m2-k1-char-link-defect-fix | `firmware/main/test/char_stroke.acceptance.c` |
| 260915-m1-k1a-key-layer-dispatch | `firmware/main/test/key_layer_dispatch.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

