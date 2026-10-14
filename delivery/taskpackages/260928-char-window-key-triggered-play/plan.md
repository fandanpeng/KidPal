---
changeId: 260928-char-window-key-triggered-play
rigor: lite
riskTier: low
sourceType: human-ad-hoc
reviewed-by: architect（/triage-change 分流审，2026-09-28）
date: 2026-09-28
---

# 查字窗 KEY 起播：静态整字初始态 + 演示 KEY 触发 + 演完定格（v1.59 实现包）

> triage 裁决（architect 2026-09-28，human 触点①确认 Lite + A2 标注项取定格文案）：② 单包 Lite，riskTier low。主线程按 F7 约定逐字搬运 architect reviewed plan，未改内容。

## 目标效果（对回 spec v1.59，requirements.md 已落稿 commit 546832c）

1. **S-F3.1 AC1**：查字窗弹出即米字格静态显示完整示范字（整字实心黑），右栏速查栏同屏；未按 KEY 前零逐笔演示。
2. **S-F3.1 AC5 / S-F3.3 AC1**：KEY 短按 = 首次起播逐笔演示；再按 = 从头重播；演示独立于 TTS 播报（播报 v1.55 口径零改）。
3. **S-F3.1 AC3 / S-F3.3 AC2**：演完定格「写完啦·共 N 笔」等按键，不自动循环（human 2026-09-26「把节奏交给孩子」）。
4. **WF-F3-01 键帽随态变注**：查字窗左槽键帽未起播「播报」、起播后「重播」；右槽 BOOT「结束」。
5. §2.3 :146 键位矩阵查字窗行路由已在世（KEY CLICK→GLOBAL_ACT_CHAR_REPLAY / KEY 长按关窗 / BOOT 结束），**零路由改动**。

## 技术方案（逐文件，按 2026-09-28 实读代码写）

### A. firmware/main/display/char_page.cc（呈现面主改）

A1 **建窗静态整字**：show() 内 :570 `charpage_render_frame(-1, 0)` 改 `charpage_render_frame(-1, g_page.n_strokes)`——cur_stroke=-1 + written=n 时 `kidpal_char_stroke_state` 判每笔 WRITTEN（char_stroke.c:527-539 既有相位复用，可行性证据属实）。此时 n_strokes = strokes 数组数（:671 的 strokeCount JSON 覆写发生在其后，不参与初始渲染——「整字实心」按数组全笔判）。

A2 **初始提示条定格态**（human 触点①裁定：用定格文案）：删除 :737 `charpage_refresh_hint(0);`（及其「首拍=第 1 笔」注释）。hint_label 在 :732 已从 `g_page.card.progress` 落字，而 card 在 :685 以 cur_stroke=-1 装配 → progress = 「写完啦·共 N 笔」（card_build 单源，char_stroke.c:582-586 既有定格文案分支）。**零新文案、零重抄**。

A3 **建窗即停表**：:740-744 timer 创建保留原位原句（**字面 `g_page.timer = lv_timer_create` 是 char_window_overlay.acceptance.c MCF-A1 区域齿的末端锚，不得移走**——这是「show 建表即 pause」而非「replay 建表」的判定原因），创建后紧跟 `lv_timer_pause(g_page.timer);`。started 经 :524 memset 归 0。

A4 **演完定格闸断**：timer_cb（:361-382）在 phase==REST（返回 1）渲染定格帧 + 刷新定格文案后 `lv_timer_pause(timer)` 并 return（不推进 elapsed_ms）。纯逻辑合约的取模回绕在装配层闸断——**char_stroke.c 零触碰**。

A5 **replay() 语义扩展**（:499-503）：守卫（panel==NULL || timer==NULL 直接返回）；持 LVGL 锁（`lvgl_port_lock(30000)`，lcd_display.cc:1464 同款先例；replay 经 board 文件 app.Schedule 在主任务跑、当前无锁）；锁内 elapsed_ms=0 + started=1 + `lv_timer_resume`；锁外调 `kidpal_lcd_keybar_sync()`（extern "C" 局部声明，kidpal_render_node.cc:141 同款先例；内部递归取锁安全）刷新键帽「播报→重播」。首播与重播同路径（elapsed 归零 + resume），started 仅服务键帽态。

A6 **started 外露 + 建窗刷键帽**：新增 `int kidpal_char_page_started(void)`（char_page.h 同步声明，extern "C" 块内）；show() 末尾调 `kidpal_lcd_keybar_sync()`（此时 CHAR_WINDOW 层已由 begin_char_window 推入，sync 的查字窗分支可正确裁决；新查字重开窗经 memset 归 0 → 键帽回「播报」）。

A7 头注 :13 「演完歇 2s 重来」等动画描述更新为 v1.59 口径（KEY 起播/重播、演完定格不循环、建窗静态整字）；保留 char-window 锚字面（M2K3-A6 齿）。

### B. firmware/main/display/keybar.{c,h}（纯逻辑值域补齐）

新增并行函数（**不动 `kidpal_keybar_slots` 既有签名与值域**——additive，既有齿不破）：

```c
/* 查字窗键帽槽（v1.59 WF-F3-01 键帽随态变）：未起播「播报」/ 已起播「重播」；
 * 右槽 BOOT「结束」（与覆盖层非提醒右槽一致）。 */
int kidpal_keybar_char_slots(int started, const char **out_left, const char **out_right);
/* NULL out 防御 -1；started!=0 → (重播, 结束)，否则 (播报, 结束)。 */
```

### C. firmware/main/display/lcd_display.cc（消费接线，最小面）

`kidpal_keybar_sync_locked()`（:1344，**签名不动**——voice_round_ui.acceptance.c:930 钉字面）内：`overlay_top != 0 && kidpal_render_node_front_layer() == KIDPAL_LAYER_CHAR_WINDOW` 时改走 `kidpal_keybar_char_slots(kidpal_char_page_started(), ...)`，否则既有 `kidpal_keybar_slots` 路径原样。需补 include char_page.h / kidpal_layer.h（builder 核既有 include 面）。注意 apply_look :1474-1475 先直写 look.keycap 再 :1480 sync——sync 分支在后覆盖，净效果正确（TTS 播报中键帽也显示「播报/重播」而非「打断」，符合 AC5 演示独立于播报 + KEY 路由本就不打断播报）。提醒弹窗压查字窗时 front=REMINDER，不进本分支，提醒键帽不受扰。

### D. 零改动核证面（verifier 复核清单）

char_stroke.c（FROZEN 纯逻辑）、char_window.{h,c}（5 分钟 idle 计时）、kidpal_render_node.cc（层栈/tick）、board 文件（KEY 路由）、mcp_server.cc（view=char 路由）、application.cc（音频门）、protocols/server/云端、docs/**。

## acceptance[] 种子（A10：测试可断言）

- ACC-1（S-F3.1-AC1 前半）：建窗首帧 = `charpage_render_frame(-1, g_page.n_strokes)`（结构齿：字面在世）+ 真机目检整字实心。
- ACC-2（S-F3.1-AC1 后半）：建窗零动画拍——timer 创建即 pause；`charpage_refresh_hint(0)` 零在世（「第 1 笔」文案不得建窗出现）。
- ACC-3（S-F3.3-AC1）：replay = elapsed 归零 + started 置位 + timer resume（首播/重播同路径；结构齿）。
- ACC-4（S-F3.1-AC3）：timer_cb REST 相位 pause（结构齿：phase REST 分支含 lv_timer_pause）+ 真机演完 2s+ 无自动重播。
- ACC-5（键帽随态）：`kidpal_keybar_char_slots` 行为齿（host 纯逻辑）：(0)→(播报,结束)、(1)→(重播,结束)、NULL 防御 -1。
- ACC-6（接线）：lcd_display sync 查字窗分支 + char_page show/replay 调 kidpal_lcd_keybar_sync（结构齿）。

## exitCriteria[] 种子（structural / suiteGate 豁免通道）

- firmware-build 设备树编译过（char_page.cc / lcd_display.cc / keybar.c 改动面）。
- host 验收全量绿（机械门全量不放松；主线程临时跑可按相关子集）。
- 真机轮（human 持板）：静态整字 → KEY 起播 → 演示中 KEY 重播 → 演完定格 → 键帽 播报→重播 随态 → TTS 播报口径不变 → 5 分钟无操作关窗仍工作。
- spec-anchor-check 无新漂移（spec 面 v1.59 已落稿，本包不动 docs）。

## impactedTests 种子（逐件处置）

| 测试件 | 处置 |
|---|---|
| char_window_overlay.acceptance.c | **锚点存活核验**（MCF-A1 区域齿末端锚 `g_page.timer = lv_timer_create` 须留在 show() 内 hint_label 创建之后——A3 方案即为此设计；MCD-A2 hint pos / M2K3-A5/A6 头注齿复核）；**新增 v1.59 齿**（A1/A2/A4/A6 对应结构锚）。既有齿预期绿。 |
| char_stroke.acceptance.c | char_stroke.c 零触碰，预期绿；复核无漂移（本方案核心取舍 = 定格在装配层闸断、不触冻结面）。 |
| keybar_three_slot.acceptance.c | `kidpal_keybar_slots` 既有齿 additive 不破（新函数并行）；**新增** kidpal_keybar_char_slots 行为齿（ACC-5）。 |
| key_layer_dispatch.acceptance.c | KEY 路由零改动，预期绿。 |
| voice_round_ui.acceptance.c | :930 钉 `static void kidpal_keybar_sync_locked(void)` 签名字面——签名不动即绿；内部分支 additive。 |
| schedule_grid.acceptance.c | :1083 rn_keybar_sync_locked 调用点计数不动即绿。 |

## changeManifest（260928 批 T4 字段，仓根相对）

- firmware/main/display/char_page.cc
- firmware/main/display/char_page.h
- firmware/main/display/keybar.c
- firmware/main/display/keybar.h
- firmware/main/display/lcd_display.cc
- firmware/main/test/char_window_overlay.acceptance.c
- firmware/main/test/keybar_three_slot.acceptance.c

（acceptance-author 若另立新验收文件，planner 经回评扩 manifest。）

## riskTier / gateProfile / budget 建议

- riskTier: **low**；rigor: **lite**；sourceType: **human-ad-hoc**（上游权威 = requirements.md v1.59 + spine；260926 arch PROPOSAL 已 superseded-by-intake 不作 origin）。
- gateProfile: 标准 dev-loop Lite（acceptance-author → builder → verifier↔fixer → integrator → supervisor 机械闸：firmware-build + host 全量）。
- budget: 单轮预期收；fixBudget 2。真机轮 human 持板。

## 关键取舍（供 planner intake / decisionCoverage 参考）

1. REST 定格实现层：装配层 pause（char_page.cc）vs 改纯逻辑合约（char_stroke.c）——取前者，零触冻结面、测试面零重钉。
2. timer 生命周期：show 建表即 pause（保 MCF-A1 锚）vs replay 建表——取前者。
3. 初始提示条文案：定格文案单源（human 触点①裁定采纳）vs 新引导文案（需触冻结面）——取前者，human 已裁定。
4. 键帽值域住 keybar.c 新并行函数 vs 扩 kidpal_keybar_slots 签名——取前者（additive，既有齿与调用面零破）。

## 红线自查（architect 结论存档）

**契约面（逃生门 reviewer-first 判据）**：**否**。核证面：D3 笔顺 JSON 消费（kidpal_char_stroke_parse / medians_parse）不动；E1 view=char 响应块路由（mcp_server.cc :724-735）不动；TTS 链路（application.cc 音频门 / protocols / server / 云端）不动；无新端→网关消息。char_stroke.c 的 FROZEN 是 dev 侧冻结验收件（firmware/main/test/ 钉），非 docs/contracts——且本方案刻意零触碰它。无契约门、无 Track A 升级。

**spec↔impl 对账（v1.59 各 AC 投影全查）**：AC1 静态初始态 → A1+A2+A3 ✓；AC2 三态 → 演示期 render 零改 ✓；AC3 定格不重播 → A4 ✓；AC4 播报 v1.55 不变 → D 面零改 ✓；AC5 KEY 起播/重播独立播报 → A5 + 路由已在世 ✓；S-F3.3 AC1/AC2 → A5 + card_build 定格文案既有 ✓；S-F3.2 五行 → 不动 ✓；§2.3 :146 矩阵 → 路由零改 ✓；键帽随态 → B+C ✓；NFR-PERF-2 终点重锚「窗弹出、示范字可见」→ 静态初始态天然满足，无实现动作 ✓。

**5 分钟兜底与「未起播也计时」**：实核 kidpal_render_node.cc :917-926 + char_window.c——idle 计时只认「CHAR_WINDOW 前台 + 任意键按下重计」，与演示状态零耦合；未起播也计时正是 spec「驻留到按键或 5 分钟无操作自动关窗」本义，无冲突。KEY 起播按键本身经 key_hit 重计 idle，亦无冲突。

**关键文件路径**：char_page.cc（:570 初始帧 / :737 首拍 hint / :740 timer / :499 replay / :361 timer_cb）；char_stroke.c（:501-525 anim_state 取模回绕 = 定格须装配层闸断的根因）；keybar.c / lcd_display.cc（:1344 sync_locked / :2478 kidpal_lcd_keybar_sync 公共桥）；char_window_overlay.acceptance.c（:1505-1506 timer 字面锚 = 方案形状约束）。
