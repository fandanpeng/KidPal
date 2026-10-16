# landing-verdict · 260920-gov-review-reader-scripts-align

**Verdict：PASS + 1 ADVISORY**（非阻断；merge 归 human manual-merge 门）

- change-id：260920-gov-review-reader-scripts-align
- 核验时点：2026-09-20 · worktree `C:/AI/KidPal-gov`（branch gov/kidpal）
- 独立性：零上下文重推导——不采信 landing-summary 自述；两处 census、V1–V5、结构门均为本核验官亲跑。
- 结构形态：按新模板（260920-gov-review-proposal-structure-merge 后首批）核——PROPOSAL 无 §1、每 T 五段固定式，与制品实况一致。

## 六项核验逐项结论

### 1. 逐文件 diff 对照 —— PASS（无私货）

| 文件 | 结构改动 | 语义核对（实际 diff ↔ FROZEN §2） |
|---|---|---|
| `.claude/scripts/brief-inputs.mjs` | +31/-4 | ① `isMerged` 重构为 `isMerged(changeId, dir = projectDir)`——两处既有调用点（`:258` findPrecedents、`:414` self-check mergedProbe）均单参调用、走默认 `projectDir`，**零变更**（`grep -n "isMerged("` 亲核，全部 4 处命中均为定义/自检/单参调用）；② 第三判定源逐字落在 pod-events 扫描循环内：`/"eventType"\s*:\s*"human-decision"/` **且** `/"decision"\s*:\s*"[^"]*approve-escalated-merge/` 双条件命中才 return true——与 §2 T1 改法（同行双条件）一致，方向 fail-closed（只增识别形态）；③ `--self-check` 双夹具：merged decoy（`human-decision` + `approve-escalated-merge`）**入选**、open decoy（`some-other-decision`）**不入选**、无事件文件 → false，且 `ok('isMerged 夹具非空…')` 先于三条 isMerged 断言（先断非空）；④ imports 增 `mkdirSync`/`mkdtempSync`/`rmSync`（node:fs）+ `tmpdir`（node:os），均被夹具使用。无 child_process 引入（与「不采用 git log --merges」的改法声明一致）。 |
| `.claude/scripts/dispatch-stamp.mjs` | +5/-0 | `const model = flagVal('--model') \|\| null;` + payload 条件展开 `...(model ? { model } : {})`——传了才带键、不传不带（亲跑探针两态证实）；仅落在 post-cutoff pod-events 分支，legacy pre-cutoff 路径未动。`grep -c "flagVal('--model')"` = **1**。 |

**私货扫描**：两文件 diff 逐行读毕，内容仅含上述声称改动 + 一行插入性空行（打印循环前，T1 夹具块插入的伴随空白，无语义）；全树 `git status --porcelain` 中 canonical 面恰为 manifest 两路径 + codebase-memory 两件机器再生面（常态脏、批外）+ 本批 untracked summary。**无越界、无夹带、无权限/写域扩张**（G3 两脚本，契约面/G1/G2/frontmatter 零触碰）。

**第三判定源真命中实况（亲跑 census，非转述）**：对真实仓枚举 `delivery/taskpackages/*/pod-events.jsonl`（候选口径 = `acceptanceTestsFile` 非空数组、排除 `current/`、排除自身）：

```
candidates: 34 | twoSource: 21 | withEscalatedMerge: 24 | netGain: 7
netGain pkgs: 260829-home-p1, 260829-schedule-p2, 260829-session-logic,
260829-smoke-e2e, 260829-voice-roundtrip, 260903-device-ws-switch, 260907-g-relay-fix
```

与 §2 T1 证据段「34 / 21 / 24 / 净增益 7」**逐数吻合**、7 包清单**逐字一致**（R2-F5 更正后的口径成立）。`grep -rln "approve-escalated-merge"` 独立复跑 = **24 包**，吻合。

**两处缩圈的实测地基（逐项亲核，均站得住）**：

- **子项 b 缩圈**：`buildTokenUsage()`（`dispatch-stamp.mjs:99-156`）四 flag `--token-input/output/cache-read/cache-write` 机制确在（亲读源码）；全仓现跑 `dispatch-completed` = **195 条、`payload.model` 0 条、exact 128 条、exact 带非零分项恰 4 条**、样例 `{"status":"exact","source":"harness","input":199198}` 逐字命中——与 FROZEN 时点注记（起草 192、R1 复跑 195/128）一致，承重判据「model 0 条 + 调用方未喂值」成立 ⇒ 移入 §6 留根②有据。
- **launchPreconditions 移出**：`delivery/workstreams/m1-home-qa/decision-coverage.json` 的 `launchPreconditions` 实测 = **6 条纯字符串数组**（无「核过没」位、无「属哪个包」位）；门 1 结论确在另一文件 `review.md`（`## 门 1（拆包 review 门）—— 2026-09-15`，脚本不读）；同文件 `decisionCoverage` 带 `landsIn` 者 15 条（先例在）；`260915-m1-k2-keybar-three-slot/taskpackage.json` 的 `origin.workstream = "m1-home-qa"` 实测命中 ⇒ §6 留根①「先定数据模型」有据。

### 2. packet-binding 一致 —— PASS

- summary packet-id 行 =「FROZEN PROPOSAL v1.0 §2 改法集（T1–T2）+ §5 验证集 · R1 BLOCK(F1..F4) → R2 GREEN · change-id 260920-gov-review-reader-scripts-align」↔ FROZEN §2+§5 一致。
- manifest（PROPOSAL 与 summary 双侧，各 2 路径）≡ §2 锚清单并集（T1 brief-inputs.mjs + T2 dispatch-stamp.mjs）；结构门 `proposalTargets == summaryTargets == 2 路径` 机判吻合。

### 3. 验证集复跑（§5 亲跑）—— PASS

| 项 | 本核验官亲跑结果 | 与 summary §2 比对 |
|---|---|---|
| V1 | `brief-inputs.mjs --self-check` → **23/23 passed**、SELF-CHECK GREEN、exit 0；四条新夹具**逐行具名**：`isMerged 夹具非空（两 decoy 的 pod-events 均在盘）` / `isMerged ESCALATE+人批 decoy 须入选（第三判定源）` / `isMerged 其他 human-decision 不入选（decision 词面不含 approve-escalated-merge）` / `isMerged 无事件文件 → false` | 一致（23/23、四名在场） |
| V2 | ① `grep -c "flagVal('--model')"` = 1；② temp projectDir + `--task-package-id 260920-vprobe-a`（≥260725 前缀）带 `--model` 跑 → tempDir `pod-events.jsonl` 该条含 `"model":"<id>"`（回显值逐字复验）✓；③ 不带 `--model` 跑（vprobe-b）→ `"model"` 计 0、payload 不带该键 ✓；`rm -rf` 后 `[ ! -d ]` 断言成立、`git status --porcelain delivery/taskpackages/` = 0 行（真实仓零写副作用） | 一致（三路 + 清理断言全过） |
| V3 | `route-regression-check.mjs` → **28 ok, 0 fail**；`lint-descriptions.mjs` → exit 0、仅既有 `landing-verifier.md` 超长告警（363>320） | 一致 |
| V4 | `git diff --stat -- .claude/scripts/` → **恰两路径**（brief-inputs.mjs 34 行 / dispatch-stamp.mjs 5 行），porcelain 同 | 一致 |
| V5 | `landing-fidelity-check.mjs <本批>` → **ok:true、exit 0** | 见第 4 项 |

### 4. 结构门复跑 —— PASS

`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-reader-scripts-align` → `{"ok":true, frozen:true, packetBinding:true, unauthorized:[], claimedNotLanded:[], plannedNotLanded:[], gitDegraded:false}`，exit 0。`citationAnchorAdvisory.problems` 恰 **1 条**（`review.md:5` → file-not-exist）——与 summary §3-4 披露一致（PROPOSAL §6 留根①冻结稿短形态引据，门照设计只观察不产 fail；跨批短形态第 4 例）。

### 5. 真空绿规约 —— PASS

- V1 新夹具**逐行具名**（四名全部出现在逐行输出，非只进总数）；夹具**先断非空**（代码序：`ok('…夹具非空…')` 在三条 `isMerged` 断言之前，且在 `const failed` 打印循环之前——summary §3-1 自述的「首版插错位已移正」与当前实况相符）。
- V2 探针**事件确落 temp dir**（可观察对象存在：envelope/payload 直接可读）；探针包 id 带 ≥260725 日期前缀（post-cutoff pod-events 分支，R2-F6① 前置满足）；清理断言成立。
- 无空输入假绿：census 先确认候选集 34 非空、探针先确认事件文件在盘再断言。

### 6. 诚实披露核对 —— PASS（四条均与实际一致，无该记未记）

1. 夹具块首版插错位已移正——当前代码序与输出均满足「具名 + 先断非空」，披露与实况相符（历史过程不可独立复验，但现态合规即披露有效）。
2. 净增益口径 4/19 → 7/34——本核验官独立 census 复现 7/34 与 7 包清单。
3. census 时点注记——本核验官复跑 195/0/128/4，与注记的 R1 复跑数吻合；按「model 0 条」口径判据成立。
4. citation advisory 1 条——结构门亲跑复现同一条。

## 亲核留痕（关键命令）

- `git diff`（两文件逐行读毕）；`grep -n "isMerged(" brief-inputs.mjs`（4 处：定义 `:210`、调用 `:258`/`:414`、夹具 `:445-447`）
- census 脚本（node -e，独立重写不复用落地者代码）→ `34/21/24/7` + 7 包名
- `grep -rln "approve-escalated-merge" delivery/taskpackages/*/pod-events.jsonl | wc -l` → 24
- dispatch-completed 全量普查 → `195 / model 0 / exact 128 / 非零分项 4`
- `node .claude/scripts/brief-inputs.mjs --self-check` → 23/23 GREEN exit 0
- V2 探针（mktemp -d + CLAUDE_PROJECT_DIR 两态 + rm -rf + 断言）→ 全过、真实仓零污染
- `node .claude/scripts/route-regression-check.mjs` → 28 ok 0 fail；`node .claude/scripts/lint-descriptions.mjs` → exit 0（仅既有告警）
- `git diff --stat -- .claude/scripts/` → 恰两路径
- `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-reader-scripts-align` → ok:true exit 0
- 兼容性抽查：`planner.md:60` [DISPATCH-STAMP] flag 集不含 `--model`（可选 flag 零影响）；`pod-events-append.mjs:59-60` payload 仅验 object、`pod-events-aggregate.mjs` 守卫式属性访问（增/缺 `model` 键均无破坏）；`planner.md:59` MODEL-TIER 条款「dispatch-stamp 若可得须补记实际生效模型」逐字在场（T2 义务锚实）

## ADVISORY 清单（非阻断）

1. **citationAnchorAdvisory 1 条**：`review.md:5` → file-not-exist（PROPOSAL §6 留根①冻结稿的短形态引据，落地者不可改冻结稿；门只观察不产 fail）。建议随「citation 候选域」批次一并评估——与 summary §3-4 披露一致，非本批新增。

## 结论

实际落地 diff == FROZEN PROPOSAL §2 改法集（T1–T2）+ §5 验证集，两处缩圈均有实测地基，无私货、无漏改、无与批定矛盾。**放行建议：进入下一步（retro 闭环 + 前置升级闸 → human manual-merge 门）**，merge 拍板归 human。
