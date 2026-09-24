# input-brief · builder · 260924-m4-k1-story-cloud-link

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-24T09:59:27.040Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> regenerated-because: 门1 F1/F3/F4/F5/F7 findings 吸收后 taskpackage 修订再生成（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/260924-m4-k1-story-cloud-link/taskpackage.json` · subsystem: server · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3③` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§1.3④` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 251 | ok |
| `docs/requirements.md` | `F2` | F2 星星记录（原「故事记录」，2026-09-14 改名） | 320 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `docs/architecture.md` | `§5.4` | 5.4 数据视图（谁拥有、存哪、活多久） | 254 | ok |
| `delivery/workstreams/m4-star-notebook/progress.md` | — | 无锚点（整文件） | — | bare |
| `server/gateway/story.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/display_schema.py` | — | 无锚点（整文件） | — | bare |
| `server/acceptance/archive/test_story_p3.acceptance.py` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/CONTRACT-DECISION-QUEUE.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/README.md` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/application.cc` | `HandleToggleChatEvent` | :300-303 | — |
| `firmware/main/mcp_server.cc` | `kidpal_story_list_parse` | :752-762 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_page_render` | :1182-1390 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260924-perf-k1-roundtrip-latency | `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py` |
| 260923-m3-k7-schedule-grid-restore | `server/acceptance/test_schedule_grid_restore.acceptance.py` · `firmware/main/test/schedule_grid_restore.acceptance.c` |
| 260921-m3-k1-cloud-schedule-producer | `server/acceptance/test_m3k1_schedule_producer.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `workstream:m4-star-notebook` | story_list 形状与推送（total=现存篇数语义 + items 形状 + 三时点推送） | `260924-m4-k1-story-cloud-link` | `260924-m4-k1-story-cloud-link` | 形状权威 = 契约门签字后的 interface-contract.md §1.3③；K2 消费齿按冻结形状写、不得自定字段；K1 契约 diff 未落地前 K2 acceptance-author 不冻列表形状/计数齿（M3 同款条款）。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

