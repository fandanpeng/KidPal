---
id: TECH-261010-engine-decision21-keys-not-persistent
title: 引擎决策 21 两键（24h 兜底 + 告别语关闭）未入 overlay 渲染源——线上已回落默认 120s 断连 + 告别语复活
domain: tech
status: blocker
severity: BLOCKING
owner: human（线上恢复拍板）→ dev-loop（overlay 补键批）
trigger: human 放行线上配置恢复；仓内 overlay 模板补键走 dev-loop（与 ARQ-260907-engine-deploy-patch-drift 同类病，可同批治）
opened: 2026-10-10
closed: ""
closedBy: ""
paths: ["server/deploy/provider.config.yaml", "server/deploy/apply-xiaozhi-overlay.sh"]
source: 2026-10-10 真机使用反馈两条 bug（重启后 WS 推不下通知 / 静置两分钟告别语复出）+ 引擎/front/OpenClaw 三源日志取证
---

**定性：真 bug（部署持久化缺口），非需求缺口。** 需求与架构口径完备且明确——arch §6.4 决策 21：引擎侧两道超时配 24h 纯兜底 + 告别语关闭（`close_connection_no_voice_time: 86400` + `end_prompt.enable: false`）。

**证据链（2026-10-10 取证）**：① 活配置 `/opt/kidpal/.../deploy/xiaozhi-docker-data/.config.yaml`（10-01 00:16 由 `apply-xiaozhi-overlay.sh` 再生，即 260930 model_name 被冲同次）**不含**上述两键；全部备份链（含 `bak-20260914-session-timeout` 本体）grep 均无——9-14 的修正只活在当时的活配置，从未进入任何持久化载体。② 仓内渲染源 `server/deploy/provider.config.yaml` + `apply-xiaozhi-overlay.sh` 同样无此二键（渲染面缺键 = 每次再生必然丢）。③ 引擎源码 `receiveAudioHandle.py:114/123` 默认 `close_connection_no_voice_time=120`、`end_prompt.enable=True`——今天线上跑的就是默认值。④ 引擎日志 `261010 16:52:25 大模型收到用户消息: 请你以"时间过得真快"未来头…`（引擎内置默认告别语原文逐字出现）→ `16:52:43 超时检查任务已退出`（180s 硬断）；同日 11:07 连接建立 → 11:10:19 即被超时关闭。⑤ front 日志 16:46 提醒语音注入失败（`alert_speech_injection_failed err=ConnectionClosedOK`）——推送黑洞实证。

**影响面**：设备静置约 2 分钟即被引擎断连（决策 21 的「设备常驻在线（提醒推送可达）」架构假设被击穿）→ 断连窗口内提醒全部丢失；同时告别语在每轮静置超时播给孩子听（体验回归）。用户可感的两条真机 bug（重启后通知推不下来 / 告别语复出）的云侧根因都是本条。

**修法（两层）**：① 线上止血（须 human 点头，主线程可代执行）：活配置补回两键 + `docker restart`（已拟好命令，被生产写入权限闸拦下待批；备份名拟 `bak-261010-restore-decision21`）。② 仓内根治（dev-loop 包）：`provider.config.yaml` 增补两键声明 + `apply-xiaozhi-overlay.sh` 渲染 + 起动标记行/验收齿核对「渲染产物含决策 21 键」——否则下次 overlay 再生必复发（与 ARQ-260907 部署面漂移无机械门同病，建议同批治）。**riskTier: high**（不修 = 提醒链路常态性黑洞）。关联：[[TECH-261010-device-no-reconnect-on-server-close]]（设备侧半边）。
