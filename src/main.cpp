/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.9.0  |
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



// --- Global Pointers for Core Components ---
// These pointers will hold the instances of our core components.
// They are initialized in setup() based on hardware configuration.
HardwareManager* hardwareManager = nullptr;
CommInterface* comm = nullptr;
IEC104Communicator* iec104Communicator = nullptr;
IEC104Core* iec104Core = nullptr;

 

void setup() {
  // Initialize Serial ports first
#if defined(BOARD_ESP32)
  Serial.begin(115200);
  Serial1.begin(9600);

#endif

  // 1. Initialize Hardware Abstraction Layer
  hardwareManager = new HardwareManager();
  hardwareManager->init(); // Handles I2C, Watchdog, and Pins

  // 2. Communication Module Factory
  // Select the communication module based on flags in goes_config.h
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
  comm = new ModemCommunicator(&MODEM_SERIAL, hardwareManager);
#elif defined(USE_ETHERNET)
  comm = new CommEthernet(mac, ip, ETHERNET_PORT);
#else
  #error "No communication module selected in goes_config.h"
#endif

  // 3. Initialize Communication and Core Logic
  comm->setupConnection();
  comm->restart();
  comm->flush();

  iec104Communicator = new IEC104Communicator(comm);
  iec104Core = new IEC104Core(iec104Communicator, hardwareManager);

  // Forward I-frames to core logic
  iec104Communicator->setFrameHandler(
    [](void* ctx, const byte* buf, byte len){
      static_cast<IEC104Core*>(ctx)->processReceivedFrame(buf, len);
    },
    iec104Core
  );
}

void loop() {
  hardwareManager->updateHeartbeatLED(); // This also resets the watchdog
  iec104Communicator->listen();
  iec104Core->run();
}

#endif // UNIT_TEST