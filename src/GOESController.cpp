#include "GOESController.h"

void GOESController::begin() {
  // Tambah mapping IOA <-> pin untuk DI/DO/DS
  ioaMap.addIOA(1001, TYPE_DI, 0);   // R/L cell 1 (pin 2)
  ioaMap.addIOA(1002, TYPE_DI, 0);   // GFD (pin 3)
  ioaMap.addIOA(1003, TYPE_DI, 0);   // R/L cell 2 (pin 14/A0)
  ioaMap.addIOA(1004, TYPE_DI, 0);   // Supply (pin 12)
  ioaMap.addIOA(11000, TYPE_DI, 0);  // CB1 (double status)
  ioaMap.addIOA(11001, TYPE_DI, 0);  // CB2 (double status)
  ioaMap.addIOA(23000, TYPE_DO, 0);  // Double Cmd CB1
  ioaMap.addIOA(23001, TYPE_DO, 0);  // Double Cmd CB2

  // Setup pinMode untuk semua pin di bawah:
  pinMode(2, INPUT_PULLUP);    // R/L cell 1
  pinMode(3, INPUT_PULLUP);    // GFD
  pinMode(14, INPUT_PULLUP);   // R/L cell 2 (A0)
  pinMode(12, INPUT_PULLUP);   // Supply
  pinMode(4, INPUT_PULLUP);    // CB1 open
  pinMode(5, INPUT_PULLUP);    // CB1 close
  pinMode(15, INPUT_PULLUP);   // CB2 open (A1)
  pinMode(16, INPUT_PULLUP);   // CB2 close (A2)
  pinMode(6, OUTPUT);          // Out open CB1
  pinMode(7, OUTPUT);          // Out close CB1
  pinMode(17, OUTPUT);         // Out open CB2 (A3)
  pinMode(11, OUTPUT);         // Out close CB2
  pinMode(9, OUTPUT);         // modem power

  modem.begin();      // Setup modem atau ethernet
  relay.begin();
  protocol.begin(&ioaMap, &eventQueue, &modem); // Kirim pointer untuk akses ke manager lain
}

void GOESController::run() {
  ioaMap.updateInputs();
  // Deteksi COS, push ke eventQueue
  for (uint8_t i = 0; i < ioaMap.count; i++) {
    if (ioaMap.map[i].value != ioaMap.map[i].prevValue) {
      Event e;
      e.type = 0; // COS
      e.ioa = ioaMap.map[i].ioa;
      e.value = ioaMap.map[i].value;
      eventQueue.push(e);
      ioaMap.map[i].prevValue = ioaMap.map[i].value;
    }
  }
  modem.update();
  protocol.listenFrame();
  eventQueue.process(&protocol);
}
