# GOES Configurator - Prototype UI

## Overview
Ini adalah prototype UI standalone dari GOES Configurator yang di-extract dari `src/web/web_page.h`. Prototype ini bisa dijalankan secara independen tanpa memerlukan ESP32 hardware untuk testing dan development UI.

## Features

### ✅ Prototype Mode Features:
- **Standalone Operation**: Tidak bergantung pada ESP32 API
- **Mock Data**: Data simulasi untuk testing UI functionality
- **Local Storage**: Persistent data menggunakan browser localStorage
- **Real-time Simulation**: Mock polling dan status updates
- **Visual Indicator**: Badge "Prototype Mode" untuk membedakan dari production

### ✅ Full UI Components:
- Device Configuration (Modem/Ethernet)
- IOA Configuration (Circuit Breakers, DI/DO, Analog)
- Modbus Configuration (Registers & Slaves)
- Live Status Panel
- Logs Viewer dengan filtering
- SOE (Sequence of Events) viewer
- All toggles and switches functionality

## Usage

### Quick Start:
```bash
# 1. Navigate to prototype directory
cd prototype

# 2. Open in browser (double-click or serve with local server)
open index.html

# Or serve with sync script:
./prototype_sync.sh serve

# Or serve with Python:
python3 -m http.server 8080
# Then open: http://localhost:8080
```

### Development Workflow:
1. **Edit UI**: Modify `index.html` untuk testing UI changes
2. **Test Features**: Gunakan mock data untuk validate functionality
3. **Extract Changes**: Copy successful changes back to `src/web/web_page.h`
4. **Deploy**: Upload firmware ke ESP32

### 🔄 Sync Script Commands:

```bash
# Navigate to prototype folder first
cd prototype

# Show help menu
./prototype_sync.sh

# Extract latest UI from production
./prototype_sync.sh extract

# Serve prototype locally
./prototype_sync.sh serve

# Open in browser
./prototype_sync.sh open

# Check status
./prototype_sync.sh status
```

## Mock Data
Prototype includes realistic mock data:
- **Device Config**: Modem settings, IEC104 parameters, features enabled
- **IOA Config**: Circuit breaker mappings, DI/DO assignments
- **Status Data**: Uptime, memory usage, communication stats
- **Logs**: Sample log entries dengan different categories

## Local Storage
Configuration changes disimpan di browser localStorage:
- `prototype_device_config`: Device configuration
- `prototype_ioa_config`: IOA mappings

## Differences from Production
- **No ESP32 Hardware**: Semua API calls di-mock
- **No Network Dependency**: Berjalan offline
- **Persistent Mock Data**: Config tersimpan di localStorage
- **Visual Prototype Badge**: Indicator mode prototype

## File Structure
```
prototype/
├── index.html              # Main prototype UI file
├── prototype_sync.sh       # Development sync script
└── README.md              # This documentation
```

## Development Notes

### Syncing with Production:
1. Extract HTML dari `src/web/web_page.h` dengan command:
   ```bash
   awk '/R"rawliteral\(/{flag=1; gsub(/.*R"rawliteral\(/,""); if(length($0)>0) print} flag && !/\)rawliteral"/{print} /\)rawliteral"/{gsub(/\)rawliteral".*/,""); if(length($0)>0) print; flag=0}' src/web/web_page.h > prototype/new_version.html
   ```

2. Merge changes manual dari prototype ke production file

### Mock API Endpoints:
- `/api/status` → MOCK_STATUS
- `/api/config2` → MOCK_CONFIG
- `/api/logs` → MOCK_LOGS
- `/api/config/device` → localStorage save
- `/api/config/ioa` → localStorage save

## Browser Compatibility
- ✅ Chrome/Edge/Safari (Modern browsers)
- ✅ Firefox
- ✅ Mobile browsers
- ⚠️ IE11+ (Limited support)

## Next Steps
1. Test UI changes in prototype
2. Validate functionality dengan mock data
3. Extract successful changes to production
4. Deploy to ESP32 hardware
