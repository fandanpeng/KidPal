# architect-landing-summary · 260923-gov-review-arq-ssot-restructure

packet-id：FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集 · 评审轮次 R1(BLOCK F1..F10) → R2(BLOCK G1..G4) → **R3(GREEN)** · change-id 260923-gov-review-arq-ssot-restructure

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 条目一条一文件 + 每域一文件夹 + 一次性迁移 | `docs/crq/{gov,tech,requirement}/**`（94 条目 + 5 份生成 INDEX）· `.claude/scripts/crq-migrate.mjs` · `docs/templates/crq-item.template.md` | 源 95 行 → 去重 94 条落盘（gov 76 / tech 17 / requirement 1）；双向逐条对账通过（两处已声明语义差见 §3）；模板含域表/骨架/三纪律 |
| T2 每域自持 INDEX（生成件）+ 止冲突三件套 + 归档 | `.claude/scripts/crq-render.mjs` · `.claude/scripts/crq-ledger-shared.mjs` · `.gitattributes` · `docs/ARCH-REVIEW-QUEUE.md`（stub） · `.claude/rules/repo-structure.md` | 渲染确定性（无时间依赖、升序、CRLF 归一）；union 两行 glob（主+归档，`git check-attr` 实测 5 份 INDEX 全部 `merge: union` 生效）；归档自动归位实跑（gov 顶层 76→12+INDEX，**64 条**入 `_archive/2026/`〔63 迁移已结 + 1 例外条款 schedule-grid〕；tech 2 条；**verdict 后更正**：原记 63，verifier ADVISORY-3 点名，E10 口径）；stub 落盘、路径不变 |
| T3 三域定义 + 去向纪律 + paths 机推 | `docs/crq/**` frontmatter · `.claude/scripts/crq-doctor.mjs` · `.claude/skills/gov-arq/SKILL.md` · `.claude/references/dev-loop-operating-system.md` | 域闭集三值落地；祖辈前缀两段式规则；6 布尔 → `paths[]` 机推（gateLayers 自报≠机推 ⇒ 红） |
| T4 requirement 域 | `docs/crq/requirement/ARQ-260916-home-date-format-english.md` · `crq-render.mjs` | 首条迁移落位（保留 ARQ- 前缀，FROZEN T4 改法 5）；`affects/decision/landedIn` 域内字段入 schema |
| T5 doctor 两档机检 | `.claude/scripts/crq-doctor.mjs` · `.claude/scripts/crq-ledger-shared.mjs` | 红档 8 类 + advisory 6 类；`--self-check` 13/13 带反转实证 |
| T6 消费方文面 19 处 + 三种同名物消歧 | `adjudicator.md`（description/:39/:50/:77 四处）· `review-moderator.md` · `loop-retro.md` · `arch-change.md` · `review-arch.md` · `review-governance.md` · `arch-review-protocol.md` · `dev-loop-operating-system.md`（两处）· `dev-loop.md:338` · `architect-operating-model.md`（:29/:40）· `contract-change-evidence.md` · `rules/contracts.md` · `repo-structure.md` · `scope-map.json` · `gov-arq/SKILL.md`（全篇）· `merge-to-main/SKILL.md:18` · `route-fixtures/surface-alignment.json` · `docs/glossary.md` · `docs/ARCH-REVIEW-QUEUE.md` | 全部改写为 CRQ 口径；repo-structure 增三种同名物消歧表 + docs 语义扩张说明；scope-map 增 `docs/crq` allowDir + integrator/adjudicator 写槽 |

## 2. 验证证据

- **V1** `crq-migrate.mjs --dry-run --src /tmp/orig-queue.md` → 「源 95 行（去重后 94 条）→ 94 文件；ID 集合全等：yes；域分布 gov=76 tech=17 requirement=1；why 计数 default 65 / override 28 / dual 1 / kw 0」——绿（判官 R2/R3 独立复跑吻合）。
- **V2** `crq-render.mjs --self-check` → `ok (8/8)`——绿。
- **V3** `crq-doctor.mjs --self-check` → `ok (13/13)`；反转实证（摘 `prefix-domain-mismatch` → 自检变红）已做——绿。
- **V4** `crq-doctor.mjs`（真仓）→ `RED 零命中 —— ok`；advisory 134（no-paths 94 / severity-off-rubric 5 / legacy-prefix-off-domain 18 / tech-no-handoff 17）——绿（advisory 全为在册迁移欠账，见 §3）。
- **V5** `crq-render.mjs && git diff --stat docs/crq docs/ARCH-REVIEW-QUEUE.md` → `docs/crq` 零 diff；stub 的 diff（-141/+14）= 本批 T2 落地改动本身（预期）。**口径注**：`docs/crq/**` 尚未 commit（untracked），git diff 对 untracked 无输出——**提交前以等价机械检 `crq-render.mjs --check` exit 0（内容级比对）为准**，提交后 V5 原命令恢复完整效力。
- **V6** `landing-fidelity-check.mjs --preflight <PROPOSAL>` → `ok:true / findings:[]`；anchorCount **124**（== 展开后 manifest 124 行；**verdict 后更正**：原记 26 为目录简写展开前旧值，verifier ADVISORY 点名）——绿。
- **V7** `route-regression-check.mjs` → `28 ok, 0 fail`（+ `--self-check` falsifiable 绿）；`lint-descriptions.mjs` → 仅既有 warn（landing-verifier.md 超长，非本批文件）——绿。**本批实证**：adjudicator description 改写曾丢「queue」必留信号，被本闸当场抓红后修复（起草期声明⑤第 7 处）。
- **V8** 迁移人读核 → 判官 R1 抽 4 条逐字核过 + 94 条机械对账；两处已声明语义差（§3）；**全量抽核由 landing-verifier 承接**。
- **V9** render/doctor/shared 三件 grep `ARCH-REVIEW-QUEUE` → 零命中（migrate 读历史源为声明内例外）——绿。
- **V10** `crq-render.mjs` 连跑两次 + diff → 零差异——绿。
- **V11** 合并实演两场景（异日期/同日期）→ 零人工解冲突；union 产物差异仅行序+摘要计数（两侧摘要相同、平凡取一，并集真值多 1）；doctor `index-stale` 当场抓红、重渲染归位；判官 R1 以 v0.4 现版脚本重演「双侧各结一条」复核——绿。
- **V12** R6 守卫（scratch 仓删生成标记头）→ `FATAL 目标不是生成件…拒写`、exit 1——绿。
- **V13** 视图人读核 → gov INDEX 摘要「未结 13（待拍板 0 / 待触发 11 / 已接受风险 2）；已结 64（归档索引见下）」（**verdict 后更正**：冻结时点为未结 12 / 已结 64，差 = FROZEN 后 dogfood 登记 1 条，§3-10）+ 已结区单行年度指针；ID 链接同目录相对形态可达——绿。
- **V14** 裸指涉 INDEX 扫描 → 无「单独指涉某一份却未带目录」形态（「三份/各 INDEX.md」类泛指按 T6 豁免判据不计）——绿。
- 真空绿规约：doctor/render 条目数 0 时 FATAL（E9 实证拦过一次真事故）；V1 dry-run 源非空前置；V11 两分支各自渲染成功已断言。

## 3. 诚实注记

1. **OQ 裁定状态**：OQ-1 / OQ-4 = human 直接裁定；**OQ-2=(a)、OQ-3=(a)、OQ-5=(a) 按推荐位执行**，凭 human 2026-09-24 指令「通过后landing，明早我看一下再merge」——**合并前 human 追认/否决窗口在场，否决即回滚（未合并前全量可逆）**。
2. **迁移期两处已声明语义差**（其余 92 条逐字零差）：`ARQ-260902-cert-lifecycle-deadlock` +177（双重落位合并注记，T1 改法 5-1 裁决表）；`ARQ-260923-schedule-grid-restore-gate` +316（T3 例外条款置 closed + 结项段，源内容本就写明已结）。
3. **advisory 134 条为在册历史欠账**（非本批新造）：no-paths 94（迁移项未申报路径，G 层暂不可机判）/ legacy-prefix 18（祖辈 `ARQ-` 前缀落非 gov 域）/ tech-no-handoff 17（迁移项未填去向）/ severity-off-rubric 5（`COULD`/`MAJOR` 队列历史用词，归一须另批，留根 13）。
4. **`owner` 字段为 best-effort 正则提取**（如「architect（定方向）+」截断），语义权威在正文散文；不回填不修历史（留根 15）。
5. **降级/等价替换一处**：V5 的 git-diff 形态在 untracked 期无输出，提交前以 `crq-render --check`（内容级）等价执行——已在 §2-V5 声明口径。
6. **起草期共暴露并修复 7 处自设计缺陷**（PROPOSAL 起草期声明⑤逐项），其中 2 处（adjudicator 写面三处漏改、description 丢路由信号）为两轮判官未点名、主线程自查/机械闸补捉——如实记，不归功于评审完备。
7. **census 口径更正史**：初版「52 处引用」口径未登记不可复现（R1-F5），终版三分口径 135 = 活跃 22 + changes 93 + taskpackages 14 + workstreams 5 + 账本本体 1，判官逐桶复现吻合。
8. **留根 18 条**照 FROZEN §6 原样保留（含 cdq 判据非机械化、severity 归一另批、workstreams/changes/taskpackages 豁免面、分片触发、模板升格候补）。
9. **FROZEN 后格式例外（待 human 晨间追认，先例 = 260913-arch-review-spec-split）**：原 manifest 末行目录简写 `docs/crq/` 被 `landing-fidelity-check.mjs` 判 claimed/planned-but-not-landed——该门为**逐文件精确匹配**、不消费目录锚，而 preflight 锚提取接受目录 token（两门口径不一致）。处置 = 目录简写**机械展开为 99 行成员枚举**（脚本生成非手打；§2 T1/T3/T4 锚行同步展开保「§2 并集 ≡ manifest」；先例 = 260914 批 139 行全枚举），**授权语义零变化**。陷阱本身已登记 CRQ 条目（见下条）。展开后结构门 exit 0。
10. **FROZEN 后首条账本登记（dogfood，非本批落地面）**：`docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md`（治理上一条注记的两门口径差陷阱；含 paths/gateLayers/hardExclusions 机检申报，登记后 doctor RED 0 / advisory 回到 134、`render --check` exit 0）。该文件**不在 frozen manifest**——属账本日常运行动作而非 packet 授权面；结构门 advisory 已点名之，本注即答复。
11. **结构门 advisory 另一项** `delivery/workstreams/m3-schedule/progress.md`：门跑时刻的环境扰动（portfolio-sync 钩子触碰），**复查 `git status` 已干净**——非本批改动，如实记。
12. **verdict 后记录更正（landing 面零改动）**：按 landing-verdict ADVISORY-2/3 与 V6 复跑，把本 summary 三处过时数字改准（V6 anchorCount 26→124〔展开前旧值〕；V13 未结 12→13〔差 = dogfood〕；§1-T2 归档 63→64〔差 = 例外条款条目〕），并补 `gov-arq/SKILL.md` 尾换行（ADVISORY-4，Write 面修复、非 Bash 重定向——首次尝试被 enforce-scope 拦截，钩子按设计生效）。**更正的只是记录，不是落地**；verdict 的 PASS 判定对象（落地 == FROZEN 清单）不受影响。

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