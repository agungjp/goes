#include <SoftwareSerial.h>
#include "iec104.h"

SoftwareSerial SerialAT(10, 8);  // TX=10, RX=8 (modem)
GSMConnection gsm;

unsigned long lastCheck = 0;

void setup() {
  Serial.begin(9600);
  SerialAT.begin(9600);

  Serial.println(F("IEC 104 Slave - v1.2.0"));

  gsm.setupInputPins();         
  gsm.begin(&SerialAT);         
  gsm.setupConnection();        
}

void loop() {
  gsm.listen();

  if (millis() - lastCheck >= 1000) {
    gsm.updateInputStatus();
    gsm.checkAndSendCOS();
    lastCheck = millis();
  }
}
