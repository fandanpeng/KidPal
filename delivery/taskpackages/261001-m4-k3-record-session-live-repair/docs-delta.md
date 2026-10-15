# Docs Delta · 261001-m4-k3-record-session-live-repair

> integrator 收拢制品（2026-10-01 **re-closeout**；D2 真机诊断定死后重收）。本件回答「本包文档面义务是否已全清」，并记录 spec 面（`docs/architecture.md` / `docs/requirements.md`）**检出但归属他项**的对账申报，及真机轮暴露的口径订正项。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/requirements.md#S-F2.2, docs/architecture.md#§5.2]`；`contractRefs = [interface-contract.md#§1.4, interface-contract.md#§四]`；`docsOwner = integrator`。
> **权威边界**：integrator 对 `docs/requirements.md` / `docs/architecture.md` **零写权**（human 定稿裁定 `ARQ-260929-docs-sync-spec-handoff`，`docs/crq/gov/_archive/2026/ARQ-260929-docs-sync-spec-handoff.md`）；spec 文本同步交 architect。故本件不含任何自改 spec 的 hunk，只做零改动申报 + 检出项归属申报。

## 1. 结论

- **本包 = 纯固件回归修复（Package A）**：修 `260924-m4-k3-record-session-live`（K3）落地后真机冒烟四缺陷——D1 列表延迟推送无消费方 / D2 录音态演出缺陷 / D4a 录音态 KEY 落全局 toggle 毁稿 / D4b 录音演出定时器致首页图像对象悬垂 panic。皆 K3 实现缺陷，**不引入新机制、不改任何 spec 语义**。
- **契约面：零触碰**（铁律 E2）；只读消费 LANDED §四 `kidpal_session_ctx` / §1.4 `start_recording`；`contract-lint` 闸绿（scoped r3 + final r4 exit 0）。
- **需求面（`docs/requirements.md`）：零改**（不改需求语义）。
- **架构面（`docs/architecture.md`）：零语义改动**（恢复 K3 意图行为 + D2 真机诊断的实现修复）。检出三处 spec 面事项，均**归属他项、不在本包自改**（§4）。
- **ADR 面：零改**（无新架构决策）。
- **D2 弯路如实记（见 §4.5）**：真机日志定死 D2 真根因 = **键帽派发漏 `KSTORY_PAGE_RECORDING` 分支 + REC 指示相位算术恒 off**；前两轮攻的「VOICE 覆盖层驻留」**实为弯路**——VOICE 层 round-1 即已让位（真机日志 `voice_end: top=2`）。补严缝 `[R4]`/`[R5]` 经 architect 判对 D2 为 no-op，作历史留根（齿 RSR-A6/A7 已冻结）。
- **D1 分两半（human 2026-10-01 复裁）**：冷浏览空态 = K3 已裁 dropped（`M4K3-DC-1`），human 复裁维持接受、本包不返工（pod-events `97c36553`）；推送到达后可显示 = 本包 defer 消费方，真机已证生效。此口径差记 §4.6 供后续包订正。
- **workstream / milestones**：workstream progress 追加 re-closeout 段落账；milestones M4 行 state 不动（无跳变）。
- **closeout 制品**：`delivery/changes/dev/261001-m4-k3-record-session-live-repair/`（本件 + `review-packet.md`）。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（LANDED 现文只读消费） | 只读 |
| spec·需求 | `docs/requirements.md` | 零触碰 | 只读 |
| spec·架构 | `docs/architecture.md` | **零语义改动**；检出三处归属他项申报（§4），零自改 | 只读 |
| ADR | `docs/ADR.md` | 零改（无新决策） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | 追加 re-closeout 段落账（append-only） | 记账 |
| milestones | `delivery/milestones/milestones.jsonl` | **不动**（M4 行 state 无跳变） | 只读 |
| closeout 制品 | `delivery/changes/dev/261001-m4-k3-record-session-live-repair/` | 重收（本件 + review-packet + advisory 投影） | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = `docs-sync` 闸：`scoped r3` + `final r4` 均 **exit 0**（`verifier-evidence/rounds/r3.json` / `r4.json`）。
- `docsSyncReason` 声称「arch §5.2④ 记录会话演出与层栈让位对账 + requirements 零改动」——**声明与实做一致**（本件 §1/§4：arch 面为对账并申报归属，非自改；requirements 零改动已落）。**无声明未做、无做而未声明**。

## 4. spec 面：零自改 + 检出/归属申报

### 4.1 §5.2④「引擎 manual 聆听」机制对账 → **归属 Package B（D3-甲）**

- `docs/architecture.md:213` §5.2④ 现文「引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）」与 `docs/requirements.md:431` 注记「manual 聆听 + 网关累积态已备」为**机制表述**，漂移源 = D3-甲（取甲改聆听形态 + front 累积态压制逐句回复）；触契面 §四「引擎不感知本信号」句，须 `/arch-change` Track A + 契约门定形后随 Package B。**本包不触聆听机制**，仅登记不属于本包。

### 4.2 §5.2④①「时点①恢复 = RAM 缓存填充」挂账 → **本包实现完成，文本补正随 K3 pending delta**

- `docs/architecture.md:213` §5.2④ 现文末括注「原时点①的恢复时机〔设备进 P3 语境推送 / RAM 缓存填充〕归 M4-K3 `kidpal_session_ctx` 设备半落地后重裁」。本包 D1 defer 消费方 = 该挂账「RAM 缓存填充」半的落地；K3 ctx-沿推送 = 「设备进 P3 语境推送」半。**重裁闭环**。
- **文本收口归 K3 pending delta**（`delivery/changes/dev/260924-m4-k3-record-session-live/docs-delta.md` §4.3 H2，待批态、未落地；本 worktree `docs/architecture.md:213` 实读仍旧文）。K3 H2 覆盖 provider/gating 半，**未覆盖 consumer 半**。
- **补正文本（advisory，须与 K3 pending delta H2 同批或其后落地）**：K3 H2 落地后 §5.2④① 宜追加 consumer 落点句——

  > 板端消费 defer 缓存：记录语境前台期间 `story_list` 帧只装 `g_story_*` 缓存；会话收尾 VOICE 层弹出、前台回落 `LOCAL_PAGE` 且本地页 == P3（列表态）时，渲染节点以最新缓存重渲 P3 前台（`kidpal_story_list_deferred_flush` + `kidpal_render_node_voice_end` 回落支；261001-m4-k3-record-session-live-repair 2026-10-01）。

### 4.3 D3 原描述订正申报 → **归 Package B**

- 原 D3 表述「manual 聆听 + NON_STREAM ASR **产不出 stt 帧**」**被真机反驳**：`c:/tmp/serial-live-repair-r2.log:128` 实录转写 `>> 今天是10/01放假了还要写作业，真不开心。说完了` + `:131` AI 回复 `<< 放假还有作业`——**stt 帧产出正常、AI 也逐句回复**。真实缺口在 **front 侧 story 累积/回传接线**（front 报 `empty_accum`，front 侧日志口径，非本仓串口文件）。→ **D3 订正随 Package B（D3-甲）**，本件登记供其 spec/需求面订正。

### 4.4 §5.2④ 场景④ 与 D2 实现的关系（本包不改 spec）

- D2 真机诊断定死真根因 = **键帽派发漏 `KSTORY_PAGE_RECORDING` 分支**（`lcd_display.cc` `kidpal_keybar_sync_locked`，K2 留给 K3 未落）+ **REC 指示相位算术恒 off**（`story_page.c`）。二者均为**实现缺陷**，arch §5.2④ 未述及设备侧键帽/chrome 细节 → **无 spec 漂移**。

### 4.5 D2 弯路记录（诚实；供 loop-retro / 后续包）

- 时间线：round 2（缝 [R4] `kidpal_story_voice_chrome_front` / [R5] `kidpal_story_render_page_state`）→ architect 判对 D2 **no-op**（R5 恒等、R4 甲形态返回值同旧判据），但齿 RSR-A6/A7 已冻结、留根 → round 2.1（`lcd_display.cc` 加录音态 `force_hide` 落 `kidpal_chrome_mid_sync_locked`）→ round 2.2（诊断轮，human 批「一次定死」，加 PAGE_RENDER/STORY_RENDER/RECORDING render/KEYBAR 日志）→ **round 2.3/2.5 真修**（键帽派发补 `KSTORY_PAGE_RECORDING` + REC 常亮黑块 + 时长 y 下移）。前两轮攻「VOICE 覆盖层驻留」是弯路——**VOICE 层 round-1 即已让位**（`voice_end: top=2`，r2/r3 日志）。

### 4.6 D1 两半口径 + plan 证据行口径差（供后续包订正）

- **冷浏览**（开机未开会话直接翻 P3 → `count=0` 空态）：K3 已裁 dropped（`M4K3-DC-1`），human 2026-10-01 复裁**维持接受、本包不返工**（pod-events `97c36553`）。要修需 (A) 恢复推送时点〔行为策略，260928 摘除〕或 (B) 契约门加设备 fetch 消息。
- **推送到达后可显示**（本包 D1 defer 消费方）：**真机已证生效**（退出录音态后 P3 出列表；r6 日志 `24982 render page STORY (P3) count=0`〔冷浏览〕→ `39462 render page STORY (P3) count=2`〔推送后〕）。
- **口径差**：本包 plan §1 D1 证据行（冷浏览 `count=0`）与修法（defer 消费方）**不同源**——plan 引的是冷浏览空态，修却落 defer 消费方（推送到达后才显示）。此口径差留根供后续包订正（本包按 actual 改动圈定，如实记）。

### 4.7 写权边界（如实）

- `git status --porcelain docs/` **零输出**（requirements/architecture/contracts/ADR 本批零写入）；§4 文本仅成本件记录。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/` 零输出；`contract-lint` 闸绿（r3+r4 exit 0） |
| 需求面零写入 | 同上零输出 |
| 架构面未写（如实） | 同上零输出；检出 §4.1/§4.2/§4.3（归他项）+ §4.5弯路 + §4.6口径差 |
| ADR 零改 | 同上零输出；无新决策 |
| 历史制品不改写 | K3 件零触碰，本件只 §4.2 申报其 H2 的 consumer 补正 |
| workstream | progress.md 追加 re-closeout 段落账（append-only） |
| milestones | 不动（M4 行 state 无跳变） |
