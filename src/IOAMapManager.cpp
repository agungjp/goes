#include "IOAMapManager.h"

IOAMapManager::IOAMapManager() : count(0) {}

void IOAMapManager::addIOA(uint32_t ioa, uint8_t type, uint8_t value) {
  if (count < 20) {
    map[count].ioa = ioa;
    map[count].type = type;
    map[count].value = value;
    map[count].prevValue = value;
    count++;
  }
}

int IOAMapManager::findIndex(uint32_t ioa) {
  for (uint8_t i = 0; i < count; i++) {
    if (map[i].ioa == ioa) return i;
  }
  return -1;
}

uint8_t IOAMapManager::getValue(uint32_t ioa) {
  int idx = findIndex(ioa);
  return (idx >= 0) ? map[idx].value : 0;
}

void IOAMapManager::setValue(uint32_t ioa, uint8_t value) {
  int idx = findIndex(ioa);
  if (idx >= 0) map[idx].value = value;
}

void IOAMapManager::updateInputs() {
  // Update single DI
  map[0].prevValue = map[0].value;
  map[0].value = digitalRead(2);   // IOA 1001 - R/L cell 1

  map[1].prevValue = map[1].value;
  map[1].value = digitalRead(3);   // IOA 1002 - GFD

  map[2].prevValue = map[2].value;
  map[2].value = digitalRead(14);  // IOA 1003 - R/L cell 2 (A0)

  map[3].prevValue = map[3].value;
  map[3].value = digitalRead(12);  // IOA 1004 - Supply

  // Update double status CB 1 (open/close)
  uint8_t cb1_open = digitalRead(4);
  uint8_t cb1_close = digitalRead(5);
  map[4].prevValue = map[4].value;
  if (!cb1_open && cb1_close)      map[4].value = 1;   // Open
  else if (cb1_open && !cb1_close) map[4].value = 2;   // Close
  else                             map[4].value = 0;   // Unknown

  // Update double status CB 2 (open/close)
  uint8_t cb2_open = digitalRead(15); // A1
  uint8_t cb2_close = digitalRead(16); // A2
  map[5].prevValue = map[5].value;
  if (!cb2_open && cb2_close)      map[5].value = 1;
  else if (cb2_open && !cb2_close) map[5].value = 2;
  else                             map[5].value = 0;
}
