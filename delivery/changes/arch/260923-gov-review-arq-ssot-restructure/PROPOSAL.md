# PROPOSAL · 260923-gov-review-arq-ssot-restructure

> **状态：FROZEN v1.0 · 2026-09-24 · 内容冻结，凭据齐备**——① human 立项 ack：2026-09-23/24 会话直接立项并三次裁方向（三域划分含 contract 归 tech、落点 `docs/crq/` + 域改名 requirement、archive 机制问案），及 2026-09-24 指令「改完你就开始 trackB review，通过后landing，明早我看一下再merge」；② architect 评审循环收敛：R1 BLOCK(F1..F10) → R2 BLOCK(G1..G4) → **R3 GREEN**（§7，三轮内收敛未触熔断）；③ OQ 裁定：OQ-1（落点 `docs/crq/`）与 OQ-4（三域命名 Gov/Tech/Requirement + 前缀 ARQ-/TECH-/REQ-）= **human 直接裁定**（OQ-4 命名由 human 委托主线程拟定、按本件落定）；OQ-2=(a) stub、OQ-3=(a) CDQ 保持 + `cdq:` 指针、OQ-5=(a) 不拆批 = **按推荐位执行**，凭 human「通过后landing」指令，**合并前留 human 追认/否决窗口**（明早过目后再 merge；否决即回滚，未合并前全量可逆）。R3-N1（stub「三行」措辞）已随冻结前修订落。
> **版本沿革**：v0.1 单文件平铺单 index → v0.2 三域文件夹 + 域内 INDEX（响应「合并经常冲突」）→ v0.3 根目录迁 `docs/crq/` + 域改名 requirement → v0.4 归档机制落定 + E8–E10 折入 + T6 扩围 → **v0.5 = R1 十项 findings 全量修订**（F1 登记面补 `title` 必填【BLOCKING】；F2 handoff 空档统一为 advisory；F3 例外条款落点补回 T3 + 未结计数对账 29→28；F4 schema/检查枚举与实现对齐、cdq 判据如实降为 prose 纪律；F5 census 口径全部重测重写；F6 六处陈旧数字复扫；F7 留根 1 引据三处订正；F8 E6 证据版本与措辞精确化；F9 注释/行号四处订正；F10 跨面一致性三小项）。
> **v0.4 要点**：① **归档 = 位置的纯函数**（human 2026-09-24 问「gov 下已经很多了，将来 archive 机制怎么定」）——`status: closed` 且带结项年份的条目由渲染脚本**自动归位** `docs/crq/<域>/_archive/<年>/`（镜像 `delivery/workstreams/_archive/<年>/` 先例；**人不搬文件**，重开自动搬回，doctor 红档核「位置 == f(状态)」）；主 INDEX 已结区只留每年一行指针，年度薄行索引在归档目录。**即时生效、不设阈值**。② 「顺延」节名改「**待触发**」（human 问「顺延代表啥」——词不自明即改，与 trigger 列直接对应）。③ 起草期再实测出 3 处缺陷并修复（E8 title 未入 frontmatter 致视图列全空 / E9 迁移源被 stub 覆盖·真空绿守卫拦截 / E10 归档归位实跑），见 §0。
> **承载 ARQ**：本批不承载既有 ARQ 行——它治的是账本自身的形态病（混 + 漂 + **合并冲突**），由 human 2026-09-23/24 会话直接立项。**不关的根**：源队列 29 条未结项中，**28 条内容语义零改动**（只搬运容器）；唯一例外 = `ARQ-260923-schedule-grid-restore-gate` 按 T3 例外条款置 `status: closed`（**源内容本就写明已结**，正文原文保留 + 追加结项段，非语义改写）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。不触 `docs/contracts/**`、非跨系统结构性变更 ⇒ 不升 Track A。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（零实现面、零契约面、可整批回滚；但触 docs 根一处、新增 docs 子目录、95 行存量迁移、19 处消费方文面、`.gitattributes` 合并策略）。
> **review profile**：G2+G3（G2 = `.claude/{agents,commands,references,rules,skills}/**`；G3 = `.claude/scope-map.json` + `.claude/scripts/**`）。G1 触点为零、契约面触点为零、不新增任何门、**不改任何 FROZEN 共享闸件**（v0.3 较 v0.2 再减一件）。
>
> **起草期脚手架在场声明（诚实声明，防「先落后冻」的误解）**：为履行模板【硬项 · 起草期实证义务】，起草期已把本批**几乎全部落地面写入工作树（未 commit）**：机制脚本 4 件、条目数据 94 件 + 主/归档 INDEX（`docs/crq/**`）、stub、模板、`.gitattributes`、`scope-map.json`、`repo-structure.md`、T6 的 19 处文面——并据此跑出 §0-E1–E10 的全部实测。**这不等于 landed**：FROZEN 后按 packet-bound 口径**复核**全量落地面（含评审修订项的增量执行），写 landing-summary，并由 landing-verifier 逐文件独立核验「实际落地 == frozen 清单」。**若评审判 BLOCK，工作树按 findings 修订或整体回滚（`git checkout` + 删新增文件），不以已落为由倒逼通过。**
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scope-map.json
.claude/rules/repo-structure.md
.claude/rules/contracts.md
.claude/skills/gov-arq/SKILL.md
.claude/skills/merge-to-main/SKILL.md
.claude/route-fixtures/surface-alignment.json
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/references/contract-change-evidence.md
.claude/agents/adjudicator.md
.claude/agents/review-moderator.md
.claude/commands/loop-retro.md
.claude/commands/arch-change.md
.claude/commands/review-arch.md
.claude/commands/review-governance.md
.claude/scripts/crq-ledger-shared.mjs
.claude/scripts/crq-render.mjs
.claude/scripts/crq-doctor.mjs
.claude/scripts/crq-migrate.mjs
docs/ARCH-REVIEW-QUEUE.md
docs/glossary.md
docs/templates/crq-item.template.md
.gitattributes
docs/crq/gov/ARQ-260827-contract-mech-gate-gap.md
docs/crq/gov/ARQ-260908-known-red-count-no-mechanical-source.md
docs/crq/gov/ARQ-260908-versioned-guard-repoint-execution.md
docs/crq/gov/ARQ-260909-crossend-consumer-tiers.md
docs/crq/gov/ARQ-260913-auto-mode-classifier-legit-workflow.md
docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md
docs/crq/gov/ARQ-260914-landing-preflight-anchor-parse.md
docs/crq/gov/ARQ-260914-legacy-sxay-story-id-batch.md
docs/crq/gov/ARQ-260914-portfolio-reconcile-launchpreconditions.md
docs/crq/gov/ARQ-260918-pod-session-settings-identity-unobservable.md
docs/crq/gov/ARQ-260922-buildfeature-launch-prep-skip.md
docs/crq/gov/ARQ-260922-pod-requirements-not-authority.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/ARQ-260827-tier1-boot-review.md
docs/crq/gov/_archive/2026/ARQ-260831-cross-surface-check-missing.md
docs/crq/gov/_archive/2026/ARQ-260831-docs-top-level-registry-gap.md
docs/crq/gov/_archive/2026/ARQ-260901-closeout-check-delivery-status-gap.md
docs/crq/gov/_archive/2026/ARQ-260901-g3-hook-registration-gaps.md
docs/crq/gov/_archive/2026/ARQ-260901-g3-server-live-acceptance.md
docs/crq/gov/_archive/2026/ARQ-260901-shim-api-contract-gap.md
docs/crq/gov/_archive/2026/ARQ-260902-acceptance-coverage-python-assert.md
docs/crq/gov/_archive/2026/ARQ-260902-escalate-merge-no-human-path.md
docs/crq/gov/_archive/2026/ARQ-260902-frozen-acceptance-never-run.md
docs/crq/gov/_archive/2026/ARQ-260902-shim-boundary-open-questions.md
docs/crq/gov/_archive/2026/ARQ-260904-gate-regex-uppercase-changeid.md
docs/crq/gov/_archive/2026/ARQ-260905-escalation-gate-live-execfn-gap.md
docs/crq/gov/_archive/2026/ARQ-260905-merge-driver-worktree-topology.md
docs/crq/gov/_archive/2026/ARQ-260905-verify-gates-types-missing.md
docs/crq/gov/_archive/2026/ARQ-260905-worktree-graph-index-rules.md
docs/crq/gov/_archive/2026/ARQ-260906-index-rebuild-dev-status-drift.md
docs/crq/gov/_archive/2026/ARQ-260906-lfc-canonical-anchor-gap.md
docs/crq/gov/_archive/2026/ARQ-260906-lfc-verbatim-crlf-citation-gap.md
docs/crq/gov/_archive/2026/ARQ-260906-live-acceptance-real-path.md
docs/crq/gov/_archive/2026/ARQ-260906-live-graph-dirty-blocks-merge.md
docs/crq/gov/_archive/2026/ARQ-260906-shell-scripts-lf-gitattributes.md
docs/crq/gov/_archive/2026/ARQ-260907-architect-dispatch-model-override.md
docs/crq/gov/_archive/2026/ARQ-260907-escalation-cross-system-overfire.md
docs/crq/gov/_archive/2026/ARQ-260907-friction-event-payload-schema.md
docs/crq/gov/_archive/2026/ARQ-260907-frozen-acceptance-compile-baseline.md
docs/crq/gov/_archive/2026/ARQ-260907-gate-lib-script-separation.md
docs/crq/gov/_archive/2026/ARQ-260907-integrator-adr-canonical-location.md
docs/crq/gov/_archive/2026/ARQ-260907-lfc-citation-docs-prefix.md
docs/crq/gov/_archive/2026/ARQ-260907-merge-closeout-bookkeeping-gap.md
docs/crq/gov/_archive/2026/ARQ-260907-merge-driver-approval-state-desync.md
docs/crq/gov/_archive/2026/ARQ-260907-renormalize-clears-skip-worktree.md
docs/crq/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md
docs/crq/gov/_archive/2026/ARQ-260908-firmware-build-degraded-gate.md
docs/crq/gov/_archive/2026/ARQ-260908-gate-pytest-import-mode-incompat.md
docs/crq/gov/_archive/2026/ARQ-260908-pod-integration-no-legitimate-executor.md
docs/crq/gov/_archive/2026/ARQ-260908-tighten-only-unwritten.md
docs/crq/gov/_archive/2026/ARQ-260908-versioned-artifact-slot-collision.md
docs/crq/gov/_archive/2026/ARQ-260908-versioned-guard-version-literal-coupling.md
docs/crq/gov/_archive/2026/ARQ-260909-dispatch-stamp-telemetry-gaps.md
docs/crq/gov/_archive/2026/ARQ-260909-gate-green-anchor-regex-loose.md
docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-dirty-merge-confusion.md
docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-snapshot-direction.md
docs/crq/gov/_archive/2026/ARQ-260909-index-verdict-anchor-okform-gap.md
docs/crq/gov/_archive/2026/ARQ-260909-precedent-merged-detection-gap.md
docs/crq/gov/_archive/2026/ARQ-260911-cross-package-behavior-owner-gap.md
docs/crq/gov/_archive/2026/ARQ-260911-split-package-implementation-prose-gap.md
docs/crq/gov/_archive/2026/ARQ-260911-tooth-strength-below-contract.md
docs/crq/gov/_archive/2026/ARQ-260911-verdict-uncovered-surface.md
docs/crq/gov/_archive/2026/ARQ-260912-host-werror-comment-acceptance-gap.md
docs/crq/gov/_archive/2026/ARQ-260913-device-round-protocol.md
docs/crq/gov/_archive/2026/ARQ-260913-lfc-anchorable-delivery-gap.md
docs/crq/gov/_archive/2026/ARQ-260914-lfc-citation-references-prefix.md
docs/crq/gov/_archive/2026/ARQ-260915-changes-index-lane-split.md
docs/crq/gov/_archive/2026/ARQ-260915-path-form-family-reference.md
docs/crq/gov/_archive/2026/ARQ-260915-taskpackage-display-numbering.md
docs/crq/gov/_archive/2026/ARQ-260916-acc-gate-archive-skew.md
docs/crq/gov/_archive/2026/ARQ-260916-supervisor-auth-fp-author.md
docs/crq/gov/_archive/2026/ARQ-260916-unit-gate-archive-recursion.md
docs/crq/gov/_archive/2026/ARQ-260917-step5b-main-append-remerge-tier3.md
docs/crq/gov/_archive/2026/ARQ-260920-anchor-parse-fence-lineno.md
docs/crq/gov/_archive/2026/ARQ-260920-changes-index-mergedinmain-stale.md
docs/crq/gov/_archive/2026/ARQ-260920-short-form-citation-family.md
docs/crq/gov/_archive/2026/ARQ-260923-schedule-grid-restore-gate.md
docs/crq/gov/_archive/2026/INDEX.md
docs/crq/requirement/ARQ-260916-home-date-format-english.md
docs/crq/requirement/INDEX.md
docs/crq/tech/ARQ-260831-i1-indexed-asset-on-device.md
docs/crq/tech/ARQ-260831-p4-stroke-dasharray-visual-check.md
docs/crq/tech/ARQ-260831-simulator-lvgl-version-drift.md
docs/crq/tech/ARQ-260831-simulator-truecolor-vs-1bpp.md
docs/crq/tech/ARQ-260831-small-cjk-1bpp-readability.md
docs/crq/tech/ARQ-260901-openclaw-chatcompletions-public.md
docs/crq/tech/ARQ-260902-cert-lifecycle-deadlock.md
docs/crq/tech/ARQ-260902-multiturn-continuity.md
docs/crq/tech/ARQ-260904-v23-asr-swap-quota-recheck.md
docs/crq/tech/ARQ-260906-nvs-csv-wifi-injection.md
docs/crq/tech/ARQ-260906-protocol-version-pinned-constant.md
docs/crq/tech/ARQ-260907-contract-1-5-strip-wording.md
docs/crq/tech/ARQ-260907-engine-deploy-patch-drift.md
docs/crq/tech/ARQ-260907-openclaw-tools-rejection-rootcause.md
docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md
docs/crq/tech/INDEX.md
docs/crq/tech/_archive/2026/ARQ-260902-shim-401-gate-bypass.md
docs/crq/tech/_archive/2026/ARQ-260916-server-mvp-teeth-sweep.md
docs/crq/tech/_archive/2026/INDEX.md
```

> **FROZEN 后格式例外记录（待 human 晨间追认，先例 = 260913-arch-review-spec-split 格式例外追认）**：原 manifest 末行为目录简写 `docs/crq/`——`landing-fidelity-check.mjs` 的落地探测是**逐文件精确匹配**（`git status --porcelain -uall`），目录锚永不可判（preflight 锚提取却接受它，两门口径不一致，已另立 CRQ 条目登记该陷阱）。本次将目录简写**机械展开为成员枚举**（94 条目 + 5 生成 INDEX = 99 行），**授权语义零变化**（同一落地面，仅表示法从目录改逐件）；§2 T1/T3/T4 锚行同步展开以保持「§2 并集 ≡ manifest」。展开由脚本生成、非手打。
>
> （已跑，见 §5-V6）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本件>`。**v0.4 清单复跑实测：`anchorCount:26` == manifest 26 行、`findings:[]`（全项入集，含 `docs/crq/` 与 `.gitattributes`）**；v0.5 修订后须再复跑（§5-V6）。
> **T7 消失记（诚实记录，非删痕）**：v0.1/v0.2 用 `delivery/ledger/` 时，干跑实测 `anchorUnion` **不含该目录**——根因是 `ANCHORABLE_TEST`（`.claude/scripts/lib/governance-gate-shared.mjs:40`，行号按 v0.5 实测订正，R1-F9）的 delivery 分支只认 `delivery/(?:workstreams|milestones|taskpackages|changes)/`，`ledger` 不在其中（与 `ARQ-260913-lfc-anchorable-delivery-gap` 的 milestones 缺席同族）。v0.2 因此增设 T7（扩枚举 + 补夹具 + 连带 OQ-6「`CANONICAL_TEST` 检测面是否同扩」）。**v0.3 把根目录改到 `docs/` 下后该缺陷不存在**——`docs/` 前缀在 `ANCHORABLE_TEST` 与 `CANONICAL_TEST` **两者内**，锚提取与检测两面天然生效 ⇒ **T7 与 OQ-6 一并删除，对 FROZEN 共享件的改动归零**。这是「落点选择」直接消掉一整块机制改动的实例，值得记住。

---

## 0. 想实现的效果与差距

**目标效果一句话**：把「未结项账本」从**一个 124 KB 的人写表格**改成**每域一文件夹、域内自持 `INDEX.md`、条目一条一文件**的机读状态账——登记只写自己那一个小文件、结项只翻一个字段、看某域读该域那份自动生成的 index；**且合并时不再为账本反复解冲突**。

**现状差距（实测，命令与输出留痕）**：

| 实测项 | 值 | 命令 |
|---|---|---|
| 文件体积 / 行数 / 条目数 | 124 484 字节 / 148 行 / 95 条 | `wc -c docs/ARCH-REVIEW-QUEUE.md` |
| 未结 / 已结 | §1 未结 **29**（§1.1 一条、§1.2 十九条、§1.3 九条）；§2 已结 66 | `awk` 分节计数 |
| 单行最大 | 6 495 字符（`ARQ-260918-pod-session-settings-identity-unobservable`） | `awk '{print length($0)}' \| sort -rn \| head` |
| 该文件被 commit 过 | 95 次（登记 28 / 结移·簿记·归位·清理 13 / **合并摩擦相关：按 `冲突\|conflict\|union\|rebase` 计 4 次，放宽含「归位」计 8 次——两种口径都列出，分类词表即口径**） | `git log --oneline --follow -- docs/ARCH-REVIEW-QUEUE.md` |
| 被仓内文件引用（**口径三分，v0.5 按 R1-F5 重测**） | 全仓 worktree **135**；其中历史制品面 `delivery/changes/**` **93**（append-only 不追改，含本批 PROPOSAL 自身）+ `delivery/taskpackages/**` **14**（冻结件）；**活跃引用面 = 22**（排除上述两面 + `delivery/workstreams/**` + `docs/crq/**` 账本本体后的余集，含 stub 自身与 `crq-migrate.mjs`） | `grep -rl ARCH-REVIEW-QUEUE .`（排除 .git/node_modules，逐面 `grep -v` 计数） |
| 脚本机读面 | **零**（`.claude/scripts` 与 `.claude/hooks` 全无命中，grep exit=1） | `grep -rn ARCH-REVIEW-QUEUE .claude/scripts .claude/hooks` |
| 表内转义竖线 | 58 处 `\|` | `grep -oF '\|' docs/ARCH-REVIEW-QUEUE.md \| wc -l` |
| `docs/archive/ARCH-REVIEW-QUEUE.closed.md` | **不存在**（`§0` 与 `repo-structure.md:20` 都声明它是结移目标） | `ls docs/archive/ARCH-REVIEW-QUEUE.closed.md` |

**「零脚本机读」是关键前提**：把该文件由手写件改成 stub，**不破坏任何机械消费方**——消费方全部是 agent 定义与散文指针。

**合并冲突的实证（回应 human「合并经常冲突，每次折腾好几轮」）**：仓内已有两次同型治疗先例，且都记着理由——
- `260915-changes-index-lane-split`（行内 verbatim）：「delivery/changes/INDEX.md 单文件混装 dev / arch 两段——Pod 合并流（dev 段刷行）与 gov 合并流（arch 段 upsert）**每次各改同一文件，跨 lane 合并冲突逐次必现**」⇒ 拆成 `changes/{arch,dev}/INDEX.md`。
- `.gitattributes:12,19`：`progress.md` 与 `taskpackages/**/pod-events.jsonl` 配 `merge=union`，注释 verbatim「**双侧保留即正解**（同 progress.md merge=union 先例形态）」。

**落点选择的说明（回应 human「放在 docs 目录下」）**：账本**本来就住在 `docs/`**（`docs/ARCH-REVIEW-QUEUE.md` / `docs/CONTRACT-DECISION-QUEUE.md` 都是 docs 根文件）。迁 `docs/crq/` 是**在原地铺开**，不是把账本搬到另一棵树——概念位移比迁 `delivery/` 小，且活跃引用面（22 件，口径见上表）全部经 stub 路径继续可达。代价是 `docs/` 从此承接一个**会持续增长的记录库**（不再只是「正式顶层文档」），该语义扩张须在 `repo-structure.md` 显式写明（T6），不静默。

**起草期实证（已跑，2026-09-24，命令与输出留痕）**：

| # | 实证 | 结果 |
|---|---|---|
| E1 | `crq-migrate.mjs --dry-run` | 源 95 行 → **去重后 94 条**；ID 集合全等；域分布 **gov=76 / tech=17 / requirement=1**；源表格转义竖线 58 处 |
| E2 | `crq-migrate.mjs`（实跑）+ `--verify` | 落盘 94 文件；**逐条对账（源 → 意图）仅 1 条有差**（`ARQ-260902-cert-lifecycle-deadlock` +177，双重落位合并注记）；**逐条对账（意图 → 落盘）零差异**。**v0.5 口径更新（R1-F6）**：E10 例外处置改写 `ARQ-260923-schedule-grid-restore-gate` 正文（追加结项段 +316）后复跑 `--verify`，该条为**第二处已知语义差**——两处差异均为**显式声明的迁移期处置**（双重落位合并注记 / 例外条款结项），非静默漂移 |
| E3 | `crq-render.mjs --self-check` | ok **8/8**（v0.4 增归档归位纯函数双向夹具、归档指针/薄行夹具；含排序确定性、连渲染逐字节相同、竖线转义、已结区薄行、域过滤） |
| E4 | `crq-doctor.mjs --self-check` | ok **13/13**（v0.4 增 mislocated 双向夹具）；**反转实证**：临时摘掉 `prefix-domain-mismatch` 检查 → 自检变红（判别力成立） |
| E5 | `crq-doctor.mjs`（真仓） | **RED 零命中**；advisory 134（`no-paths` 94 · `legacy-prefix-off-domain` 18 · `tech-no-handoff` 17 · `severity-off-rubric` 5） |
| E6 | **合并实演（V11）**，两场景（异日期 / 同日期） | **两场景均 clean（零人工解冲突）**；union 产物 vs 应渲染的差**只有两类**：① 行序（各自分支的行在前，未按排序规则）；② **摘要计数陈旧**——精确表述（R1-F8 修正）：两分支的摘要行**各自相同**（各计本侧 +1），union 对相同行平凡取一，而**并集真值比任一侧多 1**（14/14 → 真值 15）：计数行天然无法表达「两侧各 +1」；**两行都在、零丢失、零重复**；`crq-doctor` 当场报 `index-stale`，**重渲染精确归位**。**证据版本注（R1-F8）**：本次实演跑于 v0.3 期渲染器（title 列空、节名「顺延」）；**R1 判官已用 v0.4 现版脚本在 scratch 仓重演「双侧各结一条」场景**——merge 零冲突、doctor `index-stale` 当场抓红、重渲染归位，机制结论不变（§7-R1 亲核留痕） |
| E7 | 实演环境的连带发现 | Windows checkout 下工作树为 CRLF、脚本产 LF ⇒ 渲染器不归一就「每次都判变了」而重写（V5/V10 会永久假红）。**已修**（渲染前行尾归一化）——该缺陷由实演暴露，非推导 |
| E8 | **人读核（V8 提前做）抓出数据丢失** | 迁移首跑后 `title` 未写入 frontmatter ⇒ 生成视图 title 列**全空**。**已修**（lib `REQUIRED_FIELDS`/`FM_ORDER` + migrate 补 `title`，重迁重渲染后逐条核过）——**教训：机械对账（长度/ID 集合）抓不住「字段整体漏迁」，人读核不可省** |
| E9 | **迁移源被 stub 覆盖 + 真空绿守卫拦截** | T2 把 `docs/ARCH-REVIEW-QUEUE.md` 转 stub 后复跑迁移 ⇒ 源解析 0 行，脚本 **FATAL 拒绝**（「真空输入，拒绝继续」）而非静默产 0 文件。**已修**：migrate 增 `--src <path>`，复跑口径 = `git show <落地前 commit>:docs/ARCH-REVIEW-QUEUE.md`（守卫按设计生效的实例） |
| E10 | **归档归位实跑（v0.4 机制）** | `crq-render` 自动归位 65 条已结（gov 63→`_archive/2026/`、tech 2）+ `ARQ-260923-schedule-grid-restore-gate` 按 T3 例外条款置 closed 后归位（64+2=66 已结）；归位后 `gov/` 顶层 = **12 未结 + INDEX**；render 自检 8/8、doctor 自检 13/13（含「位置 == f(状态)」红档夹具双向）、真仓 **RED 零命中**、advisory 134 |

**E6 的意义（这是「止冲突三件套」的验收证据）**：件 2（`merge=union`）确实让合并**零人工干预**；但 union 会产出一个**内部不自洽**的中间态（计数陈旧、行序未归一）——**这正是件 3 必须与件 2 成对的原因**：`crq-doctor` 把这个中间态**当场看见**，重渲染把它归位。若只挂 union 不配 doctor 检测，仓里就会长期留着一份「行都对、数不对」的 index，而没人知道。

**非目标（本批不做）**：

- 不改 29 条未结项的**内容语义**（只搬运容器）。
- 不新增任何门（不注册钩子、不动 `verify-gates.json`）——doctor / 渲染器作**告警级 / landing-time 手动验证工具**（`lint-descriptions.mjs` 同款先例）。
- **不改任何 FROZEN 共享闸件**（v0.3 已无 T7；`governance-gate-shared.mjs` 零触点）。
- 不动契约面（`docs/contracts/**`）与 CDQ 的**机制**（契约类条目在 tech 域以指针形态登记，见 T3）。
- 不触 L0 蓝图（`AGENTS.md` / `CLAUDE.md` / `DEV-GOVERNANCE.md` 零触点）。

---

## 0b. 本批的设计判据（三条可用性轴 + 一条冲突判据）

**轴 1 · 人怎么好用好看**——人读账本要答三问：*现在有什么未结？这条为什么还挂着？上次那件事后来怎样了？*
现状三问全差：① 要在一张 124 KB 表里翻，且 66 条已结（约 64 KB）压在尾部挡路；② 要读一个最长 6 495 字符、无换行边界的单元格；③ 只能 `git log -S <ID>` 在 95 次提交里捞——一次编辑的 diff 是**一整行 6 495 字符的增删**。
答复：薄行 index（可扫）＋ 一条目一文件（详情有换行边界）＋ **条目级 git 历史**（`git log -- docs/crq/gov/ARQ-xxx.md` 即此事全部沿革，旧形态给不了）＋ index 头部摘要（T2-R1）。

**轴 2 · agent 怎么好维护好找**——agent 要做四件事：登记 / 追加证据 / 结项 / 登记前查重。
现状**读取成本是硬的**：124 484 字节，**每次登记都要先读整个文件**才能安全地改（判官收口、`gov-arq` 查重同理）；按中文 UTF-8 约 3 字节/字估，单次量级三四万 token——**每登记一次付一次的固定税**。查重命中一行返回的是 6 495 字符的墙。
答复：登记 = 读模板 + 写自己那个小文件（不读他人条目）；查重 = `grep -l <关键词> docs/crq/*/*.md`（命中即文件名）；**文件名即 ID ⇒ 可由 ID 直接猜出路径**。

**轴 3 · git 证据面**——账本的主要价值之一是「半年后接手的 agent 靠 diff + 提交历史继续」（`repo-structure.md:28` 对 workstreams 的准入判据即此口径）。
答复：`git log -- docs/crq/` 即账本时间线，且历史天然按条目切分。

**轴 4 · 合并冲突面（承重）**——**分不分、怎么分，判据是「有没有互不相干的写者会撞同一文件」，不是「内容是不是两类」。**
v0.1 曾据此答「账本整体一个 index」——**该结论经 human 反驳后自证为错**：v0.1 论证是「视图是生成件、没人手写 ⇒ 零写者冲突」，但**生成本身就是一次写**，每个改条目的人都要重跑渲染、重写同一份 index ⇒ index 成为**新的单一热点**，与 `changes INDEX` 单文件同病。故拆三份 index 并叠两层缓解（T2）。**该判据本身保留**——它既是拆分的依据（三域的写者互不相干），也是 §6 留根里「不要按别的轴再分」的依据。

> **三域成立的判据（三角验证）**：三域之所以成立，不是「内容像不像」，而是**三条互不相干的写者/裁定线**——gov 由 architect 裁、tech 交给各面 owner、requirement 由 human 裁。**同时满足「有独立写者」与「有独立裁定人」，才配一个独立 index。**

---

## 2. 问题、根因与改法（T1…T6）

**【硬项 · 起草期实证义务】**（标签去数字）本节断言纪律：

- 凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；无实测不写断言，写「未实测」。
- 凡声称「对偶 / 比照 <先例>」→ 先读先例文件，引 file:line verbatim 确认形态再落笔。
- 凡「全 / 均 / 无」量化声明 → 每次改写其覆盖集合时，对新入集成员重新取证并留痕。
- 凡引用 commit SHA → 先 `git rev-parse <sha>` 核实存在，输出留痕。
- 凡数字引用 → 起草期现跑实测或核对权威原处；不得转引陈旧制品数字。
- 凡引用脚本 / 闸 / CLI 入口（如 `--self-check`）→ 起草期亲核入口存在。
- 搬运 / 改写既有文本中的引据视同新断言复证。

> **本件所处的起草阶段如实声明（v0.5 更新）**：① `--preflight` 已跑多次且**三次抓到真缺陷**（`delivery/ledger/` 不可锚 → T7；T6 转述式锚行被 fail-loud 点名；v0.3 清单复跑见 §5-V6）；② **迁移脚本已写并实跑**——dry-run 数字与逐条对账见 §0-E1/E2，**「95 行」的表述须更正为「95 行 → 去重后 94 条」**（源有一 ID 双重落位，见 T1 改法 7）；③ **合并实演已跑（V11/E6）**，结果与残留说明见 §0；④ **三域逐条落域表已由 dry-run 实况替换**（T3 表已改，歧义项落 §6 留根 12）；⑤ 起草期共暴露并修复 7 处自设计缺陷（前缀↔域 与「不改名」互斥、severity 闭集凭印象、CRLF 未归一、title 漏迁、迁移源被 stub 覆盖、adjudicator 写面三处漏改——两轮判官均未点名而由 v0.5 自查补捉、adjudicator description 改写丢「queue」路由信号——由 `route-regression-check` 机检当场抓红后修复，**该夹具的判别力在本批自身上得到实证**），均记 §0-E4/E5/E7/E8/E9 与 T3/T5/T6 内。**⑥ 同族数字复扫纪律（R2-G1 建议，本批起生效）**：连续两轮评审抓到「改了一处数字、全文同族未复扫」（R1-F6 六处、R2-G1 十处）——**FROZEN 前固定动作：凡改动任一计数/节名/路径形态，立即 `grep -n "<同族 token>"` 全文复扫（PROPOSAL 与落地文面两面），清零后方可标 FROZEN**；是否升格进 `PROPOSAL.template.md` 起草 checklist 见 §6 留根 18。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列「目标文件：<仓根相对路径清单>」（**裸列路径、不得转述**——本件曾在 T6 犯此错并被门 fail-loud 抓住，见上注）。

### T1 · 条目改为一条一文件 + 每域一文件夹（SSOT 化 + 一次性迁移）

**问题**：队列是一个 124 KB 的单文件表格，95 条共用一张表。任意一次登记或编辑都要改这一个大文件；条目的**身份**（历史记录）与**用途**（可变状态）压在同一个 markdown 单元格里。后果已发生四次（有 commit 为凭）：① `d1216f5`「8 条陈旧行结移 §2 + **修 3 处表格结构损坏**」；② `61527e0`「260916 两条**漂移行由 §2 已结索引移回 §1**」；③ ARQ-260912-host-werror 的**整行被邻行覆盖**、来源格内容丢失，2026-09-18 修表时只能按行内证据重建；④ 此刻在案：`ARQ-260923-schedule-grid-restore-gate` 只在 [ARCH-REVIEW-QUEUE.md:26](docs/ARCH-REVIEW-QUEUE.md#L26)（§1.1）出现、§2 无对应行，而「处理时机」格已写着「**2026-09-23 已结**」——就地结项未搬。另有 6 条 §1 行内带「已结 / 部分结」字样（`260923-schedule-grid-restore-gate` / `260914-legacy-sxay-story-id-batch` / `260914-landing-preflight-anchor-parse` / `260913-current-pointer-implicit-global` / `260901-openclaw-chatcompletions-public` / `260902-cert-lifecycle-deadlock`）。

**证据**：§0 表全部行；`git log --oneline --follow -- docs/ARCH-REVIEW-QUEUE.md`（95 条）；四次事故 commit 如上。

**根因**：`AGENTS.md` 硬规则「历史记录类文件一律 append，不覆盖既有行」与「队列行必须改状态」在同一个格子里互斥。合规解法是往格子里**追加注记**（[ARCH-REVIEW-QUEUE.md:51](docs/ARCH-REVIEW-QUEUE.md#L51) `ARQ-260914-landing-preflight-anchor-parse` 已叠四层），格子无界增长到 5 915 字符；不合规解法是直接改，于是有了事故 ③。**两条路都通到漂移——这是容器形态的病。**

**改法**：

1. 新增账本根 **`docs/crq/`**（CRQ = change request 账，三域共处一账），下设**三个域文件夹**：`gov/` · `tech/` · `requirement/`（域定义与命名见 T3）。**每条目一文件、放在所属域文件夹内**，文件名即条目 ID（`ARQ-<YYMMDD>-<slug>.md` / `TECH-<YYMMDD>-<slug>.md` / `REQ-<YYMMDD>-<slug>.md`）；**已结条目由渲染器自动归位 `<域>/_archive/<结项年>/`**（T2 改法 6，v0.4）。文件形态 = **YAML frontmatter（全部机读状态）+ 正文（append-only 证据散文）**——可变状态与历史记录拆到两个物理位置，append 铁律与状态可变性不再互斥。
   > **对 v0.1 的两处反转（如实记录）**：① v0.1 主张「平铺、不按域分子目录」，理由是「用文件夹区分等于宣称它们不是同一类东西，那就不该共用一套机制」——**该推论不成立**：分文件夹只说明**条款/读者/门不同**，而**一套机制可以实例化三份产出**（同一 lib、同一渲染器、同一 doctor，三份域配置）；真正该避免的是「建三套机制」。human 2026-09-23/24 指出合并冲突是实证痛点，三域各有互不相干的写者线（§0b 轴 4），故改为分文件夹。② 根目录由 `delivery/ledger/` 改 `docs/crq/`（human 2026-09-24 裁）——账本原本就住 `docs/`，此为原地铺开（见 §0 落点说明），并顺带消掉 T7（见文首「T7 消失记」）。
2. frontmatter 字段闭集（字段名以 `crq-ledger-shared.mjs` 为单源；**v0.5 与实现对齐，R1-F4**）：
   **必填** `id` / `title` / `domain`(gov·tech·requirement) / `status`(blocker·deferred·accepted-risk·closed，**与现行 §1.1/§1.2/§1.3 三节一一对应**) / `severity`（已结项豁免）/ `opened`；
   **建议** `owner` / `trigger` / `paths[]`(待改的确切路径，见 T3) / `source[]`(provenance) / `closed` / `closedBy`；
   **域内可选** `handoff`·`cdq`（tech）/ `affects`·`decision`·`landedIn`（requirement）；
   **机检联动可选** `gateLayers`（自报 G 层，须与 `paths[]` 机推一致，不一致 doctor 红）/ `hardExclusions`（`paths[]` 命中硬排除时应标 `true`，未标 doctor advisory）。
   正文承载四段（沿用现行队列行的散文四要素，一字不改地搬运）：`**证据**` / `**影响面**` / `**建议改动**` / `**风险**`。**正文只在尾部追加、不改既有段**——append 纪律从「不能改行」变成「追加到本文件尾」，冲突面从「与 94 条争一行」降到「只有你自己写这个文件」。
3. **「已结」的表示法（本批的核心语义变更）**：结项**不再是搬家**，而是**原地翻字段**——`status: closed` + 填 `closed`(日期) + `closedBy`(依据链接)，正文尾部追加一行「**结项**：…」。条目文件**永远不换位置、不换结构、不重打 ID 与标题**。现行「§1 六列 → §2 四列」的重打动作被删除；事故 ② 与 ④ **由建构消除**——状态在文件内、文件不搬家，就没有「搬错位置」这个失败模式。
4. **一次性迁移**：`.claude/scripts/crq-migrate.mjs` 把现行 95 行机械切分为条目文件，按 T3 判据落进三个文件夹。**实跑结果 = 95 行 → 去重后 94 条 → 94 文件**（域分布 gov 76 / tech 17 / requirement 1；见 §0-E1/E2）。必须处理三套列结构（§1 六列 / §1.3 六列但语义不同 / §2 四列）与 **58 处转义竖线 `\|`**（切分后还原为裸 `|`）。保真断言 = ID 集合全等 + **双向逐条对账**（源→意图、意图→落盘各一次；后者已实测零差）。
5. **迁移期的三处实况处置（起草期实测后加入，不预批）**：
   - **5-1 · 一 ID 双重落位**：源里 `ARQ-260902-cert-lifecycle-deadlock` 在 §1.3（watchlist）与 §2（已结索引）**各出现一次**（§1.1 的注记行自己记着「双重落位」）。新模型**一条一项一状态**，双重落位不可能 ⇒ 必须裁决唯一 status。迁移脚本以**显式裁决表**（`DUAL_PLACEMENT`）取 `accepted-risk`（监控面为活面），把 §2 的决策闭环原文作为正文段落保留，并在正文写明裁决理由；**无裁决记录的重复 ID 一律 fail-loud、不代猜**。
   - **5-2 · 源里一条表格行含未转义裸竖线**（`ARQ-260914-portfolio-reconcile-launchpreconditions`，7 个 cell）：解析改为**锚首三列 + 锚末两列**（来源 / 处理时机）、中间全部归正文，对行内裸竖线稳健；该行须人读核过。
   - **5-3 · 列数异常行标记**：dry-run 打印 `⚠ 列数=N` 供人读核（现值仅 1 条，即 5-2）。
6. 新增 `docs/templates/crq-item.template.md`（骨架，镜像 `PROPOSAL.template.md` / `retro.template.md` 先例）。
7. **不加顶层 `docs/crq/README.md`（有意省略）**：说明的单一源拆给三处——schema 在 lib、怎么填在 template、怎么读在 index 头部；再加一份 README 就是**第四处需同步的说明**，与「消灭多源」的批旨相悖。

**为什么这样改（对哪个根因）**：一条一文件消灭「与邻行争同一格」整类事故（事故 ③ 不可能再发生）、消灭表格结构损坏（事故 ①）、消灭跨结构重打（T2 根因）。SSOT 化的仓内先例 = `delivery/milestones/`（`milestones.jsonl` 为源 + 脚本再生 `STATUS.md` + 纪律「手改派生件 STATUS.md（改状态去改 milestones.jsonl 再重跑脚本）」，`repo-structure.md:27`）。

**按四轴回查**：轴 1 → 条目级 git 历史 + 详情有换行边界；轴 2 → 单次登记读取量从「整个 124 484 字节」降到「模板 + 自己的小文件」，查重 `grep -l` 命中即文件名，文件名即 ID 可猜路径；轴 3 → `git log -- docs/crq/` 即时间线；轴 4 → **条目层写冲突归零**（index 层冲突留给 T2，v0.1 在此处漏判）。

**已计入的代价（诚实声明）**：95 件迁移是本批最大机械工作量，迁移脚本须过一轮人读 diff 核验。**`docs/` 语义扩张**：`repo-structure.md` 现定义 docs 子目录为 contracts / notes / ui / templates / archive（正式文档面），`crq/` 是**会持续增长的记录库**——须在 `repo-structure.md` 显式写明该语义（T6），不静默扩。**同名物**：`docs/crq/` 下有 3 个同名 `INDEX.md`，且仓内另有 `delivery/changes/{arch,dev}/INDEX.md`——**任何引用必须带域目录**（写 `docs/crq/gov/INDEX.md`），禁裸写 `INDEX.md`。

目标文件：`docs/crq/gov/ARQ-260827-contract-mech-gate-gap.md` · `docs/crq/gov/ARQ-260908-known-red-count-no-mechanical-source.md` · `docs/crq/gov/ARQ-260908-versioned-guard-repoint-execution.md` · `docs/crq/gov/ARQ-260909-crossend-consumer-tiers.md` · `docs/crq/gov/ARQ-260913-auto-mode-classifier-legit-workflow.md` · `docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md` · `docs/crq/gov/ARQ-260914-landing-preflight-anchor-parse.md` · `docs/crq/gov/ARQ-260914-legacy-sxay-story-id-batch.md` · `docs/crq/gov/ARQ-260914-portfolio-reconcile-launchpreconditions.md` · `docs/crq/gov/ARQ-260918-pod-session-settings-identity-unobservable.md` · `docs/crq/gov/ARQ-260922-buildfeature-launch-prep-skip.md` · `docs/crq/gov/ARQ-260922-pod-requirements-not-authority.md` · `docs/crq/gov/INDEX.md` · `docs/crq/gov/_archive/2026/ARQ-260827-tier1-boot-review.md` · `docs/crq/gov/_archive/2026/ARQ-260831-cross-surface-check-missing.md` · `docs/crq/gov/_archive/2026/ARQ-260831-docs-top-level-registry-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260901-closeout-check-delivery-status-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260901-g3-hook-registration-gaps.md` · `docs/crq/gov/_archive/2026/ARQ-260901-g3-server-live-acceptance.md` · `docs/crq/gov/_archive/2026/ARQ-260901-shim-api-contract-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260902-acceptance-coverage-python-assert.md` · `docs/crq/gov/_archive/2026/ARQ-260902-escalate-merge-no-human-path.md` · `docs/crq/gov/_archive/2026/ARQ-260902-frozen-acceptance-never-run.md` · `docs/crq/gov/_archive/2026/ARQ-260902-shim-boundary-open-questions.md` · `docs/crq/gov/_archive/2026/ARQ-260904-gate-regex-uppercase-changeid.md` · `docs/crq/gov/_archive/2026/ARQ-260905-escalation-gate-live-execfn-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260905-merge-driver-worktree-topology.md` · `docs/crq/gov/_archive/2026/ARQ-260905-verify-gates-types-missing.md` · `docs/crq/gov/_archive/2026/ARQ-260905-worktree-graph-index-rules.md` · `docs/crq/gov/_archive/2026/ARQ-260906-index-rebuild-dev-status-drift.md` · `docs/crq/gov/_archive/2026/ARQ-260906-lfc-canonical-anchor-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260906-lfc-verbatim-crlf-citation-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260906-live-acceptance-real-path.md` · `docs/crq/gov/_archive/2026/ARQ-260906-live-graph-dirty-blocks-merge.md` · `docs/crq/gov/_archive/2026/ARQ-260906-shell-scripts-lf-gitattributes.md` · `docs/crq/gov/_archive/2026/ARQ-260907-architect-dispatch-model-override.md` · `docs/crq/gov/_archive/2026/ARQ-260907-escalation-cross-system-overfire.md` · `docs/crq/gov/_archive/2026/ARQ-260907-friction-event-payload-schema.md` · `docs/crq/gov/_archive/2026/ARQ-260907-frozen-acceptance-compile-baseline.md` · `docs/crq/gov/_archive/2026/ARQ-260907-gate-lib-script-separation.md` · `docs/crq/gov/_archive/2026/ARQ-260907-integrator-adr-canonical-location.md` · `docs/crq/gov/_archive/2026/ARQ-260907-lfc-citation-docs-prefix.md` · `docs/crq/gov/_archive/2026/ARQ-260907-merge-closeout-bookkeeping-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260907-merge-driver-approval-state-desync.md` · `docs/crq/gov/_archive/2026/ARQ-260907-renormalize-clears-skip-worktree.md` · `docs/crq/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md` · `docs/crq/gov/_archive/2026/ARQ-260908-firmware-build-degraded-gate.md` · `docs/crq/gov/_archive/2026/ARQ-260908-gate-pytest-import-mode-incompat.md` · `docs/crq/gov/_archive/2026/ARQ-260908-pod-integration-no-legitimate-executor.md` · `docs/crq/gov/_archive/2026/ARQ-260908-tighten-only-unwritten.md` · `docs/crq/gov/_archive/2026/ARQ-260908-versioned-artifact-slot-collision.md` · `docs/crq/gov/_archive/2026/ARQ-260908-versioned-guard-version-literal-coupling.md` · `docs/crq/gov/_archive/2026/ARQ-260909-dispatch-stamp-telemetry-gaps.md` · `docs/crq/gov/_archive/2026/ARQ-260909-gate-green-anchor-regex-loose.md` · `docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-dirty-merge-confusion.md` · `docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-snapshot-direction.md` · `docs/crq/gov/_archive/2026/ARQ-260909-index-verdict-anchor-okform-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260909-precedent-merged-detection-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-cross-package-behavior-owner-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-split-package-implementation-prose-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-tooth-strength-below-contract.md` · `docs/crq/gov/_archive/2026/ARQ-260911-verdict-uncovered-surface.md` · `docs/crq/gov/_archive/2026/ARQ-260912-host-werror-comment-acceptance-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260913-device-round-protocol.md` · `docs/crq/gov/_archive/2026/ARQ-260913-lfc-anchorable-delivery-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260914-lfc-citation-references-prefix.md` · `docs/crq/gov/_archive/2026/ARQ-260915-changes-index-lane-split.md` · `docs/crq/gov/_archive/2026/ARQ-260915-path-form-family-reference.md` · `docs/crq/gov/_archive/2026/ARQ-260915-taskpackage-display-numbering.md` · `docs/crq/gov/_archive/2026/ARQ-260916-acc-gate-archive-skew.md` · `docs/crq/gov/_archive/2026/ARQ-260916-supervisor-auth-fp-author.md` · `docs/crq/gov/_archive/2026/ARQ-260916-unit-gate-archive-recursion.md` · `docs/crq/gov/_archive/2026/ARQ-260917-step5b-main-append-remerge-tier3.md` · `docs/crq/gov/_archive/2026/ARQ-260920-anchor-parse-fence-lineno.md` · `docs/crq/gov/_archive/2026/ARQ-260920-changes-index-mergedinmain-stale.md` · `docs/crq/gov/_archive/2026/ARQ-260920-short-form-citation-family.md` · `docs/crq/gov/_archive/2026/ARQ-260923-schedule-grid-restore-gate.md` · `docs/crq/gov/_archive/2026/INDEX.md` · `docs/crq/requirement/ARQ-260916-home-date-format-english.md` · `docs/crq/requirement/INDEX.md` · `docs/crq/tech/ARQ-260831-i1-indexed-asset-on-device.md` · `docs/crq/tech/ARQ-260831-p4-stroke-dasharray-visual-check.md` · `docs/crq/tech/ARQ-260831-simulator-lvgl-version-drift.md` · `docs/crq/tech/ARQ-260831-simulator-truecolor-vs-1bpp.md` · `docs/crq/tech/ARQ-260831-small-cjk-1bpp-readability.md` · `docs/crq/tech/ARQ-260901-openclaw-chatcompletions-public.md` · `docs/crq/tech/ARQ-260902-cert-lifecycle-deadlock.md` · `docs/crq/tech/ARQ-260902-multiturn-continuity.md` · `docs/crq/tech/ARQ-260904-v23-asr-swap-quota-recheck.md` · `docs/crq/tech/ARQ-260906-nvs-csv-wifi-injection.md` · `docs/crq/tech/ARQ-260906-protocol-version-pinned-constant.md` · `docs/crq/tech/ARQ-260907-contract-1-5-strip-wording.md` · `docs/crq/tech/ARQ-260907-engine-deploy-patch-drift.md` · `docs/crq/tech/ARQ-260907-openclaw-tools-rejection-rootcause.md` · `docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md` · `docs/crq/tech/INDEX.md` · `docs/crq/tech/_archive/2026/ARQ-260902-shim-401-gate-bypass.md` · `docs/crq/tech/_archive/2026/ARQ-260916-server-mvp-teeth-sweep.md` · `docs/crq/tech/_archive/2026/INDEX.md`（新增：94 条目 + 5 生成 INDEX；目录简写之机械展开，见 manifest 下格式例外注）· `.claude/scripts/crq-migrate.mjs`（新增）· `docs/templates/crq-item.template.md`（新增）

### T2 · 每域一份自持 `INDEX.md`（生成件）+ 三件套止冲突

**问题**：同一个状态住在多处——ARQ 行、ARQ §2、`delivery/changes/arch/INDEX.md`、包内 landing-verdict；一次状态转移要人工同步多处（本仓已实测失真两回：`ARQ-260920-changes-index-mergedinmain-stale` 合并后不翻 yes、`ARQ-260909-index-verdict-anchor-okform-gap` verdict 形态一变推导静默不翻）。而 §1（六列）→ §2（四列）的重打被 [dev-loop.md:338](.claude/references/dev-loop.md#L338) 自认为「列结构为**簿记纪律**、非机检」。**更关键的是 v0.1 漏判的新病灶：若只做一份生成 index，它会被每个改条目的人重写 ⇒ 成为新的合并热点。**

**证据**：§0 的合并冲突实证两则；`dev-loop.md:338` verbatim；`4b76e5f`→`f7b33f6` 间 5 次「结移 §2」簿记 commit。

**根因**：状态是**派生**的（由条目 frontmatter 决定），却被当**源**手写两份（§1 表 + §2 表），且两份结构不同；而**「生成」这个动作本身是一次写**，凡写就参与合并冲突。

**改法（含止冲突三件套）**：

1. `.claude/scripts/crq-ledger-shared.mjs`：解析 / 序列化 / 字段闭集 / **域配置表**（域 → 文件夹 / index 路径 / ID 前缀 / 额外字段）/ 排序规则 / G 层与硬排除推导的**单源库**（镜像 `changeid-shared.mjs` / `governance-gate-shared.mjs` 先例）。**一套机制、三份域配置**。
2. `.claude/scripts/crq-render.mjs`：从 `docs/crq/**` 渲染**三份主 `INDEX.md` + 每域每年一份归档 `INDEX.md`**（`gov/` · `tech/` · `requirement/`），**分节由 `status` 派生**（`blocker` → 待拍板 / `deferred` → **待触发** / `accepted-risk` → 已接受风险 / `closed` → 归档区年度指针行）。支持 `--self-check`。
   **「已结」的机制答复**：已结**不是搬家**——它是 `status: closed` 一格；分节是渲染时按字段算出来的，结项**只改一个字段**，条目文件原地不动。**没有搬家，就没有「搬错位置」**（事故 ②、④ 在机制上不可能再发生）。
3. **止冲突三件套（本 T 的承重部分）**：
   - **件 1 · 按域拆**：每域自持 index（主 INDEX 三份 + 归档年 INDEX，生成件现共 5 份、随归档年增长），一个域的写者只碰自己域的文件 ⇒ 跨域批次合并零接触。
   - **件 2 · `merge=union` 挂全部账本 index**（`.gitattributes` 两行 glob：`docs/crq/*/INDEX.md` + `docs/crq/*/_archive/*/INDEX.md`，覆盖主与归档年）：残余的「两分支各加一行」自动并成**双侧保留**——照仓内两处先例（`progress.md`、`pod-events.jsonl`）的同一形态与同一理由。
   - **件 3 · 生成自愈 + 显式报「待重渲染」**：union 结果可能乱序/重复，**下次渲染即归位**；doctor 的「渲染后 `git diff` 非空」把「待重渲染」**显式报出来**（不静默）。**件 2 与件 3 必须成对落地**——只挂 union 不报，乱序结果会静默留在仓里。
4. `.claude/scripts/crq-doctor.mjs`（T5）承担件 3 的检测。
5. **`docs/ARCH-REVIEW-QUEUE.md` 降为指路 stub**：手写一次、**不是生成件**（简短指路：标题 + 三域去向表 + 登记/结项指引——R3-N1 修正，初稿「三行」为早期设想残留，实物形态以此为准）。**路径不变 ⇒ 全部既有引用零断链**（全仓 135 = 活跃引用面 22 + `delivery/changes` 93 + `delivery/taskpackages` 14 + `delivery/workstreams` 5 + 账本本体 1，口径见 §0 表；stub 正文**不固化计数**，R1-F5）；该文件此后几乎永不再改 ⇒ **从 124 KB 热点变成冷文件**。**该处置是 OQ-2，不预批。**
6. **归档机制（v0.4 新增，human 2026-09-24 裁定「gov 下已经很多了，将来 archive 机制怎么定」）**：**位置 = 状态的纯函数，搬文件只有脚本干，人永远不搬**。
   - 判据：`status: closed` 且 `closed` 带年份 → `docs/crq/<域>/_archive/<年>/<ID>.md`（镜像 `delivery/workstreams/_archive/<年>/` 先例）；未结 / 已结但无日期 → 域顶层。**即时生效、不设数量阈值**（human 明说「已经很多了」——阈值触发靠人记得跑，恰是本批要消灭的病）。
   - **双向、幂等**：条目重开（closed → 其它）下次渲染自动搬回顶层；归位由 `crq-render.mjs` 写盘模式顺手执行（`--check` 模式纯只读），漏跑由 doctor 红档 `mislocated` 兜住——「搬错位置」失败模式没有回来，因为**目的地是算出来的、且位置本身受机检**。
   - **视图**：主 INDEX 已结区只渲染「每年一行指针 + 计数」；年度薄行索引 = `_archive/<年>/INDEX.md`（同为生成件，同受 R6 守卫）。**主 INDEX 从此永久短**：gov 域实测从「13 未结 + 63 已结薄行」变为「12 未结 + 1 行年度指针」（E10）。
   - **合并面**：`.gitattributes` 的 union 行同步覆盖 `docs/crq/*/_archive/*/INDEX.md`（归档 index 几乎不变，冷面）。
   - **轴 2 的诚实退化**：「由 ID 猜路径」对已结条目变为「由 ID+状态猜」（未结在顶层、已结在 `_archive/<年>/`）——但已结条目本就靠 INDEX 链接点入、grep 全域有效，实际检索路径不变。
   - **不取的两个替代**（记录裁决理由）：结项时人工搬归档（把本批刚消灭的「搬错位置」请回来）；未结条目也按年分片（未结才 12 条，分片只破坏可猜性、换不到东西）。

**视图的可用性要求（轴 1 落地，逐条可判）**：

- **R1 · 头部摘要（只含计数，不含任何时间依赖）**：每份 index 顶部渲染「未结 N（待拍板 a / 待触发 b / 已接受风险 c）；已结 M（归档索引见下）」。人打开第一屏就答完「现在什么情况」。**起草期已删掉初稿里的「最近 7 天新增 n 条」**——它依赖渲染日，会让「无改动也 diff 非空」，把 R5 的确定性、V5/V10 与 doctor 的漂移检测一并变成永久假红。**这是 R5 的一次真实踩坑记录，不是理论顾虑。**
- **R2 · 分节只按 `status`**，与现行 §1.1/§1.2/§1.3/§2 一一对应（人的心智模型不变）；域不再作为列出现（域已由文件夹与 index 承载）。**节名自明化（v0.4，human 问「顺延代表啥」）**：`deferred` 节名由「顺延」改「**待触发**」——三档语义 = **待拍板**（等你裁）/ **待触发**（等 `trigger` 列写的时机）/ **已接受风险**（故意不修、只监控）；词不自明即改，标签单源在 lib 的 `STATUS_SECTIONS`。
- **R3 · ID 渲染为可点相对链接**（如 `[ARQ-260923-xxx](ARQ-260923-xxx.md)`，index 与条目同目录，链接是同目录相对形态）。
- **R4 · 已结区必须是真·薄行**：`ID / title / 已结日期 / 依据链接` 四列、**不含证据正文**。现状 §2 的 66 条把约 64 KB 证据堆在尾部，是「翻不到未结」的主因；薄行化后 66 条约 8 KB。
- **R5 · 确定性渲染（承重，非可选）**：排序规则**钉死在 lib 单源**——**未结按 `opened` 升序、同日期按 ID 升序；已结按 `closed` 升序、同日期按 ID 升序**；域与 severity 不参与排序。**两个理由**：① T5 漂移检测（「重跑后 `git diff` 为空」）**完全依赖确定性**，顺序随文件系统枚举序变动则检测恒红、等于废掉；② **升序 ⇒ 新条目落节尾 ⇒ 两分支 diff 是「各自末尾追加」⇒ union 合并几乎总无冲突**（降序则新条目落节首、两分支插同一位置，union 也会打架）。**「升序」是为合并友好而选的，与「最新在前」的直觉相反**，故在此明写取舍理由。
- **R6 · `--render` 拒绝覆写非生成件**：目标文件若无生成标记头（被手写覆盖或替换过），**fail-loud 拒写**并提示人工核对。这是「禁手改」的另一半。
- **R7 · 行尾归一化后再比对（起草期实演暴露的真缺陷）**：Windows checkout 下工作树为 CRLF、脚本产 LF ⇒ 不归一即「每次都判变了」而重写，**V5/V10 永久假红**。渲染器与 doctor 读取侧一律先 `\r\n → \n` 再比对（已修，见 §0-E7）。

**为什么这样改（对哪个根因）**：状态**只有一处源**（条目 frontmatter），index 是派生品——「同一状态住多处」结构性消除；结项退化为翻字段 + 重跑；index 层合并冲突由三件套压到「罕见且自愈」。

**残余风险（诚实声明）**：派生 index 会**陈化**——本仓已有两次实测先例（changes-index 缓存、手写 known-red 快照）。件 3 把「忘了重跑」变成**可被看见**的状态，但**不假装它归零**；`merge=union` 的实际表现**尚待 T2 起草期的临时分支实演**（V12）。

目标文件：`docs/ARCH-REVIEW-QUEUE.md`（改，转 stub）· `.claude/scripts/crq-ledger-shared.mjs`（新增）· `.claude/scripts/crq-render.mjs`（新增）· `.gitattributes`（改）· `.claude/rules/repo-structure.md`（改）

### T3 · 三域定义（Gov / Tech / Requirement）+ 非本域去向纪律

**问题**：一条队列同时装性质不同的东西，行契约就得对所有种类成立 ⇒ 每行自带一段「机检硬排除逐项声明」（触 `docs/contracts/**`？改 agent `tools`/`name`/`model`？self-def？新增门？mixed lane？L0？）。**这六个布尔完全由 touched paths 决定，却被每一行重推一次、再辩一次**——6 495 字符格子的主要体积来源。

**域集与命名（human 2026-09-24 裁定）**：

| 域（人读名） | 文件夹 | index | ID 前缀 | 谁裁 | 落点 / 出口 |
|---|---|---|---|---|---|
| **Gov**（治理机制：agent / skill / 门 / 脚本 / 纪律文面） | `docs/crq/gov/` | `gov/INDEX.md` | `ARQ-` | architect | 本账的 Track B/A 批次 |
| **Tech**（技术相关，**含 contract**） | `docs/crq/tech/` | `tech/INDEX.md` | `TECH-` | 各面 owner | 去向由 `handoff` 字段承载 |
| **Requirement**（改需求请求） | `docs/crq/requirement/` | `requirement/INDEX.md` | `REQ-` | human（需求权威） | `docs/requirements.md` §9 记账 |

命名取舍（须经 human 确认）：
- **`ARQ-` 沿用**——95 条在册，改名要动全部引用且无收益。**代价如实记**：`ARQ`（ARCH-REVIEW-QUEUE）这名字与容器 `crq` 不再对应，属**历史词汇**；本批不追改，留 §6 留根 11。
- **`REQ-`**：与文件夹 `requirement/` 同名同源，自解释。**须与 story ID（`S-Fx.y`）区分**——二者不同族，`REQ-` 为 kebab+日期 形态，无 `S-` 前缀，不会撞。**弃用 `CRQ-`**：容器叫 `crq`，再用 `CRQ-` 做条目前缀会与容器名混淆（v0.2 曾拟 `CRQ-`，v0.3 改）。
- **`TECH-`** 选自解释而非 `TRQ-`（后者要解码，且不像 ARQ/CDQ 有历史包袱值得编码）。
- index 一律 `INDEX.md`（在各自文件夹内，与 `delivery/changes/{arch,dev}/INDEX.md` 同名同形）。

**按域落位实况（`crq-migrate.mjs` 实跑，2026-09-24；本节数字以脚本输出为准，非人工点算）**：

| 域 | 未结 | 已结 | 合计 |
|---|---|---|---|
| gov | 13 | 63 | 76 |
| tech | **15** | 2 | 17 |
| requirement | 1 | 0 | 1 |
| **合计** | **29** | **65** | **94** |

> **实况更正（如实记录）**：v0.2 的人工点算表写「gov 16 / tech 12」，并据此说「gov 是最大头」。**实跑打翻该判断**——未结项里 **tech 15 > gov 13**，最大头在 tech。这不影响三域划分（划分判据是写者/裁定线，不是条数），但**影响一处论证**：v0.1 曾把「固件/工具」与「ops/部署」拆成两域，本版合并为 tech，**合并后的 tech 恰是最大待办域** ⇒ 它的 index 是最该被单独读的那一份，分域的价值因此比 v0.2 估计的更高。
>
> **总计口径**：源 95 行 → 去重后 94 条（一 ID 双重落位，见 T1 改法 5-1）；未结 29 条与源 §1 行数一致（源 §1 = 29 行，其中 `cert-lifecycle-deadlock` 同时出现在 §2，故去重发生在跨节、不影响 §1 计数）。**上表为迁移时点态**；T3 例外条款（见下「存量迁移策略」）把 `ARQ-260923-schedule-grid-restore-gate` 置 closed 后，现值 = **未结 28（gov 12 / tech 15 / requirement 1）+ 已结 66**（E10 实测）。

**根因**：**没有可机读的域维度**，于是「这条由谁裁、该去哪」只能靠散文自证。

**改法**：

1. frontmatter 增 `domain`，闭集**三值** `gov` / `tech` / `requirement`。**三值而非更细**，因为三值各有机械判据（独立写者线 + 独立裁定人，§0b 轴 4）；**再往下切没有判据**——v0.1 曾把「固件/工具」与「ops/部署」列为两域，**本版砍掉并说明理由**：`protocol-version-pinned-constant`（固件设计）、`nvs-csv-wifi-injection`（固件 bug）、`engine-deploy-patch-drift`（部署副本漂移）三者唯一共同点是「不该由架构裁定、该交回实现/运维面」；再分 dev vs ops **无机械判据**（同一件事可两读）。**一个没有判据的域划分，只会把「分类争论」从评审期搬到登记期**——正是本批要治的病的变体。故「去向」不再冒充「域」。
2. frontmatter 增 `handoff`（**tech 域去向 + 移交触发**）：契约 → `CDQ-<id>`；固件 / 工具 / ops → `delivery/workstreams/<ws>` 或 `delivery/taskpackages/<id>`；spec 文档 → 主线程 spec 批。`domain: tech` 且 `handoff` 为空 → doctor **advisory**（`tech-no-handoff`，现值 17——迁移项普遍未填；**v0.5 按 R1-F2 归一：T5 两档划分为准，本条不再是「红」**；新登记项按 skill 纪律应填，长期空置由 advisory 计数曝光）。
3. **契约类的双账处理**：human 裁定 tech **含 contract**。为免**双账**（同一决定两处记状态 ⇒ 漂移），**tech 域可收 contract 类条目，但该类条目必修 `cdq: CDQ-<id>` 指针字段**，且**门与批准状态仍以 CDQ 为唯一权威**（本账只记「有这回事 + 门在 CDQ-xxx」）。**是否把 CDQ 整体并入 tech 账**（一份账、取消 CDQ）是 OQ-3，本件不预批——那会动契约门的记录面。
4. **分流纪律（本批语义主项，须成文）**：账本保留未结项的**登记与可见性**，但工作本身交出去——账本存的是「已交 X 面、去向 Y、触发 Z」，不是「活在我这儿」。
5. **6 个机检硬排除布尔 → 1 个 `paths[]`**：由 `crq-doctor.mjs` 从路径**推** G 层与硬排除结论（输入本来就有——`gov-arq` skill 步骤 3 现在就在手推「列出要改的确切路径 → 机判 G 层」）。这一项单独就能砍掉当前格子体积的大头。
6. **前缀 ↔ 域 的机检规则（起草期修正，原表述自相矛盾）**：v0.3 初稿写的是「`ARQ-` ⇒ gov 的可机检不变量」——**该规则与本批「迁移不改名」直接冲突**：迁移后 18 条 `ARQ-` 开头的条落在 tech（17）与 requirement（1），doctor 一跑就报 18 条红（真仓实测，见 §0-E5）。修正为**两段式**：
   - **新条目**：必须用本域前缀（`TECH-` / `REQ-` / `ARQ-`），前缀与域不符 ⇒ **红**。
   - **祖辈前缀 `ARQ-`**：在任何域都**放行**，只是**告警计数**（`legacy-prefix-off-domain`）——它是历史登记词汇，`ARQ` 原义（ARCH-REVIEW-QUEUE）已与容器名 `crq` 脱钩，此债在册（§6 留根 11）。
   **为什么这样定而不是「把 18 条改名」**：改名要动全仓对这批 ID 的引用（taskpackage / pod-events / 文档），收益不抵成本；而「前缀是历史词汇、`domain` 字段才是机器真值」这条口径，本来就该这么写。
7. **severity 闭集按实况并集（起草期修正，原闭集凭印象）**：v0.3 初稿把闭集写成 rubric 的五档（`BLOCKING/SHOULD/NIT/OPTIONAL/FYI`），**但源队列实际用的是 `SHOULD` 23 / `COULD` 4 / `NIT` 1 / `MAJOR` 1**——`COULD` 与 `MAJOR` 不在 rubric 内（真仓实测，见 §0-E5 的 `bad-severity` 红项）。修正为：
   - **闭集 = 并集**（`BLOCKING/MAJOR/SHOULD/COULD/NIT/OPTIONAL/FYI`）——迁移**不造数据、不静默改档**；
   - **非 rubric 五档者出 advisory**（`severity-off-rubric`，现值 5 条），把「队列实况用词 ≠ 它自己引用的 rubric」这条漂移**显式留在册**，不掩盖。该漂移本身另立 §6 留根 13。

**存量迁移策略**：按仓内先例「**随批迁移**」（`launchPreconditions` 存量 12 条即如此处置）——本批迁移 94 条时按域落文件夹、补 frontmatter；此后新登记项走新格式；未迁格式条目由 doctor 计数（§6 留根 5）。

**例外条款（v0.5 补回落点，R1-F3——E10 与条目结项段引用的「T3 例外条款」即本段）**：源 §1.1 的 `ARQ-260923-schedule-grid-restore-gate` 属「就地结项未搬」在案漂移（T1 事故④：内容已结——human 2026-09-23 裁 accept-as-risk、契约门批准、spec/契约同 PR 落地，状态却滞留未结区）。**本批迁移时将其直接置 `status: closed`**（`closed: 2026-09-23`、`closedBy` 引 ADR-005 与该批目录，正文尾追加「**结项**」段声明处置依据）——这是**迁移期唯一一条状态改写**，其余 93 条状态一律照源搬运。**计数对账**：源 §1 = 29 行未结 → 迁移后未结 **28**（gov 12 / tech 15 / requirement 1）+ 已结 **66**；本件其余处「29 条未结」均指**源行数口径**，迁移后口径以本句为准。

目标文件：`docs/crq/gov/ARQ-260827-contract-mech-gate-gap.md` · `docs/crq/gov/ARQ-260908-known-red-count-no-mechanical-source.md` · `docs/crq/gov/ARQ-260908-versioned-guard-repoint-execution.md` · `docs/crq/gov/ARQ-260909-crossend-consumer-tiers.md` · `docs/crq/gov/ARQ-260913-auto-mode-classifier-legit-workflow.md` · `docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md` · `docs/crq/gov/ARQ-260914-landing-preflight-anchor-parse.md` · `docs/crq/gov/ARQ-260914-legacy-sxay-story-id-batch.md` · `docs/crq/gov/ARQ-260914-portfolio-reconcile-launchpreconditions.md` · `docs/crq/gov/ARQ-260918-pod-session-settings-identity-unobservable.md` · `docs/crq/gov/ARQ-260922-buildfeature-launch-prep-skip.md` · `docs/crq/gov/ARQ-260922-pod-requirements-not-authority.md` · `docs/crq/gov/_archive/2026/ARQ-260827-tier1-boot-review.md` · `docs/crq/gov/_archive/2026/ARQ-260831-cross-surface-check-missing.md` · `docs/crq/gov/_archive/2026/ARQ-260831-docs-top-level-registry-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260901-closeout-check-delivery-status-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260901-g3-hook-registration-gaps.md` · `docs/crq/gov/_archive/2026/ARQ-260901-g3-server-live-acceptance.md` · `docs/crq/gov/_archive/2026/ARQ-260901-shim-api-contract-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260902-acceptance-coverage-python-assert.md` · `docs/crq/gov/_archive/2026/ARQ-260902-escalate-merge-no-human-path.md` · `docs/crq/gov/_archive/2026/ARQ-260902-frozen-acceptance-never-run.md` · `docs/crq/gov/_archive/2026/ARQ-260902-shim-boundary-open-questions.md` · `docs/crq/gov/_archive/2026/ARQ-260904-gate-regex-uppercase-changeid.md` · `docs/crq/gov/_archive/2026/ARQ-260905-escalation-gate-live-execfn-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260905-merge-driver-worktree-topology.md` · `docs/crq/gov/_archive/2026/ARQ-260905-verify-gates-types-missing.md` · `docs/crq/gov/_archive/2026/ARQ-260905-worktree-graph-index-rules.md` · `docs/crq/gov/_archive/2026/ARQ-260906-index-rebuild-dev-status-drift.md` · `docs/crq/gov/_archive/2026/ARQ-260906-lfc-canonical-anchor-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260906-lfc-verbatim-crlf-citation-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260906-live-acceptance-real-path.md` · `docs/crq/gov/_archive/2026/ARQ-260906-live-graph-dirty-blocks-merge.md` · `docs/crq/gov/_archive/2026/ARQ-260906-shell-scripts-lf-gitattributes.md` · `docs/crq/gov/_archive/2026/ARQ-260907-architect-dispatch-model-override.md` · `docs/crq/gov/_archive/2026/ARQ-260907-escalation-cross-system-overfire.md` · `docs/crq/gov/_archive/2026/ARQ-260907-friction-event-payload-schema.md` · `docs/crq/gov/_archive/2026/ARQ-260907-frozen-acceptance-compile-baseline.md` · `docs/crq/gov/_archive/2026/ARQ-260907-gate-lib-script-separation.md` · `docs/crq/gov/_archive/2026/ARQ-260907-integrator-adr-canonical-location.md` · `docs/crq/gov/_archive/2026/ARQ-260907-lfc-citation-docs-prefix.md` · `docs/crq/gov/_archive/2026/ARQ-260907-merge-closeout-bookkeeping-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260907-merge-driver-approval-state-desync.md` · `docs/crq/gov/_archive/2026/ARQ-260907-renormalize-clears-skip-worktree.md` · `docs/crq/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md` · `docs/crq/gov/_archive/2026/ARQ-260908-firmware-build-degraded-gate.md` · `docs/crq/gov/_archive/2026/ARQ-260908-gate-pytest-import-mode-incompat.md` · `docs/crq/gov/_archive/2026/ARQ-260908-pod-integration-no-legitimate-executor.md` · `docs/crq/gov/_archive/2026/ARQ-260908-tighten-only-unwritten.md` · `docs/crq/gov/_archive/2026/ARQ-260908-versioned-artifact-slot-collision.md` · `docs/crq/gov/_archive/2026/ARQ-260908-versioned-guard-version-literal-coupling.md` · `docs/crq/gov/_archive/2026/ARQ-260909-dispatch-stamp-telemetry-gaps.md` · `docs/crq/gov/_archive/2026/ARQ-260909-gate-green-anchor-regex-loose.md` · `docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-dirty-merge-confusion.md` · `docs/crq/gov/_archive/2026/ARQ-260909-graph-cache-snapshot-direction.md` · `docs/crq/gov/_archive/2026/ARQ-260909-index-verdict-anchor-okform-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260909-precedent-merged-detection-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-cross-package-behavior-owner-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-split-package-implementation-prose-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260911-tooth-strength-below-contract.md` · `docs/crq/gov/_archive/2026/ARQ-260911-verdict-uncovered-surface.md` · `docs/crq/gov/_archive/2026/ARQ-260912-host-werror-comment-acceptance-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260913-device-round-protocol.md` · `docs/crq/gov/_archive/2026/ARQ-260913-lfc-anchorable-delivery-gap.md` · `docs/crq/gov/_archive/2026/ARQ-260914-lfc-citation-references-prefix.md` · `docs/crq/gov/_archive/2026/ARQ-260915-changes-index-lane-split.md` · `docs/crq/gov/_archive/2026/ARQ-260915-path-form-family-reference.md` · `docs/crq/gov/_archive/2026/ARQ-260915-taskpackage-display-numbering.md` · `docs/crq/gov/_archive/2026/ARQ-260916-acc-gate-archive-skew.md` · `docs/crq/gov/_archive/2026/ARQ-260916-supervisor-auth-fp-author.md` · `docs/crq/gov/_archive/2026/ARQ-260916-unit-gate-archive-recursion.md` · `docs/crq/gov/_archive/2026/ARQ-260917-step5b-main-append-remerge-tier3.md` · `docs/crq/gov/_archive/2026/ARQ-260920-anchor-parse-fence-lineno.md` · `docs/crq/gov/_archive/2026/ARQ-260920-changes-index-mergedinmain-stale.md` · `docs/crq/gov/_archive/2026/ARQ-260920-short-form-citation-family.md` · `docs/crq/gov/_archive/2026/ARQ-260923-schedule-grid-restore-gate.md` · `docs/crq/requirement/ARQ-260916-home-date-format-english.md` · `docs/crq/tech/ARQ-260831-i1-indexed-asset-on-device.md` · `docs/crq/tech/ARQ-260831-p4-stroke-dasharray-visual-check.md` · `docs/crq/tech/ARQ-260831-simulator-lvgl-version-drift.md` · `docs/crq/tech/ARQ-260831-simulator-truecolor-vs-1bpp.md` · `docs/crq/tech/ARQ-260831-small-cjk-1bpp-readability.md` · `docs/crq/tech/ARQ-260901-openclaw-chatcompletions-public.md` · `docs/crq/tech/ARQ-260902-cert-lifecycle-deadlock.md` · `docs/crq/tech/ARQ-260902-multiturn-continuity.md` · `docs/crq/tech/ARQ-260904-v23-asr-swap-quota-recheck.md` · `docs/crq/tech/ARQ-260906-nvs-csv-wifi-injection.md` · `docs/crq/tech/ARQ-260906-protocol-version-pinned-constant.md` · `docs/crq/tech/ARQ-260907-contract-1-5-strip-wording.md` · `docs/crq/tech/ARQ-260907-engine-deploy-patch-drift.md` · `docs/crq/tech/ARQ-260907-openclaw-tools-rejection-rootcause.md` · `docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md` · `docs/crq/tech/_archive/2026/ARQ-260902-shim-401-gate-bypass.md` · `docs/crq/tech/_archive/2026/ARQ-260916-server-mvp-teeth-sweep.md`（条目 frontmatter，T1 落盘面的域字段承载，逐件展开）· `.claude/scripts/crq-doctor.mjs`（新增）· `.claude/skills/gov-arq/SKILL.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）

### T4 · `requirement` 域：记「请求」，`requirements.md` §9 记「已落」

**问题**：**改需求的请求现在无处可放**。三种非正式落点各行其是：写成 ARQ 行（混进治理队列，如 `260916-home-date-format-english`）；写成 `docs/requirements.md` §2.3 的 ⚠️ 实现侧对账注记；或直接进 §9 变更记录。而 §9 是**已发生变更的台账**（v1.2 → v1.57 全在里头，append-only），**结构上装不了「还没落地的请求」**。

**证据**：`docs/requirements.md` §9 表头 `| 版本 | 日期 | 变更 | 依据 |`（动词全为已发生态）；`ARQ-260916-home-date-format-english` 行内 verbatim「human 裁定改英文 `DD Mon` 格式……需求/设计文档尚未同步」；`ARQ-260918-spec-face-3page-reconcile` 行内 verbatim「`docs/requirements.md` 行 166 ② 页序漂移台账行未消解」。

**根因**：需求变更的生命周期有**三段**——提出请求 / 裁定 / 落地记账——而仓内只有首尾两端有家（§7 开放问题放「未定」、§9 放「已落」），中间那段「**已裁、未落**」无处安放。

**改法**：

1. `requirement` **只是三域之一**，与另两域**同机制**（同一 lib、同一渲染器、同一 doctor），只差三项域配置：文件夹 `requirement/`、index 名 `INDEX.md`、ID 前缀 `REQ-`。
2. **schema 不另起一套**：共用 T1 的 frontmatter 核心，`domain: requirement` 的条目**多三个域内可选字段**——`affects[]`（受影响的 story·AC 锚，如 `S-F1.1-AC1`）/ `decision`(human 裁定原文) / `landedIn`(`docs/requirements.md` §9 的版本号)。其余字段语义与另两域一致。
3. **分工一句话（本批要钉死的语义边界）**：`docs/requirements.md` §9 = **已发生的变更**（历史账，append-only 不动）；`requirement` 域条目 = **变更的请求与裁定状态**（待办账）。落地动作 = append 一行进 §9 + 回填 `landedIn` + `status` 翻 `closed`。
4. 写权：登记与回填 = 主线程（`scope-map` 里 `docs/requirements.md` 无任何 agent allow，实况即「human / 主线程」）；本批**不授予**任何 review-loop / dev-loop agent 写权。**注意**：本 T **不写 `docs/requirements.md`**（该文件的改动仍归 human / 主线程既有通道）。
5. 现有那 1 条需求变更项（`260916-home-date-format-english`）迁为 `requirement` 域首条——**保留原 ID `ARQ-260916-home-date-format-english` 不改名**（v0.4 修正：v0.3 曾拟换前缀为 `REQ-`，与 T3 改法 6 的祖辈前缀规则「迁移不改名」自相矛盾，由起草期自查抓出；`REQ-` 前缀从**下一条新登记**起启用，该条以 `legacy-prefix-off-domain` advisory 在册）。

**为什么这样改（对哪个根因）**：补上生命周期缺的那一段，并把「改需求」这一格正名分出——它天然是一条**可审计链**（请求 → 裁定 → §9 版本行）。**复用而非新立**：增量只有「一个域值 + 三个域内字段 + 一个文件夹」，机制增量为零。

目标文件：`docs/crq/requirement/ARQ-260916-home-date-format-english.md`（requirement 域首条，T1 落盘面成员）· `.claude/scripts/crq-render.mjs`（改）

### T5 · `crq-doctor.mjs`：把「簿记纪律」变成机检事实

**问题**：`.claude/references/dev-loop.md:338` 的「队列表格列结构」一行明写它是**约定非机检**——「行内列数错位即坏行（约定非机检，列结构为簿记纪律）」。一句自认没有牙的纪律，对应的是事故 ①「3 处表格结构损坏」。

**证据**：`dev-loop.md:338` verbatim；`d1216f5` commit message verbatim「修 3 处表格结构损坏」。

**根因**：列结构、ID 唯一性、必填字段、**渲染一致性**这些**完全可机判**的性质，被当成了人工纪律。

**改法**：新增 `.claude/scripts/crq-doctor.mjs`（**告警级 / 只读，不注册钩子、不进任何闸链**），检查项**分两档**（起草期实测后确定分档——迁移项普遍带历史欠账，若一律判红则 V4「零红」不可达，且红线会立刻失去意义）：

**红档（结构性错误，须修）**：

1. frontmatter 必填字段齐全 + 值落闭集（`domain` / `status` / `severity`；**已结项豁免 severity**——源 §2 无该列，迁移不造数据）。
2. ID 唯一（跨三域）+ ID 形态合规（`^(ARQ|TECH|REQ)-\d{6}-[a-z0-9-]+$`）+ **前缀↔域**（T3 改法 6 的两段式：新条目前缀不符 ⇒ 红）。
3. **`paths[]` → G 层推导**：条目自报 `gateLayers` 与机推不一致 ⇒ **红**（`gate-layer-mismatch`）。**v0.5 拆分（R2-G2，F2 同族残留）**：`paths[]` 命中硬排除面而未标 `hardExclusions: true` ⇒ **advisory**（`hard-exclusion-undeclared`，见 advisory 项 11）——「自报 ≠ 推导」是矛盾（红），「未申报」是欠账（advisory），两性质不同档。
4. **漂移检测（合并三件套的件 3）**：盘上 `INDEX.md` ≠ 应渲染 ⇒ 红「**待重渲染**」（覆盖两种成因：有人手改；`merge=union` 并出的乱序/陈旧计数）。
5. 生成件不在场 ⇒ 红；文件无 frontmatter 块 ⇒ 红。

**advisory 档（内容性待补，只计数、不阻断；v0.5 与实现对齐，R1-F4）**：

6. `domain: tech` 且 `handoff` 空（`tech-no-handoff`，现值 17——迁移项未填去向）。
7. `paths[]` 为空（`no-paths`，现值 94——G 层不可机判）。
8. 祖辈前缀 `ARQ-` 出现在非 gov 域（`legacy-prefix-off-domain`，现值 18——历史词汇债，T3 改法 6）。
9. `severity` 在并集内但非 rubric 五档（`severity-off-rubric`，现值 5——队列用词与 rubric 漂移，T3 改法 7）。
10. 已结但无 `closed` 日期（`closed-no-date`，现值 0——无法按年归档）。
11. `paths[]` 命中硬排除但未标 `hardExclusions: true`（`hard-exclusion-undeclared`，现值 0——迁移项 paths 均空）。
12. **`--self-check` 夹具 13 组**：每项检查须有「**反转实现即变红**」的正负夹具（遵仓内夹具判别力纪律）。**起草期已实测判别力**：临时摘掉 `prefix-domain-mismatch` 检查 → 自检变红（§0-E4）。

**为什么这样改（对哪个根因）**：把 T1–T3 与 T2 件 3 的纪律各配一颗**可证伪**的牙。

**边界（如实声明）**：本项**不新增门**——不写 `verify-gates.json`、不注册 hook、不改 `settings.json`，故不触「新增 gate」硬排除。是否值得日后升级为门是 §6 留根 2。

目标文件：`.claude/scripts/crq-doctor.mjs`（新增）· `.claude/scripts/crq-ledger-shared.mjs`（新增，见 T2）

### T6 · 消费方文面改写（19 处，v0.4 按 census 复查扩围）+ 三种同名物消歧

**问题**：账本的「怎么用」写在多处 agent 定义 / 命令 / reference / rule / skill / fixture 里，全部按**手写表格**的形态描述。SSOT 化 + 分域后这些文面不改，下一个会话会照旧往无处写。

**证据**：修订后终测（`grep -rl ARCH-REVIEW-QUEUE`，排除 .git/node_modules/`delivery/**`/`docs/crq/**`）= **活跃引用面 22 件**：`.claude` 10 件 = **5 件入本 T 清单**（adjudicator〔description/:39/:50 三处 v0.5 补捉，见起草期声明⑤〕/ repo-structure / scope-map / surface-alignment / merge-to-main——改写后仍合法保留 stub 路径或历史陈述）+ **4 件豁免**（architect.md self-def / taskpackage-authoring 历史引注 / kidpal-idf-env / kidpal-nvs-ops 纯指针）+ **1 件账本机制自身**（crq-migrate 读历史源）；docs 5 件 = 2 件入清单（stub 本体、glossary 词条）+ 3 件豁免（ADR / DEV-GOVERNANCE〔G1〕/ architecture 纯指针）；`server/deploy` 6 件 + `tools` 1 件豁免（纯指针）。豁免全清单见 §6 留根 16。本 T 清单 **19 处需改语义**（初版 16，v0.4 补 3，留根 14）。

**根因**：文面按已废弃的形态描述使用方式。

**改法**：逐处把「往 `docs/ARCH-REVIEW-QUEUE.md` 写一行」改为「往 `docs/crq/<域>/<ID>.md` 写条目 + 重跑 `crq-render.mjs`」。清单（逐处落地时人读 diff 核）：

| 文件 | 改动性质 |
|---|---|
| `.claude/agents/adjudicator.md` | 写面（:77 现写「本次新增的 open findings / deadlocks / spawned-concerns」）——**须写明按域落哪个文件夹** |
| `.claude/agents/review-moderator.md` | 读面（:61 读现存条目） |
| `.claude/commands/loop-retro.md` | 产面（:40 ARQ candidate 登记） |
| `.claude/commands/arch-change.md` | 收口核验（:42 制品齐备判定含更新 queue） |
| `.claude/commands/review-arch.md` | 收口核验（:31 同款） |
| `.claude/commands/review-governance.md` | 收口核验（:33 同款） |
| `.claude/references/arch-review-protocol.md` | :170 SPAWN-NEW-CONCERN 登记行 |
| `.claude/references/dev-loop-operating-system.md` | :158 登记纪律段 + :173 出口指针 |
| `.claude/references/dev-loop.md` | :338 表格列结构约定行（转生成件后语义整条更换） |
| `.claude/references/architect-operating-model.md` | :29 决策账本清单 |
| `.claude/references/contract-change-evidence.md` | :17 Queue update 行 |
| `.claude/rules/contracts.md` | :15 指向 ARQ 的机制缺口引用 |
| `.claude/rules/repo-structure.md` | docs 根白名单 + `crq/` 新行 + **docs 语义扩张说明** + **三种同名物消歧** |
| `.claude/scope-map.json` | `docsTopLevel.allowDirs` 增 `crq` + 写权槽（`integrator` / `adjudicator` 增 `docs/crq/**`） |
| `.claude/skills/gov-arq/SKILL.md` | 全篇（登记动作从「填表格行」改「写条目文件 + 重跑渲染」，并写明三域判据） |
| `.claude/skills/merge-to-main/SKILL.md` | :18 热点清单（「本仓热点如 `docs/ARCH-REVIEW-QUEUE.md`」已不成立——stub 冷文件 + `docs/crq/*/INDEX.md` union 自愈） |
| `.claude/route-fixtures/surface-alignment.json` | adjudicator writeScope 增 `docs/crq/**`（该件在册声明「与 scope-map 对齐」，不同步即失配） |
| `docs/glossary.md` | ARQ 词条改按 CRQ 账本定义 + 新增 CRQ / TECH- / REQ- 词条 |
| `docs/ARCH-REVIEW-QUEUE.md` | 转 stub（T2 改法 5） |

**三种同名物消歧（本批必须成文，否则立刻会有人找错文件）**：

- `delivery/changes/{arch,dev}/INDEX.md`（索引**变更批次的落地状态**，lane 划分）vs `docs/crq/{gov,tech,requirement}/INDEX.md`（索引**未结决策项**，域划分）vs `delivery/workstreams/INDEX.md`（workstream 清单，hook 生成）。三者**同名不同物**，`repo-structure.md` 须各写一行说明。
- 仓内同名 `INDEX.md` **现共 8 份**（`find . -name INDEX.md` 实测，R1-F6 订正：changes 2 + workstreams 1 + crq 主 3 + crq 归档年 2；归档年份增加还会涨）⇒ 任何**指涉具体一份**的引用必须带目录（写 `docs/crq/gov/INDEX.md`），禁裸写 `INDEX.md`。**豁免（R1-F10）**：「三份 INDEX.md 是生成件」这类**泛指集合**的表述不属裸引用，V14 判据只扫「单独指涉某一份却未带目录」的形态。

目标文件：`.claude/agents/adjudicator.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/commands/loop-retro.md`（改）· `.claude/commands/arch-change.md`（改）· `.claude/commands/review-arch.md`（改）· `.claude/commands/review-governance.md`（改）· `.claude/references/arch-review-protocol.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/references/contract-change-evidence.md`（改）· `.claude/rules/contracts.md`（改）· `.claude/rules/repo-structure.md`（改）· `.claude/scope-map.json`（改）· `.claude/skills/gov-arq/SKILL.md`（改）· `.claude/skills/merge-to-main/SKILL.md`（改）· `.claude/route-fixtures/surface-alignment.json`（改）· `docs/glossary.md`（改）· `docs/ARCH-REVIEW-QUEUE.md`（改）

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 队列本体 | `docs/ARCH-REVIEW-QUEUE.md` | docs 根（非 G1 五件） | 手写 → **指路 stub**；路径不变 ⇒ 既有引用零断链（口径见 §0 表） |
| SSOT | `docs/crq/**` | 新增 docs 子目录 | 三域文件夹 + 94 条迁移（源 95 行去重）+ 5 份生成 `INDEX.md`（主 3 + 归档年 2）；须同步 `docsTopLevel.allowDirs` 与 repo-structure 白名单 |
| 合并策略 | `.gitattributes` | 新增条目 | 三份 `INDEX.md` 挂 `merge=union`（仓内两处先例） |
| 机制脚本 | `.claude/scripts/crq-ledger-shared.mjs`、`crq-render.mjs`、`crq-doctor.mjs`、`crq-migrate.mjs` | G3 | 4 件新增；均带 `--self-check` |
| 写权 | `.claude/scope-map.json` | G3 | `docsTopLevel.allowDirs` 增 `crq`；`integrator` / `adjudicator` allow 增 `docs/crq/**` |
| 规则面 | `.claude/rules/repo-structure.md`、`.claude/rules/contracts.md` | G2 | docs 语义扩张、三种（现 8 份、随归档年增长）同名物消歧、指针 |
| 角色面 | `adjudicator.md`、`review-moderator.md` | G2 | 写面（按域落文件夹）/ 读面 |
| 命令面 | `arch-change.md`、`review-arch.md`、`review-governance.md`、`loop-retro.md` | G2 | 收口核验 + 产面 |
| 参考面 | `arch-review-protocol.md`、`dev-loop-operating-system.md`、`dev-loop.md`、`architect-operating-model.md`、`contract-change-evidence.md` | G2 | 纪律与指针 |
| 技能/模板面 | `.claude/skills/gov-arq/SKILL.md`、`.claude/skills/merge-to-main/SKILL.md`、`docs/templates/crq-item.template.md` | G2 | 登记器全篇改写 + 热点清单校准 + 新模板 |
| fixture 面 | `.claude/route-fixtures/surface-alignment.json` | G3 邻接（对齐清单） | adjudicator writeScope 同步（该件声明「与 scope-map 对齐」） |
| 术语面 | `docs/glossary.md` | docs 根白名单内 | ARQ 词条重定义 + CRQ/TECH-/REQ- 新词条 |
| 锚判定式 / FROZEN 闸件 | — | **未触** | **v0.3 关键差异**：根目录在 `docs/` 下，锚提取与检测两面天然生效 ⇒ 无需改 `governance-gate-shared.mjs`（v0.2 的 T7 已删） |
| `delivery/` 结构 | — | **未触** | 账本不落 delivery ⇒ 该树顶层地图不变（v0.2 拟增第 5 顶层，v0.3 取消） |
| 契约面 | — | 未触 | 零触点（`docs/contracts/**` 不在任何 T） |
| L0 蓝图 | — | 未触 | `AGENTS.md` / `CLAUDE.md` / `DEV-GOVERNANCE.md` 零触点 |
| 门面 | — | 未触 | 不新增 / 不改任何 gate |

**【硬项 · sweep 透镜义务】**本批新增判定 pattern = T5 doctor 的**红档判据**（必填缺失 / ID 形态·唯一 / 前缀↔域（新条目）/ status·severity 出闭集 / `gateLayers` 自报 ≠ `paths[]` 机推 / 位置 ≠ f(状态) / index 漂移或缺失 / 无 frontmatter）与 **advisory 判据**（六项，见 T5）。**v0.5 更正（R1-F4）**：初版此处宣称的「`cdq` 缺」机判**未实现**——「契约类条目」无 frontmatter 判据可识别（domain=tech 不蕴含契约类），机械判定需要新字段（如 `contractGate: true`），属 schema 扩张、超出本批 ⇒ **`cdq:` 指针降为 prose 纪律**（T3 改法 3 的「必修」由 skill/template 承载，doctor 不判），留 §6 留根 17。落地后须以 doctor 红档 pattern 全仓预跑一次（命令 + 输出进 T5 证据段）。**census 类清单机械生成**（§0 的引用面三分即 `grep -rl` 逐面枚举，口径随表登记）。

**【硬项 · 复扫模式完备性先验】**本批涉「`INDEX.md` 路径文本形态」这一族：新增同名 index 后（仓内现共 **8 份**且随归档年增长，`find . -name INDEX.md` 可枚举——R1-F6 订正，初版误记 5 份），「裸写 `INDEX.md`」成为**新的歧义来源**。完备性论证 = **本批主动制造该歧义，故必须在 T6 成文「指涉具体一份的引用必须带目录」**（泛指集合的表述豁免，见 T6 消歧节）并同步扫一遍现有 `INDEX.md` 文本形态（现有 `delivery/changes/{arch,dev}/INDEX.md` 的引用写法即模板）。该扫描与替换**属本批范围**。

## 4. human 取舍点（OQ）

**OQ-1 · 账本根落点 `docs/crq/`**
背景：human 2026-09-24 已裁「放 docs 目录下、根名 `crq`」。本件确认并记其连带：账本原本就住 `docs/`（原地铺开）；`docs/` 前缀在锚判定式内 ⇒ **v0.2 的 T7 与 OQ-6 一并消失**。
代价：`docs/` 语义扩张为「正式文档 + 会增长的记录库」（须在 `repo-structure.md` 写明）；`docsTopLevel.allowDirs` 增 `crq`。
why 你定：已由你裁定；此处只确认**你是否接受 `docs/` 语义扩张**这一连带，或希望用别的方式表达（如 `docs/queues/crq/` 之类分层）。
选项：确认 `docs/crq/`（推荐）／改为 `docs/queues/` 等有语义前缀的形态／其他。

**OQ-2 · `docs/ARCH-REVIEW-QUEUE.md` 的处置**
背景：全仓 135 处引用指向它（活跃引用面 22 件，其余为历史制品/工作记忆，口径见 §0 表——R1-F5 修正后的计数供裁）；本版把它降为**简短指路 stub**（标题 + 去向表 + 登记/结项指引；路径不变、零断链、且几乎永不再改 ⇒ 从热点变冷文件）。另一路是清引用后**删掉**。
代价：(a) stub = 一个 docs 根文件长期存在（内容几乎不变，无维护成本）；(b) 清引用删除 = 干净，但要动大量文面（含 `delivery/**` 与 `tools/**` 侧引用，其中部分是 dev 侧冻结件，须逐件授权），得单开一批。
说明：`docs/archive/ARCH-REVIEW-QUEUE.closed.md`（`§0` 与 `repo-structure.md:20` 声明的结移目标）**实测从未存在**；本版已结留域内 index 薄行，归档件概念取消，故 `repo-structure.md:20` 的举例句须改。
推荐：(a) 本批落 stub + 改 `repo-structure.md:20` 举例句。
why 你定：决定「docs 根留不留一个几乎不动的指路文件」。
选项：(a) 转 stub、本批完成（推荐）／(b) 本批即清引用并删除／(c) 保留为 gov 域 index（不改 stub；但 gov 的合并热点不治，**与分域初衷冲突**）。

**OQ-3 · 契约类条目与 CDQ 的关系**
背景：你裁定 tech **含 contract**。契约变更已有专属队列 CDQ + 专属门（契约门）。本件默认取「tech 收条、`cdq:` 指针指向 CDQ、**CDQ 仍是门与批准状态的唯一权威**」以避免双账。
代价：(a) 保持 CDQ = 无侵入，但同一决定的状态分记两处（本账记「有这回事」、CDQ 记门状态），需指针纪律兜；(b) 把 CDQ 并入 tech 账 = 一份账、单源最彻底，但**动的是契约门的记录面**（`docs/CONTRACT-DECISION-QUEUE.md` 是契约流程承载物之一），影响面超出本批，且触 CDQ 多处引用。
推荐：(a)。why 你定：是否动契约门的记录面属**不可逆程度**判断——(b) 会把「整理账本」扩张成「改契约流程」。
选项：(a) 保持 CDQ + `cdq:` 指针（推荐）／(b) CDQ 并入 tech 账／(c) 先 (a)、观察一季再评估 (b)。

**OQ-4 · 三域命名与 ID 前缀（v0.4 状态更新：已按此落地，确认或改）**
背景：三域由 human 定为 **Gov / Tech / Requirement**；本件落到可机取值与路径：`docs/crq/gov|tech|requirement/` + 前缀 `ARQ-|TECH-|REQ-` + 归档 `_archive/<年>/`（沿 workstreams 先例）+ 节名「待拍板 / 待触发 / 已接受风险」。
代价/取舍：`ARQ-` 沿用（94 条不改名）但**与容器名 `crq` 脱钩**（历史词汇，留根 11）；**requirement 域现存唯一条目保留 `ARQ-` 前缀**（与祖辈规则一致，`REQ-` 自下一条新登记起用——v0.4 修正了 v0.3「单条换前缀」与 T3 改法 6 的自相矛盾）；`REQ-` 与 story ID（`S-Fx.y`）不同族不会撞；`TECH-` 自解释；index 一律 `INDEX.md`（引用必须带目录）。
why 你定：命名是你要的（「名字你帮想一下」），落定后**改名成本随文件数上升**——现在 94 件已落盘但尚未 commit，仍是低成本窗口（一次脚本重命名 + 重渲染）；过批合并后成本跳升。
选项：确认本件命名（推荐）／或指定替代（如 `ARQ-` 一并改 `GOV-`、`REQ-` 改 `RCR-`、`_archive` 改他名）。

**OQ-5 · 本批是否拆批**
背景：本批含 94 条迁移（源 95 行去重）+ 4 个脚本 + 5 份生成件（主 3 + 归档年 2）+ 19 处文面 + `.gitattributes`，是 Track B 里偏大的一批。
代价：一批做完 = 一次到位、但单批评审与核验成本高；拆批 = 每批小，但 tech / requirement 面依赖 lib 与渲染器，须等第一批落地。
推荐：**不拆**——三域共用一套机制，拆开要多付一次评审与核验的固定成本。
why 你定：批的粒度与你的评审注意力预算直接相关。
选项：(a) 一批做完（推荐）／(b) 拆两批（先 gov 面 + 机制，后 tech/requirement 迁移）／(c) 拆三批。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 命令 | 预期 | 起草期实测状态（2026-09-24） |
|---|---|---|---|
| V1 | `node .claude/scripts/crq-migrate.mjs --dry-run` | 报「源行数 → 去重后条数；ID 集合全等；**按域分布计数**；列数异常行标记」 | **已跑**：95 → 94；gov76/tech17/req1；1 条列数异常行（已处置，T1 5-2） |
| V2 | `node .claude/scripts/crq-render.mjs --self-check` | 全绿 | **已跑**：ok 8/8（v0.4 增归档归位纯函数双向夹具 + 归档指针/薄行夹具） |
| V3 | `node .claude/scripts/crq-doctor.mjs --self-check` | 全绿 | **已跑**：ok 13/13（v0.4 增 mislocated 双向夹具）；**反转实证**（摘掉一项检查→自检变红）已做 |
| V4 | `node .claude/scripts/crq-doctor.mjs`（真实仓） | **RED 零命中**；advisory 计数如实列出 | **已跑**：RED 0；advisory 134（no-paths 94 / legacy-prefix 18 / tech-no-handoff 17 / severity-off-rubric 5） |
| V5 | `node .claude/scripts/crq-render.mjs && git diff --stat docs/crq` | **diff 为空** | 须 landing 收口时跑 |
| V6 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` | `findings` 零；`anchorCount` = manifest 行数 | 已跑多次（改稿后须复跑） |
| V7 | `node .claude/scripts/route-regression-check.mjs` + `lint-descriptions.mjs` | 全绿 | 须 landing 前跑（19 处文面改完后；**v0.5 已复跑**：route-regression 28 ok，lint 仅既有 warn——adjudicator description 本轮又改，FROZEN 前须再复跑） |
| V8 | 迁移人读核：94 件抽核 | 「正文一字未改、仅容器换形 + 落对文件夹」；**两处已声明例外**：`cert-lifecycle-deadlock`（双重落位合并注记 +177）与 `schedule-grid-restore-gate`（例外条款结项段 +316，T3） | 机器侧对账已核（§0-E2 含例外口径）；判官已抽 4 条逐字核过（§7-R1）；**全量人读抽核由 landing-verifier 承接** |
| V9 | `grep -n "ARCH-REVIEW-QUEUE" .claude/scripts/crq-render.mjs .claude/scripts/crq-doctor.mjs .claude/scripts/crq-ledger-shared.mjs` | **零命中**（渲染/检查/单源库均不机读旧队列与生成件之外的面）。**v0.4 收窄声明**：一次性迁移脚本 `crq-migrate.mjs` **正当读取**历史源（`--src` 指 git 历史版本，E9），不在本断言面内 | 两期 |
| V10 | `crq-render.mjs` 连跑两次 + `diff` | **零差异** | **已跑**（自检夹具 2 覆盖；另需真仓复跑） |
| V11 | **合并实演**（两分支各加一条 → 合并） | 无人工解冲突；doctor 报「待重渲染」；重渲染归位 | **已跑，两场景**（异日期 / 同日期）——结果与残留说明见 §0-E6 |
| V12 | R6 守卫夹具：删生成标记头后跑 `crq-render.mjs` | fail-loud 拒写 | **已跑**（scratch 仓：删标记头 → `FATAL 目标不是生成件…拒写`、exit 1） |
| V13 | 视图人读核（R1/R3/R4 + v0.4 归档面） | 摘要在场、ID 可点、已结区无证据正文、已结区 = 年度指针行 | **已跑**（gov INDEX：摘要「未结 12…已结 64」+ 已结区单行指针 `_archive/2026/INDEX.md`；条目文件抽查 frontmatter/正文完整） |
| V14 | 扫「单独指涉某一份却未带目录」的 `INDEX.md` 新引用 | 无裸指涉（**泛指集合**如「三份 INDEX.md 是生成件」按 T6 豁免不计，R1-F10） | 须跑 |

**真空绿规约遵守声明**：V4/V5 在 SSOT 为空或渲染未跑时会假绿——两命令**已内建前置**（doctor/render 在条目数 0 时 FATAL exit，不判绿）；V1 dry-run 含源非空前置；V6 在空 PROPOSAL 上会假绿（`anchorCount:0` 即 `findings` 空）——故 V6 断言含 `anchorCount` 下限；V11 须先断言两分支各自渲染成功，否则「无冲突」可能是「根本没改」（**本次实演已按此断言**，且由 doctor 独立确认合并结果确含两条新行）。

## 6. 留根清单

1. **`delivery/taskpackages/**` 中 `#ARQ-<id>` 锚 5 处 / 3 件**（v0.5 按 R1-F7 订正：`260831-simulator-lvgl-align/taskpackage.json:91`、`260911-key-route-a1/taskpackage.json`、`260911-screen-unification/taskpackage.json`；`grep -rn "#ARQ-" delivery/taskpackages/` 实测 5 命中 / 3 文件，**input-brief 零命中**，原「260911-* 各 input-brief」与「3 处」均失实）——表格行本无 markdown 锚，该形态从来不可达；SSOT 化后正解是改指 `docs/crq/<域>/<ID>.md`。**本批不改**：属 dev 侧冻结件，触他人冻结件须逐件 human 授权（与 `ARQ-260908-versioned-guard-repoint-execution` 同族纪律）。
2. **doctor 是否升格为门**：本批明定为告警级 / landing-time 工具。若日后挂 Stop 钩子或进 `verify-gates.json`，即构成**新增 gate**，须单开一批走完整 review（本批不预批）。
3. **派生 index 陈化的结构性答复**：本批只做到「可被看见」。是否需在 launch 面 / 收尾相位强制重跑，留待观察一批后的实际陈化率再定。
4. **`delivery/changes/arch/INDEX.md` 的四状态与本账的关系**未收口——`ARQ-260920-changes-index-mergedinmain-stale` 那条时序问题（合并后不跑 upsert 则显旧值）本批不治。
5. **未迁格式条目的计数**：建议 doctor 输出「未补 frontmatter 的条目 N 条」，把迁移进度变成可见数字。**是否渲染进 index 头部 = 落地时任判官裁**。
6. **`docs/` 语义扩张的长期观察**：账本进 docs 后，docs 从「正式文档面」变为「文档 + 记录库」。若日后 docs 根/子目录继续膨胀，需要重新考虑 docs 的边界定义（**复议触发 = docs 子目录数再增两个或出现第二个记录库**）。
7. **归档机制已落（v0.4），残余触发只剩一个**：原「单域 300 件分片」触发已被 T2 改法 6 的自动归档取代（已结即出顶层）。**残余复议触发 = 单个 `_archive/<年>/INDEX.md` 薄行超一屏可览极限（约 300 行）或跨年归档数 >5**——届时评估归档索引再分层；未结顶层目录预计长期保持两位数，无需触发。
8. **agent 侧「新条目」辅助命令（本批不做，记账候选）**：`crq-new.mjs --id … --domain …` 这类骨架生成器能让 agent 少犯 frontmatter 形态错。先观察实际形态错率；若登记期仍反复错，再单开一批补。
9. **v0.2 的 T7 与 OQ-6 已随落点改 `docs/` 而删除**——记录留痕（文首「T7 消失记」），**不是漏做**。若日后账本部分内容再度迁出 `docs/` 树，该缺口会重新出现，须重新评估。
10. **本批已三次改向**（v0.1 平铺单 index → v0.2 三域文件夹 + `delivery/ledger/` → v0.3 `docs/crq/` + 域改名）。**若 v0.4 再改域集或目录形态，须重估「迁移脚本是否已落地」**——迁移一旦执行，改向成本从「改文档」升为「再迁一次 94 件」。
11. **`ARQ-` 前缀与容器名 `crq` 脱钩**：`ARQ` 原义 = ARCH-REVIEW-QUEUE，容器现名 `crq`，二者不再对应，属历史词汇。**本批不追改**（94 条 + 全仓引用，收益不抵成本）；若日后有别的理由做批量 ID 迁移（如编号体系重排），可一并对齐。**在册计数 = doctor 的 `legacy-prefix-off-domain` advisory（现值 18 条）**，不会静默。
12. **`260827-contract-mech-gate-gap` 的域归类（已由实跑定，不再悬）**：起草期人工点算时它在 gov 与 tech 两行均出现（「机制门缺口属治理」vs「含 contract」）。**实跑落定 = gov**（迁移脚本 `OVERRIDES` 显式登记），理由：该条治的是「pass-oracle 第 4 关没有机械门」，属**机制缺口**而非契约条文变更；契约类条目的判据是「改不改 `docs/contracts/**` 的条文」。**该判据此处一并成文**，供后续同理条目参照。
13. **队列 severity 用词与 rubric 漂移（起草期实测发现，本批不治）**：`gov-arq` skill 明文要求「severity 从五档选（`.claude/references/arch-review-rubric.md`）：BLOCKING / SHOULD / NIT / OPTIONAL / FYI」，**但源队列 29 条未结实况 = SHOULD 23 / COULD 4 / NIT 1 / MAJOR 1**——`COULD` 与 `MAJOR` **不在 rubric 内**（5 条命中，doctor `severity-off-rubric` advisory 在册）。**本批不擅自改档**（迁移不改数据），只把漂移变成可见计数；**归一（改队列用词 / 或扩 rubric 五档）须单开一批**，因为它要在「改 rubric（G2）」与「改历史条目 severity」之间做取舍。
14. **T6 清单按 census 复查扩围（16 → 19 处，v0.4）**：初版清单漏了三处**语义性**引用（非纯路径指针）——`.claude/route-fixtures/surface-alignment.json`（在册声明「与 scope-map 对齐」，adjudicator allow 已失配）、`.claude/skills/merge-to-main/SKILL.md`（「本仓热点如 `docs/ARCH-REVIEW-QUEUE.md`」已不成立——stub 是冷文件）、`docs/glossary.md`（ARQ 词条定义按旧单文件队列写）。三处已补改并入 manifest。**教训**：census 之后还要按「纯指针 vs 语义陈述」二次分类，只数文件数会漏。
15. **迁移期 `owner` 字段是 best-effort 正则提取**（`owner=…` 到首个空白/句号截断，如「architect（定方向）+」截掉了「dev-loop（落地）」后半）——**如实声明**：该字段对迁移条目仅作提示，语义权威仍在正文散文；新条目由登记人写全。不回填、不修历史（正文未动，信息无丢失）。
16. **明示豁免不改的残留引用（stub 保路径有效；v0.5 按 R1-F5 补齐、R2-G3 补回 workstreams 面）**：`.claude/agents/architect.md:50`（**self-def，改动须独立 human 授权**）、`docs/DEV-GOVERNANCE.md:118/217/263`（**G1，本批零触点承诺**）、`docs/architecture.md:365`（「~30 条」计数已陈旧）、`docs/ADR.md:16`、`.claude/references/taskpackage-authoring.md:4`（历史来源引注）、`.claude/skills/kidpal-{idf-env,nvs-ops}`、`server/deploy/**`（**6 件**：README + CARRIER + v3–v6 prompt）、`tools/asset-pipeline/tests/test_art_assets.acceptance.md`、**`delivery/workstreams/**`（5 件**：`m1-home-qa/decision-coverage.json` 含 `#ARQ-` 锚、`_archive/2026/260911-display-interaction-sovereignty/{decision-coverage.json,progress.md}` 含 `#ARQ-` 锚 2 处、`_archive/2026/kidpal-mvp/{progress.md,review.md}`——历史工作记忆，append-only 不追改）、**历史制品面**：`delivery/changes/**`（93 件，append-only 不追改）与 `delivery/taskpackages/**`（14 件，冻结件；含留根 1 的 5 处 `#ARQ-` 锚）——这些引用**指向的路径经 stub 继续可达**，语义陈化处随各自下次被动的批次顺带校准。
17. **`cdq:` 指针无机械判据（v0.5 新增，R1-F4）**：「契约类条目须带 `cdq` 指针」现为 prose 纪律（skill/template 承载）——机械判定需要「该条是否契约类」的 frontmatter 判据（如 `contractGate: true`），属 schema 扩张。**复议触发 = 首个契约类 tech 条目登记时**（届时评估加字段 + doctor advisory），或下次动 schema 的批次顺带。
18. **「同族数字复扫」升格进 PROPOSAL 模板（v0.5 新增，R2-G1 建议）**：本批连续两轮评审抓到同族残留（R1-F6 六处、R2-G1 十处），已在起草期声明⑥立为本批 FROZEN 前固定动作。**是否写进 `docs/templates/PROPOSAL.template.md` 的【起草期自检】段**（成为所有后续批次的义务）留给下一治理批裁——模板是 G2 canonical，本批 manifest 未含它、不夹带。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-24 - architect

verdict: BLOCK(F1..F10)

**评审对象**：本 PROPOSAL DRAFT v0.4 + 工作树全部起草期脚手架（脚本 4 件、`docs/crq/**` 94 条目 + 4 份 INDEX、stub、模板、`.gitattributes`、T6 的 19 处文面）。**方法**：透镜 1–7 + 本环特有焦点 1–10 全过；一切事实断言亲核（Read/Grep/Bash 实测），未凭 PROPOSAL 转述采信；计数 1 BLOCKING（F1）/ 6 SHOULD（F2–F7）/ 3 NIT（F8–F10）。**结构字段在场核对（透镜 7）**：landing-manifest 围栏块在场（26 行，`landing-fidelity-check.mjs --preflight` 复跑 `anchorCount: 26`、`findings: []`）✓；packet-id 行在场（头注第 8 行）✓。

**亲核留痕（正面核验摘要——以下声明经独立实测成立）**：

1. **声明面（焦点 8）全过**：`git diff HEAD -- AGENTS.md CLAUDE.md README.md docs/DEV-GOVERNANCE.md docs/contracts .claude/scripts/lib .claude/hooks .claude/verify-gates.json` = 空（零 G1 / 零契约 / 零 FROZEN 闸件触点）；git status 无 settings / hooks / verify-gates 变更（不新增门）。`ANCHORABLE_TEST` 与 `CANONICAL_TEST` 均含 `docs/` 前缀（T7 消失记的实体断言为真；行号微差见 F9）。
2. **doctor 真仓实跑** = `RED 零命中`、advisory 134（`no-paths=94 severity-off-rubric=5 legacy-prefix-off-domain=18 tech-no-handoff=17`）——与 E5/V4 逐字一致；`crq-render.mjs --check` exit 0（盘上 == 应渲染）。
3. **自检与判别力**：`crq-render --self-check: ok (8/8)`、`crq-doctor --self-check: ok (13/13)`（与 V2/V3/E10 一致；E4/T5 的 14 为陈旧值 → F6）；**变异测试独立复核 E4**：把 lib 的 `prefix-domain-mismatch` 由 red 降 advisory（/tmp/crq-invert 副本）→ doctor 自检 `FAIL prefix-domain` exit 1——夹具判别力成立。
4. **E1/E9 复核**：`crq-migrate.mjs --dry-run --src <git show HEAD 版>` = 95 → 94、ID 集合全等 yes、gov=76/tech=17/req=1、转义竖线 58、双重落位裁决打印、`⚠ 列数=7` 仅 1 条（portfolio-reconcile）——与 E1 一致；对 stub 源裸跑 = `FATAL 源解析出 0 行——真空输入，拒绝继续（真空绿规约）` exit 1——E9 守卫实证。
5. **归档机制（焦点 2）**：`expectedRel`/`computeRelocations`（纯函数、双向）+ `mislocated` 红档实现与夹具核过（render 夹具 6：搬入/搬回/无日期不动/已在位不动；doctor 夹具 11：mislocated 红 + 假阳正控）；盘上状态 = gov 顶层 12 未结 + INDEX + `_archive/2026/`（64 条 + INDEX）、tech 顶层 15 + INDEX + `_archive/2026/`（2 条 + INDEX）、requirement 1 + INDEX——与 E10 一致；归档年 INDEX 同受 R6 写守卫与 `index-stale` 漂移检测（`expectedIndexes` 含 `archiveIndexRel`）；`.gitattributes:29` `docs/crq/*/_archive/*/INDEX.md merge=union` 在场。
6. **升序排序（焦点 3）**：`sortForRender` = 未结按 `opened` 升序 / 已结按 `closed` 升序、同日按 ID 升序——与 R5 声明逐字吻合；渲染输出零时间依赖；R7 CRLF 归一在 writeGuarded / realIndexReader / --check 三处落实（scratch 实演在 CRLF checkout 下 doctor/render 均绿，R7 实证）。
7. **判官独立重演三件套（焦点 1）**：E6 原实演现场（/tmp/crq-reh2、/tmp/crq-reh4、/tmp/union-out.md）在场但为**旧版渲染器**产物（title 列空、节名「顺延」→ F8）；判官以**现版脚本**在 scratch 仓（/tmp/crq-judge-reh，含 union 行 .gitattributes）重演「两分支各结一条」：merge exit 0 零人工解冲突；union 残留 = 摘要计数陈旧（11/65 vs 真值 10/66）+ 双侧 undated 薄行俱在（零丢失）；doctor 当场 `RED 1 [index-stale]`；重渲染归位后 doctor exit 0。**「件 2 与件 3 必须成对」论断在起草期未实演的场景下亦成立**。
8. **抽核逐字对照（焦点 6，4 条 + 全量机械对账）**：`ARQ-260916-home-date-format-english`（requirement 首条，六 cell ↔ frontmatter 映射逐字全对）、`ARQ-260914-portfolio-reconcile-launchpreconditions`（7 列裸竖线行，锚首三列+锚末两列解析正确、正文裸 `|` 逐字保留）、`ARQ-260923-schedule-grid-restore-gate`（正文逐字 + 结项段为披露性追加）、`ARQ-260902-cert-lifecycle-deadlock`（DUAL_PLACEMENT 裁决段 + 另一处落位原文保留）；`--verify` 复跑全量：源→意图仅 cert +177（与 E2 一致）；意图→落盘 **schedule-grid +316**（E2「零差异」已陈旧 → F6⑤）。E8 修复在 migrate/lib 面属实（title 入 REQUIRED_FIELDS/FM_ORDER、INDEX title 列全实）——但**登记面未扫净 → F1**。
9. **T6 文面（焦点 7 部分）**：19 处逐一 diff 核过，改动性质与表内声明一致；HEAD 行号引用全部命中（adjudicator:77 / review-moderator:61 / arch-change:42 / review-arch:31 / review-governance:33 / protocol:170 / dev-loop-os:158,173 / dev-loop:338（「约定非机检，列结构为簿记纪律」verbatim）/ architect-operating-model:29 / contract-change-evidence:17 / rules-contracts:15 / merge-to-main:18 / 留根16 的 architect.md:50、DEV-GOVERNANCE:118/217/263、architecture.md:365、ADR.md:16）；scope-map（docsTopLevel.allowDirs + crq；integrator/adjudicator allow + docs/crq/**）与 surface-alignment（adjudicator 同步 + lastReviewed/rationale 更新）一致，`route-regression-check.mjs` 28 ok 0 fail。
10. **V6/V7/V9 复跑**：V6 preflight `anchorCount: 26`、`findings: []`；V7 route-regression 全绿、lint-descriptions 仅既有 warn（landing-verifier.md 363>320，非本批文件）；V9 grep render/doctor/lib 零命中 exit 1（migrate 收窄声明诚实——其默认 SRC 正当读取历史源）。
11. **§0 实测表复核**：124 484 字节 = HEAD blob 124 336（LF）+ 148 行 CR（CRLF 工作树下题给命令可复现，精确吻合）；148 行 / 95 次 commit / 单行最大 6 495 与次长 5 915 / §1 分布 1.1=1、1.2=19、1.3=9、§2=66 / `docs/archive/ARCH-REVIEW-QUEUE.closed.md` 不存在（archive 下仅 spec.md）——均与实测一致。**唯「被仓内文件引用 52 个」不可按题给命令复现 → F5**。

**Findings（逐条：严重度 / 证据 / 修法方向）**：

**F1（BLOCKING）· 登记双面漏必填字段 `title`——E8 同型第二处**。证据：① `docs/templates/crq-item.template.md` 骨架 frontmatter 逐字 = `id/domain/status/severity/owner/trigger/opened/closed/closedBy/paths/source`——**无 `title` 行**；② `.claude/skills/gov-arq/SKILL.md` 步骤 6 verbatim「**frontmatter 必填**：`id` / `domain` / `status` / `severity` / `opened`（已结项豁免 `severity`）；建议填 `owner` / `trigger` / `paths` / `source`」——无 title；③ `crq-ledger-shared.mjs:51` verbatim `export const REQUIRED_FIELDS = ['id', 'title', 'domain', 'status', 'severity', 'opened'];`。后果：照模板/skill 登记的新条目必中 doctor 红 `missing-field`；doctor 是告警级非门，漏跑即 INDEX title 列空——正是 §0-E8 声称「已修」的缺陷类在**登记面**重演（E8 只修了 migrate/lib 面）。修法：模板骨架补 `title: <人读标题一句话>` 行、skill 步骤 6 必填清单补 title；E8 行的「已修」声明补注覆盖面。

**F2（SHOULD）· `handoff` 空的档位自相矛盾（T3 改法 2 ↔ T5 advisory 项 6 ↔ 实现）**。证据：T3 改法 2 verbatim「`domain: tech` 且 `handoff` 为空 → doctor 报红（T5）」；T5 advisory 项 6 verbatim「`domain: tech` 且 `handoff` 空（现值 17——迁移项未填去向）」；实现 `crq-ledger-shared.mjs:243` = `advisory.push`（code `tech-no-handoff`）；真仓实跑 advisory 含 tech-no-handoff=17、RED 0。红/advisory 分档是机械可判行为定义，frozen 文本两处互斥 ⇒ landing-verifier 无法判定冻结行为是哪个。修法：裁断归一——取 advisory（T5 引语已论证「一律判红则 V4 零红不可达」，推荐）则改 T3 改法 2 措辞；若要「迁移项 advisory、新条目红」则给可机判据（如 opened ≥ 落地日）并同步实现。

**F3（SHOULD）· 「T3 例外条款」悬空引用 + 「29 条未结」口径未对账**。证据：E10 verbatim「按 T3 例外条款置 closed 后归位」、条目结项段 verbatim「迁移期按 PROPOSAL T3 例外条款归位」——**T3 全文无该条款**（grep「例外」仅命中 T3 改法 6 两段式等处）；源行（HEAD 队列 :26 处理时机格）verbatim「→ **2026-09-23 已结**：human 裁 accept-as-risk…」——翻状态本身忠于源、条目结项段已记裁决理由与 watchlist 语义（并入 trigger 留痕），处置正当但**规范性锚点不存在**；连带：header 行 6「队列内 29 条未结项的内容语义不因本批改写而改变」、§0 非目标行 94、T3 表合计「未结 29」——迁移后实测未结 **28**（三份 INDEX 摘要 12+15+1），E10 已披露 13→12/63→64 但上述三处未对账。修法：T1 改法 5 增显式条款（如 5-4「源行处理时机格自证已结 → 迁移置 closed、正文结项段记裁决、watchlist 语义并入 trigger 原文留痕」），E10 与条目结项段引用改指该条款；「29（源口径）→ 28（迁移后）」加一句对账。

**F4（SHOULD）· schema / 检查枚举与实现三处漂移**。证据：① T1 改法 2 字段闭集清单无 `title`（lib REQUIRED_FIELDS 有——F1 同根），且 doctor 消费的可选自报字段 `gateLayers`/`hardExclusions` 未列入（`crq-doctor.mjs:28-35` 读 `fm.gateLayers`/`fm.hardExclusions`；gov-arq skill 步骤 4 已写「若要自报 G 层（可选字段 `gateLayers`），必须与机推一致，不一致 doctor 报红」）；② T5 红/advisory 枚举缺两个已实现 advisory code：`closed-no-date`（lib:245）、`hard-exclusion-undeclared`（doctor:35）；③ §3 sweep 硬项 verbatim「本批新增判定 pattern = T5 doctor 的五条判据（`handoff` 空、**`cdq` 缺**、`paths[]` 推导不一致、渲染漂移、前缀↔域 不一致）」——**「cdq 缺」检查零实现**（grep `cdq` 全部脚本仅命中 lib:15 域名表与 lib:115 FM_ORDER 字段名，无任何检查逻辑）。修法：T1 改法 2 补 title 与可选字段声明；T5 枚举补两 advisory code；§3 判据清单与 T5 对齐（删「cdq 缺」，或在 T5 补检查——契约类可由 `paths` 含 `docs/contracts/**` 机判）。

**F5（SHOULD）· census 数字面不可复现 + 豁免分类不完备（焦点 7 反照结果）**。证据：① §0 表 verbatim「被仓内文件引用 | 52 | `grep -rl ARCH-REVIEW-QUEUE .`」——判官实测同命令工作树 = **135**、`git grep -l HEAD` = **144**；52 仅在 `git grep -l "ARCH-REVIEW-QUEUE" HEAD -- ':!delivery/changes'` 下复现（144−92=52），排除口径未登记；② T6「其余引用仅提及路径…豁免清单见 §6 留根 16」——留根 16 **未覆盖** `delivery/changes/**`（92 件）与 `delivery/taskpackages/**`（14 件，留根 1 只点 3 处锚），并漏 `.claude/references/taskpackage-authoring.md:4`（verbatim「来源：ARQ-260813-taskpackage-friction-fields-template（`docs/ARCH-REVIEW-QUEUE.md`，5 包独立实证）」——语义属「stub 保路径仍有效」的 provenance 指针，但既不在 T6 的 19 处也不在留根 16，恰是留根 14 教训「二次分类漏网」要防的形态）；③ stub 正文 verbatim「（52 处指向本路径）」——计数将固化进长期文件；④ 留根 16 verbatim「`server/deploy/**`（5 件）」——实测 **6** 件（OPENCLAW_SYSTEM_PROMPT_CARRIER / README.md / openclaw-system-prompt.v3/v4/v5/v6）。.claude 下 21 处 census ✓ 与实测一致。修法：重跑 census 按全量登记并声明排除口径；留根 16 补 delivery/changes、delivery/taskpackages 两条分类行与 taskpackage-authoring.md；stub 计数改准或去数字化；server/deploy 件数改 6。**连带**：OQ-2 的论据数字（「52 处引用」）应以修正后计数供 human 裁——方向不变（引用越多 stub 论据越强），数字必须真。

**F6（SHOULD）· v0.3 残留数字/映射未随 v0.4 复扫（六处，违反本文档硬项「凡数字引用 → 起草期现跑实测」）**。证据：① manifest 注 verbatim「预期**全 23 项入集**」——preflight 实测 `anchorCount: 26`（= manifest 26 行）；② E3「ok 5/5」vs 现版实测 8/8；③ E4「ok **14/14**」与 T5 改法 10「**夹具 14 条**」vs 实测 13/13（doctor selfCheck 打印 verbatim `ok (13/13)`）；④ T2 改法 2 verbatim「`deferred` → 顺延」vs R2/lib `STATUS_SECTIONS`「待触发」（lib:46，渲染 INDEX 节名实测「待触发」）；⑤ E2/V8「逐条对账（意图 → 落盘）零差异」vs `--verify` 复跑实测「ARQ-260923-schedule-grid-restore-gate: +316」（结项段追加，可解释但未回写）；⑥ T6 verbatim「仓内共有 **5 个 `INDEX.md`**」与 repo-structure 落地文面「仓内共 5 份同名 `INDEX.md`」——`find` 实测 **8** 份（delivery/changes/{arch,dev}、**delivery/workstreams/INDEX.md**（起草期 census 亦漏）、docs/crq×3、本批自产 `_archive/2026`×2），且随归档年增长——落地当天即假。修法：六处统一复跑更正；INDEX 同名计数去数字化（「多份」或「≥8 且随归档年增长」），消歧表把 `delivery/workstreams/INDEX.md` 补进计数或加行。

**F7（SHOULD）· 留根 1 引据三处失实**。证据：留根 1 verbatim「**`docs/taskpackages/**` 中 3 处 `#ARQ-<id>` 锚**（`260831-simulator-lvgl-align/taskpackage.json:91`、`260911-*` 各 input-brief）」——判官实测：`docs/taskpackages` 不存在（`ls` 报 No such file or directory，正确根 = `delivery/taskpackages/`）；`#ARQ-` 锚实测 **5 处 / 3 文件**（260831-simulator-lvgl-align/taskpackage.json:91 ✓、260911-key-route-a1/taskpackage.json:58,90、260911-screen-unification/taskpackage.json:66,110）；**input-brief 中 `#ARQ-` 零命中**（input-brief 只有无锚路径引用）；workstreams 侧另有 `#ARQ-` 3 处 / 2 文件（留根 16 对 workstreams 的「含 3 处 `#ARQ-` 锚」为真）。修法：按实测位置重写留根 1（根改 `delivery/`、列全 5 处 3 文件、删 input-brief 归因）。

**F8（NIT）· E6 证据版本未声明 + 措辞精度**。证据：/tmp/union-out.md 在场但为旧版渲染器产物（title 列空、节名「顺延」、头注措辞「从本目录条目文件生成」/「仓内共有 5 份同名 INDEX.md」均异于现版）——E6 实演早于 v0.4 渲染器变更，场景面不含 v0.4 新增共享计数行（双侧结项）；判官已用现版重演补齐（亲核留痕 7，机制结论成立）。另 E6 verbatim「union 只取了一侧的 14/11」实况 = 两侧各自渲染出的摘要行**相同**（各加 1 条 → 同为 14/11）⇒ 合并平凡取该侧，非「丢弃另一侧」。修法：V11/E6 注明实演所用渲染器版本与判官复核，或 landing 收口附现版重演一次；E6 措辞精确化。

**F9（NIT）· 脚本注释与引文行号小差（四处）**。证据：① `crq-migrate.mjs:6` 头注 verbatim「保真断言：95 行 → 95 文件」vs 实现与输出（95 → 去重 94）；② `crq-migrate.mjs:24` 注「未结 29 条…落成显式表」——OVERRIDES 实为 28 条（schedule-grid 走 `[default]`，dry-run 输出实证），且 :57 `'ARQ-260916-server-mvp-teeth-sweep': 'tech'` 错位在「// gov」注释块下；③ T7 消失记引「governance-gate-shared.mjs:38」——`ANCHORABLE_TEST` 实测在 :40（内容断言为真）；④ §0 表「冲突·union·归位相关 4」——`git log --oneline --follow | grep -ciE "冲突|union|归位"` 实测 6，且「归位」与相邻类别「结移·簿记·归位·清理 13」交叉，分类口径未声明。修法：四处照实对齐（均注释/文档级，不影响行为）。

**F10（NIT）· 跨面一致性三小项**。证据：① repo-structure `docs/crq/` 行「谁写」列 verbatim「主线程（登记）· adjudicator（review 收口）」——漏 integrator（scope-map 实测 integrator 与 adjudicator 均获 `docs/crq/**`，本件 T6 scope-map 行亦自证「`integrator` / `adjudicator` 增 `docs/crq/**`」）；② repo-structure docs 根白名单行沿用旧文 verbatim「`glossary.md`（占位登记，文件尚未创建）」——glossary.md 已存在且本批改动它（触到该行却保留假括注）；③ skill:58 / stub:19 / template:5 的「三份 `INDEX.md`」类指会被 V14 裸写扫描的朴素 grep 命中。修法：① 谁写列补 integrator；② 删「文件尚未创建」括注；③ V14 判据加「类描述豁免」注或文面改「三份域 INDEX.md」。

**升级 human 项**：无「影响目标效果」级 finding——设计三大支柱（三域写者线/裁定线划分、归档 = 位置纯函数、止冲突三件套）经判官独立实测与重演均成立，F1–F7 为文本/制品级修复，Track B 内可收敛。**注**：OQ-2（stub 处置）宜以 F5 修正后的 census 计数供 human 裁（推荐方向不变）。

**轮次声明**：R1 全面审（透镜 1–7 + 特有焦点 1–10 全过）；R2 请按轮次纪律聚焦上述 findings 修订到位性 + 修订是否引入新矛盾。

### R2 - 2026-09-24 - architect

verdict: BLOCK(G1..G4)

**评审对象**：DRAFT v0.5（版本沿革行含 F1–F10 逐项映射）。**方法**：按轮次纪律聚焦 R1 findings 修订到位性 + 修订是否引入新矛盾；所有「已修」声明亲核（Read/Grep/Bash 实测），机械面全套复跑。计数：0 BLOCKING / 3 SHOULD（G1–G3）/ 1 NIT（G4）。**结构字段复核**：landing-manifest 围栏块（26 行）与 packet-id 行仍在场；preflight 复跑 `anchorCount: 26`、`findings: []` ✓。R1 记录完整未被改动（append-only 保全，本轮记录续接其后）。

**R1 findings 修订到位性逐条判定（亲核证据）**：

| R1 | 判定 | 亲核证据 |
|---|---|---|
| F1 (BLOCKING) | **到位** | 模板 :24 verbatim「`title: <一句话人读标题——INDEX 表格 title 列渲染它，缺了即 doctor 红（missing-field）>`」；skill 步骤 6（:52）必填清单含 `title` + 「与 lib `REQUIRED_FIELDS` 同集，改一处必改另一处」；lib :51 REQUIRED_FIELDS 含 title 未变——三面一致 |
| F2 | **到位** | T3 改法 2（:252）verbatim「→ doctor **advisory**（`tech-no-handoff`…v0.5 按 R1-F2 归一：T5 两档划分为准，本条不再是「红」）」——与 T5 项 6、lib:243 一致 |
| F3 | **到位** | T3 新增「例外条款」段（:266）：唯一状态改写声明 + 计数对账（源 §1 29 → 未结 28 + 已结 66）+ blanket 口径句；承载 ARQ 行（:6）改「28 条零改动 + 1 条例外」；T3 表注（:245）标「迁移时点态 vs 现值」；E10/条目结项段的「T3 例外条款」引用现有落点 |
| F4 | **部分到位** | T1 改法 2（:150-154）四档字段与 lib 全对齐（必填含 title；gateLayers 红/hardExclusions advisory 描述与 doctor 实现精确一致）✓；T5 advisory 补项 10/11（现值 0/0——真仓 advisory 134 实测不含二者，现值准确）✓；§3 sweep 更正 + 留根 17 ✓。**残留 = T5 红档 3 未同步 → G2** |
| F5 | **部分到位** | §0 行 62 三分口径**逐桶复现**：判官实测 total=135 = 活跃 22 + changes 93 + taskpackages 14 + workstreams 5 + docs/crq 1（唯一命中 = `docs/crq/gov/_archive/2026/ARQ-260915-changes-index-lane-split.md`，即「账本本体 1」）；活跃 22 件清单含 stub 与 crq-migrate.mjs ✓；stub 已去数字化（:3 改 grep 可枚举表述）✓；OQ-2（:400）✓；留根 16 补 taskpackage-authoring.md:4 / server/deploy 6 件 / changes 93 / taskpackages 14 ✓。**残留 = 留根 16 丢 workstreams → G3；T6 证据行「52 文件 census」未扫 → G1** |
| F6 | **部分到位** | :45「anchorCount:26 == manifest 26 行」（复跑实证）✓；E3 8/8 ✓；E4 13/13 + T5 项 12「夹具 13 组」✓；T2 改法 2「待触发」✓；E2 补 +316 第二处已声明语义差 ✓；T6 消歧节 8 份 + workstreams 行 ✓；复扫先验 8 份 ✓；repo-structure 落地文面同步（8 份 + 三行表）✓。**残留 = 8+ 处旧数字未随扫 → G1** |
| F7 | **到位** | 留根 1（:449）重写为 `delivery/taskpackages/**`、5 处/3 件逐件点名、input-brief 零命中如实记——与判官 `git grep "#ARQ-" HEAD` 实测一致 |
| F8 | **到位** | E6（:84）精确表述（「两分支的摘要行各自相同…平凡取一，并集真值比任一侧多 1（14/14 → 真值 15）」）+ 证据版本注（v0.3 期渲染器 + R1 判官现版重演引用）✓ |
| F9 | **部分到位** | migrate 头注「95 行 → 去重后 94 条 → 94 文件」+ --src/--verify 用法 ✓；T7 消失记 `:40` 订正 ✓；§0 行 61 双口径**双双复现**（判官实测：窄 pattern `冲突\|conflict\|union\|rebase` = 4、含「归位」= 8，词表即口径成立）✓；shim-401 显式化后域分布不变（gov=76/tech=17/req=1 实测）✓。**残留 = OVERRIDES 注计数与自身数据不符 → G4** |
| F10 | **到位** | repo-structure crq 行谁写补「integrator（scope-map 已授写槽）」✓；glossary 括注改「（术语表，已建）」✓；V14（:443）判据改「单独指涉某一份却未带目录」+ 泛指豁免（T6 :359 同步成文）✓ |

**机械面复跑（判官亲自执行，非采信主线程转述）**：preflight `anchorCount:26`/`findings:[]`；`crq-render --self-check` ok (8/8)；`crq-doctor --self-check` ok (13/13)；doctor 真仓 `RED 零命中`/advisory 134（no-paths=94 severity-off-rubric=5 legacy-prefix-off-domain=18 tech-no-handoff=17）；`crq-render --check` exit 0（无漂移）；route-regression 28 ok 0 fail；lint-descriptions 仅既有 warn（landing-verifier.md 363>320，非本批文件）；migrate `--dry-run --src` 域分布不变。与主线程声明逐项一致。

**R2 findings（修订残留 + 修订引入的新缺口）**：

**G1（SHOULD）· v0.5 复扫仍不全——旧数字/旧计数/旧节名 10 处残留**。逐处（file:line，本件行号）：① :202 R1 摘要要求示例「未结 N（待拍板 a / **顺延** b / 已接受风险 c）」——渲染实物为「待触发」（gov INDEX :10 verbatim）；② :324 T6 标题「+ **两处**同名物消歧」vs :358 正文「**三者**同名不同物」；③ :328 T6 证据行「§0 的 **52 文件** census 中，`.claude` 下 **21 处**」——§0 已改 135 三分口径，52 无对应物；21 为 HEAD 口径，worktree 活跃面 `.claude` 实测 = 10；④ :348 T6 表 repo-structure 行「**两处同名物消歧**」；⑤ :356 消歧节标题「**两处**同名物消歧」vs 次行「三者」；⑥ :374 §3 规则面行「两处（**实为五份**）同名物消歧」——已是三者/8 份；⑦ :420 OQ-5「**16 处文面**」「**3 份 index**」——T6 已 19 处、生成件实为 3 主 + 2 归档年；⑧ :436 V7「（**16 处文面**改完后）」；⑨ **落地文面** `repo-structure.md:39` 消歧节标题「**两处**同名物消歧（…现共 8 份…）」vs 其表 3 行；⑩ `repo-structure.md:51`「首日 94 条目 + **3 份生成 index**」（实 5 份）。修法：一次全文 grep 复扫（`52|16 处|两处同名|五份|3 份|顺延`），标题类建议去计数（「同名 INDEX.md 消歧（三者，份数 find 可枚举）」）。**注**：这是连续第二轮同族（数字复扫）残留——建议主线程把「改一处数字 → 全文 grep 同族数字」作为本件 FROZEN 前的固定动作。

**G2（SHOULD）· T5 红档 3 的硬排除档位与三处互斥（F2 同族残留）**。证据：T5 红档 3（:304）verbatim「**由 `paths[]` 推导 G 层与硬排除结论**，与条目自报值不一致即红」——但实现 `crq-doctor.mjs:32-35` 仅 `gate-layer-mismatch` 为红，硬排除未申报 = **advisory**（`hard-exclusion-undeclared`）；且 T1 改法 2（:154）与 T5 advisory 项 11（:315）均已写明 advisory。同一冻结文本内红/advisory 两说 ⇒ 机械行为定义歧义（与 R1-F2 同判据）。修法：红档 3 拆两半句——「`gateLayers` 自报 ≠ `paths[]` 机推 ⇒ 红；`hardExclusions` 命中未标 ⇒ advisory（见项 11）」。

**G3（SHOULD）· 留根 16 在 v0.5 重写中丢失 `delivery/workstreams/**` 覆盖（修订引入的新缺口）**。证据：v0.4 留根 16 含「`delivery/workstreams/**`（含 3 处 `#ARQ-` 锚，另见留根 1）」；v0.5（:464）补入 changes/taskpackages 后 **workstreams 消失**——而 §0 行 62 三分口径把 workstreams **5** 单列一桶，判官实测 5 件（含 `decision-coverage.json` 的 `#ARQ-` 锚 3 处：`_archive/2026/260911-display-interaction-sovereignty` :81,:88 + `m1-home-qa` :74），豁免清单不覆盖 ⇒ census 分类再次不完备（F5② 同族）。修法：留根 16 补回 workstreams 行（5 件，含 3 处 `#ARQ-` 锚——该锚与留根 1 的 taskpackages 锚同族，「表格行本无 markdown 锚」论述同样适用）。

**G4（NIT）· migrate OVERRIDES 注释与自身数据不符（F9 修订残留）**。证据：`crq-migrate.mjs:25-26` 注 verbatim「已结条目：默认走关键词规则（TECH_SIGNALS ≥2 命中 → tech，否则 gov），**另 2 条显式**（见末尾注）」——OVERRIDES 实含**已结 id 10 个**（gov 块 8 条已结：unit-gate-archive-recursion / supervisor-auth-fp-author / acc-gate-archive-skew / path-form-family-reference / lfc-citation-docs-prefix / lfc-citation-references-prefix / lfc-anchorable-delivery-gap / host-werror-comment-acceptance-gap，+ 末尾块 2 条 tech）；判官按 dry-run 输出 × 归档成员身份分桶实测：已结 = **override 10 / kw 0 / default 56**——「默认走关键词规则」对现行已结数据**零触发**（规则在、无命中）。未结侧注释准确（19 显式 + 10 default，实测吻合：open override 18 + cert 走 dual 裁决 + default 9 + schedule-grid = 29 源口径）。分类**结果**已核无误（域分布不变、shim-401/server-mvp 均 `[override]→tech` 与落盘树一致），仅注释失实。修法：注改「已结另 10 条显式（gov 块 8 + tech 末尾块 2）；关键词规则对现行已结数据零命中，保留作复跑兜底」。

**升级 human 项**：无——G1–G4 均为文本/注释级修订，设计面（三域、归档纯函数、三件套、schema、census 口径）本轮零新问题；R1 十大 findings 中 7 项判定到位、3 项部分到位（残留即 G1–G4 所列）。**熔断提示**：本轮为 R2；G1–G4 修订后 R3 可快速收口（四处均为机械性文字修订，无需再动机制面）。

### R3 - 2026-09-24 - architect

verdict: GREEN

**评审对象**：DRAFT v0.5（G1–G4 修订后终态）。**方法**：按轮次纪律只核 R2 findings 修订到位性 + 修订是否引入新矛盾；全部「已修」声明亲核，机械面全套复跑。结果：**G1–G4 四项全部到位，零新矛盾**；余 1 处 NIT 级措辞观察（R3-N1，不阻冻结，可随 landing 顺带修）。**结构字段复核（GREEN 前置）**：landing-manifest 围栏块在场（26 行）+ packet-id 行在场（:8）；preflight 复跑 `anchorCount: 26`、`findings: []` ✓。

**G1–G4 修订到位性（亲核证据）**：

- **G1 ✓ 到位（10 处 + 落地文面 2 处全改）**：:202 R1 摘要示例改「待触发 b（归档索引见下）」（与 gov INDEX :10 渲染实物逐字吻合）；T6 标题 :324 / T6 表 :348 / 消歧节 :356 均改「三种同名物」；T6 证据行 :328 按修订后终测重写——判官独立复跑 census **逐桶吻合**：total=135 = 活跃 22 + changes 93 + taskpackages 14 + workstreams 5 + docs/crq 1；活跃 22 的分桶（.claude 10 = 5 入清单 + 4 豁免 + 1 机制自身；docs 5 = 2 入清单 + 3 豁免；server 6 + tools 1 豁免）与判官 R2 实测清单逐件一致；§3 SSOT 行 :370「94 条迁移（源 95 行去重）+ 5 份生成 INDEX.md（主 3 + 归档年 2）」、规则面行 :374「三种（现 8 份、随归档年增长）」；OQ-5 :420「5 份生成件 + 19 处文面」；V7 :436「19 处 + v0.5 已复跑」；T2 件2 :188「`.gitattributes` **两行 glob**（`docs/crq/*/INDEX.md` + `docs/crq/*/_archive/*/INDEX.md`）」——与 .gitattributes :28-29 实物一致（v0.4 旧文「新增三行」之误一并修掉，主线程自查补捉）。落地文面：`repo-structure.md:39`「三种同名物消歧（…现共 8 份且随归档年增长…）」、:51「5 份生成 index〔主 3 + 归档年 2，随归档年增长〕」✓。**残留「顺延」核**：主线程区仅 :5（v0.4 要点引 human 原话「顺延代表啥」）、:84（E6 证据版本注述旧节名）、:203（R2 节名改名说明）三处，均属合法历史引述；其余命中在 §7 评审记录（append-only 历史，不在复扫面）。复扫纪律已固化（起草期声明⑥ + 留根 18「升格进模板留下批裁，本批 manifest 未含模板、不夹带」——边界处理正确）。
- **G2 ✓ 到位**：T5 红档 3（:304）已拆两半句——「自报 `gateLayers` 与机推不一致 ⇒ 红（`gate-layer-mismatch`）；`paths[]` 命中硬排除面而未标 `hardExclusions: true` ⇒ advisory（`hard-exclusion-undeclared`，见项 11）」+ 档位理由（「矛盾红 / 欠账 advisory」）——与 `crq-doctor.mjs:24-36` 实现、T1 改法 2（:154）、T5 项 11（:315）四处一致，互斥消除。
- **G3 ✓ 到位**：留根 16 补回 `delivery/workstreams/**`（5 件逐件点名：`m1-home-qa/decision-coverage.json` 含 `#ARQ-` 锚、`_archive/2026/260911-display-interaction-sovereignty/{decision-coverage.json,progress.md}` 含锚 2 处、`_archive/2026/kidpal-mvp/{progress.md,review.md}`，标「历史工作记忆 append-only 不追改」）——与判官实测 5 件 / `#ARQ-` 锚 3 处（m1-home-qa :74 + 260911 decision-coverage :81,:88）吻合；census 五桶自此全部有豁免分类归属。
- **G4 ✓ 到位**：migrate OVERRIDES 注（:25-28）重写为「未结 29：19 显式 + 10 default；已结 66 行（去重后 65）：显式 10（gov 块 8 + tech 末尾块 2）；TECH_SIGNALS 对现行源数据零触发（kw×0），保留为源变化时兜底」——判官复跑 dry-run 实测 why 计数 **default 65 / override 28 / dual 1 / kw 0**（= 94；override 28 = 29 显式 − cert 走 dual 打印），域分布不变 gov=76/tech=17/req=1，ID 集合全等 yes——注释与数据完全一致。

**v0.5 自查补捉两处（主线程自报，判官亲核属实）**：① `adjudicator.md` description(:3)/:39/:50 三处已改写为 CRQ 账本口径（description 保留「原 ARCH-REVIEW-QUEUE」历史陈述 + 「queue 条目」路由信号；:50 写明 stub「不承接写入」）——与 scope-map allow（`docs/crq/**` + stub 路径保留）跨面一致 ✓；② description 丢「queue」信号被 `route-regression-check` 当场抓红后修复——判官复跑 28 ok 0 fail + `--self-check` falsifiable 绿 ✓。该事件已如实记入起草期声明⑤（缺陷计数 7 处，枚举与实际吻合），且是「机械闸在本批自身上抓到真回归」的实证，留痕恰当。

**机械面复跑（判官亲自执行）**：preflight `anchorCount:26`/`findings:[]`；route-regression 28 ok + self-check falsifiable 绿；lint-descriptions 仅既有 warn（landing-verifier.md 363>320，非本批文件）；render `--self-check` 8/8；doctor `--self-check` 13/13；doctor 真仓 RED 0 / advisory 134（no-paths=94 / severity-off-rubric=5 / legacy-prefix-off-domain=18 / tech-no-handoff=17）；render `--check` exit 0；migrate dry-run 分布不变。与主线程声明逐项一致。

**R3-N1（NIT，不阻冻结，可随 landing 顺带修）**：T2 改法 5（:191）与 OQ-2（:400）称 stub 为「三行（指路 stub）」——实际 stub 成品为标题 + 去向表 + 登记/结项说明约 19 行；「三行」系早期设想的残留措辞，实物形态更好，但数字与实物不符（同族数字纪律下如实记一笔）。修法：改「简短指路 stub（去向表 + 一句话登记/结项指引）」或按实物描述。

**GREEN 结论与后续**：无未结 BLOCKING/SHOULD ⇒ 本件评审面可冻结。FROZEN 仍须按 Status 行凭据齐备：human 立项 ack + 本 GREEN + human 裁 OQ 1–5（OQ-2/OQ-4 的判断数字已按 R1-F5/R2-G1 修正到位）；FROZEN 后 packet-bound landing + landing-verifier 逐文件核验（V5/V8 全量人读抽核、V14 按新判据跑）+ retro。留根 18（复扫纪律升格模板）与留根 17（cdq 机判）已妥善记账，不属本批。