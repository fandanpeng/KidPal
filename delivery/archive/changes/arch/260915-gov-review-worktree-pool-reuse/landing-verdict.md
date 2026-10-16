# landing-verdict · 260915-gov-review-worktree-pool-reuse

**判定：PASS——结构门与语义面双绿，放行进 human merge 门（merge 决定仍归 human）。**

- 核验对象：分支 `gov/260915-worktree-pool-reuse`，landing 本体 @ 7bba2ac + 结构门返修 @ 5ae0874（自 main @ 0551bf0，共 2 commit）
- packet-binding：landing-summary packet-id = FROZEN PROPOSAL §2（T1–T6）+ §5 验证集，change-id 一致；landing 实际 canonical 范围 == manifest 六件，无 scope creep
- Track B：证据集无 decision-packet，`proposal-consolidation-check.mjs` 不跑（N/A，符合门规约）

## 1. 结构门（机器）

**首验（@ 7bba2ac）**：exit 1——PROPOSAL §2 六条锚行为 `**目标文件：…**` 加粗形态，`parseSection2AnchorLists`（`.claude/scripts/lib/governance-gate-shared.mjs:345`）只认行首裸「目标文件：」，fail-loud 判六条缺锚（实质锚集与 manifest 恒等，纯格式偏离；全仓 260830 起先例均裸形态）。判 FAIL 退回主线程。

**返修复核（@ 5ae0874，独立重跑）**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260915-gov-review-worktree-pool-reuse main` → **exit 0，ok:true**。

- frozen: true、packetBinding: true
- proposalTargets == summaryTargets == manifest 六文件，双向吻合
- unauthorized / claimedNotLanded / plannedNotLanded 全空（baseRef=main；注意脚本默认 baseRef=HEAD~1 在返修 commit 叠加后须显式传 landing 前 ref，此为调用方式非门缺陷）
- citationAnchorAdvisory：1 条 advisory（`SKILL.md:56` 裸文件名 file-not-exist）——ARQ-260906-lfc-verbatim-crlf-citation-gap 在册解析盲点族，advisory 级不阻断
- lint-descriptions advisory：1 条既有 warn（landing-verifier.md 超长，非本批引入）；verbatimQuoteAdvisory 空；changedCanonicalNotInProposal 空

**返修 commit 逐行核对**（`git diff 7bba2ac..5ae0874`）：PROPOSAL.md 仅六锚行去 `**` 包裹（零语义变更）+ architect-landing-summary.md 补 §3 注记 5（坑节「传染条」连带补报）与注记 6（结构门返修登记）；canonical 六文件零触碰，语义面判定不受返修影响。

## 2. 逐文件语义核对（人读 diff，`git diff main..gov/260915-worktree-pool-reuse`，@ 7bba2ac 判定、5ae0874 未触 canonical 故沿用）

| 文件 | 结构动 | 语义核 | expected（FROZEN §2） | actual（diff） |
|---|---|---|---|---|
| `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（T1+T3） | ✓ | **ok** | ① 产物清单③+步骤 3 改「复用 ∨ 按需新建」；② 换包协议四条（空闲判读/一树一常驻分支/交接卫生白名单/按需扩容归 human）；abandoned 复位到「同步后的专用 base 分支」非 main、main→base 同步显式前置（R1-F1/N3）均逐字在位；③ base ref 三处对齐 F-05（产物清单 base 括注、worktree add 命令尾参、`--check-launch --base-ref`）全改专用 base 分支；⑤ §7 启动卡四要素（changeId / 基线锚指针 / 上包 pod-retro 留根 / `VERIFY_FW_BUILD_REQUIRED=1` 声明行含「未设即降级」机理）齐、launch 侧动作收敛三件；④ 坑节新增交接卫生条 | 全部落位。T3：producer 五要素（计数+命令+日期+量测 SHA+面域）、consumer `git diff --quiet <anchor-sha> HEAD -- <面域>` 判停、三件齐第三态、边界「本包验收齿恒现跑」逐字在位 |
| `.claude/references/dev-loop-operating-system.md`（T1⑥） | ✓ | **ok** | `:50` 每包 worktree add 同族措辞改「复用 ∨ 按需新建」口径 | 已改（V8 grep 0 命中）；另附「pod-coordination §2c 换包协议」指针（一行内一致性连带，见 §4 观察 2） |
| `.claude/references/pod-coordination.md`（T2） | ✓ | **ok** | 新增「worktree 池复用（换包协议）」节：无结构绑定（引 portfolio-planner launch 侧预填语义）+ 协议四条 + 相容声明（merge-driver worktreePath 定位+读当前分支、ARQ-260905 索引、快照流向） | §2c 全部落位；引注核为 portfolio-planner.md Step 6 ③，与 PROPOSAL 所引 :50 同段同义（亲读核对无误）。§2b 机械承载行「worktree add 前」→「新建 worktree 或复用换包前」= 已申报连带（summary §3.2），性质核为复用形态下原行字面不可达的一致性修正、无夹带 |
| `.claude/agents/integrator.md`（T4） | ✓ | **ok** | `:64` 分支亲核改「本树常驻分支（非 main/master 即过，merge-driver 同口径）」；`:68` rebase/merge 限常驻分支，「禁引入 merge commit」驱动契约原文保留 | 两处落位、契约句在位（V5 复跑：`dev/<changeId>` 0 命中、契约句 grep 命中） |
| `.claude/skills/kidpal-idf-env/SKILL.md`（T5） | ✓ | **ok** | 补两坑：`-T main` 已失效（`Error: No such option: -T`，正确用法 `espidf_wsl.sh build`）+ `MSYS_NO_PATHCONV=1` 前缀；证据引 evidence-wsl-pitfalls.md | 新节两坑 + 成功形态命令块 + 制品指针落位；`-T main` 仅以「已失效」警示形态出现（V6 复跑核对） |
| `docs/architecture.md`（T6） | ✓ | **ok** | §5.5.6 构建条目后补一行开发拓扑记账（树池 + 环境等价三件套 + 热缓存随树 + skill 指针），纯记账 | :301 一行在位（V7），与 PROPOSAL 给定文本几乎逐字一致，纯增无行为变更 |

敏感面核对：本批不触 G1 指令根 / G3 强制层 / self-def / review-loop agent 定义；integrator.md 改动 == T4 被批范围逐字，无权限/写域扩张。破坏性自检：diff 中 canonical 文件恰 = manifest 六件，`docs/contracts/**` 零触碰，manifest 外仅制品面（INDEX.md / PROPOSAL / summary / evidence / review-r1 / review-r2）；`.codebase-memory` 两件仅工作树常态漂移、未入 commit。

## 3. 验证集复核（复跑）

- V1：`worktree add` 唯一命中 = 步骤 3 按需新建条件形态 ✓
- V2：base ref 三形态 grep 0 命中（exit 1）✓（复跑）
- V4：锚点协议 5 命中 +「三件齐」1 命中 ✓
- V5：integrator 两 grep 如上 ✓（复跑）
- V6：两坑 4 命中、`-T main` 仅警示形态 ✓
- V7：:301 三关键词齐 ✓
- V8：`每个 ready TaskPackage` 0 命中 ✓（复跑，含 summary §3.1 申报的措辞级返工后终态）
- spec-anchor-check：ok:true（exit 0）✓（复跑）；route-regression-check：28 ok / 0 fail ✓（复跑）
- V9（N1b 换包实战）：按 OQ3 排期未到期，不阻塞——与 summary §3.3 申报一致

## 4. 破坏性自检 + 观察项（不构成 RED，均已闭环或认可）

1. **T1 坑节「传染条」改写**：既有坑「旧 prompt 的过期环境注记会传染」被改写为「过期的环境注记会传染（prompt 已收缩为启动卡，此坑移位到 skills / progress.md 锚行）」。不在 §2 任何 T 的逐字改法内——性质核为 T1⑤（prompt 收缩）的同文件一致性连带（原文「照抄上一批 prompt」前提被 T1⑤ 废除，不改则与新 §7 内部矛盾），纪律实质保留、无夹带。首验时未升入 summary §3 诚实注记——**返修已在注记 5 补报，本项闭环**。
2. T1⑥ 附带 §2c 指针：同上一致性连带量级，一行内、已见于 §1 对应表。复核认可不另立注记（连带本体就在 T1⑥ 授权的一行改动之内，非超出改法的独立行为）。
3. 两项均无权限面/写域扩张、与批定无矛盾。

## 5. 结论

```text
{ ok: true,
  structuralGate: "ok:true（baseRef=main 复跑，exit 0；返修 @ 5ae0874 锚行去粗后通过）",
  semanticMatch: "六文件逐 T 吻合，无漏改、无夹带、无批定矛盾",
  verificationSet: "V1–V8 复跑绿 + spec-anchor ok + route-regression 28/0；V9 按 OQ3 排期不阻塞",
  advisories: "citationAnchor 1 条（在册盲点族）+ lint-descriptions 1 条既有 warn，均 advisory 级不阻断" }
```

首验 FAIL（锚行格式不可机析）→ 返修 @ 5ae0874（纯格式 + 补报注记，canonical 零触碰，经逐行核对）→ 结构门复跑全绿。放行进 merge 门；merge 决定归 human manual-merge 门。
