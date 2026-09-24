---
id: ARQ-260923-codemem-mode-doc-drift
title: "CodebaseMemory 索引 mode 的文档与实效不符（三处：AGENTS.md:39 / launch-prep SKILL:93 / cli SKILL:42 示例）"
domain: gov
status: deferred
severity: SHOULD
owner: architect（裁
trigger: 下一治理批（与 ARQ-260922-buildfeature-launch-prep-skip 同族可并批——同动 launch-prep 文面；与 ARQ-260918-spec-face-3page-reconcile 同属「文档与实况对齐」族）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: 主线程 2026-09-23 实测（A/B 探针 logfile ×3 + 存量 Pod `index_status` ×3 + mode 空转实测；探针项目已删、可再生）
---

**证据**（2026-09-23 主线程实测，可点开）：① 干净 A/B——同仓（`C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`）两个探针项目从零各建一次：`moderate` = 10142 nodes / 38908 edges，`full` = 17952 / 48338（logfile `~/.cache/codebase-memory-mcp/logs/K7-mode-probe-{moderate-1790176388,full-1790176451,m2-1790176740}.log`）；差异几乎全在 Variable 面（`MATCH (v:Variable) RETURN count(v)` = 1226 vs 3194，−62%），Function 1298 vs 1421（−8.7%）、File 820 vs 896（−8.5%）；moderate 另按 fast-pattern 丢 `*.test.c` 一类文件、`LICENSE` 走 skip-list。② 存量 Pod 索引全为 full 级（`index_status`：pod-char-stroke 17742 / pod-gateway-directionB 16944 / pod-host-test-infra 15756 nodes，远高于 moderate 的 ~10k）。③ `index_repository(mode=…)` 对已建项目是空转（实测同 path 换 mode 重跑，nodes 逐值不变）。**影响面**：三处文档与实况不符——(a) `AGENTS.md:39` 写「均已建（moderate 模式）」，教人以为图是薄的、未来重建若照抄会把图静默薄掉 43%（`index_status` 仍报 ready，属静默缺项）；(b) `.claude/skills/kidpal-pod-launch-prep/SKILL.md:93` 教 Pod 建图传 `mode=moderate`，实测既不生效（走快照导入）又与存量不符，属误导注记；(c) `.claude/skills/codebase-memory/cli/SKILL.md:42` 的 `delete_project` 示例在上轮正名后指向正名主索引本身（误照抄会删主条目，可再生但产噪音）。另：mode 语义（只在从零解析时生效）当前无任何文档承载。**建议改动**（候选，交 review，不自批）：① AGENTS.md:39「（moderate 模式）」改 full + 一句实测依据（moderate 少约 44%、几乎全在 Variable 面；mode 只在从零解析生效、改 mode 须删条目重建）；② launch-prep SKILL:93 `mode=moderate` → `mode=full` + 一条「不要传 moderate」注记；③ cli SKILL:42 示例改无害占位（如 `{"project":"<项目名>"}`）。**riskTier: low**（纯文档勘误，零代码/零契约/零门）。**review profile**：**full-adversarial + human（Track A）**——Track B 三要件逐条成立（① 执行测出 yes：上列实测；② 问题清晰且修复明确 yes：三处均属「文档与实测对齐」、无设计分叉；③ 半径已被执行揭示 yes：存量索引与既有 Pod 已清点），但触碰 **G1 指令根**（`AGENTS.md`）属机检硬排除 → 按实标 full（存疑向严；先例 = 同族第一轮 `260923-arch-review-graph-index-name-errata` 走 human ack + 四件 + landing-verifier）。G 层事实：G1 = `AGENTS.md`；G2 = `.claude/skills/kidpal-pod-launch-prep/SKILL.md` + `.claude/skills/codebase-memory/cli/SKILL.md`。机检硬排除逐项声明：不触 `docs/contracts/**`、无 spec-contract drift、不改任何 agent frontmatter 的 tools/name/model/write scope、不改 self-def 与 review-loop agent 定义、**不新增/不修改任何 gate**、非 mixed lane、不涉 L0 蓝图。便宜验证计划：`landing-fidelity-check.mjs`（落地结构门）+ 「`grep -rn moderate AGENTS.md .claude/skills` 复核 0 残留」+ 探针 A/B 可复跑复现（图数据可再生）。owner=architect（裁 profile 与改法）+ 主线程（landing）。
