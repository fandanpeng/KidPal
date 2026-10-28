---
# —— 必填 ——
workstream: 治理债清偿（gov debt reduction）
slug: gov-debt-reduction
status: active
opened: 2026-10-26
closed:
summary: docs/debt/gov/ 未结条目的分簇清偿计划——按收益/成本比排 P1–P4 四波，P1 / P1.5 / P2 / P3（搭载） / P4 五波均已合 main；2026-10-27 human 指示残留 20 条重排 P5–P9 继续清偿，仅 4 条保持条件锁
# —— 选填 ——
owner: main-thread
area: governance
changes:
  - 261026-gov-review-merge-gate-cluster-fix
  - 261026-gov-review-final-phase-dispatch-contract
  - 261026-gov-review-p2-anchor-template-cluster
---

# task_plan · 治理债清偿（gov-debt-reduction）

## 目标

把 `docs/debt/gov/` 的未结条目从「散点触发挂靠」收敛成**分簇清偿波次**：同触发面/同脚本/同文面的条目并批处理，降低每个治理批的立项与评审边际成本；按「复购率 × 阻断度 ÷ 修法成本」排波次。

## phase（分波交付计划）

### P1 · 合并闸/强制执行层簇（✅ 2026-10-26 已落）

- 六条一次收口：ARQ-261017-merge-gate-contract-evidence-ref + ARQ-261021（同根合并消费）+ ARQ-261017-scope-unstated + ARQ-261014-tooling-hotfix + ARQ-261011-selfquote + ARQ-261018（MAJOR 搭载，human 裁）。
- changeId = `261026-gov-review-merge-gate-cluster-fix`；全环闭环（R3 GREEN → PASS+7A → 已合 main → PR #40 已合并）。
- 详见 `progress.md`。

### P1.5 · final 相位派发契约批（✅ 2026-10-26 已落，MAJOR）

- 主条目：`ARQ-261026-final-phase-dispatch-contract-unwritten`（八包实证每包烧半轮到一轮；修法 = G2 文面几句）。
- 并批（同面）：`ARQ-260923-codemem-mode-doc-drift`（同动 launch-prep 文面）+ `ARQ-261025-interrupted-dispatch-progress-note`（同动 planner 派发步/brief 模板）。
- changeId = `261026-gov-review-final-phase-dispatch-contract`；全环闭环（R2 GREEN → PASS → 已合 main `47888d5b`）。详见 `progress.md`。

### P2 · 锚解析门 + summary 模板簇（✅ 2026-10-26 已落）

- `ARQ-261019-anchor-probe-fence-blind`（governance-gate-shared §2 边界探测不跳围栏）+ `ARQ-261019-verbatim-quote-line-level-false-negative`（NIT）。
- `ARQ-260928-landing-summary-skeleton-template` 与 `ARQ-261021-landing-summary-template` **先并条再消费**（同一件事：summary 缺模板承载；低成本案 = arch-change.md 加两句字面义务）。
- §5 验证集纪律三条纯文面：`ARQ-261019-forward-derived-verification-set` + `ARQ-261025-verify-set-negative-positive-pairing` + `ARQ-261021-proposal-assert-expectations`。
- **搭载**：`ARQ-261026-release-pr-gov-lane-zero-pods`（P3 项，本批动 `.claude/scripts/**` 顺手落）。
- **拆出不动**：`ARQ-260924-landing-gate-dir-anchor-gap` 残余②（目录锚展开，human 2026-10-26 裁独立批——触承重闸判定逻辑，单独评审）。
- **立项 ack（2026-10-26）**：① summary 义务 = arch-change.md 加两句字面（不立模板，视复发再立）；② 实跑回填纪律扩到 PROPOSAL 全部实测数字；③ release-pr gov-lane 取最小版；④ 六条并批 + 搭载，Track B 主路径。changeId = `261026-gov-review-p2-anchor-template-cluster`。

### P3 · release-pr gov-lane 门面（✅ 2026-10-26 已随 P2 搭载落地结项）

- `ARQ-261026-release-pr-gov-lane-zero-pods`：已随 P2 批落地结项，无独立包。

### P4 · Pod / board 面簇（✅ 2026-10-27 已落）

- `ARQ-260918-pod-session-settings-identity-unobservable` + `ARQ-260914-portfolio-reconcile-launchpreconditions` + `ARQ-261025-shared-branch-pod-isolation` + `ARQ-261025-wait-state-closed-set` + `ARQ-261025-intake-index-retro-scope-hygiene`。文面/脚本混杂（pod-coordination / portfolio-reconcile / launch 面分属不同评审强度）。

### P4 复证记录（2026-10-26 立项期，主线程实测）

- 260918：候选① 前半已落（260921 T3，canonical 命令行两形态在启动卡模板 :131-133）；残余 = ① 后半「VS Code 扩展会话降级面标注」（实测前置 = 扩展会话能否传 `--settings`/`--agent`，两批均未实测）+ 候选② 在场性自检落 pod-events（planner.md 无此步，未落）+ 候选③ 门策略。候选③ 的最低成本形态（planner.md :111-112 supervisor-gate-check 显式步骤）已在场。**本条自带设计分叉**（身份要不要机械自证 / 非 CLI 形态如何表达 canonical），原条目 profile 标 full-adversarial 候选。
- 260914：读侧已落（260921 T6，portfolio-reconcile.mjs 17 处消费）；残余 = 存量 12 条数据迁移——实测 `m1-home-qa` / `m2-char-window` 两 workstream 的 `launchPreconditions` 均仍为裸 string，两 workstream 仍在册。
- shared-branch-pod-isolation：落点在场（merge-driver.mjs / pod-coordination.md），未消化。
- wait-state-closed-set：实测 `POD_STATUSES`（portfolio-reconcile.mjs :36）八值闭集、无 awaiting-human 档，未消化。
- intake-index-retro-scope-hygiene：实测 `computeRetroScope`（loop-health-report.mjs :177）无 `current` 排除；intake INDEX 漂移标现存 4 处（且 261012 已挂 ⚠滞留10d——漂移在增长，「下轮 retro 仍见漂移」触发条件已成立）。
- 判档预估：五条中三条落 scripts（portfolio-reconcile / changes-index / loop-health-report）→ 命中轻路径判据③，**整批恒 Track B**（只升不降）。
- **立项 ack（2026-10-26 human 四拍）**：① 五条一批 Track B 主路径（拆批不降评审强度、只加开销）；② 260918 只取低分叉子集——候选② 在场性自检落 pod-events + ① 后半降级面标注，不新增 gate 不裁机械牙，门策略留观察；③ shared-branch 取方案① 文面（pod-coordination §2c 互斥租用禁令 + 每包 base ref 钉死 + launch 检查项），**worktree 池化（Pod pool：空闲工位注册/租用/释放）由 human 另登记为独立 idea 走后续专项批**，方案② merge-driver 扩参留给池化批按其形态再定；④ 260914 存量 12 条迁移搭本批。
- changeId = `261026-gov-review-p4-pod-board-cluster`，architect 起草中。
- 2026-10-27 起草完成（DRAFT v0.1，546 行，preflight 11 锚 ok；draft 已 upsert 进 delivery/review/INDEX.md）。OQ human 前置拍板（2026-10-27）：**OQ1 = (a) 保守标注 + 观察期**（与草案一致）；**OQ2 = (b) 同批打通**——merge-driver approvalRefUnlock（:1101 附近，以 blocked 为键）解锁判据同批扩 awaiting-human:merge-gate，manifest +1，riskTier 重估；wait-state 条目可望整条结项（非部分）。R1 四查进行中，R1 findings + OQ2 修订合并一轮改稿。

### 不动档（触发挂靠，不排波）

- ~~12 条「下次动 X 时并批」型 + 已接受风险 2 条 + 2 条明确单开批~~ → **2026-10-27 human 指示「不想再攒」，20 条残留重排为 P5–P9**（见下），仅 4 条保持条件锁。

### P5 · 新机制前置 + 锚母版校验簇（✅ 2026-10-27 已落，2 条 + 搭载 1 条，含全场唯一 MAJOR）

- **立项 ack（2026-10-27 human 已拍：P5 先开，两条并批 Track B 主路径；P5 由 human 在新 session 启动，本 session 只做立项材料与复证）**。
- **复证记录（2026-10-27 主线程实测）**：
  - 261024（spike-first，MAJOR）：三条 paths（`change-triage.md` / `dev-loop.md` / `PROPOSAL.template.md`）全部在场；`change-triage.md` 实测无 spike/消费端相关字面 = 处方未被后续批消化，落点干净。注意条目自带形态分叉预告：若 review 裁定以机械 gate（G3）落地则升 full-adversarial + human——起草时如实标，不预裁。
  - 261017（anchor-check 母版校验，SHOULD）：`spec-anchor-check.mjs:35-41` `currentDesignFile()` 逐字在场、与条目引用完全一致 = 未被消化；触发条件「再犯即升格」未发生，本次按主动修排批（human 裁）。条目注意点：与 `docs/debt/tech/ARQ-260918-spec-face-3page-reconcile.md` 同工具不同根，起草期互查咬合。
- 建议条目承载的修法方向（详见各条目原文）：261024 = spike-first 原则钉进 triage 判档 / build-feature plan / Gate 链消费端断言（落点候选 a/b/c 三处不互斥，human 原话「先做一轮小验证，然后再做开发，再做数据回灌」）；261017 = `currentDesignFile()` 选出候选后加完整性自查（与前一版 `data-od-id` 数比对）+ 命名族纪律文面（`vN.html` = 完整母版）+ 顺序侧兜底。
- **立项追加（2026-10-27 human 拍并入 P5）**：UI 设计版本结构整体调整并入本批一起改——现行「最高版号即现行母版」隐式判据换**显式 current 指针 + 逐版备注声明**（镜像 `OPENCLAW_SYSTEM_PROMPT_CARRIER` 声明面先例），锚机检 / 出图 / 截图引用以声明为准；截图布局（逐版文件夹增量、TaskPackage `designRefs` 引用形如 `docs/ui/V5/canvas-p1.png`）一并入范围。T2 扩面重写；R1 四查照跑、findings 作改稿输入，改稿后按轮次纪律复审。

### P6 · landing 门机器面簇（✅ 2026-10-27 已落，2+1 条 + 顺手登记 1 条新 ARQ）

- `ARQ-260914-landing-preflight-anchor-parse` 残余：**部分结**（byte 比对脚本化 `--cite-check` + 双信号源规约落地；条目维持 deferred，③ 连带项非机械化半留根）。
- `ARQ-261026-verbatim-extract-backtick-cut`：**结项**。
- `ARQ-260924-landing-gate-dir-anchor-gap`：**结项**（含 2026-09-25 部分承载的处方①③，残余② 本批补齐）。
- **复证记录（2026-10-27 主线程实测）**：
  - 260914 残余：`--cite-check` 不存在（landing-fidelity-check.mjs 无 cite 字面、scripts 下无独立 cite 脚本）= byte 比对脚本化未消化；双信号源规约（追注 §4-4 正读数镜像）在模板/自检句无承载（:69 ③ 只有负结果交叉半边）= 未消化。同条目另三子项已消化：围栏承载条款在 PROPOSAL.template.md:65、评估时点逐子句在 :69①、目录 token advisory 在 landing-fidelity-check.mjs:245-250——P6 范围与残余行精确吻合。
  - 261026 backtick-cut：提取正则 `/```[a-zA-Z]*\n([\s\S]*?)```/g` 原样在 landing-fidelity-check.mjs:472，无独立成行判定 = 未消化；触发条件「下次动该脚本 verbatim 面」本次满足。
  - 260924 残余②：:656/:658 仍 `changed.has` 精确成员判定、目录锚不展开 = 未消化；处方①③（advisory :245-250 + 模板纪律句）已在场。
  - 判档：三条 diff 全触 `.claude/scripts/**` → 恒 Track B；dir-anchor ② 触承重闸判定逻辑、批内独立 T 全强度评审（human 已裁）。
- **立项 ack（2026-10-27 human 两拍）**：① byte 比对脚本化载体 = 挂 `landing-fidelity-check.mjs --cite-check` 子命令（否独立脚本）；② 目录锚口径 = **仍劝阻、展开作兜底**——advisory 与模板纪律句保留改写为「可用但劝阻、展开是兜底不是许可」，迁移批写作习惯不变。批形态 = 三条一批、Track B 主路径、dir-anchor ② 批内独立 T 全强度评审。changeId = `261027-gov-review-p6-landing-gate-cluster`。
- 2026-10-27 起草完成（DRAFT v0.1，966 行，preflight 五锚 ok；draft 已 upsert 进 delivery/review/INDEX.md）。R1 四查 BLOCK(F1–F4)：F1 BLOCKING（V6 断言字面与改后 frontmatter 形态不符）、F2/F3 SHOULD（装配副本行数陈旧 1121→1129 / census 三话术短语缺豁免面）、F4 NIT（留根 3 与 T1⑦ 口径差）；改法方向零打回、无升级 human、判档 Track B 主路径成立。已转 architect 一轮改稿，改稿后 R2 聚焦修订到位性。
- 2026-10-27 v0.2（1008 行）修订全到位 → **R2 GREEN**（仅余 N1 位置括注 NIT、不拦绿，已转 architect 一行收口）；判档 Track B 主路径、无升级 human。凭「立项 ack + GREEN」主线程标 FROZEN，进 packet-bound 落地。
- 2026-10-27 **全环闭环**：FROZEN v0.2 → 装配落地五件（landing-fidelity-check 1129 行 sha256 对账一致）→ 验证集 V1–V7/V9 全绿（判别夹具 18/18）→ landing-verifier **PASS + 2 ADVISORY**（无升级项）→ 落地 commit `4aaabce2` → V8 升级闸 **PASS**（kind=gov）→ INDEX 翻 landed → retro 落盘。**P6 收口；下一波 = P7（dev-loop 文面纪律簇，6 条，轻路径候选）**。

### P6.5 · 起草成本减负批（ARQ-261027 消费批，2 便宜项 + 拆分纪律）

- **来源**：ARQ-261027 专项讨论（2026-10-27，human 与主线程），方向裁定见条目「方向裁定追注」——否决 dev-loop 路由与写权通道（执法层自改风险），收敛两便宜项 + 一新规则。
- `ARQ-261027-draft-sandbox-assembly-scaffolding`（SHOULD）：T1 共享锚文装配器（旧串锚定替代行号基线，architect 每批只写编辑清单）+ T2 复跑分档句（PROPOSAL.template §5，治 retro F2 复跑税）+ T3 脚本批拆分纪律句（`.claude/scripts/**` 净增 >~150 行或 feature 级单独成批、fix 级 <~50 行可搭车；落 gov-change.md 起草节 + PROPOSAL.template，初值随复发校准）。
- **立项 ack（2026-10-27 human）**：「立项吧」。changeId = `261027-gov-review-draft-cost-relief`，Track B（触 `.claude/scripts/**`）。T1 落地后本批起草即自消费试用。
- 2026-10-27 **全环闭环**：DRAFT v0.1 → R1 BLOCK(F1,F2)+F3（判官构造 junction 穿透仓界、净增公式漏新文件）→ v0.2（realpathDeepest 治本 + git add -N 口径）→ R2 GREEN → FROZEN → 装配器自消费落地四件（sha256 逐值对账）→ landing-verifier **PASS + 3 ADVISORY**（ADVISORY-1 抓出主线程 summary 失实句、更正留痕 + junction 清理）→ 落地 commit → V8 闸 PASS → INDEX landed → retro 落盘。**ARQ-261027 整条结项归位；同量级实现全批成本较 P6 降 ~40%；拆分纪律 150 行初值起用**。P6.5 收口。

### P7 · dev-loop 文面纪律簇（轻路径四条）

- **立项 ack（2026-10-27 human 两拍）**：① 六条全部立项、修法照各条目建议改法；② 批形态 = **乙**——轻路径收 ①②③⑥ 四条（判档以起草实落点为准、只升不降），恒 Track B 的 ④⑤ 组 **P7.5 小批**随后开（不并 P8——P8 是门语义簇，主题不同）。
- 本批四条：`ARQ-261025-adjacent-frozen-anchor-sweep`（intake/1g 扫相邻字面锚成文）· `ARQ-261014-rename-batch-second-token-census`（改名双 token 族 census 义务成文）· `ARQ-261025-debt-archive-ref-atomic`（归档与引用同批义务成文）· `ARQ-260908-known-red-count-no-mechanical-source` ③（runner 报告头 acceptance-evidence 文面）。
- **复证记录（2026-10-27 主线程实测）**：四条处方全部未消化——① 三落点均无字面锚扫查义务文面，最近似邻接物 planner.md 1f「拆包快照保鲜核验」只核引用存在性不做符号级扫查（互补不重叠，起草期须写咬合防双头）；② path-form-family.md 零「token 族/stem」承载、PROPOSAL.template :77 sweep 透镜只管新门/断言 pattern；③ debt-render 归位在场但无 stub（条目倾向①+③ 纯文面不冲突）、task-intake 无结项 debt 归位预留自查句；⑥ ② 已消化（acceptance-evidence.md :45-51 root 注入节）、③ 报告头执行目录现值口径零承载。
- **落点约束（立项 ack 附带）**：① 落点收 task-intake-and-slicing.md + dev-loop.md（1g 档定义在 dev-loop.md），避免触 planner.md；③ 落点收 task-intake-and-slicing.md 起包自查、integrator.md 侧句或可省；⑥ 只落 acceptance-evidence.md 文面不触 runner 代码。任一条起草中发现必须触轻路径排除集（commands/agents/hooks/scripts/scope-map/settings/verify-gates/AGENTS/CLAUDE/DEV-GOVERNANCE）→ 该条挪出、只升不降。
- changeId = `261027-gov-review-p7-devloop-doc-cluster`，architect 起草中。
- 2026-10-27 起草完成（DRAFT v0.1，341 行；装配副本 / 沙盘 / 验证脚本全在仓外 C:/tmp/p7-work，仓内零杂散；draft 已 upsert 进 delivery/review/INDEX.md）。判档自评 = 轻路径，但 §1 向严声明「四条为文档义务非错字排版、判据②可取暖读法」。R1 四查 **GREEN**（无 BLOCKING/SHOULD、三条 NIT），**判档升 Track B 主路径**（判据②不满足——新增规范性义务文面非措辞级勘误；只升不降，改法集不变不重做，立项 ack 已预授权）。凭「立项 ack + GREEN」标 **FROZEN v0.1**，按 Track B 七步落地。
- 2026-10-27 **全环闭环**：landing-verifier **PASS + 1 ADVISORY**（A1 = summary 在途件快照时序差，已条目化 ARQ-261027-landing-summary-inflight-snapshot）→ 落地 commit `78a1105d` → V7 升级闸 **PASS (kind=gov)** → ff 合本地 main → 复证后过闸 → **PR #46 已开**（release/261027-4；发布首跑报 diverged——PR #45 壳 `eca8ac14` 在远端、人裁后本地 no-ff 追平 + INDEX 全量重建归位，P6.5 同款处置链二例）。PR #46 合并后待：main worktree `--sync` + gov 回同步 + `--upsert` 翻已合main。**P7 收口；下一波 = P7.5（收口写点映射 + opendesign 出图先例，恒 Track B 两条）**。

### P7.5 · 收口写点映射 + 出图先例簇（恒 Track B 两条，随 P7 轻路径批后开）

- `ARQ-261014-closeout-obligation-write-authority-vacuum`（收口义务→承载写点映射成文；DEV-GOVERNANCE.md 属治理文档非轻允许集 + 修法可能触 scope-map 写权矩阵 → 恒 Track B）+ `ARQ-261019-opendesign-shoot-script-precedent`（opendesign-connect skill + uiux-author 定义增补；uiux-author.md 是 agents 面 → 恒 Track B）。
- 复证：两条件目落点均未消化（DEV-GOVERNANCE 现有表格是入口/批准路由表非写点映射；uiux-author.md :34 仍是「OD 失联→出图 pending 挂 human-decision」反向误导；CDQ 行子项已 261019 批闭合、余面未治）。⑤ 起草期须与 P5 落地后的 docs/ui 显式 current 指针纪律咬合，不写第二套口径。
- changeId = `261027-gov-review-p75-closeout-shoot-cluster`，2026-10-27 architect 起草中（Track B 主路径全件；④ 修法取①+②、scope-map 类候选③列 OQ；⑤ 修法取条目三改进项、须对齐 P5 落地的 docs/ui 现行口径）。
- 2026-10-27 起草完成（DRAFT v0.1，536 行，T1–T6 共 17 处编辑、manifest 8 件；G3 面 / 契约 / spec / docs/ui 零 diff）。**OQ-1 human 拍 = (a) 维持文面映射、不开机械通道**（scope-map 类候选③不开，先积累「仍须 human 兜」数据）。**插话记账**：工作树发现两条「已接受风险」ARQ 裸删（contract-mech-gate-gap / crossend-consumer-tiers），human 确认有意删 + 裁「已过时/被消化」→ 主线程补正规记账（还原 + 翻 closed + 追注 + 归档 + contracts.md 指针刷新），commit `8c262d4c`。R1 四查进行中。
- 2026-10-27 R1 = **BLOCK(F1,F2,F3)**（三条 SHOULD 文本级：F1 T6 追注对 ARQ-261019 改进项③表述错位 / F2 单写者约定指示 Edit 但 uiux-author 工具面无 Edit / F3 B10 摘要与实跑不符 17→16+1 歧义）+ 2 NIT；判档 Track B 成立、T1–T6 结构零打回；判官亲核装配复现 / scope-map 零换授 / 消费者普查穷尽（story-author「需出图」通道中立豁免正确）。已转 architect 出 v0.2（566 行，五处修订 + 复跑全绿、结构落点 manifest 零变），R2 聚焦复验中。
- 2026-10-27 **R2 GREEN**（五处修订全到位、机检复跑逐值相符、判档维持 Track B）→ **FROZEN v0.2** → packet-bound 落地：V1① 复跑装配 8 件 sha 与 B2 逐值同 → 仓内 apply 8/8 一致 → debt-render 归位 ARQ-261019 → cite-check phase=landing 17 checked / 0 missing（ARQ-261019 两块按归档位命中，P5 先例兜底生效）→ G3/契约/spec/docs/ui 零 diff → `_scratch/` 已删。landing-summary 全件落盘，landing-verifier 核验中。
- 2026-10-27 **全环闭环**：landing-verifier **PASS + 1 ADVISORY**（A1 = upsert 未跑，落地 commit 前闭合）→ retro 落盘（F1 工具面盲区条目化 ARQ-261027-draft-toolface-blind；near-miss = 账本裸删差点混进批 commit，全量 status 逐件定性接住）→ 落地 commit `2d2c997b` → V8 升级闸 **PASS (kind=gov)** → main 已前进（b68fdf68 不在 gov 历史）→ gov 回同步 merge + 生成件重建 → 闸复跑 PASS → ff 合本地 main `6ea7769b` → **PR #47 已开**（release/261027-5，发布预检零拦——PR #45 壳已在本轮回同步中追平）。**ARQ-261019 整条结项归档；ARQ-261014 部分闭合维持 deferred**。P7.5 收口；下一波 = P8（门语义簇 2 条，恒 Track B）。

### P8 · 门语义簇（2 条，恒 Track B）

- `ARQ-261014-supervisor-escalate-preflight-ignored`（supervisor/pre-merge 闸消费 launch-card 前置申报）+ `ARQ-261001-host-blind-outcome-device-gate`（human 2026-10-27 改裁**并 P8 同批**，批内独立 T 全强度评审）。
- **复证记录（2026-10-28 主线程实测，两条均未消化）**：① 261014——闸脚本实际落点已漂移至 `.claude/hooks/supervisor-verdict-gate.mjs`（条目 frontmatter 仍写 scripts 路径，起草时对齐并随批修正）；实测闸只消费 declared riskTier（只升不降）+ security 轴，`isAllMock` 启发式（:270-277）仍一刀切，launch-card ⑦ 申报消费 / 同款先例库 / 先例快批提示三样零承载；pre-merge 闸「先例/噪音」仅两处无关注释，噪音路径白名单零承载。② 261001——`verify-gates.json`（104 行）与 `dev-loop-operating-system.md` 对设备轮硬门 / 目击证据 / 用户可见结果判据 / host 不可编面词汇全部零命中；2026-10-25 追加的 chrome 三件自查句同样零承载。
- **立项 ack（2026-10-28 human 两拍）**：① 261014 取 ①+②（闸消费 launch-card ⑦ 申报 + 在册同款先例命中降级 advisory 留痕；human 批准语义结构化 approvedVia + 同型 pattern 标签累积先例库）+ ③ 兜底（ESCALATE 输出带先例快批提示）；② 261001 取 ①硬门 + 配套全套（真机目击升阻断合并硬门 + defer-merge-pending-live-round 收口链承认成文 + chrome 三件自查句 + 诊断日志保留 / builder 行为面观察申报节两条伴生文面），候选②抽缝留 dev-loop 不入本批；**human 追加：现在有设备板载日志（KPLOG），日志可以作为目击佐证——硬门证据形态须含 KPLOG 日志段引用，不限于人工在场目检**。批形态 = 2 条一批、Track B 主路径、261001 批内独立 T 全强度评审。
- changeId = `261028-gov-review-p8-gate-semantics-cluster`，architect 起草中。
- 2026-10-28 起草完成（DRAFT：T1–T5、42 处旧串锚定编辑、manifest 15 件；自证全绿——42/42 锚定命中、闸自检 96/96 + 81/81 + 10/10、反转判别 5/5、preflight/cite-check 零 findings；draft 已 upsert 进 delivery/review/INDEX.md）。起草期插话：human 问「这次还是产生一次性脚本吗」——主线程核 `_scratch/make-edits.mjs` 定性 = 编辑清单序列化器（装配/核对已归共享工具），仓内不留痕；若下批复发再立起草成本改进项。
- **OQ 三拍（2026-10-28 human，全取草案推荐项）**：① 先例库载体 = 种子文件（只引用真实人批事件）+ 未来先例只进 pod-events，不双写；② 设备轮硬门触发 = 三路咬合（启动卡申报 + acceptance 标记 + 实际 diff 触登记 host 不可编路径，fail-closed）；③ 脚本净增 414 行超 P6.5 拆分纪律初值 = **一批 + 明示例外**（批内独立 T 全强度评审保评审强度，例外记 PROPOSAL 与 retro）。裁定已回注 PROPOSAL。
- 2026-10-28 **R1 = BLOCK(F1) + 2 NIT**（判档维持 Track B 主路径 / riskTier high / G2+G3，不触发 Track A，熔断 1/3）：F1 BLOCKING——launch-card 模板默认占位符 `"userVisible":false, "reason":"<无用户可见结果面时填理由…>"` 长 22+ 字 ≥ 12 字理由地板，照抄模板未填而 diff 恰触登记 host 不可编路径的包会**静默豁免真机证据**，与本批 fail-closed 目标正面冲突；F2 NIT（probe 失败分支 error.message 明细未并 gate reasons）、F3 NIT（KPLOG 段号正则宽于 skill 的 000..031）。判官亲核装配 42/42、自检 96+81+10、mutation 5/5、种子反查 3/3 全复现。已转 architect 一轮改稿，R2 聚焦 F1 修订到位性。
- 2026-10-28 **v0.2 改稿完成**（F1 取①+②双保险：模板默认 reason=`none` + 闸把 none/尖括号占位符/<12 字均判未申报，T1b 文面同步；F2 probe 明细上抛；F3 KPLOG 收紧 000..031；OQ-3 同步为 426 行并注明 human 拍的是 R1 前 414 行、新增行为同方向加固）。自证全绿：装配 42/42、自检 96+85+10、mutation 6/6、preflight/cite-check 零 findings。
- 2026-10-28 **R2 = BLOCK(F1')**（熔断观察线 2/3）：F1 文面两臂确实落了但只闭合到 assess 层——T3b `!required` 分支写死 `required:false` 返回，T3c 消费判据 `required && !ok`，判官对装配副本 `evaluate` 五案直调实测 A/B（default-none 形 / 占位符形）`blocked=false` 放行——R1-F1 的洞在闸层原样存活；自检电池 85/85+6/6 对该洞全绿（两负例与 mutation 判别 case 均打在 assess 层，夹具判别缺位再现）。F4 NIT（probe 失败 reason 双前缀重复）；F2/F3 修订到位确认。判档维持 Track B、不升 human。R3 焦点唯一：T3b return 改 `required: reasons.length > 0`（或 T3c 改 `ok===false` 一律拦）+ 两负例升 evaluate 层断言 blocked + mutation 判别 case 升闸层 + F4 去重，勿扩面；R3 不绿即触发熔断三选一交 human。
- 2026-10-28 **v0.3 改稿完成**（选案①：T3b return 改 `required: reasons.length > 0`；两负例升 evaluate 层断言、mutation 判别 case 升闸层；F4 前缀去重；行数 436 = 193+6+237，manifest 15 件不变）。自证全绿：装配 42/42、自检 96+88+10、mutation 6/6、evaluate 五案独立直调 5/5（A/B/C/E 阻断、D 放行、probe 前缀恰一次）、preflight/cite-check 零 findings。R3（原判官）复验中——绿则过评审，不绿触发熔断三选一。
- 2026-10-28 **R3 GREEN，评审循环收敛（R1 BLOCK(F1) → R2 BLOCK(F1′) → R3 GREEN），未触发熔断**。判官三重实证闭合 F1′（结构性不变量 `probed ∧ ok=false ⟹ required=true` 三条 return 路径全验 + evaluate 五案直调 + 判别力升层核实 mutation exit 1）；F5 NIT（OQ-3 注记轮次措辞易误读）已冻结前顺手修一行。判档维持 Track B 主路径 / riskTier high / G2+G3。**主线程凭「立项 ack + R3 GREEN」标 FROZEN v0.3**（状态行照抄形态、preflight 复跑 15 锚零 findings、INDEX 翻 frozen）。**human 指示：landing 暂停，待通知**——packet-bound 落地 / landing-verifier / 升级闸 / 合并全部未动。

### P9 · 脚本卫生簇（2 条）

- `ARQ-261025-brief-inputs-anchor-noise`（brief-inputs.mjs 锚解析降噪）+ `ARQ-261014-pod-events-booking-telemetry-fidelity`（pod-events schema/aggregate 一致性）。两条不同脚本，可一批两 T 或拆两小批。

### 保持条件锁（4 条，不排波）

- `ARQ-260908-versioned-guard-repoint-execution`：human 2026-10-27 裁**现在授权清掉**。4 件待重指 = ① schedule_p2 P2-A6 wiring 齿 ② VRT live 探针（A4/A7）③ CHR-A8/A9 live 探针 ④ schedule_p2 PROMPT_CARRIER_V2 常量——重指清单原文在 `delivery/archive/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` 附录 A.3（append-only 归档，只读）；**逐件授权材料待批内 present 给 human**（A.3 为权威清单，本行只是指针），随 P5 搭载或独立轻批落。
- `ARQ-260913-auto-mode-classifier-legit-workflow`：双臂批须单开（口径臂 + 机械臂不得拆）。
- `ARQ-260913-current-pointer-implicit-global`：量化触发器未到（30 match settled / 1 absent 实例即启动）。
- 已接受风险 2 条（contract-mech-gate-gap / crossend-consumer-tiers）：复议条件未到。

### 不动档（触发挂靠，不排波）

- 保持条件锁的 4 条见上（versioned-guard-repoint / auto-mode-classifier / current-pointer / 已接受风险 2 条）。
- 2026-10-27 human 指示驱动的重排历史：原不动档 20 条中 16 条已重排入 P5–P9，仅 4 条留条件锁。

## 决策记录

- 2026-10-27 human 指示：P6 起草期观察「architect 无 canonical 写权但 PROPOSAL 须逐字节承诺 + §5 实跑，每批现写沙盘装配脚手架」立 debt ARQ → `docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md`（SHOULD，profile = full-adversarial + human，四修法方向候选在条目、方向未收敛待专项讨论；不排入 P5–P9 波次）。
- 2026-10-27 human 拍板：P5 评审熔断（R1/R2/R3 均 BLOCK，findings 逐轮收敛 3→2→1 行、全文面级）三选一取**换新判官实例再战 1 轮**（R4 只核 F4 单行；仍不绿自动转 Track A 段）。
- 2026-10-27 human 拍板：P5 批 OQ2 截图图夹布局取**丙（单一活目录 + 冻结快照）**（否 architect 推荐甲）；本批只落规则句与声明面备注，frozen 快照机制与存量 V5/V6/V7 目录整形另立需求面动作。
- 2026-10-27 human 拍板：UI 设计版本结构整体调整（显式 current 指针 + 逐版备注 + 截图布局）**并入 P5** 一起改（三选一拍「并入」；单独立项 / 只登记暂缓两案否）。
- 2026-10-26 human 拍板：261018 搭载 P1；260924② 拆出独立批；OQ1 取「允许集从账本拓扑正推」（候选 2 形态）；OQ2 修门+兜底成文；OQ4 闸内点名。
- 2026-10-26 human 指示：261014④ 拆出须登记 governance debt 防忘 → ARQ-261026-final-phase-dispatch-contract-unwritten。
- 2026-10-26 human 反馈：发布 PR 门面「没写改了啥」→ ARQ-261026-release-pr-gov-lane-zero-pods；PR #40 门面已人工重写。
- 本 workstream 走 `/gov-change` 面 loop（不涉 dev-loop 拆包派遣，milestones.jsonl 登记不适用；若后续某波要拆 dev-loop 包，先补里程碑登记再派 portfolio-planner）。
