# architect-landing-summary · 261023-arch-review-memory-wiring-verified-behavior

> **packet-binding**：packet-id = `261023-arch-review-memory-wiring-verified-behavior`（被批 review id 同名；承载物 = FROZEN v1.0 `PROPOSAL.md` §2 改法集 T1–T8 + §5 验证集）。
> **landing 执行**：主线程 packet-bound 落地（2026-10-23，未 commit，工作树态即待验态）。
> **本件定稿说明**：主线程委托 landing-verifier 定稿内容；所载声称均已由 landing-verifier 对工作树 diff 逐项实证（见同目录 landing-verdict.md），非落地者自述孤证。

## landing-manifest（落地清单）

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
docs/glossary.md
```

## 落地声称（逐 T）

| 改法 | 文件 | 声称 | landing-verifier 实证结论 |
|---|---|---|---|
| T1 | `docs/architecture.md` §6.7 全节替换 | 新节「### 6.7 记忆写入范围与画像」按 PROPOSAL 围栏块逐字落；`历史 backfill` / `删除语义` / `降级` 三段保持原文 | 字节级精确匹配（2470 字符），三段未入 diff（零增删） |
| T2 | `docs/architecture.md` §1.3 :55 整行替换 | 设备级 peer → 设备级 session + 家庭单 owner 画像（PROPOSAL 围栏块逐字） | 整行字节级精确匹配（211 字符）；旧语面 `设备级 peer` 残留 0 |
| T3 | `docs/architecture.md` §4 NFR-PRIV-1 映射行 | 「记忆写入资格门（白名单入册）」→「记忆写入范围（已验证接线…）」（PROPOSAL 围栏块逐字） | 整行字节级精确匹配（206 字符） |
| T4 | `docs/architecture.md` :192 / :203 两处行内片段 | front 行括注与记忆子系统行按 PROPOSAL 围栏块逐字替换 | 两片段逐字在场；旧片段 `§6.7 资格门：非资格回合不写记忆、不注入画像` 与 `写入受 §6.7 资格门约束` 残留 0 |
| T5 | `docs/architecture.md` :283 行内片段 | `（每轮 persist，受 §6.7 资格门约束）` → `（每轮 persist；写入范围与残余见 §6.7）` | 片段逐字在场；旧片段残留 0 |
| T6 | `docs/architecture.md` §12 表首行插入 | changeId = 本批行，位于 `261022-device-memory-prompt-overhaul` 行之上 | 逐字在场（:489），紧邻 261022 行（:490）之上 |
| T8 | `docs/glossary.md` :32 词条内片段 | `写入资格与边界见 arch §6.7` → `写入范围与残余见 arch §6.7`，词条其余不动 | 片段逐字在场；numstat = 1 增 1 删，只动该词条 |
| T7 | `docs/decisions/ADR.md` 文末追加 ADR-016 | PROPOSAL 围栏块逐字追加；既有条目零改（append-only） | 块逐字在文末（1085 字符）；numstat = 8 增 0 删；ADR-014 原文未动 |

## 验证集复跑结果（landing-verifier 独立复跑，2026-10-23）

| # | 检查 | 期望 | 实测 | 结论 |
|---|---|---|---|---|
| V2 | `spec-anchor-check.mjs` | ok:true / errors:[] | ok:true / storyCount:14 / anchorLines:36 / errors:[] | PASS |
| V3 | `grep -c "资格门" docs/architecture.md` | 0（基线 8） | 0 | PASS |
| V4 | `设备级 peer` → 0；`设备级 session` → ≥2 | 0 / ≥2（基线 1 / 0） | 0 / 2 | PASS |
| V5 | `受 §6.7 资格门约束` → 0 | 0（基线 2） | 0 | PASS |
| V6 | `记忆写入资格与画像` → 1（历史行）；新标题 → 1；全仓残留 → 1 | 1 / 1 / 1（基线 2） | 1（:490 历史行）/ 1 / 1 | PASS |
| V7 | glossary 旧片段 → 0；新片段 → 1 | 0 / 1（基线 1 / 0） | 0 / 1 | PASS |
| V8 | `^## ADR-016` → 1；ADR numstat 删除 = 0 | 1 / 0（基线 0） | 1 / 0 | PASS |
| V9 | changeId 行在场 → ≥1 | ≥1 | 1 | PASS |
| V10 | §6.7 三段保全；glossary numstat 1/1 | 零增删 / 1 增 1 删 | 零增删 / 1 增 1 删 | PASS |
| V11 | 人读逐字核（T1–T8） | 与 PROPOSAL 围栏块一致 | 全部一致（字节级） | PASS |

范围隔离：`git status --porcelain docs/` 仅上述三文件；`docs/contracts/**` / `docs/requirements.md` / `docs/debt/**` 零 diff。
