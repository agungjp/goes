#ifndef PIN_ESP32_H
#define PIN_ESP32_H

#include "PinInterface.h"
#ifndef NATIVE_ENV
#include <Arduino.h>
#endif

// Tidak ada lagi definisi pin di sini.
// Semua pin sekarang didefinisikan di src/config/device_config.h

class PinESP32 : public PinInterface {
public:
    void setupPins() override;
    void setPinMode(int pin, int mode) override;
    void setPin(int pin, int value) override;
    int getPin(int pin) override;
};

#endif // PIN_ESP32_H
