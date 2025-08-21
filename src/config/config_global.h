#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#include "config/config_ioa.h"

static const int MAX_BUFFER = 64;
const unsigned long TEST_ACT_TIMEOUT = 5UL * 60UL * 1000UL; // Waktu maksimal tanpa TESTFR_ACT = 5 menit

// IEC 104 Type Identification
#define C_IC_NA_1 100 // Interrogation Command
#define C_CS_NA_1 103 // Clock Synchronization Command
#define C_DC_NA_1 46  // Double Command

// U-format control field values
#define U_STARTDT_ACT 0x07
#define U_STARTDT_CON 0x0B
#define U_STOPDT_ACT  0x13
#define U_STOPDT_CON  0x23
#define U_TESTFR_ACT  0x43
#define U_TESTFR_CON  0x83

// Type Identification
#define M_SP_NA_1 1   // Single-point information
#define M_DP_NA_1 3   // Double-point information
#define M_ME_NC_1 13  // Measured value, short floating point
#define C_SC_NA_1 45  // Single command
#define C_DC_NA_1 46  // Double command
#define C_SE_NC_1 50  // Set-point command, short floating point
#define C_IC_NA_1 100 // Interrogation command
#define C_CS_NA_1 103 // Clock synchronization command

#endif // CONFIG_GLOBAL_H