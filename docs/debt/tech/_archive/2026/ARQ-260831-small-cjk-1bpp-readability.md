---
id: ARQ-260831-small-cjk-1bpp-readability
title: 16px 以下 CJK 文字在 1bpp 真机的可读性下限未实测，直接锁死 V3 设计稿字号下限
domain: tech
status: closed
severity: SHOULD
owner: ""
trigger: 真机字体子集化任务 / V3 设计稿字号定稿前
opened: 2026-08-31
closed: 2026-10-28
closedBy: "consumed-by: 261028-font-typography-scaleup（change-intake §3a 立项消费结项）"
paths: []
source: 功能支撑验证批（2026-08-31，PROPOSAL §2 真机留根项）+ ui-implementation-strategy §6
---

实战（V6）发现设计稿 12–16px 细字经阈值化后明显糊边——模拟器（FreeType 灰度抗锯齿）上可读的细字，真机 1bpp 未必成立。V3 设计稿（实现导向）能用多密的排版、多小的字，取决于本项实测出的字号下限。功能支撑验证批 human 已裁本批只模拟器+云端、不烧真机，故留真机字体子集化时实测。

---

## 结项（2026-10-28）

- **结项方式**：consumed-by 立项消费（change-intake §3a）——字号下限已由 2026-09-15 真机九档对照实测钉定（14px 清晰 / 12px 不可辨，commit 70c19eba），且 2026-10-28 复测轮再看大字号档（16/18/20/24）后，本条「下限未实测」风险正式由立项消费：**261028-font-typography-scaleup**（workstream `delivery/workstreams/261028-font-typography-scaleup/`，spine `delivery/intake/261028-font-typography-scaleup.md`）——正文 16px / 标题 20px 的字号体系升级列车，在其上游需求批与字体 spike 包中正式承载字号口径。
