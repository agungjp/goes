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
#include "config/goes_config.h"
#include "config/config_global.h"
#include "config/config_ioa.h"

#include "hal/HardwareManager.h"
#include "comm/CommEthernet.h"
#include "comm/ModemCommunicator.h"
#include "iec104/core/IEC104Core.h"
#include "iec104/transport/IEC104Communicator.h"

#include "tasks/comm_task.h"
#include "tasks/hal_task.h"
#include "tasks/iec104_task.h"
#include "tasks/task_params.h"

#include "utils/Queue.h"

// Global variables
HardwareManager hardwareManager;

// Define which communication method to use
// #define USE_ETHERNET
#define USE_MODEM

// Forward declarations for communication objects
#if defined(USE_ETHERNET)
#include "comm/CommEthernet.h"
CommEthernet comm;
#elif defined(USE_MODEM)
#include "comm/ModemCommunicator.h"
#include "comm/CommInterface.h"
// We need a dummy CommInterface for the modem if Ethernet is not used.
// This could be a simple class that does nothing.
class DummyComm : public CommInterface {
public:
    void setupConnection() override {}
    void restart() override {}
    void sendData(const uint8_t* data, size_t len) override {}
    int available() override { return 0; }
    int read() override { return -1; }
    void write(const uint8_t* data, size_t len) override {}
    void flush() override {}
};
DummyComm dummy;
ModemCommunicator comm(&dummy);
#else
#error "No communication method defined. Please define USE_ETHERNET or USE_MODEM."
#endif

IEC104Core iec104Core;
IEC104Communicator iec104Communicator(&comm, &iec104Core);

QueueHandle_t incomingFrameQueue;
QueueHandle_t outgoingFrameQueue;
QueueHandle_t measurementQueue;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("GOES MCU Application");

  hardwareManager.begin();
  
  comm.setupConnection();

  // Create queues
  incomingFrameQueue = xQueueCreate(10, sizeof(FrameData));
  outgoingFrameQueue = xQueueCreate(10, sizeof(FrameData));
  measurementQueue = xQueueCreate(20, sizeof(MeasurementData));

  // Create task parameters
  static CommTaskParams comm_params;
  comm_params.communicator = &iec104Communicator;
  comm_params.incomingQueue = incomingFrameQueue;
  comm_params.outgoingQueue = outgoingFrameQueue;

  static Iec104TaskParams iec104_params;
  iec104_params.core = &iec104Core;
  iec104_params.incomingQueue = incomingFrameQueue;
  iec104_params.outgoingQueue = outgoingFrameQueue;
  iec104_params.measurementQueue = measurementQueue;

  // Create tasks
  xTaskCreatePinnedToCore(comm_task, "Comm_Task", 4096, &comm_params, 10, NULL, 0);
  xTaskCreatePinnedToCore(iec104_task, "IEC104_Task", 4096, &iec104_params, 8, NULL, 1);
  xTaskCreatePinnedToCore(hal_task, "HAL_Task", 2048, measurementQueue, 5, NULL, 1);
}

void loop() 
{
  vTaskDelete(NULL);
}