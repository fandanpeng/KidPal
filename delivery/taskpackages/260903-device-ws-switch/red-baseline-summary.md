# WSA 红基线判定摘要（acceptance-author 2026-09-05 实测）

> 归位注记：本文件目标位置是
> `.claude/taskpackages/260903-device-ws-switch/red-baseline-summary.md`（对偶
> pod-s-handoff/red-baseline-summary.md 形态），但 acceptance-author 的 scope 边界
> 只放行 acceptance 面写入，`.claude/taskpackages/**` 被 enforce-scope 拦截。
> 故先落盘于此（`firmware/main/test/acceptance/red-baseline/`），planner / 有写权的
> 收口方请原样归位到上述路径后可删除本文件。

TaskPackage `260903-device-ws-switch`（设备切 WS + 方向 B 冒烟）冻结验收
`firmware/main/test/device_ws_switch.acceptance.c` 的红基线执行记录。

## 红基线命令与跑法

冻结验收文件的闸上 runner 判据 = host_app linux-target ELF（HOST_TEST_CONVENTIONS.md
[C4.4]），但 [C4.2] 接线（把 `../../device_ws_switch.acceptance.c` 加入
`host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS`）属于 builder 的 wiring 职责，
acceptance-author 写权限边界不含该 CMakeLists。为不交付「未验证验收」，本次用边界内
脚手架（本目录 `unity.h` stub + `runner.c`，非冻结件、非闸 runner）把**同一份冻结验收
文件**在 WSL Ubuntu-24.04 上编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /tmp/wsa_red_runner \
    ../../device_ws_switch.acceptance.c runner.c          # BUILD_CLEAN（零告警）
cd <repo root>
/tmp/wsa_red_runner                                        # exit code = 失败 TEST_CASE 数
```

实测：**8 Tests 6 Failures，ELF 退出码 = 6**（退出码纪律单独核实过：`MSYS_NO_PATHCONV=1
wsl ... -- /tmp/wsa_red_runner; echo $?` → 6；先前经 `bash -c` 包装测到的 0 是 wsl.exe
参数重组吞引号的伪影，非 runner 缺陷）。

另做闸上形态逼近核验：以**真 IDF unity 头**（`C:\esp\v5.5.1\esp-idf` components/unity，
`-DUNITY_INCLUDE_CONFIG_H -DCONFIG_UNITY_ENABLE_IDF_TEST_RUNNER=1` + 最小 sdkconfig 替身）
对冻结文件 `gcc -fsyntax-only` → `REAL_UNITY_SYNTAX_OK`，确认两参 `TEST_CASE`、
`*_MESSAGE` 断言宏、dirent/strings.h 用法在真构建链下成立。

## 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为，2026-09-05 实测） |
|---|---|---|
| WSA-A1（WS 切换实现面结构） | **RED** | `application.cc` 仍走 `CONFIG_USE_SHIM_PROTOCOL`→ShimProtocol（Pod S 形态）；`websocket` 命名空间只有 `websocket_protocol.cc` 读缝 + `ota.cc:167` 的 OTA 应答写入面，**无非 ota.cc 的配网直写入口**（spec §6.3 决定的地址入口缺失） |
| WSA-A2（配网直写 NVS + 短路激活） | **RED** | 配网直写入口缺失（同上）；token 生成（esp_fill_random/esp_random，决策 22 ≥128-bit）无处落；`application.cc` 仍含 `ota_->Activate()` / `ShowActivationCode`（tenclass 激活环未短路）且仍以 `HasWebsocketConfig` 从 OTA 应答取 WS 配置 |
| WSA-A3（wss 自签 CA 嵌入与加载） | **RED** | `firmware/main` 下无任何 `.pem/.crt`（自签 CA 未嵌入）；无 `cacert` 加载路径引用（V10 缺口）；`websocket_protocol.cc` 建连无证书接线（裸 `network->CreateWebSocket(1)`） |
| WSA-A4（方向 B 首链冒烟证据） | **RED** | 证据目录 `server/acceptance/_artifacts/device-ws-switch/` 不存在——flash.log / provision.log / smoke-roundtrip.log / screen 屏面证据全缺（方向 B 物理冒烟未跑，evidence-based 断言按性质红） |
| WSA-A5（HTTP 台阶退役） | **RED** | `protocols/shim_protocol.cc/.h`、`audio/mp3_decoder.cc/.h`、`AudioService::PlayMp3`、main CMakeLists 的 `shim_protocol`/`mp3_decoder` 源行、Kconfig `USE_SHIM_PROTOCOL`/`SHIM_*`、`application.cc` 的 `ShimProtocol`/`CONFIG_USE_SHIM_PROTOCOL` 全部在世；激活面文件（Kconfig.projbuild/application.cc）仍有 `tenclass` 字符串（Kconfig 默认 OTA URL = api.tenclass.net + application.cc:328 注记） |
| WSA-A6（安全收尾） | **RED** | `ecs-teardown-evidence.md` 未落盘（ECS shim 停服 + 8080 安全组关闭未执行/未记录）；`server/shim.py` 仍在世且无 ARCHIVED 墓碑；`changes/` 下无 shim.py 归档记录 |
| WSA-A7（supersede 归属注记） | GREEN（by construction） | 处置已由 acceptance-author 于 2026-09-05 执行完毕（两份文件 docstring 头注，diff 逐字核对仅 +13 行注记、断言零变更），断言守卫注记不被回删 |
| boundary/security（扫描器自检 + 守卫） | GREEN（必须保持绿） | 扫描器三态自检（有/无/不可读 + 必然空匹配）通过；`firmware/main` 无私钥材料；范围守卫成立（无 `openclaw`/`18789` 直连引用，Pod S A2− 对偶） |

## 红的构成一句话

**红 = A1–A6 六条各自对当前 story 缺失行为成立：固件仍指 HTTP shim（Pod S 形态）、
无配网直写入口、无自签 CA 加载路径、tenclass 激活门未短路、HTTP/mp3 死代码未清、
ECS/shim.py 收尾未做——方向 B 首链不存在；本包把它切到绿。** A7 与 boundary 为
by-construction 绿 / 持续守卫绿，不构成假红。

## 降级与留痕记录（诚实边界）

1. **闸 runner 未接线**：冻结文件尚未编入 host_app ELF（builder 一行接线，见 [C4.2]）。
   本次红以 stub runner 执行同一文件得出，断言行为与闸上一致；builder 接线后 verifier
   应在 host_app ELF 上复跑（期望转绿时 8 Tests 0 Failures + EXIT=0）。
2. **真机/live 面**：WSA-A4（烧录/配网/一轮流式往返/上屏）与 WSA-A6 ECS 腿按
   evidence-based 性质当前红（证据未落盘），其红因不是「测试跑不了」而是「行为未发生」。
3. **设备 target 编译**（WSA-A5 的「编译通过」尾巴）：host ELF 不证明芯片 target，
   由 builder/verifier 的设备构建闸承载（Kconfig/源面退役断言已在 A5 结构面钉住）。
4. 脚手架位置：本目录（`unity.h` stub + `runner.c`，均带非冻结声明头注；`test/` 目录
   不在验收扫描面也不在设备构建面，不产生自匹配/自编译）。

## 修订附注（acceptance-author 2026-09-05 续）

- **WSA-A2 CSPRNG 齿收紧（fixer mutation 后重派修订）**：符号存在匹配（`esp_random` 裸 `#include <esp_random.h>` 即可保持绿——即使真实 token 生成被换成 memset 常量）改为调用点语法匹配（`esp_fill_random(` / `esp_random(`，大小写敏感）。冻结文件头注已补修订记录行。复跑 host_app 闸 runner（WSL，MSYS_NO_PATHCONV=1 直取退出码）：**15 Tests 2 Failures，GATE_RC=2**——A2 仍绿（真实调用点在 `provisioning/ws_provision.cc:20` `esp_fill_random(bytes, sizeof(bytes));`，旁证核过；裸 include 不再命中），其余判定不变：13 PASS（A1/A2/A3/A5/A7/boundary + INF 组 + builder ws_provision 组）/ 2 FAIL（A4/A6，证据待落红）。其余断言语义未动，仅收紧，无 commit。

## 修订附注二（acceptance-author 2026-09-05 续）

- **WSA-A6 齿 3 first-match 缺陷修复（builder 扫描复刻发现，planner 重派修订）**：`wsa_scan_tree("changes","shim.py",…)` 单针 first-match 恒命中 `changes/arch/260902-arch-review-kidpal-spec-v0.2/adjudications.md`（含 shim.py 字样、不含 archiv，目录序恒先于记录件），齿 3 无论记录件落不落都永远红。改为双针匹配：扫描器新增 `needle2` 参数与 `wsa_scan_tree_both` 变体（同文件须同时含 `shim.py` 且含 `archiv`），A6 调用点切换；既有 `wsa_scan_tree` 调用点（A2 调用点扫描等）经 needle2=NULL 不受影响。冻结文件头注修订记录区已 append。复跑 host_app 闸 runner：**15 Tests 2 Failures 0 Ignored，GATE_RC=2**，终态不变；A6 红因只剩齿 1（`ecs-teardown-evidence.md` 落盘缺失，齿 3 未到达——记录件 `changes/dev/260903-device-ws-switch/shim-archive-record.md` 已由 planner 落盘，双针命中旁证核过），A4 红因 flash.log 不变，A7/A2 等其余全绿。无 commit。
