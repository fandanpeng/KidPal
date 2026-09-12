# input-brief · acceptance-author · 260911-display-first-m1

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-11T16:52:48.762Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `.claude/taskpackages/260911-display-first-m1/taskpackage.json` · subsystem: server+firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/review.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/decision-coverage.json` | — | 无锚点（整文件） | — | bare |
| `docs/ARCH-REVIEW-QUEUE.md` | — | 无锚点（整文件） | — | bare |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§2.2` | 2.2 显示先行（#5，机制候选 → OQ2） | 73 | ok |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§4` | 4. 拆包预判与依赖序（供 portfolio-planner intake） | 130 | ok |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§5` | 5. 验证面（workstream 级） | 150 | ok |
| `.planning/260911-display-interaction-sovereignty/PROPOSAL.md` | `§7` | 7. 事实源 / 契约触面申报（逃生门，本 PROPOSAL 不批以下任何一项） | 164 | ok |
| `.planning/260911-display-interaction-sovereignty/review.md` | `F3` | （未定位到标题节） | — | not-found |
| `.planning/260911-display-interaction-sovereignty/review.md` | `F5` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/mcp_relay.py` | `display_tool_call` | :10-11 | 99 |
| `server/deploy/engine-patch/connection.py` | `DisplayFenceStateMachine` | :1168-1194 | — |
| `server/deploy/engine-patch/kidpal_display_strip.py` | `feed` | :11-19 | 41, 51, 55 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260911-ecs-deploy-convergence | `server/acceptance/test_gateway_directionB.acceptance.py` · `server/acceptance/test_char_stroke.acceptance.py` · `server/acceptance/test_story_p3.acceptance.py` · `server/acceptance/test_story_wiring.acceptance.py` · `firmware/main/test/device_ws_switch.acceptance.c` |
| 260911-key-route-a1 | `firmware/main/test/key_route_a1.acceptance.c` |
| 260911-screen-unification | `firmware/main/test/screen_unification.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

