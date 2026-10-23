# Review Packet · 261019-p3-record-dialogue-guide

- 日期：2026-10-19 · 作者：integrator（dev-loop 收口相位）
- changeId / correlationId：261019-p3-record-dialogue-guide
- rigor=full · riskTier=high · ②单包 Full（human 触点① 2026-10-19 确认）
- 预期合并门：riskTier=high → supervisor 合并门升级 human（status=awaiting-human:merge-gate，单向守卫预期行为，taskpackage notes ⑤）

## Change Size

- Files changed：17 tracked + 2 untracked（`server/gateway/front/prompts/` 两枚 prompt 制品、`verifier-evidence/`、`verify-runs.jsonl`）——全部在 changeManifest 清单内（manifest 含 `server/gateway/front/prompts/**`、`server/tests/**`、`firmware/main/test/**` glob）。
- Net diff：约 +1111 / −162 行（`git diff --stat`）。
- Scope：server（story.py 决策族 / front_app.py sweep 重写与引导·整理 LLM 回合 / display_schema.py 自推分叉 / engine-patch connection.py 认领·第三守卫·豁免重绑）+ firmware（application.cc 聆听形态与 kvr 守卫 / view_router additive / mcp_server 消费 case / story_page 子态与整理态渲染）+ 冻结验收两件（server 17 齿 + firmware 8 TEST case）+ 配套单测重写。
- Split recommendation：不拆——server 与固件是同一原子行为单元的两半（architect 评审原判，plan §拆包被否理由）。

## Verification Evidence

### Scoped verifier 轮证据（r1–r5，事实件 = `verifier-evidence/rounds/rN.json`）

| 轮 | 结果 | 红因与闭环 |
|---|---|---|
| r1 | 红（exit 1） | `acceptance-coverage`：脚手架 sh 两件（rdg_red_wrapper.sh / rdg_inner.sh）误入 acceptanceTestsFile——round1 覆盖检查裁定脚手架无断言位、移出；planner 自错自修、轮空 fixer。承载改由 redBaselineCommand + taskpackage notes ⑪。 |
| r2 | 红（exit 1） | `mutation-check`：mutation 证据按全 id 子串匹配口径缺对齐——planner 元数据自修，零实现红。 |
| r3 | 红（exit 1） | `docs-sync`：docRefs 未提及任何声明验收 id——以 `docsNoChangeReason` 申报字段闭环（零 canonical 改动据实申报），零实现红。 |
| r4 | 红（exit 1） | `security`：securityReviewRequired=true 而 securityEvidence 缺位——planner 回填三轴实现侧复核证据，零实现红。 |
| r5 | **全绿（exit 0）** | 8/8 闸 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security。invocationId = 9462e692-4362-49c3-be05-0e6a4d7ff71c。 |

r2–r4 三红均为 planner 元数据字段缺位（pod-events 2026-10-19T15:47:45 friction 条逐轮留痕），逐轮自修、**零实现红、零 fixer 轮**。

### 测试实态

- server：323 passed——冻结件 17→0 收敛（15 设计态红 + M4K1-A5 重钉 2 齿全转绿）；`server/tests` 96 passed 零新增红（261016 锚 91 → 96，增长 = builder 配套义务① 120s 语义退役重写新增）。
- firmware host 全量：217 Tests 3 Failures 0 Ignored——3 红全为既有 p1_auxbar AB-A1/A2/A4 先存红，零新增红（红基线锚 210T3F → 217T3F，总数增长归新登记件、红数不变）。
- 固件新件：host runner 在册实跑（RDG-F1..F5 全 PASS；RDG-F6 = live-manual structural 不入通过面）。

### 结构性闸（suiteGates / structural 豁免清单，dev-loop SSOT 审计消费节）

- `firmware-build`（idf.py 构建干净）：suiteGate——结构性编译门，非逐 AC 可断言行为。
- 既有套件零新增红（server/tests / firmware host 全量）：suiteGate——套件级回归门（先存红 3 条在册）。
- `change-manifest` pre-cutoff 存量放行：gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28（闸输出原文留痕 verify-runs.jsonl）。

### Security gate

PASS（r5）。三轴证据 = `securityEvidence`（taskpackage.json）：① 儿童对话内容通道面零新增出网沿（build_organizing_input 只读 accum、素材边界不扩大）；② 两枚 prompt 制品均含纯文本回合「禁止触发任何工具调用」约束行（guide-round.v1.md:18-19 / faithful-organize.v1.md:24-25，本包实地复核在位）；③ chat() 第三守卫失败路径 = 当轮素材不归档 + 固定安抚句（ORGANIZING_FAIL_SOOTHING，R1-N4 口径）、不置 client_abort。

### Mutation check

PASS（r5）。13 条 perAcceptance 证据（taskpackage.json mutationEvidence），**RDG-A1..A7 / RDG-F1..F4 全 turnedRed=true 且还原复绿**；豁免两条有据：RDG-F5（登记守 anti-vacuity——摘除登记行使全部 weak 哨兵连坐红，无「恰好只让目标断言红」的单变异面）；RDG-F6（live-manual structural 不取证，261018 SLP-A6 口径）。要点：A2 变体首试误伤 RDG-A6 后条件化恰好命中；A7 变异清单外文件（reminders.py 先例通道）立即还原、git diff 零净差；F2 变异为复钉后补证（见锚漂移第 1 项）。

### Docs sync

PASS（r5）。零新增 canonical 改动，据实申报见同目录 [docs-delta.md](docs-delta.md)。

## Builder 分段实现摘要 + 6 未核项结论

### 分段摘要（S1–S4；S5 真机轮待 human 持板）

- **S1 server 纯决策面**：story.py 新增 is_stuck_phrase（与 is_finish_phrase 同构子串扫描、词表 STUCK_PHRASES 单源常量）；StoryAccumulator 决策族换代（timed_out_sessions 退役 → stuck_due_sessions / auto_finish_due_sessions / mark_guide_round / guide_rounds / build_organizing_input）；feed 即回应（guide_rounds/guide_since 清零）；start 新会话沿记账清零。
- **S2 server 接线 + engine-patch**：front_app.py sweep 重写（签名不变、注入 now 驱动纯决策）、引导/整理 LLM 回合（_openclaw_chat_once → loopback 18789、request 级 system 承载制品指令、SSE 双形态解析、离线快失败 None）、Package B 等价信号两收敛入口注入 + 收尾同拍 active=false、三式收尾统一 start_organizing（self_push=True 放行、重复幂等）、R1-N4 失败路径固定安抚句；connection.py 认领缝 _kidpal_on_record_context + chat() depth==0 第三守卫（先于 dialogue.put 与 TTS、不置 client_abort）+ stop-truncation 豁免重绑（manual **或** 记录语境标志，非记录语境 auto 照常截断）。
- **S3 固件聆听形态 + 引导子态**：application.cc HandleStartListeningEvent 记录语境切 AutoStop/Realtime（AEC 开→Realtime，AEC 关→AutoStop 包内裁）、kvr 场景守卫（记录语境先分流，不误映 THINK 跳页）；story_page.c rec_substate 子态（引导播报中：KEY 短/长=INTERRUPT_GUIDE 12 + 子态复位 0、BOOT 任一手势=NONE 毁稿防线）+ 回显常驻渲染。
- **S4 固件整理过渡态**：view_router.h/.c VRT_CTRL_START_ORGANIZING=3 additive；mcp_server.cc 控制 case 3 消费（story_consumed 日志同型 + 重复幂等 + 首帧渲染）与 case 12 INTERRUPT_GUIDE 消费（AbortSpeaking 继续聆听零归档副作用）；story_page.c KSTORY_PAGE_ORGANIZING=6（KEY/BOOT 全手势 NONE + 回显环复用不清空）；story_page.h 枚举与 rec_substate 字段尾部 additive。

### 6 未核项结论（plan §未核项申报逐条）

1. **引擎 ASR stt 下行发出点**：S2 抑制挂点定为 chat() depth==0 第三守卫（守卫注释 + RDG-A4 冻结齿承载「先于 dialogue.put 与 TTS」）；stt 下行先于 chat 不受抑制是注释级推断、host 不可机判——运行时验证归 S2 live 齿（真 OpenClaw loopback 驱动），已列入真机操作单前置。
2. **kidpal_render_node.c 场景映射**：S4 落法以 kvr 场景守卫承载（application.cc stt-final 回调记录语境先分流），未触 kidpal_render_node.c（changeManifest 本不含它）——与 RDG-F4 冻结齿一致，零越界。
3. **story_page.c RECORDING 渲染区现状**：帧 A 修订增量未做——气泡文案「你讲吧，我听着呢」未实现（见「未覆盖面申报」第 1 条，设计稿裁决/扩单候选）；story_page.c 改动收敛为帧 B 子态 + ORGANIZING 分支 + 回显常驻，全部 additive。
4. **既有冻结验收对 120s 语义钉法**：已闭环——acceptance-author 按 120s 语义换代重钉 test_m4k1_story_cloud_link.acceptance.py（2 齿改名 + 文件头两处重述 + REVISION LOG 2026-10-19 条，正常改验收通道非 tighten-only），随 cedd4d1f commit。
5. **reminders.py 注入先例**：复用经 RDG-A7 冻结齿承载（build_speech_injection 同型 {type,text} 逐字、空 speech 降级 None 零偏移），mutation A7 turnedRed=true 证明齿敏感——先例通道零偏移实态确认。
6. **P3 KEY 入口设备侧路由**：host 半闭环——mcp_server 控制 case 2（start_recording）补 kidpal_story_session_ctx_set(1)，与 REC_SESSION 入口同态收敛（AC1 双入口同一记录语境）；按键→StartListening 沿运行时半归真机轮（S5）。

## 结构性义务两条状态

1. **display_schema 自推通路分叉（风险 1）**：已落地——validate_display 增 keyword-only `self_push=False` 形参（既有调用点零偏移），GATEWAY_DECISION_CONTROL_ACTIONS 含 start_organizing 且 FORWARDABLE 不含；relay 路径拒转语义一字不动（end_session 即便自推仍拒）；RDG-A5 冻结齿承载 + mutation A5 turnedRed。
2. **stop-truncation 豁免重绑（风险 2）**：已落地——豁免判据重绑为「manual 模式 **或** 记录语境标志」（connection.py :499-502），记录语境切非 manual 后 KEY 收尾末句转写仍不截断；**非记录语境 auto 照常截断**（261014 既有齿保绿）；RDG-A4 冻结齿承载。

## 锚漂移 4 项处置（builder 申报，integrator 逐项复核）

1. **RDG-F2 TEST3 探针缺陷** → **已闭环**。acceptance-author 最小复钉（:659 前重设 v.rec_substate = RDL_REC_SUB_GUIDE，语义零改动、非 tighten-only），REVISION LOG 2026-10-19 条在盘（record_dialogue_guide.acceptance.c 文件尾）；F2 mutation 证据复钉后补证（217T4F→还原 217T3F 唯一新增红 = F2 :668，无误伤）。本包实地核对：复钉行与 REVISION LOG 均在位。
2. **冻结件头注称 view_router.c 已在册 vs 实况未在册** → **已闭环**。builder 按 manifest 首次登记 `../../../display/view_router.c` + `../../record_dialogue_guide.acceptance.c` 两行，CMakeLists 注释留痕（「head 注…与本清单现况不符——本包首次登记, builder 交付注记」+ 必须置于 host smoke acceptance 之前的注册序守卫）。本包实地核对：登记两行与注释在位，host runner 217T 实跑含新件（RDG-F1..F5 全 PASS = 登记生效反证）。
3. **story.py stuck_due 边界头注 >= vs 可执行测试严格 >** → **已闭环（以可执行测试为准）**。冻结测试 RDG-A2 边界② NotIn 恰阈值（t0+5s, timeout_s=5）/ In 过阈值（t0+5.5s）钉严格大于；实现取严格 `>` 且 docstring 明注边界语义与测试口径（story.py stuck_due_sessions docstring「恰 timeout_s 未计入——冻结测试 RDG-A2 边界②以 NotIn 钉…」）。本包实地核对：docstring、单测 test_stuck_due_boundary_and_exclusions（恰 8s NotIn / 8.5s In）与冻结测试三者一致；冻结件头注 `>=` 措辞为已申报锚漂移、不再改冻结件（改动留待后续冻结件修订通道，非本包义务）。
4. **plan S3 帧 A 气泡文案未实现** → **不阻塞本包，申报为设计稿裁决/扩单候选项**。单源 kidpal_story_guide_bubble 被 RSL-A4 冻结齿钉死 AC1 逐字、lcd_display.cc 在 changeManifest 之外（改动即越界）、RDG 冻结齿未钉该文案——三重障碍下实现侧正确地未动。去向：本 review-packet「未覆盖面申报」承载 + 交 planner/human 裁（扩单或后续包）。另 builder 补 mcp case 2 kidpal_story_session_ctx_set(1)（AC1 双入口收敛补全）——本包实地核对在位（mcp_server.cc :1260-1266 区域），属 RDG-F4/AC1 语义内实现，非扩单。

## 未覆盖面申报（对照 acceptance + 契约条目，逐条一行）

- 帧 A 气泡文案「你讲吧，我听着呢」：无验收断言（RDG 冻结齿未钉该文案；RSL-A4 冻结齿钉死单源 AC1 逐字）——现状 = RECORDING 帧沿用既有气泡单源；去向 = 设计稿裁决/扩单候选，交 planner/human。
- live/真机五项 GAP（RDG-F6 ①–⑤：VAD 真沿与 AFE/AEC / 引导播完继续录音不离记录会话 / 引导播报不跳页观感半 / state-organizing 帧真机可见 / S-F1.4-AC6 端到端）：无自动断言（structural live-manual 口径）——现状 = 代码锚全部在位（kvr 守卫 / 聆听形态切换 / rec_substate 渲染 / ORGANIZING 消费 / RM-A3 门控）；去向 = 真机操作单 real-device-runbook.md 承载，status=awaiting-human:real-device。
- 卡壳阈值手感（约 8 秒起步的手感调优）：无验收断言（NFR-PERF-3 明文不得写死为验收值）——现状 = STUCK_GUIDE_SECONDS=8 实现期可调常量；去向 = 真机调优后立项拍数（观感直修长环），plan 种子 8 包外留根。
- 其余逐项排查：**none**——RDG-A1..A7 / RDG-F1..F5 全部有冻结断言承载，契约 §1.4 start_organizing 表行有 RDG-F1/A5/A6 三侧断言，零其它缺口。

## Live-manual 如实申报（不造 offline 假齿）

- 引导/整理 LLM 回合真实行为链（_openclaw_chat_once 真端点往返）：live 层承载，offline 仅验形状与快失败（EQ-4）。
- S2 live 齿（真 OpenClaw loopback 18789 + 假 stt 序列驱动：引导注入 / 引擎零 LLM 派发 / 整理产出 / 归档下发）：到门即停，见 real-device-runbook.md 前置节。
- S5 真机轮（S-F2.2 五 AC 全链 + RDG-F6 五项）：需 human 持板，status=awaiting-human:real-device。
- offline 通过面不含任何 live 假齿：RDG-F6 structural 无断言不计入 runner；mutation F5/F6 豁免有据（见上）。

## 可回滚路径

- 本包全部改动以单 commit 落 dev/pod-m2（不 push、不碰 main）；回滚 = 该 commit `git revert`（零跨包耦合——硬前置 261016-p3-story-body-sd-cache 已先合并，本包无后续包依赖其头）。
- 关键面可分解回滚：固件 additive 面（view_router/story_page/mcp_server 枚举与分支）revert 即回 0/1/2 三值语义；server 决策族 revert 即回 120s 语义（M4K1-A5 重钉齿会回红——须连带 revert cedd4d1f 冻结件，回滚脚本按 commit 逆序执行即可）。
- 数据面零迁移：无 schema/存储格式变更，回滚无数据恢复义务。

## Simplification Debt（`minimal:` 标记台账）

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/story_page.c:1001 | 布局常量按 host seam 400x300 定坐标 | 真实 RLCD 屏观感 | 真机轮观感不可接受（先存标记，非本包新增） |
| firmware/main/display/story_page.c:1309 | 超长文退化为底部数字指示 | 分页/滚动方案 | 真机正文超长高频出现（先存标记，非本包新增） |
| firmware/main/display/story_page.c:1434 | 内圈虚线省略（LVGL9 无便捷 setter） | border dash pattern | 设计稿定稿要求虚线（先存标记，非本包新增） |
| firmware/main/display/story_page.c:1478 | 影子单例 mcp g_story_view、不跨视图持久 | 持久化视图层 | 跨页态残留缺陷出现（先存标记，非本包新增） |

本包零新增 `minimal:` 标记（diff 实测 `grep -E "^\+.*minimal:"` 零命中）；上表 4 条均为 story_page.c 先存标记、upgrade trigger 全部 no-trigger（本包真机轮不改变其触发条件）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | server/gateway/story.py:659 | stuck_due_sessions 边界取严格 >，与冻结件头注 `>=` 措辞不一致（实现与可执行测试一致，锚漂移第 3 项） | changed-code hunk（stuck_due_sessions docstring）+ 冻结测试 RDG-A2 边界② NotIn 恰阈值断言实测核对 | 无——已申报闭环；冻结件头注措辞修订留后续冻结件修订通道 |
| SHOULD | judgement-call | server/gateway/front/front_app.py `_openclaw_chat_once` | 一次性端点调用自写 SSE 解析循环（约 30 行 delta 拼装 + 非流兜底），与 provider 面已有流式单源并存 | changed-code hunk + `yagni`——引导/整理回合当前唯一消费方，若后续出现第二调用方再抽封装；现内联可读性可接受 | no-trigger；真机轮若 LLM 回合稳定性出问题优先复查此处双形态解析 |
| FYI | judgement-call | server/deploy/engine-patch/connection.py `_kidpal_on_record_context` | 类型字面用裸串 "kidpal_record_context" 而非模块常量（文件内 KIDPAL_RECORD_CONTEXT_TYPE 已定义） | changed-code hunk（docstring 自述：冻结验收按 AST 抽取方法体在受限命名空间 exec，仅注入五名） | 无——约束有实现注记承载，属验收通道刻意的受限写法 |

无 BLOCKING / 无 hard-violation——结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行了五轴语义 review）。

## Five-Axis Review

1. **Correctness**：S-F2.2 五 AC + NFR-PERF-3 五条边界 + S-F1.4-AC6 逐条有冻结断言承载（RDG-A1..A7/F1..F5 全绿）；边界条件实核三处——卡壳计时严格大于边界（三处口径一致，见 Findings 第 1 条）、feed 重置 streak（guide_since=None 回退锚）、start 新会话沿记账清零（防上一轮污染）；120s 退役守卫单测在世（复活即红）。收声抑制轮照常计轮（q6）由 mark_guide_round(suppressed) + 冻结断言承载。
2. **Test quality**：冻结断言钉真实行为而非实现细节——server 侧时钟注入快进（NFR 五边界全 host 可闭环）、假 ws 双桩、AST 抽取方法体断言（A4 patch 认领/守卫）；固件侧 weak 哨兵 + 行为断言 + 登记守（anti-vacuity）。mutation 13 条中 11 条 turnedRed 且还原复绿、2 条豁免有据——齿敏感面经变异实测。回显常驻、重复幂等、pending-stop 10s 窗口保留等防回归锚齐。
3. **Contract/boundary safety**：零契约漂移——契约 §1.4 start_organizing 表行为上游批已 landing 的 wire，本包纯 additive 消费；等价信号 kidpal_record_context 形状不入契约（ADR-013 口径，服务端内部链路）；GATEWAY_DECISION_CONTROL_ACTIONS 与 FORWARDABLE 不相交（伪造/注入面保持拒转）；end_session 即便自推仍拒；docs/contracts/** 本包零触碰（docs-delta 申报）。
4. **Operability/security**：儿童对话内容通道零新增出网沿（OpenClaw loopback 既有 §1.2 路径②）；prompt 制品含纯文本回合约束（防引导/整理回合触发工具调用）；第三守卫失败路径 = 当轮不归档 + 固定安抚句（不发明新语义、不静默毁稿）；守卫不置 client_abort（不波及 front 合法下发）；日志均带 story_consumed / record-context 等同型可检索锚。
5. **Maintainability/simplicity**：改动局部性良好——固件全 additive（枚举尾部追加 + switch 分支，唯一改写点 = RECORDING 键位 case 子态化，即毁稿防线本体）；server 决策族与接线分层清晰；两枚 prompt 版本化制品独立于代码演进。过度设计排查：无 `delete`/`stdlib`/`native` 命中；`yagni` 一条 SHOULD（SSE 解析内联，见 Findings）。

## Residual Risk

decisionCoverage 中 `landsIn: dropped` 条目两条（「卡壳阈值数值」/「真机停顿手感」），reason 均非 `risk-accepted` 起始（分别为 NFR-PERF-3 明文不锁值与 plan 种子 8 包外留根），故无 `risk-accepted` 承载条目需列；两 dropped 条目的去向已在「未覆盖面申报」逐条留根。

- Known limitation：真机运行时行为（VAD 真沿、AEC 回灌抑制、引导播报观感、state-organizing 帧可见性、录音期提醒排队端到端）未经实证——host 不可机判面，全部钉在 real-device-runbook.md 五项观测 + S2 live 前置。
- Follow-up recommended：① 真机轮 S5 + RDG-F6 五项观测（human 持板）；② 帧 A 气泡文案设计稿裁决（扩单候选）；③ 卡壳阈值真机调优后立项拍数；④ 冻结件头注 `>=` 措辞与 stuck_due 严格大于口径的下次冻结件修订对齐。
- Human decision needed：supervisor 合并门（riskTier=high 升级 human 预期路径）+ 真机轮持板 + 帧 A 气泡文案去向。

## 里程碑判定

`delivery/milestones/milestones.jsonl` 无状态跳变——本包属 M4-star-notebook（state=in_delivery）K 线内 K2/K3 重拆版后续包，包收口不翻账（M4 整体 accepted 须 K 线全部收口 + 真机轮闭环）；未改 milestones.jsonl、未跑 delivery-status.mjs（无 projection 变更义务）。

## Step 9b 影响面记账（advisory）

detect_changes 以 git diff 为准、无法覆盖未 commit 工作树——本包 Step 9b advisory 记账降级为「change-manifest 闸对账」（r5 PASS：17 tracked + prompts 目录全在 manifest 内、manifest 外零触）+ commit 后由 planner final bearing verify 轮在已 commit 树上补跑（test-scope 圈定与 final 轮归 planner，本相位不代跑 verify.mjs）。per-Pod 图索引在册（launch 面）；无 re-index 需求申报。补跑闭环（2026-10-20，已由 planner 触发）：实现与冻结件均已 commit（cedd4d1f / ef3e5066 @ dev/pod-m2），detect_changes 在已 commit 树上正常可用（Pod 索引 C-AI-KidPal-pod-m2：changed_files 41 / seed_symbols 204 / impacted_total 129，零降级），影响面圈定清单已落盘 delivery/taskpackages/261019-p3-record-dialogue-guide/scope-final.md（server 20 件 / firmware 27 件）；原降级申报撤销。

---

## 2026-10-20 接线轮（SAO-A1 族重钉 + 三式收尾 organize 单源）· 增量收口

- 作者：integrator（增量收口相位）· 性质：本包此前多轮 closeout 制品已在位，本轮为增量刷新、不推倒重来。

### 授权链与派发事实

- 授权链：pod-events human-decision `da35cae4-461b-4049-9ecd-46539cbd5bd0`（SAO-A1 族重钉 + 接线授权，消解 fixer r3 升级 `75f58b5d`）→ agent-escalation-resolved `e2640c49-cfdc-4ca6-971d-498a60d2af8d`。本包实地核对：两事件均在 pod-events.jsonl 本轮新增条目中在位。
- acceptance-author 三次派发：① SAO-A1 族重钉（SAO-A1 双分支成功/失败 + SAO-A4 改忠实整理输出断言 + E4 忠实性 fixture「中途提问句/幻听串不进正文」+ 人工抽检申报 S5 + v1.66「说完了」guard case + M4K1 a2/a3/a5 正路径补桩）——红基线 5 设计态红；② RDG-A6 收尾词 case 补桩（断言零改）；③ RDG-A4 收尾沿 case 补桩（断言零改）。
- builder 一次派发：`server/gateway/front/front_app.py` 三式收尾单源（新增 `_organize_and_archive` :211 / `_story_finish_session` :837 / `_speak_to_legs` :195；三检测器改收尾沿纯判定；auto-finish 委托单源；成功链 = 整理输出归档 + story_text + story_list + downlink；失败链 R1-N4 = 零归档 + ORGANIZING_FAIL_SOOTHING 安抚句 + 会话释放回 chat）。本包实地核对：三符号行号与派单一致（:195/:211/:837 实测在位）。
- verifier：scoped r11（8 闸全绿，invocationId 3b7e2f1a-9c4d-4e8b-a1d5-6f2e8b9c0d4e）+ final r12（10 闸全绿、零 notExecuted，invocationId c5d9a3f7-2b6e-4a1c-8d3f-9e4b7a2c5d8f）；scope-file 复用 scope-final.md——本轮改动面（front_app.py + 两冻结件）全部在原圈定 server 面内，scope 复用合法、未缩圈未扩圈。test-scope 清单落盘路径 = `delivery/taskpackages/261019-p3-record-dialogue-guide/scope-final.md`。

### 测试实态与 demo 对账

- 全协议 pytest：325 passed / 0 failed / 33 subtests（r12 unit 闸 stdout 逐字；acceptance 闸 229 passed / 4 deselected，live 类由 marker 机械 deselect）。
- demo 对账行：本轮 demo 证据 = 全协议 pytest 325 passed 0 failed + SAO 双分支行为级实证（SAO-A1 成功分支归档正文 == 忠实整理输出、失败分支零归档 + 固定安抚句 + 会话不挂死）；真机 demo 归 R3 轮。
- 结构闸：r12 全 10 闸 PASS（taskpackage-required / change-manifest〔pre-cutoff 存量放行口径不变〕/ firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance），零 notExecuted。final fact `ok:true` 成立，closeout 承重判断以 r12 为准；`current/verdict.json` 投影已同步 final r12（non-bearing compatibility projection）。

### docs-sync 增量

本轮 canonical 文档零改动（同目录 [docs-delta.md](docs-delta.md) 2026-10-20 接线轮增量节）：重钉与接线均在实现/验收面，requirements v1.65/v1.66 与契约 §1.4 已随上游批 landing；docs-sync 闸 r11/r12 双绿复验。

### 本轮未覆盖面申报（逐条一行）

- 真 LLM 整理忠实度（忠实性 E4 fixture 只能机判形状与排除面）：offline 不可证——人工抽检申报 S5 真机轮。
- 语音收尾词路径新增 story_list 推送：行为增量、真机核（S5 真机轮观测项）。
- RDG-F6 五项 live-manual structural：无自动断言（既有口径不变，real-device-runbook.md 承载）。
- S2 live 齿：需真 OpenClaw loopback 18789 驱动（到门即停，前置清单不变）。
- 部署/烧录：front 服务重载 + engine-patch 容器重启 + 固件烧板，归主线程（不在本包范围）。
- 其余逐项排查（对照 acceptance[] + 契约 §1.4 + roadmap 效果段）：**none**——SAO-A1..A4 / M4K1 a2/a3/a5 / RDG-A4/A6 重钉与补桩后断言面全覆盖本轮改动行为，零缺口。

### 回滚与簿记

- 本轮改动随增量 commit 落 dev/pod-m2（不 push、不碰 main）；回滚 = `git revert` 该 commit（实现面 + 冻结件原子同回，SAO 重钉齿连带——与主包回滚口径一致）。
- 簿记刷新：taskpackage.json notes ⑫ + redBaselineResult 增补、verify-runs.jsonl 两行（r11/r12）、pod-events.jsonl 九条（授权链 + 三派发 + builder + verifier + 收口）、input-brief.md 重生成快照（2026-10-20T05:38:48Z）、current/ 三件投影同步、INDEX.md 本包状态 open→closed（含末尾重复行去重）。

### 里程碑与 Step 9b 增量记账（advisory）

- milestones.jsonl 无状态跳变，未改未跑 delivery-status.mjs（M4-star-notebook 仍 in_delivery，判定同主节）。
- 本轮改动面 = front_app.py + 两冻结件 + 簿记，全部落在 scope-final.md 已圈定面（server 20 件含三冻结件与 server/tests 相邻面）内；未跑增量 detect_changes——主节 Step 9b 补跑（changed_files 41 / impacted_total 129）已覆盖本包树，本轮增量在此圈定面子集内，复用合法。
