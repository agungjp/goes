#include "WebServerManager.h"
#include "web_page.h" // Contains the HTML/CSS/JS
#include "hal/HardwareManager.h"
#include <ArduinoJson.h>
#include <esp_heap_caps.h>
#include "utils/LogBuffer.h"

WebServerManager::WebServerManager(ConfigManager& configManager)
    : _server(80), _configManager(configManager) {}

void WebServerManager::begin() {
    _setupRoutes();
    _server.begin();
    Serial.println("Web server started.");
}

void WebServerManager::_setupRoutes() {
    // Route for the main web page
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", WEB_PAGE);
    });

    // Route to get current configurations
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest *request){
        JsonDocument doc;
        doc["device"] = serialized(_configManager.getDeviceConfig());
        doc["ioa"] = serialized(_configManager.getIoaConfig());
        doc["rev"] = _configManager.getRevision();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // New endpoint: direct nested JSON objects (no double serialization), more robust for UI
    _server.on("/api/config2", HTTP_GET, [this](AsyncWebServerRequest *request){
        String devStr = _configManager.getDeviceConfig();
        String ioaStr = _configManager.getIoaConfig();
        JsonDocument docDevice; JsonDocument docIoa; JsonDocument root;
        if (deserializeJson(docDevice, devStr) != DeserializationError::Ok) {}
        if (deserializeJson(docIoa, ioaStr) != DeserializationError::Ok) {}
        root["device"] = docDevice;
        root["ioa"] = docIoa;
        root["rev"] = _configManager.getRevision();
        String response; serializeJson(root, response);
        request->send(200, "application/json", response);
    });

    // Raw (unmerged) files for debugging persistence (not for production exposure)
    _server.on("/api/config/raw", HTTP_GET, [this](AsyncWebServerRequest *request){
        JsonDocument doc;
        doc["device_raw"] = serialized(_configManager.getRawDeviceFile());
        doc["ioa_raw"] = serialized(_configManager.getRawIoaFile());
        String response; serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // Lightweight status (for polling)
    extern HardwareManager hardwareManager; // defined in main.cpp
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        extern String g_currentCommMethod; // declared in main.cpp
        extern uint32_t g_bootCount;       // in main.cpp (RTC_DATA_ATTR)
        extern String g_resetReason;       // in main.cpp
    extern TaskHandle_t g_commTaskHandle; extern TaskHandle_t g_iec104TaskHandle; extern TaskHandle_t g_halTaskHandle;
        auto stats = hardwareManager.getI2CStats();
        JsonDocument doc;
        doc["uptime"] = (uint32_t)(millis() / 1000);
        doc["comm_method"] = g_currentCommMethod;
        doc["rev"] = _configManager.getRevision();
        doc["boot_count"] = g_bootCount;
        doc["reset_reason"] = g_resetReason;
        doc["heap_free"] = (uint32_t)esp_get_free_heap_size();
        // Only HAL task stack currently tracked via periodic log; approximate high water mark reading here
        UBaseType_t hw = uxTaskGetStackHighWaterMark(NULL); // main (loop) task context
        doc["main_task_stack_free_words"] = (uint32_t)hw;
    if (g_commTaskHandle) doc["comm_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_commTaskHandle);
    if (g_iec104TaskHandle) doc["iec104_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_iec104TaskHandle);
    if (g_halTaskHandle) doc["hal_task_stack_free_words"] = (uint32_t)uxTaskGetStackHighWaterMark(g_halTaskHandle);
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
    _server.on("/api/logs", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = LogBuffer::toJSON();
        request->send(200, "application/json", json);
    });

    // Route to save device config
    _server.on("/api/config/device", HTTP_POST, [](AsyncWebServerRequest *request){},
        NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        
        String body = "";
        for(size_t i=0; i<len; i++){
          body += (char)data[i];
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
                request->send(200, "text/plain", "Device config saved.");
            }
        } else {
            request->send(500, "text/plain", "Failed to save device config.");
        }
    });

    // Route to save IOA config
    _server.on("/api/config/ioa", HTTP_POST, [](AsyncWebServerRequest *request){},
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
