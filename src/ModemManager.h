    #ifndef MODEMMANAGER_H
#define MODEMMANAGER_H

#include <Arduino.h>

class ModemManager {
public:
  ModemManager();
  void begin();
  void update();          // Cek teks USR, reconnect, dsb.
  bool available();
  int read();
  void write(const uint8_t* buf, size_t len);
  // Tambah method sesuai kebutuhan
private:
  // Hardware serial/software serial di sini (Stream*)
};

#endif
