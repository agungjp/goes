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

#ifndef NATIVE_ENV
#include <IPAddress.h> // Required for IPAddress type
#endif

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
// The communication module is selected via build flags in platformio.ini.
// Do not define USE_MODEM_* or USE_ETHERNET here. See `[env:esp32dev-*]`.


//==============================================================================
// 3. ETHERNET CONFIGURATION (only used if USE_ETHERNET is enabled)
//==============================================================================
#if defined(USE_ETHERNET) && !defined(NATIVE_ENV)
  static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  static IPAddress ip(192, 168, 1, 177);
  #define ETHERNET_PORT 2404
#endif


//==============================================================================
// 4. SERIAL PORT CONFIGURATION
//==============================================================================
// Define the serial ports for debugging and modem communication.
// NOTE: For some boards, only specific ports are available.

#if defined(BOARD_ESP32) && !defined(NATIVE_ENV)
  #define DEBUG_SERIAL Serial
  #define MODEM_SERIAL Serial1
#endif


#endif // GOES_CONFIG_H