# Review Packet · 261024-vq-overlay-close-end-session

> 产出方: integrator（2026-10-24，closeout 相位）。模板依据 `.claude/references/code-review-gate.md` §Review Packet。本包: rigor=full · riskTier=high · sourceType=regression · 纯实现面（firmware/**）。

## 1. Change Size

工作树实测（git diff HEAD，含制品投影）：10 文件，+497 / −182。功能面 = 3 实现文件（application.h +14、application.cc +69、waveshare-s3-rlcd-4.2.cc ±24）+ 2 验收文件（voice_round_ui.acceptance.c +161、boot_auto_connect.acceptance.c +81）；其余为包内制品（taskpackage.json 冻结回填 + mutationEvidence、current/ 三投影、sdkconfig 注释再生漂移 5 行）。**档位 = S-M**：功能语义集中在一个新事件位入口及其板级接线，单处理器同步收尾序列，无跨层 / 跨端扩散。

## 2. Verification Evidence

**verifier 三轮轨迹（scoped 相位，evidence = `delivery/taskpackages/261024-vq-overlay-close-end-session/verifier-evidence/rounds/`）**：

| 轮 | invocationId | 结果 | 要点 |
|---|---|---|---|
| r1 | 876d489f-1ae5-40ed-b6ab-53eb1735786a | ok=false | 红：acceptance-coverage（TEST 8/TEST 4 头注缺 OCE-A2/OCE-A3 coverage 登记标记；host 套件同时实测 7F = 先存红 3 + 设计态红 4，mutation 面如预期红） |
| r2 | 21480d8f-ee24-49eb-afab-ab8dd7175ea9 | ok=false | 红：mutation-check（mutationEvidence perAcceptance 面未齐——acceptance-author 补记三 acceptanceId 的 mutant 面/结果/evidenceRefs 后复跑） |
| r3 | c6ad3806-b657-4da9-883e-47357087ca6c | **ok=true** | 8 闸全绿：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（**degraded**，见 Finding F-1）/ contract-lint（1 条既有 broken-link warning，与本包无关）/ acceptance-coverage / mutation-check / docs-sync / security |

红基线（acceptance-author 冻结相位实跑，2026-10-24）：build 零 warning 零 error；elf = 237 Tests 7 Failures——4 条新增红全为设计态红（OCE-A1 TEST12 tooth1 / KVR-A4 tooth2 / RC-A2 tooth6 计数 4≠5 / RC-B2 第四项切取失败），先存红 3 条（p1_auxbar AB-A1/A2/A4 @6e8425ff）逐条复现零漂移。builder 落地后回落 **237 Tests 3 Failures（恰先存红，零新增红）**——OCE-A3 回归网闭合，OCE-X1/X2 达成。mutation 三条 acceptanceId 均「failed as expected → 落地转绿」，evidenceRefs = r1.json + r2.json（taskpackage.json `mutationEvidence` 已程序留痕）。

**final bearing verification**：归 planner 终审相位执行（verify.mjs `--phase=final --scope-file <本文件 §7 清单落盘路径>`）；scope-file 缺失红闸 fail-closed，本包已备齐。

**范围圈定记账（Step 9b advisory）**：detect_changes 实跑成功（per-Pod 图索引可用，project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，merge_base c4276adc）——seed_symbols=96，**impacted_total=5**（CustomBoard / InitializeButtons / app_main / boot_auto_connect acceptance / voice_round_ui acceptance），impacted_modules 全落 `firmware/main`，**零出界符号**；结果已同步记入本包 `verifier-evidence` 消费链与 scope-file.md §1。

## 3. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| BLOCKING | judgement-call | firmware-build / OCE-X3 | r3 firmware-build 闸 **degraded：buildAttempted=false**（工具链两路探测均不可用，VERIFY_FW_BUILD_REQUIRED unset，降级放行非假绿）——exitCriteria OCE-X3「设备编译闸绿（VERIFY_FW_BUILD_REQUIRED=1 真编译通过）」当前**未闭环** | r3.json firmware-build 结果字段（`notExecuted: true` + degraded reason）对照 taskpackage.json exitCriteria OCE-X3 字面 | planner final 相位在具备工具链环境补跑真编（VERIFY_FW_BUILD_REQUIRED=1）转绿；或如实降级申报 human 裁定接受。未处置前不进合并门 |
| NIT | judgement-call | 构建再生面 | `firmware/sdkconfig` 出现 changeManifest 外 5 行 diff——仅「Image Effects Configuration」空注释块删除，ESP-IDF 构建再生注释层漂移，零功能语义 | 收口前 git diff 实测（非图内符号，detect_changes 不映射） | 已随本包 commit 收口留痕（docs-delta §2 / scope-file §4 双登记）；planner 备读，无需回改 |

结构闸（verify.mjs r3）PASS/FAIL 不复述为 finding；gate 全绿仍执行 §4 五轴语义 review。无 hard-violation finding——本包无契约 / spec 触面，Basis 前缀核验条款不适用。

## 4. Author 五轴语义 review

- **安全**：无新增攻击面。收尾序列先 abort 后关通道（同 TCP 有序，沿用 SendKidpalStoryDiscard 先例）；`auto_connect_in_flight_` 守卫避免与后台 AutoConnect 抢 `websocket_`；不发 SendStopListening（不触录音归档语义）；无密钥 / 日志敏感信息（local_close_settle 日志仅时间戳与状态）。
- **契约面**：零触 `docs/contracts/**`。abort / 关通道均为既有 WS 消息语义，无 wire 变更；crossEnd=exempt 申报成立（server/** 零 diff）。
- **边界与权限**：板级只保留「前台是否 VOICE 覆盖层」路由策略，三态门下沉 Application 单源裁决——职责边界收干净；KEY 短按打断语义（HandleToggleChatEvent Speaking 分支）零改动，需求 §2.3 冻结语义未触碰；plan ⑥ 负锚七条逐条守住（未抽收尾 helper、未加 aborted_ 复位、分支在 SCHEDULE 之前、宏/掩码/分支三处齐改）。
- **可运维性**：local_close_settle `src=overlay_close` 与 toggle_chat / discard_record / wake_invoke 诊断口径并列，可区分关层收尾路径；聆听态 src 由 toggle_chat 变 overlay_close 属诊断口径变更（plan ③ 已申报），非行为变更；时序自证注释（tts stop 晚到竞态自愈）在处理器定义处写死，可追溯。
- **一致性（docs↔实现）**：板级原「复用 ToggleChatState」注释已重写（改后即谎的注释消除）；验收重钉留痕（tighten-only 纯 additive 申报、义务红形态偏移判读、M12 判读更新注记）齐备；canonical spec / contracts 零漂移（docs-delta §1 申报，docs-sync 闸绿佐证）。

## 5. Simplification Debt（`minimal:` 台账）

本包 5 个功能 diff 文件 grep `minimal:`：**零新增标记**（waveshare 板文件 :449 既有 1 条属先前批次 RTC 面，不在本包 diff hunk 内，不入台账）。本包无可收简化债务。

## 6. Residual Risk

- **OCE-A4 真机目检 pending（human）**：会话收尾时序（断连 / 静音 / 不收音 / 页面回原页 / ≥4 分钟无 ASR）host 行为断言不可闭环（Application / Protocol / AudioService 不进 host 构建——plan ④ 申报），行为 oracle 归真机目检六项；Pod 停等 `awaiting-human:real-device`，不自行烧录。这是本包最大的残余不确定面。
- **OCE-X3 设备编译降级**：见 Finding F-1（BLOCKING，planner 处置）。
- **`aborted_` 不复位**：plan ⑦ 已论证（全仓只有写没有读，下一次 tts start 帧复位）——接受，无动作。
- **覆盖态申报（断言覆盖轴差集）**：本包验收 = OCE-A1/A2 结构断言（host 源锚代理档）+ OCE-A3 回归网 + OCE-A4 真机目检六项。无 host 行为断言的设计面 = ① 收尾动作时序（ResetDecoder→Abort→CloseChannel→Idle 的实际运行序，host 只钉源文本位次）；② 断连后自动重连不触发（本地关分类的运行时消费）；③ ≥4 分钟无收音。三者现状 = 均无 host 行为断言（目标符号不进 host 构建），去向 = OCE-A4 真机目检 ①/②/⑥ 项覆盖。对照 plan ④ 申报一致，无新增未申报面。
- **遗留（plan ⑨ out-of-scope，另案）**：覆盖层 BOOT 双击路由返回 NONE，与 §2.3「双击=同短按」漂移——与本包无冲突（将来修好后自然汇入同一 EndVoiceSession 入口），另案立包。
- **knownRedFloor**：先存红 3 条（p1_auxbar AB-A1/A2/A4 @6e8425ff）与本包无关，零漂移。
- **decisionCoverage 残余裁定**：本 taskpackage.json 无 `decisionCoverage` 字段 → 无 `landsIn: dropped` / `risk-accepted` 条目可列（空集，如实申报）。

## 7. 对账与适用性注记

- demo 对账行：**不适用**——本包无 workstream 归属、非效果段包，如实注明。
- UI design-review 证据（Step 6b）：**不适用**——input-brief ①c `uiux block = none`，本包无 UI 设计面（板级渲染零 diff，仅会话状态语义）。
- 集成执行回报（merge-chain §消解完成后清单）：**未触发**——merge-driver 冲突消解未发生，无对端冻结件复跑义务。
- test-scope 清单落盘路径（final verifier fact results 元数据必含行）：`delivery/taskpackages/261024-vq-overlay-close-end-session/scope-file.md`。
- 结构化日志 correlationId：本包收尾日志锚 `corr_id=kdp-<ts&0xFFFFFF>`（application.cc local_close_settle 行），K6 链路验证归 OCE-A4 真机目检。

## 8. Closeout 状态

- 制品：docs-delta.md / scope-file.md / review-packet.md 三件落盘包目录；晋升 `delivery/taskpackages/261024-vq-overlay-close-end-session/` 即本目录（本包目录即 canonical 包位，无二次晋升动作）。
- closeout-check `--update-index`：integrator 执行（结果见交付回执）。
- supervisor 机械闸（supervisor-verdict-gate.mjs）与 final verify.mjs：归 planner；verdict.json / verification.json 投影由 verify.mjs 程序 fs-write，integrator 只消费不重跑。
- commit：feature 分支 `dev/261016-sd-card-mount` 本地 commit（不 push、不合 main）——SHA 见交付回执。
