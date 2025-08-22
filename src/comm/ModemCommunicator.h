#ifndef MODEM_COMMUNICATOR_H
#define MODEM_COMMUNICATOR_H

#include <Arduino.h>
#include "CommInterface.h"

class ModemCommunicator : public CommInterface {
public:
    // Constructor now takes a HardwareSerial object for the modem
    ModemCommunicator(HardwareSerial* serial);

    // Implementations for the CommInterface
    void setupConnection() override;
    void restart() override;
    void sendData(const uint8_t* data, size_t len) override;
    int available() override;
    int read() override;
    void write(const uint8_t* data, size_t len) override;
    void flush() override;
    size_t readBytes(uint8_t* buffer, size_t length) override;

    // Loop function to be called periodically to check for incoming data
    void loop();
    
    // Callback for when a full frame is received
    using FrameReceivedCallback = void(*)(void* ctx, const byte* buf, byte len);
    void setFrameReceivedCallback(FrameReceivedCallback callback, void* ctx);

private:
    HardwareSerial* _modemSerial;
    
    // Helper to print modem responses to the main Serial for debugging
    void updateSerial();

    FrameReceivedCallback _frameReceivedCallback = nullptr;
    void* _callback_ctx = nullptr;
};

#endif // MODEM_COMMUNICATOR_H