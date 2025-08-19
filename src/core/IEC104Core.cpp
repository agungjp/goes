#include "IEC104Core.h"
#include "goes_config.h"
#include "config/config_global.h" // For TEST_ACT_TIMEOUT
#include "hal/HardwareManager.h" // For HardwareManager
#include "hal/PinESP32.h" // Direct include for pin definitions

IEC104Core::IEC104Core(IEC104Communicator* comm, HardwareManager* hw) : _comm(comm), _hw(hw) {
    // Constructor
}

void IEC104Core::run() {
    // Main loop for core logic
    updateInputs();
    if (connectionState == 2) { // STATE_CONNECTED
        checkCOS();
    }
    handlePendingRelayTI46();
    // Check for communication timeout
    if (millis() - _comm->getLastFrameReceived() > TEST_ACT_TIMEOUT) { // Need to expose getLastFrameReceived from IEC104Communicator
        softwareReset();
    }
}

void IEC104Core::processReceivedFrame(const byte* buf, byte len) {
    // This function will be called by IEC104Communicator when a frame is received
    // Based on the TI, call appropriate handler
    byte ti = buf[6];
    switch (ti) {
        case 100: handleGI(buf, len); break;
        case 103: handleRTC(buf, len); break;
        case 46:  handleTI46(&buf[6], len - 6); break;
        default: break;
    }
}

void IEC104Core::updateInputs() {
  prevRemote1 = remote1;
  prevRemote2 = remote2;
  prevGFD     = gfd;
  prevSupply  = supply;
  prevCB1     = cb1;
  prevCB2     = cb2;

  bool rawRemote1 = _hw->getPin(IOA_REMOTE1) == LOW; // IOA 1001
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
  bool rawRemote2 = _hw->getPin(IOA_REMOTE2) == LOW; // IOA 1003
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
  gfd = _hw->getPin(IOA_GFD) == LOW;         // IOA 1002
  supply = _hw->getPin(IOA_SUPPLY);   // IOA 1004
  bool cb1_open  = _hw->getPin(PIN_CB1_OPEN);
  bool cb1_close = _hw->getPin(PIN_CB1_CLOSE);
  cb1 = getDoublePoint(cb1_open, cb1_close);  // IOA 11000
  bool cb2_open  = _hw->getPin(PIN_CB2_OPEN);
  bool cb2_close = _hw->getPin(PIN_CB2_CLOSE);
  cb2 = getDoublePoint(cb2_open, cb2_close);  // IOA 11001
}

uint8_t IEC104Core::getDoublePoint(uint8_t open, uint8_t close) {
  if (!open && !close) return 0; // Unknown
  else if (!open && close) return 1; // Open
  else if (open && !close) return 2; // Close
  else return 3; // Unknown
}

void IEC104Core::checkCOS() {
  if (relayBusy) return;
  // Serial.println("[DEBUG] Memeriksa COS...");
  if (gfd != prevGFD) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevGFD); Serial.print(" → "); Serial.println(gfd);
    #endif
    sendTimestamped(30, IOA_GFD, gfd ? 1 : 0); // Need to expose sendTimestamped from IEC104Communicator
    prevGFD = gfd;
  }
  if (supply != prevSupply) {
    #ifdef DEBUG
    Serial.print("[DEBUG] GFD berubah: "); Serial.print(prevSupply); Serial.print(" → "); Serial.println(supply);
    #endif
    sendTimestamped(30, IOA_SUPPLY, supply ? 1 : 0);
    prevSupply = supply;
  }
  if (remote1 != prevRemote1) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Remote 1 berubah: "); Serial.print(prevRemote1); Serial.print(" → "); Serial.println(remote1);
    #endif
    sendTimestamped(30, IOA_REMOTE1, remote1 ? 0 : 1);
    prevRemote1 = remote1;
  }
  if (cb1 != prevCB1) {
    if(remote1){return;}
    #ifdef DEBUG
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB1); Serial.print(" → "); Serial.println(cb1);
    #endif
    sendTimestamped(31, IOA_CB1, cb1);
    prevCB1 = cb1;
  }
  if (remote2 != prevRemote2) {
    #ifdef DEBUG
    Serial.print("[DEBUG] Remote 2 berubah: "); Serial.print(prevRemote2); Serial.print(" → "); Serial.println(remote2);
    #endif
    sendTimestamped(30, IOA_REMOTE2, remote2 ? 0 : 1);
    prevRemote2 = remote2;
  }
  if (cb2 != prevCB2) {
    if(remote2){return;}
    #ifdef DEBUG
    Serial.print("[DEBUG] CB berubah: "); Serial.print(prevCB2); Serial.print(" → "); Serial.println(cb2);
    #endif
    sendTimestamped(31, IOA_CB2, cb2);
    prevCB2 = cb2;
  }
}

void IEC104Core::handleGI(const byte* buf, byte len) {
  #ifdef DEBUG
    Serial.println("Ti 100 (General Introgation)");
  #endif
  byte ca_lo = buf[10];
  byte ca_hi = buf[11];
  updateInputs();

  byte actCon[] = {0x64, 0x01, 0x07, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  _comm->sendIFrame(actCon, sizeof(actCon)); delay(50);

  byte ti1[] = {
    0x01, 0x04, 0x14, 0x00, ca_lo, ca_hi,
    // Remote/Local cell 1 (IOA 1001)
    0xE9, 0x03, 0x00, (byte)(remote1 ? 0 : 1),
    // GFD (IOA 1002)
    0xEA, 0x03, 0x00, (byte)(gfd ? 1 : 0),
    // Remote/Local cell 2 (IOA 1003)
    0xEB, 0x03, 0x00, (byte)(remote2 ? 0 : 1),
    // Supply (IOA 1004)
    0xEC, 0x03, 0x00, (byte)(supply ? 1 : 0)
  };
  _comm->sendIFrame(ti1, sizeof(ti1)); delay(50);

  byte ti3[] = {
    0x03, 0x02, 0x14, 0x00, ca_lo, ca_hi,
    // CB1 (IOA 11000)
    0xF8, 0x2A, 0x00, cb1,
    // CB2 (IOA 11001)
    0xF9, 0x2A, 0x00, cb2
  };
  _comm->sendIFrame(ti3, sizeof(ti3)); delay(50);

  byte term[] = {0x64, 0x01, 0x0A, 0x00, ca_lo, ca_hi, 0x00, 0x00, 0x00, 0x14};
  _comm->sendIFrame(term, sizeof(term));
  delay(200);
}

void IEC104Core::handleTI46(const byte* d, byte len) {
  uint32_t ioa = d[6] | (d[7] << 8) | ((uint32_t)d[8] << 16);
  byte sco = d[9] & 0x03;

  #ifdef DEBUG
  Serial.print("[TI46] IOA: "); Serial.print(ioa);
  Serial.print(" | SCO: "); Serial.println(sco);
  #endif
  
  // Validasi remote & relay idle
  bool isRemote = (ioa == IOA_RELAY_CB1 && remote1) || (ioa == IOA_RELAY_CB2 && remote2);
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
  if (ioa == IOA_RELAY_CB1) {
    if (sco == 1) _hw->setPin(PIN_CB1_OUT_OPEN, HIGH);
    else if (sco == 2) _hw->setPin(PIN_CB1_OUT_CLOSE, HIGH);
  } else if (ioa == IOA_RELAY_CB2) {
    if (sco == 1) _hw->setPin(PIN_CB2_OUT_OPEN, HIGH);
    else if (sco == 2) _hw->setPin(PIN_CB2_OUT_CLOSE, HIGH);
  }
}

void IEC104Core::handlePendingRelayTI46() {
  if (!relayBusy) return;

  // Matikan relay jika sudah >800 ms
  if (millis() - relayStart > 800) {
    if (ti46_pending_ioa == IOA_RELAY_CB1) {
      _hw->setPin(PIN_CB1_OUT_OPEN, LOW);
      _hw->setPin(PIN_CB1_OUT_CLOSE, LOW);
    }
    if (ti46_pending_ioa == IOA_RELAY_CB2) {
      _hw->setPin(PIN_CB2_OUT_OPEN, LOW);
      _hw->setPin(PIN_CB2_OUT_CLOSE, LOW);
    }
  }

  // Cek status CB, kirim ke master bila sudah sesuai perintah
  // updateInputs(); // pastikan status terupdate
  bool cb_ok = false;
  if (ti46_pending_ioa == IOA_RELAY_CB1) cb_ok = (cb1 == ti46_pending_sco);
  if (ti46_pending_ioa == IOA_RELAY_CB2) cb_ok = (cb2 == ti46_pending_sco);

  if (cb_ok && !cot7_sent) {
    // 1. Kirim COT=7 (ACK)
    byte ack[] = {
      0x2E, 0x01, 0x07, 0x00, 0x01, 0x00,
      (byte)(ti46_pending_ioa & 0xFF), (byte)((ti46_pending_ioa >> 8) & 0xFF), (byte)((ti46_pending_ioa >> 16) & 0xFF),
      ti46_pending_sco
    };
    _comm->sendIFrame(ack, sizeof(ack)); delay(50);

    // 2. Kirim TI 31 (status CB terbaru)
    if (ti46_pending_ioa == IOA_RELAY_CB1) sendTimestamped(31, IOA_CB1, cb1);
    else if (ti46_pending_ioa == IOA_RELAY_CB2) sendTimestamped(31, IOA_CB2, cb2);
    delay(50);

    // 3. Kirim Termination COT=10
    byte term[] = {
      0x2E, 0x01, 0x0A, 0x00, 0x01, 0x00,
      (byte)(ti46_pending_ioa & 0xFF), (byte)((ti46_pending_ioa >> 8) & 0xFF), (byte)((ti46_pending_ioa >> 16) & 0xFF),
      ti46_pending_sco
    };
    _comm->sendIFrame(term, sizeof(term)); delay(50);

    cot7_sent = true;
    relayBusy = false;

    #ifdef DEBUG
    Serial.println("TI 46 Termination - Selesai (event-driven)");
    #endif
  }
}

void IEC104Core::handleRTC(const byte* buf, byte len) {
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
  _comm->sendIFrame(ack, sizeof(ack));
}

void IEC104Core::setRTCFromCP56(const byte* time) {
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

  rtc.setDoW(dow);
  rtc.setDate(date);
  rtc.setMonth(month);
  rtc.setYear(year % 100);
  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(ms / 1000);
}

void IEC104Core::convertCP56Time2a(uint8_t* buffer) {
  bool h12, PM_time;
  byte second = rtc.getSecond();
  byte minute = rtc.getMinute();
  byte hour = rtc.getHour(h12, PM_time);
  byte date = rtc.getDate();
  byte dow = rtc.getDoW();
  byte month = rtc.getMonth(h12);
  byte year = rtc.getYear();

  uint16_t ms = second * 1000;
  buffer[0] = ms & 0xFF;
  buffer[1] = (ms >> 8) & 0xFF;
  buffer[2] = minute & 0x3F;
  buffer[3] = hour & 0x1F;
  buffer[4] = ((dow & 0x07) << 5) | (date & 0x1F);
  buffer[5] = month & 0x0F;
  buffer[6] = year & 0x7F;
}

void IEC104Core::sendTimestamped(byte ti, uint16_t ioa, byte value) {
  uint8_t cp56[7];
  convertCP56Time2a(cp56);
  byte pdu[17] = {ti, 1, 3, 0, 1, 0, (byte)(ioa & 0xFF), (byte)(ioa >> 8), 0x00, value};
  memcpy(&pdu[10], cp56, 7);
  _comm->sendIFrame(pdu, sizeof(pdu));
}

void IEC104Core::softwareReset() {
  #ifdef DEBUG
    Serial.println(F("⚠️  Tidak ada Komunikasi dari master >5 menit → software reset..."));
  #endif
  _hw->softwareReset();
}
