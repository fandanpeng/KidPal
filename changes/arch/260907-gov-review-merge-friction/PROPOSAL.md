# PROPOSAL · 260907-gov-review-merge-friction

> **状态：DRAFT v0.1 · 2026-09-07 · 起草完成，待 human 立项 ack**（FROZEN 行须含凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果；主线程凭据自冻，授权记 Status 行）。
> **承载 ARQ**：`ARQ-260907-merge-driver-approval-state-desync` + `ARQ-260907-merge-closeout-bookkeeping-gap`（同批根治，两条根因同属 merge 段机械面）；**不承载** `ARQ-260907-escalation-cross-system-overfire`（判据语义变更走 Track A，见 §6 留根指针）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（全部为合并机械链自动化补全 + 文档 runbook；行为向严：自动收口只认白名单路径、人批豁免复用既有五要素核验、union 仅限 append-only 纪律文件；无新门语义、无不可逆操作）。
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

**根因**：append-only 纪律文件未配 `merge=union`（`.gitattributes` 现无该条；根级 `.gitattributes` 在位已核，现有内容为 `.codebase-memory` binary merge=ours 共存注记）。

（CMakeLists additive 冲突为第五摩擦面，但其根治 = HOST_ACCEPTANCE_SRCS 顺序承重消除，属固件测试基建、归 dev-loop 包，本批不承载——见 §6 留根。）

## 2. 改法（T1…T5）

### T1 · merge-driver 人批豁免消解（兑现 OQ2=(a) 既有承诺）

改什么：merge-driver 入口（board reconcile 后、ready 收集前）对「`--pod X` + `--human-approval-ref <eventId>` 且 X 状态 = blocked」形状：复用 pre-merge-escalation-gate 既有五要素核验（同 Step 1b 透传路径，不重复实现）核验 ref——核验通过即 `setPod(X, ready-to-merge)` 并 stderr 一行留痕（`approval-ref 解锁 blocked → ready-to-merge`）；核验失败 fail-closed 保持 blocked 并报核验失败原因。无 `--human-approval-ref` 的 blocked Pod 行为零变化。

为什么：planner.md:96 条款已经这么承诺，本 T 只补缺失实现；机械链向严（核验失败不放行、无 ref 不豁免），不引入新自由度。

**目标文件：`.claude/scripts/merge-driver.mjs`**（改）

### T2 · merge-driver Step 0 治理面自动收口（白名单 auto-commit）

改什么：rebase 步之前新增 Step 0：`git status --porcelain` 检测本 worktree unstaged/untracked 变更，若**全部**落在白名单（`.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**`）内 → 自动 `git add <精确路径> && git commit`（message 形如 `chore(devloop): <pod-id> merge 前治理面收口——<文件清单摘要>`）；任一文件在白名单外 → 保持现状 fail 行为（报 unstaged、置 blocked），绝不吞非治理面变更。

为什么：P2 根因 = 流程产出的收尾制品无人 commit。让 driver 收口比让 planner 记得（runbook 条款）更机械——照文档顺序走的 Pod 从「必踩」变「无感」。白名单收窄到本 Pod 治理面，防止误吞他源变更。

**目标文件：`.claude/scripts/merge-driver.mjs`**（改）

### T3 · 投影冲突 --theirs 特判

改什么：rebase 冲突消解辅助：driver 检测冲突文件集时，对 `.claude/taskpackages/current/taskpackage.json`、`.claude/taskpackages/current/verdict.json` 自动 `git checkout --theirs` + `git add`（被合并的 Pod 赢——current 投影语义本就指向最后合并的活跃包）；其余冲突文件维持现有 abort + blocked + 人工消解路径。

为什么：P3 根因 = 结构性必现 + 解法恒定。语义上「被合并包赢」无分叉，机械特判安全；不 untrack current/（OQ3 备选案，动 verify-gates 读取面、riskTier 升，不取）。

**目标文件：`.claude/scripts/merge-driver.mjs`**（改）

### T4 · progress.md 标 merge=union

改什么：`.gitattributes` 增一行 `.planning/kidpal-mvp/progress.md merge=union`（git 原生 union merge 驱动；append-only 纪律下双侧保留即正解）。

为什么：P4 根因纯机械。union 驱动对「双方同区段改动」会交错保留双方行——progress.md 的 append-only 纪律（AGENTS.md Hard Rules「历史记录类文件一律 append」）保证输入形状安全；若未来出现同区段非 append 改写，责任在违反纪律的写入方，union 是该纪律下的正确默认。

**目标文件：`.gitattributes`**（改）

### T5 · pod-coordination.md 补 merge 冲突消解 runbook + post-merge 簿记口径

改什么：`.claude/references/pod-coordination.md`（追加节，不动既有节）：① 「merge 冲突消解 runbook」——driver abort + blocked 后的合法消解序（手动 `git rebase main` → 消解 → `git rebase --continue` → 重置 ready-to-merge → 重跑 `merge-driver --pod … --human-approval-ref …`；强调 rebase --continue 属 driver 流程内消解、不绕任何闸）；② 「post-merge 簿记落点」钉口径 = 落 feature 分支 commit（schedule-p2 `7717eea` 做法转正），main 上 INDEX 一致性由下批 `changes-index.mjs` 全量重建收敛（INDEX 头本就声明全量重建为纠偏手段）；③ T1/T2 落地后 OQ2=(a) 条款成立性复核注记（planner.md 零改动）。

为什么：摩擦的处置手段目前全靠会话记忆；runbook 把「流程外知识」变「文档内知识」。G2 面纯文档，无行为变更。

**目标文件：`.claude/references/pod-coordination.md`**（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 | T1/T2/T3 三个行为补全；消费方 = 所有 Pod 的 merge 段（含 ESCALATE 人批路径）；既有五自检为回归守卫 |
| 仓库属性 | `.gitattributes` | 仓根点文件 | T4 单行追加；消费方 = git merge 驱动选择；与既有 `.codebase-memory` 条目无路径交集 |
| 流程文档 | `.claude/references/pod-coordination.md` | G2 | T5 追加节；消费方 = 各 Pod planner 会话；无行为面 |
| 派生投影 | `.claude/taskpackages/current/*`、`.planning/INDEX.md` | 未触 | T3 只改消解策略不改投影本身；INDEX 维持「全量重建纠偏」既有机制 |
| 升级闸判据 | `supervisor-verdict-gate.mjs` / `pre-merge-escalation-gate.mjs` | 未触 | 本批不承载（ARQ-escalation-cross-system-overfire 走 Track A） |

**【硬项 1b · sweep 透镜】**：本批新增机械断言 = Step 0 白名单判定 pattern（`.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**` 前缀匹配）。census（机械枚举，2026-09-07 起草期跑）：`ls .claude/taskpackages/`（16 目录）与 `ls changes/dev/`（12 目录）实测在册——pattern 覆盖面 = 现存全部 Pod 目录形状（含 current/ 投影，其文件本就在白名单路径下、属合法收口对象）。本批不新增独立门、无退役语面清扫（硬项 1c 不适用）。

## 4. human 取舍点（OQ）

**OQ1 · Step 0 自动收口白名单范围**
- (a) 仅 `.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**`【推荐】——最小面：这两处是 P2 病灶的病灶文件（pod-retro/verdict/pod-events 全在此）；`.planning/**` 的投影钩子维持人工重建先例。
- (b) 加 `.planning/**`——把 progress.md 收口也自动化；但 `.planning` 含 INDEX 日期戳等派生面，自动 commit 会把「该重建的」当「该提交的」固化，噪声大于收益。
- 推荐理由一句话：(a) 只收「真制品」、(b) 会收「派生投影」，机械收口应只碰前者。

**OQ2 · post-merge 簿记落点口径（T5 ③）**
- (a) 落 feature 分支、main 一致性由下批 index 全量重建收敛【推荐】——schedule-p2 实际做法；不需要主线程在场、不占 main checkout。
- (b) main 直接 chore commit——home-p1 做法（`46d99b5`）；要求主线程恰好可用 main checkout（本次因 char-stroke 会话占用而不可行），时序耦合差。
- 推荐理由一句话：(a) 无时序耦合且收敛机制已存在（INDEX 全量重建），(b) 依赖巧合。

**OQ3 · current/ 投影处置策略（T3）**
- (a) rebase 冲突时 --theirs 特判【推荐】——保守：投影仍 tracked、verify-gates 读取面零变化。
- (b) current/ untrack（.gitignore + 门改读各包 taskpackage）——结构化根治，但动 gate 读取面、涉及 fresh-clone 冷启动语义，riskTier 升出本批 low-medium 定位。
- 推荐理由一句话：(a) 零门触碰即消除摩擦，(b) 值得但要单独立项。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **既有五自检复跑零回退**：`node .claude/scripts/merge-driver.mjs --self-check-retro-advisory` / `--self-check-quick-checks` / `--self-check-detect-changes` / `--self-check-rebase-target` / `--self-check-worktree-topology` → 全部 `ok:true`（入口亲核：merge-driver.mjs:977-993，2026-09-07 grep 实录）。
2. **T1 断言**：新增自检/夹具——blocked Pod + 有效 `--human-approval-ref` → 自动 ready-to-merge（stderr 留痕行在场）；无效 ref → 保持 blocked + 核验失败原因输出（fail-closed 向严）；无 ref 的 blocked Pod → 行为零变化。
3. **T2 断言**：夹具——白名单内 unstaged（pod-events.jsonl + pod-retro.md）→ 自动 commit 生成且 `git status` 净；白名单外文件在场 → 维持现有 fail 路径（不吞变更）。
4. **T3 断言**：夹具——构造 current/taskpackage.json 冲突 → 自动 --theirs 解除且 rebase 推进；非白名单冲突文件 → abort + blocked 维持。
5. **T4 断言**：构造双分支 progress.md EOF append → merge 后双侧条目俱在。
6. **T5 验证**：`route-regression-check.mjs` + `lint-descriptions.mjs` 复跑（G2 面惯例）绿；runbook 节落位 diff 人读核对。
7. **landing 后**：`landing-fidelity-check.mjs`（结构门 + 锚清单比对）+ 下一个真实 Pod merge 段实测（机械链行为级验证——本批验收的最终权重在「下个 Pod 一次跑成」）。

## 6. 留根清单

- **ARQ-260907-escalation-cross-system-overfire 不承载**：升级闸 cross-system 判据把 acceptance/deploy 常规面计入跨系统信号、双面板 Pod 恒 ESCALATE——gate 语义变更 + 真设计分叉，走 Track A（`/arch-change`），owner=architect + human 拍板。
- **HOST_ACCEPTANCE_SRCS 顺序承重不承载**：boundary 齿依赖「进程内最先跑」使列表顺序变语义承重、纯 additive 行也不敢自动合并——根治 = seam 加 reset()、boundary 齿自建新鲜 seam，属固件测试基建，归下个碰 host_app 的 dev-loop 包顺手落（ARQ-260907-merge-closeout-bookkeeping-gap 建议改动段有指针）。
- **CMakeLists additive 冲突消解先例**：本 Pod 手动消解的双保留解法（双侧注释/顺序约束兼容性核对）已随 merge `b7df3ab` 入 main，供 runbook 期参考；顺序承重消除前此类冲突仍需人工。
- **自律件（不进模板）**：ARQ 登记与 PROPOSAL 起草同会话完成时，队列行「处理时机」须与 PROPOSAL 承载关系一致（本批三行中两行指向本 PROPOSAL、一行指向 Track A）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
