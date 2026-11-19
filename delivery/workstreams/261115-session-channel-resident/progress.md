# 261115-session-channel-resident · progress

（append-only；本文件记录调度判定、门结果、环境事实与遗留 human 项。）

## 2026-11-16 · 拆包与 launch 面准备

**调度判定**：P1 = `261116-channel-resident-core`（mergeOrder 77，riskTier high）先行，serial WIP=1；P2 `261117-session-end-intent-longretry`（mo78）等 P1 合 main 后再 launch。依据 = workstream PROPOSAL §4 + package-split §4.1（P2 依赖 P1 建立的统一会话结束处理器与常驻模型守卫语义）。

**架构+契约面锚**：FROZEN PROPOSAL v1.1（R2 GREEN）packet-bound landed，reviewedCommit `87dbf775`，合并本地 main `7e4c44d8`；`docs/architecture.md §12` 已出现 `261115-arch-review-session-channel-resident` 行。

**拆包 commit**：`214e7a2e`（dev/pool-channel；base ref = `base/261115-session-channel-resident`）。

**上游已知红基线（producer 记锚，五要素）**：

- **server 面**：`457 passed, 9 deselected, 1 warning, 33 subtests passed`（0 failed）。
  - 命令：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-channel/server/acceptance"`（仓根起）
  - 日期：2026-11-16 ｜ 量测 HEAD：`214e7a2e` ｜ 面域：`server`
- **host 面（firmware）**：`301 Tests 3 Failures 0 Ignored`——3 红全为既有 `p1_auxbar_style.acceptance.c` 存量红（`ic_wifi_14` / `ic_thermo_12` / `ic_batt_15`），与本 workstream 两包无关。
  - 命令：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`（仓根起）
  - 日期：2026-11-16 ｜ 量测 HEAD：`214e7a2e` ｜ 面域：`firmware`
- **consumer 差异判停**：Pod intake 拿到本锚后先跑 `git diff --quiet 214e7a2e HEAD -- server` / `-- firmware`；零差异 → 引用本行（SHA + 命令 + 落盘锚 verbatim），有差异 → 重跑该面。

**环境事实**：新工作树 `C:/AI/KidPal-pod-channel` 已完成固件环境等价化——`firmware/main/test/host_app/sdkconfig` 与两份 `dependencies.lock` 自 pod-m2 树复制，`firmware/managed_components` 与 `firmware/main/test/host_app/managed_components` 建为指向 pod-m2 树的 junction（共享组件缓存）；新树 `build/` 首建完成后热缓存随树保留。专属图索引 `C-AI-KidPal-pod-channel` 已建（full，22,339 nodes / 66,622 edges）。

**门结果**：`portfolio-reconcile --check-launch 261116-channel-resident-core --base-ref base/261115-session-channel-resident` = `ok:true / blocking:[] / warn:[]`；`settings-lint` = 0 RED / 0 WARN；`taskpackage-required` 仅缺 planner Step 7 自产的 `input-brief.md`（预期）。

**遗留 human 项**：无（headless 拉起授权 = human 2026-11-16 明确选择「授权无审批 headless」）。

## 2026-11-16 · P1 `261116-channel-resident-core` 收口（integrator）

**收口状态**：P1 离线面全部转绿、待 supervisor 复核 + 前置升级闸 + merge-driver 本地合并；真机轮 A 未发生，故 workstream 效果段与两条 debt 结项均**未闭合**。

- **final 终验**：round 2（`verifier-evidence/rounds/r2.json`，`ok:true / observedGateExit:0`）十闸全绿（含 `VERIFY_FW_BUILD_REQUIRED=1` 真编译）；`verifier-evidence-check` exit 0 / `factOk:true`。scoped r1 先绿，final 按 scope 圈定（`test-scope-final.txt`，25 条）跑。
- **验收实跑**：host `306 Tests / 3 Failures / 0 Ignored`（3 红全为上游 `p1_auxbar_style` AB-A1/A2/A4，本包零新增红）；server `473 passed / 0 failed / 9 deselected`。scr 15 条 acceptance（SCR-A1..A7 + SCRF-B1..B6 含 B2b/B4b）全绿，15 条 per-acceptance 变异转红留痕。
- **证据指针**：`delivery/taskpackages/261116-channel-resident-core/{review-packet.md, docs-delta.md, verification.json, verdict.json, test-scope-final.txt, verifier-evidence/rounds/r2.json}`。
- **debt 处置**：`TECH-261114`（候选 A = T1）与 `TECH-261012`（心跳保活 = T5/T6）实现已 land、离线绿，但按 PROPOSAL §6「结项凭据必须真机」status 维持 `deferred`、仅刷新 trigger + 追加收口注记；真机后由主线程结项并 `debt-render.mjs` 归位。
- **跨包交接（给 P2 `261117-session-end-intent-longretry`）**：SCR-A2 in-flight 守卫重锚、`mcp_server.cc` 注释双源、退避变事实长跑形态基线、end_session 三段补链 / 自动重连封顶——五项见 `verification.json.crossPackageHandoff` 与 `taskpackage.crossPackageBehaviors`。
- **遗留 human 项**：**一轮真机轮 A（A1–A13，human 持板，front 由 agent 部署）**——本包 exitCriteria.KRC-E4 出口；三源日志互证、证据落本包 `current/`；未发生前不宣布 workstream 目的达成、不结项 debt。
## 2026-11-17 · P1 收口卡在合并门（待人工）

**状态**：`261116-channel-resident-core` 离线面全部完成（冻结验收 15 条全绿、final r2 十闸全绿、closeout-check exit 0、docs-delta / review-packet / pod-retro 齐；host 306/3 = 3 条上游存量红，server 473/0）。本地 `dev/pool-channel` 已提交，**未 push、未 merge**。

**阻塞 1（需 human 持板）**：真机轮 A（A1–A13）+ high riskTier 的 ESCALATE 人批。合并门前置闸当前 BLOCK 于 `device-round-evidence-missing`。轮 A 三源证据交回后由 planner 落 `device-round-evidence.md` 并走人批合并链。

**阻塞 2（工具缺口，需主线程修）**：`supervisor-gate-check.mjs` 的 G4 要求 `.claude/verify-logs/*.log` 非空，但 `verify.mjs` 仅在 gate 失败时写日志（`verify.mjs:288-290` 在 `if (!ok)` 内）——本 Pod 两轮全绿 → 零日志 → G4 结构性恒红 → 会话无法正常收尾。诊断与 patch 见 `delivery/taskpackages/261116-channel-resident-core/gate-tooling-gap-g4.md`；待裁清单见同目录 `pending-human-ruling.md`。`.claude/**` 对 agent 禁写，planner 无法自修。

**debt 处置**：`TECH-261114` / `TECH-261012` 维持 `deferred`（按 PROPOSAL「结项凭据必须是真机」），仅刷 trigger + 追加收口注记；轮 A 通过后由主线程结项。

## 2026-11-17 → 11-18 · P1 真机轮 A 修复轮（planner 收尾）

**轮 A 结论**：**FAILED 诊断轮**（human 持板 + P1 固件/front 部署）。三源取证落 `delivery/taskpackages/261116-channel-resident-core/device-round-a-evidence.md` + `device-evidence/round-a/`（KPLOG / front journal / 引擎 docker，三份 SHA-256）：
- **D2（重）**：丢弃/会话关闭不掐断在途回合——stop 先转发、语境后释放，尾音 ASR 抢在释放后派发 LLM/TTS，设备 Idle 被拽回 Speaking。
- **D3**：空素材记录会话静置不结束（front 引导链因空 accum skip、设备 60s producer 对记录语境硬豁免）。
- **D1**：human 原推断「front 未进入记录语境」经三源核证**不成立**（每会话都进过、语境 ASR 被抑制）；真实缺陷 = ctx 注入依赖边缘 + front 侧无语境审计行。

**修复轮（同包边界内，human 2026-11-17「三个问题都修」）**：
- D1：ctx(story) 每会话**无条件幂等注入** + 审计 token `story_record_ctx_enter`（单 emit 点）；`control/start_recording` 消费沿返回语义 consumed。
- D2：discard 分支内经泵内本连接引擎腿**先发 active=false**（帧序先于 listen stop），由引擎既有 `_kidpal_listen_closed` 先清后转硬截断；**不发 abort**（轮 A 实证 abort 无效 + 引擎冻结纪律禁置 client_abort）；另有 front 侧「会话关闭迟到帧门」+ 自注 speak 令牌（提醒弹窗半结构性免疫）。KEY 收尾归档路径零改动（对照齿保绿）。
- D3：设备 CLOCK_TICK 区新增空素材 producer（`kStoryEmptySessionTimeoutS=60` + `!s_story_rec_entered` + `!s_vad_speaking` + 记录语境门）→ 统一收口处理器；front 既有 pending 10s 释放链零改动（释放、不归档、不推 story_list、不盖章）。
- 验收：新增冻结 5 条（SCRF-B7/B8/B9、SCR-A8、SCRF-B10）+ RDG-A4 显式重锚；变异 19 组留痕（`mutation-evidence/round-a-fix.md`，含 4 条如实观察项）。
- 离线实跑：host 307/3（3 = 上游存量）、server 486/0；final r5 十闸全绿；closeout-check exit 0。

**仍未闭合（交 human / 主线程）**：
1. **真机轮 B 复测**（B-D1/B-D2/B-D3 + A5/A6/A7 回归）——本包合并出口；`device-round-evidence.md` 未生成、不伪造 PASS。
2. **合并门**：supervisor ESCALATE（riskTier high）+ `parallel-overlap-tier3`（生成件 `delivery/workstreams/INDEX.md` 日期行；architect 判 ②，建议 `.gitattributes merge=union`；**人批豁免不了**）。
3. **G4 工具缺口**（`gate-tooling-gap-g4.md`）与本轮新发现（`gate-tooling-gap-overlap-index.md`）由主线程 `/gov-change` 修。
4. **spec 漂移**：D3 收窄 requirements §2.1 记录会话 60s 豁免 → 主线程/需求面轻量注记；本 Pod 对 `docs/**` 零 diff。
5. debt 两条维持 `deferred`（轮 B PASS 后结项）；spine/workstream 锚点更新归主线程。

## 2026-11-19 · P1 `261116-channel-resident-core` merged 本地 main

- 真机轮 E 聚焦复测通过（human 持板）：D3-UI 空素材会话 60s 自动收尾后立即回故事列表（KPLOG：`story_empty_session_timeout quiet=60` → `PAGE_RENDER in=3 sview=0` → `STORY_RENDER page=0 LIST`）；手动结束/归档/丢弃观感正常；A6 回答页驻留正常；注入测试提醒全链（front `delivered=True speech=True`、设备 `alert_render raised=1` + `alert_receipt ack=True dequeued=True` + TTS）。
- 合并：`dev/pool-channel` → main（ff，`3a2dd854` 后持续提交）；前置升级闸 PASS（human-approval `d49568ee-20c7-4461-84b5-4dc49bc49e97`）。
- 快修（human 2026-11-19 指示跳过完整 dev-loop）：`KidpalStorySessionEndApply` 内 `story_on_screen` 守卫 + `refresh_story` 重绘（`3dff5bbd`）+ 结构锚 RSL2-A6；host 322/3（3 = 上游 p1_auxbar 存量）；变异实证。
- debt 结项：`TECH-261114`（温通道时延/burst）与 `TECH-261012`（首按键）已 closed（closedBy 本包，真机凭据 + 根因消除，诚实注记见条目）。`TECH-261014-session-end` / `TECH-261014-autoconnect` 仍待 P2。
- 遗留：P2 `261117-session-end-intent-longretry` 待从新 main 起分支（`end_session` 三段补链 + 长重连）；`docs/requirements.md:48` ⚠️ 对账注记主线程路由；引擎静音 ASR 幻觉建议登记 tech debt。
