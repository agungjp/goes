
#include "IEC104Slave.h"
#include <string.h>

#define DEBUG

IEC104Slave::IEC104Slave(Stream* serial) {
  modem = serial;
}

void IEC104Slave::begin() {
  rtc.begin();
  setupPins();
  setupConnection();

  updateInputs();
  prevRemote = remote;
  prevGFD = gfd;
  prevCB = cb;
}

void IEC104Slave::run() {
  updateInputs();
  checkCOS();
  listen();
  checkCOS();
}

void IEC104Slave::setupPins() {
  pinMode(PIN_REMOTE, INPUT_PULLUP);
  pinMode(PIN_GFD, INPUT_PULLUP);
  pinMode(PIN_CB1, INPUT_PULLUP);
  pinMode(PIN_CB2, INPUT_PULLUP);
  pinMode(PIN_OPEN, OUTPUT);
  pinMode(PIN_CLOSE, OUTPUT);
  digitalWrite(PIN_OPEN, LOW);
  digitalWrite(PIN_CLOSE, LOW);
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

void IEC104Slave::updateSerial() {
  delay(2000);
  while (modem->available()) Serial.write(modem->read());
  while (Serial.available()) modem->write(Serial.read());
}

void IEC104Slave::updateInputs() {
  prevRemote = remote;
  prevGFD = gfd;
  prevCB = cb;

  remote = digitalRead(PIN_REMOTE) == HIGH; // HIGH = Local, LOW = Remote
  gfd = digitalRead(PIN_GFD);

  bool cb1 = digitalRead(PIN_CB1);
  bool cb2 = digitalRead(PIN_CB2);
  if (!cb1 && !cb2) cb = 0;
  else if (cb1 && !cb2) cb = 1;
  else if (!cb1 && cb2) cb = 2;
  else cb = 3;
}

void IEC104Slave::checkCOS() {
  // Serial.println("[DEBUG] Memeriksa COS...");
  if (remote != prevRemote) {
    Serial.print("[DEBUG] Remote berubah: "); Serial.print(prevRemote); Serial.print(" → "); Serial.println(remote);
    sendTimestamped(30, 1001, remote ? 1 : 0);
    prevRemote = remote;
  }
  if (gfd != prevGFD) {
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevGFD); Serial.print(" → "); Serial.println(gfd);
    sendTimestamped(30, 1002, gfd ? 1 : 0);
    prevGFD = gfd;
  }
  if (cb != prevCB) {
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB); Serial.print(" → "); Serial.println(cb);
    sendTimestamped(31, 11000, cb);
    prevCB = cb;
  }

  if (gfd != prevGFD) {
    sendTimestamped(30, 1002, gfd ? 1 : 0);
    prevGFD = gfd;
  }
  if (cb != prevCB) {
    sendTimestamped(31, 11000, cb);
    prevCB = cb;
  }
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

void IEC104Slave::setRTCFromCP56(const byte* time) {
  uint16_t ms = time[0] | (time[1] << 8);
  byte minute = time[2] & 0x3F;
  byte hour = time[3] & 0x1F;
  byte date = time[4] & 0x1F;
  byte dow = (time[4] >> 5) & 0x07;
  byte month = time[5] & 0x0F;
  byte year = 2000 + (time[6] & 0x7F);
  rtc.setDOW(dow);
  rtc.setDate(date, month, year);
  rtc.setTime(hour, minute, ms / 1000);
}

void IEC104Slave::sendTimestamped(byte ti, uint16_t ioa, byte value) {
  uint8_t cp56[7];
  convertCP56Time2a(cp56);
  byte pdu[17] = {ti, 1, 6, 0, 1, 0, (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, value};
  memcpy(&pdu[10], cp56, 7);
  sendIFrame(pdu, sizeof(pdu));
}

void IEC104Slave::sendIFrame(const byte* payload, byte len) {
  byte frame[6 + len];
  frame[0] = 0x68;
  frame[1] = len;
  frame[2] = txSeq << 1;
  frame[3] = txSeq >> 7;
  frame[4] = rxSeq << 1;
  frame[5] = rxSeq >> 7;
  memcpy(&frame[6], payload, len);
  modem->write(frame, 6 + len);
  txSeq++;
#ifdef DEBUG
  Serial.print("Slave : NS ("); Serial.print(txSeq); Serial.print(") NR("); Serial.print(rxSeq); Serial.print(").    → ");
  for (int i = 0; i < 6 + len; i++) {
    if (frame[i] < 0x10) Serial.print("0");
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
  Serial.println();
#endif
}

void IEC104Slave::sendUFormat(byte controlByte) {
  byte r[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
  modem->write(r, 6);
  Serial.print("Slave : NS ("); Serial.print(txSeq); Serial.print(") NR("); Serial.print(rxSeq); Serial.print(").    → ");
  for (int i = 0; i < 6; i++) {
    if (r[i] < 0x10) Serial.print("0");
    Serial.print(r[i], HEX); Serial.print(" ");
  }
  Serial.println();
}


void IEC104Slave::listen() {
  static byte buf[MAX_BUFFER];
  int len = 0;

  if (modem->available()) {
    while (modem->available() && len < MAX_BUFFER) {
      delayMicroseconds(1500);
      buf[len++] = modem->read();
    }

    Serial.print("Master : NS ("); Serial.print(ns); Serial.print(") NR("); Serial.print(nr); Serial.print(")     → ");
    for (int i = 0; i < len; i++) {
      if (buf[i] < 0x10) Serial.print("0");
      Serial.print(buf[i], HEX); Serial.print(" ");
    }
    Serial.println();

    if (len >= 6 && buf[0] == 0x68 && (buf[2] & 0x01) == 0) {
      handleIFrame(buf, len);
    } else if (len == 6 && buf[0] == 0x68 && buf[2] == 0x01) {
      handleSFrame(buf);
    } else if (len >= 6 && buf[0] == 0x68 && buf[1] == 0x04) {
      handleUFrame(buf);
    }
  }
}

void IEC104Slave::handleIFrame(const byte* buf, byte len) {
  // Ambil NS dan NR dari frame yang diterima
  ns = (buf[3] << 7) | (buf[2] >> 1);
  nr = (buf[5] << 7) | (buf[4] >> 1);

  // Perbarui rxSeq untuk acknowledgment berikutnya
  rxSeq = ns + 1;

  // Perbarui txSeq mengikuti NR master
  txSeq = nr;

#ifdef DEBUG
  Serial.print("[DEBUG] Master NS: ");
  Serial.print(ns);
  Serial.print(" | Master NR: ");
  Serial.println(nr);
#endif

  // Proses sesuai Type Identification (TI)
  byte ti = buf[6];
  switch (ti) {
    case 100: handleGI(buf, len); break;
    case 103: handleRTC(&buf[12]); break;
    case 46:  handleTI46(&buf[6], len - 6); break;
    default: break;
  }
}

void IEC104Slave::handleSFrame(const byte* buf) {
  nr = (buf[5] << 7) | (buf[4] >> 1);
  txSeq = nr;
}

void IEC104Slave::handleUFrame(const byte* buf) {
  if (buf[2] == 0x07) sendUFormat(0x0B);
  else if (buf[2] == 0x43) sendUFormat(0x83);
}

void IEC104Slave::handleGI(const byte* buf, byte len) {
  byte ca_lo = buf[10];
  byte ca_hi = buf[11];
  updateInputs();

  byte actCon[] = {0x64, 0x01, 0x07, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  sendIFrame(actCon, sizeof(actCon)); delay(50);

  byte ti1[] = {
    0x01, 0x02, 0x14, 0x00, ca_lo, ca_hi,
    0xE9, 0x03, 0x00, remote ? 1 : 0,
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
  if (len < 11) return;
  uint16_t ioa = d[6] | (d[7] << 8);
  byte sco = d[9] & 0x03;

  if (!remote) {
    Serial.println("[REJECT] LOCAL mode");
  } else if ((sco == 1 && cb == 1) || (sco == 2 && cb == 2)) {
    Serial.println("[REJECT] CB status sudah sesuai");
  } else if (sco == 1 || sco == 2) {
    triggerRelay(sco);
  } else {
    Serial.println("[REJECT] SCO tidak valid");
  }

  byte ack[11] = {0x2E, 0x01, 0x07, 0x00, 0x01, 0x00,
    (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, sco};
  sendIFrame(ack, sizeof(ack)); delay(50);

  byte term[11] = {0x2E, 0x01, 0x0A, 0x00, 0x01, 0x00,
    (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, sco};
  sendIFrame(term, sizeof(term));
}

void IEC104Slave::handleRTC(const byte* time) {
  setRTCFromCP56(time);
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
