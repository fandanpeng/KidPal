# architect-landing-summary · 261027-gov-review-p6-landing-gate-cluster

packet-id：FROZEN PROPOSAL（v0.2）§2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK(F1–F4) → DRAFT v0.2 → R2 GREEN（N1 括注收口后标 FROZEN） · change-id 261027-gov-review-p6-landing-gate-cluster

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · `--cite-check` 子命令 + 模板自检句③扩双信号源正读数镜像 | .claude/scripts/landing-fidelity-check.mjs · docs/templates/PROPOSAL.template.md | 落地后脚本 1129 行（sha256 `7f1c798b…dbb462`，与 FROZEN 装配基准逐字节一致）；`--phase draft\|landing` 双相位、判据复用 `checkVerbatimQuotes` 行尾归一抽 `findQuoteInTargets` 单源；模板 `:69` 自检句③扩正读数镜像义务 + msys awk/sed 剥 `\r` 伪影随句承载 |
| T2 · 围栏提取逐行状态机 + 自检夹具 | .claude/scripts/landing-fidelity-check.mjs | `extractFencedBlocks` 闭合围栏须独立成行；verbatim-quotes 自检 10→13（新增块完整性判别组） |
| T3 · 目录锚前缀展开兜底 + advisory/模板纪律句同步改写 | .claude/scripts/landing-fidelity-check.mjs · docs/templates/PROPOSAL.template.md | claimed/planned-but-not-landed 收敛 `computeNotLanded`（目录 token 展开兜底、纯文件 token 分支与改前同源）；`dirTokenAdvisories` 文案与模板 manifest 纪律句改写为「可用但劝阻、逐件枚举是正形、展开是兜底不是许可」 |
| 三条 ARQ 收口 | docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md（维持 deferred，文末 append 批 P6 落地注）· docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md（结项）· docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md（结项） | 260924 / 261026 已由 `debt-render.mjs` 自动归位 `docs/debt/gov/_archive/2026/`（位置 = 状态的纯函数）；260914 frontmatter 不动、append 段落在原路径 |

落地通道：主线程 packet-bound——仓外装配（`_scratch/assemble-script.mjs` / `assemble-docs.mjs`，逐编辑旧串恰 1 次 fail-loud）→ sha256 对账 → 拷回 canonical，与 P5 批先例同构；装配后 `_scratch/` 按环内自律删除（V5 过）。

## 2. 验证证据

- **V1** `--self-check`（落地后脚本）：12 组全绿逐值同 B2——新增 `cite-check 21/21`、`dir-anchor-expansion 7/7`，`verbatim-quotes 13`（原 10 + 块完整性 3）；调用顺序 `runIsFrozenSelfCheck` 仍最后。绿。
- **V2** `--cite-check <本 PROPOSAL>`（落地后脚本，draft 相位自动判 landing）：`phase:"landing"`、`frozen:true`、**checked 19 / missing []** / unverified [] / exit 0；原文块 23 条进 notChecked（landing 相位不检，符合设计）；两条 append 命中**归档位兜底**（`docs/debt/gov/_archive/2026/ARQ-261026….md` / `…ARQ-260924….md`，路径如实回报）。绿。
- **V3** `--preflight <本 PROPOSAL>`（FROZEN 后复跑）：`ok:true`、anchorCount 5、advisories []、findings []、exit 0。绿。
- **V4** `git diff --name-only`（debt-render 之后）：禁触面清单（contracts / spec 两文件 / firmware / server / tools / hooks / scope-map / settings / verify-gates / pre-merge-escalation-gate / lib / AGENTS / CLAUDE / DEV-GOVERNANCE）**空输出**；manifest 面命中 = 五件 + 两个 debt-render 生成件 INDEX（`docs/debt/gov/INDEX.md`、`docs/debt/gov/_archive/2026/INDEX.md`，V4 并发口径内如实记入本节）。绿。
- **V5** `_scratch/` 删除后 `ls`：目录不存在，review 目录仅余 `PROPOSAL.md`。绿。
- **V6** 文档字面正推（grep -cF）：`正读数镜像` template = 1 ✓；`可用但劝阻` template = 1、脚本 = 2（T3④ 函数注释 + advisory 文案，R2-N1 修正后口径）✓；`目录须展开为成员枚举` = 0、`目录简写收口期必红` = 0、`landing 门不消费目录锚` = 0（旧句归零）✓；`--cite-check` 脚本 = 9（≥3）✓；三条目 frontmatter——260924 / 261026 各 `status: deferred` = 0、`status: closed` = 1、`closed: 2026-10-27` = 1、`closedBy: 261027-gov-review-p6-landing-gate-cluster` = 1 ✓，260914 `status: deferred` = 1（维持）✓。绿。
- **V7** `debt-render.mjs`（pre-commit 执行，与 V9 同理提前——规避「生成件漏带、另开 chore 补提」先例）→ `debt-doctor.mjs`：260924 / 261026 归位 `_archive/2026/`、主 INDEX 转已结、归档 INDEX 出现薄行；**RED 零命中**。绿（`--check`/`--self-check` 终证 post-commit 复跑）。
- **V9** `changes-index.mjs --upsert`：FROZEN 翻转后实跑（`status:"frozen"` replaced），落地 commit 之前完成。绿。
- **判别夹具（B3 落地复跑）**：`verify-fixtures.mjs` 18/18——四变异（旧闭合规则 / cue 取首个 / 撤归档兜底 / 目录锚不展开）各自变红后恢复绿。绿。
- **行尾双信号源（本批新规约自证）**：`git ls-files --eol` 三件存量均 `i/lf w/lf` + `tr -dc '\r' | wc -c` 字节计数 = 0，两独立信号源一致，落地五件无行尾漂移（git 的 LF→CRLF 警告为 autocrlf 全局 checkout 预告，入库内容恒 LF，与 P5 B22 口径一致）。

## 3. 诚实注记

- **changedCanonicalNotInProposal（V4 并发口径）**：同工作树另有主线程簿记在途件，逐件核性质、不并入本批 manifest——`delivery/workstreams/gov-debt-reduction/task_plan.md`（P6 立项 ack / R1–R2 进度记账，主线程写面）、`delivery/review/INDEX.md` + `delivery/review/.changes-index-state.json`（V9 生成件，随本批 commit 携带）、`docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md`（human 指示新登记条目 + gov INDEX 重渲染，独立事项不消费本批）。以上均非 canonical 实现面。
- **`--cite-check` v1 覆盖面留根（§6 留根 1）**：blockquote 承载的插入段与多 span 混合行内形态不在覆盖面（`coverage.blockquote:false` 如实输出），该类仍归 landing-verifier 人读 diff。
- **V8 未实测**（依赖 FROZEN + landing-verdict，post-commit 执行）；本批无契约 diff。
- 两条已结 ARQ 归位后，本 manifest 所列原路径以 git 删除态出现在改动集（`changed.has` 命中删除行）、归位新路径为 untracked 新增——V2 的归档位兜底已在归位后实测 `missing:[]`，两门口径均闭合。

```landing-manifest
.claude/scripts/landing-fidelity-check.mjs
docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md
docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md
docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md
docs/templates/PROPOSAL.template.md
```
