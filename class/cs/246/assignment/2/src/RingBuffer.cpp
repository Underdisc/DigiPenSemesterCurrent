////////////////////////////////////////////////////////////////////////////////
// File:        RingBuffer.cpp
// Class:       CS246
// Asssignment: 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-09-24
////////////////////////////////////////////////////////////////////////////////

#include "RingBuffer.h"

RingBuffer::RingBuffer(int size) : buffer(size, 0), current_index(0)
{
}

void RingBuffer::put(float x)
{
    // Move the index forward through the ring buffer before setting the value.
    // If we go over the size of the buffer, we wrap back to the beginning.
    ++current_index;
    if(current_index >= (int)buffer.size())
    {
        current_index = 0;
    }
    buffer[current_index] = x;
}

float RingBuffer::get(int n)
{
    // If n is larger or equal to the buffer size, the value is redundant and 
    // can be reduced to a value below the buffer size while retaining the same
    // effect.
    if(n >= (int)buffer.size())
    {
        n %= (int)buffer.size();
    }

    // Find the index of the sample that will be returned and return the sample.
    int return_index = current_index;
    return_index -= n;
    if(return_index < 0)
    {
        return_index += (int)buffer.size();
    }
    return buffer[return_index];
}

