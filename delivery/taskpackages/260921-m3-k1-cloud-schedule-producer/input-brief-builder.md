# input-brief · builder · 260921-m3-k1-cloud-schedule-producer

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-21T02:30:15.311Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/taskpackage.json` · subsystem: server · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3` | 1.3 各 view 类型定义 | 44 | ok |
| `docs/contracts/interface-contract.md` | `§2` | 2.1 设计原则 | 155 | ok |
| `docs/requirements.md` | `F1` | F1 课表 / 日程助手 | 204 | ok |
| `delivery/milestones/milestones.jsonl` | `M3-schedule` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.2` | S-F1.2 · 课表页看「一周总览」 | 257 | ok |
| `docs/requirements.md` | `S-F1.3` | S-F1.3 · 语音问课表 | 272 | ok |
| `docs/requirements.md` | `S-F1.5` | S-F1.5 · 家长录入课表 | 304 | ok |
| `docs/requirements.md` | `S-F1.1` | S-F1.1 · 常显首页看「接下来干嘛」 | 221 | ok |
| `docs/requirements.md` | `S-F1.1-AC1` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.5-AC1` | （未定位到标题节） | — | not-found |
| `docs/architecture.md` | `§5.1` | 5.1 逻辑视图（组件与职责） | 159 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `server/gateway/schedule.py` | `parse_schedule_md` | （未定位到标题节） | — | not-found |
| `server/gateway/front/front_app.py` | `_relay_display_blocks_to_device` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/schedule.py` | `parse_schedule_md` | :236-237 | — |
| `server/gateway/schedule.py` | `group_upcoming` | :284-289 | — |
| `server/gateway/front/front_app.py` | `_relay_display_blocks_to_device` | :271-273 | 327, 341, 388 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260920-m2-charwindow-display-fix | `firmware/main/test/char_window_overlay.acceptance.c` · `server/acceptance/test_char_meaning_merge.acceptance.py` |
| 260911-display-first-m1 | `server/acceptance/test_g_relay_fix.acceptance.py` |
| 260911-ecs-deploy-convergence | `server/acceptance/test_gateway_directionB.acceptance.py` · `server/acceptance/test_char_stroke.acceptance.py` · `server/acceptance/test_story_p3.acceptance.py` · `server/acceptance/test_story_wiring.acceptance.py` · `firmware/main/test/device_ws_switch.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `workstream:m3-schedule` | schedule 周网格 payload 形状（契约冻结单源） | `260921-m3-k1-cloud-schedule-producer` | `260921-m3-k1-cloud-schedule-producer` | 形状权威 = 契约门签字后的 interface-contract.md §1.3②；K2 消费齿按冻结形状写、不得自定字段；K1 契约 diff 未落地前 K2 acceptance-author 不得冻网格形状齿。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

