# input-brief · acceptance-author · 260924-m4-k2-p3-star-notebook-ui

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-24T09:59:28.850Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> regenerated-because: 门1 F1/F3/F4/F5/F7 findings 吸收后 taskpackage 修订再生成（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3③` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§1.3④` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 251 | ok |
| `docs/requirements.md` | `F2` | F2 星星记录（原「故事记录」，2026-09-14 改名） | 320 | ok |
| `docs/requirements.md` | `§2.3` | 2.3 按键手势（3 键，无触摸；本节为权威键位定义） | 134 | ok |
| `docs/architecture.md` | `§5.5.4` | 5.5.4 UI 演出与现状 | 286 | ok |
| `docs/ui/kidpal-ui-design-v5.html` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/m4-star-notebook/progress.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/story_page.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/story_page.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_layer.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/mcp_server.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/key_gesture_parser.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/char_page.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/key_layer_dispatch.acceptance.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/test/acceptance/archive/story_p3.acceptance.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/art_assets.c` | — | 无锚点（整文件） | — | bare |
| `tools/asset-pipeline/gen_art_spiffs.mjs` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/story.py` | `list_stories` | :177-190 | — |
| `server/gateway/front/front_app.py` | `_relay_display_blocks_to_device` | :283-285 | 339, 353, 400, 537 |

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
| `package:260924-m4-k1-story-cloud-link` | story_text.data.seq（入册流水号 N 唯一权威源） | `260924-m4-k1-story-cloud-link` | `260924-m4-k2-p3-star-notebook-ui` | seq 语义与账本 owner = K1；板端盖章 N 消费承接 = K2（易失 stamp_index 退役负锚）。 |
| `package:260924-m4-k1-story-cloud-link` | 删除链路（kidpal_story_delete 载体 + front 执行 + 板端覆盖层与发送） | `260924-m4-k1-story-cloud-link` | `260924-m4-k2-p3-star-notebook-ui` | 载体契约与云端执行 owner = K1（M4K1-A4）；板端确认子态 + 发送接线承接 = K2（M4K2-A3）；live 复证挂 K3。 |
| `workstream:m4-star-notebook` | story_text.data.seq（入册流水号 N 唯一权威源） | `260924-m4-k1-story-cloud-link` | `260924-m4-k2-p3-star-notebook-ui` | seq 语义与账本 owner = K1（契约冻结 + story.py 持久账本）；板端盖章 N 消费承接 = K2（M4K2-A5，易失 stamp_index 退役负锚）。 |
| `workstream:m4-star-notebook` | 删除链路（kidpal_story_delete 载体 + front 执行 + 板端覆盖层与发送） | `260924-m4-k1-story-cloud-link` | `260924-m4-k2-p3-star-notebook-ui` | 载体契约与云端执行 owner = K1（M4K1-A4）；板端确认子态 + 发送接线承接 = K2（M4K2-A3，按契约冻结形状）；live 复证挂 K3（M4K3-A6）。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

