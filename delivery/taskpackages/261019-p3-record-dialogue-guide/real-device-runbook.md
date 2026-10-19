# Real-Device Runbook · 261019-p3-record-dialogue-guide

- 日期：2026-10-19 · 维护：integrator（收口相位落盘，执行归 S5 真机轮）
- 适用：RDG-F6 五项观测 + S2 live 齿前置 + S5 五 AC 全链联调
- 停等位：**status=awaiting-human:real-device**——本单全部步骤需 human 持板与真 OpenClaw 环境在位，agent 不代跑；到门即停是预期行为、非缺陷。

## 0. 前置条件（缺任一即停等）

1. 设备：ESP32-S3 RLCD 持板、烧录含本包改动的固件镜像（dev/pod-m2 本包 commit 后构建；`application.cc` / `story_page.*` / `view_router.*` / `mcp_server.cc` 均在镜像内）。
2. 服务端：front + gateway 跑本包 commit 后的代码；engine-patch（connection.py）已挂。
3. **真 OpenClaw loopback 18789 在位**（S2 live 齿硬前置）：`curl http://127.0.0.1:18789/v1/models` 通、OpenAI 兼容端点可出文本——离线态下引导/整理回合快失败是 EQ-4 预期，但本单验真实行为链必须真端点。
4. STT 通道可用（设备麦克风 → 引擎 ASR → stt 下行逐句帧）。
5. 家长端 front 可见列表页与 P3 入口（双入口验证用）。

## 1. S2 live 齿（真 OpenClaw loopback，无设备可先行）

用真 stt 序列（假 ws 双桩或 dev 前端注入）驱动，逐项记录实测：

| 步骤 | 操作 | 预期观测 |
|---|---|---|
| 1.1 | 进记录会话后注入逐句 stt（孩子讲述内容若干句） | 引擎日志出现 `kidpal record-context: 抑制 LLM 派发（记录语境闭嘴回显）`——**零 LLM 零 TTS 回合**；front 累积照常增长、设备逐句回显 |
| 1.2 | 停顿超过 STUCK_GUIDE_SECONDS（当前 8s） | sweep 触发引导回合——OpenClaw 真端点收到 guide-round.v1 制品 system 指令；返回非空则经 kidpal_speak 注入播报；返回空/异常则本轮不播报仍计轮（日志 story_guide_round suppressed） |
| 1.3 | 说收尾词「我说完了」 | 归档同拍下发 start_organizing 恰一帧 + 注入 active=false 出语境信号；OpenClaw 收到 faithful-organize.v1 制品回合，产出整理文本经 story_text(seq) 推送 |
| 1.4 | 整理回合中拔掉 18789 复跑 | 快失败归 None → 当轮素材不归档 + 固定安抚句「刚才的话我没能记下来，我们再讲一次好吗？」（R1-N4 口径，零新语义） |
| 1.5 | 非记录语境普通问答复跑一轮 | 引擎正常 LLM 派发 + auto stop 照常截断（261014 齿语义回归确认） |

## 2. RDG-F6 五项观测（human 持板，逐项打✅/❌并留实测注记）

| # | 观测项 | 操作 | ✅ 判据 |
|---|---|---|---|
| ① | VAD 真沿与 AFE/AEC | 记录会话中正常讲述 + 停顿 + 引导播报期间继续说话 | 引导播报期间设备播音不因孩子声音回灌而误判孩子说话（AEC 抑制回灌）；AEC 关路径（AutoStop 形态）回灌行为如实记录留观感判断 |
| ② | 引导播完继续录音不离记录会话 | 等卡壳引导播报播完 | 播完自动回录音聆听态、页态保持 RECORDING、逐句回显继续、**不尾随 auto-listen 回会话式问答**（TECH-261014-post-speak-listening-lingering 防回归 case） |
| ③ | 引导播报不跳页观感半 | 引导播报中观察屏幕 | 停留记录屏（帧 B 子态：气泡换引导语、左键帽「打断」右留空、回显区不清空）；无 THINKING 跳页、无闪烁 |
| ④ | state-organizing 帧真机可见（翻✅条件③） | 任一式收尾触发 start_organizing | 设备切整理过渡演出（「整理中…」+「你刚说」+ 回显内容在右区、键帽留空）；state-organizing 帧真实可见即为 S-F2.2 可行性翻✅条件③达成 |
| ⑤ | S-F1.4-AC6 录音期提醒排队不回归 | 记录会话进行中制造一条到期提醒 | 提醒不即时打断录音——排队延后到会话结束；引导注入（kidpal_speak）不以提醒帧形态弹窗；会话结束后提醒正常补达 |

## 3. S5 五 AC 全链联调（双入口起步 → 盖章收束）

1. **AC1 双入口同一记录语境对话态**：入口 A = 语音触发词进记录会话（REC_SESSION 入口）；入口 B = 语音说进入录音类指令走 control/start_recording（mcp case 2 置 kidpal_story_session_ctx_set(1)）。两入口均验证：AI 默认闭嘴（无 TTS 回应）、切句只逐句回显、不进思考跳页。
2. **AC2 卡壳轻引导**：① 停顿超阈值 → 引导一句（只基于已讲内容、不提供情节选项、不代编——human 主观判读引导语合规）；② 明示说「卡住了」→ 立即引导（不等计时）；③ 引导播报中按 KEY → 打断、页态保持、继续聆听；④ 引导播报中按 BOOT（短/双/长各一次分别试）→ **无操作、绝不丢稿**（毁稿防线真机复核）；⑤ 引导语 / 孩子中途提问句 / AI 回答均不出现在最终整理成篇（素材边界，最终从成篇核对）。
3. **AC3 盖章照旧**：整理成篇后走既有盖章流程，行为与 P3 既有版本一致。
4. **AC4 三式收尾 + 忠实整理**：收尾词 / 再按 KEY / 连续 2 轮引导无回应三条路径各自触发收尾——统一出 start_organizing 一帧、整理中按 KEY/BOOT 均不响应、成篇只用孩子原话（去口水/断句/分段/标题，不换表达不补情节——human 抽检忠实度）、新篇入列表置顶；另验「讲错啦」/BOOT 取消丢弃：不设恢复、出语境。
5. **AC5 自动收尾**：故意两轮引导都不回应（含一轮在收声抑制窗口内不说话）→ 第 2 轮计时到自动收尾成篇不丢弃（收声不停摆）。

## 4. 记录与升级

- 每步实测结果（✅/❌ + 现象一句话）回报 planner，聚合进本包真机轮结论；RDG-F6 五项打✅即翻 S-F2.2 可行性条件③与观测项闭环。
- 任一项❌：按现象归因（固件消费 / server 决策 / 引擎抑制 / 观感），交 planner 走 fixer 或升级；**观感类（③/手感）不现场调参**——归观感直修长环（plan 种子 8 包外留根）。
- 卡壳阈值手感（8s 起步是否合适）本轮只记录主观感受，不拍数；拍数走立项。
- 真机轮全绿后本包方具备 complete 面；合并门仍按 riskTier=high 走 supervisor 升级 human（本单不改变合并门路径）。
