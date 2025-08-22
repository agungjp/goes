#ifndef TASK_PARAMS_H
#define TASK_PARAMS_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

class IEC104Communicator;
class IEC104Core;
class HardwareManager;

// Struct to pass necessary components to the communication task
struct CommTaskParams {
    IEC104Communicator* communicator;
    QueueHandle_t incomingQueue;
    QueueHandle_t outgoingQueue;
};

// Struct to pass necessary components to the IEC104 logic task
struct Iec104TaskParams {
    IEC104Core* core;
    HardwareManager* hardwareManager;
    QueueHandle_t incomingQueue;
    QueueHandle_t outgoingQueue;
    QueueHandle_t measurementQueue;
};

#endif // TASK_PARAMS_H
