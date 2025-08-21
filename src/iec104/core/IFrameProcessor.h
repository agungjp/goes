#ifndef I_FRAME_PROCESSOR_H
#define I_FRAME_PROCESSOR_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
using byte = uint8_t;
#endif

class IFrameProcessor {
public:
    virtual ~IFrameProcessor() {}
    virtual void processFrame(const byte* buf, byte len) = 0;
};

#endif // I_FRAME_PROCESSOR_H
