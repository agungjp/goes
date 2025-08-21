#ifndef IEC104_TASK_H
#define IEC104_TASK_H

#include "iec104/core/IEC104Core.h"

/**
 * @brief FreeRTOS task for managing the IEC 104 application core logic.
 * 
 * This task runs the main logic loop of the IEC 104 protocol, such as
 * handling timeouts, checking for Cause of Transmission (COT) events, etc.
 * @param pvParameters A pointer to the IEC104Core instance.
 */
void iec104_task(void *pvParameters);

#endif // IEC104_TASK_H
