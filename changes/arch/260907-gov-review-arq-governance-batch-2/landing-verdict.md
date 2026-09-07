# Landing Verdict · 260907-gov-review-arq-governance-batch-2

> 核验人：landing-verifier（Track B 步骤 5，独立于落地者主线程）
> 日期：2026-09-07
> 核验对象：FROZEN PROPOSAL v1.0（commit e81c600 = 当前 HEAD）§2 改法集（T1–T8）+ §5 验证集 + 头部 landing-manifest（13 路径）vs 当前工作树未提交改动
> 判定来源：机械脚本退出码 + 逐文件人读 diff 实证（`git diff HEAD -- <path>`，docs/ADR.md 为 untracked 读全文）+ 验证集独立复跑；不以 landing-summary 自述为通过依据

## 1. 核验方法

1. 结构门 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-arq-governance-batch-2`（机械，独立复跑，exit 0）；
2. 逐文件人读 diff：manifest 13 件逐一 `git diff HEAD` 全读比对 FROZEN §2 对应 T 改法，无抽样；
3. 验证集独立复跑：V1 / V3（五个具名自检入口，未触裸 `--self-check`）/ V4（四包真数据重放，脚本纯 console.log 只读）/ V6 / V7 实跑；V2 census 独立重算；V5 / V9 人读核对（V8 = 结构门本身，见 §2；V9 计数见 §3 agent def 行）；
4. 破坏性自检：T3 三拍语义与失败路径、T4 unknown 兜底双轨、检测面/锚提取两职责分离论证、manifest 外全量对账（`git status --porcelain` + `git diff --cached --stat` 逐一归账）；
5. Track B 确认：证据目录仅 PROPOSAL.md + architect-landing-summary.md，无 decision-packet.md → 设计侧门 `proposal-consolidation-check.mjs` 按 profile N/A 不跑（FROZEN ↔ 批定一致性由 R2 GREEN 段 + Status 行 FROZEN 承载）。

## 2. 结构门输出（独立复跑，2026-09-07）

`ok:true`、`frozen:true`、`packetBinding:true`、`gitDegraded:false`；`proposalTargets` 与 `summaryTargets` 两侧各 13 件逐字一致 = manifest 满配形态；`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []` 全空。**本批 T1/T2 改动的自证效应在门输出中直接可见**：锚集 ≡ manifest 首次可机判（13 件含四件 agent def 与 changes/ 外全部形态均被 ANCHORABLE_TEST 收入）；citation advisory 的 `resolvedByPrefix` 列 13 条留痕消警（裸文件名引用经 `.claude/scripts/` / `.claude/scripts/lib/` / `.claude/hooks/` 前缀重解析命中）——T2 的「消警有痕、不静默」设计按声明工作。

advisory 定性（均不阻塞）：

- **citation 残余 2 条 `file-not-exist`**（`DEV-GOVERNANCE.md:280`、`PROPOSAL.template.md:49`）：裸 docs 相对文件名不在 T2 三前缀候选域内（候选只覆盖 .claude 脚本面）。**落地与 FROZEN 完全一致**——FROZEN T2 只钉了三前缀，此处非漏改，是 FROZEN 设计域的已知边界。建议留根为后续 ARQ 候选（前缀候选补 `docs/` 相对形态），不在本批核验义务内。
- **lintDescriptionsAdvisory 1 条**（landing-verifier.md 超长 363 > 320 chars）：本 verifier 自身 agent def，非本批 manifest 面、非本批改动（先于本批在册），advisory-only。

## 3. 逐文件 diff 核对表（manifest 13 件全覆盖）

| file | 对应 T | diff 实证 | 结论 |
|---|---|---|---|
| `.claude/scripts/lib/governance-gate-shared.mjs` | T1 | 新增导出 `ANCHORABLE_TEST = /^(?:\.claude\/|docs\/|\.planning\/|README\.md|AGENTS\.md|CLAUDE\.md\|\.[A-Za-z0-9_-]+$\|changes\/)/`（头注释标明消费方唯一 + 两职责分离 + R1-F8 核对）；:386 锚过滤 `CANONICAL_TEST.test(...)` → `ANCHORABLE_TEST.test(...)` 恰此一处切换；**`CANONICAL_TEST` 本体零触碰**；`runSection2AnchorSelfCheck` 补仓根点文件锚（dotfile-anchor 两断言）+ changes/** 锚（changes-anchor 两断言）用例 | ✅ 逐字命中 FROZEN T1 |
| `.claude/scripts/landing-fidelity-check.mjs` | T1/T2 | checkVerbatimQuotes 循环内双侧归一 `norm(content).includes(norm(block))`；checkCitationAnchors file-not-exist 前追加三前缀依次试探，命中入独立 `resolved` 数组（返回值新增第三键，checked/problems 不混）；citation 自检 +3 断言（prefix-resolved 计数/形态/零 problems）、verbatim 自检 +2 断言（CRLF 目标文件命中）；section2Anchors 自检用例在本文件由 gate-shared 导入后生效；`:367` changedNotInProposal 检测面无 hunk 触碰；新增 import `mkdirSync` 仅自检夹具用 | ✅ 逐字命中 FROZEN T1/T2 |
| `.claude/scripts/merge-driver.mjs` | T3 | `ensureSkipWorktree` → `ensureGraphCacheSynced`：三拍 = ① S 在册先 `--no-skip-worktree`、② `git checkout HEAD -- <path>`（index+worktree 双还原，R1-F2 消解）、③ 恒复旗标；stderr 留痕 `N content-restored, M flag-only`；失败 warn advisory 不阻塞（两调用点 rebase 步 / merge 步语义仅换名，位置不变）；头注释补 renormalize 操作序注记（「旗标与内容（含 staging）双兜底」）+ rebase 中断重复 pick 清 todo 指引；拓扑自检 +4 断言（形态一 worktree 漂移：还原 + 旗标复位；形态二 staged 漂移：`git status --porcelain` 净树 + 旗标复位） | ✅ 逐字命中 FROZEN T3（见 §5 断言计数说明） |
| `.claude/scripts/pod-events-aggregate.mjs` | T4 | `aggregateFriction` 新增 `malformedFrictions` 列：空 payload（含非对象 payload）/ 缺 frictionType / 缺 summary 三判据，入列含 eventId + ts + missing；**既有 `unknown` 兜底保留原样**（byType 归 unknown 与入列告警双轨）；无 self-check 入口（与 FROZEN 声明一致，验证走 V4 真数据重放） | ✅ 命中 FROZEN T4（一处字面差异见 §5 观察 1） |
| `docs/DEV-GOVERNANCE.md` | T4/T5/T6/T7 | 纯新增 21 行：§7 表新增 ADR 行（T7 指针）；「### 4. C 类冻结验收实编（编译通道 error 为零）」（§3 并列不互替、host-test-infra 实证引证）；「## 起包格式期望前置」五条清单逐条对 ARQ 五子项（含四包实证引证）；「## friction 事件最小 schema」（frictionType+summary 必填、扩展键允许、空 payload = 落账缺陷、当次交互内随手记）；既有内容零改动 | ✅ 节名与内容命中 FROZEN T4/T5/T6/T7 |
| `docs/templates/PROPOSAL.template.md` | T8 | 硬项 2a 段末恰追加两句（`### T<n>` 标题形态 + manifest 裸路径形态）+ 溯源括注 | ✅ 逐字命中 FROZEN T8 |
| `docs/ADR.md`（untracked 新增，全文实读） | T7 | 头节性质声明（区别于 changes/** 过程证据与 docs/notes/）+ OQ3 凭据 + writer 声明（integrator 收口 + 主线程治理批，append-only）+ 条目格式约定（`## ADR-NNN` + 日期/决策/理由/备选/关联）；零决策条目预填（含明示「不做考古搬迁」） | ✅ 命中 FROZEN T7 第 1 件 |
| `.claude/scope-map.json` | T7 | `universal.docsTopLevel.allowFiles` 加 `docs/ADR.md`（全路径形态，R1-F10 消解）+ integrator allow 加 `docs/ADR.md`（写槽）；JSON 有效（V6 实测 parse 过）；其余键零触碰 | ✅ 命中 FROZEN T7 第 2 件 |
| `.claude/rules/repo-structure.md` | T7 | docs 根白名单行加 `ADR.md`（裸名形态，照现行形态落——R1-F10 两处异向措辞均按建议执行） | ✅ 命中 FROZEN T7 第 3 件 |
| `.claude/agents/acceptance-author.md` | T5/T6 | 恰 +2 行（§4 C 类实编引用 + 起包清单引用），References 节内，frontmatter / 工具面 / Boundaries 零改动 | ✅ V9 计数命中 |
| `.claude/agents/planner.md` | T4/T6 | 恰 +2 行（friction schema 引用 + 起包清单引用） | ✅ V9 计数命中（F4 消解实证） |
| `.claude/agents/portfolio-planner.md` | T6 | 恰 +1 行（起包清单引用） | ✅ V9 计数命中 |
| `.claude/agents/builder.md` | T6 | 恰 +1 行（起包清单引用） | ✅ V9 计数命中 |

agent def 合计恰 +6 行（2/2/1/1），四件全部只加 References 引用行、职责/工具面结构零改动——OQ2=(a) 批准范围严格成立，无借引用行夹带权限或写域扩张。

## 4. 验证集独立复跑（2026-09-07，全部实跑）

- **V1** `landing-fidelity-check.mjs --self-check` → exit 0；section2Anchors `pass:19`（原 17 + 2 新用例 4 断言）、citation-anchors `pass:5`（原 2 + 3 前缀断言）、verbatim-quotes `pass:6`（原 4 + 2 CRLF 断言）——与 summary 声称计数逐项一致 ✅
- **V2** ANCHORABLE_TEST census 独立重算：仓根点文件 3 件（.cbmignore / .gitattributes / .gitignore）+ changes/ 前缀 126 条——与 FROZEN §3 census 及 summary 一致 ✅
- **V3** merge-driver 五具名自检全 ok：`--self-check-worktree-topology` → `pass:9 fail:0 ok:true`（原 5 + 两形态各 2 断言；stderr 留痕两行实测出现 `2 content-restored, 0 flag-only` / `2 content-restored, 2 flag-only`）；retro-advisory 3/3、quick-checks ok、detect-changes 16/16、rebase-target 7/7 零回退 ✅
- **V4** 四包真数据重放（脚本纯 console.log 只读）：260829-voice-roundtrip → fb2c70c9 / 55a43813 均 `empty-payload`；260903-device-ws-switch → b2d0d32c / 05957d1c 均 `missing-frictionType` + `missing-summary`；260831-ui-art-assets 跑通不崩（friction total 0，ledger=0/retro=N 形态按 FROZEN 口径留人读）；260829-session-logic 跑通不崩（1 条完好 environment 不入列）——与 FROZEN V4 断言逐条吻合 ✅
- **V5** 成文 grep：三新节标题（:285 / :289 / :299）在场；scope-map `docs/ADR.md` 2 命中（allowFiles + integrator allow）；repo-structure 白名单行在场；ADR.md 骨架在册 ✅
- **V6** `JSON.parse(scope-map.json)` 有效 ✅
- **V7** `node --check` 四件 .mjs 全过 ✅
- **V8** = §2 结构门（独立复跑 ok:true，锚集 ≡ manifest 首次可机判——T1 自证成立；verbatim checked 空集 = §2 无围栏代码块、missing 零——T2 自证口径成立）✅
- **V9** diff 行数逐件实数：acceptance-author +2 / planner +2 / portfolio-planner +1 / builder +1，合计恰 +6；职责/工具面零改动 ✅

## 5. 破坏性自检与诚实注记复核

**破坏性自检：**

1. **T3 失败路径不阻塞合并：成立**。ls-files / checkout / update-index 三处失败均走 `warn` advisory 返回；③拍恒复旗标（注释明示「失败时 git 自身的脏树门仍兜底 fail-closed」）——无新增放行路径。
2. **检测面/锚提取两职责分离：成立**。全 diff 中 `CANONICAL_TEST` 仅在 :386 锚过滤一处被换；landing-fidelity-check `:367` changedNotInProposal 无 hunk 触碰——changes/** 常规制品写入不会误报 changed-not-in-proposal。
3. **T4 双轨语义：成立**。byType unknown 兜底行逐字未动，malformedFrictions 为纯增量输出字段；聚合脚本无写盘操作（实测确认）。
4. **manifest 外零流浪改动：成立**。`git status --porcelain` 全部条目 = 12 M + `docs/ADR.md` ??（manifest 第 7 件）+ `architect-landing-summary.md` ??（Track B 步骤 4 规定制品）；`git diff --cached --stat` 空（staged 区干净）；契约面 / spec / firmware / server / verify-gates.json / settings 零触碰；`.codebase-memory/` 两件 skip-worktree 文件不在 porcelain（符合预期不入账）。HEAD = e81c600 恰为 FROZEN 钉行 commit（只触 changes/ 簿记 + PROPOSAL 本体），landing 改动全部在工作树未提交——与 Track B 步骤 5 时序一致。

**summary §3 六条诚实注记逐条复核：**

1. **V3 pass:9 vs FROZEN「pass:7」**——成立。FROZEN V3 写「原 5 断言 + 内容级还原 + staged 漂移新断言」按每形态 1 条断言预估；实际落地每形态 2 条（还原生效 + 旗标复位各一），4 > 2 为断言粒度超集，断言集合完整覆盖 FROZEN 声明的两形态场景（含 staged 漂移断言 `git status --porcelain` 净树），非设计变更。宽免。
2. **`mergedInMain=yes` 提前钉入**——成立且如实。changes-index 的 `gitContainedInMain` 推导在本批 PROPOSAL 草稿 commits 已随并发会话 ff-merge 入 main 历史时为真，脚本语义下非误账；本批 landing commits 合并仍在步骤 7。属簿记推导边界，summary 已披露，不构成 landing 保真问题。
3. **会话拓扑插曲（89ca380 误落 main 已修复）**——git log 实证自洽：55abc92（v0.3 修订）在 1c46aa1（并发批 landing）之上、e81c600（FROZEN）在其上，拓扑与注记描述一致。已修复态确认。
4. **V9 当场修正（acceptance-author 1 行拆 2）**——终态 diff 实数 +2 与 FROZEN 一致，修正已发生且如实登记。
5. **交日项结论（包 2 踩 T6 / T7 残余 / T8 遵循度）**——三项结论均在册且定性合理（T6 踩 = 本批 T3 承接；T7 残余定性为 worktree checkout 代次、非规则缺陷；T8 遵循度在册）。属 retro 素材，与 landing 保真无冲突。
6. **V4 计数口径（机判仅覆盖逐事件形态）**——与 FROZEN V4 原文口径一致（「缺事件」形态明示留人读），无虚报。

**观察（不阻塞，供主线程/retro 记账）：**

1. **T4 定位信息字段字面差异**：FROZEN T4 写入列「含 changeId + eventType 侧定位信息」，实际入列字段为 `eventId + ts`（输出顶层有 `changeId`、且 aggregateFriction 构造上 eventType 恒为 friction）。eventId+ts 定位精度高于 changeId+eventType，语义意图（可定位残缺事件）达成，但与 FROZEN 字面不符——定性为措辞级偏差、意图内超集，建议 retro 时顺手在 DEV-GOVERNANCE 或脚本注释对齐措辞（非本批必修）。
2. **T2 前缀候选域不含 docs 相对裸文件名**：结构门残余 2 条 file-not-exist advisory 即此（见 §2）——FROZEN 设计域边界如实落地，留根 ARQ 候选。
3. summary §1 所记 merge-driver 两调用点行号（:800/:895）与实际（约 :821/:916）有漂移，纯 summary 行号注记、不影响 FROZEN 声明（FROZEN 只钉「两调用点不变」，实况成立）。

## 6. Verdict

**GREEN** — 实际落地 == FROZEN PROPOSAL v1.0 的 landing 清单。结构门独立复跑全绿（frozen / packetBinding / 双侧 13 件一致 / unauthorized-claimed-planned 三列全空，T1/T2 自证效应在门输出实证可见）；逐文件 diff 13/13 全读核对通过（含敏感面四件 agent def 与 scope-map/repo-structure 结构边界逐字核）；验证集 V1–V9 独立复跑全绿；破坏性自检四项均成立；manifest 外零流浪改动；summary §3 六条诚实注记逐条复核成立、无隐瞒。两处措辞级观察（T4 定位字段、T2 前缀域边界）均在 FROZEN 意图内，留根记账不阻塞。

```json
{"ok": true, "changeId": "260907-gov-review-arq-governance-batch-2", "verdict": "GREEN", "structuralGate": "pass(ok:true, frozen, packetBinding, 13/13 both-sides, unauthorized/claimedNotLanded/plannedNotLanded all empty; 2 citation advisories = FROZEN T2 prefix-domain boundary, advisory-only)", "designGate": "N/A(TrackB, no decision-packet)", "perFile": "13/13 ok (T1 anchor-split + :386 switch, T2 CRLF-norm + 3-prefix-resolve, T3 three-beat HEAD-checkout both call-sites, T4 malformedFrictions 3-criteria, T5-T7 docs/scope-map/repo-structure/ADR, T8 template 2-sentences, agent-def +6 rows refs-only)", "verificationSet": "V1-V9 all green (V1 self-check exit0 pass19/5/6; V2 census 3+126; V3 topology pass:9 + four legacy self-checks; V4 four-package replay exact-match; V5-V7 grep/parse/node-check; V8 = structural gate; V9 +2/+2/+1/+1)", "destructiveSelfCheck": "4/4 hold (T3 advisory-only failure path; detection/anchor separation intact; T4 unknown-fallback preserved; zero stray changes, staged clean, HEAD=e81c600 FROZEN-pin)", "honestNotes": "6/6 confirmed, no concealment", "observations": ["T4 locating-field literal deviation (eventId+ts vs FROZEN changeId+eventType) — intent-superset, wording-level, retro-bookkeeping", "T2 prefix domain lacks docs-relative bare names — FROZEN design boundary as-landed, ARQ candidate", "summary §1 merge-driver call-site line numbers drifted (:800/:895 vs actual) — summary-only"], "blockers": []}
```

放行建议：可进 Track B 步骤 6（retro report，含 §6 交日项「包 2 是否踩 T6/T7/T8」结论记账 + 本批两条观察留根）→ 步骤 7 前置升级闸（gov 档机锚 + off-main 校验）→ 自动合并本地 main（可回滚；remote push 仍归 human）。T4 定位字段措辞对齐与 T2 前缀域扩展建议随 retro 登记为后续 ARQ 候选，不在合并前置路径上。
