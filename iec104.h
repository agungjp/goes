#ifndef IEC104_H
#define IEC104_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class GSMConnection {
public:
  void begin(Stream* serial);
  void setupConnection();
  void listen();
  void setupInputPins();

private:
  Stream* modemSerial;
  void updateSerial();
  void sendUFormat(byte controlByte);
  void sendIFrame(const byte* payload, byte len);
  void updateInputStatus();

  // NS/NR
  uint16_t txSequence = 0;
  uint16_t rxSequence = 0;

  // Status input
  bool statusLocalRemote = false;
  bool statusGFD = false;
  byte statusCB = 0;

  // Pin mapping
  const int pinLocalRemote = 2;
  const int pinGFD         = 3;
  const int pinCB_1        = 4;
  const int pinCB_2        = 5;
};

#endif
