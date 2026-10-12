---
id: TECH-261012-edgetts-403-hotpatch
title: Edge TTS 微软端全线 403（speech.platform.bing.com 2026-10 拒旧宿主）致设备语音全哑——容器热补丁切 api.msedgeservices.com 端点已恢复，耐久化（补丁进部署源）未落
domain: tech
status: deferred
severity: MAJOR
owner: 主线程/dev 批（耐久化补丁进部署源：engine-patch 载体或镜像 requirements 钉 edge-tts>=7.2.9+patch）
trigger: 下次动 server/deploy 引擎部署面的批顺带落耐久化；或容器重建/镜像更新时热补丁丢失即复现（可判触发）
opened: 2026-10-12
closed: ""
closedBy: ""
paths: ["server/deploy/engine-patch"]
source: 2026-10-12 真机轮三源对账：设备侧零语音（串口 c:/tmp/hv5-realtime-round.log）+ 引擎 Edge TTS 403 ×25（docker logs 13:30:38-13:31:11 每句 5 重试全败）+ rany2/edge-tts issue #491（2026-10-12 开，定位微软端变更）
---

**事件**：2026-10-12 08:36（最后一次成功）至 13:30:38（首次 403）之间，微软端 `speech.platform.bing.com` 开始对合成 WebSocket 全线 403（`Invalid response status`）——EdgeTTS 的 `Sec-MS-GEC` 令牌校验被微软侧作废（2026-10 起多国生效、10-12 起美国也挂，rany2/edge-tts #490/#491）。引擎每句语音 5 重试全败 → **设备零语音输出**（孩子问天气，答案卡在 TTS 层，与 261012-kidpal-realtime-facts 的 prompt 改动无关——该批未触 TTS 面）。最新发布版 7.2.8 同样 403。

**热补丁（2026-10-12 已生效）**：按 issue #491 修法，容器内 `edge_tts/constants.py` 的 `WSS_URL` 从 `wss://speech.platform.bing.com/consumer/speech/synthesize/readaloud/edge/v1?TrustedClientToken=...` 改为 `wss://api.msedgeservices.com/tts/cognitiveservices/websocket/v1?Ocp-Apim-Subscription-Key=...`（Edge 155 新合成通道，接受旧令牌走 Ocp-Apim-Subscription-Key 头；VOICES 列表仍在旧宿主，不动）——容器内探针出声 11232 字节、引擎重启加载后真机语音恢复（14:27 轮语音生成成功 ×8）。

**耐久化缺口（本条目主体）**：热补丁在容器层，**容器重建/镜像更新即丢失**。修法候选：① `server/deploy/engine-patch/` 载体加 constants.py 补丁（overlay 应用时落）；② 镜像构建 requirements 钉 `edge-tts @ fix 版`（等 #491 发版后钉 >=修复版）；③ 两手都要（patch 兜底 + 版本钉追新）。验收 = 容器重建后 TTS 探针仍出声。
