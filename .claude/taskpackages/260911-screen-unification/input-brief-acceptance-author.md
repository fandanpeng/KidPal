# input-brief · acceptance-author · 260911-screen-unification

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-11T05:43:46.737Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `.claude/taskpackages/260911-screen-unification/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/review.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/decision-coverage.json` | — | 无锚点（整文件） | — | bare |
| `docs/ARCH-REVIEW-QUEUE.md` | — | 无锚点（整文件） | — | bare |
| `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§2.1` | 2.1 三屏主权收口（#4，核心结构题 → OQ1） | 56 | ok |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§2.3` | 2.3 回答驻留（#6 → OQ3） | 89 | ok |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§4` | 4. 拆包预判与依赖序（供 portfolio-planner intake） | 130 | ok |
| `.planning/260911-display-interaction-sovereignty/review.md` | `F6` | （未定位到标题节） | — | not-found |
| `.planning/260911-display-interaction-sovereignty/review.md` | `F7` | （未定位到标题节） | — | not-found |
| `.planning/260911-display-interaction-sovereignty/review.md` | `F8` | （未定位到标题节） | — | not-found |
| `docs/ARCH-REVIEW-QUEUE.md` | `ARQ-260911-split-package-implementation-prose-gap` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/application.cc` | `SetChatMessage` | :71-81 | 153, 310, 372, 378, 395, 443, 473, 482, 525 |
| `firmware/main/application.cc` | `SetStatus` | :115-116 | 143, 152, 156, 404, 542, 553, 709, 715, 720 |
| `firmware/main/application.cc` | `SetEmotion` | :396-399 | 489, 543, 554, 710, 716, 721 |
| `firmware/main/display/kidpal_render_node.cc` | `kidpal_render_node_show_chat` | :306-320 | — |
| `firmware/main/display/kidpal_render_node.cc` | `kidpal_render_node_begin_page` | :332-344 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260901-ui-art-integration | `firmware/main/test/ui_art_integration.acceptance.c` |
| 260829-char-stroke | `firmware/main/test/char_stroke.acceptance.c` · `server/acceptance/test_char_stroke.acceptance.py` |
| 260829-hardware-validation | `firmware/main/test/hardware_validation.acceptance.c` · `server/acceptance/test_story_wiring.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

