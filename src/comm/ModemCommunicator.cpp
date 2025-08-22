#include "ModemCommunicator.h"
#include "config/device_config.h"

// Constructor: Initializes the modem communicator with a hardware serial port.
ModemCommunicator::ModemCommunicator(HardwareSerial* serial) : _modemSerial(serial) {}

// Sets up the modem connection by sending a sequence of AT commands.
void ModemCommunicator::setupConnection() {
    // Wait for the modem to initialize
    delay(15000); 

    // Send AT commands to configure the modem for TCP server mode
    _modemSerial->println("AT"); updateSerial(); delay(5000);
    _modemSerial->println("AT+CSQ"); updateSerial();
    _modemSerial->println("AT+CGATT=1"); updateSerial();
    _modemSerial->println("AT+CIPMODE=1"); updateSerial();
    _modemSerial->println("AT+CSTT=\"m2mplnapd\""); updateSerial();
    _modemSerial->println("AT+CIICR"); updateSerial(); delay(3000);
    _modemSerial->println("AT+CIFSR"); updateSerial();
    _modemSerial->println("AT+CIPSERVER=1,2404"); updateSerial();
    _modemSerial->println("AT+CIPSTATUS"); updateSerial();
}

// Restarts the modem (placeholder, needs implementation)
void ModemCommunicator::restart() {
    // TODO: Implement modem restart logic, e.g., using AT+CFUN=1,1
}

// Sends data to the modem.
void ModemCommunicator::sendData(const uint8_t* data, size_t len) {
    _modemSerial->write(data, len);
}

// Checks for available data from the modem.
int ModemCommunicator::available() {
    return _modemSerial->available();
}

// Reads a single byte from the modem.
int ModemCommunicator::read() {
    return _modemSerial->read();
}

// Writes data to the modem.
void ModemCommunicator::write(const uint8_t* data, size_t len) {
    _modemSerial->write(data, len);
}

// Waits for the transmission of outgoing serial data to complete.
void ModemCommunicator::flush() {
    _modemSerial->flush();
}

// Reads a specified number of bytes from the modem into a buffer.
size_t ModemCommunicator::readBytes(uint8_t* buffer, size_t length) {
    return _modemSerial->readBytes(buffer, length);
}

// Periodically checks for incoming data and forwards it via callback.
void ModemCommunicator::loop() {
    if (_modemSerial->available()) {
        // A simple approach: assume the available data is a complete frame.
        // A more robust implementation would buffer and parse for frame boundaries.
        byte buffer[256];
        int len = _modemSerial->readBytes(buffer, sizeof(buffer));
        if (len > 0 && _frameReceivedCallback) {
            _frameReceivedCallback(_callback_ctx, buffer, len);
        }
    }
}

// Sets the callback function for when a data frame is received.
void ModemCommunicator::setFrameReceivedCallback(void (*callback)(void*, const byte*, byte), void* ctx) {
    _frameReceivedCallback = callback;
    _callback_ctx = ctx;
}

// Helper function to forward modem responses to the main serial port for debugging.
void ModemCommunicator::updateSerial() {
  delay(500);
  while (Serial.available()) {
    _modemSerial->write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(_modemSerial->available()) {
    Serial.write(_modemSerial->read());//Forward what Software Serial received to Serial Port
  }
}
