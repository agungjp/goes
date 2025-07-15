#ifndef IOAMAPMANAGER_H
#define IOAMAPMANAGER_H

#include <Arduino.h>

#define TYPE_DI  0
#define TYPE_DO  1
#define TYPE_AI  2

struct IOAEntry {
  uint32_t ioa;
  uint8_t  type;
  uint8_t  value;
  uint8_t  prevValue;
};

class IOAMapManager {
public:
  IOAMapManager();
  void addIOA(uint32_t ioa, uint8_t type, uint8_t value = 0);
  int findIndex(uint32_t ioa);
  uint8_t getValue(uint32_t ioa);
  void setValue(uint32_t ioa, uint8_t value);
  void updateInputs();

  IOAEntry map[20];
  uint8_t count;
};

#endif
