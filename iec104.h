#ifndef IEC104_H
#define IEC104_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DS3231.h>

#define FW_VERSION "v1.3.0"

class GSMConnection {
public:
  void begin(Stream* serial);
  void setupConnection();
  void listen();
  void setupInputPins();
  void updateInputStatus();
  void checkAndSendCOS();
  void sendTimestampedData(byte ti, uint16_t ioa, byte value);
  void handleTI46(const byte* data, byte len);
  void triggerRelay(byte command);

private:
  Stream* modemSerial;
  void updateSerial();
  void sendUFormat(byte controlByte);
  void sendIFrame(const byte* payload, byte len);
  void convertToCP56Time2a(uint8_t* buffer);

  uint16_t txSequence = 0;
  uint16_t rxSequence = 0;

  bool statusLocalRemote = false;
  bool statusGFD = false;
  byte statusCB = 0;

  bool prevLocalRemote = false;
  bool prevGFD = false;
  byte prevCB = 0;

  const int pinLocalRemote = 2;
  const int pinGFD         = 3;
  const int pinCB_1        = 4;
  const int pinCB_2        = 5;
  const int pinRelayOpen   = 6;
  const int pinRelayClose  = 7;

  DS3231 rtc = DS3231(SDA, SCL);
};

#endif
