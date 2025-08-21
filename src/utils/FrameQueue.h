#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <cstring>
#include <vector>
using byte = uint8_t;
#endif

#include "config/config_global.h" // For FRAME_SIZE

#ifndef FRAME_SIZE
#define FRAME_SIZE 255 // Default frame size
#endif

#define QUEUE_SIZE 10 // Number of frames the queue can hold

class FrameQueue {
public:
    FrameQueue();
    bool enqueue(const byte* frame, byte length);
    bool dequeue(byte* frame, byte& length);
    bool isEmpty() const;
    bool isFull() const;

private:
#ifdef ARDUINO
    byte _queue[QUEUE_SIZE][FRAME_SIZE];
    byte _lengths[QUEUE_SIZE];
    int _head;
    int _tail;
    int _count;
#else
    std::vector<std::vector<byte>> _queue;
#endif
};

#endif // FRAME_QUEUE_H
