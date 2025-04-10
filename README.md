# 📡 IEC 60870-5-104 Arduino Slave – v1.4.0 (Refactor)

Proyek ini adalah implementasi protokol **IEC 60870-5-104** menggunakan **Arduino UNO** untuk keperluan komunikasi SCADA, khususnya sebagai **slave/server**.

Versi ini adalah pengembangan dari `v1.3.1` dengan tambahan:
- ✨ **RTC Sync via TI 103**
- ✨ Struktur class modular OOP (`IEC104Slave`)
- ✨ Logging NS/NR (debug mode)

---

## 🎯 Tujuan

- Menerima perintah dari master (CB OPEN/CLOSE)
- Mengontrol output relay sesuai perintah
- Membaca status digital input (DI):
  - Mode Remote/Local
  - Status GFD
  - Status CB (Open/Close)
- Menjawab Interrogation SCADA
- Mengirim data COS + timestamp
- Sinkronisasi waktu RTC dari SCADA (TI 103)

---

## ⚙️ Perangkat Keras

| Komponen       | Fungsi                             |
|----------------|------------------------------------|
| Arduino UNO    | Mikrokontroler utama               |
| Modul relay    | Kontrol CB OPEN / CLOSE            |
| RTC DS3231     | Sumber waktu real (via I2C)        |
| Saklar/tombol  | Simulasi input digital             |
| Modem (AT Cmd) | Koneksi TCP ke SCADA (port 2404)   |

---

## 📌 Definisi Pin

| Fungsi            | Arduino Pin |
|-------------------|-------------|
| Remote / Local    | D2          |
| GFD Status        | D3          |
| CB Status (Bit 1) | D4          |
| CB Status (Bit 2) | D5          |
| Relay CB OPEN     | D6 (PD6)    |
| Relay CB CLOSE    | D7 (PD7)    |

---

## 🧱 Struktur Program

| File             | Fungsi                               |
|------------------|----------------------------------------|
| `goes.ino`       | Program utama (setup & loop)           |
| `IEC104Slave.h`  | Header class: deklarasi pin & status   |
| `IEC104Slave.cpp`| Implementasi semua fungsi protokol     |

---

## 📤 Data yang Dikirim

| Tipe Info | TI | IOA     | Keterangan                       |
|-----------|----|---------|----------------------------------|
| SP+Time   | 30 | 1001    | Mode Remote/Local                |
| SP+Time   | 30 | 1002    | Status GFD                       |
| DP+Time   | 31 | 11000   | Status CB (Open/Close)           |
| Command   | 46 | 23000*  | Kontrol CB OPEN/CLOSE            |

---

## 🧩 Fitur-Fitur Lengkap v1.4.0

| Fitur                                               | Status |
|-----------------------------------------------------|--------|
| 📥 Respon **STARTDT_ACT** / **TEST ACT**            | ✅     |
| 🧠 Respon **General Interrogation (TI 100)**        | ✅     |
| 📤 Kirim status **TI 30** (Remote, GFD)             | ✅     |
| 📤 Kirim status **TI 31** (CB Status)               | ✅     |
| ⏱️ **CP56Time2a** dari RTC **DS3231**              | ✅     |
| 🔄 **COS – Change of State otomatis**               | ✅     |
| 🎮 **TI 46 – Double Command (OPEN/CLOSE)**          | ✅     |
| 🔐 Proteksi TI 46: Mode Remote + Status berbeda     | ✅     |
| 📡 Kirim **ACK (COT=7)** dan **Termination (COT=10)**| ✅     |
| 🔁 NS / NR sinkronisasi otomatis                    | ✅     |
| ⏱️ **Sinkronisasi waktu (TI 103)**                  | ✅     |
| 🧩 Struktur class OOP modular                       | ✅     |
| 🔍 DEBUG NS/NR log (aktif jika `#define DEBUG`)     | ✅     |

---

## 🔐 Proteksi TI 46

Perintah dari SCADA hanya dieksekusi jika:
- Mode = Remote
- Status CB berbeda dari perintah
- SCO = 1 (OPEN) atau 2 (CLOSE)

> Namun, slave **tetap mengirim ACK dan TERM** meskipun perintah ditolak.

---

## 🔄 COS – Change of State

Perubahan pada:
- Remote/Local
- GFD
- Status CB  
akan langsung mengirim data (TI 30 / TI 31) ke master dengan timestamp CP56Time2a.

---

## ⏱️ Format Timestamp CP56Time2a

| Byte | Isi                     |
|------|--------------------------|
| 0–1  | Millisecond (little-endian) |
| 2    | Minute (6-bit)          |
| 3    | Hour (5-bit)            |
| 4    | Day + Day of Week       |
| 5    | Month (4-bit)           |
| 6    | Year (offset 2000)      |

---

## 🔁 NS / NR Sinkron Otomatis

- NS (send) dan NR (ack) otomatis dibaca & disusun
- Slave update `rxSeq` dari frame master
- Master update `txSeq` dari S-format

> Bisa dimonitor dengan `#define DEBUG` → log NS/NR muncul

---

## 🕒 Sinkronisasi Waktu (TI 103)

- Saat master kirim **TI 103** dengan CP56Time2a
- Slave akan update waktu RTC secara otomatis
- Waktu terbaru ditampilkan jika `DEBUG` aktif

