---
id: CDQ-261103-star-points-story-cover
title: 星星积分本端云契约——points_home/points_ledger 两 view、add_points/redeem_points 薄意图、session_ctx 六值、kidpal_points_delete/page 两私有消息、story_list 改 P4 共享帧
domain: contract
status: resolved
severity: MUST
owner: dev-loop
trigger: 实现面承载包 K0–K5（workstream 261103-star-points-story-cover；launch 前置 = 各面 anchors 回填 + K0 spike 在前）
opened: 2026-11-04
closed: 2026-11-04
closedBy: main-thread（契约 diff 随批原子落 + landing-verifier PASS）
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/review/261103-arch-review-star-points-story-cover/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: 261103-arch-review-star-points-story-cover PROPOSAL FROZEN v1.0（2026-11-04；凭据 = spine human 触点① + R1 BLOCK(F1)→v0.2 + R2 靶向 GREEN §7）+ landing-verifier PASS（b930f498 逐文件人读 + §5 复跑不窄化）+ 契约 §1.3⑦⑧/§1.4/§1.5/§1.6/§四 与 arch §5.1/§5.2⑥/§5.4/§6.5、ADR-018 同批原子落。**闸凭据例外（human 2026-11-04 授权）**：pre-merge-escalation-gate BLOCK（off-main 隔离被破坏——落地 commit 已直接在本地 main，无独立分支可合），human 裁「授权例外直落 main」，BLOCK 凭据与授权记 landing-summary 与本条目；闸的分支隔离语义本批以人工授权替代。
---

# CDQ-261103-star-points-story-cover · 星星积分本端云契约包

契约面五触点随 `261103-arch-review-star-points-story-cover` Track B 原子落（commit b930f498）：① §1.3 新增 `points_home`（可负余额、唯一激活目标、最近三条、空账/未达两态、restore）与 `points_ledger`（统一分录六键、全量 total、keyset 分页、reset）；② §1.4 新增 `add_points` / `redeem_points` 网关决定类薄意图（front 截获交 points.py 确定性执行，引擎 display 流拒转，同 `start_organizing` 先例）；③ §1.5 白名单 7→9 + 积分来源门禁 + 第 7 条积分面机械职责；④ §1.6 system prompt 积分纪律改写（模型不生成任何权威积分字段）；⑤ §四 `kidpal_session_ctx` 闭集扩六值（points_ledger 可带只读 selected_id）+ 新增 `kidpal_points_delete`（单向幂等删除、重算重推对账、隐式 ack 沿 `kidpal_story_delete` 先例）+ 新增 `kidpal_points_page`（keyset 分页请求—应答，沿 `kidpal_story_fetch`/ADR-011 先例）。连带：§1.3③ story_list 改 P4 模块共享数据帧（封皮计数复用 `total`、后台推送统一 `restore:true`、显式「打开故事」= 空 data 薄载荷进 P4 封皮）+ §1.3①④ 页位句对账。

**四分类 = ACCOMPANIES_SPEC_CHANGE**（architect-reviewer R1 正式裁定，2026-11-04；反证五问全过——全部契约改动逐条支撑 frozen requirements v1.71 的 F6/S-F2.6 并随架构同批原子落）。

**剩余条件**：无契约侧剩余；实现消费归 workstream `261103-star-points-story-cover` 的 K0–K5 串行切片（K0 spike 以设备真实可见为证据，不以文件写入自证）。CDQ-260914-kidpal-session-ctx 不随本条自动核销——其核销前置 = K5 对原 story/home/schedule 三值发送与 front 拦截义务的回归验收。
