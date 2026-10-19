# architect-landing-summary · 261018-arch-review-sd-log-persist

packet-id：FROZEN PROPOSAL §2 改法集（T1–T8）+ §5 验证集 · 评审轮次 **R1 BLOCK(F1–F3) → R2 GREEN** · change-id 261018-arch-review-sd-log-persist（跨面 spine = `261018-sd-log-persist`）

> 主线程 packet-bound landing 记录。作者 = architect（PROPOSAL §2 围栏内逐字文本）；主线程只做逐字搬运，未起草任何架构内容。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 决策 24 白名单增列「设备诊断日志」+「日志」口径消歧（§10 决策 24 句尾 append） | `docs/architecture.md` | 已落：逐字 append 在既有「边界不变」句之后，原文一字未删 |
| T2 · §2.4 原则 5 镜像（句尾 append） | `docs/architecture.md` | 已落：逐字 append |
| T3 · §5.3.1 设备节点「存储」行镜像（行内片段替换） | `docs/architecture.md` | 已落：原文片段 → 替换句逐字一致 |
| T4 · §5.4 数据视图新增「设备诊断日志」数据行 | `docs/architecture.md` | 已落：插在 story 行之后、1bpp 美术行之前 |
| T5 · §6.2 离线边界与介质禁令镜像（节尾 append） | `docs/architecture.md` | 已落：逐字 append |
| T6 · §6.6 可观测性：设备日志的板端留存通道 | `docs/architecture.md` | 已落：插在「三日志源」句与「关联法与已知坑」句之间；三日志源计数语义不变（不新增第四源） |
| T7 · 新增 ADR-012（append 到 ADR.md 尾） | `docs/ADR.md` | 已落：标题 + 固定五小节逐字 append；ADR-011 及既有条目原文未动 |
| T8 · `architecture.md` §12 变更记录 changeId 行 append | `docs/architecture.md` | 已落：新行置于 `261016-p3-story-body-sd-cache` 行之上，依据列不含 FROZEN 版本 token |

## 2. 验证证据（landing 时实跑）

1. **锚完整性** — `node .claude/scripts/spec-anchor-check.mjs` → `"archSectionCount": 47`、`"anchorLines": 31`、`"errors": []`、`"ok": true`、exit 0。**绿**。
2. **PROPOSAL 结构门（preflight）** — `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md` → `"ok": true`、`"findings": []`、`anchorUnion = {docs/ADR.md, docs/architecture.md}` ≡ landing-manifest。**绿**（评审 R1 / R2 各复跑一次，结果一致）。
3. **ADR append-only 守持** — `git diff --numstat -- docs/ADR.md` → `8 0 docs/ADR.md`（删除行数 = 0，纯新增）。**绿**。
4. **整体 diff 统计** — `git diff --stat -- docs/` → `docs/ADR.md | 8 ++++++++`、`docs/architecture.md | 16 +++++++++++-----`，合计 19 insertions / 5 deletions。5 处删除行均为 T1/T2/T3/T5/T6 的「原行被整行重写」计入（原文句保留在重写后的行内），非内容删除；T4 / T8 为纯新增行。
5. **逐字 diff 对账（人读）** — **归 landing-verifier**：需逐文件核对实际落地 == FROZEN §2 各 T 的插入 / 替换段（T1 一段 insert、T2 一段 insert、T3 一段 replace、T4 一行 insert、T5 一段 insert、T6 一段 insert、T7 一段 ADR-012 append、T8 一行 insert）。
6. **边界守持核（人读）** — 归 landing-verifier：carve-out 只给「设备诊断日志（常开 + 有界轮转、目录总量 16MB 封顶、可删除）」；业务日志语义不变；提醒 / 课表仍不落板；断网无提醒不变；诊断日志不提供离线功能；「三日志源」计数不变。
7. **隐私注记在场核** — 实跑 `grep -c "对话原文" docs/architecture.md docs/ADR.md` → `architecture.md:3`、`ADR.md:2`；`grep -c "TECH-261016-sd-story-confidentiality" docs/architecture.md docs/ADR.md` → `architecture.md:1`、`ADR.md:3`。

## 3. 诚实注记

1. **§5 第 6 条的两处 ADR 预期计数与实际不符**：FROZEN §5 写「`对话原文` ADR 预期 1 / `TECH-261016…` ADR 预期 2」，实测为 **2 / 3**。原因是 ADR-012 除决策④外，**标题行**也含「含对话原文」、**关联小节**也含 `TECH-261016-sd-story-confidentiality`——§5 的预期公式按「每条目 1 次」估算，漏计这两处。**落地文本与 §2 §6 围栏逐字一致**（未多字未少字），差异属验证集预期公式失准，不属落地偏差。此处如实留痕，由 landing-verifier 独立判定其是否构成阻断。
2. **本摘要 §2 的第 5、6 两条在写作时尚未执行**——按 SSOT 归 landing-verifier 人读核验；本摘要不预支结论，对应结论以 `landing-verdict.md` 为准。
3. **ADR numstat 8/0** 与 §5 第 3 条预期同向；architecture 的 5 处删除行已逐条说明（见 §2 第 4 条）。
4. **未落项**：无。§6 留根清单各项均为有意的批外留根（隐私 debt append 归主线程收口、实现面归承载包、口径消歧无机械齿等），非漏落。
5. **本批不触契约面**：`docs/contracts/**` 零 diff；`pre-merge-escalation-gate.mjs` 的契约分支不适用（无契约路径改动）。

```landing-manifest
docs/architecture.md
docs/ADR.md
```
