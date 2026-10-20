# 真机轮 R1 缺陷现场包（261019-p3-record-dialogue-guide，2026-10-20 09:49-09:54）

> 来源 = human 真机轮（部署后第一轮）。pod = blocked（merge-gate + real-device 双门停等中）。本文件 = fixer 轮的 ErrorEnvelope 输入（三源对账已齐，含两个独立缺陷族）。

## 缺陷族 D2 · 固件三次静默崩（BLOCKING，挡全部真机验收）

### 现场 1（09:50:05，boot W —— human 主诉「进录音页面，然后就崩了」）
- 会话流（引擎对账）：09:49:50 ASR「我要讲故事」→ LLM 回「好呀」；front 检出意图 → 09:49:59 下发 `control start_recording`；09:50:00 生成记录语境引导语「星星要讲故事啦，我这就准备好听你说～」（human 主观「没有引导语」——实际已下发，疑被崩截断或未被感知）。
- 设备日志（KPLOG001.TXT，全量已取回 `/c/tmp/kplog-full-261020.log`）：
  - 39985 `MCP: control start_recording story_consumed=1 page=2` + Abort speaking + speaking→listening
  - RECORDING 渲染正常跳动 5 秒（`RECORDING render wave rec_seconds=2..5`，每 ~330ms 一 tick）
  - 44615 `story_vad_recording_enter consumed=1 page=2`
  - **45115 `kidpal_rn: render home 400x300` + KEYBAR 首页键帽 `left=说话 right=翻页`**（录音会话中渲染了首页！）
  - 45175 RECORDING 渲染 tick 仍在画（rec=5）
  - 45215 `kidpal_rn: voice_end: top=2`（语音轮在引导语 TTS 播完时被终结——记录语境下语音轮**本应挂住**）
  - **崩**（09:50:05.6，无 backtrace）
- 签名：会话覆盖页渲染中穿插 home 渲染 + voice_end，~1.3s 后崩。疑似 = 记录语境聆听形态切换后，TTS 结束 → 语音轮按普通问答语义收 round（voice_end → home）与 story_page RECORDING 渲染 tick 竞态/use-after-free。architect 评审点名的 kvr 场景守卫（application.cc:776 ctx 选取 × 记录语境标志联挂）与聆听形态切换（ManualStop→非 manual）是高危区。

### 现场 2（09:51:53，boot X —— 首页静置自崩）
- boot X 09:50:05 重启 → 09:50:15 ws 重连（session 25fee1a1）→ home restore 后**零输入零下发**（front 同期引导轮 LLM 失败被抑制、未发任何帧）→ 09:51:53 自崩。无触发点、无 backtrace。悬案——可能为残留状态看门狗或独立缺陷，fixer 勿强行归因。

### 现场 3（09:53:02，boot Y —— 整理态渲染后 1 秒崩）
- 09:53:01 `MCP: control start_organizing story_consumed=1 page=6` → STORY_RENDER page=6 渲染成功 → **68335 首页键帽又插一帧**（与现场 1 同型）→ 崩。
- 同型竞态：整理态页渲染 + 安抚句 TTS（「刚才的话我没能记下来」×3 重复下发——front 三次 push_err 重试所致，见 D1 顺带项）播完 → voice_end → 崩。

### 无 backtrace 的根因（要记）
三次崩全部**零 panic backtrace**——SdLog 只捕 esp_log 钩子，panic 输出（Guru Meditation）直出 UART 绕过了 SD 日志路径。SdLog 补 panic 捕获（panic handler 钩子 / coredump-to-SD）= 独立改进项，不在本 fix 范围，登记待办。

## 缺陷族 D1（已修，hotfix 已部署，待 fixer 收编）
- `server/gateway/front/front_app.py:131` `_openclaw_chat_once` 请求**不带 Authorization 头** → OpenClaw 18789 401 → 引导/整理 LLM 回合全部 `story_llm_round_unreachable err=HTTPError`（front journalctl 09:51/09:52/09:53 各一）。token 有效（secrets.env OPENCLAW_TOKEN 直 curl = 200）。
- 已修（+Authorization Bearer 头，token 源同 provider.py:76 的 OPENCLAW_TOKEN env）+ 已部署 ECS（front active，旧文件 .bak-261019-dialogue-guide）。**工作树未 commit**——fixer 核后随 fix commit 收编。
- 失败兜底按设计正确触发：2 轮抑制照常计轮 → auto_finish → organize_failed → 安抚句 + start_organizing。R1-N4 语义实证通过。
- **验收覆盖缺口（要记给 acceptance-author）**：RDG 全部齿只断言「LLM 失败/空 = 仍计轮」的降级路径，无任何一条断言**成功的引导/整理回合**端到端（含 auth）——这就是 401 能穿过 verifier 的原因。fixer 修完 D2 后，验收收紧走 acceptance-author 通道，不属于 fixer 改验收的权限。

## 顺带项（小，可同轮带）
- front 在 device ws 断开时 `story_start_organizing_push_err` 重试 3 次导致安抚句 TTS ×3 重复下发——推帧失败重试无去重，酌情评估（非 BLOCKING）。

## 缺陷族 D3 · 固件 P3 会话两页未按 v7 设计稿渲染（BLOCKING，human 2026-10-20 真机反馈「录音页面也不是我们设计的页面」）

builder 按验收 GAP-2 申报只落了结构缝、视觉保真未落——现状（story_page.c 已核）与 v7 规格差：

- **录音中（帧 A，KSTORY_PAGE_RECORDING :1324-1380）**：无 pose-record 猫咪；正常录音子态无气泡（v7 = 「你讲吧，我听着呢」气泡 left:24/top:28 猫头顶上方）；REC 指示 = 24×24 实心黑方块（v7 = ● 12px 闪点 + REC 文案，180/18）；大时长/声波/回显全部 x=16~60 左列旧布局（v7 = 右区 180 列）；键帽 = 语音轮冻结闭集「说完啦/结束」（v7 = 左「收尾」右「丢弃」，破坏性提示）。
- **引导播报中（帧 B，rec_substate==1 :1370-1378）**：气泡画在 (16,8) 且键帽「打断」直接 label 画 (16, LV_VER_RES-24)——位式与 v7（气泡 24/28、键帽条左「打断」右留空）不符。
- **整理中（帧 C，KSTORY_PAGE_ORGANIZING :1381-1399）**：只有「整理中…」(16,40) + 「你刚说」+ 回显行——无 pose-think / 三问号组 / 波浪线 / 云朵 / 气泡「我来整理一下…」(24,28)；真机崩前日志 `KEYBAR ovl=1 left=说完啦 right=结束` 证明键帽路由也没切（v7 = 槽留空非交互）。

**改法权威** = `docs/ui/kidpal-ui-design-v7.html` P3 节三帧 + 规格表（坐标逐帧在案）+ `docs/ui/V7/` 三帧 PNG（布局坐标定位权威）；pose 资产 = /art 六姿态图集（pose-record / pose-think 已在库，渲染先例 = 语音轮四态 apply_look）。键帽路由：记录语境各页态/子态切 v7 键帽集（收尾/丢弃/打断/留空），不得再落语音轮闭集文案——注意「说完啦」属 kidpal_voice_states 冻结闭集、只在语音轮四态用，记录语境不是语音轮态。
**验收**：host_app 渲染分支/文案/键位齿可加（结构锚型，对齐现有 RDG-F* 风格）；视觉终判 = 真机看屏（human 观感轮，闸链后补账）。

## 日志与证据路径
- 板端全量日志：`/c/tmp/kplog-full-261020.log`（382KB，现场 1 在 4864-4945 行区、现场 3 在 67605-68335 行区——以 uptime 时间戳为准）
- front：ECS `journalctl -u kidpal-front --since '09:50'`（story_llm_round_unreachable / story_guide_round suppressed / story_auto_finish organize_failed / push_err ×3）
- 引擎：ECS `docker logs --since 60m xiaozhi-esp32-server`（识别文本/大模型收到/发送第一段语音/kidpal_record_context）
