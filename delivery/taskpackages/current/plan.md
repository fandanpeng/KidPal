---
changeId: 261001-m4-k3-record-session-live-repair
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect
date: 2026-10-01
spine: delivery/changes/intake/261001-m4-k3-record-session-live-repair.md
---

# TaskPackage plan · 261001-m4-k3-record-session-live-repair（Package A：纯固件修复）

> 本件 = `/triage-change` 分流后的 **Package A**，architect reviewed（2026-10-01），human 触点①确认 ③ 拆包路由后落盘。
> **本包只含纯固件面缺陷**（D1 / D2 层栈半 / D4a / D4b）；D3（语音收尾「说完了」）因触需求/架构/契约三面，已拆为 **Package B**，走契约门 + `/arch-change` Track A，其实现面冻结至 spec/contract anchors 回填——见 spine facePlan 顺序3。
> 证据来源：串口被动捕获 `c:/tmp/p3-smoke-261001.log`（COM5，含 LVGL panic 与 backtrace；ELF SHA `0422fa290` 与本机 `firmware/build/xiaozhi.elf` 逐字一致，backtrace 已符号化）+ ECS 引擎 `docker logs` + front `journalctl`。

## 0. 一句话

按 KEY 进记录会话后，会话层（VOICE）始终压住故事录音页——这既让孩子看不到录音页（D2），又让板级「故事页优先」的键路由失效（其判据要求无覆盖层在前台），KEY 因此落到全局会话 toggle 上，走了代码里明令禁止的「关通道不发声 → front 走 ws_disconnect 丢弃」毁稿路径（D4a）；录音态未被复位又让录音演出定时器继续在已重建的首页上清屏，把首页图像对象打成悬垂指针并崩溃重启（D4b）。列表缺陷（D1）是延迟推送缺消费方。

## 1. 缺陷清单

### D1 — P3 星星记录本无列表（切页即空态）

- 现场：串口 t=18845 `render page STORY (P3) count=0`（空缓存走友好空态，`kidpal_render_node.cc:807-819`）。
- 推送时机现为三处：ctx(story) 沿 / 归档成功后 / 删除执行后（`server/gateway/front/front_app.py:185-198`、`533-536`、`199-202`）；**建连即推已于 260928-story-connect-push-defer 摘除**（`front_app.py:976-982`，human 方向锚 b036f554）。BOOT 翻页进 P3 不触发任何推送。
- 唯一一次推送到达时被板端 gating 挡下：t=40045 `story_list_dispatch items=1 counter=1` + `story_list_gate defer_front page=0 voice_front=1`（`firmware/main/mcp_server.cc:1041-1066`）。
- defer 分支**只装缓存**，注释称「下次前台回落时以最新缓存渲染」（`mcp_server.cc:1062-1063`），但代码中**没有该触发点**——与既有 `kidpal_story_list_gate` 验收齿同族的「纯逻辑函数无驱动方」（`record_session_live.acceptance.c:864-875`）。
- 修法方向：给 defer 缓存补消费方——在「VOICE 层弹出 / 前台回落到 LOCAL_PAGE 且本地页 == P3」时以缓存重渲（倾向前者：不动 wire、不动云端推送时点）。

### D2 — 按 KEY 落声波/聊天层，看不到录音页（**本包只做层栈让位**）

- 现场：t=39745 `story_key rec_session_slot page=0` → `show_chat: VOICE layer push (top=3)`（`mcp_server.cc:234-258` 的 REC_SESSION 位）；t=40275 `story_vad_recording_enter consumed=1 page=2`——录音页**已渲染**（`KidpalStoryVadEnterRecording` → `ScheduleStoryRender`，`mcp_server.cc:369-377`），但 VOICE 层仍在前台，录音页被压在下层。
- 根因：K3 所述「开口切录音态」（`application.cc:339-353`）只切**视图页态**，未让 VOICE 层让位。
- **范围钉死（architect finding）**：本包只交付「进录音态时 VOICE 层让位 + 录音页与『● 录音中』可见」；**不交付逐句回显内容**——回显内容依赖 stt 帧，属 Package B（D3-甲）域。验收矩阵相应不提「回显逐句出现」。

### D4a — 按 KEY 停录被丢弃（毁稿路径）

- 键路由前置判据要求**无覆盖层**才交故事页：`keya_story_page_wants()` 仅当 `overlay_top == 0 && page == P3` 返回 true（`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc:197-206`）。
- 录音时 VOICE 层在前台 → 判据不成立 → 手势落全局路由：t=116965 `key_route_dispatch action=1`（= `GLOBAL_ACT_SESSION_TOGGLE`），**未出现**预期的 `story_key stop_rec archive_pend=`。
- 该 toggle 在 Listening 态走关通道路径，正是 K3 标红的毁稿负锚（`mcp_server.cc:326-338` 注释）；现场吻合：WS 断开（t=116965-116975），front 断开钩子 `acc.cancel(session_id, reason="ws_disconnect")` 丢弃累积（`front_app.py:987-994`）。
- 修法方向：把「记录语境会话中的录音态」纳入故事页手势优先判据闭集；**D2 与 D4a 必须同一设计决策内一致选定**（见 §3 决策点）。

### D4b — use-after-free panic（崩溃重启）

- 崩溃现场（t≈117715）：
  ```
  Guru Meditation Error: Core 0 panic'ed (LoadProhibited). EXCVADDR 0x130
  lv_obj_get_screen ← lv_obj_get_display ← lv_obj_invalidate_area ← lv_obj_invalidate
    ← lv_image_set_src (lv_image.c:155)
    ← host_infra_render_p1_frame (kidpal_render_node.cc:184)
    ← kidpal_home_p1_render_frame (home_p1.c:1333)
    ← kidpal_render_node_tick (kidpal_render_node.cc:986) ← lv_timer_handler
  ```
- 发生链：会话收尾只清 `kidpal_story_session_ctx` 与 `s_story_rec_entered`（`application.cc:970-974`），**不复位 `g_story_view.page`**（该页态只在键输入 / `kidpal_story_on_control` 里变，`story_page.c:1513/1554/1562/1591`）→ 录音演出定时器判定 `page != RECORDING` 恒不成立、**不自删**（`mcp_server.cc:111-116`）→ 其每 100ms 的 `kidpal_story_page_render`（`mcp_server.cc:121`）**不夹 begin/end、不通知渲染节点**，而该函数内部 `lv_obj_clean(scr)`（`story_page.c:1087`）会销毁 `voice_end` 刚重建的首页图像对象（`kidpal_render_node.cc:566-589`），却**不置空 `s_kidpal_img`**（仅 `rn_render_home_locked` / `rn_render_kidpal_page_locked` 置空：`kidpal_render_node.cc:355-356`、`751-752`）→ 下一拍 tick 守卫（`kidpal_render_node.cc:969-972`）三者皆真 → 对悬垂对象 `lv_image_set_src` → panic。
- **修法三修并行、缺一不可**：
  1. 会话收尾复位录音页态并停录音定时器（Idle 分支或 `kidpal_story_session_ctx_set(0)` 消费侧），不依赖页态自删；
  2. 录音演出渲染走渲染节点认可的路径（夹 begin/end 或经 `rn_render_*`），保证「清的屏」与「记的指针」同源；
  3. `host_infra_render_p1_frame` 加对象有效性守卫（LVGL 9.3 提供 `lv_obj_is_valid`，`managed_components/lvgl__lvgl/src/core/lv_obj.c:410`）。

## 2. changeManifest（本包）

- `firmware/main/mcp_server.cc`（录音定时器生命周期与渲染路径；story_list defer 消费方；REC_SESSION 位）
- `firmware/main/application.cc`（Idle 收尾复位；VAD 桥进录音态的层栈动作）
- `firmware/main/display/kidpal_render_node.cc`（tick 守卫 / 图像对象有效性；前台回落重渲钩子）
- `firmware/main/display/story_page.c`（如需暴露录音态复位 / 渲染安全入口）
- `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（故事页手势优先判据闭集）

**不在本包**（architect finding 已从原草稿收缩）：
- `server/gateway/front/front_app.py`、`server/gateway/story.py` —— 随 Package B（D3-甲）拆出；
- 引擎 overlay model_name 冲刷修复 —— 另立独立小包（companion spine `261001-openclaw-model-name-overlay-drift.md`）。

## 3. 待定设计决策点（实现前须在包内钉死）

**D2 层栈让位形态二选一**（须与 D4a 的 `keya_story_page_wants` 闭集修改**一致选定**）：
- 甲：进录音态**弹出 VOICE 层** → 基层无覆盖层，D4a 判据天然成立（改动小、语义清晰）；代价：聆听态的表情/状态 chrome 期间不可见。
- 乙：把录音页**提到 VOICE 之上** → 须显式给 `keya_story_page_wants` 补「VOICE 前台 + `kidpal_story_session_ctx_active()` + page == RECORDING」分支；
- 无论取哪支，须补 host 侧可机判齿（见 §4）。

## 4. 验收方案

可机判（host 侧新增/加固齿）：
- 录音语境下的键手势归属：录音页态 + ctx active 时 KEY 必产 `KSPV2_ACT_STOP_RECORD`，**绝不产** `GLOBAL_ACT_SESSION_TOGGLE`（D4a 正/负双齿）。
- 会话收尾后录音定时器必停、页态必回 LIST（D4b-1）。
- 渲染不变量：任何外部 `kidpal_*_page_render` 清屏后，渲染节点不得以悬垂对象出帧（D4b-3，可用 `lv_obj_is_valid` 桩件）。
- defer 缓存消费方存在性（D1，沿用 `kidpal_story_list_gate` 已有「无消费方」反齿形状）。

真机复验矩阵（单窗口覆盖，串口 + 引擎日志双源对齐）：
1. BOOT 翻页进 P3 → 列表须显示后端已有条目（D1）；
2. P3 按 KEY → 须看到录音页与「● 录音中」，**不再是声波层**（D2；回显内容不在本包验收面）；
3. 录音中按 KEY（KEY 兜底路径）→ 正常归档保存、**不崩溃不重启**，`story_key stop_rec archive_pend=` 与 `listen/state=stop` 双日志可见（D4a+D4b）；
4. 回归：唤醒词问答、P2 课表、P3 删除子态不受影响。

**明确不在本包验收面**：语音收尾「我说完了」（D3，随 Package B）；逐句回显内容（随 Package B）；120s 静默自动归档路径（本轮窗口 < 120s 未覆盖，另行单验）。

## 5. 附件与引用

- spine：`delivery/changes/intake/261001-m4-k3-record-session-live-repair.md`（faceMap / facePlan / openQ）
- 前序包：`delivery/taskpackages/260924-m4-k3-record-session-live/`
- 真机捕获：`c:/tmp/p3-smoke-261001.log`
- 设备固件基线：`xiaozhi` 2.1.0，构建 2026-09-23 12:52:24，ELF SHA `0422fa290`
