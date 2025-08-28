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
#include "config/device_config.h"
#include "stdint.h"
#include "iec104/core/IFrameProcessor.h"
#include "iec104/iec104_config.h"
#include <vector>

struct MeasurementData {
    uint16_t ioa;
    int value;
    byte type;
};

// Struct for queueing frame data
struct FrameData {
    byte buffer[256];
    uint16_t length; // unified with utils/Queue.h
};

class IEC104Core : public IFrameProcessor {
public:
    IEC104Core();
    void loop();
    void processFrame(const byte* buf, byte len) override;
    void sendData(uint16_t ioa, int value, byte type);
    // Build IEC104 I-frame for spontaneous measurement without queuing/callback.
    // Returns true on success and fills outLen (<=256) and outBuf with frame bytes.
    bool buildDataFrame(uint16_t ioa, int value, byte type, byte* outBuf, byte &outLen);
    void setCommonAddress(uint16_t ca) { _commonAddress = ca; }
    void setSendWindow(uint8_t k) { _kWindow = k; }

    using FrameReadyCallback = void(*)(void* ctx, const byte* buf, byte len);
    void setFrameReadyCallback(FrameReadyCallback callback, void* ctx);

    using CommandCallback = void(*)(void* ctx, uint16_t ioa, uint8_t command);
    void setCommandCallback(CommandCallback callback, void* ctx);

private:
    FrameReadyCallback _frameReadyCallback = nullptr;
    CommandCallback _commandCallback = nullptr;
    void* _frameReadyCtx = nullptr;   // context for frame ready (e.g., outgoing queue)
    void* _commandCtx = nullptr;      // context for command handling (e.g., hardware manager)

    uint16_t _txSeq = 0;
    uint16_t _rxSeq = 0;
    uint16_t _commonAddress = IEC104_ASDU_ADDRESS;
    uint8_t _kWindow = IEC104_K;

    void sendUFormat(byte controlByte);
    void sendSFrame();
    void handleIFrame(const byte* buf, byte len);
    void handleUFrame(const byte* buf, byte len);
};

#endif


