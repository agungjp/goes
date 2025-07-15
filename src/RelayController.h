#ifndef RELAYCONTROLLER_H
#define RELAYCONTROLLER_H

#include <Arduino.h>

class RelayController {
public:
  RelayController();
  void begin();
  void trigger(uint8_t relayId, uint8_t command); // relayId: 1=CB1, 2=CB2; command: 1=open, 2=close
  void update();                                  // Non-blocking, matikan relay otomatis
  void resetAll();                                // Matikan semua relay

private:
  struct Relay {
    uint8_t pinOpen;
    uint8_t pinClose;
    uint8_t state;           // 0: idle, 1: open aktif, 2: close aktif
    unsigned long startTime;
    const unsigned long duration = 800; // ms
  };
  Relay relays[2]; // CB1 dan CB2
};

#endif
