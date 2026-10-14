---
id: ARQ-261014-supervisor-escalate-preflight-ignored
title: supervisor/pre-merge 机械闸不消费 launch-card 前置申报与同款先例——all-mock 类 known-pattern 升级多连发，human 逐次人批同型包
domain: gov
status: deferred
severity: SHOULD
owner: architect（闸判据修订）+ dev-loop（脚本落地）
trigger: 下次治理批 / 下个动 supervisor-verdict-gate.mjs 或 pre-merge-escalation-gate.mjs 的批
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/scripts/supervisor-verdict-gate.mjs", ".claude/scripts/pre-merge-escalation-gate.mjs"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据（loop-retro-2026-10-14，三连发 + 同型先例）**：① 261010-p3-storylist-cold-start-restore：supervisor ESCALATE critical（触契约 SSOT critical path），human 批准本地合并（pod-events `d13b108a`）——升级事件本体未入 pod-events、仅 human-decision note 侧写；② 261010-prompt-v9-schedule-anti-narration：ESCALATE 三理由（all-mock 前置申报先例 / security-axis 误报实证 / 升档为机械后果），human 逐条判读后批准（`e02f05e3`）；③ 261012-kidpal-realtime-facts：all-mock 判定（全文件内容锚 + live 观测项、无 DB/事件断言）→ riskTier medium→high → ESCALATE（`81b32bfb`），human `9d4c1f68` 人批。另有 261012-topbar `afd16e19` 档位确认、261010-device-reconnect `f499501b` 等同型出场。**共性**：launch-card ⑦ 已前置申报 all-mock / 验证形态，且同款先例在册（早批人批记录躺在 pod-events 里），闸一律全 diff 重算升级、不消费申报与先例——human 每次都在批同型包。

**影响面**：human 门噪声（每包 high tier 收尾必出场一次同型裁决）+ 收尾链路时延；对冲风险 = 机械降噪可能漏真异常，故降级判据必须逐轴白名单、只折抵「已人批的同款先例」，不可折抵新轴。

**修法候选**（交 review，不自批）：① 闸消费 launch-card ⑦ 申报字段 + 在册先例（changeId 引用），命中已人批同型即降级为 advisory 留痕、不再阻断；② human 批准语义结构化（approvedVia + 同型 pattern 标签）累积先例库，供 ① 消费；③ 最小成本解：不动闸判据，只在 ESCALATE 报错输出里带一句「同款先例 changeId=…（在册），human 可凭先例快批」。倾向 ①+②，③ 兜底。**riskTier: low**（判据降噪面，须逐轴白名单防新轴漏升；不改契约面、不涉 spec）。
