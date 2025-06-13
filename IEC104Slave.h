/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.5.0  |
|==============================================================================|
|  Copyright (C) 2024-2025 Mr. Pegagan (agungjulianperkasa@gmail.com)         |
|  All rights reserved.                                                        |
|==============================================================================|
|  IEC 60870-5-104 Arduino Slave is free software: you can redistribute it     |
|  and/or modify it under the terms of the Lesser GNU General Public License   |
|  as published by the Free Software Foundation, either version 3 of the       |
|  License, or (at your option) any later version.                             |
|                                                                              |
|  This program is distributed in the hope that it will be useful,             |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                        |
|  See the Lesser GNU General Public License for more details.                 |
|==============================================================================*/

#ifndef IEC104SLAVE_H
#define IEC104SLAVE_H
#define DEBUG
// #define SET_MANUAL_RTC

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DS3231.h>
#include <avr/wdt.h>    // untuk kontrol Watchdog Timer

class IEC104Slave {
public:
  IEC104Slave(Stream* serial);
  void begin();
  void run();

private:
  void handleRTC(const byte* buf, byte len);
  void handleIFrame(const byte* buf, byte len);
  void handleSFrame(const byte* buf, byte len);
  void handleUFrame(const byte* buf, byte len);
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
  void handleModemText(String text);
  void restartModem();
  bool readByteTimeout(Stream& s, uint8_t& out, unsigned long timeout = 200);

  DS3231 rtc = DS3231(SDA, SCL);
  Stream* modem = nullptr;

  uint16_t txSeq = 0, rxSeq = 0;
  uint16_t ns = 0, nr = 0;
  bool prevRemote = false, remote = false;
  bool prevGFD = false, gfd = false;
  byte prevCB = 0, cb = 0;
  int connectionState = 0; // 0 = disconnected, 1 = waiting STARTDT, 2 = connected

  const int PIN_REMOTE = 2;
  const int PIN_GFD = 12;
  const int PIN_CB1 = 5;
  const int PIN_CB2 = 4;
  const int PIN_OPEN = 6;
  const int PIN_CLOSE = 7;
  const int MODEM_POWER_PIN = 9;

  static const int MAX_BUFFER = 64;
};

#endif
