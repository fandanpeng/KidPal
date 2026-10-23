# fixer 轮补充 · D2 主线程证据链与根因假设（planner 2026-10-20）

> 本文件 = fixer 的根因假设输入，**不是结论**。fixer 须自行验证/修正后落自己的根因假设；与现场日志冲突时以日志为准。配合 `fixer-brief-r1.md`（缺陷现场包）一起读。

## 主线程已验证的机制事实（代码核对过，可直接采信）

1. **`kidpal_story_page_render` 每次调用 `lv_obj_clean(lv_screen_active())`**（story_page.c:1118），无任何渲染节点侧联动——它不知道（也不通知）render node 屏上对象被清了。
2. **`StoryRecTimerCb` 每 100ms 直调 `kidpal_story_page_render`**（mcp_server.cc:116-128），唯一守卫 = `g_story_view.page == KSTORY_PAGE_RECORDING`，**零前台/层/屏真相核**：哪怕别的路径已把屏重建成首页，定时器仍照画 RECORDING。
3. **render node 静态缓存 `s_kidpal_img` 悬垂窗口**：`rn_render_home_locked` / `rn_render_kidpal_page_locked` 自身 clean 后会置 `s_kidpal_img = NULL` 再重建（kidpal_render_node.cc:404/838），但 **story_page.c 的 clean 不经过这两处**——录音态渲染把 `s_kidpal_img`（首页图像控件）destroy 掉后指针不清，随后任何首页 tick（`kidpal_render_node_tick` 判据 `s_local_page==HOME && top==LOCAL_PAGE && s_kidpal_img != NULL`，kidpal_render_node.cc:1080）会拿悬垂指针走 `kidpal_home_p1_render_frame()` 触碰已释放对象。
4. **首页动画 tick 与录音演出定时器是两个独立 lv_timer**，都画 `s_kidpal_scr`；层栈/前台归属只在事件驱动路径（voice_begin/voice_end/show_home/ScheduleStoryRender 夹钳）维护，两个 timer 都不核前台。

## 现场 1 时序对账（kplog-full-261020.log 4864-4945 行区，与机制 1-4 逐条咬合）

| uptime | 事件 | 机制解释 |
|---|---|---|
| 39985 | control start_recording consumed=1 | ctx 置位（mcp_server.cc:1264）+ StartListening + ScheduleStoryRender（SERVER_PAGE 夹钳→渲染→story_local_front 回落 LOCAL_PAGE，解释 39995 chrome_front top=4→回落）+ StartStoryRecTimer |
| 40035-44615 | RECORDING tick 每 ~330ms 正常渲染（rec 0→5） | 录音演出定时器独占屏，此阶段无害 |
| 44615 | story_vad_recording_enter consumed=1 | 桥（mcp_server.cc:719-740）：voice_end **早退**（VOICCE 层不在栈上——无 `voice_end: top=` 日志）+ ScheduleStoryRender + StartStoryRecTimer（幂等） |
| 45115 | `render home 400x300` + 首页键帽 说话/翻页 | **某路径重建了首页**（三个候选：show_home / voice_begin(had_server) / voice_end else 分支；wrapper 尾日志缺失疑为 SD 日志缓冲在崩前丢失——不必钉死调用方，fixer 可在 host 侧插桩定位） |
| 45175 | STORY_RENDER page=2 rec=5 仍画 | 录音定时器对首页重建毫无感知，`lv_obj_clean` **销毁了 45115 刚建的 s_kidpal_img（指针未清）** |
| 45215 | `voice_end: top=2` | VOICE 层在 45115 前后被重新推入（voice_begin/show_chat），此刻弹出；top=2=LOCAL_PAGE；on_story_front 判 `s_local_page==KP_PAGE_STORY`——45115 的首页渲染已把 s_local_page 翻成 HOME → else 分支**再渲染一次首页**（s_kidpal_img 重建） |
| ~46.0s | 崩（无 backtrace；SD 只捕 esp_log，panic 直出 UART） | 录音定时器下一拍 clean 再毁 s_kidpal_img（仍未清指针）→ 首页动画 tick（1s 周期）用悬垂 s_kidpal_img 渲染 → use-after-free。崩点时序吻合 |

现场 3（ORGANIZING，68335 首页键帽插一帧 → 1s 后崩）同型：整理态渲染与首页重建互砸，同一条悬垂链。

## 根因假设（fixer 验证/修正的对象）

**直接根因** = 单一前台屏 `s_kidpal_scr` 上两套互不知情的渲染系统（录音演出 lv_timer × 首页重建/首页动画路径）竞争，`kidpal_story_page_render` 的 `lv_obj_clean` 与 render node 的静态控件指针（`s_kidpal_img`）无生命周期联动 → use-after-free。
**触发根因** = 记录语境守卫缺口：会话覆盖页渲染期间，任何首页重建路径（服务端 home 推送路由 / 语音轮收尾 voice_end else 分支 / 语音轮重启 voice_begin(had_server)）都无记录语境守卫，可与录音会话并发发生。
**为什么零 backtrace** = panic 输出直出 UART 绕过 SdLog（brief 已记，独立改进项不本修）。

## 最小补丁面建议（方向，fixer 自行裁剪）

1. **生命周期联动（承重）**：`kidpal_story_page_render` clean 后通知 render node（新增 extern C 缝，如 `kidpal_render_node_notify_screen_cleaned()`，在 kidpal_render_node.cc 内把 `s_kidpal_img` 置 NULL——C/C++ 混编按 story_page.c 现有 include story_page.h 的接线惯例做；host 侧 weak 实现可空）。凡 clean `s_kidpal_scr` 的路径必须全部收口此联动，不留第二条悬垂窗。
2. **录音演出定时器真相核（承重）**：`StoryRecTimerCb` 渲染前核「录音页真的在前台基层」（如 `kidpal_render_node_get_page()==KP_PAGE_STORY` 且层栈 top==LOCAL_PAGE 且无覆盖层；核判据住纯逻辑缝供 host 齿），不满足则跳过本拍渲染（演出断一拍无害，UAF 致命）。不满足时**不要**代为重建录音页（重建也要走单点权威）。
3. **记录语境收尾守卫**：`kidpal_render_node_voice_end` 在记录语境（`kidpal_story_session_ctx_active()` 且 page ∈ {RECORDING, ORGANIZING}）不得走 else 首页重建分支——保持 P3 会话页前台（[R5] 判据消费点语义不变，on_story_front 判据放宽为「记录语境且会话页在世」而非仅 s_local_page，s_local_page 被中途翻页时也能回正）。`show_home` 两处服务端路由入口（mcp_server.cc:1223/1236）与板级入口在记录语境记录中降级 no-op + W 日志（屏上真相 = 录音页，不该回首页）。
4. **语音轮挂住（语义面，按 [G6]/VR-A2 既有守卫风格最小改）**：tts stop 出口三守卫（application.cc:702-721）在记录语境下应回聆听（会话挂住），不按普通问答收 round。注意现场 1 里引导 TTS 期间无态机迁移日志——语音轮生命周期在记录语境的实际形态先在 host 侧核实再动，勿盲改状态机。

现场 2（首页静置自崩）：证据不足，**不强行归因**、不为其改码；报告里如实记悬案（若补丁 1 收口全部 clean 路径后该场景也可能被顺带覆盖，如实申报即可）。

## host 复现与取证

- 优先 host_app：录音定时器 × 首页重建的竞态在 host（LVGL 未接真实屏）下难以 1:1 复现 UAF 时序；**结构性锚点可 host 断言**：补丁 1 的联动缝（clean 后 s_kidpal_img==NULL）、补丁 2 的真相核判据（伪造「s_local_page 被翻」场景，断言定时器跳渲染）、补丁 3 的守卫判据（记录语境 voice_end 不出首页重建分支）。放 `firmware/main/test/*.test.c`（**不碰 `*.acceptance.c`**——RDG-A/F 冻结件与 acceptance 目录对 fixer 机械禁写，新增验收齿走 acceptance-author 后续通道，本轮不代写）。
- 真机复现需持板：到门即停，status=`awaiting-human:real-device`，不要自行烧板。
