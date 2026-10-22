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
/* [G3] 帧 B 子态沿桥（261020 D4-5, fixer; 定义住 mcp_server.cc, 同款免扩公共头纪律）:
 * 引导播报中子态从设备侧既有 tts 帧派生（记录语境内引擎抑制 LLM/TTS, kidpal_speak
 * 引导注入 = 唯一 TTS 源, 零 wire 新增）——tts start 沿置 rec_substate=1、tts stop 沿
 * 复位 0（幂等, 仅 RECORDING 页态有语义）。RDG-F2 键位分叉 / RDG-F3 整理态吞键零触碰
 * （桥只写子态标量, 分叉判定仍归 story_page_input 冻结决策表）。 */
void KidpalStoryGuideSpeechEdge(int speaking);
static bool s_vad_speaking = false;
static bool s_story_rec_entered = false;

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
 * K6（TaskPackage 260923-m3-k6-boot-auto-connect）[S3] 有界退避决策表：
 * 自动建连重试 3 次、递增秒序列 5s/10s/20s（封顶 3 次; host 齿钉决策表形态不钉
 * 时序, 实时节奏归真机窗 K6-A7; vTaskDelay 换算乘法形态见 TryStartAutoConnect task）。
 * ------------------------------------------------------------------------ */
static const int kAutoConnectBackoffS[3] = { 5, 10, 20 };

/* -------------------------------------------------------------------------- *
 * 261010 [S9] 常驻重建 T 秒延迟（B 路径, 决策 29「永不休眠」）: 会话收尾类本地关后
 * 延迟重建常驻连接——值 5（退避表首档 5s 量级, plan 定值并冻结进验收件; 改值须回
 * planner 重冻结）。本 needle 首现即本定义。
 * ------------------------------------------------------------------------ */
static const int kResidentRebuildDelayS = 5;

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
        xEventGroupSetBits(event_group_, MAIN_EVENT_STATE_CHANGED);
    });

    // Start the clock timer to update the status bar
    esp_timer_start_periodic(clock_timer_handle_, 1000000);

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
        MAIN_EVENT_STATE_CHANGED;

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
        }
    }
}

void Application::HandleNetworkConnectedEvent() {
    ESP_LOGI(TAG, "Network connected");
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
        TryStartAutoConnect();
    }

    // Update the status bar immediately to show the network state
    auto display = Board::GetInstance().GetDisplay();
    display->UpdateStatusBar(true);
}

void Application::HandleNetworkDisconnectedEvent() {
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

    /* [K6-A1] 守卫三条件（main-loop handler 内「判 + 起」, TOCTOU 判设闭合）:
     * state==Idle && 通道未开 && 非 in-flight。无协议对象时退化 no-op。 */
    if (GetDeviceState() != kDeviceStateIdle ||
        (protocol_ && protocol_->IsAudioChannelOpened()) ||
        auto_connect_in_flight_) {
        return;
    }
    auto_connect_in_flight_ = true;

    /* 后台 task（沿用 activation 模板: 栈 4096*2、优先级 2、lambda 直排——不阻塞主
     * 循环; [S3] 3 次退避重试、每次尝试前重查守卫三条件、所有退出路径必清标志、
     * 退出前打栈高水位日志）。 */
    xTaskCreate([](void* arg) {
        Application* app = static_cast<Application*>(arg);
        for (int attempt = 0; attempt < 3; attempt++) {
            /* 重试前重查守卫三条件（违反即 bail——人工路径已接管或通道已开）。 */
            if (app->GetDeviceState() != kDeviceStateIdle ||
                (app->protocol_ && app->protocol_->IsAudioChannelOpened())) {
                break;
            }
            if (app->protocol_->OpenAudioChannel()) {
                break;  /* 成功: 通道开 + Idle = 首轮对话后稳态, 无状态迁移 */
            }
            if (attempt < 2) {
                vTaskDelay(pdMS_TO_TICKS(kAutoConnectBackoffS[attempt] * 1000));
            }
        }
        app->auto_connect_in_flight_ = false;  /* 所有退出路径（成功/重试耗尽/bail）必清 */
        ESP_LOGI(TAG, "AutoConnect task exit, stack high water mark=%u",
                 (unsigned)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }, "AutoConnect", 4096 * 2, this, 2, nullptr);
}

void Application::ScheduleResidentRebuild() {
    /* [S9] 常驻重建缝（B 路径; 决策 29「永不休眠」）: 会话收尾类本地关后 T 秒
     * （kResidentRebuildDelayS）重建常驻连接。延迟在独立短命线程内睡眠（不阻塞主
     * 循环——helper 由主循环 handler / Schedule lambda 调用）; 到点经 Schedule( 回主
     * 循环上下文排入 TryStartAutoConnect()（复用既有 AutoConnect task 守卫三条件与
     * 退避——重建只经既有 AutoConnect task, 零第二条重连路径; 判 + 起在主循环, 与
     * K6 [S1] 判设同源）。 */
    std::thread([this]() {
        vTaskDelay(pdMS_TO_TICKS(kResidentRebuildDelayS * 1000));
        Schedule([this]() {
            TryStartAutoConnect();
        });
    }).detach();
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

void Application::StartListening() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_START_LISTENING);
}

void Application::StopListening() {
    xEventGroupSetBits(event_group_, MAIN_EVENT_STOP_LISTENING);
}

void Application::CloseAudioChannel() {
    /* K3 [S5]: 录音态丢弃面（BOOT 任一手势）——断连即达 front ws_disconnect
     * cancel 钩子（弹缓冲不落盘）; 不发 listen stop（丢弃不是归档）。 */
    if (protocol_) {
        protocol_->CloseAudioChannel();
        /* 261010 [S9] 本地关收尾补偿（[S7] 令 OnDisconnected 对本地关提前 return，
         * A 区 lambda 的 Idle 收尾 + LOW_POWER 剖面被抑制 → 设备态停留
         * Listening/Speaking，常驻重建守卫 state!=Idle 直 return = 重建永不触发）:
         * 主循环上下文（按键 dispatch 栈）内同步补回设备态 + 省电剖面（Connecting/
         * Listening/Speaking→Idle 全合法, device_state_machine.cc:80-93）。收尾落
         * 调用点，不入 helper（[S9] helper 区零 SetDeviceState 负锚）。 */
        SetDeviceState(kDeviceStateIdle);
        Board::GetInstance().SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
        ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=discard_record state=idle",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
    }
    /* 261010 [S9] 会话收尾类本地关 → T 秒内常驻重建（B 路径）。 */
    ScheduleResidentRebuild();
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
        protocol_->CloseAudioChannel();
        /* 261010 [S9] 本地关收尾补偿（同 CloseAudioChannel 丢弃面）: 主循环 handler
         * 内同步补回 Idle + LOW_POWER 剖面。收尾落调用点，不入 helper。 */
        SetDeviceState(kDeviceStateIdle);
        Board::GetInstance().SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
        ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=toggle_chat state=idle",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        /* 261010 [S9] 会话收尾类本地关 → T 秒内常驻重建（B 路径）。 */
        ScheduleResidentRebuild();
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
        return;
    }

    if (!protocol_) {
        ESP_LOGE(TAG, "Protocol not initialized");
        return;
    }

    // K6 (M3-K6): 自动建连 in-flight 期间人工 start-listening no-op（守卫; 既有分支
    // 行为语义零改动）。
    if (auto_connect_in_flight_) {
        ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual start listening");
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

void Application::HandleWakeWordDetectedEvent() {
    if (!protocol_) {
        return;
    }

    auto state = GetDeviceState();

    if (state == kDeviceStateIdle) {
        // K6 (M3-K6): 自动建连 in-flight 期间唤醒 no-op（守卫置于 EncodeWakeWord 之前
        // ——in-flight 期不启动唤醒音频编码; 既有分支行为语义零改动）。
        if (auto_connect_in_flight_) {
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
             * 汇入本分支）。负锚: 本信号只住会话发起, 会话收尾/中断路径零 ctx。 */
            protocol_->SendKidpalSessionCtx(
                kidpal_story_ctx_source_page(kidpal_render_node_get_page()));

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
            if (protocol_) {
                protocol_->CloseAudioChannel();
            }
            /* 261010 [S9] 本地关收尾补偿（同 CloseAudioChannel 丢弃面）: Schedule
             * lambda = 主循环上下文，同步补回 Idle + LOW_POWER 剖面。收尾落调用点。 */
            SetDeviceState(kDeviceStateIdle);
            Board::GetInstance().SetPowerSaveLevel(PowerSaveLevel::LOW_POWER);
            ESP_LOGI(TAG, "local_close_settle corr_id=kdp-%u src=wake_invoke state=idle",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            /* 261010 [S9] 会话收尾类本地关 → T 秒内常驻重建（B 路径）。 */
            ScheduleResidentRebuild();
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

