---
id: TECH-260928-school-merge-inprogress
title: 合并「上学」项在校时段进行中语义未实现（无下一事项时 end=None 走 start-only 被过滤）
domain: tech
status: blocker
severity: SHOULD
owner: dev-loop（server 面，含语义参数化设计决断）
trigger: [[TECH-260928-school-start-upcoming-empty]] 修复批落地后随批处理或另行走 /arch-change
opened: 2026-09-28
paths: [server/gateway/schedule.py, server/deploy/openclaw-schedule.config.yaml]
source: [主线程 2026-09-28 现场诊断中代码级推演（本条目「证据」段留 file:line；live 实证待前条修复落地后取得）]
handoff: review profile 为 full-adversarial + human → 交 /arch-change（Track A）或并入前条修复批由 human 裁路由
---

**证据**：`server/gateway/schedule.py::group_upcoming` 合并项 end 语义：

- `schedule.py:326-332`：合并「上学」项 end 仅在「当日存在下一事项」时才赋值
  （`end = min(next_starts)`）；**无下一事项 → end 保持 None**。
- `schedule.py:334-340`：end 为 None 走 start-only 规则 `it["time"] >= now_hhmm` ——
  school_start（08:00）一过，该项即被判「已过」剔除。
- 模块 docstring（`schedule.py:299-301`）钉的口径是「合并『上学』项在校时段
  （至当日下一事项开始前）计为进行中」——**对「整天只有上学、无课外课」的纯在校日
  （如现行周一/周二/周五），在校时段没有实现承载**。
- 诚实标注：此项为 2026-09-28 现场诊断中的代码级推演（file:line 如上），非 live
  实测——因 [[TECH-260928-school-start-upcoming-empty]]（school_start 从未生效），
  合并项在生产上从未跑过；前条修复落地后此行为才会显形。

**影响面**：纯在校日（现行周一/二/五）白天任何一次建连推/零点推刷新，都会把
「上学」项过滤掉 → P1 首页在校时段空屏；与决策 26「在校时段计为 upcoming」的
口径冲突。且注入语义是整体替换（front_app 推送即覆盖 seam），白天重连会把零点推
已显示的「上学」抹掉。

**建议改动**（交 review、不自批）：存在**真设计分叉**，候选：
- 甲：配置载体加 `school_end`（如 "16:30"），合并项 end 恒 = school_end（无下一
  事项时）——显式参数化，改 `openclaw-schedule.config.yaml` + `group_upcoming`；
- 乙：合并项 end 缺省 = 当日 23:59（在校日整天视为进行中）——零新配置但语义粗；
- 丙：维持现状、登记为 accepted-risk（周一白天不看板）。
方向与参数值需 human 拍板，故不走快路径。

**风险**：riskTier: low——纯 producer 分组语义、不触契约面（B1 表体零变更、
item 形状不变）；影响为显示体验降级非数据错误。Track B 三要件：① 执行测出——
**推演非实测**，no（诚实降档）；② 问题清晰修复明确——存在真设计分叉（end 参数
来源三选一），no；③ 爆炸半径已被执行揭示——no。→ **review profile =
full-adversarial + human（Track A）**，绝不回标快路径。owner = dev-loop（server 面，
含语义参数化设计决断）。
