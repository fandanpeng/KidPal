# Docs Delta · 261001-m4-k3-record-session-live-repair

> integrator 收拢制品（2026-10-01）。本件回答「本包文档面义务是否已全清」，并记录 spec 面（`docs/architecture.md` / `docs/requirements.md`）**检出但归属他项的**对账申报。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/requirements.md#S-F2.2, docs/architecture.md#§5.2]`；`contractRefs = [interface-contract.md#§1.4, interface-contract.md#§四]`；`docsOwner = integrator`。
> **权威边界（本件硬前提）**：integrator 对 `docs/requirements.md` / `docs/architecture.md` **零写权**（human 定稿裁定 `ARQ-260929-docs-sync-spec-handoff`，`docs/crq/gov/_archive/2026/ARQ-260929-docs-sync-spec-handoff.md`）；spec 面任何文本同步交 architect 承接。故本件 **不含任何自改 spec 的 hunk**，只做（a）零改动申报 + （b）检出的 spec 面归属他项申报。

## 1. 结论

- **本包 = 纯固件回归修复（Package A）**：修 `260924-m4-k3-record-session-live`（K3）落地后真机 P3 冒烟暴露的四缺陷——D1 列表延迟推送无消费方 / D2 会话层压住录音页 / D4a 录音态 KEY 落全局 toggle 毁稿 / D4b 录音演出定时器致首页图像对象悬垂 panic。四项皆为 **K3 实现缺陷**，修复使设备行为**回到 K3 声明的意图行为**，**不引入新机制、不改任何 spec 语义**。
- **契约面（`docs/contracts/interface-contract.md`）：零触碰**（铁律 E2）。只读消费 LANDED 现文（§四 `kidpal_session_ctx` / §1.4 `start_recording`），wire 形状零变；`contract-lint` 闸绿（scoped r1 + final r2 均 exit 0）。
- **需求面（`docs/requirements.md`）：零改**。本包不改需求语义（S-F2.2 行为权威冻结在前，本包只消费）。
- **架构面（`docs/architecture.md`）：零语义改动**。本包 = 恢复 K3 既有意图行为的实现修复，无新架构陈述。检出两处 spec 面事项，**均归属他项、不在本包自改**（见 §4）。
- **ADR 面：零改**。本包无新架构决策。实现前钉死的设计决策点「D2 层栈让位形态」（plan §3 甲/乙二选一）为**包内局部 UI 行为选择**（builder 取甲 = 弹 VOICE 层），非跨系统结构性裁决，由代码 + 冻结齿 + review-packet 承载，不升 ADR。
- **workstream / milestones**：`delivery/workstreams/m4-star-notebook/progress.md` 追加本包 integrator closeout 段落账；`delivery/milestones/milestones.jsonl` **M4 行 state 不动**（真机复验矩阵 E3 未跑，阶段态无跳变——K1/K2/K3 遗留同批裁量）。
- **closeout 制品**：`delivery/changes/dev/261001-m4-k3-record-session-live-repair/`（本件 + `review-packet.md`）integrator 收拢落盘。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（`contractRefs#§1.4/#§四`） | **零触碰**（LANDED 现文只读消费；wire 形状零变） | 只读 |
| spec 面·需求 | `docs/requirements.md`（`docRefs#S-F2.2`） | 零触碰 | 只读 |
| spec 面·架构 | `docs/architecture.md`（`docRefs#§5.2`） | **零语义改动**；检出两处归属他项申报（§4），零自改 | 只读 |
| ADR | `docs/ADR.md` | 零改（无新决策） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | 追加本包 integrator closeout 段落账（append-only） | 记账 |
| milestones | `delivery/milestones/milestones.jsonl` | **不动**（E3 真机复验未跑，M4 行 state 无跳变） | 只读 |
| closeout 制品 | `delivery/changes/dev/261001-m4-k3-record-session-live-repair/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = scoped 相位 `docs-sync` 闸：TaskPackage 有 `docRefs` 且 `docsSyncRequired=true` → 走 `docsSyncReason` 分支。**scoped r1 闸 exit 0**（`verifier-evidence/rounds/r1.json`）+ **final r2 闸 exit 0**（`verifier-evidence/rounds/r2.json`）。
- `docsSyncReason` 声明 = 「arch 面收口：§5.2 场景④ 记录会话演出与层栈让位对账（K3 落地后的实况修正）；requirements 面零改动」。**逐项核对**：
  - 「requirements 面零改动」→ 本件 §1/§2 一致（**已落**）。
  - 「arch §5.2④ 记录会话演出与层栈让位对账」→ 本件 §4 检出：该对账的**语义主体归属 Package B（D3-甲）**、**实现主体已由 K3 落地 + 本包补齐**；本包**不自改 spec**，只做归属申报 + 补正文本（advisory）交 architect/human spec 批。**声明与实做一致**（声明的动作 = 对账并申报归属，非自改）。
- **无声明未做的项、无做而未声明的项**。

## 4. spec 面：零自改 + 检出项归属申报

> 本节为 **advisory 申报**，非本包自改 spec。integrator 无 `docs/requirements.md` / `docs/architecture.md` 写权（`ARQ-260929-docs-sync-spec-handoff`）；下列文本交 architect / human spec 批处置。

### 4.1 检出项一 · §5.2④「引擎 manual 聆听」机制对账 → **归属 Package B（D3-甲）**，不在本包

- `docs/architecture.md:213` §5.2④ 现文「**引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）**」与 `docs/requirements.md:431` 可行性注记「manual 聆听 + 网关累积态已备」为**机制表述**，其漂移源 = **D3-甲（取甲：改可产逐句 stt 的聆听形态 + front 累积态压制逐句回复）**。
- 该机制对账触 **契面 §四「引擎不感知本信号」句**（spine faceMap contract 动），human 261001 触点①已裁「引擎侧再动抑制 LLM」→ 须 **`/arch-change` Track A 定形 → 契约门签字 → Package B 重备 packet**。
- **本包（Package A）不触聆听机制**（纯板端 D1/D2/D4a/D4b），**该对账随 Package B**，本件仅登记不属于本包。

### 4.2 检出项二 · §5.2④「时点①恢复 = RAM 缓存填充」挂账 → **本包实现完成，spec 补正随 K3 pending delta**

- `docs/architecture.md:213` §5.2④ 现文末括注「**原时点①的恢复时机〔设备进 P3 语境推送 / RAM 缓存填充〕归 M4-K3 `kidpal_session_ctx` 设备半落地后重裁**」。
- **本包 D1 修复 = 该挂账中「RAM 缓存填充」消费方的实现落地**（`kidpal_story_list_deferred_flush` 判据 + `kidpal_render_node_voice_end` 前台回落 P3 重渲；见 review-packet）。K3 的 ctx-沿推送 = 挂账中「设备进 P3 语境推送」半。**至此该重裁已闭环**。
- **但该挂账的文本收口归属 K3 pending delta**：`delivery/changes/dev/260924-m4-k3-record-session-live/docs-delta.md` §4.3（H2）已把 §5.2④① 子句改写为「ctx 沿条件推送」并 supersede 本挂账（**该 delta 为待批态、尚未落地**——本 worktree `docs/architecture.md:213` 实读仍为旧文，未含 K3 H2 新文）。K3 H2 新文覆盖 provider/gating 半，**未覆盖 consumer 半**。
- **补正文本（advisory，须与 K3 pending delta H2 同批或在其后落地；integrator 零 spec 写权）**：K3 H2 落地后，§5.2④① 子句宜追加一句 consumer 落点——

  > 板端消费 defer 缓存：记录语境前台（VOICE 层/录音态）期间 `story_list` 帧只装 `g_story_*` 缓存；会话收尾 VOICE 层弹出、前台回落 `LOCAL_PAGE` 且本地页 == P3（列表态）时，渲染节点以最新缓存重渲 P3 前台（`kidpal_story_list_deferred_flush` 判据 + `kidpal_render_node_voice_end` 回落支，260928-story-connect-push-defer 摘除「建连即推」后的板端消费方补齐；261001-m4-k3-record-session-live-repair 2026-10-01）。

  该补正**不改任何机制语义**，仅把旧文已预期、K3 H2 改写时丢失的「RAM 缓存填充」语义恢复为现役实况。

### 4.3 本地写权边界（如实）

- 本包 `git status --porcelain docs/` **零输出**（`docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` / `docs/ADR.md` 本批零写入）；§4.1/§4.2 文本仅成本件记录，未落 spec。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出（`docs/` 整面无输出）；`contract-lint` 闸绿（scoped r1 + final r2 exit 0） |
| 需求面零写入 | `git status --porcelain docs/requirements.md` 零输出 |
| 架构面未写（如实） | `git status --porcelain docs/architecture.md` 零输出；检出项 §4.1（归 Package B）/ §4.2（归 K3 pending delta + advisory 补正文本） |
| ADR 零改 | `git status --porcelain docs/ADR.md` 零输出；无新决策（D2 甲 = 包内局部 UI 选择，不升 ADR） |
| 历史制品不改写 | K3 件（`delivery/changes/dev/260924-m4-k3-record-session-live/`）零触碰，本件只以 §4.2 申报其 H2 的 consumer 补正，不替换 K3 文本 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` 追加本包 closeout 段落账（append-only，不改既有行） |
| milestones | 不动（E3 真机复验未跑，M4 行 state 无跳变） |
