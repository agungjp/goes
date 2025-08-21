#include "PinESP32.h"
#include <Arduino.h>



void PinESP32::setupPins() {
    // Input pins
    pinMode(PIN_GFD, INPUT_PULLUP);
    pinMode(PIN_SUPPLY, INPUT_PULLUP);
    pinMode(PIN_REMOTE1, INPUT_PULLUP);
    pinMode(PIN_CB1_OPEN, INPUT_PULLUP);
    pinMode(PIN_CB1_CLOSE, INPUT_PULLUP);
    pinMode(PIN_REMOTE2, INPUT_PULLUP);
    pinMode(PIN_CB2_OPEN, INPUT_PULLUP);
    pinMode(PIN_CB2_CLOSE, INPUT_PULLUP);
    
    // Output pins
    pinMode(PIN_MODEM_POWER, OUTPUT);
    pinMode(PIN_CB1_OUT_OPEN, OUTPUT);
    pinMode(PIN_CB1_OUT_CLOSE, OUTPUT);
    pinMode(PIN_CB2_OUT_OPEN, OUTPUT);
    pinMode(PIN_CB2_OUT_CLOSE, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Initialize output pins to LOW
    digitalWrite(PIN_CB1_OUT_OPEN, LOW);
    digitalWrite(PIN_CB1_OUT_CLOSE, LOW);
    digitalWrite(PIN_CB2_OUT_OPEN, LOW);
    digitalWrite(PIN_CB2_OUT_CLOSE, LOW);
    digitalWrite(PIN_MODEM_POWER, LOW);
    digitalWrite(LED_BUILTIN, LOW);
}

void PinESP32::setPinMode(int pin, int mode) {
    pinMode(pin, mode);
}

void PinESP32::setPin(int pin, int value) {
    digitalWrite(pin, value);
}

int PinESP32::getPin(int pin) {
    return digitalRead(pin);
}