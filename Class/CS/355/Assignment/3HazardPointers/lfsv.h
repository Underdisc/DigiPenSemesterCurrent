/*****************************************************************************/
/*!
\file lfsv.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par DigiPen login: connor.deakin
\par Course: CS 355
\par Assignment: lfsv
\date 06/04/2018
\brief
  Contains an interface and implementation for a "lock-free" sorted vector.
*/
/*****************************************************************************/

#include <iostream>       // std::cout
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <deque>          // std::deque
#include <mutex>          // std::mutex
#include <utitity>
#include <cstring>

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

// stores vector pointer and size
// reference_counter stores how many threads are currently reading a value in
// the vector
struct Data
{
  int * pointer;
  int size;
  int reference_count;

  void Print()
  {
    char str[100];
    sprintf(str, "Size: %i\nCount: %i\n", size, reference_count);
    std::cout << str;
  }
}; //  __attribute__((aligned(16),packed)); // bug in GCC 4.*, fixed in 5.1?
// alignment needed to stop std::atomic<Data>::load to segfault

// so it is the scan function that will actually perform the
// deletion of pointers

// The scan
// we make a vector of hazard pointers
// adding every hazard pointer in our shared list into a single vector
// (make sure the hp is not null)
// sort the vector
// go through every element in threads retired list

// why am I using a map of unsigned to unsigned
// if we Add threads during execution - problems
// map aims to fix that
// We can use a lock too, but will refrain for now
// if the thread gets succcessfully added great, we
// have another thread, if not, return false and try again
class RetiredRecord
{
public:
  static unsigned AddThread(void);
  static std::vector<void *> * GetRetired(unsigned thread);
  static std::vector<std::vector<void *> > retired;
};

unsigned RetiredRecord::AddThread(unsigned thread_id)
{
  unsigned index = retired.size();
  retired.push_back(std::vector<void *>());
  return index;
}

std::vector<void *> * RetiredRecord::GetRetired(unsigned thread)
{
  return &(retired[thread]);
}

struct HazardPointerNode
{
  HazardPointerNode * next;
  void * hazard_pointer;
  std::atomic<bool> active;
}


// This leaks hard man
// just do a single threaded delete of all the hazard pointer nodes at the
// end
class HazardPointerRecord
{
  static std::atomic<HazardPointerNode *> head;

  static HazardPointerNode * Take()
  {
    // move through list until we find an inactive node
    HazardPointerNode * node = head;
    while(node)
    {
      if(node->active.load() || !node->active.compare_exchange_strong(0, 1))
      {
        node = node->next;
        continue;
      }
      return node;
    }
    // new node must be added for a new hazard pointer
    HazardPointerNode * new_node = new HazardPointerNode;
    new_node->hazard_pointer = nullptr;
    new_node->active.store(true);
    // we push the node to the front to garuntee that other threads have
    // not changed this list
    HazardPointerNode * old_head;
    do
    {
      old_head = head.load();
      new_node->next = old_head;
    } while(!head.compare_exchange_strong(old_head, new_node));
    // we could keep a length that we update, but why bother
  }

  static void Give(HazardPointerNode * old_node)
  {
    // we don't need to perform CAS on active since only
    // one thread will give the hazard pointer back
    old_node->hazard_pointer = nullptr;
    old_node->active.store(false);
  }
};

void DeleteRetiredPointers(std::)



// why not do some sort of multi pointer
// where every pointer has a refcount
// and when that ref count reaches zero
// it can be freed
// that's essentially hazard pointers

// each thread will hold it's own list of retire pointers
// figuring out how to get that in here might be a bit difficult

// So ok, now we need the rlist
// The constructor for lfsv won't work


// A "lock-free" sorted vector implementation
class LFSV
{
  //MemoryBank<std::vector<int>, 290000, sizeof(void*) > mb;
  MemoryBank<int, 2000, sizeof(int[10000])> mb2;
public:
  std::atomic<Data> data;

  LFSV() : mb2(), data(Data{mb2.Get(), 0, 0})
  {}

  ~LFSV()
  {
    Data temp = data.load();
    int* p = temp.pointer;
    if ( p != nullptr )
    {
      mb2.Store( p );
    }
  }

  void Retire(void * retired_address, unsigned retired_index)
  {
    std::vector<void *> * retired_vector;
    retired_vector = RetiredRecord::GetRetired(retired_index);
    retired_vector->push_back(retired_address);
    if(retired_vector.size() >= 10)
    {

    }
  }


  // Inserts a value into the vector. The vector will stay sorted in an
  // ascending order.
  void Insert(int const & v, unsigned retired_index)
  {
    // value of data will only change on when its reference_count is 0
    Data data_old;
    Data data_new;
    data_new.pointer = nullptr;
    data_new.size = 0;
    do
    {
      // get current pointer and size
      // reference_count needs to be set to zero everytime because data_old
      // might be modified by compare_exchange_strong
      data_old = data.load();
      data_old.reference_count = 0;
      // get new pointer for data
      if(data_new.pointer != nullptr)
      {
        mb2.Store(data_new.pointer);
      }
      data_new.pointer = mb2.Get();
      data_new.size = data_old.size;
      data_new.reference_count = 0;
      std::memcpy(data_new.pointer, data_old.pointer,
        data_new.size * sizeof(int));
      int * vector = data_new.pointer;
      // add new value to array sorted in ascending order
      vector[data_new.size] = v;
      // switch value's position to sort
      for (int i=0; i < data_new.size; ++i)
      {
        if (vector[i] > v )
        {
          for (int j = data_new.size; j > i; --j)
          {
            // move new element to proper position
            std::swap(vector[j], vector[j-1]);
          }
          break;
        }
      }
      // set new size
      ++data_new.size;
    } while(!data.compare_exchange_strong(data_old, data_new));
    // store unused pointer
    mb2.Store(data_old.pointer);
  }

  // access a value of pos within the vector
  int operator[] ( int pos ) {
    Data data_old;
    Data data_new;
    // increment reference_count
    do
    {
      data_old = data.load();
      data_new = data_old;
      ++data_new.reference_count;
    } while(!data.compare_exchange_strong(data_old, data_new));
    int ret_val = data.load().pointer[pos];
    // decrement reference_count
    do
    {
      data_old = data.load();
      data_new = data_old;
      --data_new.reference_count;
    } while(!data.compare_exchange_strong(data_old, data_new));
    return ret_val;
  }
};
