---
id: TECH-261023-story-voice-discard-no-text-carrier
title: S-F2.2 AC4「或说『讲错啦』Then 取消」语音丢弃路径零文本检出载体——真实丢弃链 = 取消键/BOOT → kidpal_story_discard 契约帧 → front acc.cancel，记录语境引擎不派发 LLM，「讲错啦」作为语音输入在当前链路无落点
domain: tech
status: deferred
severity: SHOULD
owner: human（/triage-change 分流：spec 侧澄清 or 实现侧补检出通道）
trigger: 下次动 S-F2.2 记录会话需求面 / front 丢弃链 / STT 素材入口过滤的包
opened: 2026-10-23
closed: ""
closedBy: ""
paths:
  - server/gateway/front/front_app.py
  - server/gateway/story.py
  - firmware/main
  - docs/requirements.md
source: "261023-agents-md-static-semantics-alignment B2 对账项（plan.md §5 + review-packet 对账节，builder 静态对账留证）；architect advisory 同判（2026-10-23）；主线程代登记"
---

## 缺口本体（builder 静态对账，离线可判）

- `docs/requirements.md` S-F2.2 AC4 原文含「**或说「讲错啦」，Then 取消**」——但 `server/gateway/front/front_app.py`、`server/gateway/story.py`、`firmware/main/**` 对「讲错啦 / 不对不对 / 重说」**零文本检出**（全仓唯一命中 = red-baseline 历史 md 文档行，非运行期载体）。
- **真实丢弃链**：设备取消键/BOOT → 契约 §四 `kidpal_story_discard`（`protocol.cc:116-134`、`mcp_server.cc:751`）→ front `acc.cancel`（`front_app.py:647-659`，:653）——丢弃由**按键通道**承载；记录语境下引擎不派发 LLM，「讲错啦」作为**语音**输入在当前链路无落点。
- AGENTS.md 故事节（批 A+B 改后）诚实处理：保留「讲错啦」原词与 AC4 对齐，但机制归属写「丢弃由板子的取消键和网关的丢弃链承载」，不承诺模型执行动作。

## 对 spec 的影响

这是 **spec↔实现的既有缺口**（非 261023 批引入，prompt 不可修复）：AC4 的「说『讲错啦』Then 取消」分支没有实现载体。孩子真说「讲错啦」时，语音进不了记录语境的取消通路（引擎不派发），行为退化为素材入缓冲（若窗口过滤未拦）——取消动作实际只能靠按键。

## 候选修法（立项时裁）

- **spec 侧（轻）**：`/requirement-change` 档 1 一句话——AC4 丢弃触发改为「按取消键（或 BOOT）」，删「或说『讲错啦』」或标注为按键通路的口语引导语而非独立触发。
- **实现侧（重）**：front STT 素材入口在记录语境加 discard 短语检出（词表单源可测、与既有 is_finish_phrase/is_stuck_phrase 同族）——需评估误触发面（孩子正常讲述含这些词）。

## 关联

- 批 A+B 包 `delivery/taskpackages/261023-agents-md-static-semantics-alignment/`（review-packet 对账节 = 证据）。
- `REQ-261023-family-arrangement-scope-clarification`（同日另一笔 spec 张力，不同条目）。
