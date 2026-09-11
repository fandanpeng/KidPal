# Review Packet · 260911-screen-unification（PKG-2 归一屏）

> closeout 综述（integrator 产出，2026-09-11）。subsystem=firmware，riskTier=high，
> rigor=full，mergeOrder=19，workstream 260911-display-interaction-sovereignty 核心包。
> 设计权威 = PROPOSAL FROZEN v1.0（OQ1=纯B / OQ3=c 已裁）+ review F6/F7/F8；
> deputy 代裁两条：SU-RULING-01（优先级语义）、SU-RULING-02（单物理屏）。

## Change Size

- Files changed（firmware 面，本包）：
  - 修改 11：`firmware/main/CMakeLists.txt`、`application.cc`、`boards/common/wifi_board.cc`、
    `boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`、`display/char_page.cc`、
    `display/kidpal_render_node.cc/.h`、`display/lcd_display.cc/.h`、`mcp_server.cc`、
    `test/host_app/main/CMakeLists.txt`
  - 新增 2 实现：`display/kidpal_layer.c/.h`
  - 新增 1 冻结验收 + 1 红基线摘要：`test/screen_unification.acceptance.c`、
    `test/acceptance/red-baseline/screen-unification-red-baseline-summary.md`
- Net diff（firmware/main 跟踪文件）：463 insertions / 249 deletions（不含新增文件）。
- Scope：纯 firmware 显示层结构/归属重构 + 设备装配接线；server/docs/契约零触。
- Split recommendation：无需再拆。本包是最小可交付核心单元；physical reparent 与
  PROVISION 接线是 SU-RULING-02/SU-A2 裁定的随包交付，非可独立拆出的增项。

## Verification Evidence

- **Structural gates（scoped 轮 r4 全绿，7 闸）**：taskpackage-required / firmware-build /
  contract-lint / acceptance-coverage / mutation-check / docs-sync / security 全 PASS。
- **firmware-build**：设备 IDF 真编译 exit 0，`xiaozhi.elf` → `xiaozhi.bin`（0x2c25c0，
  分区余 20%）；`display/kidpal_layer.c` 编入设备构建。
- **host 全量（仓库根复跑，2026-09-11 integrator 复跑）**：`wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`
  → **111 Tests / 3 Failures / 0 Ignored，exit 非 0**。本包 7 齿全绿（s1/s2/s3 结构 +
  b1/b2/b3 行为 + s4 boundary），3 红 = 在册基线红（见下）；无本包新增红。
- **server pytest（复跑）**：`python -m pytest -q --import-mode=importlib -m "not live" acceptance`
  → **125 passed / 1 failed / 6 skipped / 45 deselected**；唯一红 = SMK-A4
  （smoke-e2e `screen.md` 证据件缺失，server 面，非本包）。
- **Security gate**：securityReviewRequired=false；s4 boundary 齿 GREEN（无 openclaw/18789
  直连、契约/spec canary 在世、扫描器自检防假绿）。
- **Mutation check**：4 mutant（M1 push 比较符反转、M2 chat_target 漂移、M3 chat_clear
  时机破坏、M4 chat_clear 分支反转）逐一 per-AC turned red，全量 111/5 或 111/4 →
  验收齿对实现真实敏感，非空过。
- **Docs sync**：docsSyncRequired=false，docsSyncReason 在案（spec 零修订 / 契约零触碰）。

**在册红点名（3 条 + 1 条 SMK-A4，均在 main 基线即红，非本包责任）**：

| 红 | 位置 | owner | 性质 |
|---|---|---|---|
| WSA-A6:830 | `device_ws_switch.acceptance.c` | 260903-device-ws-switch | 本地-only 证据齿（ecs-teardown-evidence.md 缺），从未入仓 |
| WSA-A4:663 | `device_ws_switch.acceptance.c` | 260903-device-ws-switch | 本地-only 证据齿（flash.log 缺），从未入仓 |
| P2-A1:571 | `schedule_p2.acceptance.c` | 260911-render-pages（批次③） | KSP-A1 first-match 歧义（标题弹跳动画面缺失） |
| SMK-A4 | `server/acceptance/test_smoke_e2e.acceptance.py:422` | 260829-smoke-e2e | smoke-e2e `screen.md` 证据件缺失（server 面） |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无） | 本包 diff 未引入新 `minimal:` 标记；既有 `minimal:` 均在他包（story_page/schedule_page/reminder_page/audio_service）与本包无涉。 | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Maintainability | `kidpal_render_node.h` 末行 `#endif` 与 `kidpal_render_node.cc` 末行缺收尾换行符（git 报 "No newline at end of file"）。 | changed-code hunk：`kidpal_render_node.h` L170 及 `.cc` L630 无 trailing newline；C/C++ 预处理器/编译器不敏感。 | 不阻塞；后续编辑器顺手补。 |

承接记录：无 BLOCKING / SHOULD finding。结构闸 PASS/FAIL 不在此复述为 finding——
gate 全绿后仍已完整执行下方五轴语义 review，未发现需整改项。

## Five-Axis Review

1. **Correctness**：SU-A1~A9（除 SU-A8 真机 pending）全部兑现。`kidpal_layer.c` 与冻结
   离线契约逐字一致（类型/数值码/push 幂等去重/top=最高优先级/pop 指定层/chat_target
   恒=CHAT/chat_clear_event 事件表+未知保守驻留）；前台参与者五类（LOCAL_PAGE/VOICE/
   SERVER_PAGE/REMINDER/PROVISION）+ 聊天常驻层由 layer 栈单一裁决。SU-RULING-01
   （SERVER_PAGE>VOICE，回答期间内容页可见）与 SU-RULING-02（聊天 chrome 物理 reparent
   单屏）两条 deputy 裁定落地。边界（NULL/非法码/空栈/幂等）防御齐全。
2. **Test quality**：7 齿断言真实缺失行为/结构，非「没崩」空断言；红基线经实编实测
   （C 类 0 error 0 warning）证明红因 = 真实缺行为（weak 哨兵）与真实符号级残留，非
   链接错；mutation 全敏感。tighten-only 修订两次（SU-RULING-01/02）未放松断言语义，
   REVISION LOG 在案。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/spec.md`、`managed_components/**`
   零触碰（git diff 零命中 + s4 canary GREEN）；device 不直连 openclaw/18789。
4. **Operability/security**：layer 栈 8×int+int 零堆，DRAM 纪律不破；两套锁纪律
   （成对夹钳 begin_page/begin_reminder↔end_page；独立成对 begin/end_provision）均有
   注释锚定，配网进/退幂等、跨任务纯进/纯退安全；1s tick 轮询对账移除（竞态窗口消失）。
5. **Maintainability/simplicity**：三散标志 + tick 对账整体退役，换单一 layer 栈事件
   驱动——deletion test：复杂度随三标志与对账代码一起消失、不转移给调用方（正向收益）。
   `kidpal_layer` 为深模块纯逻辑（6 函数、无 heap、无过早抽象、无单调用方薄层）。

**structural / suiteGates 豁免枚举（逐条为何豁免）**：

- `structural: SU-A3`——char 独立屏并入 s_kidpal_scr 体系 + char_stroke 冻结齿复跑绿，
  是结构门（单屏不变式 / g_page.scr 零残留 / lv_screen_load 实参恒 s_kidpal_scr），由
  冻结结构齿 s3 断言覆盖，无需行为断言。
- `structural: SU-A8`——真机并发（语音轮+推送页、提醒恰在语音轮到点）0 复位 0 崩溃，
  是 human 持板真机观测型条目，非 host 可断言；证据落 `changes/dev/260911-screen-unification/`
  （当前 pending-human）。
- `suiteGate: SU-A6`——全量既有冻结齿复跑绿，套件级回归门（既有套件零新增红）——
  host ELF 全量 111/3 承重，3 红均在册基线、非本包新增。
- `suiteGate: SU-A9`——host runner 终跑全绿 exit 0，是过程性结论（非逐 id 断言）；
  本包 7 齿全绿、红基线已切绿，runner exit 非 0 仅因 3 条在册红。

## Residual Risk

- Known limitation：**SU-A8 真机持板证据尚未产生（pending-human）**——本包不代跑真机，
  如实标注；语音轮+推送页并发、提醒恰在语音轮到点 0 复位 0 崩溃的最终人证由 human 持板
  落 `changes/dev/260911-screen-unification/`（workstream 级收口由末位包
  260911-display-first-m1 统一对账）。
- decisionCoverage 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：**无**
  （本包 8 条决策均 landsIn=acceptance/package/exitCriteria，无 risk-accepted dropped 条目）。
- S2 exploratory scan：本包 `s2ExploratoryRequired` 未声明 → 未执行，无 S2 evidence。
- Follow-up recommended：观感面（字体子集/V3 换肤/排版）归批次③ 260911-render-pages
  （mergeOrder 21，本包后并）——本包明确零触碰（SU-E2）。
- Human decision needed：SU-A8 真机持板（触点② 阶段 5）；deputy 两条裁定
  （SU-RULING-01/02，actor=human-deputy，deviation=false）human 事后复核窗口开着，终裁
  HIT/MISS/DIFF-CREATED 尚未回写。

---

## 附：host 矩阵清单（SU-E4 · 全部写屏路径 × 触发源 × 前台归属）

阶段 0 勘察口径（builder 14 条写屏路径，本表按实现逐条收口；归一屏后前台归属 = layer 栈单一权威）：

| # | 写屏路径 | 触发源 | 归一屏后前台归属 |
|---|---|---|---|
| 1 | 首页 P1 帧渲染 `kidpal_render_node_show_home` | 开机 / 回首页 / 翻页回 home | LOCAL_PAGE 基层（清 transient 上层） |
| 2 | 首页动画重渲 `kidpal_render_node_tick` | LVGL 1s 定时器 | 仅 top()==LOCAL_PAGE 时重渲（无前台对账） |
| 3 | 聊天内容 `SetChatMessage` | 服务端/系统气泡 | 常驻 chat layer（CHAT，非栈成员） |
| 4 | 状态栏 `SetStatus` | 设备状态机跃迁 | 常驻 chat layer |
| 5 | 表情 `SetEmotion` | 情绪/状态事件 | 常驻 chat layer |
| 6 | 语音轮置前 `show_chat`/`voice_begin` | 按键会话 / Connecting | VOICE push（<SERVER_PAGE，语音不拦内容页） |
| 7 | 语音轮结束 `voice_end` | 回 Idle | VOICE pop；无服务端页回首页、有则回答驻留 |
| 8 | 服务端推送页（课表/故事）`begin_page` 夹钳 | 服务端内容下发 | SERVER_PAGE push（提醒让位） |
| 9 | 提醒弹窗 `begin_reminder` 夹钳 | RaiseReminderQueue 到点 | REMINDER push（>VOICE/SERVER） |
| 10 | 查生字页 `kidpal_char_page_show` | char 路由 / 翻页到 char | 画于 s_kidpal_scr（并入单屏，不再自建屏） |
| 11 | 配网进 `begin_provision` | WifiConfigMode 进 | PROVISION push（最高 6） |
| 12 | 配网退 `end_provision` | 配网成功 / 退出 | PROVISION pop，按优先级回落 |
| 13 | 手动翻页 `cycle_next` | BOOT 双按（home→char→schedule→story） | 清 transient + LOCAL_PAGE 基层 |
| 14 | 聊天 chrome 物理 reparent `KidPalReparentChatToLayer` | 板装配 init 前 | 迁 lv_layer_top（单屏时序：先 reparent 后 init） |