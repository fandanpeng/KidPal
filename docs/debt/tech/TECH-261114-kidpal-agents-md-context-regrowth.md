---
id: TECH-261114-kidpal-agents-md-context-regrowth
title: workspace-kidpal AGENTS.md 静态纪律承载再增长至 20.7KB（每轮 prompt 9,285 tokens）——M5「首轮 ≤11,000」目标按当前增速 1~2 个功能批内破线；结构性冗余（同一规则最多重复 4 遍）留有 3~5KB 压缩空间
domain: tech
status: deferred
severity: SHOULD
owner: dev 批（prompt 承载面，走 dev-loop）
trigger: 下一个要往 AGENTS.md 增回合纪律的功能批（批内预检 probe prompt_tokens）；或探针实测 prompt_tokens > 11000；或 NFR-PERF-4 延迟定阈批
opened: 2026-11-14
closed: ""
closedBy: ""
paths: ["server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md"]
source: 2026-11-14 延迟专项 P1-6 复查——仓内/live 字节对比（20715B vs 20606B，CRLF/LF 规范化差；每日漂移检查 drift_count=0 佐证两边一致）+ live 备份链（workspace-kidpal/AGENTS.md.bak-*）+ loopback 探针 usage 实测
handoff: 与 NFR-PERF-4 定阈批、下一个 prompt 面功能批同批评估
---

**证据（增长是设计使然，不是漂移）**：

1. **增长链（live 备份 + git log 互证）**：261012 前 = 1072B（`AGENTS.md.bak-20261012-prev10`）→ 261022-m5-k1 prompt 承载迁移批立为「板侧 kidpal agent 静态语义单一事实源」（manifest.yaml 登记原文），引擎 prompt v11 纪律迁入 → 15939B（bak-20261024T163201）→ 261024 记忆回忆回合 +1.2KB、261104-k0 积分 spike +1.6KB（bak-20261105-k0 = 17095B）→ **261104-k2 积分问答回合纪律 +3.5KB（2026-11-14 14:58 部署，live 现值 20606B）**。仓内三笔对应 commit：431cdf4b / b5f69f64 / 59b1285e。
2. **每轮 token 成本（2026-11-14 18:3x loopback 探针，新会话冷启动形态）**：`prompt_tokens=9285`（cached 7296，命中率 79%）。对照链：2026-09 基线 22,116 → M5 迁移后 7,761（10/24，progress.md:88）→ 现值 9,285（+20%/3 周）。M5 批立的目标「首轮 ≤11,000」尚在线内，**增速 ≈ +1~1.5k tokens/功能批，再来 1~2 个功能批越线**。
3. **结构冗余（压缩空间所在）**：同一规则在文件内重复 2~4 遍——「回合路由表行 → 详解节 → 硬约束节 → 底线不变量」四层叠加。实例：查字禁工具/禁坐标 ×4、课表永不凭印象 ×4、实时信息不编造 ×3、积分摘要优先/只读 state.json ×3。每层冗余各自对应一次真实事故修复（课表幻觉 / 天气编造 / story 鬼影块 / 积分抢屏），有 LLM 纪律强化合理性，但详解节与路由表单元格几乎全文重复（积分问答路由格 ~700 字 ≈ 积分节全文）。

**影响面**：每轮 prefill 成本与首 token 延迟随文件线性增长；破 11k 目标后叠加 Honcho 注入（~9KB）与动态 context，首轮 input 回到 12k+ 量级，侵蚀 M5 批 -65% 的成果。

**建议改动（候选，交 review 不自批）**：① 硬约束节收敛为指针式摘要，仅三条底线不变量保留全文（它们是真破过线的规则）；② 路由表单元格只留「识别特征 + 一句纪律 + 指向详解节」；③ 提醒 cron 等「正常不会到本工作区」双保险段压至一两行。预估省 3~5KB ≈ 1.5~2.5k tokens/轮。**回归门必须**：改后真机冒烟四类回合（课表 / 查字 / 积分 / 故事·记录）+ display 白名单合法率不减——prompt 纪律面瘦身的每刀都踩在历史事故修复面上，走 /triage-change 起包，不直改。

**风险**：riskTier: medium——纯承载文件内容变更（部署面已有 drift 闸与备份链兜底），但语义回归风险实在（纪律弱化 = 儿童可见行为缺陷），必须带真机回归轮。
