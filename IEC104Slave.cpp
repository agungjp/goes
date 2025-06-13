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

#include "IEC104Slave.h"
#include <string.h>
extern unsigned long lastTestAct;

IEC104Slave::IEC104Slave(Stream* serial) {
  modem = serial;
}

void IEC104Slave::begin() {
  rtc.begin();
  #ifdef SET_MANUAL_RTC
  // rtc.setDOW(5);                   // 1.senin 2.selasa 3.rabu 4.kamis 5.jumat 6.sabtu 7.minggu
  // rtc.setDate(11, 4, 2025);        // Contoh: 10 Mei 2024
  // rtc.setTime(17, 30, 00);         // Contoh: 14:33:00
  #endif

  setupPins();
  restartModem();
  setupConnection();

  updateInputs();
  prevRemote = remote;
  prevGFD = gfd;
  prevCB = cb;
}

void IEC104Slave::setupPins() {
  pinMode(PIN_REMOTE, INPUT_PULLUP);
  pinMode(PIN_GFD, INPUT_PULLUP);
  pinMode(PIN_CB1, INPUT_PULLUP);
  pinMode(PIN_CB2, INPUT_PULLUP);
  pinMode(MODEM_POWER_PIN, OUTPUT);
  pinMode(PIN_OPEN, OUTPUT);
  pinMode(PIN_CLOSE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(PIN_OPEN, LOW);
  digitalWrite(PIN_CLOSE, LOW);
  digitalWrite(MODEM_POWER_PIN, LOW);
}

void IEC104Slave::restartModem() {
  #ifdef DEBUG
  Serial.println("🔁 Restarting modem via D9...");
  #endif
  digitalWrite(MODEM_POWER_PIN, HIGH);
  delay(500);
  digitalWrite(MODEM_POWER_PIN, LOW);
  delay(500);
  #ifdef DEBUG
  Serial.println("✅ Restart done.");
  #endif
}

void IEC104Slave::setupConnection() {
  delay(15000);
  modem->println("AT"); updateSerial(); delay(5000);
  modem->println("AT+CGATT=1"); updateSerial();
  modem->println("AT+CIPMODE=1"); updateSerial();
  modem->println("AT+CSTT=\"m2mplnapd\""); updateSerial();
  modem->println("AT+CIICR"); updateSerial();
  modem->println("AT+CIFSR"); updateSerial();
  modem->println("AT+CIPSERVER=1,2404"); updateSerial();
}

// jabar = m2mplnapd
// kskt = M2MAPDKSKT
// sumbar = apdsumbarm2m / apdsumbarm2m.xl
// kaltimra =M2MAPDKALTIMRA
// lampung =plnlampung

void IEC104Slave::updateSerial() {
  delay(2000);
  #ifdef DEBUG
  while (modem->available()) Serial.write(modem->read());
  while (Serial.available()) modem->write(Serial.read());
  #endif
}

void IEC104Slave::updateInputs() {
  prevRemote = remote;
  prevGFD = gfd;
  prevCB = cb;

  remote = digitalRead(PIN_REMOTE) == LOW; // HIGH = Local; LOW = Remote
  gfd = digitalRead(PIN_GFD);

  bool cb1 = digitalRead(PIN_CB1);
  bool cb2 = digitalRead(PIN_CB2);
  if (!cb1 && !cb2) cb = 0;
  else if (cb1 && !cb2) cb = 1;
  else if (!cb1 && cb2) cb = 2;
  else cb = 3;
}

void IEC104Slave::run() {
  updateInputs();
  checkCOS();
  listen();
  checkCOS();
}

void IEC104Slave::checkCOS() {
  // Serial.println("[DEBUG] Memeriksa COS...");
  if (remote != prevRemote) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Remote berubah: "); Serial.print(prevRemote); Serial.print(" → "); Serial.println(remote);
    #endif
    sendTimestamped(30, 1001, remote ? 0 : 1);
    prevRemote = remote;
  }
  if (gfd != prevGFD) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevGFD); Serial.print(" → "); Serial.println(gfd);
    #endif
    sendTimestamped(30, 1002, gfd ? 1 : 0);
    prevGFD = gfd;
  }
  if (cb != prevCB) {
    #ifdef DEBUG
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB); Serial.print(" → "); Serial.println(cb);
    #endif
    sendTimestamped(31, 11000, cb);
    prevCB = cb;
  }

  // if (gfd != prevGFD) {
  //   sendTimestamped(30, 1002, gfd ? 1 : 0);
  //   prevGFD = gfd;
  // }
  // if (cb != prevCB) {
  //   sendTimestamped(31, 11000, cb);
  //   prevCB = cb;
  // }
}

void IEC104Slave::listen() {
  static byte buf[MAX_BUFFER];
  static String modemText = "";
  int len = 0;

  while (modem->available()) {
    char c = modem->read();

    // Teks dari modem
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

    // Frame IEC 104 diawali 0x68
    if ((byte)c == 0x68) {
      buf[0] = 0x68;
      while (modem->available() < 1);  // tunggu 1 byte
      byte length = modem->read();
      buf[1] = length;

      // overflow guard
      if (length > MAX_BUFFER - 2) return;

      for (int i = 0; i < length; i++) {
        while (!modem->available());
        buf[2 + i] = modem->read();
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

bool IEC104Slave::readByteTimeout(Stream& s, uint8_t& out, unsigned long timeout) {
  unsigned long start = millis();
  while (!s.available()) {
    if (millis() - start >= timeout) return false;
  }
  out = s.read();
  return true;
}

void IEC104Slave::handleModemText(String text) {
  text.toUpperCase();
  if (text.indexOf("CLOSED") >= 0) {
    connectionState = 1;
    #ifdef DEBUG
    Serial.println("⚠️ Koneksi CLOSED. Siap reconnect...");
    #endif
  } else if ((text.indexOf("REMOTE IP") >= 0 || text.indexOf("CONNECT") >= 0) && connectionState == 1) {
    connectionState = 2;
    #ifdef DEBUG
    Serial.println("🔌 CONNECT terdeteksi. Menunggu STARTDT_ACT...");
    #endif
  }
  Serial.println("📡 MODEM: " + text);
  text.toUpperCase();

  if (text.indexOf("CLOSED") >= 0) {
    connectionState = 0;
    #ifdef DEBUG
    Serial.println("⚠️  Koneksi terputus.");
    #endif
  }
  else if (text.indexOf("REMOTE IP") >= 0 || text.indexOf("CONNECT") >= 0) {
    connectionState = 1;
    #ifdef DEBUG
    Serial.println("🔄 Modem CONNECTED. Menunggu STARTDT_ACT...");
    #endif
  }
} 

void IEC104Slave::handleIFrame(const byte* buf, byte len) {
  // Ambil NS dan NR dari frame yang diterima
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

  // Perbarui rxSeq untuk acknowledgment berikutnya
  rxSeq = ns + 1;

  // Perbarui txSeq mengikuti NR master
  txSeq = nr;

  // Proses sesuai Type Identification (TI)
  byte ti = buf[6];
  switch (ti) {
    case 100: handleGI(buf, len); break;
    case 103: handleRTC(buf, len); break;  // <--- Ini untuk TI 103
    case 46:  handleTI46(&buf[6], len - 6); break;
    default: break;
  }
}

void IEC104Slave::handleSFrame(const byte* buf, byte len) {
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
  }
  else if (buf[2] == 0x43){
    lastTestAct = millis();     // ← reset timer
    sendUFormat(0x83);
  }
}

void IEC104Slave::handleGI(const byte* buf, byte len) {
  byte ca_lo = buf[10];
  byte ca_hi = buf[11];
  updateInputs();

  byte actCon[] = {0x64, 0x01, 0x07, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(actCon, sizeof(actCon)); delay(50);

  byte ti1[] = {
    0x01, 0x02, 0x14, 0x00, ca_lo, ca_hi,
    0xE9, 0x03, 0x00, remote ? 0 : 1,
    0xEA, 0x03, 0x00, gfd ? 1 : 0
  };
  sendIFrame(ti1, sizeof(ti1)); delay(50);

  byte ti3[] = {
    0x03, 0x01, 0x14, 0x00, ca_lo, ca_hi,
    0xF8, 0x2A, 0x00, cb
  };
  sendIFrame(ti3, sizeof(ti3)); delay(50);

  byte term[] = {0x64, 0x01, 0x0A, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(term, sizeof(term));
}

void IEC104Slave::handleTI46(const byte* d, byte len) {

  // ✅ Perbaikan: IOA pakai 3 byte (little-endian)
  uint32_t ioa = d[6] | (d[7] << 8) | (d[8] << 16);
  byte sco = d[9] & 0x03;

  #ifdef DEBUG
  Serial.print("[TI46] IOA: ");
  Serial.print(ioa);
  Serial.print(" | SCO: ");
  Serial.println(sco);
  #endif

  if (!remote) {
    #ifdef DEBUG
    Serial.println("[REJECT] Mode LOCAL");
    #endif
  } else if ((sco == 1 && cb == 1) || (sco == 2 && cb == 2)) {
    #ifdef DEBUG
    Serial.println("[REJECT] Status CB sudah sesuai");
    #endif
  } else if (sco == 1 || sco == 2) {
    triggerRelay(sco);
  } else {
    #ifdef DEBUG
    Serial.println("[REJECT] SCO tidak valid");
    #endif
  }

  // ✅ Kirim ACK (COT = 7)
  byte ack[] = {
    0x2E, 0x01, 0x07, 0x00, 0x01, 0x00,
    (byte)(ioa & 0xFF), (byte)((ioa >> 8) & 0xFF), (byte)((ioa >> 16) & 0xFF),
    sco
  };
  sendIFrame(ack, sizeof(ack)); delay(50);

  // ✅ Kirim Termination (COT = 10)
  byte term[] = {
    0x2E, 0x01, 0x0A, 0x00, 0x01, 0x00,
    (byte)(ioa & 0xFF), (byte)((ioa >> 8) & 0xFF), (byte)((ioa >> 16) & 0xFF),
    sco
  };
  sendIFrame(term, sizeof(term));
}

void IEC104Slave::handleRTC(const byte* buf, byte len) {
  const byte* time = &buf[15];

  setRTCFromCP56(time);

  // Kirim balasan ACK – TI 103, COT = 7
  byte ack[16] = {
    0x67, 0x01,       // TI = 103, VSQ = 1
    0x07, 0x00,       // COT = 7
    0x01, 0x00,       // CA = 1
    0x00, 0x00, 0x00  // IOA = 0
  };

  memcpy(&ack[9], time, 7);  // Salin CP56Time2a
  sendIFrame(ack, sizeof(ack));
}

void IEC104Slave::setRTCFromCP56(const byte* time) {
  uint16_t ms     = time[0] | (time[1] << 8);     // byte 0-1: millisecond
  byte minute     = time[2] & 0x3F;               // byte 2: 0-5 bit minute
  byte hour       = time[3] & 0x1F;               // byte 3: 0-4 bit hour
  byte date       = time[4] & 0x1F;               // byte 4: 0-4 bit date
  byte dow        = (time[4] >> 5) & 0x07;        // byte 4: 5-7 bit day of week
  byte month      = time[5] & 0x0F;               // byte 5: 0-3 bit month
  uint16_t year       = 2000 + (time[6] & 0x7F);      // byte 6: 0-6 bit year

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
  byte pdu[17] = {ti, 1, 6, 0, 1, 0, (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, value};
  memcpy(&pdu[10], cp56, 7);
  sendIFrame(pdu, sizeof(pdu));
}

void IEC104Slave::triggerRelay(byte command) {
  if (command == 1) {
    digitalWrite(PIN_OPEN, HIGH);
    delay(800);
    digitalWrite(PIN_OPEN, LOW);
  } else if (command == 2) {
    digitalWrite(PIN_CLOSE, HIGH);
    delay(800);
    digitalWrite(PIN_CLOSE, LOW);
  }
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
  delay(100);
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