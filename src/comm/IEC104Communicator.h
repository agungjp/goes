#ifndef IEC104_COMMUNICATOR_H
#define IEC104_COMMUNICATOR_H

#include <Arduino.h>
#include "CommInterface.h" // For Stream* and basic communication methods
#include "config/config_global.h"

class IEC104Communicator {
public:
    IEC104Communicator(CommInterface* comm);
    void listen();
    void handleIFrame(const byte* buf, byte len);
    void handleSFrame(const byte* buf, byte len);
    void handleUFrame(const byte* buf, byte len);
    void sendIFrame(const byte* payload, byte len);
    void sendUFormat(byte controlByte);
    unsigned long getLastFrameReceived() const;

private:
    CommInterface* _comm;

    uint16_t txSeq = 0, rxSeq = 0;
    uint16_t ns = 0, nr = 0;
    unsigned long lastFrameReceived = 0;

    enum IEC104State { STATE_DISCONNECTED, STATE_WAIT_STARTDT, STATE_CONNECTED };
    IEC104State state = STATE_DISCONNECTED;
};

#endif