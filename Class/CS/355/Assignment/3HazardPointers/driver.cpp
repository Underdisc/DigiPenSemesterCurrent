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
        //std::cout << " " << range[i] << "\r\n";
        lfsv.Insert(range[i], retired_index);
        //printing_mutex.unlock();
    }
    delete [] range;
}

std::atomic<bool> doread( true );

void read_position_0() {
    int c = 0;
    while (doread.load())
    {
        int val = lfsv[0];
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
    for (auto& th : threads) th.join();

    doread.store( false );
    reader.join();

    for (int i = 0; i < num_threads * num_per_thread; ++i) {
        //std::cout << lfsv[i] << ' ';
        if ( lfsv[i] != i-1 ) {
            std::cout << "Error\n";
            //return;
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

/*
Basically what's going to happen here
This driver is going to create a LFSV structer
It's then going to create a shit ton of threads and all of these threads are
going to try inserting into LFSV at the same time.

You're gaol is to make it work

with lock free shit
*/

#include <cstdio>    /* sscanf */
int main( int argc, char ** argv )
{
  if (argc==2)
  { //use test[ argv[1] ]
    int test = 0;
    // TODO: REPLACE WITH SCANF
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
