#ifndef IEC104SLAVE_H
#define IEC104SLAVE_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DS3231.h>

class IEC104Slave {
public:
  IEC104Slave(Stream* serial);
  void begin();
  void run();

private:
  void handleRTC(const byte* time);
  void handleIFrame(const byte* buf, byte len);
  void handleSFrame(const byte* buf);
  void handleUFrame(const byte* buf);
  void handleGI(const byte* buf, byte len);
  void setRTCFromCP56(const byte* time);
  void setupConnection();
  void setupPins();
  void updateInputs();
  void checkCOS();
  void listen();
  void handleTI46(const byte* data, byte len);
  void sendTimestamped(byte ti, uint16_t ioa, byte value);
  void sendIFrame(const byte* payload, byte len);
  void sendUFormat(byte controlByte);
  void convertCP56Time2a(uint8_t* buffer);
  void triggerRelay(byte command);
  void updateSerial();

  DS3231 rtc = DS3231(SDA, SCL);
  Stream* modem = nullptr;

  uint16_t txSeq = 0, rxSeq = 0;
  bool prevRemote = false, remote = false;
  bool prevGFD = false, gfd = false;
  byte prevCB = 0, cb = 0;

  const int PIN_REMOTE = 2;
  const int PIN_GFD = 3;
  const int PIN_CB1 = 4;
  const int PIN_CB2 = 5;
  const int PIN_OPEN = 6;
  const int PIN_CLOSE = 7;

  static const int MAX_BUFFER = 64;
};

#endif
