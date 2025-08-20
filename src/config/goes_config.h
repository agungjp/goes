/*
|==============================================================================|
|                                GOES CONFIG                                   |
|==============================================================================|
| This is the main configuration file for the GOES project.                    |
| Choose your target hardware here.                                            |
|==============================================================================|
*/

#ifndef GOES_CONFIG_H
#define GOES_CONFIG_H

#include <IPAddress.h> // Required for IPAddress type

//==============================================================================
// 1. BOARD SELECTION
//==============================================================================
// Uncomment one of the following board definitions to select your target MCU.

#define BOARD_ESP32

#if defined(BOARD_ESP32)
  #include "config/board_esp32.h"
#else
  #error "No target board selected. Please define BOARD_ESP32 in goes_config.h."
#endif


//==============================================================================
// 2. MODEM/COMMUNICATION SELECTION


//==============================================================================
// 2. MODEM/COMMUNICATION SELECTION
//==============================================================================
// The communication module is now selected via build flags in platformio.ini.
// See the different `[env:esp32dev-*]` environments.

#define USE_MODEM_SIM800L
// #define USE_MODEM_SIM7600CE
// #define USE_MODEM_QUECTEL_EC25
// #define USE_ETHERNET


//==============================================================================
// 3. ETHERNET CONFIGURATION (only used if USE_ETHERNET is enabled)
//==============================================================================
extern byte mac[];
extern IPAddress ip;
#define ETHERNET_PORT 2404


//==============================================================================
// 4. SERIAL PORT CONFIGURATION
//==============================================================================
// Define the serial ports for debugging and modem communication.
// NOTE: For some boards, only specific ports are available.

#if defined(BOARD_ESP32)
  #define DEBUG_SERIAL Serial
  #define MODEM_SERIAL Serial1

#endif


#endif // GOES_CONFIG_H