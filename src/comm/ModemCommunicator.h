#ifndef MODEM_COMMUNICATOR_H
#define MODEM_COMMUNICATOR_H

#include "CommInterface.h"
#include <Arduino.h>

class ModemCommunicator : public CommInterface {
public:
    ModemCommunicator(CommInterface* physicalLayer);
    void setupConnection() override;
    void restart() override;
    void sendData(const uint8_t* data, size_t len) override;
    void loop();
    void setFrameReceivedCallback(void (*callback)(void*, const byte*, byte), void* ctx);

    // Implement pure virtual functions from CommInterface
    int available() override { return _physicalLayer ? _physicalLayer->available() : 0; }
    int read() override { return _physicalLayer ? _physicalLayer->read() : -1; }
    void write(const uint8_t* data, size_t len) override { sendData(data, len); }
    void flush() override { if (_physicalLayer) _physicalLayer->flush(); }
    size_t readBytes(uint8_t* buffer, size_t length) override {
        return _physicalLayer ? _physicalLayer->readBytes(buffer, length) : 0;
    }

private:
    CommInterface* _physicalLayer;
    void (*_frameReceivedCallback)(void*, const byte*, byte) = nullptr;
    void* _callback_ctx = nullptr;
};

#endif // MODEM_COMMUNICATOR_H