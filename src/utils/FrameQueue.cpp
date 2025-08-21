#include "utils/FrameQueue.h"

#ifdef ARDUINO
FrameQueue::FrameQueue() : _head(0), _tail(0), _count(0) {}

bool FrameQueue::enqueue(const byte* frame, byte length) {
    if (isFull() || length > FRAME_SIZE) {
        return false;
    }
    memcpy(_queue[_tail], frame, length);
    _lengths[_tail] = length;
    _tail = (_tail + 1) % QUEUE_SIZE;
    _count++;
    return true;
}

bool FrameQueue::dequeue(byte* frame, byte& length) {
    if (isEmpty()) {
        return false;
    }
    length = _lengths[_head];
    memcpy(frame, _queue[_head], length);
    _head = (_head + 1) % QUEUE_SIZE;
    _count--;
    return true;
}

bool FrameQueue::isEmpty() const {
    return _count == 0;
}

bool FrameQueue::isFull() const {
    return _count == QUEUE_SIZE;
}

#else // Native environment
FrameQueue::FrameQueue() {}

bool FrameQueue::enqueue(const byte* frame, byte length) {
    if (_queue.size() >= QUEUE_SIZE) {
        return false; // Queue is full
    }
    _queue.push_back(std::vector<byte>(frame, frame + length));
    return true;
}

bool FrameQueue::dequeue(byte* frame, byte& length) {
    if (_queue.empty()) {
        return false;
    }
    std::vector<byte> front_frame = _queue.front();
    _queue.erase(_queue.begin());
    length = front_frame.size();
    memcpy(frame, front_frame.data(), length);
    return true;
}

bool FrameQueue::isEmpty() const {
    return _queue.empty();
}

bool FrameQueue::isFull() const {
    return _queue.size() >= QUEUE_SIZE;
}
#endif
