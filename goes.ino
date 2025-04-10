#include <SoftwareSerial.h>
#include "iec104.h"

SoftwareSerial SerialAT(10, 8);  // TX=10, RX=8 (modem)

GSMConnection gsm;

void setup() {
  Serial.begin(9600);
  SerialAT.begin(9600);

  gsm.setupInputPins();         // Inisialisasi pin input
  gsm.begin(&SerialAT);         // Inisialisasi modem
  gsm.setupConnection();        // AT Command
}

void loop() {
  gsm.listen();                 // Baca & respon frame dari master
}
