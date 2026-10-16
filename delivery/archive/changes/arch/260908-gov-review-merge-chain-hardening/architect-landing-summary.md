# architect-landing-summary · 260908-gov-review-merge-chain-hardening

> 主线程 packet-bound landing 执行回报（2026-09-08）。蓝本 = FROZEN v1.0 PROPOSAL（`af0ac8e`）+ 附录 A 逐字正文；落点 = 8 manifest 路径；落地 commit = `b89bf33`。
> packet-id = 260908-gov-review-merge-chain-hardening §2 改法集（T1–T7）+ §5 验证集（同 PROPOSAL packet-binding 行）。

```landing-manifest
.claude/scripts/merge-driver.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/references/pod-coordination.md
.claude/references/acceptance-evidence.md
.claude/agents/integrator.md
.claude/dev-loop.settings.json
docs/ARCH-REVIEW-QUEUE.md
```

## 0. verdict 转绿修复记录（2026-09-08，landing-verdict RED 五条件逐条）

1. 制品名归位（landing-summary.md → architect-landing-summary.md，git mv）+ 本 manifest 围栏块 + packet-id 行补齐。
2. PROPOSAL T3 锚行或锚 fail-loud：括注内 `docs/contracts/**` / `docs/spec.md` backtick 去 backtick 转散文提及（冻结层格式修正——verifier 列为转绿条件追认；落地正文零改动）。
3. V1-② 行尾 sha256 夹具 + V2-④ N5 背书夹具补齐 = merge-driver 自检 T8a/T8b/T8c（20/20 全绿；Step 1b 背书结构性注记在自检注释）。
4. acceptance-evidence.md:12 掉字修复（「措辞守卫」补「卫」）。
5. 队列 §2 结移行插断表头修复（行/分隔符换位）。

## 1. 落点对照（manifest 8/8）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/merge-driver.mjs` | T1 + T6 | ① 版本化制品 add/add 特判：纯 helper `detectVersionedAddAdd`（AA + `.vN.md` 正则）+ `VERSIONED_ADD_ADD_GUIDANCE` 并集指引，接线 rebase 与 merge 两处冲突点（abort 前探测、命中打显式指引）；② 投影消解 LF 归一 `normalizeLf/normalizeLfFile` 接线投影特判；③ `ensureGraphCacheSynced` 命名导出；④ `resolveUnlockWorktree`（A-3 已裁：findWorktree 失败显式 errorExit、删 `|| projectDir` 回退分支）+ `approvalRefUnlock` 重写（T6 import `resolveHumanApproval` 单源五要素核验，N5 收窄申报句在注释）；⑤ 自检新增 T7 段 7 断言（T7a-g） |
| `.claude/scripts/lib/governance-gate-shared.mjs` | T6 | B8 段纯新增并入：`resolveHumanApproval` 迁入（gate 原函数体逐字 + deps.decision 参数化，B-6）；`existsSync` 补入 fs import；既有导出零改动 |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | T5 + T6 | ① `resolveHumanApproval` 本地定义删除、改 lib 具名导入；② T5 `contractFindingsAccounted` helper + evaluate 通用 4 分支（`contract-findings-unaccounted`，注释钉 A-8 只拦可识别形态 + B-4 Basis 前缀固定串比对）+ buildCtx 字段 + 自检 +3 例（21/21）；③ T6 import.meta 守卫（`isDirectRun()`）包 CLI 入口 |
| `.claude/references/pod-coordination.md` | T2 | §10c 新节（槽位预约机制 / 双源关系 authority / 登记表实测超集六行 / 维护三句 / 单一权威互指注记）+ §10a 补「消解纪律五条」（形态钉死 / graph 缓存三拍引用 / LF 归一 / 双方冻结件复跑清单 / 版本化制品禁单边）+ 第 3 步改素形 `git rebase --continue` + core.editor 一次性设置句 |
| `.claude/references/acceptance-evidence.md` | T7 | 「必要证据」段末追加版本无关 green-guard 三步条款 + guard-class token 格式 + 母版与清点范围句；「反例」节挂硬写版本字面反例（正形指针） |
| `.claude/agents/integrator.md` | T3 | 「Merge-Chain Execution / 集成链执行」节（附录 A.1 逐字：三前置缺一即停 / 四铁律逐字 / 消解执行三条 / 复跑清单 / fs-write 收口归属）+ :42 B-4 句（hard-violation 且 Basis 涉契约/spec 须以 docs/contracts/** 或 docs/spec.md 字面前缀起引） |
| `.claude/dev-loop.settings.json` | T4 | `permissions.ask` 两形（`Bash(git merge:*)` / `Bash(git rebase:*)`）+ `_doc` 加批准判据行 + 主张限定句（N6/A-4 回填义务 ①③） |
| `docs/ARCH-REVIEW-QUEUE.md` | V5 簿记 | 两条 260907 merge ARQ 结移 §2（含 260907-gov-review-merge-friction 承载批引用 + 核销确认）；B-8 重指排队行（versioned-guard-repoint-execution）入 §1 开放表 |

## 1b. 落地中的事实性偏差（如实申报，交 landing-verifier 复核）

1. **V4 首条断言大小写不符**：冻结 V4 写 `grep -c "merge-chain execution"`（小写），附录 A.1 标题为「Merge-Chain Execution / 集成链执行」——落盘以附录 A.1 逐字为准（题行大写），断言按「Merge-Chain Execution」字面验（=1）。**不改冻结稿**，偏差如实在此申报，请 verifier 复核判「落地忠实于附录 A.1、断言笔误」。
2. **V6 演练形态适配**：merge-driver 顶层自执行 `main()`（无 import 守卫——T6 只薄壳化 gate），V6 演练改为「临时仓真实 add/add 场景产出 porcelain → 与仓内自检 T7a 实测输入形状逐字对账 + reflog 零 ours/theirs」；合并链等价：driver 冲突点接线已由 T7a-g 单元夹具 + 两冲突点接线 diff 亲核承载。
3. **T7③ 重指执行不在本批**：按冻结条款只登记清单与规约（A.3 八实例「待重指 4 队」），重指执行 = B-8 排队行（队列 :56 行）承接、逐件 human 授权——零冻结件改动（§0 非目标红线保持）。
偏差仅此三条；其余落点与冻结稿逐字对账。

## 1b 侧注：修订统计

10 文件 +345/−55；跨文件接线核（merge-driver ↔ lib ↔ gate）：gate/driver 均具名导入 lib B8 `resolveHumanApproval` 单源、既有消费面零破坏（七消费方自检全绿）。

## 2. 验证集执行记录（V1–V6）

| 项 | 结果 |
|---|---|
| V1 merge-driver 自检 | 七套全绿：retro-advisory 3/3、quick-checks 对齐、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9、merge-friction **20/20**（11 旧 + T7 段 7 新 T7a-g + T8 转绿补件 3 新 T8a-c） |
| V2 gate 自检 + lib | gate 21/21（18 旧 + T5 两态 3 新）；lib import probe OK（顶层零副作用）+ `process.argv` 零命中（V2-② 辅断言） |
| V2-③ 七消费方名册 | landing-fidelity-check / proposal-consolidation-check / closeout-check / taskpackage-required-check / devloop-artifact-check / changes-index / portfolio-reconcile 七套 --self-check 全绿；名册复跑零回退（N2 名册六改七） |
| V3 settings ask 探针 | ask 两形在 settings 逐字在场（V4 同款 -cF 断言）；「授权事件在场」条件语义机械层不判（T3 正文纪律承载，A-4 已裁） |
| V4 投影 grep | 正断言 8 项全过（列 1b 偏差 1 的大小写注记）；deny 两行 -cF = 1/1；ask 两形 -cF = 1/1；allow 无 contracts 条目；integrator 铁律逐字在场；JSON parse OK |
| V5 队列簿记 | 两条 260907 行结移 §2（核销确认 + 承载批引用）+ B-8 重指排队行 + `changes-index --upsert` → frozen/no |
| V6 端到端演练 | 临时仓复刻双包抢 v3：rebase → `AA server/deploy/openclaw-system-…v3.md` 在 porcelain，与仓内自检 T7a 输入形状逐字一致 → 检测命中、并集指引在案、reflog 零 ours/theirs（详 1b 偏差 2 形态适配注记） |
| 结构门 | proposal-consolidation-check（FROZEN 后复跑）：frozen:true / missingTargets:[] / advisory 25/27（GA-8/GA-4 轴代号噪音） |

## 3. 插曲 / 摩擦（诚实记录）

1. enforce-scope 拦 bash heredoc 写 lib（universal.bashDenyWrite）——**治理机层按设计工作**的正面实例：本批治理内容即「重定向写 .claude/** 红线」，改走受门 Write/Edit 路径，无绕行。
2. deny-main-push.mjs 对评审制品归档 heredoc（内含 push 字样）误拦一次——改 Write 路径（findings-r2 归档），无绕行。
3. V6 夹具两轮调试（pkgB 基点与空目录跟踪语义），终态如实记录（1b 偏差 2）。
4. V4 大小写偏差（1b 偏差 1）——冻结断言笔误，落地以附录 A.1 逐字为准，交 verifier 复核。
5. landing summary 初稿两处本文件内部笔误（雫/巢/garble 类乱码词），本稿为修订重写稿。
6. 提交序：冻结态（`af0ac8e`）→ landing（`b89bf33`）先行，本 summary 落盘后随 verifier 制品同 commit。
提交序注记：评审制品链 `014adfe → fae886a → cc2bde7 → 1c75c30 → af0ac8e → b89bf33` 全部在 gov/kidpal 分支，合并后由下批 index 全量重建收敛（§10b 口径）。

## 4. 残余风险 / 留根指针

- 留根清单见 PROPOSAL §6（11 行）：escalation Track A 批（OQ4 已裁「本批 ack 后即启」，待 human 启动）、hook airtight 另批、B-1 claim 机械挂点观测后批、B-8 重指排队行等。
- 七消费方名册全绿 = lib 扩展零回退实证；但 gate 新核验行 live 首跑在下一 dev-loop Pod 的 merge 段——首跑表现进下批 retro 观察。
- settings ask 弹窗首跑体验（每次消解 1-2 次人交互）待实测，降级预案在留根（A-4）。
- 由 landing-verifier 独立判定 landing 保真度；本 summary 不自评通过。
