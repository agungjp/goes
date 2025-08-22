#ifndef IEC104_CONFIG_H
#define IEC104_CONFIG_H

// =================================================================
// ==         GLOBAL CONFIGURATION FOR IEC-104 PROTOCOL           ==
// =================================================================

/* FREERTOS TASK PRIORITIES */
#define TASK_HAL_PRIORITY       5
#define TASK_COMM_PRIORITY      10
#define TASK_IEC104_PRIORITY    8

/* IEC 104 PROTOCOL PARAMETERS */
// These parameters define the behavior of the IEC 104 stack.
// All time values are in seconds.

// -- Timeouts --
#define IEC104_T0   30  // Connection establishment timeout
#define IEC104_T1   15  // APDU response timeout
#define IEC104_T2   10  // Acknowledgment timeout for S-frames (T2 < T1)
#define IEC104_T3   20  // Timeout for sending test frames in long idle states

// -- APDU Flow Control --
#define IEC104_K    12  // Max number of unacknowledged I-frames (sending window)
#define IEC104_W    8   // Max number of received I-frames before acknowledging (receiving window, W < K)

// -- ASDU (Application Service Data Unit) Configuration --
#define IEC104_ASDU_ADDRESS     1   // Common Address of ASDU (1 or 2 bytes)
#define IEC104_ORIGINATOR_ADDR  0   // Originator Address (0 for master)

// -- Cause of Transmission (COT) --
#define COT_PERIODIC            1
#define COT_BACKGROUND_SCAN     2
#define COT_SPONTANEOUS         3
#define COT_INITIALIZED         4
#define COT_REQUEST             5
#define COT_ACTIVATION          6
#define COT_ACTIVATION_CON      7
#define COT_DEACTIVATION        8
#define COT_DEACTIVATION_CON    9
#define COT_ACTIVATION_TERM     10
#define COT_INTERROGATED_STATION 20
#define COT_INTERROGATED_GROUP_1 21

// -- Type Identification (ASDU Type ID) --
#define M_SP_NA_1   1   // Single-point information
#define M_DP_NA_1   3   // Double-point information
#define M_ST_NA_1   5   // Step position information
#define M_ME_NA_1   9   // Measured value, normalized value
#define M_ME_NB_1   11  // Measured value, scaled value
#define M_ME_NC_1   13  // Measured value, short floating point number
#define M_SP_TB_1   30  // Single-point information with time tag CP56Time2a
#define M_DP_TB_1   31  // Double-point information with time tag CP56Time2a
#define M_EI_NA_1   70  // End of initialization
#define C_SC_NA_1   45  // Single-command
#define C_DC_NA_1   46  // Double-command
#define C_IC_NA_1   100 // Interrogation command
#define C_CS_NA_1   103 // Clock synchronization command

// -- U-Frame Types --
#define U_STARTDT_ACT   0x07
#define U_STARTDT_CON   0x0B
#define U_STOPDT_ACT    0x13
#define U_STOPDT_CON    0x23
#define U_TESTFR_ACT    0x43
#define U_TESTFR_CON    0x83

#endif // IEC104_CONFIG_H
