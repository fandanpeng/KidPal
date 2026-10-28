---
id: 261028-p3-record-session-mirror-desync-fix
title: P3 记录会话镜像与 ctx 双滞后修复（故事列表页卡死、长按 KEY 回不了首页）
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect
date: 2026-10-28
status: pending
---

# plan · P3 记录会话镜像与 ctx 双滞后修复（261028-p3-record-session-mirror-desync-fix）

> 来源：/triage-change 2026-10-27 真机 blocker 取证，architect 比例化 review 判 **② 单包 Full**（riskTier=high，sourceType=regression）。四面探测触面 = 1（纯实现面），不建 spine；逃生门预查 = 干净（零契约/spec/agent/gate 触点，零 wire 帧变化）。
> human 触点① 已确认（2026-10-28）：档位 ②Full+high 确认；修复范围 M1+M2+M3 全修；被拒会话语义 = 静默放弃、再按一次。
> **judge 判注（诚实申报，快路径不适用原因）**：修法落点 `application.cc` / `kidpal_render_node.cc` / `mcp_server.cc` 均不进 host 构建，host 验收断言只能覆盖 `story_page.c` 纯逻辑与结构锚；运行时正确性必须由真机轮承载。本包**不适用** lite 快路径，按 full 环境备。

## human 意图（触发链）

human 原话（2026-10-27）：「查一下现在在 story list 页面卡住了，长按key 也不回首页」。设备停在 P3 故事列表，长按 KEY 只打日志不换页，只能重启恢复。

**按键口径修正**（architect 复核）：能产生 `GLOBAL_ACT_HOME` 的只有 **KEY 长按**（requirements §2.3 矩阵 P3 列表行「KEY 长按 = 回首页」）。板级 BOOT 长按回调只把手势交 `HandleStoryKey`、返回值丢弃、从不发全局动作；录音态 BOOT 长按 = 丢弃。真机验收务必把「回首页」钉成 KEY 长按，否则按 BOOT 长按在基层本就是空分支，会把修好误判成没修好。

## 根因（KPLOG 实证逐环比对，两处独立泄漏叠满）

拦截点 = `kidpal_render_node.cc:590` `kidpal_render_node_show_home()` 守卫，判据 `rn_record_session_live_locked()`（同文件 :1289）：

```c
return (kidpal_story_session_ctx_active() == 1 &&
        (page == KSTORY_PAGE_RECORDING || page == KSTORY_PAGE_ORGANIZING)) ? 1 : 0;
```

`page` = 渲染节点镜像 `s_story_session_page`（:160），生产写入点仅两处：`mcp_server.cc:810`（VAD enter 写 RECORDING）、`mcp_server.cc:914`（`KidpalStorySessionEndApply` 写回 LIST）。

**泄漏 A（镜像不复位）**：输入函数 `kidpal_story_page_input`（`story_page.c:1915-1931`）在**返回动作码之前**就把 `v->page` 置 LIST（停录 :1919 / 丢弃 :1927）。随后 Idle 分支（`application.cc:1242-1248`）调 `kidpal_story_session_end(KidpalStoryViewPageState(), &out)`——该冻结判据只在 `page == RECORDING` 时返 1，此刻已是 LIST → 返 0 → `KidpalStorySessionEndApply()` 不执行 → `kidpal_render_node_story_session_page(LIST)` 永不落地，镜像恒停 RECORDING(2)。

**泄漏 B（ctx 置位后无清位沿）**：`mcp_server.cc:606`（`KSPV2_ACT_REC_SESSION`）与 `mcp_server.cc:1450`（`control/start_recording` 触发词入口）**两个**置位点都是「先无条件 `kidpal_story_session_ctx_set(1)`，再调 void 的 `StartListening()`」。`application.cc:1058-1061` 在 `auto_connect_in_flight_` 为真时直接 return（日志实证 `AutoConnect in-flight, ignore manual start listening`）；ctx 清位全仓单点只有 `application.cc:1272`（进入 Idle 的状态变更沿），设备本就已在 idle、状态机再未迁出 → ctx 永久停在 1。

**真机日志实证**（2026-10-27 21:34-21:37，TF 卡 KPLOG）：
- 21:34:36 `story_vad_recording_enter consumed=1 page=2`（VAD 自动进录音态，镜像写 2）
- 21:34:40 BOOT 点击 → `story_key discard_rec cancel_count=1` → WS 断开 → `StateMachine: listening -> idle`；此后**零条** `story_session_end` 行
- 21:34:45 `story_key rec_session_slot page=0` → `AutoConnect in-flight, ignore manual start listening`；此后**零条** `StateMachine` 行
- 21:35:04 起渲染诊断行恒为 `PAGE_RENDER ... sview=0 ssess=2`（权威页态 LIST vs 镜像 RECORDING 分叉）
- 21:36:31 / 21:36:38 / 21:36:58 / 21:37:14 四次 `key_route_dispatch action=2(home)` 紧跟 `W kidpal_rn: show_home suppressed: record session live (page=2 ...)`

## 死同类（同形态路径清查，方案必须一并收口）

根因不是「某条路径忘了复位」，而是**两个缓存载体的写入边与真实状态边不同源**：真实真相只有 `g_story_view.page`（视图权威）与设备状态机。凡「Idle 沿之前已把页改走」的收尾都会漏镜像复位；凡「置 ctx 后聆听没真起来」都会漏 ctx 清位。

1. **KEY 停录保存（STOP_RECORD）**：同泄漏 A 形态；因 ctx 同时被清当前不显 blocker，但下一轮 `REC_SESSION` 会在 LIST 上先触发假 live。
2. **整理收尾（`VRT_CTRL_START_ORGANIZING`，`mcp_server.cc:1460-1485`）**：页切 ORGANIZING 后才 StopListening → Idle 沿 `session_end(ORGANIZING)=0`（冻结 [R2] 只认 RECORDING）→ 镜像仍停 RECORDING；随后盖章→列表全程不经 enter/apply，镜像脏到下一次会话。更隐蔽。
3. **服务端 `end_session`/`stop_speaking` 在 ORGANIZING 页态收尾**：同第 2 条（在 RECORDING 页态则由 [R2] 接住，这条干净）。
4. **ctx 第二入口 `control/start_recording`**：同泄漏 B。
5. **同一漏型的第二爆点（本次未触发）**：`kidpal_render_node_voice_end()`（`kidpal_render_node.cc:689-695`）在「本地页 = P3 或记录会话在世」时用**陈旧镜像**解析页态并**写回节点缓存** `s_story_view.page = page_state`——镜像脏 RECORDING 时会把丢弃后的 LIST **复活成 RECORDING**。本次 VOICE 层已先弹空走了 early return 所以没炸。
6. 阅读态 / 删除子态 / 正文取不回：不在 live 判据集合，不自成 blocker，但镜像脏 + ctx=1 时同样会被假 live 抑制首页重建。
7. `KSPV2_ACT_INTERRUPT_GUIDE`：不是同型（子态复位就地完成，页态保持 RECORDING 是正确语义）。
8. 服务端 `story_session_end` 下发路径：不存在——`kidpal_story_session_end` 是本地纯逻辑缝，无 wire 帧。

**结论**：必须按「镜像 = 权威视图页态」的不变量修，**不得**按「给每条收尾路径补一次复位」修，否则会继续漏第 2、3 条。

## 五档裁决与 rationale（② 单包 Full，riskTier = high）

- **非 ⓪ nano**：命中排除清单两项（触状态机语义、跨模块三 TU）；目标符号爆炸半径远超阈值（`kidpal_render_node_cache_story` 生产调用点 6 处跨两 TU；`HandleStartListeningEvent` 是多入口中枢；`rn_record_session_live_locked` 三个消费点全在首页重建/前台回落主链）；且需新行为验收语义承载（镜像跟随权威的不变量 + 收尾沿次序），「无验收语义净增」不成立。
- **非 ③ 拆包**：单一缺陷、单一切片（记录会话生命周期一致性），无需多 Pod / 迁移。
- **② Full 而非 Lite**：判据表 Full 行含「触状态机」；且 §7 lite 判注义务不成立（测试面不可闭环，见文首判注）。
- **riskTier = high**：真机 blocker（需重启恢复）、改的是会话生命周期 × 设备状态机沿、失败模式为设备级卡死而非功能降级。
- **新机制批判定：非新机制批**——不引入新子系统、新外部依赖或数据迁移，只是给两个既有缓存载体补写入纪律 + 一个状态沿的次序修正。
- **单向守卫**：riskTier=high + rigor=full，无 riskTier-overrides-rigor 冲突。

## 修法（主选：重建不变量，M1+M2+M3）

总不变量：**I1 镜像恒等于权威视图页态**；**I2 ctx 的生命周期 = 聆听会话真的起来了**。

**M1（I1，单点派生）— `kidpal_render_node.cc` `kidpal_render_node_cache_story()`（:1181-1193）**

```c
if (v != NULL) {
    memcpy(&s_story_view, v, sizeof(s_story_view));
    s_story_session_page = v->page;   /* 镜像跟随权威视图页态 */
}
```

依据：`g_story_view.page` 的每一次变更最终都经 `cache_story` 落节点缓存（逐点核过：`ScheduleStoryRender` :115、VAD enter :811、SessionEndApply :915、盖章 cache-only :199、story_text :1705），镜像从此不可能独立漂移。保留原 `kidpal_render_node_story_session_page()` 桥与两处显式调用（幂等冗余，冻结 seam [R5] 文字不动；[R5] 明写「机制 builder 择一」，本改法在其允许面内）。已知无害边界：删除覆盖层渲染闭包不走 cache_story，镜像停 LIST（DELETE 不在 live 集合）。

**M2（I1，收尾沿次序确定）— `application.cc` Idle 分支（:1242-1248 块之后、`kidpal_keybar_scene_reset`/`voice_end` 之前）**

```c
KidpalStorySessionEndSync();   /* 镜像 := 权威视图页态（收尾沿确定性同步） */
```

配套：`mcp_server.cc` 在 `KidpalStorySessionEndApply` 旁（:900-918 区）定义

```cpp
void KidpalStorySessionEndSync() { kidpal_render_node_story_session_page(g_story_view.page); }
```

`application.cc` 顶部加 extern 声明（沿用既有「不进 mcp_server.h」纪律）。

理由：主循环 **STATE_CHANGED 先于 SCHEDULE**，收尾当轮节点缓存可能仍是旧值；显式同步保证 `voice_end` 的 [R5] 页态解析取真值、杜绝「页态复活」。**硬约束：此处不得调 `cache_story`**——它会把 `s_story_data_received` 置 1，破坏 AC6 冷启动空态。

**M3（I2）— `application.cc` `HandleStartListeningEvent()`（:1039-1093）**

在三个「聆听没真起来」的早退边缘各加 `kidpal_story_session_ctx_set(0);`：`WifiConfiguring` 分支（:1045-1049）、`!protocol_`（:1051-1054）、`auto_connect_in_flight_`（:1058-1061）。`Activating` 分支落到 Idle 已有清位，不加。安全性：State==Listening/Speaking 的正常沿不经过这些分支；能在这三个边缘被拒时必然没有正在进行的记录会话，清位不误伤。覆盖两个置位入口（:606 与 :1450）。

**备选与明确不选**

- B2 不选：删镜像、让判据直读 `s_story_view.page`——最彻底，但消解 [R5] 的「分叉」前提与 RSR-A7 判别位叙事，文书与冻结面成本高于收益。
- B3 不选：扩 [R2] 让 ORGANIZING 也返 1——改冻结 seam 语义、与 RSR-A3 行为齿冲突，且整理收尾本不应把页拉回 LIST；该形态由 M1 覆盖。

**明确不动**：`story_page_input` 的「先改页再返码」顺序（`story_page_v2` 冻结矩阵钉死 `page==LIST`）；STOP/DISCARD 两 case 的 `StopListening`/`SendKidpalStoryDiscard`/`CloseAudioChannel` 接线（RSL-A5 齿）。

## changeManifest（仓根相对）

```
firmware/main/display/kidpal_render_node.cc        # M1: kidpal_render_node_cache_story()
firmware/main/application.cc                       # M2 调用点 + extern; M3: HandleStartListeningEvent()
firmware/main/mcp_server.cc                        # M2: 新增 KidpalStorySessionEndSync()（仅此一符）
firmware/main/test/record_session_lifecycle.acceptance.c   # 新验收件（不动既有冻结件）
firmware/main/test/host_app/main/CMakeLists.txt    # 登记新件（builder FIRST ACTION）
```

零新增公共头、零设备侧新文件。

## 验收要点

**必须保持绿（回归网）**：`record_session_repair.acceptance.c`（RSR-A3 [R2] 语义、RSR-A7 [R5]）、`record_session_repair.test.c`、`record_dialogue_guide.acceptance.c`（RDG-F4 ctx 顺序锚 / RDG-F5）、`record_session_live.acceptance.c`（RSL-A5 正负锚）、`story_page_v2.acceptance.c`（v1.47 输入表）、`chrome_ghost.acceptance.c`（CG-A1 收尾沿 scene 复位位置）、`record_dialogue_v7_fix.test.c`（show_home/voice_end 守卫在位）、`key_layer_dispatch.acceptance.c`；host runner 全量零新增红。

**新增断言种子**（建议 `record_session_lifecycle.acceptance.c`，结构锚为主，运行时半 host 不可达须如实申报）：

1. `kidpal_render_node_cache_story` 区内出现镜像写入（`s_story_session_page = v->page` 或等价 setter 调用）+ 负锚：镜像赋值不得散落在 cache_story 与 setter 定义之外；
2. `application.cc` Idle 分支窗口切片：`kidpal_story_session_end(` 之后、`kidpal_render_node_voice_end(` 之前出现 `KidpalStorySessionEndSync(`（顺序比较，RDG-F4 切片先例）；
3. `HandleStartListeningEvent` 窗口切片：`auto_connect_in_flight_` 早退分支与 `!protocol_` 早退分支内含 `kidpal_story_session_ctx_set(0)`；
4. 冻结面 canary：`kidpal_story_session_end` 对 LIST/READER/DELETE/STAMP 仍返 0、仅 RECORDING 返 1（RSR-A3 复跑，must-stay-green）。

注意：CG 的 scene 复位锚钉在 Idle 分支「同区」，M2 插行不得插在 `kidpal_keybar_scene_reset` 与其判据之间；RDG-F4 窗口在 stt 回调附近，不受影响。

**真机轮（唯一运行时验证面，必做）**：

- **T1 复现路径**：VAD 进录音 → BOOT 丢弃 → auto-connect 期按 KEY（被拒）→ **KEY 长按必须回首页**；日志不得再有 `show_home suppressed`，`PAGE_RENDER` 的 `sview`/`ssess` 不再分叉。
- **T2 保存路径**：KEY 停录 → 归档/盖章 → KEY 长按回首页正常；`ssess` 在 LIST 态应为 0。
- **T3 整理收尾**：服务端 start_organizing → 盖章 → 列表，收尾后长按回首页可用。
- **T4 负回归**：普通语音轮仍不进故事录音态（ctx 未被误置）；录音期间服务端 home 推送仍被抑制（既有演出保护不得丢）。
- **T5 连续两轮记录会话**：第二轮 `REC_SESSION` 后不假 live，丢弃后不锁死。

## 影响面与风险

- **影响面**：首页重建守卫（show_home）、前台回落（voice_begin/voice_end）、录音演出渲染、stt 旁路喂入、VAD 自动进录音、聆听形态切换（ctx 的全部消费者）；均只作用于记录会话语境，普通语音轮应在 M3 下零变化。

1. **R1**：M1 使 [R5] 两源归一——判据仍被消费、语义退化为恒取缓存；实现与证据里须**如实申报**这条退化，不得写成「[R5] 判据增强」。
2. **R2**：M2 插行改变 Idle 区顺序，CG/RDG/RSR 的窗口切片类结构齿可能误红，实现时逐个齿窗口核对。
3. **R3**：M3 清位后「被拒的 REC_SESSION」不再留 ctx，但 UI 侧已做的 `echo_reset` + `show_chat`（VOICE 层前推）不回滚——用户看到「按了没反应、语音层在前台」，再按一次即可。这是本包**接受的行为边界**（human 2026-10-28 裁定：静默放弃、不重试、不排 pending）。
4. **R4**：跨任务写布局不变（键盘回调写 `g_story_view.page`、主任务写镜像），沿用既有 32 位标量宽松度，不新增锁。

## 不做清单（out of scope）

- 不扩 `docs/contracts/**`、不动 wire 帧、不改 `requirements.md` / `architecture.md`（本次是恢复既有规定行为，非修改）；
- 不碰 `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md` 那批在册缺陷（键帽鬼影 / 鬼覆盖层 / 引导节奏 / 素材过滤 / 排序标题）；
- 不改 `story_page_input` 冻结决策表（v1.47 矩阵）；
- 不删镜像、不改 [R2]/[R5] 冻结 seam 的文字语义；
- 不做「auto-connect 后自动补发会话发起」（human 已裁静默放弃）。

## 冻结齿映射（intake 终版回填，acceptance-author 冻结件承载）

- **RSL2-A1** = 新增断言 1（M1 镜像跟随权威，含负锚）；
- **RSL2-A2** = 新增断言 2（M2 Idle 收尾沿次序）；
- **RSL2-A3** = 新增断言 3（M3 三处早退边缘 ctx 清位）；
- **RSL2-A4** = 新增断言 4 + 回归网八族（must-stay-green）；
- **RSL2-A5** = 真机轮 T1-T5（live-manual，structural 口径不入 runner 通过面）。
