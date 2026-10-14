---
id: TECH-261011-front-half-open-proxy
title: front 代理半开洞——引擎死而设备空闲无流量时泵不收场（设备腿挂死、设备不知情、alert legs 指向死引擎腿）
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（front 代理会话生命周期批）
trigger: 下个动 server/gateway/front 的包；与提醒可达性问题族（决策 21 / 常驻在线批）同族，可与 TECH-261010-device-no-reconnect-on-server-close 后续小批并议
opened: 2026-10-11
closed: ""
closedBy: ""
paths: ["server/gateway/front/front_app.py"]
source: 261010-device-reconnect 真机轮 RC-A4 执行发现（主线程三源对账 + ss TCP 实证，2026-10-11 11:42–11:46）；10-10 16:46 alert_speech_injection_failed 为同机制历史实证
---

**定性：真 bug（front 独立缺陷），非本批两包引入**——260928 前的既有形态，真机轮顺带实证。

**证据（2026-10-11）**：11:42:26 重启引擎容器（RC-A4 首次触发尝试）→ 设备当时空闲。此后 4 分钟：① `ss -tn` 实证设备腿（front:8000 ⇄ 设备）**ESTAB 持续在场**、8010 侧零连接（引擎腿已死）；② front journal 零事件——不关设备腿、不报 `ws forward not available`；③ 设备串口零 `Websocket disconnected`——**设备全程不知情**。机制（front_app.py 会话泵形态）：`gather(pump_up, pump_dn)` 中 pump_up 随引擎腿关闭正常收场，但 pump_dn 阻塞等设备帧（空闲态无帧）→ gather 永不齐 → finally（`_unregister_alert_legs`）不执行 → 会话挂死半开。`ws forward not available` 异常处理只覆盖建连失败/中继报错，不覆盖「上行泵静默收场 + 下行泵阻塞」组合。

**影响面**：① 提醒可达性——alert legs 持续登记在册、指向死引擎腿，到点注入必失败（10-10 16:46 `alert_speech_injection_failed err=ConnectionClosedOK` 即此机制；决策 21 键修复后引擎不主动断，但引擎重启/崩溃/部署仍触发）；② 设备「假在线」——front 认为设备腿在场可投递，设备与引擎间链路实断，孩子下一次说话才暴露（首轮失败/安抚句）。与「设备常驻在线」承诺（决策 29）叠加：引擎侧任何静默死亡都会造出一个提醒黑洞窗。

**修法候选（dev-loop）**：pump_up 收场时主动收 pump_dn 并关闭设备腿（`asyncio.gather` 改 first-complete-wins 或对 pump_up 结尾 `pump_dn.cancel()` + `websocket.close(1011)`）；或 finally 改 `asyncio.gather(..., return_exceptions=True)` 配超时看门狗。注意与 260928-story-connect-push-defer 的拓扑语义（每设备连接一代理会话）正交，不触 story 三时点。**riskTier: medium**（真机已实证复现路径，改动面小但触所有会话收尾路径，须 Full 齿）。关联：决策 21 / [[TECH-261010-device-no-reconnect-on-server-close]]（真机轮同批发现）。
