# 📡 IEC 60870-5-104 Arduino Slave – v1.3.1 (Refactor)

Implementasi protokol **IEC 60870-5-104** pada **Arduino UNO** dalam bentuk **class OOP** (`IEC104Slave`) yang modular dan scalable.  
Cocok untuk simulasi **RTU slave SCADA**, pelatihan, dan pengembangan lebih lanjut ke STM32/ESP32.

---

## 🏷️ Riwayat Versi

| Versi     | Tanggal     | Fitur Utama                                               |
|-----------|-------------|------------------------------------------------------------|
| v1.0.0    | -           | Struktur dasar, respon STARTDT/TEST                      |
| v1.1.0    | -           | Respon General Interrogation (TI 100)                    |
| v1.2.0    | 2025-04-10  | COS + Timestamp (TI 30 / 31), RTC via DS3231             |
| v1.3.0    | 2025-04-10  | TI 46 (Command CB), Proteksi, ACK + Termination          |
| **v1.3.1**| 2025-04-10  | 🔧 Refactor: OOP class, modular, siap pengembangan lanjut |

---

## 🎯 Tujuan

- Berfungsi sebagai **slave SCADA** IEC 104
- Mengirim status digital input (DI)
- Menjawab permintaan master (interrogation, command)
- Kirim data COS dengan timestamp CP56Time2a
- Eksekusi kontrol CB (relay) dari master

---

## 📂 Struktur File

| File             | Fungsi                                     |
|------------------|---------------------------------------------|
| `goes.ino`       | Entry point utama                          |
| `IEC104Slave.h`  | Header class: pin, status, fungsi utama    |
| `IEC104Slave.cpp`| Implementasi semua logika protokol         |

---

## ⚙️ Perangkat Keras

| Komponen     | Fungsi                          |
|--------------|---------------------------------|
| Arduino UNO  | Mikrokontroler utama            |
| RTC DS3231   | Waktu real-time CP56Time2a      |
| Modul Relay  | Kontrol CB (Open/Close)         |
| Modem Serial | Koneksi TCP (port 2404) via AT  |

---

## 📌 Konfigurasi Pin

| Fungsi            | Pin Arduino |
|-------------------|-------------|
| Remote / Local    | D2          |
| GFD Status        | D3          |
| CB Status (Bit 1) | D4          |
| CB Status (Bit 2) | D5          |
| Relay CB OPEN     | D6 (PD6)    |
| Relay CB CLOSE    | D7 (PD7)    |

---

## 🧩 Fitur-Fitur Lengkap v1.3.1

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

---

## 🧠 Logika Penting

### 🔐 Proteksi TI 46
Perintah hanya dieksekusi jika:
- Mode = Remote
- Perintah berbeda dari status
- SCO valid (1 = OPEN, 2 = CLOSE)

### 📤 COS (Change of State)
Jika terjadi perubahan pada:
- Remote/Local (IOA 1001)
- GFD (IOA 1002)
- CB Status (IOA 11000)  
→ Maka slave otomatis mengirim frame TI 30 / TI 31.

### 🕒 Timestamp Format CP56Time2a
- Diambil dari RTC DS3231
- Format 7-byte: ms, menit, jam, tanggal, bulan, tahun

### 🔁 NS / NR
- Dibaca dari setiap frame SCADA
- Sinkron otomatis pada I-format & S-format

---