#include "tasks/comm_task.h"
#include <Arduino.h>

void comm_task(void *pvParameters) {
    IEC104Communicator* iec104Communicator = static_cast<IEC104Communicator*>(pvParameters);

    while (1) {
        // Task 1: Read incoming bytes from hardware and enqueue them
        iec104Communicator->listen();

        // Task 2: Dequeue any pending frames and send them to hardware
        iec104Communicator->processSendQueue();

        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
