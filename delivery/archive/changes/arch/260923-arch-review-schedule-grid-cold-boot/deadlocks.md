# Deadlock 拍板包 · F-C5（restore 生产纪律运行时闸）· 260923-arch-review-schedule-grid-cold-boot

> 按 charter §6.2 打包：正反最强论点 + 利害 + 各方案代价 + adjudicator 推荐（**建议即地板，可砍更大**）。裁断权在 human。
> 存档注记：adjudicator 制品 Write 被环境工具层拦截，本文件由 review-moderator 代为落盘，内容为 adjudicator 回执全文逐字转存。

## 争议点
LLM 幻觉直产带 `data.restore` 的完整网格 display 块可过 shape floor 直达设备、静默填入 P2 缓存（不接管前台）。「谁可置 restore」的生产纪律现只有 prose + 一条守错路径的验收齿（守展开路径、不守幻觉路径），无运行时闸。事实面双方无分歧（front_app.py:511-514 截获层只拦薄载荷；display_schema.py:87-105 shape floor 不查多余键——判官独立复核成立）。

## 正方（challenger）最强论点：机械闸
- 运行时闸把生产纪律从 prose 升为可执行——纪律没有牙 = 依赖每个未来调用方自觉。
- charter S6 即此场景，且 charter 排除轴注记预言「校验面真缺口可升轴 10」——应验。
- 代价：① `validate_display` 拒 restore 键须配 provenance 旁路（front 自产 restore 推送走同一校验门，front_app.py:768 → build_display_tool_calls → validate_display），须在冻结中继链入口（front_app.py:533）穿孔；② front 保留前缀键候选 = schedule data 校验从 floor 升 ceiling，侵蚀 §1.3⑥ additive 演进先例（与 C6 兼容纪律反向）。利害 = 堵住幻觉呈现面，但动冻结面 + 契约校验语义收紧。

## 反方（defender）最强论点：不单独设闸、ADR-005 显式记残余风险
- **不对称安全表演**：LLM 幻觉直产**不带 restore** 的完整网格今天就能过 floor 并**接管前台**（危害严格更高）；给更低危害变体单独上闸而对更重既存洞不设防，security 无增益。
- **执行地位对称**：契约 §1.3② 既有 producer 纪律「front 机械计算、不由 LLM 生成」同样无运行时闸——validate_display 不分 provenance；对新纪律句要求更高执行标准超出现有契约纪律水位。
- **根因缓解在位**：system prompt 薄载荷约束（契约 §1.6）+ ADR-004 单源；残余敞口 = 幻觉网格静默填缓存、下次三触发推送即被真数据覆盖（自愈）。
- 方案 = ADR-005 增一段显式风险记录 + 撤「三道拦截」高估表述；未来 LLM 直产 display 面整体收紧（provenance 旁路 + floor 收紧）时 restore 拒键随该收口一并做。代价 = 幻觉呈现面敞口存续（自愈型、低危）；利害 = 零冻结面改动、诚实记账、留整体收口路径。

## adjudicator 推荐
采反方第三条路（ADR-005 残余风险记录），理由：① 危害不对称论证成立（不解决既存更重洞的单点设闸是姿态不是防线）；② 与既有契约纪律执行水位对称；③ 动冻结中继链 + floor→ceiling 的架构代价对低危自愈型敞口不成比例。**此为建议即地板**——human 可裁更重方案（如整体收口 LLM 直产 display 面，则 restore 拒键一并做），不可裁更轻（至少 ADR-005 显式风险记录必须落，即 R 清单增一项）。裁后动作：accept → 修复清单增 R11（ADR-005 风险段）+ ARQ-260923-schedule-grid-restore-gate 转 watchlist 或销行；reject → 修复清单增 validate_display 拒键 + provenance 旁路项并评估是否升契约门议程。
