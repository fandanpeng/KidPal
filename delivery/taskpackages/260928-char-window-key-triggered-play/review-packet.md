# Review Packet — 260928-char-window-key-triggered-play

- 日期：2026-09-28
- 分支：dev/260915-m1-k1a-key-layer-dispatch（Pod worktree，未 push）
- 来源 spine：delivery/changes/intake/260928-char-window-key-triggered-play.md（需求面 + 架构面已收口 v1.59 落稿 546832c；实现面 Pod 循环本包）
- TaskPackage：delivery/taskpackages/260928-char-window-key-triggered-play/taskpackage.json
- riskTier：low · rigor：lite · gateProfile：test-quality · subsystem：firmware/main/display

## 1. 变更摘要

查字窗演示与播报解耦（对回 S-F3.1 AC1/AC3/AC5 + S-F3.3 AC1/AC2 + WF-F3-01 键帽随态变，
需求权威 = docs/requirements.md v1.59，commit 546832c 已落稿）。改动面 = taskpackage
changeManifest 七文件：

1. `char_page.cc/.h`——建窗首帧改静态整字（`charpage_render_frame(-1, g_page.n_strokes)`
   判每笔 WRITTEN，char_stroke.c 冻结相位复用）；`charpage_refresh_hint(0)` 摘除（初始
   提示条 = card_build 单源定格文案「写完啦·共 N 笔」，DC-3）；timer 建表即
   `lv_timer_pause`（紧跟 MCF-A1 末端锚 `g_page.timer = lv_timer_create` 之后原位，DC-2）；
   timer_cb REST 相位渲染定格帧 + 刷定格文案后 pause + return、不推进 elapsed（演完定格
   不循环，DC-1——取模回绕自动重播在装配层闸断，char_stroke.c 纯逻辑合约零触碰）；
   `replay()` 改归零 elapsed + 置 started + `lv_timer_resume`（首播/重播同路径），持
   LVGL 锁改态、锁外刷键帽，窗未开防御返回；新增 `kidpal_char_page_started()` 起播态外露。
2. `keybar.c/.h`——新并行纯逻辑函数 `kidpal_keybar_char_slots(started, &left, &right)`：
   started=0 →（播报， 结束）/ started≠0 →（重播， 结束）/ NULL out 防御 -1。additive
   取向（DC-4），既有 `kidpal_keybar_slots` 签名与值域零动。
3. `lcd_display.cc`——`kidpal_keybar_sync_locked` 加查字窗前台分支（front_layer ==
   KIDPAL_LAYER_CHAR_WINDOW 时走新函数 + `kidpal_char_page_started()`，否则既有路径
   原样）；`char_page` show 末尾与 replay 锁外均调 `kidpal_lcd_keybar_sync`（ACC-6 接线）。
4. 冻结验收件两件——`char_window_overlay.acceptance.c`（test 12/13：ACC-1/2/3/4 +
   sync_locked 分流结构齿）、`keybar_three_slot.acceptance.c`（test 6：ACC-5 行为齿 +
   keybar.h 声明结构齿）。

TTS 播报链路（application.cc 音频门 / mcp_server.cc view=char 路由 / protocols / server /
云端）零改动；播报弹窗即自动播的既有行为不变（v1.59 口径：播报与演示解耦，播报保持现状）。

## 2. 验收齿映射

| 齿 | 落点 | 覆盖 |
|---|---|---|
| ACC-1 | char_window_overlay test 12 | 建窗首帧 `charpage_render_frame(-1, n_strokes)` 静态整字（结构齿） |
| ACC-2 | char_window_overlay test 12 | timer 建表即 pause + `charpage_refresh_hint(0)` 零在世（零动画拍） |
| ACC-3 | char_window_overlay test 12 | replay = 归零 + started 置位 + resume（首播/重播同路径，结构齿） |
| ACC-4 | char_window_overlay test 12 | timer_cb REST 相位定格帧 + pause + return（不推进 elapsed） |
| ACC-5 | keybar_three_slot test 6 | kidpal_keybar_char_slots 决策表行为齿（0→播报/≠0→重播/NULL→-1）+ keybar.h 声明在世 |
| ACC-6 | char_window_overlay test 13 | lcd_display sync 查字窗分支 + show 末尾/replay 锁外 keybar_sync（结构齿） |

既有冻结齿存活：MCF-A1 timer 字面锚区段 / MCD-A2 / M2K3-A5/A6 头注齿、voice_round_ui
sync_locked 签名钉、schedule_grid rn_keybar_sync_locked 计数同跑全绿。

## 3. 红→绿证据

- 红基线（acceptance-author 2026-09-28 冻结相位实跑）：host runner 全量
  **106 Tests / 3 Failures**——三红全部 = 本包新齿设计态红（运行期红）：
  char_window_overlay test 12（ACC-1 首红 tooth：`(-1, n_strokes)` 不在世，现状
  `(-1, 0)`）、test 13（ACC-6：sync_locked 零 CHAR_WINDOW 分流）、keybar test 6
  （ACC-5：weak 哨兵 KKB_UNIMPLEMENTED）；既有 103 齿同跑零新增红。
- 绿态：builder 落地方案 A1–A6/B/C 后 **106 Tests / 0 Failures**（builder 全绿回执）。
- verifier 机械闸 scoped 轮（verify.mjs 产出，fact 随包 commit）：
  - r1（invocationId f0379643…）红：taskpackage-required——taskpackage.json 决策字段
    格式未定稿（contractRefs 缺失 / origin.type 缺失 / decisionCoverage 六条字段缺）。
  - r2（invocationId 9e7952a8…）红：taskpackage-required 已转绿；mutation-check 红
    （mutationRequired=false 缺 mutationSkipReason）。
  - r3（invocationId 3659c962…）**全绿**：taskpackage-required / change-manifest
    （pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 11% 余量）/
    contract-lint / acceptance-coverage（ACC-1～6 全覆盖）/ mutation-check
    （skipped，skip 理由合规）/ docs-sync / security 全过，`ok: true`、
    observedGateExit 0。
- **final bearing 轮待 planner**：integrator 已落 scope 清单
  `delivery/taskpackages/260928-char-window-key-triggered-play/final-scope.txt`
  （六测试文件全列，逐一核证在盘），final `--phase=final --scope-file` 轮由 planner
  以 final completed-only round 与 invocationId 跑——本 packet 提交时 final fact 尚未
  产出，如实申报（final evidence-checker PASS 为 closeout 前置）。

## 4. docs-sync 状态

- docsSyncRequired=false（对账结论全文见同目录 `docs-delta.md`）：spec 面 v1.59 已由
  需求面档 2 落稿 commit 546832c（requirements + architecture 镜像句同批），本包纯
  实现面零 docs 改动；架构镜像句不重改。零触 `docs/contracts/**`。

## 5. 遗留 / 未覆盖面申报

- **真机轮（human 持板）deferred——不阻塞 supervisor**：静态整字首帧目检、KEY 短按
  起播、再按从头重播、演完定格不自动重播（演完 2s+ 观察）、键帽随态变（播报↔重播）、
  TTS 播报口径不变（弹窗即自动播）、5 分钟无操作关窗。host 结构齿钉源面字面在世，
  真机行为归本条覆盖；human 持板轮另约（launch 指令明示登记）。
- **结构齿面限**：ACC-1/2/3/4/6 为源面字面/区间在世齿（host 编译期结构核证），非行为
  仿真——逐笔渲染效果、动画节奏（950ms/笔）真机观感不在 host 齿能力内，由上条真机轮
  覆盖。行为齿仅 ACC-5（kidpal_keybar_char_slots 三输入值域 + NULL 防御）。
- 本包非 workstream 包，demo 对账行不适用（真机轮即 demo 形态，deferred 已申报）。
- 除上述两条外无其它未覆盖缺口（明写：无）。
- **【销账 2026-09-28】真机轮已执行、human 确认功能 OK**：app 分区烧录（ota_0 @ 0x20000，
  3.2MB hash 校验过、NVS 保留）→ 启动观测 0 复位循环 / RX 48KB / uptime 平滑；串口实录
  KEY 短按 → `key_route_dispatch action=5`（GLOBAL_ACT_CHAR_REPLAY）→
  `char page replay from first stroke`，新起播路径实弹触发无崩溃、SRAM 平稳（~63KB）；
  human 持板逐项验收（上行清单七项）确认「功能 OK」（2026-09-28 会话原话）。本条销账后
  本包无未覆盖缺口（明写：无）。

## 6. 五轴语义 review（author 自审）

- 正确性：REST 相位闸断取装配层（pause + return 不推进 elapsed），char_stroke.c 纯逻辑
  合约零触碰；replay 防御窗未开/表未建悬垂句柄；started 语义「非 0 即已起播」不为 1
  特判留缝（test 6 tooth 3 守护）——正确。
- 契约/边界：零触 docs/contracts/**，端→网关消息与 wire 形状零变化（crossEnd.exempt）；
  键帽值域走 keybar 纯逻辑单源、设备侧零重算；提醒弹窗压查字窗时 front = REMINDER
  不进查字窗分支（提醒键帽不受扰）——无越界。
- 安全：无新增攻击面（呈现层状态机与文案函数，不透传外部输入；replay 锁超时 ESP_LOGE
  退回，不带锁返回）。
- 可运维性：真机轮 deferred 已申报（§5）；键帽新值域与既有键帽体系并行函数隔离，回滚
  = revert 五文件即可（冻结验收件随回滚面一并 revert）。
- 简洁性：~100 行实现面 + 新并行函数一枝，无新增抽象层；keybar.c 文件末尾缺换行符为
  HEAD 既有状态非本包引入（git show HEAD 核证），不判本包 NIT。

## 7. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD / NIT） | — | — | 本批未发现需登记 finding | — | — |

## 8. Simplification debt（minimal: 标记台账）

- grep `minimal:` 于 changeManifest 五实现文件：零命中 → `no-trigger`，无简化债务台账条目。

## 9. Residual risk（decisionCoverage risk-accepted 条目）

- DC-5「演完歇 2 秒自动重播取消」— `landsIn: dropped` — reason:
  `human 2026-09-26 明示「同意取消（演完歇 2 秒自动重播）」`——reason 非
  `risk-accepted` 前缀（为 human 明示需求裁决、语义已由 S-F3.1 AC3 v1.59 定格不循环
  承载），按模板判据不列 risk-accepted 清单，此处如实说明。
- DC-6「不缓存、按 KEY 现取笔顺数据路线撤回」— `landsIn: dropped` — reason 为路线
  撤回（非 risk-accepted），零实现义务，不列。
- 综上：`decisionCoverage` 中无 `landsIn === 'dropped' && reason.startsWith('risk-accepted')`
  条目，无 D\<n\> 清单行。

## 10. S2 exploratory scan

- TaskPackage `s2ExploratoryRequired=false` → 未执行，无 S2 证据。

## 11. 变更影响面核对（detect_changes，advisory 记账）

- per-Pod 图索引可用（项目 C-AI-KidPal-pod-m1-k1a-key-layer-dispatch，launch 面必办项
  在位）→ **正常圈定，无降级**。detect_changes（scope=files，base=main，merge_base
  4b55a1b）：changed_files 27 与本包申报面（changeManifest 七文件 + taskpackage 目录
  + current/ 投影 + spine + 需求面档 2 已 commit 件）逐项对上，无溢出实现符号面；
  范围判定归 Step 4——scope 清单已按 impactedTests 种子合成落盘
  `delivery/taskpackages/260928-char-window-key-triggered-play/final-scope.txt`
  （本行为 test-scope 清单落盘路径锚，供 final verifier fact results 元数据引用）。
- 全量回归 = 人工触发位，不圈全仓。

## 12. 制品清单（晋升面）

- `delivery/changes/dev/260928-char-window-key-triggered-play/review-packet.md`（本件）
- `delivery/changes/dev/260928-char-window-key-triggered-play/docs-delta.md`
- scope 清单：`delivery/taskpackages/260928-char-window-key-triggered-play/final-scope.txt`
- 承重机器件：`delivery/taskpackages/260928-char-window-key-triggered-play/verifier-evidence/rounds/r1-r3.json`（verify.mjs 产出，随包 commit；r3 = 最高已产 bearing fact）
- verdict.json（current/ 投影，script fs-write 产出）/ verification.json：final 轮后
  由 planner 侧产出投影——本 packet 提交时仅有 scoped r3 对应 current/verdict.json
  （non-bearing compatibility projection），closeout 判断以 r3 fact + planner final fact
  为准，advisory-only 如实申报。
- pod-events.jsonl / verify-runs.jsonl：存在于包目录（advisory 检查通过，非 blocking）。
- session-digest / loop-health / loop-metrics / pod-journal：Post-EVENTS_SCHEMA_CUTOFF
  不产；pod-retro.md 归 planner 收尾相位。
- ADR：adrRequired=false（DC-1/2/3/4 均为 plan.md 内既有取舍，无新架构裁决），未产。
- milestones / workstreams：本包非 workstream 包、无里程碑跳变登记义务（deliveryImpact
  = milestone-affecting 的里程碑行更新若需，由主线程 delivery-status 流程承接——本包
  未新增 milestone 条目变更，不跑 closeout-check --update-index 写路径之外的动作）。

## 13. 交门状态

- 本 packet + docs-delta + scope 清单 + 分支 commit 完成后交 planner：跑 final bearing
  verification（--phase=final --scope-file final-scope.txt）→ verifier-evidence-check
  → supervisor-verdict-gate 机械闸。integrator 不自行判定通过、不执行 merge。
