---
id: ARQ-261025-brief-inputs-anchor-noise
title: brief-inputs 锚点解析噪音——整文件退化为 advisory、裸 token ref 报「文件不在磁盘」，连续四包稀释真告警
domain: gov
status: deferred
severity: COULD
owner: architect（advisory 口径）+ dev-loop（`brief-inputs.mjs`）
trigger: 下一治理批（动 `brief-inputs.mjs` 或起包 advisory 面时并批）；或再出现一次真实 ref 腐坏被噪声掩盖
opened: 2026-10-25
closed: ""
closedBy: ""
paths: [".claude/scripts/brief-inputs.mjs"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261013-lang-config-thinking-fix/pod-retro.md, delivery/taskpackages/261022-m5-k3-honcho-memory-production/pod-retro.md, delivery/taskpackages/261023-agents-md-static-semantics-alignment/pod-retro.md, delivery/taskpackages/261023-honcho-memory-wiring-restoration/pod-retro.md]
---

**证据**：① `261013-lang-config-thinking-fix`：brief 清单退化整文件引用（refs 无锚点 ADV 3 条），直接诱发 acceptance-author 超读 10 行。② `261022-m5-k3-honcho-memory-production`：裸 token ref（`NFR-PRIV-1` / `TECH-*`）报「文件不在磁盘」，retro 记「连续第三包误报，advisory 噪声稳定存在但无害——若某天真有 ref 腐坏，报警可信度会被稀释」。③ `261023-agents-md-static-semantics-alignment`：8 个 refs 全报「无锚点、整文件引用」，本包 refs 天然是文档面多锚，advisory 对这类包是常驻噪声。④ `261023-honcho-memory-wiring-restoration`：5 条 ref 无锚点告警，起包时顺手清可封住阅读面。

**影响面**：每个起包/派发包都会收到若干条 advisory；当前无人消费，真 ref 腐坏时与噪声不可区分。本波 4 包命中，均为 advisory、零阻塞。

**建议改动**（交 review、不自批）：① 裸 token ref（非路径形态）不再按「文件不在磁盘」报，改报「非路径 ref、跳过锚点解析」；② 文档类 ref 的整文件引用降为 info 或按「文件内章节锚」解析，不作为缺锚警告；③ advisory 输出汇总成一行计数，避免逐条刷屏。不改任何 blocking 判据。

**风险**：riskTier: low——纯 advisory 输出面，不改门判定。
