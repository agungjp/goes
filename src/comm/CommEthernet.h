#ifndef COMM_ETHERNET_H
#define COMM_ETHERNET_H

#if defined(USE_ETHERNET)

#include "CommInterface.h"
#include <WiFi.h> // Required for WiFiEvent_t and WiFi.onEvent, WiFiServer, WiFiClient
#include <ETH.h> // For ESP32 Ethernet

class CommEthernet : public CommInterface {
public:
    CommEthernet(byte mac[], IPAddress ip, uint16_t port);
    void setupConnection() override;
    void restart() override;
    void sendData(const uint8_t* data, size_t len) override;
    int available() override;
    int read() override;
    void write(const uint8_t* data, size_t len) override;
    void flush() override;

private:
    void checkClient();
    byte _mac[6];
    IPAddress _ip;
    WiFiServer _server;
    WiFiClient _client;
};

#endif // USE_ETHERNET
#endif // COMM_ETHERNET_H