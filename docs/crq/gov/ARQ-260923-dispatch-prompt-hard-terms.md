---
id: ARQ-260923-dispatch-prompt-hard-terms
title: 派发书模板缺三条硬约定——子代理 Write 被拦的回执姿势 / scope 写权前置核对 / mutation 执行证据落盘责任人——全活在各棒 retro 的「下次改进」里、每棒重学一遍
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（与 ARQ-260922-buildfeature-launch-prep-skip / ARQ-260922-pod-requirements-not-authority 同族并批——同动 planner.md 编排文面）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-23（pod-retro 实证：k7 卡点 3/4、k6 卡点 3、k5 追加轮 4、display-fix 钩子节、m3-k1 卡点 6；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（loop-retro-2026-09-23，跨棒实证）：① 子代理 Write 被环境拦（「Subagents should return findings as text, not write report files」）：K7 一棒三撞（challenger / defender / adjudicator / acceptance-author 均中），前两次事后代存补救（两次白跑的转录）、第三次起派单前置写明「被拦 → 全文回执、我代存」才止损——K7 retro 原话「这个约定应该在派单模板里，而不是让每个 agent 撞一次再学」；② scope 写权前置核对缺位：builder 无 `delivery/taskpackages/**` 写权致 mutation evidence 落盘被迫改派或代落——K6（提前想到挪给 integrator，但 builder 主轮仍撞一次、白跑一轮转录落差）、K7（第二次复现，其 retro 自判「同一坑跨棒复现 = 记账没升级成机制」）；enforce-scope 把正确活挡门外（display-fix 的 char_stroke_lib.py docstring 第二例、m3-k1 taskpackage 回填第三例）——「冻成齿转义 + planner 代落」手法已稳定但依赖 planner 每次想得起；③ mutation 执行证明 provenance 转述差：K5 跨三棒只做到「设计态红≙变异态在案」，K6 把「六条变异第一方实跑 + 原始输出落 mutation-evidence/」写进 integrator 派单后销账走通（字节级备份回拷 + sha256 复核），K7 又回到代落。共同根：三条约定散落在 6+ 棒 pod-retro 里，派发 prompt 模板无硬条目。**建议改动**（候选，交 review，不自批）：planner.md 派发步固化三句模板条目——① 「Write 被拦 → 全文回执、planner 代存」；② 「派单前列 scope 写权表：谁落盘、落哪个目录、有无写权」（无权即预指派代落人，不让 agent 撞钩子再学）；③ 「mutation 执行证据落仓内 evidence 文件 + 指定落盘责任人」（K6 手法成文）。零新门、纯编排文面。**riskTier: low**。owner=architect。
