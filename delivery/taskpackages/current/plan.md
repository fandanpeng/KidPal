---
changeId: 261014-engine-stop-truncation-asr-filter
title: 引擎 listen stop 收尾截断 + ASR 幻听串过滤——查字/播报收尾后引擎不再对尾音幻听出「不客气」寒暄
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect (/triage-change 分流 review；manual 豁免为承重发现，防止盲截断毁故事归档；human 触点① = 「顺势把它跟 engine-patch 一起做成一个小批，做吧」2026-10-14)
date: 2026-10-14
---

# 根因与来源

根因、证据链、代码锚点在 CRQ 条目内闭环，本 plan 不复制：
[TECH-261014-engine-asr-hallucination-after-stop](docs/crq/tech/TECH-261014-engine-asr-hallucination-after-stop.md)。
分流 review 核验后的机制修正（CRQ「修法方向」节的精确化）：

1. 病灶触发器 = listenMessageHandler stop 分支在非流式 ASR 下**直接触发**缓冲尾音
   整段识别（参考树 listenMessageHandler.py:38-54；Qwen3ASRFlash interface_type=
   NON_STREAM，部署版 0.9.6 与参考树同刻克隆）。stop 分支无 auto 路径既有的
   最短音频门（asr/base.py:79）。证据时序（stop :43 → 识别文本 :44 → LLM :44
   → TTS :48）逐帧吻合。
2. 正常对话轮 ASR 由服务端 VAD 触发（silero.py:106-109 → base.py receive_audio），
   不经 stop——截断 stop 触发路径不伤正常对话。
3. **manual 模式（故事录音会话）的 stop 触发 ASR 是 front pending-stop 归档的承重
   通道**（front_app.py:133-161：最后一句转写 ~1.7s 后经 stt 下行到达才归档）——
   截断必须按 conn.client_listen_mode != "manual" 豁免，否则 SAO-A2「转写被丢弃
   = 毁稿」回归。

本批同时闭 261014-r2 明确不做 #1（F2 残余：引擎侧「stop 后丢弃 in-flight audio」
缺失，设备侧无解）。

# 包边界

## 动

- `server/deploy/engine-patch/connection.py`（唯一实现文件，三处改动，见「修复」）。
- `server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py`（新增，
  循 test_kidpal_speak_sync.acceptance.py harness 先例）。

## 不动（显式零触碰面）

- firmware/** 零改动（CRQ 已核实设备侧无责：判据② 两轮正确触发）。
- wire 协议零变更：listen start/stop 为既有消息，本批只改引擎接收后内部处置
  （front 对 listen stop 是旁路消费、转发不变，story 归档链不受扰）。
- manifest.yaml / docker-compose.yml / deploy.sh 零改动：全部改动收在既有
  bind-mount 单文件（connection.py）内，不扩部署面。
- 镜像内上游文件（core/handle/**、core/providers/**）零触碰。
- manual 模式（故事录音）行为零改动——豁免是硬边界，见回归风险 2。
- `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`：零 diff。
- 邻接未结 CRQ 不并入：TECH-261014-session-end-channels-unwired（结束意图通道
  含 wire 取舍须 Track A，其「云端判结束」将来可复用本批截断标志作挂点，记输入
  不合并）、TECH-261012-edgetts-403-hotpatch 耐久化（留正规部署批）。

# 修复

## 改动点 1：listen 帧旁路截断（_route_message :418-420 区域）

新增 `_kidpal_on_listen_message(message)`（async，放 _kidpal_try_speak 同区），
在 kidpal_speak 认领之后、handleTextMessage 之前调用。解析 JSON（畸形静默返回），
type=="listen" 时：
- state=="start" → `self._kidpal_listen_closed = False`（新聆听窗，复位截断态）。
- state=="stop" 且 `self.client_listen_mode != "manual"` → 置
  `self._kidpal_listen_closed = True`；`self.asr_audio.clear()`；非阻塞排空
  `self.asr_audio_queue`（get_nowait 循环，循 clear_queues 先例）；记 info
  「kidpal stop-truncation: listen stop 收尾，丢弃在途音频」。

承重顺序 = 先清后转：handleTextMessage 的 stop 分支读到空 asr_audio →
len()>0 门不进 → stop 触发的 ASR 整体不发生。manual 模式不截断（故事归档
转写通道保护）。消息照常转 handleTextMessage，上游语义零改。

## 改动点 2：chat 入口守卫（chat :1116 顶部）

depth==0 且在 `self.dialogue.put`(:1127) 与 TTS FIRST 入队(:1128) 之前：

```python
if depth == 0:
    if self._kidpal_listen_closed:
        self.logger.bind(tag=TAG).info(
            f"kidpal stop-truncation: 丢弃收尾后 ASR 轮: {query}")
        return
    if _kidpal_is_asr_hallucination(query):
        self.logger.bind(tag=TAG).info(
            f"kidpal asr-hallucination filter: 丢弃幻听串: {query}")
        return
```

兜 stop 前 VAD 已触发的在途 ASR 轮竞态（~1s 窗）。**不置 client_abort**
（避免波及 stop 后 front 注入的合法 speak/story 下发），用自有标志，
`__init__` 状态区（:134 附近）初值 False。守卫仅 depth==0（递归轮不拦）。

## 改动点 3：幻听串表（模块级）

`_KIDPAL_ASR_HALLUCINATIONS = frozenset({"thank you", "thanks for watching",
"thank you for watching", "谢谢观看", "请不吝点赞"})`（种子，builder 定稿时逐条
带来源注记）+ `_kidpal_is_asr_hallucination(text)`：归一化（strip/lower/去
中英标点）后**整串相等**匹配，绝不子串。放 LLM 入口（chat）而非 ASR 结果入口：
单收口在 patch 文件内、离线可测；stt 下行与意图判定在 chat 之前、不受扰。

## 执行边界（human 已裁，随包留痕）

- 只 commit 到分支；**手工部署 ECS 测试**（复用 261014 engine-patch 误 await 修的
  手工通道：备份/ecs_put.py 上传/md5 对账/docker restart/日志核验，见部署节）；
  远端正规部署另行（manifest 已含 connection.py，届时零增量）。
- 部署前置机械核对：容器内 listenMessageHandler stop 分支形状（docker exec grep
  确认 `len(conn.asr_audio)` 门存在）——「先清后转跳过」依赖它，0.9.6 参考树
  已核验、部署时再核一次防镜像漂移。

# 验收要点

## host（相关子集，不全量）

1. `test_engine_stop_truncation_asr_filter.acceptance.py`（新，AST 抽取真实方法体
   + SimpleNamespace conn 替身，循 speak_sync harness）：
   - ET-a stop 截断（auto 模式）：flag 置位 + asr_audio 清空 + queue 排空。
   - ET-b **manual 豁免（承重齿）**：client_listen_mode="manual" 下同一 stop 消息
     → flag 不置、buffer/queue 不动——漏掉即故事毁稿回归。
   - ET-c start 复位；ET-d 非 listen/畸形消息零扰动零异常。
   - ET-e 幻听串表：三英文串命中（大小写/标点变体）、「谢谢你，这个字怎么写」
     「让我们一起说thank you吧」不命中（整串保护负锚）。
   - ET-f/ET-g 结构齿：chat 守卫在 dialogue.put 之前；_route_message 拦截在
     handleTextMessage 之前。
2. `test_kidpal_speak_sync.acceptance.py` 零改动回归绿（connection.py 邻改 canary）。
3. 相对有效基线零新增红。
4. all-mock 申报：本包含真机轮（human 手工部署后），不应 all-mock；若机械闸判
   all-mock 按既有纪律升 human 放行，不静默。

## 真机（判定标准；human 在场，串口捕获规程；手工部署 ECS 后）

1. **查字轮主验（defect of record 复现面，两轮）**：问「X 怎么写」→ 介绍播报 →
   引擎日志：`收到listen消息 stop` 后**无** `识别文本:`、无 `大模型收到用户消息`、
   无 `发送第一段语音`；设备无「不客气」追加寒暄；查字窗驻留不受扰。
2. **正常对话回归（最大误伤面）**：暖通道多轮问答 + 连续追问全程正常
   （VAD 轮 chat 不受守卫影响）；唤醒词入口回归。
3. **故事录音回归（manual 豁免判定面，必测）**：录音 → 讲述 → KEY 收尾 →
   归档正常（story_text/story_list 下发、pending-stop 转写到达）。
4. 提醒轮回归：提醒 speak 照常出音（speak 链不进 chat 守卫）。
5. 引擎日志核验：重启后既有 marker（display-strip patch active）+ 本批建议在
   _initialize_components 同区追加 marker 行「kidpal stop-truncation/asr-filter
   patch active」（部署生效锚）。

# 回归风险

1. **正常对话轮被误杀**（最大风险）：结构性不可达——正常轮不走 stop 触发路径
   （服务端 VAD）；chat 守卫只在 flag 置位后拦，flag 仅 stop（非 manual）置位、
   listen start 复位；ET-a/c 钉住。
2. **manual 豁免漏做/漂移**（毁稿级）：ET-b 承重齿 + 真机第 3 项双重钉。
3. **stop 前在途真实语音被丢**：用户恰在收尾瞬间说话则该轮不回——与设备侧
   r2 F2「stop 不可撤回」裁决同语义，接受并留痕。
4. 真实裸「Thank you.」被滤：失一轮回复、info 可观测，接受（中文优先儿童产品）。
5. 部署时点漂移：容器若在 commit 与手工部署间重建会回退镜像原版——部署时以
   md5 对账兜（既有通道）。

# 非目标 / 已知残余（不扩本包）

1. stop 前在途轮的 stt 下行已发出（竞态窗内聊天面板可能闪一条识别文本）——
   cosmetic，登记不修。
2. manual 模式尾音幻听串经 stt 下行进故事累积器——未观测到，登记不修。
3. 最短音频/能量判据（裁掉，见 plan 论证）。
4. 引擎 stop 语义上游回馈 xiaozhi-esp32-server。

# changeManifest（预定改动面，仓根相对）

- server/deploy/engine-patch/connection.py
- server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py（新）

# impactedTests 种子

- server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py（新）
- server/acceptance/test_kidpal_speak_sync.acceptance.py（canary，零改动回归绿）

# crossEnd 种子（planner 消费）

- 无 firmware 对应面（本批纯引擎侧，wire 零变更）；真机复验项为判定标准非改动面。

# 部署与回滚（主线程执行，human 在场）

- 手工部署复用 261014 误 await 修的实证通道：① commit 先行；② 备份 live 为
  **.bak-261014-pre2**（勿覆盖 .bak-261014-pre——误 await 修前回滚点）；③
  ecs_put.py 上传（MSYS_NO_PATHCONV=1）；④ 上传前核远端==当前基线（误 await 修
  版 45013aba）、上传后 md5 对账 == repo 版；⑤ docker restart（设备全断一次，
  已知 autoconnect 3 次即退出缺陷 TECH-261014-autoconnect-gives-up-silently，
  human 在场可按 KEY 重建）；⑥ 日志核验两 marker + 设备重连。
- 回滚 = 还原 .bak-261014-pre2 + restart + md5 复核；已知良好基线 = 当前 live。
- 远端正规部署（测试通过后另行）：push 分支 → deploy.sh 通道，manifest 零增量；
  届时顺带闭 TECH-261012-edgetts-403-hotpatch 耐久化。
