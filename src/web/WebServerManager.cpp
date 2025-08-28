#include "WebServerManager.h"
#include "web_page.h" // Contains the HTML/CSS/JS
#include "hal/HardwareManager.h"
#include <ArduinoJson.h>
#include <esp_heap_caps.h>
#include "utils/LogBuffer.h"
#include "utils/SOEBuffer.h"
#include "iec104/core/IEC104Core.h" // for extern iec104Core usage

WebServerManager::WebServerManager(ConfigManager& configManager)
    : _server(80), _configManager(configManager) {}

void WebServerManager::begin() {
    _setupRoutes();
    _server.begin();
    Serial.println("Web server started.");
}

void WebServerManager::_setupRoutes() {
    // CORS headers for all requests
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET,HEAD,PUT,PATCH,POST,DELETE");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type,Authorization");

    // Handle CORS preflight requests for all API endpoints
    _server.on("/api/config", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/config2", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/config/raw", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/status", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/restart", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/logs", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/logs/resize", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/logs/clear", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/soe", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });
    _server.on("/api/soe/clear", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        request->send(200);
    });

    // Route for the main web page
        _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
            if (!_checkAuth(request)) return; // will handle auth challenge
            request->send(200, "text/html", CONFIG_PAGE);
    });

        // New route for monitoring page
        _server.on("/monitor", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { request->send_P(200, "text/html", MONITOR_PAGE); });
    // Handle favicon.ico requests
    _server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(404); // Return 404 for favicon
    });

    // Handle form submission fallback (if JavaScript fails)
    _server.on("/", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        Serial.println("[API] POST / - Form submission fallback");
        // Redirect back to main page - JavaScript should handle the form
        request->send(400, "text/html", 
            "<script>alert('Form submission failed - please ensure JavaScript is enabled'); history.back();</script>");
    });

    // Route to get current configurations
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        Serial.println("[API] GET /api/config");
        JsonDocument doc;
        // Sanitize sensitive fields before sending
        String devStr = _configManager.getDeviceConfig();
        if (devStr.length()) {
            JsonDocument tmp;
            if (deserializeJson(tmp, devStr) == DeserializationError::Ok) {
                tmp.remove("wifi_ssid");
                tmp.remove("wifi_password");
                tmp.remove("apn_user");
                tmp.remove("apn_pass");
                String sanitized; serializeJson(tmp, sanitized);
                doc["device"] = serialized(sanitized);
            } else {
                doc["device"] = serialized(devStr);
            }
        } else {
            doc["device"] = serialized(devStr);
        }
        doc["ioa"] = serialized(_configManager.getIoaConfig());
        doc["rev"] = _configManager.getRevision();
        
        String response;
        serializeJson(doc, response);
        Serial.println("[API] Config response length: " + String(response.length()));
        request->send(200, "application/json", response);
    });

    // New endpoint: direct nested JSON objects (no double serialization), more robust for UI
    _server.on("/api/config2", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        Serial.println("[API] GET /api/config2");
        String devStr = _configManager.getDeviceConfig();
        String ioaStr = _configManager.getIoaConfig();
        JsonDocument docDevice; JsonDocument docIoa; JsonDocument root;
        if (deserializeJson(docDevice, devStr) != DeserializationError::Ok) {}
        if (deserializeJson(docIoa, ioaStr) != DeserializationError::Ok) {}
    // Remove sensitive keys (not exposed in UI) for security
    docDevice.remove("wifi_ssid");
    docDevice.remove("wifi_password");
    docDevice.remove("apn_user");
    docDevice.remove("apn_pass");
        root["device"] = docDevice;
        root["ioa"] = docIoa;
        root["rev"] = _configManager.getRevision();
        String response; serializeJson(root, response);
        request->send(200, "application/json", response);
    });

    // Raw (unmerged) files for debugging persistence (not for production exposure)
    _server.on("/api/config/raw", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        JsonDocument doc;
        doc["device_raw"] = serialized(_configManager.getRawDeviceFile());
        doc["ioa_raw"] = serialized(_configManager.getRawIoaFile());
        String response; serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // Lightweight status (for polling)
    extern HardwareManager hardwareManager; // defined in main.cpp
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        Serial.println("[API] GET /api/status");
        extern String g_currentCommMethod; // declared in main.cpp
        extern uint32_t g_bootCount;       // in main.cpp (RTC_DATA_ATTR)
        extern String g_resetReason;       // in main.cpp
    extern TaskHandle_t g_commTaskHandle; extern TaskHandle_t g_iec104TaskHandle; extern TaskHandle_t g_halTaskHandle;
    // Placeholder IEC104 link status & heartbeat (should be set by communicator/core in future)
    static uint32_t lastHeartbeatMs = millis(); // TODO: update from real events
        auto stats = hardwareManager.getI2CStats();
        JsonDocument doc;
        doc["uptime"] = (uint32_t)(millis() / 1000);
        doc["comm_method"] = g_currentCommMethod;
        doc["rev"] = _configManager.getRevision();
        doc["boot_count"] = g_bootCount;
        doc["reset_reason"] = g_resetReason;
        doc["heap_free"] = (uint32_t)esp_get_free_heap_size();
        // Add RTU time (current system time) and source (placeholder) for new UI clock bar
        time_t now = time(nullptr);
        struct tm tinfo; localtime_r(&now, &tinfo);
        char buf[32]; snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03lu", 
            tinfo.tm_year+1900, tinfo.tm_mon+1, tinfo.tm_mday, tinfo.tm_hour, tinfo.tm_min, tinfo.tm_sec, (unsigned long)(millis()%1000));
        doc["rtu_time"] = buf;
        doc["rtu_time_source"] = "rtc"; // TODO: set based on actual sync source (ntp/iec104)
        // Only HAL task stack currently tracked via periodic log; approximate high water mark reading here
        UBaseType_t hw = uxTaskGetStackHighWaterMark(NULL); // main (loop) task context
        doc["main_task_stack_free_words"] = (uint32_t)hw;
    if (g_commTaskHandle) doc["comm_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_commTaskHandle);
    if (g_iec104TaskHandle) doc["iec104_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_iec104TaskHandle);
    if (g_halTaskHandle) doc["hal_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_halTaskHandle);
    doc["iec104_connected"] = true; // TODO: dynamic
    doc["heartbeat_ms"] = (uint32_t)(millis() - lastHeartbeatMs);
    // Deprecated createNestedObject replaced with doc["i2c"].to<JsonObject>()
    JsonObject i2c = doc["i2c"].to<JsonObject>();
        i2c["errors"] = stats.errorCount;
        i2c["consecutive"] = stats.consecutiveErrors;
        i2c["recoveries"] = stats.recoveryCount;
        i2c["last_error_s"] = stats.lastErrorMs/1000;
        i2c["last_recovery_s"] = stats.lastRecoveryMs/1000;
        String response; serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // Recent logs
    _server.on("/api/logs", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        String json = LogBuffer::toJSON();
        request->send(200, "application/json", json);
    });
    _server.on("/api/logs/clear", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        LogBuffer::init(120,160); // re-init default
        request->send(200, "text/plain", "Logs cleared.");
    });
    _server.on("/api/logs/resize", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        if (!request->hasParam("cap", true)) { request->send(400, "text/plain", "Missing cap"); return; }
        int cap = request->getParam("cap", true)->value().toInt();
        if (cap < 10 || cap > 1000) { request->send(400, "text/plain", "Invalid cap range"); return; }
        LogBuffer::init(cap,160);
        request->send(200, "text/plain", "Log buffer resized.");
    });

    // SOE endpoints
    _server.on("/api/soe", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        request->send(200, "application/json", SOEBuffer::toJSON());
    });
    _server.on("/api/soe/clear", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        SOEBuffer::clear();
        request->send(200, "text/plain", "SOE cleared.");
    });

    // Manual restart endpoint
    _server.on("/api/restart", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (!_checkAuth(request)) return;
        Serial.println("[API] POST /api/restart - Restart requested via web API");
        request->send(200, "text/plain", "Restarting...");
        delay(300);
        ESP.restart();
    });

    // Route to save device config
    _server.on("/api/config/device", HTTP_POST, [this](AsyncWebServerRequest *request){ if (!_checkAuth(request)) return; },
        NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        
        String body = "";
        for(size_t i=0; i<len; i++){
          body += (char)data[i];
        }

        // Purge any sensitive fields if they arrive (we no longer store them)
        if (body.length()) {
            JsonDocument incoming;
            if (deserializeJson(incoming, body) == DeserializationError::Ok) {
                incoming.remove("wifi_ssid");
                incoming.remove("wifi_password");
                incoming.remove("apn_user");
                incoming.remove("apn_pass");
                String sanitized; serializeJson(incoming, sanitized);
                body = sanitized;
            }
        }

        // Determine if comm_method changed to force restart
        String current = _configManager.getDeviceConfig();
        String oldMethod = ""; String newMethod = "";
        {
            JsonDocument docOld; deserializeJson(docOld, current);
            if (docOld["comm_method"].is<const char*>()) oldMethod = docOld["comm_method"].as<const char*>();
        }
        {
            JsonDocument docNew; deserializeJson(docNew, body);
            if (docNew["comm_method"].is<const char*>()) newMethod = docNew["comm_method"].as<const char*>();
        }

    bool methodChanged = (oldMethod != "" && newMethod != "" && oldMethod != newMethod);
    bool saved = _configManager.saveDeviceConfig(body);
        // After saving, adjust dynamic runtime flags (web log capture) without restart
        if (saved) {
            String merged = _configManager.getDeviceConfig();
            if (merged.indexOf("feat_web_log") >= 0) {
                int idx2 = merged.indexOf("feat_web_log"); int colon2 = merged.indexOf(':', idx2);
                if (colon2 > 0) {
                    bool enableLog = merged.substring(colon2+1).toInt() != 0;
                    LogBuffer::setEnabled(enableLog);
                }
            }
        }
        if (saved) {
            if (methodChanged) {
                request->send(200, "text/plain", "Device config saved. Restarting to apply comm method...");
                delay(500);
                ESP.restart();
            } else {
                // Apply runtime adjustable params (iec_common_addr, iec_k, log_cap)
                String merged = _configManager.getDeviceConfig();
                JsonDocument applyDoc; if (deserializeJson(applyDoc, merged) == DeserializationError::Ok) {
                    if (applyDoc["iec_common_addr"].is<int>()) {
                        extern IEC104Core iec104Core; iec104Core.setCommonAddress((uint16_t)applyDoc["iec_common_addr"].as<int>());
                    }
                    if (applyDoc["iec_k"].is<int>()) {
                        // currently unused for flow control in simplified stack
                    }
                    if (applyDoc["log_cap"].is<int>()) {
                        int cap = applyDoc["log_cap"].as<int>(); if (cap>10 && cap<1000) LogBuffer::init(cap,160);
                    }
                }
                request->send(200, "text/plain", "Device config saved.");
            }
        } else {
            request->send(500, "text/plain", "Failed to save device config.");
        }
    });

    // Route to save IOA config
    _server.on("/api/config/ioa", HTTP_POST, [this](AsyncWebServerRequest *request){ if (!_checkAuth(request)) return; },
        NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        
        String body = "";
        for(size_t i=0; i<len; i++){
          body += (char)data[i];
        }

        if (_configManager.saveIoaConfig(body)) {
            request->send(200, "text/plain", "IOA config saved.");
        } else {
            request->send(500, "text/plain", "Failed to save IOA config.");
        }
    });
}

// Very lightweight Basic Auth (no HTTPS). Default creds hardcoded.
static const char* DEFAULT_USER = "admin";
static const char* DEFAULT_PASS = "admin123";

bool WebServerManager::_checkAuth(AsyncWebServerRequest *request) {
    if (!request) return false;
    if (request->hasHeader("Authorization")) {
        const AsyncWebHeader* h = request->getHeader("Authorization");
        String v = h->value();
        if (v.startsWith("Basic ")) {
            String b64 = v.substring(6);
            // decode base64 manually (simple, limited) or rely on Arduino built-in? We'll do simple
            // For brevity, accept only expected string
            String expected = String(DEFAULT_USER) + ":" + DEFAULT_PASS;
            // Quick encode expected (naive) to compare (avoid bringing full base64 lib)
            // Precomputed base64 for default pair: admin:admin123 => YWRtaW46YWRtaW4xMjM=
            if ((b64 == "YWRtaW46YWRtaW4xMjM=") || (b64.length()<5 && false)) {
                return true;
            }
        }
    }
    auto *resp = request->beginResponse(401, "text/plain", "Auth Required");
    resp->addHeader("WWW-Authenticate", "Basic realm=GOES");
    request->send(resp);
    return false;
}
