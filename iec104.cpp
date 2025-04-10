#include "iec104.h"
#include <string.h>

void GSMConnection::begin(Stream* serial) {
  modemSerial = serial;
  delay(1000);
  rtc.begin();
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
  prevLocalRemote = statusLocalRemote;
  prevGFD = statusGFD;
  prevCB = statusCB;

  statusLocalRemote = digitalRead(pinLocalRemote) == LOW;
  statusGFD = digitalRead(pinGFD);

  bool cb1 = digitalRead(pinCB_1);
  bool cb2 = digitalRead(pinCB_2);
  if (!cb1 && !cb2) statusCB = 0;
  else if (cb1 && !cb2) statusCB = 1;
  else if (!cb1 && cb2) statusCB = 2;
  else statusCB = 3;
}

void GSMConnection::checkAndSendCOS() {
  if (statusLocalRemote != prevLocalRemote) {
    Serial.println("[COS] Remote/Local berubah");
    sendTimestampedData(30, 1001, statusLocalRemote ? 1 : 0);
  }
  if (statusGFD != prevGFD) {
    Serial.println("[COS] GFD berubah");
    sendTimestampedData(30, 1002, statusGFD ? 1 : 0);
  }
  if (statusCB != prevCB) {
    Serial.println("[COS] Status CB berubah");
    sendTimestampedData(31, 11000, statusCB);
  }
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

    if (i >= 6 && ByteArray[0] == 0x68 && (ByteArray[2] & 0x01) == 0) {
      uint16_t nsMaster = (ByteArray[3] << 7) | (ByteArray[2] >> 1);
      uint16_t nrMaster = (ByteArray[5] << 7) | (ByteArray[4] >> 1);
      rxSequence = nsMaster + 1;
      Serial.print("[INFO] NS(Master): "); Serial.print(nsMaster);
      Serial.print(" | NR(Master): "); Serial.println(nrMaster);
    }

    else if (i == 6 && ByteArray[0] == 0x68 && ByteArray[2] == 0x01) {
      uint16_t nr = (ByteArray[5] << 7) | (ByteArray[4] >> 1);
      Serial.print("[INFO] S-format ACK → Master ACK hingga NS = ");
      Serial.println(nr - 1);
      if (nr > txSequence) {
        Serial.println("[SYNC] txSequence disesuaikan ke NR master");
      }
      txSequence = nr;
    }

    if (i >= 6 && ByteArray[0] == 0x68 && ByteArray[1] == 0x04) {
      byte controlByte = ByteArray[2];
      if (controlByte == 0x07) sendUFormat(0x0B);
      else if (controlByte == 0x43) sendUFormat(0x83);
    }

    if (i >= 15 && ByteArray[6] == 0x64) {
      byte ca_lo = ByteArray[10];
      byte ca_hi = ByteArray[11];
      updateInputStatus();

      byte actCon[] = {0x64,0x01,0x07,0x00,ca_lo,ca_hi,0x00,0x00,0x00,0x14};
      sendIFrame(actCon, sizeof(actCon)); delay(50);

      sendTimestampedData(30, 1001, statusLocalRemote ? 1 : 0); delay(50);
      sendTimestampedData(30, 1002, statusGFD ? 1 : 0); delay(50);
      sendTimestampedData(31, 11000, statusCB); delay(50);

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

void GSMConnection::convertToCP56Time2a(uint8_t* buffer) {
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

void GSMConnection::sendTimestampedData(byte ti, uint16_t ioa, byte value) {
  uint8_t cp56[7];
  convertToCP56Time2a(cp56);

  byte payload[17];
  payload[0] = ti;
  payload[1] = 1;
  payload[2] = 6; payload[3] = 0;
  payload[4] = 1; payload[5] = 0;
  payload[6] = ioa & 0xFF;
  payload[7] = (ioa >> 8) & 0xFF;
  payload[8] = 0x00;
  payload[9] = value;
  memcpy(&payload[10], cp56, 7);

  sendIFrame(payload, sizeof(payload));
}
