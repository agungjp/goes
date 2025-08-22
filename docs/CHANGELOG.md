# Changelog

All notable changes to this project will be documented in this file.

The format is loosely based on Keep a Changelog, and this project adheres to semantic versioning (MAJOR.MINOR.PATCH).

## [Unreleased]
- (placeholder) Dynamic runtime enable/disable of web server (feat_web) without reboot.
- (placeholder) Digital input controlled WiFi / web enable.
- (placeholder) Additional measurement types & timestamped ASDU variants.

## [2.1.0] - 2025-08-22
### Added
- Embedded Web UI overhaul: structured `/api/config2`, feature toggles, memory (heap + per-task stack) panel, live log viewer.
- LogBuffer ring buffer with `/api/logs` endpoint and dynamic enable (`feat_web_log`).
- Feature flags: `feat_web`, `feat_web_log`, extended granular IO / sensor groups.
- Per-task stack high-water metrics exposed via `/api/status`.

### Changed
- Config persistence now merges JSON (prevents unintended field loss) and skips zero IOA overwrites.
- Web server conditional startup via `feat_web` flag.
- HAL diagnostics routed through unified `LOGF` macro (captured in web log if enabled).

### Fixed
- UI previously showing zeros due to double-serialized JSON in legacy `/api/config`; replaced by structured `/api/config2`.
- Deprecated ArduinoJson `createNestedObject` usage replaced with `doc[key].to<JsonObject>()`.

### Notes
- Patched AsyncTCP locally to silence const qualifier warning (temporary; consider upstream PR / fork).

## [2.0.0] - 2025-08-20
### Added
- Major project restructure for 2.x line: new `src/iec104/{transport,core}` separation.
- Introduced unified device & IOA configuration direction (pre‑merge groundwork).
### Changed
- File / folder layout cleaned, legacy sketch renamed to `main.cpp`.
- Simplified PlatformIO environments & build configuration.
### Fixed
- Build cleanup; removed editor & generated artifacts from VCS.

## [1.6.2] - 2025-07-26
### Added
- Final refinements for 1.6.x branch (documentation + internal consistency).
### Changed
- Documentation: initial changelog scaffolding & restructuring.

## [1.6.1] - 2025-07-20
### Changed
- Incremental improvements to stability prior to 1.6.2.

## [1.4.8] - 2025-04-20
### Added
- Finalization of 1.4.x feature set.

## [1.4.5] - 2025-04-18
### Added
- Watchdog: restart logic if no TEST ACT > 5 minutes.

## [1.4.4] - 2025-04-15
### Changed
- Iterative improvements / tuning (release hardening).

## [1.4.3] - 2025-04-11
### Added
- Modem disconnect auto‑restart logic (link resilience).

## [1.4.2] - 2025-04-10 / 2025-04-11
### Changed
- Program refinements & finalization steps for 1.4.2.

## [1.4.1] - 2025-04-10
### Fixed
- Minor code fixes after 1.4.0.

## [1.4.0] - 2025-04-10
### Added
- RTC Clock Sync (TI 103).
- Extended NS/NR debug logging.
### Changed
- README updated to reflect new capabilities.

## [1.3.1] - 2025-04-10
### Changed
- Refactor / cleanup after TI 46 addition.

## [1.3.0] - 2025-04-10
### Added
- Double Command (TI 46) handling (CB control path).

## [1.2.0] - 2025-04-10
### Added
- Timestamps, COS reporting, RTC integration, TI 30 & 31.

## [1.1.x & Earlier] - 2023-12-28 → 2025-04-09
### Added
- Initial project scaffolding, basic IEC 60870-5-104 frame handling (STARTDT, TESTFR, I/S/U parsing).
- Early modem / relay control groundwork.
### Notes
- Detailed per‑feature notes not tracked then; reconstructed from commit history.

