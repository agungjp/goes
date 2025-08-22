#include "HardwareManager.h"
#include "config/device_config.h" // File konfigurasi terpusat
#include "config/device_config.h"
#include "PinESP32.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// Library untuk PCF8574T (xreef)
#include <PCF8574.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Include platform-specific headers for watchdog
#if defined(BOARD_ESP32)
#include <esp_task_wdt.h>
#endif

// Membuat instance untuk setiap chip PCF8574T
PCF8574 pcfInputsGeneral(PCF_INPUTS_GENERAL_ADDR);
PCF8574 pcfStatusCB(PCF_STATUS_CB_ADDR);
PCF8574 pcfOutputsCmd(PCF_OUTPUTS_CMD_ADDR);

// Global mutex to guard I2C (Wire) access (PCF8574 library is not thread-safe across cores)
static SemaphoreHandle_t g_i2cMutex = nullptr;
static inline bool I2C_LOCK(TickType_t to = pdMS_TO_TICKS(20)) {
    if (!g_i2cMutex) return false; // not yet created
    return xSemaphoreTake(g_i2cMutex, to) == pdTRUE;
}
static inline void I2C_UNLOCK() {
    if (g_i2cMutex) xSemaphoreGive(g_i2cMutex);
}

// Forward declare internal helpers
static bool pcfBeginRetry(PCF8574 &pcf, int attempts, uint32_t &errCounter);
static int pcfDigitalReadRetry(PCF8574 &pcf, uint8_t pin, int attempts, uint32_t &errCounter);
static void pcfDigitalWriteRetry(PCF8574 &pcf, uint8_t pin, uint8_t val, int attempts, uint32_t &errCounter);

HardwareManager::HardwareManager() {
    // Use Arduino framework's built-in macros to detect the architecture
    // and instantiate the correct pin driver.
#if defined(ARDUINO_ARCH_ESP32)
    _pinDriver = new PinESP32();
#else
    #error "Unsupported board architecture."
#endif
}

HardwareManager::~HardwareManager() {
    delete _pinDriver;
}

void HardwareManager::init() {
    // Inisialisasi bus komunikasi utama
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
#if !defined(SIMULATE_NO_PERIPHERALS)
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
#endif

    // Inisialisasi pin-pin yang terhubung langsung ke ESP32
    if (_pinDriver) {
        _pinDriver->setupPins();
    }

    // Inisialisasi GPIO Expanders (skip bila simulasi)
#if !defined(SIMULATE_NO_PERIPHERALS)
    if (!g_i2cMutex) {
        g_i2cMutex = xSemaphoreCreateMutex();
    }
    if (g_i2cMutex) {
        if (I2C_LOCK()) { pcfBeginRetry(pcfInputsGeneral, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
        if (I2C_LOCK()) { pcfBeginRetry(pcfStatusCB, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
        if (I2C_LOCK()) { pcfBeginRetry(pcfOutputsCmd, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
    } else {
        // Fallback attempt without mutex (single-core assumption)
        pcfBeginRetry(pcfInputsGeneral, 3, _i2cStats.errorCount);
        pcfBeginRetry(pcfStatusCB, 3, _i2cStats.errorCount);
        pcfBeginRetry(pcfOutputsCmd, 3, _i2cStats.errorCount);
    }
#endif

    // Set pin mode hanya bila tidak simulasi
#if !defined(SIMULATE_NO_PERIPHERALS)
    for (int i = 0; i < digitalInputCount; ++i) {
        if (digitalInputs[i].expanderAddress == PCF_INPUTS_GENERAL_ADDR) {
            if (I2C_LOCK()) { pcfInputsGeneral.pinMode(digitalInputs[i].expanderPin, INPUT); I2C_UNLOCK(); }
        } else if (digitalInputs[i].expanderAddress == PCF_STATUS_CB_ADDR) {
            if (I2C_LOCK()) { pcfStatusCB.pinMode(digitalInputs[i].expanderPin, INPUT); I2C_UNLOCK(); }
        }
    }
    for (int i = 0; i < circuitBreakerCount; ++i) {
        if (circuitBreakers[i].open_input.expanderAddress == PCF_STATUS_CB_ADDR)
            if (I2C_LOCK()) { pcfStatusCB.pinMode(circuitBreakers[i].open_input.expanderPin, INPUT); I2C_UNLOCK(); }
        if (circuitBreakers[i].close_input.expanderAddress == PCF_STATUS_CB_ADDR)
            if (I2C_LOCK()) { pcfStatusCB.pinMode(circuitBreakers[i].close_input.expanderPin, INPUT); I2C_UNLOCK(); }
        if (circuitBreakers[i].open_command.expanderAddress == PCF_OUTPUTS_CMD_ADDR)
            if (I2C_LOCK()) { pcfOutputsCmd.pinMode(circuitBreakers[i].open_command.expanderPin, OUTPUT); I2C_UNLOCK(); }
        if (circuitBreakers[i].close_command.expanderAddress == PCF_OUTPUTS_CMD_ADDR)
            if (I2C_LOCK()) { pcfOutputsCmd.pinMode(circuitBreakers[i].close_command.expanderPin, OUTPUT); I2C_UNLOCK(); }
    }
    for (int i = 0; i < digitalOutputCount; ++i) {
        if (digitalOutputs[i].expanderAddress == PCF_OUTPUTS_CMD_ADDR) {
            if (I2C_LOCK()) { pcfOutputsCmd.pinMode(digitalOutputs[i].expanderPin, OUTPUT); I2C_UNLOCK(); }
        }
    }
#endif

    // Inisialisasi watchdog hanya jika diaktifkan
#if defined(BOARD_ESP32) && ENABLE_TASK_WATCHDOG
    esp_task_wdt_init(8, true); // 8-second timeout, enable panic reset
    esp_task_wdt_add(NULL);     // Tambahkan current task (biasanya loopTask) ke watchdog
#endif
}

// Fungsi ini sekarang tidak lagi relevan karena setup pin dilakukan di init()
// dan di dalam kelas PinESP32.
/*
void HardwareManager::setupPins() {
    // Deprecated
}
*/

void HardwareManager::setPin(int pin, int value) {
    if (_pinDriver) _pinDriver->setPin(pin, value);
}

int HardwareManager::getPin(int pin) {
    if (_pinDriver) return _pinDriver->getPin(pin);
    return -1;
}

void HardwareManager::resetWatchdog() {
#if defined(BOARD_ESP32) && ENABLE_TASK_WATCHDOG
    esp_task_wdt_reset();
#endif
}

void HardwareManager::softwareReset() {
#if defined(BOARD_ESP32)
    ESP.restart();
#endif
}

void HardwareManager::updateHeartbeatLED() {
    // Menggunakan pin yang didefinisikan di device_config.h
    setPin(LED_HEARTBEAT_PIN, millis() % 1000 < 100);
    resetWatchdog();
}

// --- Digital Input & Output Methods using PCF8574T ---

// Struct internal untuk melacak status input dari expander
struct ExpanderInputState {
    uint8_t expanderAddress;
    uint8_t expanderPin;
    int lastValue;
    int currentValue;
};

// Array untuk melacak status semua input digital dari expander
static ExpanderInputState expanderInputs[digitalInputCount + circuitBreakerCount * 2]; // Tambahkan ruang untuk status CB

void HardwareManager::begin() {
    init();
    // Inisialisasi state tracker untuk DI umum
    for (int i = 0; i < digitalInputCount; ++i) {
        expanderInputs[i].expanderAddress = digitalInputs[i].expanderAddress;
        expanderInputs[i].expanderPin = digitalInputs[i].expanderPin;
        expanderInputs[i].lastValue = -1;
        expanderInputs[i].currentValue = -1;
    }
    // Inisialisasi state tracker untuk status CB
    for (int i = 0; i < circuitBreakerCount; ++i) {
        int baseIndex = digitalInputCount + i * 2;
        expanderInputs[baseIndex].expanderAddress = circuitBreakers[i].open_input.expanderAddress;
        expanderInputs[baseIndex].expanderPin = circuitBreakers[i].open_input.expanderPin;
        expanderInputs[baseIndex].lastValue = -1;
        expanderInputs[baseIndex].currentValue = -1;

        expanderInputs[baseIndex + 1].expanderAddress = circuitBreakers[i].close_input.expanderAddress;
        expanderInputs[baseIndex + 1].expanderPin = circuitBreakers[i].close_input.expanderPin;
        expanderInputs[baseIndex + 1].lastValue = -1;
        expanderInputs[baseIndex + 1].currentValue = -1;
    }
}

void HardwareManager::readAllDigitalInputs() {
#if defined(SIMULATE_NO_PERIPHERALS)
    // Simulasi: toggle pola sederhana setiap panggilan (~100ms) agar ada event
    static uint32_t counter = 0; counter++;
    for (int i = 0; i < (digitalInputCount + circuitBreakerCount * 2); ++i) {
        expanderInputs[i].lastValue = expanderInputs[i].currentValue;
        // pola: bit parity + index
        int simulated = ((counter / 5) + i) & 0x1; // berubah tiap ~500ms
        expanderInputs[i].currentValue = simulated;
    }
#else
    for (int i = 0; i < (digitalInputCount + circuitBreakerCount * 2); ++i) {
        expanderInputs[i].lastValue = expanderInputs[i].currentValue;
        if (expanderInputs[i].expanderAddress == PCF_INPUTS_GENERAL_ADDR) {
            if (I2C_LOCK()) { expanderInputs[i].currentValue = pcfDigitalReadRetry(pcfInputsGeneral, expanderInputs[i].expanderPin, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
        } else if (expanderInputs[i].expanderAddress == PCF_STATUS_CB_ADDR) {
            if (I2C_LOCK()) { expanderInputs[i].currentValue = pcfDigitalReadRetry(pcfStatusCB, expanderInputs[i].expanderPin, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
        }
    }
#endif
}

bool HardwareManager::hasDigitalInputChanged(int index) {
    if (index < 0 || index >= digitalInputCount) return false;
    // Perubahan pertama kali selalu dilaporkan
    if (expanderInputs[index].lastValue == -1) return true;
    return expanderInputs[index].currentValue != expanderInputs[index].lastValue;
}

int HardwareManager::getDigitalInputValue(int index) {
    if (index < 0 || index >= digitalInputCount) return -1;
    return expanderInputs[index].currentValue;
}

uint16_t HardwareManager::getDigitalInputIoa(int index) {
    if (index < 0 || index >= digitalInputCount) return 0;
    return digitalInputs[index].ioa;
}

int HardwareManager::getDigitalInputCount() {
    return digitalInputCount;
}

void HardwareManager::setDigitalOutput(uint16_t ioa, bool value) {
    for (int i = 0; i < digitalOutputCount; ++i) {
        if (digitalOutputs[i].ioa == ioa) {
            // Hanya ada satu expander untuk output di desain ini
            if (digitalOutputs[i].expanderAddress == PCF_OUTPUTS_CMD_ADDR) {
                if (I2C_LOCK()) { pcfDigitalWriteRetry(pcfOutputsCmd, digitalOutputs[i].expanderPin, value ? HIGH : LOW, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
            }
            return;
        }
    }
}

// --- Circuit Breaker Specific Methods ---

uint8_t HardwareManager::getCircuitBreakerStatus(int cbNumber) {
    for (int i = 0; i < circuitBreakerCount; ++i) {
        if (circuitBreakers[i].cbNumber == cbNumber) {
            int open_val = -1;
            int close_val = -1;

            // Cari nilai dari state tracker
            int baseIndex = digitalInputCount + i * 2;
            open_val = expanderInputs[baseIndex].currentValue;
            close_val = expanderInputs[baseIndex + 1].currentValue;

            if (open_val == -1 || close_val == -1) {
                return 0; // Intermediate state during initialization
            }

            return getDoublePoint(open_val, close_val);
        }
    }
    return 3; // Not found or error
}

void HardwareManager::operateCircuitBreaker(int cbNumber, int command) {
    // command: 1 = OPEN, 2 = CLOSE
    for (int i = 0; i < circuitBreakerCount; ++i) {
        if (circuitBreakers[i].cbNumber == cbNumber) {
            const auto& cb = circuitBreakers[i];
            if (command == 1) { // OPEN command
                if (I2C_LOCK()) { pcfDigitalWriteRetry(pcfOutputsCmd, cb.open_command.expanderPin, HIGH, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
                delay(200); // Pulse duration
                if (I2C_LOCK()) { pcfDigitalWriteRetry(pcfOutputsCmd, cb.open_command.expanderPin, LOW, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
            } else if (command == 2) { // CLOSE command
                if (I2C_LOCK()) { pcfDigitalWriteRetry(pcfOutputsCmd, cb.close_command.expanderPin, HIGH, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
                delay(200); // Pulse duration
                if (I2C_LOCK()) { pcfDigitalWriteRetry(pcfOutputsCmd, cb.close_command.expanderPin, LOW, 3, _i2cStats.errorCount); I2C_UNLOCK(); }
            }
            return;
        }
    }
}

uint8_t HardwareManager::getDoublePoint(uint8_t open, uint8_t close) {
  // Assuming active-low inputs (0 = active)
  if (open == 0 && close != 0) return 2; // Close (CB is closed, so 'open' contact is active)
  else if (open != 0 && close == 0) return 1; // Open (CB is open, so 'close' contact is active)
  else if (open == 0 && close == 0) return 3; // Indeterminate/Error
  else return 0; // Intermediate/Transition
}

// ---------------- I2C Health & Retry Helpers ----------------

static bool pcfBeginRetry(PCF8574 &pcf, int attempts, uint32_t &errCounter) {
    for (int i=0;i<attempts;i++) {
        if (pcf.begin()) return true;
        errCounter++;
        delay(5);
    }
    return false;
}

static int pcfDigitalReadRetry(PCF8574 &pcf, uint8_t pin, int attempts, uint32_t &errCounter) {
    int val = HIGH;
    for (int i=0;i<attempts;i++) {
        val = pcf.digitalRead(pin);
        // Library returns 0/1; assume both are valid; treat other values as error if ever occurs
        if (val==0 || val==1) return val;
        errCounter++;
        delay(2);
    }
    return val; // last attempt
}

static void pcfDigitalWriteRetry(PCF8574 &pcf, uint8_t pin, uint8_t val, int attempts, uint32_t &errCounter) {
    for (int i=0;i<attempts;i++) {
        pcf.digitalWrite(pin, val);
        // No direct error feedback; optionally could read back
        break; // single attempt sufficient unless library adds error codes
    }
}

void HardwareManager::pollI2CHealth() {
#if defined(SIMULATE_NO_PERIPHERALS)
    // Dalam mode simulasi, lewati logika health check agar tidak ada noise
    return;
#endif
    uint32_t now = millis();
    if (now - _lastHealthCheckMs < 2000) return; // every 2s
    _lastHealthCheckMs = now;

    // Heuristic: if errorCount advanced since last check and consecutiveErrors high, attempt recovery
    static uint32_t prevErrorCount = 0;
    if (_i2cStats.errorCount > prevErrorCount) {
        _i2cStats.lastErrorMs = now;
        _i2cStats.consecutiveErrors += (_i2cStats.errorCount - prevErrorCount);
    } else {
        // decay consecutive errors
        if (_i2cStats.consecutiveErrors>0) _i2cStats.consecutiveErrors /= 2;
    }
    prevErrorCount = _i2cStats.errorCount;

    const uint32_t RECOVERY_THRESHOLD = 25; // tune as needed
    if (_i2cStats.consecutiveErrors >= RECOVERY_THRESHOLD) {
        Serial.println("[I2C] Recovering bus...");
        // Attempt to re-init bus and expanders
        if (I2C_LOCK()) {
            Wire.end();
            delay(10);
            Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
            pcfBeginRetry(pcfInputsGeneral, 5, _i2cStats.errorCount);
            pcfBeginRetry(pcfStatusCB, 5, _i2cStats.errorCount);
            pcfBeginRetry(pcfOutputsCmd, 5, _i2cStats.errorCount);
            I2C_UNLOCK();
        }
        _i2cStats.recoveryCount++;
        _i2cStats.lastRecoveryMs = now;
        _i2cStats.consecutiveErrors = 0;
    }
}

HardwareManager::I2CStats HardwareManager::getI2CStats() {
    return _i2cStats; // return by value (small struct)
}
