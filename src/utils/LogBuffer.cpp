#include "LogBuffer.h"
#include <vector>
#include <Arduino.h>

namespace {
    struct RB {
        size_t cap = 0;
        size_t maxLen = 0;
        std::vector<String> lines;
        size_t idx = 0;
        bool full = false;
        bool enabled = true; // controlled by feature flag
    } rb;
}

namespace LogBuffer {
    void init(size_t capacity, size_t maxLineLen) {
        rb.cap = capacity; rb.maxLen = maxLineLen; rb.lines.assign(capacity, String()); rb.idx = 0; rb.full = false; rb.enabled = true; }
    void setEnabled(bool en) { rb.enabled = en; }
    bool isEnabled() { return rb.enabled; }
    void append(const char *line) {
        if (!rb.cap || !rb.enabled) return;
        String s(line);
        if (s.length() > rb.maxLen) s.remove(rb.maxLen);
        rb.lines[rb.idx] = s;
        rb.idx = (rb.idx + 1) % rb.cap;
        if (rb.idx == 0) rb.full = true;
    }
    void appendf(const char *fmt, ...) {
        if (!rb.cap || !rb.enabled) return;
        char buf[256];
        va_list ap; va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        // Remove trailing newlines
        size_t len = strlen(buf);
        while (len && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = '\0';
        append(buf);
    }
    String toJSON() {
        String out = F("{\"lines\":[");
        bool first = true;
        auto emit = [&](const String &l){
            if (!first) out += ','; first=false; out += '\"';
            for (size_t i=0;i<l.length();++i){ char c=l[i]; if (c=='"') out += '\\"'; else if (c=='\\') out += "\\\\"; else if (c=='\n') out += "\\n"; else if (c=='\r') out += "\\r"; else out += c; }
            out += '\"';
        };
        if (rb.cap) {
            if (rb.full) {
                for (size_t i=0;i<rb.cap;i++) emit(rb.lines[(rb.idx + i) % rb.cap]);
            } else {
                for (size_t i=0;i<rb.idx;i++) emit(rb.lines[i]);
            }
        }
        out += F("]}");
        return out;
    }
}
