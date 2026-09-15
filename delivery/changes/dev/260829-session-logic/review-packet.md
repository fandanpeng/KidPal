# Review Packet — 260829-session-logic

> integrator 收口。合并门前审查包。任务包 = KidPal 包 3：会话状态机（聆听/思考/播报/超时/结束）+
> KEY/BOOT 全局按键手势解析（§4.0/§5.3 纯逻辑部分），host 测试（subsystem=firmware，riskTier=high，
> mergeOrder 6，milestone-affecting，dependsOn=260829-host-test-infra）。

## Change Size

- **Files changed**（本包 + closeout，全部相对 feature 分支 base，`git diff --stat` + 新增文件）：
  - 新增实现 `firmware/main/session_state_machine.{h,cc}`（83+148 行）、`firmware/main/key_gesture_parser.{h,cc}`（115+125 行）——纯逻辑、无动态分配、无 FreeRTOS/定时器依赖，时间戳/超时全部显式事件注入（host 可测 seam）
  - 新增冻结验收 `firmware/main/test/session_logic.acceptance.c`（673 行，9 TEST_CASE）
  - 接线 `firmware/main/CMakeLists.txt`（SOURCES 追加 2 行）、`firmware/main/test/host_app/main/CMakeLists.txt`（HOST_ACCEPTANCE_SRCS 追加 3 行 + 注释）
  - taskpackage 面 `.claude/taskpackages/260829-session-logic/`（taskpackage.json notes/redBaselineResult/mutationEvidence append + pod-events.jsonl + verify-runs.jsonl + verifier-evidence/rounds/r1~r4.json）+ `current/` 投影
  - closeout 制品 `changes/dev/260829-session-logic/`（本 review-packet + docs-delta）
  - `.planning/kidpal-mvp/progress.md`（closeout 一线 append）
- **Net diff**：纯新增 + 5 行注册接线；零改动既有产品代码（`device_state_machine` 等既有状态机不动）。
- **Scope**：单一子系统（firmware）纯逻辑单元 + host 单测。设备集成（真机按键/音频链路）与跨页端到端不在本包（SES-E2 显式留根包 9b）。
- **Split recommendation**：不拆。单包内聚，验收与实现 1:1。

## Verification Evidence

### Bearing machine evidence（closeout 判断来源）

- **final verifier-round fact**：`.claude/taskpackages/260829-session-logic/verifier-evidence/rounds/r4.json`（`authority:bearing`、`phase:final`、`round:4`、`ok:true`、`observedGateExit:0`，invocation `f991bafb-c80f-4536-a5f9-70ca00355ece`，2026-09-05T12:23:32Z）。
- **`verifier-evidence-check.mjs` 复验**（integrator，本收口）：`{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0,"phase":"final","round":4}`，EXIT=0。fact `ok:true` → closeout 判断成立。
- **r3 scoped fact**：`r3.json`（invocation `b384c91f-7242-4c85-8598-541f387af07a`，ok:true，7 闸绿），final 前置就位。
- **verifier 收敛轨迹（4 轮）**：r1 红（acceptance-coverage：builder 实现未对齐接缝）→ r2 红（mutation-check：mutation 证据缺口）→ r3 scoped 绿 → r4 final 九闸全绿。无跳轮、无豁免。
- **九闸明细（r4 results）**：taskpackage-required / firmware-build（**degraded=true**：`idf.py 不可用，固件编译未在本机核验（降级放行，非假绿）`——本包为 host 单测包，真实编译运行由下方 host runner 证据承载）/ contract-lint / acceptance-coverage / mutation-check / docs-sync（docsNoChangeReason 走 docs-delta「无需改」判定）/ security（securityReviewRequired:false）/ unit（executor-skipped=live-only on-target）/ acceptance（oracle-pass + executor-skipped=live-only on-target）。
- **current/verdict.json**：non-bearing 投影（producedBy `verify.mjs (script fs-write)`），closeout 判断不以它为准；`verification.json` 未产出（advisory-only，不阻塞 closeout-check）。

### Host runner 实测（引用现有证据，integrator 未改任何 firmware 文件故不重跑）

- **13 Tests 0 Failures EXIT=0**：host_infra_test.elf（ESP-IDF linux target unity runner）含本包 9 个 SES TEST_CASE + 包 0 既有 4 个 host_infra 测试，全绿。fixer 实测 2026-09-05（mutation 回滚复跑后钉在 taskpackage.mutationEvidence 各条 note：`回滚后 13 绿 EXIT=0`）。
- **红基线复验**（acceptance-author append，redBaselineResult）：红基线时 host runner 4 Tests 全绿但不含任何 SES 测试——验收文件未收集 + 接缝头不存在，统一红定性成立，逐条映射钉在验收文件尾注释；scratch 桩验证（契约一致最小实现，已清理）9/9 绿证明验收断言自洽、非测试假红。

### 验收映射（SES-Ay → TEST_CASE → 绿）

| 验收 | TEST_CASE（session_logic.acceptance.c） | 状态 |
|---|---|---|
| SES-A1（结构：接缝头存在、两机器可实例化落初始态） | 「纯逻辑接缝头文件存在且两台机器可实例化落初始态」 | 绿 |
| SES-A2（全转移链 + 60s 超时/结束意图/再按键收束 + 非法转移拒绝） | 「聆听思考播报全转移链与 60s 超时、结束意图收束」「非法转移逐条拒绝且状态不被改写」 | 绿 |
| SES-A3（KEY 短按启动/结束/打断停播 + 长按回首页） | 「KEY 短按启动结束会话打断停播与长按回首页」 | 绿 |
| SES-A4（BOOT 四页循环 + 对话中 BOOT 结束会话） | 「BOOT 短按四页循环翻页与对话中结束会话」 | 绿 |
| SES-A5（思考超 4s 安抚不冷场） | 「思考超四秒切入安抚播报且与正常回包可区分」 | 绿 |
| SES-A6（单击/双击/长按消抖 + KEY/BOOT 职责分离 + §4.0 手势表） | 「单击双击长按时序消抖判定正确」「KEY BOOT 职责分离全局路由表与 PWR 键位拒收」 | 绿 |
| 包 8 接合面（MANUAL_RECORDING 累积态进/出/生命周期） | 「manual 录音累积态切入退出接缝且生命周期不被破坏」（[pkg8_seam]） | 绿 |
| SES-E1（结构）、SES-E3（PWR 不入状态机，注释体现） | 结构断言 + 实现注释（key_gesture_parser 对 PWR 事件拒收） | 绿 |
| SES-E2（跨页端到端整合） | 不在本包——PROPOSAL §7 显式由包 9b 兜底验收（residual risk 留根） | 留根 |

### Mutation check（M1~M4，逐条敏感全真）

mutation-check 闸 PASS，M1~M4 破坏→变红→回滚三段式全部实证（taskpackage.mutationEvidence，fixer 实测 2026-09-05）：

- **M1**（SPEAKING 态 KEY_SHORT_PRESS 打断转移改拒绝）→ SES-A3 TEST_CASE line 384 红（13→1 Failures）；同时覆盖 SES-A2/A3 敏感性池——SES-A2 全链走 SPEAK_DONE 回聆听不经过该边，双向覆盖后全链断言仍独立命中。
- **M2**（key_page_next 页序 +1 改 +2，破坏 P4→P1 回绕）→ SES-A4 line 423 红（Expected 1 Was 2）。
- **M3**（THINKING_TIMEOUT_4S 安抚转移改拒绝）→ SES-A5 line 471 红。
- **M4**（双击窗 `<= double_gap_ms` 改 `<= 0u`）→ SES-A6 line 516 红（双击误判单击）。
- 全部回滚后恢复 13 绿 EXIT=0（md5 核对 a3593c53 / 988c4e58 还原）。SES-A6 的路由/PWR 拒收与 SES-E3 结构断言属结构/路由面，由 M4 + 既有断言矩阵承载（如实标注，非全点单 mutation 敏感）。

### 架构/依赖卫生与 detect_changes

- **detect_changes: N/A (worktree)**——`KidPal` 图索引建在主 checkout（C:/AI/AInativecompany），本 worktree 新增符号不在图内（已用 search_graph 抽查确认：session_state_machine / key_gesture_parser 均未入索引）。替代核对：`git diff --stat`（纯新增 + 5 行注册）+ 新增符号均为主构建 leaf（被 main.cc 之外的接线消费，尚未被既有调用图引用，依赖方向 firmware/main ← test/host_app 单向，无循环依赖、无跨层调用、无新增第三方依赖、无绕过 contracts 的本地类型——两接缝头为 C-compatible 纯逻辑接口，不触端云协议契约面）。
- 架构裁决：无 ADR 需求（纯逻辑 + 显式事件注入延续包 0 host 测试约定，非新架构面）。

## Simplification Debt

（grep `minimal:` 标记零命中；五轴第 5 轴「过度设计」：转移表驱动 + 单文件解析器，无重复实现——状态机与设备既有 `device_state_machine` 职责正交，deletion test「删掉接合面 MANUAL_RECORDING 态则包 8 需改冻结验收文件重开红基线」= 接合面预留有据（PROPOSAL v0.4 注），非投机设计。）

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `key_gesture_parser.cc` 手势解析 | 时间戳差值消抖（单/双/长按），不引入去抖中断/定时器抽象 | 需要真机消抖实测调参（缺省窗参数可在 header cfg 调） | 包 9b 真机联调手感不适配时换型 |
| `session_state_machine.cc` 转移表 | 平面二维表 + side-effect 标记位，不引入层级状态机框架 | 状态数增长（当前 6 态 × 11 事件）超出可读性 | 后续包若需嵌套/正交状态再评估 |

> 无 `no-trigger` 项；两行为具唯一 upgrade trigger（真机联调），不阻塞合并门。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 可运维性 | `espidf_wsl.sh` 当前检出为 CRLF 行尾，WSL 内直跑报错；本包 acceptance-author/builder 以等价 `idf.py` 直呼绕过 | pod-events friction 事件（2026-09-05，environment 类）+ redBaselineResult 环境备注 | 下游固件包 builder 留意；建议后续一次性归一 `.sh` 行尾（LF + .gitattributes），非本包阻塞 |
| FYI | judgement-call | 验证完整性 | r4 firmware-build 闸 degraded=true（本机 idf.py 不可用），固件整体编译（esp32 target，非 host target）未在闸内核验 | r4.json firmware-build stdout `degraded:true` | on-target 编译/验收由包 9b（全功能端到端真机）承载既有留根，无需本包动作；如实记录不虚报 |

> 无 BLOCKING / SHOULD finding。结构闸（r4 九闸）PASS/FAIL 结果不复述为 finding（findings 写法约定）；gate 全绿之上仍完整执行五轴语义 review 如下。

## Five-Axis Review

1. **Correctness**：转移表逐格对齐验收钉定接缝契约（IDLE 仅 KEY 启动、BOOT 不启动会话；LISTENING 三收束 + BOOT 也可结束 + 包 8 START_RECORDING 接合；SPEAKING KEY/END_INTENT 打断；MANUAL_RECORDING 生命周期语义不破坏且只从 LISTENING 进入）。非法转移拒绝路径 accepted=0、to==from、flags==0、计数器递增、状态不被改写——拒绝而非 crash，语义显式。无效事件 id（>=SES_EVT_COUNT）同样走拒绝路径，无越界读表。手势解析以 uint32 回绕安全时间差（`ts_ms - pending_ts <= window`）判定，双击窗与长按阈值互斥，PWR 事件拒收（SES-E3）。
2. **Test quality**：断言真实行为（转移 from/to/flags/accepted 计数逐字段断言，非「没报错」）；超时用显式事件注入（SES_EVT_SESSION_TIMEOUT_60S / THINKING_TIMEOUT_4S）而非真实定时器，host 可复现；统一红基线按条目映射 + mutation 四点真牙齿兜底逐条敏感性（verifier 不得豁免已声明并执行）。
3. **Contract/boundary safety**：契约面零触碰（contract-lint PASS，本包无端云协议暴露面）；冻结验收文件未改（builder/fixer scope deny 生效）；接缝头 location 由验收文件相对 include 钉定，builder 只能加 include dir 不能挪头。
4. **Operability/security**：纯逻辑无 I/O、无内存分配、无中断上下文假设，设备集成风险收敛在下游集成包；本包 security 闸 securityReviewRequired:false，人工复核同判（纯本地输入解析，无外部输入信任边界、无 secret 面）。
5. **Simplification/过度设计**：见 Simplification Debt 节——两处有据简化，零投机抽象。

## Residual Risk

- **SES-E2 跨页端到端整合**（「4 页循环翻页」跨页 host 阶段无专包认领）：按 PROPOSAL §7 显式由**包 9b（全功能端到端真机）兜底验收**——decisionCoverage `PROPOSAL-§7-跨页翻页归属` landsIn `exitCriteria:SES-E2`，非 dropped/risk-accepted 项，属计划内留根非残余风险敞口。
- **on-target 编译/验收未在本包闸内核验**：firmware-build degraded（见 Finding FYI）；固件 esp32 target 编译与真机验收由包 9b 承载。
- **手势消抖参数为 host 语义值**：真机手感（消抖窗/长按阈值毫秒值）待包 9b 真机联调调参（header cfg 可调，无结构返工）。
- **环境摩擦**：espidf_wsl.sh CRLF（见 Finding NIT）。
- **decisionCoverage dropped/risk-accepted 审计**：本包 `taskpackage.json.decisionCoverage` 三条（PROPOSAL-§2-包3 → SES-A2、spec-§5.2-会话模型 → SES-A3、PROPOSAL-§7-跨页翻页归属 → SES-E2）**均无 `landsIn: dropped` 条目**，无 human risk-accepted 决策需要列示。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired=true`，按纪律不执行 S2 official scan。

## Closeout 制品与合并准备

- `changes/dev/260829-session-logic/`：本 review-packet + docs-delta.md（post-cutoff：制品从 taskpackage 解析，不晋升 taskpackage 本体；不产 session-digest/loop-health/loop-metrics/pod-journal）。
- pod-events.jsonl 存在（5 事件：dispatch×4 + friction×1）——advisory 检查通过，非 blocking。
- 已 commit 到 feature 分支 `dev/260829-session-logic`（不 push、不合并主干）；回滚方式 = 分支 drop / `git reset` 至 base `85cc0c6`。
- 等待 supervisor 机械闸（`supervisor-verdict-gate.mjs`，由 planner 主线程跑）裁断；本 packet 不自行判定通过，不执行 merge。
