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



// --- Statically allocate core components instead of using pointers ---
// This avoids dynamic memory allocation (new/delete), making the system
// more stable and predictable, which is critical for embedded devices.
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
  // The communicator will now call the core's handler when I-frames arrive.
  iec104Communicator.setFrameHandler(
    [](void* ctx, const byte* buf, byte len){
      static_cast<IEC104Core*>(ctx)->processReceivedFrame(buf, len);
    },
    &iec104Core // Pass the address of the static core object
  );
}

void loop() {
  hardwareManager.updateHeartbeatLED(); // This also resets the watchdog
  iec104Communicator.listen();
  iec104Core.run();
}

#endif // UNIT_TEST