# 📡 IEC 60870-5-104 Arduino Slave – v1.3.1 (Refactor)

Implementasi protokol **IEC 60870-5-104** berbasis Arduino UNO menggunakan struktur program yang lebih modular dan maintainable (OOP).  
Versi ini adalah **refactor dari v1.3.0** yang memisahkan logika ke dalam class `IEC104Slave`.

---

## 🏷️ Versi Program

| Versi     | Tanggal     | Fitur Utama                                               |
|-----------|-------------|-----------------------------------------------------------|
| v1.0.0    | -           | Struktur dasar IEC 104, respon STARTDT/TEST               |
| v1.1.0    | -           | Respon General Interrogation (TI 100)                     |
| v1.2.0    | 2025-04-10  | COS + Timestamp (TI 30 / 31), RTC via DS3231              |
| v1.3.0    | 2025-04-10  | TI 46 (Double Command), Proteksi Mode, ACK+TERM           |
| **v1.3.1**| 2025-04-10  | Refactor class OOP, lebih modular dan siap dikembangkan   |

---

## 🎯 Tujuan

- Mengirim status DI (Remote/Local, GFD, CB Status) ke master SCADA
- Menjawab interogasi SCADA (TI 100)
- Merespon perintah CB (TI 46 OPEN/CLOSE)
- Kirim timestamp CP56Time2a via RTC DS3231
- Mengirim otomatis saat ada perubahan status (COS)

---

## ⚙️ Perangkat Keras

| Komponen     | Fungsi                          |
|--------------|---------------------------------|
| Arduino UNO  | Mikrokontroler utama            |
| RTC DS3231   | Waktu real-time                 |
| Modul Relay  | Kontrol CB (Open/Close)         |
| Modem (GSM)  | Koneksi TCP port 2404           |

---

## 🧱 Struktur File

| File             | Keterangan                            |
|------------------|----------------------------------------|
| `goes.ino`       | Entry point program                    |
| `IEC104Slave.h`  | Header class untuk logika IEC104       |
| `IEC104Slave.cpp`| Implementasi lengkap fungsi protokol   |

---

## 🧠 Input / Output

| Fungsi            | Pin Arduino | Keterangan                   |
|-------------------|-------------|------------------------------|
| Remote / Local    | D2          | LOW = Remote                 |
| GFD Status        | D3          | HIGH/LOW (GFD On/Off)        |
| CB Status Bit 1   | D4          |                              |
| CB Status Bit 2   | D5          |                              |
| Relay CB OPEN     | D6          | Aktif 800ms untuk buka CB    |
| Relay CB CLOSE    | D7          | Aktif 800ms untuk tutup CB   |

---

## 📤 Data yang Dikirim

| Tipe Info | TI   | IOA     | Keterangan                           |
|-----------|------|---------|--------------------------------------|
| SP+Time   | 30   | 1001    | Remote / Local                       |
| SP+Time   | 30   | 1002    | GFD Status                           |
| DP+Time   | 31   | 11000   | Status CB (Open/Close/Unknown)       |
| Command   | 46   | dari master | Perintah CB OPEN/CLOSE           |

---

## 🔐 Proteksi Perintah TI 46

Perintah hanya dieksekusi jika:
- Mode = **Remote**
- Perintah berbeda dari status saat ini
- SCO valid (1 = OPEN, 2 = CLOSE)

Tetap kirim **ACK (COT = 7)** dan **Termination (COT = 10)** meskipun ditolak.

---

## 🔄 COS – Change of State

Jika status berikut berubah, maka otomatis akan dikirim ke SCADA:

| Parameter       | TI   | IOA   |
|------------------|------|--------|
| Remote / Local   | 30   | 1001   |
| GFD              | 30   | 1002   |
| CB Status        | 31   | 11000  |

---

## 🕒 Format Timestamp – CP56Time2a

Format 7-byte waktu dari RTC (untuk TI 30 / TI 31):

| Byte | Isi                |
|------|---------------------|
| 0-1  | Millisecond (LE)   |
| 2    | Menit              |
| 3    | Jam                |
| 4    | Hari + Hari Minggu |
| 5    | Bulan              |
| 6    | Tahun (offset 2000)|

---

## 📦 Fitur Pendukung

- AT Command untuk modem TCP
- NS/NR parsing & tracking
- Output serial log frame Master/Slave
- Buffer aman (MAX 64 byte)

---