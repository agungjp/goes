/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave - FCO                    v1.0  |
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
  bool prevDI1 = false, DI1 = false;
  bool prevDI2 = false, DI2 = false;
  bool prevDI3 = false, DI3 = false;
  // byte prevCB = 0, cb = 0;
  int connectionState = 0; // 0 = disconnected, 1 = waiting STARTDT, 2 = connected

  const int PIN_DI_1 = 2;
  const int PIN_DI_2 = 3;
  const int PIN_DI_3 = 4;
  const int PIN_DI_4 = 5;
  const int PIN_DO_1 = 6;
  const int PIN_DO_2 = 7;
  const int MODEM_POWER_PIN = 10;

  static const int MAX_BUFFER = 64;
};

#endif
