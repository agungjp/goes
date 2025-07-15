    #ifndef MODEMMANAGER_H
#define MODEMMANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class ModemManager {
public:
  ModemManager(uint8_t rxPin, uint8_t txPin);
  void begin(long baud = 9600);
  void update();          // Cek teks USR, reconnect, dsb.
  bool available();
  int read();
  void write(const uint8_t* buf, size_t len);
  // Tambah method sesuai kebutuhan
private:
  // Hardware serial/software serial di sini (Stream*)
  SoftwareSerial* modemSerial;
  uint8_t _rxPin, _txPin;
};

#endif
