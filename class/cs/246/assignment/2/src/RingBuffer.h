// RingBuffer.h
// -- circular queue of fixed size
// cs 246 9/17

#ifndef CS246_RINGBUFFER_H
#define CS246_RINGBUFFER_H

#include <vector>


class RingBuffer {
  public:
    RingBuffer(int size);
    void put(float x);
    float get(int n);
  private:
    std::vector<float> buffer;
    int current_index;
};

#endif

