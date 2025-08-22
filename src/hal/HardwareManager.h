#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "PinInterface.h"
#include <stdint.h>

class HardwareManager {
public:
    HardwareManager();
    ~HardwareManager();

    void init();
    void begin(); // Alias for init, for consistency

    // --- Direct ESP32 Pin Control ---
    void setPin(int pin, int value);
    int getPin(int pin);

    // --- Expander-based Digital I/O ---
    void readAllDigitalInputs();
    bool hasDigitalInputChanged(int index);
    int getDigitalInputValue(int index);
    uint16_t getDigitalInputIoa(int index);
    int getDigitalInputCount();
    void setDigitalOutput(uint16_t ioa, bool value);

    // Watchdog and System
    void resetWatchdog();
    void softwareReset();
    void updateHeartbeatLED();

    // Helper functions
    uint8_t getDoublePoint(uint8_t open, uint8_t close);
    uint8_t getCircuitBreakerStatus(int cbNumber);
    void operateCircuitBreaker(int cbNumber, int command);

    // I2C health monitoring & stats
    struct I2CStats {
        uint32_t errorCount;
        uint32_t consecutiveErrors;
        uint32_t recoveryCount;
        uint32_t lastErrorMs;
        uint32_t lastRecoveryMs;
    };
    void pollI2CHealth(); // non-blocking periodic health check (call from task loop)
    I2CStats getI2CStats();

private:
    PinInterface* _pinDriver;
    // I2C health state (protected by mutex implicitly as accesses are lightweight)
    I2CStats _i2cStats {0,0,0,0,0};
    uint32_t _lastHealthCheckMs = 0;
};

#endif // HARDWARE_MANAGER_H