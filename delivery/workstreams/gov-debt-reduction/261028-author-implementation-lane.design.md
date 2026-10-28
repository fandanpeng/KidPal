# 设计讨论记录 · 治理批作者直达实现通道（author-implementation lane）

> 日期：2026-10-28。来源：P8 全环 retro 后 human 与主线程的工艺复盘。本文是已获 human 批准的设计讨论记录，不是 canonical，不自评通过、不授权直接实施；下游入口 = `/gov-change`。

## 9. 追加裁定（2026-10-28，主线程落地可行性核中发现）

主线程核 FROZEN 前落地形态发现：manifest 26 件中仅 5 件带逐字旧/新块，其余 21 件（含 enforce-scope、两闸脚本、scope-map 等承重代码）只有行为级规格；本批自declared packet-bound 与之矛盾——严格旧通道要么主线程按 prose 当作者（正废除的模式）、要么 architect 补齐 21 份逐字块（P8 式成本复发）。human 拍 **路线 B：自用一次新通道**——本批 21 件实现由 architect 直接在 gov worktree 完成（5 组逐字块照规格实现，scope-map 自授权 diff 由 reviewer 专项审），不补逐字块；reviewer 审真实 diff、verifier 钉 tree + changedFiles==manifest；机器闸（tree 重算）本批落地后才存在，本批凭据靠 git 钉点人工核 + verifier 独立会话，如实标注「闸后置」为唯一一次。逐字旧/新块的定位据此澄清：它是「设计者≠落地者」时的转写凭证，不是验证的必需品；作者自落地 + 独立审稿 + 同树验证即达成「落进仓的 == 评审过的」。

## 1. 问题与证据

P8 为 42 处编辑、15 件 manifest 制作了 1434 行 PROPOSAL，其中大量篇幅是 edits.json 已携带的旧/新代码逐字副本。为证明「architect 的设计 == 主线程代写 == 最终落地」，全批产生 4 个一次性仓内脚本（make-edits / build-proposal / mutation-check / evaluate-five-cases）与 18 个仓外装配或 overlay 副本，同一装配至少完整跑 6 遍。装配与 sha256 本身便宜，主要成本是重复书写、重复阅读、重复对账，以及测试写成一次性脚本后删除。

根因不是 architect 不能起草（PROPOSAL 本就由它起草），而是：author 不能提交 canonical 实现 + 主线程必须代写 + PROPOSAL 必须逐字承诺未来代码。独立 reviewer 与 verifier 已经存在，代写并不是 author≠judge 的必要条件。

## 2. human 已拍板的设计选择

1. 采用「作者实现、独立审稿、独立验证」通道，**同时开放 `.claude/scripts/**` 与 `.claude/hooks/**`**，不只 scripts。
2. 隔离边界按分支计算：**本治理批可直接使用 gov worktree，不新建 worktree；禁止写 main**。开发 Pod 照旧从 main 拉各自 worktree。
3. 不再要求 PROPOSAL 携带逐字旧/新代码，不再由主线程按文本代写；以 Git commit/tree 绑定被审、被验与被合并的实现。
4. 测试可以由 architect 编写，但作者测试不是唯一判据；reviewer 与 verifier 的独立性保留。
5. P9 作为新通道的首个 scripts 试点；hooks 虽同步开放，但 P9 不实际触 hooks，首个 hooks 实证另行全强度评审与记录。

## 3. 目标流程

1. **设计冻结**：轻 PROPOSAL 只写目标、文件 manifest、行为约束、正反测试要求、明确不做的事项。human 立项 ack 与 Track A 方向收敛判据不变。
2. **作者实现**：architect 在 gov worktree 的批准分支内直接修改 manifest 中的 scripts/hooks，运行测试、修正实现并提交 commit。可以边验证边改，但不得静默扩 manifest。
3. **真实 diff 评审**：architect-reviewer 审实际实现、测试、文件范围与行为约束。发现实现问题原则上由 architect 修复，主线程保持纯编排。GREEN 后钉 `reviewedCommit/tree`。
4. **独立验证**：landing-verifier 对同一 tree 复跑测试，并独立执行消费层/黑盒探测；核对 `changedFiles == manifest`、`reviewedTree == verifiedTree`。PASS 后形成 verdict。
5. **过闸合并**：升级闸与本地 main 合并纪律不变。rebase、回同步或任何后续改动使 tree 变化时必须复验，不能用旧 SHA 放行。

## 4. 权限与责任边界

architect 可以：在冻结 manifest 内修改 scripts/hooks；编写常驻测试；在 gov worktree 提交实现；按 reviewer findings 修代码。

architect 不可以：自批、自己签 verdict、合并 main；修改 scope-map/settings/agent 定义或自身权限；把未审实现当作已通过；超出 manifest 静默扩面。

主线程：派发、状态翻转、记账、过闸与合并编排；不再逐字代写，也不替 reviewer 改判。

reviewer：作者≠审者不变；除审 diff 外，必须从设计而非实现独立提出失败判据，至少覆盖消费/拦截层风险。

verifier：独立核验不变；不得只采信作者测试报告，须在同一 tree 上独立执行验证。

## 5. 测试与临时件处置

- 有长期保护价值的测试进入脚本常驻 `--self-check` 或正式测试文件，不再批后删除。
- 一次性环境夹具可以存在，但不得成为唯一验收判据；测试判据必须可复核。
- mutation/反转 case 必须落在消费/拦截层；计算层与消费层分离时两层均有判别（与 ARQ-281028-mutation-fixture-consumer-layer 咬合）。
- `make-edits` / `build-proposal` 类为逐字 PROPOSAL 服务的脚手架在新通道中取消；不再生成装配副本证明主线程抄录一致。

## 6. 机械面必须同步改造

不能只从 scope-map 删除 deny。 enabling 批须一并设计并评审：

- `.claude/scope-map.json` 与 `.claude/agents/architect.md`：新写权、禁止事项与 gov/main 分支边界；
- `AGENTS.md`、`gov-change.md`、`arch-change.md`、architect-operating-model：落地通道与角色职责；
- PROPOSAL / landing-summary 模板：设计 + manifest + commit/tree 凭据取代逐字代码块；
- `landing-fidelity-check.mjs` 与 `pre-merge-escalation-gate.mjs`：识别新通道的 reviewed/verified tree、verdict 与 changedFiles==manifest 凭据；
- reviewer / verifier 简报：真实 diff 评审、独立失败判据与同 tree 验证义务。

**enabling 批本身仍走现行主线程 packet-bound landing，不得用尚未生效的新权限自举。新通道落地后 P9 才启用。**

## 7. 保留不动的防线

- human 立项 ack、Track A 方向收敛判据、作者≠审者、landing-verifier 独立核验、合并机械闸、远程发布纪律全部保留。
- contracts、spec、firmware/server、scope-map/settings/agent 定义不进入日常直达实现范围；它们若属某批目标，仍按各自既有防线处理。
- 新通道不是「降低检查」，而是把「证明主线程没抄错」替换为「证明审、验、合并的是同一 tree」。

## 8. 成功标准与试点记录

P9 试点应实测并回填：PROPOSAL 行数、临时脚本数、仓外装配副本数、重复装配次数、评审轮次、主线程代写工作量、验证发现的问题。若新通道仍需要逐字代码块或主线程代写才能过机械闸，视为设计未闭合，不宣称完成。

hooks 面的首次实际使用单独记录：是否影响当前会话、是否出现自动执行副作用、reviewer/verifier 是否能独立复现。P9 scripts 成功不等于 hooks 已实证。
