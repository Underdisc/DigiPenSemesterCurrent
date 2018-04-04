#include <iostream>       // std::cout
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <deque>          // std::deque
#include <mutex>          // std::mutex
#include <cstring>

// so this isn't lock free
// why can I use it if I want to make a lock free vector
template< typename T, int NUMBER, unsigned SIZE >
class MemoryBank {
    std::deque< T* > slots;
    std::mutex m;
    public:
        MemoryBank() : slots(NUMBER) {
            for ( int i=0; i<NUMBER; ++i ) {
                slots[i] = reinterpret_cast<T*>( new char[ SIZE ] );
            }
        }
        T* Get() {
            std::lock_guard<std::mutex> lock( m );
            T* p = slots[0];
            slots.pop_front();
            return p;
        }
        void Store( T* p ) {
            std::memset( p, 0, SIZE ); // clear data
            std::lock_guard<std::mutex> lock( m );
            slots.push_back( p );
        }
        ~MemoryBank() {
            for ( auto & el : slots ) { delete [] reinterpret_cast<char*>( el ); }
        }
};


struct Pair {
    int*    pointer;
    int     size;
}; //  __attribute__((aligned(16),packed)); // bug in GCC 4.*, fixed in 5.1?
// alignment needed to stop std::atomic<Pair>::load to segfault

class LFSV {
//    MemoryBank<std::vector<int>, 290000, sizeof(void*) > mb;
    MemoryBank<int, 2000, sizeof(int[10000]) > mb2;
    std::atomic< Pair > pdata;
    public:

    LFSV() : mb2(), pdata( Pair{ mb2.Get(), 0 } ) {
    }

    ~LFSV() {
        Pair temp = pdata.load();
        int* p = temp.pointer;
        if ( p != nullptr ) {
//            p->~vector();
            mb2.Store( p );
        }
    }

    void Insert( int const & v ) {
        Pair pdata_new, pdata_old;
        pdata_new.pointer  = nullptr;
        do {
            //delete pdata_new.pointer;
            if ( pdata_new.pointer != nullptr ) {
//                pdata_new.pointer->~vector();
                mb2.Store( pdata_new.pointer );
            }
            pdata_old = pdata.load();

            pdata_new.size = pdata_old.size;
//            pdata_new.pointer   = new (mb.Get()) std::vector<int>( *pdata_old.pointer );
            pdata_new.pointer   = mb2.Get();
            std::memcpy( pdata_new.pointer, pdata_old.pointer, pdata_new.size*sizeof(int) );

            int * a = pdata_new.pointer;
            a[pdata_new.size]=v; // add new value in the end

            for ( int i=0; i<pdata_new.size; ++i ) {
                if ( a[i]>=v ) {
                    for ( int j=pdata_new.size; j>i; --j ) {
                        std::swap( a[j], a[j-1] ); // move new element to proper position
                    }
                    break;
                }
            }
            ++pdata_new.size; // set new size
        } while ( !(this->pdata).compare_exchange_strong( pdata_old, pdata_new  ));
        //delete pdata_old.pointer;
        //pdata_old.pointer->~vector();
        mb2.Store( pdata_old.pointer );
    }

    int operator[] ( int pos ) { // not a const method anymore
        int ret_val = pdata.load().pointer[pos];
        return ret_val;
    }
};
