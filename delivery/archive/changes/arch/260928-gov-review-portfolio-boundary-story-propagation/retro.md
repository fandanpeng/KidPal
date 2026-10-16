# retro · 260928-gov-review-portfolio-boundary-story-propagation（Track B 全环：立项对齐 → 起草 → R1 BLOCK→闭环 → R2 GREEN → FROZEN → landing T1–T9 → verifier FAIL→PASS）

> 主线程第一人称热记忆 · 2026-09-28 · 乘客 #?（Track B 治理批）· harness 形态 = 判官（architect）与核验官（landing-verifier）均为零上下文子 agent、判官跨 R1/R2 续会话复用上下文 · 跨乘客对照：第 2 次踩「ARQ 证据转写失真」坑（F2/F3）。
> 起草形态特例：human 裁 Track A→Track B 改轨 + 主线程亲起草（不派 architect 起草 subagent）——作者=主线程、判官=architect，作者≠判官保全成立。

**【硬项 R1】verdict PASS 后趁热写，第一人称无隔场。**

**【硬项 R2 · 计量诚实】** 主线程侧无 harness token 计量（主线程会话无回执），诚实声明：主线程段（立项对齐 / 起草 / landing / retro）tokens = 未计量、按工具调用次数与制品规模粗估 ±50%；子 agent 侧有 harness 回执精确值（见 §2 表）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens（子 agent 精确值） |
|---|---|---|---| | |
| 立项对齐 | 四裁定 + OQ 前置问答 + 笔误修正 | 会话内 | 会话内 | ~40 min | 0（主线程） |
| 起草 | PROPOSAL v0.1→v0.2（增 T9）→v0.3（R1 闭环） | 会话内 | 会话内 | ~50 min | 0（主线程） |
| 评审 R1 | architect 零上下文 subagent | — | — | 8.6 min | 153,285（26 tool uses） |
| R2 复核 | 同 judge 会话续跑 | — | — | 2.2 min | 154,372（6 tool uses） |
| FROZEN | OQ 三裁 + 状态行 + preflight | 会话内 | 会内 | ~10 min | 0（主线程） |
| landing | T1–T9 逐文件落地 + 探测夹具 | 会话内 | 会话内 | ~60 min | 0（主线程） |
| 核验 v1 | landing-verifier 首验 FAIL（summary 格件） | — | — | 6.5 min | 84,249（21 tool uses） |
| 核验 v2 | 复验 PASS | — | — | 2.0 min | 92,224（6 tool uses） |
| **合计** | | | | ~2.9 h | 子 agent 精确 484,130 |

对照：上一治理批（260928-cross-face-intake-spine）同日合流，量级相当；本批特殊点 = Track A→B 改轨 + 立项会话裁定密度高（human 触点 5 次：笔误、四裁定、OQ 三裁、T9 新增、「下一批再说」两件）。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（对齐+起草+landing+retro） | 未计量（±50% 粗估 ~200k） | — | 无 harness 回执 |
| 判官（R1+R2） | 307,657 | 62% | 跨会话复用上下文省重读成本 |
| 核验官（v1+v2） | 176,473 | 36% | v1 21 tool uses 抓出 summary 格件缺口 |
| **合计（子 agent）** | **484,130** | | |

花费最大处 = 判官 R1（26 tool uses 全仓独立复核）。可压缩分析：R1 的 26 次工具调用里约 1/3 在复核主线程已附实测的证据段——若起草期实测留痕更规范（命令 + verbatim 输出），判官可降抽核力度；但对抗结构的价值恰在独立复核，不建议压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · ARQ 证据转写失真沿袭进 PROPOSAL**：现象 = T3 引「planner.md:39 现文含『acceptance/decisionCoverage 为权威』」→ 根因 = 起草时沿 ARQ-260927 证据 4 的转写、未逐句复证现文 → 证据锚 = R1-F2（判官 grep 全仓零命中该句）+ planner.md:39 现文。**「搬运引据视同新断言复证」硬项的价值实证**。
- **F2 · census 命令转录丢 `-E`（空转假绿险）**：现象 = 起草期实测 census 命令转录进 PROPOSAL 时丢 `-E` → 根因 = 目视转录 + 未实跑复核转录结果 → 证据锚 = R1-F1（判官实测转录形态零命中、连排除项都不匹配）。**near-miss（最险一刻）**：若判官未抓，§5-8「例句位零残留」将永远假绿、T8 验证集成摆设——「判官独立复核」保险丝接住了作者偏差。
- **F3 · landing summary 制品格式义务无模板承载**：现象 = 首验 FAIL（缺 manifest 围栏块 + packet-id 字样）→ 根因 = PROPOSAL.template 有硬项清单、summary 无对应骨架，格式义务靠会话记忆 → 证据锚 = landing-verdict.md FAIL→PASS 演进记录。
- **F4 · enforce-scope 拦截探测包 Bash 重定向（正确拦截、一次改道）**：现象 = §5-3 探测夹具用 heredoc 建探测包被 enforce-scope 红线拦（delivery/taskpackages bashDenyWrite）→ 根因 = 我图快用了重定向 → 教训 = 探测夹具也走受门 Write，红线工作正常无摩擦。

## 4. 优化建议（按 ROI 排序）

1. **landing-summary 制品骨架模板化**：治 F3 · 载体 = **ARQ 收件箱**（合并后与 ARQ 结项同批登记，候选名 `ARQ-260928-landing-summary-skeleton-template`——PROPOSAL.template 有硬项清单而 summary 无骨架，格式义务靠会话记忆不可靠，verifier FAIL 一轮的根因） · 预期收益 = 消掉每批一次 summary 格件返工轮。
2. **起草期实测命令「转录后复跑」两分钟纪律**：治 F1/F2 · 载体 = **登记不硬治**（PROPOSAL.template 起草期自检三句已有「byte 对账」义务，本批实证其价值；不必新增机检） · 预期收益 = census/命令转录空转假绿险归零。
3. **探测夹具一律走受门 Write**：治 F4 · 载体 = **自律件**（enforce-scope 红线工作正常，无治理动作） · 预期收益 = 免一轮拦截往返。

## 5. 一句总结

~2.9 h / 子 agent 484k tokens / human 触点 5 次 / 制品 4 件齐（PROPOSAL + summary + verdict + retro）· 最险一刻 = F2 census 转录空转假绿险（判官接住）· **保险丝判定：两方独立结构接住作者偏差**（判官 R1 抓 F1 失真引文 + F2 转义空转；verifier v1 抓 summary 格件缺口——两方各产出真 findings，作者偏差无一路漏网）· 最值得带走的一条 = **「搬运引据视同新断言复证」不是文牍义务而是本批两条 BLOCK 级 findings 的直接来源**（F1/F2 同根）。

## 6. 本环遗留（批次型变更特有）

- 留根各项随 PROPOSAL §6（devloop-landing-manifest 已单开 ARQ；OQ-2b 机械齿挂量化触发器；brief 冒号形态解析兼容未覆盖）。
- 三 ARQ（260927 主 + 260924 / 260914 并批）结项动作 = 合并后改条目状态 + crq-render（合并后执行）。
- F3 建议载体：landing-summary 制品骨架模板化 → 登记 ARQ 收件箱（合并后与 ARQ 结项同批登记）。
