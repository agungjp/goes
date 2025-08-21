#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "PinInterface.h"
#include <stdint.h> // For uint8_t

class HardwareManager {
public:
    HardwareManager();
    ~HardwareManager();

    void init();
    void setupPins();
    void setPin(int pin, int value);
    int getPin(int pin);
    uint8_t getCircuitBreakerStatus(int cbNumber);
    void operateCircuitBreaker(int cbNumber, int command);
    void updateHeartbeatLED();
    void resetWatchdog();
    void softwareReset();

    // Methods for digital inputs
    void begin();
    void readAllDigitalInputs();
    bool hasDigitalInputChanged(int index);
    int getDigitalInputValue(int index);
    uint16_t getDigitalInputIoa(int index);
    int getDigitalInputCount();

private:
    PinInterface* _pinDriver = nullptr;
    uint8_t getDoublePoint(uint8_t open, uint8_t close);
};

#endif // HARDWARE_MANAGER_H