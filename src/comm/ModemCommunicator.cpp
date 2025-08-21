#include "ModemCommunicator.h"
#include "config/goes_config.h"

ModemCommunicator::ModemCommunicator(CommInterface* physicalLayer) : _physicalLayer(physicalLayer) {}

void ModemCommunicator::setupConnection() {
    if (_physicalLayer) {
        _physicalLayer->setupConnection();
    }
}

void ModemCommunicator::restart() {
    if (_physicalLayer) {
        _physicalLayer->restart();
    }
}

void ModemCommunicator::sendData(const uint8_t* data, size_t len) {
    if (_physicalLayer) {
        _physicalLayer->write(data, len);
    }
}

void ModemCommunicator::loop() {
    if (!_physicalLayer) return;

    while (_physicalLayer->available() > 0) {
        byte buffer[256];
        int len = _physicalLayer->readBytes(buffer, sizeof(buffer));
        if (len > 0 && _frameReceivedCallback) {
            _frameReceivedCallback(_callback_ctx, buffer, len);
        }
    }
}

void ModemCommunicator::setFrameReceivedCallback(void (*callback)(void*, const byte*, byte), void* ctx) {
    _frameReceivedCallback = callback;
    _callback_ctx = ctx;
}
