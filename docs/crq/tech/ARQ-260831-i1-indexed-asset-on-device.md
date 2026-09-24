---
id: ARQ-260831-i1-indexed-asset-on-device
title: I1 索引格式在真机 LVGL 9.3 + SPIFFS 文件源路径下的可用性未验证
domain: tech
status: accepted-risk
severity: SHOULD
owner: ""
trigger: 真机联调期（TP-B B8 已核：未超预算、不触发 I1）
opened: 2026-08-31
closed: ""
closedBy: ""
paths: []
source: 功能支撑验证批（2026-08-31，PROPOSAL §2 真机留根项）+ ui-implementation-strategy §6
---

实战证实 9.5 软件渲染直渲路径下 I1 调色板被绕过（调色板仅 bin-decoder 对文件源走 `decode_indexed_line` 生效），故资产管线基准直出 RGB565。但 RGB565 占 Flash 是 I1 的 16 倍（P1 一套已约 96KB，大时间字块独占 55KB）；若 I1 在真机 9.3 + 文件源路径可用，可回收约 16 倍空间。功能支撑验证批 human 已裁本批不烧真机，此项留待真机联调期验证；若 TP-B（260831-ui-art-assets）的 B8 资产总量预算核算判定必须上 I1，则此项优先级升 P0。
