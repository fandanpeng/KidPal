# gov-change · 治理面变更流程（review loop）

> 权威面：本文件是治理面变更的流程 SSOT（261029-gov-review-gov-loop-simplification 改为常规 direct-implementation）。入口命令 = `.claude/commands/gov-change.md`。常规流程 = 轻设计 + 设计审 + 作者实施 + 真实 diff 审；Track A 段只在方向未收敛时启用。

## 立项（含 ARQ 消费）

1. **human 裁意图**（新方向或新取舍）或确认既定 ARQ 波次启动。命令开场先读条目和 spine sources，不让人重述。
2. **既定 ARQ 只做一句启动确认**：说明本波按哪条已裁方向执行，不重复展开旧修法；只有新增方向、新取舍或旧裁定失效才重新问 human。
3. **收敛度判断**：方向清晰直接轻设计；方向未收敛进 Track A 段。触及承重机器不自动升 Track A。

## 常规流程

1. **architect 轻设计**：在 `delivery/review/<changeId>/PROPOSAL.md` 写目标、非目标、关键方案、保护边界、验收标准和轻量 `landing-manifest`。manifest 每行 = `路径 | architect/main-thread | ordinary/protected`，可加短理由；允许目录，不写逐行改法。头部标 `> **gov-flow**: direct-implementation`。
2. **architect-reviewer 审设计**：四查 = 意图保真、纪律合规、撞车复核、牵连面完整。reviewer 自写评审段，主线程不转录；同时判「常规流程 + 条件加强」，可升可降。
3. **主线程标 FROZEN**：冻结意图、边界、关键方案和验收标准，不冻结未来字节。实质方向变化回 reviewer，新 human 取舍回 human。
4. **architect 实施与自测**：在当前 gov worktree 非 main 分支修改普通治理文件，提交真实实现 commit，并在批记录追加实际改动和测试证据。保护面由主线程按同一份设计处理。
5. **architect-reviewer 审真实 diff**：核对意图、scope、消费方和测试判别力。最后 verdict 后出者胜；最后实现评审以 `reviewedCommit` / `reviewedTree` 各一行钉 40 位小写 SHA。任何后续实现变化都使旧钉点失效。
6. **机械检查与合并**：主线程跑聚焦检查和前置升级闸，放行后按 merge-to-main skill 合本地 main，再 upsert `delivery/review/INDEX.md`。普通批无强制 landing-verifier、landing-summary、landing-verdict 或事后 retro。

主线程在设计、设计评审、实现提交、实现评审等节点实时追加 `delivery/review/<changeId>/events.jsonl`，并记录摩擦、建议和实际 usage。事件流 fail-open，不进闸或 INDEX，不设齐全门，不在本流程内建聚合器。

## 轻量 scope 与机械凭据

共享解析谓词在 `.claude/scripts/lib/governance-gate-shared.mjs`；前置升级闸与 INDEX 不各自解析。

- 新流程 manifest 是**预计 scope**：`main...reviewedCommit` 实际非证据 diff 必须是其子集，允许计划内文件最终未改，不做旧恰集或文本保真对账。目录条目以 `/` 结尾并按路径段前缀匹配。
- 出 scope 时补理由并由 reviewer 回审；作者不得自扩自过。证据尾仍由 `isEvidenceTailPath` 识别。
- 闸核 FROZEN、最后 verdict GREEN、最后一组 commit/tree 与 Git 一致、钉点是 source 祖先、评审后无非证据尾改。缺件或后出 BLOCK 均 fail-closed。
- INDEX 新批从批记录现推 landed，不写旧 landed 缓存；旧批仍读 landing-verdict。`--upsert` 在落地 commit 前运行。
- 无 `gov-flow` 标记的旧 packet-bound / author-implementation 批按旧凭据；契约三件凭据、off-main、废止路径与 NOT_APPLICABLE 不变。新流程标记不能豁免契约检查，轻量 scope 也不能作契约放行 manifest。

## 条件加强（不按路径机械触发）

- **第二独立核验（landing-verifier）**：改核心合并保护逻辑或自身/review-loop 权限、高影响难回滚、实现评审争议，或出现评审未覆盖的真实语义风险。核心落点 = 前置升级闸、共享闸库、merge-driver、deny-main-push、enforce-scope、INDEX landed 推导。
- **新鲜 reviewer 实例**：原 reviewer 卷入争议、上下文锚定影响独立性，或 review-loop 第五阀要求。
- **详细设计 / 完整普查**：聚焦检索圈不住消费方，或设计含穷举、零残留承诺。
- **独立 retro**：事故、可复用教训或明显偏离预期；普通批以实时事件流代替形式复盘。
- **主线程代落地**：保护面、写权不足或 human 明确选择。
- **Track A 段**：方向未收敛、根本方案分叉或声明红队；机器 SSOT = `arch-review-protocol.md`。

reviewer 记录每项加强的事实依据。风险消失可降回常规，但不得降掉作者≠最终审者、契约/权限边界和机械合并检查；human 明确指定的加强只能由 human 解除。

## 角色分工

| 角色 | 职责 | 禁止 |
|---|---|---|
| 主线程 | 派发、FROZEN 状态、保护面处理、events、机械检查、合并与 INDEX | 不起草变更内容；不常规转录评审；不改 reviewer 判决 |
| architect | 轻设计；普通治理文件实施、自测和提交；批记录追加实施证据 | 不改 reviewer 段；不自评；不改保护面；不合并 main 或发布 |
| architect-reviewer | 设计审、双向判档、真实 diff 审；自写 PROPOSAL 评审段 | 不落 canonical；与 architect 不得互兼 |
| landing-verifier | 条件加强时独立核验同一真实 diff / commit | 非常规必经；不改被验文件 |
| human | 新方向/新取舍、既定加强和例外裁定 | — |

脚本改动仍按语义拆批：新增子命令、判定重写等 feature 级改动单独成批；小 fix 可随相关批搭车。行数只作提醒，不作路径一票升重。
