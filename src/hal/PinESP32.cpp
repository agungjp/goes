#include "PinESP32.h"
#include "config/device_config.h" // Menggunakan file konfigurasi terpusat
#include <Arduino.h>

void PinESP32::setupPins() {
    // Inisialisasi pin-pin yang terhubung langsung ke ESP32
    
    // -- Komunikasi --
    // I2C sudah diinisialisasi di HardwareManager
    // SPI sudah diinisialisasi di HardwareManager
    
    // UART untuk Modem
    Serial.begin(115200); // Serial debug
    Serial2.begin(9600, SERIAL_8N1, UART_MODEM_RX_PIN, UART_MODEM_TX_PIN);

    // -- Output Kritis --
    pinMode(LED_HEARTBEAT_PIN, OUTPUT);
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(LED_HEARTBEAT_PIN, LOW);
    digitalWrite(MODEM_RESET_PIN, LOW);

    // -- Sensor --
    pinMode(DHT22_DATA_PIN, INPUT);

    // Catatan: Pin untuk GPIO expander (PCF8574T) tidak di-setup di sini.
    // Mereka dikelola oleh library PCF8574T melalui I2C.
}

void PinESP32::setPinMode(int pin, int mode) {
    pinMode(pin, mode);
}

void PinESP32::setPin(int pin, int value) {
    digitalWrite(pin, value);
}

int PinESP32::getPin(int pin) {
    return digitalRead(pin);
}