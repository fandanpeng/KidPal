# architect-landing-summary · 261019-gov-review-docs-root-drift-cleanup

packet-id：FROZEN PROPOSAL §2 改法集（T1–T9）+ §5 验证集 · 评审轮次 R1 BLOCK(F1) → R2 GREEN · change-id 261019-gov-review-docs-root-drift-cleanup

> 落地 = 主线程 packet-bound（FROZEN v1.0 PROPOSAL §2 T1–T9 + §5 验证集）。落地日 2026-10-19，gov worktree `C:/AI/KidPal-gov`。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1（§7 头契约门口径改现行） | `docs/architecture.md` | ✅ 括注逐字替换为「契约变更走 /arch-change loop，合并过机械凭据闸 pre-merge-escalation-gate.mjs——261015 批起无人签门」 |
| T2（矛盾双 bullet 并一、状态归账本） | `docs/architecture.md` | ✅ :348-349 两条「契约门在办」删除，落新「契约决策账」bullet（状态唯一权威 = docs/debt/contract/ + 音量调节可见性锚例外行，与 FROZEN 处方逐字一致） |
| T3（收编注元注记尾巴删） | `docs/architecture.md` | ✅ 「，『在办』状态更新」删除，收编注本体保留 |
| T4（两处 ADR 死链改指） | `docs/architecture.md` | ✅ :5 与 :397 两处 `](ADR.md)` → `](decisions/ADR.md)`（replace_all 恰 2 处，与普查基数一致） |
| T5（决策 12 注记尾巴裸删回原文） | `docs/architecture.md` | ✅ ⚠️ 每日一字注 + 删除线层整条删除，条目回 D3 原文形态 |
| T6（决策 20 按 14/15 先例整形） | `docs/architecture.md` | ✅ 恢复原文「BOOT 同时照常翻页」+ 单行 ⚠️ 历史口径指针（现状见 requirements.md §2.3），三层勘误清除 |
| T7（文首两处死指针改指） | `docs/DEV-GOVERNANCE.md` | ✅ 结构参考改指 repo-structure.md（本仓结构 SSOT）；「术语表暂缺，建立时补」改「术语表 = [glossary.md](glossary.md)（已建）」 |
| T8（团队表两行并一 + 缺口条目删） | `docs/DEV-GOVERNANCE.md` | ✅ :54-55 两行并为「系统级 UI 设计执行 = uiux-author（档 2 流程 4b 生成模式）」一行；§8 uiux-designer 缺口条目整行删除 |
| T9（human 职责行去「契约门」） | `docs/DEV-GOVERNANCE.md` | ✅ 改「立项 ack、例外升级拍板、高影响拍板、治理批准」 |

未触面（按 PROPOSAL 声明）：契约面 `docs/contracts/**` 零写；G2/G3/实现面零写；§10 其余决策注记未动（留根 3）；`docs/debt/` 零写（音量调节补登记归主线程后续，留根 4）。

## 2. 验证证据（§5 pre-commit 1–7 执行记录；第 8 条 post-merge 待执行）

| 项 | 结果 |
|---|---|
| 1 退役语清零（九探针） | ✅ 全零命中：`契约门在办` / `human 签字` / `](ADR.md)` / `注记已消解\|每日一字` / `措辞对齐 requirements.md`（architecture.md）；`repo-structure.net-template\|术语表暂缺\|契约门、高影响拍板` / `uiux-designer`（DEV-GOVERNANCE.md） |
| 2 新文在场（八断言） | ✅ 逐条 grep -F 计数 = 1/1/2/1/1/1/1/1（③ `decisions/ADR.md` 计 2 = :5/:397 两处，符合断言） |
| 3 链接目标存在 | ✅ `ALL-EXIST`（docs/decisions/ADR.md + docs/glossary.md + .claude/rules/repo-structure.md） |
| 4 锚机检回零 | ✅ `spec-anchor-check.mjs` → `"ok":true`、`errors:[]`、`archSectionCount:47`（与起草期基线一致，小节编号零变化） |
| 5 uiux-designer 残留 ≡ 白名单 | ✅ 命中恰两件：`docs/debt/gov/_archive/2026/ARQ-260923-planner-roster-reference-drift.md`（2 行，已结条目 append-only 原文）+ `docs/debt/gov/_archive/2026/INDEX.md`（1 行，归档薄行生成件）；白名单外零命中 |
| 6 diff 范围 ≡ manifest | ✅ `git diff --stat` 触及 = `docs/architecture.md` + `docs/DEV-GOVERNANCE.md`（≡ manifest）+ `delivery/review/INDEX.md` 与 `.changes-index-state.json`（本批 FROZEN upsert 簿记产物，非 canonical）+ `.codebase-memory/` 两件（机器再生面常驻脏，纪律上不 stage）；逐文件人读 diff 逐字核对 = §2 各 T before→after（landing-verifier 核验本体） |
| 7 结构门干跑 | ✅ `landing-fidelity-check.mjs --preflight` → `ok:true`、`findings:[]`、anchorUnion ≡ {`docs/DEV-GOVERNANCE.md`, `docs/architecture.md`} |
| 8 post-merge 簿记 | 待合并后执行（main 上复跑锚机检 + `--upsert`） |

## 3. 诚实注记

1. T8 缺口条目删除的首次 Edit 因主线程凭记忆拼接相邻行未命中（实际相邻关系与记忆不符），重读原文后二次命中——失败调用零写入，无残片入仓。
2. `.codebase-memory/artifact.json` 与 `graph.db.zst` 为工具自刷新机器再生面（常驻脏 = 常态），本批不 stage、不还原，纪律沿 AGENTS.md 快照流向条款。
3. 留根清单 7 项（PROPOSAL §6）去向不变：DEV-GOVERNANCE.md :47 --no-ff 已核非漂移不改；收编注本体保留；§10 其余决策注记留 human 后续裁定；音量调节补登记归主线程；contract 域 7 条 deferred 条目处置不属本批；日落复盘项（引用普查漏检方法对照）交 loop-retro；architecture.md :4 裸「（契约门）」括注后续批顺手对齐。

```landing-manifest
docs/architecture.md
docs/DEV-GOVERNANCE.md
```
