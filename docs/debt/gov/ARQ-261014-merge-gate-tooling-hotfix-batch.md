---
id: ARQ-261014-merge-gate-tooling-hotfix-batch
title: 合并闸工具链热修两例未入账留痕 + 生成件（INDEX.md）与手写相撞的渲染器权威口径缺口
domain: gov
status: deferred
severity: COULD
owner: architect（口径修订）+ dev-loop（脚本核验）
trigger: 下次治理批 / 下个动 merge-driver.mjs 或 pre-merge-escalation-gate.mjs 的批（与 ARQ-261011 同脚本可同批）
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/scripts/merge-driver.mjs", ".claude/scripts/pre-merge-escalation-gate.mjs"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据**：① **merge-driver ReferenceError**：260924-m4-k2 merge-driver.mjs Step4 `ReferenceError: target is not defined (line 1449)`，pod 卡 merging 态（event `a8a3b44c`，gate-tooling-bug / high），human 热修 commit 0303e98 复跑越点（`baee1996`）；② **detect-changes CLI 失配**：defaultDetectChangesCli(:147) 与 CLI 0.10.8 输出形态失配，quick-check 恒 fail-closed（event `9b1181bc`，gate-tooling-bug / high），human 裁方案 1 落 commit 670139a，幂等续跑至合并完成——两 bug 已热修在树但从未入 CRQ 账本，本条目补登记留痕并建议各补一枚 verify 齿防回归；③ **生成件相撞族**：261013-tomorrow-row `502f2515` pre-merge 闸 BLOCK 第二理由 parallel-overlap-tier3——本分支 INDEX.md 与 main 2ce0d42 changes-index 全量重建同 hunk 相撞，「生成件以渲染器为权威」的口径闸不知道（与 260924-m4-k1 retro ⑦「--upsert 与 dev lane 冲突」留根同族）；④ **附带教育面三例**（闸行为正确、文献缺）：required-check 参数形状踩两次（260929-char-window-v5，须无参走 current/ 指针）、final 相位裸跑被 fail-closed 举手（261012-topbar / 260928-char-window-key「正确序列是 scoped 绿 → integrator 圈定 → final」）、Windows 下 /proc UUID 不可用应直用 node crypto.randomUUID()（260929-char-window-v5）。

**修法候选**（交 review，不自批）：① 闸对生成件（INDEX.md 类渲染器产物）的 overlap 判定改按渲染器权威口径——豁免或提示重跑渲染，而非 BLOCK；② ①② 两热修补 verify 齿防回归；③ ④ 的教育面写进 planner.md 或 launch-prep 文面一句（参数形状 / 相位序列）。**riskTier: low**。

**追加证据（loop-retro-2026-10-25，25 包复盘）**：④ 教育面在 final 相位复现八包——261013-lang-config（scope-file 前置未进派单模板）、261014-engine-stop（final r4 缺 --scope-file；ordinal 语义靠试错）、261018-sd-log-usb-read（final scope-file 依赖低估）、261019-p3-record-dialogue-guide（连续第二轮）、261020-p3-rec-frame-visual（首次撞）、261022-m5-k1（final 轮缺 scope-file）、261022-p3-record-chrome-ghost-fix（final round 必须是 next completed ordinal）、261023-agents-md（沿用上一包 round 号误派）。另有 261014-voice-round 记「补轮重跑前须显式删旧 write-once verdict，报错不自解释」。③ 生成件相撞新时点：261016-sd-card / 261018-sd-log-persist 的 current/ 投影与 delivery/review/INDEX.md 双向 append 零语义冲突仍被 parallel-overlap tier3 拦。建议升 severity/priority；本波每包至少烧半轮到一轮。证据：delivery/retro/2026-10-25-loop-retro.md §二 T1/T11。
