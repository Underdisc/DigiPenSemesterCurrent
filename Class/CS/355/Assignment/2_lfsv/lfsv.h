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


struct Data
{
    int * pointer;
    int size;
    int reference_count;
}; //  __attribute__((aligned(16),packed)); // bug in GCC 4.*, fixed in 5.1?
// alignment needed to stop std::atomic<Data>::load to segfault

class LFSV
{
    //MemoryBank<std::vector<int>, 290000, sizeof(void*) > mb;
    MemoryBank<int, 2000, sizeof(int[10000])> mb2;
    std::atomic<Data> data;
public:

    LFSV() : mb2(), data( Data{mb2.Get(), 0, 0} )
    {}

    ~LFSV()
    {
        Data temp = data.load();
        int* p = temp.pointer;
        if ( p != nullptr )
        {
            //p->~vector();
            mb2.Store( p );
        }
    }

    void Insert( int const & v )
    {
        Data data_old;
        Data data_new;

        // START -- MY IMPLEMENTATION
        data_old.size = data.load().size;
        data_old.reference_count = 1;
        data_new.pointer = nullptr;
        data_new.size = 0;
        data_new.reference_count = 1;

        int * previous_pointer = nullptr;
        do
        {
            data_old.pointer = data.load().pointer;
            if(previous_pointer != data_old.pointer)
            {
                if(data_new.pointer != nullptr)
                    mb2.Store(data_new.pointer)
                data_new.pointer = mb2.Get();
                data_new.size = data
                std::memcpy(data_new.pointer, data_old.pointer,
                    data_new.size*sizeof(int));

                int * a = data_new.pointer;
                // add new value in the end
                a[data_new.size] = v;
                // switch values position to put array in ascending order
                for (int i=0; i<data_new.size; ++i)
                {
                    if ( a[i]>=v )
                    {
                        for (int j = data_new.size; j > i; --j)
                        {
                            // move new element to proper position
                            std::swap( a[j], a[j-1] );
                        }
                        break;
                    }
                }
                // set new size
                ++data_new.size;
            }
        } while(!(this->data).compare_exchange_strong(data_old, data_new))
        mb2.Store(data_old.pointer)
        // END -- MY IMPLEMENTATION
        /*do
        {
            //delete data_new.pointer;
            if (data_new.pointer != nullptr)
            {
                //data_new.pointer->~vector();
                mb2.Store(data_new.pointer);
            }
            data_old = data.load();

            data_new.size = data_old.size;
            //data_new.pointer = new (mb.Get()) std::vector<int>( *data_old.pointer );
            data_new.pointer = mb2.Get();
            std::memcpy( data_new.pointer, data_old.pointer, data_new.size*sizeof(int));

            int * a = data_new.pointer;
            // add new value in the end
            a[data_new.size] = v;
            // switch values position to put array in ascending order
            for (int i=0; i<data_new.size; ++i)
            {
                if ( a[i]>=v )
                {
                    for (int j = data_new.size; j > i; --j)
                    {
                        // move new element to proper position
                        std::swap( a[j], a[j-1] );
                    }
                    break;
                }
            }
            // set new size
            ++data_new.size;
        } while ( !(this->data).compare_exchange_strong(data_old, data_new));
        //delete data_old.pointer;
        //data_old.pointer->~vector();
        mb2.Store( data_old.pointer );*/
    }

    int operator[] ( int pos ) { // not a const method anymore
        int ret_val = data.load().pointer[pos];
        return ret_val;
    }
};
