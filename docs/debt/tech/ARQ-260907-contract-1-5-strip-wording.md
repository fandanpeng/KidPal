---
id: ARQ-260907-contract-1-5-strip-wording
title: 契约 §1.5「display 切分」职责措辞与实况存在精度差：物理摘取在引擎部署副本，契约只写了网关职责
domain: tech
status: deferred
severity: COULD
owner: architect（契约门）
trigger: 下次契约门（触及 interface-contract.md 的任何批次顺带校准）
opened: 2026-09-07
closed: ""
closedBy: ""
paths: []
source: "260907-g-relay-fix（review-packet Findings #4 + taskpackage decisionCoverage contract-§1.5 注记，2026-09-07）"
---

**证据**（260907-g-relay-fix 收口，2026-09-07）：契约 §1.5 写「display 切分与机械校验是网关（front）职责」；human 批准的方案 A（pod-events `engine-patch-approval`）落地后，fence 的**物理摘取**（流式状态机剥离）实际落在引擎部署副本（`server/deploy/engine-patch/connection.py` + `kidpal_display_strip.py`），front 是**校验/下发唯一权威**（display_schema V27 + kidpal/display relay + 回执消费）且保留单帧纵深防御 strip——职责语义不冲突（front 职责履行未被削弱），但「切分」一词的执行位与契约字面不符。**影响面**：仅措辞精度，无行为漂移；将来按契约字面重实现或评审的会话会误判「引擎侧剥离属越权」。**处理**：本包不改契约（契约门纪律），下次契约门把 §1.5 措辞校准为「机械校验与下发唯一权威在网关；物理剥离执行位可为引擎侧（经批准的部署形态），front 保留纵深防御」。**riskTier: low**。owner=architect（契约门）。

**trigger 触发留痕（2026-10-12 integrator）**：契约批 261011-p1-home-v5-rework（CDQ-261011-p1-home-v5-rework）触及 interface-contract.md，本条 trigger 字面触发；但该批 CCP 四触点（preview 数组化 + 示例值 + seam 签名 + 变更记录行）**不含 §1.5 校准**，human 契约门批准范围不含本条处置内容——不扩批、不就地改契约，本条维持 deferred，校准顺延至下次含 §1.5 触点的契约门。
