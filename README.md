# 📡 IEC 60870-5-104 Arduino Slave – v1.3.0

Proyek ini adalah implementasi protokol **IEC 60870-5-104** pada **Arduino UNO** sebagai **slave/server** untuk komunikasi SCADA berbasis TCP (port 2404).

---

## 🏷️ Versi Program

| Versi    | Tanggal     | Fitur Utama                                                      |
|----------|-------------|------------------------------------------------------------------|
| v1.0.0   | -           | Struktur dasar program, komunikasi TCP, respon STARTDT/TEST     |
| v1.1.0   | -           | Jawaban General Interrogation (TI 100) dengan ACT_CON & ACT_TERM|
| v1.2.0   | 2025-04-10   | COS (Change of State), Timestamp (CP56Time2a), TI 30 & 31       |
| **v1.3.0** | 2025-04-10 | ✅ TI 46: Eksekusi perintah CB OPEN/CLOSE + ACK & Termination    |

---

## 🎯 Tujuan

- Menerima perintah dari SCADA (TI 46, TI 100)
- Mengirim status input digital
- Kirim data dengan timestamp (TI 30/31)
- Menjawab interogasi umum
- Deteksi perubahan input dan kirim otomatis (COS)

---

## ⚙️ Perangkat Keras

| Komponen        | Keterangan                            |
|-----------------|----------------------------------------|
| Arduino UNO     | Mikrokontroler utama                   |
| RTC DS3231      | Waktu real-time (via I2C)              |
| Modul Relay     | Kontrol CB OPEN (D6) & CB CLOSE (D7)   |
| Saklar/Tombol   | Simulasi input digital                 |
| Modem Serial    | Komunikasi TCP via AT Command          |

---

## 📌 Konfigurasi Pin

| Fungsi            | Arduino Pin |
|-------------------|-------------|
| Remote / Local    | D2          |
| GFD Status        | D3          |
| CB Status Bit 1   | D4          |
| CB Status Bit 2   | D5          |
| Relay CB OPEN     | D6          |
| Relay CB CLOSE    | D7          |

---

## 🧠 Status CB

| D4 | D5 | Status     |
|----|----|------------|
| 0  | 0  | UNKNOWN    |
| 1  | 0  | CB OPEN    |
| 0  | 1  | CB CLOSE   |
| 1  | 1  | UNKNOWN    |

---

## 🔄 Protokol IEC 104

| Tipe Info | TI   | IOA     | Keterangan                          |
|-----------|------|---------|-------------------------------------|
| SP + Time | 30   | 1001    | Remote/Local (0/1)                  |
| SP + Time | 30   | 1002    | GFD Status (0/1)                    |
| DP + Time | 31   | 11000   | CB Status (0:?, 1:Open, 2:Close, 3:?) |
| Command   | 46   | bebas   | Perintah CB OPEN/CLOSE              |

---

## 🔐 Proteksi Perintah (TI 46)

Perintah dari SCADA hanya **dieksekusi** jika:

- ✅ Mode = **Remote**
- ✅ Status CB saat ini **berbeda**
- ✅ Nilai SCO = 1 (OPEN) atau 2 (CLOSE)

Perintah tetap akan dikirim ACK + Termination, meskipun tidak dieksekusi.

---

## 📤 Laporan Perubahan (COS)

Program mengirim data otomatis saat terjadi perubahan:

- Remote/Local → TI 30, IOA 1001
- GFD → TI 30, IOA 1002
- CB Status → TI 31, IOA 11000

---

## ⏱️ Format Waktu: CP56Time2a

7 byte timestamp dikirim pada TI 30 dan 31:

| Byte | Isi              |
|------|------------------|
| 0–1  | Milliseconds     |
| 2    | Minutes          |
| 3    | Hours            |
| 4    | Date + DOW       |
| 5    | Month            |
| 6    | Year (offset 2000)|

---

## 📁 Struktur File

| File         | Keterangan                               |
|--------------|-------------------------------------------|
| `goes.ino`   | Program utama Arduino                     |
| `iec104.h`   | Header class, definisi pin/status         |
| `iec104.cpp` | Implementasi protokol + COS + TI 46       |

---

## 🚀 Siap Digunakan Untuk

- RTU / Slave SCADA berbasis IEC 104
- Simulasi sistem proteksi & kontrol CB
- Pelatihan komunikasi protokol SCADA

---