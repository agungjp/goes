#pragma once

#include "CommInterface.h"
#include "hal/HardwareManager.h" // For PIN_MODEM_POWER
#include <Arduino.h> // For Stream, String
#include <stddef.h> // For size_t
#include <stdint.h> // For uint8_t

class ModemCommunicator : public CommInterface {
public:
    ModemCommunicator(Stream* serial, HardwareManager* hw);
    void setupConnection() override;
    void restart() override; // Implements CommInterface::restart()
    void sendData(const uint8_t* data, size_t len) override;
    int available() override;
    int read() override;
    void write(const uint8_t* data, size_t len) override;
    void flush() override;

    // Specific modem functions, not part of CommInterface
    void updateSerial();
    void handleModemText(String text); // Use String

private:
    Stream* _modem;
    HardwareManager* _hardware;
};