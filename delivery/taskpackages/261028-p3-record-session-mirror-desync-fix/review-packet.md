# Review Packet — 261028-p3-record-session-mirror-desync-fix

> integrator 产出（合并门前）。supervisor 机械闸复核输入；riskTier=high ⇒ supervisor 合并门预期升级 human（taskpackage notes ⑤，非缺陷）。

## Change Size

- **Files changed**：实现面 5（changeManifest 全覆盖：`firmware/main/display/kidpal_render_node.cc`、`firmware/main/application.cc`、`firmware/main/mcp_server.cc`、`firmware/main/test/record_session_lifecycle.acceptance.c`、`firmware/main/test/host_app/main/CMakeLists.txt`）+ 制品面（`delivery/taskpackages/261028-…/`：taskpackage / plan / launch-card / input-brief / dispatch-progress ×2 / docs-delta / review-packet / test-scope-final.txt / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/** / red-baseline 脚本 ×2）。
- **Net diff**：实现面 +47/-5 行（application.cc +18、kidpal_render_node.cc +9/-1、mcp_server.cc +11、CMakeLists.txt +8、验收件新增 1010 行）。
- **Scope**：固件显示/会话生命周期单点修正——M1 镜像单点派生（cache_story 体内一行）、M2 Idle 收尾沿确定性同步桥（新符号 `KidpalStorySessionEndSync()`，仅此一符）、M3 HandleStartListeningEvent 三处早退清位；server/** 零 diff。
- **Split recommendation**：不拆（单缺陷双不变量一体修复，triage ② 单包 Full 已裁）。

## Verification Evidence

- **范围圈定**：detect_changes（project `C-AI-KidPal-pod-m2`，since `75df5d32`，depth 3）正常圈定（图索引可用，未降级）；清单落盘 = `delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/test-scope-final.txt`（本行为清单路径留根锚）。种子 = 本包 acceptanceTestsFile；合成 = blast radius 映射测试（red-baseline 脚本 ×2 + server acceptance ×3〔File 级弱链路，实 diff 零，作为映射项保留在清单〕）∪ RSL2-A4 回归网家族 ∪ host runner 共享 seam 余量（firmware/main/test 全部 21 件）。**预期外命中**：firmware/scripts 工具面（p3_tools / Image_Converter / spiffs_assets 等，56 符号，File 级弱边噪声，非测试面、无测试可映射）+ server/acceptance 3 件（已入清单，acceptance 闸 scope 交集实跑覆盖 2 件）。
- **final bearing verification**：r3 / phase=final / invocationId `e8ee4421-4e85-410d-ab8e-b90de55dbe53`，`verifier-evidence/rounds/r3.json`，fact `ok:true`、`observedGateExit:0`；十闸全绿（含 firmware-build 真编译 build-pass、unit 33 passed / 4 deselected、acceptance oracle-pass firmware 27 + server 2）。verifier-evidence-check 复核 PASS（`factOk:true`）。
- **scoped 轮**：r2 / invocationId `7a1ee9f6-71d4-40d9-822f-b00ac5c53454`，fact `ok:true`（round 1 因 mutation-evidence 文书缺口红，已作废占序）。
- **builder 自验**：host 全量 runner **242 Tests / 3 Failures**（3 红全为 p1_auxbar AB-A1/A2/A4 先存红，零新增红，基线锚 237T3F @ aecded8b 口径一致）；通道 B 独立冒烟 COMPILE_EXIT=0 零 warning、5 Tests 1 Failures（唯一红 = TEST 4 canary 独立通道哨兵，GAP-4 通道形态：独立通道未链接 story_page.c，非实现失败）；设备编译 build-pass（三设备 TU 均重编）。
- **Security gate**：securityReviewRequired=false，闸绿。
- **Mutation check**：六处定向变异（/tmp 合成树经 `rsl2_sat_harness.py`）逐条 turned red，断言非空过（详见 taskpackage `mutationEvidence`）。
- **Docs sync**：none（零文档改动），理由与逐面判定见 `docs-delta.md`；docs-sync 闸 r2/r3 恒绿。

**Demo 对账行**：本包证据形态 = host runner 输出（242T/3F，零新增红；本包 5 TEST_CASE 全 PASS）+ 设备真编译 build-pass + **真机轮 RSL2-A5 待执行**（human 持板）。一致性结论：host 面证据与验收声明一致——RSL2-A1/A2/A3 的 host 断言是**源码结构锚**（三设备 TU 不进 host 构建，host 无法运行其行为），其行为正确性在 host 面只由「编译通过 + 结构锚成立 + A4 canary 回归网绿」间接担保；运行时半（收尾沿时序、ctx 被拒清位、show_home 解锁）**只能真机目检**，当前未验。宿主证据不构成 RSL2-A5 的替代。

**未覆盖面申报**（对照 acceptance[] 逐条）：

| 验收项 | 证据形态 | 申报 |
|---|---|---|
| RSL2-A1（镜像跟随权威视图页态，M1） | **源码结构锚**（tree-scan：cache_story 体内镜像写入 + 恰 2 处负锚） | 结构锚已断言；**行为未验**（设备 TU 不进 host 构建）——只能真机目检（RSL2-A5 T1/T5） |
| RSL2-A2（Idle 收尾沿次序，M2） | **源码结构锚**（五符号序 + 体内 cache_story 零命中负锚） | 结构锚已断言；**行为未验**——只能真机目检（T1/T2/T3） |
| RSL2-A3（ctx 生命周期，M3） | **源码结构锚**（三早退分支切片） | 结构锚已断言；**行为未验**——只能真机目检（T1 auto-connect 被拒路径） |
| RSL2-A4（回归网 + 冻结语义 canary） | **行为 canary**（kidpal_story_session_end 冻结语义，强符号源 = display/story_page.c host runner 通道；八族登记守） | host runner 内已验（canary + 登记守 PASS）；独立通道的 GAP-4 哨兵红为通道形态、如实单列 |
| RSL2-A5（真机轮 T1-T5） | **live-manual，无自动断言**（kind=structural） | **未执行**——human 持板留痕，planner 到真机门前停等（RSL2-X3）；不得写成「行为已验」 |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记命中——台账为空） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD finding） | — | — | — | — | — |

NIT 级备忘（不阻断）：KidpalStorySessionEndSync 桥与既有显式投递（VAD enter / SessionEndApply）形成幂等冗余——冻结 seam [R5] 文字不动的前提下的有意保留（kidpal_render_node.cc 注释已申报），非债务。

## Five-Axis Review

- **类型/编译轴**：firmware-build 真编译 build-pass（r3 闸内 + builder 独立 VERIFY_FW_BUILD_REQUIRED=1 双源一致）；通道 B `-Wall -Wextra` 零 warning。三设备 TU 与 host 构建的 extern 桥声明（`KidpalStorySessionEndSync()`）与定义（mcp_server.cc）签名一致；沿用「免扩公共头」既有纪律，未新增头文件暴露面。
- **规范/一致性轴**：M1/M2/M3 落点与冻结 plan 逐条对得上——M1 恰一行镜像派生入 cache_story 体内（负锚守恰 2 处写入形态）；M2 桥调用落在 [R2] 收尾块之后、keybar_scene_reset/voice_end 之前，未插进 keybar_scene_reset 与其判据之间（CG-A1 既有锚同区互守，上包 pod-retro 教训已吸收）；M3 三处早退清位覆盖 WifiConfiguring / !protocol_ / auto_connect_in_flight_， REC_SESSION 与 control/start_recording 两个置位入口均被覆盖。
- **契约轴**：零 wire 改动、零契约面触碰；桥不调 cache_story（s_story_data_received 不被置位，AC6 冷启动空态负锚在断言内承载）；既有显式投递幂等保留，冻结 seam 语义未动。
- **安全轴**：securityReviewRequired=false；改动面无输入解析 / 无内存边界变化（memcpy 原语句不动，仅新增一行同结构体字段赋值）；无新增依赖。
- **可运维性轴**：诊断可观察性未回退——修法把「卡死后只能重启」的故障面收敛为「ctx 清位沿 + 镜像同步沿」两条确定性路径；代码注释携带 KPLOG 2026-10-27 21:34:45 根因锚，后续真机轮留痕可回对。

## 集成执行回报

本轮未触发 Merge-Chain Execution（无跨包冲突消解）；圈定 floor 注不适用（无双包对消解场景）。

## Residual Risk

- **Known limitation**：运行时行为半（RSL2-A5 T1-T5）在合并门前无证据——host 结构锚 + canary + 真编译是合并门可得的全部机器信号；真机轮由 human 持板执行并留痕（RSL2-X3），planner 到真机门前停等。若真机轮发现时序问题，回滚 = 本包 commit revert（单 commit、零数据迁移、零 server 面）。
- **decisionCoverage**：taskpackage 无 `decisionCoverage` 字段 ⇒ 无 `landsIn: dropped` / `risk-accepted` 条目可列。
- **Follow-up recommended**：p1_auxbar AB-A1/A2/A4 三先存红消解（knownRedFloor 已记另立批）；通道 B 独立链接 story_page.c 的 GAP-4 形态（如需独立通道 canary 全绿）留待后续治理批。
- **Human decision needed**：supervisor APPROVE 后合并门按 riskTier=high 升级 human（预期路径）。

## UI design-review 证据

不适用（本包无 UIUX 工作；改动为状态沿与渲染节点内部写入纪律，无视图外观变化）。

## 制品与投影记账

- blocking 三件：`taskpackage.json`（在盘）+ `docs-delta.md`（本包目录，§docs-delta）+ `review-packet.md`（本件）。
- 承重机器件：`verifier-evidence/rounds/r3.json`（final bearing fact）——closeout 判断以 validated final fact 为准；`current/verdict.json` 仅为 non-bearing compatibility projection（verify.mjs 程序 fs-write，R11），verdict/verification 不晋升包目录（changes-index R5 口径：advisory、永不晋升）。
- post-cutoff 确认：不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在盘（advisory 检查通过）。
- 9b 记账：detect_changes 已跑（本树图索引一致，未 rebase），改动符号面 = mcp_server / application / display 三个 TU 的邻域 + 验收件模块，无预期外实现面符号（scripts/server 命中为 File 级弱边，见范围圈定段）。
