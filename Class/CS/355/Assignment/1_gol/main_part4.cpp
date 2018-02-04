/*****************************************************************************/
/*!
\file main_part4.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par DigiPen login: connor.deakin
\par Course: CS 355
\par Assignment: 1 Game of Life
\date 02/02/2018
\brief
  Contains the implementation of the fourth part of the assignment. This
  implementation is identical to part 1, however, it tracks whether a specific
  cell in the grid has been modified in order to speed up the process of
  calculating the grid's next state.
*/
/*****************************************************************************/

#include <fstream>     /* ifstream */
#include <cstdio>      /* sscanf */
#include <iostream>    /* cout */
#include <stdexcept>   /* runtime_error */
#include <string>      /* string */
#include <string.h>    /* memset */
#include <utility>     /* pair */
#include <vector>      /* vector */
#include <pthread.h>   /* threads */
#include <semaphore.h> /* semaphores */

// Value of alive and dead cell
#define ALIVE 1
#define DEAD 0
#define NUM_NEIGHBOURS 8

// read file buffer size
#define FILE_BUFFSIZE 512
#define NUMBER_BUFFSIZE 10

typedef unsigned int uint;

//-----// GRID //-------------------------------------------------------------//

// A singleton class used for managing and updating cells within a game of life
// grid
class Grid
{
public:
  static void CreateInstance(const char * fname);
  static Grid * GetInstance();
  static void DestroyInstance();
  bool CanCellChange(int center);
  char CalculateCell(int center);
  void SetCell(int center, char value);
  char ValueAt(uint x, uint y) const;
  char & ValueAt(uint x, uint y);
  void PrintBuffer() const;
  void WriteBuffer(const char * fname) const;
  uint Size() const;
private:
  Grid(uint width, uint height);
  Grid(const char * fname);
  ~Grid();
  void Allocate();
  char * _buffer;
  // tracks whether cells are modified or not
  bool * _modified;
  std::vector<int *> _cellNeighbours;
  int _width;
  int _height;
  int _maxCol;
  int _maxRow;
  uint _size;
private:
  static Grid * _instance;
};

// static initializations
Grid * Grid::_instance = NULL;

// singleton instance functions
void Grid::CreateInstance(const char * fname)
{
  if(!_instance)
    _instance = new Grid(fname);
}

Grid * Grid::GetInstance()
{
  return _instance;
}

void Grid::DestroyInstance()
{
  if(_instance)
  {
    delete _instance;
    _instance = NULL;
  }
}

// Find out if this can change during the next grid iteration
bool Grid::CanCellChange(int center)
{
  int * neighbors = _cellNeighbours[center];
  for(uint i = 0; i < NUM_NEIGHBOURS; ++i)
  {
    if(_modified[neighbors[i]])
      return true;
  }
  return false;
}

char Grid::CalculateCell(int center)
{
  int * neighbours = _cellNeighbours[center];
  char neighbour_values[NUM_NEIGHBOURS];
  for(uint i = 0; i < NUM_NEIGHBOURS; ++i)
    neighbour_values[i] = _buffer[neighbours[i]];
  // finding how many neighbours are alive
  uint alive_neighbours = 0;
  for(uint i = 0; i < NUM_NEIGHBOURS; ++i)
  {
    if(neighbour_values[i])
      ++alive_neighbours;
  }
  // find out if the cell should be alive or dead in the next state
  char current_state = _buffer[center];
  char next_state;
  if(_buffer[center] == ALIVE)
  {
    if(alive_neighbours < 2 || alive_neighbours > 3)
      next_state = DEAD;
    else
      next_state = ALIVE;
  }
  else
  {
    if(alive_neighbours == 3)
      next_state = ALIVE;
    else
      next_state = DEAD;
  }
  // update modified buffer if grid value changed
  // It is assumed that set cell will be used in conjunction with this
  if(current_state == next_state)
    _modified[center] = false;
  else
    _modified[center] = true;
  return next_state;
}

void Grid::SetCell(int center, char value)
{
  _buffer[center] = value;
}

// get the value at an (x, y) coordinate
// TODO: Create char * array so these can be accessed without math opaerations
// only used for file io currently
inline char Grid::ValueAt(uint x, uint y) const
{
  uint index = y * _width + x;
  return _buffer[index];
}

inline char & Grid::ValueAt(uint x, uint y)
{
  uint index = y * _width + x;
  return _buffer[index];
}

// print the buffer to console
void Grid::PrintBuffer() const
{
  for(uint i = 0; i < _size; ++i)
  {
    if(i % _width == 0 && i != 0)
      std::cout << std::endl;
    if (_buffer[i])
      std::cout << "1";
    else
      std::cout << "0";
    std::cout << " ";
  }
  std::cout << std::endl;
}

// Write buffer to file
void Grid::WriteBuffer(const char * fname) const
{
  std::ofstream out_file;
  out_file.open(fname, std::ofstream::out);
  if(!out_file.is_open())
  {
    std::string error("Failed to open: ");
    error.append(fname);
    throw std::runtime_error(error);
  }
  for ( int i=0; i<_width; ++i )
  {
    for ( int j=0; j<_height; ++j )
    {
      out_file << (ValueAt(i, j)%2?"#":".") << " ";
    }
    out_file << std::endl;
  }
  out_file.close();
}

uint Grid::Size() const
{
  return _size;
}

// Grid constructors / destructors
Grid::Grid(uint width, uint height) : _width(width), _height(height),
  _maxCol(width - 1), _maxRow(height - 1), _size(width * height)
{
  Allocate();
}

Grid::Grid(const char * fname)
{
  std::ifstream in_file;
  in_file.open(fname, std::ifstream::in);
  if(!in_file.is_open())
  {
    std::string error("Failed to open file for writing: ");
    error.append(fname);
    throw std::runtime_error(error);
  }
  // reading in widthf and height
  in_file >> _width >> _height;
  _size = _width * _height;
  _maxCol = _width - 1;
  _maxRow = _height - 1;
  Allocate();
  // reading in which indicies start out as alive
  while(!in_file.eof())
  {
    int x, y;
    in_file >> x >> y;
    uint index = y * _width + x;
    _buffer[index] = 1;
    _modified[index] = true;
  }
}

// free buffers
Grid::~Grid()
{
  delete [] _buffer;
  delete [] _modified;
  for(uint i = 0; i < _cellNeighbours.size(); ++i)
    delete [] _cellNeighbours[i];
}

// allocate buffers
void Grid::Allocate()
{
  _buffer = new char[_size];
  _modified = new bool[_size];
  memset(_buffer, 0, _size);
  memset(_modified, 0, _size);
  // finding the indicies of neighbors for each cell
  for(int center = 0; center < static_cast<int>(_size); ++center)
  {
    // finding deltas for each direction
    int d_right, d_left, d_up, d_down;
    // calculating left / right index shifts
    int col = center % _width;
    if(col == 0)
    {
      // wraps on the left
      d_right = 1;
      d_left = _maxCol;
    }
    else if(col ==  _maxCol)
    {
      // wraps on the right
      d_right = -_maxCol;
      d_left = -1;
    }
    else
    {
      // no left / right wrapping
      d_right = 1;
      d_left = -1;
    }
    // calculating up / down index shifts
    int row = center / _width;
    if(row == 0)
    {
      // wraps above
      d_up = _maxRow * _width;
      d_down = _width;
    }
    else if(row == _maxRow)
    {
      // wraps below
      d_up = -_width;
      d_down = -(_width * _maxRow);
    }
    else
    {
      // no wrapping
      d_up = -_width;
      d_down = _width;
    }
    // finding indicies of cells around center
    // grid below shows which the indicies of neighbor_values
    // refers to in relation to the center index c.
    //*******/
    // 0 1 2
    // 3 c 4
    // 5 6 7
    //*******/
    int * neighbouring_cells = new int[NUM_NEIGHBOURS];
    neighbouring_cells[0] = center + d_left + d_up;
    neighbouring_cells[1] = center + d_up;
    neighbouring_cells[2] = center + d_right + d_up;
    neighbouring_cells[3] = center + d_left;
    neighbouring_cells[4] = center + d_right;
    neighbouring_cells[5] = center + d_left + d_down;
    neighbouring_cells[6] = center + d_down;
    neighbouring_cells[7] = center + d_right + d_down;
    _cellNeighbours.push_back(neighbouring_cells);
  }
}

//-----// CANAL //------------------------------------------------------------//
// A canal is a way to make a group of threads (travelers) travel through a
// certain number of locks (barriers) before the reach the end of the canal.
// Each thread must be within a lock before they can move to the next.
// This will continue until all threads have been through all locks.
class Canal
{
public:
  // Create travelers when a traveler thread is created. This will synchronize
  // all threads so that they approach and wait at the same locks.
  class Traveler
  {
  public:
    Traveler();
  private:
    sem_t * _currentLock;
    uint * _threadsAtCurrentLock;
    friend class Canal;
  };
public:
  static void Initialize(uint total_threads, uint total_locks);
  static void Lock(Traveler * traveler);
  static bool AllLocksPassed();
  static bool GateStop();
  static bool GatePass();
  static void Purge();
private:
  // semaphores for lock control
  static sem_t _mutex;
  static sem_t _lockA;
  static sem_t _lockB;
  // tracks the number of threads at each lock
  static uint _threadsAtLockA;
  static uint _threadsAtLockB;
  // totals for tracking when locks are full and when the whole
  // canal has been passed
  static uint _totalThreads;
  static uint _totalLocks;
  static uint _locksComplete;
};

// static initializations
sem_t Canal::_mutex;
sem_t Canal::_lockA;
sem_t Canal::_lockB;
uint Canal::_threadsAtLockA;
uint Canal::_threadsAtLockB;
uint Canal::_totalThreads;
uint Canal::_totalLocks;
uint Canal::_locksComplete;

void Canal::Initialize(uint total_threads, uint total_locks)
{
  // prep canal
  sem_init(&_mutex, 0, 1);
  sem_init(&_lockA, 0, 0);
  sem_init(&_lockB, 0, 0);
  _threadsAtLockA = 0;
  _threadsAtLockB = 0;
  _totalThreads = total_threads;
  _totalLocks = total_locks;
  _locksComplete = 0;
}

void Canal::Lock(Traveler * traveler)
{
  // traveler entering lock
  sem_wait(&_mutex);
  ++(*traveler->_threadsAtCurrentLock);
  // once all travelers are in the lock, travelers can progress to the next
  if(*traveler->_threadsAtCurrentLock == _totalThreads)
  {
    ++_locksComplete;
    sem_post(traveler->_currentLock);
  }
  sem_post(&_mutex);
  // traveler waits here until final traveler reaches lock
  sem_wait(traveler->_currentLock);
  sem_post(traveler->_currentLock);
  // traveler exits lock
  sem_wait(&_mutex);
  --(*traveler->_threadsAtCurrentLock);
  // once all travelers exit lock, reset lock semaphore to 0 so it can be
  // used as the next lock
  if(*traveler->_threadsAtCurrentLock == 0)
    sem_wait(traveler->_currentLock);
  sem_post(&_mutex);
  // set the traveler's next lock that they will wait at
  if(traveler->_currentLock == &_lockA)
  {
    traveler->_currentLock = &_lockB;
    traveler->_threadsAtCurrentLock = &_threadsAtLockB;
  }
  else
  {
    traveler->_currentLock = &_lockA;
    traveler->_threadsAtCurrentLock = &_threadsAtLockA;
  }
}

bool Canal::AllLocksPassed()
{
  if(_locksComplete < _totalLocks)
    return false;
  return true;
}

void Canal::Purge()
{
  // purge all semaphores managing canal
  sem_destroy(&_mutex);
  sem_destroy(&_lockA);
  sem_destroy(&_lockB);
}

Canal::Traveler::Traveler()
{
  _currentLock = &Canal::_lockA;
  _threadsAtCurrentLock = &Canal::_threadsAtLockA;
}

//-----// GAME OF LIFE THREAD //----------------------------------------------//

// each thread will run this code
void * RunCell(void * cell_index)
{
  uint index = *reinterpret_cast<uint *>(cell_index);
  Grid * grid_instance = Grid::GetInstance();
  // the thread is a traveler in the canal
  Canal::Traveler traveler;
  while(!Canal::AllLocksPassed())
  {
    bool change_cell_value = grid_instance->CanCellChange(index);
    char cell_value;
    if(change_cell_value)
      cell_value = grid_instance->CalculateCell(index);
    // calculate next grid state
    // stop all threads from progressing so we can set the cell values
    Canal::Lock(&traveler);
    if(change_cell_value)
      grid_instance->SetCell(index, cell_value);
    // wait for all threads to update the gird
    Canal::Lock(&traveler);
  }
  return NULL;
}

//-----// GAME OF LIFE //-----------------------------------------------------//

// Contains thread id and the index that thread is responsible for
struct ThreadInfo
{
  // The thread's id
  pthread_t id;
  // The index the thread will update in the grid
  uint index;
};

void RunGameofLife(const char * read_file, unsigned iterations,
  const char * write_file)
{
  // create grid
  Grid::CreateInstance(read_file);
  uint num_threads = Grid::GetInstance()->Size();
  // intialize the canal
  // Each update needs to locks. One for read and one for write
  Canal::Initialize(num_threads, iterations * 2);
  // create space for thread ids and thread data
  std::vector<ThreadInfo> threads;
  threads.reserve(num_threads);
  // Begin creating all needed threads
  uint threads_left = num_threads;
  uint index = 0;
  while(threads_left)
  {
    // creating a new thread
    threads.push_back(ThreadInfo());
    ThreadInfo & new_thread = threads.back();
    new_thread.index = index;
    int error = pthread_create(&new_thread.id, NULL, RunCell,
      reinterpret_cast<void *>(&new_thread.index));
    if(error)
      throw std::runtime_error("A thread was not created.");
    // for next thread
    --threads_left;
    ++index;
  }
  // wait for all threads to finish
  for(uint t = 0; t < num_threads; ++t)
  {
    // return value is not used
    void * return_value;
    int error = pthread_join(threads[t].id, &return_value);
    if(error)
      throw std::runtime_error("A thread failed to join");
  }
  // write buffer to file and finish
  Grid::GetInstance()->WriteBuffer(write_file);
  Grid::DestroyInstance();
}

//-----// MAIN //-------------------------------------------------------------//
int main( int argc, char ** argv )
{
    if ( argc != 4 ) {
        std::cout << "<- Expected 3 parameters -> " << std::endl
          << "1. Initial population file to read" << std::endl
          << "2. Number of iterations" << std::endl
          << "3. Final population file to write to" << std::endl;
        return 1;
    }
    char read_file[FILE_BUFFSIZE];
    int iterations = 0;
    char write_file[FILE_BUFFSIZE];
    std::sscanf(argv[1],"%s",read_file);
    std::sscanf(argv[2],"%i",&iterations);
    std::sscanf(argv[3],"%s",write_file);

    // Try running game of life with given parameters
    try
    {
      RunGameofLife(read_file, iterations, write_file);
    }
    catch(const std::runtime_error & error)
    {
      std::cout << error.what();
    }
}
