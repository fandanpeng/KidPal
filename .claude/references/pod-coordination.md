# Pod Coordination — blackboard / merge token / reconcile（canonical mechanics）

> 状态：canonical dev-loop 协调机制参考。落定自 `delivery/changes/arch/dev-loop-os-rte/PROPOSAL.md`（FROZEN v2.1）§7–§9。
> 上层定义见 [`dev-loop-operating-system.md`](dev-loop-operating-system.md)；本文件只讲多 Pod 并行的协调**机制**。角色见 [`.claude/agents/portfolio-planner.md`](../agents/portfolio-planner.md)。

## 1. 原则

portfolio-planner **不是**常驻盯文件的 session；agent 之间**不**互相发消息（平台无此原语）。协调用 **blackboard = 共享 repo 文件**，agent episodic 读写，靠状态而非活会话。

## 2. 文件布局（协调态在 git common dir，不入版本控制）

**问题**：每个 Pod 在独立 `git worktree`，被 git 跟踪的文件是**各 worktree 独立副本**，互相不可见。故协调态**不能**放被跟踪路径（否则各 worktree 看不到彼此，reconcile 无法跨 Pod 聚合）。

**运行时协调态**放所有 linked worktree 共享的 git common dir，不入版本控制：

```text
$(git rev-parse --git-common-dir)/devloop/board.json            # 聚合视图 + merge token（合并进 board，不单列 merge-lock）
$(git rev-parse --git-common-dir)/devloop/pods/<change-id>.json # 各 Pod 生命周期状态（该 Pod 单写）
$(git rev-parse --git-common-dir)/devloop/reconcile.lock        # advisory lock（mkdir，见 §4）
```

`git rev-parse --git-common-dir` 在同机所有 linked worktree 解析到**同一目录**（主仓 `.git/`），跨 worktree 共享且不会被误 commit。

> 实现备注：主 worktree 下该命令返回**相对** `.git`，linked worktree 返回**绝对**路径；脚本必须对结果做 `path.resolve(cwd, commonDir)`（或用 `--path-format=absolute`），否则主 worktree 在非 repo-root cwd 下解析错。

**耐久证据**仍入版本控制：post-cutoff 为 pod-events / pod-retro / taskpackage / review 制品；pre-cutoff 继续兼容 session-digest / loop-health / pod-journal。它们落 `delivery/changes/dev/<change-id>/` 与 `delivery/taskpackages/<change-id>/`，随 Pod worktree 合并进 main。

> 跨设备不靠 live board：本机多 worktree 用共享 git common dir；跨设备协作 post-cutoff 走 handoff + pod-retro（叙事连续性）+ pod-events（结构化证据），pre-cutoff 仍可用 session-digest；不共享运行时 board。

## 2b. Launch 前置门（commit-before-launch 校验）

portfolio-planner 交 launch 前（见 [`portfolio-planner.md`](../agents/portfolio-planner.md) Step 6 commit-before-launch 不变量），launch 序列加**前置门**：launch 前对每个 ready 包跑 `git cat-file -e <base-ref>:<path>` 校验**基准 ref**（专用 base ref，非 main）已含 check 集：

- **check 集 + 分层（defender 倾向 / human A-3 裁）**：
  - `taskpackage.json`（`delivery/taskpackages/<changeId>/taskpackage.json`）= **blocking**
  - PROPOSAL（`origin.upstreamRef` 解析）= **blocking**
  - `decision-coverage.json`（`delivery/workstreams/<ws>/decision-coverage.json`）= **warn**
- `origin.upstreamRef` 校验前按 task-intake DP-1 **剥片段**（`:S-Fx.y` / `:S-Fx.y-ACn` 等锚点，复用不新造；句法见 task-intake「story 锚点引用与转写」节）；PROPOSAL 路径**双路解析** `delivery/workstreams/<ws>` ∨ `delivery/changes/(arch|dev)/**`（复用 `isProposalMdPath`，change-triage.md:106）。
- **`launchPreconditions` 核验 = convention + 人读（本门不含机械齿，260912 拆包链 M-1，260913 human 裁）**：`--check-launch` 的机械 check 集**不读** workstream `decision-coverage.json` 顶层 `launchPreconditions`；该面由 **launch 执行者（human 点火前）人工核**——逐条核前置是否满足，其中 `需修订` 类 spec 裁定按落点二分核对应文件已冻结（story 语义 → `docs/requirements.md`、支撑语义 → `docs/architecture.md`）。未满足 → 承载包不得 launch（粒度 = 包，非整 workstream 陪等）。canonical 定义见 [task-intake-and-slicing.md](task-intake-and-slicing.md)「launchPreconditions」节；机械齿（脚本读该键）另登 ARQ 留根，人读反复漏检再机械化——**本门判定语义不变**。
- **board 不入 check 集**：board/token 恒在 git common dir（§2）、不入版本控制，其可见性由 common dir 给、不靠 commit；board 失同步由 planner intake-reconcile 修。
- 缺失（blocking 项）→ **拒绝 launch**，报「拆包产物未 commit 到基准 ref」，回 portfolio-planner 补 commit（对偶 planner.md F-09 needs-triage 默认出口）。

> 机械承载（2026-08-25-gov-review-launch-reachability-guard T1 落地）：`node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref <ref>`——launch 执行者在新建 worktree **或复用换包**前跑，blocking 缺即 exit 1 拒绝（对偶本节 `git cat-file -e` 语义）。

> **门归属（非第 7 工程门）**：本前置门是 launch 编排的**结构性前置条件**——判者 = `git cat-file -e` 确定性退出码机械检查（制度族，对偶 `taskpackage-required-check.mjs` 起包结构闸），非 human judgment 门。DEV-GOVERNANCE §5「工程门 = 6 行」折进注（G1，human-only）明文其为「launch 编排结构性前置条件，非第 7 工程门」；本文件只引其相位，不落折进注。

## 2c. worktree 池复用（换包协议 · 260915-gov-review-worktree-pool-reuse T2）

**worktree 与 TaskPackage 无结构绑定**：board 的 worktreePath 是 launch 侧预填的运行时记录（见 [`portfolio-planner.md`](../agents/portfolio-planner.md) Step 6 ③——launch 方是唯一知道 worktree 绝对路径的实体），树是新建还是复用对下游机械透明；merge-driver 经 `pods/<change-id>.json` 的 worktreePath 定位 + 读 Pod worktree **当前所在分支**并入 main（不认 `dev/<changeId>` 名字）。

换包协议四条（操作手册 = [`kidpal-pod-launch-prep` skill](../skills/kidpal-pod-launch-prep/SKILL.md) 步骤 3）：

1. **空闲判读**：树上有会话在跑即占用（一个目录一个会话，并行同树在操作层不可达）；board 的 worktreePath 是**记录不是锁**，不设独占校验门。
2. **一树一常驻分支**：每棵树一个常驻分支，包与包之间不换不删——上一包 merged → 直接续跑（merge-driver rebase 步已把常驻分支 rebase 到 main）；上一包 abandoned → 先 main→base 同步（显式前置一步），再 `git reset --hard <同步后的专用 base 分支>` 复位清半成品（复位目标非 main——main..base 含未 launch 包的拆包制品）。
3. **交接卫生**：换包前 tracked 改动须净、untracked 清理，白名单保留 `build/` 与 `managed_components` 软链（host build 热缓存随树）。
4. **按需扩容**：建新树决策归 human（评估并行收益 vs 一棵树的 host 冷编译成本）；每树各自常驻分支。

与既有机制的相容：ARQ-260905 每 worktree 专属图索引随树复用、自动增量刷新；快照流向不涉 Pod 树、不变；分支亲核语义见 [`integrator.md`](../agents/integrator.md)（常驻分支口径）。

## 3. Pod 状态机

```text
queued -> running -> verifier-green -> ready-to-merge -> merging -> merged
                                        \-> blocked / abandoned
```

它是持久标记，不是活内存；board 只是聚合视图。

## 4. 单写者 + 原子写 + advisory lock

- **Pod 只写自己**：hook 触发时 Pod 只写自己的 `devloop/pods/<change-id>.json`（本 Pod 单写，天然无冲突）。**hook 绝不写 board**。
- **board/token 单一逻辑写者 + 串行化**：聚合 board、轮转 merge token 只由 `portfolio-reconcile.mjs` 做，用 **advisory lock 串行化**——起手 `mkdir` 抢 `devloop/reconcile.lock`（原子、`EEXIST` 即失败），抢不到就幂等 `exit 0`（下次写会再触发）；写 board 用 **temp 文件 + `rename`** 原子替换；结束删锁。多个 Pod 的 hook 并发触发时只有一个真正跑，其余无害退出，无竞态。
- **reconcile 幂等**：每次从全量 `devloop/pods/*.json` 重算 board，不做增量——漏跑/重跑都安全。
- **lock stale 检测**：`mkdir` 锁不会在持锁进程崩溃后自动释放（reconcile 死在 `rmdir` 前 → 锁永久残留 → 后续全 `exit 0` = 死锁）。故 `reconcile.lock` 目录带 mtime：其龄超过可配阈值（默认 60s，远大于一次 reconcile 时长）时可被后续 reconcile 强制 `rmdir` 再重建。与 §8 token TTL 同构，均靠持久态 + mtime，不靠进程存活。

## 5. 通讯 = 写文件

Pod 报「我 ready」= 写自己的 `devloop/pods/<change-id>.json`；reconcile（hook 触发或 human/portfolio-planner 手动）聚合所有 pod 状态、更新 board、轮转 merge token。不是活会话对话、不是 agent 间消息。

## 6. 合并串行化

- supervisor 是 **per-Pod**：每个 Pod 到合并门各跑一次；不是某 Pod 独占、也不是全局合并器。
- **Pod 内交付闭环**：每个 Pod 在自己的 worktree 内跑完 acceptance→builder→verifier↔fixer→integrator→**supervisor**。supervisor APPROVE 后才标 `ready-to-merge`。
- **合并门被 merge-driver 串行化**：N 个 Pod 并行开发、各自闭环；planner 在 supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后自动触发 `node .claude/scripts/merge-driver.mjs --pod <changeId>`，按 mergeOrder 逐个对 ready-to-merge Pod 执行 rebase→quick-check→merge。
- 顺序由 portfolio-planner 定（依赖序、触共享文件/契约面者优先、冲突面最小化）；它只**排序 + 初始化 board**，不替 supervisor 判过/不过。
- **正确性关键**：merge-driver 在 merge 前 **rebase 到解析出的最新合并基线**（本地 main 优先，origin/main 领先时切 origin/main——含已落地前序 Pod），并跑 quick-check（contract-lint + acceptance，260905 对齐后两门案；firmware-build 有意不入冒烟——分钟级 + fail-open，由 verifier scoped 相位承担）抓 rebase 后集成问题。supervisor 已在 Pod 内复核过 closeout 制品；quick-check 是 rebase 后的补充验证。
- 跨 Pod 冲突在 **merge-driver 的 rebase+quick-check** 处暴露解决。rebase 冲突 → blocked（人工介入）；quick-check 失败 → 重试一次，仍失败 → blocked。
- gate 权在 supervisor（APPROVE/BLOCK/ESCALATE），落地走 merge-driver 的本地 --no-ff 可回滚路径；portfolio-planner 不 merge、不越 gate。

> 一句话：**并行开发、各自闭环，串行合并；portfolio-planner 定序并初始化 board，planner 跑到 supervisor APPROVE 标 ready，merge-driver 逐个 rebase+quick-check+merge，谁都不能并行直落 main。**

## 7. merge token 消费协议

planner（Pod 内单会话）**不轮询、不等待**：

1. planner 在自己的 worktree 内跑完 acceptance→builder→verifier↔fixer→integrator→**supervisor**。supervisor APPROVE → 写 `pods/<change-id>.json` 状态 `ready-to-merge` → **退出**（不自己进合并门、不自旋）。
2. merge token 的**消费者是 merge-driver（`node .claude/scripts/merge-driver.mjs`）**，由 planner 自动触发（单 Pod `--pod`，经前置升级闸放行）。merge-driver 启动时**无条件 reconcile** 获取最新 board：
   - 从 `ready-to-merge` 的 Pod 里按 merge order 选第一个 → 该 Pod 持有 token。
   - 对当前 holder Pod：读 `pods/<change-id>.json` 的 `worktreePath` 定位 worktree → rebase → quick-check → merge → 标 `merged`。
3. 该 Pod merged → reconcile 清空 `mergeToken.holder` → 下一轮选下一个（直至无 ready Pod）。

> token 是「谁现在可以进合并门」的持久标记，被 merge-driver 读取；planner 只标 ready 就走人，避免任何自旋/常驻。

## 8. merge token 生命周期状态机

`mergeToken = { holder: <change-id>|null, grantedAt: <ts> }`。转移：

| 事件 | token 动作 |
|---|---|
| reconcile 选中一个 ready Pod | `holder=<id>`, `grantedAt=now` |
| supervisor **APPROVE** → merged | `holder=null`（释放，选下一个） |
| supervisor **BLOCK** | **立即释放 `holder=null`**；Pod 回 fixer，`ready-to-merge`→`blocked`，修好再 `ready-to-merge` 重新排队。**token 不在 fixer 循环期间被占用**（防自锁）。 |
| supervisor **ESCALATE** | `holder=null` 释放；Pod 挂 `blocked`（等人类），不占 token。人批后重占 token 走 merge-driver 人批路径（`--human-approval-ref`，见 planner.md 合并门段）。 |
| holder Pod **崩溃/abandon**（`grantedAt` 超 TTL 且 `pods/<id>.json` 未推进 / mtime 陈旧） | reconcile **回收** `holder=null` + 标 `abandoned`，升一条告警。 |

> 防饿死：被 BLOCK 退回 fixer 的 Pod 修好重新 `ready-to-merge` 时**保持原 merge-order 位次**（不排队尾），避免新 Pod 持续插队导致旧 Pod 饿死。TTL 须 > 正常 rebase→integrator→supervisor 合并门时长（避免误判回收）；因 ESCALATE 已立即释放 token，不存在「持 token 等人类慢审」的长窗口。

## 9. Watcher / 触发器（便宜档，不建 daemon）

- **Decomposition（拆包）**：需判断 → agent/human，episodic 手动，无法也不该 hook 化。
- **Scheduling / merge-token 轮转**：确定性规则（依赖序、共享面优先、WIP limit、rebase 校验）→ 确定性脚本 `portfolio-reconcile.mjs`，可被 hook 触发。

v1 决定（加便宜档）：

- 建**确定性 `portfolio-reconcile.mjs` + in-session PostToolUse hook**（`portfolio-sync.mjs`，照抄 `planning-sync.mjs`：写 `devloop/pods/<change-id>.json` 后触发 reconcile，非阻塞 `exit 0`，内部抢 §4 advisory lock）；
- **不建**常驻 daemon/cron；
- 判断类 decomposition 仍手动；hook 只自动化确定性的 scheduling/token 轮转；
- 触发器可换（manual→hook→cron→CI 同一个 reconcile），并发量上来再升级，零架构成本。

**毕业条件（量化，任一满足即评估升级）**：并行 Pod 稳定 ≥3、或 merge 频率 ≥2/day、或出现 ≥1 次「会话结束后无人 reconcile 导致 Pod 卡 >TTL」的实测事件。升级只换触发器（同一个 `portfolio-reconcile.mjs`），不改数据结构。owner=architect（随 loop-health 复盘评估）。

## 10. merge 冲突消解 runbook + post-merge 簿记口径（260907-gov-review-merge-friction T5）

### 10a. merge 冲突消解 runbook

merge-driver 因冲突 abort + blocked 后，合法消解序（全部属 driver 流程内消解，**不绕任何闸**——闸在 Step 1b，重跑时照走）：

1. 在 Pod worktree 手动重放：`git rebase <target.ref>`（target = driver 当时输出的 rebase target）；
2. 逐冲突消解——current/ 投影冲突（`taskpackage.json` / `verdict.json`）恒取被合并 Pod 侧（rebase 语境 `--theirs`）；progress.md 已配 union 自动保留双侧；其余文件（如 CMakeLists additive）人工按语义消解并核对兼容性（先例：b7df3ab 双保留解）；
3. `git rebase --continue`（循环至 rebase 完成；前置 = 消解开始前在 Pod worktree 一次性 `git config core.editor true`，使素形 continue 免交互且命中 settings ask 前缀——`GIT_EDITOR=true` 环境前缀形态不命中 `Bash(git rebase:*)` 前缀、不弹窗，禁用；`git -c core.editor=true` 同理不命中，须 config 落盘形态）；
4. `portfolio-reconcile.mjs --set-pod <id> --status ready-to-merge` 重置状态；
5. 重跑 `merge-driver.mjs --pod <id> --human-approval-ref <eventId>`（ESCALATE 包凭据不变，仍有效）。

**消解纪律五条（260908 批补，与上五步配套）**：

1. **形态钉死**：Pod 分支集成一律 rebase 形态、**禁在 Pod 分支引入 merge commit**——Pod 分支内含「并 main」的 merge commit 与 merge-driver 的朴素 `git rebase main` 结构性不兼容（260829-schedule-reminder 实测：`Rebasing (1/17)` → 撞冲突 → abort + blocked，合并 fail-closed 无损但空转；线性化后同命令 up to date）。
2. **graph 缓存**：`.codebase-memory/{graph.db.zst,artifact.json}`（`merge=ours`）内容漂移会让 merge/rebase 起手中止——调 merge-driver 命名导出的 `ensureGraphCacheSynced`（三拍：no-skip-worktree → `git checkout HEAD --` → 复旗标），不自行复刻。
3. **current/ 投影**：恒取被并包侧且**归一 LF**——`core.autocrlf=true` 会把 `git checkout --ours/--theirs` 物化成 CRLF，而 `taskpackage-required-check.mjs:143` 用 sha256 逐字节比对 canonical ↔ current，CRLF 即翻红；merge-driver 投影特判已内置 LF 归一（260908 批）。
4. **消解完成后复跑**：**双方**冻结件（本包 + 对端包）+ 全档 pytest + host ELF（文档命令位）+ 设备真编 + 九闸脚本并回报实测数字——只跑本包档不算完成，对端红漏检是实测陷阱（消解副作用只在对端冻结齿上显形，260829 实测）。
5. **版本化制品 add/add**：禁 `--ours`/`--theirs` 单边消解——单边会让被丢正文一侧的 needle 齿偶然命中而假绿；消解 = 取并集 + 复跑双方 carrier 齿并留 exit code；槽位预约与登记表见 §10c。

人批后首跑 no-op（blocked 未消解）不应再出现：driver 入口对 `--pod X + --human-approval-ref` 且 X=blocked 自动核验消解（Step 1b 同路径，核验失败 fail-closed 保持 blocked）。

### 10b. post-merge 簿记落点口径

- **pod-events 的 merge-completed 事件**：merge-driver Step 5b 机械落 main（合并现场单文件 pathspec commit）——main 自持、feature 分支可删，planner 无需手动补（home-p1 `46d99b5` / schedule-p2 `7717eea` 的手动簿记由此机械化）。簿记失败为 advisory warn、不阻塞合并链。
- **delivery/changes/dev/INDEX.md**：不由 Step 5b 动；一致性由下批 `changes-index.mjs` 全量重建收敛（INDEX 头声明全量重建为纠偏手段）。INDEX 已按 lane 拆分（dev / arch 各一份），Pod 合并流只触 dev 侧。
- **merge 前收尾制品**（pod-retro / supervisor-verdict / pod-events 追加）：Step 0 白名单自动收口 commit（`delivery/taskpackages/<pod-id>/**` + `delivery/changes/dev/<pod-id>/**`，目录段精确等值）；白名单外变更不吞、维持 fail。

## 10c. 版本化制品 `.vN` 槽位预约与登记表（260908 批，A-7/B-2/B-0 已裁）

**槽位预约机制**：Pod launch 时在 pod-events 记 `versioned-artifact-claim: <path>@vN`（自由事件型）；后到 Pod 见在先 claim 即从 **vN+1** 起编，或主动等待前包合并后重取基线——把「谁占哪一版」从两包各自看 deploy 目录猜，变成机械可查。

**双源关系（A-7 已裁）**：本登记表为 **authority**、`*.v[0-9]+.md` 形态正则为兜底——不一致时赢家 = 登记表；正则角色 = merge-driver 版本化制品 add/add 特判命中集的形态兜底，登记表外新形态（无 `.vN` 后缀的版本化物，如 engine-patch）退回通用 blocked fail-closed（补集不变式：特判只 widen 指引文案、不 widen 放行）。

**登记表（初始清单 = 实测超集，F9 修向）**：

| 路径形态 | 类别 | 说明 |
|---|---|---|
| `server/deploy/openclaw-system-prompt.vN.md` | prompt 载体 | 当前唯一带 `.vN` 后缀的版本化物（v1/v2/v3 在册） |
| `server/deploy/engine-patch/**` | 引擎部署副本补丁 | add/add 时退通用 blocked（正则不命中、登记表兜住指引语义） |
| `server/deploy/openclaw-patch/**` | 引擎部署副本补丁 | 同上 |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | 无扩展名 carrier | 无 `.vN` 后缀，特判正则不命中、登记表兜住 |
| `server/deploy/provider.config.yaml` | 接线面配置 | bump 时 add/add 风险面 |
| `server/deploy/apply-xiaozhi-overlay.sh` | 接线面脚本 | bump 时 add/add 风险面 |

**维护三句（B-2）**：owner = 批主线程（触碰登记表所列形态的批 landing 时随批更新）；更新触发 = 批版本化制品路径集与登记表 diff 非空；vN 回收 = 不做机械回收、槽位由 forward-bump 语义由后版覆盖（实测需要再另行立项）。

**单一权威互指注记（B-0）**：本节为版本化制品消解口径单一权威；merge-driver 特判指引文案与 integrator.md 消解段为引用方，改本节口径须同批核两引用。

## 10d. 并行相交面分级路由 + 取舍判据（260908 批 ladder，ARQ-260907-escalation-cross-system-overfire 判据换轴）

升级闸 `parallelOverlapProbe`（只读 merge-base 交集 + merge-tree 实撞，`--probe-out` 落 overlap-probe.json）按裁决深度分级；**cross-system 目录面启发式已退役**（对双面板 Pod 恒真，home-p1 误报实证），真跨系统保护由本节路由承担：

| 梯级 | 情形 | 裁决者 | 通道 |
|---|---|---|---|
| ① | 零相交（与 main 含同批已并 Pod） | 机械放行（driver rebase + quick-check 照走） | — |
| ② | 相交·无取舍 | 机械（不升人） | git 自并 + **双方冻结件 + 全档复跑**（对端红纪律）；实撞与探测不一致 → 存疑向严升 ③ |
| ③ | 相交·有取舍（同 hunk 语义单元相撞） | **architect 消解评审**（建议即地板，不落地） | gate BLOCK `parallel-overlap-tier3`，升级件带相交清单 + 双方 diff；派发失败/超时 → 直接 ④ |
| ④ | architect 判不了 / 齿红 / 需动语义 | **escalate human** | 升级件带逐文件相交清单 + 双方版本正文 + architect 意见 + 消解选项；人批走既有 `--human-approval-ref` |

**取舍机判判据**：**无取舍** = 相交文件双方改动落在不同 hunk / 不同语义段（git rebase 可自动并；`.vN` 载体不同段 / append 类按此判）；**有取舍** = 同 hunk 语义单元相撞（同 key 异值 / 同锚互斥断言 / 版本字面互斥——后者复用 260908 批 guard-class wiring 判据）；**已登记 §10c 登记表形态的版本化制品 add/add 恒归 ②**（复用 merge-driver 特判并集），未登记形态归 ③（补集不变式 fail-closed）。判不了 = 存疑向严升 ③。

## 11. 锚 3 · demo 对账与 roadmap 达成状态（260912-gov-review-portfolio-decomposition-chain T4，FROZEN v1.0）

**demo 对账行（包收口必含）**：integrator 的 review-packet / supervisor-verdict 闸消费面必含一行 demo 对账 = **demo 证据**（形态按 `roadmap.md` ③ 节本包拆包时定的：模拟器截图 / host runner 输出 / 串口日志 / 真机轮）+ **与本包效果段的一致性结论**。义务的生产者可达性由 **planner Step 11 派 integrator 的 dispatch prompt 显式携带**（与锚 2 brief 携带效果段同构，派发侧传递；integrator.md 不因此改）。

**未覆盖面申报（包收口必含 · 260920-gov-review-bearing-strength-align T3，FROZEN v1.0）**：与 demo 对账行**同址、正交**的第二轴——demo 对账行管**证据形态轴**（有什么证据、与效果段一致否），本段管**齿覆盖轴**：对照本包 `acceptance[]` 集合 + 本包引用的契约条目 + `roadmap.md` §3 效果段，列出**本次无齿 / 未跑的设计性质**，逐条一行写「性质 + 现状 + 去向（deferred 到哪包 / `structural` 豁免 id / 由真机轮或 human 持板兜 / 明确接受不覆盖）」；**零缺口时明写 `none`**（防「没写」与「没有」混同）。**为什么单列一轴**：`acceptance-coverage` 只对**已声明进 `acceptance[]` 的条目**给豁免清单（`structural` / `suiteGates`）、demo 对账行只看**证据形态**——若某设计性质**根本没进 `acceptance[]`**，二者全部沉默；本段的对照基准含**设计面**（契约条目 + 效果段），故能给出差集。义务的生产者可达性同样由 **planner Step 11 的 dispatch prompt 携带**（与 demo 对账行同构、同一次携带；`integrator.md` 不因此改）。落点与分级口径见 `dev-loop.md`「### 合并门 Review Packet」必列项。

**真机轮 / 串口日志类 demo 证据的读法（260918-gov-review-dispatch-chain-friction T1③）**：判「串口日志 / 真机轮」形态的 demo 证据时，`.claude/skills/kidpal-serial-monitor/SKILL.md` 的「Device round protocol / 真机轮协调协议」一节载有**捕获窗口 / DTR-RTS 复位伪影识别 / 动作点口令**三条姿势——知道它们有助于**判该证据本身的质量**（例：与捕获启停**秒级吻合**的「重启」是端口复位伪影、非真重启，据此可判证据是否被误读）。该 skill 的**主通道是它自身的 description 触发**（真机轮必含串口读），此处指针是**收口 / 对账读者的次要发现路径**；**不转抄正文**（防双源漂移）。

**不一致 = 就地升裁，禁攒到集成夜**：走既有 escalation 通道（`agent-escalation` → 需要人则 ESCALATE 停 human），不新设通道。human 裁断后 planner 落一条 pod-events 事件——**既有事件型** `human-decision` + `payload.decision="anchor3-deviation-ruling"`、append 姿势 `--actor human`（eventId 即裁定锚；eventType 是 `pod-events-append.mjs` 机械闭集，不扩闭集，沿 `approve-escalated-merge` 先例形态）。roadmap 达成状态由 `render-roadmap.mjs` 按该区分值渲染为 `deviation（裁定锚 <eventId>）`。

**绿路径上 portfolio-planner 沉默**：常规「下一包 ready」由 `portfolio-reconcile.mjs` 机械计算（依赖已满足 + WIP 未满 + mergeOrder 最靠前），human 照常点火（`git worktree add` + 起 planner，现行流程不变）；与 roadmap 的匹配判断已在锚 3 于**合并前**完成，不后置到 merge 之后。roadmap 达成状态刷新时机 = 包 merge 后由 human 合并簿记顺跑一次 `node .claude/scripts/render-roadmap.mjs --workstream <slug> --write-into delivery/workstreams/<ws>/roadmap.md`（全量重算、幂等），门 1 与收口相位亦可调用。

**portfolio-planner 再出场条件（收窄）** = 仅**调度异常**时由 human 唤起重裁：① 锚 3 deviation 影响后续包（依赖它的包是否照原计划跑）；② 依赖关系与实际不符；③ WIP / mergeOrder 需重排。不 per-merge、不 per-launch 例行出场。

## 相关

- [`dev-loop-operating-system.md`](dev-loop-operating-system.md)
- [`.claude/scripts/portfolio-reconcile.mjs`](../scripts/portfolio-reconcile.mjs)
- [`.claude/hooks/portfolio-sync.mjs`](../hooks/portfolio-sync.mjs)
- [`.claude/agents/portfolio-planner.md`](../agents/portfolio-planner.md)
