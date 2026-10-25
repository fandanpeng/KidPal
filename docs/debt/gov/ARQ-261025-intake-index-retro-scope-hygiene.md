---
id: ARQ-261025-intake-index-retro-scope-hygiene
title: intake INDEX 漂移标连续多轮无人消费 + retro 游标把 `current` 指针目录当 waiting 包
domain: gov
status: deferred
severity: COULD
owner: architect（消费口径）+ dev-loop（`changes-index.mjs` / `loop-health-report.mjs`）
trigger: 下一治理批（动 `changes-index.mjs` / `loop-health-report.mjs` 时并批）；或下一轮 retro 仍见漂移标
opened: 2026-10-25
closed: ""
closedBy: ""
paths: [".claude/scripts/changes-index.mjs", ".claude/scripts/loop-health-report.mjs"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/intake/INDEX.md]
---

**证据**：① `delivery/intake/INDEX.md` 现存三处漂移标——`261022-device-memory-prompt-overhaul`（⚠未闭环，in-flight 4/4）、`261021-p3-record-cancel-lifecycle`（⚠值域外，触面/status/anchors 全 `—`）、`261014-single-tier-bringlist-home`（⚠未闭环，awaiting-human:立项-ack 4/4）。retro 命令步骤 5 明文把「仍见漂移」列为 T5 复议触发点（升级机械闸另立批），但连续多轮 retro 未消费该信号。② `node .claude/scripts/loop-health-report.mjs --pending` 的 `waitingOnLoopHealth` 连续多轮包含 `current`——这是 `delivery/taskpackages/current/` 指针目录，不是 changeId；游标把它当作「旧格式、缺 loop-health」的包计入 waiting，制造虚假待办。

**影响面**：intake 漂移标是「检测不拦截」设计，但没有 owner 与动作，漂移会长期挂着；retro 游标的 `current` 假阳性使每轮 waiting 计数 +1，触发阈值与报告都被污染。本波三处漂移 + 一处假阳性，均为低危但稳定复现。

**建议改动**（交 review、不自批）：① 给漂移标指定消费动作与 owner（例如每轮 retro 必须逐条判「续期 / 清理 / 升级」，在 retro 报告固定节留痕）；② `loop-health-report.mjs` 在 `computeRetroScope` 源头排除 `current`（指针目录非 changeId）；③ `changes-index.mjs` 对 `status: —` 的 intake 行报出源文件缺口，而不是只显示 ⚠值域外。不新增工程门。

**风险**：riskTier: low——纯投影与游标脚本整理，不改任何闸判据。
