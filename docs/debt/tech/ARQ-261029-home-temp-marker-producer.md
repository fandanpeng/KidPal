---
id: ARQ-261029-home-temp-marker-producer
title: F-9 首页临时事件下划线——上游写入侧从不往 name 落 ※ 前缀，[U1] 机制真机永不触发
domain: tech
status: blocker
severity: MAJOR
owner: ""
trigger: P1 包（261028-m6-k2a-p1-home-16px）acceptance-author 冻结轮跨端核对（2026-10-29）
opened: 2026-10-29
closed: ""
closedBy: ""
paths: ["server/gateway/schedule.py", "firmware/main/display/home_p1.c"]
source: delivery/taskpackages/261028-m6-k2a-p1-home-16px/taskpackage.json notes 升级② + human 2026-10-29 裁定（pod-events / AskUserQuestion 留痕）
---

P1 包 acceptance-author 冻结轮跨端核对实况：server/gateway/schedule.py 的 home_upcoming_payload 逐项 {time,name} verbatim——**现网 producer（front / OpenClaw 写入侧）从不往 name 写任何临时标记**。服务端临时性由 flags bit0 承载，但 home 载荷剥 flags（wire 不变是 F-9 的原裁定），v1.68 线框「春游 ※临时」从未在设备落地。P1 冻结件 [U1] 钉的「name 首字符 ※（U+203B）= 临时」是 firmware 消费半唯一零 wire 通道——机制本身可全绿，但上游不写 ※ 则真机永不触发。

**human 2026-10-29 裁定**：上游补写入侧任务——临时事件创建路径（front / OpenClaw 工具写入侧）在 name 落 `※` 前缀，wire 不变、不扩字段，符合 F-9 原裁定「不改 wire、不扩字段」；须在 M6 统一真机轮（P0 探针 + P1 首页 + P2/P3 终端轮）之前落位，否则该轮下划线观测无样本。

**处置去向**：server/OpenClaw 侧单面实现小任务——走 /triage-change 分流立包，或并入既有 server 侧批次；完成判据 = 创建临时事件后设备侧 home 载荷 name 首字符为 0x203B（真机统一轮观测）。P1 固件消费半不阻塞本条目（[U1] 断言照钉，[M] 键 p1_home_p1_h_no_temp_field 负向锁不扩字段）。
