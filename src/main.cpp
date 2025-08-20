#include <SoftwareSerial.h>
#include "IEC104Slave.h"
#include <avr/wdt.h>

SoftwareSerial modem(10, 8);
IEC104Slave slave(&modem);

void setup() {
  Serial.begin(115200);
  modem.begin(9600);
  #ifdef DEBUG
  Serial.println(F("GOES - IEC 60870-5-104 Slave v1.6.2"));
  #endif
  slave.begin();
  wdt_enable(WDTO_8S);
}

void loop() {
  slave.run();
  wdt_reset();
}


