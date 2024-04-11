#pragma once
#include <Arduino.h>

template <typename T, size_t N>
class RingBuffer
{
private:
    uint32_t head;
    T data[N];
public:
    RingBuffer() : head(0) {}
    T& operator[](size_t idx) { return data[(head + idx) % N]; }
    void push(T value)
    {
        data[head] = value;
        head = (head + 1) % N;
    }
};