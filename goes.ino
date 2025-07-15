#define DEBUG

#ifdef DEBUG
  #define LOG_DEBUG(x) Serial.println(F(x))
#else
  #define LOG_DEBUG(x)  // kosong (no operation)
#endif

#include "src/GOESController.h"
#include <avr/wdt.h>

GOESController goes;
ModemManager modem(8, 10); 

const unsigned long TEST_ACT_TIMEOUT = 300000UL; // 5 menit (ms)
unsigned long lastTestAct = 0;

void setup() {
  Serial.begin(115200);   // Untuk debug log (opsional)
  modem.begin(9600);
  goes.begin();
  LOG_DEBUG("GOES - IEC 60870-5-104 Modular Slave v1.6.1");
  wdt_enable(WDTO_8S);         // Watchdog enable
  lastTestAct = millis();
}

void loop() {
  goes.run();
  if (millis() - lastTestAct > TEST_ACT_TIMEOUT) {
    LOG_DEBUG("Tidak ada TESTFR_ACT >5 menit → hardware reset...");
    wdt_enable(WDTO_8S);
    while (1) { }
  }
  wdt_reset();
}
