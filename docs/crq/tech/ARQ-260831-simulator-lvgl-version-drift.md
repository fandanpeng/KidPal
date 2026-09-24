---
id: ARQ-260831-simulator-lvgl-version-drift
title: 模拟器↔固件 LVGL 版本错位是常驻风险（本次 9.5→9.3 已对齐，构建期校验待自动化）
domain: tech
status: accepted-risk
severity: SHOULD
owner: ""
trigger: 固件 LVGL 组件锁变更 / 下次 simulator 相关 dev-loop 包立项（补构建期版本校验）
opened: 2026-08-31
closed: ""
closedBy: ""
paths: []
source: 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V1）
---

TP-C（260831-simulator-lvgl-align，2026-09-01）已把模拟器 LVGL 降到 9.3.x 与固件同版（`lv_version.h` 三号一致 + MSBuild exit 0 编译通过），本次 9.5 vs 9.3 错位已消除。残余 = 「版本对齐的构建期校验」仍未做成自动门：模拟器↔固件 LVGL 版本同步眼下靠人工记得，固件每次升 LVGL 组件锁仍可能无声再漂。原风险实证保留备查（详见 ARQ-260831-i1-indexed-asset-on-device）：9.5 软件渲染的 I1 索引图像路径绕过调色板——「模拟器结论不直接当真机结论」纪律仍须双端同资产验证。建议下次 simulator 相关包落地时，把「`lv_version.h` 三号 == 固件 dependencies.lock」做成构建期断言。
