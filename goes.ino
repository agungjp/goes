#include "iec104.h"

SoftwareSerial  SerialAT(10,8); // TX, RX
iec104 srv(SerialAT);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  delay(1000);

  Serial.println("Gardu Online Surveillance - MAIN");
}

void loop() {

}
