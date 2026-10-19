# pod-retro · 261019-p3-record-dialogue-guide（第一人称热记忆，planner 收尾相位 2026-10-19）

> 标签：第一人称主观过程体感。tokens / gates / rounds / verdict / test results 等以 machine artifacts（verifier-evidence / pod-events / taskpackage）为权威，本文件不复述、不冠权威。

## 卡点与绕轮体感

- **最别扭的一段是我自己造的**：上会话 429 死前对 taskpackage 的格式回填带了散文注记（landsIn / changeManifest / acceptanceTestsFile），本会话机械核当场曝红。教训具体化：**回填结构字段时散文一律进 notes，字段本体永远纯形态**——这条我这次是用三轮 verify 红换来的，下次起包模板自查就该拦住。
- 同族第二刀：我把 red-baseline 冒烟脚手架 sh 也「顺手」纯路径化进了 acceptanceTestsFile，被 acceptance-coverage 打回。两处都是「格式归一时没想清楚字段的语义边界」——纯路径化 ≠ 全部路径都该进这个字段。
- mutation-check 的全 id 子串匹配（raw 全文提及）我第一版用短 id 撞了墙。短 id 对人可读、对闸不可见——**证据字段的匹配口径先看脚本再落笔**，这一刀其实可以预判。
- docs-sync / security 两红同根：字段存在性+内容判据（docsNoChangeReason 逃生口、securityEvidence 逐轴子串）都在脚本里写着，我起包时没逐闸过一遍 TaskPackage 字段面。**下次 finalize 前把 8 个闸脚本各自的 TaskPackage 期望字段清单过一遍**，可以把 r1–r4 全部消掉。

## verifier↔fixer 绕轮原因的主观判断

- fixer 一次都没派——五轮 scoped 全是 planner 元数据自修。这不说明实现零风险（builder 停点申报的 TEST3 探针缺陷是真问题），说明**闸对元数据形态的敏感度高于我对它的预期**。
- TEST3 停点申报质量很高：builder 拒绝 sticky 方案的理由（真机会吞孩子打断后的收尾保存）是对的，「实现与冻结件冲突 → 停」的处置也干净。acceptance-author 复钉也快。这条链路（builder 停点 → acceptance-author 复钉 → builder 补变异取证）是本次跑得最顺的一段。

## 帮倒忙/帮上忙的钩子与规则

- 帮上忙：taskpackage-required-check 的 landsIn/glob 纯形态校验在派 builder 前就拦住了三处——否则进 verifier 才爆，返工更贵。
- 帮上忙：scope-map deny 让「builder 修冻结件」这条路物理不可行，逼着走了 acceptance-author 复钉的正确通道。
- 体感别扭：mutation 全 id 子串匹配 + securityEvidence 逐轴原文包含，都是「证据字段必须内嵌权威全文」的形态——防漂移的意图我认，但引导（错误信息只说 missing 列表、不说「把 acceptance 原文嵌进去即可」），每次都要读脚本源码才明白。

## 人工纠偏体感

- 本会话零人工纠偏（续跑全程自主推进）。上会话 429 中断恢复成本比预想低：恢复点描述 + 盘上实况逐项对上，投影漂移一处即修。**恢复点描述写清楚「哪些在盘上、哪些没 commit」是续跑能自主推进的关键**——这条值得进 launch-card 惯例。

## 下次改进（可执行）

1. finalize 前逐闸过 TaskPackage 字段面（8 闸期望字段清单化，附进起包 checklist 心智卡）。
2. 结构字段回填零散文注记（注记一律 notes）。
3. evidence 类字段（mutation/security）落笔前先读对应 check 脚本的匹配口径。
4. 工作树 untracked 制品（prompts/ 新目录）在 builder 派单时点名申报——本次 builder 主动报了，但派单模板里可以固化。

## escalation themes（pod-events cross-ref）

- friction/stall（867c0ff7）：上会话 429 中断续跑——恢复点核验 + 投影漂移修复。
- friction/degraded（48dd2f63）：RDG-F2 TEST3 冻结件探针缺陷 → acceptance-author 复钉闭环（agent-escalation 未触发，停点申报在 builder 回执内闭环）。
- friction/degraded（24c1b0bf… / a9ada8a4）：round1 脚手架误列 / round2-4 元数据三红——planner 自修闭环。
- 无 human-decision / human-correction 事件（本会话 human 触点为零，符合分层出场口径：常规派发不逐次请示）。
