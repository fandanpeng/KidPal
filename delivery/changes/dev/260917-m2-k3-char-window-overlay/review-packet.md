# Review Packet · 260917-m2-k3-char-window-overlay

> workstream m2-char-window ｜ M2-K3 ｜ mergeOrder 30 ｜ rigor full ｜ riskTier high ｜ subsystem firmware
> integrator 收拢制品（2026-09-19）；合并门判读归 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。

## Change Size

- **实现（builder 未提交改动，firmware 面 12 文件 +349/-155 + 3 新增文件 +235）**：
  - `firmware/main/display/kidpal_layer.h` +37/-… / `kidpal_layer.c` +22/-…——**层数值码 renumber**：插入 `KIDPAL_LAYER_CHAR_WINDOW = 5`，`REMINDER` 5→6、`PROVISION` 6→7，下层 `LOCAL_PAGE=2 / VOICE=3 / SERVER_PAGE=4` **零偏移**；`is_stackable` 上界为符号引用自动跟随；transient 集 `s_transient[]` 纳入 CHAR_WINDOW；头注「现役权威」改指本包新冻结件 `char_window_overlay.acceptance.c` seam [1]（归档件 `screen_unification.acceptance.c` 的旧「逐字一致」断言清除）。
  - `firmware/main/key_gesture_parser.h` +23/-… / `.cc` +30/-…——`GLOBAL_ACT_CHAR_REPLAY = 5`（append-only，既有四值零偏移）；`key_route_layer` 第三参语义**扩为前台层归一化值**（0 = 基层/配网/无前台；非 0 = 前台覆盖层层值），新增查字窗列（KEY 短按=重播 / KEY 长按·BOOT 单击=关窗回原页 / 双击族·BOOT 长按=NONE）；新增 `#include "display/kidpal_layer.h"`（符号引用层闭集，零数值字面耦合）。
  - `firmware/main/display/kidpal_render_node.h` +52/-… / `.cc` +110/-…——新增 `begin/end_char_window` 持锁夹钳（推 CHAR_WINDOW 层、不起会话/聆听）；`front_is_overlay` 闭集纳入 CHAR_WINDOW（页点隐藏 + 键路由同源）；新增 **5 分钟 tick 驱动（seam [9]）** + `kidpal_render_node_char_window_key_hit` 归零点；**收拢 M2-K2 遗留写侧死态 API** `cache_char` / `cache_char_data` 及静态 `s_has_char_data` / `s_char_json`（定义 + 声明整体删除）。
  - `firmware/main/display/char_page.h` +17/-… / `.cc` +128/-…——锚 `canvas-p4` → **`char-window`** 改名收尾；覆盖窗**叠层不清屏**（关窗回原页不翻页）；**右栏对账三项**（部首行补上屏 = 四行齐 / 进度珠死成员 `progress_label` 清零 / 握笔小手 12px 圆环占位 `charpage_draw_penhand` 与 `CHARPAGE_PRESS_SIZE` 整体删除）；底部提示条文案改消费 `kidpal_char_card_build` 的 `progress` 单源（S-F3.3 AC2 定格「写完啦·共 N 笔」）；`charpage_on_panel_delete` 补清 `panel/canvas` 句柄防悬垂二删。
  - `firmware/main/mcp_server.cc` +34/-…——case 3 查字路由改经 `begin/end_char_window`（**路由语义与 `view_router` 数值码本体零改动**；`cache_char_data` 调用点收拢删除）。
  - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` +32/-…——`keya_front_route_top` 前台层归一化喂第三参；`GLOBAL_ACT_CHAR_REPLAY` → `kidpal_char_page_replay`；两键 `OnPressDown` 调 `char_window_key_hit`（任意键重计）。
  - `firmware/main/CMakeLists.txt` +6/-0——`display/char_window.c` 进设备源表。
  - `firmware/main/test/host_app/main/CMakeLists.txt` +13/-0——新验收件 + `char_window.c` 强符号源 + builder 单测注册进 `HOST_ACCEPTANCE_SRCS`（**置于 host_infra_smoke 之前**）。
  - **新增 3 文件**：`firmware/main/display/char_window.{h,c}`（26+39 行，5 分钟驻留计时纯逻辑单源 `kidpal_char_window_idle_step` / `KIDPAL_CHAR_WINDOW_IDLE_MS = 300000`）、`firmware/main/test/char_window.test.c`（170 行，builder 补强面，**非判定权威**）。
- **验收冻结（commit `759709e` + `7f2e0f3`，firmware 面 3 文件 +1479/-10）**：
  - 新增 `firmware/main/test/char_window_overlay.acceptance.c`（**1375 行，7 TEST_CASE**，覆盖 A1–A6 + boundary；seam [1]–[9]）。
  - `key_layer_dispatch.acceptance.c` +70/-0（append-only 追加查字窗列三齿 + seam [1] 第三参语义扩注，既有通用覆盖层列/基层列零改动）。
  - `retract_p4_three_page.acceptance.c` +44/-10（**完备性反查发现的假红翻正**——test 6 tooth 2/tooth 5 的「cache_char 保留」守卫随本包收拢翻正为「已收」）。
- **Net diff**：firmware 面合计 **18 文件 +2063/-165**（实现 15 + 验收 3）。
- **簿记面（非实现改动，随收口 commit 冻结）**：`delivery/taskpackages/<id>/taskpackage.json`（mutationEvidence.perAcceptance 6 条回填）、`delivery/taskpackages/current/taskpackage.json`（planner finalize 投影）、`input-brief-*.md`、`pod-events.jsonl`（append 3 条）、`verify-runs.jsonl`、`verifier-evidence/`（immutable，只引路径不复制正文）。
- **Scope**：板端 `firmware/` 单一子系统——① 查字窗层注册 + 键位语义矩阵 + 页点隐藏；② 5 分钟驻留计时（纯逻辑 + 设备侧驱动）；③ 右栏对账三项 + 锚改名 + M2-K2 遗留 API 收拢。云端 `server/` **零改动**，`docs/**`（含 `docs/contracts/**`）、`.claude/**` **零触碰**（`git diff 210bb6e..worktree` 逐面零命中）。
- **Split recommendation**：不拆——层注册/键位矩阵/驻留计时是同一「会话内覆盖窗」语义的三面（层归属决定键路由与页点、计时决定关窗时机），半拆即窗语义断裂；验收件单文件冻结、一次 RED→GREEN 链承载。

## Verification Evidence

- **Structural gates**：final r3 fact **九闸全绿**（`ok:true`、`observedGateExit:0`、invocationId `1221bff2-dbc5-4f73-9d5d-b72b3014d1e0`，2026-09-19T00:39:10.961Z–00:39:25.229Z，evidenceRef `delivery/taskpackages/260917-m2-k3-char-window-overlay/verifier-evidence/rounds/r3.json`）——taskpackage-required / **firmware-build（device build 真编译，`VERIFY_FW_BUILD_REQUIRED=1`，`xiaozhi.bin` 0x30b7d0 bytes、分区余量 11%，未降级）** / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（pytest 37 passed）/ acceptance（oracle-pass，firmwareAcceptanceFiles 22）。scoped r1 fact（invocationId `f1b37186-d4ce-4a1c-b70d-aca61559d66c`，rounds/r1.json）**mutation-check 红**（mutationEvidence 空骨架，设计态红）；scoped r2 fact（invocationId `268044e3-aef8-4ff2-b425-713cd6e5d09d`，rounds/r2.json）七闸全绿。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260917-m2-k3-char-window-overlay --phase final --round 3 --invocation-id 1221bff2-…` **integrator 实跑 exit 0、`factOk:true`、`observedGateExit:0`**（2026-09-19）——final evidence 有效，可进 closeout。
- **host runner 全量（integrator 独立复跑，非闸境）**：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build` 后直调 `./build/host_infra_test.elf`（**elf 相对 host_app/**）→ **`58 Tests / 0 Failures / 0 Ignored`、EXIT=0**（2026-09-19 integrator 实跑）。账实相符：launch 基线 **46T/0F @ 210bb6e**（progress.md 锚）+ 本包新齿（新件 7 TEST_CASE + 两在世件留痕修订 + builder 单测 3 齿 + seal TEST 8）= 58；红基线链 57T/1F（seal 后，唯一红 = A3 驱动半）→ 58T/0F。
- **server 面（跨子系统覆盖边界）**：`server/` 本包零改动（`git diff 210bb6e..worktree -- server/` 零命中）；pytest `37 passed`（unit 闸）。本包无 server 侧事件流/契约面改动，故「跨子系统事件流面」无本包可归因的增量；覆盖边界 = 固件 host 逻辑面（host runner 全量）+ 设备编译面（device build 闸），真机渲染/层交互面 deferred（见 demo 对账行）。
- **Mutation check**：TaskPackage `mutationEvidence.perAcceptance` **六条 record（M2K3-A1/A2/A3/A4/A5/A6）全部 `sensitive:true` + `failed as expected`**，`sensitive:true × 6`，M2K3-A7/A8 structural 豁免——各条「单点变异 → 转红 → 还原复绿」链完整，还原指纹逐字节相同（sha256 逐条在册），全仓零变异残留。7/7 全红（含 A4/A6 本轮新造靶）。A5 关键加固证据：变异态下切区内仍留携带「部首」字样的注释、齿仍转红——实证 sealing pass 的 C 注释剥离收紧有效（原裸扫形态会假绿）。本 packet 引用 taskpackage 字段，不重跑。
- **Red baseline（taskpackage `redBaselineResult` 逐字消费）**：**封口后现行基线 = host runner 全量 57 Tests / 1 Failures / 0 Ignored**（唯一 FAIL = 新 TEST 8「M2K3-A3 驱动半」设备侧驱动接线，设计态红）；**封口前 = 56T/0F**（builder 实现态）；**launch 基线 = 46T/2F**（delta 恰好 2 条本包义务红、其余 44 零回归）；新件 standalone 线束 = 8 Tests / 1 Failures（仅 test 8 红）。
- **Docs sync**：`docsSyncRequired=false`；实现面零 docs/contracts/spec 触碰经 `git diff` 核对（详见同目录 `docs-delta.md`）。契约零改动裁定另有 architect-reviewer 门 1 GREEN 在案。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。

## Demo 对账行（workstream 包锚 3 · roadmap §③ K3 demo 证据形态核对）

roadmap 权威口径（`delivery/workstreams/m2-char-window/roadmap.md` §③ K3）：**「demo 证据形态：host runner + device build 闸 + F3 真机确认轮（human 持板：S-F3.1 AC1–AC4 清单 + NFR-PERF-2 串口计时，沿用 RM-A8 取证形态）」**；同节「完成后人能看到什么」= ① 说「某个字怎么写」当前页上弹出查字窗（不再有查字页）：米字格超大字逐笔演示 + 右栏拼音带声调/共 N 画/部首/组词全四行 ② KEY 短按从头重播、长按关窗回原页，BOOT 关窗回原页不翻页 ③ 5 分钟没动自动关窗 ④ 窗内没有页点、播完安静让孩子照着写（不回聆听态）⑤ 提醒弹窗弹出时照旧压过查字窗 ⑥ 真机确认轮同时复测 NFR-PERF-2（5 秒档）并复证 K1 两缺陷 live 修复。逐项对账：

| roadmap 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| host runner（A1–A6 齿全绿留痕） | `char_window_overlay.acceptance.c` 7 TEST_CASE（层注册/键位矩阵/驻留计时/页点隐藏/右栏对账/锚改名）+ builder 单测 3 齿 + `key_layer_dispatch` 查字窗列三齿 → 全量 **58T/0F EXIT=0**（integrator 独立复跑）；A1–A6 各一条 mutation 反证（`sensitive:true × 6` + 还原复绿） | 一致 |
| device build 闸 | final r3 `firmware-build` exit 0（真编译、`VERIFY_FW_BUILD_REQUIRED=1` 未降级、`xiaozhi.bin` 0x30b7d0 / 余量 11%；`char_window.c` / `kidpal_render_node.cc` 的新驱动均在编译内，E3 device build 面满足） | 一致 |
| F3 真机确认轮（human 持板：S-F3.1 AC1–AC4 + NFR-PERF-2 串口计时，RM-A8 形态） | **deferred、未执行**——M2K3-A7 / A8 structural 条目在册（note：真机齿 deferred，human 持板，证据落 `delivery/changes/dev/<changeId>/`）；本包**host 半闭环、真机半未做** | 一致（roadmap 口径本即「human 持板」；deferred 与口径吻合，无超出声明的证据形态、无未声明的降级） |

**一致性结论：一致。** demo 证据形态三项与 roadmap §③ K3 权威口径逐项吻合；**本包 host 半闭环、真机半（A7/A8）未做**——review-packet 明记此点，**不得读成整包已验收**。效果段核对：①②③④由 host 齿 + device build 闸落证（A1 层注册 / A2 键位矩阵 / A3 驻留计时 + 页点隐藏 / A4 页点 / A5 右栏四行 / A6 锚改名）；⑤ 提醒压查字窗层级由 A1 结构齿（CHAR_WINDOW 落 REMINDER 之下）+ `front_is_overlay` 闭集钉；⑥ NFR-PERF-2 5 秒档 + K1 两缺陷 live 复证**归 A7/A8 真机轮**（roadmap 口径）。数字链自洽：46（launch 锚）+ 本包新齿 = 58 = 全量实跑；红基线 57T/1F 的唯一红 = A3 驱动半（sealing pass 补钉前后）。**无不一致项，无需就 demo 口径上报 planner。**

### UI design-review 证据（纯文本；本包触查字窗覆盖窗 + 键帽条页点）

- 改动面：`char-window` 覆盖窗（米字格大字盘 + 右栏四行速查卡 + 底部提示条 + 逐笔动画）与键帽条中槽页点隐藏为 UI 可视行为；设计权威 = `docs/requirements.md` F3 全节 + §2.2 查字窗行/提醒行（v1.37 线框 WF-F3-01 + 设计稿 v4 `char-window`）。
- **design-drift summary（brief↔实现）**：右栏四行「拼音带声调 + 共 N 画 + 部首 + 组词」与 S-F3.2 AC1 现文一致——**部首行补上屏即本包消除的历史漂移**（数据面已装配、原 label 拼装漏）；底部提示条文案改为消费 `kidpal_char_card_build` 的 `progress` 单源（去静态占位「跟随笔画一笔一画描红」，落 S-F3.3 AC2 定格「写完啦·共 N 笔」）；窗层归属 CHAR_WINDOW（提醒之下）与 §2.2 行 67「提醒 = 最高层」一致；「播完不回聆听态」由 `begin_char_window` 不起会话（无 ToggleChatState/show_chat/voice_begin）钉。**无设计漂移。**
- viewport coverage / a11y summary：本包不触 a11y 面；viewport = RLCD 单一物理屏（归一屏单屏不变式不动）；真机像素级目击 deferred 归 A7 设备轮（同 demo 对账行）。
- external artifact reference：无（bootstrap 期不在仓内存 screenshot/video/HTML 证据，重资产留仓外）。
- residual risk：真机覆盖窗观感、5 分钟自动关窗目击、提醒压查字窗层级目击未在本包，挂 A7 设备轮（见 Residual Risk）。
- UI finding severity：本包无 UI BLOCKING / SHOULD（设计权威面零漂移）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （空） | changed diff 内无 `minimal:` 标记（`git diff 210bb6e..worktree -- firmware/main \| grep minimal:` 实证零命中） | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | firmware/build | `firmware/main/CMakeLists.txt:76` 新注释写「设备侧调用点待接 —— 见本包 builder 交付报告「遗留」节」，但设备侧调用点**已在本包接通**（`kidpal_render_node_tick` 1s 累计喂 `kidpal_char_window_idle_step`）——注释与代码现状相左，会误导审读者以为 A3 驱动半未接 | changed-code hunk `firmware/main/CMakeLists.txt`（+6，新注释块）+ 实证调用点在场（`grep -n kidpal_char_window_idle_step firmware/main/display/kidpal_render_node.cc` → :751；graph `trace_path` inbound 在本工作树项目解析 0 边属解析局限，以 grep 为准） | 登记（本 packet + docs-delta）；**不在本包就地改**——合并门冻结已验 diff，post-verification 改动需新轮（iterationBudget 已用 3）；建议 planner 后续 micro-fix 批校正该注释 |
| NIT | judgement-call | firmware/display | `firmware/main/display/kidpal_render_node.h:81` 注释「begin_page 内部取锁，调用方不得已持有 LVGL 锁（否则递归死锁——不得在 LVGL 定时器回调内调用）」与本包新驱动的实证相左——`kidpal_render_node_tick`（LVGL 定时器回调，锁已持有）内直调 `kidpal_render_node_dismiss_front_overlay`（内部再取锁）**不死锁**，因 `lvgl_port_lock` = 递归互斥量（builder 实证 esp_lvgl_port lvgl9 `xSemaphoreCreateRecursiveMutex`，pod-events `4cf60886-…` 超读链） | changed-code adjacency：`kidpal_render_node.cc` tick 内的新驱动块（changed hunk）**依赖**该递归锁性质；`kidpal_render_node.h:81` 陈注声称相反 | 登记（改善项，不扩面）；建议后续批校正该注释（本包未改，如实留痕） |
| FYI | judgement-call | firmware/display | `firmware/main/display/char_page.cc:523` device build 告警 `variable 'ch' set but not used [-Wunused-but-set-variable]`（`const char *pinyin = "?" , *radical = "?" , *ch = "?" , *words = "?" ;`）为本包改动块**邻近的基线既有**告警，本包未顺手改 | final r3 `firmware-build` stdout 逐字（编译告警）+ 该行在本包 diff 中为**未改的 context 行** | no-trigger——本包未顺手改（不扩面），如实登记；建议后续批清理 |
| FYI | judgement-call | firmware | `key_gesture_parser.cc` 新增 `#include "display/kidpal_layer.h"`（parser → layer 的新 include 边） | changed hunk（+`#include "display/kidpal_layer.h"`）+ 反向零回引（`kidpal_layer.h` 不 include `key_gesture_parser.h`，**无环**）+ 引用为**符号常量**（优于复制数值字面） | no-trigger——符号引用层闭集（零数值字面耦合），无环、无跨层新增调用方（见「影响面核对」） |
| FYI | judgement-call | governance/tooling | 本 Pod 五条 friction 已落 pod-events：① 拆包期 sourceRefs 遗漏四处真实改动面（`ac30fd55-…`）② `origin.type` 枚举值 `milestone` 不在闸闭集（`0337c1b8-…`）③ acceptance-author 挂钟停滞（`bf5c5107-…`）④ 三处 over-scan 申报（`5bff6dd0-…` / `93fe9f88-…` / `da58c461-…` / `4cf60886-…`）⑤ **seam 漏钉一处承重落地面**（A3 设备侧驱动无齿，`cfefdf89-…`） | pod-events 事件 id 在场 + 该等项不在 verify 闸集内（多为拆包器产出侧与验收面欠覆盖，已由 sealing pass 消解） | no-trigger——非本包阻断项，候选并入下次治理批（承重缺口已由 acceptance-author 补 seam [9] 消解） |

无 BLOCKING。结构闸 PASS/FAIL 不复述为 finding（verifier evidence 承载）；gate 全绿仍完成五轴语义 review（见下）。

## Five-Axis Review

1. **Correctness**：五条功能齿语义齐备——A1 层注册（CHAR_WINDOW 进栈模型 + transient 清层语义 + stackable 一致）；A2 键位矩阵（查字窗列 KEY 短按=重播 / KEY 长按·BOOT 单击=关窗不翻页 / 双击族·BOOT 长按=NONE；`key_layer_dispatch` 基层列等价 guard 零回归）；A3 驻留计时（纯逻辑 + 设备侧驱动落 `kidpal_render_node_tick`）；A4 页点隐藏（`front_is_overlay` 闭集纳 CHAR_WINDOW）；A5 右栏四行 + A6 锚改名/API 收拢。边界条件齐：`idle_ms == NULL` 防御（从不为 UB）、到点幂等关窗、**非查字窗前台把累计器归零**（防历史累计误关窗，防「窗一开首拍即关」）、`pop_transients` 空栈/配网共存零触碰、键越界拒收。层 renumber 机械安全：**全仓消费点均为符号常量引用、零数值字面耦合**（唯一字面在两处注释/归档件，归档件不编入任何构建）；下层三值零偏移。设备侧关键接缝已闭合：`dismiss_front_overlay`（关窗唯一入口）在同任务递归锁下安全、`charpage_on_panel_delete` 补清句柄防悬垂二删。
2. **Test quality**：断言真实行为非实现镜像——A1/A2/A3 行为半直接断言 host 可编纯逻辑（`kidpal_layer_*` / `key_route_layer` / `kidpal_char_window_idle_step`）的返回值；结构半对设备专属文件（`kidpal_render_node.{h,cc}` / `char_page.{h,cc}` / `mcp_server.cc` / board）用稳定签名切区 + 「区内零字面/计数」结构齿。**sealing pass 加固**：13 处正向计数齿改走 **C/C++ 注释剥离器**（保字符串字面量体）+ CMake `#` 注释剥离，受控变异 M4' 实证「真删部首行 + 保留携带字面量的注释」正确转红；负向齿与钉头注的锚齿不剥（剥了会放松）。六条功能齿全部 mutation 敏感（`sensitive:true × 6`）+ 七条单点变异逐齿翻红 + 还原复绿（A4/A6 本轮新造靶）。红基线由 acceptance-author 独立实跑（launch 46T/2F / 封口前 56T/0F / 封口后 57T/1F），非 builder 自证。**完备性反查两次命中**（K2 retro 教训 #1 的第二次实例）：`retract_p4_three_page.acceptance.c` 的 K2 守卫齿随收拢必假红 → 翻正为「已收」。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（`git diff` 零命中 + contract-lint 绿）；`view_router` 数值码零改动（case 3 区守卫齿钉）；`key_route_layer` 第三参语义**扩展**（0 子集保留、非 0 从「1」扩为「实际层值」）——通用覆盖层列行为等价（REMINDER/VOICE/SERVER_PAGE 仍走通用列，A2 guard 齿 + 板级 `keya_overlay_consume` 分流路径零回归）；层闭集改动落 `firmware/main/display/kidpal_layer.h/.c`（**实现面，非 `docs/contracts/**`**），走 acceptance-author 留痕通道。`server/` 零改动。**spec 面零漂移**（向已冻结需求对账）。`mcp_server` char 路由的缺字段降级门保持不动。
4. **Operability/security**：无 secret / 权限 / correlationId 面（security 闸 `securityReviewRequired=false`，exit 0）。新增 `kidpal_render_node_char_window_key_hit` 为跨任务无锁整数置位（按键回调任务写、LVGL 定时器任务取并清位；32 位对齐整数读写原子，未消费也清位防跨窗口陈旧置位）；`char_window.c` 对 `NULL` 指针防御（从不为 UB）。计时器不另起（落既有 1s tick），无新增攻击面。**无新增依赖**（`char_window.{h,c}` 仅引 `stddef.h`；无 chip/FreeRTOS/LVGL 依赖）。
5. **Maintainability/simplicity**：改动局部、单源清晰——层闭集单一权威（`kidpal_layer.h`，路由表符号引用而非复制）；5 分钟计时纯逻辑单源（`char_window.c`，设备侧仅驱动）；底部提示条文案消费 `card.progress` 单源（零重抄格式）；握笔小手占位与 `progress_label` 死成员是**纯删除**（复杂度消失、非转移，符合 deletion test）；锚 `char-window` 改名收尾消解 M2-K2 遗留写侧死态 API（登记交接闭环）。**无过早抽象、无重复实现**：`keya_front_route_top` 是薄归一化函数（1 调用点族内），复用 `front_is_overlay` 单一裁决；`char_window.test.c` 明标「补强面非判定权威」（不与冻结齿争权威）。simplification tags 零命中（`minimal:` 零命中）。两处陈注（见 Findings）为既有/本包引入的注释准确性问题，非结构冗余。

## Residual Risk

- Known limitation:
  - **M2K3-A7/A8 真机确认轮 deferred（human 持板）**——F3 真机确认轮（S-F3.1 AC1–AC4 / S-F3.2 AC1 / S-F3.3 AC1–AC2 / 窗口交互包全项含 5 分钟自动关窗 / 提醒压查字窗层级 / K1 两缺陷 live 复证）+ NFR-PERF-2 5 秒档复测在本包外；本包 **host 半闭环、真机半未做**（device build 闸只担保编译，不担保实机行为）。**接手口径**：随 M2 设备轮（human 持板，沿用 RM-A8 串口取证形态）一并执行；证据落 `delivery/changes/dev/260917-m2-k3-char-window-overlay/`。
  - **层数值码残留登记（非活耦合、未纳入本包）**：归档件 `firmware/main/test/acceptance/archive/screen_unification.acceptance.c` 的 `#define KIDPAL_LAYER_REMINDER 5` 与活跃 assert 在 renumber 后不准确——**已实证不编入任何构建**（host_app 与设备 CMakeLists 均不含），属非活耦合，未硬改亦未停包；`reminder_overlay_unify.acceptance.c` 数处 stale 注释文本；另两处在世冻结件（`key_layer_dispatch` / `retract_p4_three_page`）残留结构同型的裸正扫齿。**建议后续批统一 refresh**（含引入注释剥离器）。
  - **Findings 两条陈注**（CMakeLists「待接」/ render_node.h「递归死锁」）未在本包就地改，登记交后续批（见 Findings；合并门冻结已验 diff 之故）。
- Follow-up recommended：
  - M2 设备轮执行 A7/A8（F3 确认 + NFR-PERF-2）；后续批 refresh 归档件/注释/裸正扫齿 + 校正两处陈注 + 清理 `char_page.cc:523` 未用变量告警。
  - 五条 friction（拆包器 sourceRefs 遗漏 / 枚举形态税 / acceptance-author 停滞 / over-scan / seam 漏钉）候选并入下次治理批。
- Human decision needed：**无**（无契约/spec 变更面由本包就地裁、无依赖新增、无安全残余）——A7/A8 真机轮走 M2 设备轮 human 持板既有通道，非本包新增升级点。回滚方式 = 本包 commit 单点 revert（改动面收敛于 firmware 12 源/接线 + 3 新件 + 3 测试件，无 schema/契约迁移；`char_window_overlay.acceptance.c` / `char_window.{h,c}` / `char_window.test.c` 新增件随 revert 移除，CMakeLists 登记行反向删除）。
- decisionCoverage：TaskPackage `decisionCoverage` 为**空集（`[]`）**——无 `landsIn: dropped && reason: risk-accepted` 条目，无 D<n> 风险接受挂账。

## 影响面核对（worktree 口径 · step 11b）

- **detect_changes: N/A (worktree)**——工具图与索引 checkout（主 checkout）非同一树；Pod worktree 的 diff 对主索引不可见。本树另有图项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（随树增量刷新，已见 `char_window.c` 新件），以 `git diff --stat` 闭集 + 关键共享符号 `trace_path` / grep 抽查替代（2026-09-19 integrator 实跑）。
- `git diff --stat 210bb6e..worktree`：改动闭集 = firmware 18 文件（实现 15 + 验收 3）+ 簿记面；`server/`、`docs/contracts/`、`docs/requirements.md`、`docs/architecture.md`、`.claude/` **零命中**。
- 关键共享符号抽查（均在 firmware/main 场内，无跨层/跨子系统新增调用方）：
  - `kidpal_render_node_front_is_overlay`（**签名与语义闭集改**）`trace_path` inbound = 11 callers，全在 board / render_node / lcd_display 场内（含新 `keya_front_route_top`）——消费侧闭集不变、无外部新增调用方。
  - `kidpal_layer_push` / `kidpal_layer_pop_transients`（**闭集改**）callers 全在 `kidpal_render_node.cc`（7 push 点；pop_transients 2 点：`dismiss_front_overlay` / `cycle_next`）——符号引用、签名不变。
  - `key_route_layer`（**第三参语义扩**）inbound = 波板 `InitializeButtons` / `CustomBoard`（板级 dispatch）——签名不变，第三参由「0/1 归一化」扩为「0/前台层值」，差集为**新层值 5（CHAR_WINDOW）**，`keya_overlay_consume` 与通用覆盖层列零回归。
  - `kidpal_char_window_idle_step`（新）live 消费点 = `kidpal_render_node_tick`（唯一设备侧消费者；host 强符号覆盖 frozen weak 哨兵）；`kidpal_char_page_replay` 消费点 = 板级 `:144`；`begin_char_window` 消费点 = `mcp_server.cc:666`；`char_window_key_hit` 消费点 = 板级 `:431/:439`。
  - 注：`trace_path` 对本树 `kidpal_char_window_idle_step` / `kidpal_layer_push` / `pop_transients` 解析 inbound 0 边（图调用边解析局限），以**直接 grep caller 清单**为准（上列行号实证）。
- 索引状态：工具图与工作树差异如实以本节手工抽查承载，未虚报；re-index 需求归主线程/human。

## Merge Packet / Supervisor 输入

- 待合并分支：`dev/260915-m1-k1a-key-layer-dispatch`（本 Pod 常驻分支，本包改动冻结于其上，mergeOrder 30）。
- 合并路径：planner 主线程跑 `supervisor-verdict-gate.mjs` 机械闸 → APPROVE 后前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行 `merge-driver --pod` 本地合并；**本 integrator 不执行 merge、不 push、不在 main 上 commit**。
- 阻塞项：**无 BLOCKING finding**；`closeout-check` 两 blocking 制品（`docs-delta.md` / `review-packet.md`）已晋升本目录。
- 唯一需 planner 裁定的非阻塞项：Findings SHOULD（CMakeLists 陈注「待接」）——登记或走后续 micro-fix 批，**不阻断**本包合并门。
- 回滚方式：单点 revert 本包 commit（见 Residual Risk）。
- 遗留脏行说明：`delivery/taskpackages/current/verdict.json`（verify.mjs 脚本 fs-write 的 non-bearing 投影，指向 r3）为簿记面 M 态，随本收口 commit 一并冻结；`.codebase-memory/` 脏为工具再生面常态，不 stage、不还原；`firmware/build/` 与 `firmware/main/test/host_app/build/` 为白名单构建产物，不 stage。

## 边界外移交项（integrator scope 外 · 主线程执行）

- **里程碑状态判读（`delivery/milestones/**` 不在 integrator scope allow 面内，不绕过）**：`delivery/milestones/milestones.jsonl#M2-char-window` 现 `state: planning`。**dogfood 判读结论 = 应动状态，目标 `planning → in_delivery`**。依据（`build-feature.md` 里程碑完工度规则 + `delivery-status.mjs` state 权重）：① M2 的**首刀已落**（K1 已 merged 16b0d4e、K2 已 merged 2608b68，本包 K3 为末包）——`in_delivery` 转移此前漏做；② **不宜直接跳 `ready_for_acceptance`**：K3 **尚未 merged**（在合并门），且 **A7/A8 human 设备确认轮未闭环**（milestone `nextAction` 自载「live 端到端待实证」）——`ready_for_acceptance`（权重 .9）当前过早。
  - **主线程具体动作**：改 `delivery/milestones/milestones.jsonl` 第 2 行 `M2-char-window` 的 `"state":"planning"` → `"state":"in_delivery"`，跑 `node .claude/scripts/delivery-status.mjs` 重建 `delivery/milestones/STATUS.md`；（可选）本包合入后由 planner 在设备轮闭环时再走 `→ ready_for_acceptance → accepted`。STATUS.md **不手改**。
  - 附注：本包 taskpackage 未置 `deliveryMilestoneStateChanged`/`milestoneStateChanged`，故 `closeout-check` 不因 STATUS.md 泳移而红；主线程改状态后须跑 delivery-status.mjs 收敛。