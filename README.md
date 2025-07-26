## 👤 AUTHOR & LICENSE

- **Author** : Mr. Pegagan  
- **Email**  : agungjulianperkasa@gmail.com  
- **License**: GNU Lesser General Public License v3.0

---

# 📡 IEC 60870-5-104 Arduino Slave – v1.6.2

## 📑 CHANGELOG

### v1.6.2 (26/07/2025)
- Add debouncing for remote inputs to prevent flickering.
- Replace blocking `delay()` with non-blocking `millis()` for more reliable modem data reading.

### v1.6.1 (20/07/2025)
- Refactored to support a second Circuit Breaker (CB).
- Updated pin definitions, Change of State (COS) logic, and double command handling for two CBs.

### v1.6.0 (12/07/2025)
- Enabled `DEBUG` mode for easier troubleshooting.

### v1.4.5 (Undated)
- Implemented a watchdog timer to automatically reset the device if no `TESTFR_ACT` is received for 5 minutes, improving long-term stability.

### v1.4.3 (Undated)
- Added logic to automatically restart the modem if the connection is lost.

### v1.4.0 (Undated)
- Implemented **TI 103** for clock synchronization from the master.
- Added NS/NR logging for debugging communication flow.

### v1.3.0 (Undated)
- Implemented **TI 46 (Double Command)** to allow the master to control relays (Open/Close).

### v1.2.0 (Undated)
- Implemented **Change of State (COS)** to send data only when values change.
- Added **CP56Time2a timestamping** for all data points, read from an external **RTC DS3231**.
- Implemented **TI 30** (Single Point with Time) and **TI 31** (Double Point with Time).

### Pre-v1.2.0 (Undated)
- Initial implementation of the IEC 104 protocol structure.
- Support for **General Interrogation (GI)**, **STARTDT**, and **TESTFR_ACT**.
- Basic relay control functionality.

---