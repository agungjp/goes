#include "IEC104Protocol.h"

IEC104Protocol::IEC104Protocol()
  : ioaMap(nullptr), eventQueue(nullptr), modem(nullptr) {}

void IEC104Protocol::begin(IOAMapManager* ioaMap, EventQueue* eventQueue, ModemManager* modem) {
  this->ioaMap = ioaMap;
  this->eventQueue = eventQueue;
  this->modem = modem;
}

void IEC104Protocol::listenFrame() {
  // Contoh: parsing RX frame dari modem, panggil handler TI/COT, dsb.
  // FSM parsing IEC104 lengkap bisa diimplementasikan di sini.
  if (TI == TESTFR_ACT) {
    // ...
    if (testActCb) testActCb(); // Panggil callback, update timer!
  }
}

void IEC104Protocol::sendCOS(uint32_t ioa, uint8_t value) {
  // Bangun frame COS (misal TI 30/31) lalu kirim ke master via modem->write()
}
