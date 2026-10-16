# architect-landing-summary · 260924-gov-review-gate-parse-teeth

packet-id：FROZEN §2 改法集（T1–T10）+ §5 验证集（V0–V11） · 评审轮次 R1 BLOCK(F1,F2) → v0.3 修订 → R2 BLOCK(F3) → v0.4 修订 → **R3 GREEN**（三轮收敛、未触熔断、零升级 human，§7 在案）· change-id 260924-gov-review-gate-parse-teeth · 落地窗口 2026-09-25（FROZEN v0.4 后同日）

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 commit-safety 空态文案按时序分流 | `.claude/hooks/commit-safety.mjs` | `statLine(trigger, stat)` 纯函数（add 相空 → 时序常态文案 / commit 相空 → 原异常警示文案保留 / 非空原样返回）+ `buildAdvisory` 接线 + 3 判别 case；头注时序注补分流句 |
| T2 verify.mjs 未知 phase fail-loud（OQ-2=(a)） | `.claude/hooks/verify.mjs` | `VALID_PHASES`/`phaseArgError`/`assertPhaseArg` 入口校验（缺省→scoped 保留）+ `--self-check` 分支（纯 case 6 + spawn 拒绝 case 2）；校验先于一切派发、零副作用 |
| T3 changes-index / mutation-check 传参面 | `.claude/scripts/changes-index.mjs` · `.claude/scripts/mutation-check.mjs` | 前者 `--help` + unknown-flag exit 2（裸跑仍全量重建）；后者 `--change-id <id>` 旗标解析 + `--help` + unknown-flag 拒绝；两脚本 self-check 增 entrance case |
| T4 enforce-scope-canary 退役（OQ-3=(b)） | `.claude/scripts/enforce-scope-canary.mjs`（删） | `git rm` 删除；enforce-scope.mjs 本体零 diff（契约逻辑不触）；结项注含覆盖缺口逐字记录 |
| T5 引用字段磁盘存在性 warn 齿 | `.claude/scripts/taskpackage-required-check.mjs` · `.claude/scripts/brief-inputs.mjs` · `.claude/scripts/mutation-check.mjs` | 前者 `collectRefWarnings` 纯函数 + `warnings` 键（ok/fail 两路）+ `--self-check` 5 case；中者 missing 态 advisory + 判别 case；后者缺失验收文件从静默跳过改 `warnings` 可见 |
| T6 current/ 解析点常驻回显齿 | `.claude/scripts/lib/current-pointer-shared.mjs`（新增）+ hooks 3 + scripts 11 逐件接线 | 新 lib 单源（`echoResolution`/`resolveCurrentTaskPackage`/`resolveExplicitTaskPackage`，自带 self-check 7 case）；14 件回显接线（显式/current 两路、source 三态如实） |
| T7 dispatch-stamp 可观测齿（OQ-1=(c)） | `.claude/scripts/dispatch-stamp.mjs` | 事件 payload 增 `contextSource` + `expectCheck`（absent/match/mismatch/no-context 四态）；exit 行为逐字不动 |
| T8 landing-summary 模板指针 | `.claude/references/arch-review-protocol.md` · `.claude/references/track-b-verifier-brief.md` | protocol 第 4 步补指针句（复制勿手写 + 两实例注）；verifier-brief 核验清单第 2 项同句加固 |
| T9 preflight 目录 token advisory + 模板纪律句 | `.claude/scripts/landing-fidelity-check.mjs` · `docs/templates/PROPOSAL.template.md` | `dirTokenAdvisories` 纯函数 + preflight 输出 `advisories` 键（ok/exit 零变更）+ 2 判别 case；模板四处（manifest 说明行 / 读法注第 4 条围栏承载 / 自检句 §5 逐子句时点 / 自检第三句） |
| T10 账本结项与状态回写 | `docs/crq/gov/` 三结项件（归位 `_archive/2026/`）+ 两部分承载件追注 + INDEX 两件 | render 归位三条 + 两件部分承载追注 + INDEX 重渲染；doctor RED 零命中 |

## 2. 验证证据

**V0 起草期基线**：六脚本及邻接件 self-check 全绿（记录于 PROPOSAL §5 V0）；canary 红基线 3/7（T4 证据段 verbatim）；T5 census 43 包目录 694 条（478 在场/87 缺失/129 非路径；R1-F1 勘正 + R2 口径裁定接受「排除 current 副本」）；T6 枚举 18 件分类 14+2+2。

**V1**（T1 判别）：`node .claude/hooks/commit-safety.mjs --self-check` → `{"selfCheck":"commit-safety","pass":15,"fail":0,"total":15,"ok":true}`（12 基线 + 3 新 case）；判别性 = `statLine` 三态断言（add 相空 → 新文案**逐字** / commit 相空 → 原异常警示文案逐字 / 非空 → 原样返回），反转实现必须变红。**live 佐证（如实）**：landing 期真实 `git add` 的快照**非空**（canary 删除先前已 staged）——回落行未触发，空态分支的实证由上述 self-check case 承担（真实空态快照需「add 前零 staged」场景，本批 landing 流程不自然产生）。**绿**。

**V2**（T2 判别，OQ-2=(a)）：`node .claude/hooks/verify.mjs --self-check` → `pass:8 fail:0`；live `node .claude/hooks/verify.mjs --phase=full` → exit 2 + stderr「未知 --phase 值 "full" —— 只认 scoped|final（fail-loud，不静默回落；bearing 路径 normalizeExpected 同判据）」，校验先于一切派发零副作用。**绿**。

**V3**（T3 判别）：`changes-index --help` → exit 0 用法输出；`changes-index --bogus-selfcheck-probe` → exit 2 且 `git diff --stat -- delivery/changes/` 仅余 upsert 过程件（**未触发重建**）；`mutation-check --help` → exit 0；`mutation-check --change-id 260829-char-stroke` → 解析到 canonical 路径并跑闸（ok:true）；`mutation-check --bogus x` → exit 2。self-check：changes-index 31 例 fail 0 / mutation-check 12/12。**绿**。

**V4**（T4 退役，OQ-3=(b)）：`test -f .claude/scripts/enforce-scope-canary.mjs` → 非零（已删）；`grep -rln "enforce-scope-canary" .claude/` → 零（脚本本体删除后无任何 .claude 面命中；docs/delivery 命中 = 账本结项注 + 历史制品 + 本批制品，逐面枚举）；`git diff --stat -- .claude/hooks/enforce-scope.mjs` 为空（契约本体零触碰）；结项注含覆盖缺口关键词（无 --self-check / F1/F3）。**绿**。

**V5**（T5 判别）：三脚本 self-check 全绿含新增 case（taskpackage-required 5/5 / brief-inputs 29/29 / mutation-check 12/12）；**闸行为不变断言**：char-stroke 历史包显式跑，改前版（HEAD 版临时对照件，落盘跑毕即删）与改后版 **ok / missing 逐条 / coverageErrors 全相同**，改后 `warnings` 3 条 == 起草期 census 该包缺失数 3（逐条明细：docRefs `docs/spec.md`×2 + acceptanceTestsFile 1，纯增量键）。**绿**。

**V6**（T6 判别）：lib self-check 7/7；接线 census = `grep -rl current-pointer-shared` 命中 **14 件**（+lib 本体）；**live 回显活证**：gov worktree 裸跑 `taskpackage-required-check` → stderr `[taskpackage-required] TaskPackage 上下文解析：changeId=260924-perf-k1-roundtrip-latency（来源：delivery/taskpackages/current/taskpackage.json）`（指错包现场报警在真实漂移上生效）；语法 `node --check` 15/15；宿主 self-check 全家回归绿（见 §3 缺陷注）；**F1 修复后补测**：无 current/ 夹具下 11 件改前（HEAD 独立树）↔改后 exit 全等（5 修复件 0/0 + 对照 6 件逐一对上）、有 current 时 5 件回显照常（见 §3 注 10）。**绿**。

**V7**（T7 判别，OQ-1=(c)）：mkdtemp 夹具场（`CLAUDE_PROJECT_DIR` 指向夹具、零仓内写入）带旗标 → 事件 `expectCheck= "match"`、`contextSource= "--task-package-id"`；不带旗标 → `expectCheck= "absent"`（**值而非缺键** = 判别性断言）；消费面 `pod-events-aggregate` 对夹具事件照常跑通（ok:true / eventsTotal 2）。**绿**。

**V8**（T8）：protocol 第 4 步指针句 grep 命中 1（「骨架见 …，复制勿手写」）；verifier-brief 加固句命中 1；`--preflight` 本批 PROPOSAL 复跑 ok:true / anchors 31 / findings []。**绿**。

**V9**（T9）：目录 token 样例 → `ok:true` / exit 0 / `advisories: 1`（点名 `docs/crq/`）；纯文件样例 → `advisories: 0`（判别性双向）；landing-fidelity `--self-check` preflight 套件 19/19（含新增 2 判别 case）；模板四处 grep 各命中 1；本批 PROPOSAL preflight 复跑 ok:true / anchors 31 / advisories 0。**绿**。

**V10**（账本与状态机）：`crq-render` 三结项件归位 `_archive/2026/`（三条归位行 verbatim）；render 复跑五 INDEX 全「未变」（**pre-commit 等效句**，commit 后终证见 retro §6）；`crq-doctor` RED 零命中（142 advisory 为存量不改）；`changes-index --upsert` 于起草/frozen 两会话点已跑（draft→frozen 行实录）。**绿（post-commit 幂等子句待 commit 后复跑）**。

**V11**（结构门与申报）：见 §3 工作树申报；`landing-fidelity-check` 本批目录全绿（landing-verifier 复跑为独立复核面）。

## 3. 诚实注记

1. **落地期实测抓出并当场修复一缺陷（如实登记）**：新 lib `current-pointer-shared.mjs` 初版 self-check 无 isMain 守卫，被 import 时（宿主带 `--self-check`）抢先 `process.exit`，**遮蔽宿主自检**（verify.mjs / mutation-check / taskpackage-required-check 末行曾被 lib 行顶替——「自检跑了 ≠ 自检真跑了」真空绿陷阱的实例）。已加 isMain 守卫（acceptance-id-shared 对偶形态）并复跑全量回归：宿主自检名全部回归（verify.mjs 8/8 / mutation-check 12/12 / taskpackage-required 5/5）。FROZEN §2 未含该守卫描述——按「落 == FROZEN」基准此为 lib 内部实现细节的缺陷修复（对外行为 = FROZEN 所述「自带 --self-check」），**主动披露供核验官裁**。
2. **ADRIVSORY/WARNING 枚举**：无新增 ADVISORY；存量 `crq-doctor` 142 条 advisory（no-paths 102 / severity-off-rubric 5 / legacy-prefix-off-domain 18 / tech-no-handoff 17）为历史存量、本批未动。
3. **行为边缘变更（如实行）**：loop-metrics `resolveChangeId` 由「current 副本 JSON 损坏则崩溃」改为「返回 null（fail-safe）」——经 lib 单源化后的唯一非逐字等价边缘（pod-events-aggregate / pod-events-append / journal-append 原实现本就有 try/catch → null，逐字等价）；方向为 fail-safe、判定语义面零变更，如实登记。
4. **dispatch-stamp legacy（pre-cutoff）分支不带新键**：`contextSource`/`expectCheck` 仅入 post-cutoff `dispatch-completed` 事件 payload（FROZEN §2 T7 所指面）；pre-cutoff 走 `dispatch-log.jsonl`（不同 schema）保持零变更。现存包全部 post-cutoff（cutoff 2026-07-25）。
5. **未结项（留根去向）**：`ARQ-260913-current-pointer-implicit-global` **部分承载不结**（残余②闸级强制挂量化触发器：`expectCheck:'absent'` ≥1 → 启动 (b) 批 / 30 全 match → settled 结项）；`ARQ-260924-landing-gate-dir-anchor-gap` **部分承载不结**（处方② landing 门目录前缀展开维持不预批、须独立批完整评审）。两件 trigger 已更新为现役形态。
6. **ARQ-260914 §4-1「承重引文 byte 级比对脚本化」未做**（留根 1，本批面已大不扩）；批 B retro §4-4 载体 = 登记不硬治（memory `partial-view-as-whole`），本批 T9③ 已把其纪律面成文进模板自检第三句。
7. **`.codebase-memory/` 两件常驻脏**：本批未 stage、未 stash（AGENTS.md 纪律）；landing commit 不含之。
8. **过程制品**：`delivery/changes/arch/INDEX.md`（changes-index upsert 过程件）不入 manifest、于本节申报认列；V5 对照用的 HEAD 版临时件（`.claude/scripts/tpq-before-tmp.mjs`）跑毕即删、零残留（已核）。
9. **零 G1 / 契约 / scope-map / verify-gates / agent defs / settings 触点**维持；`.claude/skills/**` 零触碰（T4 退役裁定裁 SKILL 锚出）。
10. **核验官首轮 FAIL(F1) 与修复记录（如实登记）**：F1 = 5 件（acceptance-coverage-check / closeout-check / docs-sync-check / security-check / mutation-check）的 path 解析由原 `return existsSync(current) ? current : null;` 被换成无条件 `.path`，破坏「current/ 缺席 → `{ok:true,skipped:true,'no TaskPackage context'}` exit 0」短路判据 → 落 `fail('TaskPackage file does not exist')` exit 1（判定语义变更、违 FROZEN「零变更」不变量；核验官以「改前原命令作基线」纪律抓出，首轮 summary 该记未记 = 本注补记）。**修复**：5 件恢复原判据形态 `const resolved = resolveCurrentTaskPackage(...); return existsSync(resolved.path) ? resolved.path : null;`；对照实证 = 无 current/ 夹具下 11 件（5 修复件 + taskpackage-required〔fail→fail 1〕/ devloop-artifact-check〔0〕/ journal-append〔2〕/ loop-metrics / pod-events-aggregate / pod-events-append〔均 0〕）**改前（HEAD 独立树）与改后 exit 全等**；有 current 时 5 件回显照常（exit 0 + 回显行在场）。核验官 ADVISORY 2 一并处置：mutation-check `--self-check` 补 T3④ 两 case（未知旗标 spawn exit 2 + 位置参数夹具实证），现 14/14。
11. **核验官其余 2 项 ADVISORY 为接受项（不改）**：(a) T2 承重代码块拆 `phaseArgError`/`assertPhaseArg` 两函数非逐字——语义/message/exit 全保真、拆分为可测性实现细节；(c) `expectCheck` 的 mismatch/no-context 两值因 exit(2) 先于 payload 写入而事件面不可产（只产 absent/match）——与「exit 逐字不动」自洽、为 (b) 批预留值域。

**工作树申报（B6 类全量，2026-09-25 landing 完成时）**：`git status --porcelain` 全量输出与逐行注性质见 landing commit 前实录（见下「工作树申报实录」小节——机械枚举不手写清单）。

工作树申报实录（commit 前实跑 `git status --porcelain` 全量 38 行，逐行注性质）：

```
 D docs/crq/gov/ARQ-260923-gate-tooling-noise-batch.md            ← render 归位（旧址删除）
 D docs/crq/gov/ARQ-260923-portfolio-decomposer-drift.md          ← render 归位（旧址删除）
 D docs/crq/gov/ARQ-260924-landing-summary-template-pointer-gap.md ← render 归位（旧址删除）
 M .claude/hooks/commit-safety.mjs                                ← T1
 M .claude/hooks/supervisor-gate-check.mjs                        ← T6
 M .claude/hooks/supervisor-verdict-gate.mjs                      ← T6
 M .claude/hooks/verify.mjs                                       ← T2+T6
 M .claude/references/arch-review-protocol.md                     ← T8
 M .claude/references/track-b-verifier-brief.md                   ← T8
 M .claude/scripts/acceptance-coverage-check.mjs                  ← T6
 M .claude/scripts/brief-inputs.mjs                               ← T5+T6
 M .claude/scripts/changes-index.mjs                              ← T3
 M .claude/scripts/closeout-check.mjs                             ← T6
 M .claude/scripts/devloop-artifact-check.mjs                     ← T6
 M .claude/scripts/dispatch-stamp.mjs                             ← T7
 M .claude/scripts/docs-sync-check.mjs                            ← T6
 M .claude/scripts/journal-append.mjs                             ← T6
 M .claude/scripts/landing-fidelity-check.mjs                     ← T9
 M .claude/scripts/loop-metrics.mjs                               ← T6
 M .claude/scripts/mutation-check.mjs                             ← T3+T5+T6
 M .claude/scripts/pod-events-aggregate.mjs                       ← T6
 M .claude/scripts/pod-events-append.mjs                          ← T6
 M .claude/scripts/security-check.mjs                             ← T6
 M .claude/scripts/taskpackage-required-check.mjs                 ← T5+T6
 M .codebase-memory/artifact.json                                 ← 常驻脏（不 stage）
 M .codebase-memory/graph.db.zst                                  ← 常驻脏（不 stage）
 M delivery/changes/arch/INDEX.md                                 ← upsert 过程件（不入 manifest）
 M docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md     ← T10 部分承载追注
 M docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md         ← T10 部分承载追注
 M docs/crq/gov/INDEX.md                                          ← T10 render 再生
 M docs/crq/gov/_archive/2026/INDEX.md                            ← T10 render 再生
 M docs/templates/PROPOSAL.template.md                            ← T9
?? .claude/scripts/lib/current-pointer-shared.mjs                 ← T6 新增 lib
?? delivery/changes/arch/260924-gov-review-gate-parse-teeth/      ← 本批制品目录
?? docs/crq/gov/_archive/2026/ARQ-260923-gate-tooling-noise-batch.md    ← render 归位（新址）
?? docs/crq/gov/_archive/2026/ARQ-260923-portfolio-decomposer-drift.md  ← render 归位（新址）
?? docs/crq/gov/_archive/2026/ARQ-260924-landing-summary-template-pointer-gap.md ← render 归位（新址）
D  .claude/scripts/enforce-scope-canary.mjs                       ← T4 退役删除（git rm 已 staged）
```

manifest 31 件全落在场；申报外仅 `delivery/changes/arch/INDEX.md`（过程件）+ 本批目录 + `.codebase-memory/` 常驻脏 2 件。

```landing-manifest
.claude/hooks/commit-safety.mjs
.claude/hooks/verify.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/mutation-check.mjs
.claude/scripts/enforce-scope-canary.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/brief-inputs.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/scripts/closeout-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/docs-sync-check.mjs
.claude/scripts/journal-append.mjs
.claude/scripts/loop-metrics.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/security-check.mjs
.claude/scripts/lib/current-pointer-shared.mjs
.claude/references/arch-review-protocol.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/landing-fidelity-check.mjs
docs/templates/PROPOSAL.template.md
docs/crq/gov/_archive/2026/ARQ-260923-gate-tooling-noise-batch.md
docs/crq/gov/_archive/2026/ARQ-260923-portfolio-decomposer-drift.md
docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md
docs/crq/gov/_archive/2026/ARQ-260924-landing-summary-template-pointer-gap.md
docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/INDEX.md
```
