---
id: TECH-261023-openclaw-tool-face-prompt-only-readonly
title: kidpal agent「数据目录只读」是 prompt-only 防线——tools.allow 仍含 write/exec，且 workspace 内 kidpal/、stories/ 为 symlink 指向 main workspace 真身，一次 stray write 可改真课表/真故事
domain: tech
status: deferred
severity: SHOULD
owner: human（立项裁决）
trigger: 下个动 OpenClaw 工具面 / 需与 K1-E4 冻结验收同批的包；或出现 stray write 迹象时提前
opened: 2026-10-23
closed: ""
closedBy: ""
paths:
  - server/deploy/261022-openclaw-kidpal-tools/
  - server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md
source: "workspace-kidpal/AGENTS.md advisory review S2（delivery/review/261022-arch-review-device-memory-prompt-overhaul/agents-md-advisory-20261023.md）；human 2026-10-23 指示记 debt、不入批 A+B 文本批"
---

## 现状与风险

- AGENTS.md 底线「数据只读」(L17/L85) 对 `kidpal/`、`stories/` 两数据目录禁写——但这是 **prompt-only 防线**:live `tools.allow=["read","write","exec","web_search"]`(K1-E4 声明、K3 honcho plugin 的 drift_guard 现状锚)里 write/exec 都在,工具面并不阻止写。
- `workspace-kidpal/kidpal/`、`stories/` 是指向 main workspace 真身的 **symlink**——本 agent 一次 stray write(或 exec 间接写)落在 symlink 上即改真课表/真故事,main 侧单写者纪律(schedule-write skill / front archive_story)被旁路,且无机械闸可拦。
- 风险量级:概率低(守则 + 回合路由多重约束)但后果直击数据完整性;「机械优先于 prompt」原则下,能用工具面收窄消掉的防线不应长期靠提示词独扛。

## 候选修法（advisory S2 原案，立项时复核）

- 工具面收窄至 `["read","web_search"]`:`exec` 在 K1 收窄时的现存用途 = skill 的 `date` 推算,课表机械注入落地后摘要已自带星期标签,该用途可复核是否已消;`write` 的现存合法用途 = 提醒语义产物 JSON 写 `/opt/kidpal/alerts/`(交接目录、原子写归确定性脚本)——若该路径改为脚本独立承载或经 main 侧,write 可撤。
- **必须同批联动三处**:① `261022-openclaw-kidpal-tools/` 声明工件 changes;② K1-E4 冻结验收 `test_261022_openclaw_kidpal_tools.acceptance.py` 的 allow 断言(tighten-only);③ K3 `261022-openclaw-honcho-plugin/` 声明工件的 drift_guard 现状锚(钉死 P1 已应用态 tools.allow 四项——先改工具面不改此锚 = apply exit 3)。
- AGENTS.md「数据只读」底线在收窄落地后从「prompt 防线」降级为「冗余声明」,可保留作纵深。

## 关联

- 批 A+B 文本包 `261023-agents-md-static-semantics-alignment`(advisory B1/B2 等文本修;本条 S2 刻意不与其混装——文本批 Lite、本条动工具面与冻结验收,归批独立)。
- 上游 advisory 报告同文 S2 节;K1-E4 工作台与 K3 honcho plugin 声明工件为联动受影响面。
