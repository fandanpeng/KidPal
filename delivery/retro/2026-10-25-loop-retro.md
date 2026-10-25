# loop-retro · 2026-10-25（Dev Loop OS 周期复盘 · 第 5 轮）

> 范围：`pendingAnalysis` 25 个 new-format 包全析出（261013 批 3 + 261014 批 2 + 261015 批 3 + 261016 批 2 + 261017 批 1 + 261018 批 2 + 261019 批 1 + 261020 批 1 + 261021 批 1 + 261022 批 7 + 261023 批 2 + 260920 批 1）。游标由 `.claude/loop-retro/ledger.jsonl` 判定；本报告只读证据、不重算 verifier/supervisor 事实。
> 机器侧：25 包 `pod-events-aggregate.mjs` 全量聚合；散文侧：22 份 pod-retro 逐份通读。缺 pod-retro 3 包（`260920-gov-review-dispatch-telemetry-source`、`261013-qq-schedule-write-closure`、`261022-diag-log-content`），按设计只 advisory、不阻复盘。
> 另 7 个 waiting（旧格式、无 loop-health.md）见 §七。

## 一、机器聚合总览

- **token coverage**：`{dispatchesTotal: 111, dispatchesWithTokenData: 111, dispatchesUnavailable: 0}`，exact token 合计 **18,834,173**。这是本仓首次出现整轮 100% 覆盖，transcript 通道稳定。
- **friction**：58 条。按类型：`over-scan` 30、`degraded` 14、`unknown` 7、`scan-miss` 5、`stall` 1、`scope-drift` 1；malformed payload 7 条（其中 `261023-agents-md-static-semantics-alignment` 四条的 payload 全空，语义不可恢复）。
- **escalation**：20 条。按 category：`other` 8、`scope-or-permission` 4、`environment` 3、`gate-tooling-bug` 2、`contract-or-spec` 2、`ambiguous-acceptance` 1。机器判 unresolved 11 条，但其中多条已由 human-decision 语义闭合（同 ARQ-261014 已记的闭合链缺口），本轮不按机器数下结论。
- **模型分布**：`glm-5-3-flash-260828` 56、`deepseek-flash` 25、`deepseek-v4-1-flash-261017` 17、`deepseek-v4-1-flash-260910` 9、`deepseek-v4-pro` 2、`kimi-k3` 1、`<synthetic>` 1。模型换代与混用并存，再次印证跨月效率比较必须按能力纪元分层（见正在等待 human 立项 ack 的 `261024-gov-review-retro-efficiency-instrumentation`）。
- **正向信号**：多包零 fixer 轮（261016-p3-story-body 一轮 fixer 是抓 BLOCKING 后主动补齿、261018-sd-log-usb-read 零 fixer、261022-m5-k2b 零绕轮、261022-m5-n2a 零 fixer）；`over-scan` 申报纪律持续良好，30 条中绝大多数是「断言对象不在 input-brief 清单」的结构性必要读，属预期噪声；真机轮的诚实豁免形态（261016-sd-card 的「代码面静态断言，非运行时观察」、261018-usb-read 的 8 PASS + 1 NOT-EXECUTABLE）值得保留。

## 二、跨包共性主题

### T1 · final 相位派发契约未成文——scope-file 归属与 round 序数八包复现

- **证据**：261013-lang-config（卡点 5：final 的 scope-file 前置没进派单模板）、261014-engine-stop（final r4 缺 `--scope-file` 举手；ordinal「失败未落 fact 不占序」靠试错）、261018-sd-log-usb-read（final 相位 scope-file 依赖被低估，流序 = scoped 绿 → integrator 圈定 → final）、261019-p3-record-dialogue-guide（同一 fail-closed 连续第二轮发挥作用）、261020-p3-rec-frame-visual（第一次撞、靠闸教学）、261022-m5-k1（final 轮缺 scope-file）、261022-p3-record-chrome-ghost-fix（final round 必须是 next completed ordinal，派单写 1 被拒）、261023-agents-md（沿用上一包 r3 先例、本包 nextRound=2）。另有 261014-voice-round 记「补轮重跑 supervisor 闸前必须先删旧 write-once verdict，报错不自解释」。
- **根因**：final 相位的三个契约（谁跑、scope-file 谁产、round 序数怎么算）只活在 `verify.mjs` 的 fail-closed 行为和个别先例里，没有一处成文进 planner 派单模板或 launch card。
- **处置**：同根已在册——`ARQ-261014-merge-gate-tooling-hotfix-batch` 的附带教育面 ④ 已点名「final 相位裸跑被 fail-closed 举手 / 正确序列是 scoped → integrator 圈定 → final」。按 GC1 追加证据、不新立；**建议升该条严重度并优先处置**（本波八包、每包至少烧半轮到一轮）。

### T2 · 起包/收口簿记形状税第五波——新子项继续变形

- **证据**（跨 12 包以上）：
  - coverage 标记：261013-lang-config（为过闸发明「coverage: LG-A1 (T1)marker」胶水词）、261023-honcho（本地 ID 放第二位被判缺，须置首）、261016-sd-card（条目内嵌 ASCII 冒号致 `shortAcceptanceRef` 截断，同族脆弱形态）。
  - acceptance id 约定：261022-m5-k4（随手用 `K4-A1..`，家族既有约定是 E-id，六个 id 全判缺覆盖）。
  - docRefs 锚句：261019（docRefs 断言提及需 docsNoChangeReason）、261022-p3-chrome-ghost（docRefs 是 docs-sync 锚面，plan 写于冻结齿之前无 CG 锚，多烧一轮）。
  - securityEvidence 骨架：261019（finalize 留空）、261021（r2 才补）、261022-m5-k3（拆包模板没列该字段）、261022-m5-k4（r2 才补）。
  - mutationEvidence raw-id：261015-p1-auxbar（缩写占位）、261015-voice-rightcol（缺 perAcceptance/structural 标注）、261018-sd-log-persist（短 id）、261016-sd-card（指针形态失配）、261019（全 id 子串匹配）。
  - 数字口径：261022-m5-k3（redBaseline 目标数 296 vs 313）、261023-honcho（全量数含不含被改件）。
  - TaskPackage 原子写：261013-lang-config（heredoc 直写写成 1 字节，靠 current 投影救回）。
- **根因**：与 `ARQ-260907-taskpackage-format-expectations-frontload` 完全同根——闸的提取器语义只在源码里，起包骨架靠闸红反推。本波已是第五波，且新子项（coverage 置首、id 家族约定、docRefs 锚句、securityEvidence 骨架、红基线口径）继续变形。
- **处置**：GC1 追加证据到该条（已归档、closed），**建议升 priority**；不新立。

### T3 · intake/1g 不扫相邻冻结件字面锚——跨包冻结冲突靠 builder 首轮撞出

- **证据**：261018-sd-log-persist（intake 只核本包 manifest 与 scope-map，未扫既有冻结件；`sd_card_mount.acceptance.c` 钉 `max_files=5`、本包要 8，builder 首轮 host 全量才暴露，需 human 窄口径授权重锚；retro 原话「intake 缺少一个『相邻冻结齿字面锚』的扫查动作」）；261022-m5-k4（1f 逐字段对磁盘核验才发现 ADR-016 已改写措辞，避免照旧口径实现）；261016-p3-story-body（覆盖层 chrome 面不在任何冻结齿里，靠 integrator 语义 review 抓住 BLOCKING）；261018-sd-log-usb-read（消化了 persist 教训、起包前扫查，零 fixer）。
- **根因**：intake 的检查集是「本包 changeManifest + scope-map」，不含「既有冻结件里钉住同一实现点的字面锚」；跨包冲突因此天然延迟到 builder/verifier 才暴露。
- **处置**：查无同根 → **新立 ARQ-261025-adjacent-frozen-anchor-sweep**。

### T4 · 同分支/同 worktree 多活跃 Pod——包级 diff、current 指针与合并圈定互相污染

- **证据**：261018-sd-log-persist（两包共用 `dev/pod-m2`，merge-driver 是整分支语义，只能临时分支 cherry-pick 本包提交；retro 原话「要么 launch 面禁止同分支双活跃 Pod，要么 merge-driver 增加按提交范围圈定」）；261018-sd-log-usb-read（persist 收口会话在共享分支落 15 个提交、current 指针被翻走；`git rebase` 在 settings 的 ask 列，headless 会话弹窗未达 human 即自动拒绝）；261022-p3-record-front-polish（`detect_changes` 先跑 base=main 圈进两个并联固件包符号，切到包首提交父才对）；261023-honcho（并行会话在 same worktree 出现面 C 的 PROPOSAL 与 spec 改动，靠 integrator stat 核对防线）；261013/261014 的 current 指针漂移。
- **根因**：共享 worktree/分支是常态，但「包级 diff 的 base」与「current 指针归属」都没有机械锚——每包现场自选基准，错了就静默污染。
- **处置**：查无同根（`ARQ-260905-merge-driver-worktree-topology` 已于 2026-09-06 结项、治的是 fetch/checkout 拓扑）→ **新立 ARQ-261025-shared-branch-pod-isolation**；指针面证据按 GC1 追加到 `ARQ-260913-current-pointer-implicit-global`。

### T5 · brief-inputs 锚点解析噪音——整文件退化与裸 token 误报第四包

- **证据**：261013-lang-config（brief 清单退化整文件引用，诱发 acceptance-author 超读 10 行）、261022-m5-k3（裸 token ref `NFR-PRIV-1`/`TECH-*` 报「文件不在磁盘」连续第三包）、261023-agents-md（8 个 refs 全报无锚点，本包 refs 天然是文档面多锚）、261023-honcho（5 条 ref 无锚点 advisory）。
- **根因**：`brief-inputs.mjs` 的锚点解析只认文件内 anchor，文档类 ref 与裸 token ref 都退化成 advisory；噪声稳定存在，会稀释真告警的可信度。
- **处置**：查无同根 → **新立 ARQ-261025-brief-inputs-anchor-noise**（severity COULD）。

### T6 · 等待态/board 状态闭集缺「等人操作」档

- **证据**：261016-sd-card（launch-card 写 `awaiting-human:real-device`，`portfolio-reconcile.mjs` 的 `POD_STATUSES` 闭集无该值，`--set-pod` fail-loud 拒收，只能降级为 `blocked` + 事件承载精确语义；retro 明确「建议留给治理面」）；261021-p3-record-cancel-lifecycle（`awaiting-human:merge-gate` / `awaiting-human:real-device` 同形态）。
- **根因**：状态闭集只有调度态，没有「等人持板/等人批合并」的一等语义；精确停等只能塞进 pod-events。
- **处置**：查无同根 → **新立 ARQ-261025-wait-state-closed-set**（severity COULD）。

### T7 · 中断/配额恢复没有落盘约定——agent 被 429 打断后无回执

- **证据**：261019-p3-record-dialogue-guide（上会话 HTTP429 中断在验收冻结收尾，产物在盘未 commit，恢复靠核验 current 指针与 352 字节级刷新）；261022-m5-k3（acceptance-author 冻结主体已落盘、收口回填未做，429 后 agent 不留任何回执，只能靠 `git status` 考古；retro 建议「派发模板约定中断前先落进度注记」）。
- **根因**：派发契约只覆盖正常返回，不覆盖中断；429/配额是环境常态，恢复成本每次都重新付。
- **处置**：查无同根 → **新立 ARQ-261025-interrupted-dispatch-progress-note**（severity COULD）。

### T8 · debt 结项归档与引用更新不同批——docRefs/sourceRefs 静默陈旧

- **证据**：261013-lang-config（integrator 把 TECH 条目 `crq-render` 归档进 `_archive/2026/`，sourceRefs/docRefs 立刻陈旧，final r4 docs-sync 红才暴露）；261017-prompt-v11（同型：integrator 先翻 closed、`debt-render` 自动归位，TaskPackage docRefs 仍指旧顶层路径，final 红）；261022-p3-chrome-ghost（docRefs 锚句起包时未回填，r2 红）。
- **根因**：归档归位是渲染器的自动副作用，引用更新是起包时的手写事实，两者没有同批绑定。
- **处置**：查无同根（`ARQ-260929-docs-sync-spec-handoff` 治的是 spec 写权、非归档搬迁）→ **新立 ARQ-261025-debt-archive-ref-atomic**（severity SHOULD）。

### T9 · intake INDEX 漂移标与 retro 游标的 `current` 假阳性

- **证据**：`delivery/intake/INDEX.md` 现存三处漂移标——`261022-device-memory-prompt-overhaul`（⚠未闭环，in-flight 4/4）、`261021-p3-record-cancel-lifecycle`（⚠值域外，触面/status/anchors 全 `—`）、`261014-single-tier-bringlist-home`（⚠未闭环，awaiting-human:立项-ack 4/4）。按 retro 步骤 5 的 T5 复议触发点，仍见漂移 → 需升级机械闸另立批。另：`loop-health-report.mjs --pending` 的 `waitingOnLoopHealth` 连续多轮包含 `current`（指针目录，不是 changeId），是游标面的假阳性。
- **根因**：漂移标是「检测不拦截」设计，但没有消费方把它变成动作；游标脚本未排除 `current` 投影目录。
- **处置**：查无同根 → **新立 ARQ-261025-intake-index-retro-scope-hygiene**（severity COULD）。

### T10 · 收口义务写权真空复发——设计稿母版 / docs 面 / taskpackage 元数据

- **证据**：261015-p1-auxbar（planner Pod 会话可派集无 `uiux-author`，真母版 `docs/ui` 内容级同步无人可写，降级并入真机轮 human 门）；261016-sd-card（`docs/notes/**` 无任何 agent 的 allow 覆盖；`--agent planner` 起的 Pod 主会话受 per-agent 约束，plan 的「主线程通道代落」假设不成立）；261022-m5-k1（acceptance-author 的 scope 只许写 `server/acceptance`，taskpackage 元数据回填被拦，planner 代写多一个往返）。
- **根因**：与 `ARQ-261014-closeout-obligation-write-authority-vacuum` 同根——收口义务清单没有逐项钉承载写点，且 Pod 主会话与裸主会话在 enforce-scope 眼里不同。
- **处置**：GC1 追加证据到该条；其中「planner Pod cast 缺 uiux-author」作为新子项一并记入。

### T11 · 机械闸误升级与噪音——all-mock、sdkconfig 再生、current 投影相交

- **证据**：261023-agents-md（`all-mock` 启发式把 prompt 文本对齐包判 high → ESCALATE，retro 称「结构上内生于该包型」）；261015-voice-rightcol（`idf.py` 再生的 `firmware/sdkconfig` 空节 +5 行被 G6 重算判 critical → ESCALATE，剥离噪音 commit 后 APPROVE）；261016-sd-card / 261018-sd-log-persist（`current/` 投影与 `delivery/review/INDEX.md` 的 parallel-overlap tier3，双向 append 零语义冲突）。
- **根因**：机械闸的事件型判据不区分「行为面真变」与「生成/投影面噪音」，也不消费 launch-card 的前置申报。
- **处置**：all-mock 与 sdkconfig 噪音按 GC1 追加到 `ARQ-261014-supervisor-escalate-preflight-ignored`；INDEX 生成件相撞追加到 `ARQ-261014-merge-gate-tooling-hotfix-batch` ③。

### T12 · 「可见面」验收轴仍无固化机制

- **证据**：261016-p3-story-body（行为对了不等于界面对了：底栏键帽槽文案、页点、story_page 自画键帽被遮挡，全部不在冻结齿里，靠 integrator 五轴语义 review 抓住 BLOCKING；retro 原话「『可见面』是一类独立的验收轴……这不是可复制的机制」）；261016-sd-card（诚实豁免形态把「没做」写进纸面）。
- **根因**：验收冻结只钉行为/结构，不钉新态的 chrome 三件（键帽槽 / 页点 / 主标题画法）；是否挂真机目检也没有二选一申报。
- **处置**：与 `ARQ-261001-host-blind-outcome-device-gate` 同族（用户可见、host 不可编的结果面），按 GC1 追加证据，不新立。

### T13 · 遥测保真与 GC3 覆盖缺口继续

- **证据**：261019-p3-record-dialogue-guide（dispatch-stamp 报的实际生效模型与派发指定不符，cc-switch 代理改写）；261022-m5-k3（transcript 派生模型显示与当前会话不同）；以及 §四的覆盖比对结果。
- **处置**：GC1 追加到 `ARQ-261014-pod-events-booking-telemetry-fidelity`。

## 三、GC1 · findings 对账 canonical / 已裁决项

对账集 = `docs/debt/gov/` 在册活条目 + `_archive/2026/` 直接相关条目 + 现行 canonical（OS §5c/§5d、`planner.md`、`dev-loop.md`、`task-intake-and-slicing.md`）。结果：

- **7 条与已裁决项同根，记复现/追加、不新立**：T1→ARQ-261014-merge-gate-tooling-hotfix-batch④；T2→ARQ-260907-taskpackage-format-expectations-frontload（复现五）；T4 指针面→ARQ-260913-current-pointer-implicit-global；T10→ARQ-261014-closeout-obligation-write-authority-vacuum；T11→ARQ-261014-supervisor-escalate-preflight-ignored + ARQ-261014-merge-gate-tooling-hotfix-batch③；T12→ARQ-261001-host-blind-outcome-device-gate；T13→ARQ-261014-pod-events-booking-telemetry-fidelity。
- **0 条与现行 canonical 冲突**：本轮未出现「建议补建已退役 def」类漂移；T2 的五条新子项均为既有条目的形态变体，不构成新根。
- **0 条需要升级 human 的方向级问题**：全部为流程/工具面，方向已在既有裁定内。

## 四、GC3 · new-format 覆盖比对（机器聚合 vs 散文主题）

25 个 new-format 包逐主题核对，**「retro 有主题、events 无对应 event」仍是系统性常态**：

- `261020-p3-rec-frame-visual`：ledger friction=0，retro 记 three themes（taskpackage-required 两轮自修、最终 scope-file 流序、爆炸半径口径）——典型 ledger=0 / retro=N。
- `261022-m5-k3-honcho-memory-production`：ledger friction=1，retro 记四条（429 中断、redBaseline 口径、securityEvidence 缺位、brief-inputs 噪音）。
- `261013-lang-config-thinking-fix`：ledger friction=4，retro 记六条卡点。
- `261016-p3-story-body-sd-cache`：ledger friction=3，retro 记五条（1g 预评、integrator BLOCKING、通道语义、误发 human-decision、UI brief 警示行）。
- `261023-agents-md-static-semantics-alignment`：ledger friction=4 但 payload 全空、语义不可恢复（malformed=4），retro 三条主题。
- 反向：`261018-sd-log-usb-read` ledger friction=0 而 retro 明确记「mutation-check 提前直跑省一轮」「final scope-file 流序」两条——机器侧零摩擦不代表过程零摩擦。
- **结论**：偏差不采信任一源；系统性缺口按 GC1 追加到 `ARQ-261014-pod-events-booking-telemetry-fidelity`，不另立。

## 五、分流清单

**治理类 → 新立 ARQ（主线程登记进 `docs/debt/gov/`）**

| 条目 | 主题 | severity | riskTier |
|---|---|---|---|
| ARQ-261025-adjacent-frozen-anchor-sweep | intake/1g 增相邻冻结件字面锚扫查 | SHOULD | low |
| ARQ-261025-shared-branch-pod-isolation | 同分支多活跃 Pod 的包级 diff / 合并圈定 / 指针 | SHOULD | low |
| ARQ-261025-debt-archive-ref-atomic | debt 归档归位与引用更新同批 | SHOULD | low |
| ARQ-261025-brief-inputs-anchor-noise | brief-inputs 锚点解析噪音与裸 token 误报 | COULD | low |
| ARQ-261025-wait-state-closed-set | board/launch-card 等待态闭集 | COULD | low |
| ARQ-261025-interrupted-dispatch-progress-note | 中断/配额恢复的进度注记约定 | COULD | low |
| ARQ-261025-intake-index-retro-scope-hygiene | intake INDEX 漂移消费 + retro 游标排除 current | COULD | low |

**治理类 → 追加证据（不新立）**

| 条目 | 追加内容 |
|---|---|
| ARQ-261014-merge-gate-tooling-hotfix-batch | T1 八包 final 相位证据；T11 INDEX 生成件相撞新时点 |
| ARQ-260907-taskpackage-format-expectations-frontload | T2 第五波五条新子项 |
| ARQ-260913-current-pointer-implicit-global | T4 指针漂移四包证据 |
| ARQ-261014-closeout-obligation-write-authority-vacuum | T10 三包证据（含 planner cast 缺 uiux-author） |
| ARQ-261014-supervisor-escalate-preflight-ignored | T11 all-mock 与 sdkconfig 噪音证据 |
| ARQ-261001-host-blind-outcome-device-gate | T12 可见面 chrome 证据 |
| ARQ-261014-pod-events-booking-telemetry-fidelity | T13 模型失配 + §四 GC3 覆盖偏差 |

**实现类 → 正常 dev-loop**：无确定的 `firmware/**` 缺陷待修；真机轮发现的缺陷均已在原 Pod 内由 fixer 收口。

**信息类 / 接受风险**：over-scan 良性噪声（30 条，结构性必要读，不加收紧动作）；sonnet 经 cc-switch 代理 400 → 改 fable 通路（环境侧降级，非选型）；部署后核对实际加载版（261019/261021 两条教训，owner dev-loop，不进 ARQ）；真机轮诚实豁免形态（正向，保留）。

## 六、waiting 清单（不动）

`260829-audio-bargein-hw`、`260829-shim-skeleton`、`260831-cloud-link-bringup`、`260831-simulator-lvgl-align`、`260913-happy-flow-host-capture`、`260924-ops-openclaw-family-user-mapping`、`current`。

前六个为旧格式（pre-cutoff）且 `loop-health.md` 缺失，连续多轮 waiting；若 human 裁定其结构性永不析出，可走 human-only 销账（`--writeoff`）。`current` 为指针目录，属游标假阳性，已入 ARQ-261025-intake-index-retro-scope-hygiene。

## 七、下一周期关注点

1. **T1 与 T2 是本波最大的两块时间税**：final 相位契约八包、簿记形状税十二包以上；两者都有在册条目、建议下一治理批优先处置，不再只追加证据。
2. **T3/T4 是新的结构性根**：相邻冻结锚与同分支多 Pod 都会把冲突推迟到最贵的相位才暴露，建议紧随 T1/T2 之后的治理批。
3. **能力纪元分层**：本波模型混用（7 种取值）再次证明跨月比较不可混杂；`261024-gov-review-retro-efficiency-instrumentation` 正在等待立项 ack。
4. **token coverage 首次 100%**：后续若再出现 unavailable，优先查 transcript 通道与 dispatch-stamp 调用侧旗标。
