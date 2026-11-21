#include "application.h"
#include "board.h"
#include "display.h"
#include "system_info.h"
#include "audio_codec.h"
#include "mqtt_protocol.h"
#include "websocket_protocol.h"
#include "provisioning/ws_provision.h"
#include "certs/ws_tls_ca.h"
#include "assets/lang_config.h"
#include "mcp_server.h"
#include "assets.h"
#include "settings.h"
/* 归一屏（包 260911-screen-unification）：前台归属 = display/kidpal_layer layer 栈
 * 单一权威；本文件在状态跃迁处事件驱动语音层进出 + 聊天层驻留清除时机（OQ3=c）。 */
#include "display/kidpal_layer.h"
#include "display/kidpal_render_node.h"
/* 语音轮场景映射 + 话语回显纯逻辑单源 (包 260915-m1-k3-voice-round-ui, seam [1]/[2]):
 * stt 分支同刻消费 kidpal_voice_round_scene 决策表驱动思考页弹出 + SetChatMessage(user)
 * 既有回显通道。设备侧零重抄映射。 */
#include "display/voice_round.h"
#include "display/kidpal_voice_states.h"
#include "display/story_page.h"  /* K3: source_page 映射/引导态/回显/记录语境标志冻结缝 */
#include "display/points_page.h" /* 261104-k2-points-device-ui: 积分子态 + selected_id
                                  * 只读注入决策（详细账本语境 source_page 扩点） */
#include "display/keybar.h"      /* 261022-p3-record-chrome-ghost-fix CG-A1: 键帽场景单源
                                  * 收尾沿复位 kidpal_keybar_scene_reset */

/* K3 (260924-m4-k3-record-session-live): 记录会话消费状态（文件级静态; 单写者——
 * on_vad_change 回调写 speaking / 主循环读, 32 位标量, 与既有跨任务宽松度同级）。
 * s_story_rec_entered = 引导态->录音态已切换镜像（幂等防重切; Idle 收尾与标志同步清）。 */
void KidpalStoryVadEnterRecording();  /* 视图动作桥, 定义住 mcp_server.cc（mcp_server.h
                                       * 冻结面零改动——变更清单不含该头, 免扩公共面） */
/* K3R [R2] 会话收尾消费侧桥（261001-m4-k3-record-session-live-repair, D4b-1; 定义
 * 住 mcp_server.cc, 同款免扩公共头纪律）: 读 story 视图页态供 kidpal_story_session_end
 * 判据 + 复位录音页态并停录音演出定时器。 */
int  KidpalStoryViewPageState();
void KidpalStorySessionEndApply(int list_page);
/* RSL2-A2 [I1]（261028-p3-record-session-mirror-desync-fix, M2）: 收尾沿镜像 := 权威视图
 * 页态的确定性同步桥（定义住 mcp_server.cc, 同款免扩公共头纪律）。 */
void KidpalStorySessionEndSync();
/* [G3] 帧 B 子态沿桥（261020 D4-5, fixer; 定义住 mcp_server.cc, 同款免扩公共头纪律）:
 * 引导播报中子态从设备侧既有 tts 帧派生（记录语境内引擎抑制 LLM/TTS, kidpal_speak
 * 引导注入 = 唯一 TTS 源, 零 wire 新增）——tts start 沿置 rec_substate=1、tts stop 沿
 * 复位 0（幂等, 仅 RECORDING 页态有语义）。RDG-F2 键位分叉 / RDG-F3 整理态吞键零触碰
 * （桥只写子态标量, 分叉判定仍归 story_page_input 冻结决策表）。 */
void KidpalStoryGuideSpeechEdge(int speaking);
/* 引导播报看门狗「音频感知」标志（真机轮追补 261022 r2; 定义面 = 本文件音频收包路径,
 * 消费方 = mcp_server.cc StoryGuideWatchdogCb）。语义: 本次引导播报窗口内是否有真实
 * opus 音频包进入解码队列——孤儿 tts start（其音频已被引擎 abort 清掉）恒 false,
 * 真句子引擎 TTS 首包实测 ~1-2s 内到达恒 true。extern "C" + 消费者 extern 声明, 与
 * 既有 KidpalStoryGuideSpeechEdge 桥同款「不扩公共头」纪律（mcp_server.h 冻结面零改）。
 * 非原子布尔: 写方 = 协议接收任务 OnIncomingAudio 回调, 读方 = LVGL 定时器任务
 * （看门狗到期）。单字节 store/load 在 Xtensa 上不可撕裂; 语义上只需「窗口内见过」的
 * 一次性单调置位（布防时由消费方清 false）, 无跨字段次序依赖——与 g_story_* 既有
 * 跨任务宽松度同级。形式说明: 先 `extern "C"` 声明定链接、后一行定义（单行
 * `extern "C" bool X = false;` 会触发 -Wextern-initializer, 故拆两行; C 链接由前置
 * 声明继承）。 */
extern "C" bool g_story_guide_audio_seen;
bool g_story_guide_audio_seen = false;
static bool s_vad_speaking = false;
static bool s_story_rec_entered = false;

/* 261116-channel-resident-core（真机轮 B 修复轮 tighten, 2026-11-18）: 状态迁移的
 * 旧状态快照。写者 = Initialize 注册的状态迁移监听器（DeviceState 枚举标量，
 * 与 s_vad_speaking 同级宽松度）；读者 = HandleStateChangedEvent 的 Listening case
 * 尾部复位分派。初值 kDeviceStateIdle = 「新会话」路径。
 * 用途：区分会话内 TTS 往返（旧状态 == Speaking -> 软复位，保 asr_seen）与跨会话
 * 入口（七字段全清）。旧状态比较**写在 Listening case 内**是冻结接线形态（冻结件
 * EQ-4）：上移到监听器算布尔属语义等价写法但会让 SCR-A15 红，故不得改形态。 */
static DeviceState s_watch_prev_state = kDeviceStateIdle;

/* 261014 VR-A2/A3 轮来源采样（fork② 判别器依据）: sentence_start 分支进 Speaking 前取
 * 真实设备态、每轮重置（single write point）; tts stop 出口据其判「出会话 speak 不回
 * 聆听」。文件级静态（单 Application 实例, 单写者 = sentence_start 回调 / 主循环读,
 * 与 s_vad_speaking 同级宽松度）——本包 changeManifest 未含 application.h, 故不落类
 * 成员以守住文件级写权边界。 */
static DeviceState speaking_round_entered_from_ = kDeviceStateIdle;

#include <cstring>
#include <esp_log.h>
#include <cJSON.h>
#include <driver/gpio.h>
#include <arpa/inet.h>
#include <font_awesome.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_sntp.h>
#include <sys/time.h>
#include <time.h>          /* 261022-diag-log-content: localtime_r / struct tm（时间前缀产出） */
#include "time_sync.h"
#include "diagnostics/log_semantics.h" /* 261022-diag-log-content: 时间前缀缓存产出 + SystemInfo 降频决策 */
#include "session_voice_watch.h" /* 261116 真机轮 B 修复轮: 语音看门缝（判据本体住纯逻辑 C 源） */

/* 261116-channel-resident-core（真机轮 B 修复轮）: 语音看门缝实例与阈值配置。
 * file-static 而非 Application 成员——本包 changeManifest 不含 application.h
 * （成员化 = 公共头零 diff 破）。单写者 = 主循环（CLOCK_TICK tick / Listening 入口
 * reset / stt 分支 lambda 内 note_asr 都在主循环上下文），与 s_vad_speaking 同级
 * 宽松度；s_vad_speaking 本身仍由 VAD 回调任务写、主循环读（32 位标量）。 */
static kidpal_voice_watch_t s_voice_watch;
static kidpal_voice_watch_cfg_t s_voice_watch_cfg;

/* 诊断行跳变快照（上一次 emit 时的电平 / 记录语境 / 录音页镜像 / suspect；
 * 初值 -1 = 首次必判为跳变）。快照刻意放 application.cc 而非缝结构尾部：缝结构
 * 保持冻结件镜像的七字段原形，诊断展示态不混进判据状态机。 */
static int s_vw_diag_vad = -1;
static int s_vw_diag_rec_ctx = -1;
static int s_vw_diag_rec_mat = -1;
static int s_vw_diag_suspect = -1;

#define TAG "Application"

/* -------------------------------------------------------------------------- *
 * K5（TaskPackage 260922-m3-k5-sntp-time-sync）[S2] SNTP 校时挂点：
 * WiFi got IP 后异步启动（HandleNetworkConnectedEvent -> KidPalStartSntpTimeSync），
 * 不阻塞激活/主流程——esp_sntp_init 只配 server 列表与起 lwip SNTP 轮询任务（自身
 * 异步），后续同步监测在独立低优先级 task 内以 kidpal_ts_backoff_delay_ms 退避节奏
 * 轮询（失败不重试风暴：SNTP 内部重试恒开, 监测只观测不重启）。TZ 走 POSIX `CST-8`
 * （UTC+8, F4 符号反转; 勿照抄 emote_display.cc `GMT+0` 错误先例）。OTA 校时通道
 * 不复活（K5-E2）：本文件零 CheckVersion 调用, ota.cc server_time 路径保持死代码。
 * 板级写回（SNTP 同步成功 -> PCF85063）由板文件经 esp_sntp_set_time_sync_notification_cb
 * 承接（见 boards/waveshare-s3-rlcd-4.2.cc），应用层不触碰 I2C。
 * ------------------------------------------------------------------------ */
static bool s_sntp_started = false;

/* -------------------------------------------------------------------------- *
 * K6（TaskPackage 260923-m3-k6-boot-auto-connect）[S3] 自动建连退避决策表:
 * 2026-11-19（261117-session-end-intent-longretry P2）由「3 次即止、递增秒序列
 * 5s/10s/20s」改为封顶递增长跑——先按递增秒序列逐档退避, 档位耗尽后按稳态周期
 * 恒等退避、无上限（arch §6.4「{5,10,20,40,60} 递增后恒 60 秒」）。host 齿钉决策表
 * 形态不钉时序, 实时节奏归真机窗 K6-A7 / B4; vTaskDelay 换算乘法形态见
 * TryStartAutoConnect task（换算按 1 秒分片重查意向位 / 网络断退出请求位）。
 * ------------------------------------------------------------------------ */
static const int kAutoConnectBackoffS[] = { 5, 10, 20, 40, 60 };
/* 封顶递增档位耗尽后的稳态重试周期（秒）——长跑无上限, 恒以本周期退避。 */
static const int kAutoConnectSteadyRetryS = 60;

/* -------------------------------------------------------------------------- *
 * 261116-channel-resident-core（P1 T2）会话空闲超时阈值: 聆听态连续 60 秒无语音
 * （VAD 电平恒静 + 非记录语境）即自动结束会话（requirements §2.1 结束三通道①）。
 * 位置接替 P1 删除的常驻重建延迟常量（同区）。值 60 冻结进验收件
 * session_channel_resident.acceptance.c SCR-A5；改值须回 planner 重冻结。
 * 说明: 这是**唯一**的 60s 阈值常量——producer 只在本文件主循环 CLOCK_TICK 区消费,
 * 不接纯模型会话状态机事件（voice_round_ui VQ-E1 tooth2 钉全 firmware/main 的该事件
 * 符号命中文件数恰 2 = session_state_machine.h/.cc）。
 * ------------------------------------------------------------------------ */
static const int kSessionIdleTimeoutS = 60;

/* -------------------------------------------------------------------------- *
 * 261116-channel-resident-core（真机轮 A 修复轮 D3）空素材会话空闲收尾阈值:
 * 记录语境会话在聆听态**从未收到任何素材**（VAD 起音沿未置位素材信号）且静置
 * 60 秒即自动结束会话——只释放、不归档（空素材会话没有引导链，静置不结束是轮 A
 * 实测缺陷）。阈值与 requirements §2.1 的单一 60s 口径对齐，不引入第二个魔数；
 * 值 60 冻结进验收件 session_channel_resident.acceptance.c 的 SCR-A8，改值须回
 * planner 重冻结。注意注释刻意不复述既有无语音超时常量的符号字面（SCR-A5 的区切
 * 以该字面首现为起点，注释写回会抢走首现点 -> 假红）。
 * ------------------------------------------------------------------------ */
static const int kStoryEmptySessionTimeoutS = 60;

/* -------------------------------------------------------------------------- *
 * 261022-diag-log-content（板端诊断日志内容增强）SystemInfo 打印门控状态：
 * 上一次放行的单调毫秒（单写者 = main event loop，与 clock_ticks_ 同上下文）。降频
 * 决策本体归 diagnostics/log_semantics.c kidpal_log_sysinfo_due（间隔 = 头文件
 * KIDPAL_LOG_SYSINFO_INTERVAL_MS 单源 60s，调用点不得裸写字面）。
 * ------------------------------------------------------------------------ */
static long long s_sysinfo_last_print_ms = 0;

/* SNTP 同步监测 task：成功即退出；失败按 [S1] 退避决策表递增观测间隔（封顶 32s）。 */
static void KidPalSntpMonitorTask(void *arg)
{
    int failures = 0;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(kidpal_ts_backoff_delay_ms(failures)));
        time_t now = time(NULL);
        struct tm tm_local;
        localtime_r(&now, &tm_local);
        if (kidpal_ts_year_ok(tm_local.tm_year)) {
            char corr_id[24];
            snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            ESP_LOGI(TAG, "sntp_time_sync corr_id=%s ok year=%d", corr_id,
                     tm_local.tm_year + 1900);
            break; /* 同步成功, 监测任务退出（SNTP 按配置周期继续保时） */
        }
        failures++;
        if (failures <= 3 || failures % 10 == 0) { /* 退避降噪: 前几次 + 低频续报 */
            char corr_id[24];
            snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            ESP_LOGW(TAG, "sntp_time_sync corr_id=%s not_synced failures=%d "
                          "next_backoff_ms=%d", corr_id, failures,
                     kidpal_ts_backoff_delay_ms(failures));
        }
    }
    vTaskDelete(NULL);
}

static void KidPalStartSntpTimeSync()
{
    if (s_sntp_started) {
        return; /* 幂等: WiFi 重连反复触发本路径, SNTP 只起一次 */
    }
    s_sntp_started = true;

    /* POSIX TZ `CST-8` = UTC+8（F4: POSIX 符号反转——西半球为正, 东八区写 8）。 */
    setenv("TZ", "CST-8", 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "ntp.aliyun.com");   /* 主（plan 关键接缝） */
    esp_sntp_setservername(1, "ntp1.aliyun.com");  /* 备 1 */
    esp_sntp_setservername(2, "pool.ntp.org");     /* 备 2 */
    esp_sntp_init(); /* 异步起 lwip SNTP 轮询, 不阻塞调用方（主事件循环） */

    char corr_id[24];
    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
    ESP_LOGI(TAG, "sntp_time_sync corr_id=%s started servers=ntp.aliyun.com,ntp1.aliyun.com,pool.ntp.org tz=CST-8",
             corr_id);

    xTaskCreate(KidPalSntpMonitorTask, "sntp_mon", 4096, nullptr, 1, nullptr);
}



Application::Application() {
    event_group_ = xEventGroupCreate();

#if CONFIG_USE_DEVICE_AEC && CONFIG_USE_SERVER_AEC
#error "CONFIG_USE_DEVICE_AEC and CONFIG_USE_SERVER_AEC cannot be enabled at the same time"
#elif CONFIG_USE_DEVICE_AEC
    aec_mode_ = kAecOnDeviceSide;
#elif CONFIG_USE_SERVER_AEC
    aec_mode_ = kAecOnServerSide;
#else
    aec_mode_ = kAecOff;
#endif

    esp_timer_create_args_t clock_timer_args = {
        .callback = [](void* arg) {
            Application* app = (Application*)arg;
            xEventGroupSetBits(app->event_group_, MAIN_EVENT_CLOCK_TICK);
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "clock_timer",
        .skip_unhandled_events = true
    };
    esp_timer_create(&clock_timer_args, &clock_timer_handle_);
}

Application::~Application() {
    if (clock_timer_handle_ != nullptr) {
        esp_timer_stop(clock_timer_handle_);
        esp_timer_delete(clock_timer_handle_);
    }
    vEventGroupDelete(event_group_);
}

bool Application::SetDeviceState(DeviceState state) {
    return state_machine_.TransitionTo(state);
}

void Application::Initialize() {
    auto& board = Board::GetInstance();
    SetDeviceState(kDeviceStateStarting);

    // Setup the display
    auto display = board.GetDisplay();

    // Print board name/version info
    display->SetChatMessage("system", SystemInfo::GetUserAgent().c_str());

    // Setup the audio service
    auto codec = board.GetAudioCodec();
    audio_service_.Initialize(codec);
    audio_service_.Start();

    AudioServiceCallbacks callbacks;
    callbacks.on_send_queue_available = [this]() {
        xEventGroupSetBits(event_group_, MAIN_EVENT_SEND_AUDIO);
    };
    callbacks.on_wake_word_detected = [this](const std::string& wake_word) {
        xEventGroupSetBits(event_group_, MAIN_EVENT_WAKE_WORD_DETECTED);
    };
    callbacks.on_vad_change = [this](bool speaking) {
        /* K3 [S2]: VAD 沿值留档（引导态开口切录音态决策入参; notes ⑨ UTT_START
         * 事件沿——主循环消费, 零新增计时器）。 */
        s_vad_speaking = speaking;
        xEventGroupSetBits(event_group_, MAIN_EVENT_VAD_CHANGE);
    };
    audio_service_.SetCallbacks(callbacks);

    // Add state change listeners
    state_machine_.AddStateChangeListener([this](DeviceState old_state, DeviceState new_state) {
        /* 旧状态快照供 Listening 入口按迁移来源分派复位（会话内 TTS 往返 vs 新会话）。 */
        s_watch_prev_state = old_state;
        xEventGroupSetBits(event_group_, MAIN_EVENT_STATE_CHANGED);
    });

    // Start the clock timer to update the status bar
    esp_timer_start_periodic(clock_timer_handle_, 1000000);

    /* 261116-channel-resident-core（真机轮 B 修复轮）: 语音看门缝阈值单源注入。
     * cfg_default 填三件头宏阈值（sustain / asr 新鲜窗 / 电平可信窗）；空闲阈值由
     * 本处注入既有命名常量——**唯一注入点**，两侧各写一个 60 = 两个阈值各自漂移
     * （SCR-A15 ⑤ 钉这三件同区在世）。 */
    kidpal_voice_watch_cfg_default(&s_voice_watch_cfg);
    s_voice_watch_cfg.idle_timeout_s = kSessionIdleTimeoutS;

    // Add MCP common tools (only once during initialization)
    auto& mcp_server = McpServer::GetInstance();
    mcp_server.AddCommonTools();
    mcp_server.AddUserOnlyTools();

    // Set network event callback for UI updates and network state handling
    board.SetNetworkEventCallback([this](NetworkEvent event, const std::string& data) {
        auto display = Board::GetInstance().GetDisplay();
        
        switch (event) {
            case NetworkEvent::Scanning:
                display->ShowNotification(Lang::Strings::SCANNING_WIFI, 30000);
                xEventGroupSetBits(event_group_, MAIN_EVENT_NETWORK_DISCONNECTED);
                break;
            case NetworkEvent::Connecting: {
                if (data.empty()) {
                    // Cellular network - registering without carrier info yet
                    display->SetStatus(Lang::Strings::REGISTERING_NETWORK);
                } else {
                    // WiFi or cellular with carrier info
                    std::string msg = Lang::Strings::CONNECT_TO;
                    msg += data;
                    msg += "...";
                    display->ShowNotification(msg.c_str(), 30000);
                }
                break;
            }
            case NetworkEvent::Connected: {
                std::string msg = Lang::Strings::CONNECTED_TO;
                msg += data;
                display->ShowNotification(msg.c_str(), 30000);
                xEventGroupSetBits(event_group_, MAIN_EVENT_NETWORK_CONNECTED);
                break;
            }
            case NetworkEvent::Disconnected:
                xEventGroupSetBits(event_group_, MAIN_EVENT_NETWORK_DISCONNECTED);
                break;
            case NetworkEvent::WifiConfigModeEnter:
                // WiFi config mode enter is handled by WifiBoard internally
                break;
            case NetworkEvent::WifiConfigModeExit:
                // WiFi config mode exit is handled by WifiBoard internally
                break;
            // Cellular modem specific events
            case NetworkEvent::ModemDetecting:
                display->SetStatus(Lang::Strings::DETECTING_MODULE);
                break;
            case NetworkEvent::ModemErrorNoSim:
                Alert(Lang::Strings::ERROR, Lang::Strings::PIN_ERROR, "triangle_exclamation", Lang::Sounds::OGG_ERR_PIN);
                break;
            case NetworkEvent::ModemErrorRegDenied:
                Alert(Lang::Strings::ERROR, Lang::Strings::REG_ERROR, "triangle_exclamation", Lang::Sounds::OGG_ERR_REG);
                break;
            case NetworkEvent::ModemErrorInitFailed:
                display->SetStatus(Lang::Strings::DETECTING_MODULE);
                display->SetChatMessage("system", Lang::Strings::DETECTING_MODULE);
                break;
            case NetworkEvent::ModemErrorTimeout:
                display->SetStatus(Lang::Strings::REGISTERING_NETWORK);
                break;
        }
    });

    // Start network asynchronously
    board.StartNetwork();

    // Update the status bar immediately to show the network state
    display->UpdateStatusBar(true);
}

void Application::Run() {
    const EventBits_t ALL_EVENTS = 
        MAIN_EVENT_SCHEDULE |
        MAIN_EVENT_SEND_AUDIO |
        MAIN_EVENT_WAKE_WORD_DETECTED |
        MAIN_EVENT_VAD_CHANGE |
        MAIN_EVENT_CLOCK_TICK |
        MAIN_EVENT_ERROR |
        MAIN_EVENT_NETWORK_CONNECTED |
        MAIN_EVENT_NETWORK_DISCONNECTED |
        MAIN_EVENT_TOGGLE_CHAT |
        MAIN_EVENT_START_LISTENING |
        MAIN_EVENT_STOP_LISTENING |
        MAIN_EVENT_ACTIVATION_DONE |
        MAIN_EVENT_STATE_CHANGED |
        MAIN_EVENT_END_VOICE_SESSION;

    while (true) {
        auto bits = xEventGroupWaitBits(event_group_, ALL_EVENTS, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & MAIN_EVENT_ERROR) {
            SetDeviceState(kDeviceStateIdle);
            Alert(Lang::Strings::ERROR, last_error_message_.c_str(), "circle_xmark", Lang::Sounds::OGG_EXCLAMATION);
        }

        if (bits & MAIN_EVENT_NETWORK_CONNECTED) {
            HandleNetworkConnectedEvent();
        }

        if (bits & MAIN_EVENT_NETWORK_DISCONNECTED) {
            HandleNetworkDisconnectedEvent();
        }

        if (bits & MAIN_EVENT_ACTIVATION_DONE) {
            HandleActivationDoneEvent();
        }

        if (bits & MAIN_EVENT_STATE_CHANGED) {
            HandleStateChangedEvent();
        }

        if (bits & MAIN_EVENT_TOGGLE_CHAT) {
            HandleToggleChatEvent();
        }

        /* 261024-vq-overlay-close-end-session（TECH-261024 修复）: 关层结束会话分支
         * 位次冻结——在 SCHEDULE 之前（关层手势的 dismiss 同轮落定回原页, 先于
         * voice_end 收尾; 详见 HandleEndVoiceSessionEvent 定义处时序自证）。 */
        if (bits & MAIN_EVENT_END_VOICE_SESSION) {
            HandleEndVoiceSessionEvent();
        }

        if (bits & MAIN_EVENT_START_LISTENING) {
            HandleStartListeningEvent();
        }

        if (bits & MAIN_EVENT_STOP_LISTENING) {
            HandleStopListeningEvent();
        }

        if (bits & MAIN_EVENT_SEND_AUDIO) {
            while (auto packet = audio_service_.PopPacketFromSendQueue()) {
                if (protocol_ && !protocol_->SendAudio(std::move(packet))) {
                    break;
                }
            }
        }

        if (bits & MAIN_EVENT_WAKE_WORD_DETECTED) {
            HandleWakeWordDetectedEvent();
        }

        if (bits & MAIN_EVENT_VAD_CHANGE) {
            if (GetDeviceState() == kDeviceStateListening) {
                /* 261116-channel-resident-core（P1 T2）: 说话沿重置 60s 空闲计时——
                 * 孩子一开口计时即清零（否则上一段静默累积到 60 会误收）。
                 * 电平判据: 本回调只在沿上产生事件；起音沿 s_vad_speaking == true。
                 * 不另写状态迁移归零（HandleStateChangedEvent 已承载）。 */
                if (s_vad_speaking) {
                    clock_ticks_ = 0;
                }
                auto led = Board::GetInstance().GetLed();
                led->OnStateChanged();
                /* K3 [S2]: 开口切录音态（S-F2.2-AC1 第一句即进录音态; notes ⑨）——
                 * 决策住 story_page.c 纯逻辑缝 kidpal_story_recording_enter, 应用层
                 * 只供页态镜像 + speaking 沿; 决策 1 -> 视图动作经 mcp_server 桥
                 * （复用 start_recording 冻结消费面）。AFE VAD 真沿 = live 目击项
                 * （host 不可机判, 不造 offline 假齿）。 */
                if (kidpal_story_session_ctx_active()) {
                    int story_page = s_story_rec_entered ? KSTORY_PAGE_RECORDING
                                                         : KSTORY_PAGE_LIST;
                    if (kidpal_story_recording_enter(story_page, 1,
                                                     s_vad_speaking ? 1 : 0) == 1 &&
                        !s_story_rec_entered) {
                        s_story_rec_entered = true;
                        KidpalStoryVadEnterRecording();
                    }
                }
            }
        }

        if (bits & MAIN_EVENT_SCHEDULE) {
            std::unique_lock<std::mutex> lock(mutex_);
            auto tasks = std::move(main_tasks_);
            lock.unlock();
            for (auto& task : tasks) {
                task();
            }
        }

        if (bits & MAIN_EVENT_CLOCK_TICK) {
            clock_ticks_++;
            auto display = Board::GetInstance().GetDisplay();
            display->UpdateStatusBar();

            /* 261022-diag-log-content [DLG-C4]⑤（R1 缓解的**产出半**）：1s clock tick 把
             * 当前墙上时间预格式化进日志前缀缓存——esp_log 钩子（diagnostics/sd_log_fs.cc
             * KidpalSdLogVprintf）只取现成串 memcpy 拼接，**不在钩子内**做 localtime_r /
             * snprintf（钩子可在任意任务 / 中断上下文被调，重入与时延风险即 plan R1）。
             * synced 判定 = 既有 K5 时间可信门 kidpal_ts_year_ok（系统时间未过 2025 门 =
             * SNTP/RTC 未对齐）-> 缓存降级为 "[BOOT+<ms>ms] "，至少保住开机后的先后序；
             * 真实判定来源的残差归真机轮 DLG-A6①（GAP-2）。 */
            {
                struct timeval tv_now;
                struct tm tm_now = {};

                gettimeofday(&tv_now, nullptr);
                localtime_r(&tv_now.tv_sec, &tm_now);
                kidpal_log_time_cache_update(
                    kidpal_ts_year_ok(tm_now.tm_year),
                    tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                    tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
                    static_cast<unsigned long long>(esp_timer_get_time() / 1000));
            }

            /* SystemInfo 降频（plan 改动点 6）：原「每 10 秒一条 SRAM 统计」占导出日志
             * 篇幅约 70%，改走 kidpal_log_sysinfo_due 决策单源（60s 间隔，头文件宏）。
             * 首条在开机 60s 后放行（上次放行记 0 = 开机起点）。 */
            {
                long long now_ms = static_cast<long long>(esp_timer_get_time() / 1000);

                if (kidpal_log_sysinfo_due(now_ms, s_sysinfo_last_print_ms,
                                           KIDPAL_LOG_SYSINFO_INTERVAL_MS) == 1) {
                    s_sysinfo_last_print_ms = now_ms;
                    SystemInfo::PrintHeapStats();
                }
            }

            /* 261116-channel-resident-core（真机轮 B 修复轮）: 语音看门缝每 1 秒恰一次
             * 推进（判据本体住 session_voice_watch.c，本区只留接线）。电平入参只作
             * 「连续段时长」输入——它不是收尾的必要条件；收尾主累加器 = 缝内 quiet_s
             * （距上次有效语音证据的秒数）。两条 producer 共用本 tick 的返回值。 */
            int vw_act = kidpal_voice_watch_tick(&s_voice_watch, s_vad_speaking ? 1 : 0,
                                                 &s_voice_watch_cfg);

            /* 轮 B 可观测性（human 必办）: 低频聆听诊断行。触发 = 缝纯函数
             * kidpal_voice_watch_diag_due（周期 10s + 跳变，最小间隔 3s 封顶；
             * 模数走缝自持 tick 计数——本区不出现裸的 clock_ticks_ 取模形态，
             * diag_log DLG-A4⑤ 与本包 SCR-A14 双钉）。非聆听态零输出。
             * 跳变源 = 电平 / 记录语境 / 录音页镜像 / suspect 与上次 emit 快照比对。 */
            {
                int vw_vad = s_vad_speaking ? 1 : 0;
                int vw_rec_ctx = kidpal_story_session_ctx_active() ? 1 : 0;
                int vw_rec_mat = s_story_rec_entered ? 1 : 0;
                int vw_changed = (vw_vad != s_vw_diag_vad) ||
                                 (vw_rec_ctx != s_vw_diag_rec_ctx) ||
                                 (vw_rec_mat != s_vw_diag_rec_mat) ||
                                 (s_voice_watch.suspect != s_vw_diag_suspect);

                if (GetDeviceState() == kDeviceStateListening &&
                    kidpal_voice_watch_diag_due(s_voice_watch.tick_s,
                                                s_voice_watch.diag_last_s,
                                                KIDPAL_VW_DIAG_INTERVAL_S,
                                                KIDPAL_VW_DIAG_MIN_GAP_S,
                                                vw_changed) == 1) {
                    const char *vw_src = vw_changed ? "edge" : "tick";

                    ESP_LOGI(TAG, "listening_diag corr_id=kdp-%u src=%s state=%d vad=%d run=%d quiet=%d asr=%d ticks=%d rec_ctx=%d rec_mat=%d suspect=%d",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu), vw_src,
                             static_cast<int>(GetDeviceState()), vw_vad,
                             s_voice_watch.level_run_s, s_voice_watch.quiet_s,
                             s_voice_watch.asr_age_s, clock_ticks_, vw_rec_ctx, vw_rec_mat,
                             s_voice_watch.suspect);
                    s_voice_watch.diag_last_s = s_voice_watch.tick_s;
                    s_vw_diag_vad = vw_vad;
                    s_vw_diag_rec_ctx = vw_rec_ctx;
                    s_vw_diag_rec_mat = vw_rec_mat;
                    s_vw_diag_suspect = s_voice_watch.suspect;
                }
            }

            /* 261116-channel-resident-core（P1 T2；真机轮 B 修复轮重锚）: 60s 无语音
             * 超时 producer（requirements §2.1 结束三通道①）。判据三件:
             *   ① 设备态 == 聆听态（非聆听态不计时）
             *   ② 记录语境硬豁免（requirements §2.1 v1.65；记录会话由引导/归档链或
             *      下方空素材 producer 收尾）
             *   ③ 缝返回空闲收尾动作（= 有效语音证据累计静默达阈值；电平卡死 / 起音沿
             *      风暴两种轮 B 失效都走得到——判据本体见 session_voice_watch.c）
             * 旧判据的瞬时电平必要条件与裸计时比较已退役（电平卡死时前者恒假、起音沿
             * 风暴下后者反复归零，两者在真机日志里同构）。
             * 命中 -> 直接投统一收口处理器（主循环上下文 = 直调形态; 协议回调上下文
             * 走事件位, 两类不混用）。审计行 token = session_idle_timeout。 */
            if (GetDeviceState() == kDeviceStateListening &&
                !kidpal_story_session_ctx_active() && vw_act == KVW_ACT_IDLE_FIRE) {
                ESP_LOGI(TAG, "session_idle_timeout corr_id=kdp-%u seconds=%d quiet=%d asr=%d suspect=%d src=idle",
                         static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu),
                         clock_ticks_, s_voice_watch.quiet_s, s_voice_watch.asr_age_s,
                         s_voice_watch.suspect);
                HandleEndVoiceSessionEvent();
            }

            /* 261116-channel-resident-core（真机轮 A 修复轮 D3；轮 B 修复轮换源）:
             * 空素材记录会话空闲收尾 producer。判据四件:
             *   ① 设备态 == 聆听态
             *   ② 记录语境会话（正向门——与上方既有 60s 无语音 producer 的豁免门互补，
             *      两者恰好覆盖「非记录语境 / 空素材记录语境」两半）
             *   ③ 本会话**引擎从未转写出一句**（素材门读缝内 asr_seen；旧信号是 VAD 起音
             *      沿置位的录音页镜像，轮 B 设备日志证明无人说话时起步 1 秒内也会让出
             *      一记 speech 沿把它置真 -> 旧判据在真机上结构性不可达）
             *   ④ 缝内静默秒数达到空素材阈值
             * 命中 -> 直调统一收口处理器（主循环上下文 = 直调形态，与上方同款）。
             * 审计行 token 固定（轮 B 三源对账的设备侧唯一可观测证据）。 */
            if (GetDeviceState() == kDeviceStateListening &&
                kidpal_story_session_ctx_active() && !s_voice_watch.asr_seen &&
                s_voice_watch.quiet_s >= kStoryEmptySessionTimeoutS) {
                ESP_LOGI(TAG, "story_empty_session_timeout corr_id=kdp-%u seconds=%d quiet=%d asr=%d suspect=%d src=empty_story",
                         static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu),
                         clock_ticks_, s_voice_watch.quiet_s, s_voice_watch.asr_age_s,
                         s_voice_watch.suspect);
                HandleEndVoiceSessionEvent();
            }

            /* 261116-channel-resident-core（P1 T4）: 通道自愈巡检（§6.4 常驻保活）。
             * 会话间隙本就零下行, 设备侧「120 秒无下行 = 死链」判据必然到期
             * （TECH-261012 首按键失同步第一段根因）——本巡检在 Idle 且通道未开时
             * 复用既有自动建连任务补建。
             * **守卫短路顺序是承重件**: 先判自动建连 in-flight 标志、再判通道开合——
             * 顺序写反会让 in-flight 期（3 次退避 ≈35s）仍每秒走到通道判据，其内
             * IsTimeout 的 ESP_LOGE 回到每秒一条；正确顺序 = 事实降频（因此本包零改
             * protocol.cc）。注释刻意不写判据符号字面（源文本计数断言会因此稀释）。
             * 隔离: 只经既有自动建连任务（判 + 起在主循环 handler 内）；本区零直连
             * 建连调用、不新建 task = 零第二条重连路径。 */
            if (GetDeviceState() == kDeviceStateIdle && protocol_ &&
                !auto_connect_in_flight_ &&
                !protocol_->IsAudioChannelOpened()) {
                TryStartAutoConnect();
            }
        }
    }
}

void Application::HandleNetworkConnectedEvent() {
    ESP_LOGI(TAG, "Network connected");
    /* 261117-session-end-intent-longretry（P2）: 网络恢复（建连成功路径）清网络断退出
     * 请求位——否则长跑自动建连的预检会永久早退（断网期置的位不复位即后续再也不建连）。 */
    network_exit_requested_ = false;
    /* K5 [S2]: SNTP 校时挂在 WiFi got IP（本事件区）之后异步启动——不阻塞激活/
     * 主流程（esp_sntp_init 自身异步 + 监测在独立 task）；写回 PCF85063 由板级
     * sync 回调承接。幂等, 重连不重启。 */
    KidPalStartSntpTimeSync();
    auto state = GetDeviceState();

    if (state == kDeviceStateStarting || state == kDeviceStateWifiConfiguring) {
        // Network is ready, start activation
        SetDeviceState(kDeviceStateActivating);
        if (activation_task_handle_ != nullptr) {
            ESP_LOGW(TAG, "Activation task already running");
            return;
        }

        xTaskCreate([](void* arg) {
            Application* app = static_cast<Application*>(arg);
            app->ActivationTask();
            app->activation_task_handle_ = nullptr;
            vTaskDelete(NULL);
        }, "activation", 4096 * 2, this, 2, &activation_task_handle_);
    } else if (state == kDeviceStateIdle) {
        // K6 (M3-K6 boot-auto-connect): 网络重连后 Idle 且通道未开 → 自动建连恢复
        // （触发点 2——断网重连自动恢复; main-loop handler 内判 + 起）。
        // P2: 调自动建连之前再确认退出请求位已清（幂等防竞态）。
        network_exit_requested_ = false;
        TryStartAutoConnect();
    }

    // Update the status bar immediately to show the network state
    auto display = Board::GetInstance().GetDisplay();
    display->UpdateStatusBar(true);
}

void Application::HandleNetworkDisconnectedEvent() {
    /* 261117-session-end-intent-longretry（P2）: 网络断退出请求位——置位在函数体最顶端、
     * 三态门之外：任何态下断网都要令长跑自动建连 task 退出空转重试（WiFi 已断时不得在
     * 已断链上反复握手）。网络恢复建连成功时清位。 */
    network_exit_requested_ = true;
    // Close current conversation when network disconnected
    auto state = GetDeviceState();
    if (state == kDeviceStateConnecting || state == kDeviceStateListening || state == kDeviceStateSpeaking) {
        ESP_LOGI(TAG, "Closing audio channel due to network disconnection");
        protocol_->CloseAudioChannel();
        /* 261010 [S9] 网络断关收尾补偿: wifi 骤断 TCP 未察觉时 OnDisconnected 回调
         * 亦被抑制（[S7] 本地关标记 + TCP 静默）→ 设备态停留非 Idle，网络恢复后
         * HandleNetworkConnectedEvent 仅 Idle 支路接自动建连 = K6 既有重连语义被静默
         * 破坏。此处主循环 handler 内同步补回 Idle + LOW_POWER 剖面。网络断关仍零
         * 重建排程（恢复走既有 K6 触发点 2 网络重连事件, RC-B3 负锚）。 */
        SetDeviceState(kDeviceStateIdle);
        Board::GetInstance().SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
        ESP_LOGI(TAG, "net_disconnect_settle corr_id=kdp-%u state=idle",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
    }

    // Update the status bar immediately to show the network state
    auto display = Board::GetInstance().GetDisplay();
    display->UpdateStatusBar(true);
}

void Application::HandleActivationDoneEvent() {
    ESP_LOGI(TAG, "Activation done");

    SystemInfo::PrintHeapStats();
    SetDeviceState(kDeviceStateIdle);

    has_server_time_ = ota_->HasServerTime();

    auto display = Board::GetInstance().GetDisplay();
    std::string message = std::string(Lang::Strings::VERSION) + ota_->GetCurrentVersion();
    display->ShowNotification(message.c_str());
    display->SetChatMessage("system", "");

    // Play the success sound to indicate the device is ready
    audio_service_.PlaySound(Lang::Sounds::OGG_SUCCESS);

    // Release OTA object after activation is complete
    ota_.reset();
    auto& board = Board::GetInstance();
    board.SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);

    // K6 (M3-K6 boot-auto-connect): 激活完成后自动建连（触发点 1——Idle 且通道未开
    // 时在 main-loop handler 内判 + 起, 后台 task 建连, 不阻塞）。状态已入
    // kDeviceStateIdle（见本函数上方 SetDeviceState(kDeviceStateIdle)）。
    TryStartAutoConnect();
}

void Application::ActivationTask() {
    // Create OTA object for activation process
    ota_ = std::make_unique<Ota>();

    // 短路激活 (spec §6.3, device_ws_switch WSA-A2): 设备服务器地址入口 = 配网直写
    // NVS namespace "websocket" (url/token/version), 不走 OTA 应答端点、不拉
    // 版本、不轮询激活码 —— 自建 OTA 应答端点留二期。token 缺失时由配网直写入口生成
    // 写入 (决策22: 配网时 CSPRNG 生成 >=128-bit 随机)。
    WsProvision::EnsureWebsocketToken();

    // Check for new assets version
    CheckAssetsVersion();

    // Initialize the protocol (方向 B: 短路激活后直选 WS, 见 InitializeProtocol)
    InitializeProtocol();

    // Signal completion to main loop
    xEventGroupSetBits(event_group_, MAIN_EVENT_ACTIVATION_DONE);
}

void Application::TryStartAutoConnect() {
    /* [K6-A3] 预检只读（与 websocket_protocol.cc read 形态逐字同款）: namespace
     * "websocket" 只读 url/token, 任一空 → 单条 log + 完全 no-op（不起 task、不重试、
     * 不报错; Settings 只读不写——写入口 = WsProvision::EnsureWebsocketToken, activation
     * 期已跑）。 */
    Settings settings("websocket", false);
    std::string url = settings.GetString("url");
    std::string token = settings.GetString("token");
    if (url.empty() || token.empty()) {
        ESP_LOGI(TAG, "AutoConnect: no websocket url/token configured, skip");
        return;
    }

    /* 261117-session-end-intent-longretry（P2）: 网络断退出请求位预检早退——网已断
     * （断网事件置位、重连成功清位）时不起 task、不空转重试（守卫由三条件扩四条件,
     * 显式申报；本预检必须位于置 in-flight 之前）。 */
    if (network_exit_requested_) {
        ESP_LOGI(TAG, "AutoConnect: network down (exit requested), skip");
        return;
    }

    /* [K6-A1] 守卫三条件（main-loop handler 内「判 + 起」, TOCTOU 判设闭合）:
     * state==Idle && 通道未开 && 非 in-flight。无协议对象时退化 no-op。 */
    if (GetDeviceState() != kDeviceStateIdle ||
        (protocol_ && protocol_->IsAudioChannelOpened()) ||
        auto_connect_in_flight_) {
        return;
    }
    auto_connect_in_flight_ = true;

    /* 后台 task（沿用 activation 模板: 栈 4096*2、优先级 2、lambda 直排——不阻塞主
     * 循环; [S3] 封顶递增长跑、每次尝试前重查守卫三条件、所有退出路径必清标志、退出前
     * 打栈高水位日志）。长跑三退出条件: 成功建连 / bail（守卫被人工路径接管或通道已开）
     * / 网络断退出请求位（WiFi 已断则不在已断链上空转）。 */
    xTaskCreate([](void* arg) {
        Application* app = static_cast<Application*>(arg);
        const int backoff_count =
            static_cast<int>(sizeof(kAutoConnectBackoffS) / sizeof(kAutoConnectBackoffS[0]));
        bool connected = false;
        int step = 0;   /* 退避档位：先走递增表, 耗尽后转稳态周期 */
        while (true) {
            /* 重试前重查守卫三条件（违反即 bail——人工路径已接管或通道已开）。 */
            if (app->GetDeviceState() != kDeviceStateIdle ||
                (app->protocol_ && app->protocol_->IsAudioChannelOpened())) {
                break;
            }
            /* 网络断退出请求位：网已断则退出（不空转重试）。 */
            if (app->network_exit_requested_) {
                break;
            }
            if (app->protocol_->OpenAudioChannel()) {
                connected = true;
                break;  /* 成功: 通道开 + Idle = 首轮对话后稳态, 无状态迁移 */
            }
            /* 退避：封顶递增走表, 档位耗尽后恒稳态周期（无上限）。按 1 秒粒度分片睡眠,
             * 每片重查人工意向位（→ 截短退避）与网络断退出请求位（→ ≤1s 退出）。 */
            int delay_s = (step < backoff_count) ? kAutoConnectBackoffS[step]
                                                 : kAutoConnectSteadyRetryS;
            for (int s = 0; s < delay_s; ++s) {
                vTaskDelay(pdMS_TO_TICKS(1000));
                if (app->network_exit_requested_) {
                    break;
                }
                if (app->pending_manual_intent_ != 0) {
                    break;  /* 人工意向命中：截短退避、立即重试 */
                }
            }
            ++step;
        }
        /* 所有退出路径（成功 / bail / 网络断）必清单点——先清 in-flight, 成功路径再经
         * Schedule 补投人工意向位（先清后投, 否则补投被自身 in-flight 守卫吞掉）。只在
         * 成功路径补投；bail / 断网退出清而不投（意向位保留, 下次成功建连仍可接续）。 */
        app->auto_connect_in_flight_ = false;
        if (connected && app->pending_manual_intent_ != 0) {
            EventBits_t intent_bits = app->pending_manual_intent_;
            app->pending_manual_intent_ = 0;
            app->Schedule([app, intent_bits]() {
                xEventGroupSetBits(app->event_group_, intent_bits);
            });
        }
        ESP_LOGI(TAG, "AutoConnect task exit, stack high water mark=%u",
                 (unsigned)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }, "AutoConnect", 4096 * 2, this, 2, nullptr);
}

void Application::CheckAssetsVersion() {
    // Only allow CheckAssetsVersion to be called once
    if (assets_version_checked_) {
        return;
    }
    assets_version_checked_ = true;

    auto& board = Board::GetInstance();
    auto display = board.GetDisplay();
    auto& assets = Assets::GetInstance();

    if (!assets.partition_valid()) {
        ESP_LOGW(TAG, "Assets partition is disabled for board %s", BOARD_NAME);
        return;
    }
    
    Settings settings("assets", true);
    // Check if there is a new assets need to be downloaded
    std::string download_url = settings.GetString("download_url");

    if (!download_url.empty()) {
        settings.EraseKey("download_url");

        char message[256];
        snprintf(message, sizeof(message), Lang::Strings::FOUND_NEW_ASSETS, download_url.c_str());
        Alert(Lang::Strings::LOADING_ASSETS, message, "cloud_arrow_down", Lang::Sounds::OGG_UPGRADE);
        
        // Wait for the audio service to be idle for 3 seconds
        vTaskDelay(pdMS_TO_TICKS(3000));
        SetDeviceState(kDeviceStateUpgrading);
        board.SetPowerSaveLevel(PowerSaveLevel::PERFORMANCE);
        display->SetChatMessage("system", Lang::Strings::PLEASE_WAIT);

        bool success = assets.Download(download_url, [display](int progress, size_t speed) -> void {
            std::thread([display, progress, speed]() {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%d%% %uKB/s", progress, speed / 1024);
                display->SetChatMessage("system", buffer);
            }).detach();
        });

        board.SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (!success) {
            Alert(Lang::Strings::ERROR, Lang::Strings::DOWNLOAD_ASSETS_FAILED, "circle_xmark", Lang::Sounds::OGG_EXCLAMATION);
            vTaskDelay(pdMS_TO_TICKS(2000));
            SetDeviceState(kDeviceStateActivating);
            return;
        }
    }

    // Apply assets
    assets.Apply();
    display->SetChatMessage("system", "");
    display->SetEmotion("microchip_ai");
}

void Application::InitializeProtocol() {
    auto& board = Board::GetInstance();
    auto display = board.GetDisplay();
    auto codec = board.GetAudioCodec();

    display->SetStatus(Lang::Strings::LOADING_PROTOCOL);

    // 方向 B (spec §6.3, device_ws_switch WSA-A1/A2): 短路激活后直选 WebSocket (小智
    // 原生 WS+Opus 管线)。服务器地址入口 = 配网直写 NVS namespace "websocket"
    // (url/token/version), 不再以 OTA 应答配置决定协议。MQTT 分支
    // 保留可编译但非冒烟默认路径。
    protocol_ = std::make_unique<WebsocketProtocol>();

    // 自签 CA pinning (V10): 校验 cacert 加载面已接通; 真实握手证书校验由
    // websocket_protocol.cc 建连路径承载 (协议层, 见 ws_tls_ca)。
    ws_tls_ca::VerifyCacertConfigured();

    protocol_->OnConnected([this]() {
        DismissAlert();
    });

    protocol_->OnNetworkError([this](const std::string& message) {
        // K6 (M3-K6 boot-auto-connect) 静默失败锚: 自动建连 in-flight 期间的网络错误
        // 拦截豁免——静默（不弹错误卡、保持 Idle; 重试节奏由 AutoConnect task 承担）。
        // 人工路径错误路由零改动（错误事件位仍在世, 非拦截面; 拦截判定位次先于错误
        // 事件置位）。
        if (auto_connect_in_flight_) {
            ESP_LOGI(TAG, "AutoConnect: suppressing network error while in-flight");
            return;
        }
        last_error_message_ = message;
        xEventGroupSetBits(event_group_, MAIN_EVENT_ERROR);
    });
    
    protocol_->OnIncomingAudio([this](std::unique_ptr<AudioStreamPacket> packet) {
        if (GetDeviceState() == kDeviceStateSpeaking) {
            /* 音频感知标志置位（真机轮追补 261022 r2）: 真实包到达**且**会进解码队列
             * （= 真会被听到）才算「本窗口有音频」——置位点在状态门内、解码入队前,
             * 音频路径本身零行为变更（只多一次布尔写）。孤儿 tts start 后引擎 abort
             * 已清队列, 该窗口永不走本支 -> 看门狗保 false -> 3s 到期复位。 */
            g_story_guide_audio_seen = true;
            audio_service_.PushPacketToDecodeQueue(std::move(packet));
        }
    });
    
    protocol_->OnAudioChannelOpened([this, codec, &board]() {
        // K6 (M3-K6 boot-auto-connect): 回调统一回主循环（Schedule 包裹）——自动建连
        // 路径下该回调在后台 AutoConnect task 线程执行, 人工路径本就在主循环（包裹无害）。
        Schedule([this, codec, &board]() {
            board.SetPowerSaveLevel(PowerSaveLevel::PERFORMANCE);
            if (protocol_->server_sample_rate() != codec->output_sample_rate()) {
                ESP_LOGW(TAG, "Server sample rate %d does not match device output sample rate %d, resampling may cause distortion",
                    protocol_->server_sample_rate(), codec->output_sample_rate());
            }
        });
    });
    
    protocol_->OnAudioChannelClosed([this, &board]() {
        board.SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
        Schedule([this]() {
            auto display = Board::GetInstance().GetDisplay();
            display->SetChatMessage("system", "");
            SetDeviceState(kDeviceStateIdle);
            /* [S8] A 路径: 非本地关（服务端/网络侧断）→ 排入自动重连（复用既有
             * AutoConnect task 守卫与退避; 零第二条 task; 重连区零 SetDeviceState——
             * 屏显维持 Idle）。本地关不达本 lambda（协议层分类, [S7]）——分类互斥的
             * application 半。 */
            TryStartAutoConnect();
        });
    });
    
    protocol_->OnIncomingJson([this, display](const cJSON* root) {
        // Parse JSON data
        auto type = cJSON_GetObjectItem(root, "type");
        if (strcmp(type->valuestring, "tts") == 0) {
            auto state = cJSON_GetObjectItem(root, "state");
            if (strcmp(state->valuestring, "start") == 0) {
                /* 语音轮演出 (device-round VQ-A7 r1 修复面①): tts start 帧不再触发语音
                 * 轮场景切换 —— 方向 B 网关在 stt 之后立即发 tts start (先于 LLM 出结果),
                 * 旧形态「进 Speaking 设备态」经状态级联 SetStatus(SPEAKING) ->
                 * apply_look(TALK) 把 THINKING 页 10ms 顶掉。「开始播报」语义迁往首个
                 * sentence_start 分支 (实际起播时刻), 此处只复位打断标志。 */
                Schedule([this]() {
                    aborted_ = false;
                    /* [G3] 帧 B 子态置位沿 (261020 D4-5): 记录语境内 tts start =
                     * 引导播报开始（引擎抑制下 kidpal_speak 是唯一 TTS 源; 桥内
                     * RECORDING 页态门 + 幂等）——键帽路由据此分叉帧 B（打断）。 */
                    KidpalStoryGuideSpeechEdge(1);
                });
            } else if (strcmp(state->valuestring, "stop") == 0) {
                Schedule([this]() {
                    /* [G3] 帧 B 子态复位沿 (261020 D4-5): 引导播报结束/被打断 ——
                     * 无条件先于 Speaking 出口守卫（tts stop 到达时设备可能已因
                     * 打断离场 Speaking, 复位沿不得被出口判据吞掉; 桥内幂等 +
                     * RECORDING 页态门）。 */
                    KidpalStoryGuideSpeechEdge(0);
                    if (GetDeviceState() == kDeviceStateSpeaking) {
                        /* 261014 VR-A2 tts stop 出口三守卫（序即优先级）:
                         * ① 手动模式既有语义（出口回 Idle, 优先级最高）;
                         * ② 查字收尾（查字窗在前台时 tts stop -> 结束会话回 Idle, 查字窗
                         *    驻留到按键/5 分钟兜底语义零改 —— requirements v1.37 :652
                         *    「播完会话即结束、不回聆听态」）;
                         * ③ 出会话 speak 不回聆听（fork② 设备侧: 轮来源非聆听态的旁路注入
                         *    〔提醒/查字播报〕回 Idle, 不发 listen start、不开麦 ——
                         *    TECH-261014-post-speak-listening-lingering 本体）;
                         * else 正向路径保留: 对话轮（轮来源 == 聆听态）播完回聆听 =
                         *    连续追问保护（requirements §2.1 :39）。 */
                        if (listening_mode_ == kListeningModeManualStop) {
                            SetDeviceState(kDeviceStateIdle);
                        } else if (kidpal_render_node_front_layer() == KIDPAL_LAYER_CHAR_WINDOW) {
                            protocol_->SendStopListening();
                            SetDeviceState(kDeviceStateIdle);
                        } else if (speaking_round_entered_from_ != kDeviceStateListening) {
                            SetDeviceState(kDeviceStateIdle);
                        } else {
                            SetDeviceState(kDeviceStateListening);
                        }
                    }
                });
            } else if (strcmp(state->valuestring, "sentence_start") == 0) {
                auto text = cJSON_GetObjectItem(root, "text");
                if (cJSON_IsString(text)) {
                    ESP_LOGI(TAG, "<< %s", text->valuestring);
                    Schedule([this, display, message = std::string(text->valuestring)]() {
                        /* 语音轮演出 (device-round VQ-A7 r1 修复面①): 「开始播报」绑定
                         * 实际起播时刻 —— 首个 sentence_start 才进 Speaking 态 (设备态
                         * 级联 SetStatus(SPEAKING) -> apply_look(TALK)); 网关时序保证本句
                         * 音频包恒后于 sentence_start 到达 (sendAudioHandle.py: 先发
                         * sentence_start 再 sendAudio), 状态先行不丢首包。喂入经冻结映射
                         * 单源 kidpal_voice_round_scene (映射表零改动, 只动触发点), 消费
                         * 链形态与 stt 分支同款。 */
                        /* 261014 VR-A2 轮来源采样（fork② 判别器依据）: 进 Speaking 前取真实
                         * 设备态, 每轮 sentence_start 重置（single write point, 无 stale
                         * 轮来源残留）——tts stop 出口据其判「出会话 speak 不回聆听」。 */
                        /* 261014-r2 A1 主因修正: sentence_start 是逐句消息, 无条件写点会使第
                         * 2 句起状态已 Speaking 覆写采样 -> 收尾判据③ 误伤多句对话轮回 Idle;
                         * 加状态守卫后「逐轮一次」由「状态离场 Speaking 才重采」自然保证
                         * （轮间 tts stop 已把状态推出 Speaking, 下一轮首句必重采; abort 后
                         * 同样自然重采）——与原注释意图一致。 */
                        if (GetDeviceState() != kDeviceStateSpeaking) {
                            speaking_round_entered_from_ = GetDeviceState();
                        }
                        SetDeviceState(kDeviceStateSpeaking);
                        kvr_ctx_t ctx = (listening_mode_ == kListeningModeManualStop)
                                            ? KVR_CTX_LISTEN_MANUAL : KVR_CTX_LISTEN_AUTO;
                        int scene = kidpal_voice_round_scene(ctx, KVR_EVT_TTS_START);
                        if (scene == KVST_SCENE_TALK) {
                            display->SetStatus(Lang::Strings::SPEAKING);
                        }
                        display->SetChatMessage("assistant", message.c_str());
                    });
                }
            }
        } else if (strcmp(type->valuestring, "stt") == 0) {
            auto text = cJSON_GetObjectItem(root, "text");
            if (cJSON_IsString(text)) {
                ESP_LOGI(TAG, ">> %s", text->valuestring);
                Schedule([this, display, message = std::string(text->valuestring)]() {
                    /* 261116-channel-resident-core（真机轮 B 修复轮）: 引擎真的转写出
                     * 一句话 = 「本会话有素材」的证据（与 front 累积器同源——front 的
                     * accum 也只吃 stt 文本）。放 lambda 内 = 主循环上下文，与 tick /
                     * reset 同写者，零锁。 */
                    kidpal_voice_watch_note_asr(&s_voice_watch);
                    /* 语音轮演出 (包 260915-m1-k3-voice-round-ui, seam [2]): 思考触发 =
                     * 服务端 VAD 切句完成 (stt 下发) 同刻消费场景单源 —— 状态切换与页面
                     * 弹出同一时刻 (requirements §2.2 思考中行), 非 tts start、非定时器。
                     * 回显 (SetChatMessage("user") 既有通道) 与思考切换同一回调。manual
                     * 记录语境 (LISTEN_MANUAL) 切句只回显不逐句进思考。 */
                    display->SetChatMessage("user", message.c_str());
                    /* K3 [S6]: 记录语境旁路喂入（与 front _story_feed_from_stt 旁路
                     * 同构, 不改 wire）——录音态话语回显区逐句入列（kidpal_story_echo_*,
                     * 零共享 kidpal_voice_echo 态）; 非记录语境零消费。 */
                    if (kidpal_story_session_ctx_active()) {
                        (void)kidpal_story_echo_push(message.c_str());
                    }
                    /* [G6] kvr 场景守卫（261019-p3-record-dialogue-guide）: 记录语境
                     * **先分流**（引用先于 kidpal_voice_round_scene——ctx 选取必须在
                     * 映射前联挂记录语境标志）: 记录语境切逐句 stt 形态后 ctx 选取不得
                     * 按 listening_mode_ 误映 KVST_SCENE_THINK -> SetStatus(THINKING)
                     * 跳页（AC1「切句只回显不进思考」; 回显已在上支承载）。非记录语境
                     * 既有场景映射零偏移。 */
                    if (!kidpal_story_session_ctx_active()) {
                        kvr_ctx_t ctx = (listening_mode_ == kListeningModeManualStop)
                                            ? KVR_CTX_LISTEN_MANUAL : KVR_CTX_LISTEN_AUTO;
                        int scene = kidpal_voice_round_scene(ctx, KVR_EVT_STT_FINAL);
                        if (scene == KVST_SCENE_THINK) {
                            display->SetStatus(Lang::Strings::THINKING);
                        }
                    }
                });
            }
        } else if (strcmp(type->valuestring, "llm") == 0) {
            auto emotion = cJSON_GetObjectItem(root, "emotion");
            if (cJSON_IsString(emotion)) {
                Schedule([this, display, emotion_str = std::string(emotion->valuestring)]() {
                    display->SetEmotion(emotion_str.c_str());
                });
            }
        } else if (strcmp(type->valuestring, "mcp") == 0) {
            auto payload = cJSON_GetObjectItem(root, "payload");
            if (cJSON_IsObject(payload)) {
                McpServer::GetInstance().ParseMessage(payload);
            }
        } else if (strcmp(type->valuestring, "system") == 0) {
            auto command = cJSON_GetObjectItem(root, "command");
            if (cJSON_IsString(command)) {
                ESP_LOGI(TAG, "System command: %s", command->valuestring);
                if (strcmp(command->valuestring, "reboot") == 0) {
                    // Do a reboot if user requests a OTA update
                    Schedule([this]() {
                        Reboot();
                    });
                } else {
                    ESP_LOGW(TAG, "Unknown system command: %s", command->valuestring);
                }
            }
        } else if (strcmp(type->valuestring, "alert") == 0) {
            auto status = cJSON_GetObjectItem(root, "status");
            auto message = cJSON_GetObjectItem(root, "message");
            auto emotion = cJSON_GetObjectItem(root, "emotion");
            if (cJSON_IsString(status) && cJSON_IsString(message) && cJSON_IsString(emotion)) {
                Alert(status->valuestring, message->valuestring, emotion->valuestring, Lang::Sounds::OGG_VIBRATION);
            } else {
                ESP_LOGW(TAG, "Alert command requires status, message and emotion");
            }
#if CONFIG_RECEIVE_CUSTOM_MESSAGE
        } else if (strcmp(type->valuestring, "custom") == 0) {
            auto payload = cJSON_GetObjectItem(root, "payload");
            ESP_LOGI(TAG, "Received custom message: %s", cJSON_PrintUnformatted(root));
            if (cJSON_IsObject(payload)) {
                Schedule([this, display, payload_str = std::string(cJSON_PrintUnformatted(payload))]() {
                    display->SetChatMessage("system", payload_str.c_str());
                });
            } else {
                ESP_LOGW(TAG, "Invalid custom message format: missing payload");
            }
#endif
        } else if (strcmp(type->valuestring, "kidpal_keepalive") == 0) {
            /* 契约 §四 心跳帧（261116-channel-resident-core P1 T3）: front 每连接周期
             * 下行的裸心跳 JSON（非 display 帧）。no-op 直接返回——不得落 Unknown
             * message type WARN（每 30s 一条会按 ~144KB/日 冲 SD 日志轮转预算）。
             * 位次承重: 在 #endif（上一条件编译块尾，本分支不依赖该配置）之后、
             * WARN 之前。注释不重抄帧 type 字面（验收件按源文本计数该字面，注释里的
             * 重复会让「分支在位」断言失真）。 */
            return;
        } else {
            ESP_LOGW(TAG, "Unknown message type: %s", type->valuestring);
        }
    });
    
    protocol_->Start();
}

void Application::Alert(const char* status, const char* message, const char* emotion, const std::string_view& sound) {
    ESP_LOGW(TAG, "Alert [%s] %s: %s", emotion, status, message);
    auto display = Board::GetInstance().GetDisplay();
    display->SetStatus(status);
    display->SetEmotion(emotion);
    display->SetChatMessage("system", message);
    if (!sound.empty()) {
        audio_service_.PlaySound(sound);
    }
}

void Application::DismissAlert() {
    if (GetDeviceState() == kDeviceStateIdle) {
        auto display = Board::GetInstance().GetDisplay();
        display->SetStatus(Lang::Strings::STANDBY);
        display->SetEmotion("neutral");
        display->SetChatMessage("system", "");
    }
}

void Application::ToggleChatState() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_TOGGLE_CHAT);
}

/* 261024-vq-overlay-close-end-session: 覆盖层关层手势的统一会话结束入口（事件位,
 * 线程安全——板级按键回调不在主任务上下文; 实际收尾在
 * HandleEndVoiceSessionEvent 内单源执行）。与 ToggleChatState 是两个语义入口: 后者
 * 播报态 = 打断 -> 聆听（需求 §2.3 冻结）, 本入口 = 三态一律结束会话回 Idle。 */
void Application::EndVoiceSession() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_END_VOICE_SESSION);
}

void Application::StartListening() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_START_LISTENING);
}

void Application::StopListening() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_STOP_LISTENING);
}

void Application::CloseAudioChannel() {
    /* 261116-channel-resident-core（P1 T1）: 本函数**名字为历史名、语义 = 会话收尾**
     * （K3 [S5] 录音态丢弃面 / BOOT 任一手势）。通道常驻后会话结束不再拆通道：改投
     * 统一会话结束处理器的事件位（MAIN_EVENT_END_VOICE_SESSION）。
     *
     * 形态二分（不得混用）: 本 helper 的调用上下文 = **协议接收任务**
     * （OnIncomingJson -> McpServer::ParseMessage -> mcp 回调），跨任务禁止直调
     * HandleEndVoiceSessionEvent()（后者碰 protocol_ / 状态机 / audio_service_），
     * 故只投事件位，收尾在主循环下一拍单源执行（listen stop + Idle）。
     *
     * 丢弃面契约 §四 顺序约束（2026-11-16 重锚）: SendKidpalStoryDiscard() 由调用方
     * 在本调用**之前**已在同一 TCP 上行；本 helper 投的事件位在主循环下一拍才发
     * listen stop —— 帧先行、stop 后至，front 先按 discard 取消会话（routing 回 chat）
     * 再消费 stop = 丢弃素材不会复活成归档。mcp_server.cc 因此零改动（守 P2 边界）。 */
    ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=discard_record state=session_end_requested",
             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
    EndVoiceSession();
}

void Application::HandleToggleChatEvent() {
    auto state = GetDeviceState();
    
    if (state == kDeviceStateActivating) {
        SetDeviceState(kDeviceStateIdle);
        return;
    } else if (state == kDeviceStateWifiConfiguring) {
        audio_service_.EnableAudioTesting(true);
        SetDeviceState(kDeviceStateAudioTesting);
        return;
    } else if (state == kDeviceStateAudioTesting) {
        audio_service_.EnableAudioTesting(false);
        SetDeviceState(kDeviceStateWifiConfiguring);
        return;
    }

    if (!protocol_) {
        ESP_LOGE(TAG, "Protocol not initialized");
        return;
    }

    // K6 (M3-K6): 自动建连 in-flight 期间人工按键 no-op（守卫不加锁, 主循环 handler 内
    // 判设闭合; toggling 既有分支行为语义零改动）。
    if (auto_connect_in_flight_) {
        /* 261117 P2: 离线期人工意向位——长跑 in-flight 时不再静默 no-op：置意向位 +
         * 审计行, task 据此截短退避、连上后经 Schedule 补投（孩子那一次动作不丢）。 */
        pending_manual_intent_ = MAIN_EVENT_TOGGLE_CHAT;
        ESP_LOGI(TAG, "manual_intent_queued corr_id=kdp-%u src=queued_toggle",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual toggle");
        return;
    }

    if (state == kDeviceStateIdle) {
        if (!protocol_->IsAudioChannelOpened()) {
            SetDeviceState(kDeviceStateConnecting);
            if (!protocol_->OpenAudioChannel()) {
                return;
            }
        }

        SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop : kListeningModeRealtime);
    } else if (state == kDeviceStateSpeaking) {
        AbortSpeaking(kAbortReasonNone);
    } else if (state == kDeviceStateListening) {
        /* 261116-channel-resident-core（P1 T1）: KEY 单击结束会话——主循环上下文，
         * 直调统一收口（保同步语义; 协议回调上下文才走事件位，两类不混用）。
         * 不再拆通道 / 不降省电档 / 不排常驻重建。本路审计 token 留在调用点的日志行
         * 上（四路诊断口径区分；注释不重抄 token 字面）。 */
        ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=toggle_chat state=session_end",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        HandleEndVoiceSessionEvent();
    }
}

void Application::HandleStartListeningEvent() {
    auto state = GetDeviceState();
    
    if (state == kDeviceStateActivating) {
        SetDeviceState(kDeviceStateIdle);
        return;
    } else if (state == kDeviceStateWifiConfiguring) {
        audio_service_.EnableAudioTesting(true);
        SetDeviceState(kDeviceStateAudioTesting);
        /* RSL2-A3 [I2]（261028-p3-record-session-mirror-desync-fix, M3）: 聆听没真起来 ->
         * 清记录语境标志（否则置位后本分支 return, ctx 永久停 1）。 */
        kidpal_story_session_ctx_set(0);
        return;
    }

    if (!protocol_) {
        ESP_LOGE(TAG, "Protocol not initialized");
        /* RSL2-A3 [I2]（M3）: !protocol_ 早退 -> 清记录语境标志（被拒 REC_SESSION 不留 ctx）。 */
        kidpal_story_session_ctx_set(0);
        return;
    }

    // K6 (M3-K6): 自动建连 in-flight 期间人工 start-listening no-op（守卫; 既有分支
    // 行为语义零改动）。
    if (auto_connect_in_flight_) {
        ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual start listening");
        /* RSL2-A3 [I2]（M3）: auto-connect 期被拒 -> 清记录语境标志（KPLOG 2026-10-27
         * 21:34:45 卡死直接根因: 置位后本分支 return, ctx 永久停 1, 长按 KEY 回不了首页）。 */
        kidpal_story_session_ctx_set(0);
        /* 261117 P2: 离线期人工意向位——插入槽须在 ctx 清位之后、return 之前
         * （RSL2-A3 序钉）：置意向位 + 审计行, 连上后经 Schedule 补投该次聆听入口。 */
        pending_manual_intent_ = MAIN_EVENT_START_LISTENING;
        ESP_LOGI(TAG, "manual_intent_queued corr_id=kdp-%u src=queued_listen",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        return;
    }

    if (state == kDeviceStateIdle) {
        if (!protocol_->IsAudioChannelOpened()) {
            SetDeviceState(kDeviceStateConnecting);
            if (!protocol_->OpenAudioChannel()) {
                return;
            }
        }

        /* [G3 聆听形态切换]（261019-p3-record-dialogue-guide）: 记录语境从 ManualStop
         * 切到产逐句 stt 形态——切句只回显 + 云侧累积 + 卡壳计时引导（manual 整段转写
         * 形态无逐句 stt, 引导/回显/累积全断）。形态取舍（包内裁）: AEC 开 ->
         * Realtime（QA KEY 路径 :926 同族先例）; AEC 关 -> AutoStop（realtime 需 AEC
         * 挡引导播报回灌; AFE/AEC 依赖归真机轮 GAP-4 承载）。 */
        if (kidpal_story_session_ctx_active()) {
            SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop
                                                  : kListeningModeRealtime);
        } else {
            SetListeningMode(kListeningModeManualStop);
        }
    } else if (state == kDeviceStateSpeaking) {
        AbortSpeaking(kAbortReasonNone);
        /* [G3] 打断衔接（引导播报中 KEY/长按打断 -> StartListening 沿）: 记录语境
         * 保持逐句 stt 形态继续聆听（不回落 manual——回落即离逐句 stt, 回显/累积断）。 */
        if (kidpal_story_session_ctx_active()) {
            SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop
                                                  : kListeningModeRealtime);
        } else {
            SetListeningMode(kListeningModeManualStop);
        }
    }
}

void Application::HandleStopListeningEvent() {
    auto state = GetDeviceState();

    if (state == kDeviceStateAudioTesting) {
        audio_service_.EnableAudioTesting(false);
        SetDeviceState(kDeviceStateWifiConfiguring);
        return;
    } else if (state == kDeviceStateListening) {
        if (protocol_) {
            protocol_->SendStopListening();
        }
        SetDeviceState(kDeviceStateIdle);
    }
}

/* 261024-vq-overlay-close-end-session（TECH-261024 真机事故修复）: 覆盖层关层手势的
 * 统一会话结束处理器——三态（Connecting / Listening / Speaking）一律真正断会话,
 * 不是只打断停播。
 *
 * 261116-channel-resident-core（P1 T1）重锚: 会话生命周期与 WS 通道生命周期解耦
 * （§6.4 / ADR-019）——**会话结束不再拆通道**，本处理器成为全部会话结束触发源的
 * 唯一收口点（四条本地关 + 主循环 60s 空闲超时 producer；云端 end_session 归 P2）。
 * 动作序列（主任务上下文同步序）:
 *   ① protocol_ 判空 / ② auto_connect_in_flight_ 守卫（避免与后台 AutoConnect 抢
 *   websocket_）→ ③ 三态门 → ④ 仅 Speaking: ResetDecoder 即时静音（Idle 分支只停
 *   音频处理器、不清解码/播放队列, 旧队列会在 Idle 后继续出声, 必须显式补）→
 *   ⑤ 仅 Speaking: AbortSpeaking（abort 帧与随后 listen stop 同 TCP 有序, 沿用
 *   SendKidpalStoryDiscard 先例）→ ⑥ **send listen stop**（会话边界带内信号: 通知
 *   front 收尾——录音会话归档 / 记录语境释放；通道保持常驻）→ ⑦ 不等任何 ack
 *   （单向消息、协议无 ack 语义）→ ⑧ SetDeviceState(Idle) → ⑨ 收尾审计行（四路
 *   诊断口径之一，本处理器 = 覆盖层关层来源）。**不降省电档、不排常驻重建**
 *   （通道仍在世）。注释不重抄审计 token / 判据符号字面（验收件按源文本计数）。
 *
 * 收口形态二分（不得混用）: 协议回调上下文（本文件 CloseAudioChannel 历史名 helper）
 * 走事件位投递（跨任务安全）; 主循环上下文（toggle / wake-invoke / CLOCK_TICK 60s
 * producer）直调本函数（保同步语义）。
 *
 * 时序自证（关层落定先于 voice_end）: 本分支在主循环 MAIN_EVENT_SCHEDULE 之前跑 →
 * 置 Idle 只置 STATE_CHANGED 位 → 同轮 SCHEDULE 先执行 dismiss_front_overlay
 * （VOICE 出栈, 回原页落定）→ 下一轮 STATE_CHANGED 才跑 Idle 分支, voice_end 见
 * VOICE 已不在栈上而零副作用。竞态自愈: 若服务端 tts stop 帧晚于本处理器到达, 其
 * 出口判据 GetDeviceState() == Speaking 已为假 → no-op, 不会把状态推回 Listening。 */
void Application::HandleEndVoiceSessionEvent() {
    if (!protocol_) {
        return;
    }
    if (auto_connect_in_flight_) {
        /* 261117 P2: 离线期人工意向位——结束会话入口在长跑 in-flight 时不静默 no-op：
         * 置意向位 + 审计行, 连上后经 Schedule 补投结束会话事件位（收尾仍由统一处理器
         * 单源执行, 本分支只登记意向）。 */
        pending_manual_intent_ = MAIN_EVENT_END_VOICE_SESSION;
        ESP_LOGI(TAG, "manual_intent_queued corr_id=kdp-%u src=queued_end",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        ESP_LOGI(TAG, "AutoConnect in-flight, ignore end voice session");
        return;
    }

    auto state = GetDeviceState();
    if (state != kDeviceStateConnecting && state != kDeviceStateListening &&
        state != kDeviceStateSpeaking) {
        return;
    }

    if (state == kDeviceStateSpeaking) {
        /* 即时静音: Idle 分支只 EnableVoiceProcessing(false)（停处理器不清队列）,
         * 旧播放队列不清会继续出声——这是「打断→聆听」路径靠 EnableVoiceProcessing
         * (true) 内 ResetDecoder 顺带覆盖、而结束会话没有的隐性坑（plan ⑦-4）。 */
        audio_service_.ResetDecoder();
        AbortSpeaking(kAbortReasonNone);
    }

    /* 261116-channel-resident-core（P1 T1）: 会话结束 = 发 listen stop（会话边界带内
     * 信号, 通知 front 收尾：录音会话归档 / 记录语境释放）, 通道保持常驻
     * （§6.4 / ADR-019）。恰一次; 与 AbortSpeaking 同 TCP 有序。 */
    protocol_->SendStopListening();
    SetDeviceState(kDeviceStateIdle);
    ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=overlay_close state=idle",
             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
}

void Application::HandleWakeWordDetectedEvent() {
    if (!protocol_) {
        return;
    }

    auto state = GetDeviceState();

    if (state == kDeviceStateIdle) {
        // K6 (M3-K6): 自动建连 in-flight 期间唤醒 no-op（守卫置于 EncodeWakeWord 之前
        // ——in-flight 期不启动唤醒音频编码; 既有分支行为语义零改动）。
        if (auto_connect_in_flight_) {
            /* 261117 P2: 离线期人工意向位——唤醒词入口（检测沿）在长跑 in-flight 时不
             * 静默 no-op：置意向位 + 审计行, 连上后经 Schedule 补投唤醒事件位。 */
            pending_manual_intent_ = MAIN_EVENT_WAKE_WORD_DETECTED;
            ESP_LOGI(TAG, "manual_intent_queued corr_id=kdp-%u src=queued_wake",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            ESP_LOGI(TAG, "AutoConnect in-flight, ignore wake word");
            return;
        }

        audio_service_.EncodeWakeWord();

        /* 261014 VR-A1 唤醒词 A″ 入口直推: connecting 态事件经状态合并被跳过致 VOICE 层
         * 不推入——idle 分支在通道判断前直推轮首 UI 两件（与 connecting 分支同款语义、
         * 与 KEY 路径入口直推同构）。置于通道判断前 = 同时覆盖「通道已温直跳 listening」
         * 子路径（无合并也照样漏推的第二活实例）。 */
        {
            auto display = Board::GetInstance().GetDisplay();
            if (kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)) {
                display->SetChatMessage("system", "");
            }
        }
        kidpal_render_node_voice_begin();

        if (!protocol_->IsAudioChannelOpened()) {
            SetDeviceState(kDeviceStateConnecting);
            if (!protocol_->OpenAudioChannel()) {
                audio_service_.EnableWakeWordDetection(true);
                return;
            }
        }

        auto wake_word = audio_service_.GetLastWakeWord();
        ESP_LOGI(TAG, "Wake word detected: %s", wake_word.c_str());
#if CONFIG_SEND_WAKE_WORD_DATA
        // Encode and send the wake word data to the server
        while (auto packet = audio_service_.PopWakeWordPacket()) {
            protocol_->SendAudio(std::move(packet));
        }
        // Set the chat state to wake word detected
        protocol_->SendWakeWordDetected(wake_word);
        SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop : kListeningModeRealtime);
#else
        // Set flag to play popup sound after state changes to listening
        // (PlaySound here would be cleared by ResetDecoder in EnableVoiceProcessing)
        play_popup_on_listening_ = true;
        SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop : kListeningModeRealtime);
#endif
    } else if (state == kDeviceStateSpeaking) {
        AbortSpeaking(kAbortReasonWakeWordDetected);
    } else if (state == kDeviceStateActivating) {
        // Restart the activation check if the wake word is detected during activation
        SetDeviceState(kDeviceStateIdle);
    }
}

void Application::HandleStateChangedEvent() {
    DeviceState new_state = state_machine_.GetState();
    clock_ticks_ = 0;

    auto& board = Board::GetInstance();
    auto display = board.GetDisplay();
    auto led = board.GetLed();
    led->OnStateChanged();
    
    switch (new_state) {
        case kDeviceStateUnknown:
        case kDeviceStateIdle:
            display->SetStatus(Lang::Strings::STANDBY);
            display->SetEmotion("neutral");
            audio_service_.EnableVoiceProcessing(false);
            audio_service_.EnableWakeWordDetection(true);
            /* K3R [R2]（261001-m4-k3-record-session-live-repair, D4b-1）: 会话收尾复位
             * 录音页态 + 停录音演出定时器——**先于**前台回落, 令 voice_end 的 [R3] 缓存
             * 重渲判定见 LIST 页态; 复位不依赖页态自删（录音定时器即时停, 防其在首页
             * 重建后仍每 100ms 清屏砸毁首页图像对象致悬垂 panic）。判据住 story_page.c
             * 纯逻辑缝 kidpal_story_session_end（seam [R2] 权威 = record_session_repair.
             * acceptance.c 文件头; 页态/定时器为设备运行态, 经 mcp 桥执行）。 */
            {
                int story_end_page = -1;
                if (kidpal_story_session_end(KidpalStoryViewPageState(),
                                             &story_end_page) == 1) {
                    KidpalStorySessionEndApply(story_end_page);
                }
            }
            /* RSL2-A2 [I1]（261028-p3-record-session-mirror-desync-fix, M2）: 收尾沿镜像
             * := 权威视图页态（确定性同步）。主循环 STATE_CHANGED 先于 SCHEDULE, 收尾当轮
             * 节点缓存可能仍是旧值——显式同步保证随后 voice_end 的 [R5] 页态解析取真值,
             * 杜绝「story_page_input 先改页再返码」路径上把丢弃后的 LIST 复活成 RECORDING。
             * 次序承重: 必须在 [R2] 收尾块之后、keybar_scene_reset/voice_end 之前, 且不得
             * 插进 keybar_scene_reset 与其判据之间（chrome_ghost CG-A1 既有锚同区互守）。 */
            KidpalStorySessionEndSync();
            /* 261022-p3-record-chrome-ghost-fix（CG-A1）: 语音会话收尾沿**先**复位键帽
             * 场景态（keybar 场景单源 kidpal_keybar_scene_reset）——回「非语音场景默认」
             * （出闭集）。修复前该态永不复位, 下一轮故事子态缓存未同步的过渡窗会被通用
             * 分支画出上一次语音轮残留键帽（KPLOG005 缺陷①因一: 聆听残留「说完啦/结束」、
             * 思考残留「不等了/结束」约 300~500ms）。
             * 落位在 voice_end 之前: voice_end 内部会重渲基层页并驱动键帽条同步, 复位先落
             * 才能让那次同步读到的已是默认态（否则会话收尾那一次刷新仍按残留场景裁决,
             * 键帽要等下一次 chrome 同步才盖回——正是「鬼影 300~500ms」的形态）。
             * 提醒弹窗驻留（REMINDER 在顶）时不复位——提醒材料壳的显隐判据取同一场景态
             * （KVST_SCENE_REMIND）, 复位会把驻留中的提醒外壳/小伴整页熄掉; 提醒关闭走
             * 自己的场景沿（REMIND 之后的下一次 apply_look）。复位幂等: 非语音轮路径
             * （无 VOICE 层）同样安全。 */
            if (kidpal_render_node_front_is_reminder() != 1) {
                kidpal_keybar_scene_reset();
                ESP_LOGI(TAG, "keybar_scene_reset corr_id=kdp-%u src=session_end",
                         static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            }
            /* 语音轮结束 -> 前台回落（无服务端页驻留则回首页，有回答页驻留则保持其前台，
             * OQ3=c 回答驻留）；事件驱动取代原 1s tick 轮询对账。 */
            kidpal_render_node_voice_end();
            /* K3: 会话收尾 -> 记录语境标志清位 + 录音态切换镜像复位（引导气泡覆写/
             * 开口切录音态/stt 旁路喂入的消费门; 置位单点 = mcp_server REC_SESSION
             * 消费位。盲区申报③: 清位时机 = 实现细节, 可观测后果由消费链承载）。 */
            kidpal_story_session_ctx_set(0);
            s_story_rec_entered = false;
            /* M1-K4 RM-A3「录音会话结束立刻弹」触发（触发点路由不锁）：录音态提醒到点
             * 排队延后，会话/录音收尾回 Idle 即调 RaiseReminderQueue —— 此刻录音态已
             * 消除、gate 判 RAISE，把排队提醒立即弹出（无排队或弹窗已驻留时其内部
             * raise_next 返回 0 = no-op，恒安全幂等）。 */
            McpServer::GetInstance().RaiseReminderQueue();
            break;
        case kDeviceStateConnecting:
            display->SetStatus(Lang::Strings::CONNECTING);
            display->SetEmotion("neutral");
            /* 驻留清除时机 (OQ3=c, 由 layer 管理冻结语义): 新语音轮 -> 清聊天内容。
             * 回答结束/页面切换不清（聊天内容任何页可见，回答天然留屏）。 */
            if (kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)) {
                display->SetChatMessage("system", "");
            }
            /* 新语音轮开始 -> 清上一轮驻留 transient + 推 VOICE 前台层（事件驱动）。 */
            kidpal_render_node_voice_begin();
            break;
        case kDeviceStateListening:
            display->SetStatus(Lang::Strings::LISTENING);
            display->SetEmotion("neutral");

            /* K3 [S7]: 会话发起信号 kidpal_session_ctx（契约 §四）——首帧音频前、
             * 音频处理器启动门之外无条件执行（门外 = 通道复用会话不短路漏发, notes
             * ②③; 调用恰一次, 发送失败降级普通会话不重试）。source_page 单点映射
             * 消费（notes ④: KEY 按经 REC_SESSION 消费位聆听入口与唤醒词双入口都
             * 汇入本分支）。负锚: 本信号只住会话发起, 会话收尾/中断路径零 ctx。
             * K2 additive（261104-k2-points-device-ui / D-08）: P3 详细账本子页把
             * source_page 扩为 points_ledger 并附带 selected_id（只读、只来自最近
             * 收到的 points_ledger items；设备不造 id）。积分主页/其余页保持原映射。 */
            {
                int pts_page = kidpal_render_node_get_page();
                const char* source_page = kidpal_story_ctx_source_page(pts_page);
                const char* selected_id = nullptr;
                if (pts_page == KP_PAGE_POINTS_HOME &&
                    kidpal_points_ui_substate() == KPTS_SUBSTATE_LEDGER) {
                    source_page = "points_ledger";
                    const char* cand = kidpal_points_ui_selected_id();
                    if (cand != nullptr && cand[0] != '\0') {
                        selected_id = cand;
                    }
                }
                /* K5 封皮普通问答语境（261104-k5-story-cover-nav; S-F2.6-AC4 / 契约 §四）:
                 * P4 封皮子态 -> source_page="story_cover"（front 闭集普通问答, 不切记录
                 * 语境、不推 story_list）。列表子态仍为 "story" 记录语境（
                 * kidpal_story_ctx_source_page 映射零改——[S1] 签名不动）。判据用会话页态
                 * 镜像只读 getter（VOICE 前台时 kidpal_render_node_story_substate 恒 -1,
                 * 不能用）。 */
                if (pts_page == KP_PAGE_STORY &&
                    kidpal_render_node_story_session_substate() == KSTORY_PAGE_COVER) {
                    source_page = "story_cover";
                }
                protocol_->SendKidpalSessionCtx(source_page, selected_id);
            }

            // Make sure the audio processor is running
            if (!audio_service_.IsAudioProcessorRunning()) {
                // Send the start listening command
                protocol_->SendStartListening(listening_mode_);
                audio_service_.EnableVoiceProcessing(true);
                audio_service_.EnableWakeWordDetection(false);
            }

            // Play popup sound after ResetDecoder (in EnableVoiceProcessing) has been called
            if (play_popup_on_listening_) {
                play_popup_on_listening_ = false;
                audio_service_.PlaySound(Lang::Sounds::OGG_POPUP);
            }

            /* 261116-channel-resident-core（真机轮 B 修复轮, M3 卫生复位 + 迁移条件
             * 分派）: 会话入口复位语音看门缝 + VAD 电平闩锁。AfeAudioProcessor::Stop()
             * 不复位成员 is_speaking_，旧会话的电平可能被新会话继承（闩锁）。
             *   - 旧状态 == Speaking（记录会话的引导播报 TTS 造成 Listening -> Speaking
             *     -> Listening 往返）：软复位，清计时/诊断/审计六件但**保留 asr_seen**
             *     ——它是 D3 空素材 producer 的唯一素材门，清 0 会把有素材的故事会话判成
             *     空素材、静置 60 秒不归档地释放（孩子已讲内容丢失）；
             *   - 其余入口（新会话）：七字段全清，跨会话零残留（quiet_s / asr_seen /
             *     suspect / diag_last_s 不得继承上一会话）。
             * 落位 = 本分支尾部（SendKidpalSessionCtx 与其页态解析块之后，不打断
             * RSL-A2 的发送时点时序）；复位与发送互不依赖。 */
            if (s_watch_prev_state == kDeviceStateSpeaking) {
                kidpal_voice_watch_reset_soft(&s_voice_watch);  /* 会话内 TTS 往返：保素材证据 */
            } else {
                kidpal_voice_watch_reset(&s_voice_watch);       /* 新会话：七字段全清 */
            }
            s_vad_speaking = false;
            /* 会话入口无条件诊断行（src=enter; 诊断限频的周期起点由本行把
             * diag_last_s 归到复位后的 tick 基准）。 */
            {
                int vw_rec_ctx = kidpal_story_session_ctx_active() ? 1 : 0;
                int vw_rec_mat = s_story_rec_entered ? 1 : 0;

                ESP_LOGI(TAG, "listening_diag corr_id=kdp-%u src=enter state=%d vad=%d run=%d quiet=%d asr=%d ticks=%d rec_ctx=%d rec_mat=%d suspect=%d",
                         static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu),
                         static_cast<int>(GetDeviceState()), s_vad_speaking ? 1 : 0,
                         s_voice_watch.level_run_s, s_voice_watch.quiet_s,
                         s_voice_watch.asr_age_s, clock_ticks_, vw_rec_ctx, vw_rec_mat,
                         s_voice_watch.suspect);
                s_voice_watch.diag_last_s = s_voice_watch.tick_s;
                s_vw_diag_vad = s_vad_speaking ? 1 : 0;
                s_vw_diag_rec_ctx = vw_rec_ctx;
                s_vw_diag_rec_mat = vw_rec_mat;
                s_vw_diag_suspect = s_voice_watch.suspect;
            }
            break;
        case kDeviceStateSpeaking:
            display->SetStatus(Lang::Strings::SPEAKING);

            if (listening_mode_ != kListeningModeRealtime) {
                audio_service_.EnableVoiceProcessing(false);
                // Only AFE wake word can be detected in speaking mode
                audio_service_.EnableWakeWordDetection(audio_service_.IsAfeWakeWord());
            }
            audio_service_.ResetDecoder();
            break;
        case kDeviceStateWifiConfiguring:
            audio_service_.EnableVoiceProcessing(false);
            audio_service_.EnableWakeWordDetection(false);
            break;
        default:
            // Do nothing
            break;
    }
}

void Application::Schedule(std::function<void()>&& callback) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        main_tasks_.push_back(std::move(callback));
    }
    xEventGroupSetBits(event_group_, MAIN_EVENT_SCHEDULE);
}

void Application::AbortSpeaking(AbortReason reason) {
    ESP_LOGI(TAG, "Abort speaking");
    aborted_ = true;
    if (protocol_) {
        protocol_->SendAbortSpeaking(reason);
    }
}

void Application::SetListeningMode(ListeningMode mode) {
    listening_mode_ = mode;
    SetDeviceState(kDeviceStateListening);
}

void Application::Reboot() {
    ESP_LOGI(TAG, "Rebooting...");
    // Disconnect the audio channel
    if (protocol_ && protocol_->IsAudioChannelOpened()) {
        protocol_->CloseAudioChannel();
    }
    protocol_.reset();
    audio_service_.Stop();

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}

bool Application::UpgradeFirmware(const std::string& url, const std::string& version) {
    auto& board = Board::GetInstance();
    auto display = board.GetDisplay();

    std::string upgrade_url = url;
    std::string version_info = version.empty() ? "(Manual upgrade)" : version;

    // Close audio channel if it's open
    if (protocol_ && protocol_->IsAudioChannelOpened()) {
        ESP_LOGI(TAG, "Closing audio channel before firmware upgrade");
        protocol_->CloseAudioChannel();
    }
    ESP_LOGI(TAG, "Starting firmware upgrade from URL: %s", upgrade_url.c_str());

    Alert(Lang::Strings::OTA_UPGRADE, Lang::Strings::UPGRADING, "download", Lang::Sounds::OGG_UPGRADE);
    vTaskDelay(pdMS_TO_TICKS(3000));

    SetDeviceState(kDeviceStateUpgrading);

    std::string message = std::string(Lang::Strings::NEW_VERSION) + version_info;
    display->SetChatMessage("system", message.c_str());

    board.SetPowerSaveLevel(PowerSaveLevel::PERFORMANCE);
    audio_service_.Stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    bool upgrade_success = Ota::Upgrade(upgrade_url, [display](int progress, size_t speed) {
        std::thread([display, progress, speed]() {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d%% %uKB/s", progress, speed / 1024);
            display->SetChatMessage("system", buffer);
        }).detach();
    });

    if (!upgrade_success) {
        // Upgrade failed, restart audio service and continue running
        ESP_LOGE(TAG, "Firmware upgrade failed, restarting audio service and continuing operation...");
        audio_service_.Start(); // Restart audio service
        board.SetPowerSaveLevel(PowerSaveLevel::LOW_POWER); // Restore power save level
        Alert(Lang::Strings::ERROR, Lang::Strings::UPGRADE_FAILED, "circle_xmark", Lang::Sounds::OGG_EXCLAMATION);
        vTaskDelay(pdMS_TO_TICKS(3000));
        return false;
    } else {
        // Upgrade success, reboot immediately
        ESP_LOGI(TAG, "Firmware upgrade successful, rebooting...");
        display->SetChatMessage("system", "Upgrade successful, rebooting...");
        vTaskDelay(pdMS_TO_TICKS(1000)); // Brief pause to show message
        Reboot();
        return true;
    }
}

void Application::WakeWordInvoke(const std::string& wake_word) {
    if (!protocol_) {
        return;
    }

    auto state = GetDeviceState();

    // K6 (M3-K6): 自动建连 in-flight 期间唤醒 invoke no-op（守卫; 本板非活路径, 一并
    // 加; 既有分支行为语义零改动）。
    if (auto_connect_in_flight_) {
        /* 261117 P2: 离线期人工意向位——编程唤起入口在长跑 in-flight 时不静默 no-op：
         * 置意向位 + 审计行, 连上后经 Schedule 补投唤醒事件位（与检测沿同主事件位）。 */
        pending_manual_intent_ = MAIN_EVENT_WAKE_WORD_DETECTED;
        ESP_LOGI(TAG, "manual_intent_queued corr_id=kdp-%u src=queued_invoke",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        ESP_LOGI(TAG, "AutoConnect in-flight, ignore wake word invoke");
        return;
    }

    if (state == kDeviceStateIdle) {
        audio_service_.EncodeWakeWord();

        if (!protocol_->IsAudioChannelOpened()) {
            SetDeviceState(kDeviceStateConnecting);
            if (!protocol_->OpenAudioChannel()) {
                audio_service_.EnableWakeWordDetection(true);
                return;
            }
        }

        ESP_LOGI(TAG, "Wake word detected: %s", wake_word.c_str());
#if CONFIG_USE_AFE_WAKE_WORD || CONFIG_USE_CUSTOM_WAKE_WORD
        // Encode and send the wake word data to the server
        while (auto packet = audio_service_.PopWakeWordPacket()) {
            protocol_->SendAudio(std::move(packet));
        }
        // Set the chat state to wake word detected
        protocol_->SendWakeWordDetected(wake_word);
        SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop : kListeningModeRealtime);
#else
        // Set flag to play popup sound after state changes to listening
        // (PlaySound here would be cleared by ResetDecoder in EnableVoiceProcessing)
        play_popup_on_listening_ = true;
        SetListeningMode(aec_mode_ == kAecOff ? kListeningModeAutoStop : kListeningModeRealtime);
#endif
    } else if (state == kDeviceStateSpeaking) {
        Schedule([this]() {
            AbortSpeaking(kAbortReasonNone);
        });
    } else if (state == kDeviceStateListening) {
        Schedule([this]() {
            /* 261116-channel-resident-core（P1 T1）: 编程唤起结束会话——lambda 体在
             * 主循环上下文（= Schedule 排队），直调统一收口（保同步语义）。
             * 不再拆通道 / 不降省电档 / 不排常驻重建。本路审计 token 留在调用点的
             * 日志行上（四路诊断口径区分；注释不重抄 token 字面）。 */
            ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=wake_invoke state=session_end",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            HandleEndVoiceSessionEvent();
        });
    }
}

bool Application::CanEnterSleepMode() {
    if (GetDeviceState() != kDeviceStateIdle) {
        return false;
    }

    if (protocol_ && protocol_->IsAudioChannelOpened()) {
        return false;
    }

    if (!audio_service_.IsIdle()) {
        return false;
    }

    // Now it is safe to enter sleep mode
    return true;
}

void Application::SendMcpMessage(const std::string& payload) {
    // Always schedule to run in main task for thread safety
    Schedule([this, payload = std::move(payload)]() {
        if (protocol_) {
            protocol_->SendMcpMessage(payload);
        }
    });
}

void Application::SendKidpalStoryDelete(const char* id) {
    // Always schedule to run in main task for thread safety (SendMcpMessage 同款).
    // id 拷贝进闭包——调用方缓冲（mcp_server pending 记账区）可能先于发送被改写。
    std::string safe_id = (id != nullptr) ? id : "";
    Schedule([this, safe_id]() {
        if (protocol_) {
            protocol_->SendKidpalStoryDelete(safe_id.c_str());
        }
    });
}

void Application::SetAecMode(AecMode mode) {
    aec_mode_ = mode;
    Schedule([this]() {
        auto& board = Board::GetInstance();
        auto display = board.GetDisplay();
        switch (aec_mode_) {
        case kAecOff:
            audio_service_.EnableDeviceAec(false);
            display->ShowNotification(Lang::Strings::RTC_MODE_OFF);
            break;
        case kAecOnServerSide:
            audio_service_.EnableDeviceAec(false);
            display->ShowNotification(Lang::Strings::RTC_MODE_ON);
            break;
        case kAecOnDeviceSide:
            audio_service_.EnableDeviceAec(true);
            display->ShowNotification(Lang::Strings::RTC_MODE_ON);
            break;
        }

        // If the AEC mode is changed, close the audio channel
        if (protocol_ && protocol_->IsAudioChannelOpened()) {
            protocol_->CloseAudioChannel();
        }
    });
}

void Application::PlaySound(const std::string_view& sound) {
    audio_service_.PlaySound(sound);
}

void Application::ResetProtocol() {
    Schedule([this]() {
        // Close audio channel if opened
        if (protocol_ && protocol_->IsAudioChannelOpened()) {
            protocol_->CloseAudioChannel();
        }
        // Reset protocol
        protocol_.reset();
    });
}

