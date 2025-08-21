#ifndef IEC104_TRANSPORT_IEC104COMMUNICATOR_H_
#define IEC104_TRANSPORT_IEC104COMMUNICATOR_H_

#include "comm/CommInterface.h"
#include "iec104/core/IFrameProcessor.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
using byte = uint8_t;
#endif

class IEC104Communicator {
public:
    IEC104Communicator(CommInterface* comm, IFrameProcessor* core);
    
    void loop();
    void send(const byte* payload, byte len);

    using FrameReceivedCallback = void(*)(void* ctx, const byte* buf, byte len);
    void setFrameReceivedCallback(FrameReceivedCallback callback, void* ctx);

private:
    CommInterface* _comm;
    IFrameProcessor* _core;

    FrameReceivedCallback _frameReceivedCallback = nullptr;
    void* _callback_ctx = nullptr;
};

#endif


