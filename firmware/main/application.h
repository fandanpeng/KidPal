#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <esp_timer.h>

#include <string>
#include <mutex>
#include <deque>
#include <memory>

#include "protocol.h"
#include "ota.h"
#include "audio_service.h"
#include "device_state.h"
#include "device_state_machine.h"

// Main event bits
#define MAIN_EVENT_SCHEDULE             (1 << 0)
#define MAIN_EVENT_SEND_AUDIO           (1 << 1)
#define MAIN_EVENT_WAKE_WORD_DETECTED   (1 << 2)
#define MAIN_EVENT_VAD_CHANGE           (1 << 3)
#define MAIN_EVENT_ERROR                (1 << 4)
#define MAIN_EVENT_ACTIVATION_DONE      (1 << 5)
#define MAIN_EVENT_CLOCK_TICK           (1 << 6)
#define MAIN_EVENT_NETWORK_CONNECTED    (1 << 7)
#define MAIN_EVENT_NETWORK_DISCONNECTED (1 << 8)
#define MAIN_EVENT_TOGGLE_CHAT          (1 << 9)
#define MAIN_EVENT_START_LISTENING      (1 << 10)
#define MAIN_EVENT_STOP_LISTENING       (1 << 11)
#define MAIN_EVENT_STATE_CHANGED        (1 << 12)


enum AecMode {
    kAecOff,
    kAecOnDeviceSide,
    kAecOnServerSide,
};

class Application {
public:
    static Application& GetInstance() {
        static Application instance;
        return instance;
    }
    // Delete copy constructor and assignment operator
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * Initialize the application
     * This sets up display, audio, network callbacks, etc.
     * Network connection starts asynchronously.
     */
    void Initialize();

    /**
     * Run the main event loop
     * This function runs in the main task and never returns.
     * It handles all events including network, state changes, and user interactions.
     */
    void Run();

    DeviceState GetDeviceState() const { return state_machine_.GetState(); }
    bool IsVoiceDetected() const { return audio_service_.IsVoiceDetected(); }
    
    /**
     * Request state transition
     * Returns true if transition was successful
     */
    bool SetDeviceState(DeviceState state);

    /**
     * Schedule a callback to be executed in the main task
     */
    void Schedule(std::function<void()>&& callback);

    /**
     * Alert with status, message, emotion and optional sound
     */
    void Alert(const char* status, const char* message, const char* emotion = "", const std::string_view& sound = "");
    void DismissAlert();

    void AbortSpeaking(AbortReason reason);

    /**
     * Toggle chat state (event-based, thread-safe)
     * Sends MAIN_EVENT_TOGGLE_CHAT to be handled in Run()
     */
    void ToggleChatState();

    /**
     * Start listening (event-based, thread-safe)
     * Sends MAIN_EVENT_START_LISTENING to be handled in Run()
     */
    void StartListening();

    /**
     * Stop listening (event-based, thread-safe)
     * Sends MAIN_EVENT_STOP_LISTENING to be handled in Run()
     */
    void StopListening();

    void Reboot();
    void WakeWordInvoke(const std::string& wake_word);
    bool UpgradeFirmware(const std::string& url, const std::string& version = "");
    bool CanEnterSleepMode();
    void SendMcpMessage(const std::string& payload);
    /* §四 kidpal_story_delete 公开委托（SendMcpMessage 同款 Schedule→protocol;
     * TaskPackage 260924-m4-k2-p3-star-notebook-ui 发送原语四件套之二）。id 闭集
     * 纪律由调用方（mcp_server 确认路径）承接: 只发 story_list items 下发过的 id。 */
    void SendKidpalStoryDelete(const char* id);
    /* K3 (260924-m4-k3-record-session-live) [S5]: 录音态丢弃（BOOT 任一手势）断连面
     * ——直接关音频通道即达 front 断开钩子 acc.cancel（弹缓冲不落盘, 不发 listen
     * stop）。语义单点直连 protocol, 不经 toggle 状态机（STOP 收尾保存面走
     * StopListening, 两路径不混——notes (e)）。 */
    void CloseAudioChannel();
    void SetAecMode(AecMode mode);
    AecMode GetAecMode() const { return aec_mode_; }
    void PlaySound(const std::string_view& sound);
    AudioService& GetAudioService() { return audio_service_; }
    
    /**
     * Reset protocol resources (thread-safe)
     * Can be called from any task to release resources allocated after network connected
     * This includes closing audio channel, resetting protocol and ota objects
     */
    void ResetProtocol();

private:
    Application();
    ~Application();

    std::mutex mutex_;
    std::deque<std::function<void()>> main_tasks_;
    std::unique_ptr<Protocol> protocol_;
    EventGroupHandle_t event_group_ = nullptr;
    esp_timer_handle_t clock_timer_handle_ = nullptr;
    DeviceStateMachine state_machine_;
    ListeningMode listening_mode_ = kListeningModeAutoStop;
    AecMode aec_mode_ = kAecOff;
    std::string last_error_message_;
    AudioService audio_service_;
    std::unique_ptr<Ota> ota_;

    bool has_server_time_ = false;
    bool aborted_ = false;
    bool assets_version_checked_ = false;
    bool play_popup_on_listening_ = false;  // Flag to play popup sound after state changes to listening
    int clock_ticks_ = 0;
    TaskHandle_t activation_task_handle_ = nullptr;
    // K6 (M3-K6 boot-auto-connect, Shape A): 自动建连互斥标志——in-flight 期间恒 true,
    // 标志互斥替代 Connecting 状态互斥（自动建连全程状态恒 Idle）。置位仅主循环 handler
    // 判定后、task 所有退出路径必清（泄漏 = 人工按键永久 no-op + 自动建连永久失效）。
    bool auto_connect_in_flight_ = false;


    // Event handlers
    void HandleStateChangedEvent();
    void HandleToggleChatEvent();
    void HandleStartListeningEvent();
    void HandleStopListeningEvent();
    void HandleNetworkConnectedEvent();
    void HandleNetworkDisconnectedEvent();
    void HandleActivationDoneEvent();
    void HandleWakeWordDetectedEvent();

    // Activation task (runs in background)
    void ActivationTask();
    // K6 (M3-K6): 自动建连「判 + 起」入口——由两个触发点（激活完成 / 网络重连 Idle）
    // 在 main-loop handler 内同步调用（TOCTOU 判设闭合）；内部预检 url/token、守卫
    // 三条件、置 in-flight 标志、xTaskCreate "AutoConnect" 后台 task（task 内 3 次
    // 退避重试 + 每次重试前重查三条件 + 所有退出路径必清标志）。
    void TryStartAutoConnect();

    /* 261010 [S9] 常驻重建排程（B 路径, 决策 29「永不休眠」）: 会话收尾类本地关后延迟
     * kResidentRebuildDelayS 秒经 Schedule 回主循环排入 TryStartAutoConnect（复用守卫
     * 三条件与退避——重建只经既有 AutoConnect task）。 */
    void ScheduleResidentRebuild();

    // Helper methods
    void CheckAssetsVersion();
    void InitializeProtocol();
    void SetListeningMode(ListeningMode mode);
    
    // State change handler called by state machine
    void OnStateChanged(DeviceState old_state, DeviceState new_state);
};


class TaskPriorityReset {
public:
    TaskPriorityReset(BaseType_t priority) {
        original_priority_ = uxTaskPriorityGet(NULL);
        vTaskPrioritySet(NULL, priority);
    }
    ~TaskPriorityReset() {
        vTaskPrioritySet(NULL, original_priority_);
    }

private:
    BaseType_t original_priority_;
};

#endif // _APPLICATION_H_
