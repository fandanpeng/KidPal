# landing-verdict · 260928-gov-review-requirement-face

> landing-verifier 独立核验（Track B，2026-09-28）。判定依据 = 结构门脚本 + 逐文件人读 diff + 验证集实跑复现，**不采信 landing-summary 自述**。核验者只读 codebase，本文件为唯一写入。

## 总 verdict：**绿（ok: true）——放行建议成立，merge 仍归 human manual-merge 门**

七路径实际落地 == frozen PROPOSAL v1.0 的 T1–T6 改法，无漏改、无批定矛盾、无敏感面夹带；两处措辞级微差与三处 manifest 外工作树制品均不构成语义越界，逐条列于「差异与注记」。

## 一、结构门（机器）

- `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-face/PROPOSAL.md` → **ok: true，anchorCount 7，anchorUnion 与 manifest 七路径逐一吻合，advisories []，findings []**，exit 0。
- 设计侧门：Track B 证据集无 decision-packet，`proposal-consolidation-check.mjs` 不跑（N/A）；「frozen ↔ 批定一致」由 review.md R2 GREEN（:102 verdict: GREEN，M1–M4 修毕后冻）+ PROPOSAL 头部 FROZEN v1.0 承载。
- 实际 git 变更面 ⊆ manifest ∪ 预期伴生件（batch 目录自身三件 / INDEX.md 再生 / changes-index-state / 工作树遗留两件，见「四」）。

## 二、逐文件人读核对表

| T | 文件 | 核对点 | expected（frozen PROPOSAL） | actual（diff/全文） | 结果 |
|---|---|---|---|---|---|
| T1 | `.claude/agents/story-author.md`（新增） | 只起草不裁定 / 起草三件（write-story 自检 + spec-anchor-check 绿 + 全量读相邻 story 撞车报告）/ 档 2 牵连面清单（关联 story / §2 共享底座 / 线框 / §6 路线图 / §8 已砍清单）/ 同会话 return 不落盘 / 作者≠判官不互兼 / tools = Read, Grep, Glob, Bash 无 Write / Bash 限 spec-anchor-check / 升档明示义务 | T1 改法四条职责边界 | 逐条在场：职责边界 1/2、起草动作 3 件、档位增量（档 1 牵连携带 + ≥2 处或含 NFR/Epic 明示「建议升档 2」）、交付形态 return、硬边界四条 | **ok** |
| T2 | `.claude/agents/requirement-reviewer.md`（新增） | 四查（意图保真/纪律合规/撞车复核/牵连面完整）/ (d) 口径 = 「档 2 必查；携带牵连面清单的档 1 同查」（R2-M4）/ 否回权循环至过 / 不代笔 / 无 Write / 与 story-author 不互兼 / 与 architect-reviewer（契约门）消歧 | T2 改法四条职责边界 | 四查逐条在场，(d) **逐字** = R2-M4 口径（:21）；结论形态 PASS/FAIL + 否回项引原句；「本 agent 无 Write」在册；契约面路由 architect-reviewer 四分类 | **ok** |
| T3 | `.claude/references/requirement-change.md`（新增） | 三档判据 + 两例边界消歧（跨 story 零语义清扫=档 0；NFR 数值微调=档 1）/ 升档规则（≥2 处或含 NFR/Epic）/ 档 1 无 human 终确认 / 档 2 architect「human 指定」+ human 终拍 + §9 依据列承载（OQ-5 (a)）/ 跳过例外记「跳过 story-author/reviewer」/ 定稿载体 = §9 版本行即 changeId（不另铸 ID，TaskPackage 撞名禁用）/ REQ- CRQ 三件必填（owner 禁空 / trigger 禁「human 想起」/ 销行条件） | T3 五块内容 | 五块齐备：三档表含两例消歧（:9-10）；升档规则（:13）；档 1 第 4 步「无 human 终确认」+ 事后否回依据（:28）；档 2 六步含 architect 三态只出意见、human 终拍、§9 依据列浓缩承载（:34-37）；跳过例外（:21，措辞含 AGENTS.md Hard Rules 对齐）；定稿载体（:43-44）；REQ- 三件必填（:48-52）；角色分工表收束 | **ok**（跳过例外置于档 0 节下而非独立小节，内容零缺——见差异注记 A3） |
| T4 | `docs/requirements.md` | §9 纪律引言追加句 == PROPOSAL T4 引文逐字 / 既有句零改动 / 零 story 正文变动 | 追加「语义修改（档 1）与结构性变更（档 2）的变更行以版本号为 changeId（不另铸 ID）……跳过 story-author/reviewer』。流程判据见 requirement-change.md。」 | `git diff HEAD` 单行变更：既有纪律句逐字保留，追加文本与 PROPOSAL T4 引文**逐字一致**；全文件无其它 diff 行 → 零 story 语义改动 | **ok** |
| T5 | `docs/crq/requirement/REQ-260916-home-date-format-english.md`（迁入） | git mv 改名 / frontmatter id 同步 REQ- / 正文与改名前一致 / 旧 ARQ- 文件已不在 | id 改 `REQ-260916-home-date-format-english`，正文内容不变 | `git diff -M` similarity **95%**，唯一变更行 = frontmatter `id:`；`git status` RM 在案；`docs/crq/requirement/` 目录仅 INDEX.md + REQ- 文件，旧 ARQ- 文件已不在 | **ok** |
| T5 | `docs/crq/requirement/INDEX.md`（再生） | crq-render 再生、含 REQ- 条目行 | 生成件再生，不手改 | diff 仅条目行 ARQ-→REQ- 换名（链接 + 行内 ID），title/严重度/触发列逐字不变；:24 REQ- 条目在场 | **ok** |
| T6 | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | 末尾 append 注记在场 / 既有行零改动（append-only） | PROPOSAL T6 引文段落 | diff = 末尾 +2 行（空行 + 注记行），既有 31 行零触碰；注记承载「建议 2 已由本批 T3 承载（三档 + 定稿载体 = §9 版本行即 changeId）；建议 1/3/4 与 /arch-change 退役安置归后续批」，与 PROPOSAL 语义一致，措辞见差异注记 A1 | **ok** |

敏感面核对：两 agent 定义均无 Write、Bash 限 `node .claude/scripts/spec-anchor-check.mjs`（与 T1/T2 批定一致，无权限/写域扩张）；`scope-map.json` / hooks / settings / AGENTS.md / CLAUDE.md / architect.md / write-story skill **零触**（`git diff --name-only` 过滤 `.claude|AGENTS|CLAUDE|scope-map|hooks|settings` 零命中；untracked 仅两新 agent + 一新 reference）。

## 三、验证集复核（独立实跑）

| # | summary 声称 | 本核验实跑 | 结果 |
|---|---|---|---|
| V1 preflight | ok，anchorCount 7，零 advisory | 同命令重跑：ok:true / 7 / [] / exit 0 | **吻合** |
| V2 lint-descriptions | exit 0，flagged 1 = landing-verifier.md 超长（存量） | exit 0，scanned 15 / flagged 1 = landing-verifier.md（363>320，存量既有；两新文件未 flag） | **吻合**（抽核通过） |
| V3 route-regression-check | 28 ok 0 fail | 28 ok, 0 fail | **吻合**（抽核通过） |
| V4 spec-anchor-check | ok:true，13 story / 31 锚行 / errors [] | ok:true，storyCount 13 / anchorLines 31 / errors []，exit 0 | **吻合** |
| V5 crq-render + doctor | INDEX 再生 REQ- 入册；108 条目；RED 零命中 | crq-doctor 实跑：108 条目（gov=89 tech=18 requirement=1）、RED 零命中、exit 0；INDEX REQ- 条目已核（二表 T5 行） | **吻合** |
| V6 全 ID 残留 Grep | 销项达成，三归类清单 | 全仓实跑（跟踪面 git grep + 全树 grep）：命中全部落在 ②（260923 批制品 / 260915 dev 制品 + 本批自身证据引文）与 ③（crq-migrate.mjs:32）；**①类零命中**（INDEX 已随再生换 REQ-）；firmware / server / pytest.ini / 参考树**零命中** | **销项达成**（计数口径见差异注记 A2） |
| V7 | landing 后 commit 时核 | 本批未 commit（工作树态），N/A 如实记录 | — |

packet-binding 核对：landing-summary 引 packet = FROZEN PROPOSAL v1.0 + 本 summary + review.md，与 PROPOSAL §头 packet-id 行一致；landing 实际范围 = manifest 七路径，无越界（结构门 scope 结论 + 上述 diff 抽查双证）。

## 四、破坏性自检

- `.claude/agents/` 既有 14 个定义：`git diff --name-only` + status 双核，**零改动**；仅新增 story-author / requirement-reviewer 两件。
- write-story skill / triage-change / change-triage / arch-change / AGENTS.md / scope-map / hooks / settings：**零改动**。
- 工作树伴生件（manifest 外，逐件定性）：
  - `delivery/changes/arch/INDEX.md` + `delivery/changes/.changes-index-state.json`：lane INDEX upsert 簿记，AGENTS.md 治理路由第 7 步的标准动作，非夹带。
  - `docs/showme/show-me-requirement-face.html`（untracked）：**manifest 外制品，landing-summary 未声称**——不属本批 landing 范围；提示 merge 门勿静默裹挟进本批 commit（如随批提交须独立留痕定性，沿 docs(showme) 簿记先例）。
  - `.codebase-memory/artifact.json` / `graph.db.zst`：机器再生面常驻脏，按既有裁定不追交，与本批无关。

## 五、差异与注记（均非阻断，不构成红项）

- **A1（措辞级）· T6 注记**：落地文本较 PROPOSAL 引文作两处润饰——「（三档判据 + 流程落…）」→「——三档判据 + 两档流程落…」；「建议 1/3/4」追加括注（intake 四面申报 / launchPreconditions 推广②路径 / grilling 收敛，内容即该 ARQ :27-31 原清单的忠实缩写）。承载语义零偏移，append-only 纪律守住。
- **A2（计数口径）· V6 三归类清单**：landing-summary 记 260923 PROPOSAL ×6 / review-packet ×3，实测分别为 ×7（:120/:271/:368/:374/:384/:388/:587）/ ×4（:32/:51/:106/:117）。全部命中仍属 ② 类（append-only 历史留痕），「三归类之外零命中」销项不受影响；系 summary 计数笔误，建议主线程顺手勘正 summary 两数。
- **A3（结构级）· T3 跳过例外落位**：PROPOSAL 将跳过例外列为独立内容块 3，落地置于「档 0」节内作 **跳过例外** 小段（且按 R2 脉冲明确其适用「档 1 级语义修改的一次性直改」）。要素全数在场（允许跳过 + AGENTS.md 对齐 + §9 记「跳过 story-author/reviewer」），无漏义。

## 六、凭据链

human 立项 ack（2026-09-28）→ R1 BLOCK(F1 截断清扫模式， F2 changeId 撞名) → 主线程修订 → R2 GREEN（M1–M4 修毕）→ human 裁 OQ-5=(a) + FROZEN → packet-bound landing → **本独立核验：绿**。

下一步归主线程：commit 分支 → retro report → 前置升级闸 → 自动合并本地 main（remote push 仍 human）；merge 门注意「四」中 showme html 的定性处置。
