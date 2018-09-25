// RingBufferTest.cpp
// -- simple test of ring buffer class
// cs246 9/17

#include <iostream>
#include "RingBuffer.h"
using namespace std;


int main(void) {

  // size=8
  RingBuffer buffer(8);     // 0,0,0,0,0,0,0,0<

  // put 4 values
  buffer.put(6);            // 0,0,0,0,0,0,0,6<
  buffer.put(2);            // 0,0,0,0,0,0,6,2<
  buffer.put(3);            // 0,0,0,0,0,6,2,3<
  buffer.put(1);            // 0,0,0,0,6,2,3,1<
  // delay of 3 samples     //         3 2 1 0
  float x = buffer.get(3);
  cout << x << endl;

  // put a new sample
  buffer.put(7);            // 0,0,0,6,2,3,1,7<
  // delay of 2 samples     //           2 1 0
  x = buffer.get(2);
  cout << x << endl;

  // put new samples (force wrap)
  buffer.put(5);            // 0,0,6,2,3,1,7,5<
  buffer.put(4);            // 0,6,2,3,1,7,5,4<
  buffer.put(9);            // 6,2,3,1,7,5,4,9<
  buffer.put(8);            // 2,3,1,7,5,4,9,8<
  // delay of 5 samples     //     5 4 3 2 1 0
  x = buffer.get(5);
  cout << x << endl;

  return 0;
}

