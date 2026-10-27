# architect-landing-summary · 261027-gov-review-draft-cost-relief

packet-id：FROZEN PROPOSAL（v0.2）§2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 BLOCK(F1,F2) + F3 NIT → DRAFT v0.2 → R2 GREEN（N1 归主线程落地执行） · change-id 261027-gov-review-draft-cost-relief

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 共享锚文装配器 | .claude/scripts/assemble-edits.mjs（新增，201 行） | `{edits, copy}` 清单制、oldStr 恰 1 次 fail-loud、失败零写盘、输出根强制仓外（realpathDeepest 解析链接后判仓界）、自检 16 断言；**落地执行本身就是它的首秀**——四件 canonical 全部由它从 FROZEN 编辑清单装配产出 |
| T2 · 复跑分档句 | docs/templates/PROPOSAL.template.md §5 | 改稿轮/确认轮按影响面窄化复跑，全量义务钉 FROZEN 前 + 落地后核验前两时点；三规则原文一字未动（V3 哨兵过） |
| T3 · 脚本批拆分纪律（两处各一句） | docs/templates/PROPOSAL.template.md（自检句⑤ + 表头四→五句）· .claude/references/gov-change.md（起草节 bullet） | 净增 = numstat 增减合计、新增/未跟踪件先 `git add -N` 或按行数直计（R1-F1 口径）；>~150 行或 feature 级单独成批、fix 级 <~50 行可搭车、初值随复发校准 |
| T4 · ARQ-261027 结项 | docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md | frontmatter 三字段翻转（status: closed / closed: 2026-10-27 / closedBy: 本批）+ 文末结项 append；已由 debt-render 归位 `docs/debt/gov/_archive/2026/` |

落地通道：主线程 packet-bound——**用本批新落的装配器自身**（`assemble-edits.mjs` + FROZEN 编辑清单）装配至仓外 `C:/tmp/dcr-landing`，四件 sha256 与 B3 基线逐值一致后拷回；装配后 `_scratch/` 删除（V5 过）。

## 2. 验证证据

- **自检**：`assemble-edits.mjs --self-check` 16/16、exit 0（canonical 件直跑）。绿。
- **preflight**：`ok:true`、anchorCount 4、anchorUnion = manifest 四行、findings/advisories 双空。绿。
- **cite-check**（FROZEN 后自动 landing 相位）：`ok:true`、targetCount 4、**checked 8 / missing []** / exit 0。绿。
- **V4**：禁触面清单空输出；改动面 = manifest 四件 + 生成件（gov INDEX + _archive INDEX + review INDEX/state，ARQ-261027 呈删除态于原路径、归位路径 untracked 新增——与 P6 同口径）。**新脚本为未跟踪件不进 `git diff`（恰为本批 T3 修的 F1 口径），staging 显式 add**。绿。
- **V5**：`_scratch/` 删除后 review 目录仅余 PROPOSAL.md 与本 summary。绿。
- **V6/V7**：V6 字面断言含于装配器 sha256 对账（四件 before/after 全值吻合 B3）；debt-render 归位 + debt-doctor **RED 零命中**。绿。
- **V9**：changes-index upsert FROZEN 翻转在落地 commit 之前完成。绿。
- **复跑分档首次实践**：改稿轮按 T2 新规窄化（T1 全量 + T3 字面 + F3 口径），全量义务在 FROZEN 标注前由 architect 全套跑毕（数字见 §5 B 表）。

## 3. 诚实注记

- **N1（R2 遗留）处置**：~~V6② 链接面用例的 junction 由 architect 测试后即时清理（`C:/tmp/dcr-jrepo` 已不存在，主线程落地期复核确认）~~ **更正（landing-verifier ADVISORY-1 实测抓出本句失实）**：junction 实际残留至核验期，主线程原句系采信 architect 自报未亲测——已 `rmdir` 清理（主线程落地 commit 前实测 `C:/tmp/` 无 dcr-jrepo），本更正 append 留痕。教训：主线程自己写进 summary 的核验断言同样受「双信号源」纪律约束，不得采信转述。
- **changedCanonicalNotInProposal（V4 并发口径）**：`delivery/workstreams/gov-debt-reduction/task_plan.md`（P6.5 立项与进度记账）、`delivery/review/INDEX.md` + `.changes-index-state.json`（V9 生成件随本批 commit）、`docs/debt/gov/INDEX.md` + `_archive/2026/INDEX.md`（T4 结项渲染生成件）。均非实现面。
- **T1 是新写路径**（riskTier medium 证成）：其仓内写防线 = 输出根强制仓外（realpathDeepest 深度解析链接）+ 自检 16 断言；残余面（蓄意构造 FS 链接）已登记 §6 留根 2，主目标「防意外 canonical 写」成立。
- **V8 未实测**（依赖 landing-verdict，post-commit 归主线程）。
- 发布注记：PR #44（P6 批）尚未合并，本批合并本地 main 后**暂不发布**——待 #44 合并、`--sync` 追平后再走 release（避免重叠 release 分支）。

```landing-manifest
.claude/scripts/assemble-edits.mjs
.claude/references/gov-change.md
docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md
docs/templates/PROPOSAL.template.md
```
