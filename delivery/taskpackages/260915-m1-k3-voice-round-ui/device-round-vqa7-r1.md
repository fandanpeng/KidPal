# device-round VQ-A7 · 第 1 轮（2026-09-17 human 持板，主线程串口取证；planner 转写）

板上固件字节级核实 = 8a3fc07 构建。结果：VQ-A4 打断手感 ✅；思考页弹出（VQ-A1）/ 回显（VQ-A2）/ 长文滚动问答框（VQ-A3）+ 骨架成页（VQ-A5 观感面）❌。

## 取证事实（串口日志 + 代码复读）

1. **链路通**：stt 文本到达（`Application: >> 世界上最高的树是什么？` @257212），tts sentence_start 两句到达（@269362 / @281252）。数据面无罪。
2. **思考态被顶掉**：stt 后仅 10ms（@257222）`StateMachine: listening -> speaking`——方向 B 网关在 stt 之后立即发 `tts start`（先于 LLM 出结果），冻结映射 LISTEN_AUTO: TTS_START→TALK 当场生效，THINKING 存活 10ms。第一句答案距 stt 12 秒。修复方向（**映射表零改动、application.cc 触发点移动**）：KVR_EVT_TTS_START 改在首个 tts sentence_start（或音频真正起播）喂入，使「开始播报」语义对齐实际起播时刻；tts start 帧不再触发场景切换。注意 VQ-A6 的 4s 安抚计时起点 = THINKING 入口，此修复后安抚窗口才真实存在。
3. **文字零上屏根因（高置信）**：lcd_display.cc SetupUI 活分支（~1020/1036）g_kidpal_voice_echo_label / g_kidpal_voice_answer_label 只设了 text_color 未设字体——LVGL 默认字体无中文字形，整段空白。对照同文件 keybar 三 label（~965/987）显式 `font_puhui_14_1` 且注释「全量字体： 键帽文案字形全覆盖 (basic 子集缺字)」。字号按 NFR-READ-1 下限选 puhui 全量字体对应型号。
4. **「两只猫」/ 骨架未成页**：单物理屏模型（kidpal_render_node）下语音轮期间前台内容仍是首页（HOME 画在 s_kidpal_scr，show_chat 只 push VOICE 层不动内容），姿态挂 chrome emoji_box_（96×96 居中）叠在首页猫上方；echo/answer 两容器 TOP_MID 挂 `screen`，被页面内容盖住。WF-B-01/02/03 的「左小伴 pose · 右内容区」整页版式未落成。修复方向：语音轮活跃期（VOICE 层前台 + mid_mode=1）把 echo/answer 容器挂到可见面（聊天 chrome 子树或语音轮专用内容面）、按 WF-B 骨架布几何（pose 左、内容右），并处理首页猫在语音轮期间的遮蔽（整页版式盖住或首页内容隐）——具体取舍按 UIUX-DESIGN-BRIEF §5 冻结稿，若冻结线框与单屏模型存在实现层冲突 fail-closed 上报。

## 交接

修复轮纪律照旧：verifier 现跑终验（host 零新增红 + 新齿绿），完成后汇报，主线程重烧板交 human 复测。
