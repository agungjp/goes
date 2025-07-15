#include "ModemManager.h"

ModemManager::ModemManager() {}

void ModemManager::begin() {
  // Setup serial modem (atau Ethernet)
}

void ModemManager::update() {
  // Parsing text USR (CLOSED, CONNECT), reconnect, dsb.
}

bool ModemManager::available() {
  // Return apakah ada data masuk dari modem
  return false;
}

int ModemManager::read() {
  // Ambil data RX dari modem
  return -1;
}

void ModemManager::write(const uint8_t* buf, size_t len) {
  // Kirim data ke modem
}
