# GOES – IEC 60870-5-104 Slave for ESP32

![PlatformIO CI](https://github.com/agungjp/goes/actions/workflows/ci.yml/badge.svg)

Version: 2.1.0

GOES is a flexible, modular firmware to build an IEC 60870-5-104 slave (controlled station) on ESP32. It targets RTU use-cases for substation monitoring and control, with clean layering, time stamping, and multiple communication backends.

## Highlights

- IEC 60870-5-104 (slave): STARTDT/TESTFR, I/S/U frames, GI, time sync (TI 103), double command (TI 46), COS with CP56Time2a.
- Modular communications (transport):
  - Ethernet (ESP32 native Ethernet API)
  - Cellular modems (SIM800L, SIM7600CE, Quectel EC25)
- Hardware abstraction: easy to adapt pinouts, relays, and sensors.
- Time management: RTC DS3231 with CP56Time2a conversions.
- Built with PlatformIO: reproducible builds and dependency management.
- Embedded Web UI (feature‑gated) with live status (heap, per-task stack), configuration editor, and real‑time log viewer.
- Runtime feature flags persisted in LittleFS (digital inputs, breakers, outputs, granular sensor groups, Ethernet option, RTC, web, web log capture).
- In‑memory ring buffer log (/api/logs) toggleable without reboot.

## Architecture

The codebase is structured in three clear layers:

- transport: byte-stream drivers and IEC 104 link/sequence handling
  - `src/iec104/transport/IEC104Communicator.{h,cpp}`
  - integrates with `src/comm/` backends: `CommEthernet`, `ModemCommunicator`, `CommInterface`
- protocol: payload encoding/decoding (roadmap)
  - future module for TI 30/31/46/100/103 packing/unpacking
- app: application logic and points
  - `src/iec104/core/IEC104Core.{h,cpp}` – GI, time sync, COS, double command orchestration, timestamping

Entry point:
- `src/main.cpp` wires HAL, communication backend, communicator, and core. The communicator forwards received I-frames to the core via a registered callback.

## Project Structure

```text
src/
  comm/
    CommInterface.h
    CommEthernet.{h,cpp}
    ModemCommunicator.{h,cpp}
  config/
    board_esp32.h
    config_global.h
    config_ioa.h
    device_config.h
    ioa_config.h
  hal/
    HardwareManager.{h,cpp}
    PinInterface.h
    PinESP32.{h,cpp}
  iec104/
    transport/IEC104Communicator.{h,cpp}
    core/IEC104Core.{h,cpp}
  main.cpp
test/
  test_native_logic/ (host-side tests)
  test_iec104_u/     (on-device tests)
```

## Getting Started

Prerequisites:
- Visual Studio Code
- PlatformIO IDE extension

Clone and open:
```bash
git clone https://github.com/agungjp/goes.git
cd goes
```

### Build Environments

Select the desired environment in `platformio.ini` or via CLI:
- `esp32dev-ethernet`
- `esp32dev-sim800l`
- `esp32dev-sim7600ce`
- `esp32dev-quectel-ec25`

Build:
```bash
pio run
```

Upload:
```bash
pio run -t upload
```

Serial monitor (optional):
```bash
pio device monitor -b 115200
```

### Configuration

Current unified config files (legacy `board_esp32.h`, `config_global.h`, `config_ioa.h` have been removed and merged):

- Hardware, pinout, tasks, comm selection: `src/config/device_config.h`
- IEC-104 IOA mapping (authoritative addresses): `src/config/ioa_config.h`
- Runtime persistent configuration manager (LittleFS merge & revision hash): `src/config/ConfigManager.{h,cpp}`
- IEC‑104 protocol constants & type IDs: `src/iec104/iec104_config.h`

Use the embedded Web UI (if `feat_web` enabled) to edit and persist `device` & `ioa` config JSON; files stored in LittleFS as `/device_config.json` and `/ioa_config.json`.

## Testing

- Host-side tests (no hardware):
  - environment: `[env:native]`
  - purpose: validate communicator logic and helpers without Arduino headers
  - run: `pio test -e native`

- On-device tests (Unity on ESP32):
  - environment example: `esp32dev-sim7600ce`
  - run: `pio test -e esp32dev-sim7600ce --upload-port <your-serial-port>`
  - if you only want to compile tests without uploading: `pio test -e esp32dev-sim7600ce --without-uploading --without-testing`

## Changelog

See `docs/CHANGELOG.md` for full release history. Unreleased section tracks upcoming work.

## Roadmap (Reliability & Features)

- Remove blocking delays with non-blocking schedulers.
- TX pacing via transmission queue (respecting NS/NR window), no hard `delay()` in send paths.
- SOE ring buffer (e.g., 200 events) with backpressure and retry on link restore.
- Dedicated protocol codec for TI 30/31/46/100/103.
- Periodic TESTFR and robust reconnect for modem backends.

## Contributing

Contributions are welcome. Please open an issue or a pull request with a clear description of the change, rationale, and tests when applicable.

## License

MIT License – see `LICENSE` for details.

## Author

- Author: Mr. Pegagan
- Email: agungjulianperkasa@gmail.com
