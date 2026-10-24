# scope-file · 261024-vq-overlay-close-end-session（影响面圈定清单）

> 产出方: integrator（Step 4 范围圈定，2026-10-24）。**本文件 = planner 终审 final 相位 verify.mjs `--scope-file` 输入**。圈定路径 = 三分法第一档正常圈定：per-Pod 专属图索引可用（detect_changes 实跑成功，project = `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`），无降级。
> 实测时点: 收口 commit 前工作树（git diff HEAD 未提交部分 + 本包待 commit 制品），merge_base = c4276adc（main）。

## 1. detect_changes 实测爆炸半径

- changed_files = 14（本包 5 firmware 文件 + sdkconfig + 包内制品 8）。
- seed_symbols = 96；**impacted_total = 5，零截断**：

| 符号 | 文件 | hop |
|---|---|---|
| CustomBoard | firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc | 2 |
| InitializeButtons | 同上 | 1 |
| app_main | firmware/main/main.cc | 1 |
| acceptance（boot_auto_connect） | firmware/main/test/boot_auto_connect.acceptance.c | 1 |
| acceptance（voice_round_ui） | firmware/main/test/voice_round_ui.acceptance.c | 1 |

- impacted_modules 汇总 = `firmware/main`（5）——**爆炸半径全部落在 firmware/main 预期面内，零出界符号**（server/**、协议层、device_state_machine、ToggleChatState inbound 102 均零触碰，与 plan ⑤ 负锚一致）。`firmware/sdkconfig` 为构建再生注释层漂移（非图内符号），见 §4。

## 2. test-scope 清单（final 相位 runner 面）

影响面合成（范围 = 本包 acceptanceTestsFile ∪ blast radius 映射测试 ∪ 共享 seam 相关 ∪ 安全余量 → host 全量套件）：

- **host 全量套件（237 Tests，OCE-A3 回归网）**——本包实测计数含新增 TEST 12；先存红 3 条（p1_auxbar AB-A1/A2/A4 @6e8425ff）按 knownRedFloor 豁免。
- 重点族（全绿判据）：`key_layer_dispatch` / `voice_round_ui` / `boot_auto_connect` / `record_session_live` 四族全绿；新增 `OCE-A1`（voice_round_ui TEST 12）与重钉齿 `KVR-A4 tooth2` / `RC-A2 tooth6` / `RC-B2`（sites 四项各恰 1）。
- 映射测试文件锚：`firmware/main/test/voice_round_ui.acceptance.c`、`firmware/main/test/boot_auto_connect.acceptance.c`（acceptanceTestsFile）；`key_layer_dispatch.acceptance.c` / `record_session_live.acceptance.c` 为共享 seam 回归族；host 全量 = 安全余量档（OCE-A3 冻结判据本就是全量零新增红，不缩水到单族）。
- 命令（红基线口径）：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- firmware/main/test/host_app/build/host_infra_test.elf`

## 3. 非 runner 面（final 相位一并核）

- **设备编译闸**（OCE-X3 / firmware-build）：本 Pod worktree r3 实测**降级**（工具链两路探测均不可用，buildAttempted=false，VERIFY_FW_BUILD_REQUIRED unset；degraded 放行非假绿）——final 相位如环境仍不可用，须如实降级留痕或回主线程环境补真编，不得按绿申报。
- **OCE-A4 真机目检六项**：human 持板，pending（`awaiting-human:real-device`），不计入 runner 通过面。

## 4. 判档预估 vs 收口实测双层对照

plan ⑤ 预估：3 实现文件（+2 验收文件）；设备面仅 waveshare-s3-rlcd-4.2 一板消费 OVERLAY_CLOSE（全仓 grep 唯一）。

| 项 | 预估 | 收口实测 | 判定 |
|---|---|---|---|
| 实现文件 | application.h / application.cc / waveshare-s3-rlcd-4.2.cc | 恰 3，零多改 | ✅ 一致 |
| 验收文件 | voice_round_ui / boot_auto_connect（+CMakeLists 仅当新增验收文件） | 恰 2；CMakeLists 零 diff（冻结回填 ⑥ 预判成立） | ✅ 一致 |
| 设备板面 | 仅 waveshare-s3-rlcd-4.2 消费 OVERLAY_CLOSE | detect_changes inbound 仅 CustomBoard/InitializeButtons（同板），其余板零触面 | ✅ 一致 |
| 出界符号 | ToggleChatState(102)/CloseAudioChannel/状态机/协议层均不改 | impacted 零出界（§1） | ✅ 一致 |
| 非图内观察 | — | `firmware/sdkconfig` 5 行注释块删除（Image Effects Configuration 空块，ESP-IDF 构建再生漂移，零功能语义；不在 changeManifest） | ⚠️ 出界文件· benign，已随本包 commit 收口并双向留痕（review-packet §6 / 本文件） |

**结论：无功能出界文件**；唯一清单外 diff = sdkconfig 注释层再生漂移（benign，如实申报）。圈定失败/缺清单情形未发生，无举手项。
