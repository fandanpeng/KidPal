# Review Packet · 261001-m4-k3-record-session-live-repair

> integrator 合并门前产出（2026-10-01）。供 supervisor 机械闸自动复核；高风险（`riskTier=high`）必升 human review。
> TaskPackage：`delivery/taskpackages/261001-m4-k3-record-session-live-repair/taskpackage.json`（rigor=full / sourceType=regression / riskTier=high / docsSyncRequired=true）。
> 本件不替代契约门，也不允许跳过可回滚路径（本地 merge）。

## Change Size

- **实现面（firmware，tracked）**：7 文件，**+230 / −17**。
  - `firmware/main/display/story_page.c`（+72）：尾部追加 K3R 冻结缝 [R1]/[R2]/[R3] 三条纯逻辑判据（`kidpal_story_key_owns` / `kidpal_story_session_end` / `kidpal_story_list_deferred_flush`），additive、只增不改。
  - `firmware/main/display/story_page.h`（+34）：三缝声明（additive 尾部）。
  - `firmware/main/mcp_server.cc`（+42/−5）：D2 甲（录音进入路径 VOICE 让位 `kidpal_render_node_voice_end()`）；[R2] 消费侧二桥（`KidpalStoryViewPageState` / `KidpalStorySessionEndApply`）；D1 defer 分支补缓存同步 `kidpal_render_node_cache_story`。
  - `firmware/main/application.cc`（+18）：Idle 收尾先跑 [R2]（复位录音页态 + 停录音演出定时器）再前台回落。
  - `firmware/main/display/kidpal_render_node.cc`（+34/−…）：D4b-3 出帧前 `lv_obj_is_valid` 对象有效性守卫；D1 voice_end 前台回落 P3 支 + [R3] 判据消费。
  - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+34/−…）：`keya_story_page_wants` 收敛为单源 `kidpal_story_key_owns`（D4a）。
  - `firmware/main/test/host_app/main/CMakeLists.txt`（+13）：HOST_ACCEPTANCE_SRCS 接线（新冻验收 + builder 单测，置于 host smoke 之前）。
- **新增件（untracked 转正）**：3 文件，832 行。
  - `firmware/main/test/record_session_repair.acceptance.c`（721，acceptance-author 冻结件，RSR-A1..A5 + RSR-C1）。
  - `firmware/main/test/record_session_repair.test.c`（87，builder 单测）。
  - `firmware/main/test/acceptance/red-baseline/rsr_red_wrapper.sh`（24，新齿红跑独立通道）。
- **簿记面（delivery，非实现）**：taskpackage.json / verify-runs.jsonl / verifier-evidence/rounds/r1+r2.json / test-scope-final-r2.txt / docs-delta.md / review-packet.md / current/ 投影五件。
- **Scope**：纯固件实现面（`firmware/main/**`）；服务端 / 契约 / spec 零改。
- **Split recommendation**：无。本包 = K3 落地的四缺陷止血，单一内聚切片；D3「说完了」+ 逐句回显已拆 Package B（不在本包面，见 §未覆盖面申报）。

## Verification Evidence

- **Structural gates**：`firmware-build`（真编译，app bin 0x311350，分区余量 0x5ecb0 = 11% free）/ `contract-lint` / `change-manifest`（pre-cutoff 存量放行）/ `taskpackage-required` —— **scoped r1 全绿**。
- **Final 相位全闸（10 闸）**：`verifier-evidence/rounds/r2.json` **ok:true / observedGateExit 0**，含 `unit`（timing=full）+ `acceptance`（timing=full，`phase:oracle-pass`，firmwareAcceptanceFiles=6）。
- **Host 行为齿（真跑）**：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `./build/host_infra_test.elf` → **145 Tests / 0 Failures / 0 Ignored，exit 0**（2026-10-01，本树）。RSR-A1(D4a)/A2(D2)/A3(D4b-1)/A4(D4b-3)/A5(D1)/C1（回归+注册序）**逐齿 PASS** + builder 单测 [R1]/[R2]/[R3] 边界 PASS。
- **Security gate**：`securityReviewRequired` 未声明 → `security` 闸绿（`securityReviewRequired:false`）。
- **Mutation check**：5 条真实变异逐 AC 敏感（见下）。`mutation-check` 闸绿。
- **Docs sync**：`docs-sync` 闸绿；`docs-delta.md` 申报 spec 面零自改（检出两处归属他项）。
- **Test-scope 清单落盘路径**（Step 4 范围圈定落盘行）：`delivery/taskpackages/261001-m4-k3-record-session-live-repair/test-scope-final-r2.txt`（detect_changes project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` since `base/m4-star-notebook` / merge_base 4cb77a8，changed_files 26 / impacted 7 节点[firmware/main]∪ impactedTestsSeed[] 合成）。

### Mutation 证据（5 条，真实变异重放，逐 AC）

| id | 变异点 | 期望红 | 实测（counterEvidence 摘要） |
|---|---|---|---|
| K3R-A1 | `story_page.c` [R1] 规则(a) `return 1 -> 0`（[R1] 为 A1/A2 共享缝） | RSR-A1 tooth1/2 + RSR-A2 tooth1 | exit 3 / 145T **3 Failures**：RSR-A1 记录语境录音态未判故事页拥有 KEY（D4a 毁稿根） |
| K3R-A2 | `mcp_server.cc` 移除 `kidpal_render_node_voice_end()`（甲让位接线） | RSR-A2 tooth2 | exit 1 / 145T **1 Failure**：录音路径零层让位接线 |
| K3R-A3 | `story_page.c` [R2] 复位支 `return 1 -> 0` | RSR-A3 tooth1 | exit 2 / 145T **2 Failures**：会话收尾未复位录音页态（D4b 根） |
| K3R-A4 | `kidpal_render_node.cc` 移除 `lv_obj_is_valid` 守卫整块 | RSR-A4 tooth1 | exit 1 / 145T **1 Failure**：出帧前无对象有效性守卫（D4b-3 根） |
| K3R-A5 | `story_page.c` [R3] 命中支 `return 1 -> 0` | RSR-A5 tooth1 | exit 2 / 145T **2 Failures**：前台回落 P3 未判以 defer 缓存重渲（D1 根） |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | no new `minimal:` markers introduced | — | no-trigger |

> 本包改动面**未新增** `minimal:` 标记；存量 5 处（`story_page.c:981/1226/1300/1344`、`waveshare-s3-rlcd-4.2.cc:437`）均在未改动的上下文行，非本包引入。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | Correctness | [R1] 规则(a)「`page_state==RECORDING && ctx_active!=0 -> 1`」**不判 front_layer**——记录语境录音态下即使有更高覆盖层前台亦判故事页拥有 KEY。 | 冻结缝 [R1] 契约即如此定义（`record_session_repair.acceptance.c` 文件头 + `story_page.h` 声明），验收齿 RSR-A1 覆盖；记录期提醒排队延后（requirements v1.46）、配网/查字窗不与录音同屏 → 现役无冲突路径。 | 无（按冻结缝实现即正确） |
| FYI | judgement-call | Correctness/UX | D2 取甲（弹 VOICE 层）= 聆听期表情/状态 chrome 期间不可见；plan §3 已列为此形态已知代价。 | plan §3（甲/乙二选一，甲=改动小、语义清晰）；human 261001 触点①「同意甲」。 | 无（human 已裁，代价知悉） |
| FYI | judgement-call | Maintainability | `application.cc` 以本地 `extern` 声明消费 `KidpalStoryViewPageState` / `KidpalStorySessionEndApply`（不进 `mcp_server.h` 公共头）。 | 与既有 `KidpalStoryVadEnterRecording` 同款纪律（免扩公共面，changeManifest 不含该头）；同 TU 边界清晰。 | 无 |
| NIT | judgement-call | Operability | D2 录音进入路径 `kidpal_render_node_voice_end()` 与 `ScheduleStoryRender()` 先后调用，voice_end 可能先渲一帧 P3 列表再渲录音页（瞬态双渲）。 | `mcp_server.cc:KidpalStoryVadEnterRecording` changed-code hunk；单线程串行、无撕裂、齿不受影响。 | 无（另可选优化，非阻塞） |

> 无 BLOCKING / SHOULD finding。契约/spec 面零 `hard-violation`（本批零契约/spec 改动）。

## Five-Axis Review

1. **Correctness**：六条验收（K3R-A1..A5 + K3R-C1）逐条对上实现——D4a 归属判据（[R1] 规则 a，录音态+ctx 必产 `KSPV2_ACT_STOP_RECORD`，绝不落 `GLOBAL_ACT_SESSION_TOGGLE`）/ D2 层栈让位（甲，`kidpal_render_node_voice_end()` 令基层无覆盖层 + 录音页可见）/ D4b-1（[R2] 复位 LIST + 停定时器，不依赖页态自删）/ D4b-3（出帧前 `lv_obj_is_valid` 守卫，悬垂复位 NULL）/ D1（[R3] + `kidpal_render_node_cache_story` 补 defer 消费方）。边界：`kidpal_story_session_end(NULL)` → −1 不崩；非录音态幂等 0（out_page 零触碰）；[R3] 非命中（VOICE 仍前台/非 P3/子态）→ 0 不覆盖。host 145T0F 实证。**无缺口**。
2. **Test quality**：断言真实行为（host ELF 真跑，非 mock 空转）；5 条**真实变异**逐 AC 敏感（每条变异真红、还原真绿，见上表）；结构齿 RSR-C1 = canary 在世 + K3 冻结面零偏移（`record_session_live.acceptance.c` 未改一字）+ 注册序守卫（新件置于 host smoke 之前）；K2/M1-K1a 冻结面零偏移（host 145T 全绿）。红基线：acceptance-author 实跑（6 齿设计态红、零 fixture 缺陷红；零偏移基线 136T0F @5490a5c）。
3. **Contract/boundary safety**：契约零改（只读消费 §四 `kidpal_session_ctx` / §1.4 `start_recording`）；spec 零改（`docs/` 整面零写入）；无新依赖（`lv_obj_is_valid` = LVGL 9.3 既有）；跨层调用 = display 内（story_page ↔ kidpal_render_node/kidpal_layer，同子系统），无循环依赖。
4. **Operability/security**：`securityReviewRequired=false` → 无安全轴触发；无新增输入面/secret；新增日志 = `kidpal_render_node_voice_end` 的 `voice_end: story front list_flush=` + `KidpalStorySessionEndApply` 的 `story_session_end corr_id=kdp-<tick>`（可运维观测，无敏感数据）。
5. **Maintainability/simplicity**：板级 `keya_story_page_wants` 收敛为单源纯逻辑缝（`kidpal_story_key_owns`），三缝 additive 尾部追加（冻结纪律），局部、易懂、无过早抽象、无重复实现。deletion test：[R1]/[R2]/[R3] 若删，复杂度泄漏回板级分支 + 消费侧 → 深模组信号，保留。

## Residual Risk

- **Known limitation**：真机复验矩阵四窗目击（K3R-E3）+ AFE VAD 真沿 = **设备轮盲区，本相位未跑**（host 齿不覆盖真机渲染/触感/真 VAD 触发沿）；120s 静默自动归档路径本轮窗口未覆盖。详见「未覆盖面申报」。
- **Follow-up recommended**：`docs-delta.md` §4.2 的 §5.2④① consumer 补正文本须与 K3 pending delta 同批交 human spec 批落地（integrator 零 spec 写权）；`docs-delta.md` §4.1 的 §5.2④ 机制对账随 Package B（D3-甲）Track A。
- **Human decision needed**：本包 `riskTier=high` → supervisor 机械闸必产 **ESCALATE**，合并须 human 批（前置升级闸放行后由 planner 主线程 `merge-driver --pod` 本地合并）。真机轮 human 持板时点待裁。
- **decisionCoverage 中 `landsIn: dropped` + `reason: risk-accepted` 条目**：**无**（本包 `decisionCoverage` 为空）。

## 未覆盖面申报（逐条「性质 + 现状 + 去向」）

1. **真机复验矩阵四窗目击（K3R-E3）** — 性质：设备轮盲区（host 齿不覆盖真实 RLCD 渲染/触感）。现状：**未跑**（无真机证据，不伪装）。去向：live 确认轮 human 持板目击（BOOT 翻页进 P3 列表显示 / P3 KEY 见录音页非声波层 / 录音中 KEY 正常归档不崩溃 / 唤醒词问答+P2 课表+P3 删除回归）。
2. **AFE VAD 真沿（开口进录音态的设备触发沿）** — 性质：设备轮盲区。现状：**未目击**（host 齿为纯逻辑决策，真机沿未实测）。去向：live 确认轮目击。
3. **120s 静默无语音自动收尾归档路径** — 性质：时长窗口未覆盖（本轮窗口 < 120s）。现状：**未验**。去向：另行单验 / 设备轮。
4. **逐句回显内容（stt 帧回显）** — 性质：包外面（依赖 stt 帧）。现状：**不在本包验收面**。去向：Package B（D3-甲）。
5. **D3 语音收尾「说完了」** — 性质：包外面（触需求/架构/契约三面）。现状：**不在本包面**。去向：Package B（D3-甲），Track A + 契约门。
6. **服务端（server/**）面** — 性质：不适用（本包纯固件）。现状：**零改动**。去向：N/A。

> 本包面对面缺口：**无未申报项**（以上 6 条已逐条列明性质/现状/去向）。

## demo 对账行（anchor3：真机轮串口日志 + host 齿）

| 缺陷 | host 齿（真跑实测） | 真机轮串口日志（E3） |
|---|---|---|
| D1 列表无消费方 | RSR-A5 PASS（[R3] VOICE 弹出 + P3 + 列表态 → 1，以缓存重渲） | **deferred**（`story_list_deferred_flush` 落屏目击待设备轮） |
| D2 会话层压录音页 | RSR-A2 PASS（录音路径层让位接线 + [R1] 两态同结论） | **deferred**（P3 KEY 见录音页非声波层的视觉目击待设备轮） |
| D4a 键路由毁稿 | RSR-A1 PASS（录音态+ctx → KEY 归故事页 → STOP_RECORD） | **deferred**（`story_key stop_rec archive_pend=` 双日志待设备轮） |
| D4b 悬垂 panic | RSR-A3 PASS（[R2] 复位 LIST）+ RSR-A4 PASS（出帧前 `lv_obj_is_valid` 守卫） | **deferred**（录音中 KEY 不崩溃不重启待设备轮） |
| 回归/注册序 | RSR-C1 PASS（canary + K3 冻结面零偏移 + 注册序）；host **145T0F** 全量（K1/K2/K3 既有齿零回归） | **deferred**（唤醒词问答+P2 课表+P3 删除回归待设备轮） |

> **deferred 项 = 设备轮盲区，本相位无真机证据，如实申报、不伪装**。

## UI design-review 证据（文本；本包 UI-adjacent，无新增设计）

- **viewport coverage summary**：本包**无新增版式/布局**——D2 修复为**层栈让位**（既有录音页 `p3-recording` 由被 VOICE 层压住改为前台可见），不改任何页面构成。
- **a11y summary**：无新增交互面；键帽随态映射不变（[R1] 只改归属判据，不改 `kidpal_voice_states` 映射）。
- **design-drift summary（brief ↔ 实现）**：无 design-brief 漂移——本包不改设计稿母版（`docs/ui/**` 零触碰）、不改 UIUX-DESIGN-BRIEF 规则；录音页可见性属 K3 既有演出的修复。
- **external artifact reference**：无（bootstrap 期不在仓内存 screenshot/video/HTML）。
- **residual risk（UI）**：视觉目击（录音页/「● 录音中」可见性）归设备轮 K3R-E3（deferred，见未覆盖面申报 #1）。UI findings = 无（复用既有严重度阶梯，无 BLOCKING/SHOULD/NIT UI finding）。

## pod-events 交叉引用

- `delivery/taskpackages/261001-m4-k3-record-session-live-repair/pod-events.jsonl` **不存在**（本 Pod 未产 pod-events 事件流；Step 8c advisory，缺失不阻塞 closeout / supervisor / merge）。
- 本包 human-decision 源头交叉引用 spine：`delivery/changes/intake/261001-m4-k3-record-session-live-repair.md`（`humanVerbatim` 四条 + 已 resolved openQ：q1-四档路由确认③拆包 / q2-取甲机制漂移分类 / q5-model_name 冲刷另立包 / q6-甲案外部性处置「引擎侧再动抑制 LLM」）。
- post-cutoff 口径：不产 session-digest / loop-health / loop-metrics / pod-journal（EVENTS_SCHEMA_CUTOFF 250725 后）。
