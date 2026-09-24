---
id: ARQ-260831-simulator-truecolor-vs-1bpp
title: 模拟器（真彩灰度）与真机（1bpp 阈值化）的显色差异是第二类失真
domain: tech
status: accepted-risk
severity: COULD
owner: ""
trigger: simulator 相关 dev-loop 包立项 / 真机字体子集化任务
opened: 2026-08-31
closed: ""
closedBy: ""
paths: []
source: 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V6）
---

同一张 UI 在模拟器上细线/小字可读（FreeType 灰度抗锯齿），阈值化后可能糊掉（实测设计稿 12–16px 文字阈值化后明显糊边）。建议 simulator 包顺手做「画布快照阈值化 PNG」一键自查，验收图一律以阈值化版本为准；真机字体子集化时 16px 以下 CJK 的字号下限需实测。
