#include "CommEthernet.h"

#include <Arduino.h>

#if defined(USE_ETHERNET)

// Callback for Ethernet events
static void WiFiEvent(WiFiEvent_t event)
{
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            Serial.println("ETH Started");
            //set eth hostname here
            ETH.setHostname("esp32-goes");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("ETH Connected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.print("ETH MAC: ");
            Serial.print(ETH.macAddress());
            Serial.print(", IPv4: ");
            Serial.print(ETH.localIP());
            Serial.println("");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("ETH Disconnected");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("ETH Stopped");
            break;
        default:
            break;
    }
}

CommEthernet::CommEthernet(byte mac[], IPAddress ip, uint16_t port) : _server(port) {
    // MAC address and IP are not directly used by WiFiServer/Client
    // but kept for compatibility with the constructor signature.
}

void CommEthernet::setupConnection() {
    WiFi.onEvent(WiFiEvent);
    ETH.begin();

    // Wait for Ethernet to connect and get an IP address
    Serial.print("Connecting to Ethernet");
    int timeout = 0;
    while (ETH.localIP() == IPAddress(0,0,0,0) && timeout < 20) { // Wait up to 20 seconds
        delay(500);
        Serial.print(".");
        timeout++;
    }
    Serial.println("");

    if (ETH.localIP() != IPAddress(0,0,0,0)) {
        Serial.print("Ethernet connected, IP: ");
        Serial.println(ETH.localIP());
        _server.begin();
    } else {
        Serial.println("Failed to connect to Ethernet.");
    }
}

void CommEthernet::restart() {
    // For Ethernet, a software restart could involve re-initializing.
    // A hardware reset would be more effective but is out of scope here.
    setupConnection();
}

void CommEthernet::checkClient() {
    if (!_client || !_client.connected()) {
        _client = _server.available(); // Listen for a new client
    }
}

void CommEthernet::sendData(const uint8_t* data, size_t len) {
    if (_client && _client.connected()) {
        _client.write(data, len);
    }
}

int CommEthernet::available() {
    checkClient();
    if (_client && _client.connected()) {
        return _client.available();
    }
    return 0;
}

int CommEthernet::read() {
    if (_client && _client.connected()) {
        return _client.read();
    }
    return -1;
}

void CommEthernet::write(const uint8_t* data, size_t len) {
    sendData(data, len); // Alias for sendData
}

void CommEthernet::flush() {
    if (_client && _client.connected()) {
        _client.flush();
    }
}

#endif // USE_ETHERNET