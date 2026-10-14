---
id: ARQ-260915-taskpackage-display-numbering
title: TaskPackage 显示编号机制（承重/游离 K/N + 波次 + workstream 短码）——roadmap 列车图上编号一眼可读主干/侧线/波次，编号由依赖图机械推导非人工赋值
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-15
closed: 2026-09-15
closedBy: ""
paths: []
source: ""
---

human 指示直改、跳过 Track B（小改，如实记录）：定义落 `.claude/references/task-intake-and-slicing.md` 拆包链节「TaskPackage 显示编号」（形态 `<WS>-<KN><波次><并行字母?>`；K=承重=被 in-scope 下游依赖或主干终点、N=游离=无人等待侧线；波次=依赖深度+1）；推导与渲染落 `render-roadmap.mjs`（assignDisplayIds 全量重算 + 状态表编号列 + 图例随 ② 节渲染 + self-check 扩 7 断言）；WORKSTREAM-ROADMAP.template.md ② 注释补指针；m1-home-qa roadmap 随 base 分支补 commit 重渲染实证（M1-K1a/N1b/K2/K3/K4）。触发 = human 读 m1-home-qa roadmap 发现包名不含依赖信息（2026-09-15 会话），方案两轮讨论定形（先 W 波次案 → 弃 W 字母、改承重/游离判据并钉「被依赖∨主干终点」机械式）；常设规则追补（2026-09-15 human 裁）：changeId 嵌编号 token 升为拆包建包常设规则（task-intake「TaskPackage 显示编号」节 changeId 嵌入段——`YYMMDD-<ws>-<kn><波次><字母?>-<slug>`，建包时点镜像、键名冻结不追溯），本批五包已按规则改名（base/m1-home-qa commit 21907df）。
