#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Data structure for frame data
struct FrameData {
    byte buffer[256];
    byte length;
};

// Data structure for measurement data
struct MeasurementData {
    uint16_t ioa;
    int value;
    byte type;
};

#endif // QUEUE_H
