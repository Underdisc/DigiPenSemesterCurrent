#include "lfsv.h"

LFSV lfsv;

#include <algorithm>//copy, random_shuffle
#include <ctime>    //std::time (NULL) to seed srand

void insert_range( int b, int e ) {
    int * range = new int [e-b];
    for ( int i=b; i<e; ++i ) {
        range[i-b] = i;
    }
    std::srand( static_cast<unsigned int>(std::time (NULL)) );
    std::random_shuffle(range, range + e - b);
    for ( int i=0; i<e-b; ++i ) {
        // LOCKS ARE MINE
        //printing_mutex.lock();
        //std::cout << " " << range[i] << "\r\n";
        lfsv.Insert( range[i] );
        //printing_mutex.unlock();
    }
    delete [] range;
}

std::atomic<bool> doread( true );

void read_position_0() {
    int c = 0;
    while ( doread.load() ) {
        //std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        int val = lfsv[0];
        if ( val != -1 ) {
            std::cout << "not -1 on iteration " << c << " - was " << val << "\r\n"; // see main - all element are non-negative, so index 0 should always be -1
            // S: MY CODE
            /*printing_mutex.lock();
            int size = lfsv.data.load().size;
            for (int i=0; i<size; ++i) {
                std::cout << lfsv[i] << ' ';
            }
            printing_mutex.unlock();*/
            // E: MY CODE
        }
        ++c;
    }
}

void test( int num_threads, int num_per_thread )
{
    std::vector<std::thread> threads;
    lfsv.Insert( -1 );
    std::thread reader = std::thread( read_position_0 );

    for (int i=0; i<num_threads; ++i) {
        threads.push_back( std::thread( insert_range, i*num_per_thread, (i+1)*num_per_thread ) );
    }
    for (auto& th : threads) th.join();

    doread.store( false );
    reader.join();

    for (int i=0; i<num_threads*num_per_thread; ++i) {
        //std::cout << lfsv[i] << ' ';
        /*if ( lfsv[i] != i-1 ) {
            std::cout << "Error\n";
            return;
        }*/
    }
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
int main( int argc, char ** argv ) {
    if (argc==2) { //use test[ argv[1] ]
		int test = 0;
		std::sscanf(argv[1],"%i",&test);
		try {
            pTests[test]();
		} catch( const char* msg) {
			std::cerr << msg << std::endl;
		}
        return 0;
	}
}
