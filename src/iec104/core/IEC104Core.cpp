#include "IEC104Core.h"
#include "iec104/iec104_config.h"
#include <cstring>

IEC104Core::IEC104Core() {}

void IEC104Core::loop() {
    // Core logic loop, can be used for periodic tasks if any
}

void IEC104Core::processFrame(const byte* buf, byte len) {
    if (len < 4) return; // Minimal frame length

    // Simplified frame parsing
    if ((buf[2] & 0x01) == 0) { // I-Frame
        handleIFrame(buf, len);
    } else if ((buf[2] & 0x03) == 0x01) {
        // S-Frame, just acknowledge for now
        _rxSeq = ((buf[5] << 8) | buf[4]) >> 1;
    } else if ((buf[2] & 0x03) == 0x03) {
        handleUFrame(buf, len);
    }
}

void IEC104Core::sendData(uint16_t ioa, int value, byte type) {
    byte payload[11];
    payload[0] = type; // Type Identification
    payload[1] = 1;    // Variable Structure Qualifier
    payload[2] = 3;    // Cause of Transmission: Spontaneous
    payload[3] = 0;    // Originator Address
    payload[4] = _commonAddress & 0xFF;    // Common Address low
    payload[5] = (_commonAddress >> 8) & 0xFF;    // Common Address high
    
    // Information Object Address
    payload[6] = ioa & 0xFF;
    payload[7] = (ioa >> 8) & 0xFF;
    payload[8] = 0x00;

    // Value
    payload[9] = value;
    
    // For timestamped data, add timestamp here
    // For now, we send without timestamp

    byte frame[256];
    frame[0] = 0x68;
    frame[1] = sizeof(payload) + 4; // length field (APCI excluding start 2 bytes?) simplified
    frame[2] = _txSeq << 1;
    frame[3] = _txSeq >> 7;
    frame[4] = _rxSeq << 1;
    frame[5] = _rxSeq >> 7;
    memcpy(&frame[6], payload, sizeof(payload)); // safe, frame has room

    if (_frameReadyCallback) {
    size_t total = sizeof(payload) + 6;
    if (total > sizeof(frame)) total = sizeof(frame);
    _frameReadyCallback(_frameReadyCtx, frame, total);
    }
    _txSeq++;
}

bool IEC104Core::buildDataFrame(uint16_t ioa, int value, byte type, byte* outBuf, byte &outLen) {
    if (!outBuf) return false;
    byte payload[11];
    payload[0] = type; // Type Identification
    payload[1] = 1;    // VSQ: 1 object
    payload[2] = 3;    // COT: Spontaneous
    payload[3] = 0;    // Originator
    payload[4] = _commonAddress & 0xFF;    // ASDU addr low
    payload[5] = (_commonAddress >> 8) & 0xFF;    // ASDU addr high
    payload[6] = ioa & 0xFF;
    payload[7] = (ioa >> 8) & 0xFF;
    payload[8] = 0;    // IOA third byte (unused)
    payload[9] = value & 0xFF; // value (truncated)
    payload[10] = 0;   // padding / quality (optional)

    // Build frame
    outBuf[0] = 0x68;
    outBuf[1] = sizeof(payload) + 4; // simplistic length
    outBuf[2] = _txSeq << 1;         // control field
    outBuf[3] = _txSeq >> 7;
    outBuf[4] = _rxSeq << 1;
    outBuf[5] = _rxSeq >> 7;
    memcpy(&outBuf[6], payload, sizeof(payload));
    size_t total = sizeof(payload) + 6;
    if (total > 256) total = 256;
    outLen = (byte)total;
    _txSeq++;
    return true;
}

void IEC104Core::setFrameReadyCallback(FrameReadyCallback callback, void* ctx) {
    _frameReadyCallback = callback;
    _frameReadyCtx = ctx;
}

void IEC104Core::sendUFormat(byte controlByte) {
    byte frame[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
    if (_frameReadyCallback) {
    _frameReadyCallback(_frameReadyCtx, frame, 6);
    }
}

void IEC104Core::sendSFrame() {
    byte frame[6] = {0x68, 0x04, 0x01, 0x00, 0x00, 0x00};
    frame[4] = _rxSeq << 1;
    frame[5] = _rxSeq >> 7;
    if (_frameReadyCallback) {
    _frameReadyCallback(_frameReadyCtx, frame, 6);
    }
}

void IEC104Core::handleIFrame(const byte* buf, byte len) {
    _txSeq = ((buf[3] << 8) | buf[2]) >> 1;
    _rxSeq = ((buf[5] << 8) | buf[4]) >> 1;
    _rxSeq++;

    // Check for command type in I-frame
    byte type = buf[6];
    if (type == C_SC_NA_1 || type == C_DC_NA_1) {
        // Expected minimum length for command ASDU (very simplified) >= 15
        if (len >= 15) {
            if (_commandCallback) {
                uint16_t ioa = (buf[13] << 8) | buf[12];
                uint8_t command = buf[14] & 0x7F; // Extract command, ignore S/E bit
                _commandCallback(_commandCtx, ioa, command);
            }
        } else {
            // Ignore malformed short command frame
            return;
        }
    }

    sendSFrame(); // Acknowledge I-Frame
}

void IEC104Core::handleUFrame(const byte* buf, byte len) {
    if (buf[2] == U_STARTDT_ACT) {
        sendUFormat(U_STARTDT_CON);
    } else if (buf[2] == U_TESTFR_ACT) {
        sendUFormat(U_TESTFR_CON);
    }
}

void IEC104Core::setCommandCallback(CommandCallback callback, void* ctx) {
    _commandCallback = callback;
    _commandCtx = ctx;
}


