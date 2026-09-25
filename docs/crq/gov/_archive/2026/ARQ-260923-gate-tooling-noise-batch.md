---
id: ARQ-260923-gate-tooling-noise-batch
title: 治理脚本 / 钩子的「用户面」噪声与危险静默小修集合——误导提示 / 静默相位回落 / 传参形态不一 / 非闸守卫脚本漂移无主
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 已结（2026-09-25，260924-gov-review-gate-parse-teeth T1–T4 全承载）
opened: 2026-09-23
closed: "2026-09-25"
closedBy: 260924-gov-review-gate-parse-teeth
paths: []
source: loop-retro-2026-09-23（pod-retro 实证：m2-k3 钩子节、k7 钩子节、m2-k1 卡点 5、rma8 钩子节、m2-k2 卡点 5；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（loop-retro-2026-09-23，跨棒四处）：① commit-safety.mjs 在 `git add` 后弹「staged 为空——git add 可能未生效」（读的是 add 之前的快照）——m2-k2 登记一次、m2-k3 一棒三回、card-fix 两回，每回多跑一步 `git diff --cached --stat` 消化，m2-k3 retro 原话「同一条已登记两次的噪声提示，值得在下一批治理里修掉——它消耗的是每一棒 Pod 的注意力」；② verify.mjs 未知 phase 值静默回落 scoped——K7 传 `--phase=full` 静默退回 scoped、差点把 scoped 绿当 final 绿，其 retro 判「参数不认时静默降级为默认相位是危险形态」，建议未知值直接报错；③ 治理脚本传参形态不统一 + `--help` 缺失——changes-index.mjs 无 --help 分支（rma8 整合期想看用法反触发全量重建、顺带纠偏 4 行历史 INDEX）、mutation-check.mjs 位置参数且 `--change-id` 旗标被当路径（m2-k1 踩），m2-k1 retro 判「治理脚本传参形态不统一是系统性小税」；④ enforce-scope-canary 期望过期（m2-k2 四条，两条与治理变更后新增授权面相悖、两条与 docs 目录层级退役相悖）——非闸集脚本漂移后无人会被提醒复跑。**建议改动**（候选，交 review，不自批）：小修批四处各一行级——① commit-safety 提示口径改「以 `git diff --cached --stat` 为准」或读 add 后快照；② verify.mjs 未知 phase 值 fail-loud；③ changes-index / mutation-check 补 `--help` + 传参旗标对齐；④ enforce-scope-canary 挂进某复跑清单或声明退役归档。均零门判定语义变更。**riskTier: low**。owner=architect + 主线程 landing。

**批 C 起草期实测复证（2026-09-24，260924-gov-review-gate-parse-teeth 起草前）**：四处处方落点逐一复核**均仍在、未被后续批消化**——① commit-safety.mjs:60-67 `buildAdvisory` 在 PreToolUse（`git add` 执行**前**）时点读 staged 快照，:66 空态文案「staged 为空——git add 可能未生效或被静默拒绝」对 add 触发恒现身（批 C 开工会话内 add 相即再现一次噪声；commit 相同钩子输出为真快照、非噪声。头注 :11-12 已自述该时序，改法两选〔改文案口径 vs 换时点读 add 后快照〕交评审裁）。② verify.mjs:83-85 `phaseFromArgs` 只认 `'final'`、其余值一律静默回落 `'scoped'`；bearing 路径 `normalizeExpected`（verifier-evidence-check.mjs:114）已 fail-loud（`phase must be scoped or final`），**缺口仅存 legacy 路径**；调用面实扫：canonical 模板（planner.md:71 / integrator.md:38）只传 scoped/final 且恒带 `--evidence-mode=round`，**无存量调用方依赖静默回落**；K7 病例 `--phase=full` = timing 词汇（gate `timing:'full'`）误用为 phase 值；改法须保留「`--phase` 缺省 → scoped」的 fail-safe 默认（:80-81 注释成文设计）。③ changes-index.mjs:528-544 传参分支无 `--help`（else 分支直落 `main()` 全量重建，rma8 病例机制属实）+ mutation-check.mjs:13-18 只认位置参数 `argv[2]`（`--change-id` 类旗标被当路径、落 `fail('TaskPackage file does not exist')`）——两处均在。④ enforce-scope-canary 现跑 **3/7、4 FAIL**：3×「architect 写 .claude/agents/planner.md 须拦」期望过期（scope-map 现状 architect allow 含 `.claude/agents/**`、review-loop 自定义另列个别 deny）+ 1×「architect 写 docs/systems/** 须放行」期望过期（docs 平铺后 universal 层拦未登记子目录）；总数与 m2-k2 登记（四条、3/7）一致，**细分与本条转引「两条授权面 + 两条 docs 退役」有出入——现扫为 3+1，以现扫为准**；脚本不在 verify 闸集（verify-gates.json 无该闸），「漂移无主」诊断维持。

**结项（2026-09-25，260924-gov-review-gate-parse-teeth T1–T4）**：四处全承载——① T1 commit-safety 空态文案按时序分流（`statLine` 纯函数 + 3 判别 case，self-check 15/15 绿）；② T2 verify.mjs 未知 phase fail-loud（`assertPhaseArg` + `--self-check` 8/8 + live `--phase=full` exit 2 零副作用；OQ-2=(a) human 批准凭据记该批 PROPOSAL Status 凭据段）；③ T3 changes-index 补 `--help` + unknown-flag 拒绝、mutation-check `--change-id` 旗标解析 + `--help`（self-check 31 例 / 12 例全绿；rma8「想看用法反触发全量重建」与 m2-k1「旗标被当路径」两病灶闭合）；④ T4 canary 退役删除（OQ-3=(b)，非推荐位 human 明确择退役）。**覆盖缺口记录（T4(b) 义务，逐字）**：enforce-scope.mjs 本体**无 `--self-check`**（grep `self-check|selfCheck` 0 命中 + `--self-check` 实跑无输出 exit 0，2026-09-24 实测），F1/F3 行为契约（agent_type 硬门 :174 / 大小写归一 :178-180，**逻辑本体零触碰**）在 canary 退役后**零等价回归覆盖**——缺口如实记录、不掩盖；未来若需回归覆盖，路径 = 给 enforce-scope 本体补 `--self-check`（另批承接、不重开本条）。canary 活引用 census = 零（10 命中全为脚本本体/历史制品/账本/本批 PROPOSAL）。证据：本批 PROPOSAL §2 T1–T4 + §5 V1–V4 + landing-summary。
