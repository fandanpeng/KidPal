# Review Packet · 261023-agents-md-static-semantics-alignment（rigor: lite）

- **author**：integrator（dev-loop，pod-m2 worktree `C:\AI\KidPal-pod-m2`，分支 `dev/pod-m2`）
- **date**：2026-10-23
- **性质**：批 A+B 静态 prompt 措辞对齐（两文件 + 新验收件），零代码、零契约、零 spec。
- **plan**：`delivery/taskpackages/261023-agents-md-static-semantics-alignment/plan.md`（冻结技术输入，只读消费）

## 1. Change size

- `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`：改（102 → 93 行，提醒节压缩 -10 行 + 其余行内改写；plan §7.6 预估 94 行，实测 wc -l = 93，差异为行尾计数口径，不涉锚）。
- `server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md`：改（4 处正文 + 2 处指针，行数不变 40 行）。
- `server/acceptance/test_261023_agents_md_static_semantics_alignment.acceptance.py`：新增（A1/A2/A3 三条）。
- `delivery/taskpackages/261023-agents-md-static-semantics-alignment/**`：收尾制品（plan / test-scope.txt / verifier-evidence / review-packet / docs-delta）。
- 禁入清单核对：`git status` + `git diff --stat` 确认零触 `docs/contracts/**`、`server/deploy/manifest.yaml`、`verify-deploy.sh`、`kidpal-agent-prompt.txt`、`kidpal-dynamic-context.v1.txt`、`server/gateway/**`、`firmware/**`、任何既有 `server/acceptance/` 件。

## 2. Verification evidence

### 2.1 final bearing verification（本包机器件）

- 命令：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261023-agents-md-static-semantics-alignment/test-scope.txt --change-id 261023-agents-md-static-semantics-alignment --round 2 --invocation-id 7f85f3bb-fa81-4a7a-8b90-a9fca54e46b1`
- **evidence 路径**：`delivery/taskpackages/261023-agents-md-static-semantics-alignment/verifier-evidence/rounds/r2.json`
- **fact `ok: true`**，`observedGateExit: 0`，十闸全绿：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 9% 余量）/ contract-lint（仅存量 warning，与本包无关）/ acceptance-coverage / mutation-check（skip 有据）/ docs-sync / security / unit（134 passed, 6 deselected）/ acceptance（oracle-pass + executor-run 134 passed, 6 deselected）。
- round 序号说明：round 2 = 机器口径 nextRound = 完成轮数 + 1；round 1 为 scoped 相位事实（`r1.json`，同样全绿）；上一轮 final 因序号参数口径 blocked 未落 evidence、不占序号。
- checker：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261023-agents-md-static-semantics-alignment --phase final --round 2 --invocation-id 7f85f3bb-fa81-4a7a-8b90-a9fca54e46b1` → `{"ok":true,"gate":"verifier-evidence","evidenceRef":".../r2.json","factOk":true,"observedGateExit":0,"phase":"final","round":2}`，exit 0。

### 2.2 两文件 diff 逐条对账（diff 已逐条过目）

**AGENTS.md（对 plan §2.1 (1)–(18)）**

| # | 条目 | diff 对账 |
|---|---|---|
| (1) | N1 · L1 标题去「（仓内镜像）」 | ✓ 整行替换，与 plan 逐字一致 |
| (2) | N3 · 查字行去重 `（详见「查字」节）` | ✓ 行内子串替换；绝对禁工具 / 最小意图块 / `meaning` / 坐标 / 静态字库锚组全保 |
| (3) | B1 · 课表问答行整行替换（摘要优先 + 三触发降级 + 反叙述尾句） | ✓ 逐字一致；识别特征列逐字未动 |
| (4) | S1 · 提醒 cron 行收编兜底 | ✓ 逐字一致；备注 / 语义 / entry_key / 不算时刻四锚组全在 |
| (5) | N5 · 边界行收指针 | ✓ 与 plan 逐字一致 |
| (6) | N4 · 「翻屏由问答框滚动承载」 | ✓ 行内子串替换 |
| (7) | B1+S4+S3-半句 · 课表节首段整行替换 | ✓ 逐字一致；含「（或仅语音）」删除 + 备注 read-back 以孩子日程为界句 |
| (8) | N2 · 删 `（notes ⑦）` | ✓ |
| (9) | B2 · 故事整理行（你不产标题、不产正文、不产 id / 不是逐字转写） | ✓ 逐字一致 |
| (10) | B2 · 回退四条 → 单条意图级 | ✓ 逐字一致；「讲错啦」原词保留 |
| (11) | S1 · 提醒节整节收编（标题 + 正文） | ✓ 单源指针 + 兜底一句，字段级细节外移 |
| (12) | B1 · 硬约束行同步（先看摘要、摘要没覆盖再读） | ✓ 逐字一致 |
| (13) | S3 · 对象锁定句锚串后追加 | ✓ K3 锚串（含右括号）逐字在场，仅其后以「——」追加；追加句以课表 / 提醒备注原文所载为限 |
| (14) | B1 · 底线第一不变量（摘要 ∪ 本回合读到，二者同源派生） | ✓ 逐字一致 |
| (15) | N6 · 自声明去「主承载与」 | ✓「静态语义」「单一事实源」「以本文件为准」三锚全在 |
| (16) | S5 · 记忆节标题去「（v12）」 | ✓ 新标题「## 记忆守则：孩子世界的事实要记，家长隐私不记」 |
| (17) | B3+S6 · 记忆首段（平台自动承载 / 不写文件 / 不调工具 / 记不清 / 绝不编造） | ✓ 逐字一致 |
| (18) | S5+S6 · 事实条删「照记、主动记」与 rationale 句 | ✓；L101 家长隐私条零触碰（diff 末段可见原样） |

**SKILL.md（对 plan §2.2 (1)–(6)）**

| # | 条目 | diff 对账 |
|---|---|---|
| 1 | 第 3 行 description 摘要优先化 | ✓ 逐字一致 |
| 2 | 第 8 行首段（单源派生 + 窗口 + 降级三触发） | ✓ 逐字一致 |
| 3 | 第 10 行「上下文残影」≠ 摘要 + 2026-10-23 起摘要优先 | ✓ 逐字一致；反编造覆盖句保留 |
| 4 | 第 14 行步骤 1 先判摘要覆盖 | ✓ 逐字一致 |
| 5 | 第 15 行行首「（窗口外路径）」限定 | ✓ |
| 6 | 第 40 行边界条追加窗口外适用注记 | ✓ 逐字一致 |

步骤 2/3/4 与「验证」节零触碰 ✓；`openclaw.json` / manifest 零触 ✓。

### 2.3 红→绿证据链

- **新件设计态红 3/3**：acceptance-author 冻结相位实跑（2026-10-23，记于 taskpackage.json `redBaselineResult`）——A1/A2/A3 三条全 failed，与 plan §10 预言一致；全量含新件 = 3 failed + 313 passed + 6 deselected + 33 subtests（3 failed 恰为新件自身）。
- **新件转绿（本包复跑第一手）**：`pytest -q --import-mode=importlib -m "not live" acceptance/test_261023_agents_md_static_semantics_alignment.acceptance.py -v` → **3 passed**。
- **六件读数冻结件（本包复跑）**：K1 / M3K3 / K3 / v7 / M4K3 / perf-k1 六件合跑 → **101 passed / 4 deselected**，与基线逐件一致、零新增红。
- **全量 acceptance（本包复跑）**：`pytest -q --import-mode=importlib -m "not live" acceptance -rs` → **316 passed / 6 deselected / 33 subtests passed**（= 基线 313 + 新件 3，零 failure，不降于 plan §9 期望 ≥313）。
- **diff 侧判法③**：`git diff` 不含任何既有 `server/acceptance/` 件与 `server/deploy/{manifest.yaml,verify-deploy.sh,provider.config.yaml,OPENCLAW_SYSTEM_PROMPT_CARRIER}`（实测仅两 prompt 文件被改 + 新件 untracked）。

## 3. plan §3 冻结锚保护清单逐条对账

验证方式：六件读数冻结件合跑 101 passed / 4 deselected = 各锚的机器载体全绿（锚断言即在这些件内）；辅以 diff 逐条过目。

| 冻结件 | 锚 | 状态 | 验证方式 |
|---|---|---|---|
| K1 | `K1_ROUTING_ROWS["课表问答回合"]` 八锚（先读 / schedule.md / 读不到\|查不到 / 诚实 / 全称量词 / 后天 / 大后天 / 备注） | ✓ 全在（「先读…再作答」保留在降级分支；「都拿不到就诚实说现在查不到」；「答复含备注读回」） | diff + K1 件绿 |
| K1 | 课表行时间面 ≥3/6、周X\|星期X 正则、主题面 ≥5/6、反叙述 ≥2、语音半 + 禁止半 | ✓ 识别特征列与反叙述尾句逐字未改 | diff + K1 件绿 |
| K1 | `K1_ROUTING_ROWS["提醒 cron 回合"]` 四锚（备注 / 语义 / entry_key / 不算时刻） | ✓ 全在 | diff + K1 件绿 |
| K1 | 正文级 display / 白名单 / story_list / story_text / 帮我记下来 / 问答框 / 薄载荷 / 机械计算 + 禁产 story JSON + 「12 字」 | ✓ 零触碰（故事节只改两行） | diff + K1 件绿 |
| K1 | 自声明「静态语义」「单一事实源」「以本文件为准」 | ✓ 三者全在（只删「主承载与」） | diff + K1 件绿 |
| K1 | K1-E6：verify-deploy.sh 零特例、manifest 条目不动 | ✓ 本包零触 manifest / verify 脚本 | git status |
| K3 | heading 正则 `^##\s+[^\n]*记忆` | ✓「## 记忆守则：…」命中 | K3 件绿 |
| K3 | 节内 needles（孩子世界的事实 / 宠物 / 同学 / 家长隐私 / 家庭安排） | ✓ 全在节内；家庭安排在 L101 逐字不动 | diff + K3 件绿 |
| K3 | 极性组（必须记\|要记\|… × 不记\|不进\|不复述） | ✓「必须记」「都会被自动记进记忆画像」「不记」「不复述」在场；删「照记、主动记」不构成极性缺失 | diff + K3 件绿 |
| K3 | `K3_P1_AGENTS_ANCHORS` 六串逐字（含隐私锚串含右括号） | ✓ 全部零改；S3 只在锚串后以「——」追加 | diff + K3 件绿 |
| K3 | `assertNotIn("次要承载")` | ✓ 新文本零命中 | K3 件绿 |
| K3 | 动态块负锚 | ✓ 动态 context 两文件零触碰 | git status |
| M3K3 | 课表节窗口七锚（问答框 / 不切 / 备注 / 答复\|读回 / 帮我显示课表 / 薄载荷\|{} / 不生成\|网关\|机械计算） | ✓ 全命中 | M3K3 件绿 |
| M3K3 | 旧 `{title,items}` 行级负锚 | ✓ 未引入 | M3K3 件绿 |
| v7 | PN-A4 反叙述（先读 / schedule.md / 类别 ≥2 / 语音 / 禁止） | ✓ 四词（回合判定 / 文件路径 / 流程步骤 / 内部动作）在场 | v7 件绿 |
| v7 | PV-A3 扩列 + RF-A4 | ✓ 识别特征列逐字不动；RF-A4 零触碰 | v7 件绿 |
| M4K3 | 故事节窗口（我要讲故事 / 帮我记下来 / 明天带跳绳 / story_list / 禁产 / 「12 字」字面） | ✓ 全保；新行写「标题（≤12 字）」，阿拉伯数字 + 空格保留 | diff + M4K3 件绿 |
| perf-k1 | AGENTS.md 在场非空 + sha 一致（随 md5 纪律，铺放后核） | ✓ 内容级无锚，六件合跑含 perf 件绿 | perf 件绿 |

归档件说明：`test_story_p3` 在 `archive/` 退役面，非现役红；本包保留「讲错啦」原词。✓ 与 plan §3 归档说明一致。

## 4. B2 对账节（「讲错啦」零文本检出 + 丢弃链载体）

builder 静态对账结论经本包独立复核确认（grep + 读码，2026-10-23）：

- **零文本检出**：`server/gateway/front/front_app.py`、`server/gateway/story.py`、`firmware/main/**` 对「讲错啦 / 不对不对 / 我重说」**零代码命中**——全仓 grep 唯一命中为 `firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md:140`（red-baseline 历史 md 文档行，非代码、非现役断言）。
- **丢弃链三处载体**（真实机制 = 设备取消键 → 契约 §四 `kidpal_story_discard` → front cancel）：
  - 契约：`docs/contracts/interface-contract.md:279` 一带 §四 `kidpal_story_discard` 条目（单向通知、网关按本连接 session_id 权威取消、幂等 no-op）。
  - 固件：`firmware/main/protocols/protocol.cc:116-134`（拼串形态注释 + 发送时机 = DISCARD_RECORD 内 CloseAudioChannel 之前）+ `firmware/main/protocols/protocol.h:94`（声明）+ `firmware/main/mcp_server.cc:751`（DISCARD_RECORD case）。
  - 网关：`server/gateway/front/front_app.py:647-659`（`kidpal_story_discard` 消费分支：连接权威 session_id、载荷 sid 仅审计；`_get_story_acc().cancel(session_id, reason="discard")` 在 **:653**；异常吞掉 = 幂等 no-op 语义）。
- **对 spec 的影响**：requirements S-F2.2 AC4「或说『讲错啦』，Then 取消」的语音路径在当前链路无文本检出载体——**spec↔实现既有缺口**，非本包引入、不能由 prompt 修复。
- **本包处置**：AGENTS.md 保留「讲错啦」原词（与 AC4 对齐），机制归属改为「丢弃由板子的取消键和网关的丢弃链承载」，不承诺模型执行动作。
- **debt 候选申报（登记动作归主线程）**：建议 `tech` 域，标题指向「S-F2.2 AC4 语音丢弃路径无文本检出载体」。本包不登记（plan §5 划归主线程）。

## 5. 范围圈定（test-scope）

- **清单落盘路径**：`delivery/taskpackages/261023-agents-md-static-semantics-alignment/test-scope.txt`（10 件 = 种子 7 ∪ blast radius 映射 3，含安全余量注记）。
- **圈定方式**：detect_changes（project `C-AI-KidPal`，inbound，depth=2）图可用、无降级；changed_files 仅 .codebase-memory 快照 2 件 + 两份 prompt 文本 / 新验收件（非符号承载面）→ 代码符号 impacted_total = 0；以 TaskPackage impactedTests 种子 + acceptance 全目录 grep 两份 prompt 资产引用合成清单。final verify 以 `--scope-file` 消费该清单（fail-closed 口径，scope-file 在场）。

## 6. Five-axis semantic review

- **安全**：S3 追加句以 K3 锚串逐字保留为前提、追加范围以备注原文所载为限，家长隐私条（L101）零触碰；「数据只读」底线句（原 L85）未动（S2 纵深冗余侧保留）。无新增攻击面（纯 prompt 文本）。**无 BLOCKING**。
- **契约**：`docs/contracts/**` 零触；四分类裁决 `NO_CONTRACT_CHANGE` 在案（plan §6）。**无 BLOCKING**。
- **边界**：改面 = 两 prompt 文本 + 新验收件，全在 changeManifest 内；禁入清单零触碰。**无 BLOCKING**。
- **可运维性**：AGENTS.md md5 已知窗口纪律在案（见 §8 部署注记）；verify-deploy.sh 未弱化、无单文件特例。**无 BLOCKING**。
- **结构闸**：final r2 十闸全绿 + checker PASS，不复述为 finding；结构闸之外本节五轴语义 review 独立执行完毕。

### Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | spec 对账 | S-F2.2 AC4「讲错啦」语音丢弃路径无文本检出载体（spec↔实现既有缺口，本包只留痕不修复） | changed-code hunk = AGENTS.md 回退行（「丢弃由板子的取消键和网关的丢弃链承载」句）；heuristic/tag：spec-implementation-gap / 文本检出面 grep 零命中实测（§4） | 主线程登记 tech 域 debt 候选（标题见 §4 末条），后续批次裁改 spec 或补检出载体 |
| NIT | judgement-call | 文档 | plan §7.6 预估合并后 94 行 vs 实测 wc -l 93 行（行尾计数口径差异，指向仍成立） | changed-code hunk = AGENTS.md 末行；heuristic/tag：line-count-drift / 无锚影响 | 无需动作，主线程按指路理解即可 |

无 hard-violation finding。S2（工具面收窄）/ S7（占位外观残留）按 human 裁决不入本包、已归 debt（`TECH-261023-openclaw-tool-face-prompt-only-readonly.md`）。

## 7. Simplification debt

- 全包 grep `minimal:` 标记：零命中 → 简化债务台账为空，无升级触发，全部条目 `no-trigger`。
- 本包本身即一次简化（prompt 102 → 93 行，删旧管道 / 双源 / 对偶词）。

## 8. Residual risk 与部署注记

- **md5 已知窗口**：AGENTS.md 与 SKILL.md 在 `manifest.yaml` 均为 `verify: md5` 条目；仓内镜像改后、live 铺放前 `verify-deploy.sh` 报 md5 漂移属**预期已知窗口**（K1-E6 判红豁免在案），不得弱化脚本或加单文件特例。**铺放 = merge 后 human 部署轮由 `deploy.sh` 同批铺放两份**，铺放后两 md5 转绿。
- **收口态**：本包到 live 门前停等，**`awaiting-human:live-deploy`**——与 K3 部署轮合流；seeded 负向探针测最终措辞（plan §6 live 判读注记：探针按记忆侧口径判、探针语料与真 schedule.md 备注无同串核对，落 human 部署轮 checklist）。
- **live 轮 K3 探针注意**（plan §6 附带风险，人类判断点）：S3「原词读回」若探针句与 schedule 备注家庭安排串重合可能误判命中——已在 plan §6 与本节双留痕，交 human 部署轮。
- **decisionCoverage 风险接受条目**：本 taskpackage.json 无 `decisionCoverage` 字段 → 无 `landsIn: dropped` / `risk-accepted` 条目可列（如实记录，非遗漏）。

## 9. UI design-review 证据

不适用：本包零 UI/前端面（纯 prompt 文本 + 验收件）。

## 10. Docs-sync

`docsSyncRequired = false`，零 canonical docs 改动、零 spec 漂移；依据见 `docs-delta.md`（plan §1 权威对账 + final r2 docs-sync 闸 PASS）。spec 面 `docs/requirements.md` / `docs/architecture.md` 零改动，无 architect 移交项。

## 11. 架构 / 依赖卫生

零新增依赖、零跨层调用、零循环依赖、零绕过 contracts 的本地类型（改动面为 prompt 文本，无代码符号；detect_changes impacted_total = 0 旁证）。重复实现检查：S1 提醒节收编为单源指针恰是**消除**一处双源漂移。

## 12. Closeout 与集成状态

- 制品齐：plan.md / test-scope.txt / verifier-evidence/rounds/{r1,r2}.json（r2 = final bearing fact，`ok:true`）/ docs-delta.md / review-packet.md（本件）/ pod-events.jsonl（advisory 在场）。
- `verdict.json` / `verification.json` 投影件由 verify.mjs / closeout 脚本程序产出，本包只消费不重跑。
- post-cutoff 口径：不产 session-digest / loop-health / loop-metrics.json / pod-journal 晋升。
- ADR：`adrRequired=false`，无架构裁决，不归档 ADR。
- commit：`dev/pod-m2` 分支上 commit 冻结 diff（不 push、不 merge）；治理账本面脏文件（`docs/debt/**`、`delivery/taskpackages/current/**` 等，会话起始即在）不混入本包 commit。
- 回滚方式：单 commit revert 即可（纯文本面，无迁移、无数据）。
- 下游：交 planner 跑 supervisor-verdict-gate 机械闸；不自行判定通过，不执行 merge。
