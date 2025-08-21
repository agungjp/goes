#ifndef I_FRAME_PROCESSOR_H
#define I_FRAME_PROCESSOR_H

#ifdef NATIVE_ENV
#include <cstdint>
typedef uint8_t byte;
#else
#include <Arduino.h>
#endif

/**
 * @brief Interface for a class that listens to processed frame events.
 * This is used to decouple FrameProcessor from IEC104Core for testing.
 */
class IFrameProcessorListener {
public:
    virtual ~IFrameProcessorListener() {}
    virtual void handleInterrogation() = 0;
    virtual void handleClockSync(const byte* asdu, uint8_t len) = 0;
    virtual void handleDoubleCommand(const byte* asdu, uint8_t len) = 0;
};

#endif // I_FRAME_PROCESSOR_H
