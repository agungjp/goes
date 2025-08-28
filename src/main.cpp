/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v2.1.0  |
|==================================================================ok ============|
|  Copyright (C) 2024-2025 Mr. Pegagan (agungjulianperkasa@gmail.com)         |
|  All rights reserved.                                                        |
|==============================================================================|
|  This program is free software and is distributed under the terms of the     |
|  Lesser GNU General Public License. See the LICENSE file for details.        |
|==============================================================================*/

#include <Arduino.h>
#include "config/device_config.h"
#include "iec104/iec104_config.h"

#include "hal/HardwareManager.h"
#include "comm/CommEthernet.h"
#include "comm/ModemCommunicator.h"
#include "iec104/core/IEC104Core.h"
#include "iec104/transport/IEC104Communicator.h"

#include "tasks/comm_task.h"
#include "tasks/hal_task.h"
#include "tasks/iec104_task.h"
#include "tasks/task_params.h"

#include "utils/Queue.h"
#include <WiFi.h>
#include "web/WebServerManager.h"
#include "config/ConfigManager.h"
#include <ArduinoJson.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <esp_system.h>
#include <esp_heap_caps.h>
#endif
#include "utils/LogBuffer.h"

// Global variables
HardwareManager hardwareManager;
ConfigManager configManager;
WebServerManager *webServer = nullptr;

// Runtime communication selection
#include "comm/CommInterface.h"
#if defined(USE_ETHERNET)
#include "comm/CommEthernet.h"
#endif
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
#include "comm/ModemCommunicator.h"
#endif

HardwareSerial& modemSerial = Serial2; // shared modem serial if needed

IEC104Core iec104Core;
// Pointer versions to allow late binding
CommInterface* activeComm = nullptr;
IEC104Communicator* iec104Communicator = nullptr;
// Exposed for status endpoint
String g_currentCommMethod = "modem";
// Boot / reset diagnostics
#if defined(ARDUINO_ARCH_ESP32)
RTC_DATA_ATTR uint32_t g_bootCount = 0; // Persist across deep sleep / soft resets
static const char* resetReasonToStr(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON: return "POWERON";
    case ESP_RST_EXT: return "EXT";
    case ESP_RST_SW: return "SW";
    case ESP_RST_PANIC: return "PANIC";
    case ESP_RST_INT_WDT: return "INT_WDT";
    case ESP_RST_TASK_WDT: return "TASK_WDT";
    case ESP_RST_WDT: return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    case ESP_RST_SDIO: return "SDIO";
    default: return "UNKNOWN";
  }
}
String g_resetReason = "UNKNOWN";
#endif

// Helpers
static void initCommunication(const String& comm_method) {
  if (activeComm) { delete iec104Communicator; iec104Communicator = nullptr; /* don't delete activeComm if static */ }

  String method = comm_method; method.toLowerCase();
#if defined(USE_ETHERNET)
  if (method == "ethernet") {
  extern uint8_t mac[]; // from device_config
  extern IPAddress ip;
  static CommEthernet ethernetComm(mac, ip, 2404);
    activeComm = &ethernetComm;
  } else
#endif
  {
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
    static ModemCommunicator modemComm(&modemSerial);
    activeComm = &modemComm;
#else
    activeComm = nullptr;
#endif
  }
  if (activeComm) {
    iec104Communicator = new IEC104Communicator(activeComm, &iec104Core);
    activeComm->setupConnection();
  } else {
    Serial.println("ERROR: No communication interface available.");
  }
}

QueueHandle_t incomingFrameQueue;
QueueHandle_t outgoingFrameQueue;
QueueHandle_t measurementQueue;
// Task handles for stack high watermark reporting
TaskHandle_t g_commTaskHandle = nullptr;
TaskHandle_t g_iec104TaskHandle = nullptr;
TaskHandle_t g_halTaskHandle = nullptr;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("GOES MCU Application");
  // Will adjust size after loading config
  LogBuffer::init(120, 160); // temporary init

  // Initialize modem serial (missing previously) so ModemCommunicator can send AT commands
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
  // Begin Serial2 with configured pins and baud; RX, TX order follows Arduino ESP32 signature
  modemSerial.begin(MODEM_BAUD_RATE, SERIAL_8N1, UART_MODEM_RX_PIN, UART_MODEM_TX_PIN);
  Serial.printf("[Modem] Serial2 started @ %lu baud (RX=%d TX=%d)\n", (unsigned long)MODEM_BAUD_RATE, UART_MODEM_RX_PIN, UART_MODEM_TX_PIN);
#endif

#if defined(ARDUINO_ARCH_ESP32)
  g_bootCount++;
  g_resetReason = resetReasonToStr(esp_reset_reason());
  Serial.printf("[Diag] Boot Count: %lu Reset Reason: %s\n", (unsigned long)g_bootCount, g_resetReason.c_str());
#endif

  hardwareManager.begin();

  // Print revision (may depend on files loaded later, but early best effort)
  Serial.printf("[Boot] FreeHeap:%u\n", (unsigned)esp_get_free_heap_size());

  // WiFi setup (STA, fallback AP)
  WiFi.mode(WIFI_STA);
  if (strlen(WIFI_STA_SSID) > 0) {
    Serial.printf("[WiFi] Connecting to %s...\n", WIFI_STA_SSID);
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < WIFI_CONNECT_TIMEOUT_MS) {
      delay(500);
      Serial.print('.');
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n[WiFi] Connected. IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n[WiFi] Failed to connect, starting AP mode...");
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD)) {
      Serial.printf("[WiFi] AP started. SSID: %s  IP: %s\n", WIFI_AP_SSID, WiFi.softAPIP().toString().c_str());
    } else {
      Serial.println("[WiFi] Failed to start AP!");
    }
  }

  // Start config storage
  configManager.begin();
  // Check web feature flags quickly
  bool enableWeb = true, enableWebLog = true;
  {
    String dev = configManager.getDeviceConfig();
    if (dev.indexOf("feat_web") >= 0) {
      int idx = dev.indexOf("feat_web"); int colon = dev.indexOf(':', idx); if (colon>0) enableWeb = dev.substring(colon+1).toInt() != 0;
    }
    if (dev.indexOf("feat_web_log") >= 0) {
      int idx = dev.indexOf("feat_web_log"); int colon = dev.indexOf(':', idx); if (colon>0) enableWebLog = dev.substring(colon+1).toInt() != 0;
    }
  }
  LogBuffer::setEnabled(enableWebLog);
  if (enableWeb) {
    webServer = new WebServerManager(configManager);
    webServer->begin();
  } else {
    Serial.println("[Boot] Web server disabled by feat_web=0");
  }
  
  // Determine communication method from saved config
  String devCfg = configManager.getDeviceConfig();
  String comm_method = "modem";
  JsonDocument doc; // ArduinoJson v7 preferred alias
  if (deserializeJson(doc, devCfg) == DeserializationError::Ok) {
    if (doc["comm_method"].is<const char*>()) comm_method = doc["comm_method"].as<const char*>();
    if (doc["log_cap"].is<int>()) { int cap = doc["log_cap"].as<int>(); if (cap>10 && cap<1000) LogBuffer::init(cap,160); }
  }
  initCommunication(comm_method);
  g_currentCommMethod = comm_method;
  Serial.printf("[Boot] Active communication: %s\n", g_currentCommMethod.c_str());

  // Create queues
  incomingFrameQueue = xQueueCreate(10, sizeof(FrameData));
  outgoingFrameQueue = xQueueCreate(10, sizeof(FrameData));
  measurementQueue = xQueueCreate(20, sizeof(MeasurementData));
  Serial.printf("[Boot] Queues created (in:%p out:%p meas:%p)\n", incomingFrameQueue, outgoingFrameQueue, measurementQueue);

  // Create task parameters
  static CommTaskParams comm_params;
  comm_params.communicator = iec104Communicator;
  comm_params.incomingQueue = incomingFrameQueue;
  comm_params.outgoingQueue = outgoingFrameQueue;

  static Iec104TaskParams iec104_params;
  iec104_params.core = &iec104Core;
  iec104_params.hardwareManager = &hardwareManager;
  iec104_params.incomingQueue = incomingFrameQueue;
  iec104_params.outgoingQueue = outgoingFrameQueue;
  iec104_params.measurementQueue = measurementQueue;

  // Create tasks
  BaseType_t rc1 = xTaskCreatePinnedToCore(comm_task, "Comm_Task", COMM_TASK_STACK_SIZE, &comm_params, 10, &g_commTaskHandle, 0);
  Serial.printf("[Boot] Comm_Task create %s handle=%p\n", rc1==pdPASS?"OK":"FAIL", g_commTaskHandle);
  BaseType_t rc2 = xTaskCreatePinnedToCore(iec104_task, "IEC104_Task", IEC104_TASK_STACK_SIZE, &iec104_params, 8, &g_iec104TaskHandle, 1);
  Serial.printf("[Boot] IEC104_Task create %s handle=%p\n", rc2==pdPASS?"OK":"FAIL", g_iec104TaskHandle);
  BaseType_t rc3 = xTaskCreatePinnedToCore(hal_task, "HAL_Task", HAL_TASK_STACK_SIZE, measurementQueue, 5, &g_halTaskHandle, 1);
  Serial.printf("[Boot] HAL_Task create %s handle=%p\n", rc3==pdPASS?"OK":"FAIL", g_halTaskHandle);
  Serial.printf("[Boot] Setup complete. Heap:%u\n", (unsigned)esp_get_free_heap_size());
}

void loop() 
{
  // Jika watchdog task tidak digunakan, kita bisa menghapus loopTask.
  // Namun ketika ENABLE_TASK_WATCHDOG aktif, pertahankan loopTask untuk feed watchdog.
#if ENABLE_TASK_WATCHDOG
  hardwareManager.updateHeartbeatLED();
  vTaskDelay(pdMS_TO_TICKS(500));
#else
  vTaskDelete(NULL);
#endif
}