#ifndef IOA_CONFIG_H
#define IOA_CONFIG_H

#include <stdint.h>

// =================================================================
// ==         IEC-104 Information Object Addresses (IOA)          ==
// =================================================================
//  File ini adalah satu-satunya sumber untuk semua alamat IOA.
//  Jika IOA perlu diubah, cukup ubah di file ini.

// -- Single Point Information (M_SP_NA_1) --
// General Digital Inputs
const uint16_t IOA_DI_GFD           = 2001;
const uint16_t IOA_DI_SUPPLY_STATUS = 2002;
const uint16_t IOA_DI_REMOTE_1      = 2003;
const uint16_t IOA_DI_REMOTE_2      = 2004;
const uint16_t IOA_DI_REMOTE_3      = 2005;
const uint16_t IOA_DI_CADANGAN_1    = 2006;
const uint16_t IOA_DI_CADANGAN_2    = 2007;
const uint16_t IOA_DI_CADANGAN_3    = 2008;

// Additional Sensors / Inputs
// Door contact (single point)
const uint16_t IOA_DI_DOOR          = 2009;
// Aliases & new generic DI slots (numeric naming)
#define IOA_DI_1 IOA_DI_CADANGAN_1  // maps to 2006
#define IOA_DI_2 IOA_DI_CADANGAN_2  // maps to 2007
#define IOA_DI_3 IOA_DI_CADANGAN_3  // maps to 2008
const uint16_t IOA_DI_4             = 2010; // new
const uint16_t IOA_DI_5             = 2011; // new
// Analog / measured values (could map to M_ME_NA_1 etc. when implemented)
const uint16_t IOA_AI_TEMPERATURE   = 3001;
const uint16_t IOA_AI_HUMIDITY      = 3002;

// -- Double Point Information (M_DP_NA_1) --
// Circuit Breaker Status
const uint16_t IOA_STATUS_CB_1      = 11000;
const uint16_t IOA_STATUS_CB_2      = 11001;
const uint16_t IOA_STATUS_CB_3      = 11002;

// -- Double Command (C_DC_NA_1) --
// Circuit Breaker Commands
const uint16_t IOA_COMMAND_CB_1     = 23000;
const uint16_t IOA_COMMAND_CB_2     = 23001;
const uint16_t IOA_COMMAND_CB_3     = 23002;

// -- Single Command (C_SC_NA_1) --
// General Digital Outputs
const uint16_t IOA_DO_CADANGAN_1    = 23003;
const uint16_t IOA_DO_CADANGAN_2    = 23004;
// Aliases for numeric DO naming
#define IOA_DO_1 IOA_DO_CADANGAN_1
#define IOA_DO_2 IOA_DO_CADANGAN_2

#endif // IOA_CONFIG_H
