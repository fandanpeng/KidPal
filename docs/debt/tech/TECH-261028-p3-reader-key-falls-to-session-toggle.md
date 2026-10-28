---
id: TECH-261028-p3-reader-key-falls-to-session-toggle
title: P3 阅读态 KEY 短按落全局 SESSION_TOGGLE 开语音会话（违反 S-F2.3 AC2「不响应」），阅读态键帽槽同因缺位显示 (null)
domain: tech
status: deferred
severity: MAJOR
owner: 主线程（固件按键归属面 + 键帽裁决面，按包分流）
trigger: 下个动 P3 阅读态 / 板级按键路由 / 键帽裁决面的包；或 human 指定单独立包（/triage-change 起）
opened: 2026-10-28
closed: ""
closedBy: ""
paths: ["firmware/main/display/story_page.c", "firmware/main/display/story_page.h", "firmware/main/display/keybar.c", "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc", "firmware/main/key_gesture_parser.h"]
source: 2026-10-28 真机轮（验证 261028-p3-record-session-mirror-desync-fix 时的附带发现）——设备 KPLOG 08:23:09-08:23:30 逐事件还原 + 代码路由链核对（kidpal_story_key_owns / key_route_layer / keya_front_route_top）+ requirements §S-F2.3 AC2 对照
handoff: 登记后按 human 指示起 /triage-change；本条目与 261028 死锁修复正交，不阻塞该包合并
---

**现象**（human 真机原话，2026-10-28）：「回 list 页面后是录音状态（页面上方中间有 3 个点标志），长按 Key 推出，再长按回首页」。三个点是**语音聆听会话的 chrome**（不是录音态），30 秒内自行消失不了，须长按 KEY 或按 BOOT 关层。

**触发链（KPLOG 逐事件还原，2026-10-28 08:23）**：

1. 08:23:09 列表上 KEY 双击 → 进阅读态（`STORY_RENDER page=1`，正文画在 `KIDPAL_LAYER_SERVER_PAGE`=4 上，`chrome_front layer_top=4`）。
2. 08:23:13 KEY 短按 → `key_route_dispatch action=1(session_toggle)` → `show_chat: VOICE layer push` + `StateMachine: State: idle -> listening`，键帽变「说完啦/结束」。屏上 3 个点即此会话。
3. 08:23:30 KEY 长按 → `action=4(overlay_close)` → 关会话回列表。
4. 08:23:36 KEY 长按 → `action=2(home)` → `show_home` 正常执行（同批 261028 死锁修复已生效，零 `show_home suppressed`）。

**需求依据（冲突实锤）**：`docs/requirements.md` S-F2.3 AC2 逐字「Given 阅读中，When 按 KEY 短按，Then **不响应**（朗读已砍，v1.52）」；同节补充规则「阅读态口径 = 一义到底（纯阅读），**无会话/问答入口**」，并把「故事语境会话（按 KEY 带故事背景开会话）」列入已砍清单（2026-09-14 human 裁）。实现侧把阅读态的 KEY 短按送进了全局会话路由。

**根因（已钉到代码）**：

- `kidpal_story_key_owns()`（`story_page.c:2368`）三支只在 ① 录音态 + ctx、② 删除确认覆盖层、③ `front_layer == KIDPAL_LAYER_NONE` 且本地页为故事页 时归故事页所有；阅读态把正文画在 `SERVER_PAGE`（层值 4），因此 ③ 不成立 → 归属判据返回 0 → 板级 `keya_story_page_wants()`（`waveshare-s3-rlcd-4.2.cc:211`）放行到全局。
- `key_route_layer()` 的覆盖层列定义（权威注在 `test/key_layer_dispatch.acceptance.c:37-44`）：`overlay_top != 0` 时 `KEY CLICK -> GLOBAL_ACT_SESSION_TOGGLE`、`KEY LONG_PRESS -> GLOBAL_ACT_OVERLAY_CLOSE`。长按语义（关层回列表）恰好与「退出阅读」同向，所以只有短按这一格暴雷。
- 同因第二个症状：阅读态键帽应显「长按退出 / 翻屏」（`keybar.c` 的 `KSTORY_PAGE_READER` 槽），实机日志为 `KEYBAR ovl=1 page=2 substate=-1 last_known=0 left=(null) right=(null)`——键帽裁决同样走覆盖层通用分支，未取阅读态槽。

**影响面**：孩子在阅读态误按 KEY（短按）→ 麦克风开、进入聆听态、可能被引擎当会话起点；与「纯阅读、无问答入口」的产品裁定直接冲突。不影响列表态 / 录音态 / 其它 SERVER_PAGE 内容页（查字、课表答案页的 KEY 短按 = 开会话是既定语义，**不得**一刀切改全局表）。真实使用频率未测——本次为验证别的包时的附带发现，非 human 主动上报。

**修法候选（权衡留给 triage 判档）**：

- **甲案（最小）**：`kidpal_story_key_owns()` 增一支——`page_state == KSTORY_PAGE_READER && front_layer == KIDPAL_LAYER_SERVER_PAGE` → 归故事页；`KidpalStoryPageInput` 在 READER 页态把 `KEY CLICK` 映 `KSPV2_ACT_NONE`（吞手势、绝不外泄），`KEY LONG_PRESS` 仍落既有 `KSPV2_ACT_READER_EXIT`。同时给 keybar 的阅读态槽补上裁决入口（`substate=-1` 过渡窗取最后已知子态的先例 = 261022 chrome-ghost-fix 的 `kidpal_keybar_story_transition_slots`）。
- **乙案（彻底）**：阅读态从 `SERVER_PAGE` 改为本地页渲染（层栈 + 键帽 + 前台回落一并迁）。改动面大、会与 `kidpal_story_on_screen()` / `story_list_gate` 的「LOCAL_PAGE 前台」判据族相交，需重核 K2/K3 的冻结齿。
- **不选**：改 `key_route_layer` 全局表把覆盖层列的 KEY 短按一律吞掉——会打死查字/课表答案页的既定会话入口。

**验证方式**：真机阅读态①按 KEY 短按零反应（日志无 `action=1(session_toggle)`、无 `idle -> listening`）②长按退出回列表、再长按回首页③键帽显「长按退出 / 翻屏」。host 侧可落结构锚（归属判据新支 + 输入函数映射），运行时半同 261028 包须真机轮承载。
