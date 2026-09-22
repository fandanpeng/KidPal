# Plan · 260923-m3-k6-boot-auto-connect

```yaml
rigor: lite
riskTier: medium
sourceType: null          # origin.type = human-confirmed-plan（无 sourceType 镜像，缺省）
reviewed-by: architect（triage 四档裁决 ②单包 Lite 2026-09-23；方案形状按 Shape A 修订——「标志互斥 + 状态恒 Idle」，Lite 判定绑定条件：不新增/不改设备状态机语义、不改 protocol.h 接口签名）
date: 2026-09-23
```

## 来源与立项

human 现场报告（2026-09-23）：开机/重启后设备与 Front 网关的 WS 连接不自动建立，需按一下 Key 才建连；导致①首页「接下来」无内容、②提醒投递链悬空。主线程已实证根因；human 立项 ack 已给（A-only：症状 B 课表页冷启动无数据另走设计入口，不入本包）。human 离席前预授权（2026-09-23）：「技术问题 call architect 分析判断」——architect 的 Shape A 形状级修订按该授权落，human 回席后补一句话 ack（留痕于此）。

**根因（主线程实证，代码锚）**：

- `WebsocketProtocol::Start()` 是空桩（websocket_protocol.cc:26-29，「Only connect to server when audio channel is needed」——小智上游惰性连接语义原样继承）。
- 真正建连 `OpenAudioChannel()` 只在四个人工交互点被调：`HandleToggleChatEvent`（application.cc:712 按键）、`HandleStartListeningEvent`（745）、`HandleWakeWordDetectedEvent`（784）、`WakeWordInvoke`（986）。
- boot 链路（`HandleNetworkConnectedEvent` → `ActivationTask` → `InitializeProtocol` → `HandleActivationDoneEvent` → Idle）无任何建连调用。
- server 侧按「设备开机即连」设计：front_app.py ws_gateway 建连即推首页「接下来」（课表三触发①，front_app.py:762-774）+ 注册提醒投递轮询。设备不连则推送与提醒链全悬空。
- 课表页（症状 B）根因更深：周网格缓存唯一填充源是语音轮 LLM display tool call（mcp_server.cc:643），三个推送触发均只推 home upcoming、从不推周网格——即使建连，冷启动直切课表页仍是空态。**B 不入本包**（推送形状/设备缓存语义涉及契约面或脆弱启发式，human 拍板另走设计入口）。

## 目标效果（一句话）

设备开机激活完成后无需任何人工交互即自动与 Front 建立 WS 连接（网络断开重连后同样自动恢复）；建连即生效的 server 推送链（首页「接下来」、提醒投递）在开机后立即可用。

## 范围裁定（human 立项 ack 2026-09-23）

- **A-only（选定）**：固件自动建连，server/front 零改动（建连推送已存在，设备连上即生效）。
- 症状 B（课表页冷缓存）不入本包——另走设计入口（可能触契约），独立立项。
- 触发面 = ①激活完成后一次 + ②网络重连时（Idle 且通道未开）。不做通道关闭后的无限重连风暴（有界退避，见方案）；重试耗尽后无周期兜底（慢速探活留后续独立小包，architect 3.4 裁定不入本包）。

## 方案形状（architect 裁定 Shape A：标志互斥 + 状态恒 Idle，弃 Connecting 互斥）

- **自动建连全程设备态保持 `kDeviceStateIdle`**，互斥用原子标志 `auto_connect_in_flight_`（新成员，application.h）。收益：零 UI 抖动（不触发 voice_begin/voice_end/聊天清屏）、零状态机事件、`device_state_machine.cc` 零触碰（保住 Lite 判据）、开机 UI 序列不变（首页稳落）。
- **「判 + 起」必须发生在主循环 handler 内**（触发点 1 = `HandleActivationDoneEvent` 末尾；触发点 2 = `HandleNetworkConnectedEvent` 的 Idle 支路），后台 task 只做 `OpenAudioChannel` + 收尾。主循环内串行判守卫，TOCTOU 竞态从根上消除（所有人工路径 handler 都在主循环）。
- **守卫三条件**：`state == kDeviceStateIdle && !protocol_->IsAudioChannelOpened() && !auto_connect_in_flight_`。
- **人工路径四处 call site 加 in-flight 守卫**：application.cc:712（ToggleChat）、:745（StartListening）、:784（WakeWordDetected，守卫放在 `EncodeWakeWord()` :780 之前）、:986（WakeWordInvoke——本板非活路径，守卫便宜、一并加）。
- **静默失败（quiet 路径）**：不动 protocol 层、不改 `OpenAudioChannel` 签名——在 Application 已有的路由点拦：`InitializeProtocol` 的 `OnNetworkError` lambda（application.cc:508-511）里判 `auto_connect_in_flight_`，命中则 log + return（不置 `MAIN_EVENT_ERROR`）。单文件、人工路径语义零改动、hello 发送失败同链覆盖。`SetError` 置位的 `error_occurred_` 无害（下次 `OpenAudioChannel` 开头 websocket_protocol.cc:94 复位），builder 勿画蛇添足复位。
- **重试**：task 内 3 次、5s/10s/20s 退避；每次重试前重查三条件，违反即 bail。
- **成功**：无任何状态迁移——通道开 + Idle，恰是「首轮对话后稳态」；`OnAudioChannelClosed → Schedule(Idle)`（application.cc:527-534）在 Idle→Idle 是 no-op，状态抖动结构性不存在。
- **回调线程加固**：`InitializeProtocol` 里 `OnAudioChannelOpened` lambda 体包进 `Schedule()`——自动路径下该回调在后台 task 线程执行，统一回主循环（人工路径本就在主循环，改动一行、延迟无害）。
- **task 模板**：沿用 activation task（application.cc:361-366：栈 `4096*2`、优先级 2、handle 成员退出置空）；task 退出前打一行 `uxTaskGetStackHighWaterMark` 日志（真机锚顺带核）。

## 关键接缝（architect 核验结论留痕）

- **弃 Connecting 互斥的两条独立理由（architect REBUT 记录）**：
  1. `kDeviceStateConnecting` 是承重 UI 语义非中性互斥位：进 Connecting 触发 `HandleStateChangedEvent`（application.cc:839-849）——`kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)` 清聊天 + `kidpal_render_node_voice_begin()`（kidpal_render_node.cc:528-553）弹掉 SERVER_PAGE/REMINDER 层、推 VOICE 前台层 = 语音轮 UI 接管。开机自动建连走 Connecting = 开机即弹语音轮演出。
  2. use-after-free 竞态：Connecting 态下 `HandleNetworkDisconnectedEvent`（application.cc:374-385）会调 `CloseAudioChannel()` → 主循环线程 `websocket_.reset()`，与后台 task 持同一 `websocket_` 在 Connect/SendText 内并发 = use-after-free。Shape A 下 in-flight 期间状态恒 Idle，该分支不命中，竞态结构性回避。
- **人工路径 TOCTOU**：主循环 handler 内「判 + 设」闭合（见方案形状），并发守卫不依赖状态位。
- **功耗/睡眠无冲突（architect CONFIRMED）**：`CanEnterSleepMode`（application.cc:1020-1034）仅被 power_save_timer.cc:64 / sleep_timer.cc:68 调用，两组件须板级实例化，waveshare-s3-rlcd-4.2 板零命中（死代码）。新语义差异：PERFORMANCE 功耗档从「首次交互后」提前为「开机默认」，电池产品有真实功耗差——有意为之，验收注记钉住。

## 契约核查（architect CONFIRMED 不触契约面；正式四分类权在 architect-reviewer，无触面信号）

interface-contract.md 已核：契约面 = E1 display JSON 形状（§一）/ B1 课表 MD（§二）/ 提醒推送语义（§三）/ 小智协议消息子集（§四）。**建连时机明文排除在契约外**——§1.3⑥「推送时机（WS 建连/每日零点/新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约」。本包复用既有 hello 握手，零新消息形状；front_app.py:758-773 建连推送已存在、零触碰。

## 验收方向（architect reviewed plan 机判锚）

1. **正锚·自动建连缝（host 可判半）**：needle 指向 = 守卫入口三条件、双触发点接线、spawn 判定在主循环 handler 内；**负 needle：自动路径零 `SetDeviceState(kDeviceStateConnecting)` 调用**（防 UI 接管回归的机判齿）。
2. **正锚·静默失败**：needle 指 `OnNetworkError` lambda 内 in-flight 拦截分支（不置 `MAIN_EVENT_ERROR`）；开机失败零错误卡、退避封顶 3 次、状态回 Idle。
3. **负锚·未配网/空 url**：自动建连入口预检 Settings("websocket") 的 url（token 同检）非空，空则单条 log + 完全 no-op（不起 task、不重试、不报错）。只读不写，namespace 对齐 websocket_protocol.cc:86-89。
4. **负锚·in-flight 生命周期**：`auto_connect_in_flight_` 在 task **所有退出路径**（成功/失败/bail）必清——泄漏 true = 人工按键永久 no-op + 自动建连永久失效，本形状唯一单点灾难模式，verifier 逐路径核对 + needle。
5. **人工路径守卫齿**：四个 call site（application.cc:712/:745/:784/:986）in-flight 守卫 needle；按键/唤醒语义零改动（分支行为 needle 零偏移）。
6. **回归**：既有 host 齿全绿零偏移（key_layer_dispatch / schedule_grid / p1_home_battery / time_sync 等）；boot UI 序列不变（首页稳落、无 VOICE 层推入——若 host 基建有 layer 断言面则入切片）；xiaozhi.bin device build 闸 + 体积余量记录。
7. **真机锚（挂设备轮窗口，human 在场，不阻塞 Pod）**：①冷启动无按键 → 串口见 WS connect + hello 交换、front 日志见建连推送发出、P1 显示「接下来」；②wifi 真实闪断重连 → 连接自动恢复（**避开 IsTimeout 路径**，见边角）；③server 宕机开机 → 设备静默落首页、零错误卡，此后按键人工建连仍可用；④AutoConnect task 栈高水位日志无溢出迹象。

## 非目标

不改 server/front（front_app.py 零触碰）；不推周网格（症状 B 独立立项）；不动按键/唤醒建连语义（人工路径保持现状，仅加 in-flight 守卫）；不动状态机（device_state_machine.cc 零触碰）与 protocol.h 接口签名（违反须回 triage 升 Full）；不加深睡/功耗策略；不做无限重连/周期兜底探活；不动 `docs/contracts/**`；不动 spec 冻结面；**不顺手修** `IsTimeout` 120s 覆盖式重连的既有行为（见边角，scope 纪律）。

## 边角（architect review 发现，builder 易漏）

- **`IsTimeout` 既有行为不修**：`IsAudioChannelOpened()` 含 `!IsTimeout()`（websocket_protocol.cc:78），`last_incoming_time_` 仅 OnData（:198）刷新——Idle 通道开无流量 >120s 后通道自报超时，下一次 network event 触发覆盖式重连（task 线程覆盖 `websocket_`，旧对象 task 线程析构）。**既有行为非新缺陷，本包不动**；真机锚 7-② 避开该路径，verifier 注记。
- **in-flight 期间人工交互窗口**：单次尝试最长 ~10s、全程最坏 ~35s 内人工按键/唤醒 no-op——Shape A 已知代价，"AutoConnect" tag log 足迹钉住；重试间重查三条件保证不会更糟。
- **功耗档时序**：激活收尾 `SetPowerSaveLevel(LOW_POWER)`（application.cc:406）先于建连成功回调 `SetPowerSaveLevel(PERFORMANCE)`（application.cc:519-525）——后到覆盖，行为可接受、验收注记（非回归）。
- **预检读 Settings 只读**：namespace "websocket"，不得复制写逻辑（写入口 = WsProvision::EnsureWebsocketToken，activation 期已跑）。

## 调度注记

常驻树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、分支 `dev/260915-m1-k1a-key-layer-dispatch`，K5 已合并收尾（b869658）——本包排入该树第 10 棒。真机半（锚 7）挂设备轮窗口，human 在场协调按真机轮协议（单一长捕获 + 就绪信号开窗）。
