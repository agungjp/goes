#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <Arduino.h>
#include "IOAMapManager.h"

struct Event {
  uint8_t type;      // 0 = COS, 1 = COMMAND, dst.
  uint32_t ioa;
  uint8_t value;
};

class IEC104Protocol; // Forward declaration

class EventQueue {
public:
  EventQueue();
  bool push(const Event &e);
  bool pop(Event &e);
  void process(IEC104Protocol* protocol); // Proses 1 event per loop
  bool isEmpty() const;
  bool isFull() const;
private:
  static const uint8_t SIZE = 16;
  Event buffer[SIZE];
  uint8_t head, tail, cnt;
};

#endif
