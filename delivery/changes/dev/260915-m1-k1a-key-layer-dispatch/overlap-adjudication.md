# 并行相交面消解评审（pod-coordination §10d 梯级 ③）

> changeId = 260915-m1-k1a-key-layer-dispatch（M1-K1a 键位对账块①，mergeOrder 22）
> 判官 = architect（dev-loop 合并链路并行相交面消解评审；只读诊断 + 本裁决记录，零代码改动）
> 日期 = 2026-09-15
> 触发 = 前置升级闸 `parallelOverlapProbe` BLOCK `parallel-overlap-tier3`（tradeoff）。
> 落盘形态注：本文件由 planner 按 architect 交付全文转写落盘——architect 写边界不含 delivery/changes/dev/**（enforce-scope 拦截，判官不绕闸，先例 = workstream 门 1 裁决同款处理）。

## 1. 事实面（引用逐字精确）

- 探针：`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json` — `tier: "tradeoff"`，`intersectingFiles` 与 `conflictedFiles` 均仅 `delivery/changes/INDEX.md`，`addAddFiles` 空。
- merge-base = `0551bf0`。main 侧 base 后新增 5 commits（`7bba2ac`→`7a3df8e`，治理批 260915-gov-review-worktree-pool-reuse）；本分支侧 6 commits（`68b2360`→`25e2355`，含两个 feature commit `ffc34c0` / `25e2355` 与拆包批同步）。
- `git merge-tree --write-tree main HEAD` 预测实撞：`CONFLICT (content): Merge conflict in delivery/changes/INDEX.md`——与探针一致，实撞确认。
- **双方对 INDEX.md 的改动性质（本裁决关键事实）**：
  - main 侧 = **单行新增** arch 行：`260915-gov-review-worktree-pool-reuse | 2026-09-15 | landed | yes`（commit `22a25fe`，`7a3df8e` 收口 mergedInMain）；全文件 LF 洁净。
  - 本分支侧 = closeout 投影刷新（integrator 跑 `closeout-check.mjs --update-index`，改动并入 `ffc34c0`）：新增 dev 行 `260915-m1-k1a-key-layer-dispatch | closed | m1-home-qa` + **全文件 EOL 翻转**——HEAD blob 混杂 CRLF/CR/LF（`file` 判读 "with CRLF, CR, LF line terminators"），基线与 main 均为纯 LF。逐行 diff 显示正文语义零差异，全文件重写即 EOL 翻转 + 单行插入。
- **INDEX.md = 投影再生件**：AUTO-GENERATED header 明文「勿手改（投影，源 = 各包 origin / 目录）；手改会被全量重建覆盖」；pod-coordination.md §10b 明文「一致性由下批 `changes-index.mjs` 全量重建收敛（INDEX 头声明全量重建为纠偏手段）」。AGENTS.md 对 INDEX 无快照流向纪律（那是 `.codebase-memory` 的）。
- 主批 footprint（`git diff --stat 0551bf0 main`）：`.claude/{agents,references,skills}/**`、`delivery/changes/arch/260915-gov-review-worktree-pool-reuse/**` 证据、`delivery/changes/INDEX.md`（+1 行）、`delivery/changes/.changes-index-state.json`（+4 行，含本批 mergedInMain 缓存条目）、`docs/architecture.md`（+1 行）——**零 `firmware/**` / `server/**` 触碰**。
- N1b 侧：分支 `dev/260915-m1-n1b-p1-home-battery` tip = `82a6fb9`（拆包批同步 commit，**零 Pod 工作提交**，未碰 INDEX）；门 1 勘误（`delivery/workstreams/m1-home-qa/review.md` 门 1 第 2 条）已备案串行合并协议（KD-A2 与 PH-A3 共编 `firmware/main/display/kidpal_render_node.cc`，串行 merge + rebase + 合并后重跑 host 基线可容）。
- human 已批合并：pod-events `bdf8bcf0-99cc-4fe1-bc98-a0908a130714`（decision=approve-escalated-merge，2026-09-15T11:43:04Z）——human 批的是放行取舍（含 INDEX 撞件走门 1 已备案串行协议、riskTier 如实申报、security-axis 误报判读），tier3 的**语义消解**仍由本记录承载，机械闸不被人批豁免。

## 2. 消解策略裁决

**裁定：merge-driver rebase onto main 途中，在 INDEX.md 冲突点跑 `node .claude/scripts/changes-index.mjs`（裸调用 = 全量幂等重建）消解，不取手工并集、不取 `closeout-check.mjs --update-index`。**

具体序（并入 §10a runbook 语境）：

1. merge-driver rebase 到 main（`7a3df8e`），replay 触发 INDEX.md 冲突；
2. 冲突点在 Pod worktree 跑 `node .claude/scripts/changes-index.mjs`——裸调用走 `main()` 全量重建（collectDev + collectArch 扫目录），`writeFileSync(buildIndexMd(items))` 以 `lines.join('\n')` 写出，**确定性 LF 归一**；
3. 核验两点：① 双方行都在（本包 dev 行〔dev 段计数 23→24〕+ worktree-pool-reuse arch 行 landed|yes，后者由 arch 目录 landing-verdict + `.changes-index-state.json` 缓存现推，rebase 已把 main 侧 state cache 带入）；② `git diff main -- delivery/changes/INDEX.md` 应**只剩本包 dev 行差异**（全文件再无 EOL 噪声）；
4. `git add` + `git rebase --continue`（§10a 步 3 前置：一次性 `git config core.editor true`）；merge-driver 原有 quick-check（contract-lint + acceptance）照跑。

**为什么不用另两个候选**：

- `closeout-check.mjs --update-index`：它是 dev 段**单行 best-effort upsert**，`readText` 保留工作树现行 EOL、不归一——正是本次混杂 EOL blob 的根因路径（`core.autocrlf=true` checkout 物化 CRLF → 插入行裸 `\n` → 混 CR → git 判 round-trip 不安全拒绝 renormalize）。在冲突消解语境它既不覆盖 arch 段、也不修 EOL；用它会把混杂 EOL 留给 N1b 侧下一次 INDEX 交互再撞一次。
- 手工并集：INDEX 是投影非事实源，机械再生是 §10b 已文的唯一收敛权威；手工拼接引入人为转录误差面，劣于机械再生。

**§10a 纪律 4（双方冻结件全量复跑）不触发**：相交面是投影件非代码，对端（治理批）无可执行冻结齿；对端红风险由 merge-driver rebase 后 quick-check 机械覆盖。若 quick-check 红、或重建后 diff 出现双方行以外任何差异 → 存疑向严升梯级 ④（本梯级地板）。

## 3. 合并安全性判断

- **N1b 侧 rebase 不受影响**：N1b 分支零 Pod 提交，rebase 到新 main 为平凡路径；其后续 closeout 刷 INDEX 时基线已是归一 LF——**本消解反而消除** N1b 侧的 EOL 二次撞。KD/PH 共编 `kidpal_render_node.cc` 的真代码相交由门 1 勘误备案的串行合并协议（rebase + 合并后 host 基线重跑）承载，与本 INDEX 消解正交、不受本裁决影响。
- **主批与本包 firmware 零交互**：主批 footprint 全在治理面（.claude/** + arch 证据 + INDEX + state cache + architecture.md 单行），不触任何实现文件；无代码级交互风险。主批的流程面变更（pod-coordination §2c worktree 池复用）是 launch 侧协议，不回溯影响本包已完成的 closeout 证据效力。
- **附带发现（root cause 留根，不在本裁决内修）**：`closeout-check.mjs` `upsertIndexRow` 不做 LF 归一 + `core.autocrlf=true` → 混 EOL blob 进版本库，git 拒绝 renormalize。建议随本包 retro 提共性、登 ARQ：给 `upsertIndexRow` 写出前归一 LF（对偶 §10a 纪律 3 current/ 投影归一先例）。architect 不就地改脚本（实现走治理批通道）。

## 4. 裁决

**GO**（有界条件）：

- 消解 = 上述第 2 节四步（rebase 冲突点跑 `changes-index.mjs` 全量重建 + 两点核验 + quick-check 照跑）；
- GO 的边界：quick-check 绿、重建后 INDEX diff 仅剩本包 dev 行、无其它冲突文件（探针 addAdd 为空、唯一实撞即 INDEX）——任一不成立即回 BLOCK 升 ④。

依据一句话：唯一相交面是**机械可再生的投影件**，双方改动是两段互不冲突的新增行，冲突本体是 EOL 翻转噪声——机械再生一次性消掉信号与噪声，且是 §10b 已文的权威收敛路径，无语义取舍残留。
