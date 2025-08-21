#ifndef HAL_TASK_H
#define HAL_TASK_H

#include "hal/HardwareManager.h"

/**
 * @brief FreeRTOS task for managing hardware-related periodic operations.
 * 
 * This task handles blinking the heartbeat LED and resetting the watchdog timer.
 * @param pvParameters A pointer to the HardwareManager instance.
 */
void hal_task(void *pvParameters);

#endif // HAL_TASK_H
