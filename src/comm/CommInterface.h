#ifndef COMM_INTERFACE_H
#define COMM_INTERFACE_H

#include <stddef.h> // For size_t
#include <stdint.h> // For uint8_t

#ifdef ARDUINO
#include <Arduino.h> // For Stream compatibility
#endif

class CommInterface {
public:
    virtual ~CommInterface() = default;
    virtual void setupConnection() = 0;
    virtual void restart() = 0; // Generic restart for communication module
    virtual void sendData(const uint8_t* data, size_t len) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual void write(const uint8_t* data, size_t len) = 0;
    virtual void flush() = 0;
    virtual size_t readBytes(uint8_t* buffer, size_t length) {
        size_t i = 0;
        while (i < length) {
            int c = read();
            if (c < 0) break;
            buffer[i++] = (uint8_t)c;
        }
        return i;
    }
};

#endif // COMM_INTERFACE_H