#include <SoftwareSerial.h>
#include "IEC104Slave.h"

SoftwareSerial modem(10, 8);           // TX, RX ke modem
IEC104Slave slave(&modem);             // Konstruktor pakai Stream*

void setup() {
  Serial.begin(9600);
  modem.begin(9600);
  Serial.println(F("IEC 60870-5-104 Slave v1.4.1"));
  slave.begin();
}

void loop() {
  slave.run();  // renamed from loop()
}
