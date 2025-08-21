#ifndef COMM_TASK_H
#define COMM_TASK_H

#include "iec104/transport/IEC104Communicator.h"

/**
 * @brief FreeRTOS task for handling the communication layer.
 * 
 * This task is responsible for listening for incoming data from the physical layer
 * (Ethernet/Modem) and passing it up to the IEC104 transport layer.
 * @param pvParameters A pointer to the IEC104Communicator instance.
 */
void comm_task(void *pvParameters);

#endif // COMM_TASK_H
