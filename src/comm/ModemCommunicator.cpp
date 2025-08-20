#include "ModemCommunicator.h"
#include "config/goes_config.h"
#include "hal/HardwareManager.h" // For PIN_MODEM_POWER
#include "hal/PinESP32.h" // Direct include for pin definitions
#include <Arduino.h> // For Serial, millis, delay, String

ModemCommunicator::ModemCommunicator(Stream* serial, HardwareManager* hw) : _modem(serial), _hardware(hw) {
    // Constructor
}

void ModemCommunicator::setupConnection() {
#if defined(USE_MODEM_SIM800L)
  // --- AT Command Sequence for SIM800L (GPRS) ---
  delay(10000); // Wait for modem to register to network
  _modem->println("AT"); updateSerial(); delay(1000);
  _modem->println("AT+CSQ"); updateSerial();
  _modem->println("AT+CGATT?"); updateSerial();
  _modem->println("AT+CSTT=\"m2mplnapd\""); updateSerial(); // Set APN
  _modem->println("AT+CIICR"); updateSerial(); delay(3000); // Bring up wireless connection
  _modem->println("AT+CIFSR"); updateSerial(); // Get local IP address
  _modem->println("AT+CIPSERVER=1,2404"); updateSerial(); // Start TCP server on port 2404
  _modem->println("AT+CIPSTATUS"); updateSerial();

#elif defined(USE_MODEM_SIM7600CE)
  // --- AT Command Sequence for SIM7600CE (4G) ---
  delay(10000);
  _modem->println("AT"); updateSerial(); delay(1000);
  _modem->println("AT+CSQ"); updateSerial();
  _modem->println("AT+CPSI?"); updateSerial(); // Check network registration
  _modem->println("AT+CSTT=\"m2mplnapd\""); updateSerial(); // Set APN
  _modem->println("AT+NETOPEN"); updateSerial(); delay(3000); // Open network
  _modem->println("AT+IPADDR"); updateSerial(); // Show IP address
  _modem->println("AT+CIPSERVER=1,2404"); updateSerial(); // Start TCP server

#elif defined(USE_MODEM_QUECTEL_EC25)
  // --- AT Command Sequence for Quectel EC25 (4G) ---
  delay(10000);
  _modem->println("AT"); updateSerial(); delay(1000);
  _modem->println("AT+CSQ"); updateSerial();
  _modem->println("AT+QINISTAT"); updateSerial(); // Check network registration status
  _modem->println("AT+QICSGP=1,1,\"m2mplnapd\",\"\",\"\",1"); updateSerial(); // Configure APN
  _modem->println("AT+QIACT=1"); updateSerial(); delay(3000); // Activate PDP context
  _modem->println("AT+QIACT?"); updateSerial(); // Query local IP address
  _modem->println("AT+QILISTEN=2404"); updateSerial(); // Listen on port 2404

#endif
}

void ModemCommunicator::restart() { // Implements CommInterface::restart()
  // Serial.println("🔁 Restarting modem via D9..."); // Removed
  _hardware->setPin(PIN_MODEM_POWER, HIGH);
  delay(500);
  _hardware->setPin(PIN_MODEM_POWER, LOW);
  delay(500);
  // Serial.println("✅ Restart done."); // Removed
}

void ModemCommunicator::sendData(const uint8_t* data, size_t len) {
    _modem->write(data, len);
}

int ModemCommunicator::available() {
    return _modem->available();
}

int ModemCommunicator::read() {
    return _modem->read();
}

void ModemCommunicator::write(const uint8_t* data, size_t len) {
    _modem->write(data, len);
}

void ModemCommunicator::flush() {
  // Buang semua data yang belum terbaca di modem buffer
  while (_modem->available()) _modem->read();
  delay(100);
  // Jika pakai Serial debug, buang juga
  // while (Serial.available()) Serial.read(); // Removed
}

void ModemCommunicator::updateSerial() {
  delay(2000);
  while (_modem->available()) Serial.write(_modem->read());
  while (Serial.available()) _modem->write(Serial.read());
}

void ModemCommunicator::handleModemText(String text) { // Use String
  text.toUpperCase();
  // Need to pass connectionState or have a callback for it
  // if (text.indexOf("CLOSED") >= 0) {
  //   connectionState = 1;
  //   Serial.println("⚠️ Koneksi CLOSED. Siap reconnect...");
  // } else if ((text.indexOf("REMOTE IP") >= 0 || text.indexOf("CONNECT") >= 0) && connectionState == 1) {
  //   connectionState = 2;
  //   Serial.println("🔌 CONNECT terdeteksi. Menunggu STARTDT_ACT...");
  // }
  // Serial.println("📡 MODEM: " + text); // Removed
  // text.toUpperCase(); // Already done above

  // if (text.indexOf("CLOSED") >= 0) {
  //   connectionState = 0;
  //   Serial.println("⚠️  Koneksi terputus.");
  // }
  // else if (text.indexOf("REMOTE IP") >= 0 || text.indexOf("CONNECT") >= 0) {
  //   connectionState = 1;
  //   Serial.println("🔄 Modem CONNECTED. Menunggu STARTDT_ACT...");
  // }
}
