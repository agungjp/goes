#include "HardwareManager.h"
#include <Arduino.h>
#include <Wire.h>
#include "goes_config.h"

// Include all possible pin drivers
#include "PinESP32.h"
#include "PinAVR.h"

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
#elif defined(BOARD_ATMEGA328P)
    // For AVR, a software reset can be triggered by enabling and then immediately resetting the watchdog
    wdt_enable(WDTO_15MS);
    while(1); // Wait for watchdog to reset
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
