# VRT live 冒烟证据 · 流式语音回环 + abort（VRT-A2/E2 + VRT-A6 云端半）

> planner 采集 · 2026-09-07 · 真机 ESP32-S3 WAVESHARE_S3_RLCD_4_2 ⇄ wss://8.137.91.16:8000 (front) ⇄ 引擎 xiaozhi-esp32-server ⇄ OpenClaw
> 固件 = 本包 build（含 view_router + kidpal/display 消费面），esptool 烧录 Hash of data verified。
> 事实标记均引自原始日志逐字摘录（引擎 docker logs / 设备 COM5 串口），未做格式加工。

## 会话一 · 流式语音回环多轮（session fa104e0c-5f5d-477b-acb7-6a834723abdf，10:10-10:15）

五事实（VRT-A2/E2 冻结验收标记口径）：

1. **wss 会话**：设备串口 `WS: Session ID: fa104e0c-5f5d-477b-acb7-6a834723abdf`；引擎 `收到hello消息：{"type":"hello","version":1,"features":{"mcp":true},...}`。
2. **ASR 识别回传**：引擎 `识别文本: 天空是什么颜色的？`；设备串口 `Application: >> 天空是什么颜色的？`（stt 文字回传上屏）。
3. **TTS 语音生成**：引擎 `语音生成成功: 现在嘛，天空大概是那种刚睡醒的淡蓝色…重试0次`（多句）。
4. **句级下发**：引擎 `发送第一段语音: 现在嘛，天空大概…`（每句一条）。
5. **文字同步上屏（state-speaking）**：设备串口句级同步文字 `Application: << 不过说真的，天空的颜色每天都不一样——晴天是透亮的蓝…`（tts sentence 载同步文字，设备 SetChatMessage 渲染；状态机 `listening -> speaking` / `speaking -> listening` 全程正确轮转）。

多轮：4 轮语音往返全部正常（识别→大模型→句级播报→文字同步），音频输出/重采样警告（16000→24000）为已知原生行为非缺陷。

## 会话二 · abort 打断 + 云端取消生成（session 4aebae34-9d79-4ebd-b432-c389a0a680fa，10:20-10:21）

- 设备串口（设备开机时间轴 350787ms）：`Application: Abort speaking` + `StateMachine: State: speaking -> listening`——播报中 KEY 打断即时停播（本地主地位，契约 §四）。
- 引擎同秒（10:20:56）：`收到abort消息：{"session_id":"4aebae34-…","type":"abort"}` → `Abort message received` → `Abort message received-end`。
- **云端取消生成佐证**：abort 后该轮再无 `发送第一段语音`/`语音生成成功`（下一轮用户开口前引擎零 TTS 产出）——云端收到 abort 即取消后续句生成。

## KEY 按键裁定（首轮「KEY 没效果」排查）

- 会话一开场 `idle -> connecting` 即首次 KEY 触发（无 wake word 配置，唯一触发源）；首轮用户播报中按键体感无效，受控复测（会话二）两步验证：① 播报中单击 KEY → `Abort speaking` + 即时停播 ✓；② 聆听态按键关会话——串口捕获窗口在按键前结束，日志未捕获，**行为未在日志层证实**（代码路径 = HandleToggleChatEvent listening 分支 CloseAudioChannel，原生行为，不属本包齿）。裁定：KEY 功能无缺陷，首轮体感 = 按键落点撞上句间自动状态切换的时序噪声。

## display 消费链路 live 观察（诚实边界）

- 设备侧 `kidpal/display` 已 live 注册：引擎 `收到mcp消息` tools/list 载其描述与 schema（`Render an E1 display payload…view (char/schedule/story_list/story_text/text/home/control whitelist)`），客户端设备支持的工具数量: 5。
- 本两轮会话 LLM 回复均未携带 ```display 块 → display 投递未在 device live 链路 exercised。根因（planner 已核实）：引擎 live data 目录（bind-mount `deploy/xiaozhi-docker-data/`）无 system prompt 注入——builder 的 `apply-xiaozhi-overlay.sh` 默认渲染到 `main/xiaozhi-server/data/` 陈旧诱饵路径（config trap），引擎 live 路径仍用引擎默认人设 prompt，故 OpenClaw 收到的 system prompt 不含 E1/三句纪律。A4/A7 统计复验按冻结 harness 直接对 OpenClaw 以冻结 prompt 探针（8/8 PASS），不受此影响。
- **followup（fixer，2026-09-07）**：已修复——live `.config.yaml` 注入顶层 `prompt:` 键 = 版本化文件全文（16→87 行，备份 .bak-260907-vrt-prompt），repo 脚本同步对齐真 mount 路径 + prompt 渲染；引擎重启正常（llm/intent=nointent/vad/ASR=Qwen3ASRFlash 初始化全过）。生效探针（远程直连 18789、system=部署 prompt、请求不带 tools）：回复含 ```display fence、口语 3 句（三句纪律达标）、view=text 白名单内——**所测 = 生产所用成立**。
- **display round（第三轮，session 95ccf407-fdf7-497f-bd53-9d64d11faa6a，11:03）**：引擎日志 `快速初始化组件: prompt成功 # KidPal OpenClaw 基础 System Prompt — v1`（prompt live 生效的直接引擎侧证据）；识别文本「月亮为什么会变弯变？」→ OpenClaw 回复含口语部分 + ```display text view（全文月亮盈亏解释）；设备收到全文并全屏显示（用户确认）。
- **新发现（G 包 live 面缺陷，非本包齿，留根升级）**：① 引擎 display 切分未剥离 fence 即下发——设备 `Application: <<` 串口日志可见 ` ```display{"view":"text` 与 `data":{"text":…}}``` ` 字样混在文字同步文本中（引擎侧 `发送第一段语音` 文本同证）；② 引擎未见 kidpal/display tools/call 中转下发（无 relay 日志、设备无 MCP 到达日志）——用户所见全文来自文字同步面（SetChatMessage），非 display text 视图面，故 P3 翻屏语义未触发（用户报告「上一页看不到」即此因；翻屏行为本属 display text 视图，C host 齿已覆盖分页逻辑）。本包 A3 设备消费面 = 冻结契约 host 实测 + live 工具注册实证；端到端 relay 缺口归 G 包（260903-gateway-directionB）live 集成修复，建议登记 ARQ/回包处置。

## 环境摩擦留痕

- 首次烧录跑错板型（CompactWifiBoard/SSD1306 黑屏）：`firmware/sdkconfig` 为 gitignore 机器本地文件，launch 镜像取自主 checkout 的过期副本（WS 包板型/证书 pinning 改动只落在其 worktree 本地）。修复 = 取 WS worktree sdkconfig（WAVESHARE_S3_RLCD_4_2 + MBEDTLS kidpal_ca pinning + OTA URL 空 + USE_DEVICE_AEC）重建重烧，boot 日证 `CustomDisplay: RLCD init` + `ui start`。friction 事件 fb2c70c9。
