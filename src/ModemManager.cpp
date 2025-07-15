#include "ModemManager.h"

ModemManager::ModemManager(uint8_t rxPin, uint8_t txPin) : _rxPin(rxPin), _txPin(txPin){
  modemSerial = new SoftwareSerial(_rxPin, _txPin);
}

void ModemManager::begin(long baud) {
  // Setup serial modem (atau Ethernet)
  modemSerial->begin(baud);
}

void ModemManager::update() {
  // Parsing text USR (CLOSED, CONNECT), reconnect, dsb.
}

bool ModemManager::available() {
  // Return apakah ada data masuk dari modem
  return modemSerial->available();
}

int ModemManager::read() {
  // Ambil data RX dari modem
  return modemSerial->read();
}

void ModemManager::write(const uint8_t* buf, size_t len) {
  // Kirim data ke modem
  modemSerial->write(buf, len);
}
