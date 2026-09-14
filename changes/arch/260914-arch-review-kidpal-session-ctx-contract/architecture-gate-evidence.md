# architecture-gate-evidence · 260914-arch-review-kidpal-session-ctx-contract

> 骨架依据 `.claude/references/contract-change-evidence.md`；本台账由主线程落地相位写、landing-verifier 核验。

## 1. 变更一句话

契约 §四 新增端→网关 `kidpal_session_ctx` 消息定义（P3 会话上下文信号载体），spec 两面指针句同步，CDQ 登记。

## 2. 触发与授权

- 触发：2026-09-14 架构侧可行性 review 发现「S-F2.2 信号载体随契约定但未挂契约门清单」；human 指示「有契约吗，没有就现在定义……把 plan 升级为正式 proposal，请 architect review 一遍，没问题再落」。
- 四分类：**CONTRACT_CHANGE_REQUIRED（独立契约变更）**——§四 additive 新增一条消息定义，非随 spec 改动携带（spec 侧仅指针句）。

## 3. human 门（原文与日期）

- 立项 ack：2026-09-14「有契约吗，没有就现在定义」+「把 plan 升级为正式 proposal，然后请 architect review 一遍，没问题再落」。
- OQ2（source_page 闭集宽度）：AskUserQuestion 2026-09-14 裁 **(a) 三值 `story/home/schedule`**（推荐位采纳）。
- 状态转移授权：R3 GREEN 后按 Track B 常驻规则自动落地（human 预授权「没问题再落」）。

## 4. 评审链

- PROPOSAL：`PROPOSAL.md`（FROZEN v1.1，凭据行齐）。
- 评审：`architect-review.md` R1（BLOCKED 9 项）→ R2（BLOCKED 3 项）→ **R3 GREEN**（2026-09-15），逐条修改指令与吸收确认在档。
- OQ 耦合处置：OQ2 裁三值 → T1/T3.1 按「仅 story 触发累积态 + 闭集预留」条件语义写定（R1 MAJOR-1 修复与裁定合并）。

## 5. applied diff inventory

| 文件 | 变更 |
|---|---|
| docs/contracts/interface-contract.md | +1 行：§四 新增 `kidpal_session_ctx` 条目（== PROPOSAL T1 钉死文本） |
| docs/architecture.md | 3 处改行：§5.2 场景④ 载体句（T2.1）；§7 契约门在办「三件→四件」+ ④ 项（T2.2 整行替换）；§9 to-be ⑥ 扩 `kidpal_session_ctx` 发送/拦截（T2.3） |
| docs/requirements.md | 3 处：版本号 v1.52→v1.53；S-F2.2 补充规则载体引用（T3.1）；§9 v1.53 变更行（T3.2） |
| docs/CONTRACT-DECISION-QUEUE.md | +1 行：CDQ-260914-kidpal-session-ctx（T4.1，append-only） |

落地前置已满足：v1.52 批（朗读砍除 + 可行性批改 + 去反引号）先行独立 commit `0a303d9`；本批 landing diff 不含 T3.3（去反引号，已随 v1.52 落地）。

## 6. validation

- `node .claude/scripts/spec-anchor-check.mjs` → `ok:true, errors:[]`（2026-09-15 复跑）。
- `git diff --check` → 干净；人读 diff == frozen 清单（§5）逐项核对通过。
- census：`grep -rln "kidpal_session_ctx" docs/` → 恰 4 文件（contract / architecture / requirements / CONTRACT-DECISION-QUEUE），口径一致（端→网关、front 拦截、引擎不感知、仅 story 触发累积态）。

## 7. 剩余条件

CDQ 状态转 LANDED 待 landing-verifier 绿；实现面（固件发送 + front 拦截）归 dev-loop 后续 TaskPackage（arch §9 to-be ⑥ 已登记接线点）。
