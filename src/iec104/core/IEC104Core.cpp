#include "IEC104Core.h"
#include "config/config_global.h"
#include <cstring>

IEC104Core::IEC104Core() {}

void IEC104Core::loop() {
    // Core logic loop, can be used for periodic tasks if any
}

void IEC104Core::processFrame(const byte* buf, byte len) {
    if (len < 4) return; // Minimal frame length

    // Simplified frame parsing
    if ((buf[2] & 0x01) == 0) {
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
    payload[4] = 0;    // Common Address of ASDU (low byte)
    payload[5] = 0;    // Common Address of ASDU (high byte)
    
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
    frame[1] = sizeof(payload) + 4;
    frame[2] = _txSeq << 1;
    frame[3] = _txSeq >> 7;
    frame[4] = _rxSeq << 1;
    frame[5] = _rxSeq >> 7;
    memcpy(&frame[6], payload, sizeof(payload));

    if (_frameReadyCallback) {
        _frameReadyCallback(_callback_ctx, frame, sizeof(payload) + 6);
    }
    _txSeq++;
}

void IEC104Core::setFrameReadyCallback(FrameReadyCallback callback, void* ctx) {
    _frameReadyCallback = callback;
    _callback_ctx = ctx;
}

void IEC104Core::sendUFormat(byte controlByte) {
    byte frame[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
    if (_frameReadyCallback) {
        _frameReadyCallback(_callback_ctx, frame, 6);
    }
}

void IEC104Core::sendSFrame() {
    byte frame[6] = {0x68, 0x04, 0x01, 0x00, 0x00, 0x00};
    frame[4] = _rxSeq << 1;
    frame[5] = _rxSeq >> 7;
    if (_frameReadyCallback) {
        _frameReadyCallback(_callback_ctx, frame, 6);
    }
}

void IEC104Core::handleIFrame(const byte* buf, byte len) {
    _txSeq = ((buf[3] << 8) | buf[2]) >> 1;
    _rxSeq = ((buf[5] << 8) | buf[4]) >> 1;
    _rxSeq++;
    sendSFrame(); // Acknowledge I-Frame
}

void IEC104Core::handleUFrame(const byte* buf, byte len) {
    if (buf[2] == U_STARTDT_ACT) {
        sendUFormat(U_STARTDT_CON);
    } else if (buf[2] == U_TESTFR_ACT) {
        sendUFormat(U_TESTFR_CON);
    }
}


