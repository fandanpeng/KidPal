# Pod S (260829-smoke-e2e) 连续性交接（在 worktree 重开会话用）

> 2026-09-01 主线程（原会话）写。human 已拍板：子 agent 锚点问题走「在 worktree 重开会话」路线。
> 新会话第一句话建议：「续跑 Pod S（260829-smoke-e2e），先读 `.planning/kidpal-mvp/pod-s-handoff/`，按 planner 流程从 Step 7（acceptance-author 落盘）继续」。

## 0. 新会话启动方式（给 human）

```
cd C:\AI\KidPal-pod-smoke-e2e
claude          # 或 claude --agent planner
```
在 worktree 根启动后，`CLAUDE_PROJECT_DIR` = worktree，子 agent 的 enforce-scope 锚点即正。

## 1. 状态快照

- worktree：`C:/AI/KidPal-pod-smoke-e2e`，分支 `dev/260829-smoke-e2e`；`devloop/pods/260829-smoke-e2e.json` = running、mergeOrder 1、worktreePath 已登记。
- TaskPackage：`.claude/taskpackages/260829-smoke-e2e/taskpackage.json`（intake 完成版，含 9 条承重 notes；`current/taskpackage.json` 已同步）。
- 事件流：`.claude/taskpackages/260829-smoke-e2e/pod-events.jsonl`（已有：human-decision×2、agent-escalation×1、agent-escalation-resolved×1）。
- 阶段：**Step 7 进行中**——acceptance-author 已产出冻结验收全文 + 红基线逐条判定（本目录 `frozen-acceptance.py`、`red-baseline-summary.md`），但落盘被锚点 bug 拦住。新会话动作 = 重派 acceptance-author 把 `frozen-acceptance.py` 内容权威落盘到 `server/acceptance/test_smoke_e2e.acceptance.py`（让它核对后原样写，勿由主线程代写，保验收归属），跑红基线、回填 `redBaselineCommand/redBaselineResult` → Finalize TaskPackage → 派 builder。
- planner.md Step 5b 自检已过（无 return/propose-drop/notify 触发）；Step 1c scope-map 核对已过（见 taskpackage notes ③）。

## 2. 验收未决点（acceptance-author 留下，需 planner/人裁）

1. **SHIM_DEVICE_TOKEN 是否在 live 阶段置位**（安全轴 public-exposure-and-device-token）：验收已自适应（无 token 时 401 测试自跳过）。建议置位（公网暴露的冒烟更应带 token）——置位后 401 断言变硬闸。
2. **ECS shim 的日志汇点**：`test_a2_live_shim_receives_device_request` 假定 systemd unit `kidpal-shim`（仓里有 `server/kidpal-shim.service` 单元文件，部署时对上了就用它）；若换日志形态需调该断言命令。
3. E1 的绿牙来自小智基座（板 overlay + 16m 分区表），红牙 = shim 未进 build；串口状态机日志归 E2 burn.log 承接，未进 E1 机械断言（合理，不动）。

## 3. 环境既成事实（勿重复排障）

1. **IDF 构建通路（已打通）**：Git Bash 的 `MSYSTEM` 戳被 IDF 拒（`idf_tools.py:__main__` 硬检 `os.environ['MSYSTEM']`，且 MSYS 运行时会给每个子进程重新盖戳，`env -u` 无效）；解法 = 在原生 python 进程内先 `os.environ.pop('MSYSTEM')` 再套 `idf_tools.py export --format=key-value` 的环境。**可用启动器：`c:\tmp\idf_run.py <idf.py 参数...>`**（实测 `--version` = ESP-IDF v5.5.1 通过）。新会话第一步先确认该文件还在（Temp 目录），不在就按本条重写（逻辑 10 行）。**调用纪律（260829-char-stroke 实证，3+ Pod 踩坑）**：必须用 IDF venv python 启动——`"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build`（在 `firmware/` 目录下跑）；裸 `python` 启动会以 `No module named 'esp_idf_monitor'` 失败（wrapper 用 `sys.executable` 跑 idf.py，报错形似 venv 损坏、实为解释器用错）。2026-09-08 起 wrapper 已加自守卫：非 venv python 启动自动 re-exec 到 venv python，裸 `python` 形态亦可直接用；详见 skill `kidpal-idf-env`。
2. `firmware/managed_components` = junction → 主 checkout 同名目录（640MB 共享，gitignore，新增组件落进去无害）；`firmware/dependencies.lock` 已复制进 worktree。
3. ECS 8.137.91.16：OpenClaw 网关 18789 **在线**（2026-09-01 TCP 实测）；shim 未部署（8080 关闭，live 阶段需部署 + 安全组开端口 + token）；SSH 经 `c:\tmp\ssh_helper.py`（paramiko 密码认证，TP-A 先例）。
4. **板子（保持插着！）**：ESP32-S3 rev v0.2 / 16MB flash / 8MB PSRAM / MAC 58:e6:c5:72:0c:bc，**COM5** = 原生 USB（USB-Serial-JTAG），现为出厂固件 03_FactoryProgram（烧录即覆盖，无需保留）。探测脚本：`C:\Users\15802\AppData\Local\Temp\serial_sniff2.py`（开串口→DTR/RTS 复位→抓启动日志，可复用于烧录后验证）；esptool 走 `~/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe -m esptool --port COM5 <cmd>`（注意这版命令用下划线：chip_id/read_mac/flash_id）。
5. 固件闸 `firmware-build`：钩子里 `idf.py` 不在 PATH → 按脚本设计降级放行（诚实降级、非假绿）；真编译证据由构建产物/烧录日志承载。
6. 板型切换：`firmware/main/Kconfig.projbuild` 定义 `CONFIG_BOARD_TYPE_*`，`firmware/main/CMakeLists.txt` 按它挑板级源文件；当前 `firmware/sdkconfig` 是 BREAD_COMPACT_WIFI，builder 需改置 `CONFIG_BOARD_TYPE_WAVESHARE_S3_RLCD_4_2=y`（waveshare 板 Kconfig 项已存在）。
7. 主仓现成 `firmware/build/xiaozhi.bin` 是 bread-compact 板型且已过期——**不可用**，勿误烧。

## 4. 设备侧实现的落点指针（图探索成果，喂 builder）

- 协议选择：`firmware/main/application.cc` `Application::InitializeProtocol`（469-606 行，`ota_->HasMqttConfig()/HasWebsocketConfig()` 分支处加第三分支或冒烟期编译期开关）；`OnIncomingJson` 处理器说 xiaozhi 消息语言（tts/stt/llm/…），shim 的 `{audio, display}` 需翻译成这些回调（屏显一行可借 `{type:"tts",state:"sentence_start",text:...}` → `display->SetChatMessage`）。
- OTA 依赖：xiaozhi 的 OTA 配置拉取指向官方后端——冒烟固件应**编译期给定 shim 基址**（Kconfig/NVS 皆可，验收只认「指向 shim、不出现 18789/openclaw」），别走 OTA 拉配置路径。
- 音频：`AudioService::PushPacketToDecodeQueue`（audio_service.cc:412）走 Opus 解码队列；下行 mp3 需新增解码路径（候选：managed component `chmorgan/esp-libhelix-mp3` 或 espressif/esp-adf-libs 的 mp3 decoder；加进 `firmware/main/idf_component.yml`），解码后 PCM 找 `AudioOutputTask`/codec `Write`（板用 BoxAudioCodec，box_audio_codec.cc:240）注入。
- 按键：板级现只有 BOOT(GPIO0)→ToggleChatState（waveshare-s3-rlcd-4.2.cc）；spec 的 KEY 键位查参考树（只读）确认后接线，冒烟期用哪个键触发会话要在证据里写明。
- 录音上行：xiaozhi 音频管线录音是 Opus 帧；上行要 WAV PCM16 16k mono（协议层攒帧组 WAV 即可，勿改声学管线）。

## 5. 根因记录（锚点 bug，供将来治理批参考）

子 agent（.claude/agents 自定义型）的 Write/Edit 钩子进程里 `CLAUDE_PROJECT_DIR` 被 harness 钉在**会话启动目录**（main 仓），`EnterWorktree` 不改它；enforce-scope 用它做 projectDir → worktree 绝对路径算出 `../` 前缀 → 全部 allow 失配被拒。主线程（无 agent_type）不受 per-agent 约束，通用子型（'claude'）未注册也不受约束，故只有自定义角色中招。本 Pod 用「在 worktree 重开会话」解；若将来要做成机械修法（钩子按 devloop/pods 登记感知 worktree），属治理变更走 /arch-change 或 ARQ。
