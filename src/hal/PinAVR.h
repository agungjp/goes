#ifndef PIN_AVR_H
#define PIN_AVR_H

#include "PinInterface.h"

class PinAVR : public PinInterface {
public:
    void setupPins() override;
    void setPin(int pin, int value) override;
    int getPin(int pin) override;
};

#endif // PIN_AVR_H
