---
id: CDQ-261021-p3-record-cancel-lifecycle
title: 新增 kidpal_story_discard（端→网关私有扩展消息：用户主动丢弃记录会话、素材销毁不入册）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: 实现面承载包（② 单包 Full；launch 前置 = 契约面 anchor 回填）
opened: 2026-10-21
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/review/261021-arch-review-p3-record-cancel-lifecycle/]
fourClass: CONTRACT_CHANGE_REQUIRED
loopEvidence: 261021-arch-review-p3-record-cancel-lifecycle PROPOSAL FROZEN v1.1（2026-10-21；凭据 = human 立项 ack spine q1–q4 resolved 触点① + architect-reviewer R2 GREEN §7）+ 契约 §四/变更记录条与 arch §5.2④/§5.1.2/§12 随批原子落（本条目 landing 时回填）
---

# CDQ-261021-p3-record-cancel-lifecycle · 新增 kidpal_story_discard（端→网关私有扩展消息）

契约面两触点：① §四新增 `kidpal_story_discard` 行（端→网关，`{"session_id":"...","type":"kidpal_story_discard"}`；用户主动丢弃记录会话、素材销毁不入册——requirements S-F2.2 AC4 端→云载体补齐；网关按本连接 session_id 权威取消、不信载荷值，幂等 + 审计，单向无应答；与 `kidpal_story_delete` 先例同构）+ 变更记录条一条；② §1.4 `start_organizing` 行包络核对结论 = 既有行文「设备离开录音聆听演出」已在包络内（含结束聆听）、零改（该触点四分类 = NO_CONTRACT_CHANGE）。

**四分类 = CONTRACT_CHANGE_REQUIRED**（主触点：现有契约无法表达已批准设计 S-F2.2 AC4 的端→云销毁通知语义）——architect-reviewer R1 正式裁定（2026-10-21，反证检查五项全过）；次触点 `start_organizing` = NO_CONTRACT_CHANGE 零改维持。

**剩余条件**：

实现面承载包 261021-p3-record-cancel-lifecycle（② 单包 Full：front discard 消费分支 / teardown 悬挂修复 / 在途引导守卫与腿归口；固件 DISCARD_RECORD 发送链 + `start_organizing` settle；launch 前置 = 本批契约面 anchor 回填）。
