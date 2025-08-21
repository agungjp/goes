#ifndef COMM_TASK_H
#define COMM_TASK_H

#include "tasks/task_params.h"

/**
 * @brief FreeRTOS task for handling the communication layer.
 * 
 * This task is responsible for:
 * 1. Listening for incoming data from the physical layer (Ethernet/Modem).
 * 2. Placing received data frames into the incomingFrameQueue.
 * 3. Checking the outgoingFrameQueue for frames to be sent.
 * 4. Sending frames via the physical layer.
 * 
 * @param pvParameters A pointer to a CommTaskParams struct.
 */
void comm_task(void *pvParameters);

#endif // COMM_TASK_H
