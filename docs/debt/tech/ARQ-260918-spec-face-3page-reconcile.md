---
id: ARQ-260918-spec-face-3page-reconcile
title: spec 面两文件仍写撤页前的旧实现态：`docs/architecture.md` 三处过时叙述 + 一处未收回的【修复项】标记，`docs/requirements.md` 行 166 ② 页序漂移台账行未消解
domain: tech
status: deferred
severity: NIT
owner: architect
trigger: 下一批 spec / 治理批（与 requirements §2.3 台账消解同批）；**不阻断 M2-K2 合并**（本包 review-packet 已如实登记去向）
opened: 2026-09-18
closed: ""
closedBy: ""
paths: []
source: 260917-m2-k2-retract-p4-three-page 集成期 finding（`delivery/changes/dev/260917-m2-k2-retract-p4-three-page/review-packet.md`，2026-09-18）
---

**证据**（260917-m2-k2 集成期 review-packet finding + 本包落地实证，2026-09-18）：集成角色按 `architecture.md:404` 指定的「现状权威」句核对，发现该文三处仍描述撤页前形态——`:170` 渲染节点层行写「页循环枚举 `HOME→CHAR→SCHEDULE→STORY`（注意：与 spec P1→P2→P3 页序漂移，撤 P4 后改 3 页【修复项】）」、`:380` as-is 段写「四页旧形态渲染（HOME→CHAR→SCHEDULE→STORY）」、`:382` to-be ④「撤 P4 常驻页 → 3 页循环 + `char-window` 覆盖窗」的**撤页半已达成**（覆盖窗半归 M2-K3 在办）；`docs/requirements.md:166` ② 的「页序漂移…撤 P4 改 3 页时一并理顺」对账台账行同待消解标记。实证 = M2-K2 实现已落成 `kidpal_render_node.h` 枚举 `HOME=0 / SCHEDULE=1 / STORY=2 / COUNT=3` 且 `KP_PAGE_CHAR` 整值撤出（实现 commit 在 `dev/260915-m1-k1a-key-layer-dispatch` 分支），而该包 E2 铁律明禁 dev 包改 spec，故非漏改、是归属问题。**影响面**：读者按 architecture.md as-is 认识现状会误判「页循环仍 4 页含查字页」「撤 P4 仍属未做修复项」，而 requirements 侧行 186/§2.2 早已是撤页后语义 ⇒ **spec 两面自相矛盾且与实现不等**；后续规划若照该【修复项】立包，会重做已完成的活。零机械闸被影响（`docs/contracts/**` 不在本项范围内）。**建议改动**（候选，交 review，不自批）：spec 批一次 reconcile 四处——① `architecture.md:170` 改为三页序并收回【修复项】；② `:380` as-is 改「三页循环渲染」；③ `:382` to-be ④ 拆为「撤 P4 常驻页 ✅（M2-K2 已落地）/ `char-window` 覆盖窗（M2-K3 在办）」；④ `requirements.md:166` ② 行按该文既有台账体例加消解标记（引 M2-K2 落地为凭）。owner=architect + 主线程 spec 批 landing（dev-loop 角色对 `docs/**` 无写权、不代改）。**riskTier: low**（纯文面、零行为与契约面；代价是持续误导读者与规划面）。**review profile**：Track B（三要件逐条 —— ① 执行测出 yes：集成期 finding + 本包实现实证；② 无真设计分叉 yes：照实现态对账、无方案选择；③ 半径已执行揭示 yes：两句文档 + 一行台账，零代码面）。G 层：spec 面（`docs/architecture.md` / `docs/requirements.md`），不命中 G1 定义（README/AGENTS/CLAUDE/blueprint/DEV-GOVERNANCE 五件不含这两件）、非 G2/G3/G4。机检硬排除逐项声明：不触 `docs/contracts/**`；无 spec-contract drift（本项是 spec↔实现对账，不涉契约文本）；不改任何 agent 定义/tools/name/model/write scope；不新增或改动任何门；非 mixed lane；非 L0 蓝图。便宜验证计划：`spec-anchor-check.mjs`（锚完整性，改后无悬空 `arch §x.y` 引用）+ landing 后复读 `architecture.md` 现状权威句与改后一致。
