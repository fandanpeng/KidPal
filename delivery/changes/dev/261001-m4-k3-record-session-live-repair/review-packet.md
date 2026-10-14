# Review Packet · 261001-m4-k3-record-session-live-repair

> integrator 合并门前产出（2026-10-01 **re-closeout**；D2 真机诊断定死后重收，取代首收产物）。供 supervisor 机械闸自动复核；`riskTier=high` 必升 human review。
> TaskPackage：`delivery/taskpackages/261001-m4-k3-record-session-live-repair/taskpackage.json`（rigor=full / sourceType=regression / riskTier=high / docsSyncRequired=true）。
> 本件不替代契约门，也不允许跳过可回滚路径（本地 merge）。

## Change Size

- **本包实现面（firmware，5490a5c 基线 → 现工作树，tracked）**：**11 文件，+1495 / −36**。
  - 实现 8 改：`application.cc`(+18) / `boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`(+34) / `display/kidpal_render_node.cc`(+77) / `display/lcd_display.cc`(+81) / `display/story_page.c`(+141) / `display/story_page.h`(+54) / `mcp_server.cc`(+54) / `test/host_app/main/CMakeLists.txt`(+13)。
  - 新冻验收 + 单测 + 红跑通道：`test/record_session_repair.acceptance.c`(948) / `test/record_session_repair.test.c`(87) / `test/acceptance/red-baseline/rsr_red_wrapper.sh`(24)。
- **re-closeout 相对首收 5950bb7 的增量**：13 文件 +704/−138（D2 三段修复 + 诊断日志 + 键帽 + REC + acceptance REVISION R1 追加 RSR-A6/A7 + pod 簿记）。
- **簿记面（delivery，非实现）**：taskpackage.json / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/r1–r4.json / test-scope-final-r2+r4.txt / docs-delta.md / review-packet.md / verdict.json / verification.json / current/ 投影。
- **Scope**：纯固件实现面（`firmware/main/**`）；服务端 / 契约 / spec 零改。
- **Split recommendation**：无（D3「说完了」+ 逐句回显 + 录音页 UI 还原度已 human 裁出本包，见未覆盖面申报）。

## Verification Evidence

- **Structural gates**：`firmware-build`（真编译，app bin 0x311350，分区余量 11%）/ `contract-lint` / `change-manifest`（firmware-build 面 + lcd_display.cc 已在 changeManifest）/ `taskpackage-required`。
- **Final 相位全闸（10 闸）**：`verifier-evidence/rounds/r4.json` **ok:true / observedGateExit 0**（含 full `unit` + `acceptance`）。轨迹：scoped **r3**（`7fccbab3-e289-444c-8230-0e969bf112d6`）+ final **r4**（`e9448712-5602-49d8-a669-024385feecbe`）均全绿；首收 r1/r2 在案（旧码）。
- **Host 行为齿（真跑）**：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `./build/host_infra_test.elf` → **147 Tests / 0 Failures / 0 Ignored，exit 0**。RSR-A1(D4a)/A2(D2)/A3(D4b-1)/A4(D4b-3)/A5(D1)/**A6([R4])/A7([R5])**/C1 逐齿 PASS；K1/K2/K3 既有齿零回归。
- **真机设备轮（human 目视 v6 通过）**：D2 底栏「收尾/结束」+ `00:14` 时长 + 常亮黑 REC 方块；D4a `story_key stop_rec archive_pend=1`；D4b 零 panic；D1 推送到达后 P3 出列表。证据 = `c:/tmp/serial-live-repair-r{2..6}.log` + pod-events `e2ce7911`/`97c36553`。
- **Security gate**：`securityReviewRequired=false` → `security` 闸绿。
- **Mutation check**：7 条（K3R-A2 含 3 条：voice_end 让位 / [R4] RSR-A6 / [R5] RSR-A7）逐 AC 敏感（真红→真绿）。
- **Docs sync**：`docs-delta.md` 申报 spec 面零自改 + 检出三处归属他项。
- **Test-scope 清单落盘路径（Step 4）**：`delivery/taskpackages/261001-m4-k3-record-session-live-repair/test-scope-final-r4.txt`（detect_changes project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` since `base/m4-star-notebook` / merge_base 4cb77a8；changed_files 39 / impacted 45 节点[firmware/main]∪ impactedTestsSeed[]）。

### Mutation 证据（逐 AC）

| id | 变异点 | 实测（counterEvidence 摘要） |
|---|---|---|
| K3R-A1 | `story_page.c` [R1] 规则(a) `return 1 -> 0` | exit 3 / 145T **3 Failures**（RSR-A1 记录语境键归属） |
| K3R-A2 | `mcp_server.cc` 移除 `kidpal_render_node_voice_end()` | exit 1 / 145T **1 Failure**（录音路径零层让位接线） |
| K3R-A2 | `story_page.c` [R4] 记录语境支 `return 0 -> return 1` | exit 1 / 147T **1 Failure**（RSR-A6 录音态未令 chrome 退隐） |
| K3R-A2 | `story_page.c` [R5] `if(on_story_front) return session_page` 改 `return node_cached_page` | exit 1 / 147T **1 Failure**（RSR-A7 页态未取权威会话态） |
| K3R-A3 | `story_page.c` [R2] 复位支 `return 1 -> 0` | exit 2 / 145T **2 Failures**（会话收尾未复位录音页态） |
| K3R-A4 | `kidpal_render_node.cc` 移除 `lv_obj_is_valid` 守卫 | exit 1 / 145T **1 Failure**（出帧前无对象有效性守卫） |
| K3R-A5 | `story_page.c` [R3] 命中支 `return 1 -> 0` | exit 2 / 145T **2 Failures**（前台回落未判以缓存重渲） |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | no new `minimal:` markers introduced | — | no-trigger |
| `firmware/main/display/lcd_display.cc`（原 K2 `minimal:` 注） | **移除**：K2 期「录音态键帽随 K3 接线同批落」的 defer 注记，本包补落 RECORDING 分支后注记消失（债务 -1） | — | no-trigger |

> 存量 4 处 `minimal:`（`story_page.c`，行号随编辑移位）均在未改语义的上下文，非本包引入。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | Maintainability/Process | D2 走了两轮弯路（[R4]/[R5] no-op 缝 + `force_hide`），真根因（键帽派发漏分支 + REC 相位算术）由真机诊断定死。 | changed-code hunk：`lcd_display.cc` keybar 派发 + `story_page.c` REC 块；对比 `voice_end: top=2`（VOICE 早让位）。 | 无（历史留根；弯路记 docs-delta §4.5，供 loop-retro） |
| FYI | judgement-call | Maintainability | 补严缝 [R4]/[R5] 经 architect 判**对 D2 no-op**（R5 恒等、R4 甲形态返回同旧判据），仍保留（`kidpal_story_voice_chrome_front` / `kidpal_story_render_page_state` 被消费）。 | `story_page.h`/`.c` changed hunk + architect 判定（review-packet 前序留根）；齿 RSR-A6/A7 已冻结。 | 无（冻结齿承载，作 `kidpal_chrome`/页态解析单源，deletion test：删则判据散回调用点 → 保留） |
| FYI | judgement-call | Operability | **诊断日志有意保留**（`PAGE_RENDER in/out`、`STORY_RENDER`、`RECORDING render`、`KEYBAR`、`chrome_mid_sync`、`chrome_front_in`）——诊断债务。 | `kidpal_render_node.cc`/`story_page.c`/`lcd_display.cc` changed hunks；收口说明明示保留。 | 无（后续包可择机收敛为 debug 级/条件编译） |
| FYI | judgement-call | Correctness | D1 **冷浏览空态**（开机未开会话翻 P3 `count=0`）**未修**——K3 已裁 dropped（`M4K3-DC-1`），human 2026-10-01 复裁维持接受。 | pod-events `97c36553`；requirements §5.2④ 推送时点属行为策略。 | 无（human 已裁接受；要修需 (A) 恢复推送时点或 (B) 契约门加 fetch 消息） |
| FYI | judgement-call | Correctness/UI | D2 派发点 `lcd_display.cc` **host 不可编 → 无法机判**，只能设备轮目击（真机 v6 已过）。 | `changeManifest` 含 lcd_display.cc 但不在 host ELF 源清单；acceptance RSR-A* 均 host 面、不含该派发点。 | 见 Residual Risk human 项（architect 建议升硬门，归 human 裁） |

> 无 BLOCKING / SHOULD。spec/contract 面零 `hard-violation`（本批零契约/spec 改动）。

## Five-Axis Review

1. **Correctness**：六条验收（K3R-A1..A5 + C1）逐条对上实现；D2 真根因两处（键帽 RECORDING 派发 + REC 常亮）由真机定死并目视通过。边界：`kidpal_story_session_end(NULL)` → −1 不崩；非录音态幂等 0；[R3]/[R4]/[R5] 非命中分支零回归。host 147T0F + 真机 v6 双证。
2. **Test quality**：断言真实行为（host ELF 真跑）；7 条真实变异逐 AC 敏感（含 D2 的 [R4]/[R5] 两缝）；结构齿 RSR-C1（canary + K3 冻结面零偏移 + 注册序）。**缺口如实**：D2 真修落点 `lcd_display.cc` 无 host 齿（不可编），仅设备轮目击——见未覆盖面。
3. **Contract/boundary safety**：契约零改；spec 零改（`docs/` 零写入）；无新依赖；跨层调用限 display 子系统内（story_page ↔ render_node ↔ lcd_display 经 C 桥，同子系统，无循环依赖）。
4. **Operability/security**：`securityReviewRequired=false`；新增 C 桥 `kidpal_lcd_recording_force_hide` / `kidpal_render_node_story_session_page` 带 NULL 守卫（`s_kidpal_lcd_self`）；新增日志可运维观测、无敏感数据。
5. **Maintainability/simplicity**：三缝 [R1]/[R2]/[R3] + 补严缝 [R4]/[R5] 皆 additive 纯逻辑单源；板级/lcd 收敛为缝消费点；局部、易懂。D2 弯路 = 过程成本（非最终代码复杂度），记 docs-delta §4.5。

## Residual Risk

- **Known limitation**：D2 真修派发点（`lcd_display.cc`）host 不可编 → 无机判齿，**只有设备轮目击**（真机 v6 已过）；冷浏览空态未修（human 裁接受）；逐句回显 + D3 + 录音页 UI 还原度不在本包。
- **Follow-up recommended**：① `docs-delta.md` §4.2 consumer 补正文本随 K3 pending delta 交 human spec 批；② §4.3 D3 表述订正随 Package B；③ §4.5 D2 弯路进 loop-retro 素材；④ §4.6 D1 plan 口径差供后续包订正。
- **Human decision needed**：
  - **【升级项】D2 设备轮硬门建议**：architect 建议把「D2 设备轮窗口」从 deferred **升为阻断合并的硬门**。**该升格是治理面改动（工程门变更），归 human 裁；integrator 不自行改门。** 现状：设备轮已 human 目视 v6 通过（非 formal gate）。
  - **D1 冷浏览**：human 2026-10-01 已裁接受 dropped（`97c36553`），无需再裁。
  - **UI 还原度出包**：录音页设计图 `docs/ui/V5/p3-recording.png`（小伴/折线波/气泡/回显行）human 裁不纳入本包、另立包（pod-events `64a4012d`）。
  - 本包 `riskTier=high` → supervisor 机械闸必产 **ESCALATE**，合并须 human 批。
- **decisionCoverage 中 `landsIn: dropped` + `reason: risk-accepted` 条目**：**无**（本包 `decisionCoverage` 为空；D1 冷浏览 dropped 为 K3 的 `M4K3-DC-1` + human 复裁，非本包 decisionCoverage 条目）。

## 未覆盖面申报（逐条「性质 + 现状 + 去向」，零缺口）

1. **D2 真修派发点（`lcd_display.cc` 键帽 RECORDING 派发 + chrome 强屏蔽）** — 性质：host 不可编 → **无法机判**。现状：设备轮已目视通过（v6）。去向：architect 建议升「设备轮硬门」（治理面，归 human 裁）。
2. **D1 冷浏览空态（开机未开会话翻 P3 `count=0`）** — 性质：K3 已裁 dropped（`M4K3-DC-1`）。现状：**维持接受、本包不返工**（human `97c36553`）。去向：要修需 (A) 恢复推送时点〔行为策略〕或 (B) 契约门加设备 fetch 消息。
3. **录音页 UI 还原度（设计图 `docs/ui/V5/p3-recording.png` 小伴/折线波/气泡/回显行）** — 性质：human 裁定出包。现状：**不纳入本包**。去向：另立包（human `64a4012d`）。
4. **逐句回显内容（stt 帧回显）** — 性质：包外面。去向：Package B（D3-甲）。
5. **D3 语音收尾「说完了」** — 性质：包外面；且原表述「manual 聆听产不出 stt 帧」**被真机反驳**（r2 日志有转写 + AI 回复，真缺口在 front story 累积/回传接线 `empty_accum`）。去向：Package B（D3-甲）订正 + Track A/契约门。
6. **120s 静默无语音自动收尾归档** — 性质：时长窗口未覆盖。去向：另行单验/设备轮。
7. **server 面** — 性质：不适用（纯固件）。现状：零改动。去向：N/A。

## demo 对账行（anchor3：真机轮串口日志 + host 齿）

| 缺陷 | host 齿（真跑） | 真机轮串口日志（`c:/tmp/serial-live-repair-r*.log`） |
|---|---|---|
| D1 推送到达后可显示 | RSR-A5 PASS（[R3] 前台回落以缓存重渲） | r6 `39462 render page STORY (P3) count=2`（退出录音态后出列表）；冷浏览 r6 `24982 count=0`（dropped，human 裁接受） |
| D2 录音态演出 | RSR-A2 PASS（层让位）+ RSR-A6/A7 PASS（[R4] 覆盖层退隐 / [R5] 页态同步） | 定死轮 r4 `24423 KEYBAR ovl=0 page=2 substate=2 left=说话·双击看 right=下移` + `RECORDING render`（录音页在画但键帽错）→ 修复后 r5/r6 `26372/39462 KEYBAR ... left=收尾 right=结束` + `RECORDING render ... rec_block=(20,52,24,24) steady=1` |
| D4a 键路由毁稿 | RSR-A1 PASS（录音态+ctx → KEY 归故事页） | r2 `80212 story_key stop_rec archive_pend=1`（不再落 GLOBAL_ACT_SESSION_TOGGLE） |
| D4b 悬垂 panic | RSR-A3 PASS（[R2] 复位）+ RSR-A4 PASS（出帧守卫） | r2–r6 **零 `Guru Meditation` / 零 panic** |
| D2 弯路反证 | — | r2/r3 `voice_end: top=2`（VOICE 层 round-1 即已让位，证「VOICE 覆盖层驻留」假设为弯路） |

## UI design-review 证据（文本）

- **viewport coverage**：本包无新增版式；D2 为既有录音页 `p3-recording` 的可见性/键帽修复（REC 常亮块 y=52 + 时长 y=48 避顶部 chrome 带）。
- **a11y summary**：无新增交互面；键帽随态映射复用 `kidpal_keybar_story_slots`（RECORDING 补入 = K2 未落分支）。
- **design-drift（brief ↔ 实现）**：录音页对设计图 `docs/ui/V5/p3-recording.png` 的**还原度（小伴/折线波/气泡/回显行）human 裁定不纳入本包、另立包**（`64a4012d`）；本包只修「键帽 RECORDING 分支 + ● 录音中可见」。
- **external artifact reference**：无（bootstrap 期不在仓内存 screenshot/video/HTML）。
- **residual risk（UI）**：还原度另包（见未覆盖面 #3）；D2 视觉目击已通过（非 formal gate，见 Residual Risk human 项）。

## pod-events 交叉引用

- `delivery/taskpackages/261001-m4-k3-record-session-live-repair/pod-events.jsonl` **在世**（4 条 human-decision）：`ef280e1a`（defer-merge-pending-live-round）/ `64a4012d`（scope-exclude-record-page-ui-fidelity）/ `e2ce7911`（accept-device-round-d2-fixed）/ `97c36553`（accept-coldbrowse-dropped-per-k3）。
- human-decision 源头亦见 spine：`delivery/changes/intake/261001-m4-k3-record-session-live-repair.md`。
- post-cutoff 口径：不产 session-digest / loop-health / loop-metrics / pod-journal。
