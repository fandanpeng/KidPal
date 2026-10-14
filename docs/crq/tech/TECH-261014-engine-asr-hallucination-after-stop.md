---
id: TECH-261014-engine-asr-hallucination-after-stop
title: 引擎在收到 listen stop 后仍处理尾音、ASR 于静音/杂音上幻听（典型 "Thank you."），LLM 顺口回复并被推成 TTS——表现为查字/播报结束后设备多播一段「不客气呀」之类无关寒暄
domain: tech
status: deferred
severity: SHOULD
owner: dev 批（server/deploy 引擎部署面）
trigger: 下一次动引擎部署面（server/deploy / engine-patch）的批顺带；或提醒/播报链路的可靠性小包
opened: 2026-10-14
paths: ["server/deploy/engine-patch/connection.py", "（引擎侧上游 core/handle/ · core/providers/asr）"]
source: 2026-10-14 真机联测（human 目击「播完字的介绍后多出一段『不客气……』」+ 串口 /c/tmp/live-261014-round2-r2.log + ECS 引擎容器日志同刻，两轮逐字命中：14:27 游字轮 / 14:38 蓝字轮）
handoff: 引擎部署面独立项；与 TECH-261014-post-speak-listening-lingering 的「查字收尾」设备侧已修不同面（本条是引擎侧收尾后行为）
---

**现象（2026-10-14 真机联测 human 目击）**：查字播报（「蓝」字介绍）结束后，设备额外播出一段无关寒暄（「不客气呀」+「还有哪个字想写，随时来问我」）。human 问「这段是哪里来的」。

**逐字证据（设备串口与 ECS 引擎容器日志同刻，两轮同模式）**：

```
14:38:43 [textMessageProcessor]-INFO-收到listen消息：{"type":"listen","state":"stop"}     ← 设备侧判据② 的正常收尾（SendStopListening）
14:38:44 [providers.asr.base]-INFO-识别文本: Thank you.                                  ← stop 之后 1 秒，引擎仍产出 ASR
14:38:44 [core.connection]-INFO-大模型收到用户消息: Thank you.
14:38:48 [sendAudioHandle]-INFO-发送第一段语音: 不客气呀                                    ← LLM 回复被推成 TTS
14:38:54 [textMessageProcessor]-INFO-收到listen消息：{"type":"listen","state":"stop"}     ← 设备对这段播报再次正常收尾
```

同模式第二轮（「游」字）：`14:27:07 listen stop` → `14:27:08 识别文本: Thank you.` → `14:27:12 发送第一段语音: 不客气`。

**根因（两层，均为引擎侧）**：

1. **stop 后仍处理在途/尾音**：设备已发 listen stop（且设备侧状态已回 Idle、不再喂新音频），引擎仍在 1 秒后完成一轮 ASR 并把结果送 LLM——即 `stop` 未截断正在进行的识别链、也未丢弃其产出。
2. **ASR 静音幻听**：产出文本恒为 "Thank you."（英文）——Qwen/Whisper 系 ASR 在低能量/静音段的已知幻听模式（静音 → "Thank you."/"Thanks for watching" 一类）。故这条链的输入本质是「停止后的静音」，却被当成一句用户话。

**设备侧行为核实（无责，逐条对照）**：判据②（查字窗前台 → `SendStopListening` + Idle）两轮均正确触发（设备串口 `speaking -> idle` 两次、查字窗 `layer_top=5` 全程驻留）；设备对引擎推来的 TTS 照常播报属协议正常路径。**本条修法不在设备侧。**

**修法方向（引擎侧，归包计划期裁）**：① **stop 语义截断**——收到 listen stop 即取消/丢弃该会话正在进行的 ASR→LLM→TTS 链（含已排队未发出的 TTS）；② **幻听过滤**——对短时/低能量音频的 ASR 结果做置信度或已知幻听串表过滤（"Thank you." 等）；③ 两者都做（推荐：①治链、②治因）。需核引擎上游是否已有 stop 相关钩子可挂（`core/handle/textMessageProcessor` 的 listen 分支）——**若需在设备与引擎间新增/改变消息语义即触跨系统结构性升级判据，走 Track A + 契约面评估**（当前判断：①②均为引擎内部行为，不动 wire，但排包时由 architect 复核）。

**影响面**：查字/任意播报轮结束后多播一段无关寒暄——用户感知为「它自己又说话了」，且延长了本该结束的交互；与 TECH-261014-post-speak-listening-lingering 同族的「AI 插话」体验问题（那条是设备侧滞留聆听，本条是引擎侧幻听回复）。

**风险**：riskTier 预估 low-medium——①若做 stop 截断需谨慎（勿误伤正常多轮中迟到的合法结果）；②过滤串表需可维护（避免把真实 "thank you" 误杀）。真机验法 = 复现查字轮 + 观察引擎日志无 stop 后 ASR 产出。
