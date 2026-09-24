---
id: ARQ-260916-server-mvp-teeth-sweep
title: server 面可能仍有 MVP 验证期孪生/伴随验收齿未随 544a804 归档批退役——已实证一例：`test_voice_roundtrip_b.acceptance.py` server 半边（设备半边 C 孪生已归档，VRT-A2 孪生对齐齿钉旧址故红，human 裁定 `bba7d239` 模块级 skip 退出现役销账）
domain: tech
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-16
closed: 2026-09-16
closedBy: ""
paths: []
source: ""
---

260915-m1-n1b-p1-home-battery integrator 登记（登记由主线程派单）：留独立批 sweep——对 `server/acceptance/**` 存量齿逐件核「设备半边/锚定路径是否已随归档批失效」，失效者按 human 裁定通道退役；**不阻本包**（本包 r12 final 已唯余 SMK-A4/SRE-E4 在册红，见 `changes/dev/260915-m1-n1b-p1-home-battery/existing-red-inventory.md`）。**已闭（2026-09-16，human 裁不走 Track B、快速关闭）**：sweep 已执行——`server/acceptance/**` 存量 10 件 MVP/验证期冻结验收（260829–260907 era）整体 `git mv` 归档至 `server/acceptance/archive/` + README 登记（镜像 9-15 固件归档批形态，收集 glob 天然不命中、零治理脚本改动；commit `77b6b0c`，pod-events `03112981`）；R1（SMK-A4）/R2（SRE-E4）随归档自然消红；`server/tests/test_reminder_units.py` human 裁保留现役；残余影响 = server/gateway 离线守卫清零，由 M1 后续包 acceptance-author 重钉补位。
