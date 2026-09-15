# input-brief · acceptance-author · 260829-hardware-validation

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-09T14:22:22.867Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `.claude/taskpackages/260829-hardware-validation/taskpackage.json` · subsystem: firmware+server · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `.planning/kidpal-mvp/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `docs/spec.md` | `§2` | 2. 硬件能力盘点（依据本地官方 Demo 实测确认） | 20 | ok |
| `docs/spec.md` | `§5.4` | 5.4 页面结构（4 页循环） | 497 | ok |
| `9b` | — | （文件不存在） | — | missing |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

`crossEnd` 字段**不在场**。起包期必须补骨架 `{ "counterparts": [] }`（空骨架也行，字段必须在场）——见 DEV-GOVERNANCE「起包格式期望前置」第 6 条。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260901-ui-art-integration | `firmware/main/test/ui_art_integration.acceptance.c` |
| 260829-char-stroke | `firmware/main/test/char_stroke.acceptance.c` · `server/acceptance/test_char_stroke.acceptance.py` |
| 260829-schedule-reminder | `firmware/main/test/schedule_reminder_b.acceptance.c` · `server/acceptance/test_schedule_reminder.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

