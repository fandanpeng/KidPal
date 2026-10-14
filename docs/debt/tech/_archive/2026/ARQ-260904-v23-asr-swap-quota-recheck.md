---
id: ARQ-260904-v23-asr-swap-quota-recheck
title: V23 ASR 换型 + token-plan 配额重置后补验（流式/VAD 切句兼容性与引擎 provider 可行性）
domain: tech
status: closed
severity: SHOULD
owner: ""
trigger: "token-plan 配额重置（2026-09-07 14:19 UTC）+ 阿里云 ASR 凭据开通后补 live 实测与换型拍板"
opened: 2026-09-04
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: []
source: 260903-gateway-directionB（dev-loop 收口 V23 证据，2026-09-05）
---

阿里云云 ASR（决策 19 云优先）live 实测被两块外因阻断：① 阿里云凭据（AliyunStreamASR appkey/token、DashScope/百炼 `sk-` key）不可得 → V23 终选 AliyunStreamASR 停留在结构面可声明形态，live ASR/TTS 未达标基线；② human 直供 key 证认出的正确端点是 `token-plan.cn-beijing.maas.aliyuncs.com/compatible-mode/v1`（non-standard DashScope 形态），但 **token-plan 1 周配额耗尽**（429 `insufficient_quota`），将于 **2026-09-07 14:19 UTC** 重置——转写正确性/延迟量级/流式支持/音频输出格式/VAD 切句语义（AliyunStreamASR `max_sentence_silence` vs Qwen3-ASR-Flash 非经典逐句）本轮未能实测。另引擎 ASR provider 换型待 human 拍板：token-plan Qwen via `OpenaiASR`（`type:openai` 指 token-plan `/audio/transcriptions`，不改引擎，需验路径存在）vs `Qwen3ASRFlash`（SDK 直连，无 base_url 覆写需引擎改动）。

---

## 收口（2026-10-12 主线程盘点）

换型已定谳并 live 在役：human 2026-09-06 ASR 终选 Qwen3ASRFlash（`server/deploy/provider.config.yaml:82-92` 定谳指针，DashScope 凭据 secrets.env 已配 `DASHSCOPE_API_KEY`）；live 多轮真机实测通过（261010/261012 真机轮识别文本正常）。原条目待的「token-plan 配额重置后补验 + ASR provider 换型拍板」均已过点，关闭。
