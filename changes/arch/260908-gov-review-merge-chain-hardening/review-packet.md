# Review Packet · 260908-gov-review-merge-chain-hardening

> adjudicator 收口 · 2026-09-08 · governance profile · Track A full-adversarial。findings 汇总 + 裁定 + 级联 + canary 复核 + governance pass-oracle 逐关结论。逐条裁定明细见同目录 `adjudications.md`，裁断正文与 human 拍板清单见 `decision-packet.md`，全过程时间线见 `debate-log.md`。

## 1. Findings 汇总与裁定

### 1.1 计数

| 来源 | 条数 | CONFIRMED | DISMISSED | DEADLOCK |
|---|---|---|---|---|
| R1 findings F1–F12 | 12 | 11（F9 拆分计 CONFIRMED 清单部分；F11 零影响） | 1（F9 严重性定性部分） | 0 |
| R1 B-0–B-8 断链 | 9 | 5（B-0/B-2/B-3/B-6/B-7）+ B-8 队列化部分 | 3（B-1/B-4/B-5，R2 撤回成立）+ B-8 定性部分 | 0 |
| R2 新 finding N1–N5 + N6 残留 | 6 | 6 | 0 | 0 |
| 合计 | 27 | 22 | 4（3 条整条 + 2 条拆分定性） | 0 |

被审对象最重三条（R1 BLOCKING，全部 CONFIRMED 且修向批内回灌）：

1. **F1**——T6 要落地的 lib 路径 `.claude/scripts/lib/governance-gate-shared.mjs` 已被 518 行承重单源占用（七消费方，非六），PROPOSAL 全程按「新增」申报，landing 即撞名。修向终裁 = 并入既有 lib（追加导出、既有导出零改动）。
2. **F2**——V4 负断言被 settings deny 条目自身字面证伪（:7/:18-19 三处命中亲核），验证集含一个永不可过的断言。修向 = -cF 固定串正向钉死 deny 面 + ask 白名单化。
3. **F3**——V5 明令结移的 `docs/ARCH-REVIEW-QUEUE.md` 不在 manifest 六路径，锚集对账 landing 时机判红。修向 = FROZEN 前 manifest 补行（与 A-5 无关可先行）。

### 1.2 级联派生（N 系）

R2 定向复攻产出 5 条新 finding + 1 条残留，全部 CONFIRMED：

- **N1/N2** 是 F1/F8 修向的级联抽验结果——修向文案自身被既有字面证伪（lib 五处 process.exit）、消费方名册漏计 portfolio-reconcile。两修向按 defender R2 终稿（import 探针主断言）落地。
- **N3** 是 settings ask 探针矩阵的实测新实例——runbook §10a 第 3 步逐字命令 `GIT_EDITOR=true git rebase --continue`（pod-coordination.md:126 亲核）前缀不命中 ask，非复合命令的单条逃逸，砍在消解主循环最高频位。该探针改写了 GA-8 ask 张力的成本收益两侧，直接输入 A-4 裁断。
- **N4** 齿名漂移 makemeahanzi（亲核 ：678 def / :22 / :172），本批管的正是指向精度，逐字订正。
- **N5** 本轮最锋利新分叉——T6 spawn→import 隐含解锁核验集从「全闸六条 + 豁免」收窄为「五要素」未申报。adjudicator 亲核 merge-driver :1368-1370 守卫 + processPod :1117 → Step 1b :1136-1150 无条件全闸背书后，裁 defender 选项 2（收窄 + Step 1b 背书补偿断言 + T6 申报句）——详见 adjudications.md §三 N5 行。

### 1.3 裁定项（A 系）结论速览

| 项 | 裁断 | 归档位置 |
|---|---|---|
| A-1 | 不触 lightweight 硬排除判据字面（frontmatter/write scope/authority 零变更），但属 agent def 执行语义变更、恒 human-approved——判据先例归档 | decision-packet §二.1 + human 拍板清单 |
| A-2 | T5 = 既有前置升级闸门内加项（折进注范式），非新门；T4 ask = settings 条目非新门 | decision-packet §二.2（agent 层归档） |
| A-3 | 事实层同形确认；设计层裁「维持两形、只改豁免路径回退分支（回退改显式 errorExit）」 | decision-packet §二.3 + human 拍板（建议即地板） |
| A-4 / OQ3 | ask 条件保留 + 先修 runbook 命令形态互洽 + 四条回填义务 | decision-packet §二.4 + human 拍板 |
| A-5 | 落点 = acceptance-evidence.md；被排除两落点不需指回引用 | decision-packet §二.5 + human 拍板 |
| A-6 | 本批不设新门；lint 夹具 + gate 化留根；guard-class 格式随裁钉死 | decision-packet §二.6 + human 拍板 |
| A-7 | 登记表为 authority + 正则为形态兜底；初始清单 = 实测超集 | decision-packet §二.7（agent 层归档 + 整包批确认） |
| A-8 | 放行（只拦可识别形态）+ B-4 写作侧对齐约定同场裁 | decision-packet §二.8 + human 拍板 |

## 2. Canary 复核结论

charter §6 十二条 regression-canaries：challenger R1 **12/12 独立重派成功**（非照抄 architect §review-bg 候选清单——重叠处属独立收敛），其中 10 条全绿不碰、2 条活碰撞：

- **#7（settings deny 面结构不变）× F2**：碰撞形态 = deny 面本体零删改与 V4 负断言「零命中」互斥——是验证集断言写法缺陷，不是 deny 面将被动。F2 修向（-cF 正向钉死）落地后 canary 7 反而获得更强机证。
- **#10（off-main 求值主体）× F5**：唯一被 PROPOSAL 候选口径自违的 canary——F5 修向（删候选/标注先验排除）落地后解除。

adjudicator 抽验亲核：#4 kept-blocked（merge-driver :927 起 catch 块 + setPod ready-to-merge 在案）、#5 gate 例外清单与向严默认（:150-158 / :249 `hasContractGateEvidence: false` 在案）、#7 deny 两行（:18-19 在案）、#10 求值形状（:919/:1136-1142 同形在案）。结论：canary 5 关 GREEN，两条活碰撞已随 F2/F5 修向闭环。

F11 属 charter 自身 erratum（enforce-scope.mjs 无 `--self-check` 入口亲核确认），canary 11 意图（hook 层零触碰）不受影响，交 moderator 注记。

## 3. dev-loop handoff

**not required**——本批全部 CONFIRMED 改法均为 PROPOSAL 文本回灌（措辞修正 / manifest 补行 / 验证集断言改写 / 留根增行 / 条款申报句），由 governance profile 定稿阶段 architect 机械回灌 frozen PROPOSAL 承载，无需要 dev-loop planner intake 的实现侧修复 finding；触 human / 契约裁决面者已入 decision-packet 的 human 拍板清单与 blockingDecisions 对应项（见 decision-packet §四），未伪装成可执行 TaskPackage。

## 4. Governance pass-oracle 六关逐关结论

| 关 | 结论 | 证据 |
|---|---|---|
| 1 治理判据覆盖 | **GREEN** | charter 十轴 GA-1..GA-10 逐条被 challenger 走过并有 finding 或显式确认落点：GA-1（F4 packet-binding 计数 + A-5 G1 触面论证）、GA-2（B-2/B-5/B-8）、GA-3（A-1/F7/S-2 链）、GA-4（A-2/F8/N5）、GA-5（A-3/F5/N3/B-0）、GA-6（F9/A-7/B-1）、GA-7（F9 补集不变式夹具/A-8）、GA-8（A-4/N3/B-1 张力）、GA-9（F2 独立跑核 + sweep 对账）、GA-10（F6/F8/N1/V6）。六场景 S-1..S-6 全部走进 findings（S-1→F9、S-2→F7/B-3、S-3→F8/N1/N5、S-4→F12/S-4 类别判据、S-5→N3 探针、S-6→T2-2④ 复跑纪律强制力记账于 B 系与 A-8 补偿控制） |
| 2 finding 全结 | **GREEN** | 27 条 finding/A 项全部落到 CONFIRMED（附批内回灌修向 / 留根 / decision-packet 裁断）或 DISMISSED（附引原文依据）终态；无未结 BLOCKING/SHOULD——三条 BLOCKING 修向均为批内回灌、FROZEN 前由 architect 定稿落地 |
| 3 高敏项已标 human | **GREEN** | agent def（A-1/integrator.md 新节）、强制执行层（A-4/settings ask、A-6/闸判据、A-8/闸方向、N5/解锁核验集）、闸判据与判据先例（A-3/A-5/B-1 挂点）全部列入 decision-packet §四 human 拍板清单，不在 loop 内落 canonical |
| 4 触契约已转门 | **GREEN（未触发）** | sweep 表契约面零触 + T5 只读消费 CDQ 不开写权（§0 红线）+ manifest 六路径与 A-5 落点均不含 `docs/contracts/**`；adjudicator 亲核 settings deny 两行在位（:18-19）= 契约冻结机械层在位；V4 按 F2 修向改写后该事实可机证。无契约门转派需要 |
| 5 canaries 全中 | **GREEN** | 12/12 独立重派（§2），2 条活碰撞已随修向闭环，charter erratum 一条（F11）交 moderator |
| 6 decision-packet 完整 | **GREEN** | `decision-packet.md` 逐项载：8 个裁定项裁断与归档位置、CONFIRMED 改法清单（PROPOSAL 回填点逐条列 / 留根 / 后续批）、riskTier 复核、human 拍板清单（9 项，含整包批准）；A-8 项内已同场载 B-4 写作侧对齐约定（R2 A-2 设定的 packet 完整性核验义务履约） |

**总结论：governance pass-oracle 六关 GREEN——本批可进入 human 拍板（decision-packet）→ architect 定稿回灌 → human 盖 FROZEN → 主线程 packet-bound landing 的定稿序列。** 残余风险与冻结建议见 §5。

## 5. 残余风险与冻结建议

- **残余风险 1（A-4 待裁）**：ask 覆盖盲区在裁断落地前真实存在（复合命令 / env 前缀 / child_process 三类绕道在册）；裁断落地后残余 = 手动 Bash 路径 best-effort，由 T3 正文纪律 + hook 留根承载，非无声豁口。
- **残余风险 2（A-8 放行方向）**：未识别形态放行 = 漏账不被机械拦，与批前零核验同水平不恶化；补偿控制（写作侧对齐 + 两态夹具 + defer 锚格式）落地前该闸的拦截面为空——T5 落地时三者必须同批到位。
- **残余风险 3（B-1 观测期）**：versioned-artifact-claim 漏记 / 竞态在机械挂点建成前靠会话自查，兜底链 fail-closed 但多付一次消解成本；按 §9 毕业条件观测，漏记率达量化条件即评估机械化。
- **冻结建议**：**可冻结（条件 = CONFIRMED 改法清单全部回灌 PROPOSAL 冻结稿 + human 拍板清单 9 项裁断后）**。本批不存在 RED 退回项；debate-log.md 全过程时间线在案（moderator→scenario-author→challenger→defender→adjudicator 完整），无缺失段。

## 6. 制品清单

- `adjudications.md`——逐条裁定记录（27 条 + A 系 8 项结论）。
- `review-packet.md`——本文件。
- `decision-packet.md`——裁定项裁断正文 + 改法清单 + human 拍板清单。
- `debate-log.md`——末尾 append adjudicator 裁定段（本会话）。
- `docs/ARCH-REVIEW-QUEUE.md`——四条承载 ARQ 行 + 两条待销账行注记（append 语义）。
