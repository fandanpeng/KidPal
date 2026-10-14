---
id: ARQ-260912-host-werror-comment-acceptance-gap
title: "host 验收 runner 被冻结验收件注释触发 `-Werror=comment` 编译失败——`schedule_p2.acceptance.c:1103` / `story_p3.acceptance.c:2233` 注释内含 `docs/contracts/**` 序列（`/*` 嵌套形态），260911-render-pages 合并后 host runner 从未编译通过（缓存 ELF 早于 merge 23 分钟，绿信号一度是陈旧产物）"
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-12
closed: 2026-09-18
closedBy: ""
paths: []
source: ""
---

构建面候选②落地：`-Wno-error=comment` 钉进 `firmware/main/test/host_app/main/CMakeLists.txt:121`（含成因注释；仓库内显式事实、非会话环境变量，fullclean 后不丢）。生成面候选①未做——冻结验收件不可改，构建面显式豁免即等效替代，无需另行成文。**本行原「来源」单元格被 ARQ-260913-device-round-protocol 整行覆盖，2026-09-18 修表格时按行内证据重建来源**（= 260911-render-pages fixer 轮披露）并结移 §2。
