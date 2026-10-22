---
id: TECH-261022-p3-record-session-state-churn
title: P3 记录会话状态演出族缺陷——键帽鬼影闪烁 / 成功路径鬼覆盖层吃按键 / 引导垄断与「说完了」竞态 / 记录语境幻听入素材 / 列表排序与敷衍标题（KPLOG005 真机轮实证）
domain: tech
status: blocker
severity: MAJOR
owner: 主线程（固件 chrome/键帽面 + front 引导与素材面，按包分流）
trigger: 已拍板即批处理——human 2026-10-22：登记后紧接走 /triage-change 分流起包（建议拆两包：固件键帽+覆盖层族一包；front 引导节奏+素材过滤+排序标题一包）
opened: 2026-10-22
closed: ""
closedBy: ""
paths: ["firmware/main/display/lcd_display.cc", "firmware/main/display/keybar.c", "firmware/main/display/kidpal_voice_states.c", "firmware/main/application.cc", "firmware/main/mcp_server.cc", "server/gateway/front/front_app.py", "server/gateway/story.py"]
source: 2026-10-22 真机轮三源取证（设备 KPLOG005 全量 + kplog_user_test_full 补段、front journalctl、引擎 docker logs）——13:02:24-13:03:20 一笔完整记录会话逐秒还原
handoff: 紧随登记起 /triage-change 分流；建议拆包——① 固件包：键帽鬼影 + 鬼覆盖层（lcd_display/keybar/render_node 面）；② front 包：引导节奏 + 素材幻听过滤 + 排序与标题（front_app/story 面）
---

**背景**：261022-diag-log-content 真机轮拉回 KPLOG005，对 13:02 一笔成功归档的记录会话（id `2026-10-22-s789d9474`）做逐秒还原，human 确认「日志里的状态乱跳与屏上所见一致」。本条收该笔实证出的五个相互独立、可分别修复的缺陷。整理回合 60s 超时修复（2160fef7）在该笔工作正常（整理 ~5s），本条不含超时面。

**缺陷① 键帽鬼影闪烁（human 观感「状态乱跳」本体，全笔 12 次）**
转场瞬间键帽条闪出**上一次语音会话残留的键帽**（聆听残留=「说完啦/结束」、思考残留=「不等了/结束」）约 300~500ms，随后页级键帽才盖回；同一事件常触发两次 chrome 同步 = 双连闪。最刺眼的实证：13:02:15/18/21 只是在阅读卡里翻看（**无任何语音会话**），仍三次闪出 75 秒前天气问答留下的「说完啦/结束」。
机制（已钉到代码）：键帽裁决 `kidpal_keybar_sync_locked`（lcd_display.cc:1353-1457）读渲染节点缓存的故事子态——层切换/语音事件触发的 chrome 重同步跑在故事子态缓存更新之前，读到 **-1**，裁决掉进通用分支 `kidpal_keybar_slots(page, g_kidpal_keybar_scene, overlay_top)`；而 `g_kidpal_keybar_scene` 在语音会话结束后**永不复位**，于是通用分支恒画出残留场景键帽。三因叠加：场景键帽残留 + 过渡窗读 -1 + 单事件双同步。
候选修法：语音会话收尾复位 keybar scene；故事会话期（录音/整理/阅读子态）键帽裁决对 -1 过渡窗取最后已知子态或硬门通用分支。

**缺陷② 成功路径鬼覆盖层白吃一次按键**
故事归档成功、列表已刷新后，覆盖层在层栈里逻辑上仍占前台：13:03:08「不等了/结束」鬼影闪两次，13:03:20 human 按 BOOT 被路由成 `action=4(overlay_close)` 而非列表页该响应的动作，`dismiss_front_overlay` 后才恢复。上午 09:31 那笔（旧版 front）同样形态（3480745 同 `action=4`），非新版回归。困死修复 4068dc6c 只给**失败**路径补了页面退出帧，**成功**路径无对应 dismiss。
候选修法：成功归档路径补显式覆盖层 dismiss，对齐 4068dc6c 失败路径形态。

**缺陷③ 引导播报垄断会话 + 收尾零缓冲，「说完了」恒成孤儿帧**
39 秒会话里引导 TTS 独播约 28 秒（19s + 9s 两段），孩子只说了 4 句；末段引导结束的**同一毫秒** front 下发 `control start_organizing`（设备日志 346462/346472 同毫秒），引导若问句收尾孩子连接话窗口都没有。孩子说「说完了」须经 ASR，方向 B 链路终结延迟实测 5~14 秒，而引擎静默计时器在引导结束后 0.5~1 秒即收会话——语音收尾信号**结构性永远迟到**（全天四笔笔笔如此），到达时 session ctx 已清 = 孤儿帧。
候选修法：front 引导节奏——单段引导篇幅上限、末段引导与 start_organizing 之间留应答窗；「说完了」类收尾词的语音通路要么接进收尾判据、要么引导话术不再暗示它有效。

**缺陷④ 记录语境 ASR 幻听直进故事素材（human 实证非本人声音）**
13:02:34 引导播报期间 ASR 幻听出「Okay.」（引擎日志 `识别文本: Okay.` + `kidpal record-context: 抑制 LLM 派发`）。引擎**对话面**设防有效（未触发 LLM 回话），但**素材面不设防**：STT 帧经 front `_story_feed_from_stt` 旁路进素材缓冲、经设备 `kidpal_story_echo_push` 进录音回显区。本次成文干净是整理 LLM 把噪声丢了（运气非设计）；261014 批的幻听过滤只覆盖「查字/播报收尾后尾音」缝，记录语境引导播报段不在其覆盖面。
候选修法：front 素材入口对引导播报期间（子态=1）STT 丢弃或降权标记；或引擎侧复用 261014 幻听串过滤并扩覆盖窗到记录语境。

**缺陷⑤ 列表排序与标题两个小件（human「找不到刚录的故事」实证）**
(a) 列表排序 = 日期倒序 + **同日按 id 字符串倒序**（story.py:406），id 后缀为随机散列不含时间序——13:03 新篇（s789d9474）排在 09:31 旧篇（s9074aa51）**之下**，「最新在最上」直觉不成立；`.seq.json` 账本有严格递增 seq（16/17）可作同日排序键。
(b) 标题 = 整理 LLM 输出首行 ≤12 字（front_app.py:296），无敷衍兜底——该笔 LLM 起了「我的故事」，在列表里无可识别性（对照：同日上午篇「糖葫芦」）。

**影响面**：①②③ 每笔记录会话必现，是 human 真机观感「状态乱跳/找不到故事」的直接来源；④ 内容污染风险（本次侥幸）；⑤ 可发现性损害。全部不动契约、不动 wire——固件 chrome 面与 front 应用面各自内部修。

**风险**：riskTier: medium——①② 在固件 chrome/键帽裁决面，有既有 acceptance 族（keybar/story 冻结件）兜底防回归；③④ 在 front 引导/素材面，需重跑记录会话冒烟；⑤ 排序键变更需对设备端既有列表渲染零扰动核验。
