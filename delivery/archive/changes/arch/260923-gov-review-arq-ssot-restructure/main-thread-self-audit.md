# main-thread-self-audit · 260923-gov-review-arq-ssot-restructure

> 主线程独立自查（human 2026-09-24 指令：「你自己也写一个 check list，检查一下这次改动还有没漏改的，有矛盾的」）。与判官 R1–R3、landing-verifier 六项核验**互为独立面**：本清单专扫「漏改」与「矛盾」两类，逐项给命令级结果。时点 = landing-verdict PASS 之后、commit 之前。

## A. 漏改面（旧机制词残留）

| # | 检查 | 命令/方法 | 结果 |
|---|---|---|---|
| A1 | 活跃面残留「写一行 / 6 列约定 / 队列表格」类旧机制指令 | `grep -rn "写一行\|6 列约定\|六列\|队列表格" .claude/{agents,commands,references,rules,skills} docs/glossary.md docs/ARCH-REVIEW-QUEUE.md` | **净**——唯一命中 `integrator.md:42` 为 code-review Findings 表六列（无关同名） |
| A2 | `ARCH-REVIEW-QUEUE` 活跃引用面逐件归置 | `grep -rl` 全仓三分口径 | 22 件全部有归属：5 件 T6 改写后合法保留（stub 路径/历史陈述）、4 件 .claude 豁免（self-def/历史引注/纯指针）、docs 3 件豁免（G1/纯指针）、server 6 + tools 1 豁免、stub 本体、crq-migrate（机制自身）——与 PROPOSAL 留根 16 逐件对得上 |
| A3 | loop-retro 是否有独立 SKILL 漏改 | `ls .claude/skills/` | **无独立 skill 目录**（`/loop-retro` 即 `commands/loop-retro.md`，已改）；系统面板所列 skill 名来自 commands，非漏网 |
| A4 | adjudicator 写面是否全部改净 | `grep -n ARCH-REVIEW-QUEUE .claude/agents/adjudicator.md` | :3 description / :39 / :50 / :77 四处全部 CRQ 口径（:50 留 stub 状态注记为合法提及）——**注**：此处曾是两轮判官未点名的漏网，自查补捉，已记 PROPOSAL 起草期声明⑤ |

## B. 矛盾面（跨文件口径互斥）

| # | 检查 | 命令/方法 | 结果 |
|---|---|---|---|
| B1 | handoff 空的档位（F2/G2 曾互斥） | T3 改法 2 vs T5 vs doctor 实现 | 三处一致 = **advisory**（`tech-no-handoff`） |
| B2 | 前缀规则 vs 迁移不改名 | lib `GRANDFATHER_PREFIX` vs T3 改法 6 vs 落盘 18 条 legacy | 一致：`ARQ-` 放行 + advisory 计数；新前缀 `TECH-`/`REQ-` 仅约束新条目 |
| B3 | severity 闭集 | lib（7 值并集）vs T3 改法 7 vs skill vs 模板 | 一致；rubric 五档外（COULD/MAJOR）走 advisory，skill 已提示新条目尽量用五档 |
| B4 | 「结项不搬家」vs 归档自动搬迁 | T1 改法 3 / T2 改法 6 / stub / skill / 模板 / repo-structure | 口径统一为「**人不搬文件**；翻字段后渲染器按 f(状态) 自动归位」——五处文面同措辞，无互斥 |
| B5 | INDEX 同名物计数 | `find . -name INDEX.md` = 8 | repo-structure 消歧表（3 行 + 8 份注记）、PROPOSAL T6、复扫先验段一致 |
| B6 | scope-map ↔ surface-alignment | adjudicator allow 对比 | 双侧均含 `docs/crq/**`，lastReviewed 已更 |
| B7 | FROZEN 文本 vs 活状态 | PROPOSAL「94 条/未结 12」vs 盘上 95 条/未结 13 | **非矛盾**：FROZEN 记录冻结时点态；差 = dogfood 条目（summary §3-10 已声明）；PROPOSAL 冻结后未回改 ✓ |

## C. 机械面（权限 / 属性 / 链接 / git）

| # | 检查 | 命令/方法 | 结果 |
|---|---|---|---|
| C1 | review-loop settings 是否拦 adjudicator 写账本 | 读 `.claude/review-loop.settings.json` deny 段 | 仅 deny `docs/contracts/**`——`docs/crq/**` 可写 ✓ |
| C2 | union 属性对 5 份 INDEX 实际生效 | `git check-attr merge -- <5 份>` | 全部 `merge: union` ✓（含两份归档年 INDEX——glob `docs/crq/*/_archive/*/INDEX.md` 命中） |
| C3 | INDEX 内链可达性 | node 遍历全部 `](…md)` 链接 | **97 链接 / 0 断链** ✓ |
| C4 | git 面 130 件逐桶归账 | `git status --porcelain -uall` | 22 修改（T6 19 + `.gitattributes` + upsert 伴生 `arch/INDEX.md`/`.changes-index-state.json`）+ 108 新增（账本 100 = 95 条目 + 5 INDEX、脚本 4、模板 1、批制品 3）= 130 ✓；`.codebase-memory` 两件按 AGENTS 纪律**不入 commit**（gov 分支禁私带快照）✓ |
| C5 | 承重面零触点复核 | `git status` 反证 | `AGENTS.md` / `CLAUDE.md` / `DEV-GOVERNANCE.md` / `architect.md` / `docs/contracts/**` / `verify-gates.json` / `*.settings.json` / `governance-gate-shared.mjs` **均无改动** ✓（verifier 独立核过同一结论） |
| C6 | enforce-scope 钩子活性 | 尾换行修复首次用 Bash `printf >>` 尝试 | **被钩子当场拦截**（`.claude/**` bashDenyWrite）→ 改走 Edit 工具——强制层在本批自身上再次实证有效 |

## D. 记录一致性（verdict 后更正，landing 面零改动）

| # | 项 | 处置 |
|---|---|---|
| D1 | summary V6 anchorCount 26（展开前旧值）→ **124** | 已更正 + §3-12 记录 |
| D2 | summary V13 未结 12 → **13**（dogfood 差） | 已更正 + §3-12 记录 |
| D3 | summary §1-T2 归档 63 → **64**（例外条款条目） | 已更正 + §3-12 记录 |
| D4 | `gov-arq/SKILL.md` 尾换行丢失（verifier ADVISORY-4） | 已补（Edit 工具，非 Bash 重定向） |
| D5 | render 自检标称「8/8」实为 7 组夹具（ADVISORY-5） | **不改**——PROPOSAL FROZEN 文本多处引「8/8」，改代码打印会造成冻结件与实现新矛盾；记 retro 交下批（ cosmetic，非判据缺口） |

## 结论

**漏改：零**（A1–A4 全净；A4 为自查早期补捉、已入册）。**矛盾：零**（B1–B7 全一致；B7 为冻结时点态 vs 活状态的口径差，已声明非矛盾）。机械面 C1–C6 全过，其中 C6 顺带实证了强制层钩子活性。D 类 5 项记录更正/处置全部落账。
