# Docs Delta · 260924-m4-k3-record-session-live

> integrator 收拢制品（2026-09-30）。本件回答「本包文档面义务是否已全清」，并承载 spec 面（`docs/architecture.md`）的 **patch 形态 delta**（integrator scope-map allow 不含 `docs/architecture.md`，delta 在此成文、合并后由 human spec 批逐字过目落地——K1 docs-delta §4 五 hunk + defer docs-delta §3 supersession 双先例同族）。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/requirements.md#S-F2.2, docs/requirements.md#§2.1, docs/architecture.md#§5.2, docs/architecture.md#§5.5.5]`；`contractRefs = [interface-contract.md#§四, #§1.4]`；`docsOwner = integrator`。

## 1. 结论

- **契约面：本批零触碰**（铁律 E2）。消费契约 §四 `kidpal_session_ctx` LANDED 现文（板端发送半 = 本包落实现，wire 形状逐字契约冻结版）与 §1.4 `start_recording` 既有；`contract-lint` 闸绿（scoped r1 + integrator 六闸复跑均 exit 0）。**契约 §1.5 括注漂移（story_list 时点枚举陈旧——「建连/归档/删除后」未记 defer 摘除①）维持登记、处置走契约门四分类（architect-reviewer + human 契约门），不在包内自改契约**。
- **需求面（`docs/requirements.md`）：零改**。S-F2.2 / §2.1 / §2.3 行为权威冻结在前（v1.26–v1.61），本包只消费；F2 对账销账半已归 K2（K2 docs-delta §4.2–§4.6 待批 delta 承载）。
- **架构面（`docs/architecture.md`）：本批不可写 → patch 形态 delta 成文本件 §4（八处 hunk + 一处交接申报），合并后由 human spec 批落地**。落地前 gate evidence 保持 pending 语义注记（K1/K2 先例）。
- **K1/K2/defer 历史制品：不改写**（铁律）。defer 包 docs-delta §4.1 替换句中「①恢复时机归 M4-K3 重裁」挂账由本包 §4 H2 **supersede**（①已恢复落地，落地措辞以本件 H2 为准）；defer 件本体逐字未动。K2 件 §4.8 H7（§5.5.4 改名对账收口）仍待批有效，本包 §4 H3 为其**后继追加行**（K3 跨端演出半），不替换 K2 文本。
- **ADR 面：零改**。本包 = 契约 LANDED 消息的实现落地 + prompt 版本化管线既有机制的 v8 常规 bump（ADR-001 接线不变式零改、守卫零版本字面断言已由 a8 齿承载），无新架构决策。
- **workstream 面：本相位不写**。`delivery/workstreams/m4-star-notebook/progress.md` K3 段落账属真机轮后（E3），planner 派单明示本相位不写；milestones.jsonl M4 行 state 收口提议同理（本包只备证据不直改 state）。
- **closeout 制品**：`delivery/changes/dev/260924-m4-k3-record-session-live/`（本件 + `review-packet.md`）integrator 收拢落盘。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（`contractRefs#§四/#§1.4`） | **零触碰**（§四 LANDED 现文只读消费；§1.5 括注漂移维持登记走契约门） | 只读 |
| spec 面·需求 | `docs/requirements.md` | 零触碰 | 只读 |
| spec 面·架构 | `docs/architecture.md` | **本批不可写 → patch 形态 delta 本件 §4**（§5.2 证据行 + §5.2④ 场景 + §5.5.4 K3 追加行 + §5.5.5 v8 + to-be ⑥ 销账 + 三处连带 v7→v8 指针） | delta（待批） |
| defer 历史制品 | `delivery/changes/dev/260928-story-connect-push-defer/docs-delta.md` | **不改写**；①挂账 supersession 注记落本件 §4 H2（defer 件逐字未动） | 只读 + 注记 |
| K1/K2 历史制品 | `delivery/changes/dev/260924-m4-k1-story-cloud-link/`、`.../260924-m4-k2-p3-star-notebook-ui/` | 零触碰（K2 §4.8 H7 待批有效，本包 H3 为其后继追加） | 只读 |
| ADR | `docs/ADR.md` | 零改（无新决策） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | **本相位不写**（K3 段落账属真机轮后 E3） | 不适用 |
| closeout 制品 | `delivery/changes/dev/260924-m4-k3-record-session-live/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = scoped 相位 `docs-sync` 闸：TaskPackage 有 `docRefs` 且 `docsSyncRequired=true` → 走 `docsSyncReason` 分支。**scoped r1 闸 exit 0**（`verifier-evidence/rounds/r1.json`）+ integrator 六闸复跑（taskpackage-required / change-manifest / acceptance-coverage / mutation-check / docs-sync / security）**全部真实退出码 0**（2026-09-30）。
- 语义层核对：`docsSyncReason` 声明的 arch 收口动作（§5.2④ live 实证态 / §5.5.4 记录会话演出收口 / §5.5.5 prompt 现役版 / to-be ⑥ 销账注记）= 本件 §4 的 H1/H3/H4/H5，一一对应，**无声明未做的项、无做而未声明的项**（H6–H8 为 §4.5 所列连带指针面，防落地后残留新漂移；§4.9 为交接申报非本包实义改动）。
- **快照陈旧如实注记**：`docsSyncReason` 原文「arch 正文现写『现状 = v4』为双重漂移（正文 v4→v8 + 补 v5/v6/v7 沿革）」系拆包期（2026-09-29）假设——260925-prompt-v7-round-routing 批已把 §5.5.5 刷新至 v7 且沿革链 v1–v7 完整在文（本 worktree 现文 `:297` 实读）。**delta 实做 = v7→v8 + 沿革链追加 v8 条目**（v5/v6/v7 无需补录，已在文）；「双重漂移」前提已由该批消解，本注记销账。

## 4. spec 面 patch 形态 delta（本批不可写 `docs/architecture.md`；合并后由 human spec 批逐字过目 → 落地）

> **结构性事实前置（如实）**：integrator scope-map allow = `["firmware/**","server/**","docs/systems/**","docs/templates/**","docs/ARCH-REVIEW-QUEUE.md","docs/crq/**","docs/ADR.md","delivery/changes/dev/**","delivery/taskpackages/**","delivery/workstreams/**"]`——不含 `docs/architecture.md`（含已废弃 `docs/systems/**` 的不一致面已在 K1 review-packet Findings #1 登记，归 loop-retro/human）。
> **基线**：`docs/architecture.md` 现行文本 = 本 worktree 工作树态（2026-09-30，已含 K1 C4 spec 批 c735062 与 K2 spec 批落地后文本；行号以该态计）。落地方式沿用 K2 件注记：工作树实态若为 CRLF，`git apply` 本环境不可用（K1 C4 批实测先例），用 **node 字节级直写 apply + 落地前副本先验**。
> **落地顺序**：H3 需在 K2 件 §4.8 H7 落地**之后**追加（或同批按 K2 先、K3 后顺序 apply）；其余 hunk 独立。

### 4.1 delta 汇总表

| # | 位置（现行行号） | 动作 | 归属 |
|---|---|---|---|
| H1 | §5.2 证据状态行 `:205` | ④ 子句改写（offline 全绿 + live 确认轮待跑诚实措辞） | docsSyncReason「§5.2④ live 实证态」 |
| H2 | §5.2 场景④ `:213` | 行内①恢复子句重写（ctx 沿条件推送落地 + 板端 gating；defer 挂账 supersede） | docsSyncReason「§5.2④」+ defer 承接 |
| H3 | §5.5.4 `:290` 后追加一行 | K3 跨端演出半新行（K2 H7 后继追加，不改 K2 文本） | docsSyncReason「§5.5.4 记录会话演出收口」K3 半 |
| H4 | §5.5.5 `:297` | 现状 v7→v8 + 沿革链追加 v8 条目 | docsSyncReason「§5.5.5 prompt 现役版收口」 |
| H5 | §9 to-be `:382` | ⑥ 行内销账注记（session_ctx 发送/拦截 + 故事删除已落地；音量调节仍在办） | docsSyncReason「to-be ⑥ 销账注记」 |
| H6 | §5.1.2 OpenClaw 本体行 `:196` | 连带：`system prompt 版本化 v7` → v8（指针面，防 H4 落地后残留新漂移） | 连带现状行 |
| H7 | §3 NFR-SAFE-1 行 `:150` | 连带：`system prompt v7 简洁不啰嗦纪律` → v8 零偏移继承措辞 | 连带现状行 |
| H8 | §8.1 债 7 `:358` | 连带：闭合句追加 v8 刷新注记（债 7 历史闭合状态不变） | docsSyncReason「视情更新 §8.1 债 7」 |

### 4.2 H1 · §5.2 证据状态行（`:205`）

上下文：`:204` = 「> ← 收编自旧 §2.6…」引导行本体；`:206` 空行。本 hunk 仅替换 `:205` 单行内 ④ 子句，①②③⑤ 逐字不动。

```diff
@@ -205,1 +205,1 @@  §5.2 证据状态行
-> ← 收编自旧 §2.6 一条扩为五条。证据状态（缺口详见 §8.1）：① 开机常显 live 实证；② 语音问答 live 全通；③ 查字 live 已复验恢复（2026-09-15 真机实证——板端两缺陷定位见 §8 风险债 5）；④ 记录归档 host 验收过、live 面部分在办；⑤ 提醒 live L3 投递绿、L4 渲染回执留红。
+> ← 收编自旧 §2.6 一条扩为五条。证据状态（缺口详见 §8.1）：① 开机常显 live 实证；② 语音问答 live 全通；③ 查字 live 已复验恢复（2026-09-15 真机实证——板端两缺陷定位见 §8 风险债 5）；④ 记录归档 offline 全绿（host 136T0F + server 203P0F + device build + mutation 7/7 转红还原，M4-K3 2026-09-30）——live 端到端确认轮待跑（human 持板 LP4，设备轮时点 human 裁量）；⑤ 提醒 live L3 投递绿、L4 渲染回执留红。
```

### 4.3 H2 · §5.2 场景④（`:213`）行内①恢复子句重写

本 hunk 只动 `:213` 行内「**story_list = front 机械 producer 推送（现役两时点）**（…）」括注内的①恢复子句，其余全文（触发词/收尾三式/账本/删除链）逐字不动。**supersede 申报**：defer 包 docs-delta §4.1 替换句中「原时点①的恢复时机〔设备进 P3 语境推送 / RAM 缓存填充〕归 M4-K3 `kidpal_session_ctx` 设备半落地后重裁」——该挂账已由本包落地消解，human spec 批落地时 ①措辞**以本 hunk 新文为准**（defer 件本体不改写）。

```diff
@@ -213,1 +213,1 @@  §5.2 场景④（行内子句替换，其余不动）
-**story_list = front 机械 producer 推送（现役两时点）**（归档成功后 / 删除执行后；「WS 建连即推」已摘除——260928-story-connect-push-defer：真机拓扑下设备每会话重开 WS，建连即推会在每次按 KEY 时给设备打 story_list 帧、旧固件无条件渲染 P3 页盖掉聆听态〔story_page 零 CJK → 半截页〕；原时点①的恢复时机〔设备进 P3 语境推送 / RAM 缓存填充〕归 M4-K3 `kidpal_session_ctx` 设备半落地后重裁；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；
+**story_list = front 机械 producer 推送（现役：①ctx 沿条件推送 + ②归档成功后 + ③删除执行后）**（①= front 收 `kidpal_session_ctx(source_page=story)` 首帧切累积态沿（routing != "accumulate"）恰推一帧——M4-K3 2026-09-30 落地，即 defer 包摘除「WS 建连即推」后原时点①的重裁恢复形态〔设备每会话重开 WS 的盖页缺陷根除：推送绑定记录语境会话建立而非建连；板端记录语境前台（VOICE 层/录音态）只装 `g_story_*` 缓存绝不推前台 = `kidpal_story_list_gate` gating，defer 真机缺陷形态的板端防护半〕；重复 ctx / ctx(home/schedule/未知) / 无 ctx 普通会话零帧；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；
```

### 4.4 H3 · §5.5.4 `:290` 后追加 K3 跨端演出半新行

**前置**：本 hunk 在 K2 件 §4.8 H7（改名对账【修复项】销账收口）落地**之后**追加（K2 件亦为 patch 待批态——同批 apply 时先 K2 后 K3；K2 H7 落地前本行追加于现 `:290` 行之后亦可，两行并存无冲突，K2 H7 只重写 `:290` 原行、不动本追加行）。`-` 侧无（纯插入行）。

```diff
@@ -290,0 +291,1 @@  §5.5.4 UI 演出与现状 · 记录会话演出（K2 H7 后追加一行）
+- **记录会话演出·跨端半（M4-K3 落地，2026-09-30）**：引导态 = P3 按 KEY 进会话推语音层前台 + 气泡逐字「需要我帮你记点什么？」（`kidpal_story_guide_bubble` 单源，lcd_display.cc apply_look 区覆写消费——M1 冻结映射 `kidpal_voice_states` 零触碰）；开口第一句即切录音态（UTT_START 事件沿纯逻辑决策 `kidpal_story_recording_enter`，AFE VAD 真沿归 live 轮目击）；录音态话语逐句回显区（`kidpal_story_echo_*` FIFO ≤4 句，stt 旁路喂入、AI 不逐句回复）；story_list 帧记录语境前台只装缓存不推前台（`kidpal_story_list_gate`，防盖引导/录音演出）；收尾 = KEY 再按 listen-stop 归档面 / BOOT 关通道丢弃面（acc.cancel 弹缓冲）双路径不混；新冻结验收面 = `record_session_live.acceptance.c` + `test_m4k3_record_session_live.acceptance.py`；live 目击项（双入口真判定/逐字节 wire/演出冲突）待真机确认轮。
```

### 4.5 H4 · §5.5.5（`:297`）现状 v7→v8 + 沿革追加

```diff
@@ -297,1 +297,1 @@  §5.5.5 OpenClaw system prompt 版本化
-声明件 `OPENCLAW_SYSTEM_PROMPT_CARRIER`（无扩展名）显式声明当前接线版 + **承载版本演进史唯一全量副本**（260925-prompt-v7-round-routing 起：声明面/渲染面散文只留「当前 = vN」指针，沿革不再四处复述）；**现状 = v7**（v1 基座 → v2 +课表 producer+char C1 → v3 +故事 producer+提醒并集 → v4 +F4 故事分工修正 → v5 +char meaning/禁附录 → v6 +M3-K3 课表问答口径 → v7 回合类型路由表 + 故事段对齐 2026-09-24 契约门禁产 story JSON + 语音「简洁不啰嗦」纪律 + **制品本体不带前言**〔沿革外置 carrier，渲染不剥离保「验收所测 = 生产所用」〕）。历史文件不改、共存可 diff。**接线不变式：渲染面（overlay PROMPT_SRC）== 声明面（provider.config 三键）== 被测面**（ADR-001：守卫从结构化指向解析当前版，禁版本字面断言）。
+声明件 `OPENCLAW_SYSTEM_PROMPT_CARRIER`（无扩展名）显式声明当前接线版 + **承载版本演进史唯一全量副本**（260925-prompt-v7-round-routing 起：声明面/渲染面散文只留「当前 = vN」指针，沿革不再四处复述）；**现状 = v8**（v1 基座 → v2 +课表 producer+char C1 → v3 +故事 producer+提醒并集 → v4 +F4 故事分工修正 → v5 +char meaning/禁附录 → v6 +M3-K3 课表问答口径 → v7 回合类型路由表 + 故事段对齐 2026-09-24 契约门禁产 story JSON + 语音「简洁不啰嗦」纪律 + **制品本体不带前言**〔沿革外置 carrier，渲染不剥离保「验收所测 = 生产所用」〕→ v8 +「帮我记下来」第二指令（S-F2.2 双指令批，260924-m4-k3-record-session-live）+ 意图边界负例「帮我记下来明天带跳绳」= 课表录入不判开录；影子口径/标题 ≤12 字零偏移继承 v7）。历史文件不改、共存可 diff（v7 本体 sha256 零回改铁律齿在档）。**接线不变式：渲染面（overlay PROMPT_SRC）== 声明面（provider.config 三键）== 被测面**（ADR-001：守卫从结构化指向解析当前版，禁版本字面断言）。
```

### 4.6 H5 · §9 to-be ⑥（`:382`）销账注记

```diff
@@ -382,1 +382,1 @@  §9 to-be ⑥
-**to-be（Phase 1 结构变更）**：① 键路由改造——dispatch 按 `layer_top` 分流 + `cycle_next` 拆「清层/进页」（BOOT 隔离、KEY 长按返回上一层、单双击消歧仅列表态）；② 60s/120s 双阈值 timer 生产者接线（会话状态机激活）；③ 思考态渲染接线；④ 撤 P4 常驻页 → 3 页循环 + `char-window` 覆盖窗；⑤ display 周网格数据（契约）；⑥ 新增 control 动作（故事删除/音量调节，随契约门）+ 端→网关 `kidpal_session_ctx` 发送/拦截（契约已定义；固件侧接线点 = application.cc 会话发起入口读 `kidpal_layer_top()`，front 侧 = 上行泵帧解析处拦截）；⑦ L4 渲染回执修复 + 查字 live 链路修复。
+**to-be（Phase 1 结构变更）**：① 键路由改造——dispatch 按 `layer_top` 分流 + `cycle_next` 拆「清层/进页」（BOOT 隔离、KEY 长按返回上一层、单双击消歧仅列表态）；② 60s/120s 双阈值 timer 生产者接线（会话状态机激活）；③ 思考态渲染接线；④ 撤 P4 常驻页 → 3 页循环 + `char-window` 覆盖窗；⑤ display 周网格数据（契约）；⑥ 新增 control 动作（故事删除/音量调节，随契约门）+ 端→网关 `kidpal_session_ctx` 发送/拦截（契约已定义；固件侧接线点 = application.cc 会话发起入口读 `kidpal_layer_top()`，front 侧 = 上行泵帧解析处拦截）——**⑥ 大半已销账**：故事删除动作（契约 §四 `kidpal_story_delete`，M4-K1 落地）；`kidpal_session_ctx` front 拦截半（M4-K1）+ 板端发送半（M4-K3 2026-09-30：protocol 层 `SendKidpalSessionCtx` 原语 + Listening 分支会话发起入口无条件发送 + source_page 闭集单点映射）；**仍留办**：音量调节动作（requirements v1.44，③契约门在办）；⑦ L4 渲染回执修复 + 查字 live 链路修复。
```

### 4.7 H6/H7/H8 · 连带指针面（`:196` / `:150` / `:358`，防 H4 落地后残留新漂移）

```diff
@@ -196,1 +196,1 @@  §5.1.2 OpenClaw 本体行（行内子句替换）
-| OpenClaw 本体 | ECS 部署（2026.8.1） | 完整 agent（意图/多轮/记忆/工具/QQ 录入）；system prompt 版本化 v7（§5.5.5）；原生 cron（提醒 15min 重算 + 录入即时触发） |
+| OpenClaw 本体 | ECS 部署（2026.8.1） | 完整 agent（意图/多轮/记忆/工具/QQ 录入）；system prompt 版本化 v8（§5.5.5）；原生 cron（提醒 15min 重算 + 录入即时触发） |
```

```diff
@@ -150,1 +150,1 @@  §3 NFR-SAFE-1 行（行内子句替换）
-| NFR-SAFE-1 内容安全 | system prompt v7 简洁不啰嗦纪律（回合路由表承载，不设硬性句数上限）+ front 兜底文案与 V32 截断（超限按有处理的失败模式） | §5.1 front 组件 / §5.5.5 prompt 版本化 |
+| NFR-SAFE-1 内容安全 | system prompt v8 简洁不啰嗦纪律（自 v7 零偏移继承，回合路由表承载，不设硬性句数上限）+ front 兜底文案与 V32 截断（超限按有处理的失败模式） | §5.1 front 组件 / §5.5.5 prompt 版本化 |
```

```diff
@@ -358,1 +358,1 @@  §8.1 风险债 7（闭合句追加注记，历史闭合状态不变）
-| 7 | `server/deploy/README.md` prompt 现状曾写 v3、arch 曾写 v4（两处散文漂移） | 部署指引误导 | 已随 260925-prompt-v7-round-routing 收敛：README prompt 节改指针式（沿革归 carrier 唯一副本），arch §5.1/§5.5.5 刷新 v7【已闭合】 | dev-loop |
+| 7 | `server/deploy/README.md` prompt 现状曾写 v3、arch 曾写 v4（两处散文漂移） | 部署指引误导 | 已随 260925-prompt-v7-round-routing 收敛：README prompt 节改指针式（沿革归 carrier 唯一副本），arch §5.1/§5.5.5 刷新 v7【已闭合】；2026-09-30 M4-K3 prompt v8 批同族指针面（README `:44`「现状 = v8」已随批实改；arch §5.1/§5.5.5 v8 刷新 = 本包 docs-delta H4/H6 待批）【已闭合·v8 续账注记】 | dev-loop |
```

### 4.8 交接申报（非本包 delta 实义，交 human spec 批处置）

- **§7「契约门在办」重复行**：现文 `:341`（K1 H5 新文「已销三留一」）与 `:342`（K1 H5 应替换的旧文「另登记四件随下次契约门裁…」）**并存重复**——疑为 K1 C4 spec 批（c735062）落地时插入未删旧行。处置建议：human spec 批落地本包 delta 时**删除 `:342` 旧行**（其内容已被 `:341` 完全覆盖且 `:342` 的④表述「随本批契约门落」为过时态）。本包不越权改动 spec 行内容，仅申报。
- **§7 在办 ③/④ 行内 K3 指针**：`:341` 行「④ …板端发送半归 M4-K3」在 M4-K3 落地后可随下次 spec 批补「（M4-K3 2026-09-30 已落地）」——非漂移（该行表述「归 M4-K3」仍真实），仅顺手项。

### 4.9 可 apply 性与边界（如实）

- 八处 hunk 的 `-` 侧原文逐字取自本 worktree `docs/architecture.md` 现行文本（行号 `:150`/`:196`/`:205`/`:213`/`:297`/`:358`/`:382`，读取时点 2026-09-30；H3 为纯插入行无 `-` 侧），单命中无歧义；互不重叠。
- H2 的 `-` 侧 = `:213` 现文中的①恢复子句段（该行全长逾千字，hunk 按子句段给出——落地时以「现役两时点」句首至「推送触发属行为策略（不进契约）；」句尾为唯一锚段做行内替换）。
- `docs/architecture.md` / `docs/requirements.md` / `docs/contracts/**` 本批零写入（`git status --porcelain docs/` 零输出）；defer/K1/K2 历史制品零改写。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（scoped r1 + 六闸复跑 exit 0） |
| 需求面零写入 | `git status --porcelain docs/requirements.md` 零输出 |
| 架构面未写（如实） | `git status --porcelain docs/architecture.md` 零输出；patch delta 成文本件 §4（待 human spec 批） |
| ADR 零改 | `git status --porcelain docs/ADR.md` 零输出；ADR-001 接线不变式由 a8 三面一致齿承载 |
| 历史制品不改写 | `git status --porcelain delivery/changes/dev/260924-m4-k1-story-cloud-link/ 260924-m4-k2-p3-star-notebook-ui/ 260928-story-connect-push-defer/` 零输出；supersession 注记落本件 §4.3 |
| workstream 不写 | planner 派单明示 K3 段落账属真机轮后（E3）；本相位 progress.md 零触碰 |

