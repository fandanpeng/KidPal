---
id: ARQ-261001-host-blind-outcome-device-gate
title: 「用户可见、host 不可编」的结果面缺可机判门——建议把设备轮窗口从 deferred 升为阻断合并的硬门
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（dev-loop 门 / 纪律文面；或下一个声明「用户可见结果」的包 launch 前）
opened: 2026-10-01
paths: [.claude/hooks/verify.mjs, .claude/verify-gates.json, .claude/scripts/pre-merge-escalation-gate.mjs, .claude/references/dev-loop-operating-system.md]
source: [delivery/changes/dev/261001-m4-k3-record-session-live-repair/review-packet.md, delivery/taskpackages/261001-m4-k3-record-session-live-repair/verifier-evidence/rounds/r4.json, delivery/taskpackages/261001-m4-k3-record-session-live-repair/pod-events.jsonl, delivery/changes/dev/261001-m4-k3-record-session-live-repair/pod-retro.md]
---

**证据**：包 `261001-m4-k3-record-session-live-repair`（Package A）D2「录音态声波覆盖层压住录音页」。

- **host 全绿 ≠ 现象对（实证）**：冻结齿 `RSR-A6`/`RSR-A7` 测两缝 `[R4] kidpal_story_voice_chrome_front` / `[R5] kidpal_story_render_page_state`（`firmware/main/display/story_page.{c,h}`），host 连轮 **147 Tests / 0 Failures**；architect 独立分析证 **这两缝对 D2 为 no-op**（R5 消费点两入参传成同一个 `s_story_view.page` = 恒等函数；R4 在甲形态 `layer_top=LOCAL_PAGE` 时返回值与旧内联判据**完全相同**）——即**齿绿对 D2 的证明力为零**。
- **真修点恰在 host 不可编面**：D2 真根因 = `firmware/main/display/lcd_display.cc` 的 `kitpal_keybar_sync_locked` **派发 `if` 漏 `KSTORY_PAGE_RECORDING`**（K2 注释明写该接线「随 K3 同批落」而 K3 未落）；次因 = `story_page.c` REC 指示相位 `flash=(rec_seconds*1000/250)%2 ≡ 0` 恒 off。`lcd_display.cc` **不在 host 编译面** → 无法机判。
- **代价（实测轮数）**：真机日志 `c:/tmp/serial-live-repair-r{3,4,5,6}.log` 记录 **连烧 4 版**、前两轮攻「VOICE 覆盖层」（而 `voice_end: top=2` 早在首轮即证 VOICE 已让位）；直到 human 批**纯日志诊断轮**（`PAGE_RENDER`/`STORY_RENDER`/`RECORDING render`/`KEYBAR` 四类行），机器一次报出「录音页一直在画、键帽 `substate=2` 却出基层文案」才定位。
- 收口：final r4 十闸全绿 / `verifier-evidence-check` factOk；human 批合并（pod-events `e2ce7911` 设备轮通过、`002e5c10` approve-escalated-merge）；architect advisory + human 裁「记录 gov ARQ」。
- 同族历史：K3 `260924-m4-k3-record-session-live` pod-retro 已记「host 齿为纯逻辑决策表、真机沿未目击」——本条是**同一族的第二次实证**，且这次反证了「用纯逻辑缝补覆盖」的做法。

**影响面**：凡「验收条声明的是**用户可见结果**、而该结果的实现/派发落在 host 不可编面（`lcd_display.cc` 等 LVGL 合成 / 板级 wiring）」的包，host 齿可以全绿而真机现象错——**带病合并风险**。发生频率：本轮 1 次即烧 4 版 + 3 轮误导性修复；后续每个声明可见结果的固件包都在同一风险面上。

**建议改动（候选，交 review、不自批）**：
1. **设备轮窗口升为阻断合并的硬门**：对「验收条含用户可见结果且实现面 host 不可编」的包，把相应真机目击窗口从 `deferred 目击项` 升为**合并前必过**（判据 = 串口/目击证据落到包制品，缺失即 gate BLOCK）。
2. **或**（互补）**把结果决策抽成 host 可测缝**：如把 `lcd_display` 的键帽派发谓词抽成 `keybar.c` 内纯函数（host 可编），使「RECORDING → story_slots」这类**决策**可机判（本轮 bug 正落在该决策上）。
3. 两条可并存；1 治通用面、2 治可抽缝的具体面。

**风险**：riskTier: **high**——候选改动直接动**门机制**（`.claude/hooks/verify.mjs` / `verify-gates.json` / `pre-merge-escalation-gate.mjs`，机推 **G3**）+ 合并纪律文面（`dev-loop-operating-system.md`）；且「用户可见结果」的判定边界含设计分叉（哪些条算、窗口怎么算过）。按 skill 纪律「新增/修改任何 gate 本身就是 lightweight 机检硬排除」→ **review profile = full-adversarial + human（Track A）**，不得回标 Track B。owner = architect（门机制面裁定）。

**追加证据（loop-retro-2026-10-14，18 包复盘——常态链与责任位增量，原条目判据不变）**：① **defer-merge-pending-live-round 已成 high tier 常态收口链三连**：261001-m4-k3 `ef280e1a` / 261001-m4-k3-story-archive-order `39386ee6` / 261010-device-reconnect `f499501b` 均为「supervisor ESCALATE → 真机复验 → approve-escalated-merge」同型——真机轮实质是事实上的阻断合并硬门，流程条文未承认；② **烧录/部署责任位缺位实录**：260b1f48 human 真机初测发现问题，根因竟是「三包零烧录零部署」，human 指示「立马执行烧录和部署」——谁烧录、谁部署、何时，launch/finalize 面无条文；③ **红利实证**：261012-topbar `dd77c762` TB-A4 真机轮四项逐项记录中白捡既有缺陷归因（立 TECH-261013-p3-reader-entry-no-text-silent-noop）；④ **同族第三波独立再判**：261010-device-reconnect retro 独立得出同一结论（host 结构齿全绿 ≠ 行为全对，断链藏在态跃迁里；fixer 四关点补钉无 host 测试承载，靠真机轮 RC-A4/B4/B5/B6 兜底）——与本条目 source 的 261001 D2 实证互为独立复证；⑤ 伴生建议两条随本条处置：诊断日志保留债成文钉住（261001 纯日志诊断轮一次定死 D2，复用价值实证）、builder 模板补「行为面观察」申报节（261010 builder 主动上交 [S7] 运行时断链是本波最值钱交付质量）。证据：`delivery/changes/retro/2026-10-14-loop-retro.md`。
