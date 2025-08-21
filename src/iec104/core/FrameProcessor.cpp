#include "config/config_global.h"
#include "FrameProcessor.h"
#include "config/config_ioa.h"

FrameProcessor::FrameProcessor(IFrameProcessorListener* listener) : _listener(listener) {}

void FrameProcessor::processReceivedFrame(const byte* buf, uint8_t len) {
    // APDU starts with 2 bytes (Start 0x68, Length)
    // Then Control Fields (4 bytes for I-frame)
    // Then ASDU
    if (!buf || len < 12) return; // Basic validation for I-frame with minimum ASDU

    const byte* asdu = buf + 6;
    uint8_t asduLen = len - 6;

    if (asduLen < 6) return; // Minimum ASDU length (TypeID, VSQ, COT, Common Addr)

    byte typeId = asdu[0];
    // COT is 2 bytes, little-endian
    uint16_t cot = (uint16_t)(asdu[3] << 8 | asdu[2]);

    // ASDU data starts after the common ASDU header
    const byte* asduData = asdu + 6;
    uint8_t asduDataLen = asduLen - 6;

    switch (typeId) {
        case C_IC_NA_1: // Interrogation Command
            if (cot == 6) { // Activation
                if (_listener) _listener->handleInterrogation();
            }
            break;

        case C_CS_NA_1: // Clock Synchronization Command
            if (cot == 6) { // Activation
                if (_listener) _listener->handleClockSync(asduData, asduDataLen);
            }
            break;
        
        case C_DC_NA_1: // Double Command
            if (cot == 6 || cot == 8) { // Activation or Deactivation
                 if (_listener) _listener->handleDoubleCommand(asduData, asduDataLen);
            }
            break;
    }
}
