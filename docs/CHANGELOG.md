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

