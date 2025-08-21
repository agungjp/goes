#ifndef IEC104_CORE_IEC104CORE_H_
#define IEC104_CORE_IEC104CORE_H_

#ifdef NATIVE_ENV
#include <cstdint>
typedef uint8_t byte;
#else
#include <Arduino.h>
#include <DS3231.h> // For RTC
#endif

#include "iec104/transport/IEC104Communicator.h"
#include "hal/HardwareManager.h"
#include "config/config_ioa.h"
#include "iec104/core/IFrameProcessor.h"

// Struct for queueing measurement data
struct MeasurementData {
    uint16_t ioa;
    int value;
    byte type;
};

// Struct for queueing frame data
struct FrameData {
    byte buffer[256];
    byte length;
};

class IEC104Core : public IFrameProcessor {
public:
    IEC104Core();
    void loop();
    void processFrame(const byte* buf, byte len) override;
    void sendData(uint16_t ioa, int value, byte type);

    using FrameReadyCallback = void(*)(void* ctx, const byte* buf, byte len);
    void setFrameReadyCallback(FrameReadyCallback callback, void* ctx);

private:
    FrameReadyCallback _frameReadyCallback = nullptr;
    void* _callback_ctx = nullptr;

    uint16_t _txSeq = 0;
    uint16_t _rxSeq = 0;

    void sendUFormat(byte controlByte);
    void sendSFrame();
    void handleIFrame(const byte* buf, byte len);
    void handleUFrame(const byte* buf, byte len);
};

#endif


