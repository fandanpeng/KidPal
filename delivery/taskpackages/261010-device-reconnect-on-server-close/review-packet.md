# Review Packet —— 261010-device-reconnect-on-server-close

> integrator 制品轮产出（2026-10-11）。第一轮 = 制品轮、不 commit；第二轮（2026-10-11）= 收口轮：arch 四投影点已由 architect 逐字落笔（对照 docs-delta.md 零漂移），final verify r3 绿，冻结 diff 一次 commit 收拢，交合并门。

## Change Size

- Files changed: 5（firmware/main/application.cc +66/-、application.h +5、protocols/websocket_protocol.cc +12、websocket_protocol.h +4、test/boot_auto_connect.acceptance.c +593）
- Net diff: +679 / -1
- Scope: 单面固件——连接生命周期（A 服务端/网络侧断自动重连 + B 永不休眠常驻重建）+ host 冻结验收件 6 新齿；server 面零改动、WS wire 形状零变化
- Split recommendation: 不拆（A/B 共享 local_close_pending_ 分类缝与 TryStartAutoConnect 复用基建，拆分即制造跨包耦合）

## Verification Evidence

- Structural gates: firmware-build 真编译 PASS（r2 fact，11% app 分区余量）、contract-lint PASS、taskpackage-required / change-manifest（pre-cutoff 存量放行）PASS
- Acceptance/unit/api/ui/e2e: host 全量 153 Tests / 0 Failures（冻结件 6 新齿 RC-A1/A2/A3/B1/B2/B3 全绿 + 既有 147 零损失）；红基线 4 红恰对应当前 story 缺失行为（设计态红），先红后绿全程留痕
- Security gate: securityReviewRequired=false（r2 fact）
- Mutation check: PASS（r2 fact）——冻结轮 M7-M12 + 实现面 M7/M8/M12 复刻全 sensitive；fixer 补钉自证「无齿承载，变异不红」如实申报（见 Findings F1）
- Docs sync: 机器闸 PASS（r2/r3 fact，docsNoChangeReason 申报态）；**arch 四投影点已落笔**（architect 逐字转写 docs-delta.md 文本，integrator 抽查 diff 逐块对照零漂移，§10 头注按 advisory 注记补例外指针句）
- verifier-round fact（bearing）: `delivery/taskpackages/261010-device-reconnect-on-server-close/verifier-evidence/rounds/r2.json`（phase=scoped, round=2, invocationId=41ef47d1-e49a-44dd-abd4-467c995b0508, ok=true）；r1 红（mutation-check 记账缺陷，planner 簿记修正后 r2 绿）
- final verifier-round fact（bearing）: `delivery/taskpackages/261010-device-reconnect-on-server-close/verifier-evidence/rounds/r3.json`（phase=final, round=3, invocationId=c2047072-f05d-4e7a-8617-08d1edc81ca6, ok=true, observedGateExit=0, 2026-10-11T03:23:58Z 起 10 闸全 PASS）——final bearing verify 由 planner 跑（scope-file = 本包 test-scope-final.txt），r3 fact `ok:true` 为 closeout 判据；test-scope 清单落盘路径已入 r3 unit 闸 stdout 元数据
- test-scope 清单（final verify --scope-file 消费）: `delivery/changes/dev/261010-device-reconnect-on-server-close/test-scope-final.txt`
- 影响面核对记账（Step 9b advisory，范围判定归 test-scope 清单）: detect_changes（Pod 专属图索引，base=main，inbound depth=3）= 22 seed symbols / 17 impacted，全落 firmware/main（Application.Run/ActivationTask/HandleNetworkConnectedEvent、mcp_server Story*、各板 InitializeButtons），无跨模块越界、无预期外符号；Pod 专属图索引用常（launch 面必办项在位），无降级
- demo 对账行：真机轮证据四场景（服务端断自发恢复 / 会话收尾常驻重建 + 跨夜静置提醒可达 / 各功能性关零重建 / 复用通道会话回归）= **pending human 在场持板**，承载位 = RC-A4/B4/B5/B6（冻结件头注 DEFERRED 登记，host 零伪造证据）；host 侧已绿证据照登如上（153/0 + verify scoped r2）

## 未覆盖面申报（对照 acceptance[] + 契约条目 + plan 效果段）

- B 路径收尾 SetDeviceState(Idle) 补钉无 host 齿承载（变异不红实证）→ 去向 RC-B4 真机轮（行为兜底）
- 运行时态跃迁 / 异步时序（回调序、task 竞争）host 结构齿不可证 → 去向 verifier 逐点核对 + 真机窗
- T=5s 实时节奏 host 不钉（定值已冻结进齿，节奏本身 live）→ 去向 RC-B4 观测
- RC-B6 modem 档位观测 pending（LOW_POWER/BALANCED keepalive 长稳）→ 去向真机轮，结论冻结后回填 docs-delta
- 其余 acceptance 条目（RC-A1/A2/A3/B1/B2/B3 host 齿）与契约条目（RC-E2 零改申报）：已覆盖，零缺口面 = none

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （`minimal:` 标记 grep 全改动文件 = 零命中，无台账条目） | — | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | firmware/application.cc B 收尾点 | B 路径收尾 SetDeviceState(Idle)+SetPowerSaveLevel 补钉无 host 齿承载——fixer 自证摘除该补钉变异不红（无齿承载），齿强度缺口已 advisory 申报，planner 裁不补齿 | changed-code hunk（application.cc 三 B 收尾点 + :429 补钉）+ 齿强度 heuristic（ARQ-260911-tooth-strength-below-contract 同族面）；非契约/spec 字面违反，不引规则前缀 | RC-B4 真机轮行为兜底（已在案）；不阻塞合并门 |
| NIT | judgement-call | plan/docs-delta | plan「docs-sync 落档」节第 2 条源文本「（260910 包）」为 changeId 笔误，docs-delta 已修正为 261010 并留痕 | changed-code hunk（plan.md:59）+ 逐字精确 heuristic；同 RC-E3 建包修正笔误先例 | 无（已随 docs-delta 留痕消解） |
| NIT | judgement-call | docs/architecture.md §10 头注 | 决策 29 append 与 §10 头注「新决策一律登记 ADR.md、冻结不再新增」措辞张力 | changed-artifact hunk（docs-delta.md 投影点 1 advisory 注记）；architect 代写时裁量 | architect 落笔时补一句例外指针（建议文本见 docs-delta 投影点 1）——**已落笔**（architect 采用建议文本，第二轮 diff 抽查确认） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。无 hard-violation finding——如出现将以 `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` 字面前缀起引 Basis；本轮 none。

## Five-Axis Review

- 安全：无新增攻击面——重连复用既有鉴权建连序列（wss + token 恒原路径），零新增依赖、零新增出网路径；无 hard-violation。
- 契约：WS wire 形状零变化（hello 建连序列复用），契约面零触（RC-E2 申报成立）；会话结束关 audio channel 既有拓扑不改，常驻在线经收尾重建达成。
- 边界与依赖卫生：重连 = 复用 TryStartAutoConnect 守卫三条件 + 既有退避表 {5,10,20}s，零第二条 task、零新退避表（RC-A3 齿 + M9 变异敏感实证）；本地关 8 调用点逐点归类留痕（RC-E4 证据件 local-close-classification.md），零跨层调用、零循环依赖、零未批准新增依赖；屏显零触碰负锚（新触发点区零 SetDeviceState，M9 破红实证）。
- 可运维性：退避 3 次穷尽后静默停、不自旋；网络断关点零重建（恢复走既有 K6 网络重连事件，不刷日志）；常驻在线 = 提醒必达运维语义兑现。
- 资源与功耗：Wi-Fi 常供电 + WS keepalive 常开，续航代价已由 human「永远不休眠」裁定一并接受（plan 电池代价注记，裁不重开）；缓解自由度 = modem 档位真机调优（RC-B6）。

## Residual Risk

- Known limitation: B 收尾态无 host 齿承载（见 F1）；T=5s 实时节奏 host 不钉；真机四场景证据 pending（RC-A4/B4/B5/B6，human 在场持板硬前置）
- Follow-up recommended: arch 四投影点已落笔并随本轮 commit；RC-B6 modem 档位结论冻结后回填 docs-delta §6.4 句（闭账文本已预留 RC-B6 挂针）
- Human decision needed: riskTier=high → supervisor 裁断后仍升 human 人批合并；真机轮持板为硬前置。decisionCoverage 中 `landsIn: dropped && reason: risk-accepted` 条目 = 无（本 taskpackage 未启用 decisionCoverage 字段）

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包冻结件 boot_auto_connect.acceptance.c（已在 scope 清单首位）。
