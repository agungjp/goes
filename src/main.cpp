/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v2.0.0  |
|==================================================================ok ============|
|  Copyright (C) 2024-2025 Mr. Pegagan (agungjulianperkasa@gmail.com)         |
|  All rights reserved.                                                        |
|==============================================================================|
|  This program is free software and is distributed under the terms of the     |
|  Lesser GNU General Public License. See the LICENSE file for details.        |
|==============================================================================*/

#include <Arduino.h>
#ifndef UNIT_TEST
#include "config/goes_config.h" // Master configuration file

#include "hal/HardwareManager.h"
#include "comm/CommInterface.h"
#include "iec104/transport/IEC104Communicator.h"
#include "iec104/core/IEC104Core.h"

// --- Include all possible communication modules ---
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
#include "comm/ModemCommunicator.h"
#elif defined(USE_ETHERNET)
#include "comm/CommEthernet.h"
#endif

// --- Include FreeRTOS task headers ---
#include "tasks/hal_task.h"
#include "tasks/comm_task.h"
#include "tasks/iec104_task.h"

// --- Statically allocate core components ---
HardwareManager hardwareManager;

#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
ModemCommunicator comm(&MODEM_SERIAL, &hardwareManager);
#elif defined(USE_ETHERNET)
CommEthernet comm(mac, ip, ETHERNET_PORT);
#else
#error "No communication module selected in goes_config.h"
#endif

IEC104Communicator iec104Communicator(&comm);
IEC104Core iec104Core(&iec104Communicator, &hardwareManager);

void setup() {
  // Initialize Serial ports first
#if defined(BOARD_ESP32)
  Serial.begin(115200);
  Serial1.begin(9600);
#endif

  // 1. Initialize Hardware Abstraction Layer
  hardwareManager.init(); // Handles I2C, Watchdog, and Pins

  // 2. Initialize Communication and Core Logic
  comm.setupConnection();
  comm.restart();
  comm.flush();

  // 3. Forward I-frames to core logic
  iec104Communicator.setFrameHandler(
    [](void* ctx, const byte* buf, byte len){
      static_cast<IEC104Core*>(ctx)->processReceivedFrame(buf, len);
    },
    &iec104Core // Pass the address of the static core object
  );

  // 4. Create FreeRTOS tasks and pin them to specific cores
  // Core 0: Dedicated to the main application logic (IEC-104 and communication)
  // Core 1: Used for lower-priority tasks and the default Arduino loop/system tasks
  xTaskCreatePinnedToCore(comm_task, "Comm_Task", 4096, &iec104Communicator, 10, NULL, 0);
  xTaskCreatePinnedToCore(iec104_task, "IEC104_Task", 4096, &iec104Core, 8, NULL, 0);
  
  // Pin the hardware task to Core 1 to keep it separate from the main logic
  xTaskCreatePinnedToCore(hal_task, "HAL_Task", 2048, &hardwareManager, 5, NULL, 1);

  // After starting the scheduler, setup() and loop() will no longer be the main
  // execution thread. The tasks will take over.
}

void loop() {
  // The loop is intentionally left empty. FreeRTOS scheduler handles all operations.
  vTaskDelay(portMAX_DELAY); // Or simply leave it empty
}

#endif // UNIT_TEST