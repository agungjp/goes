#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H
#include <Arduino.h>

// Simple ring buffer to expose recent logs via web API.
// Stores last N lines (truncated) to minimize RAM.
namespace LogBuffer {
    void init(size_t capacity = 100, size_t maxLineLen = 160);
    void append(const char *line);           // append a full line (will copy & truncate)
    void appendf(const char *fmt, ...);      // formatted append (auto newline removed)
    String toJSON();                         // return JSON array string: {"lines":["..",...]}
    bool isEnabled();                        // runtime flag (feat_web_log)
    void setEnabled(bool en);
}

// Helper macro to mirror to Serial and log buffer if enabled.
#define LOGF(fmt, ...) do { \
    Serial.printf(fmt, ##__VA_ARGS__); \
    if (LogBuffer::isEnabled()) LogBuffer::appendf(fmt, ##__VA_ARGS__); \
} while(0)

#endif // LOG_BUFFER_H
