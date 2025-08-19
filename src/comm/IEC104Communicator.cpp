#include "IEC104Communicator.h"
#include "config/config_global.h"

IEC104Communicator::IEC104Communicator(CommInterface* comm) : _comm(comm) {
    // Constructor
}

void IEC104Communicator::listen() {
  static byte buf[MAX_BUFFER];
  static String modemText = ""; // This should probably be handled by ModemCommunicator

  int len = 0;
  
  while (_comm->available()) {
    char c = _comm->read();

    // Teks dari modem
    if (c == '\n' || c == '\r') {
      if (modemText.length() > 0) {
        // modemText.trim(); // This should be handled by ModemCommunicator
        // handleModemText(modemText); // This should be handled by ModemCommunicator
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
      while (_comm->available() < 1) {
        if (millis() - tStart > 200) {
          // Timeout: frame gagal, lanjut
          #ifdef DEBUG
          Serial.println("[TIMEOUT] Wait length byte");
          #endif
          return;
        }
      }
      byte length = _comm->read();
      buf[1] = length;

      // overflow guard
      if (length > MAX_BUFFER - 2) return;

      bool timeout = false;
      for (int i = 0; i < length; i++) {
        unsigned long t1 = millis();
        while (!_comm->available()) {
          if (millis() - t1 > 200) {
            timeout = true;
            #ifdef DEBUG
            Serial.print("[TIMEOUT] Wait data byte ke-"); Serial.println(i);
            #endif
            break;
          }
        }
        if (timeout) break; // keluar dari for loop jika timeout
        buf[2 + i] = _comm->read();
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

void IEC104Communicator::handleIFrame(const byte* buf, byte len) {
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
    // case 100: handleGI(buf, len); break; // Will be handled by core logic
    // case 103: handleRTC(buf, len);  break;  // Will be handled by core logic
    // case 46:  handleTI46(&buf[6], len - 6); break; // Will be handled by core logic
    default: break;
  }
}

void IEC104Communicator::handleSFrame(const byte* buf, byte len) {
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

void IEC104Communicator::handleUFrame(const byte* buf, byte len) {
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

void IEC104Communicator::sendIFrame(const byte* payload, byte len) {
  byte frame[6 + len];
  frame[0] = 0x68;
  frame[1] = len+4;
  frame[2] = txSeq << 1;
  frame[3] = txSeq >> 7;
  frame[4] = rxSeq << 1;
  frame[5] = rxSeq >> 7;
  memcpy(&frame[6], payload, len);
  _comm->write(frame, 6 + len);

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

void IEC104Communicator::sendUFormat(byte controlByte) {
  byte r[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
  _comm->write(r, 6);

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

unsigned long IEC104Communicator::getLastFrameReceived() const {
    return lastFrameReceived;
}