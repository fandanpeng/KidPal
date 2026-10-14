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
