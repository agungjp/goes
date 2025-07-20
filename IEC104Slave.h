/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.6.1  |
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
  void updateRelay();
  void updateSerial();
  void handleModemText(String text);
  void restartModem();
  void sendTI46AckAndTerm(uint32_t ioa, byte sco);
  uint8_t getDoublePoint(uint8_t open, uint8_t close);
  void handleDoubleCommand(uint32_t ioa, uint8_t command);
  void handleFrameTx();
  void queueFrame(const byte* data, uint8_t len);
  void softwareReset();

  DS3231 rtc = DS3231(SDA, SCL);
  Stream* modem = nullptr;

  uint16_t txSeq = 0, rxSeq = 0;
  uint16_t ns = 0, nr = 0;

  bool remote1, prevRemote1;
  bool remote2, prevRemote2;
  bool gfd, prevGFD;
  bool supply, prevSupply;
  byte cb1, prevCB1;
  byte cb2, prevCB2;

  int connectionState = 0; // 0 = disconnected, 1 = waiting STARTDT, 2 = connected

  // --- Flag TI 46 ---
  // true = sedang proses relay CB1/CB2
  bool cbBusy = false;
  uint32_t ti46_pending_ioa = 0;
  byte ti46_pending_sco = 0;
  unsigned long ti46_pending_start = 0;
  const unsigned long ti46_timeout = 8000;
  unsigned long remote1ChangeStart = 0;
  unsigned long remote2ChangeStart = 0;
  bool remote1Stable = true;
  bool remote2Stable = true;
  uint8_t remote1LastSample = 0;
  uint8_t remote2LastSample = 0;

  // Relay pulse state (per CB)
  bool relayActiveCB1Open = false;
  bool relayActiveCB1Close = false;
  bool relayActiveCB2Open = false;
  bool relayActiveCB2Close = false;
  unsigned long relayStartCB1Open = 0;
  unsigned long relayStartCB1Close = 0;
  unsigned long relayStartCB2Open = 0;
  unsigned long relayStartCB2Close = 0;
  const unsigned long relayPulse = 800; // 800 ms

  uint8_t modemRestartStep = 0;
  unsigned long modemRestartTime = 0;
  bool modemRestarting = false;

  // Input pin
  const int PIN_GFD           = 3;
  const int PIN_SUPPLY        = 12;
  const int PIN_REMOTE1       = 2;
  const int PIN_CB1_OPEN      = 4;
  const int PIN_CB1_CLOSE     = 5;
  const int PIN_REMOTE2       = 14;
  const int PIN_CB2_OPEN      = 15;
  const int PIN_CB2_CLOSE     = 16;
  // Output pin
  const int PIN_CB1_OUT_OPEN  = 6;
  const int PIN_CB1_OUT_CLOSE = 7;
  const int PIN_CB2_OUT_OPEN  = 17;
  const int PIN_CB2_OUT_CLOSE   = 11;
  const int PIN_MODEM_POWER   = 9;

  static const int MAX_BUFFER = 64;

  unsigned long lastFrameSent = 0;
  const unsigned long minFrameInterval = 20; // ms
  bool framePending = false;
  uint8_t frameBuffer[64]; // buffer untuk simpan frame, sesuaikan ukuran
  uint8_t frameLength = 0;

  const unsigned long TEST_ACT_TIMEOUT = 300000UL; // Waktu maksimal tanpa TESTFR_ACT = 5 menit
  unsigned long lastTestAct = 0;
};

#endif

// jabar = m2mplnapd
// kskt = M2MAPDKSKT
// sumbar = apdsumbarm2m / apdsumbarm2m.xl
// kaltimra =M2MAPDKALTIMRA
// lampung =plnlampung
