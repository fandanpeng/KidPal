# landing-verdict · 261026-gov-review-retro-efficiency-fidelity

verdict: PASS + 4 ADVISORY

- 判定：**PASS**。R1 的 F1（`manifestGlobToRegExp` 方言替换次序 bug + summary 拆分数字失实）已按验收点修复，复验全绿；merge 放行建议（merge 仍归 human 门）。
- 判定史：R1 `FAIL(F1)`（本文件历史版，见文末「R1 判定记录」）→ 主线程修复（占位符次序 + 多层夹具 + 死代码移除 + summary §3 第 8/9 条订正）→ R2 复验通过。

## R2 复验记录（F1 修复验收，2026-10-26，全部亲核）

| 验收点 | 结果 |
|---|---|
| ① `manifestGlobToRegExp` 占位符次序落地 | 现文：转义非 glob 特殊符 → `**/`→`\u0000`、`/**`→`\u0001`、`**`→`\u0002` → `*`→`[^/]*` → 恢复 `(?:.*/)?` / `(?:/.*)?` / `.*`。独立八组语义用例全过：`dir/**` 匹配零层（`dir`）与多层（`dir/a/b/c.c`）；`**/x.c` 匹配零层（`x.c`）与多层（`a/b/x.c`）；`a/**/b` 跨层（`a/b`、`a/x/y/b`）；`*` 不跨 `/`（`diagnostics/*` 对 `diagnostics/sub/log.c` 不命中）。与 FROZEN §2 T1 方言规格逐条相符 |
| ② 多层夹具真判别（反转必红） | 用旧实现（替换次序 bug 版）独立重放新夹具三条多层路径（`shared/deep/nested/intersection.c` / `a/deep/nested/only.c` / `b/deep/nested/only.c`）：旧实现全部不命中 → 夹具断言必红；新实现全命中 → 绿。判别力成立 |
| ③ 语料复跑（--json） | `batchUnattributedPaths` = **17 条，no-manifest 11 + ambiguous-manifest 6**——与 FROZEN §5-5 断言逐数一致；`firmware/main/test/host_app/main/CMakeLists.txt` 现为 `ambiguous-manifest`、candidates = 两包（诊断闭合） |
| ④ 修复不改变行数/token | 三包行数 354/0/1,096、280/0/1,585、1,228/961/1,403 不变；`totals.token` 26,048,215 不变；dedupe 6 行 / 932,254 不变；`261020` 仍 `no-lifecycle-data` |
| ⑤ 死代码移除无悬空 | `findMergeShas` 已删；全仓 grep 零引用；`node --check` 通过；九条新夹具 + 旧十四条 `--self-check` 23/23 passed、exit 0 |
| ⑥ summary 订正 | §1 T1 行与 §2 第 5 条现为实测值「11 无主 + 6 歧义」（与 --json 实跑一致）；§3 新增第 8 条如实记录 R1 `FAIL(F1)` 与修复；第 9 条如实点名 §5-6 与 T3 值域的 FROZEN 内部措辞张力（历史包 22 行 `wall-clock-legacy` + 3 行 `unknown`，按 T3 值域实现，未伪装成 `lifecycle`）——R1 的 A2 披露缺口已闭合 |
| ⑦ 结构门复跑 | `landing-fidelity-check.mjs` → exit 0，`ok:true / frozen:true / packetBinding:true`，`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 均空；canonical diff 仍恰九 manifest 文件（661 insertions / 71 deletions，含 `findMergeShas` 删除） |

## 六项核验清单逐项结论（R1 全量核验 + R2 复验合并结论）

### 1. 逐文件 diff 对照 — ok

| 文件 | 结论 | 依据 |
|---|---|---|
| `.claude/agents/fixer.md` | ok | +1 行：返回时申报根因类别（申报非判分），与 T4 逐字同义 |
| `.claude/agents/integrator.md` | ok | Step 9 增 `pod-closed` 写入点（closeout-check 后、supervisor 闸前、payload `{}`、fail-open），与 T3 一致 |
| `.claude/agents/planner.md` | ok | Step 5 增 `pod-started` 写入点 + [DISPATCH-STAMP] 模板与 Step 10 增 `--root-cause` 五值枚举；无门判据、无写域变化 |
| `.claude/commands/loop-retro.md` | ok | 九列→十列（档位插 changeId 后）+「效率表三张」+ fixer 根因小表 + 数据缺口补四项，与 T2/T3/T4 一致 |
| `.claude/references/dev-loop-operating-system.md` | ok | §5d 闭集补 `pod-started`/`pod-closed` + non-bearing 句；§6 指针句同步 + unknown 不回退句，与 T2/T3 一致 |
| `.claude/scripts/dispatch-stamp.mjs` | ok | 导入 `classifyFixerRootCause`，payload 展开 rootCause 三态（exact / invalid+advisory / ignored-non-fixer），不传不带键；与 T4 逐条同义 |
| `.claude/scripts/merge-driver.mjs` | ok | payload 仅加 `mergeSha` 一键；try/catch advisory 语义逐字未动；T6 夹具新断言在 `step5bBookkeeping` 调用**前**取 HEAD（N2 落实）且断言 40 位 |
| `.claude/scripts/pod-events-append.mjs` | ok（披露偏差判可接受，ADVISORY A1） | 闭集 +2 值；`FIXER_ROOT_CAUSES` 五值 frozen + `classifyFixerRootCause` 单源导出 |
| `.claude/scripts/retro-efficiency.mjs` | ok（R2 复验后） | T1 subject 分类器 / 三级切分 / 三来源并集 / shaSource 优先级 / guard / T2 档位 / T3 生命周期与三条件剔除 / T4 根因聚合全部按 FROZEN 落；R1 的 `manifestGlobToRegExp` 方言偏离已修复（见 R2 复验表①–④） |

超出 manifest 的 canonical 改动：**无**（canonical diff 恰九文件；`.codebase-memory` 两件为可再生常态脏、`.changes-index-state.json` 仅 261025 批残留条目，见 ADVISORY A6）。

### 2. packet-binding 一致 — ok

- packet-id 行 = FROZEN v0.4 §2 改法集 + §5 验证集；summary 侧 `landing-manifest` 围栏块九行与 PROPOSAL manifest、结构门 `proposalTargets`/`summaryTargets` 三方逐 token 一致。
- T2 五件组件、T3 六件、T4 五件组件均按 FROZEN 组件清单落齐。
- summary 验证证据段经 R2 逐条对表：原「11 无主 + 6 歧义」失实已订正为实测值；其余声称与亲跑输出一致。

### 3. 验证集复跑 — ok（R1 亲跑 + R2 修复后复跑）

| §5 条 | 结果 |
|---|---|
| 1（preflight/结构门） | ok:true / anchorCount:9 / findings:[]（R2 复跑仍绿） |
| 2（基线 14/14） | 由改后 23/23 含全部旧断言名承载；旧十四条全部在列且绿 |
| 3（23 夹具） | **亲跑（R2 修复后）：`self-check: 23/23 passed`、exit 0**；多层路径补强后 `batch-split-ambiguous-dropped` 对旧实现必红（独立重放证实） |
| 4（十列/25 行/档位抽查） | **亲跑：exit 0**；表头恰十列；25 行；`261016-p3-story-body-sd-cache=full`、`261020-p3-rec-frame-visual=nano`、`261022-m5-n2a-schedule-mechanical-injection=lite·拆包`；全表 governance 0 行 |
| 5（T1 数值） | 261013 = 48/0/2424 ✓；三 261022 包 354/280/1228 互不相等 ✓；`mergeAttribution` 各含 `017de34c` verdict=batch shaSource=merged-legacy、`attributionSources` = [commit-message, merged-legacy] ✓；261013 含 085c7427 backsync（不计入）+ 4746cd4f named ✓；`batchUnattributedPaths` 17 条 = **11 无主 + 6 歧义**（R2 修复后达标）✓；文件账 74 = 57 + 17 ✓ |
| 6（T3 时间拆分） | --json：25 Pod `podStartAt`/`podCloseoutAt` 全 null ✓、261020 `unknownReason=no-lifecycle-data` ✓（markdown 表与 §5-6 字面的张力属 FROZEN 内部措辞问题，summary §3 第 9 条已如实点名，见 R2 复验表⑥） |
| 7（重复盖章） | `restampedRows=6`、`restampedToken=932254`、`totals.token=26048215`（恰少 932254）✓；`ambiguousIdGroups` 2 组均在 261021、未剔除 ✓ |
| 8（fixer 根因空） | `fixerRootCause` 顶层数组空、逐 Pod 空 ✓；markdown 打印「fixer 根因：0/11 派发带根因」✓、无 token=0 根因行 ✓ |
| 9（pod-events 夹具） | **亲跑（c:/tmp/pe-fixture 重建）**：pod-started / pod-closed 均 exit 0 且落行 ✓；非法类型 `pod-open` → **exit 1**、报文列现行八值（含两个新值）✓ |
| 10（root-cause 夹具） | **亲跑（c:/tmp/ds-fixture 重建）**：fixer+合法值 → `rootCause:"acceptance-gap"` + `exact` ✓；`nope` → exit 0 + stderr advisory + `rootCause:null` + `invalid` ✓；builder+值 → `ignored-non-fixer` 且无 rootCause 键 ✓；不传 → 两键全无 ✓ |
| 11–12（grep） | 档位/时间拆分/根因/效率表三张在 loop-retro.md :45 命中；planner/integrator 的 pod-started/起包/收口、planner/fixer 的 root-cause/根因均命中 |
| 13（向后兼容） | 顶级 10 既有键 + pods[] 20 既有键全部在场；主表九旧列名与顺序不变 ✓ |
| 14（落地集 ≡ manifest） | canonical diff 恰九文件（R2 后 661/71），无夹带 ✓ |
| 15（merge-driver 自检） | **亲跑：23/23 PASS、exit 0**，含新断言「T6 簿记 payload.mergeSha ≡ 调用前 HEAD 且 40 位」 |
| 16（合并后观察） | 未跑——属落地后下一次真实合并观察项，非 landing 前置；summary 已如实披露，合规 |

### 4. 结构门复跑 — ok（R1、R2 各跑一次均 exit 0）

`node .claude/scripts/landing-fidelity-check.mjs delivery/review/261026-gov-review-retro-efficiency-fidelity` → `ok:true / frozen:true / packetBinding:true`，`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 均空。两处 advisory 亲核均如 summary §3 第 7 条所述：`docs/debt` 两文件属 commit `2517a8c4`（`git diff HEAD~1 HEAD --stat` 证实，本次工作树不含）；verbatim 引文系 §2 的 git 历史证据文本（subject 与 shortstat 亲核逐字一致），本就不在九个目标文件里。

### 5. 真空绿规约 — ok

- 重放类：本批无重放脚本；各断言输入非空（夹具有实料、批量切分走真实语料 74 文件）。
- 接线类：T6 夹具新断言 grep「payload.mergeSha ≡ 调用前 HEAD」在场且通过；pod-events / dispatch-stamp 夹具为行为断言。
- 多层夹具补强后，`batch-split-ambiguous-dropped` 对 R1 的 bug 实现已证实可反转（R2 复验表②）——真空绿风险闭合。

### 6. 诚实披露核对 — ok（R2 后）

- 披露偏差（FIXER_ROOT_CAUSES 落位）：核实成立、判可接受（见 A1）。
- N4–N6 v0.4 落件、无 R3：R2 判官已明示「不阻冻结、随落件处理」，披露一致。
- 档位计数 full 9 / lite 15 / nano 1：实跑一致，披露属实。
- R1 抓出的「11+6 失实」已订正为实测值（summary §1/§2）；修复过程已记录（summary §3 第 8 条）；§5-6 张力已如实点名（第 9 条）。无其它该记未记。

## ADVISORY 清单（非阻断）

- **A1**（披露偏差判可接受）：`FIXER_ROOT_CAUSES` / `classifyFixerRootCause` 落在 `pod-events-append.mjs` 而非 dispatch-stamp——单一来源成立、导入链自毁前提亲核属实（`lib/transcript-telemetry.mjs:376` 在 argv 含 `--self-check` 时 `process.exit`）、文件在九行 manifest 内。按「单一来源 + 九文件 manifest」判，不是 FROZEN 语义偏离。
- **A4**：integrator.md 原句「261015 批 T5 齿」随触碰改写为「T5 子项」——属输出标准「存量文档触碰时顺手改写」的合规顺手改，非夹带。
- **A5**：dispatch-stamp.mjs `flagVal` 后多一空行（纯格式，未随 F1 修复清理）。
- **A6**：工作树另有 `.codebase-memory/artifact.json` + `graph.db.zst`（可再生常态脏）与 `delivery/review/.changes-index-state.json`（261025 批 mergedInMain 回写残留）——均非本批 packet 内容，无 canonical scope creep。
- **A7**：§5 第 16 条为合并后观察项（真实合并产出 `mergeSha`、历史 1 条不被改写、并集语义），landing 不前置验证；主线程在下次真实合并后按 FROZEN 断言观察即可。

（R1 曾列的 A2「§5-6 张力未点名」已由 summary §3 第 9 条闭合；A3「`findMergeShas` 死代码」已随 F1 修复消解——均不再列为在案 advisory。）

## 亲核留痕

- **R1（全量）**：`git status --short` + `git diff HEAD --stat`（canonical 恰九文件）；九文件 diff 逐行人读（retro-efficiency 791 行 diff 逐段比对 FROZEN T1–T4 条款）；`--self-check` 23/23；`--since 261013 --until 261023` markdown + --json 全量对表；独立复算批量切分（一次性 node 脚本、标准方言占位符实现，非复用被验代码）三方对账钉死 R1-F1；`merge-driver.mjs --self-check-merge-friction` 23/23 PASS；两夹具重建四态验证；`git log`/`--shortstat` 逐字核对三 SHA 引文；§5 第 11/12 条 grep 全命中。
- **R2（修复验收）**：读修后 `manifestGlobToRegExp` 源码；独立八组方言语义用例全过（含零层/多层/跨层/单段四方向）；旧实现独立重放新夹具三条多层路径全不命中（反转判别成立）；`--self-check` 复跑 23/23 exit 0；`--json` 复跑 `batchUnattributedPaths` 17 = 11 + 6、host_app 文件 ambiguous（candidates 两包）、三包行数与 token/dedupe/lifecycle 全不变；全仓 grep `findMergeShas` 零引用 + `node --check` 通过；summary §1/§2/§3 第 8/9 条与实跑逐条对表；结构门复跑 exit 0。
- `grep -n "self-check" .claude/scripts/lib/transcript-telemetry.mjs` → :376 `process.exit`（A1 前提核实）。

## 判定依据重申

- F1 修复后，落地与 FROZEN v0.4 §2 改法集（T1–T4）+ §5 验证集语义一致；无私货、无 scope creep、无未授权 canonical 改动；结构门与 packet-binding 绿。
- PASS = 放行建议；merge 仍归 human manual-merge 门。§5 第 16 条（合并后观察）由主线程在下次真实 Pod 合并后按 FROZEN 断言核验，不影响本 verdict。

---

## 附：R1 判定记录（历史，保留备查）

R1 verdict: FAIL(F1) —— `manifestGlobToRegExp` 先展开 `**` 家族（插入含 `*` 的展开串）、最后才把 `*` → `[^/]*`，把 `.*` 改写成 `.[^/]*`，`dir/**` 只匹配一层（实测正则源 `^firmware\/main\/test(?:\/.[^/]*)?$`）；语料后果：`017de34c` 切分中 `firmware/main/test/host_app/main/CMakeLists.txt` 本应歧义（两包 manifest 均含 `firmware/main/test/**`）落为无主，`batchUnattributedPaths` 实际 12 无主 + 5 歧义 ≠ FROZEN §5-5 断言 11 + 6（总数 17、归因 57、逐包 18/20/19、三包行数均不受影响）；且 summary §1/§2 把「11 无主 + 6 歧义」当实测输出转写。category = FROZEN 改法语义偏离 + 验证证据失实；根因 = 替换管线次序。R1 已显式标注升级 human（履行升级线义务，不涉方向分叉）。
