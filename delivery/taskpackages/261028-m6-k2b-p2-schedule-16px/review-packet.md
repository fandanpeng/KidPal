# Review Packet · 261028-m6-k2b-p2-schedule-16px

> integrator 收口（2026-11-02）。test-scope 清单落盘路径：`delivery/taskpackages/261028-m6-k2b-p2-schedule-16px/test-scope.md`。包级 diff ref = `bfb8551e`（pod-started 在案）。

## Change Size
- Files changed: 13（代码 6：schedule_page.c/.h、三冻结验收件、host_app/main/CMakeLists.txt；治理制品 7）
- Net diff: +2243 / −217（其中 schedule_grid_16px.acceptance.c 新增 1391 行为最大单件）
- Scope: 全部落入 changeManifest（5 件）+ acceptance-wiring-patch.md 授权（CMakeLists 六处增量）+ 治理台账；唯一出界件为 openclaw 包 verify-runs.jsonl +1 行（verifier 首轮 changeId 解析窗口期产物，治理 ledger append、零代码改动，归属无害——详见 test-scope.md）
- Split recommendation: 不拆——单一显示组件改造 + 其验收镜像，同包原子交付

## Verification Evidence
- Structural gates: verify 8 闸全绿（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security），最新一轮 2026-11-02T04:25:55Z，`verdict.json`（script fs-write 投影）+ verify-runs.jsonl 7 轮在案
- Acceptance: host 通道① = 258 Tests / 3 Failures / 0 Ignored（修订轮后终态；3 红全为存量 p1_auxbar AB-A1/A2/A4，launch 基线锚在册、与本包无关）；SG16-A1..A6 全绿、既有 247 条零偏移、零段错误。冻结件缺陷 6 处（builder 对抗轮咬出）经 acceptance-author 收紧型修订轮修掉（REVISION LOG 留痕于冻结件文件尾），终态复测 258/7→258/3
- Device build: xiaozhi.bin 0x324d10 bytes（app 分区余 9%），firmware-build 闸真编译绿
- Server: 零 diff；395 passed / 9 deselected / 33 subtests passed 基线维持（启动卡申报，本包未触 server 面）
- Mutation check: 9 条变异全 turnedRed（M1 pitch 16 / M2 带回退 / M3 容量 {4,4,4} / M4 cap 误缩 48 / M5 下划线摘除 / M6 下划线 2→3px / M7 画回 ※ / M8 星标摘除 / M9 font 消费点换 title）。M3/M6 首测 MASKED（冻结件缺陷齿先中断），冻结件修订后复测转正——证据已回填 mutationEvidence，18ff3a22 落盘
- Docs sync: docsSyncRequired=false，零改动声明见 `docs-delta.md`；spec/契约面零漂移检出
- crossEnd: server/gateway/schedule.py（week_grid_payload）零 diff——云端全量下发不变（F-8）实证

## 事故与修复（如实记录）
- **dac2aa32 提交事故**：主线程在 builder 变异轮进行中提交，把 M3 变异值 `ksg_slot_caps {4,4,4}` 误带入 HEAD。**18ff3a22 修复**：还原 {4,4,3}（schedule_page.c 1 行）+ M3/M6 修订后复测转红证据回填（taskpackage.json mutationEvidence）。已核 HEAD 现状：`git diff bfb8551e..HEAD -- firmware/main/display/schedule_page.c` 终态无变异残留，host 终态 258/3 与干净基线吻合。
- **冻结件 6 处逻辑缺陷**（builder 通道③暴露，非实现缺陷）：A1 墨高窗越证据域 / 旧位全行白负锚与竖线家具互斥 / TEST 3 三处截断帧对误比 / tooth6 x 带错位 / helper 极值函数命中带底竖线。builder 以不入库诊断副本（_sgx_oracle_tmp.c，跑完即删，git status 干净）证意图可满足后，acceptance-author 收紧型修订轮修复，零负锚削弱（越带零墨 / CAP=84 / 星标三齿 / restore 镜像原样保全）。

## 集成执行回报
- 本包无跨包冲突消解，merge-chain 执行节不触发。
- 收口时点 pod-closed 事件：由 `pod-events-append.mjs` 追加（actor/source = integrator），见 pod-events.jsonl。

## Simplification Debt
| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （diff 内零 `minimal:` 标记） | — | — | no-trigger |

## Findings
| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 治理流程 | 主线程在 builder 变异轮进行中提交 dac2aa32，变异值短暂入库（18ff3a22 已修复还原） | changed-code hunk：schedule_page.c `ksg_slot_caps` 于 dac2aa32 带 {4,4,4}、18ff3a22 还原 {4,4,3}；启发式 tag：变异态不得跨 commit 边界 | 新铁律候选入 retro：主线程 commit 前必须核 builder 是否挂着临时变异 |
| NIT | judgement-call | 台账卫生 | openclaw 包 verify-runs.jsonl 被本包 verifier 首轮窗口期 append 1 行（changeId 仍解析为旧包） | changed-code hunk：delivery/taskpackages/261031-openclaw-cron-trigger-gating/verify-runs.jsonl 首行后新增行，ts=2026-11-02T04:01:09Z；tag：跨包台账误挂 | no-op（append-only 台账、事实无误、可审计）；无需行动 |

## Five-Axis Review
- **正确性/契约**：flags bit0 wire 语义不变（§1.3②），只改板端标记形态；※ 字形探测与 ksg_marker_temp_proc 程序化兜底退役（M7 变异实证退役可判别）；★ bit1 不变（M8 双档互证）。CAP=84 与云端全量下发不动（F-8），截断只在渲染层（M4 变异实证解析 cap 判别力）。
- **测试判别力**：9/9 变异 turnedRed；同名双课夹具消除墨高差使 pitch 锚精确 == 19；旧位负锚改「除竖线家具列外全白」防假绿；越带零墨 / CAP=84 / 星标三齿 / restore 镜像负断言零削弱。
- **安全**：securityReviewRequired=false；security 闸绿；本包无输入解析面变化（解析逻辑不动，仅渲染层与标记形态）。
- **可运维性**：device build 0x324d10（app 分区余 9%，无新增分区压力）；降级路径继承 P0 统一句柄与 14px 降级不变式（crossPackageBehaviors 第 1 条，不在包内另造资产加载）。
- **简洁性**：schedule_page.c 净 +164 行为 pitch/带/下划线直接实现 + ※ 兜底摘除；无抽象层新增、无跨层调用、无新依赖；结构半 FAS 键（sched_c_font_seam 直连 [K4] 句柄）由 CMakeLists 六处增量承载，逐字落线。

## Residual Risk
- Known limitation: 16px 态 CJK 实测墨高 14（资产 line_height=23/base_line=5、「课」box 15x14、「口」box 12x13）——冻结时点预判窗 [15..18] 下界越证据域，修订轮以实测重基线 [13..16]；P1 同资产同值，两包一致。
- Known limitation: 真机临时课下划线观测受上游写入侧制约——ARQ-261029（human 已裁上游补写入侧）未派单前，真机轮若当周无临时课样本，下划线观测按「观测受限」记录（P1 统一轮同先例）。
- Follow-up recommended: 真机轮 P2S-E5（pitch 19 紧凑观感 / 4-4-3 容量 / 静默截断 / 下划线★ / 长课截断）——收口后由主线程/human 执行，结果供 P3 终端收口复证。
- Human decision needed: 远程发布（主线程 `release-pr.mjs --publish` 后 human 合 PR）；ARQ-261029 上游写入侧派单方式待 human 确认。
- decisionCoverage 扫描：`landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目 = **0 条**（8 条 decision 全部落 exitCriteria 或关联包）。

## 遗留项（不阻断本包收口）
1. 真机轮 P2S-E5——design review + 烧板看屏，收口后主线程/human 执行。
2. ARQ-261029 上游 ※ 写入侧——待 human 确认派单方式（/triage-change）。
3. 远程发布——待 human。
4. 存量 p1_auxbar 三红（AB-A1/A2/A4）跨包在册，非本包义务。
