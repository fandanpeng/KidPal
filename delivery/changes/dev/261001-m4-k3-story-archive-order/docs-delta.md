# Docs Delta · 261001-m4-k3-story-archive-order

> integrator 收拢制品（2026-10-01；② 单包 Full / rigor=full / sourceType=regression / riskTier=high）。
> 本件回答「本包文档面义务是否已全清」。TaskPackage `docsSyncRequired=true`；
> `docRefs = [docs/requirements.md#S-F2.2, docs/architecture.md#§5.2]`；`contractRefs = [interface-contract.md#§1.4]`；`docsOwner = integrator`。
> **权威边界**：integrator 对 `docs/requirements.md` / `docs/architecture.md` **零写权**（human 定稿裁定 `ARQ-260929-docs-sync-spec-handoff`）；spec 文本同步交 architect。故本件不含任何自改 spec 的 hunk，只做零改动申报 + 检出项归属申报。

## 1. 结论

- **本包 = 纯 server（front / story.py）回归修复**：修「归档早于转写」顺序缺陷（缺 261001 真机 r2 实录——listen/state=stop 上行帧先到、转写 stt 1.7s 后才到；旧实现于 stop 帧立即 `acc.stop()`，累积必空 → 返 None 并把会话打回 chat → 转写被丢弃 → 内容彻底丢失）。修法 = pending-stop 挂起 + 转写到达后归档 + 正文剔尾随收尾词 + 10s 超时释放（plan §2）。**已 merged 包（K1 260924-m4-k1）冻结验收件的归档链齿时序由 stt→stop 改写为真机序 stop→stt**（SAO-A1 承接）。
- **契约面：零触碰**（铁律 E2）；只读消费 LANDED 契约（`contract-lint` 闸 scoped r2 + final r3 均 exit 0）；本包不改任何 wire 消息、不触固件与契约/spec 面。
- **需求面（`docs/requirements.md`）：零改**（纯实现时序修复，不改 S-F2.2 语义）。
- **架构面（`docs/architecture.md`）：零改**（`docsSyncReason` 述「§5.2④ 实现口径补正」= 时序实现订正，非 spec 文本漂移；§3 详述）。
- **ADR 面：零改**（无新架构决策 / 无契约变更 / 无系统边界变化）。
- **workstream / milestones**：workstream progress 追加段落账（append-only）；milestones M4 行 state 不动（`in_delivery`，无阶段跳变）。
- **closeout 制品**：`delivery/changes/dev/261001-m4-k3-story-archive-order/`（本件 + `review-packet.md` + advisory 投影）。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（LANDED 现文只读消费） | 只读 |
| spec·需求 | `docs/requirements.md` | 零触碰 | 只读 |
| spec·架构 | `docs/architecture.md` | 零触碰（§5.2④ 为行为级描述，本包实现时序订正不构成其漂移；§3 详述） | 只读 |
| ADR | `docs/ADR.md` | 零改（无新决策） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | 追加本包段落账（append-only） | 记账 |
| milestones | `delivery/milestones/milestones.jsonl` | **不动**（M4 行 state 无跳变） | 只读 |
| closeout 制品 | `delivery/changes/dev/261001-m4-k3-story-archive-order/` | 产出（本件 + review-packet + advisory 投影） | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = `docs-sync` 闸：`scoped r2` + `final r3` 均 **exit 0**（`verifier-evidence/rounds/r2.json` / `r3.json`）。
- TaskPackage `docsSyncReason` 声称「arch 面收口：§5.2④『listen-stop 归档』的实现口径补正（触发仍是 listen-stop，归档动作等转写到达）——architect 判不构成 spec 漂移，是否落文本由 integrator 依实际改动圈定」。**声明与实做一致**：本包实际改动 = 归档**动作发生时刻**（等转写到达）的实现订正，**不改** listen-stop 触发语义、不改 wire、不改 spec。

### 3.1 §5.2④ 文本是否需要补正 → **判不需要（零改）**

- `docs/architecture.md:213` §5.2④ 现文（收尾三式）：「再按 KEY（listen-stop）→ 归档 + 下发」——**行为级描述**（listen-stop 触发 → 归档 → 下发正文），与本包修后实际行为一致（触发仍是 listen-stop、归档 + 下发三件皆在）。本包订正的是**归档动作在实现上等待转写到达**（manual 模式 ASR 由 listen stop 触发、转写 1.7s 后才回），属实现时序细节，**spec 未述及该时序**，故无 §5.2④ 事实陈述被推翻。
- architect 已判「不构成 spec 漂移」；integrator 对 `docs/architecture.md` 无写权（`ARQ-260929-docs-sync-spec-handoff`）且本包 scope-map allow 不含该文件。**判零改、不自改**。
- **advisory（仅报告，不动手，路由 requirement-change / architect）**：若未来希望 §5.2④ 显式写明「归档动作发生在转写 stt 到达之后（manual 模式下 listen-stop 触发 ASR、转写回报存在延迟）」，该文本补正宜随需求/架构面批处理并入（route `/requirement-change` 或 arch 批），**不在本包自改**。登记供后续 spec 批择机消费。

## 4. spec 面：零自改 + 检出/归属申报

### 4.1 §5.2④「引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）」机制表述 → **归属 Package B（D3-甲）**

- 该机制表述的漂移源 = D3-甲（取甲改聆听形态 + front 累积态压制逐句回复），触契约 §四「引擎不感知本信号」句，须 `/arch-change` Track A + 契约门定形后随 Package B（spine `261001-m4-k3-record-session-live-repair` faceMap/q2）。**本包不触聆听机制**，仅登记不属于本包。

### 4.2 收尾后引擎仍把整段当一轮回答并播报（`<< …`）→ **归属 B3 面**

- 261001 真机 r2 实录 `<< 放假还有作业`（收尾后引擎整段回一轮 + TTS），违反「收尾即盖章、AI 闭嘴」口径，属**引擎侧抑制面**，随 B2/B3 的 Track A（plan §6）。本包不管引擎侧，仅登记。

### 4.3 120s 自动收尾路径（`_story_timeout_sweep` 内 `stop()`）未加剔词 → **归属 B2/B3**

- plan §3 明载「不在本包：front_app.py 的 120s sweep 分支（同源缺陷，划 B2/B3）」；builder 已标（120s sweep 的归档 `stop()` 未传 `strip_finish=True`，故该路径归档正文不剔尾随收尾词）。本包只管 listen-stop / 收尾词两条路径的剔词。登记供 B2/B3 返工点。

### 4.4 写权边界（如实）

- `git status --porcelain docs/` **零输出**（requirements / architecture / contracts / ADR 本批零写入）；§3.1 / §4 文本仅成本件记录。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/` 零输出；`contract-lint` 闸绿（r2+r3 exit 0） |
| 需求面零写入 | 同上零输出 |
| 架构面零写入 | 同上零输出；§3.1 判不需要文本补正（architect 判不构成漂移）+ §4 检出三处归属他项 |
| ADR 零改 | 同上零输出；无新决策 |
| 历史制品不改写 | K1 件（`260924-m4-k1`）零触碰；本包只在其冻结验收件内改时序假设（改冻结 oracle，经 closeout + supervisor riskTier=high 承载，plan §5） |
| workstream | progress.md 追加本包段落账（append-only） |
| milestones | 不动（M4 行 state 无跳变） |
