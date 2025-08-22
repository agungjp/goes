#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <stdint.h>

#ifndef NATIVE_ENV
#include <IPAddress.h> // Diperlukan untuk tipe IPAddress
#endif

#include "ioa_config.h"

// =================================================================
// ==         SINGLE SOURCE OF TRUTH FOR HARDWARE CONFIG          ==
// =================================================================
// File ini adalah pusat konfigurasi untuk semua pengaturan:
// 1. Pilihan Board & Komunikasi
// 2. Konfigurasi Jaringan (Ethernet/Modem)
// 3. Pinout Perangkat Keras (GPIO, I2C, SPI, UART)
// 4. Pemetaan Input/Output Digital dan Circuit Breaker
//
// Untuk konfigurasi alamat IOA IEC-104, lihat 'ioa_config.h'
// =================================================================


// =================================================================
// -- 1. PENGATURAN UTAMA
// =================================================================

// -- Pemilihan Board --
#define BOARD_ESP32

// -- Pemilihan Modul Komunikasi --
// Opsi ini diatur melalui build flags di platformio.ini.
// Contoh: -D USE_ETHERNET atau -D USE_MODEM_SIM800L
// Jangan definisikan di sini.

// -- Konfigurasi Serial Debug --
#if defined(BOARD_ESP32) && !defined(NATIVE_ENV)
  #define DEBUG_SERIAL Serial
  #define DEBUG_BAUD_RATE 115200
#endif


// =================================================================
// -- 2. PENGATURAN APLIKASI & TUGAS (TASKS)
// =================================================================

// -- Konfigurasi Logging --
// Aktifkan untuk menampilkan pesan debug di Serial Monitor.
// Komentari baris ini untuk build produksi.
#define ENABLE_DEBUG_PRINT

// -- Mode Simulasi (tidak ada hardware eksternal terpasang) --
// Jika hanya menggunakan board ESP32 tanpa expander PCF8574, sensor, modem, dll,
// aktifkan definisi berikut (atau tambahkan -D SIMULATE_NO_PERIPHERALS di build flags)
// untuk menghindari spam error I2C dan menghasilkan data dummy.
// #define SIMULATE_NO_PERIPHERALS

// -- Task Watchdog (ESP32) --
// Penyebab reset berulang yang Anda lihat adalah karena loopTask didaftarkan ke watchdog
// lalu segera dihapus (vTaskDelete) sehingga tidak pernah mem-feed watchdog.
// Aktifkan hanya jika Anda sudah memastikan ada task yang rutin memanggil esp_task_wdt_reset().
// 0 = nonaktif (default), 1 = aktifkan inisialisasi watchdog di HardwareManager.
#define ENABLE_TASK_WATCHDOG 0

// -- Parameter Tugas (Tasks) --
#define HAL_TASK_DELAY_MS       100   // Delay untuk loop utama Hardware Abstraction Layer
#define HAL_TASK_STACK_SIZE     6144  // Ukuran stack untuk HAL Task (increased for stability)
#define COMM_TASK_STACK_SIZE    4096  // Ukuran stack untuk Communication Task
#define IEC104_TASK_STACK_SIZE  8192  // Ukuran stack untuk IEC-104 Task


// =================================================================
// -- 3. KONFIGURASI JARINGAN
// =================================================================

// -- Ethernet (hanya digunakan jika USE_ETHERNET aktif) --
#if defined(USE_ETHERNET) && !defined(NATIVE_ENV)
    static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    static IPAddress ip(192, 168, 1, 177);
    #define ETHERNET_PORT 2404
#endif

// -- Modem (hanya digunakan jika USE_MODEM_* aktif) --
#if defined(USE_MODEM_SIM800L) || defined(USE_MODEM_SIM7600) || defined(USE_MODEM_SIM7600CE) || defined(USE_MODEM_QUECTEL_EC25)
  #define MODEM_APN "m2mplnapd" // Ganti dengan APN provider Anda
  #define MODEM_USER ""         // Ganti jika diperlukan
  #define MODEM_PASS ""         // Ganti jika diperlukan
  #define MODEM_BAUD_RATE 9600
#endif

// -- WiFi (selalu di-compile untuk akses Web UI; jika SSID kosong akan fallback ke AP) --
#define WIFI_STA_SSID      "How I Met Coffee"      // Ganti sesuai kebutuhan
#define WIFI_STA_PASSWORD  "beawesome"  // Ganti sesuai kebutuhan
#define WIFI_AP_SSID       "GOES_Config_AP"      // SSID fallback AP
#define WIFI_AP_PASSWORD   "goes1234"            // Minimal 8 karakter untuk WPA2
#define WIFI_CONNECT_TIMEOUT_MS 15000              // 15 detik mencoba konek STA


// =================================================================
// -- 4. PENGATURAN PINOUT PERANGKAT KERAS (ESP32)
// =================================================================

// -- Bus I2C (Shared) --
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22

// -- Bus SPI (Shared) --
#define SPI_MOSI_PIN        23
#define SPI_MISO_PIN        19
#define SPI_SCK_PIN         18
#define SPI_CS_ETHERNET_PIN 5
#define SPI_CS_SDCARD_PIN   15

// -- Bus UART --
#define UART_MODEM_NUM      2 // Menggunakan Serial2
#define UART_MODEM_TX_PIN   17
#define UART_MODEM_RX_PIN   16

#define UART_BMS_NUM        1 // Menggunakan Serial1
#define UART_BMS_TX_PIN     4
#define UART_BMS_RX_PIN     2

#define UART_RS485_NUM      0 // Menggunakan Serial0
#define UART_RS485_TX_PIN   1
#define UART_RS485_RX_PIN   3
#define UART_RS485_DE_PIN   25 // Direction Enable Pin

// -- Peripheral Lainnya --
#define DHT22_DATA_PIN      26
#define LED_HEARTBEAT_PIN   27
#define MODEM_RESET_PIN     32
#define RTC_I2C_ADDR        0x68


// =================================================================
// -- 4. PENGATURAN GPIO EXPANDER (PCF8574T)
// =================================================================

// -- Alamat I2C untuk setiap chip PCF8574T --
#define PCF_INPUTS_GENERAL_ADDR   0x20
#define PCF_STATUS_CB_ADDR        0x21
#define PCF_OUTPUTS_CMD_ADDR      0x22

// -- Definisi Input Digital (DI) --
struct DigitalInputConfig {
    uint8_t expanderAddress; // Alamat I2C expander
    uint8_t expanderPin;     // Pin pada expander (0-7)
    uint16_t ioa;            // Alamat IOA untuk IEC-104
    const char* description; // Deskripsi fungsional
};

const DigitalInputConfig digitalInputs[] = {
    // Chip 1: INPUTS - GENERAL (0x20)
    {PCF_INPUTS_GENERAL_ADDR, 0, IOA_DI_GFD, "DI - GFD"},
    {PCF_INPUTS_GENERAL_ADDR, 1, IOA_DI_SUPPLY_STATUS, "DI - Supply Status"},
    {PCF_INPUTS_GENERAL_ADDR, 2, IOA_DI_REMOTE_1, "DI - Remote 1 Status"},
    {PCF_INPUTS_GENERAL_ADDR, 3, IOA_DI_REMOTE_2, "DI - Remote 2 Status"},
    {PCF_INPUTS_GENERAL_ADDR, 4, IOA_DI_REMOTE_3, "DI - Remote 3 Status"},
    {PCF_INPUTS_GENERAL_ADDR, 5, IOA_DI_CADANGAN_1, "DI - Cadangan 1"},
    {PCF_INPUTS_GENERAL_ADDR, 6, IOA_DI_CADANGAN_2, "DI - Cadangan 2"},
    {PCF_INPUTS_GENERAL_ADDR, 7, IOA_DI_CADANGAN_3, "DI - Cadangan 3"},
};

// -- Definisi Output Digital (DO) --
struct DigitalOutputConfig {
    uint8_t expanderAddress; // Alamat I2C expander
    uint8_t expanderPin;     // Pin pada expander (0-7)
    uint16_t ioa;            // Alamat IOA Perintah untuk IEC-104
    const char* description; // Deskripsi fungsional
};

const DigitalOutputConfig digitalOutputs[] = {
    // Chip 3: OUTPUTS - COMMANDS (0x22)
    // Output untuk CB sekarang dikelola oleh struct CircuitBreakerConfig
    {PCF_OUTPUTS_CMD_ADDR, 6, IOA_DO_CADANGAN_1, "DO - Cadangan 1"},
    {PCF_OUTPUTS_CMD_ADDR, 7, IOA_DO_CADANGAN_2, "DO - Cadangan 2"},
};


// =================================================================
// -- 5. PENGATURAN CIRCUIT BREAKER (CB)
// =================================================================
// Menggunakan status double-point (M_DP_NA_1) dan perintah double-command (C_DC_NA_1)

struct CBStatusInput {
    uint8_t expanderAddress;
    uint8_t expanderPin;
};

struct CBCommandOutput {
    uint8_t expanderAddress;
    uint8_t expanderPin;
};

struct CircuitBreakerConfig {
    int cbNumber;
    uint16_t status_ioa;      // IOA untuk status (M_DP_NA_1)
    CBStatusInput open_input;
    CBStatusInput close_input;
    uint16_t command_ioa;     // IOA untuk perintah (C_DC_NA_1)
    CBCommandOutput open_command;
    CBCommandOutput close_command;
    const char* description;
};

const CircuitBreakerConfig circuitBreakers[] = {
    {
        1, IOA_STATUS_CB_1, // CB 1 Status IOA
        {PCF_STATUS_CB_ADDR, 0}, // Pin untuk status OPEN
        {PCF_STATUS_CB_ADDR, 1}, // Pin untuk status CLOSE
        IOA_COMMAND_CB_1, // CB 1 Command IOA
        {PCF_OUTPUTS_CMD_ADDR, 0}, // Pin untuk perintah OPEN
        {PCF_OUTPUTS_CMD_ADDR, 1}, // Pin untuk perintah CLOSE
        "Circuit Breaker 1"
    },
    {
        2, IOA_STATUS_CB_2, // CB 2 Status IOA
        {PCF_STATUS_CB_ADDR, 2}, // Pin untuk status OPEN
        {PCF_STATUS_CB_ADDR, 3}, // Pin untuk status CLOSE
        IOA_COMMAND_CB_2, // CB 2 Command IOA
        {PCF_OUTPUTS_CMD_ADDR, 2}, // Pin untuk perintah OPEN
        {PCF_OUTPUTS_CMD_ADDR, 3}, // Pin untuk perintah CLOSE
        "Circuit Breaker 2"
    },
    {
        3, IOA_STATUS_CB_3, // CB 3 Status IOA
        {PCF_STATUS_CB_ADDR, 4}, // Pin untuk status OPEN
        {PCF_STATUS_CB_ADDR, 5}, // Pin untuk status CLOSE
        IOA_COMMAND_CB_3, // CB 3 Command IOA
        {PCF_OUTPUTS_CMD_ADDR, 4}, // Pin untuk perintah OPEN
        {PCF_OUTPUTS_CMD_ADDR, 5}, // Pin untuk perintah CLOSE
        "Circuit Breaker 3"
    },
};


// Menghitung jumlah item secara otomatis
const int digitalInputCount = sizeof(digitalInputs) / sizeof(digitalInputs[0]);
const int digitalOutputCount = sizeof(digitalOutputs) / sizeof(digitalOutputs[0]);
const int circuitBreakerCount = sizeof(circuitBreakers) / sizeof(circuitBreakers[0]);


#endif // DEVICE_CONFIG_H
