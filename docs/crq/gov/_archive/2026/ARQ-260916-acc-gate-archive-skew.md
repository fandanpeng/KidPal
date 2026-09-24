---
id: ARQ-260916-acc-gate-archive-skew
title: acceptance-run-check.mjs|固件面枚举不跳|archive|退役面（SKIP_DIRS|无|archive|且|FW_ACC_RE|按形状匹配任意深度，`firmware/main/test/acceptance/archive/`|下|9|件归档|`.acceptance.c`|仍被枚举为在盘验收文件）——与|unit|闸（260916|已修）两闸退役口径不同步；另|archive|保留目录名约定只在两处代码注释、DEV-GOVERNANCE|未成文
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-16
closed: 2026-09-19
closedBy: ""
paths: []
source: ""
---

260918-gov-review-closeout-chain-friction T1 落地（① `acceptance-run-check.mjs` `SKIP_DIRS` 补 `archive`、镜像 unit 闸 260916 同款修法 + 自检补实盘断言 `real-repo-no-archive-collected`；② `docs/DEV-GOVERNANCE.md`「验收测试纪律」新增 §5「退役保留目录名」，钉死语义 + 三处代码承载点；landing-verdict PASS、commit `ae1dce0`）。**件数更正**：原文记归档 9 件，实测 **13 件**（已在该批申报）。证据：[260918-gov-review-closeout-chain-friction](../changes/arch/260918-gov-review-closeout-chain-friction/)。
