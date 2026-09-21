# input-brief · builder · 260921-m3-k2-p2-week-grid-page

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-21T02:30:15.784Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/260921-m3-k2-p2-week-grid-page/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3` | 1.3 各 view 类型定义 | 44 | ok |
| `docs/requirements.md` | `F1` | F1 课表 / 日程助手 | 204 | ok |
| `delivery/milestones/milestones.jsonl` | `M3-schedule` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.2` | S-F1.2 · 课表页看「一周总览」 | 257 | ok |
| `docs/requirements.md` | `S-F1.2-AC1` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.2-AC2` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.2-AC3` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.2-AC4` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `S-F1.1` | S-F1.1 · 常显首页看「接下来干嘛」 | 221 | ok |
| `docs/requirements.md` | `S-F1.1-AC1` | （未定位到标题节） | — | not-found |
| `docs/requirements.md` | `NFR-READ-1` | NFR-READ-1 · 1bpp CJK 字号可读性下限 | 598 | ok |
| `firmware/main/display/schedule_page.c` | `kidpal_schedule_parse` | （未定位到标题节） | — | not-found |
| `firmware/main/display/home_p1.c` | `kidpal_home_p1_schedule_inject` | （未定位到标题节） | — | not-found |
| `firmware/main/display/kidpal_render_node.cc` | `kidpal_render_node_cache_schedule` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/schedule.py` | `parse_schedule_md` | :236-237 | — |
| `server/gateway/front/front_app.py` | `_relay_display_blocks_to_device` | :271-273 | 327, 341, 388 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260920-m2-charwindow-card-fix | `firmware/main/test/char_stroke.acceptance.c` · `firmware/main/test/char_window_overlay.acceptance.c` |
| 260920-m2-charwindow-display-fix | `firmware/main/test/char_window_overlay.acceptance.c` · `server/acceptance/test_char_meaning_merge.acceptance.py` |
| 260917-m1-k4-rma8-fixbatch | `firmware/main/test/reminder_overlay_unify.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `package:260921-m3-k1-cloud-schedule-producer` | 首页「接下来」home data 承载 | `260921-m3-k1-cloud-schedule-producer` | `260921-m3-k2-p2-week-grid-page` | payload 语义 owner = K1（契约冻结 home 可选 data 形状）；板端注入接线承接 = K2（home_p1 schedule_inject 冻结 seam 消费）。 |
| `workstream:m3-schedule` | 首页「接下来」home data 承载与注入 | `260921-m3-k1-cloud-schedule-producer` | `260921-m3-k2-p2-week-grid-page` | payload 语义 owner = K1（经契约门冻结 home 可选 data 形状）；实际板端接线承接 = K2（home_p1 schedule_inject 冻结 seam 消费，seam 本体零改）。读侧按 carriedBy 判承接。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

