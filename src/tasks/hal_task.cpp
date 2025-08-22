#include "hal_task.h"
#include "comm_task.h"
#include "iec104_task.h"
#include "task_params.h"
#include <Arduino.h>
#include "hal/HardwareManager.h"
#include "iec104/core/IEC104Core.h"
#include "config/ConfigManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <ArduinoJson.h>
#include "utils/LogBuffer.h"

extern HardwareManager hardwareManager;

void hal_task(void *pvParameters)
{
    QueueHandle_t measurementQueue = static_cast<QueueHandle_t>(pvParameters);
    uint8_t last_cb_status[circuitBreakerCount] = {0};

    // Cached feature flags (updated periodically by reading config file)
    bool featDI = true, featCB = true, featDO = true; // DO placeholder for future
    // Dynamic IOA maps (initialised with compile-time defaults)
    static uint16_t ioaMapDI[digitalInputCount] = {
        IOA_DI_GFD, IOA_DI_SUPPLY_STATUS, IOA_DI_REMOTE_1, IOA_DI_REMOTE_2,
        IOA_DI_REMOTE_3, IOA_DI_CADANGAN_1, IOA_DI_CADANGAN_2, IOA_DI_CADANGAN_3
    };
    static uint16_t ioaMapCBStatus[circuitBreakerCount] = {
        IOA_STATUS_CB_1, IOA_STATUS_CB_2, IOA_STATUS_CB_3
    };
    static uint16_t ioaMapCBCommand[circuitBreakerCount] = {
        IOA_COMMAND_CB_1, IOA_COMMAND_CB_2, IOA_COMMAND_CB_3
    };
    static uint16_t ioaMapDO[2] = { IOA_DO_CADANGAN_1, IOA_DO_CADANGAN_2 };
    uint32_t lastCfgCheck = 0;
    uint32_t lastDiag = 0;
    uint32_t lastUptimeLog = 0;
    static uint32_t diagCounter = 0;

    LOGF("[HAL] Task started.\n");
    while (true)
    {
        // Reload feature toggles every 5s to allow runtime changes
        if (millis() - lastCfgCheck > 5000) {
            extern ConfigManager configManager; // defined in main.cpp
            configManager.loadFeatureFlags(featDI, featCB, featDO);
            // Load dynamic IOA config
            String ioaJson = configManager.getIoaConfig();
            // Parse only needed keys (avoid realloc loops)
            JsonDocument doc; // small document; keys limited
            if (deserializeJson(doc, ioaJson) == DeserializationError::Ok) {
                auto up = [&](const char* key, uint16_t &slot){ if (doc[key].is<int>()) { int v = doc[key].as<int>(); if (v > 0 && v < 65536) slot = (uint16_t)v; } };
                up("di_gfd", ioaMapDI[0]);
                up("di_supply_status", ioaMapDI[1]);
                up("di_remote_1", ioaMapDI[2]);
                up("di_remote_2", ioaMapDI[3]);
                up("di_remote_3", ioaMapDI[4]);
                up("di_cadangan_1", ioaMapDI[5]);
                up("di_cadangan_2", ioaMapDI[6]);
                up("di_cadangan_3", ioaMapDI[7]);
                up("status_cb_1", ioaMapCBStatus[0]);
                up("status_cb_2", ioaMapCBStatus[1]);
                up("status_cb_3", ioaMapCBStatus[2]);
                up("command_cb_1", ioaMapCBCommand[0]);
                up("command_cb_2", ioaMapCBCommand[1]);
                up("command_cb_3", ioaMapCBCommand[2]);
                up("do_cadangan_1", ioaMapDO[0]);
                up("do_cadangan_2", ioaMapDO[1]);
            }
            lastCfgCheck = millis();
        }

        hardwareManager.readAllDigitalInputs();

        if (featDI) {
            // Process general digital inputs (single-point)
        for (int i = 0; i < hardwareManager.getDigitalInputCount(); ++i) {
                if (hardwareManager.hasDigitalInputChanged(i)) {
                    MeasurementData data;
            data.ioa = (i < digitalInputCount) ? ioaMapDI[i] : hardwareManager.getDigitalInputIoa(i);
                    data.value = hardwareManager.getDigitalInputValue(i);
                    data.type = M_SP_NA_1; // Single Point
                    xQueueSend(measurementQueue, &data, pdMS_TO_TICKS(10));
                }
            }
        }

        if (featCB) {
            // Process circuit breaker statuses (double-point)
        for (int i = 0; i < circuitBreakerCount; i++) {
                uint8_t current_status = hardwareManager.getCircuitBreakerStatus(circuitBreakers[i].cbNumber);
                if (current_status != last_cb_status[i]) {
                    last_cb_status[i] = current_status;
                    MeasurementData data;
            data.ioa = ioaMapCBStatus[i];
                    data.value = current_status;
                    data.type = M_DP_NA_1; // Double Point
                    xQueueSend(measurementQueue, &data, pdMS_TO_TICKS(10));
                }
            }
        }

        // Future: featDO could control periodic output refresh / operations

        // I2C health poll (cheap)
        hardwareManager.pollI2CHealth();
        // Periodic diagnostics every 5s (include I2C stats)
        if (millis() - lastDiag > 5000) {
            UBaseType_t hw = uxTaskGetStackHighWaterMark(NULL); // words remaining
            auto stats = hardwareManager.getI2CStats();
            uint32_t nowMs = millis();
            diagCounter++;
          LOGF("[HAL][T+%lu ms][D%lu] FreeStack:%lu Heap:%u Flags(DI:%d CB:%d DO:%d) I2C(err:%lu cons:%lu rec:%lu lastErr:%lus)\n",
              (unsigned long)nowMs, (unsigned long)diagCounter,
              (unsigned long)hw, (unsigned)esp_get_free_heap_size(), featDI, featCB, featDO,
              (unsigned long)stats.errorCount, (unsigned long)stats.consecutiveErrors, (unsigned long)stats.recoveryCount,
              stats.lastErrorMs/1000);
            lastDiag = nowMs;
        }
        if (millis() - lastUptimeLog > 10000) {
            LOGF("[HAL][T+%lu ms] Uptime(s): %lu\n", (unsigned long)millis(), (unsigned long)(millis()/1000));
            lastUptimeLog = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Read every 100ms
    }
}
