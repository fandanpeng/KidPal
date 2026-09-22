---
workstream: 阶段三·课表（一周网格 + 家长录入 + 数据接首页）
slug: m3-schedule
status: active
opened: 2026-09-21
closed:
summary: M3-schedule 里程碑拆包——P2 一周总览网格（S-F1.2）+ 语音问课表问答框口径（S-F1.3）+ 家长录入即时链路（S-F1.5/B4）+ 课表数据接首页「接下来」合流（决策 26 三触发 producer 链路）
owner: portfolio-planner
area: delivery
changes:
  - 260921-m3-k1-cloud-schedule-producer
  - 260921-m3-k2-p2-week-grid-page
  - 260921-m3-k3-voice-schedule-entry-live
---

# m3-schedule · task_plan

## 目标

孩子翻到课表页能看到一周网格整周的谱（周一~周日 × 上午/下午/晚上，今天列高亮、临时安排带 ※），
随口问「明天有什么课」在问答框里得到含备注的答复（不切页），家长在 QQ 上录一条课三处即时生效
（课表页/问课表/提醒），首页「接下来」从此由课表数据驱动（决策 26 三触发）。

## 设计权威

- 需求权威 = `docs/requirements.md` F1 节现文（S-F1.1 / S-F1.2 / S-F1.3 / S-F1.5，v1.19 课表大改口径）+ §6 阶段三；
  拆包输入 = `delivery/milestones/milestones.jsonl` 行 `M3-schedule` nextAction 三块（sourceType=delivery-story，无 PROPOSAL 输入——m1/m2 同款先例）。
- 支撑锚 = `docs/architecture.md` §5.1（服务端组件·课表数据链路/家长录入链路）、§5.2 场景①②（日程刷新三触发 / 语音问答链路）、§5.5.4（课表页渲染）、§10 决策 26。
- 契约面 = `docs/contracts/interface-contract.md` §1.3②（schedule view）——本 workstream 触契约变更，走 architect-reviewer 四分类 + 人类契约门（K1 承载，launchPreconditions 第 1 条）。
