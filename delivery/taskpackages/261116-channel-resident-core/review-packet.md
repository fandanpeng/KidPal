# Review Packet · 261116-channel-resident-core

> workstream `261115-session-channel-resident` 的 P1（mergeOrder 77，rigor full，riskTier high）。
> 供 `supervisor-verdict-gate.mjs` 机械复核；BLOCKING / ESCALATE 时升级人类。本包**不含 UIUX 工作面**（`uiux.uiuxRequired=false`），无 UI design-review 节。
> **2026-11-18 更新（修复轮）**：真机轮 A（FAILED 诊断轮，D1/D2/D3）修复轮并入，final r5 十闸全绿；acceptance 累计 **20 条**（SCR-A1–A8 + SCRF-B1–B10 含 B2b/B4b）；deviceOutcome = **PENDING 轮 B**（`device-round-evidence.md` 未生成）。历史轮次记录（r1/r2 与 P1 首轮验收表）保留不删。

## Change Size

- **Files changed**：`changeManifest[]` 11 件 = 9 件修改 + 2 件新增。
  - 固件实现：`firmware/main/application.cc`、`firmware/main/application.h`
  - 固件验收：`firmware/main/test/session_channel_resident.acceptance.c`（新）、`boot_auto_connect.acceptance.c`、`voice_round_ui.acceptance.c`、`record_session_live.acceptance.c`、`record_dialogue_guide.acceptance.c`（planner 扩单入 manifest）、`firmware/main/test/host_app/main/CMakeLists.txt`
  - server 实现：`server/gateway/front/front_app.py`
  - server 验收：`server/acceptance/test_session_channel_resident.acceptance.py`（新）、`server/acceptance/test_m4k3_record_session_live.acceptance.py`（planner 扩单入 manifest）
- **Net diff**：9 件修改 `542 insertions(+) / 115 deletions(-)`（`git diff --stat HEAD -- firmware server`）；2 件新验收件 1859 行（`session_channel_resident.acceptance.c` 647 + `test_session_channel_resident.acceptance.py` 1212）。实现面主体 ≈ 400 行（`application.cc` 171 / `application.h` 28 / `front_app.py` 253）。
- **Zone 负锚兑现**：`mcp_server.cc` / `server/gateway/story.py` / `protocol.cc` / `protocol.h` / `display_schema.py` / `server/deploy/**` / `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` **零 diff**（`git diff --stat HEAD` 不含这些路径）。`docs/` 面实际改动仅 `docs/debt/tech/**` 三件账本记录（见 docs-delta）。
- **Split recommendation**：单包，不拆。改动内聚在「会话/通道解耦」一条机制链（固件四收尾点 + 60s producer + 巡检 + 心跳 + front 分层收尾），拆开会破坏「统一收口处理器」的单一原子性；P2（`261117-session-end-intent-longretry`）边界已在 `crossPackageBehaviors` 显式登记，不属拆分缺口。

### 修复轮（真机轮 A 修复轮，2026-11-18）Change Size

- **本轮 Files changed**：5 件（`git diff --numstat HEAD -- firmware server`；相对上一收口 commit）：
  - `firmware/main/application.cc` +32/−0（D3 空素材会话 60s producer + 命名常量 `kStoryEmptySessionTimeoutS`）
  - `server/gateway/front/front_app.py` +209/−35（D1 ctx 注入去边缘化 + `story_record_ctx_enter` 审计单点；D2 discard 释放语境；B9 会话关闭迟到帧门 + 有界令牌）
  - `server/acceptance/test_session_channel_resident.acceptance.py` +723/−0（SCRF-B7/B8/B9/B10 四组新断言）
  - `firmware/main/test/session_channel_resident.acceptance.c` +141/−0（SCR-A8 源锚代理断言）
  - `server/acceptance/test_record_dialogue_guide.acceptance.py` +35/−6（RDG-A4 显式重锚，旧钉→新钉见下）
  - **合计 +1140/−41**。
- **负锚复核（本轮实测零 diff，`git diff --stat HEAD -- <paths>` 为空）**：`firmware/main/mcp_server.cc`、`server/gateway/display_schema.py`、`server/gateway/story.py`、`firmware/main/protocol.*`、`firmware/main/story_page.c`、`docs/contracts/**`、`docs/**`、`server/deploy/**`、`engine-patch/**`、`firmware/main/application.h`。诊断见 D1/D2/D3 的边界声明：修复只落在 `front_app.py` 与 `application.cc` 两件实现件，manifest 零扩。
- **无新增依赖、无跨层新调用、无本地绕过契约的类型；`docs/contracts/**` 仍只读。**

## Verification Evidence

- **Final round fact（承重）**：`delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r2.json`，`phase=final / round=2 / ok=true / observedGateExit=0`，10 闸全跑（`timing` scoped 8 + full 2）。
- **机器凭据闸**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 2 --invocation-id e5cb4370-0470-4df6-a1ab-316ac66cb4aa` → `exit 0 / factOk=true / phase=final / round=2`。
- **Structural gates**：`taskpackage-required`、`change-manifest`（pre-cutoff 存量放行声明）、`firmware-build`（`VERIFY_FW_BUILD_REQUIRED=1` 真编译 `build-pass`）、`contract-lint`、`acceptance-coverage`（15 acceptance id 全覆盖）、`mutation-check`、`docs-sync`、`security`（`securityReviewRequired=false`）——8 闸 ok。
- **Acceptance / unit（full 相位按 scope 圈定真跑）**：`unit` 187 passed / 3 deselected / 22 subtests（14 文件）；`acceptance` 135 passed / 3 deselected / 3 subtests（server acceptance 11 文件，firmware acceptance 面 11 件存在性核对）。
- **Host runner（planner 亲跑 @ working tree，执行目录 `firmware/main/test/host_app`）**：`306 Tests / 3 Failures / 0 Ignored`——3 红全为上游存量 `p1_auxbar_style.acceptance.c` AB-A1 / AB-A2 / AB-A4（`ic_batt_15` / `ic_thermo_12` / `ic_wifi_14`），与本包无调用关系；本包 SCR-A1..A7 与四件重锚件全绿。
- **Server 全量（planner 亲跑 @ 仓根，`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）**：`473 passed / 0 failed / 9 deselected`（launch 基线 457 → +16 = 本包新增 host/server 验收面合计；零新增红）。
- **Mutation check**：15 条 per-acceptance 变异逐条「精确字符串改坏 → 对应冻结验收转红 → sha1 校验还原」留痕（builder dispatch 表）；另如实申报 1 条不敏感项（SCR-A4 `RaiseReminderQueue` needle 被既有注释遮蔽，见 Findings N-01）。
- **Docs sync**：`docsSyncRequired=false`，`docs-sync` 闸 PASS；人工复核理由见 `docs-delta.md`（spec/契约/架构/ADR 四类零 diff；两条 debt 待真机结项）。
- **Scope 圈定（上一轮 r2，历史留痕）**：`delivery/taskpackages/261116-channel-resident-core/test-scope-final.txt`（当时 25 条）。合成 = `acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（16）∪ `detect_changes`（project `C-AI-KidPal-pod-channel`，since `base/261115-session-channel-resident` / merge_base `3b3832cb`，inbound depth 2：23 changed files / 334 seed 符号 / 103 impacted 节点，其中 firmware/main 21、server/acceptance 73、server/gateway 5、server/tests 4）映射测试 ∪ 安全余量。per-Pod 专属图索引可用 → **正常圈定、未降级**；不缩到本包档、不默认全量。收口 commit 后 detect_changes 复核：仍 334 seed 符号 / 103 impacted 节点（firmware/main 21、server/gateway 5、server/acceptance 73、server/tests 4），与圈定一致，实现面符号影响无外溢。

### 修复轮复验（本轮，承重）

- **轮序**（completed-only 序数，seq 由 planner 分配、不得改）：r1 scoped 8/8 → r2 final 10/10（上一收口）→ **r3 scoped（`ok:false`）** → r4 scoped 8/8 → **r5 final 10/10**。
- **r3 红因（不隐瞒，事实占序）**：`verifier-evidence/rounds/r3.json` = `phase=scoped / round=3 / ok:false / observedGateExit=1`，唯一红闸 = `mutation-check`（`missingSensitive` 点名 SCRF-B7/B8/B9 等新增条目的 acceptance 描述）。根因 = planner 代 builder 落盘 `mutationEvidence` 时截断 acceptance 字段，mutation-check 按 id + 描述比对失配；**非实现缺陷、非断言缺陷**，修正 taskpackage 后 r4 转绿。
- **r4 scoped 8/8 绿**：`r4.json`，invocationId `8c2e5ab9-1806-4caf-ba6b-bb562d789f91`，observedGateExit=0。
- **r5 final（承重）**：`verifier-evidence/rounds/r5.json`，`phase=final / round=5 / ok=true / observedGateExit=0 / invocationId 832705da-ca3a-42a3-bdc3-c766fee33c6c`，10 闸全跑（scoped 8 + full 2）。闸表：
  - `taskpackage-required` / `change-manifest`（pre-cutoff 存量放行）/ `contract-lint` / `acceptance-coverage`（20 acceptance id 全覆盖）/ `mutation-check`（20 条 per-acceptance 变异 evidence 齐）/ `docs-sync` / `security` —— 7 闸 pass（各 105–369ms）。
  - `firmware-build`：`VERIFY_FW_BUILD_REQUIRED=1` 真编译 `build-pass`（xiaozhi.bin 0x329080 bytes，9.1s）。
  - `unit`（full，scope 求交）：**247 passed / 3 deselected / 22 subtests**（14 py 文件，37.9s）。
  - `acceptance`（full，scope 求交）：**195 passed / 3 deselected / 3 subtests**；surface = server 14 / firmware 11（firmware on-target 仅结构与 runner 存在性核对，实机归轮 B）。
- **机械凭据闸**：`verifier-evidence-check.mjs --phase final --round 5 --invocation-id 832705da-ca3a-42a3-bdc3-c766fee33c6c` → `exit 0 / factOk=true`。
- **Host runner / server 全量（builder 修复轮基线，integrator 采信）**：host `307 Tests / 3 Failures / 0 Ignored`（3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4）；server 全量 `486 passed / 0 failed / 9 deselected`。零新增红。
- **Scope 圈定（本轮，范围圈定清单落盘路径一行）**：`delivery/taskpackages/261116-channel-resident-core/test-scope-final.txt`（**28 条**，覆盖写）。合成 = `acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（17 去重）∪ `detect_changes`（project `C-AI-KidPal-pod-channel`，since merge_base `3b3832cb`，inbound depth 2）映射测试 ∪ 安全余量。圈定时 detect_changes 实测 = 47 changed files（含本包 delivery/docs 簿记件；实现/验收面 12 件）/ impacted_total 324（图索引当时处于刷新过程，firmware/main 面膨胀至 217 个 board 文件节点）；**收口 commit 后复核（两次调用稳定）= seed 387 符号 / impacted 108 节点（server/acceptance 73、firmware/main 21、server/gateway 10、server/tests 4）**——两次观测指向同一测试面，圈定清单覆盖两者，实现面符号影响无外溢。per-Pod 图索引可用 → **正常圈定、未降级**；不缩到本包档、不默认全量。本轮 detect_changes 新命中 `points_ledger_core` / `qq_points_channel` / `schedule_grid_restore`（上轮按「构造上不触」排除）——本轮按影响面清单纳入，圈定面**只增不减**；`archive/test_schedule_reminder` 属退役面（两闸 SKIP_DIRS）如实排除并留痕。

## Per-Acceptance 证据（15 条；P1 首轮，历史留痕）

| Acceptance | 断言载体（测试） | 结果 | 变异判别力 |
|---|---|---|---|
| SCR-A1 四收尾点收口负锚+正锚闭集 | `session_channel_resident.acceptance.c:288` 四收尾点收口负锚 | PASS | 变异 #1（helper 插回 `protocol_->CloseAudioChannel`）→ 本断言转红 |
| SCR-A2 统一处理器动作序 | `:364` 统一处理器动作序 | PASS | 变异 #2（Speaking 分支复制 `ResetDecoder`）→ 转红 |
| SCR-A3 五真拆通道点保绿 | `:421` 五个真拆通道保绿 | PASS | 变异 #3（`Reboot` 真拆点改 `SendStopListening`）→ 转红 |
| SCR-A4 Idle 八件清位链保绿 | `:463` Idle 八件清位链保绿 | PASS | 变异 #4（`kidpal_keybar_scene_reset` → `..._restore`）→ 转红（含 RSL2-A2 / CG-A1 同向共红） |
| SCR-A5 60s producer + VAD 说话沿重置 | `:504` 60s producer + 说话沿重置 | PASS | 变异 #5（常量 60 → 30）→ 转红 |
| SCR-A6 心跳 no-op 分支位次 | `:560` 心跳 noop 分支位次 | PASS | 变异 #6（`kidpal_keepalive` 分支 `== 0` 改 `false`）→ 转红 |
| SCR-A7 自愈巡检短路序 | `:601` 自愈巡检短路序 | PASS | 变异 #7（两操作数互换）→ 转红 |
| SCRF-B1 心跳 producer（front） | `test_session_channel_resident.acceptance.py::test_scr_b1_*`（4 例：默认开/周期恰一帧/`=0` 零帧/异常静默收尾仍落） | PASS | 变异 #8（极性反转）→ `test_scr_b1_heartbeat_gate_default_on_and_rollback_off` 转红 |
| SCRF-B2 `listen stop` 分层收尾 | `test_scr_b2_chat_noop_and_accumulate_consumed_once`、`test_scr_b2_pending_protected_and_repeated_stop_idempotent` | PASS | 变异 #9（pending 保护改 cancel）→ 转红 |
| SCRF-B2b pending 保护 + sweep 释放 | `test_scr_b2b_sweep_releases_at_ten_seconds`、`test_scr_b2b_stt_recovery_prevents_double_release` | PASS | 变异 #10（sweep 释放改 `pass`）→ 转红 |
| SCRF-B3 discard→stop 有序不归档 | `test_scr_b3_discard_then_stop_no_archive` | PASS | 变异 #11（discard cancel 改 `dropped=False`）→ 转红 |
| SCRF-B4 ctx 边沿兜底分态 | `test_scr_b4_accumulate_cancels_and_releases`、`test_scr_b4_pending_stop_no_action`、`test_scr_b4_chat_marked_only_release` | PASS | 变异 #12（accumulate 删 cancel）→ 转红 |
| SCRF-B4b pending_stop 边沿不动作（F-01 防线） | `test_scr_b4b_pending_stop_edge_then_stt_archives_once` | PASS | 变异 #13（pending 保护改 cancel+释放）→ 转红 |
| SCRF-B5 连接级 teardown 保绿 | `test_scr_b5_teardown_completes_cancel_and_legs_removed` | PASS | 变异 #14（teardown cancel 改 `pass`）→ 转红 |
| SCRF-B6 既有冻结 oracle 保绿回归网 | `test_scr_b6_frozen_oracles_present` + 全量 suite 绿留痕 | PASS | 变异 #15（挂起 `test_m4k1` 文件）→ 转红 |

> firmware SCR-A* 断言的 PASS 证据来自 host runner（planner 亲跑 306/3/0）；`verify.mjs` 的 `acceptance` 闸对固件面只作存在性/结构核对（on-target 需实机），已在闸 output `note` 明示。

## Per-Acceptance 证据 · 修复轮追加（5 条；全包累计 20 条）

| Acceptance | 断言载体（测试） | 结果 | 变异判别力（引 `mutation-evidence/round-a-fix.md`） |
|---|---|---|---|
| SCRF-B7 ctx(story) 每会话**无条件**注入 + 审计单点 | `test_session_channel_resident.acceptance.py:1387` `test_scr_b7_repeat_ctx_story_injects_again_story_list_stays_edge_only`、`:1441` `..._start_recording_consumption_injects_when_accumulating`、`:1476` `..._audit_token_single_emit_point` | PASS | 变异 #1（恢复 `ctx_push_edge` 条件）→ 两例转红；#2（返回语义回退 `enter_edge`）→ start_recording 例转红；#3（审计 token 改非超串名）→ B7 三例转红；#4（删 `_inject_record_context_enter(` 调用）→ 转红。观察项：token **超串**改名不转红（计数/子串匹配特性），已申报验收质量 NIT。 |
| SCRF-B8 丢弃分支释放语境且严格先于转发的 listen stop | `:1508` `test_scr_b8_discard_releases_context_before_forwarded_stop`、`:1561` `..._repeated_stop_injects_no_second_release`、`:1593` `..._discard_branch_has_no_global_alert_legs`、`:1609` `..._key_stop_still_archives_once_and_releases_after_stop`（对照齿） | PASS | 变异 #5（删 discard 释放）→ 前两例转红；#6（腿集改 `_session_legs(sid) or _alert_legs`）→ 无全局腿例 + 两例转红；#16（对照：在转发 stop **前**插释放）→ B8 三例转红（含 KEY 收尾对照）。全程零 abort 帧由断言内负锚承载。 |
| SCRF-B9 会话关闭迟到帧门 + 有界出音令牌 | `:1666` `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt`、`:1711` `..._gate_disarms_on_next_listen_start_and_ctx_story`、`:1750` `..._speak_token_allows_round_then_tts_stop_clears`、`:1795` `..._alert_speech_registers_expected_round_token` | PASS | #7（删门内二进制丢弃）→ 两例红；#8（丢弃集 `("tts","llm")`→`"tts"`）→ 红；#9（丢弃集加 `"mcp"`）→ 红；#10（删提醒 speech 令牌登记）→ 红；#11（tts stop 先清后判）→ 令牌放行例红。 |
| SCR-A8 空素材会话空闲收尾 producer（源锚代理） | `session_channel_resident.acceptance.c:667` `TEST_CASE("会话通道常驻: 空素材空闲收尾producer (SCR-A8)", …)` | PASS | #12（常量 60→30）→ host 307/4 红；#13（删 `!s_story_rec_entered &&`）→ 红；#14（审计 token 改名）→ 红；#15（在既有 60s 常量前插含符号字面 + ASCII `;` 的注释）→ SCR-A8 **+ SCR-A5 交叉红**（307/5）。观察项：#15 首试注释不含 ASCII `;` 不转红（交叉负锚只对含分号注释成立），已申报。 |
| SCRF-B10 空素材收尾 = 释放、不归档、不推 story_list、不盖章 | `:1843` `test_scr_b10_empty_material_stop_releases_without_archive`、`:1897` `test_scr_b10_tail_hallucination_rejected_same_release_contract` | PASS | #17（kill-shot：空素材走 `_story_finish_session`）→ `fin == []` 断言红；#18（`_archive_material_reject_reason` 首行 `return None`）→ 零 `start_organizing` 断言红；#19（会话腿 push `story_list` 的等价源变异）→ 新增会话腿观测断言红（观测面已闭合）。 |

**RDG-A4 显式重锚（冻结件，KRC-E2 留痕，非新 acceptance id）**：

- **旧钉**：第二次 ctx(story)（routing 已 accumulate）**零** `active=true` 注入（「进语境收敛入口幂等」= 不重复发帧）。
- **新钉**：第二次 ctx(story) 仍**恰一帧** `active=true`；`story_list` 仍零新增（`_push_story_list` 继续守 `ctx_push_edge`）。
- **为什么**：human 2026-11-17「三个问题都修」+ `device-round-a-evidence.md` §三——轮 A 三源证明引擎侧语境在场，但 front 注入带 `ctx_push_edge` 条件，累积态泄漏 / 引擎腿重建（routing 已 accumulate）时不注入 → 引擎不抑制。修复 = 无条件注入（幂等语义改为「重复发帧亦安全且必要」）。断言方向只把「预期帧数 0 → 1」并新增 story_list 零新增守卫，未放宽其余断言；REVISION LOG 已落 `test_record_dialogue_guide.acceptance.py`。

## 实施偏离与扩单留痕（builder 三处 + planner 两笔）

**Builder 三处实现偏离（planner 已接收，随本包申报）**：

1. **记录语境标记清位按会话键跨连接**：`_record_ctx_clear(session_id, legs)`（`front_app.py:983`）遍历全部已登记 device websocket 按 `session_id` 清位，而非仅按 `legs` 的 device_ws 清。理由（docstring `:984-990`）：泵直调/离线线束下「收 ctx 的 device ws」与「归属腿 device ws」可能非同一对象，逐腿清会留残标记 → 重复 ctx 触发二次释放破坏 SCRF-B4 幂等。会话键每连接唯一（HWV-A11），语义仍等价「按连接清」。**评估**：正确性成立，代价是释放时 O(n) 扫描存活连接（n 极小），见 Findings N-03。
2. **审计行追加 `state=` 后缀**：四路审计行由原 `state=idle` 改为语义化值——`discard_record`→`state=session_end_requested`（`application.cc:1026`）、`toggle_chat`→`state=session_end`（`:1075`）、`wake_invoke`→`state=session_end`（`:1565`）、`overlay_close` 维持 `state=idle`（`:1215`）。`local_close_settle` + `src=` 冻结 token 零变，故 OCE-A1 / SCR-A1 region 断言（只钉 `local_close_settle` / `src=overlay_close` / `src=<路>` 存在性）不受影响。**评估**：诊断口径更细，但 `state=` 词汇三态不统一，见 Findings N-02。
3. **注释刻意不复述判据字面**：`application.cc` 的 T2 producer / T4 巡检 / T3 keepalive 三处注释明写「注释刻意不写判据原文的符号字面——验收件按源文本计数，注释里的字面会稀释断言区分力」。**评估**：这是对源文本计数断言的合理防稀释，但把「注释不得写回字面」变成隐性维护约束，见 Findings N-01 同族观察。

**Planner 扩单两笔（均已回评补 manifest）**：

1. `firmware/main/test/record_dialogue_guide.acceptance.c` 重锚：builder 首跑曝露 RCL-F1 粗粒度区切（`HandleStopListeningEvent`→`HandleWakeWordDetectedEvent`）误跨统一处理器致假红；最小重锚 = `fn_end` 收窄为 `void Application::HandleEndVoiceSessionEvent` + REVISION LOG，断言语义零变化（tighten-only）。该件 1g 扫描漏件，已入 manifest 与 impactedTestsSeed。
2. `server/acceptance/test_m4k3_record_session_live.acceptance.py` 入 manifest：package-split §2.1 v1.2 已升为非条件成员（A7 ctx 非 story 零帧语义前提精确化 + 新增释放边沿姊妹齿），原文清单漏列；断言零删弱。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包新增代码无 `minimal:` 标记；修复轮新增 front_app.py 门/令牌一节与 application.cc producer 均无 `minimal:` 标记） | — | — | `no-trigger` |

> `front_app.py:2654` 的既有 `minimal:` 标记属 `261022-p3-record-front-polish-fix` 的 `_story_sweep_loop`（本包零改该函数），不归本包债务台账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Test quality | SCR-A4 的 `RaiseReminderQueue` needle 被 `application.cc:1342` 既有注释字面遮蔽，该 needle 单独不具判别力（变异后 SCR-A4 仍绿，仅 `reminder_overlay_unify` RM-A3 转红）。 | changed-code hunk `session_channel_resident.acceptance.c` SCR-A4 断言表；builder mutation「不敏感项」实测（变异后 SCR-A4 绿、RM-A3 红）；`application.cc:1342` 注释含该符号 | 不阻塞：builder 已用同区 `kidpal_keybar_scene_reset` 承载敏感变异，SCR-A4 整体仍有判别力；留作验收质量观察项，后续触及该区时把 needle 换到代码单点、注释无该字面（交 P2 或专门验收批）。 |
| NIT | judgement-call | Operability | 同族审计行 `state=` 取值三态不统一（`session_end` / `session_end_requested` / `idle`），按 `src` 分别解析日志时需额外汇总规则。 | changed-code hunks `application.cc:1026` / `:1075` / `:1215` / `:1565` | 不阻塞：登记为 builder 偏离；若真机 KPLOG 解析需要统一，P2 归一或加解析别名，本轮不改。 |
| NIT | judgement-call | Maintainability | 实现注释「刻意不复述判据字面」把「注释不得写回被 count 的 token」变成隐性约束；未来编辑若在注释里写回 token 会使源文本计数断言假绿。 | changed-code hunks `application.cc` T2 producer / T4 巡检 / T3 keepalive 注释明写该约定 | 不阻塞：建议在新验收件头注一句「本区注释刻意避判据字面」，防止回摆（可后续批处理；本轮内容治理已由 builder 注释留痕）。 |

| NIT | judgement-call | Test quality | 修复轮变异灵敏度两处缺口：SCRF-B7 审计 token **超串**改名（`story_record_ctx_enter` → `story_record_ctx_enter_renamed`）不转红（子串/count 匹配特性）；SCR-A8 交叉负锚只对「含 ASCII `;` 的注释」转红（变异 #15 首试不含分号不红）。 | changed-code hunk `test_session_channel_resident.acceptance.py` B7 token 断言；`session_channel_resident.acceptance.c` SCR-A5 区切；`mutation-evidence/round-a-fix.md` §三.1/§三.2 实测 | 不阻塞：非超串改名与含分号注释均转红，判别力总量成立；登记验收质量观察项，后续收紧断言时改精确等值匹配 / 改按定义行解析。 |
| NIT | judgement-call | Operability | 出音令牌只登记在 `_speak_to_legs` 与 `_deliver_alert_now` 两处；`_consume_points_intent` 的去重语音直注不经 `_speak_to_legs`，会话关闭门 armed 时该语音可能被当迟到回合丢弃。 | changed-code hunks `front_app.py` `_speak_to_legs`（`_speech_token_register`）、`_deliver_alert_now`、`_consume_points_intent` 注入沿；冻结方案（plan.md D2/B9 节）明列两处登记点 | 不阻塞（accepted residual：最坏丢一次积分语音，积分入账与 UI 帧不破）；轮 B 观测，真机复现另立批补第三登记点。 |
| NIT | judgement-call | Operability | 会话关闭门 arm 沿传入 `legs` 传播；sweep 的腿集为 `_session_legs(sid) or list(_alert_legs)`，会话腿已摘除时 fallback 全局提醒腿 → 多连接部署下其他在线设备也被 arm（各自下一会话开始时 disarm）。 | changed-code hunk `front_app.py` `_release_record_context` arm 循环 + `_story_timeout_sweep` 腿集既有语义 | 不阻塞（单设备部署无感；风险低）；轮 B 观测；多连接场景若暴露另立批把 arm 收到会话归属腿。 |
| NIT | judgement-call | Test quality | SCRF-B10 观测面两处残余：`start_organizing` 断言仍只观测外层设备腿（同类形状的 `story_list` 断言已 tighten-only 闭合）；「零归档 md」断言在离线无 LLM stub 时判别力有限（实际由「零演出」断言抓红）。 | changed-code hunks `test_session_channel_resident.acceptance.py:1843/:1897`；`mutation-evidence/round-a-fix.md` §三.3/§三.4 实测 | 不阻塞：本轮判别力由 kill-shot（变异 #17）与零演出（#18）承担，md 断言作第二道；轮 B 用 STORIES_DIR 零新增三源对账补强，后续专批收紧观测面。 |

> 无 BLOCKING、无 SHOULD（hard-violation 零条）。P1 首轮三条 + 修复轮四条 = 七条，均为 NIT 级 judgement-call，均有 changed-code hunk + heuristic / 实测依据，不伪造 rule citation。

## Five-Axis Review

**1. Correctness（S-Fx.y / 边界条件）**
- **四收尾点收口形态二分正确**：协议接收任务上下文（`CloseAudioChannel` 历史名 helper，调用链 `OnIncomingJson → McpServer::ParseMessage → mcp 回调`）只投事件位 `MAIN_EVENT_END_VOICE_SESSION`（`application.cc:1000-1002`）；主循环上下文（toggle / wake-invoke / CLOCK_TICK 60s）直调 `HandleEndVoiceSessionEvent()`。事件位在 `Run()` `:378` 单源消费。两类不混用，避免跨任务竞态。
- **处理器守卫与动作序逐字保留**：`!protocol_` 早退（`:1189`）、`auto_connect_in_flight_` 守卫（`:1192`）、三态门（Connecting/Listening/Speaking，`:1199`）均在 `protocol_->SendStopListening()`（`:1211`）之前，空指针与竞态安全成立；Speaking 分支 `ResetDecoder()` 先于 `AbortSpeaking` 先于 `SendStopListening` 与 abort 帧同 TCP 有序。
- **60s producer 计时单位正确**：`clock_timer` 周期 `esp_timer_start_periodic(..., 1000000)` = 1 秒（`application.cc:258`），`clock_ticks_++` 每 tick（`:441`），故 `clock_ticks_ >= kSessionIdleTimeoutS(=60)` = 60 秒；状态迁移（`:1282`）与 VAD 起音沿（`:409`）归零。四判据（聆听态 + `!s_vad_speaking` 电平门 + 计时到阈 + 记录语境豁免）齐备；电平门是「连续讲 >90s 不被误收」的承重件（仅沿重置会在第 60 秒误收）。
- **巡检零第二条重连路径**：`Idle && protocol_ && !auto_connect_in_flight_ && !IsAudioChannelOpened()` → `TryStartAutoConnect()`（`:498-503`），守卫短路序 in-flight 先于通道判据（降频承重），区内零直连建连、不新建 task、不复用 `clock_ticks_`、不写 `clock_ticks_ % 10`。
- **front 分层收尾**：`pending_stop` 当刻绝不 cancel（F-01 保护，`front_app.py:1996-2000`）；`accumulate` 已被归档路径消费不动作；`chat` + 标记真只释放；ctx 边沿分态（pending 不动作 / accumulate cancel+释放 / chat 标记真只释放）；未知 `source_page` 仍拒绝 + warning + 不释放（`elif` 逐字保留）。`_story_timeout_sweep` 补 `_release_record_context`（承重缺口闭环）。
- **心跳**：sleep-first（建连零帧，不污染三件真跑 `ws_gateway` 的零偏移前提）、裸 `send_text`（非 display 帧）、`except Exception` 静默退出（`CancelledError` 是 BaseException 不被吞）、`finally` 首行 `cancel()` + `gather(return_exceptions=True)`、不进 `FIRST_COMPLETED` 集合。

**2. Test quality**
- 15 acceptance 各有独立断言载体，且逐条有 per-acceptance 变异转红证据（见上表）；变异含「删/换符号/查极性/交换短路序」多型，非仅覆盖率点缀。
- 断言面区分力真实：源文本计数断言对注释字面敏感（builder 主动避字面防稀释），结构性断言用命中计数 + 位次偏移 + 负锚。
- 观察：SCR-A4 `RaiseReminderQueue` needle 被注释遮蔽（N-01）；注释-断言耦合（N-03 同族）已在 Findings 登记，不影响本轮判别力总量。

**3. Contract / boundary safety**
- `docs/contracts/**` 零 diff；本包 T3/T5–T8 是契约 §四（`kidpal_keepalive` / `kidpal_session_ctx` / `listen` / `kidpal_story_discard`）的**实现落地**，无新帧、无新 view（display 白名单不扩）、无 `SPEC_CONTRACT_DRIFT`。
- discard→stop 顺序约束依赖「`SendKidpalStoryDiscard` 帧先（调用方在同一 TCP 上行）、事件位在主循环下一拍才发 listen stop」——`mcp_server.cc` 零改动即守住 P2 边界与契约 §四顺序。
- `story.py` 三缝（`release_pending` / `cancel` / `PENDING_STOP_SECONDS`）零改动；engine-patch / ECS `core/**` / `close_connection_no_voice_time` / `end_prompt.enable` 零 diff。

**4. Operability / security**
- 心跳极性正确：`KIDPAL_CHANNEL_HEARTBEAT` **代码默认开**，`0/false/off/no` 为回滚开关（与 `KIDPAL_STORY_RESTORE_PUSH` 极性相反，理由 = 默认关会让新固件退化 churn）；周期 `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S` 默认 30。
- 审计 token 带 `corr_id=kdp-<ts>`，四路 src 可区分；keepalive no-op 避免每 30s 一条 `Unknown message type` WARN 冲 SD 日志轮转预算。
- `securityReviewRequired=false`；无新 secret、无权限面变更、无外部输入解析新增攻击面（心跳帧仅 `strcmp` 识别 type 后 return）。
- 观测风险：`IsTimeout()` 日志密度真机未量测（见未覆盖面 #5）。

**5. Maintainability / simplicity**
- `CloseAudioChannel()` 保留历史名（头注明确「名字为历史名、语义 = 会话收尾」，改名须碰 `mcp_server.cc` = 越界），删除 `ScheduleResidentRebuild()` / `kResidentRebuildDelayS` 出清冷重建面；`kSessionIdleTimeoutS` 接替常量位置。
- `_record_ctx_seen` 沿 `_POINTS_OBSERVED` 先例（`WeakKeyDictionary` + 属性回退），清位单点归 `_release_record_context`，三既有调用点与新增 sweep / ctx 边沿自动一致。
- 无过早抽象、无重复实现、无新依赖。既有 `minimal:` 标记不归本包。N-02/N-03 为可维护性观察项，不阻塞。

### 修复轮五轴复核（2026-11-18，author 语义 review）

**1. Correctness**
- **D1 去边缘化**：`_consume_kidpal_control` ctx(story) 分支只去掉 `ctx_push_edge` 注入条件，`_push_story_list` 恢复推送仍守边缘（契约 §四「重复发送安全」不破）；`_story_route_display_payload` 返回语义 `enter_edge → consumed`，唯一消费者（泵两处）同步改，`edge_probe` 出参在 `acc.start` **之前**自查 routing（消费即转 accumulate，事后查不到原态）；审计行 `story_record_ctx_enter` 单一 emit 点收在 `_inject_record_context_enter`，三处调用点共用。
- **D2 discard 掐断在途回合**：discard 分支返回前 `await _release_record_context(session_id, [(websocket, upstream)])`——同一协程、同一 upstream、顺序 await ⇒ `active=false` 严格先于随后被转发的 `listen stop`；腿集只用泵内本连接对，不取全局 `_alert_legs`；**不发 abort 帧**（轮 A 实证 abort 已收仍产 TTS；引擎冻结纪律禁置 `client_abort`，engine-patch 零 diff）。重复 stop 幂等由 `_record_ctx_seen` 早退承载，不另写计数。
- **B9 会话关闭门**：arm 单点 = `_release_record_context`（discard / 空素材 / 归档拒绝 / 整理完成 / pending sweep / ctx 边沿六类关闭路径共用）；disarm = 设备下一次上行 `listen start` 或 ctx(story)；门内只丢二进制音频帧与 `tts` / `llm` 文本帧，`stt` 与 `mcp`（提醒 / story_text / story_list）照常放行；令牌**先判后清**防 tts stop 被吞导致设备 Speaking 悬挂；30s deadline 兜底、重复登记取 max。
- **D3 空素材 producer**：五判据（聆听态 + 记录语境正向门 + `!s_story_rec_entered` + `!s_vad_speaking` 电平门 + 计时到阈）齐备；素材信号选 `s_story_rec_entered`（application.cc file-static，VAD 起音沿置位、Idle 分支清位，避开 `mcp_server.cc` 跨包改动）；阈值 60 沿用既有单一口径；`clock_ticks_` 在状态迁移与 VAD 起音沿归零，故 60 tick = 会话静置 60s；不写 `clock_ticks_ % 10`、常量注释不复述既有超时常量字面（保 SCR-A5 区切）。
- 边界声明兑现：`firmware/main/application.h`、`server/gateway/story.py`、`display_schema.py`、`protocol.*`、`story_page.c`、`mcp_server.cc`、`docs/**`、`docs/contracts/**` 本轮零 diff（实测）。

**2. Test quality**
- 修复轮 19 条变异（含 1 条对照齿、1 条 kill-shot）逐条「精确改坏 → 对应断言转红 → sha256 还原」；五条新 acceptance 各有独立载体与判别力（见上表）。
- RDG-A4 为显式重锚而非静默改齿：REVISION LOG 写全「旧钉 → 新钉 → 为什么」，只把预期帧数 0→1 并新增 story_list 零新增守卫。
- 四处观察项（超串改名、注释分号、观测面残余、md 断言判别力）如实入 Findings / 未覆盖面，未虚报。

**3. Contract / boundary safety**
- `docs/contracts/**` 零 diff，无新帧、无 display 白名单扩项；D1/D2/B9 都是契约 §四既有帧（`kidpal_record_context` / `listen`）的实现语义修正。
- 不触碰 `mcp_server.cc`（discard 上行语义保持「帧先、事件位下一拍」）、`story.py` 三缝、engine-patch 与 ECS `core/**`。

**4. Operability / security**
- 新增 `story_record_ctx_enter` 与 `story_empty_session_timeout` 两条可判别审计 token，供轮 B 三源对账（front / 引擎 / 设备各一侧）；front 新增日志量约 1 行/会话 + ≤3 行引导轮，量小。
- B9 门状态用 `WeakKeyDictionary` + 属性面回退，连接回收即摘除，无无界增长；不新增 asyncio 锁（沿用既有 residual）。无新 secret、无权限面变更、无新增外部输入解析。

**5. Maintainability / simplicity**
- 三个 conn-state helper（get/set/pop）归一复用，沿 `_RECORD_CTX_SEEN` 先例；门与令牌逻辑集中在 `front_app.py` 一节，臂/撤臂各单点，注释写明设计依据与副作用（每会话 1+引导轮 active=true 帧、约 50B/帧）。
- 无过早抽象、无重复实现、无新依赖。已知简化债：令牌只两处登记（NIT 已登记），不阻塞。

## Demo 对账行（workstream 包纪律）

> 本包 demo 形态（roadmap §3 与 plan「demo 证据形态」节）= host runner + server pytest + device build + 一轮真机轮 A（human 持板）。逐形态对账：

| 证据形态 | 实际结果 | 与 roadmap 效果段（§① 目的 1/2/4/6）一致性结论 |
|---|---|---|
| host runner（设备 C 验收，结构/行为半） | P1 收口 `306 Tests / 3 Failures / 0 Ignored`；修复轮 `307 Tests / 3 Failures / 0 Ignored`；3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4；SCR-A1..A8 + 五件重锚件全绿 | 与「实现已 land、结构断言真」一致；**不足以单独证明**用户可见效果（时延 / 连续讲 / 自愈），效果确认待真机轮 B |
| server pytest（front 验收） | P1 收口 `473 passed / 0 failed / 9 deselected`；修复轮全量 `486 passed / 0 failed / 9 deselected`；final r5 scope 求交 `unit 247 / acceptance 195` 全绿 | 与「front 分层收尾 / 心跳 / ctx 分态 / 修复轮三缺陷离线成立」一致；真实 30s 节奏与真机帧序待轮 B |
| device build | `firmware-build` 闸 `VERIFY_FW_BUILD_REQUIRED=1` 真编译 `build-pass`（xiaozhi.bin 生成，r5 为 0x329080 bytes） | 与「编译面全绿」一致 |
| 真机轮 A（human 持板，front 由 agent 部署） | **已发生，FAILED 诊断轮**（2026-11-17，D1/D2/D3 三缺陷，`device-round-a-evidence.md` 三源取证）→ 修复轮离线转绿；`device-round-evidence.md` **仍未生成**（轮 B 未发生，不伪造 PASS） | 轮 A 证明了用户可见效果段的**真实缺口**（D2 迟到 TTS 拽回 Speaking、D3 空素材不结束），本轮修复后待**轮 B 复测**闭合 roadmap 目的 1/2/4/6 效果段；判据 = `exitCriteria.KRC-E4`（A1–A13）+ 修复轮 B-D1/B-D2/B-D3；deviceOutcome = PENDING |

## 设备轮证据（真机轮 A 诊断轮 + 轮 B 待复测）

**轮 A = FAILED 诊断轮（2026-11-17，human 持板 + P1 固件烧板 + front 部署 ECS）**。证据文件 = `delivery/taskpackages/261116-channel-resident-core/device-round-a-evidence.md`；三源最小脱敏摘录与 SHA-256：

| 源 | 摘录路径 | SHA-256 |
|---|---|---|
| 设备 KPLOG（含 `KPLOG000b.TXT` 并窗） | `device-evidence/round-a/kplog-excerpt.txt` | `32e034e2c27e7b90bcce780cbcde4b25ccb3598c30d9d6dcea06128885b08151` |
| front systemd journal（ECS） | `device-evidence/round-a/front-journal-excerpt.txt` | `4fd8acd182e4b90efc417e70269df04d536ab3d76b69627c7c7fc9fdd4ecce45` |
| 引擎 docker logs（ECS `xiaozhi-esp32-server`） | `device-evidence/round-a/engine-docker-excerpt.txt` | `1465aef458c258b0311360dfe6561d4c0fb82157b7464a8d2fe42645fe7c2649` |

三条缺陷（观察窗 22:24–22:53 CST，缺陷集中段 22:46:30–22:53:30；目击固件 commit / `sha256(xiaozhi.bin)` **未采集**，human 未交付、不补造）：

- **D1 记录语境注入依赖边缘 + 可观测性缺口**（事实澄清：引擎侧每会话确实进过语境，「front 从未进入」推断与三源不符——front 缺审计行）。修复 = 每会话无条件幂等注入 + `story_record_ctx_enter` 单一 emit 点（SCRF-B7）。
- **D2 丢弃/会话关闭不掐断在途回合**（最重）：`kidpal_story_discard` 只 `acc.cancel` 不释放语境，随后 `listen stop` 先转发给引擎、`active=false` 迟到，尾音 ASR「嗯。」成立并被 LLM/TTS 回放，设备 `idle -> speaking`（同族复现 22:48:01 / 22:49:59 / 22:53:24）。修复 = discard 分支内同协程释放语境（严格先于转发 stop，不发 abort 帧）+ 会话关闭迟到帧门（SCRF-B8 / SCRF-B9）。
- **D3 空素材会话静置不结束**：`routing==accumulate` 且 `accum` 空 → `_story_guide_round` 恒 `skipped=no_session` → `auto_finish_due_sessions` 永不到期，front 无收尾；设备侧 60s producer 对记录语境豁免 → 常驻 Listening 2 分钟（human 手动退出）。修复 = 设备侧空素材 60s producer（释放、不归档、不推 story_list、不盖章；SCR-A8 源锚 + SCRF-B10 行为）。

**deviceOutcome 状态 = PENDING（轮 B）**：`device-round-evidence.md` **未生成、未伪造 PASS**。SCR-A8 头注已按成对形式申报设备结果（`device-outcome: SCR-A8` + `host-uncompilable-path: firmware/main/application.cc`）；合并门 `pre-merge-escalation-gate` 的 device 判据将要求 `device-round-evidence.md` 对 launch-card 申报的两条 requirements 引用（`docs/requirements.md#S-F1.4` / `#§2.1`）与 SCR-A8 各有 PASS 目击（human-visual / serial-capture / KPLOG 三形态最小要件）。**未 PASS 前不得合并。**

**轮 B 复测清单**（种子 = `device-round-a-evidence.md` §五；口径按修复轮修正）：

1. **B-D1**：故事页 KEY 起会话 → front 审计行 `story_record_ctx_enter`（`src=ctx_story` / `edge=true`）+ 引擎 `进入记录语境`；说普通问句（如「明天是星期几？」）→ 引擎只出 stt 回显、**零 TTS**、设备不 `idle -> speaking`。
2. **B-D2**：录音中说话后 **BOOT 丢弃**（尾音在途）→ 设备回列表并停留 Idle ≥5s；引擎日志无该回合 `大模型收到用户消息` / `发送第一段语音`；front 出现 abort + `active=false` 先于 listen stop 的次序证据。回归：KEY 收尾仍正常归档（A5 录音态 KEY 收尾）。
3. **B-D3（口径修正）**：故事页 KEY 起会话后**完全不说话**，静置 60–70s → 设备自动回列表（非 BOOT 手动）；front 无 `story_list` 推送、无 `story_text`、STORIES_DIR 零新增；**随后 ≥10s 普通问答可应答；或 10s 内首次话语允许被抑制、再按一次即恢复**——pending ≤10s 抑制窗是已冻结 accepted residual，**不得写成「立即」**。
4. **回归**：A5 全链四式、A6 普通问答 60s 静默关闭、A7 连续讲 >90s 不误收。

## 未覆盖面申报（覆盖断言覆盖轴的设计面差集）

> 对照 `acceptance[]` 15 条 + 契约条目 + roadmap §① 效果段，逐项列「本次无验收断言 / 未跑的设计性质 → 现状 → 去向」。**非 `none`**（真机半与长跑形态均未覆盖）。

| # | 设计性质（无断言/未跑） | 现状 | 去向 |
|---|---|---|---|
| 1 | 温通道按键进聆听 ≤0.82s（目的 1） | 未跑：离线无真实 TLS/AFE 时延轴 | 轮 A A1（对照 KPLOG029） |
| 2 | 连续讲 >90s 不被误收（目的 4 反面） | 未跑：离线仅静态断言电平门在场（SCR-A5） | 轮 A A7 |
| 3 | 心跳 30s 真实节奏 + SD 日志心跳痕迹（目的 2） | 未跑：离线注入假时钟（SCRF-B1） | 轮 A A3 |
| 4 | 三方并发写同一 device websocket 无锁（上泵 / sweep `_story_finish_session` / 心跳） | 未测：front 现无 asyncio 发送锁，P1 不新建锁（超范围） | accepted residual：心跳帧极小、单次 await，实际风险低；轮 A 观测间发/丢帧；若需锁另立批 |
| 5 | `IsTimeout()` 日志密度（T4 事实降频是否达成） | 未量测：离线无法计真机日志速率 | 轮 A A3/A12 带量测；若仍 >每轮一条，回报 planner 走扩单（`protocol.cc` + `protocol.h` 双入 manifest） |
| 6 | `_story_timeout_sweep` 10s 窗口后在途抑制的 accepted residual（listen stop 后 <10s 首次话语可能被抑制、再按一次即恢复且 ≤10s 已释放） | 未以断言覆盖为「可接受形态」：B2/B2b 只断言 pending 保护与 +10s 释放，不断言 <10s 语音行为 | accepted residual（KRC-E4 A5 明列「间隔 <10s 允许首次话语被抑制 = accepted residual 但再按一次即恢复」）；轮 A A5 观测 |
| 7 | 心跳 / `listen stop` 迟到下行帧（stt / sentence_start / tts）把人拽回思考/播报 | 未测：通道常驻后四条收尾路径首次获得该行为 | 轮 A 观测项（「结束后 1–2s 内屏幕是否被迟到帧拽回」）；**非 P1 要修的缺陷** |
| 8 | P1 把「3 次退避即止→静默」变事实长跑（每 ≈35s 一轮 TLS 握手、无上限） | 未测：离线逻辑不改，形态在真实死链期才显形 | 轮 A A3/A12 观测；封顶长跑归 P2 SEI-A2/A3（`crossPackageBehaviors` 已登记形态基线） |
| 9 | `auto_connect_in_flight_` 守卫期（≈35s 窗）内结束会话被静默吞（仅 INFO 日志） | 无断言覆盖：守卫为 261024 逐字保留、P2 SEI-A4 重锚 | 残差/观测；P2 重锚为「守卫在场 + 置意向位 + 审计行」 |
| 10 | 引擎 24h 日切为唯一剩余重建窗口（C7） | 未跑：需测试环境临时调小 `close_connection_no_voice_time` | 轮 A A10 |
| 11 | 录音态提醒排队例外（目的 6 / S-F1.4） | 未跑：离线仅 RM-A3 静态 `RaiseReminderQueue` 在场 | 轮 A A8 |
| 12 | 查字窗关闭路径（播报完不回聆听、驻留按键或 5 分钟） | 未跑 | 轮 A A9 |
| 13 | 断网 / front 重启自愈（目的 5，P2 主面；P1 仅巡检雏形） | 未跑 | P2 轮 B（B4/B5）；P1 A3 部分观测 |
| 14 | 录音会话全链四式（KEY 结束无归档 / 间隔 >10s 正常应答 / KEY 收尾归档 / BOOT 丢弃 / 2 轮无回应收尾） | 离线 SCRF-B2/B3 覆盖部分，真机全链未跑 | 轮 A A5 |
| 15 | RC-B6 省电档观测口径重定（决策 A6） | 未跑 | 轮 A A12 |
| 16 | front/固件回滚剧本可执行（两半一起回滚） | 未演练 | 轮 A A13 |

**修复轮新增未覆盖/观察项（编号续上）**：

| # | 设计性质（无断言/未跑） | 现状 | 去向 |
|---|---|---|---|
| 17 | B9 出音令牌 30s deadline 到期路径（`GAP-B9-1`） | 无离线断言：验收覆盖「令牌在场放行整轮含 tts stop」「tts stop 清位」，未覆盖「令牌超 30s 未收 tts stop 时自动过期、门回生效」 | accepted residual（deadline 是兜底而非主路径：正常一轮 TTS 远短于 30s）；轮 B 观测；如需断言另立批补「时钟推进 + 过期」用例 |
| 18 | 门内放行帧仍可能闪屏 | 未测：会话关闭门只丢音频 / tts / llm，`mcp` display 块照常放行；若在途回合附带 display 块且门已 armed，设备可能短暂显示 | accepted residual：B8 修好后该回合不应再产生（在途回合已被语境释放掐断）；轮 B 观测；不扩改 |
| 19 | SCRF-B10「零归档 md」断言的离线判别力有限 | 无 LLM stub 时该断言不能单独抓红；实际由「零演出」（`_issue_finish_downlink` 先于 LLM）断言承担判别力（变异 #18 实测） | 如实申报，不伪造判别力；轮 B 用 STORIES_DIR 零新增三源对账补强 |
| 20 | SCRF-B10 `start_organizing` 断言仍只观测外层 `dev` | 观测面与已闭合的 `story_list` 同类形状；空素材释放路径本不下发该帧，本轮不扩改 | 验收质量观察项；后续专批或 P2 收紧 |
| 21 | SCR-A8 只钉源锚代理（结构） | 运行时行为（空素材静置 ~60s 自动回列表、零 story_list、随后问答恢复）host 不可编 | 轮 B B-D3（三源对账）；device-outcome 已申报，未 PASS 前不得合并 |
| 22 | `_consume_points_intent` 的去重语音直注不经 `_speak_to_legs` | 未覆盖：该路径直接注入引擎腿、不登记出音令牌——会话关闭门 armed 时该语音可能被当迟到回合丢弃（冻结方案只列 `_speak_to_legs` 与 `_deliver_alert_now` 两处令牌点） | accepted residual（最坏 = 一次积分语音被丢，不影响积分入账与 UI 帧）；轮 B 观测；如真机复现另立批补第三登记点 |
| 23 | 变异灵敏度两处观察项 | 变异 #3 token **超串**改名不转红（计数/子串匹配特性）；变异 #15 注释不含 ASCII `;` 不转红（SCR-A5 交叉负锚只对含分号注释成立） | 验收质量 NIT，登记留痕；不影响本轮判别力总量（非超串改名与含分号注释均转红） |
| 24 | 会话关闭门 arm 沿 `legs` 传播（多连接场景） | 未测：`_release_record_context` 对传入 legs 逐腿 arm；sweep 的腿集是 `_session_legs(sid) or list(_alert_legs)`，会话腿已摘除时 fallback 到全局提醒腿 → 多连接部署下其他在线设备也会被 arm（各自下次 listen start / ctx(story) disarm） | 风险低（单设备部署无感；disarm 由该设备下一会话开始自动完成）；轮 B 观测；多连接场景若暴露另立批把 arm 收到会话归属腿 |

## 回滚路径与 Merge Packet

- **原子可回滚**：P1 实现/验收/簿记改动落在本地 feature 分支 `dev/pool-channel` 的单个收口 commit（31 文件 / +4300 −370；hash 见本包 git log，`8a7508ad` 起的收口链）。回滚 = `git revert <该收口 commit>`（或合并前 `git reset --hard` 至父提交），一步回到 P1 前状态。
- **修复轮 commit 链（2026-11-18，本轮）**：修复轮改动落单 commit（commit message 含「261116-channel-resident-core 修复轮 final r5」），父 = P1 收口链 HEAD。回滚 = `git revert <本轮修复轮 commit>`（或合并前 `git reset --hard` 到其父提交）——回滚后回到「P1 已收口、轮 A 缺陷未修」状态，`device-round-a-evidence.md` 与 `mutation-evidence/` 等证据件随该 commit 一并移除（同批回滚口径）。
- **两半一起回滚（承重）**：front 心跳默认开、设备侧升级为「120s 无心跳 = 死链」；只回滚单侧会让设备退化 churn 或空转。故代码回滚必须 front（`front_app.py`）与固件（`application.cc`/`application.h`）同批；紧急单侧回滚可用 `KIDPAL_CHANNEL_HEARTBEAT=0` 关 front 心跳（回滚开关），但同时须确保设备侧 heartbeat no-op 分支仍在（新固件收到 keepalive 仅一条 DEBUG 或无输出，无副作用）。**修复轮同口径**：D2 的语境释放与 B9 迟到帧门只存在于 front，D3 producer 只存在于固件；两半一起回滚 = revert 本轮单 commit（前端单侧回滚会让设备侧空素材 producer 与 front 收尾链不配对）。
- **merge packet**：本地合并 = planner 在 supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后 `merge-driver --pod`，integrator 不 push、不 merge 主干。合并前排他：P2 从新 main 起分支、重取行号锚（见 `crossPackageHandoff`）。
- **INDEX 投影**：integrator 相位以只读跑（`indexProjection: skipped-read-only`；`changes-index.mjs --upsert` 是 review-lane 专用、对 dev id 报命名错）；**planner 收尾相位已用 `node .claude/scripts/closeout-check.mjs --update-index` 补写本包行**（`delivery/taskpackages/INDEX.md:120`），并随 `cc6f6667` 提交。

## 合并机械项（2026-11-18 收尾复跑 `pre-merge-escalation-gate` 新增）

`node .claude/scripts/pre-merge-escalation-gate.mjs 261116-channel-resident-core --source-ref HEAD` BLOCK 四因：`device-round-evidence-missing`、`parallel-overlap-tier3`、`riskTier=high`、`supervisor verdict=ESCALATE`。

其中 `parallel-overlap-tier3` 为本轮新发现：

- **实撞 = `delivery/workstreams/INDEX.md`**（生成件，头注「勿手改」；渲染器 = `node .claude/scripts/planning-index.mjs`）。`git merge-tree` 的唯一冲突 hunk = 头部「最后更新 <date>」一行；我方 workstream 行与 `Active (6)→(7)` 已被 git 自动并。
- **architect §10d ③ 消解评审**（dispatch `a7c1ad6c73ea2da9f`，advisory）：判 **②（无取舍/生成件）**，非 ③；建议根治 = `.gitattributes` 加 `delivery/workstreams/INDEX.md merge=union`（同批裁 `index.jsonl`），落地后闸自然 `union-safe`。诊断见 `gate-tooling-gap-overlap-index.md`。
- **人批豁免不了本条**：`parallel-overlap-tier3` 不在闸的 `escalateExempt` 覆盖内；合并动作前必须先让冲突消失（union 条目 或 主线程 rebase/merge 消解 + 重跑 `planning-index.mjs` 使 main 成为祖先）。
- 本项不构成独立停等理由（实质停等 = 轮 B 真机 + riskTier high ESCALATE + G4），但它是合并动作的前置机械项。

## Residual Risk

- **Known limitation**：
  1. 用户可见效果段仍未闭合：真机轮 A 已发生且 **FAILED**（D1/D2/D3，诊断轮），修复后只有离线结构/单元证据；时延 / 连续讲 / 自愈 / 提醒 / 60s 自动关闭 / 空素材自动收尾的效果判据待**轮 B**。
  2. 心跳三方并发写无锁（未覆盖面 #4）为 accepted residual。
  3. `_story_timeout_sweep` 10s 窗口内在途抑制（未覆盖面 #6）与 B9 出音令牌 30s 到期路径（#17）为 accepted residual。
  4. 会话关闭门内放行的 display 块可能闪屏（#18）；`_consume_points_intent` 直注语音可能被门丢（#22）；多连接下 gate arm 沿 legs 传播（#24）。
  5. `IsTimeout()` 日志密度与「3 次退避变长跑」形态变更（未覆盖面 #5/#8）待真机量测/观测。
  6. `auto_connect_in_flight_` 守卫期结束会话被静默吞（未覆盖面 #9），归 P2 SEI-A4 重锚。
  7. SCR-A8 只钉源锚代理（#21）；SCRF-B10 的「零归档 md」判别力有限、`start_organizing` 观测面未扩（#19/#20）。
- **Follow-up recommended**：
  1. **真机轮 B 复测**（B-D1/B-D2/B-D3 + A5/A6/A7 回归）作为本包合并出口；证据落 `device-round-evidence.md`（须含 SCR-A8 与 launch-card 两条 requirements 引用的 PASS 目击 + 目击固件 `sourceCommit` / `binarySha256`）。
  2. 轮 B PASS 后由主线程按真机凭据结项 `docs/debt/tech/TECH-261114`（A1+A2）与 `TECH-261012`（A3）并 `debt-render.mjs` 归位；本轮两条维持 `deferred`。
  3. **spec 漂移**：requirements §2.1「记录会话不适用 60s 会话超时」的收窄（D3）由主线程按需求面轻量注记补对账句；本 Pod 零改 spec，`docsSyncRequired=false` 与实际影响不一致已申报，不写成「已同步」。
  4. 变异灵敏度观察项（超串改名 / 注释 ASCII 分号）后续收紧断言时处理。
  5. P2（`261117-session-end-intent-longretry`）从新 main 起分支、重取行号锚，接 `crossPackageBehaviors` 五项（含 SCR-A2 in-flight 守卫重锚、`mcp_server.cc` 注释双源）。
- **Human decision needed**：
  1. **真机轮 B（human 持板）**——本包合并的唯一产出出口；未 PASS 前不得合并、不得结项两条 debt。
  2. **G4 工具缺口（仍拦收尾）**：`supervisor-gate-check.mjs` 的 G4 要求 `.claude/verify-logs/*.log` 非空，但 `verify.mjs` 仅在 gate 失败时写日志——全绿 Pod 零日志 → G4 结构性恒红。诊断与 patch 见 `gate-tooling-gap-g4.md`；待裁清单见 `pending-human-ruling.md`。`.claude/**` 对 agent 禁写（`scope-map.universal.bashDenyWrite`），planner 无权自修。本包合并待 G4 落地 + 轮 B 复测。
  3. 回滚剧本执行口径（A13，两半一起回滚）由 human 在轮 B 确认可执行。
- **decisionCoverage 残差**：本包 8 条 `decisionCoverage` 全部 `landsIn = exitCriteria:KRC-E1/E3/E4`，**无 `landsIn: dropped` + `reason` 以 `risk-accepted` 开头的条目**，故本表无 human 已接受的 risk-accepted dropped 项。
- **跨面变更（spine 在册）**：本包实态 = 离线全绿（final r5 十闸）、合并门仍被轮 B 与 G4 拦；建议退出 status = `ready-to-merge` 仅在轮 B PASS + G4 落地后成立，workstream `261115-session-channel-resident` P1 锚点更新归主线程（integrator 不直改 spine）。

## 真机轮 B 修复轮（final r7，2026-11-18；本节为当前态，历史轮次行保留不删）

> 覆盖上文「2026-11-18 更新（修复轮）」的状态口径：那时是轮 A 修复轮收口（final r5、20 条验收、deviceOutcome = PENDING 轮 B）。轮 B 已发生并部分失败（D3 / A6），按 human 2026-11-18「按 B 修」进入本轮判据改造；本轮离线面收口于 **final r7 十闸全绿**，验收累计 **27 条**。**真机轮 C 尚未发生，本包不伪造任何真机 PASS**。

### 本轮 Change Size

- **本轮 Files changed = 7 件（4 修改 + 3 新增）**，全部在固件侧；`git diff --numstat HEAD -- firmware server`：`firmware/main/application.cc` +144/−25、`firmware/main/CMakeLists.txt` +7/−0、`firmware/main/test/host_app/main/CMakeLists.txt` +9/−0、`firmware/main/test/session_channel_resident.acceptance.c` +1019/−54；新增 `firmware/main/session_voice_watch.c`（155 行）、`.h`（132 行）、`firmware/main/test/session_voice_watch.test.c`（340 行）。合计 +1179/−79 与 627 行新件。
- **零 diff 面（实测）**：`server/gateway/front/front_app.py`、`server/gateway/story.py`、`display_schema.py`、`firmware/main/mcp_server.cc`、`firmware/main/application.h`、`protocol.cc` / `protocol.h`、`story_page.c`、`server/deploy/**`、`engine-patch/**`、`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/**` 全部零 diff。front 与引擎零改动意味着**回滚 = 只 revert 固件侧**。
- **manifest**：planner 扩单 3 件（`session_voice_watch.c` / `.h` / `firmware/main/CMakeLists.txt`）+ builder 单测 `session_voice_watch.test.c` 回填，`changeManifest[]` 现 15 件；对账面 7 件、清单外 0 件（planner 手工对账，见 §gate-tooling gaps 第 3 条）。

### 一、真机轮 B 结果与轮 C 复测清单

**轮 B 实测（2026-11-18，human 持板；证据 = `device-evidence/round-b/kplog-excerpt.txt`，源 `KPLOG000.TXT` 全量 1,713,125 字节 / sha256 `21599cbc296db16e3deeb61ebbfeaeec5d4fb1a6ee472392491a141ecb086188`）**：

| 项 | 结果 | 事实 |
|---|---|---|
| D1 记录语境注入 + 审计行 | 通过 | front 每会话注入 + `story_record_ctx_enter` 审计在场 |
| D2 丢弃掐断在途回合 | 通过 | 丢弃后设备回列表、无尾音 TTS 回放 |
| A5 归档 | 通过 | 07:14:08 `stop_rec archive_pend=1` → `story_sd_write bytes=133` → `story_saved_stamp seq=32` → `story_list_dispatch items=16` |
| **D3 空素材会话自动收尾** | **失败** | 07:15:03 `listen start` → 07:17:43 手动丢弃（**2m41s**），无 `story_empty_session_timeout` |
| **A6 普通会话静置关麦** | **回归失败** | 07:18:23 `idle -> listening` → 07:21:36 手动 `overlay_close`（**3m13s**），无 `session_idle_timeout`；A6 producer 本轮一字未动 |

**planner 只读定位（设备日志，新证据）**：D3 的根因不是电平门。每次**故事**会话进入聆听后 0.25–0.7 秒内必出一条 `MCP: story_vad_recording_enter consumed=1 page=2`（本窗口 07:14:30 / 07:15:04；全量 12 条均落在会话起步 1 秒内），唯一发射点 = `application.cc` 的 `KidpalStoryVadEnterRecording()`，前置条件含 `s_vad_speaking == 1`——即 **VAD 在无人说话时也让出一记 speech 沿**。该沿把 `s_story_rec_entered` 置真，而它正是 D3 producer 的「从未收到素材」判据，**置真后该会话对 D3 producer 永久不可达，与电平其后是否回落无关**；同一沿还让记录会话 0.7 秒内从列表页跳到 RECORDING 录音页。A6 窗内零状态迁移（计时未被状态迁移清零），但设备当时不打印 VAD 电平，M1（电平恒真）与 M2（起音沿周期性清零计时）在日志里同构——这正是 human 要求补低频 listening 诊断行的直接理由。

**轮 C 复测清单（至少项）**：

1. **A6**：普通会话静置 60s 自动关麦（实测窗内 `quiet=` 到 60 时设备是否 `listening -> idle`，无手动按键）。
2. **A7**：连续讲 >90s 不误收（全程有转写；关注 `asr=` 最大值与 `run=` 走势）。
3. **D3**：故事页起会话、全程不说话，静置 60s 自动释放回列表；**不归档、无 `story_list`、无 `story_text`**（STORIES_DIR 零新增）。
4. **A5 回归**：有素材的记录会话归档链正常（`story_sd_write` → `story_saved_stamp` → `story_list_dispatch`）。
5. **采集项**：
   - **C-1** 两条 producer 审计行的 `suspect=` / `quiet=` 必采（判本轮是否命中真实失效模式 M1/M2/M3）。
   - **C-2** A7 窗内 `asr=` 列最大值（标定 `KIDPAL_VW_ASR_FRESH_S=25` 是否合理）。
   - **C-3** home（非记录语境）会话 `rec_ctx=` 恒 0（M4 记录语境镜像卡死排查）。
   - **C-4** 静置窗 `run=` 是否反复归零（M2 起音沿风暴排查）。
   - **C-5** 日志落盘增量与诊断行预算对账（单行约 190 字节、Listening 态最坏约 3.8 KB/分钟）。
   - **C-6** D3 窗同时满足 `rec_ctx=1 && asr_seen=0 && quiet 到 60` —— 新素材信号 `asr_seen` 的真机承载。

### 二、本轮改动面与判据

**新缝 `session_voice_watch.{c,h}`（判据本体，host 可编）**：零芯片依赖纯逻辑 C 源（不 include `esp_*` / FreeRTOS / LVGL，不读真实时钟），设备树与 host 树编同一份 `.c`，行为断言在 host 上真跑。状态七字段（`quiet_s` / `level_run_s` / `asr_age_s` / `tick_s` / `diag_last_s` / `suspect` / `asr_seen`）由调用方持有，缝内零全局可变态。暴露 `cfg_default` / `reset` / `reset_soft` / `note_asr` / `tick` / `diag_due` 六个函数。

**`application.cc` 七处接线**（源锚由 SCR-A15 逐条钉）：

1. 状态迁移监听器写旧状态快照 `s_watch_prev_state`（`DeviceState` 标量，初值 `kDeviceStateIdle`）。
2. `Initialize()` 里 `kidpal_voice_watch_cfg_default(...)` + `idle_timeout_s = kSessionIdleTimeoutS`——**唯一阈值注入点**（两侧各写一个 60 = 两个阈值各自漂移）。
3. `CLOCK_TICK` 区 `kidpal_voice_watch_tick(...)` 每 tick 恰 1，并附低频 `listening_diag` 诊断行（周期 10s + 跳变，最小间隔 3s 封顶，模数走缝自持 `tick_s`，区内不出现 `clock_ticks_ % 10`）。
4. A6 producer 判据换为 `Listening && !记录语境 && vw_act == KVW_ACT_IDLE_FIRE`，审计行追加 `quiet=` / `asr=` / `suspect=` / `src=idle`。
5. D3 producer 素材门换为缝内 `asr_seen`，静置判据换为 `quiet_s >= kStoryEmptySessionTimeoutS`，审计行追加同四件 + `src=empty_story`。
6. `stt` 分支 lambda 内 `kidpal_voice_watch_note_asr(...)` 恰 1（主循环上下文，与 tick / reset 同写者，零锁）。
7. `HandleStateChangedEvent` 的 Listening case 尾部：按旧状态分派 `reset_soft`（旧状态 == `kDeviceStateSpeaking`）或 `reset`（新会话），并 `s_vad_speaking = false`（M3 卫生复位），附会话入口诊断行 `src=enter`。

**两侧 CMakeLists 登记**：`firmware/main/CMakeLists.txt` additive 一行（设备树）；`host_app/main/CMakeLists.txt` 登记缝源与 builder 单测，**必须置于 host smoke 之前**（smoke 末位 = 最先执行，注册序守卫）。

**判据与阈值（保守档）**：`KIDPAL_VW_VOICE_SUSTAIN_S=3`（短于 3 秒的连续段按瞬态噪声不采信 = M2 解药；真话语由 asr_fresh 兜住）、`KIDPAL_VW_ASR_FRESH_S=25`（引擎证据新鲜窗）、`KIDPAL_VW_VAD_TRUST_S=100`（**严格大于 A7 的 90 秒界**，留 10s 余量；连续电平超过它即视为卡死不再采信 = M1/M3 解药）、`DIAG_INTERVAL_S=10` / `DIAG_MIN_GAP_S=3` / `ASR_AGE_SAT=999`。逐场景结果：纯静默恰 60s 收尾；M1 电平卡死 `suspect=1`、最迟 100+60=160s 有界收尾；M2 起音沿风暴尖峰不采信、仍恰 60s；M3 闩锁由 Listening 入口复位掐掉、残余 ≤160s；A7 有 stt 时永不误收、零 stt 时 ≤90s 绝不收（160s 才收）。

**设计取舍定理（本方案地基）**：只用「VAD 电平 + 时间」两个量，**不可能**同时满足 A7（连续电平 ≥90s 不收尾）与「电平卡死 ≤70s 收尾」——两种情形在信号层面逐 tick 同构。必须引入第三路证据（引擎 stt 到达时刻）并接受其缺失时的降级（可信窗内先采信、窗过期有界收尾）。

### 三、D3 素材门换源

旧判据 `!s_story_rec_entered` 已被轮 B 日志证明会被起步噪声沿提前置真 → 真机上结构性不可达。本轮素材门换为缝内 `asr_seen`（`kidpal_voice_watch_note_asr()` 置 1、`reset()` 清 0、`reset_soft()` 保留），语义 = 「本会话引擎真的转写出一句话」，与 front 累积器的素材来源**同源**（front 的 accum 也只吃 `stt` 文本）——设备与网关对「有没有素材」不再可能判断不一致，ASR 失败时两侧同时判无素材、同时释放不归档。`s_story_rec_entered` 保留原用途（录音页切换 + 幂等），**不进 producer 判据**；起步噪声沿导致的无端跳录音页登记为 residual（不阻塞，D3 收尾后回列表）。

### 四、白盒复核补丁（planner 2026-11-18，builder 首版之后）

builder 首版把 `kidpal_voice_watch_reset(...)` 无条件落在 Listening case 尾部（每次进聆听全清七字段）。planner 复核发现高风险回归：记录会话里**引导播报 TTS** 会让设备 `Listening -> Speaking -> Listening`，回到聆听时那次全清会把 `asr_seen`（D3 空素材 producer 的唯一素材门）清 0 → 一个**有素材**、孩子已讲过话的故事会话，在引导播报后静置 60 秒会被判成「空素材」并释放，**front 侧该 `listen stop` 不归档**（孩子已讲内容丢失）。

修法（seam 与接线各一处，均 additive）：① 缝新增 `kidpal_voice_watch_reset_soft()`——清 `quiet_s / level_run_s / asr_age_s / tick_s / diag_last_s / suspect` 六件、**保留 `asr_seen`**；② 状态迁移监听器存旧状态，Listening 入口按迁移来源分派（旧状态 == `kDeviceStateSpeaking` 走软复位，否则全清）。验收收紧：SCR-A13 追加「`reset_soft` 后 `asr_seen` 仍为 1、其余六件全 0；其后 `reset()` 仍七字段全清」；SCR-A15 追加 Listening 区三件（`reset_soft(&s_voice_watch)` ≥1、`reset(&s_voice_watch)` ≥1、`kDeviceStateSpeaking` ≥1）。红基线 320/5（仅 SCR-A13 / SCR-A15 两条设计态红）。教训登记 pod-retro：「会话入口复位」必须区分「新会话」与「会话内状态往返」。

### 五、Verification Evidence（本轮，承重）

- **Final round fact（承重）**：`delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r7.json`，`phase=final / round=7 / ok=true / observedGateExit=0 / invocationId c5add419-3690-4d81-bd17-b5e4e7d642dd`，10 闸全跑。
- **机械凭据闸**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 7 --invocation-id c5add419-3690-4d81-bd17-b5e4e7d642dd` → `exit 0 / factOk=true / phase=final / round=7`。
- **闸表**：`taskpackage-required` / `change-manifest`（pre-cutoff 存量放行，见 gate-tooling gaps 第 3 条）/ `contract-lint`（1 条既有相对链接 warning）/ `acceptance-coverage`（27 acceptance id 全覆盖）/ `mutation-check`（27 条 per-acceptance 变异 evidence 齐）/ `docs-sync` / `security` 七闸 pass；`firmware-build` `build-pass`（增量 5 步，源未变，`xiaozhi.bin` 0x329580 bytes / app 分区 8% free）；`unit`（full，scope 求交）**247 passed / 3 deselected / 22 subtests**；`acceptance`（full，scope 求交）**195 passed / 3 deselected / 3 subtests**，surface = server 14 / firmware 12。
- **Host runner（integrator 亲跑 @ working tree，执行目录 `firmware/main/test/host_app`）**：`321 Tests / 3 Failures / 0 Ignored`——3 红全为上游存量 `p1_auxbar_style.acceptance.c` AB-A1 / AB-A2 / AB-A4（`ic_batt_15` / `ic_thermo_12` / `ic_wifi_14`），与本包无调用关系；本包 SCR-A1..A15 与 builder 单测全绿，零新增红。
- **Scope 圈定（本轮 r7，范围圈定清单落盘路径一行）**：`delivery/taskpackages/261116-channel-resident-core/test-scope-final.txt`（**30 条**，覆盖写，相对 r5 只增不减）。合成 = `acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（19 去重）∪ `detect_changes`（project `C-AI-KidPal-pod-channel`，since main / merge_base `73f43637`，inbound depth 2：**68 changed files**（含本包 delivery/docs 簿记件；实现/验收面 14 件）/ **seed 445 符号** / **impacted_total 304**，其中 firmware/main 217、server/acceptance 73、server/gateway 10、server/tests 4）映射测试 ∪ 共享缝相关测试 ∪ 安全余量。新增项 = builder 单测 `session_voice_watch.test.c`（新文件）与 `font_assets_runtime.acceptance.c`（`firmware/main/CMakeLists.txt` 入改动面，其 FAS 块读该文件）。per-Pod 专属图索引可用 → **正常圈定、未降级**；不缩到本包档、不默认全量。**收口 commit `f368926c` 后复核**（Step 9b advisory 记账）：detect_changes 69 changed files / seed 442 符号 / impacted_total **110** 节点（server/acceptance 73、firmware/main 23、server/gateway 10、server/tests 4）——与圈定时 304 节点的膨胀面（图索引刷新过程态、firmware/main 面 217 个 board 文件节点）指向同一测试面，实现面符号影响无外溢。
- **变异证据**：本轮 13 条（`mutation-evidence/round-b-fix.md`），含 kill-shot #2（删可信窗上界 = 轮 B 缺陷形态，SCR-A10 转红）、A7 承重 #6（`VAD_TRUST_S` 100→30，SCR-A12 转红）、电平门回潮 #10（A6 判据加回 `&& !s_vad_speaking`，SCR-A15④ + SCR-A5 双红）、软复位退化 #12、条件分派删除 #13。

### 六、未覆盖面申报（覆盖轴差集）

> 逐条列「本次无验收断言 / 未跑的设计性质 → 现状 → 去向」，零缺口不成立，故不写 `none`。

| # | 设计性质（无断言/未跑） | 现状 | 去向 |
|---|---|---|---|
| B1 | 真机端到端：A6 静置 60s 关麦、A7 连续讲 >90s 不误收、D3 空素材 60s 释放、A5 归档回归 | 未跑：轮 B 已 FAILED（D3/A6），本轮修复后需轮 C 复测；`device-round-evidence.md` 未生成 | 轮 C（human 持板）；证据落 `device-round-evidence.md`，未 PASS 前不得合并 |
| B2 | `s_vad_speaking` 的真实电平分布与 `run=` 时序（M1 还是 M2 占主导） | 完全未观测：设备此前不打印 VAD 电平；本轮新增 `listening_diag` 才第一次可采 | 轮 C 采集项 C-1 / C-4；若 160s 有界收尾在真实 M1 下不可忍，另立快速后续批 |
| B3 | M4 记录语境镜像卡死（`kidpal_story_session_ctx_active()` 残留 → A6 被硬豁免、D3 又不满足素材门） | 未修：置位单点在 `mcp_server.cc`（P2 边界、manifest 外）；本轮只由 `rec_ctx=` 字段使其可归因 | 轮 C 采集项 C-3 确认是否发生；修复归 P2 / 另立批 |
| B4 | M3 根源：`AfeAudioProcessor::Stop()` 不复位 `is_speaking_` | 未修：判据对 M3 已鲁棒，并由 Listening 入口 `s_vad_speaking=false` + 缝复位兜住；根治要连带决定「是否补发一次 offset 回调」（影响 `voice_detected_` → LED） | 独立后续批（需自验的语义变更） |
| B5 | TTS 回声被 ASR 转写成 stt 的自回声残差 | 未测：记录会话的引导播报若被设备自身麦克风拾取并被引擎转写为 stt，会把 `asr_seen` 置 1 → D3 对「孩子其实没说话」的会话判成有素材、静置不释放（记录语境下 A6 被硬豁免，无第二兜底） | accepted residual：真实回声抑制由 AFE/AEC 承担；轮 C 观察 D3 窗是否出现 `asr_seen=1` 而窗外无 `>>` 转写的组合；如复现，另立批把素材门收窄到非引导窗 |
| B6 | 起步噪声沿导致的无端跳录音页（D3 判据已换源，页面副作用仍在） | 未修：`s_story_rec_entered` 保留原用途 | 轮 C 观察；根治归 `AfeAudioProcessor` 卫生复位后续批 |
| B7 | 诊断行真实日志密度与轮转预算 | 未量测：单行约 190 字节、最坏约 3.8 KB/分钟为推算 | 轮 C 采集项 C-5 实测对账 |
| B8 | 设备构建 depfile 时序（新头首次引入不触发验收 TU 重编） | 已知缺口：头宏变异须「先删 `.o` 强制重编」才有判别力 | 既有构建卫生问题，交主线程；本包变异证据已按强制重编口径执行并如实申报 |
| B9 | `change-manifest` 闸对账未真正执行（恒判 pre-cutoff） | 已知缺口：planner 手工逐条对账，清单外 0 件 | `gate-tooling-gap-manifest-cutoff.md`，修复面 `.claude/**`、归主线程 |
| B10 | 变异观察项：头宏强制重编口径、co-red 申报、builder 自身读写缺陷已修正 | 如实申报，不影响判别力总量 | 见 `mutation-evidence/round-b-fix.md` §三 |

### 七、Per-Acceptance 证据 · 轮 B 修复轮追加（7 条；全包累计 27 条）

| Acceptance | 断言载体（测试） | 结果 | 变异判别力 |
|---|---|---|---|
| SCR-A5（重锚）60s producer + 说话沿重置 | `session_channel_resident.acceptance.c` TEST_CASE「60s超时producer+说话沿重置 (SCR-A5)」：判据 token 换为缝 tick 返回值与 `src=idle` 审计字段，VAD 说话沿重置锚保留 | PASS | 变异 #10（A6 判据加回电平门）→ SCR-A5 与 SCR-A15④ 双红 |
| SCR-A8（重锚）空素材空闲收尾 producer | 同件 TEST_CASE「空素材空闲收尾producer (SCR-A8)」：素材门读缝 `asr_seen`，区内 `!s_story_rec_entered` 恰 0 | PASS | 变异 #2（删可信窗上界）→ SCR-A10 红（kill-shot，等价轮 B 缺陷形态）；#10 同区 |
| SCR-A9 静默 60 秒恰 tick 60 收尾 | 同件 TEST_CASE「静默60秒恰tick60收尾 (SCR-A9)」（真行为） | PASS | 变异 #1（收尾比较 `>=` → `>`）→ 转红 |
| SCR-A10 电平卡死有界收尾（kill-shot） | 同件 TEST_CASE「电平卡死有界收尾 (SCR-A10)」：400 tick 恒电平零 stt → tick 1..159 零 fire、tick 160 恰 fire 且 `quiet_s=60 / level_run_s=160 / suspect=1` | PASS | 变异 #2（删可信窗上界）→ 转红；#6（`VAD_TRUST_S` 100→30）→ 同向 |
| SCR-A11 起音沿风暴 vs 短暴发 | 同件 TEST_CASE「起音沿风暴vs短暴发 (SCR-A11)」：单秒尖峰 → 恰 60 收尾；4 秒连续段 → 200 tick 零 fire 且 `quiet_s <= 3` | PASS | 变异 #3（下界改 1）/ #4（`VOICE_SUSTAIN_S` 3→5）→ 转红 |
| SCR-A12 A7 连续讲话零误收（承重） | 同件 TEST_CASE「A7连续讲话零误收 (SCR-A12)」：有 stt 200 tick 零 fire；零 stt 150 tick 零 fire | PASS | 变异 #5（去掉 asr_fresh 臂）/ #6（`VAD_TRUST_S` 100→30）→ 转红 |
| SCR-A13 跨会话零残留 + 软复位六清一保 | 同件 TEST_CASE「reset清零与note_asr契约 (SCR-A13)」 | PASS | 变异 #8（删 `reset` 的 `asr_seen=0`）/ #12（`reset_soft` 退化成全清）→ 转红 |
| SCR-A14 诊断限频决策表 + 结构半 | 同件 TEST_CASE「诊断限频决策表+结构 (SCR-A14)」：`diag_due` 四支 + `listening_diag` 六字段 + 区内 `clock_ticks_ % 10` 恰 0 | PASS | 变异 #7（`DIAG_MIN_GAP_S` 3→0）/ #11（诊断触发改 `clock_ticks_ % 10 == 0`，三处同红） |
| SCR-A15 应用层接线六件（结构） | 同件 TEST_CASE「语音看门缝接线六件 (SCR-A15)」：①..⑥ + 附头宏六字面与 seam 源进设备构建 | PASS | 变异 #9（删 `note_asr`）/ #10 / #13（删条件分派）→ 转红 |

> 说明：SCR-A9..A15 的 PASS 来自 host runner（integrator 亲跑 321/3/0）；`verify.mjs` 的 `acceptance` 闸对固件面只作结构与 runner 存在性核对，on-target 归轮 C。

### 八、Demo 对账行

| 证据形态 | 实际结果 | 一致性结论 |
|---|---|---|
| host runner（设备 C 验收，真行为 + 结构） | `321 Tests / 3 Failures / 0 Ignored`（3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4）；SCR-A9..A14 以注入 tick 序列真跑判据本体，SCR-A15 钉接线 | 与「判据语义已离线可判、零新增红」一致；**不足以单独证明**用户可见效果 |
| server pytest（front 验收） | r7 scope 求交 `unit 247 passed / acceptance 195 passed`，零失败 | 与「本轮 front 零 diff、既有冻结 oracle 全保绿」一致 |
| device build | `firmware-build` 闸 `build-pass`（`xiaozhi.bin` 0x329580 bytes，增量 5 步、源未变） | 与「编译面全绿」一致 |
| 真机（三源） | 轮 A = FAILED 诊断轮；轮 B = 部分通过（D1/D2/A5 过，D3/A6 失败）；**轮 C 待跑，未 PASS**；`device-round-evidence.md` 未生成 | **本包出口未闭合**：roadmap 目的 1/2/4/6 的效果段仍未取得 PASS 目击 |

### 九、device-outcome 申报范围收窄

SCR-A8 头注的 `device-outcome: SCR-A8` + `host-uncompilable-path: firmware/main/application.cc` 成对申报**保留**，但范围收窄为「producer 接线与主循环运行时」——判据本体已迁入 host 可编缝并由 SCR-A9..A14 以真行为断言承载。合并门 `pre-merge-escalation-gate` 仍要求 `device-round-evidence.md` 对 launch-card 申报的 requirements 引用与 SCR-A8 各有 PASS 目击。

### 十、变异证据与 gate-tooling gaps

- **变异证据指针**：`mutation-evidence/round-a-fix.md`（轮 A，19 条）、`mutation-evidence/round-b-fix.md`（轮 B，13 条：kill-shot #2、A7 承重 #6、电平门回潮 #10、软复位退化 #12、条件分派删除 #13；4 条如实观察项）。
- **gate-tooling gaps**：① G4（`.claude/verify-logs` 结构性恒红，`gate-tooling-gap-g4.md`，既有）；② `change-manifest` 经 `current/` 指针恒判 pre-cutoff（`gate-tooling-gap-manifest-cutoff.md`，新，planner 已手工对账）；③ 构建 depfile 时序（新头首次引入不触发验收 TU 重编，record 于 `mutation-evidence/round-b-fix.md` §三.1，本 review-packet 未覆盖面 B8）。

### 十一、Five-Axis Review（本轮，author 语义 review）

**1. Correctness**
- 判据方向正确：`quiet_s = 距上次有效语音证据的秒数`，电平只作连续段时长输入、不再作收尾必要条件；两条证据互补（可信窗内电平 / stt 新鲜且本会话确实收到过 stt），M1/M2/M3 三条失效路径都能走到收尾且都有界（≤160s）。
- `asr_seen` 门不可省：`reset` 后 `asr_age_s` 归零，缺此门则前 25 秒恒 fresh，会把纯静默会话的 60s 收尾拖到 85s——builder 单测与 SCR-A13 都钉住该门。
- 会话入口分派正确：`reset_soft` 六清一保（保 `asr_seen`）对应「Listening → Speaking → Listening」往返，`reset` 七字段全清对应新会话；两种复位语义不可互换，SCR-A13 与 SCR-A15 双向钉住。
- `s_vad_speaking = false` 落在 Listening 入口 = M3 闩锁的卫生复位；写者仍在 VAD 回调任务，主循环读，32 位标量与既有同级宽松度一致（无新竞态面）。
- 诊断行模数走缝自持 `tick_s`，`application.cc` 物理上不出现 `clock_ticks_ % 10`，与 DLG-A4⑤ 负锚双保险不冲突。

**2. Test quality**
- 判据本体以真行为断言承载：SCR-A9（静默恰 60）、A10（卡死有界 160 + `suspect=1`，kill-shot）、A11（风暴 vs 短暴发成对）、A12（A7 成对）、A13（复位契约）、A14（限频决策表）；结构断言只覆盖芯片面接线（SCR-A15）。
- 变异 13 条逐条「精确改坏 → 重建 → 实跑 → 全文写回 + sha256 比对」，含轮 B 缺陷的唯一离线警报（#2）。头宏变异采用强制重编口径，已如实申报（见未覆盖面 B8）。
- 观察项：co-red 分布（#4/#6 同打 SCR-A13 的 cfg_default 三阈值断言；#11 三处同红）属预期同向，不作独立判别力计。

**3. Contract / boundary safety**
- `docs/contracts/**` 零 diff；本包不新增帧、不扩 display 白名单、无 `SPEC_CONTRACT_DRIFT`。
- `mcp_server.cc` / `application.h` / `story.py` / `display_schema.py` / `protocol.*` / `story_page.c` / `engine-patch/**` / `server/deploy/**` 零 diff（实测）。
- 新缝是纯本地判据，不消费跨端协议、不读契约载荷。

**4. Operability / security**
- 低频 `listening_diag` 是 human 必办项：`ESP_LOGI`（DEBUG 默认不落盘，本轮不再吃这个亏），字段十件 `src/state/vad/run/quiet/asr/ticks/rec_ctx/rec_mat/suspect`，触发 = Listening 态每 10s + 跳变（最小间隔 3s 封顶），非 Listening 态零输出；不加运行期开关（诊断行的价值就在真机窗）。轮转预算单行约 190 字节、最坏约 3.8 KB/分钟（16MB 封顶约 70 小时 Listening），远低于被消掉的 `Unknown message type` WARN。
- 两条 producer 审计行保留 `seconds=`（与轮 A 对账链不断）并追加 `quiet=` / `asr=` / `suspect=` / `src=`；`local_close_settle` 既有四 token 零改（OCE-A1 钉住）。
- `securityReviewRequired=false`：无新 secret、无权限面变更、无新增外部输入解析（缝只吃整数 tick 与电平）。

**5. Maintainability / simplicity**
- 判据与接线分离（纯逻辑缝 + 应用层单点接线），host 与设备同源一份 C 源，消除「只有真机能判」的结构性可测性缺口。
- 阈值单源：三个阈值住头宏、`idle_timeout_s` 由调用方注入既有 `kSessionIdleTimeoutS`，`cfg_default` 刻意不覆盖它。
- 两条 `60`（`kSessionIdleTimeoutS` 与 `kStoryEmptySessionTimeoutS`）仍是两个独立常量（见 Findings N-R1），本包按既有命名常量口径保留。
- 无过早抽象、无重复实现、无新依赖；`suspect` 只作审计位，不引入第二个定时器。

### 十二、Findings（本轮追加）

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Maintainability | A6 与 D3 两条 producer 的 60 秒阈值来自两个独立命名常量（`kSessionIdleTimeoutS` 经 cfg 注入 vs 直接比较 `kStoryEmptySessionTimeoutS`）；改任一个而不改另一个会让两条收尾时点分叉。 | changed-code hunk `application.cc` CLOCK_TICK 区 A6 / D3 两条 producer 判据行；`session_voice_watch.h` `cfg_default` 注释明写「idle_timeout_s 由调用方注入」 | 不阻塞：两值当前同为 60 且各有审计行可区分；后续若调阈值，同批改两处或把 D3 也接入同一个 cfg 字段。 |
| NIT | judgement-call | Test quality | 本轮头宏变异（#4/#6/#7）依赖人工「先删 `.o` 强制重编」才具判别力；自动化变异若按默认增量构建会假绿。 | `mutation-evidence/round-b-fix.md` §三.1 实测；`session_voice_watch.h` 在验收 TU depfile 中未登记 | 不阻塞：已如实申报；构建卫生问题交主线程（未覆盖面 B8）。 |

> 本轮无 BLOCKING、无 SHOULD（hard-violation 零条）；两条均为 NIT 级 judgement-call，均有 changed-code hunk + 实测依据，不伪造 rule citation。

### 十三、Residual Risk（本轮追加）

- **不可合并声明（承重）**：`device-round-evidence.md` **未生成**、真机轮 C **未跑** → 本包出口未闭合，**不得合并**。轮 C 至少覆盖 A6 / A7 / D3 / A5 回归与 C-1..C-6 采集项；任一失败回到 planner。
- 新增残余：M3 根源（`AfeAudioProcessor::Stop()` 不复位 `is_speaking_`）、M4 只诊断不修（`rec_ctx=` 归因，修复归 P2）、故事会话起步噪声沿的无端录音页跳转、TTS 回声被转写为 stt 使 D3 判成有素材（未覆盖面 B5）、构建 depfile 时序（B8）、`change-manifest` 闸恒判 pre-cutoff（B9）——详见 `docs-delta.md`「新增残余申报」与本节未覆盖面表。
- 两条 debt（`TECH-261114` / `TECH-261012`）维持 `deferred`，**结项凭据 = 真机轮 C 通过**，本轮不改账本。
- `decisionCoverage` 残差：本包 8 条全部 `landsIn = exitCriteria:KRC-E1/E3/E4/E6/E8`，**无 `landsIn: dropped` + `reason` 以 `risk-accepted` 开头的条目**，故无 human 已接受的 risk-accepted dropped 项。
- 回滚：本轮改动全在固件侧（front / 引擎零 diff），回滚 = revert 本轮收口 commit；两半一起回滚的旧口径对 D2/B9 仍适用（那两个修复在轮 A 修复轮 commit 内）。
- **跨面变更（spine 在册）**：imp anchor 事实 = 离线面全绿（final r7 十闸）、真机轮 C 未跑、合并门仍被轮 C 与 G4 拦；建议 status = `ready-to-merge` **不成立**（应为 `in-progress`，待轮 C）；workstream `261115-session-channel-resident` P1 锚点与 status 回写归主线程（integrator 不直改 spine）。

## 真机轮 C 修复轮（final r9，2026-11-18；本节为当前态，以上各节为历史轮次，保留不删）

> **状态口径覆盖**：上一节是轮 B 修复轮（final r7、27 条验收、deviceOutcome = PENDING 轮 C）。轮 C 在线取证（2026-11-18 21:20–21:24 CST，planner 只读亲拉 ECS 三源）暴露两层新缺陷，human 当日指示「现在就修」（pod-events `human-correction` `c50b1954-181c-45fd-816a-74237b9fc6ae`）→ 本轮补素材侧过滤与产物侧占位守卫。离线面收口于 **final r9 十闸全绿**，验收累计 **31 条**（原 27 + SCRF-C1..C4）。**真机轮 C 完整复测仍未跑**：`device-round-evidence.md` 未生成、本轮不申报任何真机 PASS、本包不得合并。

### 一、本轮 Change Size

- **本轮 Files changed = 3 件**（`git diff --numstat HEAD -- firmware server docs`，相对上一收口 commit `095122d2`）：

  | 文件 | 增/删 | 内容 |
  |---|---|---|
  | `server/gateway/front/front_app.py` | +219/−14 | 段级静音幻觉过滤（`_FRONT_SILENT_ASR_HALLUCINATIONS` 三段分档 + `_is_silent_asr_hallucination`，接入 `_story_feed_from_stt` 引导播报窗判定之后）+ 整理产物占位守卫（`_guard_compact` / `_organize_body_reject_reason` R1–R4 / `_FRONT_ORGANIZE_PLACEHOLDERS`，落在 `archive_story` 之前）+ `reason_sink` keyword-only 可变出参 + 收尾 reason 分档（静默释放类不安抚、丢稿类保留安抚） |
  | `server/acceptance/test_session_channel_resident.acceptance.py` | +744/−0 | 新 helpers 四件（`_guard_compact` / `_story_text_frames` / `_kidpal_speak_frames` / `_ledger_snapshot`）+ 两组 TestCase：`ScrfSilentAsrSegmentFilterAcceptance`（C1 一例、C4 四例）、`ScrfOrganizePlaceholderGuardAcceptance`（C2 两例、C3 两例） |
  | `server/tests/test_archive_guard_units.py` | +161/−0 | 纯 additive：第二 AST 抽取器 `_load_output_guard`（wanted 含 `_guard_compact` 引用的 `_FRONT_GUARD_STRIP_RE`）+ `OrganizeOutputGuardUnits` 六例；既有 `_load_guard` 的 wanted/ns 与四条判定 case 逐字不动 |

  - **合计 +1124/−14**。
- **acceptance 744 行归因**：helpers 与常量约 90 行；九条用例的端到端断言约 650 行——逐条含「STORIES_DIR 零新增 / seq 账本不增」「零 `story_text`」「`kidpal_speak` 安抚帧零或恰一」「`start_organizing` 恰 1」「页面退出帧 `story_list` 恰 1 且 `total==0`/`items==[]`」「审计 token 恰 1」「stt 转发帧数不变」等观测面。
- **零 diff 面（实测）**：`firmware/**`、`server/gateway/story.py`、`server/gateway/display_schema.py`、`server/gateway/front/prompts/**`（prompt 制品）、`docs/**`、`docs/contracts/**`、`engine-patch/**`、`server/deploy/**` 全部零 diff。回滚 = revert 本轮单 commit；本轮无固件/引擎/契约面改动，回滚只涉 front 单侧。
- **manifest 对账（本轮工作树口径，按闸本应执行的规则手工核）**：`git status --porcelain` 的实现面（`firmware/**` + `server/**`）文件 3 件，全部命中 `changeManifest[]`（现 16 件），**清单外 0 件**。闸本体因 `current/` 指针恒判 pre-cutoff 未真执行（见第十节缺口 3）。

### 二、缺陷与修复（两层缺口，缺一不可）

轮 C 在线三源（front journal / 引擎 docker / ECS STORIES_DIR 文件）给出的缺陷面：

| 层 | 缺口 | 真机证据锚 |
|---|---|---|
| 素材侧（ingest 无防线） | `_story_feed_from_stt` 只判消息类型与空文本，不判内容：引擎静音 ASR 幻觉 `Okay.` 经上行 stt 喂进 pending_stop 累积器；`_archive_material_reject_reason` 归一化后 `okay` 不在幻听表、compact 长度 4 ≥ 2 → 放行进整理 | `device-evidence/round-c/engine-asr-2120-2123.txt:3-4`（21:22:06 `识别文本: Okay.` + 同刻记录语境抑制行）；`ecs-front-journal-2120-2134.txt:3-6`（21:22:05 pending_stop 保护 → 21:22:09 `story_organize_round ok=true` → `story_finish archived=true id=2026-11-18-s33477cee`） |
| 产物侧（整理输出零验收） | `_organize_and_archive` 只判 `lines` 为空（且有 `body = lines[1:].strip() or reply.strip()` rescue）；整理 LLM 的占位/拒绝式回复「（没有可整理的内容）」是合法非空行 → 直接进 `archive_story` | `device-evidence/round-c/ecs-story-2026-11-18-s33477cee.md`（标题与正文逐字「（没有可整理的内容）」、mtime 21:22）；控制组 `ecs-story-2026-11-18-sbe0bff90.md`（同夜 A5 真素材正常归档） |

修复三件（全部落 `front_app.py`）：

1. **段级静音幻觉过滤（素材侧防线）**：`_story_feed_from_stt` 在引导播报窗判定之后、routing 判定之前插入「整段归一化后与 `_FRONT_SILENT_ASR_HALLUCINATIONS` **整串相等**才 drop」；不子串、不挪到转发路径之前、不 `continue`；`pending_stop` 下被过滤**不即时释放**，交既有 10s sweep（防吞 1.7s 后才到的真实转写）。审计行 `story_feed_guard dropped=silent_asr_hallucination`。
2. **整理产物占位守卫（产物侧后防线）**：`_organize_body_reject_reason` 四档机械判据——R1 归一化空（`empty_body`）/ R4 整串成对括号或方尖括号族包裹且剥壳后 ≤16 字（`placeholder_body`，结构判据、措辞无关）/ R2 compact <2（`insignificant_body`）/ R3 compact 命中占位集合（`placeholder_body`）。守卫落在 `archive_story` **之前**（`archive_story` 先占 seq 后写 MD，删档会留 seq 缺口）；单行回复先过判据、多行走首行标题 + 其余行正文。
3. **reason 分档处置（收尾下行）**：`reason_sink` keyword-only 出参让每个 `return None` 前登记原因；静默释放类（`empty_body` / `insignificant_body` / `placeholder_body`）= 不安抚、只发页面退出帧、审计 `reason=organize_placeholder` + `body_reason=` 细分；丢稿类（`llm_unavailable` / `empty_reply` / `archive_valueerror` / 缺省）= 逐字保留「安抚句 + 页面退出帧」既有行为。两类都**必须**发 `_push_story_list(restore=False)` 页面退出帧（ORGANIZING 过渡态唯一机械出口，261021 R6 先例）。

### 三、Verification Evidence（本轮，承重）

- **Final round fact（承重）**：`delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r9.json`，`phase=final / round=9 / ok=true / observedGateExit=0 / invocationId 9cee1805-4f5b-4621-bf69-be61013cb78f`（2026-11-18T14:27:14Z–14:28:37Z），10 闸全跑。
- **机械凭据闸**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 9 --invocation-id 9cee1805-4f5b-4621-bf69-be61013cb78f` → `exit 0 / factOk=true / phase=final / round=9`。
- **r8 scoped（8 闸 ok）**：`verifier-evidence/rounds/r8.json`，`phase=scoped / round=8 / ok=true / observedGateExit=0 / invocationId ffeeab66-d5f9-4a44-849a-40ba9faaf93b`；`verifier-evidence-check --phase scoped --round 8 --invocation-id ffeeab66-…` exit 0。
- **闸表（r9 final）**：`taskpackage-required` / `change-manifest`（pre-cutoff 存量放行，缺口 3）/ `contract-lint`（1 条既有相对链接 warning）/ `acceptance-coverage`（**31 acceptance id 全覆盖**）/ `mutation-check`（31 条 per-acceptance 变异 evidence 齐）/ `docs-sync` / `security` 七闸 pass；`firmware-build` `build-pass`（`xiaozhi.bin` 0x329580 bytes / app 分区 8% free；本轮固件零 diff，增量构建）；`unit`（full，scope 求交）**302 passed / 3 deselected / 102 subtests**（33.6s）；`acceptance`（full，scope 求交）**204 passed / 3 deselected / 13 subtests**（34.1s），surface = server 14 / firmware 12。
- **Host runner（integrator 亲跑 @ working tree，执行目录 `firmware/main/test/host_app`，WSL Ubuntu-24.04）**：`321 Tests / 3 Failures / 0 Ignored`——3 红全为上游存量 `p1_auxbar_style.acceptance.c` AB-A1 / AB-A2 / AB-A4，与本包无调用关系；本轮固件零 diff，数字与轮 B 修复轮一致。
- **server 全量（integrator 亲跑 @ 仓根，`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance server/tests`）**：`799 passed / 0 failed / 9 deselected / 137 subtests`（78.4s）——零新增红。
- **目标件（planner/builder 实测基线）**：`test_session_channel_resident.acceptance.py` + `test_archive_guard_units.py` = **47 passed / 0 failed / 99 subtests**。
- **变异证据**：`mutation-evidence/round-c-fix.md`（8 条实跑：R4 删除 / `_ORGANIZE_BODY_MIN_CHARS 2→0` / 删段级过滤 / 段级集合加入 `嗯` / 静默释放加回安抚 / 删页面退出帧 / reason 改名 / 守卫移到 `archive_story` 之后）；8/8 `sha256` 还原一致。
- **Scope 圈定（本轮 r9，范围圈定清单落盘路径一行）**：`delivery/taskpackages/261116-channel-resident-core/test-scope-final-r9.txt`（**31 条去重**，新落文件；r7 清单 `test-scope-final.txt` 保留为历史）。合成 = `acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（25 去重）∪ `detect_changes`（project `C-AI-KidPal-pod-channel`，since `base/261115-session-channel-resident` / merge_base `3b3832cb`，inbound depth 2：**89 changed files**（含本包簿记件；实现/验收面 16 件）/ **seed 490 符号** / **impacted_total 138** 节点，其中 server/acceptance 78、server/gateway 21、server/tests 16、firmware/main 23）映射测试 ∪ 共享缝相关测试 ∪ 安全余量。本轮增量复核（`since HEAD`，覆盖未提交工作树）= 18 changed files / seed 46 / impacted_total 51，与包级结果指向同一 server 测试面。per-Pod 专属图索引可用 → **正常圈定、未降级**（detect_changes 与 trace_path 均可用）；相对 r7 只增一项 `server/tests/test_story_units.py`（architect 建议、impactedTestsSeed 已登记），不缩到本包档、不默认全量。`server/acceptance/archive/test_schedule_reminder.acceptance.py` 属 archive 退役面（两闸 SKIP_DIRS），如实排除并留痕。
- **收口 commit 后复核（Step 9b advisory 记账，commit `b0c0d9a4`）**：detect_changes（同项目、同 base、inbound depth 2）96 changed files / seed 488 符号 / impacted_total **121** 节点（server/acceptance 75、firmware/main 23、server/gateway 17、server/tests 6）——与圈定时 138 节点指向同一测试面，未出现清单外新测试文件，实现面符号影响无外溢。

### 四、Per-Acceptance 证据 · 轮 C 追加（4 条；全包累计 31 条）

| Acceptance | 断言载体（测试） | 结果 | 变异判别力（引 `mutation-evidence/round-c-fix.md`） |
|---|---|---|---|
| SCRF-C1 段级过滤 + pending_stop 不归档（主防线，端到端） | `test_scr_c1_pending_stop_silent_asr_dropped_then_sweep_releases`（`test_session_channel_resident.acceptance.py:2046`）：ctx(story) → `listen stop` 累积空落 pending_stop（`action=protected`）→ 注入 `stt=Okay.` → `_story_finish_session` 零调用、累积器仍空、routing 仍 pending_stop；+10.5s 既有 sweep 释放：`active=false` 恰一帧、零 `start_organizing`、零 `story_text`、`story_list` 零新增、STORIES_DIR 零新增、seq 不占号、routing 回 chat；审计 `dropped=silent_asr_hallucination` 恰 1；stt 转发不受影响（恰 1 帧） | PASS | #3（删段级过滤调用）→ 本件红；#4（段级集合加入单字 `嗯`）→ SCRF-B10 case 2 与 C4 不变式红 |
| SCRF-C2 整理产物占位守卫（后防线，承重） | `test_scr_c2_organize_placeholder_guard_releases_with_page_exit`（`:2372`）+ `test_scr_c2_output_guard_mechanical_discrimination`（`:2470`）：素材 `好的。` 过段级过滤与素材守卫 → 整理 stub 返回 `（没有可整理的内容）` → R4 命中 → 零归档（MD 零新增 / seq 不增 / 零 `story_saved_stamp`）、零 `story_text`、零 `kidpal_speak` 安抚、`start_organizing` 恰 1、`story_list` 页面退出恰 1 且 `total==0`/`items==[]`/`restore` 缺省或 false、`active=false` 恰 1、audit `reason=organize_placeholder` 恰 1；判别力八样本（R1/R2/R3/R4 各自拒 + 真实短故事与引号族放行） | PASS | #1（删 R4 块）/ #2（阈值 2→0）/ #5（加回安抚）/ #6（删页面退出帧）/ #7（reason 改名）/ #8（两处守卫同移到 `archive_story` 后）→ 按预期转红 |
| SCRF-C3 A5 正常路径回归（must-stay-green） | `test_scr_c3_normal_archive_path_keeps_working`（`:2552`）+ `test_scr_c3_llm_unavailable_still_soothes_and_exits`（`:2632`）：真实素材 + 正常两行整理输出 → `archived=true`、恰新增 1 篇且标题/正文 == 整理结果、`story_text` 恰一帧、`story_list` 含新篇、seq +1、routing 回 chat；丢稿类（`llm_unavailable` / `empty_reply` / `archive_valueerror`）仍安抚 + 页面退出帧（SAO-A1 四件保绿） | PASS | #5 / #7 与已测 `llm_unavailable` 对照分支反证分档未吞丢稿类 |
| SCRF-C4 过滤保守性与集合不变式（判据判别力） | `test_scr_c4_hit_family_dropped_and_mixed_speech_kept`（`:2149`）、`test_scr_c4_segment_set_invariant_and_single_source`（`:2224`）、`test_scr_c4_pending_stop_filter_does_not_release_immediately`（`:2290`）、`test_scr_c4_finish_phrase_surface_unaffected`（`:2319`）：命中族全 drop、`嗯。` 不命中（留给素材守卫）、混排真话四例照常入素材、三段式后整理输入恰含两句真话、集合不变式每元素 compact 长度 ≥2、判定单源可换表、pending_stop 下不即时释放、收尾词判定面零影响 | PASS | #3 / #4 → 命中族与不变式子测红；单测 `test_silent_asr_hit_family_and_misses` / `test_silent_set_invariant` / `test_placeholder_table_is_compact_and_disjoint` 同向 |

> 产物守卫单元面另由 `server/tests/test_archive_guard_units.py::OrganizeOutputGuardUnits` 承重：`test_r1_r2_r3_r4_rejects_with_reason`、`test_r4_two_layer_shell_and_length_upper_bound`、`test_real_bodies_pass`（纯函数直调，不经泵）。

### 五、下行序列与 human 原话「不推 story_list」的口径分法（显式留痕，planner 裁定）

human 2026-11-18 原话「不推 story_list」（出自 D3 指令：「无有效素材时自动收尾走释放、不归档、不推 story_list、不盖章」，pod-events `human-decision` `6df808ac-…`）按语境分两条路径：

1. **素材侧过滤路径（会话从未进整理回合）→ 严格零帧**：`Okay.` 类静音幻觉被段级过滤挡在累积器之外，routing 保持 `pending_stop`，由既有 10s sweep 释放（`active=false` 恰一帧），**零 `start_organizing` / 零 `story_list` / 零 `story_text`**——human 原话在此路径字面成立（SCRF-C1 断言承载）。
2. **整理回合已发生后命中占位 → 必须发 `restore=False` 页面退出帧**：设备已收 `start_organizing`、处在 ORGANIZING 非交互过渡态（KEY/BOOT 全吞）；无下行帧即困死整合页（261021 真机轮 R6 实证）。该帧属「页面退出」而非「列表新增」——SCRF-C2 断言 `total==0` / `items==[]` / `story_text` 零帧。同一理由下 SAO-A1 已于 261021 包把「零帧」重钉为「恰一帧页面退出帧且 total==0」。

**human 真正在意的「列表不虚增」由不归档保证**（`total` 由 `list_stories` 现算，不归档则不变），不由不推帧保证。**planner 裁定**：human 如改判为严格零帧，代价 = 设备困死整合页复现，planner 不建议。

### 六、上游残余登记与 builder 观察项（只登记，不新建债务文件）

1. **上游残余（不在本仓修）**：引擎静音 ASR 幻觉（`Okay.` / 「嗯。」族）为**引擎 ASR provider 的上游问题**，本轮只在 front 侧加防线、不改引擎代码（`engine-patch/**`、ECS `core/**` 零 diff）。**建议主线程登记 tech debt**（本 Pod 只写 review-packet 与 docs-delta，不新建 `docs/debt/**` 条目）。
2. **builder 观察项（已由 planner 裁定采纳，留痕）**：
   - R4 判序实现为 **R1→R4→R2→R3**（架构枚举为 R1→R2→R3→R4）：冻结验收 C2 对 `【无】`（compact=1）期望 `placeholder_body`，R2 会先命中 `insignificant_body`；验收不可改，故结构判据 R4 先于长度判据 R2。两类 reason 同属静默释放档，判据集合与语义未变（`mutation-evidence/round-c-fix.md` 观察项 2）。
   - `_load_output_guard` 的 wanted 集合增列 `_FRONT_GUARD_STRIP_RE`：`_guard_compact` 直接引用该模块级正则，不抽取会在受限 exec 期 NameError（假红）；既有 `_load_guard` 的 wanted/ns 零改（观察项 3）。
3. **变异观察项**：「#8 须同移两处守卫才转红」——单行分支的前置 `_organize_body_reject_reason` 是独立的第一道产物闸，只移主体守卫时八个单行样本被前置接住、0 红；两处同移后按预期转红。结论 = 「守卫在 `archive_story` 之前」的结构性质须两处同时变异才能钉住，已如实登记（观察项 1）。

### 七、未覆盖面申报（覆盖断言覆盖轴的设计面差集）

> 逐条列「本次无验收断言 / 未跑的设计性质 → 现状 → 去向」。**零缺口不成立，故不写 `none`。**

| # | 设计性质（无断言/未跑） | 现状 | 去向 |
|---|---|---|---|
| GAP-C1 | **真机端到端：整理回合占位命中后设备退出 ORGANIZING 并回列表** | 未跑：离线只用 `_LLMStub` 注入占位回复 + 泵缝断言「页面退出帧下发」；设备侧「收到帧 → 退出过渡态 → 页面停留列表」无真机目击 | 真机轮 C 复测（第九节追加项）；证据落 `device-round-evidence.md`，未 PASS 不合并 |
| GAP-C2 | **真机端到端：静音幻觉发生在 pending_stop 窗口时 front 过滤生效** | 未跑：本地以 `stt=Okay.` 注入模拟，front 日志行只在真机部署后可见；且需真实引擎 provider 产出幻觉 | 真机轮 C 复测（D3 窗采集 `story_feed_guard dropped=silent_asr_hallucination`） |
| EQ-C1 | **host 与真机不等价接缝（本轮）**：本轮改动全在 server 面（front），host runner 不编译 front；离线证据由 server pytest 泵缝承担 | 已由冻结验收申报：SCRF-C1..C4 的断言载体全部在 `server/acceptance/test_session_channel_resident.acceptance.py`，设备侧行为只经 `_FakeDeviceSocket` / `_FakeUpstream` 代理 | 等价性由 server 泵缝 + 真机 front（ECS 部署）与设备 KPLOG 三源互证；不扩 host 面 |
| EQ-C2 | **引擎产生面不可测**：静音 ASR 幻觉的产生发生在引擎 ASR provider（ECS 容器，本仓零改动、不可离线复现） | `device-evidence/round-c/engine-asr-2120-2123.txt` 证明缺陷**发生**，不证明修复后引擎输出；front 侧只能防「已上行的幻觉」 | 上游残余（第六节第 1 条）；若引擎侧可配静音门控，另立批评估 |
| C-R4 | **R4 括号族长度上界（≤16 字）的漏网面**：长于 16 字的括号占位按真实正文放行；未做「忠实度重叠检查」 | 未做：planner 裁定本轮不做（纯拉丁素材 vs 中文 body 零重叠会误杀） | follow-up 候选：真机轮 D 若见长尾占位漏网再启 |
| C-C | **档 C 日语静音族（うん/そう/はい/いいえ）证据强度** | 家族模式推断、非本仓实录（代码注释已诚实标注） | 真机若出现日语音幻觉误归档，按新证据扩表或收表 |
| C-CH | **中文应答词（好的/知道了/行吧/可以/对的）不入表** | 有意排除（孩子真会说的话、无本仓幻觉实证） | 先靠审计 token 积累证据，再决定是否扩表 |
| C-TTS | **TTS 引导播报被转写为 stt 的自回声**（轮 B 未覆盖面 B5 延续） | 本轮段级过滤只收静音幻觉族，收不住「TTS 回声被识别成真话」 | 真机轮 C 引导播报回归项观测；若复现另立批收窄素材门到非引导窗 |
| C-PROMPT | **prompt 制品不改**：整理 LLM 仍可能返回其他措辞的占位 | 有意不做（模型不遵守措辞，改 prompt 不解决机械可判性） | 长尾由 R3/R4 与审计行 `body_reason=` 积累证据，后续按证据扩表 |

### 八、Demo 对账行（workstream 包纪律）

| 证据形态 | 实际结果 | 一致性结论 |
|---|---|---|
| host runner（设备 C 验收） | `321 Tests / 3 Failures / 0 Ignored`（3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4）；本轮固件零 diff | 与「固件面静止、零新增红」一致；**本轮修复不在固件面，host 不能证明本轮效果** |
| server pytest（front 验收） | final r9 scope 求交 `unit 302 passed` / `acceptance 204 passed`；server 全量 `799 passed / 0 failed` | 与「两层守卫离线成立、既有 27 条冻结验收零松动」一致 |
| device build | `firmware-build` 闸 `build-pass`（`xiaozhi.bin` 0x329580 bytes，8% free） | 与「编译面全绿」一致 |
| 离线代理（stt / LLM stub 注入） | 段级过滤与占位守卫均以泵缝 + stub 真跑（SCRF-C1..C4） | 与「判据机械可判」一致 |
| **真机轮 C** | **未跑**（轮 A FAILED、轮 B 部分失败、轮 C 在线取证仅为缺陷锚定，不是完整真机轮）；`device-round-evidence.md` 未生成 | **真机未验、不得视为 PASS**；本包出口未闭合，不得合并 |

### 九、真机轮 C 复测清单（human 持板；沿 `pending-human-ruling.md` 待裁一并追加本轮两项）

1. **D3 空素材静置**：故事页 KEY 起会话、全程不说话，静置 60 秒 → 设备自动回列表；**不归档 / 列表不变（`total` 不增）/ 不盖章**；front 无 `story_text`；**若引擎吐 `Okay.` 类幻觉，front 日志应有 `story_feed_guard dropped=silent_asr_hallucination`**（本轮新增观察点，GAP-C2）。
2. **A5 有素材归档正常**：讲完 → 整理 → 归档 → 盖章全链（`story_sd_write` → `story_saved_stamp` → `story_list_dispatch`）。
3. **A6 普通会话静置 60 秒自动关麦**（审计 `session_idle_timeout`）。
4. **A7 连续讲 >90 秒不误收**（关注 `asr=` 最大值与 `run=` 走势）。
5. **引导播报回归**（轮 B 白盒补丁的真机承载）：记录会话触发一次卡壳引导播报（Listening→Speaking→Listening）后静置 60 秒 → 不得被判成空素材释放，已讲内容能归档。
6. **【本轮追加】整理占位回归**：构造素材但让整理不可用/返回占位（如临时改 prompt 或断 LLM 后走占位路径）→ **不归档 + 设备能退出 ORGANIZING**（页面退出帧承载；GAP-C1）。
7. **诊断采集项沿用**（轮 B 修复轮清单 C-1..C-6）：`listening_diag` 的 `suspect=` / `quiet=` / `asr=` / `run=` / `rec_ctx=` / `rec_mat=` / `ticks=`；D3 窗重点 `rec_ctx=1 && asr_seen=0 && quiet 到 60`。
8. 轮 C 通过后由 human 交付 `sourceCommit` + `sha256(xiaozhi.bin)`，planner/integrator 才能生成 `device-round-evidence.md`（含 launch-card 两条 requirements 引用与 SCR-A8 的逐条 PASS 目击）。**未生成、未跑即不合并、不伪造 PASS。**

### 十、已知工具缺口（沿上一轮，不新裁）

1. **G4 `verify-logs` 结构性缺口**（`gate-tooling-gap-g4.md`）：全绿 Pod 零 verify-log → 监督闸恒红；本包因既有失败轮留有 log 暂未触发，**不代表已修**。修复面 `.claude/**`，归主线程。
2. **`parallel-overlap-tier3` 对生成件误判**（`gate-tooling-gap-overlap-index.md`）：实撞 = `delivery/workstreams/INDEX.md` 头部「最后更新」日期行；architect 评审判 ②（生成件）、建议 `.gitattributes` 加 `merge=union`。不在 `escalateExempt` 内，人批豁免不了。
3. **`change-manifest` 经 `current/` 指针恒判 pre-cutoff**（`gate-tooling-gap-manifest-cutoff.md`）：闸的清单对账结构性不执行；本轮按闸本应执行的规则手工对账（3 件实现面文件、清单外 0 件）。

### 十一、Five-Axis Review（本轮，author 语义 review）

**1. Correctness**
- 段级过滤位置正确：排在引导播报窗判定之后、routing 之前（既有 FROZEN CONTRACT B 节语义第一优先不动）；只作用于旁路素材消费，stt 转发帧数不变（SCRF-C1 显式断言）。
- 整串判定防误伤：混排真话（「嗯，然后我们去公园」「Okay，我知道了」）照常入素材；「嗯。」compact=1 不命中段级集合而由素材守卫 `<2` 拒绝——SCRF-B10 case 2 的「尾音幻觉仍进收尾单源」保绿的机械依据。
- 产物守卫位置正确：在 `archive_story` 之前（seq 先占号，删档会留缺口）；R4 剥壳只认成对括号/方尖括号族、排除引号族，引用原话不被误伤。
- reason 分档保守：静默释放类只收「模型明确声明无内容」三档；`empty_reply` 归丢稿类（空返回成因不可区分）；缺省 reason 兜底丢稿类。
- `reason_sink` 为 keyword-only，位置参数序 `(raw, corr_id, device_id)` 逐字不变（memory_isolation K3-E2④ 保绿）。

**2. Test quality**
- 四条新验收各有独立载体，变异 8 条逐条转红（含 R4 删除、阈值改零、删过滤、集合污染、加回安抚、删退出帧、reason 改名、守卫移位）。
- 集合不变式单独冻结断言（每元素 compact ≥2）+ 单测 `test_silent_set_invariant` / `test_placeholder_table_is_compact_and_disjoint` 双钉。
- 观察项：「#8 须同移两处守卫才转红」如实登记；R4 长尾（>16 字括号占位）为 planner 裁定的 follow-up。

**3. Contract / boundary safety**
- `docs/contracts/**` 零 diff；本轮不新增帧、不扩 display 白名单、无 `SPEC_CONTRACT_DRIFT`。审计 token 复用既有前缀（`story_feed_guard` / `story_finish`）只加新取值，全仓无断言依赖旧 token 的闭集。
- `server/gateway/story.py` 三缝零改（RDG-A3 边界：装配层不代行滤除）；`firmware/**`、`engine-patch/**`、ECS 部署件零 diff。

**4. Operability / security**
- 新增审计行：`story_feed_guard dropped=silent_asr_hallucination text=%r`（每次命中一行）、`story_organize_round ok=false reason=%s body=%r`（body 截断 32 字）、`story_finish archived=false reason=organize_placeholder body_reason=%s story_list_pushed=%d`——三处均可与该窗既有审计链对账，日志增量小（仅命中时）。
- 静默释放类不发安抚：孩子侧唯一反馈是页面退出帧；由 human 原话口径裁定承接（第五节），属有意设计。
- 无新 secret、无权限面变更、无新增外部输入解析扩展（只新增整串比对）。

**5. Maintainability / simplicity**
- 段级集合与素材守卫集合**独立成表、不 union**（union 会迫使既有 AST 抽取器 wanted 改名，且单字元素会让 ingest 吞掉「嗯。」）；两张表的形态与判定同源（`_guard_compact`）。
- 判据与处置分离：`_organize_body_reject_reason` 纯函数（host 单测直调）、`reason_sink` 只作通道、`_story_finish_session` 只做分档下发。
- 无过早抽象、无重复实现、无新依赖。`minimal:` 标记零新增（本包无新增标记）。

### 十二、Findings（本轮追加）

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Test quality | 段级集合档 C（`うん` / `そう` / `はい` / `いいえ`）为家族模式推断、非本仓实录；档 B 中「谢谢观看 / 请不吝点赞」等为引擎侧同族表移植。两档证据强度已在代码注释标注，但验收只证「在表内即 drop」，不证这些词在本仓真会作为幻觉出现。 | changed-code hunk `front_app.py` `_FRONT_SILENT_ASR_HALLUCINATIONS` 档 A/B/C 注释；`device-evidence/round-c/engine-asr-2120-2123.txt`（仅 `Okay.` 有本仓实证） | 不阻塞：命中集合保守（整串相等），误伤面仅为孩子恰好只说出这两个词；真机按审计 token 积累证据后再扩/收表。 |
| NIT | judgement-call | Correctness | R4 括号族剥壳长度上界 16 字：长于 16 字的括号占位（如长句「（本轮没有可整理的有效内容，请重新录制）」>16 字）会按真实正文放行归档；未做语义重叠检查。 | changed-code hunk `front_app.py` `_ORGANIZE_BRACKETED_MAX_CHARS = 16` 与 R4 分支；plan.md 裁定 7（不重叠检查为 follow-up） | 不阻塞：R3 整串集合覆盖已实证措辞；真机轮 D 见长尾漏网再启 follow-up。 |
| NIT | judgement-call | Maintainability | `_story_finish_session` 用 `reason_sink[-1]` 取原因、空表兜底 `"unknown"`；当前每条 `return None` 路径恰一次 append，但未来若有人在一路径中途多次 append，取末项语义会漂移。 | changed-code hunk `front_app.py` `_story_finish_session` `body_reason = reason_sink[-1] if reason_sink else "unknown"` | 不阻塞：现有路径 1:1；若扩判据，建议改成显式 `(reason, detail)` 单值返回或断言 len==1。 |
| NIT | judgement-call | Operability | 静默释放类不发安抚：整理回合占位命中时孩子侧只有页面退出、无语音反馈，可能被感知为「讲了却没反应」。 | changed-code hunk `front_app.py` `_story_finish_session` 静默释放分支；plan.md 裁定 2/5（与 R1-N4「纯幻听不安抚」同语义） | 不阻塞：与 human D3 口径（无有效素材 = 释放）一致；真机轮 C 观察孩子侧观感，若需提示另立批。 |

> 本轮无 BLOCKING、无 SHOULD（hard-violation 零条）；四条均为 NIT 级 judgement-call，均有 changed-code hunk + heuristic / 实测依据，不伪造 rule citation。结构闸 PASS/FAIL 不在本节复述。

### 十三、Residual Risk（本轮追加）

- **不可合并声明（承重）**：`device-round-evidence.md` **未生成**、真机轮 C **未跑** → 本包出口未闭合，**不得合并**。轮 C 至少覆盖第九节 1–8 项。
- 新增残余：上游引擎静音 ASR 幻觉（仅 front 侧防线）、R4 长度上界长尾、档 B/C 集合适配疑、`reason_sink[-1]` 取末项、静默释放无安抚（见 Findings / 未覆盖面）。
- 两条 debt（`TECH-261114` / `TECH-261012`）维持 `deferred`，**结项凭据 = 真机轮 C 通过**，本轮不改账本。
- `decisionCoverage` 残差：本包 8 条全部 `landsIn = exitCriteria:KRC-E1/E3/E4/E6/E8`，**无 `landsIn: dropped` + `reason` 以 `risk-accepted` 开头的条目**，故无 human 已接受的 risk-accepted dropped 项。
- 回滚：本轮改动全在 server 面（front + 验收 + 单测），回滚 = revert 本轮收口 commit；固件/引擎/契约零 diff，不涉两半一起回滚口径。
- **跨面变更（spine 在册）**：imp anchor 事实 = 离线面全绿（final r9 十闸）、真机轮 C 未跑（在线取证只锚定缺陷，不构成 PASS）、合并门仍被轮 C 与工具缺口拦；建议退出 status = `in-progress`（`ready-to-merge` **不成立**）；workstream `261115-session-channel-resident` P1 锚点与 status 回写归主线程（integrator 不直改 spine）。