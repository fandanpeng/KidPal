# architect-landing-summary · 261026-gov-review-p2-anchor-template-cluster

packet-id：FROZEN PROPOSAL §2 改法集（T1–T7）+ §5 验证集 · 评审轮次 R1（GREEN，0 BLOCKING / 3 NIT 非阻断）· change-id gov-review-261026-gov-review-p2-anchor-template-cluster

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1（§2 边界探测跳围栏 ×3 处 + 围栏未闭合 fail-loud + 早退不吞 problems + 自检夹具） | `.claude/scripts/lib/governance-gate-shared.mjs`、`.claude/scripts/landing-fidelity-check.mjs` | 三处切片统一「flags 先算、边界探测跳围栏」；围栏未闭合 fail-loud 与早退 problems 保留落共享 lib；共享 lib 4 条 + preflight 1 条新夹具全 PASS |
| T2（verbatim-quote 比对前对块 trim） | `.claude/scripts/landing-fidelity-check.mjs` | `norm(block).trim()` 落地；4 条新夹具（含漂移正控）全 PASS |
| T3（landing-summary 结构两件义务进 face SSOT） | `.claude/references/arch-change.md`、`.claude/references/gov-change.md` | arch-change 新节「Track B 制品义务（landing-summary 结构两件 + 验证集实跑回填）」落地；gov-change Track B 后端句尾追加单源指针 |
| T4（验证集白名单/计数从处方正推） | `docs/templates/PROPOSAL.template.md` | 「验证集撰写三规则」硬项块第 2 条落地 |
| T5（负向断言配正向 happy-path） | `docs/templates/PROPOSAL.template.md`、`.claude/references/track-b-judge-brief.md` | 三规则第 3 条 + 判官简报透镜第 6 条「配对与实跑抽查」句落地 |
| T6（预期值实跑回填，扩至全部实测数字） | `.claude/references/arch-change.md`、`docs/templates/PROPOSAL.template.md`、`.claude/references/track-b-judge-brief.md` | 义务定义处（arch-change ② 段）+ 模板三规则第 1 条 + 判官抽查句三面齐落 |
| T7（release-pr gov-lane 门面，最小版） | `.claude/scripts/release-pr.mjs` | `extractGovChangeIds` / `derivePrTitle` 两新函数 + `buildPrBody` 整函数替换 + publishFlow lane 感知 + live/fake deps `reviewDiffNames` 两处 + 7 条自检夹具 |

## 2. 验证证据

逐条 = FROZEN §5 验证集落地期实跑（2026-10-26，gov worktree，commit 前）：

- **V1**：`node .claude/scripts/landing-fidelity-check.mjs --self-check` → 10 组全 `ok:true`，`section2Anchors 29` / `landing-preflight 20` / `verbatim-quotes 10`（与预期逐值一致，其余组与基线 B1 相同），exit 0。绿。
- **V2**：`node .claude/scripts/release-pr.mjs --self-check` → `{"selfCheck":"release-pr","pass":26,"fail":0,"total":26,"ok":true}`，exit 0；`extract-gov-changeids` / `gov-lane-title` / `gov-lane-body-pointer` / `gov-lane-no-zero-pod` / `dev-lane-title` / `unknown-lane-title` / `unknown-lane-body` 七条新夹具逐条 PASS。绿。
- **V3**：直探脚本（夹具 = 围栏含 `## ` 行 + 其后 T2 锚行）→ `{"union":["docs/a.md","docs/b.md"],"problems":[]}`（基线 B3 现行解析器缺 `docs/b.md`，负向/正向配对成立）。绿。
- **V4**：两份夹具各跑 `--preflight` → 正常锚行版 `ok:true`、`anchorCount:2`、`anchorUnion` 收两路径、`findings:[]`；加粗锚行版 `ok:false`、findings 含 `bold-anchor-line lines:[9]` 与 `missing-anchor-list`（点名「T2 · 乙 无『目标文件：』行」）——与预期逐值一致（基线 B4 两版均 `findings:[]`）。绿。
- **V5**：语料脚本（落地后解析器 × 76 份语料）→ `{"files":76,"filesWithProblems":6}`，6 份全部为存量既有 problems（4 归档旧批 + 2 模板骨架，B6 已证非本批引入）。绿。
- **V6**：白名单 13 条 `grep -cF` 逐条 = 1（基线 B9 全 0）。绿。
- **V7**：旧比对 `includes(norm(block))` = 0、新比对 `includes(norm(block).trim())` = 1、旧兜底字面 = 0；**第 4 条偏差**：新 gov 指针片段 `（制品见 delivery/review/` 落地实跑 = **2**（起草期预期 1）——见 §3 偏差申报。三条绿 + 一条覆写后绿。
- **V8**：`--preflight` 本批 PROPOSAL → `ok:true`、`anchorCount:7`、`anchorUnion` 与 manifest 七行逐条一致、`advisories:[]`、`findings:[]`。绿。
- **V9**：`git diff --name-only -- docs/contracts/ .claude/agents/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json` → 空输出。绿。
- **V10 / V11 / V12**：依赖 landing 提交与 verdict，由结构门、post-commit 复跑与升级闸承担，本 summary 不预填。

真空绿规约遵守：自检输出含新增自检名（`fence-boundary-*` / `verbatim-clause-*` / `extract-gov-changeids` / `gov-lane-*` 等逐条 PASS 行在案）；判别力由基线 B3/B4/B5 反转实测背书（现行实现下各断言变红的取值已在 FROZEN §5 逐条登记）；环境基线 B1/B2 起草期已立、无环境红。

## 3. 诚实注记

**偏差申报 1 条（非语义）**：V7 第 4 条「新 gov 指针片段」起草期预期 1，落地实跑 = 2。根因 = 字面 `（制品见 delivery/review/` 在 §2 处方新文出现两处（`buildPrBody` 实现行 + `gov-lane-body-pointer` 自检夹具行），起草期白名单只计实现行、漏计夹具行；按本批自立的 T4「白名单从处方正推」规则，落地期覆写 §5 预期为 2 并在 PROPOSAL §7 Landing 留痕段留痕。方向 = 断言锚点更多、更严，非放宽。

**R1 三条 NIT（非阻断，结项时处理）**：F1/F2 = census prose 口径订正（结项订正 ARQ 条目时按判官给的口径改写，不改本批制品）；F3 = 「围栏未闭合」全文件 fence 计数的跨节配对残余，§6 留根 2 已登记。

**留根去向**：§6 留根 1–8 全数保留（orphan-T 不做的复议触发、fence 奇偶例外、spec-anchor-check / contract-lint 不动、T7 检测面 = delivery/review/、ARQ-261021 机判口径改写、五处义务句漂移观察、T5 硬项标签、T7 两线皆空分支）；七条 ARQ 结项登记（含 260928/261021 并条的「前提已过时」注记）由主线程结项时执行；`ARQ-260924` 残余②零触碰。

**环内自律件**：architect 的 scratch 装配副本在仓外 `c:/tmp/p2-proto/`（未进仓、未 stage）；落地文本全部逐字取自 FROZEN §2，装配脚本路径不构成落地依赖。

```landing-manifest
.claude/references/arch-change.md
.claude/references/gov-change.md
.claude/references/track-b-judge-brief.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/release-pr.mjs
docs/templates/PROPOSAL.template.md
```
