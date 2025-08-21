#ifndef IEC104_COMMUNICATOR_H
#define IEC104_COMMUNICATOR_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
using byte = uint8_t;
#endif

#include "comm/CommInterface.h"
#include "utils/FrameQueue.h"
#include "config/config_global.h"

#define FRAME_SIZE 255

class IEC104Communicator {
public:
    IEC104Communicator(CommInterface* comm);
    
    // --- Methods for comm_task (I/O Operations) ---
    void listen();           // Reads from hardware and populates _receiveQueue
    void processSendQueue(); // Dequeues from _sendQueue and writes to hardware

    // --- Methods for iec104_task (Protocol Logic) ---
    void processReceivedQueue(); // Dequeues from _receiveQueue and handles frame
    void queueIFrame(const byte* payload, byte len); // Enqueues an I-frame to _sendQueue
    void queueUFormat(byte controlByte);             // Enqueues a U-frame to _sendQueue

    // --- Callback for upper layer ---
    using FrameHandler = void(*)(void* ctx, const byte* buf, byte len);
    void setFrameHandler(FrameHandler handler, void* ctx) { _frameHandler = handler; _frameHandlerCtx = ctx; }

    // --- Status methods ---
    unsigned long getLastFrameReceived() const;
    bool isConnected() const { return state == STATE_CONNECTED; }

private:
    // Internal frame handlers
    void handleIFrame(const byte* buf, byte len);
    void handleSFrame(const byte* buf, byte len);
    void handleUFrame(const byte* buf, byte len);

    CommInterface* _comm;
    FrameQueue _sendQueue;
    FrameQueue _receiveQueue;

    uint16_t txSeq = 0, rxSeq = 0;
    uint16_t ns = 0, nr = 0;
    unsigned long lastFrameReceived = 0;

    enum IEC104State { STATE_DISCONNECTED, STATE_WAIT_STARTDT, STATE_CONNECTED };
    IEC104State state = STATE_DISCONNECTED;

    FrameHandler _frameHandler = nullptr;
    void* _frameHandlerCtx = nullptr;
};

#endif


