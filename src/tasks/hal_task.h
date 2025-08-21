#ifndef HAL_TASK_H
#define HAL_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/**
 * @brief FreeRTOS task for managing hardware-related periodic operations.
 * 
 * This task handles:
 * 1. Reading sensor data and digital inputs.
 * 2. Placing new measurement data into the measurementQueue.
 * 3. Blinking the heartbeat LED.
 * 4. Resetting the watchdog timer.
 * 
 * @param pvParameters A pointer to the measurementQueue handle.
 */
void hal_task(void *pvParameters);

#endif // HAL_TASK_H
