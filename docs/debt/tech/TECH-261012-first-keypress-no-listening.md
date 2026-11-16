---
id: TECH-261012-first-keypress-no-listening
title: 重启后首次按键不跳聆听页（human 口述「每次重启都有」）+ 实测复现：按键撞 781s 空闲通道超时 → WS 重建后会话失同步、音频通道秒关弹回 idle，连按两次恢复
domain: tech
status: deferred
severity: SHOULD
owner: dev 批（261010 device-reconnect 谱系 follow-up——常驻在线的通道保活/首按恢复语义）
trigger: "待 P1 真机轮 A A3 真机凭据结项——261116-channel-resident-core（T5/T6 心跳保活 + T4 自愈巡检）已实现、离线绿；A3 = 闲置 >5 分钟首按键一次进聆听、无重连无「按了没反应」。依据 workstream PROPOSAL §6「debt 结项凭据必须真机、不是合入」，收口记账归主线程。原：下一次动固件连接生命周期/常驻在线逻辑的批"
opened: 2026-10-12
closed: ""
closedBy: ""
paths: ["firmware/main/application.cc", "server/deploy/kidpal-front.service"]
source: 2026-10-12 真机轮（human 口述既有现象）+ 串口 c:/tmp/hv5-realtime-round3.log 两次按键全轨迹 + front/引擎日志对账
---

**现象（human 口述 + 2026-10-12 实测两轮复现）**：设备重启后第一次按 KEY 不跳聆听页；连续按两次恢复正常。human 证言「每次重启都有的」——261010 device-reconnect 批（永不休眠常驻在线）落地后的既有行为，非 261012 批引入。

**实测证据链（14:11 / 14:17 两轮同模式）**：
- 按键在常驻 WS 空闲 **781 秒**时到达（`E Protocol: Channel timeout 781 seconds`——设备协议层检测到通道静默超时，源头待查：front 侧空闲关闭或设备协议 keepalive 阈值，决策 21 的引擎 `close_connection_no_voice_time=86400` 未覆盖此层）。
- 设备按断线重连逻辑 2.2-4.7s 重建 WS、hello 成功、恢复聆听态——但**会话状态失同步**：front 把新连接当全新会话（推 home/schedule 恢复包、无进行中会话语境）→ 音频通道打开后**同毫秒被关**（press 1：AFE 任务启动与 `listening -> idle` 同毫秒 795845）或**通道根本未开**（press 2：无 AFE 行、790ms 后 idle）→ 设备弹回首页，首次按键意图被吞。
- 第二次按键时 WS 已是新鲜连接 → 正常进入聆听（human「连按两次就能正常跳」与此吻合）。

**待查**：① 781s 空闲关闭的权威承载（front_app.py 通道空闲逻辑 vs 设备协议 keepalive 参数——决策 21 只钉了引擎侧）；② 设备侧常驻在线保活为何未阻止/未感知该关闭（261010 RC-B2 常驻重建语义 vs 781s 空闲关闭的分类归属——按 RC-A2 互斥分类这是「服务端断」应触发重建，但实测重建后首按仍失同步）；③ 首按失同步的根因在设备会话状态机还是 front 会话语境。

**修法方向（归后续批裁）**：A) 保活对齐——把 front 空闲超时提到 ≥ 设备保活周期（配置层）；B) 设备侧感知——通道被远端关闭后常驻重建完成前，KEY 按压改为「等待重建完成后自动接续会话」；C) 首按恢复——重连完成后自动补发会话开启。

---

**收口注记（2026-11-16，integrator · 261116-channel-resident-core）**：本包按修法方向 A/B 的合取落地——front 每连接 30s 周期下行 `kidpal_keepalive` 裸心跳（`KIDPAL_CHANNEL_HEARTBEAT` 代码默认开、`0/false/off/no` 为回滚开关），设备 `OnIncomingJson` 加 no-op 分支消化心跳并跳过 Unknown WARN；设备侧「120s 无下行 = 死链」判据由此升级为「120s 无心跳 = 死链」，并在主循环 CLOCK_TICK 区加自愈巡检（Idle && 通道未开 → 复用既有 `TryStartAutoConnect`，零第二条重连路径）。离线面已过：final r2 十闸全绿、scr_b1/b2 心跳与保活断言全绿、server 全量 473 passed。**本条目未结项**：按 workstream PROPOSAL §6「debt 结项凭据必须真机、不是合入」待 P1 真机轮 A A3（闲置 >5 分钟首按键一次进聆听、无重连无「按了没反应」）凭据，收口记账归主线程。status 维持 `deferred`（本次仅刷新 trigger 指向真机验证；债务账 status 闭集不含「verifying」形态）。
