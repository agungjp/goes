// Debug
#define SrvDebug

#include "Arduino.h"
#include <avr/wdt.h>
#include <SPI.h>
#include <SoftwareSerial.h>

//Type of data


class iec104
{
public:
  iec104(SoftwareSerial &swSerial);
  bool Run();   
  bool Active;    
  unsigned long PreviousActivityTime;
  int Runs, Reads=1, Writes;
  SoftwareSerial *client;
  void setLocal(int stat);
private: 
  uint8_t ByteArray[260];
  void SetFC(int fc);
  int FC;
};