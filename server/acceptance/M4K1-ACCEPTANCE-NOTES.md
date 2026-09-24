# M4K1 冻结验收 · 设计说明 + 红基线 + 契约条目↔齿强度申报 + mutation 证据

> 承载包：`260924-m4-k1-story-cloud-link`（subsystem: server；workstream m4-star-notebook；mergeOrder 38；rigor full；riskTier medium）。作者 = acceptance-author（本 Pod）。日期 2026-09-24。
> **落盘责任人 = 本 Pod**（任务书：实测记录由 acceptance-author 本人落）。
> 冻结件 = `server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（26 齿：21 设计态红 + 5 现状绿）。
> 覆盖 ids：M4K1-A1 / A2 / A3 / A4 / A5 / A6 + E1（M4K1-A7 = structural deferred 归 M4-K3 设备轮，**不冻 live 齿**）。
> **写边界如实记**：acceptance-author 写边界 = `firmware/**/*.acceptance.c(pp)` + `firmware/**/acceptance/**` + `server/**/acceptance/**`，**不含 `delivery/taskpackages/**`**。故本说明落在写边界内的本目录；taskpackage.json 字段（`acceptanceTestsFile` / `redBaselineResult` / `mutationEvidence.perAcceptance`）交 planner 代落（见 §7）。

## 1. 齿形态与冻结契约（builder MUST implement）

三面增量（无 live 齿——A7 deferred）：

- **`server/gateway/story.py`（纯模块，离线可 import）**：`STORY_LIST_WINDOW=16`、`AUTO_STOP_SECONDS=120`、`is_finish_phrase(text)`、`list_stories` 窗口语义（items=最新 ≤16，total=全量现存篇数）、`story_text` additive `seq`、`archive_story` additive `seq` + 持久账本 `<stories_dir>/.seq.json`（原子写、单写者）、`delete_story(dir,id)->bool`（哨兵头校验 + 硬删 + 拒绝审计 + 幂等）、`StoryAccumulator.timed_out_sessions(now,timeout_s=None)`（复用既有 `now` seam，不改既有 V31 语义）。
- **`server/gateway/display_schema.py`（Q8 story 形状 floor）**：story_list `total` 在场须 int≥0、`items` 条目三键 `{id,title,date}` 齐；story_text `seq` 在场须 int。schedule floor / 白名单零扩（kidpal_story_delete 为 §四消息、非 display view）。
- **`server/gateway/front/front_app.py`（接线面，离线 fastapi-stub 可装载）**：`build_display_tool_calls` Q7 relay 收紧（story_* 引擎侧块降级）；`_push_story_list(websocket)` async（front 自推 story_list 单源，三时点调用）；上行泵 `pump_client_to_upstream` 拦截 `kidpal_session_ctx` + `kidpal_story_delete`（不下透引擎）；下行泵累积态 stt 命中收尾词 → stop 归档 + 下发正文，停录归档后追加 story_list 推送。

**跨端 wire 形状锚**（对照对端实际接受什么，非按本侧契约想象；brief ② 必读面）：`firmware/main/display/story_page.c:494 kidpal_story_list_parse`（total 必整数、items 超 `KSTORY_LIST_MAX=16` 整单 -1 降级）、`:645 kidpal_story_text_parse`（未知键含 seq 一律忽略 `:707-712` → seq additive 安全）、`:615 kidpal_story_counter`（优先 total、缺省回退 items.length）、`firmware/main/mcp_server.cc:752/:793`（解析失败显式降级不盲渲染）。

**齿强度差异（性质档）**：机制级 = Q8 floor / 落文锚；行为级 = 窗口/seq 账本/删除/拦截/收尾（真跑运行时）；源锚代理 = 契约落文锚 + 120s 前端接线 + 建连点接线 + `_push_story_list` 命名缝（逐条见 §6）。

## 2. 红基线（实测 2026-09-24，本 Pod 树）

- 命令：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance`（仓根）。
- 结果：**21 failed / 139 passed / 4 deselected**。
- 既有面零新增红：139 passed = 上游锚 **134 passed / 0 failed / 4 deselected @5e862d0**（`git diff --quiet 5e862d0 HEAD -- server/` 零差异，本树实测成立）+ 本包 5 条现状绿齿。
- 21 红全为**设计态红**（本包实现工件未落盘，见 §3 逐齿映射）；无一来自环境/依赖/测试写错。
- **冻结即冒烟**（DEV-GOVERNANCE §3）：`--collect-only` 收集 164 项、**零错误**（执行通道 error 为零；本包无 live 面故无 SSH 线束冒烟项）。§4（C 类实编）不适用（本包无 C 类验收件）。

## 3. 逐齿映射（齿 → acceptanceRef / 断言的真实结果 / 红名 ↔ 缺失行为 / 性质档）

| 齿（test 名） | coverage | 绑定的真实结果（不是「没报错」） | 红名 ↔ 缺失行为 | 性质档 |
|---|---|---|---|---|
| a1_story_list_new_shape_passes_floor | A1 | 真门 validate_display 放行冻结新形状 story_list | 现状绿（无 floor 故放行） | 行为级 |
| a1_story_text_seq_optional_passes_floor | A1 | story_text 带/缺 seq 均过真门 | 现状绿 | 行为级 |
| a1_story_list_total_must_be_nonneg_int | A1 | total 字符串/负数/浮点被真门拒绝 | 红 ↔ Q8 floor 未实现 | 行为级 |
| a1_story_list_items_three_keys_required | A1 | 缺键 item / 非对象 item 被拒绝 | 红 ↔ Q8 floor 未实现 | 行为级 |
| a1_story_text_seq_must_be_int_when_present | A1 | seq 字符串/浮点被拒绝 | 红 ↔ Q8 floor 未实现 | 行为级 |
| a1_relay_rejects_engine_side_story_blocks_q7 | A1 | build_display_tool_calls 真跑：story_* 块降级为友好 text（不透传载荷） | 红 ↔ Q7 relay 收紧未实现 | 行为级 |
| a1_relay_keeps_other_views_unchanged | A1 | 非 story view 合法块按既有语义中继（收紧不过宽） | 现状绿（must-stay） | 行为级 |
| a1_contract_carries_three_touchpoints | A1 | 契约正文含五处落文锚点串 | 现状绿（已落地） | 源锚代理 |
| a2_session_ctx_intercepted_not_forwarded | A2 | 上行泵不转发 session_ctx + routing=accumulate + 全链真归档 | 红 ↔ 现 pump 照原样转发、无拦截 | 行为级 |
| a2_source_page_values_and_backward_compat | A2 | 全取值拦截 + 其余取值不切累积态 + 省略消息向后兼容 | 红 ↔ 未拦截 | 行为级 |
| a2_idempotent_convergence_with_start_recording | A2 | session_ctx 与 start_recording 双入口收敛 + 重复触发单归档 + 内容不丢 | 红（step1 routing=chat）↔ session_ctx 未切累积态 | 行为级 |
| a3_list_stories_window_and_total_independent | A3 | 20 篇 -> items 恰 16（最新窗口）、total=20、item 三键、过真门、空目录语义 | 红（items=20）↔ 窗口未实现 | 行为级 |
| a3_push_seam_connect_and_wire_timepoints | A3 | `_push_story_list` 恰一帧 + 建连源锚 + 归档后 wire 追加 story_list（total=3） | 红（无该缝、零生产推送）↔ 三时点未接线 | 行为级 + 源锚代理 |
| a4_delete_executes_and_repushes | A4 | delete 拦截 + MD 硬删 + 重推对账 total−1 | 红 ↔ delete 未接线 | 行为级 |
| a4_delete_nonexistent_id_idempotent_repush | A4 | 不存在 id 不误删 + 仍重推（total 不变）= 隐式 ack | 红 ↔ 同上 | 行为级 |
| a4_delete_rejects_traversal_and_no_sentinel | A4 | 路径穿越/无哨兵头拒绝 + 目录外文件完好 + 审计留痕 + 畸形 id 不崩 | 红 ↔ 同上 | 行为级 |
| a5_finish_phrase_detection | A5 | is_finish_phrase 精确短语命中/不误判 | 红 ↔ 缺函数 | 行为级 |
| a5_finish_phrase_triggers_stop_in_accumulate_only | A5 | 累积态收尾词 -> 自动归档 + 下发正文；问答会话零影响 | 红（无归档）↔ 收尾检测缺失 | 行为级 |
| a5_timeout_120s_decision | A5 | AUTO_STOP_SECONDS==120；timed_out_sessions 边界（>=120 计 / <120 不计 / 非累积态排除） | 红（常数 None/无方法）↔ 纯决策缺失 | 行为级 |
| a5_front_wiring_references_timeout_decision | A5 | front 源码引用 timed_out_sessions | 红 ↔ 前端接线未落地 | 源锚代理 |
| a6_archive_assigns_persistent_monotonic_seq | A6 | 归档 seq=1,2,3 + `.seq.json` 落盘 + story_text 带 seq 过真门 | 红（seq=None）↔ 账本缺失 | 行为级 |
| a6_rearchive_same_id_keeps_seq | A6 | 同 id 重写 seq 不变、不涨计数 | 红 ↔ 账本缺失 | 行为级 |
| a6_restart_survival | A6 | 新实例续分配 seq=4（账本存活） | 红 ↔ 账本缺失 | 行为级 |
| a6_delete_does_not_rollback_seq | A6 | 存 3 删 1 -> total=2 且下一篇 seq=4 | 红 ↔ 账本缺失 | 行为级 |
| a6_ledger_missing_rebuilds_without_rollback | A6 | 删账本 -> 新篇 seq>3（不倒退）+ 告警留痕 | 红 ↔ 账本缺失 | 行为级 |
| e1_existing_story_semantics_zero_offset | E1 | archive/read/list/story_text/StoryAccumulator V31 四回退 + 有界 | 现状绿（must-stay） | 行为级 |

## 4. 逐齿 mutation 设计 + 实测输出（第一方原始输出）

### 4.1 设计态红齿的 counterEvidence

A1–A6 的 21 条设计态红齿，其**红名本人即 counterEvidence**（每条红断言直指缺失行为，见 §3「红名 ↔ 缺失行为」列）——无需另造变异即可证敏感。逐条 mutation 设计 + expectedRed = §7.3 `mutationEvidence.perAcceptance`。

### 4.2 现状绿齿（must-stay-green）的实测变异（apply → 还原，第一方输出）

变异以**运行期扰动已装载模块**实现（不落作者面实现改动），apply 后即还原。原始输出（2026-09-24，本树，`PYTHONIOENCODING=utf-8`）：

```
== T0 baseline: must-stay-green teeth ==
  test_a1_relay_keeps_other_views_unchanged                      GREEN
  test_e1_existing_story_semantics_zero_offset                   GREEN

== MUT-1  floor 过宽：story_list 一律拒绝 (patch validate_display) ==
  -> RED  | False is not true : M4K1-A1 RED: 冻结 story_list 新形状被真门拒绝 (mutated-overbroad-floor) …
  restore: GREEN

== MUT-2  floor 过宽：story_text 一律拒绝 (patch validate_display) ==
  -> RED  | False is not true : M4K1-A1 RED: story_text 带 seq 被真门拒绝 (mutated-overbroad-floor)。
  restore: GREEN

== MUT-3  Q7 过宽：relay 一律降级 (patch front.build_display_tool_calls) ==
  -> RED  | {'text': '降级'} != {'text': '你好呀'}
  restore: GREEN

== MUT-4  契约锚回退：CONTRACT 指向缺锚点文件 ==
  -> RED  | '现存篇数 = 星星计数' not found in '空契约\n' : M4K1-A1 源锚代理 RED: …
  restore: GREEN

== MUT-6  既有语义偏移：story_text 恒塞 seq (patch st.story_text) ==
  -> RED  | Items in the first set but not the second: …
  restore: GREEN

== MUT-7  E1 既有语义偏移：归档空转写不再拒绝 (patch archive_story) ==
  -> RED  | ValueError not raised : M4K1-E1 RED: 空转写未 ValueError（空故事不得入册）。
  restore: GREEN
```

- MUT-1/2/3 证 Q7/Q8 齿**双向敏感**：过松（未实现 = 设计态红）与过宽（误杀 = 变异红）都被判红 → 非 vacuous。
- MUT-6/7 证 E1 零偏移守卫齿对着既有 story.py 契约面敏感。
- **MUT-5（幂等方向，post-wiring 变异）待 builder 转绿后重放**：symmetrically break `StoryAccumulator.start`（重复 start 清空累积缓冲）应使 `test_a2_idempotent_convergence_with_start_recording` 转红——现状树该齿已因 session_ctx 未接线而红（step1 routing=chat），该变异与现状红不可区分，故显式声明**待 builder 转绿后重放**。
- 其余实现级变异（§7.3）同属**待 builder 转绿后重放**（现状设计态红 = 变异与缺失行为同源不可区分；verifier 用同命令组重放 RED→GREEN）。

## 5. 缺口与歧义（交 verifier/planner；不自裁）

1. **120s 前端计时循环接线 offline 不可验**：只冻纯决策（`timed_out_sessions`）+ 源锚代理（front 引用该决策）。行为等价假设「front 轮询每 tick 调该决策并对命中会话 stop + 下发」显式化为未验；live 全链归 M4-K3（M4K1-A7）。
2. **建连时点（三时点①）**：真 ws 建连面 offline 不跑——`_push_story_list` 行为齿直接调该缝 + 源锚代理（ws_gateway 引用）。行为等价假设「ws_gateway 调 `_push_story_list` ≙ 建连即推」显式化。
3. **Q7「引擎侧来源」判据**：offline 只钉 relay 路径（build_display_tool_calls）对 story_* 降级，不断言真实引擎帧来源标记。行为等价假设「relay 路径 = 引擎侧来源面」显式化（front 自推走 `_push_story_list`、不经 relay 天然豁免）。
4. **floor 深度 = 形状 floor**：不钉逐字段深度（item 额外键放行与否、id 非空、date 格式）——契约未定义处，留实现自由。
5. **删除审计格式**：断言拒绝路径日志记录含 offending id 或拒绝类 token；审计正文格式不钉。
6. **账本文件名 `.seq.json`** = 机制级冻结（plan 取舍点 B 甲案）；账本内部 JSON 结构不钉（行为级断言 seq 语义）。
7. **窗口双层钉**：producer（`list_stories`）+ wire（pump 帧）。若 builder 仅在 front 层窗口化、producer 仍返全量 → producer 齿红（frozen seam 已明示）。
8. **`delete_story` 返回 bool**（True=真删 / False=no-op 或拒绝）为冻结签名；拒绝路径的「是否重推」未在契约明定 → 齿不断言拒绝路径的重推行为。

## 6. 契约条目 ↔ 齿 强度申报（acceptance-evidence T2 体例；落点亦见冻结件头注）

| 契约 / 设计条目 | 承载齿 | 性质档 |
|---|---|---|
| §1.3③ total=现存篇数 + items ≤16 窗口（Q2 丙案） | a3_list_stories_window_and_total_independent（producer）+ a3_push_seam…（wire 帧） | 行为级（双层） |
| §1.3④ data.seq 可选整数（只增不减） | a6_*（账本真跑）+ a1_story_text_seq_must_be_int_when_present（真门） | 行为级 / 机制级 |
| §四 kidpal_story_delete（单向 + 隐式 ack + 硬删 + 边界仅 story MD） | a4_delete_executes_and_repushes / a4_delete_nonexistent… / a4_delete_rejects_traversal… | 行为级 |
| §四 kidpal_session_ctx（已 LANDED 零改：整条拦截 + story 切累积态） | a2_session_ctx_intercepted_not_forwarded / a2_source_page… / a2_idempotent_convergence… | 行为级 |
| §1.5-3 Q7 relay 收紧（story 单源） | a1_relay_rejects_engine_side_story_blocks_q7（+ 不过宽守卫 a1_relay_keeps_other_views_unchanged） | 行为级 |
| §1.5-3 Q8 story 形状 floor | a1_story_list_total_must_be_nonneg_int / a1_story_list_items_three_keys_required / a1_story_text_seq_must_be_int_when_present（+ 正样本 a1_*_passes_floor） | 行为级 |
| §1.3③④ / §1.6 story 段落文（契约门落地） | a1_contract_carries_three_touchpoints | 源锚代理 |
| arch §5.2④ 三时点推送（行为策略，不进契约） | a3_push_seam…（时点②③ = pump wire；时点① = 命名缝 + 源锚代理） | 行为级 + 源锚代理 |
| requirements §2.1 120s 录音态超时 | a5_timeout_120s_decision（纯决策）+ a5_front_wiring…（源锚代理） | 行为级 + 源锚代理 |
| spec S-F2.4 AC2 流水号只增不减 | a6_restart_survival / a6_delete_does_not_rollback_seq / a6_ledger_missing_rebuilds… | 行为级 |

**弱于契约处（逐条，显式）**：见 §5 的 1/2/3/4/5/8——均以「行为等价假设显式化 + 缺口声明」处置，未以绿信号掩盖。**行为等价假设清单**：① 「relay 路径 = 引擎侧来源面」；② 「ws_gateway 调 `_push_story_list` ≙ 建连即推」；③ 「front 每 tick 调 `timed_out_sessions` 并 stop ≙ 120s 自动收尾」。

## 7. 交 planner 落地件（超出 acceptance-author 机器写边界，请代落）

### 7.1 `acceptanceTestsFile[]`

```json
["server/acceptance/test_m4k1_story_cloud_link.acceptance.py"]
```

### 7.2 `redBaselineCommand` / `redBaselineResult`

命令（逐字，仓根）：

```
python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
```

`redBaselineResult`（逐字）：

```
21 failed / 139 passed / 4 deselected —— 2026-09-24 acceptance-author 实跑（仓根，命令逐字
`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance`）。21 红全为
设计态红（本包实现工件未落盘：story.py 无窗口/seq 账本/delete_story/is_finish_phrase/timed_out_sessions；
display_schema 无 story floor；build_display_tool_calls 未收紧 story_*；front 泵未拦截 session_ctx/
kidpal_story_delete 且无 _push_story_list、停录只推 story_text），逐齿映射见
server/acceptance/M4K1-ACCEPTANCE-NOTES.md §3。既有面零新增红 = 139 passed = 上游锚 134 passed /
0 failed / 4 deselected @5e862d0（`git diff --quiet 5e862d0 HEAD -- server/` 零差异）+ 本包 5 条
现状绿齿（must-stay-green：Q8 正样本 ×2 + Q7 不过宽守卫 + 契约落文锚 + E1 零偏移守卫）。
收集零错误（--collect-only 164 项、冻结即冒烟 §3 过）。A7 live 面不冻（structural deferred 归 M4-K3）。
```

### 7.3 `mutationEvidence.perAcceptance`（JSON-ready；`acceptanceId` = taskpackage `acceptance[]` 原文逐字）

```json
（JSON 全文见 §8；`acceptanceId` = taskpackage `acceptance[]` 原文逐字，故须从 taskpackage.json 取原串。）
```

### 7.4 `coveredAcceptance[]`

```json
[
  "M4K1-A1", "M4K1-A2", "M4K1-A3", "M4K1-A4", "M4K1-A5", "M4K1-A6"
]
```

（另含 E1 零偏移守卫齿，E1 属 exitCriteria 非 acceptance id；M4K1-A7 不冻齿 = structural deferred。）

### 7.5 `qualityNotes`

- 数据路径用真实落盘（tempdir + 真 `archive_story` / 真 `delete_story` / 真 `.seq.json` 账本读写），非 mock；front 面用 **真实 front_app 模块**（fastapi stub 离线装载，g-relay-fix 先例）+ 真实 pump 协程 + I/O 双桩（设备/引擎腿），断言到达设备的 mcp 帧的 **wire 形状**（`{"view":str,"data":json 串}` 解回 JSON 逐字段），非「调用了就算过」。
- 错误/边界/权限路径齐备：空转写拒绝、路径穿越、无哨兵头文件、畸形 id、不存在 id 幂等、total/seq 类型非法、item 缺键、120s 边界（119/120/130s）、offset 超界、问答会话零影响负锚、向后兼容负锚。
- 跨端 wire 形状对照对端解析器源码钉（story_page.c/.h 的 int total / ≤16 cap / 未知键忽略），不按本侧契约想象。
- 未越界写实现：变异以运行期扰动已装载模块实现（apply→还原），作者不改任何实现文件。
- 契约门三触点落文由源锚代理齿把守（pass-oracle 第 4 关无脚本兜底，ARQ-260827-contract-mech-gate-gap 在册），非行为本体，已自陈代理性质。

### 7.6 强度申报表

见 §6（映射 + 缺口 + 行为等价假设三件齐）。

### 7.7 清单外阅读申报（brief ④ 原样回传）

| 清单外文件 | 原因（一句话） |
|---|---|
| `docs/DEV-GOVERNANCE.md`（「## 验收测试纪律」§1–§6） | 角色定义要求起草前必读：live 两态 / 冻结即冒烟 / C 类实编 / 退役目录 / tighten-only / 起包格式前置。 |
| `.claude/references/acceptance-evidence.md` | 强度申报体例（T2）+ evidence 类扫描齿 root 注入纪律的执行依据。 |
| `.claude/references/test-quality-antipatterns.md` | 判「什么算真断言」的共享反模式表（避免 source-grep 空过）。 |
| `delivery/taskpackages/260924-m4-k1-story-cloud-link/plan.md` | 派发 prompt 指定：取舍裁定 + 铁律（零偏移负锚）。 |
| `delivery/taskpackages/260924-m4-k1-story-cloud-link/launch-card.md` | 派发 prompt 指定：launch 基线锚 + 契约已先行落地 intake 特别注意。 |
| `delivery/taskpackages/260924-m4-k1-story-cloud-link/architecture-gate-evidence.md` | 派发 prompt 指定：§4 十问裁定 + §6 条件表（C1 = Q7/Q8 机械执行面）。 |
| `delivery/taskpackages/260924-m4-k1-story-cloud-link/CONTRACT-CHANGE-PACKET.md` | 派发 prompt 指定：§5 裁定逐字权威（Q2 丙案 / Q5 / Q7 / Q8 等）。 |
| `delivery/workstreams/m4-star-notebook/roadmap.md#260924-m4-k1-story-cloud-link` | 派发 prompt 指定：回指 AC 号集反查验收方向。 |
| `server/acceptance/conftest.py` | 确认 live 项机械 skip 闸语义（本文件无 live 面）。 |
| `server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md` | 派发 prompt 指定的 notes 先例形态（落盘责任人制式）。 |
| `firmware/main/display/story_page.h` | brief ② 对端 `story_page.c` 的结构头（`KSTORY_LIST_MAX=16` / total 缺省语义）= 跨端 wire 判据源。 |
| `firmware/main/display/story_page.c`（:245-395 / :1050-1260 段） | brief ② 对端文件本身的其余段（`stamp_index=counter+1` / 渲染计数）——核对旧板端过渡窗口。 |
| `server/acceptance/archive/test_g_relay_fix.acceptance.py` | **扩先例面（超 brief ③ 封顶 3 个）**：front_app 离线 fastapi-stub 装载 + pump I/O 双桩的冻结先例，本包 front 齿装机形态直接沿用。 |
| `server/acceptance/archive/test_story_wiring.acceptance.py` | **扩先例面（超 brief ③ 封顶）**：story 接线齿先例，确认既有 must-stay 面（云侧 stop/feed/story_text 接线）。 |
| `server/pytest.ini` | 收集通路配置（`norecursedirs`/archive、markers），确认本文件收集零错误。 |
| `.claude/scripts/mutation-check.mjs` | 核 `mutationEvidence` 字段形态（`acceptanceId` raw 全 id + `sensitive` 信号），保证 JSON-ready。 |
| `server/tests/`（目录列举） | 界定 134 passed 上游锚构成（server/tests 面既有文件）。 |

（planner 据本表落一条 `over-scan` friction 记账——纯 advisory，非门。）

## 8. §7.3 `mutationEvidence.perAcceptance` 全文（JSON）

```json
{
  "perAcceptance": [
    {
      "acceptanceId": "M4K1-A1: 契约门齿——CONTRACT-CHANGE-PACKET v2（本包目录，四分类预审 2026-09-24 NEEDS_REVISION→v2 修订并入：分类拆标 ①漂移/②a②b 拆分/③独立变更）经 human 契约门 2026-09-24 裁十问（HUMAN_APPROVED，裁定记录 = CCP §5；Q2 改判丙案、余九问按拟答）（含预审漏问点 1–7：16-cap 上限口径 / seq 缺省降级 / 删除可靠性隐式 ack / 硬删软删与连带边界 / 双源窗口 relay 收紧 / display_schema story floor / §1.6 story 段）后，契约 diff 按裁定定稿随本包同 PR 落地；gate evidence 按 .claude/references/contract-change-evidence.md 十节骨架随包收口（CDQ 三行 + ADR 追加 + 契约变更记录行 + 人工 diff 核对清单 = CCP §1 钉行表四处 + Q2/Q5/Q7/Q8/Q9 落文处）",
      "sensitive": true,
      "mutation": "移除 display_schema 的 story_list / story_text 形状 floor（Q8）+ 移除 build_display_tool_calls 对 story_* 的降级过滤（Q7）",
      "expectedRed": "test_a1_story_list_total_must_be_nonneg_int / test_a1_story_list_items_three_keys_required / test_a1_story_text_seq_must_be_int_when_present / test_a1_relay_rejects_engine_side_story_blocks_q7",
      "note": "设计态即红（floor/收紧未实现）= counterEvidence；过宽方向（floor 误杀新形状 / 收紧误杀非 story view）由 must-stay-green 齿实测已证敏感：MUT-1 story_list 全拒 -> test_a1_story_list_new_shape_passes_floor 转红；MUT-2 story_text 全拒 -> test_a1_story_text_seq_optional_passes_floor 转红；MUT-3 relay 全降级 -> test_a1_relay_keeps_other_views_unchanged 转红（均 apply 后还原复绿）。"
    },
    {
      "acceptanceId": "M4K1-A2: session_ctx 拦截齿——front 上行泵拦截 kidpal_session_ctx（整条消息不下透引擎、无论取值）：source_page=story → 该会话切累积态，与 control/start_recording 收敛同一幂等入口（重复触发不重复开会话）；home/schedule 取值现版本无行为（闭集预留）；省略整条消息 = 普通会话（向后兼容负锚）；引擎不感知（下行透传面零变化）",
      "sensitive": true,
      "mutation": "移除 pump_client_to_upstream 对 kidpal_session_ctx 的拦截（整条恢复透传引擎）+ 移除 source_page=story 切累积态",
      "expectedRed": "test_a2_session_ctx_intercepted_not_forwarded / test_a2_source_page_values_and_backward_compat / test_a2_idempotent_convergence_with_start_recording",
      "note": "设计态红（未拦截 = 现行 pump 照原样转发）即 counterEvidence；幂等方向（重复触发清空累积缓冲）MUT-5 属 post-wiring 变异——**待 builder 转绿后重放**（session_ctx 接线后令重复 start cancel+start 应使 test_a2_idempotent_convergence_with_start_recording 转红）。"
    },
    {
      "acceptanceId": "M4K1-A3: story_list 推送齿——list_stories() 接线（零生产调用点 → front 推送 producer）：WS 建连 + 归档成功后 + 删除执行后三时点下发 story_list；**items 上限 = 契约门 Q2 丙案定稿（2026-09-24）：恒推最新 16 条窗口（日期倒序）+ total 恒全量现存篇数**（板端 KSTORY_LIST_MAX=16 维持现值；警示句 items.length 不顶替 total load-bearing）；幂等不重复、畸形不推；推送触发时机 = 行为策略（arch §5.2④ 扩展承载，不进契约——Q10 认可）",
      "sensitive": true,
      "mutation": "list_stories 返回全量（去 STORY_LIST_WINDOW 窗口）+ 移除三时点 story_list 推送（_push_story_list / 归档后 / 删除后）",
      "expectedRed": "test_a3_list_stories_window_and_total_independent / test_a3_push_seam_connect_and_wire_timepoints",
      "note": "设计态红（现树 list_stories 无窗口、零 story_list 生产推送点）即 counterEvidence；窗口在 producer（list_stories）+ wire（pump 帧）双层钉。"
    },
    {
      "acceptanceId": "M4K1-A4: 删除执行齿——kidpal_story_delete 消费：story.py delete_story（哨兵头校验 kidpal-story:1 + <id>.md 删除 + 不存在 id 幂等 no-op 不报错——隐式 ack 重发承接面，Q5 裁定形态）+ 流水号账本不变（删除不回退 seq——S-F2.4 AC2）+ 删后 story_list 重推对账（计数 −1 可见 = 隐式 ack 载体）；非法/越权 id（路径穿越、无哨兵头文件）拒绝并审计日志留痕；硬删/软删与审计范围按契约门 Q6 裁定",
      "sensitive": true,
      "mutation": "移除 delete_story 的哨兵头 / 路径穿越校验（任意 id 直删）+ 移除删除后 story_list 重推对账",
      "expectedRed": "test_a4_delete_executes_and_repushes / test_a4_delete_nonexistent_id_idempotent_repush / test_a4_delete_rejects_traversal_and_no_sentinel",
      "note": "设计态红（kidpal_story_delete 未接线、delete_story 不在位）即 counterEvidence；拒绝路径审计留痕由 test_a4_delete_rejects_traversal_and_no_sentinel 钉。"
    },
    {
      "acceptanceId": "M4K1-A5: 收尾与超时齿——累积态收尾词机械检测（stt 文本扫描「我说完了」/「讲完啦」→ stop 归档，仅累积态消费、问答会话零影响）+ 120s 无语音自动收尾归档（front 侧计时、走正常盖章流程不丢弃——requirements §2.1 录音态超时口径）+ BOOT 丢弃路径（设备 listen-stop 前 cancel 语义 / 「讲错啦」V31 回退既有）零偏移核对",
      "sensitive": true,
      "mutation": "AUTO_STOP_SECONDS 改 999 / 移除 is_finish_phrase / 移除累积态 stt 命中收尾词触发 stop 归档",
      "expectedRed": "test_a5_finish_phrase_detection / test_a5_finish_phrase_triggers_stop_in_accumulate_only / test_a5_timeout_120s_decision / test_a5_front_wiring_references_timeout_decision",
      "note": "设计态红（is_finish_phrase / timed_out_sessions / AUTO_STOP_SECONDS 均不在位）即 counterEvidence；**120s 前端计时循环接线 offline 不可验**（仅源锚代理证 front 引用该决策），行为等价假设显式化、live 归 M4-K3。"
    },
    {
      "acceptanceId": "M4K1-A6: 流水号账本齿——入册流水号持久账本（归档分配 seq=max+1、删除不回退、front 重启存活——stories 目录账本文件形态归 plan.md 取舍点）+ story_text.data.seq 下发（契约冻结形状，additive）+ story_list.data.total=现存篇数口径分离（两数各自独立断言：存 3 删 1 → total=2 且下一篇 seq=4）",
      "sensitive": true,
      "mutation": "移除 seq 持久账本（每次从现存 MD 计数分配 seq，不回写 .seq.json）",
      "expectedRed": "test_a6_archive_assigns_persistent_monotonic_seq / test_a6_rearchive_same_id_keeps_seq / test_a6_restart_survival / test_a6_delete_does_not_rollback_seq / test_a6_ledger_missing_rebuilds_without_rollback",
      "note": "设计态红（archive_story 现无 seq、无账本）即 counterEvidence；删除回退方向由 test_a6_delete_does_not_rollback_seq 直接钉（存 3 删 1 -> total=2 且下一篇 seq=4）。"
    }
  ]
}
```

---

**REVISION LOG**：v1（2026-09-24 初版冻结，acceptance-author 本人落盘）。
sha256（冻结件 `server/acceptance/test_m4k1_story_cloud_link.acceptance.py`）= `61c0501742eda85c50a6c67b0ea399a12c8b273da56daee778e89eb3eb49cc26`。