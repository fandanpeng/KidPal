# Review Packet — 260829-schedule-p2（包 6：课表页 今天/明天渲染 + 语音问课表 + 家长录入→可消费链路）

> integrator 产出 · 2026-09-07 · 分支 `dev/260829-schedule-p2` · 供 supervisor 合并门复核。
> 交付本体：契约 B1 课表 MD 的消费链路首次落地——孩子翻到 P2 就知道书包该背什么。设备面（P2 渲染 + 两态圆点 + BOOT 切换 + 语音定位）+ 云面（B1 解析/筛选/决策 26 producer 分组 + prompt v2 课表 producer 扩展）+ 录入面（QQ→OpenClaw 改写 schedule.md→可消费）。
> 口径注记：本包 spec §4.1 已砍「当前课反白高亮」（P2-E3，2026-08-29 human 裁定），课表页只承担「看安排」；提醒触发归包 7（P2-E4，`remind_before_min` 废止不消费）。

## Change Size

- Files changed: firmware 面 7（新建 `display/schedule_page.{c,h}` 742+88 行、冻结验收 `schedule_p2.acceptance.c` 981 行、红基线摘要 1、修改 `CMakeLists.txt`/`mcp_server.cc` +55/`host_app CMakeLists.txt` +8）+ server 面 7（新建 `gateway/schedule.py` 339 行、冻结验收 `test_schedule_p2.acceptance.py` 719 行、`openclaw-system-prompt.v2.md` 96 行、`openclaw-schedule.config.yaml` 11 行、修改 overlay 三件）+ closeout 制品 2（review-packet / docs-delta）+ taskpackage 面（`.claude/taskpackages/260829-schedule-p2/` 全套 + `current/` 投影）+ `.planning/kidpal-mvp/progress.md` append
- Net diff: 实现（非验收/非制品）约 +1200 行，双面 additive 为主（mcp_server.cc case 4 为既有 switch 追加分支，G 包网关代码面 additive 纪律遵守——`server/gateway/` 仅新增 `schedule.py` 独立纯模块，未触碰他人文件结构）
- Scope: firmware + server 双面（taskpackage `subsystem` 钉定）；`docs/contracts/**`、`docs/spec.md` 零改动（contract-lint PASS + git diff 确认）
- Split recommendation: 不拆——设备渲染消费面与云解析/producer 面由同一契约 data 形状钉死（`{title, items:[{time,name,location}]}` 零增减），拆开任一面都会出现中间态红；mergeOrder 9 单包收口

## Verification Evidence

- **Structural gates**: verifier final round 3 九闸全 PASS（fact `ok:true`，`observedGateExit 0`）——`.claude/taskpackages/260829-schedule-p2/verifier-evidence/rounds/r3.json`（invocationId `c4d9e6f0-2a3b-4c8d-9e1f-0a4b6c8d2e59`）。`verifier-evidence-check.mjs` 复核 PASS：`{"factOk":true,"observedGateExit":0,"phase":"final","round":3}`（integrator 实跑 EXIT=0）。收敛轨迹：round1 scoped 红（mutation-check 无执行证据）→ builder mutation 矩阵 16 点取证 + planner 规范化回填 taskpackage mutationEvidence → round2 scoped 绿 → round3 final 绿。`verdict.json`/`verification.json` 未产（机器产线件，缺失按 SSOT 表 advisory-only 记录，closeout-check 预期 advisory 不拦）。
- **Acceptance（integrator 本机独立复跑，2026-09-07）**:
  - python 侧：`pytest acceptance/test_schedule_p2.acceptance.py` → **12 passed, 1 skipped**（skip = `@pytest.mark.live` 机械闸，ARQ-260901-g3 口径；真录一轮殿后见 Residual Risk）。
  - C 侧 host runner（WSL 执行 `host_infra_test.elf`）：**39 Tests 2 Failures**——schedule 7 个 P2 TEST_CASE（P2-A1 装配结构 / A2 device face / A3 两态+真帧像素 / A4 手势 / A5 定位+续接）**全 PASS**；2 失败 = **WSA-A4 / WSA-A6 先行红**（烧录/ECS 收尾证据文件为本地-only gitignore 件，落 WS 包 worktree，本 worktree 缺失所致，与本包面零交集——builder 回报 3F 含 VRT-A2/E2，本机复跑时该证据齿在世通过，差 1 条即先行红证据在位性差异，如实记录）。
  - 红基线（`firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md`）：C 侧 6T5F exit=5 / python 10F2P1S，红全落缺失行为（weak 哨兵 / schedule.py 模块缺失），无伪红——本包把它做到绿。
  - host runner 退出码缺陷（HOSTEXIT=0 与 2 Failures 并存）：先行缺陷非本包引入，见 Findings #3；绿判据以打印行为准（39 Tests 2 Failures 逐条核对，schedule 面零失败）。
- **Mutation check**: 16 变异点 = **12 敏感转红**（weekday 错移 5 齿联红、过期<=匹配 2 红、脏行静默跳过 3 断言失守、dot_state 硬编码双齿红、BOOT click 误触发、KEY 吞长按、time-only 定位、A6 分组丢项 3 红、prompt 载荷 v1 化双断言红、E4 remind_before_min 进模型双断言红）+ **3 不敏感**（齿粒度局限如实记录：P2-A1 删消费调用点 tooth 文件粒度、P2-A3 去 stable sort 同刻双项断言缺失、P2-A5 子串反例缺失——各附 tighten-only 建议）+ **1 变异点空缺**（P2-A6 parse 缓存：本实现为无状态纯函数无该面，stale-cache 缺陷归 A6 live 真录承载）。每变异 sha256 还原核销。
- **Security gate**: PASS（securityReviewRequired=false）——无 secrets；解析面脏行大声拒绝（ValueError 不静默）、畸形 data 显式降级不盲渲染（V27 语义）、7 view 白名单/65536 cap 守卫在世（boundary 齿绿）。
- **Docs sync**: PASS（r3.json docs-sync 闸）——spec/contract 零改动，判定与理由见 `docs-delta.md` §A（无需更新）。
- **变更影响面核对（detect_changes 降级）**: 分支基点 = main tip（ec4851b），全部交付物在工作树未提交 → detect_changes 基 git ref 无 diff 可算，按规程以 `git diff --stat` + 抽查替代：图索引对新符号 `kidpal_schedule_page_render` 零命中（未提交面不入图，如实降级），改以 grep 抽查跨包共享面——`openclaw-system-prompt` 引用面全查：既有 `test_voice_roundtrip_b.acceptance.py`（VRT-A4/A7）引用 v1.md，v1 文件本包未删未改，overlay 渲染源切 v2 不影响既有验收（本机 python 复跑 12P 佐证套件间无连带红）；`mcp_server.cc` case 4 为既有 display switch 追加分支，default 降级路径未动。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/schedule_page.c:622 | 布局常量按 host seam 400x300 定坐标（列表纵向排布）；行数超屏不滚动（P2 只承担「看安排」） | 真实屏幕观感（1bpp RLCD 真机）殿后 9b（P2-E2 口径） | 真机观感实测后按需调布局常量 |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD→已登记 | hard-violation | test-infra | host runner 退出码缺陷：`host_app_main.c` `exit(UNITY_END())` 未把失败数传导给进程退出码（包 0 既有面，本包未改其输入）——39 Tests 2 Failures 时 exit 0，机械消费方若以 `$?` 判绿会假绿 | Basis: 稳定规则 = 红基线摘要钉定的 runner 判据「exit code = 失败 TEST_CASE 数」（[C4.4]，acceptance-author 冻结纪律）vs 本机实测 HOSTEXIT=0；changed 无关（先行缺陷），但本包以打印行为判绿即受其遮蔽 | 交回 planner：建议后续包升级 host_app_main.c 退出码传导（acceptance-author owner）；机械闸以 stdout「Tests N Failures」行为准（本包按此执行） |
| FYI | judgement-call | server | group_upcoming「上学」项进行中语义为 prose↔test 张力点的实现取解：合并「上学」项视为自 school_start 起至当日下一事项开始前处于进行中（end := 当日下一事项 start，无下一事项回退 start-only），满足全部冻结断言；与 spec S-F1.1「合并成一项 XX:XX 上学」无冲突，schedule.py docstring 有注记 | Basis: changed hunk `server/gateway/schedule.py` docstring + group_upcoming 实现（heuristic: 冻结断言全绿 + 语义注记可溯）；builder 六条偏差中唯一实质点 | 无需动作；如后续 producer 语义扩展触发歧义，走 acceptance-author tighten-only 或 spec 校准 |
| NIT | judgement-call | firmware | `is_tomorrow` = title 含「明天」的设备侧最小判定：producer↔设备存在 title 词汇表隐式耦合（producer 换措辞则明天视图圆点两态失准），data 形状零变更下的 minimal 实现，代码注记钉明升级路径（producer 显式约定 title 词汇表） | Basis: changed hunk `firmware/main/mcp_server.cc` case 4 注记 + minimal 标记（heuristic: minimal-implementation 升级路径显式化）；locate_row 端到端接线同款（留语音消费路径） | 无需本批动作；词汇表约定若成形归 producer 侧后续包 |
| FYI | judgement-call | firmware/build | 设备树真编译未本地验证：WSL 无 xtensa toolchain，firmware-build 闸降级放行（非假绿，闸 reason 如实）；mcp_server.cc case 4 语法对齐既有 case 模式（cJSON/app.Schedule/display 取板模式同款），host 侧同源编译 BUILD_CLEAN 佐证 C 语法面 | Basis: changed hunk 语法模式对照 + verifier r3.json firmware-build degraded reason（heuristic: 不虚报已验范围） | on-target 编译/真机验收归 9b（与包 2/3/WS 同口径留根，非本包新增风险） |
| FYI | judgement-call | test-quality | mutation 3 不敏感点 + 1 空缺点（详见 Verification Evidence 段）：全部如实记录、已还原 sha256 核销、附 tighten-only 建议（同刻双项排序断言 / 子串反例配对 / tooth 区分声明-定义-调用形态）；无敏感缺口被掩饰 | Basis: changed 证据 = taskpackage mutationEvidence notes 逐条 + builder pod-events 事件 afbc6785（heuristic: mutation 诚实记录纪律） | acceptance-author 后续 tighten-only，不阻塞本包 |
| FYI | judgement-call | live-baseline | P2-A6 live 真录一轮（QQ 录入→OpenClaw 自改写→即时重算→问课消费）+ ECS 部署/live 冒烟殿后（operator/9b 口径）；offline 套件对 stale-cache 场景不可观测（变异点空缺同理），live 为唯一承载 | Basis: taskpackage acceptanceTestsFile live 标记 + 红基线摘要 live 面口径（heuristic: probe ≠ exercise） | operator 殿后项承接；证据落 `changes/dev/260829-schedule-p2/` |
| FYI | judgement-call | cross-package | 本 worktree 复跑 host runner 的 2 失败（WSA-A4/A6）为先行红：烧录/ECS 收尾证据 = 本地-only gitignore 件落 WS 包 worktree，本包零交集；builder 环境 3F 含 VRT-A2/E2 的差异同为先行红证据在位性差异，不构成本包回归 | Basis: changed 无关——`device_ws_switch.acceptance.c` 证据齿读 `server/acceptance/_artifacts/device-ws-switch/`（WS 包口径），本包 schedule 7 齿全 PASS | 无需动作；WS 证据跨 worktree 在位性问题如实引证，避免误记为本包残余 |

## Five-Axis Review

1. **Correctness**: 冻结离线契约 5 函数双面强定义（C weak 哨兵被覆盖、python 纯模块过真实 display_schema 门）；行为齿全绿——B1 三类区块解析（脏行 ValueError 大声拒绝、过期忽略）、按星期/当前时间筛选、决策 26 分组（「上学」合并 + 培训班/一次性逐项 + upcoming 进行中保留/已结束剔除）、圆点两态（纯逻辑齿 + LVGL 真帧像素齿双层钉）、BOOT 长按切今天⇄明天 + KEY 长按全局回首页不被页内吞、time+name 双匹配精确定位（strcmp 非子串）。边界条件：schedule data 畸形/缺字段 → 显式降级友好提示不盲渲染（V27 语义，corr_id=kdp-<tick> 可追溯）；RTC 未同步 → 00:00 全实心降级。波及面：mcp_server case 4 仅追加、default 降级路径未动（grep 核实）。
2. **Test quality**: 冻结验收断言真实行为而非自证（红基线 6T5F/10F2P1S 红全落缺失行为，weak 哨兵机制保证「契约未实现即红」）；守卫齿在世（boundary no-view-invention、E1 回归 gateway-directionB 套件、E4 remind_before_min 不消费双断言）；mutation 16 点含敏感 12 全转红 + 不敏感 3 诚实记录粒度局限；既有验收套件无连带红（python 12P 本机复跑）。
3. **Contract/boundary safety**: `docs/contracts/**` 与 spec 零触碰（contract-lint PASS + git diff 确认）；契约 §1.3 ② data 形状零增减——星标 = 设备侧渲染标志不进 data、`school_start` = producer 显式入参 + repo 配置载体非契约字段、B1 表体零变更；`remind_before_min` 废止不消费（2026-09-03 契约门裁定）有齿钉死；G 包 additive 纪律遵守（schedule.py 独立纯模块，gateway 他人文件零结构改动）。
4. **Operability/security**: 无 secrets 变更；overlay v1→v2 切换为版本化演进（v1 在世可 diff、回滚 = 三处接线引回 v1）；school_start 改配置即时生效永不烧板（同三档提醒口径）；降级路径可观测（ESP_LOGW degrade=true + corr_id）；prompt v2 的「验收所测 = 生产所用」链路保持（渲染面/声明面/消费面三处同步切换）。
5. **Maintainability/simplicity**: 单一 `minimal:` 标记（渲染布局常量，真机观感殿后 9b，见 Simplification Debt）；`is_tomorrow` 最小判定 + 升级路径注记显式化；schedule.py 纯函数无依赖（offline 可导入，同 display_schema 先例）；无新依赖引入；重复实现检查：C/python 双面为「设备渲染 vs 云解析」不同职责面而非重复（契约 data 形状为唯一接缝）；无投机抽象（deletion test：删 5 函数任一即对应验收齿红）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired`——未执行 S2 official scan，无 S2 evidence、无 knowledgeSuggestion 持久化项。

## Residual Risk

- Known limitation:
  - 设备树真编译未本地验证（xtensa toolchain 缺位，firmware-build 闸降级）——on-target 验证归 9b，与本包 C 语法面证据（host BUILD_CLEAN + 同源编译）组合可信但非实机证明。
  - P2-A6 live 真录一轮 + ECS 部署/live 冒烟殿后（operator/9b 口径）——offline 套件对 QQ 录入端到端与 stale-cache 场景不承载，live 为唯一证明面。
  - host runner 退出码缺陷（Findings #3）——机械消费方须以打印行为判绿。
  - mutation 齿粒度局限 3 处 + 变异点空缺 1 处（Findings #5）——对应行为由冻结断言之外的证据形态（live / 后续 tighten-only）补承。
  - producer↔设备 title 词汇表隐式耦合（Findings #3 NIT）——minimal 取解，升级路径已注记。
- Follow-up recommended: 后续包升级 host_app_main.c 退出码传导；acceptance-author tighten-only 三齿；operator 承接 A6 live + ECS 冒烟；9b 承接 on-target 编译/真机观感。
- Human decision needed: 无本批新增。`decisionCoverage` 四条 decision 全部 `landsIn` acceptance/exitCriteria 齿（P2-A2/P2-E1/P2-E3），无 `landsIn: dropped && reason: risk-accepted` 条目，故本包无 risk-accepted 台账项。

## 合并门前状态

- Review Packet 无 BLOCKING finding——不触发停门。
- 可回滚路径：feature 分支 `dev/260829-schedule-p2` 全部交付物为分支级 commit（未 push、未动 main，分支基点 = main tip ec4851b）；回滚 = 分支级 revert/删除即可，main 零暴露。overlay 回滚 = 三处接线引回 v1（v1 文件在世）。
- 合并相位：supervisor-verdict-gate 机械闸裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并——本 integrator 不自行判定通过、不执行 merge。
- closeout-check：integrator 只读跑 exit 0（结果见回报）；`pod-events.jsonl` 在世（advisory 满足）；`verdict.json`/`verification.json` 未产（advisory-only，非阻塞）。
