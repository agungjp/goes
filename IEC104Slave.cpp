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
    remote1(false), prevRemote1(false),
    remote2(false), prevRemote2(false),
    gfd(false), prevGFD(false),
    supply(false), prevSupply(false),
    cb1(0), prevCB1(0),
    cb2(0), prevCB2(0),
    remote1Stable(false), remote2Stable(false),
    remote1DebounceActive(false), remote2DebounceActive(false),
    remote1DebounceStart(0), remote2DebounceStart(0),
    lastFrameReceived(0),
    state(STATE_DISCONNECTED) 
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
  restartModem();
  setupConnection();
  flushModem(); 
  updateInputs();
  lastFrameReceived = millis();
  prevRemote1 = remote1;
  prevRemote2 = remote2;
  prevGFD     = gfd;
  prevSupply  = supply;
  prevCB1     = cb1;
  prevCB2     = cb2;
  remote1Stable = remote1;
  remote2Stable = remote2;
}

void IEC104Slave::run() {
  updateInputs();
  if (state == STATE_CONNECTED){
    checkCOS();
  } 
  listen();
  handlePendingRelayTI46();
  if (millis() - lastFrameReceived > TEST_ACT_TIMEOUT) {
    softwareReset();
  }
}

void IEC104Slave::setupPins() {
  // Input pin
  pinMode(PIN_GFD, INPUT_PULLUP);
  pinMode(PIN_SUPPLY, INPUT_PULLUP);
  pinMode(PIN_REMOTE1, INPUT_PULLUP);
  pinMode(PIN_CB1_OPEN, INPUT_PULLUP);
  pinMode(PIN_CB1_CLOSE, INPUT_PULLUP);
  pinMode(PIN_REMOTE2, INPUT_PULLUP);
  pinMode(PIN_CB2_OPEN, INPUT_PULLUP);
  pinMode(PIN_CB2_CLOSE, INPUT_PULLUP);
  // Output pin
  pinMode(PIN_MODEM_POWER, OUTPUT);
  pinMode(PIN_CB1_OUT_OPEN, OUTPUT);
  pinMode(PIN_CB1_OUT_CLOSE, OUTPUT);
  pinMode(PIN_CB2_OUT_OPEN, OUTPUT);
  pinMode(PIN_CB2_OUT_CLOSE, OUTPUT);
  // Inisialisasi
  digitalWrite(PIN_CB1_OUT_OPEN, LOW);
  digitalWrite(PIN_CB1_OUT_CLOSE, LOW);
  digitalWrite(PIN_CB2_OUT_OPEN, LOW);
  digitalWrite(PIN_CB2_OUT_CLOSE, LOW);
  digitalWrite(PIN_MODEM_POWER, LOW);
}

void IEC104Slave::restartModem() {
  #ifdef DEBUG
  Serial.println("🔁 Restarting modem via D9...");
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
  prevRemote1 = remote1;
  prevRemote2 = remote2;
  prevGFD     = gfd;
  prevSupply  = supply;
  prevCB1     = cb1;
  prevCB2     = cb2;

  bool rawRemote1 = digitalRead(PIN_REMOTE1) == LOW; // IOA 1001
  if (rawRemote1 != remote1Stable) {
    remote1DebounceStart = millis();
    remote1DebounceActive = true;
    remote1Stable = rawRemote1;
  }
  if (remote1DebounceActive && (millis() - remote1DebounceStart >= 1500)) {
    remote1 = remote1Stable;
    remote1DebounceActive = false;
    if (!remote1DebounceReady) {
      prevRemote1 = remote1;       // Samakan agar tidak trigger COS pertama kali
      remote1DebounceReady = true; // Arming, berikutnya normal
    }
  }
  bool rawRemote2 = digitalRead(PIN_REMOTE2) == LOW; // IOA 1003
  if (rawRemote2 != remote2Stable) {
    remote2DebounceStart = millis();
    remote2DebounceActive = true;
    remote2Stable = rawRemote2;
  }
  if (remote2DebounceActive && (millis() - remote2DebounceStart >= 1500)) {
    remote2 = remote2Stable;
    remote2DebounceActive = false;
    if (!remote2DebounceReady) {
      prevRemote2 = remote2;
      remote2DebounceReady = true;
    }
  }
  gfd = digitalRead(PIN_GFD) == LOW;         // IOA 1002
  supply = digitalRead(PIN_SUPPLY);   // IOA 1004
  bool cb1_open  = digitalRead(PIN_CB1_OPEN);
  bool cb1_close = digitalRead(PIN_CB1_CLOSE);
  cb1 = getDoublePoint(cb1_open, cb1_close);  // IOA 11000
  bool cb2_open  = digitalRead(PIN_CB2_OPEN);
  bool cb2_close = digitalRead(PIN_CB2_CLOSE);
  cb2 = getDoublePoint(cb2_open, cb2_close);  // IOA 11001
}

// Fungsi helper double point IEC104 (Open = 1, Close = 2, Unknown = 0/3)
uint8_t IEC104Slave::getDoublePoint(uint8_t open, uint8_t close) {
  if (!open && !close) return 0; // Unknown
  else if (!open && close) return 1; // Open
  else if (open && !close) return 2; // Close
  else return 3; // Unknown
}

void IEC104Slave::checkCOS() {
  if (relayBusy) return;
  // Serial.println("[DEBUG] Memeriksa COS...");
  if (gfd != prevGFD) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevGFD); Serial.print(" → "); Serial.println(gfd);
    #endif
    sendTimestamped(30, 1002, gfd ? 1 : 0);
    prevGFD = gfd;
  }
  if (supply != prevSupply) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevSupply); Serial.print(" → "); Serial.println(supply);
    #endif
    sendTimestamped(30, 1004, supply ? 1 : 0);
    prevSupply = supply;
  }
  if (remote1 != prevRemote1) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Remote 1 berubah: "); Serial.print(prevRemote1); Serial.print(" → "); Serial.println(remote1);
    #endif
    sendTimestamped(30, 1001, remote1 ? 0 : 1);
    prevRemote1 = remote1;
  }
  if (cb1 != prevCB1) {
    if(remote1){return;}
    #ifdef DEBUG
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB1); Serial.print(" → "); Serial.println(cb1);
    #endif
    sendTimestamped(31, 11000, cb1);
    prevCB1 = cb1;
  }
  if (remote2 != prevRemote2) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Remote 2 berubah: "); Serial.print(prevRemote2); Serial.print(" → "); Serial.println(remote2);
    #endif
    sendTimestamped(30, 1003, remote2 ? 0 : 1);
    prevRemote2 = remote2;
  }
  if (cb2 != prevCB2) {
    if(remote2){return;}
    #ifdef DEBUG
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB2); Serial.print(" → "); Serial.println(cb2);
    #endif
    sendTimestamped(31, 11001, cb2);
    prevCB2 = cb2;
  }
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
      unsigned long tStart = millis();
      while (modem->available() < 1) {
        if (millis() - tStart > 200) {
          // Timeout: frame gagal, lanjut
          #ifdef DEBUG
          Serial.println("[TIMEOUT] Wait length byte");
          #endif
          return;
        }
      }
      byte length = modem->read();
      buf[1] = length;

      // overflow guard
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
        if (timeout) break; // keluar dari for loop jika timeout
        buf[2 + i] = modem->read();
      }

      if (timeout) {
        // Jika timeout saat baca data frame, frame dibuang
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
  lastFrameReceived = millis();
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
  sendIFrame(actCon, sizeof(actCon)); delay(50);

  byte ti1[] = {
    0x01, 0x04, 0x14, 0x00, ca_lo, ca_hi,
    // Remote/Local cell 1 (IOA 1001)
    0xE9, 0x03, 0x00, remote1 ? 0 : 1,
    // GFD (IOA 1002)
    0xEA, 0x03, 0x00, gfd ? 1 : 0,
    // Remote/Local cell 2 (IOA 1003)
    0xEB, 0x03, 0x00, remote2 ? 0 : 1,
    // Supply (IOA 1004)
    0xEC, 0x03, 0x00, supply ? 1 : 0
  };
  sendIFrame(ti1, sizeof(ti1)); delay(50);

  byte ti3[] = {
    0x03, 0x02, 0x14, 0x00, ca_lo, ca_hi,
    // CB1 (IOA 11000)
    0xF8, 0x2A, 0x00, cb1,
    // CB2 (IOA 11001)
    0xF9, 0x2A, 0x00, cb2
  };
  sendIFrame(ti3, sizeof(ti3)); delay(50);

  byte term[] = {0x64, 0x01, 0x0A, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(term, sizeof(term));
  delay(200);
}

void IEC104Slave::handleTI46(const byte* d, byte len) {
  uint32_t ioa = d[6] | (d[7] << 8) | (d[8] << 16);
  byte sco = d[9] & 0x03;

  #ifdef DEBUG
  Serial.print("[TI46] IOA: "); Serial.print(ioa);
  Serial.print(" | SCO: "); Serial.println(sco);
  #endif
  
  // Validasi remote & relay idle
  bool isRemote = (ioa == 23000 && remote1) || (ioa == 23001 && remote2);
  if (!isRemote) {
    Serial.println("[TI46] Gagal eksekusi: Bukan remote!");
    return;
  }
  if (relayBusy) {
    Serial.println("[TI46] Gagal eksekusi: Relay masih aktif!");
    return;
  }

  // Simpan state pending
  relayBusy = true;
  ti46_pending_ioa = ioa;
  ti46_pending_sco = sco;
  relayStart = millis();
  cot7_sent = false;

  // Trigger relay (ON), tidak pakai delay!
  if (ioa == 23000) {
    if (sco == 1) digitalWrite(PIN_CB1_OUT_OPEN, HIGH);
    else if (sco == 2) digitalWrite(PIN_CB1_OUT_CLOSE, HIGH);
  } else if (ioa == 23001) {
    if (sco == 1) digitalWrite(PIN_CB2_OUT_OPEN, HIGH);
    else if (sco == 2) digitalWrite(PIN_CB2_OUT_CLOSE, HIGH);
  }
}

void IEC104Slave::handlePendingRelayTI46() {
  if (!relayBusy) return;

  // Matikan relay jika sudah >800 ms
  if (millis() - relayStart > 800) {
    if (ti46_pending_ioa == 23000) {
      digitalWrite(PIN_CB1_OUT_OPEN, LOW);
      digitalWrite(PIN_CB1_OUT_CLOSE, LOW);
    }
    if (ti46_pending_ioa == 23001) {
      digitalWrite(PIN_CB2_OUT_OPEN, LOW);
      digitalWrite(PIN_CB2_OUT_CLOSE, LOW);
    }
  }

  // Cek status CB, kirim ke master bila sudah sesuai perintah
  // updateInputs(); // pastikan status terupdate
  bool cb_ok = false;
  if (ti46_pending_ioa == 23000) cb_ok = (cb1 == ti46_pending_sco);
  if (ti46_pending_ioa == 23001) cb_ok = (cb2 == ti46_pending_sco);

  if (cb_ok && !cot7_sent) {
    // 1. Kirim COT=7 (ACK)
    byte ack[] = {
      0x2E, 0x01, 0x07, 0x00, 0x01, 0x00,
      (byte)(ti46_pending_ioa & 0xFF), (byte)((ti46_pending_ioa >> 8) & 0xFF), (byte)((ti46_pending_ioa >> 16) & 0xFF),
      ti46_pending_sco
    };
    sendIFrame(ack, sizeof(ack)); delay(50);

    // 2. Kirim TI 31 (status CB terbaru)
    if (ti46_pending_ioa == 23000) sendTimestamped(31, 11000, cb1);
    else if (ti46_pending_ioa == 23001) sendTimestamped(31, 11001, cb2);
    delay(50);

    // 3. Kirim Termination COT=10
    byte term[] = {
      0x2E, 0x01, 0x0A, 0x00, 0x01, 0x00,
      (byte)(ti46_pending_ioa & 0xFF), (byte)((ti46_pending_ioa >> 8) & 0xFF), (byte)((ti46_pending_ioa >> 16) & 0xFF),
      ti46_pending_sco
    };
    sendIFrame(term, sizeof(term)); delay(50);

    cot7_sent = true;
    relayBusy = false;

    #ifdef DEBUG
    Serial.println("TI 46 Termination - Selesai (event-driven)");
    #endif
  }
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
  wdt_enable(WDTO_15MS); // Atur watchdog ke 15ms
  while(1) {}            // Tunggu watchdog reset chip
}

void IEC104Slave::flushModem() {
  // Buang semua data yang belum terbaca di modem buffer
  while (modem->available()) modem->read();
  delay(100);
  // Jika pakai Serial debug, buang juga
  #ifdef DEBUG
  while (Serial.available()) Serial.read();
  #endif
}