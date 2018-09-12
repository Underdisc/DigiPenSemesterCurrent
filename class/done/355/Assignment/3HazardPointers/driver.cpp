/*//////////////////////////////////////////////////////////////////////////////
Connor Deakin - connor.deakin
CS 355 Final Project - Hazard Pointers

Compile
g++ -o gcc0.exe -Wl,--enable-auto-import driver.cpp  -O3 -Wall -Wextra -std=c++11 -latomic -mcx16 -pthread
/*//////////////////////////////////////////////////////////////////////////////

#include "lfsv.h"

LFSV lfsv;

#include <algorithm>//copy, random_shuffle
#include <ctime>    //std::time (NULL) to seed srand

void insert_range(int b, int e, unsigned retired_index) {
    int * range = new int [e-b];
    for ( int i=b; i<e; ++i ) {
        range[i-b] = i;
    }
    std::srand( static_cast<unsigned int>(std::time (NULL)) );
    std::random_shuffle(range, range + e - b);
    for ( int i=0; i<e-b; ++i ) {
        lfsv.Insert(range[i], retired_index);
    }
    delete [] range;
}

std::atomic<bool> doread( true );

void read_position_0() {
    int c = 0;
    while (doread.load())
    {
        int val = lfsv[0];
        // I removed the sleep here to continuously test the lock free
        // vector from the main thread
        if ( val != -1 ) {
            // see main - all element are non-negative,
            // so index 0 should always be -1
            std::cout << "not -1 on iteration " << c
              << " - was " << val << "\r\n";
        }
        ++c;
    }
}

void test( int num_threads, int num_per_thread )
{
    // prepare retired vectors
    // A vector contain retired indicies isn't necessarily needed
    std::vector<unsigned> retired_indices;
    // thread 0: main thread
    // thread 1: reader thread
    // thread [2, total_threads): writer threads
    int total_threads = num_threads + 2;
    for(int i = 0; i < total_threads; ++i)
    {
      retired_indices.push_back(RetiredRecord::AddThread());
    }
    // insert using the main thread
    lfsv.Insert(-1, 0);
    // kick off reader thread
    std::thread reader = std::thread(read_position_0);
    // kick off writer threads
    std::vector<std::thread> threads;
    for (int i = 2; i < total_threads; ++i) {
        int start = (i - 2) * num_per_thread;
        int end = start + num_per_thread;
        threads.push_back(std::thread(insert_range, start, end,
          retired_indices[i]));
    }
    // Join threads
    for (auto& th : threads) th.join();
    doread.store( false );
    reader.join();
    // check for error
    for (int i = 0; i < num_threads * num_per_thread; ++i) {
        if ( lfsv[i] != i-1 ) {
            std::cout << "Error\n";
            return;
        }
    }
    HazardPointerRecord::FreeNodes();
    std::cout << "All good\r\n";
}

void test0() { test( 1, 100 ); }
void test1() { test( 2, 100 ); }
void test2() { test( 8, 500 ); }
void test3() { test( 100, 100 ); }

void (*pTests[])() = {
    test0,test1,test2,test3//,test4,test5,test6,test7
};

#include <cstdio>    /* sscanf */
int main( int argc, char ** argv )
{
  if (argc==2)
  {
    int test = 0;
    sscanf(argv[1],"%i",&test);
    try
    {
      pTests[test]();
    }
    catch( const char* msg)
    {
      std::cerr << msg << std::endl;
    }
  }
  else
  {
    std::cout << "Include Test number as argument - ./gcc0.exe TestNumber\n";
  }
  return 0;
}
