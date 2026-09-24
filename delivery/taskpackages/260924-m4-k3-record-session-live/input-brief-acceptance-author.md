# input-brief · acceptance-author · 260924-m4-k3-record-session-live

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-24T09:59:32.398Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> regenerated-because: 门1 F1/F3/F4/F5/F7 findings 吸收后 taskpackage 修订再生成（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/260924-m4-k3-record-session-live/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 251 | ok |
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图后下发） | 154 | ok |
| `docs/requirements.md` | `S-F2.2` | S-F2.2 · 开录（双入口）（原「一键开录」「一键录故事」） | 422 | ok |
| `docs/requirements.md` | `§2.1` | 2.1 语音交互模型（已定：按一下 KEY / 喊唤醒词启动会话；KEY 打断停播） | 29 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `docs/architecture.md` | `§5.5.5` | 5.5.5 OpenClaw system prompt 版本化 | 294 | ok |
| `delivery/workstreams/m4-star-notebook/progress.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/application.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/websocket_protocol.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/protocol.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/mcp_server.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/story_page.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/session_state_machine.h` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/story.py` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-system-prompt.v6.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `server/deploy/provider.config.yaml` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `_story_feed_from_stt` | :98-149 | 334 |
| `server/gateway/front/front_app.py` | `_story_route_display_payload` | :85-149 | 294 |
| `server/gateway/story.py` | `StoryAccumulator` | :243-283 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260923-m3-k6-boot-auto-connect | `firmware/main/test/boot_auto_connect.acceptance.c` |
| 260923-m3-k7-schedule-grid-restore | `server/acceptance/test_schedule_grid_restore.acceptance.py` · `firmware/main/test/schedule_grid_restore.acceptance.c` |
| 260922-m3-k5-sntp-time-sync | `firmware/main/test/time_sync.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `package:260924-m4-k1-story-cloud-link` | kidpal_session_ctx 信号双端实现（契约已 LANDED 零改动） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | front 拦截半 = K1（M4K1-A2）；板端发送半 = K3（M4K3-A1）；两端合并前互不阻塞（省略消息 = 普通会话向后兼容）。 |
| `package:260924-m4-k1-story-cloud-link` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | 单真机不两轮撞车——K1-A7/K2-A7 live 半全部挂 K3 设备轮一次性收口；K3 设备轮前 K1/K2 host 半须 merged。 |
| `package:260924-m4-k2-p3-star-notebook-ui` | mcp_server.cc story 路由面双写串行 | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 改形 story_list/story_text 消费 + 删除发送接线；K3 在其上加 stt 回显 + 录音 control + 引导态——mergeOrder 39→40 串行，K3 intake rebase K2 冻结面。 |
| `package:260924-m4-k2-p3-star-notebook-ui` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | K2-A7 真机目测半挂 K3 设备轮；K3 设备轮前 K2 host 半须 merged。 |
| `workstream:m4-star-notebook` | kidpal_session_ctx 信号双端实现（契约已 LANDED 零改动） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | front 拦截半 = K1（M4K1-A2，上行泵解析处拦截不下透）；板端发送半 = K3（M4K3-A1，protocol 原语 + application.cc 会话发起入口读 layer_top）；两端合并前互不阻塞（省略消息 = 普通会话向后兼容，契约既有语义）。 |
| `workstream:m4-star-notebook` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | 单真机不两轮撞车（M2/M3 先例）——K1-A7/K2-A7 live 半全部挂 K3 设备轮一次性收口；K3 设备轮前 K1/K2 host 半须 merged。 |
| `workstream:m4-star-notebook` | mcp_server.cc story 路由面双写串行 | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 改形 story_list/story_text 消费 + 删除发送接线；K3 在其上加 stt 回显消费 + 录音 control + 引导态——mergeOrder 39→40 串行，K3 intake rebase K2 冻结面、新齿对 K2 新缝写不改 K2 语义。 |
| `workstream:m4-star-notebook` | 删除发送原语复用（protocol 层 §四消息发送缝） | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 落 kidpal_story_delete 发送原语；K3 session_ctx 发送同族扩展（同一缝、不改 K2 冻结断言）。门 1 F6 聚合完备性修复：包级登记为准、workstream 级同步收录。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

