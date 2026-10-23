---
id: TECH-261023-edge-tts-drm-single-point
title: TTS 单点外部依赖——Edge TTS DRM 参数被微软滚动收紧（7.2.6 全量 403、设备卡思考页），发现靠真机报障、无可用性探针、备选 TTS 未接通
domain: tech
status: deferred
severity: SHOULD
owner: 主线程（运维面）+ human（备选 TTS 选型）
trigger: 下次动 server/deploy 部署面 / TTS 面选型前；或 edge-tts 再爆版本淘汰
opened: 2026-10-23
closed: ""
closedBy: ""
paths:
  - server/deploy/engine-patch/edge-tts-731/
  - server/deploy/docker-compose.yml
  - server/deploy/manifest.yaml
source: "261022-m5-k2b 部署轮 live 实测（human 持板报「卡在思考页」，2026-10-23）"
---

## 现象与根因

2026-10-23 14:48 起（P2 持久会话部署轮真机语音回合），Edge TTS 全量 403（`WSServerHandshakeError: 403 Invalid response status`，`speech.platform.bing.com`），每段语音 5 次重试全败 → 无音频回传 → 设备卡思考页。判别过程：ECS 时钟与 NTP 精准同步（排除时钟漂移）；edge-tts 7.2.6 容器内 3/3 稳定 403（排除瞬时抖动）；同日 13:40 P5 实测 TTS 正常（13:40–14:48 之间微软侧收紧了 `Sec-MS-GEC` 参数校验）；本地机器同库 7.3.1 合成成功 → 升级容器内 edge-tts 7.2.6 → 7.3.1 后 403 消失。**根因 = edge-tts DRM 参数被微软滚动收紧，旧版本参数集被拒**。

## 已落处置（20261023 live 热修，随批回流）

- 容器内 pip 升级 edge-tts 7.2.6 → 7.3.1 + 引擎重启（即时解除）。
- 耐久化载体：`server/deploy/engine-patch/edge-tts-731/`（wheel 原样解包，与 ECS 载体逐字节同源）+ docker-compose 两条 bind-mount 阴影容器 site-packages（`edge_tts` + `edge_tts-7.3.1.dist-info`）+ manifest 16 条 `restart: none` 登记——`docker compose up -d` 重建镜像不再回退 7.2.6。

## 风险余量（为什么本条保持 open）

1. **TTS 单点外部依赖**：微软可再次收紧 DRM（下一次版本淘汰只是时间问题）。carrier 只解决「重建不回退」，不解决「7.3.1 本身被淘汰」。
2. **发现路径靠真机报障**：日常无 TTS 可用性探针，故障发现 = 孩子卡思考页。
3. **备选 TTS 未接通**：box 上现有凭据（DashScope key / ALIYUN NLS appkey+token）均无可直插的 TTS 路径（compatible-mode /audio/speech 404；NLS TTS 要 AK/Secret 签名对）；备选切换需 human 选型新凭据。

## 候选修法（任一或组合）

- **可机判监控**：每日 drift timer 里加一条容器内 edge-tts 合成探针（3 秒内非 403 即绿），漂移告警先于人耳发现。
- **版本升级通道**：把 carrier 的 wheel 来源、升级步骤写进部署 runbook（本轮全靠现场判别，下次照方抓药 10 分钟内解除）。
- **备选 TTS 接通**：human 选型（DashScope CosyVoice WebSocket 新 provider / 腾讯 / 火山等），engine-patch 同款方式落第二 TTS 并在 .config.yaml 留热切换开关。

## 关联

- 部署形态先例：`docs/debt/tech/ARQ-260907-engine-deploy-patch-drift.md`（bind-mount + manifest 漂移护栏同族）
- 事件记账：`delivery/workstreams/device-memory-overhaul/progress.md` 2026-10-23 P2 部署轮节
