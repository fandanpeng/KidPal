# 契约面纪律（碰 `docs/contracts/**` 时自动加载）

> 本仓契约面 = `docs/contracts/**`（单一事实源：笔顺 JSON 格式 D3 / display 统一响应块 E1 / 课表 MD 结构 B1 / shim API）。**契约变更 = 架构变更的一种**（261015-gov-review-face-loop-unify）：走 `/arch-change` loop，无人类签字门。

## 结构红线

- `docs/contracts/**` 写防线保留：Bash 重定向写恒拦（universal.bashDenyWrite）、实现面各 agent 与 dev-loop 会话契约禁写不变（scope-map per-agent deny / dev-loop.settings.json）；architect（起草）与 review-moderator（Track A 段编排）的 scope-map 契约写权维持现状——落地恒归主线程 packet-bound。
- 契约变更走 `/arch-change` loop（流程 SSOT = [references/arch-change.md](../references/arch-change.md)「触契约分支」节）：契约包三节（proposedContractDiff / impact / validationPlan）并入该批 PROPOSAL，随批原子落地；纯契约变更（无架构配套）同样以本 loop 为入口。
- 合并由 `pre-merge-escalation-gate.mjs` **机械凭据闸**放行：契约 diff 须同批有 FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖该契约路径，三件联立、缺一即 BLOCK。
- 影子目录禁令：根级 `contracts/**` 是影子（scope-map universal.denyCreate 拦截新建）；真契约面唯一在 `docs/contracts/**`。

## 四分类 canonical 枚举（唯一权威；其余文件引用本表，不得各自定义）

| 值 | 含义 | 处置 |
|---|---|---|
| `NO_CONTRACT_CHANGE` | 现有契约足够（实现走偏 / 未读对 / 设备端本地渲染细节） | architect-reviewer 返 `doNotEscalateToHuman`，原地指导 |
| `IMPLEMENTATION_ADAPTER` | 契约合理，实现侧需 adapter / mapper / 本地格式转换 | 同上 |
| `ACCOMPANIES_SPEC_CHANGE` | 随 spec/架构变更携带，同批原子落地 | 契约包三节并入该批 PROPOSAL |
| `SPEC_CONTRACT_DRIFT` | 已批准 spec 与契约冲突，须裁改哪边 | 转 `/arch-change` |
| `CONTRACT_CHANGE_REQUIRED` | 现有契约无法表达已批准设计（独立契约变更） | 转 `/arch-change` |

## 本仓实况注记

- 契约已归位为 `docs/contracts/interface-contract.md`（2026-08-27 主线程按 human 指示归位）。
- pass-oracle 第 4 关（spec↔contract 机械门）本仓暂无脚本兜底，降级为人工核对并如实记录——见 debt 账本条目 `docs/debt/gov/ARQ-260827-contract-mech-gate-gap.md`。
- 历史人签门制品（`CONTRACT-CHANGE-PACKET.md` / `architecture-gate-evidence.md`）不再作为新批要求（261015 批撤销人类契约签字门）；证据义务由 loop 标准制品承载（见 [references/contract-change-evidence.md](../references/contract-change-evidence.md) 收窄版）；CDQ 保留为跨时间账本——家 = `docs/debt/contract/` 域（一条目一文件；原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列 261019 批并入并删除），列义「loop 凭据」（条目机读字段 `fourClass` / `loopEvidence`）。
