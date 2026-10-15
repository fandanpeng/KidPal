> 归位注记（acceptance-author, 2026-09-08）：本文件目标位置为
> `C:/AI/KidPal/delivery/taskpackages/260829-story-p3/acceptance/red-baseline-summary.md`
> （launch 输入授权的落点之一），但 enforce-scope 拦截（acceptance-author 写边界 =
> `firmware/**/*.acceptance.c` + `firmware/**/acceptance/**` + `server/**/acceptance/**` +
> `tools/**`；`delivery/taskpackages/**` 归 planner）。故落边界内本路径（WSA/CHR 包
> red-baseline-summary 同目录先例；包专名防覆盖同名件），请有写权方原样归位。

# KSP3（260829-story-p3 故事包）红基线判定摘要（acceptance-author 2026-09-08 实测）

TaskPackage `260829-story-p3`（包 8「故事：列表 + 录制上传归档 + 回看/回听 + 成就盖章」，
subsystem=firmware+server，riskTier=high，testHost=host+PC 云端）冻结验收两文件的红基线执行记录：

- `firmware/main/test/story_p3.acceptance.c`（C 侧, host 单测, taskpackage `acceptanceTestsFile` 钉定）
- `server/acceptance/test_story_p3.acceptance.py`（python 侧, offline + live）

## 0. 归位/接线注记（acceptance-author 被 enforce-scope 拦的三处，请有写权方落）

1. **host_app CMakeLists 接线**（launch 输入第 3 步，实测被钩子拦）：
   `firmware/main/test/host_app/main/CMakeLists.txt` 不在 acceptance-author allow 面内
   （任务包 `notes.intakeC5_scopeMap` 自身亦记为「builder 可写登记 HOST_ACCEPTANCE_SRCS」）。
   拦下原文：
   `scope: agent "acceptance-author" 只能写 firmware/**/*.acceptance.c, firmware/**/*.acceptance.cpp,
   firmware/**/acceptance/**, server/**/acceptance/**, tools/**；
   firmware/main/test/host_app/main/CMakeLists.txt 超出边界。`
   builder 须向 `HOST_ACCEPTANCE_SRCS` 加两行（照包 2/3/5/7 先例，实现落地后紧随第二行）：
   `"../../story_p3.acceptance.c"` 与 `"../../../display/story_page.c"`；
   **列表首行位置即可（保持在 `host_infra_smoke.acceptance.c` 之前）——该文件必须仍在列表
   最后**（包 0 boundary 齿注册序守卫：后列出=先注册=先跑，其 nothing-rendered 断言须对新鲜
   进程执行；本包渲染齿在其之后跑，不破坏该守卫）。
   无需新增 include dir：host 组件 `INCLUDE_DIRS "." "../../../display"` 已在册，
   验收文件经 `__has_include("story_page.h")` 在实现头落地后自动切到真实契约声明。
2. **taskpackage 勘误**：`acceptanceTestsFile` 需补第二行
   `"server/acceptance/test_story_p3.acceptance.py"`（云侧承载；python 先例同包 5/6/7）。
3. **本摘要**：见顶部归位注记。

## 1. C 侧红基线（命令与实跑）

闸上 runner 判据 = host_app linux-target ELF（HOST_TEST_CONVENTIONS [C4.4]），但接线属 builder
（见 §0.1）。本次按包 2/4 先例用边界内脚手架（本目录 `unity.h` stub + `runner.c`，非冻结件）
把**同一份冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o <out>/runner ../../story_p3.acceptance.c runner.c
        # BUILD_EXIT=0、告警数 0（§4 C 类冻结验收实编通道过：零 error 零 warning）
cd <repo root> && <out>/runner          # 每 TEST_CASE 首个失败即中断，退出码 = 失败 TEST_CASE 数
```

实测：**7 Tests 6 Failures**（boundary 守卫 PASS）。
（`wsl` 下 `echo $?` 恒 0 是 wsl.exe 参数重组伪影，VRT/CHR 摘要已记录同款；判据取报告行计数。）

判读要点（冻结纪律：红必须红在真实行为缺失）：
- 行为齿经 **weak-symbol frozen offline contract 真执行**：验收文件头钉死 14 个纯逻辑契约函数
  （`kidpal_story_list_parse / counter / text_parse / view_init / page_input / on_control /
  rec_tick / rec_time / open_reader / screen_label / stamp_spec / on_saved / stamp_text /
  page_render`），weak 默认返回哨兵 -2 —— 契约未实现时断言以「frozen contract NOT implemented」红，
  builder 落地强定义后 weak 被覆盖、真逻辑被测。红 = 行为缺失，不是链接/编译错误。

### 逐条判定（stub run, 2026-09-08 实测）

| TEST_CASE | 判定 | 红因（各自真实缺失行为） |
|---|---|---|
| P3-A1 装配结构 (structural) | **RED** | firmware/main 无任何文件实现 frozen contract（`kidpal_story_list_parse` 零引用）——齿 1/2/3/4/5/6a/6b 全缺：无 story 页模块、未编入 CMakeLists、`mcp_server.cc` 无 `VRT_ROUTE_STORY_LIST`/`VRT_ROUTE_STORY_TEXT` 路由 ack（view 5/6 仍落 default 降级）、渲染面无调用点、无降级语义标记、无「故事徽章」/「我要讲故事」文案面；包 2 底座守卫绿（view_router include / CMakeLists 装配 / kidpal/display 注册 / 数值码 5·6·2 未偏移） |
| P3-A2 (story_list 解析 + ×N 双钉) | **RED** | `kidpal_story_list_parse` 未实现（weak 哨兵）——逐字段解析、**total 优先**（total=40/3 items → 计数器 40）、**缺省回退 items.length**、total=0 与缺省可区分、畸形/缺字段/非整数·负 total/超 cap 的 -1 降级、扩展位容忍、safe-truncate 全缺 |
| P3-A3 (光标 + 双击进阅读 + 全局不吞) | **RED** | `kidpal_story_view_init`/`page_input` 未实现——KEY 短按下移（含回绕）、KEY 双击进阅读态、空列表不动、KEY 长按/BOOT 单击/BOOT 长按交全局、越界键位与 NULL 视图零污染全缺 |
| P3-A4 (start_recording → 录音态 → 停录触发归档) | **RED** | `kidpal_story_on_control`/`rec_tick`/`rec_time` 未实现——契约 §1.4 入口切页、计数器不预推、重试幂等、REC 闪点（在闪/非频闪）、声波在跳、大时长 "00:47"/"01:15"、KEY 停录 → 一次归档请求、V31 KEY 长按取消丢弃（archive_pend 不涨）、非录音态/未知 action 不消费全缺 |
| P3-A5 (story_text 分屏 + 翻屏 + 朗读触发) | **RED** | `kidpal_story_text_parse` 未实现——{id,title,text} 逐字段、无正文/无 id 拒绝、生字标注扩展位忽略不消费（P3-E4）、`open_reader` 全文不丢分页与单调、`screen_label` 逐字「第 X 屏 · 共 N 屏」、BOOT 长按翻页 + 末屏钳制、KEY 朗读触发计数、`screen_lines=0` 不除零全缺 |
| P3-A6 (盖章帧 + N 严格同步) | **RED** | `kidpal_story_stamp_spec`/`on_saved`/`stamp_text` 未实现——双圈/内虚/-8°/800ms 几何、盖章切页、N=total+1 且与 counter 同步、文案逐字「故事徽章·第 4 颗已入册！」、同 id 幂等、新 id 再推、刷新以云端 total 权威覆盖、NULL/空 id 零污染、盖章帧不吃页内输入全缺 |
| P3-A2/A3/A4/A5/A6 **render**（LVGL 真帧齿） | **未在本次 stub run 执行（编译排除）** | 需 `lvgl.h` include path，独立 stub 无 —— 经 `__has_include` 守卫被编译排除，**其红态在 builder 接线进 host runner 时兑现**（`kidpal_story_page_render` weak 哨兵 → 红）。齿内容：列表暗像素内容存在 / cursor 0·1 反白可见差异 / ×3·×10 计数差异 / 阅读第 1·2 屏差异 / 录音·盖章态画面互异 / NULL 与缺数据不崩 (-1) |
| boundary/security（扫描器自检 + 润色/生字高亮/归档归属） | **GREEN（守卫，必须保持绿）** | 扫描器四态自检 + 计数器校验通过；固件零「润色」、零 `story_highlight`/`newWordMarks`（P3-E4 已裁不消费）、零 `stories/` 归档路径引用、零 openclaw/18789 直连；`control/start_recording` 到达面在世 |

### 守卫非空洞性实证（额外一次链接跑）

把**同一份冻结验收文件**与包 2/包 3 的真实强符号一起链接执行
（`story_p3.acceptance.c` + `../../../display/view_router.c` + `../../../session_state_machine.cc`
+ `../../../key_gesture_parser.cc` + `runner.c`，gcc/g++ 混链，链接 error 0）：
结果仍是 **7 Tests 6 Failures**，且 boundary 齿内的
`kidpal_display_control_action("start_recording") == VRT_CTRL_START_RECORDING` 守卫
**真执行并 PASS** —— 证明：① weak 指针判空跳过机制在强符号在世时会真的跑（守卫非空转）；
② 本文件与包 2/3 的 C 符号形状（extern "C" + weak 重声明）在混链下无冲突。
`ses_machine_*` / `key_route_global` / `kidpal_display_route` 三处同类守卫因 P3-A4/A5 齿先打在
weak 哨兵上（实现未落地）而在本相位不执行，语义正确性已逐条对照
`firmware/main/session_state_machine.h` 与 `key_gesture_parser.h` 的冻结转移表核过。

## 2. 真闸 runner（host_app ELF）当前地板实测

接线前先把闸上 runner 的既有地板钉清楚（供 builder/verifier 判读「本包新增红 vs 地板红」）：

```
wsl -d Ubuntu-24.04 -- bash .../host_app/espidf_wsl.sh --preview set-target linux   # 一次性 (本次新配)
wsl -d Ubuntu-24.04 -- bash .../host_app/espidf_wsl.sh build                        # Build 560/560 OK
# 从**仓库根**跑 ELF：
wsl -d Ubuntu-24.04 -- bash -c 'cd <repo root> && ./firmware/main/test/host_app/build/host_infra_test.elf'
```

- **从仓库根执行（推荐）：`60 Tests 2 Failures`** —— 与任务包 `notes.knownRedFloor` 记录的
  **2 红地板**完全一致：`WSA-A4`（flash.log 本地-only 未入仓）+ `WSA-A6`
  （ecs-teardown-evidence.md 同上）。二者均为本地证据件缺失、`main` 本身即红，与本包无关。
- 从 `firmware/main/test/host_app/` 目录执行：**`60 Tests 3 Failures`** —— 第 3 红是
  `voice_roundtrip_b.acceptance.c` 的 **VRT-A2/E2 证据齿**，属**既有验收件的 harness 缺陷**：
  其 `vrt_find_evidence` 以**仓库相对路径**直接 `stat()`（未加定位到的 root 前缀），
  换 cwd 即找不到 `delivery/taskpackages/260829-voice-roundtrip/roundtrip-live-evidence.md`
  （该件确实在仓内、五事实标记齐，`grep` 已核）。**不是本包引入的红**，
  也不属本包修复面（acceptance-author 不得改他人冻结件）。
  → **给 verifier/planner 的操作性结论**：host runner 请**从仓库根**执行，
  否则地板恒为 3（并会与本包新增红混淆）。
- 本包文件的扫描齿一律经 `kstory_rel()` 前缀已定位 root，**与 cwd 无关**（已从两个 cwd 各跑一次核对）。

接线后的期望计数（builder 落 story_page.c 后）：TEST_CASE 数 60 → **68**
（本包 8 齿，含 render 齿），全绿判据 = `68 Tests 2 Failures`（2 = WSA-A4/A6 地板）
+ 从仓库根执行；本包 8 齿必须 0 红。

## 3. python 侧红基线（命令与实跑）

```
cd server
python -m pytest acceptance/test_story_p3.acceptance.py -q --import-mode=importlib
```

环境注记（如实）：WSL Ubuntu-24.04 的系统 python3 与 IDF venv python3 **均未装 pytest**
（`No module named pytest`），故本相位用 Windows 侧 python 3.14 + pytest 9.1.1 执行
（260829-char-stroke 包先例同款：其红基线亦记「Windows python + pytest 实测」）；
文件同时可 `python acceptance/test_story_p3.acceptance.py` 独立跑（本次双通道都跑过）。

实测：**18 failed, 2 passed, 1 skipped（0.89s），收集零错误**
（§3 冻结即冒烟：pytest collection error = 0；standalone unittest 同 20 项、18 失败）。

| test | 判定 | 红因 |
|---|---|---|
| test_a1_story_module_landed_and_reached | **RED** | `server/gateway/story.py` 缺失——云端故事面（归档/下发/V31 回退/有界缓冲）与「被服务路径消费」两面皆缺（P3-A1 云侧半边） |
| test_a4_archive_writes_real_md_and_roundtrips | **RED** | `archive_story`/`list_stories`/`read_story_file` 缺失——停录整段转写**真实落盘** + 读回闭环 + 同 id 幂等重写全缺（数据路径须真落盘，非 mock 形状） |
| test_a4_archive_rejects_empty_transcript | **RED** | 同上——空/仅空白/None 转写的 `ValueError` 大声拒绝 + 「拒绝即不写文件」缺失 |
| test_a4_archive_title_fallback_is_nonempty | **RED** | 同上——缺 title 时的**非空**回退（首标点前片段、<=12 字）+ 纯 CJK 标题下 id 仍守 `<YYYY-MM-DD>-<slug>` 形态缺失 |
| test_a4_accumulate_mode_not_sentence_by_chat | **RED** | `StoryAccumulator` 缺失——chat↔accumulate 路由、start/feed/stop 链路、空累积 stop 返回 None 全缺（契约 §1.4「不按句送对话」无承载） |
| test_a2_list_total_is_full_set_not_page_slice | **RED** | 同上——**total = 全集计数**、`limit/offset` 只切 items 不动 total、越界页空而 total 原值、新归档后 total 单调 +1（= 盖章「第 N 颗」的权威源）全缺 |
| test_a2_list_ordering_item_shape_and_junk_ignored | **RED** | 同上——按完整日期倒序、item keys 恰 {id,title,date}、date 显示 "MM-DD"、杂物文件不灌水、整包过真实 `validate_display` 门全缺 |
| test_a2_list_empty_dir_is_zero_not_fake | **RED** | 同上——空目录 total=0/items=[]（零故事 ≠ 缺省）且过 schema 门缺失 |
| test_a5_story_text_shape_no_newword_field | **RED** | `story_text` 缺失——data keys 恰 {id,title,text}（P3-E4：无生字标注字段）、逐字段等值、过 schema 门、列表 id → 正文键闭环全缺 |
| test_v31_cancel_discards_without_archiving | **RED** | V31 (a) 录音中取消 discard（不归档、total 不涨、回 chat、缓冲不残留）缺失 |
| test_v31_wrong_take_returns_to_chat | **RED** | V31 (b)「讲错啦」回闲聊丢弃缺失 |
| test_v31_redo_within_window_undoes_archive | **RED** | V31 (c) 停录 10s 内「重录」——撤销上一归档（**真删文件** + total 回落）+ 转写回累积态 + 再 stop 恰留一条 缺失 |
| test_v31_redo_after_window_keeps_archive | **RED** | V31 (c) 边界——超窗「重录」拒绝且归档保持、无上一归档时 None 缺失 |
| test_v31_recover_rescues_missed_story | **RED** | V31 (d) 漏判「刚才那段是故事」——近 N 句保留 + 挽回归档 + 挽回一次即清环（不重复入册）+ 空环返回 None 全缺 |
| test_v31_cancelled_content_not_recoverable | **RED** | V31 关键区分——cancel 丢弃的内容不得被 recover 捞回（丢弃 vs 漏判两条路径可分）缺失 |
| test_buffer_bounded_per_session_ring | **RED** | notes 硬约束——近 N 句环**句数 + 字符双上限**、FIFO 淘汰最旧（保 newest「刚才那段」）缺失（防 char-stroke §8.3 内存叠压同型缺陷） |
| test_buffer_bounded_across_sessions | **RED** | notes 硬约束——会话面 `max_sessions` + LRU、总量 <= max_sessions × char_window、取消后会话态释放（仅活动会话持缓冲）缺失 |
| test_prompt_carrier_v3_landed_and_wired | **RED** | overlay `PROMPT_SRC` 现指 **v2** < 3：故事 producer 意图（我要讲故事→start_recording）/ 停录整段归档起标题 / V31 回退判断的 prompt 增量未版本化落 v3 新文件；渲染面与 `provider.config.yaml` 声明面两面切换未做（「验收所测 = 生产所用」） |
| test_boundary_contract_surface_untouched | **GREEN（守卫，必须保持绿）** | `VALID_VIEWS` 七 view + `MAX_DATA_BYTES=65536` + `start_recording` 可透传 + `end_session` 网关切决定 四项冻结面在世；server code 零「润色」（P3-E3 后置不混入） |
| test_e2_existing_directionb_acceptance_green | **GREEN（守卫，必须保持绿）** | gateway-directionB 套件独立复跑 exit 0（20 Tests OK, 0.065s 实测） |
| StoryP3LiveAcceptance.test_live_story_roundtrip_evidence | **SKIP（机械闸）** | `@pytest.mark.live` + `server/acceptance/conftest.py` 默认 skip（ARQ-260901-g3）；`RUN_LIVE=1`/`KSP3_RUN_LIVE=1`/`-m live` 显式放行。四事实 = 意图→start_recording 下发 / 停录整段转写 / `stories/*.md` 归档落盘 / 列表回读与盖章出现；**麦克风采音殿后 9b（P3-E2）**，本齿在本包台架相位或 9b 相位执行并回填，不入冻结红基线计数 |

### 守卫目标选择的如实记录

E2 回归守卫沿包 5 先例选 `test_gateway_directionB.acceptance.py`（实测 20 Tests OK）。
未选 `test_voice_roundtrip_b.acceptance.py`：其 C 孪生 VRT-A2 证据齿对 runner cwd 敏感
（§2 已定位成因），云侧孪生亦有证据件依赖，选它作守卫会引入恒红噪声——与本包无关，
不掩盖、不改他人冻结件，只记录。

## 4. 红的构成一句话

**红 = C 侧 6/7（+接线后第 7 个 render 齿）对当前 story 缺失行为成立（固件无 P3 故事页契约面、
mcp_server.cc view 5/6 仍落 default 降级、无盖章/提示文案面）+ python 侧 18/21 成立
（`server/gateway/story.py` 与 prompt v3 载体全缺）；C boundary 与 python 双守卫为 green、
live 一齿机械 skip。无一红因是 import/collection/编译错误**（C BUILD_EXIT=0 且 0 warning、
pytest 收集 0 error、断言全落缺失行为）。本包把它切到绿。

## 5. 冻结契约给 builder 的输入（唯一被迫接受面）

- **C 侧**（建议落点 `firmware/main/display/story_page.(c|h)`；头须与本文件 fallback 块
  **逐字一致**，可加字段不可改名/改语义）：14 个函数签名 + 结构体字段名 + 数值码
  （页态 LIST0/READER1/RECORDING2/STAMP3；动作码 -2 全局 / -3 降级 / 0 none / 1 cursor /
  2 open_reader / 3 page_turn / 4 tts_read / 5 stop_record / 6 cancel_record；
  盖章 rings=2 · inner_dashed=1 · angle_deci=-80 · hold_ms=800；
  `KSTORY_LIST_MAX=16`、`KSTORY_TOTAL_ABSENT=-1`；时长 "MM:SS"、上限 5999s）+
  逐字文案（`第 %d 屏 · 共 %d 屏` / `故事徽章·第 %d 颗已入册！`，取 spec S-F2.3/S-F2.4）+
  全部语义与降级路径。详见 `firmware/main/test/story_p3.acceptance.c` 文件头
  FROZEN OFFLINE CONTRACT 与「冻结相位决策登记 D1/D2/D3」。
- **python 侧**（建议落点 `server/gateway/story.py`，纯 python、import 期不碰 fastapi/front）：
  `archive_story / read_story_file / list_stories / story_text / StoryAccumulator`
  （含 `sentence_window/char_window/max_sessions/redo_window_s` 四参数与
  `routing/start/feed/stop/cancel/redo/recover/recent_depth/recent_chars/active_sessions/
  total_buffer_bytes` 方法面）+ id 形态 `<YYYY-MM-DD>-<slug>` 与散列回退 + 标题非空回退
  （<=12 字）+ 排序按完整日期显示 "MM-DD"。详见
  `server/acceptance/test_story_p3.acceptance.py` 模块 docstring。
  云侧另须：把故事 producer 指令**版本化落 `openclaw-system-prompt.v3.md`**（新文件、v1/v2 可
  diff）并把 `apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC` 与 `provider.config.yaml` 的
  `artifact/source` 两面同时切到 v3。

## 6. 与本包无关但必须上报的读码事实（Red Flag，交 planner 处置）

冻结前定位到 `firmware/main/mcp_server.cc` 在 HEAD 上**语法不完整**（设备目标构建必红）：
`kidpal/display` 工具回调的 switch 里，`case 4:`（schedule）的
`app.Schedule([...]() { int drawn = …; ESP_LOGI(…);` **lambda 从未闭合** —— 其闭合格
`});` / `break;` / `}` 现在成了紧随其后的 `case 3:`（char）lambda 的闭合，
`case 3:` 与 `default:` 因而都落在 schedule 的 lambda 体内（case 标签不在 switch 内 = ill-formed）。

归因（逐字核对，非 merge 丢行）：`git show f2a67dd -- firmware/main/mcp_server.cc`
的 hunk `@@ -212,6 +214,22 @@` 把 16 行 case-3 新代码插在
「`ESP_LOGI(… "schedule_render" …)`」与既有上下文三行 `});` / `break;` / `}` **之间**，
即插在了 case 4 lambda 的体与它的闭合格中间；`git diff f2a67dd HEAD -- 同文件` 为空
→ 缺陷自 **260829-char-stroke 落地提交** 起即在，非 merge-driver 造成。
旁证：该包 commit message 自记 `firmware-build(降级)`（设备构建门降级），
故设备侧语法红未被机械闸拦下。

**影响面** = 设备目标构建（`idf.py build` / 烧板）；**不影响本包 host 验收**
（host runner 的 `HOST_ACCEPTANCE_SRCS` 不含 `mcp_server.cc`，本包结构齿对其只做文本扫描，
且 `VRT_ROUTE_STORY_LIST/TEXT` needle 今日零命中 = 真缺行为红）。
**为何必须现在上报**：本包 builder 落地 P3-A1 齿 3 时要往同一 switch 补 `case 5/6`，
会在一个已破损的 lambda 上动刀——先由 planner 定修复归属（属 char-stroke 遗留缺陷面，
不在本包 story 验收面内，也不在 acceptance-author 写面内）。
