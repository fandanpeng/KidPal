# Docs Delta · 260921-m3-k3-voice-schedule-entry-live

> integrator 收拢制品（2026-09-21）。本件回答「本包文档面义务是否已全清」；TaskPackage `docsSyncRequired=false`，逐面核对与事实锚如下。

## 结论

- **TaskPackage 声明 `docsSyncRequired=false`（docsSyncReason：prompt 版本化演进面（v6 新文件 + carrier 声明）随代码与部署制品承载；spec/契约零改动（口径已冻结：S-F1.3 v1.19、契约形状归 K1 契约门）。server/deploy/README.md 的 v3 文档债不属本包（既有 ARQ 在册））：本包责任范围内成立，义务已全清。** 本包全生命周期对 `docs/**` 零写入，docs-sync 闸全轮绿（r1 scoped + r2 final，`ok:true`，`docsSyncRequired:false` 回显一致）。
- **零 docs 面改动的理由**：
  - 本包核心改动面 = **prompt v6 版本化演进**（新文件 `openclaw-system-prompt.v6.md` 落盘 + carrier 声明切换 + overlay 渲染接线 + provider.config 声明面同步）——演进史由版本文件序列自身承载（v1..v6 可 diff、历史零回改，sha256 机制齿在册），无需 docs 正文复述。
  - 「帮我显示课表」意图链的契约形状（§1.3② 薄载荷 + front 截获展开）在 K1 契约门 2026-09-21 定稿（HUMAN_APPROVED，随 `24bb1b8` 落地），本包只消费不定义。
  - spec 面 `docs/requirements.md` 的 S-F1.3 / S-F1.5 / S-F1.2 / S-F1.1 / NFR-READ-1 需求行在 v1.19 口径已冻结在前，本包是实现向 spec 收敛，需求面只读不修改。
- 实现细节由代码注释/docstring（`schedule.py` 截获展开决策面 docstring、`front_app.py` 接线注记、v6 prompt 课表段正文）与 closeout 制品（本目录 review-packet.md + device-round-record.md）承载，不另立 docs 文档。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（铁律 M3K3-E2；消费 K1 契约门冻结形状 §1.3②） | 只读消费 |
| spec 面·需求 | `docs/requirements.md`（docRefs `#F1`；sourceRefs S-F1.3/S-F1.5/S-F1.2/S-F1.1/NFR-READ-1） | 零触碰（v1.19 口径冻结在前，实现即向其收敛） | 只读消费 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | 只读消费 |
| prompt 版本化面 | `server/deploy/openclaw-system-prompt.v6.md`（新文件）+ `OPENCLAW_SYSTEM_PROMPT_CARRIER` + `apply-xiaozhi-overlay.sh` + `provider.config.yaml` | v6 落盘 + 三面接线同步（渲染面 == 声明面 == 载体面 == 被测面，ADR-001；v5 及以下零回改，sha256 齿在册） | 随代码/部署制品承载 |
| workstream 过程账 | `delivery/workstreams/m3-schedule/progress.md` | K3 收口段 append（append-only 纪律） | 过程账 |
| closeout 制品 | `delivery/changes/dev/260921-m3-k3-voice-schedule-entry-live/`（本件 + review-packet.md + device-round-record.md + verdict.json + verification.json） | integrator 收拢制品落盘 | closeout |

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git diff -- docs/ .claude/ firmware/` 零命中（2026-09-21 integrator 实跑，`docs/contracts/**` 亦零命中）；contract-lint 闸绿（r1 scoped + r2 final，exit 0） |
| spec 面零写入 | 同上 `git diff` 零命中（docs/ 全目录）；docs-sync 闸绿（r2 final stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径） |
| agent 定义 / gate | `git diff -- .claude/` 零命中（全生命周期） |
| prompt 历史版本零回改 | `test_a1_v5_prompt_file_immutable`（v5 sha256 机制齿）+ `test_a1_v5_base_verbatim_diff_confinement`（diff 界面齿）双齿在册，r2 final 全绿 |
| K1 契约批归属 | 本包基线已含 `24bb1b8`（K1 契约落地批在分支基线）——本包 diff 内零契约 hunks，归属清晰不混提 |
| 实现细节承载 | 代码注释/docstring 随实现 commit 在案（见上清单）；无需 docs 正文承载的内容 = 意图措辞口径（v6 prompt 正文即工件面） |
