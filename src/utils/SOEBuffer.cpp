#include "SOEBuffer.h"
#include <vector>

namespace {
    struct RB { std::vector<SOEBuffer::Event> ev; size_t cap=0; size_t idx=0; bool full=false; bool of=false; uint32_t seq=0; } rb;
}

namespace SOEBuffer {
    void init(size_t capacity) { rb.cap = capacity?capacity:1; rb.ev.assign(rb.cap, {}); rb.idx=0; rb.full=false; rb.of=false; rb.seq=0; }
    void clear(){ if(!rb.cap)return; for(auto &e:rb.ev)e=Event{}; rb.idx=0; rb.full=false; rb.of=false; }
    void push(uint16_t ioa, int value){ if(!rb.cap)return; Event e; e.ms=millis(); e.ioa=ioa; e.value=(int16_t)value; e.seq=++rb.seq; rb.ev[rb.idx]=e; rb.idx=(rb.idx+1)%rb.cap; if(rb.full) rb.of=true; if(rb.idx==0) rb.full=true; }
    size_t count(){ return rb.full?rb.cap:rb.idx; }
    bool overflowed(){ return rb.of; }
    String toJSON(){ String out=F("{\"events\":["); bool first=true; size_t c=count(); if(c){ size_t start= rb.full?rb.idx:0; for(size_t i=0;i<c;i++){ const auto &e = rb.ev[(start+i)%rb.cap]; if(!first) out+=','; first=false; out+='{'; out+="\"ms\":"+String(e.ms); out+=" ,\"ioa\":"+String(e.ioa); out+=" ,\"v\":"+String(e.value); out+=" ,\"seq\":"+String(e.seq); out+='}'; } } out+=F("],\"count\":"); out+=String(c); out+=F(",\"cap\":"); out+=String(rb.cap); out+=F(",\"overflow\":"); out+=(rb.of?"true":"false"); out+='}'; return out; }
}
