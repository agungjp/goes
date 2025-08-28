#include "ConfigManager.h"
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "device_config.h"
#include "ioa_config.h"
#include "iec104/iec104_config.h" // for IEC104_ASDU_ADDRESS, IEC104_K

#define DEVICE_CONFIG_PATH "/device_config.json"
#define IOA_CONFIG_PATH    "/ioa_config.json"

// Simple CRC32 (polynomial 0xEDB88320) for revision fingerprint
static uint32_t _crc32_update(uint32_t crc, uint8_t data) {
    crc = crc ^ data;
    for (int i = 0; i < 8; ++i) {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xEDB88320UL;
        else
            crc = crc >> 1;
    }
    return crc;
}

ConfigManager::ConfigManager() : _initFS(false) {}

void ConfigManager::begin() {
    if (LittleFS.begin(true)) { // true = format if mount failed
        _initFS = true;
        Serial.println("LittleFS mounted successfully.");
    } else {
        Serial.println("FATAL: Failed to mount LittleFS.");
    }
}

String ConfigManager::_readFile(const char* path) {
    if (!_initFS) return "{}";
    File file = LittleFS.open(path, "r");
    if (!file || file.isDirectory()) {
        return "{}";
    }
    String content = file.readString();
    file.close();
    return content;
}

bool ConfigManager::_writeFile(const char* path, String json) {
    if (!_initFS) return false;
    File file = LittleFS.open(path, "w");
    if (!file) {
        return false;
    }
    bool success = file.print(json) > 0;
    file.close();
    return success;
}

bool ConfigManager::saveDeviceConfig(String json) {
    // Validate JSON and prevent accidental empty overwrite
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) return false;
    // Strip sensitive keys (deprecated)
    doc.remove("wifi_ssid");
    doc.remove("wifi_password");
    doc.remove("apn_user");
    doc.remove("apn_pass");
    // Merge with existing (so fields user didn't send aren't lost)
    String existing = _readFile(DEVICE_CONFIG_PATH);
    if (existing != "{}") {
        JsonDocument cur;
        if (deserializeJson(cur, existing) == DeserializationError::Ok) {
            // Also strip from existing in case older file has them
            cur.remove("wifi_ssid");
            cur.remove("wifi_password");
            cur.remove("apn_user");
            cur.remove("apn_pass");
            for (JsonPair kv : doc.as<JsonObject>()) {
                cur[kv.key()] = kv.value();
            }
            String merged; serializeJson(cur, merged); return _writeFile(DEVICE_CONFIG_PATH, merged);
        }
    }
    return _writeFile(DEVICE_CONFIG_PATH, json);
}

String ConfigManager::getDeviceConfig() {
    // Defaults
    JsonDocument defaults;
    defaults["comm_method"] = "modem"; // modem | ethernet
#if defined(MODEM_APN)
    defaults["apn"] = MODEM_APN;
#else
    defaults["apn"] = "";
#endif
    // Removed wifi_ssid, wifi_password, apn_user, apn_pass from exposure and persistence
    // Feature toggles (runtime)
#if ENABLE_TASK_WATCHDOG
    defaults["feat_watchdog"] = 1;
#else
    defaults["feat_watchdog"] = 0;
#endif
    defaults["feat_digital_inputs"] = 1;
    defaults["feat_circuit_breakers"] = 1;
    defaults["feat_digital_outputs"] = 1;
    // Extended granular flags (persisted only if user sets via UI; defaults = 1 (enabled))
    defaults["feat_gfd"] = 1;
    defaults["feat_supply"] = 1;
    defaults["feat_remote"] = 1; // remote group (1..3)
    defaults["feat_cb1"] = 1;
    defaults["feat_cb2"] = 1;
    defaults["feat_cb3"] = 1;
    defaults["feat_temp"] = 1;
    defaults["feat_di_cadangan"] = 1; // cadangan DI group
    defaults["feat_do_cadangan"] = 1; // cadangan DO group
    defaults["feat_ethernet"] = 1; // allow ethernet comm selection in UI
    defaults["feat_rtc"] = 1; // DS3231 / timekeeping
    defaults["feat_web"] = 1; // allow web server
    defaults["feat_web_log"] = 1; // expose serial log buffer
    defaults["feat_soe"] = 1; // sequence of events enable
    defaults["soe_buffer"] = 64; // default SOE buffer length
    defaults["iec_common_addr"] = IEC104_ASDU_ADDRESS; // runtime adjustable common address
    defaults["iec_k"] = IEC104_K; // send window
    defaults["log_cap"] = 120; // default log capacity

    String content = _readFile(DEVICE_CONFIG_PATH);
    if (content != "{}") {
    JsonDocument current;
        if (deserializeJson(current, content) == DeserializationError::Ok) {
            // Merge existing into defaults (keeping existing values)
            for (JsonPair kv : current.as<JsonObject>()) {
                defaults[kv.key()] = kv.value();
            }
        }
    }
    String out;
    serializeJson(defaults, out);
    return out;
}

bool ConfigManager::saveIoaConfig(String json) {
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) return false;
    // Build sanitized object: allow zero (explicit disable) but skip negative values
    JsonDocument sanitized;
    for (JsonPair kv : doc.as<JsonObject>()) {
        if (kv.value().is<int>()) {
            int v = kv.value().as<int>();
            if (v < 0) continue; // negative not allowed
        }
        sanitized[kv.key()] = kv.value();
    }
    String existing = _readFile(IOA_CONFIG_PATH);
    if (existing != "{}") {
        JsonDocument cur;
        if (deserializeJson(cur, existing) == DeserializationError::Ok) {
            for (JsonPair kv : sanitized.as<JsonObject>()) {
                cur[kv.key()] = kv.value();
            }
            String merged; serializeJson(cur, merged); return _writeFile(IOA_CONFIG_PATH, merged);
        }
    }
    String out; serializeJson(sanitized, out); return _writeFile(IOA_CONFIG_PATH, out);
}

String ConfigManager::getIoaConfig() {
    // Build defaults for ALL IOA values
    JsonDocument defaults;
    defaults["di_gfd"] = IOA_DI_GFD;
    defaults["di_supply_status"] = IOA_DI_SUPPLY_STATUS;
    defaults["di_remote_1"] = IOA_DI_REMOTE_1;
    defaults["di_remote_2"] = IOA_DI_REMOTE_2;
    defaults["di_remote_3"] = IOA_DI_REMOTE_3;
    defaults["di_cadangan_1"] = IOA_DI_CADANGAN_1;
    defaults["di_cadangan_2"] = IOA_DI_CADANGAN_2;
    defaults["di_cadangan_3"] = IOA_DI_CADANGAN_3;
    defaults["di_door"] = IOA_DI_DOOR;
    // Analog style placeholders
    defaults["ai_temperature"] = IOA_AI_TEMPERATURE;
    defaults["ai_humidity"] = IOA_AI_HUMIDITY;
    // Circuit breaker status
    defaults["status_cb_1"] = IOA_STATUS_CB_1;
    defaults["status_cb_2"] = IOA_STATUS_CB_2;
    defaults["status_cb_3"] = IOA_STATUS_CB_3;
    // Circuit breaker commands
    defaults["command_cb_1"] = IOA_COMMAND_CB_1;
    defaults["command_cb_2"] = IOA_COMMAND_CB_2;
    defaults["command_cb_3"] = IOA_COMMAND_CB_3;
    // Digital outputs
    defaults["do_cadangan_1"] = IOA_DO_CADANGAN_1;
    defaults["do_cadangan_2"] = IOA_DO_CADANGAN_2;

    String content = _readFile(IOA_CONFIG_PATH);
    if (content != "{}") {
        JsonDocument current;
        if (deserializeJson(current, content) == DeserializationError::Ok) {
            for (JsonPair kv : current.as<JsonObject>()) {
                // Respect stored value even if zero (zero means disabled IOA per feature toggle)
                defaults[kv.key()] = kv.value();
            }
        }
    }
    String out;
    serializeJson(defaults, out);
    return out;
}

String ConfigManager::getRevision() {
    String d = _readFile(DEVICE_CONFIG_PATH);
    String i = _readFile(IOA_CONFIG_PATH);
    uint32_t crc = 0xFFFFFFFFUL;
    for (size_t idx = 0; idx < d.length(); ++idx) {
        crc = _crc32_update(crc, static_cast<uint8_t>(d[idx]));
    }
    for (size_t idx = 0; idx < i.length(); ++idx) {
        crc = _crc32_update(crc, static_cast<uint8_t>(i[idx]));
    }
    crc ^= 0xFFFFFFFFUL;
    char buf[11]; // 0x + 8 hex + null
    snprintf(buf, sizeof(buf), "0x%08lX", static_cast<unsigned long>(crc));
    return String(buf);
}

void ConfigManager::loadFeatureFlags(bool &featDI, bool &featCB, bool &featDO) {
    featDI = featCB = featDO = true;
    String content = _readFile(DEVICE_CONFIG_PATH);
    if (content == "{}") return;
    // Simple manual parse to avoid allocating DynamicJsonDocument in tight loops
    // Expect keys like "feat_digital_inputs":1 etc.
    int idx;
    if ((idx = content.indexOf("feat_digital_inputs")) >= 0) {
        int colon = content.indexOf(':', idx);
        if (colon > 0) featDI = content.substring(colon+1).toInt() != 0;
    }
    if ((idx = content.indexOf("feat_circuit_breakers")) >= 0) {
        int colon = content.indexOf(':', idx);
        if (colon > 0) featCB = content.substring(colon+1).toInt() != 0;
    }
    if ((idx = content.indexOf("feat_digital_outputs")) >= 0) {
        int colon = content.indexOf(':', idx);
        if (colon > 0) featDO = content.substring(colon+1).toInt() != 0;
    }
}

void ConfigManager::loadExtendedFeatureFlags(
        bool &featGFD,
        bool &featSupply,
        bool &featRemote,
        bool &featCB1,
        bool &featCB2,
        bool &featCB3,
        bool &featTemp,
    /* removed featBMS */
        bool &featDICadangan,
        bool &featDOCadangan,
        bool &featEthernet,
        bool &featRTC) {
    // defaults (fail-open) so missing keys do not block features
    featGFD = featSupply = featRemote = featCB1 = featCB2 = featCB3 = true;
    featTemp = featDICadangan = featDOCadangan = featEthernet = featRTC = true; // featBMS removed
    String content = _readFile(DEVICE_CONFIG_PATH);
    if (content == "{}") return;
    auto flagParse = [&](const char* key, bool &target){
        int idx = content.indexOf(key);
        if (idx < 0) return;
        int colon = content.indexOf(':', idx);
        if (colon < 0) return;
        // find next delimiter , or }
        int end = content.indexOf(',', colon+1);
        if (end < 0) end = content.indexOf('}', colon+1);
        String num = content.substring(colon+1, end);
        target = num.toInt() != 0;
    };
    flagParse("\"feat_gfd\"", featGFD);
    flagParse("\"feat_supply\"", featSupply);
    flagParse("\"feat_remote\"", featRemote);
    flagParse("\"feat_cb1\"", featCB1);
    flagParse("\"feat_cb2\"", featCB2);
    flagParse("\"feat_cb3\"", featCB3);
    flagParse("\"feat_temp\"", featTemp);
    flagParse("\"feat_di_cadangan\"", featDICadangan);
    flagParse("\"feat_do_cadangan\"", featDOCadangan);
    flagParse("\"feat_ethernet\"", featEthernet);
    flagParse("\"feat_rtc\"", featRTC);
    // New (optional) web flags not yet used in extended loader parameters, kept for future
}

String ConfigManager::getRawDeviceFile() { return _readFile(DEVICE_CONFIG_PATH); }
String ConfigManager::getRawIoaFile() { return _readFile(IOA_CONFIG_PATH); }
