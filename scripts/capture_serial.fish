#!/usr/bin/env fish
# Capture serial output for a fixed duration (default 120s) into a timestamped logfile.
# Usage: ./scripts/capture_serial.fish [/dev/cu.SLAB_USBtoUART] [seconds]

set PORT (test -n "$argv[1]"; and echo $argv[1]; or echo /dev/cu.SLAB_USBtoUART)
set SECS (test -n "$argv[2]"; and echo $argv[2]; or echo 120)
set TS (date +%Y%m%d_%H%M%S)
set LOG serial_$TS.log

echo "[capture] Port: $PORT  Duration: $SECS s  Log: $LOG"

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
