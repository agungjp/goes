# GOES Scripts

Development and utility scripts for GOES RTU project.

## Contents

```
scripts/
├── capture_serial.fish     # Serial output capture tool
├── logs/                   # Generated log files (gitignored)
│   └── serial_*.log       # Timestamped serial captures
├── .gitignore             # Ignore log files in git
└── README.md              # This file
```

## Serial Capture Tool

### Quick Start
```bash
# Show help
./scripts/capture_serial.fish --help

# Test configuration (dry run)
./scripts/capture_serial.fish --dry-run

# Capture for 60 seconds
./scripts/capture_serial.fish 60

# Capture specific port for 30 seconds  
./scripts/capture_serial.fish /dev/cu.usbserial-0001 30
```

### Features
- ✅ Auto-detects USB serial ports
- ✅ Configurable capture duration
- ✅ Timestamped log files in `logs/` folder
- ✅ Dry-run mode for testing
- ✅ Comprehensive help documentation
- ✅ Clean termination with timeout

### Log Files
- **Location**: `scripts/logs/`
- **Format**: `serial_YYYYMMDD_HHMMSS.log`
- **Content**: Raw serial output from ESP32
- **Git**: Automatically ignored (.gitignore)

### Examples
```bash
# Auto-detect port, capture for 2 minutes (default)
./scripts/capture_serial.fish

# Capture for 30 seconds
./scripts/capture_serial.fish 30

# Use specific port, capture for 60 seconds
./scripts/capture_serial.fish /dev/cu.usbserial-0001 60

# Test without actually capturing
./scripts/capture_serial.fish --dry-run 10
```

### Dependencies
- Fish shell
- PlatformIO CLI
- ESP32 device connected via USB

### Troubleshooting

**Port not found?**
```bash
# List available ports
ls /dev/cu.*

# Use specific port
./scripts/capture_serial.fish /dev/cu.your-port-here
```

**Permission denied?**
```bash
# Make script executable
chmod +x ./scripts/capture_serial.fish
```

**Script hangs?**
```bash
# Use dry-run to test first
./scripts/capture_serial.fish --dry-run
```
