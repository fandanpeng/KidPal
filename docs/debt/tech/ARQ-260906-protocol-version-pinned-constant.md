---
id: ARQ-260906-protocol-version-pinned-constant
title: websocket 协议 version 是可漂移的 NVS 数据键，但方向 B 网关恒讲 version=1——它该是代码钉死的常量（或配网写入面强校验），不是能被刷错/刷漂移的运维数据
domain: tech
status: deferred
severity: SHOULD
owner: architect（定方向）+
trigger: 下个动 websocket_protocol/ws_provision 的包 / 下次架构治理批
opened: 2026-09-06
closed: ""
closedBy: ""
paths: []
source: 260903-device-ws-switch 联调实测（2026-09-06，pod-retro「卡点」节）
---

**证据**（260903-device-ws-switch 联调实测，2026-09-05/06）：早前 NVS CSV 误写 version=2，设备按 v2 二进制头去解官方服务端 0.9.6 在 ws 腿上发的**裸 opus**（v2 头只走 MQTT 网关）→ 垃圾 payload_size →（固件加固前）`bad_alloc` 崩溃、（加固后）全帧丢弃无声；真值 version=1 由后续一次刷写落进去才恢复。**影响面**：任何一次配网/NVS 写入把 version 写成非 1 即全链无声或崩溃，且无门报警；`websocket_protocol.cc` 默认 1、仅 NVS 非零才覆写，等于把「正确性」押在一个可被写错的运行时键上。**建议改动**（候选，交 review，不自批）：① 固件侧把方向 B 的 version 钉为常量（1）、删掉 NVS 覆写路径；② 或配网写入面（`ws_provision.Provision`）对 version 强制只接受 1；③ 保留可配但加启动断言/高声日志报警。涉及 `firmware/main/protocols/websocket_protocol.cc` + `firmware/main/provisioning/ws_provision.*`，属自有实现面设计变更，交 dev-loop 包承接。**riskTier: low**。owner=architect（定方向）+ dev-loop（落地）。
