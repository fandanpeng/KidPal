# Review Packet — 261022-diag-log-content

> integrator 合并门前审查包（2026-10-22，dev/pod-m2 worktree，基线 cbc8f72a）。verdict：**停在合并门前**——七闸唯一红 docs-sync 的落盘主体不在 integrator 写权内（见 Findings F1），交回 planner 落盘后复跑 final verify 再进 closeout/commit。

## 包事实

- changeId `261022-diag-log-content` · sourceType `human-ad-hoc`（human 两段原话：①日志无价值——至少记页面与状态跳转；②连时间都没记）· rigor `lite` · riskTier `medium`。
- 验收：DLG-A1..A5 host 自动齿（`firmware/main/test/diag_log.acceptance.c`）+ DLG-A6 structural 真机轮观测项（human 持板，Pod 到门停等）。
- 零 wire 形状改动（triage 四面探测裁 NO_CONTRACT_CHANGE），`server/**` 零 diff。

## Change Size

- Files changed：**9 实现面文件**（7 modified + 2 新建 log_semantics.c/.h）+ docs 待落盘 1 句 + taskpackage 投影 3 件 + verify-runs.jsonl；净 diff 实现面 +127/−10（`git diff --stat`）。
- Scope：firmware/main/diagnostics（新纯逻辑单源 + 钩子拼接）、application.cc、device_state_machine.cc、kidpal_render_node.cc、waveshare 板、test 登记两件 CMakeLists。
- Split recommendation：单包不分拆（lite 档单缝主题，五缝共享同一验收件）。

## Verification Evidence

- **结构闸**（verify.mjs round 2026-10-22T03:37:41Z，`delivery/taskpackages/current/verdict.json`）：taskpackage-required ok / change-manifest ok（pre-cutoff 存量放行）/ firmware-build ok（lite 包 scoped 相位 executor-skipped，final 恒跑）/ contract-lint ok（1 条先存断链 warning，非本包）/ acceptance-coverage ok（A1..A6 全覆盖）/ mutation-check ok。**docs-sync FAIL**（唯一红）。
- **host 验收**：红基线 → 绿轨迹——acceptance-author 冻结实跑 5 Tests 5 Failures（设计态红 verbatim 落 taskpackage `redBaselineResult`，冻结件 sha256=2fd2efe8…838）→ builder/fixer 后 host 全量 **227 Tests 3 Failures**（227−222 = 5 本包新绿，3 Failures 全为先存 p1_auxbar AB-A1/A2/A4，锚 5412af06，零新增红零扰动，DLG-X1 满足）。
- **设备编译闸**（DLG-X2）：VERIFY_FW_BUILD_REQUIRED=1 真编译通过；[C4] 三锚保持绿；落卡管线口径零 diff（DLG-X3）。
- **Mutation check**：5/5 functional acceptance id 各一条真树变异，**全 sensitive**（变异即红、复原逐字节一致 + 全量复跑回 227T3F）：
  1. DLG-A1 摘零填充（格式串 `%04d…`→`%d…`）→ `:716` 长度非 22 红；
  2. DLG-A2 页族 fallback 退化空串 → `:894` page 3 伪语义名红；
  3. DLG-A3 删 interval<=0 防线 → `:949` 零间隔红（Expected -1 Was 1）；
  4. DLG-A4 钩子摘除 `kidpal_log_time_cache()` 消费（m9 存在性锚）→ `:1055` 前缀未拼接红；顺序锚（p_prev<p_cache<p_write）由 acceptance-author /tmp 合成树 m1/m2 取红证；
  5. DLG-A5 host 登记摘除 sd_log.acceptance.c → `:1244` 回归网缩网红。
- **Docs sync**：FAIL——详见 Findings F1；verbatim 修复句已备妥 `docs-delta.md` §2，落盘主体 = 持 spec 写权者。
- **final bearing verification**：**未执行**（scope 清单已落盘 `scope-final.md`，final round/invocationId 待 planner 于 docs-sync 转绿后签发；scope-file 路径一行：`delivery/taskpackages/261022-diag-log-content/scope-final.md`）。

## 简化债 / 取舍申报台账

- **D1 扩单留痕**（architect 裁决 2026-10-22，taskpackage `changeManifestNotes`）：append `firmware/main/CMakeLists.txt`——[DLG-C5] 明示要求 + 设备链接必需（sd_log_fs.cc 消费 log_semantics 符号）+ 单行显式 SOURCES 登记零行为面；非范围扩张非升档。
- **builder 取舍申报**：
  - **D2** `rn_render_kidpal_page_locked` 保持单参——retract_p4_three_page.acceptance.c M2K2-A1 冻结签名文本锚，改签名打红冻结回归网；触发源改走 `rn_note_page_switch_trigger` 旁路标注（单写者 = 持 lvgl 锁调用方，读点同锁区间复位默认），保 retract 齿绿。
  - **D3** idle 超时 trigger=key 边界——idle 超时回页未标注 NET/TIMER，落 key 默认值（s_page_switch_trigger 复位语义）。
  - **D4** show_home 回首页**不产 PAGE_SWITCH 行**——plan 冻结面内边界（首页渲染不经 rn_render_kidpal_page_locked）；如需补齐走 tighten-only 修订，不在本包扩面。
- 简化债（`minimal:` 标记，本包新增 1 条）：

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/diagnostics/log_semantics.c:65 | 时间缓存两槽 + RELAXED/RELEASE，非严格 seqlock | 读侧可能读到跨 tick 拼接对（同秒内前后缀对不齐），可容忍 | 若要求「同一行时间恒与日志内容原子一致」→ 升严格 seqlock；no-trigger（现口径降级标注已覆盖） |

（waveshare-s3-rlcd-4.2.cc:451 的 minimal 标记为先存（K5 time_sync 面），非本包。）

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| BLOCKING | hard-violation | docs-sync | docs/architecture.md 的 DLG 锚句 integrator 无写权落盘，verify 七闸保持一红，closeout 无法闭合 | docs/architecture.md 属 spec 面：scope-map `agents.integrator.allow` 不含两份 spec 文件 + human 裁定 ARQ-260929-docs-sync-spec-handoff（integrator 对 spec 零自改、交持写权者）；hook enforce-scope.mjs 实拦（Edit 被拒） | planner 落盘 docs-delta.md §2 verbatim 句（主线程 packet-bound 或派 architect）→ 复跑 verify.mjs 全绿 → 签发 final round/invocationId 跑 final verify + verifier-evidence-check → 回 integrator 收口 commit |
| NIT | judgement-call | device_state_machine.cc | 被拒转移日志字面 "Invalid state transition: %s -> %s" 改为 "state_reject %s -> %s"——串口 grep 习惯的消费者（观测 skill / 真机轮）需感知新字面 | changed-code hunk device_state_machine.cc:115-125 + heuristic（观测面字符串变更）；日志明文无契约权威（板端自定义行格式，261018 已裁非端云数据格式） | 无强制动作；真机轮操作单已含状态机行核验项，随 DLG-A6 自然覆盖 |

（结构闸 PASS/FAIL 不复述为 finding；七闸中六绿已记 Verification Evidence。）

## Five-Axis Review

1. **正确性/契约**：PAGE_SWITCH / state_reject / 语义名行全部走既有 esp_log 流，不新增数据类、不触 261018 隐私暴露面（页名/键名/状态名非 PII）；corr_id 纪律保留；`n` 应写长度 + 前缀长合计截断保 NUL 位（sd_log_fs.cc 体内），prefix_len 越界守卫（32 << 512 不可能态仍防）。
2. **安全**：零新增依赖、零新文件权限面；钩子只拼接不格式化（R1 ISR 缓解消费半），产出半在 application.cc 1s clock tick（低频任务上下文）；fallback 闭集外不崩（DLG-A2 变异已证）。
3. **并发**：s_page_switch_trigger 单写者（lvgl 锁持者）读点同锁区间；时间缓存两槽单写多读（简化债已记 ceiling）。
4. **边界/可运维性**：BOOT+ms 降级保开机后先序；SystemInfo 60s 降频砍导出日志约 70% 篇幅；`KIDPAL_LOG_SYSINFO_INTERVAL_MS=60000LL` 头文件单源禁裸字面（DLG-A3 纪律）。
5. **简约**：五缝全部最小面——纯逻辑单源 C 文件 + 各缝 1-5 行接线；无投机抽象（deletion test：log_semantics 删掉则格式化/映射/降频决策散落 5 个调用点，复杂度扩散 = 深模组信号，留）。

## Residual Risk

- Known limitation：
  - D4——show_home 回首页无 PAGE_SWITCH 行（plan 冻结面内边界，tighten-only 可补）；
  - D3——idle 超时 trigger 落 key 默认值，不区分真实触发源；
  - 时间缓存两槽非原子对（见简化债行）；
  - 首条 SystemInfo 在开机 60s 后才出现（上次放行记 0 = 开机起点口径）。
- Follow-up recommended：DLG-A6 真机轮四项观察后如发现记录面缺口，走 tighten-only 修订（同 D4 口径）。
- Human decision needed：**DLG-A6 真机轮操作单**（human 持板，Pod 到门停等 status=awaiting-human:real-device，不自行烧录）：
  1. 导卡逐行核五类记录面——①每行时间前缀（含未同步 `BOOT+<ms>ms` 降级形态）②PAGE_SWITCH from→to+trigger ③状态机跳转行（含被拒 `state_reject`）④按键 `key_route_dispatch action=N(语义名)` / `key_gesture_parser key=N(名) gesture=N(名)` ⑤SystemInfo 降频至 60s 一条；
  2. 串口 COM5 不中断（钩子链式转调在先）；
  3. 无钩子递归迹象（无日志行自吞/重复放大）；
  4. 轮转与 dropped 计数正常（16MB 封顶口径未动）。
- decisionCoverage：taskpackage 无 `decisionCoverage` 字段（lite 档未启用），`landsIn: dropped` + `risk-accepted` 条目为空集，无 human 风险接受项需列。

## 白盒复核（actual diff vs changeManifest）

7 条 glob 逐条对账（`git diff --stat` + untracked 清点）：

| changeManifest glob | 实际 diff | 对账 |
|---|---|---|
| `firmware/main/diagnostics/*` | sd_log_fs.cc（+19/−？）+ 新建 log_semantics.c/.h | ✓ |
| `firmware/main/device_state_machine.cc` | state_reject 行 +6/−2 | ✓ |
| `firmware/main/display/kidpal_render_node.cc` | PAGE_SWITCH + trigger 标注 +39 | ✓ |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/*` | 语义名挂接 +9/−3 | ✓ |
| `firmware/main/application.cc` | 时间缓存产出 + SystemInfo 降频 +46/−4 | ✓ |
| `firmware/main/test/**` | host_app/main/CMakeLists.txt 登记 +11；diag_log.acceptance.c 为 acceptance-author 冻结件（本树已含，未再动） | ✓ |
| `firmware/main/CMakeLists.txt` | log_semantics.c 登记 +7（D1 扩单，architect 已裁） | ✓ |

零 manifest 外实现面文件。detect_changes 复核（worktree 图索引可用，未走降级）：impacted 33 符号全部落 firmware/main 预期集群（Application 日志消费面 / 板级按键路径 / 渲染节点 / mcp_server 故事页回落），无预期外符号——详 `scope-final.md` §2。范围判定归 scope-final（Step 4），本节只记账。

## 状态与未竟项

- closeout-check：**未跑 --update-index 收口**（闸红状态跑 --update-index 只会登记红态；待 docs-sync 转绿后由 integrator 复召执行）。
- commit：**未执行**（指令条件 = 三 blocking 制品齐 + 闸绿；闸红不 commit，避免 feat 提交定格在一红状态）。
- 本包到真机门前停等 human（DLG-A6），Pod 不自行烧录。
