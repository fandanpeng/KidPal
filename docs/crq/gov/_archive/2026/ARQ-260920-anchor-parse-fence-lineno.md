---
id: ARQ-260920-anchor-parse-fence-lineno
title: 锚解析 / 起草期干跑的两处精度缺陷：① `--preflight` 报 §2 节域相对行号（作者须心算偏移）；② `parseSection2AnchorLists` 不跳 fenced code block
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-20
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-citation-anchor-residue T3 子项 a / b 双落地：① 报**文件绝对行号**（`i + 1 + offset`）；② 新增 `fenceLineFlags` 状态机，条目标题扫描与锚行扫描**都跳围栏内**。**子项 b 的判断点由 R1 判 `HUMAN_REQUIRED` = 否**——「§2 围栏块是示范 / 原文钉死内容、非声明」立场有 `checkVerbatimQuotes` 对 §2 围栏的既有字节级对账消费作支撑；跳围栏后真实声明若误入围栏仍 fail-loud「缺锚清单」，无静默绕过面。**可验证收益**：canonical 模板的 `〔第五段·锚清单〕` 变形标记随之撤销、示范行还原裸形态（与门同形）。自检 +6 夹具（围栏带 backtick / 裸写两形态 + 幻影条目不吞锚正控）。landing-verdict PASS + 4 ADVISORY、commit `6c726b9`。证据：[260920-gov-review-citation-anchor-residue](../changes/arch/260920-gov-review-citation-anchor-residue/)。
