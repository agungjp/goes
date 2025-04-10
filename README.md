<<<<<<< HEAD
# 📡 IEC 60870-5-104 Arduino Slave (SCADA Integration)

Implementasi protokol **IEC 60870-5-104** menggunakan **Arduino UNO** untuk keperluan komunikasi **SCADA** sebagai **slave/server**.
=======
# 📡 IEC 60870-5-104 Arduino Slave – Timestamp & COS

Proyek ini adalah implementasi **protokol IEC 60870-5-104** menggunakan **Arduino UNO** sebagai **slave/server** untuk sistem **SCADA**. Dilengkapi fitur **timestamp** (CP56Time2a) dan **Change of State (COS)**.
>>>>>>> da3af0a (update program ti 30 dan 31)

---

## 🎯 Tujuan

<<<<<<< HEAD
- Menerima perintah dari master (misalnya: **CB OPEN**, **CB CLOSE**)
- Mengontrol **relay output** sesuai perintah
- Membaca status **Digital Input (DI)**:
  - Mode **Remote / Local**
  - Status **GFD**
  - Status **CB** (Open/Close)

---

## ⚙️ Fitur yang Sudah Jalan
Fitur	Status	Keterangan
STARTDT_ACT / TEST ACT	✅	Terima dan jawab otomatis
General Interrogation (TI100)	✅	ACT_CON + TI1 + TI3 + ACT_TERM
Parsing input digital	✅	Remote/Local, GFD, CB status
Serial Command (CB OPEN/CLOSE)	✅	Dijalankan hanya saat mode Remote & status beda
NS / NR handling	✅	NS naik tiap TX, NR dari Master diproses
S-format parsing	✅	NR dari master → sinkronisasi txSequence

## ⚙️ Perangkat Keras

| Komponen              | Keterangan                     |
|----------------------|---------------------------------|
| Arduino UNO          | Mikrokontroler utama            |
| Modul relay 2CH      | Kontrol output CB               |
| Saklar / tombol      | Simulasi sinyal input           |
| Ethernet Shield (opsional) | Untuk IEC 104 TCP/IP  |
| RTC Module (opsional)| DS1302/DS3231 untuk timestamp   |
=======
- Menerima perintah dari master SCADA (misalnya: General Interrogation)
- Mengirim status input dengan **timestamp**
- Mendeteksi perubahan input (COS) dan mengirim otomatis
- Menangani protokol standar: **I-format**, **S-format**, **U-format**

---

## ⚙️ Perangkat Keras

| Komponen         | Keterangan                      |
|------------------|----------------------------------|
| Arduino UNO       | Mikrokontroler utama             |
| RTC DS3231        | Modul waktu real-time via I2C    |
| Relay Module      | Kontrol output (CB Open/Close)   |
| Saklar / tombol   | Simulasi sinyal input digital    |
| GSM / Ethernet    | Komunikasi TCP (port 2404)       |
>>>>>>> da3af0a (update program ti 30 dan 31)

---

## 📌 Definisi Pin

<<<<<<< HEAD
| Fungsi                  | Arduino Pin |
|-------------------------|-------------|
| Remote / Local          | D2          |
| GFD Status              | D3          |
| CB Status (bit 1)       | D4          |
| CB Status (bit 2)       | D5          |
| Relay CB OPEN           | D6 (PD6)    |
| Relay CB CLOSE          | D7 (PD7)    |
=======
| Fungsi             | Arduino Pin |
|--------------------|-------------|
| Remote / Local     | D2          |
| GFD Status         | D3          |
| CB Status bit 1    | D4          |
| CB Status bit 2    | D5          |
| Relay CB OPEN      | D6 (PD6)    |
| Relay CB CLOSE     | D7 (PD7)    |
>>>>>>> da3af0a (update program ti 30 dan 31)

---

## 🧠 Logika Status CB

| D4 | D5 | Status     |
|----|----|------------|
| 0  | 0  | UNKNOWN    |
| 1  | 0  | CB OPEN    |
| 0  | 1  | CB CLOSE   |
| 1  | 1  | UNKNOWN    |

---

<<<<<<< HEAD
## 🖥️ Interaksi via Serial Monitor

- **Ketik** perintah berikut di Serial Monitor (baud: `9600`):
  - `CB OPEN` → Aktifkan relay OPEN selama 800ms
  - `CB CLOSE` → Aktifkan relay CLOSE selama 800ms

⚠️ Perintah **hanya akan dieksekusi** jika:
- Mode = **Remote**
- Status CB saat ini **berbeda** dengan perintah yang dikirim

---

## 🔄 Integrasi IEC 60870-5-104

Program mendukung komunikasi dengan master SCADA menggunakan protokol **IEC 60870-5-104**, mengikuti spesifikasi **SPLN S4.003-2011**.

### ✔️ Fitur IEC 104:

- **I-format**, **S-format**, dan **U-format** frame
- Parsing & handling:
  - `STARTDT_ACT`, `TEST ACT` → otomatis dijawab
  - `General Interrogation (TI 100)` → dijawab dengan TI 1, TI 3, ACT_TERM
- Struktur ASDU sesuai standar
- Pengelolaan `NS` dan `NR` (sequence number)
- Disiapkan untuk mendukung `TI 30`, `TI 31` (timestamp) dan `TI 46` (command)

---

## 🧾 Definisi ASDU (Information Object Address - IOA)

| Tipe                | IOA    | Deskripsi                  |
|---------------------|--------|----------------------------|
| DI (Single)         | 1001   | Status Remote / Local      |
| DI (Single)         | 1002   | Status GFD                 |
| DI (Double)         | 11000  | Status CB (Double Point)   |
| DO (Double Command) | 23000  | Control CB OPEN / CLOSE    |

---

## 📁 Struktur File (3 File Sederhana)

| File         | Fungsi                                       |
|--------------|----------------------------------------------|
| `goes.ino`   | Main Arduino sketch                          |
| `iec104.h`   | Header berisi class dan definisi protokol    |
| `iec104.cpp` | Implementasi logika IEC 104 & I/O handling   |

---

## 🛠️ Status

✅ Stabil untuk komunikasi dasar IEC 104  
✅ Teruji dengan frame nyata dari master SCADA  
🚧 Siap dikembangkan lebih lanjut:
- Timestamp: CP56Time2a (TI 30/31)
- Command control: TI 46
- Event logging

---

=======
## 🔄 Integrasi IEC 60870-5-104

Program mendukung komunikasi **IEC 60870-5-104 TCP/IP** berdasarkan **SPLN S4.003-2011**.

### ✔️ Fitur Protokol:

| Fitur                | Keterangan                                   |
|----------------------|----------------------------------------------|
| STARTDT_ACT          | Dikenali dan dijawab (STARTDT_CON)          |
| TEST ACT             | Dikenali dan dijawab (TEST_CON)             |
| General Interrogation| Jawab: ACT_CON + TI 30 + TI 31 + ACT_TERM   |
| NS / NR              | Dikelola otomatis (sequence sinkronisasi)   |
| S-format (ACK)       | Digunakan untuk sinkronisasi txSequence     |

---

## 🕒 Timestamp (CP56Time2a)

- Semua data TI 30 & TI 31 dikirim dengan **timestamp 7-byte**
- Format CP56Time2a sesuai standar IEC
- Sumber waktu: **RTC DS3231**

---

## ⚡ Change of State (COS)

Program otomatis mengirim data saat terjadi perubahan input:

| Input            | IOA   | Tipe    |
|------------------|-------|---------|
| Remote / Local   | 1001  | TI 30   |
| GFD Status       | 1002  | TI 30   |
| CB Status        | 11000 | TI 31   |

Jika nilai berubah, maka:
- Fungsi `sendTimestampedData()` akan dipanggil
- Data dikirim ke master dengan waktu terkini

---

## 📤 Struktur Pengiriman Data

| Tipe Info | TI  | IOA    | Nilai     | Timestamp |
|-----------|-----|--------|-----------|-----------|
| Single
>>>>>>> da3af0a (update program ti 30 dan 31)
