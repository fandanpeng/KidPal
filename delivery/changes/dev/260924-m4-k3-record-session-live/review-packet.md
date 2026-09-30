# Review Packet · 260924-m4-k3-record-session-live

> integrator 收拢制品（2026-09-30），供 supervisor 机械合并门复核、高风险/critical 时供人类 review。本包 = M4-K3 记录会话跨端链路收口：板端 `kidpal_session_ctx` 发送缝（protocol 原语 + Listening 分支会话发起入口 + source_page 闭集单点映射）+ 引导态演出（气泡覆写 + 开口切录音态）+ 录音手势族会话接线（STOP 保存 / BOOT 丢弃双路径）+ 录音态话语回显（echo FIFO）+ story_list 推送 gating 板端半 + front ctx 沿条件推送（defer ①恢复重裁）+ prompt v8 双指令批（四件动作）。闭环口径：`M4K3-A1`~`A9`（offline 齿）+ `M4K3-E1`~`E3` exitCriteria；live 真机确认轮 = human 持板（LP4），本包 offline 全绿 + live 待跑态申报。

## Change Size

- **实现面（14 tracked 文件 +395/−57，`git diff --stat -- firmware server` 实测）+ 新增 4 件**：
  - `firmware/main/protocols/protocol.{h,cc}` **+17**：`SendKidpalSessionCtx(const char* source_page)` 原语（逐字仿 `SendAbortSpeaking`/`SendKidpalStoryDelete` 形态，wire = `{"session_id","type":"kidpal_session_ctx","source_page"}` 走 SendText；NULL page 兜底空串；无重试环）。
  - `firmware/main/application.{cc,h}` **+59**：Listening 分支发送点（`SendStartListening` 之前、`IsAudioProcessorRunning` 门之外无条件）；VAD 沿值留档 + 主循环开口切录音态消费（决策出参经 `KidpalStoryVadEnterRecording()` 桥）；stt 分支记录语境旁路喂入；`CloseAudioChannel()` 公开方法（丢弃面）；Idle 分支记录语境清位。
  - `firmware/main/display/story_page.{c,h}` **+173**：seam [S1]–[S4] additive 尾部追加（source_page 映射 / guide_bubble + recording_enter 决策表 / echo FIFO ≤4 句 / list_gate 决策表）+ 记录语境标志对。
  - `firmware/main/display/lcd_display.cc` **+13**：apply_look 区记录语境 LISTEN 场景气泡覆写缝（M1 冻结映射零触碰）。
  - `firmware/main/mcp_server.cc` **+98/−20**：REC_SESSION 消费位会话发起接线（置标志 + echo 清零 + show_chat + StartListening）；STOP_RECORD → `StopListening()`（收尾保存面）；DISCARD_RECORD → `CloseAudioChannel()`（丢弃面）；`KidpalStoryVadEnterRecording()` free function 桥（mcp_server.h 零改——变更清单不含该头，extern 消费，manifest 内合规形态）；VRT_ROUTE_STORY_LIST 接 `kidpal_story_list_gate`。
  - `firmware/main/test/host_app/main/CMakeLists.txt` **+8**：`record_session_live.acceptance.c` 注册进 HOST_ACCEPTANCE_SRCS（RSL-C1 注册序守卫承载）。
  - `server/gateway/front/front_app.py` **+17/−1**：`_consume_kidpal_control` ctx 分支 routing 沿自查（`acc.routing != "accumulate"` 先于 `acc.start`，story.py 零改动）→ 条件推送恰一帧 `story_list`；推送失败 try/except 不阻断 ctx 拦截消费。
  - `server/deploy/` 四件动作：`openclaw-system-prompt.v8.md` 新文件（v7 为基只动故事段）/ `OPENCLAW_SYSTEM_PROMPT_CARRIER` 声明翻转 + v7 沿革条目 / `provider.config.yaml` 三键同步 / `apply-xiaozhi-overlay.sh` PROMPT_SRC/DST 版本行翻转 + `README.md` 指针刷新（现状 = v8）。
  - 新增：`firmware/main/test/record_session_live.acceptance.c`（host 冻结齿 RSL-A1~A7 + C 类结构齿）/ `server/acceptance/test_m4k3_record_session_live.acceptance.py`（a7 五齿 + a8 三齿 + a4 两守卫齿）/ `firmware/main/test/acceptance/red-baseline/rsl_red_wrapper.sh`（红基线通道）/ `server/deploy/openclaw-system-prompt.v8.md`。
- **簿记面**：taskpackage / plan / briefs ×2 / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/r1.json / mutation-evidence-record-session-live.md / current/ 投影。
- **Scope**：firmware（protocols/application/display/mcp_server/test）+ server（front/deploy）双面；`docs/architecture.md` 零直改（patch delta 待批）；契约/spec/requirements/ADR 零触碰。
- **Split recommendation**：无需拆分。变更 = 「板端会话发起信号 → front 条件推送 → 板端消费 gating」一条跨端内聚链 + prompt v8 双指令批，切开即断链（session_ctx 发送半与消费半互为对端）。

## Verification Evidence

- **六闸诊断复跑（integrator，2026-09-30，真实退出码直读）**：`taskpackage-required` **0** / `change-manifest` **0**（pre-cutoff 存量放行）/ `acceptance-coverage` **0**（A1–A8 functional、structural:["M4K3-A9"]）/ `mutation-check` **0**（A1–A8 各有 mutationEvidence、A9 suiteGate）/ `docs-sync` **0** / `security` **0**（securityReviewRequired:false）。
- **Bearing 机器件**：scoped 轮 fact = `delivery/taskpackages/260924-m4-k3-record-session-live/verifier-evidence/rounds/r1.json`（`authority:bearing`、`producedBy:verify.mjs`、`phase:scoped`、`round:1`、`invocationId:08fd9799-1848-4389-930c-f50546cb9294`、`observedGateExit:0`、`ok:true`），8 闸全绿（上述六闸 + `firmware-build` 真编译 build-pass / `contract-lint`）。**本批未跑 final 相位 `verify.mjs --phase=final`**（派单未含 final `round` 与 `invocationId`；K1 包先例同款处理——integrator 不自行生成轮次号以免造伪 round fact）。如需 final fact，请 planner 指定轮次与 invocationId 后补跑。
- **范围圈定（Step 4 两步走）**：① 清单落盘 = `delivery/taskpackages/260924-m4-k3-record-session-live/test-scope-final.txt`（detect_changes impacted 21 节点 + impactedTestsSeed 合成；本包新冻件 ∪ blast radius 映射 ∪ 共享 seam 冻结面 ∪ 冒烟余量，9 件）；② 终跑 = planner 派单指定双通道全量（覆盖清单为下限 floor）。final scope-file 相位 verify 未跑（同上 final round/invocationId 未派）——**清单路径按义务记入本 packet 此行**，供「圈定 vs 事后全 blast radius」差集留根观察。
- **detect_changes（Step 9b advisory 记账）**：project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 池树专属索引**可用**（非降级路径），base main / merge_base `ddd3875`：changed_files **33**、seed_symbols **143**、**impacted_total 21**——`firmware/main` 10 / `server/gateway` 8 / `server/acceptance` 3，**全部落在 changeManifest 面**（application/mcp_server/lcd_display 的既有消费方 + K1 件 acceptance 节点），无 firmware 外溢、无跨子系统意外节点。实现改动只影响预期符号，核对有效。
- **双通道终跑（integrator 实跑，2026-09-30）**：
  - 通道① host：`espidf_wsl.sh build` + `host_infra_test.elf` → **136 Tests 0 Failures 0 Ignored**，`HOST_EXIT=0`（含 K1 27 齿 + K2 冻结齿 + M1/M2/M3 全量既有齿零偏移 + 本包新齿）。
  - 通道② server：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **203 passed, 4 deselected, 2 subtests passed**，`exit 0`（K1 27 齿 + defer 守卫 + 本包 10 齿全绿 + 全量回归零新增红）。
  - device build（r1 闸内真编译）：`xiaozhi.bin` **0x3110e0 = 3,215,584 B**，app 分区 0x370000，余 **0x5ef20 = 388,896 B（11%）**——闸 exit 0。
- **Mutation 重放（builder 第一方，`mutation-evidence-record-session-live.md`）**：sensitive 七条逐条变异 → 红（RED verbatim 与冻结基线同名）→ 还原 → 复绿；host 基线 136T / server 基线 203P。M4K3-A4 守卫齿不重放（must-stay-green，敏感性由 front cancel 语义面承载）；M4K3-A9 套件级豁免（M-4 成文）。

## demo 对账（workstream 包必含①）

- **offline 证据 vs roadmap §3 效果段一致性**：效果段（roadmap `:70`）声明面 = 引导气泡「需要我帮你记点什么？」/ 开口即录 REC 演出 / 整段累积回显 / AI 不插话 / 收尾三式 / 盖章新星+1 / 双入口开录（prompt **v8**——快照修正注：原文「v7」读作 v8）/ BOOT 丢弃真消失 + 真机确认轮一次性看全（双入口/累积回显/收尾三式/盖章 seq/删除云同步/重启流水号不回退）。offline 侧逐半对上：发送缝（RSL-A1/A2/A3）/ 引导态（RSL-A4）/ 手势接线双路径（RSL-A5）/ 回显（RSL-A6）/ gating（RSL-A7）/ ctx 条件推送（a7 五齿）/ prompt v8 三面一致 + 双指令内容锚（a8 三齿）——**offline 可承载半 = 全绿一致**。
- **live 轮证据形态声明**：效果段「真机确认轮一次性看全」本就是 live 形态声明——**设备轮未跑**（LP4 human 在场持板，真机时点 human 裁量，单真机不撞车核对归设备轮前），本包交付 = offline 全绿 + live 待跑态申报，**非落差**（roadmap demo 证据形态 `:72` 明写 human 持板真机确认轮为证据面之一）。
- **数字偏差如实申报**：planner 转述 builder 回执「device build 余量 22.1%」与 bearing 证据 **r1.json firmware-build 实测「0x5ef20 bytes (11%) free」不符**（bin 体积 3,215,584 B 一致）——以 bearing 证据 11% 为准（见 Findings #1）。
- **结论 = 一致（offline 半）+ 待跑（live 半，诚实申报态）**。

## Acceptance 逐锚结果（M4K3-A1~A9）

| 锚 | 内容 | 齿 | 结果 |
|---|---|---|---|
| **A1** | session_ctx 发送缝（原语 + 发送点 + source_page 闭集映射 + 双入口收敛 + toggle 负锚） | RSL-A1/A2/A3 | **绿** |
| **A2** | 引导态（气泡逐字单源 + apply_look 覆写 + 开口切录音态决策表 + REC_SESSION 接线） | RSL-A4 | **绿** |
| **A3** | 录音手势会话接线（STOP → StopListening 保存面 + DISCARD → CloseAudioChannel 丢弃面 + 双负锚） | RSL-A5 | **绿** |
| **A4** | 引导态结束 ≠ 录音态丢弃 front 双路径守卫（真实连接流程两齿） | test_a4_*（must-stay-green） | **绿** |
| **A5** | story 录音态话语回显（echo FIFO 机理 + stt 旁路喂入） | RSL-A6 | **绿** |
| **A6** | story_list 推送 gating 板端半（决策表 + 路由消费 needle） | RSL-A7 | **绿** |
| **A7** | ctx 条件推送 front 半（首帧恰一帧 / 重复零帧 / 非 story 零帧 / 落点结构守卫） | test_a7_* 五齿 | **绿** |
| **A8** | prompt v8 双指令批（三面一致解析 + 内容锚 + v7 sha256 零回改） | test_a8_* 三齿 | **绿** |
| **A9** | 零偏移与注册守卫（suiteGate：spec/契约 canary + K2 语义位 + HOST_ACCEPTANCE_SRCS 注册序 + 全量承载） | RSL-C1 + 全量通道 | **绿** |

## exitCriteria 铁律逐项复核

- **M4K3-E1（host 齿全量零红 + server 零新增红 + device build 闸）**：136T0F + 203P0F（integrator 复跑逐字）+ firmware-build exit 0（3,215,584 B / 11% 余量）。**遵**。
- **M4K3-E2（铁律）**：① `docs/contracts/**` 与 spec 零改（`git status --porcelain docs/` 零输出 + contract-lint 绿）；② prompt v8 新文件不回改 v7（a8 sha256 铁律齿绿 + carrier 声明翻转 + 渲染脚本版本行翻转 + ADR-001 零版本字面断言）；③ OpenClaw 本体架构零改动（prompt = 部署制品四件动作模式）；④ 板端零新增计时器（复用既有 StoryTickTimerCb 演出 tick，无新增 timer 生产者；60s/120s 设备侧 timer 不复活——`ses_machine_dispatch` 生产零调用现状未触碰）；⑤ StoryAccumulator/front 拦截 K1 冻结语义零触碰（ctx 分支只增条件推送增强，拦截/累积/cancel 面只消费；K1 27 齿全绿佐证）。**遵**。
- **M4K3-E3（live 确认轮证据落账 + 真机轮后段落账 + docs-sync 面）**：live 轮**未跑**——本包交付 offline 全绿 + live 待跑态申报（r1 证据 + 本 packet 未覆盖面清单 + docs-delta H1「live 确认轮待跑」措辞）；progress.md K3 段落账 + pod-retro.md 属真机轮后 closeout 相位（planner 派单明示本相位不写）；docs-sync 面 = docs-delta §4 patch delta 待批；milestones M4 行 state 收口提议只备证据不直改。**部分遵（live 半挂账，申报在案）**。

## 未覆盖面申报（workstream 包必含②；对照 `acceptance[]` + 契约条目 + roadmap §3 差集逐条一行）

**live 目击项清单（offline 无假齿、全部归真机确认轮）**：

1. **AFE VAD 真沿切页** — host 只机判 UTT_START 事件沿纯逻辑决策（RSL-A4 决策表）；真 VAD 沿→录音态切换演出归 live（M4K3-A2 验收明示「AFE VAD 真沿 = live 目击项，不造 offline 假齿」）。
2. **ctx wire 逐字节复证** — offline 齿为机制级代理（SendText 调用 + 消息形状拼接断言）；设备→front 真链路逐字节（session_id 拼接、TLS 后拦截）归 live（M4K3-A1「逐字节复证挂设备轮」）。
3. **LLM「帮我记下来」真判定** — prompt 文本双指令 + 边界负例 offline 可机判（a8 齿）；真机「帮我记下来」开录 / 「帮我记下来明天带跳绳」不误触课表 = live 目击（notes ⑩，M3-K3 同款诚实申报）。
4. **引导→录音演出冲突** — 引导气泡、开口切录音态、REC 演出、话语回显在真机 1bpp 屏上的时序冲突/观感（行高视觉拟合等）offline 不可验，归 live。
5. **defer 包「设备侧停弹页面」复验** — 260928-story-connect-push-defer 遗留真机复验项，由本包 gating 落地后的设备轮一并目击（crossPackageBehaviors 承接条在案）。
6. **K1-A7 与 K2 deferred live 半** — K1 全链 live 复证（120s 前端计时真循环 / 建连面 / 引擎侧来源标记）+ K2 真机目测半（手势手感/1bpp 观感）全部在本包 live 确认轮一次性收口（crossPackageBehaviors 第 3 条）。
7. **收尾三式 + 盖章 seq + 删除云同步 + 重启流水号不回退目击** — roadmap 效果段「真机确认轮一次性看全」清单；offline 为齿级承载、真机目击归 live 轮（front 账本证据 + 设备侧盖章 N 目测双证）。

**显式取舍与登记项**：

- **M4K3-DC-1（dropped 显式取舍）——单独提示 human**：「P3 冷浏览（无会话翻页）无列表数据源」残余缺口——ctx 时点恢复后，未开会话直接翻到 P3 浏览列表仍无数据源（契约零改铁律下无干净缝：story_list 为 front→设备下行、设备无请求消息，加 fetch 消息 = 契约改动越界）。补偿语义 = 首次开录（ctx 沿推送）/ 归档后 / 删除后三时点可见 + K2 影子缓存跨会话留存 + KSTORY_EMPTY_HINT 空态承载。**若 human 要求冷浏览有列表 = 需开契约门新增设备请求消息（非本包面）**。
- **契约 §1.5 括注漂移**（story_list 时点枚举陈旧——未记 defer 摘除①与本包 ctx 沿恢复）：登记在案、处置走契约门四分类（architect-reviewer + human 契约门），不在包内自改契约。
- **跨子系统 e2e / correlationId 链路验证**：跨端对偶面（`_story_feed_from_stt` / `_story_route_display_payload` / `StoryAccumulator`）front 半 K1 冻结 + 板端半本包落地，offline 双通道齿级闭环；端到端 correlationId 三源日志关联归 live 轮（见上第 2/7 条）——**offline 侧缺口 none**（对照 `acceptance[]` 九锚全覆盖：A1–A8 行为齿 + A9 套件门，acceptance-coverage 闸零缺报）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无新增 `minimal:` 标记） | — | — | no-trigger |

- `grep -rn "minimal:"` 对本包 14 个实现文件实测：5 处命中**全部为 K2 已合并存量**（`git diff` 新 hunk 零命中，exit 1）——本包无新增 `minimal:` 简化债务标记。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Evidence bookkeeping | planner 转述 builder 回执「device build 余量 22.1%」与 bearing 证据 r1.json firmware-build 实测「0x5ef20 bytes (11%) free」不符（bin 体积 3,215,584 B 一致；疑为 builder 早中期构建快照转述）。 | changed-artifact 锚：`verifier-evidence/rounds/r1.json` results[firmware-build].stdout 逐字 + 本 packet demo 对账节；heuristic = 转述数与承重机器件数冲突时以 bearing 证据为准 | 以 11% 为准记账（本 packet 已照实）；builder 回执转述链无需回改，retro 素材 |
| NIT | judgement-call | Firmware concurrency | `application.cc` 文件级 `static bool s_vad_speaking` 由 VAD 回调（音频任务）写、主循环读，无锁——32 位标量宽松读，且真同步锚是既有 `MAIN_EVENT_VAD_CHANGE` 事件位；代码注释已自报「与既有跨任务宽松度同级」。 | changed-code hunk `application.cc` K3 注记块（`s_vad_speaking` 定义处）；heuristic = 单 bool 撕裂读不可见 + 事件位承载真同步，风险为理论级 | 明示接受（与既有 `s_story_rec_entered` 同款单写者约定）；多任务强化归 retro 素材，不阻断 |
| ADVISORY | judgement-call | Spec face | `docs/architecture.md` §7「契约门在办」存在 K1 新旧两行并存重复（`:341` 新文 / `:342` 旧文）——疑为 K1 C4 spec 批 c735062 落地时插入未删旧。 | changed-code hunk：spec 现文 `:341`–`:342` 实读（本 packet 撰写时点）；heuristic = K1 docs-delta H5 声明为「替换」而实态为「并存」 | 交 human spec 批删除 `:342` 旧行（docs-delta §4.8 交接申报已载）；本包不越权改 spec |
| ADVISORY | judgement-call | Governance / write-scope | integrator scope-map allow 含已废弃 `docs/systems/**` 却不含 `docs/architecture.md`（K1 Findings #1 同族未决）——本包 arch 收口义务再次以 patch delta 形态间接落地。 | `.claude/scope-map.json` `agents.integrator.allow`（K1/K2/K3 三包实读一致）；heuristic = 义务与写权路径持续不匹配 | 沿用 K1 建议（loop-retro/human：撤 `docs/systems/**`、按需补白名单）；非阻断 |

> 结构闸（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）PASS/FAIL 不复述为本表 finding；本批**无 hard-violation finding**（全闸真实退出码 0、零契约/spec 漂移、detect_changes 无越面符号、manifest 与实际 diff 面一致——`docs/architecture.md` 在 manifest 内但为「预期零直改」项，零 diff 即合规）。

## Five-Axis Review

1. **Correctness（是否满足声明 `M4K3-Ax`，边界条件）**：A1–A9 全绿。边界承载：source_page 越界/负值兜底 "home" 绝不外溢第四取值（RSL-A3 闭集齿）；echo NULL/空串/全空白拒绝不污染计数、满容量 FIFO 挤最旧（RSL-A6 机理齿）；recording_enter 决策表幂等/非记录语境/阅读态/越界全不切（RSL-A4）；gate 决策表 VOICE 前台/录音态只装缓存、LIST 冷浏览照旧推前台（RSL-A7 双向）；ctx 重复/非 story/普通会话零帧 + 落点结构守卫（a7 五齿）；prompt 边界负例 + v7 sha256 零回改（a8）；STOP/DISCARD 双路径负锚（RSL-A5 双向）。发送点在 IsAudioProcessorRunning 门之外保通道复用会话不漏发（RSL-A2 恰一次齿）。
2. **Test quality（是否断言真实行为）**：host 齿为行为齿（frozen-seam weak 哨兵 + 强符号覆盖机理——实现缺席即红，mutation 7/7 逐条转红还原实证非空过）；server 齿走真实连接流程（a4 守卫齿全程 ws_gateway 离线鉴权/泵/断连 cancel 钩子）+ wire 帧级断言；v8 齿版本无关解析（ADR-001，后续 bump 不假红）。冻结期修订窗 2 条（R1 线束假象 / R2 落点切片收紧）均为 fixture 自身缺陷修正、零断言语义变化、零 fixture 缺陷红残留（REVISION LOG 在件头）。
3. **Contract/boundary safety（是否漂移契约/spec）**：契约零触碰（wire 形状逐字 §四 冻结版；§1.5 漂移维持登记走契约门）；spec 零直改（patch delta 待批）；K1 front 冻结语义 / K2 冻结面（story_page 语义位 + delete 原语）/ M1 冻结映射（kidpal_voice_states）只消费零修改；`mcp_server.h` 零改（桥函数 extern 消费，变更清单不含该头）。**无漂移**。
4. **Operability/security（错误/权限/输入/secret/日志）**：发送失败降级不重试（notes ③）；front 条件推送 try/except 不阻断 ctx 拦截消费（K1 建连推送先例同款）；DISCARD 面零归档语义（毁稿防护负锚双向钉死）；无 secret 落档、无 shell 注入面（deploy 四件为纯文本/指针翻转）；日志留痕（story_key/story_list_gate/story_vad_recording_enter ESP_LOGI 带 page/计数上下文）。**无安全残余需人类判断**（security 闸 securityReviewRequired:false）。
5. **Maintainability/simplicity（局部/易懂/无过早抽象/无重复实现）**：决策逻辑全部住 story_page.c 纯逻辑缝（决策/视图动作分离，host 可机判）；echo 缝与 kidpal_voice_echo 零共享态零重命名；source_page 映射单点一次做全（notes ④ 防散落根因）；protocol 原语同族复用 K2 delete 缝形态；front 条件推送 6 行复用 `_push_story_list` 单源。`KidpalStoryVadEnterRecording` free function + extern 为免扩 mcp_server.h 公共面的克制形态（builder 回执声明 manifest 内合规，本 review 核实一致）。

## C4 arch 收口处置（本批不可写）

- 详见 `docs-delta.md §4`：八处 hunk（H1 `:205` / H2 `:213` / H3 `:290` 后追加 / H4 `:297` / H5 `:382` / H6–H8 连带指针面）+ §4.8 交接申报（§7 重复行清理建议）。
- 处置声明：**合并后由 human spec 批逐字过目落地**；H3 在 K2 件 §4.8 H7 之后 apply（或同批按序）；H2 supersede defer 件 §4.1 的①挂账措辞。**本批不写 `docs/architecture.md` / `docs/requirements.md` / `docs/contracts/**`**。

## Residual Risk

- **Known limitation**：① live 真机确认轮未跑（E3 live 半挂账——七项目击清单见「未覆盖面申报」，LP4 human 持板、时点 human 裁量）；② M4K3-DC-1 冷浏览无列表数据源（dropped 显式取舍，提示 human 项）；③ arch delta 待 human spec 批落地（落地前 §5.2④/§5.5.5/§5.5.4 保持收口前文本 + §7 重复行）；④ final 相位 verify.mjs 未跑（final round/invocationId 未派，K1 先例处理——scoped r1 bearing fact + 双通道全量绿为现行事实验证基座）。
- **Follow-up recommended**：① 真机确认轮（七项目击 + 三源日志 + human 记名）→ 轮后 progress.md K3 段 + pod-retro.md + milestones M4 state 收口；② 契约门：§1.5 括注漂移处置（四分类）+ （若 human 要求）DC-1 冷浏览 fetch 消息契约扩展；③ loop-retro：Findings #1 回执转述链 / #2 宽松度约定 / #4 scope-map 修订。
- **Human decision needed**：① **M4K3-DC-1 取舍确认**——维持 dropped（冷浏览空态补偿）或开契约门新增设备请求消息（非本包面）；② arch delta spec 批落地（与 K1 C4 余项 / K2 件 delta 同批搭载可一行收口）；③ 真机轮时点裁量（LP4 持板 + 单真机不撞车核对）。
- **decisionCoverage dropped/risk-accepted 过滤**：`M4K3-DC-1` — P3 冷浏览无列表数据源残余缺口 — `landsIn: dropped` — reason 前缀为「契约零改铁律约束下无干净缝…」（**非** `risk-accepted` 前缀，显式取舍形态）——human accepted 处置 = taskpackage decisionCoverage 登记态（2026-09-30 冻结），本 packet「未覆盖面申报」已单独提示。
- **pod-events advisory 检查**：`delivery/taskpackages/260924-m4-k3-record-session-live/pod-events.jsonl` 在世（本包 Pod 事件流）；post-cutoff 口径不产 session-digest/loop-health/loop-metrics/pod-journal（EVENTS_SCHEMA_CUTOFF 250725 后），缺失不阻塞 closeout。

## 集成执行回报（Merge-Chain 节，条件触发）

- 本包收拢为单 worktree 直接收口，**未触发** merge-driver 冲突消解路径（无 Pod 分支 add/add 冲突、无 rebase 重放）——前置三项（逐文件预登记 / human 授权事件 / 常驻分支亲核）未进入判定，本节仅留痕「未触发」。消解复跑清单（双方冻结件 + 全档 + 九闸）不适用。

## 回滚方式

- **本仓改动回滚**：本 Pod 分支未 push、未 merge；弃用 = 丢弃本收口 commit（及 verifier 之后的簿记 commit）即可。回滚面 = firmware 七件 + server 五件（tracked）回到 HEAD^ 态 + 删除新增 4 件（两验收件 / rsl_red_wrapper.sh / v8 prompt 文件）+ closeout 制品与簿记为新增文件无既有面破坏。
- **prompt 面回滚**（如需退回 v7 现役）：还原 carrier 声明行 + provider.config 三键 + apply-xiaozhi-overlay.sh 版本行（v8 文件本体可留树不影响接线）；**严禁回改 v7 本体**（sha256 铁律齿会红 = 正确防线）。ECS 侧若已渲染 v8，data 目录回置 v7 文件即可。
- **运行时面回滚**：本包零新增云端持久面（story_list 推送为既有单源复用；板端 echo/gate 均为 RAM 态随重启消散）——无数据迁移面。
- **合并门**：本批 pending supervisor 机械闸复核；supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后，由 planner 主线程 `merge-driver --pod` 本地合并（integrator 不 push、不 merge、不烧录）。

