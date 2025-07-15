#ifndef GOESCONTROLLER_H
#define GOESCONTROLLER_H

#include "IOAMapManager.h"
#include "EventQueue.h"
#include "ModemManager.h"
#include "IEC104Protocol.h"
#include "RelayController.h"

typedef void (*TestActCallback)();

class GOESController {
public:
  void begin();
  void run();
  void setTestActCallback(TestActCallback cb) { testActCb = cb; }
private:
  IOAMapManager ioaMap;
  EventQueue eventQueue;
  ModemManager modem;
  IEC104Protocol protocol;
  RelayController relay;
  unsigned long lastInputsUpdate = 0;
  TestActCallback testActCb = nullptr;
};

#endif
