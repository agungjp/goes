#include "IEC104Slave.h"

IEC104Slave slave;

void setup() {
  Serial.begin(9600);
  Serial.println(F("IEC 104 Slave - v1.3.1 (Refactor)"));
  slave.begin();
}

void loop() {
  slave.loop();
}