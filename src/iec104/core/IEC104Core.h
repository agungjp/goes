#ifndef IEC104_CORE_H
#define IEC104_CORE_H

#ifdef NATIVE_ENV
#include <cstdint>
typedef uint8_t byte;
#else
#include <Arduino.h>
#include <DS3231.h> // For RTC
#endif

#include "iec104/transport/IEC104Communicator.h"
#include "hal/HardwareManager.h"
#include "config/config_ioa.h" // For IOA definitions

class IEC104Core {
public:
    IEC104Core(IEC104Communicator* comm, HardwareManager* hw);
    void run(); // Main loop for core logic
    void processReceivedFrame(const byte* buf, byte len); // Callback from IEC104Communicator

    IEC104Communicator* getCommunicator() { return _comm; }

private:
    IEC104Communicator* _comm;
    HardwareManager* _hw;
#ifndef NATIVE_ENV
    DS3231 rtc = DS3231(Wire); // RTC instance
#endif

    // Member variables related to IEC104 state and data points
    bool remote1, prevRemote1;
    bool remote2, prevRemote2;
    bool gfd, prevGFD;
    bool supply, prevSupply;
    byte cb1, prevCB1;
    byte cb2, prevCB2;

    // Deprecated: use _comm->isConnected() instead
    int connectionState = 0; // 0 = disconnected, 1 = waiting STARTDT, 2 = connected

    // --- Flag TI 46 ---
    bool relayBusy = false;
    uint32_t ti46_pending_ioa = 0;
    byte ti46_pending_sco = 0;
    unsigned long relayStart = 0;
    bool cot7_sent = false;

    // Debounce remote1 & remote2
    bool remote1Stable, remote2Stable;
    unsigned long remote1DebounceStart;
    unsigned long remote2DebounceStart;
    bool remote1DebounceActive;
    bool remote2DebounceActive;
    bool remote1DebounceReady = false;
    bool remote2DebounceReady = false;

    // Private helper functions
    void updateInputs();
    void checkCOS();
    void handleGI(const byte* buf, byte len);
    void handleRTC(const byte* buf, byte len);
#ifndef NATIVE_ENV
    void setRTCFromCP56(const byte* time);
    void convertCP56Time2a(uint8_t* buffer);
#endif
    void sendTimestamped(byte ti, uint16_t ioa, byte value);
    void handleTI46(const byte* data, byte len);
    void handlePendingRelayTI46();
    uint8_t getDoublePoint(uint8_t open, uint8_t close);
    void softwareReset();
};

#endif


