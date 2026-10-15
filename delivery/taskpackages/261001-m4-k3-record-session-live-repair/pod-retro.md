# pod-retro · 261001-m4-k3-record-session-live-repair（planner 第一人称热记忆）

> 写于 re-closeout（final r4 十闸全绿）之后、supervisor 机械闸 ESCALATE（high）待人类批之前。机器事实以 verifier-evidence/rounds、supervisor-verdict.json、closeout 制品为权威，本件不复述数字。

## 一、本棒最值钱的一笔：D2 走了三段弯路，最后靠「一次定死」的诊断日志翻案

**真根因 = 键帽派发漏 `KSTORY_PAGE_RECORDING` 分支**（`lcd_display.cc` `kidpal_keybar_sync_locked`；K2 注释明写「录音态键帽 [收尾]/[结束] 设备接线随 K3 录音会话接线同批落」——K3 没落）+ **REC 指示相位算术恒 off**（`flash=(rec_seconds*1000/250)%2 ≡ 0`，黑块从不可见）。

**前两轮全打偏**：round 2 的缝 [R4]/[R5]（architect 后判对 D2 为 no-op——R5 恒等、R4 甲形态返回值同旧判据）、round 2.1 的 `lcd_display` force_hide——都在攻「VOICE 覆盖层」，而 **`voice_end: top=2` 早在 round-1 就证明 VOICE 已让位**。三轮烧版、真机三轮「还是声波」，直到 human 批了**纯日志诊断轮**（`PAGE_RENDER`/`STORY_RENDER`/`RECORDING render`/`KEYBAR`），机器一次报出：录音页一直在画、键帽 substate=2 却出基层文案。

**教训（具体到动作）**：现象级 bug 连续两轮静态推演失败后，**立刻转「日志定死」**，别再猜形状。human 那句「可以考虑先把代码 commit 了，更新图索引，通过图调用查看」也是对的——图追证把「屏上结构 = s_kidpal_scr（渲染页）+ lv_layer_top（chrome 恒浮）」摊开了，定位到 `KidPalReparentChatToLayer` 那条 reparent 线。

## 二、两处「以齿代现象」的实证（与 K3 retro 同族，但这次反了方向）

- round 2 的齿 RSR-A6/A7 测 [R4]/[R5] 两个**纯逻辑缝**——**它们对 D2 的证明力为零**（architect 证：那两缝对 D2 是 no-op）。**齿全绿 ≠ 现象对**，这条在 host 147T0F 与真机三轮失败并存的现实里再次坐实。
- 反向教训：**D2 的真修点（键帽派发）在 `lcd_display.cc`，host 不可编 → 根本机判不到**。所以正确的收口不是「再加软齿」，而是 architect 建议的 **把设备轮窗口升为阻断合并的硬门**（已写入 review-packet human 项，归 human 裁）。

## 三、D1 的 plan 口径差（如实记）

plan 的 D1 **证据行是冷浏览**（`t=18845 render page STORY (P3) count=0`），**修法却是「补 defer 缓存消费方」**——两者不同源。真机两照证实：本包 D1 的后半（推送到达后可显示）**生效**（退出录音态 → 列表出 2 行），但前半（冷浏览一进去就有列表）**本包不覆盖**，human 复裁维持 K3 `M4K3-DC-1` dropped。要修需 (A) 恢复推送时点〔行为策略〕或 (B) 契约门加设备 fetch 消息。

## 四、D3 原表述被真机反驳（Package B 订正项）

原写「manual 聆听 + NON_STREAM ASR 产不出 stt 帧」——真机 r2 日志有 `>> 今天是10/01…说完了` 转写 + `<< 放假还有作业` 回复，**stt 明明回来了**；front 侧却报 `empty_accum`。真实缺口在 **front 的 story 累积/回传接线**，不是「补 stt 帧」。已在 docs-delta 记，供 Package B 立项订正（免得朝错方向修）。

## 五、human 三判（pod-events）

- `ef280e1a` defer-merge-pending-live-round（先真机复验再裁）；
- `64a4012d` scope-exclude-record-page-ui-fidelity（设计图 UI 还原度另立包）；
- `e2ce7911` accept-device-round-d2-fixed（v6 目视三样齐）；
- `97c36553` accept-coldbrowse-dropped-per-k3（D1 冷浏览维持 dropped，不返工）。

## 六、内循环 friction（诚实）

- **元数据形态红 ×4**（三版 D2 修复期间反复）：`taskpackage-required`（contractRefs/crossEnd/kind）、`acceptance-coverage`（wrapper 误入/全角括号 id）、`mutation-check`（换 acceptance 文案后 acceptanceEntry 失配）。**建议成文**：TaskPackage 元数据四条预检（参见首版 pod-retro §二）。
- **mutationEvidence 必须「实现后重放」**：本轮 K3R-A2 的 [R4]/[R5] 两条是真变异重放（builder 实测 RED verbatim），非设计态红。
- **诊断日志是「有意保留的债」**：`PAGE_RENDER`/`STORY_RENDER`/`RECORDING render`/`KEYBAR`/`chrome_*` 留在代码里，同族缺陷下次直接用。

## 七、本棒未覆盖（如实挂账）

1. D2 真修派发点（`lcd_display.cc`）host 不可编 → **只能设备轮目击**（已过 v6，但机制上不可机判）。
2. D1 冷浏览空态（human 裁接受，`M4K3-DC-1`）。
3. 录音页 UI 还原度（设计图小伴/折线波/气泡/回显行，human 裁出包）。
4. D3「说完了」+ 逐句回显 + 120s 静默归档（Package B / 单验）。

## 八、下次改进（具体到动作）

1. **现象级 bug 两轮不中即转日志定死**（本轮实证有效）。
2. 收口前**先重跑 supervisor 闸**（write-once，旧 verdict 在盘会 fail-loud；新轮需显式移除旧件）。
3. 回归修复包 start 面：把「base ref 制品 cherry-pick」写进 launch-prep skill（首版 retro §七 已提，本棒再次临场想）。
4. 归 loop-retro 素材池：「以齿代现象」正反两面实例 + plan 证据/修法不同源口径差。
