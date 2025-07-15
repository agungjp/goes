#include "EventQueue.h"
#include "IEC104Protocol.h"

EventQueue::EventQueue() : head(0), tail(0), cnt(0) {}

bool EventQueue::push(const Event &e) {
  if (cnt >= SIZE) return false;
  buffer[tail] = e;
  tail = (tail + 1) % SIZE;
  cnt++;
  return true;
}

bool EventQueue::pop(Event &e) {
  if (cnt == 0) return false;
  e = buffer[head];
  head = (head + 1) % SIZE;
  cnt--;
  return true;
}

void EventQueue::process(IEC104Protocol* protocol) {
  Event e;
  if (pop(e)) {
    // Contoh: jika event COS, langsung kirim frame ke master
    if (protocol && e.type == 0) {
      protocol->sendCOS(e.ioa, e.value);
    }
    // Extend untuk COMMAND, dsb.
  }
}

bool EventQueue::isEmpty() const { return cnt == 0; }
bool EventQueue::isFull() const { return cnt >= SIZE; }
