#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "PinInterface.h"
#include <stdint.h> // For uint8_t
#include <HardwareSerial.h> // For HardwareSerial type

// HardwareManager is responsible for initializing and managing the correct
// hardware drivers for the selected platform.
// It delegates platform-specific calls (like setPin) to a PinInterface driver.
class HardwareManager {
public:
    HardwareManager();
    ~HardwareManager();

    void init(); // General hardware initialization
    void setupPins();
    void setPin(int pin, int value);
    int getPin(int pin);
    void updateHeartbeatLED();
    void resetWatchdog(); // Platform-specific watchdog reset
    void softwareReset(); // Platform-specific software reset

private:
    PinInterface* _pinDriver = nullptr; // Pointer to the actual pin driver
};

#endif