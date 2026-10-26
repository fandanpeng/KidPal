# landing-verdict · 261026-gov-review-p2-anchor-template-cluster

verdict: PASS + 3 ADVISORY

核验官：landing-verifier（零上下文独立核验，2026-10-26，gov worktree，landing 未 commit 工作树）。判断全部来自亲跑脚本退出码 + 逐文件人读 diff，不采信 landing-summary 自述。

## 六项核验结论

### 1. 逐文件 diff 对照（7/7 语义匹配）

| 文件 | T | 结论 | expected（FROZEN §2） | actual（git diff） |
|---|---|---|---|---|
| `.claude/scripts/lib/governance-gate-shared.mjs` | T1-1/2/5 | ok | flags 先算 + 边界跳围栏 + 围栏未闭合 fail-loud + 早退不吞 problems + section2Anchors 4 夹具（插汇总行前） | 与处方逐字一致（一处注释 byte 偏差，见 ADVISORY-2）；problem 串、早退改写、夹具代码逐字匹配 |
| `.claude/scripts/landing-fidelity-check.mjs` | T1-3/4/5 + T2 | ok | section2Body 跳围栏 + checkVerbatimQuotes 切片改写 + preflight 1 夹具 + 比对 trim + verbatim 4 夹具（插 `verbatim-crlf-checked` 行后） | 六个落点逐字匹配；trim 判据 `norm(block).trim()` 在场；夹具插入位正确（未误落 citation 段） |
| `.claude/references/arch-change.md` | T3-1 / T6-1 | ok | 「轻路径落地通道」后、「触契约分支」前插新节，节体逐字 | 新节标题与节体逐字匹配，插入位置正确 |
| `.claude/references/gov-change.md` | T3-2 | ok | 「Track B 后端」末句句尾同行追加单源指针 | 追加内容与处方逐字一致（含行首空格接续） |
| `.claude/references/track-b-judge-brief.md` | T5 / T6-3 | ok | 透镜第 6 条替换（配对与实跑抽查句） | 逐字匹配 |
| `docs/templates/PROPOSAL.template.md` | T4 / T5 / T6-2 | ok | §5 末、「## 6」前追加「验证集撰写三规则」硬项块（三条） | 逐字匹配，位置正确 |
| `.claude/scripts/release-pr.mjs` | T7 | ok | 六落点：`extractGovChangeIds` / `derivePrTitle` 两新函数（插 `extractChangeIds` 后）+ `buildPrBody` 整函数替换 + publishFlow 第 8 段与 body 调用行 + live/fake deps `reviewDiffNames` 两处 + 7 条自检夹具（插汇总行前） | 全部逐字匹配；旧兜底字面已删、无残留 |

无私货：三个脚本与四份文档的全部 diff hunk 均可逐一对应到 §2 处方文本；无超出 packet 的改动（结构门 `unauthorized:[]` 与人工 diff 通读双确认）；无与批定矛盾处。

### 2. packet-binding 一致

- summary `packet-id` 行在场且同文引 change-id `gov-review-261026-gov-review-p2-anchor-template-cluster`。
- manifest 七文件 == FROZEN §2 各 T「目标文件」锚并集 == V8 preflight `anchorUnion`（7 项逐条一致）。
- summary 含 `landing-manifest` 围栏块，七行与 PROPOSAL 侧逐字一致。
- 条件锚裁剪：无（本批无 OQ 触发的锚裁剪）；ARQ-260924 残余②路径零触碰 ✓。

### 3. 验证集复跑（V1–V9 亲跑，全绿）

- **V1** `landing-fidelity-check.mjs --self-check`：10 组全 `ok:true`、exit 0；`section2Anchors 29` / `landing-preflight 20` / `verbatim-quotes 10`，其余组与基线 B1 逐值同（4/6/29/4/19/2/19）。✓
- **V2** `release-pr.mjs --self-check`：`{"selfCheck":"release-pr","pass":26,"fail":0,"total":26,"ok":true}`、exit 0；`extract-gov-changeids` / `gov-lane-title` / `gov-lane-body-pointer` / `gov-lane-no-zero-pod` / `dev-lane-title` / `unknown-lane-title` / `unknown-lane-body` 七条新夹具逐条 PASS。✓
- **V3** 直探脚本：`{"union":["docs/a.md","docs/b.md"],"problems":[]}`——与预期逐字一致（B3 现行版缺 `docs/b.md`，负向/正向配对成立）。✓
- **V4** 两夹具 preflight：正常版 `ok:true`、`anchorCount:2`、union 两路径、`findings:[]`；加粗版 `ok:false`、`bold-anchor-line lines:[9]` + `missing-anchor-list`（点名 T2 · 乙）——逐值一致（B4 现行两版均空）。✓
- **V5** 语料脚本：`{"files":76,"filesWithProblems":6}`——与预期一致。✓
- **V6** 白名单 13 条 `grep -cF`：逐条 = 1。✓
- **V7** 配对断言：旧比对 0 / 新比对 1 / 旧兜底字面 0 / gov 指针片段 **2**（覆写后预期，偏差申报核实为真：字面恰在 `release-pr.mjs:198` 实现行与 `:695` 自检夹具行，均出自处方新文，方向 = 断言更严）。✓
- **V8** 本批 PROPOSAL preflight：`ok:true`、`anchorCount:7`、union = manifest 七行、`advisories:[]`、`findings:[]`。✓
- **V9** 禁触面 diff：空输出。✓
- **V10**（landing 期）= 本核验复跑的结构门：`ok:true`、`proposalTargets`/`summaryTargets` 各 7、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`、exit 0。✓
- V11 / V12 依赖落地提交与 verdict 在场，按 FROZEN §5 由 post-commit 复跑与升级闸承担，不在本 verdict 判定域。

### 4. 结构门复跑

`node .claude/scripts/landing-fidelity-check.mjs delivery/review/261026-gov-review-p2-anchor-template-cluster` → `ok:true`、`frozen:true`、`packetBinding:true`、exit 0。Track B 证据集无 decision-packet，`proposal-consolidation-check.mjs` 不跑（N/A）。git 未降级（`gitDegraded:false`）。

### 5. 真空绿规约

- 自检输出含全部新增自检名（`fence-boundary-*` / `genuine-boundary-still-terminates` / `unclosed-fence-fail-loud` / `verbatim-clause-*` / `verbatim-fence-boundary-checked` / `extract-gov-changeids` / `gov-lane-*` / `dev-lane-title` / `unknown-lane-*` 逐条 PASS 行亲见）；调用顺序审查通过（exit 型自检均在各组汇总行前，未挡后续组）。
- 判别力由 FROZEN §5 基线 B3/B4/B5（现行实现反转取值）背书；本核验 V3/V4 实跑复现了「现行缺 / 落地收」的双侧取值。
- 环境红基线：无环境红（B1/B2 起草期已立、本核验复跑同值）。

### 6. 诚实披露核对

- 偏差申报 1 条（V7 第 4 条预期 1 → 实跑 2）：**核实为真**（两处落点亲见，均出自处方新文；PROPOSAL §7 Landing 留痕段与 §5 V7 行覆写在案，冻结纪律执行正确）。
- R1 三条 NIT 转记与 §6 留根 1–8 保留声明与 FROZEN 文本一致。
- 无该记未记项。

## 主线程已知事项交叉核结论

- **V7 gov 指针 2 处**：属实（见上）。
- **verbatim-quote 观察列 missing 条目**：advisory-only，不阻断；独立核其解释成立——本批 PROPOSAL §2 的 js 夹具串内含 ``` 字面（如 `'```md'` 数组元素），`checkVerbatimQuotes` 的围栏正则 `` /```[a-zA-Z]*\n([\s\S]*?)```/g `` 在这些行提前闭合，块被错切；missing 列内容为错切散文片段与「旧文已替换、目标侧应消失」的应有形态，非落地缺陷。记 ADVISORY。
- **`changedCanonicalNotInProposal` 四文件**：`delivery/workstreams/**` = 主线程本批立项/进度记账（workstream 状态行 + progress/task_plan 各 1 行），非 canonical 落地面；另 `delivery/review/INDEX.md`（本批 frozen 行）与 `.codebase-memory/` 两件（自刷新再生面，仓规不追交）同为账本/再生类。性质核过，非夹带。

## ADVISORY 清单（非阻断）

1. **verbatim-quote 观察列 missing 条目**（结构门 advisory 列）：如上，根因 = js 夹具串内含 ``` 字面致围栏提取错切；检查本身 advisory-only，不进门判定。留观察，不要求返工。
2. **一处注释 byte 级偏差**：`governance-gate-shared.mjs:380` 注释写「fence 定界**行为**奇数」，FROZEN §2 T1 改法 1 处方写「fence 定界行**总数**为奇数」。仅注释、零语义差；同函数的 problem 串（:382）与处方逐字一致。可在后续触碰该文件时顺手对齐，不要求本批返工。
3. **非 manifest 工作树改动**：`delivery/workstreams/**` 四件 + `delivery/review/INDEX.md`（记账）+ `.codebase-memory/` 两件（自刷新、不追交）——性质已核为账本/再生类，非本批 canonical 落地面，merge 时注意与批改动的提交切分即可。

## 放行建议

landing 实际落地 == FROZEN PROPOSAL §2 改法集（T1–T7）+ §5 验证集，语义保真成立、无私货、无漏改、无禁触面侵入。**建议放行 commit 与后续闸程**；merge 仍归 human manual-merge 门（V11/V12 由 post-commit 复跑与 `pre-merge-escalation-gate.mjs` 承担）。
