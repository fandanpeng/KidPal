/*
 * MCP Server Implementation
 * Reference: https://modelcontextprotocol.io/specification/2024-11-05
 */

#include "mcp_server.h"
#include <esp_log.h>
#include <esp_app_desc.h>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <esp_pthread.h>

#include "application.h"
#include "display.h"
#include "oled_display.h"
#include "board.h"
#include "settings.h"
#include "lvgl_theme.h"
#include "lvgl_display.h"
#include "display/view_router.h"
#include "display/schedule_page.h"
#include "display/char_stroke.h"   /* 查生字 P4 (包 260829-char-stroke): 缺字段防御降级门 */
#include "display/char_page.h"     /* 查生字 P4 LVGL 页 (设备侧) */

#define TAG "MCP"

/* 从一段 E1 display 载荷的嵌套 data JSON 里取一个字符串字段 (如 text / action)。
 * 取不到/非字符串 -> 空串 (调用方按降级或空内容处理, 不崩)。 */
static std::string mcp_json_get_string(const std::string& json, const char* key) {
    if (json.empty()) {
        return std::string();
    }
    cJSON *root = cJSON_Parse(json.c_str());
    if (root == nullptr) {
        return std::string();
    }
    std::string out;
    cJSON *item = cJSON_GetObjectItem(root, key);
    if (cJSON_IsString(item)) {
        out = item->valuestring;
    }
    cJSON_Delete(root);
    return out;
}

McpServer::McpServer() {
}

McpServer::~McpServer() {
    for (auto tool : tools_) {
        delete tool;
    }
    tools_.clear();
}

void McpServer::AddCommonTools() {
    // *Important* To speed up the response time, we add the common tools to the beginning of
    // the tools list to utilize the prompt cache.
    // **重要** 为了提升响应速度，我们把常用的工具放在前面，利用 prompt cache 的特性。

    // Backup the original tools list and restore it after adding the common tools.
    auto original_tools = std::move(tools_);
    auto& board = Board::GetInstance();

    // Do not add custom tools here.
    // Custom tools must be added in the board's InitializeTools function.

    AddTool("self.get_device_status",
        "Provides the real-time information of the device, including the current status of the audio speaker, screen, battery, network, etc.\n"
        "Use this tool for: \n"
        "1. Answering questions about current condition (e.g. what is the current volume of the audio speaker?)\n"
        "2. As the first step to control the device (e.g. turn up / down the volume of the audio speaker, etc.)",
        PropertyList(),
        [&board](const PropertyList& properties) -> ReturnValue {
            return board.GetDeviceStatusJson();
        });

    AddTool("self.audio_speaker.set_volume", 
        "Set the volume of the audio speaker. If the current volume is unknown, you must call `self.get_device_status` tool first and then call this tool.",
        PropertyList({
            Property("volume", kPropertyTypeInteger, 0, 100)
        }), 
        [&board](const PropertyList& properties) -> ReturnValue {
            auto codec = board.GetAudioCodec();
            codec->SetOutputVolume(properties["volume"].value<int>());
            return true;
        });
    
    auto backlight = board.GetBacklight();
    if (backlight) {
        AddTool("self.screen.set_brightness",
            "Set the brightness of the screen.",
            PropertyList({
                Property("brightness", kPropertyTypeInteger, 0, 100)
            }),
            [backlight](const PropertyList& properties) -> ReturnValue {
                uint8_t brightness = static_cast<uint8_t>(properties["brightness"].value<int>());
                backlight->SetBrightness(brightness, true);
                return true;
            });
    }

    // kidpal/display (方向 B, 包 260829-voice-roundtrip): E1 display 消费端。
    // 网关经 mcp JSON-RPC tools/call 下发 (契约 §四 mcp 工具调用 / §1.5-4)，载荷形状
    // {"view":…,"data":{…}}。view 经共享底座 kidpal_display_route 路由到对应渲染面；
    // 白名单外/NULL view -> UNKNOWN 独立降级路径，绝不盲渲染 (V27/§4.0)。工具回调返回
    // cJSON 结果，McpTool::Call 序列化成 JSON-RPC result -> 送达确认 (对齐 relay_ack)。
    AddTool("kidpal/display",
        "Render an E1 display payload issued by the KidPal gateway on this board. "
        "The payload carries a view (char/schedule/story_list/story_text/text/home/"
        "control whitelist) and optional data; the board routes it to the matching "
        "display face, applies control actions, and returns a delivery acknowledgement.",
        PropertyList({
            Property("view", kPropertyTypeString),
            Property("data", kPropertyTypeString, std::string(""))
        }),
        [](const PropertyList& arguments) -> ReturnValue {
            std::string view = arguments["view"].value<std::string>();
            std::string data = arguments["data"].value<std::string>();
            int route = kidpal_display_route(view.c_str());
            auto& app = Application::GetInstance();

            cJSON *ack = cJSON_CreateObject();
            switch (route) {
                case 0: /* VRT_ROUTE_TEXT —— 全文上屏 (滚动/翻屏由显示阅读面承载, 分页
                         逻辑 kidpal_display_text_page_count; S-F4.1 否摘要) */
                    cJSON_AddStringToObject(ack, "route", "text");
                    {
                        std::string text = mcp_json_get_string(data, "text");
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display, text]() {
                            display->SetChatMessage("assistant", text.c_str());
                        });
                    }
                    break;
                case 1: /* VRT_ROUTE_HOME —— 回 P1 首页常显 */
                    cJSON_AddStringToObject(ack, "route", "home");
                    {
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display]() {
                            display->SetChatMessage("system", "");
                        });
                    }
                    break;
                case 2: { /* VRT_ROUTE_CONTROL —— 契约 §1.4 动作执行 */
                    cJSON_AddStringToObject(ack, "route", "control");
                    std::string action = mcp_json_get_string(data, "action");
                    int a = kidpal_display_control_action(action.c_str());
                    cJSON_AddStringToObject(ack, "action", action.c_str());
                    switch (a) {
                        case 0: /* end_session —— 结束会话回首页 (本地停播主地位不变) */
                            app.AbortSpeaking(kAbortReasonNone);
                            break;
                        case 1: /* stop_speaking —— 云端确认停播补充 */
                            app.AbortSpeaking(kAbortReasonNone);
                            break;
                        case 2: /* start_recording —— 包 8 录音接合 (manual 聆听, stub) */
                            app.StartListening();
                            break;
                        default: /* 未知 action -> 降级, 不执行 */
                            cJSON_AddBoolToObject(ack, "degrade", true);
                            break;
                    }
                    break;
                }
                case 4: { /* VRT_ROUTE_SCHEDULE —— P2 课表页 (契约 §1.3 ②, 包
                           * 260829-schedule-p2)。解析 data {"title","items":[…]}：
                           * 成功 -> kidpal_schedule_page_render 渲染面真画 (标题
                           * 弹跳/圆点两态/行列表)；解析失败 -> 显式降级 (友好提示,
                           * 绝不盲渲染, V27 / spec §4.0)。 */
                    cJSON_AddStringToObject(ack, "route", "schedule");
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    char title[64];
                    kidpal_schedule_item_t items[16];
                    int n = kidpal_schedule_parse(data.c_str(), title, sizeof(title),
                                                  items, 16);
                    if (n < 0) {
                        /* 降级：schedule data 畸形/缺字段 -> 友好提示, 不盲渲染 */
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        ESP_LOGW(TAG, "display_dispatch view=schedule corr_id=%s items=0 degrade=true", corr_id);
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display]() {
                            display->SetChatMessage("system", "课表数据有点问题，稍后再试试");
                        });
                        break;
                    }
                    /* 明天视图判定 (P2-A4 全空心前提)：契约 data 形状零变更、无
                     * is_tomorrow 字段 —— producer 在 title 语义里携带, 设备侧按
                     * title 含「明天」做最小判定 (minimal, 升级路径=producer 显式
                     * 约定 title 词汇表, 不动 data 形状)。 */
                    int is_tomorrow = (strstr(title, "明天") != nullptr) ? 1 : 0;
                    int locate_row = -1; /* 语音问课表定位行由语音消费路径经
                                          * kidpal_schedule_locate 续接, 默认无星标 */
                    /* 设备当前 HH:MM (圆点两态判定; RTC 未同步时按 00:00 全实心降级) */
                    char now_hhmm[8] = "00:00";
                    {
                        time_t now_t = time(nullptr);
                        struct tm tm_now;
                        if (localtime_r(&now_t, &tm_now) != nullptr) {
                            snprintf(now_hhmm, sizeof(now_hhmm), "%02d:%02d",
                                     tm_now.tm_hour, tm_now.tm_min);
                        }
                    }
                    cJSON_AddNumberToObject(ack, "items", n);
                    ESP_LOGI(TAG, "display_dispatch view=schedule corr_id=%s items=%d is_tomorrow=%d degrade=false",
                             corr_id, n, is_tomorrow);
                    auto display = Board::GetInstance().GetDisplay();
                    std::string now_str = now_hhmm;
                    app.Schedule([display, title, items, n, is_tomorrow, now_str, locate_row, corr_id]() {
                        int drawn = kidpal_schedule_page_render(
                            title, items, n, is_tomorrow, now_str.c_str(), locate_row);
                        ESP_LOGI(TAG, "schedule_render corr_id=%s items=%d drawn=%d",
                                 corr_id, n, drawn);
                case 3: { /* VRT_ROUTE_CHAR —— 查生字 P4 (包 260829-char-stroke, 本包
                             additive 补 case 3, 只消费不改 view_router 数值码). */
                    cJSON_AddStringToObject(ack, "route", "char");
                    std::string char_data = data;
                    /* 缺字段防御降级门 (闸 A 包内裁定): 必需键六件不全 -> 降级提示、
                     * 不盲渲染、不崩 (对齐 view_router UNKNOWN 语义)。 */
                    if (kidpal_char_data_validate(char_data.c_str()) != 0) {
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        cJSON_AddStringToObject(ack, "degrade_reason",
                            "char data missing required keys (char/pinyin/radical/"
                            "strokeCount/strokes/medians)");
                        break;
                    }
                    /* 通过校验 -> 切 P4 页喂数据 (LVGL 主线程延迟执行) */
                    app.Schedule([char_data]() {
                        kidpal_char_page_show(char_data.c_str());
                    });
                    break;
                }
                default: /* VRT_ROUTE_UNKNOWN —— 白名单外 view 的独立降级路径 */
                    cJSON_AddStringToObject(ack, "route", "unknown");
                    cJSON_AddBoolToObject(ack, "degrade", true);
                    cJSON_AddStringToObject(ack, "unknown_view", view.c_str());
                    break;
            }
            return ack; /* cJSON* -> McpTool::Call 序列化为 result(送达确认) 回执 */
        });

#ifdef HAVE_LVGL
    auto display = board.GetDisplay();
    if (display && display->GetTheme() != nullptr) {
        AddTool("self.screen.set_theme",
            "Set the theme of the screen. The theme can be `light` or `dark`.",
            PropertyList({
                Property("theme", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto theme_name = properties["theme"].value<std::string>();
                auto& theme_manager = LvglThemeManager::GetInstance();
                auto theme = theme_manager.GetTheme(theme_name);
                if (theme != nullptr) {
                    display->SetTheme(theme);
                    return true;
                }
                return false;
            });
    }

    auto camera = board.GetCamera();
    if (camera) {
        AddTool("self.camera.take_photo",
            "Take a photo and explain it. Use this tool after the user asks you to see something.\n"
            "Args:\n"
            "  `question`: The question that you want to ask about the photo.\n"
            "Return:\n"
            "  A JSON object that provides the photo information.",
            PropertyList({
                Property("question", kPropertyTypeString)
            }),
            [camera](const PropertyList& properties) -> ReturnValue {
                // Lower the priority to do the camera capture
                TaskPriorityReset priority_reset(1);

                if (!camera->Capture()) {
                    throw std::runtime_error("Failed to capture photo");
                }
                auto question = properties["question"].value<std::string>();
                return camera->Explain(question);
            });
    }
#endif

    // Restore the original tools list to the end of the tools list
    tools_.insert(tools_.end(), original_tools.begin(), original_tools.end());
}

void McpServer::AddUserOnlyTools() {
    // System tools
    AddUserOnlyTool("self.get_system_info",
        "Get the system information",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& board = Board::GetInstance();
            return board.GetSystemInfoJson();
        });

    AddUserOnlyTool("self.reboot", "Reboot the system",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& app = Application::GetInstance();
            app.Schedule([&app]() {
                ESP_LOGW(TAG, "User requested reboot");
                vTaskDelay(pdMS_TO_TICKS(1000));

                app.Reboot();
            });
            return true;
        });

    // Firmware upgrade
    AddUserOnlyTool("self.upgrade_firmware", "Upgrade firmware from a specific URL. This will download and install the firmware, then reboot the device.",
        PropertyList({
            Property("url", kPropertyTypeString, "The URL of the firmware binary file to download and install")
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            auto url = properties["url"].value<std::string>();
            ESP_LOGI(TAG, "User requested firmware upgrade from URL: %s", url.c_str());
            
            auto& app = Application::GetInstance();
            app.Schedule([url, &app]() {
                bool success = app.UpgradeFirmware(url);
                if (!success) {
                    ESP_LOGE(TAG, "Firmware upgrade failed");
                }
            });
            
            return true;
        });

    // Display control
#ifdef HAVE_LVGL
    auto display = dynamic_cast<LvglDisplay*>(Board::GetInstance().GetDisplay());
    if (display) {
        AddUserOnlyTool("self.screen.get_info", "Information about the screen, including width, height, etc.",
            PropertyList(),
            [display](const PropertyList& properties) -> ReturnValue {
                cJSON *json = cJSON_CreateObject();
                cJSON_AddNumberToObject(json, "width", display->width());
                cJSON_AddNumberToObject(json, "height", display->height());
                if (dynamic_cast<OledDisplay*>(display)) {
                    cJSON_AddBoolToObject(json, "monochrome", true);
                } else {
                    cJSON_AddBoolToObject(json, "monochrome", false);
                }
                return json;
            });

#if CONFIG_LV_USE_SNAPSHOT
        AddUserOnlyTool("self.screen.snapshot", "Snapshot the screen and upload it to a specific URL",
            PropertyList({
                Property("url", kPropertyTypeString),
                Property("quality", kPropertyTypeInteger, 80, 1, 100)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto quality = properties["quality"].value<int>();

                std::string jpeg_data;
                if (!display->SnapshotToJpeg(jpeg_data, quality)) {
                    throw std::runtime_error("Failed to snapshot screen");
                }

                ESP_LOGI(TAG, "Upload snapshot %u bytes to %s", jpeg_data.size(), url.c_str());
                
                // 构造multipart/form-data请求体
                std::string boundary = "----ESP32_SCREEN_SNAPSHOT_BOUNDARY";
                
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);
                http->SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
                if (!http->Open("POST", url)) {
                    throw std::runtime_error("Failed to open URL: " + url);
                }
                {
                    // 文件字段头部
                    std::string file_header;
                    file_header += "--" + boundary + "\r\n";
                    file_header += "Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.jpg\"\r\n";
                    file_header += "Content-Type: image/jpeg\r\n";
                    file_header += "\r\n";
                    http->Write(file_header.c_str(), file_header.size());
                }

                // JPEG数据
                http->Write((const char*)jpeg_data.data(), jpeg_data.size());

                {
                    // multipart尾部
                    std::string multipart_footer;
                    multipart_footer += "\r\n--" + boundary + "--\r\n";
                    http->Write(multipart_footer.c_str(), multipart_footer.size());
                }
                http->Write("", 0);

                if (http->GetStatusCode() != 200) {
                    throw std::runtime_error("Unexpected status code: " + std::to_string(http->GetStatusCode()));
                }
                std::string result = http->ReadAll();
                http->Close();
                ESP_LOGI(TAG, "Snapshot screen result: %s", result.c_str());
                return true;
            });
        
        AddUserOnlyTool("self.screen.preview_image", "Preview an image on the screen",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);

                if (!http->Open("GET", url)) {
                    throw std::runtime_error("Failed to open URL: " + url);
                }
                int status_code = http->GetStatusCode();
                if (status_code != 200) {
                    throw std::runtime_error("Unexpected status code: " + std::to_string(status_code));
                }

                size_t content_length = http->GetBodyLength();
                char* data = (char*)heap_caps_malloc(content_length, MALLOC_CAP_8BIT);
                if (data == nullptr) {
                    throw std::runtime_error("Failed to allocate memory for image: " + url);
                }
                size_t total_read = 0;
                while (total_read < content_length) {
                    int ret = http->Read(data + total_read, content_length - total_read);
                    if (ret < 0) {
                        heap_caps_free(data);
                        throw std::runtime_error("Failed to download image: " + url);
                    }
                    if (ret == 0) {
                        break;
                    }
                    total_read += ret;
                }
                http->Close();

                auto image = std::make_unique<LvglAllocatedImage>(data, content_length);
                display->SetPreviewImage(std::move(image));
                return true;
            });
#endif // CONFIG_LV_USE_SNAPSHOT
    }
#endif // HAVE_LVGL

    // Assets download url
    auto& assets = Assets::GetInstance();
    if (assets.partition_valid()) {
        AddUserOnlyTool("self.assets.set_download_url", "Set the download url for the assets",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                Settings settings("assets", true);
                settings.SetString("download_url", url);
                return true;
            });
    }
}

void McpServer::AddTool(McpTool* tool) {
    // Prevent adding duplicate tools
    if (std::find_if(tools_.begin(), tools_.end(), [tool](const McpTool* t) { return t->name() == tool->name(); }) != tools_.end()) {
        ESP_LOGW(TAG, "Tool %s already added", tool->name().c_str());
        return;
    }

    ESP_LOGI(TAG, "Add tool: %s%s", tool->name().c_str(), tool->user_only() ? " [user]" : "");
    tools_.push_back(tool);
}

void McpServer::AddTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    AddTool(new McpTool(name, description, properties, callback));
}

void McpServer::AddUserOnlyTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    auto tool = new McpTool(name, description, properties, callback);
    tool->set_user_only(true);
    AddTool(tool);
}

void McpServer::ParseMessage(const std::string& message) {
    cJSON* json = cJSON_Parse(message.c_str());
    if (json == nullptr) {
        ESP_LOGE(TAG, "Failed to parse MCP message: %s", message.c_str());
        return;
    }
    ParseMessage(json);
    cJSON_Delete(json);
}

void McpServer::ParseCapabilities(const cJSON* capabilities) {
    auto vision = cJSON_GetObjectItem(capabilities, "vision");
    if (cJSON_IsObject(vision)) {
        auto url = cJSON_GetObjectItem(vision, "url");
        auto token = cJSON_GetObjectItem(vision, "token");
        if (cJSON_IsString(url)) {
            auto camera = Board::GetInstance().GetCamera();
            if (camera) {
                std::string url_str = std::string(url->valuestring);
                std::string token_str;
                if (cJSON_IsString(token)) {
                    token_str = std::string(token->valuestring);
                }
                camera->SetExplainUrl(url_str, token_str);
            }
        }
    }
}

void McpServer::ParseMessage(const cJSON* json) {
    // Check JSONRPC version
    auto version = cJSON_GetObjectItem(json, "jsonrpc");
    if (version == nullptr || !cJSON_IsString(version) || strcmp(version->valuestring, "2.0") != 0) {
        ESP_LOGE(TAG, "Invalid JSONRPC version: %s", version ? version->valuestring : "null");
        return;
    }
    
    // Check method
    auto method = cJSON_GetObjectItem(json, "method");
    if (method == nullptr || !cJSON_IsString(method)) {
        ESP_LOGE(TAG, "Missing method");
        return;
    }
    
    auto method_str = std::string(method->valuestring);
    if (method_str.find("notifications") == 0) {
        return;
    }
    
    // Check params
    auto params = cJSON_GetObjectItem(json, "params");
    if (params != nullptr && !cJSON_IsObject(params)) {
        ESP_LOGE(TAG, "Invalid params for method: %s", method_str.c_str());
        return;
    }

    auto id = cJSON_GetObjectItem(json, "id");
    if (id == nullptr || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "Invalid id for method: %s", method_str.c_str());
        return;
    }
    auto id_int = id->valueint;
    
    if (method_str == "initialize") {
        if (cJSON_IsObject(params)) {
            auto capabilities = cJSON_GetObjectItem(params, "capabilities");
            if (cJSON_IsObject(capabilities)) {
                ParseCapabilities(capabilities);
            }
        }
        auto app_desc = esp_app_get_description();
        std::string message = "{\"protocolVersion\":\"2024-11-05\",\"capabilities\":{\"tools\":{}},\"serverInfo\":{\"name\":\"" BOARD_NAME "\",\"version\":\"";
        message += app_desc->version;
        message += "\"}}";
        ReplyResult(id_int, message);
    } else if (method_str == "tools/list") {
        std::string cursor_str = "";
        bool list_user_only_tools = false;
        if (params != nullptr) {
            auto cursor = cJSON_GetObjectItem(params, "cursor");
            if (cJSON_IsString(cursor)) {
                cursor_str = std::string(cursor->valuestring);
            }
            auto with_user_tools = cJSON_GetObjectItem(params, "withUserTools");
            if (cJSON_IsBool(with_user_tools)) {
                list_user_only_tools = with_user_tools->valueint == 1;
            }
        }
        GetToolsList(id_int, cursor_str, list_user_only_tools);
    } else if (method_str == "tools/call") {
        if (!cJSON_IsObject(params)) {
            ESP_LOGE(TAG, "tools/call: Missing params");
            ReplyError(id_int, "Missing params");
            return;
        }
        auto tool_name = cJSON_GetObjectItem(params, "name");
        if (!cJSON_IsString(tool_name)) {
            ESP_LOGE(TAG, "tools/call: Missing name");
            ReplyError(id_int, "Missing name");
            return;
        }
        auto tool_arguments = cJSON_GetObjectItem(params, "arguments");
        if (tool_arguments != nullptr && !cJSON_IsObject(tool_arguments)) {
            ESP_LOGE(TAG, "tools/call: Invalid arguments");
            ReplyError(id_int, "Invalid arguments");
            return;
        }
        DoToolCall(id_int, std::string(tool_name->valuestring), tool_arguments);
    } else {
        ESP_LOGE(TAG, "Method not implemented: %s", method_str.c_str());
        ReplyError(id_int, "Method not implemented: " + method_str);
    }
}

void McpServer::ReplyResult(int id, const std::string& result) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id) + ",\"result\":";
    payload += result;
    payload += "}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::ReplyError(int id, const std::string& message) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id);
    payload += ",\"error\":{\"message\":\"";
    payload += message;
    payload += "\"}}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::GetToolsList(int id, const std::string& cursor, bool list_user_only_tools) {
    const int max_payload_size = 8000;
    std::string json = "{\"tools\":[";
    
    bool found_cursor = cursor.empty();
    auto it = tools_.begin();
    std::string next_cursor = "";
    
    while (it != tools_.end()) {
        // 如果我们还没有找到起始位置，继续搜索
        if (!found_cursor) {
            if ((*it)->name() == cursor) {
                found_cursor = true;
            } else {
                ++it;
                continue;
            }
        }

        if (!list_user_only_tools && (*it)->user_only()) {
            ++it;
            continue;
        }
        
        // 添加tool前检查大小
        std::string tool_json = (*it)->to_json() + ",";
        if (json.length() + tool_json.length() + 30 > max_payload_size) {
            // 如果添加这个tool会超出大小限制，设置next_cursor并退出循环
            next_cursor = (*it)->name();
            break;
        }
        
        json += tool_json;
        ++it;
    }
    
    if (json.back() == ',') {
        json.pop_back();
    }
    
    if (json.back() == '[' && !tools_.empty()) {
        // 如果没有添加任何tool，返回错误
        ESP_LOGE(TAG, "tools/list: Failed to add tool %s because of payload size limit", next_cursor.c_str());
        ReplyError(id, "Failed to add tool " + next_cursor + " because of payload size limit");
        return;
    }

    if (next_cursor.empty()) {
        json += "]}";
    } else {
        json += "],\"nextCursor\":\"" + next_cursor + "\"}";
    }
    
    ReplyResult(id, json);
}

void McpServer::DoToolCall(int id, const std::string& tool_name, const cJSON* tool_arguments) {
    auto tool_iter = std::find_if(tools_.begin(), tools_.end(), 
                                 [&tool_name](const McpTool* tool) { 
                                     return tool->name() == tool_name; 
                                 });
    
    if (tool_iter == tools_.end()) {
        ESP_LOGE(TAG, "tools/call: Unknown tool: %s", tool_name.c_str());
        ReplyError(id, "Unknown tool: " + tool_name);
        return;
    }

    PropertyList arguments = (*tool_iter)->properties();
    try {
        for (auto& argument : arguments) {
            bool found = false;
            if (cJSON_IsObject(tool_arguments)) {
                auto value = cJSON_GetObjectItem(tool_arguments, argument.name().c_str());
                if (argument.type() == kPropertyTypeBoolean && cJSON_IsBool(value)) {
                    argument.set_value<bool>(value->valueint == 1);
                    found = true;
                } else if (argument.type() == kPropertyTypeInteger && cJSON_IsNumber(value)) {
                    argument.set_value<int>(value->valueint);
                    found = true;
                } else if (argument.type() == kPropertyTypeString && cJSON_IsString(value)) {
                    argument.set_value<std::string>(value->valuestring);
                    found = true;
                } else if (argument.type() == kPropertyTypeString &&
                           (cJSON_IsObject(value) || cJSON_IsArray(value))) {
                    /* E1 display 载荷的嵌套 data 是 JSON 对象 (如 {"view":"text","data":
                     * {"text":"…"}})。对声明为 string 型的属性, 把对象/数组序列化后
                     * 整体传入, 让 kidpal/display 消费端自行解析 (无对象的原生小智工具
                     * 不受影响; 该分支仅对 string 型属性 + 对象/数组参数触发)。 */
                    char *dumped = cJSON_PrintUnformatted(value);
                    if (dumped != nullptr) {
                        argument.set_value<std::string>(dumped);
                        cJSON_free(dumped);
                        found = true;
                    }
                }
            }

            if (!argument.has_default_value() && !found) {
                ESP_LOGE(TAG, "tools/call: Missing valid argument: %s", argument.name().c_str());
                ReplyError(id, "Missing valid argument: " + argument.name());
                return;
            }
        }
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "tools/call: %s", e.what());
        ReplyError(id, e.what());
        return;
    }

    // Use main thread to call the tool
    auto& app = Application::GetInstance();
    app.Schedule([this, id, tool_iter, arguments = std::move(arguments)]() {
        try {
            ReplyResult(id, (*tool_iter)->Call(arguments));
        } catch (const std::exception& e) {
            ESP_LOGE(TAG, "tools/call: %s", e.what());
            ReplyError(id, e.what());
        }
    });
}
