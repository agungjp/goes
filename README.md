# 📡 IEC 60870-5-104 Arduino Slave – v1.4.2

Proyek ini adalah implementasi protokol **IEC 60870-5-104** menggunakan **Arduino UNO** untuk keperluan komunikasi SCADA, khususnya sebagai **slave/server**.

Versi ini adalah pengembangan dari `v1.4.1` dengan tambahan:
- ✨ perbaikan program

---

## 1. 🎯 TUJUAN PROGRAM

- Menjadi slave/server IEC 104  
- Komunikasi TCP dengan SCADA  
- Kirim status DI: Remote/Local, GFD, CB Status  
- Eksekusi perintah OPEN/CLOSE (TI 46)  
- Kirim data COS otomatis saat perubahan  
- Timestamp CP56Time2a via RTC DS3231  
- Sinkronisasi waktu via TI 103  

---

## 2. 🧱 STRUKTUR FILE MODULAR

| File             | Fungsi                           |
|------------------|----------------------------------|
| `goes.ino`       | Setup & loop utama               |
| `IEC104Slave.h`  | Header class                     |
| `IEC104Slave.cpp`| Implementasi seluruh logika      |

---

## 3. 🧩 FITUR FINAL v1.4.0

| Fitur                                               | Status |
|-----------------------------------------------------|--------|
| Respon STARTDT_ACT / TEST ACT                       | ✅     |
| Respon General Interrogation (TI 100)               | ✅     |
| Kirim status TI 30 (Remote, GFD)                    | ✅     |
| Kirim status TI 31 (CB Status)                      | ✅     |
| COS – Change of State otomatis                      | ✅     |
| CP56Time2a dari RTC DS3231                          | ✅     |
| TI 46 – Double Command (OPEN/CLOSE)                 | ✅     |
| Proteksi TI 46: Mode Remote + Status berbeda        | ✅     |
| ACK (COT=7) + Termination (COT=10)                  | ✅     |
| NS / NR sinkron otomatis                            | ✅     |
| RTC Sync dari master (TI 103)                       | ✅     |
| Struktur class modular (IEC104Slave)                | ✅     |
| Log debug NS/NR dan RTC (opsional #define DEBUG)    | ✅     |

---

## 4. 📌 KONFIGURASI PIN

| Fungsi            | Arduino Pin |
|-------------------|-------------|
| Remote / Local    | D2          |
| GFD Status        | D3          |
| CB Status (Bit 1) | D4          |
| CB Status (Bit 2) | D5          |
| Relay CB OPEN     | D6 (PD6)    |
| Relay CB CLOSE    | D7 (PD7)    |

---

## 5. 📤 DATA YANG DIKIRIM

| TI   | IOA     | Keterangan              |
|------|---------|--------------------------|
| 30   | 1001    | Remote/Local (SP+Time)   |
| 30   | 1002    | GFD                      |
| 31   | 11000   | CB Status (DP+Time)      |
| 46   | 23000*  | Double Command           |

---

## 6. ⚙️ PERANGKAT KERAS

| Komponen         | Fungsi                           |
|------------------|----------------------------------|
| Arduino UNO      | Mikrokontroler utama             |
| RTC DS3231       | Waktu real-time (CP56Time2a)     |
| Relay Module     | Kontrol CB OPEN/CLOSE            |
| Modem Serial/GSM | Komunikasi TCP ke SCADA          |
| Saklar / Tombol  | Simulasi status DI               |

---

## 7. 📥 FRAME YANG DITANGANI

| TI       | Fungsi                                |
|----------|----------------------------------------|
| 64       | General Interrogation (TI 100)         |
| 46       | Double Command                         |
| 67       | RTC Sync (TI 103)                      |
| U-Format | STARTDT_ACT, TEST_ACT                  |
| S-Format | ACK Frame (update txSeq)               |

---

## 8. 📘 PENJELASAN PROGRAM PENTING

### a. 🔐 PROTEKSI PERINTAH TI 46

Perintah hanya dieksekusi jika:
- Mode = Remote  
- Status CB berbeda dari perintah  
- SCO valid (1 = OPEN, 2 = CLOSE)

> Tetap kirim **ACK** dan **Termination** meskipun perintah ditolak.

---

### b. 🔄 COS (Change of State)

Perubahan pada:
- D2 → Remote/Local  
- D3 → GFD  
- D4 & D5 → CB Status  

Akan otomatis mengirim TI 30 atau TI 31 + timestamp CP56Time2a.

---

### c. ⏱️ TIMESTAMP (CP56Time2a)

| Byte | Keterangan                |
|------|---------------------------|
| 0–1  | Millisecond (little endian) |
| 2    | Minute                    |
| 3    | Hour                      |
| 4    | Date + Day of Week        |
| 5    | Month                     |
| 6    | Year (offset 2000)        |

Digunakan untuk semua frame TI 30 / 31 dan COS.

---

### d. 🔁 NS / NR

- `rxSeq` ← dari NS master (I-format)  
- `txSeq` ← dari NR master (S-format)  
- Sinkronisasi otomatis  
- Log NS/NR aktif jika `#define DEBUG`

---

### e. 🕒 RTC SYNC – TI 103

- Slave menerima frame TI 103 dari SCADA  
- Waktu diambil dari CP56Time2a (7 byte)  
- RTC DS3231 langsung di-set ke waktu terbaru  
- Output:
