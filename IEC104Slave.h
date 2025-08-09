/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.6.2  |
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
// #define DEBUG

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DS3231.h>
#include <avr/wdt.h>

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
  void sendTimestamped(byte ti, uint16_t ioa, byte value);
  void sendIFrame(const byte* payload, byte len);
  void sendUFormat(byte controlByte);
  void convertCP56Time2a(uint8_t* buffer);
  void updateSerial();
  void handleModemText(String text);
  void restartModem();
  void softwareReset();
  void flushModem();

  DS3231 rtc = DS3231(SDA, SCL);
  Stream* modem = nullptr;

  uint16_t txSeq = 0, rxSeq = 0;
  uint16_t ns = 0, nr = 0;

  bool gfd, prevGFD;
  bool supply, prevSupply;

  // Voltage Divider Resistors for GFD
  const float R1 = 15000.0; // 15k Ohm
  const float R2 = 10000.0; // 10k Ohm

  // GFD Blink Detection Logic
  bool currentVoltageState;
  bool previousVoltageState;
  unsigned long lastTransitionTime;
  const float GFD_THRESHOLD_VOLTAGE = 0.5;
  const unsigned long NO_BLINK_TIMEOUT_MS = 5000;

  // Input pin
  const int PIN_GFD           = 14; // A0
  const int PIN_SUPPLY        = 3;
  const int PIN_MODEM_POWER   = 10;

  static const int MAX_BUFFER = 64;

  const unsigned long TEST_ACT_TIMEOUT = 5UL * 60UL * 1000UL;
  unsigned long lastFrameReceived = 0;

  enum IEC104State { STATE_DISCONNECTED, STATE_WAIT_STARTDT, STATE_CONNECTED };
  IEC104State state = STATE_DISCONNECTED;
};

#endif
