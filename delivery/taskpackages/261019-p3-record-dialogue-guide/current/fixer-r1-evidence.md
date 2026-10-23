# fixer 轮一（二跑）· mutation 取证与证据存档（planner 代落，2026-10-20）

> fixer（dispatch a685039318172b4c6）无 delivery/** 写权，本文件 = 其报告取证段与闸链事实的 planner 代落存档。报告全文见派发回执（会话 transcript）。

## 1. fixer 报告的 host 测试实跑输出（本包 mutation 取证）

- host 编译：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → Done
- host 运行：`wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`（cwd = firmware/main/test/host_app）
- 结果：**219 Tests, 3 Failures, 0 Ignored**
  - 本轮新增 guard 全绿：`record_dialogue_v7_fix.test.c:169`（RDV7-A1 keybar v7 键帽集 + 闭集零混入负锚）PASS；`record_dialogue_v7_fix.test.c:243`（RDV7-A2 D2 联动缝/真相核/记录语境守卫源锚齿 + v7 资产落点与气泡单源）PASS
  - 受影响冻结套件 216 PASS 保持绿（RDG-F1..F5、RSR-A2/A3/A4/A7、RSL 系列、SPV2/story_cache/key_layer_dispatch 等）
  - **3 失败 = 存量红，非本轮引入**：`p1_auxbar_style.acceptance.c` AB-A1/A2/A4（home_p1.c 于 04f8409a「2026-10-16 看屏轮打回直修」改 ic_batt_15→ic_batt_20、退役 wifi 位图，冻结锚未回钉；两涉案文件本轮 `git status` 零触碰）
- 设备侧编译：`idf.py build`（ESP-IDF v5.5.1，/c/tmp/kp_idf_build.bat）→ Project build complete，改动四 TU 全部重编零告警；构建再生 sdkconfig 噪音已还原

## 2. verifier 闸链（planner Bash 直跑，bearing）

- scoped r7：`verify.mjs --evidence-mode=round --phase=scoped --round 7 --invocation-id 59856c80-c835-4eab-a16d-f65cb38d52c9` → ok:true，evidence r7.json，verifier-evidence-check exit 0
- final r8（full bearing，--scope-file = 本包 scope-final.md，planner 补 fix 增量行 record_dialogue_v7_fix.test.c）→ ok:true，evidence r8.json，verifier-evidence-check exit 0
- r8 闸明细：taskpackage-required / change-manifest / firmware-build（真编译）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security（scoped 全绿）+ unit（323 passed, 4 deselected）/ acceptance（227 passed, 4 deselected；firmware on-target 验收注明由实机/台架相位承载）（full 全绿）

## 3. D1 hotfix 核对结论（fixer 核，planner 存档）

`server/gateway/front/front_app.py:131` diff 核对通过：headers 增加 `"Authorization": "Bearer " + os.environ.get("OPENCLAW_TOKEN", "")`，token 源与 gateway/provider.py:76 同名 env 逐字一致，方法/URL/超时未动，无其它改动。已部署 ECS（front active，旧文件 .bak-261019-dialogue-guide），本修随 fix commit 收编。

## 4. 遗留登记（不阻本修，交回 human / 后续通道）

1. **存量红回钉**：p1_auxbar_style AB-A1/A2/A4 —— 04f8409a 直修未回钉冻结锚，独立小包/fixer 轮清理。
2. **RDG-F3 锚张力**（acceptance-author 通道）：帧 B 键帽「打断」移 keybar 单源后，现由帧 B 分支诊断日志字面承载；建议重钉为 keybar `KSTORY_RECORDING_GUIDE_KEYCAP` 宏消费链锚。
3. **帧 B 不可达**：全仓无置 `rec_substate=1` 的路径——引导播报中的「置位链 + 引导文案下发 wire」缺失属跨端改动（可能触契约），超出 fixer 边界。
4. **真机回归**：三现场均未真机复现（UAF 竞态 host 不可 1:1），status=`awaiting-human:real-device`；视觉终判（D3 三帧）归 human 看屏轮。
5. **现场 2（boot X 首页静置自崩）**：维持悬案，本补丁两条根修链不触及该场景。
6. **SdLog panic 捕获缺口**（零 backtrace 根因）：独立改进项。
7. **安抚句 ×3 重复下发**：front 推帧失败重试去重——host 侧无法定谳，需 ECS journalctl 对账，最小改法不显然，未改。
