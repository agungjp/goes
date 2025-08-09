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

#include "IEC104Slave.h"
#include <string.h>

IEC104Slave::IEC104Slave(Stream* serial) : 
    gfd(false), prevGFD(false),
    supply(false), prevSupply(false),
    lastFrameReceived(0),
    state(STATE_DISCONNECTED),
    currentVoltageState(false),
    previousVoltageState(false),
    lastTransitionTime(0)
{
  modem = serial;
}

void IEC104Slave::setupConnection() {
  delay(15000);
  modem->println("AT"); updateSerial(); delay(5000);
  modem->println("AT+CSQ"); updateSerial();
  modem->println("AT+CGATT=1"); updateSerial();
  modem->println("AT+CIPMODE=1"); updateSerial();
  modem->println("AT+CSTT=\"m2mplnapd\""); updateSerial();
  modem->println("AT+CIICR"); updateSerial();delay(3000);
  modem->println("AT+CIFSR"); updateSerial();
  modem->println("AT+CIPSERVER=1,2404"); updateSerial();
  modem->println("AT+CIPSTATUS"); updateSerial();
}

void IEC104Slave::begin() {
  rtc.begin();
  setupPins();

  // Inisialisasi status GFD awal
  float Vin = (analogRead(PIN_GFD) * (5.0 / 1023.0)) * ((R1 + R2) / R2);
  currentVoltageState = (Vin > GFD_THRESHOLD_VOLTAGE);
  previousVoltageState = currentVoltageState;
  lastTransitionTime = millis();
  gfd = false; // Asumsi GFD normal saat startup
  prevGFD = gfd;

  restartModem();
  setupConnection();
  flushModem(); 
  updateInputs();
  lastFrameReceived = millis();
  prevSupply  = supply;
}

void IEC104Slave::run() {
  digitalWrite(LED_BUILTIN, millis() % 1000 < 100);
  updateInputs();
  if (state == STATE_CONNECTED){
    checkCOS();
  } 
  listen();
  if (millis() - lastFrameReceived > TEST_ACT_TIMEOUT) {
    softwareReset();
  }
  wdt_reset();
}

void IEC104Slave::setupPins() {
  // Input pin
  pinMode(PIN_GFD, INPUT);
  pinMode(PIN_SUPPLY, INPUT_PULLUP);
  
  // Output pin
  pinMode(PIN_MODEM_POWER, OUTPUT);
  digitalWrite(PIN_MODEM_POWER, LOW);
}

void IEC104Slave::restartModem() {
  #ifdef DEBUG
  Serial.println("🔁 Restarting modem via D10...");
  #endif
  digitalWrite(PIN_MODEM_POWER, HIGH);
  delay(500);
  digitalWrite(PIN_MODEM_POWER, LOW);
  delay(500);
  #ifdef DEBUG
  Serial.println("✅ Restart done.");
  #endif
}

void IEC104Slave::updateSerial() {
  delay(2000);
  #ifdef DEBUG
  while (modem->available()) Serial.write(modem->read());
  while (Serial.available()) modem->write(Serial.read());
  #endif
}

void IEC104Slave::updateInputs() {
  prevGFD     = gfd;
  prevSupply  = supply;

  // GFD Blink Detection Logic
  float Vin = (analogRead(PIN_GFD) * (5.0 / 1023.0)) * ((R1 + R2) / R2);
  currentVoltageState = (Vin > GFD_THRESHOLD_VOLTAGE);

  if (currentVoltageState != previousVoltageState) {
    lastTransitionTime = millis();
    previousVoltageState = currentVoltageState;
  }

  if (millis() - lastTransitionTime > NO_BLINK_TIMEOUT_MS) {
    gfd = false; // Normal
  } else {
    gfd = true; // Aktif (ada kedipan)
  }
  
  // Supply Reading
  supply = digitalRead(PIN_SUPPLY);      // IOA 1004
}

void IEC104Slave::checkCOS() {
  if (gfd != prevGFD) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevGFD); Serial.print(" → "); Serial.println(gfd);
    #endif
    sendTimestamped(30, 1002, gfd ? 1 : 0);
    prevGFD = gfd;
  }
  if (supply != prevSupply) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Supply berubah: "); Serial.print(prevSupply); Serial.print(" → "); Serial.println(supply);
    #endif
    sendTimestamped(30, 1004, supply ? 1 : 0);
    prevSupply = supply;
  }
}

void IEC104Slave::listen() {
  static byte buf[MAX_BUFFER];
  static String modemText = "";
  int len = 0;
  
  while (modem->available()) {
    char c = modem->read();

    if (c == '\n' || c == '\r') {
      if (modemText.length() > 0) {
        modemText.trim();
        handleModemText(modemText);
        modemText = "";
      }
    } else if ((byte)c != 0x68) {
      modemText += c;
      continue;
    }

    if ((byte)c == 0x68) {
      buf[0] = 0x68;
      unsigned long tStart = millis();
      while (modem->available() < 1) {
        if (millis() - tStart > 200) {
          #ifdef DEBUG
          Serial.println("[TIMEOUT] Wait length byte");
          #endif
          return;
        }
      }
      byte length = modem->read();
      buf[1] = length;

      if (length > MAX_BUFFER - 2) return;

      bool timeout = false;
      for (int i = 0; i < length; i++) {
        unsigned long t1 = millis();
        while (!modem->available()) {
          if (millis() - t1 > 200) {
            timeout = true;
            #ifdef DEBUG
            Serial.print("[TIMEOUT] Wait data byte ke-"); Serial.println(i);
            #endif
            break;
          }
        }
        if (timeout) break;
        buf[2 + i] = modem->read();
      }

      if (timeout) {
        #ifdef DEBUG
        Serial.println("[TIMEOUT] Frame IEC104 tidak lengkap, dibuang!");
        #endif
        continue;
      }

      len = length + 2;

      if ((buf[2] & 0x01) == 0) {
        handleIFrame(buf, len);
      }
      else if (buf[2] == 0x01) {
        handleSFrame(buf, len);
      }
      else if (buf[1] == 0x04) {
        handleUFrame(buf, len);
      }
    }
  }
}

void IEC104Slave::handleModemText(String text) {
  text.toUpperCase();
  if (text.indexOf("CLOSED") >= 0) {
    state = STATE_DISCONNECTED;
    #ifdef DEBUG
    Serial.println("⚠️ Koneksi CLOSED. Siap reconnect...");
    #endif
  } else if (text.indexOf("REMOTE IP") >= 0 || text.indexOf("CONNECT") >= 0) {
    state = STATE_WAIT_STARTDT;
    #ifdef DEBUG
    Serial.println("🔌 CONNECT terdeteksi. Menunggu STARTDT_ACT...");
    #endif
  }
  Serial.println("📡 MODEM: " + text);
} 

void IEC104Slave::handleIFrame(const byte* buf, byte len) {
  lastFrameReceived = millis();
  ns = (buf[3] << 7) | (buf[2] >> 1);
  nr = (buf[5] << 7) | (buf[4] >> 1);

  #ifdef DEBUG
  Serial.print("Master : NS ("); Serial.print(ns);
  Serial.print(") NR("); Serial.print(nr); Serial.print(").    → ");
  for (int i = 0; i < len; i++) {
    if (buf[i] < 0x10) Serial.print("0");
    Serial.print(buf[i], HEX); Serial.print(" ");
  }
  Serial.println();
  #endif

  rxSeq = ns + 1;
  txSeq = nr;

  byte ti = buf[6];
  switch (ti) {
    case 100: handleGI(buf, len); break;
    case 103: handleRTC(buf, len); break;
    default: break;
  }
}

void IEC104Slave::handleSFrame(const byte* buf, byte len) {
  lastFrameReceived = millis();
  nr = (buf[5] << 7) | (buf[4] >> 1);
  
  #ifdef DEBUG
  Serial.print("Master (S-Format): NS ("); Serial.print(ns);
  Serial.print(") NR("); Serial.print(nr); Serial.print(").    → ");
  for (int i = 0; i < len; i++) {
    if (buf[i] < 0x10) Serial.print("0");
    Serial.print(buf[i], HEX); Serial.print(" ");
  }
  Serial.println();
  #endif

  txSeq = nr;
}

void IEC104Slave::handleUFrame(const byte* buf, byte len) {
  lastFrameReceived = millis();
  #ifdef DEBUG
  Serial.print("Master (U-Format):.    → ");
  for (int i = 0; i < len; i++) {
    if (buf[i] < 0x10) Serial.print("0");
    Serial.print(buf[i], HEX); Serial.print(" ");
  }
  Serial.println();
  #endif

  if (buf[2] == 0x07){
    sendUFormat(0x0B);
    state = STATE_CONNECTED;
  }
  else if (buf[2] == 0x43){
    sendUFormat(0x83);
  }
}

void IEC104Slave::handleGI(const byte* buf, byte len) {
  #ifdef DEBUG
    Serial.println("Ti 100 (General Introgation)");
  #endif
  byte ca_lo = buf[10];
  byte ca_hi = buf[11];
  updateInputs();

  byte actCon[] = {0x64, 0x01, 0x07, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(actCon, sizeof(actCon)); 
  delay(50);

   byte ti1[] = {
    0x01, 0x04, 0x14, 0x00, ca_lo, ca_hi,
    // Remote/Local cell 1 (IOA 1001)
    0xE9, 0x03, 0x00, 0,
    // GFD (IOA 1002)
    0xEA, 0x03, 0x00, gfd ? 1 : 0,
    // Remote/Local cell 2 (IOA 1003)
    0xEB, 0x03, 0x00, 0,
    // Supply (IOA 1004)
    0xEC, 0x03, 0x00, supply ? 1 : 0
  };
  sendIFrame(ti1, sizeof(ti1)); delay(50);

  byte ti3[] = {
    0x03, 0x02, 0x14, 0x00, ca_lo, ca_hi,
    // CB1 (IOA 11000)
    0xF8, 0x2A, 0x00, 1,
    // CB2 (IOA 11001)
    0xF9, 0x2A, 0x00, 1
  };
  sendIFrame(ti3, sizeof(ti3)); delay(50);

  byte term[] = {0x64, 0x01, 0x0A, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(term, sizeof(term));
  delay(200);
}

void IEC104Slave::handleRTC(const byte* buf, byte len) {
  const byte* time = &buf[15];
  setRTCFromCP56(time);
  byte ack[16] = {0x67, 0x01, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
  memcpy(&ack[9], time, 7);
  sendIFrame(ack, sizeof(ack));
}

void IEC104Slave::setRTCFromCP56(const byte* time) {
  uint16_t ms     = time[0] | (time[1] << 8);
  byte minute     = time[2] & 0x3F;
  byte hour       = time[3] & 0x1F;
  byte date       = time[4] & 0x1F;
  byte dow        = (time[4] >> 5) & 0x07;
  byte month      = time[5] & 0x0F;
  uint16_t year   = 2000 + (time[6] & 0x7F);

  #ifdef DEBUG
  Serial.print("🕒 RTC diatur ke: ");
  Serial.print(year); Serial.print("-");
  if (month < 10) Serial.print("0");
  Serial.print(month); Serial.print("-");
  if (date < 10) Serial.print("0");
  Serial.print(date); Serial.print(" ");
  if (hour < 10) Serial.print("0");
  Serial.print(hour); Serial.print(":");
  if (minute < 10) Serial.print("0");
  Serial.print(minute); Serial.print(":");
  if ((ms / 1000) < 10) Serial.print("0");
  Serial.print(ms / 1000);
  Serial.print(" (DOW: "); Serial.print(dow); Serial.println(")");
  #endif

  rtc.setDOW(dow);
  rtc.setDate(date, month, year);
  rtc.setTime(hour, minute, ms / 1000);
}

void IEC104Slave::convertCP56Time2a(uint8_t* buffer) {
  Time t = rtc.getTime();
  uint16_t ms = t.sec * 1000;
  buffer[0] = ms & 0xFF;
  buffer[1] = (ms >> 8) & 0xFF;
  buffer[2] = t.min & 0x3F;
  buffer[3] = t.hour & 0x1F;
  buffer[4] = ((t.dow & 0x07) << 5) | (t.date & 0x1F);
  buffer[5] = t.mon & 0x0F;
  buffer[6] = (t.year - 2000) & 0x7F;
}

void IEC104Slave::sendTimestamped(byte ti, uint16_t ioa, byte value) {
  uint8_t cp56[7];
  convertCP56Time2a(cp56);
  byte pdu[17] = {ti, 1, 3, 0, 1, 0, (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, value};
  memcpy(&pdu[10], cp56, 7);
  sendIFrame(pdu, sizeof(pdu));
}

void IEC104Slave::sendIFrame(const byte* payload, byte len) {
  byte frame[6 + len];
  frame[0] = 0x68;
  frame[1] = len+4;
  frame[2] = txSeq << 1;
  frame[3] = txSeq >> 7;
  frame[4] = rxSeq << 1;
  frame[5] = rxSeq >> 7;
  memcpy(&frame[6], payload, len);
  modem->write(frame, 6 + len);

  #ifdef DEBUG
  Serial.print("Slave : NS ("); Serial.print(txSeq); Serial.print(") NR("); Serial.print(rxSeq); Serial.print(").    ← ");
  for (int i = 0; i < 6 + len; i++) {
    if (frame[i] < 0x10) Serial.print("0");
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
  Serial.println();
  #endif
  txSeq++;
  delay(50);
}

void IEC104Slave::sendUFormat(byte controlByte) {
  byte r[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
  modem->write(r, 6);

  #ifdef DEBUG
  Serial.print("Slave (U-Format).    ← ");
  for (int i = 0; i < 6; i++) {
    if (r[i] < 0x10) Serial.print("0");
    Serial.print(r[i], HEX); Serial.print(" ");
  }
  Serial.println();
  #endif
  
  delay(50);
}

void IEC104Slave::softwareReset() {
  #ifdef DEBUG
    Serial.println(F("⚠️  Tidak ada Komunikasi dari master >5 menit → hardware reset..."));
  #endif
  wdt_enable(WDTO_15MS);
  while(1) {}
}

void IEC104Slave::flushModem() {
  while (modem->available()) modem->read();
  delay(100);
  #ifdef DEBUG
  while (Serial.available()) Serial.read();
  #endif
}
