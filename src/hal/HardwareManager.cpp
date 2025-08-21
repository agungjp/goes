#include "HardwareManager.h"
#include <Arduino.h>
#include <Wire.h>
#include "config/goes_config.h"

// Include all possible pin drivers
#include "PinESP32.h"


// Include platform-specific headers for watchdog, etc.
#if defined(BOARD_ESP32)
#include <esp_task_wdt.h>
#elif defined(BOARD_ATMEGA328P)
#include <avr/wdt.h>
#endif
// STM32 watchdog is more complex, requires CubeMX/HAL setup, skipping for now.

HardwareManager::HardwareManager() {
    // Use Arduino framework's built-in macros to detect the architecture
    // and instantiate the correct pin driver.
#if defined(ARDUINO_ARCH_ESP32)
    _pinDriver = new PinESP32();
#elif defined(ARDUINO_ARCH_AVR)
    _pinDriver = new PinAVR();
#else
    #error "Unsupported board architecture. Please implement a PinInterface for your board."
#endif
}

HardwareManager::~HardwareManager() {
    delete _pinDriver;
}

void HardwareManager::init() {
    Wire.begin(); // Initialize I2C bus
    // Initialize platform-specific watchdog
#if defined(BOARD_ESP32)
    esp_task_wdt_init(8, true); // 8-second timeout, enable panic reset
    esp_task_wdt_add(NULL);     // Add current task to watchdog
#elif defined(BOARD_ATMEGA328P)
    wdt_enable(WDTO_8S);
#endif

    // Setup all pins using the driver
    setupPins();
}

void HardwareManager::setupPins() {
    if (_pinDriver) _pinDriver->setupPins();
}

void HardwareManager::setPin(int pin, int value) {
    if (_pinDriver) _pinDriver->setPin(pin, value);
}

int HardwareManager::getPin(int pin) {
    if (_pinDriver) return _pinDriver->getPin(pin);
    return -1; // Return -1 if driver is not available
}

void HardwareManager::resetWatchdog() {
#if defined(BOARD_ESP32)
    esp_task_wdt_reset();
#elif defined(BOARD_ATMEGA328P)
    wdt_reset();
#endif
}

void HardwareManager::softwareReset() {
#if defined(BOARD_ESP32)
    ESP.restart();

#else
    // Fallback for unsupported boards
    // You might want to add a Serial.println("Software reset not supported on this board.");
    // or just loop indefinitely.
    while(true);
#endif
}

void HardwareManager::updateHeartbeatLED() {
    // Use the generic setPin which delegates to the correct driver
    setPin(LED_BUILTIN, millis() % 1000 < 100);
    resetWatchdog(); // Reset watchdog on every heartbeat
}

uint8_t HardwareManager::getCircuitBreakerStatus(int cbNumber) {
    if (!_pinDriver) return 0; // Unknown state if no driver

    bool open, close;
    if (cbNumber == 1) {
        open = _pinDriver->getPin(PIN_CB1_OPEN);
        close = _pinDriver->getPin(PIN_CB1_CLOSE);
    } else if (cbNumber == 2) {
        open = _pinDriver->getPin(PIN_CB2_OPEN);
        close = _pinDriver->getPin(PIN_CB2_CLOSE);
    } else {
        return 0; // Invalid CB number
    }
    return getDoublePoint(open, close);
}

void HardwareManager::operateCircuitBreaker(int cbNumber, int command) {
    if (!_pinDriver) return;

    if (cbNumber == 1) {
        if (command == 1) _pinDriver->setPin(PIN_CB1_OUT_OPEN, HIGH);
        else if (command == 2) _pinDriver->setPin(PIN_CB1_OUT_CLOSE, HIGH);
    } else if (cbNumber == 2) {
        if (command == 1) _pinDriver->setPin(PIN_CB2_OUT_OPEN, HIGH);
        else if (command == 2) _pinDriver->setPin(PIN_CB2_OUT_CLOSE, HIGH);
    }
}

uint8_t HardwareManager::getDoublePoint(uint8_t open, uint8_t close) {
  if (!open && !close) return 0; // Unknown
  else if (!open && close) return 1; // Open
  else if (open && !close) return 2; // Close
  else return 3; // Unknown
}
