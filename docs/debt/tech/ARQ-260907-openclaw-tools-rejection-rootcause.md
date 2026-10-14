---
id: ARQ-260907-openclaw-tools-rejection-rootcause
title: "live 引擎 `Intent:nointent` 根因未查：OpenClaw 拒收 tools 数组（LLM tool calling 不可用），Intent 能力回开成本待重估"
domain: tech
status: deferred
severity: SHOULD
owner: architect（定方向）+
trigger: 下次架构治理批 / OpenClaw 升级或 B 方案重估时
opened: 2026-09-07
closed: ""
closedBy: ""
paths: []
source: 260829-voice-roundtrip（live evidence 遗留）+ 260907-g-relay-fix 收口转登记（2026-09-07）
---

**证据**（260829-voice-roundtrip live 实测遗留，WS 包 review-packet SHOULD #2；260907-g-relay-fix 收口时仍挂）：live 一轮会话引擎恒出 `Intent:nointent`，根因是 OpenClaw（`type:openai` 接入）对带 tools 数组的请求拒收——LLM tool calling 全链不可用，引擎侧无法自行发起任何 tool-call（含 kidpal/alert 提醒、未来 Intent 类能力）。260907-g-relay-fix 的 display relay 由 front 发起、不依赖 LLM tool calling，故本项不阻塞已交付语义；但 tools 修复回开 Intent 后引擎侧可能自行 tool-call，front relay 需幂等/去重意识（taskpackage notes 已载实现注记）。**诊断资源**：OpenClaw 开源克隆在 `c:/AI/OpenSourceReference/openclaw-main`（只读参考），可对照拒收路径查根因。**建议动作**（候选）：① 用克隆定位 tools 数组拒收的具体校验点与版本条件；② 重估 Intent 回开成本（引擎侧 few-shot/配置 vs OpenClaw 升级/替代）；③ 回开前给 front relay 补幂等/去重注记的落地核验。**riskTier: low**（能力缺失非缺陷，已有 front 发起 relay 兜底）。owner=architect（定方向）+ dev-loop（诊断/落地）。
