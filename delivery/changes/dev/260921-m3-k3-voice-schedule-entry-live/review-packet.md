# Review Packet · 260921-m3-k3-voice-schedule-entry-live

> workstream m3-schedule ｜ M3-K3 语音问课表口径（prompt v6 + 「帮我显示课表」意图链）+ M3 live 设备确认轮（K1/K2 deferred 真机半随轮收口）｜ mergeOrder 35 ｜ rigor full ｜ riskTier medium ｜ subsystem server ｜ sourceType delivery-story
> integrator 收拢制品（2026-09-21）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。dependsOn [K1, K2] 均已 merged。

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit）**：
  - `server/deploy/openclaw-system-prompt.v6.md`（新文件）——v6 = v5 基座逐字保留 + 版本沿革前言 + 课表段重写：答复走问答框文本（AC1 不切页不定位）、答复含备注内容（AC4 带物读回）、「帮我显示课表/打开课表」意图 → schedule 薄载荷 `data:{}`（AC2 意图信号化，K1 契约门 Q2 甲案）、网格数据 LLM 不生成（front 机械算，契约 §1.3②）+ display 用法表 schedule 行改述薄载荷语义（Q5 即废旧形状）。
  - `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` + `apply-xiaozhi-overlay.sh` + `provider.config.yaml`——三面接线同步指 v6（接线不变式：渲染面 == 声明面 == 载体面 == 被测面，ADR-001；版本无关解析，ARQ-260908 零版本字面）。
  - `server/gateway/schedule.py` +26——`expand_schedule_intent_payload` 增量纯函数：恰薄载荷（dict + view==schedule + data=={}）才截获，展开 = `week_grid_payload` 单源同参复用；其余一切 payload 返回 None 原样走 relay 链；脏 B1 沿 ValueError 大声传播。既有 parse/网格/upcoming 函数语义零偏移。
  - `server/gateway/front/front_app.py` +50——relay 链接线：薄载荷在 validate_display/下发之前被截获替换为展开网格（离线机械验证为源锚代理 + 真机确认轮 AC2 目击兜底，见验收件文件头强度申报）。
  - `server/tests/test_schedule_producer_units.py` +66——截获展开边界单测（贪截获拒绝、非薄载荷透传等规格未逐字段钉的边界）。
  - `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py`（新文件，14 齿）——A1 v6 落盘/措辞/diff 界面/接线锚齿、A2 意图链四齿（含 wire 往返 + 真实 validate_display 门）、A3 设备轮落地/清单对账两齿；文件头「契约条目 ↔ 齿强度申报」+ 显式缺口申报在案。
- **回填与过程账**：`delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/taskpackage.json`（redBaselineResult / mutationEvidence 14 齿 RED→GREEN 回填）+ `pod-events.jsonl` + `verifier-evidence/rounds/r1–r2.json` + `verify-runs.jsonl`（verify.mjs 程序 fs-write 过程态）+ `delivery/workstreams/m3-schedule/progress.md` K3 节。
- **closeout 制品**：本目录 `device-round-record.md`（M3K3-A3 真机确认轮记录，三源日志逐项对账）+ 本件 + `docs-delta.md` + `verdict.json` / `verification.json`。
- **Scope**：`server/deploy/**` + `server/gateway/**` + `server/tests/**` + `server/acceptance/**` + taskpackage 过程账；firmware/**、docs/**、.claude/** 零命中（`git diff -- docs/ .claude/ firmware/` 实跑零输出）。ECS 部署（v6 prompt + front_app + schedule.py 等 6 文件，`.bak-260921-k3-v6` 备份在远端）已随设备轮前置完成（progress.md「K3 设备轮进行中」段留痕）。
- **Split recommendation**：不拆——v6 口径、意图截获展开、接线三面、设备轮制品是同一条「语音问课表」链路的四段，拆开任一段都无法独立验收。

## Verification Evidence

- **final round 2 九闸全绿（integrator，`d4839306-bfe9-4f4e-9716-d106dbd8ead2`，2026-09-21，`ok:true` / `observedGateExit:0`）**：evidenceRef `delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/verifier-evidence/rounds/r2.json`（immutable verifier fact，本 packet 只引用不复制）。scoped r1（`5b656c37-…`，verifier，7 闸全绿，offline 半程）同 `ok:true`。
  - taskpackage-required / docs-sync / security / contract-lint：全绿。
  - firmware-build：真编译未降级（xiaozhi.bin 0x30c2a0 bytes、app 分区余量 11%；本包 server-only，闸保固件基线不红）。
  - unit：**95 passed**（server/tests 54 + 验收件收集 41）。
  - acceptance：oracle-pass + executor-run **41 passed**（`server/acceptance/` 全目录：K1 17 齿 + K2 归档外既有 + 本包 14 齿全绿，零新增红）。
  - mutation-check / acceptance-coverage：functional = M3K3-A1/A2/A3 全部在册（14 齿，12 RED→GREEN + 2 must-stay-green，redBaselineResult 12 failed/29 passed 实跑落账 taskpackage.json）。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260921-m3-k3-voice-schedule-entry-live --phase final --round 2 --invocation-id d4839306-…` integrator 实跑 **exit 0、`factOk:true`、`observedGateExit:0`**——final evidence 有效，可进 closeout。
- **integrator 独立复跑 demo 命令**：`python -m pytest -q --import-mode=importlib server/acceptance/` → **41 passed**；本包单件 **14 passed**（2026-09-21 integrator 实跑，A3 两齿随 device-round-record.md 落盘转绿，与 final r2 acceptance 闸一致）；`server/tests/` 54 passed 零回归。
- **Impact audit（worktree 降级形态）**：detect_changes **N/A (worktree)**（Pod worktree diff 对主索引不可见）——替代抽查：① `git diff --stat` 闭集核对（见 Change Size，firmware / docs / .claude 零命中）；② 新增符号 `expand_schedule_intent_payload` 消费闭集 = front_app relay 接线（本包）+ 冻结齿/单测（行为断言），零其它消费方；三面接线解析（渲染/声明/载体）全部唯一指向 v6（`test_a1_prompt_wired_three_faces_version_agnostic` 齿背书）。
- **Docs sync**：`docsSyncRequired=false` 义务核清（判定与依据详见同目录 `docs-delta.md`）——spec/契约零改动，prompt 版本化演进面随代码与部署制品承载。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。
- **Loop metrics / Pod Journal（post-cutoff dual-format）**：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`；`pod-events.jsonl` 在场（advisory 检查通过），缺失不阻塞 closeout。
- **机器证据投影**：`current/verdict.json` + `current/verification.json`（verify.mjs 程序 fs-write 投影，non-bearing advisory）已随本次收口 commit 刷新至本包 final r2 口径，并按 SSOT 表晋升副本至本 closeout 目录（只消费不重跑）。
- **真机确认轮三源取证**：串口 `c:/tmp/k3_round1.log`（仓外，先例 = K2 device-round-record 不拷贝进仓只引路径）+ ECS front journalctl / 引擎 docker logs（integrator 收口实拉，摘录与归因更正见 device-round-record.md）。

## Demo 对账行（taskpackage `testHost` / roadmap §3 K3 效果段证据形态核对）

taskpackage 权威口径（`testHost` 逐字）：**「pytest offline 齿 + human 持板真机确认轮（三源日志取证）」**；roadmap §3 K3「demo 证据形态」：「pytest offline 齿（prompt v6 锚 + 意图路径）+ human 持板真机确认轮（串口 + front/引擎日志三源关联取证）」。逐项对账：

| demo 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| offline 齿（prompt v6 锚 + 意图路径）RED→GREEN | 红基线 12 failed / 29 passed（acceptance-author 2026-09-21 实跑落账：v6 未落盘 5 + 接线锚红 1 + 意图展开未实现 4 + 设备轮未落 2 + 2 must-stay-green）→ 实现后 **14/14 全绿**（`server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py`，final r2 acceptance 闸 + integrator 独立复跑一致）；全目录 41 passed 零新增红 | 一致 |
| human 持板真机确认轮（三源关联） | `device-round-record.md`（2026-09-21 晚，human 目击设备轮 OK）：串口 30min 完整捕获 + ECS journalctl + 引擎 docker logs 三源逐字对账——AC2 意图链完整目击 ×2（22:11:37 courses=37 / 22:44:01 courses=40，均自动切 P2）、AC1 问答框答复（route:text，不切页，日期准）、三触发①三度到端目击、②通路级 journalctl 行、QQ 录入 37→40 全通路实证 | 一致（缺口逐条显式化，见未覆盖面申报） |
| roadmap §3 K3「完成后人能看到什么」效果段 | 「答复在问答框给、该带的念出来、不切页」= AC1/AC4 目击（书法班时间地点进答复）；「明确说帮我显示课表屏幕才切 P2」= AC2 目击 ×2；「QQ 录一条课后课表页·问课表·提醒三处即时变化」= human 目击确认 + 门数 37→40 机器实证；「首页接下来三触发刷新」= ①确认/②通路级/③human 目击 + 机器日志缺位挂账；「网格版式/今天列/※/星标/14px 一次性看全」= 网格确认，细节四项 human 未逐项口述（如实挂账） | **一致但带挂账**——效果段主链全目击，目击粒度差异逐条显式化（见未覆盖面申报 1/2/3），无静默缩水 |

**一致性结论：demo 证据 == roadmap 效果段承诺。** offline 14 齿 RED→GREEN 全程留痕 + 真机确认轮三源取证在案，效果段承诺的「问答框答复 / 意图切页 / 录入秒知道 / 三触发」主链全部有目击或行为级齿背书；目击粒度未覆盖的细节项（※/★/明早/14px 逐项、真零点点位、③ journalctl 行）逐条挂账显式化，不存在「口头宣称已验」的条目。

## K1/K2 deferred 真机半闭环声明（M3K3-E3 closeout 对账）

- **M3K1-A5（K1 live 三触发复证，structural deferred 归本包设备轮）**：随本轮按证据程度收口——触发①联网建连**确认**（串口 `upcoming_apply ok` ×3 + front WS accepted + engine route:home acks 三源同拍）；触发②零点档**通路级闭环**（journalctl `trigger=midnight … sent=1` 22:00:34 在案；如实注记 = v6 部署重启后补推，非真零点 tick，真零点点位未目击）；触发③录入后即时**human 目击确认 + 门数 37→40 设备侧实证**，但 journalctl `trigger=immediate` 全量回溯 0 命中——机器日志半未闭环，缺项在未覆盖面申报④挂账，不写全闭环。
- **M3K2-A7（K2 真机半：版式 1bpp 目测/翻页手感/今天列观感等）**：human 报「切页显示正常、网格有课」= 切页/网格两项**确认**；※/★/「明早」标签/14px 目测细节 **human 未逐项口述确认**——按 human 实际确认范围如实记录（device-round-record.md「K2 deferred」节），不扩写为已确认。板级零点跨天重读 RTC 遗留（K2 挂账）本轮未见异常复现，遗留归属不变。

## 未覆盖面申报（对照 acceptance[] + 契约条目 + roadmap §3 效果段，逐条列「本次无齿/未跑的设计性质 + 现状 + 去向」）

| # | 未覆盖面 | 设计性质与现状 | 去向 |
|---|---|---|---|
| 1 | **※/★/「明早」标签/14px 真字形观感未逐项口述确认**（M3K2-A7 细节半 + NFR-READ-1 目测） | human 真机轮实际确认范围 =「切页显示正常、网格有课」；四个细节项在真机屏上有渲染条件（网格 37/40 门在场）但 human 未逐项口述，机器侧无逐字形目击齿 | 后续真机轮（任一 K 包设备轮顺带）逐项口述补目击；不阻塞本包（非本包 acceptance 义务，K2 deferred 口径） |
| 2 | **设备时钟缺陷 → 今天列高亮落错列**（周五列，实为周一） | 非本包义务：根因 = OTA 响应缺 server_time（K1 热修轮登记的独立缺口，串口 tm_year 70 警告全程在场） | K1 热修轮缺口②独立小变更；修后真机轮复验今天列 |
| 3 | **WS 会话中段静默断开**（本轮 +9s/+59s/+178s 三次 mbedtls error=76，断开前均有按键事件，归因未定位） | 非本包义务：K1 热修轮缺口①同族（~36s 掉线）；设备均自动重连成功、未阻断清单执行；「断开跟随按键」的观察已如实入档，NAT/keepalive vs 主动 abort 两假设并存 | K1 热修轮缺口①跟踪；复现归因后独立修 |
| 4 | **触发③ journalctl `trigger=immediate` 0 命中**——「录入 → marker → front 恰一次消费 → 即时推」的机器日志环节缺位 | QQ 录入的设备侧效果有实证（37→40 + human 目击三处即时变化）；但 marker 链路（reminder_schedule_lib 落笔 → 轮询消费 → 日志行）无 journalctl 证据：lib 已随 v6 部署批上 ECS（progress 在案）仍零 firing，疑 OpenClaw 容器侧 drop 入口未触达或 alerts 目录错位；851.5s 的 home 显示经三源关联为 farewell 轮 view=home（v6 口径「说再见 → home 空」），非 ③ 推送——派单素材的 ③ 归因已更正（device-round-record.md 时间线） | **K1 部署面复核项**：核 OpenClaw 容器内 drop_refresh_marker 挂载与 alerts 目录路径一致性 + 后续录入场景对齐取证；交 planner/主线程 |
| 5 | **真零点点位未目击**——trigger=midnight 当日 4 条均为部署重启补推（20:07/20:18/20:28/22:00:34，pid 各异） | ② 通路端到端为真（due 判定 → payload → 下发 → applied）；「每 local 日恰一次」在重启窗口不成立 = K1 review-packet finding #1 的真机复现实录 | 真零点目击归后续自然运行窗口（无需专门轮）；重启补推幂等无害（K1 finding #1 结论维持） |
| 6 | **S-F1.5 AC2 直改文件副通道/跨天兜底未专门目击** | human 未口述直改 schedule.md 副通道测试；兜底语义（无 marker 不即时推、靠①②兜底）由 K1 offline A4 模拟齿承载 | 后续真机轮顺带目击；非本包 acceptance 义务（A3 清单内为 human 目击项，未发生即如实记） |
| 7 | **LLM 意图措辞遵循度真机目击覆盖范围有限**——v6 措辞是工件面，「prompt 写明 ≠ LLM 遵循」（验收件强度申报③显式化） | 本轮正向目击 ×2：「帮我显示课表」「帮我显示课表页」两种措辞均正确触发意图链且答复不产网格（courses 由设备回执计数）；反向面（LLM 在无意图时误切页、问课时误产课表数据）本轮未构造 | 长期随用随验（行为在线、外显）；如出现误触发走 prompt v7 演进通道（版本化纪律不变） |
| 8 | **首页 upcoming CJK 课程名画不出** | 非本包义务：挂账④已立项 K4 包 `260921-m3-k4-p1-cjk-dynamic-text`（Lite，分支 commit `40ce8c7`，human 立项 ack + triage ②单包 Lite 在案） | K4 包串行承接（排 K3 合并后） |

以上 8 条**零一条是本包义务内未清项**（1/5/6/7 为口径内 human 目击粒度边界或显式自陈缺口，2/3/8 为他包在册缺口/立项，4 为 K1 部署面复核项转出）——故不写 `none`，逐条如实挂账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | server/deploy（ECS 部署面） | **触发③ marker 链路在 ECS 无运行痕迹**：journalctl `trigger=immediate` 全量 0 命中、alerts 目录无 marker 残留——QQ 录入后设备效果可达（37→40），但「OpenClaw 改写 MD → drop_refresh_marker → front 恰一次消费」链路未见 firing。reminder_schedule_lib 已随 v6 部署批上 ECS（progress.md「v6 部署 ECS 完成」段在案），疑点收窄到 OpenClaw 容器侧 drop 入口是否真的落笔（QQ 录入实际走的话路是否经过该 patch 入口、alerts 交接目录挂载是否同一路径） | changed-evidence hunk（device-round-record.md 触发③节：journal 全量回溯 0 命中 + alerts 目录仅 pending.json）+ 启发式：lib 已部署（progress 在案）但零 firing 记录，「部署缺件」假设降权、「drop 入口未触达/目录错位」假设升权，两者均未排除 | 不 BLOCKING 的理由：③ 的效果面有 human 目击与 37→40 实证兜底，①②通路独立在案；建议 planner/主线程在 OpenClaw 容器内核 drop_refresh_marker 挂载与 alerts 目录路径一致性，补一次录入场景日志取证（未覆盖面④） |
| NIT | judgement-call | delivery/changes/dev/…/device-round-record.md（本包制品） | 派单素材将 851.5s `view=home upcoming_apply ok` 归因为「三触发③录入后即时推送」，三源关联显示该时点（22:19:59）早于 QQ 录入的设备侧实证时点（37→40 发生在 22:11:42→22:44:08 间）、且与 farewell 轮同拍（v6 口径「说再见 → home 空 data」）——素材归因有误，记录已按证据更正 | changed-evidence hunk（device-round-record.md 时间线「归因更正」行 + engine 22:19:51–22:20:01 farewell 链逐字摘录）+ 启发式：courses=37 在 22:11:42 仍在册，录入发生于其后；851.5s 早于 22:43:42 的 courses=40 会话 | 已在 device-round-record.md 更正并在此复述；无代码动作。教训：真机轮素材的三源关联应在落记录前逐条对钟（本轮由 integrator 补做） |
| NIT | judgement-call | server/gateway/schedule.py | `expand_schedule_intent_payload` 的「恰薄载荷」判据对 `data` 为 `None` 的形态走「data 非 dict → None 不截获」分支——若上游 LLM 产出 `data:null` 薄载荷变体（契约 §1.3② 未定义该形态），将透传给 validate_display 拒绝降级而非展开 | changed-code hunk（判据链 `data == {}` 恰配 + 边界单测覆盖 data 非 dict 透传）+ 启发式：契约 Q2 甲案只定义 `data:{}` 一种薄载荷，`data:null` 非契约形态；validate_display 拒绝降级是 Q5 冻结的兜底路径，行为安全 | 不动；如真机出现 `data:null` 变体样本，随包扩判据（一行）+ 单测 |

**结构闸结果**：final r2 九闸全绿（结构闸 PASS/FAIL 不复述为 finding）；结构闸之外，author 五轴语义 review 段完整执行（见下）。零 BLOCKING、零 hard-violation。

## Five-Axis Review（author 语义审查）

1. **正确性**：截获展开 = `week_grid_payload` 单源同参复用（A2 首齿逐字断言，防两套网格计算漂移）；「恰薄载荷」边界七样本透传 + 脏 B1 ValueError 大声传播（P2-A2 先例）；wire 形状 json.dumps/loads 往返保真 + 展开产物过真实 validate_display 机械门；v6 相对 v5 diff 界面逐行钉（兄弟段逐字保留）；真机侧 AC2 意图链两轮目击与 offline 判据互证。已知边界 = Findings 第 3 条（data:null 形态）+ 未覆盖面 7。
2. **边界**：非薄载荷一律原样走 relay 链交机械门裁决（贪截获被边界齿钉死——mutation 实测贪截获变体齿红）；脏 MD 沿 ValueError 传播不静默产空网格；front 接线失败不阻断建连/服务。
3. **契约**：契约面零触碰（`docs/contracts/**` 全生命周期零命中）；薄载荷/意图信号化消费 K1 契约门 HUMAN_APPROVED 冻结形状；v5 历史版本 sha256 机制齿 + diff 界面齿双保险（M3K3-E2 铁律落地）；schedule 表行改述薄载荷语义、旧 {title,items} 即废口径与 Q5 一致。
4. **安全**：`securityReviewRequired=false`；展开载荷全由云端 schedule.py 单源机械计算（非 LLM 生成，注入面收敛）；v6 prompt 为仓内部署制品（非 secrets 面）；三面接线解析零版本字面（ARQ-260908 纪律）；无新增依赖。
5. **可运维性**：v6 演进史由版本文件序列自承载（v1..v6 可 diff，未来 bump 沿同一 diff 界面齿纪律）；ECS 部署已随设备轮前置完成且远端留 `.bak-260921-k3-v6` 备份（progress.md 在案）；front 建连推送静默成功路径的观察面缺口已在 K1 部署面复核项（未覆盖面④）挂账；设备轮记录三源归因方法论（会话 id / 门数 / 文本逐字比对）沉淀在 device-round-record.md，可复用。

## Simplification Debt

`grep "minimal:"` 本包全部实现/测试文件**零命中**——无简化债务台账条目，全部 `no-trigger`。

## Residual Risk

- **decisionCoverage**：TaskPackage `decisionCoverage: []`（空）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 human 冒险接受项需列示。
- 残余风险实质 = 未覆盖面申报表 8 条（真机目击粒度边界 + 他包在册缺口/立项 + K1 部署面复核项转出），无新增高风险敞口：offline 14 齿行为级垫底 + 真机主链三源目击在案；③ marker 链路的部署面疑点（Findings SHOULD）有效果面兜底且已显式转出，不存在静默漂移路径。

## 合并门输入摘要

- 前置依赖：K1（merged @ 08:16Z）+ K2（merged @ 11:58Z）均已 merged，dependsOn 解锁。
- 回滚方式：单 commit revert 本包实现批即可整体回退（server 面零跨包耦合；ECS 侧回滚 = 恢复远端 `.bak-260921-k3-v6` 备份 + 重启 front/引擎，progress.md 部署段留有文件清单）。
- supervisor 判读输入：final r2 `ok:true` + evidence-check PASS + demo 对账一致（带挂账）+ K1/K2 deferred 按证据程度收口声明 + 8 条未覆盖面全部口径内挂账/转出 + findings 零 BLOCKING（1 SHOULD + 2 NIT）。
- 本包不判定通过、不 merge、不 push——supervisor-verdict-gate.mjs 机械闸归 planner 主线程，其后 pre-merge-escalation-gate 放行 → merge-driver --pod 本地合并。
