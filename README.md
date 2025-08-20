# Gardu Online Surveillance (GOES) - IEC 60870-5-104 Slave

![PlatformIO CI](https://github.com/agungjp/goes/actions/workflows/ci.yml/badge.svg)

**Versi: 2.0.0**

Firmware yang fleksibel dan modular untuk membangun Remote Terminal Unit (RTU) berbasis protokol IEC 60870-5-104 pada platform ESP32. Proyek ini dirancang untuk menjadi fondasi yang mudah diadaptasi untuk berbagai kebutuhan surveilans dan kontrol gardu listrik.

## ✨ Fitur Utama

- **Protokol IEC 104:** Implementasi sisi *slave* (controlled station).
- **Dukungan Komunikasi Modular:**
  - Ethernet (W5500)
  - Modem Seluler (SIM800L, SIM7600CE, Quectel EC25)
- **Abstraksi Hardware:** Memudahkan penambahan atau perubahan komponen hardware seperti sensor dan aktuator.
- **Manajemen Waktu:** Sinkronisasi waktu menggunakan RTC (DS3231) dan dukungan *Clock Synchronization* dari master.
- **Dibangun dengan PlatformIO:** Konfigurasi build yang modern dan manajemen dependensi yang mudah.

## 🛠️ Memulai

Proyek ini dikelola menggunakan PlatformIO. Direkomendasikan menggunakan Visual Studio Code dengan ekstensi PlatformIO IDE.

### 1. Persiapan

- Install [Visual Studio Code](https://code.visualstudio.com/)
- Install ekstensi [PlatformIO IDE](https://platformio.org/platformio-ide) dari marketplace VSCode.

### 2. Clone Repository

```bash
git clone https://github.com/agungjp/goes.git
cd goes
```

### 3. Build dan Upload

Proyek ini menggunakan *Build Environments* di PlatformIO untuk memilih modul komunikasi yang akan digunakan. Anda tidak perlu mengubah kode untuk berganti modul.

Buka file `platformio.ini` untuk melihat daftar environment yang tersedia, contohnya:
- `esp32dev-ethernet`
- `esp32dev-sim800l`
- `esp32dev-sim7600ce`
- `esp32dev-quectel-ec25`

**Untuk Build:**
Gunakan perintah `pio run` dengan flag `-e` untuk memilih environment.

```bash
# Contoh: Build untuk target dengan Ethernet
pio run -e esp32dev-ethernet
```

**Untuk Upload ke Hardware:**
Tambahkan target `-t upload`.

```bash
# Contoh: Upload ke ESP32 dengan modul Ethernet
pio run -e esp32dev-ethernet -t upload
```

### 4. Konfigurasi

- **Pin Hardware:** Pengaturan pin untuk sensor, relay, dan LED dapat ditemukan di `src/config/board_esp32.h`.
- **Konfigurasi Global:** Pengaturan lain seperti alamat IP (untuk Ethernet) dan konfigurasi modul komunikasi berada di `src/config/goes_config.h`.

## 👤 AUTHOR & LICENSE

- **Author**: Mr. Pegagan
- **Email**: agungjulianperkasa@gmail.com
- **License**: MIT License
