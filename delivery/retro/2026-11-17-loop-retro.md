# loop-retro · 2026-11-17（Dev Loop OS 周期复盘 · 第 6 轮）

> 范围：`loop-health-report.mjs --pending` 报 `pendingAnalysis` 19 包（261024 ~ 261104）+ `waitingOnLoopHealth` 7 包；`pending + waiting = 26 ≥ 3`，复合触发条款成立。19 包全部 post-cutoff 且 `pod-events.jsonl` 在位 → 全部走 new-format（不读 loop-health / session-digest / pod-journal）。其中 `261031-openclaw-cron-trigger-gating` 与 `261104-k0-points-e2e-spike` 缺 `pod-retro.md`（advisory，不阻复盘）。
> 口径：机器面数字以 `pod-events-aggregate.mjs` / `retro-efficiency.mjs` 为唯一权威；本报告只做跨包归纳与分流，不重算 verifier / supervisor 事实。

## 一、机器聚合总览

- **19 包 / 277 条事件**（合计 eventsTotal），malformed 行 0。
- **schema 告警 138 条**，几乎全是 `unknown-event-type`（事件类型不在 `pod-events-append.mjs` 的 9 值闭集内）：`merge-completed`、`human-merge-gate-approved`、`planner-ruling`、`verify-red`、`live-deploy`、`device-flash`、`acceptance-frozen`、`builder-complete`…… 单包最高 261104-k2 的 32 条、19 种类型。闭集只用于写入口校验，实际事件流早已是更大的词表；聚合器 fail-open 告警、按未知类型跳过专项聚合。
- **dispatch 遥测断层**：19 包中 9 包（261028-m6-k1 / m6-k2a / m6-k2b、261031-cron、261104-k0 / k1 / k2 / k4 / k5）**零 `dispatch-completed` 事件**，因而 token / agent 分钟 / 模型列全空。有 dispatch 的 10 包共 42 条、全部带 exact token。
- **escalation**：2 条，均 unresolved——261024-vq-overlay `29530b79`（environment：真机目检门停等，info）与 261104-k2 `66447168`（无 `category` 字段 → 聚合为 unknown；实为 final r3 红闸升级，实际已由 A 案 + round 4 绿 + human 批合并消解，但缺 resolved 事件）。
- **friction**：23 条，类型分布 `over-scan` 13 / `unknown` 6 / `degraded` 2 / `scan-miss` 1 / `doc-mismatch` 1（其中 unknown 6 条含 `item/workaround/suggestion` 承载、无 `frictionType`，聚合落 unknown）。语义承载率仍低。
- **human-correction 2 条**（触发条款①）：261024-oneshot `f13a53a8`（request 级 system 指令被 OpenClaw 忽略，human 裁「回到验证过的形态」）、261104-k3-qq `07dffb14`（human 原话「K2 的真机轮跑过了呀」——planner 只读 progress 冻结时点段落误记状态）。

## 二、跨包共性主题

### T1 · `change-manifest` 闸的 pre-cutoff 判定读的是 current/ 投影文件 → **所有 post-cutoff 包被静默跳过**

- **证据**：261031-cron `pod-events.jsonl` friction `3ef694bb-7a50-4631-ba96-d28d2b223279` 逐字：「`change-manifest-check` 默认读 current/taskpackage.json 取 gitFirstCommitDate，因该指针文件自 2026-09-11 起就存在，任何新包都会被判 pre-cutoff 跳过（本包实测 skipped）」。脚本实核：`.claude/scripts/change-manifest-check.mjs:53-58` `taskPackagePath()` 优先返回 current/ 投影路径，`:137-139` 对该路径取 `gitFirstCommitDate`；`MANIFEST_CUTOFF = '2026-09-28'`。对照同谓词姊妹闸 `.claude/scripts/taskpackage-required-check.mjs:146-150`（`gitFn(\`delivery/taskpackages/${cid}\`)`，取包目录首提日）——两者对同一 cutoff 用了不同路径基准。
- **影响面**：cutoff 之后（≥ 2026-09-28）的**每一个包**。changeManifest 对账（清单外实现面文件 fail-closed）在 verify 里报 `ok:true / skipped:'pre-cutoff'`，是**静默假绿**；261031-cron 是首个显式发现者，靠手传显式路径才真正对账。
- **处置**：查无同根 → **新立 ARQ-261117-change-manifest-cutoff-projection-bypass**（severity MAJOR）。

### T2 · 冻结验收件带内部矛盾 / 不可满足断言进冻结——本轮六包复现，且「自检条款」是散文义务

- **证据**：
  - 261028-m6-k2b F1：builder step-0 现跑 258/7，追出**冻结件 6 处逻辑不可满足缺陷**（A1 墨高窗 [15..18] 越证据域；旧位全行白负锚与 7 条日列竖线互斥；TEST 3 截断帧对误比；tooth6 day0 判 day2；helper 极值命中带底与竖线）。
  - 261028-m6-k3 F1：STY-A2 多行带跨 2 自然行恒越 [13..16]（与 STY-A4 WRAP 折行互斥 = 对任何实现不可满足）；STY-A6 STAMP 带 y0=150 与印章圈底缘相切。pod-retro 自评「P2 F1 教训兑现……但仍漏了『夹具内容物与既有渲染家具的空间关系』」。
  - 261104-k1 #1：三处（A4 revision 与 A8/契约互斥、chain helper ±1 步长假设、A9 期望字节/权威值算错），「全是『实现缺席型红』，只有实现在盘后才暴露」。
  - 261104-k4 #1：K4G-A1 对空目录先快照再断言「非空」（不可满足）；K4G-A7 与 K4G-A2 对「删激活中自定义」给出相反期望。
  - 261104-k5 #2：冻结件跨用例状态残留（SPV2-A1 tooth 3 留 COVER、tooth 4 未复位），「K2/K4 之后已写进派单『跨用例一致性自检』，仍然发生」。
  - 261028-m6-k2a F1：P0 冻结件只钉返回码不钉度量合理性 → P0 假绿，16px 字体 LARGE 标志错误直到 P1 交叉红才暴露。
- **根因**：验收冻结的「红基线正确」只证红因与设计态，不证「绿路径可满足」；跨用例一致性自检已三次成文、三次未执行（散文义务无执行位）。正向对照：builder 全程不改冻结件、用诊断副本定位，acceptance-author 凭实测收紧型修订——跨角色对抗把缺陷钉在可诊断形态，没有溜进实现面歪修。
- **处置**：查无同根（ARQ-261025-adjacent-frozen-anchor-sweep 治的是**跨包**字面锚，非**单件内**一致性）→ **新立 ARQ-261117-frozen-acceptance-satisfiability-discipline**（severity SHOULD）。

### T3 · mutationEvidence 生命周期三缺口：投影无 owner、变异态跨 commit 边界、MASKED 不复测

- **证据**：
  - **投影无 owner**：261028-m6-k3 F3「verify r1 红 = mutation-check 报 7 条 missingSensitive——builder 变异取证齐全，但 taskpackage.mutationEvidence.perAcceptance 投影没人写（builder 不写 taskpackage、acceptance-author 不掌变异、planner finalize 时点早于变异取证完成）」；261104-k3 #3 逐字「连续两包留到 verify 才补（K2 同型，K3 复发）」；261104-k4 遗留「verdict.json advisory：本轮 bearing 事实是 rounds/r1.json，current/verdict.json 未产」。
  - **变异态跨 commit 边界**：261028-m6-k2b F2「主线程在 builder 变异轮进行中提交实现轮 commit，把 M3 变异值 `ksg_slot_caps {4,4,3}→{4,4,4}` 带进 HEAD」；修复 = 18ff3a22 还原 + 复测回填。pod-retro 自提「新铁律候选：主线程 commit 前必须核 builder 是否挂着临时变异」。
  - **MASKED 不复测**：261028-m6-k2b F3「M3/M6 首测 MASKED——当时冻结件缺陷齿先中断、变异齿没跑到……MASKED 不是终点：遮挡齿消除后必须复测，否则变异证据缺两条、判别力覆盖虚假齐全」。
- **处置**：full-text 匹配口径属在册 `ARQ-261028-mutation-evidence-authoring-pointer`（见 GC1），三缺口为新根 → **新立 ARQ-261117-mutation-lifecycle-ownership-and-commit-guard**（severity SHOULD）。

### T4 · 机械闸的标记 / 断言识别面窄——新子项两条 + 在册复现一条

- **新子项**：
  - `hasRealAssertion` 不认 Unity 断言：`.claude/scripts/acceptance-coverage-check.mjs:74-83` 只认 JS 风格 / Python unittest / 裸 assert；`TEST_ASSERT_*_MESSAGE` 因词边界不匹配而一律不认。261104-k4 #2 逐字「纯 Unity 的 C 验收件会假红，K2 能过只是因为恰有一处 libc `assert()`。本包按 K2 先例补真实 OOM 守卫绕过」。
  - 旗标面不一致：`acceptance-coverage-check.mjs:23` 只吃位置参数路径（`process.argv[2]`），不认 `--change-id`；`mutation-check.mjs` 认。261104-k5 #4「两次派单都写错、靠执行者自行纠正」。
  - 标记正则只认紧邻 id：`.claude/scripts/acceptance-coverage-check.mjs:47-50` 要求 `coverage: <id>` 紧邻。261025-kidpal-tools-profile-fix 报 `coverage: A1, K3-E6`（id 不在首位）假红；261104-k5 #1 报 `coverage: S-F2.6-AC1 -> KCN-C1` 人读合理、闸只认 `coverage: KCN-C1`，round 1 整批 missing。
- **处置**：前两条查无同根 → **新立 ARQ-261117-acceptance-coverage-assertion-and-flag-gaps**（SHOULD）；第三条同根在册 `ARQ-260907-taskpackage-format-expectations-frontload`（已归档、闭集内已收五次复现）→ **追加证「复现六」，不新立**。

### T5 · 派发 / 起包的操作性事实没有前置——目标文件死活、manifest 起草面、只读预评无界、写通道、设备编译通道

- **证据**：
  - **派单目标文件已归档**：261104-k2 #2「派 acceptance-author 扩 artt_expected 前没核 ui_art_integration.acceptance.c 已归档（260915），派单即死件。与 K1 教训『路由语义先读权威文件』同型第三次」。
  - **manifest 起草漏编译单元与生成件命名律**：261104-k4 #3「契约逻辑标识直接写成文件名，五个里五个错」；261104-k5 #5「连续两包漏『实现会动的 TU』，粗 glob 只列 .cc 会漏 .h」。
  - **只读 advisory 无界扩散**：261104-k3 #2「architect 首轮给了 11 项必读清单 + 一条追加消息，40 分钟未收口」。正向做法同包第二轮：硬帽 12 次工具调用 + 明确产出格式 + 已知事实前置，10 分钟收口。
  - **写通道提示**：261104-k3 #4「acceptance-author 超预算执行里有一次 `cat >` 被 universal.bashDenyWrite 拦，改 Write 才落盘」。
  - **设备编译通道**：261028-m6-k3 near-miss ①「brief 误写『WSL 下跑 device build』——skill 铁律 device build = Windows venv + `c:/tmp/idf_run.py`」。
  - **上一包状态事实源**：261104-k3-qq human-correction `07dffb14`「核上一包真机轮状态时读 `device-round-evidence.md` 与 pod-events 尾部，不只读 progress 冻结时点的段落」。
- **根因**：派单 brief 承载的是「要做什么」，不承载「先核目标在不在 / 怎么写 / 用哪个通道 / 去哪读事实」这批操作性事实；同类第三次、第四次仍由执行者自觉兜住。
- **处置**：查无同根 → **新立 ARQ-261117-dispatch-brief-operational-facts**（severity SHOULD）。

### T6 · 孤儿 headless writer 双写冻结面——被停进程未清场就重拉

- **证据**：261104-k0a #1 逐字「前两次被停的 headless planner 进程存活，其 acceptance-author 与新会话派出者并发写同一冻结验收面——acceptance-author 停点上报 + planner 定点 taskkill（按 CommandLine 鉴别）+ 已落盘产物在盘评审后接管」；pod-events 有 `orphan-writer-resolved` 事件。共性结论原文：「headless 拉起被停后必须进程面清场（查残留 `claude -p` 进程）再重拉」。
- **影响面**：一切 headless / 可中断拉起形态；双写冻结面会静默污染验收基线，且两个 writer 都不知道对方存在。
- **处置**：查无同根 → **新立 ARQ-261117-orphan-headless-writer-cleanup**（severity SHOULD）。

### T7 · 新闸上线没有全链演练——首包当小白鼠

- **证据**：261104-k0a #4 逐字「merge-driver 传闸 `--source-ref` 字面量 'HEAD' 不归一化 + 闸 deviceOutcome sourceCommit 严格相等在『证据随合并提交』下自引用不可满足——261028 P8 设备轮硬门立起后首个端到端走通该路径的包把两处缺口全部踩出。共性：新闸立起后应有一次『闸链演练』（dry-run 走通全链）再依赖；首包当小白鼠的成本是三轮真机 + 四轮闸拉锯」。human 两次亲签热修。
- **影响面**：每次新闸 / 闸链改动的第一批包；成本形状 = 真机轮与闸拉锯叠加，且失败点分散在 merge-driver 与 evidence 校验两处。
- **处置**：查无同根（ARQ-261014-merge-gate-tooling-hotfix-batch 已归档，治的是具体工具缺陷，非「上线前演练」纪律）→ **新立 ARQ-261117-new-gate-dry-run-rehearsal**（severity SHOULD）。

### T8 · final 相位 scope-file 序列——在册已结项仍三包复现（GC1，不新立）

- **证据**：261024-oneshot「verify.mjs final 相位要 --scope-file，而我第一跑没带……planner Step 10『终审 full bearing 跑一次』实际只能排在 integrator 之后」；261024-vq-overlay「scope-file 的消费接口要写进 integrator 派单……final r4 被防 typo fail-closed 打回」；261025-openclaw-dreaming「final 相位缺 scope-file 把我拦了一下……这不是 bug，是我对『final 归谁跑』的记忆有点糊」。
- **对账**：现行 canonical 已在位——`.claude/agents/planner.md:35-39`「派发书 final 相位三条契约」（谁跑 / scope-file 谁产 / round 序数），`.claude/hooks/verify.mjs:239` 报错行附带「正确序列 = scoped 绿 → integrator 圈定 → final；本次在开跑前退出，未发布 fact、round 序数不消耗」。`ARQ-261026-final-phase-dispatch-contract-unwritten` 已于 2026-10-26 结项并落这四件。
- **裁定**：**复现——已裁决项未被行为闭合**，不新立 ARQ；证据引现行锚点，记入本报告与下一周期关注点。

### T9 · 遥测保真继续失真：闭集外事件类型、零 dispatch 事件、escalation 缺字段 / 缺闭合（GC1 追加）

- **证据**：见 §一（138 条 schema 告警 / 9 包零 dispatch / 2 条 unresolved escalation）。
- **对账**：`ARQ-261014-pod-events-booking-telemetry-fidelity` 在册，其 ①（escalation 闭合链）、②（schema 校验收敛）、追加校验（事件类型枚举告警 + payload 必填校验）由 P9 批（`261029-gov-review-p9-script-hygiene-cluster`）承接；本轮的 138 条告警正是 P9「枚举告警」落地后的读数——**告警在工作、闭集本身没有扩**，写侧仍大量手写闭集外类型。
- **裁定**：同根在册 → **追加证据，不新立**。

### T10 · `changes-index --upsert` 在 dev lane 被误用（GC1，不新立）

- **证据**：261025-openclaw-dreaming「我按角色文档让 integrator 跑 `changes-index.mjs --upsert`，结果那个旗标是 review lane 专用，dev 包必失败。integrator 没硬顶，改用裸跑全量重建再回退别的 lane」。
- **对账**：现行 canonical 已限定 lane——`.claude/references/gov-change.md:29`「`--upsert` 在落地 commit 前运行」（治理面）；`.claude/skills/merge-to-main/SKILL.md:35-38` 同一用法；`ARQ-260915-changes-index-lane-split` 已于 2026-09-15 结项（dev / review / intake 三份 INDEX）。**复现——已裁决项未被行为闭合**（角色文档找不到「dev 包刷 INDEX 走全量重建」的正面一句），不新立 ARQ；建议并入 T5 的派发事实块。

### T11 · 「host 绿 ≠ 真机成立」继续兑现（正向，不新立）

- **证据**：261028-mirror-desync 三处修法全落在 `application.cc` / `kidpal_render_node.cc` / `mcp_server.cc`，host 只能写结构锚，「三分申报（结构锚 / 可执行 canary / 只能真机目检）」写进件头；261104-k0a #3「host 全绿 ≠ 真机成立（F1 键帽 sync 链、F2 云端阅读路径双真相源）」；261028-m6-k2a F3「消费侧冻结前先核对 producer 实况，防 host 全绿真机零样本」；261104-k5 遗留「`pre-merge-escalation-gate` 已按预期 BLOCK 在 `device-round-evidence-missing`」。
- **裁定**：同族在册（`ARQ-261001-host-blind-outcome-device-gate`，已归档）且本轮为**正向兑现**——真机门机械拦在位、三分申报在文面。不追加负向证据；保留为下一周期观察点。

## 三、GC1 · findings 对账 canonical / 已裁决项

对账集 = `docs/debt/gov/` 在册活条目 + `_archive/2026/` 直接相关条目 + 现行 canonical（`planner.md` 派发书、`verify.mjs` 报错行、`gov-change.md`、`merge-to-main/SKILL.md`、`pod-events-append.mjs` 闭集、`acceptance-coverage-check.mjs` 现行判据）。

- **4 条与已裁决项同根，记复现 / 追加、不新立**：T4③→`ARQ-260907-taskpackage-format-expectations-frontload`（复现六）；T8→`ARQ-261026-final-phase-dispatch-contract-unwritten`（已结，三包复现，现行锚点已核：`planner.md:35-39`、`verify.mjs:239`）；T9→`ARQ-261014-pod-events-booking-telemetry-fidelity`；T10→`ARQ-260915-changes-index-lane-split`（已结，一包复现，`gov-change.md:29`）。
- **0 条与现行 canonical 冲突**：本轮未出现「建议补建已退役 def」类漂移；新立条目均为现行文面未覆盖的新对象。
- **0 条需要升级 human 的方向级问题**：全部为流程 / 工具面，方向已在既有裁定内（intake 漂移标的 `261014-single-tier-bringlist-home` 例外，见 §七）。

## 四、GC3 · new-format 覆盖比对（机器聚合 vs 散文主题）

17 个有 `pod-retro.md` 的 new-format 包逐主题人工核对，**「retro 有主题、events 无对应 event」仍是系统性常态**：

| 包 | ledger friction | retro 主题数（近似） | 偏差形状 |
|---|---|---|---|
| 261028-m6-k1 | 0 | 4（F1–F4） | ledger=0 / retro=4 |
| 261028-m6-k2a | 0 | 3（F1–F3） | ledger=0 / retro=3 |
| 261028-m6-k2b | 0 | 4（F1–F4） | ledger=0 / retro=4 |
| 261028-m6-k3 | 0 | 4（F1–F4） | ledger=0 / retro=4 |
| 261028-mirror-desync | 0 | 3 | ledger=0 / retro=3 |
| 261104-k0a | 0 | 5 | ledger=0 / retro=5 |
| 261104-k1 | 0 | 5 | ledger=0 / retro=5 |
| 261104-k2 | 0 | 6 | ledger=0 / retro=6 |
| 261104-k5 | 1（doc-mismatch） | 5 | 1 vs 5 |
| 261025-openclaw-dreaming | 1 | 5 | 1 vs 5 |
| 261024-vq-overlay | 3 | 6 | 3 vs 6 |
| 261104-k3 | 2 | 5 | 2 vs 5 |
| 261104-k4 | 3 | 4 | 3 vs 4 |
| 其余 4 包 | 2–4 | 4–5 | 2 vs 4 / 3 vs 4 等 |

- 16/17 包为「ledger 摩擦主题数 < retro 散文主题数」，其中 8 包为 `ledger=0 / retro=3..6`。
- 另有 2 包无 pod-retro（261031-cron / 261104-k0）：仅能读事件面，不参与覆盖比对（advisory）。
- 反向补充：9 包零 `dispatch-completed`（§一）使机器侧连「过程长度」都测不到，覆盖比对只能靠散文。
- **结论**：偏差不采信任一源；系统性缺口按 GC1 追加到 `ARQ-261014-pod-events-booking-telemetry-fidelity`，不另立。

## 五、分流清单

**治理类 → 新立 ARQ（主线程登记进 `docs/debt/gov/`）**

| 条目 | 主题 | severity | riskTier |
|---|---|---|---|
| ARQ-261117-change-manifest-cutoff-projection-bypass | 闸的 pre-cutoff 判定读 current/ 投影 → 新包静默跳过 | MAJOR | low |
| ARQ-261117-frozen-acceptance-satisfiability-discipline | 冻结件内部矛盾 / 不可满足断言的交付前自检与实测校准 | SHOULD | medium |
| ARQ-261117-mutation-lifecycle-ownership-and-commit-guard | mutationEvidence 投影 owner + 变异态 commit 边界 + MASKED 复测 | SHOULD | low |
| ARQ-261117-acceptance-coverage-assertion-and-flag-gaps | Unity 断言不识别 + 旗标面与 mutation-check 不一致 | SHOULD | low |
| ARQ-261117-dispatch-brief-operational-facts | 派单目标死活 / manifest 起草面 / 只读预演边界 / 写通道 / 设备编译通道 | SHOULD | low |
| ARQ-261117-orphan-headless-writer-cleanup | 被停 headless 进程未清场即重拉 → 冻结面双写 | SHOULD | low |
| ARQ-261117-new-gate-dry-run-rehearsal | 新闸上线前的全链 dry-run 演练纪律 | SHOULD | low |

**治理类 → 追加证据（不新立）**

| 条目 | 追加内容 |
|---|---|
| `docs/debt/gov/_archive/2026/ARQ-260907-taskpackage-format-expectations-frontload.md` | 复现六：coverage 标记多 id / 语义箭头形态（261025、261104-k5） |
| `docs/debt/gov/ARQ-261028-mutation-evidence-authoring-pointer.md` | 四包复发（261028-m6-k1 F4、m6-k3 F3、261104-k2、k3）；K0 记录「ARQ-097 有意分歧」为第三轮根因 |
| `docs/debt/gov/ARQ-261014-pod-events-booking-telemetry-fidelity.md` | 138 条 schema 告警 / 9 包零 dispatch / 2 条 unresolved escalation / GC3 16/17 偏差 |
| `docs/debt/gov/ARQ-260913-current-pointer-implicit-global.md` | 261028-m6-k2b near-miss：切包与 verifier 首跑之间的时序窗，误 append 到旧包 verify-runs.jsonl |
| `docs/debt/gov/_archive/2026/ARQ-261026-final-phase-dispatch-contract-unwritten.md` | 复现：三包（261024-oneshot / vq-overlay / 261025-dreaming）仍撞 final scope-file 序列 |

**实现类 → 正常 dev-loop**：无确定的 `firmware/**` / `server/**` 缺陷待修；本轮真机轮发现的缺陷均已在原 Pod 内由 fixer 收口（K2 四线修复批、K5 归档离页帧等）。

**信息类 / 接受风险**：
- over-scan 13 条（acceptance-author / builder 的取证性超读）——结构性必要读，不加收紧动作。
- 261104-k0a 的 `ic_wifi_14` / `p1_chip_mingzao` 资产管线母版漂移（K2 已登 tech-debt 同族，本包不重复立账）——owner = dev-loop，触发 = 下次全量重跑资产管线。
- 261028-m6-k3 的 FAS 键名漂移（`story_h_reader_lines3` vs `story_reader_lines3`，冻结件头注与 wiring patch 两处独立命名）——建议 dev-loop 侧「wiring patch 从冻结件 [M] 节逐字复制」，不立 ARQ。
- 261104-k3 的 `command_id` 平台消息 id 可得性未核——真实 QQ 轮第一件事，owner = dev-loop。
- 261104-k4 的预算低估（builder 124/45、acceptance 26/20 等）——含资产 / 二进制 / 宿主编译的包按「次数量级」给预算，owner = dev-loop 排期面。

## 六、waiting 清单（不动）

`260829-audio-bargein-hw`、`260829-shim-skeleton`、`260831-cloud-link-bringup`、`260831-simulator-lvgl-align`、`260913-happy-flow-host-capture`、`260924-ops-openclaw-family-user-mapping`、`261024-qa-session-timeout-producer`。

前六个为 pre-cutoff 旧格式且 `loop-health.md` 缺失，连续多轮 waiting；`261024-qa-session-timeout-producer` 为新增（既无 pod-events 也无 loop-health，疑为未收口或未晋升的包），待 integrator 补证据后进入 new-format 路径。七个均不动；若 human 裁定前六个结构性永不析出，可走 human-only 销账（`--writeoff`）。本轮 `waitingOnLoopHealth` 已不再含 `current` 假阳性（前轮已由 `ARQ-261025-intake-index-retro-scope-hygiene` 结项消化）。

## 七、intake 漂移处置（owner = 主线程，本轮 retro 执行者）

| changeId | 漂移标 | 三裁 | 理由 / 动作 |
|---|---|---|---|
| 261103-star-points-story-cover | ⚠未闭环 | **续期** | K0–K5 实现面在途（K5 真机轮 / 跨包联合收口 / ECS 部署轮未闭）。新期限 = 2026-12-05。 |
| 261028-font-typography-scaleup | ⚠未闭环 | **续期** | M6 K1–K3 landed，P3 终端真机轮 + human design review 终判未跑。新期限 = 2026-12-05。 |
| 261022-device-memory-prompt-overhaul | ⚠未闭环 | **续期** | workstream M5 列车 P1–P4 已落，P4 backfill 复测 / P5 收口与真机配额未闭。新期限 = 2026-12-05。 |
| 261021-p3-record-cancel-lifecycle | ⚠源文件缺口（frontmatter 未闭合） | **升级（主线程修补 spine）** | spine 实文含 `status: in-flight`；`changes-index.mjs` 仍报缺口，说明 frontmatter 解析面另有未闭合处（疑似 faceMap 嵌套 / 中文冒号串干扰简易 YAML 解析）。动作 = 主线程修 spine 或解析器，使 `status` 可读；完成后重跑 `changes-index.mjs` 全量重建。 |
| 261014-single-tier-bringlist-home | ⚠未闭环 ⚠滞留32d | **升级（交 human 裁）** | `status: awaiting-human:立项-ack` 自 2026-10-14 滞留 32 天，agent 无法自行放行；请 human 裁 ack 继续 / 关闭 / 重排。 |
| 261012-kidpal-realtime-facts | ⚠未闭环 | **清理** | taskpackage 在 `delivery/taskpackages/INDEX.md` 已为 `closed`（`supervisor-verdict.json` 在场，pod 已于 loop-retro-2026-10-14 覆盖）；spine 的 `status: in-flight` 系陈旧未翻。动作 = 主线程把 spine `status` 翻 `closed` 并重跑 `changes-index.mjs`。 |

> 逐条留痕视为本项完成；「仍见漂移」本身不再是升级判据——升级判据 = 同一条目连续两轮被裁「续期」仍无动作。

## 八、下一周期关注点

1. **T1 是本轮最危险的一条**：一个 fail-closed 闸因路径基准选错而静默放行所有新包——建议下一个治理批优先处置，并在修复后取一个已跳过包做负向复证（删 manifest 行必须变红）。
2. **T2 / T3 / T4 是「验收与变异」三件套**：冻结件可满足性、变异投影归属、断言/标记识别面；它们合起来解释本周期大量 verify 轮返工。建议同一治理批顺序处置。
3. **T8 / T10 的复现说明：文面成文 ≠ 行为闭合**。两者都已有现行锚点与机械报错提示，仍各烧轮；下一周期若再复现，应考虑从「提示」升级为「派单模板硬条目 + 闸级强制」。
4. **遥测面**：9 包零 dispatch 事件意味着效率表对半数新包失效；建议在 `ARQ-261014` 后续批中把 `dispatch-completed` 缺失本身做成可见告警（当前只有 silent 空行）。
5. **真机轮诚实申报继续兑现**：K2a / K5 的三分申报与 merge 门 BLOCK 是正向样本，保持；不要把「host 全绿」再当作收口语言。
6. **账本卫生（本轮发现，非 retro 产物）**：`node .claude/scripts/debt-doctor.mjs` 报 5 条既有 RED——`TECH-261105-p3-list-screen-label-clipped`（status `open` 出闭集、severity `MINOR` 出闭集）、`REQ-261105-story-list-beyond-window-visibility`（status `open`）、`CDQ-261103-star-points-story-cover`（status `resolved`、severity `MUST`）。与本轮新立的 7 条无关（新条目 doctor 零红）；建议主线程顺手修字段（`STATUSES` / severity 闭集见 `.claude/scripts/debt-ledger-shared.mjs`）。

## 九、效率表三张（`retro-efficiency.mjs --since 2026-10-24`，原样贴出）

### ① Pod 表

```text
# 效率表（since=261024）

| changeId | 档位 | 模型 | 代码行 | 其中测试行 | 文档行 | 文档代码比 | token | agent 分钟 | 墙钟分钟 |
|---|---|---|---|---|---|---|---|---|---|
| 261024-m5-k4b-corpus-parser-v2 | lite·拆包 | glm-5.3 | 1,813 | 754 | 1,375 | 0.76 | 543,444 (3/3) | 63.1 | 36.2 |
| 261024-memory-recall-tool-enable | lite·拆包 | deepseek-flash | 947 | 787 | 1,826 | 1.93 | 439,023 (3/3) | 17.0 | 52.8 |
| 261024-oneshot-user-uniformity | lite | deepseek-v4.1+glm-5.3 | 655 | 512 | 1,342 | 2.05 | 800,380 (7/7) | 53.2 | 157.0 |
| 261024-qa-session-timeout-producer | full |  |  |  |  |  |  | 0.0 |  |
| 261024-vq-overlay-close-end-session | full | deepseek-v4.1+glm-5.3 | 354 | 242 | 1,409 | 3.98 | 431,624 (5/5) | 39.4 | 159.0 |
| 261025-kidpal-tools-profile-fix | lite·拆包 | deepseek-flash | 1,547 | 1,364 | 1,594 | 1.03 | 588,964 (5/5) | 26.2 | 50.7 |
| 261025-openclaw-dreaming-disable | lite·拆包 | deepseek-flash | 2,344 | 1,652 | 1,441 | 0.61 | 526,338 (4/4) | 23.2 | 31.0 |
| 261028-m6-k1-font-assets-runtime-loading | full·拆包 |  | 3,387 | 2,730 | 3,156 | 0.93 |  | 0.0 | 1881.0 |
| 261028-m6-k2a-p1-home-16px | full·拆包 |  |  |  |  |  |  | 0.0 | 675.2 |
| 261028-m6-k2b-p2-schedule-16px | full·拆包 |  | 1,868 | 1,639 | 1,433 | 0.77 |  | 0.0 | 298.4 |
| 261028-m6-k3-p3-story-typography | full·拆包 | deepseek-v4.1+glm-5.3 | 1,573 | 1,498 | 2,244 | 1.43 | 3,054,930 (5/5) | 128.0 | 367.5 |
| 261028-p3-record-session-mirror-desync-fix | full | deepseek-v4.1+glm-5.3 | 1,057 | 1,018 | 1,144 | 1.08 | 461,835 (3/3) | 44.7 | 306.4 |
| 261031-openclaw-cron-trigger-gating | lite |  | 2,439 | 1,748 | 1,938 | 0.79 |  | 0.0 | 143.4 |
| 261104-k0-points-e2e-spike | full·拆包 |  | 4,779 | 2,788 | 1,387 | 0.29 |  | 0.0 | 1192.9 |
| 261104-k0a-four-page-skeleton | full·拆包 | deepseek-v4.1+glm-5.3 | 2,487 | 2,166 | 3,368 | 1.35 | 2,087,292 (6/6) | 92.4 | 1150.8 |
| 261104-k1-points-ledger-core | full·拆包 |  | 2,987 | 2,092 | 1,445 | 0.48 |  | 0.0 | 145.8 |
| 261104-k2-points-device-ui | full·拆包 |  | 5,493 | 2,745 | 3,450 | 0.63 |  | 0.0 | 12681.2 |
| 261104-k3-qq-points-channel | full·拆包 | qwen3.8-max |  |  |  |  | 16,236 (1/1) | 10.0 | 826.9 |
| 261104-k4-reward-goal-config | full·拆包 |  | 3,676 | 1,630 | 1,560 | 0.42 |  | 0.0 | 702.2 |
| 261104-k5-story-cover-nav | full·拆包 |  | 3,093 | 2,147 | 2,927 | 0.95 |  | 0.0 | 648.8 |
```

> 文档代码比 = 文档行 ÷ 代码行；档位 = `taskpackage.json.rigor` 原值；行数只归因包内命名合并与按 manifest 可切分的批量合并。口径以脚本为唯一权威。

### ② 按 Agent 角色汇总

```text
| 角色 | 派发次数 | exact token | agent 分钟 | token 占比 | 时长占比 | 覆盖（有数据 Pod 数） |
|---|---|---|---|---|---|---|
| acceptance-author | 14 | 4,236,302 | 180.0 | 39.3% | 36.2% | token 9 / 时长 9 |
| builder | 10 | 2,603,341 | 188.1 | 24.1% | 37.8% | token 8 / 时长 8 |
| planner | — | 1,832,431 | 1807.5（主线程时段，含人等） | 17.0% |  | 6/20 |
| integrator | 10 | 1,076,432 | 68.8 | 10.0% | 13.8% | token 9 / 时长 9 |
| fixer | 4 | 651,919 | 37.0 | 6.0% | 7.4% | token 3 / 时长 3 |
| architect | 4 | 382,072 | 23.4 | 3.5% | 4.7% | token 4 / 时长 4 |
| 脚本驱动（verifier / supervisor） | — | — | — | — | — | 不计入 agent 成本 |
```

> token 占比分母 = 本表所有有 exact 值的行合计（含 planner，合计 10,782,497）；planner 的时间列是「主线程时段 = 墙钟 − Σ派发时长」，包含人等与串行等待，不并入时长占比。

### ③ 时间拆分

```text
| changeId | 观察窗分钟 | agent 分钟 | 等待分钟 | 数据来源 |
|---|---|---|---|---|
| 261024-m5-k4b-corpus-parser-v2 | 36.2 | 63.1 |  | wall-clock-legacy |
| 261024-memory-recall-tool-enable | 52.8 | 17.0 |  | wall-clock-legacy |
| 261024-oneshot-user-uniformity | 157.0 | 53.2 |  | wall-clock-legacy |
| 261024-qa-session-timeout-producer |  | 0.0 |  | unknown |
| 261024-vq-overlay-close-end-session | 159.0 | 39.4 |  | wall-clock-legacy |
| 261025-kidpal-tools-profile-fix | 50.7 | 26.2 |  | wall-clock-legacy |
| 261025-openclaw-dreaming-disable | 31.0 | 23.2 |  | wall-clock-legacy |
| 261028-m6-k1-font-assets-runtime-loading | 486.9 | 0.0 | 486.9 | lifecycle |
| 261028-m6-k2a-p1-home-16px | 623.0 | 0.0 | 623.0 | lifecycle |
| 261028-m6-k2b-p2-schedule-16px | 224.8 | 0.0 | 224.8 | lifecycle |
| 261028-m6-k3-p3-story-typography | 155.7 | 128.0 | 27.7 | lifecycle |
| 261028-p3-record-session-mirror-desync-fix | 34.5 | 44.7 |  | lifecycle |
| 261031-openclaw-cron-trigger-gating | 26.2 | 0.0 | 26.2 | lifecycle |
| 261104-k0-points-e2e-spike | 632.9 | 0.0 | 632.9 | lifecycle |
| 261104-k0a-four-page-skeleton | 712.6 | 92.4 | 620.3 | lifecycle |
| 261104-k1-points-ledger-core | 145.8 | 0.0 |  | wall-clock-legacy |
| 261104-k2-points-device-ui | 12666.3 | 0.0 | 12666.3 | lifecycle |
| 261104-k3-qq-points-channel | 81.6 | 10.0 | 71.6 | lifecycle |
| 261104-k4-reward-goal-config | 78.5 | 0.0 | 78.5 | lifecycle |
| 261104-k5-story-cover-nav | 646.6 | 0.0 | 646.6 | lifecycle |
```

> 观察窗缺失显示 unknown、不显示 0、不倒推。`wall-clock-legacy` = 旧的首末事件差近似。等待分钟 = 观察窗 − agent 分钟。

### ④ fixer 根因分布

```text
fixer 根因：0/4 派发带根因（本批之前的历史派发无此字段）。
```

> `friction` 是 planner 视角的过程摩擦，`rootCause` 是单次 fixer 派发的归因标签；两者粒度为生产者不同，不 join、不双记、不相加。

### ⑤ 数据缺口说明

- 行集 20 个 Pod：行数可归因 17、归因不到 3（留空）。
- 档位：full 14 / lite 6 / nano 0 / governance 0 / 拆包 15 / 留空 0。
- token：有 exact 10、无 exact 10（**其中 9 包零 dispatch 事件**，见 §一）。
- 模型：有记录 10、无记录 10。
- planner 检查点：可算 6、首末相同 3、首末为负 0、相邻倒退 0、检查点不足 1。
- 重复盖章：剔除 0 行 / 0 token（涉及 0 个 Pod）；id 复用组 3 个（只计数、未剔除）；无 dispatchId 的 dispatch 0 条。
- 批量合并切分无主 / 歧义路径：58 条（见 `--json.batchUnattributedPaths`）。
- 域外路径：0 个文件 / 0 行。
- **token coverage**：事件面 `{dispatchesTotal: 42, dispatchesWithTokenData: 42, dispatchesUnavailable: 0}`；包面 10/19 有 exact token，9/19 零 dispatch 事件（不可测，不是 0）。