#ifndef PIN_ESP32_H
#define PIN_ESP32_H

#include "PinInterface.h"
#ifndef NATIVE_ENV
#include <Arduino.h>
#endif

// Pin mappings for ESP32 Dev Module
#define LED_BUILTIN 2
#define PIN_GFD           3
#define PIN_SUPPLY        12
#define PIN_REMOTE1       2
#define PIN_CB1_OPEN      4
#define PIN_CB1_CLOSE     5
#define PIN_REMOTE2       14
#define PIN_CB2_OPEN      15
#define PIN_CB2_CLOSE     16
#define PIN_CB1_OUT_OPEN  6
#define PIN_CB1_OUT_CLOSE 7
#define PIN_CB2_OUT_OPEN  17
#define PIN_CB2_OUT_CLOSE 11
#define PIN_MODEM_POWER   13

class PinESP32 : public PinInterface {
public:
    void setupPins() override;
    void setPin(int pin, int value) override;
    int getPin(int pin) override;
};

#endif // PIN_ESP32_H
