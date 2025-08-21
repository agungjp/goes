#ifndef FRAME_PROCESSOR_H
#define FRAME_PROCESSOR_H

#ifdef NATIVE_ENV
#include <cstdint>
typedef uint8_t byte;
#else
#include <Arduino.h>
#endif

#include "IFrameProcessor.h"

class FrameProcessor {
private:
    IFrameProcessorListener* _listener;

public:
    FrameProcessor(IFrameProcessorListener* listener);
    void processReceivedFrame(const byte* buf, uint8_t len);
};

#endif // FRAME_PROCESSOR_H
