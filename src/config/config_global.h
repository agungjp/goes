#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

static const int MAX_BUFFER = 64;
const unsigned long TEST_ACT_TIMEOUT = 5UL * 60UL * 1000UL; // Waktu maksimal tanpa TESTFR_ACT = 5 menit

// IEC 104 Type Identification
#define C_IC_NA_1 100 // Interrogation Command
#define C_CS_NA_1 103 // Clock Synchronization Command
#define C_DC_NA_1 46  // Double Command

#endif