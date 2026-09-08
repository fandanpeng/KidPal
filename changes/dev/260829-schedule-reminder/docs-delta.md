# docs-delta — 260829-schedule-reminder（包 7：日程提醒全链路 · 三档排程 + 网关投递 + 设备弹窗三件套）

> 性质：功能实现批（subsystem = firmware+server，rigor=full，riskTier=high，milestone-affecting，mergeOrder 11）。
> 核心结论：**契约零变更 + spec 无漂移 + canonical docs 本分支零改动**。三件事都是被机器核过的、不是推断（见 §A/§D）。
> 同时本文件显式登记一处**口径差**（V20「排程时预合成存缓存」半边未实现，措辞同时活在 `docs/spec.md` 与契约 §三）——它属「spec↔contract 漂移」四分类候选，须走 architect 预审 + 人类契约门，**不由 integrator 自改**，此处只登记事实与去处（§C-3）。
> docs-sync 判定（integrator）：**canonical 面无需同步**；本包动的是 ADR 追加两条 + ARCH-REVIEW-QUEUE 追加四条 + `.planning/kidpal-mvp/progress.md` append 一段（三者均在 integrator 写权面内）。

## A. docRefs 现状与无需更新理由

| docRef | 现状 | 本包影响 | 无需更新理由 |
|---|---|---|---|
| `.planning/kidpal-mvp/PROPOSAL.md` | FROZEN（`origin.upstreamFrozen = sha1:88a5bdb83abdd867`）：§2 包 7 = 日程提醒全链路（v0.4 重写 + contractBlocked 解除） | 逐条落地（RMD-A1~A8 / RMD-E1~E4） | PROPOSAL 措辞与本包交付一一对应，无漂移；包级收口记走 `.planning/kidpal-mvp/progress.md` append（integrator 例行进度线），不回写 PROPOSAL 本体。closeout-check 的 FROZEN 守卫按 `origin.ref` 机判（本包该 ref 在场且状态行为 FROZEN 形态）。 |
| `docs/spec.md#§4.1`（S-F1.4） | 定稿：按区块判范围 + 三段阶梯 + 重试到确认（60s×3）+ 四态解耦 + V18 滞留顶替 + V20 speech 解耦 + 决策 24 断网无提醒 | 逐字实现（范围/阶梯/四态/V18/介质禁令） | spec 是**本包的输入而非输出**：`docs/spec.md` 不在 integrator 写权面（`.claude/scope-map.json` agents.integrator.allow 无此项），且本包无「实现产生了 spec 未定义的新语义」的情形——两处实现级细化由代码内注记承载、不构成漂移：① 提醒语音的出音通路按 human 裁定二改道引擎既有链（措辞口径差另见 §C-3，不在此列）；② 设备状态实例上界 4096 字节带来的「排队/驻留槽不保存 `view_json`」取舍（`firmware/main/display/reminder_page.{c,h}` 头注记 + Review Packet 残余）。 |
| `docs/contracts/interface-contract.md#§三`（并 §四 / §二 B1） | §三 提醒推送与出队语义（方向 B 重写，2026-09-03 契约门四题批准）；§四 `mcp kidpal/alert` 工具载荷；§二 B1 表格主体 | 只读消费，**一字未动** | 机器核据两条：① `git diff bc0951f..HEAD -- docs/` = **零行**（本分支整个 diff 未触 `docs/**` 任何文件，§D 附输出）；② verifier 终审 r3 `contract-lint` 闸 `ok:true`（闸对象正是本文件）。本包对契约的履职点：§三 五键载荷 `{id, fire_at, title, speech, view}` 零增减（`build_alert_envelope`）、幂等 id = 确定性散列（条目 + 档位 + 事件日期）、渲染即确认即出队、60 秒×3 重试上限、过期静默作废；§四 的 `kidpal/alert` 由设备侧新注册工具承载（`mcp_server.cc` AddTool），小智原生 alert/custom 通道按契约「弃用」未被触碰；B1 表格主体零变更——「备注里哪部分算要带的」由 `server/deploy/openclaw-system-prompt.v3.md`（RMD-A8）承载，**不加列、不加字段**（冻结齿 `test_a8_b1_table_body_zero_change_reuses_package6_parser` 同时钉死「复用包 6 解析器、不另起一套」）。 |

## B. 本包落地（非 canonical 文档面 —— 代码与部署载体）

| 目标 | 位置 | 说明 |
|---|---|---|
| 设备提醒消费面 | `firmware/main/display/reminder_page.{c,h}`（新建 1142+175 行） | 冻结离线契约强定义：`kidpal_alert_parse` / `kidpal_fire_at_minutes` / 四态状态机（`receive` / `raise_next` / `may_emit_receipt` / `mark_rendered` / `mark_acked`）/ `render_actions` / `key` 按键裁决 / 纯 LVGL 渲染面 + 提示音弱缝。定长数组、零动态分配、零持久层（决策 24 介质禁令）。 |
| 设备 `kidpal/alert` 接线 | `firmware/main/mcp_server.{cc,h}`（+254/+15） | AddTool 注册 + `HandleAlertCall`（同步回复恒 `"ack": false` = 仅受理）+ `EmitRenderReceipt`（渲染完成后独立 `ReplyResult` 出 `"ack": true` + `reminder_id`）+ drain timer 驱动排队/V18 顶替。`DoToolCall` 尾部同步自动回复形状与 display 路径零改动。 |
| 装配接线 | `firmware/main/CMakeLists.txt`（+6）、`firmware/main/test/host_app/main/CMakeLists.txt`（+9） | 设备树编入 `reminder_page.c`；host runner `HOST_ACCEPTANCE_SRCS` 加冻结件 + 被测实现 + 自带单测，**保持 `host_infra_smoke.acceptance.c` 末位**（包 0 boundary 齿依赖，实测接线见 §5 注记）。 |
| 服务端提醒投递面 | `server/gateway/reminders.py`（新建 586 行） | 每分钟轮询读 `pending.json` + 机械校验（内嵌 view 过**真实** `display_schema.validate_display`）+ 到点推送 + 重试到确认状态机 + 幂等出队 + 审计事件 + `MAX_*` 有界；`alert_receipt_id` 为提醒路径专用的「只认 `ack is True`」收窄判定。 |
| front 投递接线 | `server/gateway/front/front_app.py`（+160/-1） | 双腿登记（`ALERT_LEG_CAP=8`）、`run_alert_poll_once`（先接出队回执再 tick）、每分钟对齐整分钟轮询、渲染回执经 `consume_receipts` 接上出队方（补 g-relay-fix 留下的「acked_ids 有人收集、无人消费」缺口）。 |
| 网关 relay 数字化 | `server/gateway/mcp_relay.py`（+52/-13） | `alert_tool_call` 改为委托 `reminders.build_alert_envelope`（单一事实源、不分叉第二套算法；reminders 不可达时退回通用 `_tool_call`）。**`relay_ack` 与 `display_tool_call` 冻结语义一字未动**（diff 内该两函数体零改动，仅 `alert_tool_call` 与其上方注记被替换——上游注记明文把「alert 传输 id 数字化」划归本包）。 |
| 生产侧排程脚本 | `server/deploy/openclaw-patch/reminder_schedule_lib.py`（新建 472 行）+ `openclaw-reminder.config.yaml`（22 行） | 裁定一「混合分层」的确定性那一半：三档算术 + 幂等 id 散列 + 原子写 + 机械校验 + 坏输入降级；纯 stdlib、导入期零 I/O；课表解析复用包 6 `gateway/schedule.py::parse_schedule_md`（RMD-E3 不重复造数据）。 |
| RMD-A8 语义约定载体 | `server/deploy/openclaw-system-prompt.v3.md`（新建 181 行）+ `OPENCLAW_SYSTEM_PROMPT_CARRIER` + `apply-xiaozhi-overlay.sh` + `provider.config.yaml` | 版本化惯例：v1/v2 不改可 diff，v3 = 在 v2 基座上把「包 7 — 提醒备注解析约定」占位展开为正式约定（备注列携带物判定 + `entry_key` 对齐 + 三档范围 + cron 回合与对话回合职责分界）；渲染面/声明面/消费面三处同批切。**⚠ 本文件与并行包 story-p3（已并 main）撞成两份不同的 `.v3.md`，合并时必须取并集——见 review-packet「合并门须知」第 1 条 BLOCKING。** |
| 引擎侧出音认领 | `server/deploy/engine-patch/connection.py`（+40） | 裁定二落点：`_kidpal_try_speak` 认领 `{"type":"kidpal_speak","text":…}` 内部指令并转调引擎既有 `speak_txt`，认领后不再进 ASR/LLM；出音失败只记日志不抛（降级不阻塞投递）。 |
| 部署 runbook | `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` §8（+82） | cron 注册（`--command` 载荷）、配置渲染、front 投递侧、live 验收命令与五事实落盘要求、回滚步骤。**本包只写载体，未写 ECS**（live 面需 human 批准，见 review-packet Residual Risk）。 |
| 冻结验收两文件 + 红基线摘要 | `firmware/main/test/schedule_reminder_b.acceptance.c`（1716 行）、`server/acceptance/test_schedule_reminder.acceptance.py`（1681 行）、`firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md`（453 行） | acceptance-author 产出；修订史两条见 §C-2（作者权 + tighten-only）。 |
| closeout 制品 | `changes/dev/260829-schedule-reminder/{docs-delta.md, review-packet.md}` | 本文件 + 审查包（FS-EVIDENCE-3 blocking 两件）。 |
| 决策与治理登记 | `docs/ADR.md`（ADR-002 / ADR-003）、`docs/ARCH-REVIEW-QUEUE.md`（四条 governance ARQ，owner=architect）、`.planning/kidpal-mvp/progress.md`（append 一段） | 见对应文件；ARQ 为**建议条目**，不自我批准、不开 TaskPackage、不改 canonical 规则文本。 |

## C. 明确没动的面（及为什么 —— 别从 diff 缺席里自己猜）

### C-1 `docs/contracts/**` 一字未动（契约零变更）

- 本分支 31 文件 +9084/−117 的 diff 里 `docs/` 前缀文件数 = **0**（`git diff --name-only bc0951f..HEAD -- docs/` 输出为空，§D 附命令）。
- §三（提醒推送与出队语义）、§四（端云协议剖面 `mcp` 工具调用）、§二 B1（课表 MD 表格主体与 frontmatter）**均无字节变化**；`contract-lint` 闸 r3 `ok:true`。
- 按 architect 四分类，本包属**「无需改」**类：契约锁的是载荷形状与送达/出队语义，本包恰好按原形状实现。三处易被误读为「该改契约」的点，逐条给出不改理由：
  1. **JSON-RPC 传输 id 数字化** —— 不是新契约语义：契约 §三 的 `id` 是**字符串业务 id**，本包把它留在 `arguments.id` 不动；数字化只发生在 JSON-RPC 信封的**传输 id** 上，起因是设备 `ParseMessage` 的 `cJSON_IsNumber` 判据（字符串 id 整条按 `Invalid id` 拒收，g-relay-fix 二轮冒烟实证）。该活由上游 `mcp_relay.alert_tool_call` 注记明文划归包 7，属实现层适配、非契约字段变化。
  2. **`kidpal/alert` 设备侧新注册工具** —— 契约 §四 已把 `kidpal/alert` 列为两种 `mcp` 载荷之一并写明「小智原生 alert/custom 通道弃用」，本包是首次实现该既有形状，不新增工具名、不改载荷。
  3. **`remind_before_min` 已废止** —— 2026-09-03 契约门既裁；本包只做「不消费」的守卫（真 ECS frontmatter 里该字段在世、fixture 按真形状造，齿 `test_a2_real_ecs_schedule_shape` 钉它不参与任何计算），不改契约文字。

### C-2 冻结验收件被改过两次，但**不是**放宽（登记修订史）

| 提交 | 内容 | 语义影响 | 依据 |
|---|---|---|---|
| `7c6d7c5` | acceptance-author 修自己冻结件两处自身缺陷：① 钢琴课三档期望簇参考周次（`now` 是周二，`09-07` 周次三档全已过、与同档 past-skip 齿互斥，按 spec §4.1 + 决策 20 取 next occurrence `09-13/09-14`，牵动 3 齿）；② `test_a4_retry_to_ack_60s_x3_then_void` 内 sink 采集形状与自身二元组解包冲突（实测 `ValueError`），拆 `pushed` / `envs` 两采集器 | **断言语义零变更**。自证 = 与被测实现无关的 `datetime` 正向推导 + 三条扰动探针（临期档 ±1min / 丢跨天映射 / 旧期望）全部判不匹配，不是抄实现输出；`test_a2_past_ladders_skipped` 未动仍作守卫。改动仅 python +34/−13，C 冻结件一字未动。 | builder escalation `90fcd38c` → planner 独立核实 → 结案 `33e8e8fa` |
| `7ab2d47` | tighten-only 补两齿（RMD-A3 front 引擎腿注入的离线行为齿 + RMD-A5 渲染面「先清屏」具名化），闭环 mutation 跑出的两处 `sensitive:false` | **纯 additive**：`git diff --numstat` = C 档 `101/0`、py 档 `200/0`，`git diff -U0` 删除行计数 **0**；不新增 acceptance id、不改行为定义；两条新齿各用当初暴露 vacuity 的**原变异**重放自证会咬（新齿 FAIL + 同批其余齿全绿），`mutationEvidence` 对应两条 `sensitive:false` 翻 `true`。 | acceptance-author 作者权 + 本仓既成 tighten-only 纪律（architect advisory `0e55cfab`，意见非判据）。**该纪律尚无成文条款** → 本包登记 ARQ-260908-tighten-only-unwritten（owner=architect）。 |

### C-3 一处 spec↔contract **口径差**：V20「speech 排程时预合成存缓存」半边未实现（须走契约门，不由本包改）

- 事实：`docs/spec.md:332`「**speech 合成解耦（V20 定）**：`speech` 于**排程时预合成**存缓存、到点随推送下发」，以及契约 §三 第二条 bullet「**`speech`** = 排程时预合成的 TTS 文案，音频随推送下发（走下行 Opus/tts 通道）」——**「排程时预合成」这一时间性要求同时出现在 spec 与契约两处**。
- 本包按 human 裁定二（eventId `09359e74-005b-45bb-9535-69e19c14ee04`）落地为：文案在排程时产出（脚本 + agent 语义产物），**音频到点由引擎现合成**（front 注入 `kidpal_speak` → `speak_txt` → 既有 edge-tts provider + 既有下行 Opus/tts 通道），front 侧零合成、零 opus 编码、零二进制注入。契约括号里的「走下行 Opus/tts 通道」半句成立；「排程时预合成」半句**刻意不做**。
- 后果与去处：这是 spec/契约措辞与已裁实现之间的口径差，不是行为缺陷（弹窗 + 提示音必达与降级面均已被齿钉住），但**canonical 文字与 shipped 行为不一致不可长期悬**。四分类应归「spec↔contract 漂移」，处置 = architect-reviewer 预审 + 人类契约门（校准措辞为「文案于排程时产出、音频于到点经引擎既有 TTS/Opus 下行链出音」，或明示保留预合成缓存为后续增强并加注）。本 integrator 无 `docs/contracts/**` 与 `docs/spec.md` 写权，**不自行改一字**，只登记并交回 planner。
- 该口径差同时已写进 `taskpackage.decisionCoverage` 的 `spec-决策27-提醒主动播报` 条目（`landsIn: acceptance:RMD-A3`，正文载明「V20『预合成存缓存』半边的口径差已在 notes 与裁决事件中记明」），并在 Review Packet 的 Residual Risk 复述一次——不留到聊天记录里。

### C-4 其它刻意未动的共享面 / 数据面

| 面 | 未动理由 |
|---|---|
| `server/pytest.ini` | 闸内执行器跑裸 `python -m pytest -q`（无 `--import-mode=importlib`），加 `addopts` 可根治——但 `pytest.ini` 是并行包共享面（本包 notes 与 planner 指令两处明写不动），改它属治理动作。本包保持「冻结件内含 live 齿」形态（既有制式），真跑证据由点名命令承载。风险与实测见 review-packet Findings（NIT 一条）。 |
| `.delivery/milestones.jsonl` / `.delivery/STATUS.md` | 无里程碑状态跳变：`taskpackage.deliveryImpact = milestone-affecting` 但**未**声明 `deliveryMilestoneStateChanged`，`closeout-check` 的 delivery 子闸为条件路径（本包不入该支）。M4「4 页 UI 重做 + 课表 + RTC 提醒」的 `accepted` 需四页 + live 真机齐备，本包只覆盖其中提醒切片，改状态 = 虚报完工度。故 M4 维持 `planning`，STATUS 不需重跑（`checkStatus` 未触发）。 |
| `changes/INDEX.md` | 本相位按 planner 明令跑 `closeout-check.mjs` **默认只读**（不传 `--update-index`），index 投影写权留给 planner / merge 相位。理由：同批 story-p3 刚并入 main，INDEX 行与 `.planning` 面属并发共享文件（ARQ-260907-merge-closeout-bookkeeping-gap 在册）。此为对 integrator 角色默认（step 11 跑 `--update-index`）的一次**有意偏离**，按「上游指令优先」执行并如实登记。 |
| `docs/notes/`、`docs/ui/`、`docs/templates/` | 无新增经验/策略类内容需外溢（踩坑已入 pod-events 与本 packet），UI 观感与网点疏密留 9b 真机相位；ADR 落点按 `docs/ADR.md` 单文件日志（human OQ3 自裁 docs 根平铺，260907 批 T7）。 |

### C-5 本相位**动了**的三件受控文档 + 合并相位的 append 冲突预告

| 文件 | 本包写入 | 编号/冲突注记 |
|---|---|---|
| `docs/ADR.md` | append **ADR-002**（生产侧混合分层，裁定 `acaef302`）+ **ADR-003**（提醒语音借引擎 `speak_txt` 既有音频链，裁定 `09359e74`） | 编号**跳过 ADR-001**：ADR-001 已被并行包 story-p3 于 main 占用（`8f51a95`，主题 = 版本化制品接线不变式），本 worktree base 早于该合并故文件内暂不可见。rebase 时该文件两侧都是 EOF append ⇒ **预计 add/add 或相邻行冲突**，消解 = main 的 ADR-001 与本包 ADR-002/003 **三条并存、不重排编号**。append-only 纪律遵守：既有条目（含「（尚无条目…）」那行历史注记）一字未改。 |
| `docs/ARCH-REVIEW-QUEUE.md` | §1.2 表尾 append 四条 governance 建议条目（owner=architect，**不自我批准、不开 TaskPackage、不改 canonical 规则文本**）：`ARQ-260908-tighten-only-unwritten` / `ARQ-260908-versioned-artifact-slot-collision` / `ARQ-260908-gate-pytest-import-mode-incompat` / `ARQ-260908-known-red-count-no-mechanical-source` | 与 main 上 story-p3 新增的两条（`ARQ-260908-versioned-guard-version-literal-coupling` / `ARQ-260908-firmware-build-degraded-gate`）同表同位 ⇒ rebase 时预计冲突，消解 = 六条全留。**设备构建闸降级一条本包未重复登记**（main 在册条已覆盖「恒降级 + merge/CI 设 `VERIFY_FW_BUILD_REQUIRED=1`」，本包只在其覆盖面上补新事实，见 review-packet Findings #5）。 |
| `.planning/kidpal-mvp/progress.md` | append「2026-09-08 包 7 dev-loop closeout」一段（既有行零改动） | 两侧 EOF append ⇒ 预计冲突；`merge=union` 建议已在册（ARQ-260907-merge-closeout-bookkeeping-gap ③）。 |

## D. 机器可核据（integrator 本机实跑，2026-09-08，HEAD = `339fec3`，工作树未动）

```
git diff --name-only bc0951f..HEAD -- docs/                     → 空输出（canonical docs 零改动）
git diff --stat bc0951f..HEAD                                  → 31 files changed, 9084 insertions(+), 117 deletions(-)
node .claude/scripts/verifier-evidence-check.mjs --change-id 260829-schedule-reminder \
     --phase final --round 3 --invocation-id 73476b97-f17e-44b5-9090-7c07f42ce7ca
    → {"ok":true,"factOk":true,"observedGateExit":0,"phase":"final","round":3}  EXIT=0
cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
    → 34 passed, 5 skipped
cd server && python -m pytest -q --import-mode=importlib
    → 8 failed, 135 passed, 40 skipped      # 8 = 既有红（smoke_e2e 7 + VRT-A4 证据齿 1），一条未动
cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
    → 77 Tests / 3 Failures                 # 三条既有 [evidence] 红 VRT-A2 / WSA-A6 / WSA-A4；本包 12 验收齿 + 5 自带单测齿全 PASS
```

完整闸面与判读见 `review-packet.md` §Verification Evidence。
