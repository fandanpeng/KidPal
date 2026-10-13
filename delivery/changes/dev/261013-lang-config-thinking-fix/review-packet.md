# Review Packet —— 261013-lang-config-thinking-fix

> integrator 制品轮产出（2026-10-14）。状态：verifier scoped round 3 全绿（八闸 PASS、fact ok=true）+ docs-sync 收口（TECH-261013 条目 closed 归档 + 连带残留另立 TECH-261014）+ closeout 制品齐 + 范围圈定终验（detect_changes 正常圈定，test-scope-final.txt 落盘），feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。rigor=lite 单包、sourceType=regression、riskTier=medium。

## 来源链

- 2026-10-13 主线程 dev-loop 承重轮（261013-qq-schedule-write-closure，server-only 零固件改动包）踩中：verify.mjs firmware-build 闸真编译红 `lcd_display.cc:2180 'THINKING' is not a member of 'Lang::Strings'` → TECH-261013-lang-config-thinking-key-regen-landmine（docs/crq/tech/，根因权威：Lang::Strings::THINKING 键只存在于 untracked 手补生成头 `lang_config.h:60`，两 locale 源缺键、生成器不可产出，干净构建必炸）→ architect `/triage-change` plan.md（2026-10-13，1g 前置技术预评本体，human 触点①确认）→ 本包 Lite 单包。

## Change Size

- 实现面 = changeManifest 六文件逐一对账（git diff --stat 实测）：两 locale json（zh-CN/en-US 各 +2−1 = THINKING 键落 strings 节）+ `firmware/main/CMakeLists.txt`（+12−5：LANG_HEADER 迁 binary dir + INCLUDE_DIRS 追加 + `--assets-dir` 命令行传参 + DEPENDS 补 en-US + 排序接线两行 + 撤 `add_custom_target(lang_header ALL)`）+ `firmware/scripts/gen_lang.py`（+14−10：`--assets-dir` 可选参、旧反推逻辑保留）+ `firmware/.gitignore`（−1：忽略行撤）+ 验收件 `lang-config-gen.acceptance.md`（新增，LG-A1..A9 + §6 机检断言）。固件面净 +35−18（不含 sdkconfig）。
- Scope: 纯固件编译层/生成链修复（locale 数据键 + CMake 生成链正形），零 wire 形状变更、零云端面、零契约面、零 UI 面（UIUX Level 0）。
- Split recommendation: 不拆——单根因单修法单包原子变（Lite 分包即回炉）。

## Verification Evidence（verifier 终态）

- **终态 fact = r3 全绿**：`delivery/taskpackages/261013-lang-config-thinking-fix/verifier-evidence/rounds/r3.json`（phase=scoped、round=3、invocationId=860711b3-d466-49d5-ac85-b78e8e0ee021、ok=true、observedGateExit=0、gatesRun=8）——八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译绿：xiaozhi.bin 0x312670 bytes、app 分区余量 0x5d990 = 11%）/ contract-lint / acceptance-coverage（LG-A1..A9 全落）/ mutation-check（9 条 perAcceptance 全在案、零缺位）/ docs-sync / security。compatibility 投影 `delivery/taskpackages/current/verdict.json`（程序 fs-write、non-bearing）与 r3 fact 同源同值。
- verifier 轨迹：r1（acceptance-coverage 红——.acceptance.md 未命中机检断言正则）→ 冻结件 §6 additive 修订补齐 → r2 反馈 mutation 判别缺位 → builder 补齐变异取证（M-A2/A3/A5/A7 四条真实实跑）→ **r3 全绿**。integrator 未另跑 final 相位轮（派单口径以 r3 为证据态收口；verify fact 程序 fs-write、只消费不重跑，r3 顶层 phase=scoped 如实留痕，supervisor 裁断时可见）。
- **范围圈定（Step 4，两步走之①）**：detect_changes 实跑（Pod 专属图索引 `C-AI-KidPal-pod-m2`，base=main，merge_base=f6cb61f，inbound depth=2，含工作树未 commit diff）——10 seed / 2 impacted，impacted 全落 firmware 两个已改模块（firmware/scripts + firmware/main），零跨模块越界、零额外测试面命中；∪ impactedTestsSeed（验收载体 + application.cc / lcd_display.cc 两零改动回归观察面）→ **test-scope-final.txt 落盘 `delivery/taskpackages/261013-lang-config-thinking-fix/test-scope-final.txt`**（test-scope 清单落盘路径一行记入 final verifier fact / review-packet results 元数据锚）。三分法：per-Pod 专属图索引可用 → 正常圈定，无降级；本包零 server 面改动 → server 交集诚实绿跳过。
- **final bearing verification（两步走之②）**：本包为编译层包、无 .acceptance.c runner 承载面，runner 类终验由 r3 firmware-build 真编译绿 + LG-A6 对账 + LG-A7/A8/A9 生成三面承载（见未覆盖面申报的 deferred 申报）；verify.mjs final 相位轮未由 planner 派发，test-scope-final.txt 内留占位行待回填。
- 影响面核对记账（Step 9b advisory，detect_changes 同上）：改动只影响预期符号面（gen_lang 生成链模块 + main 组件构建脚本），两 THINKING 引用站点零改动、编译解析切至 build 目录生成头（LG-A2 断言 62 include 站点零改动承载）。无降级、无虚报。

## 验收线 LG-A1..A9 判定汇总

| 条目 | plan 任务 | 判定 | 承载证据 |
|---|---|---|---|
| LG-A1 | T1 两 locale 正源补键 | **PASS** | zh-CN `"思考中..."`（与手补头 :60 逐字一致）+ en-US `"Thinking..."`；§6.1 assert PASS（§6.10 实跑记录）+ 变异 B1 同源红（摘键即对账红 + 编译红） |
| LG-A2 | T2-① 生成头迁 build 目录 | **PASS** | `LANG_HEADER` 指 `${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h` + INCLUDE_DIRS 追加；§6.2 assert `LG-A2 PASS sites=62 (dedup=1)`（include 字面量唯一形态、62 站点零改动）+ 变异 M-A2 红（回退源树 → 源树回潮 10697 bytes + assert 红） |
| LG-A3 | T2-② `--assets-dir` 可选参 | **PASS** | §6.3 assert PASS（--help 含参数且非 required + CMake 传参一行）；骨架显式传参生成通 + 不传参旧反推逻辑双跑 exit 0（§5 修订后命令实测）+ 变异 M-A3 红（argparse exit 2） |
| LG-A4 | T2-③ DEPENDS 补全 + 排序接线 | **PASS** | §6.4 assert PASS（DEPENDS 三项 + ALL target 零命中 + 接线两行在位）+ 变异 B2 同源红（摘 en-US → touch 不再触发再生成）；接线摘除变异 deferred（时序竞态无稳定红判据，见未覆盖面申报） |
| LG-A5 | T2-④ 收尾 | **PASS** | §6.5 assert PASS（源树生成头不存在 + ignore 行计数 0）+ 变异 M-A5 红（ignore 回填 → assert 红）、M-A2 兼证源树回潮判据 |
| LG-A6 | T3 全量对账 | **PASS** | `LG-A6 PASS refs=49 producible=71 UNDEFINED: []`（§6.6，redBaselineCommand 同式）——红基线 `['THINKING']` 归零 |
| LG-A7 | T4 host 生成复现 | **PASS** | §6.7 复放 PASS（骨架 `--assets-dir` 产物含 `constexpr const char* THINKING = "思考中...";`）+ 变异 M-A7 红（host 摘除态 exit 2） |
| LG-A8 | T4-① 增量复现 | **PASS** | B2 还原复验绿：touch en-US → mtime 1791904889 > touch、REGENERATED、THINKING 命中 1（mutation-evidence.md B2 节 verbatim） |
| LG-A9 | T4-② 全量首建绿 | **PASS** | builder 主回执两次独立全量首建（rm -rf build → exit 0 + 生成头非空 THINKING 命中 1 + 源树无回潮）；缺键态红由 B1 同源承载（首建与增量同一生成命令） |

mutationRequired=true 满足：9 条 perAcceptance 全部 `turnedRed=true && sensitive=true`——6 条真实变异实跑（B1/B2/M-A2/M-A3/M-A5/M-A7）+ 3 条同源承载（LG-A1←B1 / LG-A4←B2 / LG-A9←B1），全部证据 verbatim 落 `mutation-evidence.md`（builder 无 delivery/ 写权、planner 代存），MUTATION 残留 grep 零命中、临时编辑精确还原（终态 diff 与变异前基线逐字一致）。

## 红基线 → 终态

- 红基线（acceptance-author 冻结实跑 2026-10-13，HEAD 0ee420c，命令 = taskpackage `redBaselineCommand` 逐字）：**refs 49 / producible 70，UNDEFINED = ['THINKING']**——与 triage 期及 planner intake 基线逐字一致（唯一一颗）；引用站点 application.cc:747 + lcd_display.cc:2180；现编译存活仅依赖 untracked 手补头。红基线性质 = 对账红 + host 生成红 + 形态缺陷，非编译红（手补头存活）。
- 终态（builder 落地后，§6.10 实跑 + r3 闸）：**对账差集 ['THINKING'] → []，refs 49 / producible 71**（+1 = 新补 THINKING 键）；生成三面（host 骨架 / 增量 touch 再生成 / 删 build 全量首建）全绿；形态缺陷归零（untracked 手补头删除 + ignore 行撤除 + 源树零回潮）；firmware-build 真编译绿（xiaozhi.bin 0x312670 bytes）。

## 未覆盖面申报（对照 acceptance[] + 契约条目引用 + plan T1–T4，逐条列「无验收断言/未跑的设计性质 + 现状 + 去向」）

- **契约条目消费面**：本包 contractRefs = `docs/contracts/interface-contract.md` bare 整文件引用，无被消费的契约条目（locale JSON 与 lang_config.h 板端私有、无 wire/契约承载，冻结件头「契约条目 ↔ 齿的强度申报」在案）——契约级缺口 = **none**。
- **LG-A1..A9 九条**：全部有验收断言且转绿（见上表），缺口 = **none**。
- **已知 deferred 项**（如实申报，不折算为缺口豁免）：
  1. **真机轮 deferred 归 Phase-1 语音轮**（plan T4 已裁 + 冻结件头豁免声明）：本修复为编译层修复，行为面与手补头存活时代等价（`8a3fc07` 接线已在）；思考页真机表现归 Phase-1 语音轮 workstream（requirements §2.2 已标 Phase 1 修复项）。现状 = 烧录面验证仅到 host 侧全量编译绿；去向 = Phase-1 语音轮真机轮顺带目测。
  2. **LG-A4 接线摘除变异 deferred**（撤 `target_sources`/`set_source_files_properties`）：编译/生成时序竞态非确定性、无稳定机判红，诚实申报不单独实跑；排序正确性由行为侧兜底 = LG-A8 增量构建日志实证生成完成先于对象编译 + LG-A9 全量首建绿。现状 = 结构断言（§6.4）绿 + 行为证据双面在场；去向 = 无追加动作（若未来 IDF 升级动构建排序语义，随包重估）。
  3. **TECH 连带第 6 条 sdkconfig 装饰性空节**：范围外残留（Kconfig 构建期回写，与生成链缺陷同源不同体），不进本包 commit、范围帽排除；去向 = **已另开轻量 CRQ `TECH-261014-sdkconfig-decorative-empty-section`**（docs/crq/tech/，NIT/deferred）。
  4. **LG-A6 对账的 10 个可产出未引用 strings 键**（ACTIVATION / BATTERY_CHARGING 等，冻结件 §2 LG-A6 已照会）：无害余量、非缺陷，本包范围外；去向 = 无（保留自然余量）。
- 其余设计性质未跑面：**none**（本包无 .acceptance.c runner 面，host 套件不适用；无性能/功耗/安全断言义务——securityReviewRequired=false 且 security 闸 PASS）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | verifier 终态相位 | r3 顶层 `phase=scoped`（非 final）——派单口径以 r3 为证据态收口、integrator 未另跑 final 相位轮；test-scope-final.txt 已留占位行待 planner 补跑回填 | r3.json hunk（phase 字段）+ 留痕诚实性 heuristic | planner 合并门前可补跑 final 相位轮（scope-file = test-scope-final.txt）回填占位；不阻塞（r3 ok=true 八闸全绿为 bearing fact） |
| NIT | judgement-call | 工作树卫生 | `firmware/sdkconfig` 工作树 M 态（构建期回写「Image Effects Configuration」5 行装饰性空节）不进本包 commit、留工作树——每构建必脏的既有卫生债 | changed-artifact hunk（git diff firmware/sdkconfig）+ 范围帽 heuristic（taskpackage 连带第 6 条划出） | 已立 TECH-261014-sdkconfig-decorative-empty-section 跟踪；本包收口人工排除即可 |

结构闸 PASS/FAIL 不复述为 finding（r3 八闸全绿仍完整执行五轴语义 review，见下）。本轮 findings 表 = 上方两条 NIT，无任何升级级（hard-violation 等级）finding；亦无触及治理 spec 面（契约/需求/架构）的 Basis 引用。

## Five-Axis Review

- 安全：零新增攻击面——locale 数据键补齐 + 生成链 CMake/脚本参数面正形，零 wire / 零持久化语义变更 / 零新增依赖 / 零新增出网路径；`--assets-dir` 为构建期显式路径参数、无注入面（值由 CMake 变量提供）。
- 契约：零触（contract-lint PASS）；locale 键与生成头均为板端私有资产，display 响应块/笔顺/课表/shim API 四契约条目零消费、零变更。
- 边界与依赖卫生：零新增依赖；单机制排序接线（组件目标 GENERATED/HEADER_FILE_ONLY）替代冗余 ALL custom target，消双挂；include 字面量全树唯一形态 62 站点零改动——include 站点层零迁移成本（LG-A2 断言承载）；untracked 手补生成物形态正式消灭（防「跨 worktree 拷贝传播」复发，即本雷根因形态）。
- 可运维性：回滚 = 本 commit revert（单包原子，零跨面耦合）；新 worktree/clone 首建自洽 = 本雷触发条件直验绿（LG-A9）；`--assets-dir` 向后兼容保留旧调用方行为（骨架双跑 exit 0 实测）；构建产物归 build 目录后源树零生成物噪音。
- 资源与功耗：xiaozhi.bin 0x312670 bytes、app 分区余量 11%（r3 firmware-build 实测）；locale 补键仅增一常量字符串，零运行时开销变化。

## Simplification Debt

`minimal:` 标记收成：本包改动文件（两 locale json / CMakeLists.txt / gen_lang.py / .gitignore）grep `minimal:` 零命中——**零新增债务**；无既有标记在本包改动面内。

## Residual Risk

- Known limitation: 真机轮 deferred（见未覆盖面申报第 1 条）——编译层修复的行为等价假设（build 目录生成头解析 = 源树生成头解析）由 LG-A8/LG-A9 全量编译绿实测兜住，非仅凭推理；真机目测归 Phase-1 语音轮。
- Follow-up recommended: TECH-261014（sdkconfig 装饰性空节，NIT）随下个固件构建治理批顺带消。
- Human decision needed: 无新增（本包无真机持板项、无 all-mock 重算风险——断言全为对账/产物/构建行为级，非 mock 断言）。taskpackage 未启用 decisionCoverage 字段——`landsIn: dropped && reason: risk-accepted` 条目 = 零，Residual Risk 节按模板申报零条目。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包冻结验收件 `firmware/main/test/acceptance/lang-config-gen.acceptance.md`（在 test-scope-final.txt 首位）。detect_changes 记账见 Verification Evidence（2 impacted 零越界，无降级）。全量回归 = 人工触发位，本轮为 Lite 包按派单口径收口。

## 制品清单

- `delivery/changes/dev/261013-lang-config-thinking-fix/review-packet.md`（本件）+ `docs-delta.md`（blocking 两件）。
- `delivery/taskpackages/261013-lang-config-thinking-fix/test-scope-final.txt`（范围圈定清单，Step 4①落盘）。
- `verdict.json` / `verification.json` 投影：advisory-only 未另产——compatibility 投影已在 `delivery/taskpackages/current/verdict.json`（程序 fs-write），closeout 判断以 r3 bearing fact 为准（closeout SSOT 表口径，缺失不阻塞）。
- TECH-261013 条目收口（closed 归档 `_archive/2026/`）+ TECH-261014 新立 + 两 INDEX 重渲染（见 docs-delta.md）。
- post-cutoff 纪律：不产 session-digest / loop-health / loop-metrics / pod-journal 晋升；`pod-events.jsonl` 在场（8 events，advisory non-blocking）；`pod-retro.md` 归 planner 收尾相位。

> 修订注记（2026-10-14）：Findings 节元描述句改写——pre-merge-escalation-gate 字面集误报消解（门语义 = hard-violation finding 必须有 CONTRACT-DECISION-QUEUE 对账行或 human defer 锚；本轮零 hard-violation，误报源于元描述句自身含治理文档字面路径，被门正则同句命中）。授权：planner 裁定 + 闸反馈通道。仅改该句并补本注记，其余内容零触碰。
