#ifndef SOE_BUFFER_H
#define SOE_BUFFER_H
#include <Arduino.h>

// Simple Sequence Of Events buffer (in-RAM ring) with timestamp (millis), ioa, value.
// Intended for small capacity (e.g. 64..256). Not persisted across reboot.
namespace SOEBuffer {
    struct Event { uint32_t ms; uint16_t ioa; int16_t value; uint32_t seq; };
    void init(size_t capacity = 64);
    void clear();
    void push(uint16_t ioa, int value); // drops oldest when full
    String toJSON(); // {"events":[{"ms":..,"ioa":..,"v":..,"seq":..},...],"count":n,"cap":c,"overflow":b}
    size_t count();
    bool overflowed();
}

#endif // SOE_BUFFER_H
