#include "tasks/iec104_task.h"
#include <Arduino.h>

void iec104_task(void *pvParameters) {
    IEC104Core* iec104Core = static_cast<IEC104Core*>(pvParameters);

    while (1) {
        // Task 1: Process any frames that have been received and queued
        iec104Core->getCommunicator()->processReceivedQueue();

        // Task 2: Run the core protocol logic (timers, state checks, etc.)
        iec104Core->run();

        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
