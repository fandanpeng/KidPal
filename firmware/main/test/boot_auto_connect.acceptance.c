/*
 * Frozen acceptance tests — TaskPackage 260923-m3-k6-boot-auto-connect (subsystem:
 * firmware; M3-K6; 开机自动建连 · rigor=lite, riskTier=medium).
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1）——设备开机激活完成后自动与 Front
 * 建立 WS 连接（网络断开重连同样自动恢复），使 server「设备联网时」推送触发（S-F1.1
 * 补充规则明文：刷新触发 = 设备联网时 + 每日零点 + 新建日程后即时）与提醒投递链开机
 * 即生效。技术方案 = plan.md（architect Shape A：标志互斥 + 状态恒 Idle，triage ②
 * 单包 Lite 2026-09-23）。语义权威 = plan.md；验收语义 = taskpackage acceptance[]
 * K6-A1..A7 与 mutationEvidence.perAcceptance。
 *
 * 根因（plan.md 实证锚）：WebsocketProtocol::Start() 是空桩（websocket_protocol.cc:
 * 26-29 惰性连接语义），真正建连 OpenAudioChannel() 只在四个人工交互点被调
 * （application.cc HandleToggleChatEvent / HandleStartListeningEvent /
 * HandleWakeWordDetectedEvent / WakeWordInvoke）；boot 链路
 * （HandleNetworkConnectedEvent → ActivationTask → InitializeProtocol →
 * HandleActivationDoneEvent → Idle）无任何建连调用 → 开机不连，front 建连推送
 * （front_app.py:762-774）与提醒投递全悬空。
 *
 * ===========================================================================
 * Frozen seam contract pinned by this file（builder implements，本文件只读扫真实
 * 工作树——Shape A 全部落点 application.cc / application.h，零新源文件、零协议层
 * 触碰；「home =」为冻结落点）:
 *
 *   [S1] 自动建连缝（home = firmware/main/application.cc + application.h）:
 *     - application.h 新成员 `auto_connect_in_flight_`（bool 或 std::atomic<bool>，
 *       needle 字面不变）+ 私有方法声明 `void TryStartAutoConnect();`。
 *     - application.cc 定义 `void Application::TryStartAutoConnect()`（「判 + 起」
 *       入口，由两个触发点在主循环 handler 内同步调用——所有人工路径 handler 都在
 *       主循环，TOCTOU 判设闭合，plan 关键接缝）。内部序:
 *       ① 预检 Settings("websocket", false) 只读读 url（token 同检），任一空 →
 *          单条 log + return（完全 no-op：不起 task、不重试、不报错）;
 *       ② 守卫三条件: GetDeviceState() == kDeviceStateIdle &&
 *          !protocol_->IsAudioChannelOpened() && !auto_connect_in_flight_;
 *       ③ 置 auto_connect_in_flight_ = true;
 *       ④ xTaskCreate 后台 task（task 名 "AutoConnect"，沿用 activation 模板
 *          application.cc:361-366: 栈 4096*2、优先级 2、lambda 直排——不阻塞主循环）。
 *       task 体（lambda 内）: 3 次重试、退避经 [S3] kAutoConnectBackoffS、每次尝试
 *       前重查守卫三条件（违反即 bail）、所有退出路径（成功 / 重试耗尽 / bail）必清
 *       auto_connect_in_flight_、退出前打一行 uxTaskGetStackHighWaterMark 日志
 *       （plan 任务模板，真机锚 A7-④ 顺带核）。
 *     - 触发点 1 = HandleActivationDoneEvent 函数区内调 TryStartAutoConnect()（末尾
 *       接线）；触发点 2 = HandleNetworkConnectedEvent 函数区 Idle 支路（新
 *       kDeviceStateIdle 分支）调 TryStartAutoConnect()。
 *     - 设备态纪律（Shape A 核心）: 自动建连全程状态恒 kDeviceStateIdle——自动缝
 *       相关区（TryStartAutoConnect 函数区，含 task lambda）内零 `SetDeviceState(`
 *       调用；全 application.cc `SetDeviceState(kDeviceStateConnecting)` 计数恒 4
 *       （四个既有人工 call site，2026-09-23 冻结基线逐行实证 :711/:744/:783/:985）
 *       ——防语音轮 UI 接管回归（进 Connecting → HandleStateChangedEvent →
 *       kidpal_layer_chat_clear_event(NEW_VOICE) + voice_begin 弹层推 VOICE 前台 =
 *       开机即语音轮演出）与 use-after-free 竞态（Connecting 态下
 *       HandleNetworkDisconnectedEvent 会 CloseAudioChannel → 主循环 websocket_.reset()
 *       与后台 task 并发，plan REBUT 记录）的机判负 needle。
 *
 *   [S2] 静默失败缝（home = application.cc InitializeProtocol 的 OnNetworkError
 *        lambda）: lambda 内 auto_connect_in_flight_ 拦截分支（命中则 log + return，
 *        不置 MAIN_EVENT_ERROR、不弹错误卡），分支位于 MAIN_EVENT_ERROR 置位语句
 *        之前（位置序 needle）。失败全程设备态保持 Idle（[S1] 零 SetDeviceState 承
 *        载）；人工路径错误路由零改动（MAIN_EVENT_ERROR 仍在世，非拦截面）。
 *
 *   [S3] 有界退避决策表（K5 留根常识②: host 齿钉决策表形态，不钉实时时序——时序归
 *        真机窗 K6-A7）: application.cc 冻结命名常量
 *        `kAutoConnectBackoffS`，冻结形态 = 3 元素递增秒值表 `{ 5, 10, 20 }`
 *        （封顶 3 次、5s/10s/20s 递增序列，plan 方案形状），定义 + task 内消费
 *        >= 2 处。每次重试前重查三条件 = [S1] task 体三条件 token 下界承载。
 *        （命名常量是本冻结件的缝契约裁定——与 K5 kidpal_ts_* 先例同款：给 mutation
 *        敏感性与决策表形态一个可机判的锚；换算 vTaskDelay 的乘法形态实现自由。）
 *
 *   [S4] 预检只读缝（K6-A3 负锚）: 预检用 `Settings settings("websocket", false)`
 *        显式只读形参（与 websocket_protocol.cc:86 读形态逐字同款）；缝区内零
 *        Settings 写 API（SetString / SetInt / SetBool / EraseKey / EraseAll 零命中
 *        ——写入口 = WsProvision::EnsureWebsocketToken，activation 期已跑，plan 边角）。
 *
 *   [S5] 人工路径守卫（K6-A5）: 四个 OpenAudioChannel call site 函数区
 *        （HandleToggleChatEvent / HandleStartListeningEvent /
 *        HandleWakeWordDetectedEvent / WakeWordInvoke）各接 in-flight 守卫
 *        （auto_connect_in_flight_ needle 进各区）；WakeWordDetected 区守卫置于
 *        EncodeWakeWord() 之前（plan 关键接缝）；按键/唤醒既有分支行为语义零改动
 *        （各区既有 `SetDeviceState(kDeviceStateConnecting)` + `OpenAudioChannel()`
 *        在世锚零偏移）。
 *
 *   [S6] 回归 + 铁律（K6-A6 / K6-E2）: OnAudioChannelOpened lambda 体包 Schedule()
 *        （回调统一回主循环，plan 方案形状）；本验收件接线 HOST_ACCEPTANCE_SRCS
 *        （smoke 末位序守卫）；protocol 层零触碰（Start() 空桩惰性注释在世 +
 *        IsAudioChannelOpened 的 !IsTimeout() 120s 覆盖式重连既有行为不顺手修）。
 *
 *   [S7] 本地关分类标记缝（home = firmware/main/protocols/websocket_protocol.h/.cc,
 *        261010-device-reconnect-on-server-close 包新增; 关闭事件分类的协议层载体）:
 *     - websocket_protocol.h 新成员 `bool local_close_pending_ = false;`（needle 字面
 *       不变; 「本地主动关」分类标记）。
 *     - CloseAudioChannel() 区内 `local_close_pending_ = true;` 置位先于
 *       websocket_.reset()——reset 析构若回调 OnDisconnected（plan 实现前核项）,
 *       标记已覆盖析构路径。
 *     - OpenAudioChannel() 区内 `local_close_pending_ = false;` 清位——每条新连接
 *       基线 = 非本地关, 上一连接的陈旧标记不污染新连接分类。
 *     - OnDisconnected lambda 区: 先消费标记（本地关 = Application 主动 close, 不调
 *       on_audio_channel_closed_ → A 路径零触发）再走服务端/网络侧断分支调回调——
 *       消费位次先于回调（分类互斥的协议层半）。
 *     - 全 ws.cc `local_close_pending_ = true` 恰 1 处（分类写点单点 = CloseAudioChannel;
 *       第二写点 = 分类面被旁路）。
 *
 *   [S8] A 路径触发缝（home = application.cc OnAudioChannelClosed lambda 区）:
 *     - lambda 区内 Schedule 排入 TryStartAutoConnect() 恰 1 处——非本地关 = 服务端/
 *       网络侧断, 自动重连; 复用既有 AutoConnect task 守卫与退避（零第二条 task）。
 *     - lambda 区内 OpenAudioChannel( / ScheduleResidentRebuild() 恒 0——连接只经
 *       既有 AutoConnect task; A 区永不调 B helper（分类互斥的 application 半）。
 *     - lambda 区内 SetDeviceState( 恰 1 处——既有 Idle 收尾零偏移; 新增即屏显触碰,
 *       requirements.md:67「重连尝试期间维持 Idle 屏显」语义。
 *
 *   [S9] 常驻重建缝（B 路径, home = application.h + application.cc 新私有方法）:
 *     - application.h 声明 + application.cc 定义 `void ScheduleResidentRebuild();`。
 *     - application.cc 冻结命名常量 `kResidentRebuildDelayS`，冻结值 = 5（T 秒, plan
 *       「退避表首档 5s 量级」定值冻结进验收件; 修改须回 planner 重冻结; 定义须为
 *       该 needle 在 application.cc 的首次出现——定义片段切片判读前提）。
 *     - helper 区内: 消费 kResidentRebuildDelayS + 排入 TryStartAutoConnect()（复用
 *       守卫三条件与退避——重建连接只经既有 AutoConnect task）+ 经 Schedule( 回主循环
 *       （判+起在主循环上下文, 与 K6 [S1] 判设同源）; 区内 OpenAudioChannel( /
 *       kAutoConnectBackoffS / SetDeviceState( 恒 0——零第二条重连路径 / 零新退避
 *       消费 / 屏显零触碰。
 *     - 三个会话收尾类本地关调用点（plan 归类表: application.cc :805
 *       Application::CloseAudioChannel（K3 [S5] 录音态丢弃面）/:849
 *       HandleToggleChatEvent Listening 分支/:1189 WakeWordInvoke Listening 分支;
 *       builder 复核 :1189 语义后留痕为验收前提）各调 ScheduleResidentRebuild() 恰
 *       1 次; 各区 TryStartAutoConnect() 恒 0（触发只经 helper——同点直调 = 双触发）。
 *       全 application.cc ScheduleResidentRebuild() 恰 4 处（定义 + 三调用点, 零泄漏）。
 *
 *   [S10] 五负锚路径（plan 归类表零重建类）: HandleNetworkDisconnectedEvent（:422
 *       网络断关, 恢复走既有 K6 触发点 2）/ Reboot（:1081）/ UpgradeFirmware（:1100
 *       OTA）/ SetAecMode（:1254）/ ResetProtocol（:1267）五函数区内
 *       ScheduleResidentRebuild() 与 TryStartAutoConnect() 恒 0; 各区既有 close 语义
 *       token 在世锚（既有语义零偏移）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K6-A1 自动建连缝正锚 -> test 1（守卫入口三条件 token + 双触发点接线 +
 *             主循环 spawn 判定 + task 化 xTaskCreate/"AutoConnect" + 负 needle:
 *             Connecting 计数恒 4 / 缝区零 SetDeviceState）。真机半（冷启动建连成功
 *             事件序列）= deferred 设备轮（K6-A7①）。
 *   coverage: K6-A2 静默失败锚 -> test 2（OnNetworkError in-flight 拦截分支 + 置位
 *             前序 + 退避决策表形态: 封顶 3 / 递增 5,10,20 / 重试前三条件重查下界 +
 *             失败全程 Idle）。真机半（server 宕机静默落首页、退避节奏实测）=
 *             deferred 设备轮（K6-A7③ + 时序）。
 *   coverage: K6-A3 未配网/空 url 负锚 -> test 3（预检只读形参逐字 + 写 API 零命中 +
 *             空检查存在 + no-op 前序: 空检查先于 xTaskCreate + protocol 读形态
 *             在世）。
 *   coverage: K6-A4 in-flight 生命周期负锚 -> test 4（task 区清标志 needle + 置位/
 *             重查/清三态下界 + 栈高水位日志 + activation 模板在世）。全退出路径
 *             逐路径核对 = verifier 义务（host 扫描无法穷尽控制流，头注强度申报
 *             显式声明该缺口）。
 *   coverage: K6-A5 人工路径守卫齿 -> test 5（四 call site 区 in-flight needle +
 *             WakeWordDetected 守卫先于 EncodeWakeWord 序 + 既有分支语义零改动锚）。
 *   coverage: K6-A6 回归 + device build 闸 -> test 6（OnAudioChannelOpened Schedule
 *             包裹 + HOST_ACCEPTANCE_SRCS 注册序 + 既有齿注册在世 + protocol 零触碰
 *             + 需求契约 canary + 扫描器自检）；既有齿零偏移由红基线通道①全量实跑
 *             承载；device build 闸（xiaozhi.bin + 体积余量）= builder/verifier
 *             相位义务（头注 DEFERRED 清单）；boot UI 序列不变（首页稳落、无 VOICE
 *             层推入）host 半 = Connecting 计数恒 4 负 needle 承载，真机目测 =
 *             K6-A7①。
 *   coverage: K6-A7 真机锚（四子锚）-> 冻齿侧零 host 齿，仅本头注 deferred 登记
 *             （①冷启动无按键建连 + front 推送 + P1「接下来」；②wifi 闪断自动恢复、
 *             避开 IsTimeout 120s 路径；③server 宕机开机静默零错误卡、此后人工建连
 *             可用；④AutoConnect task 栈高水位无溢出）。挂设备轮窗口，human 在场，
 *             不阻塞 Pod（K6-E3）。
 *   coverage: RC-A1 服务端断自动重连 -> test 7（[S7] 分类标记三态: 置位先于 reset /
 *             清位 / 消费先于回调 + [S8] A 区 Schedule TryStartAutoConnect 恰一次;
 *             既有 Idle 收尾零偏移锚）。真机半（服务端断退避窗内自发 hello 建连序列 +
 *             P1 屏显不变）= deferred 设备轮（RC-A4）。
 *   coverage: RC-A2 分类互斥 -> test 8（分类写点单点 + OnDisconnected 消费先于回调 +
 *             A 区零 ScheduleResidentRebuild + 三会话收尾点零直调缝 + 全文件 helper
 *             恰 5 处——261024-vq-overlay-close-end-session 重钉 4→5, 定义 + 四调用点;
 *             第四调用点 HandleEndVoiceSessionEvent 未纳入零直调缝守卫 = 按该包
 *             plan ⑧-2 冻结口径, planner 可另案收紧）。真机半 = deferred 设备轮
 *             （RC-B4/B5 对账）。
 *   coverage: RC-A3 复用守卫 -> test 9（B helper 区复用 TryStartAutoConnect + 经
 *             Schedule( 回主循环 + 零 OpenAudioChannel/零退避消费/零 SetDeviceState
 *             + kResidentRebuildDelayS 定义消费下界 + A 区零 OpenAudioChannel）。
 *   coverage: RC-B1 永不休眠 -> test 10（板级文件零休眠接线 needle + 直连 sleep API
 *             零命中 + common 层休眠入口文件在世 canary + CanEnterSleepMode 既有门
 *             在世）。设计态绿（现状结构事实钉死; 变异敏感 = 板级加接线即红）。
 *   coverage: RC-B2 常驻重建 -> test 11（T=5 冻结进首定义片段 + 四会话收尾点各恰
 *             一次——261024-vq-overlay-close-end-session 追加 HandleEndVoiceSession-
 *             Event 第四项 + 各区既有 close token 零偏移锚）。真机半（收尾 T 秒内
 *             重建、跨夜提醒可达）= deferred 设备轮（RC-B4）。
 *   coverage: RC-B3 负锚 -> test 12（五路径零重建排程 + 既有 close token 在世）。
 *             实现前 trivially green（负锚在 B 缝在世后才承载）; 变异敏感 = 任一负锚
 *             区新增重建排程即红。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载齿 -> 性质档；acceptance-evidence T2 纪律）:
 *   S-F1.1 AC1「设备联网时」推送触发可用 -> K6-A1 齿组 -> 机制级（双触发点接线 +
 *     守卫三条件 token + task 化 xTaskCreate 结构在世）+ 源锚代理（Connecting 计数
 *     恒 4 = 零 UI 接管的文本代理）。行为等价假设（显式化）:
 *     ① 「helper 由主循环 handler 同步调用 ≙ 判+起在主循环内」——依据 = 两个触发点
 *        均为 Run() 主循环事件 handler（handle 事件分派序），TryStartAutoConnect 同步
 *        执行于 handler 栈; 若 builder 把判+起挪进后台 task，tooth（触发点区 needle）
 *        仍绿——该假设由「守卫与 xTaskCreate 同处 TryStartAutoConnect 区」的结构钉
 *        部分兜住（区在 handler 调用栈内），完全兜底 = verifier 核对调用序。
 *     ② 「Connecting 计数恒 4 ≙ 自动路径零 Connecting」——依据 = 自动缝区内零
 *        SetDeviceState( 更强钉; 若 builder 在缝区外新增自动路径 Connecting 调用，
 *        计数齿红（敏感）。
 *     ③ 「Settings("websocket", false) 文本在世 ≙ 预检只读」——源锚代理; 真实只读
 *        由 NVS read_write_ 标志承载（settings.h），文本代理与协议文件同款逐字。
 *   K6-A2 静默失败 -> 行为级弱代理 + 源锚代理: 拦截分支 needle 是结构在世钉，host
 *     无法执行 lambda 行为; 弱于契约处缺口 = 「错误卡不弹」「状态保持 Idle」端到端
 *     仅真机可证（K6-A7③），显式挂账。退避决策表 = 源锚代理（命名常量表），
 *     「递增/封顶」形态由表值 {5,10,20} 冻结承载，实时节奏归真机（K5 留根②纪律）。
 *   K6-A4 in-flight 生命周期 -> 源锚代理（清标志 needle + 三态下界）; 弱于契约处
 *     缺口 = host 扫描无法穷尽控制流退出路径，「所有退出路径必清」的完全证明 =
 *     verifier 逐路径核对（acceptance 明文指定）+ 真机泄漏表象（按键永久 no-op）
 *     归 K6-A7 窗口可观测，显式挂账。
 *   K6-A5 人工路径守卫 -> 机制级（四函数区 needle + 序钉）; 既有语义零改动 =
 *     各区既有 token 在世锚 + 通道①零偏移双承载。
 *   K6-A6 回归 -> 机制级（注册序 + protocol 零触碰文本锚）; 行为等价假设（显式化）:
 *     「Start() 空桩注释在世 ≙ protocol 层零触碰」——廉价文本卫兵，主承载 = 通道①
 *     既有齿全绿零偏移（行为面）。
 *   RC-A1/A2/B2 -> 机制级（分类标记置位/清位/消费位次 + 触发点接线恰一次计数）+
 *     源锚代理（既有语义 token 在世）; 弱于契约处缺口（显式挂账）: ①「同一关闭事件
 *     恰归一类」的运行时判定（reset 析构是否回调 OnDisconnected、异步时序）host 扫描
 *     不可证 = verifier 逐点核对 + RC-A4/B4 真机窗; ②「T 秒内重建」的实时节奏 host
 *     不钉（K5 留根②纪律: 钉 T 定值冻结 = 5, 节奏归真机 RC-B4）; ③「判+起在主循环
 *     上下文」由 helper 区 Schedule( 在世承载文本级, 完全兜底 = verifier 核对调用栈。
 *   RC-A3 -> 机制级（复用缝 needle + 负 needle: 零新退避消费 / 零直连
 *     OpenAudioChannel( / 零 SetDeviceState(）; 屏显零触碰的完全证明 =
 *     requirements.md:67 真机回归（RC-A4）。
 *   RC-B1 -> 源锚代理（结构事实钉死; 板级零休眠接线是「永不休眠」的编译面载体）。
 *     行为等价假设（显式化）: 「板级零休眠接线 且 common 层入口不接 ≙ 永不休眠」
 *     ——若板级任何代码 include sleep/power_save 头或起 timer, needle 红; Wi-Fi
 *     modem 省电档兼容性不在本齿面（RC-B6 观测项, 结论进 docs-delta）。
 *   RC-B3 -> 负锚机制级（五区零重建 needle）; 实现前 trivially green 申报: 负锚齿在
 *     B 缝在世前不承载, 变异敏感 = 任一区加排程即红。
 *
 * ---------------------------------------------------------------------------
 * DEFERRED（真机半 + 相位义务，不阻塞 host 半收口）:
 *   - K6-A7 四子锚真机实证（设备轮窗口，human 在场；K6-E3）: ①冷启动无按键: 串口见
 *     WS connect + hello 交换、front 日志见建连推送发出、P1 显示「接下来」; ②wifi
 *     真实闪断重连自动恢复（避开 IsTimeout 120s 路径）; ③server 宕机开机: 静默落
 *     首页、零错误卡、此后按键人工建连仍可用; ④AutoConnect task 栈高水位日志无溢出
 *     迹象。冻齿侧零 host 齿、不伪造 host 证据。
 *   - K6-A6 device build 闸: xiaozhi.bin 构建过 + 体积余量记录（builder/verifier
 *     相位义务，非 host 齿）。
 *   - mutation 实跑: mutationEvidence.perAcceptance 六条 expectedRed 补跑（builder/
 *     收尾相位），原始输出逐 acceptance 落 delivery/taskpackages/
 *     260923-m3-k6-boot-auto-connect/mutation-evidence/（K5 留根欠账①）。
 *   - RC-A4 / RC-B4 / RC-B5 / RC-B6（261010 包真机轮, taskpackage acceptance[] 登记,
 *     human 在场持板）: host 侧零齿、不伪造 host 证据。RC-A4 服务端断退避窗内自发
 *     hello 建连序列 + P1 屏显不变; RC-B4 会话收尾 T 秒内常驻重建 + 跨夜静置提醒
 *     可达（front/引擎无 alert_skip no_online_device）; RC-B5 复用通道会话回归
 *     （story ctx + reminder 四态 + 引导态演出既有齿真机回归）; RC-B6 Idle 常驻
 *     modem 档位/keepalive 观测（结论进 docs-delta）。
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，HOST_TEST_CONVENTIONS.md [C4.2] 约定）:
 *   1) firmware/main/application.h: Application 私有区新增成员
 *      `auto_connect_in_flight_`（bool 或 std::atomic<bool>，needle 字面不变）+
 *      私有方法声明 `void TryStartAutoConnect();`。
 *   2) firmware/main/application.cc: 定义 `void Application::TryStartAutoConnect()`
 *      （[S1] 内部序①②③④；task 体 lambda 直排 xTaskCreate 内，task 名
 *      "AutoConnect"，沿用 activation 模板: 栈 4096*2、优先级 2）；文件内定义
 *      `static const int kAutoConnectBackoffS[3] = { 5, 10, 20 };`（[S3]，文件域或
 *      函数域皆可——needle 扫 application.cc 全文；vTaskDelay 换算乘法形态自由）。
 *      纪律: 缝区（含 task lambda）内零 SetDeviceState( 调用、零 Settings 写 API。
 *   3) 触发点接线: HandleActivationDoneEvent 末尾 + HandleNetworkConnectedEvent 新
 *      Idle 支路（kDeviceStateIdle 分支）各调 TryStartAutoConnect()。
 *   4) InitializeProtocol 的 OnNetworkError lambda: 在 MAIN_EVENT_ERROR 置位语句之前
 *      加 auto_connect_in_flight_ 拦截分支（命中 log + return，不置
 *      MAIN_EVENT_ERROR）; 其余错误路由零改动。
 *   5) OnAudioChannelOpened lambda 体包进 Schedule()（统一回主循环; 体内容零改动）。
 *   6) 四 call site in-flight 守卫: HandleToggleChatEvent / HandleStartListeningEvent
 *      / HandleWakeWordDetectedEvent（守卫置于 EncodeWakeWord() 之前）/
 *      WakeWordInvoke; 既有分支行为语义零改动（仅加守卫）。
 *   7) host 接线: firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../boot_auto_connect.acceptance.c"，置于
 *      "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位）。
 *   8) device build 闸: xiaozhi.bin 构建过 + 体积余量记录（builder/verifier 相位）。
 *   9) mutation 自证（mutationRequired=true）: 六条变异态红由通道②独立命令复现
 *      （映射表见文件尾 MUTATION REPLAY MAP），原始输出落
 *      delivery/taskpackages/260923-m3-k6-boot-auto-connect/mutation-evidence/。
 *      变异编辑不得顺手改动既有齿与未列面。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY（本验收件注册 HOST_ACCEPTANCE_SRCS 后随
 *      host runner 全量执行; 本文件零新实现源、无强符号链接项）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* -------------------------------------------------------------------------- *
 * 被扫文件（真实工作树，root 自手持 cwd 定位——evidence 类扫描齿 root 注入纪律，
 * 禁依赖调用点 cwd; test 目录排除——本冻结文件自身住 test 目录，其注释合法提及
 * needle，不进扫描）。
 * ------------------------------------------------------------------------ */
#define KAC_APP_CC    "firmware/main/application.cc"
#define KAC_APP_H     "firmware/main/application.h"
#define KAC_WS_CC     "firmware/main/protocols/websocket_protocol.cc"
#define KAC_PROTO_H   "firmware/main/protocols/protocol.h"
#define KAC_HOST_CML  "firmware/main/test/host_app/main/CMakeLists.txt"
#define KAC_REQ_MD    "docs/requirements.md"
#define KAC_CONTRACT  "docs/contracts/interface-contract.md"
#define KAC_WS_H      "firmware/main/protocols/websocket_protocol.h"
#define KAC_BOARD_CC  "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define KAC_SLEEP_CC  "firmware/main/boards/common/sleep_timer.cc"
#define KAC_PST_CC    "firmware/main/boards/common/power_save_timer.cc"

#define KAC_MAX_PATH 1024
#define KAC_MAX_READ (8u * 1024u * 1024u)

/* 冻结 needle（本文件为单源; 值 = 2026-09-23 冻结基线逐 needle 实证）:
 *   - application.cc `SetDeviceState(kDeviceStateConnecting)` 恰 4 处
 *     （:711 ToggleChat / :744 StartListening / :783 WakeWordDetected /
 *     :985 WakeWordInvoke——四个既有人工 call site, [S1] 负 needle 基线）。
 *   - application.cc `OpenAudioChannel()` 恰 4 处（同上四点; builder 在 task 体
 *     新增第 5 处属预期, 本 needle 只钉人工四处不减）。
 *   - `auto_connect_in_flight_` / `TryStartAutoConnect` / `kAutoConnectBackoffS`
 *     全仓零命中（设计态红前提, 2026-09-23 grep 实证）。 */
#define KAC_CONNECTING_NEEEDLE "SetDeviceState(kDeviceStateConnecting)"
#define KAC_OPEN_CHANNEL_ND    "OpenAudioChannel()"
#define KAC_FLAG_ND            "auto_connect_in_flight_"
#define KAC_SEAM_FN_ND         "void Application::TryStartAutoConnect()"
#define KAC_SEAM_CALL_ND       "TryStartAutoConnect()"
#define KAC_BACKOFF_ND         "kAutoConnectBackoffS"

/* RC 批 needle（261010-device-reconnect-on-server-close 包新增; 值 = 2026-10-11
 * 冻结基线逐 needle 实证——`local_close_pending_` / `ScheduleResidentRebuild` /
 * `kResidentRebuildDelayS` 全仓零命中, 2026-10-11 grep 实证 = 设计态红前提）。 */
#define KAC_LC_PENDING_ND      "local_close_pending_"
#define KAC_B_HELPER_ND        "ScheduleResidentRebuild"
#define KAC_B_HELPER_CALL_ND   "ScheduleResidentRebuild()"
#define KAC_B_HELPER_FN_ND     "void Application::ScheduleResidentRebuild()"
#define KAC_REBUILD_DELAY_ND   "kResidentRebuildDelayS"

static char kac_root[KAC_MAX_PATH];
static int kac_root_ready;

static int kac_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kac_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../..", "../../../../../../.."
    };
    char probe[KAC_MAX_PATH];

    if (kac_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kac_is_regular(probe)) {
            snprintf(kac_root, sizeof(kac_root), "%s", cands[i]);
            kac_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kac_rel(char *out, size_t cap, const char *rel)
{
    if (!kac_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", kac_root, rel);
    return 1;
}

static int kac_exists(const char *rel)
{
    char p[KAC_MAX_PATH];
    return kac_rel(p, sizeof(p), rel) && kac_is_regular(p);
}

static char *kac_read(const char *rel, size_t *out_len)
{
    char p[KAC_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kac_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KAC_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int kac_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kac_file_contains(const char *rel, const char *needle)
{
    char *buf = kac_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kac_contains(buf, needle);
    free(buf);
    return r;
}

static int kac_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kac_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

static int kac_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL.
 * Caller frees. end_needle == NULL -> slice to EOF. */
static char *kac_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = kac_read(rel, NULL);
    const char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    if (end_needle == NULL) {
        e = buf + strlen(buf); /* 无尾锚——切到 EOF */
    } else {
        e = strstr(s + strlen(start_needle), end_needle);
        if (e == NULL) {
            e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
        }
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* 成员函数区切取：start_needle 起至下一个 Application 成员函数定义（换行 + 返回
 * 类型 + Application:: 形态）或 EOF。供新缝函数（无既有尾锚）使用。 */
static char *kac_region_member_fn(const char *rel, const char *start_needle)
{
    char *buf = kac_read(rel, NULL);
    const char *s;
    const char *e = NULL;
    static const char *const sigs[] = {
        "\nvoid Application::", "\nbool Application::", "\nint Application::",
        "\nApplication::"
    };
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    for (size_t i = 0; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
        const char *hit = strstr(s + strlen(start_needle), sigs[i]);
        if (hit != NULL && (e == NULL || hit < e)) {
            e = hit;
        }
    }
    if (e == NULL) {
        e = buf + strlen(buf);
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

#define KAC_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kac_locate_root(), \
    "K6 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper（K1a 先例：动态诊断先格式化进静态缓冲再进单串断言宏）。 */
static char kac_fm_buf[1024];
static const char *kac_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kac_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kac_fm_buf, sizeof(kac_fm_buf), fmt, ap);
    va_end(ap);
    return kac_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — K6-A1 正锚：自动建连缝（守卫三条件 + 双触发点 + 主循环 spawn + task 化
 * + 零 Connecting 负 needle）。
 * MUST-STAY-GREEN 半（现状即绿）：scanner 自检 + Connecting 计数恒 4 基线。
 * RED today：缝未实现（auto_connect_in_flight_ / TryStartAutoConnect 全仓零命中,
 * 2026-09-23 grep 实证）。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A1 -- 自动建连缝正锚：守卫入口三条件 token + 双触发点接线（ActivationDone 末尾 + NetworkConnected Idle 支路）+ 主循环 handler 内 spawn 判定 + 后台 task 化（xTaskCreate "AutoConnect"）+ 负 needle：Connecting 计数恒 4 / 缝区零 SetDeviceState（防语音轮 UI 接管回归） */
TEST_CASE("开机自动建连: 自动建连缝正锚 (K6-A1)",
          "[boot_auto_connect][K6-A1][structural]")
{
    char *seam;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 负 needle 冻结基线): application.cc
     * SetDeviceState(kDeviceStateConnecting) 恰 4 处——四个既有人工 call site。
     * 现状即绿; 自动路径任何 Connecting 调用（互斥形状回潮）→ 5+ → 红。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_CONNECTING_NEEEDLE);
    TEST_ASSERT_TRUE_MESSAGE(n == 4,
        kac_fm("K6-A1 RED: application.cc SetDeviceState(kDeviceStateConnecting) "
               "= %d 处（冻结基线恒 4）——自动路径被改成 Connecting 互斥形状（语音轮 "
               "UI 接管回归: 进 Connecting 弹 VOICE 前台层 + use-after-free 竞态窗, "
               "plan REBUT 记录）或人工 call site 被拆", n));

    /* tooth 2 (MUST-STAY-GREEN, 扫描器自检防假绿): 已知文件可读 + 永不存在的
     * needle 零命中。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_exists(KAC_APP_CC) && kac_exists(KAC_APP_H) &&
                                 kac_exists(KAC_WS_CC),
        "K6 RED(infra): scanner cannot see known-good files — 扫描器失灵");
    TEST_ASSERT_TRUE_MESSAGE(kac_count_in_file(KAC_APP_CC,
        "__k6_no_such_needle__") == 0,
        "K6 RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");

    /* tooth 3 (RED today): application.h 声明互斥标志成员 auto_connect_in_flight_。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_H, KAC_FLAG_ND),
        "K6-A1 RED: application.h 无 auto_connect_in_flight_——标志互斥成员未落"
        "（Shape A 核心, BUILDER DUTY 1)）");

    /* tooth 4 (RED today): application.h 声明 + application.cc 定义
     * TryStartAutoConnect()——「判+起」缝入口。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_H, KAC_SEAM_CALL_ND),
        "K6-A1 RED: application.h 无 TryStartAutoConnect 声明——缝入口未落"
        "（BUILDER DUTY 1)）");
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_CC, KAC_SEAM_FN_ND),
        "K6-A1 RED: application.cc 无 void Application::TryStartAutoConnect() 定义"
        "——自动建连缝未实现（开机建连缺口 = 本包根因现状在世）");

    /* tooth 5 (RED today, structural): 缝区守卫三条件齐备——kDeviceStateIdle /
     * IsAudioChannelOpened / auto_connect_in_flight_ 三 token 同在缝区。 */
    seam = kac_region_member_fn(KAC_APP_CC, KAC_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "K6-A1 RED(infra): TryStartAutoConnect 函数区切取失败——缝函数存在但形态"
        "漂移（须复核锚; 禁改名绕过）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "kDeviceStateIdle") &&
                                 kac_contains(seam, "IsAudioChannelOpened") &&
                                 kac_contains(seam, KAC_FLAG_ND),
        "K6-A1 RED: TryStartAutoConnect 区内守卫三条件不齐（state==Idle && "
        "!IsAudioChannelOpened() && !auto_connect_in_flight_ 缺一）——TOCTOU 判设"
        "闭合被拆（三条件缺一 = 竞态或重复 spawn 入口）");
    TEST_ASSERT_TRUE_MESSAGE(kac_count_in_slice(seam, "kDeviceStateIdle") >= 2 &&
                                 kac_count_in_slice(seam, "IsAudioChannelOpened") >= 2,
        "K6-A1 RED: 缝区三条件仅出现 1 次——缺「task 内重试前重查三条件」半"
        "（K6-A2 语义, 重查下界承载）");

    /* tooth 6 (RED today, structural): 缝区零 SetDeviceState( ——自动建连全程状态恒
     * Idle（Shape A 核心; 成功/失败/bail 全路径无状态迁移）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(seam, "SetDeviceState(") == NULL,
        "K6-A1 RED: TryStartAutoConnect 区内出现 SetDeviceState(——自动路径引入"
        "状态迁移（Shape A 恒 Idle 纪律破; 语音轮 UI 接管回归入口）");

    /* tooth 7 (RED today, structural): 后台 task 化——xTaskCreate + task 名
     * "AutoConnect" 在缝区（沿用 activation 模板, 不阻塞主循环; "AutoConnect" tag
     * log 足迹 = plan 边角钉点）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "xTaskCreate") &&
                                 kac_contains(seam, "\"AutoConnect\""),
        "K6-A1 RED: 缝区无 xTaskCreate + \"AutoConnect\" task 名——自动建连仍阻塞"
        "主循环或 task 足迹缺席（plan 任务模板未落）");

    /* tooth 8 (RED today): 触发点 1——HandleActivationDoneEvent 函数区调
     * TryStartAutoConnect()。 */
    {
        char *r1 = kac_region_between(KAC_APP_CC,
                                      "void Application::HandleActivationDoneEvent()",
                                      "void Application::ActivationTask()");
        TEST_ASSERT_NOT_NULL_MESSAGE(r1,
            "K6-A1 RED(infra): HandleActivationDoneEvent 函数区切取失败——锚漂移");
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(r1, KAC_SEAM_CALL_ND),
            "K6-A1 RED: HandleActivationDoneEvent 区未接 TryStartAutoConnect——"
            "触发点 1（激活完成）未接线（开机建连缺口, BUILDER DUTY 3)）");
        free(r1);
    }

    /* tooth 9 (RED today): 触发点 2——HandleNetworkConnectedEvent 函数区 Idle 支路
     * （kDeviceStateIdle 分支）调 TryStartAutoConnect()。 */
    {
        char *r2 = kac_region_between(KAC_APP_CC,
                                      "void Application::HandleNetworkConnectedEvent()",
                                      "void Application::HandleNetworkDisconnectedEvent()");
        TEST_ASSERT_NOT_NULL_MESSAGE(r2,
            "K6-A1 RED(infra): HandleNetworkConnectedEvent 函数区切取失败——锚漂移");
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(r2, "kDeviceStateIdle") &&
                                     kac_contains(r2, KAC_SEAM_CALL_ND),
            "K6-A1 RED: HandleNetworkConnectedEvent 区无 Idle 支路 + "
            "TryStartAutoConnect——触发点 2（网络重连恢复）未接线（断网重连自动恢复"
            "缺口）");
        free(r2);
    }

    /* tooth 10 (MUST-STAY-GREEN 下界): 缝入口在 application.cc 恰被两触发点调用——
     * 定义 + 两调用 >= 3 处（防触发点被静默摘除后其余齿仍绿的孤岛态）。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_SEAM_CALL_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kac_fm("K6-A1 RED: TryStartAutoConnect( 在 application.cc 仅 %d 处（下界 3: "
               "定义 + 两触发点调用）——双触发点接线被摘", n));

    free(seam);
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — K6-A2 静默失败锚 + 有界退避决策表（K5 留根②: 钉决策表形态不钉时序）。
 * MUST-STAY-GREEN 半：OnNetworkError lambda 区可切 + MAIN_EVENT_ERROR 既有路由在世。
 * RED today：缝未实现。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A2 -- 静默失败锚：OnNetworkError lambda in-flight 拦截分支（不置 MAIN_EVENT_ERROR）+ 置位前序 + 退避决策表形态（封顶 3 次 + 5s/10s/20s 递增序列 kAutoConnectBackoffS）+ 重试前三条件重查下界 + 失败全程 Idle */
TEST_CASE("开机自动建连: 静默失败+有界退避决策表 (K6-A2)",
          "[boot_auto_connect][K6-A2][structural]")
{
    char *err_region, *seam;
    const char *p_flag, *p_err;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): OnNetworkError lambda 区可切（锚在世）。 */
    err_region = kac_region_between(KAC_APP_CC,
                                    "protocol_->OnNetworkError(",
                                    "protocol_->OnIncomingAudio(");
    TEST_ASSERT_NOT_NULL_MESSAGE(err_region,
        "K6-A2 RED(infra): OnNetworkError lambda 区切取失败——InitializeProtocol "
        "回调链被改动（锚漂移须复核; 禁整 lambda 退役绕过）");

    /* tooth 2 (MUST-STAY-GREEN): 人工路径错误路由在世——MAIN_EVENT_ERROR 置位仍在
     * lambda 区（拦截分支只豁免自动路径, 非拦截面零改动）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(err_region, "MAIN_EVENT_ERROR"),
        "K6-A2 RED: OnNetworkError 区 MAIN_EVENT_ERROR 路由丢失——人工路径错误"
        "处理被连带拆除（既有语义零改动义务）");

    /* tooth 3 (RED today): 拦截分支在世——lambda 区含 auto_connect_in_flight_。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(err_region, KAC_FLAG_ND),
        "K6-A2 RED: OnNetworkError lambda 无 auto_connect_in_flight_ 拦截分支——"
        "自动建连失败会置 MAIN_EVENT_ERROR 弹错误卡（静默失败语义未落, 开机失败"
        "即错误卡回潮）");

    /* tooth 4 (RED today, 前序): 拦截判定位次先于 MAIN_EVENT_ERROR 置位（命中即
     * return, 不落错误事件）。 */
    p_flag = err_region != NULL ? strstr(err_region, KAC_FLAG_ND) : NULL;
    p_err = err_region != NULL ? strstr(err_region, "MAIN_EVENT_ERROR") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_flag != NULL && p_err != NULL && p_flag < p_err,
        "K6-A2 RED: in-flight 拦截判定未先于 MAIN_EVENT_ERROR 置位——拦截分支"
        "形同虚设（自动路径错误事件仍会发出, 错误卡回潮）");
    free(err_region);

    /* tooth 5 (RED today): 退避决策表定义——kAutoConnectBackoffS >= 2 处（定义 +
     * task 内消费; [S3] 冻结命名缝）。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_BACKOFF_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kac_fm("K6-A2 RED: kAutoConnectBackoffS 在 application.cc 仅 %d 处（下界 2: "
               "定义 + task 内消费）——有界退避决策表未落（重试风暴或无重试入口）",
               n));

    /* tooth 6 (RED today, 决策表形态): 表值 = { 5, 10, 20 } 递增秒序列（5s/10s/20s,
     * plan 定值; 形态钉值序不钉排版——首定义片段内 5 < 10 < 20 位序）。 */
    {
        char *def = kac_region_between(KAC_APP_CC, KAC_BACKOFF_ND, ";");
        TEST_ASSERT_NOT_NULL_MESSAGE(def,
            "K6-A2 RED(infra): kAutoConnectBackoffS 定义片段切取失败");
        {
            const char *p5 = def != NULL ? strstr(def, "5") : NULL;
            const char *p10 = def != NULL ? strstr(def, "10") : NULL;
            const char *p20 = def != NULL ? strstr(def, "20") : NULL;
            TEST_ASSERT_TRUE_MESSAGE(p5 != NULL && p10 != NULL && p20 != NULL &&
                                         p5 < p10 && p10 < p20,
                "K6-A2 RED: kAutoConnectBackoffS 首定义非 { 5, 10, 20 } 递增秒序列"
                "——退避形态偏离冻结值（5s/10s/20s, plan 方案形状; 偏离回 planner "
                "重冻结）");
        }
        free(def);
    }

    /* tooth 7 (RED today, 封顶形态): 表长 3（[3] 字面同现——封顶 3 次的决策表形态
     * 承载; 无界重试 = 表退化或缺失）。 */
    {
        char *def = kac_region_between(KAC_APP_CC, KAC_BACKOFF_ND, ";");
        TEST_ASSERT_TRUE_MESSAGE(def != NULL && kac_contains(def, "[3]"),
            "K6-A2 RED: kAutoConnectBackoffS 定义无 [3] 长度——退避封顶 3 次形态"
            "未落（重试无界 = 连接风暴）");
        free(def);
    }

    /* tooth 8 (RED today): 退避真消费在 task 区——缝区内消费 kAutoConnectBackoffS
     * （防「表定义了但 task 裸睡固定值」的假接线）。 */
    seam = kac_region_member_fn(KAC_APP_CC, KAC_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "K6-A2 RED(infra): TryStartAutoConnect 函数区切取失败（TEST 2 tooth 8）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, KAC_BACKOFF_ND),
        "K6-A2 RED: TryStartAutoConnect 区（含 task 体）未消费 kAutoConnectBackoffS"
        "——退避决策表是孤岛（重试延迟未接决策表）");

    /* tooth 9 (RED today, 重试前三条件重查): task 区重查三条件下界（tooth 承载见
     * TEST 1 tooth 5 双现下界; 此处钉重查与消费同区——重查逻辑不在孤岛函数）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_count_in_slice(seam, KAC_FLAG_ND) >= 3,
        "K6-A2 RED: 缝区 auto_connect_in_flight_ 出现 < 3 次（下界: 守卫查 + 置位 "
        "+ task 重查/清）——重试前重查或收尾清标志缺失");

    free(seam);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — K6-A3 未配网/空 url 负锚：预检只读 + 完全 no-op。
 * MUST-STAY-GREEN 半：protocol 读形态在世（零触碰锚）。
 * RED today：缝未实现。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A3 -- 未配网/空 url 预检负锚：Settings("websocket", false) 只读形参逐字 + 缝区零 Settings 写 API（SetString/SetInt/SetBool/EraseKey/EraseAll）+ 空 url/token 检查存在 + no-op 前序（空检查先于 xTaskCreate）+ protocol 读形态在世零触碰 */
TEST_CASE("开机自动建连: 未配网预检负锚 (K6-A3)",
          "[boot_auto_connect][K6-A3][structural]")
{
    char *seam;
    const char *p_empty, *p_spawn;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): protocol 读形态在世——websocket_protocol.cc:86
     * `Settings settings("websocket", false)` 零触碰锚（本包不动 protocol 层）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_WS_CC,
                                               "Settings settings(\"websocket\", false)"),
        "K6-A3 RED: websocket_protocol.cc 读形态 Settings(\"websocket\", false) "
        "丢失——protocol 层被触碰（K6-E2 零触碰铁律）");

    /* tooth 2 (RED today): 缝区预检读——Settings("websocket", false) 显式只读形参
     * （与 protocol 读形态逐字同款; namespace 对齐 websocket_protocol.cc:86-89）。 */
    seam = kac_region_member_fn(KAC_APP_CC, KAC_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "K6-A3 RED(infra): TryStartAutoConnect 函数区切取失败（TEST 3 tooth 2）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "Settings settings(\"websocket\", false)"),
        "K6-A3 RED: 缝区无 Settings(\"websocket\", false) 只读预检——未配网/空 url "
        "时自动建连不 no-op（重试风暴/报错卡入口; namespace 须对齐 protocol 读面）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "GetString(\"url\")") &&
                                 kac_contains(seam, "GetString(\"token\")"),
        "K6-A3 RED: 预检未同检 url 与 token——token 缺失即建连（对端拒收浪费重试"
        "窗; namespace 对齐面缺半）");

    /* tooth 3 (RED today): 空检查存在——empty() 判定在缝区（空 url 单条 log 后
     * return）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "empty()"),
        "K6-A3 RED: 缝区无 empty() 空值判定——未配网（空 url）不被预检拦截");

    /* tooth 4 (RED today, no-op 前序): 空检查位次先于 xTaskCreate——空 url 完全
     * no-op（不起 task、不重试、不报错）。 */
    p_empty = seam != NULL ? strstr(seam, "empty()") : NULL;
    p_spawn = seam != NULL ? strstr(seam, "xTaskCreate") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_empty != NULL && p_spawn != NULL && p_empty < p_spawn,
        "K6-A3 RED: 空值检查未先于 xTaskCreate——未配网仍会起 task（no-op 语义"
        "破: 重试风暴入口）");

    /* tooth 5 (RED today, 只读纪律): 缝区零 Settings 写 API——SetString / SetInt /
     * SetBool / EraseKey / EraseAll 零命中（预检只读; 写入口 =
     * WsProvision::EnsureWebsocketToken activation 期已跑, plan 边角）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(seam, "SetString(") == NULL && strstr(seam, "SetInt(") == NULL &&
        strstr(seam, "SetBool(") == NULL && strstr(seam, "EraseKey(") == NULL &&
        strstr(seam, "EraseAll(") == NULL,
        "K6-A3 RED: 缝区出现 Settings 写 API——预检改写配置（只读纪律破; NVS 配置"
        "面被自动建连路径污染）");

    free(seam);
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — K6-A4 in-flight 生命周期负锚：全退出路径必清。
 * MUST-STAY-GREEN 半：activation 模板收尾锚在世（退出置空先例）。
 * RED today：缝未实现。
 * 注: 「所有退出路径必清」的完全证明 = verifier 逐路径核对（acceptance 明文）;
 * host 扫描钉的是清标志在世 + 三态下界, 强度缺口已在头注强度申报显式挂账。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A4 -- in-flight 生命周期负锚：task 区清 auto_connect_in_flight_ needle（xTaskCreate 之后有清点）+ 缝区三态下界（查/置/清）+ 退出前 uxTaskGetStackHighWaterMark 栈高水位日志 + activation 模板收尾锚在世 */
TEST_CASE("开机自动建连: in-flight生命周期 (K6-A4)",
          "[boot_auto_connect][K6-A4][structural]")
{
    char *seam;
    const char *p_spawn, *p_clear;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): activation 模板收尾锚在世——activation_task_handle_
     * 退出置空先例（task 模板形态参照, 零触碰锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_CC,
                                               "activation_task_handle_ = nullptr"),
        "K6-A4 RED(infra): activation task 收尾置空锚丢失——task 模板参照面被改动"
        "（零触碰锚漂移须复核）");

    /* tooth 2 (RED today): 缝区可切。 */
    seam = kac_region_member_fn(KAC_APP_CC, KAC_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "K6-A4 RED(infra): TryStartAutoConnect 函数区切取失败（TEST 4 tooth 2）");

    /* tooth 3 (RED today, 核心齿): 清标志点在 task 区（xTaskCreate 之后存在
     * auto_connect_in_flight_ 清点——后台 task 收尾清标志在世; 泄漏 true = 人工按键
     * 永久 no-op + 自动建连永久失效, 本形状唯一单点灾难模式）。 */
    p_spawn = seam != NULL ? strstr(seam, "xTaskCreate") : NULL;
    p_clear = seam != NULL && p_spawn != NULL
                  ? strstr(p_spawn, KAC_FLAG_ND)
                  : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_spawn != NULL && p_clear != NULL,
        "K6-A4 RED: task 区（xTaskCreate 之后）无 auto_connect_in_flight_ 清点——"
        "task 退出不清标志（泄漏 true = 人工按键永久 no-op + 自动建连永久失效, "
        "单点灾难模式）");

    /* tooth 4 (RED today, 三态下界): 缝区 auto_connect_in_flight_ >= 3 处
     * （守卫查 + 置位 + task 清; 与 TEST 1 tooth 5 双现下界互为独立承载）。 */
    n = kac_count_in_slice(seam, KAC_FLAG_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kac_fm("K6-A4 RED: 缝区 auto_connect_in_flight_ 仅 %d 处（下界 3: 守卫查 + "
               "置位 + task 清点）——生命周期三态缺环", n));

    /* tooth 5 (RED today): task 退出前栈高水位日志——uxTaskGetStackHighWaterMark 在
     * 缝区（plan 任务模板钉点; 真机锚 A7-④ 顺带核的 host 侧锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(seam, "uxTaskGetStackHighWaterMark"),
        "K6-A4 RED: 缝区无 uxTaskGetStackHighWaterMark——task 退出无栈高水位日志"
        "（plan 任务模板未落; 真机锚 ④ 无 host 侧对照锚）");

    free(seam);
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — K6-A5 人工路径守卫齿：四 call site in-flight 守卫 + 既有分支语义零改动。
 * MUST-STAY-GREEN 半：四函数区可切 + 各区既有 Connecting/OpenAudioChannel token
 * 在世（语义零改动锚; 现状即绿）。
 * RED today：守卫未接（四区零 auto_connect_in_flight_）。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A5 -- 人工路径守卫齿：四个 OpenAudioChannel call site（ToggleChat/StartListening/WakeWordDetected/WakeWordInvoke）函数区 in-flight 守卫 needle + WakeWordDetected 守卫先于 EncodeWakeWord 序 + 既有分支行为语义零改动（各区 Connecting+OpenAudioChannel token 在世） */
TEST_CASE("开机自动建连: 人工路径守卫+既有语义零改动 (K6-A5)",
          "[boot_auto_connect][K6-A5][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
    } sites[] = {
        { "void Application::HandleToggleChatEvent()",
          "void Application::HandleStartListeningEvent()", "ToggleChat" },
        { "void Application::HandleStartListeningEvent()",
          "void Application::HandleStopListeningEvent()", "StartListening" },
        { "void Application::HandleWakeWordDetectedEvent()",
          "void Application::HandleStateChangedEvent()", "WakeWordDetected" },
        { "void Application::WakeWordInvoke(",
          "bool Application::CanEnterSleepMode()", "WakeWordInvoke" },
    };
    size_t i;

    KAC_REQUIRE_ROOT();

    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        char *region = kac_region_between(KAC_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("K6-A5 RED(infra): %s 函数区切取失败——人工路径锚漂移须复核"
                   "（禁整函数退役绕过）", sites[i].label));

        /* 既有语义零改动锚 (MUST-STAY-GREEN): 各区 Connecting + OpenAudioChannel
         * token 在世（现状即绿; 人工分支行为被改动 → 红）。 */
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(region, KAC_CONNECTING_NEEEDLE) &&
                                     kac_contains(region, KAC_OPEN_CHANNEL_ND),
            kac_fm("K6-A5 RED: %s 区既有建连分支 token 丢失——按键/唤醒人工路径"
                   "既有行为语义被改动（仅许加 in-flight 守卫, 零语义改动义务）",
                   sites[i].label));

        /* 守卫齿 (RED today): 区内 auto_connect_in_flight_ in-flight 守卫在世。 */
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(region, KAC_FLAG_ND),
            kac_fm("K6-A5 RED: %s call site 无 auto_connect_in_flight_ 守卫——"
                   "自动建连 in-flight 期间人工按键与后台 task 竞态（重复 spawn / "
                   "并发 websocket_ 访问窗）", sites[i].label));
        free(region);
    }

    /* 序钉 (RED today): WakeWordDetected 区守卫置于 EncodeWakeWord() 之前（plan
     * 关键接缝: 守卫在 :780 EncodeWakeWord 之前——唤醒音频编码不得在 in-flight 期
     * 启动）。 */
    {
        char *region = kac_region_between(KAC_APP_CC,
                                          "void Application::HandleWakeWordDetectedEvent()",
                                          "void Application::HandleStateChangedEvent()");
        const char *p_guard, *p_enc;
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "K6-A5 RED(infra): WakeWordDetected 函数区切取失败（序钉 tooth）");
        p_guard = region != NULL ? strstr(region, KAC_FLAG_ND) : NULL;
        p_enc = region != NULL ? strstr(region, "EncodeWakeWord()") : NULL;
        TEST_ASSERT_TRUE_MESSAGE(p_guard != NULL && p_enc != NULL && p_guard < p_enc,
            "K6-A5 RED: WakeWordDetected 区守卫未置于 EncodeWakeWord() 之前——"
            "in-flight 期唤醒音频编码仍启动（plan 关键接缝位次破）");
        free(region);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — K6-A6 回归 + 注册 + OnAudioChannelOpened Schedule 包裹 + protocol 零触碰。
 * MUST-STAY-GREEN 半：既有齿注册在世 + protocol 零触碰锚 + 需求契约 canary +
 * 扫描器自检（现状即绿）。
 * RED today：Schedule 包裹未落 + 本验收件未注册 HOST_ACCEPTANCE_SRCS。
 * ------------------------------------------------------------------------ */
/* coverage: K6-A6 -- 回归+注册闸：OnAudioChannelOpened lambda 体 Schedule() 包裹（回调统一回主循环）+ 本验收件接线 HOST_ACCEPTANCE_SRCS（smoke 末位序）+ 既有齿注册在世（零偏移回归面）+ protocol 零触碰（Start 空桩 + !IsTimeout() 既有行为不修）+ 需求契约 canary + 扫描器自检 */
TEST_CASE("开机自动建连: 回归注册+Schedule包裹+protocol零触碰 (K6-A6)",
          "[boot_auto_connect][K6-A6][structural]")
{
    char *opened_region;
    char *mk;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 扫描器自检): 已知文件可读 + 永不存在 needle 零命中
     * （防假绿）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_exists(KAC_APP_CC) && kac_exists(KAC_WS_CC) &&
                                 kac_exists(KAC_PROTO_H) && kac_exists(KAC_HOST_CML),
        "K6 RED(infra): scanner cannot see known-good files — 扫描器失灵");
    TEST_ASSERT_TRUE_MESSAGE(kac_count_in_file(KAC_WS_CC,
        "__k6_no_such_needle__") == 0,
        "K6 RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");

    /* tooth 2 (MUST-STAY-GREEN, 需求契约 canary): 需求面锚在世 + 契约面在世。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_REQ_MD, "S-F1.1"),
        "K6 RED: requirements S-F1.1 锚丢失——需求面被修订（发现冲突走 spec-lane "
        "逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(kac_exists(KAC_CONTRACT),
        "K6 RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3 (MUST-STAY-GREEN, protocol 零触碰): Start() 空桩惰性注释在世。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(
        KAC_WS_CC, "Only connect to server when audio channel is needed"),
        "K6-A6 RED: websocket_protocol.cc Start() 空桩注释丢失——protocol 层被触碰"
        "（K6-E2 零触碰铁律; 本包实现面仅 application.cc/h）");

    /* tooth 4 (MUST-STAY-GREEN, 边角纪律): IsAudioChannelOpened 的 !IsTimeout() 在世
     * ——120s 覆盖式重连既有行为不顺手修（K6-E2; 真机锚 ② 避开该路径）。 */
    {
        char *opened = kac_region_between(KAC_WS_CC,
                                          "bool WebsocketProtocol::IsAudioChannelOpened()",
                                          "void WebsocketProtocol::CloseAudioChannel()");
        TEST_ASSERT_NOT_NULL_MESSAGE(opened,
            "K6-A6 RED(infra): IsAudioChannelOpened 函数区切取失败——锚漂移须复核");
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(opened, "!IsTimeout()"),
            "K6-A6 RED: IsAudioChannelOpened 的 !IsTimeout() 丢失——120s 覆盖式重连"
            "既有行为被顺手修（K6-E2 scope 纪律; 既有行为非本包缺陷）");
        free(opened);
    }

    /* tooth 5 (MUST-STAY-GREEN): 既有齿注册在世——回归面零偏移（key_layer_dispatch /
     * schedule_grid / p1_home_battery / time_sync / smoke）。 */
    mk = kac_read(KAC_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "K6 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(mk, "key_layer_dispatch.acceptance.c") != NULL &&
        strstr(mk, "schedule_grid.acceptance.c") != NULL &&
        strstr(mk, "p1_home_battery.acceptance.c") != NULL &&
        strstr(mk, "time_sync.acceptance.c") != NULL &&
        strstr(mk, "host_infra_smoke.acceptance.c") != NULL,
        "K6-A6 RED: 既有冻结齿注册行丢失——回归面被本包拆（K6-A6 既有齿零偏移义务）");
    free(mk);

    /* tooth 6 (RED today): OnAudioChannelOpened lambda 体包 Schedule()——回调统一
     * 回主循环（自动路径下该回调在后台 task 线程执行; 人工路径本就在主循环,
     * 包裹无害, plan 方案形状）。 */
    opened_region = kac_region_between(KAC_APP_CC,
                                       "protocol_->OnAudioChannelOpened(",
                                       "protocol_->OnAudioChannelClosed(");
    TEST_ASSERT_NOT_NULL_MESSAGE(opened_region,
        "K6-A6 RED(infra): OnAudioChannelOpened lambda 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(opened_region, "Schedule("),
        "K6-A6 RED: OnAudioChannelOpened lambda 体未包 Schedule()——回调仍在后台 "
        "task 线程直执行（自动路径线程纪律破; SetPowerSaveLevel/采样率告警脱离主循环）");
    free(opened_region);

    /* tooth 7 (RED today): 本验收件接线 HOST_ACCEPTANCE_SRCS，置于 smoke 之前。 */
    mk = kac_read(KAC_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "K6 RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *acc = strstr(mk, "boot_auto_connect.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(acc != NULL,
            "K6-A6 RED: boot_auto_connect.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
            " — host 验收构建未接本包（BUILDER DUTY 7)）");
        TEST_ASSERT_TRUE_MESSAGE(acc != NULL && smoke != NULL && acc < smoke,
            "K6-A6 RED: boot_auto_connect 注册行必须排在 host_infra_smoke.acceptance.c"
            " 之前（smoke 末位 = 最先执行, boundary 齿注册序守卫）");
    }
    free(mk);

    /* tooth 8 (MUST-STAY-GREEN): protocol.h OpenAudioChannel 接口面在世（签名零改
     * 的存在性锚; 逐签名比对 = verifier 相位义务, K6-E2）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_PROTO_H, KAC_OPEN_CHANNEL_ND),
        "K6-A6 RED: protocol.h OpenAudioChannel 声明丢失——protocol 接口面被触碰"
        "（K6-E2: protocol.h 接口签名零改, 违反须回 triage 升 Full）");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — RC-A1 服务端断自动重连排程（[S7] 本地关分类标记 + [S8] A 路径触发）。
 * MUST-STAY-GREEN 半：扫描器自检 + OnAudioChannelClosed lambda 区既有语义锚在世
 * （SetPowerSaveLevel + SetDeviceState( 恰 1 = 既有 Idle 收尾零偏移）。
 * RED today：local_close_pending_ 全仓零命中（设计态红前提, 2026-10-11 grep 实证）
 * + A 区零 TryStartAutoConnect（根因现状在世: OnAudioChannelClosed 仅收尾零重连）。
 * ------------------------------------------------------------------------ */
/* coverage: RC-A1 -- 非本地关（服务端断）→ 自动重连排程在场：websocket_protocol 本地关分类标记三态（CloseAudioChannel 置位先于 reset / OpenAudioChannel 清位 / OnDisconnected 消费先于回调）+ OnAudioChannelClosed lambda 区 Schedule TryStartAutoConnect 恰一次（既有 Idle 收尾零偏移锚） */
TEST_CASE("断线重连常驻: 服务端断自动重连排程 (RC-A1)",
          "[boot_auto_connect][RC-A1][structural]")
{
    char *a_region, *lc_region, *open_region, *dis_region;
    const char *p_m, *p_c, *p_set, *p_reset;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, 扫描器自检): 新增扫描面可读（websocket_protocol.h +
     * 板级文件; 防假绿）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_exists(KAC_WS_H) && kac_exists(KAC_BOARD_CC),
        "RC RED(infra): scanner cannot see known-good files — 扫描器失灵");

    /* tooth 2 (MUST-STAY-GREEN): A 区（OnAudioChannelClosed lambda 区）可切 + 既有
     * 语义锚在世——SetPowerSaveLevel 在世 + SetDeviceState( 恰 1（既有 Idle 收尾;
     * RC-A3 屏显零触碰的基线载体: 重连触发区新增状态迁移即 2 → 红）。 */
    a_region = kac_region_between(KAC_APP_CC,
                                  "protocol_->OnAudioChannelClosed(",
                                  "protocol_->OnIncomingJson(");
    TEST_ASSERT_NOT_NULL_MESSAGE(a_region,
        "RC-A1 RED(infra): OnAudioChannelClosed lambda 区切取失败——锚漂移须复核"
        "（禁整 lambda 退役绕过）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(a_region, "SetPowerSaveLevel"),
        "RC-A1 RED: A 区 SetPowerSaveLevel 既有收尾锚丢失——既有语义零偏移义务破");
    n = kac_count_in_slice(a_region, "SetDeviceState(");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        kac_fm("RC-A1 RED: A 区 SetDeviceState( = %d 处（冻结基线恰 1 = 既有 Idle "
               "收尾）——重连触发区新增状态迁移 = 屏显触碰（requirements.md:67 语义, "
               "重连尝试期间维持 Idle 屏显）", n));

    /* tooth 3 (RED today): websocket_protocol.h 声明本地关标记 local_close_pending_。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_WS_H, KAC_LC_PENDING_ND),
        "RC-A1 RED: websocket_protocol.h 无 local_close_pending_ 成员——本地主动关"
        "分类标记未落（[S7]; 服务端断与本地关不可分 = 分类互斥无从谈起）");

    /* tooth 4 (RED today): CloseAudioChannel 区置位先于 websocket_.reset()（reset
     * 析构若回调 OnDisconnected, 标记须覆盖析构路径——plan 实现前核项的结构解）。 */
    lc_region = kac_region_between(KAC_WS_CC,
                                   "void WebsocketProtocol::CloseAudioChannel()",
                                   "bool WebsocketProtocol::OpenAudioChannel()");
    TEST_ASSERT_NOT_NULL_MESSAGE(lc_region,
        "RC-A1 RED(infra): CloseAudioChannel 函数区切取失败——锚漂移须复核");
    p_set = lc_region != NULL ? strstr(lc_region, KAC_LC_PENDING_ND) : NULL;
    p_reset = lc_region != NULL ? strstr(lc_region, "websocket_.reset()") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_set != NULL && p_reset != NULL && p_set < p_reset,
        "RC-A1 RED: CloseAudioChannel 未在 websocket_.reset() 之前置位 "
        "local_close_pending_——析构路径回调时分类标记未覆盖（本地关误判服务端断, "
        "重连风暴入口）");
    free(lc_region);

    /* tooth 5 (RED today): OpenAudioChannel 区清位——每条新连接基线 = 非本地关, 陈旧
     * 标记不污染新连接分类。 */
    open_region = kac_region_between(KAC_WS_CC,
                                     "bool WebsocketProtocol::OpenAudioChannel()",
                                     "std::string WebsocketProtocol::GetHelloMessage()");
    TEST_ASSERT_NOT_NULL_MESSAGE(open_region,
        "RC-A1 RED(infra): OpenAudioChannel 函数区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(
        kac_contains(open_region, "local_close_pending_ = false"),
        "RC-A1 RED: OpenAudioChannel 区无 local_close_pending_ = false 清位——新连接"
        "分类基线未复位（陈旧标记 = 建连后被误判本地关, A 路径失效）");
    free(open_region);

    /* tooth 6 (RED today): OnDisconnected lambda 区先消费标记再走回调分支（本地关
     * 不调 on_audio_channel_closed_ → A 路径零触发; 服务端/网络侧断回调在世）——
     * 消费位次先于回调（分类互斥的协议层半, [S7]）。 */
    dis_region = kac_region_between(KAC_WS_CC,
                                    "websocket_->OnDisconnected(",
                                    "ESP_LOGI(TAG, \"Connecting to websocket server");
    TEST_ASSERT_NOT_NULL_MESSAGE(dis_region,
        "RC-A1 RED(infra): OnDisconnected lambda 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(dis_region, KAC_LC_PENDING_ND) &&
                                 kac_contains(dis_region, "on_audio_channel_closed_"),
        "RC-A1 RED: OnDisconnected 区无 local_close_pending_ 消费 + "
        "on_audio_channel_closed_ 回调——关闭事件分类未落协议层");
    p_m = dis_region != NULL ? strstr(dis_region, KAC_LC_PENDING_ND) : NULL;
    p_c = dis_region != NULL ? strstr(dis_region, "on_audio_channel_closed_") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_m != NULL && p_c != NULL && p_m < p_c,
        "RC-A1 RED: OnDisconnected 区标记消费未先于回调——本地关会触发 "
        "on_audio_channel_closed_（分类互斥破: 同一关闭事件双类入口）");
    free(dis_region);

    /* tooth 7 (RED today): A 区 Schedule 排入 TryStartAutoConnect() 恰 1 次——非本地
     * 关（服务端/网络侧断）自动重连; 复用既有 AutoConnect task 守卫与退避（零第二条
     * task）。 */
    n = kac_count_in_slice(a_region, KAC_SEAM_CALL_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        kac_fm("RC-A1 RED: A 区 TryStartAutoConnect( = %d 处（冻结恰 1）——服务端断"
               "自动重连排程缺席（0 = 根因现状在世）或多排程（不双触发破）", n));
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(a_region, "Schedule("),
        "RC-A1 RED: A 区无 Schedule(——重连排程脱离主循环统一派发（回调线程纪律）");

    free(a_region);
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — RC-A2 分类互斥（同一关闭事件恰归一类、不双触发）。
 * MUST-STAY-GREEN 半：三会话收尾点区 + A 区 + OnDisconnected 区可切（锚在世）。
 * RED today：分类写点计数 0 ≠ 1（标记未落）。
 * ------------------------------------------------------------------------ */
/* coverage: RC-A2 -- 服务端断路径与常驻重建路径触发分类互斥：分类写点单点（local_close_pending_ = true 恰 1）+ OnDisconnected 消费先于回调 + A 区零 ScheduleResidentRebuild + 三会话收尾点零直调 TryStartAutoConnect + 全文件 ScheduleResidentRebuild() 恰 5 处（定义 + 四调用点, 第四点 = HandleEndVoiceSessionEvent, 零泄漏面） */
/* coverage: OCE-A2 -- 既有断言重钉（本文件承载面）: RC-A2 tooth 6 全文件 ScheduleResidentRebuild() 计数 4→5（定义 + 四会话收尾点, 第四点 = HandleEndVoiceSessionEvent）+ RC-B2 sites 数组追加第四项承载；verifier round 1 判红锚 = RC-A2 tooth6 重钉段 */
/* coverage: OCE-A3 -- 回归网（四族全绿 + host 全量零新增红 vs 冻结基线 236T3F）: 判据锚 = verifier round evidence（host 全量 runner 结果），本标记为覆盖登记、非本文件单断言承载 */
TEST_CASE("断线重连常驻: 本地关分类互斥 (RC-A2)",
          "[boot_auto_connect][RC-A2][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
    } sites[] = {
        { "void Application::CloseAudioChannel()",
          "void Application::HandleToggleChatEvent()", "CloseAudioChannel(:805)" },
        { "void Application::HandleToggleChatEvent()",
          "void Application::HandleStartListeningEvent()",
          "ToggleChat Listening(:849)" },
        { "void Application::WakeWordInvoke(",
          "bool Application::CanEnterSleepMode()", "WakeWordInvoke Listening(:1189)" },
    };
    char *region;
    const char *p_m, *p_c;
    int n;
    size_t i;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): 三会话收尾点区可切。 */
    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = kac_region_between(KAC_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("RC-A2 RED(infra): %s 函数区切取失败——锚漂移须复核", sites[i].label));
        free(region);
    }

    /* tooth 2 (RED today): 分类写点单点——ws.cc `local_close_pending_ = true` 恰 1
     * （唯一写点 = CloseAudioChannel; 第二写点 = 分类面被旁路）。 */
    n = kac_count_in_file(KAC_WS_CC, "local_close_pending_ = true");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        kac_fm("RC-A2 RED: ws.cc local_close_pending_ = true = %d 处（冻结恰 1 = "
               "CloseAudioChannel 单点）——分类写点多点 = 关闭事件分类面被旁路", n));

    /* tooth 3 (RED today): OnDisconnected 消费先于回调（与 TEST 7 tooth 6 互为独立
     * 承载; 本齿 = 互斥判定位次钉）。 */
    region = kac_region_between(KAC_WS_CC, "websocket_->OnDisconnected(",
                                "ESP_LOGI(TAG, \"Connecting to websocket server");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "RC-A2 RED(infra): OnDisconnected 区切取失败——锚漂移须复核");
    p_m = region != NULL ? strstr(region, KAC_LC_PENDING_ND) : NULL;
    p_c = region != NULL ? strstr(region, "on_audio_channel_closed_") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p_m != NULL && p_c != NULL && p_m < p_c,
        "RC-A2 RED: OnDisconnected 区标记消费未先于 on_audio_channel_closed_ 回调"
        "——本地关与服务端断同走回调 = 同一关闭事件双类（互斥破）");
    free(region);

    /* tooth 4 (guard, B 缝在世后承载): A 区零 ScheduleResidentRebuild——A 触发区永不
     * 调 B helper（非本地关路径不产生常驻重建触发）。 */
    region = kac_region_between(KAC_APP_CC, "protocol_->OnAudioChannelClosed(",
                                "protocol_->OnIncomingJson(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region, "RC-A2 RED(infra): A 区切取失败");
    n = kac_count_in_slice(region, KAC_B_HELPER_CALL_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        kac_fm("RC-A2 RED: A 区出现 ScheduleResidentRebuild = %d 处——服务端断路径调 "
               "B helper（分类互斥破: 非本地关触发常驻重建）", n));
    free(region);

    /* tooth 5 (guard, B 缝在世后承载): 三会话收尾点零直调 TryStartAutoConnect——触发
     * 只经 helper（同点直调 = 双触发入口, 恰归一类破）。 */
    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = kac_region_between(KAC_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("RC-A2 RED(infra): %s 函数区切取失败（tooth 5）", sites[i].label));
        n = kac_count_in_slice(region, KAC_SEAM_CALL_ND);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            kac_fm("RC-A2 RED: %s 区直调 TryStartAutoConnect = %d 处——会话收尾点绕过 "
                   "B helper 双触发（恰归一类破）", sites[i].label, n));
        free(region);
    }

    /* tooth 6 (RED today; OCE-A2 重钉, 261024-vq-overlay-close-end-session):
     * 全 application.cc ScheduleResidentRebuild() 恰 5 处（定义 + 四调用点;
     * 泄漏到其它路径 = 分类面破）。重钉事由: 新增第四会话收尾点
     * HandleEndVoiceSessionEvent（播报态关层结束会话, TECH-261024 修复）也调
     * helper, 原计数恰 4 改后必红——「定义 + 三调用点」改钉「定义 + 四调用点」。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_B_HELPER_CALL_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 5,
        kac_fm("RC-A2 RED: application.cc ScheduleResidentRebuild() = %d 处（冻结恰 5: "
               "定义 + 四会话收尾点, OCE-A2 重钉 261024-vq-overlay-close-end-session）"
               "——触发点泄漏或缺失", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 9 — RC-A3 重连复用 TryStartAutoConnect（零第二条 task / 零新退避表 / 屏显
 * 零触碰）。
 * MUST-STAY-GREEN 半：A 区既有零直连 OpenAudioChannel(（现状即绿）。
 * RED today：B helper 未实现（声明/定义/常量全零命中）。
 * ------------------------------------------------------------------------ */
/* coverage: RC-A3 -- 重连复用 TryStartAutoConnect：B helper 区消费 kResidentRebuildDelayS + 排入 TryStartAutoConnect + 经 Schedule( 回主循环 + 区内 OpenAudioChannel(/kAutoConnectBackoffS/SetDeviceState( 恒 0（零第二条重连路径 / 零新退避消费 / 屏显零触碰）+ kResidentRebuildDelayS 定义消费下界 + A 区零 OpenAudioChannel（连接只经既有 AutoConnect task） */
TEST_CASE("断线重连常驻: 重连复用守卫 (RC-A3)",
          "[boot_auto_connect][RC-A3][structural]")
{
    char *region, *helper;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (RED today): B helper 声明 + 定义在世。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_H, KAC_B_HELPER_ND),
        "RC-A3 RED: application.h 无 ScheduleResidentRebuild 声明——常驻重建缝未落"
        "（[S9]; 会话收尾类本地关零重建 = 根因现状在世）");
    TEST_ASSERT_TRUE_MESSAGE(kac_file_contains(KAC_APP_CC, KAC_B_HELPER_FN_ND),
        "RC-A3 RED: application.cc 无 void Application::ScheduleResidentRebuild() "
        "定义——常驻重建缝未实现");

    /* tooth 2 (RED today): helper 区可切 + 复用守卫在世——消费 kResidentRebuildDelayS
     * + 排入 TryStartAutoConnect() + 经 Schedule( 回主循环（判+起与 K6 [S1] 同源）。 */
    helper = kac_region_member_fn(KAC_APP_CC, KAC_B_HELPER_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(helper,
        "RC-A3 RED(infra): ScheduleResidentRebuild 函数区切取失败——形态漂移须复核"
        "（禁改名绕过）");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(helper, KAC_REBUILD_DELAY_ND) &&
                                 kac_contains(helper, KAC_SEAM_CALL_ND) &&
                                 kac_contains(helper, "Schedule("),
        "RC-A3 RED: ScheduleResidentRebuild 区未同时消费 kResidentRebuildDelayS + 排入 "
        "TryStartAutoConnect + 经 Schedule( 回主循环——常驻重建未复用既有守卫与退避"
        "（第二条重连路径入口）或判+起脱离主循环上下文");

    /* tooth 3 (RED today, 复用负 needle): helper 区零 OpenAudioChannel( / 零
     * kAutoConnectBackoffS / 零 SetDeviceState(——连接只经既有 AutoConnect task,
     * 重试退避只消费既有表, 屏显零触碰。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(helper, "OpenAudioChannel(") == NULL &&
            strstr(helper, KAC_BACKOFF_ND) == NULL &&
            strstr(helper, "SetDeviceState(") == NULL,
        "RC-A3 RED: ScheduleResidentRebuild 区出现 OpenAudioChannel( / "
        "kAutoConnectBackoffS / SetDeviceState(——第二条重连路径 / 新退避消费 / "
        "屏显触碰（[S9] 负 needle 破）");

    /* tooth 4 (RED today): kResidentRebuildDelayS 定义 + 消费 >= 2 处（[S9] 冻结命名
     * 缝, 与 K5 kidpal_ts_* / K6 kAutoConnectBackoffS 命名常量先例同款）。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_REBUILD_DELAY_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kac_fm("RC-A3 RED: kResidentRebuildDelayS 在 application.cc 仅 %d 处（下界 2: "
               "定义 + helper 消费）——T 秒延迟常量未落", n));

    /* tooth 5 (MUST-STAY-GREEN, 既有语义零偏移): A 区零 OpenAudioChannel(——A 路径
     * 连接也只经既有 AutoConnect task（既有 4 人工 call site 不减由 K6-A5 承载）。 */
    region = kac_region_between(KAC_APP_CC, "protocol_->OnAudioChannelClosed(",
                                "protocol_->OnIncomingJson(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region, "RC-A3 RED(infra): A 区切取失败");
    n = kac_count_in_slice(region, "OpenAudioChannel(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        kac_fm("RC-A3 RED: A 区出现 OpenAudioChannel( = %d 处——服务端断路径绕过 "
               "AutoConnect task 直连（守卫与退避被旁路, 第二条重连路径）", n));
    free(region);

    free(helper);
}

/* -------------------------------------------------------------------------- *
 * TEST 10 — RC-B1 永不休眠结构事实（板级零休眠接线 needle; 设计态绿——现状结构
 * 事实钉死, 变异敏感 = 板级加休眠接线即红）。
 * MUST-STAY-GREEN：全部（设计态绿申报: 零接线是现状事实, 齿的作用 = 钉死不回潮;
 * 强度申报见头注 RC-B1 行）。
 * ------------------------------------------------------------------------ */
/* coverage: RC-B1 -- 永不休眠结构事实：板级文件零休眠 timer 接线（power_save_timer / sleep_timer / OnEnterSleepMode / OnEnterLightSleepMode 零命中）+ 直连 sleep API 零命中（esp_light_sleep_start / esp_deep_sleep_start / esp_sleep_enable）+ common 层休眠入口源文件在世 canary（零命中非 vacuous）+ Application::CanEnterSleepMode 既有休眠门在世零偏移 */
TEST_CASE("断线重连常驻: 永不休眠结构事实 (RC-B1)",
          "[boot_auto_connect][RC-B1][structural]")
{
    static const char *const sleep_needles[] = {
        "power_save_timer", "sleep_timer", "OnEnterSleepMode",
        "OnEnterLightSleepMode", "esp_light_sleep_start", "esp_deep_sleep_start",
        "esp_sleep_enable"
    };
    char *sleep_gate;
    size_t i;
    int n;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN, canary): common 层休眠入口源文件在世——证明板级零
     * 命中非 vacuous（缝在树上, 只是本板不接 = 编译面不可达留痕）。 */
    TEST_ASSERT_TRUE_MESSAGE(kac_exists(KAC_SLEEP_CC) && kac_exists(KAC_PST_CC),
        "RC-B1 RED(infra): boards/common 休眠入口源文件缺席——扫描器失灵或 common 层"
        "被重构（canary 失效 = 板级零命中断言 vacuous 风险）");

    /* tooth 2 (设计态绿, 结构事实钉死): 板级文件零休眠接线（include/实例化/回调接线/
     * 直连 sleep API 全族零命中; 「永不休眠」= human 2026-10-10 触点①裁定）。 */
    for (i = 0; i < sizeof(sleep_needles) / sizeof(sleep_needles[0]); i++) {
        n = kac_count_in_file(KAC_BOARD_CC, sleep_needles[i]);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            kac_fm("RC-B1 RED: 板级文件出现休眠接线 needle \"%s\" = %d 处——「永不"
                   "休眠」（决策 29）破: 板级休眠路径在接线面可达, 须回 planner 重冻结",
                   sleep_needles[i], n));
    }

    /* tooth 3 (MUST-STAY-GREEN): Application::CanEnterSleepMode 既有休眠门零偏移
     * （Idle + 通道未开条件在世; 本包零触碰面, 漂移须复核）。 */
    sleep_gate = kac_region_member_fn(KAC_APP_CC,
                                      "bool Application::CanEnterSleepMode()");
    TEST_ASSERT_NOT_NULL_MESSAGE(sleep_gate,
        "RC-B1 RED(infra): CanEnterSleepMode 函数区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(kac_contains(sleep_gate, "kDeviceStateIdle") &&
                                 kac_contains(sleep_gate, "IsAudioChannelOpened"),
        "RC-B1 RED: CanEnterSleepMode 既有休眠门条件缺失——既有语义零偏移义务破");
    free(sleep_gate);
}

/* -------------------------------------------------------------------------- *
 * TEST 11 — RC-B2 会话收尾常驻重建（T = 5 冻结 + 四调用点恰一次 + 既有语义零偏移;
 * 第四调用点 HandleEndVoiceSessionEvent 为 261024-vq-overlay-close-end-session 重钉
 * ——OCE-A2, plan ⑧-2）。
 * MUST-STAY-GREEN 半：三调用点区既有 close token 在世（现状即绿）。
 * RED today：kResidentRebuildDelayS / ScheduleResidentRebuild 全仓零命中。
 * ------------------------------------------------------------------------ */
/* coverage: RC-B2 -- 会话收尾类本地关 → T 秒内恰一次常驻重建排程：kResidentRebuildDelayS 首定义片段含 5（T 冻结值, 改值须回 planner 重冻结）+ 定义消费下界 + 四会话收尾点（:805/:849/:1189 plan 归类表 + HandleEndVoiceSessionEvent 261024-vq-overlay-close-end-session 追加, 各恰 1; builder 复核 :1189 语义后留痕为验收前提）各调 ScheduleResidentRebuild() 恰 1 + 各区既有 close token 零偏移锚 */
TEST_CASE("断线重连常驻: 常驻重建T值冻结 (RC-B2)",
          "[boot_auto_connect][RC-B2][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
        const char *existing_token;
    } sites[] = {
        { "void Application::CloseAudioChannel()",
          "void Application::HandleToggleChatEvent()",
          "CloseAudioChannel(:805)", "protocol_->CloseAudioChannel()" },
        { "void Application::HandleToggleChatEvent()",
          "void Application::HandleStartListeningEvent()",
          "ToggleChat Listening(:849)", "protocol_->CloseAudioChannel()" },
        { "void Application::WakeWordInvoke(",
          "bool Application::CanEnterSleepMode()",
          "WakeWordInvoke Listening(:1189)", "CloseAudioChannel()" },
        /* OCE-A2 追加第四项（261024-vq-overlay-close-end-session）: 播报态关层结束
         * 会话收尾点（TECH-261024 修复, plan ②）——落 HandleStopListeningEvent 与
         * HandleWakeWordDetectedEvent 之间（既有验收切区锚决定, 勿挪）。 */
        { "void Application::HandleEndVoiceSessionEvent()",
          "void Application::HandleWakeWordDetectedEvent()",
          "EndVoiceSession(261024)", "protocol_->CloseAudioChannel()" },
    };
    char *region, *def;
    const char *p5;
    int n;
    size_t i;

    KAC_REQUIRE_ROOT();

    /* tooth 1 (MUST-STAY-GREEN): 四调用点区既有 close token 在世——既有语义零偏移
     * （:805 = K3 [S5] 录音态丢弃面, :849 = toggle 收尾, :1189 = 唤醒词收尾,
     * EndVoiceSession = 261024 关层收尾）。 */
    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = kac_region_between(KAC_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("RC-B2 RED(infra): %s 函数区切取失败——锚漂移须复核", sites[i].label));
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(region, sites[i].existing_token),
            kac_fm("RC-B2 RED: %s 区既有 close token 丢失——既有语义零偏移义务破",
                   sites[i].label));
        free(region);
    }

    /* tooth 2 (RED today): T 冻结——kResidentRebuildDelayS 首定义片段含 5（plan
     * 「退避表首档 5s 量级」定值; 定义须为该 needle 首次出现, [S9]）。 */
    def = kac_region_between(KAC_APP_CC, KAC_REBUILD_DELAY_ND, ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def,
        "RC-B2 RED: application.cc 无 kResidentRebuildDelayS 定义——常驻重建 T 秒"
        "延迟常量未落（[S9]）");
    p5 = def != NULL ? strstr(def, "5") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(p5 != NULL,
        "RC-B2 RED: kResidentRebuildDelayS 首定义片段不含 5——T 值偏离冻结值 5"
        "（退避表首档 5s 量级; 偏离回 planner 重冻结）");
    free(def);

    /* tooth 3 (RED today): 定义 + helper 消费 >= 2 处。 */
    n = kac_count_in_file(KAC_APP_CC, KAC_REBUILD_DELAY_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kac_fm("RC-B2 RED: kResidentRebuildDelayS 在 application.cc 仅 %d 处（下界 2: "
               "定义 + helper 消费）——T 常量未接重建路径", n));

    /* tooth 4 (RED today): 四调用点各调 ScheduleResidentRebuild() 恰 1 次。 */
    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = kac_region_between(KAC_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("RC-B2 RED(infra): %s 函数区切取失败（tooth 4）", sites[i].label));
        n = kac_count_in_slice(region, KAC_B_HELPER_CALL_ND);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            kac_fm("RC-B2 RED: %s 区 ScheduleResidentRebuild() = %d 处（冻结恰 1）——"
                   "会话收尾常驻重建排程缺席（0 = 根因现状）或多次（不双触发破）",
                   sites[i].label, n));
        free(region);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 12 — RC-B3 负锚：五条零重建路径（网络断关/Reboot/OTA/AEC/ResetProtocol）。
 * MUST-STAY-GREEN：全部（负锚实现前 trivially green 申报——B 缝在世前不承载, 见头注
 * 强度申报 RC-B3 行; 变异敏感 = 任一负锚区新增重建排程即红）。
 * ------------------------------------------------------------------------ */
/* coverage: RC-B3 -- 网络断关(:422)/Reboot(:1081)/OTA(:1100)/AEC 切换(:1254)/ResetProtocol(:1267) 五路径零重建排程（ScheduleResidentRebuild 与 TryStartAutoConnect 恒 0）+ 各路径既有 close 语义 token 在世（既有语义零偏移锚; 网络断关恢复走既有 K6 触发点 2, 不自发重建） */
TEST_CASE("断线重连常驻: 负锚五路径零重建 (RC-B3)",
          "[boot_auto_connect][RC-B3][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
        const char *existing_token;
    } paths[] = {
        { "void Application::HandleNetworkDisconnectedEvent()",
          "void Application::HandleActivationDoneEvent()",
          "NetworkDisconnected(:422)", "protocol_->CloseAudioChannel()" },
        { "void Application::Reboot()",
          "bool Application::UpgradeFirmware(",
          "Reboot(:1081)", "protocol_.reset()" },
        { "bool Application::UpgradeFirmware(",
          "void Application::WakeWordInvoke(",
          "UpgradeFirmware(:1100)", "protocol_->CloseAudioChannel()" },
        { "void Application::SetAecMode(",
          "void Application::PlaySound(",
          "SetAecMode(:1254)", "protocol_->CloseAudioChannel()" },
        { "void Application::ResetProtocol()",
          NULL,
          "ResetProtocol(:1267)", "protocol_->CloseAudioChannel()" },
    };
    char *region;
    int n;
    size_t i;

    KAC_REQUIRE_ROOT();

    for (i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        region = kac_region_between(KAC_APP_CC, paths[i].start, paths[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            kac_fm("RC-B3 RED(infra): %s 函数区切取失败——锚漂移须复核"
                   "（禁整函数退役绕过）", paths[i].label));

        /* 既有语义零偏移锚 (MUST-STAY-GREEN): 各区既有 close token 在世。 */
        TEST_ASSERT_TRUE_MESSAGE(kac_contains(region, paths[i].existing_token),
            kac_fm("RC-B3 RED: %s 区既有 close 语义 token 丢失——既有路径语义零偏移"
                   "义务破", paths[i].label));

        /* 负锚 (guard, B 缝在世后承载): 零重建排程。 */
        n = kac_count_in_slice(region, KAC_B_HELPER_CALL_ND);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            kac_fm("RC-B3 RED: %s 区出现 ScheduleResidentRebuild() = %d 处——该路径"
                   "被误归会话收尾（plan 归类表破）或误加重建排程", paths[i].label, n));
        n = kac_count_in_slice(region, KAC_SEAM_CALL_ND);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            kac_fm("RC-B3 RED: %s 区出现 TryStartAutoConnect() = %d 处——零重建路径"
                   "被误接自动重连（网络断关恢复须走既有 K6 触发点 2 网络重连事件）",
                   paths[i].label, n));
        free(region);
    }
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-23 实跑回填）:
 *   通道① 既有 host runner 全量实跑（本文件未接线前, 既有齿零偏移通道）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *       key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *     cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *       ./build/host_infra_test.elf
 *     实跑（2026-09-23）= build 过（ninja no work to do, 既有面零改动）+ **92 Tests
 *     / 0 Failures 0 Ignored**——与上游锚 92/0 @ b869658 零偏移（回归判据成立）。
 *   通道② 本文件新齿红通道（C 类冻结验收实编 + 实跑, DEV-GOVERNANCE 验收测试纪律
 *   §4/§3; 独立线束 = red-baseline/runner.c + unity stub, 不链任何实现——本包零新
 *   实现源可链, 全部结构齿扫真实工作树）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/k6_red_runner \
 *         ../../boot_auto_connect.acceptance.c runner.c
 *     /tmp/k6_red_runner  （exit code = 失败 TEST_CASE 数）
 *     编译通道 error 为零（验收文件本身可编译, 验收测试纪律 §4）; 设计态红判读:
 *       - TEST 1 红 = tooth 3/4 起（auto_connect_in_flight_ / TryStartAutoConnect
 *         全仓零命中——frozen seam not implemented, 真实缺失; tooth 1/2 为
 *         MUST-STAY-GREEN 基线齿先行实跑证明绿）;
 *       - TEST 2 红 = tooth 3 起（OnNetworkError 无拦截分支 + kAutoConnectBackoffS
 *         零命中; tooth 1/2 MUST-STAY-GREEN 先行）;
 *       - TEST 3 红 = tooth 2 起（缝区不存在→预检 needle 全缺; tooth 1 protocol 读
 *         形态在世 MUST-STAY-GREEN 先行）;
 *       - TEST 4 红 = tooth 2 起（同缝区缺席; tooth 1 模板锚 MUST-STAY-GREEN 先行）;
 *       - TEST 5 红 = 首个 call site 守卫齿（四区现状零 in-flight 守卫; 各区既有
 *         语义零改动锚 MUST-STAY-GREEN 先行）;
 *       - TEST 6 红 = tooth 6 起（OnAudioChannelOpened 无 Schedule 包裹 +
 *         boot_auto_connect 注册行缺失; tooth 1-5/8 MUST-STAY-GREEN 先行）。
 *     Unity abort-on-first-fail 语义: 每 TEST 首 红 tooth 即 abort, 同 TEST 后续
 *     解析性必红齿不逐条打印（与 K5 同款判读口径）。
 *   MUST-STAY-GREEN at baseline: TEST 1 tooth 1/2（Connecting 恒 4 + 扫描器自检）、
 *     TEST 2 tooth 1/2、TEST 3 tooth 1、TEST 4 tooth 1、TEST 5 各区既有语义锚、
 *     TEST 6 tooth 1-5/8（canary + protocol 零触碰 + 既有注册面）。
 *   cwd 口径: 结构齿扫真实工作树, root 自手持 cwd 定位（仓根 / red-baseline /
 *   host_app 均可——候选探针上溯 7 级, red-baseline 深度 5 级在域内）。
 *   通道② 实跑（2026-09-23, red-baseline/k6_red_wrapper.sh）: COMPILE_EXIT=0
 *    （-Wall -Wextra 零 error 零 warning）; 实跑 = **6 Tests / 6 Failures**
 *     （RUN_EXIT=6）, 全红 = 设计态红, 逐 TEST 首 红 tooth 与预判一致:
 *     TEST 1 = tooth 3（application.h 无 auto_connect_in_flight_）、TEST 2 = tooth 3
 *    （OnNetworkError 无拦截分支）、TEST 3 = tooth 2（TryStartAutoConnect 区缺席）、
 *     TEST 4 = tooth 2（同）、TEST 5 = ToggleChat 守卫齿（四 call site 现状零
 *     in-flight 守卫）、TEST 6 = tooth 6（OnAudioChannelOpened 无 Schedule 包裹;
 *     tooth 7 注册行缺失被 abort 屏蔽, 解析性必红, builder 接线后转绿）。各 TEST 的
 *     MUST-STAY-GREEN 齿全部先行实跑绿（首红 tooth 位次即证明）。
 *
 *   MUTATION REPLAY MAP（K5 留根①: 六条 expectedRed 补跑手法逐字可执行; builder/
 *   收尾相位在「builder 实现已全绿」态执行——每条 = 变异编辑 + 通道②独立命令 + 原始
 *   输出落 mutation-evidence/<条目>.txt + git checkout 还原）:
 *     通用命令（每条复用; cd firmware/main/test/acceptance/red-baseline && gcc
 *     -std=c11 -Wall -Wextra -I. -o /tmp/k6_red_runner
 *     ../../boot_auto_connect.acceptance.c runner.c && /tmp/k6_red_runner; exit =
 *     失败 TEST_CASE 数; 期望红 TEST 见各条目）:
 *     M1 (K6-A1) 删守卫入口任一条件（如摘 !auto_connect_in_flight_ 或 state==Idle）,
 *        或摘双触发点接线任一处（删 HandleActivationDoneEvent 末尾
 *        TryStartAutoConnect() 调用）, 或在缝区加 SetDeviceState(kDeviceStateConnecting)
 *        互斥形状回归 → 期望 TEST 1 红（tooth 5 三条件缺一 / tooth 8 触发点缺 /
 *        tooth 1 计数 5 ≠ 4）。
 *     M2 (K6-A2) 删 OnNetworkError lambda 拦截分支（in-flight return 摘除）, 或
 *        kAutoConnectBackoffS 表改无限重试（表长 [3] 摘除或退避恒 0）, 或删 task 内
 *        重试前三条件重查 → 期望 TEST 2 红（tooth 3 拦截缺 / tooth 6/7 表形态破 /
 *        TEST 1 tooth 5 双现下界破）。
 *     M3 (K6-A3) 删 url 预检（空 url 直落 xTaskCreate）或预检改 Settings 写
 *        （("websocket", false) → true 或加 SetValue/SetString）→ 期望 TEST 3 红
 *        （tooth 2 只读形参缺 / tooth 4 no-op 前序破 / tooth 5 写 API 命中）。
 *     M4 (K6-A4) task 任一退出路径漏清 auto_connect_in_flight_（唯一共享清点删除）→
 *        期望 TEST 4 红（tooth 3 task 区清点缺 / tooth 4 三态下界破）; 敏感度注记:
 *        若实现为多出口各自清, 单路径摘除可能不降计数——该情形敏感度由 verifier
 *        逐路径核对兜底（acceptance 明文）, 头注强度申报已显式声明缺口。
 *     M5 (K6-A5) 摘任一 call site 守卫（四区任一 auto_connect_in_flight_ 判定删除）
 *        → 期望 TEST 5 红（对应区守卫齿; 循环体四区逐一独立断言, 摘哪区红哪条）。
 *     M6 (K6-A6) 摘 HOST_ACCEPTANCE_SRCS 注册行或摘 OnAudioChannelOpened 的
 *        Schedule() 包裹 → 期望 TEST 6 红（tooth 7 注册缺 / tooth 6 包裹缺）。
 * --------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（RC 批, 261010-device-reconnect-on-server-close 包,
 * acceptance-author 2026-10-11 实跑回填）:
 *   通道① 全量 host runner（taskpackage redBaselineCommand 逐字）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *       espidf_wsl.sh build
 *     cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *       ./build/host_infra_test.elf
 *   通道② standalone harness（与 K6 通道②同款, red-baseline/k6_red_wrapper.sh）:
 *     cd firmware/main/test/acceptance/red-baseline && gcc -std=c11 -Wall -Wextra
 *       -I. -o /tmp/rc_red_runner ../../boot_auto_connect.acceptance.c runner.c
 *       && /tmp/rc_red_runner  （exit code = 失败 TEST_CASE 数）
 *   设计态红判读（TEST 7-12; TEST 1-6 既有齿全绿零破坏 = 通道①既有面零偏移）:
 *     TEST 7 (RC-A1) 红 = tooth 3 起（local_close_pending_ 全仓零命中——frozen seam
 *       not implemented, 真实缺失; tooth 1/2 MUST-STAY-GREEN 先行）;
 *     TEST 8 (RC-A2) 红 = tooth 2 起（分类写点计数 0 ≠ 1; tooth 1 先行）;
 *     TEST 9 (RC-A3) 红 = tooth 1 起（helper 声明/定义零命中）;
 *     TEST 10 (RC-B1) 绿（设计态绿: 板级零休眠接线是现状结构事实, 齿 = 钉死不回潮;
 *       tooth 1 canary 证明零命中非 vacuous）;
 *     TEST 11 (RC-B2) 红 = tooth 2 起（kResidentRebuildDelayS 零命中; tooth 1 既有
 *       close token 锚先行）;
 *     TEST 12 (RC-B3) 绿（负锚实现前 trivially green; B 缝在世后承载）。
 *   期望冻结相位通道②: 12 Tests / 4 Failures（TEST 7/8/9/11 红, 其余绿）。
 *
 *   MUTATION REPLAY MAP（RC 批, 冻结轮 vacuity 自证; 手法 = 假实现全绿态 + 单点变异
 *   令齿红 + 还原复跑回绿 + sha256 核对还原一致; 通道②独立命令, 变异编辑不得顺手
 *   改动既有齿与未列面）:
 *     假实现（全绿态, 四文件最小缝实现）: ws.h 加 local_close_pending_ 成员; ws.cc
 *       CloseAudioChannel 置位 / OpenAudioChannel 清位 / OnDisconnected 消费后回调;
 *       app.cc A 区 Schedule 体尾接 TryStartAutoConnect() + kResidentRebuildDelayS=5
 *       定义 + ScheduleResidentRebuild 定义（task 延迟 T 后 Schedule TryStartAutoConnect）
 *       + application.h 声明 + 三调用点各接 helper 一次 → 通道② 12 Tests / 0 Failures。
 *     M7 (RC-A1) 摘 A 区 Schedule 体尾 TryStartAutoConnect() → 期望 TEST 7 红
 *        （tooth 7 计数 0 ≠ 1）。
 *     M8 (RC-A2) 摘 OnDisconnected 区 local_close_pending_ 消费块（本地关直走回调）→
 *        期望 TEST 8 红（tooth 3 位次破; TEST 7 tooth 6 连带红 = 同缝独立承载正常）。
 *     M9 (RC-A3) helper 内改直调 protocol_->OpenAudioChannel()（绕过 AutoConnect
 *        task）→ 期望 TEST 9 红（tooth 3 直连 needle; tooth 2 复用 needle 连带）。
 *     M10 (RC-B1) 板文件加一行 `#include "power_save_timer.h"` → 期望 TEST 10 红
 *        （tooth 2 零接线 needle 破）。
 *     M11 (RC-B2) kResidentRebuildDelayS 定义值 5 改 10 → 期望 TEST 11 红（tooth 2
 *        首定义片段无 5, T 冻结破）。
 *     M12 (RC-B3) Reboot 区加 ScheduleResidentRebuild() 调用 → 期望 TEST 12 红
 *        （Reboot 区 helper 计数 1 ≠ 0; TEST 8 tooth 6 恰 4 破连带 = 泄漏面独立承载）。
 * --------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * 重钉留痕（261024-vq-overlay-close-end-session, acceptance-author 2026-10-24;
 * tighten-only 纯 additive/等值重钉, 语义零放松）:
 *   真机事故 TECH-261024（播报态关层只打断未结束会话）新增第四会话收尾点
 *   HandleEndVoiceSessionEvent（播报态关层统一结束入口, plan ②）, 两处既有断言重钉:
 *     ① RC-A2 tooth 6（OCE-A2, plan ⑧-2）: 全文件 ScheduleResidentRebuild() 恰 4 →
 *        恰 5（定义 + 四调用点）;
 *     ② RC-B2 sites 数组追加第四项（OCE-A2, plan ⑧-2）: void Application::
 *        HandleEndVoiceSessionEvent() → HandleWakeWordDetectedEvent()（下一函数锚,
 *        位置由 plan ② 既有验收切区锚决定勿挪）, tooth 1/4 对四项各恰 1。
 *   义务红形态偏移（相对 2026-10-11 RC 批判读口径）:
 *     - TEST 11 (RC-B2) 今天首红 = tooth 1 RED(infra)（第四项 start needle 零命中,
 *       函数未落; 既有三项 close token 锚先行绿）; builder 落地后 tooth 1 全绿,
 *       tooth 2 起仍按原口径;
 *     - TEST 8 (RC-A2) 今天首红 = tooth 6（计数 4 ≠ 5）; builder 落地后转绿;
 *     - MUTATION REPLAY MAP M12 注: 「TEST 8 tooth 6 恰 4 破连带」今天读作
 *       「恰 5 破连带」（Reboot 区加排程 → 计数 6 ≠ 5, 判读不变）。
 *   范围申报（planner 备读, advisory）: RC-A2 tooth 5 零直调 TryStartAutoConnect
 *   守卫数组维持三站点未加第四项（按该包 plan ⑧-2 冻结口径; 新收尾点经 helper 调用,
 *   守卫不承载——若 planner 判定需同守第四点, 走 tighten-only 返钉）。
 *   2026-10-24  补标记（verifier round 1 acceptance-coverage 闸红, changeId
 *               261024-vq-overlay-close-end-session）: TEST 8 头注补 OCE-A2 coverage
 *               标记（RC-A2 tooth6 计数 4→5 + RC-B2 sites 第四项承载面登记）+
 *               OCE-A3 回归网登记标记——纯注释行, 零断言语义变更、零测试计数变化
 *               （acceptance-author, verifier round 1 派单）。
 *   2026-10-24  头注等值修注（changeId 261024-vq-overlay-close-end-session）:
 *               TEST 8 (RC-A2) 头注 coverage 行存量「ScheduleResidentRebuild() 恰 4 处
 *               （定义 + 三调用点）」措辞滞后于 tooth 6 重钉断言（恰 5、定义 + 四调用
 *               点, 第四点 = HandleEndVoiceSessionEvent）——同步修注, 纯注释行、零断言
 *               逻辑变更、零测试计数变化。
 * --------------------------------------------------------------------------- */
