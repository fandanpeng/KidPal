# architect-landing-summary · 261026-gov-review-retro-efficiency-fidelity

packet-id：FROZEN v0.4 §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 `BLOCK(F1..F7)` → R2 `GREEN` · change-id `261026-gov-review-retro-efficiency-fidelity`

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 行数归因保真：subject 分类器为主判据、批量合并按 changeManifest 三级切分、回同步合并排除、三来源 SHA 并集去重、跨包共享 SHA 防御性 guard、`merged`/`mergedCommit` 只读消费、多笔合并 max 定性为下界 | `.claude/scripts/retro-efficiency.mjs` | 已落；`261013-lang-config-thinking-fix` 由 60,017/12,231/125,046 回到 48/0/2,424；三个 `261022` 包由同值 5,084/4,041/4,665 分为 354/0/1,096、280/0/1,585、1,228/961/1,403；`batchUnattributedPaths` 17 条（11 无主 + 6 歧义） |
| T1 写侧：`merge-completed` payload 增 `mergeSha`（全 40 位，取 Step 5b 已算出的 HEAD） | `.claude/scripts/merge-driver.mjs` | 已落；T6 夹具新增断言「payload.mergeSha ≡ 调用前 HEAD 且 40 位」通过 |
| T2 档位列：读 `taskpackage.json.rigor`，`origin.workstream` 加 `·拆包`，governance 按 subsystem / changeManifest 全落治理根，缺值留空 | `.claude/scripts/retro-efficiency.mjs` · `.claude/commands/loop-retro.md` | 已落；窗口实测 full 9 / lite 15 / nano 1 / governance 0 / 拆包 5 / 留空 0；三个抽查值 `261016-p3-story-body-sd-cache=full`、`261020-p3-rec-frame-visual=nano`、`261022-m5-n2a-schedule-mechanical-injection=lite·拆包` |
| T3 生命周期：`pod-started` / `pod-closed` 两事件（planner 起包、integrator 收口），读侧「观察窗 / agent / 等待」第三张表，缺失 unknown 不回退 | `.claude/scripts/pod-events-append.mjs` · `.claude/scripts/retro-efficiency.mjs` · `.claude/agents/planner.md` · `.claude/agents/integrator.md` · `.claude/references/dev-loop-operating-system.md` · `.claude/commands/loop-retro.md` | 已落；窗口历史包 25 行全部 `unknown` 或 `wall-clock-legacy`，`261020` 的 `unknownReason = no-lifecycle-data`；临时夹具下 `pod-started` / `pod-closed` 均 exit 0，非法 eventType exit 1 |
| T3 重复盖章：同 dispatchId + 零时长 + exact total 与同 id 非零行相等，三条件剔除 | `.claude/scripts/retro-efficiency.mjs` | 已落；窗口剔除 6 行 / 932,254 token，`totals.token` 26,980,469 → 26,048,215；id 复用 2 组只计数未剔除 |
| T4 fixer 根因：`--root-cause` 五值闭集、仅 fixer 记录、非法值 fail-open、非 fixer 记 ignored、无旗标不带键；读侧根因分布小表 | `.claude/scripts/dispatch-stamp.mjs` · `.claude/scripts/pod-events-append.mjs` · `.claude/scripts/retro-efficiency.mjs` · `.claude/agents/planner.md` · `.claude/agents/fixer.md` · `.claude/commands/loop-retro.md` | 已落；临时夹具验证 exact / invalid / ignored-non-fixer / 无键四态；窗口历史数据打印 `fixer 根因：0/11 派发带根因` |

## 2. 验证证据

- **§5 第 1 条（pre-commit）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-retro-efficiency-fidelity/PROPOSAL.md` → `ok: true`，`section2BodyFound: true`，`anchorCount: 9`，`findings: []`，`advisories: []`；`anchorUnion` 九成员与 landing-manifest 九行逐 token 一致。
- **§5 第 3 条（post-commit）**：`node .claude/scripts/retro-efficiency.mjs --self-check` → `self-check: 23/23 passed`，exit 0；新增九条具名断言全部在列（`named-only-drops-backsync`、`batch-split-ambiguous-dropped`、`event-sha-union-not-replace`、`shared-sha-forces-batch`、`tier-column-classification`、`lifecycle-window-unknown-not-zero`、`waiting-negative-unknown`、`restamp-dedupe-triple-condition`、`root-cause-enum-closed`）。
- **§5 第 4 条（post-commit）**：`node .claude/scripts/retro-efficiency.mjs --since 261013 --until 261023` → exit 0；主表恰为十列 `changeId / 档位 / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟`；行集 25 行；三个档位抽查值见 §1。
- **§5 第 5 条（post-commit）**：`--json` 下 `261013-lang-config-thinking-fix` 的 `codeLines=48`、`testLines=0`、`docLines=2424`；三个 `261022` 包的 `codeLines` 为 354 / 280 / 1,228，互不相等；三者的 `mergeAttribution[]` 各含 `017de34c`、`verdict:'batch'`、`shaSource:'merged-legacy'`；`261013` 的 `mergeAttribution[]` 含 `085c7427`（`verdict:'backsync'`，不计入）与 `4746cd4f`（`verdict:'named'`）；`batchUnattributedPaths` 17 条，文件账 74 = 57 归因 + 11 无主 + 6 歧义。
- **§5 第 6 条（post-commit）**：时间拆分表全部 25 行输出空观察窗或 `wall-clock-legacy`，`261020-p3-rec-frame-visual` 的 `unknownReason = no-lifecycle-data`；`--json` 中 25 个 Pod 的 `podStartAt` / `podCloseoutAt` 全为 `null`。
- **§5 第 7 条（post-commit）**：`--json.dedupe` 在窗口内 `restampedRows = 6`、`restampedToken = 932,254`；`totals.token = 26,048,215`，比改前同命令恰少 932,254；`ambiguousIdGroups` 2 组（均在 `261021-p3-record-cancel-lifecycle`），未被剔除。
- **§5 第 8 条（post-commit）**：`--json.fixerRootCause` 为空数组，markdown 打印「fixer 根因：0/11 派发带根因」，无 token 为 0 的根因行。
- **§5 第 9 条（post-commit）**：临时夹具 `CLAUDE_PROJECT_DIR=c:/tmp/pe-fixture` 下，`pod-events-append.mjs --event-type pod-started` 与 `pod-closed` 均 exit 0，`pod-events.jsonl` 各增一行且 eventType 正确；`--event-type nope` 实测 exit 1（不是 2）。
- **§5 第 10 条（post-commit）**：临时夹具 `CLAUDE_PROJECT_DIR=c:/tmp/ds-fixture` 下，`--actor fixer --root-cause acceptance-gap` → `rootCause: 'acceptance-gap'` + `rootCauseStatus: 'exact'`；`--root-cause nope` → exit 0 + stderr advisory + `rootCause: null` + `rootCauseStatus: 'invalid'`；`--actor builder --root-cause environment` → `rootCauseStatus: 'ignored-non-fixer'` 且无 `rootCause` 键；不传旗标 → 无键。
- **§5 第 11–12 条（post-commit）**：`grep -n "档位|时间拆分|根因" .claude/commands/loop-retro.md` 命中；`grep -n "pod-started|收口时点|起包时点|root-cause|根因" .claude/agents/{planner,integrator,fixer}.md` 命中。
- **§5 第 13 条（post-commit）**：`--json` 顶级键仍含 `ok / generatedAt / scope / coverage / totals / pods / roles / scriptRoles / unmappedActors / unmappedModels`，新增 `fixerRootCause` / `batchUnattributedPaths`；`pods[]` 既有 20 个键全部在场，新增 `tier / tierValue / mergeAttribution / attributionSources / mergeCount / multiMerge / podStartAt / podCloseoutAt / observationWindowMs / waitingMs / lifecycleSource / unknownReason / dedupe / fixerRootCause`。
- **§5 第 14 条（post-commit）**：`git diff --stat` 覆盖九个 manifest 文件（650 insertions / 50 deletions），无夹带 scope creep。
- **merge-driver 自检**：`node .claude/scripts/merge-driver.mjs --self-check-merge-friction` → 23/23 PASS，含新增 `T6 簿记 payload.mergeSha ≡ 调用前 HEAD 且 40 位`。

## 3. 诚实注记

1. **实现偏差（一处）**：FROZEN T4 写「枚举闭集（脚本内单源导出 `FIXER_ROOT_CAUSES`）」。实际落地把 `FIXER_ROOT_CAUSES` 与 `classifyFixerRootCause` 放在 `.claude/scripts/pod-events-append.mjs`（两脚本本来就共同依赖的事件信封工具），由 `dispatch-stamp.mjs` 与 `retro-efficiency.mjs` 共同导入。原因是 `retro-efficiency.mjs` 若静态导入 `dispatch-stamp.mjs`，后者依赖的 `lib/changeid-shared.mjs` 与 `lib/transcript-telemetry.mjs` 会在看到 `--self-check` 时自行输出并 `process.exit`，导致读侧 23 条自检无法执行。枚举仍是单一来源、两脚本共用同一实现；文件仍在九行 manifest 内，未新增文件。请核验官按「单一来源 + 九文件 manifest」判，不按函数物理位置判。
2. **N4–N6 的处理**：R2 判官留的三条 NIT 已在 v0.4 落件（T4「一次」改「合计」、§4 OQ4 标历史快照并指向现行 T1、T1 写死 `shaSource` 优先级 `merge-completed > merged-legacy > commit-message`）。判官明确「不阻冻结、随落件处理」，本轮未因此再开 R3。
3. **档位计数与起草期估计的差异**：起草期估「窗口 full 10 / lite 14 / nano 1」；落地后按当前 `taskpackage.json` 实测 full 9 / lite 15 / nano 1（拆包 5）。三个点名抽查值全部符合 FROZEN §5 第 4 条；计数差异不影响该条断言，已按现跑为准记录。
4. **未实跑项**：§5 第 16 条是「落地后下一次真实合并」的观察项（`merge-completed.payload.mergeSha` 在真实合并中出现、历史 1 条不被改写、`attributionSource` 两态），不是 landing 前置；本轮不制造真实合并去验证它。其余 §5 条目均已实跑或由临时夹具验证。
5. **历史数据不回填**：25 个历史 Pod 的生命周期字段一律 `null` / `unknown`；重复盖章只做读侧剔除，不追溯改写事件；`merged-legacy` 只读消费，不追认为 canonical 事件类型。
6. **`261013-p1-tomorrow-row-sliver-fix`** 仍随日期窗口进入表（上轮已析出），本轮不改其归属；如需从窗口剔除，属阅读口径，不改脚本。
7. **结构门 advisory 的两处解读**：`changedCanonicalNotInProposal` 列出 `docs/debt/gov/ARQ-261026-efficiency-table-package-tier.md` 与 `docs/debt/gov/INDEX.md`——那是上一个 commit `2517a8c4`（ARQ-261026 登记）相对 `HEAD~1` 的改动，不是本次 landing 的工作树改动；本次 staged 集不含它们。`verbatimQuoteAdvisory` 的两处 `not-found-verbatim-in-any-target` 是 PROPOSAL §2 的 git 历史引文（`085c7427` / `4746cd4f` / `017de34c` 的 subject 与 shortstat），它们是证据文本、本来就不出现在九个目标文件里；`git log` 已复核逐字一致。
8. **landing-verifier R1 `FAIL(F1)` 与修复**：判官指出 `manifestGlobToRegExp` 先展开 `**`、最后才替换 `*`，把 `.*` 改写成 `.[^/]*`，使 `dir/**` 只匹配一层；语料后果是 `017de34c` 的 `firmware/main/test/host_app/main/CMakeLists.txt` 从歧义掉进无主，实际为 12 无主 + 5 歧义。已改为占位符次序（`**/` / `/**` / `**` 先占位，`*` 替换后再恢复），`batch-split-ambiguous-dropped` 夹具补三层路径，移除旧 `findMergeShas` 死代码。复跑后 `batchUnattributedPaths` = 11 无主 + 6 歧义，与 FROZEN §5-5 一致；三包行数 354/280/1,228、`totals.token` 26,048,215、dedupe 6 行 / 932,254 token 均不变。此条由判官 R1 抓出，核验官独立结构生效。
9. **§5-6 与 T3 值域的张力**：§5-6 字面写「全部 25 行数据来源 = unknown」，但 T3 的 `数据来源` 值域包含 `wall-clock-legacy`；落地按 T3 实现（历史包 22 行 `wall-clock-legacy` + 3 行 `unknown`），未把历史近似伪装成 `lifecycle`。此处是 FROZEN 文本内部的措辞张力，不是实现偏离。

```landing-manifest
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/commands/loop-retro.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/retro-efficiency.mjs
```
