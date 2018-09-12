/*****************************************************************************/
/*!
\file lfsv.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par DigiPen login: connor.deakin
\par Course: CS 355
\par Assignment: HazardPointers
\date 26/04/2018
\brief
  Contains an interface and implementation for a "lock-free" sorted vector that
  makes use of hazard pointers in order to make the structure lock free.
*/
/*****************************************************************************/

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

//============================================================================//
// RetiredRecord //
//============================================================================//
/*//////////////////////////////////////////////////////////////////////////////
Holds and maintains all of the pointers that are considered retired from the
LFSV.
//////////////////////////////////////////////////////////////////////////////*/
class RetiredRecord
{
public:
  static unsigned AddThread(void);
  static std::vector<void *> * GetRetired(unsigned thread);
  static std::vector<std::vector<void *> > retired;
};

std::vector<std::vector<void *> > RetiredRecord::retired;

unsigned RetiredRecord::AddThread()
{
  unsigned index = retired.size();
  retired.push_back(std::vector<void *>());
  return index;
}

std::vector<void *> * RetiredRecord::GetRetired(unsigned thread)
{
  return &(retired[thread]);
}

//============================================================================//
// HazardPointerNode //
//============================================================================//
/*//////////////////////////////////////////////////////////////////////////////
A node that is used in the HazardPointerRecord. The HazardPointerRecord is a
linked list of HazardPointerNodes
//////////////////////////////////////////////////////////////////////////////*/
struct HazardPointerNode
{
  HazardPointerNode * next;
  void * hazard_pointer;
  std::atomic<bool> active;
};

//============================================================================//
// HazardPointerRecord //
//============================================================================//
/*//////////////////////////////////////////////////////////////////////////////
A linked list of HazardPointerNodes. Anytime a lookup is performed into the
LFSV, a HazardPointer node is acquired by the thread lookin up information
//////////////////////////////////////////////////////////////////////////////*/
class HazardPointerRecord
{
private:
  static std::atomic<HazardPointerNode *> head;
public:
  static HazardPointerNode * Head()
  {
    return head;
  }

  static void FreeNodes()
  {
    HazardPointerNode * node = head;
    while(node)
    {
      HazardPointerNode * next_node = node->next;
      delete node;
      node = next_node;
    }
  }

  static HazardPointerNode * Take()
  {
    // move through list until we find an inactive node
    HazardPointerNode * node = head;
    while(node)
    {
      bool old_val = false;
      bool new_val = true;
      if(node->active.load() ||
        !node->active.compare_exchange_strong(old_val, new_val))
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
    return new_node;
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

std::atomic<HazardPointerNode *> HazardPointerRecord::head;

//============================================================================//
// LFSV //
//============================================================================//
/*//////////////////////////////////////////////////////////////////////////////
A lock free sorted vector implementation that uses hazard pointers to provide
multithreaded access
//////////////////////////////////////////////////////////////////////////////*/
struct Data
{
  int * pointer;
  int size;
};

class LFSV
{
public:
  std::atomic<Data> data;

  LFSV() : data(Data{nullptr, 0})
  {}

  ~LFSV()
  {
    Data temp = data.load();
    int* p = temp.pointer;
    if ( p != nullptr )
    {
      delete [] p;
    }
  }

  void DeleteRetiredPointers(std::vector<void *> * retired_pointers)
  {
    // get all existing non null hazard pointers
    std::vector<void *> hazard_pointers;
    HazardPointerNode * current_node = HazardPointerRecord::Head();
    while(current_node)
    {
      void * hp = current_node->hazard_pointer;
      if(hp)
      {
        hazard_pointers.push_back(hp);
      }
      current_node = current_node->next;
    }
    // sort the hazard pointers that have been collected
    std::sort(hazard_pointers.begin(), hazard_pointers.end());
    // iterate through retire pointers
    // if a retired cannot be found among hazard pointes, we can delete it
    for(unsigned i = 0; i < retired_pointers->size(); ++i)
    {
      void * retired = (*retired_pointers)[i];
      bool found = std::binary_search(hazard_pointers.begin(),
        hazard_pointers.end(), retired);
      if(!found)
      {
        int * int_retired = (int *)retired;
        delete [] int_retired;
        (*retired_pointers)[i] = retired_pointers->back();
        retired_pointers->pop_back();
      }
    }
  }

  void Retire(void * retired_address, unsigned retired_index)
  {
    std::vector<void *> * retired_vector;
    retired_vector = RetiredRecord::GetRetired(retired_index);
    retired_vector->push_back(retired_address);
    if(retired_vector->size() >= 10)
    {
      DeleteRetiredPointers(retired_vector);
    }
  }

  // Inserts a value into the vector. The vector will stay sorted in an
  // ascending order.
  void Insert(int const & v, unsigned retired_index)
  {
    Data data_old;
    Data data_new;
    data_new.pointer = nullptr;
    data_new.size = 0;
    do
    {
      // get the current pointer and size
      data_old = data.load();
      // get new pointer for data
      if(data_new.pointer != nullptr)
      {
        delete [] data_new.pointer;
      }
      data_new.size = data_old.size + 1;
      data_new.pointer = new int[data_new.size];
      std::memcpy(data_new.pointer, data_old.pointer,
        data_old.size * sizeof(int));
      int * vector = data_new.pointer;
      // add new value to array sorted in ascending order
      vector[data_old.size] = v;
      for (int i = 0; i < data_old.size; ++i)
      {
        if (vector[i] > v )
        {
          for (int j = data_old.size; j > i; --j)
          {
            std::swap(vector[j], vector[j-1]);
          }
          break;
        }
      }
    } while(!data.compare_exchange_strong(data_old, data_new));
    // retired the old pointer
    Retire((void *)data_old.pointer, retired_index);
  }

  // access a value of pos within the vector
  int operator[] ( int pos ) {
    HazardPointerNode * node = HazardPointerRecord::Take();
    int * pointer;
    do {
      pointer = data.load().pointer;
      node->hazard_pointer = (void *)pointer;
    } while(pointer != data.load().pointer);
    int value = pointer[pos];
    HazardPointerRecord::Give(node);
    return value;
  }
};
