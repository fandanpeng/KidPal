---
id: ARQ-261014-device-round-de-facto-hard-gate
title: 真机轮已成 high tier 事实上的阻断合并硬门——defer-merge-pending-live-round 常态链高频复现，但真机验证责任位（烧录/部署）与成文缺位
domain: gov
status: deferred
severity: SHOULD
owner: architect（OS/DEV-GOVERNANCE 文面）+ 主线程（责任位裁定）
trigger: 下次治理批；或下个动 DEV-GOVERNANCE 合并门节 / launch-prep 文面的批
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/references/dev-loop-operating-system.md", ".claude/skills/kidpal-pod-launch-prep/SKILL.md"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据**：① **责任位缺位实录**：261001-m4-k3-record-session-live `260b1f48`——human 真机初测发现问题后根因竟是「三包零烧录零部署」，human 指示「立马执行烧录和部署」：谁烧录、谁部署、何时，launch/finalize 面无条文，靠 human 现场推；② **常态收口链三连**：261001-m4-k3 `ef280e1a` / 261001-m4-k3-story-archive-order `39386ee6` / 261010-device-reconnect `f499501b` 均为「supervisor ESCALATE → defer-merge-pending-live-round → 真机轮 → approve-escalated-merge」同型——真机轮实质是 high tier 的阻断合并硬门，但流程条文未承认它；③ **红利实证**：261012-topbar `dd77c762` TB-A4 真机轮四项逐项记录中白捡既有缺陷归因（立 TECH-261013-p3-reader-entry-no-text-silent-noop）；④ **「以齿代现象」双向实证**：261001-m4-k3-repair retro——RSR-A6/A7 齿全绿对 D2 证明力为零、D2 真修点 host 不可编根本机判不到；261010-device-reconnect retro 同判（host 结构齿全绿 ≠ 行为全对，断链藏在态跃迁里）——两包独立建议「设备轮升阻断合并硬门」「行为面观察单列 builder 模板节」。

**影响面**：high tier 包每包都靠现场即兴安排真机轮与烧录部署，节奏 / 责任 / 豁免条件全靠人记；漏烧录漏部署到合并后才暴露（① 实证）。

**修法候选**（交 review，不自批）：① DEV-GOVERNANCE 把「riskTier=high ⇒ 真机复验轮为合并前置」成文，含豁免条款（纯 host 可判面 + human 明示豁免）；② launch-prep 启动卡把烧录 / 部署前置件与责任位写进第五要素之后；③ 诊断日志保留债同文钉住（261001-m4-k3 纯日志诊断轮一次定死 D2，实证复用价值）；④ builder 模板补「行为面观察」申报节（不新增闸，写作约定）。**riskTier: low-medium**（成文动作，不改既有闸机器判据；不与 human 2026-09-23「不用每次都请示」口径冲突——真机轮是验证动作不是请示动作）。
