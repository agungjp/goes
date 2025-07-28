/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.7.0  |
|==============================================================================|
|  Implementation file for the IEC104Slave class.                              |
|  Refactored to a non-blocking, queue-based cooperative scheduler architecture.|
|==============================================================================*/

#include "IEC104Slave.h"
#include <string.h>

// ===================================================================
// === SETUP & INISIALISASI ==========================================
// ===================================================================

IEC104Slave::IEC104Slave(Stream* serial) {
  modem = serial;
}

void IEC104Slave::begin() {
  rtc.begin();
  setupPins();
  restartModem();
  setupConnection();

  #ifdef DEBUG
  Serial.println(F("Inisialisasi status awal (menunggu stabilitas input)..."));
  #endif

  task_updateAndCheckInputs(true);
  delay(JEDA_DEBOUNCE + 100);
  task_updateAndCheckInputs(true);

  #ifdef DEBUG
  Serial.println(F("Inisialisasi selesai. Siap beroperasi."));
  #endif
}

void IEC104Slave::setupPins() {
  pinMode(PIN_REMOTE, INPUT_PULLUP);
  pinMode(PIN_GFD, INPUT_PULLUP);
  pinMode(PIN_CB_OPEN, INPUT_PULLUP);
  pinMode(PIN_CB_CLOSE, INPUT_PULLUP);
  pinMode(PIN_MODEM_POWER, OUTPUT);
  pinMode(PIN_RELAY_OPEN, OUTPUT);
  pinMode(PIN_RELAY_CLOSE, OUTPUT);
  digitalWrite(PIN_RELAY_OPEN, LOW);
  digitalWrite(PIN_RELAY_CLOSE, LOW);
  digitalWrite(PIN_MODEM_POWER, LOW);
}

void IEC104Slave::restartModem() {
    #ifdef DEBUG
    Serial.println(F(" Merestart modem..."));
    #endif
    digitalWrite(PIN_MODEM_POWER, HIGH);
    delay(200);
    digitalWrite(PIN_MODEM_POWER, LOW);
    delay(1000);
}

void IEC104Slave::setupConnection() {
  #ifdef DEBUG
  Serial.println(F("Memulai koneksi modem..."));
  #endif
  delay(10000);
  modem->println("ATE0"); delay(500);
  modem->println("AT"); delay(2000);
  modem->println("AT+CGATT=1"); delay(2000);
  modem->println("AT+CIPMODE=1"); delay(2000);
  modem->println("AT+CSTT=\"m2mplnapd\""); delay(2000);
  modem->println("AT+CIICR"); delay(5000);
  modem->println("AT+CIFSR"); delay(2000);
  modem->println("AT+CIPSERVER=1,2404"); delay(2000);
  while (modem->available()) modem->read();
}

// ===================================================================
// === FUNGSI UTAMA / SCHEDULER ======================================
// ===================================================================

void IEC104Slave::run() {
  task_listenToModem();
  task_processIncomingQueue();
  task_updateAndCheckInputs();
  task_updateTimedRelays();
  task_sendFromOutgoingQueue();
  task_checkWatchdog();
}

// ===================================================================
// === IMPLEMENTASI FUNGSI-FUNGSI TUGAS (TASKS) ======================
// ===================================================================

void IEC104Slave::task_listenToModem() {
    static char modemTextBuffer[50];
    static int modemTextIdx = 0;

    if (parserState != WAITING_FOR_START && millis() - lastByteTime > FRAME_ASSEMBLY_TIMEOUT) {
        #ifdef DEBUG
        Serial.println(F("[PARSER] Frame timeout, parser direset."));
        #endif
        parserState = WAITING_FOR_START;
    }

    while (modem->available()) {
        lastByteTime = millis();
        byte c = modem->read();

        switch (parserState) {
            case WAITING_FOR_START:
                if (c == 0x68) {
                    frameBuffer[0] = c;
                    bytesRead = 1;
                    parserState = WAITING_FOR_LENGTH;
                } else {
                    if (isprint(c) || c == '\r' || c == '\n') {
                        if (c == '\n' || c == '\r') {
                            if (modemTextIdx > 0) {
                                modemTextBuffer[modemTextIdx] = '\0';
                                handleModemText(modemTextBuffer);
                                modemTextIdx = 0;
                            }
                        } else {
                            if (modemTextIdx < sizeof(modemTextBuffer) - 1) modemTextBuffer[modemTextIdx++] = (char)c;
                        }
                    }
                }
                break;
            case WAITING_FOR_LENGTH:
                frameLength = c;
                frameBuffer[1] = c;
                bytesRead = 2;
                if (frameLength > MAX_BUFFER_SIZE - 2 || frameLength == 0) {
                    parserState = WAITING_FOR_START;
                    break;
                }
                parserState = READING_PAYLOAD;
                break;
            case READING_PAYLOAD:
                frameBuffer[bytesRead] = c;
                bytesRead++;
                if (bytesRead >= (frameLength + 2)) {
                    int nextTail = (rxQueueTail + 1) % QUEUE_SIZE;
                    if (nextTail != rxQueueHead) {
                        memcpy(rxQueue[rxQueueTail], frameBuffer, bytesRead);
                        rxQueueLength[rxQueueTail] = bytesRead;
                        rxQueueTail = nextTail;
                    }
                    parserState = WAITING_FOR_START;
                }
                break;
        }
    }
}

void IEC104Slave::task_processIncomingQueue() {
    if (rxQueueHead == rxQueueTail) return;

    byte* frame = rxQueue[rxQueueHead];
    int len = rxQueueLength[rxQueueHead];

    if ((frame[2] & 0x01) == 0) handleIFrame(frame, len);
    else if ((frame[2] & 0x03) == 0x01) handleSFrame(frame, len);
    else if ((frame[2] & 0x03) == 0x03) handleUFrame(frame, len);

    rxQueueHead = (rxQueueHead + 1) % QUEUE_SIZE;
}

void IEC104Slave::task_updateAndCheckInputs(bool isInitializing) {
    if (!isInitializing) {
        prevRemote = remote;
        prevGFD = gfd;
        prevCB = cb;
    }

    bool remoteSaatIni = (digitalRead(PIN_REMOTE) == LOW);
    if (remoteSaatIni != kondisiPinRemoteSebelumnya) waktuPerubahanRemote = millis();
    kondisiPinRemoteSebelumnya = remoteSaatIni;
    if (millis() - waktuPerubahanRemote >= JEDA_DEBOUNCE) remote = remoteSaatIni;

    bool gfdSaatIni = digitalRead(PIN_GFD);
    if (gfdSaatIni != kondisiPinGfdSebelumnya) waktuPerubahanGfd = millis();
    kondisiPinGfdSebelumnya = gfdSaatIni;
    if (millis() - waktuPerubahanGfd >= JEDA_DEBOUNCE) gfd = gfdSaatIni;
    
    int cbSaatIni;
    bool cbOpen = (digitalRead(PIN_CB_OPEN) == LOW);
    bool cbClose = (digitalRead(PIN_CB_CLOSE) == LOW);
    if (cbOpen && !cbClose) cbSaatIni = 1;
    else if (!cbOpen && cbClose) cbSaatIni = 2;
    else if (!cbOpen && !cbClose) cbSaatIni = 0;
    else cbSaatIni = 3;
    if (cbSaatIni != kondisiPinCbSebelumnya) waktuPerubahanCb = millis();
    kondisiPinCbSebelumnya = cbSaatIni;
    if (millis() - waktuPerubahanCb >= JEDA_DEBOUNCE) cb = cbSaatIni;

    if (isInitializing) {
        prevRemote = remote; prevGFD = gfd; prevCB = cb;
        kondisiPinRemoteSebelumnya = remote; kondisiPinGfdSebelumnya = gfd; kondisiPinCbSebelumnya = cb;
        return;
    }

    if (remote != prevRemote) {
        #ifdef DEBUG
        Serial.print(F("[COS] Remote: ")); Serial.print(prevRemote); Serial.print(F(" -> ")); Serial.println(remote);
        #endif
        uint8_t cp56[7]; convertCP56Time2a(cp56);
        byte asdu[] = {30, 1, 3, 0, 1, 0, 0xE9, 0x03, 0x00, (byte)(remote ? 1 : 0), 0,0,0,0,0,0,0};
        memcpy(&asdu[10], cp56, 7);
        queueOutgoingFrame(asdu, sizeof(asdu), false);
    }
    if (gfd != prevGFD) {
        #ifdef DEBUG
        Serial.print(F("[COS] GFD: ")); Serial.print(prevGFD); Serial.print(F(" -> ")); Serial.println(gfd);
        #endif
        uint8_t cp56[7]; convertCP56Time2a(cp56);
        byte asdu[] = {30, 1, 3, 0, 1, 0, 0xEA, 0x03, 0x00, (byte)(gfd ? 1 : 0), 0,0,0,0,0,0,0};
        memcpy(&asdu[10], cp56, 7);
        queueOutgoingFrame(asdu, sizeof(asdu), false);
    }
    if (cb != prevCB) {
        #ifdef DEBUG
        Serial.print(F("[COS] CB: ")); Serial.print(prevCB); Serial.print(F(" -> ")); Serial.println(cb);
        #endif
        uint8_t cp56[7]; convertCP56Time2a(cp56);
        byte asdu[] = {31, 1, 3, 0, 1, 0, 0xF8, 0x2A, 0x00, (byte)cb, 0,0,0,0,0,0,0};
        memcpy(&asdu[10], cp56, 7);
        queueOutgoingFrame(asdu, sizeof(asdu), false);
    }
}

void IEC104Slave::task_sendFromOutgoingQueue() {
    static unsigned long lastSendTime = 0;
    
    if (millis() - lastSendTime < INTERFRAME_DELAY) return;

    byte* asduToSend = nullptr;
    int lenToSend = 0;

    if (txQueueHeadHighPrio != txQueueTailHighPrio) {
        asduToSend = txQueueHighPrio[txQueueHeadHighPrio];
        lenToSend = txQueueLengthHighPrio[txQueueHeadHighPrio];
        txQueueHeadHighPrio = (txQueueHeadHighPrio + 1) % QUEUE_SIZE;
    } 
    else if (txQueueHeadLowPrio != txQueueTailLowPrio) {
        asduToSend = txQueueLowPrio[txQueueHeadLowPrio];
        lenToSend = txQueueLengthLowPrio[txQueueHeadLowPrio];
        txQueueHeadLowPrio = (txQueueHeadLowPrio + 1) % QUEUE_SIZE;
    }

    if (asduToSend != nullptr) {
        buildAndSendIFrame(asduToSend, lenToSend);
        lastSendTime = millis();
    }
}

void IEC104Slave::task_updateTimedRelays() {
  if (relayOpenAktif && (millis() - waktuMulaiRelayOpen >= DURASI_RELAY)) {
    digitalWrite(PIN_RELAY_OPEN, LOW);
    relayOpenAktif = false;
  }
  if (relayCloseAktif && (millis() - waktuMulaiRelayClose >= DURASI_RELAY)) {
    digitalWrite(PIN_RELAY_CLOSE, LOW);
    relayCloseAktif = false;
  }
}

// ===================================================================
// === FUNGSI-FUNGSI HANDLER & HELPER ================================
// ===================================================================

void IEC104Slave::queueOutgoingFrame(const byte* asdu, byte len, bool highPriority) {
    if (highPriority) {
        int nextTail = (txQueueTailHighPrio + 1) % QUEUE_SIZE;
        if (nextTail != txQueueHeadHighPrio) {
            memcpy(txQueueHighPrio[txQueueTailHighPrio], asdu, len);
            txQueueLengthHighPrio[txQueueTailHighPrio] = len;
            txQueueTailHighPrio = nextTail;
        }
    } else {
        int nextTail = (txQueueTailLowPrio + 1) % QUEUE_SIZE;
        if (nextTail != txQueueHeadLowPrio) {
            memcpy(txQueueLowPrio[txQueueTailLowPrio], asdu, len);
            txQueueLengthLowPrio[txQueueTailLowPrio] = len;
            txQueueTailLowPrio = nextTail;
        }
    }
}

void IEC104Slave::handleIFrame(const byte* buf, int len) {
    uint16_t ns = (buf[3] << 7) | (buf[2] >> 1);
    rxSeq = ns + 1;
    txSeq = (buf[5] << 7) | (buf[4] >> 1);

    const byte* asdu = &buf[6];
    switch (asdu[0]) {
        case 100: handleGI(asdu); break;
        case 103: handleRTC(asdu); break;
        case 46:  handleTI46(asdu); break;
    }
}

void IEC104Slave::handleUFrame(const byte* buf, int len) {
    if (buf[2] == 0x07) {
        connectionState = 2;
        sendUFormat(0x0B);
    } else if (buf[2] == 0x43) {
        lastTestAct = millis();
        sendUFormat(0x83);
    } else if (buf[2] == 0x13) {
        connectionState = 1;
        sendUFormat(0x23);
    }
}

void IEC104Slave::handleGI(const byte* asdu) {
    byte ca_lo = asdu[4];
    byte ca_hi = asdu[5];

    byte actCon[] = {100, 1, 7, 0, ca_lo, ca_hi, 0,0,0, 20};
    queueOutgoingFrame(actCon, sizeof(actCon), true);

    uint8_t cp56[7];
    convertCP56Time2a(cp56);
    byte asdu_remote[] = {30, 1, 20, 0, ca_lo, ca_hi, 0xE9, 0x03, 0x00, (byte)(remote ? 1 : 0), 0,0,0,0,0,0,0};
    memcpy(&asdu_remote[10], cp56, 7);
    queueOutgoingFrame(asdu_remote, sizeof(asdu_remote), false);

    byte asdu_gfd[] = {30, 1, 20, 0, ca_lo, ca_hi, 0xEA, 0x03, 0x00, (byte)(gfd ? 1 : 0), 0,0,0,0,0,0,0};
    memcpy(&asdu_gfd[10], cp56, 7);
    queueOutgoingFrame(asdu_gfd, sizeof(asdu_gfd), false);

    byte asdu_cb[] = {31, 1, 20, 0, ca_lo, ca_hi, 0xF8, 0x2A, 0x00, (byte)cb, 0,0,0,0,0,0,0};
    memcpy(&asdu_cb[10], cp56, 7);
    queueOutgoingFrame(asdu_cb, sizeof(asdu_cb), false);

    byte term[] = {100, 1, 10, 0, ca_lo, ca_hi, 0,0,0, 20};
    queueOutgoingFrame(term, sizeof(term), true);
}

void IEC104Slave::handleTI46(const byte* asdu) {
    uint32_t ioa = (uint32_t)asdu[6] | ((uint32_t)asdu[7] << 8) | ((uint32_t)asdu[8] << 16);
    byte sco = asdu[9];

    bool execute = false;
    if (remote && (sco == 1 || sco == 2)) {
      if ((sco == 1 && cb != 1) || (sco == 2 && cb != 2)) {
        execute = true;
      }
    }
    
    byte ack[] = {46, 1, 7, 0, asdu[4], asdu[5], asdu[6], asdu[7], asdu[8], sco};
    queueOutgoingFrame(ack, sizeof(ack), true);
    
    if (execute) {
        triggerRelay(sco);
    }
    
    byte term[] = {46, 1, 10, 0, asdu[4], asdu[5], asdu[6], asdu[7], asdu[8], sco};
    queueOutgoingFrame(term, sizeof(term), true);
}

void IEC104Slave::triggerRelay(byte command) {
  if (relayOpenAktif || relayCloseAktif) return;
  if (command == 1) {
    digitalWrite(PIN_RELAY_OPEN, HIGH);
    relayOpenAktif = true;
    waktuMulaiRelayOpen = millis();
  } else if (command == 2) {
    digitalWrite(PIN_RELAY_CLOSE, HIGH);
    relayCloseAktif = true;
    waktuMulaiRelayClose = millis();
  }
}

void IEC104Slave::buildAndSendIFrame(const byte* asdu, byte len) {
    byte frame[MAX_BUFFER_SIZE];
    if (6 + len > MAX_BUFFER_SIZE) return;

    frame[0] = 0x68;
    frame[1] = len + 4;
    frame[2] = txSeq << 1;
    frame[3] = 0;
    frame[4] = rxSeq << 1;
    frame[5] = 0;
    memcpy(&frame[6], asdu, len);

    sendFrame(frame, 6 + len);
    txSeq++;
}

void IEC104Slave::sendUFormat(byte controlByte) {
    byte frame[6] = {0x68, 0x04, controlByte, 0x00, 0x00, 0x00};
    sendFrame(frame, 6);
}

void IEC104Slave::sendFrame(const byte* frame, byte len) {
    #ifdef DEBUG
    Serial.print(F("Slave: "));
    if ((frame[2] & 0x01) == 0) {
        Serial.print(F("NS(")); Serial.print((frame[3] << 7) | (frame[2] >> 1));
        Serial.print(F(") NR(")); Serial.print((frame[5] << 7) | (frame[4] >> 1));
        Serial.print(F("). <- "));
    } else {
        Serial.print(F("(U/S-Format). <- "));
    }
    for (int i = 0; i < len; i++) {
        if (frame[i] < 0x10) Serial.print("0");
        Serial.print(frame[i], HEX); Serial.print(" ");
    }
    Serial.println();
    #endif
    modem->write(frame, len);
}

void IEC104Slave::handleModemText(const char* text) {
    #ifdef DEBUG
    Serial.print(F("MODEM: ")); Serial.println(text);
    #endif
    if (strstr(text, "CONNECT") != NULL) connectionState = 1;
    else if (strstr(text, "CLOSED") != NULL) connectionState = 0;
}

void IEC104Slave::handleSFrame(const byte* buf, int len) {
  txSeq = (buf[5] << 7) | (buf[4] >> 1);
  #ifdef DEBUG
  Serial.print(F("Master (S-Format): NR(")); Serial.print(txSeq); Serial.println("). -> ");
  #endif
}

void IEC104Slave::handleRTC(const byte* asdu) {
  const byte* time = &asdu[9];
  setRTCFromCP56(time);

  byte ack[16] = {103, 1, 7, 0, asdu[4], asdu[5], 0,0,0};
  memcpy(&ack[9], time, 7);
  queueOutgoingFrame(ack, sizeof(ack), true);
}

void IEC104Slave::setRTCFromCP56(const byte* time) {
  uint16_t ms = time[0] | (time[1] << 8);
  byte minute = time[2] & 0x3F;
  byte hour   = time[3] & 0x1F;
  byte date   = time[4] & 0x1F;
  byte month  = time[5] & 0x0F;
  uint16_t year = 2000 + (time[6] & 0x7F);
  
  rtc.setDate(date, month, year-2000);
  rtc.setTime(hour, minute, ms / 1000);
}

void IEC104Slave::convertCP56Time2a(uint8_t* buffer) {
  Time t = rtc.getTime();
  uint16_t ms = t.sec * 1000;
  buffer[0] = ms & 0xFF;
  buffer[1] = (ms >> 8) & 0xFF;
  buffer[2] = t.min & 0x3F;
  buffer[3] = t.hour & 0x1F;
  buffer[4] = ((t.dow << 5) | (t.date & 0x1F));
  buffer[5] = t.mon & 0x0F;
  buffer[6] = (t.year - 2000) & 0x7F;
}

void IEC104Slave::task_checkWatchdog() {
  if (millis() - lastTestAct > TEST_ACT_TIMEOUT) {
    #ifdef DEBUG
    Serial.println(F("\n!!! Watchdog triggered: No TESTFR_ACT received. Resetting... !!!"));
    #endif
    wdt_enable(WDTO_8S); // Enable watchdog with 8s timeout
    while(1) {}          // Loop indefinitely until watchdog resets
  }
}