# pending-human-ruling · 261116-channel-resident-core

> 非交互收尾的 fail-closed 载体（planner 收尾相位，2026-11-18 轮 C 修复轮更新）。本包**每轮修复的离线面都已完成并 commit**，停在合并门；以下事项需 human 裁决/执行，未闭合前不推进、不合并、不结项。

## 当前状态（已完成、可复核）

- **轮 C 修复轮离线面已全部完成并 commit**（2026-11-18，本地 `dev/pool-channel`，**未 push、未 merge**）：
  - `b0c0d9a4` 主 commit（28 文件 / +2413 −305）：front 段级静音幻觉过滤 + 整理产物占位守卫（R1–R4）+ reason 分档（静默释放 vs 丢稿安抚）+ 冻结验收 SCRF-C1..C4 + 归档守卫单测 additive + 变异证据 + 收口制品。
  - `49054c84` 簿记（pod-closed + detect_changes 收口后复核数字）。
  - `b0c0d9a4` 之前：`095122d2`（轮 B 收口）→ …（轮 A/B 历史栈保留）。
- **机械证据**：`verifier-evidence/rounds/r8.json`（scoped 八闸 ok，invocationId `ffeeab66…`）、`r9.json`（final 十闸 ok，scope 圈定 `test-scope-final-r9.txt` 31 条，invocationId `9cee1805…`）；`verifier-evidence-check` 两轮均 exit 0。
- **实跑**：host `321 Tests / 3 Failures`（3 红 = 上游存量 `p1_auxbar_style`，本轮固件零改动）；server 全量 `799 passed / 0 failed / 9 deselected / 137 subtests`；final 闸内 unit 302 passed、acceptance 204 passed。
- **closeout**：`docs-delta.md`、`review-packet.md`、`pod-retro.md` 齐；`closeout-check.mjs` exit 0；`supervisor-verdict-gate.mjs` 产出 **ESCALATE**（declared riskTier high、evidenceComplete=true、bearingFact=r9）；`supervisor-gate-check.mjs` exit 0。
- **未合并、未 push**：本包出口未闭合（`device-round-evidence.md` 未生成，**真机轮 C 未跑**）。**不得合并**。

## 待裁一：真机轮 C（未跑；本轮修复的验收凭据）

轮 A = FAILED 诊断轮；轮 B = 部分通过（D1/D2/A5 通过，D3/A6 失败）→ 轮 B 修复轮；轮 C（2026-11-18 在线取证）= **D3 设备侧已修好**（60 秒 `story_empty_session_timeout` 正常触发），失败点前移到 **front 归档决策**：引擎静音幻觉 `Okay.` 经 pending_stop 迟到转写进累积器 → 整理 LLM 返回「（没有可整理的内容）」被当正文归档（`2026-11-18-s33477cee`）。本轮修复即针对该失败点。三源在线证据见 `device-evidence/round-c/`（front journal / 引擎 ASR / ECS 垃圾篇 + A5 对照篇）。

### 轮 C 复测清单（human 持板；review-packet 同步写死）

1. **D3 空素材静置 60 秒** → 不归档 / 列表不变（total 不增）/ 不盖章 / front 无 `story_text`；若引擎吐 `Okay.` 类幻觉，front 日志应见 `story_feed_guard dropped=silent_asr_hallucination`（证明段级过滤在真机生效）。
2. **A5 有素材归档正常**（讲完 → 整理 → 归档 → 盖章）。
3. **A6**：普通会话静置 60 秒 → 自动关麦，审计行 `session_idle_timeout` 在场。
4. **A7**：连续讲 >90 秒 → 不被误收。
5. **引导播报回归**：记录会话中触发卡壳引导播报（Listening→Speaking→Listening），随后静置 60 秒 → 不得被判空素材释放。
6. **追加（轮 C 修复项）整理占位回归**：构造素材但整理不可用/占位 → 不归档 + 设备能退出 ORGANIZING（页面退出帧承载）。
7. **诊断采集**：`listening_diag` 的 `suspect/quiet/asr/run/rec_ctx`；D3 窗 `rec_ctx=1 && asr_seen=0 && quiet` 到阈值；日志落盘增量对账。
8. 轮 C 通过后由 human 交付 `sourceCommit` + `sha256(xiaozhi.bin)`，planner/integrator 才能生成 `device-round-evidence.md`（含逐 acceptanceRef PASS 目击）。**未生成、未跑即不合并、不伪造 PASS。**

## 待裁二：合并门 ESCALATE + riskTier high

- `supervisor-verdict-gate.mjs` = **ESCALATE**（declared riskTier high，证据完整）。前置升级闸残项：`device-round-evidence-missing`（真机轮 C 未跑）、`riskTier=high`、`supervisor verdict=ESCALATE`、`parallel-overlap-tier3`（见待裁三）。
- human 明确批准合并后：planner 落 `human-decision`（`payload.decision="approve-escalated-merge"`）→ `merge-driver --pod <changeId> --human-approval-ref <eventId>`。**人批不豁免真机证据、不豁免 tier3。**
- **口径分法需 human 知晓（planner 裁定，已留痕）**：human 2026-11-18 原话要求占位命中「不推 story_list」。实际分两条路径：① 素材侧过滤路径（会话从未进整理回合）→ **严格零帧**，原话字面成立；② 整理回合已发生后命中占位 → 设备已在 ORGANIZING，**必须**发 `restore=False` 页面退出帧，否则困死整合页（261021 R6 实证；SAO-A1 同因重钉先例）。human 真正在意的「列表不虚增」由不归档保证。**如 human 改判为严格零帧，代价 = 设备困页复现，planner 不建议。** 详见 review-packet「下行序列与 human 原话『不推 story_list』的口径分法」节。

## 待裁三：三处工具/治理缺口（阻断合并动作）

1. **G4 verify-logs 结构性缺口**（`gate-tooling-gap-g4.md`）：全绿 Pod 无 verify-logs → 监督闸恒红；本包因既有失败轮留有 log 未触发，**不等于已修**。
2. **`parallel-overlap-tier3` 对生成件误判**（`gate-tooling-gap-overlap-index.md`）：`delivery/workstreams/INDEX.md` 实撞仅「最后更新」日期行，按 §10d 应判 ②；建议 `.gitattributes` 加 `merge=union`（+ 同批裁 `index.jsonl`）。**不在 `escalateExempt` 内**，合并前必须让冲突消失。
3. **`change-manifest` 闸经 current/ 指针恒判 pre-cutoff 放行**（`gate-tooling-gap-manifest-cutoff.md`）：本轮 planner 已手工按同规则对账（清单外 0 件），**闸本体需主线程修**。

## 待裁四：spec 漂移与上游残余（主线程/需求面，Pod 零写）

1. **spec 漂移（沿用）**：D3 收窄 requirements §2.1「记录会话不适用 60s 会话超时」的逐字冻结（空素材记录会话由设备侧 60s 收尾 = 释放不归档）；`docs/architecture.md` §5.1.1 需补「有效语音证据」定义。建议主线程按需求面/架构面轻量注记处理；**本 Pod 对 `docs/**` 零 diff**。
2. **上游残余（轮 C 新增，建议登记 tech debt）**：引擎静音 ASR 幻觉（`Okay.` / 「嗯。」族）产生在引擎 ASR provider（ECS 容器，本仓与引擎本轮均零改动）。本轮 front 只做「不因幻觉产出垃圾篇」的防御；**引擎侧幻觉本身未修**，建议主线程立项（`docs/debt/` 由主线程/需求面落，Pod 不写）。证据：`device-evidence/round-c/engine-asr-2120-2123.txt`。
3. **follow-up 候选（planner 裁定本轮不做）**：整理产物「忠实度重叠检查」（长尾无括号占位）；中文应答词（好的/知道了等）暂不入段级过滤表，先积累审计证据；档 C 日语族证据强度 = 家族模式推断。

## 非裁项（已按纪律处置，仅备案）

- 两条 debt（`TECH-261114` / `TECH-261012`）维持 `deferred`（结项凭据 = 真机轮 C）；轮 C 通过后由主线程改 `closed`/`closedBy` 并 `debt-render.mjs` 归位。
- 变异观察项（`mutation-evidence/round-c-fix.md` 三节）：R4 判序 R1→R4→R2→R3（planner 采纳）；`_load_output_guard` wanted 增列 `_FRONT_GUARD_STRIP_RE`；#8 须同移单行前置与主体守卫才转红。
- workstream `261115-session-channel-resident` 的 progress/INDEX 与 spine 回填归主线程（Pod 零写 `delivery/intake/**`）。
- `loop-retro` 触发阈值 advisory（closeout-check 提示）：属主线程 /loop-retro 责任，非本 Pod 门。