#!/usr/bin/env fish
# Capture serial output for a fixed duration (default 120s) into a timestamped logfile.
# Usage: ./scripts/capture_serial.fish [port] [seconds]
# Usage: ./scripts/capture_serial.fish --help
# Usage: ./scripts/capture_serial.fish --dry-run [port] [seconds]
# Auto-detects USB serial port if not specified

# Check for help or dry-run flags
if test "$argv[1]" = "--help" -o "$argv[1]" = "-h"
    echo "GOES Serial Capture Tool"
    echo "========================"
    echo "Captures ESP32 serial output to timestamped log files"
    echo ""
    echo "Usage:"
    echo "  ./scripts/capture_serial.fish [port] [seconds]"
    echo "  ./scripts/capture_serial.fish --dry-run [port] [seconds]"
    echo "  ./scripts/capture_serial.fish --help"
    echo ""
    echo "Parameters:"
    echo "  port     Serial port (auto-detected if not specified)"
    echo "  seconds  Capture duration in seconds (default: 120)"
    echo ""
    echo "Output:"
    echo "  Log files saved to: scripts/logs/serial_YYYYMMDD_HHMMSS.log"
    echo ""
    echo "Examples:"
    echo "  ./scripts/capture_serial.fish                    # Auto-detect port, 120s"
    echo "  ./scripts/capture_serial.fish 60                 # Auto-detect port, 60s" 
    echo "  ./scripts/capture_serial.fish /dev/cu.usb* 30    # Specific port, 30s"
    echo "  ./scripts/capture_serial.fish --dry-run          # Test without capturing"
    exit 0
end

set DRY_RUN false
if test "$argv[1]" = "--dry-run"
    set DRY_RUN true
    set argv $argv[2..-1]  # Remove --dry-run from arguments
end

# Auto-detect USB serial port if not provided
if test -z "$argv[1]"
    set available_ports (ls /dev/cu.usbserial* 2>/dev/null)
    if test (count $available_ports) -gt 0
        set PORT $available_ports[1]
        echo "[capture] Auto-detected port: $PORT"
    else
        set PORT /dev/cu.usbserial-0001
        echo "[capture] No USB ports found, using default: $PORT"
    end
else
    set PORT $argv[1]
end

set SECS (test -n "$argv[2]"; and echo $argv[2]; or echo 120)
set TS (date +%Y%m%d_%H%M%S)
set SCRIPT_DIR (dirname (realpath (status --current-filename)))
set LOG_DIR "$SCRIPT_DIR/logs"
set LOG "$LOG_DIR/serial_$TS.log"

# Ensure logs directory exists
mkdir -p "$LOG_DIR"

echo "[capture] Port: $PORT  Duration: $SECS s  Log: $LOG"

if test "$DRY_RUN" = "true"
    echo "[capture] DRY RUN MODE - No actual capture will be performed"
    echo "[capture] Would run: pio device monitor -e esp32dev-sim7600ce --port $PORT --baud 115200"
    echo "[capture] Log would be saved to: $LOG"
    echo "[capture] Available ports:"
    ls /dev/cu.* 2>/dev/null | grep -E "(usb|serial)" | head -5
    exit 0
end

# Metode 1: gunakan 'script' jika tersedia (lebih mudah stop)
if type -q script
	echo "[capture] Menggunakan utilitas 'script' untuk logging."
	script -q $LOG pio device monitor -e esp32dev-sim7600ce --port $PORT --baud 115200 &
	set MON_PID $last_pid
	sleep $SECS
	# kirim INT agar pio monitor wrap up
	kill -INT $MON_PID 2>/dev/null; or kill $MON_PID 2>/dev/null
	wait $MON_PID 2>/dev/null
else
	echo "[capture] Fallback ke pipeline tee (akan kirim sinyal ke process group)."
	# Jalankan pipeline dalam subshell supaya punya process group sendiri
	bash -c "pio device monitor -e esp32dev-sim7600ce --port $PORT --baud 115200" 2>&1 | tee $LOG &
	set PIPE_PID $last_pid
	# Ambil PGID dari proses tee (PGID = PID group leader dalam kasus ini)
	set PGID (ps -o pgid= -p $PIPE_PID | tr -d ' ')
	sleep $SECS
	# Kirim SIGINT ke seluruh process group (-PGID)
	if test -n "$PGID"
		kill -INT -$PGID 2>/dev/null; or kill -$PGID 2>/dev/null
	else
		kill -INT $PIPE_PID 2>/dev/null; or kill $PIPE_PID 2>/dev/null
	end
	wait $PIPE_PID 2>/dev/null
end

echo "[capture] Saved log -> $LOG"
# Quick summary: show first and last 15 lines
set head_lines (head -n 15 $LOG)
set tail_lines (tail -n 15 $LOG)
echo "----- BEGIN (first 15 lines) -----"
echo "$head_lines"
echo "----- END (last 15 lines) ------"
echo "$tail_lines"
