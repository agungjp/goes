# 📡 IEC 60870-5-104 Arduino Slave – v1.4.4

Proyek ini adalah implementasi protokol **IEC 60870-5-104** menggunakan **Arduino UNO** untuk keperluan komunikasi SCADA, khususnya sebagai **slave/server**.

Versi ini adalah pengembangan dari `v1.4.3` dengan tambahan dan penyempurnaan:
- 🔁 Sinkronisasi waktu dari SCADA (TI 103) ke RTC DS3231
- ⏱️ Perbaikan parsing CP56Time2a dan offset memcpy
- 🛡️ Validasi frame sinkronisasi waktu dan IOA
- 🧠 Perbaikan COS agar hanya kirim jika status DI berubah
- 📶 Penanganan reconnect TCP dan kirim ulang status
- 🔧 Tambahan komentar, dokumentasi, dan modularisasi kode
- 🧪 Logging debug untuk frame, waktu, dan NS/NR

---

## 👤 AUTHOR & LICENSE

- **Author** : Mr. Pegagan  
- **Email**  : agungjulianperkasa@gmail.com  
- **License**: GNU Lesser General Public License v3.0

---

## 1. 🎯 TUJUAN PROGRAM

- Menjadi **slave/server IEC 104**
- Komunikasi TCP dengan **SCADA Master**
- Kirim status DI:
  - Remote/Local
  - GFD
  - CB (Open/Close)
- Eksekusi perintah DO: TI 46 (Open/Close)
- Timestamp CP56Time2a dari RTC DS3231
- Sinkronisasi waktu dari SCADA (TI 103)

---

## 2. 🧱 STRUKTUR FILE

| File             | Fungsi                           |
|------------------|----------------------------------|
| `goes.ino`       | Program utama (setup & loop)     |
| `IEC104Slave.h`  | Header class IEC104Slave         |
| `IEC104Slave.cpp`| Implementasi logika protokol     |
| `README.md`      | Dokumentasi versi                |

---

## 3. 🧩 FITUR UTAMA v1.4.4

| Fitur                                               | Status |
|-----------------------------------------------------|--------|
| STARTDT_ACT / TEST ACT response                     | ✅     |
| General Interrogation (TI 100)                      | ✅     |
| TI 30 – Remote, GFD status                          | ✅     |
| TI 31 – CB status (Double Point + Time)             | ✅     |
| COS – hanya kirim saat status berubah               | ✅     |
| CP56Time2a – encode dari RTC DS3231                 | ✅     |
| TI 46 – Double Command: OPEN/CLOSE                  | ✅     |
| Proteksi TI 46: Mode Remote & Status berbeda        | ✅     |
| ACK (COT=7) dan Termination (COT=10)                | ✅     |
| NS/NR sinkron dari master (Control Field)           | ✅     |
| Sinkronisasi RTC dari master (TI 103)               | ✅     |
| Penanganan reconnect TCP + kirim ulang status       | ✅     |
| Debug log: CP56Time2a, frame RX/TX, NS/NR           | ✅     |
| Modular class IEC104Slave                           | ✅     |

---

## 4. ⚙️ KONFIGURASI PIN

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
| 46   | 23000   | Double Command (Open/Close) |

---

## 6. 📥 FRAME YANG DIDUKUNG

| TI       | Fungsi                              |
|----------|--------------------------------------|
| 64       | General Interrogation (TI 100)       |
| 46       | Double Command (TI 46)               |
| 67       | RTC Sync (TI 103 – Clock Set)        |
| 1, 3     | Status DI (tanpa timestamp)          |
| 30, 31   | Status DI (dengan CP56Time2a)        |
| U-Format | STARTDT_ACT, TESTFR_ACT              |
| S-Format | Frame ACK / NS update                |

---

## 7. 🕒 FORMAT TIMESTAMP (CP56Time2a)

| Byte | Keterangan                  |
|------|-----------------------------|
| 0–1  | Millisecond (Little Endian) |
| 2    | Minute (6-bit)              |
| 3    | Hour (5-bit)                |
| 4    | Date (5-bit) + DayOfWeek    |
| 5    | Month (4-bit)               |
| 6    | Year (offset +2000)         |

---

## 8. 🧠 LOGIKA `handleRTC()` TI 103

```cpp
void IEC104Slave::handleRTC(const byte* buf, byte len) {
  const byte* time = &buf[15];         // Ambil 7 byte CP56Time2a
  setRTCFromCP56(time);                // Set RTC dari data master
  byte ack[16] = {
    0x67, 0x01, 0x07, 0x00,            // TI=103, VSQ=1, COT=7 (ACK)
    0x01, 0x00, 0x00, 0x00, 0x00       // CA=1, IOA=0
  };
  memcpy(&ack[9], time, 7);            // Salin CP56Time2a ke posisi benar
  sendIFrame(ack, sizeof(ack));        // Kirim balasan ke master
}```

 ## 9. 🔁 FLOW NS / NR
	•	rxSeq ← dari NS master (I-format)
	•	txSeq ← dari NR master (S-format)
	•	Sinkron otomatis
	•	Debug NS/NR jika aktifkan #define DEBUG

[CLOSED / CONNECT] → Deteksi modem
   ↓
[Menunggu STARTDT_ACT]
   ↓
← STARTDT_ACT → Kirim STARTDT_CON
   ↓
→ Kirim ulang status (TI 30, 31)


 ## 10. 📑 CHANGELOG RINGKAS – v1.4.4

	•	✅ Penambahan fungsi sinkronisasi waktu (TI 103)
	•	✅ Perbaikan parsing CP56Time2a dan penempatan memcpy(&ack[9], ...)
	•	✅ COS hanya kirim saat ada perubahan
	•	✅ Proteksi IOA dan validasi DO TI 46
	•	✅ Sinkronisasi NS/NR dari frame master
	•	✅ Penanganan reconnect TCP dan pengiriman ulang status
	•	✅ Dokumentasi & log diperjelas
