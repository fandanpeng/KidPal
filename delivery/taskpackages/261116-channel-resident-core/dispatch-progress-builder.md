# dispatch-progress · builder · 261116-channel-resident-core（真机轮 A 修复轮）

> 本文件 = 当前派发（D1/D2/D3 实现 + 冻结验收转绿）的自包含进度快照。覆盖写；
> 不载 tokens / gates / rounds / verdict。权威判据归 verifier。

## 本轮目标

在包边界内实现真机轮 A 三缺陷修复（D1 语境注入去边缘化 / D2 丢弃终止在途回合 +
迟到帧门 / D3 空素材会话空闲收尾），让冻结验收 SCRF-B7/B8/B9、SCR-A8、SCRF-B10
与 RDG-A4 重锚转绿，既有回归网保绿。

## 实现落点

### D1 — front（`server/gateway/front/front_app.py`）

- `_consume_kidpal_control` ctx(story) 分支（:1315）：注入条件
  `ctx_push_edge and upstream is not None` → `upstream is not None`（每会话无条件注入
  `kidpal_record_context active=true`）；`_push_story_list` 仍守 `ctx_push_edge`（:1324 一带）。
- `_story_route_display_payload`（:495）：返回语义 `enter_edge` → `consumed`
  （命中 `control/start_recording` 即 True；`acc.start` 幂等照旧）。
- `_relay_display_blocks_to_device`（:1828）：同步返回语义 consumed；新增 additive
  `edge_probe` 可变列表出参（消费前自查 routing，供审计行标 edge）。
- 审计 token `story_record_ctx_enter` 单一 emit 点放 `_inject_record_context_enter`
  （:1976）：additive kwargs `session_id/src/edge`，字段
  `corr_id=<sid> src=ctx_story|start_recording edge=true|false injected=true|false`。
  三处调用点（ctx 分支 :1315 / side-channel relay :1943 / fence relay :1964）共用单点。
  源文本计数 = 1（实测）。

### D2 — front（同文件）

- discard 分支（:1432）：返回前 `_release_record_context(session_id,
  [(websocket, upstream)])`——只用泵内本连接腿；upstream 缺席时 legs=[]（仍清标记 /
  arm 门）；不发 abort 帧。
- 会话关闭门：arm 单点 = `_release_record_context`（:2036）；disarm = 设备上行
  `listen start` / ctx(story)（`pump_client_to_upstream` :2205 一带）。
- `pump_upstream_to_client`（:1904 一带）：门内丢弃二进制音频帧与 `type in
  ("tts","llm")` 文本帧；放行 `stt` / `mcp`。令牌在场整轮放行，tts stop **先判后清**。
- 令牌登记点两处：`_speak_to_legs`（:406，覆盖引导 / 整理安抚）与
  `_deliver_alert_now`（:2687，提醒 speech；条件 = speech 文本非空）。
- 状态容器：`_CLOSED_SESSION_GATE` / `_EXPECTED_SPEECH`（:1022 一带），沿
  `_RECORD_CTX_SEEN` 先例（WeakKeyDictionary + 属性面回退）；deadline 30s、重复取 max。

### D3 — firmware（`firmware/main/application.cc`）

- 常量（:127）：`static const int kStoryEmptySessionTimeoutS = 60;`，定义在既有
  `kSessionIdleTimeoutS = 60;`（:116）之后；注释不含既有常量符号字面。
- CLOCK_TICK 区新 producer（:522-525）：`Listening && 记录语境正向门 &&
  !s_story_rec_entered && !s_vad_speaking && clock_ticks_ >= 阈值` → 审计行
  `story_empty_session_timeout` + `HandleEndVoiceSessionEvent()`。既有 60s 无语音
  producer 四判据与语义零改动；未写 `clock_ticks_ % 10`；未直调 `OpenAudioChannel(`。

## 自测实跑

| 面 | 命令（cwd） | 结果 |
|---|---|---|
| server 全量 | `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`（仓根） | **486 passed / 0 failed / 9 deselected**（1 warning + 33 subtests passed） |
| host 全量 | `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` | **307 Tests / 3 Failures / 0 Ignored**；3 红全为上游 `p1_auxbar_style` AB-A1/AB-A2/AB-A4；`(SCR-A8):PASS` |
| firmware 编译 | `cmd /c "set MSYSTEM=&& set IDF_PYTHON_ENV_PATH=...idf5.5_py3.10_env&& cd /d <repo>\firmware && <idf>\export.bat && idf.py build"` | 编译通过；`application.cc.obj` 构建并链接；`build/xiaozhi.bin` 0x329080 字节（app 分区 8% free） |

host build：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build`
→ `ninja: no work to do`（application.cc 不在 host 编面；验收件本轮未改）。

## 变异证据（逐条实跑，源文本精确替换 → 跑 → 整文件还原 + sha256 校验）

还原方式：每次变异前读取原文件全文，`finally` 写回并按 sha256 比对（全部 True）；
未用 `git checkout`（工作树持有未提交实现）。

| # | 组 | 精确变异 | 命令 | 转红测试（FAILED 行） |
|---|---|---|---|---|
| 1 | D1 | ctx 分支 `if upstream is not None:` → `if ctx_push_edge and upstream is not None:` | pytest SCR+RDG `-k "b7 or a4"` | `test_scr_b7_repeat_ctx_story_injects_again_story_list_stays_edge_only`、`test_rdg_a4_front_injects_record_context_enter_exit_on_engine_leg` |
| 2 | D1 | `_story_route_display_payload` `return True` → 先算 `enter_edge` 再 `return enter_edge` | 同上 | `test_scr_b7_start_recording_consumption_injects_when_accumulating` |
| 3 | D1 | 审计 token `story_record_ctx_enter` → `story_record_ctx_hit` | 同上 | B7 三例（single_emit_point / repeat_ctx / start_recording） |
| 4 | D1 | 删 ctx 分支 `_inject_record_context_enter(...)` 调用 | 同上 | `test_scr_b7_repeat_ctx_story_injects_again_story_list_stays_edge_only`、`test_rdg_a4_front_injects_record_context_enter_exit_on_engine_leg` |
| 5 | D2 帧序 | 删 discard 分支 `_release_record_context(...)` 两行 | pytest SCR `-k b8` | `test_scr_b8_discard_releases_context_before_forwarded_stop`、`test_scr_b8_repeated_stop_injects_no_second_release` |
| 6 | D2 腿集 | `_discard_legs = [(websocket, upstream)] ...` → `_session_legs(session_id) or list(_alert_legs)` | 同上 | `test_scr_b8_discard_branch_has_no_global_alert_legs`（源锚负锚）+ `discard_releases...` + `repeated_stop...` |
| 7 | D2 门 | 删门内二进制丢弃（`if _closed_gate_closed(...) and not _speech_token_active(...): continue`） | pytest SCR `-k b9` | `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt`、`test_scr_b9_speak_token_allows_round_then_tts_stop_clears` |
| 8 | D2 门 | `("tts", "llm")` → `== "tts"`（删 llm） | 同上 | `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt` |
| 9 | D2 门 | 门集合加 `"mcp"` | 同上 | `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt` |
| 10 | D2 令牌 | 删 `_deliver_alert_now` 内 `_speech_token_register(device_ws)` | 同上 | `test_scr_b9_alert_speech_registers_expected_round_token` |
| 11 | D2 令牌 | tts stop「先判后清」→「先清后判」 | 同上 | `test_scr_b9_speak_token_allows_round_then_tts_stop_clears` |
| 12 | D3 | `kStoryEmptySessionTimeoutS = 60;` → `= 30;` | host 全量 | `(SCR-A8):FAIL`（307 Tests / 4 Failures） |
| 13 | D3 | producer 删 `!s_story_rec_entered &&` | host 全量 | `(SCR-A8):FAIL`（307/4） |
| 14 | D3 | 审计 token `story_empty_session_timeout` → `story_empty_session_hit` | host 全量 | `(SCR-A8):FAIL`（307/4） |
| 15 | D3 交叉负锚 | 在 `static const int kSessionIdleTimeoutS = 60;` 前插 `/* kSessionIdleTimeoutS; 变异注释 */` | host 全量 | `(SCR-A8):FAIL` + `(SCR-A5):FAIL`（307/5） |
| 16 | 兜底 | 在转发 `listen stop` 之前插入一次 `_release_record_context(session_id, [(websocket, upstream)])`（= 把释放挪到 stop 之前） | pytest SCR `-k b8` | B8 三例，含对照齿 `test_scr_b8_key_stop_still_archives_once_and_releases_after_stop` |

观察项（验收质量，非实现缺陷）：
- 第 3 组首试用**超串**改名（`story_record_ctx_enter_renamed`）不转红——冻结断言用
  `str.count` / 子串匹配，原名是改名后的子串；改用非超串 token（`..._hit`）后转红。
  源锚代理的区分力边界，留作 acceptance 质量观察。
- 第 15 组首试注释不含 ASCII `;`：SCR-A8 交叉负锚红、SCR-A5 仍绿（其区切从注释到定义行
  的 `;` 之间仍含 `60`）；补 ASCII `;` 后两件同红。如实记录。

## 清单外阅读申报

| 文件 | 原因 |
|---|---|
| `server/gateway/reminders.py` | 读 `deliver_alert` / `build_speech_injection` / `build_alert_frames`，确认提醒 speech 的引擎腿注入不经 `_speak_to_legs`，据此把令牌登记点落在 `_deliver_alert_now`（只读，未改）。 |
| `server/acceptance/test_points_e2e_spike.acceptance.py` | 核对 `_relay_display_blocks_to_device` 的源区正锚（K0PTS-A4）不受返回语义/新形参影响（只读）。 |
| `firmware/main/test/session_channel_resident.acceptance.c`（SCR-A8 之外的 helper 段） | 读 `scr_read` / `scr_region_between` / `scr_locate_root` 以确认源锚扫描口径与 cwd 无关（只读）。 |
| `delivery/taskpackages/261116-channel-resident-core/dispatch-progress-acceptance-author.md` | 读 acceptance-author 回执口径与变异建议（派发指令指定）。 |
| `server/acceptance/test_record_dialogue_guide.acceptance.py`（RDG-A4 段） | 核对重锚后的断言（只读，未改）。 |

## 未决 / 风险

- `_consume_points_intent` 去重语音走 `_send_engine_leg_frames` 直注、不经
  `_speak_to_legs`：会话关闭门在场时该语音可能被当迟到回合丢弃。冻结方案只列两处令牌
  登记点（`_speak_to_legs` / `_deliver_alert_now`），本包按冻结面落，未扩第三点，列 residual。
- 门内放行的 `mcp` 帧含 display 块（迟到回合仍可能闪屏）——B8 修好后该回合不应存在；
  冻结件已列 residual（GAP-B9-2）。
- 令牌 deadline 30s 到期路径无离线断言（GAP-B9-1），归真机轮 B-D2。
- D3 设备侧 60s 自动收尾的运行时行为 = 真机轮 B-D3；本轮只落 producer 与源锚代理。
- firmware 编译用 Windows 侧 ESP-IDF（`IDF_PYTHON_ENV_PATH` 显式指向 `idf5.5_py3.10_env`
  以绕开 `export.bat` 默认查找 `idf5.5_py3.14_env` 的缺省偏差）；构建产物在
  `firmware/build/`（可再生面）。