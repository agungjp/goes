#include "iec104.h"
#include <string.h>

void GSMConnection::begin(Stream* serial) {
  modemSerial = serial;
  delay(1000);
}

void GSMConnection::setupConnection() {
  delay(15000);
  modemSerial->println("AT");               updateSerial();
  delay(5000);
  modemSerial->println("AT+CGATT=1");       updateSerial();
  modemSerial->println("AT+CIPMODE=1");     updateSerial();
  modemSerial->println("AT+CSTT=\"m2mplnapd\""); updateSerial();
  modemSerial->println("AT+CIICR");         updateSerial();
  modemSerial->println("AT+CIFSR");         updateSerial();
  modemSerial->println("AT+CIPSERVER=1,2404"); updateSerial();
}

void GSMConnection::setupInputPins() {
  pinMode(pinLocalRemote, INPUT_PULLUP);
  pinMode(pinGFD, INPUT_PULLUP);
  pinMode(pinCB_1, INPUT_PULLUP);
  pinMode(pinCB_2, INPUT_PULLUP);
}

void GSMConnection::updateInputStatus() {
  statusLocalRemote = digitalRead(pinLocalRemote) == LOW;
  statusGFD = digitalRead(pinGFD);
  bool cb1 = digitalRead(pinCB_1);
  bool cb2 = digitalRead(pinCB_2);
  if (!cb1 && !cb2) statusCB = 0;
  else if (cb1 && !cb2) statusCB = 1;
  else if (!cb1 && cb2) statusCB = 2;
  else statusCB = 3;
}

void GSMConnection::updateSerial() {
  delay(2000);
  while (modemSerial->available()) Serial.write(modemSerial->read());
  while (Serial.available()) modemSerial->write(Serial.read());
}

void GSMConnection::listen() {
  static byte ByteArray[256];
  int i = 0;

  if (modemSerial->available()) {
    while (modemSerial->available()) {
      delayMicroseconds(1500);
      ByteArray[i++] = modemSerial->read();
    }

    Serial.print("Master : ");
    for (int j = 0; j < i; j++) {
      if (ByteArray[j] < 0x10) Serial.print("0");
      Serial.print(ByteArray[j], HEX); Serial.print(" ");
    }
    Serial.println();

    // ✅ I-format (NS/NR)
    if (i >= 6 && ByteArray[0] == 0x68 && (ByteArray[2] & 0x01) == 0) {
      uint16_t nsMaster = (ByteArray[3] << 7) | (ByteArray[2] >> 1);
      uint16_t nrMaster = (ByteArray[5] << 7) | (ByteArray[4] >> 1);
      rxSequence = nsMaster + 1;
      Serial.print("[INFO] NS(Master): "); Serial.print(nsMaster);
      Serial.print(" | NR(Master): "); Serial.println(nrMaster);
    }

    // ✅ S-format (ACK-only)
    else if (i == 6 && ByteArray[0] == 0x68 && ByteArray[2] == 0x01) {
      uint16_t nr = (ByteArray[5] << 7) | (ByteArray[4] >> 1);
      Serial.print("[INFO] S-format ACK → Master ACK hingga NS = ");
      Serial.println(nr - 1);
      
      // ✅ Sinkronisasi txSequence
      if (nr > txSequence) {
        Serial.println("[SYNC] txSequence disesuaikan ke NR master");
      }
      txSequence = nr;
    }

    // ✅ U-format
    if (i >= 6 && ByteArray[0] == 0x68 && ByteArray[1] == 0x04) {
      byte controlByte = ByteArray[2];
      if (controlByte == 0x07) sendUFormat(0x0B); // STARTDT_CON
      else if (controlByte == 0x43) sendUFormat(0x83); // TEST CON
    }

    // ✅ General Interrogation
    if (i >= 15 && ByteArray[6] == 0x64) {
      byte ca_lo = ByteArray[10];
      byte ca_hi = ByteArray[11];
      updateInputStatus();

      byte actCon[] = {0x64,0x01,0x07,0x00,ca_lo,ca_hi,0x00,0x00,0x00,0x14};
      sendIFrame(actCon, sizeof(actCon)); delay(50);

      byte ti1[] = {
        0x01,0x02,0x14,0x00,ca_lo,ca_hi,
        0xE9,0x03,0x00, statusLocalRemote ? 1 : 0,
        0xEA,0x03,0x00, statusGFD ? 1 : 0
      };
      sendIFrame(ti1, sizeof(ti1)); delay(50);

      byte ti3[] = {
        0x03,0x01,0x14,0x00,ca_lo,ca_hi,
        0xF8,0x2A,0x00, statusCB
      };
      sendIFrame(ti3, sizeof(ti3)); delay(50);

      byte actTerm[] = {0x64,0x01,0x0A,0x00,ca_lo,ca_hi,0x00,0x00,0x00,0x14};
      sendIFrame(actTerm, sizeof(actTerm));
    }
  }
}

void GSMConnection::sendUFormat(byte controlByte) {
  byte response[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
  modemSerial->write(response, 6);

  Serial.print("Slave  : ");
  for (int i = 0; i < 6; i++) {
    if (response[i] < 0x10) Serial.print("0");
    Serial.print(response[i], HEX); Serial.print(" ");
  }
  Serial.println();
}

void GSMConnection::sendIFrame(const byte* payload, byte len) {
  byte frame[6 + len];
  frame[0] = 0x68;
  frame[1] = len;
  frame[2] = txSequence << 1;
  frame[3] = txSequence >> 7;
  frame[4] = rxSequence << 1;
  frame[5] = rxSequence >> 7;
  memcpy(&frame[6], payload, len);
  modemSerial->write(frame, 6 + len);
  txSequence++;

  Serial.print("Slave  : ");
  for (int i = 0; i < 6 + len; i++) {
    if (frame[i] < 0x10) Serial.print("0");
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
  Serial.println();
  Serial.print("[TX] NS: "); Serial.print(txSequence - 1);
  Serial.print(" | NR: "); Serial.println(rxSequence);
}
