# input-brief · acceptance-author · 260911-render-pages

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-12T04:50:10.271Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260911-render-pages/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md` | `§四A` | （未定位到标题节） | — | not-found |
| `delivery/changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md` | `§四D` | （未定位到标题节） | — | not-found |
| `delivery/changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md` | `§七` | 七、遗留问题总清单与分批方案（2026-09-11 登记，收尾合并后） | 153 | ok |
| `delivery/changes/dev/260829-hardware-validation/review-packet.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/260911-display-interaction-sovereignty/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/kidpal-ui-design-v3.html` | — | 无锚点（整文件） | — | bare |
| `docs/notes/ui-implementation-strategy.md` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/dev/260829-hardware-validation/review-packet.md` | `F5` | （未定位到标题节） | — | not-found |
| `delivery/changes/dev/260829-hardware-validation/review-packet.md` | `F2` | Review Packet · 260829-hardware-validation（包 9b 硬件真机验收：物理冒烟 + 全功能端到端 + F2 接线 + ECS 窗口） | 1 | ok |
| `docs/ui/kidpal-ui-design-v3.html` | `sec-pages` | （未定位到标题节） | — | not-found |
| `docs/ui/kidpal-ui-design-v3.html` | `sec-states` | （未定位到标题节） | — | not-found |
| `docs/ui/kidpal-ui-design-v3.html` | `sec-rules` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/display/schedule_page.c` | `kidpal_schedule_page_render` | :700-805 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_page_render` | :1122-1301 | — |
| `firmware/main/display/char_stroke.c` | `kidpal_char_card_build` | :486-514 | — |
| `firmware/main/application.cc` | `SetChatMessage` | :75-85 | 157, 314, 376, 382, 399, 447, 477, 486, 529 |
| `firmware/main/application.cc` | `SetStatus` | :119-120 | 147, 156, 160, 408, 546, 557, 713, 722, 733 |
| `firmware/main/application.cc` | `SetEmotion` | :400-403 | 493, 547, 558, 714, 723, 734 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260911-display-first-m1 | `server/acceptance/test_g_relay_fix.acceptance.py` |
| 260911-key-route-a1 | `firmware/main/test/key_route_a1.acceptance.c` |
| 260911-screen-unification | `firmware/main/test/screen_unification.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

