#ifndef IEC104_TASK_H
#define IEC104_TASK_H

#include "tasks/task_params.h"

/**
 * @brief FreeRTOS task for managing the IEC 104 application core logic.
 * 
 * This task is responsible for:
 * 1. Checking the incomingFrameQueue for new frames and processing them.
 * 2. Checking the measurementQueue for new hardware data and processing it.
 * 3. Running the main logic loop of the IEC 104 protocol (timeouts, etc.).
 * 4. Placing frames to be sent into the outgoingFrameQueue.
 * 
 * @param pvParameters A pointer to an Iec104TaskParams struct.
 */
void iec104_task(void *pvParameters);

#endif // IEC104_TASK_H
