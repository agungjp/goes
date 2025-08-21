#include "IEC104Communicator.h"
#include "config/config_global.h"
#include <cstring>

#ifndef ARDUINO
#include <chrono>
static unsigned long millis() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return (unsigned long)duration_cast<milliseconds>(steady_clock::now() - start).count();
}
static void delay(unsigned long) {}
#endif

IEC104Communicator::IEC104Communicator(CommInterface* comm) : _comm(comm) {
    // Constructor
}

void IEC104Communicator::listen() {
#ifdef ARDUINO
    static byte buf[FRAME_SIZE];
    
    while (_comm->available()) {
        char c = _comm->read();

        // This part is simplified. A real implementation might need a more robust
        // state machine to handle partial frames or modem text.
        if ((byte)c != 0x68) {
            continue; // Ignore non-start bytes for now
        }

        buf[0] = 0x68;
        unsigned long tStart = millis();
        
        // Wait for length byte
        while (_comm->available() < 1) {
            if (millis() - tStart > 200) return; // Timeout
        }
        byte length = _comm->read();
        buf[1] = length;

        if (length > FRAME_SIZE - 2) return; // Overflow guard

        // Read the rest of the frame
        for (int i = 0; i < length; i++) {
            unsigned long t1 = millis();
            while (!_comm->available()) {
                if (millis() - t1 > 200) return; // Timeout
            }
            buf[2 + i] = _comm->read();
        }

        // If we got a full frame, enqueue it
        _receiveQueue.enqueue(buf, length + 2);
    }
#endif
}

void IEC104Communicator::processSendQueue() {
    byte frame[FRAME_SIZE];
    uint8_t length = 0;
    if (!_sendQueue.isEmpty()) {
        _sendQueue.dequeue(frame, length);
        _comm->write(frame, length);
    }
}

void IEC104Communicator::processReceivedQueue() {
    if (_receiveQueue.isEmpty()) {
        return;
    }

    byte frame[FRAME_SIZE];
    uint8_t len = 0;
    _receiveQueue.dequeue(frame, len);

    if (len < 6) return; // Basic validation

    if ((frame[2] & 0x01) == 0) {
        handleIFrame(frame, len);
    } else if (frame[2] == 0x01) {
        handleSFrame(frame, len);
    } else if (frame[1] == 0x04) { // U-frame has fixed length of 4
        handleUFrame(frame, len);
    }
}

void IEC104Communicator::handleIFrame(const byte* buf, byte len) {
    lastFrameReceived = millis();
    ns = (buf[3] << 7) | (buf[2] >> 1);
    nr = (buf[5] << 7) | (buf[4] >> 1);

    rxSeq = ns + 1;
    txSeq = nr;

    if (_frameHandler) {
        _frameHandler(_frameHandlerCtx, buf, len);
    }
}

void IEC104Communicator::handleSFrame(const byte* buf, byte len) {
    lastFrameReceived = millis();
    nr = (buf[5] << 7) | (buf[4] >> 1);
    txSeq = nr;
}

void IEC104Communicator::handleUFrame(const byte* buf, byte len) {
    lastFrameReceived = millis();
    if (buf[2] == 0x07) { // STARTDT_ACT
        queueUFormat(0x0B); // STARTDT_CON
        state = STATE_CONNECTED;
    } else if (buf[2] == 0x43) { // TESTFR_ACT
        queueUFormat(0x83); // TESTFR_CON
    }
}

void IEC104Communicator::queueIFrame(const byte* payload, byte len) {
    byte frame[FRAME_SIZE];
    if (len + 6 > FRAME_SIZE) return; // Ensure it fits

    frame[0] = 0x68;
    frame[1] = len + 4;
    frame[2] = txSeq << 1;
    frame[3] = txSeq >> 7;
    frame[4] = rxSeq << 1;
    frame[5] = rxSeq >> 7;
    memcpy(&frame[6], payload, len);
    
    _sendQueue.enqueue(frame, len + 6);
    txSeq++;
}

void IEC104Communicator::queueUFormat(byte controlByte) {
    byte frame[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
    _sendQueue.enqueue(frame, 6);
}

unsigned long IEC104Communicator::getLastFrameReceived() const {
    return lastFrameReceived;
}


