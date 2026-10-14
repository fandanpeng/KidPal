---
id: ARQ-260920-short-form-citation-family
title: 短形态引据族（**跨批累计观察、此前无在册行**）：引据解析不中时一律记 `file-not-exist`，而该格里混着语义完全不同的四种成因
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

260920-gov-review-citation-anchor-residue T2 落地并**随本批登记**：结果由单一 `file-not-exist` 一分为四（`abbreviated` / `ambiguousShortForm` / `shortFormUnresolved` / 真断链），六步判定顺序钉死。**逐形态处置**：前缀省略由 T1 治（实测批③ 那 4 条 `file-not-exist` 归零；全量 26 条转解析）；**中段省略**（`progress.md:18` 型，仓内 5 份同名）**结构上不可唯一解析——只分类不解析、如实声明不造枚举补丁**（正解 = 起草期写全路径，已进模板 checklist）；**缩写形态**（含 `…` / `...`，实测 7 条）单列一类。全量基线：410 份 md / 71 条去重 → 26 转解析 : 7 缩写 : 11 歧义 : 27 仍不中。landing-verdict PASS + 4 ADVISORY、commit `6c726b9`。证据：[260920-gov-review-citation-anchor-residue](../changes/arch/260920-gov-review-citation-anchor-residue/)。
