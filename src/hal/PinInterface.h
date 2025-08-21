#ifndef PIN_INTERFACE_H
#define PIN_INTERFACE_H

class PinInterface {
public:
    virtual ~PinInterface() = default;
    virtual void setupPins() = 0;
    virtual void setPinMode(int pin, int mode) = 0;
    virtual void setPin(int pin, int value) = 0;
    virtual int getPin(int pin) = 0;
};

#endif