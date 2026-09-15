# decision-packet · 260908-gov-review-escalation-conflict-ladder

> Track B 评审循环的 confirmed 改法 delta 对照源（判官 = architect R1/R2；human 拍板在册）。landing 以 FROZEN PROPOSAL v0.2 §2 为准，本件只列「评审确认的 delta」供定稿核验对照。

## 裁定项终态

| # | 项 | 裁断 | 来源 |
|---|---|---|---|
| 1 | review 轨 | Track B（human 质询「有那么复杂吗」→ 主线程复判降档；architect R1 D 节独立复核维持，不行使打回权） | human 2026-09-08 + R1-D |
| 2 | B1 cross-system 宿主修正 | 真宿主 = `.claude/hooks/supervisor-verdict-gate.mjs` isCrossSystem :224-231；退役实现 = riskTier 派生轴摘除、verdict APPROVE/BLOCK 语义不动；hook 入 manifest | human 确认修法 2026-09-08 |
| 3 | OQ1 齿复跑范围 | ② 级自动并集后复跑双方冻结件 + 全档 | human 2026-09-08 |
| 4 | OQ2 ③ 级 architect 权形态 | 建议即地板（出消解建议 + 可机判依据，不落地；author≠judge） | human 2026-09-08 |
| 5 | OQ3 探测频率 | 仅合并门时点 | human 2026-09-08 |
| 6 | OQ4 真跨系统保护 | 接受相交探测承担（③/④ 级兜住） | human 2026-09-08 |
| 7 | S1 `.vN` add/add carve-out | 已登记 §10c 形态 → ② 级（复用 260908 特判）；未登记 → 判不了 → ③（fail-closed） | R1-S1 |
| 8 | S2 探测结果传递 | gate `--probe-out <path>` JSON 制品，显式文件非 stdout | R1-S2 |
| 9 | S3 ③ 派发 fallback | 派发失败/超时/无消费方 → 直接 ④ escalate human | R1-S3 |
| 10 | S5 DEV-GOVERNANCE 落点 | §6 例外升级拍板段内一句（非独立节） | R1-S5 |

## confirmed 改法目标（= manifest 六路径）

`.claude/scripts/pre-merge-escalation-gate.mjs` · `.claude/scripts/merge-driver.mjs` · `.claude/hooks/supervisor-verdict-gate.mjs` · `.claude/references/pod-coordination.md` · `docs/DEV-GOVERNANCE.md` · `docs/ARCH-REVIEW-QUEUE.md`

## verdict 链

R1 = NEEDS-WORK（9 findings：1B+5S+3C）→ human 三裁 → v0.2 机械回灌 → R2 = **GREEN**（0 残余；闭合核对表 `architect-review-r1.md` R2 节）。判官角色注记：v0.2 修订由判官按 human 三裁 + 主线程指令机械回灌，作者≠判官的最终保障在 landing-verifier 独立核验（R2 角色注记在案）。
