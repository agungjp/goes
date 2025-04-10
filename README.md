# 📡 IEC 60870-5-104 Arduino Slave – v1.2.0

Implementasi **protokol IEC 60870-5-104** menggunakan **Arduino UNO** sebagai **slave/server** untuk sistem **SCADA**.  
Program ini mendukung pengiriman data dengan **timestamp CP56Time2a**, serta fitur **Change of State (COS)**.

---

## 🏷️ Versi

**Versi**: v1.2.0  
**Tanggal**: 2025-04-10  
**Fitur Baru**:
- Timestamped data (TI 30 & TI 31)
- COS (Change of State) → kirim data jika input berubah
- RTC DS3231 (I2C)
- Penanganan U-format, S-format, I-format
- Struktur file modular (3 file: `.ino`, `.h`, `.cpp`)

---

## 🎯 Tujuan

- Menghubungkan Arduino UNO ke SCADA menggunakan IEC 104
- Mengirim status input (remote/local, GFD, CB status)
- Memberikan timestamp waktu nyata (dari RTC DS3231)
- Menjawab general interrogation (TI 100)
- Mengirim data saat status input berubah otomatis (COS)

---

## ⚙️ Perangkat Keras

| Komponen         | Keterangan                            |
|------------------|----------------------------------------|
| Arduino UNO       | Mikrokontroler utama                   |
| RTC DS3231        | Modul waktu real-time (I2C)            |
| Modul Relay       | Relay untuk kontrol CB OPEN/CLOSE      |
| Saklar/Tombol     | Simulasi input digital                 |
| GSM / Ethernet    | Komunikasi TCP port 2404 (IEC 104)     |

---

## 📌 Definisi Pin Arduino

| Fungsi            | Pin |
|-------------------|-----|
| Remote / Local    | D2  |
| GFD Status        | D3  |
| CB Status Bit 1   | D4  |
| CB Status Bit 2   | D5  |
| Relay CB OPEN     | D6  |
| Relay CB CLOSE    | D7  |

---

## 🧠 Logika Status CB

| D4 | D5 | Status     |
|----|----|------------|
| 0  | 0  | UNKNOWN    |
| 1  | 0  | CB OPEN    |
| 0  | 1  | CB CLOSE   |
| 1  | 1  | UNKNOWN    |

---

## 🔄 Fitur IEC 60870-5-104

| Fitur                | Keterangan                                   |
|----------------------|----------------------------------------------|
| U-format             | Menjawab STARTDT_ACT dan TEST ACT otomatis   |
| S-format             | Parsing NR untuk sinkronisasi txSequence     |
| I-format             | Kirim data lengkap sesuai format IEC 104     |
| General Interrogation| Kirim: ACT_CON → TI30/31 → ACT_TERM          |
| NS / NR              | Dikelola otomatis                            |
| Timestamp            | Format CP56Time2a (7 byte) dari RTC DS3231   |

---

## ⚡ Change of State (COS)

Program mendeteksi dan mengirim **data otomatis** saat terjadi perubahan nilai input:

| Input            | IOA   | TI    |
|------------------|-------|-------|
| Remote / Local   | 1001  | TI 30 |
| GFD              | 1002  | TI 30 |
| CB Status        | 11000 | TI 31 |

---

## 🕒 Format Timestamp (CP56Time2a)

| Byte | Isi             |
|------|-----------------|
| 0–1  | Millisecond (LE)|
| 2    | Minute          |
| 3    | Hour            |
| 4    | Day + DayOfWeek |
| 5    | Month           |
| 6    | Year - 2000     |

---

## 📤 Tipe Data IEC 104 yang Didukung

| Tipe         | Deskripsi                        | TI   | IOA    |
|--------------|----------------------------------|------|--------|
| Single Point | Remote / Local                   | 30   | 1001   |
| Single Point | GFD                              | 30   | 1002   |
| Double Point | CB Status (Open / Close)         | 31   | 11000  |

---

## 📁 Struktur File

| File         | Fungsi                                 |
|--------------|----------------------------------------|
| `goes.ino`   | Program utama Arduino                  |
| `iec104.h`   | Header class & konfigurasi pin/status  |
| `iec104.cpp` | Implementasi logika protokol + RTC + COS |

---

## 📌 Selanjutnya Bisa Dikembangkan ke:

- 🔘 TI 46 (perintah Open/Close dari SCADA)
- 🧾 Logging event perubahan ke EEPROM atau file
- 🕒 Kirim event TI 30/31 secara periodik

---

