# PROPOSAL · 260907-gov-review-merge-friction

> **状态：FROZEN · 2026-09-07 · R2=GREEN（凭据自冻，授权记本行：human 立项 ack 2026-09-07「同意立项」+ architect R2 GREEN + human 裁 OQ1=(a)/OQ2 终裁 (c)/OQ3=(a)）**（packet = 本 FROZEN PROPOSAL + landing-manifest 三路径 + architect-landing-summary；landing 走主线程 packet-bound，GREEN 不豁免任何落地门——landing-verifier 独立核验、retro、前置升级闸照走）。
> **承载 ARQ**：`ARQ-260907-merge-driver-approval-state-desync` + `ARQ-260907-merge-closeout-bookkeeping-gap`（同批根治，两条根因同属 merge 段机械面）；**不承载** `ARQ-260907-escalation-cross-system-overfire`（判据语义变更走 Track A，见 §6 留根指针）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（全部为合并机械链自动化补全 + 文档 runbook；行为向严：自动收口只认白名单路径、人批豁免复用既有五要素核验、union 仅限 append-only 纪律文件、T6 落 main 簿记 = 合并现场同类机械 commit（单文件 append、可回滚、message 留痕）；无新门语义、无不可逆操作）。
> **review profile**：G2+G3（触面：`.claude/scripts/merge-driver.mjs` = G3、`.gitattributes` = 仓根点文件、`.claude/references/pod-coordination.md` = G2；G1/契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/merge-driver.mjs
.gitattributes
.claude/references/pod-coordination.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：**ESCALATE 人批之后的 merge-driver 调用一次成功，双 Pod 近距合并不再因收尾制品未提交、投影文件冲突、append 冲突而手动返工**。

现状差距（本批两 Pod merge 段实录，2026-09-07）：260829-schedule-p2 四次 merge-driver 调用才合并成功（人批后首跑 no-op → unstaged 拒绝 → rebase 冲突 abort → 成功）；260829-home-p1 为同一病灶付出 3 个补丁式簿记 commit（`82cb8fb` 收尾簿记 / `e063fb1` rebase 期投影重建 / `46d99b5` post-merge 簿记落 main）。全部摩擦的处置手段都是流程外知识（读源码、手动 --theirs/union、盲猜状态机），无 runbook 承载。

**非目标（本批不做）**：① 不改升级闸 cross-system 判据语义（ARQ-260907-escalation-cross-system-overfire 走 Track A 另行）；② 不动 HOST_ACCEPTANCE_SRCS 顺序承重的结构性根治（seam reset 归 dev-loop 包，见 §6）；③ 不重排 merge-driver 既有合并链步骤序（rebase→quick-check→merge 链不动，只在入口与冲突消解面补全）；④ 不改 planner.md（T1 是补实现兑现其既有承诺，条款零改动）。

## 1. 问题与根因（地面真值）

### P1 · 人批后首跑恒 no-op（文档承诺与脚本实况脱节）

**证据（实测，2026-09-07）**：human 批准合并（260829-schedule-p2 pod-events `human-decision` eventId `d5c2134d-d22d-412f-ae1d-588a458759fd`）后直跑：

```
node .claude/scripts/merge-driver.mjs --pod 260829-schedule-p2 --human-approval-ref d5c2134d-d22d-412f-ae1d-588a458759fd
→ [merge-driver] no ready-to-merge pods; nothing to do
```

而 planner.md:96 明文承诺（verbatim）：「直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>`（仍经 merge token 串行化；**blocked 停等态由闸内联动豁免消解，OQ2=(a)**）」。脚本实况（merge-driver.mjs:1011-1017，grep 亲核）：就绪判据只有 `ready.filter((p) => p.status === 'ready-to-merge')`，`blocked` 停等态无任何豁免消解路径——承诺的机制不存在。恢复靠 planner 读源码后手动补 `portfolio-reconcile.mjs --set-pod <id> --status ready-to-merge`（流程外知识，无文档）。

**根因**：260904-gov-review-escalate-merge-approval 批落地了 `--human-approval-ref` 透传（merge-driver Step 1b 前置升级闸核验人批事件），但「blocked → ready-to-merge」的状态消解没有随批落地；planner.md/pod-coordination 的 OQ2=(a) 条款描述的是目标语义而非已实现行为。

**实证次数**：1 次直接实证（schedule-p2）；home-p1 走同一文档路径（其人批 eventId `76cd3415-1c90-42c4-8859-fbc16a194886`，merge `20f1e09`），按条款推演同型摩擦（未留首跑实录，如实注记为推断）。

### P2 · merge 前治理面收口无定义步骤（unstaged 拒绝）

**证据（实测，2026-09-07）**：260829-schedule-p2 按 planner.md 顺序走完（integrator 产生 commit `e625135` → planner 写 pod-retro.md → 跑 supervisor-verdict-gate.mjs fs-write `supervisor-verdict.json` → 落 human-decision 事件追加 pod-events.jsonl）后调 merge-driver：

```
→ WARN: 260829-schedule-p2: rebase conflict — aborting rebase
→ error: cannot rebase: You have unstaged changes.
→ error: Please commit or stash them.
```

对照实证：home-p1 的 main `82cb8fb`「chore(devloop): 260829-home-p1 收尾簿记——pod-retro 落盘 + supervisor-verdict(ESCALATE 误报裁定) + human 批准事件入 pod-events」即同病灶的补丁式解（先 commit 再 merge）。两 Pod 独立踩中 = 流程结构性，非个案。

**根因**：流程产物时序缺口——pod-retro（planner 收尾相位）、supervisor-verdict.json（闸 fs-write）、pod-events 追加（human-decision）全部产生于 integrator「最终」commit 之后，而流程没有定义「merge-driver 调用前的最终簿记 commit」步骤；merge-driver 对非 `.codebase-memory` 的 unstaged 变更 fail-closed（ARQ-260906-live-graph-dirty-blocks-merge 只解决了图缓存一类脏源）。

### P3 · 投影文件结构性冲突（tracked 派生状态）

**证据（实测，2026-09-07）**：schedule-p2 手动 rebase onto main 时三处冲突文件：`.claude/taskpackages/current/taskpackage.json`、`.claude/taskpackages/current/verdict.json`、`.planning/kidpal-mvp/progress.md`。前两者是「最后写者赢」投影——每个 Pod 的收口 commit 都把它重指到自己的 taskpackage，双 Pod 近距合并时后者分支切割于前者合并之前，rebase 必冲突（解法恒为 `--theirs`，语义上「被合并的包赢」永远正确）。旁证：home-p1 `e063fb1`「chore(planning): 260829-home-p1 rebase 期 .planning 投影钩子重建（最后更新日期戳）」= `.planning/INDEX.md` 日期戳同型冲突的补丁式解。

**根因**：派生投影以 tracked 文件提交 + 每 Pod 收口必重写 → 结构性必现，与具体实现无关。

### P4 · append-only 共享文件 EOF 冲突

**证据（实测，2026-09-07）**：`.planning/kidpal-mvp/progress.md` 冲突形态 = HEAD 侧（launch 准备记 + home-p1 收口记）vs 本包收口记，双方都在文件尾部 append——append-only 纪律文件的典型 EOF 竞合，git 无法自动解，union 保留双侧是唯一正解，但每 Pod 都要人手做一遍。

**根因**：append-only 纪律文件未配 `merge=union`（`.gitattributes` 现无该条；根级 `.gitattributes` 在位已核，现有内容为 `*.sh text eol=lf` 钉死条（ARQ-260906-shell-scripts-lf-gitattributes）；`.codebase-memory` 的 binary merge=ours 注记在 `.codebase-memory/.gitattributes` 独立文件——两者与 T4 单行追加均无路径交集）。

（CMakeLists additive 冲突为第五摩擦面，但其根治 = HOST_ACCEPTANCE_SRCS 顺序承重消除，属固件测试基建、归 dev-loop 包，本批不承载——见 §6 留根。）

## 2. 改法（T1…T6）

### T1 · merge-driver 人批豁免消解（兑现 OQ2=(a) 既有承诺）

改什么：merge-driver 入口（board reconcile 后、ready 收集前）对「`--pod X` + `--human-approval-ref <eventId>` 且 X 状态 = blocked」形状：复用 pre-merge-escalation-gate 既有五要素核验（同 Step 1b 透传路径，不重复实现）核验 ref——核验通过即 `setPod(X, ready-to-merge)` **并重跑 reconcile()（或同步改内存 board 视图）再进 ready 收集 / 单 Pod 查找**（main() 的 ready 收集读 freshBoard() 已产出的快照，setPod 只写 pod 文件不回写内存 board——漏此步则 T1 落地仍 no-op；对偶 portfolio-reconcile CLI `--set-pod` 模式「写 pod 后无条件 reconcile」既有语义）并 stderr 一行留痕（`approval-ref 解锁 blocked → ready-to-merge`）；核验失败 fail-closed 保持 blocked 并报核验失败原因。无 `--human-approval-ref` 的 blocked Pod 行为零变化。

为什么：planner.md:96 条款已经这么承诺，本 T 只补缺失实现；机械链向严（核验失败不放行、无 ref 不豁免），不引入新自由度。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T2 · merge-driver Step 0 治理面自动收口（白名单 auto-commit）

改什么：rebase 步之前新增 Step 0：`git status --porcelain` 检测本 worktree unstaged/untracked 变更，若**全部**落在白名单（`.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**`）内 → 自动 `git add <精确路径> && git commit`（message 形如 `chore(devloop): <pod-id> merge 前治理面收口——<文件清单摘要>`）；任一文件在白名单外 → 保持现状 fail 行为（报 unstaged、置 blocked），绝不吞非治理面变更。白名单的 `<pod-id>` 须按**目录段精确等值**判定（不做子串 / 可拼路径匹配），防畸形 `--pod` 参数（如含路径分隔符的形状）使 `current/` 或他 Pod 目录落入白名单。

为什么：P2 根因 = 流程产出的收尾制品无人 commit。让 driver 收口比让 planner 记得（runbook 条款）更机械——照文档顺序走的 Pod 从「必踩」变「无感」。白名单收窄到本 Pod 治理面，防止误吞他源变更。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T3 · 投影冲突 --theirs 特判

改什么：rebase 冲突消解辅助：driver 检测冲突文件集时，对 `.claude/taskpackages/current/taskpackage.json`、`.claude/taskpackages/current/verdict.json` 自动 `git checkout --theirs` + `git add`（被合并的 Pod 赢——current 投影语义本就指向最后合并的活跃包）；**冲突文件集含任一非特判文件 → 整体 abort + blocked（投影特判不部分生效）**，其余冲突文件维持现有 abort + blocked + 人工消解路径。rebase 重放多 commit 时冲突须循环消解至 rebase 完成、`git rebase --continue` 以 `GIT_EDITOR=true` 免交互。

为什么：P3 根因 = 结构性必现 + 解法恒定。语义上「被合并包赢」无分叉，机械特判安全；不 untrack current/（OQ3 备选案，动 verify-gates 读取面、riskTier 升，不取）。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T4 · progress.md 标 merge=union

改什么：`.gitattributes` 增一行 `.planning/kidpal-mvp/progress.md merge=union`（git 原生 union merge 驱动；append-only 纪律下双侧保留即正解）。

为什么：P4 根因纯机械。union 驱动对「双方同区段改动」会交错保留双方行——progress.md 的 append-only 纪律（AGENTS.md Hard Rules「历史记录类文件一律 append」）保证输入形状安全；若未来出现同区段非 append 改写，责任在违反纪律的写入方，union 是该纪律下的正确默认。

目标文件：`.gitattributes`（改）

### T5 · pod-coordination.md 补 merge 冲突消解 runbook + post-merge 簿记口径

改什么：`.claude/references/pod-coordination.md`（追加节，不动既有节）：① 「merge 冲突消解 runbook」——driver abort + blocked 后的合法消解序（手动 `git rebase main` → 消解 → `git rebase --continue` → 重置 ready-to-merge → 重跑 `merge-driver --pod … --human-approval-ref …`；强调 rebase --continue 属 driver 流程内消解、不绕任何闸）；② 「post-merge 簿记落点」钉口径 = merged 事件行由 T6 机械落 main（pod-events main 自持、分支可删）；INDEX 一致性由下批 `changes-index.mjs` 全量重建收敛（本批立项 review 实测 main INDEX 确缺 schedule-p2 行、待重建补齐）；③ T1/T2 落地后 OQ2=(a) 条款成立性复核注记（planner.md 零改动）。

为什么：摩擦的处置手段目前全靠会话记忆；runbook 把「流程外知识」变「文档内知识」。G2 面纯文档，无行为变更。

目标文件：`.claude/references/pod-coordination.md`（改）

### T6 · merge-driver 合并完成即落 main 簿记（merged 事件行进 main，分支可删）

改什么：merge-driver Step 5 合并成功后新增 **Step 5b**（既有 Step 6「标记 merged」/ Step 7 reconcile 编号不动、防撞号漂移；同一 `mergeCwd` = 持有 main 的 worktree，即合并现场）：向 `.claude/taskpackages/<pod-id>/pod-events.jsonl` 追加一条 `merge-completed` 事件行（schemaVersion 1、actor=`merge-driver`、payload 摘 main tip sha / human-approval-ref / 冲突消解摘要），`git add` 该单文件 + `git commit`（message 形如 `chore(devloop): <pod-id> merge 完成事件入 pod-events（merged <sha>）`）。只 append 单 Pod 自有文件（路径与他 Pod 不相交，结构性无冲突）；DRY_RUN 跳过；INDEX 不在此步动（全量重建仍归下批）。**失败语义**：簿记失败（append / commit 抛错）时 merge 已成事实 → 一律 advisory warn 留痕、绝不 errorExit / 未捕获抛出（防全量模式下后续 ready Pod 不被处理，对偶 detect-changes F1 承重墙先例）；commit 以 pathspec 限定（`git commit -m … -- <file>` 形态）——graph 同步 advisory 失败时可能残留的 staged `.codebase-memory` 变更不得被簿记 commit 夹带。

为什么：merged 事件是跨 Pod 审计锚点，feature 分支可能删除、main 必须自持（human 复议裁定，2026-09-07）；driver 本就在持有 main 的 worktree 执行合并（mergeCwd = findMainHolderWorktree，Step 5 既有实现），顺势落簿记零额外前提——原 OQ2(b)「时序耦合差」异议只对 planner 手动补做成立，对 driver 机械做不成立。home-p1 `46d99b5` / schedule-p2 `7717eea` 的 pod-events 部分由此机械化，不再依赖会话记得。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 | T1/T2/T3/T6 四个行为补全；消费方 = 所有 Pod 的 merge 段（含 ESCALATE 人批路径）；既有五自检为回归守卫 |
| 仓库属性 | `.gitattributes` | 仓根点文件 | T4 单行追加；消费方 = git merge 驱动选择；与既有 `.codebase-memory` 条目无路径交集 |
| 流程文档 | `.claude/references/pod-coordination.md` | G2 | T5 追加节；消费方 = 各 Pod planner 会话；无行为面 |
| 派生投影 | `.claude/taskpackages/current/*`、`.planning/INDEX.md` | 未触 | T3 只改消解策略不改投影本身；INDEX 维持「全量重建纠偏」既有机制 |
| 升级闸判据 | `supervisor-verdict-gate.mjs` / `pre-merge-escalation-gate.mjs` | 未触 | 本批不承载（ARQ-escalation-cross-system-overfire 走 Track A） |

**【硬项 1b · sweep 透镜】**：本批新增机械断言 = Step 0 白名单判定 pattern（`.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**` 前缀匹配）。census（机械枚举，2026-09-07 起草期跑，R1 判官复跑勘误）：`ls .claude/taskpackages/` 实测 20 项（19 个 `<pod-id>` 目录 + current）与 `ls changes/dev/`（12 目录）在册——pattern 覆盖面 = 现存全部 Pod 目录形状；**不含** `.claude/taskpackages/current/` 投影（不匹配 `<pod-id>` 形状）——current/ 的冲突面由 T3 特判消解，若其以 unstaged 形态在场则 Step 0 按白名单外 fail-closed 维持现状（不吞，设计意图，如实注记）。T6 的写入面（`<pod-id>/pod-events.jsonl` append）同属 `<pod-id>` 形状族、与他 Pod 路径不相交；其 commit 落点为 main 合并现场（driver 既有 mergeCwd），不与 Step 0（feature worktree、merge 前）交叉。本批不新增独立门、无退役语面清扫（硬项 1c 不适用）。

## 4. human 取舍点（OQ）

**OQ1 · Step 0 自动收口白名单范围**（human 裁定：(a)，2026-09-07）
- (a) 仅 `.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**`【推荐】——最小面：这两处是 P2 病灶的病灶文件（pod-retro/verdict/pod-events 全在此）；`.planning/**` 的投影钩子维持人工重建先例。
- (b) 加 `.planning/**`——把 progress.md 收口也自动化；但 `.planning` 含 INDEX 日期戳等派生面，自动 commit 会把「该重建的」当「该提交的」固化，噪声大于收益。
- 推荐理由一句话：(a) 只收「真制品」、(b) 会收「派生投影」，机械收口应只碰前者。

**OQ2 · post-merge 簿记落点口径（T5 ③ → 经复议升级为 T6）**（human 裁定：初裁 (a) 落 feature 分支，2026-09-07 同日复议改判 (c)，2026-09-07）
- (a) 落 feature 分支、main 一致性由下批 index 全量重建收敛【初裁】——schedule-p2 实际做法；不需要主线程在场、不占 main checkout。**复议否决**：实测核明 pod-events 的 merged 事件行无任何重建机制覆盖、main 永久缺行，而 feature 分支可能删除，跨 Pod 审计锚点丢失且不可恢复。
- (b) main 直接 chore commit（planner 手动）——home-p1 做法（`46d99b5`）；要求主线程恰好可用 main checkout，时序耦合差。
- (c) merged 事件行由 merge-driver Step 5b 机械落 main【终裁】——driver 本就在持有 main 的 worktree 执行合并，顺势落簿记零额外前提；pod-events 为 per-Pod 文件、结构性无跨 Pod 冲突；INDEX 全量重建仍留下批。落地为新增 T6。
- 改判理由一句话：merged 事件是审计锚点必须 main 自持，而「时序耦合差」异议对 driver 机械做不成立（driver 本就站在合并现场的 main worktree 里）。

**OQ3 · current/ 投影处置策略（T3）**（human 裁定：(a)，2026-09-07）
- (a) rebase 冲突时 --theirs 特判【推荐】——保守：投影仍 tracked、verify-gates 读取面零变化。
- (b) current/ untrack（.gitignore + 门改读各包 taskpackage）——结构化根治，但动 gate 读取面、涉及 fresh-clone 冷启动语义，riskTier 升出本批 low-medium 定位。
- 推荐理由一句话：(a) 零门触碰即消除摩擦，(b) 值得但要单独立项。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **既有五自检复跑零回退**：`node .claude/scripts/merge-driver.mjs --self-check-retro-advisory` / `--self-check-quick-checks` / `--self-check-detect-changes` / `--self-check-rebase-target` / `--self-check-worktree-topology` → 全部 `ok:true`（入口亲核：merge-driver.mjs:977-993，2026-09-07 grep 实录）。
2. **T1 断言**：新增自检/夹具——blocked Pod + 有效 `--human-approval-ref` → 自动 ready-to-merge（stderr 留痕行在场）；无效 ref → 保持 blocked + 核验失败原因输出（fail-closed 向严）；无 ref 的 blocked Pod → 行为零变化。
3. **T2 断言**：夹具——白名单内 unstaged（pod-events.jsonl + pod-retro.md）→ 自动 commit 生成且 `git status` 净；白名单外文件在场 → 维持现有 fail 路径（不吞变更）。
4. **T3 断言**：夹具——构造 current/taskpackage.json 冲突 → 自动 --theirs 解除且 rebase 推进；非白名单冲突文件 → abort + blocked 维持。
5. **T4 断言**：构造双分支 progress.md EOF append → merge 后双侧条目俱在。
6. **T6 断言**：夹具——合并成功后 main 上 `<pod-id>/pod-events.jsonl` 末行为 `merge-completed` 事件（schemaVersion/actor/main tip 在场）且以独立 `chore(devloop)` commit 落 main；DRY_RUN 下不追加不 commit；多 Pod 顺序合并时各 Pod 各一行、互不交叉。
7. **T5 验证**：`route-regression-check.mjs` + `lint-descriptions.mjs` 复跑（G2 面惯例）绿；runbook 节落位 diff 人读核对。
8. **landing 后**：`landing-fidelity-check.mjs`（结构门 + 锚清单比对）+ 下一个真实 Pod merge 段实测（机械链行为级验证——本批验收的最终权重在「下个 Pod 一次跑成」）。

## 6. 留根清单

- **ARQ-260907-escalation-cross-system-overfire 不承载**：升级闸 cross-system 判据把 acceptance/deploy 常规面计入跨系统信号、双面板 Pod 恒 ESCALATE——gate 语义变更 + 真设计分叉，走 Track A（`/arch-change`），owner=architect + human 拍板。
- **HOST_ACCEPTANCE_SRCS 顺序承重不承载**：boundary 齿依赖「进程内最先跑」使列表顺序变语义承重、纯 additive 行也不敢自动合并——根治 = seam 加 reset()、boundary 齿自建新鲜 seam，属固件测试基建，归下个碰 host_app 的 dev-loop 包顺手落（ARQ-260907-merge-closeout-bookkeeping-gap 建议改动段有指针）。
- **CMakeLists additive 冲突消解先例**：本 Pod 手动消解的双保留解法（双侧注释/顺序约束兼容性核对）已随 merge `b7df3ab` 入 main，供 runbook 期参考；顺序承重消除前此类冲突仍需人工。
- **自律件（不进模板）**：ARQ 登记与 PROPOSAL 起草同会话完成时，队列行「处理时机」须与 PROPOSAL 承载关系一致（本批三行中两行指向本 PROPOSAL、一行指向 Track A）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-07 · architect（Track B 判官，第 1 轮）

**评审依据**（只读 canonical，全文亲核）：`AGENTS.md`「治理路由」节、`.claude/agents/planner.md` 合并门段（:92-97）、`.claude/scripts/merge-driver.mjs`、`.claude/scripts/pre-merge-escalation-gate.mjs`（resolveHumanApproval / evaluate）、`.claude/scripts/portfolio-reconcile.mjs`（setPod / reconcile）、`.gitattributes`、`.codebase-memory/.gitattributes`、`.claude/references/pod-coordination.md`、`.claude/scripts/changes-index.mjs`、`docs/ARCH-REVIEW-QUEUE.md`:45-47、`docs/templates/PROPOSAL.template.md`。

**清单 1 · 地面真值抽查 —— PASS**：

- merge-driver.mjs:1011-1017 就绪判据声明亲核成立——ready 收集只有 `filter((p) => p.status === 'ready-to-merge')`（:1011-1013），no-op 退出在 :1015-1017，全文件无任何 blocked 消解路径。
- planner.md:96 引文逐字命中（含「blocked 停等态由闸内联动豁免消解，OQ2=(a)」与「人批不豁免 off-main/契约/provenance，闸照拦」两句）。
- 七件引用 commit（82cb8fb / e063fb1 / 46d99b5 / 20f1e09 / e625135 / 7717eea / b7df3ab）`git show --no-patch` 全部在册、subject 与 PROPOSAL 描述一致。
- 两个 eventId（d5c2134d-d22d-412f-ae1d-588a458759fd、76cd3415-1c90-42c4-8859-fbc16a194886）在对应 pod-events.jsonl 命中，五要素形状完整。
- OQ2 终裁 (c) 的事实前提坚实：亲核 7717eea（schedule-p2 merged 事件行 eventId 2cdc7558）**仅在 `dev/260829-schedule-p2` 分支、main 不含**——「merged 行 main 永久缺行、feature 分支删即丢」实测成立；对照组 46d99b5（home-p1）在 main。
- T5 ② 前提成立：main 的 changes/INDEX.md 确无 260829-schedule-p2 行（仅 home-p1 行在 :44）。
- 五自检入口亲核在册（merge-driver.mjs:977-996 五个 `--self-check-*` flag 分支）。

**清单 2 · 改法充分性与正确性 —— PASS（附 6 项起草面修正，见打回区；无新风险升级项）**：

- **T1 闸语义**：setPod(ready-to-merge) 只消「blocked 驻留标记」、使 Pod 得以进合并链；Step 1b 前置升级闸照跑全判——亲核 evaluate()（pre-merge-escalation-gate.mjs:151-187）：off-main 与契约面无条件、provenance 缺失永不豁免、verdict=BLOCK 永不豁免，escalateExempt 只豁免 riskTier / 非 APPROVE / blocked 三条。planner.md「人批不豁免 off-main/契约/provenance，闸照拦」条款语义保持，向严性成立（无 ref 不豁免、核验失败保持 blocked 并报因）。
- **T2 白名单**：P2 病灶文件三件全落白名单（pod-retro.md 与 supervisor-verdict.json 在 `changes/dev/<pod-id>/**`、pod-events.jsonl 在 `.claude/taskpackages/<pod-id>/**`）；`current/` 不匹配 `<pod-id>` 形状 → 白名单外 fail-closed，与硬项 1b 注记自洽，行为一致性成立。
- **T3 方向亲核**：rebase 语境 ours=基线（main）侧、theirs=被重放的 Pod 侧——`git checkout --theirs` 确为「被合并包赢」，与 current 投影「指向最后合并的活跃包」语义一致。方向正确，无反向风险。
- **T4 边界**：union 仅 progress.md 单路径，与既有 `*.sh text eol=lf` 无路径交集、与 `.codebase-memory/.gitattributes`（binary merge=ours）分文件无叠加；append-only 纪律（AGENTS.md Hard Rules「历史记录类文件一律 append」）成立下 union 是正确默认，同区段非 append 改写责任归写入方——假设边界已在 T4 正文如实声明。
- **T6 相容性**：driver 在 mergeCwd（findMainHolderWorktree 解析出的 main worktree）本就执行本地 `--no-ff` merge commit；簿记 chore commit 属同一行为类（本地、单文件 append、可回滚、message 留痕、DRY_RUN 跳过、无 remote push）。「任何路径都不允许直推 main」（planner.md:97）语义 = 不绕 merge-driver 合并链 / 不 push remote，本地机械 commit 由 driver 在合并现场产出，与 46d99b5 人读先例同类——相容。多 Pod 顺序合并逐 Pod 落行正确（各 Pod pod-events 路径不相交，后 Pod rebase onto 含前 Pod 簿记 commit 的 main 无新增冲突面）；与 skip-worktree/ensureGraphCacheSynced 无干扰（graph 两件 skip-worktree 在册不进 status），唯 commit 面须收窄（见 F-3）。

**清单 3 · 影响面 sweep —— PASS（附 1 处数字失真，见 F-2）**：分类表覆盖四个触面 + 两处显式未触，UNCLASSIFIED=0 成立；硬项 1b 结论成立（pattern 形状族覆盖现存全部 Pod 目录、current/ 由 T3 特判 + Step 0 fail-closed 双兜底、T6 写入面注记在册），唯 census 计数失真须修正。

**清单 4 · 结构硬条款 —— PASS**：landing-manifest 围栏块在场（Status 头部块下方固定位、一行一裸路径、恰三条）；packet-id 行在场且记 T1–T6；围栏块三路径 ≡ §2 六 T 目标文件并集（T1/T2/T3/T6 → merge-driver.mjs、T4 → .gitattributes、T5 → pod-coordination.md）；各 T 均有「目标文件：」锚清单、标题 `### T<n> · ` 形态合规。唯 §2 节标题仍写「T1…T5」漏 T6（见 F-5）。

**清单 5 · 验证集可执行性 —— PASS**：八项均机判可落地；T2/T3/T4/T6 夹具可循 worktree-topology 自检先例（临时 git 仓 + 双分支构造）搭建；T6 断言观测点明确（main tip log + pod-events 末行 + DRY_RUN 对照 + 多 Pod 各一行）。

**清单 6 · riskTier 复核 —— PASS**：low-medium 恰当——三个行为扩展全部白名单 / 特判 / 单文件收窄、fail-closed、可回滚、stderr 留痕；无新门语义、无不可逆操作。ARQ-260907-merge-closeout-bookkeeping-gap 自评 medium 是「不修的代价」侧，修法本身 low-medium 证成成立。

**打回项（全部为起草面修正项，主线程可自行改；无「影响目标效果的打回项」、无需升级 human）**：

- **F-1（T1，改法机械缺口）**：T1 钉的落点「入口（board reconcile 后、ready 收集前）」按字面实现必然仍 no-op——main() 的 ready 收集读 freshBoard() 已产出的 board 快照（merge-driver.mjs:1000-1013），setPod 只写 `devloop/pods/<id>.json`、不回写内存 board。改法：T1 补一句「setPod 后重跑 reconcile()（或同步改内存 board 视图）再进 ready 收集 / 单 Pod 查找」，对偶 portfolio-reconcile CLI `--set-pod` 模式「写 pod 后无条件 reconcile」既有语义。
- **F-2（硬项 1b，census 数字失真）**：起草面声明「ls .claude/taskpackages/（16 目录）」实测为 20 项（19 个 `<pod-id>` 目录 + current；判官 2026-09-07 复跑 `ls` 实录）；changes/dev 12 目录无误。形状族全覆盖与 current 不匹配的结论不变，但按硬项 1a「凡数字引用起草期现跑实测」义务须改正数字并留痕。
- **F-3（T6，失败语义未钉）**：Step 6 簿记失败（append / commit 抛错）时 merge 已成事实，处置未定义。须钉：advisory warn 留痕、绝不 errorExit / 未捕获抛出（全量模式下未捕获异常 crash node → 后续 ready Pod 全不处理，对偶 detect-changes F1 承重墙先例 merge-driver.mjs:129-133）；且 commit 须 pathspec 限定（`git commit -m … -- <file>` 形态）——graph 同步 advisory 失败时可能残留的 staged `.codebase-memory` 变更不得被簿记 commit 夹带。
- **F-4（T3，混合集语义钉死）**：补一句「冲突文件集含任一非特判文件 → 整体 abort + blocked（投影特判不部分生效）」；并注记 rebase 重放多 commit 时冲突须循环消解至 rebase 完成、`git rebase --continue` 须 GIT_EDITOR=true 免交互。
- **F-5（编号/标题，两处轻改）**：① §2 节标题「改法（T1…T5）」漏 T6，改「T1…T6」；② 既有代码已有「Step 6: 标记 merged」「Step 7: reconcile」（merge-driver.mjs:959-969），新 T6 与之撞号——T6 正文改称「Step 5b」或声明既有 Step 6/7 顺延重编号，防 runbook 与代码注释漂移。
- **F-6（T2，白名单匹配形状钉死）**：白名单前缀匹配须按**目录段精确等值**判定 `<pod-id>`（不得做子串 / 可拼路径匹配），防畸形 `--pod` 参数（如含路径分隔符的形状）使 `current/` 或他 Pod 目录落入白名单。一行钉进 T2 即可。

**单一结论：NOT-GREEN（R1）**——方向、地面真值、闸语义、结构硬条款、验证集、riskTier 全部核过成立；F-1 / F-3 属改法正确性缺口（不修则 T1 落地即失效、T6 失败可中断整批合并），F-2 / F-4 / F-5 / F-6 为义务性修正。六项均不动 human 已裁取舍点（OQ1=(a) / OQ2 终裁 (c) / OQ3=(a) 结论零变化）、无需升级 human；主线程修正后交 R2 复核（本轮次 1/3，熔断阈值未近）。

### R2 · 2026-09-07 · architect（Track B 判官，第 2 轮）

**复核对象**：DRAFT v0.4 全文重读；R1 六项打回逐项核销 + 修正旁及面抽验（Status 行 / §2 标题 / §3 硬项 1b / §4 OQ2(c) / §5 / manifest / packet-id 行），未重开已裁事项（human 取舍点、ARQ 承载关系、留根清单零变化）。

**R1 打回复核（六项全部消解）**：

- **F-1 → 消解**：T1 正文（v0.4）已补「`setPod(X, ready-to-merge)` 并重跑 reconcile()（或同步改内存 board 视图）再进 ready 收集 / 单 Pod 查找」，且附快照 vs pod 文件的机制理由与 portfolio-reconcile CLI `--set-pod` 模式对偶——与我 R1 亲核的 main() 数据流（freshBoard 快照 :1000-1013）一致，改法自洽。
- **F-2 → 消解**：硬项 1b census 已改为「实测 20 项（19 个 `<pod-id>` 目录 + current）」并标注「R1 判官复跑勘误」——数字与判官 2026-09-07 实测一致，勘误留痕形态符合硬项 1a 义务。
- **F-3 → 消解**：T6 已补「失败语义」段——advisory warn 留痕、绝不 errorExit / 未捕获抛出（防全量模式后续 ready Pod 不被处理，对偶 detect-changes F1 承重墙）+ commit pathspec 限定防夹带 staged `.codebase-memory` 残留。两项处置均与 merge-driver 既有代码形态（:129-133 catch-all 先例）对齐。
- **F-4 → 消解**：T3 已补「冲突文件集含任一非特判文件 → 整体 abort + blocked（投影特判不部分生效）」+ rebase 重放多 commit 循环消解至完成 + `git rebase --continue` 以 `GIT_EDITOR=true` 免交互。混合集语义钉死，无部分生效歧义。
- **F-5 → 消解**：§2 节标题已改「改法（T1…T6）」；T6 落点改称 **Step 5b** 并声明「既有 Step 6『标记 merged』/ Step 7 reconcile 编号不动、防撞号漂移」；OQ2(c) 行同步改称 Step 5b。全文（含 §3 硬项 1b、§5、Status/riskTier 行）无残留旧「Step 6 = 簿记」指称。
- **F-6 → 消解**：T2 已补白名单「`<pod-id>` 须按**目录段精确等值**判定（不做子串 / 可拼路径匹配），防畸形 `--pod` 参数（如含路径分隔符的形状）使 `current/` 或他 Pod 目录落入白名单」。

**修正旁及面抽验（无新缺口）**：① Status 行更新为 DRAFT v0.4 并如实记 R1=NOT-GREEN（六项起草面修正、无升级项）——与 R1 记录一致；② manifest 围栏块三路径、packet-id 行（T1–T6）、各 T「目标文件：」锚清单、`### T<n> · ` 标题形态——R1 已核结构不变项复核仍全数在场；③ 修正仅旁及 T1/T2/T3/T6 正文、§2 标题、§3 硬项 1b、§4 OQ2(c)、Status 行，未触 §1 地面真值与 §5 验证集实质内容，R1 的地面真值结论免重验；④ F-1 的「或同步改内存 board 视图」备选实现路径两种均与既有 reconcile 语义等价，不构成新分叉。

**单一结论：GREEN（R2）**——R1 六项打回全部消解、无新引入缺口；地面真值、改法充分性、闸语义保持、影响面 sweep、结构硬条款、验证集可执行性、riskTier 定位七面在 R1 已全数核过成立、本轮无翻案项。可标 FROZEN（FROZEN 行凭据 = human 立项 ack + R2 GREEN + human 裁 OQ1=(a) / OQ2 终裁 (c) / OQ3=(a) 结果；主线程凭据自冻，授权记 Status 行）。后续落地仍按 Track B 既有链走主线程 packet-bound + landing-verifier 独立核验，本 GREEN 不豁免任何落地门。
