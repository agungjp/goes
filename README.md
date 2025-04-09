# GOES - IEC 60870-5-104 Arduino Project

Proyek ini adalah implementasi protokol **IEC 60870-5-104** menggunakan **Arduino UNO** untuk keperluan komunikasi SCADA, khususnya sebagai *slave/server*.

## 🎯 Tujuan

- Menerima perintah dari master (seperti `CB OPEN` dan `CB CLOSE`)
- Mengontrol output relay sesuai perintah
- Membaca status digital input (DI) seperti:
  - Mode Remote/Local
  - Status GFD
  - Status CB (Open/Close)

## ⚙️ Perangkat Keras

- Arduino UNO
- Modul relay 2 channel
- Input digital menggunakan saklar/tombol atau simulasi sinyal eksternal
- Koneksi Ethernet (opsional untuk IEC 104)
- Modul RTC (misal DS1302) untuk timestamp (jika digunakan)

## 📌 Definisi Pin

| Fungsi              | Arduino Pin |
|---------------------|-------------|
| Remote/Local        | D2          |
| GFD Status          | D3          |
| CB Status (bit 1)   | D4          |
| CB Status (bit 2)   | D5          |
| Relay CB OPEN       | D6 (PD6)    |
| Relay CB CLOSE      | D7 (PD7)    |

## 🧠 Logika CB Status

- `D4 = 0, D5 = 0` → UNKNOWN
- `D4 = 1, D5 = 0` → CB OPEN
- `D4 = 0, D5 = 1` → CB CLOSE
- `D4 = 1, D5 = 1` → UNKNOWN

## 🖥️ Interaksi Serial

Ketik perintah berikut via Serial Monitor (baud 9600):

- `CB OPEN` → Aktifkan relay CB OPEN selama 800ms
- `CB CLOSE` → Aktifkan relay CB CLOSE selama 800ms

⚠️ Perintah hanya akan dijalankan jika:
- Mode = Remote
- Status CB berbeda dari perintah yang dikirim

## 🔄 Integrasi IEC 104

Proyek ini dirancang untuk mendukung integrasi ke IEC 60870-5-104 dengan:
- Format frame sesuai SPLN S4.003-2011
- Tipe data: TI 1, 3, 30, 31, 46, 100
- Dukungan timestamp (CP56Time2a)
- Simulasi I/O

## 📁 Struktur File
