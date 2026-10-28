# dispatch-progress · acceptance-author · 261028-p3-record-session-mirror-desync-fix

> 自包含进度快照（Write 覆盖语义；不载 tokens/gates/rounds）。机器权威在 verifier-evidence / pod-events。

## 已完成段

- [x] 输入面消费：input-brief.md 封闭清单 + plan.md（改动指令源）+ launch-card.md + 既有冻结件冲突检查（story_page_v2 / chrome_ghost / record_dialogue_guide / record_session_repair / record_session_live / record_dialogue_v7_fix / key_layer_dispatch）。
- [x] 冻结验收件落盘：`firmware/main/test/record_session_lifecycle.acceptance.c`（五个 TEST_CASE：RSL2-A1/A2/A3 结构锚 + RSL2-A4 canary（weak 哨兵）+ RSL2-A4 登记守；RSL2-A5 live-manual 申报，不入 runner 通过面）。
- [x] 红基线通道 B 实跑（`firmware/main/test/acceptance/red-baseline/rsl2_red_wrapper.sh`）：COMPILE_EXIT=0（-Wall -Wextra 零 warning），5 Tests 5 Failures，红名逐条 = 设计态缺失面（RSL2-A1 M1 镜像写缺席 / RSL2-A2 Sync 缺席 / RSL2-A3 清位缺席 / RSL2-A4 canary 通道形态红 / RSL2-A4 本件未登记）。
- [x] 双向可满足性取证（`firmware/main/test/acceptance/red-baseline/rsl2_sat_harness.py`，/tmp 合成树 KIDPAL_REPO_ROOT 注入，不写工作树）：绿树 5 Tests 0 Failures；六处定向变异（摘 M1 / 镜像赋值散落第三处 / Sync 插 voice_end 之后 / Sync 体调 cache_story / 摘 auto_connect 清位 / 摘登记）各自只打红对应 TEST_CASE。SAT-HARNESS OK。
- [x] 通道 A 引用条件核验：`git diff --quiet aecded8b HEAD -- firmware/` exit 0 → 引用 237T/3F 锚不重跑（launch-card 差异判停条款）。

## 已落盘文件

- `firmware/main/test/record_session_lifecycle.acceptance.c`（冻结验收件）
- `firmware/main/test/acceptance/red-baseline/rsl2_red_wrapper.sh`（通道 B 脚本）
- `firmware/main/test/acceptance/red-baseline/rsl2_sat_harness.py`（推演 harness）

## 下一步（planner / builder）

- builder FIRST ACTION：向 `firmware/main/test/host_app/main/CMakeLists.txt` HOST_ACCEPTANCE_SRCS 追加 `../../record_session_lifecycle.acceptance.c`（置于 host_infra_smoke 之前；登记行不在 acceptance-author 写权内）。
- builder 落地 plan M1/M2/M3 后：通道 B 应 5 Tests 0 Failures（RSL2-A4 canary 仍红 = 通道形态 GAP-4，不阻塞）；host 全量 runner 零新增红。
- 真机轮 RSL2-A5 T1-T5 由 human 持板（status=awaiting-human:real-device）；按键口径 = KEY 长按。
