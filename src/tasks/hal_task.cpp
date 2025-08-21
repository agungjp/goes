#include "tasks/hal_task.h"
#include <Arduino.h>

void hal_task(void *pvParameters) {
    HardwareManager* hardwareManager = static_cast<HardwareManager*>(pvParameters);
    
    while (1) {
        // This single call updates the LED and resets the watchdog.
        hardwareManager->updateHeartbeatLED(); 
        
        // Run this task periodically.
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
