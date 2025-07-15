#ifndef IEC104PROTOCOL_H
#define IEC104PROTOCOL_H

#include <Arduino.h>
#include "IOAMapManager.h"
#include "EventQueue.h"
#include "ModemManager.h"

class IEC104Protocol {
public:
  IEC104Protocol();
  void begin(IOAMapManager* ioaMap, EventQueue* eventQueue, ModemManager* modem);
  void listenFrame();      // Parsing RX frame IEC104 (I/S/U)
  void sendCOS(uint32_t ioa, uint8_t value);  // Kirim frame COS ke master
  // Tambah handler TI/COT, dst

private:
  IOAMapManager* ioaMap;
  EventQueue* eventQueue;
  ModemManager* modem;
  // Tambah state/protocol variable lain sesuai kebutuhan
};

#endif
