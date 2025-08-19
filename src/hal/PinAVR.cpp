#include "PinAVR.h"
#include <Arduino.h>

#if defined(BOARD_ATMEGA328P)

// Pin mappings for ATmega328P (e.g., Arduino Uno/Nano)
#define LED_BUILTIN 13
#define PIN_GFD           2
#define PIN_SUPPLY        3
#define PIN_REMOTE1       4
#define PIN_CB1_OPEN      5
#define PIN_CB1_CLOSE     6
#define PIN_REMOTE2       7
#define PIN_CB2_OPEN      8
#define PIN_CB2_CLOSE     9
#define PIN_CB1_OUT_OPEN  A0
#define PIN_CB1_OUT_CLOSE A1
#define PIN_CB2_OUT_OPEN  A2
#define PIN_CB2_OUT_CLOSE A3
#define PIN_MODEM_POWER   12 // Using D12 for modem power

void PinAVR::setupPins() {
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

void PinAVR::setPin(int pin, int value) {
    digitalWrite(pin, value);
}

int PinAVR::getPin(int pin) {
    return digitalRead(pin);
}

#endif // BOARD_ATMEGA328P