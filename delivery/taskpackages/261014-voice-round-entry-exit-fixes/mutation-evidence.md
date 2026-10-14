# mutation evidence · 261014-voice-round-entry-exit-fixes

> 取证责任人 = builder（变异面 `firmware/main/application.cc` / `server/deploy/engine-patch/connection.py` 均在其写权域内，261013 教训已过查）；本文件由 planner 代落（builder 对 `delivery/taskpackages/**` 无写权，按派发书硬约定①回执代存，内容逐字取自 builder 回执）。
> 全部变异经「备份 → 变异 → 跑受影响齿 → 还原」四步，还原后 `diff -q` 逐一确认，末态 `CLEAN_NO_RESIDUE`（工作树无变异残留）。
> firmware 结构齿在运行时读磁盘源码（源文本 structural 齿），故变异后无需重编译即反映。

## M1 · VR-A1：摘除 idle 分支 A″ 直推两件

- 变异：删去插入的注释块 + `{ auto display = ...; if (kidpal_layer_chat_clear_event(...)) { SetChatMessage("system",""); } }` + `kidpal_render_node_voice_begin();`（`diff` 1006,1017d1005，共 12 行）。
- 命令：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd firmware/main/test/host_app && ./build/host_infra_test.elf"`
- 逐字红输出：`166 Tests 1 Failures 0 Ignored` / `FAIL`
  `voice_round_ui.acceptance.c:1695:...:FAIL:Function [voice_round_ui][VR-A1][VR-A2][VR-A3][structural].  VR-A1 RED: 唤醒词 idle 分支零 kidpal_render_node_voice_begin()——合并路径漏推 VOICE 层（语音页整轮不显示, TECH-261014-wakeword-connecting-voice-layer 本体）`
- 回退确认：还原后 `166 Tests 0 Failures 0 Ignored`；`diff -q` 备份 = 实现态。

## M2 · VR-A2/VR-A3（同源变异，合并执行）：tts stop 三守卫回退为原二分支

- 变异：三守卫块回退为 `if (listening_mode_ == kListeningModeManualStop) Idle else Listening`（删注释 + 查字守卫 + entered_from 守卫，`diff` 703,712d702 + 714,718d703）。
- 逐字红输出：`166 Tests 1 Failures 0 Ignored`（Unity `TEST_ASSERT` 首失败即中止，落 tooth 4）
  `voice_round_ui.acceptance.c:1748:...:FAIL:...  VR-A2 RED: tts stop 出口缺查字收尾守卫（kidpal_render_node_front_layer() == KIDPAL_LAYER_CHAR_WINDOW -> SendStopListening + Idle）——查字窗播完会话未结束（requirements v1.37 :652「播完会话即结束、不回聆听态」）`
- 回退确认：还原后全绿；`diff -q` 一致。

## M3 · VR-A2：仅摘除 sentence_start 轮来源采样（保留守卫）

- 变异：删去采样注释 3 行 + `speaking_round_entered_from_ = GetDeviceState();`（`diff` 737,740d736）。
- 逐字红输出：`166 Tests 1 Failures 0 Ignored`（落 tooth 7）
  `voice_round_ui.acceptance.c:1794:...:FAIL:...  VR-A2 RED: sentence_start 未在 SetDeviceState(Speaking) 之前采样轮来源（sample@-1, GetDeviceState@-1, setSpeaking@1034）——采样须取进入 Speaking 前的真实设备态（fork② 判别器依据）`
- 回退确认：还原后全绿；`diff -q` 一致。

## M4 · VR-A6：connection.py 同步调用回退为 await

- 变异：`390c390` `speak_txt(self, text.strip())` → `await speak_txt(self, text.strip())`。
- 命令：`python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive server/acceptance/test_kidpal_speak_sync.acceptance.py`
- 逐字红输出：`1 failed, 2 passed in 0.10s`；唯一红 = `KidpalSpeakSyncAcceptance::test_vr_a6_sync_speak_txt_takes_no_error_path`
  `AssertionError: Lists differ: ["kidpal_speak 播报注入失败: 'NoneType' object can't be awaited (提醒降级：弹窗+提示音仍必达)"] != []`
- 回退确认：还原后 `167 passed, 4 deselected`；`diff -q` 一致。

## 报告粒度说明（非弱断言）

Unity `TEST_ASSERT` 首失败即中止该 TEST_CASE，故 M2 只报 tooth 4 红、M3 只报 tooth 7 红——`mutationEvidence` 登记的 tooth 4/5/6/7 为「同族判别力」集合（同一守卫族变异多齿联动），非逐齿同时可见，属 Unity 语义所致。

## 取证豁免面

VR-A4（VQ-E1 冻结齿保持绿）/ VR-A5（record_session_live 零改动 canary）/ VR-A7（相对基线零新增红，套件级）/ VR-A8（真机轮 live-manual structural 豁免）——无逐 AC 变异对应物，取证豁免（口径同 261013 先例）。

---

# 补充轮 261014-r2 mutation 取证（2026-10-14）

> 取证责任人 = builder（变异面 `firmware/main/application.cc` / `firmware/main/protocols/websocket_protocol.cc` 均在其写权域内）；本文件由 planner 代落（builder 对 `delivery/taskpackages/**` 无写权，按派发书硬约定①回执代存，内容逐字取自 builder 回执）。
> 全部变异经「变异 → 跑受影响齿 → 完全回退」三步；回退后以同一命令复核 `168 Tests 0 Failures 0 Ignored`（`OK` / exit=0），末态工作树零残留。
> firmware 结构齿在运行时读磁盘源码（源文本 structural 齿），故变异后无需重编译即反映。

## M5 · VR-A9：摘除 A1 采样状态守卫 → 回退无条件逐句写

- 变异：把 `application.cc` L745-747 的 `if (GetDeviceState() != kDeviceStateSpeaking) { ... }` 守卫整块摘除，还原为原始两行无条件写（`speaking_round_entered_from_ = GetDeviceState();` / `SetDeviceState(kDeviceStateSpeaking);`）。
- 命令：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd firmware/main/test/host_app && ./build/host_infra_test.elf"`
- 逐字红输出：`168 Tests 1 Failures 0 Ignored` / `FAIL`
  `voice_round_ui.acceptance.c:1963:语音轮入口出口: 补充轮A1 采样写点状态守卫 (VR-A9):FAIL:Function [voice_round_ui][VR-A9][structural].  VR-A9 RED: sentence_start 采样写点零状态守卫「if(GetDeviceState()!=kDeviceStateSpeaking){」（现 0 处）——无条件逐句写使第 2 句起状态已 Speaking 覆写采样, 收尾判据③ 误伤多句对话轮回 Idle（补充轮 A1 主因; 真机证: 7 句妖怪轮 / 5 句天气轮末段 speaking -> idle）`
  首失败落 TEST 10 tooth 1（文件 :1963）；同变异下负向齿 tooth 3 亦应红（Unity 首失败即中止）。
- 回退确认：守卫块完整还原至 L745-747；回退后复核 `168 Tests 0 Failures 0 Ignored` / `OK` / exit=0。

## M6 · VR-A10：摘除 OpenAudioChannel 内 CloseAudioChannel() → 回退裸重赋值

- 变异：把 `websocket_protocol.cc` L95-97 的 `if (websocket_) { CloseAudioChannel(); }` 整块摘除（回退为裸重赋值旧 WebSocket 析构形态）。
- 逐字红输出：`168 Tests 1 Failures 0 Ignored` / `FAIL`
  `voice_round_ui.acceptance.c:2019:语音轮入口出口: 补充轮A2 旧通道拆除序 (VR-A10):FAIL:Function [voice_round_ui][VR-A10][structural].  VR-A10 RED: OpenAudioChannel 函数体零 CloseAudioChannel() 调用（现 0 处）——裸重赋值析构旧 WebSocket, stale OnDisconnected 迟到 Schedule(SetDeviceState(Idle)) 压在新会话之后（补充轮 A2 次因; 冷通道 listening -> idle 毛刺）`
  首失败落 TEST 11 tooth 4（文件 :2019）。
- 回退确认：`if (websocket_) { CloseAudioChannel(); }` 完整还原至 L95-97；回退后复核 `168 Tests 0 Failures 0 Ignored` / `OK` / exit=0。

## 取证豁免面（补充轮）

VR-A11（真机复验 live-manual structural 豁免）——无逐 AC 变异对应物，取证豁免（口径同 r1 VR-A8 / 261013 先例）。

## builder advisory 登记（非阻断，超包外风险）

TEST 11 tooth 5 以 `kvr_index_of(region, "CreateWebSocket")`（取 `OpenAudioChannel` 体内首个匹配）作序锚；builder 初版注释曾含字面 token「CreateWebSocket」致误红（命中注释而非真调用点），已改注释措辞消解、本轮绿。登记风险：将来任何在 `OpenAudioChannel` 体内、真 `CreateWebSocket` 之前出现 `CreateWebSocket` / `local_close_pending_ = false` / `CloseAudioChannel();` 字面的注释或代码，都可能使 tooth 4/5 误判。
