#include "iec104/transport/IEC104Communicator.h"
#include "iec104/core/IFrameProcessor.h"

IEC104Communicator::IEC104Communicator(CommInterface* comm, IFrameProcessor* core) 
    : _comm(comm), _core(core) {}

void IEC104Communicator::loop() {
    if (!_comm) return;

    while (_comm->available() > 0) {
        // Simplified loop: assumes one full frame is available
        byte buffer[256];
        int len = _comm->readBytes(buffer, sizeof(buffer));
        if (len > 0 && _frameReceivedCallback) {
            _frameReceivedCallback(_callback_ctx, buffer, len);
        }
    }
}

void IEC104Communicator::send(const byte* payload, byte len) {
    if (_comm) {
        _comm->write(payload, len);
    }
}

void IEC104Communicator::setFrameReceivedCallback(FrameReceivedCallback callback, void* ctx) {
    _frameReceivedCallback = callback;
    _callback_ctx = ctx;
}


